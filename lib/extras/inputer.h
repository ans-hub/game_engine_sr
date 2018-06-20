// *******************************************************************
// File:    inputer.h
// Descr:   abstract base class for user input
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *******************************************************************

#ifndef GC_INPUTER_H
#define GC_INPUTER_H

#include <vector>

#include "lib/extras/exceptions.h"
#include "lib/draw/gl_enums.h"
#include "lib/window/enums.h"
#include "lib/window/base_window.h"
#include "lib/extras/camera_button.h"

namespace anshub {

//****************************************************************************
// Inputer - performs input for camera, objects, etc...
//****************************************************************************

template<class Action, class State, class Value>
struct Inputer
{
  Inputer();

  void   SetButton(Action, KbdBtn, int delay = 0);
  KbdBtn GetButton(Action);
  bool   GetState(State) const;
  void   SetState(State, bool value);
  float  GetValue(Value) const;
  void   SetValue(Value, float value);
  
  virtual void ProcessInput(const BaseWindow&) =0;

protected:
  using A_CamAction = std::vector<CameraBtn>;
  using A_CamState = std::vector<bool>;
  using A_CamValues = std::vector<float>;

  bool IsButtonPressed(const BaseWindow&, Action);

  A_CamAction keys_;      // see note #1 after code
  A_CamState  states_;
  A_CamValues values_;

}; // class Inputer 

//****************************************************************************
// Inline member functions implementation
//****************************************************************************

template<class Action, class State, class Value>
inline Inputer<Action,State,Value>::Inputer()
  : keys_(static_cast<int>(Action::_count))
  , states_(static_cast<int>(State::_count))
  , values_(static_cast<int>(Value::_count))
{ }

template<class A, class S, class V>
inline void Inputer<A,S,V>::SetButton(A action, KbdBtn btn, int delay)
{
  keys_[static_cast<int>(action)] = CameraBtn(btn, delay);
}

template<class A, class S, class V>
inline KbdBtn Inputer<A,S,V>::GetButton(A action)
{
  return keys_[static_cast<int>(action)].btn_;
}

template<class A, class S, class V>
inline bool Inputer<A,S,V>::IsButtonPressed(const BaseWindow& w, A action)
{
  auto& btn = keys_[static_cast<int>(action)];
  if (!btn.Delay() && w.IsKeyboardBtnPressed(btn.btn_))
  {
    btn.ResetDelay();
    return true;
  }
  return false;
}

template<class A, class S, class V>
inline void Inputer<A,S,V>::SetState(S s, bool val)
{
  states_[static_cast<int>(s)] = val;
} 

template<class A, class S, class V>
inline bool Inputer<A,S,V>::GetState(S s) const
{
  return states_[static_cast<int>(s)];
}

template<class A, class S, class V>
inline float Inputer<A,S,V>::GetValue(V v) const
{
  return values_[static_cast<int>(v)];
}

template<class A, class S, class V>
inline void Inputer<A,S,V>::SetValue(V v, float val)
{
  values_[static_cast<int>(v)] = val;
}

} // namespace anshub

#endif // GC_INPUTER_H

// Note #1 : we have array of keys, states and values of phys.obj. We use not
// map or unordered map, but std::array. Size of array is compile time and
// recieved from "_count" of each enum. The one case when this system would
// be broken if somewho explicit set "_count" value of enum to wrong value.
// In all other cases we have const access time and tiny interface of camera
// operator class