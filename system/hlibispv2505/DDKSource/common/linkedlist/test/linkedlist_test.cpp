/**
******************************************************************************
@file linkedlist_test.cpp

@brief template double linked list tests

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
#include <cstdlib>
#include <cstdio>
#include <gtest/gtest.h>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include "linkedlist.h"

extern "C" {
#include "linkedlist.c"
}

/*
 * better have it even as N_Elem/2 is used
 */
#define N_Elem 50

/**
 * @brief Test class to setup 3 arrays and populate them with known values. Each array is N_Elem wide
 */
struct LinkedList: public ::testing::Test
{
	int *a;
	int *b;
	int *c;

	/*
	 * triggered before a new TEST_F
	 */
	void SetUp()
	{
		a = NULL;
		b = NULL;
		c = NULL;
	}

	void generateArrays()
	{
		if (!a) a = (int*)IMG_MALLOC(sizeof(int)*N_Elem);
		if (!b) b = (int*)IMG_MALLOC(sizeof(int)*N_Elem);
		if (!c) c = (int*)IMG_MALLOC(sizeof(int)*N_Elem);

		for (int i = 0 ; i < N_Elem ; i++ )
		{
			a[i] = i;
			b[i] = 2*i;
			c[i] = 3*i;
		}
	}

	/*
	 * triggered at the end of a TEST_F
	 */
	void TearDown()
	{
		if (a) IMG_FREE(a);
		if (b) IMG_FREE(b);
		if (c) IMG_FREE(c);
	}
};

/**
 * @brief Used for the constructor @ref createIntArray
 */
struct intArrayParam
{
	int nb;
	int multiplier;
};

/**
 * @brief Creates an Int array using @ref struct intArrayParam as a parameter
 */
void* createIntArray(void* param)
{
	int multiplier = ((intArrayParam*)param)->multiplier;
	int nb = ((intArrayParam*)param)->nb;
	int* a = new int[nb];

	for ( int i = 0 ; i < nb ; i++ )
	{
		a[i] = i*multiplier;
	}

	return (void*)a;
}

void deleteIntArray(void* param)
{
	delete [] (int*)param;
}

void deleteInt(void* param)
{
	delete (int*)param;
}

/**
 * @brief call functions with NULL parameters
 */
TEST_F (LinkedList, null_parameters)
{
	sLinkedList_T sList;
	sCell_T cell; // just to have cell to play with

	/*
	 * List_init
	 */
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_init(NULL));

	/*
	 * List_createPool
	 */	
	ASSERT_EQ (IMG_SUCCESS, List_initPool(&sList, 10, NULL, NULL)) << "can create a pool without constructor";
	
	cell.pContainer = NULL;
	cell.pNext = NULL;
	cell.pPrev = NULL;
	cell.object = NULL;

	/*
	 * List_clear
	 */
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_clear(NULL)) << "List_clear";

	/*
	 * List_freeObject
	 */
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_clearObjects(NULL, NULL));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_clearObjects(&sList, NULL));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_clearObjects(NULL, &deleteInt));

	/*
	 * List_pushBackObject
	 */
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_pushBackObject(NULL, NULL));
	EXPECT_EQ (IMG_SUCCESS, List_pushBackObject(&sList, NULL)) << "can add NULL object";
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_pushBackObject(NULL, (void*)1));

	/*
	 * List_pushFrontObject
	 */

	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_pushFrontObject(NULL, NULL));
	EXPECT_EQ (IMG_SUCCESS, List_pushFrontObject(&sList, NULL));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_pushFrontObject(NULL, (void*)1));

	/*
	 * List_pushBack
	 */

	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_pushBack(NULL, NULL));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_pushBack(&sList, NULL)) << "can't add NULL cell";
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_pushBack(NULL, &cell));

	/*
	 * List_pushFront
	 */

	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_pushFront(NULL, NULL));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_pushFront(&sList, NULL)) << "can't add NULL cell";
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_pushFront(NULL, &cell));

	/*
	 * List_detach
	 */
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_detach(NULL));

	/*
	 * List_remove
	 */
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_remove(NULL));

	/*
	 * List_removeObject
	 */
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_removeObject(NULL, NULL));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_removeObject(&cell, NULL));
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, List_removeObject(NULL, &deleteInt));

	/*
	 * List_popBack
	 */
	EXPECT_TRUE (List_popBack(NULL) == NULL);

	/*
	 * List_popFront
	 */
	EXPECT_TRUE (List_popFront(NULL) == NULL);

	/*
	 * List_getHead
	 */
	EXPECT_TRUE (List_getHead(NULL) == NULL);

	/*
	 * List_getTail
	 */
	EXPECT_TRUE (List_getTail(NULL) == NULL);

	/*
	 * List_isEmpty
	 */
	EXPECT_EQ (IMG_TRUE, List_isEmpty(NULL));

	/*
	 * List_nElem
	 */
	EXPECT_EQ (0, List_nElem(NULL));

	// clean list
	List_clear(&sList);
}

