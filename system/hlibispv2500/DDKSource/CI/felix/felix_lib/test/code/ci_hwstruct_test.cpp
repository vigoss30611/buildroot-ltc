/**
******************************************************************************
 @file ci_hwstruct_test.cpp

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

#include "unit_tests.h"
#include "felix.h"

#include "ci/ci_api.h"
#include "ci_kernel/ci_kernel.h"

#include <hw_struct/ctx_pointers.h>
#include <hw_struct/ctx_config.h>
#include <hw_struct/save.h>

#include <tal.h>
#include <reg_io2.h>

#include "ci_kernel/ci_hwstruct.h"

#include "felix.h"
#include "unit_tests.h"

class HW_Load: public FullFelix, public ::testing::Test
{
public:
	char pConfig[FELIX_LOAD_STRUCTURE_BYTE_SIZE];

	HW_Load(): FullFelix()
	{
		IMG_MEMSET(pConfig, 0, FELIX_LOAD_STRUCTURE_BYTE_SIZE);
	}

	void SetUp()
	{
		configure(32, 32, YVU_420_PL12_8, RGB_888_24, false, 1);
	}
};

TEST_F(HW_Load, BLC)
{
	/// @ put values & check results
	HW_CI_Load_BLC(pConfig, &(pPipeline->sBlackCorrection));
}

TEST_F(HW_Load, RLT)
{
	/// @ put values & check results
    HW_CI_Load_RLT(pConfig, &(pPipeline->sRawLUT));
}

TEST_F(HW_Load, LSH_Deshading)
{
	/// @ put values & check results
	HW_CI_Load_LSH_DS(pConfig, &(pPipeline->sDeshading));
}

TEST_F(HW_Load, colourConv_YCC)
{
	/// @ put values & check results
	HW_CI_Load_Y2R(pConfig, &(pPipeline->sYCCToRGB));
}

TEST_F(HW_Load, colourConv_RGB)
{
	/// @ put values & check results
	HW_CI_Load_R2Y(pConfig, &(pPipeline->sRGBToYCC));
}

TEST_F(HW_Load, LSH)
{
	/// @ put values & check results
	HW_CI_Load_LCA(pConfig, &(pPipeline->sChromaAberration));
}

TEST_F(HW_Load, CCM)
{
	/// @ put values & check results
	HW_CI_Load_CCM(pConfig, &(pPipeline->sColourCorrection));
}

TEST_F(HW_Load, TNM)
{
	/// @ put values & check results
	HW_CI_Load_TNM(pConfig, &(pPipeline->sToneMapping));
}

TEST_F(HW_Load, DSC)
{
	/// @ put values & check results
	HW_CI_Load_Scaler(pConfig, &(pPipeline->sDisplayScaler));
}

TEST_F(HW_Load, ESC)
{
	/// @ put values & check results
	HW_CI_Load_Scaler(pConfig, &(pPipeline->sEncoderScaler));
}
