# Copyright 2013 Google Inc. All Rights Reserved.

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DHANTRODEC_DEBUG
else
  DEBFLAGS = -O2
endif

ifneq ($(KERNELRELEASE),)
# recursive call from kernel build system

hantrodec-objs :=
obj-m         += hantrodec.o

else

KVER := $(shell uname -r)
KDIR := /lib/modules/$(KVER)/build

PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
	rm -rf modules.order Module.symvers
