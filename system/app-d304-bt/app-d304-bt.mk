################################################################################
#
# qiwobt
#
################################################################################

APP_D304_BT_VERSION = 1.0.0
APP_D304_BT_SOURCE = 
APP_D304_BT_SITE  = 

APP_D304_BT_LICENSE = 
APP_D304_BT_LICENSE_FILES = README

APP_D304_BT_MAINTAINED = YES
APP_D304_BT_AUTORECONF = YES
APP_D304_BT_INSTALL_STAGING = YES
APP_D304_BT_DEPENDENCIES = qlibsys eventhub

APP_D304_BT_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_DIR)  install

define APP_D304_BT_POST_INSTALL_TARGET_SHARED
		mkdir -p $(TARGET_DIR)/bt/
		cp -rfv $(@D)/firmware/bcm43438a0.hcd $(TARGET_DIR)/bt/
		cp -rfv $(@D)/firmware/bcm43438a1.hcd $(TARGET_DIR)/bt/
		cp -rfv $(@D)/firmware/bsa_server $(TARGET_DIR)/usr/bin/
		cp -rfv $(@D)/start_bsa_server $(TARGET_DIR)/usr/bin/
endef
APP_D304_BT_POST_INSTALL_TARGET_HOOKS  += APP_D304_BT_POST_INSTALL_TARGET_SHARED

$(eval $(autotools-package))