#ifdef IMG_MALLOC_TEST
IMG_UINT32 gui32AllocFails = 0;

TEST_F (LinkedList, malloc_failed)
{
	sLinkedList_T sList;

	ASSERT_EQ (IMG_SUCCESS, List_init(&sList));

	{
		gui32AllocFails = 1;
		EXPECT_EQ (IMG_ERROR_MALLOC_FAILED, List_pushBackObject(&sList, NULL));
		gui32AllocFails = 1;
		EXPECT_EQ (IMG_ERROR_MALLOC_FAILED, List_pushFrontObject(&sList, NULL));
	}
}
#endif //IMG_MALLOC_TEST

TEST_F (LinkedList, create_lists_pools_and_free)
{
	sLinkedList_T sList;

	ASSERT_EQ (IMG_SUCCESS, List_init(&sList));
	
	EXPECT_TRUE (List_getHead(&sList) == NULL);
	EXPECT_TRUE (List_getTail(&sList) == NULL);
	EXPECT_EQ (0, List_nElem(&sList));
	EXPECT_EQ (IMG_TRUE, List_isEmpty(&sList));
	//EXPECT_EQ (IMG_ERROR_MEMORY_IN_USE, List_create(&sList));

	EXPECT_EQ (IMG_SUCCESS, List_clear(&sList));

	EXPECT_TRUE (List_getHead(&sList) == NULL);
	EXPECT_TRUE (List_getTail(&sList) == NULL);
	EXPECT_EQ (0, List_nElem(&sList));
	EXPECT_EQ (IMG_TRUE, List_isEmpty(&sList));

	ASSERT_EQ (IMG_SUCCESS, List_initPool(&sList, 10, NULL, NULL));
	
	EXPECT_TRUE (List_getHead(&sList) != NULL);
	EXPECT_TRUE (List_getTail(&sList) != NULL);
	EXPECT_EQ (10, List_nElem(&sList));
	EXPECT_EQ (IMG_FALSE, List_isEmpty(&sList));

	EXPECT_EQ (IMG_SUCCESS, List_clear(&sList));

	EXPECT_TRUE (List_getHead(&sList) == NULL);
	EXPECT_TRUE (List_getTail(&sList) == NULL);
	EXPECT_EQ (0, List_nElem(&sList));
	EXPECT_EQ (IMG_TRUE, List_isEmpty(&sList));
}

TEST_F (LinkedList, create_pool_array)
{
	sLinkedList_T sPool;
	struct intArrayParam param;

	param.nb = 10;
	param.multiplier = 0;

	ASSERT_EQ (IMG_SUCCESS, List_initPool(&sPool, 10, (void*)(&param), &createIntArray));

	EXPECT_EQ (10, List_nElem(&sPool));

	ASSERT_EQ (IMG_SUCCESS, List_clearObjects(&sPool, &deleteIntArray));
}

void* createInt(void *param)
{
	struct intArrayParam *info = (struct intArrayParam *)param;
	static int value = 0;
	int *ret;

	if ( value >= info->nb )
		value = 0;

	ret = new int();

	*ret = value;

	value++;

	return (void*)ret;
}

