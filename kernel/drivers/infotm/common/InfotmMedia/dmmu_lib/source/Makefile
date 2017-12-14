#
# Makefile InfotmMeida pmm i800 driver.
#
# Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
#
#


include drivers/infotm/common/InfotmMedia/env.mk

ifeq ($(SOC), imapx800)
EXTRA_CFLAGS += -DSOC_IMAPX800
endif
ifeq ($(SOC), imapx900)
EXTRA_CFLAGS += -DSOC_IMAPX900
endif


obj-y += dmmu_lib.o
obj-y += dmmu_pwl_linux.o

EXTRA_CFLAGS += -Idrivers/infotm/common/InfotmMedia/include
EXTRA_CFLAGS += -Idrivers/infotm/common/InfotmMedia/dmmu_lib/include
EXTRA_CFLAGS += -Idrivers/infotm/common/InfotmMedia/dmmu_lib/source

EXTRA_CFLAGS += -DIM_DEBUG

