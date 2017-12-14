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

#include "PCMLiveAudioSource.hh"
#include "GroupsockHelper.hh"
#include <pthread.h>



////////// PCMLiveAudioSource //////////

PCMLiveAudioSource*
PCMLiveAudioSource::createNew(UsageEnvironment& env,const char *chanID) {

	return new PCMLiveAudioSource(env,chanID);
}

unsigned char PCMLiveAudioSource::getAudioFormat() {
  return fAudioFormat;
}

PCMLiveAudioSource::PCMLiveAudioSource(UsageEnvironment& env,const char *chanID)
  : AudioInputDevice(env, 0, 0, 0, 0)/* set the real parameters later */,
 	fAudioFormat(WA_UNKNOWN) {
 	fr_INITBUF(&fFrBuf, NULL);
	fChanID = strDup(chanID);
	env << "PCMLiveAudioSource::fChanID:" << fChanID <<"\n";
	fHandle = -1;
	f_last_timestamp = 0;
	
	f_audio_codec = NULL;
	memset((char *)&f_audio_codec_info, 0, sizeof(struct codec_info));
	memset((char *)&f_audio_codec_addr_info,0,sizeof(struct codec_addr));
	f_audio_codec_addr_info.out = new char[AUDIO_BUF_SIZE];
	
	fAudioFormat = WA_PCMA;

	audio_fmt_t fmt;
	memset(&fmt,0,sizeof(audio_fmt_t));
	
	fHandle = audio_get_channel(fChanID,NULL, CHANNEL_BACKGROUND);
    if (fHandle < 0) {
	    LOGE("audio get channel of %s failed\n", fChanID);
    }
	printf("fmt->samplingrate:%d\n",fmt.samplingrate);

	//audio box init
	int ret = -1;
	ret = audio_get_format(fChanID,&fmt);
    if (ret < 0) {
        LOGE("audio get format of %d failed\n", ret);
    }

	printf("fmt.sample_size:%d\n",fmt.sample_size);

	//open audio codec
	f_audio_codec_info.in.codec_type = AUDIO_CODEC_PCM;
	f_audio_codec_info.in.channel = fmt.channels == 0 ? 2:fmt.channels;
	f_audio_codec_info.in.sample_rate = fmt.samplingrate == 0 ? 8000: fmt.samplingrate;
	f_audio_codec_info.in.bitwidth = fmt.bitwidth == 0 ? 24:fmt.bitwidth;
	f_audio_codec_info.in.bit_rate = f_audio_codec_info.in.channel \
		* f_audio_codec_info.in.sample_rate \
		* GET_BITWIDTH(f_audio_codec_info.in.bitwidth);
	f_audio_codec_info.out.codec_type = AUDIO_CODEC_G711A;
	f_audio_codec_info.out.channel = 1;
	f_audio_codec_info.out.sample_rate = 8000;
	f_audio_codec_info.out.bitwidth = 16;
	f_audio_codec_info.out.bit_rate = f_audio_codec_info.out.channel \
		* f_audio_codec_info.out.sample_rate \
		* GET_BITWIDTH(f_audio_codec_info.out.bitwidth);
	f_audio_codec= codec_open(&f_audio_codec_info);
	if (!f_audio_codec) {
		LOGE("audio send open codec decoder failed %x \n",  f_audio_codec_info.out.codec_type);
	}

	while(fGranularityInMS == 0)
	{
		int ret = -1;
		ret = audio_get_frame(fHandle, &fFrBuf);
		if ((ret < 0) || (fFrBuf.size <= 0) || (fFrBuf.virt_addr == NULL)) {
			LOGE("audio_get_frame() failed, ret=%d\n",ret);
		}

		if(f_last_timestamp != 0)
		{
			fGranularityInMS = fFrBuf.timestamp - f_last_timestamp;
			f_last_timestamp = 0;
		}

		f_last_timestamp = fFrBuf.timestamp;
	}

	fSamplingFrequency = f_audio_codec_info.out.sample_rate;
	fBitsPerSample = f_audio_codec_info.out.bitwidth;
	fNumChannels = f_audio_codec_info.out.channel;
	printf("fSamplingFrequency:%u\n",fSamplingFrequency);
	printf("fBitsPerSample:%u\n",fBitsPerSample);
	printf("fNumChannels:%u\n",fNumChannels);
	printf("fGranularityInMS:%u\n",fGranularityInMS);
  
}

PCMLiveAudioSource::~PCMLiveAudioSource() {
	delete[] fChanID;
	delete[] (char *)f_audio_codec_addr_info.out;
	if (fHandle >= 0) {
		audio_put_channel(fHandle);
		fHandle = -1;
	}
	closeAudioCodec();
}

void PCMLiveAudioSource::doGetNextFrame() {
	if(strlen(fChanID) == 0)
	{
		LOGE("fChanID is NULL\n");
		return;
	}

	int ret = -1;
	
	ret = audio_get_frame(fHandle, &fFrBuf);

	if ((ret < 0) || (fFrBuf.size <= 0) || (fFrBuf.virt_addr == NULL)) {
		LOGE("audio_get_frame() failed, ret=%d\n",ret);
		handleClosure();
		return;
	} else {
		f_audio_codec_addr_info.in = fFrBuf.virt_addr;
		f_audio_codec_addr_info.len_in = fFrBuf.size;
		f_audio_codec_addr_info.len_out = AUDIO_BUF_SIZE;
		ret = codec_encode(f_audio_codec, &f_audio_codec_addr_info);
		if (ret <= 0) {
			LOGE("audio codec encode failed %d \n",  ret);
			audio_put_frame(fHandle, &fFrBuf);
			return;
		}
		
		fFrameSize = ret;
		fSizePerFrame = ret;
				
		if(fFrameSize > fMaxSize)
		{
			fFrameSize = fMaxSize;
			fNumTruncatedBytes = ret - fFrameSize;
			LOGE("fFrameSize(%u) is bigger than fMaxSize(%u),data has been truncted\n",fFrameSize,fMaxSize);
			LOGD("fNumTruncatedBytes:%d\n",fNumTruncatedBytes);
		}

		memmove(fTo,f_audio_codec_addr_info.out,fFrameSize);

		fDurationInMicroseconds = fGranularityInMS * 1000;
		if(f_last_timestamp != 0)
		{
			fDurationInMicroseconds = (fFrBuf.timestamp - f_last_timestamp) * 1000;//42*1000+encode time
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

		LOGD("                               audio timestamp:%llu,cur time:%u:%u\n",fFrBuf.timestamp,fPresentationTime.tv_sec,fPresentationTime.tv_usec);

		f_last_timestamp = fFrBuf.timestamp;

		audio_put_frame(fHandle, &fFrBuf);
	}

	FramedSource::afterGetting(this);
}

void PCMLiveAudioSource::doStopGettingFrames() {
	closeAudioCodec();
}


void PCMLiveAudioSource::closeAudioCodec()
{
	if (f_audio_codec) {
		int length;
		int ret = -1;
		do {
			ret = codec_flush(f_audio_codec, &f_audio_codec_addr_info, &length);
			if (ret == CODEC_FLUSH_ERR)
				break;

			/* TODO you need least data or not ?*/
		} while (ret == CODEC_FLUSH_AGAIN);
		codec_close(f_audio_codec);
		f_audio_codec = NULL;
		LOGD("audio codec quit\n");
	}
}


Boolean PCMLiveAudioSource::setInputPort(int /*portIndex*/) {
  return True;
}

double PCMLiveAudioSource::getAverageLevel() const {
  return 0.0;//##### fix this later
}


