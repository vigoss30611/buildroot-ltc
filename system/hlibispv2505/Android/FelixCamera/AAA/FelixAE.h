/**
******************************************************************************
@file FelixAE.h

@brief Interface for AE v2500 HAL module

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

#ifndef FELIX_AE_H
#define FELIX_AE_H

#include "Region.h"
#include "FelixAAA.h"
#include "ispc/Camera.h"
#include "Controls/ispc/ControlAE.h"
#include "Modules/ispc/ModuleFLD.h"

#ifdef FELIX_HAS_DG
//tolerance margin value
#define FELIX_AE_TARGET_BRACKET (0.53)
#else
#define FELIX_AE_TARGET_BRACKET (0.1)
#endif

//within this range <-0.59 ; 0.59> exposure time doubles/decrease
//for single EV change
#define FELIX_AE_BRIGHTNESS_RANGE (0.59)

namespace android {

class HwManager;
class FelixMetadata;

class FelixAE: public ISPC::ControlAE,
    public AAAHalInterface,
    public RegionHandler<> {

public:
    FelixAE();
    virtual ~FelixAE(){}

    /* implementations of AAAHalInterface pure virtual methods */
    status_t initialize(void);

    status_t processAeStateInput();
    status_t processAeStateOutput();

    status_t processDeferredHALMetadata(FelixMetadata &settings);
    status_t processUrgentHALMetadata(FelixMetadata &settings);
    status_t updateHALMetadata(FelixMetadata &settings);

    static void advertiseCapabilities(CameraMetadata &setting,
                    HwManager& hwManager);
    status_t initRequestMetadata(FelixMetadata &settings, int type);

    /**
     * @brief dummy implementation of precapture
     * @note finished the process immediately
     */
    status_t doPrecapture() { mPrecaptureFinished = true; return OK; }
private:
    /* implementations of RegionHandler pure virtual methods */
    double weight(int32_t indexH, int32_t indexV);
    status_t configureRoiMeteringMode(const region_t& region);
    status_t configureDefaultMeteringMode(void);

    typedef camera_metadata_enum_android_control_ae_mode_t
        control_ae_mode_t;
    typedef camera_metadata_enum_android_control_ae_state_t
        control_ae_state_t;
    typedef camera_metadata_enum_android_control_ae_antibanding_mode_t
        control_ae_ab_mode_t;

    bool mAeLocked;

    control_ae_mode_t mMode;
    control_ae_state_t mState;
    control_ae_ab_mode_t mAbMode;

    int32_t mAeTriggerId;
    bool mPrecaptureStarted;
    bool mPrecaptureFinished;

    nsecs_t mExposureTime;
    int32_t mISOSensitivity;
    int32_t mAeCompensation;
    double mTargetBrightness;
    static const uint8_t kCompensationStep = 2; //denominator value
    static const uint8_t kCompensationRange = 4; // <-4;4>

}; /*class FelixAWB*/
} /*namespace android*/

#endif /*FELIX_AE_H*/