TEST_F(LinkedList, pushback)
{
	sCell_T sCellA, sCellB;
	sLinkedList_T sList;

	ASSERT_EQ(IMG_SUCCESS, List_init(&sList));

	EXPECT_TRUE(sList.sAnchor.pNext == &(sList.sAnchor));
	EXPECT_TRUE(sList.sAnchor.pPrev == &(sList.sAnchor));

	IMG_MEMSET(&sCellA, 0, sizeof(sCell_T));
	IMG_MEMSET(&sCellB, 0, sizeof(sCell_T));

	EXPECT_EQ(IMG_SUCCESS, List_pushBack(&sList, &sCellA));
	EXPECT_TRUE(sList.sAnchor.pNext == &sCellA); // head
	EXPECT_TRUE(sList.sAnchor.pPrev == &sCellA); // tail
	EXPECT_TRUE(sCellA.pPrev == &(sList.sAnchor));
	EXPECT_TRUE(sCellA.pNext == &(sList.sAnchor));
	EXPECT_TRUE(sCellA.pContainer == &sList);

	EXPECT_EQ(IMG_SUCCESS, List_pushBack(&sList, &sCellB));
	EXPECT_TRUE(sList.sAnchor.pNext == &sCellA); // head
	EXPECT_TRUE(sList.sAnchor.pPrev == &sCellB); // tail
	EXPECT_TRUE(sCellA.pNext == &(sCellB));
	EXPECT_TRUE(sCellA.pPrev == &(sList.sAnchor));
	EXPECT_TRUE(sCellB.pNext == &(sList.sAnchor));
	EXPECT_TRUE(sCellB.pPrev == &(sCellA));
}

TEST_F(LinkedList, pushfront)
{
	sCell_T sCellA, sCellB;
	sLinkedList_T sList;

	ASSERT_EQ(IMG_SUCCESS, List_init(&sList));

	EXPECT_TRUE(sList.sAnchor.pNext == &(sList.sAnchor));
	EXPECT_TRUE(sList.sAnchor.pPrev == &(sList.sAnchor));

	IMG_MEMSET(&sCellA, 0, sizeof(sCell_T));
	IMG_MEMSET(&sCellB, 0, sizeof(sCell_T));

	EXPECT_EQ(IMG_SUCCESS, List_pushFront(&sList, &sCellA));
	EXPECT_TRUE(sList.sAnchor.pNext == &sCellA); // head
	EXPECT_TRUE(sList.sAnchor.pPrev == &sCellA); // tail
	EXPECT_TRUE(sCellA.pPrev == &(sList.sAnchor));
	EXPECT_TRUE(sCellA.pNext == &(sList.sAnchor));
	EXPECT_TRUE(sCellA.pContainer == &sList);

	EXPECT_EQ(IMG_SUCCESS, List_pushFront(&sList, &sCellB));
	EXPECT_TRUE(sList.sAnchor.pNext == &sCellB); // head
	EXPECT_TRUE(sList.sAnchor.pPrev == &sCellA); // tail
	EXPECT_TRUE(sCellA.pPrev == &(sCellB));
	EXPECT_TRUE(sCellA.pNext == &(sList.sAnchor));
	EXPECT_TRUE(sCellB.pPrev == &(sList.sAnchor));
	EXPECT_TRUE(sCellB.pNext == &(sCellA));

	EXPECT_TRUE(sCellB.pContainer == &sList);
}

TEST_F(LinkedList, detach)
{
	sCell_T sCellA, sCellB, sCellC;
	sLinkedList_T sList;

	ASSERT_EQ(IMG_SUCCESS, List_init(&sList));

	IMG_MEMSET(&sCellA, 0, sizeof(sCell_T));
	IMG_MEMSET(&sCellB, 0, sizeof(sCell_T));
	IMG_MEMSET(&sCellC, 0, sizeof(sCell_T));

	EXPECT_EQ(IMG_SUCCESS, List_pushBack(&sList, &sCellA));
	EXPECT_EQ(IMG_SUCCESS, List_pushBack(&sList, &sCellB));
	EXPECT_EQ(IMG_SUCCESS, List_pushBack(&sList, &sCellC));
	
	EXPECT_TRUE(sCellA.pNext == &sCellB);
	EXPECT_TRUE(sCellA.pPrev == &(sList.sAnchor));
}

