################################################################################
#
# hlibg1v6
#
################################################################################

HLIBG1V6_VERSION = 1.0.0
HLIBG1V6_SOURCE = 
HLIBG1V6_SITE  = 

HLIBG1V6_LICENSE = 
HLIBG1V6_LICENSE_FILES = README

HLIBG1V6_MAINTAINED = YES
HLIBG1V6_AUTORECONF = YES
HLIBG1V6_INSTALL_STAGING = YES
HLIBG1V6_DEPENDENCIES =  host-pkgconf

#install headers
define HLIBG1V6_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/inc/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/jpeg/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/vp6/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/avs/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/mpeg4/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/config/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/common/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/pp/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/mpeg2/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/vp8/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/vc1/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/h264high/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/source/rv/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/rvds/jpeg/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/rvds/mpeg4/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/rvds/pp/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/rvds/mpeg2/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/rvds/h264high/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/rvds/dwl/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/rvds/rv/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/linux/jpeg/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/linux/ldriver/kernel_26x/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/linux/memalloc/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/linux/mpeg4/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/linux/pp/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/linux/mpeg2/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/linux/h264high/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/linux/dwl/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/linux/rv/*.h  $(STAGING_DIR)/usr/include/hlibg1v6

	cp -rfv $(@D)/source/h264high/h264decapi.h  $(STAGING_DIR)/usr/include/hlibg1v6
	#cp -rfv $(@D)/*.h  $(STAGING_DIR)/usr/include/hlibg1v6
	cp -rfv $(@D)/hlibg1v6.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	echo "---------------------------------------"
endef
HLIBG1V6_POST_INSTALL_STAGING_HOOKS  += HLIBG1V6_POST_INSTALL_STAGING_HEADERS



$(eval $(autotools-package))

