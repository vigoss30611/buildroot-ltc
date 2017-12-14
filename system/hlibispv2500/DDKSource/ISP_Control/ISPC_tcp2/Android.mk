LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#$(info $(FELIX_TOP))
#$(info $(LOCAL_PATH))

ifeq ($(FELIX_TOP),)
$(error "FELIX_TOP not defined. Build this module from higher dir level.")
endif

COMMON_PATH := ../../common
DDKSRC_PATH := $(FELIX_TOP)/DDKSource

# where cmake files stay
OBJ_DIR_NAME := obj

CI_VER_FILE := $(strip $(wildcard $(DDKSRC_PATH)/$(OBJ_DIR_NAME)/CI/felix/felix_lib/user/include/ci/ci_version.h))
ifeq ($(CI_VER_FILE),)
$(error "ci_version.h is missing in $(DDKSRC_PATH)/$(OBJ_DIR_NAME)")
endif


LOCAL_SRC_FILES:= \
      main.cpp \
	  ISPC2_tcp.cpp \
	  $(COMMON_PATH)/dyncmd/code/commandline.c \
	  $(COMMON_PATH)/paramsocket2/src/paramsocket.cpp \
	  $(COMMON_PATH)/paramsocket2/src/paramsocket_client.cpp \
	  $(COMMON_PATH)/paramsocket2/src/paramsocket_server.cpp \


LOCAL_SHARED_LIBRARIES := libfelix_ci libfelix_common liblog libcutils libutils libfelix_ispc libstlport libfelix_sensor

LOCAL_C_INCLUDES += \
    $(DDKSRC_PATH)/common/dyncmd/include \
    $(DDKSRC_PATH)/common/paramsocket2/include \
    $(DDKSRC_PATH)/ISP_Control/ISPC_lib/include \
	$(DDKSRC_PATH)/sensorapi/include \
	$(DDKSRC_PATH)/CI/felix/felix_lib/user/include \
	$(DDKSRC_PATH)/ISP_Control/ISPC_lib/include/Controls \
	$(DDKSRC_PATH)/ISP_Control/ISPC_lib/include/Auxiliar \
	$(DDKSRC_PATH)/ISP_Control/ISPC_lib/include/Modules \
	$(DDKSRC_PATH)/obj/CI/felix/felix_lib/user/include \
	$(LOCAL_PATH) \


LOCAL_MODULE:= ispc_tcp

LOCAL_MODULE_TAGS := eng

include $(BUILD_EXECUTABLE)
