/**
******************************************************************************
 @file unit_test.cpp

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
#include "img_types.h"
#include "img_defs.h"
#include "img_errors.h"

#ifndef IMG_BUILD_KO
#include <stdlib.h>
#include <stdio.h>
#else

#include <linux/init.h>
#include <linux/module.h>	/* Needed by all modules */

#define printf printk

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#endif

#ifndef USE_GTEST

#define TEST_NAME(test_case_name, test_name) test_case_name ## _ ## test_name
#define TEST(test_case_name, test_name) void TEST_NAME(test_case_name, test_name)(void)

#define _ASSERT_EQ(expect, actual, message) if ( actual != expect ) { printf("Failed checking EQ: %s\n", message); return; }
// v1 is greater than or equal to v2
#define _ASSERT_GE(v1, v2, message) if ( v1 < v2 ) { printf("Failed checking GE: %s\n", message); return; }
// v1 is lower than or equal to v2
#define _ASSERT_LE(v1, v2, message) if ( v1 > v2 ) { printf("Failed checking LE: %s\n", message); return; }
// pointers are checked with ASSERT_TRUE
#define _ASSERT_TRUE(exp, message) if ( !(exp) ) { printf("Failed checking TRUE: %s\n", message); return; }

#else

#ifdef IMG_BUILD_KO
#error "building KO using Gtest is not possible"
#endif

#include <gtest/gtest.h>

#define _ASSERT_EQ(expect, actual, message) ASSERT_EQ(expect, actual) << message
#define _ASSERT_GE(expect, actual, message) ASSERT_GE(expect, actual) << message
#define _ASSERT_LE(expect, actual, message) ASSERT_LE(expect, actual) << message
#define _ASSERT_TRUE(expect, message) ASSERT_TRUE(expect) << message

#endif

// correct sizes

TEST(TypesSize, integral)
{
#ifndef USE_GTEST
	printf("%s ...\n", __FUNCTION__);
#endif

	_ASSERT_EQ(1, sizeof(IMG_INT8), "IMG_INT8");
	_ASSERT_EQ(1, sizeof(IMG_UINT8), "IMG_UINT8");
	
	_ASSERT_EQ(2, sizeof(IMG_INT16), "IMG_INT16");
	_ASSERT_EQ(2, sizeof(IMG_UINT16), "IMG_UINT16");
	
	_ASSERT_EQ(4, sizeof(IMG_INT32), "IMG_INT32");
	_ASSERT_EQ(4, sizeof(IMG_UINT32), "IMG_UINT32");
	
	_ASSERT_EQ(8, sizeof(IMG_INT64), "IMG_INT64");
	_ASSERT_EQ(8, sizeof(IMG_UINT64), "IMG_UINT64");

#ifndef USE_GTEST
	printf("%s SUCCESS\n", __FUNCTION__);
#endif
}

TEST(TypesSize, memory)
{
#ifndef USE_GTEST
	printf("%s ...\n", __FUNCTION__);
#endif
	_ASSERT_EQ(1, sizeof(IMG_BYTE), "IMG_BYTE");
	
	_ASSERT_LE(sizeof(size_t), sizeof(IMG_SIZE), "IMG_SIZE_T");
	_ASSERT_LE(sizeof(uintptr_t), sizeof(IMG_UINTPTR), "IMG_UINTPTR");
	_ASSERT_LE(sizeof(ptrdiff_t), sizeof(IMG_PTRDIFF), "IMG_PTRDIFF");

#ifndef USE_GTEST
	printf("%s SUCCESS\n", __FUNCTION__);
#endif
}

