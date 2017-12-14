# Copyright 2013 Google Inc. All Rights Reserved.

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DHANTRODEC_DEBUG
else
  DEBFLAGS = -O2 -DG1G2_MUTEX
endif

EXTRA_CFLAGS += $(DEBFLAGS)
EXTRA_CFLAGS += -Idrivers/infotm/common/InfotmMedia/external/project/fpga_vdec_264_g1/

ifneq ($(KERNELRELEASE),)
# recursive call from kernel build system

hantrodec-objs :=
obj-m         += vdec_g2.o

else

KDIR := /mnt/export/Testing/Board_Version_Control/SW_Common/SOCLE_MDK-3D/openlinux/2.6.29/v0_5/android_linux-2.6.29

PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
	rm -rf modules.order Module.symvers
