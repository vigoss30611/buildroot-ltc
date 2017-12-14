/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: wapper header file
 *
 * Author:
 *     beca.zhang <beca.zhang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/29/2017 init
 */
#ifndef VENCODER_WRAPPER_H
#define VENCODER_WRAPPER_H

#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
#include <hevcencapi.h>
#define T265 (encode->tH265)
#endif

#ifdef VENCODER_DEPEND_H1V6
#include <h264encapi.h>
#include <h264encapi_ext.h>
#define T264 (encode->tH264)
#endif

typedef unsigned char u8;
typedef signed char i8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;
typedef signed int i32;
typedef unsigned long long u64;

typedef enum
{
    VENC_OK = 0,
    VENC_FRAME_READY = 1,
    VENC_ERROR = -1,
    VENC_NULL_ARGUMENT = -2,
    VENC_INVALID_ARGUMENT = -3,
    VENC_MEMORY_ERROR = -4,
    VENC_EWL_ERROR = -5,
    VENC_EWL_MEMORY_ERROR = -6,
    VENC_INVALID_STATUS = -7,
    VENC_OUTPUT_BUFFER_OVERFLOW = -8,
    VENC_HW_BUS_ERROR = -9,
    VENC_HW_DATA_ERROR = -10,
    VENC_HW_TIMEOUT = -11,
    VENC_HW_RESERVED = -12,
    VENC_SYSTEM_ERROR = -13,
    VENC_INSTANCE_ERROR = -14,
    VENC_HRD_ERROR = -15,
    VENC_HW_RESET = -16
} VEncRet;
typedef enum
{
    VENC_YUV420_PLANAR = 0,
    VENC_YUV420_SEMIPLANAR,
    VENC_YUV420_SEMIPLANAR_VU,
    VENC_YUV422_INTERLEAVED_YUYV,
    VENC_YUV422_INTERLEAVED_UYVY,
    VENC_RGB565,
    VENC_BGR565,
    VENC_RGB555,
    VENC_BGR555,
    VENC_RGB444,
    VENC_BGR444,
    VENC_RGB888,
    VENC_BGR888,
    VENC_RGB101010,
    VENC_BGR101010
} VEncPictureType;

typedef struct
{
#ifdef VENCODER_DEPEND_H1V6
    H264EncStreamType H264StreamType; //default 264 265 need convert
    H264EncViewMode viewMode; //h1
    H264EncLevel H264level; //not same with h2v1
#endif
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
    HEVCEncStreamType HEVCStreamType; //h2v1
    HEVCEncLevel HEVClevel; //h2v1
#endif
#ifdef VENCODER_DEPEND_H2V4
    HEVCEncProfile profile; //h2v4
#endif
    u32 width;
    u32 height;
    u32 frameRateNum;
    u32 frameRateDenom;
    u32 scaledWidth; //h1
    u32 scaledHeight; //h1
    u32 refFrameAmount;
    u32 constrainedIntraPrediction; //h2v1
    u32 strongIntraSmoothing; //h2v1
    u32 sliceSize; //h2v1
    u32 compressor; //h2v1
    u32 interlacedFrame; //h2v1
    u32 linkMode; //h1
    i32 chromaQpIndexOffset; //h1
    u32 bitDepthLuma; //h2v4
    u32 bitDepthChroma; //h2v4
    u32 maxTLayers; //h2v4
} ST_EncConfig;

