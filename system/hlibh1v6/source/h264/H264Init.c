/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : Encoder initialization and setup
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "H264Init.h"
#include "enccommon.h"
#include "ewl.h"

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/
#define H264ENC_MIN_ENC_WIDTH       132     /* 144 - 12 pixels overfill */
#define H264ENC_MAX_ENC_WIDTH       4080
#define H264ENC_MIN_ENC_HEIGHT      96
#define H264ENC_MAX_ENC_HEIGHT      4080
#define H264ENC_MAX_REF_FRAMES      2

#define H264ENC_MAX_MBS_PER_PIC     65025   /* 4080x4080 */

/* Level 51 MB limit is increased to enable max resolution */
#define H264ENC_MAX_LEVEL           51

#define H264ENC_DEFAULT_QP          26

/* Tracing macro */
#define APITRACE(str) printf(str)

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/
static bool_e SetParameter(h264Instance_s * inst,
                           const H264EncConfig * pEncCfg);
static bool_e CheckParameter(const h264Instance_s * inst);

static i32 SetPictureBuffer(h264Instance_s *inst);

/*------------------------------------------------------------------------------

    H264CheckCfg

    Function checks that the configuration is valid.

    Input   pEncCfg Pointer to configuration structure.

    Return  ENCHW_OK      The configuration is valid.
            ENCHW_NOK     Some of the parameters in configuration are not valid.

------------------------------------------------------------------------------*/
void fw_H264CheckCfg3(const H264EncConfig * pEncCfg)
{
}

