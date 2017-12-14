################################################################################
#
# QLIBCRYPTO
#
################################################################################

QLIBCRYPTO_VERSION = 1.0.0
QLIBCRYPTO_SOURCE = 
QLIBCRYPTO_SITE  = 

QLIBCRYPTO_LICENSE = 
QLIBCRYPTO_LICENSE_FILES = README

QLIBCRYPTO_MAINTAINED = YES
QLIBCRYPTO_AUTORECONF = YES
QLIBCRYPTO_INSTALL_STAGING = YES
QLIBCRYPTO_DEPENDENCIES = host-pkgconf openssl

#install headers
define QLIBCRYPTO_POST_INSTALL_STAGING_HEADERS
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/crypto.h $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/crypto.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef
QLIBCRYPTO_POST_INSTALL_STAGING_HOOKS  += QLIBCRYPTO_POST_INSTALL_STAGING_HEADERS

QLIBCRYPTO_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_DIR) install

$(eval $(autotools-package))

