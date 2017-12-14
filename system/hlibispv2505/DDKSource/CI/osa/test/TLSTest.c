#include "TLSTest.h"
#include "osa.h"
#include "img_errors.h"
#include "img_defs.h"

#include <stdlib.h>
#include <time.h>

#define N_THREADS 10

IMG_RESULT OSA_TEST_TLSSetGet()
{
    IMG_INT32     iSomeData = 123;
    IMG_VOID    * pvRetrivedData;

    OSA_ThreadLocalDataSet(&iSomeData);
    OSA_ThreadLocalDataGet(&pvRetrivedData);

    return *(IMG_INT32*)pvRetrivedData == iSomeData ? IMG_SUCCESS : IMG_ERROR_TEST_MISMATCH;
}

static IMG_VOID GetSetRandom(IMG_VOID * pvParam)
{
    IMG_INT32   iSomeData = rand();
    IMG_VOID  * pvRetrivedData;

    OSA_ThreadLocalDataSet(&iSomeData);
    OSA_ThreadLocalDataGet(&pvRetrivedData);

    IMG_ASSERT(*(IMG_INT32*)pvRetrivedData == iSomeData );
}


IMG_RESULT OSA_TEST_TLSMultiSetGet()
{
	IMG_HANDLE hThread[N_THREADS];
	int i, seed;
    seed = (int)time(NULL);
    srand(seed);

	for(i=0; i<N_THREADS; i++)
	{
		if( OSA_ThreadCreateAndStart(GetSetRandom,
								 NULL,
								 "FakeResourceThread",
								 OSA_THREAD_PRIORITY_NORMAL,
								 &hThread[i]) != IMG_SUCCESS)
		{
			fprintf(stderr,"MutexWaitForResource cannot create new thread");
			return IMG_ERROR_FATAL;
		}

	}

	for(i=0; i<N_THREADS; i++)
	{
		OSA_ThreadWaitExitAndDestroy(hThread[i]);
	}

	return IMG_SUCCESS;
}
