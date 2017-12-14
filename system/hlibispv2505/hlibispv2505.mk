################################################################################
#
# ispddk
#
################################################################################

HLIBISPV2505_VERSION = 2.8.4
HLIBISPV2505_SOURCE = 
HLIBISPV2505_SITE  = 

HLIBISPV2505_LICENSE = 
HLIBISPV2505_LICENSE_FILES = README

HLIBISPV2505_MAINTAINED = NO
HLIBISPV2505_MAINTAINED_CMAKE = YES
HLIBISPV2505_AUTORECONF = NO
HLIBISPV2505_INSTALL_STAGING = YES
HLIBISPV2505_CONF_OPT = -DBUILD_DEMOS=ON
HLIBISPV2505_DEPENDENCIES = linux host-pkgconf hlibcamsensor

HLIBISPV2505_SUBDIR = DDKSource
HLIBISPV2505_CONF_ENV = 
HLIBISPV2505_CONF_OPT =  -DLINUX_KERNEL_BUILD_DIR=$(LINUX_BUILDDIR) \
    	-DCMAKE_INSTALL_PREFIX=$(TARGET_DIR)/usr/bin \
		-DCROSS_COMPILE=$(BR2_TOOLCHAIN_EXTERNAL_PATH)/bin/$(BR2_TOOLCHAIN_EXTERNAL_CUSTOM_PREFIX)-    	

ifeq ($(BR2_PACKAGE_LIBISPV2505_LANCHOU),y)
HLIBISPV2505_CONF_OPT += -DINFOTM_LANCHOU_PROJECT=yes
endif

ifeq ($(BR2_PACKAGE_LIBISPV2505_HW_AWB),y)
HLIBISPV2505_CONF_OPT += -DINFOTM_HW_AWB=yes
endif

ifeq ($(BR2_PACKAGE_LIBISPV2505_MNEON),y)
	HLIBISPV2505_DEPENDENCIES += libmneon
	HLIBISPV2505_CONF_OPT += -DINFOTM_MNEON_SUPPORT=yes
endif
HLIBISPV2505_MAKE = make
HLIBISPV2505_MAKE_ENV =
HLIBISPV2505_MAKE_OPT =
HLIBISPV2505_INSTALL_STAGING_OPT =
HLIBISPV2505_INSTALL_TARGET_OPT = install
#install headers
define HLIBISPV2505_POST_INSTALL_STAGING_HEADERS
	@echo "++++++++++++install staging target: libaray and include---------"	
	mkdir -p $(STAGING_DIR)/usr/include/hlibispv2505	
	cp -rfv $(@D)/DDKSource/out/lib/*.a $(STAGING_DIR)/usr/lib 
	cp -rfv $(@D)/$(HLIBISPV2505_SUBDIR)/ISP_Control/ISPC_lib/include/* $(STAGING_DIR)/usr/include/hlibispv2505
	cp -rfv $(@D)/$(HLIBISPV2505_SUBDIR)/common/img_includes/* $(STAGING_DIR)/usr/include/hlibispv2505
	cp -rfv $(@D)/$(HLIBISPV2505_SUBDIR)/CI/felix/felix_lib/kernel/include/* $(STAGING_DIR)/usr/include/hlibispv2505
	cp -rfv $(@D)/$(HLIBISPV2505_SUBDIR)/CI/felix/felix_lib/user/include/* $(STAGING_DIR)/usr/include/hlibispv2505	
	cp -rfv $(@D)/$(HLIBISPV2505_SUBDIR)/common/felixcommon/include/* $(STAGING_DIR)/usr/include/hlibispv2505
	cp -rfv $(@D)/$(HLIBISPV2505_SUBDIR)/CI/felix/regdefs2.7/vhc_out/* $(STAGING_DIR)/usr/include/hlibispv2505
	cp -rfv $(@D)/$(HLIBISPV2505_SUBDIR)/CI/felix/regdefs2.7/include/* $(STAGING_DIR)/usr/include/hlibispv2505
	cp -rfv $(@D)/$(HLIBISPV2505_SUBDIR)/sensorapi/include/* $(STAGING_DIR)/usr/include/hlibispv2505
	cp -rfv $(@D)/$(HLIBISPV2505_SUBDIR)/common/dyncmd/include/* $(STAGING_DIR)/usr/include/hlibispv2505	
	cp -rfv $(shell pwd)/system/hlibispv2505/hlibispv2505.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	
endef

HLIBISPV2505_POST_INSTALL_STAGING_HOOKS  += HLIBISPV2505_POST_INSTALL_STAGING_HEADERS

define HLIBISPV2505_POST_INSTALL_STAGING_MNEON_FIX
	sed -i s/ADDMNEONLIB\=/ADDMNEONLIB\=\-lmneon/g $(STAGING_DIR)/usr/lib/pkgconfig/hlibispv2505.pc
endef

ifeq ($(BR2_PACKAGE_LIBISPV2505_MNEON),y)
	HLIBISPV2505_POST_INSTALL_STAGING_HOOKS  += HLIBISPV2505_POST_INSTALL_STAGING_MNEON_FIX
endif

$(eval $(cmake-package))

