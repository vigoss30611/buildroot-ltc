LOCAL_PATH := $(call my-dir)
# Directory with FELIX generated files. Change as needed.
include $(CLEAR_VARS)
include $(FELIX_TOP)/FelixDefines.mk

# Felix CI files
LOCAL_SRC_FILES := \
		felix/felix_lib/user/source/ci_api.c \
        felix/felix_lib/user/source/ci_gasket.c \
        felix/felix_lib/user/source/ci_intdg.c \
		felix/felix_lib/user/source/ci_modulesdefaults.c \
        felix/felix_lib/user/source/ci_pipeline.c \
		felix/felix_lib/user/source/mc_config.c \
		felix/felix_lib/user/source/mc_convert.c \
        felix/felix_lib/user/source/mc_extract.c \
		felix/felix_lib/user/source/mc_pipeline.c \
		felix/felix_lib/user/source/sys_userio.c

ifeq ($(FELIX_DATA_GENERATOR), true)
LOCAL_SRC_FILES += \
		felix/felix_lib/data_generator/user/dg_api.c \
		felix/felix_lib/data_generator/user/dg_converter.c
endif

# Felix register includes
LOCAL_C_INCLUDES := \
		$(LOCAL_PATH)/felix/regdefs/include \
		$(LOCAL_PATH)/felix/regdefs/vhc_out

# Felix CI includes
LOCAL_C_INCLUDES += \
		$(LOCAL_PATH)/felix/felix_lib/user/include \
		$(LOCAL_PATH)/felix/felix_lib/kernel/include \
		$(LOCAL_PATH)/felix/felix_lib/data_generator/include

LOCAL_CFLAGS := -DANDROID

ifeq ($(FELIX_DATA_GENERATOR), true)
LOCAL_CFLAGS += -DFELIX_HAS_DG
endif

ifeq ($(ANDROID_EMULATOR), true)
LOCAL_CFLAGS += -DANDROID_EMULATOR
endif

LOCAL_SHARED_LIBRARIES := libfelix_common liblog

# Export Felix CI includes
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_C_INCLUDES)

LOCAL_MODULE := libfelix_ci
LOCAL_MODULE_TAGS := eng

LOCAL_CFLAGS += $(FELIX_LOCAL_CFLAGS)

include $(BUILD_SHARED_LIBRARY)
