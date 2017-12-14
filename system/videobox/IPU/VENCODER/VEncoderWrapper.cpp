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
 * Description: wrapper of h1v6 h2v1 h2v4
 *
 * Author:
 *     beca.zhang <beca.zhang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/24/2017 init
 */
#include "VEncoderWrapper.h"

#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
VEncRet dH265::VEncInitOn(U_ENCODE *encode)
{
    VEncRet ret = VENC_OK;
#ifdef VENCODER_DEPEND_H2V1
    ret = (VEncRet)HEVCEncInitOn();
#endif
    return ret;
}
VEncRet dH265::VEncInit(U_ENCODE *encode)
{
    VEncRet ret = (VEncRet)HEVCEncInit(&T265.EncConfig, &T265.EncInst);
    return ret;
}
VEncRet dH265::VEncGetCodingCtrl(U_ENCODE *encode, ST_EncCodingCtrl *codingctrl)
{
    VEncRet ret = (VEncRet)HEVCEncGetCodingCtrl(T265.EncInst, &T265.EncCodingCtrl);
    codingctrl->sliceSize = T265.EncCodingCtrl.sliceSize;
    codingctrl->seiMessages = T265.EncCodingCtrl.seiMessages;
    codingctrl->videoFullRange = T265.EncCodingCtrl.videoFullRange;
    codingctrl->disableDeblockingFilter = T265.EncCodingCtrl.disableDeblockingFilter;
    codingctrl->tc_Offset = T265.EncCodingCtrl.tc_Offset;  //h2v1
    codingctrl->beta_Offset = T265.EncCodingCtrl.beta_Offset;  //h2v1
    codingctrl->enableDeblockOverride = T265.EncCodingCtrl.enableDeblockOverride;  //h2v1
    codingctrl->deblockOverride = T265.EncCodingCtrl.deblockOverride;  //h2v1
    codingctrl->enableSao = T265.EncCodingCtrl.enableSao;  //h2v1
    codingctrl->enableScalingList = T265.EncCodingCtrl.enableScalingList;  //h2v1
    codingctrl->sampleAspectRatioWidth = T265.EncCodingCtrl.sampleAspectRatioWidth;
    codingctrl->sampleAspectRatioHeight = T265.EncCodingCtrl.sampleAspectRatioHeight;
    codingctrl->cabacInitFlag = T265.EncCodingCtrl.cabacInitFlag;  //h2v1
    codingctrl->cirStart = T265.EncCodingCtrl.cirStart;
    codingctrl->cirInterval = T265.EncCodingCtrl.cirInterval;
    codingctrl->HEVCIntraArea = T265.EncCodingCtrl.intraArea;  //h2v1
    codingctrl->HEVCRoi1Area = T265.EncCodingCtrl.roi1Area;  //h2v1
    codingctrl->HEVCRoi2Area = T265.EncCodingCtrl.roi2Area;  //h2v1
    codingctrl->roi1DeltaQp = T265.EncCodingCtrl.roi1DeltaQp;
    codingctrl->roi2DeltaQp = T265.EncCodingCtrl.roi2DeltaQp;
#ifdef VENCODER_DEPEND_H2V1
    codingctrl->max_cu_size = T265.EncCodingCtrl.max_cu_size;  //h2v1
    codingctrl->min_cu_size = T265.EncCodingCtrl.min_cu_size;  //h2v1
    codingctrl->max_tr_size = T265.EncCodingCtrl.max_tr_size;  //h2v1
    codingctrl->min_tr_size = T265.EncCodingCtrl.min_tr_size;  //h2v1
    codingctrl->tr_depth_intra = T265.EncCodingCtrl.tr_depth_intra;  //h2v1
    codingctrl->tr_depth_inter = T265.EncCodingCtrl.tr_depth_inter;  //h2v1
    codingctrl->insertrecoverypointmessage = T265.EncCodingCtrl.insertrecoverypointmessage;  //h2v1
    codingctrl->recoverypointpoc = T265.EncCodingCtrl.recoverypointpoc;  //h2v1
#endif
    codingctrl->fieldOrder = T265.EncCodingCtrl.fieldOrder;
    codingctrl->m_s32ChromaQpOffset = T265.EncCodingCtrl.chroma_qp_offset;  //h2v1
#ifdef VENCODER_DEPEND_H2V4
    codingctrl->roiMapDeltaQpEnable = T265.EncCodingCtrl.roiMapDeltaQpEnable;  //h2v4
    codingctrl->roiMapDeltaQpBlockUnit = T265.EncCodingCtrl.roiMapDeltaQpBlockUnit;  //h2v4
    codingctrl->noiseReductionEnable = T265.EncCodingCtrl.noiseReductionEnable;  //h2v4
    codingctrl->noiseLow = T265.EncCodingCtrl.noiseLow;  //h2v4
    codingctrl->firstFrameSigma = T265.EncCodingCtrl.firstFrameSigma;  //h2v4
    codingctrl->gdrDuration = T265.EncCodingCtrl.gdrDuration;  //h2v4
#endif
    return ret;
}
VEncRet dH265::VEncSetCodingCtrl(U_ENCODE *encode)
{
    VEncRet ret = (VEncRet)HEVCEncSetCodingCtrl(T265.EncInst, &T265.EncCodingCtrl);
    return ret;
}
VEncRet dH265::VEncGetRateCtrl(U_ENCODE *encode, ST_EncRateCtrl *ratectrl)
{
    VEncRet ret = (VEncRet)HEVCEncGetRateCtrl(T265.EncInst, &T265.EncRateCtrl);
    ratectrl->pictureRc = T265.EncRateCtrl.pictureRc;
#ifdef VENCODER_DEPEND_H2V4
    ratectrl->mbRc = T265.EncRateCtrl.ctbRc;  //h2v4
    ratectrl->blockRCSize = T265.EncRateCtrl.blockRCSize;  //h2v4
#endif
    ratectrl->pictureSkip = T265.EncRateCtrl.pictureSkip;
    ratectrl->qpHdr = T265.EncRateCtrl.qpHdr;
    ratectrl->qpMin = T265.EncRateCtrl.qpMin;
    ratectrl->qpMax = T265.EncRateCtrl.qpMax;
    ratectrl->bitPerSecond = T265.EncRateCtrl.bitPerSecond;
    ratectrl->hrd = T265.EncRateCtrl.hrd;
    ratectrl->hrdCpbSize = T265.EncRateCtrl.hrdCpbSize;
    ratectrl->gopLen = T265.EncRateCtrl.gopLen;
    ratectrl->intraQpDelta = T265.EncRateCtrl.intraQpDelta;
    ratectrl->fixedIntraQp = T265.EncRateCtrl.fixedIntraQp;
#ifdef VENCODER_DEPEND_H2V4
    ratectrl->bitVarRangeI = T265.EncRateCtrl.bitVarRangeI;  //h2v4
    ratectrl->bitVarRangeP = T265.EncRateCtrl.bitVarRangeP;  //h2v4
    ratectrl->bitVarRangeB = T265.EncRateCtrl.bitVarRangeB;  //h2v4
    ratectrl->tolMovingBitRate = T265.EncRateCtrl.tolMovingBitRate;  //h2v4
    ratectrl->monitorFrames = T265.EncRateCtrl.monitorFrames;  //h2v4
#endif
    return ret;
}
VEncRet dH265::VEncSetRateCtrl(U_ENCODE *encode)
{
    VEncRet ret = (VEncRet)HEVCEncSetRateCtrl(T265.EncInst, &T265.EncRateCtrl);
    return ret;
}
VEncRet dH265::VEncOn(U_ENCODE *encode)
{
    VEncRet ret = VENC_OK;
#ifdef VENCODER_DEPEND_H2V1
    ret = (VEncRet)HEVCEncOn(T265.EncInst);
#endif
    return ret;
}
VEncRet dH265::VEncStrmStart(U_ENCODE *encode, ST_EncOut *out)
{
    VEncRet ret = (VEncRet)HEVCEncStrmStart(T265.EncInst, &T265.EncIn, &T265.EncOut);
    out->HEVCCodingType = T265.EncOut.codingType;  //h2v1
    out->streamSize = T265.EncOut.streamSize;
    out->pNaluSizeBuf = T265.EncOut.pNaluSizeBuf;
    out->numNalus = T265.EncOut.numNalus;
    out->busScaledLuma = T265.EncOut.busScaledLuma;
    out->scaledPicture = T265.EncOut.scaledPicture;
    return ret;
}
VEncRet dH265::VEncStrmEncode(U_ENCODE *encode, ST_EncOut *out)
{
    VEncRet ret = (VEncRet)HEVCEncStrmEncode(T265.EncInst, &T265.EncIn, &T265.EncOut, NULL, NULL);
    out->HEVCCodingType = T265.EncOut.codingType;  //h2v1
    out->streamSize = T265.EncOut.streamSize;
    out->pNaluSizeBuf = T265.EncOut.pNaluSizeBuf;
    out->numNalus = T265.EncOut.numNalus;
    out->busScaledLuma = T265.EncOut.busScaledLuma;
    out->scaledPicture = T265.EncOut.scaledPicture;
    return ret;
}
VEncRet dH265::VEncOff(U_ENCODE *encode)
{
    VEncRet ret = VENC_OK;
#ifdef VENCODER_DEPEND_H2V1
    ret = (VEncRet)HEVCEncOff(T265.EncInst);
#endif
    return ret;
}
VEncRet dH265::VEncStrmEnd(U_ENCODE *encode)
{
    VEncRet ret = (VEncRet)HEVCEncStrmEnd(T265.EncInst, &T265.EncIn, &T265.EncOut);
    return ret;
}
VEncRet dH265::VEncRelease(U_ENCODE *encode)
{
    VEncRet ret = (VEncRet)HEVCEncRelease(T265.EncInst);
    T265.EncInst = NULL;
    return ret;
}
VEncRet dH265::VEncGetPreProcessing(U_ENCODE *encode, ST_EncPreProcessingCfg *EncPreProCfg)
{
    VEncRet ret = (VEncRet)HEVCEncGetPreProcessing(T265.EncInst, &T265.EncPreProcessingCfg);
    EncPreProCfg->origWidth = T265.EncPreProcessingCfg.origWidth;
    EncPreProCfg->origHeight = T265.EncPreProcessingCfg.origHeight;
    EncPreProCfg->xOffset = T265.EncPreProcessingCfg.xOffset;
    EncPreProCfg->yOffset = T265.EncPreProcessingCfg.yOffset;
    EncPreProCfg->HEVCInputType = T265.EncPreProcessingCfg.inputType;  //h2v1
    EncPreProCfg->HEVCRotation = T265.EncPreProcessingCfg.rotation;  //h2v1
    EncPreProCfg->HEVCcolorConversion = T265.EncPreProcessingCfg.colorConversion;  //h2v1
    EncPreProCfg->scaledWidth = T265.EncPreProcessingCfg.scaledWidth;  //h2v1
    EncPreProCfg->scaledHeight = T265.EncPreProcessingCfg.scaledHeight;  //h2v1
    EncPreProCfg->scaledOutput = T265.EncPreProcessingCfg.scaledOutput;
    EncPreProCfg->virtualAddressScaledBuff = T265.EncPreProcessingCfg.virtualAddressScaledBuff;  //h2v1
    EncPreProCfg->busAddressScaledBuff = T265.EncPreProcessingCfg.busAddressScaledBuff;  //h2v1
    EncPreProCfg->sizeScaledBuff = T265.EncPreProcessingCfg.sizeScaledBuff;  //h2v1
#ifdef VENCODER_DEPEND_H2V1
    EncPreProCfg->interlacedFrame = T265.EncPreProcessingCfg.interlacedFrame;
    EncPreProCfg->videoStabilization = T265.EncPreProcessingCfg.videoStabilization;
#endif
    return ret;
}
VEncRet dH265::VEncSetPreProcessing(U_ENCODE *encode)
{
    VEncRet ret = (VEncRet)HEVCEncSetPreProcessing(T265.EncInst, &T265.EncPreProcessingCfg);
    return ret;
}

