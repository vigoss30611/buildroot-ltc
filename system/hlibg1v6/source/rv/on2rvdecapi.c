/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2007 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : The API module C-functions of RV Decoder
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: on2rvdecapi.c,v $
--  $Date: 2010/12/13 11:50:41 $
--  $Revision: 1.12 $
--
------------------------------------------------------------------------------*/

#include "rvdecapi.h"
#include "rv_container.h"
#include "on2rvdecapi.h"
#include "regdrv.h"

#define GET_MAJOR_VER(ver) ((ver>>28)&0x0F)
#define GET_MINOR_VER(ver) ((ver>>20)&0xFF)
#define MAX_NUM_RPR_SIZES 8

On2RvDecRet On2RvDecInit(void *pRV10Init, void **decoderState, u32 mmuEnable)
{

    RvDecRet ret;
    u32 major, minor;
    u32 isRv8;

    On2DecoderInit *pInit = (On2DecoderInit *)pRV10Init;

    if (!pRV10Init || !decoderState)
        return ON2RVDEC_POINTER;

    major = GET_MAJOR_VER(pInit->ulStreamVersion);
    minor = GET_MINOR_VER(pInit->ulStreamVersion);

    /* check that streams is either RV8 or RV9 */
    if (!(major == 4 && minor == 0) && !(major == 3 && minor == 2))
        return ON2RVDEC_INVALID_PARAMETER;

    isRv8 = (major == 3) && (minor == 2);

    ret = RvDecInit((RvDecInst*)decoderState,
        0, /* useVideoFreezeConcealment */
        0,
        NULL,
        isRv8 ? 0 : 1,
        pInit->pels,
        pInit->lines,
        0,
        DEC_REF_FRM_RASTER_SCAN,
        mmuEnable);

    switch (ret)
    {
        case RVDEC_OK:
            return ON2RVDEC_OK;

        case RVDEC_PARAM_ERROR:
            return ON2RVDEC_INVALID_PARAMETER;

        case RVDEC_MEMFAIL:
            return ON2RVDEC_OUTOFMEMORY;

        default:
            return ON2RVDEC_FAIL;
    }

}

On2RvDecRet On2RvDecFree(void *global)
{

    RvDecRelease((RvDecInst)global);

    return ON2RVDEC_OK;

}

On2RvDecRet On2RvDecDecode(u8 *pRV10Packets, u8 *pDecodedFrameBuffer,
    void *pInputParams, void *pOutputParams, void *global)
{

    u32 i;
    DecContainer *pDecCont;
    On2DecoderOutParams *out;
    On2DecoderInParams *in;
    RvDecInput rvIn;
    RvDecOutput rvOut;
    RvDecPicture rvPic;
    RvDecSliceInfo sliceInfo[128];
    RvDecRet ret;
    u32 more = 0;

    if (!pRV10Packets || !pInputParams || !global)
        return ON2RVDEC_POINTER;

    pDecCont = (DecContainer*)global;
    if (pDecCont->StrmStorage.isRv8 &&
        pDecCont->StrmStorage.frameCodeLength == 0)
        return ON2RVDEC_FAIL;

    in = (On2DecoderInParams*)pInputParams;
    out = (On2DecoderOutParams*)pOutputParams;

    rvIn.pStream = (u8*)pRV10Packets;
    rvIn.streamBusAddress = in->streamBusAddr;
    rvIn.dataLen = in->dataLength;
    rvIn.timestamp = in->timestamp;
    rvIn.picId = in->timestamp;
    rvIn.sliceInfoNum = in->numDataSegments;
    rvIn.pSliceInfo = sliceInfo;
    rvIn.skipNonReference = in->skipNonReference;
    for (i = 0; i < rvIn.sliceInfoNum; i++)
    {
        rvIn.pSliceInfo[i].offset = in->pDataSegments[i].ulSegmentOffset;
        rvIn.pSliceInfo[i].isValid = in->pDataSegments[i].bIsValid;
    }
   
    out->numFrames = 0;
    out->notes = 0;
    out->timestamp = 0;

    if ((in->flags & ON2RV_DECODE_MORE_FRAMES) ||
        (in->flags & ON2RV_DECODE_LAST_FRAME))
    {
        ret = RvDecNextPicture((RvDecInst)global, &rvPic, 
            (in->flags & ON2RV_DECODE_LAST_FRAME));
        if (ret == RVDEC_PIC_RDY && !(in->flags & ON2RV_DECODE_DONT_DRAW))
        {
            out->numFrames = 1;
            out->width = rvPic.codedWidth;
            out->height = rvPic.codedHeight;
            out->pOutFrame = rvPic.pOutputPicture;
            if (rvPic.keyPicture)
                out->notes |= ON2RV_DECODE_KEY_FRAME;
            out->timestamp = rvPic.picId;
            out->outputFormat = rvPic.outputFormat;
        }
        return ON2RVDEC_OK;
    }

    do
    {
        more = 0;
        ret = RvDecDecode((RvDecInst)global, &rvIn, &rvOut);

        switch (ret)
        {
            case RVDEC_HDRS_RDY:
                more = 1;
                break;

            case RVDEC_OK:
            case RVDEC_PIC_DECODED:
            case RVDEC_STRM_PROCESSED:
            case RVDEC_NONREF_PIC_SKIPPED:
                ret = RvDecNextPicture((RvDecInst)global, &rvPic, 0);
                if (ret == RVDEC_PIC_RDY && !(in->flags & ON2RV_DECODE_DONT_DRAW))
                {
                    out->numFrames = 1;
                    out->width = rvPic.codedWidth;
                    out->height = rvPic.codedHeight;
                    out->pOutFrame = rvPic.pOutputPicture;
                    if (rvPic.keyPicture)
                        out->notes |= ON2RV_DECODE_KEY_FRAME;
                    out->timestamp = rvPic.picId;
                    out->outputFormat = rvPic.outputFormat;
                }
                break;

            default:
                return ON2RVDEC_FAIL;
        }

    } while (more);

    return ON2RVDEC_OK;

}

