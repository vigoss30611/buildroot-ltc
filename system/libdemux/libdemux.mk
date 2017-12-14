################################################################################
#
# LIBDEMUX
#
################################################################################

LIBDEMUX_VERSION = 1.0.0
LIBDEMUX_SOURCE = 
LIBDEMUX_SITE  = 

LIBDEMUX_LICENSE = 
LIBDEMUX_LICENSE_FILES = README

LIBDEMUX_MAINTAINED = YES
LIBDEMUX_AUTORECONF = YES
LIBDEMUX_INSTALL_STAGING = YES
LIBDEMUX_DEPENDENCIES = host-pkgconf libffmpeg

#install headers
define LIBDEMUX_POST_INSTALL_STAGING_HEADERS
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/demux.h $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/demux.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef
LIBDEMUX_POST_INSTALL_STAGING_HOOKS  += LIBDEMUX_POST_INSTALL_STAGING_HEADERS

LIBDEMUX_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_DIR) install

$(eval $(autotools-package))

