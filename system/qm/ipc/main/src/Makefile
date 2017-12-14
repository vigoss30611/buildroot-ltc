######

INCLUDE :=
LDFLAGS := -Wall -O2
EXT_LIBS :=
TARGET = qm_app

ifeq ($(CHIP_ARCH),Q3)
include ../../Makefile.param
LDFLAGS	+= $(CFLAGS_BASE)
endif

INCLUDE += -I$(LIBINC_PATH)/
INCLUDE += -I$(IPC_PATH)/ProtocolService/src/

##导入外部服务库##
ifeq ($(CFLAGS_PPCS_P2P),y)
LDFLAGS += -DQ3_PPCS_P2P
INCLUDE += -I$(IPC_PATH)/PpcsP2p/inc
EXT_LIBS += $(LIBS_PATH)/$(CHIP_ARCH)/ppcs/ppcs.a
EXT_LIBS += $(IPC_PATH)/PpcsP2p/libPPCS_API.a
endif

ifeq ($(CFLAGS_QVT_P2P),y)
LDFLAGS += -DQ3_QVT_P2P
INCLUDE += -I$(IPC_PATH)/QuickVisionP2P/inc
EXT_LIBS += $(LIBS_PATH)/$(CHIP_ARCH)/QuickVisionP2P/QuickVisionP2P.a
EXT_LIBS += $(LIBS_PATH)/$(CHIP_ARCH)/QuickVisionP2P/libQVT.so
endif


ifeq ($(CFLAGS_RTSP_SERVER),y)
LDFLAGS += -DQ3_RTSP_SERVER
INCLUDE += -I$(IPC_PATH)/RtspServer/inc
EXT_LIBS += $(LIBS_PATH)/$(CHIP_ARCH)/libRtspServer/libRtspServer.a
endif

ifeq ($(CFLAGS_NET_SERVER),y)
LDFLAGS += -DQ3_NET_SERVER
INCLUDE += -I$(IPC_PATH)/netserver/inc
EXT_LIBS += $(LIBS_PATH)/$(CHIP_ARCH)/netserver/netserver.a
endif

ifeq ($(CFLAGS_PROTO_ONVIF),y)
LDFLAGS += -DQ3_PROTO_ONVIF
INCLUDE += -I$(IPC_PATH)/onvif/include
endif

ifeq ($(CFLAGS_PROTO_HIK),y)
LDFLAGS += -DQ3_PROTO_HIK
INCLUDE += -I$(IPC_PATH)/libhik/inc
endif

ifeq ($(CFLAGS_PROTO_XMAI),y)
LDFLAGS += -DQ3_PROTO_XMAI
INCLUDE += -I$(IPC_PATH)/libxmai/inc
endif

ifeq ($(CFLAGS_PROTO_ZHINUO),y)
LDFLAGS += -DQ3_PROTO_ZHINUO
INCLUDE += -I$(IPC_PATH)/libzhinuo/inc
endif

ifeq ($(CFLAGS_PROTO_JUAN),y)
LDFLAGS += -DQ3_PROTO_JUAN
INCLUDE += -I$(IPC_PATH)/libjuan/inc
EXT_LIBS += $(LIBS_PATH)/$(CHIP_ARCH)/libjuan/libjuan.a
endif

INCLUDE += -I$(IPC_PATH)/libnetworkservice/inc
EXT_LIBS += $(LIBS_PATH)/$(CHIP_ARCH)/libRtspServer/libRtspServer.a

INCLIBS := -lpthread -lrt -lcrypt -lcrypto -lssl -lstdc++ -ldl -lm  -lvideobox -lqvplay -lh264bitstream -liconv

EXT_LIBS += $(LIBS_PATH)/$(CHIP_ARCH)/libQM.so
EXT_LIBS += $(LIBS_PATH)/$(CHIP_ARCH)/ProtocolService/libProtocolService.so

ifeq ($(CFLAGS_THTTPD_SERVER),y)
LDFLAGS += -DQ3_THTTPD_SERVER
INCLUDE += -I$(IPC_PATH)/thttpd-2.25b/inc
EXT_LIBS += $(LIBS_PATH)/$(CHIP_ARCH)/libthttpd/libthttpd.so
endif


LDFLAGS += $(INCLUDE)

TEST_OBJS := 
OBJS := $(filter-out $(TEST_OBJS), $(patsubst %.c,%.o,$(wildcard *.c)))

OUT_LIB_DIR := $(LIBS_PATH)/$(CHIP_ARCH)/main

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "* CREATE $(OUT_LIB_DIR)"
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))			
	@echo "Make " $(TARGET)
	$(CC) -o $(TARGET) $(OBJS) $(INCLIBS) $(EXT_LIBS)
	$(STRIP) $(TARGET)
	cp $(TARGET) $(OUT_LIB_DIR)/
	@echo "=================BUILD $(CHIP_ARCH) SUCESS=========================="

%.o: %.c
	$(CC) $(LDFLAGS) -c -o $@ $<

clean:
	-$(RM) -rf *.o $(TARGET)
