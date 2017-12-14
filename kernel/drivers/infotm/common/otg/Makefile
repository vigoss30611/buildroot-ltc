#
# Makefile for DWC_otg Highspeed USB controller driver
#


ccflags-$(CONFIG_DWC_OTG_DEBUG) += -DDEBUG

# Use one of the following flags to compile the software in host-only or
# device-only mode.
ccflags-$(CONFIG_DWC_OTG_HOST_ONLY) += -DDWC_HOST_ONLY
ccflags-$(CONFIG_DWC_OTG_DEVICE_ONLY) += -DDWC_DEVICE_ONLY

KBUILD_CPPFLAGS		+= -Dlinux -DDWC_HS_ELECT_TST
#KBUILD_CPPFLAGS	+= -DDWC_EN_ISOC
KBUILD_CPPFLAGS   	+= -I$(PWD)/../dwc_common_port
#KBUILD_CPPFLAGS   	+= -I$(PORTLIB)
KBUILD_CPPFLAGS   	+= -DDWC_LINUX
KBUILD_CPPFLAGS   	+= $(CFI)
KBUILD_CPPFLAGS		+= $(BUS_INTERFACE)
#KBUILD_CPPFLAGS		+= -DDWC_DEV_SRPCAP

obj-$(CONFIG_INFOTM_OTG)	:= dwc_otg.o

dwc_otg-objs	:= dwc_otg_driver.o dwc_otg_attr.o 
dwc_otg-objs	+= dwc_otg_cil.o dwc_otg_cil_intr.o 
dwc_otg-objs	+= dwc_otg_pcd_linux.o dwc_otg_pcd.o dwc_otg_pcd_intr.o 
dwc_otg-objs	+= dwc_otg_hcd.o dwc_otg_hcd_linux.o dwc_otg_hcd_intr.o dwc_otg_hcd_queue.o dwc_otg_hcd_ddma.o
dwc_otg-objs	+= dwc_otg_adp.o
ifneq ($(CFI),)
dwc_otg-objs	+= dwc_otg_cfi.o
endif
dwc_otg-objs	+= dwc_common_linux.o


