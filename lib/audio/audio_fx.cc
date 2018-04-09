// *************************************************************
// File:    audio_fx.cc
// Descr:   wrapper to BASS_FX addon of BASS audio library
// Author:  Novoselov Anton @ 2018
// URL:     https://github.com/ans-hub/audio_out
// *************************************************************

#include "audio_fx.h"

namespace anshub {

// Constructs AudioOut with BassFx plugin capabilities

AudioFx::AudioFx() : AudioOut() { }

// Overrided destructor

AudioFx::~AudioFx()
{
  for (auto& stream : streams_)
    BASS_StreamFree(stream.second);
}

// Loads channel by filename, saves to streams_ vector and return its handle

AudioFx::Handle AudioFx::LoadFx(const FileName& fname, bool repeat)
{
  // Check if stream already loaded

  auto hndl = FindLoadedFx(fname);
  if (hndl)
    return hndl;
  
  // Create stream and store it in container

  auto flags = BASS_STREAM_DECODE;
  hndl = BASS_StreamCreateFile(FALSE, fname.c_str(), 0,0, flags);
  
  // Create tempo stream and free old stream

  if (hndl)
  {
    flags = BASS_FX_FREESOURCE;
    if (repeat)
      flags |= BASS_SAMPLE_LOOP;
    hndl = BASS_FX_TempoCreate(hndl, flags);
    streams_.push_back(std::make_pair(fname, hndl));
  }
  else
    audio_helpers::PrintBassError();

  return hndl;
}

// Plays the channel (with loading its before)

bool AudioFx::PlayFx(const std::string& fname, bool repeat)
{
  // Try to load stream (may be loaded already)

  auto hndl = LoadFx(fname, repeat);
  if (!hndl)
    return audio_helpers::PrintBassError();

  // Try to play stream

  if (BASS_ChannelPlay(hndl, FALSE))
    return true;
  else
    return audio_helpers::PrintBassError();
}

// Stops playing the channel in two ways - by immediately and by
// set loop flag to 0, therefore next loop is not come

bool AudioFx::StopFx(const std::string& fname)
{
  auto hndl = FindLoadedFx(fname);
  if (!hndl)
    return audio_helpers::PrintBassError();

  BASS_ChannelSetPosition(hndl, 0, BASS_POS_BYTE);
  return BASS_ChannelStop(hndl);
}

// Returns handle of sample if its were been loaded early

AudioFx::Handle AudioFx::FindLoadedFx(const std::string& fname) const
{
  for (auto& s : streams_)
    if (s.first == fname) return s.second;
  return 0;
}

// Changes tempo-pitch (where val is -60 .. +60 semitones as sayed in
// BASS_FX docs)

bool AudioFx::ChangeTempoPitch(const std::string& fname, int val)
{
  // Try to load channel

  auto hndl = FindLoadedFx(fname);
  if (!hndl)
    return audio_helpers::PrintBassError();    
  
  // Clamp val

  val = std::max(-60, val);
  val = std::min(+60, val);

  // Set val

  return BASS_ChannelSetAttribute(hndl, BASS_ATTRIB_TEMPO_PITCH, val);
}

}  // namespace anshub