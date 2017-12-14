#ifndef HAL_TEST_H
#define HAL_TEST_H

#include <hardware/camera3.h>
#include <device3/Camera3Device.h>
#include <utils/KeyedVector.h>
#include <utils/TypeHelpers.h>
#include <media/stagefright/MediaSource.h>
#include <media/stagefright/MediaBufferGroup.h>

//-----------------------------------------------------------------------------
// Camera3
//-----------------------------------------------------------------------------
class MemoryAllocator {
private:
    const gralloc_module_t * mGrallocModule;
    alloc_device_t *mGrallocDevice;

public:
    MemoryAllocator();
    ~MemoryAllocator();

    buffer_handle_t allocateGrallocBuffer(unsigned width, unsigned height,
            int usage, int *stride, unsigned bufferFormat);
    void freeGrallocBuffer(buffer_handle_t buffer);
    int lockBuffer(buffer_handle_t buffer, int usage, int l, int t, int w,
            int h, void **vaddr);
    int unlockBuffer(buffer_handle_t buffer);
};
//-----------------------------------------------------------------------------
// Camera3 class definition
//-----------------------------------------------------------------------------
class Camera3 : public camera_module_callbacks_t, private camera3_callback_ops {
private:
    unsigned mCameraId;
    camera_module_t *mCameraModule;
    camera3_device_t *mCameraDevice;

    camera_info mInfo;
    vendor_tag_query_ops_t mVendorTagOps;
    camera3_stream_configuration mConfig;

    camera3_stream_t mEncoderOutputStream;
    camera3_stream_t mDisplayOutputStream;

    camera3_stream_buffer_set mEncoderOutputBufferSet;
    camera3_stream_buffer_set mDisplayOutputBufferSet;

    android::Vector<camera3_stream_buffer_t> mOutputStreamsBuffers;

    unsigned mNumberOfCameras;
    unsigned mFrameNumber;

    bool mInitialized;

public:
    struct CaptureResult {
        CaptureResult() {}
        CaptureResult(camera3_error_msg_t errorMsg)
            : mErrorMsg(errorMsg) {}

        ~CaptureResult() {
            mFilledBuffers.clear();
        }
        camera3_error_msg_t mErrorMsg;
        typedef android::key_value_pair_t<unsigned, buffer_handle_t *> KVP;
        android::Vector<KVP> mFilledBuffers;
    };

private:
    android::KeyedVector<unsigned, struct CaptureResult> mCaptureResults;

    // Static callback forwarding methods from HAL to instance
    static callbacks_process_capture_result_t sProcessCaptureResult;
    static callbacks_notify_t sNotify;

    // Callback functions from HAL device
    void processCaptureResult(const camera3_capture_result *result);
    void notify(const struct camera3_notify_msg *msg);

    // Disable them to prevent copying
    Camera3(const Camera3&);
    void operator=(const Camera3&);

public:
    Camera3(unsigned cameraId);
    ~Camera3();

    android::status_t configureOutputStream(buffer_handle_t **encoderBuffers,
            buffer_handle_t **displayBuffers, unsigned buffersCount,
            unsigned width, unsigned height, unsigned encoderPixelFormat,
            unsigned displayPixelFormat);

    android::status_t startCapture(int type, unsigned &frameNo);
    struct CaptureResult getCapture(unsigned frameNo);
};
//-----------------------------------------------------------------------------
struct TestOption : public option {
    const char * description;
    TestOption() {}
    TestOption(const char *name, int hasArg, int *flag, int val,
            const char *desc)
        : description(desc) {
            name = name;
            has_arg = hasArg;
            flag = flag;
            val =val;
        }
};
//-----------------------------------------------------------------------------
// Test Parameters
//-----------------------------------------------------------------------------
struct TestParameters {
    TestParameters()
        : mCameraId(0)
        , mWidth(176)
        , mHeight(40)
        , mEncoderPixelFormat(HAL_PIXEL_FORMAT_YCrCb_420_SP/*HAL_IMG_FormatYUV420SP : Check this!!!*/)
        , mDisplayPixelFormat(HAL_PIXEL_FORMAT_RGB_888)
        , mBytesPerPixel(4)
        , mBuffersCountInOutputStream(1)
        , mCameraTemplate(CAMERA2_TEMPLATE_STILL_CAPTURE)
        , mOutputFileName("camera3_") {
            unsigned optIdx = 0;
            mTestOptions[optIdx++] = TestOption("cameraid", required_argument,
                    NULL, 0, "-i    Camera id.");
            mTestOptions[optIdx++] = TestOption("width", required_argument,
                    NULL, 0, "-w    Camera image width.");
            mTestOptions[optIdx++] = TestOption("height", required_argument,
                    NULL, 0, "-g    Camera image height.");
            mTestOptions[optIdx++] = TestOption("buffers-count",
                    optional_argument, NULL, 0,
                    "-c    Number of buffers in camera output stream.");
            mTestOptions[optIdx++] = TestOption("output-file",
                    optional_argument, NULL, 0, "-o    Output file name.");
            mTestOptions[optIdx++] = TestOption("help", no_argument, NULL, 'h',
                    "-h    Print this help.");
            mTestOptions[optIdx++] = TestOption(NULL, no_argument,
                    NULL, 0, "");
    }

public:
    void loadParameters(int argc, char **argv);
    void displayUsage();

    unsigned mCameraId;
    unsigned mWidth;
    unsigned mHeight;
    unsigned mEncoderPixelFormat;
    unsigned mDisplayPixelFormat;
    unsigned mBytesPerPixel;
    unsigned mBuffersCountInOutputStream;
    unsigned mCameraTemplate;
    const char *mOutputFileName;

private:
    static const char *mOptString;
    struct TestOption mTestOptions[7];

    unsigned optionsCount() {
        return sizeof(mTestOptions) / sizeof(mTestOptions[0]);
    }
};
//-----------------------------------------------------------------------------
// DummySource
//-----------------------------------------------------------------------------
class DummySource : public android::MediaSource {
public:
    DummySource(int width, int height,
            int nFrames, int fps, int colorFormat);
    android::status_t addBuffer(void * cameraOutputBuffer);
    virtual android::sp<android::MetaData> getFormat();
    virtual android::status_t start(android::MetaData *params);
    virtual android::status_t stop();
    virtual android::status_t read(android::MediaBuffer **buffer,
            const android::MediaSource::ReadOptions *options);

protected:
    virtual ~DummySource() {}

private:
    android::MediaBufferGroup mGroup;
    int mWidth, mHeight;
    int mMaxNumFrames;
    int mFrameRate;
    int mColorFormat;
    size_t mSize;
    int64_t mNumFramesOutput;
    int64_t mNumFramesInput;

    DummySource(const DummySource &);
    DummySource &operator=(const DummySource &);
};
//-----------------------------------------------------------------------------
// Buffers Manager
//-----------------------------------------------------------------------------
class BuffersManager {
private:
    size_t mBuffersCount;
    buffer_handle_t *mBuffers;
    MemoryAllocator &mMemoryAllocator;

    BuffersManager(const BuffersManager &);
    BuffersManager &operator=(const BuffersManager &);

public:
    BuffersManager(size_t buffersCount, MemoryAllocator &memoryAllocator,
            unsigned width, unsigned height, int usage, int &stride,
            unsigned bufferFormat);
    ~BuffersManager();

    android::status_t lockBuffer(size_t buffNo, int usage, int l, int t, int w,
            int h, void **vaddr);
    android::status_t unlockBuffer(size_t buffNo);
    buffer_handle_t **getBuffers();
};
//-----------------------------------------------------------------------------
#endif //  HAL_TEST_H

