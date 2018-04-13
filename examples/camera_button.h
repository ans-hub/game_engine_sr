// *******************************************************************
// File:    camera_button.h
// Descr:   used to store info about camera buttons and its delays
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
// *******************************************************************

#ifndef GC_CAMERA_BUTTON_H
#define GC_CAMERA_BUTTON_H

#include "lib/window/enums.h"

namespace anshub {

// Helpers struct to store buttons info

struct CameraBtn
{
  explicit CameraBtn(KbdBtn btn, int init_delay = 0);
  bool Delay();
  void ResetDelay();
  KbdBtn btn_;

private:
  int curr_delay_;
  int init_delay_;

}; // struct CameraBtn

inline CameraBtn::CameraBtn(KbdBtn btn, int init_delay)
  : btn_{btn}
  , curr_delay_{0}
  , init_delay_{init_delay}
{ }

// Make delay by decrement curr_delay_ and returns true if delay is active

inline bool CameraBtn::Delay()
{
  if (curr_delay_ != 0)
    --curr_delay_;
  return static_cast<bool>(curr_delay_);
}

inline void CameraBtn::ResetDelay()
{
  curr_delay_ = init_delay_;
}

} // namespace anshub

#endif // GC_CAMERA_BUTTON_H