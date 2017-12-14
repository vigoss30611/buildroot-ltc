/**
******************************************************************************
 @file test_assert.c

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
#include <img_defs.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef IMG_MALLOC_CHECK
IMG_UINT32 gui32Alloc = 0;
IMG_UINT32 gui32Free = 0;
#endif
#ifdef IMG_MALLOC_TEST
IMG_UINT32 gui32AllocFails = 0;
#endif

int main()
{
  printf("testing assert(1)\n");
  IMG_ASSERT(1);
  printf("testing assert(0)\n");
#if defined(EXIT_ON_ASSERT)&&!defined(NDEBUG)
  printf("   should exit\n");
#else
  printf("   sould not exit\n");
#endif
  IMG_ASSERT(0);
#if defined(EXIT_ON_ASSERT)&&!defined(NDEBUG)
  printf("FAILED: assert should have exited\n");
  return EXIT_FAILURE;
#else
  printf("SUCCESS: assert didn't exit\n");
  return EXIT_SUCCESS;
#endif
}
