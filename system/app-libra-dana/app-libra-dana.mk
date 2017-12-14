################################################################################
#
# danap2p binary
#
################################################################################

APP_LIBRA_DANA_VERSION = 1.0.0
APP_LIBRA_DANA_SOURCE = 
APP_LIBRA_DANA_SITE  = 

APP_LIBRA_DANA_LICENSE = 
APP_LIBRA_DANA_LICENSE_FILES = README

APP_LIBRA_DANA_MAINTAINED = YES
APP_LIBRA_DANA_AUTORECONF = YES
APP_LIBRA_DANA_INSTALL_STAGING = YES
APP_LIBRA_DANA_DEPENDENCIES = videobox hlibfr audiobox libcodecs

#install headers
define APP_LIBRA_DANA_POST_INSTALL_STAGING_HEADERS
	echo "++++++++++++++install staging+++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/danap2p
	-cp -rfv $(@D)/include/*  $(STAGING_DIR)/usr/include/danap2p
	-cp -rfv $(@D)/danap2p.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef
APP_LIBRA_DANA_POST_INSTALL_STAGING_HOOKS  += APP_LIBRA_DANA_POST_INSTALL_STAGING_HEADERS


define APP_LIBRA_DANA_POST_INSTALL_TARGET_SHARED
	@echo "++++++++++++++++install target +++++++++++++++++++++++"
	mkdir -p $(TARGET_DIR)/opt/ipnc
	-cp -rfv $(@D)/danale.conf $(TARGET_DIR)/opt/ipnc/
	-cp -rfv $(@D)/S905libra_dana $(TARGET_DIR)/etc/init.d/
endef
APP_LIBRA_DANA_POST_INSTALL_TARGET_HOOKS  += APP_LIBRA_DANA_POST_INSTALL_TARGET_SHARED
$(eval $(autotools-package))

