/**
******************************************************************************
 @file compile_test.c

 @brief Tests that the data types and definitions are defined

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
#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>
#include <img_structs.h>

/*
 * integral types are defined
 */

IMG_CHAR type_char;
IMG_WCHAR type_wchar;
IMG_INT type_integer;
IMG_UINT type_unsigned_integer;
IMG_INT8 type_integer8;
IMG_UINT8 type_unsigned_integer8;
IMG_INT16 type_integer16;
IMG_UINT16 type_unsigned_integer16;
IMG_INT32 type_integer32;
IMG_UINT32 type_unsigned_integer32;
IMG_INT64 type_integer64;
IMG_UINT64 type_unsigned_integer64;

IMG_BYTE type_byte;
IMG_SIZE type_size;
IMG_UINTPTR type_uintptr;
IMG_PTRDIFF type_ptrdiff;

/*
 * floating point types
 */

IMG_FLOAT type_float;
IMG_DOUBLE type_double;

/*
 * global types
 */

IMG_VOID type_void(
	IMG_VOID
){};
IMG_HANDLE type_handle;

IMG_BOOL type_bool;
IMG_BOOL8 type_bool8;

#if !defined(IMG_FALSE) || IMG_FALSE != 0
#error "IMG_FLASE should be 0"
#endif
#if !defined(IMG_TRUE) || IMG_TRUE == 0
#error "IMG_TRUE should not be 0"
#endif

#ifdef IMG_BUILD_KO
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#define printf printk
#else
#include <stdio.h>
#endif

#ifdef IMG_MALLOC_CHECK
IMG_UINT32 gui32Alloc = 0;
IMG_UINT32 gui32Free = 0;
#endif
#ifdef IMG_MALLOC_TEST
IMG_UINT32 gui32AllocFails = 0;
#endif

IMG_UINT8 IMG_ALIGN(4) array[32];

struct element {
	int data;
};

struct container {
	struct element elementName;
};

void test_container_of(void)
{
	struct container container;
	struct container *pContainer = NULL;
	struct element *pElem = &(container.elementName);

	pElem->data = 2;
	
	pContainer = container_of(pElem, struct container, elementName);
	IMG_ASSERT(pContainer == &container);
}

void test_enums(void)
{
	IMG_eBufferType a;
	IMG_eOrientation b;

	a = IMG_BUFFERTYPE_FRAME;
	IMG_ASSERT(a == 0);
	a = IMG_BUFFERTYPE_FIELD_TOP;
	a = IMG_BUFFERTYPE_FIELD_BOTTOM;
    a = IMG_BUFFERTYPE_PAIR;

	b = IMG_ROTATE_NEVER;
	b = IMG_ROTATE_0;
    b = IMG_ROTATE_90;
    b = IMG_ROTATE_180;
    b = IMG_ROTATE_270;
}

IMG_INLINE void test_mallocs(IMG_CONST IMG_CHAR *pszName)
{
	IMG_CHAR *ptr = IMG_NULL;
	IMG_SIZE length = IMG_STRLEN(pszName)+1;

	ptr = (IMG_CHAR*)IMG_MALLOC(length);
	IMG_ASSERT(ptr);

	ptr = (IMG_CHAR*)IMG_REALLOC(ptr, length+1);
	IMG_ASSERT(ptr);

	IMG_MEMCPY(ptr, pszName, length);
	ptr[length] = '\0';

	IMG_ASSERT(IMG_MEMCMP(pszName, ptr, length) == 0);
	
	IMG_MEMSET(ptr, 0, length);
	IMG_ASSERT(IMG_STRCMP(pszName, ptr) != 0);

	IMG_STRNCPY(ptr, pszName, IMG_STRLEN(pszName));
	IMG_STRCPY(ptr, pszName);
	IMG_FREE(ptr);
	ptr = NULL;

	ptr = (IMG_CHAR*)IMG_CALLOC(length+1, sizeof(IMG_CHAR));
	IMG_ASSERT(ptr);
#ifdef NDEBUG // assert should be ignored
	IMG_ASSERT(0); // should not kill
#endif
	IMG_FREE(ptr);
	ptr = NULL;

	ptr = IMG_STRDUP(pszName);
	IMG_FREE(ptr);

	printf("%s successful (back to %s)\n", __FUNCTION__, pszName);
}

#if !defined(IMG_FSEEK64) && !defined(IMG_NO_FSEEK64)
#error "no fseek64"
#endif
#if !defined(IMG_FTELL64) && !defined(IMG_NO_FTELL64)
#error "no ftell64"
#endif

#if !defined(IMG_BYTE_ORDER) || !defined(IMG_LITTLE_ENDIAN) || !defined(IMG_BIG_ENDIAN)
#error "endianness not defined"
#elif (IMG_BYTE_ORDER!=IMG_LITTLE_ENDIAN) && (IMG_BYTE_ORDER!=IMG_BIG_ENDIAN)
#error "endianess is not little nor big endian"
#elif IMG_LITTLE_ENDIAN == IMG_BIG_ENDIAN
#error "no difference between little and big endian"
#endif

#ifndef IMG_BUILD_KO
int main(int argc, char *argv[])
#else
static int __init imginclude_build(void)
#endif
{
	IMG_RESULT type_result = IMG_SUCCESS;
	IMG_UINT64 size = 2;
	IMG_SIZE z = 3;
	IMG_PTRDIFF t = 11;
	type_result = IMG_ERROR_FATAL;
	printf("test_malloc  %" IMG_I64PR "u %" IMG_SIZEPR "u 0x%" IMG_PTRDPR "x ...\n", size, z, t);
	// use the defines
	test_mallocs(__FUNCTION__);	
	test_container_of();
	
#ifndef WIN32 // for linux IMG_I64PR is "ll" - here checking that long long is 64b indeed
	IMG_ASSERT(sizeof(long long int)==8);
#endif

	return IMG_SUCCESS;
}

#ifdef IMG_BUILD_KO

static void imginlude_exit(void)
{
}

module_init(imginclude_build);
module_exit(imginlude_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Imagination Technologies Ltd");
MODULE_DESCRIPTION("IMG include kernel build test");

#endif
