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
--  Description : interaction with hw
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vc1hwd_asic.c,v $
--  $Revision: 1.73 $
--  $Date: 2011/01/25 13:37:58 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Include headers
------------------------------------------------------------------------------*/

#include "vc1hwd_asic.h"
#include "vc1hwd_regdrv.h"
#include "vc1hwd_util.h"
#include "vc1hwd_headers.h"
#include "refbuffer.h"

/*------------------------------------------------------------------------------
    External compiler flags
------------------------------------------------------------------------------*/

#ifndef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          do{}while(0)
#else
#undef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          printf(__VA_ARGS__)
#endif

/*------------------------------------------------------------------------------
    Module defines
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    Local function prototypes
------------------------------------------------------------------------------*/

static void SetIntensityCompensationParameters(decContainer_t *pDecCont);

static void SetReferenceBaseAddress(decContainer_t *pDecCont);

static void SetPp(decContainer_t *pDecCont);

void Vc1DecPpInit( vc1DecPp_t *pDecPp, bufferQueue_t *bq );
void Vc1DecPpSetFieldOutput( vc1DecPp_t *pDecPp, u32 fieldOutput );
void Vc1DecPpNextInput( vc1DecPp_t *pDecPp, u32 framePic );
void Vc1DecPpBuffer( vc1DecPp_t *pDecPp, u32 decOut, u32 bFrame );
void Vc1DecPpSetPpOutp( vc1DecPp_t *pDecPp, DecPpInterface *pc );
void Vc1DecPpSetPpOutpStandalone( vc1DecPp_t *pDecPp, DecPpInterface *pc );
void Vc1DecPpSetPpProc( vc1DecPp_t *pDecPp, DecPpInterface *pc );

