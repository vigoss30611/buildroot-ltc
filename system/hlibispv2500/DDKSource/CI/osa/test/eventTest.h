#ifndef EVENTTEST_H
#define EVENTTEST_H 

#include <img_types.h>
#include <img_defs.h>

IMG_RESULT OSA_TEST_EventCreateAndDestroy();
IMG_RESULT OSA_TEST_EventSignalNoTimeout();
IMG_RESULT OSA_TEST_EventSignalTimeout();
IMG_RESULT OSA_TEST_EventSignalTimeoutElapse();
IMG_RESULT OSA_TEST_EventSignalFromOtherThread();
IMG_RESULT OSA_TEST_EventSignalThreadTimeout();
IMG_RESULT OSA_TEST_EventSignalThreadTimeoutElaps();
IMG_RESULT OSA_TEST_EventWaitForResource();
IMG_RESULT OSA_TEST_EventCreatePlenty();
IMG_RESULT OSA_TEST_EventSimulateScheduler();
IMG_RESULT OSA_TEST_EventSimulateDispatcher();

#endif 
