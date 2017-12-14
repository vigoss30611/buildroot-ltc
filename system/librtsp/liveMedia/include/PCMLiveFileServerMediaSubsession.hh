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
// A 'ServerMediaSubsession' object that creates new, unicast, "RTPSink"s
// on demand, from an WAV audio file.
// C++ header

#ifndef _PCM_LIVE_FILE_SERVER_MEDIA_SUBSESSION_HH
#define _PCM_LIVE_FILE_SERVER_MEDIA_SUBSESSION_HH

#ifndef _ON_DEMAND_SERVER_MEDIA_SUBSESSION_HH
#include "OnDemandServerMediaSubsession.hh"
#endif

class PCMLiveFileServerMediaSubsession: public OnDemandServerMediaSubsession{
public:
  static PCMLiveFileServerMediaSubsession*
  createNew(UsageEnvironment& env, char const* chanID, Boolean reuseFirstSource,
	    Boolean convertToULaw = False);
      // If "convertToULaw" is True, 16-bit audio streams are converted to
      // 8-bit u-law audio prior to streaming.

protected:
  PCMLiveFileServerMediaSubsession(UsageEnvironment& env, char const* fileName,
				    Boolean reuseFirstSource, Boolean convertToULaw);
      // called only by createNew();
  virtual ~PCMLiveFileServerMediaSubsession();

protected: // redefined virtual functions
  virtual char const* getAuxSDPLine(RTPSink* rtpSink,
					  FramedSource* inputSource);
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
					      unsigned& estBitrate);
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
                                    unsigned char rtpPayloadTypeIfDynamic,
				    FramedSource* inputSource);

protected:
  Boolean fConvertToULaw;

  // The following parameters of the input stream are set after
  // "createNewStreamSource" is called:
  unsigned char fAudioFormat;
  unsigned char fBitsPerSample;
  unsigned fSamplingFrequency;
  unsigned fNumChannels;
  unsigned fFrameSize;
  char * fChanID;
  struct demux_t * fDemux;
};

#endif
