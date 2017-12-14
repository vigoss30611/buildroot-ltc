################################################################################
#
# libmneon
#
################################################################################

LIBMNEON_VERSION = 1.0.0
LIBMNEON_SOURCE =
LIBMNEON_SITE  =

LIBMNEON_LICENSE =
LIBMNEON_LICENSE_FILES = README

LIBMNEON_MAINTAINED = YES
LIBMNEON_AUTORECONF = YES
LIBMNEON_INSTALL_STAGING = YES
LIBMNEON_DEPENDENCIES =  host-pkgconf

#install headers
define LIBMNEON_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	cp -rfv $(@D)/source/*.h  $(STAGING_DIR)/usr/include/
	mv $(STAGING_DIR)/usr/include/math_neon.h $(STAGING_DIR)/usr/include/mneon.h
	cp -rfv $(@D)/.libs/*.a $(STAGING_DIR)/usr/lib
	cp -rfv $(@D)/libmneon.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	echo "---------------------------------------"
endef
LIBMNEON_POST_INSTALL_STAGING_HOOKS  += LIBMNEON_POST_INSTALL_STAGING_HEADERS



$(eval $(autotools-package))

