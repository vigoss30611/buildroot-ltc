#
# Makefile for the Synopsys HDMI driver
#

obj-y	+= audio.o
obj-y	+= audioParams.o
obj-y	+= control.o
obj-y	+= halAudioClock.o
obj-y	+= halAudioGenerator.o
obj-y	+= halAudioGpa.o
obj-y	+= halAudioHbr.o
obj-y	+= halAudioI2s.o
obj-y	+= halAudioSpdif.o
obj-y	+= halColorSpaceConverter.o
obj-y	+= halFrameComposerAcp.o
obj-y	+= halFrameComposerAudio.o
obj-y	+= halFrameComposerAudioInfo.o
obj-y	+= halFrameComposerAvi.o
obj-y	+= halFrameComposerDebug.o
obj-y	+= halFrameComposerGamut.o
obj-y	+= halFrameComposerGcp.o
obj-y	+= halFrameComposerIsrc.o
obj-y	+= halFrameComposerPackets.o
obj-y	+= halFrameComposerSpd.o
obj-y	+= halFrameComposerVideo.o
obj-y	+= halFrameComposerVsd.o
obj-y	+= halIdentification.o
obj-y	+= halInterrupt.o
obj-y	+= halMainController.o
obj-y	+= halVideoGenerator.o
obj-y	+= halVideoPacketizer.o
obj-y	+= halVideoSampler.o
obj-y	+= packets.o
obj-y	+= productParams.o
obj-y	+= video.o
obj-y	+= videoParams.o

HDMI_LIB := drivers/infotm/common/display2/controller/synopsys_hdmi_tx_1.4a
EXTRA_CFLAGS += -I$(HDMI_LIB)/api
EXTRA_CFLAGS += -I$(HDMI_LIB)/bsp
EXTRA_CFLAGS += -I$(HDMI_LIB)/core
EXTRA_CFLAGS += -I$(HDMI_LIB)/edid
EXTRA_CFLAGS += -I$(HDMI_LIB)/hdcp
EXTRA_CFLAGS += -I$(HDMI_LIB)/phy
EXTRA_CFLAGS += -I$(HDMI_LIB)/util