TEST(Memory, mem_and_alloc)
{
	IMG_CHAR *ptrA = NULL, *ptrB = NULL;
	IMG_UINT32 i;
#ifndef USE_GTEST
	printf("%s ...\n", __FUNCTION__);
#endif

	ptrA = (IMG_CHAR*)IMG_CALLOC(10, sizeof(IMG_CHAR));
	_ASSERT_TRUE(ptrA!=NULL, "calloc");
	ptrB = (IMG_CHAR*)IMG_MALLOC(10*sizeof(IMG_CHAR));
	_ASSERT_TRUE(ptrB!=NULL, "malloc");
	// calloc is setting memory to 0
	IMG_MEMSET(ptrB, 0, 10*sizeof(IMG_CHAR));

	for ( i = 0 ; i < 10 ; i++ )
	{
		_ASSERT_EQ(0, ptrB[i], "malloc+memset");
		_ASSERT_EQ(0, ptrA[i], "calloc");
	}

	IMG_MEMSET(ptrA, 1, 10*sizeof(IMG_CHAR));
	IMG_MEMCPY(ptrB, ptrA, 10*sizeof(IMG_CHAR));

	for ( i = 0 ; i < 10 ; i++ )
	{
		_ASSERT_EQ(1, ptrB[i], "memcpy");
		_ASSERT_EQ(1, ptrA[i], "memset");
	}

	_ASSERT_EQ(0, IMG_MEMCMP(ptrA, ptrB, 10*sizeof(IMG_CHAR)), "memcmp");

	IMG_FREE(ptrA);
	IMG_FREE(ptrB);

#ifndef USE_GTEST
	printf("%s SUCCESS\n", __FUNCTION__);
#endif
}

#ifdef IMG_MALLOC_TEST

IMG_UINT32 gui32AllocFails = 0;

TEST(Memory, test_alloc)
{
	IMG_VOID *ptrA = NULL;
	IMG_INT i;
#ifndef USE_GTEST
	printf("%s ...\n", __FUNCTION__);
#endif

	gui32AllocFails = 0; // shouldn't fail
	ptrA = IMG_MALLOC(sizeof(char));
	_ASSERT_TRUE(ptrA != NULL, "malloc != NULL");
	_ASSERT_EQ(0, gui32AllocFails, "gui32AllocFiails didn't change");

	IMG_FREE(ptrA);
	ptrA=NULL;

	gui32AllocFails = 1; // should fail
	ptrA = IMG_MALLOC(sizeof(char));
	_ASSERT_TRUE(ptrA == NULL, "malloc failed");
	_ASSERT_EQ(0, gui32AllocFails, "gui32AllocFails back to 0");

	gui32AllocFails = 3; // thrid malloc will fail
	for ( i = 1 ; i <= 2 ; i++ )
	{
		if ( i%2 == 0 )
			ptrA = IMG_MALLOC(sizeof(char));
		else
			ptrA = IMG_CALLOC(1, sizeof(char));
		_ASSERT_TRUE(ptrA != NULL, "malloc/calloc didn't fail");
		_ASSERT_EQ (3-i, gui32AllocFails, "gui32AllocFails reduced");

		IMG_FREE(ptrA);
		ptrA = NULL;
	}
	// this allocation should fail
	ptrA = IMG_MALLOC(sizeof(char));
	_ASSERT_TRUE(ptrA == NULL, "malloc fails");
	_ASSERT_EQ(0, gui32AllocFails, "gui32AllocFails back to 0");

#ifndef USE_GTEST
	printf("%s SUCCESS\n", __FUNCTION__);
#endif
}

#endif // IMG_MALLOC_TEST

#ifdef IMG_MALLOC_CHECK

IMG_UINT32 gui32Alloc = 0;
IMG_UINT32 gui32Free = 0;

TEST(Memory, check_alloc)
{
	IMG_UINT32 baseAlloc = gui32Alloc, baseFree = gui32Free;
	IMG_VOID *ptrA = NULL, *ptrB = NULL;
#ifndef USE_GTEST
	printf("%s ...\n", __FUNCTION__);
#endif

	_ASSERT_EQ(baseAlloc, baseFree, "memory leak in the previous tests");

	ptrA = IMG_MALLOC(sizeof(char));
	_ASSERT_EQ(baseAlloc+1, gui32Alloc, "incremented gui32Alloc");

	ptrB = IMG_CALLOC(1, sizeof(char));
	_ASSERT_EQ(baseAlloc+2, gui32Alloc, "incremented gui32Alloc");

	_ASSERT_EQ(baseFree, gui32Free, "free didn't change");
	IMG_FREE(ptrA);
	_ASSERT_EQ(baseFree+1, gui32Free, "incremented free");
	IMG_FREE(ptrB);
	_ASSERT_EQ(baseFree+2, gui32Free, "incremented free");

#ifndef USE_GTEST
	printf("%s SUCCESS\n", __FUNCTION__);
#endif
}

