/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------
--
--  Abstract : Encoder Wrapper Layer, common parts
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "base_type.h"
#include "ewl.h"
#include "ewl_linux_lock.h"
#include "ewl_x280_common.h"

#include "hx280enc.h"
#include "memalloc.h"

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#ifdef USE_EFENCE
#include "efence.h"
#endif

#include <fr/libfr.h>

#ifndef MEMALLOC_MODULE_PATH
#define MEMALLOC_MODULE_PATH        "/tmp/dev/memalloc"
#endif
#ifndef ENC_MODULE_PATH
#define ENC_MODULE_PATH             "/dev/vench2"
#endif

#ifndef SDRAM_LM_BASE
#define SDRAM_LM_BASE               0x00000000
#endif

/* macro to convert CPU bus address to ASIC bus address */
#ifdef PC_PCI_FPGA_DEMO
//#define BUS_CPU_TO_ASIC(address)    (((address) & (~0xff000000)) | SDRAM_LM_BASE)
#define BUS_CPU_TO_ASIC(address,offset)    ((address) - (offset))
#else
#define BUS_CPU_TO_ASIC(address,offset)    ((address) | SDRAM_LM_BASE)
#endif

#ifdef TRACE_EWL
static const char *busTypeName[7] = { "UNKNOWN", "AHB", "OCP", "AXI", "PCI", "AXIAHB", "AXIAPB" };
static const char *synthLangName[3] = { "UNKNOWN", "VHDL", "VERILOG" };
#endif

volatile u32 asic_status = 0;

FILE *fEwl = NULL;

int MapAsicRegisters(hx280ewl_t * ewl)
{
    unsigned long base;
    unsigned int size;
    u32 *pRegs;

    ioctl(ewl->fd_enc, HX280ENC_IOCGHWOFFSET, &base);
    ioctl(ewl->fd_enc, HX280ENC_IOCGHWIOSIZE, &size);

    /* map hw registers to user space */
    pRegs =
        (u32 *) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                     ewl->fd_mem, base);
    if(pRegs == MAP_FAILED)
    {
        PTRACE("EWLInit: Failed to mmap regs\n");
        return -1;
    }

    ewl->regSize = size;
    ewl->regBase = base;
    ewl->pRegBase = pRegs;

    return 0;
}

/*******************************************************************************
 Function name   : EWLReadAsicID
 Description     : Read ASIC ID register, static implementation
 Return type     : u32 ID
 Argument        : void
*******************************************************************************/
u32 EWLReadAsicID()
{
    u32 id = ~0;
    int fd_mem = -1, fd_enc = -1;
    unsigned long base = ~0;
    unsigned int size;
    u32 *pRegs = MAP_FAILED;

    EWLHwConfig_t cfg_info;

    memset(&cfg_info, 0, sizeof(cfg_info));

    if(fEwl == NULL)
            fEwl = fopen("ewl.trc", "w");

    fd_mem = open("/dev/mem", O_RDONLY);
    if(fd_mem == -1)
    {
        PTRACE("EWLReadAsicID: failed to open: %s\n", "/dev/mem");
        goto end;
    }

    fd_enc = open(ENC_MODULE_PATH, O_RDONLY);
    if(fd_enc == -1)
    {
        PTRACE("EWLReadAsicID: failed to open: %s\n", ENC_MODULE_PATH);
        goto end;
    }

    /* ask module for base */
    if(ioctl(fd_enc, HX280ENC_IOCGHWOFFSET, &base) == -1)
    {
        PTRACE("ioctl failed\n");
        goto end;
    }
    if(ioctl(fd_enc, HX280ENC_IOCGHWIOSIZE, &size) == -1)
    {
        PTRACE("ioctl failed\n");
        goto end;
    }
    /* map hw registers to user space */
    pRegs = (u32 *) mmap(0, size, PROT_READ, MAP_SHARED, fd_mem, base);

    if(pRegs == MAP_FAILED)
    {
        PTRACE("EWLReadAsicID: Failed to mmap regs\n");
        goto end;
    }

    id = pRegs[0];

  end:
    if(pRegs != MAP_FAILED)
        munmap(pRegs, size);
    if(fd_mem != -1)
        close(fd_mem);
    if(fd_enc != -1)
        close(fd_enc);

    PTRACE("EWLReadAsicID: 0x%08x at 0x%08lx\n", id, base);

    return id;
}

