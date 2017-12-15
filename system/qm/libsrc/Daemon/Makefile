

include ../Makefile.param
	
TARGET = qm_daemon

OBJS := $(filter-out $(TEST_OBJS), $(patsubst %.c,%.o,$(wildcard src/*.c)))

OUT_LIB_DIR :=   $(LIBS_PATH)/$(CHIP_ARCH)/Daemon/

INCLUDE := -I./include

CFLAGS += -Wall -Werror -O2 $(INCLUDE)
LDFLAGS := -lpthread
LDFLAGS += -laudiobox -lupgrade -lsys -lcodecs -ldemux -lfr -leventhub -litems 

all: $(TARGET)
$(TARGET):src/Deamon_msg_s.c
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	$(STRIP) $(TARGET)
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))
	cp $(TARGET) $(OUT_LIB_DIR)
	
%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) *.gdb *.elf *.o $(OBJS)
