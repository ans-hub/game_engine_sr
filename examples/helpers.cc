// *************************************************************
// File:    helpers.cc
// Descr:   input-output helpers
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "helpers.h"

namespace anshub {

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

void helpers::HandleFullscreen(Btn key, int mode, GlWindow& win)
{
  if (key == Btn::ESC)
    win.ToggleFullscreen(mode);
}

void helpers::HandleObject(Btn key, Vector& vel, Vector& rot, Vector& scale)
{
  switch(key)
  {
    case Btn::UP :    vel.y +=  0.5f; break;
    case Btn::DOWN :  vel.y += -0.5f; break;
    case Btn::LEFT :  vel.x += -0.5f; break;
    case Btn::RIGHT : vel.x +=  0.5f; break;
    case Btn::Z :     rot.x -= 0.5f; break;
    case Btn::X :     rot.x += 0.5f; break;
    case Btn::C :     rot.y -= 0.5f; break;
    case Btn::V :     rot.y += 0.5f; break;
    case Btn::B :     rot.z -= 0.5f; break;
    case Btn::N :     rot.z += 0.5f; break;
    case Btn::NUM6 :  scale.x += 0.02;
                      scale.y += 0.02;
                      scale.z += 0.02; break;
    case Btn::NUM7 :  scale.x -= 0.02;
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

void helpers::PrintInfoOnScreen(
  GlText& text, FpsCounter& fps, 
  const Vector& obj_pos, const Vector& obj_rot, 
  const Vector& cam_pos, const Vector& cam_rot,
  int nfo_culled, int nfo_hidden)
{
  using vector::operator<<;
  std::ostringstream oss {};
  
  oss << "FPS: " << fps.ReadPrev()
      << ", culled: " << nfo_culled << ", hidden: " << nfo_hidden;
  text.PrintString(60, 90, oss.str().c_str());

  oss.str("");
  oss.clear();
  oss << "Obj pos: " << obj_pos << ", cam pos: " << cam_pos;
  text.PrintString(60, 70, oss.str().c_str());

  oss.str("");
  oss.clear();
  oss << "Obj rot: " << obj_rot << ", cam rot: " << cam_rot;
  text.PrintString(60, 50, oss.str().c_str());
  
  oss.str("");
  oss.clear();
  oss << "Controls: W,S - forward/backward, ARROWS - move, Z-N - rotate";
  text.PrintString(60, 30, oss.str().c_str());  
}

void helpers::PrintInfoOnCmd(FpsCounter& fps, int culled, int hidden, cVector& pos)
{
  if (fps.Ready())
  {
    using vector::operator<<;    
    std::cerr << "Frames per second: " << fps.ReadPrev() << '\n';
    std::cerr << "Camera pos: " << pos << '\n';
    std::cerr << "Objects culled: " << culled << '\n';
    std::cerr << "Hidden surface: " << hidden << "\n\n";
  }
}

void helpers::PrintFpsOnCmd(FpsCounter& fps)
{
  if (fps.Ready())
    std::cerr << "Frames per second: " << fps.ReadPrev() << '\n';
}

} // namespace anshub