/*******************************************************************************
 Function name   : EWLReadAsicConfig
 Description     : Reads ASIC capability register, static implementation
 Return type     : EWLHwConfig_t 
 Argument        : void
*******************************************************************************/
EWLHwConfig_t EWLReadAsicConfig(void)
{
    int fd_mem = -1, fd_enc = -1;
    unsigned long base;
    unsigned int size;
    u32 *pRegs = MAP_FAILED, cfgval;

    EWLHwConfig_t cfg_info;

    memset(&cfg_info, 0, sizeof(cfg_info));

    fd_mem = open("/dev/mem", O_RDONLY);
    if(fd_mem == -1)
    {
        PTRACE("EWLReadAsicConfig: failed to open: %s\n", "/dev/mem");
        goto end;
    }

    fd_enc = open(ENC_MODULE_PATH, O_RDONLY);
    if(fd_enc == -1)
    {
        PTRACE("EWLReadAsicConfig: failed to open: %s\n", ENC_MODULE_PATH);
        goto end;
    }

    ioctl(fd_enc, HX280ENC_IOCGHWOFFSET, &base);
    ioctl(fd_enc, HX280ENC_IOCGHWIOSIZE, &size);

    /* map hw registers to user space */
    pRegs = (u32 *) mmap(0, size, PROT_READ, MAP_SHARED, fd_mem, base);

    if(pRegs == MAP_FAILED)
    {
        PTRACE("EWLReadAsicConfig: Failed to mmap regs\n");
        goto end;
    }

    cfgval = pRegs[80];

    cfg_info.scalingEnabled = (cfgval >> 30) & 1;
    cfg_info.bFrameEnabled = (cfgval >> 29) & 1;
    cfg_info.rgbEnabled = (cfgval >> 28) & 1;
    cfg_info.hevcEnabled = (cfgval >> 27) & 1;
    cfg_info.vp9Enabled = (cfgval >> 26) & 1;
    cfg_info.deNoiseEnabled = (cfgval >> 25) & 1;
    
    cfg_info.main10Enabled = (cfgval >> 24) & 1;

    cfg_info.busType = (cfgval >> 21) & 7;
    cfg_info.synthesisLanguage = (cfgval >> 17) & 3;
    cfg_info.busWidth = (cfgval >> 13) & 3;
    cfg_info.maxEncodedWidth = cfgval & ((1 << 13) - 1);
#if 0
    PTRACE("EWLReadAsicConfig:\n"
           "    maxEncodedWidth   = %d\n"
           "    hevcEnabled       = %s\n"
           "    vp9Enabled        = %s\n"
           "    rgbEnabled        = %s\n"
           "    scalingEnabled    = %s\n"
           "    busType           = %s\n"
           "    synthesisLanguage = %s\n"
           "    busWidth          = %d\n"
           "    bFrameEnabled     = %s\n"
           "    deNoiseEnabled       = %s\n"
           "    main10Enabled       = %s\n"
           cfg_info.maxEncodedWidth,
           cfg_info.hevcEnabled == 1 ? "YES" : "NO",
           cfg_info.vp9Enabled == 1 ? "YES" : "NO",
           cfg_info.rgbEnabled == 1 ? "YES" : "NO",
           cfg_info.scalingEnabled == 1 ? "YES" : "NO",
           cfg_info.busType < 7 ? busTypeName[cfg_info.busType] : "UNKNOWN",
           cfg_info.synthesisLanguage < 3 ? synthLangName[cfg_info.synthesisLanguage] : "ERROR",
           (cfg_info.busWidth+1) * 32,
           cfg_info.bFrameEnabled == 1 ? "YES" : "NO",
           cfg_info.deNoiseEnabled == 1 ? "YES" : "NO",
           cfg_info.main10Enabled == 1 ? "YES" : "NO");
#endif

  end:
    if(pRegs != MAP_FAILED)
        munmap(pRegs, size);
    if(fd_mem != -1)
        close(fd_mem);
    if(fd_enc != -1)
        close(fd_enc);
    return cfg_info;
}

