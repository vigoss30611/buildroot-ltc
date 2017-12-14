# Copyright 2013 Google Inc. All Rights Reserved.

#DEFINES += -D_STREAM_POS_TRACE
#DEFINES += -D_DWL_DEBUG # for wrapper layer trace
#DEFINES += -DMEMORY_USAGE_TRACE #trace all memory allocations
#DEFINES += -D_READ_DEBUG_REGS
#DEFINES += -D_DWL_FAKE_HW_TIMEOUT # Change stream error interrupt to hw timeouts
#DEFINES += -D_DWL_ENABLE_ACTIVITY_TRACE # Trace hw active/idle ratio
DEFINES += -DDWL_DISABLE_REG_PRINTS # Do not trace all register accesses

ifeq ($(USE_INTERNAL_TEST),y)
  DEFINES += -DINTERNAL_TEST
  DWL_SRCS += software/linux/dwl/internal_test.c
endif

ifeq ($(strip $(ENV)),x86_linux)
  DWL_SRCS +=  software/linux/dwl/dwl_hw_core_array.c \
               software/linux/dwl/dwl_hw_core.c \
               software/linux/dwl/dwl_swhw_sync.c \
               software/linux/dwl/dwl_pc.c \
               software/linux/dwl/dwl_activity_trace.c \
               software/linux/dwl/dwl_buf_protect.c
  DEFINES += -D_DWL_PCLINUX
endif
ifneq ($(strip $(ENV)),x86_linux)
  DWL_SRCS += software/linux/dwl/dwl_linux.c \
              software/linux/dwl/dwl_linux_hw.c \
              software/linux/dwl/dwl_activity_trace.c \
              software/linux/dwl/dwl_buf_protect.c
endif

