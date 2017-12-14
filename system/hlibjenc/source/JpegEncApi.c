#include "ewl.h"
#include <stdio.h>
#include "JpegEncApi.h"
#include "JpegEncInstance.h"
#include "JpegEncCodeFrame.h"
#include "JpegEncDhtTables.h"
#include <malloc.h>
#include <pthread.h>
pthread_mutex_t enc_mutex = PTHREAD_MUTEX_INITIALIZER;

JpegEncRet JpegInit(const JpegEncCfg * pEncCfg, jpegInstance_s **instAddr);
JpegEncRet JpegEncSetPictureSize(JpegEncInst inst, const JpegEncCfg *pEncCfg);
i32 CheckFullSize(const JpegEncCfg *pCfg);
JpegEncRet JpegEncRelease(JpegEncInst inst);
JpegEncRet JpegEncSetThumbnail(JpegEncInst inst, const JpegEncThumb *pJpegThumb);



JpegEncRet JpegEncOn(JpegEncInst inst)
{
    jpegInstance_s *pEncInst = (jpegInstance_s*) inst;
    if (pEncInst == NULL)
    {
        printf("JpegEncOn: ERROR Null argument");
        return JPEGENC_NULL_ARGUMENT;
    }

    EWLOn(pEncInst->asic.ewl);
    return JPEGENC_OK;
}

JpegEncRet JpegEncOff(JpegEncInst inst)
{
    jpegInstance_s *pEncInst = (jpegInstance_s *) inst;
    if (pEncInst == NULL)
    {
        printf("JpegEncOff: ERROR Null argument");
        return JPEGENC_NULL_ARGUMENT;
    }

    EWLOff(pEncInst->asic.ewl);
    return JPEGENC_OK;
}

JpegEncRet JpegEncInitOn(void)
{
    EWLInitOn();
    return JPEGENC_OK;
}

void EncHwLock(void)
{
    pthread_mutex_lock(&enc_mutex);
}

void EncHwUnlock(void)
{
    pthread_mutex_unlock(&enc_mutex);
}

JpegEncRet JpegEncInitSyn(const JpegEncCfg * pEncCfg, JpegEncInst * instAddr)
{
    JpegEncRet ret;
    EncHwLock();
    JpegEncInitOn();
    ret = JpegEncInit(pEncCfg, instAddr);
    EncHwUnlock();
    return ret;
}

JpegEncRet JpegEncEncodeSyn(JpegEncInst inst, const JpegEncIn * pEncIn, JpegEncOut * pEncOut)
{
    JpegEncRet ret;
    EncHwLock();
    JpegEncOn(inst);
    ret = JpegEncEncode(inst, pEncIn, pEncOut);
    JpegEncOff(inst);
    EncHwUnlock();
    return ret;
}

JpegEncRet JpegEncReleaseSyn(JpegEncInst inst)
{
    JpegEncRet ret;
    EncHwLock();
    JpegEncOff(inst);
    JpegEncRelease(inst);
    EncHwUnlock();
    return ret;
}

JpegEncRet JpegEncInit(const JpegEncCfg * pEncCfg, JpegEncInst * instAddr) {

    JpegEncRet ret;
    jpegInstance_s *pEncInst = NULL;

    printf("Enter JpegEncInit\n");
    if(pEncCfg == NULL || instAddr == NULL) {
        printf("JpegEncInit: ERROR null argument\n");
        return JPEGENC_NULL_ARGUMENT;
    }

    ret = JpegInit(pEncCfg, &pEncInst);
    if(ret != JPEGENC_OK) {
        printf("JpegEncInit: ERROR Initialization failed\n");
        return ret;
    }

    *instAddr = (JpegEncInst)pEncInst;
    return JPEGENC_OK;
}

