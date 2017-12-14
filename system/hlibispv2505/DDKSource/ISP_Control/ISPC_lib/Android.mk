LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include $(FELIX_TOP)/FelixDefines.mk

# Felix ISPC source files
ISP_FILE_LIST := $(wildcard $(LOCAL_PATH)/src/*.cpp)
AUXILIAR_FILE_LIST := $(wildcard $(LOCAL_PATH)/src/Auxiliar/*.cpp)
MODULES_FILE_LIST := $(wildcard $(LOCAL_PATH)/src/Modules/*.cpp)
CONTROL_FILE_LIST := $(wildcard $(LOCAL_PATH)/src/Controls/*.cpp)	 

LOCAL_SRC_FILES := $(ISP_FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(AUXILIAR_FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(MODULES_FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(CONTROL_FILE_LIST:$(LOCAL_PATH)/%=%)

# Export Felix ISPC header files
EXPORT_FELIX_ISPC_INCLUDE_DIRS := \
        $(LOCAL_PATH)/include \
		$(LOCAL_PATH)/include/Modules \
		$(LOCAL_PATH)/include/Auxiliar \
		$(LOCAL_PATH)/include/Controls

EXPORT_FELIX_ISPC_INCLUDE_DIRS += \
       $(EXPORT_FELIX_CI_INCLUDE_DIRS) \
       $(EXPORT_FELIX_SENSORAPI_INCLUDE_DIRS) \
       $(EXPORT_FELIX_COMMON_INCLUDE_DIRS) \
       external/stlport/stlport \
       bionic

LOCAL_CFLAGS := -Wno-reorder

ifeq ($(FELIX_DATA_GENERATOR), true)
LOCAL_CFLAGS += -DISPC_EXT_DATA_GENERATOR
endif
# could have an option for ISPC_HAS_PERFLOG but may not work on android

LOCAL_SHARED_LIBRARIES := \
    libfelix_common \
    libfelix_ci \
    libfelix_sensor \
    liblog \
    libstlport

LOCAL_STATIC_LIBRARIES := \
    libfelix_sim_image

LOCAL_C_INCLUDES += $(EXPORT_FELIX_ISPC_INCLUDE_DIRS)

LOCAL_MODULE := libfelix_ispc
LOCAL_MODULE_TAGS := eng

LOCAL_CFLAGS += $(FELIX_LOCAL_CFLAGS)

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/ispc2test/Android.mk

