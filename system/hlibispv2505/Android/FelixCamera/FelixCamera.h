/**
******************************************************************************
@file FelixCamera.h

@brief Declaration of main FelixCamera class in v2500 HAL module

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

#ifndef FELIX_CAMERA_H
#define FELIX_CAMERA_H

#include "FelixMetadata.h"
#include "Helpers.h"

#include "hardware/camera3.h"
#include "hardware/camera_common.h"

#include <utils/RefBase.h>
#include <utils/List.h>
#include <utils/Vector.h>
#include <utils/Mutex.h>
#include <utils/KeyedVector.h>
#include <ui/GraphicBufferAllocator.h>
#include "gralloc_priv.h"

#include "ispc/Camera.h"
#include "ispc/Sensor.h"
#include "ispc/ISPCDefs.h"
#include "ispc/Average.h"
#include "sensorapi/sensorapi.h"
#include <felixcommon/userlog.h>
#ifdef USE_GPU_LIB
#include "cameragpu_api.h"
#endif

namespace android {

#ifndef USE_GPU_LIB
class camera_gpu{
public:
    camera_gpu(unsigned , unsigned, unsigned , unsigned) {}
};
#endif //USE_GPU_LIB

#ifdef USE_GPU_LIB
#define MAX_GPU_STREAM_USE_COUNT 10
#endif

// forward declarations
class ProcessingThread;
class CaptureRequest;
class JpegEncoder;
class HwCaps;


class CameraGpu : public camera_gpu, public RefBase {
    virtual ~CameraGpu(){}
public:
    CameraGpu(unsigned YUVresX, unsigned YUVresY, unsigned RGBAresX, unsigned RGBAresY)
        : camera_gpu(YUVresX, YUVresY, RGBAresX, RGBAresY)
    {
        // do nothing here, this is just to call right camera_gpu constructor.
    };
};


enum {
    GPU_STREAM_NONE = 0,
    GPU_STREAM_PREVIEW_RGB,
    GPU_STREAM_PREVIEW_YUV,
    GPU_STREAM_ENCODER
};

typedef struct GPUstreamInfo_tag {
    int target;
    camera3_stream_t *stream;
    int requestedCount;
    int usedCount;
    bool requested;
} GPUstreamInfo_t;
//
// Encapsulates functionality for a v3.0 HAL for Felix camera IP
//
class FelixCamera : public camera3_device, public RefBase {
    friend class ProcessingThread;
    friend class CaptureRequest;
    friend class FelixMetadata;

public:
    FelixCamera(int cameraId, bool facingBack, struct hw_module_t *module);
    virtual ~FelixCamera();

    status_t Initialize(
        const std::string& sensorName,
        const std::string& defaultConfig,
        const uint8_t sensorMode = 0,
        const int flip = (int)SENSOR_FLIPPING::SENSOR_FLIP_NONE,
        const bool externalDG = false);

    // ------------------------------------------------------------------------
    // Camera module API and generic hardware device API implementation
    // ------------------------------------------------------------------------

    static int close(struct hw_device_t *device);
    status_t connectCamera(hw_device_t **device);
    status_t closeCamera();
    status_t getCameraInfo(struct camera_info *info);
    static FelixCamera* getInstance(const camera3_device_t *d);

protected:
    void sendNotify(camera3_notify_msg_t *msg);
    // Callbacks back to the framework
    void sendCaptureResult(camera3_capture_result_t *result);

    // implementation of HAL device operations
    virtual status_t initializeDevice(const camera3_callback_ops *callbackOps);
    virtual status_t configureStreams(camera3_stream_configuration *streamList);
    virtual status_t registerStreamBuffers(const camera3_stream_buffer_set *bufferSet) ;
    virtual const camera_metadata_t* constructDefaultRequestSettings(int type);
    virtual status_t processCaptureRequest(camera3_capture_request *request);

    HwManager& getHwInstance() { return mCameraHw; }

    status_t reconfigureCameraGPU(camera3_stream_t const * const newStream);

    void dumpInfo(int fd);

    const char *getVendorSectionName(uint32_t tag);
    const char *getVendorTagName(uint32_t tag);
    int getVendorTagType(uint32_t tag);

    void signalProcessingThreadIdle();
    // framework notification
    void notifyShutter(const uint32_t frameNumber, const nsecs_t timestamp);
    void notifyError(const uint32_t frameNumber,
            camera3_stream_t * const stream,
            const camera3_error_msg_code_t code);
    void notifyFatalError(void);
    void notifyRequestError(const uint32_t frameNumber,
            camera3_stream_t * const stream = NULL);
    void notifyMetadataError(const uint32_t frameNumber,
            camera3_stream_t * const stream);
    void notifyBufferError(const uint32_t frameNumber,
            camera3_stream_t * const stream);

    virtual status_t process3A(FelixMetadata &frameSettings);

    /**
     * @brief starts capture on all pipelines
     */
    status_t startCapture();

    /**
     * @brief starts capture on all pipelines
     */
    status_t stopCapture();

    /**
     * @brief reprogram all pipelines
     */
    status_t program();

    //
    // Static implementation of device operations - directly provided to
    // the android framework
    //
    static int initialize(const struct camera3_device *,
            const camera3_callback_ops_t *callback_ops);
    static int configure_streams(const struct camera3_device *,
            camera3_stream_configuration_t *stream_list);
    static int register_stream_buffers(const struct camera3_device *,
            const camera3_stream_buffer_set_t *buffer_set);
    static const camera_metadata_t *construct_default_request_settings(
            const struct camera3_device *, int type);
    static int process_capture_request(const struct camera3_device *,
            camera3_capture_request_t *request);
    static void get_metadata_vendor_tag_ops(const camera3_device_t *,
            vendor_tag_query_ops_t *ops);
    static const char *get_camera_vendor_section_name(
            const vendor_tag_query_ops_t *, uint32_t tag);
    static const char *get_camera_vendor_tag_name(
            const vendor_tag_query_ops_t *, uint32_t tag);
    static int get_camera_vendor_tag_type(
            const vendor_tag_query_ops_t *,uint32_t tag);
    static void dump(const camera3_device_t *, int fd);

    // error handling
    status_t deviceError(void) {
        mStatus = STATUS_ERROR;
        return NO_INIT;
    }

    status_t WaitUntilIdle();

    virtual bool isFlushing(void) { return false; }

    //
    // Build the static info metadata buffer for this device
    //
    virtual status_t constructStaticInfo(void);
    virtual void initJpegCapabilities(CameraMetadata& info);
    virtual void initRequestJpegSettings(int type, CameraMetadata& info);
    virtual void initLensCapabilities(CameraMetadata& info,
        const sensor_t& sensor);
    virtual status_t initSensorCapabilities(CameraMetadata& info,
        const sensor_t& sensor);
    virtual void initScalerCapabilities(CameraMetadata& info);
    virtual void initAAACapabilities(CameraMetadata& info);
    virtual void initStatsCapabilities(CameraMetadata& info);

    /**
     * @brief Configure all active digital scalers using data read from
     * ANDROID_SCALER_CROP_REGION
     *
     * @param metadata contains crop region info
     * @return status of operation
     */
    status_t processNewCropRegion(FelixMetadata& metadata);

    //
    // Helper methods
    //
    static void dumpStreamInfo(const camera3_stream_t *stream);

    // ------------------------------------------------------------------------
    // Data members
    // ------------------------------------------------------------------------
    static camera3_device_ops_t   sDeviceOps;
    const  camera3_callback_ops_t *mCallbackOps;
    // Cache for default templates. Once one is requested, the pointer must be
    // valid at least until close() is called on the device
    camera_metadata_t *mDefaultRequestTemplates[CAMERA3_TEMPLATE_COUNT];

    // Fixed camera information for camera2 devices. Must be valid to access if
    // mCameraDeviceVersion is >= HARDWARE_DEVICE_API_VERSION(2,0)
    camera_metadata_t *mCameraStaticInfo;

    struct TagOps : public vendor_tag_query_ops {
        FelixCamera *parent;
    };

    TagOps mVendorTagOps;

    // Zero-based ID assigned to this camera.
    int mCameraID;

    // Facing back (true) or front (false) switch.
    bool  mFacingBack;

    // Full mode (true) or limited mode (false) switch.
    bool  mFullMode;
    bool  mCaptureStarted;

    // ------------------------------------------------------------------------
    // Static configuration information
    // ------------------------------------------------------------------------

    static const uint32_t kMaxBufferCount = 7;
    static const uint32_t kMaxShotsCount = kMaxBufferCount;

    static const int32_t kHistogramBucketCount;
    static const int32_t kMaxHistogramCount;

    // storage for currently configured streams
    typedef List<PrivateStreamInfo*>           StreamList;
    typedef StreamList::iterator               StreamIterator;

    camera3_stream_t*  mInputStream; ///<@brief Shortcut to the input stream
    StreamList         mStreams; ///<@brief All streams, including input stream

    Mutex mLock;        ///<@brief HAL interface serialization lock
    Condition           mCameraIdleSignal; ///<@brief Signal the idle state

    bool                mPoolInitialized; ///<@brief is Shot pool allocated?
    FelixMetadata        mFrameSettings; ///<@brief last request metadata
    HwManager            mCameraHw;      ///<@brief abstraction of ISP hardware

    sp<ProcessingThread> mProcessingThread; ///<@brief request processing
    sp<CameraGpu>        mGPU; ///<@brief gpu vision processing instance
    Vector<GPUstreamInfo_t> mGPUstream;
    int                 mGPUstreamCurrentIndex;
    sp<HwCaps>          mHwCapabilities;

    bool                mAfSupported; ///<@brief is AF supported by the lens?
    std::string         mDefaultConfig; ///<@brief ISPC config filename

    /**
     * @brief handle to HAL allocated non ZSL buffer
     * @note used in cases when requests involving JPEG compression do not
     * provide ZSL input stream
     *
     * @ change to sp
     */
    sp<localStreamBuffer> mNonZslOutputBuffer;

    enum {
        // State at construction time, and after a device operation error
        STATUS_ERROR = 0,
        // State after startup-time init and after device instance close
        STATUS_CLOSED,
        // State after being opened, before device instance init
        STATUS_OPEN,
        // State after device instance initialization
        STATUS_READY,
        // State while actively capturing data
        STATUS_ACTIVE
    } mStatus;

};

