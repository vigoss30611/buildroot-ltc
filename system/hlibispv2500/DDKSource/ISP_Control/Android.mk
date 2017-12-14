LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
include $(FELIX_TOP)/FelixDefines.mk

# Felix ISPC source files
ISP_FILE_LIST := $(wildcard $(LOCAL_PATH)/ISPC_lib/src/*.cpp)
AUXILIAR_FILE_LIST := $(wildcard $(LOCAL_PATH)/ISPC_lib/src/Auxiliar/*.cpp)
MODULES_FILE_LIST := $(wildcard $(LOCAL_PATH)/ISPC_lib/src/Modules/*.cpp)
CONTROL_FILE_LIST := $(wildcard $(LOCAL_PATH)/ISPC_lib/src/Controls/*.cpp)	 

LOCAL_SRC_FILES := $(ISP_FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(AUXILIAR_FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(MODULES_FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(CONTROL_FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/ISPC_lib/include \
		$(FELIX_TOP)/DDKSource/sensorapi/include \
		$(FELIX_TOP)/DDKSource/common/sim_image \
		$(LOCAL_PATH)/ISPC_lib/include/Modules \
		$(LOCAL_PATH)/ISPC_lib/include/Auxiliar \
		$(LOCAL_PATH)/ISPC_lib/include/Controls

LOCAL_CFLAGS := -Wno-reorder

ifeq ($(FELIX_DATA_GENERATOR), true)
LOCAL_CFLAGS += -DISPC_EXT_DATA_GENERATOR
endif

LOCAL_SHARED_LIBRARIES := libfelix_ci \
			  libfelix_common \
			  liblog \
			  libfelix_sensor \
			  libstlport

# Export Felix ISPC header files
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_C_INCLUDES)

LOCAL_MODULE := libfelix_ispc
LOCAL_MODULE_TAGS := eng

LOCAL_CFLAGS += $(FELIX_LOCAL_CFLAGS)

include $(BUILD_SHARED_LIBRARY)


# ISPC_loop application
include $(CLEAR_VARS)

LOCAL_MODULE := ISPC_loop
LOCAL_MODULE_TAGS := eng

LOCAL_DYNCMD_PATH := ../common/dyncmd

LOCAL_SHARED_LIBRARIES := libfelix_ispc \
			  libfelix_ci \
			  libfelix_common \
			  liblog \
			  libfelix_sensor \
			  libstlport
			  
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(LOCAL_DYNCMD_PATH)/include \
					$(LOCAL_PATH)/ISPC_lib/ispc2test/include

LOCAL_SRC_FILES := test_apps/src/ISPC_loop.cpp \
				$(LOCAL_DYNCMD_PATH)/code/commandline.c \
				ISPC_lib/ispc2test/info_helper.cpp
				

include $(BUILD_EXECUTABLE)

