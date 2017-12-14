/**
******************************************************************************
 @file imgmmu_test.cpp

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

#include <img_defs.h>

extern "C" {
	#include "imgmmu.c"
}

// used in main to print mmu info
void print_mmuInfo(void)
{
	printf("IMGMMU_PHYS_SIZE %u\n", IMGMMU_PHYS_SIZE);
	printf("IMGMMU_VIRT_SIZE %u\n", IMGMMU_VIRT_SIZE);
	printf("IMGMMU_PAGE_SIZE %u\n", IMGMMU_PAGE_SIZE);
	printf("IMGMMU_PAGE_SHIFT %u\n", IMGMMU_PAGE_SHIFT);
	printf("IMGMMU_DIR_SHIFT %u\n", IMGMMU_DIR_SHIFT);
}

extern "C" {

struct MMUPage* Fake_PageAlloc(void)
{
	IMGMMU_Page *pPage = NULL;
	IMG_UINT32 physMask = (~ ( (1<<(IMGMMU_PAGE_SHIFT-(IMGMMU_PHYS_SIZE-IMGMMU_VIRT_SIZE)))-1) );

	pPage = (IMGMMU_Page *)IMG_CALLOC(1, sizeof(IMGMMU_Page));
	if ( pPage != NULL )
	{
		pPage->uiCpuVirtAddr = (IMG_UINTPTR)malloc(IMGMMU_PAGE_SIZE);
		pPage->uiPhysAddr = pPage->uiCpuVirtAddr & physMask;
	}
	return pPage;
}

void Fake_PageFree(struct MMUPage *pPage)
{
	free((void*)pPage->uiCpuVirtAddr);
	IMG_FREE(pPage);
}

void Fake_PageUpdate(struct MMUPage *pPage)
{
	(void)pPage;
}

}

struct Directory: public ::testing::Test
{
	struct MMUDirectory *pDir;
	IMGMMU_Info configuration;

	Directory()
	{
		pDir = NULL;
		IMG_MEMSET(&configuration, 0, sizeof(IMGMMU_Info));
		configuration.pfnPageAlloc = &Fake_PageAlloc;
		configuration.pfnPageFree = &Fake_PageFree;
		configuration.pfnPageWrite = NULL;
		configuration.pfnPageRead = NULL;
		configuration.pfnPageUpdate = &Fake_PageUpdate;
	}

	// done at start of unit test
	void SetUp()
	{
		IMG_RESULT res;

		if ( pDir != NULL )
		{
			FAIL() << "pDir is not NULL!";
			pDir = NULL;
			return;
		}
		
		ASSERT_TRUE( (pDir=IMGMMU_DirectoryCreate(&configuration, &res)) != NULL) << "creation of directory failed";
		ASSERT_EQ(IMG_SUCCESS, res);
	}
	
	// "allocates" and map a fake page
	struct MMUMapping* map(struct MMUDirectory *pDirectory, IMG_UINTPTR uiStartDev, IMG_RESULT &res, unsigned nPages = 1, unsigned int uiMapFlags = 0)
	{
		struct MMUMapping *pMapping = NULL;
		IMG_UINT64 *aPages = NULL;
		IMGMMU_HeapAlloc sHeapAlloc;
		// to mask the bottom bits that should be there if it was real a page allocation at 4k boundaries
		IMG_UINT32 physMask = (~ ( (1<<(IMGMMU_PAGE_SHIFT-(IMGMMU_PHYS_SIZE-IMGMMU_VIRT_SIZE)))-1) );

		sHeapAlloc.uiVirtualAddress = uiStartDev;
		sHeapAlloc.uiAllocSize = (IMGMMU_PAGE_SIZE*nPages);

		aPages = (IMG_UINT64*)calloc(nPages, sizeof(IMG_UINT64));
		
		for ( unsigned i = 1 ; i < nPages ; i++ )
		{
			aPages[i] = (IMG_UINT64)(aPages + i*(IMG_UINTPTR)pDirectory) & physMask; // just to create some random address - MMU does not access the pages
		}
		
		pMapping = IMGMMU_DirectoryMap(pDirectory, aPages, &sHeapAlloc, uiMapFlags, &res);

		free(aPages);

		return pMapping;
	}

	// called when test finishes
	void TearDown()
	{
		if ( pDir != NULL )
		{
			IMGMMU_DirectoryDestroy(pDir);
			pDir = NULL;
		}
	}
};

/**
 * @brief Creates and destroy an MMU directory correctly
 */
