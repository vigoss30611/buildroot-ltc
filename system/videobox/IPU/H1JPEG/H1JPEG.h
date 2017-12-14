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

#ifndef IPU_H1_H
#define IPU_H1_H

#include <IPU.h>
#include <jpegencapi.h>


#ifdef MACRO_H1JPEG_SWSCALE
#include <qsdk/jpegutils.h>
#pragma message "h1jpeg swscale on"
#define COMPILE_OPT_JPEG_SWSCALE
#else
#pragma message "h1jpeg swscale off"
#endif

#ifdef MACRO_H1JPEG_CROP
//TODO Please extract crop function into macro define
#include <qsdk/jpegutils.h>
#pragma message "h1jpeg crop on"
#define COMPILE_OPT_JPEG_CROP
#else
#pragma message "h1jpeg crop off"
#endif

#define TIMEUSAGE 0
#define INTRA 1
#define EXTRA 2
#define TYPE(n)  (1 << n)

typedef enum configType {
    UPDATE_NONE = 0,
    UPDATE_THUMBRES = TYPE(0),
    UPDATE_CROPINFO = TYPE(1),
    UPDATE_QUALITY = TYPE(2)
}configType_t;

typedef enum prepost
{
    ACTION_NONE = 0,
    SOFTWARE_CROP = TYPE(0),
    SOFTWARE_SCALE = TYPE(1),
    HARDWRE_CROP = TYPE(2)
} EN_PREPOST;

typedef class excData {
    public:
            vbres_t  thumbRes;
            struct v_pip_info pipInfo;
            int qLevel;
            int flag;


}excData_t;

class IPU_H1JPEG : public IPU {
    private:
        JpegEncRet enJencRet;
        ST_VENC_INFO m_stVencInfo;
        int Rotate;
        int scaledSliceYUVSize;
        int scaledSliceHeight;
        int scaledlumSliceRowsSize;
        float heightRatio;
        int srcSliceHeight;
        int srclumSliceRowsSize;
        int srcSliceYUVSize;
        int inputPicSize;

        int frameSerial;
        int swScaledWidth;
        int swScaledHeight;
        int encodedSize;
        char *encodedPicBuffer;
        JpegEncCodingType encFrameMode;
        struct fr_buf_info scaledSliceBuffer;
        #if TIMEUSAGE
        struct timeval tmInterpolation, tmEncoded, tmCopy;
        struct timeval tmFrameStart, tmFrameEnd;
        #endif
        int xOffset, yOffset, cropWidth, cropHeight;
        bool qualityFlag;
        struct fr_buf_info backupBuf;
        bool hasBackupBuf;
        bool thumbnailEnable;
        bool m_bApp1Enable;
        unsigned int m_u32XmpDataMaxLength;
        int thumbnailMode;
        JpegEncThumb jpegThumb;
        int thumbnailHeight;
        int thumbnailWidth;
        int s32ThumbEncSize;
        struct fr_buf_info thumbnailInputBuffer;
        struct fr_buf_info thumbnailEncodedBuffer;
        int thumbEncBufSize;
        JpegEncCfg thumbJpegEncCfg;
        excData_t configData;
        int outBufferRes;
        char *thumbCropBuffer;
        int m_s32PrePost;
        char *m_ps8MainCropBuffer;
        pthread_mutex_t updateMut;

        int AllocBackupBuf();
        int FreeBackupBuf();
        int CopyToBackupBuf(Buffer *from, Buffer *to);
        int updateSetting();
    public:
        enum {
            Auto = 0,
            Trigger
        };

        JpegEncInst pJpegEncInst;
        JpegEncCfg pJpegEncCfg;
        JpegEncIn pJpegEncIn;
        JpegEncOut pJpegEncOut;
        int Mode;
        int Triggerred;
        int SnapSerial;
        int sliceCount;

        IPU_H1JPEG(std::string, Json *);
        void Prepare();
        void Unprepare();
        void WorkLoop();
        int UnitControl(std::string, void*);

        int SetJpegEncDefaultCfgParams();
        int StartJpegEncode(Buffer *InBuf, Buffer *OutBuf);
        int StartJpegSliceEncode(Buffer *InBuf, Buffer *OutBuf);

        int SetQuality(int Qlevel);
        void ParamCheck();
        void TriggerSnap();
        void SetTrigger();
        int GetTrigger();
        void ClearTrigger();
        int CheckEncCfg(JpegEncCfg *pJpegEncCfg);
        int SetSnapParams(struct v_pip_info * arg);
        int GetThumbnail();
        JpegEncRet EncThumnail(Buffer *InBuf);
        int SetPipInfo(struct v_pip_info *vpinfo);
        int SetThumbnailRes(vbres_t *res);
        int GetDebugInfo(void *pInfo, int *ps32Size);
};


#endif
