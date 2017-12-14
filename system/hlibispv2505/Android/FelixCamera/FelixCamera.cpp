/**
******************************************************************************
@file FelixCamera.cpp

@brief Definition of main FelixCamera class in v2500 HAL module

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

#include "ui/Rect.h"
#include "ui/GraphicBufferMapper.h"
#include "ui/GraphicBufferAllocator.h"

#include "ispc/ParameterList.h"
#include "ispc/ParameterFileParser.h"

#undef UPSCALING_ENABLED /* needs GPU support */

namespace android {

//
// Constants for camera capabilities
//

// static global HAL configuration parameters

const int32_t FelixCamera::kHistogramBucketCount = HIS_GLOBAL_BINS;
const int32_t FelixCamera::kMaxHistogramCount = 1000;

// ----------------------------------------------------------------------------
// Camera device lifecycle methods
// ----------------------------------------------------------------------------

// Felix camera constructor
// The common field comes from the camera3_device FelixCamera inherits from
FelixCamera::FelixCamera(int cameraId, bool facingBack,
        struct hw_module_t *module) :
    mFacingBack(facingBack),
    mCaptureStarted(false),
    mInputStream(NULL),
    mPoolInitialized(false),
    mFrameSettings(*this),
    mGPU(NULL),
    mAfSupported(false),
    mDefaultConfig(),
    mStatus(STATUS_ERROR)
    {

    mGPUstreamCurrentIndex = -1;
    mGPUstream.clear();
    // Common header for hw device
    common.tag          = HARDWARE_DEVICE_TAG;
    common.version      = CAMERA_DEVICE_API_VERSION_3_0;
    common.module       = module; //HW module this device is created from
    common.close        = FelixCamera::close;      //close function

    // class members initialization
    mCameraStaticInfo         = NULL;
    mCameraID           = cameraId; // 0 or 1 (back/front)
    //----------------------------------------------------

    LOG_INFO("Constructing FelixCamera (ID: %d) facing %s",
            mCameraID, facingBack ? "back" : "front");

    for (size_t i = 0; i < CAMERA3_TEMPLATE_COUNT; i++) {
        mDefaultRequestTemplates[i] = NULL;
    }

    //
    // Front cameras = limited mode
    // Back cameras = full mode
    //
    mFullMode = facingBack;

    ops = &sDeviceOps;

    mCallbackOps = NULL;

    mVendorTagOps.get_camera_vendor_section_name =
        FelixCamera::get_camera_vendor_section_name;
    mVendorTagOps.get_camera_vendor_tag_name =
        FelixCamera::get_camera_vendor_tag_name;
    mVendorTagOps.get_camera_vendor_tag_type =
        FelixCamera::get_camera_vendor_tag_type;
    mVendorTagOps.parent = this;
}

const char* FelixCamera::get_camera_vendor_section_name(
        const vendor_tag_query_ops_t *v, uint32_t tag) {
    FelixCamera* ec = static_cast<const TagOps*>(v)->parent;
    return ec->getVendorSectionName(tag);
}

const char* FelixCamera::get_camera_vendor_tag_name(
        const vendor_tag_query_ops_t *v, uint32_t tag) {
    FelixCamera* ec = static_cast<const TagOps*>(v)->parent;
    return ec->getVendorTagName(tag);
}

int FelixCamera::get_camera_vendor_tag_type(
        const vendor_tag_query_ops_t *v, uint32_t tag)  {
    FelixCamera* ec = static_cast<const TagOps*>(v)->parent;
    return ec->getVendorTagType(tag);
}

FelixCamera::~FelixCamera() {
    LOG_DEBUG("E");
    closeCamera();
    // Free created camera setting templates
    for (size_t i = 0; i < CAMERA3_TEMPLATE_COUNT; i++) {
        if (mDefaultRequestTemplates[i] != NULL) {
            free_camera_metadata(mDefaultRequestTemplates[i]);
        }
    }
}

void FelixCamera::dumpStreamInfo(const camera3_stream_t *stream) {
    if (!stream)
        LOG_ERROR("stream is NULL!");
    else {
        LOG_DEBUG("----------------------------------------------------------");
        LOG_DEBUG("STREAM INFO:");
        LOG_DEBUG("----------------------------------------------------------");
        LOG_DEBUG("ptr = %p type = %d format = 0x%x, usage = 0x%x w = %d h = %d",
                stream, stream->stream_type, stream->format, stream->usage, stream->width,
                stream->height);
        LOG_DEBUG("----------------------------------------------------------");
    }
}



// This is called from the module device_open function (in FelixCameraHAL) and
// must return the device struct with all the information filled in.
status_t FelixCamera::connectCamera(hw_device_t** device) {
    LOG_DEBUG("E");
    Mutex::Autolock l(mLock);

    if (mStatus == STATUS_ERROR) {
        return NO_INIT;
    }

    if (device == NULL) {
        return BAD_VALUE;
    }

    if (mStatus != STATUS_CLOSED) {
        LOG_ERROR("Can't connect in state %d", mStatus);
        return INVALID_OPERATION;
    }

    // Create captures processing thread...
    ALOG_ASSERT((ProcessingThread*)NULL == mProcessingThread.get(),
        "%s: mProcessingThread not NULL", __FUNCTION__);
    mProcessingThread = new ProcessingThread(*this);
    if (!mProcessingThread.get()) {
        LOG_ERROR("Allocation failed!");
        return INVALID_OPERATION;
    }

    /* Instantiate hardware manager object with 2 pipelines per instance
     * : this value should be dependent on the hardware
     * hardware up to 2.x should use at least 2 */

    if (mCameraHw.createHwPipelines(2)!=NO_ERROR ||
        // setup modules using default ISPC parameters
        mCameraHw.programParameters()!=NO_ERROR ||
        mCameraHw.initControlModules()!=NO_ERROR ||
        // disableAllOutputs() ensures no outputs are enabled before first
        // call of configureStreams()
        mCameraHw.disableAllOutputs()!=NO_ERROR)
    {
        LOG_ERROR("Unable to instantiate camera hardware pipelines");
        return deviceError();
    }
    mFrameSettings.clear();

    // common is declared automatically as this class inherits from
    // camera3_device_t
    *device = &common;
    mStatus = STATUS_OPEN;
    return NO_ERROR;
}

// Static callback function which is available to the framework from the device
// structure filled in the connectCamera function.
int FelixCamera::close(struct hw_device_t* device) {
    // Get a pointer to the FelixCamera instance corresponding to the device
    // and invoke the closeCamera method.
    FelixCamera* ec = static_cast<FelixCamera*>(
            reinterpret_cast<camera3_device_t*>(device));

    if (ec == NULL) {
        LOG_ERROR("Unexpected NULL FelixCamera device");
        return BAD_VALUE;
    }
    return ec->closeCamera();
}

status_t FelixCamera::reconfigureCameraGPU(camera3_stream_t const * const newStream) {

    LOG_DEBUG("line %d Encoder, format %d size %dx%d newStream->usage %d", __LINE__, newStream->format, newStream->width, newStream->height, newStream->usage);
    if( ( HAL_PIXEL_FORMAT_YCrCb_420_SP              == newStream->format ) ||
        ( HAL_PIXEL_FORMAT_YCbCr_420_888             == newStream->format ) ||
        ( HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED    == newStream->format   &&
          isEncoderUsage(newStream->usage)))
    {
        if(mGPU.get())       mGPU.clear();
        LOG_DEBUG("line %d Encoder", __LINE__);
        mGPU = new CameraGpu(   newStream->width, newStream->height,
                                0, 0);
    }
    if( ( HAL_PIXEL_FORMAT_RGBX_8888                == newStream->format )||
        ( HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED   == newStream->format  &&
          isDisplayUsage(newStream->usage)))
    {
        if(mGPU.get())       mGPU.clear();
        LOG_DEBUG("line %d Display", __LINE__);
        mGPU = new CameraGpu(   0, 0,
                                newStream->width, newStream->height);
    }
    return NO_ERROR;
}
// Non static method for closing the camera. Called from the static close
// function which pointer is filled in in the device struct corresponding
// to the camera.
status_t FelixCamera::closeCamera() {
    LOG_DEBUG("E");

    Mutex::Autolock l(mLock);
    if (mStatus == STATUS_CLOSED) {
        return NO_ERROR;
    }

    LOG_DEBUG("Cleaning processing thread...");
    WaitUntilIdle();

    mProcessingThread->requestExit();
    mProcessingThread->join();
    mProcessingThread.clear();
    LOG_DEBUG("Processing thread cleaned.");

    stopCapture();

    {
        mNonZslOutputBuffer.clear();
        // Clear out private stream information
        for (StreamIterator s = mStreams.begin(); s != mStreams.end(); s++) {
            PrivateStreamInfo *privStream = *s;
            delete privStream;
        }
        mStreams.clear();
    }

    mGPU.clear();

    mCameraHw.clearStreams();
    mCameraHw.destroyHwPipelines();

    mPoolInitialized = false;
    mFrameSettings.clear();

    LOG_DEBUG("camera closed.");
    mStatus = STATUS_CLOSED;
    return NO_ERROR;
}

status_t FelixCamera::getCameraInfo(struct camera_info *info) {

    info->facing = mFacingBack ? CAMERA_FACING_BACK : CAMERA_FACING_FRONT;
    info->orientation = 0;

    info->device_version = common.version;
    info->static_camera_characteristics = mCameraStaticInfo;

    return NO_ERROR;
}

// ----------------------------------------------------------------------------
// Protected API. Callbacks to the framework.
// ----------------------------------------------------------------------------

void FelixCamera::sendCaptureResult(camera3_capture_result_t *result) {
    LOG_DEBUG("Sending capture result for frame #%d...",
            result->frame_number);
    mCallbackOps->process_capture_result(mCallbackOps, result);
    LOG_DEBUG("Capture result sent for frame #%d...",
            result->frame_number);
}

void FelixCamera::sendNotify(camera3_notify_msg_t *msg) {
    mCallbackOps->notify(mCallbackOps, msg);
}

void FelixCamera::notifyShutter(const uint32_t frameNumber,
        const nsecs_t timestamp) {
    camera3_notify_msg_t msg;
    msg.type = CAMERA3_MSG_SHUTTER;
    msg.message.shutter.frame_number = frameNumber;
    msg.message.shutter.timestamp = timestamp;
    sendNotify(&msg);
}

void FelixCamera::notifyError(const uint32_t frameNumber,
        camera3_stream_t * const stream,
        const camera3_error_msg_code_t code) {
    camera3_notify_msg_t msg;
    msg.type = CAMERA3_MSG_ERROR;
    msg.message.error.frame_number = frameNumber;
    msg.message.error.error_stream = stream;
    msg.message.error.error_code = code;
    sendNotify(&msg);
}

void FelixCamera::notifyFatalError(void) {
    notifyError(0, NULL, CAMERA3_MSG_ERROR_DEVICE);
    mStatus = STATUS_ERROR;
}

void FelixCamera::notifyRequestError(const uint32_t frameNumber,
        camera3_stream_t * const stream) {
    notifyError(frameNumber, stream, CAMERA3_MSG_ERROR_REQUEST);
}

void FelixCamera::notifyMetadataError(const uint32_t frameNumber,
        camera3_stream_t * const stream) {
    notifyError(frameNumber, stream, CAMERA3_MSG_ERROR_RESULT);
}

void FelixCamera::notifyBufferError(const uint32_t frameNumber,
        camera3_stream_t * const stream) {
    notifyError(frameNumber, stream, CAMERA3_MSG_ERROR_BUFFER);
}

// This is not the initialization function in the camera3_device_ops_t.
// This is called from the FelixCameraHAL after instantiating the FelixCamera.
status_t FelixCamera::Initialize(
    const std::string& sensorName,
    const std::string& defaultConfig,
    const uint8_t sensorMode,
    const int flip,
    const bool externalDG) {
    LOG_DEBUG("E");
    status_t res = OK;

    if (mStatus != STATUS_ERROR) {
        LOG_ERROR("Already initialized!");
        return INVALID_OPERATION;
    }

    if(!mCameraHw.statusOk()) {
        LOG_ERROR("Camera HW not initialized");
        return NO_INIT;
    }
    mDefaultConfig = defaultConfig;

    // load default ISPC parameters from external file
    mCameraHw.loadParametersFromFile(mDefaultConfig);

    if(!externalDG) {
        res = mCameraHw.initSensor(sensorName,
                sensorMode,
                (SENSOR_FLIPPING)flip);
    } else {
        res = mCameraHw.initDgSensor(sensorName, sensorMode, externalDG);
    }

    if(res!=OK) {
        LOG_ERROR("initSensor() failed");
        return res;
    }

    mAfSupported = mCameraHw.getSensor()->getFocusSupported();
    LOG_DEBUG("Sensor support AF %s", mAfSupported ? "enabled" : "disabled");

    mHwCapabilities = new HwCaps(sensorName, sensorMode, mCameraHw);
    if(!mHwCapabilities.get()) {
        LOG_ERROR("Unable to allocate HW capabilities");
        mStatus = STATUS_ERROR;
        return NO_INIT;
    }
    res = constructStaticInfo();
    if (res != OK) {
        LOG_ERROR("Unable to allocate static info: %s (%d)",
                strerror(-res), res);
        mStatus = STATUS_ERROR;
    } else {
        mStatus = STATUS_CLOSED;
    }
    return res;
}


// ----------------------------------------------------------------------------
// These are the methods that are part of the camera3_device_ops_t
// ----------------------------------------------------------------------------

//
// Camera3 interface methods
//

// This is called from camera3_device_ops_t->initialize once the camera is
// already instantiated by the FelixCameraHAL.
// This is called from the framework only one time after opening the device.
// It is used to pass the the framework callback function pointers to the HAL
status_t FelixCamera::initializeDevice(
        const camera3_callback_ops *callbackOps) {

    if (callbackOps == NULL) {
        LOG_ERROR("NULL callback ops provided to HAL!");
        return BAD_VALUE;
    }

    if (mStatus != STATUS_OPEN) {
        LOG_ERROR("Trying to initialize a camera in state %d!",
                mStatus);
        return INVALID_OPERATION;
    }

    mCallbackOps = callbackOps;
    mStatus = STATUS_READY;

    return NO_ERROR;
}

// This function resets the HAL camera device processing pipeline and set up
// new input and output streams. Must be called at least once after
// initialize() before a request is submitted with process_capture_request()
// This function will be called if no captures are being processed: all
// in-flight input/output buffers have been returned and their release sync
// fences have been signalled by the HAL.
status_t FelixCamera::configureStreams(
        camera3_stream_configuration *streamList) {
    Mutex::Autolock l(mLock);

    if (mStatus == STATUS_ERROR) {
        return NO_INIT;
    }

    WaitUntilIdle();

    if (mStatus != STATUS_READY) {
        LOG_ERROR("Cannot configure streams in state %d",
                mStatus);
        return INVALID_OPERATION;
    }

    if (streamList == NULL || streamList->streams == NULL) {
        LOG_ERROR("NULL stream configuration");
        return BAD_VALUE;
    }
    LOG_DEBUG("%d streams", streamList->num_streams);

    PrivateStreamInfo *privStream;

    // We iterate through all the streams in the list looking for input streams
    camera3_stream_t *inputStream = NULL;
    for (size_t i = 0; i < streamList->num_streams; ++i) {
        camera3_stream_t *newStream = streamList->streams[i];

        // If one of the input streams is NULL we have an error.
        if (newStream == NULL) {
            LOG_ERROR("Stream index %d was NULL", i);
            return BAD_VALUE;
        }

        // We have an input (or bidirectional) stream.
        // Only one input stream is allowed
        if (newStream->stream_type == CAMERA3_STREAM_INPUT ||
            newStream->stream_type == CAMERA3_STREAM_BIDIRECTIONAL) {
            if (inputStream != NULL) {
                LOG_ERROR("Multiple input streams requested!");
                return BAD_VALUE;
            }
            inputStream = newStream; // We have a variable (and not an array)
                                     // to store an input stream (there can be
                                     // only one).

            dumpStreamInfo(inputStream);
        }

        // Check the stream is one of the formats that we support in
        // FelixCamera
        bool validFormat = false;
        for (size_t f = 0; f < HwCaps::kNumAvailableFormats; f++) {
            if (newStream->format == HwCaps::kAvailableFormats[f]) {
                validFormat = true;
                break;
            }
        }
        privStream = streamToPriv(newStream);
        LOG_DEBUG("Stream #%d (%dx%d): type = %d, format = 0x%x (valid = %d), "
                "usage = %#x, registered=%s, ptr=%p", i,
                newStream->width, newStream->height, newStream->stream_type,
                newStream->format, validFormat, newStream->usage,
                privStream ? (privStream->registered ? "true" : "false") :
                "NULL", newStream);

        if (!validFormat) {
            LOG_ERROR("Unsupported stream format 0x%x requested",
                    newStream->format);
            return BAD_VALUE;
        }
    }

    // We set the class member input stream to the value in inputStream
    // (initialized with an input stream in the list if available)
    mInputStream = inputStream;

    // stop the context before applying any changes
    stopCapture();

    // disconnect all existing streams from all available pipelines
    for (StreamIterator s = mStreams.begin(); s != mStreams.end(); ++s) {
        privStream = *s;
        if(!privStream) {
            LOG_ERROR("priv ==NULL field in stream!");
            return deviceError();
        }
        privStream->alive = false;
        ALOG_ASSERT(privStream->getParent());
        // detach all streams from the HW pipelines
        mCameraHw.detachStream(privStream->getParent());
    }

    // Find new streams and mark still-alive ones
    //
    camera3_stream_t *nonZslBufferCandidate = NULL;
    camera3_stream_t *blobBufferCandidate = NULL;
    // detach and deallocate before reconfiguring
    mNonZslOutputBuffer.clear();

    for (size_t i = 0; i < streamList->num_streams; ++i) {
        camera3_stream_t *newStream = streamList->streams[i];

        CI_BUFFTYPE type = CI_TYPE_NONE;
        // attach provided stream to first available output
        // of available pipeline
        cameraHw_t* cam = mCameraHw.attachStream(newStream, type);

        // If newStream->priv != NULL we had registered it before
        if (newStream->priv == NULL) {

            // New stream
            privStream = new PrivateStreamInfo(newStream);
            if (!privStream) {
                LOG_ERROR("Allocation failed!");
                return deviceError();
            }

            // internal type of stream we obtained from attachStream()
            privStream->ciBuffType = type;
            privStream->alive = true;

#ifdef USE_GPU_LIB
            LOG_DEBUG("mGPU config stream newStream = %p, newStream->stream_type = %d, privStream = %p", newStream, newStream->stream_type, privStream);
            if((CAMERA3_STREAM_OUTPUT           == newStream->stream_type) ||
               (CAMERA3_STREAM_BIDIRECTIONAL    == newStream->stream_type))
            {
                // list all streams
//                LOG_DEBUG("line %d mGPUstream.size = %d", __LINE__, mGPUstream.size());
//                for(int x=0; x<mGPUstream.size(); x++) {
//                    LOG_DEBUG("x = %d, mGPUstream.itemAt(x).stream = %p, mGPUstream.itemAt(x).target = %d, mGPUstream.itemAt(x).requestedCount = %d, mGPUstream.itemAt(x).usedCount = %d",
//                                x, mGPUstream.itemAt(x).stream, mGPUstream.itemAt(x).target, mGPUstream.itemAt(x).requestedCount, mGPUstream.itemAt(x).usedCount);
//                }

                int streamIndex = -1;
                GPUstreamInfo_t gpuStreamInfo;
                dumpStreamInfo(newStream);
                gpuStreamInfo.stream    = newStream;
                gpuStreamInfo.requestedCount    = 0;
                gpuStreamInfo.usedCount         = 0;
                gpuStreamInfo.requested         = false;

                if( ( HAL_PIXEL_FORMAT_YCbCr_420_888             == newStream->format )||
                    ( HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED    == newStream->format  &&
                      isEncoderUsage(newStream->usage)))
                {
                    LOG_DEBUG("%d mGPU New Encoder Stream = %p", __LINE__, newStream);
                    // this is best candidate for vision libraries processing
                    gpuStreamInfo.target = GPU_STREAM_ENCODER;
                    mGPUstreamCurrentIndex = streamIndex = mGPUstream.add(gpuStreamInfo);
                    reconfigureCameraGPU(mGPUstream.itemAt(mGPUstreamCurrentIndex).stream);
                }

                if( ( HAL_PIXEL_FORMAT_YCrCb_420_SP             == newStream->format ) ||
                    ( HAL_PIXEL_FORMAT_RGBX_8888                == newStream->format ) ||
                    ( HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED   == newStream->format   &&
                      isDisplayUsage(newStream->usage)))
                {
                    // preview, second best candidate
                    gpuStreamInfo.target = ( HAL_PIXEL_FORMAT_YCrCb_420_SP == newStream->format ) ? GPU_STREAM_PREVIEW_YUV : GPU_STREAM_PREVIEW_RGB;
                    streamIndex = mGPUstream.add(gpuStreamInfo);
                    if(-1 == mGPUstreamCurrentIndex) {
                        LOG_DEBUG("%d mGPU New Display Stream = %p", __LINE__, newStream);
                        mGPUstreamCurrentIndex = streamIndex;
                        reconfigureCameraGPU(mGPUstream.itemAt(mGPUstreamCurrentIndex).stream);
                    } else {
                        if(mGPUstream.itemAt(streamIndex).target >=  mGPUstream.itemAt(mGPUstreamCurrentIndex).target) {
                            LOG_DEBUG("%d mGPU New Display Stream = %p", __LINE__, newStream);
                            mGPUstreamCurrentIndex = streamIndex;
                            reconfigureCameraGPU(mGPUstream.itemAt(mGPUstreamCurrentIndex).stream);
                        }
                    }
                }
                // list all streams
//                LOG_DEBUG("%d mGPUstream.size = %d", __LINE__, mGPUstream.size());
//                for(int x=0; x<mGPUstream.size(); x++) {
//                    LOG_DEBUG("%d x = %d, mGPUstream.itemAt(x).stream = %p, mGPUstream.itemAt(x).target = %d, mGPUstream.itemAt(x).requestedCount = %d, mGPUstream.itemAt(x).usedCount = %d",
//                                __LINE__, x, mGPUstream.itemAt(x).stream, mGPUstream.itemAt(x).target, mGPUstream.itemAt(x).requestedCount, mGPUstream.itemAt(x).usedCount);
//                }
            }
#endif

            switch (newStream->stream_type) {
            case CAMERA3_STREAM_OUTPUT:
                if(cam) {
                    // hardware support for provided stream
                    privStream->usage = newStream->usage |
                        GRALLOC_USAGE_HW_CAMERA_WRITE;
                    newStream->max_buffers = FelixCamera::kMaxBufferCount;
                } else if (newStream->format == HAL_PIXEL_FORMAT_BLOB) {
                    // BLOB not supported by HW
                    blobBufferCandidate = newStream;
                    privStream->usage = newStream->usage |
                        GRALLOC_USAGE_HW_CAMERA_WRITE |
                        GRALLOC_USAGE_SW_WRITE_RARELY;
                    newStream->max_buffers = 1;
                } else {
                    LOG_ERROR("Not supported stream: type = %d usage = "
                            "%#x format = %d!",
                            newStream->stream_type, newStream->usage,
                            newStream->format);
                    return deviceError();
                }
                break;
            case CAMERA3_STREAM_INPUT:
                privStream->usage = newStream->usage |
                    GRALLOC_USAGE_HW_CAMERA_READ |
                    GRALLOC_USAGE_SW_READ_RARELY;
                newStream->max_buffers = 1;
                // : add support for RAW reprocess using internal DG
                break;
            case CAMERA3_STREAM_BIDIRECTIONAL:
                if (cam) {
                    // use the stream only if explicit ZSL usage
                    // has been set by the framework
                    if(isZslUsage(newStream->usage)) {
                        nonZslBufferCandidate = newStream;
                    }
                } else {
                    LOG_ERROR("Not supported stream: type = %d usage = "
                        "%#x format = %d!",
                        newStream->stream_type, newStream->usage,
                        newStream->format);
                    return deviceError();
                }
                // : add support for RAW reprocess using internal DG
                privStream->usage = newStream->usage |
                    GRALLOC_USAGE_HW_CAMERA_READ |
                    GRALLOC_USAGE_HW_CAMERA_WRITE;
                newStream->max_buffers = FelixCamera::kMaxBufferCount;
                break;
            }
            // update the flags with our producer flags
#if CAMERA_DEVICE_API_VERSION_CURRENT >= HARDWARE_DEVICE_API_VERSION(3, 2)
            // HAL3.2 allows updating usage field to reflect HAL requirements
            newStream->usage = privStream->usage;
#endif
            newStream->priv = privStream;
            // Add the stream to the list.
            mStreams.push_back(privStream);
        } else {
            // Existing stream, mark as still alive.
            privStream = streamToPriv(newStream);
            ALOG_ASSERT(privStream, "priv ==NULL field in stream!");

            privStream->alive = true;
            if (newStream->format == HAL_PIXEL_FORMAT_BLOB) {
                blobBufferCandidate = newStream;
            } else if(newStream->stream_type == CAMERA3_STREAM_BIDIRECTIONAL &&
                    isZslUsage(newStream->usage)) {
                nonZslBufferCandidate = newStream;
            }
        }
        privStream->setPipeline(cam);
        mCameraHw.scaleAndNoCrop(newStream);   // apply scaler output size
    }

    for (StreamIterator s = mStreams.begin(); s != mStreams.end();) {
        PrivateStreamInfo *privStream = *s;
        if(!privStream) {
            LOG_ERROR("priv ==NULL field in stream!");
            return BAD_VALUE;
        }
        // deallocate all stream buffers from the pipeline prior to
        // reconfiguration of the pipeline
        LOG_DEBUG("Deregistering all buffers from stream %p", *s);
        // Reap the dead streams
        if (!privStream->alive) {
#ifdef USE_GPU_LIB
            for(int x=0; x<mGPUstream.size(); x++) {
                if(mGPUstream.itemAt(x).stream == privStream->getParent()) {
                    if(mGPUstreamCurrentIndex == x) {
                        mGPUstreamCurrentIndex = -1;
                    } else {
                        if(mGPUstreamCurrentIndex > x) {
                            mGPUstreamCurrentIndex--;
                        }
                    }
                    mGPUstream.removeAt(x);
                }
            }
#endif
            // destruction of private stream will deregister
            // the buffers as well
            delete privStream;
            // remove the stream to camera pipeline association
            s = mStreams.erase(s);
        } else {
            privStream->deregisterBuffers();
#if CAMERA_DEVICE_API_VERSION_CURRENT >= HARDWARE_DEVICE_API_VERSION(3, 2)
            // for 3v2 we flush all buffers completely
            // the buffers will be re-imported in processCaptureRequest()
            // because they can be completely new buffers
            privStream->clear();
            privStream->registered = false; // every stream will be deregistered
#endif
            ++s;
        }
    }

    // allocate non Zsl buffer only if BLOB stream configured
    if(blobBufferCandidate) {
        ALOG_ASSERT(NULL == mNonZslOutputBuffer.get(),
                "mNonZslOutputBuffer not NULL");
        if(nonZslBufferCandidate) {
            LOG_DEBUG("Using BIDIRECTIONAL stream as a template for nonZsl");
            // even if ZSL stream has been configured, we still need to have
            // nonZsl buffer handle for non Zsl captures which can occur

            // this stream has been already attached to one pipeline
            mNonZslOutputBuffer =
                    new localStreamBuffer(*nonZslBufferCandidate);
        } else {
            LOG_DEBUG("Using BLOB stream as a template for nonZsl");
            // there are no planned ZSL captures
            // allocate mNonZslEncOutputBuffer using blobBufferCandidate
            // resolution and default camera format for YUV
            // : allocate this buffer using ANY HW output available
            // even RGB (will be converted in case of captureRequest)
            // conservation of YUV outputs needed by
            // android.hardware.camera2.cts.RobustnessTest#testMandatoryOutputCombinations
            uint32_t format;
            if(!mCameraHw.getFirstHwOutputAvailable(
                    blobBufferCandidate->width,
                    blobBufferCandidate->height,
                    format)) {
                LOG_ERROR("No available HW output for JPEG found.");
                return INVALID_OPERATION;
            }
            mNonZslOutputBuffer =
                new localStreamBuffer(
                        blobBufferCandidate->width,
                        blobBufferCandidate->height,
                        format,
                        GRALLOC_USAGE_HW_CAMERA_ZSL);
            if(mNonZslOutputBuffer.get()) {
                mNonZslOutputBuffer->attachToHw(mCameraHw);
                // setup scaler for this output
                mCameraHw.scaleAndNoCrop(mNonZslOutputBuffer->stream);
            }
        }
        if(NULL == mNonZslOutputBuffer.get()) {
            LOG_ERROR("Allocation failed");
            return deviceError();
        }
        LOG_DEBUG("current mNonZslOutputBuffer = %p",
                *mNonZslOutputBuffer->buffer);
    }

    // Can't reuse settings across configure call
    mFrameSettings.clear(); //Camera Settings / Camera Metadata

    // update global pipeline setup with new stream settings
    LOG_DEBUG("Setting up pipeline...");

    if(mCameraHw.setupModules() != NO_ERROR) {
        LOG_ERROR("Cannot set up modules!");
        return deviceError();
    }
    if(program()!= NO_ERROR) {
        LOG_ERROR("ISPC ERROR programming pipeline.");
        return deviceError();
    }

    if (!mPoolInitialized) {
        LOG_DEBUG("Adding pool of %d shots", FelixCamera::kMaxShotsCount);
        // do not add new shots to the pool if we have added them earlier
        // preallocate a pool of in-flight shots
        if(mCameraHw.addShots(FelixCamera::kMaxShotsCount) !=
                NO_ERROR) {
            LOG_ERROR("Failed to add pool of shots.");
            return deviceError();
        }
        mPoolInitialized = true;
    }
#if CAMERA_DEVICE_API_VERSION_CURRENT < HARDWARE_DEVICE_API_VERSION(3, 2)
    // for HAL3v0 we reimport the buffers for still alive streams
    // because registerStreamBuffers() won't be called second time for these
    for (StreamIterator s = mStreams.begin(); s != mStreams.end();) {
        PrivateStreamInfo *privStream = *s;
        if(!privStream) {
            LOG_ERROR("priv ==NULL field in stream!");
            return deviceError();
        }
        // import all stream buffers which were registered earlier
        if(privStream->registered &&
           privStream->registerBuffers()!=OK)
        {
            LOG_ERROR("Error while registering buffers");
            return deviceError();
        }
        // totally new buffers will be imported by registerStreamBuffers()
        ++s;
    }
#endif
    if(mNonZslOutputBuffer.get()) {
        // if allocated, register nonZsl buffer to the pipeline
        if (mNonZslOutputBuffer->registerBuffer() != IMG_SUCCESS) {
            LOG_ERROR("Importing mNonZsl buffer failed!");
            return deviceError();
        }
    }
    return OK;
}

// Register Buffers for a given stream with the HAL device. Called after
// configure_streams and before buffers are included in a capture request.
// If same stream is reused this function will not be called again.
// This method allows the HAL device to map or prepare the buffers for later
// use. Buffers passed in will be locked, at the end of the call all of them
// must be ready to be returned to the stream. This function allows the device
// to map the buffers or whatever other operation is required. The buffers
// passed will be already locked for use.
status_t FelixCamera::registerStreamBuffers(
        const camera3_stream_buffer_set *bufferSet) {
    LOG_DEBUG("E");

    // This lock will be automatically unlocked when leaving the context.
    Mutex::Autolock l(mLock);

    //
    // Sanity checks
    //
    if(mStatus == STATUS_ERROR) {
        return NO_INIT;
    }
    // OK: register streams at any time during configure (but only once per
    // stream). Check current camera state.
    if (mStatus != STATUS_READY || mStreams.size() == 0) {
        LOG_ERROR("Cannot register buffers in state %d",
                mStatus);
        return INVALID_OPERATION;
    }

    if (bufferSet == NULL || bufferSet->stream == NULL) {
        LOG_ERROR("NULL buffer set!");
        return BAD_VALUE;
    }

    dumpStreamInfo(bufferSet->stream);

    // Look for the stream this buffers correspond to.
    StreamIterator s = mStreams.begin();
    PrivateStreamInfo *privStream = NULL;
    for (; s != mStreams.end(); ++s) {
        privStream = *s;
        ALOG_ASSERT(privStream);
        if (bufferSet->stream == privStream->getParent()) break;
    }
    if (s == mStreams.end()) {
        LOG_ERROR("Trying to register buffers for a non-configured stream!");
        return BAD_VALUE;
    }

    if(!privStream) {
        LOG_ERROR("priv ==NULL field in stream!");
        return BAD_VALUE;
    }
    if (privStream->registered) {
        LOG_ERROR("Illegal to register buffer more than once");
        return BAD_VALUE;
    }

    if (privStream->ciBuffType == CI_TYPE_NONE) {
        LOG_DEBUG("Buffer does not need registration in HW");
        return OK;
    }
    status_t res = OK;
    if ((res = privStream->registerBuffers(bufferSet)) == OK) {
        privStream->registered = true;
    } else {
        LOG_ERROR("Failed to register buffers in stream private area!");
    }

    return res;
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
const camera_metadata_t* FelixCamera::constructDefaultRequestSettings(
        int type) {
    LOG_DEBUG("E (%d)", type);
    Mutex::Autolock l(mLock);

    if (mStatus < STATUS_OPEN) {
        LOG_ERROR("Invalid state");
        return NULL;
    }

    // Make sure this is a valid request
    if (type < 0 || type >= CAMERA3_TEMPLATE_COUNT) {
        LOG_ERROR("Unknown request settings template: %d", type);
        return NULL;
    }

    //
    // Cache is not just an optimization - pointer returned has to live at
    // least as long as the camera device instance does.
    //

    // We have to keep the response in memory until the device is closed
    if (mDefaultRequestTemplates[type] != NULL) {
        return mDefaultRequestTemplates[type];
    }

    FelixMetadata settings(*this);

    // android.request
    const uint8_t requestType = ANDROID_REQUEST_TYPE_CAPTURE;
    settings.update(ANDROID_REQUEST_TYPE, &requestType, 1);

    const uint8_t metadataMode = ANDROID_REQUEST_METADATA_MODE_FULL;
    settings.update(ANDROID_REQUEST_METADATA_MODE, &metadataMode, 1);

    const int32_t id = 0;
    settings.update(ANDROID_REQUEST_ID, &id, 1);

    const int32_t frameCount = 0;
    settings.update(ANDROID_REQUEST_FRAME_COUNT, &frameCount, 1);

    // android.lens
    const float focusDistance = 0;
    settings.update(ANDROID_LENS_FOCUS_DISTANCE, &focusDistance, 1);

    const float aperture = (float)mCameraHw.getSensor()->flAperture;
    settings.update(ANDROID_LENS_APERTURE, &aperture, 1);

    const float focalLength = (float)mCameraHw.getSensor()->uiFocalLength;
    settings.update(ANDROID_LENS_FOCAL_LENGTH, &focalLength, 1);

    const float filterDensity = 0;
    settings.update(ANDROID_LENS_FILTER_DENSITY, &filterDensity, 1);

    const uint8_t opticalStabilizationMode =
        ANDROID_LENS_OPTICAL_STABILIZATION_MODE_OFF;
    settings.update(ANDROID_LENS_OPTICAL_STABILIZATION_MODE,
        &opticalStabilizationMode, 1);

    // android.sensor
    const int64_t exposureTime = 10 * MSEC; // in ns
    settings.update(ANDROID_SENSOR_EXPOSURE_TIME, &exposureTime, 1);

    const int64_t frameDuration = 0; //kFrameDurationRange[0];
    settings.update(ANDROID_SENSOR_FRAME_DURATION, &frameDuration, 1);

    const int64_t timestamp = 0;
    settings.update(ANDROID_SENSOR_TIMESTAMP, &timestamp, 1);

    const int32_t sensitivity = 100;
    settings.update(ANDROID_SENSOR_SENSITIVITY, &sensitivity, 1);

    // android.flash
    const uint8_t flashMode = ANDROID_FLASH_MODE_OFF;
    settings.update(ANDROID_FLASH_MODE, &flashMode, 1);

    const uint8_t flashPower = 10;
    settings.update(ANDROID_FLASH_FIRING_POWER, &flashPower, 1);

    const int64_t firingTime = 0;
    settings.update(ANDROID_FLASH_FIRING_TIME, &firingTime, 1);
#if(0)
    // Processing block default modes
    uint8_t hotPixelMode = ANDROID_HOT_PIXEL_MODE_FAST;     // hw: DPF module
    uint8_t demosaicMode = ANDROID_DEMOSAIC_MODE_FAST;
    uint8_t noiseMode = ANDROID_NOISE_REDUCTION_MODE_FAST;  // hw: DNS module
    uint8_t shadingMode = ANDROID_SHADING_MODE_FAST;        // hw: LSH module
#if CAMERA_DEVICE_API_VERSION_CURRENT < HARDWARE_DEVICE_API_VERSION(3, 2)
    uint8_t geometricMode = ANDROID_GEOMETRIC_MODE_FAST;    // hw: LCA module
#endif
    uint8_t colorMode = ANDROID_COLOR_CORRECTION_MODE_FAST; // hw: CCM module
    uint8_t tonemapMode = ANDROID_TONEMAP_MODE_FAST;        // hw: TNM module
    uint8_t edgeMode = ANDROID_EDGE_MODE_FAST;              // hw: SHA module

    switch (type) {
        case CAMERA3_TEMPLATE_STILL_CAPTURE:
            // fall-through
        case CAMERA3_TEMPLATE_VIDEO_SNAPSHOT:
            // fall-through
        case CAMERA3_TEMPLATE_ZERO_SHUTTER_LAG:
            hotPixelMode = ANDROID_HOT_PIXEL_MODE_HIGH_QUALITY;
            demosaicMode = ANDROID_DEMOSAIC_MODE_HIGH_QUALITY;
            noiseMode = ANDROID_NOISE_REDUCTION_MODE_HIGH_QUALITY;
            shadingMode = ANDROID_SHADING_MODE_HIGH_QUALITY;
#if CAMERA_DEVICE_API_VERSION_CURRENT < HARDWARE_DEVICE_API_VERSION(3, 2)
            geometricMode = ANDROID_GEOMETRIC_MODE_HIGH_QUALITY;
#endif
            colorMode = ANDROID_COLOR_CORRECTION_MODE_HIGH_QUALITY;
            tonemapMode = ANDROID_TONEMAP_MODE_HIGH_QUALITY;
            edgeMode = ANDROID_EDGE_MODE_HIGH_QUALITY;
            break;
        case CAMERA3_TEMPLATE_PREVIEW:
            // fall-through
        case CAMERA3_TEMPLATE_VIDEO_RECORD:
            // fall-through
        default:
            break;
    }
#else
    uint8_t hotPixelMode = ANDROID_HOT_PIXEL_MODE_HIGH_QUALITY;     // hw: DPF module
    uint8_t demosaicMode = ANDROID_DEMOSAIC_MODE_HIGH_QUALITY;
    uint8_t noiseMode = ANDROID_NOISE_REDUCTION_MODE_HIGH_QUALITY;  // hw: DNS module
    uint8_t shadingMode = ANDROID_SHADING_MODE_HIGH_QUALITY;        // hw: LSH module
#if CAMERA_DEVICE_API_VERSION_CURRENT < HARDWARE_DEVICE_API_VERSION(3, 2)
    uint8_t geometricMode = ANDROID_GEOMETRIC_MODE_HIGH_QUALITY;    // hw: LCA module
#endif
    uint8_t colorMode = ANDROID_COLOR_CORRECTION_MODE_HIGH_QUALITY; // hw: CCM module
    uint8_t tonemapMode = ANDROID_TONEMAP_MODE_HIGH_QUALITY;        // hw: TNM module
    uint8_t edgeMode = ANDROID_EDGE_MODE_HIGH_QUALITY;              // hw: SHA module
#endif
    settings.update(ANDROID_HOT_PIXEL_MODE, &hotPixelMode, 1);
    settings.update(ANDROID_DEMOSAIC_MODE, &demosaicMode, 1);
    settings.update(ANDROID_NOISE_REDUCTION_MODE, &noiseMode, 1);
    settings.update(ANDROID_SHADING_MODE, &shadingMode, 1);
#if CAMERA_DEVICE_API_VERSION_CURRENT < HARDWARE_DEVICE_API_VERSION(3, 2)
    settings.update(ANDROID_GEOMETRIC_MODE, &geometricMode, 1);
#endif
    settings.update(ANDROID_COLOR_CORRECTION_MODE, &colorMode, 1);
    settings.update(ANDROID_TONEMAP_MODE, &tonemapMode, 1);
    settings.update(ANDROID_EDGE_MODE, &edgeMode, 1);

    // android.noise
    const uint8_t noiseStrength = 5;
    settings.update(ANDROID_NOISE_REDUCTION_STRENGTH, &noiseStrength, 1);

    // android.color
    const camera_metadata_rational_t colorTransform[9] = {
        { 1, 1 }, { 0, 1 }, { 0, 1 },
        { 0, 1 }, { 1, 1 }, { 0, 1 },
        { 0, 1 }, { 0, 1 }, { 1, 0 }
    };
    settings.update(ANDROID_COLOR_CORRECTION_TRANSFORM,
            colorTransform,
            ARRAY_NUM_ELEMENTS(colorTransform));

    const float colorGains[] = {
        1.0, 1.0, 1.0, 1.0
    };
    settings.update(ANDROID_COLOR_CORRECTION_GAINS,
            colorGains, ARRAY_NUM_ELEMENTS(colorGains));

    // android.tonemap
    const float tonemapCurve[4] = {
        0.f, 0.f,
        1.f, 1.f
    };
    settings.update(ANDROID_TONEMAP_CURVE_RED,
            tonemapCurve, ARRAY_NUM_ELEMENTS(tonemapCurve));
    settings.update(ANDROID_TONEMAP_CURVE_GREEN,
            tonemapCurve, ARRAY_NUM_ELEMENTS(tonemapCurve));
    settings.update(ANDROID_TONEMAP_CURVE_BLUE,
            tonemapCurve, ARRAY_NUM_ELEMENTS(tonemapCurve));

    // android.edge
    const uint8_t edgeStrength = 5;
    settings.update(ANDROID_EDGE_STRENGTH, &edgeStrength, 1);

    // android.scaler
    uint32_t width, height;
    if(!mCameraHw.getSensorResolution(width, height)) {
        return NULL;
    }
    const int32_t cropRegion[4] = {
            0, 0, (int32_t)width-1, (int32_t)height-1
    };
    settings.update(ANDROID_SCALER_CROP_REGION,
            cropRegion, ARRAY_NUM_ELEMENTS(cropRegion));

    initRequestJpegSettings(type, settings);

    // android.stats
    const uint8_t faceDetectMode =
    ANDROID_STATISTICS_FACE_DETECT_MODE_OFF;
    settings.update(ANDROID_STATISTICS_FACE_DETECT_MODE, &faceDetectMode, 1);

    const uint8_t histogramMode = ANDROID_STATISTICS_HISTOGRAM_MODE_OFF;
    settings.update(ANDROID_STATISTICS_HISTOGRAM_MODE, &histogramMode, 1);

    const uint8_t sharpnessMapMode =
    ANDROID_STATISTICS_SHARPNESS_MAP_MODE_OFF;
    settings.update(ANDROID_STATISTICS_SHARPNESS_MAP_MODE, &sharpnessMapMode,
            1);

    const uint8_t lensShadingMapMode =
        ANDROID_STATISTICS_LENS_SHADING_MAP_MODE_OFF;
    settings.update(ANDROID_STATISTICS_LENS_SHADING_MAP_MODE,
            &lensShadingMapMode, 1);

    // android.control
    uint8_t controlIntent = ANDROID_CONTROL_CAPTURE_INTENT_CUSTOM;
    uint8_t controlMode = ANDROID_CONTROL_MODE_AUTO;
    switch (type) {
        case CAMERA3_TEMPLATE_PREVIEW:
            controlIntent = ANDROID_CONTROL_CAPTURE_INTENT_PREVIEW;
            break;
        case CAMERA3_TEMPLATE_STILL_CAPTURE:
            controlIntent = ANDROID_CONTROL_CAPTURE_INTENT_STILL_CAPTURE;
            break;
        case CAMERA3_TEMPLATE_VIDEO_RECORD:
            controlIntent = ANDROID_CONTROL_CAPTURE_INTENT_VIDEO_RECORD;
            break;
        case CAMERA3_TEMPLATE_VIDEO_SNAPSHOT:
            controlIntent = ANDROID_CONTROL_CAPTURE_INTENT_VIDEO_SNAPSHOT;
            break;
        case CAMERA3_TEMPLATE_ZERO_SHUTTER_LAG:
            controlIntent = ANDROID_CONTROL_CAPTURE_INTENT_ZERO_SHUTTER_LAG;
            break;
        case CAMERA3_TEMPLATE_MANUAL:
            controlIntent = ANDROID_CONTROL_CAPTURE_INTENT_MANUAL;
            controlMode = ANDROID_CONTROL_MODE_OFF;
            break;
        default:
            break;
    }

    settings.update(ANDROID_CONTROL_CAPTURE_INTENT, &controlIntent, 1);
    settings.update(ANDROID_CONTROL_MODE, &controlMode, 1);

    const uint8_t effectMode = ANDROID_CONTROL_EFFECT_MODE_OFF;
    settings.update(ANDROID_CONTROL_EFFECT_MODE, &effectMode, 1);

#ifdef USE_GPU_LIB
    const uint8_t sceneMode = ANDROID_CONTROL_SCENE_MODE_DISABLED; // by default disabled.
//    const uint8_t sceneMode = ANDROID_CONTROL_SCENE_MODE_FACE_PRIORITY;
#else
    const uint8_t sceneMode = ANDROID_CONTROL_SCENE_MODE_DISABLED;
#endif
    settings.update(ANDROID_CONTROL_SCENE_MODE, &sceneMode, 1);

    if(mCameraHw.getMainPipeline()) {
        FelixAF *afModule =
                mCameraHw.getMainPipeline()->getControlModule<FelixAF>();
        if(afModule) {
            afModule->initRequestMetadata(settings, type);
        }
        FelixAWB *awbModule =
                mCameraHw.getMainPipeline()->getControlModule<FelixAWB>();
        if(awbModule) {
            awbModule->initRequestMetadata(settings, type);
        }
        FelixAE *aeModule =
                mCameraHw.getMainPipeline()->getControlModule<FelixAE>();
        if(aeModule) {
            aeModule->initRequestMetadata(settings, type);
        }
    }
    const uint8_t vstabMode =
        ANDROID_CONTROL_VIDEO_STABILIZATION_MODE_OFF;
    settings.update(ANDROID_CONTROL_VIDEO_STABILIZATION_MODE,
            &vstabMode, 1);

    // 'generate' the camera_metadata_t pointer from the CameraMetadata object
    mDefaultRequestTemplates[type] = settings.release();

    return mDefaultRequestTemplates[type];
}

status_t FelixCamera::processNewCropRegion(
        __maybe_unused FelixMetadata& metadata) {
        status_t res = NO_ERROR;

    // calculate and setup specific cropping for each active stream
    StreamIterator iter = mStreams.begin();
    while(iter != mStreams.end()) {
        PrivateStreamInfo *privStream = *iter;
        ALOG_ASSERT(privStream);
        const camera3_stream_t* stream = privStream->getParent();
        ALOG_ASSERT(stream);
        Rect rect;
        res = metadata.calculateStreamCrop(
                stream->width, stream->height, rect);

        if(res != BAD_VALUE) {
            res = mCameraHw.scaleAndCrop(stream, rect);
        }
        if(res != NO_ERROR) {
            return res;
        }
        ++iter;
    }
    return NO_ERROR;
}

status_t FelixCamera::stopCapture() {
    if (mCaptureStarted) {
        // : if the request queue is not empty,
        // wait until latest buffer has been acquired in threadLoop
        if (mCameraHw.stopCapture() != IMG_SUCCESS) {
            LOG_WARNING("ERROR stopping capture.");
        }
        mCaptureStarted = false;
    }
    return NO_ERROR;
}

status_t FelixCamera::startCapture() {
    if (!mCaptureStarted) {
        if (mCameraHw.startCapture() != IMG_SUCCESS) {
            LOG_ERROR("Failed to start capture.");
            notifyFatalError();
            return NO_INIT;
        }
        mCaptureStarted = true;
        // : signal start of capture
        LOG_DEBUG("Capture started.");
    }
    return NO_ERROR;
}

status_t FelixCamera::program() {
    if (mCameraHw.program() != NO_ERROR) {
        LOG_ERROR("ERROR programming pipeline.");
        notifyFatalError();
        return NO_INIT;
    }
    return NO_ERROR;
}

//  Sends new capture to the HAL which should not return until ready to accept
//  the next request. The HAL returns the result through the
//  process_capture_result() call which requires the result metadata to be
//  available but output buffers may simply provide sync fences to wait on.
status_t FelixCamera::processCaptureRequest(
        camera3_capture_request *request) {
    status_t res;
    sp<CaptureRequest> internalCaptureRequest;

    uint32_t frameNumber;

    Mutex::Autolock l(mLock);

    if (mStatus == STATUS_ERROR) {
        return NO_INIT;
    }

    // Validation
    if (mStatus != STATUS_READY && mStatus != STATUS_ACTIVE) {
        LOG_ERROR("Can't submit capture requests in state %d",
                mStatus);
        return INVALID_OPERATION;
    }

    if (request == NULL) {
        LOG_ERROR("NULL request!");
        return BAD_VALUE;
    }
    frameNumber = request->frame_number;
    // The request must come with settings or a previous request
    // had to have set the setting
    if (request->settings == NULL && !mFrameSettings.entryCount()) {
        LOG_ERROR("Request %d: NULL settings for first request after "
                "configureStreams()", frameNumber);
        return BAD_VALUE;
    }

    // This case is an input buffer with a wrong input stream associated
    if (request->input_buffer != NULL &&
            request->input_buffer->stream != mInputStream) {
        LOG_ERROR("Request %d: Input buffer not from input stream!",
                frameNumber);
        return BAD_VALUE;
    }

    // We need at least one output buffer defined
    if (request->num_output_buffers < 1 ||
        request->output_buffers == NULL) {
        LOG_ERROR("Request %d: No output buffers provided!",
                frameNumber);
        return BAD_VALUE;
    }

    frameNumber = request->frame_number;
    LOG_DEBUG("frame number=%d out_streams=%d metadata=%p", frameNumber,
        request->num_output_buffers, request->settings);

    // Validate all buffers, starting with input buffer if it's given
    ssize_t idx;
    const camera3_stream_buffer_t *b;

    if (request->input_buffer != NULL) {
        idx = -1;
        b = request->input_buffer;
        LOG_DEBUG("INPUT BUFFER PROVIDED! %dx%d, format=0x%x usage=0x%x "
                "fd=0x%x handle=%p",
                b->stream->width, b->stream->height,
                b->stream->format, b->stream->usage,
                getIonFd(*b->buffer), *b->buffer);
    } else {
        idx = 0;
        b = request->output_buffers;
        LOG_DEBUG("OUTPUT BUFFERS PROVIDED!  %dx%d, stream=0x%x format=0x%x usage=0x%x stream_type=0x%x " \
                "fd=0x%x handle=%p",
                b->stream->width, b->stream->height,
                b->stream, b->stream->format, b->stream->usage, b->stream->stream_type,
                getIonFd(*b->buffer), *b->buffer);
    }

    // Check that the stream(s) we are going to use is(are) properly
    // initialised
    do {
        if(mProcessingThread->isFlushing()) {
            // don't loop if we're already flushing the queue
            break;
        }
        PrivateStreamInfo *priv = streamToPriv(b->stream);
        if (priv == NULL) {
            LOG_ERROR("Request %d: Buffer %d: Unconfigured stream!",
                    frameNumber, idx);
            return BAD_VALUE;
        }
        bool newHwBuffer = false;
#if CAMERA_DEVICE_API_VERSION_CURRENT < HARDWARE_DEVICE_API_VERSION(3, 2)
        if (!priv->alive || (!priv->registered &&
                    (priv->ciBuffType != CI_TYPE_NONE))) {
            LOG_ERROR("Request %d: Buffer %d: Unregistered or dead stream!"
                    " (registered=%d, alive=%d)",
                    frameNumber, idx, priv->registered, priv->alive);
            return BAD_VALUE;
        }
#else
        // check if the buffer should be imported prior to enqueuing
        newHwBuffer =
                b != request->input_buffer &&
                b->stream->format != HAL_PIXEL_FORMAT_BLOB &&
                !priv->isBufferRegistered(*b->buffer);

        // In case of JPEG priv->registered flag can be not set.
        if (!priv->alive) {
            LOG_ERROR("Request %d: Buffer %d: dead stream!"
                    " (alive=%d)",
                    frameNumber, idx, priv->alive);
            return BAD_VALUE;
        }
#endif

        LOG_DEBUG("Buffer %dx%d, stream=0x%x format=0x%x usage=0x%x stream_type=0x%x fd=0x%x handle=%p" \
                " %s",
                b->stream->width, b->stream->height,
                b->stream, b->stream->format, b->stream->usage, b->stream->stream_type,
                getIonFd(*b->buffer), *b->buffer,
                (!newHwBuffer ? "" : "(new)"));

        if (b->status != CAMERA3_BUFFER_STATUS_OK) {
            LOG_ERROR("Request %d: Buffer %d: Status not OK!",
                    frameNumber, idx);
            return BAD_VALUE;
        }

        // We must be able to use the buffer
        if (b->release_fence != -1) {
            LOG_ERROR("Request %d: Buffer %d: Has a release fence!",
                    frameNumber, idx);
            return BAD_VALUE;
        }

        if (b->buffer == NULL) {
            LOG_ERROR("Request %d: Buffer %d: NULL buffer handle!",
                    frameNumber, idx);
            return BAD_VALUE;
        }
#if CAMERA_DEVICE_API_VERSION_CURRENT >= HARDWARE_DEVICE_API_VERSION(3, 2)
        if(newHwBuffer) {
            Mutex::Autolock lhw(mCameraHw.getHwLock());

            if ((res = priv->registerBuffer(*b->buffer)) != OK) {
                LOG_ERROR("Failed to register buffers in stream private area!");
            }
        }
#endif
        idx++;
        // iterate through the rest of the requested buffers
        b = &(request->output_buffers[idx]);
        LOG_DEBUG(" line %d idx = %d request->num_output_buffers =%d ", __LINE__, idx, request->num_output_buffers);
    } while (idx < (ssize_t)request->num_output_buffers);

    if(request->settings) {
            // clone the data in current
        mFrameSettings = request->settings;
    }
    sp<FelixMetadata> metadata = new FelixMetadata(*this);
    if(!metadata.get()) {
        LOG_ERROR("No memory for metadata!");
        return NO_MEMORY;
    }
    metadata->operator=(mFrameSettings); // assignment clones the data

    // --------------------------------------------------------------------
    // Deal with Felix: start triggering pictures...
    // --------------------------------------------------------------------
    internalCaptureRequest = new CaptureRequest(*this, metadata);
    if(!internalCaptureRequest.get()) {
        notifyFatalError();
        return NO_INIT;
    }

    if(mProcessingThread->isFlushing()) {
        // setting error flag is enough to flush current request
        internalCaptureRequest->mRequestError = true;
    }

    internalCaptureRequest->preprocessRequest(*request);
    LOG_DEBUG("Request preprocessing completed.");

    if(internalCaptureRequest->mRequestError ||
       processNewCropRegion(*metadata) != NO_ERROR) {
        // Queue the capture for processing in thread...
        notifyRequestError(frameNumber);
        mProcessingThread->queueCaptureRequest(*internalCaptureRequest);
        return OK;
    }

    mStatus = STATUS_ACTIVE; // processing a request

    nsecs_t duration = metadata->getFrameDuration();
    if(duration > 0) {
        LOG_DEBUG("Duration set to %llu", duration);
        // FIXME: add support for exact delay for each shot programmed
        LOG_WARNING("Setting explicit frame duration is not supported!");
    }
    if(!internalCaptureRequest->isReprocessing()) {

        internalCaptureRequest->preprocessMetadata();
        // Program shot...
        if(internalCaptureRequest->program() == NO_INIT) {
            // the fatal error has been already notified
            return NO_INIT;
        }
        LOG_DEBUG("Programming completed.");
    }

    // if JPEG output requested then wait for encoder before new action
    if (internalCaptureRequest->mJpegRequested) {
        LOG_DEBUG("Waiting for JPEG encoder to be ready...");
        if (!JpegEncoder::get().waitForDone()) {
            LOG_ERROR("Timeout waiting for JPEG compression to complete!");
            internalCaptureRequest->mRequestError = true;
            notifyRequestError(frameNumber);
        }
    }

    // Queue the capture for processing in thread...
    mProcessingThread->queueCaptureRequest(*internalCaptureRequest);

    LOG_DEBUG("Queued frame %d (err=%d)", frameNumber,
        internalCaptureRequest->mRequestError);

    return OK;
}

void FelixCamera::dumpInfo(__maybe_unused int fd) {

    std::ostringstream output;
    mCameraHw.dumpPipelines(output);
    size_t size = output.str().size();
    if(size > 0) {
        write(fd, output.str().c_str(), size);
    }
    return;
}

FelixCamera* FelixCamera::getInstance(const camera3_device_t *d) {
    const FelixCamera* cec = static_cast<const FelixCamera*>(d);
    return const_cast<FelixCamera*>(cec);
}

// ----------------------------------------------------------------------------
// camera3_device_ops_t method implementation
//
// These are static methods than invoke the associated methods in a given
// instance of the device
// ----------------------------------------------------------------------------

int FelixCamera::initialize(const struct camera3_device *d,
        const camera3_callback_ops_t *callback_ops) {
    FelixCamera* ec = getInstance(d);
    int status = ec->initializeDevice(callback_ops);
    return status;
}

int FelixCamera::configure_streams(const struct camera3_device *d,
        camera3_stream_configuration_t *stream_list) {
    FelixCamera* ec = getInstance(d);
    int status = ec->configureStreams(stream_list);
    return status;
}

int FelixCamera::register_stream_buffers(const struct camera3_device *d,
        const camera3_stream_buffer_set_t *buffer_set) {
    FelixCamera* ec = getInstance(d);
    int status = ec->registerStreamBuffers(buffer_set);
    return status;
}

int FelixCamera::process_capture_request(const struct camera3_device *d,
        camera3_capture_request_t *request) {
    FelixCamera* ec = getInstance(d);
    int status = ec->processCaptureRequest(request);
    return status;
}

const camera_metadata_t* FelixCamera::construct_default_request_settings(
        const camera3_device_t *d, int type) {
    FelixCamera* ec = getInstance(d);
    const camera_metadata_t *metadata =
        ec->constructDefaultRequestSettings(type);
    LOG_DEBUG("STOP");
    return metadata;
}

void FelixCamera::get_metadata_vendor_tag_ops(
        __maybe_unused const camera3_device_t *d,
        vendor_tag_query_ops_t *ops) {
    ops->get_camera_vendor_section_name = get_camera_vendor_section_name;
    ops->get_camera_vendor_tag_name = get_camera_vendor_tag_name;
    ops->get_camera_vendor_tag_type = get_camera_vendor_tag_type;
}

void FelixCamera::dump(const camera3_device_t *d, int fd) {
    FelixCamera* ec = getInstance(d);
    ec->dumpInfo(fd);
}

// Tag query methods
const char* FelixCamera::getVendorSectionName(__maybe_unused uint32_t tag) {
    return NULL;
}

const char* FelixCamera::getVendorTagName(__maybe_unused uint32_t tag) {
    return NULL;
}

int FelixCamera::getVendorTagType(__maybe_unused uint32_t tag) {
    return 0;
}

void FelixCamera::initLensCapabilities(
        CameraMetadata& info, const sensor_t& sensor){
    // : device specific parameters - move to ro props
    LOG_DEBUG("E");

    const float minFocusDistance =
            mAfSupported ? 1000.0/sensor.getMinFocus() : 0.0;
    // 5 m hyperfocal distance for back camera,
    // infinity (fixed focus) for front
    const float hyperFocalDistance = mAfSupported ? minFocusDistance : 0.0;
    const float focalLength = (float)sensor.uiFocalLength; // mm
    const float aperture =  sensor.flAperture;
    const float filterDensity = 0;
    const uint8_t availableOpticalStabilization =
        ANDROID_LENS_OPTICAL_STABILIZATION_MODE_OFF;
#if CAMERA_DEVICE_API_VERSION_CURRENT < HARDWARE_DEVICE_API_VERSION(3, 2)
    const int32_t geometricCorrectionMapSize[] = { 2, 2 };
    const float geometricCorrectionMap[2 * 3 * 2 * 2] = {
        0.f, 0.f,  0.f, 0.f,  0.f, 0.f,
        1.f, 0.f,  1.f, 0.f,  1.f, 0.f,
        0.f, 1.f,  0.f, 1.f,  0.f, 1.f,
        1.f, 1.f,  1.f, 1.f,  1.f, 1.f};
#endif
    const uint8_t lensFacing = mFacingBack ?
        ANDROID_LENS_FACING_BACK : ANDROID_LENS_FACING_FRONT;
    float lensPosition[3];
    if (mFacingBack) {
        // Back-facing camera is center-top on device
        lensPosition[0] = 0;
        lensPosition[1] = 20;
        lensPosition[2] = -5;
    } else {
        // Front-facing camera is center-right on device
        lensPosition[0] = 20;
        lensPosition[1] = 20;
        lensPosition[2] = 0;
    }

    info.update(ANDROID_LENS_INFO_MINIMUM_FOCUS_DISTANCE,
            &minFocusDistance, 1);
    info.update(ANDROID_LENS_INFO_HYPERFOCAL_DISTANCE,
            &hyperFocalDistance, 1);
    info.update(ANDROID_LENS_INFO_AVAILABLE_FOCAL_LENGTHS, &focalLength, 1);
    info.update(ANDROID_LENS_INFO_AVAILABLE_APERTURES, &aperture, 1);
    info.update(ANDROID_LENS_INFO_AVAILABLE_FILTER_DENSITIES,
        &filterDensity, 1);
    info.update(ANDROID_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION,
        &availableOpticalStabilization, 1);

    int32_t width, height;
    mCameraHw.getLSHMapSize(width, height);
    const int32_t lensShadingMapSize[] = { width, height };
    info.update(ANDROID_LENS_INFO_SHADING_MAP_SIZE,
            lensShadingMapSize,
            ARRAY_NUM_ELEMENTS(lensShadingMapSize));

#if CAMERA_DEVICE_API_VERSION_CURRENT < HARDWARE_DEVICE_API_VERSION(3, 2)
    info.update(ANDROID_LENS_INFO_GEOMETRIC_CORRECTION_MAP_SIZE,
            geometricCorrectionMapSize,
            ARRAY_NUM_ELEMENTS(geometricCorrectionMapSize));
    info.update(ANDROID_LENS_INFO_GEOMETRIC_CORRECTION_MAP,
            geometricCorrectionMap,
            ARRAY_NUM_ELEMENTS(geometricCorrectionMap));
#endif

    info.update(ANDROID_LENS_FACING, &lensFacing, 1);
    info.update(ANDROID_LENS_POSITION, lensPosition,
            ARRAY_NUM_ELEMENTS(lensPosition));
}

#ifndef UPSCALING_ENABLED
uint32_t sensorWidth, sensorHeight;
#endif

status_t FelixCamera::initSensorCapabilities(
        CameraMetadata& info, const sensor_t& sensor) {
    // : assert if sensor not initialized

    const int64_t maxFrameDuration = 30*SEC;
    // define max raw value as max value of 'bit level' bits
    const int32_t kMaxRawValue = (1 << sensor.uiBitDepth)-1;

    if(!mCameraHw.getSensorResolution(sensorWidth, sensorHeight)) {
        return NO_INIT;
    }
    const int32_t resolution[4] = {
            0, 0, (int32_t)sensorWidth, (int32_t)sensorHeight
    };
    const int32_t orientation = 0;
    // : sensitivity values above max analog sensitivity could be obtained
    // using felix pipeline config
    const int32_t kMaxAnalogSensitivity = 1600;
    // : calculate sensitivity range using sensor->GetGainRange()
    int32_t kSensitivityRange[2] = { 100, 1600 };
    uint8_t kColorFilterArrangement;

    const nsecs_t kExposureTimeRange[2] ={
            (nsecs_t)sensor.getMinExposure()*USEC ,
            (nsecs_t)sensor.getMaxExposure()*USEC } ; // 1 us - 2 sec

    // : this probably needs to be defined after calibration ?
    const uint32_t kBlackLevel  = 1000;
    const int32_t blackLevelPattern[4] = { (int32_t)kBlackLevel,
        (int32_t)kBlackLevel, (int32_t)kBlackLevel, (int32_t)kBlackLevel };

    // FIXME: need to obtain these values for specific sensor
    const camera_metadata_rational_t baseGainFactor = { 1, 1 };

    switch(sensor.eBayerFormat)
    {
    case MOSAIC_RGGB:
        kColorFilterArrangement =
            ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_RGGB;
        break;
    case MOSAIC_GRBG:
        kColorFilterArrangement =
            ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_GRBG;
        break;
    case MOSAIC_GBRG:
        kColorFilterArrangement =
            ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_GBRG;
        break;
    case MOSAIC_BGGR:
        kColorFilterArrangement =
            ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_BGGR;
        break;
    case MOSAIC_NONE:
    default:
        LOG_ERROR("Invalid bayer type %d",
                sensor.eBayerFormat);
        return INVALID_OPERATION;
    }

    info.update(ANDROID_SENSOR_ORIENTATION, &orientation, 1);
    // android.sensor
    info.update(ANDROID_SENSOR_INFO_EXPOSURE_TIME_RANGE,
            (int64_t *)&kExposureTimeRange, 2);
    info.update(ANDROID_SENSOR_INFO_MAX_FRAME_DURATION,
            &maxFrameDuration, 1);
    info.update(ANDROID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT,
            &kColorFilterArrangement, 1);
    // depends on the actual sensor type
    info.update(ANDROID_SENSOR_INFO_PHYSICAL_SIZE,
            mHwCapabilities->sensorPhysicalSize, 2);
    info.update(ANDROID_SENSOR_INFO_PIXEL_ARRAY_SIZE,
            (int32_t *)&resolution[2], 2);
    info.update(ANDROID_SENSOR_INFO_ACTIVE_ARRAY_SIZE,
            (int32_t *)&resolution, 4);
    info.update(ANDROID_SENSOR_INFO_WHITE_LEVEL,
            (int32_t *)&kMaxRawValue, 1);
    info.update(ANDROID_SENSOR_MAX_ANALOG_SENSITIVITY,
            &kMaxAnalogSensitivity, 1);
    info.update(ANDROID_SENSOR_INFO_SENSITIVITY_RANGE,
            (int32_t *)&kSensitivityRange, 2);
    info.update(ANDROID_SENSOR_BLACK_LEVEL_PATTERN, blackLevelPattern,
            ARRAY_NUM_ELEMENTS(blackLevelPattern));

    info.update(ANDROID_SENSOR_BASE_GAIN_FACTOR, &baseGainFactor, 1);


    //: sensor color calibration fields
    return OK;
}

void FelixCamera::initScalerCapabilities(CameraMetadata& info) {
    // android.scaler
#ifdef UPSCALING_ENABLED
#error "maxZoom not defined until GPU image processing has been integrated "
    "into HAL"
#else
    // Felix supports downscaling only so currently no zoom supported
    // but in order to do some fake cts tests, define zoom to 2
    const float maxZoom = 2.0;
#endif
    // android.scaler
    info.update(ANDROID_SCALER_AVAILABLE_FORMATS,
            HwCaps::kAvailableFormats, HwCaps::kNumAvailableFormats);

    // All fields below became deprecated since HAL v3.2
    // but we are in HALv3.0 for Kitkat so we must init them anyway
    {
        Vector<int32_t> availableRawSizes;
        Vector<int64_t> availableRawMinDurations;
        SENSOR_MODE modeInfo;
        SensorResCaps_t const *table =
            mHwCapabilities->kAvailableRawImageParams;
        for(size_t i=0;i<mHwCapabilities->kNumAvailableRawParams;++i) {
            availableRawSizes.push(table[i].width);
            availableRawSizes.push(table[i].height);

            IMG_RESULT res = mCameraHw.getSensor()->getMode(
                table[i].sensorMode,
                modeInfo);
            ALOG_ASSERT(res == IMG_SUCCESS,
                "Unsupported sensor mode %d defined in HwCaps",
                table[i].sensorMode);
            availableRawMinDurations.push(
                seconds_to_nanoseconds(1)/modeInfo.flFrameRate);
        }

        // we support one raw size same as sensor size
        info.update(ANDROID_SCALER_AVAILABLE_RAW_SIZES,
                availableRawSizes.array(),
                availableRawSizes.size());
        info.update(ANDROID_SCALER_AVAILABLE_RAW_MIN_DURATIONS,
            availableRawMinDurations.array(),
            availableRawMinDurations.size());
    }
    {
        Vector<int32_t> availableProcessedSizes;
        Vector<int64_t> availableProcessedMinDurations;
        SENSOR_MODE modeInfo;
        SensorResCaps_t const *table =
            mHwCapabilities->kAvailableProcessedImageParams;

        for(size_t i=0;i<mHwCapabilities->kNumAvailableProcessedParams;++i) {
            availableProcessedSizes.push(table[i].width);
            availableProcessedSizes.push(table[i].height);

            IMG_RESULT res = mCameraHw.getSensor()->getMode(
                table[i].sensorMode,
                modeInfo);
            ALOG_ASSERT(res == IMG_SUCCESS,
                "Unsupported sensor mode %d defined in HwCaps",
                table[i].sensorMode);
            availableProcessedMinDurations.push(
                seconds_to_nanoseconds(1)/modeInfo.flFrameRate);
        }
        info.update(ANDROID_SCALER_AVAILABLE_PROCESSED_MIN_DURATIONS,
            availableProcessedMinDurations.array(),
            availableProcessedMinDurations.size());
        info.update(ANDROID_SCALER_AVAILABLE_PROCESSED_SIZES,
                availableProcessedSizes.array(),
                availableProcessedSizes.size());
    }
    {
        Vector<int32_t> availableJpegSizes;
        Vector<int64_t> availableJpegMinDurations;
        SensorResCaps_t const *table =
                    mHwCapabilities->kAvailableJpegImageParams;
        for(size_t i=0;i<mHwCapabilities->kNumAvailableJpegParams;++i) {
            availableJpegSizes.push(table[i].width);
            availableJpegSizes.push(table[i].height);
            availableJpegMinDurations.push(table[i].minDurationNs);
        }
        // we support one raw size same as sensor size
        info.update(ANDROID_SCALER_AVAILABLE_JPEG_SIZES,
                availableJpegSizes.array(),
                availableJpegSizes.size());
        info.update(ANDROID_SCALER_AVAILABLE_JPEG_MIN_DURATIONS,
            availableJpegMinDurations.array(),
            availableJpegMinDurations.size());
    }

    info.update(ANDROID_SCALER_AVAILABLE_MAX_DIGITAL_ZOOM, &maxZoom, 1);
}

void FelixCamera::initAAACapabilities(CameraMetadata& info) {
    // : define the list
#ifdef USE_GPU_LIB
    const uint8_t availableSceneModes[] = {
            ANDROID_CONTROL_SCENE_MODE_DISABLED,
            ANDROID_CONTROL_SCENE_MODE_FACE_PRIORITY
    };
#else
    const uint8_t availableSceneModes[] = {
        ANDROID_CONTROL_SCENE_MODE_DISABLED
    };
#endif
    info.update(ANDROID_CONTROL_AVAILABLE_SCENE_MODES,
            availableSceneModes,
            ARRAY_NUM_ELEMENTS(availableSceneModes));

#ifdef USE_GPU_LIB
    const uint8_t sceneModeOverrides[] = {
        ANDROID_CONTROL_AE_MODE_ON,
        ANDROID_CONTROL_AWB_MODE_AUTO,
        ANDROID_CONTROL_AF_MODE_AUTO,
        ANDROID_CONTROL_AE_MODE_ON,
        ANDROID_CONTROL_AWB_MODE_AUTO,
        ANDROID_CONTROL_AF_MODE_AUTO
    };
#else
    const uint8_t sceneModeOverrides[] = {
        ANDROID_CONTROL_AE_MODE_ON,
        ANDROID_CONTROL_AWB_MODE_AUTO,
        ANDROID_CONTROL_AF_MODE_AUTO
    };
#endif
    info.update(ANDROID_CONTROL_SCENE_MODE_OVERRIDES,
            sceneModeOverrides,
            ARRAY_NUM_ELEMENTS(sceneModeOverrides));

    FelixAE::advertiseCapabilities(info, getHwInstance());
    FelixAWB::advertiseCapabilities(info);
#if CAMERA_DEVICE_API_VERSION_CURRENT < HARDWARE_DEVICE_API_VERSION(3, 2)
    int32_t max3aRegions = 7*7;
    info.update(ANDROID_CONTROL_MAX_REGIONS, &max3aRegions, 1);
#else
    // The order of the elements is: (AE, AWB, AF).
    int32_t max3aRegions[3] = { 7*7, 1, 7*7 };
    info.update(ANDROID_CONTROL_MAX_REGIONS,
            max3aRegions,
            ARRAY_NUM_ELEMENTS(max3aRegions));
#endif

    if (mAfSupported) {
        FelixAF::advertiseCapabilities(info);
    } else {
        const uint8_t availableAfModesOff[] = {
            ANDROID_CONTROL_AF_MODE_OFF
        };
        info.update(ANDROID_CONTROL_AF_AVAILABLE_MODES,
                availableAfModesOff,
                ARRAY_NUM_ELEMENTS(availableAfModesOff));
    }

}

void FelixCamera::initStatsCapabilities(CameraMetadata& info) {

#ifdef USE_GPU_LIB
    const uint8_t availableFaceDetectModes[] = {
        ANDROID_STATISTICS_FACE_DETECT_MODE_OFF
        ,ANDROID_STATISTICS_FACE_DETECT_MODE_SIMPLE
//        ,ANDROID_STATISTICS_FACE_DETECT_MODE_FULL
    };
#else
    const uint8_t availableFaceDetectModes[] = {
        ANDROID_STATISTICS_FACE_DETECT_MODE_OFF
    };
#endif

    info.update(ANDROID_STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES,
            availableFaceDetectModes,
            ARRAY_NUM_ELEMENTS(availableFaceDetectModes));


#ifdef USE_GPU_LIB
    const int32_t maxFaceCount = 4;
#else
    const int32_t maxFaceCount = 0;
#endif
    info.update(ANDROID_STATISTICS_INFO_MAX_FACE_COUNT,
            &maxFaceCount, 1);

    info.update(ANDROID_STATISTICS_INFO_HISTOGRAM_BUCKET_COUNT,
            &kHistogramBucketCount, 1);

    info.update(ANDROID_STATISTICS_INFO_MAX_HISTOGRAM_COUNT,
            &kMaxHistogramCount, 1);

    const int32_t sharpnessMapSize[2] = { 64, 64 };
    info.update(ANDROID_STATISTICS_INFO_SHARPNESS_MAP_SIZE,
            sharpnessMapSize,
            ARRAY_NUM_ELEMENTS(sharpnessMapSize));

    const int32_t maxSharpnessMapValue = 1000;
    info.update(ANDROID_STATISTICS_INFO_MAX_SHARPNESS_MAP_VALUE,
            &maxSharpnessMapValue, 1);
}

void FelixCamera::initJpegCapabilities(CameraMetadata& info) {
    int32_t* jpegThumbnailSizes = NULL;
    int32_t jpegThumbnailElements = 0;

    // static getters
    JpegEncoder::getThumbnailCapabilities(
            &jpegThumbnailSizes, jpegThumbnailElements);

    const int32_t maxJpegBufferSize =
        JpegEncoder::getMaxBufferSize();

    // android.jpeg
    info.update(ANDROID_JPEG_AVAILABLE_THUMBNAIL_SIZES,
            jpegThumbnailSizes, jpegThumbnailElements);
    info.update(ANDROID_JPEG_MAX_SIZE,
            &maxJpegBufferSize, 1);
}

void FelixCamera::initRequestJpegSettings(__maybe_unused int type,
        CameraMetadata& settings) {
    // android.jpeg
    const uint8_t jpegQuality = 90;
    settings.update(ANDROID_JPEG_QUALITY, &jpegQuality, 1);

    int32_t* jpegThumbnailSizes = NULL;
    int32_t jpegThumbnailElements = 0;
    // static getters
    JpegEncoder::getThumbnailCapabilities(
            &jpegThumbnailSizes, jpegThumbnailElements);
    ALOG_ASSERT(jpegThumbnailSizes);
    ALOG_ASSERT(jpegThumbnailElements>1);
    // use biggest available pair by default, they are sorted in ascending order
    settings.update(ANDROID_JPEG_THUMBNAIL_SIZE,
            &jpegThumbnailSizes[jpegThumbnailElements-2], 2);

    const uint8_t thumbnailQuality = 6;
    settings.update(ANDROID_JPEG_THUMBNAIL_QUALITY,
            &thumbnailQuality, 1);
}

status_t FelixCamera::constructStaticInfo(void) {
    const uint8_t flashAvailable = ANDROID_FLASH_INFO_AVAILABLE_FALSE;
    const int64_t flashChargeDuration = 0;
    const int32_t tonemapCurvePoints = 128;

    //: read these from video stabilisation instance
    const uint8_t availableVstabModes[] = {
        ANDROID_CONTROL_VIDEO_STABILIZATION_MODE_OFF
    };
    const uint8_t supportedHardwareLevel =
        mFullMode ? ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_FULL :
            ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL_LIMITED;

    CameraMetadata info;
    // android.lens
    ALOG_ASSERT(mCameraHw.getSensor(), "no sensor!");
    initLensCapabilities(info, *mCameraHw.getSensor());

    // android.flash
    initSensorCapabilities(info, *mCameraHw.getSensor());

    info.update(ANDROID_REQUEST_MAX_NUM_OUTPUT_STREAMS,
            HwCaps::maxNumOutputStreams,
            ARRAY_NUM_ELEMENTS(HwCaps::maxNumOutputStreams));

    info.update(ANDROID_REQUEST_MAX_NUM_REPROCESS_STREAMS,
            &HwCaps::maxNumOutputStreams[HwCaps::STALLING], 1);

    // android.scaler
    initScalerCapabilities(info);

    // android.flash
    // FIXME: parametrise
    info.update(ANDROID_FLASH_INFO_AVAILABLE,
            &flashAvailable, 1);

    info.update(ANDROID_FLASH_INFO_CHARGE_DURATION,
            &flashChargeDuration, 1);

    // android.tonemap
    info.update(ANDROID_TONEMAP_MAX_CURVE_POINTS,
            &tonemapCurvePoints, 1);

    // android.stats
    initStatsCapabilities(info);

    // android.control
    initAAACapabilities(info);

    initJpegCapabilities(info);

    // effects
    const uint8_t *effectArray;
    uint8_t effectCount;
    mCameraHw.getAvailableEffects(effectArray, effectCount);
    info.update(ANDROID_CONTROL_AVAILABLE_EFFECTS,
            effectArray, effectCount);

    info.update(ANDROID_CONTROL_AVAILABLE_VIDEO_STABILIZATION_MODES,
            availableVstabModes,
            ARRAY_NUM_ELEMENTS(availableVstabModes));

    // android.info
    info.update(ANDROID_INFO_SUPPORTED_HARDWARE_LEVEL, &supportedHardwareLevel,
            /*count*/1);

    mCameraStaticInfo = info.release();

    return OK;
}

status_t FelixCamera::process3A(FelixMetadata &frameSettings) {
    LOG_DEBUG("E");
    status_t res = OK;
    camera_metadata_entry e;

    e = frameSettings.find(ANDROID_CONTROL_MODE);
    if (e.count == 0) {
        LOG_ERROR("No control mode entry!");
        return BAD_VALUE;
    }
    camera_metadata_enum_android_control_mode_t controlMode =
        (camera_metadata_enum_android_control_mode_t)e.data.u8[0];

    uint8_t sceneMode = ANDROID_CONTROL_SCENE_MODE_DISABLED;
    e = frameSettings.find(ANDROID_CONTROL_SCENE_MODE);
    if(e.count && e.data.u8) {
        sceneMode = *e.data.u8;
    }

    LOG_DEBUG("process3A: control mode=%d scene_mode=%d",
            controlMode, sceneMode);
    if (controlMode == ANDROID_CONTROL_MODE_USE_SCENE_MODE &&
        sceneMode != ANDROID_CONTROL_SCENE_MODE_DISABLED &&
        sceneMode != ANDROID_CONTROL_SCENE_MODE_FACE_PRIORITY) {

        //
        LOG_ERROR("Only DISABLED and FACE_PRIORITY modes supported yet!");
        res = BAD_VALUE;
    }
    // FelixAF handles ANDROID_CONTROL_MODE internally
    if(mAfSupported) {
        FelixAF *afModule =
                mCameraHw.getMainPipeline()->getControlModule<FelixAF>();
        if(afModule) {
            afModule->updateHALMetadata(frameSettings);
        }
    } else {
        // provide default values so framework won't issue any errors
        const uint8_t afState = (uint8_t)ANDROID_CONTROL_AF_STATE_INACTIVE;
        const int32_t afTrigger = 0;
        frameSettings.update(ANDROID_CONTROL_AF_STATE,(uint8_t *) &afState, 1);
        frameSettings.update(ANDROID_CONTROL_AF_TRIGGER_ID, &afTrigger, 1);
    }
    FelixAWB *awbModule =
            mCameraHw.getMainPipeline()->getControlModule<FelixAWB>();
    if(awbModule) {
        awbModule->updateHALMetadata(frameSettings);
    }
    FelixAE *aeModule =
            mCameraHw.getMainPipeline()->getControlModule<FelixAE>();
    if(aeModule) {
        aeModule->updateHALMetadata(frameSettings);
    }
    return res;
}

status_t FelixCamera::WaitUntilIdle() {
    status_t res = NO_ERROR;

    // Check once with timeout
    if(!mProcessingThread->isIdle()) {
        // 1 second
        res = mCameraIdleSignal.waitRelative(mLock, 5000000000L);
        if(res == TIMED_OUT) {
            LOG_WARNING("TIME OUT waiting for idle");
        }
    }
    // this method will be called in configureStreams api only
    // if res is other than NO_ERROR, probably thread is in locked state
    // so we can return critical device error
    return res;
}

void FelixCamera::signalProcessingThreadIdle() {
    mLock.lock();
    if(mStatus == STATUS_ACTIVE) {
        LOG_DEBUG("mStatus = STATUS_READY");
        mStatus = STATUS_READY;
    }
    mLock.unlock();
    mCameraIdleSignal.signal();
    LOG_DEBUG("Now idle...");
}

camera3_device_ops_t FelixCamera::sDeviceOps = {
    FelixCamera::initialize,
    FelixCamera::configure_streams,
    FelixCamera::register_stream_buffers,
    FelixCamera::construct_default_request_settings,
    FelixCamera::process_capture_request,
    FelixCamera::get_metadata_vendor_tag_ops,
    FelixCamera::dump,
    NULL,   // flush
    { NULL }    // reserved
};

}; // namespace android
