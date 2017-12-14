#ifndef _EWL_H_
#define _EWL_H_

#ifdef __cpluscplus
extern "C"
{
#endif

#include "basetype.h"

#define EWL_OK     0
#define EWL_ERROR -1


#define EWL_HW_WAIT_OK       EWL_OK
#define EWL_HW_WAIT_ERROR    EWL_ERROR
#define EWL_HW_WAIT_TIMEOUT  1

typedef struct EWLLinearMem
{
  u32 *virtualAddress;
  u32 busAddress;
  u32 size;
} EWLLinearMem_t;


int EWLOff(const void * inst);
int EWLOn(const void * inst);
int EWLInitOn(void);
const void *EWLInit();
i32 EWLRelease(const void *inst);
void EWLWriteReg(const void *inst, u32 offset, u32 val);
u32 EWLReadReg(const void *inst, u32 offset);
void *EWLmalloc(u32 n);
void EWLfree(void *p);
void *EWLcalloc(u32 num, u32 len);


#ifdef __cpluscplus
}
#endif
#endif