bool_e H264CheckCfg(const H264EncConfig * pEncCfg)
{
//    printf("H264CheckCfg ,cfg:%x\n", pEncCfg);
    u32 height = pEncCfg->height;

    /* Interlaced coding sets height limitations per field */
    if(pEncCfg->viewMode == H264ENC_INTERLACED_FIELD)
        height /= 2;

    if ((pEncCfg->streamType != H264ENC_BYTE_STREAM) &&
        (pEncCfg->streamType != H264ENC_NAL_UNIT_STREAM)) {
        printf("H264CheckCfg: Invalid stream type, streamType:%d, bytestm:%d, nal:%d\n", 
            pEncCfg->streamType, H264ENC_BYTE_STREAM, H264ENC_NAL_UNIT_STREAM);
        return ENCHW_NOK;
    }

    /* Encoded image width limits, multiple of 4 */
    if(pEncCfg->width < H264ENC_MIN_ENC_WIDTH ||
       pEncCfg->width > H264ENC_MAX_ENC_WIDTH || (pEncCfg->width & 0x3) != 0) {
        printf("H264CheckCfg: Invalid width:%d, min:%d, max:%d\n", 
            pEncCfg->width ,H264ENC_MIN_ENC_WIDTH,H264ENC_MAX_ENC_WIDTH);
        return ENCHW_NOK;
    }

    /* Encoded image height limits, multiple of 2 */
    if(height < H264ENC_MIN_ENC_HEIGHT ||
       height > H264ENC_MAX_ENC_HEIGHT || (height & 0x1) != 0) {
        printf("H264CheckCfg: Invalid hei:%d, min:%d, max:%d\n", 
            height,H264ENC_MIN_ENC_HEIGHT,H264ENC_MAX_ENC_HEIGHT);
        return ENCHW_NOK;
    }

    /* Scaled image width limits, multiple of 4 (YUYV) and smaller than input */
    if((pEncCfg->scaledWidth > pEncCfg->width) ||
       (pEncCfg->scaledWidth & 0x3) != 0) {
        printf("H264CheckCfg: Invalid scalwid:%d, >? %d, &3:%d\n", 
            pEncCfg->scaledWidth,pEncCfg->width,pEncCfg->scaledWidth & 0x3);
        return ENCHW_NOK;
    }

    if((pEncCfg->scaledHeight > height) ||
       (pEncCfg->scaledHeight & 0x1) != 0) {
        printf("H264CheckCfg: Invalid scalhei:%d, >? %d, &1:%d\n", 
            pEncCfg->scaledHeight,height,pEncCfg->scaledHeight & 0x1);
        return ENCHW_NOK;
    }

    if((pEncCfg->scaledWidth == pEncCfg->width) &&
       (pEncCfg->scaledHeight == height)) {
        printf("H264CheckCfg: Invalid scal:%d-%d, =? %d-%d\n", 
            pEncCfg->scaledWidth,pEncCfg->scaledHeight,pEncCfg->width,height);
        return ENCHW_NOK;
    }

    /* total macroblocks per picture limit */
    if(((height + 15) / 16) * ((pEncCfg->width + 15) / 16) >
       H264ENC_MAX_MBS_PER_PIC) {
        printf("H264CheckCfg: Invalid macroblk:%d > %d, \n", 
            ((height + 15) / 16) * ((pEncCfg->width + 15) / 16), H264ENC_MAX_MBS_PER_PIC);
        return ENCHW_NOK;
    }

    /* Check frame rate */
    if(pEncCfg->frameRateNum < 1 || pEncCfg->frameRateNum > ((1 << 20) - 1)) {
        printf("H264CheckCfg: Invalid framerate:%d , \n",  pEncCfg->frameRateNum);
        return ENCHW_NOK;
    }

    if(pEncCfg->frameRateDenom < 1) {
        printf("H264CheckCfg: Invalid frameRateDenom:%d , \n",  pEncCfg->frameRateDenom);
        return ENCHW_NOK;
    }

    /* special allowal of 1000/1001, 0.99 fps by customer request */
    if(pEncCfg->frameRateDenom > pEncCfg->frameRateNum &&
       !(pEncCfg->frameRateDenom == 1001 && pEncCfg->frameRateNum == 1000)) {
        printf("H264CheckCfg: Invalid frameRateDenom2:%d  > %d ratenum:%d, \n",  
            pEncCfg->frameRateDenom,pEncCfg->frameRateNum,pEncCfg->frameRateNum);
        return ENCHW_NOK;
    }

    /* check level */
    if((pEncCfg->level > H264ENC_MAX_LEVEL) &&
       (pEncCfg->level != H264ENC_LEVEL_1_b)) {
        printf("H264CheckCfg: Invalid lvl:%d  > %d !=%d, \n",  
            pEncCfg->level, H264ENC_MAX_LEVEL, H264ENC_LEVEL_1_b);
        return ENCHW_NOK;
    }

    if(pEncCfg->refFrameAmount < 1 ||
       pEncCfg->refFrameAmount > H264ENC_MAX_REF_FRAMES ||
       (pEncCfg->refFrameAmount > 1 &&
        pEncCfg->viewMode != H264ENC_BASE_VIEW_MULTI_BUFFER)) {
        printf("H264CheckCfg: Invalid ref:%d  > %d !=%d, \n",  
            pEncCfg->refFrameAmount, H264ENC_MAX_REF_FRAMES, H264ENC_BASE_VIEW_MULTI_BUFFER);
        return ENCHW_NOK;
    }

    /* check HW limitations */
    {
        EWLHwConfig_t cfg = EWLReadAsicConfig();
        /* is H.264 encoding supported */
        if(cfg.h264Enabled == EWL_HW_CONFIG_NOT_SUPPORTED) {
        printf("H264CheckCfg: Invalid HW lim enable:%d   \n",   cfg.h264Enabled);
        printf("H264CheckCfg: Invalid HW lim error . fengwu continue\n");
            //return ENCHW_NOK;
        }

        /* max width supported */
        if(cfg.maxEncodedWidth < pEncCfg->width) {
        printf("H264CheckCfg: Invalid HW max enc wid:%d < %d   \n",  cfg.maxEncodedWidth, pEncCfg->width) ;
        printf("H264CheckCfg: Invalid HW max enc but fengwu continue   \n");
            cfg.maxEncodedWidth = pEncCfg->width;
        //    return ENCHW_NOK;
        }
    }

//    printf("H264CheckCfg ,OK\n");
    return ENCHW_OK;
}