/*******************************************************************************
 Function name   : EWLInit
 Description     : Allocate resources and setup the wrapper module
 Return type     : ewl_ret 
 Argument        : void
*******************************************************************************/
const void *EWLInit(EWLInitParam_t * param)
{
    hx280ewl_t *enc = NULL;


    PTRACE("EWLInit: Start\n");

    /* Check for NULL pointer */
    if(param == NULL || param->clientType > 4)
    {

        PTRACE(("EWLInit: Bad calling parameters!\n"));
        return NULL;
    }

    /* Allocate instance */
    if((enc = (hx280ewl_t *) EWLmalloc(sizeof(hx280ewl_t))) == NULL)
    {
        PTRACE("EWLInit: failed to alloc hx280ewl_t struct\n");
        return NULL;
    }

    enc->clientType = param->clientType;
    enc->fd_mem = enc->fd_enc = enc->fd_memalloc = -1;

    /* New instance allocated */
    enc->fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
    if(enc->fd_mem == -1)
    {
        PTRACE("EWLInit: failed to open: %s\n", "/dev/mem");
        goto err;
    }

    enc->fd_enc = open(ENC_MODULE_PATH, O_RDWR);
    if(enc->fd_enc == -1)
    {
        PTRACE("EWLInit: failed to open: %s\n", ENC_MODULE_PATH);
        goto err;
    }
	#if 0
	enc->fd_memalloc = 0;

    enc->fd_memalloc = open(MEMALLOC_MODULE_PATH, O_RDWR);
    if(enc->fd_memalloc == -1)
    {
        PTRACE("EWLInit: failed to open: %s\n", MEMALLOC_MODULE_PATH);
        goto err;
    }
	#endif

    /* map hw registers to user space */
    if(MapAsicRegisters(enc) != 0)
    {
        goto err;
    }

    PTRACE("EWLInit: mmap regs %d bytes --> %p\n", enc->regSize, enc->pRegBase);

    {
        /* common semaphore with the decoder */
        key_t key = 0x8070;
        int semid;

        if((semid = binary_semaphore_allocation(key, 2, O_RDWR)) != -1)
        {
            PTRACE("EWLInit: HW lock sem aquired (key=%x, id=%d)\n", key,
                   semid);
        }
        else if(errno == ENOENT)
        {
            semid = binary_semaphore_allocation(key, 2, IPC_CREAT | O_RDWR);

            if(binary_semaphore_initialize(semid, 0) != 0)
            {
                PTRACE("binary_semaphore_initialize error: %s\n",
                       strerror(errno));
            }

            /* init PP semaphore also */
            if(binary_semaphore_initialize(semid, 1) != 0)
            {
                PTRACE("binary_semaphore_initialize error: %s\n",
                       strerror(errno));
            }

            PTRACE("EWLInit: HW lock sem created (key=%x, id=%d)\n", key,
                   semid);
        }
        else
        {
            PTRACE("binary_semaphore_allocation error: %s\n", strerror(errno));
            goto err;
        }

        enc->semid = semid;
    }

    PTRACE("EWLInit: Return %0xd\n", (u32) enc);
    return enc;

  err:
    EWLRelease(enc);
    PTRACE("EWLInit: Return NULL\n");
    return NULL;
}

/*******************************************************************************
 Function name   : EWLRelease
 Description     : Release the wrapper module by freeing all the resources
 Return type     : ewl_ret 
 Argument        : void
*******************************************************************************/
i32 EWLRelease(const void *inst)
{
    hx280ewl_t *enc = (hx280ewl_t *) inst;

    assert(enc != NULL);

    if(enc == NULL)
        return EWL_OK;

    /* Release the instance */
    if(enc->pRegBase != MAP_FAILED)
        munmap((void *) enc->pRegBase, enc->regSize);

    if(enc->fd_mem != -1)
        close(enc->fd_mem);
    if(enc->fd_enc != -1)
        close(enc->fd_enc);
	#if 0
    if(enc->fd_memalloc != -1)
        close(enc->fd_memalloc);
	#endif

    EWLfree(enc);

    PTRACE("EWLRelease: instance freed\n");
   
    if(fEwl != NULL) {
        fclose(fEwl);
        fEwl = NULL;
    }

    return EWL_OK;
}

