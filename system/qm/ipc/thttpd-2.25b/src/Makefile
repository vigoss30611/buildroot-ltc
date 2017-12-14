include ../../Makefile.param

ROOT_DIR := .
DEPFILE := dep
SRC_ROOT            :=   $(ROOT_DIR)/src
LIBSS_PATH           :=   $(ROOT_DIR)/lib
OBJ_ROOT            :=   $(ROOT_DIR)/obj


CCOPT =		-O2
DEFS =		 -DHAVE__PROGNAME=1 -DHAVE_FCNTL_H=1 -DHAVE_GRP_H=1 -DHAVE_MEMORY_H=1 -DHAVE_PATHS_H=1 -DHAVE_POLL_H=1 -DHAVE_SYS_POLL_H=1 
DEFS += -DTIME_WITH_SYS_TIME=1 -DHAVE_DIRENT_H=1 -DHAVE_LIBCRYPT=1 -DHAVE_STRERROR=1 -DHAVE_WAITPID=1 -DHAVE_VSNPRINTF=1 -DHAVE_DAEMON=1 
DEFS += -DHAVE_SETSID=1 -DHAVE_GETADDRINFO=1 -DHAVE_GETNAMEINFO=1 -DHAVE_GAI_STRERROR=1 
#DEFS += -DHAVE_SIGSET=1 
DEFS += -DHAVE_ATOLL=1 -DHAVE_UNISTD_H=1 
DEFS += -DHAVE_GETPAGESIZE=1 
DEFS += -DHAVE_SELECT=1 -DHAVE_POLL=1 -DHAVE_TM_GMTOFF=1 -DHAVE_INT64T=1 -DHAVE_SOCKLENT=1 

INCLS = -I../inc
INCLS += -I$(LIBINC_PATH)/

CFLAGS =	$(CCOPT) $(DEFS) $(INCLS)
CFLAGS += $(CFLAGS_BASE)
LDFLAGS =	
LIBS =		-lcrypt 
NETLIBS =	
INSTALL =	/usr/bin/install -c
LIB_PATH = ../lib/

CFLAGS += -Wall -O2 -fPIC

#CFLAGS += -Wall -fPIC -Os -ffunction-sections  -fdata-sections -DWITH_NOIDREF -fvisibility=hidden -Wl,-gc-sections 


TARGET	= libthttpd.a
TARGET_SO	= libthttpd.so

OUT_LIB_DIR :=   $(LIBS_PATH)/$(CHIP_ARCH)/libthttpd

.c.o:
	@rm -f $@
	$(CC) $(CFLAGS) -c $*.c

SRC =		thttpd.c libhttpd.c fdwatch.c mmc.c timers.c match.c tdate_parse.c cgiinterface.c

OBJ =		$(SRC:.c=.o) 

ALL =		thttpd

GENHDR =	mime_encodings.h mime_types.h

SUBDIRS =	cgi-src extras

all:		this 
this:	$(GENHDR) $(ALL)

thttpd: $(OBJ)
	@rm -f $@
	@echo "*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	@echo "* CREATE $(OUT_LIB_DIR)"
	$(if $(shell find $(OUT_LIB_DIR)), $(shell cd ./), $(shell mkdir $(OUT_LIB_DIR)))			
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $(OUT_LIB_DIR)/$@ $(OBJ) $(LIBS) $(NETLIBS)
	#$(CC) -w $(CFLAGS) $(LDFLAGS) -fPIC -shared -fvisibility=hidden -Wl,-gc-sections -o $(OUT_LIB_DIR)/$(TARGET_SO) $(OBJ)
	$(CC) -shared -o $(OUT_LIB_DIR)/$(TARGET_SO) $(OBJ)
	$(STRIP) $(OUT_LIB_DIR)/$(TARGET_SO)
	#$(AR) -rcv libthttpd.a $(OBJ)
	#cp -f libthttpd.a $(OUT_LIB_DIR)/

mime_encodings.h:	mime_encodings.txt
	rm -f mime_encodings.h
	sed < mime_encodings.txt > mime_encodings.h \
	  -e 's/#.*//' -e 's/[ 	]*$$//' -e '/^$$/d' \
	  -e 's/[ 	][ 	]*/", 0, "/' -e 's/^/{ "/' -e 's/$$/", 0 },/'

mime_types.h:	mime_types.txt
	rm -f mime_types.h
	sed < mime_types.txt > mime_types.h \
	  -e 's/#.*//' -e 's/[ 	]*$$//' -e '/^$$/d' \
	  -e 's/[ 	][ 	]*/", 0, "/' -e 's/^/{ "/' -e 's/$$/", 0 },/'

clean:
	rm -f *.o libthttpd.a
	rm -f $(TARGET_SO) $(OBJ)
	
