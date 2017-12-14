
#
#Makefile for linux kernel
#
#


#IMAPX9 support files

obj-$(CONFIG_ARCH_CORONAMPW)  += timer-gtm.o timer-cmn.o timer-hr.o\
			 pad.o power-gate.o mem-reserve.o \
	devices.o usb-phy.o irq.o sram.o
obj-$(CONFIG_PM) += pm.o sleep.o

#imapx9 machine specific support
obj-$(CONFIG_MACH_CORONAMPW)  += board-ip6313.o
obj-$(CONFIG_MACH_CORONAMPW)  += mach-coronampw.o   common.o
obj-$(CONFIG_MACH_CORONAMPW)  += items.o item_parse.o
obj-$(CONFIG_MACH_CORONAMPW)  += rballoc.o rtcbits.o
#obj-$(CONFIG_SMP)            += platsmp.o headsmp.o
ifeq ($(CONFIG_MTD_FTL),y)
obj-$(CONFIG_MACH_CORONAMPW) += crc32.o nvedit.o
endif


