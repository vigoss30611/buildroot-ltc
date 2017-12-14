/**
******************************************************************************
@file FelixAE.cpp

@brief Definition of AE v2500 HAL module

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

#define LOG_TAG "FELIX_AE"
#undef BE_VERBOSE

#include <felixcommon/userlog.h>
#include "Controls/ispc/ControlAE.h"
#include "ispc/ModuleHIS.h"
#include "ispc/ModuleFLD.h"
#include "FelixMetadata.h"
#include "FelixAE.h"
#include "Helpers.h"

#undef PRINT_ARRAYS

using namespace ISPC;

namespace { //init values

char metadataStr[100] = { 0 };

} //anonymous namespace

namespace android {

// : add initializer list
FelixAE::FelixAE() : ControlAE(LOG_TAG), RegionHandler<>(),
        mAeLocked(false),
        mMode(ANDROID_CONTROL_AE_MODE_OFF),
        mState(ANDROID_CONTROL_AE_STATE_INACTIVE),
        mAeTriggerId(0),
        mPrecaptureStarted(false),
        mPrecaptureFinished(false),
        mExposureTime(1000), //FIXME
        mISOSensitivity(100), //FIXME
        mAeCompensation(0)
{
#if(0)
    mMainRegShiftVector[0]=HIS_REGION_VTILES/2;
    mMainRegShiftVector[1]=HIS_REGION_HTILES/2;
    for (int i=0; i<5; i++) mOldChosenRegion[i] = 0;
#endif
}

double FelixAE::weight(int32_t, int32_t){ return 0; }

status_t FelixAE::configureRoiMeteringMode(const region_t& region) {
    ModuleHIS *his = 0;
    if(!getPipelineOwner()) {
        LOG_ERROR("getPipelineOwner() failed");
        return IMG_ERROR_FATAL;
    }
    his = getPipelineOwner()->getModule<ModuleHIS>();
    if(!his) {
        LOG_ERROR("get ModuleHIS failed");
        return IMG_ERROR_FATAL;
    }

    //Program region 1
    his->aGridStartCoord[0] = region.left;
    his->aGridStartCoord[1] = region.top;
    his->aGridTileSize[0] = (int)floor((region.getWidth())/7.0);
    his->aGridTileSize[1] = (int)floor((region.getHeight())/7.0);
    his->bEnableGlobal = true;
    his->bEnableROI = true;

    his->requestUpdate();

    return IMG_SUCCESS;
}

status_t FelixAE::configureDefaultMeteringMode(void) {
    if(getPipelineOwner()) {
        ModuleHIS *his = getPipelineOwner()->getModule<ModuleHIS>();
        if(his) {
            his->bEnableGlobal = false;
            his->bEnableROI = false;
            configureStatistics();
        }
    }
    return IMG_SUCCESS;
}

status_t FelixAE::initRequestMetadata(FelixMetadata &settings,
        int type) {

    const uint8_t aeMode = (type != CAMERA3_TEMPLATE_MANUAL) ?
            ANDROID_CONTROL_AE_MODE_ON :
            ANDROID_CONTROL_AE_MODE_OFF;
    settings.update(ANDROID_CONTROL_AE_MODE, &aeMode, 1);

    const uint8_t aeLock = ANDROID_CONTROL_AE_LOCK_OFF;
    settings.update(ANDROID_CONTROL_AE_LOCK, &aeLock, 1);

    const int32_t aeExpCompensation = 0;
    settings.update(ANDROID_CONTROL_AE_EXPOSURE_COMPENSATION,
            &aeExpCompensation, 1);

    // : make use of this value in AE algorithm
    const int32_t fixedFps =
            ceil(getPipelineOwner()->getSensor()->flFrameRate);
    const int32_t aeTargetFpsRange[] = { fixedFps, fixedFps};
    settings.update(ANDROID_CONTROL_AE_TARGET_FPS_RANGE,
            aeTargetFpsRange,
            ARRAY_NUM_ELEMENTS(aeTargetFpsRange));

    const uint8_t aeAntibandingMode =
        ANDROID_CONTROL_AE_ANTIBANDING_MODE_AUTO;
    settings.update(ANDROID_CONTROL_AE_ANTIBANDING_MODE,
            &aeAntibandingMode, 1);

    const uint8_t aePrecaptureTrigger =
            ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_IDLE;
    settings.update(ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER,
            &aePrecaptureTrigger, 1);

    const int32_t aePrecaptureId =
            ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_IDLE;
    settings.update(ANDROID_CONTROL_AE_PRECAPTURE_ID,
            &aePrecaptureId, 1);

    return settings.updateMetadataWithRegions<FelixAE>(
        ANDROID_CONTROL_AE_REGIONS, mDefaultRegions);
}

status_t FelixAE::updateHALMetadata(FelixMetadata &settings) {
    const Sensor *sensor = 0;
    if(!getPipelineOwner()) {
        LOG_ERROR("getPipelineOwner() failed");
        return IMG_ERROR_FATAL;
    }
    sensor = getPipelineOwner()->getSensor();
    if(!sensor) {
        LOG_ERROR("getSensor() from pipeline owner failed");
        return IMG_ERROR_FATAL;
    }
    mExposureTime = sensor->getExposure() * 1000; //[ns]
    //mISOSensitivity = transform2ISO(sensor->getGain());//FIXME
    settings.update(ANDROID_SENSOR_EXPOSURE_TIME, &mExposureTime, 1);
    settings.update(ANDROID_SENSOR_SENSITIVITY, &mISOSensitivity, 1);

    processAeStateOutput();
    settings.update(ANDROID_CONTROL_AE_STATE, (uint8_t *) &mState, 1);

    // : Trigger IDs need a think-through
    //settings.update(ANDROID_CONTROL_AE_PRECAPTURE_ID, &mAeTriggerId, 1);

    const uint8_t flashState = ANDROID_FLASH_STATE_UNAVAILABLE;
    settings.update(ANDROID_FLASH_STATE, &flashState, 1);

    if(mCurrentRegions) {
        settings.updateMetadataWithRegions<FelixAE>(
            ANDROID_CONTROL_AE_REGIONS, *mCurrentRegions);
    }

    camera_metadata_enum_snprint(ANDROID_CONTROL_AE_STATE, mState,
            metadataStr, sizeof(metadataStr));
    LOG_DEBUG("AE state is %s (%d)", metadataStr, mState);
    LOG_DEBUG("AE internal flicker state %s %s %fHz",
            getFlickerRejection() ? "ON" : "",
            getAutoFlickerRejection() ? "(auto)" : "",
            getFlickerRejectionFrequency());
    return OK;
}

// called before acquireShot() because it makes decision about doAE
status_t FelixAE::processAeStateInput() {

    doAE = true;

    // override only if precapture not active
    if (mAeLocked && !mPrecaptureStarted) {
        mState = ANDROID_CONTROL_AE_STATE_LOCKED;
        doAE = false;
        return OK;
    }
    switch(mState) {
    case ANDROID_CONTROL_AE_STATE_INACTIVE:
    case ANDROID_CONTROL_AE_STATE_FLASH_REQUIRED:
    case ANDROID_CONTROL_AE_STATE_CONVERGED:
    case ANDROID_CONTROL_AE_STATE_LOCKED:
    case ANDROID_CONTROL_AE_STATE_PRECAPTURE:
        // would change eventually after metering
        break;

    case ANDROID_CONTROL_AE_STATE_SEARCHING:
#ifdef DUMMY_AE
        mState = ANDROID_CONTROL_AE_STATE_CONVERGED;
        doAE = false;
#endif
        break;
    default:
        // possible change after update
        break;
    }
    return OK;
}

// called in update()
status_t FelixAE::processAeStateOutput() {

    switch(mState) {
    case ANDROID_CONTROL_AE_STATE_INACTIVE:
    case ANDROID_CONTROL_AE_STATE_FLASH_REQUIRED:
        // switch to SEARCHING automatically
        mState = ANDROID_CONTROL_AE_STATE_SEARCHING;
        break;

    case ANDROID_CONTROL_AE_STATE_SEARCHING:
        if (hasConverged()) {
            mState = ANDROID_CONTROL_AE_STATE_CONVERGED;
        } else if(isUnderexposed()) {
            mState = ANDROID_CONTROL_AE_STATE_FLASH_REQUIRED;
        }
        break;

    case ANDROID_CONTROL_AE_STATE_CONVERGED:
#ifndef DUMMY_AE
        if (!hasConverged()) {
            mState = ANDROID_CONTROL_AE_STATE_SEARCHING;
        }
#endif /*DUMMY_AE*/
        break;

    case ANDROID_CONTROL_AE_STATE_LOCKED:
        if(mAeLocked) {
            break;
        }
        // change only if switched to unlocked
        if(isUnderexposed()){
            // converged with bad conditions
            mState = ANDROID_CONTROL_AE_STATE_FLASH_REQUIRED;
        } else if (hasConverged()) {
            // converged with good conditions
            mState = ANDROID_CONTROL_AE_STATE_CONVERGED;
        } else {
            mState = ANDROID_CONTROL_AE_STATE_SEARCHING;
        }
        break;

    case ANDROID_CONTROL_AE_STATE_PRECAPTURE:
        doPrecapture();

        // mPrecaptureFinished is set internally in doPrecapture()
        if (!mPrecaptureFinished) {
            break;
        }
        if(mAeLocked) {
            mState = ANDROID_CONTROL_AE_STATE_LOCKED;
        } else if (hasConverged()) {
            mState = ANDROID_CONTROL_AE_STATE_CONVERGED;
        } else {
            mState = ANDROID_CONTROL_AE_STATE_PRECAPTURE;
        }
        mPrecaptureStarted = false;
        mPrecaptureFinished = false;
        break;

    default:
        break;
    }

    return OK;
}

