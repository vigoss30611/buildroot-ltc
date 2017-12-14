/**
******************************************************************************
 @file ci_mem_test.cpp

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
#include <cstdio>
#include <cstring>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <tal.h>

#include "unit_tests.h"
#include "felix.h"

#include "ci/ci_api.h"
#include "ci_kernel/ci_connection.h"
#include "ci_kernel/ci_kernel.h"
#include "ci_kernel/ci_hwstruct.h"
#include "ci_kernel/ci_ioctrl.h"

#include <hw_struct/ctx_pointers.h>
#include <hw_struct/ctx_config.h>
#include <hw_struct/save.h>



//as defined in shot.c
#define HW_OUTPUT_OFF(reg) FELIX_LINK_LIST_ ## reg ## _OFFSET

static IMG_UINT32 getTalInternalAdd(KRN_CI_SHOT *pBuffer)
{
	IMG_UINT32 oriContext, address;
	IMG_UINT32 *pHWPtr = (IMG_UINT32 *)Platform_MemGetMemory(&(pBuffer->hwLinkedList));

	//TALMEM_ReadWord32(pBuffer->hwLinkedList.hMem, HW_OUTPUT_OFF(CONTEXT_TAG), &oriContext);
	oriContext = pHWPtr[HW_OUTPUT_OFF(CONTEXT_TAG)/4];
	SYS_MemWriteAddress(&(pBuffer->hwLinkedList), HW_OUTPUT_OFF(CONTEXT_TAG), &(pBuffer->hwLinkedList), 0);
	//TALMEM_WriteDevMemRef32(pBuffer->hwLinkedList.hMem, HW_OUTPUT_OFF(CONTEXT_TAG), pBuffer->hwLinkedList.hMem, 0);
	//TALMEM_ReadWord32(pBuffer->hwLinkedList.hMem, HW_OUTPUT_OFF(CONTEXT_TAG), &address);
	address = pHWPtr[HW_OUTPUT_OFF(CONTEXT_TAG)/4];
	//TALMEM_WriteWord32(pBuffer->hwLinkedList.hMem, HW_OUTPUT_OFF(CONTEXT_TAG), oriContext);
	SYS_MemWriteWord(&(pBuffer->hwLinkedList), HW_OUTPUT_OFF(CONTEXT_TAG), oriContext);

	return address;
}

/// temp function - duplicate of the ci_capture.c one
static IMG_BOOL8 FindFirstWithStatus(void* listParam, void* status)
{
	KRN_CI_SHOT *pGivenShot = static_cast<KRN_CI_SHOT*>(listParam);
	enum KRN_CI_SHOT_eSTATUS searchedStatus = *((enum KRN_CI_SHOT_eSTATUS*)status);

	if ( pGivenShot->eStatus == searchedStatus ) return IMG_FALSE; // stop processing
	return IMG_TRUE; // continue processing
}

// the elements are not attached together anymore
/*static IMG_BOOL8 checkAttached(void* elem, void* param)
  {
  KRN_CI_SHOT *pBuffer = (KRN_CI_SHOT*)elem;
  KRN_CI_PIPELINE *pCapture = (KRN_CI_PIPELINE*)param;
  IMG_UINT32 val;

  if ( pBuffer->hwLinkedList.hMem == NULL )
  {
  ADD_FAILURE() << "hwPointers not allocated";
  return IMG_FALSE;
  }
  if ( pBuffer->hwLoadStructure.hMem == NULL )
  {
  ADD_FAILURE() << "hwLoadStructure not allocated";
  return IMG_FALSE;
  }
  if ( pBuffer->hwSave.hMem == NULL )
  {
  ADD_FAILURE() << "hwSave not allocated";
  return IMG_FALSE;
  }

  if ( pCapture == NULL ) // in the config - checking that all CI_SHOT have no links
  {
//TALMEM_ReadWord32(pBuffer->hwLinkedList.hMem, HW_OUTPUT_OFF(CONTEXT_TAG), &val);
//EXPECT_EQ (SYSMEM_INVALID_ADDR, val) << "capture address is not correct";

TALMEM_ReadWord32(pBuffer->hwLinkedList.hMem, HW_OUTPUT_OFF(CONTEXT_LINK_ADDR), &val);
EXPECT_EQ (SYSMEM_INVALID_ADDR, val) << "next address is not correct";
}
else
{
sCell_T *pCell = container_of(elem, sCell_T, object);
TALMEM_ReadWord32(pBuffer->hwLinkedList.hMem, HW_OUTPUT_OFF(CONTEXT_LINK_ADDR), &val);

sCell_T *pCurr = List_getHead(pCell->pContainer);
sCell_T *pPendingTail = List_getTail(pCell->pContainer);

if ( pCurr == NULL || pPendingTail == NULL )
{
ADD_FAILURE() << "empty list\n";
}

if ( pBuffer == (KRN_CI_SHOT*)pPendingTail->object )
{
// if tail no other elements

EXPECT_EQ (SYSMEM_INVALID_ADDR, val) << "next address is not correct";
}
else
{
// pCurr is currently the head
// find the element
while (pCurr != NULL && pBuffer != (KRN_CI_SHOT*)pCurr->object)
{
pCurr = List_getNext(pCurr);
}

if ( pCurr == NULL )
{
ADD_FAILURE() << "next address is not correct";
}
else
{
KRN_CI_SHOT* pNext = (KRN_CI_SHOT*)pCurr->pNext->object;
// check its next one is correct
EXPECT_EQ (getTalInternalAdd(pNext), val );
}
}

}

return IMG_TRUE;
}*/