/*------------------------------------------------------------------------------
    Functions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

    Function name: VC1RunAsic

        Functional description:

        Inputs:
            pDecCont        Pointer to decoder container
            pStrm           Pointer to stream data structure
            strmBusAddr     Stream bus address for hw

        Outputs:
            None

        Returns:
            asicStatus      Hardware status

------------------------------------------------------------------------------*/
u32 VC1RunAsic(decContainer_t *pDecCont, strmData_t *pStrm, u32 strmBusAddr)
{

    u32 i, tmp;
    i32 ret;
    i32 itmp;
    u32 asicStatus = 0;
    u32 refPicLsb;
    u32 refbuFlags = 0;

    ASSERT(pDecCont);
    if (!pDecCont->asicRunning)
    {
        SetDecRegister(pDecCont->vc1Regs, HWIF_PIC_INTERLACE_E,
                (pDecCont->storage.picLayer.fcm != PROGRESSIVE) ? 1 : 0);
        SetDecRegister(pDecCont->vc1Regs, HWIF_PIC_FIELDMODE_E,
                (pDecCont->storage.picLayer.fcm == FIELD_INTERLACE) ? 1 : 0);
        SetDecRegister(pDecCont->vc1Regs, HWIF_PIC_B_E,
                        pDecCont->storage.picLayer.picType&2 ? 1 : 0);       

        if (pDecCont->storage.picLayer.picType == PTYPE_B)
        {
            SetDecRegister(pDecCont->vc1Regs, HWIF_FWD_INTERLACE_E,
                (pDecCont->storage.pPicBuf[pDecCont->storage.work1].fcm ==
                 PROGRESSIVE) ? 0 : 1);
        }
        else
        {
            SetDecRegister(pDecCont->vc1Regs, HWIF_FWD_INTERLACE_E,
                (pDecCont->storage.pPicBuf[pDecCont->storage.work0].fcm ==
                 PROGRESSIVE) ? 0 : 1);
        }

        switch(pDecCont->storage.picLayer.picType)
        {
            case PTYPE_I:
            case PTYPE_BI:
                tmp = 0;
                break;
            case PTYPE_P:
            case PTYPE_B:
                tmp = 1;
                break;
            default:
                tmp = 1;
        }
        SetDecRegister(pDecCont->vc1Regs, HWIF_PIC_INTER_E, tmp);

        SetDecRegister(pDecCont->vc1Regs, HWIF_PIC_TOPFIELD_E,
                pDecCont->storage.picLayer.isFF ==
                pDecCont->storage.picLayer.tff);
        SetDecRegister(pDecCont->vc1Regs, HWIF_TOPFIELDFIRST_E,
                pDecCont->storage.picLayer.tff);

        SetDecRegister(pDecCont->vc1Regs, HWIF_PIC_MB_WIDTH,
            pDecCont->storage.picWidthInMbs);
        SetDecRegister(pDecCont->vc1Regs, HWIF_PIC_MB_HEIGHT_P,
            pDecCont->storage.picHeightInMbs);

        /* set offset only for advanced profile */
        if (pDecCont->storage.profile == VC1_ADVANCED)
        {
            SetDecRegister(pDecCont->vc1Regs, HWIF_MB_WIDTH_OFF,
                                (pDecCont->storage.curCodedWidth & 0xF));
            SetDecRegister(pDecCont->vc1Regs, HWIF_MB_HEIGHT_OFF,
                                (pDecCont->storage.curCodedHeight & 0xF));
        }
        else
        {
            SetDecRegister(pDecCont->vc1Regs, HWIF_MB_WIDTH_OFF, 0);
            SetDecRegister(pDecCont->vc1Regs, HWIF_MB_HEIGHT_OFF, 0);
        }
        
        if ( (pDecCont->storage.picLayer.picType == PTYPE_P) &&
             (pDecCont->storage.picLayer.fcm == FIELD_INTERLACE) &&
             (pDecCont->storage.picLayer.numRef == 0) )
        {
            /* Current field and reference field shall have same TFF value.
             * If reference frame is coded as a progressive or an interlaced
             * frame then the TFF value of the reference frame shall be
             * assumed to be the same as the TFF value of the current frame
             * [10.3.1] */
            if (pDecCont->storage.picLayer.refField == 0)
            {
                tmp = pDecCont->storage.picLayer.topField ^ 1;
            }
            else /* (refField == 1) */
            {
                tmp = pDecCont->storage.picLayer.topField;
            }
            refPicLsb = 1-tmp;
            SetDecRegister(pDecCont->vc1Regs, HWIF_REF_TOPFIELD_E, tmp);
        }
        else
        {
            if( pDecCont->storage.picLayer.fcm == FIELD_INTERLACE )
            {
                refPicLsb = pDecCont->storage.picLayer.topField;
            }
            else
            {
                refPicLsb = 0;
            }
            SetDecRegister(pDecCont->vc1Regs, HWIF_REF_TOPFIELD_E, 0 );
        }

        SetDecRegister(pDecCont->vc1Regs, HWIF_FILTERING_DIS,
                        pDecCont->storage.loopFilter ? 0 : 1);
        SetDecRegister(pDecCont->vc1Regs, HWIF_PIC_FIXED_QUANT,
                (pDecCont->storage.dquant == 0) ? 1 : 0);

        if (pDecCont->storage.picLayer.picType == PTYPE_P)
        {
            if(pDecCont->storage.maxBframes)
            {
               SetDecRegister(pDecCont->vc1Regs, HWIF_WRITE_MVS_E, 1);
               ASSERT((pDecCont->directMvs.busAddress != 0));
            }

        } else if (pDecCont->storage.picLayer.picType == PTYPE_B)
        {
            SetDecRegister(pDecCont->vc1Regs, HWIF_WRITE_MVS_E, 0);
        }
        else
        {
            SetDecRegister(pDecCont->vc1Regs, HWIF_WRITE_MVS_E, 0);
        }
        SetDecRegister(pDecCont->vc1Regs, HWIF_SYNC_MARKER_E,
            pDecCont->storage.syncMarker);
        SetDecRegister(pDecCont->vc1Regs,
                        HWIF_DQ_PROFILE,
                        pDecCont->storage.picLayer.dqProfile ==
                        DQPROFILE_ALL_MACROBLOCKS ? 1 : 0 );
        SetDecRegister(pDecCont->vc1Regs,
                        HWIF_DQBI_LEVEL,
                        pDecCont->storage.picLayer.dqbiLevel );

        if (pDecCont->storage.picLayer.picType == PTYPE_B)
        {
            /* force RANGEREDFRM to be same as in the subsequent anchor frame.
             * Reference decoder seems to work this way when current frame and
             * backward reference frame have different (illagal) values
             * [7.1.1.3] */
            SetDecRegister(pDecCont->vc1Regs, HWIF_RANGE_RED_FRM_E,
                (u32)(pDecCont->storage.
                        pPicBuf[pDecCont->storage.work0].rangeRedFrm));
        }
        else
        {
           SetDecRegister(pDecCont->vc1Regs, HWIF_RANGE_RED_FRM_E,
                (u32)(pDecCont->storage.
                        pPicBuf[pDecCont->storage.workOut].rangeRedFrm));
        }
        SetDecRegister(pDecCont->vc1Regs, HWIF_FAST_UVMC_E,
                        pDecCont->storage.fastUvMc);
        SetDecRegister(pDecCont->vc1Regs, HWIF_TRANSDCTAB,
            pDecCont->storage.picLayer.intraTransformDcIndex);
        SetDecRegister(pDecCont->vc1Regs, HWIF_TRANSACFRM,
            pDecCont->storage.picLayer.acCodingSetIndexCbCr);
        SetDecRegister(pDecCont->vc1Regs, HWIF_TRANSACFRM2,
            pDecCont->storage.picLayer.acCodingSetIndexY);
        SetDecRegister(pDecCont->vc1Regs, HWIF_MB_MODE_TAB,
                pDecCont->storage.picLayer.mbModeTab);
        SetDecRegister(pDecCont->vc1Regs, HWIF_MVTAB,
            pDecCont->storage.picLayer.mvTableIndex);
        SetDecRegister(pDecCont->vc1Regs, HWIF_CBPTAB,
            pDecCont->storage.picLayer.cbpTableIndex);
        SetDecRegister(pDecCont->vc1Regs, HWIF_2MV_BLK_PAT_TAB,
            pDecCont->storage.picLayer.mvbpTableIndex2);
        SetDecRegister(pDecCont->vc1Regs, HWIF_4MV_BLK_PAT_TAB,
            pDecCont->storage.picLayer.mvbpTableIndex4);


        SetDecRegister(pDecCont->vc1Regs, HWIF_REF_FRAMES,
                pDecCont->storage.picLayer.numRef);

        SetDecRegister(pDecCont->vc1Regs, HWIF_START_CODE_E,
                (pDecCont->storage.profile == VC1_ADVANCED) ? 1 : 0);
        SetDecRegister(pDecCont->vc1Regs, HWIF_INIT_QP,
            pDecCont->storage.picLayer.pquant);

        if( pDecCont->storage.picLayer.picType == PTYPE_P)
        {
            if( pDecCont->storage.picLayer.fcm == FIELD_INTERLACE)
            {
                SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE0_E, 0 );
                SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE1_E, 0 );
                SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE2_E, 0 );
            }
            else
            {
                SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE0_E,
                    pDecCont->storage.picLayer.rawMask & MB_SKIPPED ? 0 : 1);
                SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE1_E,
                    pDecCont->storage.picLayer.rawMask & MB_4MV ? 0 : 1);
                SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE2_E, 0 );
            }
        }
        else if( pDecCont->storage.picLayer.picType == PTYPE_B )
        {
            if (pDecCont->storage.picLayer.fcm == FIELD_INTERLACE)
            {
                SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE0_E,
                    pDecCont->storage.picLayer.rawMask & MB_FORWARD_MB ? 0 : 1);
                SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE1_E, 0 );
            }
            else
            {
                SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE0_E,
                    pDecCont->storage.picLayer.rawMask & MB_SKIPPED ? 0 : 1);
                SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE1_E,
                    pDecCont->storage.picLayer.rawMask & MB_DIRECT ? 0 : 1);
            }
            SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE2_E, 0 );
        }
        else
        {
            /* acpred flag */
            SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE0_E,
                pDecCont->storage.picLayer.rawMask & MB_AC_PRED ? 0 : 1);
            SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE1_E,
                pDecCont->storage.picLayer.rawMask & MB_OVERLAP_SMOOTH ? 0 : 1);
            if (pDecCont->storage.picLayer.fcm == FRAME_INTERLACE)
            {
                SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE2_E,
                    pDecCont->storage.picLayer.rawMask & MB_FIELD_TX ? 0 : 1);
            }
            else
            {
                SetDecRegister(pDecCont->vc1Regs, HWIF_BITPLANE2_E, 0 );
            }
        }
        SetDecRegister(pDecCont->vc1Regs, HWIF_ALT_PQUANT,
                        pDecCont->storage.picLayer.altPquant );
        SetDecRegister(pDecCont->vc1Regs, HWIF_DQ_EDGES,
                        pDecCont->storage.picLayer.dqEdges );
        SetDecRegister(pDecCont->vc1Regs, HWIF_TTMBF,
                pDecCont->storage.picLayer.mbLevelTransformTypeFlag);
        SetDecRegister(pDecCont->vc1Regs, HWIF_PQINDEX,
                pDecCont->storage.picLayer.pqIndex);
        SetDecRegister(pDecCont->vc1Regs, HWIF_BILIN_MC_E,
            pDecCont->storage.picLayer.picType &&
            (pDecCont->storage.picLayer.mvmode == MVMODE_1MV_HALFPEL_LINEAR));
        SetDecRegister(pDecCont->vc1Regs, HWIF_UNIQP_E,
                pDecCont->storage.picLayer.uniformQuantizer);
        SetDecRegister(pDecCont->vc1Regs, HWIF_HALFQP_E,
                pDecCont->storage.picLayer.halfQp);
        SetDecRegister(pDecCont->vc1Regs, HWIF_TTFRM,
                pDecCont->storage.picLayer.ttFrm);
        SetDecRegister(pDecCont->vc1Regs, HWIF_DQUANT_E,
                        pDecCont->storage.picLayer.dquantInFrame );
        SetDecRegister(pDecCont->vc1Regs, HWIF_VC1_ADV_E,
                (pDecCont->storage.profile == VC1_ADVANCED) ? 1 : 0);

        if ( (pDecCont->storage.picLayer.fcm == FIELD_INTERLACE) &&
             (pDecCont->storage.picLayer.picType == PTYPE_B))
        {
            /* Calculate forward and backward reference distances */
            tmp =
                ( pDecCont->storage.picLayer.refDist *
                  pDecCont->storage.picLayer.scaleFactor ) >> 8;
            SetDecRegister(pDecCont->vc1Regs, HWIF_REF_DIST_FWD, tmp);
            itmp = (i32) pDecCont->storage.picLayer.refDist - tmp - 1;
            if (itmp < 0) itmp = 0;
            SetDecRegister(pDecCont->vc1Regs, HWIF_REF_DIST_BWD, itmp );
        }
        else
        {
            SetDecRegister(pDecCont->vc1Regs, HWIF_REF_DIST_FWD,
                    pDecCont->storage.picLayer.refDist);
            SetDecRegister(pDecCont->vc1Regs, HWIF_REF_DIST_BWD, 0);
        }

        SetDecRegister(pDecCont->vc1Regs, HWIF_MV_SCALEFACTOR,
                        pDecCont->storage.picLayer.scaleFactor );

        SetIntensityCompensationParameters(pDecCont);

        tmp = pStrm->pStrmCurrPos - pStrm->pStrmBuffStart;
        tmp = strmBusAddr + tmp;

        SetDecRegister(pDecCont->vc1Regs, HWIF_RLC_VLC_BASE, tmp & ~0x7);

        SetDecRegister(pDecCont->vc1Regs, HWIF_STRM_START_BIT,
            8*(tmp & 0x7) + pStrm->bitPosInWord);
        tmp = pStrm->strmBuffSize - ((tmp & ~0x7) - strmBusAddr);
        SetDecRegister(pDecCont->vc1Regs, HWIF_STREAM_LEN, tmp );

        if( pDecCont->storage.profile == VC1_ADVANCED &&
            pStrm->pStrmCurrPos > pStrm->pStrmBuffStart &&
            pStrm->pStrmBuffStart + pStrm->strmBuffSize
                - pStrm->pStrmCurrPos >= 3 &&
            pStrm->pStrmCurrPos[-1] == 0x00 &&
            pStrm->pStrmCurrPos[ 0] == 0x00 &&
            pStrm->pStrmCurrPos[ 1] == 0x03 )
        {
            SetDecRegister(pDecCont->vc1Regs,
                HWIF_2ND_BYTE_EMUL_E, HANTRO_TRUE );
        }
        else
        {
            SetDecRegister(pDecCont->vc1Regs,
                HWIF_2ND_BYTE_EMUL_E, HANTRO_FALSE );
        }

        if (pDecCont->storage.picLayer.topField)
        {
            /* start of top field line or frame */
            SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_OUT_BASE,
            (u32)(pDecCont->storage.pPicBuf[pDecCont->storage.workOut].
                  data.busAddress));
        }
        else
        {
            /* start of bottom field line */
            SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_OUT_BASE,
            (u32)(pDecCont->storage.pPicBuf[pDecCont->storage.workOut].
                  data.busAddress + (pDecCont->storage.picWidthInMbs<<4)));
        }

        SetReferenceBaseAddress(pDecCont);

        SetDecRegister(pDecCont->vc1Regs, HWIF_PIC_HEADER_LEN,
                pDecCont->storage.picLayer.picHeaderBits);
        SetDecRegister(pDecCont->vc1Regs, HWIF_PIC_4MV_E,
            pDecCont->storage.picLayer.picType &&
            (pDecCont->storage.picLayer.mvmode == MVMODE_MIXEDMV));

        /* is inter */
        if( pDecCont->storage.picLayer.fcm == FIELD_INTERLACE )
        {
            tmp = (u32)( pDecCont->storage.picLayer.isFF ==
                    pDecCont->storage.picLayer.tff ) ^ 1;
        }
        else
        {
            tmp = 0;
        }

        SetDecRegister(pDecCont->vc1Regs, HWIF_PREV_ANC_TYPE,
            pDecCont->storage.anchorInter[ tmp ] );

        if (pDecCont->storage.profile != VC1_ADVANCED)
        {
            if (pDecCont->storage.picLayer.picType == PTYPE_P)
            {
                SetDecRegister(pDecCont->vc1Regs, HWIF_RANGE_RED_REF_E,
                 (pDecCont->storage.pPicBuf[pDecCont->storage.work0].rangeRedFrm));
            }
            else if (pDecCont->storage.picLayer.picType == PTYPE_B)
            {
                SetDecRegister(pDecCont->vc1Regs, HWIF_RANGE_RED_REF_E,
                    (i32)(pDecCont->storage.
                            pPicBuf[pDecCont->storage.work1].rangeRedFrm));
            }
        }
        else
        {
            SetDecRegister(pDecCont->vc1Regs, HWIF_RANGE_RED_REF_E, 0 );
        }
        if (pDecCont->storage.profile == VC1_ADVANCED)
        {
            SetDecRegister(pDecCont->vc1Regs, HWIF_VC1_DIFMV_RANGE,
                pDecCont->storage.picLayer.dmvRange);
        }
        SetDecRegister(pDecCont->vc1Regs, HWIF_MV_RANGE,
                        pDecCont->storage.picLayer.mvRange);

        if ( pDecCont->storage.overlap &&
             ((pDecCont->storage.picLayer.picType ==  PTYPE_I) ||
              (pDecCont->storage.picLayer.picType ==  PTYPE_P) ||
              (pDecCont->storage.picLayer.picType ==  PTYPE_BI)) )
        {
            /* reset this */
            SetDecRegister(pDecCont->vc1Regs, HWIF_OVERLAP_E, 0);

            if (pDecCont->storage.picLayer.pquant >= 9)
            {
                SetDecRegister(pDecCont->vc1Regs, HWIF_OVERLAP_E, 1);
                SetDecRegister(pDecCont->vc1Regs, HWIF_OVERLAP_METHOD, 1);
            }
            else
            {
                if(pDecCont->storage.picLayer.picType ==  PTYPE_P )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_OVERLAP_E, 0);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_OVERLAP_METHOD, 0);
                }
                else /* I and BI */
                {
                    switch (pDecCont->storage.picLayer.condOver)
                    {
                        case 0: tmp = 0; break;
                        case 2: tmp = 1; break;
                        case 3: tmp = 2; break;
                        default: tmp = 0; break;
                    }
                    SetDecRegister(pDecCont->vc1Regs, HWIF_OVERLAP_METHOD, tmp);
                    if (tmp != 0)
                        SetDecRegister(pDecCont->vc1Regs, HWIF_OVERLAP_E, 1);
                }
            }
        }
        else
        {
            SetDecRegister(pDecCont->vc1Regs, HWIF_OVERLAP_E, 0);
            SetDecRegister(pDecCont->vc1Regs, HWIF_OVERLAP_METHOD, 0);
        }

        SetDecRegister(pDecCont->vc1Regs, HWIF_MV_ACCURACY_FWD,
            pDecCont->storage.picLayer.picType &&
            (pDecCont->storage.picLayer.mvmode == MVMODE_1MV_HALFPEL ||
             pDecCont->storage.picLayer.mvmode ==
                                        MVMODE_1MV_HALFPEL_LINEAR) ? 0 : 1);
        SetDecRegister(pDecCont->vc1Regs, HWIF_MPEG4_VC1_RC,
            pDecCont->storage.rnd ? 0 : 1);

        SetDecRegister(pDecCont->vc1Regs, HWIF_BITPL_CTRL_BASE,
            pDecCont->bitPlaneCtrl.busAddress);

        /* For field pictures, select offset to correct field */
        tmp = pDecCont->directMvs.busAddress;

        if( pDecCont->storage.picLayer.fcm == FIELD_INTERLACE &&
            pDecCont->storage.picLayer.isFF != pDecCont->storage.picLayer.tff )
        {
            tmp += 4 * 2 * ((pDecCont->storage.picWidthInMbs *
                            ((pDecCont->storage.picHeightInMbs+1)/2)+3)&~0x3);
        }

        SetDecRegister(pDecCont->vc1Regs, HWIF_DIR_MV_BASE,
            tmp);

        /* Setup reference picture buffer */
        if( pDecCont->refBufSupport )
        {
            RefbuSetup( &pDecCont->refBufferCtrl, pDecCont->vc1Regs, 
                        (pDecCont->storage.picLayer.fcm == FIELD_INTERLACE) ?
                            REFBU_FIELD : REFBU_FRAME, 
                        (pDecCont->storage.picLayer.picType == PTYPE_I ||
                         pDecCont->storage.picLayer.picType == PTYPE_BI),
                        pDecCont->storage.picLayer.picType == PTYPE_B, 
                        0, 2, refbuFlags);
        }

        if (!pDecCont->storage.keepHwReserved)
        {
            ret = G1DWLReserveHw(pDecCont->dwl);
            if (ret != DWL_OK)
            {
                return X170_DEC_HW_RESERVED;
            }
        }

        /* Reserve HW, enable, release HW */
        pDecCont->asicRunning = 1;

        /* PP setup and start */
        if (pDecCont->ppInstance != NULL)
            SetPp(pDecCont);

        G1DWLWriteReg(pDecCont->dwl, 0x4, 0);

        for (i = 2; i < DEC_X170_REGISTERS; i++)
        {
            G1DWLWriteReg(pDecCont->dwl, 4*i, pDecCont->vc1Regs[i]);
            pDecCont->vc1Regs[i] = 0;
        }

        SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_E, 1);
        DWLEnableHW(pDecCont->dwl, 4 * 1, pDecCont->vc1Regs[1]);
    }
    else /* buffer was empty and now we restart with new stream values */
    {
        tmp = pStrm->pStrmCurrPos - pStrm->pStrmBuffStart;
        tmp = strmBusAddr + tmp;

        SetDecRegister(pDecCont->vc1Regs, HWIF_RLC_VLC_BASE, tmp & ~0x7);

        SetDecRegister(pDecCont->vc1Regs, HWIF_STRM_START_BIT,
            8*(tmp & 0x7) + pStrm->bitPosInWord);
        SetDecRegister(pDecCont->vc1Regs, HWIF_STREAM_LEN,
            pStrm->strmBuffSize - ((tmp & ~0x7) - strmBusAddr));

        G1DWLWriteReg(pDecCont->dwl, 4 * 5, pDecCont->vc1Regs[5]);
        G1DWLWriteReg(pDecCont->dwl, 4 * 6, pDecCont->vc1Regs[6]);
        G1DWLWriteReg(pDecCont->dwl, 4 * 12, pDecCont->vc1Regs[12]);

        /* start HW by clearing IRQ_BUFFER_EMPTY status bit */
        DWLEnableHW(pDecCont->dwl, 4 * 1, pDecCont->vc1Regs[1]);
    }


    ret = G1DWLWaitHwReady(pDecCont->dwl, (u32)DEC_X170_TIMEOUT_LENGTH);

    for (i = 0; i < DEC_X170_REGISTERS; i++)
    {
        pDecCont->vc1Regs[i] = G1DWLReadReg(pDecCont->dwl, 4*i);
    }

    /* Get current stream position from HW */
    tmp = GetDecRegister(pDecCont->vc1Regs, HWIF_RLC_VLC_BASE);


    if (ret == DWL_HW_WAIT_OK)
    {
        asicStatus = GetDecRegister(pDecCont->vc1Regs, HWIF_DEC_IRQ_STAT);
    }
    else
    {
        /* reset HW */
        SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_IRQ_STAT, 0);
        SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_IRQ, 0); /* just in case */

        DWLDisableHW(pDecCont->dwl, 4 * 1, pDecCont->vc1Regs[1]);

        /* End PP co-operation */
        if(pDecCont->ppControl.ppStatus == DECPP_RUNNING ||
           pDecCont->ppControl.ppStatus == DECPP_PIC_NOT_FINISHED)
        {
            if(pDecCont->ppInstance != NULL)
                pDecCont->PPEndCallback(pDecCont->ppInstance);

            pDecCont->ppControl.ppStatus = DECPP_PIC_READY;
        }

        asicStatus = (ret == DWL_HW_WAIT_TIMEOUT) ?
            X170_DEC_TIMEOUT : X170_DEC_SYSTEM_ERROR;

        return (asicStatus);
    }

    if(asicStatus & (DEC_X170_IRQ_BUFFER_EMPTY|DEC_X170_IRQ_DEC_RDY))
    {
        if ( (tmp - strmBusAddr) <= (strmBusAddr + pStrm->strmBuffSize))
        {
            /* current stream position is in the valid range */
            pStrm->pStrmCurrPos =
                pStrm->pStrmBuffStart +
                (tmp - strmBusAddr);
        }
        else
        {
            /* error situation, consider that whole stream buffer was consumed */
            pStrm->pStrmCurrPos = pStrm->pStrmBuffStart + pStrm->strmBuffSize;
        }
    }

    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_IRQ_STAT, 0);
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_IRQ, 0);    /* just in case */

    if (!(asicStatus & DEC_X170_IRQ_BUFFER_EMPTY))
    {
        pDecCont->storage.keepHwReserved = 0;
        /* End PP co-operation */
        if( (pDecCont->ppControl.ppStatus == DECPP_RUNNING || 
             pDecCont->ppControl.ppStatus == DECPP_PIC_NOT_FINISHED) &&
            pDecCont->ppInstance != NULL)
        {
            if(pDecCont->storage.picLayer.fcm == FIELD_INTERLACE && 
               pDecCont->storage.picLayer.isFF &&
               pDecCont->ppConfigQuery.deinterlace &&
               /* if decoding was not ok -> don't leave PP running */
               (asicStatus & DEC_X170_IRQ_DEC_RDY))
            {
                pDecCont->ppControl.ppStatus = DECPP_PIC_NOT_FINISHED;
                pDecCont->storage.keepHwReserved = 1;
            }
            else
            {
                TRACE_PP_CTRL("VC1RunAsic: PP Wait for end\n");
                pDecCont->PPEndCallback(pDecCont->ppInstance);
                TRACE_PP_CTRL("VC1RunAsic: PP Finished\n");
                pDecCont->ppControl.ppStatus = DECPP_PIC_READY;
            }
        }

        if (!pDecCont->storage.keepHwReserved)
            (void)G1G1DWLReleaseHw(pDecCont->dwl);

        /* HW done, release it! */
        pDecCont->asicRunning = 0;
    }

    if( pDecCont->storage.picLayer.picType != PTYPE_B &&
        pDecCont->refBufSupport &&
        (asicStatus & DEC_X170_IRQ_DEC_RDY) &&
        pDecCont->asicRunning == 0)
    {
        u8 *pTmp = (u8*)pDecCont->directMvs.virtualAddress;
        if( pDecCont->storage.picLayer.fcm == FIELD_INTERLACE &&
            pDecCont->storage.picLayer.isFF != pDecCont->storage.picLayer.tff )
        {
            pTmp += 4 * 2 * ((pDecCont->storage.picWidthInMbs *
                            ((pDecCont->storage.picHeightInMbs+1)/2)+3)&~0x3);
        }

        RefbuMvStatistics( &pDecCont->refBufferCtrl, 
                            pDecCont->vc1Regs,
                           (u32 *)pTmp,//add by franklin for rvds compile
                            pDecCont->storage.maxBframes, /* B frames <=> mv data */
                            pDecCont->storage.picLayer.picType == PTYPE_I ||
                            pDecCont->storage.picLayer.picType == PTYPE_BI );
    }

    return(asicStatus);

}