VEncRet dH265::VEncLinkMode(U_ENCODE *encode, ST_EncConfig *config)
{
    return (VEncRet)0;
}

VEncRet dH265::VEncSetSeiUserData(U_ENCODE *encode, const u8 *pu8UserData, u32 u32UserDataSize)
{
    VEncRet ret = (VEncRet)HEVCEncSetSeiUserData(T265.EncInst, pu8UserData, u32UserDataSize);
    return ret;
}
VEncRet dH265::VEncCalculateInitialQp(U_ENCODE *encode, i32 *s32QpHdr, u32 u32Bitrate)
{
    VEncRet ret = (VEncRet)HEVCEncCalculateInitialQp(T265.EncInst, s32QpHdr, u32Bitrate);
    return ret;
}
VEncRet dH265::VEncGetRealQp(U_ENCODE *encode, i32 *s32QpHdr)
{
    VEncRet ret = (VEncRet)HEVCEncGetRealQp(T265.EncInst, s32QpHdr);
    return ret;
}

bool dH265::IsInstReady(U_ENCODE *encode)
{
    return T265.EncInst != NULL ? true : false;
}

//variable set
void dH265::SetEncConfig(U_ENCODE *encode, ST_EncConfig *config)
{
    T265.EncConfig.streamType = config->HEVCStreamType;
    T265.EncConfig.level = config->HEVClevel;
    T265.EncConfig.width = config->width;
    T265.EncConfig.height = config->height;
    T265.EncConfig.frameRateNum = config->frameRateNum;
    T265.EncConfig.frameRateDenom = config->frameRateDenom;
    T265.EncConfig.refFrameAmount = config->refFrameAmount;
#ifdef VENCODER_DEPEND_H2V1
    T265.EncConfig.constrainedIntraPrediction = config->constrainedIntraPrediction;
    T265.EncConfig.sliceSize = config->sliceSize;
#endif
    T265.EncConfig.strongIntraSmoothing = config->strongIntraSmoothing;
    T265.EncConfig.compressor = config->compressor;
    T265.EncConfig.interlacedFrame = config->interlacedFrame;
#ifdef VENCODER_DEPEND_H2V4
    T265.EncConfig.bitDepthLuma = config->bitDepthLuma; //h2v4
    T265.EncConfig.bitDepthChroma = config->bitDepthChroma; //h2v4
    T265.EncConfig.maxTLayers = config->maxTLayers; //h2v4
    T265.EncConfig.profile = config->profile; //h2v4
#endif
}

