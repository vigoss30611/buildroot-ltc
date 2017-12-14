/**
******************************************************************************
@file FelixCameraHAL.cpp

@brief Definition of v2500 Camera HAL factory module

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

#define LOG_TAG "FelixCameraHAL"

#include <cutils/properties.h>
#include <utils/RefBase.h>
#include <utils/StrongPointer.h>

#include "FelixCameraHAL.h"
#include "FelixCamera.h"
#include "felixcommon/userlog.h"
#include "sensorapi/sensorapi.h"

/*
 * Required HAL header.
 */
camera_module_t HAL_MODULE_INFO_SYM = {
        /*common: */{
            /*tag:*/                HARDWARE_MODULE_TAG,
            /*module_api_version:*/ CAMERA_MODULE_API_VERSION_2_1, //FIXME
            /*hal_api_version:*/    HARDWARE_HAL_API_VERSION,
            /*id:*/                 CAMERA_HARDWARE_MODULE_ID,
            /*name:*/               "Felix/Raptor Camera Module",
            /*author:*/             "Imagination Technologies Ltd",
//HW module common methods. In this case we have the device_open method
            /*methods:*/            &android::FelixCameraHAL::mCameraModuleMethods,
            /*dso:*/                NULL,
            /*reserved:*/           {0}
        },
//function to query about the number of camera devices that the module has
//support
        /*get_number_of_cameras:*/  android::FelixCameraHAL::get_number_of_cameras,
        //return metadata information about a particular camera
        /*get_camera_info:*/        android::FelixCameraHAL::get_camera_info,
        //mainly for setting the callback to notify about changes
        // in the status of a camera (like disconnection).
        // Not supported at the moment
        /*set_callbacks:*/          android::FelixCameraHAL::set_callbacks,
        /*get_vendor_tag_ops:*/     NULL,
#if CAMERA_MODULE_API_VERSION_CURRENT >= HARDWARE_DEVICE_API_VERSION(2, 3)
        /*open_legacy:*/            NULL,
#endif
        /*reserved:*/               {NULL,}
};

#if defined CAM1_SENSOR && not defined CAM1_PARAMS
#error "CAM1_PARAMS not defined!"
#endif

#if defined CAM0_SENSOR and not defined CAM0_PARAMS
#error "CAM0_PARAMS not defined!"
#endif

#ifndef FELIX_HAS_DG
#ifdef CAM1_DG
#error "Please define FELIX_DATA_GENERATOR in FelixDefines.mk!"
#endif

#ifdef CAM0_DG
#error "Please define FELIX_DATA_GENERATOR in FelixDefines.mk!"
#endif

#endif /* FELIX_HAS_DG */

/* These are defined in FelixDefines.mk */
#ifdef CAM1_DG
#undef CAM1_SENSOR
#define CAM1_SENSOR CAM1_DG_FILENAME
#define CAM1_DG_USED       true
#else
#define CAM1_DG_USED       false
#ifndef CAM1_SENSOR_FLIP
#define CAM1_SENSOR_FLIP (SENSOR_FLIPPING::SENSOR_FLIP_NONE)
#endif /* CAM1_SENSOR_FLIP */
#endif /* CAM1_SENSOR */

#ifdef CAM0_DG
#undef CAM0_SENSOR
#define CAM0_SENSOR CAM0_DG_FILENAME
#define CAM0_DG_USED       true
#else
#define CAM0_DG_USED       false
#ifndef CAM0_SENSOR_FLIP
#define CAM0_SENSOR_FLIP (SENSOR_FLIPPING::SENSOR_FLIP_NONE)
#endif /* CAM0_SENSOR_FLIP */
#endif /* CAM0_SENSOR */