/*******************************************************************************
 Function name   : EWLWriteReg
 Description     : Set the content of a hadware register
 Return type     : void 
 Argument        : u32 offset
 Argument        : u32 val
*******************************************************************************/
void EWLWriteReg(const void *inst, u32 offset, u32 val)
{
    hx280ewl_t *enc = (hx280ewl_t *) inst;

    assert(enc != NULL && offset < enc->regSize);

    if(offset == 0x04)
    {
        //asic_status = val;
    }

    offset = offset / 4;
    *(enc->pRegBase + offset) = val;

   PTRACE("EWLWriteReg 0x%03x with value %08x\n", offset * 4, val);
}

/*------------------------------------------------------------------------------
    Function name   : EWLEnableHW
    Description     : 
    Return type     : void 
    Argument        : const void *inst
    Argument        : u32 offset
    Argument        : u32 val
------------------------------------------------------------------------------*/
void EWLEnableHW(const void *inst, u32 offset, u32 val)
{
    hx280ewl_t *enc = (hx280ewl_t *) inst;

    assert(enc != NULL && offset < enc->regSize);    

    offset = offset / 4;
    *(enc->pRegBase + offset) = val;

    PTRACE("EWLEnableHW 0x%02x with value %08x\n", offset * 4, val);
}

/*------------------------------------------------------------------------------
    Function name   : EWLDisableHW
    Description     : 
    Return type     : void 
    Argument        : const void *inst
    Argument        : u32 offset
    Argument        : u32 val
------------------------------------------------------------------------------*/
void EWLDisableHW(const void *inst, u32 offset, u32 val)
{
    hx280ewl_t *enc = (hx280ewl_t *) inst;

    assert(enc != NULL && offset < enc->regSize);

    offset = offset / 4;
    *(enc->pRegBase + offset) = val;

    PTRACE("EWLDisableHW 0x%02x with value %08x\n", offset * 4, val);
}

/*******************************************************************************
 Function name   : EWLReadReg
 Description     : Retrive the content of a hadware register
                    Note: The status register will be read after every MB
                    so it may be needed to buffer it's content if reading
                    the HW register is slow.
 Return type     : u32 
 Argument        : u32 offset
*******************************************************************************/
u32 EWLReadReg(const void *inst, u32 offset)
{
    u32 val;
    hx280ewl_t *enc = (hx280ewl_t *) inst;

    assert(offset < enc->regSize);
    
    offset = offset / 4;
    val = *(enc->pRegBase + offset);

    PTRACE("EWLReadReg 0x%03x --> %08x\n", offset * 4, val);

    return val;
}

/*------------------------------------------------------------------------------
    Function name   : EWLMallocRefFrm
    Description     : Allocate a frame buffer (contiguous linear RAM memory)
    
    Return type     : i32 - 0 for success or a negative error code 
    
    Argument        : const void * instance - EWL instance
    Argument        : u32 size - size in bytes of the requested memory
    Argument        : EWLLinearMem_t *info - place where the allocated memory
                        buffer parameters are returned
------------------------------------------------------------------------------*/
i32 EWLMallocRefFrm(const void *instance, u32 size, EWLLinearMem_t * info)
{
    hx280ewl_t *enc_ewl = (hx280ewl_t *) instance;
    EWLLinearMem_t *buff = (EWLLinearMem_t *) info;
    i32 ret;

    assert(enc_ewl != NULL);
    assert(buff != NULL);

    PTRACE("EWLMallocRefFrm\t%8d bytes\n", size);

    ret = EWLMallocLinear(enc_ewl, size, buff);

    PTRACE("EWLMallocRefFrm %08x --> %p\n", buff->busAddress,
           buff->virtualAddress);

    return ret;
}

/*------------------------------------------------------------------------------
    Function name   : EWLFreeRefFrm
    Description     : Release a frame buffer previously allocated with 
                        EWLMallocRefFrm.
    
    Return type     : void 
    
    Argument        : const void * instance - EWL instance
    Argument        : EWLLinearMem_t *info - frame buffer memory information
------------------------------------------------------------------------------*/
void EWLFreeRefFrm(const void *instance, EWLLinearMem_t * info)
{
    hx280ewl_t *enc_ewl = (hx280ewl_t *) instance;
    EWLLinearMem_t *buff = (EWLLinearMem_t *) info;

    assert(enc_ewl != NULL);
    assert(buff != NULL);

    EWLFreeLinear(enc_ewl, buff);

    PTRACE("EWLFreeRefFrm\t%p\n", buff->virtualAddress);
}

