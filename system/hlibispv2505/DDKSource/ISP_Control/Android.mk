LOCAL_ISPC_PATH := $(call my-dir)

include $(LOCAL_ISPC_PATH)/ISPC_lib/Android.mk
include $(LOCAL_ISPC_PATH)/ISPC_tcp3/Android.mk
include $(LOCAL_ISPC_PATH)/test_apps/Android.mk

