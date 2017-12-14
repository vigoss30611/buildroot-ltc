/**
******************************************************************************
 @file main_test.cpp

 @brief Point of entry of the unit tests

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

#ifdef IMG_MALLOC_TEST
IMG_UINT32 gui32AllocFails = 0;
#endif

void print_TALInfo(void)
{
	printf("TAL Info:");
// from FindTAL.cmake
#ifdef DEVIF_DEFINE_DEVIF1_FUNCS
	printf(" DEVIF_DEFINE_DEVIF1_FUNCS");
#endif
#ifdef TAL_PORT_FWRK
	printf(" TAL_PORT_FWRK");
#endif
#ifdef TAL_NORMAL
	printf(" TAL_NORMAL");
#endif
#ifdef TAL_LIGHT
	printf(" TAL_LIGHT");
#endif
#ifdef TAL64
	printf(" TAL64");
#endif
// from CMakeLists
#ifdef PDUMP1_NO_RELATIVE_OFFSET
	printf(" PDUMP1_NO_RELATIVE_OFFSET");
#endif
#ifdef __TAL_USE_MEOS__
	printf(" __TAL_USE_MEOS__");
#endif
#ifdef __TAL_USE_OSA__
	printf(" __TAL_USE_OSA__");
#endif
#ifdef __TAL_NOT_THREADSAFE__
	printf(" __TAL_NOT_THREADSAFE__");
#endif
	printf("\n");
}

int main(int argc, char** argv)
{
	int ret = EXIT_SUCCESS;
	
	print_TALInfo();
	
	::testing::InitGoogleTest(&argc, argv);
	ret = RUN_ALL_TESTS();
	
	return ret;
}