/*------------------------------------------------------------------------------

    H264Init

    Function initializes the Encoder and create new encoder instance.

    Input   pEncCfg     Encoder configuration.
            instAddr    Pointer to instance will be stored in this address

    Return  H264ENC_OK
            H264ENC_MEMORY_ERROR 
            H264ENC_EWL_ERROR
            H264ENC_EWL_MEMORY_ERROR
            H264ENC_INVALID_ARGUMENT

------------------------------------------------------------------------------*/
H264EncRet H264Init(const H264EncConfig * pEncCfg, h264Instance_s ** instAddr)
{
    h264Instance_s *inst = NULL;
    const void *ewl = NULL;
    u32 i;
    H264EncRet ret = H264ENC_OK;
    EWLInitParam_t param;

    ASSERT(pEncCfg);
    ASSERT(instAddr);

    *instAddr = NULL;

    param.clientType = EWL_CLIENT_TYPE_H264_ENC;

    /* Init EWL */
    if((ewl = EWLInit(&param)) == NULL)
        return H264ENC_EWL_ERROR;

    /* Encoder instance */
    inst = (h264Instance_s *) EWLcalloc(1, sizeof(h264Instance_s));

    if(inst == NULL)
    {
        ret = H264ENC_MEMORY_ERROR;
        goto err;
    }

    /* Default values */
    H264SeqParameterSetInit(&inst->seqParameterSet);
    H264PicParameterSetInit(&inst->picParameterSet);
    H264SliceInit(&inst->slice);

    /* Set parameters depending on user config */
    if(SetParameter(inst, pEncCfg) != ENCHW_OK)
    {
        ret = H264ENC_INVALID_ARGUMENT;
        goto err;
    }

    if(SetPictureBuffer(inst) != ENCHW_OK) {
        ret = H264ENC_INVALID_ARGUMENT;
        goto err;
    }

    /* Check and init the rest of parameters */
    if(CheckParameter(inst) != ENCHW_OK)
    {
        ret = H264ENC_INVALID_ARGUMENT;
        goto err;
    }

    if(H264InitRc(&inst->rateControl, 1) != ENCHW_OK)
    {
        return H264ENC_INVALID_ARGUMENT;
    }

    if (EncPreProcessAlloc(&inst->preProcess,
                           inst->mbPerRow*inst->mbPerCol) != ENCHW_OK)
        return ENCHW_NOK;

    /* Initialize ASIC */
    inst->asic.ewl = ewl;
    (void) EncAsicControllerInit(&inst->asic);

    /* Allocate internal SW/HW shared memories */
    if(EncAsicMemAlloc_V2(&inst->asic,
                          (u32) inst->preProcess.lumWidth,
                          (u32) inst->preProcess.lumHeight,
                          (u32) inst->preProcess.scaledWidth,
                          (u32) inst->preProcess.scaledHeight,
                          ASIC_H264, inst->numRefBuffsLum,
                          inst->numRefBuffsChr) != ENCHW_OK)
    {

        ret = H264ENC_EWL_MEMORY_ERROR;
        goto err;
    }

    /* Assign allocated HW frame buffers into picture buffer */
    inst->picBuffer.size = MAX(1, inst->numRefBuffsLum - 1);
    for (i = 0; i < inst->numRefBuffsLum; i++)
        inst->picBuffer.refPic[i].picture.lum = inst->asic.internalImageLuma[i].busAddress;
    for (i = 0; i < inst->numRefBuffsChr; i++)
        inst->picBuffer.refPic[i].picture.cb = inst->asic.internalImageChroma[i].busAddress;
    /* single luma buffer -> copy buffer pointer */
    if (inst->numRefBuffsLum == 1) /* single buffer mode */
        inst->picBuffer.refPic[1].picture.lum = 
            inst->picBuffer.refPic[0].picture.lum;

    *instAddr = inst;

    /* init VUI */
    {
        const h264VirtualBuffer_s *vb = &inst->rateControl.virtualBuffer;

        H264SpsSetVuiTimigInfo(&inst->seqParameterSet,
                               vb->timeScale, vb->unitsInTic);
    }

    /* Disable 4x4 MV mode for high levels to limit MaxMvsPer2Mb */
    if(inst->seqParameterSet.levelIdc >= 31)
        inst->asic.regs.h264Inter4x4Disabled = 1;
    else
        inst->asic.regs.h264Inter4x4Disabled = 0;

    return ret;

  err:
    if(inst != NULL)
        EWLfree(inst);
    if(ewl != NULL)
        (void) EWLRelease(ewl);

    return ret;
}

