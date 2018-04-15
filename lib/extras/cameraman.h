// *******************************************************************
// File:    cameraman.h
// Descr:   used to manage camera movements and user input
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *******************************************************************

#ifndef GC_CAMERAMAN_H
#define GC_CAMERAMAN_H

#include "lib/extras/exceptions.h"
#include "lib/extras/inputer.h"
#include "lib/draw/gl_camera.h"
#include "lib/draw/gl_camera_uvn.h"
#include "lib/draw/gl_camera_fol.h"
#include "lib/draw/gl_enums.h"
#include "lib/window/enums.h"
#include "lib/window/base_window.h"
#include "lib/math/vector.h"
#include "lib/physics/dynamics.h"

namespace anshub {

//****************************************************************************
// Inputer enumerators - performs behavior of cameraman
//****************************************************************************

enum class CamAction
{
  TURN_LEFT,    TURN_RIGHT,
  LOOK_UP,      LOOK_DOWN,
  LOOK_LEFT,    LOOK_RIGHT,
  MOVE_FORWARD, MOVE_BACKWARD,
  MOVE_UP,      MOVE_DOWN,
  STRAFE_LEFT,  STRAFE_RIGHT,
  SPEED_UP,     SLOW_DOWN,
  SIT,          JUMP,
  ZOOM_IN,      ZOOM_OUT,
  WIRED,        ROLL_MODE,      // see note #1 after code
  SWITCH_TYPE,  FLY_MODE,
  BIFILTERING_MODE, MIPMAP_MODE,
  _count

}; // enum class CamAction

enum class CamState
{
  ROLL_MODE,    WIRED_MODE,
  FLY_MODE,     SPEED_UP_MODE,
  ON_GROUND,    BIFILTERING_MODE,
  MIPMAP_MODE,
  _count

}; // enum class State

enum class CamValue
{
  SPEED_UP, JUMP_HEIGHT,
  OPERATOR_HEIGHT, MOUSE_SENSITIVE,
  _count

}; // enum class CamValue

//****************************************************************************
// Camera`s constants
//****************************************************************************

namespace camera_const {

  const float kSpeedUpDefault {3.0f};
  const float kJumpDefault {3.0f};
  const float kOperatorHeightDefault {4.0f};
  const float kMouseSensitiveDefault {1.0f};

} // namespace camera_const

//****************************************************************************
// Camera operator - performs camera movements and other features
//****************************************************************************

struct CameraMan : public Inputer<CamAction, CamState, CamValue>
{
  using Btn = CamAction;

  template<class ...Args> CameraMan(Args&&...);
  template<class T> auto& GetCamera(T);
  template<class T> void  UseCamera(T);
  void  SetDynamics(Dynamics&& dyn) { dyn_ = std::move(dyn); }
  auto& GetDynamics() const { return dyn_; }  
  auto& GetCurrentCamera() { return *cam_; }
  void  ProcessInput(const BaseWindow&) override;
  void  SetGroundPosition(float ypos);

protected:

  GlCamera    cam_eul_;
  CameraUvn   cam_uvn_;
  CameraFol   cam_fol_;
  GlCamera*   cam_;       // current camera
  Dynamics    dyn_;       // dynamics movements

  Pos prev_mouse_pos_;    // used to determine where was mouse in prev frame

}; // class CameraMan 

//****************************************************************************
// Inline member functions implementation
//****************************************************************************

// Constructor implementation

template<class ... Args>
inline CameraMan::CameraMan(Args&&... args)
  : cam_eul_{std::forward<Args>(args)...}
  , cam_uvn_{std::forward<Args>(args)...}
  , cam_fol_{std::forward<Args>(args)...}
  , cam_{&cam_eul_}
  , dyn_{}
  , prev_mouse_pos_{-1,-1}
{
  SetValue(CamValue::SPEED_UP, camera_const::kSpeedUpDefault);
  SetValue(CamValue::JUMP_HEIGHT, camera_const::kJumpDefault);
  SetValue(CamValue::OPERATOR_HEIGHT, camera_const::kOperatorHeightDefault);
  SetValue(CamValue::MOUSE_SENSITIVE, camera_const::kMouseSensitiveDefault);
}

// Camera`s getters implementation

template<>
inline auto& CameraMan::GetCamera<CamType::Euler>(CamType::Euler)
{ 
  return cam_eul_;
}

template<>
inline auto& CameraMan::GetCamera<CamType::Uvn>(CamType::Uvn)
{ 
  return cam_uvn_;
}

template<>
inline auto& CameraMan::GetCamera<CamType::Follow>(CamType::Follow)
{ 
  return cam_fol_;
}

template<>
inline void CameraMan::UseCamera<CamType::Euler>(CamType::Euler)
{ 
  cam_eul_.vrp_ = cam_->vrp_;
  cam_eul_.dir_ = cam_->dir_;
  cam_ = &cam_eul_;
  cam_->Preprocess();  
}

template<>
inline void CameraMan::UseCamera<CamType::Uvn>(CamType::Uvn)
{ 
  cam_uvn_.vrp_ = cam_->vrp_;
  cam_uvn_.dir_ = cam_->dir_;
  cam_uvn_.Reinitialize();
  cam_ = &cam_uvn_;
  cam_->Preprocess();  
}

template<>
inline void CameraMan::UseCamera<CamType::Follow>(CamType::Follow)
{ 
  cam_fol_.vrp_ = cam_->vrp_;
  cam_fol_.dir_ = cam_->dir_;
  cam_ = &cam_fol_;
  cam_->Preprocess();
}

} // namespace anshub

#endif // GC_CAMERAMAN_H

// Note #1 : roll_mode - if true, then cam rotate z axis, else x