#ifdef IMG_MALLOC_TEST // both!

TEST(Memory, check_test_alloc)
{
	IMG_UINT32 baseAlloc = gui32Alloc, baseFree = gui32Free;
	IMG_VOID *ptrA = NULL;
	IMG_INT i;
#ifndef USE_GTEST
	printf("%s ...\n", __FUNCTION__);
#endif

	_ASSERT_EQ(baseAlloc, baseFree, "memory leak in the previous tests");

	gui32AllocFails = 1;
	ptrA = IMG_MALLOC(sizeof(char));

	_ASSERT_TRUE(ptrA == NULL, "malloc failed");
	_ASSERT_EQ(baseAlloc, gui32Alloc, "allocation failed so gui32Alloc didn't change");
	_ASSERT_EQ(baseFree, gui32Free, "free didn't changed");

	for ( i = 1 ; i <= 2 ; i++ )
	{
		if ( i%2 == 0 )
			ptrA = IMG_MALLOC(sizeof(char));
		else
			ptrA = IMG_CALLOC(1, sizeof(char));
		_ASSERT_TRUE(ptrA != NULL, "malloc/calloc didn't fail");
		
		_ASSERT_EQ(baseAlloc+i, gui32Alloc, "gui32Alloc incremented");

		IMG_FREE(ptrA);
		ptrA = NULL;

		_ASSERT_EQ(baseFree+i, gui32Free, "free incremented");
	}

#ifndef USE_GTEST
	printf("%s SUCCESS\n", __FUNCTION__);
#endif
}

#endif

#endif // IMG_MALLOC_CHECK

/* //disbaled because the macro TO_UITN32  doesn't really behave in the _ASSERT_EQ macro
// maybe the conversion should be done in a function rather than a macro (so that a result code could be checked to know if it worked rather than asserting)
TEST(TypesSize, toUi32)
{
  IMG_UINT32 ui32val = 12345;
  IMG_UINT64 ui64val = 12345;
#ifndef USE_GTEST
  printf("%s ...\n", __FUNCTION__);
#endif

  _ASSERT_EQ(ui32val, IMG_UINT64_TO_UINT32(ui64val), "64b 12345 -> 32b 12345");
#ifndef EXIT_ON_ASSERT
  ui64val = ui64val<<32;
  _ASSERT_EQ(0, IMG_UINT64_TO_UINT32(ui64val), "should not work");
#endif

#ifndef USE_GTEST
  printf("%s SUCCESS\n", __FUNCTION__);
#endif
}*/

#ifndef IMG_BUILD_KO
int main(int argc, char *argv[])
#else
static int __init imginclude_init(void)
#endif
{

#ifdef USE_GTEST
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
#else

	TEST_NAME(TypesSize, integral)();
	TEST_NAME(TypesSize, memory)();
	//TEST_NAME(TypesSize, toUi32)();
	TEST_NAME(Memory, mem_and_alloc)();

#ifdef IMG_MALLOC_TEST
	TEST_NAME(Memory, test_alloc)();
#endif
#ifdef IMG_MALLOC_CHECK
	TEST_NAME(Memory, check_alloc)();
#ifdef IMG_MALLOC_TEST // both!
	TEST_NAME(Memory, check_test_alloc)();
#endif
#endif

	printf("test is a success\n");
	return EXIT_SUCCESS;
#endif

}

#ifdef IMG_BUILD_KO

static void imginlude_exit(void)
{
}

module_init(imginclude_init);
module_exit(imginlude_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Imagination Technologies Ltd");
MODULE_DESCRIPTION("IMG include kernel build test");

#endif
