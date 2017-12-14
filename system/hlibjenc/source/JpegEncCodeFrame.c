#include "JpegEncCodeFrame.h"
#include "ewl_jpeg_enc.h"
#include "ewl.h"
#include "JpegEncPutBits.h"
#include "JpegEnclsQuantTables.h"
#include <malloc.h>
#include <string.h>
#define BITN(n) (0x00000001 << n)


static const u32 qtRegOrder[64] =
{
    0,2,3,9,10,20,21,35,
    1,4,8,11,19,22,34,36,
    5,7,12,18,23,33,37,48,
    6,13,17,24,32,38,47,49,
    14,16,25,31,39,46,50,57,
    15,26,30,40,45,51,56,58,
    27,29,41,44,52,55,59,62,
    28,42,43,53,54,60,61,63
};

void dump(ewl_t *ewl, int flag);

static void EncAsicFrameStart(const void *ewl, regValues_s * val);

jpegEncodeFrame_e EncJpegCodeFrame(jpegInstance_s * inst) {
    jpegEncodeFrame_e ret = JPEGENCODE_OK;
    asicData_s *asic = &inst->asic;
    i32 ewl_ret;
    u32 outEndAdr;
    u32 curAdr;

    EncJpegSetBuffer(&inst->stream, asic->regs.outputStrmSize);
    inst->jpeg.qTable.pQlumi = QuantLuminance[inst->jpeg.qtLevel];
    inst->jpeg.qTable.pQchromi = QuantChrominance[inst->jpeg.qtLevel];
    asic->regs.qtLevel = inst->jpeg.qtLevel;
    EncJpegHdr(&inst->stream, &inst->jpeg);

    asic->regs.outputStrmBase += inst->stream.byteCnt;
    asic->regs.outputStrmSize -= inst->stream.byteCnt;
    EncAsicFrameStart(asic->ewl, &(asic->regs));

    #if 0  //interupt mode need fix future
    printf("Start EWLWaitHwRdy\n");
    //ewl_ret = EWLWaitHwRdy(asic->ewl);
    ewl_ret = EWL_OK;
    printf("EWLWaitHwRdy ewl_ret:%d\n", ewl_ret);
    if(ewl_ret != EWL_OK) {
        ret = JPEGENCODE_TIMEOUT;
        return ret;
    }
    printf("End EWLWaitHwRdy\n\n");
    #endif

    outEndAdr = EWLReadReg(asic->ewl, OFFSET_JEOBEA);
    curAdr = EWLReadReg(asic->ewl, OFFSET_JEOCBA);

    if(curAdr >= asic->regs.outputStrmBase + asic->regs.outputStrmSize - 4) {
        printf("Jenc overflow!\n");
        return JPEGENCODE_DATA_ERROR;
    }

    int offset = curAdr - asic->regs.outputStrmBase + 1;
    inst->stream.byteCnt += offset + 1;
    inst->stream.stream += offset + 1;

    *(inst->stream.stream + 1) = 0xFF;
    *(inst->stream.stream + 2) = 0xD9;
    inst->stream.stream += 2;
    inst->stream.byteCnt += 3;

    EWLWriteReg(asic->ewl, OFFSET_JECTRL0, 0);
    return ret;
}

