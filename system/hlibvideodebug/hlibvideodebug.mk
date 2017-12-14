################################################################################
#
# hlibvideodebug
#
################################################################################

HLIBVIDEODEBUG_VERSION = 1.0.0
HLIBVIDEODEBUG_SOURCE =
HLIBVIDEODEBUG_SITE  =

HLIBVIDEODEBUG_LICENSE =
HLIBVIDEODEBUG_LICENSE_FILES = README

HLIBVIDEODEBUG_MAINTAINED = YES
HLIBVIDEODEBUG_AUTORECONF = YES
HLIBVIDEODEBUG_INSTALL_STAGING = YES
HLIBVIDEODEBUG_DEPENDENCIES =  host-pkgconf videobox hlibfr

#install headers
define HLIBVIDEODEBUG_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/hlibvideodebug
	cp -rfv $(@D)/include/{video_debug.h,list.h}  $(STAGING_DIR)/usr/include/hlibvideodebug/
	cp -rfv $(@D)/hlibvideodebug.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	echo "---------------------------------------"
endef
HLIBVIDEODEBUG_POST_INSTALL_STAGING_HOOKS  += HLIBVIDEODEBUG_POST_INSTALL_STAGING_HEADERS


$(eval $(autotools-package))
