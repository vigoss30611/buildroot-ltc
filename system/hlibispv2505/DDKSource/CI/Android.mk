LOCAL_PATH := $(call my-dir)
# Directory with FELIX generated files. Change as needed.
include $(CLEAR_VARS)
include $(FELIX_TOP)/FelixDefines.mk

# Felix CI files
LOCAL_SRC_FILES := \
        felix/felix_lib/user/source/ci_api.c \
        felix/felix_lib/user/source/ci_converter.c \
        felix/felix_lib/user/source/ci_gasket.c \
        felix/felix_lib/user/source/ci_intdg.c \
        felix/felix_lib/user/source/ci_modulesdefaults.c \
        felix/felix_lib/user/source/ci_pipeline.c \
        felix/felix_lib/user/source/ci_load_hw2.c \
        felix/felix_lib/user/source/mc_config.c \
        felix/felix_lib/user/source/mc_convert.c \
        felix/felix_lib/user/source/mc_extract.c \
        felix/felix_lib/user/source/mc_pipeline.c \
        felix/felix_lib/user/source/sys_userio.c

# Felix register includes
EXPORT_FELIX_CI_INCLUDE_DIRS := \
        $(LOCAL_PATH)/felix/regdefs$(HW_ARCH)/include \
        $(LOCAL_PATH)/felix/regdefs$(HW_ARCH)/vhc_out
#EXPORT_FELIX_CI_INCLUDE_DIRS := \
#        $(LOCAL_PATH)/felix/regdefs2.4/include \
#        $(LOCAL_PATH)/felix/regdefs2.4/vhc_out

# Felix CI includes
EXPORT_FELIX_CI_INCLUDE_DIRS += \
        $(LOCAL_PATH)/felix/felix_lib/user/include \
        $(LOCAL_PATH)/felix/felix_lib/kernel/include \
        $(LOCAL_PATH)/felix/felix_lib/data_generator/include \
        $(LOCAL_PATH)/imglib/libraries/RegDefsUtils/include \
        $(EXPORT_FELIX_COMMON_INCLUDE_DIRS) \
        $(EXPORT_FELIX_LINKEDLIST_INCLUDE_DIRS) \
        $(EXPORT_FELIX_SIM_IMAGE_INCLUDE_DIRS)

LOCAL_CFLAGS := -DANDROID

ifeq ($(FELIX_DATA_GENERATOR), true)
LOCAL_SRC_FILES += \
        felix/felix_lib/data_generator/user/dg_api.c \
        felix/felix_lib/data_generator/user/dg_camera.c

LOCAL_CFLAGS += -DFELIX_HAS_DG
endif

ifeq ($(ANDROID_EMULATOR), true)
LOCAL_CFLAGS += -DANDROID_EMULATOR
endif

LOCAL_SHARED_LIBRARIES := libfelix_common liblog

LOCAL_STATIC_LIBRARIES := \
    libfelix_linkedlist \
    libfelix_sim_image

# Export Felix CI includes
LOCAL_C_INCLUDES := $(EXPORT_FELIX_CI_INCLUDE_DIRS)
                       

LOCAL_MODULE := libfelix_ci
LOCAL_MODULE_TAGS := eng

LOCAL_CFLAGS += $(FELIX_LOCAL_CFLAGS)

include $(BUILD_SHARED_LIBRARY)