typedef struct
{
    u32 sliceSize;
    u32 seiMessages;
    u32 videoFullRange;
    u32 constrainedIntraPrediction; //h1
    u32 disableDeblockingFilter;
    i32 tc_Offset; //h2v1
    i32 beta_Offset; //h2v1
    u32 enableDeblockOverride; //h2v1
    u32 deblockOverride; //h2v1
    u32 enableSao; //h2v1
    u32 enableScalingList; //h2v1
    u32 sampleAspectRatioWidth;
    u32 sampleAspectRatioHeight;
    u32 enableCabac; //h1
    u32 cabacInitIdc; //h1
    u32 cabacInitFlag; //h2v1
    u32 transform8x8Mode; //h1
    u32 quarterPixelMv; //h1
    u32 cirStart;
    u32 cirInterval;
    u32 intraSliceMap1; //h1
    u32 intraSliceMap2; //h1
    u32 intraSliceMap3; //h1
#ifdef VENCODER_DEPEND_H1V6
    H264EncPictureArea H264IntraArea; //h1
    H264EncPictureArea H264Roi1Area; //h1
    H264EncPictureArea H264Roi2Area; //h1
#endif
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
    HEVCEncPictureArea HEVCIntraArea; //h2
    HEVCEncPictureArea HEVCRoi1Area; //h2
    HEVCEncPictureArea HEVCRoi2Area; //h2
#endif
    i32 roi1DeltaQp;
    i32 roi2DeltaQp;
    i32 max_cu_size; //h2v1
    i32 min_cu_size; //h2v1
    i32 max_tr_size; //h2v1
    i32 min_tr_size; //h2v1
    i32 tr_depth_intra; //h2v1
    i32 tr_depth_inter; //h2v1
    i32 adaptiveRoi; //h1
    i32 adaptiveRoiColor; //h1
    u32 fieldOrder;
    i32 m_s32ChromaQpOffset; //h2v1
    u32 roiMapDeltaQpEnable; //h2v4
    u32 roiMapDeltaQpBlockUnit; //h2v4
    u32 noiseReductionEnable; //h2v4
    u32 noiseLow; //h2v4
    u32 firstFrameSigma; //h2v4
    u32 gdrDuration; //h2v4
    u32 insertrecoverypointmessage; //h2v1
    u32 recoverypointpoc; //h2v1
} ST_EncCodingCtrl;

typedef struct
{
    u32 pictureRc;
    u32 mbRc; //h1
    //u32 ctbRc; //h2v4
    u32 blockRCSize; //h2v4
    u32 pictureSkip;
    i32 qpHdr;
    u32 qpMin;
    u32 qpMax;
    u32 bitPerSecond;
    u32 hrd;
    u32 hrdCpbSize;
    u32 gopLen;
    i32 intraQpDelta;
    u32 fixedIntraQp;
    i32 bitVarRangeI; //h2v4
    i32 bitVarRangeP; //h2v4
    i32 bitVarRangeB; //h2v4
    i32 tolMovingBitRate; //h2v4
    i32 monitorFrames; //h2v4
    i32 mbQpAdjustment; //h1
    i32 longTermPicRate; //h1
    i32 mbQpAutoBoost; //h1
    u32 u32NewStream;
} ST_EncRateCtrl;

typedef struct
{
    u32 busLuma;
    u32 busChromaU;
    u32 busChromaV;
    u32 timeIncrement;
    u32 *pOutBuf;
    u32 busOutBuf;
    u32 outBufSize;
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
    HEVCEncPictureCodingType HEVCCodingType; //h2v1
#endif
    u32 busLumaStab; //h1
#ifdef VENCODER_DEPEND_H1V6
    H264EncPictureCodingType H264CodingType; //h1
    H264EncRefPictureMode ipf; //h1
    H264EncRefPictureMode ltrf; //h1
#endif
    i32 poc; //h2v1
#ifdef VENCODER_DEPEND_H2V4
    HEVCGopConfig gopConfig; //h2v4
#endif
    i32 gopSize; //h2v4
    i32 gopPicIdx; //h2v4
    u32 roiMapDeltaQpAddr; //h2v4
} ST_EncIn;

typedef struct
{
#ifdef VENCODER_DEPEND_H1V6
    H264EncPictureCodingType H264CodingType; //h1
    H264EncRefPictureMode ipf; //h1
    H264EncRefPictureMode ltrf; //h1
#endif
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
    HEVCEncPictureCodingType HEVCCodingType; //h2v1
#endif
    u32 streamSize;
    i8 *motionVectors; //h1
    u32 *pNaluSizeBuf;
    u32 numNalus;
    u32 mse_mul256;  //h1
    u32 busScaledLuma;
    u8 *scaledPicture;
} ST_EncOut;

typedef struct
{
    u32 origWidth;
    u32 origHeight;
    u32 xOffset;
    u32 yOffset;
#ifdef VENCODER_DEPEND_H1V6
    H264EncPictureType H264InputType;
    H264EncPictureRotation H264Rotation;
    H264EncColorConversion H264colorConversion;
#endif
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
    HEVCEncPictureType HEVCInputType;
    HEVCEncPictureRotation HEVCRotation;
    HEVCEncColorConversion HEVCcolorConversion; //h2v1
#endif
    u32 videoStabilization;
    u32 scaledWidth; //h2v1
    u32 scaledHeight; //h2v1
    u32 scaledOutput;
    u32 *virtualAddressScaledBuff; //h2v1
    u32 busAddressScaledBuff; //h2v1
    u32 sizeScaledBuff; //h2v1
    u32 interlacedFrame;
} ST_EncPreProcessingCfg;

