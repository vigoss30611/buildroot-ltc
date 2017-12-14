################################################################################
#
# qlibwifi
#
################################################################################

QLIBWIFI_VERSION = 1.0.0
QLIBWIFI_SOURCE = 
QLIBWIFI_SITE  = 

QLIBWIFI_LICENSE = 
QLIBWIFI_LICENSE_FILES = README

QLIBWIFI_MAINTAINED = YES
QLIBWIFI_AUTORECONF = YES
QLIBWIFI_INSTALL_STAGING = YES
QLIBWIFI_DEPENDENCIES = host-pkgconf  hlibitems openssl eventhub qlibsys readline

WIFI_HARDWARE_MODULE = $(call qstrip,$(BR2_WIFI_HARDWARE_MODULE))
WIFI_MODE = $(call qstrip,$(BR2_WIFI_MODE))
define QLIBWIFI_PRE_INSTALL_BUILD_HEADERS
	echo "++++++++++++++install wifi prebuild+++++++++++++++++++++++++"
	cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/$(WIFI_HARDWARE_MODULE).c  $(@D)/wifi_hal.c
endef
QLIBWIFI_PRE_BUILD_HOOKS  += QLIBWIFI_PRE_INSTALL_BUILD_HEADERS

#install headers
define QLIBWIFI_POST_INSTALL_STAGING_HEADERS
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/wifi.h  $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/wifi.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef
QLIBWIFI_POST_INSTALL_STAGING_HOOKS  += QLIBWIFI_POST_INSTALL_STAGING_HEADERS

QLIBWIFI_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_DIR)  install

define QLIBWIFI_POST_INSTALL_TARGET_SHARED
		mkdir -p $(TARGET_DIR)/wifi/
		mkdir -p $(TARGET_DIR)/wifi/firmware

if test $(WIFI_MODE) = both; then \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware $(TARGET_DIR)/wifi/; \
fi

if test $(WIFI_MODE) = sta; then \
    if test $(WIFI_HARDWARE_MODULE) = bcm6210; then \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/nvram_ap6210.txt $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/fw_bcm40181a2.bin $(TARGET_DIR)/wifi/firmware/; \
    elif test $(WIFI_HARDWARE_MODULE) = bcm6212; then \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/nvram_ap6212.txt $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/nvram_ap6212a.txt $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/fw_bcm43438a0.bin $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/fw_bcm43438a1.bin $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/config.txt $(TARGET_DIR)/wifi/firmware/; \
    elif test $(WIFI_HARDWARE_MODULE) = bcm6214; then \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/nvram_ap6214.txt $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/nvram_ap6214a.txt $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/fw_bcm43438a0.bin $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/fw_bcm43438a1.bin $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/config.txt $(TARGET_DIR)/wifi/firmware/; \
    elif test $(WIFI_HARDWARE_MODULE) = bcm6236; then \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/nvram_ap6236.txt $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/fw_bcm43436b0.bin $(TARGET_DIR)/wifi/firmware/; \
    else cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware $(TARGET_DIR)/wifi/; fi fi

if test $(WIFI_MODE) = ap; then \
    if test $(WIFI_HARDWARE_MODULE) = bcm6210; then \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/nvram_ap6210.txt $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/fw_bcm40181a2_apsta.bin $(TARGET_DIR)/wifi/firmware/; \
    elif test $(WIFI_HARDWARE_MODULE) = bcm6212; then \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/nvram_ap6212.txt $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/nvram_ap6212a.txt $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/fw_bcm43438a0_apsta.bin $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/fw_bcm43438a1_apsta.bin $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/config.txt $(TARGET_DIR)/wifi/firmware/; \
    elif test $(WIFI_HARDWARE_MODULE) = bcm6214; then \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/nvram_ap6214.txt $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/nvram_ap6214a.txt $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/fw_bcm43438a0_apsta.bin $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/fw_bcm43438a1_apsta.bin $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/config.txt $(TARGET_DIR)/wifi/firmware/; \
    elif test $(WIFI_HARDWARE_MODULE) = bcm6236; then \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/nvram_ap6236.txt $(TARGET_DIR)/wifi/firmware/; \
		cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware/fw_bcm43436b0_apsta.bin $(TARGET_DIR)/wifi/firmware/; \
    else cp -rfv $(@D)/$(WIFI_HARDWARE_MODULE)/firmware $(TARGET_DIR)/wifi/; fi fi
endef
QLIBWIFI_POST_INSTALL_TARGET_HOOKS  += QLIBWIFI_POST_INSTALL_TARGET_SHARED

$(eval $(autotools-package))

