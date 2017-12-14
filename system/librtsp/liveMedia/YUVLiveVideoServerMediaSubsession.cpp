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
// on demand, from live YUV source.
// Implementation

#include "YUVLiveVideoServerMediaSubsession.hh"
#include "YUVVideoRTPSink.hh"
#include "YUVLiveVideoSource.hh"

YUVLiveVideoServerMediaSubsession*
YUVLiveVideoServerMediaSubsession::createNew(UsageEnvironment& env,
					      char const* chanID,
					      Boolean reuseFirstSource) {
  return new YUVLiveVideoServerMediaSubsession(env, chanID, reuseFirstSource);
}

YUVLiveVideoServerMediaSubsession::YUVLiveVideoServerMediaSubsession(UsageEnvironment& env,
								       char const* chanID, Boolean reuseFirstSource)
  : OnDemandServerMediaSubsession(env,reuseFirstSource),fAuxSDPLine(NULL) {
    fChanID = strDup(chanID);
	if(NULL == fChanID)
		fChanID = strDup("isp-out");
	env << "YUVLiveVideoServerMediaSubsession::fChanID:" << fChanID <<"\n";
}

YUVLiveVideoServerMediaSubsession::~YUVLiveVideoServerMediaSubsession() {
  delete[] fAuxSDPLine;
  delete[] fChanID;
}

char const* YUVLiveVideoServerMediaSubsession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource) {
  //return "a=fmtp:96 packetization-mode=1;profile-level-id=4D6028;sprop-parameter-sets=J01gKI1oBQBbpsgAAAMACAAAAwDweKEV,KO4D0kg=\r\n";
  static char auxSDP[256]= {0};
  int w,h;
  w=h=0;
  video_get_resolution(fChanID,&w,&h);
  sprintf(auxSDP,"a=fmtp:110 packetization-mode=1;profile-level-id=4D6028\r\na=x-dimensions:%d,%d\r\n",w,h);
  return auxSDP;
}

FramedSource* YUVLiveVideoServerMediaSubsession::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
  estBitrate = 8 * 1024 * 1024; // kbps, estimate

  // Create the video source:
  return YUVLiveVideoSource::createNew(envir(), fChanID);
}

RTPSink* YUVLiveVideoServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/) {
	return YUVVideoRTPSink::createNew(envir(), rtpGroupsock, 110);
}
