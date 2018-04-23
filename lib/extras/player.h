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
#include "lib/draw/gl_camdir.h"

#include "lib/extras/inputer.h"
#include "lib/extras/terrain.h"

#include "lib/physics/dynamics.h"

#include "lib/window/base_window.h"

namespace anshub {

//****************************************************************************
// Inputer enumerators - performs behavior of player
//****************************************************************************

enum class ObjAction
{
  TURN_LEFT,    TURN_RIGHT,
  MOVE_FORWARD, MOVE_BACKWARD,
  STRAFE_LEFT,  STRAFE_RIGHT,
  LOOK_UP,      LOOK_DOWN,
  ROLL_LEFT,    ROLL_RIGHT,
  SPEED_UP,     SLOW_DOWN,
  _count

}; // enum class ObjAction

enum class ObjState
{
  FLY_MODE,     SPEED_UP_MODE,
  ON_GROUND,
  _count

}; // enum class ObjState

enum class ObjValue
{
  PLAYER_HEIGHT, SPEED_UP,
  _count

}; // enum class ObjValue

//****************************************************************************
// Player object - performs player movements and other features
//****************************************************************************

struct Player
  : public Inputer<ObjAction, ObjState, ObjValue>
  , public GlObject
{
  enum DirectionType { YAW, PITCH, ROLL };

  using Btn = ObjAction;
  
  Player(GlObject&&, float player_h, cVector& dir, cVector& pos, cTrigTable&);
  void  SetDynamics(Dynamics&& dyn) { dyn_ = std::move(dyn); }
  auto& GetDynamics() const { return dyn_; }
  void  ProcessInput(const BaseWindow&) override;
  void  ProcessMovement(const Terrain&);

  template<class ... Args>
  void SetDirection(DirectionType, Args&&...);

// private:
  Dynamics  dyn_;
  CamDir    yaw_;
  CamDir    pitch_;
  CamDir    roll_;
  TrigTable trig_;

  void SetGroundPosition(float ypos);
  void ProcessGroundPosition(const Terrain&);
  void ProcessGroundDirection(const Terrain&);
  void ProcessPlayerOrientation();
  void ProcessPlayerRotating();

}; // struct Player

//****************************************************************************
// Inline implementation
//****************************************************************************
    
template<class ... Args>
inline void Player::SetDirection(DirectionType type, Args&& ...args)
{
  switch(type)
  {
    case Player::DirectionType::YAW :
      yaw_ = CamDir(std::forward<Args>(args)...);
      break;
    case Player::DirectionType::PITCH:
      pitch_ = CamDir(std::forward<Args>(args)...);
      break;
    case Player::DirectionType::ROLL :
      roll_ = CamDir(std::forward<Args>(args)...);
      break;
    default: break;
  }
}

}  // namespace anshub

#endif // GC_PLAYER_ENTITY_H