namespace android {

ANDROID_SINGLETON_STATIC_INSTANCE(FelixCameraHAL);

void FelixCameraHAL::instantiateCamera(int& index,
            const std::string& sensorName,
            const std::string& defaultConfig,
            int sensorMode,
            int flip,
            bool backCamera,
            bool dataGenerator) {
    sp<FelixCamera> camera;
    status_t res;

// WARNING - do not use direct macros like CAMERA_DEVICE_API_VERSION_3_2
// because they are not defined in older releases. C preprocessor would
// treat undefined macro as zero and resolve this expression to false

    LOG_INFO("CAM%d : sensor=\"%s\" config=\"%s\"", index,
        sensorName.c_str(), defaultConfig.c_str());

#if CAMERA_DEVICE_API_VERSION_CURRENT < HARDWARE_DEVICE_API_VERSION(3, 2)
    camera = new FelixCamera(index, backCamera, &HAL_MODULE_INFO_SYM.common);
#else
    camera = new FelixCamera3v2(index, backCamera, &HAL_MODULE_INFO_SYM.common);
#endif

    if(camera.get()) {
        res = camera->Initialize(sensorName,
            defaultConfig,
            sensorMode,
            flip,
            dataGenerator);

        if (res != NO_ERROR) {
            LOG_ERROR("Unable to initialize Felix back camera %d: %s (%d)",
                    index, strerror(-res), res);
            camera.clear();
        } else {
            mCamera.add(index, camera);
            ++index;
        }
    } else {
        LOG_ERROR("Unable to instantiate Felix camera class");
    }
}

FelixCameraHAL::FelixCameraHAL():
        mConstructedOK(false),
        mCallbacks(NULL)
{

#if CAMERA_DEVICE_API_VERSION_CURRENT < HARDWARE_DEVICE_API_VERSION(3, 2)
#define HAL_VERSION "v3.0"
#else
#define HAL_VERSION "v3.2"
#endif
    LOG_INFO("IMG Camera HAL " HAL_VERSION " (#%s)", HAL_CHANGELIST);

    int num = 0;
#ifdef CAM0_SENSOR
    const std::string CAM0_SENSOR_NAME(CAM0_SENSOR);
    const std::string CAM0_PARAMS_FILE(CAM0_PARAMS);
    if(CAM0_SENSOR_NAME.length()) {
        instantiateCamera(num,
                CAM0_SENSOR_NAME,
                CAM0_PARAMS_FILE,
                0, /* sensorMode */
                CAM0_SENSOR_FLIP,
                true, /* backCamera */
                CAM0_DG_USED);
    } else {
        LOG_ERROR("CAM0: Empty sensor name");
    }
#else
#warning "Not building CAM0 camera"
#endif /* CAM0_SENSOR */

#ifdef CAM1_SENSOR
    const std::string CAM1_SENSOR_NAME(CAM1_SENSOR);
    const std::string CAM1_PARAMS_FILE(CAM1_PARAMS);
    if(CAM1_SENSOR_NAME.length()) {
        instantiateCamera(num,
                CAM1_SENSOR_NAME,
                CAM1_PARAMS_FILE,
                0, /* sensorMode */
                CAM1_SENSOR_FLIP,
                false, /* backCamera */
                CAM1_DG_USED);
    } else {
        LOG_ERROR("CAM1: Empty sensor name");
    }
#else
#warning "Not building CAM1 camera"
#endif /* CAM1_SENSOR */

    if(mCamera.size()>0) {
        mConstructedOK = true;
    }
}

FelixCameraHAL::~FelixCameraHAL()
{
    mCamera.clear();
}

/****************************************************************************
 * Camera HAL API handlers.
 *
 * Each handler simply verifies existence of an appropriate EmulatedBaseCamera
 * instance, and dispatches the call to that instance.
 *
 ***************************************************************************/

//receives a camera_id (which has to be available in the module) and fills in
// the hw device struct with the device information
int FelixCameraHAL::cameraDeviceOpen(int camera_id, hw_device_t** device)
{
    LOG_DEBUG("id = %d", camera_id);

    if (!isConstructedOK()) {
        LOG_ERROR("FelixCameraHAL has failed to initialize");
        return -EINVAL;
    }

    if(camera_id < 0 || camera_id>=(int)mCamera.size()) {
        LOG_ERROR("Camera id %d is out of bounds (%d)",
            camera_id, getCameraNum());
        return -ENODEV;
    }
    if(!device || mCamera.valueFor(camera_id).get()==NULL) {
        return -ENODEV;
    }
    *device = NULL;

    return mCamera.valueFor(camera_id)->connectCamera(device);
}

int FelixCameraHAL::getCameraInfo(int camera_id, struct camera_info* info)
{
    LOG_DEBUG("id = %d", camera_id);

    if (!isConstructedOK()) {
        LOG_ERROR("FelixCameraHAL has failed to initialize");
        return -EINVAL;
    }

    if(camera_id < 0 || camera_id>=(int)mCamera.size()) {
        LOG_ERROR("Camera id %d is out of bounds (%d)",
            camera_id, getCameraNum());
        return -ENODEV;
    }
    if(NULL == info || mCamera.valueFor(camera_id).get()==NULL) {
        return -ENODEV;
    }
    return mCamera.valueFor(camera_id)->getCameraInfo(info);
}

int FelixCameraHAL::setCallbacks( const camera_module_callbacks_t *callbacks)
{
    LOG_DEBUG("callbacks = %p", callbacks);
    mCallbacks = callbacks;

    return OK;
}

/****************************************************************************
 * Camera HAL API callbacks.
 ***************************************************************************/

int FelixCameraHAL::device_open(const hw_module_t* module, const char* name,
        hw_device_t** device)
{
    /*
     * Simply verify the parameters, and dispatch the call inside
     * the FelixCameraHAL instance.
     */

    //Check we are opening a devide in the expected module
    if (module != &HAL_MODULE_INFO_SYM.common) {
        LOG_ERROR("Invalid module %p expected %p", module,
            &HAL_MODULE_INFO_SYM.common);
        return -EINVAL;
    }

    //We need a camera name (which is a string with the camera id number).
    if (name == NULL || device == NULL) {
        LOG_ERROR("Invalid parameters");
        return -EINVAL;
    }

    //Call the cameraDeviceOpen of the global FelixCameraHal instance
    return FelixCameraHAL::getInstance().cameraDeviceOpen(atoi(name), device);
}

int FelixCameraHAL::get_number_of_cameras(void)
{
    //call the global FelixCameraHall instance corresponding function
    return FelixCameraHAL::getInstance().getCameraNum();
}

int FelixCameraHAL::get_camera_info(int camera_id, struct camera_info* info)
{
    //call the global FelixCameraHall instance corresponding function
    return FelixCameraHAL::getInstance().getCameraInfo(camera_id, info);
}

int FelixCameraHAL::set_callbacks(const camera_module_callbacks_t *callbacks)
{
    //call the global FelixCameraHall instance corresponding function
    return FelixCameraHAL::getInstance().setCallbacks(callbacks);
}

/******************************************************************************
 * Internal API
 ******************************************************************************/

/******************************************************************************
 * Initializer for the static member structure.
 ******************************************************************************/

/* Entry point for camera HAL API. */
struct hw_module_methods_t FelixCameraHAL::mCameraModuleMethods = {
        open: FelixCameraHAL::device_open
};

}; /* namespace android */
