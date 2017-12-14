################################################################################
#
#IQ Test
#
################################################################################

APP_ITTD_VERSION = 1.0.0
APP_ITTD_SOURCE = 
APP_ITTD_SITE  = 

APP_ITTD_LICENSE = 
APP_ITTD_LICENSE_FILES = README

APP_ITTD_MAINTAINED = YES
APP_ITTD_AUTORECONF = YES
APP_ITTD_INSTALL_STAGING = YES
APP_ITTD_DEPENDENCIES = host-pkgconf librtsp

#install headers
define APP_ITTD_POST_INSTALL_STAGING_HEADERS
	echo "--------++++++++++++++++++++"
	cp -rfv $(@D)/ittd $(TARGET_DIR)/usr/bin/
endef

APP_ITTD_POST_INSTALL_STAGING_HOOKS  += APP_ITTD_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))

