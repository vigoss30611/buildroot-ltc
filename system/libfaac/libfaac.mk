################################################################################
#
# libosa
#
################################################################################

LIBFAAC_VERSION = 1.28
LIBFAAC_SOURCE = 
LIBFAAC_SITE  = 

LIBFAAC_LICENSE = 
LIBFAAC_LICENSE_FILES = README

LIBFAAC_MAINTAINED = YES
LIBFAAC_AUTORECONF = NO
LIBFAAC_INSTALL_STAGING = YES
LIBFAAC_DEPENDENCIES = host-pkgconf 

#install headers
define LIBFAAC_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/faac
	cp -rfv $(@D)/include/*.h  $(STAGING_DIR)/usr/include/faac
	cp -rfv $(@D)/faac.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef
LIBFAAC_POST_INSTALL_STAGING_HOOKS  += LIBFAAC_POST_INSTALL_STAGING_HEADERS



$(eval $(autotools-package))