status_t FelixAE::processDeferredHALMetadata(FelixMetadata &settings) {
    LOG_DEBUG("E");
    camera_metadata_entry e;

    //if CONTROL_MODE == OFF, inactivate AE
    e = settings.find(ANDROID_CONTROL_MODE);
    if (e.count == 0) {
        LOG_ERROR("No control mode entry!");
        return BAD_VALUE;
    }
    uint8_t controlMode = e.data.u8[0];
    camera_metadata_enum_snprint(ANDROID_CONTROL_MODE, controlMode,
            metadataStr, sizeof(metadataStr));
    LOG_DEBUG("ANDROID_CONTROL_MODE mode is %s", metadataStr);

    if (controlMode == ANDROID_CONTROL_MODE_OFF){
        mState = ANDROID_CONTROL_AE_STATE_INACTIVE;
        doAE = false;
        return OK;
    }

    //scene modes, except FACE_PRIO, should inactivate AE
    if (controlMode == ANDROID_CONTROL_MODE_USE_SCENE_MODE) {
        e = settings.find(ANDROID_CONTROL_SCENE_MODE);
        if (e.count == 0) {
            LOG_ERROR("No scene mode defined!");
            return BAD_VALUE;
        }
        if (e.count != 0) {
            uint8_t sceneMode = e.data.u8[0];
            camera_metadata_enum_snprint(ANDROID_CONTROL_SCENE_MODE, sceneMode,
                metadataStr, sizeof(metadataStr));
            LOG_DEBUG("ANDROID_CONTROL_SCENE_MODE is %s", metadataStr);
            if (sceneMode != ANDROID_CONTROL_SCENE_MODE_FACE_PRIORITY) {
                mState = ANDROID_CONTROL_AE_STATE_INACTIVE;
                doAE = false;
                return OK;
            }
        }
    }

    e = settings.find(ANDROID_CONTROL_AE_MODE);
    if (e.count == 0) {
        LOG_ERROR("No AE mode entry!");
        return BAD_VALUE;
    }
    mMode = (control_ae_mode_t) e.data.u8[0];

    //user locked AE?
    e = settings.find(ANDROID_CONTROL_AE_LOCK);
    if (e.count != 0) {
        mAeLocked = (e.data.u8[0] == ANDROID_CONTROL_AE_LOCK_ON);
    }

    mAeCompensation = 0;
    targetBrightness = mTargetBrightness;
    e = settings.find(ANDROID_CONTROL_AE_EXPOSURE_COMPENSATION);
    if (e.count != 0 && e.data.i32) {
        mAeCompensation = e.data.i32[0];
    }
    //FIXME - find out how are brigthness and compensation related
    targetBrightness+= (double)mAeCompensation *
                (FELIX_AE_BRIGHTNESS_RANGE/(double)kCompensationRange);

    ISPC::clip(targetBrightness, -1.0, 1.0);

    LOG_DEBUG("TargetBrightness set to %f", targetBrightness);
    LOG_DEBUG("AE Compensation set to %d", mAeCompensation);

    //should we start precapture?
    e = settings.find(ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER);
    bool precaptureTrigger = false;
    if (e.count != 0) {
        precaptureTrigger =
            (e.data.u8[0] == ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER_START);
            camera_metadata_enum_snprint(ANDROID_CONTROL_AF_TRIGGER, precaptureTrigger,
                    metadataStr, sizeof(metadataStr));
            LOG_DEBUG("AE trigger set to %s", metadataStr);
    }

    // If we have an aePrecaptureTrigger, aePrecaptureId should be set too
    if (e.count != 0) {
        e = settings.find(ANDROID_CONTROL_AE_PRECAPTURE_ID);

        if (e.count == 0) {
            LOG_ERROR("When android.control.aePrecaptureTrigger is set "
                    " in the request, aePrecaptureId needs to be set as well");
            return BAD_VALUE;
        }

        mAeTriggerId = e.data.i32[0];
    }

    if (precaptureTrigger) {
        // take shortcut to precapture state
        mState = ANDROID_CONTROL_AE_STATE_PRECAPTURE;
        LOG_DEBUG("Pre capture trigger!");
        mPrecaptureStarted = true;
    }

    switch(mMode) {
        case ANDROID_CONTROL_AE_MODE_OFF:
            mState = ANDROID_CONTROL_AE_STATE_INACTIVE;
            doAE = false;
            mPrecaptureStarted = false;
            mPrecaptureFinished = false;
            break;
        case ANDROID_CONTROL_AE_MODE_ON:
        case ANDROID_CONTROL_AE_MODE_ON_AUTO_FLASH:
        case ANDROID_CONTROL_AE_MODE_ON_ALWAYS_FLASH:
        case ANDROID_CONTROL_AE_MODE_ON_AUTO_FLASH_REDEYE:
            processAeStateInput();
            break;

        default:
            mState = ANDROID_CONTROL_AE_STATE_INACTIVE;
            doAE = false;
            break;
    }

    // can't be initialized in processUrgentHALMetadata() because
    // more shots would possibly be enqueued between
    // processUrgentHALMetadata() and processDeferredHALMetadata()
    mCurrentRegions = &settings.getAeRegions();
    mMeteringMode = getMeteringMode(*mCurrentRegions);

    if(mMeteringMode == CENTER_WEIGHT)
    {
        // override provided regions with all supported regions
        updateRegionsWithDefaults();
    }

    camera_metadata_enum_snprint(ANDROID_CONTROL_AE_MODE, mMode,
            metadataStr, sizeof(metadataStr));
    LOG_DEBUG("AE got mode %s (%d)", metadataStr, mMode);
    camera_metadata_enum_snprint(ANDROID_CONTROL_AE_STATE, mState,
            metadataStr, sizeof(metadataStr));
    LOG_DEBUG("AE got state %s (%d)", metadataStr, mState);
    camera_metadata_enum_snprint(ANDROID_CONTROL_AE_LOCK, mAeLocked,
                metadataStr, sizeof(metadataStr));
    LOG_DEBUG("AE lock state %s", metadataStr);
    camera_metadata_enum_snprint(ANDROID_CONTROL_AE_ANTIBANDING_MODE, mAbMode,
                metadataStr, sizeof(metadataStr));
    LOG_DEBUG("AE flicker state %s (%d)", metadataStr, mAbMode);

    return OK;
}
// process metadata provided by app framework
status_t FelixAE::processUrgentHALMetadata(FelixMetadata &settings) {
    status_t ret = OK;
    camera_metadata_entry e;

    e = settings.find(ANDROID_CONTROL_AE_ANTIBANDING_MODE);
    if(e.count) {
        mAbMode = (control_ae_ab_mode_t)*e.data.u8;
    }
    enableAutoFlickerRejection(false);
    switch(mAbMode) {
    case ANDROID_CONTROL_AE_ANTIBANDING_MODE_OFF:
        enableFlickerRejection(false);
        break;
    case ANDROID_CONTROL_AE_ANTIBANDING_MODE_50HZ:
        enableFlickerRejection(true, 50);
        break;
    case ANDROID_CONTROL_AE_ANTIBANDING_MODE_60HZ:
        enableFlickerRejection(true, 60);
        break;
    case ANDROID_CONTROL_AE_ANTIBANDING_MODE_AUTO:
        enableFlickerRejection(true);
        enableAutoFlickerRejection(true);
        break;
    default:
        LOG_ERROR("Invalid Antibanding mode set");
        break;
    }

    if(getMeteringMode(settings.getAeRegions()) == SINGLE_ROI)
    {
        const region_t region(*settings.getAeRegions().begin());
        LOG_DEBUG("single ROI region (%d,%d,%d,%d, w=%f) requested",
                region.left, region.top, region.right, region.bottom,
                region.weight);
        ret = configureRoiMeteringMode(region);
    } else {
        ret = configureDefaultMeteringMode();
    }
    return ret;
}