/*------------------------------------------------------------------------------
    Function name   : EWLMallocLinear
    Description     : Allocate a contiguous, linear RAM  memory buffer
    
    Return type     : i32 - 0 for success or a negative error code  
    
    Argument        : const void * instance - EWL instance
    Argument        : u32 size - size in bytes of the requested memory
    Argument        : EWLLinearMem_t *info - place where the allocated memory
                        buffer parameters are returned
------------------------------------------------------------------------------*/
i32 EWLMallocLinear(const void *instance, u32 size, EWLLinearMem_t * info)
{
    hx280ewl_t *enc_ewl = (hx280ewl_t *) instance;
    EWLLinearMem_t *buff = (EWLLinearMem_t *) info;

#if 0
    MemallocParams params;
    u32 pgsize = getpagesize();

    assert(enc_ewl != NULL);
    assert(buff != NULL);

    PTRACE("EWLMallocLinear\t%8d bytes\n", size);

    buff->size = (size + (pgsize - 1)) & (~(pgsize - 1));

    params.size = size;

    buff->virtualAddress = 0;
    /* get memory linear memory buffers */
    ioctl(enc_ewl->fd_memalloc, MEMALLOC_IOCXGETBUFFER, &params);
    if(params.busAddress == 0)
    {
        PTRACE("EWLMallocLinear: Linear buffer not allocated\n");
        return EWL_ERROR;
    }

    /* Map the bus address to virtual address */
    buff->virtualAddress = MAP_FAILED;
    buff->virtualAddress = (u32 *) mmap(0, buff->size, PROT_READ | PROT_WRITE,
                                        MAP_SHARED, enc_ewl->fd_mem,
                                        params.busAddress);

    if(buff->virtualAddress == MAP_FAILED)
    {
        PTRACE("EWLInit: Failed to mmap busAddress: 0x%08x\n",
               params.busAddress);
        return EWL_ERROR;
    }

    /* ASIC might be in different address space */
    buff->busAddress = BUS_CPU_TO_ASIC(params.busAddress, params.translationOffset);
#else
    //use fr buffer
    char buf[32];
    sprintf(buf, "ewl-%p", instance);
	buff->virtualAddress = fr_alloc_single(buf, size,  &buff->busAddress);
	if(!buff->virtualAddress) {
		printf("hevc enc unable to allocat memory anymore ->%d\n", size);
		return EWL_ERROR;
	}
	buff->size = size;
#endif
//	printf("alloc linear buf %d->v0x%X b0x%X\n",size ,info->virtualAddress,info->busAddress);
    PTRACE("EWLMallocLinear 0x%08x (CPU) 0x%08x (ASIC) --> %d\n",
           buff->virtualAddress, buff->busAddress, buff->size);

    return EWL_OK;
}

/*------------------------------------------------------------------------------
    Function name   : EWLFreeLinear
    Description     : Release a linera memory buffer, previously allocated with 
                        EWLMallocLinear.
    
    Return type     : void 
    
    Argument        : const void * instance - EWL instance
    Argument        : EWLLinearMem_t *info - linear buffer memory information
------------------------------------------------------------------------------*/
void EWLFreeLinear(const void *instance, EWLLinearMem_t * info)
{
    hx280ewl_t *enc_ewl = (hx280ewl_t *) instance;
    EWLLinearMem_t *buff = (EWLLinearMem_t *) info;

    assert(enc_ewl != NULL);
    assert(buff != NULL);
	PTRACE("EWLFreeLinear\t%p\n", buff->virtualAddress);
#if 0 
    if(buff->busAddress != 0)
        ioctl(enc_ewl->fd_memalloc, MEMALLOC_IOCSFREEBUFFER, &buff->busAddress);

    if(buff->virtualAddress != MAP_FAILED)
        munmap(buff->virtualAddress, buff->size);

#else
	 if(buff->virtualAddress != NULL  && buff->busAddress != 0){
		 fr_free_single(buff->virtualAddress, buff->busAddress);
		 buff->virtualAddress  = NULL;
		  buff->busAddress = 0;	 

	 	}
#endif

    
}