TEST_F(Directory, create_destroy)
{
	//IMG_RESULT res;
	
	// done at setup
	//EXPECT_TRUE( (pDir=IMGMMU_DirectoryCreate(&configuration, &res)) != NULL);
	//EXPECT_EQ(IMG_SUCCESS, res);
		
	// access the page table directly... a bit violent
	IMG_UINT32 *pMemory = (IMG_UINT32 *)pDir->pDirectoryPage->uiCpuVirtAddr;

	for ( size_t i = 0 ; i < IMGMMU_N_TABLE ; i++ )
	{
		EXPECT_EQ(MMU_FLAG_INVALID, pMemory[i]) << "expect page " << i << " to be invalid";
	}
	
	EXPECT_EQ(IMG_SUCCESS, IMGMMU_DirectoryDestroy(pDir));
	pDir = NULL; // so it is not destroyed again
}

TEST_F(Directory, create_destroy_invalid)
{
	struct MMUDirectory *pSecDir = NULL;
	IMG_RESULT res;
	
	configuration.pfnPageAlloc = NULL;
	configuration.pfnPageFree = NULL;
			
	EXPECT_TRUE( (pSecDir=IMGMMU_DirectoryCreate(&configuration, &res)) == NULL);
	EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, res);

	configuration.pfnPageAlloc = &Fake_PageAlloc;
			
	EXPECT_TRUE( (pSecDir=IMGMMU_DirectoryCreate(&configuration, &res)) == NULL);
	EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, res);

	configuration.pfnPageAlloc = NULL;
	configuration.pfnPageFree = &Fake_PageFree;
			
	EXPECT_TRUE( (pSecDir=IMGMMU_DirectoryCreate(&configuration, &res)) == NULL);
	EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, res);

#ifdef IMG_MALLOC_TEST
	configuration.pfnPageAlloc = &Fake_PageAlloc;
	configuration.pfnPageFree = &Fake_PageFree;

	gui32AllocFails = 1; // next directory struct fails
	EXPECT_TRUE( (pSecDir=IMGMMU_DirectoryCreate(&configuration, &res)) == NULL) << "should have failled allocation the physical page";
	EXPECT_EQ(IMG_ERROR_MALLOC_FAILED, res);
	EXPECT_TRUE (pSecDir == NULL) << "oups should not have been";

	gui32AllocFails = 2; // next directory struct fails
	EXPECT_TRUE( (pSecDir=IMGMMU_DirectoryCreate(&configuration, &res)) == NULL) << "should have failled allocation the physical page";
	EXPECT_EQ(IMG_ERROR_MALLOC_FAILED, res);
	EXPECT_TRUE (pSecDir == NULL) << "oups should not have been";

	gui32AllocFails = 3; // next page alloc will fail!
	EXPECT_TRUE( (pSecDir=IMGMMU_DirectoryCreate(&configuration, &res)) == NULL) << "should have failled allocation the physical page";
	EXPECT_EQ(IMG_ERROR_FATAL, res);
	EXPECT_TRUE (pSecDir == NULL) << "oups should not have been";
#endif
}

/**
 * @brief Test a very simple mapping of a single page
 */
