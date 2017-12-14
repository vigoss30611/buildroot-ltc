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
#include <img_defs.h>
#include <gtest/gtest.h>

// this is defined in imgmmu_test.cpp
void print_mmuInfo(void);

#ifdef IMG_MALLOC_TEST
IMG_UINT32 gui32AllocFails = 0;
#endif

int main(int argc, char** argv)
{
	int ret = EXIT_SUCCESS;
	
	print_mmuInfo();
	
	::testing::InitGoogleTest(&argc, argv);
	ret = RUN_ALL_TESTS();
	
	return ret;
}
