################################################################################
#
# qiwoupgrade
#
################################################################################

APP_D304_UPGRADE_VERSION = 1.0.0
APP_D304_UPGRADE_SOURCE = 
APP_D304_UPGRADE_SITE  = 

APP_D304_UPGRADE_LICENSE = 
APP_D304_UPGRADE_LICENSE_FILES = README

APP_D304_UPGRADE_MAINTAINED = YES
APP_D304_UPGRADE_AUTORECONF = YES
APP_D304_UPGRADE_INSTALL_STAGING = YES
APP_D304_UPGRADE_DEPENDENCIES = hlibitems libcurl qlibsys qlibwifi eventhub qlibupgrade

APP_D304_UPGRADE_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_INITRD_DIR) install

define APP_D304_UPGRADE_NET
		mkdir -p ${TARGET_INITRD_DIR}/wifi/firmware/
		mkdir -p ${TARGET_INITRD_DIR}/lib/modules
		mkdir -p $(TARGET_INITRD_DIR)/etc/ 
		-cp -rfv $(TARGET_DIR)/wifi/firmware/fw_bcm43438a0.bin $(TARGET_INITRD_DIR)/wifi/firmware/
		-cp -rfv $(TARGET_DIR)/wifi/firmware/nvram_ap6212.txt $(TARGET_INITRD_DIR)/wifi/firmware/
		-cp -rfv $(TARGET_DIR)/usr/lib/libwifi.so $(TARGET_INITRD_DIR)/usr/lib/libwifi.so
		-cp -rfv $(TARGET_DIR)/usr/lib/libevent.so.1.0.0 $(TARGET_INITRD_DIR)/usr/lib/libevent.so.1
		-cp -rfv $(TARGET_DIR)/usr/lib/libhistory.so.6.2 $(TARGET_INITRD_DIR)/usr/lib/libhistory.so.6
		-cp -rfv $(TARGET_DIR)/usr/lib/libreadline.so.6.2 $(TARGET_INITRD_DIR)/usr/lib/libreadline.so.6
		-cp -rfv $(TARGET_DIR)/usr/lib/libncurses.so.5.9 $(TARGET_INITRD_DIR)/usr/lib/libncurses.so.5
		-cp -rfv $(TARGET_DIR)/usr/lib/libssl.so.1.0.0 $(TARGET_INITRD_DIR)/usr/lib/libssl.so.1.0.0
		-cp -rfv $(TARGET_DIR)/usr/lib/libz.so.1.2.8 $(TARGET_INITRD_DIR)/usr/lib/libz.so.1
		-cp -rfv $(TARGET_DIR)/usr/lib/libcrypto.so.1.0.0 $(TARGET_INITRD_DIR)/usr/lib/libcrypto.so.1.0.0
		-cp -rfv $(TARGET_DIR)/usr/lib/libncurses.so.5.9 $(TARGET_INITRD_DIR)/usr/lib/libncurses.so.5
		-cp -rfv $(TARGET_DIR)/usr/lib/libiconv.so.2.5.1 $(TARGET_INITRD_DIR)/usr/lib/libiconv.so.2
		-cp -rfv $(TARGET_DIR)/lib/modules/3.10.0-infotm+/kernel/drivers/infotm/common/net/bcmdhd6212/bcmdhd.ko  $(TARGET_INITRD_DIR)/wifi/
		-cp -rfv $(TARGET_DIR)/usr/sbin/wpa_supplicant  $(TARGET_INITRD_DIR)/usr/bin/
		-cp -rfv $(TARGET_DIR)/usr/lib/libnl-3.so.200.18.0 $(TARGET_INITRD_DIR)/usr/lib/libnl-3.so
		-cp -rfv $(TARGET_DIR)/usr/lib/libnl-genl-3.so.200.18.0  $(TARGET_INITRD_DIR)/usr/lib/libnl-genl-3.so
		-cp -rfv $(TARGET_DIR)/usr/lib/libcurl.so.4.3.0  $(TARGET_INITRD_DIR)/usr/lib/libcurl.so.4
		-cp -rfv $(TARGET_DIR)/usr/bin/dhcpcd  $(TARGET_INITRD_DIR)/usr/bin/
		-cp -rfv $(TARGET_DIR)/libexec/  $(TARGET_INITRD_DIR)/
		-cp -rfv $(TARGET_DIR)/var/  $(TARGET_INITRD_DIR)/
		-cp -rfv $(TARGET_DIR)/run/  $(TARGET_INITRD_DIR)/
		-cp -rfv $(TARGET_DIR)/etc/dhcpcd.conf  $(TARGET_INITRD_DIR)/etc/
		-cp -rfv $(TARGET_DIR)/etc/wpa_supplicant.conf   $(TARGET_INITRD_DIR)/etc/
endef

ifeq ($(BR2_PACKAGE_APP_D304_UPGRADE),y)
APP_D304_UPGRADE_POST_INSTALL_TARGET_HOOKS  += APP_D304_UPGRADE_NET 
endif

$(eval $(autotools-package))

