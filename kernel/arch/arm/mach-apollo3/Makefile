
#
#Makefile for linux kernel
#
#


#IMAPX9 support files

obj-$(CONFIG_ARCH_APOLLO3)  += timer-gtm.o timer-cmn.o timer-hr.o\
			 pad.o power-gate.o mem-reserve.o \
	devices.o usb-phy.o irq.o sram.o
obj-$(CONFIG_PM) += pm.o sleep.o

#imapx9 machine specific support
obj-$(CONFIG_MACH_APOLLO3)  += board-ip6313.o board-ip6303.o
obj-$(CONFIG_MACH_APOLLO3)  += mach-apollo3.o   common.o
obj-$(CONFIG_MACH_APOLLO3)  += items.o item_parse.o
obj-$(CONFIG_MACH_APOLLO3)  += rballoc.o rtcbits.o
#obj-$(CONFIG_SMP)            += platsmp.o headsmp.o
ifeq ($(CONFIG_MTD_FTL),y)
obj-$(CONFIG_MACH_APOLLO3) += crc32.o nvedit.o
endif


