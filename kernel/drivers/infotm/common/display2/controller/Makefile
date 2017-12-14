obj-y += lcdc.o
obj-y += tvif.o
obj-y += irq.o

ifeq ($(CONFIG_ARCH_APOLLO), y)
obj-y += i80.o
obj-y += hdmi_tx.o
obj-y += synopsys_hdmi_tx_1.4a/

HDMI_LIB := drivers/infotm/common/display2/controller/synopsys_hdmi_tx_1.4a
EXTRA_CFLAGS += -I$(HDMI_LIB)/api
EXTRA_CFLAGS += -I$(HDMI_LIB)/bsp
EXTRA_CFLAGS += -I$(HDMI_LIB)/core
EXTRA_CFLAGS += -I$(HDMI_LIB)/edid
EXTRA_CFLAGS += -I$(HDMI_LIB)/hdcp
EXTRA_CFLAGS += -I$(HDMI_LIB)/phy
EXTRA_CFLAGS += -I$(HDMI_LIB)/util
endif

EXTRA_CFLAGS += -Idrivers/infotm/common/display2/include