JpegEncRet JpegEncEncode(JpegEncInst inst, const JpegEncIn * pEncIn,JpegEncOut * pEncOut) {

    jpegInstance_s *pEncInst = (jpegInstance_s *)inst;
    jpegData_s *jpeg;
    asicData_s *asic;
    stream_s * buffer;
    jpegEncodeFrame_e ret;

    printf("Enter JpegEncEncode!\n");
    if((pEncInst == NULL) || (pEncIn == NULL) || (pEncOut == NULL)) {
        printf("JpegEncEncode: ERROR null argument\n");
        return JPEGENC_NULL_ARGUMENT;
    }

    if(pEncIn->pOutBuf == (void *)-1 || pEncIn->pOutBuf == NULL) {
       printf("JpegEncEncode: ERROR invalid argument\n");
       return JPEGENC_INVALID_ARGUMENT;
    }

    asic = &pEncInst->asic;
    jpeg = &pEncInst->jpeg;

    pEncOut->jfifSize = 0;
    buffer = &pEncInst->stream;
    buffer->stream= (u8 *)pEncIn->pOutBuf;
    buffer->size = (u32)pEncIn->outBufSize;
    /* Enable/disable app1 xmp header */
    if(pEncIn->app1)
    {
        jpeg->appn.App1= ENCHW_YES;
        jpeg->appn.XmpIdentifier = pEncIn->xmpId;
        jpeg->appn.XmpData = pEncIn->xmpData;
    }
    else
    {
        jpeg->appn.App1 = ENCHW_NO;
    }

    asic->regs.inputLumBase = pEncIn->busLum;
    asic->regs.inputCbBase = pEncIn->busCb;
    asic->regs.inputCrBase = pEncIn->busCr;
    asic->regs.outputStrmBase = pEncIn->busOutBuf;
    asic->regs.outputStrmSize = pEncIn->outBufSize;

    asic->regs.picWidth = jpeg->width;
    asic->regs.picHeight= jpeg->height;
    asic->regs.stride = jpeg->stride;
    asic->regs.codingMode = jpeg->frameType;

    ret = EncJpegCodeFrame(pEncInst);

    if(ret != JPEGENCODE_OK) {
        printf("EncJpegCodeFrame: ERROR Jpeg encode fail, ret:%d\n",ret);
        JpegEncRet ret_usr;
        switch(ret) {
            case JPEGENCODE_TIMEOUT:
                ret_usr = JPEGENC_HW_TIMEOUT;
                break;
            default:
                ret_usr = JPEGENC_SYSTEM_ERROR;
        }

        return ret_usr;
    }

    pEncOut->jfifSize = pEncInst->stream.byteCnt;

    return JPEGENC_FRAME_READY;
}

JpegEncRet JpegEncSetPictureSize(JpegEncInst inst, const JpegEncCfg *pEncCfg) {
    jpegInstance_s *pEncInst = (jpegInstance_s *)inst;

    if((pEncInst == NULL) || (pEncCfg == NULL)) {
        printf("JpegEncSetPictureSize: ERROR null argument\n");
        return JPEGENC_NULL_ARGUMENT;
    }

    if(CheckFullSize(pEncCfg) != JPEGENC_OK) {
        printf("JpegEncSetPictureSize:ERROR Out of range image dimension\n");
        return JPEGENC_INVALID_ARGUMENT;
    }

    pEncInst->jpeg.width = pEncCfg->inputWidth;
    pEncInst->jpeg.height = pEncCfg->inputHeight;
    pEncInst->jpeg.stride = pEncCfg->stride;
    pEncInst->jpeg.qtLevel = (pEncCfg->qLevel < 0 || pEncCfg->qLevel > 10)?5:pEncCfg->qLevel;
    pEncInst->jpeg.codingMode = ENC_420_MODE;
    pEncInst->jpeg.frameType = pEncCfg->frameType;

    return JPEGENC_OK;

}

JpegEncRet JpegEncSetThumbnail(JpegEncInst inst, const JpegEncThumb *pJpegThumb)
{
    jpegInstance_s *pEncInst = (jpegInstance_s *) inst;

    /* Check for illegal inputs */
    if((pEncInst == NULL) || (pJpegThumb == NULL))
    {
        printf("JpegEncSetThumbnail: ERROR null argument\n");
        return JPEGENC_NULL_ARGUMENT;
    }

    pEncInst->jpeg.appn.thumbEnable = 1;

    /* save the thumbnail config */
    memcpy(&pEncInst->jpeg.thumbnail, pJpegThumb, sizeof(JpegEncThumb));

    return JPEGENC_OK;
}

