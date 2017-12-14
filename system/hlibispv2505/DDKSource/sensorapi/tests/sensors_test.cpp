/**
******************************************************************************
 @file sensors_test.cpp

 @brief test the implementation of all available sensors

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
#include <sensorapi/sensorapi.h>

#include "sensorapi_test.hpp"
#ifndef INFOTM_ISP
#include "sensors/ar330.h"
#include "sensors/ov4688.h"
#include "sensors/p401.h"
#endif
#include <string.h>
#include <img_errors.h>

#include <ci/ci_api.h>

// just to have a different name
struct SensorsImplementation: public SensorFake
{
    static void testSensor(int s, const char *sensorName)
    {
        SENSOR_HANDLE sensor = NULL;
        
        ASSERT_EQ(IMG_SUCCESS, Sensor_Initialise(s, &sensor)) 
            << "failed to init sensor " << s << ": " << sensorName;
        
        SensorFake::testMinimal(sensor);
        
        if ( HasFailure() )
        {
            FAIL() << "failed to run tests for sensor " << s << ": " 
                << sensorName;
        }
    }
};

/**
 * @brief Tests that all implemented sensors can be initialised
 * 
 * @note disabled until we decide how to handle the connection to CI and fake
 * i2c connection.
 * Check CI_unit tests and may want to create a library so that the 
 * initialisation of the NULL interface does not have to be duplicated
 * 
 * @warning cannot do all at once because data generators need connection to 
 * CI to have some of their basic functions (like GetInfo()) working
 */
TEST_F(SensorsImplementation, DISABLED_initialiseSensors)
{
    unsigned s = 0, t;
    const char **sensorNames = Sensor_ListAll();
    const int toSearchSize = 3;
    const char *toSearch[] = { 
        AR330_SENSOR_INFO_NAME, 
        OV4688_SENSOR_INFO_NAME,
        P401_SENSOR_INFO_NAME };
    CI_CONNECTION *pConnection = NULL;
    IMG_RESULT ret;
    
    ASSERT_EQ(toSearchSize, sizeof(toSearch)/sizeof(const char*)) 
        << "update test!";
    ASSERT_GE(Sensor_NumberSensors(), 1u) 
        << "need at least 1 implemented sensor to run this test";
    
    ret = CI_DriverInit(&pConnection);
    ASSERT_EQ(IMG_SUCCESS, ret) << "failed to init CI";
        
    for ( t = 0 ; t < toSearchSize ; t++ )
    {
        for ( s = 0 ; s < Sensor_NumberSensors() ; s++ )
        {
            if (0==strncmp(sensorNames[s], toSearch[t], strlen(toSearch[t])))
            {
                testSensor(s, sensorNames[s]);
                t++;
            }
        }
    }
}
