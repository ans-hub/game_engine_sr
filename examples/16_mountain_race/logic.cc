// *************************************************************
// File:    logic.cc
// Descr:   logic class for mountain race game
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "logic.h"

namespace anshub {

Logic::Logic(const Config& cfg, GlWindow& win, Level& level)
  : win_{win}
  , level_{level}
  , first_collision_{true}
{
  StartAudioEngineSound(cfg);
}

// Processes game frame

bool Logic::Process()
{
  auto kbtn = win_.ReadKeyboardBtn(BtnType::KB_DOWN);
  
  ProcessSystemInput(kbtn);
  ProcessTreesCollisions();

  level_.camman_.ProcessInput(win_);
  level_.player_.ProcessInput(win_);
  level_.player_.ProcessMovement(level_.terrain_);
  level_.follow_cam_.FollowFor(level_.player_);

  ProcessPlayerSounds();
  level_.rain_.Process(level_.player_.world_pos_);
  level_.terrain_.ProcessDetalization(level_.camman_.GetCurrentCamera().vrp_);
  
  return kbtn != Btn::ESC;
}

// Starts in-game sound

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

// Processes system input

void Logic::ProcessSystemInput(Btn kbtn)
{
  helpers::HandlePause(kbtn, win_);
}

// Processes collisions with player and trees

void Logic::ProcessTreesCollisions()
{
  auto collisions = level_.bvh_tree_.FindCollision(level_.player_);
  
  if (!collisions.empty())
  {
    auto& dyn = level_.player_.GetDynamics();
    if (dyn.GetCurrentSpeed() > dyn.GetMaxSpeed()/3 &&    // todo: magic
        !audio_helpers::IsNowPlaying(level_.audio_, level_.crash_snd_))
      level_.audio_.Play(level_.crash_snd_, false);
    if (first_collision_)
    {
      dyn.SetAcceleration(dyn.GetAcceleration() * -10.0f);
      first_collision_ = false;
    }
  }
  else
    first_collision_ = true;
}

// Processes player`s sounds (engine volume in depends of speed)

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