include ../Makefile.param
RANLIB = true
ARFLAGS = rcv

#ROOT_DIR := $(shell pwd)
ROOT_DIR := .

SRC_ROOT            :=   $(ROOT_DIR)/src
LIBS_ROOT           :=   $(ROOT_DIR)/lib
OBJ_ROOT            :=   $(ROOT_DIR)/obj

DEP_ROOT            :=   $(OBJ_ROOT)

OUT_LIB_DIR :=   $(LIBS_PATH)/$(CHIP_ARCH)/libRtspServer


CFLAGS      +=   -O2 -ffunction-sections  -fdata-sections -DWITH_NOIDREF  -fPIC
OUT_LIB     :=   $(OUT_LIB_DIR)/libRtspServer.a

OUT_LIB_SO		:=	 $(OUT_LIB_DIR)/libRtspServer.so

#****源代码路径********************
SRC_DIR             :=  $(SRC_ROOT)
#***不需要包含的源代码路径*********************
#SRC_EXP_DIR         :=  $(INIPARSER_PATH)      \
#                        $(VSCP_PATH)

SRCS                :=  $(shell find $(SRC_DIR) -name '*.c')
SRCS                :=  $(sort $(SRCS))

#过滤掉不希望包含的文件路径
ifneq ($(strip $(SRC_EXP_DIR)),)
    SRCS_EXP        :=  $(shell find $(SRC_EXP_DIR) -name '*.c')
    SRCS            :=  $(filter-out $(SRCS_EXP),$(SRCS))
endif

OBJS                :=  $(SRCS:$(ROOT_DIR)%.c=$(OBJ_ROOT)%.o)
OBJ_DIR             :=  $(sort $(dir $(OBJS)))

#*** 
SRC_DEP_DIR         :=  $(OBJ_DIR)
#SRC_DEPS            :=  $(OBJS:%.o=%.d)

INCLUDE_DIR         :=  $(SRC_ROOT)
ifneq ($(strip $(INCLUDE_EXP_DIR)),)
    INCLUDE_DIR     :=  $(filter-out $(INCLUDE_EXP_DIR),$(INCLUDE_DIR))
endif

INCLUDES            :=  $(shell echo "find $(INCLUDE_DIR) -name '*.h'";find $(INCLUDE_DIR) -name '*.h')
INCLUDE_DIR         :=  $(sort $(dir $(INCLUDES)))
INCLUDE_SPATH       :=  $(foreach dir,$(INCLUDE_DIR), -I$(dir))

#*********
LIB_INCLUDE_PATH     := $(INCLUDE_SPATH)
LIB_INCLUDE_PATH     += -I./inc
LIB_INCLUDE_PATH     += -I$(SRC_ROOT)/ext_defs
LIB_INCLUDE_PATH     += -I$(LIBINC_PATH)/libqmapi
LIB_INCLUDE_PATH     += -I$(LIBINC_PATH)/
LIB_INCLUDE_PATH     += -I$(LIBINC_PATH)/librecord/inc
LIB_INCLUDE_PATH     += -I$(LIBINC_PATH)/libmtran/inc
LIB_INCLUDE_PATH     += -I$(IPC_PATH)/thttpd-2.25b/inc
LIB_INCLUDE_PATH     += -I$(LIBINC_PATH)/libhdmng/inc


#CFLAGS += -Wall -O2 -Wunused-variable
#***********************************************************#
#********************* prepare compiler ********************#
#***********************************************************#


#定义连接时用到的库
COMPILE         =  $(CC) $(CFLAGS) -c "$<" -o "$@" $(LIB_INCLUDE_PATH)

COMPILEDEP      =  $(CCDEP) -MM "$<"  $(CFLAGS)  $(LIB_INCLUDE_PATH)

CreateDir        =   $(shell [ -d $1 ] || mkdir -p $1 || echo ":mkdir '$1' fail")
RemoveDir        =   $(shell [ -d $1 ] && rm -rf $1 && echo -e "rmdir '$1'\t [ OK ]" || echo ":rm dir '$1' fail")
#create obj subdirectories
CreateResult        :=
dummy               :=  $(foreach dir,$(OBJ_DIR),CreateResult += $(call CreateDir,$(dir)))
ifneq ($(strip CreateResult),)
    err = $(error $(CreateResult))
endif
#create obj/lib directory
CreateResult        :=
dummy               :=  $(foreach dir,$(OUT_LIB_DIR),CreateResult += $(call CreateDir,$(dir)))
ifneq ($(strip CreateResult),)
    err = $(error $(CreateResult))
endif




#.PHONY: clean cleanall

all : $(SRC_DEPS) $(OBJS) $(OUT_LIB) $(OUT_LIB_SO) success

success: $(OUT_LIB) $(OUT_LIB_SO)
	@echo " *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo " *SUCCESSFUL COMPILE $(OUT_LIB)"
	@echo " *SUCCESSFUL COMPILE $(OUT_LIB_SO)"
	@echo " *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

	
$(OBJS) : $(OBJ_ROOT)/%.o : $(ROOT_DIR)/%.c
	@echo complie $@
	$(COMPILE)

$(SRC_DEPS) : $(DEP_ROOT)/%.d : $(ROOT_DIR)/%.c
	@set -e;$(COMPILEDEP) > $@.$$$$; \
	sed 's,.*\.o[ :]*,$(@:%.d=%.o) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(SRC_DEPS)


$(OUT_LIB)  :   $(SRC_DEPS) $(OBJS) 
	@echo "*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo "* CREATE $(OUT_LIB_DIR)"
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))			
	$(AR) $(ARFLAGS) $@ $(OBJS) 
	$(RANLIB) $@
	rm -Rf $(LIBS_ROOT)/inc 
	@echo "*******************************************************"

$(OUT_LIB_SO)	:	$(SRC_DEPS)	$(OBJS)
	@echo "*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo "* CREATE $(OUT_LIB_DIR)"
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))			
	$(CC) $(CFLAGS) -fPIC -shared -fvisibility=hidden -Wl,--gc-sections -o $@ $(OBJS)	
	$(STRIP) $(OUT_LIB_SO)
	@echo "*******************************************************"

	
.PHONY: cleanall clean 

cleanall: clean
	@echo "DEL depends files"
	@rm -f $(SRC_DEPS)
	@echo "--------------------------------------------"    	
clean:
	@echo "DEL OBJ files $(SRC_DEPS)"
	@rm -f $(OBJS)
	@rm -f $(SRC_DEPS)
	@echo "DEL LIBS file $(OUT_LIB)"
	@rm -f $(OUT_LIB)
	@rm -f $(OUT_LIB_SO)
	@echo "--------------------------------------------"    	
