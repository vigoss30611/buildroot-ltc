LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libhealthd.$(TARGET_BOARD_PLATFORM)
LOCAL_SRC_FILES := healthd_board.cpp 
LOCAL_C_INCLUDES += \
	system/core/healthd
include $(BUILD_STATIC_LIBRARY)
