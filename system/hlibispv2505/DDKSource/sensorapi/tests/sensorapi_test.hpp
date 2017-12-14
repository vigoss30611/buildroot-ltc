/**
******************************************************************************
 @file sensorapi_test.hpp

 @brief test the basic implementation of sensorapi

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
#ifndef SENSORAPI_TEST
#define SENSORAPI_TEST

#include <gtest/gtest.h>
#include <sensorapi/sensorapi.h>

extern "C" {

    typedef struct FakeSensor
    {
        struct _Sensor_Functions_ sFunctions;
        
        int mode;
        int flipping;
        SENSOR_STATE state;
        double gain;
        int exposure;
    }FakeSensor;

    #define TO_HANDLE(s) ((SENSOR_HANDLE)&s)
    
    void Fake_Initialise(FakeSensor *sensor);

}

struct SensorFake: ::testing::Test
{
    void SetUp()
    {
        
    }
    
    void TearDown()
    {
    }

    /** @brief configure a minimal fake sensor */
    static void configureMinimal(FakeSensor &sensor);
    
    /** 
     * @brief test that minimal functions are implemented
     * @return if Sensor_Destroyed was called (last one)
     */
    static bool testMinimal(SENSOR_HANDLE sensor);

    static bool testFlipping(SENSOR_HANDLE sensor, IMG_UINT16 ui16Mode, IMG_UINT8 ui8Flipping);
};

#endif // SENSORAPI_TEST
