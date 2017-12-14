/**
******************************************************************************
@file FelixAAA.h

@brief Definition of AWB v2500 HAL module

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

// verbose logs disabled by default
//#define LOG_NDEBUG 0
#define LOG_TAG "FELIX_AWB"
#undef BE_VERBOSE

#include <felixcommon/userlog.h>
#include <math.h>
#include "Controls/ispc/ControlAWB.h"
#include "FelixMetadata.h"
#include "FelixAWB.h"
#include "ispc/ModuleWBS.h"

#undef USE_AE_REGIONS_AS_AWB_REGIONS

namespace {
    char metadataStr[100] = { 0 };
}

using namespace ISPC;

namespace android {

double FelixAWB::weight(int32_t, int32_t){ return 0; }

status_t FelixAWB::configureRoiMeteringMode(const region_t& region) {
    ModuleWBS *wbs = 0;
    if(!getPipelineOwner()) {
        LOG_ERROR("getPipelineOwner() failed");
        return IMG_ERROR_FATAL;
    }
    wbs = getPipelineOwner()->getModule<ModuleWBS>();
    if(!wbs) {
        LOG_ERROR("get ModuleWBS  failed");
        return IMG_ERROR_FATAL;
    }
    //Program region 1
    wbs->aROIStartCoord[0][0]=region.left;
    wbs->aROIStartCoord[0][1]=region.top;
    wbs->aROIEndCoord[0][0]=region.right;
    wbs->aROIEndCoord[0][1]=region.bottom;

    //Program region 2
    wbs->aROIStartCoord[1][0]=region.left;
    wbs->aROIStartCoord[1][1]=region.top;
    wbs->aROIEndCoord[1][0]=region.right;
    wbs->aROIEndCoord[1][1]=region.bottom;
    wbs->requestUpdate();

    return IMG_SUCCESS;
}

status_t FelixAWB::configureDefaultMeteringMode(void) {
    statisticsRegionProgrammed = false;
    configureStatistics();
    return IMG_SUCCESS;
}

status_t FelixAWB::initRequestMetadata(FelixMetadata &settings,
        int type) {

    const uint8_t awbMode = (type != CAMERA3_TEMPLATE_MANUAL) ?
            ANDROID_CONTROL_AWB_MODE_AUTO :
            ANDROID_CONTROL_AWB_MODE_OFF;
    settings.update(ANDROID_CONTROL_AWB_MODE, &awbMode, 1);

    const uint8_t awbLock = ANDROID_CONTROL_AWB_LOCK_OFF;
    settings.update(ANDROID_CONTROL_AWB_LOCK, &awbLock, 1);

    return settings.updateMetadataWithRegions<FelixAWB>(
        ANDROID_CONTROL_AWB_REGIONS, mDefaultRegions);
}

status_t FelixAWB::updateHALMetadata(FelixMetadata &settings) {

    settings.update(ANDROID_CONTROL_AWB_STATE, (uint8_t *) &mState, 1);

    if(mCurrentRegions) {
        settings.updateMetadataWithRegions<FelixAWB>(
            ANDROID_CONTROL_AWB_REGIONS, *mCurrentRegions);
    }

    camera_metadata_enum_snprint(ANDROID_CONTROL_AWB_STATE, mState,
            metadataStr, sizeof(metadataStr));
    LOG_DEBUG("AWB state is %s (%d)", metadataStr, mState);

    return OK;
}

status_t FelixAWB::processDeferredHALMetadata(FelixMetadata &settings) {
    LOG_DEBUG("E");
    camera_metadata_entry e;

    //if CONTROL_MODE == OFF, inactivate AWB
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
        mState = ANDROID_CONTROL_AWB_STATE_INACTIVE;
        doAwb = false;
        return OK;
    }

    //scene modes, except FACE_PRIO, should inactivate AWB
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
                mState = ANDROID_CONTROL_AWB_STATE_INACTIVE;
                doAwb = false;
                return OK;
            }
        }
    }

    e = settings.find(ANDROID_CONTROL_AWB_MODE);
    if (e.count == 0) {
        LOG_ERROR("No AWB mode entry!");
        return BAD_VALUE;
    }
    mMode = (control_awb_mode_t) e.data.u8[0];
    doAwb = true;
    //user locked AWB?
    e = settings.find(ANDROID_CONTROL_AWB_LOCK);
    if (e.count != 0) {
        mAwbLocked = (e.data.u8[0] == ANDROID_CONTROL_AWB_LOCK_ON);
    }

    bConverged = hasConverged();

    switch(mMode) {
        case ANDROID_CONTROL_AWB_MODE_OFF:
            mNextState = ANDROID_CONTROL_AWB_STATE_INACTIVE;
            doAwb = false;
            break;
        case ANDROID_CONTROL_AWB_MODE_AUTO:
            switch(mState) {
                case ANDROID_CONTROL_AWB_STATE_INACTIVE:
                    if (mAwbLocked) {
                        mNextState = ANDROID_CONTROL_AWB_STATE_LOCKED;
                        doAwb = false;
                    }
                    else {
                        mNextState = ANDROID_CONTROL_AWB_STATE_SEARCHING;
                    }
                break;

                case ANDROID_CONTROL_AWB_STATE_SEARCHING:
                    if (mAwbLocked) {
                        mNextState = ANDROID_CONTROL_AWB_STATE_LOCKED;
                        doAwb = false;
                    }
                    else if (bConverged)
                    {
                        mNextState = ANDROID_CONTROL_AWB_STATE_CONVERGED;
                    }
                break;

                case ANDROID_CONTROL_AWB_STATE_CONVERGED:
                    if (mAwbLocked) {
                        mNextState = ANDROID_CONTROL_AWB_STATE_LOCKED;
                        doAwb = false;
                    }
                    else if (!bConverged) {
                        mNextState = ANDROID_CONTROL_AWB_STATE_SEARCHING;
                    }
                break;

                case ANDROID_CONTROL_AWB_STATE_LOCKED:
                    if (mAwbLocked) {
                        doAwb = false;
                    } else {
                        if (bConverged) {
                            mNextState = ANDROID_CONTROL_AWB_STATE_CONVERGED;
                        } else{
                            mNextState = ANDROID_CONTROL_AWB_STATE_SEARCHING;
                        }
                    }
                break;
                default:
                    mNextState = ANDROID_CONTROL_AWB_STATE_INACTIVE;
                    doAwb = false;
                break;
            }
            break;

        //These modes must be implemented in ISPC. They will use predefined
        //CCM settings, which are not available yet.
        case ANDROID_CONTROL_AWB_MODE_INCANDESCENT:
        case ANDROID_CONTROL_AWB_MODE_FLUORESCENT:
        case ANDROID_CONTROL_AWB_MODE_WARM_FLUORESCENT:
        case ANDROID_CONTROL_AWB_MODE_DAYLIGHT:
        case ANDROID_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT:
        case ANDROID_CONTROL_AWB_MODE_TWILIGHT:
        case ANDROID_CONTROL_AWB_MODE_SHADE:
        default:
            mNextState = ANDROID_CONTROL_AWB_STATE_INACTIVE;
            break;
    }

    // get focus regions
#ifdef USE_AE_REGIONS_AS_AWB_REGIONS
    e = settings.find(ANDROID_CONTROL_AE_REGIONS);
#else
    e = settings.find(ANDROID_CONTROL_AWB_REGIONS);
#endif
    uint32_t regions = e.count / region_t::rawEntrySize;
    rawRegion_t data = e.data.i32;
    if(!data || (regions > mMaxRegions)) {
        LOG_ERROR("Inconsistent region metadata provided from framework!");
        return BAD_VALUE;
    }
    mCurrentRegions = &settings.getAwbRegions();
    mMeteringMode = getMeteringMode(*mCurrentRegions);
    if(mMeteringMode == CENTER_WEIGHT)
    {
        // override provided regions with all supported regions
        updateRegionsWithDefaults();
    }
    if(mMeteringMode == SINGLE_ROI) {
        region_t region(data);
        imageTotalPixels = region.getWidth()*region.getHeight();
    } else {
        const ISPC::Sensor *sensor = 0;
        if(getPipelineOwner()) {
            sensor = getPipelineOwner()->getSensor();
            if(!sensor) {
                LOG_ERROR("failed to getSensor() from pipeline owner");
                return IMG_ERROR_FATAL;
            }
        } else {
            LOG_ERROR("getPipelineOwner() failed");
            return IMG_ERROR_FATAL;
        }

        imageTotalPixels = sensor->uiWidth*sensor->uiHeight;
    }

    mState = mNextState;

    camera_metadata_enum_snprint(ANDROID_CONTROL_AWB_MODE, mMode,
            metadataStr, sizeof(metadataStr));
    LOG_DEBUG("AWB got mode %s", metadataStr);
    camera_metadata_enum_snprint(ANDROID_CONTROL_AWB_STATE, mState,
            metadataStr, sizeof(metadataStr));
    LOG_DEBUG("AWB got state %s", metadataStr);
    camera_metadata_enum_snprint(ANDROID_CONTROL_AWB_LOCK, mAwbLocked,
                metadataStr, sizeof(metadataStr));
        LOG_DEBUG("AWB lock state %s", metadataStr);

    return OK;
}

// process metadata provided by app framework
status_t FelixAWB::processUrgentHALMetadata(FelixMetadata &settings) {
    status_t ret = OK;

#ifdef USE_AE_REGIONS_AS_AWB_REGIONS
    if(getMeteringMode(settings.getAeRegions()) == SINGLE_ROI)
#else
    if(getMeteringMode(settings.getAwbRegions()) == SINGLE_ROI)
#endif
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

void FelixAWB::advertiseCapabilities(CameraMetadata &settings) {
    static const uint8_t availableAwbModes[] = {
        ANDROID_CONTROL_AWB_MODE_OFF,
        ANDROID_CONTROL_AWB_MODE_AUTO
        // must be implemented in ISPC
        /*
        ANDROID_CONTROL_AWB_MODE_INCANDESCENT,
        ANDROID_CONTROL_AWB_MODE_FLUORESCENT,
        ANDROID_CONTROL_AWB_MODE_DAYLIGHT,
        ANDROID_CONTROL_AWB_MODE_SHADE
        */
    };
    settings.update(ANDROID_CONTROL_AWB_AVAILABLE_MODES,
            availableAwbModes, sizeof(availableAwbModes));
}

status_t FelixAWB::initialize(void) {

    status_t res = configureStatistics();
    if(res!=IMG_SUCCESS) {
        LOG_ERROR("configureStatistics() failed");
        return res;
    }

    const ISPC::Sensor *sensor = 0;
    ModuleWBS *wbs = 0;
    if(!getPipelineOwner()) {
        LOG_ERROR("getPipelineOwner() failed");
        return IMG_ERROR_FATAL;
    }
    wbs = getPipelineOwner()->getModule<ModuleWBS>();
    if(!wbs) {
        LOG_ERROR("get ModuleWBS  failed");
        return IMG_ERROR_FATAL;
    }
    sensor = getPipelineOwner()->getSensor();
    if(!sensor) {
        LOG_ERROR("getSensor() from pipeline owner failed");
        return IMG_ERROR_FATAL;
    }

    uint32_t regionX = wbs->aROIStartCoord[0][0];
    uint32_t regionY = wbs->aROIStartCoord[0][1];
    uint32_t regionWidth = wbs->aROIEndCoord[0][0]- regionX;
    uint32_t regionHeight = wbs->aROIEndCoord[0][1]- regionY;

    // WBS
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
    return NO_ERROR;
}

} /*namespace android*/
