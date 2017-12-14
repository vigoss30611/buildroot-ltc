/**
******************************************************************************
 @file sensorapi_test.cpp

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

#include <gtest/gtest.h>
#include <sensorapi/sensorapi.h>
#include <felixcommon/pixel_format.h>
#include "sensorapi_test.hpp"

#include <img_types.h>
#include <img_errors.h>

/** @brief checks that listAll can be called and at least 1 sensor is implemented */
TEST(Sensor_API, listAll)
{
    const char **list = 0;
    ASSERT_GE(Sensor_NumberSensors(), 1u) << "need at least 1 sensor implemented to run that test";
    
    list = Sensor_ListAll();
}

TEST(Sensor_API, wrongInitialise)
{
    unsigned int n = Sensor_NumberSensors();
    SENSOR_HANDLE sensor = NULL;
    
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_Initialise(n, &sensor));
    EXPECT_TRUE(NULL == sensor) << "should not have changed";
    
    if ( n > 0 )
    {
        EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_Initialise(0, NULL));
    }
}

extern "C" {
   /*
    * functions for the fake sensor
    */ 
    
    
#define TO_HANDLE(s) ((SENSOR_HANDLE)&s)
    
    void Fake_Initialise(FakeSensor *sensor)
    {
        sensor->mode = -1;
        sensor->flipping = SENSOR_FLIP_NONE;
        sensor->gain = 1.0f;
        sensor->exposure = 10000;
        sensor->state = SENSOR_STATE_UNINITIALISED;
        
        
        // should set the functions here but we clean it instead
        memset(&(sensor->sFunctions), 0, sizeof(struct _Sensor_Functions_));
    }
    
    static IMG_RESULT Fake_GetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nIndex, SENSOR_MODE *psModes)
    {
        //FakeSensor *sensor = (FakeSensor*)hHandle;
                
        switch(nIndex)
        {
        case 0:
            psModes->ui8BitDepth = 10;
            psModes->ui16Width = 640;
            psModes->ui16Height = 480;
            psModes->flFrameRate = 1.0f;
            psModes->ui16HorizontalTotal = psModes->ui16Width
                + psModes->ui16Width / 10;
            psModes->ui16VerticalTotal = psModes->ui16Height
                + psModes->ui16Height / 10;
            psModes->flPixelRate = psModes->ui16HorizontalTotal
                *psModes->ui16VerticalTotal*psModes->flFrameRate;
            psModes->ui8SupportFlipping = SENSOR_FLIP_NONE;
            psModes->ui32ExposureMin = 90;
            psModes->ui32ExposureMin = 9000;
            psModes->ui8MipiLanes = 0;
            return IMG_SUCCESS;
        case 1:
            psModes->ui8BitDepth = 10;
            psModes->ui16Width = 1280;
            psModes->ui16Height = 720;
            psModes->flFrameRate = 1.0f;
            psModes->ui16HorizontalTotal = psModes->ui16Width
                + psModes->ui16Width / 10;
            psModes->ui16VerticalTotal = psModes->ui16Height
                + psModes->ui16Height / 10;
            psModes->flPixelRate = psModes->ui16HorizontalTotal
                *psModes->ui16VerticalTotal*psModes->flFrameRate;
            psModes->ui8SupportFlipping = SENSOR_FLIP_BOTH;
            psModes->ui32ExposureMin = 90;
            psModes->ui32ExposureMin = 9000;
            psModes->ui8MipiLanes = 1;
            return IMG_SUCCESS;
        }
        return IMG_ERROR_VALUE_OUT_OF_RANGE;
    }

    static IMG_RESULT Fake_GetState(SENSOR_HANDLE hHandle, SENSOR_STATUS *psStatus)
    {
        FakeSensor *sensor = (FakeSensor*)hHandle;
        
        if (sensor->mode >= 0 )
        {
            psStatus->ui16CurrentMode = sensor->mode;
            psStatus->ui8Flipping = sensor->flipping;
        }
        psStatus->eState = sensor->state;
        
        return IMG_SUCCESS;
    }
    
    static IMG_RESULT Fake_SetMode(SENSOR_HANDLE hHandle, IMG_UINT16 nMode, IMG_UINT8 ui8Flipping)
    {
        FakeSensor *sensor = (FakeSensor*)hHandle;
        /// @ implement the state and mode saving into sensorapi not per sensor
        
        if (SENSOR_STATE_RUNNING == sensor->state)
        {
            return IMG_ERROR_UNEXPECTED_STATE;
        }
        
        if (2 > nMode)
        {
            SENSOR_MODE sMode; // this is normally stored
            Fake_GetMode(hHandle, nMode, &sMode);
            if(ui8Flipping != (ui8Flipping&(sMode.ui8SupportFlipping)))
            {
                return IMG_ERROR_NOT_SUPPORTED;
            }
            
            sensor->mode = nMode;
            sensor->flipping = ui8Flipping;
            sensor->state = SENSOR_STATE_IDLE;
            return IMG_SUCCESS;
        }
        return IMG_ERROR_VALUE_OUT_OF_RANGE;
    }
    
    static IMG_RESULT Fake_Enable(SENSOR_HANDLE hHandle)
    {
        FakeSensor *sensor = (FakeSensor*)hHandle;
        
        if (SENSOR_STATE_IDLE == sensor->state)
        {
            sensor->state = SENSOR_STATE_RUNNING;
            return IMG_SUCCESS;
        }
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    
    static IMG_RESULT Fake_Disable(SENSOR_HANDLE hHandle)
    {
        FakeSensor *sensor = (FakeSensor*)hHandle;
        
        if (SENSOR_STATE_RUNNING == sensor->state)
        {
            sensor->state = SENSOR_STATE_IDLE;
            return IMG_SUCCESS;
        }
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    
    static IMG_RESULT Fake_Destroy(SENSOR_HANDLE hHandle)
    {
        FakeSensor *sensor = (FakeSensor*)hHandle;
        
        if (SENSOR_STATE_IDLE == sensor->state)
        {
            sensor->state = SENSOR_STATE_UNINITIALISED;
            sensor->mode = -1;
            return IMG_SUCCESS;
        }
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    
    static IMG_RESULT Fake_GetGainRange(SENSOR_HANDLE hHandle, double *pflMin, double *pflMax, IMG_UINT8 *puiContexts)
    {
        FakeSensor *sensor = (FakeSensor*)hHandle;
        
        if (SENSOR_STATE_UNINITIALISED != sensor->state)
        {
            *pflMin = 1.0f;
            *pflMax = 10.0f;
            *puiContexts = 0;
            return IMG_SUCCESS;
        }
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    
    static IMG_RESULT Fake_GetCurrentGain(SENSOR_HANDLE hHandle, double *pflCurrent, IMG_UINT8 ui8Context)
    {
        FakeSensor *sensor = (FakeSensor*)hHandle;
        
        if (SENSOR_STATE_UNINITIALISED != sensor->state)
        {
            if (ui8Context == 0)
            {
                *pflCurrent = sensor->gain;
                return IMG_SUCCESS;
            }
            return IMG_ERROR_VALUE_OUT_OF_RANGE;
        }
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    
    static IMG_RESULT Fake_SetGain(SENSOR_HANDLE hHandle, double flGain, IMG_UINT8 ui8Context)
    {
        FakeSensor *sensor = (FakeSensor*)hHandle;
        
        if (SENSOR_STATE_UNINITIALISED != sensor->state)
        {
            if (ui8Context == 0)
            {
                sensor->gain = flGain;
                return IMG_SUCCESS;
            }
            return IMG_ERROR_VALUE_OUT_OF_RANGE;
        }
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    
    static IMG_RESULT Fake_GetExposureRange(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Min, IMG_UINT32 *pui32Max, IMG_UINT8 *pui8Contexts)
    {
        FakeSensor *sensor = (FakeSensor*)hHandle;
        
        if (SENSOR_STATE_UNINITIALISED != sensor->state)
        {
            *pui32Min = 1000;
            *pui32Max = 100000;
            *pui8Contexts = 0;
            return IMG_SUCCESS;
        }
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    
    static IMG_RESULT Fake_GetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 *pui32Exposure, IMG_UINT8 ui8Context)
    {
        FakeSensor *sensor = (FakeSensor*)hHandle;
        
        if (SENSOR_STATE_UNINITIALISED != sensor->state)
        {
            if(ui8Context == 0)
            {
                *pui32Exposure = sensor->exposure;
                return IMG_SUCCESS;
            }
            return IMG_ERROR_NOT_SUPPORTED;
        }
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    
    static IMG_RESULT Fake_SetExposure(SENSOR_HANDLE hHandle, IMG_UINT32 ui32Exposure, IMG_UINT8 ui8Context)
    {
        FakeSensor *sensor = (FakeSensor*)hHandle;
        
        if (SENSOR_STATE_UNINITIALISED != sensor->state)
        {
            if(ui8Context == 0)
            {
                sensor->exposure = ui32Exposure;
                return IMG_SUCCESS;
            }
            return IMG_ERROR_NOT_SUPPORTED;
        }
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    
    static IMG_RESULT Fake_GetInfo(SENSOR_HANDLE hHandle, SENSOR_INFO *psInfo)
    {
        FakeSensor *sensor = (FakeSensor*)hHandle;
        
        if (SENSOR_STATE_UNINITIALISED != sensor->state)
        {
            psInfo->eBayerOriginal = MOSAIC_RGGB;
            psInfo->eBayerEnabled = psInfo->eBayerOriginal;
            // when flipping changes the bayer pattern
            if (SENSOR_FLIP_NONE!=psInfo->sStatus.ui8Flipping)
            {
                psInfo->eBayerEnabled = MosaicFlip(psInfo->eBayerOriginal,
                    psInfo->sStatus.ui8Flipping&SENSOR_FLIP_HORIZONTAL?1:0,
                    psInfo->sStatus.ui8Flipping&SENSOR_FLIP_VERTICAL?1:0);
            }
            sprintf(psInfo->pszSensorName, "fake");
            sprintf(psInfo->pszSensorVersion, "1.0.0");
            psInfo->fNumber = 2.0f;
            psInfo->ui16FocalLength = 1;
            psInfo->ui32WellDepth = 16;
            // bitdepth is part of the mode
            psInfo->flReadNoise = 2.0f;
            psInfo->ui8Imager = 1;
            psInfo->bBackFacing = IMG_FALSE;
            // other information is done by Sensor_GetInfo()
            return IMG_SUCCESS;
        }
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    
    static IMG_RESULT Fake_ConfigureFlash(SENSOR_HANDLE hHandle, IMG_BOOL bAlwaysOn, IMG_INT16 i16FrameDelay, IMG_INT16 i16Frames, IMG_UINT16 ui16FlashPulseWidth)
    {
    }
    
    static IMG_RESULT Fake_GetExtendedParamInfo(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Index, char ** ppszParamName, IMG_SIZE *size)
    {
    }
    
    static IMG_RESULT Fake_GetExtendedParamValue(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Index, void *Param, IMG_SIZE size)
    {
    }
    
    static IMG_RESULT Fake_SetExtendedParamValue(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Index, const void *Param, IMG_SIZE size)
    {
    }
    
    static IMG_RESULT Fake_Insert(SENSOR_HANDLE hHandle)
    {
    }
    
    static IMG_RESULT Fake_GetFocusRange(SENSOR_HANDLE hHandle, IMG_UINT16 *pui16Min, IMG_UINT16 *pui16Max)
    {
    }
    
    static IMG_RESULT Fake_GetCurrentFocus(SENSOR_HANDLE hHandle, IMG_UINT16 *pui16Current)
    {
    }
    
    static IMG_RESULT Fake_SetFocus(SENSOR_HANDLE hHandle, IMG_UINT16 ui16Focus)
    {
    }
}

void SensorFake::configureMinimal(FakeSensor &sensor)
{
    Fake_Initialise(&sensor);
    
    sensor.sFunctions.GetMode = Fake_GetMode;
    sensor.sFunctions.GetState = Fake_GetState;
    sensor.sFunctions.SetMode = Fake_SetMode;
    sensor.sFunctions.Enable = Fake_Enable;
    sensor.sFunctions.Disable = Fake_Disable;
    sensor.sFunctions.Destroy = Fake_Destroy;
    sensor.sFunctions.GetGainRange = Fake_GetGainRange;
    sensor.sFunctions.GetCurrentGain = Fake_GetCurrentGain;
    sensor.sFunctions.SetGain = Fake_SetGain;
    sensor.sFunctions.GetExposureRange = Fake_GetExposureRange;
    sensor.sFunctions.GetExposure = Fake_GetExposure;
    sensor.sFunctions.SetExposure = Fake_SetExposure;
    sensor.sFunctions.GetInfo = Fake_GetInfo;
    
    // other functions should be left out
}
    
bool SensorFake::testMinimal(SENSOR_HANDLE sensor)
{
    SENSOR_MODE sModes;
    SENSOR_STATUS sState;
    SENSOR_INFO sInfo;
    bool wasCalled = false;
    
    // first info to get name
    EXPECT_NE(IMG_ERROR_FATAL, Sensor_GetInfo(sensor, &sInfo));
    if (HasFailure()) return wasCalled;
    
    EXPECT_NE(IMG_ERROR_FATAL, Sensor_GetMode(sensor, 0, &sModes))
        << sInfo.pszSensorName;
    if (HasFailure()) return wasCalled;
    
    EXPECT_NE(IMG_ERROR_FATAL, Sensor_GetState(sensor, &sState))
        << sInfo.pszSensorName;
    if (HasFailure()) return wasCalled;
    
    EXPECT_NE(IMG_ERROR_FATAL, Sensor_SetMode(sensor, 0, SENSOR_FLIP_NONE))
        << sInfo.pszSensorName;
    if (HasFailure()) return wasCalled;
    
    EXPECT_NE(IMG_ERROR_FATAL, Sensor_Enable(sensor))
        << sInfo.pszSensorName;
    if (HasFailure()) return wasCalled;
    
    EXPECT_NE(IMG_ERROR_FATAL, Sensor_Disable(sensor))
        << sInfo.pszSensorName;
    if (HasFailure()) return wasCalled;
    
    // called last because it should free the associated memory!
    EXPECT_NE(IMG_ERROR_FATAL, Sensor_Destroy(sensor))
        << sInfo.pszSensorName;
    if (!HasFailure())
    {
        wasCalled = true;
    }
    
    return wasCalled;
}

bool SensorFake::testFlipping(SENSOR_HANDLE sensor, IMG_UINT16 ui16Mode, IMG_UINT8 ui8Flipping)
{
    IMG_RESULT ret;
    SENSOR_MODE sMode;
    SENSOR_INFO sInfo;

    ret = Sensor_GetMode(sensor, ui16Mode, &sMode);
    if(IMG_SUCCESS!=ret)
    {
        ADD_FAILURE() << "GetMode " << ui16Mode << " failed";
        return false;
    }
    EXPECT_EQ(ui8Flipping, (ui8Flipping&sMode.ui8SupportFlipping)) << "selected mode does not support needed flipping";
    if(HasFailure())
    {
        return false;
    }
        
    ret = Sensor_SetMode(sensor, ui16Mode, ui8Flipping);
    if(IMG_SUCCESS!=ret)
    {
        ADD_FAILURE() << "SetMode " << ui16Mode << " & flipping " << (int)ui8Flipping << " failed";
        return false;
    }

    ret = Sensor_GetInfo(sensor, &sInfo);
    if(IMG_SUCCESS!=ret)
    {
        ADD_FAILURE() << "GetInfo failed";
        return false;
    }

    EXPECT_EQ(ui16Mode, sInfo.sStatus.ui16CurrentMode);
    EXPECT_EQ(ui8Flipping, sInfo.sStatus.ui8Flipping);
    EXPECT_EQ(SENSOR_STATE_IDLE, sInfo.sStatus.eState);

    EXPECT_EQ(sMode.ui16Width, sInfo.sMode.ui16Width);
    EXPECT_EQ(sMode.ui16Height, sInfo.sMode.ui16Height);
    EXPECT_EQ(sMode.flFrameRate, sInfo.sMode.flFrameRate);
    EXPECT_EQ(sMode.ui16HorizontalTotal, sInfo.sMode.ui16HorizontalTotal);
    EXPECT_EQ(sMode.ui16VerticalTotal, sInfo.sMode.ui16VerticalTotal);
    EXPECT_EQ(sMode.ui8SupportFlipping, sInfo.sMode.ui8SupportFlipping);

    if(SENSOR_FLIP_NONE==ui8Flipping)
    {
        EXPECT_EQ(sInfo.eBayerOriginal, sInfo.eBayerEnabled) << "no flipping should not affect bayer mosaic!";
    }
    else
    {
        EXPECT_EQ(sInfo.eBayerEnabled, 
            MosaicFlip(sInfo.eBayerOriginal, 
            (ui8Flipping&SENSOR_FLIP_HORIZONTAL)?IMG_TRUE:IMG_FALSE, 
            (ui8Flipping&SENSOR_FLIP_VERTICAL)?IMG_TRUE:IMG_FALSE)
        ) << "wrong flipping bayer info";
    }

    if(!HasFailure())
        return true;
    return false;
}

/**
 * @brief Ensure that if a function is not implemented the sensor API functions
 * realise it an return the correct error code
 */
TEST_F(SensorFake, ensure_implemented)
{
    //configureMinimal();
    FakeSensor sensor;
    SENSOR_MODE sModes;
    SENSOR_STATUS sState;
    SENSOR_INFO sInfo;
    
    Fake_Initialise(&sensor);
    
    ASSERT_EQ(IMG_ERROR_FATAL, Sensor_GetMode(TO_HANDLE(sensor), 0, &sModes));
    ASSERT_EQ(IMG_ERROR_FATAL, Sensor_GetState(TO_HANDLE(sensor), &sState));
    ASSERT_EQ(IMG_ERROR_FATAL, Sensor_SetMode(TO_HANDLE(sensor), 0, SENSOR_FLIP_NONE));
    ASSERT_EQ(IMG_ERROR_FATAL, Sensor_Enable(TO_HANDLE(sensor)));
    ASSERT_EQ(IMG_ERROR_FATAL, Sensor_Disable(TO_HANDLE(sensor)));
    ASSERT_EQ(IMG_ERROR_FATAL, Sensor_Destroy(TO_HANDLE(sensor)));
    ASSERT_EQ(IMG_ERROR_FATAL, Sensor_GetInfo(TO_HANDLE(sensor), &sInfo));
}

/**
 * @brief Ensure that optional functions that are not implemented return the
 * correct error code
 */
TEST_F(SensorFake, ensure_optional)
{
    FakeSensor sensor;
    IMG_SIZE size = 1;
    IMG_UINT16 fmin = 0, fmax = 250;
    
    configureMinimal(sensor);
    
    testMinimal(TO_HANDLE(sensor));
    
    ASSERT_EQ(IMG_ERROR_NOT_SUPPORTED, Sensor_GetFocusRange(TO_HANDLE(sensor), &fmin, &fmax));
    ASSERT_EQ(IMG_ERROR_NOT_SUPPORTED, Sensor_GetCurrentFocus(TO_HANDLE(sensor), &fmin));
    ASSERT_EQ(IMG_ERROR_NOT_SUPPORTED, Sensor_SetFocus(TO_HANDLE(sensor), fmin));
}

/**
 * @brief Test that functions return correct invalid code when expecting pointers
 */
TEST_F(SensorFake, testInvalidCalls)
{
    FakeSensor sensor;
    SENSOR_MODE sModes;
    SENSOR_STATUS sState;
    SENSOR_INFO sInfo;
    IMG_SIZE size=1;
    IMG_UINT32 ui32Value;
    IMG_UINT16 ui16Value;
    IMG_UINT8 ui8Value;
    double fValue;
    
    configureMinimal(sensor);
    
    // GetMode
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetMode(NULL, 0, NULL));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetMode(NULL, 0, &sModes));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetMode(TO_HANDLE(sensor), 0, NULL));
    
    // GetState
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetState(NULL, NULL));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetState(NULL, &sState));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetState(TO_HANDLE(sensor), NULL));
    
    // SetMode
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_SetMode(NULL, 0, SENSOR_FLIP_NONE));
    EXPECT_EQ(IMG_ERROR_VALUE_OUT_OF_RANGE, Sensor_SetMode(TO_HANDLE(sensor), 2, SENSOR_FLIP_NONE));
    EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, Sensor_SetMode(TO_HANDLE(sensor), 0, SENSOR_FLIP_HORIZONTAL)) << "this mode should not support flipping";
    
    // Enable
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_Enable(NULL));
    
    // Disable
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_Disable(NULL));
    
    // Destroy
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_Destroy(NULL));
    
    // GetInfo
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetInfo(NULL, NULL));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetInfo(NULL, &sInfo));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetInfo(TO_HANDLE(sensor), NULL));
    
    // GetFocusRange
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetFocusRange(NULL, NULL, NULL));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetFocusRange(TO_HANDLE(sensor), &ui16Value, NULL));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetFocusRange(TO_HANDLE(sensor), NULL, &ui16Value));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetFocusRange(NULL, &ui16Value, &ui16Value));
    
    // SetFocus
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_SetFocus(NULL, ui16Value));
    
    // GetGainRange
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetGainRange(NULL, NULL, NULL, NULL));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetGainRange(TO_HANDLE(sensor), &fValue, &fValue, NULL));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetGainRange(TO_HANDLE(sensor), NULL, &fValue, &ui8Value));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetGainRange(TO_HANDLE(sensor), &fValue, NULL, &ui8Value));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetGainRange(NULL, &fValue, &fValue, &ui8Value));
    
    // GetCurrentGain
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetCurrentGain(NULL, NULL, 0));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetCurrentGain(TO_HANDLE(sensor), NULL, 0));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetCurrentGain(NULL, &fValue, 0));
    
    // SetGain
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_SetGain(NULL, 1.0f, 0));
    
    // GetExposureRange
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetExposureRange(NULL, NULL, NULL, NULL));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetExposureRange(TO_HANDLE(sensor), &ui32Value, &ui32Value, NULL));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetExposureRange(TO_HANDLE(sensor), NULL, &ui32Value, &ui8Value));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetExposureRange(TO_HANDLE(sensor), &ui32Value, NULL, &ui8Value));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetExposureRange(NULL, &ui32Value, &ui32Value, &ui8Value));
    
    // GetExposure
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetExposure(NULL, NULL, 0));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetExposure(TO_HANDLE(sensor), NULL, 0));
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_GetExposure(NULL, &ui32Value, 0));
    
    // SetExposure
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_SetGain(NULL, 10000, 0));
    
    // Insert
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_Insert(NULL));
    
    // ConfigureFlash
    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, Sensor_ConfigureFlash(NULL, IMG_FALSE, 2, 5, 16));
}

/**
 * @brief Test Sensor_GetInfo() gets all information
 */
TEST_F(SensorFake, GetInfo)
{
    FakeSensor sensor;

    configureMinimal(sensor);

    ASSERT_TRUE(testFlipping(TO_HANDLE(sensor), 0, SENSOR_FLIP_NONE)) << "mode 0 & no flip";
    
    ASSERT_TRUE(testFlipping(TO_HANDLE(sensor), 1, SENSOR_FLIP_NONE)) << "mode 1 & no flip";

    ASSERT_TRUE(testFlipping(TO_HANDLE(sensor), 1, SENSOR_FLIP_VERTICAL)) << "mode 1 & vertical flip";

    ASSERT_TRUE(testFlipping(TO_HANDLE(sensor), 1, SENSOR_FLIP_HORIZONTAL)) << "mode 1 & horizontal flip";

    ASSERT_TRUE(testFlipping(TO_HANDLE(sensor), 1, SENSOR_FLIP_BOTH)) << "mode 1 & both flip";
}

/// @ implement all the unit tests that check the ranges and status once it has been decided
