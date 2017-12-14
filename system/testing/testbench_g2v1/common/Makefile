# Copyright 2013 Google Inc. All Rights Reserved.
#
# Makefile for G2 decoder testbench.

G2_SRCS += $(DWL_SRCS)
G2_SRCS += $(COMMON_SRCS)
G2_SRCS += $(TEST_SRCS)
ifeq ($(findstring hevcdec,$(MAKECMDGOALS)),hevcdec)
G2_SRCS += $(HEVC_SRCS)
DEFINES += -DENABLE_HEVC_SUPPORT
endif
ifeq ($(findstring vp9dec,$(MAKECMDGOALS)),vp9dec)
G2_SRCS += $(VP9_SRCS)
DEFINES += -DENABLE_VP9_SUPPORT
endif

#ifeq ($(findstring g2dec,$(MAKECMDGOALS)),g2dec)
G2_SRCS += $(VP9_SRCS)
G2_SRCS += $(HEVC_SRCS)
DEFINES += -DENABLE_HEVC_SUPPORT
DEFINES += -DENABLE_VP9_SUPPORT
#endif
G2_SRCS += software/source/common/decapi.c \
           software/test/common/bytestream_parser.c \
           software/test/common/command_line_parser.c \
           software/test/common/error_simulator.c \
           software/test/common/file_sink.c \
           software/test/common/md5_sink.c \
           software/test/common/null_sink.c \
           software/test/common/vpxfilereader.c \
           software/test/common/yuvfilters.c

ifeq ($(strip $(USE_SDL)),y)
  G2_SRCS += software/test/common/sdl_sink.c
endif

ifeq ($(strip $(USE_MODEL_SIMULATION)),y)
  G2_SRCS += $(SYS_SRCS)
  G2_SRCS += software/test/common/simulation_trace.c
else
  G2_SRCS += software/test/common/null_trace.c
endif

DEC_SRCS += software/test/common/dectestbench.c #fengwu change
#DEC_SRCS += software/test/hevc/dectestbench.c
DEFINES += -DFIFO_DATATYPE=void* -DDOWN_SCALER

G2_OBJS := $(sort $(patsubst %,$(OBJDIR)/%,$(G2_SRCS:.c=.o)))
DEC_OBJS := $(sort $(patsubst %,$(OBJDIR)/%,$(DEC_SRCS:.c=.o)))
LDFLAGS2 += -lInfotmMediaBufferAlloc -lInfotmMediaION -L../../../release_arm/infomedia/lib/ -L../../../release_arm/lib_static/
vp9dec: env depend $(LIBS) $(G2_OBJS) $(DEC_OBJS)
	@echo -e "[LINK]\t$(OBJDIR)/$@"
	@$(CC) $(LDFLAGS) $(filter %.o, $^) $(LIBS) -o $(OBJDIR)/$@
ifeq ($(strip $(STRIP)),y)
	@echo -e "[STRIP]\t$(OBJDIR)/$@"
	@strip --strip-all $(OBJDIR)/$@
endif

hevcdec: env depend $(LIBS) $(G2_OBJS) $(DEC_OBJS)
	@echo -e "[LINK]\t$(OBJDIR)/$@"
	@$(CC) $(LDFLAGS) $(filter %.o, $^) $(LIBS) -o $(OBJDIR)/$@
ifeq ($(strip $(STRIP)),y)
	@echo -e "[STRIP]\t$(OBJDIR)/$@"
	@strip --strip-all $(OBJDIR)/$@
endif

g2lib: env depend $(LIBS) $(G2_OBJS) 
	@echo -e "[LINK]\t$(OBJDIR)/$@"
	$(CC) $(LDFLAGS) $(filter %.o, $^) $(LIBS) -o $(OBJDIR)/$@ $(LDFLAGS2) 
	
#fengwu add LDFLAGS2
g2dec: env depend $(LIBS) $(G2_OBJS) $(DEC_OBJS)
	@echo -e "[LINK]\t$(OBJDIR)/$@"
	@echo  "LIBS: $(LIBS)"
	@echo  "G2_OBJS: $(G2_OBJS) "
	@echo  "DEC_OBJS: $(DEC_OBJS)"
	$(CC) $(LDFLAGS) $(filter %.o, $^) $(LIBS) -o $(OBJDIR)/$@ $(LDFLAGS2) 
	$(AR)  libg2.a `find out -name "*.o"` 
	mkdir -p ../../../release_arm/lib_static
	cp libg2.a ../../../release_arm/lib_static
ifeq ($(strip $(SYMLINKS)),y)
	@ln -fs $@ $(OBJDIR)/hevcdec
	@ln -fs $@ $(OBJDIR)/vp9dec
endif
ifeq ($(strip $(STRIP)),y)
	@echo -e "[STRIP]\t$(OBJDIR)/$@"
	@strip --strip-all $(OBJDIR)/$@
endif
