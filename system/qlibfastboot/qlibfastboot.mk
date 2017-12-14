################################################################################
#
# qlibfastboot
#
################################################################################

QLIBFASTBOOT_VERSION = 1.0.0
QLIBFASTBOOT_SOURCE = 
QLIBFASTBOOT_SITE  = 

QLIBFASTBOOT_LICENSE = 
QLIBFASTBOOT_LICENSE_FILES = README

QLIBFASTBOOT_MAINTAINED = YES
QLIBFASTBOOT_AUTORECONF = YES
QLIBFASTBOOT_INSTALL_STAGING = YES
QLIBFASTBOOT_DEPENDENCIES =  host-pkgconf 
QLIBFASTBOOT_DEPENDENCIES += qlibupgrade

#install headers
define QLIBFASTBOOT_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/qlibfastboot
	cp -rfv $(@D)/*.h  $(STAGING_DIR)/usr/include/qlibfastboot/
	cp -rfv $(@D)/qlibfastboot.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	echo "---------------------------------------"
endef
QLIBFASTBOOT_POST_INSTALL_STAGING_HOOKS  += QLIBFASTBOOT_POST_INSTALL_STAGING_HEADERS


$(eval $(autotools-package))
