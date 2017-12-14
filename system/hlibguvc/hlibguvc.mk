################################################################################
#
# hlibguvc
#
################################################################################

HLIBGUVC_VERSION = 1.0.0
HLIBGUVC_SOURCE = 
HLIBGUVC_SITE  = 

HLIBGUVC_LICENSE = 
HLIBGUVC_LICENSE_FILES = README

HLIBGUVC_MAINTAINED = YES
HLIBGUVC_AUTORECONF = YES
HLIBGUVC_INSTALL_STAGING = YES
HLIBGUVC_DEPENDENCIES =  host-pkgconf videobox hlibfr

#install headers
define HLIBGUVC_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/hlibguvc
	cp -rfv $(@D)/include/{uvc.h,ch9.h,video.h,videodev2.h,guvc.h,UvcH264.h}  $(STAGING_DIR)/usr/include/hlibguvc/
	cp -rfv $(@D)/hlibguvc.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	echo "---------------------------------------"
endef
HLIBGUVC_POST_INSTALL_STAGING_HOOKS  += HLIBGUVC_POST_INSTALL_STAGING_HEADERS


$(eval $(autotools-package))
