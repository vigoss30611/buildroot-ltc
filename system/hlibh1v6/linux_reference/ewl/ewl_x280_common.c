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
--  Abstract : Encoder Wrapper Layer for 6280/7280/8270/8290/H1, common parts
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
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
#include <fr/libfr.h>

#ifdef USE_EFENCE
#include "efence.h"
#endif

#ifndef MEMALLOC_MODULE_PATH
#define MEMALLOC_MODULE_PATH        "/tmp/dev/memalloc"
#endif

#ifndef ENC_MODULE_PATH
#define ENC_MODULE_PATH             "/tmp/dev/hx280"
#endif

#ifndef SDRAM_LM_BASE
#define SDRAM_LM_BASE               0x00000000
#endif

/* macro to convert CPU bus address to ASIC bus address */
#ifdef PC_PCI_FPGA_DEMO
#define BUS_CPU_TO_ASIC(address)    (((address) & (~0xff000000)) | SDRAM_LM_BASE)
#else
#define BUS_CPU_TO_ASIC(address)    ((address) | SDRAM_LM_BASE)
#endif

static const char *busTypeName[7] = { "UNKNOWN", "AHB", "OCP", "AXI", "PCI", "AXIAHB", "AXIAPB" };
static const char *synthLangName[3] = { "UNKNOWN", "VHDL", "VERILOG" };
static u32 enc_power_on = 0;

volatile u32 asic_status = 0;