void dH265::SetEncCodingCtrl(U_ENCODE *encode, ST_EncCodingCtrl *codingctrl)
{
    T265.EncCodingCtrl.sliceSize = codingctrl->sliceSize;
    T265.EncCodingCtrl.seiMessages = codingctrl->seiMessages;
    T265.EncCodingCtrl.videoFullRange = codingctrl->videoFullRange;
    T265.EncCodingCtrl.disableDeblockingFilter = codingctrl->disableDeblockingFilter;
    T265.EncCodingCtrl.tc_Offset = codingctrl->tc_Offset; //h2v1
    T265.EncCodingCtrl.beta_Offset = codingctrl->beta_Offset; //h2v1
    T265.EncCodingCtrl.enableDeblockOverride = codingctrl->enableDeblockOverride; //h2v1
    T265.EncCodingCtrl.deblockOverride = codingctrl->deblockOverride; //h2v1
    T265.EncCodingCtrl.enableSao = codingctrl->enableSao; //h2v1
    T265.EncCodingCtrl.enableScalingList = codingctrl->enableScalingList; //h2v1
    T265.EncCodingCtrl.sampleAspectRatioWidth = codingctrl->sampleAspectRatioWidth;
    T265.EncCodingCtrl.sampleAspectRatioHeight = codingctrl->sampleAspectRatioHeight;
    T265.EncCodingCtrl.cabacInitFlag = codingctrl->cabacInitFlag; //h2v1
    T265.EncCodingCtrl.cirStart = codingctrl->cirStart;
    T265.EncCodingCtrl.cirInterval = codingctrl->cirInterval;
    T265.EncCodingCtrl.intraArea = codingctrl->HEVCIntraArea; //h2v1
    T265.EncCodingCtrl.roi1Area = codingctrl->HEVCRoi1Area; //h2v1
    T265.EncCodingCtrl.roi2Area = codingctrl->HEVCRoi2Area; //h2v1
    T265.EncCodingCtrl.roi1DeltaQp = codingctrl->roi1DeltaQp;
    T265.EncCodingCtrl.roi2DeltaQp = codingctrl->roi2DeltaQp;
#ifdef VENCODER_DEPEND_H2V1
    T265.EncCodingCtrl.max_cu_size = codingctrl->max_cu_size; //h2v1
    T265.EncCodingCtrl.min_cu_size = codingctrl->min_cu_size; //h2v1
    T265.EncCodingCtrl.max_tr_size = codingctrl->max_tr_size; //h2v1
    T265.EncCodingCtrl.min_tr_size = codingctrl->min_tr_size; //h2v1
    T265.EncCodingCtrl.tr_depth_intra = codingctrl->tr_depth_intra; //h2v1
    T265.EncCodingCtrl.tr_depth_inter = codingctrl->tr_depth_inter; //h2v1
    T265.EncCodingCtrl.insertrecoverypointmessage = codingctrl->insertrecoverypointmessage; //h2v1
    T265.EncCodingCtrl.recoverypointpoc = codingctrl->recoverypointpoc; //h2v1
#endif
    T265.EncCodingCtrl.fieldOrder = codingctrl->fieldOrder;
    T265.EncCodingCtrl.chroma_qp_offset = codingctrl->m_s32ChromaQpOffset; //h2v1
#ifdef VENCODER_DEPEND_H2V4
    T265.EncCodingCtrl.roiMapDeltaQpEnable = codingctrl->roiMapDeltaQpEnable; //h2v4
    T265.EncCodingCtrl.roiMapDeltaQpBlockUnit = codingctrl->roiMapDeltaQpBlockUnit; //h2v4
    T265.EncCodingCtrl.noiseReductionEnable = codingctrl->noiseReductionEnable; //h2v4
    T265.EncCodingCtrl.noiseLow = codingctrl->noiseLow; //h2v4
    T265.EncCodingCtrl.firstFrameSigma = codingctrl->firstFrameSigma; //h2v4
    T265.EncCodingCtrl.gdrDuration = codingctrl->gdrDuration; //h2v4
#endif
}