template<
class TEncRet,
      class TEncInst,
      class TEncIn,
      class TEncOut,
      class TEncConfig,
      class TEncCodingCtrl,
      class TEncRateCtrl,
      class TEncPreProcessingCfg>
      class TEncode
{
    public:
        TEncRet EncRet;
        TEncInst EncInst;
        TEncIn EncIn;
        TEncOut EncOut;
        TEncConfig EncConfig;
        TEncCodingCtrl EncCodingCtrl;
        TEncRateCtrl EncRateCtrl;
        TEncPreProcessingCfg EncPreProcessingCfg;
};

#ifdef VENCODER_DEPEND_H1V6
typedef TEncode<H264EncRet, H264EncInst, H264EncIn, H264EncOut, H264EncConfig,
        H264EncCodingCtrl, H264EncRateCtrl, H264EncPreProcessingCfg> TH264;
#endif
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
typedef TEncode<HEVCEncRet, HEVCEncInst, HEVCEncIn, HEVCEncOut, HEVCEncConfig,
        HEVCEncCodingCtrl, HEVCEncRateCtrl, HEVCEncPreProcessingCfg> TH265;
#endif

typedef union
{
#ifdef VENCODER_DEPEND_H1V6
    TH264 tH264;
#endif
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
    TH265 tH265;
#endif
} U_ENCODE;

class bENCODE
{
    public:
        virtual VEncRet VEncInitOn(U_ENCODE *encode){return (VEncRet)0;}
        virtual VEncRet VEncInit(U_ENCODE *encode){return (VEncRet)0;}
        virtual VEncRet VEncGetCodingCtrl(U_ENCODE *encode, ST_EncCodingCtrl *codingctrl){return (VEncRet)0;}
        virtual VEncRet VEncSetCodingCtrl(U_ENCODE *encode){return (VEncRet)0;}
        virtual VEncRet VEncGetRateCtrl(U_ENCODE *encode, ST_EncRateCtrl *ratectrl){return (VEncRet)0;}
        virtual VEncRet VEncSetRateCtrl(U_ENCODE *encode){return (VEncRet)0;}
        virtual VEncRet VEncOn(U_ENCODE *encode){return (VEncRet)0;}
        virtual VEncRet VEncStrmStart(U_ENCODE *encode, ST_EncOut *out){return (VEncRet)0;}
        virtual VEncRet VEncStrmEncode(U_ENCODE *encode, ST_EncOut *out){return (VEncRet)0;}
        virtual VEncRet VEncOff(U_ENCODE *encode){return (VEncRet)0;}
        virtual VEncRet VEncStrmEnd(U_ENCODE *encode){return (VEncRet)0;}
        virtual VEncRet VEncRelease(U_ENCODE *encode){return (VEncRet)0;}
        virtual VEncRet VEncGetPreProcessing(U_ENCODE *encode, ST_EncPreProcessingCfg *EncPreProCfg){return (VEncRet)0;}
        virtual VEncRet VEncSetPreProcessing(U_ENCODE *encode){return (VEncRet)0;}
        virtual VEncRet VEncLinkMode(U_ENCODE *encode, ST_EncConfig *config){return (VEncRet)0;}
        virtual VEncRet VEncSetSeiUserData(U_ENCODE *encode, const u8 *pu8UserData, u32 u32UserDataSize){return (VEncRet)0;}
        virtual VEncRet VEncCalculateInitialQp(U_ENCODE *encode, i32 *s32QpHdr, u32 u32Bitrate){return (VEncRet)0;}
        virtual VEncRet VEncGetRealQp(U_ENCODE *encode, i32 *s32QpHdr){return (VEncRet)0;}

        virtual bool IsInstReady(U_ENCODE *encode){return true;}

        //set variable
        virtual void SetEncConfig(U_ENCODE *encode, ST_EncConfig *config){}
        virtual void SetEncCodingCtrl(U_ENCODE *encode, ST_EncCodingCtrl *codingctrl){}
        virtual void SetEncRateCtrl(U_ENCODE *encode, ST_EncRateCtrl *ratectrl){}
        virtual void SetEncIn(U_ENCODE *encode, ST_EncIn *in){}
        virtual void SetEncOut(U_ENCODE *encode, ST_EncOut *out){}
        virtual void SetPreProcessingCfg(U_ENCODE *encode, ST_EncPreProcessingCfg *EncPreProCfg){}
};

