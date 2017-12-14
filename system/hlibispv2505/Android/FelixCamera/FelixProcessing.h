/**
******************************************************************************
@file FelixProcessing.h

@brief Interface of FelixProcessing request processing thread class
in v2500 Camera HAL

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

#ifndef FELIX_PROCESSING_H
#define FELIX_PROCESSING_H

#include "JpegEncoder/JpegEncoder.h"
#include "FelixMetadata.h"
#include "CaptureRequest.h"

#include <hardware/camera3.h>

#include <utils/Thread.h>
#include <utils/List.h>
#include <utils/Mutex.h>
#include <utils/StrongPointer.h>

#include "ci/ci_api_structs.h" // for CI_BUFFID

namespace android {

// forward declarations
class FelixCamera;

/**
 * @brief Internal capture requests/results processing thread.
 */
class ProcessingThread : public Thread,
                         private JpegEncoder::JpegListener {
public:
    ProcessingThread(FelixCamera &parent);
    ~ProcessingThread();
    /**
     * @brief Place request in the queue to wait for capture result acquiring.
     */
    void queueCaptureRequest(CaptureRequest &internalRequest);

    void signalCaptureResultSent(void);

    /**
     * @brief Test if the processing thread is idle (queue is empty)
     */
    bool isIdle();

    bool isQueueEmpty();

    bool isFlushing() {
        Mutex::Autolock l(mLock);
        return mFlushing; }

    void setFlushing(bool flush) {
        Mutex::Autolock l(mLock);
        mFlushing = flush; }

    nsecs_t getLastFrameTimestamp(void) {
        Mutex::Autolock l(mLock);
        return mPrevFrameTimestamp;
    }

protected:
    void popCaptureRequest(void); ///<@brief remove oldest request from queue
    status_t processISandFD(CaptureRequest &captureRequest);
    status_t processJpegEncodingRequest(CaptureRequest &captureRequest);
    status_t processRegularRequest(CaptureRequest &captureRequest,
            nsecs_t &timestampNs);
    status_t processTimestamp(const uint32_t hwTimestamp,
            nsecs_t& durationNs, nsecs_t& frameTimestampNs);

    void tryIdle();

    bool flushQueue(void);

    virtual bool threadLoop();
    // JPEG compressor callbacks
    virtual void onJpegDone(const StreamBuffer &jpegBuffer,
            const int32_t jpegSize, bool success);

private:
    static const nsecs_t    kWaitPerLoop = 100000000L; // 100 ms
    nsecs_t                 mPrevFrameTimestamp;
    FelixCamera             &mParent;
    HwManager               &mCameraHw;  ///<@brief abstraction of ISP hardware
    Mutex                   mLock;
    bool                   mThreadActive;
    bool                    mFlushing;

    List<sp<CaptureRequest> >    mCaptureRequestsQueue;
    Condition               mCaptureRequestQueuedSignal;
    Condition               mCaptureResultSentSignal;

    // ----------------------------------------------------------------
    // Reprocessing related fields
    // ----------------------------------------------------------------
    Mutex                   mReprocessLock;
    bool                    mReprocessWaiting;
    sp<CaptureRequest>      mReprocessingRequest;
};
// ------------------------------------------------------------------------

} // namespace android

#endif // FELIX_PROCESSING_H