/*------------------------------------------------------------------------------

    H264Shutdown

    Function frees the encoder instance.

    Input   h264Instance_s *    Pointer to the encoder instance to be freed.
                            After this the pointer is no longer valid.

------------------------------------------------------------------------------*/
void H264Shutdown(h264Instance_s * data)
{
    const void *ewl;

    ASSERT(data);

    ewl = data->asic.ewl;

    EncAsicMemFree_V2(&data->asic);

    EncPreProcessFree(&data->preProcess);

    EWLfree(data);

    (void) EWLRelease(ewl);
}

/*------------------------------------------------------------------------------

    SetParameter

    Set all parameters in instance to valid values depending on user config.

------------------------------------------------------------------------------*/
bool_e SetParameter(h264Instance_s * inst, const H264EncConfig * pEncCfg)
{
    i32 width, height, tmp, bps;
    EWLHwConfig_t cfg = EWLReadAsicConfig();

    ASSERT(inst);

    /* Internal images, next macroblock boundary */
    width = 16 * ((pEncCfg->width + 15) / 16);
    height = 16 * ((pEncCfg->height + 15) / 16);

    /* stream type */
    if(pEncCfg->streamType == H264ENC_BYTE_STREAM)
    {
        inst->asic.regs.h264StrmMode = ASIC_H264_BYTE_STREAM;
        inst->picParameterSet.byteStream = ENCHW_YES;
        inst->seqParameterSet.byteStream = ENCHW_YES;
        inst->rateControl.sei.byteStream = ENCHW_YES;
        inst->slice.byteStream = ENCHW_YES;
    }
    else
    {
        inst->asic.regs.h264StrmMode = ASIC_H264_NAL_UNIT;
        inst->picParameterSet.byteStream = ENCHW_NO;
        inst->seqParameterSet.byteStream = ENCHW_NO;
        inst->rateControl.sei.byteStream = ENCHW_NO;
        inst->slice.byteStream = ENCHW_NO;
    }

    if(pEncCfg->viewMode == H264ENC_BASE_VIEW_SINGLE_BUFFER)
    {
        inst->numViews = 1;
        inst->numRefBuffsLum = 1;
        inst->numRefBuffsChr = 2;
        inst->seqParameterSet.numRefFrames = 1;
    }
    else if(pEncCfg->viewMode == H264ENC_BASE_VIEW_DOUBLE_BUFFER)
    {
        inst->numViews = 1;
        inst->numRefBuffsLum = 2;
        inst->numRefBuffsChr = 2;
        inst->seqParameterSet.numRefFrames = 1;
    }
    else if(pEncCfg->viewMode == H264ENC_BASE_VIEW_MULTI_BUFFER)
    {
        inst->numViews = 1;
        inst->numRefBuffsLum = pEncCfg->refFrameAmount + 1;
        inst->numRefBuffsChr = pEncCfg->refFrameAmount + 1;
        inst->seqParameterSet.numRefFrames = pEncCfg->refFrameAmount;
    }
    else if(pEncCfg->viewMode == H264ENC_MVC_STEREO_INTER_VIEW_PRED)
    {
        inst->numViews = 2;
        inst->numRefBuffsLum = 1;
        inst->numRefBuffsChr = 2;
        inst->seqParameterSet.numRefFrames = 1;
    }
    else if(pEncCfg->viewMode == H264ENC_MVC_STEREO_INTER_PRED)
    {
        inst->numViews = 2;
        inst->numRefBuffsLum = 2;
        inst->numRefBuffsChr = 3;
        inst->seqParameterSet.numRefFrames = 1;
    }
    else /*if(pEncCfg->viewMode == H264ENC_INTERLACED_FIELD)*/
    {
        inst->numViews = 1;
        inst->numRefBuffsLum = 2;
        inst->numRefBuffsChr = 3;
        inst->seqParameterSet.frameMbsOnly = ENCHW_NO;
        /* 2 ref frames will be buffered so each field will reference previous
           field with same parity (except for I-frame bottom referencing top) */
        inst->seqParameterSet.numRefFrames = 2;
        inst->interlaced = 1;
        /* Map unit 32-pixels high for fields */
        height = 32 * ((pEncCfg->height + 31) / 32);
    }

    /* Slice */
    inst->slice.sliceSize = 0;

    /* Macroblock */
    inst->mbPerRow = width / 16;
    inst->mbPerCol = height / (16*(1+inst->interlaced));
    inst->mbPerFrame = inst->mbPerRow * inst->mbPerCol;

    /* Disable intra and ROI areas by default */
    inst->asic.regs.intraAreaTop = inst->asic.regs.intraAreaBottom = inst->mbPerCol;
    inst->asic.regs.intraAreaLeft = inst->asic.regs.intraAreaRight = inst->mbPerRow;
    inst->asic.regs.roi1Top = inst->asic.regs.roi1Bottom = inst->mbPerCol;
    inst->asic.regs.roi1Left = inst->asic.regs.roi1Right = inst->mbPerRow;
    inst->asic.regs.roi2Top = inst->asic.regs.roi2Bottom = inst->mbPerCol;
    inst->asic.regs.roi2Left = inst->asic.regs.roi2Right = inst->mbPerRow;

    /* Sequence parameter set */
    inst->seqParameterSet.levelIdc = pEncCfg->level;
    inst->seqParameterSet.picWidthInMbsMinus1 = width / 16 - 1;
    inst->seqParameterSet.picHeightInMapUnitsMinus1 = height / (16*(1+inst->interlaced)) - 1;

    /* Set cropping parameters if required */
    if( pEncCfg->width%16 || pEncCfg->height%16 ||
        (inst->interlaced && pEncCfg->height%32))
    {
        inst->seqParameterSet.frameCropping = ENCHW_YES;
        inst->seqParameterSet.frameCropRightOffset = (width - pEncCfg->width) / 2 ;
        inst->seqParameterSet.frameCropBottomOffset = (height - pEncCfg->height) / 2;
    }

    /* Level 1b is indicated with levelIdc == 11 (later) and constraintSet3 */
    if(pEncCfg->level == H264ENC_LEVEL_1_b)
    {
        inst->seqParameterSet.constraintSet3 = ENCHW_YES;
    }

    /* Get the index for the table of level maximum values */
    tmp = H264GetLevelIndex(inst->seqParameterSet.levelIdc);
    if(tmp == INVALID_LEVEL)
        return ENCHW_NOK;

    inst->seqParameterSet.levelIdx = tmp;

#if 1   /* enforce maximum frame size in level */
    if(inst->mbPerFrame > H264MaxFS[inst->seqParameterSet.levelIdx])
    {
        return ENCHW_NOK;
    }
#endif

#if 0   /* enforce macroblock rate limit in level */
    {
        u32 mb_rate =
            (pEncCfg->frameRateNum * inst->mbPerFrame) /
            pEncCfg->frameRateDenom;

        if(mb_rate > H264MaxMBPS[inst->seqParameterSet.levelIdx])
        {
            return ENCHW_NOK;
        }
    }
#endif

    /* Picture parameter set */
    inst->picParameterSet.picInitQpMinus26 = (i32) H264ENC_DEFAULT_QP - 26;

    /* CABAC enabled by default */
    inst->picParameterSet.enableCabac = 1;

    /* Rate control setup */

    /* Maximum bitrate for the specified level */
    bps = H264MaxBR[inst->seqParameterSet.levelIdx];

    {
        h264RateControl_s *rc = &inst->rateControl;

        rc->outRateDenom = pEncCfg->frameRateDenom;
        rc->outRateNum = pEncCfg->frameRateNum;
        rc->mbPerPic = (width / 16) * (height / 16);
        rc->mbRows = height / 16;

        {
            h264VirtualBuffer_s *vb = &rc->virtualBuffer;

            vb->bitRate = bps;
            vb->unitsInTic = pEncCfg->frameRateDenom;
            vb->timeScale = pEncCfg->frameRateNum;
            vb->bufferSize = H264MaxCPBS[inst->seqParameterSet.levelIdx];
        }

        rc->hrd = ENCHW_YES;
        rc->picRc = ENCHW_YES;
        rc->mbRc = ENCHW_NO;
        rc->picSkip = ENCHW_NO;

        rc->qpHdr = H264ENC_DEFAULT_QP;
        rc->qpMin = 10;
        rc->qpMax = 51;

        rc->frameCoded = ENCHW_YES;
        rc->sliceTypeCur = ISLICE;
        rc->sliceTypePrev = PSLICE;
        rc->gopLen = 150;

        /* Default initial value for intra QP delta */
        rc->intraQpDelta = -3;
        rc->fixedIntraQp = 0;
        /* default long-term pic rate */
        rc->longTermPicRate = 15;
    }

    /* no SEI by default */
    inst->rateControl.sei.enabled = ENCHW_NO;

    /* Pre processing */
    inst->preProcess.lumWidth = pEncCfg->width;
    inst->preProcess.lumWidthSrc =
        H264GetAllowedWidth(pEncCfg->width, H264ENC_YUV420_PLANAR);

    inst->preProcess.lumHeight = pEncCfg->height;
    if (inst->interlaced) inst->preProcess.lumHeight = pEncCfg->height/2;
    inst->preProcess.lumHeightSrc = pEncCfg->height;

    inst->preProcess.horOffsetSrc = 0;
    inst->preProcess.verOffsetSrc = 0;

    inst->preProcess.rotation = ROTATE_0;
    inst->preProcess.inputFormat = H264ENC_YUV420_PLANAR;
    inst->preProcess.videoStab = 0;
    inst->preProcess.scaledWidth    = pEncCfg->scaledWidth;
    inst->preProcess.scaledHeight   = pEncCfg->scaledHeight;

    /* Is HW scaling supported */
    if (cfg.scalingEnabled == EWL_HW_CONFIG_NOT_SUPPORTED)
        inst->preProcess.scaledWidth = inst->preProcess.scaledHeight = 0;

    inst->preProcess.scaledOutput   =
        (inst->preProcess.scaledWidth*inst->preProcess.scaledHeight ? 1 : 0);
    inst->preProcess.adaptiveRoi = 0;
    inst->preProcess.adaptiveRoiColor  = 0;
    inst->preProcess.adaptiveRoiMotion = -5;

    inst->preProcess.colorConversionType = 0;
    EncSetColorConversion(&inst->preProcess, &inst->asic);

    /* Level 1b is indicated with levelIdc == 11 (constraintSet3) */
    if(pEncCfg->level == H264ENC_LEVEL_1_b)
    {
        inst->seqParameterSet.levelIdc = 11;
    }            

    return ENCHW_OK;
}

