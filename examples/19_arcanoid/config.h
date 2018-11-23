// *************************************************************
// File:    config.h
// Descr:   enumerators
// Author:  Novoselov Anton @ 2018
// *************************************************************

#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

namespace anshub {

namespace cfg {

//****************************************************************************
// Music settings
//****************************************************************************

  constexpr const char* kBackgroundMusic = "resources/sounds/background.mp3";

//****************************************************************************
// Sounds settings
//****************************************************************************

  constexpr const char* kScratchSnd = "resources/sounds/scratch.mp3";
  constexpr const char* kClickSnd = "resources/sounds/click.wav";

//****************************************************************************
// Game settings
//****************************************************************************

namespace color {

  constexpr int kBlack = 0;
  constexpr int kWhite = (255<<24)|(255<<16)|(255<<8)|255;
  constexpr int kLightBlue = (54<<24)|(47<<16)|(37<<8)|255;
  constexpr int kDeepRed = (24<<24)|(122<<16)|(121<<8)|255;
  constexpr int kDeepGray = (50<<24)|(50<<16)|(50<<8)|50;
  constexpr int kDeepYellow = (0<<24)|(70<<16)|(89<<8)|81;
  constexpr int kBrightYellow = (0<<24)|(230<<16)|(230<<8)|230;
  constexpr int kMidOrange = (0<<24)|(88<<16)|(196<<8)|67;

}

  // Color settings

  constexpr int kBallColor = color::kBrightYellow;
  constexpr int kPaddleColor = color::kDeepRed;
  constexpr int kBlockColor = color::kBrightYellow;

} // namespace cfg

} // namespace anshub

#endif  // GAME_CONFIG_H