TEST_F (LinkedList, findPointer)
{
	sCell_T sCellA, sCellB, *pFound = NULL;
	sLinkedList_T sList;

	ASSERT_EQ(IMG_SUCCESS, List_init(&sList));

	IMG_MEMSET(&sCellA, 0, sizeof(sCell_T));
	sCellA.object = (void*)1;
	IMG_MEMSET(&sCellB, 0, sizeof(sCell_T));
	sCellB.object = (void*)2;

	EXPECT_EQ(IMG_SUCCESS, List_pushBack(&sList, &sCellA));
	pFound = List_visitor(&sList, sCellB.object, &ListVisitor_findPointer);
	EXPECT_TRUE (pFound == NULL);
	
	EXPECT_EQ(IMG_SUCCESS, List_pushBack(&sList, &sCellB));
	pFound = List_visitor(&sList, sCellB.object, &ListVisitor_findPointer);
	EXPECT_TRUE (pFound == &(sCellB));
}

TEST_F (LinkedList, swap_between_lists)
{
	sLinkedList_T sListA, sListB;
	sCell_T *pCell = NULL;
	struct intArrayParam param;

	param.nb = 20;
	param.multiplier = 0;

	ASSERT_EQ (IMG_SUCCESS, List_initPool(&sListA, 10, (void*)(&param), &createInt));
	ASSERT_EQ (IMG_SUCCESS, List_initPool(&sListB, 10, (void*)(&param), &createInt));

	EXPECT_EQ (10, List_nElem(&sListA));
	EXPECT_EQ (10, List_nElem(&sListB));

	int i = 0;

	// we know that
	EXPECT_EQ (0, *((int*)List_getHead(&sListA)->object));
	EXPECT_EQ (9, *((int*)List_getTail(&sListA)->object));
	EXPECT_EQ (10, *((int*)List_getHead(&sListB)->object));
	EXPECT_EQ (19, *((int*)List_getTail(&sListB)->object));

	// test that pushBack and pushFront cannot be called with already attached cells
	EXPECT_EQ (IMG_ERROR_UNEXPECTED_STATE, List_pushBack(&sListA, List_getHead(&sListB)));
	EXPECT_EQ (IMG_ERROR_UNEXPECTED_STATE, List_pushFront(&sListA, List_getHead(&sListB)));

	// move all &sListB into &sListA from last to first
	i = 1;
	while ( (pCell=List_getTail(&sListB)) != NULL )
	{
		EXPECT_EQ (IMG_SUCCESS, List_detach(pCell));
		EXPECT_EQ (IMG_SUCCESS, List_pushBack(&sListA, pCell));

		EXPECT_EQ (10-i, List_nElem(&sListB));
		EXPECT_EQ (10+i, List_nElem(&sListA));

		i++;
	}
	
	EXPECT_EQ (0, *((int*)List_getHead(&sListA)->object));
	EXPECT_EQ (10, *((int*)List_getTail(&sListA)->object));
	EXPECT_EQ (IMG_TRUE, List_isEmpty(&sListB)) << "nelem="<<List_nElem(&sListB);
	EXPECT_EQ (IMG_FALSE, List_isEmpty(&sListA));

	// then put first elements in listB
	i = 1;
	while ( (pCell=List_getHead(&sListA)) != NULL && i <= 10 )
	{
		EXPECT_EQ (IMG_SUCCESS, List_detach(pCell));
		EXPECT_EQ (IMG_SUCCESS, List_pushFront(&sListB, pCell));

		EXPECT_EQ (i, List_nElem(&sListB));
		EXPECT_EQ (20-i, List_nElem(&sListA));

		i++;
	}

	// we know that
	EXPECT_EQ (19, *((int*)List_getHead(&sListA)->object));
	EXPECT_EQ (10, *((int*)List_getTail(&sListA)->object));
	EXPECT_EQ (9, *((int*)List_getHead(&sListB)->object));
	EXPECT_EQ (0, *((int*)List_getTail(&sListB)->object));

	pCell = List_getHead(&sListA);
	int* pC = (int*)pCell->object;
	List_remove(pCell);
	deleteInt(pC);

	ASSERT_EQ (IMG_SUCCESS, List_clearObjects(&sListA, &deleteInt));
	ASSERT_EQ (IMG_SUCCESS, List_clearObjects(&sListB, &deleteInt));
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
