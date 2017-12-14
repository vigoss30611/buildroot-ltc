# libfr

HLIBFR_VERSION = 1.0.0
HLIBFR_SOURCE = 
HLIBFR_SITE  = 

HLIBFR_LICENSE = 
HLIBFR_LICENSE_FILES = README

HLIBFR_MAINTAINED = YES
HLIBFR_AUTORECONF = YES
HLIBFR_INSTALL_STAGING = YES
HLIBFR_DEPENDENCIES = host-pkgconf
ifeq ($(BR2_FR_CC_BUFFER), y)
HLIBFR_CONF_OPT += --enable-cc_buffer
endif

ifeq ($(BR2_FR_CC_WRITETHROUGH), y)
HLIBFR_CONF_OPT += --enable-cc_writethrough
endif

ifeq ($(BR2_FR_CC_WRITEBACK), y)
HLIBFR_CONF_OPT += --enable-cc_writeback
endif

ifeq ($(BR2_FR_MMAP_ONLY_ONE), y)
HLIBFR_CONF_OPT += --enable-new_mmap
endif

# install headers
define HLIBFR_POST_INSTALL_STAGING_HEADERS
	mkdir -p $(STAGING_DIR)/usr/include/fr
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	cp -rfv $(@D)/include/fr/*.h  $(STAGING_DIR)/usr/include/fr
	cp -rfv $(@D)/fr.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef

HLIBFR_POST_INSTALL_STAGING_HOOKS  += HLIBFR_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))

