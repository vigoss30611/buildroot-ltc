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
--  Description : System Wrapper Layer for Linux. Polling version.
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: dwl_x170_linux_no_drv.c,v $
--  $Revision: 1.13 $
--  $Date: 2010/03/24 06:21:33 $
--
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "dwl.h"
#include "dwl_linux.h"

#include "memalloc.h"
#include "dwl_linux_lock.h"

#include "hx170dec.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/timeb.h>

#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>
#include <errno.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_EFENCE
#include "efence.h"
#endif

#define X170_SEM_KEY 0x8070

/* a mutex protecting the wrapper init */
pthread_mutex_t g1_x170_init_mutex = PTHREAD_MUTEX_INITIALIZER;

/* the decoder device driver nod */
const char *g1_dec_dev = DEC_MODULE_PATH;

/* the memalloc device driver nod */
const char *mem_dev = MEMALLOC_MODULE_PATH;

/*------------------------------------------------------------------------------
    Function name   : G1DWLInit
    Description     : Initialize a DWL instance

    Return type     : const void * - pointer to a DWL instance

    Argument        : G1DWLInitParam_t * param - initialization params
------------------------------------------------------------------------------*/
const void *G1DWLInit(G1DWLInitParam_t * param)
{
    hX170dwl_t *dec_dwl;
    unsigned long base;

    assert(param != NULL);

    dec_dwl = (hX170dwl_t *) malloc(sizeof(hX170dwl_t));

    if(dec_dwl == NULL)
    {
        DWL_DEBUG("Init: failed to allocate an instance\n");
        return NULL;
    }

#ifdef INTERNAL_TEST
    dec_dwl->regDump = fopen(REG_DUMP_FILE, "a+");
    if(NULL == dec_dwl->regDump)
    {
        DWL_DEBUG("DWL: failed to open: %s\n", REG_DUMP_FILE);
        goto err;
    }
#endif

    pthread_mutex_lock(&g1_x170_init_mutex);
    dec_dwl->clientType = param->clientType;

    switch (dec_dwl->clientType)
    {
    case DWL_CLIENT_TYPE_H264_DEC:
    case DWL_CLIENT_TYPE_MPEG4_DEC:
    case DWL_CLIENT_TYPE_JPEG_DEC:
    case DWL_CLIENT_TYPE_PP:
    case DWL_CLIENT_TYPE_VC1_DEC:
    case DWL_CLIENT_TYPE_MPEG2_DEC:
    case DWL_CLIENT_TYPE_VP6_DEC:
    case DWL_CLIENT_TYPE_VP8_DEC:
    case DWL_CLIENT_TYPE_RV_DEC:
    case DWL_CLIENT_TYPE_AVS_DEC:
        break;
    default:
        DWL_DEBUG("DWL: Unknown client type no. %d\n", dec_dwl->clientType);
        goto err;
    }

    dec_dwl->fd = -1;
    dec_dwl->fd_mem = -1;
    dec_dwl->fd_memalloc = -1;
    dec_dwl->pRegBase = MAP_FAILED;
    dec_dwl->sigio_needed = 0;

    /* Linear momories not needed in pp */
    if(dec_dwl->clientType != DWL_CLIENT_TYPE_PP)
    {
        /* open memalloc for linear memory allocation */
        dec_dwl->fd_memalloc = open(mem_dev, O_RDWR | O_SYNC);

        if(dec_dwl->fd_memalloc == -1)
        {
            DWL_DEBUG("DWL: failed to open: %s\n", mem_dev);
            goto err;
        }

    }

    /* open mem device for memory mapping */
    dec_dwl->fd_mem = open("/dev/mem", O_RDWR | O_SYNC);

    if(dec_dwl->fd_mem == -1)
    {
        DWL_DEBUG("DWL: failed to open: %s\n", "/dev/mem");
        goto err;
    }

    {
        /* open the device */
        dec_dwl->fd = open(g1_dec_dev, O_RDWR);
        if(dec_dwl->fd == -1)
        {
            DWL_DEBUG("DWL: failed to open '%s'\n", g1_dec_dev);
            goto err;
        }

        if(ioctl(dec_dwl->fd, HX170DEC_IOCGHWOFFSET, &base) == -1)
        {
            DWL_DEBUG("DWL: ioctl failed\n");
            goto err;
        }
        if(ioctl(dec_dwl->fd, HX170DEC_IOCGHWIOSIZE, &dec_dwl->regSize) == -1)
        {
            DWL_DEBUG("DWL: ioctl failed\n");
            goto err;
        }
        close(dec_dwl->fd);
        dec_dwl->fd = -1;
    }

    /* map the hw registers to user space */
    dec_dwl->pRegBase =
        G1DWLMapRegisters(dec_dwl->fd_mem, base, dec_dwl->regSize, 1);

    if(dec_dwl->pRegBase == MAP_FAILED)
    {
        DWL_DEBUG("DWL: Failed to mmap regs\n");
        goto err;
    }

    DWL_DEBUG("DWL: regs size %d bytes, virt %08x\n", dec_dwl->regSize,
              (u32) dec_dwl->pRegBase);

    {
        /* use ASIC ID as the shared sem key */
        key_t key = X170_SEM_KEY;
        int semid;

        if((semid = binary_semaphore_allocation(key, O_RDWR)) != -1)
        {
            DWL_DEBUG("DWL: HW lock sem %x aquired\n", key);
        }
        else if(errno == ENOENT)
        {
            semid = binary_semaphore_allocation(key, IPC_CREAT | O_RDWR);

            binary_semaphore_initialize(semid);

            DWL_DEBUG("DWL: HW lock sem %x created\n", key);
        }
        else
        {
            DWL_DEBUG("DWL: FAILED to get HW lock sem %x\n", key);
            goto err;
        }

        dec_dwl->semid = semid;
    }

    pthread_mutex_unlock(&g1_x170_init_mutex);
    return dec_dwl;

  err:
    pthread_mutex_unlock(&g1_x170_init_mutex);
    G1DWLRelease(dec_dwl);
    return NULL;
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

    pthread_mutex_lock(&g1_x170_init_mutex);
    if(dec_dwl == NULL)
        return DWL_ERROR;

    if(dec_dwl->pRegBase != MAP_FAILED)
        G1G1DWLUnmapRegisters((void*)dec_dwl->pRegBase, dec_dwl->regSize);

    dec_dwl->pRegBase = MAP_FAILED;

    if(dec_dwl->fd != -1)
        close(dec_dwl->fd);

    if(dec_dwl->fd_mem != -1)
        close(dec_dwl->fd_mem);

    /* linear memory allocator */
    if(dec_dwl->fd_memalloc != -1)
        close(dec_dwl->fd_memalloc);

#ifdef INTERNAL_TEST
    fclose(dec_dwl->regDump);
    dec_dwl->regDump = NULL;
#endif

    free(dec_dwl);

    pthread_mutex_unlock(&g1_x170_init_mutex);

    return (DWL_OK);
}