void dH265::SetEncRateCtrl(U_ENCODE *encode, ST_EncRateCtrl *ratectrl)
{
    T265.EncRateCtrl.pictureRc = ratectrl->pictureRc;
#ifdef VENCODER_DEPEND_H2V4
    T265.EncRateCtrl.ctbRc = ratectrl->mbRc; //h2v4
    T265.EncRateCtrl.blockRCSize = ratectrl->blockRCSize; //h2v4
#endif
    T265.EncRateCtrl.pictureSkip = ratectrl->pictureSkip;
    T265.EncRateCtrl.qpHdr = ratectrl->qpHdr;
    T265.EncRateCtrl.qpMin = ratectrl->qpMin;
    T265.EncRateCtrl.qpMax = ratectrl->qpMax;
    T265.EncRateCtrl.bitPerSecond = ratectrl->bitPerSecond;
    T265.EncRateCtrl.hrd = ratectrl->hrd;
    T265.EncRateCtrl.hrdCpbSize = ratectrl->hrdCpbSize;
    T265.EncRateCtrl.gopLen = ratectrl->gopLen;
    T265.EncRateCtrl.intraQpDelta = ratectrl->intraQpDelta;
    T265.EncRateCtrl.fixedIntraQp = ratectrl->fixedIntraQp;
#ifdef VENCODER_DEPEND_H2V4
    T265.EncRateCtrl.bitVarRangeI = ratectrl->bitVarRangeI; //h2v4
    T265.EncRateCtrl.bitVarRangeP = ratectrl->bitVarRangeP; //h2v4
    T265.EncRateCtrl.bitVarRangeB = ratectrl->bitVarRangeB; //h2v4
    T265.EncRateCtrl.tolMovingBitRate = ratectrl->tolMovingBitRate; //h2v4
    T265.EncRateCtrl.monitorFrames = ratectrl->monitorFrames; //h2v4
#endif
    T265.EncRateCtrl.u32NewStream = ratectrl->u32NewStream;
}