/*******************************************************************************
 Function name   : EWLReleaseHw
 Description     : Release HW resource when frame is ready
*******************************************************************************/
void EWLReleaseHw(const void *inst)
{
    u32 val;
    hx280ewl_t *enc = (hx280ewl_t *) inst;

    assert(enc != NULL);

    val = EWLReadReg(inst, 0x14);
    EWLWriteReg(inst, 0x14, val & (~0x01)); /* reset ASIC */

    PTRACE("EWLReleaseHw: PID %d trying to release ...\n", getpid());

    if(binary_semaphore_post(enc->semid, 0) != 0)
    {
        PTRACE("binary_semaphore_post error: %s\n", strerror(errno));
        assert(0);
    }

    /* we have to release postprocessor also */
    if(binary_semaphore_post(enc->semid, 1) != 0)
    {
        PTRACE("binary_semaphore_post error: %s\n", strerror(errno));
        assert(0);
    }

    PTRACE("EWLReleaseHw: HW released by PID %d\n", getpid());
}

/* SW/SW shared memory */
/*------------------------------------------------------------------------------
    Function name   : EWLmalloc
    Description     : Allocate a memory block. Same functionality as
                      the ANSI C malloc()
    
    Return type     : void pointer to the allocated space, or NULL if there
                      is insufficient memory available
    
    Argument        : u32 n - Bytes to allocate
------------------------------------------------------------------------------*/
void *EWLmalloc(u32 n)
{

    void *p = malloc((size_t) n);

    PTRACE("EWLmalloc\t%8d bytes --> %p\n", n, p);

    return p;
}

/*------------------------------------------------------------------------------
    Function name   : EWLfree
    Description     : Deallocates or frees a memory block. Same functionality as
                      the ANSI C free()
    
    Return type     : void 
    
    Argument        : void *p - Previously allocated memory block to be freed
------------------------------------------------------------------------------*/
void EWLfree(void *p)
{
    PTRACE("EWLfree\t%p\n", p);
    if(p != NULL)
        free(p);
}

/*------------------------------------------------------------------------------
    Function name   : EWLcalloc
    Description     : Allocates an array in memory with elements initialized
                      to 0. Same functionality as the ANSI C calloc()
    
    Return type     : void pointer to the allocated space, or NULL if there
                      is insufficient memory available
    
    Argument        : u32 n - Number of elements
    Argument        : u32 s - Length in bytes of each element.
------------------------------------------------------------------------------*/
void *EWLcalloc(u32 n, u32 s)
{
    void *p = calloc((size_t) n, (size_t) s);

    PTRACE("EWLcalloc\t%8d bytes --> %p\n", n * s, p);

    return p;
}

/*------------------------------------------------------------------------------
    Function name   : EWLmemcpy
    Description     : Copies characters between buffers. Same functionality as
                      the ANSI C memcpy()
    
    Return type     : The value of destination d
    
    Argument        : void *d - Destination buffer
    Argument        : const void *s - Buffer to copy from
    Argument        : u32 n - Number of bytes to copy
------------------------------------------------------------------------------*/
void *EWLmemcpy(void *d, const void *s, u32 n)
{
    return memcpy(d, s, (size_t) n);
}

/*------------------------------------------------------------------------------
    Function name   : EWLmemset
    Description     : Sets buffers to a specified character. Same functionality
                      as the ANSI C memset()
    
    Return type     : The value of destination d
    
    Argument        : void *d - Pointer to destination
    Argument        : i32 c - Character to set
    Argument        : u32 n - Number of characters
------------------------------------------------------------------------------*/
void *EWLmemset(void *d, i32 c, u32 n)
{
    return memset(d, (int) c, (size_t) n);
}

/*------------------------------------------------------------------------------
    Function name   : EWLmemcmp
    Description     : Compares two buffers. Same functionality
                      as the ANSI C memcmp()

    Return type     : Zero if the first n bytes of s1 match s2

    Argument        : const void *s1 - Buffer to compare
    Argument        : const void *s2 - Buffer to compare
    Argument        : u32 n - Number of characters
------------------------------------------------------------------------------*/
int EWLmemcmp(const void *s1, const void *s2, u32 n)
{
    return memcmp(s1, s2, (size_t) n);
}
