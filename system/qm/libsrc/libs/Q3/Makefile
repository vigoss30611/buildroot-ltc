include ../../Makefile.param
CFLAGS := $(CFLAGS_BASE)

LIST  = >> cc.lst 2>&1

TARGET = libQM.so

OUT_LIB_DIR := $(LIBS_PATH)/$(CHIP_ARCH)/

INCLUDE =  -I../inc
INCLUDE += -I$(SDK_INC_PATH)

INCLUDE += -I$(LIBINC_PATH)/libqmapi
INCLUDE += -I$(LIBINC_PATH)/libAVServer/inc
INCLUDE += -I$(LIBINC_PATH)/libAudio/inc
INCLUDE += -I$(LIBINC_PATH)/libAlarmServer/inc
INCLUDE += -I$(LIBINC_PATH)/libSysTime/inc
INCLUDE += -I$(LIBINC_PATH)/libNetWork/inc
INCLUDE += -I$(LIBINC_PATH)/librecord/inc
INCLUDE += -I$(LIBINC_PATH)/libSYSCfg/inc
INCLUDE += -I$(LIBINC_PATH)/libhdmng/inc
INCLUDE += -I$(LIBINC_PATH)/libsystem_msg/inc
INCLUDE += -I$(LIBINC_PATH)/libBasic/inc
INCLUDE += -I$(LIBINC_PATH)/libVideoMarker/inc
INCLUDE += -I$(LIBINC_PATH)/libupgrade/inc
INCLUDE += -I$(LIBINC_PATH)/libCamera/inc
INCLUDE += -I$(LIBINC_PATH)/libftp/inc

INCLUDE += -I$(LIBSRC_PATH)/SRDK/inc
INCLUDE += -I$(LIBSRC_PATH)/NetWork/inc

INCLIBS := -lpthread -lrt -lcrypt -lstdc++ -ldl -lm
#INCLIBS += -lmediabuffer -L$(LIBS_PATH)/$(CHIP_ARCH)/libmediabuffer
INCLIBS += -lvideobox -laudiobox -lcodecs -lupgrade -lsys -ldemux -levent -litems -lfr -lwifi -L$(SDK_LIB_PATH)
INCLIBS += -lQMHAL

OBJECTS :=
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/SRDK/SRDK.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/SysTime/SysTime.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/libNetWork/libNetWork.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/libftp/libftp.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/libAlarmServer/libAlarmServer.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/libAVServer/libAVServer.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/SYSCfg/SYSCfg.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/VideoMarker/VideoMarker.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/Audio/Audio.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/Camera/Camera.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/libRecord/libRecord.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/libupgrade/libupgrade.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/libhdmng/libhdmng.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/libsystem_msg/libsystem_msg.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/Basic/Basic.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/libmediabuffer/libmediabuffer.a
OBJECTS += $(LIBS_PATH)/$(CHIP_ARCH)/libLog/Log.a

all: $(TARGET) $(TARGET_A)

$(TARGET): $(OBJECTS)
	rm -f $(TARGET) 
	$(CC) -shared -fPIC $(INCLIBS) -Wl,--whole-archive $(OBJECTS) -Wl,--no-whole-archive -Wl,-soname -Wl,$@ -o $@
	@echo "FINISHED CREATING $(TARGET) TARGET" $(LIST)
	@echo "-----------------------------------------------------" $(LIST)
	cp $(TARGET) $(OUT_LIB_DIR)

clean:
	rm -f $(TARGET) *.gdb *.elf *.o

