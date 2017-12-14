################################################################################
#
# qlibcertif
#
################################################################################

QLIBCERTIF_VERSION = 1.0.0
QLIBCERTIF_SOURCE = 
QLIBCERTIF_SITE  = 

QLIBCERTIF_LICENSE = 
QLIBCERTIF_LICENSE_FILES = README

QLIBCERTIF_MAINTAINED = YES
QLIBCERTIF_AUTORECONF = YES
QLIBCERTIF_INSTALL_STAGING = YES
QLIBCERTIF_DEPENDENCIES = host-pkgconf openssl
ifeq ($(BR2_PACKAGE_QLIBCERTIF_ENABLE_TEST),y)
QLIBCERTIF_DEPENDENCIES += qlibsys
endif

ifeq ($(BR2_PACKAGE_QLIBCERTIF_ENABLE_TEST),y)
QLIBCERTIF_CONF_OPT += --enable-certiftest
endif

#install headers
QLIBCERTIF_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_DIR) install
define QLIBCERTIF_POST_INSTALL_STAGING_HEADERS
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/include/qsdk/*.h $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/qlibcertif.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	cp -rfv $(@D)/.libs/*.so* $(TARGET_DIR)/usr/lib/
endef
QLIBCERTIF_POST_INSTALL_STAGING_HOOKS  += QLIBCERTIF_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))

