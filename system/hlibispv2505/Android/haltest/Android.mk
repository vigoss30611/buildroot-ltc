LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := haltest.cpp

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libutils \
	libbinder \
	libhardware \
	libdl \
	libstagefright \
    libcamera_metadata \
	libstagefright_omx \
	libstagefright_foundation \
	libcameraservice

LOCAL_C_INCLUDES:= \
	$(TOP)/system/media/camera/include \
	$(TOP)/frameworks/av/services/camera/libcameraservice \
	$(TOP)/frameworks/native/include/media/openmax

#LOCAL_CFLAGS +=

LOCAL_MODULE:= haltest
LOCAL_MODULE_TAGS := eng

include $(BUILD_EXECUTABLE)
