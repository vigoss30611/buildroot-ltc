/**
******************************************************************************
@file HwManager.cpp

@brief Definition of Hardware manager class providing a level of abstraction
from ISPC library in v2500 Camera HAL

@note Hides specifics of multi context support which is not directly supported
in ISPC

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

#define LOG_TAG "CameraHwManager"

#include <felixcommon/userlog.h>

#include <climits>

#include <ui/Rect.h>
#include <utils/SystemClock.h>

#include "FelixMetadata.h"
#include "HwManager.h"
#include "AAA/FelixAF.h"
#include "AAA/FelixAWB.h"
#include "AAA/FelixAE.h"
#include "Helpers.h"

// ISPC includes
#include "sensors/dgsensor.h" // to access registered name in sensor API
#include "sensors/iifdatagen.h"

#include "ispc/ParameterFileParser.h"
#include "ispc/Camera.h"

// module used
#include "ispc/ModuleOUT.h"
#include "ispc/ModuleESC.h"
#include "ispc/ModuleDSC.h"
#include "ispc/ModuleLSH.h"

// control algorithms used
#include "ispc/ControlAE.h"
#include "ispc/ControlTNM.h"
#include "ispc/ControlDNS.h"
#include "ispc/ControlLBC.h"

#undef MONITOR_CLOCK_DRIFT

namespace android {

const uint32_t HwManager::kImplementationDefinedEncoderFormat =
    HAL_PIXEL_FORMAT_YCbCr_420_888;
const uint32_t HwManager::kImplementationDefinedDisplayFormat =
    HAL_PIXEL_FORMAT_RGBX_8888;

// Instantiation of static class members
uint32_t HwManager::mMaxHwSupportedWidth=0;
uint32_t HwManager::mMaxHwSupportedHeight=0;
uint8_t  HwManager::mHwContexts = 0;
BitSet32  HwManager::mHwContextsAvailable;
CI_HWINFO HwManager::sHWInfo;
Mutex HwManager::mHwLock;

/**
 * @brief Array used for buffer formats conversion between
 * camera HAL buffer formats and ISPC buffer formats.
 */
const struct HwManager::BufferFormatPair
                    HwManager::bufferFormatConvArray[] = {
    { HAL_PIXEL_FORMAT_RAW_SENSOR, BAYER_RGGB_12 },
    { HAL_PIXEL_FORMAT_RGB_888, BGR_888_24 },
    // the Android RGB and Felix BGR names mean the same memory layout
    { HAL_PIXEL_FORMAT_RGBA_8888, BGR_888_32 },
#ifdef USE_IMG_SPECIFIC_PIXEL_FORMATS
    { HAL_PIXEL_FORMAT_RGBX_101010, RGB_101010_32 },
#endif
    { HAL_PIXEL_FORMAT_RGBX_8888, BGR_888_32 },
    // BGRA_8888 disabled because not supported in framework -> CTS fails
    // java.lang.IllegalArgumentException:
    // format 0x5 was not defined in either ImageFormat or PixelFormat
    //{ HAL_PIXEL_FORMAT_BGRA_8888, RGB_888_32 },
    { HAL_PIXEL_FORMAT_YCrCb_420_SP, YVU_420_PL12_8 },
    { HAL_PIXEL_FORMAT_YCbCr_420_888, YUV_420_PL12_8 },
    { HAL_PIXEL_FORMAT_YCbCr_422_SP, YUV_422_PL12_8 },
    //
    // Encoder specific but currently not defined in Felix:
    //{ HAL_IMG_FormatYUV420PP, PXL_NONE },
    //{ HAL_IMG_FormatYUV420SP, PXL_NONE },
};

// names of effect configuration files
static const HwManager::Param_t defaultEffectConfigFiles[10] = {
    { ANDROID_CONTROL_EFFECT_MODE_OFF, "HAL_EFFECT_OFF_FILE" },
    { ANDROID_CONTROL_EFFECT_MODE_MONO, "HAL_EFFECT_MONO_FILE" },
    { ANDROID_CONTROL_EFFECT_MODE_NEGATIVE, "HAL_EFFECT_NEGATIVE_FILE" },
    { ANDROID_CONTROL_EFFECT_MODE_SOLARIZE, "HAL_EFFECT_SOLARIZE_FILE" },
    { ANDROID_CONTROL_EFFECT_MODE_SEPIA, "HAL_EFFECT_SEPIA_FILE" },
    { ANDROID_CONTROL_EFFECT_MODE_POSTERIZE, "HAL_EFFECT_POSTERIZE_FILE" },
    { ANDROID_CONTROL_EFFECT_MODE_WHITEBOARD, "HAL_EFFECT_WHITEBOARD_FILE" },
    { ANDROID_CONTROL_EFFECT_MODE_BLACKBOARD, "HAL_EFFECT_BLACKBOARD_FILE" },
    { ANDROID_CONTROL_EFFECT_MODE_AQUA, "HAL_EFFECT_AQUA_FILE" },
    { },
};

