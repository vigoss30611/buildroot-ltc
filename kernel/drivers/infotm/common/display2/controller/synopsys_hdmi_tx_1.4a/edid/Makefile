#
# Makefile for the Synopsys HDMI driver
#

obj-y	+= colorimetryDataBlock.o
obj-y	+= dtd.o
obj-y	+= edid.o
obj-y	+= halEdid.o
obj-y	+= hdmivsdb.o
obj-y	+= monitorRangeLimits.o
obj-y	+= shortAudioDesc.o
obj-y	+= shortVideoDesc.o
obj-y	+= speakerAllocationDataBlock.o
obj-y	+= videoCapabilityDataBlock.o

HDMI_LIB := drivers/infotm/common/display2/controller/synopsys_hdmi_tx_1.4a
EXTRA_CFLAGS += -I$(HDMI_LIB)/api
EXTRA_CFLAGS += -I$(HDMI_LIB)/bsp
EXTRA_CFLAGS += -I$(HDMI_LIB)/core
EXTRA_CFLAGS += -I$(HDMI_LIB)/edid
EXTRA_CFLAGS += -I$(HDMI_LIB)/hdcp
EXTRA_CFLAGS += -I$(HDMI_LIB)/phy
EXTRA_CFLAGS += -I$(HDMI_LIB)/util

