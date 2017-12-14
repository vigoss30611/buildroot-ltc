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

#include "H264or5LiveVideoServerMediaSubsession.hh"
#include "H264VideoRTPSink.hh"
#include "H264VideoStreamFramer.hh"
#include "H265VideoRTPSink.hh"
#include "H265VideoStreamFramer.hh"

#include "H264or5LiveVideoSource.hh"

H264or5LiveVideoServerMediaSubsession*
H264or5LiveVideoServerMediaSubsession::createNew(UsageEnvironment& env,
					      char const* chanID,
					      Boolean reuseFirstSource) {
  return new H264or5LiveVideoServerMediaSubsession(env, chanID, reuseFirstSource);
}

H264or5LiveVideoServerMediaSubsession::H264or5LiveVideoServerMediaSubsession(UsageEnvironment& env,
								       char const* chanID, Boolean reuseFirstSource)
  : OnDemandServerMediaSubsession(env,reuseFirstSource),
    fAuxSDPLine(NULL), fDoneFlag(0), fDummyRTPSink(NULL) {
    fChanID = strDup(chanID);
	if(NULL == fChanID)
		fChanID = strDup("enc1080p-stream");
	env << "H264LiveVideoServerMediaSubsession::fChanID:" << fChanID <<"\n";

	if(video_enable_channel(fChanID) < 0)
	{
		LOGE("video_enable_channel(%s) failed,video crashed\n", fChanID);
	}

	struct v_basic_info basicInfo;
	video_get_basicinfo(fChanID,&basicInfo);
	fEncoderType = basicInfo.media_type;
}

H264or5LiveVideoServerMediaSubsession::~H264or5LiveVideoServerMediaSubsession() {
  video_disable_channel(fChanID);
  delete[] fAuxSDPLine;
  delete[] fChanID;
}

static void afterPlayingDummy(void* clientData) {
  H264or5LiveVideoServerMediaSubsession* subsess = (H264or5LiveVideoServerMediaSubsession*)clientData;
  subsess->afterPlayingDummy1();
}

void H264or5LiveVideoServerMediaSubsession::afterPlayingDummy1() {
  // Unschedule any pending 'checking' task:
  envir().taskScheduler().unscheduleDelayedTask(nextTask());
  // Signal the event loop that we're done:
  setDoneFlag();
}

static void checkForAuxSDPLine(void* clientData) {
  H264or5LiveVideoServerMediaSubsession* subsess = (H264or5LiveVideoServerMediaSubsession*)clientData;
  subsess->checkForAuxSDPLine1();
}

void H264or5LiveVideoServerMediaSubsession::checkForAuxSDPLine1() {
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

char const* H264or5LiveVideoServerMediaSubsession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource) {
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

FramedSource* H264or5LiveVideoServerMediaSubsession::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
  estBitrate = 500; // kbps, estimate

  // Create the video source:
  H264or5LiveVideoSource* liveSource = H264or5LiveVideoSource::createNew(envir(), fChanID);
  if (liveSource == NULL) return NULL;

  // Create a framer for the Video Elementary Stream:
  if(fEncoderType == VIDEO_MEDIA_H264)
  	return H264VideoStreamFramer::createNew(envir(), liveSource);
  else if(fEncoderType == VIDEO_MEDIA_HEVC)
  	return H265VideoStreamFramer::createNew(envir(), liveSource);
  else
  	return H265VideoStreamFramer::createNew(envir(), liveSource);
}

RTPSink* H264or5LiveVideoServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/) {
  if(fEncoderType == VIDEO_MEDIA_H264)
	return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
  else if(fEncoderType == VIDEO_MEDIA_HEVC)
  	return H265VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
  else
  	return H265VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}
