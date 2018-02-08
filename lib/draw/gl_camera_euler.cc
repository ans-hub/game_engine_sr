// *************************************************************
// File:    gl_camera_euler.cc
// Descr:   imitates euler camera for renderer
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#include "gl_camera_euler.h"

namespace anshub {

GlCameraEuler::GlCameraEuler
(
    float fov,
    float dov, 
    int scr_w, 
    int scr_h, 
    const Vector& pos, 
    const Vector& dir,
    float z_near,
    float z_far
)
  : trig_{}
  , fov_{fov}
  , dov_{dov}
  , wov_{2 * trig::CalcOppositeCatet(dov_, fov_/2, trig_)}
  , z_near_{z_near}
  , z_far_{z_far}
  , scr_w_{scr_w}
  , scr_h_{scr_h}
  , ar_{(float)scr_w_ / (float)scr_h_}
  , pos_{pos}
  , dir_{dir}
{

}

} // namespace anshub