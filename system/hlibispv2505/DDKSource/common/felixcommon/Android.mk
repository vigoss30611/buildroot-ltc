
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libfelix_common
LOCAL_MODULE_TAGS := eng

EXPORT_FELIX_COMMON_INCLUDE_DIRS := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/../img_includes \
    $(LOCAL_PATH)/../img_includes/c99

LOCAL_C_INCLUDES := \
    $(EXPORT_FELIX_COMMON_INCLUDE_DIRS)

LOCAL_SRC_FILES := \
    include $(call all-c-files-under, source) \
    ../img_includes/img_errors.c

LOCAL_SHARED_LIBRARIES := liblog

LOCAL_CFLAGS += $(FELIX_LOCAL_CFLAGS)

include $(BUILD_SHARED_LIBRARY)