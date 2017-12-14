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
--  Description :  VP6 decoder and PP pipeline support
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: vp6_pp_pipeline.c,v $
--  $Revision: 1.4 $
--  $Date: 2010/12/02 10:53:51 $
--
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "vp6_pp_pipeline.h"
#include "vp6hwd_container.h"
#include "vp6hwd_debug.h"

#ifndef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          do{}while(0)
#else
#undef TRACE_PP_CTRL
#define TRACE_PP_CTRL(...)          printf(__VA_ARGS__)
#endif

/*------------------------------------------------------------------------------
    Function name   : vp6RegisterPP
    Description     :
    Return type     : i32
    Argument        : const void * decInst
    Argument        : const void  *ppInst
    Argument        : (*PPRun)(const void *)
    Argument        : void (*PPEndCallback)(const void *)
------------------------------------------------------------------------------*/
i32 vp6RegisterPP(const void *decInst, const void *ppInst,
                   void (*PPDecStart) (const void *, const DecPpInterface *),
                   void (*PPDecWaitEnd) (const void *),
                   void (*PPConfigQuery) (const void *, DecPpQuery *))
{
    VP6DecContainer_t  *pDecCont;

    pDecCont = (VP6DecContainer_t *) decInst;

    if(decInst == NULL || pDecCont->pp.ppInstance != NULL ||
       ppInst == NULL || PPDecStart == NULL || PPDecWaitEnd == NULL
       || PPConfigQuery == NULL)
    {
        TRACE_PP_CTRL("vp6RegisterPP: Invalid parameter\n");
        return -1;
    }

    if(pDecCont->asicRunning)
    {
        TRACE_PP_CTRL("vp6RegisterPP: Illegal action, asicRunning\n");
        return -2;
    }

    pDecCont->pp.ppInstance = ppInst;
    pDecCont->pp.PPConfigQuery = PPConfigQuery;
    pDecCont->pp.PPDecStart = PPDecStart;
    pDecCont->pp.PPDecWaitEnd = PPDecWaitEnd;

    pDecCont->pp.decPpIf.ppStatus = DECPP_IDLE;

    TRACE_PP_CTRL("vp6RegisterPP: Connected to PP instance 0x%08x\n", (size_t)ppInst);

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : vp6UnregisterPP
    Description     :
    Return type     : i32
    Argument        : const void * decInst
    Argument        : const void  *ppInst
------------------------------------------------------------------------------*/
i32 vp6UnregisterPP(const void *decInst, const void *ppInst)
{
    VP6DecContainer_t *pDecCont;

    pDecCont = (VP6DecContainer_t *) decInst;

    ASSERT(decInst != NULL && ppInst == pDecCont->pp.ppInstance);

    if(ppInst != pDecCont->pp.ppInstance)
    {
        TRACE_PP_CTRL("vp6UnregisterPP: Invalid parameter\n");
        return -1;
    }

    if(pDecCont->asicRunning)
    {
        TRACE_PP_CTRL("vp6UnregisterPP: Illegal action, asicRunning\n");
        return -2;
    }

    pDecCont->pp.ppInstance = NULL;
    pDecCont->pp.PPConfigQuery = NULL;
    pDecCont->pp.PPDecStart = NULL;
    pDecCont->pp.PPDecWaitEnd = NULL;

    TRACE_PP_CTRL("vp6UnregisterPP: Disconnected from PP instance 0x%08x\n",
                  (size_t)ppInst);

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : vp6PreparePpRun
    Description     : 
    Return type     : void 
    Argument        : 
------------------------------------------------------------------------------*/
void vp6PreparePpRun(VP6DecContainer_t * pDecCont)
{
    DecPpInterface *decPpIf = &pDecCont->pp.decPpIf;

    if(pDecCont->pp.ppInstance != NULL) /* we have PP connected */
    {
        pDecCont->pp.ppInfo.tiledMode =
            pDecCont->tiledReferenceEnable;
        pDecCont->pp.PPConfigQuery(pDecCont->pp.ppInstance,
                                &pDecCont->pp.ppInfo);

        TRACE_PP_CTRL
            ("vp6PreparePpRun: output picture => PP could run!\n");

        decPpIf->usePipeline = pDecCont->pp.ppInfo.pipelineAccepted & 1;

        if(decPpIf->usePipeline)
        {
            TRACE_PP_CTRL
                ("vp6PreparePpRun: pipeline=ON => PP will be running\n");
            decPpIf->ppStatus = DECPP_RUNNING;
        }
        /* parallel processing if previous output pic exists */
        else if (pDecCont->outCount)
        {
            TRACE_PP_CTRL
                ("vp6PreparePpRun: pipeline=OFF => PP has to run after DEC\n");
            decPpIf->ppStatus = DECPP_RUNNING;
        }
    }
}
