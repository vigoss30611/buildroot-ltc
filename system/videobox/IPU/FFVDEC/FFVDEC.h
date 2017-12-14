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

#ifndef IPU_FFVDEC_H
#define IPU_FFVDEC_H

#include <IPU.h>
#include <semaphore.h>
extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavutil/frame.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/opt.h>
}

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef int int32_t;
enum
{
    FLAGS         = 0x40080100
};

#define READUV(U,V) (tables[256 + (U)] + tables[512 + (V)])
//#define READY(Y)    tables[Y]
#define FIXUP(Y)  \
    do { \
        int tmp = (Y) & FLAGS; \
        if (tmp != 0){ \
            tmp  -= tmp>>8; \
            (Y)  |= tmp; \
            tmp   = FLAGS & ~(Y>>1); \
            (Y)  += tmp>>8; \
        } \
    } while (0 == 1)

#define STORE(Y,DSTPTR) \
    do { \
            (DSTPTR) = (Y & 0xFF) | (0xFF00 & (Y>>14)) | (0xFF0000 & (Y<<5));\
    }while (0 == 1)
#define MAX_BUFFERS_COUNT 6

class IPU_FFVDEC : public IPU{
    private:
        FRBuffer* decBuf;
        FRBuffer* m_pOutFrBuf[MAX_BUFFERS_COUNT];
        Buffer m_pOutBuf[MAX_BUFFERS_COUNT];
        bool Headerflag;
        bool m_bH265;
        uint8_t extra[32];
        char *StreamHeader;
        int StreamHeaderLen;
        int m_s32OutBufNum;
        long FfvdecFrameCnt;
        AVPacket FfvdecAvpkt;
        AVCodec *FfvdecAvcodec;
        AVFrame *FfvdecAvframe;
        AVCodecContext *FfvdecAvContext;
        int InitBuffer(int s32Width, int s32Height);
        void DeinitBuffer();

    public:
        IPU_FFVDEC(std::string name, Json *js);
        ~IPU_FFVDEC();
        void Prepare();
        void Unprepare();
        void WorkLoop();
        void InitFfVdecParam();
        bool SetStreamHeader(struct v_header_info *strhdr);
        void YUV420p_to_RGBA8888(uint8_t **src, uint8_t *dst, int width, int height);
        int UnitControl(std::string, void *);
};
#endif