static void EncAsicFrameStart(const void *ewl, regValues_s * val) {

    u32 y_idx,uv_idx;

    EWLWriteReg(ewl, OFFSET_JECTRL0, 0);
    //EWLWriteReg(ewl, OFFSET_JEAXI, 0x0201);
    EWLWriteReg(ewl, OFFSET_JESRCA, val->inputLumBase);
    EWLWriteReg(ewl, OFFSET_JESRCUVA, val->inputCbBase);
    EWLWriteReg(ewl, OFFSET_JESZ, (val->picWidth << 16) | (val->picHeight));

    EWLReadReg(ewl, OFFSET_JECTRL0);

    EWLWriteReg(ewl, OFFSET_JEOA, val->outputStrmBase);
    EWLWriteReg(ewl, OFFSET_JEOEA, val->outputStrmBase + val->outputStrmSize);

    EWLWriteReg(ewl, OFFSET_JEQCIDX, 0x00020000);
    EWLReadReg(ewl, OFFSET_JEQCIDX);

    for (y_idx=0;y_idx<64;y_idx++) {
        EWLReadReg(ewl, OFFSET_JEQCIDX);
        EWLWriteReg(ewl, OFFSET_JEQCV, 0x400/QuantLuminance[val->qtLevel][qtRegOrder[y_idx]]);
    }
    for (uv_idx=0;uv_idx<64;uv_idx++) {
        EWLReadReg(ewl, OFFSET_JEQCIDX);
        EWLWriteReg(ewl, OFFSET_JEQCV, 0x400/QuantChrominance[val->qtLevel][qtRegOrder[uv_idx]]);
    }

    EWLWriteReg(ewl, OFFSET_JESRCS, val->stride);

    if(val->codingMode) {
        printf("set nv12 mode!\n");
        EWLWriteReg(ewl, OFFSET_JECTRL0, EWLReadReg(ewl, OFFSET_JECTRL0) | BITN(9) | BITN(10));//nv12
    } else {
        printf("set nv21 mode!\n");
        EWLWriteReg(ewl, OFFSET_JECTRL0, (EWLReadReg(ewl, OFFSET_JECTRL0) & ~(BITN(9))) | BITN(10));
    }

    if((EWLReadReg(ewl, OFFSET_JESTAT0) & 0x01) != 0) {
        printf("err:asic-jenc is busy\n");
    }

    //dump(ewl, 0);
    EWLWriteReg(ewl, OFFSET_JECTRL0, (EWLReadReg(ewl, OFFSET_JECTRL0) & 0xFFFFFC)| BITN(0));
    EWLReadReg(ewl, OFFSET_JECTRL0);
    while( !(EWLReadReg(ewl, OFFSET_JECTRL0) & (1 << 24)) ) {

    }
    printf("jenc encode finish!\n");
    //dump(ewl, 1);
    EWLWriteReg(ewl, OFFSET_JECTRL0, EWLReadReg(ewl, OFFSET_JECTRL0) & ~(1 << 24));
    while((EWLReadReg(ewl, OFFSET_JESTAT0) & 0x01) != 0) {
        //printf("err:asic-jenc is busy\n");
    }

}

void dump(ewl_t *ewl, int flag) {
    if (flag == 0) {
        printf("before encode---------------------------\n");
    } else {
        printf("after encode---------------------------\n");
    }
    printf("module regs:\n");
    printf("            Control regs:0x%x\n", EWLReadReg(ewl, OFFSET_JECTRL0));
    printf("            Status regs:0x%x\n", EWLReadReg(ewl, OFFSET_JESTAT0));
    printf("            Start YAdr:0x%x\n", EWLReadReg(ewl, OFFSET_JESRCA));
    printf("            Start UVAdr:0x%x\n", EWLReadReg(ewl, OFFSET_JESRCUVA));
    printf("            Stride regs:0x%x\n", EWLReadReg(ewl, OFFSET_JESRCS));
    printf("            Imag Size:0x%x\n", EWLReadReg(ewl, OFFSET_JESZ));
    printf("            Out SAdr:0x%x\n", EWLReadReg(ewl, OFFSET_JEOA));
    printf("            Out EAdr:0x%x\n", EWLReadReg(ewl, OFFSET_JEOEA));
    printf("lum qt\n");
    for(int i=0;i<64;i++) {
        printf("            Out EAdr:0x%x  ,0x%x\n", EWLReadReg(ewl, OFFSET_JEQCIDX), EWLReadReg(ewl, OFFSET_JEQCV));
    }
    printf("cbcr qt\n");
    for(int i=0;i<64;i++) {
        printf("            Out EAdr:0x%x  ,0x%x\n", EWLReadReg(ewl, OFFSET_JEQCIDX), EWLReadReg(ewl, OFFSET_JEQCV));
    }
    printf("            Out Bitstream EAdr:0x%x\n", EWLReadReg(ewl, OFFSET_JEOBEA));
    printf("            Out Bitstream Current Adr:0x%x\n", EWLReadReg(ewl, OFFSET_JEOCBA));
    printf("            Out Bitstream AXI:0x%x\n", EWLReadReg(ewl, OFFSET_JEAXI));

}
