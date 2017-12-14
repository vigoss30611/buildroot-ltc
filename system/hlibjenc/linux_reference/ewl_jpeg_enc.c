#include <sys/ioctl.h>
#include <stdio.h>
#include <assert.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "ewl_jpeg_enc.h"
#include "ewl.h"
#include <fr/libfr.h>



static u8 enc_power_on = 0;

static int MapAsicRegisters(ewl_t *ewl) {

    unsigned long base, size;
    u32 *pRegs;

    ioctl(ewl->fd_enc, JENC_IOCGHWOFFSET, &base);
    ioctl(ewl->fd_enc, JENC_IOCGHWIOSIZE, &size);

    pRegs = (u32 *)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, ewl->fd_mem, base);

    if(pRegs == MAP_FAILED) {
        printf("MapAsicRegisters: Failed to mmap regs\n");
        return -1;
    }

    ewl->regSize = size;
    ewl->regBase = base;
    ewl->pRegs = pRegs;
    return 0;
}

int EWLOff(const void * inst) {
    ewl_t *enc = (ewl_t*)inst;
    assert(enc != NULL);
    if (enc_power_on == 0)
        return -1;
    enc_power_on = 0;
    ioctl(enc->fd_enc, JENC_IOCG_CORE_OFF, 0);
    return 0;
}

int EWLOn(const void * inst) {
    ewl_t *enc = (ewl_t*)inst;
    assert(enc != NULL);
    if (enc_power_on)
        return -1;
    enc_power_on = 1;
    ioctl(enc->fd_enc, JENC_IOCG_CORE_ON, 0);
    return 0;

}

int EWLInitOn(void) {
    int fd_enc = -1;
    if (enc_power_on)
        return -1;
    enc_power_on = 1;
    fd_enc = open(JENC_MODULE_PATH, O_RDONLY);
    if(fd_enc == -1) {
        printf("EWLInitOn:failed to open: %s\n",JENC_MODULE_PATH);
        return -1;
    }

    ioctl(fd_enc, JENC_IOCG_CORE_ON, 0);
    close(fd_enc);
    return 0;
}

const void *EWLInit() {
    ewl_t *enc = NULL;

    enc = (ewl_t *)malloc(sizeof(ewl_t));
    if(enc == NULL) {
        printf("EWLInit: memory malloc err\n");
        return NULL;
    }

    enc->fd_mem = enc->fd_enc = enc->fd_memalloc = -1;

    enc->fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
    if(enc->fd_mem == -1) {
        printf("EWLInit: failed to open: %s\n", "/dev/mem");
        goto err;
    }

    enc->fd_enc = open(JENC_MODULE_PATH, O_RDWR);
    if(enc->fd_enc == -1) {
        printf("EWLInit: failed to open: %s\n", JENC_MODULE_PATH);
        goto err;
    }

    if(MapAsicRegisters(enc) != 0) {
        printf("EWLInit: MapAsicRegisters fails!\n");
        goto err;
    }

    return enc;

err:
    EWLRelease(enc);
    return NULL;
}

i32 EWLRelease(const void *inst) {
    ewl_t *enc = (ewl_t *)inst;

    if(enc == NULL) {
        return EWL_OK;
    }

    if(enc->pRegs != MAP_FAILED) {
        munmap((void *)enc->pRegs, enc->regSize);
    }

    if(enc->fd_mem != -1) {
        close(enc->fd_mem);
    }

    if(enc->fd_enc) {
        close(enc->fd_enc);
    }

    free(enc);
    return EWL_OK;
}

void EWLWriteReg(const void *inst, u32 offset, u32 val) {
    ewl_t *enc = (ewl_t *)inst;

    if(enc == NULL || offset >= enc->regSize) {
        printf("EWLWriteReg: Error,enc:0x%x ,offset:%d ,regSize:%d\n", enc ,offset ,enc->regSize);
        assert(0);
    }

    *(enc->pRegs + offset) = val;
}

u32 EWLReadReg(const void *inst, u32 offset) {
    u32 val;
    ewl_t *enc = (ewl_t *)inst;

    if(offset >= enc->regSize) {
        printf("EWLReadReg: Error,offset(0x%x),regSize(0x%x)\n", offset, enc->regSize);
        assert(0);
    }

    val = *(enc->pRegs + offset);
    return val;
}

void *EWLmalloc(u32 n) {
    void *p = malloc((size_t)n);
    return p;
}

void EWLfree(void *p) {
    if(p != NULL)
        free(p);
}

void *EWLcalloc(u32 num, u32 len) {
    void *p = calloc((size_t)num, (size_t)len);
    return p;
}

i32 EWLWaitHwRdy(const void *ewl) {
    ewl_t *enc = (ewl_t *)ewl;

    struct timeval tv;
    fd_set fds;
    i32 ret;

    if(enc == NULL) {
        return EWL_HW_WAIT_ERROR;
    }

    FD_ZERO(&fds);
    FD_SET(enc->fd_enc, &fds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    printf("enter EWLWaitHwRdy\n");
    ret = select(enc->fd_enc + 1, &fds, NULL, NULL, &tv);
    if(ret < 0) {
        printf("EWLWaitHwRdy: select failed\n");
        return EWL_HW_WAIT_ERROR;
    }
    if(ret == 0) {
        printf("EWLWaitHwRdy:select timeout tv.tv_sec:%d s\n", tv.tv_sec);
        return EWL_HW_WAIT_ERROR;
    }
    printf("out EWLWaitHwRdy\n");
    return EWL_OK;
}

i32 EWLMallocLinear(const void *instance, u32 size, EWLLinearMem_t *info)
{
    ewl_t *enc_ewl = (ewl_t *)instance;
    assert(enc_ewl != NULL);
    assert(info != NULL);

    info->virtualAddress = fr_alloc_single("ewl",size, &info->busAddress);
    if(!info->virtualAddress) {
        printf("unable to allocat memory anymore\n");
        return EWL_ERROR;
    }

    info->size = size ;

    return EWL_OK;
}

void EWLFreeLinear(const void *instance, EWLLinearMem_t * info)
{
    ewl_t *enc_ewl = (ewl_t *) instance;
    assert(enc_ewl != NULL);
    assert(info != NULL);

    fr_free_single(info->virtualAddress, info->busAddress);
}

