################################################################################
#
# DFBSee
#
################################################################################

DFBSEE_VERSION = 0.7.4
DFBSEE_SITE = http://directfb.org/downloads/Programs/
DFBSEE_SOURCE = DFBSee-$(DFBSEE_VERSION).tar.gz
DFBSEE_LICENSE = GPLv2
DFBSEE_LICENSE_FILES = COPYING
DFBSEE_AUTORECONF = YES

DFBSEE_DEPENDENCIES = directfb host-directfb

DFBSEE_CONF_ENV = \
	PKG_CONFIG_PATH=$(STAGING_DIR)/usr/lib/pkgconfig \
	PATH=$(HOST_DIR)/usr/bin:$(PATH)
#define DFBSEE_CONFIGURE_CMDS
#	cd $(@D);\
#	$(TARGET_CONFIGURE_ARGS) \
#	PATH=$(HOST_DIR)/usr/bin:$(PATH) \
#	PKG_CONFIG_PATH=$(STAGING_DIR)/usr/lib/pkgconfig \
#	$(DFBSEE_CONFDIR)/configure \
#		--target=$(GNU_TARGET_NAME) \
#		--host=$(GNU_TARGET_NAME) \
#		--build=$(GNU_HOST_NAME) \
#		--prefix=/usr \
#		--exec-prefix=/usr \
#		--sysconfdir=/etc \
#		--program-prefix="" \
#		$(DISABLE_DOCUMENTATION) \
#		$(DISABLE_NLS) \
#		$(DISABLE_LARGEFILE) \
#		$(DISABLE_IPV6) \
#		$(SHARED_STATIC_LIBS_OPTS) \
#		$(QUIET)
#endef

$(eval $(autotools-package))