TEST_F(Directory, map_single)
{
	struct MMUMapping *pMapping = NULL;
	IMG_RESULT res;
	
	// map with a wrong flag
	pMapping = this->map(pDir, 0, res, 1, MMU_FLAG_VALID); // MMU_FLAG_VALID is an internal flag and should not be used
	ASSERT_TRUE( pMapping == NULL);
	ASSERT_EQ( IMG_ERROR_INVALID_PARAMETERS, res);

	pMapping = this->map(pDir, 0, res, 1, 0x7); // sets the 1st bit is invalid
	ASSERT_TRUE( pMapping == NULL);
	ASSERT_EQ( IMG_ERROR_INVALID_PARAMETERS, res);

	// map a single page
	pMapping = this->map(pDir, 0, res, 1, 0x2);
	ASSERT_TRUE( pMapping != NULL);
	ASSERT_EQ( IMG_SUCCESS, res);
	EXPECT_TRUE( pDir == pMapping->pDirectory );
	EXPECT_EQ(0x2, pMapping->uiUsedFlag );
	EXPECT_EQ(1, pDir->ui32NMapping);
	EXPECT_EQ(1, pMapping->ui32NEntries);
	{
		EXPECT_TRUE(pDir->ppPageMap[0] != NULL) << "page table should have been created";
		// direct access to memory is a bit violent
		IMG_UINT32 *pMemory = NULL;
		
		pMemory = (IMG_UINT32*)pDir->pDirectoryPage->uiCpuVirtAddr;
		EXPECT_EQ(0, pMemory[0]&MMU_FLAG_INVALID) << "new entry in directory should not be invalid";

		pMemory = (IMG_UINT32*)pDir->ppPageMap[0]->pPage->uiCpuVirtAddr;
		EXPECT_EQ(0, pMemory[0]&MMU_FLAG_INVALID) << "new entry in page table should not be invalid";

		for ( size_t i = 1 ; i < IMGMMU_N_TABLE ; i++ )
		{
			EXPECT_TRUE( pDir->ppPageMap[i] == NULL ) << "page table should not have been allocated";
		}
	}

	EXPECT_EQ( IMG_SUCCESS, IMGMMU_DirectoryUnMap(pMapping) );

	{
		EXPECT_TRUE(pDir->ppPageMap[0] != NULL) << "page table should not have been destroyed";
		// direct access to memory is a bit violent
		IMG_UINT32 *pMemory = NULL;
		
		pMemory = (IMG_UINT32*)pDir->pDirectoryPage->uiCpuVirtAddr;
		EXPECT_EQ(0, pMemory[0]&MMU_FLAG_INVALID) << "page table in directory should not be invalid";

		pMemory = (IMG_UINT32*)pDir->ppPageMap[0]->pPage->uiCpuVirtAddr;
		EXPECT_EQ(MMU_FLAG_INVALID, pMemory[0]&MMU_FLAG_INVALID) << "entry in page table should be invalid";
	}

#ifdef IMG_MALLOC_TEST
	gui32AllocFails = 1; // Mapping structure
	pMapping = this->map(pDir, 0, res, 1, 0);
	ASSERT_TRUE(pMapping == NULL);
	EXPECT_EQ(IMG_ERROR_MALLOC_FAILED, res);

	gui32AllocFails = 3; // 2nd page table creation fails - to verify leak if running with valgind
	pMapping = this->map(pDir, 2*IMGMMU_N_PAGE*IMGMMU_PAGE_SIZE, res, 2, 0);
	ASSERT_TRUE(pMapping == NULL);
	EXPECT_EQ(IMG_ERROR_FATAL, res);
#endif
}

/**
 * @brief Try to map several pages to a similar place and then to a different place
 */
TEST_F(Directory, map_single_overlap)
{
	struct MMUMapping *pMappingA = NULL, *pMappingB = NULL;
	IMG_RESULT res;
	
	// map a single page in the 1st entry
	pMappingA = this->map(pDir, 0, res, 1, 0);
	ASSERT_TRUE (pMappingA != NULL);

	// try to use same memory than mappingA
	pMappingB = this->map(pDir, 0, res, 1, 0);
	EXPECT_TRUE( pMappingB == NULL );
	EXPECT_EQ( IMG_ERROR_MEMORY_IN_USE, res ) << "result should be MEMORY_IN_USE";

	// map a single page in the 2nd entry
	pMappingB = this->map(pDir, IMGMMU_PAGE_SIZE*IMGMMU_N_PAGE, res, 1, 0);
	EXPECT_TRUE (pMappingB != NULL);
	
	for ( size_t i = 0 ; i < 2 ; i++ )
	{
		EXPECT_TRUE(pDir->ppPageMap[i] != NULL) << "page table " << i << " should have been created";
		// direct access to memory is a bit violent
		IMG_UINT32 *pMemory = NULL;
		
		pMemory = (IMG_UINT32*)pDir->pDirectoryPage->uiCpuVirtAddr;
		EXPECT_EQ(0, pMemory[i]&MMU_FLAG_INVALID) << "entry " << i << " in directory should not be invalid";

		pMemory = (IMG_UINT32*)pDir->ppPageMap[i]->pPage->uiCpuVirtAddr;
		EXPECT_EQ(0, pMemory[0]&MMU_FLAG_INVALID) << "entry in page table should not be invalid";
	}
	for ( size_t i = 2 ; i < IMGMMU_N_TABLE ; i++ ) // other pages
	{
		EXPECT_TRUE( pDir->ppPageMap[i] == NULL ) << "page table " << i << " should not have been allocated";
	}

	EXPECT_EQ(IMG_SUCCESS, IMGMMU_DirectoryUnMap(pMappingA));
	{
		EXPECT_TRUE(pDir->ppPageMap[0] != NULL) << "page table should not have been destroyed";
		// direct access to memory is a bit violent
		IMG_UINT32 *pMemory = NULL;
		
		pMemory = (IMG_UINT32*)pDir->pDirectoryPage->uiCpuVirtAddr;
		EXPECT_EQ(0, pMemory[0]&MMU_FLAG_INVALID) << "page table in directory should not be invalid";

		pMemory = (IMG_UINT32*)pDir->ppPageMap[0]->pPage->uiCpuVirtAddr;
		EXPECT_EQ(MMU_FLAG_INVALID, pMemory[0]&MMU_FLAG_INVALID) << "entry in page table should be invalid";
	}

	EXPECT_EQ(IMG_SUCCESS, IMGMMU_DirectoryUnMap(pMappingB));
}

