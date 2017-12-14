################################################################################
#
# upgrade
#
################################################################################

QLIBUPGRADE_VERSION = 1.0.0
QLIBUPGRADE_SOURCE = 
QLIBUPGRADE_SITE  = 

QLIBUPGRADE_LICENSE = 
QLIBUPGRADE_LICENSE_FILES = README

QLIBUPGRADE_MAINTAINED = YES
QLIBUPGRADE_AUTORECONF = YES
QLIBUPGRADE_INSTALL_STAGING = YES
QLIBUPGRADE_DEPENDENCIES = hlibitems libcurl qlibsys zlib

define QLIBUPGRADE_POST_INSTALL_STAGING_HEADERS
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/upgrade.h  $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/qlibupgrade.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef

QLIBUPGRADE_POST_INSTALL_STAGING_HOOKS  += QLIBUPGRADE_POST_INSTALL_STAGING_HEADERS

QLIBUPGRADE_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_INITRD_DIR) install
define QLIBUPGRADE_POST_INSTALL_TARGET_SHARED
		-rm -rf $(TARGET_INITRD_DIR)/usr/lib/*.la
		cp -rfv $(@D)/.libs/*.so* $(TARGET_DIR)/usr/lib/
		cp -rfv $(TARGET_DIR)/usr/lib/libz* $(TARGET_INITRD_DIR)/usr/lib/
endef
QLIBUPGRADE_POST_INSTALL_TARGET_HOOKS  += QLIBUPGRADE_POST_INSTALL_TARGET_SHARED

define BINQWUPGRADE
	rm -rf $(TARGET_INITRD_DIR)/usr/bin/upgrade
endef

ifneq ($(BR2_PACKAGE_BINUPGRADE),y)
	QLIBUPGRADE_POST_INSTALL_TARGET_HOOKS += BINQWUPGRADE
endif

$(eval $(autotools-package))

