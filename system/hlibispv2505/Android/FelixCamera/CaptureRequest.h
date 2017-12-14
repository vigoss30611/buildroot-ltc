/**
******************************************************************************
@file CaptureRequest.h

@brief Interface of CaptureRequest class in v2500 HAL module

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

#ifndef CAPTURE_REQUEST_H
#define CAPTURE_REQUEST_H

#include "FelixMetadata.h"
#include "HwManager.h"
#include "Helpers.h"

#include <hardware/camera3.h>

#include <utils/Vector.h>
#include <utils/Mutex.h>
#include <utils/KeyedVector.h>
#include <utils/RefBase.h>
#include <utils/StrongPointer.h>

#include "ci/ci_api_structs.h" // for CI_BUFFID

namespace android {

// forward declarations
class FelixCamera;

typedef Vector<camera3_stream_buffer_t> BufferList_t;

/**
 * @brief Descriptor of shot (a set of buffers) to be programmed
 * in the single HW pipeline
 */
typedef CI_BUFFID Shot_t;
/**
 * @brief Vector of all shots to be programmed for current request
 * @note keyed by pointer to pipeline instance
 */
typedef KeyedVector<wp<cameraHw_t>, Shot_t> ShotList_t;

/**
 * @class CaptureRequest
 * @brief Stores HAL specific data for capture requests
 */

class CaptureRequest : public RefBase {
public:
    CaptureRequest(
            FelixCamera &parent,
            sp<FelixMetadata>& meta);
    ~CaptureRequest();

    void cleanUp(void);

    /**
     * @brief preprocess android capture request,
     * initialize internal data structure
     * @param captureRequest reference to framework capture req descriptor
     * @return eror status
     */
    status_t preprocessRequest(
            camera3_capture_request &captureRequest);

    /**
     * @brief preprocess request metadata
     * @note must be called prior to programming of the pipeline and shot
     */
    status_t preprocessMetadata();

    /**
     * @brief program request in HW pipeline
     */
    status_t program();

    /**
     * @brief acquire shot from HW pipeline
     * @note due to asynchronous queue nature of CI shot handling,
     * you are never sure that acquired shot is the same as programmed one
     * until you check the buffer id's
     */
    status_t acquireShot(ISPC::Shot& acquiredShot);

    /**
     * @brief send capture result using internal data
     * @param sendMetadata is false then result field is NULL
     * @return error status
     */
    status_t sendCaptureResult(bool sendMetadata = true);

    /**
     * @brief send reprocess result using internal data
     *
     * @note sends mReprocessOutputBuffer and optionally mReprocessInputBuffer
     * (using mSendInputBufferWithResult flag)
     * @return error status
     */
    status_t sendReprocessResult(const bool success);

    /**
     * @brief Increase number of pipeline processing steps for current request
     */
    void newPipelineStep() { ++mPipelineDepth; }

    /**
     * @brief return depth of processing pipeline this request went through
     */
    uint8_t getPipelineSteps() { return mPipelineDepth; }

    /**
     * @brief return true if the request is reprocessing
     */
    bool isReprocessing() const { return mReprocessing; }

    /**
     * @brief frame number for current request
     */
    uint32_t                      mFrameNumber;

    /**
     * @brief list of framework buffers associated with current request
     */
    BufferList_t                  mBuffers;

    /**
     * @brief list of hardware shots to camera context mappings for current
     * capture request
     */
    ShotList_t                    mShotList;

    /**
     * @brief metadata associated with current capture request
     */
    sp<FelixMetadata>                 mFrameSettings;

    // JPEG related fields
    /**
     * @brief if true then input buffer has been provided in request
     */
    bool                          mReprocessing;

    /**
     * @brief true if jpeg compression requested
     */
    bool                          mJpegRequested;

    /**
     * @brief capture request output buffer for jpeg compressed image
     */
    camera3_stream_buffer_t       mReprocessOutputBuffer;

    /**
     * @brief capture request input buffer descriptor
     */
    camera3_stream_buffer_t       mInputBuffer;

    /**
     * @brief allocated when PRESCALE_IMAGE_FOR_JPEG_COMPRESSOR is defined
     */
    sp<StreamBuffer>         mPrescaledInputBuffer;

    /**
     * @brief request error signaling
     */
    bool                          mRequestError;

    /**
     * @brief signaling of metadata handling error
     */
    bool                          mMetadataError;

    /**
     * @brief counts request processing steps this CaptureRequest goes through
     */
    uint8_t                       mPipelineDepth;

private:
    /**
     * @brief prevents assignments, so we can use only pointers to requests
     */
    CaptureRequest& operator=(const CaptureRequest&){ return *this; }

    /**
     * @brief insert streamBuffer into list of shot descriptors
     * @param streamBuffer single output buffer from capture request
     * @return error if insertion not possible due to:
     * - specific output from specific pipeline is already requested
     * - invalid state of streamBuffer
     * @note mLock shall be locked prior to calling this method
     */
    status_t insertStreamBuffer(
            const camera3_stream_buffer_t &streamBuffer);

    Mutex mLock; ///<@brief access serialization lock
    FelixCamera &mParent; ///<@brief parent HAL class
    HwManager   &mCameraHw;      ///<@brief abstraction of ISP hardware
};

} // namespace android

#endif // CAPTURE_REQUEST_H
