# Copyright 2013 Google Inc. All Rights Reserved.
#
# Makefile for HEVC decoder testbench.

SRCS += $(DWL_SRCS)
SRCS += $(COMMON_SRCS)
ifeq (y,$(strip $(USE_MODEL_SIMULATION)))
  SRCS += $(SYS_SRCS)
endif
#HEVC_SRCS += software/test/hevc/dectestbench.c

#OBJS := $(sort $(patsubst %,$(OBJDIR)/%,$(SRCS:.c=.o)))
#HEVC_OBJS := $(sort $(patsubst %,$(OBJDIR)/%,$(HEVC_SRCS:.c=.o)))

#hevcdec: HEVC=1
#hevcdec: env depend $(OBJS) $(HEVC_OBJS)
#	@echo -e "[LINK]\t$(OBJDIR)/$@"
#	@$(CC) $(LDFLAGS) $(OBJS) $(HEVC_OBJS) $(LIBS) -o $(OBJDIR)/$@
#ifeq ($(strip $(STRIP)),y)
#	@echo -e "[STRIP]\t$(OBJDIR)/$@"
#	@strip --strip-all $(OBJDIR)/$@
#endif

