################################################################################
#
#common event provider
#
################################################################################

APP_BOOT_SELECTOR_VERSION = 1.0.0
APP_BOOT_SELECTOR_SOURCE = 
APP_BOOT_SELECTOR_SITE  = 

APP_BOOT_SELECTOR_LICENSE = 
APP_BOOT_SELECTOR_LICENSE_FILES = README

APP_BOOT_SELECTOR_MAINTAINED = YES
APP_BOOT_SELECTOR_AUTORECONF = YES
APP_BOOT_SELECTOR_INSTALL_STAGING = YES
APP_BOOT_SELECTOR_DEPENDENCIES = host-pkgconf cep qlibsys eventhub

#install headers
define APP_BOOT_SELECTOR_POST_INSTALL_STAGING_HEADERS
	echo "--------++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
endef

APP_BOOT_SELECTOR_POST_INSTALL_STAGING_HOOKS  += APP_BOOT_SELECTOR_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))

