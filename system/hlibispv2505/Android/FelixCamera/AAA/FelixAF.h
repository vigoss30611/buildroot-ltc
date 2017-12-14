/**
******************************************************************************
@file FelixAF.h

@brief Interface for AF v2500 HAL module

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

#ifndef FELIX_AF_H
#define FELIX_AF_H

#include "Region.h"
#include "FelixAAA.h"
#include "ispc/Camera.h"
#include "ispc/Average.h"
#include "ispc/ControlAF.h"
#include <system/camera_metadata.h>
/**
 * @brief defines absolute minimum sharpness below of which lens is reported
 * as out of focus in single shot mode
 * FIXME: provide better means for detection of out of focus
 */
#define OUT_OF_FOCUS_MIN_SHARPNESS 20000.0

/**
 * @brief number of steps in rough mode
 */
#ifdef FELIX_HAS_DG
#define AF_ROUGH_STEPS 2
#else
#define AF_ROUGH_STEPS 10
#endif

/**
 * @brief number of steps in fine focusing
 *
 * @note can't be too high because minimum step in focusDistance may be
 * less than 1 near minFocusDistance
 *
 */
#ifdef FELIX_HAS_DG
#define AF_FINE_STEPS 2
#else
#define AF_FINE_STEPS 5
#endif

#define MAVG_WINDOW 32

namespace android {

class FelixMetadata;

class sAfRegion : public sRegion {
public:
    double sharpness;

    sAfRegion& operator=(const sAfRegion& rsh) {
        sRegion::operator=(rsh);
        sharpness = rsh.sharpness;
        return *this;
    }
    sAfRegion(const uint32_t x, const uint32_t y,
              const uint32_t w, const uint32_t h) :
        sRegion(x, y, w, h), sharpness(0.0){}

    /**
     * @brief constructs region using raw CameraMetadata field and
     * width & height
     */
    sAfRegion(const rawRegion_t data) :
        sRegion(data), sharpness(0.0) {}

    /**
     * copy constructor
     */
    sAfRegion(const sAfRegion& reg) :
        sRegion(reg), sharpness(reg.sharpness) {}

    /**
     * empty region
     */
    sAfRegion(void) : sRegion(), sharpness(0.0) {};
};

/**
 * @class FelixAF
 * @brief Autofocus control module for ISPC in Android HAL
 *
 * @note This class does not support concurrency so processHALMetadata(),
 * update() and updateHALMetadata() MUST be called from single thread.
 */
typedef RegionHandler<sAfRegion> AfRegionHandler;

class FelixAF:
    public ISPC::ControlAF,
    public AAAHalInterface,
    public AfRegionHandler {

public:
    FelixAF() : mAfScanMode(AF_MODE_SINGLE_SHOT),
                mAfMode(ANDROID_CONTROL_AF_MODE_OFF),
                mAfState(ANDROID_CONTROL_AF_STATE_INACTIVE),
                mAfTrigger(ANDROID_CONTROL_AF_TRIGGER_IDLE),
                mAfTriggerId(0),
                targetFocusDiopter(0.0),
                scanInitDiopter(0.0),
                scanEndDiopter(0.0),
                refineSharpnessThreshold(0.0),
                roughScanStep(0.0),
                fineScanStep(0.0),
                average(MAVG_WINDOW) {}
    virtual ~FelixAF(){}

    /* implementations of AAAHalInterface pure virtual methods */
    status_t initialize(void);
    status_t processDeferredHALMetadata(FelixMetadata &settings);
    status_t processUrgentHALMetadata(FelixMetadata &settings);
    status_t updateHALMetadata(FelixMetadata &settings);

    static void advertiseCapabilities(CameraMetadata &info);
    status_t initRequestMetadata(FelixMetadata &settings, int type);

private:
    /* implementations of RegionHandler pure virtual methods */
    double weight(int32_t indexH, int32_t indexV);
    status_t configureRoiMeteringMode(const region_t& region);
    status_t configureDefaultMeteringMode(void);

    /**
     * @brief update method
     * @note reimplements android AF state machine
     */
    virtual IMG_RESULT update(const ISPC::Metadata &metadata);
    /**
     * @brief calcuates weighted average sharpness or best sharpness across
     * specific regions depending on mAutoRegion value
     */
    double calculateRegionSharpness(const ISPC::Metadata &metadata);

    /**
     * @brief reimplements internal AF state machine
     */
    void runAF(unsigned int lastFocusDistance, double sharpness,
        ISPC::ControlAF::Command command);

    /**
     * @brief helper for clipping scan limits
     */
    void clipScanLimits(void);

    typedef camera_metadata_enum_android_control_mode_t
        control_mode_t;
    typedef camera_metadata_enum_android_control_af_mode_t
        control_af_mode_t;
    typedef camera_metadata_enum_android_control_af_state_t
        control_af_state_t;
    typedef camera_metadata_enum_android_control_af_trigger_t
        control_af_trigger_t;

    /**
     * @brief scanning mode enumeration
     */
    typedef enum {
        AF_MODE_SINGLE_SHOT,
        AF_MODE_CONTINUOUS
    } af_mode_t;

    /**
     * @brief current scan mode (single shot / continuous)
     */
    af_mode_t mAfScanMode;

    control_mode_t    m3aMode;        ///<@brief AAA master mode
    control_af_mode_t mAfMode;        ///<@brief Android AF mode
    control_af_state_t mAfState;      ///<@brief Android AF state
    control_af_trigger_t mAfTrigger;  ///<@brief Android AF trigger type
    int mAfTriggerId;                 ///<@brief Android AF trigger number

    //double sharpness;
    double minSharpness;       ///<@brief Worse sharpness measure found so far
    double bestFocusDiopter;   ///<@brief Best focus distance found so far
    double minFocusDiopter;    ///<@brief Minimum focus (lens dependent)
    double maxFocusDiopter;    ///<@brief Maximum focus (lens dependent)
    double targetFocusDiopter; ///<@brief Position to be set in current iteration
    double scanInitDiopter;    ///<@brief Init position for the scan
    double scanEndDiopter;     ///<@brief End position for the scan

    /**
     * @brief sharpness threshold for entering refine state
     */
    double refineSharpnessThreshold;
    double roughScanStep; ///<@brief scan step in rough mode
    double fineScanStep;  ///<@brief scan step in fine mode
    /**
     * @brief reciprocal factor in conversion from distance->diopter
     */
    static const double RECIP;

    ISPC::mavg<double> average; ///<@brief average sharpness
}; //class FelixAF

} //namespace android

#endif /*FELIX_AF_H*/
