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
--  Description : Internal traces
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include <stdio.h>

#include "enctrace.h"

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/
static FILE *fileAsic = NULL;
static FILE *filePreProcess = NULL;
static FILE *fRegs = NULL;
static FILE *fRecon = NULL;
static FILE *fTrace = NULL;
static FILE *fTraceProbs = NULL;
static FILE *fTraceSegments = NULL;
static FILE *fDump = NULL;

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/

void EncTraceCloseAll(void)
{
    if(fileAsic != NULL)
        fclose(fileAsic);
    if(filePreProcess != NULL)
        fclose(filePreProcess);
    if(fRegs != NULL)
        fclose(fRegs);
    if(fRecon != NULL)
        fclose(fRecon);
    if(fTrace != NULL)
        fclose(fTrace);
    if(fTraceProbs != NULL)
        fclose(fTraceProbs);
    if(fTraceSegments != NULL)
        fclose(fTraceSegments);
    if(fDump != NULL)
        fclose(fDump);
}

/*------------------------------------------------------------------------------

    EncTraceAsic

------------------------------------------------------------------------------*/
void EncTraceAsicParameters(asicData_s * asic)
{
    if(fileAsic == NULL)
        fileAsic = fopen("sw_asic.trc", "w");

    if(fileAsic == NULL)
    {
        fprintf(stderr, "Unable to open trace file: asic.trc\n");
        return;
    }

    fprintf(fileAsic, "ASIC parameters:\n");
    fprintf(fileAsic, "Input lum bus:       0x%08x\n",
            (u32) asic->regs.inputLumBase);
    fprintf(fileAsic, "Input cb bus:        0x%08x\n",
            (u32) asic->regs.inputCbBase);
    fprintf(fileAsic, "Input cr bus:        0x%08x\n\n",
            (u32) asic->regs.inputCrBase);

    /*fprintf(fileAsic, "Internal image base W: 0x%08x\n", (u32)asic->internalImageBase.virtualAddress); */

    fprintf(fileAsic, "Encoding type:           %6d\n", asic->regs.codingType);
    /*
     * fprintf(fileAsic, "Input endian mode:       %6d\n",
     * asic->regs.inputEndianMode);
     * fprintf(fileAsic, "Input endian width:      %6d\n",
     * asic->regs.inputEndianWidth);
     * fprintf(fileAsic, "Output endian width:      %6d\n",
     * asic->regs.outputEndianWidth);
     */
    fprintf(fileAsic, "Mbs in row:              %6d\n", asic->regs.mbsInRow);
    fprintf(fileAsic, "Mbs in col:              %6d\n", asic->regs.mbsInCol);
    fprintf(fileAsic, "Input lum width:         %6d\n", asic->regs.pixelsOnRow);
    fprintf(fileAsic, "Frame type:              %6d\n",
            asic->regs.frameCodingType);
    fprintf(fileAsic, "QP:                      %6d\n", asic->regs.qp);
    fprintf(fileAsic, "Round control:           %6d\n",
            asic->regs.roundingCtrl);
/*    fprintf(fileAsic, "MV SAD penalty:          %6d\n", asic->regs.mvSadPenalty);
    fprintf(fileAsic, "Intra SAD penalty:       %6d\n", asic->regs.intraSadPenalty);
    fprintf(fileAsic, "4MV SAD penalty:         %6d\n", asic->regs.fourMvSadPenalty);
*/

    fprintf(fileAsic, "Burst size:              %6d\n", ENCH1_BURST_LENGTH);

}

/*------------------------------------------------------------------------------

    EncTracePreProcess

------------------------------------------------------------------------------*/
void EncTracePreProcess(preProcess_s * preProcess)
{
    if(filePreProcess == NULL)
        filePreProcess = fopen("sw_preprocess.trc", "w");

    if(filePreProcess == NULL)
    {
        fprintf(stderr, "Unable to open trace file: preprocess.trc\n");
        return;
    }

    fprintf(filePreProcess, "Input width: %d\n", preProcess->lumWidthSrc);
    fprintf(filePreProcess, "Input height: %d\n", preProcess->lumHeightSrc);
    fprintf(filePreProcess, "Hor offset src: %d\n", preProcess->horOffsetSrc);
    fprintf(filePreProcess, "Ver offset src: %d\n", preProcess->verOffsetSrc);
}

