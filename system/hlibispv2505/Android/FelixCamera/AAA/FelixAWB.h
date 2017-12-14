/**
******************************************************************************
@file FelixAWB.h

@brief Interface for AWB v2500 HAL module

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

#ifndef FELIX_AWB_H
#define FELIX_AWB_H

#include "Region.h"
#include "FelixAAA.h"
#include "ispc/Camera.h"
#include "Controls/ispc/ControlAWB_PID.h"

#ifdef FELIX_HAS_DG
//android awb state goes quickly to state CONVERGED and doesn't care
//about real measurements
#define FELIX_AWB_ERROR_MARGIN (500)
#else
#define FELIX_AWB_ERROR_MARGIN (100)
#endif

namespace android {

class FelixMetadata;

class FelixAWB:
    public ISPC::ControlAWB_PID,
    public AAAHalInterface,
    public RegionHandler<> {

public:
    FelixAWB() :
        ControlAWB_PID(),

        mAwbLocked(false),
        mMode(ANDROID_CONTROL_AWB_MODE_OFF),
        mState(ANDROID_CONTROL_AWB_STATE_INACTIVE),
        mNextState(ANDROID_CONTROL_AWB_STATE_INACTIVE) {};
    virtual ~FelixAWB(){}

    /* implementations of AAAHalInterface pure virtual methods */
    status_t initialize(void);
    status_t processDeferredHALMetadata(FelixMetadata &settings);
    status_t processUrgentHALMetadata(FelixMetadata &settings);
    status_t updateHALMetadata(FelixMetadata &settings);
    static void advertiseCapabilities(CameraMetadata &setting);
    status_t initRequestMetadata(FelixMetadata &settings, int type);

private:
    /* implementations of RegionHandler pure virtual methods */
    double weight(int32_t indexH, int32_t indexV);
    status_t configureRoiMeteringMode(const region_t& region);
    status_t configureDefaultMeteringMode(void);

    typedef camera_metadata_enum_android_control_awb_mode_t
        control_awb_mode_t;
    typedef camera_metadata_enum_android_control_awb_state_t
        control_awb_state_t;

    bool mAwbLocked;
    bool bConverged;

    control_awb_mode_t  mMode;
    control_awb_state_t mState;
    control_awb_state_t mNextState;
}; /*class FelixAWB*/

} /*namespace android*/

#endif /*FELIX_AWB_H*/
