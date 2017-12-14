/**
******************************************************************************
@file FelixCamera3v2.cpp

@brief Definition of main FelixCamera v3.2 class in v2500 HAL module

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

#define LOG_TAG "FelixCamera"
#include "felixcommon/userlog.h"

#include <cmath>
#include <list>

// HAL includes
#include "FelixCamera.h"
#include "FelixMetadata.h"
#include "FelixProcessing.h"
#include "CaptureRequest.h"

#include "HwManager.h"
#include "HwCaps.h"
#include "Helpers.h"
#include "AAA/FelixAF.h"
#include "AAA/FelixAWB.h"
#include "AAA/FelixAE.h"
#include "JpegEncoder/JpegEncoderSw.h"
// Android includes

#include "utils/Timers.h"
#include "ui/Rect.h"
#include "ui/GraphicBufferMapper.h"
#include "ui/GraphicBufferAllocator.h"

#undef UPSCALING_ENABLED /* needs GPU support */

namespace android {

// : find a way to automatically manage these both lists
const int32_t FelixCamera3v2::mSupportedRequestKeys[] = {
    ANDROID_COLOR_CORRECTION_MODE,
    ANDROID_COLOR_CORRECTION_TRANSFORM,
    ANDROID_CONTROL_AE_ANTIBANDING_MODE,
    ANDROID_CONTROL_AE_EXPOSURE_COMPENSATION,
    ANDROID_CONTROL_AE_LOCK,
    ANDROID_CONTROL_AE_MODE,
    ANDROID_CONTROL_AE_PRECAPTURE_ID,
    ANDROID_CONTROL_AE_PRECAPTURE_TRIGGER,
    ANDROID_CONTROL_AE_REGIONS,
    ANDROID_CONTROL_AE_TARGET_FPS_RANGE,
    ANDROID_CONTROL_AF_MODE,
    ANDROID_CONTROL_AF_REGIONS,
    ANDROID_CONTROL_AF_TRIGGER_ID,
    ANDROID_CONTROL_AF_TRIGGER,
    ANDROID_CONTROL_AWB_LOCK,
    ANDROID_CONTROL_AWB_MODE,
    ANDROID_CONTROL_AWB_REGIONS,
    ANDROID_CONTROL_CAPTURE_INTENT,
    ANDROID_CONTROL_EFFECT_MODE,
    ANDROID_CONTROL_MODE,
    ANDROID_CONTROL_SCENE_MODE,
    ANDROID_CONTROL_VIDEO_STABILIZATION_MODE,
    ANDROID_DEMOSAIC_MODE,
    ANDROID_EDGE_MODE,
    ANDROID_EDGE_STRENGTH,
    ANDROID_FLASH_FIRING_POWER,
    ANDROID_FLASH_FIRING_TIME,
    ANDROID_FLASH_MODE,
    ANDROID_HOT_PIXEL_MODE,
    ANDROID_JPEG_GPS_COORDINATES,
    ANDROID_JPEG_GPS_PROCESSING_METHOD,
    ANDROID_JPEG_GPS_TIMESTAMP,
    ANDROID_JPEG_ORIENTATION,
    ANDROID_JPEG_QUALITY,
    ANDROID_JPEG_THUMBNAIL_QUALITY,
    ANDROID_JPEG_THUMBNAIL_SIZE,
    ANDROID_LENS_APERTURE,
    ANDROID_LENS_FILTER_DENSITY,
    ANDROID_LENS_FOCAL_LENGTH,
    ANDROID_LENS_FOCUS_DISTANCE,
    ANDROID_LENS_OPTICAL_STABILIZATION_MODE,
    ANDROID_NOISE_REDUCTION_MODE,
    ANDROID_NOISE_REDUCTION_STRENGTH,
    ANDROID_REQUEST_FRAME_COUNT,
    ANDROID_REQUEST_ID,
    ANDROID_REQUEST_METADATA_MODE,
    ANDROID_REQUEST_TYPE,
    ANDROID_SCALER_CROP_REGION,
    ANDROID_SENSOR_EXPOSURE_TIME,
    ANDROID_SENSOR_FRAME_DURATION,
    ANDROID_SENSOR_SENSITIVITY,
    ANDROID_SENSOR_TIMESTAMP,
    ANDROID_SENSOR_TEST_PATTERN_MODE,
    ANDROID_SHADING_MODE,
    ANDROID_STATISTICS_FACE_DETECT_MODE,
    ANDROID_STATISTICS_HISTOGRAM_MODE,
    ANDROID_STATISTICS_SHARPNESS_MAP_MODE,
    ANDROID_STATISTICS_LENS_SHADING_MAP_MODE,
    ANDROID_STATISTICS_HOT_PIXEL_MAP_MODE,
    ANDROID_TONEMAP_CURVE_BLUE,
    ANDROID_TONEMAP_CURVE_GREEN,
    ANDROID_TONEMAP_CURVE_RED,
    ANDROID_TONEMAP_MODE,
};

const int32_t FelixCamera3v2::mSupportedResultKeys[] = {
    ANDROID_CONTROL_AE_PRECAPTURE_ID,   // updated in FelixAE
    ANDROID_CONTROL_AE_REGIONS,         // updated in FelixAE
    ANDROID_CONTROL_AE_STATE,           // updated in FelixAE
    ANDROID_CONTROL_AF_REGIONS,         // updated in FelixAF
    ANDROID_CONTROL_AF_STATE,           // updated in FelixAF
    ANDROID_CONTROL_AF_TRIGGER_ID,      // updated in FelixAF
    ANDROID_CONTROL_AWB_REGIONS,        // updated in FelixAWB
    ANDROID_CONTROL_AWB_STATE,          // updated in FelixAWB
    ANDROID_FLASH_STATE,                // updated in FelixAE
    ANDROID_JPEG_SIZE,                  // updated in ProcessingThread
    ANDROID_LENS_FOCUS_DISTANCE,        // updated in FelixAF
    ANDROID_LENS_APERTURE,
    ANDROID_LENS_FILTER_DENSITY,
    ANDROID_SENSOR_EXPOSURE_TIME,       // updated in FelixAE
    ANDROID_SENSOR_FRAME_DURATION,      // updated in FelixMetadata
    ANDROID_SENSOR_ROLLING_SHUTTER_SKEW,// updaetd in FelixMetadata
    ANDROID_SENSOR_SENSITIVITY,         // updated in FelixAE
    ANDROID_SENSOR_TIMESTAMP,           // updated in ProcessingThread
    ANDROID_SENSOR_TEST_PATTERN_MODE,
    ANDROID_STATISTICS_FACE_IDS,        // updated in ProcessingThread
    ANDROID_STATISTICS_FACE_LANDMARKS,  // updated in ProcessingThread
    ANDROID_STATISTICS_FACE_RECTANGLES, // updated in ProcessingThread
    ANDROID_STATISTICS_FACE_SCORES,     // updated in ProcessingThread
    ANDROID_STATISTICS_HISTOGRAM,       // updated in FelixMetadata
    ANDROID_STATISTICS_SCENE_FLICKER,   // updated in FelixMetadata
    ANDROID_STATISTICS_LENS_SHADING_MAP,// updated in FelixMetadata
    ANDROID_STATISTICS_HOT_PIXEL_MAP_MODE,
    ANDROID_TONEMAP_CURVE_BLUE,
    ANDROID_TONEMAP_CURVE_GREEN,
    ANDROID_TONEMAP_CURVE_RED,
};

// ----------------------------------------------------------------------------
// Camera device lifecycle methods
// ----------------------------------------------------------------------------

// Felix camera constructor
// The common field comes from the camera3_device FelixCamera inherits from
FelixCamera3v2::FelixCamera3v2(int cameraId, bool facingBack,
        struct hw_module_t *module) :
            FelixCamera(cameraId, facingBack, module)
    {

    // override mode to LIMITED due to no HW support for all features
    // FULL is required to support
    mFullMode = false;
    // override the HAL API version
    common.version      = CAMERA_DEVICE_API_VERSION_3_2;
    // override the ops
    ops = &sDeviceOps;

}

FelixCamera3v2::~FelixCamera3v2() {
    LOG_DEBUG("E");
}

//
// Camera3 interface methods
//

// Register Buffers for a given stream with the HAL device. Called after
// configure_streams and before buffers are included in a capture request.
// If same stream is reused this function will not be called again.
// This method allows the HAL device to map or prepare the buffers for later
// use. Buffers passed in will be locked, at the end of the call all of them
// must be ready to be returned to the stream. This function allows the device
// to map the buffers or whatever other operation is required. The buffers
// passed will be already locked for use.
status_t FelixCamera3v2::registerStreamBuffers(
        __maybe_unused const camera3_stream_buffer_set *bufferSet) {

    ALOG_ASSERT(false, "Prohibited!");
    return NO_INIT;
}

// Create capture setting for standard camera use cases.
// The device must return a settings buffer that is configured to meet
// the requested use case which must be one of:
//              CAMERA3_TEMPLATE_PREVIEW
//              CAMERA3_TEMPLATE_CAPTURE
//              CAMERA3_TEMPLATE_RECORD
//              CAMERA3_TEMPLATE_SNAPSHOT
//              CAMERA3_TEMPLATE_ZERO_SHUTTER_LAG
//              CAMERA3_TEMPLATE_COUNT
//
// We have to fill a CameraMetadata object with all the configuration
// parameters CameraMetadata is a C++ wrapper for manipulating
// camera_metadata_t (which is C based)
const camera_metadata_t* FelixCamera3v2::constructDefaultRequestSettings(
        int type) {
    LOG_DEBUG("E (%d)", type);

    if (mStatus < STATUS_OPEN) {
        LOG_ERROR("Invalid state");
        return NULL;
    }

    // Make sure this is a valid request
    if (type < 0 || type >= CAMERA3_TEMPLATE_COUNT) {
        LOG_ERROR("Unknown request settings template: %d", type);
        return NULL;
    }

    // We have to keep the response in memory until the device is closed
    if (mDefaultRequestTemplates[type] != NULL) {
        return mDefaultRequestTemplates[type];
    }

    // we have to extend base request settings with new ones,
    // related to camera2 api
    FelixMetadata settings(const_cast<camera_metadata_t*>(
            FelixCamera::constructDefaultRequestSettings(type)), *this);

    Mutex::Autolock l(mLock);

    const int32_t testPatternMode = ANDROID_SENSOR_TEST_PATTERN_MODE_OFF;
    settings.update(ANDROID_SENSOR_TEST_PATTERN_MODE,
            &testPatternMode, 1);

    const uint8_t hotPixelMapMode = ANDROID_STATISTICS_HOT_PIXEL_MAP_MODE_OFF;
    settings.update(ANDROID_STATISTICS_HOT_PIXEL_MAP_MODE,
            &hotPixelMapMode, 1);

    // 'generate' the new camera_metadata_t pointer from the
    // CameraMetadata object
    mDefaultRequestTemplates[type] = settings.release();

    return mDefaultRequestTemplates[type];
}

status_t FelixCamera3v2::flushQueue(void) {

    {
        Mutex::Autolock f(mLock);

        if(mStatus == STATUS_ERROR) {
            return NO_INIT;
        }
        mProcessingThread->setFlushing(true);
        LOG_INFO("Flushing all pending requests");
    }
    // unlock mutex as processCaptureRequest might be waiting on mLock already

    // send fake signal in case requestCaptureRequest() is currently
    // waiting to push currently waiting request to the queue
    mProcessingThread->signalCaptureResultSent();

    Mutex::Autolock l(mLock);
    WaitUntilIdle();
    // at this moment all pending requests have been discarded
    mProcessingThread->setFlushing(false);

    // we've flushed the requests in ProcessingThread,
    // we also have to quickly flush them in HW
    mCameraHw.flushRequests();
    mStatus = STATUS_READY;

    return OK;
}

void FelixCamera3v2::initScalerCapabilities(CameraMetadata& info) {

    LOG_INFO("E");
    FelixCamera::initScalerCapabilities(info);

    uint32_t width=0, height=0;
    mCameraHw.getSensorResolution(width, height);
    // legacy

    // android.scaler.availableMinFrameDurations (static)
    // android.scaler.availableStallDurations (static)
    // android.scaler.availableStreamConfigurations (static)

    // declared locally to make things shorter
    enum {
        SC_OUT = ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT,
        SC_IN = ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_INPUT
    };

    Vector<int32_t> availableStreamConfigurations;
    Vector<int64_t> availableMinFrameDurations;
    Vector<int64_t> availableStallDurations;

    for(size_t i=0;i<HwCaps::kNumAvailableFormats;++i) {
        SensorResCaps_t const *table=NULL;
        int64_t minDurationNs;
        uint32_t size=0;
        uint32_t format = HwCaps::kAvailableFormats[i];
        bool canBeInput = true;
        if(format == HAL_PIXEL_FORMAT_BLOB) {
            table = mHwCapabilities->kAvailableJpegImageParams;
            size = mHwCapabilities->kNumAvailableJpegParams;
            canBeInput = false;
        } else if (format == HAL_PIXEL_FORMAT_RAW10 ||
                format == HAL_PIXEL_FORMAT_RAW16 ||
                format == HAL_PIXEL_FORMAT_RAW_OPAQUE ||
                format == HAL_PIXEL_FORMAT_RAW_SENSOR) {
            table = mHwCapabilities->kAvailableRawImageParams;
            size = mHwCapabilities->kNumAvailableRawParams;
            ALOG_ASSERT(true);
        } else {
            // everything else is a processed image (YUV, RGB)
            table = mHwCapabilities->kAvailableProcessedImageParams;
            size = mHwCapabilities->kNumAvailableProcessedParams;
        }
        ALOG_ASSERT(table);
        ALOG_ASSERT(size);
        for (size_t j=0;j<size;++j) {
            // do not advertise resolutions larger than sensor output
            // NOTE: unless we support high quality upscaling
            if(table->width>(int32_t)width || table->height>(int32_t)height){
                LOG_WARNING("Dimension %dx%d larger than native sensor but"\
                        "no upscaling defined",
                        table->width, table->height);
                ++table;
                continue;
            }
            if(format == HAL_PIXEL_FORMAT_BLOB) {
                minDurationNs = table->minDurationNs;
                availableStallDurations.push(format);
                availableStallDurations.push(table->width);
                availableStallDurations.push(table->height);
                availableStallDurations.push(minDurationNs);
            } else {
                SENSOR_MODE modeInfo;
                IMG_RESULT res = mCameraHw.getSensor()->getMode(
                    table->sensorMode, modeInfo);
                ALOG_ASSERT(res == IMG_SUCCESS,
                        "Unsupported sensor mode %d defined in HwCaps",
                        table->sensorMode);
                minDurationNs = seconds_to_nanoseconds(1)/modeInfo.flFrameRate;
            }

            availableStreamConfigurations.push(format);
            availableStreamConfigurations.push(table->width);
            availableStreamConfigurations.push(table->height);
            availableStreamConfigurations.push(SC_OUT);

            availableMinFrameDurations.push(format);
            availableMinFrameDurations.push(table->width);
            availableMinFrameDurations.push(table->height);
            availableMinFrameDurations.push(minDurationNs);

            if(canBeInput) {
                availableStreamConfigurations.push(format);
                availableStreamConfigurations.push(table->width);
                availableStreamConfigurations.push(table->height);
                availableStreamConfigurations.push(SC_IN);
            }
            ++table;
        }
    }
    info.update(ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS,
        availableStreamConfigurations.array(),
        availableStreamConfigurations.size());
    info.update(ANDROID_SCALER_AVAILABLE_MIN_FRAME_DURATIONS,
        availableMinFrameDurations.array(),
        availableMinFrameDurations.size());
    info.update(ANDROID_SCALER_AVAILABLE_STALL_DURATIONS,
            availableStallDurations.array(),
            availableStallDurations.size());

    // android.scaler.croppingType (static)
    uint8_t type = ANDROID_SCALER_CROPPING_TYPE_FREEFORM;
    info.update(ANDROID_SCALER_CROPPING_TYPE, &type, 1);

    // full
    // android.scaler.availableInputOutputFormatsMap (static)
    // currently only reprocessing from yuv to JPEG allowed
    // : RAW
    int32_t availableInputOutputFormatsMap[] = {
        HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, 1, HAL_PIXEL_FORMAT_BLOB,
        HAL_PIXEL_FORMAT_YCbCr_420_888, 1, HAL_PIXEL_FORMAT_BLOB,
        HAL_PIXEL_FORMAT_YCbCr_422_SP, 1, HAL_PIXEL_FORMAT_BLOB
    };
    info.update(ANDROID_SCALER_AVAILABLE_INPUT_OUTPUT_FORMATS_MAP,
        availableInputOutputFormatsMap,
        ARRAY_NUM_ELEMENTS(availableInputOutputFormatsMap));

}

bool FelixCamera3v2::isFlushing(void) {
    if(!mProcessingThread.get()) return false;

    return mProcessingThread->isFlushing();
}

int FelixCamera3v2::flush(const struct camera3_device * d) {
    FelixCamera3v2* ec = static_cast<FelixCamera3v2*>(getInstance(d));
    return ec->flushQueue();
}

camera3_device_ops_t FelixCamera3v2::sDeviceOps = {
    FelixCamera::initialize,
    FelixCamera::configure_streams,
    NULL,
    FelixCamera::construct_default_request_settings,
    FelixCamera::process_capture_request,
    NULL/*FelixCamera::get_metadata_vendor_tag_ops*/,
    FelixCamera::dump,
    FelixCamera3v2::flush,   // flush
    { NULL }    // reserved
};

status_t FelixCamera3v2::constructStaticInfo(void) {

    status_t res = FelixCamera::constructStaticInfo();

    // we have to extend base request settings with new, related to camera2 api
    CameraMetadata info(mCameraStaticInfo);
    mCameraStaticInfo = NULL;

    const uint8_t availableHotPixelModes[] = {
        ANDROID_HOT_PIXEL_MODE_OFF,
        ANDROID_HOT_PIXEL_MODE_HIGH_QUALITY,
    };
    info.update(ANDROID_HOT_PIXEL_AVAILABLE_HOT_PIXEL_MODES,
            availableHotPixelModes,
            ARRAY_NUM_ELEMENTS(availableHotPixelModes));

    const uint8_t availableToneMapModes[] = {
        ANDROID_TONEMAP_MODE_FAST,
        ANDROID_TONEMAP_MODE_HIGH_QUALITY,
    };
    info.update(ANDROID_TONEMAP_AVAILABLE_TONE_MAP_MODES,
            availableToneMapModes,
            ARRAY_NUM_ELEMENTS(availableToneMapModes));

    const uint8_t availableEdgeModes[] = {
        ANDROID_EDGE_MODE_OFF,
        ANDROID_EDGE_MODE_HIGH_QUALITY,
    };
    info.update(ANDROID_EDGE_AVAILABLE_EDGE_MODES,
            availableEdgeModes,
            ARRAY_NUM_ELEMENTS(availableEdgeModes));

    const uint8_t availableAberrationModes[] = {
        ANDROID_COLOR_CORRECTION_ABERRATION_MODE_OFF,
        ANDROID_COLOR_CORRECTION_ABERRATION_MODE_HIGH_QUALITY,
    };
    info.update(ANDROID_COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES,
            availableAberrationModes,
            ARRAY_NUM_ELEMENTS(availableAberrationModes));

    const uint8_t availableNoiseReductionModes[] = {
        ANDROID_NOISE_REDUCTION_MODE_HIGH_QUALITY,
        //ANDROID_NOISE_REDUCTION_MODE_OFF
    };
    info.update(ANDROID_NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES,
            availableNoiseReductionModes,
            ARRAY_NUM_ELEMENTS(availableNoiseReductionModes));

    // lens
    const uint8_t focusDistanceCalibration =
            ANDROID_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_APPROXIMATE;
    info.update(ANDROID_LENS_INFO_FOCUS_DISTANCE_CALIBRATION,
            &focusDistanceCalibration, 1);

    // sensor
    const int32_t availableSensorTestPatternModes[] = {
        ANDROID_SENSOR_TEST_PATTERN_MODE_OFF
    };
    info.update(ANDROID_SENSOR_AVAILABLE_TEST_PATTERN_MODES,
            availableSensorTestPatternModes,
            ARRAY_NUM_ELEMENTS(availableSensorTestPatternModes));
    const uint8_t timestampSource =
            ANDROID_SENSOR_INFO_TIMESTAMP_SOURCE_REALTIME;
    info.update(ANDROID_SENSOR_INFO_TIMESTAMP_SOURCE,
            &timestampSource, 1);


    const uint8_t availableCapabilities[] = {
        ANDROID_REQUEST_AVAILABLE_CAPABILITIES_BACKWARD_COMPATIBLE,
        //ANDROID_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_SENSOR,
        //ANDROID_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_POST_PROCESSING,
        //ANDROID_REQUEST_AVAILABLE_CAPABILITIES_RAW,
        ANDROID_REQUEST_AVAILABLE_CAPABILITIES_READ_SENSOR_SETTINGS,
        //ANDROID_REQUEST_AVAILABLE_CAPABILITIES_BURST_CAPTURE
    };
    info.update(ANDROID_REQUEST_AVAILABLE_CAPABILITIES,
            availableCapabilities,
            ARRAY_NUM_ELEMENTS(availableCapabilities));

    // regular capture : expose+readout = 2
    // jpeg capture: regular capture + jpeg compress = 3
    const uint8_t pipelineMaxDepth = 3;
    info.update(ANDROID_REQUEST_PIPELINE_MAX_DEPTH, &pipelineMaxDepth, 1);

    const int32_t maxLatency = ANDROID_SYNC_MAX_LATENCY_UNKNOWN;
    info.update(ANDROID_SYNC_MAX_LATENCY, &maxLatency, 1);

    const int32_t maxNumInputStreams = 1;;
    info.update(ANDROID_REQUEST_MAX_NUM_INPUT_STREAMS,
            &maxNumInputStreams, 1);

    const int32_t partialResultCount = 1;;
        info.update(ANDROID_REQUEST_PARTIAL_RESULT_COUNT,
                &partialResultCount, 1);

    info.update(ANDROID_REQUEST_AVAILABLE_REQUEST_KEYS,
            mSupportedRequestKeys,
            ARRAY_NUM_ELEMENTS(mSupportedRequestKeys));

    info.update(ANDROID_REQUEST_AVAILABLE_RESULT_KEYS,
            mSupportedResultKeys,
            ARRAY_NUM_ELEMENTS(mSupportedResultKeys));

    // android.request.availableCharacteristicsKeys
    // due to the runtime generation, the field below has to be filled
    // at the very end of this function
    const camera_metadata_t *ro_data = info.getAndLock();
    extractKeysFromRawMetadata(ro_data, mSupportedCharacteristicsKeys);
    info.unlock(ro_data);
    mSupportedCharacteristicsKeys.push(
        ANDROID_REQUEST_AVAILABLE_CHARACTERISTICS_KEYS);
    info.update(ANDROID_REQUEST_AVAILABLE_CHARACTERISTICS_KEYS,
            mSupportedCharacteristicsKeys.array(),
            mSupportedCharacteristicsKeys.size());

    // 'generate' the new camera_metadata_t pointer from the
    // CameraMetadata object
    mCameraStaticInfo = info.release();

    return res;
}

status_t FelixCamera3v2::extractKeysFromRawMetadata(const camera_metadata_t* metadata,
            Vector<int32_t>& output) {

    if(!metadata) {
        return BAD_VALUE;
    }
    for(uint32_t section = 0; section < ANDROID_SECTION_COUNT; ++section) {
        uint32_t first = camera_metadata_section_bounds[section][0];
        uint32_t last = camera_metadata_section_bounds[section][1];
        for(uint32_t key=first; key<=last; ++key) {
            camera_metadata_ro_entry entry;
            if(find_camera_metadata_ro_entry(metadata, key, &entry) == 0){
                output.push(key);
            }
        }
    }
    return OK;
}

}; // namespace android
