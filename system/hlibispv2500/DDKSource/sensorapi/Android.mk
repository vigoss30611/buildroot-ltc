LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
include $(FELIX_TOP)/FelixDefines.mk

# Sensor files
LOCAL_SRC_FILES := \
		sensors/iifdatagen.c \
		sensors/ar330.c \
		sensors/p401.c \
		sensors/ov4688.c \
		sensors/imx179.c
LOCAL_CFLAGS += -DSENSOR_AR330 -DSENSOR_P401 -DSENSOR_OV4688 -DSENSOR_IMX179

ifeq ($(FELIX_DATA_GENERATOR), true)
LOCAL_SRC_FILES += sensors/dgsensor.c
LOCAL_CFLAGS += -DSENSOR_DG
endif

# sensor API files
LOCAL_SRC_FILES += \
	source/sensorapi.c \
        source/sensor_phy.c \
	source/pciuser.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := libfelix_ci libfelix_common liblog

LOCAL_MODULE := libfelix_sensor
LOCAL_MODULE_TAGS := eng

LOCAL_CFLAGS += $(FELIX_LOCAL_CFLAGS)

include $(BUILD_SHARED_LIBRARY)
