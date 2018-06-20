// *************************************************************
// File:    level.cc
// Descr:   level class for mountain race game
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "level.h"

namespace anshub {

Level::Level(const Config& cfg)
  : trig_{}

  // Initializes audio objects

  , audio_{}
  , engine_snd_ {cfg.Get<std::string>("snd_engine")}
  , crash_snd_ {cfg.Get<std::string>("snd_crash")}
  , ambient_snd_ {cfg.Get<std::string>("snd_ambient")}
  , engine_mod_ {AudioFx::Modifier::PITCH}

  // Initializes gameplay objects

  , camman_{
      cfg.Get<float>("cam_fov"),
      cfg.Get<float>("cam_dov"),
      cfg.Get<int>("win_w"),
      cfg.Get<int>("win_h"),
      cfg.Get<Vector>("cam_pos"),
      cfg.Get<Vector>("cam_dir"),
      cfg.Get<float>("cam_nearz"),
      cfg.Get<float>("cam_farz"),
      trig_
  }
  , player_{
      object::Make(
          cfg.Get<std::string>("player_obj"),
          trig_,
          {2.0f, 2.0f, 2.0f},
          cfg.Get<Vector>("cam_pos"),
          {-90.0f, 0.0f, 0.0f}),
      2.0f,                         // todo: magic (player height)
      cfg.Get<Vector>("cam_pos"),
      {0.0f, 125.0f, 0.0f},         // todo: magic (player initial direction)
      trig_
  }
  , follow_cam_{camman_.GetCamera(CamType::Follow::type)}

  , skybox_{
      cfg.Get<std::string>("ter_sky").c_str(),
      cfg.Get<Vector>("cam_pos")
  }
  , terrain_{
      cfg.Get<std::string>("ter_hm").c_str(),
      cfg.Get<std::string>("ter_tx").c_str(),
      cfg.Get<float>("ter_divider"),
      cfg.Get<int>("ter_chunk"),
      static_cast<Shading>(cfg.Get<float>("ter_shading"))
  }
  , water_{
      terrain_.GetHmWidth(),
      cfg.Get<float>("ter_water_lvl"),
      color::fOceanBlue,
      Shading::GOURAUD
  }
  , trees_{
      cfg.Get<std::string>("ter_objs"),
      0.5f,                         //  todo: magic (scale)
      terrain_,
      trig_
  }
  , nature_{
      cfg.Get<std::string>("ter_objs"),
      0.5f,                         //  todo: magic (scale)
      terrain_,
      trig_
  }
  , rain_{
      cfg.Get<int>("ter_rain"),
      15.0f                         // todo: magic (blob width)
  }
  
  // Initializes renderer objects
  
