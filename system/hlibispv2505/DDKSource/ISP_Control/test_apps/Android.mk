# ISPC_loop application

LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := ISPC_loop
LOCAL_MODULE_TAGS := eng

LOCAL_SHARED_LIBRARIES := \
    libfelix_ispc \
    libfelix_ci \
    libfelix_sensor \
    libfelix_common \
    liblog \
    libstlport

LOCAL_STATIC_LIBRARIES := \
    libfelix_ispc2test \
    libfelix_dyncmd

LOCAL_C_INCLUDES := \
    $(EXPORT_FELIX_ISPC_INCLUDE_DIRS) \
    $(EXPORT_FELIX_DYNCMD_INCLUDE_DIRS) \
    $(EXPORT_FELIX_ISPC2TEST_INCLUDE_DIRS)

LOCAL_SRC_FILES := src/ISPC_loop.cpp

include $(BUILD_EXECUTABLE)