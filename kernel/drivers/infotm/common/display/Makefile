obj-$(CONFIG_DISPLAY_IMAPX) += common/
obj-$(CONFIG_DISPLAY_IMAPX) += implementation/
obj-$(CONFIG_DISPLAY_IMAPX) += abstraction/
#obj-$(CONFIG_DISPLAY_IMAPX) += strategy/
obj-$(CONFIG_DISPLAY_IMAPX) += display_wa.o
obj-$(CONFIG_DISPLAY_IMAPX) += imapfb.o

EXTRA_CFLAGS += -Idrivers/infotm/common/display/common
EXTRA_CFLAGS += -Idrivers/infotm/common/display/abstraction
EXTRA_CFLAGS += -Idrivers/infotm/common/display/implementation
EXTRA_CFLAGS += -Idrivers/infotm/common/display/implementation/module/access
EXTRA_CFLAGS += -Idrivers/infotm/common/display/implementation/module/controller
EXTRA_CFLAGS += -Idrivers/infotm/common/display/implementation/module/interface
EXTRA_CFLAGS += -Idrivers/infotm/common/display/implementation/module/scaler
EXTRA_CFLAGS += -Idrivers/infotm/common/InfotmMedia/include
EXTRA_CFLAGS += -Idrivers/infotm/common/InfotmMedia/dmmu_lib/include/
#if defined(CONFIG_ARCH_IMAPX15) && defined (CONFIG_IMAPX15_G2D)
EXTRA_CFLAGS += -Idrivers/infotm/imapx15/g2d/
#endif
EXTRA_CFLAGS += -Idrivers/infotm/common/display/implementation/module/interface/hdmi/synopsys_hdmi_tx_1.4a/api
EXTRA_CFLAGS += -Idrivers/infotm/common/display/implementation/module/interface/hdmi/synopsys_hdmi_tx_1.4a/bsp
EXTRA_CFLAGS += -Idrivers/infotm/common/display/implementation/module/interface/hdmi/synopsys_hdmi_tx_1.4a/core
EXTRA_CFLAGS += -Idrivers/infotm/common/display/implementation/module/interface/hdmi/synopsys_hdmi_tx_1.4a/edid
EXTRA_CFLAGS += -Idrivers/infotm/common/display/implementation/module/interface/hdmi/synopsys_hdmi_tx_1.4a/hdcp
EXTRA_CFLAGS += -Idrivers/infotm/common/display/implementation/module/interface/hdmi/synopsys_hdmi_tx_1.4a/phy
EXTRA_CFLAGS += -Idrivers/infotm/common/display/implementation/module/interface/hdmi/synopsys_hdmi_tx_1.4a/util