/*------------------------------------------------------------------------------
    Function name   : DWLWaitDecHwReady
    Description     : Wait until hardware has stopped running.
                      Used for synchronizing software runs with the hardware.
                      The wait can succeed or timeout.

    Return type     : i32 - one of the values DWL_HW_WAIT_OK
                                              DWL_HW_WAIT_TIMEOUT

    Argument        : const void * instance - DWL instance
                      u32 timeout - timeout period for the wait specified in
                                milliseconds; 0 will perform a poll of the
                                hardware status and -1 means an infinit wait
------------------------------------------------------------------------------*/

i32 DWLWaitDecHwReady(const void *instance, u32 timeout)
{
    return DWLWaitPpHwReady(instance, timeout);
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLWaitHwReady
    Description     : Wait until hardware has stopped running.
                      Used for synchronizing software runs with the hardware.
                      The wait can succeed or timeout.

    Return type     : i32 - one of the values DWL_HW_WAIT_OK
                                              DWL_HW_WAIT_TIMEOUT

    Argument        : const void * instance - DWL instance
                      u32 timeout - timeout period for the wait specified in
                                milliseconds; 0 will perform a poll of the
                                hardware status and -1 means an infinit wait
------------------------------------------------------------------------------*/

i32 DWLWaitPpHwReady(const void *instance, u32 timeout)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;
    volatile u32 irq_stats;

    u32 irqRegOffset;

    assert(dec_dwl != NULL);

    if(dec_dwl->clientType == DWL_CLIENT_TYPE_PP)
        irqRegOffset = HX170PP_REG_START;   /* pp ctrl reg offset */
    else
        irqRegOffset = HX170DEC_REG_START;  /* decoder ctrl reg offset */

    /* wait for decoder */
#ifdef _READ_DEBUG_REGS
    (void) G1DWLReadReg(dec_dwl, 40 * 4);
    (void) G1DWLReadReg(dec_dwl, 41 * 4);
#endif
    irq_stats = G1DWLReadReg(dec_dwl, irqRegOffset);
    irq_stats = (irq_stats >> 12) & 0xFF;

    if(irq_stats != 0)
    {
        return DWL_HW_WAIT_OK;
    }
    else if(timeout)
    {
        struct timespec polling_time;
        struct timespec remaining_time;
        u32 sleep_time = 0;
        int forever = 0;
        int loop = 1;
        i32 ret = DWL_HW_WAIT_TIMEOUT;
        int sleep_interrupted = 0;
        int sleep_error = 0;
        int sleep_ret = 0;
        int polling_interval = 1;   /* 1ms polling interval */

        if(timeout == (u32) (-1))
        {
            forever = 1;    /* wait forever */
        }

        polling_time.tv_sec = polling_interval / 1000;
        polling_time.tv_nsec = polling_interval - polling_time.tv_sec * 1000;
        polling_time.tv_nsec *= (1000 * 1000);

        do
        {
            time_t actual_sleep_time;
            struct timeb start;
            struct timeb end;

            ftime(&start);
            sleep_ret = nanosleep(&polling_time, &remaining_time);
            ftime(&end);
            actual_sleep_time =
                (end.time * 1000 + end.millitm) - (start.time * 1000 +
                                                   start.millitm);

            DWL_DEBUG("Polling time in milli seconds: %d\n",
                      (u32) polling_time.tv_nsec / (1000 * 1000));
            DWL_DEBUG("Remaining time in milli seconds: %d\n",
                      (u32) remaining_time.tv_nsec / (1000 * 1000));
            DWL_DEBUG("Sleep time in milli seconds: %d\n",
                      (u32) actual_sleep_time);

            if(sleep_ret == -1 && errno == EINTR)
            {
                DWL_DEBUG("Sleep interrupted!\n");
                sleep_interrupted = 1;
            }
            else if(sleep_ret == -1)
            {
                DWL_DEBUG("Sleep error!\n");
                sleep_error = 1;
            }
            else
            {
                DWL_DEBUG("Sleep OK!\n");
                sleep_interrupted = 0;
                sleep_error = 0;
            }
#ifdef _READ_DEBUG_REGS
            (void) G1DWLReadReg(dec_dwl, 40 * 4);
            (void) G1DWLReadReg(dec_dwl, 41 * 4);
            (void) G1DWLReadReg(dec_dwl, 42 * 4);
            (void) G1DWLReadReg(dec_dwl, 43 * 4);
#endif

            irq_stats = G1DWLReadReg(dec_dwl, irqRegOffset);
            irq_stats = (irq_stats >> 12) & 0xFF;

            if(irq_stats != 0)
            {
                ret = DWL_HW_WAIT_OK;
                loop = 0;   /* end the polling loop */
            }
            else if(sleep_error)
            {
                ret = DWL_HW_WAIT_TIMEOUT;
                loop = 0;   /* end the polling loop */
            }
            /* if not a forever wait and sleep was interrupted */
            else if(!forever && sleep_interrupted)
            {

                sleep_time +=
                    (polling_time.tv_sec - remaining_time.tv_sec) * 1000 +
                    (polling_time.tv_nsec -
                     remaining_time.tv_nsec) / (1000 * 1000);

                if(sleep_time >= timeout)
                {
                    ret = DWL_HW_WAIT_TIMEOUT;
                    loop = 0;
                }
                sleep_interrupted = 0;
            }
            /* not a forever wait */
            else if(!forever)
            {
                /* We need to calculate the actual sleep time since
                 * 1) remaining_time does not contain correct value
                 * in case of no signal interrupt
                 * 2) due to system timer resolution the actual sleep
                 * time can be greater than the polling_time
                 */
                /*sleep_time += (polling_time.tv_sec - remaining_time.tv_sec) * 1000
                 * + (polling_time.tv_nsec - remaining_time.tv_nsec) / (1000*1000); */
                /*sleep_time += polling_interval; */
                sleep_time += actual_sleep_time;

                if(sleep_time >= timeout)
                {
                    ret = DWL_HW_WAIT_TIMEOUT;
                    loop = 0;
                }
            }
            DWL_DEBUG("Loop for HW timeout. Total sleep: %dms\n", sleep_time);
        }
        while(loop);

        return ret;
    }
    else
    {
        return DWL_HW_WAIT_TIMEOUT;
    }
}

/*------------------------------------------------------------------------------
    Function name   : G1DWLReserveHw
    Description     :
    Return type     : i32
    Argument        : const void *instance
------------------------------------------------------------------------------*/
i32 G1DWLReserveHw(const void *instance)
{
    i32 ret;
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    do
    {
        /* select which semaphore to use */
        if(dec_dwl->clientType == DWL_CLIENT_TYPE_PP)
        {
            DWL_DEBUG("DWL: PP locked by PID %d\n", getpid());
            ret = binary_semaphore_wait(dec_dwl->semid, 1);
        }
        else
        {
            DWL_DEBUG("DWL: Dec locked by PID %d\n", getpid());
            ret = binary_semaphore_wait(dec_dwl->semid, 0);
        }
    }   /* if error is "error, interrupt", try again */
    while(ret != 0 && errno == EINTR);

    DWL_DEBUG("DWL: success\n");

    if(ret)
        return DWL_ERROR;
    else
        return DWL_OK;
}
