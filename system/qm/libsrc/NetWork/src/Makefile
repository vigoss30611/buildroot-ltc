# Makefile for 19vision video ipc project
# Copyright(C) 2007-2020 19vision Inc.
# Author yi.zhang

include ../../Makefile.param
CFLAGS := $(CFLAGS_BASE)


### Object path configure
OBJSPATH 				:= ./debug
OBJS 					:= $(wildcard $(OBJSPATH)/*.o) 

INCLUDE =  -I../inc
INCLUDE += -I$(LIBSRC_PATH)/common/inc
INCLUDE += -I$(LIBSRC_PATH)/SystemMsg/
INCLUDE += -I$(LIBSRC_PATH)/AlarmServer/inc
INCLUDE += -I$(LIBSRC_PATH)/AVServer/inc

CFLAGS += $(INCLUDE)

PROJECT_ROOT = .

PROJECT_SRC := $(wildcard $(PROJECT_ROOT)/*.c)
PROJECT_SRC += $(wildcard $(PROJECT_ROOT)/*.cpp)



OBJECTS_DIR:=$(PROJECT_ROOT)/debug
OBJECT_FILE:=$(addprefix $(OBJECTS_DIR)/, $(addsuffix .o, $(basename $(notdir $(PROJECT_SRC)))))
OBJECT_FILE:= $(filter-out $(OBJECTS_DIR)/tlflash.o,$(OBJECT_FILE))
DEPFILE:=$(PROJECT_ROOT)/deps

OUT_LIB_DIR := $(LIBS_PATH)/$(CHIP_ARCH)/libNetWork

EXE_NAME = $(OBJECTS_DIR)/TL_HI3511_DVS.out
LIB_NAME = libNetWork.a

all:$(DEPFILE) $(EXE_NAME)

$(DEPFILE): $(PROJECT_SRC) Makefile
	@mkdir -p debug
	@-rm -f $(DEPFILE)
	@echo "Generating new dependency file...";
	@for f in $(PROJECT_SRC); do \
	OBJ=$(OBJECTS_DIR)/`basename $$f|sed -e 's/\.cpp/\.o/' -e 's/\.c/\.o/'`; \
          echo $$OBJ: $$f>> $(DEPFILE); \
          echo '	$(CC) $$(CFLAGS) -c -o $$@ $$^'>> $(DEPFILE); \
        done
 
 
-include $(DEPFILE)

####################################################################
$(EXE_NAME): $(OBJECT_FILE)
	@echo "* CREATE $(OUT_LIB_DIR)"
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))			
	@echo "Make " $(LIB_NAME)
#	$(CC) $(TLENV) $(CFLAGS) -o $@ $^ -lpthread -lrt -lcrypt -lstdc++ $(EXT_DYNAMIC_LIBS) -lm $(EXT_LIBS) $(LIBS)
	$(AR) -rcv $(LIB_NAME) $?
	cp $(LIB_NAME) $(OUT_LIB_DIR)/


clean:
	-$(RM) -rf debug $(DEPFILE)
	-$(RM) -rf $(LIB_NAME)

