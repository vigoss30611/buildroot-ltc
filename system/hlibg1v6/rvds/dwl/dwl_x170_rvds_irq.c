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
--  Description : System Wrapper Layer for Linux using IRQs
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: dwl_x170_linux_irq.c,v $
--  $Revision: 1.15 $
--  $Date: 2010/03/24 06:21:33 $
--
------------------------------------------------------------------------------*/

#include <imapx_system.h>
#include "cmnlib.h"
#include "imapx_dec_irq.h"
#include "basetype.h"
#include "dwl.h"
#include "dwl_rvds.h"

#include <errno.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_EFENCE
#include "efence.h"
#endif


/*------------------------------------------------------------------------------
    Function name   : G1DWLInit
    Description     : Initialize a DWL instance

    Return type     : const void * - pointer to a DWL instance

    Argument        : void * param - not in use, application passes NULL
------------------------------------------------------------------------------*/
const void *G1DWLInit(G1DWLInitParam_t * param)
{
    hX170dwl_t *dec_dwl;
    unsigned long base;

    dec_dwl = (hX170dwl_t *) G1DWLmalloc(sizeof(hX170dwl_t));

    DWL_DEBUG("DWL: INITIALIZE\n");
    if(dec_dwl == NULL)
    {
        DWL_DEBUG("DWL: failed to alloc hX170dwl_t struct\n");
        return NULL;
    }
	G1DWLmemset((void *)dec_dwl,0,sizeof(hX170dwl_t));

    dec_dwl->clientType = param->clientType;

    dec_dwl->fd_mem = -1;
    dec_dwl->fd_memalloc = -1;
    dec_dwl->pRegBase = (volatile u32 *)IMAPX_VDEC_REG_BASE;
    dec_dwl->regSize = 101*4;

	vdec_irq_init();

    DWL_DEBUG("DWL: regs size %d bytes, virt %08x\n", dec_dwl->regSize,
              (u32) dec_dwl->pRegBase);
	return dec_dwl;
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLRelease
    Description     : Release a DWl instance

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - instance to be released
------------------------------------------------------------------------------*/
i32 G1DWLRelease(const void *instance)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    assert(dec_dwl != NULL);

    G1DWLfree(dec_dwl);

	vdec_irq_deinit();

    return (DWL_OK);
}

/*------------------------------------------------------------------------------
    Function name   : DWLWaitDecHwReady
    Description     : Wait until hardware has stopped running
                        and Decoder interrupt comes.
                      Used for synchronizing software runs with the hardware.
                      The wait could succed, timeout, or fail with an error.

    Return type     : i32 - one of the values DWL_HW_WAIT_OK
                                              DWL_HW_WAIT_TIMEOUT
                                              DWL_HW_WAIT_ERROR

    Argument        : const void * instance - DWL instance
------------------------------------------------------------------------------*/
i32 DWLWaitDecHwReady(const void *instance, u32 timeout)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;
    DWL_DEBUG("DWLWaitDecHwReady: Start\n");

    /* Check invalid parameters */
    if(dec_dwl == NULL)
    {
        assert(0);
        return DWL_HW_WAIT_ERROR;
    }
	vdec_irq_wait();

    DWL_DEBUG("DWLWaitDecHwReady: OK\n");
    return DWL_HW_WAIT_OK;

}

/*------------------------------------------------------------------------------
    Function name   : DWLWaitPpHwReady
    Description     : Wait until hardware has stopped running
                      and pp interrupt comes.
                      Used for synchronizing software runs with the hardware.
                      The wait could succed, timeout, or fail with an error.

    Return type     : i32 - one of the values DWL_HW_WAIT_OK
                                              DWL_HW_WAIT_TIMEOUT
                                              DWL_HW_WAIT_ERROR

    Argument        : const void * instance - DWL instance
------------------------------------------------------------------------------*/
i32 DWLWaitPpHwReady(const void *instance, u32 timeout)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    DWL_DEBUG("DWLWaitPpHwReady: Start\n");

    /* Check invalid parameters */
    if(dec_dwl == NULL)
    {
        assert(0);
        return DWL_HW_WAIT_ERROR;
    }

	pp_irq_wait();
    DWL_DEBUG("DWLWaitPpHwReady: OK\n");
    return DWL_HW_WAIT_OK;

}

/* HW locking */

/*------------------------------------------------------------------------------
    Function name   : G1DWLReserveHw
    Description     :
    Return type     : i32
    Argument        : const void *instance
------------------------------------------------------------------------------*/
i32 G1DWLReserveHw(const void *instance)
{
    return DWL_OK;
}

