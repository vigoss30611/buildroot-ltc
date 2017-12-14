# Copyright (C) 2011 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(FELIX_TOP)/FelixDefines.mk

IMG_ANDROID_MAJOR_VERSION := $(word 1, $(subst ., , $(PLATFORM_VERSION)))
IMG_ANDROID_MINOR_VERSION := $(word 2, $(subst ., , $(PLATFORM_VERSION)))

# read changelist number from changelist.txt
# sed does not support lazy matching, what a shame
#IMG_HAL_CHANGELIST := $(shell grep Changelist $(LOCAL_PATH)/../../DDKSource/changelist.txt | sed -n "s/.* \([0-9]*\)\{1\} $.*/\1/p")
IMG_HAL_CHANGELIST := "\"$(shell awk ' /Changelist/ { print $$3 }' $(LOCAL_PATH)/../../DDKSource/changelist.txt)\""
LOCAL_CFLAGS += -DHAL_CHANGELIST=$(IMG_HAL_CHANGELIST)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

LOCAL_CFLAGS += \
    -fno-short-enums \
    -Wunused \
    -D__maybe_unused=__attribute__\(\(unused\)\)
LOCAL_CFLAGS += $(FELIX_LOCAL_CFLAGS)

LOCAL_CPPFLAGS += -std=c++11

ifeq ($(FELIX_DATA_GENERATOR), true)
# To use hardware sensor data generator defines should be disabled
LOCAL_CFLAGS += -DFELIX_HAS_DG
endif

ifneq ($(CAM0_SENSOR),NONE)
ifeq ($(CAM0_SENSOR),"DATA_GENERATOR")
LOCAL_CFLAGS += -DCAM0_DG
ifneq ($(CAM0_DG_FILENAME),)
LOCAL_CFLAGS += -DCAM0_DG_FILENAME=\"$(CAM0_DG_FILENAME)\"
endif
endif
ifneq ($(CAM0_SENSOR_FLIP),)
LOCAL_CFLAGS += -DCAM0_SENSOR_FLIP=$(CAM0_SENSOR_FLIP)
endif
LOCAL_CFLAGS += -DCAM0_SENSOR=\"$(CAM0_SENSOR)\" 
LOCAL_CFLAGS += -DCAM0_PARAMS=\"$(CAM0_PARAMS)\"
endif

ifneq ($(CAM1_SENSOR),NONE)
ifeq ($(CAM1_SENSOR),"DATA_GENERATOR")
LOCAL_CFLAGS += -DCAM1_DG
ifneq ($(CAM1_DG_FILENAME),)
LOCAL_CFLAGS += -DCAM1_DG_FILENAME=\"$(CAM1_DG_FILENAME)\"
endif
endif
ifneq ($(CAM1_SENSOR_FLIP),)
LOCAL_CFLAGS += -DCAM1_SENSOR_FLIP=$(CAM1_SENSOR_FLIP)
endif
LOCAL_CFLAGS += -DCAM1_SENSOR=\"$(CAM1_SENSOR)\"
LOCAL_CFLAGS += -DCAM1_PARAMS=\"$(CAM1_PARAMS)\"
endif

LOCAL_SHARED_LIBRARIES := \
    libcamera_metadata \
    liblog \
    libutils \
    libcutils \
    libcamera_client \
    libui \
    libstlport

# JPEG conversion libraries and includes.
LOCAL_SHARED_LIBRARIES += \
    libjpeg \
    libexif

LOCAL_C_INCLUDES += \
    bionic/libm/include \
    external/jpeg \
    external/libexif \
    frameworks/native/include/media/hardware \
    $(FELIX_TOP)/Android/gralloc \
    $(EXPORT_FELIX_ISPC_INCLUDE_DIRS) \
    $(FELIX_TOP)/Android/FelixCamera/AAA \
    $(FELIX_TOP)/Android/FelixCamera/JpegEncoder \
    $(FELIX_TOP)/Android/FelixCamera/Scaler \
    $(call include-path-for, camera)

LOCAL_SHARED_LIBRARIES += \
    libfelix_ispc \
    libfelix_sensor \
    libfelix_ci \
    libfelix_common

LOCAL_SRC_FILES := \
    FelixCameraHAL.cpp \
    FelixCamera.cpp \
    FelixMetadata.cpp \
    FelixProcessing.cpp \
    CaptureRequest.cpp \
    HwManager.cpp \
    HwCaps.cpp \
    Helpers.cpp \
    JpegEncoder/JpegEncoder.cpp \
    JpegEncoder/JpegEncoderSw.cpp \
    JpegEncoder/JpegExif.cpp \
    AAA/FelixAF.cpp \
    AAA/FelixAWB.cpp \
    AAA/FelixAE.cpp \
    Scaler/scale.c \
    Scaler/sccoef.c \
    Scaler/ScalerSw.cpp

# Android < 5.0 has struct ion_handle defined
# Since Lollipop, ion_user_handle_t is defined 
ifeq ($(shell test ${IMG_ANDROID_MAJOR_VERSION} -ge 5 && echo 1),1) 
    LOCAL_SRC_FILES += FelixCamera3v2.cpp
endif


ifeq ($(GPU_LIB), true)
$(info "USE_GPU_LIB defined")

#hr_TODO: defines needed by gpu includes 
# gl_tools.h
LOCAL_CFLAGS += -DFEATURE_USE_STABILISATION
LOCAL_CFLAGS += -DFEATURE_USE_FACEDETECT

LOCAL_C_INCLUDES += \
    $(GPU_TOP)/libPVRVision/include \
    $(GPU_TOP)/include

LOCAL_CFLAGS += -DUSE_GPU_LIB

LOCAL_SHARED_LIBRARIES += libcameragpu
else
$(info "USE_GPU_LIB NOT defined")
endif

LOCAL_MODULE := felix.camera.default

LOCAL_MODULE_TAGS := eng

include $(BUILD_SHARED_LIBRARY)