/*------------------------------------------------------------------------------

    Function name: SetPp

        Functional description:
            Determines how field are run through the post-processor
            PIPELINE / PARALLEL / STAND_ALONE.

        Inputs:
            pDecCont        Decoder container

        Outputs:
            None

        Returns:
            None

------------------------------------------------------------------------------*/
void SetPp(decContainer_t *pDecCont)
{
    picture_t *pPic;
    u32 w0, wOut;
    u32 ppIdx;
    u32 tmp;
    u32 nextBufferIndex;
    u32 prevBufferIndex;
    u32 previousB;
    u32 pipelineOff = 0;

    /* PP not connected or still running (not waited when first field of frame
     * finished */
    if (pDecCont->ppControl.ppStatus == DECPP_PIC_NOT_FINISHED)
    {
        if(pDecCont->storage.parallelMode2)
            Vc1DecPpBuffer( &pDecCont->storage.decPp,
                            pDecCont->storage.workOut,
                            pDecCont->storage.picLayer.picType == PTYPE_B ||
                            pDecCont->storage.picLayer.picType == PTYPE_BI);
        return;
    }
    
    pPic = (picture_t*)pDecCont->storage.pPicBuf;

    w0 = pDecCont->storage.work0;
    wOut = pDecCont->storage.workOut;
    ppIdx = wOut;

    /* reset use pipeline */
    pDecCont->ppControl.usePipeline = HANTRO_FALSE;

    pDecCont->ppConfigQuery.tiledMode =
        pDecCont->tiledReferenceEnable;
    pDecCont->PPConfigQuery(pDecCont->ppInstance, &pDecCont->ppConfigQuery);

    /* Check whether to enable parallel mode 2 */
    if( (!pDecCont->ppConfigQuery.pipelineAccepted ||
         pDecCont->storage.interlace) &&
        pDecCont->ppControl.multiBufStat != MULTIBUFFER_DISABLED &&
        (pDecCont->storage.maxBframes > 0) &&
        !pDecCont->storage.parallelMode2 &&
        pDecCont->ppConfigQuery.multiBuffer)
    {
        pDecCont->storage.parallelMode2 = 1;
        pDecCont->storage.pm2AllProcessedFlag = 0;
        pDecCont->storage.pm2lockBuf    = pDecCont->ppControl.prevAnchorDisplayIndex;
        pDecCont->storage.pm2StartPoint = 
            pDecCont->picNumber;
        Vc1DecPpInit( &pDecCont->storage.decPp, &pDecCont->storage.bqPp );
    }

    Vc1DecPpSetFieldOutput( &pDecCont->storage.decPp,
                            pDecCont->storage.interlace &&
                            !pDecCont->ppConfigQuery.deinterlace);


    /* If we have once enabled parallel mode 2, keep it on */
    if(pDecCont->storage.parallelMode2)
        pipelineOff = 1;

    /* Enable decoder output writes */
    SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_OUT_DIS, 0);

    /* Check cases that can be post-processed in pipeline */
    if (pDecCont->ppConfigQuery.pipelineAccepted &&
        !pipelineOff)
    {
        if ((pDecCont->storage.maxBframes == 0) ||
	        pDecCont->ppConfigQuery.multiBuffer)
        {
            /* use pipeline for I and P frames */
            pDecCont->ppControl.usePipeline = HANTRO_TRUE;
            pPic[wOut].field[0].decPpStat = PIPELINED;
            pPic[wOut].field[1].decPpStat = PIPELINED;
        }
        else
        {
            /* use pipeline for progressive B frames */
            if (!pDecCont->storage.interlace &&
                ((pDecCont->storage.picLayer.picType == PTYPE_B) ||
                 (pDecCont->storage.picLayer.picType == PTYPE_BI)))
            {
                pDecCont->ppControl.usePipeline = HANTRO_TRUE;
                pPic[wOut].field[0].decPpStat = PIPELINED;
                pPic[wOut].field[1].decPpStat = PIPELINED;

                /* Don't write decoder output for B frames when pipelined */
                SetDecRegister(pDecCont->vc1Regs, HWIF_DEC_OUT_DIS, 1);
            }
        }
        pDecCont->ppControl.picStruct = DECPP_PIC_FRAME_OR_TOP_FIELD;
	
        /* Multibuffer fullmode */
        if (pDecCont->ppConfigQuery.multiBuffer)
        {
            pDecCont->ppControl.multiBufStat = MULTIBUFFER_FULLMODE;
            pDecCont->storage.minCount = 1;
            pDecCont->storage.previousModeFull = 1;
        }
        else
        {
            pDecCont->ppControl.multiBufStat = MULTIBUFFER_DISABLED;
        }
    }

    /* Select new PP buffer index to use. If multibuffer is disabled, use
     * previous buffer, otherwise select new buffer from queue. */
    prevBufferIndex = pDecCont->ppControl.bufferIndex;
    if(pDecCont->ppConfigQuery.multiBuffer)
    {
        u32 buf0 = BQUEUE_UNUSED;
        /* In parallel mode 2, we must refrain from reusing future
         * anchor frame output buffer until it has been put out. */
        if(pDecCont->storage.parallelMode2)
        {
            buf0 = pDecCont->storage.pm2lockBuf;
            Vc1DecPpBuffer( &pDecCont->storage.decPp,
                            pDecCont->storage.workOut,
                            pDecCont->storage.picLayer.picType == PTYPE_B ||
                            pDecCont->storage.picLayer.picType == PTYPE_BI);
            Vc1DecPpSetPpProc( &pDecCont->storage.decPp, &pDecCont->ppControl );
            /*Vc1DecPpSetPpOutp( &pDecCont->ppControl );*/
        }
        else
        {
            nextBufferIndex = BqueueNext( &pDecCont->storage.bqPp,
                buf0,
                BQUEUE_UNUSED,
                BQUEUE_UNUSED,
                pDecCont->storage.picLayer.picType == PTYPE_B ||
                pDecCont->storage.picLayer.picType == PTYPE_BI);
            pDecCont->ppControl.bufferIndex = nextBufferIndex;
        }
    }
    else
    {
        nextBufferIndex = pDecCont->ppControl.bufferIndex = 0;
    }

    if(pDecCont->storage.parallelMode2)
    {
        /* Note: functionality moved to Vc1DecPpXXX() functions */
    }
    else if(pDecCont->storage.maxBframes == 0 ||
           (pDecCont->storage.picLayer.picType == PTYPE_B ||
            pDecCont->storage.picLayer.picType == PTYPE_BI))
    {
        pDecCont->ppControl.displayIndex = pDecCont->ppControl.bufferIndex;
    }
    else
    {
        pDecCont->ppControl.displayIndex = pDecCont->ppControl.prevAnchorDisplayIndex;
    }

    /* Connect PP output buffer to decoder output buffer */
    {
        u32 luma = 0;
        u32 chroma = 0;
        u32 workBuffer;

        if(pDecCont->storage.parallelMode2)
            workBuffer = pDecCont->storage.workOutPrev;
        else
            workBuffer = pDecCont->storage.workOut;

        luma = pDecCont->storage.pPicBuf[workBuffer].
                                data.busAddress;
        chroma = luma + ((pDecCont->storage.picWidthInMbs * 16) *
                        (pDecCont->storage.picHeightInMbs * 16));

        pDecCont->PPBufferData(pDecCont->ppInstance, 
            pDecCont->ppControl.bufferIndex, luma, chroma);
    }

    /* Check cases that will be processed in parallel or stand-alone */
    if (!pDecCont->ppControl.usePipeline)
    {
        /* Set minimum count for picture buffer handling.
         * Behaviour is same as with maxBframes > 0. */
        pDecCont->storage.minCount = 1;

        if(!pDecCont->storage.previousModeFull &&
           !pDecCont->storage.parallelMode2)
        {
            pDecCont->ppControl.bufferIndex = pDecCont->ppControl.displayIndex;
        }

        /* return if first frame of the sequence */
        if (pDecCont->storage.firstFrame)
            return;

        /* if parallel mode2 used, always process previous output picture */
        if(pDecCont->storage.parallelMode2)
        {
            ppIdx = pDecCont->storage.workOutPrev;

            /* If we got an anchor frame, lock the PP output buffer */
            if(!pDecCont->storage.previousB)
            {
                pDecCont->storage.pm2lockBuf = 
                    pDecCont->ppControl.bufferIndex;
            }

            if (pDecCont->storage.interlace)
            {
                /* deinterlace is ON -> both fields are processed
                 * at the same time */
                if (pDecCont->ppConfigQuery.deinterlace)
                {
                    /* wait until both fields are decoded */
                    if (!pDecCont->storage.picLayer.isFF &&
                        pDecCont->storage.picLayer.fcm == FIELD_INTERLACE)
                        return;

                    pPic[ppIdx].field[0].decPpStat = PARALLEL;
                    pPic[ppIdx].field[1].decPpStat = PARALLEL;
                    pDecCont->ppControl.picStruct =
                        DECPP_PIC_TOP_AND_BOT_FIELD_FRAME;
                }
                else /* field picture */
                {
                    pPic[ppIdx].field[0].decPpStat = PARALLEL;

                    if (pDecCont->storage.picLayer.fcm != FIELD_INTERLACE)
                    {
                        pPic[ppIdx].field[1].decPpStat = STAND_ALONE;
                    }
                    else
                    {
                        /* Note: here could be optimized */
                        pPic[ppIdx].field[1].decPpStat = STAND_ALONE;
                    }

                    pPic[ppIdx].field[0].ppBufferIndex = 
                        pDecCont->ppControl.bufferIndex;
                    pPic[ppIdx].field[1].ppBufferIndex = 
                        pDecCont->ppControl.bufferIndex;

                    if ( pDecCont->storage.picLayer.isFF ==
                         pPic[ppIdx].isTopFieldFirst )
                    {
                        pDecCont->ppControl.picStruct =
                            DECPP_PIC_TOP_FIELD_FRAME;
                    }
                    else
                    {
                        pDecCont->ppControl.picStruct =
                            DECPP_PIC_BOT_FIELD_FRAME;
                    }
                }
            }
            else /* progressive */
            {
                pPic[ppIdx].field[0].decPpStat = PARALLEL;
                pPic[ppIdx].field[1].decPpStat = PARALLEL;
                pDecCont->ppControl.picStruct =
                    DECPP_PIC_FRAME_OR_TOP_FIELD;
            }
        }
        /* if current picture is anchor frame -> start to
         * post-process previous anchor frame  */
        else if (pPic[wOut].field[0].type < 2)   /* I or P picture */
        {
            ppIdx = w0;
            if (pDecCont->storage.interlace)
            {
                /* deinterlace is ON -> both fields are processed
                 * at the same time */
                if (pDecCont->ppConfigQuery.deinterlace)
                {
                    /* wait until both fields are decoded */
                    if (!pDecCont->storage.picLayer.isFF &&
                        pDecCont->storage.picLayer.fcm == FIELD_INTERLACE)
                        return;

                    pPic[w0].field[0].decPpStat = PARALLEL;
                    pPic[w0].field[1].decPpStat = PARALLEL;
                    pDecCont->ppControl.picStruct =
                        DECPP_PIC_TOP_AND_BOT_FIELD_FRAME;
                }
                else /* field picture */
                {
                    pPic[w0].field[0].decPpStat = PARALLEL;

                    if (pDecCont->storage.picLayer.fcm != FIELD_INTERLACE)
                        pPic[w0].field[1].decPpStat = STAND_ALONE;
                    else
                        pPic[w0].field[1].decPpStat = PARALLEL;

                    if ( pDecCont->storage.picLayer.isFF ==
                         pPic[w0].isTopFieldFirst )
                    {
                        pDecCont->ppControl.picStruct =
                            DECPP_PIC_TOP_FIELD_FRAME;
                    }
                    else
                    {
                        pDecCont->ppControl.picStruct =
                            DECPP_PIC_BOT_FIELD_FRAME;
                    }
                }
            }
            else /* progressive */
            {
                pPic[w0].field[0].decPpStat = PARALLEL;
                pPic[w0].field[1].decPpStat = PARALLEL;
                pDecCont->ppControl.picStruct =
                    DECPP_PIC_FRAME_OR_TOP_FIELD;
            }
        }
        /* post-processing will be started as a stand-alone mode
         * from Vc1DecNextPicture. (interlace is ON and B-picture) */
        else
        {
            ASSERT(pDecCont->storage.picLayer.picType > 1); /* B or BI */

            pPic[wOut].field[0].decPpStat = STAND_ALONE;
            pPic[wOut].field[1].decPpStat = STAND_ALONE;
        }
	
    	/* Multibuffer semimode */
        if (pDecCont->ppConfigQuery.multiBuffer)
    	{
    	    pDecCont->ppControl.multiBufStat = MULTIBUFFER_SEMIMODE;
    	    pDecCont->storage.previousModeFull = 0;		
    	}
    	else
    	{   
    	    pDecCont->ppControl.multiBufStat = MULTIBUFFER_DISABLED;
    	}
    }
    
    if((pDecCont->storage.picLayer.picType == PTYPE_B) ||
       (pDecCont->storage.picLayer.picType == PTYPE_BI))
    {
        pDecCont->storage.previousB = 1;
    }
    else
    {
        pDecCont->storage.previousB = 0;
    }

    tmp = (pDecCont->storage.picWidthInMbs * 16) *
          (pDecCont->storage.picHeightInMbs * 16);

    /* fill rest of ppControl parameters */
    pDecCont->ppControl.inputBusLuma = pPic[ppIdx].data.busAddress;
    pDecCont->ppControl.inputBusChroma = pPic[ppIdx].data.busAddress + tmp;
    pDecCont->ppControl.bottomBusLuma = pPic[ppIdx].data.busAddress +
                                (pDecCont->storage.picWidthInMbs * 16);
    pDecCont->ppControl.bottomBusChroma =
                                    pDecCont->ppControl.bottomBusLuma + tmp;
    pDecCont->ppControl.littleEndian =
        GetDecRegister(pDecCont->vc1Regs, HWIF_DEC_OUT_ENDIAN);
    pDecCont->ppControl.wordSwap =
        GetDecRegister(pDecCont->vc1Regs, HWIF_DEC_OUTSWAP32_E);
    pDecCont->ppControl.inwidth = pDecCont->storage.picWidthInMbs * 16;
    pDecCont->ppControl.inheight = pDecCont->storage.picHeightInMbs * 16;
    pDecCont->ppControl.tiledInputMode = pPic[ppIdx].tiledMode;
    pDecCont->ppControl.progressiveSequence = !pDecCont->storage.interlace;
    pDecCont->ppControl.croppedW = ((pPic[ppIdx].codedWidth+7) & ~7);
    pDecCont->ppControl.croppedH = ((pPic[ppIdx].codedHeight+7) & ~7);

    if ( (pDecCont->ppControl.picStruct == DECPP_PIC_TOP_FIELD_FRAME) ||
         (pDecCont->ppControl.picStruct == DECPP_PIC_BOT_FIELD_FRAME) )
    {
        /* input height is multiple of 16 */
        pDecCont->ppControl.inheight >>= 1;
        pDecCont->ppControl.inheight=((pDecCont->ppControl.inheight+15) & ~15);
        /* cropped height is multiple of 8 */
        pDecCont->ppControl.croppedH >>= 1;
        pDecCont->ppControl.croppedH=((pDecCont->ppControl.croppedH + 7) & ~7);
    }
    pDecCont->ppControl.rangeRed = pPic[ppIdx].rangeRedFrm;
    pDecCont->ppControl.rangeMapYEnable = pPic[ppIdx].rangeMapYFlag;
    pDecCont->ppControl.rangeMapYCoeff = pPic[ppIdx].rangeMapY;
    pDecCont->ppControl.rangeMapCEnable = pPic[ppIdx].rangeMapUvFlag;
    pDecCont->ppControl.rangeMapCCoeff = pPic[ppIdx].rangeMapUv;


    /* run pp if possible */
    tmp = pDecCont->storage.picLayer.isFF ^ 1;
    if ( pPic[ppIdx].field[tmp].decPpStat != STAND_ALONE)
    {
        ASSERT(pDecCont->ppControl.ppStatus != DECPP_RUNNING);

        pDecCont->PPRun( pDecCont->ppInstance, &pDecCont->ppControl );
        pDecCont->ppControl.ppStatus = DECPP_RUNNING;

#ifdef _DEC_PP_USAGE
    PrintDecPpUsage(pDecCont,
                    pDecCont->storage.picLayer.isFF,
                    ppIdx,
                    HANTRO_FALSE,
                    pPic[ppIdx].field[tmp].picId);
#endif
    }

    if( pDecCont->storage.picLayer.picType != PTYPE_B &&
        pDecCont->storage.picLayer.picType != PTYPE_BI )
    {
        pDecCont->ppControl.prevAnchorDisplayIndex = nextBufferIndex;
    }

    if( pDecCont->ppControl.inputBusLuma == 0 &&
        pDecCont->storage.parallelMode2 )
    {
        BqueueDiscard( &pDecCont->storage.bqPp,
                       pDecCont->ppControl.bufferIndex );
    }

}


