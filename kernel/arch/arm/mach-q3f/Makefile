
#
#Makefile for linux kernel
#
#


#IMAPX9 support files

obj-$(CONFIG_ARCH_Q3F)  += timer-gtm.o timer-cmn.o timer-hr.o\
			 pad.o power-gate.o mem-reserve.o \
	devices.o usb-phy.o irq.o sram.o
obj-$(CONFIG_PM) += pm.o sleep.o

#imapx9 machine specific support
#obj-$(CONFIG_MACH_Q3F)  += board-a5pv10.o
#obj-$(CONFIG_MACH_Q3F)  += board-a5pv20.o
#obj-$(CONFIG_MACH_Q3F)  += board-q3pv10.o
obj-$(CONFIG_MACH_Q3F)  += board-ip620x.o
obj-$(CONFIG_MACH_Q3F)  += board-ip6103.o
obj-$(CONFIG_MACH_Q3F)  += mach-q3f.o   common.o
obj-$(CONFIG_MACH_Q3F)  += items.o item_parse.o
obj-$(CONFIG_MACH_Q3F)  += rballoc.o rtcbits.o
#obj-$(CONFIG_SMP)            += platsmp.o headsmp.o
ifeq ($(CONFIG_MTD_FTL),y)
obj-$(CONFIG_MACH_Q3F) += crc32.o nvedit.o
endif


