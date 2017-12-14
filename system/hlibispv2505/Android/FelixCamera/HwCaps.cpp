/**
******************************************************************************
@file HwCaps.cpp

@brief Definition of HwCaps class in v2500 HAL module

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

*****************************************************************************/

#define LOG_TAG "FelixHwCaps"
#include "felixcommon/userlog.h"

// HAL includes
#include "HwCaps.h"
#include "Helpers.h"
#include "HwManager.h"

#include "sensorapi/sensorapi.h"
#include "sensors/ov4688.h"
#include "sensors/ar330.h"

namespace android {

//
// Constants for camera capabilities
//

// static global HAL configuration parameters

static const float sensorPhysicalSize_OV4688[2] = { 3.20f, 2.40f }; // mm
static const float sensorPhysicalSize_AR330[2] = { 3.20f, 2.40f }; // mm

const double  HwCaps::kBrightnessTreshold       = 0.2;
const float   HwCaps::kNormalGain               = 1.0;

// in order of appearance :
   // - RAW streams
   // - processed uncompressed (>= 3)
   // - JPEG streams
const int32_t HwCaps::maxNumOutputStreams[3] = {0, 3, 1};

// Available formats
const int32_t HwCaps::kAvailableFormats[] = {
    ANDROID_SCALER_AVAILABLE_FORMATS_IMPLEMENTATION_DEFINED,
    ANDROID_SCALER_AVAILABLE_FORMATS_BLOB,
    // Display formats
    //HAL_PIXEL_FORMAT_RAW_SENSOR,
    //HAL_PIXEL_FORMAT_RGB_888, - disabled due to problems with stride
    HAL_PIXEL_FORMAT_RGBA_8888,
    HAL_PIXEL_FORMAT_RGBX_8888,
    //HAL_PIXEL_FORMAT_BGRA_8888, - disabled due to Lollipop CTS
    // java.lang.IllegalArgumentException: format 0x5 was not defined in either ImageFormat or PixelFormat
#ifdef USE_IMG_SPECIFIC_PIXEL_FORMATS
    HAL_PIXEL_FORMAT_RGBX_101010,
    // Encoder formats
    HAL_IMG_FormatYUV420PP, // Packed planar.
    HAL_IMG_FormatYUV420SP, // Packed semi-planar
#endif
    //HAL_PIXEL_FORMAT_YCrCb_420_SP, - disabled due to Lollipop CTS
    // junit.framework.AssertionFailedError: NV21 must not be supported
    ANDROID_SCALER_AVAILABLE_FORMATS_YCbCr_420_888, // flexible 420 yuv, required
    HAL_PIXEL_FORMAT_YCbCr_422_SP
};

const size_t HwCaps::kNumAvailableFormats =
        ARRAY_NUM_ELEMENTS(HwCaps::kAvailableFormats);

static const SensorResCaps_t kAvailableProcessedImageParams_OV4688[] = {
    { 1280, 720, 0, 0}, // 16:9, 1 mipi lane,
    { 960,  720, 0, 0}, // 4:3, 1 mipi lane,
    { 800,  480, 0, 0}, // 5:3, 1 mipi lane,
    { 768,  576, 0, 0}, // 4:3, 1 mipi lane,
    { 720,  576, 0, 0}, // 5:4, 1 mipi lane,
    { 720,  480, 0, 0}, // 3:2, 1 mipi lane,
    { 640,  480, 0, 0}, // 4:3, 1 mipi lane,
    { 352,  288, 0, 0}, // 11:9, 1 mipi lane,
    { 320,  240, 0, 0}, // 4:3, 1 mipi lane,
    { 240,  160, 0, 0}, // 3:2, 1 mipi lane,
    { 176,  144, 0, 0}, // 6:5, 1 mipi lane,
    { 128,   96, 0, 0}, // 4:3, 1 mipi lane,
};

static const SensorResCaps_t kAvailableRawImageParams_OV4688[] = {
    { 1280,  720, 0, 0 }, // 16:9, 15fps, 1 mipi lane,
};

static const SensorResCaps_t kAvailableJpegImageParams_OV4688[] = {
    { 1280,  720, 0, 500*MSEC }, // 16:9, 0.5sec jpeg processing time in SW mode
    { 640,   480, 0, 500*MSEC }, // 4:3
    { 320,   240, 0, 500*MSEC }  // 4:3
};

static const SensorResCaps_t kAvailableProcessedImageParams_AR330[] = {
#if(0)
        // : enable when supported in HW
    { 1920, 1080, 3, 0 }, // 2 lane, 30fps
    { 1440, 1080, 3, 0 }, // 2 lane, 30fps
    { 1440,  960, 3, 0 }, // 2 lane, 30fps
    { 1280, 1024, 3, 0 }, // 2 lane, 30fps
#endif
    { 1280,  720, 1, 0 },// 1 lane, 30fps
    { 960,  720, 1, 0 },// 1 lane, 30fps
    { 800,  480, 1, 0 },// 1 lane, 30fps
    { 768,  576, 1, 0 },// 1 lane, 30fps
    { 720,  576, 1, 0 },// 1 lane, 30fps
    { 720,  480, 1, 0 },// 1 lane, 30fps
    { 640,  480, 1, 0 },// 1 lane, 30fps
    { 352,  288, 1, 0 },// 1 lane, 30fps
    { 320,  240, 1, 0 },// 1 lane, 30fps
    { 240,  160, 1, 0 },// 1 lane, 30fps
    { 176,  144, 1, 0 },// 1 lane, 30fps
    { 128,   96, 1, 0 }, // 1 lane, 30fps
};

static const SensorResCaps_t kAvailableRawImageParams_AR330[] = {
    { 1280,  720, 1, 0 },// 1 lane, 30fps
};

static const SensorResCaps_t kAvailableJpegImageParams_AR330[] = {
#if(0)
    { 1920, 1080, 3, 500*MSEC }, // 2 lane, 30fps
#endif
    { 1280,  720, 0, 500*MSEC }, // 0.5sec jpeg processing time in SW mode
    { 640,   480, 0, 500*MSEC },
    { 320,   240, 0, 500*MSEC }
};

HwCaps::HwCaps(const std::string& sensorName,
        __maybe_unused const uint8_t sensorMode,
        HwManager& hwMgr) : mHwManager(hwMgr) {
    ALOG_ASSERT(sensorName.length());

    if(sensorName==OV4688_SENSOR_INFO_NAME) {
        kAvailableProcessedImageParams = kAvailableProcessedImageParams_OV4688;
        kNumAvailableProcessedParams =
            ARRAY_NUM_ELEMENTS(kAvailableProcessedImageParams_OV4688);
        kAvailableRawImageParams = kAvailableProcessedImageParams_OV4688;
        kNumAvailableRawParams =
            ARRAY_NUM_ELEMENTS(kAvailableRawImageParams_OV4688);
        kAvailableJpegImageParams = kAvailableJpegImageParams_OV4688;
        kNumAvailableJpegParams =
            ARRAY_NUM_ELEMENTS(kAvailableJpegImageParams_OV4688);
        sensorPhysicalSize = sensorPhysicalSize_OV4688;
    } else if (sensorName==AR330_SENSOR_INFO_NAME) {
        kAvailableProcessedImageParams = kAvailableProcessedImageParams_AR330;
        kNumAvailableProcessedParams =
            ARRAY_NUM_ELEMENTS(kAvailableProcessedImageParams_AR330);
        kAvailableRawImageParams = kAvailableProcessedImageParams_AR330;
        kNumAvailableRawParams =
            ARRAY_NUM_ELEMENTS(kAvailableRawImageParams_AR330);
        kAvailableJpegImageParams = kAvailableJpegImageParams_AR330;
        kNumAvailableJpegParams =
            ARRAY_NUM_ELEMENTS(kAvailableJpegImageParams_AR330);
        sensorPhysicalSize = sensorPhysicalSize_AR330;
    } else {
        ALOG_ASSERT(false, "Sensor not supported!");
    }
}

}; // namespace android
