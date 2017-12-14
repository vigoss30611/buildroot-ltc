

#ifndef _OSA_PRF_H_
#define _OSA_PRF_H_

#include <osa.h>

#define OSA_PRF_PRINT_DEFAULT   (OSA_PRF_PRINT_TIME|OSA_PRF_PRINT_VALUE)

#define OSA_PRF_PRINT_ALL       (0xFFFF)

#define OSA_PRF_PRINT_TIME      (0x0001)
#define OSA_PRF_PRINT_VALUE     (0x0002)
#define OSA_PRF_PRINT_MIN_MAX   (0x0004)
#define OSA_PRF_PRINT_COUNT     (0x0008)

/*
Profile info  : <name>
======================
Iterations    :
Avg Time (ms) :
Max Time (ms) :
Min Time (ms) :
Avg Value     :
Avg Value/sec :
Max Value     :
Min Value     :
*/

#ifdef OSA_PRF_ENABLE

typedef struct {

  Uint32 totalTime;
  Uint32 maxTime;
  Uint32 minTime;

  Uint32 totalValue;
  Uint32 maxValue;
  Uint32 minValue;

  Uint32 count;
  Uint32 curTime;
  Uint32 curValue;

} OSA_PrfHndl;

int OSA_prfBegin(OSA_PrfHndl *hndl);
int OSA_prfEnd(OSA_PrfHndl *hndl, Uint32 value);
int OSA_prfReset(OSA_PrfHndl *hndl);
int OSA_prfPrint(OSA_PrfHndl *hndl, char *name, Uint32 flags);

#else

typedef struct {

  int rsv;

} OSA_PrfHndl;

#define OSA_prfBegin(hndl)
#define OSA_prfEnd(hndl, value)
#define OSA_prfReset(hndl)
#define OSA_prfPrint(hndl, name, flags)

#endif

#endif /* _OSA_PRF_H_ */



