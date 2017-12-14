#
# Makefile InfotmMedia vdec G1 driver.
#
# Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved.
#
#

include drivers/infotm/common/InfotmMedia/env.mk
SOC=imapx800
ifeq ($(SOC), imapx800)
EXTRA_CFLAGS += -DSOC_IMAPX800
endif
ifeq ($(SOC), imapx900)
EXTRA_CFLAGS += -DSOC_IMAPX900
endif


obj-y += vdec.o

EXTRA_CFLAGS += -Idrivers/infotm/common/InfotmMedia/include
EXTRA_CFLAGS += -Idrivers/infotm/common/InfotmMedia/utlz_lib/include
EXTRA_CFLAGS += -Idrivers/infotm/common/InfotmMedia/dmmu_lib/include
EXTRA_CFLAGS += -Idrivers/staging/android/ion/infotm/
