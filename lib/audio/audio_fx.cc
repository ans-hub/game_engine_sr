// *************************************************************
// File:    audio_fx.cc
// Descr:   wrapper to BASS_FX (addon of BASS audio library)
// Author:  Novoselov Anton @ 2017
// *************************************************************

#include "audio_fx.h"

namespace anshub {

AudioFx::AudioFx() : AudioOut() { }

AudioFx::~AudioFx()
{
  for (auto& stream : streams_)
    if (stream.second.hndl_ != 0)
      BASS_StreamFree(stream.second.hndl_);
}

bool AudioFx::LoadFx(cString& fname, bool repeat)
{
  auto hndl = GetLoadedHandle_FX(fname);
 
  if (hndl)
    return audio_helpers::PrintGeneralError(fname + String("already loaded"));
  
  auto flags = BASS_STREAM_DECODE;
  hndl = BASS_StreamCreateFile(FALSE, fname.c_str(), 0,0, flags);
  
  if (hndl)
  {
    flags = BASS_FX_FREESOURCE;   // stream auto free
    if (repeat)
      flags |= BASS_SAMPLE_LOOP;
    hndl = BASS_FX_TempoCreate(hndl, flags);
    
    streams_[fname] = std::move(SoundInfo(hndl));
    return hndl;
  }
  else
    return audio_helpers::PrintBassError("BASS_StreamCreateFile");
}

bool AudioFx::PlayFx(cString& fname, bool repeat)
{
  auto hndl = GetLoadedHandle_FX(fname);

  if (!hndl && !LoadFx(fname, repeat))
      return false;

  if (BASS_ChannelPlay(hndl, FALSE))
    return true;
  else
    return audio_helpers::PrintBassError("BASS_ChannelPlay");
}

// Stops playing the channel in two ways - by immediately and by
// set loop flag to 0, therefore next loop is not come

bool AudioFx::StopFx(cString& fname)
{
  auto hndl = GetLoadedHandle_FX(fname);  
  if (!hndl)
    return audio_helpers::PrintGeneralError(fname + String(" not loaded"));

  BASS_ChannelSetPosition(hndl, 0, BASS_POS_BYTE);

  if (!BASS_ChannelStop(hndl))
    return audio_helpers::PrintBassError("BASS_ChannelStop");
  else
    return true;
}

bool AudioFx::SetModifierRange(cString& fname, Modifier m, float min, float max)
{
  auto info = streams_.find(fname);
  if (info == streams_.end())
    return audio_helpers::PrintGeneralError(fname + String(" not loaded"));
  
  assert(min >= audio_consts::kFxRanges.at(m).first);
  assert(max <= audio_consts::kFxRanges.at(m).second);
  assert(min <= max);

  info->second.ranges_[m] = std::make_pair(min, max);
  return true;
}

bool AudioFx::SetModifierValue(cString& fname, Modifier m, float val)
{
  assert(val >= audio_consts::kFxRanges.at(m).first);
  assert(val <= audio_consts::kFxRanges.at(m).second);

  auto hndl = GetLoadedHandle_FX(fname);
  if (!hndl)
    return audio_helpers::PrintGeneralError(fname + String(" not loaded"));    

  bool success {false};

  switch (m)
  {
    case Modifier::TEMPO :
      success = BASS_ChannelSetAttribute(hndl, BASS_ATTRIB_TEMPO, val);
      break;    
    case Modifier::PITCH : 
      success = BASS_ChannelSetAttribute(hndl, BASS_ATTRIB_TEMPO_PITCH, val);
      break;
    case Modifier::FREQ  : 
      success = BASS_ChannelSetAttribute(hndl, BASS_ATTRIB_TEMPO_FREQ, val);
      break;
    default    :
      return audio_helpers::PrintGeneralError("Modifier not recognized");          
  }

  if (!success)
    audio_helpers::PrintBassError("BASS_ChannelSetAttribute");
  return success;
}

AudioFx::P_Range AudioFx::GetModifierRange(const FileName& fname, Modifier m) const
{
  auto info = streams_.find(fname);
  if (info == streams_.end())
  {
    audio_helpers::PrintGeneralError(fname + String(" not loaded"));
    return std::make_pair(1.0f, 1.0f);
  }
  
  return info->second.ranges_.at(m);
}

AudioFx::Handle AudioFx::GetLoadedHandle_FX(const FileName& fname)
{
  auto found = streams_.find(fname);
  if (found != streams_.end())
    return found->second.hndl_;
  else
    return 0;
}

}  // namespace anshub