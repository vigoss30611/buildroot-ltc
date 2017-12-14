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
// A pcm live audio source
// Implementation

#include "PCMLiveFileSource.hh"
#include "GroupsockHelper.hh"

////////// PCMLiveAudioSource //////////

PCMLiveFileSource*
PCMLiveFileSource::createNew(UsageEnvironment& env,struct demux_t *demux) {

	return new PCMLiveFileSource(env,demux);
}

unsigned char PCMLiveFileSource::getAudioFormat() {
  return fAudioFormat;
}

PCMLiveFileSource::PCMLiveFileSource(UsageEnvironment& env,struct demux_t *demux)
  : AudioInputDevice(env, 0, 0, 0, 0)/* set the real parameters later */,
 	fAudioFormat(WA_UNKNOWN) {
	fDemux = demux;
	memset(&fFrame,0,sizeof(struct demux_frame_t));
	f_last_timestamp = 0;

	int ret = -1;
	while((ret = demux_get_frame(fDemux, &fFrame)) == 0) {		
		if(fFrame.codec_id != DEMUX_AUDIO_PCM_ALAW) 
		{
			demux_put_frame(fDemux, &fFrame);
			continue;
		}
		else
		{
			break;
		}
	}

	int audioCodecId = fFrame.codec_id;
	if(audioCodecId == DEMUX_AUDIO_PCM_ALAW)
	{
		fAudioFormat = WA_PCMA;
	}
	else if(audioCodecId == DEMUX_AUDIO_PCM_ULAW)
	{
		fAudioFormat = WA_PCMU;
	}
	else
	{
		fAudioFormat = WA_UNKNOWN;
	} 
	
	fSizePerFrame = fFrame.data_size ; 
	fSamplingFrequency = fFrame.sample_rate;
	fBitsPerSample = fFrame.bitwidth;	
	fNumChannels = fFrame.channels;
	printf("fAudioFormat:%d\n",fAudioFormat);
	printf("fSamplingFrequency:%u\n",fSamplingFrequency);
	printf("fBitsPerSample:%u\n",fBitsPerSample);
	printf("fNumChannels:%u\n",fNumChannels); 
	printf("fSizePerFrame:%d\n",fSizePerFrame);
	
	demux_put_frame(fDemux, &fFrame);

	demux_seek(fDemux,0,SEEK_SET);
}

PCMLiveFileSource::~PCMLiveFileSource() {	

}

void PCMLiveFileSource::doGetNextFrame() {
	int ret = -1;

	while((ret = demux_get_frame(fDemux, &fFrame)) == 0) {		
		if(fFrame.codec_id != DEMUX_AUDIO_PCM_ALAW) 
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
		fNumTruncatedBytes = fFrame.data_size - fFrameSize;
		LOGE("fFrameSize(%u) is bigger than fMaxSize(%u),data has been truncted\n",fFrameSize,fMaxSize);
		LOGD("fNumTruncatedBytes:%d\n",fNumTruncatedBytes);
	}

	memmove(fTo,fFrame.data,fFrameSize); 

 	if(f_last_timestamp != 0)
	{
		fDurationInMicroseconds = (fFrame.timestamp - f_last_timestamp) * 1000;
	}
	
	if ((fPresentationTime.tv_sec == 0) && (fPresentationTime.tv_usec == 0)) {
	  // This is the first frame, so use the current time:
	  gettimeofday(&fPresentationTime, NULL);
	} else {
	  // Increment by the play time of the previous frame:
	  unsigned uSeconds = fPresentationTime.tv_usec + fDurationInMicroseconds;
	  fPresentationTime.tv_sec += uSeconds/1000000;
	  fPresentationTime.tv_usec = uSeconds%1000000;
	}

	LOGD("                               audio timestamp:%llu,cur time:%u:%u\n",fFrame.timestamp,fPresentationTime.tv_sec,fPresentationTime.tv_usec);

	f_last_timestamp = fFrame.timestamp;

	demux_put_frame(fDemux, &fFrame);
	FramedSource::afterGetting(this);
}

void PCMLiveFileSource::doStopGettingFrames() 
{	
}

Boolean PCMLiveFileSource::setInputPort(int /*portIndex*/) {
  return True;
}

double PCMLiveFileSource::getAverageLevel() const {
  return 0.0;//##### fix this later
}