/*------------------------------------------------------------------------------

    Function name: SetReferenceBaseAddress

        Functional description:
            Updates base addresses of reference pictures

        Inputs:
            pDecCont        Decoder container

        Outputs:
            None

        Returns:
            None

------------------------------------------------------------------------------*/
void SetReferenceBaseAddress(decContainer_t *pDecCont)
{
    u32 tmp, tmp1;
    if (pDecCont->storage.maxBframes)
    {
        /* forward reference */
        if (pDecCont->storage.picLayer.picType == PTYPE_B)
        {
            if ( (pDecCont->storage.picLayer.fcm == FIELD_INTERLACE) )
            {
                if (pDecCont->storage.picLayer.isFF == HANTRO_TRUE)
                {
                    /* Use 1st and 2nd fields of temporally
                     * previous anchor frame */
                    SetDecRegister(pDecCont->vc1Regs, HWIF_REFER0_BASE,
                        (u32)(pDecCont->storage.
                                pPicBuf[pDecCont->storage.work1].
                                data.busAddress));
                    SetDecRegister(pDecCont->vc1Regs, HWIF_REFER1_BASE,
                        (u32)(pDecCont->storage.
                                pPicBuf[pDecCont->storage.work1].
                                data.busAddress));
                }
                else
                {
                    /* Use 1st field of current frame and 2nd
                     * field of prev frame */
                    if( pDecCont->storage.picLayer.tff )
                    {
                        SetDecRegister(pDecCont->vc1Regs, HWIF_REFER0_BASE,
                            (u32)(pDecCont->storage.
                                    pPicBuf[pDecCont->storage.workOut].
                                    data.busAddress));
                        SetDecRegister(pDecCont->vc1Regs, HWIF_REFER1_BASE,
                            (u32)(pDecCont->storage.
                                    pPicBuf[pDecCont->storage.work1].
                                    data.busAddress));
                    }
                    else
                    {
                        SetDecRegister(pDecCont->vc1Regs, HWIF_REFER0_BASE,
                            (u32)(pDecCont->storage.
                                    pPicBuf[pDecCont->storage.work1].
                                    data.busAddress));
                        SetDecRegister(pDecCont->vc1Regs, HWIF_REFER1_BASE,
                            (u32)(pDecCont->storage.
                                    pPicBuf[pDecCont->storage.workOut].
                                    data.busAddress));
                    }
                }
            }
            else
            {
                SetDecRegister(pDecCont->vc1Regs, HWIF_REFER0_BASE,
                    (u32)(pDecCont->storage.pPicBuf[pDecCont->storage.work1].
                          data.busAddress));
                SetDecRegister(pDecCont->vc1Regs, HWIF_REFER1_BASE,
                    (u32)(pDecCont->storage.pPicBuf[pDecCont->storage.work1].
                          data.busAddress));

            }
        }
        else /* PTYPE_P */
        {
            if ( (pDecCont->storage.picLayer.fcm == FIELD_INTERLACE) )
            {
                if( pDecCont->storage.picLayer.numRef == 0 )
                {
                    if( pDecCont->storage.picLayer.isFF == HANTRO_FALSE &&
                        pDecCont->storage.picLayer.refField == 0 )
                    {
                        tmp = pDecCont->storage.workOut;
                    }
                    else
                    {
                        tmp = pDecCont->storage.work0;
                    }
                    SetDecRegister(pDecCont->vc1Regs, HWIF_REFER0_BASE,
                        (u32)(pDecCont->storage.pPicBuf[tmp].data.busAddress));
                    SetDecRegister(pDecCont->vc1Regs, HWIF_REFER1_BASE,
                        (u32)(pDecCont->storage.pPicBuf[tmp].data.busAddress));
                }
                else /* NUMREF == 1 */
                {
                    if( pDecCont->storage.picLayer.isFF == HANTRO_TRUE )
                    {
                        /* Use both fields from previous frame */
                        tmp = pDecCont->storage.work0;
                        tmp1 = pDecCont->storage.work0;
                    }
                    else
                    {
                        /* Use previous field from this frame and second field
                         * from previous frame */
                        if( pDecCont->storage.picLayer.tff )
                        {
                            tmp = pDecCont->storage.workOut;
                            tmp1 = pDecCont->storage.work0;
                        }
                        else
                        {
                            tmp = pDecCont->storage.work0;
                            tmp1 = pDecCont->storage.workOut;
                        }
                    }
                    SetDecRegister(pDecCont->vc1Regs, HWIF_REFER0_BASE,
                        (u32)(pDecCont->storage.pPicBuf[tmp].data.busAddress));
                    SetDecRegister(pDecCont->vc1Regs, HWIF_REFER1_BASE,
                        (u32)(pDecCont->storage.pPicBuf[tmp1].data.busAddress));
                }
            }
            else
            {
                SetDecRegister(pDecCont->vc1Regs, HWIF_REFER0_BASE,
                    (u32)(pDecCont->storage.pPicBuf[pDecCont->storage.work0].
                          data.busAddress));
                SetDecRegister(pDecCont->vc1Regs, HWIF_REFER1_BASE,
                    (u32)(pDecCont->storage.pPicBuf[pDecCont->storage.work0].
                          data.busAddress));
            }
        }

        /* backward reference */
        SetDecRegister(pDecCont->vc1Regs, HWIF_REFER2_BASE,
            (u32)(pDecCont->storage.pPicBuf[pDecCont->storage.work0].
                  data.busAddress));

        SetDecRegister(pDecCont->vc1Regs, HWIF_REFER3_BASE,
            (u32)(pDecCont->storage.pPicBuf[pDecCont->storage.work0].
                  data.busAddress));

    }
    else
    {
        SetDecRegister(pDecCont->vc1Regs, HWIF_REFER0_BASE,
            (u32)(pDecCont->storage.pPicBuf[pDecCont->storage.work0].
                data.busAddress));
        SetDecRegister(pDecCont->vc1Regs, HWIF_REFER1_BASE, 0);
        SetDecRegister(pDecCont->vc1Regs, HWIF_REFER2_BASE, 0);
        SetDecRegister(pDecCont->vc1Regs, HWIF_REFER3_BASE, 0);
    }

}

