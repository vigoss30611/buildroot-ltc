/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2016 Live Networks, Inc.  All rights reserved.
// A WAV audio file source
// NOTE: Samples are returned in little-endian order (the same order in which
// they were stored in the file).
// C++ header

#ifndef _PCM_LIVE_AUDIO_SOURCE_HH
#define _PCM_LIVE_AUDIO_SOURCE_HH

#ifndef _AUDIO_INPUT_DEVICE_HH
#include "AudioInputDevice.hh"
#endif

#include "Log.hh"
#include <fr/libfr.h>
#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>

typedef enum {
  WA_PCM = 0x01,
  WA_PCMA = 0x06,
  WA_PCMU = 0x07,
  WA_IMA_ADPCM = 0x11,
  WA_UNKNOWN
} WAV_AUDIO_FORMAT;

#define AUDIO_BUF_SIZE			1024

class PCMLiveAudioSource: public AudioInputDevice {
public:

  static PCMLiveAudioSource* createNew(UsageEnvironment& env,
					const char * chanID = "default_mic");

  unsigned char getAudioFormat();

protected:
  PCMLiveAudioSource(UsageEnvironment& env, const char * chanID);
	// called only by createNew()

  virtual ~PCMLiveAudioSource();


private:
  // redefined virtual functions:
  virtual void doGetNextFrame();
  virtual void doStopGettingFrames();
  virtual Boolean setInputPort(int portIndex);
  virtual double getAverageLevel() const;
  void closeAudioCodec();
  
private:
	
  unsigned char fAudioFormat;
  const char * fChanID;
  int fHandle;
  struct fr_buf_info	fFrBuf;
  void *f_audio_codec;
  struct codec_info f_audio_codec_info;
  struct codec_addr f_audio_codec_addr_info;
  uint64_t f_last_timestamp;
};

#endif
