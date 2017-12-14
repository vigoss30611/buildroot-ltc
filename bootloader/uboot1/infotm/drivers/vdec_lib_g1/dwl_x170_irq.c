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

#include <malloc.h>
#include "imapx_dec_irq.h"
#include "basetype.h"
#include "dwl.h"
#include "dwl_bare.h"


#ifdef USE_EFENCE
#include "efence.h"
#endif


/*------------------------------------------------------------------------------
    Function name   : DWLInit
    Description     : Initialize a DWL instance

    Return type     : const void * - pointer to a DWL instance

    Argument        : void * param - not in use, application passes NULL
------------------------------------------------------------------------------*/
const void *DWLInit(DWLInitParam_t * param)
{
    hX170dwl_t *dec_dwl;
    unsigned long base;
    int i, tmp;

    dec_dwl = (hX170dwl_t *) DWLmalloc(sizeof(hX170dwl_t));

    DWL_DEBUG("DWL: INITIALIZE\n");
    if(dec_dwl == NULL)
    {
        DWL_DEBUG("DWL: failed to alloc hX170dwl_t struct\n");
        return NULL;
    }
	DWLmemset((void *)dec_dwl,0,sizeof(hX170dwl_t));

    dec_dwl->clientType = param->clientType;

    dec_dwl->fd_mem = -1;
    dec_dwl->fd_memalloc = -1;
    dec_dwl->pRegBase = (volatile u32 *)IMAPX_VDEC_REG_BASE;
    dec_dwl->regSize = 101*4;

    *(volatile unsigned int *)(0x21e0a160 + 0xc) = 1; // disable output
    *(volatile unsigned int *)(0x21e0a160 + 0x0) = 2; // choose pll, apll-0, dpll-1, epll-2, vpll-3
    *(volatile unsigned int *)(0x21e0a160 + 0x4) = 5; // set div
    *(volatile unsigned int *)(0x21e0a160 + 0xc) = 0; // enable output
    // bus clock
    *(volatile unsigned int *)(0x21e30000 + 0) = 0xffffffff; //  
    *(volatile unsigned int *)(0x21e30000 + 0xc) = 0; //  
    *(volatile unsigned int *)(0x21e30000 + 4) = 0xffffffff; // clock enable
    *(volatile unsigned int *)(0x21e30000 + 8) = 0x11; // module power on
    do{ 
        tmp = *(volatile unsigned int *)(0x21e30000 + 8); 
    }while(!(tmp & 0x2)); // wait ack.

    *(volatile unsigned int *)(0x21e30000 + 8) = 0x1; //  
    *(volatile unsigned int *)(0x21e30000 + 0x18) = 0x1; //  
    *(volatile unsigned int *)(0x21e30000 + 0xc) = 3; //  
    *(volatile unsigned int *)(0x21e30000 + 0x14) = 0; //  
    *(volatile unsigned int *)(0x21e30000 + 0) = 0x0; // 
 
    *(volatile unsigned int *)(0x21e30000 + 0x20) = 0x0; //mmu enable

 //   *(volatile unsigned int *)0x21e0c820 = 0x1; //MEM set, vdec mode
    *(volatile unsigned int *)0x21e0d000 = 0x1;     // trigger efuse read
    *(volatile unsigned int *)(0x21e30000 + 0) = 0x7; // module reset
    *(volatile unsigned int *)(0x21e30000 + 0) = 0x0;
   // printf("init id = %x \n", dec_dwl->pRegBase[0]);
    /* reset vdec asic */
    *(volatile unsigned int *)(0x25000000 + 0x04) = 0;                            
	for(i = 4; i < 104; i += 4)                        
	{                                                                           
		*(volatile unsigned int *)(0x25000000 + i) = 0;                            
	}

    vdec_irq_init();
    DWL_DEBUG("DWL: regs size %d bytes, virt %08x\n", dec_dwl->regSize,
            (u32) dec_dwl->pRegBase);
    return dec_dwl;
}

/*------------------------------------------------------------------------------
    Function name   : DWLRelease
    Description     : Release a DWl instance

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - instance to be released
------------------------------------------------------------------------------*/
i32 DWLRelease(const void *instance)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    assert(dec_dwl != NULL);

    DWLfree(dec_dwl);

	vdec_irq_deinit();

    *(volatile unsigned int *)0x21e0c820 = 0x0; //MEM set, vdec mode
    *(volatile unsigned int *)0x21e0d000 = 0x0;     // trigger efuse read
    *(volatile unsigned int *)(0x21e30000 + 8) = 0x0;   // power down
    *(volatile unsigned int *)(0x21e30000 + 4) = 0x0;   // clock disable
    
    // bus clock
    *(volatile unsigned int *)(0x21e0a160 + 0xc) = 1; // disable output

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
    Function name   : DWLReserveHw
    Description     :
    Return type     : i32
    Argument        : const void *instance
------------------------------------------------------------------------------*/
i32 DWLReserveHw(const void *instance)
{
    return DWL_OK;
}