/*------------------------------------------------------------------------------

    Function name: SetIntensityCompensationParameters

        Functional description:
            Updates intensity compensation parameters

        Inputs:
            pDecCont        Decoder container

        Outputs:
            None

        Returns:
            None

------------------------------------------------------------------------------*/
void SetIntensityCompensationParameters(decContainer_t *pDecCont)
{
    picture_t *pPic;
    u32 w0;
    u32 w1;
    u32 firstTop;

    pPic = (picture_t*)pDecCont->storage.pPicBuf;

    if (pDecCont->storage.picLayer.picType == PTYPE_B)
    {
        w0   = pDecCont->storage.work0;
        w1   = pDecCont->storage.work1;
    }
    else
    {
        w0   = pDecCont->storage.workOut;
        w1   = pDecCont->storage.work0;
    }

    /* first disable all */
    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP0_E, 0);
    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP1_E, 0);
    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP2_E, 0);
    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP3_E, 0);
    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP4_E, 0);

    SetDecRegister(pDecCont->vc1Regs, HWIF_REFTOPFIRST_E,
        pPic[w1].isTopFieldFirst);

    /* frame coded */
    if (pPic[w0].fcm != FIELD_INTERLACE)
    {
        /* set first */
        if (pPic[w0].field[0].intCompF == IC_BOTH_FIELDS)
        {
            SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP0_E, 1);
            SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE0,
                            (u32)pPic[w0].field[0].iScaleA);
            SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT0,
                            (u32)pPic[w0].field[0].iShiftA);
        }
        /* field picture */
        if (pPic[w1].fcm == FIELD_INTERLACE)
        {
            /* set second */
            if (pPic[w1].isTopFieldFirst)
            {
                if ( (pPic[w1].field[1].intCompF == IC_BOTH_FIELDS) ||
                     (pPic[w1].field[1].intCompF == IC_TOP_FIELD) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP1_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE1,
                                    (u32)pPic[w1].field[1].iScaleA);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT1,
                                    (u32)pPic[w1].field[1].iShiftA);
                }
            }
            else /* bottom field first */
            {
                if ( (pPic[w1].field[1].intCompF == IC_BOTH_FIELDS) ||
                     (pPic[w1].field[1].intCompF == IC_BOTTOM_FIELD) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP1_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE1,
                                    (u32)pPic[w1].field[1].iScaleB);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT1,
                                    (u32)pPic[w1].field[1].iShiftB);
                }
            }
        }
    }
    else /* field */
    {
        if (pPic[w0].isFirstField)
        {
            if (pPic[w0].isTopFieldFirst)
            {
                firstTop = HANTRO_TRUE;
                if ( (pPic[w0].field[0].intCompF == IC_BOTTOM_FIELD) ||
                     (pPic[w0].field[0].intCompF == IC_BOTH_FIELDS) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP0_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE0,
                                    (u32)pPic[w0].field[0].iScaleB);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT0,
                                    (u32)pPic[w0].field[0].iShiftB);
                }

                if ( (pPic[w0].field[0].intCompF == IC_TOP_FIELD) ||
                     (pPic[w0].field[0].intCompF == IC_BOTH_FIELDS) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP1_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE1,
                                    (u32)pPic[w0].field[0].iScaleA);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT1,
                                    (u32)pPic[w0].field[0].iShiftA);
                }

            }
            else /* bottom field first */
            {
                firstTop = HANTRO_FALSE;
                if ( (pPic[w0].field[0].intCompF == IC_TOP_FIELD) ||
                     (pPic[w0].field[0].intCompF == IC_BOTH_FIELDS) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP0_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE0,
                                    (u32)pPic[w0].field[0].iScaleA);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT0,
                                    (u32)pPic[w0].field[0].iShiftA);
                }

                if ( (pPic[w0].field[0].intCompF == IC_BOTTOM_FIELD) ||
                     (pPic[w0].field[0].intCompF == IC_BOTH_FIELDS) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP1_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE1,
                                    (u32)pPic[w0].field[0].iScaleB);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT1,
                                    (u32)pPic[w0].field[0].iShiftB);
                }
            }

            if (pPic[w1].fcm == FIELD_INTERLACE)
            {
                if (firstTop)
                {
                    if ( (pPic[w1].field[1].intCompF == IC_TOP_FIELD) ||
                         (pPic[w1].field[1].intCompF == IC_BOTH_FIELDS) )
                    {
                        SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP2_E, 1);
                        SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE2,
                                        (u32)pPic[w1].field[1].iScaleA);
                        SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT2,
                                        (u32)pPic[w1].field[1].iShiftA);
                    }
                }
                else
                {
                    if ( (pPic[w1].field[1].intCompF == IC_BOTTOM_FIELD) ||
                         (pPic[w1].field[1].intCompF == IC_BOTH_FIELDS) )
                    {
                        SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP2_E, 1);
                        SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE2,
                                        (u32)pPic[w1].field[1].iScaleB);
                        SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT2,
                                        (u32)pPic[w1].field[1].iShiftB);
                    }
                }
            }
        }
        else /* second field */
        {
            if (pPic[w0].isTopFieldFirst)
            {
                firstTop = HANTRO_TRUE;
                if ( (pPic[w0].field[1].intCompF == IC_TOP_FIELD) ||
                     (pPic[w0].field[1].intCompF == IC_BOTH_FIELDS) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP0_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE0,
                                    (u32)pPic[w0].field[1].iScaleA);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT0,
                                    (u32)pPic[w0].field[1].iShiftA);
                }

                if ( (pPic[w0].field[1].intCompF == IC_BOTTOM_FIELD) ||
                     (pPic[w0].field[1].intCompF == IC_BOTH_FIELDS) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP1_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE1,
                                    (u32)pPic[w0].field[1].iScaleB);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT1,
                                    (u32)pPic[w0].field[1].iShiftB);
                }

                if ( (pPic[w0].field[0].intCompF == IC_BOTTOM_FIELD) ||
                     (pPic[w0].field[0].intCompF == IC_BOTH_FIELDS) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP2_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE2,
                                    (u32)pPic[w0].field[0].iScaleB);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT2,
                                    (u32)pPic[w0].field[0].iShiftB);
                }

                if ( (pPic[w0].field[0].intCompF == IC_TOP_FIELD) ||
                     (pPic[w0].field[0].intCompF == IC_BOTH_FIELDS) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP3_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE3,
                                    (u32)pPic[w0].field[0].iScaleA);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT3,
                                    (u32)pPic[w0].field[0].iShiftA);
                }

                if (pPic[w1].fcm == FIELD_INTERLACE)
                {
                    if ( (pPic[w1].field[1].intCompF == IC_TOP_FIELD) ||
                         (pPic[w1].field[1].intCompF == IC_BOTH_FIELDS) )
                    {
                        SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP4_E, 1);
                        SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE4,
                                        (u32)pPic[w1].field[1].iScaleA);
                        SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT4,
                                        (u32)pPic[w1].field[1].iShiftA);
                    }
                }

            }
            else /* bottom field first */
            {
                firstTop = HANTRO_FALSE;
                if ( (pPic[w0].field[1].intCompF == IC_BOTTOM_FIELD) ||
                     (pPic[w0].field[1].intCompF == IC_BOTH_FIELDS) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP0_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE0,
                                    (u32)pPic[w0].field[1].iScaleB);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT0,
                                    (u32)pPic[w0].field[1].iShiftB);
                }

                if ( (pPic[w0].field[1].intCompF == IC_TOP_FIELD) ||
                     (pPic[w0].field[1].intCompF == IC_BOTH_FIELDS) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP1_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE1,
                                    (u32)pPic[w0].field[1].iScaleA);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT1,
                                    (u32)pPic[w0].field[1].iShiftA);
                }

                if ( (pPic[w0].field[0].intCompF == IC_TOP_FIELD) ||
                     (pPic[w0].field[0].intCompF == IC_BOTH_FIELDS) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP2_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE2,
                                    (u32)pPic[w0].field[0].iScaleA);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT2,
                                    (u32)pPic[w0].field[0].iShiftA);
                }

                if ( (pPic[w0].field[0].intCompF == IC_BOTTOM_FIELD) ||
                     (pPic[w0].field[0].intCompF == IC_BOTH_FIELDS) )
                {
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP3_E, 1);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE3,
                                    (u32)pPic[w0].field[0].iScaleB);
                    SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT3,
                                    (u32)pPic[w0].field[0].iShiftB);
                }

                if (pPic[w1].fcm == FIELD_INTERLACE)
                {
                    if ( (pPic[w1].field[1].intCompF == IC_BOTTOM_FIELD) ||
                         (pPic[w1].field[1].intCompF == IC_BOTH_FIELDS) )
                    {
                        SetDecRegister(pDecCont->vc1Regs, HWIF_ICOMP4_E, 1);
                        SetDecRegister(pDecCont->vc1Regs, HWIF_ISCALE4,
                                        (u32)pPic[w1].field[1].iScaleB);
                        SetDecRegister(pDecCont->vc1Regs, HWIF_ISHIFT4,
                                        (u32)pPic[w1].field[1].iShiftB);
                    }
                }

            }
        }
    }
}
#ifdef _DEC_PP_USAGE
/*------------------------------------------------------------------------------

    Function name: PrintDecPpUsage

        Functional description:

        Inputs:
            pDecCont    Pointer to decoder structure
            ff          Is current field first or second field of the frame
            picIndex    Picture buffer index
            decStatus   DEC / PP print
            picId       Picture ID of the field/frame

        Outputs:
            None

        Returns:
            None

------------------------------------------------------------------------------*/
void PrintDecPpUsage( decContainer_t *pDecCont,
                      u32 ff,
                      u32 picIndex,
                      u32 decStatus,
                      u32 picId)
{
    FILE *fp;
    picture_t *pPic;
    pPic = (picture_t*)pDecCont->storage.pPicBuf;

    fp = fopen("dec_pp_usage.txt", "at");
    if (fp == NULL)
        return;

    if (decStatus)
    {
        if (pDecCont->storage.picLayer.isFF)
        {
            fprintf(fp, "\n======================================================================\n");

            fprintf(fp, "%10s%10s%10s%10s%10s%10s%10s\n",
                    "Component", "PicId", "PicType", "Fcm", "Field",
                    "PPMode", "BuffIdx");
        }
        /* Component and PicId */
        fprintf(fp, "\n%10.10s%10d", "DEC", picId);
        /* Pictype */
        switch (pDecCont->storage.picLayer.picType)
        {
            case PTYPE_P: fprintf(fp, "%10s","P"); break;
            case PTYPE_I: fprintf(fp, "%10s","I"); break;
            case PTYPE_B: fprintf(fp, "%10s","B"); break;
            case PTYPE_BI: fprintf(fp, "%10s","BI"); break;
            case PTYPE_Skip: fprintf(fp, "%10s","Skip"); break;
        }
        /* Frame coding mode */
        switch (pDecCont->storage.picLayer.fcm)
        {
            case PROGRESSIVE: fprintf(fp, "%10s","PR"); break;
            case FRAME_INTERLACE: fprintf(fp, "%10s","FR"); break;
            case FIELD_INTERLACE: fprintf(fp, "%10s","FI"); break;
        }
        /* Field */
        if (pDecCont->storage.picLayer.topField)
            fprintf(fp, "%10s","TOP");
        else
            fprintf(fp, "%10s","BOT");

        /* PPMode and BuffIdx */
       /* fprintf(fp, "%10s%10d\n", "---",picIndex);*/
        switch (pDecCont->ppControl.multiBufStat)
        {
            case MULTIBUFFER_FULLMODE:
                fprintf(fp, "%10s%10d\n", "FULL",picIndex);
                break;
            case MULTIBUFFER_SEMIMODE:
                fprintf(fp, "%10s%10d\n", "SEMI",picIndex);
                break;
            case MULTIBUFFER_DISABLED:
                fprintf(fp, "%10s%10d\n", "DISA",picIndex);
                break;
            case MULTIBUFFER_UNINIT:
                fprintf(fp, "%10s%10d\n", "UNIN",picIndex);
                break;
            default:
                break;
        }
    }
    else /* pp status */
    {
        /* Component and PicId */
        fprintf(fp, "%10s%10d", "PP", picId);
        /* Pictype */
        switch (pPic[picIndex].field[!ff].type)
        {
            case PTYPE_P: fprintf(fp, "%10s","P"); break;
            case PTYPE_I: fprintf(fp, "%10s","I"); break;
            case PTYPE_B: fprintf(fp, "%10s","B"); break;
            case PTYPE_BI: fprintf(fp, "%10s","BI"); break;
            case PTYPE_Skip: fprintf(fp, "%10s","Skip"); break;
        }
        /* Frame coding mode */
        switch (pPic[picIndex].fcm)
        {
            case PROGRESSIVE: fprintf(fp, "%10s","PR"); break;
            case FRAME_INTERLACE: fprintf(fp, "%10s","FR"); break;
            case FIELD_INTERLACE: fprintf(fp, "%10s","FI"); break;
        }
        /* Field */
        if (pPic[picIndex].isTopFieldFirst == ff)
            fprintf(fp, "%10s","TOP");
        else
            fprintf(fp, "%10s","BOT");

        /* PPMode and BuffIdx */
        switch (pPic[picIndex].field[!ff].decPpStat)
        {
            case STAND_ALONE:
                fprintf(fp, "%10s%10d\n", "STAND",picIndex);
                break;
            case PARALLEL:
                fprintf(fp, "%10s%10d\n", "PARAL",picIndex);
                break;
            case PIPELINED:
                fprintf(fp, "%10s%10d\n", "PIPEL",picIndex);
                break;
            default:
                break;
        }
    }

    if (fp)
    {
        fclose(fp);
        fp = NULL;
    }
}
#endif