void dH265::SetEncIn(U_ENCODE *encode, ST_EncIn *in)
{
    T265.EncIn.busLuma = in->busLuma;
    T265.EncIn.busChromaU = in->busChromaU;
    T265.EncIn.busChromaV = in->busChromaV;
    T265.EncIn.timeIncrement = in->timeIncrement;
    T265.EncIn.pOutBuf = in->pOutBuf;
    T265.EncIn.busOutBuf = in->busOutBuf;
    T265.EncIn.outBufSize = in->outBufSize;
    T265.EncIn.codingType = in->HEVCCodingType; //h2v1
    T265.EncIn.poc = in->poc; //h2v1
#ifdef VENCODER_DEPEND_H2V4
    T265.EncIn.gopConfig = in->gopConfig; //h2v4
    T265.EncIn.gopSize = in->gopSize; //h2v4
    T265.EncIn.gopPicIdx = in->gopPicIdx; //h2v4
    T265.EncIn.roiMapDeltaQpAddr = in->roiMapDeltaQpAddr; //h2v4
#endif
}

void dH265::SetEncOut(U_ENCODE *encode, ST_EncOut *out)
{
    T265.EncOut.codingType = out->HEVCCodingType; //h2v1
    T265.EncOut.streamSize = out->streamSize;
    T265.EncOut.pNaluSizeBuf = out->pNaluSizeBuf;
    T265.EncOut.numNalus = out->numNalus;
    T265.EncOut.busScaledLuma = out->busScaledLuma;
    T265.EncOut.scaledPicture = out->scaledPicture;
}

