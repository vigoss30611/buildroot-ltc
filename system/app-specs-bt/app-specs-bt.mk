################################################################################
#
# qiwobt
#
################################################################################

APP_SPECS_BT_VERSION = 1.0.0
APP_SPECS_BT_SOURCE = 
APP_SPECS_BT_SITE  = 

APP_SPECS_BT_LICENSE = 
APP_SPECS_BT_LICENSE_FILES = README

APP_SPECS_BT_MAINTAINED = YES
APP_SPECS_BT_AUTORECONF = YES
APP_SPECS_BT_INSTALL_STAGING = YES
APP_SPECS_BT_DEPENDENCIES = qlibsys eventhub

APP_SPECS_BT_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_DIR)  install

define APP_SPECS_BT_POST_INSTALL_TARGET_SHARED
		mkdir -p $(TARGET_DIR)/bt/
		cp -rfv $(@D)/firmware/bcm43438a1.hcd $(TARGET_DIR)/bt/
		cp -rfv $(@D)/firmware/bcm43430b0.hcd $(TARGET_DIR)/bt/
		cp -rfv $(@D)/firmware/bsa_server $(TARGET_DIR)/usr/bin/
		cp -rfv $(@D)/start_bsa_server $(TARGET_DIR)/usr/bin/
endef
APP_SPECS_BT_POST_INSTALL_TARGET_HOOKS  += APP_SPECS_BT_POST_INSTALL_TARGET_SHARED

$(eval $(autotools-package))

