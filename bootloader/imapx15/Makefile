#
# (C) Copyright 2006-2007
# Stefan Roese, DENX Software Engineering, sr@denx.de.
#
# (C) Copyright 2008
# Guennadi Liakhovetki, DENX Software Engineering, <lg@denx.de>
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#
TOPDIR =$(shell pwd)
OBJDIR =$(TOPDIR)/out/
SRCDIR =$(TOPDIR)/src/
INCDIR =$(TOPDIR)/inc/
TOOLDIR =$(TOPDIR)/tool/

PLATFORM_LDFLAGS= -L
#TEXT_BASE = 0x3c017800
TEXT_BASE =0x3c000200 
INC_DIR =$(INCDIR)
INC_DIR +=$(INCDIR)asm-arm
INC_DIR +=$(INCDIR)asm
INC_DIR +=$(INCDIR)infotm
INC_DIR +=$(INCDIR)configs

# Set flags
ALL	= $(OBJDIR)uboot_lite-spl $(OBJDIR)uboot_lite-spl.bin $(OBJDIR)uboot_lite.isi

CFLAGS =  $(foreach incdir,$(INC_DIR),$(addprefix -I,$(incdir))) \
-g  -Os   -fno-strict-aliasing  -fno-common -ffixed-r8  -fno-strict-aliasing -msoft-float -fno-common -ffixed-r8  \
-DTEXT_BASE=$(TEXT_BASE) -fno-builtin -ffreestanding -nostdinc -pipe \
-DCONFIG_ARM -D__ARM__ -marm  -mabi=aapcs-linux -mno-thumb-interwork -march=armv7-a -Wall -Wstrict-prototypes -fno-stack-protector \
-DCONFIG_NAND_SPL -DCONFIG_PRELOADER -D__KERNEL__ -mthumb

ifdef MODE                                                                      
CFLAGS += -DMODE=SIMPLE                                                         
endif                                                                           

ifeq ($(ERASE_EMMC),y)                                                          
CFLAGS += -DERASE_EMMC                                                          
endif                                                                           
ifeq ($(ERASE_SPIFLASH),y)                                                      
CFLAGS += -DERASE_SPIFLASH                                                      
endif

ifeq ($(BOARD_BOOT_DEV_EMMC), y)
CFLAGS += -DCONFIG_BOARD_BOOT_DEV_EMMC
endif

ifeq ($(BOARD_BOOT_DEV_FLASH), y)
#CFLAGS += -DCONFIG_BOARD_BOOT_DEV_FLASH
endif

ifeq ($(BOARD_HAVE_UDC_BURN), y)
CFLAGS += -DCONFIG_BOARD_HAVE_UDC_BURN
endif

ifeq ($(BOARD_PLAT_FPGA),y)
CFLAGS += -DCONFIG_IMAPX_FPGATEST
endif

LDSCRIPT = $(TOPDIR)/uboot_lite.lds
LDFLAGS	= -Bstatic -T $(OBJDIR)uboot_lite.lds -Ttext $(TEXT_BASE) $(PLATFORM_LDFLAGS) -lgcc
OBJFLAGS = --gap-fill=0xff -O binary

mach_arch=$(shell arch)
ifeq ("$(mach_arch)","x86_64")
IUW=iuw64
else
IUW=iuw32
endif

ISI =$(TOOLDIR)$(IUW) genisi -n "uboot0" -t boot -a 0x3c000200 -e 0x3c000200 -f 0x940

# Sources
GLSOBJS += _divsi3.o _udivsi3.o
GLCOBJS += div0.o 
SOBJS	= start.o cpu_init.o lowlevel_init.o
COBJS-y	+= rballoc.o boot_main.o items.o rtcbit.o board.o imapx800_wdt.o iic.o pwm.o nand_uboot0.o nand_ecc.o
COBJS-$(BOARD_BOOT_DEV_FLASH) +=  gdma.o #flash_fpga.o
COBJS-$(BOARD_BOOT_DEV_EMMC) += div64.o timer.o imapx800_sdhc.o mmc.o 

COBJS-$(BOARD_PLAT_ASIC) += iex_2939.o
COBJS-$(BOARD_PLAT_ASIC) += iex_2885.o
COBJS-$(BOARD_PLAT_ASIC) += iex_2974.o
COBJS-$(BOARD_PLAT_ASIC) += iex_c31c431.o
COBJS-$(BOARD_PLAT_ASIC) += iex_4eadb12.o

