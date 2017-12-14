################################################################################
#
# qlibsys
#
################################################################################

QLIBSYS_VERSION = 1.0.0
QLIBSYS_SOURCE = 
QLIBSYS_SITE  = 

QLIBSYS_LICENSE = 
QLIBSYS_LICENSE_FILES = README

QLIBSYS_MAINTAINED = YES
QLIBSYS_AUTORECONF = YES
QLIBSYS_INSTALL_STAGING = YES
QLIBSYS_DEPENDENCIES = hlibitems libcurl cjson

#install headers
QLIBSYS_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_INITRD_DIR) install
define QLIBSYS_POST_INSTALL_STAGING_HEADERS
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/include/qsdk/*.h $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/qlibsys.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	cp -rfv $(@D)/.libs/*.so* $(TARGET_DIR)/usr/lib/
	cp -rfv $(TARGET_DIR)/usr/lib/libcJSON.so $(TARGET_INITRD_DIR)/usr/lib/
endef
QLIBSYS_POST_INSTALL_STAGING_HOOKS  += QLIBSYS_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))

