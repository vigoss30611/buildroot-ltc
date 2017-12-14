// Copyright (C) 2016 InfoTM, yong.yan@infotm.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef IPU_JENC_H
#define IPU_JENC_H

#include <IPU.h>
#include <JpegEncApi.h>

#ifdef MACRO_JENC_SWSCALE
#include <qsdk/jpegutils.h>
#pragma message "jenc support thumbnail on"
#define COMPILE_OPT_JPEG_SWSCALE
#else
#pragma message "jenc support thumbnail off"
#endif

#define INTRA 1
#define EXTRA 2

class IPU_JENC : public IPU {
    private:
        ST_VENC_INFO m_stVencInfo;
        JpegEncCfg m_stThumbJpegEncCfg;
        struct fr_buf_info m_stThumbnailInputBuffer;
        struct fr_buf_info m_stThumbnailEncodedBuffer;
        JpegEncThumb m_stJpegThumb;
        pthread_mutex_t m_stUpdateMut;
        vbres_t m_stThumbResolution;
        int frameSerial;
        int m_s32ThumbnailWidth;
        int m_s32ThumbnailHeight;
        int m_s32ThumbEncBufSize;
        int m_s32ThumbnailMode;
        int m_s32ThumbEncSize;
        unsigned int m_u32XmpDataMaxLength;
        bool m_s32ThumbnailEnable;
        bool m_bApp1Enable;
        #if TIMEUSAGE
        struct timeval tmFrameStart, tmFrameEnd;
        #endif
        bool qualityFlag;
        JpegEncRet EncThumnail(Buffer *pstInBuf);
        int GetThumbnail();
        void CheckThumbnailRes();
    public:
        enum {
            Auto = 0,
            Trigger
        };

        JpegEncInst pJpegEncInst;
        JpegEncCfg pJpegEncCfg;
        JpegEncIn pJpegEncIn;
        JpegEncOut pJpegEncOut;
        JpegEncRet enJencRet;
        int Mode;
        int Triggerred;
        int SnapSerial;
        int sliceCount;

        IPU_JENC(std::string, Json *);
        void Prepare();
        void Unprepare();
        void WorkLoop();
        int UnitControl(std::string, void*);

        int SetJpegEncDefaultCfgParams();
        int StartJpegEncode(Buffer *InBuf, Buffer *OutBuf);

        bool SetQuality(int Qlevel);
        void ParamCheck();
        void TriggerSnap();
        void SetTrigger();
        int GetTrigger();
        void ClearTrigger();
        int CheckEncCfg(JpegEncCfg *pJpegEncCfg);
        int SetSnapParams(struct v_pip_info * arg);
        int GetDebugInfo(void *pInfo, int *ps32Size);
};


#endif