// names of scene mode configuration files
static const HwManager::Param_t defaultSceneModeConfigFiles[20] = {
    { ANDROID_CONTROL_SCENE_MODE_DISABLED, "HAL_SCENE_DISABLED_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_FACE_PRIORITY,
            "HAL_SCENE_FACE_PRIORITY_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_ACTION, "HAL_SCENE_ACTION_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_PORTRAIT, "HAL_SCENE_PORTRAIT_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_LANDSCAPE, "HAL_SCENE_LANDSCAPE_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_NIGHT, "HAL_SCENE_NIGHT_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_NIGHT_PORTRAIT, "HAL_SCENE_PORTRAIT_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_THEATRE, "HAL_SCENE_THEATRE_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_BEACH, "HAL_SCENE_BEACH_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_SNOW, "HAL_SCENE_SNOW_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_SUNSET, "HAL_SCENE_SUNSET_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_STEADYPHOTO, "HAL_SCENE_STEADYPHOTO_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_FIREWORKS, "HAL_SCENE_FIREWORKS_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_SPORTS, "HAL_SCENE_SPORTS_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_PARTY, "HAL_SCENE_PARTY_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_CANDLELIGHT, "HAL_SCENE_CANDLELIGHT_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_BARCODE, "HAL_SCENE_BARCODE_FILE" },
#if CAMERA_DEVICE_API_VERSION_CURRENT >= HARDWARE_DEVICE_API_VERSION(3, 2)
    { ANDROID_CONTROL_SCENE_MODE_HIGH_SPEED_VIDEO,
            "HAL_SCENE_HIGH_SPEED_VIDEO_FILE" },
    { ANDROID_CONTROL_SCENE_MODE_HDR, "HAL_SCENE_HDR_FILE" },
#endif
    { }
};

bool isDisplayUsage(const uint32_t usage) {
     return usage & (GRALLOC_USAGE_HW_TEXTURE |
                     GRALLOC_USAGE_HW_RENDER |
                     GRALLOC_USAGE_HW_2D |
                     GRALLOC_USAGE_HW_FB |
                     GRALLOC_USAGE_HW_COMPOSER);
 }

bool isEncoderUsage(const uint32_t usage) {
    return (usage & GRALLOC_USAGE_HW_VIDEO_ENCODER) ||
           ((usage & GRALLOC_USAGE_HW_CAMERA_ZSL) ==
                     GRALLOC_USAGE_HW_CAMERA_ZSL);
}

bool isZslUsage(const uint32_t usage) {
    return ((usage & GRALLOC_USAGE_HW_CAMERA_ZSL) ==
                     GRALLOC_USAGE_HW_CAMERA_ZSL);
}

HwTimeBase::HwTimeBase(const uint64_t hwClockFreqHz) :
        realtimeHw(0), prevHwTimestamp(0),
        ispClockHz(hwClockFreqHz)
#ifdef MONITOR_CLOCK_DRIFT
, clockDrift(30)
#endif
{
    ALOG_ASSERT(ispClockHz>0, "%s: System clock frequency not defined",
        __FUNCTION__);
    initHwNanoTimer();
}

nsecs_t HwTimeBase::hwClockToNs(const uint64_t cycles) {
    return s2ns(1) / ispClockHz * cycles;
}

uint64_t HwTimeBase::nsToHwClock(const nsecs_t nanoTime) {
    ALOG_ASSERT(nanoTime>=0, "%s: Timestamp can't be negative", __FUNCTION__);
    return (int64_t)ispClockHz/1000 * nanoTime/1000 / 1000;
}

void HwTimeBase::initHwNanoTimer(void) {
    prevHwTimestamp = 0;
}

nsecs_t HwTimeBase::processHwTimestamp(uint32_t hwTimestamp) {
    // implementation of nanosecond time base ticked by shot captures
    // the precise Felix HW timestamps are clocked by IP clock (50MHz on FPGA)
    // so the overflow occurs after each ~86s
    nsecs_t currentRealtimeNano = elapsedRealtimeNano();
    if(!prevHwTimestamp) {
    // use system time as the baseline for first captured frame of the stream
    // because the framework uses system time as the baseline either
        realtimeHw = nsToHwClock(currentRealtimeNano);
#ifdef MONITOR_CLOCK_DRIFT
        clockDrift.reset();
#endif
    } else {
        // add frame duration to the timestamp, prevent overflow
        uint32_t duration = hwTimestamp > prevHwTimestamp ?
                hwTimestamp - prevHwTimestamp :
                UINT_MAX - prevHwTimestamp + hwTimestamp;
        realtimeHw += duration;
        // prevent overflow and errors in later conversion to nsecs_t
        // which is signed
        realtimeHw &= LLONG_MAX;
    }
    prevHwTimestamp = hwTimestamp;
    nsecs_t realtimeHwNs = hwClockToNs(realtimeHw);
#ifdef MONITOR_CLOCK_DRIFT
    // handle clock drift
    clockDrift.addSample(currentRealtimeNano - realtimeHwNs);
    LOG_DEBUG("clock drift=%f us", clockDrift.getAverage()/1000);
#if(0)
    if(fabs(clockDrift.getAverage()) > ms2ns(1)) {
        // If the clocks drift away more than 1ms then schedule resync
        // in next capture
        prevHwTimestamp = 0;
        LOG_DEBUG("Clocks resynced");
    }
#endif
#endif /* MONITOR_CLOCK_DRIFT */
    return realtimeHwNs;
}

ANDROID_SINGLETON_STATIC_INSTANCE(HwManager);

HwManager::HwManager(void) :
    mSensorMode(0),
    mExternalDG(false),
    mMainPipeline(NULL),
    EffectConfigFiles(defaultEffectConfigFiles),
    SceneModeConfigFiles(defaultSceneModeConfigFiles),
    mLshWidthCache(-1),
    mLshHeightCache(-1),
    mStatus(NO_INIT){

    ISPC::CI_Connection con;
    if(!con.getConnection())
    {
        LOG_ERROR("Error in opening connection");
        return;
    }
    if(0 == mHwContexts) {
        sHWInfo = con.getConnection()->sHWInfo;

        mMaxHwSupportedWidth =
                sHWInfo.context_aMaxWidthMult[0];
        mMaxHwSupportedHeight =
                sHWInfo.context_aMaxHeight[0];
        mHwContexts = sHWInfo.config_ui8NContexts;
        ALOG_ASSERT(mHwContexts>0);
        // create initial bitmask of available HW contexts
        for (uint32_t i=0;i<mHwContexts;++i) {
            // use markBit() as we don't know the internal representation
            // of BitSet32.value
            mHwContextsAvailable.markBit(i);
        }

        dump();
    }
    mHwTimeBase.setHwClockFreqHz(
        (unsigned long)sHWInfo.ui32RefClockMhz * 1000000UL);
    mStatus = NO_ERROR;
}

void HwManager::dump() {
    LOG_INFO("IMG Camera hardware version %d.%d.%d contexts=%d "\
        "refClk=%uMHz flags=%X",
        sHWInfo.rev_ui8Major,
        sHWInfo.rev_ui8Minor,
        sHWInfo.rev_ui8Maint,
        mHwContexts,
        sHWInfo.ui32RefClockMhz,
        sHWInfo.eFunctionalities);
    LOG_INFO("HW config=%d uid=%d", sHWInfo.ui16ConfigId, sHWInfo.rev_uid);
    LOG_INFO("KO changelist=%u", sHWInfo.uiChangelist);
    for (int i=0;i<sHWInfo.config_ui8NContexts;++i) {
        LOG_INFO("Context %d: [%d,%d]x%d @ %d queue=%d", i,
                sHWInfo.context_aMaxWidthMult[i],
                sHWInfo.context_aMaxWidthSingle[i],
                sHWInfo.context_aMaxHeight[i],
                sHWInfo.context_aBitDepth[i],
                sHWInfo.context_aPendingQueue[i]);
    }
}

void HwManager::dumpPipelines(std::ostream& output) {
    Mutex::Autolock m(mHwLock);

    ISPC::ParameterList parameters;

    output << "# Begin ISPC configuration" << std::endl;

    PipelinesIter_t i = mPipelines.begin();
    for(;i!=mPipelines.end(); ++i) {
        cameraHw_t* pipeline = i->get();
        ALOG_ASSERT(pipeline, "invalid list element");

        pipeline->saveParameters(parameters, ISPC::ModuleBase::SAVE_VAL);
        output << std::hex << "# Pipeline " << pipeline << std::endl;
        ISPC::ParameterFileParser::save(parameters, output);
    }
    output << "# End ISPC configuration" << std::endl;
}

int HwManager::translateImplementationDefinedFormat(unsigned usage) {

    if (isEncoderUsage(usage)) {
        // The same mapping is used in gralloc.
        return kImplementationDefinedEncoderFormat;
    } else if (isDisplayUsage(usage)) {
        // The same mapping is used in gralloc.
        return kImplementationDefinedDisplayFormat;
    }
    return HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED;
}

// Converts camera HAL buffer format to ISPC buffer format.
ePxlFormat HwManager::convertBufferFormat(
        int format, unsigned usage) {
    ePxlFormat bufferFormat = PXL_NONE;

    if (format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED) {
        format = translateImplementationDefinedFormat(usage);
    }
    if(format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED) {
        LOG_ERROR("Unsupported usage flags for implementation defined buffer");
        return bufferFormat;
    }
    // Iterate through all supported buffer formats and find a match.
    for (unsigned formatIdx = 0; formatIdx < sizeof(bufferFormatConvArray) /
            sizeof (bufferFormatConvArray[0]);++formatIdx) {
        if (format == bufferFormatConvArray[formatIdx].cameraHalBufferFormat) {
            bufferFormat = bufferFormatConvArray[formatIdx].ispcBufferFormat;
            break;
        }
    }

    return bufferFormat;
}

status_t HwManager::initSensor(const std::string& name,
        const uint8_t mode,
        const SENSOR_FLIPPING flip) {
    Mutex::Autolock m(mHwLock);
    // obtain sensor id for given name (provided in FelixDefines.mk)
    int sensorId = sensor_t::GetSensorId(name);
    if(sensorId<0) {
        LOG_ERROR("Invalid sensor name");
        return NO_INIT;
    }
    if(mSensor.get()) {
        LOG_ERROR("Sensor already initialized");
        return NO_INIT;
    }
    mSensor = new sensor_t(sensorId);

    if(!mSensor.get()) {
        LOG_ERROR("Error while sensor init");
        return NO_MEMORY;
    }

    mSensor->configure(mode, flip);

    if(mSensor->uiWidth > mMaxHwSupportedWidth ||
       mSensor->uiHeight > mMaxHwSupportedHeight)
    {
        LOG_ERROR("Sensor mode defines image dimensions larger than "
            " supported by current hardware");
        return NO_INIT;
    }
    mSensor->disable();
    mSensorMode = mode;
    return OK;
}

status_t HwManager::initDgSensor(const std::string& name,
        uint8_t mode, bool externalDg) {

    Mutex::Autolock m(mHwLock);
    int sensorId;
    // obtain sensor id for given name (provided in FelixDefines.mk)
    if(externalDg)
    {
        // name is used as source for the data-generator
        // so we search for the name using its registered sensor API namespace
        sensorId = sensor_t::GetSensorId(std::string(EXTDG_SENSOR_INFO_NAME));
    }
    else
    {
        sensorId = sensor_t::GetSensorId(std::string(IIFDG_SENSOR_INFO_NAME));
    }
    if(sensorId<0) {
        LOG_ERROR("Invalid sensor name");
        return NO_INIT;
    }
    // Create sensor object without pipeline hardware
    // data generator on gasket 0
    // mSensorName is used to initialise the file here
    mSensorDg = new sensorDg_t(name, 0, false);

    if(!mSensorDg.get()) {
        LOG_ERROR("Error while sensor init");
        return NO_MEMORY;
    }

    mSensorDg->configure(mode);
    if(mSensorDg->uiWidth > mMaxHwSupportedWidth ||
       mSensorDg->uiHeight > mMaxHwSupportedHeight)
    {
        LOG_ERROR("Sensor mode defines image dimensions larger than "
            " supported by current hardware");
        return NO_INIT;
    }
    mSensorDg->disable();

    mSensorMode = mode;
    return OK;
}


status_t HwManager::createHwPipelines(uint8_t numContexts) {
    status_t res = OK;
    sp<cameraHw_t> pipeline;

    ALOG_ASSERT(numContexts>0,
            "Invalid configuration : 0 contexts requested");

    Mutex::Autolock m(mHwLock);

    ALOG_ASSERT(mPipelines.empty(), "mPipelines container not empty");
    ALOG_ASSERT(numContexts <= mHwContextsAvailable.count(),
            "Invalid configuration : number of contexts requested (%d) "
            "is greater than available (%d)",
            numContexts, mHwContextsAvailable.count());

    // create pipeline instances for all supported HW contexts
    for(unsigned int i = 0; i<numContexts; ++i) {

        // get first available context
        unsigned int ctx = mHwContextsAvailable.firstMarkedBit();
        if(mExternalDG) {
            pipeline = reinterpret_cast<cameraHw_t*>(
                    new cameraDgHw_t(ctx, mSensor.get()));
        } else {
            pipeline = new cameraHw_t(ctx, mSensor.get());
        }

        if (!pipeline.get())
        {
            LOG_ERROR("Unable to create pipeline %d instance", ctx);
            continue;
        }

        if(!mMainPipeline.unsafe_get()) {
            // store first pipeline as the default main pipeline for
            // running control modules
            mMainPipeline = pipeline;
            mMainPipeline.unsafe_get()->setOwnSensor(true);
        }
        if(mExternalDG) {
            res = ISPC::CameraFactory::populateCameraFromHWVersion(
                    *pipeline, 0);
        } else {
            res = ISPC::CameraFactory::populateCameraFromHWVersion(
                    *pipeline, mSensor.get());
        }
        if(res != NO_ERROR) {
            LOG_ERROR("Unable to configure pipeline %d instance", ctx);
            continue;
        }
        mPipelines.push_back(pipeline);
        // set context as unavailable
        mHwContextsAvailable.clearBit(ctx);
        LOG_DEBUG("Created pipeline %d (%p)", ctx, pipeline.get());
    }
    return res;
}

void HwManager::destroyHwPipelines(void) {
    Mutex::Autolock m(mHwLock);
    // there should be always a main pipeline set
    ALOG_ASSERT(mMainPipeline.promote().get());
    mMainPipeline.promote().get()->setOwnSensor(false);

    // destroys all pipeline objects as well
    while (mPipelines.begin() != mPipelines.end()) {
        sp<cameraHw_t> pipeline = *mPipelines.begin();
        uint32_t ctx = pipeline->getContextNumber();
        ALOG_ASSERT(!mHwContextsAvailable.hasBit(ctx));
        // return the context to the pool
        mHwContextsAvailable.markBit(ctx);
        mPipelines.erase(mPipelines.begin());
    }
    mMainPipeline.clear(); // clear weak ref
}

status_t HwManager::forEach(mPtr_t method) {
    if(!method) {
        return BAD_VALUE;
    }
    PipelinesIter_t i = mPipelines.begin();
    for(;i!=mPipelines.end(); ++i) {
        cameraHw_t* pipeline = i->get();
        ALOG_ASSERT(pipeline, "invalid list element");
        if((pipeline->*method)()!=IMG_SUCCESS) {
            return INVALID_OPERATION;
        }
    }
    return OK;
}

status_t HwManager::forEach(mPtr2_t method, unsigned int param) {
    if(!method) {
        return BAD_VALUE;
    }
    PipelinesIter_t i = mPipelines.begin();
    for(;i!=mPipelines.end(); ++i) {
        cameraHw_t* pipeline = i->get();
        ALOG_ASSERT(pipeline, "invalid list element");
        if((pipeline->*method)(param)!=IMG_SUCCESS) {
            return INVALID_OPERATION;
        }
    }
    return OK;
}


status_t HwManager::stopCapture(void) {
    Mutex::Autolock m(mHwLock);
    PipelinesIter_t i = mPipelines.begin();
    for(;i!=mPipelines.end(); ++i) {
        cameraHw_t* pipeline = i->get();
        ALOG_ASSERT(pipeline, "invalid list element");
        if(pipeline->state != ISPC::Camera::CAM_CAPTURING) {
            continue;
        }
        pipeline->stopCapture();
    }
    if(mSensor->getState() == ISPC::Sensor::SENSOR_ENABLED) {
        mSensor->disable();
    }
    return OK;
}

status_t HwManager::startCapture(void) {
    Mutex::Autolock m(mHwLock);
    PipelinesIter_t i = mPipelines.begin();
    for(;i!=mPipelines.end(); ++i) {
        cameraHw_t* pipeline = i->get();
        ALOG_ASSERT(pipeline, "invalid list element");
        if(pipeline == getMainPipeline()) {
            // as in 'V2500 DDK.ISP Control library.doc : 7.3 :
            // the main pipeline shall be started as the last one
            continue;
        }
        if(pipeline->state != ISPC::Camera::CAM_READY) {
            // do not enable pipelines which have not been setup correctly
            // that is are not needed in current stream config
            continue;
        }
        if(pipeline->startCapture()!=IMG_SUCCESS) {
            return INVALID_OPERATION;
        }
    }
    ALOG_ASSERT(getMainPipeline());
    if(getMainPipeline()->state == ISPC::Camera::CAM_READY) {
        return OK;
    }
    if(getMainPipeline()->startCapture()!=IMG_SUCCESS) {
        return INVALID_OPERATION;
    }
    return OK;
}

status_t HwManager::pauseCapture(void) {
    Mutex::Autolock m(mHwLock);
    PipelinesIter_t i = mPipelines.begin();
    for(;i!=mPipelines.end(); ++i) {
        cameraHw_t* pipeline = i->get();
        ALOG_ASSERT(pipeline, "invalid list element");
        // can pause if actively capturing only
        if(pipeline->state != ISPC::Camera::CAM_CAPTURING) {
            continue;
        }
        pipeline->getPipeline()->stopCapture();
    }
    return OK;
}

status_t HwManager::continueCapture(void) {
    Mutex::Autolock m(mHwLock);
    PipelinesIter_t i = mPipelines.begin();
    for(;i!=mPipelines.end(); ++i) {
        cameraHw_t* pipeline = i->get();
        ALOG_ASSERT(pipeline, "invalid list element");
        if(pipeline == getMainPipeline()) {
            // as in 'V2500 DDK.ISP Control library.doc : 7.3 :
            // the main pipeline shall be started as the last one
            continue;
        }
        if(pipeline->state != ISPC::Camera::CAM_CAPTURING) {
            // can continue capture only if has been paused
            //
            continue;
        }
        if(pipeline->getPipeline()->startCapture()!=IMG_SUCCESS) {
            return INVALID_OPERATION;
        }
    }
    ALOG_ASSERT(getMainPipeline());
    if(getMainPipeline()->state == ISPC::Camera::CAM_CAPTURING &&
       getMainPipeline()->getPipeline()->startCapture()!=IMG_SUCCESS) {
        return INVALID_OPERATION;
    }
    return OK;
}

status_t HwManager::program(void) {
    Mutex::Autolock m(mHwLock);
    return forEach(&cameraHw_t::program);
}

status_t HwManager::setupModules(void) {
    Mutex::Autolock m(mHwLock);
    return forEach(&cameraHw_t::setupModules);
}

status_t HwManager::addShots(uint32_t shotCount) {
    Mutex::Autolock m(mHwLock);
    return forEach(&cameraHw_t::addShots, shotCount);
}

status_t HwManager::disableAllOutputs(void) {
    Mutex::Autolock m(mHwLock);
    PipelinesIter_t i = mPipelines.begin();
    for(;i!=mPipelines.end(); ++i) {
        cameraHw_t* pipeline = i->get();
        ALOG_ASSERT(pipeline, "invalid list element");
        ISPC::ModuleOUT* glb = pipeline->getModule<ISPC::ModuleOUT>();
        if (!glb) {
            LOG_ERROR("No global config module instantiated in the pipeline!");
            continue;
        }
        glb->load(ISPC::ParameterList()); // load defaults
        glb->setup();
    }
    return NO_ERROR;
}

status_t HwManager::programParameters(
        const ISPC::ParameterList &parameters) {
    Mutex::Autolock m(mHwLock);

    PipelinesIter_t i = mPipelines.begin();
    for(;i!=mPipelines.end(); ++i) {
        if ((*i)->loadParameters(parameters) != IMG_SUCCESS) {
            LOG_ERROR("ISPC ERROR loading pipeline setup parameters");
            return NO_INIT;
        }
    }

    if (forEach(&cameraHw_t::setupModules) != NO_ERROR) {
        LOG_ERROR("Cannot set up modules!");
        return NO_INIT;
    }
    if (forEach(&cameraHw_t::program) != NO_ERROR) {
        LOG_ERROR("Cannot set up modules!");
        return NO_INIT;
    }
    return NO_ERROR;
}
status_t HwManager::initControlModules(
            const ISPC::ParameterList &parameters) {
    Mutex::Autolock m(mHwLock);
    //
    // Create and setup control modules. These must run on main pipeline only
    //
    cameraHw_t* mainPipeline = getMainPipeline();
    ISPC::ControlModule* module = new FelixAE();
    if (!module ||
            mainPipeline->registerControlModule(module) != IMG_SUCCESS) {
        LOG_ERROR("Failed to register AE control module!");
        return NO_INIT;
    }

    module = new FelixAWB();
    if (!module ||
            mainPipeline->registerControlModule(module) != IMG_SUCCESS) {
        LOG_ERROR("Failed to register AWB control module!");
        return NO_INIT;
    }

    if(mSensor->getFocusSupported()) {
        module = new FelixAF();
        if (!module ||
                mainPipeline->registerControlModule(module) != IMG_SUCCESS) {
            LOG_ERROR("Failed to register AF control module!");
            return NO_INIT;
        }
    }

    module = new ISPC::ControlTNM();
    if (!module ||
            mainPipeline->registerControlModule(module) != IMG_SUCCESS) {
        LOG_ERROR("Failed to register TNM control module!");
        return NO_INIT;
    }
    //for all slave pipelines:
    //    module->addPipeline(slave->getPipeline());

    module = new ISPC::ControlDNS(1.0);
    if (!module ||
            mainPipeline->registerControlModule(module) != IMG_SUCCESS) {
        LOG_ERROR("Failed to register DNS control module!");
        return NO_INIT;
    }

    module = new ISPC::ControlLBC();
    if (!module ||
            mainPipeline->registerControlModule(module) != IMG_SUCCESS) {
        LOG_ERROR("Failed to register LBC control module!");
        return NO_INIT;
    }

    FelixAF* afModule = mainPipeline->getControlModule<FelixAF>();
    if(afModule) {
        if (afModule->load(parameters) != IMG_SUCCESS ||
            afModule->initialize() != IMG_SUCCESS) {
            LOG_ERROR("Failed to init FelixAF!");
            return NO_INIT;
        }
    }
    FelixAE* aeModule = mainPipeline->getControlModule<FelixAE>();
    if(aeModule) {
        if (aeModule->load(parameters) != IMG_SUCCESS ||
            aeModule->initialize() != IMG_SUCCESS) {
            LOG_ERROR("Failed to init FelixAE!");
            return NO_INIT;
        }
        aeModule->setTargetBracket(FELIX_AE_TARGET_BRACKET);
    }

    FelixAWB* awbModule = mainPipeline->getControlModule<FelixAWB>();
    if(awbModule) {
        if (awbModule->load(parameters) != IMG_SUCCESS ||
            awbModule->initialize() != IMG_SUCCESS) {
            LOG_ERROR("Failed to init FelixAWB!");
            return NO_INIT;
        }
    }
    mainPipeline->getControlModule<ISPC::ControlLBC>()->load(parameters);

    return NO_ERROR;
}

status_t HwManager::loadParametersFromFile(
        const std::string& paramsFile,
        ISPC::ParameterList &parameters) {

    // read default pipeline configuration from external file
    parameters = ISPC::ParameterFileParser::parseFile(paramsFile);

    // handle HAL custom configuration fields
    std::string file;
    bool effectModeOffLoaded = false;
    ISPC::ParameterList noEffectParams; // auto-generated params for no effect

    // load effect parameters, if provided
    int i=0;
    while(!EffectConfigFiles[i].isEnd()) {
        // find
        file = parameters.getParameterString(
            EffectConfigFiles[i].paramName, 0, "");
        if(file.empty()) {
            ++i;
            continue;
        }
        EffectConfigFiles[i].params =
            ISPC::ParameterFileParser::parseFile(file);
        if(!EffectConfigFiles[i].params.validFlag) {
            ++i;
            continue;
        }
        LOG_DEBUG("Loaded %s", EffectConfigFiles[i].paramName.c_str());
        EffectsAvailable.add(EffectConfigFiles[i].modeIndex);
        // store new tags to noEffect set
        noEffectParams+=EffectConfigFiles[i].params;
        if(EffectConfigFiles[i].modeIndex ==
                ANDROID_CONTROL_EFFECT_MODE_OFF) {
            effectModeOffLoaded = true;
        }
        ++i;
    }
    // ANDROID_CONTROL_EFFECT_MODE_OFF is generated automatically
    // that is it should contain all fields aggregated from every available
    // config with the respective values read from mDefaultParameters

    if(!effectModeOffLoaded) {
        LOG_DEBUG("Auto generating pipeline parameters for "
                "ANDROID_CONTROL_EFFECT_MODE_OFF");
        // change all values of tags in noEffectParams to those from
        // mDefaultParameters
        for(ISPC::ParameterList::iterator tag = noEffectParams.begin();
                tag!=noEffectParams.end();
                ++tag) {
            ISPC::Parameter* defaultParam =
                    mDefaultParameters.getParameter(tag->first);
            if(defaultParam) {
                // copy default Parameter over loaded one
                tag->second = *defaultParam;
                LOG_DEBUG("NO EFFECT param: %s=%s",
                        tag->first.c_str(), tag->second.getString().c_str());
            } else {
                LOG_WARNING("No default value found for %s",
                        tag->first.c_str());
            }
        }
        EffectConfigFiles[(uint8_t)ANDROID_CONTROL_EFFECT_MODE_OFF].params =
                noEffectParams;
        EffectsAvailable.add(ANDROID_CONTROL_EFFECT_MODE_OFF);
    }

    // load scene mode parameters, if provided
    i=0;
    while(!SceneModeConfigFiles[i].isEnd()) {
        // find
        file = parameters.getParameterString(
            SceneModeConfigFiles[i].paramName, 0, "");
        if(!file.empty()) {
            SceneModeConfigFiles[i].params =
                ISPC::ParameterFileParser::parseFile(file);
            if(EffectConfigFiles[i].params.validFlag) {
                LOG_DEBUG("Loaded %s", EffectConfigFiles[i].paramName.c_str());
            }
        }
        ++i;
    }

    return OK;
}

bool HwManager::getLSHMapSize(int32_t& width, int32_t& height) {
    bool res = true;
    // if called for the first time, read from config and store in cache
    if(mLshWidthCache<0 || mLshHeightCache<0) {
        ISPC::ModuleLSH lsh; // use the local instance to parse the parameters
        //if(lsh.load(mDefaultParameters)!=NO_ERROR ||
        //    !lsh.hasGrid()) {
            mLshWidthCache = 1;
            mLshHeightCache = 1;
            res = false;
        //} else {
        //    mLshWidthCache = lsh.sGrid.ui16Width;
        //    mLshHeightCache = lsh.sGrid.ui16Height;
        //}
    } // use cached version
    width = mLshWidthCache;
    height = mLshHeightCache;
    return res;
}

/**
 * @brief detach stream from hardware pipeline
 *
 * @note disables specific hardware output
 */
bool HwManager::detachStream(const camera3_stream_t* const stream) {

    Mutex::Autolock m(mHwLock);

    if(buffersMap.indexOfKey(stream)<0) {
        return true;
    }
    PrivateStreamInfo* priv = streamToPriv(stream);
    if(!priv) {
        return false;
    }
    ALOG_ASSERT(priv->getPipeline(), "invalid private struct");
    ISPC::ModuleOUT* glb = priv->getPipeline()->getModule<ISPC::ModuleOUT>();
    if (!glb) {
        LOG_ERROR("No global config module instantiated in the pipeline!");
        return false;
    }
    if(priv->ciBuffType == CI_TYPE_ENCODER) {
        ALOG_ASSERT(glb->encoderType != PXL_NONE);
        glb->encoderType = PXL_NONE;
    } else if(priv->ciBuffType == CI_TYPE_DISPLAY) {
        ALOG_ASSERT(glb->displayType != PXL_NONE);
        glb->displayType = PXL_NONE;
    } else if(priv->ciBuffType == CI_TYPE_DATAEXT) {
        ALOG_ASSERT(glb->dataExtractionType != PXL_NONE);
        glb->dataExtractionType = PXL_NONE;
    }
    LOG_DEBUG("Detaching stream %p from pipeline %p",
            stream, priv->getPipeline());
    glb->setup();
    buffersMap.removeItem(stream);
    return true;
}

bool HwManager::getOutputParams(
        const camera3_stream_t * const stream,
        CI_BUFFTYPE& type, ePxlFormat& format) {
    ALOG_ASSERT(stream, "param is NULL");

    bool ret = false;
    type = CI_TYPE_NONE;
    format = convertBufferFormat(stream->format, stream->usage);
    if (format == PXL_NONE) {
        return false;
    }
    if (isEncoderFormat(format)) {
        type = CI_TYPE_ENCODER;
        ret = true;
    } else if(isDisplayFormat(format)) {
        type = CI_TYPE_DISPLAY;
        ret = true;
    } else if(isBayerFormat(format)) {
        type = CI_TYPE_DATAEXT;
        ret = true;
    }
    // return type is unsupported in HW
    return ret;
}

bool HwManager::getFirstHwOutputAvailable(
        __maybe_unused uint32_t width, __maybe_unused uint32_t height,
        uint32_t& format) {

    bool outputfound = false;
    Mutex::Autolock m(mHwLock);

    ISPC::ModuleOUT* glb = NULL;
    cameraHw_t* pipeline;
    PipelinesIter_t iter = mPipelines.begin();
    for(;iter!=mPipelines.end(); ++iter) {
        pipeline = iter->get();
        ALOG_ASSERT(pipeline, "invalid list element");
        glb = pipeline->getModule<ISPC::ModuleOUT>();
        if (!glb) {
            LOG_ERROR("No global config module instantiated in the pipeline!");
            continue;
        }
        if(glb->encoderType == PXL_NONE) {
            LOG_DEBUG("Found enc output on %p", pipeline);
            format = kImplementationDefinedEncoderFormat;
            outputfound = true;
            break;
        } else if(glb->displayType == PXL_NONE) {
            LOG_DEBUG("Found disp output on %p", pipeline);
            if(!outputfound) {
                format = HAL_PIXEL_FORMAT_RGB_888; //kImplementationDefinedDisplayFormat;
                outputfound = true;
            }
            // YUV output has priority over RGB, so
            // no break deliberately, still searching YUV output
        }
    }
    return outputfound;
}

/**
 * @brief attach the stream to hardware pipeline output
 *
 * @param[in] newStream android stream structure
 * @param[out] ciBuffType internal type of output
 * @note enables the hardware output, configures scaler,
 * buffer size and output format
 * @return pointer to Camera instance
 * @return NULL if pipeline not available or format not supported
 */
cameraHw_t* HwManager::attachStream(
        const camera3_stream_t* const newStream,
        CI_BUFFTYPE& ciBuffType) {

    ALOG_ASSERT(newStream, "newStream==NULL");

    if(newStream->stream_type == CAMERA3_STREAM_INPUT) {
        LOG_ERROR("Invalid HW stream type");
        return NULL;
    }

    // 1. find if the stream has not been attached to the pipeline already
    // if configured then just return the pointer, don't proceed further
    cameraHw_t* pipeline = getPipeline(newStream);
    if(pipeline) {
        return pipeline;
    }

    ePxlFormat internalFormat;
    if(!getOutputParams(newStream, ciBuffType, internalFormat)) {
        LOG_INFO("HAL stream format (0x%x) not supported by camera HW",
            newStream->format);
        return NULL;
    }
    // 2. find first pipeline with free output suitable for
    // handling newStream

    Mutex::Autolock m(mHwLock);

    bool outputFound = false;
    PipelinesIter_t iter = mPipelines.begin();
    ISPC::ModuleOUT* glb = NULL;
    for(;iter!=mPipelines.end(); ++iter) {
        pipeline = iter->get();
        ALOG_ASSERT(pipeline, "invalid list element");
        glb = pipeline->getModule<ISPC::ModuleOUT>();
        if (!glb) {
            LOG_ERROR("No global config module instantiated in the pipeline!");
            continue;
        }
        if(ciBuffType == CI_TYPE_ENCODER &&
                glb->encoderType == PXL_NONE) {
            LOG_DEBUG("Found enc output on %p", pipeline);
            glb->encoderType = internalFormat;
            // configure the pipeline output to new size
            // so the sizes of imported buffers will match pipeline
            // configuration
            // else import will fail
            if(pipeline->setEncoderDimensions(
                newStream->width, newStream->height)!=IMG_SUCCESS){
                return NULL;
            }
            outputFound = true;
            break;
        } else if(ciBuffType == CI_TYPE_DISPLAY &&
                glb->displayType == PXL_NONE) {
            LOG_DEBUG("Found disp output on %p", pipeline);
            glb->displayType = internalFormat;
            if(pipeline->setDisplayDimensions(
                    newStream->width, newStream->height)!=IMG_SUCCESS){
                return NULL;
            }
            outputFound = true;
            break;
        } else if(ciBuffType == CI_TYPE_DATAEXT &&
               glb->dataExtractionType == PXL_NONE) {
            LOG_DEBUG("Found bayer output on %p", pipeline);
            glb->dataExtractionType = internalFormat;
            ALOG_ASSERT(false, "RAW not supported");
#if(0)
            if(pipeline->setDispBufferDimensions(
                   newStream->width, newStream->height)!=IMG_SUCCESS){
               return NULL;
            }
#endif
            outputFound = true;
            break;
        }
    }
    if(outputFound) {
        ALOG_ASSERT(glb);
        glb->setup();
        buffersMap.add(newStream, pipeline);
    } else {
        LOG_ERROR("No more HW outputs available for a stream!");
        return NULL;
    }

    return pipeline;
}


status_t HwManager::scaleAndCrop(
        const camera3_stream_t* stream,
        const Rect& rect) {

    if(!stream) {
        return BAD_VALUE;
    }

    PrivateStreamInfo* priv = streamToPriv(stream);
    cameraHw_t* pipeline = priv->getPipeline();
    if(!pipeline) {
        // silently discard stream because is not supported in HW
        // ge. this is true for BLOB
        return NO_ERROR;
    }

    uint32_t outputWidth = stream->width;
    uint32_t outputHeight = stream->height;
    LOG_DEBUG("%s(%d, %d, %d, %d) -> (%dx%d)",
            priv->ciBuffType == CI_TYPE_ENCODER ? "enc" : "disp",
            rect.left, rect.top, rect.getWidth(), rect.getHeight(),
            outputWidth, outputHeight);

    Mutex::Autolock m(mHwLock);

     // configure scaler modules here
    if (priv->ciBuffType == CI_TYPE_ENCODER) {
        ISPC::ModuleESC* esc = pipeline->getModule<ISPC::ModuleESC>();
        if (!esc) {
            LOG_ERROR("ESC module not found");
            return INVALID_OPERATION;
        }
        esc->aPitch[0] = (IMG_FLOAT)rect.getWidth()/outputWidth;
        esc->aPitch[1] = (IMG_FLOAT)rect.getHeight()/outputHeight;
        esc->aRect[0] = rect.left;
        esc->aRect[1] = rect.top;
        esc->aRect[2] = outputWidth;
        esc->aRect[3] = outputHeight;
        esc->eRectType = ISPC::SCALER_RECT_SIZE;
        esc->requestUpdate();
    } else if (priv->ciBuffType == CI_TYPE_DISPLAY) {
        ISPC::ModuleDSC* dsc = pipeline->getModule<ISPC::ModuleDSC>();

        if (!dsc) {
            LOG_ERROR("DSC module not found");
            return INVALID_OPERATION;
        }
        dsc->aPitch[0] = (IMG_FLOAT)rect.getWidth()/outputWidth;
        dsc->aPitch[1] = (IMG_FLOAT)rect.getHeight()/outputHeight;
        dsc->aRect[0] = rect.left;
        dsc->aRect[1] = rect.top;
        dsc->aRect[2] = outputWidth;
        dsc->aRect[3] = outputHeight;
        dsc->eRectType = ISPC::SCALER_RECT_SIZE;
        dsc->requestUpdate();
    } else {
        ALOG_ASSERT(false, "Cannot rescale outputs other than ENC and DISP");
    }

    return NO_ERROR;
}

status_t HwManager::scaleAndNoCrop(const camera3_stream_t* stream) {
    uint32_t sensorWidth=0, sensorHeight=0;
    if(!getSensorResolution(sensorWidth, sensorHeight)) {
        return NO_INIT;
    }
    const Rect rect(sensorWidth, sensorHeight);
    return scaleAndCrop(stream, rect);
}

status_t HwManager::setEffect(
    camera_metadata_enum_android_control_effect_mode_t mode) {

    // find array index of specified effect
    int index = EffectsAvailable.indexOf(mode);
    if(index == NAME_NOT_FOUND ||
        !EffectConfigFiles[mode].isValid()) {
        return BAD_VALUE;
    }
    /*
     * 1. read current config
     * 2. override parameters of MODE_OFF to get default settings
     * 3. override parameters of specific effect
     * 4. program pipeline
     */
    ISPC::ParameterList params;
    getMainPipeline()->saveParameters(
            params, ISPC::ModuleBase::SaveType::SAVE_VAL);
    params+=EffectConfigFiles[(uint8_t)ANDROID_CONTROL_EFFECT_MODE_OFF].params;
    params+=EffectConfigFiles[mode].params;
    return programParameters(params);
}

nsecs_t HwManager::processHwTimestamp(const uint32_t hwTimestamp) {
    return mHwTimeBase.processHwTimestamp(hwTimestamp);
}

void HwManager::initHwNanoTimer(void) {
    mHwTimeBase.initHwNanoTimer();
}

} // namespace android

