#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Verisilicon.					                          --
#-                                                                            --
#-                   (C) COPYRIGHT 2014 VERISILICON.                          --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
#-
#--  Abstract : Makefile for memalloc kernel driver
#--
#-------------------------------------------------------------------------------

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DMEMALLOC_DEBUG
else
  DEBFLAGS = -O2
endif

# base address and size (MB) for linear memory allocation
HLINA_START := 0x04000000
#HLINA_START := 0x08000000
#HLINA_START := 0xc8100000
#HLINA_START := 0xB0100000
#HLINA_SIZE  := 96
#HLINA_SIZE  := 256
HLINA_SIZE  := 250 

EXTRA_CFLAGS += $(DEBFLAGS)
EXTRA_CFLAGS += -DHLINA_START_ADDRESS=$(HLINA_START) -DHLINA_SIZE=$(HLINA_SIZE)

ifneq ($(KERNELRELEASE),)
# recursive call from kernel build system

memalloc-objs :=
obj-m         += memalloc.o

else

KDIR_BASE := /afs/hantro.com/projects/Testing/Board_Version_Control

#KDIR := $(KDIR_BASE)/Realview_EB/SW/Linux/v0_0/linux-2.6.19-arm2/
#KDIR := $(KDIR_BASE)/Realview_EB/SW/Linux/linux-2.6.21-arm1/v0_0/linux-2.6.21-arm1
#KDIR := $(KDIR_BASE)/Realview_PB/PB926EJS/SW/Linux/linux-2.6.24-arm2-spnlck/v0_1/linux-2.6.24-arm2-spnlck
#KDIR := $(KDIR_BASE)/Realview_PB/PB926EJS/SW/Linux/linux-2.6.28-arm1/v0_1/linux-2.6.28-arm1
KDIR := $(KDIR_BASE)/SW_Common/ARM_realview_v6/2.6.28-arm1/v0_1-v6/linux-2.6.28-arm1

PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
	rm -rf modules.order Module.symvers
