# libcodecs

LIBCODECS_VERSION = 1.0.0
LIBCODECS_SOURCE = 
LIBCODECS_SITE  = 

LIBCODECS_LICENSE = 
LIBCODECS_LICENSE_FILES = README

LIBCODECS_MAINTAINED = YES
LIBCODECS_AUTORECONF = YES
LIBCODECS_INSTALL_STAGING = YES

ifeq ($(BR2_PACKAGE_LIBCODECS_DSP_SUPPORT),y)
LIBCODECS_CONF_OPT += --enable-dspsupport
LIBCODECS_DEPENDENCIES = host-pkgconf libffmpeg hlibdsp
else
LIBCODECS_DEPENDENCIES = host-pkgconf libffmpeg
endif

# install headers
define LIBCODECS_POST_INSTALL_STAGING_HEADERS
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	cp -rfv $(@D)/include/codecs.h  $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/libcodecs.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef

LIBCODECS_POST_INSTALL_STAGING_HOOKS  += LIBCODECS_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))

