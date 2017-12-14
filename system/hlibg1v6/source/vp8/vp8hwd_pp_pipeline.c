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
--  Description :  VP8 decoder and PP pipeline support
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vp8hwd_pp_pipeline.c,v $
--  $Revision: 1.3 $
--  $Date: 2010/12/02 10:53:51 $
--
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vp8hwd_pp_pipeline.h"
#include "vp8hwd_container.h"
#include "vp8hwd_debug.h"

#ifndef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          do{}while(0)
#else
#undef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          printf(__VA_ARGS__)
#endif

/*------------------------------------------------------------------------------
    Function name   : vp8RegisterPP
    Description     :
    Return type     : i32
    Argument        : const void * decInst
    Argument        : const void  *ppInst
    Argument        : (*PPRun)(const void *)
    Argument        : void (*PPEndCallback)(const void *)
------------------------------------------------------------------------------*/
i32 vp8RegisterPP(const void *decInst, const void *ppInst,
                   void (*PPDecStart) (const void *, const DecPpInterface *),
                   void (*PPDecWaitEnd) (const void *),
                   void (*PPConfigQuery) (const void *, DecPpQuery *))
{
    VP8DecContainer_t  *pDecCont;

    pDecCont = (VP8DecContainer_t *) decInst;

    if(decInst == NULL || pDecCont->pp.ppInstance != NULL ||
       ppInst == NULL || PPDecStart == NULL || PPDecWaitEnd == NULL
       || PPConfigQuery == NULL)
    {
        TRACE_PP_CTRL("vp8hwdRegisterPP: Invalid parameter\n");
        return -1;
    }

    if(pDecCont->asicRunning)
    {
        TRACE_PP_CTRL("vp8hwdRegisterPP: Illegal action, asicRunning\n");
        return -2;
    }

    pDecCont->pp.ppInstance = ppInst;
    pDecCont->pp.PPConfigQuery = PPConfigQuery;
    pDecCont->pp.PPDecStart = PPDecStart;
    pDecCont->pp.PPDecWaitEnd = PPDecWaitEnd;

    pDecCont->pp.decPpIf.ppStatus = DECPP_IDLE;

    TRACE_PP_CTRL("vp8hwdRegisterPP: Connected to PP instance 0x%08x\n", (size_t)ppInst);

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : vpdUnregisterPP
    Description     :
    Return type     : i32
    Argument        : const void * decInst
    Argument        : const void  *ppInst
------------------------------------------------------------------------------*/
i32 vp8UnregisterPP(const void *decInst, const void *ppInst)
{
    VP8DecContainer_t *pDecCont;

    pDecCont = (VP8DecContainer_t *) decInst;

    ASSERT(decInst != NULL && ppInst == pDecCont->pp.ppInstance);

    if(ppInst != pDecCont->pp.ppInstance)
    {
        TRACE_PP_CTRL("vp8hwdUnregisterPP: Invalid parameter\n");
        return -1;
    }

    if(pDecCont->asicRunning)
    {
        TRACE_PP_CTRL("vp8hwdUnregisterPP: Illegal action, asicRunning\n");
        return -2;
    }

    pDecCont->pp.ppInstance = NULL;
    pDecCont->pp.PPConfigQuery = NULL;
    pDecCont->pp.PPDecStart = NULL;
    pDecCont->pp.PPDecWaitEnd = NULL;

    TRACE_PP_CTRL("vp8hwdUnregisterPP: Disconnected from PP instance 0x%08x\n",
                  (size_t)ppInst);

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : vp8hwdPreparePpRun
    Description     : 
    Return type     : void 
    Argument        : 
------------------------------------------------------------------------------*/
void vp8hwdPreparePpRun(VP8DecContainer_t * pDecCont)
{
    DecPpInterface *decPpIf = &pDecCont->pp.decPpIf;

    if(pDecCont->pp.ppInstance != NULL) /* we have PP connected */
    {
        pDecCont->pp.ppInfo.tiledMode =
            pDecCont->tiledReferenceEnable;
        pDecCont->pp.PPConfigQuery(pDecCont->pp.ppInstance,
                                &pDecCont->pp.ppInfo);

        TRACE_PP_CTRL
            ("vp8hwdPreparePpRun: output picture => PP could run!\n");

        decPpIf->usePipeline = pDecCont->pp.ppInfo.pipelineAccepted & 1;

        /* pipeline accepted, but previous pic needs to be processed
         * (pp config probably changed from one that cannot be run in
         * pipeline to one that can) */
        pDecCont->pendingPicToPp = 0;
        if (decPpIf->usePipeline && pDecCont->outCount)
        {
            decPpIf->usePipeline = 0;
            pDecCont->pendingPicToPp = 1;
        }

        if(decPpIf->usePipeline)
        {
            TRACE_PP_CTRL
                ("vp8hwdPreparePpRun: pipeline=ON => PP will be running\n");
            decPpIf->ppStatus = DECPP_RUNNING;
        }
        /* parallel processing if previous output pic exists */
        else if (pDecCont->outCount)
        {
            TRACE_PP_CTRL
                ("vp8hwdPreparePpRun: pipeline=OFF => PP has to run after DEC\n");
            decPpIf->ppStatus = DECPP_RUNNING;
        }
    }
}
