/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: Jpeg HardWare Decode
 *
 * Author:
 *        devin.zhu <devin.zhu@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  09/21/2017 init
 */

#ifndef IPU_G1JDEC_H
#define IPU_G1JDEC_H
#include <IPU.h>
#include "jpegdecapi.h"
#include "ppapi.h"

typedef struct dec_prams{
    int s32Width;
    int s32Height;
    int s32Format;
    int s32YUVSize;
    int s32LumSize;
} ST_DEC_PARAMS;
class IPU_G1JDEC:public IPU
{
    private:
        enum
        {
            Auto = 0,
            Trigger
        };
        JpegDecInst pJpegDecInst;
        JpegDecRet enJpegDecRet;
        JpegDecImageInfo stImageInfo;
        JpegDecInput stJpegDecIn;
        JpegDecOutput stJpegDecOut;
        PPInst pPPInst;
        PPConfig pPPConf;
        PPResult enPPDecRet;
        DWLLinearMem_t stMemIn;
        FRBuffer *stJDecInFRBuf, *stJDecOutFRBuf;
        Buffer stBufferIn, stBufferOut;
        ST_DEC_PARAMS stParamsIn, stParamsOut;
        struct fr_buf_info  stJDecInBuffer;
        Port *pIn, *pOut;

        bool bTrigger;
        bool bDecodeFinish;
        bool bPPEnble;
        int s32LumSize;
        int s32ChromaSize;
        int s32ResChange;
        int Mode;
        long long s64FrameCount;
        int s32InfoFlag;

        void CalcSize();
        int AllocLinearOutBuffer();
        JpegDecRet StartJpegDecode();
        PPResult PPDecInit(PPInst *pPPInst);
        PPResult PPDecUpdateConfig();
    public:
        IPU_G1JDEC(std::string, Json *);
        ~IPU_G1JDEC();
        void Prepare();
        void Unprepare();
        void WorkLoop();
        int UnitControl(std::string, void *);
        bool DecodePhoto(bool* bEnable);
        bool DecodeFinish(bool* bFinish);

};

#endif