void dH265::SetPreProcessingCfg(U_ENCODE *encode, ST_EncPreProcessingCfg *EncPreProCfg)
{
    T265.EncPreProcessingCfg.origWidth = EncPreProCfg->origWidth;
    T265.EncPreProcessingCfg.origHeight = EncPreProCfg->origHeight;
    T265.EncPreProcessingCfg.xOffset = EncPreProCfg->xOffset;
    T265.EncPreProcessingCfg.yOffset = EncPreProCfg->yOffset;
    T265.EncPreProcessingCfg.inputType = EncPreProCfg->HEVCInputType; //h2v1
    T265.EncPreProcessingCfg.rotation = EncPreProCfg->HEVCRotation; //h2v1
    T265.EncPreProcessingCfg.colorConversion = EncPreProCfg->HEVCcolorConversion; //h2v1
    T265.EncPreProcessingCfg.scaledWidth = EncPreProCfg->scaledWidth; //h2v1
    T265.EncPreProcessingCfg.scaledHeight = EncPreProCfg->scaledHeight; //h2v1
    T265.EncPreProcessingCfg.scaledOutput = EncPreProCfg->scaledOutput;
    T265.EncPreProcessingCfg.virtualAddressScaledBuff = EncPreProCfg->virtualAddressScaledBuff; //h2v1
    T265.EncPreProcessingCfg.busAddressScaledBuff = EncPreProCfg->busAddressScaledBuff; //h2v1
    T265.EncPreProcessingCfg.sizeScaledBuff = EncPreProCfg->sizeScaledBuff; //h2v1
#ifdef VENCODER_DEPEND_H2V1
    T265.EncPreProcessingCfg.interlacedFrame = EncPreProCfg->interlacedFrame;
    T265.EncPreProcessingCfg.videoStabilization = EncPreProCfg->videoStabilization;
#endif
}
#endif //(defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
#ifdef VENCODER_DEPEND_H1V6
VEncRet dH264::VEncInitOn(U_ENCODE *encode)
{
    return (VEncRet)0;
}
VEncRet dH264::VEncInit(U_ENCODE *encode)
{
    VEncRet ret = (VEncRet)H264EncInitSyn(&T264.EncConfig, &T264.EncInst);
    return ret;
}
VEncRet dH264::VEncGetCodingCtrl(U_ENCODE *encode, ST_EncCodingCtrl *codingctrl)
{
    VEncRet ret = (VEncRet)H264EncGetCodingCtrl(T264.EncInst, &T264.EncCodingCtrl);
    codingctrl->sliceSize = T264.EncCodingCtrl.sliceSize;
    codingctrl->seiMessages = T264.EncCodingCtrl.seiMessages;
    codingctrl->videoFullRange = T264.EncCodingCtrl.videoFullRange;
    codingctrl->constrainedIntraPrediction = T264.EncCodingCtrl.constrainedIntraPrediction;  //h1
    codingctrl->disableDeblockingFilter = T264.EncCodingCtrl.disableDeblockingFilter;
    codingctrl->sampleAspectRatioWidth = T264.EncCodingCtrl.sampleAspectRatioWidth;
    codingctrl->sampleAspectRatioHeight = T264.EncCodingCtrl.sampleAspectRatioHeight;
    codingctrl->enableCabac = T264.EncCodingCtrl.enableCabac;  //h1
    codingctrl->cabacInitIdc = T264.EncCodingCtrl.cabacInitIdc;  //h1
    codingctrl->transform8x8Mode = T264.EncCodingCtrl.transform8x8Mode;  //h1
    codingctrl->quarterPixelMv = T264.EncCodingCtrl.quarterPixelMv;  //h1
    codingctrl->cirStart = T264.EncCodingCtrl.cirStart;
    codingctrl->cirInterval = T264.EncCodingCtrl.cirInterval;
    codingctrl->intraSliceMap1 = T264.EncCodingCtrl.intraSliceMap1;  //h1
    codingctrl->intraSliceMap2 = T264.EncCodingCtrl.intraSliceMap2;  //h1
    codingctrl->intraSliceMap3 = T264.EncCodingCtrl.intraSliceMap3;  //h1
    codingctrl->H264IntraArea = T264.EncCodingCtrl.intraArea;  //h1
    codingctrl->H264Roi1Area = T264.EncCodingCtrl.roi1Area;  //h1
    codingctrl->H264Roi2Area = T264.EncCodingCtrl.roi2Area;  //h1
    codingctrl->roi1DeltaQp = T264.EncCodingCtrl.roi1DeltaQp;
    codingctrl->roi2DeltaQp = T264.EncCodingCtrl.roi2DeltaQp;
    codingctrl->adaptiveRoi = T264.EncCodingCtrl.adaptiveRoi;  //h1
    codingctrl->adaptiveRoiColor = T264.EncCodingCtrl.adaptiveRoiColor;  //h1
    codingctrl->fieldOrder = T264.EncCodingCtrl.fieldOrder;
    return ret;
}
VEncRet dH264::VEncSetCodingCtrl(U_ENCODE *encode)
{
    VEncRet ret = (VEncRet)H264EncSetCodingCtrl(T264.EncInst, &T264.EncCodingCtrl);
    return ret;
}
VEncRet dH264::VEncGetRateCtrl(U_ENCODE *encode, ST_EncRateCtrl *ratectrl)
{
    VEncRet ret = (VEncRet)H264EncGetRateCtrl(T264.EncInst, &T264.EncRateCtrl);
    ratectrl->pictureRc = T264.EncRateCtrl.pictureRc;
    ratectrl->mbRc = T264.EncRateCtrl.mbRc;  //h1
    ratectrl->pictureSkip = T264.EncRateCtrl.pictureSkip;
    ratectrl->qpHdr = T264.EncRateCtrl.qpHdr;
    ratectrl->qpMin = T264.EncRateCtrl.qpMin;
    ratectrl->qpMax = T264.EncRateCtrl.qpMax;
    ratectrl->bitPerSecond = T264.EncRateCtrl.bitPerSecond;
    ratectrl->hrd = T264.EncRateCtrl.hrd;
    ratectrl->hrdCpbSize = T264.EncRateCtrl.hrdCpbSize;
    ratectrl->gopLen = T264.EncRateCtrl.gopLen;
    ratectrl->intraQpDelta = T264.EncRateCtrl.intraQpDelta;
    ratectrl->fixedIntraQp = T264.EncRateCtrl.fixedIntraQp;
    ratectrl->mbQpAdjustment = T264.EncRateCtrl.mbQpAdjustment;  //h1
    ratectrl->longTermPicRate = T264.EncRateCtrl.longTermPicRate;  //h1
    ratectrl->mbQpAutoBoost = T264.EncRateCtrl.mbQpAutoBoost;  //h1
    return ret;
}
VEncRet dH264::VEncSetRateCtrl(U_ENCODE *encode)
{
    VEncRet ret = (VEncRet)H264EncSetRateCtrl(T264.EncInst, &T264.EncRateCtrl);
    return ret;
}
VEncRet dH264::VEncOn(U_ENCODE *encode)
{
    return (VEncRet)0;
}
VEncRet dH264::VEncStrmStart(U_ENCODE *encode, ST_EncOut *out)
{
    VEncRet ret = (VEncRet)H264EncStrmStartSyn(T264.EncInst, &T264.EncIn, &T264.EncOut);
    out->H264CodingType = T264.EncOut.codingType;  //h1
    out->streamSize = T264.EncOut.streamSize;
    out->motionVectors = T264.EncOut.motionVectors;  //h1
    out->pNaluSizeBuf = T264.EncOut.pNaluSizeBuf;
    out->numNalus = T264.EncOut.numNalus;
    out->mse_mul256 = T264.EncOut.mse_mul256;   //h1
    out->busScaledLuma = T264.EncOut.busScaledLuma;
    out->scaledPicture = T264.EncOut.scaledPicture;
    out->ipf = T264.EncOut.ipf;  //h1
    return ret;
}
VEncRet dH264::VEncStrmEncode(U_ENCODE *encode, ST_EncOut *out)
{
    VEncRet ret = (VEncRet)H264EncStrmEncodeSyn(T264.EncInst, &T264.EncIn, &T264.EncOut, NULL, NULL);
    out->H264CodingType = T264.EncOut.codingType;  //h1
    out->streamSize = T264.EncOut.streamSize;
    out->motionVectors = T264.EncOut.motionVectors;  //h1
    out->pNaluSizeBuf = T264.EncOut.pNaluSizeBuf;
    out->numNalus = T264.EncOut.numNalus;
    out->mse_mul256 = T264.EncOut.mse_mul256;   //h1
    out->busScaledLuma = T264.EncOut.busScaledLuma;
    out->scaledPicture = T264.EncOut.scaledPicture;
    out->ipf = T264.EncOut.ipf;  //h1
    return ret;
}
VEncRet dH264::VEncOff(U_ENCODE *encode)
{
    return (VEncRet)0;
}
VEncRet dH264::VEncStrmEnd(U_ENCODE *encode)
{
    VEncRet ret = (VEncRet)H264EncStrmEndSyn(T264.EncInst, &T264.EncIn, &T264.EncOut);
    return ret;
}
VEncRet dH264::VEncRelease(U_ENCODE *encode)
{
    VEncRet ret = (VEncRet)H264EncRelease(T264.EncInst);
    T264.EncInst = NULL;
    return ret;
}
VEncRet dH264::VEncGetPreProcessing(U_ENCODE *encode, ST_EncPreProcessingCfg *EncPreProCfg)
{
    VEncRet ret = (VEncRet)H264EncGetPreProcessingSyn(T264.EncInst, &T264.EncPreProcessingCfg);
    EncPreProCfg->origWidth = T264.EncPreProcessingCfg.origWidth;
    EncPreProCfg->origHeight = T264.EncPreProcessingCfg.origHeight;
    EncPreProCfg->xOffset = T264.EncPreProcessingCfg.xOffset;
    EncPreProCfg->yOffset = T264.EncPreProcessingCfg.yOffset;
    EncPreProCfg->H264InputType = T264.EncPreProcessingCfg.inputType;
    EncPreProCfg->H264Rotation = T264.EncPreProcessingCfg.rotation;
    EncPreProCfg->videoStabilization = T264.EncPreProcessingCfg.videoStabilization;
    EncPreProCfg->H264colorConversion = T264.EncPreProcessingCfg.colorConversion;
    EncPreProCfg->scaledOutput = T264.EncPreProcessingCfg.scaledOutput;
    EncPreProCfg->interlacedFrame = T264.EncPreProcessingCfg.interlacedFrame;
    return ret;
}
VEncRet dH264::VEncSetPreProcessing(U_ENCODE *encode)
{
    VEncRet ret = (VEncRet)H264EncSetPreProcessingSyn(T264.EncInst, &T264.EncPreProcessingCfg);
    return ret;
}