void FelixAE::advertiseCapabilities(CameraMetadata &settings,
                HwManager& hwManager) {

    static const uint8_t availableAeModes[] = {
        ANDROID_CONTROL_AE_MODE_OFF,
        ANDROID_CONTROL_AE_MODE_ON
    };
    settings.update(ANDROID_CONTROL_AE_AVAILABLE_MODES, availableAeModes,
            sizeof(availableAeModes));

    // - this value depends on sensor
    static const camera_metadata_rational exposureCompensationStep = {
        1, kCompensationStep
    }; // =1/kCompensationStep EV (1EV=2*current_exp_time)
    settings.update(ANDROID_CONTROL_AE_COMPENSATION_STEP,
            &exposureCompensationStep, 1);

    int32_t exposureCompensationRange[] =
                { -kCompensationRange, kCompensationRange };
    settings.update(ANDROID_CONTROL_AE_COMPENSATION_RANGE,
            exposureCompensationRange,
            sizeof(exposureCompensationRange) / sizeof(int32_t));

    sensor_t* sensor = hwManager.getSensor();
    ALOG_ASSERT(sensor);
    const int32_t sensorFps = ceil(sensor->flFrameRate);
    // for specific sensor mode, the max framerate is defined in sensor config
    // therefore we define variable [1-maxFps] and fixed [maxFps-maxFps]
    // ranges
    static const int32_t availableTargetFpsRanges[] = {
            1, sensorFps, sensorFps, sensorFps,
    };
    settings.update(ANDROID_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES,
            availableTargetFpsRanges,
            ARRAY_NUM_ELEMENTS(availableTargetFpsRanges));

    static const uint8_t availableAntibandingModes[] = {
        ANDROID_CONTROL_AE_ANTIBANDING_MODE_OFF,
        ANDROID_CONTROL_AE_ANTIBANDING_MODE_50HZ,
        ANDROID_CONTROL_AE_ANTIBANDING_MODE_60HZ,
        ANDROID_CONTROL_AE_ANTIBANDING_MODE_AUTO
    };
    settings.update(ANDROID_CONTROL_AE_AVAILABLE_ANTIBANDING_MODES,
            availableAntibandingModes, sizeof(availableAntibandingModes));
}

