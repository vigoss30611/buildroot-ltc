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
// Implementation

#include "PCMLiveFileServerMediaSubsession.hh"
#include "PCMLiveFileSource.hh"
#include "uLawAudioFilter.hh"
#include "SimpleRTPSink.hh"

PCMLiveFileServerMediaSubsession* PCMLiveFileServerMediaSubsession
::createNew(UsageEnvironment& env, char const* chanID, Boolean reuseFirstSource,
	    Boolean convertToULaw) {
  return new PCMLiveFileServerMediaSubsession(env, chanID,
					       reuseFirstSource, convertToULaw);
}

PCMLiveFileServerMediaSubsession
::PCMLiveFileServerMediaSubsession(UsageEnvironment& env, char const* chanID,
				    Boolean reuseFirstSource, Boolean convertToULaw)
  : OnDemandServerMediaSubsession(env,reuseFirstSource),
    fConvertToULaw(convertToULaw) {
    if(chanID == NULL)
		fChanID = strDup("test.mkv");
	else
		fChanID = strDup(chanID);
	
	env << "PCMLiveFileMediaSubsession::fChanID:" << fChanID <<"\n";
  	if(strchr(fChanID,'.mkv') != NULL)
  	{
	  	fDemux = demux_init(fChanID); 

	  	if(!fDemux) {
			LOGE("demux %s failed\n", fChanID);
	  	}
  	}
}

PCMLiveFileServerMediaSubsession
::~PCMLiveFileServerMediaSubsession() {
	delete[] fChanID;
	if(fDemux != NULL)
  	{
	 	demux_deinit(fDemux);
  	}
}

char const* PCMLiveFileServerMediaSubsession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource) {
  unsigned int ptime = int((fFrameSize * 8) *1000 / (fNumChannels * fBitsPerSample * fSamplingFrequency) + 0.5);
  static char auxSDP[64] = {0};
  sprintf(auxSDP,"a=ptime:%d\r\n",ptime);
  return auxSDP;
}


FramedSource* PCMLiveFileServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
  FramedSource* resultSource = NULL;
  do {
    PCMLiveFileSource* pcmSource = PCMLiveFileSource::createNew(envir(), fDemux);
    if (pcmSource == NULL) break;

    // Get attributes of the audio source:

    fAudioFormat = pcmSource->getAudioFormat();
    fBitsPerSample = pcmSource->bitsPerSample();
	fFrameSize = pcmSource->frameSize();
    // We handle only 4,8,16,20,24 bits-per-sample audio:
    if (fBitsPerSample%4 != 0 || fBitsPerSample < 4 || fBitsPerSample > 24 || fBitsPerSample == 12) {
      envir() << "The pcm live source contains " << fBitsPerSample << " bit-per-sample audio, which we don't handle\n";
      break;
    }
    fSamplingFrequency = pcmSource->samplingFrequency();
    fNumChannels = pcmSource->numChannels();
    unsigned bitsPerSecond = fSamplingFrequency*fBitsPerSample*fNumChannels;

    // Add in any filter necessary to transform the data prior to streaming:
    resultSource = pcmSource; // by default
    if (fAudioFormat == WA_PCM) {
      if (fBitsPerSample == 16) {
	// Note that samples in the WAV audio file are in little-endian order.
	if (fConvertToULaw) {
	  // Add a filter that converts from raw 16-bit PCM audio to 8-bit u-law audio:
	  resultSource = uLawFromPCMAudioSource::createNew(envir(), pcmSource, 1/*little-endian*/);
	  bitsPerSecond /= 2;
	} else {
	  // Add a filter that converts from little-endian to network (big-endian) order:
	  resultSource = EndianSwap16::createNew(envir(), pcmSource);
	}
      } else if (fBitsPerSample == 20 || fBitsPerSample == 24) {
	// Add a filter that converts from little-endian to network (big-endian) order:
	resultSource = EndianSwap24::createNew(envir(), pcmSource);
      }
    }

    estBitrate = (bitsPerSecond+500)/1000; // kbps
    return resultSource;
  } while (0);

  // An error occurred:
  Medium::close(resultSource);
  return NULL;
}

RTPSink* PCMLiveFileServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/) {
  do {
    char const* mimeType;
    unsigned char payloadFormatCode = rtpPayloadTypeIfDynamic; // by default, unless a static RTP payload type can be used
    if (fAudioFormat == WA_PCM) {
      if (fBitsPerSample == 16) {
	if (fConvertToULaw) {
	  mimeType = "PCMU";
	  if (fSamplingFrequency == 8000 && fNumChannels == 1) {
	    payloadFormatCode = 0; // a static RTP payload type
	  }
	} else {
	  mimeType = "L16";
	  if (fSamplingFrequency == 44100 && fNumChannels == 2) {
	    payloadFormatCode = 10; // a static RTP payload type
	  } else if (fSamplingFrequency == 44100 && fNumChannels == 1) {
	    payloadFormatCode = 11; // a static RTP payload type
	  }
	}
      } else if (fBitsPerSample == 20) {
	mimeType = "L20";
      } else if (fBitsPerSample == 24) {
	mimeType = "L24";
      } else { // fBitsPerSample == 8 (we assume that fBitsPerSample == 4 is only for WA_IMA_ADPCM)
	mimeType = "L8";
      }
    } else if (fAudioFormat == WA_PCMU) {
      mimeType = "PCMU";
      if (fSamplingFrequency == 8000 && fNumChannels == 1) {
	payloadFormatCode = 0; // a static RTP payload type
      }
    } else if (fAudioFormat == WA_PCMA) {
      mimeType = "PCMA";
      if (fSamplingFrequency == 8000 && fNumChannels == 1) {
	payloadFormatCode = 8; // a static RTP payload type
      }
    } else if (fAudioFormat == WA_IMA_ADPCM) {
      mimeType = "DVI4";
      // Use a static payload type, if one is defined:
      if (fNumChannels == 1) {
	if (fSamplingFrequency == 8000) {
	  payloadFormatCode = 5; // a static RTP payload type
	} else if (fSamplingFrequency == 16000) {
	  payloadFormatCode = 6; // a static RTP payload type
	} else if (fSamplingFrequency == 11025) {
	  payloadFormatCode = 16; // a static RTP payload type
	} else if (fSamplingFrequency == 22050) {
	  payloadFormatCode = 17; // a static RTP payload type
	}
      }
    } else { //unknown format
      break;
    }

	return SimpleRTPSink::createNew(envir(), rtpGroupsock,
				    payloadFormatCode, fSamplingFrequency,
				    "audio", mimeType, fNumChannels,False);
  } while (0);

  // An error occurred:
  return NULL;
}


