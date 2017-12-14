/**
******************************************************************************
 @file ci_parallel_test.cpp

 @brief

 @copyright Imagination Technologies Ltd. All Rights Reserved. 

 @license Strictly Confidential. 
   No part of this software, either material or conceptual may be copied or 
   distributed, transmitted, transcribed, stored in a retrieval system or  
   translated into any human or computer language in any form by any means, 
   electronic, mechanical, manual or other-wise, or disclosed to third  
   parties without the express written permission of  
   Imagination Technologies Limited,  
   Unit 8, HomePark Industrial Estate, 
   King's Langley, Hertfordshire, 
   WD4 8LZ, U.K. 

******************************************************************************/
#include <gtest/gtest.h>
#include <cstdlib>
#include <ostream>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include "ci_internal/sys_parallel.h"

#ifndef FELIX_FAKE
#error "cannot run tests if not using felix fake device"
#endif

#include <pthread.h>
#include <semaphore.h>


/*extern "C" {
	#include "ci_parallel.c"
}*/

#ifdef WIN32
#include <windows.h>
#endif

#include "unit_tests.h"

/*
// KRN_ functions should not check that data is correct (it should have been checked before!)
TEST(CI_Lock, invalid)
{
	SYS_LOCK *pLock = NULL;

	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, SYS_LockInit(NULL));
	
	gui32AllocFails = 1;
	EXPECT_EQ (IMG_ERROR_MALLOC_FAILED, KRN_CI_LockCreate(&pLock));
	EXPECT_TRUE (pLock == NULL);

	EXPECT_EQ (IMG_SUCCESS, KRN_CI_LockCreate(&pLock));
	EXPECT_EQ (IMG_ERROR_MEMORY_IN_USE, KRN_CI_LockCreate(&pLock));

	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, SYS_LockDestroy(NULL));
	EXPECT_EQ (IMG_SUCCESS, SYS_LockDestroy(pLock));
	
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, SYS_LockAcquire(NULL));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, SYS_LockRelease(NULL));
}*/

TEST(CI_Lock, single)
{
  SYS_LOCK sLock;

  ASSERT_EQ (IMG_SUCCESS, SYS_LockInit(&sLock));
  
  EXPECT_EQ (IMG_SUCCESS, SYS_LockAcquire(&sLock));

  EXPECT_EQ (IMG_SUCCESS, SYS_LockRelease(&sLock));

  EXPECT_EQ (IMG_SUCCESS, SYS_LockDestroy(&sLock));
}

struct recursive_param
{
  int* plockCount;
  SYS_LOCK *mutex;
};

// locks are not recursive 
void * recursive(void * arg)
{
  struct recursive_param *param = (struct recursive_param*)arg;
  EXPECT_EQ (IMG_SUCCESS, SYS_LockAcquire(param->mutex)) << "first lock";
  (*(param->plockCount))++;

  /* Should wait here (deadlocked) until (2)*/
  EXPECT_EQ (IMG_SUCCESS, SYS_LockAcquire(param->mutex)) << "second lock";
  (*(param->plockCount))++;
  EXPECT_EQ (IMG_SUCCESS, SYS_LockRelease(param->mutex)) << "release";

  return 0;
}

TEST(CI_Lock, recursive)
{
  SYS_LOCK sLock;
  int lockCount = 0;
  struct recursive_param param;

  param.plockCount = &lockCount;

  ASSERT_EQ (IMG_SUCCESS, SYS_LockInit(&sLock));
  param.mutex = &sLock;
  param.plockCount = &lockCount;

  pthread_t t;
  
  EXPECT_EQ (0, pthread_create(&t, NULL, &recursive, &param));

#ifdef _WIN32
  Sleep(1000);
#else
  usleep(100000);
#endif

  ASSERT_EQ(1, lockCount);

  /*
   * Should succeed even though we don't own the lock
   * because FAST mutexes don't check ownership.
   */
  EXPECT_EQ (IMG_SUCCESS, SYS_LockRelease(&sLock)); // (2)

#ifdef _WIN32
  Sleep(1000);
#else
  usleep(100000);
#endif

  ASSERT_EQ(2, lockCount);

  pthread_join(t, NULL);

  SYS_LockDestroy(&sLock);
}

struct mutex_param
{
    // the lock that is tested
  SYS_LOCK *pLock;
  sem_t *pSem;

  sem_t thSem;
  pthread_mutex_t var_lock;
  IMG_UINT8 *pTries;
  IMG_UINT8 *pLocked;

  mutex_param()
  {
      pthread_mutex_init(&var_lock, NULL);
      sem_init(&thSem, 0, 0);
  }

  ~mutex_param()
  {
      pthread_mutex_destroy(&var_lock);
      sem_destroy(&thSem);
  }
};

void* mutex(void* vparam)
{
  mutex_param *param = (mutex_param*)vparam;

  pthread_mutex_lock(&(param->var_lock));
  printf("tries++\n");
  (*(param->pTries))++; // 8b should be atomic
  pthread_mutex_unlock(&(param->var_lock));

  // tell this thread tried
  // 1st thread will pass acquire
  // 2nd will lock waiting
  sem_post(&(param->thSem));

  EXPECT_EQ (IMG_SUCCESS, SYS_LockAcquire(param->pLock));

  pthread_mutex_lock(&(param->var_lock));
  printf("locked++\n");
  (*(param->pLocked))++;
  pthread_mutex_unlock(&(param->var_lock));

  printf("wait\n");
  sem_wait(param->pSem);

  EXPECT_EQ (IMG_SUCCESS, SYS_LockRelease(param->pLock));
  return 0;
}

TEST(CI_Lock, mutex)
{
  SYS_LOCK sLock;
  sem_t sSem;

  IMG_UINT8 tries=0, locked=0;
  struct mutex_param param;
  struct timespec timeout;
  const int nth = 2;
  
  ASSERT_EQ (IMG_SUCCESS, SYS_LockInit(&sLock));

  ASSERT_EQ (0, sem_init(&sSem, 0, 0));

  param.pLock = &sLock;
  param.pSem = &sSem;
  param.pTries = &tries;
  param.pLocked = &locked;

  pthread_t t[nth];

  for (int i = 0; i < nth; i++)
    EXPECT_EQ (0, pthread_create(&t[i], NULL, &mutex, &param));

#ifdef _WIN32
  Sleep(1000);
#else
  usleep(100000);
#endif

  timeout.tv_sec = 2;
  timeout.tv_nsec = 0;

  for (int i = 0; i < nth; i++)
    EXPECT_EQ(0, sem_timedwait(&param.thSem, &timeout)) << "waiting on thread " << i;
  
  ASSERT_EQ (1, locked);
  ASSERT_EQ(nth, tries);

  for (int i = 0; i < nth; i++)
    sem_post(&sSem);

  for (int i = 0; i < nth; i++)
    EXPECT_EQ (0, pthread_join(t[i], NULL));

  ASSERT_EQ(nth, locked);
  ASSERT_EQ(nth, tries);

  sem_destroy(&sSem);

  EXPECT_EQ (IMG_SUCCESS, SYS_LockDestroy(&sLock));
}
