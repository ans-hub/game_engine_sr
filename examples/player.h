// *************************************************************
// File:    player.h
// Descr:   represents player in game
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *************************************************************

#ifndef GC_PLAYER_ENTITY_H
#define GC_PLAYER_ENTITY_H

#include "lib/draw/gl_object.h"
#include "lib/draw/gl_aliases.h"
#include "lib/draw/extras/terrain.h"
#include "lib/window/base_window.h"

#include "camera_operator.h"

namespace anshub {

// Performs to control an object

struct Player : public CameraOperator
{
  enum Action
  {
    TURN_LEFT,    TURN_RIGHT,
    LOOK_UP,      LOOK_DOWN,
    MOVE_FORWARD, MOVE_BACKWARD,
    MOVE_UP,      MOVE_DOWN,
    STRAFE_LEFT,  STRAFE_RIGHT,
    TILT_LEFT,    TILT_RIGHT,
    SPEED_UP
  
  }; // enum Action

  using KeyMap = std::map<Action, KbdBtn>;

  Player(
    GlObject&&, CameraOperator&, float player_h, float camera_h, cVector& dir);
  void SetButton(Action, KbdBtn);
  void ProcessInput(BaseWindow&, GlCamera&);

  GlObject    obj_;
  Vector      vrp_orig_;        // original vrp of camera (see note #1 after code)
  float       cam_height_;      // camera player offset
  Vector      dir_accel_;       // direction change acceleration
  KeyMap      keys_;            // controls

}; // struct Player

// Helpers function

namespace player {

  void InitCamera(Player&, GlCamera&, float z_off, float pitch);
  void ProcessCameraInput(Player&, CameraOperator&, BaseWindow&);
  void ProcessPlayerInput(Player&, GlCamera&, BaseWindow&);
  void ProcessGroundPosition(Player&, const Terrain&);
  void ProcessGroundDirection(Player&, GlCamera&, const Terrain&);
  void ProcessPlayerOrientation(Player&);
  void ProcessPlayerRotating(Player&);
  void ProcessCameraRotating(Player&, GlCamera&);
  
}  // namespace player

}  // namespace anshub

#endif // GC_PLAYER_ENTITY_H

// Note #1 : when camera is follow and object, the most difficult is to copy
// rotating of object and don`t break camera look position (i.e. when object
// rotates pitch, we may copy directions, but sometimes object would disappear
// from camera frustum due to vrp is not changed, and changed only direction.
// To solve this problem, we rotate camera vrp relative to object direction
// vector and get new vrp. But in next frame we need original vrp, not rotated,
// to perform free camera movement. So we have member variable vrp_orig_