ROOT_DIR         = ../..
OBJ_BASE_DIR     = $(ROOT_DIR)/$(OBJ_DIR)
MODULE_DIR       = $(OBJ_BASE_DIR)/$(MODULE_NAME)
SUB_DIR          = system_fun

MODULE_NAME      = $(shell pwd | awk -F '/' '{print $$NF}')
LIB_MODULE_NAME  = lib$(MODULE_NAME).a
TEST_MODULE_NAME = test_$(MODULE_NAME)

ifeq ($(INFOTM_SMALL_UI_ENABLE), yes)
	CFLAGS += -DGUI_SMALL_SYSTEM_ENABLE
endif

ifeq ($(INFOTM_GUI_STYLE), large)
	CFLAGS += -DGUI_LARGE_SCREEN
endif

ifeq ($(INFOTM_GUI_SHOW_STATUS_BAR), yes)
	CFLAGS += -DGUI_SHOW_STATUS_BAR
endif

ifeq ($(INFOTM_LCD_ORIENTATION), portrait)
	CFLAGS += -DGUI_ROTATE_SCREEN
endif

ifeq ($(INFOTM_RESOLUTION_SWITCH_MODULE), isp)
        CFLAGS += -DGUI_SET_RESOLUTION_SEPARATELY
endif

ifeq ($(INFOTM_CAMERA_REAR_ENABLE), yes)
	CFLAGS += -DGUI_CAMERA_REAR_ENABLE
endif

ifeq ($(INFOTM_CAMERA_PIP_ENABLE), yes)
	CFLAGS += -DGUI_CAMERA_PIP_ENABLE
endif

ifeq ($(INFOTM_WIFI_ENABLE), yes)
	CFLAGS += -DGUI_WIFI_ENABLE
endif

ifeq ($(INFOTM_WIFI_MODEL), esp8089)
        CFLAGS += -DGUI_WIFI_ESP8089
endif

ifeq ($(INFOTM_SOUND_OUTPUT), buzzer)
        CFLAGS += -DGUI_SOUND_BUZZER
endif

ifeq ($(INFOTM_WIFI_MODEL), rtl8189es)
        CFLAGS += -DGUI_WIFI_8189ES
endif

ifeq ($(INFOTM_GPS_ENABLE), yes)
	CFLAGS += -DGUI_GPS_ENABLE
endif

ifeq ($(INFOTM_ADAS_ENABLE), yes)
	CFLAGS += -DGUI_ADAS_ENABLE
endif

ifeq ($(INFOTM_GUI_TIMER_RESET), yes)
	CFLAGS += -DGUI_TIMER_RESET
endif


ifeq ($(INFOTM_GUI_AUTO_PLAY_ENABLE), yes)
	CFLAGS += -DGUI_AUTO_PLAY
endif

ifneq ($(INFOTM_VERSION_RELEASE), yes)
ifeq ($(INFOTM_GUI_DEBUG_ENABLE), yes)
	CFLAGS += -DGUI_DEBUG_ENABLE
endif
endif

ifeq ($(INFOTM_HDMI_ENABLE), yes)
	CFLAGS += -DGUI_HDMI_ENABLE
endif

ifneq ($(INFOTM_LCD_ENABLE), yes)
	CFLAGS += -DGUI_LCD_DISABLE
endif

include $(ROOT_DIR)/rule.mk
.PHONY: all
all: obj_o 


###########################################################################
#    Please modify lib here
###########################################################################
LIB_FLAGS  = \
			 -lasound \
             -lsharemem \
             -lpipeline \
             -losa \
             -llog \
             -lipc \
             -lfilemng  \
             -lpthread \
             -ldl  \
			 -lfreetype \
			 -ldemuxer -lavformat -lavcodec -lavutil -lm \
			 -litem \
			 -lgps


ifeq ($(TARGET_PLATFORM),arm)
    LIB_FLAGS  += -g -L./ -L$(ROOT_DIR)/$(SHARE_DIR) 
else
    LIB_FLAGS  +=  -g -lm -L. -lminigui_ths_ubuntu  -lpthread -ldl -lm
endif

             #-L../libgui/src/.libs/ -lminigui_ths \
###########################################################################
#    Make objs
###########################################################################
TARGET_DIR = $(MODULE_DIR)
TARGET_BIN = $(TARGET_DIR)/spv_launcher.bin
OBJFILES   = $(patsubst %.c, $(TARGET_DIR)/%.o, $(wildcard *.c))
SUBDIRFILES   = $(patsubst %.c, $(TARGET_DIR)/%.o, $(wildcard $(SUB_DIR)/*.c))
OBJFILES_CPP = $(patsubst %.cpp, $(TARGET_DIR)/%.o, $(wildcard *.cpp))
LOCAL_INC  = -I. -I../../lib_shared/demuxer/

LOCAL_INC += -Isystem_fun/ -I../../interface -I../../lib_static/filemng/include/ -I../../lib_static/item/

.PHONY: obj_o 
obj_o: MAKE_OBJ_DIR $(OBJFILES) $(SUBDIRFILES) $(OBJFILES_CPP) $(TARGET_BIN) 
	@echo -e "\033[32m ["$(MODULE_NAME)"] Make objects done \033[0m"

$(OBJFILES): $(TARGET_DIR)/%.o:%.c 
	$(COMPILE.c) $< -o $@  $(LOCAL_INC) $(CFLAGS)

$(SUBDIRFILES): $(TARGET_DIR)/%.o:%.c 
	$(COMPILE.c) $< -o $@   $(LOCAL_INC) $(CFLAGS)

$(OBJFILES_CPP): $(TARGET_DIR)/%.o:%.cpp 
	$(COMPILE.cpp) $< -o $@   $(LOCAL_INC) $(CFLAGS)

$(TARGET_BIN): $(OBJFILES) $(SUBDIRFILES) $(OBJFILES_CPP) 
	echo "obj files: $(OBJFILES) $(SUBDIRFILES)"
	echo "obj cpp files: $(OBJFILES_CPP)"
	$(LINK.cpp)  $@ $^ $(LIB_FLAGS)   

MAKE_OBJ_DIR:
	@if [ ! -d $(TARGET_DIR) ]; then mkdir -p $(TARGET_DIR); fi 
	@if [ ! -d $(TARGET_DIR)/$(SUB_DIR) ]; then mkdir -p $(TARGET_DIR)/$(SUB_DIR); fi 


###########################################################################
#    Make clean
###########################################################################
.PHONY: clean
clean:
	@rm -rf  $(TARGET_DIR)
	@rm -f   $(LIB_TARGET_DIR)/$(LIB_MODULE_NAME)
	@rm -rf  $(TEST_TARGET_DIR)
	@echo -e "\033[32m ["$(MODULE_NAME)"] Make clean done \033[0m"


