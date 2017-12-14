ROOT_DIR         = ../..
OBJ_BASE_DIR     = $(ROOT_DIR)/$(OBJ_DIR)
MODULE_DIR       = $(OBJ_BASE_DIR)/$(MODULE_NAME)

MODULE_NAME      = $(shell pwd | awk -F '/' '{print $$NF}')
LIB_MODULE_NAME  = lib$(MODULE_NAME).a
TEST_MODULE_NAME = test_$(MODULE_NAME)

include $(ROOT_DIR)/rule.mk
.PHONY: all
all: obj_o obj_lib 


###########################################################################
#    Please modify lib here
###########################################################################
LIB_FLAGS  = -losa \
             -llog \
             -lpthread \


LOCAL_INC = -I$(ROOT_DIR)/hardware/infomedia/include






###########################################################################
#    Make objs
###########################################################################
TARGET_DIR = $(MODULE_DIR)
OBJFILES   = $(patsubst %.c, $(TARGET_DIR)/%.o, $(wildcard *.c))

.PHONY: obj_o 
obj_o: MAKE_OBJ_DIR $(OBJFILES)
	@echo "\033[32m ["$(MODULE_NAME)"] Make objects done \033[0m"

$(OBJFILES): $(TARGET_DIR)/%.o:%.c 
	$(COMPILE.c) $< -o $@  $(COMMON_INC) $(LOCAL_INC)

MAKE_OBJ_DIR:
	@if [ ! -d $(TARGET_DIR) ]; then mkdir -p $(TARGET_DIR); fi


###########################################################################
#    Make archive 
###########################################################################
LIB_TARGET_DIR = $(ROOT_DIR)/$(STATIC_DIR)

.PHONY: obj_lib
obj_lib: MAKE_LIB_DIR $(LIB_MODULE_NAME)
	@echo "\033[32m ["$(MODULE_NAME)"] Create archive done \033[0m"

$(LIB_MODULE_NAME): $(OBJFILES)
	$(COMPILE.ar) $(LIB_TARGET_DIR)/$@ $^

MAKE_LIB_DIR:
	@if [ ! -d $(LIB_TARGET_DIR) ]; then mkdir -p $(LIB_TARGET_DIR); fi

###########################################################################
#    Make clean
###########################################################################
.PHONY: clean
clean:
	@rm -rf  $(TARGET_DIR)
	@rm -f   $(LIB_TARGET_DIR)/$(LIB_MODULE_NAME)
	@rm -rf  $(TEST_TARGET_DIR)
	@echo "\033[32m ["$(MODULE_NAME)"] Make clean done \033[0m"


