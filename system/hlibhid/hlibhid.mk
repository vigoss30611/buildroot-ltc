################################################################################
#
# hlibhid
#
################################################################################

HLIBHID_VERSION = 1.0.0
HLIBHID_SOURCE = 
HLIBHID_SITE  = 

HLIBHID_LICENSE = 
HLIBHID_LICENSE_FILES = README

HLIBHID_MAINTAINED = YES
HLIBHID_AUTORECONF = YES
HLIBHID_INSTALL_STAGING = YES
HLIBHID_DEPENDENCIES =  host-pkgconf 

#install headers
define HLIBHID_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/hlibhid
	cp -rfv $(@D)/include/hid.h  $(STAGING_DIR)/usr/include/hlibhid/
	cp -rfv $(@D)/hlibhid.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	echo "---------------------------------------"
endef
HLIBHID_POST_INSTALL_STAGING_HOOKS  += HLIBHID_POST_INSTALL_STAGING_HEADERS


$(eval $(autotools-package))