COBJS-$(BOARD_PLAT_FPGA) += dramc_fpga.o
COBJS-$(BOARD_PLAT_ASIC) += dramc.o
COBJS-$(BOARD_HAVE_UDC_BURN) += imapx800_udc.o linkpc.o

COBJS   := $(COBJS-y)

SRCS	 = $(addprefix $(SRCDIR),$(GLCOBJS:%.o=%.c) $(GLSOBJS:%.o=%.S) $(SOBJS:%.o=%.S) $(COBJS:%.o=%.c))
OBJS	 = $(addprefix $(OBJDIR),$(GLCOBJS) $(GLSOBJS) $(SOBJS) $(COBJS))

all:	$(SRCDIR)iex_2939.c $(OBJDIR).depend $(ALL)
all:	$(SRCDIR)iex_2885.c $(OBJDIR).depend $(ALL)
all:	$(SRCDIR)iex_2974.c $(OBJDIR).depend $(ALL)
all:	$(SRCDIR)iex_c31c431.c $(OBJDIR).depend $(ALL)
all:	$(SRCDIR)iex_4eadb12.c $(OBJDIR).depend $(ALL)

$(OBJDIR)uboot_lite.isi: $(OBJDIR)uboot_lite-spl.bin
	$(ISI) -d $< -o $@

$(OBJDIR)uboot_lite-spl.bin: $(OBJDIR)uboot_lite-spl
	$(OBJCOPY) $(OBJFLAGS) $< $@

$(OBJDIR)uboot_lite-spl: $(OBJS) $(OBJDIR)uboot_lite.lds
	cd $(OBJDIR) && $(LD) $(LDFLAGS) $(GLCOBJS) $(GLSOBJS) $(SOBJS) $(COBJS) \
		-Map $(OBJDIR)uboot_lite-spl.map \
		-o $(OBJDIR)uboot_lite-spl

$(OBJDIR)uboot_lite.lds: $(LDSCRIPT)
	$(CPP) $(CPPFLAGS) $(LDPPFLAGS) -ansi -D__ASSEMBLY__ -P - <$^ >$@

$(SRCDIR)iex_2939.c: $(INCDIR)/infotm/preloader.h
	@rm -f $@
	$(TOOLDIR)gen-entry.sh 2939 $(TOOLDIR)labels $(TOOLDIR)i2939.S > $@

$(SRCDIR)iex_2885.c: $(INCDIR)/infotm/preloader.h
	@rm -f $@
	$(TOOLDIR)gen-entry.sh 2885 $(TOOLDIR)labels $(TOOLDIR)i2885.S > $@	

$(SRCDIR)iex_2974.c: $(INCDIR)/infotm/preloader.h
	@rm -f $@
	$(TOOLDIR)gen-entry.sh 2974 $(TOOLDIR)labels $(TOOLDIR)i2974.S > $@	

$(SRCDIR)iex_c31c431.c: $(INCDIR)/infotm/preloader.h
	@rm -f $@
	$(TOOLDIR)gen-entry.sh c31c431 $(TOOLDIR)labels $(TOOLDIR)ic31c431.S > $@	

$(SRCDIR)iex_4eadb12.c: $(INCDIR)/infotm/preloader.h
	@rm -f $@
	$(TOOLDIR)gen-entry.sh 4eadb12 $(TOOLDIR)labels $(TOOLDIR)i4eadb12.S > $@	

	
$(OBJDIR)%.o:	$(SRCDIR)%.S
	$(CC) $(AFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR)%.o:	$(SRCDIR)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# defines $(out).depend target
#include $(SRCTREE)/rules.mk

$(OBJDIR).depend:
	-mkdir -p out
	@rm -f $@
#	$(CC) $(HOSTCFLAGS) $(CPPFLAGS) $(CFLAGS) -M $(SRCS)>> $@
sinclude $(OBJDIR).depend

clean:
	-rm -rf $(OBJDIR)*
.PHONY:clean $(OBJDIR).depend $(SRCDIR)iex_2939.c \
	$(SRCDIR)iex_2885.c $(SRCDIR)iex_2974.c \
	$(SRCDIR)iex_c31c431.c $(SRCDIR)iex_4eadb12.c
