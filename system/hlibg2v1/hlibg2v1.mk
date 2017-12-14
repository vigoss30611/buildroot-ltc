################################################################################
#
# libhlibg2v1
#
################################################################################

HLIBG2V1_VERSION = 1.0.0
HLIBG2V1_SOURCE = 
HLIBG2V1_SITE  = 

HLIBG2V1_LICENSE = 
HLIBG2V1_LICENSE_FILES = README

HLIBG2V1_MAINTAINED = YES
HLIBG2V1_AUTORECONF = YES
HLIBG2V1_INSTALL_STAGING = YES
HLIBG2V1_DEPENDENCIES =  host-pkgconf 

#install headers
define HLIBG2V1_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/hlibg2v1/software
	mkdir -p $(STAGING_DIR)/usr/include/hlibg2v1/software/source/inc
	mkdir -p $(STAGING_DIR)/usr/include/hlibg2v1/software/source/config
	mkdir -p $(STAGING_DIR)/usr/include/hlibg2v1/software/source/common
	mkdir -p $(STAGING_DIR)/usr/include/hlibg2v1/software/source/hevc
	mkdir -p $(STAGING_DIR)/usr/include/hlibg2v1/software/linux/dwl
	mkdir -p $(STAGING_DIR)/usr/include/hlibg2v1/software/test/common
	mkdir -p $(STAGING_DIR)/usr/include/hlibg2v1/software/test/common/swhw
	#normal add 
	cp -rfv $(@D)/software/source/inc/*  $(STAGING_DIR)/usr/include/hlibg2v1
	cp -rfv $(@D)/software/source/hevc/*.h  $(STAGING_DIR)/usr/include/hlibg2v1
	cp -rfv $(@D)/software/source/common/*.h  $(STAGING_DIR)/usr/include/hlibg2v1
	cp -rfv $(@D)/software/source/config/*.h  $(STAGING_DIR)/usr/include/hlibg2v1
	#special add
	cp -rfv $(@D)/software/source/inc/*  $(STAGING_DIR)/usr/include/hlibg2v1/software/source/inc
	cp -rfv $(@D)/software/source/config/*  $(STAGING_DIR)/usr/include/hlibg2v1/software/source/config/
	cp -rfv $(@D)/software/source/common/*.h  $(STAGING_DIR)/usr/include/hlibg2v1/software/source/common/
	cp -rfv $(@D)/software/source/hevc/*.h  $(STAGING_DIR)/usr/include/hlibg2v1/software/source/hevc/
	cp -rfv $(@D)/software/linux/dwl/*.h  $(STAGING_DIR)/usr/include/hlibg2v1/software/linux/dwl
	cp -rfv $(@D)/software/test/common/*.h  $(STAGING_DIR)/usr/include/hlibg2v1/software/test/common/
	cp -rfv $(@D)/software/test/common/swhw/*.h  $(STAGING_DIR)/usr/include/hlibg2v1/software/test/common/swhw
	#cp -rfv $(@D)/*.h  $(STAGING_DIR)/usr/include/hlibg2v1
	cp -rfv $(@D)/hlibg2v1.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	echo "---------------------------------------"
endef
HLIBG2V1_POST_INSTALL_STAGING_HOOKS  += HLIBG2V1_POST_INSTALL_STAGING_HEADERS



$(eval $(autotools-package))

