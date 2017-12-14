/**
******************************************************************************
@file HwCaps.h

@brief Declaration of HW capabilities in v2500 HAL module

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

#ifndef HW_CAPS_H
#define HW_CAPS_H

#include <string>
#include <utils/RefBase.h>
#include "hardware/camera_common.h"
#include "sensorapi/sensorapi.h"

namespace android {

class HwManager;

#define NSEC 1LL
#define USEC (NSEC * 1000LL)
#define MSEC (USEC * 1000LL)
#define SEC (MSEC * 1000LL)

// ------------------------------------------------------------------------
// Static configuration information
// ------------------------------------------------------------------------
typedef struct SensorResCaps_t {
    int32_t  width;       ///<@brief supported width available in sensor mode
    int32_t  height;      ///<@brief supported height available in sensor mode
    uint32_t sensorMode; ///<@brief sensor mode
    int64_t minDurationNs; ///<@brief min duration of frame reprocessing
} SensorResCaps_t;

class HwCaps : public RefBase {
public:
    HwCaps(const std::string& sensorName, const uint8_t sensorMode,
            HwManager& hwMgr);
    virtual ~HwCaps(){}

    float const *sensorPhysicalSize;

    // named indexes for maxNumOutputStreams
    enum {
        RAW = 0, NONSTALLING, STALLING
    };
    static const int32_t  maxNumOutputStreams[3];
    static const int32_t  kAvailableFormats[];
    static const size_t   kNumAvailableFormats;

    SensorResCaps_t const *kAvailableProcessedImageParams;
    SensorResCaps_t const *kAvailableRawImageParams;
    SensorResCaps_t const *kAvailableJpegImageParams;

    size_t   kNumAvailableProcessedParams;
    size_t   kNumAvailableRawParams;
    size_t   kNumAvailableJpegParams;

    // Maximum difference between target and metered brightness when auto
    // exposure mode is set
    static const double kBrightnessTreshold;
    static const float kNormalGain;

    HwManager&  mHwManager;
};

} // namespace android

#endif // FELIX_CAMERA_H
