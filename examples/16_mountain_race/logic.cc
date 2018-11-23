// *************************************************************
// File:    logic.cc
// Descr:   logic class for mountain race game
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "logic.h"

namespace anshub {

Logic::Logic(const Config& cfg, GlWindow& win, Level& level)
  : win_{win}
  , level_{level}
  , timer_{}
  , ignore_input_{0} // todo: magic (1 sec)
  , mem_forward_{level_.player_.GetButton(ObjAction::MOVE_FORWARD)}
  , mem_backward_{level_.player_.GetButton(ObjAction::MOVE_BACKWARD)}
  , first_collision_{true}
{
  timer_.Start();
  StartAudioEngineSound(cfg);
}

bool Logic::Process()
{
  auto kbtn = win_.ReadKeyboardBtn(BtnType::KB_DOWN);
  
  ProcessSystemInput(kbtn);
  ProcessTreesCollisions();

  ProcessPlayer();
  ProcessIgnoreTime();

  ProcessPlayerSounds();
  level_.rain_.Process(level_.player_.world_pos_);
  level_.terrain_.ProcessDetalization(level_.camman_.GetCurrentCamera().vrp_);
  
  return kbtn != Btn::ESC;
}

void Logic::StartAudioEngineSound(const Config& cfg)
{
  auto& audio = level_.audio_;
  
  auto min_pitch = cfg.Get<float>("snd_low_pitch");
  auto max_pitch = cfg.Get<float>("snd_high_pitch");
  audio.SetModifierRange(level_.engine_snd_, level_.engine_mod_, min_pitch, max_pitch);
  audio.SetModifierValue(level_.engine_snd_, level_.engine_mod_, min_pitch);

  audio.PlayFx(level_.engine_snd_);   // use fx to change tempo and pitch
  audio.Play(level_.ambient_snd_);    // and regular play in other cases
}

void Logic::ProcessSystemInput(Btn kbtn)
{
  helpers::HandlePause(kbtn, win_);
}

void Logic::ProcessTreesCollisions()
{
  auto collisions = level_.bvh_tree_.FindCollision(level_.player_);
  
  if (!collisions.empty())
  {
    auto& dyn = level_.player_.GetDynamics();
    if (!audio_helpers::IsNowPlaying(level_.audio_, level_.crash_snd_))
      level_.audio_.Play(level_.crash_snd_, false);
    if (first_collision_)
    {
      ignore_input_ = 1000;   // todo: magic (1 sec)
      dyn.SetAcceleration(dyn.GetAcceleration() * -20.0f);  // todo: magic!!
      first_collision_ = false;
    }
  }
  else
    first_collision_ = true;
}

void Logic::ProcessPlayer()
{
  if (ignore_input_ <= 0)   // also we need backward ignoring
    level_.player_.SetButton(ObjAction::MOVE_FORWARD, mem_forward_);
  else
    level_.player_.SetButton(ObjAction::MOVE_FORWARD, KbdBtn::NONE);

  level_.camman_.ProcessInput(win_);
  level_.player_.ProcessInput(win_);
  level_.player_.ProcessMovement(level_.terrain_);
  level_.follow_cam_.FollowFor(level_.player_);
}

void Logic::ProcessIgnoreTime()
{
  timer_.End();
  if (ignore_input_ > 0)
    ignore_input_ -= timer_.GetElapsed();
  timer_.Start();
}

void Logic::ProcessPlayerSounds()
{
  auto& dyn = level_.player_.GetDynamics();
  auto range1 = std::make_pair(0.0f, dyn.GetMaxSpeed());
  auto range2 = level_.audio_.GetModifierRange(level_.engine_snd_, level_.engine_mod_);
  auto curr_vel = dyn.GetCurrentSpeed();
  auto modifier_val = math::LeadToRange(range1, range2, curr_vel);
  level_.audio_.SetModifierValue(level_.engine_snd_, level_.engine_mod_, modifier_val);
}

} // namespace anshub