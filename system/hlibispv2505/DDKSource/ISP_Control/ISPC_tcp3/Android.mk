LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE:= ISPC_tcp
LOCAL_MODULE_TAGS := eng

LOCAL_SHARED_LIBRARIES := \
    libfelix_ci \
    libfelix_ispc \
    libfelix_sensor \
    libfelix_common \
    liblog \
    libcutils \
    libutils \
    libstlport

LOCAL_STATIC_LIBRARIES := \
    libfelix_paramsocket2 \
    libfelix_dyncmd \
    libfelix_ispc2test

LOCAL_C_INCLUDES := \
    $(EXPORT_FELIX_ISPC_INCLUDE_DIRS) \
    $(EXPORT_FELIX_DYNCMD_INCLUDE_DIRS) \
    $(EXPORT_FELIX_PARAMSOCKET2_INCLUDE_DIRS) \
    $(EXPORT_FELIX_ISPC2TEST_INCLUDE_DIRS) \
    $(LOCAL_PATH)

LOCAL_SRC_FILES:= \
    main.cpp \
    ISPC_tcp_mk3.cpp

include $(BUILD_EXECUTABLE)
