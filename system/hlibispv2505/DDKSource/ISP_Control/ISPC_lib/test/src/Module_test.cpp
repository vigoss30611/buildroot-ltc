/**
*******************************************************************************
 @file Module_test.cpp

 @brief Test the Module classes

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
#include <ispc/Module.h>
#include <ispc/ISPCDefs.h>
#include <gtest/gtest.h>

#undef VERBOSE

#ifndef VERBOSE
#undef printf
#define printf(...)
#endif /* VERBOSE */

/**
 * @brief need to derive due to access to protected fields
 *
 * @note all pure virtual methods have to be implemented
 */
class ControlModuleChild : public ISPC::ControlModule
{
public:

    ControlModuleChild()
    : ControlModule("test") {}

    virtual IMG_RESULT programCorrection()
    {
        return IMG_SUCCESS;
    }

    virtual IMG_RESULT update(const ISPC::Metadata &metadata)
    {
        return IMG_SUCCESS;
    }

    virtual IMG_RESULT configureStatistics()
    {
        return IMG_SUCCESS;
    }

    virtual bool hasConverged() const
    {
        return false;
    }

    virtual ISPC::ControlID getModuleID() const
    {
        return ISPC::CTRL_AUX1;
    }

    virtual IMG_RESULT load(const ISPC::ParameterList &parameters)
    {
        return IMG_SUCCESS;
    }
    virtual IMG_RESULT save(ISPC::ParameterList &parameters, SaveType t) const
    {
        return IMG_SUCCESS;
    }

    // test support
    auto getPipelineOwnerIterator() -> decltype(pipelineOwner)
    { return ISPC::ControlModule::pipelineOwner; }

    auto getPipelineOwnerContainerEnd() -> decltype(pipelineOwner)
    { return pipelineList.end(); }
};

class ControlModule: public ControlModuleChild, public testing::Test {
};

TEST_F(ControlModule, ControlModule_constructor)
{
    ControlModuleChild module;
    EXPECT_EQ(module.getPipelineOwnerContainerEnd(),
              module.getPipelineOwnerIterator());
}


