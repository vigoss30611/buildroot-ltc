#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Hantro Products Oy.                             --
#-                                                                            --
#-                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
#-
#--  Abstract : Makefile for 6150 decoder kernel driver
#--
#-------------------------------------------------------------------------------
#--
#--  Version control information, please leave untouched.
#--
#--  $RCSfile: Makefile,v $
#--  $Date: 2008/09/24 09:26:24 $
#--  $Revision: 1.4 $
#--
#-----------------------------------------------------------------------------*/


ifeq ($(obj),)
obj = .
endif

#################################################
# configuration

MDIR		:= hantro

# drivers objects
list-multi	:= memalloc.o

# what to build
obj-m		:= memalloc.o

tardest		:= .

#KDIR		:= /afs/hantro.com/projects/Testing/Integration_tools/arm_sw_integration/Versatile_PB/Linux_2_6_16/linux-2.6.16/
#KDIR		:= /afs/hantro.com/projects/Testing/Integration_tools/arm_sw_integration/Versatile_PB/Linux_2_6_9/linux-2.6.9-arm1/
#KDIR		:= /afs/hantro.com/projects/Testing/Integration_tools/arm_sw_integration/Versatile_PB/Linux_2_6_14/linux-2.6.14-arm2/
#KDIR		:= /afs/hantro.com/projects/Testing/Board_Version_Control/Realview_PB/PB926EJS/SW/Linux/linux-2.6.24-arm1/v0_1/linux-2.6.24-arm1
#KDIR		:= /afs/hantro.com/projects/Testing/Board_Version_Control/Realview_PB/PB926EJS/SW/Linux/linux-2.6.24-arm2/v0_1/linux-2.6.24-arm2
KDIR            := /afs/hantro.com/projects/Testing/Board_Version_Control/SW_Common/ARM_realview_v6/2.6.24-arm2-spnlck/v0_1-v6/linux-2.6.24-arm2-spnlck

#################################################
# compile modules

ifneq ($(KERNELRELEASE),)
# recursive call from kernel build system
dummy		:= $(shell echo $(KERNELRELEASE) > $(obj)/.version)

ifeq ($(VERSION).$(PATCHLEVEL),2.6)
 export-objs	:=
 list-multi	:=
else
 multi-m	:= $(filter $(list-multi), $(obj-m))
 int-m		:= $(sort $(foreach m, $(multi-m), $($(basename $(m))-objs)))
 export-objs	:= $(filter $(int-m) $(obj-m),$(export-objs))
endif

CC += -I$(obj) -I../../source/inc
EXTRA_CFLAGS += $

snapshot := $(wildcard $(obj)/.snapshot)
ifneq ($(snapshot),)
SNAPSHOT_CFLAGS	:= -DSNAPSHOT='$(shell cat $(snapshot))'
EXTRA_CFLAGS	+= $(SNAPSHOT_CFLAGS)
endif

-include $(TOPDIR)/Rules.make
else
# take version info from last module build if available
KERNELRELEASE	:= $(shell cat $(obj)/.version 2>/dev/null || uname -r)
endif
PWD		:= $(shell pwd)
DEST		:= /lib/modules/$(KERNELRELEASE)/$(MDIR)

# which files to install?
inst-m		:= $(wildcard *.ko)
ifeq ($(inst-m),)
  inst-m	:= $(obj-m)
endif

# locales seem to cause trouble sometimes.
LC_ALL = POSIX
export LC_ALL

default::
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install::
	strip --strip-debug $(inst-m)
	-su -c "mkdir -p $(DEST); cp -v $(inst-m) $(DEST); depmod -a"

clean::
	-rm -f *~ *.o *.ko *.mod.c
	-rm -f .version .*.o.flags .*.o.d .*.o.cmd .*.ko.cmd
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

#################################################
# build tarballs

thisdir	:= $(notdir $(PWD))
name	:= $(shell echo $(thisdir) | sed 's/-.*//')
ver	:= $(shell echo $(thisdir) | sed 's/.*-//')
date    := $(shell date +%Y%m%d)
tardest	?= .

snapdir := $(HOME)/snapshot
snap	?= $(name)

release: clean
	rm -f .snapshot
	(cd ..; tar cvzf $(tardest)/$(name)-$(ver).tar.gz $(thisdir))

snapshot snap tarball: clean
	echo $(date) > .snapshot
	(cd ..; tar czf $(snapdir)/$(snap)-$(date).tar.gz $(thisdir))
	$(MAKE) -C $(snapdir)

#################################################
# other stuff

%.asm: %.o
	objdump -S $< > $@

