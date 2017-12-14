/**
******************************************************************************
@file FelixAF.cpp

@brief Definition of AF v2500 HAL module

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
#define LOG_TAG "FELIX_AF"
#undef BE_VERBOSE

#include <felixcommon/userlog.h>
#include "FelixMetadata.h"
#include "FelixAF.h"
#include "ispc/ModuleFOS.h"
#include <math.h>

using namespace ISPC;
template class ISPC::mavg<double>;

namespace android
{

const double FelixAF::RECIP = 1.0;

// explicit instantiation needed
template class RegionHandler<sAfRegion>;

double FelixAF::weight(int32_t indexH, int32_t indexV) {
    return (ControlAF::WEIGHT_7X7_SPREAD[indexH][indexV]*
                (1.0-this->centerWeigth)+
            ControlAF::WEIGHT_7X7_CENTRAL[indexH][indexV]*
                this->centerWeigth);
}

status_t FelixAF::configureRoiMeteringMode(const region_t& region) {
    ModuleFOS *fos = 0;
    if(!getPipelineOwner()) {
        LOG_ERROR("getPipelineOwner() failed");
        return IMG_ERROR_FATAL;
    }
    fos = getPipelineOwner()->getModule<ModuleFOS>();
    if(!fos) {
        LOG_ERROR("get ModuleFOS  failed");
        return IMG_ERROR_FATAL;
    }
    fos->bEnableROI = true;
    fos->aRoiStartCoord[0] = region.left;
    fos->aRoiStartCoord[1] = region.top;
    fos->aRoiEndCoord[0] = region.right;
    fos->aRoiEndCoord[1] = region.bottom;
    fos->requestUpdate();
    return IMG_SUCCESS;
}

status_t FelixAF::configureDefaultMeteringMode(void) {
    ModuleFOS *fos = 0;
    if(!getPipelineOwner()) {
        LOG_ERROR("getPipelineOwner() failed");
        return IMG_ERROR_FATAL;
    }
    fos = getPipelineOwner()->getModule<ModuleFOS>();
    if(!fos) {
        LOG_ERROR("get ModuleFOS  failed");
        return IMG_ERROR_FATAL;
    }
    // assume no other module updates FOS configuration
    if(fos->bEnableROI) {
        fos->bEnableROI = false;
        fos->requestUpdate();
    }
    return IMG_SUCCESS;
}

status_t FelixAF::initRequestMetadata(FelixMetadata &settings, int type) {
    uint8_t afMode = ANDROID_CONTROL_AF_MODE_AUTO;
    switch (type) {
    case CAMERA3_TEMPLATE_PREVIEW:
        afMode = ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE;
        break;
    case CAMERA3_TEMPLATE_STILL_CAPTURE:
        afMode = ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE;
        break;
    case CAMERA3_TEMPLATE_VIDEO_RECORD:
        afMode = ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO;
        break;
    case CAMERA3_TEMPLATE_VIDEO_SNAPSHOT:
        afMode = ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO;
        break;
    case CAMERA3_TEMPLATE_ZERO_SHUTTER_LAG:
        afMode = ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE;
        break;
    case CAMERA3_TEMPLATE_MANUAL:
        afMode = ANDROID_CONTROL_AF_MODE_OFF;
        break;
    default:
        afMode = ANDROID_CONTROL_AF_MODE_AUTO;
        break;
    }
    settings.update(ANDROID_CONTROL_AF_MODE, &afMode, 1);

    const uint8_t afTrigger = ANDROID_CONTROL_AF_TRIGGER_IDLE;
    settings.update(ANDROID_CONTROL_AF_TRIGGER, &afTrigger, 1);
    const int32_t afTriggerId = 0;
    settings.update(ANDROID_CONTROL_AF_TRIGGER_ID, &afTriggerId, 1);

    return settings.updateMetadataWithRegions<FelixAF>(
        ANDROID_CONTROL_AF_REGIONS, mDefaultRegions);
}

status_t FelixAF::updateHALMetadata(FelixMetadata &settings) {
    const ISPC::Sensor *sensor = 0;
    if(!getPipelineOwner()) {
        LOG_ERROR("getPipelineOwner() failed");
        return IMG_ERROR_FATAL;
    }
    sensor = getPipelineOwner()->getSensor();
    if(!sensor) {
        LOG_ERROR("getSensor() from pipeline owner failed");
        return IMG_ERROR_FATAL;
    }

    //update AF state
    settings.update(ANDROID_CONTROL_AF_STATE,(uint8_t *) &mAfState, 1);

    // send latest trigger id
    settings.update(ANDROID_CONTROL_AF_TRIGGER_ID, &mAfTriggerId, 1);

    uint32_t focus = sensor->getFocusDistance();
    float curFocus = (focus < maxFocusDistance) ? 1000.0/focus : 0.0;
    settings.update(ANDROID_LENS_FOCUS_DISTANCE, &curFocus, 1);

    if(mCurrentRegions &&
       mBestRegion != mCurrentRegions->end()) {
        regions_t region;
        region.push_front(*mBestRegion);
        // return sharpest region focused in auto mode
        settings.updateMetadataWithRegions<FelixAF>(
            ANDROID_CONTROL_AF_REGIONS, region);
#ifdef BE_VERBOSE
        LOG_DEBUG("Returning sharpest region %dx%d (%d,%d,%d,%d)",
                mBestRegion->indexH(), mBestRegion->indexV(),
                mBestRegion->left, mBestRegion->top,
                mBestRegion->right, mBestRegion->bottom);
#endif
    } else {
        settings.updateMetadataWithRegions<FelixAF>(
                ANDROID_CONTROL_AF_REGIONS, *mCurrentRegions);
    }

    {
        char metadataStr[100];

        camera_metadata_enum_snprint(ANDROID_CONTROL_AF_STATE, mAfState,
                metadataStr, sizeof(metadataStr));
        LOG_DEBUG("AF state is %s (%d)", metadataStr, mAfState);
        LOG_DEBUG("AF trigger ID is %d", mAfTriggerId);
    }
    return OK;
}

// process metadata provided by app framework
status_t FelixAF::processDeferredHALMetadata(FelixMetadata &settings) {
    camera_metadata_entry e;
    control_af_mode_t newMode = mAfMode; // no change by default

    e = settings.find(ANDROID_CONTROL_MODE);
    if (e.count == 0) {
        LOG_ERROR("%s: No control mode entry!", __FUNCTION__);
        return BAD_VALUE;
    }
    m3aMode = (control_mode_t)e.data.u8[0];

    if(m3aMode == ANDROID_CONTROL_MODE_OFF) {
        mAfState = ANDROID_CONTROL_AF_STATE_INACTIVE;
    } else {
        //get mode
        e = settings.find(ANDROID_CONTROL_AF_MODE);
        if (e.count == 0) {
            LOG_ERROR("No AF mode entry!");
            return BAD_VALUE;
        }
        newMode = (control_af_mode_t) e.data.u8[0];
    }
    if(newMode != mAfMode)
    {
        // if mode has changed - state shall be forced to inactive
        mAfState = ANDROID_CONTROL_AF_STATE_INACTIVE;
    }
    mAfMode = newMode;
    if(mAfMode == ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE ||
       mAfMode == ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO)
    {
        mAfScanMode = AF_MODE_CONTINUOUS;
    }
    else
    {
        mAfScanMode = AF_MODE_SINGLE_SHOT;
    }

    //get trigger
    e = settings.find(ANDROID_CONTROL_AF_TRIGGER);
    if (e.count != 0) {
        mAfTrigger = static_cast<control_af_trigger_t>(e.data.u8[0]);

        e = settings.find(ANDROID_CONTROL_AF_TRIGGER_ID);

        if (e.count == 0) {
            LOG_ERROR("When android.control.afTrigger is set in the"
                "request, afTriggerId needs to be set as well");
            return BAD_VALUE;
        }
        if(mAfTrigger!=ANDROID_CONTROL_AF_TRIGGER_IDLE) {
            // update if not idle
            mAfTriggerId = e.data.i32[0];
        }

        LOG_DEBUG("AF trigger ID set to 0x%x", mAfTriggerId);
    } else {
        mAfTrigger = ANDROID_CONTROL_AF_TRIGGER_IDLE;
    }

    mCurrentRegions = &settings.getAfRegions();
    mMeteringMode = getMeteringMode(*mCurrentRegions);

    if(mMeteringMode == BEST_TILE ||
       mMeteringMode == CENTER_WEIGHT)
    {
        // override provided regions with all supported regions
        updateRegionsWithDefaults();
    }
    mBestRegion = mCurrentRegions->end();

    {
        char metadataStr[100] = { 0 };

        camera_metadata_enum_snprint(ANDROID_CONTROL_AF_MODE, mAfMode,
                metadataStr, sizeof(metadataStr));
        LOG_DEBUG("AF mode is %s", metadataStr);
        camera_metadata_enum_snprint(ANDROID_CONTROL_AF_STATE, mAfState,
                metadataStr, sizeof(metadataStr));
        LOG_DEBUG("AF state is %s (%d)", metadataStr, mAfState);
        camera_metadata_enum_snprint(ANDROID_CONTROL_AF_TRIGGER, mAfTrigger,
                metadataStr, sizeof(metadataStr));
        LOG_DEBUG("AF trigger set to %s", metadataStr);
    }
    return OK;
}

// process metadata provided by app framework
status_t FelixAF::processUrgentHALMetadata(FelixMetadata &settings) {
    status_t ret = OK;

    if(getMeteringMode(settings.getAfRegions()) == SINGLE_ROI)
    {
        const region_t region(*settings.getAfRegions().begin());
        LOG_DEBUG("single ROI region (%d,%d,%d,%d, w=%f) requested",
                region.left, region.top, region.right, region.bottom,
                region.weight);
        ret = configureRoiMeteringMode(region);
    } else {
        ret = configureDefaultMeteringMode();
    }
    return ret;
}

void FelixAF::advertiseCapabilities(CameraMetadata &settings) {
    const uint8_t availableAfModesFull[] = {
        ANDROID_CONTROL_AF_MODE_OFF,
        ANDROID_CONTROL_AF_MODE_AUTO,
        ANDROID_CONTROL_AF_MODE_MACRO,
        ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO,
        ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE
    };
    settings.update(ANDROID_CONTROL_AF_AVAILABLE_MODES, availableAfModesFull,
                    sizeof(availableAfModesFull));
}

double FelixAF::calculateRegionSharpness(const ISPC::Metadata &metadata) {
    double output = 0.0;
    double totalWeight = 0.0;

    if(mCurrentRegions->size() == 0) {
        LOG_ERROR("No regions provided for sharpness calculation!");
        return 0;
    }
    if(mMeteringMode == SINGLE_ROI) {
        ALOG_ASSERT(!mCurrentRegions->empty(),
            "Empty regions but ROI requested");
        mBestRegion = mCurrentRegions->begin();
        output = (double)metadata.focusStats.ROISharpness;
        LOG_DEBUG("ROI (%d, %d, %d, %d) sharpness=%f",
            mBestRegion->left, mBestRegion->top,
            mBestRegion->right, mBestRegion->bottom, output);
        return output;
    }

    // : crop current regions to current crop region

    regions_t::iterator region = mCurrentRegions->begin();
    // process all regions for current request
    while(region!=mCurrentRegions->end()) {
        region->sharpness =
            metadata.focusStats.gridSharpness
                [region->indexH()][region->indexV()];
#ifdef BE_VERBOSE
        LOG_DEBUG("Region %dx%d weight=%f sharpness=%f",
                region->indexH(), region->indexV(),
                region->weight, region->sharpness);
#endif
        if(mMeteringMode == BEST_TILE) {
            // : should we use weights?
            if((double)region->sharpness > output) {
                output = region->sharpness;
                mBestRegion = region;
            }
        } else {
            // calc weighted sharpness
            output += region->sharpness*region->weight;
            totalWeight += region->weight;
        }
        ++region;
    }
    if(mMeteringMode == WEIGHTED_AVERAGE ||
       mMeteringMode == CENTER_WEIGHT) {
        // total weighted average
        output /= totalWeight;
        // calc best region for current capture
    } else {
        LOG_DEBUG("The sharpest region in auto mode is %dx%d!",
                mBestRegion->indexH(), mBestRegion->indexV());
    }

    return output;
}

status_t FelixAF::initialize(void) {

    status_t res = configureStatistics();
    if(res!=IMG_SUCCESS) {
        LOG_ERROR("configureStatistics() failed");
        return res;
    }

    ModuleFOS *fos = 0;
    const ISPC::Sensor *sensor = 0;
    if(!getPipelineOwner()) {
        LOG_ERROR("getPipelineOwner() failed");
        return IMG_ERROR_FATAL;
    }
    fos = getPipelineOwner()->getModule<ModuleFOS>();
    if(!fos) {
        LOG_ERROR("get ModuleFOS  failed");
        return IMG_ERROR_FATAL;
    }
    sensor = getPipelineOwner()->getSensor();
    if(!sensor) {
        LOG_ERROR("getSensor() from pipeline owner failed");
        return IMG_ERROR_FATAL;
    }

    if(fos->aGridTileSize[0] == 0 || fos->aGridTileSize[1] == 0) {
        LOG_ERROR("ModuleFOS: improper tile size");
        return IMG_ERROR_NOT_INITIALISED;
    }
    initInternalMeteringRegionData(
            fos->aGridStartCoord[0], fos->aGridStartCoord[1],
            sensor->uiWidth/fos->aGridTileSize[0],
            sensor->uiHeight/fos->aGridTileSize[1],
            fos->aGridTileSize[0], fos->aGridTileSize[1]);

    return initDefaultRegions();
}

// process Android state machine
IMG_RESULT FelixAF::update(const ISPC::Metadata &metadata) {
    ISPC::Sensor *sensor = 0;
    if(!getPipelineOwner()) {
        LOG_ERROR("getPipelineOwner() failed");
        return IMG_ERROR_FATAL;
    }
    sensor = getPipelineOwner()->getSensor();
    if(!sensor) {
        LOG_ERROR("getSensor() from pipeline owner failed");
        return IMG_ERROR_FATAL;
    }
    // - move it to another (which one?) function
    if(!flagInitialized)
    {
        // cannot be done in constructor because the sensor is added later
        sensor->setFocusDistance(sensor->getMinFocus());
        minFocusDistance = sensor->getMinFocus();

        if(0 == minFocusDistance) {
            minFocusDistance = 1; // disallow infinity diopters
        }
        maxFocusDistance = sensor->getMaxFocus();
        if(0 == maxFocusDistance) {
            // disallow infinity diopters
            maxFocusDistance = minFocusDistance+1;
        }
        minFocusDiopter = RECIP/minFocusDistance;
        maxFocusDiopter = RECIP/maxFocusDistance;
        //configureStatistics();  //Configure the module statistics
        bestSharpness =0.0;
        bestFocusDistance= 0;
        currentState = AF_IDLE;
        nextState   = AF_IDLE;
        scanState   = AF_SCAN_STOP;
        lastCommand = AF_NONE;

        flagInitialized = true;
    }

    if(m3aMode == ANDROID_CONTROL_MODE_OFF &&
       currentState == AF_IDLE) {
        // if 3A off and AF state machine is idle then no further processing
        return IMG_SUCCESS;
    }

    ControlAF::Command cmd = AF_NONE;

    switch(mAfMode) {
    case ANDROID_CONTROL_AF_MODE_AUTO:
    case ANDROID_CONTROL_AF_MODE_MACRO:
        //trigger's driven scan
        if (mAfTrigger == ANDROID_CONTROL_AF_TRIGGER_START) {
            mAfState = ANDROID_CONTROL_AF_STATE_ACTIVE_SCAN;
            cmd = AF_TRIGGER;
            //currentState = AF_SCANNING;
        }
        else if (mAfTrigger == ANDROID_CONTROL_AF_TRIGGER_CANCEL) {
            mAfState = ANDROID_CONTROL_AF_STATE_INACTIVE;
            cmd = AF_STOP;
        } else if(mAfState == ANDROID_CONTROL_AF_STATE_INACTIVE) {
            cmd = AF_STOP;
        }
        //scan was activated earlier and not finished yet
        else if (mAfState == ANDROID_CONTROL_AF_STATE_ACTIVE_SCAN) {
            if(currentState == AF_FOCUSED) {
                cmd = AF_STOP;
                mAfState = ANDROID_CONTROL_AF_STATE_FOCUSED_LOCKED;
            } else if (currentState == AF_SCANNING){
                cmd = AF_NONE;
                // continue scanning
            } else if (currentState == AF_OUT) {
                mAfState = ANDROID_CONTROL_AF_STATE_NOT_FOCUSED_LOCKED;
                cmd = AF_STOP;
            }
        }
    break;

    // : the focus in video mode should be slower than in cont. picture
    case ANDROID_CONTROL_AF_MODE_CONTINUOUS_PICTURE:
    case ANDROID_CONTROL_AF_MODE_CONTINUOUS_VIDEO:
        if (mAfTrigger == ANDROID_CONTROL_AF_TRIGGER_START) {
            // framework locks focus
            if(mAfState == ANDROID_CONTROL_AF_STATE_INACTIVE)
            {
                mAfState = ANDROID_CONTROL_AF_STATE_NOT_FOCUSED_LOCKED;
            } else if(currentState == AF_FOCUSED) {
                cmd = AF_STOP;
                mAfState = ANDROID_CONTROL_AF_STATE_FOCUSED_LOCKED;
            } else if (currentState == AF_OUT ||
                       currentState == AF_SCANNING){
                mAfState = ANDROID_CONTROL_AF_STATE_NOT_FOCUSED_LOCKED;
                cmd = AF_STOP;
            }
        }
        else if (mAfTrigger == ANDROID_CONTROL_AF_TRIGGER_CANCEL) {
            mAfState = ANDROID_CONTROL_AF_STATE_INACTIVE;
            cmd = AF_STOP;
        }
        else if (mAfState == ANDROID_CONTROL_AF_STATE_INACTIVE) {
            // hal triggers new scan
            mAfState = ANDROID_CONTROL_AF_STATE_PASSIVE_SCAN;
            //currentState = AF_SCANNING;
            cmd = AF_TRIGGER;
        }
        else if ((mAfState == ANDROID_CONTROL_AF_STATE_PASSIVE_SCAN) ||
                (mAfState = ANDROID_CONTROL_AF_STATE_PASSIVE_FOCUSED) ||
                (mAfState = ANDROID_CONTROL_AF_STATE_PASSIVE_UNFOCUSED)) {
            // continue scanning
            if (currentState == AF_FOCUSED) {
                mAfState = ANDROID_CONTROL_AF_STATE_PASSIVE_FOCUSED;
            }
            else if (currentState == AF_OUT) {
                mAfState = ANDROID_CONTROL_AF_STATE_PASSIVE_UNFOCUSED;
            }
            else
            {
                mAfState = ANDROID_CONTROL_AF_STATE_PASSIVE_SCAN;
            }
        }
    break;

    default:
    //Mode == (EDOF || OFF)
        mAfState = ANDROID_CONTROL_AF_STATE_INACTIVE;
        cmd = AF_STOP;
    break;
    }

    {
        char afString[100] = { 0 };
        camera_metadata_enum_snprint(ANDROID_CONTROL_AF_STATE, mAfState,
                        afString, sizeof(afString));
        LOG_DEBUG("AF_STATE=%s cmd=%s state=%s mode %s",
            afString, CommandName(cmd), StateName(currentState),
            ScanStateName(scanState));
    }
    // calculate region sharpness
    double sharpness = 0.0;
//    if(mAfState != ANDROID_CONTROL_AF_STATE_INACTIVE &&
//        scanState != AF_SCAN_STOP &&
//        scanState != AF_SCAN_INIT) {
        sharpness = calculateRegionSharpness(metadata);
//    }
    runAF(sensor->getFocusDistance(), sharpness, cmd);

    return IMG_SUCCESS;
}

void FelixAF::clipScanLimits(void) {
    // protect for overflows
    scanInitDiopter =
        ISPC::clip(scanInitDiopter, maxFocusDiopter, minFocusDiopter);
    scanEndDiopter =
        ISPC::clip(scanEndDiopter, maxFocusDiopter, minFocusDiopter);
    scanInit =
        ISPC::clip(RECIP/scanInitDiopter, minFocusDistance, maxFocusDistance);
    scanEnd =
        ISPC::clip(RECIP/scanEndDiopter, minFocusDistance, maxFocusDistance);
}

void FelixAF::runAF(unsigned int lastFocusDistance, double sharpness,
        ControlAF::Command command) {

    LOG_DEBUG("cur=%d best=%f min=%f max=%f target=%f init=%f end=%f step=%f",
        lastFocusDistance,
        bestFocusDiopter, minFocusDiopter, maxFocusDiopter, targetFocusDiopter,
        scanInitDiopter, scanEndDiopter,
        scanState == AF_SCAN_ROUGH ? roughScanStep : fineScanStep);

    ISPC::Sensor *sensor = 0;
    if(!getPipelineOwner()) {
        LOG_ERROR("getPipelineOwner() failed");
        return;
    }
    sensor = getPipelineOwner()->getSensor();
    if(!sensor) {
        LOG_ERROR("getSensor() from pipeline owner failed");
        return;
    }

    lastCommand = command;
    if (command == AF_STOP) {
        bestSharpness =0.0;
        bestFocusDiopter= 0;
        currentState = AF_IDLE;
        nextState   = AF_IDLE;
        scanState   = AF_SCAN_STOP;
        scanInit = minFocusDistance;
        scanEnd = maxFocusDistance;
        return;
    }

    const double lastFocusDiopter = RECIP/lastFocusDistance;
    switch(currentState)
    {
    case AF_IDLE:       //Waiting
        if(command == AF_TRIGGER)
        {
            nextState = AF_SCANNING;
            scanState = AF_SCAN_INIT;
        }
        if(command == AF_FOCUS_CLOSER)
            targetFocusDistance = targetFocusDistance*1.1;

        if(command == AF_FOCUS_FURTHER)
            targetFocusDistance = targetFocusDistance/1.1;
    break;

    case AF_SCANNING: //Scanning for optimal focus
    case AF_FOCUSED:  //In-focus found, can be stopped or in finished state
        nextState = AF_SCANNING;
        if(command == AF_TRIGGER)
        {
            // shortcut in scanning mode
            scanState = AF_SCAN_INIT;
        }
        switch(scanState)
        {
        case AF_SCAN_STOP:  //just initializing the focus scan
            nextState = AF_IDLE;
        break;

        case AF_SCAN_INIT: //focus scan positioned ready to start scan
            bestSharpness = 0.0;
            minSharpness = DBL_MAX;
            // the direction depends on the actual position of lens
            // to obtain shortest sweep time
            if(lastFocusDistance < (minFocusDistance+maxFocusDistance)/2)
            {
                scanInit  = minFocusDistance;
                scanEnd   = maxFocusDistance;
            }
            else
            {
                scanInit  = maxFocusDistance;
                scanEnd   = minFocusDistance;
            }
            scanInitDiopter  = RECIP/scanInit;
            scanEndDiopter   = RECIP/scanEnd;
            roughScanStep = (scanEndDiopter - scanInitDiopter)/AF_ROUGH_STEPS;
            bestFocusDiopter = scanInitDiopter;
            targetFocusDiopter = scanInitDiopter;
            targetFocusDistance = scanInit;
            scanState = AF_SCAN_ROUGH;
        break;

        case AF_SCAN_ROUGH: //rough focus scan
            if(sharpness>bestSharpness)
            {
                bestSharpness = sharpness;
                bestFocusDiopter = lastFocusDiopter;
                LOG_DEBUG("rough: bestSharpness = %f bestFocus=%d" ,
                        bestSharpness, (int)(RECIP/bestFocusDiopter));
            } else if (sharpness<minSharpness)
            {
                minSharpness = sharpness;
            }

            // check if gone above scan limits
            if (lastFocusDistance == scanEnd)
            //finished the rough scanning
            {
                // 2*rough scan step range in fine mode
                scanInitDiopter = bestFocusDiopter + roughScanStep;
                scanEndDiopter = bestFocusDiopter - roughScanStep;
                clipScanLimits();
                fineScanStep = (scanEndDiopter - scanInitDiopter)/AF_FINE_STEPS;
                targetFocusDiopter = scanInitDiopter;
                bestFocusDiopter = scanInitDiopter;
                bestSharpness = 0.0;
                scanState = AF_SCAN_FINE;
            }
            else
            {
                targetFocusDiopter = lastFocusDiopter + roughScanStep;
                targetFocusDiopter =
                    ISPC::clip(targetFocusDiopter,
                        maxFocusDiopter, minFocusDiopter);
            }
            targetFocusDistance = RECIP/targetFocusDiopter;
            if(abs((int)targetFocusDistance-(int)lastFocusDistance)<1) {
                ALOGV("CORRECTION: %d->%d, %d",
                    lastFocusDistance,
                    targetFocusDistance, std::signbit(roughScanStep));
                targetFocusDistance += (std::signbit(roughScanStep) ? 1 : -1);
            }
            targetFocusDistance =
                ISPC::clip(targetFocusDistance, scanInit,scanEnd);


        break;

        case AF_SCAN_FINE:  //fine focus scan
            if(sharpness>bestSharpness)
            {
                bestSharpness = sharpness;
                bestFocusDiopter = lastFocusDiopter;
                LOG_DEBUG("fine: bestSharpness = %f bestFocus=%d" ,
                        bestSharpness, (int)(RECIP/bestFocusDiopter));
            } else if (sharpness<minSharpness)
            {
                minSharpness = sharpness;
            }

            // check if gone above scan limits
            if (lastFocusDistance == scanEnd)
            //finished the fine scanning
            {
                scanState = AF_SCAN_FINISHED;
                average.reset();
                targetFocusDiopter = bestFocusDiopter;
                if(mAfScanMode == AF_MODE_CONTINUOUS)
                {
                    bestSharpness = 0.0;
                }
            }
            else
            {
                targetFocusDiopter = lastFocusDiopter + fineScanStep;
                targetFocusDiopter =
                    ISPC::clip(targetFocusDiopter,
                            maxFocusDiopter, minFocusDiopter);
            }
            // protect for the situation where integer step gets smaller than 1
            targetFocusDistance = RECIP/targetFocusDiopter;
            if(abs((int)targetFocusDistance-(int)lastFocusDistance)<1) {
                ALOGV("CORRECTION: %d->%d, %d",
                     lastFocusDistance,
                     targetFocusDistance, std::signbit(fineScanStep));
                targetFocusDistance += (std::signbit(fineScanStep) ? 1 : -1);
            }
            targetFocusDistance =
                ISPC::clip(targetFocusDistance, scanInit, scanEnd);
        break;

        case AF_SCAN_REFINE:  //refine focus scan
            if(sharpness>bestSharpness)
            {
                bestSharpness = sharpness;
                bestFocusDiopter = lastFocusDiopter;
                LOG_DEBUG("refine: bestSharpness = %f bestFocus=%f" ,
                        bestSharpness, RECIP/bestFocusDiopter);
            } else if (sharpness<minSharpness)
            {
                minSharpness = sharpness;
            }

            // check if gone above scan limits
            if (lastFocusDistance == scanEnd)
            //finished the fine scanning
            {
                // FIXME: need better exit condition here
                if(bestSharpness>refineSharpnessThreshold)
                {
                    // quick refocus succeeded
                    scanState = AF_SCAN_FINISHED;
                    targetFocusDiopter = bestFocusDiopter;
                    if(mAfScanMode == AF_MODE_CONTINUOUS)
                    {
                        bestSharpness = 0.0;
                    }
                }
                else
                {
                    nextState = AF_OUT;
                }
            }
            else
            {
                targetFocusDiopter = lastFocusDiopter + fineScanStep;
                targetFocusDiopter =
                    ISPC::clip(targetFocusDiopter,
                            maxFocusDiopter, minFocusDiopter);
            }
            targetFocusDistance = RECIP/targetFocusDiopter;
            if(abs((int)targetFocusDistance-(int)lastFocusDistance)<1) {
                LOG_DEBUG("CORRECTION: %d->%d, %d",
                    lastFocusDistance,
                    targetFocusDistance, std::signbit(fineScanStep));
                targetFocusDistance += (std::signbit(fineScanStep) ? 1 : -1);
            }
            targetFocusDistance =
                ISPC::clip(targetFocusDistance, scanInit, scanEnd);
            break;

        case AF_SCAN_POSITIONING:
            targetFocusDistance =
                    ISPC::clip(targetFocusDistance,
                            minFocusDistance, maxFocusDistance);
            sensor->setFocusDistance(targetFocusDistance);
            nextState = AF_SCANNING;
            break;

        case AF_SCAN_FINISHED:
            if(mAfScanMode == AF_MODE_SINGLE_SHOT) {
                if (bestSharpness < OUT_OF_FOCUS_MIN_SHARPNESS) {
                    nextState = AF_OUT;
                    scanState = AF_SCAN_STOP;
                    LOG_DEBUG("Cannot lock focus (sharpness = %f)\n",
                             bestSharpness);
                } else {
                    nextState = AF_FOCUSED;
                    scanState = AF_SCAN_STOP;
                    LOG_DEBUG("Scan finished (sharpness = %f)\n",
                             bestSharpness);
                }
            } else {
                // detect fast increasing changes which can be
                // the complete scene changes -> initiate new scan
                if(bestSharpness>0 &&
                    sharpness > 1.5 * bestSharpness) {
                    scanState = AF_SCAN_INIT;
                    break;
                }
                // moving average for slow changes
                sharpness = average.addSample(sharpness);
                if(sharpness>bestSharpness) {
                    bestSharpness = sharpness;
                } else if (sharpness<minSharpness) {
                    minSharpness = sharpness;
                }
                refineSharpnessThreshold = 0.6*bestSharpness;
                LOG_DEBUG("fini: AVG(sharpness) = %f best=%f min=%f" ,
                        sharpness, bestSharpness, minSharpness);
                if(sharpness < refineSharpnessThreshold) {
                    average.reset();

                    bestSharpness = 0.0;

                    scanInitDiopter = bestFocusDiopter + roughScanStep;
                    scanEndDiopter = bestFocusDiopter - roughScanStep;
                    clipScanLimits();

                    targetFocusDiopter = scanInitDiopter;
                    targetFocusDistance =
                        ISPC::clip(RECIP/targetFocusDiopter, scanInit, scanEnd);
                    bestFocusDiopter = scanInitDiopter;
                    scanState = AF_SCAN_REFINE;
                }
                else
                {
                    nextState = AF_FOCUSED;
                    LOG_DEBUG("Focused (sharpness = %f)\n", sharpness);
                }
            }
            break;
        }
        targetFocusDistance =
                ISPC::clip(targetFocusDistance,
                        minFocusDistance, maxFocusDistance);
        sensor->setFocusDistance(targetFocusDistance);
        break;

        case AF_OUT:        //Unable to get in-focus
            // need to refocus
            if(command == AF_TRIGGER ||
               mAfScanMode == AF_MODE_CONTINUOUS)
            {
                nextState = AF_SCANNING;
                scanState = AF_SCAN_INIT;
            }
        break;

        default:
            break;
    }
    currentState = nextState;// prepare for next iteration
}

} //namespace android

