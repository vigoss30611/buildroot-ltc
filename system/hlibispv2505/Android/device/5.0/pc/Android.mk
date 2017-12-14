ifeq ($(TARGET_DEVICE),pc)
include $(all-subdir-makefiles)

# The Libyuv makefile only supports ARM for some reason, so redefine most of
# it here

include $(CLEAR_VARS)

LOCAL_PATH := external/libyuv

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := \
	files/source/compare.cc \
	files/source/convert.cc \
	files/source/convert_argb.cc \
	files/source/convert_from.cc \
	files/source/cpu_id.cc \
	files/source/format_conversion.cc \
	files/source/planar_functions.cc \
	files/source/rotate.cc \
	files/source/rotate_argb.cc \
	files/source/row_common.cc \
	files/source/row_posix.cc \
	files/source/scale.cc \
	files/source/scale_argb.cc \
	files/source/video_common.cc

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/files/include

LOCAL_MODULE := libyuv
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libadf_dynamic
LOCAL_MODULE_TAGS := optional
LOCAL_WHOLE_STATIC_LIBRARIES := libadf

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libadfhwc_dynamic
LOCAL_MODULE_TAGS := optional
LOCAL_WHOLE_STATIC_LIBRARIES := libadfhwc
LOCAL_SHARED_LIBRARIES := libadf_dynamic liblog libutils

include $(BUILD_SHARED_LIBRARY)

endif
