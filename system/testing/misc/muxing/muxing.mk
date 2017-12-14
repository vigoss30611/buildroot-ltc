################################################################################
#
# libosa
#
################################################################################

MUXING_VERSION = 1.0.0
MUXING_SOURCE = 
MUXING_SITE  = 

MUXING_LICENSE = 
MUXING_LICENSE_FILES = README

MUXING_MAINTAINED = YES
MUXING_AUTORECONF = YES
MUXING_INSTALL_STAGING = YES
MUXING_DEPENDENCIES = host-pkgconf  libffmpeg

#install headers
define MUXING_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/muxer
	cp -rfv $(@D)/*.h  $(STAGING_DIR)/usr/include/muxer
	cp -rfv $(@D)/muxer.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef
#MUXING_POST_INSTALL_STAGING_HOOKS  += MUXING_POST_INSTALL_STAGING_HEADERS



$(eval $(autotools-package))

