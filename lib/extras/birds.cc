// *************************************************************
// File:    birds.cc
// Descr:   represents very simple birds
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "birds.h"

namespace anshub {

Bird::Bird(GlObject&& model, int init_delay, int world_width, float fly_height)
  : GlObject(std::move(model))
  , aim_{}
  , vel_{}
  , init_delay_{init_delay}
  , curr_delay_{init_delay}
  , world_width_{world_width}
  , fly_height_{fly_height}
{
  ResetPos();
  ResetAim();
  ResetVel();
}

void Bird::Process()
{
  world_pos_ += vel_;

  if (curr_delay_ != 0)
    --curr_delay_;
  else {
    ResetAim();
    ResetVel();
    curr_delay_ = init_delay_;
  }
}

void Bird::ResetPos()
{
  world_pos_.x = rand_toolkit::get_rand(-world_width_, world_width_);
  world_pos_.y = fly_height_;
  world_pos_.z = rand_toolkit::get_rand(-world_width_, world_width_);
}

void Bird::ResetAim()
{
  aim_.x = rand_toolkit::get_rand(-world_width_, world_width_);
  aim_.y = fly_height_;
  aim_.z = rand_toolkit::get_rand(-world_width_, world_width_);
}

void Bird::ResetVel()
{
  vel_ = (aim_ - world_pos_) / init_delay_;
  vel_.Normalize();    
}

}  // namespace anshub