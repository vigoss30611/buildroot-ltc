/**
******************************************************************************
 @file talaloc_test.cpp

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
#include <climits>

#ifdef __TAL_USE_MEOS__
#include <krn.h>
#define KRN_NO_PRIORITY_LEVELS 5
static KRN_SCHEDULE_T g_sKrnScheduler;
#endif

#include <target.h>
#include "tal_config.h"

#include "tal_memory.cpp"

#define N_PAGE_SIZE 2
struct TALAlloc: public ::testing::Test
{
	IMG_HANDLE sTALDeviceMemory;
#ifndef USE_TAL_CFG
	TAL_sDeviceInfo sDeviceInfo;
#endif
    IMG_UINT32 pageSizes[N_PAGE_SIZE];

    TALAlloc()
    {
        pageSizes[0] = 4<<10;
        pageSizes[1] = 16<<10;
    }

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

        IMGMMU_SetCPUPageSize(pageSizes[0]); // init to 4kB pages - may change in tests
		sTALDeviceMemory = TAL_GetMemSpaceHandle("TOP");
		//sTALMemBottom = TAL_GetMemSpaceHandle("BOTTOM");
	}

	void TearDown()
	{
        ASSERT_EQ(0, g_allocCellMap.size()) << "still has allocations";
        g_allocCellMap.clear();

		TALSETUP_Deinitialise();
#ifdef __TAL_USE_MEOS__
#ifndef _WIN32
		// because deadlocks on windows for some reasons
		KRN_stopOS();
#endif
#endif
	}
};

TEST_F(TALAlloc, usage)
{
    IMG_RESULT ret;
	IMGMMU_TALAlloc *pAllocA = NULL, *pAllocB = NULL, *pAllocC = NULL;

    for ( int i = 0 ; i < N_PAGE_SIZE ; i++ )
    {
        ASSERT_EQ(IMG_SUCCESS, IMGMMU_SetCPUPageSize(pageSizes[i])) 
            << "failed to setup page size " << i+1 << "/" << N_PAGE_SIZE << " = " << this->pageSizes[i];

        ASSERT_EQ(pageSizes[i], IMGMMU_GetCPUPageSize()) 
            << "ensure " << pageSizes[i] << " pages failed!";
        ASSERT_EQ(4<<10, MMU_PAGE_SIZE) 
            << "ensure 4kB device pages failed"; // possibly need to rewrite the test for other sizes

        pAllocA = TALALLOC_Malloc(sTALDeviceMemory, pageSizes[i], &ret);
	    ASSERT_TRUE(pAllocA != NULL) 
            << "allocate a single CPU page should work pagesize=" << pageSizes[i];
	    EXPECT_EQ(IMG_SUCCESS, ret) 
            << "non NULL return should be success pagesize=" << pageSizes[i];
	    EXPECT_EQ(1, pAllocA->uiNCPUPages) 
            << "only need 1 CPU page pagesize=" << pageSizes[i];
	    EXPECT_TRUE(pAllocA->aPhysAlloc != NULL) 
            << "no TAL handle pagesize=" << pageSizes[i];
        EXPECT_TRUE(pAllocA->aDevAddr != NULL) 
            << "no device page pagesize=" << pageSizes[i];

        pAllocB = TALALLOC_Malloc(sTALDeviceMemory, MMU_PAGE_SIZE, &ret);
	    ASSERT_TRUE(pAllocB != NULL) 
            << "allocate a single MMU page should work pagesize=" << pageSizes[i];
	    EXPECT_EQ(IMG_SUCCESS, ret) 
            << "non NULL return should be success pagesize=" << pageSizes[i];
	    EXPECT_EQ(1, pAllocB->uiNCPUPages) 
            << "only need 1 CPU page to allocate 1 MMU page pagesize=" << pageSizes[i];
	    EXPECT_TRUE(pAllocB->aPhysAlloc != NULL) 
            << "no TAL handle pagesize=" << pageSizes[i];
        EXPECT_TRUE(pAllocB->aDevAddr != NULL) 
            << "no device page pagesize=" << pageSizes[i];

        pAllocC = TALALLOC_Malloc(sTALDeviceMemory, pageSizes[i]+pageSizes[i]/2, &ret);
	    ASSERT_TRUE(pAllocC != NULL) 
            << "allocate 1+1/2 MMU page should work pagesize=" << pageSizes[i];
	    EXPECT_EQ(IMG_SUCCESS, ret) 
            << "non NULL return should be success pagesize=" << pageSizes[i];
	    EXPECT_EQ(2, pAllocC->uiNCPUPages) 
            << "need 2 CPU page to allocate 1+1/2 page pagesize=" << pageSizes[i];
	    EXPECT_TRUE(pAllocC->aPhysAlloc != NULL) 
            << "no TAL handle pagesize=" << pageSizes[i];
        EXPECT_TRUE(pAllocC->aDevAddr != NULL) 
            << "no device page pagesize=" << pageSizes[i];

        EXPECT_EQ(IMG_SUCCESS, TALALLOC_Free(pAllocA)) << "pagesize=" << pageSizes[i];
	    pAllocA = NULL;
	    EXPECT_EQ(IMG_SUCCESS, TALALLOC_Free(pAllocB)) << "pagesize=" << pageSizes[i];
	    pAllocB = NULL;
        EXPECT_EQ(IMG_SUCCESS, TALALLOC_Free(pAllocC)) << "pagesize=" << pageSizes[i];
	    pAllocC = NULL;
    }
}

TEST_F(TALAlloc, offset)
{
	IMG_RESULT ret;
	IMG_SIZE uiOff, uiGeneralOffset;
	IMGMMU_TALAlloc *pAlloc = NULL;
	IMG_UINTPTR sMem; // expected memory

    for ( int i = 0 ; i < N_PAGE_SIZE ; i++ )
    {
        ASSERT_EQ(IMG_SUCCESS, IMGMMU_SetCPUPageSize(pageSizes[i])) 
            << "failed to setup page size " << i+1 << "/" << N_PAGE_SIZE << " = " << this->pageSizes[i];

        ASSERT_EQ(pageSizes[i], IMGMMU_GetCPUPageSize()) 
            << "ensure " << pageSizes[i] << " pages failed!";
        ASSERT_EQ(4<<10, MMU_PAGE_SIZE) 
            << "ensure 4kB device pages failed"; // possibly need to rewrite the test for other sizes

	    pAlloc = TALALLOC_Malloc(sTALDeviceMemory, 2*pageSizes[i], &ret);
	    EXPECT_TRUE(pAlloc != NULL) << "allocate should work";
	    EXPECT_EQ(IMG_SUCCESS, ret) << "non NULL return should be success";

	    uiGeneralOffset = pageSizes[i]/2;
	    uiOff = uiGeneralOffset;
	    sMem = (IMG_UINTPTR)TALALLOC_ComputeOffset(pAlloc, &uiOff);
	    EXPECT_EQ(uiOff, uiGeneralOffset);
	    EXPECT_EQ((IMG_UINTPTR)pAlloc->aPhysAlloc[0], sMem); // first page

	    uiGeneralOffset = pageSizes[i];
	    uiOff = uiGeneralOffset;
	    sMem = (IMG_UINTPTR)TALALLOC_ComputeOffset(pAlloc, &uiOff);
	    EXPECT_EQ(uiOff, 0);
	    EXPECT_EQ((IMG_UINTPTR)pAlloc->aPhysAlloc[1], sMem); // 2nd page
	
	    uiGeneralOffset = pageSizes[i]+pageSizes[i]/2;
	    uiOff = uiGeneralOffset;
	    sMem = (IMG_UINTPTR)TALALLOC_ComputeOffset(pAlloc, &uiOff);
	    EXPECT_EQ(uiOff, pageSizes[i]/2);
	    EXPECT_EQ((IMG_UINTPTR)pAlloc->aPhysAlloc[1], sMem);

        uiGeneralOffset = MMU_PAGE_SIZE;
	    uiOff = uiGeneralOffset;
	    sMem = (IMG_UINTPTR)TALALLOC_ComputeOffset(pAlloc, &uiOff);
        if ( MMU_PAGE_SIZE == pageSizes[i] )
        {
	        EXPECT_EQ(uiOff, 0);
            EXPECT_EQ((IMG_UINTPTR)pAlloc->aPhysAlloc[1], sMem);
        }
        else
        {
            EXPECT_EQ(uiOff, MMU_PAGE_SIZE);
            EXPECT_EQ((IMG_UINTPTR)pAlloc->aPhysAlloc[0], sMem);
        }
	    
	    EXPECT_EQ(IMG_SUCCESS, TALALLOC_Free(pAlloc));
	    pAlloc = NULL;
    }
}

/// @ add checks for 16kB pages and get physical address list


// not testing updateDevice and updateHost because I did not find a way to access "device" memory of the TAL
// there must be a hacky way though...
