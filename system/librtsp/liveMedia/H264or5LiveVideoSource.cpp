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

////////// H264or5LiveVideoSource //////////

#include "H264or5LiveVideoSource.hh"
#include <qsdk/video.h>


H264or5LiveVideoSource*
H264or5LiveVideoSource::createNew(UsageEnvironment& env,const char * channelID ) {
  return new H264or5LiveVideoSource(env,channelID);
}

H264or5LiveVideoSource::H264or5LiveVideoSource(UsageEnvironment& env,const char * channelID)
  : FramedSource(env){
    fChanID = strDup(channelID);
    env << "H264LiveVideoSource::fChanID:" << fChanID <<"\n";
    fHasTriggerKeyFrame = False;
    fr_INITBUF(&fFrBuf, NULL);
    fSendHeaderCount = 1;
    fTruncatedBytes = new char[VIDEO_BUF_SIZE];
    fTruncatedBytesNum = 0;
}

H264or5LiveVideoSource::~H264or5LiveVideoSource() {
    delete[] fChanID;
    delete[] fTruncatedBytes;
}

void H264or5LiveVideoSource::doGetNextFrame() {
    if(strlen(fChanID) == 0)
    {
        LOGE("fChanID is NULL\n");
        return;
    }

    if(fSendHeaderCount > 0)
    {
        char h264Header[128];
        int len = video_get_header(fChanID,h264Header,128);
        memmove(fTo,h264Header,len);
        fFrameSize = len;
        fSendHeaderCount--;
    }
    else
    {
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
                    memmove(fTo,fFrBuf.virt_addr,fFrBuf.size);
                }
            }
            
            LOGD("video timestamp %llu,timestamp:%u:%u\n",fFrBuf.timestamp,fPresentationTime.tv_sec,fPresentationTime.tv_usec/1000);

            video_put_frame(fChanID, &fFrBuf);
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
 
    FramedSource::afterGetting(this);
}

