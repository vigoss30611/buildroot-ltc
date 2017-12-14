ifeq ($(CONFIG_DISPLAY_IMAPX), y)
EXTRA_CFLAGS += -Idrivers/infotm/apollo/display/implementation/module/interface/hdmi/synopsys_hdmi_tx_1.4a/core/
EXTRA_CFLAGS += -Idrivers/infotm/apollo/display/implementation/module/interface/hdmi/synopsys_hdmi_tx_1.4a/util/
endif
ifeq ($(CONFIG_INFOTM_DISPLAY2), y)
EXTRA_CFLAGS += -Idrivers/infotm/common/display2/controller/synopsys_hdmi_tx_1.4a/core/
EXTRA_CFLAGS += -Idrivers/infotm/common/display2/controller/synopsys_hdmi_tx_1.4a/util/
endif
snd-soc-imapx-i2s-objs := imapx-i2s.o
obj-$(CONFIG_SND_IMAPX_SOC_IIS) += snd-soc-imapx-i2s.o

snd-soc-imapx-pcm-objs := imapx-pcm.o
obj-$(CONFIG_SND_IMAPX_SOC_PCM) += snd-soc-imapx-pcm.o

#snd-soc-imapx-snd-spdif-objs := imapx-spdif.o
#obj-$(CONFIG_SND_IMAPX_SOC_SPDIF) += snd-soc-imapx-snd-spdif.o

snd-soc-imapx-dma-objs := dma-ops.o imapx-dma.o  imapx-aec.o
obj-$(CONFIG_SND_IMAPX_SOC_DMA)  += snd-soc-imapx-dma.o

obj-$(CONFIG_TQLP9624_ADC) += imapx-adc.o