//
// Encapsulates functionality for a v3.2 HAL for Felix camera IP
//
class FelixCamera3v2 : public FelixCamera {
    friend class ProcessingThread;
    friend class CaptureRequest;

public:
    FelixCamera3v2(int cameraId, bool facingBack, struct hw_module_t *module);
    virtual ~FelixCamera3v2();

protected:
    static camera3_device_ops_t   sDeviceOps;

    // implementation of HAL device operations
    virtual status_t registerStreamBuffers(
            const camera3_stream_buffer_set *bufferSet);
    virtual const camera_metadata_t* constructDefaultRequestSettings(
            int type);

    /**
     * @brief initializes scaler metadata for HAL>=3.2
     * @note calls FelixCamera::initScalerCapabilities() for backward
     * compatibility
     */
    virtual void initScalerCapabilities(CameraMetadata& info);

    /**
     * @brief flush request processing queue
     */
    virtual status_t flushQueue(void);

    /**
     * @brief return true if queue is being flushed
     */
    virtual bool isFlushing(void);

    /**
     * @brief construct static info metadata for HAL v3.2
     * @note calls FelixCamera::constructStaticInfo() too
     */
    virtual status_t constructStaticInfo(void);

    static int flush(const struct camera3_device *);

    /**
     * @brief Extracts all keys which exist in camera_metadata_t structure
     * @note depends on camera_metadata_section_bounds[][] structure
     * @note Used to accurately fill android.request.availableCharacteristicsKeys
     * from mCameraStaticInfo
     */
    status_t extractKeysFromRawMetadata(const camera_metadata_t* metadata,
            Vector<int32_t>& output);

    /**
     * @brief contains the list of static characteristics supported by HAL
     * and reported in in field
     * android.request.availableCharacteristicsKeys
     *
     * @note this is a Vector<> because this list is generated at runtime
     * by extractKeysFromRawMetadata()
     */
    Vector<int32_t> mSupportedCharacteristicsKeys;

    /**
     * @brief contains the list of metadata keys which are read by HAL when
     * capture requests are processed
     *
     * @note These keys belong to the control group and are mostly
     * preinitialized  in constructDefaultRequestSettings(). In general,
     * all keys which are searched by find(key) method for, should exist here
     */
    static const int32_t mSupportedRequestKeys[];

    /**
     * @brief contains the list of metadata keys which are written by HAL into
     * capture results
     *
     * @note These keys belong to the dynamic group. In general,
     * all keys which are written by update(key) method, should exist here.
     */
    static const int32_t mSupportedResultKeys[];

};

} // namespace android

#endif // FELIX_CAMERA_H
