LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libfelix_paramsocket2
LOCAL_MODULE_TAGS := eng

EXPORT_FELIX_PARAMSOCKET2_INCLUDE_DIRS := \
    $(LOCAL_PATH)/include \
    external/stlport/stlport \
    bionic \
    $(EXPORT_FELIX_COMMON_INCLUDE_DIRS)

LOCAL_C_INCLUDES := \
    $(EXPORT_FELIX_PARAMSOCKET2_INCLUDE_DIRS)

LOCAL_SRC_FILES := \
    src/paramsocket.cpp \
    src/paramsocket_client.cpp \
    src/paramsocket_server.cpp

LOCAL_CFLAGS += $(FELIX_LOCAL_CFLAGS)

include $(BUILD_STATIC_LIBRARY)