TEST(IMG_CI_HIDEMALLOC, test)
{
	void *ptr;
	gui32AllocFails = 1;

	ptr = IMG_MALLOC(12);
	EXPECT_TRUE ( ptr == NULL );
	if ( ptr != NULL ) IMG_FREE(ptr);
}

/// @ verify that this test is usefull
TEST(Memstruct, HW_Context)
{
	FullFelix driver;
	KRN_CI_PIPELINE *pPrivateCapture = NULL;
	IMG_UINT32 nBuffer = 0;

	driver.configure();

	pPrivateCapture = driver.getKrnPipeline();

	//pCapture->bEncPipeline = IMG_TRUE;

	EXPECT_EQ (IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline));

	// elements are not attached together anymore
	//EXPECT_TRUE ( List_visitor(pPrivateCapture->pList_available, NULL, &checkAttached) == NULL ); // true if all list visited - not using capture means they should not be attached

	// push 1 into available list
	// check SW attached is correct
	// check next elem is FELIX NULL
	for (IMG_UINT32 i = 0 ; i < nBuffer ; i++ )
	{
		struct CI_BUFFER_TRIGG param;
		IMG_MEMSET(&param, 0, sizeof(struct CI_BUFFER_TRIGG));
		param.captureId = pPrivateCapture->ui32Identifier;
		param.bBlocking = IMG_FALSE;
        param.bufferIds.encId = 0;
        param.bufferIds.dispId = 0;

		EXPECT_EQ (IMG_SUCCESS, INT_CI_PipelineTriggerShoot(driver.getKrnConnection(), &param));

		// elements are not attached together anymore
		//EXPECT_TRUE ( List_visitor(pPrivateCapture->pList_available, NULL, &checkAttached) == NULL ); // true if all list visited - not using capture means they should not be attached
		//EXPECT_TRUE ( List_visitor(pPrivateCapture->pList_pending, (void*)pPrivateCapture, &checkAttached) == NULL ); // true if all list visited - using the capture means the shots are expected to be attached
	}

	// push 1 into filled list
	// check SW detached is correct
	for (IMG_UINT32 i = 0 ; i < nBuffer ; i++ )
	{
		ASSERT_EQ(IMG_SUCCESS, Felix::fakeInterrupt(pPrivateCapture));

		//EXPECT_TRUE ( List_visitor(pPrivateCapture->slist_buffer_filled, NULL, &checkAttached) == NULL ); // true if all list visited - not using capture
		//EXPECT_TRUE ( List_visitor(pPrivateCapture->slist_buffer_processing, (void*)pPrivateCapture, &checkAttached) == NULL ); // true if all list visited
	}
}

