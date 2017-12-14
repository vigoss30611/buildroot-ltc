#ifndef MUTEXTEST_H
#define MUTEXTEST_H 

#include <img_types.h>
#include <img_defs.h>

IMG_RESULT OSA_TEST_MutexCreateAndDestroy();
IMG_RESULT OSA_TEST_MutexLockUnlock();
IMG_RESULT OSA_TEST_MutexWaitForResource();
IMG_RESULT OSA_TEST_MutexCreatePlenty();
IMG_RESULT OSA_TEST_MutexNested();

#endif 
