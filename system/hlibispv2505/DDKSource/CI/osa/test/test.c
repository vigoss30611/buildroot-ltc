#if defined (WIN32)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#include <stdio.h>
#include "osa.h"

#include "img_errors.h"
#include "mutexTest.h"
#include "eventTest.h"
#include "threadTest.h"
#include "TLSTest.h"

#define TEST(function, description)\
	{printf("Test %s started.\n ", description);\
	 if((function) != IMG_SUCCESS)\
		fprintf(stderr, "Test %s failed.\n", description);\
	 else \
	    printf("Test %s success.\n", description);\
	}\

static void RunAllTLSTests();
static void RunAllMutexTests();
static void RunAllEventTests();
static void RunAllThreadTests();

#ifdef OSA_SYSTEMC
int test_main()
#else
int main()
#endif
{
	RunAllThreadTests();
	RunAllMutexTests();
	RunAllEventTests();
    RunAllTLSTests();

#if defined (WIN32)
    _CrtDumpMemoryLeaks();
#endif

	return 0;
}

static void RunAllThreadTests()
{
	TEST(OSA_TEST_ThreadCreateAndDestroy(), "ThreadCreateAndDestroy");
	TEST(OSA_TEST_ThreadPriorities(), "ThreadPriorities");
}

static void RunAllMutexTests()
{
	TEST(OSA_TEST_MutexCreateAndDestroy(), "MutexCreateAndDestroy");
	TEST(OSA_TEST_MutexLockUnlock(), "MutexLockUnlock");
	TEST(OSA_TEST_MutexWaitForResource(), "MutexWaitForResource");
	TEST(OSA_TEST_MutexCreatePlenty(), "MutexCreatePlenty");
    TEST(OSA_TEST_MutexNested(), "MutexNested");
}

static void RunAllEventTests()
{
	TEST(OSA_TEST_EventCreateAndDestroy(), "EventCreateAndDestroy");
	TEST(OSA_TEST_EventSignalNoTimeout(), "EventSignalNoTimeout");
	TEST(OSA_TEST_EventSignalTimeout(), "EventSignalTimeout");
	TEST(OSA_TEST_EventSignalTimeoutElapse(), "EventSignalTimeoutElapse");
	TEST(OSA_TEST_EventSignalFromOtherThread(), "EventSignalFromOtherThread");
	TEST(OSA_TEST_EventSignalThreadTimeout(), "EventSignalThreadTimeout");
	TEST(OSA_TEST_EventSignalThreadTimeoutElaps(), "EventSignalThreadTimeoutElaps");
	TEST(OSA_TEST_EventWaitForResource(), "OSA_TEST_EventWaitForResource");
	TEST(OSA_TEST_EventCreatePlenty(), "EventCreatePlenty");
	TEST(OSA_TEST_EventSimulateScheduler(), "EventSimulateScheduler");
    TEST(OSA_TEST_EventSimulateDispatcher(), "EventSimulateDispatcher");
}

static void RunAllTLSTests()
{
    TEST(OSA_TEST_TLSSetGet(), "OSA_TEST_TLSSetGet");
    TEST(OSA_TEST_TLSMultiSetGet(), "OSA_TEST_TLSMultiSetGet");
}
