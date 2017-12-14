# ispc2test

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libfelix_ispc2test
LOCAL_MODULE_TAGS := eng

LOCAL_SHARED_LIBRARIES := libstlport

EXPORT_FELIX_ISPC2TEST_INCLUDE_DIRS := $(LOCAL_PATH)/include

LOCAL_C_INCLUDES :=  $(EXPORT_FELIX_ISPC2TEST_INCLUDE_DIRS) \
                       $(EXPORT_FELIX_ISPC_INCLUDE_DIRS) \
                       $(EXPORT_FELIX_CI_INCLUDE_DIRS)

LOCAL_SRC_FILES := \
    info_helper.cpp \
    perf_controls.cpp

LOCAL_CFLAGS += $(FELIX_LOCAL_CFLAGS)

include $(BUILD_STATIC_LIBRARY)