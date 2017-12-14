# copyright (c) 2011 - 2012 espressif system
# linux_sdio
#ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
ARCH := arm
CROSS_COMPILE := $(TOOLCHAIN_PREFIX)
KSRC := $(KERNEL_DIR)
MODPATH := $(shell pwd)
DRIVER_NAME := esp8089

#include $(EAGLE_BASE_DIR)/esp_config.mk
EXTRA_CFLAGS += -DTICMMC -DESP_ACK_INTERRUPT -DHOST_RESET_BUG -DP2P_CONCURRENT -DESP_USE_SDIO -DHAS_INIT_DATA \
                -DESP_PREALLOC 
                #-DINIT_DATA_CONF -DESP_ANDROID_LOGGER -DESP_CLASS -DMAC80211_NO_CHANGE -DMMC_NO_CHANG
EXTRA_CFLAGS += -DDEBUG -DSIP_DEBUG -DFAST_TX_STATUS -DKERNEL_IV_WAR -DRX_SENDUP_SYNC -DHAS_FW -DTEST_MODE  

obj-$(CONFIG_ESP8089) := $(DRIVER_NAME).o
$(DRIVER_NAME)-y += esp_init.o
$(DRIVER_NAME)-y += esp_debug.o
$(DRIVER_NAME)-y += sdio_sif_esp.o
$(DRIVER_NAME)-y += spi_sif_esp.o
$(DRIVER_NAME)-y += esp_io.o
$(DRIVER_NAME)-y += esp_file.o
$(DRIVER_NAME)-y += esp_main.o
$(DRIVER_NAME)-y += esp_sip.o
$(DRIVER_NAME)-y += esp_ext.o
$(DRIVER_NAME)-y += esp_ctrl.o
$(DRIVER_NAME)-y += esp_mac80211.o
$(DRIVER_NAME)-y += esp_debug.o
$(DRIVER_NAME)-y += esp_utils.o
$(DRIVER_NAME)-y += esp_pm.o
$(DRIVER_NAME)-y += testmode.o

$(DRIVER_NAME):
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(MODPATH) modules

.PHONE:clean
clean:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(MODPATH) clean 

#else
#KDIR ?= /lib/modules/`uname -r`/build
##KDIR ?= /home/espressif/src/sdio/cleanup

#default:
#	$(MAKE) -C $(KDIR) M=$$PWD

#endif