  , lights_all_{}
  , lights_sky_{}
  , render_ctx_{
      cfg.Get<int>("win_w"),
      cfg.Get<int>("win_h"),
      color::Black                  // todo: magic (clear color)
  } 
  , bvh_tree_{
      cfg.Get<int>("ter_bvh_depth"),
      cfg.Get<float>("ter_world_size"),    
  }
{
  InitAudio(cfg);
  InitCamera(cfg);
  InitPlayer(cfg);
  InitFollowing(cfg);
  SetupSkybox(cfg);
  SetupTerrain(cfg);
  SetupTrees(cfg);
  SetupNature(cfg);
  
  SetupLights(cfg);
  SetupRenderContext(cfg);

  for (auto& tree : trees_.GetObjects())
    bvh_tree_.Insert(tree);

  for (auto& blob : rain_.GetObjects())
    object::Scale(blob, {2.0f, 1.0f, 2.0f});
}

// Initializes audio routines: loads audio class instance and loads music

void Level::InitAudio(const Config&)
{
  audio_.LoadFx(engine_snd_, true);
  audio_.Load(ambient_snd_, true);
  audio_.Load(crash_snd_, false);
}

// Initializes camera

void Level::InitCamera(const Config& cfg)
{
  const int kBtnDelay {20};

  camman_.SetButton(CamAction::STRAFE_LEFT, KbdBtn::A);
  camman_.SetButton(CamAction::STRAFE_RIGHT, KbdBtn::D);
  camman_.SetButton(CamAction::MOVE_FORWARD, KbdBtn::W);
  camman_.SetButton(CamAction::MOVE_BACKWARD, KbdBtn::S);
  camman_.SetButton(CamAction::MOVE_UP, KbdBtn::R);
  camman_.SetButton(CamAction::MOVE_DOWN, KbdBtn::F);
  camman_.SetButton(CamAction::JUMP, KbdBtn::SPACE);
  camman_.SetButton(CamAction::ZOOM_IN, KbdBtn::NUM9);
  camman_.SetButton(CamAction::ZOOM_OUT, KbdBtn::NUM0);
  camman_.SetButton(CamAction::WIRED, KbdBtn::T, kBtnDelay);
  camman_.SetButton(CamAction::SWITCH_TYPE, KbdBtn::ENTER, kBtnDelay);
  camman_.SetButton(CamAction::SPEED_UP, KbdBtn::LSHIFT);

  camman_.SetState(CamState::FLY_MODE, true);
  camman_.SetState(CamState::ON_GROUND, false);

  camman_.SetValue(CamValue::MOUSE_SENSITIVE, cfg.Get<float>("cam_mouse_sens"));
  camman_.SetValue(CamValue::OPERATOR_HEIGHT, cfg.Get<float>("cam_height"));
  camman_.SetValue(CamValue::SPEED_UP, cfg.Get<float>("cam_speed_up"));

  Dynamics dyn_camera {             // were 0.001, 0.9
    cfg.Get<float>("cam_accel"),
    cfg.Get<float>("cam_frict"),
    cfg.Get<float>("cam_gravity"),
    cfg.Get<float>("cam_max_speed")
  };
  camman_.SetDynamics(std::move(dyn_camera));  
}

// Initializes player

void Level::InitPlayer(const Config& cfg)
{
  player_.sphere_rad_ = 0.2f; // todo: magic (to prevent unnecessary collisions)
  
  // Set player controls

  player_.SetButton(ObjAction::TURN_LEFT, KbdBtn::LEFT);
  player_.SetButton(ObjAction::TURN_RIGHT, KbdBtn::RIGHT);
  player_.SetButton(ObjAction::LOOK_UP, KbdBtn::N);
  player_.SetButton(ObjAction::LOOK_DOWN, KbdBtn::M);
  player_.SetButton(ObjAction::MOVE_FORWARD, KbdBtn::UP);
  player_.SetButton(ObjAction::MOVE_BACKWARD, KbdBtn::DOWN);
  player_.SetButton(ObjAction::SPEED_UP, KbdBtn::LCTRL);

  player_.SetState(ObjState::FLY_MODE, false);
  player_.SetValue(ObjValue::SPEED_UP, cfg.Get<float>("cam_speed_up"));  

  // Player moving settings:
  // - amplitude of directions not reduced (1.0f)
  // - pitch angles -90..+90.. would be in range -45..+45 degrees
  // - roll angles -90..+90.. would be in range -30..+30 degrees
  // - yaw angles -90..+90.. would be in range -90..+90 degrees
  // - yaw velocity more than other since we rotate it by hands

  player_.SetDirection(Player::PITCH, 1.0f, 4.0f, -45.0f, 45.0f, false);
  player_.SetDirection(Player::ROLL, 1.0f, 4.0f, -30.0f, 30.0f, false);
  player_.SetDirection(Player::YAW, 1.0f, 6.0f, 0.0f, 0.0f, false);

  Dynamics dyn_player {
    cfg.Get<float>("cam_accel"),
    cfg.Get<float>("cam_frict"),
    cfg.Get<float>("cam_gravity"),
    cfg.Get<float>("cam_max_speed")
  };
  player_.SetDynamics(std::move(dyn_player));
}

// Initializes camera and player following

void Level::InitFollowing(const Config&)
{
  // Set initial camera position (by using orient_z vector)

  const Vector kDirOffset {6.5f, 0.0f, 0.0f};   // todo: magic
  const Vector kVrpOffset {0.0f, 0.5f, -0.6f};  // todo: magic

  // Follow camera settings:
  // - roll amplitude > than jeep for 20%, follow any angle
  // - pitch amplitude < than jeep for 20%, follow any angle
  // - yaw amplitude equal jeep angle
  // - velocity of directions are 2.0f

  camman_.UseCamera(CamType::Follow::type);
  follow_cam_.SetDirection(GlCamera::PITCH, 1.2f, 2.0f, 0.0f, 0.0f, false);
  follow_cam_.SetDirection(GlCamera::ROLL, 0.8f, 2.0f, 0.0f, 0.0f, false);
  follow_cam_.SetDirection(GlCamera::YAW, 1.0f, 2.0f,  0.0f, 0.0f, false);
  follow_cam_.FollowFor(player_, kVrpOffset, kDirOffset);
}

// Sets up skybox

void Level::SetupSkybox(const Config&)
{
  auto far_z = camman_.GetCurrentCamera().z_far_;
  object::Scale(skybox_, {far_z, far_z, far_z});
  object::Rotate(skybox_, {90.0f, 0.0f, 0.0f}, trig_);   // todo: magic
}

// Sets up terrain

void Level::SetupTerrain(const Config& cfg)
{
  terrain_.SetDetalization(cfg.Get<Config::V_Float>("ter_detaliz"));
}

// Sets up trees

void Level::SetupTrees(const Config&)
{
  Nature::ObjsList<NatureTypes> nature_list {
    { NatureTypes::TREE_T1, "../00_data/nature/tree_type_2.ply"} };
  trees_.SetObjects<NatureTypes>(nature_list);
  trees_.RecognizeObjects();

  for (auto& tree : trees_.GetObjects())
    tree.sphere_rad_ = 2.0f;  // todo: magic

  // Note - we make small radius to prevent unnecessary collisions, since
  // trees are tall hence bounding sphere in very big, and we will get
  // fantom collisions 
}

// Sets up other nature

void Level::SetupNature(const Config&)
{
  Nature::ObjsList<NatureTypes> nature_list {
    { NatureTypes::ROCK_T1, "../00_data/nature/rock_type_1.ply"},   // todo; magic
    { NatureTypes::GRASS_T1, "../00_data/nature/grass_type_2.ply"} };
  nature_.SetObjects<NatureTypes>(nature_list);
  nature_.RecognizeObjects();
}

// Sets up render context

void Level::SetupRenderContext(const Config& cfg)
{
  render_ctx_.is_zbuf_  = true;
  render_ctx_.is_wired_ = false;
  render_ctx_.is_alpha_ = true;
  render_ctx_.is_bifiltering_ = false;
  render_ctx_.is_mipmapping_ = true;
  render_ctx_.mipmap_dist_ = 240.0f;    // todo: magic
  render_ctx_.clarity_  = cfg.Get<float>("cam_clarity");
}

// Sets up lights

void Level::SetupLights(const Config& cfg)
{
  ColorTable color_table {};

  auto   color   = color_table[cfg.Get<std::string>("light_amb_color")];
  auto   intense = cfg.Get<float>("light_amb_int");
  Vector lpos {};
  Vector ldir {};

  lights_all_.AddAmbient(color, intense);

  color   = color_table[cfg.Get<std::string>("light_inf_color")];
  intense = cfg.Get<float>("light_inf_int");
  ldir    = cfg.Get<Vector>("light_inf_dir");

  if (intense)
    lights_all_.AddInfinite(color, intense, ldir);

  color   = color_table[cfg.Get<std::string>("light_pnt_color")];
  intense = cfg.Get<float>("light_pnt_int");
  ldir    = cfg.Get<Vector>("light_pnt_dir"); 
  lpos    = cfg.Get<Vector>("light_pnt_pos");

  if (intense)
    lights_all_.AddPoint(color, intense, lpos, ldir);

  color    = color_table[cfg.Get<std::string>("light_sky_color")];
  intense  = cfg.Get<float>("light_sky_int");
  
  lights_sky_.AddAmbient(color, intense);
}

} // namespace anshub