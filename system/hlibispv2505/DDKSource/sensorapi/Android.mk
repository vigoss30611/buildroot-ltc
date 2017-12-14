LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
include $(LOCAL_PATH)/../../FelixDefines.mk

# Sensor files
LOCAL_SRC_FILES := \
		sensors/iifdatagen.c \
		sensors/ar330.c \
		sensors/p401.c \
		sensors/ov4688.c

LOCAL_CFLAGS += -DSENSOR_AR330 -DSENSOR_P401 -DSENSOR_OV4688

ifeq ($(FELIX_DATA_GENERATOR), true)
LOCAL_SRC_FILES += sensors/dgsensor.c
LOCAL_CFLAGS += -DSENSOR_DG
endif

ifeq ($(SENSOR_I2C_POLLING), ON)
LOCAL_CFLAGS += -DIMG_SCB_POLLING_MODE
endif

# sensor API files
LOCAL_SRC_FILES += \
    source/sensorapi.c \
    source/sensor_phy.c \
    source/pciuser.c

EXPORT_FELIX_SENSORAPI_INCLUDE_DIRS := $(LOCAL_PATH)/include

LOCAL_C_INCLUDES := $(EXPORT_FELIX_SENSORAPI_INCLUDE_DIRS) \
                       $(EXPORT_FELIX_CI_INCLUDE_DIRS) \
                       $(EXPORT_FELIX_COMMON_INCLUDE_DIRS)

LOCAL_SHARED_LIBRARIES := \
    libfelix_ci \
    libfelix_common \
    liblog
    
LOCAL_STATIC_LIBRARIES := \
    libfelix_sim_image

LOCAL_MODULE := libfelix_sensor
LOCAL_MODULE_TAGS := eng

LOCAL_CFLAGS += $(FELIX_LOCAL_CFLAGS)

include $(BUILD_SHARED_LIBRARY)