VEncRet dH264::VEncLinkMode(U_ENCODE *encode, ST_EncConfig *config)
{
    VEncRet ret = (VEncRet)H264LinkMode(T264.EncInst, config->linkMode);
    return ret;
}

VEncRet dH264::VEncSetSeiUserData(U_ENCODE *encode, const u8 *pu8UserData, u32 u32UserDataSize)
{
    VEncRet ret = (VEncRet)H264EncSetSeiUserData(T264.EncInst, pu8UserData, u32UserDataSize);
    return ret;
}
VEncRet dH264::VEncCalculateInitialQp(U_ENCODE *encode, i32 *s32QpHdr, u32 u32Bitrate)
{
    VEncRet ret = (VEncRet)H264EncCalculateInitialQp(T264.EncInst, s32QpHdr, u32Bitrate);
    return ret;
}
VEncRet dH264::VEncGetRealQp(U_ENCODE *encode, i32 *s32QpHdr)
{
    VEncRet ret = (VEncRet)H264EncGetRealQp(T264.EncInst, s32QpHdr);
    return ret;
}

bool dH264::IsInstReady(U_ENCODE *encode)
{
    return T264.EncInst != NULL ? true : false;
}

//variable set
void dH264::SetEncConfig(U_ENCODE *encode, ST_EncConfig *config)
{
    T264.EncConfig.streamType = config->H264StreamType;
    T264.EncConfig.viewMode = config->viewMode;
    T264.EncConfig.level = config->H264level;
    T264.EncConfig.width = config->width;
    T264.EncConfig.height = config->height;
    T264.EncConfig.frameRateNum = config->frameRateNum;
    T264.EncConfig.frameRateDenom = config->frameRateDenom;
    T264.EncConfig.scaledWidth = config->scaledWidth;
    T264.EncConfig.scaledHeight = config->scaledHeight;
    T264.EncConfig.refFrameAmount = config->refFrameAmount;
    T264.EncConfig.linkMode = config->linkMode;
    T264.EncConfig.chromaQpIndexOffset = config->chromaQpIndexOffset;
}

void dH264::SetEncCodingCtrl(U_ENCODE *encode, ST_EncCodingCtrl *codingctrl)
{
    T264.EncCodingCtrl.sliceSize = codingctrl->sliceSize;
    T264.EncCodingCtrl.seiMessages = codingctrl->seiMessages;
    T264.EncCodingCtrl.videoFullRange = codingctrl->videoFullRange;
    T264.EncCodingCtrl.constrainedIntraPrediction = codingctrl->constrainedIntraPrediction; //h1
    T264.EncCodingCtrl.disableDeblockingFilter = codingctrl->disableDeblockingFilter;
    T264.EncCodingCtrl.sampleAspectRatioWidth = codingctrl->sampleAspectRatioWidth;
    T264.EncCodingCtrl.sampleAspectRatioHeight = codingctrl->sampleAspectRatioHeight;
    T264.EncCodingCtrl.enableCabac = codingctrl->enableCabac; //h1
    T264.EncCodingCtrl.cabacInitIdc = codingctrl->cabacInitIdc; //h1
    T264.EncCodingCtrl.transform8x8Mode = codingctrl->transform8x8Mode; //h1
    T264.EncCodingCtrl.quarterPixelMv = codingctrl->quarterPixelMv; //h1
    T264.EncCodingCtrl.cirStart = codingctrl->cirStart;
    T264.EncCodingCtrl.cirInterval = codingctrl->cirInterval;
    T264.EncCodingCtrl.intraSliceMap1 = codingctrl->intraSliceMap1; //h1
    T264.EncCodingCtrl.intraSliceMap2 = codingctrl->intraSliceMap2; //h1
    T264.EncCodingCtrl.intraSliceMap3 = codingctrl->intraSliceMap3; //h1
    T264.EncCodingCtrl.intraArea = codingctrl->H264IntraArea; //h1
    T264.EncCodingCtrl.roi1Area = codingctrl->H264Roi1Area; //h1
    T264.EncCodingCtrl.roi2Area = codingctrl->H264Roi2Area; //h1
    T264.EncCodingCtrl.roi1DeltaQp = codingctrl->roi1DeltaQp;
    T264.EncCodingCtrl.roi2DeltaQp = codingctrl->roi2DeltaQp;
    T264.EncCodingCtrl.adaptiveRoi = codingctrl->adaptiveRoi; //h1
    T264.EncCodingCtrl.adaptiveRoiColor = codingctrl->adaptiveRoiColor; //h1
    T264.EncCodingCtrl.fieldOrder = codingctrl->fieldOrder;
}

