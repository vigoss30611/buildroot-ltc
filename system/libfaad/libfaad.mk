################################################################################
#
# libosa
#
################################################################################

LIBFAAD_VERSION = 2.7.0
LIBFAAD_SOURCE = 
LIBFAAD_SITE  = 

LIBFAAD_LICENSE = 
LIBFAAD_LICENSE_FILES = README

LIBFAAD_MAINTAINED = YES
LIBFAAD_AUTORECONF = YES
LIBFAAD_INSTALL_STAGING = YES
LIBFAAD_DEPENDENCIES = host-pkgconf 

#install headers
define LIBFAAD_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/faad
	cp -rfv $(@D)/include/*.h  $(STAGING_DIR)/usr/include/faad
	cp -rfv $(@D)/faad.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef
LIBFAAD_POST_INSTALL_STAGING_HOOKS  += LIBFAAD_POST_INSTALL_STAGING_HEADERS



$(eval $(autotools-package))

