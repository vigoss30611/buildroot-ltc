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
--  Description : Encoder Wrapper Layer (user space module)
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "ewl.h"
#include "hx280enc.h"   /* This EWL uses the kernel module */
#include "ewl_x280_common.h"
#include "ewl_linux_lock.h"

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <semaphore.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

ENC_MODULE_PATH     defines the path for encoder device driver nod.
                        e.g. "/tmp/dev/hx280"
MEMALLOC_MODULE_PATH defines the path for memalloc device driver nod.
                        e.g. "/tmp/dev/memalloc"
ENC_IO_BASE         defines the IO base address for encoder HW registers
                        e.g. 0xC0000000
SDRAM_LM_BASE       defines the base address for SDRAM as seen by HW
                        e.g. 0x80000000

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

extern volatile u32 asic_status;

#ifndef ENC_DEMO
static volatile sig_atomic_t sig_delivered = 0;

/* SIGIO handler */
static void sig_handler(int signal_number)
{
    sig_delivered++;
}
#endif

void HandleSIGIO(hx280ewl_t * enc)
{

#ifndef ENC_DEMO
    struct sigaction sa;

    /* asynchronus notification handler */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;  /* restart of system calls */
    sigaction(SIGIO, &sa, NULL);
#endif

    /* EWLInit might be called in a separate thread */
    /* we want to register the encoding thread for SIGIO */
    enc->sigio_needed = 1;  /* register for SIGIO in EWLEnableHW */
}

/*******************************************************************************
 Function name   : EWLReserveHw
 Description     : Reserve HW resource for currently running codec
*******************************************************************************/
i32 EWLReserveHw(const void *inst)
{
    hx280ewl_t *enc = (hx280ewl_t *) inst;

    assert(enc != NULL);

    PTRACE("EWLReserveHw: PID %d trying to reserve ...\n", getpid());

    /* Check invalid parameters */
    if(inst == NULL)
        return EWL_ERROR;

    if(binary_semaphore_wait(enc->semid, 1) != 0)
    {
        PTRACE("binary_semaphore_wait error: %s\n", strerror(errno));
        return EWL_ERROR;
    }

    PTRACE("Codec semaphore locked\n");

#if 0
    /* we have to lock postprocessor also */
    if(binary_semaphore_wait(enc->semid, 1) != 0)
    {
        PTRACE("binary_semaphore_wait error: %s\n", strerror(errno));
        if(binary_semaphore_post(enc->semid, 0) != 0)
        {
            PTRACE("binary_semaphore_post error: %s\n", strerror(errno));
            assert(0);
        }
        return EWL_ERROR;
    }
#endif 
    PTRACE("Post-processor semaphore locked\n");

    EWLWriteReg(inst, 0x38, 0);

    PTRACE("EWLReserveHw: ENC HW locked by PID %d\n", getpid());

    return EWL_OK;
}


i32 EWL_select(const void *inst)
{
	hx280ewl_t *enc = (hx280ewl_t *)inst;

	int ret;
	fd_set fds;
	struct timeval tv;

	struct timeval old;
	struct timeval cur;
	unsigned int timeuse = 0;
    
    PTRACE("%s()\n", __FUNCTION__);

	memset(&old, 0x00, sizeof(struct timeval));
	memset(&cur, 0x00, sizeof(struct timeval));

	/* Check invalid parameters */
	if(enc == NULL)
	{
		return EWL_HW_WAIT_ERROR;
	}

	FD_ZERO(&fds);
	FD_SET(enc->fd_enc, &fds);

	tv.tv_sec	= 100;
	tv.tv_usec	= 0;
	//tv.tv_sec	= 0;
	//tv.tv_usec	= 60 * 1000;

	gettimeofday(&old, NULL);
	ret = select(enc->fd_enc + 1, &fds, NULL, NULL, &tv);
	gettimeofday(&cur, NULL);
	if(ret < 0)
	{
        PTRACE("select() failed");
		return EWL_HW_WAIT_ERROR;
	}
	if(ret == 0)
	{
        PTRACE("select() timeout");
		return EWL_HW_WAIT_ERROR;
	}

	timeuse = (cur.tv_sec - old.tv_sec) * 1000 + (cur.tv_usec - old.tv_usec) / 1000;
    PTRACE("[EWL] timeuse %dms\n", timeuse);

	return EWL_OK;
}


/*******************************************************************************
 Function name   : EWLWaitHwRdy
 Description     : Wait for the encoder semaphore
 Return type     : i32 
 Argument        : void
*******************************************************************************/
i32 EWLWaitHwRdy(const void *inst, u32 *slicesReady)
{
    hx280ewl_t *enc = (hx280ewl_t *) inst;
    u32 prevSlicesReady = 0;

    PTRACE("EWLWaitHw: Start\n");

    /* Check invalid parameters */
    if(enc == NULL)
    {
        assert(0);
        return EWL_HW_WAIT_ERROR;
    }

#ifdef EWL_NO_HW_TIMEOUT
    {
        sigset_t set, oldset;

        sigemptyset(&set);
        sigaddset(&set, SIGIO);

#ifndef ENC_DEMO
        if (slicesReady)
        {
            /* The function should return when a slice is ready */
            prevSlicesReady = *slicesReady;

            /* Get the number of completed slices from ASIC registers. */
            *slicesReady = (enc->pRegBase[21] >> 16) & 0xFF;

            /* The reference implementation uses polling for testing purpose.
             * More efficient implementation should signal the slice ready IRQ. */
            while (!sig_delivered && *slicesReady <= prevSlicesReady)
            {
                /* Get the number of completed slices from ASIC regs */
                *slicesReady = (enc->pRegBase[21] >> 16) & 0xFF;
            }
            sig_delivered = 0;
        }
        else
        {
            //fengwu 20150601
            EWL_select(inst);
        }

#else
        {
            int signo;

            sigwait(&set, &signo);
        }
#endif

    }

#else
#error Timeout not implemented!
#endif

    asic_status = enc->pRegBase[1]; /* update the buffered asic status */

    if (slicesReady)
        PTRACE("EWLWaitHw: slicesReady = %d\n", *slicesReady);
    PTRACE("EWLWaitHw: asic_status = %x\n", asic_status);
    PTRACE("EWLWaitHw: OK!\n");

    return EWL_OK;
}
