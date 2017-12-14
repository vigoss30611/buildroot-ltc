################################################################################
#
# libosa
#
################################################################################

HLIBITEMS_VERSION = 1.0.0
HLIBITEMS_SOURCE = 
HLIBITEMS_SITE  = 

HLIBITEMS_LICENSE = 
HLIBITEMS_LICENSE_FILES = README

HLIBITEMS_MAINTAINED = YES
HLIBITEMS_AUTORECONF = YES
HLIBITEMS_INSTALL_STAGING = YES
HLIBITEMS_DEPENDENCIES = host-pkgconf 

#install headers
define HLIBITEMS_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/items.h  $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/items.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef
HLIBITEMS_POST_INSTALL_STAGING_HOOKS  += HLIBITEMS_POST_INSTALL_STAGING_HEADERS

HLIBITEMS_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_INITRD_DIR) install
define HLIBITEMS_POST_INSTALL_TARGET_SHARED
	@echo "++++++++++++++++install target +++++++++++++++++++++++"
		cp -rfv $(@D)/.libs/*.so* $(TARGET_DIR)/usr/lib/
endef
HLIBITEMS_POST_INSTALL_TARGET_HOOKS  += HLIBITEMS_POST_INSTALL_TARGET_SHARED

$(eval $(autotools-package))

