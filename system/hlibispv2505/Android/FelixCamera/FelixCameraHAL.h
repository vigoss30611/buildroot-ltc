/**
******************************************************************************
@file FelixCameraHAL.h

@brief Main interface of v2500 Camera HAL factory module

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

#ifndef FELIX_CAMERA_HAL_H
#define FELIX_CAMERA_HAL_H

#include <utils/Singleton.h>
#include <utils/RefBase.h>
#include <utils/KeyedVector.h>
#include "FelixCamera.h"

namespace android {

/*
 * Contains declaration of a class FelixCameraHAL that manages available
 * Felix Cameras. A global instance of this class is statically
 * instantiated and initialized when camera emulation HAL is loaded.
 */

/* Class FelixCameraHAL cameras available for the emulation.
 *
 * When the global static instance of this class is created on the module load,
 * it enumerates cameras available for the emulation by connecting to the
 * emulator's 'camera' service. For every camera found out there it creates an
 * instance of an appropriate class, and stores it an in array.
 *
 * Instance of this class is also used as the entry point for the camera
 * HAL API, including:
 *  - hw_module_methods_t::open entry point
 *  - camera_module_t::get_number_of_cameras entry point
 *  - camera_module_t::get_camera_info entry point
 *
 */
class FelixCameraHAL : Singleton<FelixCameraHAL>{
public:
    FelixCameraHAL();

    ~FelixCameraHAL();

    /************************************************************************
     * Camera HAL API handlers.
     ***********************************************************************/
    /**
     * @brief Opens (connects to) a camera device.
     * @note This method is called in response to
     * hw_module_methods_t::open callback.
     */
    int cameraDeviceOpen(int camera_id, hw_device_t** device);

    /**
     * @brief Gets camera information.
     * @note This method is called in response to
     * camera_module_t::get_camera_info callback.
     */
    int getCameraInfo(int camera_id, struct camera_info *info);

    /**
     * @brief Sets camera callbacks.
     * @note This method is called in response to
     * camera_module_t::set_callbacks callback.
     */
    int setCallbacks(const camera_module_callbacks_t *callbacks);

    /*************************************************************************
     * Camera HAL API callbacks.
     ************************************************************************/

    /** @brief camera_module_t::get_number_of_cameras callback entry point. */
    static int get_number_of_cameras(void);

    /** @brief camera_module_t::get_camera_info callback entry point. */
    static int get_camera_info(int camera_id, struct camera_info *info);

    /** @brief camera_module_t::set_callbacks callback entry point. */
    static int set_callbacks(const camera_module_callbacks_t *callbacks);

private:
    /** @brief hw_module_methods_t::open callback entry point. */
    static int device_open(const hw_module_t* module,
            const char* name,
            hw_device_t** device);

    /*************************************************************************
     * Public API.
     ************************************************************************/

public:

    /** @brief Gets camera orientation. */
    int getCameraOrientation() {
        /* : Have a boot property that controls that. */
        return 90;
    }

    /** @brief Gets number of cameras.
     */
    int getCameraNum() const {
        return mCamera.size();
    }

    /** @brief Checks whether or not the constructor has succeeded.
     */
    bool isConstructedOK() const {
        return mConstructedOK;
    }

    void onStatusChanged(int cameraId, int newStatus);

    /*************************************************************************
     * Private API
     ************************************************************************/
private:

    void instantiateCamera(int& index,
            const std::string& sensorName,
            const std::string& defaultConfig,
            int sensorMode,
            int flip,
            bool backCamera,
            bool dataGenerator = false);

    /** @brief Available Felix cameras */
    KeyedVector<int, sp<FelixCamera> > mCamera;

    /** @brief Flags whether or not constructor has succeeded. */
    bool                mConstructedOK;

    /** @brief Camera callbacks (for status changing) */
    const camera_module_callbacks_t* mCallbacks;

public:
    /** @brief Contains device open entry point, as required by HAL API. */
    static struct hw_module_methods_t   mCameraModuleMethods;
};

}; /* namespace android */

extern android::FelixCameraHAL   gFelixCameraHAL;

#endif  /* FELIX_CAMERA_HAL_H */