JpegEncRet JpegInit(const JpegEncCfg * pEncCfg, jpegInstance_s **instAddr) {
    JpegEncRet ret = JPEGENC_OK;
    jpegInstance_s *inst = NULL;
    const void *ewl = NULL;

    if(pEncCfg == NULL || instAddr == NULL) {
        printf("JpegInit: ERROR null argument\n");
        return JPEGENC_NULL_ARGUMENT;
    }

    *instAddr = NULL;

    if((ewl = EWLInit()) == NULL) {
        printf("JpegInit: ERROR ewl init fails\n");
        return JPEGENC_EWL_ERROR;
    }

    inst = (jpegInstance_s *)calloc(1, sizeof(jpegInstance_s));

    if(inst == NULL) {
        ret = JPEGENC_MEMORY_ERROR;
        goto err;
    }

    EncJpegInit(&inst->jpeg);

    inst->asic.ewl = ewl;

    if(pEncCfg->comLength > 0 && pEncCfg->pCom != NULL)
    {
        inst->jpeg.com.comLen = pEncCfg->comLength;
        inst->jpeg.com.pComment = pEncCfg->pCom;
        inst->jpeg.com.comEnable = 1;
    }

    /* Units type */
    if(pEncCfg->unitsType == JPEGENC_NO_UNITS)
    {
        inst->jpeg.appn.units = ENC_NO_UNITS;
        inst->jpeg.appn.Xdensity = 1;
        inst->jpeg.appn.Ydensity = 1;
    }
    else
    {
        inst->jpeg.appn.units = pEncCfg->unitsType;
        inst->jpeg.appn.Xdensity = pEncCfg->xDensity;
        inst->jpeg.appn.Ydensity = pEncCfg->yDensity;
    }

    /* Marker type */
    if(pEncCfg->markerType == JPEGENC_SINGLE_MARKER)
    {
        inst->jpeg.markerType = ENC_SINGLE_MARKER;
    }
    else
    {
        inst->jpeg.markerType = ENC_MULTI_MARKER;
    }

    *instAddr = inst;

    return ret;
err:
    if(inst != NULL) {
        free(inst);
    }

    if(ewl != NULL) {
        EWLRelease(ewl);
    }
    return ret;
}

JpegEncRet JpegEncRelease(JpegEncInst inst) {
    jpegInstance_s *pEncInst = (jpegInstance_s *) inst;
    const void *ewl;

    if(pEncInst == NULL) {
        return JPEGENC_NULL_ARGUMENT;
    }

    ewl = pEncInst->asic.ewl;
    EWLfree(pEncInst);
    EWLRelease(ewl);
    return JPEGENC_OK;

}
i32 CheckFullSize(const JpegEncCfg *pCfg) {
    if(pCfg->inputWidth > 16368 || pCfg->inputHeight > 16368) {
        printf("CheckFullSize:err inputWidth:%d inputHeight:%d\n", pCfg->inputWidth, pCfg->inputHeight);
        return JPEGENC_ERROR;
    }

    if((pCfg->stride & 15) != 0) {
        printf("CheckFullSize: error, stride:%d is not 16 multiple\n",pCfg->stride);
        return JPEGENC_ERROR;
    }

    if((pCfg->inputWidth & 15) != 0) {
        printf("CheckFullSize: error, inputWidth:%d is not 16 multiple\n",pCfg->inputWidth);
        return JPEGENC_ERROR;
    }

    if((pCfg->inputHeight & 7) != 0) {
        printf("CheckFullSize: error, inputHeight:%d is not 8 multiple\n",pCfg->inputHeight);
        return JPEGENC_ERROR;
    }

    return JPEGENC_OK;
}

JpegEncRet JpegEncSetQuality(JpegEncInst instAddr, const JpegEncCfg * pEncCfg) {
    JpegEncRet ret = JPEGENC_OK;
    jpegInstance_s *pEncInst = (jpegInstance_s *) instAddr;

    /* Check for illegal inputs */
    if(pEncCfg == NULL || instAddr == NULL)
    {
        printf("JpegSetQuality: ERROR null argument");
        return JPEGENC_NULL_ARGUMENT;
    }

    if (pEncCfg->qLevel < 0 || pEncCfg->qLevel > 10) {
        ret = JPEGENC_INVALID_ARGUMENT;
        return ret;
    }

    pEncInst->jpeg.qtLevel = pEncCfg->qLevel;

   return ret;
}