TEST(Memstruct, memAllocFree)
{
	SYS_MEM sMem;
	IMG_MEMSET(&sMem, 0, sizeof(SYS_MEM));

	// no MMU
	{
		Felix driver;

		driver.configure();

		gui32AllocFails = 1;
		EXPECT_EQ (IMG_ERROR_MALLOC_FAILED, SYS_MemAlloc(&sMem, SYSMEM_ALIGNMENT/2, NULL, 0));

		EXPECT_EQ (IMG_SUCCESS, SYS_MemAlloc(&sMem, SYSMEM_ALIGNMENT/2, NULL, 0));

		EXPECT_EQ (PAGE_SIZE, sMem.uiAllocated);
		EXPECT_TRUE ( sMem.pAlloc != NULL ) << "host pointer";
		//EXPECT_TRUE ( sMem.hMem != NULL ) << "TAL handle";

		EXPECT_EQ(IMG_SUCCESS, SYS_MemFree(&sMem));

		EXPECT_EQ (0, sMem.uiAllocated);
		EXPECT_TRUE ( sMem.pAlloc == NULL ) << "host pointer";
		//EXPECT_TRUE ( sMem.hMem == NULL ) << "TAL handle";

		// driver deinit here
	}
	///< @ add MMU case
}

TEST(Memstruct, getPointers)
{
	SYS_MEM sMem;
	char *ptr = NULL, *ptr2 = NULL;
	char ori[] = "the cat!";
	IMG_MEMSET(&sMem, 0, sizeof(SYS_MEM));

	///< @ add MMU case
	{
		Felix driver;

		driver.configure();

		EXPECT_EQ (IMG_SUCCESS, SYS_MemAlloc(&sMem, strlen(ori)+1, NULL, 0));

		//EXPECT_EQ (0, ((char*)sMem.pvCpuLin)[0]); // that is not true: memory not init to 0

		ptr = (char*)Platform_MemGetMemory(&sMem);

		ptr2 = (char*)((struct SYS_MEM_ALLOC*)sMem.pAlloc)->pCPUMem;
		EXPECT_TRUE ( ptr == ptr2 );

		strcpy(ptr, ori);

		EXPECT_EQ (IMG_SUCCESS, Platform_MemUpdateDevice(&sMem)); // updates

		ptr = NULL;
		ptr = (char*)Platform_MemGetMemory(&sMem);

		for ( IMG_UINT32 i = 0 ; i < strlen(ori) ; i++ )
		{
			EXPECT_EQ (ori[i], ptr2[i]);
		}

		Platform_MemUpdateDevice(&sMem);

		SYS_MemFree(&sMem);
		// driver deinit here
	}
}

TEST(Memstruct, writeWord)
{
	SYS_MEM sMem;
	char *ptr = NULL, *ptr2 = NULL;
	char ori[] = "the cat!";
	IMG_MEMSET(&sMem, 0, sizeof(SYS_MEM));

	{
		///< @ add MMU case
		Felix driver;

		driver.configure();

		EXPECT_EQ (IMG_SUCCESS, SYS_MemAlloc(&sMem, strlen(ori)+1, NULL, 0));

		ASSERT_EQ (0, strlen(ori)%4);

		for ( IMG_UINT32 word = 0 ; word < strlen(ori)/4 ; word++ )
		{
			IMG_UINT32 reg = 0;
			for ( int byte = 0 ; byte < 4 ; byte++ )
			{
				reg += ori[word*4+byte]<<(byte)*8;
			}

			EXPECT_EQ (IMG_SUCCESS, SYS_MemWriteWord(&sMem, word*4, reg));
		}

		ptr = (char*)Platform_MemGetMemory(&sMem);
		ptr2 = (char*)((struct SYS_MEM_ALLOC*)sMem.pAlloc)->pCPUMem;

		for ( IMG_UINT32 i = 0 ; i < strlen(ori) ; i++ )
		{
			EXPECT_EQ (ori[i], ptr2[i]);
		}

		EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, SYS_MemWriteWord(&sMem, sMem.uiAllocated+1, 0)) << "writting more than allocated";

		SYS_MemFree(&sMem);
		// driver deinit here
	}
}