On2RvDecRet On2RvDecCustomMessage(void *msg_id, void *global)
{

    u32 i;
    u32 tmp;
    DecContainer *pDecCont;
	//add by franklin  On2RvCustomMessage_ID *pId = (On2RvCustomMessage_ID )msg_id;
    On2RvCustomMessage_ID *pId = (On2RvCustomMessage_ID *)msg_id;
    On2RvMsgSetDecoderRprSizes *msg;

    if (!msg_id || !global)
        return ON2RVDEC_POINTER;

    if (*pId != ON2RV_MSG_ID_Set_RVDecoder_RPR_Sizes)
        return ON2RVDEC_NOTIMPL;

    pDecCont = (DecContainer*)global;

    msg = (On2RvMsgSetDecoderRprSizes *)msg_id;
    if (msg->num_sizes < 0 || msg->num_sizes > MAX_NUM_RPR_SIZES)
        return ON2RVDEC_FAIL;

    tmp = 1;
    while (msg->num_sizes > (1<<tmp)) tmp++;

    pDecCont->StrmStorage.frameCodeLength = tmp;

    for (i = 0; i < 2*msg->num_sizes; i++)
        pDecCont->StrmStorage.frameSizes[i] = msg->sizes[i];

    SetDecRegister(pDecCont->rvRegs, HWIF_FRAMENUM_LEN, tmp);

    return ON2RVDEC_OK;
}

On2RvDecRet On2RvDecHiveMessage(void *msg, void *global)
{
    return ON2RVDEC_NOTIMPL;
}

On2RvDecRet On2RvDecPeek(void *pOutputParams, void *global)
{

    On2DecoderOutParams *out;
    DecContainer *pDecCont;
    u32 tmp = 0;

    if (!pOutputParams || !global)
        return ON2RVDEC_POINTER;

    pDecCont = (DecContainer *) global;

    /* Check if decoder is in an incorrect mode */
    if(pDecCont->ApiStorage.DecStat == UNINIT)
        return ON2RVDEC_FAIL;

    out = (On2DecoderOutParams*)pOutputParams;

    /* Nothing to send out */
    if(pDecCont->StrmStorage.outCount == 0)
    {
        (void) G1DWLmemset(out, 0, sizeof(On2DecoderOutParams));
    }
    else
    {
        picture_t *pPic;

        pPic = (picture_t *) pDecCont->StrmStorage.pPicBuf +
            pDecCont->StrmStorage.workOut;

        out->numFrames = 1;
        out->notes = 0;
        out->width = pPic->codedWidth;
        out->height = pPic->codedHeight;
        out->timestamp = pPic->picId;
		//add by franklin for rvds compile
        out->pOutFrame = (u8 *)pPic->data.virtualAddress;
        out->outputFormat = pPic->tiledMode ?
            DEC_OUT_FRM_TILED_8X4 : DEC_OUT_FRM_RASTER_SCAN;
    }

    return ON2RVDEC_OK;

}

On2RvDecRet On2RvDecSetNbrOfBuffers( u32 nbrBuffers, void *global )
{

    DecContainer *pDecCont;

    if (!global)
        return ON2RVDEC_POINTER;

    pDecCont = (DecContainer *) global;

    /* Check if decoder is in an incorrect mode */
    if(pDecCont->ApiStorage.DecStat == UNINIT)
        return ON2RVDEC_FAIL;

    if(!nbrBuffers)
        return ON2RVDEC_INVALID_PARAMETER;

    /* If buffers have already been allocated, return FAIL */
    if(pDecCont->StrmStorage.numBuffers ||
       pDecCont->StrmStorage.numPpBuffers )
        return ON2RVDEC_FAIL;

    pDecCont->StrmStorage.maxNumBuffers = nbrBuffers;

    return ON2RVDEC_OK;

}

On2RvDecRet On2RvDecSetReferenceFrameFormat( 
    On2RvRefFrmFormat referenceFrameFormat, void *global )
{

    DecContainer *pDecCont;
    DWLHwConfig_t config;

    if (!global)
        return ON2RVDEC_POINTER;

    pDecCont = (DecContainer *) global;

    /* Check if decoder is in an incorrect mode */
    if(pDecCont->ApiStorage.DecStat == UNINIT)
        return ON2RVDEC_FAIL;

    if(referenceFrameFormat != ON2RV_REF_FRM_RASTER_SCAN &&
       referenceFrameFormat != ON2RV_REF_FRM_TILED_DEFAULT)
        return ON2RVDEC_INVALID_PARAMETER;

    DWLReadAsicConfig(&config);

    if(referenceFrameFormat == ON2RV_REF_FRM_TILED_DEFAULT)
    {
        /* Assert support in HW before enabling.. */
        if(!config.tiledModeSupport)
        {
            return ON2RVDEC_FAIL;
        }
        pDecCont->tiledModeSupport = config.tiledModeSupport;
    }
    else
        pDecCont->tiledModeSupport = 0;


    return ON2RVDEC_OK;

}
