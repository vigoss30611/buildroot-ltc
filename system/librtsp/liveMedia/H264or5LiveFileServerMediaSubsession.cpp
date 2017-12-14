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
// on demand, from a H264 video file.
// Implementation

#include "H264or5LiveFileServerMediaSubsession.hh"
#include "H264VideoRTPSink.hh"
#include "H264VideoStreamFramer.hh"
#include "H265VideoRTPSink.hh"
#include "H265VideoStreamFramer.hh"
#include "H264or5LiveFileSource.hh"

H264or5LiveFileServerMediaSubsession*
H264or5LiveFileServerMediaSubsession::createNew(UsageEnvironment& env,
					      char const* chanID,
					      Boolean reuseFirstSource) {
  return new H264or5LiveFileServerMediaSubsession(env, chanID, reuseFirstSource);
}

H264or5LiveFileServerMediaSubsession::H264or5LiveFileServerMediaSubsession(UsageEnvironment& env,
								       char const* chanID, Boolean reuseFirstSource)
  : OnDemandServerMediaSubsession(env,reuseFirstSource),
    fAuxSDPLine(NULL), fDoneFlag(0), fDummyRTPSink(NULL) {
	fChanID = strDup(chanID);
	if(NULL == fChanID)
		fChanID = strDup("test.mkv");
	env << "H264LiveFileServerMediaSubsession::fChanID:" << fChanID <<"\n";
	
	if(strchr(fChanID,'.') != NULL)
  	{
	  	fDemux = demux_init(fChanID);
	  	if(!fDemux) {
			LOGE("demux %s failed\n", fChanID);
	  	}
  	}

	fCodecID = fDemux->stream_info[fDemux->video_index].codec_id;
}

H264or5LiveFileServerMediaSubsession::~H264or5LiveFileServerMediaSubsession() {
  delete[] fAuxSDPLine;
  delete[] fChanID;
  if(fDemux != NULL)
  {
	 demux_deinit(fDemux);
  }
}

static void afterPlayingDummy(void* clientData) {
  H264or5LiveFileServerMediaSubsession* subsess = (H264or5LiveFileServerMediaSubsession*)clientData;
  subsess->afterPlayingDummy1();
}

void H264or5LiveFileServerMediaSubsession::afterPlayingDummy1() {
  // Unschedule any pending 'checking' task:
  envir().taskScheduler().unscheduleDelayedTask(nextTask());
  // Signal the event loop that we're done:
  setDoneFlag();
}

static void checkForAuxSDPLine(void* clientData) {
  H264or5LiveFileServerMediaSubsession* subsess = (H264or5LiveFileServerMediaSubsession*)clientData;
  subsess->checkForAuxSDPLine1();
}

void H264or5LiveFileServerMediaSubsession::checkForAuxSDPLine1() {
  char const* dasl;

  if (fAuxSDPLine != NULL) {
    // Signal the event loop that we're done:
    setDoneFlag();
  } else if (fDummyRTPSink != NULL && (dasl = fDummyRTPSink->auxSDPLine()) != NULL) {
    fAuxSDPLine = strDup(dasl);
    fDummyRTPSink = NULL;

    // Signal the event loop that we're done:
    setDoneFlag();
  } else if (!fDoneFlag) {
    // try again after a brief delay:
    //int uSecsToDelay = 100000; // 100 ms
    int uSecsToDelay = 10; // 100 ms
    nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
			      (TaskFunc*)checkForAuxSDPLine, this);
  }
}

char const* H264or5LiveFileServerMediaSubsession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource) {
  //return "a=fmtp:96 packetization-mode=1;profile-level-id=4D6028;sprop-parameter-sets=J01gKI1oBQBbpsgAAAMACAAAAwDweKEV,KO4D0kg=\r\n";
  if (fAuxSDPLine != NULL) return fAuxSDPLine; // it's already been set up (for a previous client)

  if (fDummyRTPSink == NULL) { // we're not already setting it up for another, concurrent stream
    // Note: For H264 video files, the 'config' information ("profile-level-id" and "sprop-parameter-sets") isn't known
    // until we start reading the file.  This means that "rtpSink"s "auxSDPLine()" will be NULL initially,
    // and we need to start reading data from our file until this changes.
    fDummyRTPSink = rtpSink;

    // Start reading the file:
    fDummyRTPSink->startPlaying(*inputSource, afterPlayingDummy, this);

    // Check whether the sink's 'auxSDPLine()' is ready:
    checkForAuxSDPLine(this);
  }

  envir().taskScheduler().doEventLoop(&fDoneFlag);

  return fAuxSDPLine;
}

FramedSource* H264or5LiveFileServerMediaSubsession::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
  estBitrate = 500; // kbps, estimate

  // Create the video source:
  H264or5LiveFileSource* liveSource = H264or5LiveFileSource::createNew(envir(), fDemux);
  if (liveSource == NULL) return NULL;

  // Create a framer for the Video Elementary Stream:
  if(fCodecID == DEMUX_VIDEO_H264)
  	return H264VideoStreamFramer::createNew(envir(), liveSource);
  else if(fCodecID == DEMUX_VIDEO_H265)
  	return H265VideoStreamFramer::createNew(envir(), liveSource);
  else
  	return H265VideoStreamFramer::createNew(envir(), liveSource);
}

RTPSink* H264or5LiveFileServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/) {
  if(fCodecID == DEMUX_VIDEO_H264)
  	return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
  else if(fCodecID == DEMUX_VIDEO_H265)
  	return H265VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
  else 
  	return H265VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}
