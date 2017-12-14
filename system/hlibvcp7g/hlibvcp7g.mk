################################################################################
#
# hlibvcp7g
#
################################################################################

HLIBVCP7G_VERSION = 1.0.0
HLIBVCP7G_SOURCE = 
HLIBVCP7G_SITE  = 

TEST_HLIBVCP7G_LICENSE = 
HLIBVCP7G_LICENSE_FILES = 

HLIBVCP7G_MAINTAINED = YES
HLIBVCP7G_AUTORECONF = YES
HLIBVCP7G_INSTALL_STAGING = YES

ifeq ($(BR2_PACKAGE_HLIBVCP7G_DSP_SUPPORT),y)
HLIBVCP7G_CONF_OPT += --enable-dspsupport
DSP_EXTRA_DEPENDENCIES = hlibdsp
define DSP_EXTRA_INSTALL 
	cp -rfv $(@D)/libvcp7g_dsp.pc  $(STAGING_DIR)/usr/lib/pkgconfig/libvcp7g.pc
endef
#define DSP_EXTRA_INSTALL 
#	mkdir -p $(TARGET_DIR)/dsp/
#	cp -rfv $(@D)/firmware $(TARGET_DIR)/dsp/
#endef
else
HLIBVCP7G_CONF_OPT += --disable-dspsupport
DSP_EXTRA_DEPENDENCIES = 
endif

ifeq ($(BR2_PACKAGE_HLIBVCP7G_ARM_SUPPORT),y)
HLIBVCP7G_CONF_OPT += --enable-armsupport
ARM_EXTRA_DEPENDENCIES = 
define ARM_EXTRA_INSTALL 
	cp -rfv $(@D)/lib/libvcp.a  $(STAGING_DIR)/usr/lib/
	cp -rfv $(@D)/libvcp7g_arm.pc  $(STAGING_DIR)/usr/lib/pkgconfig/libvcp7g.pc
endef
else
HLIBVCP7G_CONF_OPT += --disable-armsupport
ARM_EXTRA_DEPENDENCIES = 
endif


ifeq ($(BR2_PACKAGE_HLIBVCP7G_ARM_SUPPORT),y)
ifeq ($(BR2_PACKAGE_HLIBVCP7G_DSP_SUPPORT),y)
define EXTRA_INSTALL 
	cp -rfv $(@D)/libvcp7g_arm_dsp.pc  $(STAGING_DIR)/usr/lib/pkgconfig/libvcp7g.pc
endef
endif
endif


HLIBVCP7G_DEPENDENCIES = host-pkgconf libcodecs $(DSP_EXTRA_DEPENDENCIES) $(ARM_EXTRA_DEPENDENCIES)

HLIBVCP7G_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_DIR)  install

define HLIBVCP7G_POST_INSTALL_TARGET_HEADER
		mkdir -p $(STAGING_DIR)/usr/include/qsdk
		mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
		cp -rfv $(@D)/include/vcp7g/vcp7g.h  $(STAGING_DIR)/usr/include/qsdk/
		cp -rfv $(@D)/libvcp7g.pc  $(STAGING_DIR)/usr/lib/pkgconfig/libvcp7g.pc
endef

#HLIBVCP7G_POST_INSTALL_TARGET_SHARED += DSP_EXTRA_INSTALL
#HLIBVCP7G_POST_INSTALL_TARGET_SHARED += ARM_EXTRA_INSTALL
#HLIBVCP7G_POST_INSTALL_TARGET_SHARED += EXTRA_INSTALL

HLIBVCP7G_POST_INSTALL_TARGET_HOOKS  += HLIBVCP7G_POST_INSTALL_TARGET_HEADER
HLIBVCP7G_POST_INSTALL_TARGET_HOOKS  += DSP_EXTRA_INSTALL
HLIBVCP7G_POST_INSTALL_TARGET_HOOKS  += ARM_EXTRA_INSTALL
HLIBVCP7G_POST_INSTALL_TARGET_HOOKS  += EXTRA_INSTALL

$(eval $(autotools-package))