void dH264::SetEncRateCtrl(U_ENCODE *encode, ST_EncRateCtrl *ratectrl)
{
    T264.EncRateCtrl.pictureRc = ratectrl->pictureRc;
    T264.EncRateCtrl.mbRc = ratectrl->mbRc; //h1
    T264.EncRateCtrl.pictureSkip = ratectrl->pictureSkip;
    T264.EncRateCtrl.qpHdr = ratectrl->qpHdr;
    T264.EncRateCtrl.qpMin = ratectrl->qpMin;
    T264.EncRateCtrl.qpMax = ratectrl->qpMax;
    T264.EncRateCtrl.bitPerSecond = ratectrl->bitPerSecond;
    T264.EncRateCtrl.hrd = ratectrl->hrd;
    T264.EncRateCtrl.hrdCpbSize = ratectrl->hrdCpbSize;
    T264.EncRateCtrl.gopLen = ratectrl->gopLen;
    T264.EncRateCtrl.intraQpDelta = ratectrl->intraQpDelta;
    T264.EncRateCtrl.fixedIntraQp = ratectrl->fixedIntraQp;
    T264.EncRateCtrl.mbQpAdjustment = ratectrl->mbQpAdjustment; //h1
    T264.EncRateCtrl.longTermPicRate = ratectrl->longTermPicRate; //h1
    T264.EncRateCtrl.mbQpAutoBoost = ratectrl->mbQpAutoBoost; //h1
    T264.EncRateCtrl.u32NewStream = ratectrl->u32NewStream;
}

void dH264::SetEncIn(U_ENCODE *encode, ST_EncIn *in)
{
    T264.EncIn.busLuma = in->busLuma;
    T264.EncIn.busChromaU = in->busChromaU;
    T264.EncIn.busChromaV = in->busChromaV;
    T264.EncIn.timeIncrement = in->timeIncrement;
    T264.EncIn.pOutBuf = in->pOutBuf;
    T264.EncIn.busOutBuf = in->busOutBuf;
    T264.EncIn.outBufSize = in->outBufSize;
    T264.EncIn.codingType = in->H264CodingType; //h1
    T264.EncIn.busLumaStab = in->busLumaStab; //h1
    T264.EncIn.ipf = in->ipf; //h1
    T264.EncIn.ltrf = in->ltrf; //h1
}

void dH264::SetEncOut(U_ENCODE *encode, ST_EncOut *out)
{
    T264.EncOut.codingType = out->H264CodingType; //h1
    T264.EncOut.streamSize = out->streamSize;
    T264.EncOut.motionVectors = out->motionVectors; //h1
    T264.EncOut.pNaluSizeBuf = out->pNaluSizeBuf;
    T264.EncOut.numNalus = out->numNalus;
    T264.EncOut.mse_mul256 = out->mse_mul256;  //h1
    T264.EncOut.busScaledLuma = out->busScaledLuma;
    T264.EncOut.scaledPicture = out->scaledPicture;
    T264.EncOut.ipf = out->ipf; //h1
}

void dH264::SetPreProcessingCfg(U_ENCODE *encode, ST_EncPreProcessingCfg *EncPreProCfg)
{
    T264.EncPreProcessingCfg.origWidth = EncPreProCfg->origWidth;
    T264.EncPreProcessingCfg.origHeight = EncPreProCfg->origHeight;
    T264.EncPreProcessingCfg.xOffset = EncPreProCfg->xOffset;
    T264.EncPreProcessingCfg.yOffset = EncPreProCfg->yOffset;
    T264.EncPreProcessingCfg.inputType = EncPreProCfg->H264InputType;
    T264.EncPreProcessingCfg.rotation = EncPreProCfg->H264Rotation;
    T264.EncPreProcessingCfg.videoStabilization = EncPreProCfg->videoStabilization;
    T264.EncPreProcessingCfg.colorConversion = EncPreProCfg->H264colorConversion;
    T264.EncPreProcessingCfg.scaledOutput = EncPreProCfg->scaledOutput;
    T264.EncPreProcessingCfg.interlacedFrame = EncPreProCfg->interlacedFrame;
}
#endif //VENCODER_DEPEND_H1V6
