
#
#Makefile for linux kernel
#
#


#IMAPX9 support files

obj-$(CONFIG_ARCH_IMAPX910)  += timer-gtm.o timer-cmn.o timer-hr.o\
	pwm.o pad.o power-gate.o mem-reserve.o \
	devices.o usb-phy.o irq_dispatch.o sram.o
obj-$(CONFIG_PM) += pm.o sleep.o

#imapx9 machine specific support
obj-$(CONFIG_MACH_IMAPX910)  += board-a5pv10.o
obj-$(CONFIG_MACH_IMAPX910)  += board-a5pv20.o
obj-$(CONFIG_MACH_IMAPX910)  += board-a9pv10.o
obj-$(CONFIG_MACH_IMAPX910)  += mach-imapx910.o   common.o
obj-$(CONFIG_MACH_IMAPX910)  += items.o item_parse.o
obj-$(CONFIG_MACH_IMAPX910)  += nvedit.o rballoc.o crc32.o rtcbits.o
obj-$(CONFIG_SMP)            += platsmp.o headsmp.o