/*------------------------------------------------------------------------------
    Vc1DecPpInit()

        Setup dec+pp bookkeeping
------------------------------------------------------------------------------*/
void Vc1DecPpInit( vc1DecPp_t *pDecPp, bufferQueue_t *bq )
{
    pDecPp->decOut.dec = BQUEUE_UNUSED;
    pDecPp->ppProc.dec = BQUEUE_UNUSED;
    pDecPp->anchor.pp = BQUEUE_UNUSED;
    pDecPp->bqPp = bq;
}

/*------------------------------------------------------------------------------
    Vc1DecPpSetFieldOutput()

        Setup dec+pp bookkeeping
------------------------------------------------------------------------------*/
void Vc1DecPpSetFieldOutput( vc1DecPp_t *pDecPp, u32 fieldOutput )
{
    pDecPp->fieldOutput = fieldOutput;
}

/*------------------------------------------------------------------------------
    Vc1DecPpNextInput()

        Signal new input frame for dec+pp bookkeeping
------------------------------------------------------------------------------*/
void Vc1DecPpNextInput( vc1DecPp_t *pDecPp, u32 framePic )
{
    pDecPp->decOut.processed = 0;
    pDecPp->decOut.framePic = framePic;
}

/*------------------------------------------------------------------------------
    Vc1DecPpBuffer()

        Buffer picture for dec+pp bookkeeping
------------------------------------------------------------------------------*/
void Vc1DecPpBuffer( vc1DecPp_t *pDecPp, u32 decOut, u32 bFrame )
{
    pDecPp->decOut.dec    = decOut;
    if(!pDecPp->fieldOutput)
        pDecPp->decOut.processed = 2;
    else
        pDecPp->decOut.processed++;
    pDecPp->decOut.bFrame = bFrame;
}

