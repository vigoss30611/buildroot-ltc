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

#ifndef _PCM_LIVE_FILE_SOURCE_HH
#define _PCM_LIVE_FILE_SOURCE_HH

#ifndef _AUDIO_INPUT_DEVICE_HH
#include "AudioInputDevice.hh"
#endif

#include "Log.hh"
#include <fr/libfr.h>
#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>
#include <qsdk/demux.h>

typedef enum {
  WA_PCM = 0x01,
  WA_PCMA = 0x06,
  WA_PCMU = 0x07,
  WA_IMA_ADPCM = 0x11,
  WA_UNKNOWN
} WAV_AUDIO_FORMAT;

#define AUDIO_BUF_SIZE			1024

class PCMLiveFileSource: public AudioInputDevice {
public:

  static PCMLiveFileSource* createNew(UsageEnvironment& env,
					struct demux_t * demux);

  unsigned char getAudioFormat();

protected:
  PCMLiveFileSource(UsageEnvironment& env, struct demux_t * demux);
	// called only by createNew()

  virtual ~PCMLiveFileSource();


private:
  // redefined virtual functions:
  virtual void doGetNextFrame();
  virtual void doStopGettingFrames();
  virtual Boolean setInputPort(int portIndex);
  virtual double getAverageLevel() const;
  
private:
  unsigned char fAudioFormat;
  struct demux_t * fDemux;
  struct demux_frame_t fFrame;
  uint64_t f_last_timestamp;
};

#endif
