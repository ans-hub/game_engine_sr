// *************************************************************
// File:    audio_fx.h
// Descr:   wrapper to BASS_FX addon of BASS audio library
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/audio_out
// *************************************************************

// BASS Library docs placed here: https://www.un4seen.com/doc/

#ifndef AUDIO_FX_H
#define AUDIO_FX_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <functional>

#include "audio_out.h"
#include "3rdparty/bass.h"
#include "3rdparty/bass_fx.h"

namespace anshub {

class AudioFx : public AudioOut
{
public:
  using Handle   = DWORD; 
  using V_Handle = std::vector<Handle>;
  using V_HandleStr = std::vector<std::pair<FileName, Handle>>;

  AudioFx();
  virtual ~AudioFx() override;
  Handle  LoadFx(const FileName&, bool repeat);
  bool    PlayFx(const FileName&, bool repeat = false);
  bool    StopFx(const FileName&);  
  Handle  FindLoadedFx(const FileName&) const;
  bool    ChangeTempoPitch(const FileName&, int val);

private:
  V_HandleStr streams_;    // loaded streams

}; // class AudioFx

}  // namespace anshub

#endif  // AUDIO_FX_H