include ../Makefile.param
CFLAGS := $(CFLAGS_BASE)

TARGET = libftp.a

TEST_OBJS := 
OBJS := $(filter-out $(TEST_OBJS), $(patsubst %.c,%.o,$(wildcard src/*.c)))

OUT_LIB_DIR := $(LIBS_PATH)/$(CHIP_ARCH)/libftp

INCLUDE =  -I./inc
INCLUDE += -I$(SDK_INC_PATH)
INCLUDE += -I$(LIBSRC_PATH)/common/inc

CFLAGS += -Wall -O2
all: $(TARGET)

CFLAGS+= -fPIC -DHI_OS_LINUX -DHI_OS_TYPE=HI_OS_LINUX -DCONFIG_NETWORK_USE_SYSTEM -DHI_LINUX_SUPPORT_UCLIBC -DHI_OS_SUPPORT_UCLINUX -DHI_MEMORY_DEBUG

$(TARGET): $(OBJS)
	$(AR) -rcv $@ $(OBJS)
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))
	cp $(TARGET) $(OUT_LIB_DIR)/
	
%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) *.gdb *.elf *.o