/*------------------------------------------------------------------------------

    EncTraceRegs

------------------------------------------------------------------------------*/
void EncTraceRegs(const void *ewl, u32 readWriteFlag, u32 mbNum)
{
    u32 i;
    u32 lastRegAddr = 0x48C;
    char rw = 'W';
    static i32 frame = 0;

    if(fRegs == NULL)
        fRegs = fopen("sw_reg.trc", "w");

    if(fRegs == NULL)
        fRegs = stderr;

    fprintf(fRegs, "pic=%d\n", frame);
    fprintf(fRegs, "mb=%d\n", mbNum);

    /* After frame is finished, registers are read */
    if (readWriteFlag)
    {
        rw = 'R';
        frame++;
    }

    /* Dump registers in same denali format as the system model */
    for(i = 0; i <= lastRegAddr; i += 4)
        /* DMV penalty tables not possible to read from ASIC: 0x180-0x27C */
        if ((i != 0xA0) && (i != 0x38) && (i < 0x180 || i > 0x27C))
            fprintf(fRegs, "%c %08x/%08x\n", rw, i, EWLReadReg(ewl, i));

    /* Regs with enable bits last, force encoder enable high for frame start */
    fprintf(fRegs, "%c %08x/%08x\n", rw, 0xA0, EWLReadReg(ewl, 0xA0));
    fprintf(fRegs, "%c %08x/%08x\n", rw, 0x38,
            EWLReadReg(ewl, 0x38) | (readWriteFlag==0));

    fprintf(fRegs, "\n");

    /*fclose(fRegs);
     * fRegs = NULL; */

}

u32 SwapEndian(u32 tmp)
{

    u32 swapped;

    swapped = tmp >> 24;
    swapped |= (tmp >> 8) & (0xFF00);
    swapped |= (tmp << 8) & (0xFF0000);
    swapped |= tmp << 24;

    return swapped;
}

/*------------------------------------------------------------------------------

    EncTraceProbs

------------------------------------------------------------------------------*/
void EncTraceProbs(u16 *ptr, i32 bytes)
{
    i32 i;

    if(fTraceProbs == NULL)
        fTraceProbs = fopen("sw_prob_count.trc", "w");

    if(fTraceProbs == NULL)
    {
        fprintf(stderr, "Unable to open trace file: sw_prob_count.trc\n");
        return;
    }

    for (i = 0; i < bytes/2; i+=4)
        fprintf(fTraceProbs, "%5d %5d %5d %5d\n",
                ptr[i], ptr[i+1], ptr[i+2], ptr[i+3]);
}

/*------------------------------------------------------------------------------

    EncDumpMem

------------------------------------------------------------------------------*/
void EncDumpMem(u32 * ptr, u32 bytes, char *desc)
{
    u32 i;

    if(fDump == NULL)
        fDump = fopen("sw_mem_dump.trc", "wb");

    if(fDump) {
        fprintf(fDump, "%08x %08x mem=%s\n", ptr[0], ptr[1], desc);
        for (i = 2; i < bytes/4; i+=2)
            fprintf(fDump, "%08x %08x\n", ptr[i], ptr[i+1]);
    }
}

/*------------------------------------------------------------------------------

    EncDumpRecon

------------------------------------------------------------------------------*/
void EncDumpRecon(asicData_s * asic)
{
    if (!asic->traceRecon) return;

    if(fRecon == NULL)
        fRecon = fopen("sw_recon_internal.yuv", "wb");

    if(fRecon)
    {
        u32 index;
        u32 size = asic->regs.mbsInCol * asic->regs.mbsInRow * 16 * 16;

        for (index = 0; index < 3; index++) {
            if (asic->regs.internalImageLumBaseW ==
                asic->internalImageLuma[index].busAddress) {
                u32 *pTmp = asic->internalImageLuma[index].virtualAddress;
                fwrite(pTmp, 1, size, fRecon);
            }
        }

        for (index = 0; index < 4; index++) {
            if (asic->regs.internalImageChrBaseW ==
                asic->internalImageChroma[index].busAddress) {
                u32 *pTmp = asic->internalImageChroma[index].virtualAddress;
                fwrite(pTmp, 1, size/2, fRecon);
            }
        }
    }
}

/*------------------------------------------------------------------------------

    EncTraceSegments

------------------------------------------------------------------------------*/
void EncTraceSegments(u32 *map, i32 bytes, i32 enable, i32 mapModified,
        i32* cnt, i32 *qp)
{
    i32 i;

    if(fTraceSegments == NULL)
        fTraceSegments = fopen("sw_segments.trc", "w");

    if(fTraceSegments == NULL) {
        fprintf(stderr, "Unable to open trace file: sw_segments.trc\n");
        return;
    }

    fprintf(fTraceSegments, "Segments: enable=%d  mapModified=%d\n",
            enable, mapModified);
    if (cnt)
        fprintf(fTraceSegments, "Segments: id[0]=%d  id[1]=%d  id[2]=%d  id[3]=%d  \n",
            cnt[0], cnt[1], cnt[2], cnt[3]);
    if (qp)
        fprintf(fTraceSegments, "Segments: qp[0]=%d  qp[1]=%d  qp[2]=%d  qp[3]=%d  \n",
            qp[0], qp[1], qp[2], qp[3]);

    fprintf(fTraceSegments, "Segment map:\n");
    for (i = 0; i < bytes/4; i++)
        fprintf(fTraceSegments, "0x%08x\n", map[i]);

    fprintf(fTraceSegments, "\n");
}


