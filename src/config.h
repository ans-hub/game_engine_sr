// *************************************************************
// File:    config.h
// Descr:   enumerators
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/game_console
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

  constexpr const char* kShotSnd = "resources/sounds/shot.mp3";
  constexpr const char* kExplodeSnd = "resources/sounds/explode.mp3";
  constexpr const char* kWingSnd = "resources/sounds/wing.mp3";

//****************************************************************************
// Game settings
//****************************************************************************

  constexpr int kSpaceColor = 0;
  constexpr int kStarsCnt = 500;
  constexpr int kStarTrack = 20;  // coeff using to eval star tracks
  constexpr int kVelocity = 43;      // viewport velocity
  constexpr int kMaxVelocity = 95;
  constexpr int kNearZ = 10;
  constexpr int kStarFarZ = 2000;
  constexpr int kShipFarZ = 20000;
  constexpr int kStarColor = (50<<24)|(50<<16)|(50<<8)|50;
  constexpr int kShipColor = (((70<<16)|(89<<8))|81);
  constexpr int kExplColor = (230<<16)|(230<<8)|230;
  constexpr int kCannonColor = ((241<<16)|(221<<8)|52)&0xffffff;
  constexpr int kAimColor = (88<<16)|(196<<8)|67;
  constexpr double kMinBrightness = 0.7;
  constexpr double kMaxBrightness = 2.5;
  constexpr int kFillColor = 0;
  constexpr int kCrossLen = 35;
  constexpr int kCrossVel = 5;
  constexpr int kCannonWait = 10;
  constexpr int kAudibleShip = 1600;
  constexpr int kDebrisCnt = 4;

} // namespace cfg

} // namespace anshub

#endif  // GAME_CONFIG_H