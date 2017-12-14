/**
*******************************************************************************
 @file sensor_test.cpp

 @brief Test the Sensor class

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
#include <ispc/Sensor.h>
#include <gtest/gtest.h>

#include <string>
#include <utility>  // pair
#include <list>

/**
 * @brief test that all state name return a string
 */
TEST(Sensor, stateName)
{
    for (int s = 0;
        s < static_cast<int>(ISPC::Sensor::SENSOR_CONFIGURED);
        s++ )
    {
        EXPECT_GT(strlen(ISPC::Sensor::StateName((ISPC::Sensor::State)s)), 0u)
            << "state " << s << " does not return a string";
    }
}

/**
 * @brief test that GetSensorNames() and GetSensorId() return the same
 * information
 *
 * GetSensorNames() should return more than one sensor
 * 
 * GetSensorId() should return the same ID than the second element of the
 * param names
 */
TEST(Sensor, GetSensorId)
{
    std::list<std::pair<std::string, int> >sensorNames;
    std::list<std::pair<std::string, int> >::const_iterator it;
    ISPC::Sensor::GetSensorNames(sensorNames);

    ASSERT_GT(sensorNames.size(), 0u) << "no name returned!";

    it = sensorNames.begin();
    while (it != sensorNames.end())
    {
        int id = ISPC::Sensor::GetSensorId(it->first);
        EXPECT_GE(id, 0) << "returned negative ID for sensor "
            << it->first << ":" << it->second;
        EXPECT_EQ(it->second, id)
            << "returned ID should be the same than returned by "
            << "GetSensorNames()";
        it++;
    }
}

/**
 * @brief test that a failure to create the sensor does not let it be
 * configured or enabled
 */
TEST(Sensor, failedToCreate)
{
    ISPC::Sensor sensor(-1);  // invalid ID

    ASSERT_EQ(ISPC::Sensor::SENSOR_ERROR, sensor.getState())
        << "should have failed to create";

    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, sensor.configure(0))
        << "should not be able to configure";

    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, sensor.enable())
        << "should not be able to enable a sensor that is not configured";
}