/*------------------------------------------------------------------------------
    Vc1DecPpSetPpOutp()

        Setup output frame for dec+pp bookkeeping
------------------------------------------------------------------------------*/
void Vc1DecPpSetPpOutp( vc1DecPp_t *pDecPp, DecPpInterface *pc )
{

    if(pDecPp->ppProc.bFrame == 1 )
    {
        pDecPp->ppOut = pDecPp->ppProc;
    }
    else
    {
        pDecPp->ppOut = pDecPp->anchor;
    }

    pc->displayIndex = pDecPp->ppOut.pp;
}

/*------------------------------------------------------------------------------
    Vc1DecPpSetPpOutpStandalone()

        Setup last frame for dec+pp bookkeeping
------------------------------------------------------------------------------*/
void Vc1DecPpSetPpOutpStandalone( vc1DecPp_t *pDecPp, DecPpInterface *pc )
{
    pc->bufferIndex = pDecPp->ppOut.pp;
    pc->displayIndex = pDecPp->ppOut.pp;
}

/*------------------------------------------------------------------------------
    Vc1DecPpSetPpProc()

        Setup dec+pp bookkeeping; next frame/field for PP to process
------------------------------------------------------------------------------*/
void Vc1DecPpSetPpProc( vc1DecPp_t *pDecPp, DecPpInterface *pc )
{
    u32 firstField = 0;


    if(pDecPp->ppProc.dec != BQUEUE_UNUSED) 
    {
        if(pDecPp->ppProc.processed == 0)
        {
            /* allocate new PP buffer */
            pDecPp->ppProc.pp = BqueueNext( pDecPp->bqPp,
                pDecPp->anchor.pp,
                BQUEUE_UNUSED,
                BQUEUE_UNUSED,
                pDecPp->ppProc.bFrame);

            pc->bufferIndex = pDecPp->ppProc.pp;

            pDecPp->ppProc.processed = pDecPp->fieldOutput ? 1 : 2;
            firstField = 1;
        }
        else
        {
            pDecPp->ppProc.processed = 2;
        }
    }

    if(firstField)
        Vc1DecPpSetPpOutp( pDecPp, pc );

    /* Take anchor */
    if( pDecPp->ppProc.bFrame == 0 )
    {
        pDecPp->anchor = pDecPp->ppProc;
    }

    /* roll over to next picture buffer */
    if( pDecPp->decOut.processed == 2 ||
        pDecPp->decOut.framePic )
    {
        pDecPp->ppProc = pDecPp->decOut;
        pDecPp->ppProc.processed = 0;
        pDecPp->ppOut.processed = 0;
        pDecPp->decOut.dec = BQUEUE_UNUSED;
    }
}

