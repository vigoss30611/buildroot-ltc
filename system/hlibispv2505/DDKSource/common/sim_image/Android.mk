LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libfelix_sim_image
LOCAL_MODULE_TAGS := eng

EXPORT_FELIX_SIM_IMAGE_INCLUDE_DIRS := \
    $(LOCAL_PATH)/ \
    $(EXPORT_FELIX_COMMON_INCLUDE_DIRS)

LOCAL_C_INCLUDES := \
    $(EXPORT_FELIX_SIM_IMAGE_INCLUDE_DIRS)

LOCAL_SRC_FILES := \
    savefile.c \
    sim_image.cpp \
    image.cpp

LOCAL_CFLAGS += $(FELIX_LOCAL_CFLAGS)

include $(BUILD_STATIC_LIBRARY)
