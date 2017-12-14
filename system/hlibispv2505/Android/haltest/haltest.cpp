#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define LOG_TAG "haltest"
#define LOG_NDEBUG 0
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#include <android/log.h>
#include <utils/Errors.h>
#include <utils/Singleton.h>
#include <utils/Vector.h>
#include <utils/Mutex.h>
#include <utils/String8.h>
#include <hardware/camera2.h>
#include <hardware/camera3.h>
#include <system/camera_metadata.h>
#include <device3/Camera3Stream.h>
#include <binder/ProcessState.h>
#include <media/stagefright/MetaData.h>
#include <media/stagefright/MediaDefs.h>
#include <media/stagefright/OMXClient.h>
#include <media/stagefright/OMXCodec.h>
#include <media/stagefright/MPEG4Writer.h>
#include <media/stagefright/foundation/ADebug.h>

#include "haltest.h"

// Only output stream is needed.
#define NUMBER_OF_STREAMS (2) // Two output streams (fb and encoder)
#define GETTING_CAPTURE_RESULT_INTERVAL_US (1000000)

#define PARAM_ENCODER_FRAME_RATE_FPS (30)
#define PARAM_ENCODER_COLOR_FORMAT (OMX_COLOR_FormatYUV420Planar)
#define PARAM_ENCODER_CODEC_TYPE (MEDIA_MIMETYPE_VIDEO_MPEG4)
#define PARAM_ENCODER_BITRATE (300000)
#define PARAM_ENCODER_I_FRAME_INTERVAL_S (1)
#define PARAM_ENCODER_VIDEO_LEVEL (-1)
#define PARAM_ENCODER_VIDEO_PROFILE (-1)
#define PARAM_ENCODER_PREFER_SW_CODEC (false)
#define PARAM_ENCODER_FILE_NAME ("/data/camera_compared.mpeg")
#define PARAM_ENCODER_WAIT_FOR_EOS_US (100000)

//#define ENCODER_SUPPORT_ENABLED

using namespace android;

