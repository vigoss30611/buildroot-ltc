# Copyright 2013 Google Inc. All Rights Reserved.

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DHANTRODEC_DEBUG
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(DEBFLAGS)
EXTRA_CFLAGS += -I$(PWD)/../dwl

ifneq ($(KERNELRELEASE),)
# recursive call from kernel build system

hantrodec-objs :=
obj-m         += hantrodec.o

else

KDIR := /mnt/export/Testing/Board_Version_Control/SW_Common/SOCLE_MDK-3D/openlinux/2.6.29/v0_5/android_linux-2.6.29

PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
	rm -rf modules.order Module.symvers
