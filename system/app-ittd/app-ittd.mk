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
APP_ITTD_MAKE = make -s
APP_ITTD_DEPENDENCIES = host-pkgconf hlibfr librtsp


ifeq ($(BR2_PACKAGE_APP_ITTD_V4L2_ONLY), y)
	APP_ITTD_CONF_OPT += -DITTD_V4L2_ONLY=yes
endif

#install headers
define APP_ITTD_POST_INSTALL_STAGING_HEADERS
	cp -rfv $(@D)/app-ittd $(TARGET_DIR)/usr/bin/app-ittd
endef

APP_ITTD_POST_INSTALL_STAGING_HOOKS  += APP_ITTD_POST_INSTALL_STAGING_HEADERS

$(eval $(cmake-package))