/**
 * @brief Simple multi page mapping
 */
TEST_F(Directory, map_multi)
{
	struct MMUMapping *pMappingA = NULL, *pMappingB = NULL;
	IMG_RESULT res;
	IMG_UINT32 *pMemory = NULL; // direct access to memory is a bit violent
	
	// map a 4 pages in the 1st entry
	pMappingA = this->map(pDir, 0, res, 4, 0);
	ASSERT_TRUE (pMappingA != NULL);

	// overlapping memory
	pMappingB = this->map(pDir, 2*IMGMMU_PAGE_SIZE, res, 8, 0);
	EXPECT_TRUE( pMappingB == NULL );
	EXPECT_EQ( IMG_ERROR_MEMORY_IN_USE, res ) << "result should be MEMORY_IN_USE";

	// map a single page in the 2nd entry (but leave the first 4 page empty
	pMappingB = this->map(pDir, IMGMMU_PAGE_SIZE*IMGMMU_N_PAGE+4*IMGMMU_PAGE_SIZE, res, 8, 0);
	EXPECT_TRUE (pMappingB != NULL);

	// now verify the page tables
	{
		// the 1st directory should have its first 4 pages mapped
		EXPECT_TRUE(pDir->ppPageMap[0] != NULL) << "page table should not have been destroyed";
		// direct access to memory is a bit violent
		IMG_UINT32 *pMemory = NULL;
		
		pMemory = (IMG_UINT32*)pDir->pDirectoryPage->uiCpuVirtAddr;
		EXPECT_EQ(0, pMemory[0]&MMU_FLAG_INVALID) << "page table 0 in directory should not be invalid";
		EXPECT_EQ(0, pMemory[1]&MMU_FLAG_INVALID) << "page table 1 in directory should not be invalid";
		// other pages should be invalid
		for ( size_t i = 2 ; i < IMGMMU_N_TABLE ; i++ )
		{
			EXPECT_TRUE( pDir->ppPageMap[i] == NULL ) << "page table " << i << " should not have been allocated";
			EXPECT_EQ(MMU_FLAG_INVALID, pMemory[i]&MMU_FLAG_INVALID) << "page table " << i << " in directory should not be valid";
		}
	}
	{
		size_t i;
		EXPECT_TRUE(pDir->ppPageMap[1] != NULL) << "page table should not have been destroyed";
		EXPECT_EQ(4, pDir->ppPageMap[0]->ui32ValidEntries);

		pMemory = (IMG_UINT32*)pDir->ppPageMap[0]->pPage->uiCpuVirtAddr;
		for (i = 0 ; i < 4 ; i++ )
		{
			EXPECT_EQ(0, pMemory[i]&MMU_FLAG_INVALID) << "page table offset " << i << "should be valid in directory entry 0";
		}
		for( ; i < IMGMMU_N_PAGE ; i++)
		{
			EXPECT_EQ(MMU_FLAG_INVALID, pMemory[i]&MMU_FLAG_INVALID) << "page table offset " << i << "should be invalid in directory entry 0";
		}

		EXPECT_EQ(8, pDir->ppPageMap[1]->ui32ValidEntries);
		pMemory = (IMG_UINT32*)pDir->ppPageMap[1]->pPage->uiCpuVirtAddr;
		for (i = 0 ; i < 4 ; i++ )
		{
			EXPECT_EQ(MMU_FLAG_INVALID, pMemory[i]&MMU_FLAG_INVALID) << "page table offset " << i << "should be invalid in directory entry 1";
		}
		for ( ; i < 4+8 ; i++ )
		{
			EXPECT_EQ(0, pMemory[i]&MMU_FLAG_INVALID) << "page table offset " << i << "should be valid in directory entry 1";
		}
		for( ; i < IMGMMU_N_PAGE ; i++)
		{
			EXPECT_EQ(MMU_FLAG_INVALID, pMemory[i]&MMU_FLAG_INVALID) << "page table offset " << i << "should be invalid in directory entry 1";
		}
	}

	IMGMMU_DirectoryUnMap(pMappingB);

	EXPECT_TRUE(pDir->ppPageMap[1] != NULL) << "page table should not have been freed";
	EXPECT_EQ(0, pDir->ppPageMap[1]->ui32ValidEntries);
	pMemory = (IMG_UINT32*)pDir->ppPageMap[1]->pPage->uiCpuVirtAddr;
	for( size_t i = 0 ; i < IMGMMU_N_PAGE ; i++)
	{
		EXPECT_EQ(MMU_FLAG_INVALID, pMemory[i]&MMU_FLAG_INVALID) << "page table offset " << i << "should be invalid in directory entry 1";
	}

	IMGMMU_DirectoryUnMap(pMappingA);

	EXPECT_TRUE(pDir->ppPageMap[0] != NULL) << "page table should not have been freed";
	EXPECT_EQ(0, pDir->ppPageMap[0]->ui32ValidEntries);
	pMemory = (IMG_UINT32*)pDir->ppPageMap[0]->pPage->uiCpuVirtAddr;
	for( size_t i = 0 ; i < IMGMMU_N_PAGE ; i++)
	{
		EXPECT_EQ(MMU_FLAG_INVALID, pMemory[i]&MMU_FLAG_INVALID) << "page table offset " << i << "should be invalid in directory entry 0";
	}
}

