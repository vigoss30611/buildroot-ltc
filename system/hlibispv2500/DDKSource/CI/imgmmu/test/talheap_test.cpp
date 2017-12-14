/**
******************************************************************************
 @file talheap_test.cpp

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

#include <target.h>
#include "tal_config.h" // used only if USE_TAL_CFG is not defined

#ifdef __TAL_USE_MEOS__
#include <krn.h>
#define KRN_NO_PRIORITY_LEVELS 5
static KRN_SCHEDULE_T g_sKrnScheduler;
#endif

//#include <addr_alloc.h>
extern "C" {
#include "tal_heap.c"
}

struct TALHeap: public ::testing::Test
{
	IMG_HANDLE sTALMemTop;
	IMG_HANDLE sTALMemBottom;
#ifndef USE_TAL_CFG
	TAL_sDeviceInfo sDeviceInfo;
#endif

	void SetUp()
	{
#ifdef __TAL_USE_MEOS__
	        // setup MeOS kernel used in TAL
	  KRN_reset(&g_sKrnScheduler, IMG_NULL, KRN_NO_PRIORITY_LEVELS-1, 0, IMG_NULL, 0);
	  KRN_startOS("MMU test"); // needed?
#endif
#ifdef USE_TAL_CFG
		TARGET_SetConfigFile("MMU_TAL_config.txt");
#else
	  setDeviceInfo_NULL(&sDeviceInfo);
	  TARGET_SetMemSpaceInfo(gasMemspace, TAL_MEM_SPACE_ARRAY_SIZE, &sDeviceInfo);
#endif
		ASSERT_EQ(IMG_SUCCESS, TALSETUP_Initialise());

		sTALMemTop = TAL_GetMemSpaceHandle("TOP");
		sTALMemBottom = TAL_GetMemSpaceHandle("BOTTOM");
	}

	void TearDown()
	{
		TALSETUP_Deinitialise();
#ifdef __TAL_USE_MEOS__
#ifndef _WIN32
		// because deadlocks on windows for some reasons
		KRN_stopOS();
#endif
#endif
	}
};

TEST_F(TALHeap, create_destroy)
{
	IMGMMU_Heap *pHeap = NULL;
	IMG_RESULT res;

	pHeap = IMGMMU_HeapCreate(0, 32, 1024, &res);
	ASSERT_TRUE(pHeap != NULL);
	EXPECT_EQ(IMG_SUCCESS, res);
	EXPECT_EQ(0, pHeap->uiVirtAddrStart);
	EXPECT_EQ(32, pHeap->uiAllocAtom);
	EXPECT_EQ(1024, pHeap->uiSize);

	EXPECT_EQ(IMG_SUCCESS, IMGMMU_HeapDestroy(pHeap));

	pHeap = IMGMMU_HeapCreate(0, 33, 1024, &res);
	ASSERT_TRUE(pHeap == NULL);
	EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, res);

	pHeap = IMGMMU_HeapCreate(5, 32, 1024, &res);
	ASSERT_TRUE(pHeap == NULL);
	EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, res);

#ifdef IMG_MALLOC_TEST
	gui32AllocFails = 1; // next alloc fails
	pHeap = IMGMMU_HeapCreate(0, 32, 1024, &res);
	ASSERT_TRUE(pHeap == NULL);
	EXPECT_EQ(IMG_ERROR_MALLOC_FAILED, res);
#endif
}

#include <list>

TEST_F(TALHeap, allocate_top_bottom)
{
	IMGMMU_Heap *pTopHeap = NULL, *pBottomHeap = NULL;
	std::list<IMGMMU_HeapAlloc*> allocList;
	IMGMMU_HeapAlloc *alloc = NULL;
	IMG_UINT64 uiStart = 0;
	IMG_RESULT res;
	unsigned int i;

	// 3 first directories
	pTopHeap = IMGMMU_HeapCreate(0, IMGMMU_PAGE_SIZE, 3*IMGMMU_N_PAGE*IMGMMU_PAGE_SIZE, &res);
	ASSERT_TRUE(pTopHeap != NULL);
	EXPECT_EQ(IMG_SUCCESS, res);

	// 3 last directories
	uiStart = ((IMGMMU_N_TABLE-3u)*IMGMMU_N_PAGE*MMU_PAGE_SIZE);
	pBottomHeap = IMGMMU_HeapCreate((IMG_UINTPTR)uiStart, IMGMMU_PAGE_SIZE, 3*IMGMMU_N_PAGE*IMGMMU_PAGE_SIZE, &res);
	ASSERT_TRUE(pTopHeap != NULL);
	EXPECT_EQ(IMG_SUCCESS, res);
	
	i=1;
	uiStart = pTopHeap->uiVirtAddrStart;
	while ( (alloc = IMGMMU_HeapAllocate(pTopHeap, 3*pTopHeap->uiAllocAtom, &res)) != NULL )
	{
		allocList.push_back(alloc);
		EXPECT_EQ(IMG_SUCCESS, res) << "alloc " << i;
		ASSERT_EQ(3*pTopHeap->uiAllocAtom, alloc->uiAllocSize) << "alloc " << i;
		ASSERT_LE(uiStart, alloc->uiVirtualAddress) << "alloc " << i;
		ASSERT_GT(uiStart + pTopHeap->uiSize, alloc->uiVirtualAddress) << "alloc " << i;
		ASSERT_GE(uiStart + pTopHeap->uiSize, alloc->uiVirtualAddress + alloc->uiAllocSize) << "alloc " << i;
		i++;
	}
	EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, res) << "top heap should be full";
	EXPECT_EQ(pTopHeap->uiSize/(3*pTopHeap->uiAllocAtom), i);

	i=1;
	uiStart = pBottomHeap->uiVirtAddrStart;
	while ( (alloc = IMGMMU_HeapAllocate(pBottomHeap, 6*pBottomHeap->uiAllocAtom, &res)) != NULL )
	{
		allocList.push_back(alloc);
		EXPECT_EQ(IMG_SUCCESS, res) << "alloc " << i;
		ASSERT_EQ(6*pBottomHeap->uiAllocAtom, alloc->uiAllocSize) << "alloc " << i;
		ASSERT_LE(uiStart, alloc->uiVirtualAddress) << "alloc " << i;
		ASSERT_GT(uiStart + pBottomHeap->uiSize, alloc->uiVirtualAddress) << "alloc " << i;
		ASSERT_GE(uiStart + pBottomHeap->uiSize, alloc->uiVirtualAddress + alloc->uiAllocSize) << "alloc " << i;
		i++;
	}
	EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, res) << "bottom heap should be full";
	EXPECT_EQ(pBottomHeap->uiSize/(6*pBottomHeap->uiAllocAtom), i);

	EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, IMGMMU_HeapDestroy(pTopHeap)) << "should not be destroyable as allocations are still here";

	while( allocList.size() > 0 )
	{
		alloc = allocList.back();
		allocList.pop_back();

		EXPECT_EQ(IMG_SUCCESS, IMGMMU_HeapFree(alloc));
	}

	EXPECT_EQ(IMG_SUCCESS, IMGMMU_HeapDestroy(pTopHeap));
	EXPECT_EQ(IMG_SUCCESS, IMGMMU_HeapDestroy(pBottomHeap));
}
