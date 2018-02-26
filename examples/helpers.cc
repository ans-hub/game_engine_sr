// *************************************************************
// File:    helpers.cc
// Descr:   input-output helpers
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "helpers.h"

namespace anshub {

void helpers::HandleCamType(Btn kbtn, GlCamera& cam)
{
  if (kbtn == Btn::ENTER)
  {
    if (cam.type_ == GlCamera::Type::EULER)
      cam.SwitchType(GlCamera::Type::UVN);
    else if (cam.type_ == GlCamera::Type::UVN)
      cam.SwitchType(GlCamera::Type::EULER);
  }
}

void helpers::HandleCamMovement(Btn kbtn, GlCamera& cam)
{
  float cam_vel = 0.5f;
  switch (kbtn)
  {
    case Btn::W :
    {
      if (cam.type_ == GlCamera::Type::EULER)
      {
        cam.vrp_.z += cam_vel * cam.trig_.Cos(cam.dir_.y);
        cam.vrp_.x += cam_vel * cam.trig_.Sin(cam.dir_.y);
      }
      else
        cam.vrp_.z += cam_vel;
    }
      break;
    case Btn::S :
    {
      if (cam.type_ == GlCamera::Type::EULER)
      {
        cam.vrp_.z -= cam_vel * cam.trig_.Cos(cam.dir_.y);
        cam.vrp_.x -= cam_vel * cam.trig_.Sin(cam.dir_.y);
      }
      else
        cam.vrp_.z -= cam_vel;
    }
      break;
    case Btn::A :
    {
      if (cam.type_ == GlCamera::Type::EULER)
      {
        cam.vrp_.x -= cam_vel * cam.trig_.Cos(cam.dir_.y);
        cam.vrp_.z += cam_vel * cam.trig_.Sin(cam.dir_.y);
      }
      else
        cam.vrp_.x -= cam_vel;
    }
      break;
    case Btn::D :
    {
      if (cam.type_ == GlCamera::Type::EULER)
      {
        cam.vrp_.x += cam_vel * cam.trig_.Cos(cam.dir_.y);
        cam.vrp_.z -= cam_vel * cam.trig_.Sin(cam.dir_.y);
      }
      else
        cam.vrp_.x += cam_vel;
    }
      break;
    case Btn::R : cam.vrp_.y += cam_vel; break;
    case Btn::F : cam.vrp_.y -= cam_vel; break;
    case Btn::NUM9 : cam.ChangeFov(cam.fov_-1); break;
    case Btn::NUM0 : cam.ChangeFov(cam.fov_+1); break;
    default     : break;
  }
}

void helpers::HandleCamRotate(
  bool mode, const Pos& mpos, Pos& mpos_prev, Vector& ang)
{
  if (mode)
    ang.z -= (mpos_prev.x - mpos.x) / 2;
  else
    ang.y -= (mpos_prev.x - mpos.x) / 2;
  ang.x -= (mpos_prev.y - mpos.y) / 2;  // todo: prevent gimbal lock
  mpos_prev = mpos;
}

void helpers::HandlePause(Btn key, GlWindow& win)
{
  if (key == Btn::P)
  {
    Timer timer {};
    timer.SetMillisecondsToWait(100);
    while (true)
    {
      if (win.ReadKeyboardBtn(BtnType::KB_DOWN) == Btn::P)
        break;
      else
        timer.Wait();
    }
  }
}

void helpers::HandleObject(Btn key, Vector& vel, Vector& rot, Vector& scale)
{
  switch(key)
  {
    case Btn::UP :    vel.y =  0.5f; break;
    case Btn::DOWN :  vel.y = -0.5f; break;
    case Btn::LEFT :  vel.x = -0.5f; break;
    case Btn::RIGHT : vel.x =  0.5f; break;
    case Btn::Z :     rot.x -= 0.5f; break;
    case Btn::X :     rot.x += 0.5f; break;
    case Btn::C :     rot.y -= 0.5f; break;
    case Btn::V :     rot.y += 0.5f; break;
    case Btn::B :     rot.z -= 0.5f; break;
    case Btn::N :     rot.z += 0.5f; break;
    case Btn::NUM1 :  scale.x += 0.02;
                      scale.y += 0.02;
                      scale.z += 0.02; break;
    case Btn::NUM2 :  scale.x -= 0.02;
                      scale.y -= 0.02;
                      scale.z -= 0.02; break;
    default : break;
  }
}

void helpers::HandleRotateObject(Btn key, Vector& rot)
{
  switch(key)
  {
    case Btn::Z :     rot.x -= 0.5f; break;
    case Btn::X :     rot.x += 0.5f; break;
    case Btn::C :     rot.y -= 0.5f; break;
    case Btn::V :     rot.y += 0.5f; break;
    case Btn::B :     rot.z -= 0.5f; break;
    case Btn::N :     rot.z += 0.5f; break;
    default : break;
  }
}

} // namespace anshub