/**
 * @brief test a relatively large allocation (multi-directory wide)
 */
TEST_F(Directory, stress_full_alloc)
{
	struct MMUMapping *pMapping[IMGMMU_N_TABLE];
	IMG_RESULT res;
	IMG_UINT32 *pMemory = NULL; // direct access to memory is a bit violent
	
	memset(pMapping, 0, 1024*sizeof(struct MMUMapping *));

	for ( unsigned int m = 0 ; m < IMGMMU_N_TABLE ; m++ )
	{
		pMapping[m] = this->map(pDir, m*IMGMMU_N_PAGE*IMGMMU_PAGE_SIZE, res, IMGMMU_N_PAGE, 0);
		ASSERT_TRUE(pMapping[m] != NULL);
		ASSERT_EQ(IMG_SUCCESS, res);
	}

	// now the virtual memory should be full - no mapping will work
	for ( int i = 0 ; i < IMGMMU_N_TABLE ; i++ )
	{
		pMemory = NULL;
		ASSERT_TRUE( pDir->ppPageMap[i] != NULL );
		pMemory = (IMG_UINT32*)pDir->pDirectoryPage->uiCpuVirtAddr;
		ASSERT_EQ(0, pMemory[i]&MMU_FLAG_INVALID) << "the directory entry " << i << " should not be invalid";

		ASSERT_EQ(IMGMMU_N_PAGE, pDir->ppPageMap[i]->ui32ValidEntries);
		pMemory = (IMG_UINT32*)pDir->ppPageMap[i]->pPage->uiCpuVirtAddr;
		for ( int j = 0 ; j < IMGMMU_N_PAGE ; j++ )
		{
			ASSERT_EQ(0, pMemory[j]&MMU_FLAG_INVALID) << "the page table entry " << j << " in directory " << i << " should not be invalid";
		}
	}

	for ( int m = 0 ; m < IMGMMU_N_TABLE ; m++ )
	{
		IMGMMU_DirectoryUnMap(pMapping[m]);
	}

	// now the virtual memory should be empty - any mapping should work
	for ( int i = 0 ; i < IMGMMU_N_TABLE ; i++ )
	{
		pMemory = NULL;
		ASSERT_TRUE( pDir->ppPageMap[i] != NULL );
		pMemory = (IMG_UINT32*)pDir->pDirectoryPage->uiCpuVirtAddr;
		ASSERT_EQ(0, pMemory[i]&MMU_FLAG_INVALID) << "the directory entry " << i << " should not be invalid";

		ASSERT_EQ(0, pDir->ppPageMap[i]->ui32ValidEntries);
		pMemory = (IMG_UINT32*)pDir->ppPageMap[i]->pPage->uiCpuVirtAddr;
		for ( int j = 0 ; j < IMGMMU_N_PAGE ; j++ )
		{
			ASSERT_EQ(MMU_FLAG_INVALID, pMemory[j]&MMU_FLAG_INVALID) << "the page table entry " << j << " in directory " << i << " should be invalid";
		}
	}

	IMGMMU_DirectoryClean(pDir); // relatively useless but just to verify everything is clean
	for ( int i = 0 ; i < IMGMMU_N_TABLE ; i++ )
	{
		pMemory = NULL;
		ASSERT_TRUE( pDir->ppPageMap[i] == NULL );
		pMemory = (IMG_UINT32*)pDir->pDirectoryPage->uiCpuVirtAddr;
		ASSERT_EQ(MMU_FLAG_INVALID, pMemory[i]&MMU_FLAG_INVALID) << "the directory entry " << i << " should not be invalid";
	}
}