status_t FelixAE::initialize(void) {

    status_t res = configureStatistics();
    if(res!=IMG_SUCCESS) {
        LOG_ERROR("configureStatistics() failed");
        return res;
    }
    if(!getPipelineOwner()) {
        LOG_ERROR("getPipelineOwner() failed");
        return IMG_ERROR_FATAL;
    }
    ModuleHIS *his = getPipelineOwner()->getModule<ModuleHIS>();
    ISPC::Sensor *sensor = getPipelineOwner()->getSensor();
    if(!his) {
        LOG_ERROR("get ModuleWBS failed");
        return IMG_ERROR_FATAL;
    }
    if(!sensor) {
        LOG_ERROR("get Sensor from pipeline owner failed");
        return IMG_ERROR_FATAL;
    }

    uint32_t regionX = his->aGridStartCoord[0];
    uint32_t regionY = his->aGridStartCoord[1];
    uint32_t regionWidth = his->aGridTileSize[0];
    uint32_t regionHeight = his->aGridTileSize[1];

    // check region 0
    if(regionWidth == 0 || regionHeight == 0) {
        LOG_ERROR("ModuleWBS: improper tile size");
        return IMG_ERROR_NOT_INITIALISED;
    }

    initInternalMeteringRegionData(
            regionX, regionY,
            sensor->uiWidth/regionWidth,
            sensor->uiHeight/regionHeight,
            regionWidth, regionHeight);
#if(0)
    return initDefaultRegions();
#else
    // it's up to the driver how to handle metering regions
    initNullRegion();
#endif

    mTargetBrightness = getTargetBrightness();

    ModuleFLD *pFLD = NULL;
    if (!getPipelineOwner() ||
        !(pFLD = getPipelineOwner()->getModule<ModuleFLD>()))
    {
        return IMG_ERROR_NOT_INITIALISED;
    }

    SENSOR_HANDLE handle = sensor->getHandle();
    SENSOR_INFO sensorMode;
    Sensor_GetInfo(handle, &sensorMode);
    pFLD->iVTotal =sensorMode.sMode.ui16VerticalTotal;
    pFLD->fFrameRate = sensorMode.sMode.flFrameRate;
    pFLD->bEnable = true;
    pFLD->setup();
    pFLD->requestUpdate();


    return NO_ERROR;
}

} /*namespace android*/

