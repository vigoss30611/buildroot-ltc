/**
******************************************************************************
@file FelixAAA.h

@brief Common interface for AAA v2500 HAL modules

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

#ifndef AAA_HAL_IF_H
#define AAA_HAL_IF_H

namespace android {

class FelixMetadata;
class CameraMetadata;

/*
 * @brief Pure virtual HAL interface for 3A modules
 *
 * @note Shall be derived by all Android HAL 3A control modules
 */
class AAAHalInterface {
public:
    virtual ~AAAHalInterface(){}
    /**
     * @brief Post construction initialization of internal fields
     * @note Requires respective module to be registered prior to exec
     */
    virtual status_t initialize(void) = 0;

    /**
     * @brief Processes input events from request metadata for the purpose of
     * internal state machine
     *
     * @note Shall be executed in ProcessingThread directly before acquireShot()
     */
    virtual status_t processDeferredHALMetadata(FelixMetadata &settings) = 0;

    /**
     * @brief Processes Android metadata and initializes the HW registers prior
     * to programming of new shot
     */
    virtual status_t processUrgentHALMetadata(FelixMetadata &settings) = 0;

    /**
     * @brief updates output metadata with current state of focus
     */
    virtual status_t updateHALMetadata(FelixMetadata &settings) = 0;

    /**
     * @brief initializes static android metadata
     */
    static void advertiseCapabilities(CameraMetadata &info);

    /**
     * @brief initializes request metadata with all supported regions
     */
    virtual status_t initRequestMetadata(FelixMetadata &settings,
        int type) = 0;
};

} //namespace android

#endif /*AAA_HAL_IF_H*/
