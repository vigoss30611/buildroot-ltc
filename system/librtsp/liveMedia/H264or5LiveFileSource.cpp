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
// A file source that is frames
// Implementation

#include "H264or5LiveFileSource.hh"
#include <qsdk/video.h>

//#include "GroupsockHelper.hh"

////////// H264LiveVideoSource //////////

H264or5LiveFileSource*
H264or5LiveFileSource::createNew(UsageEnvironment& env,struct demux_t *demux) {
  return new H264or5LiveFileSource(env,demux);
}

H264or5LiveFileSource::H264or5LiveFileSource(UsageEnvironment& env,struct demux_t *demux)
  : FramedSource(env){
	fDemux = demux;
	memset(&fFrame,0,sizeof(struct demux_frame_t));
	fTruncatedBytes = new char[VIDEO_BUF_SIZE];
	fTruncatedBytesNum = 0;
	fSendHeaderCount = 1;

	demux_seek(fDemux,0,SEEK_SET);
}

H264or5LiveFileSource::~H264or5LiveFileSource() {
	delete[] fTruncatedBytes;
}

void H264or5LiveFileSource::doGetNextFrame() {
	int ret = -1;
	
	if(fSendHeaderCount > 0)
	{
		int headerLen = 0;
		char h264Header[128];
		headerLen = demux_get_head(fDemux,h264Header,128);
		memmove(fTo,h264Header,headerLen);
		fFrameSize = headerLen;
		fSendHeaderCount--;
	}
	else
	{
		while((ret = demux_get_frame(fDemux, &fFrame)) == 0) {	
			if((fFrame.codec_id != DEMUX_VIDEO_H265) 
				&& (fFrame.codec_id != DEMUX_VIDEO_H264))
			{
				demux_put_frame(fDemux, &fFrame);
				continue;
			}
			else
			{
				break;
			}
		}
		
		if(ret < 0)
		{
			LOGE("call demux_get_frame error\n");
			handleClosure();
			return;
		}
				
		fFrameSize = fFrame.data_size;
		if(fFrameSize > fMaxSize)
		{
			fFrameSize = fMaxSize;
			int bufUsedBytes = fMaxSize;
			fNumTruncatedBytes = fFrame.data_size - bufUsedBytes;
			LOGD("Truncat %d bytes\n",fNumTruncatedBytes);
			memmove(fTo,fFrame.data,bufUsedBytes);
			memmove(fTruncatedBytes,fFrame.data + bufUsedBytes,fNumTruncatedBytes);
			fTruncatedBytesNum = fNumTruncatedBytes;
		}
		else
		{	
			if(fTruncatedBytesNum > 0)
			{
				memmove(fTo,fTruncatedBytes,fTruncatedBytesNum);
				memmove(fTo + fTruncatedBytesNum,fFrame.data,fFrame.data_size);
				fFrameSize += fTruncatedBytesNum;
				LOGD("send last truncted %d bytes\n",fTruncatedBytesNum);
				fTruncatedBytesNum = 0;
			}
			else
			{
				memmove(fTo,fFrame.data,fFrame.data_size);
			}	
		}
		
		demux_put_frame(fDemux, &fFrame);
	}

	fDurationInMicroseconds = 1000000 / fDemux->stream_info->fps;
	if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
	  // This is the first frame, so use the current time:
	  gettimeofday(&fPresentationTime, NULL);
	} else {
	  // Increment by the play time of the previous frame:
	  unsigned uSeconds = fPresentationTime.tv_usec + fDurationInMicroseconds;
	  fPresentationTime.tv_sec += uSeconds/1000000;
	  fPresentationTime.tv_usec = uSeconds%1000000;
	}
	LOGD("video timestamp %llu,cur time:%u:%u\n",fFrame.timestamp,fPresentationTime.tv_sec,fPresentationTime.tv_usec/1000);

	FramedSource::afterGetting(this);
}

