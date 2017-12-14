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
// A live source that is frames
// Implementation

////////// YUVLiveVideoSource //////////

#include "YUVLiveVideoSource.hh"
#include <qsdk/video.h>
#include "FramedSource.hh"
#include <stdio.h>


YUVLiveVideoSource*
YUVLiveVideoSource::createNew(UsageEnvironment& env,const char * channelID ) {
  return new YUVLiveVideoSource(env,channelID);
}

YUVLiveVideoSource::YUVLiveVideoSource(UsageEnvironment& env,const char * channelID)
  : FramedSource(env){
    fChanID = strDup(channelID);
    env << "YUVLiveVideoSource::fChanID:" << fChanID <<"\n";
    fr_INITBUF(&fFrBuf, NULL);
    fTruncatedBytes = new char[VIDEO_BUF_SIZE];
    fTruncatedBytesNum = 0;
}

YUVLiveVideoSource::~YUVLiveVideoSource() {
    delete[] fChanID;
    delete[] fTruncatedBytes;
}

void YUVLiveVideoSource::doGetNextFrame() {
    if(strlen(fChanID) == 0)
    {
        LOGE("fChanID is NULL\n");
        return;
    }

    int ret = -1;
    int count = 0;
again:
    /*!< get frame from videobox */
    ret = video_get_frame(fChanID, &fFrBuf);

    if ((ret < 0) || (fFrBuf.size <= 0) || (fFrBuf.virt_addr == NULL)) {
        LOGE("stream %s video_get_frame() failed, ret=%d\n", fChanID, ret);
        count++;
        if(count < 10)
            goto again;
        else
        {
            handleClosure();
            return;
        }
    }
    else {
        fFrameSize = fFrBuf.size;
        if(fFrameSize > fMaxSize)
        {
            fFrameSize = fMaxSize;
            int frBufUsedBytes = fMaxSize;
            fNumTruncatedBytes = fFrBuf.size - frBufUsedBytes;
            LOGD("Truncat %d bytes\n",fNumTruncatedBytes);
            memmove(fTo,fFrBuf.virt_addr,frBufUsedBytes);
            memmove(fTruncatedBytes,fFrBuf.virt_addr + frBufUsedBytes,fNumTruncatedBytes);
            fTruncatedBytesNum = fNumTruncatedBytes;
            }
        else
        {
            if(fTruncatedBytesNum > 0)
            {
                memmove(fTo,fTruncatedBytes,fTruncatedBytesNum);
                memmove(fTo + fTruncatedBytesNum,fFrBuf.virt_addr,fFrBuf.size);
                fFrameSize += fTruncatedBytesNum;
                LOGD("send last truncted %d bytes\n",fTruncatedBytesNum);
                fTruncatedBytesNum = 0;
            }
            else
            {
                memmove (fTo,fFrBuf.virt_addr,fFrBuf.size);
            }
        }

        fDurationInMicroseconds = 1000000 / video_get_fps(fChanID);
        if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
          // This is the first frame, so use the current time:
          gettimeofday(&fPresentationTime, NULL);
        } else {
          // Increment by the play time of the previous frame:
          unsigned uSeconds = fPresentationTime.tv_usec + fDurationInMicroseconds;
          fPresentationTime.tv_sec += uSeconds/1000000;
          fPresentationTime.tv_usec = uSeconds%1000000;
        }

        LOGD("video timestamp %llu,timestamp:%u:%u\n",fFrBuf.timestamp,fPresentationTime.tv_sec,fPresentationTime.tv_usec/1000);

        video_put_frame(fChanID, &fFrBuf);

#if 0
    static FILE * h264File = NULL;
    static int count=0;

    if(NULL == h264File)
    {
        h264File = fopen("/mnt/sd0/h264IFrame.264","wb");
        //h264File = fopen("/mnt/sd0/h264IFrame.264","rb");
        LOGD("open /mnt/sd0/h264IFrame.264\n");
    }

    if(count < 1000)
    {
        write(&fFrameSize,4,1,h264File);
        LOGD("write 4 bytes\n");
        write(fTo,fFrameSize,1,h264File);
        LOGD("write %d bytes\n",fFrameSize);
        //fread(&fFrameSize,4,1,h264File);
        //LOGD("fFrameSize:%u\n",fFrameSize);
        //fread(fTo,fFrameSize,1,h264File);
        //LOGD("read %d bytes\n",fFrameSize);
        count++;
    }
    else
    {
        if(count == 1010)
        {
            fclose(h264File);
            LOGD("close /mnt/sd0/h264IFrame.264\n");
        }
    }
    LOGD("count = %d\n",count);

#endif
    }
    
    FramedSource::afterGetting(this);
}