/*------------------------------------------------------------------------------

    CheckParameter

------------------------------------------------------------------------------*/
bool_e CheckParameter(const h264Instance_s * inst)
{
    /* Check crop */
    if(EncPreProcessCheck(&inst->preProcess) != ENCHW_OK)
    {
        return ENCHW_NOK;
    }

    return ENCHW_OK;
}

/*------------------------------------------------------------------------------

    SetPictureBuffer

------------------------------------------------------------------------------*/
i32 SetPictureBuffer(h264Instance_s *inst)
{
    picBuffer *picBuffer = &inst->picBuffer;
    i32 width, height;

    width = inst->mbPerRow*16;
    height = inst->mbPerCol*16;
    if (H264PictureBufferAlloc(picBuffer, width, height) != ENCHW_OK)
        return ENCHW_NOK;

    return ENCHW_OK;
}

/*------------------------------------------------------------------------------

    Round the width to the next multiple of 8 or 16 depending on YUV type.

------------------------------------------------------------------------------*/
i32 H264GetAllowedWidth(i32 width, H264EncPictureType inputType)
{
    if(inputType == H264ENC_YUV420_PLANAR)
    {
        /* Width must be multiple of 16 to make
         * chrominance row 64-bit aligned */
        return ((width + 15) / 16) * 16;
    }
    else
    {   /* H264ENC_YUV420_SEMIPLANAR */
        /* H264ENC_YUV422_INTERLEAVED_YUYV */
        /* H264ENC_YUV422_INTERLEAVED_UYVY */
        return ((width + 7) / 8) * 8;
    }
}
