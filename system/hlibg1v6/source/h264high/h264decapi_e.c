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
--  Abstract : Application Programming Interface (API) extension
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: h264decapi_e.c,v $
--  $Date: 2009/06/05 10:57:15 $
--  $Revision: 1.2 $
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "h264hwd_container.h"
#include "h264decapi_e.h"

/*------------------------------------------------------------------------------

    Function: H264DecNextChPicture

        Functional description:

        Input:
            decInst     decoder instance.

        Output:
            pOutput     pointer to output structure

        Returns:
            H264DEC_OK            no pictures available for display
            H264DEC_PIC_RDY       picture available for display
            H264DEC_PARAM_ERROR     invalid parameters

------------------------------------------------------------------------------*/
H264DecRet H264DecNextChPicture(H264DecInst decInst,
                              u32 **pOutput, u32 *busAddr)
{
    decContainer_t *pDecCont = (decContainer_t *) decInst;

    if(decInst == NULL || pOutput == NULL || busAddr == NULL)
    {
        return (H264DEC_PARAM_ERROR);
    }

    /* Check for valid decoder instance */
    if(pDecCont->checksum != pDecCont)
    {
        return (H264DEC_NOT_INITIALIZED);
    }

    if (pDecCont->storage.enable2ndChroma && pDecCont->storage.pCh2)
    {
        *pOutput = pDecCont->storage.pCh2;
        *busAddr = pDecCont->storage.bCh2;
        return (H264DEC_PIC_RDY);
    }
    else
    {
        return (H264DEC_OK);
    }

}