//-----------------------------------------------------------------------------
//
// Memory Allocator
//
//-----------------------------------------------------------------------------
MemoryAllocator::MemoryAllocator() :
      mGrallocModule(NULL)
    , mGrallocDevice(NULL) {
    status_t err = hw_get_module(GRALLOC_HARDWARE_MODULE_ID,
            reinterpret_cast<const hw_module_t **>(&mGrallocModule));

    if (err != OK) {
        ALOGE("Couldn't get gralloc module: %s (%d)", strerror(-err), err);
        return;
    }

    err = gralloc_open(reinterpret_cast<const hw_module_t *>(mGrallocModule),
            &mGrallocDevice);

    if (err != OK) {
        ALOGE("Couldn't open gralloc device: %s (%d)", strerror(-err), err);
        return;
    }
}
//-----------------------------------------------------------------------------
MemoryAllocator::~MemoryAllocator() {
    status_t err = gralloc_close(mGrallocDevice);

    if (err != OK) {
        ALOGE("Couldn't close gralloc device: %s (%d)", strerror(-err), err);
    }
}
//-----------------------------------------------------------------------------
buffer_handle_t MemoryAllocator::allocateGrallocBuffer(unsigned width,
        unsigned height, int usage, int *stride, unsigned bufferFormat) {
    ALOGD("%s", __FUNCTION__);

    buffer_handle_t buffer;

    status_t err = mGrallocDevice->alloc(mGrallocDevice, width,
            height, bufferFormat, usage, &buffer, stride);

    if (err != OK) {
        ALOGE("buffer allocation failed w=%d, h=%d, err=%s", width, height,
                strerror(-err));
        return NULL;
    }
    return buffer;
}
//-----------------------------------------------------------------------------
void MemoryAllocator::freeGrallocBuffer(buffer_handle_t buffer) {
    ALOGD("%s", __FUNCTION__);

    status_t err = mGrallocDevice->free(mGrallocDevice, buffer);

    if (err != OK) {
        ALOGE("gralloc buffer free failed: %s", strerror(-err));
    }
}
//-----------------------------------------------------------------------------
int MemoryAllocator::lockBuffer(buffer_handle_t buffer, int usage, int l,
        int t, int w, int h, void **vaddr) {
    return mGrallocModule->lock(mGrallocModule, buffer, usage, l, t, w, h,
            vaddr);
}
//-----------------------------------------------------------------------------
int MemoryAllocator::unlockBuffer(buffer_handle_t buffer) {
    return mGrallocModule->unlock(mGrallocModule, buffer);
}
//-----------------------------------------------------------------------------
//
// Camera3
//
//-----------------------------------------------------------------------------
Camera3::Camera3(unsigned cameraId)
    : mCameraId(cameraId)
    , mCameraModule(NULL)
    , mCameraDevice(NULL)
    , mNumberOfCameras(0)
    , mFrameNumber(0)
    , mInitialized(false) {
    ALOGD("%s", __FUNCTION__);

    camera3_callback_ops::notify = &sNotify;
    camera3_callback_ops::process_capture_result = &sProcessCaptureResult;

    // Get camera module.
    if (hw_get_module(CAMERA_HARDWARE_MODULE_ID,
                (const hw_module_t **)&mCameraModule) < 0) {
        ALOGE("Could not load camera HAL module\n");
        return;
    }

    mNumberOfCameras = mCameraModule->get_number_of_cameras();
    ALOGI("Loaded \"%s\" camera module, number of cameras = %d",
            mCameraModule->common.name,
            mNumberOfCameras);
    ALOGI("Camera module API version: %x",
            mCameraModule->common.module_api_version);

    if (mCameraModule->common.module_api_version >=
            CAMERA_MODULE_API_VERSION_2_1)
        mCameraModule->set_callbacks(this);

    // Get camera device.
    String8 deviceName = String8::format("%d", mCameraId);

    status_t err = mCameraModule->common.methods->open(
            &mCameraModule->common, deviceName.string(),
            reinterpret_cast<hw_device_t **>(&mCameraDevice));

    if (err != OK) {
        ALOGE("Could not open camera: %s (%d)", strerror(-err), err);
        return;
    }

    ALOGI("Camera device API version: %x", mCameraDevice->common.version);

    // Check device version
    if (mCameraDevice->common.version != CAMERA_DEVICE_API_VERSION_3_0) {
        ALOGE("Camera device is not version %x, reports %x instead.",
                CAMERA_DEVICE_API_VERSION_3_0, mCameraDevice->common.version);
        return;
    }

    err = mCameraModule->get_camera_info(mCameraId, &mInfo);
    if (err != OK ) {
        ALOGE("Cannot get camera info!");
        return;
    }

    if (mInfo.device_version != mCameraDevice->common.version) {
        ALOGE("%s: HAL reporting mismatched camera_info version (%x)"
                " and device version (%x).", __FUNCTION__,
                mCameraDevice->common.version, mInfo.device_version);
        return;
    }

    // Initialize device
    err = mCameraDevice->ops->initialize(mCameraDevice, this);
    if (err != OK) {
        ALOGE("Unable to initialize HAL device: %s (%d)", strerror(-err), err);
        return;
    }

    // Get vendor metadata tags
    mVendorTagOps.get_camera_vendor_section_name = NULL;
    mCameraDevice->ops->get_metadata_vendor_tag_ops(mCameraDevice,
            &mVendorTagOps);

    if (mVendorTagOps.get_camera_vendor_section_name != NULL) {
        err = set_camera_metadata_vendor_tag_ops(&mVendorTagOps);
        if (err != OK) {
            ALOGE("Unable to set tag ops: %s (%d)", strerror(-err), err);
            return;
        }
    }

    mInitialized = true;
    ALOGI("Camera initialized");
}
//-----------------------------------------------------------------------------
Camera3::~Camera3() {
    ALOGD("%s", __FUNCTION__);

    status_t err = mCameraDevice->common.close(reinterpret_cast<hw_device_t *>(
                mCameraDevice));

    if (err != OK) {
        ALOGE("Could not close camera device: %s (%d)", strerror(-err), err);
        return;
    }
}
//-----------------------------------------------------------------------------
void Camera3::sProcessCaptureResult(const camera3_callback_ops *cb,
        const camera3_capture_result *result) {
    ALOGD("%s", __FUNCTION__);
    Camera3 *d = const_cast<Camera3 *>(static_cast<const Camera3 *>(cb));
    d->processCaptureResult(result);
}
//-----------------------------------------------------------------------------
void Camera3::sNotify(const camera3_callback_ops *cb,
        const camera3_notify_msg *msg) {
    ALOGD("%s", __FUNCTION__);
    Camera3 *d = const_cast<Camera3 *>(static_cast<const Camera3 *>(cb));
    d->notify(msg);
}
//-----------------------------------------------------------------------------
status_t Camera3::configureOutputStream(
        buffer_handle_t **encoderBuffers, buffer_handle_t **displayBuffers,
        unsigned buffersCount, unsigned width, unsigned height,
        unsigned encoderPixelFormat, unsigned displayPixelFormat) {
    ALOGD("Configuring output streams...");

    // Configure streams
    Vector<camera3_stream_t*> streams;

    if (!mInitialized)
    {
        ALOGE("Camera3 not initialized.\n");
        return NO_INIT;
    }

    // Encoder output stream
    mEncoderOutputStream.stream_type = CAMERA3_STREAM_OUTPUT;
    mEncoderOutputStream.width = width;
    mEncoderOutputStream.height = height;
    mEncoderOutputStream.format = encoderPixelFormat;
    mEncoderOutputStream.usage = GRALLOC_USAGE_HW_VIDEO_ENCODER;
    mEncoderOutputStream.max_buffers = buffersCount;
    mEncoderOutputStream.priv = NULL;
    streams.add(&mEncoderOutputStream);

    // Display output stream
    mDisplayOutputStream.stream_type = CAMERA3_STREAM_OUTPUT;
    mDisplayOutputStream.width = width;
    mDisplayOutputStream.height = height;
    mDisplayOutputStream.format = displayPixelFormat;
    mDisplayOutputStream.usage = GRALLOC_USAGE_HW_FB;
    mDisplayOutputStream.max_buffers = buffersCount;
    mDisplayOutputStream.priv = NULL;
    streams.add(&mDisplayOutputStream);

    mConfig.streams = streams.editArray();
    mConfig.num_streams = NUMBER_OF_STREAMS;

    status_t err = mCameraDevice->ops->configure_streams(
            mCameraDevice, &mConfig);

    if (err != OK) {
        ALOGE("Unable to configure streams with HAL: %s (%d)",
                strerror(-err), err);
        return err;
    }

    // Register buffers for encoder stream
    mEncoderOutputBufferSet.stream = &mEncoderOutputStream;
    mEncoderOutputBufferSet.num_buffers = buffersCount;
    mEncoderOutputBufferSet.buffers = encoderBuffers;

    ALOGD("Registering stream buffers...");
    err = mCameraDevice->ops->register_stream_buffers(mCameraDevice,
            &mEncoderOutputBufferSet);

    if (err!= OK) {
        ALOGE("Couldn't register encoder output stream buffers: %s (%d)",
                strerror(-err), err);
        return err;
    }

    // Register buffers for display stream
    mDisplayOutputBufferSet.stream = &mDisplayOutputStream;
    mDisplayOutputBufferSet.num_buffers = buffersCount;
    mDisplayOutputBufferSet.buffers = displayBuffers;

    err = mCameraDevice->ops->register_stream_buffers(mCameraDevice,
            &mDisplayOutputBufferSet);

    if (err!= OK) {
        ALOGE("Couldn't register display output stream buffers: %s (%d)",
                strerror(-err), err);
        return err;
    }

    return err;
}
//-----------------------------------------------------------------------------
status_t Camera3::startCapture(int type, unsigned &frameNo) {
    // Request default settings
    ALOGD("Requesting default settings...");
    const camera_metadata_t *metadata =
        mCameraDevice->ops->construct_default_request_settings(
                mCameraDevice, type);

    if (!mInitialized)
    {
        ALOGE("Camera3 not initialized.\n");
        return NO_INIT;
    }

    if (!metadata) {
        ALOGE("Unable to construct default settings for template %d",
                type);
        return DEAD_OBJECT;
    }

    // Send capture request
    ALOGD("Sending capture request...");
    camera3_capture_request_t request = camera3_capture_request_t();

    // Encoder buffers
    for (unsigned buffIdx = 0; buffIdx < mEncoderOutputBufferSet.num_buffers;
            buffIdx++) {
        camera3_stream_buffer_t encoderOutputStreamBuffer;

        encoderOutputStreamBuffer.stream = &mEncoderOutputStream;
        encoderOutputStreamBuffer.buffer =
            mEncoderOutputBufferSet.buffers[buffIdx];
        encoderOutputStreamBuffer.status = CAMERA3_BUFFER_STATUS_OK;
        encoderOutputStreamBuffer.acquire_fence = -1;
        encoderOutputStreamBuffer.release_fence = -1;

        mOutputStreamsBuffers.insertAt(encoderOutputStreamBuffer, buffIdx * 2);
    }

    // Display buffers
    for (unsigned buffIdx = 0; buffIdx < mDisplayOutputBufferSet.num_buffers;
            buffIdx++) {
        camera3_stream_buffer_t displayOutputStreamBuffer;

        displayOutputStreamBuffer.stream = &mDisplayOutputStream;
        displayOutputStreamBuffer.buffer =
            mDisplayOutputBufferSet.buffers[buffIdx];
        displayOutputStreamBuffer.status = CAMERA3_BUFFER_STATUS_OK;
        displayOutputStreamBuffer.acquire_fence = -1;
        displayOutputStreamBuffer.release_fence = -1;

        mOutputStreamsBuffers.insertAt(displayOutputStreamBuffer,
                buffIdx * 2 + 1);
    }
    request.frame_number = mFrameNumber++;
    request.settings = metadata;
    request.input_buffer = NULL;
    request.num_output_buffers = mEncoderOutputBufferSet.num_buffers +
        mDisplayOutputBufferSet.num_buffers;
    request.output_buffers = mOutputStreamsBuffers.array();

    // Return frame number
    frameNo = request.frame_number;

    status_t err = mCameraDevice->ops->process_capture_request(
            mCameraDevice, &request);
    if (err != OK) {
        ALOGE("Unable to submit capture request %d: %s (%d)",
                request.frame_number, strerror(-err), err);
        return err;
    }
    ALOGD("Capture request sent");

    return err;
}
//-----------------------------------------------------------------------------
Camera3::CaptureResult Camera3::getCapture(unsigned frameNo) {
    ALOGD("%s", __FUNCTION__);

    ssize_t idx = 0;
    if (!mInitialized)
    {
        ALOGE("Camera3 not initialized.\n");
    }

    while (true) {
        idx = mCaptureResults.indexOfKey(frameNo);
        if (idx == NAME_NOT_FOUND) {
            // wait for the requested frame capture
            usleep(GETTING_CAPTURE_RESULT_INTERVAL_US);
        } else break;
    }

    struct CaptureResult result = mCaptureResults.valueFor(frameNo);
    mCaptureResults.removeItem(frameNo);

    return result;
}
//-----------------------------------------------------------------------------
void Camera3::processCaptureResult(const camera3_capture_result *result) {
    ALOGD("%s", __FUNCTION__);

    uint32_t frameNumber = result->frame_number;
    if (!mInitialized)
    {
        ALOGE("Camera3 not initialized.\n");
    }

    if (result->result == NULL && result->num_output_buffers == 0) {
        ALOGE("No result data provided by HAL for frame %d", frameNumber);
        return;
    }

    ALOGD("Number of output buffers: %d", result->num_output_buffers);

    struct CaptureResult outerResult;

    for (size_t i = 0; i < result->num_output_buffers; i++) {
        if (result->output_buffers[i].status != CAMERA3_BUFFER_STATUS_OK) {
            ALOGE("Output buffer %d status is not OK.", i);
            //: Provide error outside
            break;
        }

        unsigned usage = result->output_buffers[i].stream->usage;
        if (usage & GRALLOC_USAGE_HW_VIDEO_ENCODER) {
            ALOGD("Encoder buffer captured.");
        }
        else if (usage & GRALLOC_USAGE_HW_FB) {
            ALOGD("Display buffer captured.");
        } else {
            ALOGE("Unrecognised buffer captured!");
            //: Provide error outside
            break;
        }

        outerResult.mFilledBuffers.add(CaptureResult::KVP(usage,
                    result->output_buffers[i].buffer));
    }
    mCaptureResults.add(frameNumber, outerResult);
}
//-----------------------------------------------------------------------------
void Camera3::notify(const camera3_notify_msg *msg) {
    ALOGD("%s", __FUNCTION__);

    if (!msg) {
        ALOGE("HAL sent NULL notify message!");
        return;
    }

    switch (msg->type) {
        case CAMERA3_MSG_ERROR: {
            int streamId = 0;
            if (msg->message.error.error_stream != NULL) {
                camera3::Camera3Stream *stream =
                    camera3::Camera3Stream::cast(
                            msg->message.error.error_stream);
                streamId = stream->getId();
            }
            ALOGE("Camera %d: %s: HAL error, frame %d, stream %d: %d",
                    mCameraId, __FUNCTION__, msg->message.error.frame_number,
                    streamId, msg->message.error.error_code);
            // Report error
            struct CaptureResult outerResult(msg->message.error);
            mCaptureResults.add(msg->message.error.frame_number, outerResult);
            break;
        }
        case CAMERA3_MSG_SHUTTER: {
            ssize_t idx;
            nsecs_t timestamp = msg->message.shutter.timestamp;
            uint32_t frameNumber = msg->message.shutter.frame_number;
            ALOGI("Camera %d: %s: Shutter fired for frame %d at %lld",
                    mCameraId, __FUNCTION__, frameNumber, timestamp);
            break;
        }
        default:
            ALOGE("Unknown notify message from HAL: %d", msg->type);
    }
}
//-----------------------------------------------------------------------------
//
// Dummy Source
//
//-----------------------------------------------------------------------------
DummySource::DummySource(int width, int height, int nFrames, int fps,
        int colorFormat)
        : mWidth(width)
        , mHeight(height)
        , mMaxNumFrames(nFrames)
        , mFrameRate(fps)
        , mColorFormat(colorFormat)
        , mSize(width * height)
        , mNumFramesInput(0) {
}
//-----------------------------------------------------------------------------
sp<MetaData> DummySource::getFormat() {
    sp<MetaData> meta = new MetaData;
    meta->setInt32(kKeyWidth, mWidth);
    meta->setInt32(kKeyHeight, mHeight);
    meta->setInt32(kKeyColorFormat, mColorFormat);
    meta->setCString(kKeyMIMEType, MEDIA_MIMETYPE_VIDEO_RAW);

    return meta;
}
//-----------------------------------------------------------------------------
status_t DummySource::addBuffer(void * cameraOutputBuffer) {
    if (mNumFramesInput == mMaxNumFrames) {
        ALOGE("Cannot add new buffer - limit reached (%d).", mMaxNumFrames);
        return ERROR_OUT_OF_RANGE;
    }

    // : Fix me.
    mGroup.add_buffer(new MediaBuffer(cameraOutputBuffer, mSize));
    mNumFramesInput++;
    return OK;
}
//-----------------------------------------------------------------------------
status_t DummySource::start(MetaData *params) {
    mNumFramesOutput = 0;
    return OK;
}
//-----------------------------------------------------------------------------
status_t DummySource::stop() {
    return OK;
}
//-----------------------------------------------------------------------------
status_t DummySource::read(MediaBuffer **buffer,
        const MediaSource::ReadOptions *options) {

    printf(".");

    if (mNumFramesOutput == mMaxNumFrames) {
        return ERROR_END_OF_STREAM;
    }

    status_t err = mGroup.acquire_buffer(buffer);
    if (err != OK) {
        return err;
    }

    (*buffer)->set_range(0, mSize);
    (*buffer)->meta_data()->clear();
    (*buffer)->meta_data()->setInt64(kKeyTime,
            (mNumFramesOutput * 1000000) / mFrameRate);
    ++mNumFramesOutput;

    return OK;
}
//-----------------------------------------------------------------------------
//
// Test Parameters
//
//-----------------------------------------------------------------------------
const char *TestParameters::mOptString = "i:w:g:c:o:h?";
//-----------------------------------------------------------------------------
void TestParameters::displayUsage() {
    printf("Usage: haltest [OPTION(s)]\n");
    printf("\nList of options:\n");
    for (unsigned optIdx = 0; optIdx < optionsCount(); optIdx++) {
        printf("%s\n", mTestOptions[optIdx].description);
    }
}
//-----------------------------------------------------------------------------
void TestParameters::loadParameters(int argc, char **argv) {
    int longIndex = 0;
    int opt = getopt_long(argc, argv, mOptString, mTestOptions, &longIndex);

    if (opt != 'h' && opt != '?')
            printf("\nArguments passed:\n");

    while (opt != -1) {
        switch (opt) {
            case 'i':
                mCameraId = atoi(optarg);
                printf("cameraid: %d\n", mCameraId);
                break;
            case 'w':
                mWidth = atoi(optarg);
                printf("width: %d\n", mWidth);
                break;
            case 'g':
                mHeight = atoi(optarg);
                printf("height: %d\n", mHeight);
                break;
            case 'c':
                mBuffersCountInOutputStream = atoi(optarg);
                printf("buffers-count: %d\n", mBuffersCountInOutputStream);
                break;
            case 'o':
                mOutputFileName = argv[optind];
                printf("output-file: %s\n", mOutputFileName);
                break;
            case 'h':
            case '?':
                displayUsage();
                exit(0);
                break;
            default:
                printf("Unrecognised argument: %c\n", opt);
                break;
        }
        opt = getopt_long(argc, argv, mOptString, mTestOptions, &longIndex);
    }
}
//-----------------------------------------------------------------------------
//
// Buffers Manager
//
//-----------------------------------------------------------------------------
BuffersManager::BuffersManager(size_t buffersCount,
        MemoryAllocator &memoryAllocator, unsigned width, unsigned height,
        int usage, int &stride, unsigned bufferFormat)
        : mBuffersCount(buffersCount)
        , mMemoryAllocator(memoryAllocator) {

    // Allocate requested number of buffers...
    mBuffers = (buffer_handle_t *)calloc(mBuffersCount, sizeof(*mBuffers));

    if (!mBuffers) {
        ALOGE("calloc failed!");
        return;
    }

    for (size_t buffIdx = 0; buffIdx < mBuffersCount; buffIdx++) {
        mBuffers[buffIdx] = mMemoryAllocator.allocateGrallocBuffer(width,
                height, usage, &stride, bufferFormat);
    }
}
//-----------------------------------------------------------------------------
BuffersManager::~BuffersManager() {
    for (size_t buffIdx = 0; buffIdx < mBuffersCount; buffIdx++) {
        mMemoryAllocator.freeGrallocBuffer(mBuffers[buffIdx]);
        mBuffers[buffIdx] = NULL;
    }
    free(mBuffers);
}
//-----------------------------------------------------------------------------
status_t BuffersManager::lockBuffer(size_t buffNo, int usage, int l, int t,
        int w, int h, void **vaddr) {
    if (buffNo >= mBuffersCount) {
        ALOGE("Buffer index out of range %d/%d!", buffNo, mBuffersCount);
        return BAD_INDEX;
    }
    return mMemoryAllocator.lockBuffer(mBuffers[buffNo], usage, l, t, w, h,
            vaddr);
}
//-----------------------------------------------------------------------------
status_t BuffersManager::unlockBuffer(size_t buffNo) {
    if (buffNo >= mBuffersCount) {
        ALOGE("Buffer index out of range %d/%d!", buffNo, mBuffersCount);
        return BAD_INDEX;
    }
    return mMemoryAllocator.unlockBuffer(mBuffers[buffNo]);
}
//-----------------------------------------------------------------------------
buffer_handle_t ** BuffersManager::getBuffers() {
    return &mBuffers;
}
//-----------------------------------------------------------------------------
ANDROID_SINGLETON_STATIC_INSTANCE(MemoryAllocator);
ANDROID_SINGLETON_STATIC_INSTANCE(TestParameters);
//-----------------------------------------------------------------------------
// This is very simple test of camera HAL v3. Image returned from camera will
// be coded with Video Encoder and written to a file.
//-----------------------------------------------------------------------------
status_t test(TestParameters &testParams) {

    Camera3 camera(testParams.mCameraId);
    status_t err = OK;

    buffer_handle_t *cameraOutputBuffers = (buffer_handle_t *)calloc(
            testParams.mBuffersCountInOutputStream,
            sizeof(*cameraOutputBuffers));

    int stride = 0;

    ALOGI("Creating buffer manager for encoder...");
    // Prepare buffers for encoder
    BuffersManager encoderBuffersManager(testParams.mBuffersCountInOutputStream,
            Singleton<MemoryAllocator>::getInstance(), testParams.mWidth,
            testParams.mHeight, GRALLOC_USAGE_HW_VIDEO_ENCODER |
            GRALLOC_USAGE_HW_CAMERA_WRITE | GRALLOC_USAGE_HW_CAMERA_READ,
            stride, testParams.mEncoderPixelFormat);

    ALOGI("Creating buffer manager for display...");

    // Prepare buffers for display
    BuffersManager displayBuffersManager(testParams.mBuffersCountInOutputStream,
            Singleton<MemoryAllocator>::getInstance(), testParams.mWidth,
            testParams.mHeight, GRALLOC_USAGE_HW_FB |
            GRALLOC_USAGE_HW_CAMERA_WRITE | GRALLOC_USAGE_HW_CAMERA_READ,
            stride, testParams.mDisplayPixelFormat);

    // Configure output stream
    err = camera.configureOutputStream(encoderBuffersManager.getBuffers(),
            displayBuffersManager.getBuffers(),
            testParams.mBuffersCountInOutputStream, testParams.mWidth,
            testParams.mHeight, testParams.mEncoderPixelFormat,
            testParams.mDisplayPixelFormat);

    if (err != OK) {
        ALOGE("Cannot configure camera output stream: %s (%d)", strerror(-err),
                err);
        return err;
    }

    sp<DummySource> source = new DummySource(stride, testParams.mHeight,
            testParams.mBuffersCountInOutputStream,
            PARAM_ENCODER_FRAME_RATE_FPS, PARAM_ENCODER_COLOR_FORMAT);

    // Process for all requested frames...
    for (unsigned frameNo = 0; frameNo <
            testParams.mBuffersCountInOutputStream; frameNo++) {

        err = camera.startCapture(testParams.mCameraTemplate, frameNo);
        if (err != OK) {
            ALOGE("Cannot start capture: %s (%d)", strerror(-err), err);
            return err;
        }

        ALOGI("Started capturing frame #%d", frameNo);

        struct Camera3::CaptureResult captureResult =
            camera.getCapture(frameNo);

        if (captureResult.mFilledBuffers.size() == 0)
        {
            ALOGE("Camera3 reported error: %d",
                    captureResult.mErrorMsg.error_code);
            return DEAD_OBJECT;
        }

        // Prepare output file
        String8 outputPathForTheFrame("/data/");
        outputPathForTheFrame.append(testParams.mOutputFileName);
        outputPathForTheFrame.appendFormat("_%d.out", frameNo);

        ALOGD("opening %s...", outputPathForTheFrame.string());

        FILE *file = fopen(outputPathForTheFrame, "w+");

        if (!file) {
            ALOGE("Cannot open %s file for writing.",
                    testParams.mOutputFileName);
            return DEAD_OBJECT;
        }

        unsigned dispBuffersIdx = 0;
        unsigned encBuffersIdx = 0;
        // Write capture result to a file
        for (unsigned buffIdx = 0;
                buffIdx < captureResult.mFilledBuffers.size(); buffIdx++) {
            void *capture = NULL;
            unsigned usage =
                captureResult.mFilledBuffers[buffIdx].getKey();

            // Only display buffers will be stored
            if (usage & GRALLOC_USAGE_HW_FB) {
                CHECK_EQ(displayBuffersManager.lockBuffer(dispBuffersIdx,
                            GRALLOC_USAGE_HW_FB, 0, 0, 0, 0, &capture),
                        (status_t)OK);
                fwrite(capture, sizeof(uint8_t), stride * testParams.mHeight *
                        testParams.mBytesPerPixel, file);
                CHECK_EQ(displayBuffersManager.unlockBuffer(dispBuffersIdx),
                        (status_t)OK);
                dispBuffersIdx++;
            } else if (usage & GRALLOC_USAGE_HW_VIDEO_ENCODER) {
                CHECK_EQ(encoderBuffersManager.lockBuffer(encBuffersIdx,
                            GRALLOC_USAGE_HW_VIDEO_ENCODER, 0, 0, 0, 0,
                            &capture), (status_t)OK);
#ifdef ENCODER_SUPPORT_ENABLED
                // Send the buffer to encoder
                CHECK_EQ(source->addBuffer(capture), (status_t)OK);
#endif
                CHECK_EQ(encoderBuffersManager.unlockBuffer(encBuffersIdx),
                        (status_t)OK);
                encBuffersIdx++;
            }
        }

        // Close camera output files.
        fflush(file);
        fclose(file);
    }

#ifdef ENCODER_SUPPORT_ENABLED
    // Prepare encoder
    sp<MetaData> encoderMetaData = new MetaData;

    encoderMetaData->setCString(kKeyMIMEType, PARAM_ENCODER_CODEC_TYPE);
    encoderMetaData->setInt32(kKeyWidth, stride);
    encoderMetaData->setInt32(kKeyHeight, testParams.mHeight);
    encoderMetaData->setInt32(kKeyFrameRate, PARAM_ENCODER_FRAME_RATE_FPS);
    encoderMetaData->setInt32(kKeyBitRate, PARAM_ENCODER_BITRATE);
    encoderMetaData->setInt32(kKeyStride, stride);
    encoderMetaData->setInt32(kKeySliceHeight, testParams.mHeight);
    encoderMetaData->setInt32(kKeyIFramesInterval,
            PARAM_ENCODER_I_FRAME_INTERVAL_S);
    encoderMetaData->setInt32(kKeyColorFormat, PARAM_ENCODER_COLOR_FORMAT);

    if (PARAM_ENCODER_VIDEO_LEVEL != -1) {
        encoderMetaData->setInt32(kKeyVideoLevel, PARAM_ENCODER_VIDEO_LEVEL);
    }

    if (PARAM_ENCODER_VIDEO_PROFILE != -1) {
        encoderMetaData->setInt32(kKeyVideoProfile,
            PARAM_ENCODER_VIDEO_PROFILE);
    }

    OMXClient client;
    err = client.connect();
    if (err != OK) {
        ALOGE("Failed to connect OMX client!");
    }

#ifdef ENCODER_SUPPORT_ENABLED
    sp<MediaSource> encoder = OMXCodec::Create(client.interface(),
            encoderMetaData, true /* createEncoder */, source, 0,
                PARAM_ENCODER_PREFER_SW_CODEC ?
                OMXCodec::kPreferSoftwareCodecs : 0);
#endif

    sp<MPEG4Writer> writer = new MPEG4Writer(PARAM_ENCODER_FILE_NAME);
    writer->addSource(encoder);

    err = writer->start();
    if (err != OK) {
        ALOGE("Failed to start MPEG4 writer!");
    }

    while (!writer->reachedEOS()) {
        usleep(PARAM_ENCODER_WAIT_FOR_EOS_US);
    }

    err = writer->stop();
    if (err != OK) {
        ALOGE("Failed to stop MPEG4 writer!");
    }

    printf("$\n");
    client.disconnect();

    if (err != OK && err != ERROR_END_OF_STREAM) {
        printf("record failed: %d\n", err);
    }
#endif
    return err;
}
//-----------------------------------------------------------------------------
int main(int argc, char **argv) {

    TestParameters &testParams = Singleton<TestParameters>::getInstance();
    testParams.loadParameters(argc, argv);

    ProcessState::self()->startThreadPool();

    status_t err = test(testParams);

    if (err == OK)
        printf("Test completed successfully.\n");
    else
        printf("Test failed.\n");

    return err;
}
//-----------------------------------------------------------------------------
