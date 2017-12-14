#ifndef _EWL_JPEG_ENC_H_
#define _EWL_JPEG_ENC_H_

#include "basetype.h"
#include "ewl.h"

#ifndef JENC_MEMALLOC_MODULE_PATH
#define JENC_MEMALLOC_MODULE_PATH "/dev/mem_jenc_alloc"
#endif

#ifndef JENC_MODULE_PATH
#define JENC_MODULE_PATH "/dev/jenc"
#endif

#ifdef __GNUC__
#define EXPORT __attribute__((visibility("default")))   //just for gcc4+
#else
#define EXPORT  __declspec(dllexport)
#endif


#define JENC_IOCGHWOFFSET  0
#define JENC_IOCGHWIOSIZE  1
#define JENC_IOCG_CORE_OFF 2
#define JENC_IOCG_CORE_ON  3


typedef struct
{
    int fd_mem;
    int fd_enc;
    int fd_memalloc;

    u32 regSize;
    u32 regBase;
    volatile u32 *pRegs;
}ewl_t;

i32 EWLWaitHwRdy(const void *ewl);


EXPORT i32 EWLMallocLinear(const void *inst, u32 size, EWLLinearMem_t * info);
EXPORT void EWLFreeLinear(const void *inst, EWLLinearMem_t * info);



#endif