int MapAsicRegisters(hx280ewl_t * ewl)
{
        PTRACE( "MapAsicRegisters ...\n");
    unsigned long base, size;
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

int EWLLinkMode(const void *inst, u32 linkMode)
{
    hx280ewl_t *enc = (hx280ewl_t *)inst;
    assert(enc != NULL);
    ioctl(enc->fd_enc, HX280ENC_IOC_LINKMODE, &linkMode);
    return 0;
}

int EWLOff(const void *inst)
{
	hx280ewl_t *enc = (hx280ewl_t*)inst;
	assert(enc != NULL);
	if (enc_power_on == 0) return -1;
	enc_power_on = 0;
	ioctl(enc->fd_enc, HX280ENC_IOCG_CORE_OFF, 0);
	return 0;
}

int EWLOn(const void *inst)
{
	hx280ewl_t *enc = (hx280ewl_t*)inst;
	assert(enc != NULL);
	if (enc_power_on) return -1;
	enc_power_on = 1;
	ioctl(enc->fd_enc, HX280ENC_IOCG_CORE_ON, 0);
	return 0;
}

int EWLInitOn(void)
{
	int fd_enc = -1;
	if (enc_power_on) return -1;
	enc_power_on = 1;
	fd_enc = open(ENC_MODULE_PATH, O_RDONLY);
	if(fd_enc == -1)
	{
		printf("EWLInitOn:failed to open: %s\n", ENC_MODULE_PATH);
		return -1;
	}

	ioctl(fd_enc, HX280ENC_IOCG_CORE_ON, 0);
	close(fd_enc);
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
    unsigned long base = ~0, size;
    u32 *pRegs = MAP_FAILED;

    EWLHwConfig_t cfg_info;

    memset(&cfg_info, 0, sizeof(cfg_info));

    PTRACE( "EWLReadAsicID open /dev/mem\n");
    fd_mem = open("/dev/mem", O_RDONLY);
    if(fd_mem == -1)
    {
        PTRACE( "EWLReadAsicID: failed to open: %s\n", "/dev/mem");
        goto end;
    }

    PTRACE( "EWLReadAsicID open ENC_MODULE_PATH:%s\n", ENC_MODULE_PATH);
    fd_enc = open(ENC_MODULE_PATH, O_RDONLY);
    if(fd_enc == -1)
    {
        PTRACE( "EWLReadAsicID: failed to open: %s\n", ENC_MODULE_PATH);
        goto end;
    }



    ioctl(fd_enc, HX280ENC_IOCGHWOFFSET, &base);
    ioctl(fd_enc, HX280ENC_IOCGHWIOSIZE, &size);

    if(base == 0)
    {
        //fengwu change base
        base = 0x25100000;
        size = 300 * 4;
    }

    /* map hw registers to user space */
    pRegs = (u32 *) mmap(0, size, PROT_READ, MAP_SHARED, fd_mem, base);

    if(pRegs == MAP_FAILED)
    {
        PTRACE( "EWLReadAsicID: Failed to mmap regs. fd_mem:%d, base:0x%x, size:%d\n", fd_mem, base, size);
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

    PTRACE( "EWLReadAsicID: 0x%08x at 0x%08lx\n", id, base);

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
    unsigned long base, size;
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

    cfgval = pRegs[63];
    PTRACE("h264 EWLReadAsicConfig pRegs[63]: 0x%x\n", cfgval);
    cfg_info.maxEncodedWidth = cfgval & ((1 << 12) - 1);
    cfg_info.h264Enabled = (cfgval >> 27) & 1;
    cfg_info.vp8Enabled = (cfgval >> 26) & 1;
    cfg_info.jpegEnabled = (cfgval >> 25) & 1;
    cfg_info.vsEnabled = (cfgval >> 24) & 1;
    cfg_info.rgbEnabled = (cfgval >> 28) & 1;
    cfg_info.searchAreaSmall = (cfgval >> 29) & 1;
    cfg_info.scalingEnabled = (cfgval >> 30) & 1;

    cfg_info.busType = (cfgval >> 20) & 15;
    cfg_info.synthesisLanguage = (cfgval >> 16) & 15;
    cfg_info.busWidth = (cfgval >> 12) & 15;

    PTRACE("h264 EWLReadAsicConfig:\n"
           "    maxEncodedWidth   = %d\n"
           "    h264Enabled       = %s\n"
           "    jpegEnabled       = %s\n"
           "    vp8Enabled        = %s\n"
           "    vsEnabled         = %s\n"
           "    rgbEnabled        = %s\n"
           "    searchAreaSmall   = %s\n"
           "    scalingEnabled    = %s\n"
           "    busType           = %s\n"
           "    synthesisLanguage = %s\n"
           "    busWidth          = %d\n",
           cfg_info.maxEncodedWidth,
           cfg_info.h264Enabled == 1 ? "YES" : "NO",
           cfg_info.jpegEnabled == 1 ? "YES" : "NO",
           cfg_info.vp8Enabled == 1 ? "YES" : "NO",
           cfg_info.vsEnabled == 1 ? "YES" : "NO",
           cfg_info.rgbEnabled == 1 ? "YES" : "NO",
           cfg_info.searchAreaSmall == 1 ? "YES" : "NO",
           cfg_info.scalingEnabled == 1 ? "YES" : "NO",
           cfg_info.busType < 7 ? busTypeName[cfg_info.busType] : "UNKNOWN",
           cfg_info.synthesisLanguage <
           3 ? synthLangName[cfg_info.synthesisLanguage] : "ERROR",
           cfg_info.busWidth * 32);

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


    PTRACE( "EWLInit: Start, param:%x\n", param);

    /* Check for NULL pointer */
    if(param == NULL || param->clientType > 4)
    {

        PTRACE( "EWLInit: clientType:%d\n", param->clientType);
        return NULL;
    }

    /* Allocate instance */
        PTRACE( "EWLInit: EWLmalloc ...\n");
    if((enc = (hx280ewl_t *) EWLmalloc(sizeof(hx280ewl_t))) == NULL)
    {
        PTRACE( "EWLInit: EWLmalloc err\n");
        return NULL;
    }

    enc->clientType = param->clientType;
    enc->fd_mem = enc->fd_enc = enc->fd_memalloc = -1;
    enc->mmu_used = 0;
    enc->sigio_needed = 0;

    /* New instance allocated */
    enc->fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
    if(enc->fd_mem == -1)
    {
        PTRACE("EWLInit: failed to open: %s\n", "/dev/mem");
        goto err;
    }

        PTRACE( "EWLInit: open %s...\n", ENC_MODULE_PATH);
    enc->fd_enc = open(ENC_MODULE_PATH, O_RDWR);
    if(enc->fd_enc == -1)
    {
        PTRACE( "EWLInit: open %s error \n", ENC_MODULE_PATH);
        PTRACE( "EWLInit: open %s error , fengwu continue\n", ENC_MODULE_PATH);
        //goto err;
    }
	
#if 0
#if 0
    enc->fd_memalloc = open(MEMALLOC_MODULE_PATH, O_RDWR);
    if(enc->fd_memalloc == -1)
    {
        PTRACE("EWLInit: failed to open: %s\n", MEMALLOC_MODULE_PATH);
        goto err;
    }
#else
    /* use infotm linear allocator instead */
    if(alc_open(&enc->alc_inst, "h1 enc") != IM_RET_OK) 
    {
        PTRACE("EWLInit: failed to open alc \n");
        goto err; 
    }
#endif 
#endif
    /* map hw registers to user space */
        PTRACE( "EWLInit: MapAsicRegisters\n");
    if(MapAsicRegisters(enc) != 0)
    {
        PTRACE( "EWLInit: MapAsicRegisters, but fengwu continue\n");
        //goto err;
    }
        PTRACE( "EWLInit: MapAsicRegisters ok\n");

    PTRACE( "EWLInit: mmap regs %d bytes --> %p\n", enc->regSize, enc->pRegBase);

    {
        /* common semaphore with the decoder */
        key_t key = 0x8270;
        int semid;

        PTRACE( "EWLInit: binary_semaphore_allocation\n");
        if((semid = binary_semaphore_allocation(key, 2, O_RDWR)) != -1)
        {
            PTRACE( "EWLInit: HW lock sem aquired (key=%x, id=%d)\n", key,
                   semid);
        }
        else if(errno == ENOENT)
        {
            semid = binary_semaphore_allocation(key, 2, IPC_CREAT | O_RDWR);
            if (semid == -1) {
                PTRACE( "binary_semaphore_allocation error: %s\n", strerror(errno));
            }

            /* for h2 */
            if(binary_semaphore_initialize(semid, 0) != 0)
            {
                PTRACE( "binary_semaphore_initialize error: %s\n",
                       strerror(errno));
            }

            /* init h1 semaphore also */
            if(binary_semaphore_initialize(semid, 1) != 0)
            {
                PTRACE( "binary_semaphore_initialize error: %s\n",
                       strerror(errno));
            }

            PTRACE( "EWLInit: HW lock sem created (key=%x, id=%d)\n", key,
                   semid);
        }
        else
        {
            PTRACE( "binary_semaphore_allocation error: %s\n", strerror(errno));
            goto err;
        }

        enc->semid = semid;
    }


    PTRACE( "EWLInit: Return %0xd\n", (u32) enc);
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
    //if(enc->fd_memalloc != -1) close(enc->fd_memalloc);
	#if 0
	if(enc->alc_inst) {
	    alc_close(enc->alc_inst);
    }
	#else
	#endif
	
    EWLfree(enc);

    PTRACE("EWLRelease: instance freed\n");

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
        asic_status = val;
    }

    offset = offset / 4;
    *(enc->pRegBase + offset) = val;

    PTRACE("EWLWriteReg 0x%02x with value %08x\n", offset * 4, val);
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

    if(enc->sigio_needed)
    {
        int oflags;

        /* asynchronus notification enable */
        fcntl(enc->fd_enc, F_SETOWN, syscall(SYS_gettid));  /* this thread will receive SIGIO */
        oflags = fcntl(enc->fd_enc, F_GETFL);
        fcntl(enc->fd_enc, F_SETFL, oflags | FASYNC);   /* set ASYNC notification flag */
        enc->sigio_needed = 0;  /* do it only once */
    }

    if(offset == 0x04)
    {
        asic_status = val;
    }

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

    asic_status = val;

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

    //assert(offset < enc->regSize);
    if(offset >= enc->regSize)
    {
        PTRACE( "Error! EWLReadReg offset:%d < enc->regSize:%d\n", offset, enc->regSize);
    }

    if(offset == 0x04)
    {
        return asic_status;
    }

    offset = offset / 4;
    val = *(enc->pRegBase + offset);

    //PTRACE( "EWLReadReg 0x%02x --> %08x\n", offset * 4, val);

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
i32 EWLMallocLinear_222(const void *instance, u32 size, EWLLinearMem_t * info)
{
    hx280ewl_t *enc_ewl = (hx280ewl_t *) instance;
    EWLLinearMem_t *buff = (EWLLinearMem_t *) info;

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
    buff->busAddress = BUS_CPU_TO_ASIC(params.busAddress);

    PTRACE("EWLMallocLinear 0x%08x (CPU) 0x%08x (ASIC) --> %p\n",
           params.busAddress, buff->busAddress, buff->virtualAddress);

    return EWL_OK;
}

i32 EWLMallocLinear(const void *instance, u32 size, EWLLinearMem_t *info)
{
    hx280ewl_t *enc_ewl = (hx280ewl_t *)instance;
    u32 pgsize = getpagesize();
   // IM_Buffer buffer;
   // IM_RET ret;
    i32 flags;
   // assert(enc_ewl->alc_inst != IM_NULL);

    assert(enc_ewl != NULL);
    assert(info != NULL);

	#if 0
    info->size = size;
    info->virtualAddress = MAP_FAILED;
    info->busAddress = 0;

    size = (size + (pgsize - 1)) & (~(pgsize - 1));

    /* get memory linear memory buffers */
    flags = ALC_FLAG_ALIGN_8BYTES;
    flags |= enc_ewl->mmu_used ? ALC_FLAG_DEVADDR : ALC_FLAG_PHYADDR;
    ret = alc_alloc(enc_ewl->alc_inst, size, flags, &buffer);
    info->uid = buffer.uid;

    if (ret != IM_RET_OK){
            PTRACE("EWLMallocLinear: ERROR! No linear buffer available\n");
        return EWL_ERROR;
    }

    /* physic linear addr */
    if (enc_ewl->mmu_used)
    {
        info->busAddress = buffer.dev_addr;

#ifdef USE_DMMU
        dmmu_attach_buffer(enc_ewl->dmmu_inst, info->uid);
#endif
    } else {
        info->busAddress = buffer.phy_addr;
    }

    /* virtual address */
    info->virtualAddress = buffer.vir_addr;

    info->size = size;
	#endif
	
	info->virtualAddress = fr_alloc_single("ewl",size, &info->busAddress);	
	if(!info->virtualAddress) {		
		printf("unable to allocat memory anymore\n");		
		return EWL_ERROR;	
	}	
	//printf("alloc linear buf %d->v0x%X b0x%X\n",size ,info->virtualAddress,info->busAddress);	
	info->size = size ;
	
    PTRACE("H1 linear malloc:busAddress:%08x, virtual:%p  \n", info->busAddress, info->virtualAddress);

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

    u32 pgsize = getpagesize();
    u32 size = 0;
    assert(enc_ewl != NULL);
    assert(info != NULL);
	
	#if 0
    //IM_Buffer buffer;

    PTRACE("EWLFreeLinear++\t%p\n", info->virtualAddress);
    size = (info->size + (pgsize - 1)) & (~(pgsize - 1));

    buffer.phy_addr = enc_ewl->mmu_used ? 0 : info->busAddress;
    buffer.vir_addr = info->virtualAddress ;
    buffer.size = size;
    buffer.flag = ALC_FLAG_ALIGN_8BYTES;
#ifdef USE_DMMU
    if(enc_ewl->mmu_used)
        dmmu_detach_buffer(enc_ewl->dmmu_inst, info->uid);
#endif
    buffer.uid = info->uid;
    buffer.flag |= enc_ewl->mmu_used ? ALC_FLAG_DEVADDR : ALC_FLAG_PHYADDR;

    if(info->virtualAddress != 0)
        alc_free(enc_ewl->alc_inst, &buffer);
	#endif
	
	fr_free_single(info->virtualAddress, info->busAddress);

	PTRACE("EWLFreeLinear--\t%p\n", info->virtualAddress);
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

    val = EWLReadReg(inst, 0x38);
    EWLWriteReg(inst, 0x38, val & (~0x01)); /* reset ASIC */

    PTRACE("EWLReleaseHw: PID %d trying to release ...\n", getpid());

    if(binary_semaphore_post(enc->semid, 1) != 0)
    {
        PTRACE("binary_semaphore_post error: %s\n", strerror(errno));
        assert(0);
    }

#if 0 
    /* we have to release postprocessor also */
    if(binary_semaphore_post(enc->semid, 1) != 0)
    {
        PTRACE("binary_semaphore_post error: %s\n", strerror(errno));
        assert(0);
    }
#endif 

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