TEST(Memstruct, writeAddress)
{
	SYS_MEM sMemA, sMemB;
	IMG_MEMSET(&sMemA, 0, sizeof(SYS_MEM));
	IMG_MEMSET(&sMemB, 0, sizeof(SYS_MEM));

	{
		///< @ add MMU case
		Felix driver;

		driver.configure();

		EXPECT_EQ (IMG_SUCCESS, SYS_MemAlloc(&sMemA, 32, NULL, 0));

		EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, SYS_MemWriteAddress(&sMemA, 0, &sMemB, 0)) << "address not init";
		EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, SYS_MemWriteAddress(&sMemB, 0, &sMemA, 0)) << "current one not init";

		EXPECT_EQ (IMG_SUCCESS, SYS_MemAlloc(&sMemB, 32, NULL, 0));
		//((struct SYS_MEM_ALLOC*)sMem.pAlloc)->pHostMem
		IMG_MEMSET(Platform_MemGetMemory(&sMemA), 0, 32);
		EXPECT_EQ (IMG_SUCCESS, Platform_MemUpdateDevice(&sMemA));

		EXPECT_EQ (IMG_SUCCESS, SYS_MemWriteAddress(&sMemA, 0, &sMemB, 0));

		//EXPECT_EQ (TALDEBUG_GetDevMemAddress(sMemB.hMem), *((IMG_UINT64*)sMemA.pvCpuLin));
		//EXPECT_EQ(Platform_MemGetDevMem(&sMemB), (IMG_UINTPTR)((struct SYS_MEM_ALLOC*)sMemB.pAlloc)->pHostMem);

		EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, SYS_MemWriteWord(&sMemA, sMemA.uiAllocated, 0)) << "writting more than allocated";

		SYS_MemFree(&sMemA);
		SYS_MemFree(&sMemB);
		// driver deinit here
	}
}

/**
 * @brief Tests that the extra allocation needed for the alignment is memset to CI_MEMSET_ALIGNMENT
 */
TEST(Memstruct, memsetAlignment)
{
#ifdef CI_MEMSET_ALIGNMENT
	SYS_MEM sMem;
	Felix driver;
	size_t alloc = SYSMEM_ALIGNMENT/2;

	IMG_MEMSET(&sMem, 0, sizeof(SYS_MEM));

	driver.configure();

	EXPECT_EQ(IMG_SUCCESS, SYS_MemAlloc(&sMem, alloc, NULL, 0)); // will align to SYSMEM_ALIGNMENT
	EXPECT_EQ(PAGE_SIZE, sMem.uiAllocated);

	{
		char *buff = (char*)calloc(alloc, 1); // all set to 0
		//EXPECT_EQ(0, memcmp(buff, Platform_MemGetMemory(&(sMem)), alloc)) << "memset to 0";
		memset(Platform_MemGetMemory(&sMem), 0, alloc); // memset to 0

		memset(buff, CI_MEMSET_ALIGNMENT, SYSMEM_ALIGNMENT-alloc);
		EXPECT_EQ(0, memcmp(buff, (char*)Platform_MemGetMemory(&(sMem))+alloc, alloc)) << "memset to " << CI_MEMSET_ALIGNMENT;

		free(buff);
	}

	EXPECT_EQ(IMG_SUCCESS, SYS_MemFree(&sMem));

	// driver deinits here
#endif
}
