
#
#Makefile for linux kernel
#
#


#IMAPX9 support files

obj-$(CONFIG_ARCH_APOLLO)  += timer-gtm.o timer-cmn.o timer-hr.o\
	pwm.o pad.o power-gate.o \
	devices.o usb-phy.o irq.o sram.o board-bcm-rfkill.o
obj-$(CONFIG_PM) += pm.o sleep.o

#imapx9 machine specific support
#obj-$(CONFIG_MACH_APOLLO)  += board-a5pv10.o
#obj-$(CONFIG_MACH_APOLLO)  += board-a5pv20.o
obj-$(CONFIG_MACH_APOLLO)  += board-q3pv10.o
obj-$(CONFIG_MACH_APOLLO)  += mach-apollo.o   common.o
obj-$(CONFIG_MACH_APOLLO)  += items.o item_parse.o
obj-$(CONFIG_MACH_APOLLO)  += rballoc.o rtcbits.o
obj-$(CONFIG_SMP)            += platsmp.o headsmp.o
ifeq ($(CONFIG_MTD_FTL),y)
obj-$(CONFIG_MACH_APOLLO) += crc32.o nvedit.o
endif