TEST_F(Directory, read_entries)
{
	struct MMUMapping *pMapping = NULL;
	IMG_RESULT res;
	IMG_UINT64 sPage;
	IMGMMU_HeapAlloc sHeapAlloc;
	IMG_UINT32 expectedPage = 0; // expected page entry
	IMG_UINT32 expectedDir = 0; // expected directory entry
	IMG_UINT32 physMask = (~ ( (1<<(IMGMMU_PAGE_SHIFT-(IMGMMU_PHYS_SIZE-IMGMMU_VIRT_SIZE)))-1) );
	unsigned int mapflags = 0x2;

	sHeapAlloc.uiVirtualAddress = 0x0;
	sHeapAlloc.uiAllocSize = (IMGMMU_PAGE_SIZE);

	sPage = (IMG_UINTPTR)&sPage & physMask; // fake physical address
	expectedPage = (IMG_UINT32)sPage;
	if ( (IMGMMU_PHYS_SIZE-IMGMMU_VIRT_SIZE) > 0 ) // MMU HW shift
	{
		expectedPage >>= (IMGMMU_PHYS_SIZE-IMGMMU_VIRT_SIZE);
	}
	
	// map a single page at virt address 0
	pMapping = IMGMMU_DirectoryMap(pDir, &sPage, &sHeapAlloc, mapflags, &res);
	ASSERT_TRUE( pMapping != NULL);
	ASSERT_EQ( IMG_SUCCESS, res);

	EXPECT_EQ(0, IMGMMU_DirectoryGetDirectoryEntry(pDir, 0x400000)) << "unmapped virtual address - directory entry empty";
	EXPECT_EQ((IMG_UINT32)-1, IMGMMU_DirectoryGetPageTableEntry(pDir, 0x400000)) << "unmapped virtual address - page does not exists";

	EXPECT_EQ(0, IMGMMU_DirectoryGetPageTableEntry(pDir, 0x10000)) << "unmapped virtual address - page exists";
	EXPECT_EQ(0, IMGMMU_DirectoryGetPageTableEntry(pDir, 0x10000)) << "unmapped virtual address - page entry is empty";

	
	// check directory entry has the correct page

	// check that page table entry has the correct value
	// expectedPage already has the physical address, let's apply the flags and the valid bit
	EXPECT_EQ(expectedPage|mapflags|0x1, IMGMMU_DirectoryGetPageTableEntry(pDir, 0x0));

	expectedDir = (IMG_UINT32)pDir->ppPageMap[0]->pPage->uiPhysAddr;
	if ( (IMGMMU_PHYS_SIZE-IMGMMU_VIRT_SIZE) > 0 ) // MMU HW shift
	{
		expectedDir >>= (IMGMMU_PHYS_SIZE-IMGMMU_VIRT_SIZE);
	}
	// expectedDir has the page table physical address, let's apply the valid bit
	EXPECT_EQ(expectedDir|0x1, IMGMMU_DirectoryGetDirectoryEntry(pDir, 0x0));
}
