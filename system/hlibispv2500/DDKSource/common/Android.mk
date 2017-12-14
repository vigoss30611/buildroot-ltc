LOCAL_PATH := $(call my-dir)
# Directory with FELIX generated files. Change as needed.
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
        img_includes/img_errors.c \
	linkedlist/code/linkedlist.c \
	felixcommon/source/pixel_transform.c \
	felixcommon/source/lshgrid.c \
	felixcommon/source/dpfmap.c \
	felixcommon/source/userlog.c \
	felixcommon/source/ci_alloc_info.c \
	sim_image/sim_image.cpp \
	sim_image/image.cpp \
	sim_image/savefile.c

# IMG libraries includes
LOCAL_C_INCLUDES := \
		$(LOCAL_PATH)/img_includes \
		$(LOCAL_PATH)/img_includes/c99 \
		$(LOCAL_PATH)/linkedlist/include \
		$(LOCAL_PATH)/felixcommon/include \
		$(LOCAL_PATH)/sim_image \
		external/stlport/stlport \
		bionic

LOCAL_SHARED_LIBRARIES := \
		libstlport liblog

# Export header files paths to another modules
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_C_INCLUDES)

LOCAL_MODULE := libfelix_common
LOCAL_MODULE_TAGS := eng

LOCAL_CFLAGS += $(FELIX_LOCAL_CFLAGS)

include $(BUILD_SHARED_LIBRARY)