class dH264 : public bENCODE
{
    public:
        VEncRet VEncInitOn(U_ENCODE *encode);
        VEncRet VEncInit(U_ENCODE *encode);
        VEncRet VEncGetCodingCtrl(U_ENCODE *encode, ST_EncCodingCtrl *codingctrl);
        VEncRet VEncSetCodingCtrl(U_ENCODE *encode);
        VEncRet VEncGetRateCtrl(U_ENCODE *encode, ST_EncRateCtrl *ratectrl);
        VEncRet VEncSetRateCtrl(U_ENCODE *encode);
        VEncRet VEncOn(U_ENCODE *encode);
        VEncRet VEncStrmStart(U_ENCODE *encode, ST_EncOut *out);
        VEncRet VEncStrmEncode(U_ENCODE *encode, ST_EncOut *out);
        VEncRet VEncOff(U_ENCODE *encode);
        VEncRet VEncStrmEnd(U_ENCODE *encode);
        VEncRet VEncRelease(U_ENCODE *encode);
        VEncRet VEncGetPreProcessing(U_ENCODE *encode, ST_EncPreProcessingCfg *EncPreProCfg);
        VEncRet VEncSetPreProcessing(U_ENCODE *encode);
        VEncRet VEncLinkMode(U_ENCODE *encode, ST_EncConfig *config);
        VEncRet VEncSetSeiUserData(U_ENCODE *encode, const u8 *pu8UserData, u32 u32UserDataSize);
        VEncRet VEncCalculateInitialQp(U_ENCODE *encode, i32 *s32QpHdr, u32 u32Bitrate);
        VEncRet VEncGetRealQp(U_ENCODE *encode, i32 *s32QpHdr);
        bool IsInstReady(U_ENCODE *encode);
        void SetEncConfig(U_ENCODE *encode, ST_EncConfig *config);
        void SetEncCodingCtrl(U_ENCODE *encode, ST_EncCodingCtrl *codingctrl);
        void SetEncRateCtrl(U_ENCODE *encode, ST_EncRateCtrl *ratectrl);
        void SetEncIn(U_ENCODE *encode, ST_EncIn *in);
        void SetEncOut(U_ENCODE *encode, ST_EncOut *out);
        void SetPreProcessingCfg(U_ENCODE *encode, ST_EncPreProcessingCfg *EncPreProCfg);
};

class dH265 : public bENCODE
{
    public:
        VEncRet VEncInitOn(U_ENCODE *encode);
        VEncRet VEncInit(U_ENCODE *encode);
        VEncRet VEncGetCodingCtrl(U_ENCODE *encode, ST_EncCodingCtrl *codingctrl);
        VEncRet VEncSetCodingCtrl(U_ENCODE *encode);
        VEncRet VEncGetRateCtrl(U_ENCODE *encode, ST_EncRateCtrl *ratectrl);
        VEncRet VEncSetRateCtrl(U_ENCODE *encode);
        VEncRet VEncOn(U_ENCODE *encode);
        VEncRet VEncStrmStart(U_ENCODE *encode, ST_EncOut *out);
        VEncRet VEncStrmEncode(U_ENCODE *encode, ST_EncOut *out);
        VEncRet VEncOff(U_ENCODE *encode);
        VEncRet VEncStrmEnd(U_ENCODE *encode);
        VEncRet VEncRelease(U_ENCODE *encode);
        VEncRet VEncGetPreProcessing(U_ENCODE *encode, ST_EncPreProcessingCfg *EncPreProCfg);
        VEncRet VEncSetPreProcessing(U_ENCODE *encode);
        VEncRet VEncLinkMode(U_ENCODE *encode, ST_EncConfig *config);
        VEncRet VEncSetSeiUserData(U_ENCODE *encode, const u8 *pu8UserData, u32 u32UserDataSize);
        VEncRet VEncCalculateInitialQp(U_ENCODE *encode, i32 *s32QpHdr, u32 u32Bitrate);
        VEncRet VEncGetRealQp(U_ENCODE *encode, i32 *s32QpHdr);
        bool IsInstReady(U_ENCODE *encode);
        void SetEncConfig(U_ENCODE *encode, ST_EncConfig *config);
        void SetEncCodingCtrl(U_ENCODE *encode, ST_EncCodingCtrl *codingctrl);
        void SetEncRateCtrl(U_ENCODE *encode, ST_EncRateCtrl *ratectrl);
        void SetEncIn(U_ENCODE *encode, ST_EncIn *in);
        void SetEncOut(U_ENCODE *encode, ST_EncOut *out);
        void SetPreProcessingCfg(U_ENCODE *encode, ST_EncPreProcessingCfg *EncPreProCfg);
};
#endif //VENCODER_WRAPPER_H
