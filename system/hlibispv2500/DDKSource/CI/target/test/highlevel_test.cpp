/**
******************************************************************************
 @file highlevel_test.cpp

 @brief High level TAL tests

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

#include <gtest/gtest.h>

#include "tal_test.h"
#include "target.h"

struct HighLevel: public ::testing::Test, public TALBase
{
	void SetUp()
	{
		initialise(); // using structure
	}
	
	void TearDown()
	{
		finalise();
	}
};

/** 
 * @brief Tests that TAL can be init/deinit several times (using structure)
 */
TEST_F(HighLevel, init_deinit_struct)
{
	EXPECT_EQ(IMG_SUCCESS, TALSETUP_Initialise()) << "re-init TAL should not fail";
	
	EXPECT_EQ(IMG_SUCCESS, finalise()) << "deinit TAL should not fail";
	
	EXPECT_EQ(IMG_SUCCESS, initialise()) << "initialise TAL after being deinit should not fail";
}
