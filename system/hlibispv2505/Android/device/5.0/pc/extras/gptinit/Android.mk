LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := gptinit
LOCAL_SRC_FILES := gptinit.c
LOCAL_C_INCLUDES := external/zlib system/extras/ext4_utils
LOCAL_MODULE_TAGS := optional
LOCAL_STATIC_LIBRARIES := \
 libext4_utils_static libsparse_static libselinux libz libc
LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)

include $(BUILD_EXECUTABLE)
