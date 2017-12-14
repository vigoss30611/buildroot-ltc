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
--  Description :  H264 decoder and PP pipeline support
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264_pp_pipeline.c,v $
--  $Revision: 1.17 $
--  $Date: 2010/10/05 08:46:03 $
--
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "h264_pp_pipeline.h"
#include "h264_pp_multibuffer.h"
#include "h264hwd_container.h"
#include "h264hwd_debug.h"
#include "h264hwd_util.h"

#ifndef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          do{}while(0)
#else
#undef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          printf(__VA_ARGS__)
#endif

/*------------------------------------------------------------------------------
    Function name   : h264RegisterPP
    Description     :
    Return type     : i32
    Argument        : const void * decInst
    Argument        : const void  *ppInst
    Argument        : (*PPRun)(const void *)
    Argument        : void (*PPEndCallback)(const void *)
------------------------------------------------------------------------------*/
i32 h264RegisterPP(const void *decInst, const void *ppInst,
                   void (*PPDecStart) (const void *, const DecPpInterface *),
                   void (*PPDecWaitEnd) (const void *),
                   void (*PPConfigQuery) (const void *, DecPpQuery *),
                   void (*PPDisplayIndex) (const void *, u32))
{
    decContainer_t *pDecCont;

    pDecCont = (decContainer_t *) decInst;

    if(decInst == NULL || pDecCont->pp.ppInstance != NULL ||
       ppInst == NULL || PPDecStart == NULL || PPDecWaitEnd == NULL
       || PPConfigQuery == NULL)
    {
        TRACE_PP_CTRL("h264RegisterPP: Invalid parameter\n");
        return -1;
    }

    if(pDecCont->asicRunning)
    {
        TRACE_PP_CTRL("h264RegisterPP: Illegal action, asicRunning\n");
        return -2;
    }

    pDecCont->pp.ppInstance = ppInst;
    pDecCont->pp.PPConfigQuery = PPConfigQuery;
    pDecCont->pp.PPDecStart = PPDecStart;
    pDecCont->pp.PPDecWaitEnd = PPDecWaitEnd;
    pDecCont->pp.PPNextDisplayId = PPDisplayIndex;

    pDecCont->pp.decPpIf.ppStatus = DECPP_IDLE;

    pDecCont->pp.ppInfo.multiBuffer = 0;
    h264PpMultiInit(pDecCont, 0);

    pDecCont->storage.ppUsed = 1;

    TRACE_PP_CTRL("h264RegisterPP: Connected to PP instance 0x%08x\n",
                  (size_t) ppInst);

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : h264UnregisterPP
    Description     :
    Return type     : i32
    Argument        : const void * decInst
    Argument        : const void  *ppInst
------------------------------------------------------------------------------*/
i32 h264UnregisterPP(const void *decInst, const void *ppInst)
{
    decContainer_t *pDecCont;

    pDecCont = (decContainer_t *) decInst;

    ASSERT(decInst != NULL && ppInst == pDecCont->pp.ppInstance);

    if(ppInst != pDecCont->pp.ppInstance)
    {
        TRACE_PP_CTRL("h264UnregisterPP: Invalid parameter\n");
        return -1;
    }

    if(pDecCont->asicRunning)
    {
        TRACE_PP_CTRL("h264UnregisterPP: Illegal action, asicRunning\n");
        return -2;
    }

    pDecCont->pp.ppInstance = NULL;
    pDecCont->pp.PPConfigQuery = NULL;
    pDecCont->pp.PPDecStart = NULL;
    pDecCont->pp.PPDecWaitEnd = NULL;

    TRACE_PP_CTRL("h264UnregisterPP: Disconnected from PP instance 0x%08x\n",
                  (size_t) ppInst);

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : h264PreparePpRun
    Description     :
    Return type     : void
    Argument        : decContainer_t * pDecCont
------------------------------------------------------------------------------*/
void h264PreparePpRun(decContainer_t * pDecCont)
{
    const storage_t *pStorage = &pDecCont->storage;
    DecPpInterface *decPpIf = &pDecCont->pp.decPpIf;
    const dpbStorage_t *dpb = pStorage->dpbs[pStorage->outView];
    u32 multiView;

    /* decoding second field of field coded picture -> queue current picture
     * for post processing */
    if (decPpIf->ppStatus == DECPP_PIC_NOT_FINISHED &&
        !pStorage->secondField &&
        pDecCont->pp.ppInfo.multiBuffer)
    {
        pDecCont->pp.queuedPicToPp[pStorage->view] =
            pStorage->dpb->currentOut->data;
    }

    /* PP not connected or still running (not waited when first field of frame
     * finished */
    if(pDecCont->pp.ppInstance == NULL ||
       decPpIf->ppStatus == DECPP_PIC_NOT_FINISHED)
    {
        return;
    }

    if(dpb->noReordering && !dpb->numOut)
    {
        TRACE_PP_CTRL
            ("h264PreparePpRun: No reordering and no output => PP no run\n");
        /* decoder could create fake pictures in DPB, which will never go to output.
         * this is the case when new access unit detetcted but no valid slice decoded.
         */

        return;
    }

    if(pDecCont->pp.ppInfo.multiBuffer == 0)    /* legacy single buffer mode */
    {
        /* in some situations, like field decoding,
         * the dpb->numOut is not reset, but, dpb->numOut == dpb->outIndex */
        if(dpb->numOut != 0)   /* we have output */
        {
            TRACE_PP_CTRL
                ("h264PreparePpRun: output picture => PP could run!\n");

            if(dpb->outBuf[dpb->outIndexR].data == pStorage->currImage->data)
            {
                /* we could have pipeline */
                TRACE_PP_CTRL("h264PreparePpRun: decode == output\n");

                if(pStorage->activeSps->frameMbsOnlyFlag == 0 &&
                   pStorage->secondField)
                {
                    TRACE_PP_CTRL
                        ("h264PreparePpRun: first field only! Do not run PP, yet!\n");
                    return;
                }

                if(pStorage->currImage->picStruct != FRAME)
                {
                    u32 opposit_field = pStorage->currImage->picStruct ^ 1;

                    if(dpb->currentOut->status[opposit_field] != EMPTY)
                    {
                        decPpIf->usePipeline = 0;
                        TRACE_PP_CTRL
                            ("h264PreparePpRun: second field of frame! Pipeline cannot be used!\n");
                    }
                }
                else if(!pStorage->activeSps->mbAdaptiveFrameFieldFlag)
                {
                    decPpIf->usePipeline =
                        pDecCont->pp.ppInfo.pipelineAccepted & 1;
                }
                else
                    decPpIf->usePipeline = 0;

                if(decPpIf->usePipeline)
                {
                    TRACE_PP_CTRL
                        ("h264PreparePpRun: pipeline=ON => PP will be running\n");
                    decPpIf->ppStatus = DECPP_RUNNING;

                    if(!pStorage->prevNalUnit->nalRefIdc)
                    {
                        pDecCont->asicBuff->disableOutWriting = 1;
                        TRACE_PP_CTRL
                            ("h264PreparePpRun: Not reference => Disable decoder output writing\n");
                    }
                }
                else
                {
                    TRACE_PP_CTRL
                        ("h264PreparePpRun: pipeline=OFF => PP has to run after DEC\n");
                }
            }
            else
            {
                decPpIf->ppStatus = DECPP_RUNNING;
                decPpIf->usePipeline = 0;

                TRACE_PP_CTRL
                    ("h264PreparePpRun: decode != output => pipeline=OFF => PP run in parallel\n");
            }

        }
        else
        {
            TRACE_PP_CTRL
                ("h264PreparePpRun: no output picture => PP no run!\n");
        }

        return;
    }

    /* post process picture of current view. If decoding base view and only
     * stereo view  picture available for post processing -> pp
     *  (parallel dec+pp case) (needed for field coded pictures to start
     *  pp for stereo view when starting to decode second field of base view,
     *  base view pp'd while decoding first fields of base and stereo view) */
    multiView = pStorage->view;
    if (pStorage->view == 0 &&
        pDecCont->pp.queuedPicToPp[pStorage->view] == NULL &&
        pDecCont->pp.queuedPicToPp[!pStorage->view] != NULL)
        multiView = !pStorage->view;
    dpb = pDecCont->storage.dpbs[pStorage->view];

    /* multibuffer mode */
    TRACE_PP_CTRL("h264PreparePpRun: MULTIBUFFER!\n");
    if(pDecCont->pp.queuedPicToPp[multiView] == NULL &&
       pStorage->activeSps->frameMbsOnlyFlag == 0 && pStorage->secondField)
    {
        TRACE_PP_CTRL
            ("h264PreparePpRun: no queued picture and first field only! Do not run PP, yet!\n");
        return;
    }

    if(pDecCont->pp.queuedPicToPp[multiView] == NULL &&
       pStorage->currentMarked == HANTRO_FALSE)
    {
        TRACE_PP_CTRL
            ("h264PreparePpRun: no queued picture and current pic NOT for display! Do not run PP!\n");
        return;
    }

    if(pStorage->currImage->picStruct != FRAME)
    {

        TRACE_PP_CTRL("h264PreparePpRun: 2 Fields! Pipeline cannot be used!\n");

        /* we shall not have EMPTY output marked for display, missing fields are acceptable */
        ASSERT((pDecCont->pp.queuedPicToPp[multiView] != NULL) ||
               (dpb->currentOut->status[0] != EMPTY) ||
               (dpb->currentOut->status[1] != EMPTY));

        decPpIf->usePipeline = 0;
    }
    else if(!pStorage->activeSps->mbAdaptiveFrameFieldFlag)
    {
        decPpIf->usePipeline = pDecCont->pp.ppInfo.pipelineAccepted & 1;
    }
    else
        decPpIf->usePipeline = 0;

    if(decPpIf->usePipeline)
    {
        decPpIf->ppStatus = DECPP_RUNNING;
        TRACE_PP_CTRL
            ("h264PreparePpRun: pipeline=ON => PP will be running\n");
    }
    else
    {
        if(pDecCont->pp.queuedPicToPp[multiView] != NULL)
        {
            decPpIf->ppStatus = DECPP_RUNNING;
            TRACE_PP_CTRL
                ("h264PreparePpRun: pipeline=OFF, queued picture => PP run in parallel\n");
        }
        else
        {
            TRACE_PP_CTRL
                ("h264PreparePpRun: pipeline=OFF, no queued picture => PP no run\n");
            /* do not process pictures added to DPB but not intended for display */
            if(dpb->noReordering ||
               dpb->currentOut->toBeDisplayed == HANTRO_TRUE)
            {
                if(!pStorage->secondField)  /* do not queue first field */
                {
                    pDecCont->pp.queuedPicToPp[pStorage->view] = dpb->currentOut->data;
                }
            }
        }
    }
}

/*------------------------------------------------------------------------------
    Function name   : h264PpMultiAddPic
    Description     : Add a new picture to the PP processed table (frist free place).
                      Return the tabel position where added.
    Return type     : u32
    Argument        : decContainer_t * pDecCont
    Argument        : const DWLLinearMem_t * data
------------------------------------------------------------------------------*/
u32 h264PpMultiAddPic(decContainer_t * pDecCont, const DWLLinearMem_t * data)
{
    u32 nextFreeId;

    ASSERT(pDecCont->pp.ppInfo.multiBuffer != 0);

    for(nextFreeId = 0; nextFreeId <= pDecCont->pp.multiMaxId; nextFreeId++)
    {
        if(pDecCont->pp.sentPicToPp[nextFreeId] == NULL)
        {
            break;
        }
    }

    ASSERT(nextFreeId <= pDecCont->pp.multiMaxId);

    if(nextFreeId <= pDecCont->pp.multiMaxId)
        pDecCont->pp.sentPicToPp[nextFreeId] = data;

    return nextFreeId;
}

/*------------------------------------------------------------------------------
    Function name   : h264PpMultiRemovePic
    Description     : Reomove a picture form the PP processed list.
                      Return the position which was emptied.
    Return type     : u32
    Argument        : decContainer_t * pDecCont
    Argument        : const DWLLinearMem_t * data
------------------------------------------------------------------------------*/
u32 h264PpMultiRemovePic(decContainer_t * pDecCont, const DWLLinearMem_t * data)
{
    u32 picId;

    ASSERT(pDecCont->pp.ppInfo.multiBuffer != 0);

    for(picId = 0; picId <= pDecCont->pp.multiMaxId; picId++)
    {
        if(pDecCont->pp.sentPicToPp[picId] == data)
        {
            break;
        }
    }

    ASSERT(picId <= pDecCont->pp.multiMaxId);

    if(picId <= pDecCont->pp.multiMaxId)
        pDecCont->pp.sentPicToPp[picId] = NULL;

    return picId;
}

/*------------------------------------------------------------------------------
    Function name   : h264PpMultiFindPic
    Description     : Find a picture in the PP processed list. If found, return
                      the position. If not found, return an value bigger than
                      the max.
    Return type     : u32
    Argument        : decContainer_t * pDecCont
    Argument        : const DWLLinearMem_t * data
------------------------------------------------------------------------------*/
u32 h264PpMultiFindPic(decContainer_t * pDecCont, const DWLLinearMem_t * data)
{
    u32 picId;

    ASSERT(pDecCont->pp.ppInfo.multiBuffer != 0);

    for(picId = 0; picId <= pDecCont->pp.multiMaxId; picId++)
    {
        if(pDecCont->pp.sentPicToPp[picId] == data)
        {
            break;
        }
    }

    return picId;
}

/*------------------------------------------------------------------------------
    Function name   : h264PpMultiInit
    Description     : Initialize the PP processed list.
    Return type     : void
    Argument        : decContainer_t * pDecCont
    Argument        : u32 maxBuffId - max ID in use (buffs = (maxBuffId + 1))
------------------------------------------------------------------------------*/
void h264PpMultiInit(decContainer_t * pDecCont, u32 maxBuffId)
{
    u32 i;
    const u32 buffs =
        sizeof(pDecCont->pp.sentPicToPp) / sizeof(*pDecCont->pp.sentPicToPp);

    ASSERT(maxBuffId < buffs);

    pDecCont->pp.queuedPicToPp[0] = NULL;
    pDecCont->pp.queuedPicToPp[1] = NULL;
    pDecCont->pp.multiMaxId = maxBuffId;
    for(i = 0; i < buffs; i++)
    {
        pDecCont->pp.sentPicToPp[i] = NULL;
    }
}

/*------------------------------------------------------------------------------
    Function name   : h264PpMultiMvc
    Description     :
    Return type     : void
    Argument        : decContainer_t * pDecCont
    Argument        : u32 maxBuffId - max ID in use (buffs = (maxBuffId + 1))
------------------------------------------------------------------------------*/
void h264PpMultiMvc(decContainer_t * pDecCont, u32 maxBuffId)
{

    ASSERT(maxBuffId < (sizeof(pDecCont->pp.sentPicToPp) / sizeof(*pDecCont->pp.sentPicToPp)));

    pDecCont->pp.multiMaxId = maxBuffId;

}

u32 h264UseDisplaySmoothing(const void *decInst)
{

    decContainer_t *pDecCont = (decContainer_t*)decInst;
    if (pDecCont->storage.useSmoothing)
        return HANTRO_TRUE;
    else
        return HANTRO_FALSE;

}
