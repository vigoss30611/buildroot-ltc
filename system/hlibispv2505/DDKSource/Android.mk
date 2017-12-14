LOCAL_DDK_PATH:= $(call my-dir)

include $(LOCAL_DDK_PATH)/common/Android.mk
include $(LOCAL_DDK_PATH)/CI/Android.mk
include $(LOCAL_DDK_PATH)/sensorapi/Android.mk
include $(LOCAL_DDK_PATH)/ISP_Control/Android.mk

