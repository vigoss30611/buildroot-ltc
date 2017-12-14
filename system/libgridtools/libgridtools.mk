################################################################################
#
#
#
################################################################################

LIBGRIDTOOLS_VERSION = 1.0.0
LIBGRIDTOOLS_SOURCE =
LIBGRIDTOOLS_SITE  =
LIBGRIDTOOLS_LICENSE =
LIBGRIDTOOLS_LICENSE_FILES = README

LIBGRIDTOOLS_MAINTAINED = YES
LIBGRIDTOOLS_AUTORECONF = YES
LIBGRIDTOOLS_INSTALL_STAGING = YES
LIBGRIDTOOLS_DEPENDENCIES =

LIBGRIDTOOLS_DEPENDENCIES += libmneon



#install headers
define LIBGRIDTOOLS_POST_INSTALL_STAGING_HEADERS
	@echo "staging install header & static"
	@mkdir -p $(STAGING_DIR)/usr/include/qsdk
	@cp -rfv $(@D)/lib/inc/libgridtools.h $(STAGING_DIR)/usr/include/qsdk/
	@cp -rfv $(@D)/lib/libgridtools.a $(STAGING_DIR)/usr/lib/
	@cp -rfv $(@D)/libgridtools.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef
#install headers
define LIBGRIDTOOLS_POST_INSTALL_STAGING_HEADERS2
	@echo "staging install shared"
	@mkdir -p $(STAGING_DIR)/usr/include/qsdk
	@cp -rfv $(@D)/lib/inc/libgridtools.h $(STAGING_DIR)/usr/include/qsdk/
	@cp -rfv $(@D)/lib/libgridtools.so $(STAGING_DIR)/usr/lib/
endef
LIBGRIDTOOLS_POST_INSTALL_STAGING_HOOKS  += LIBGRIDTOOLS_POST_INSTALL_STAGING_HEADERS


define LIBGRIDTOOLS_POST_INSTALL_TARGET_HOOK
	@echo "target install shared"
	@cp -rfv $(@D)/lib/libgridtools.so $(TARGET_DIR)/usr/lib/
endef


ifeq ($(BR2_PACKAGE_LIBGRIDTOOLS_SHARED),y)
	LIBGRIDTOOLS_POST_INSTALL_STAGING_HOOKS  += LIBGRIDTOOLS_POST_INSTALL_STAGING_HEADERS2
	LIBGRIDTOOLS_POST_INSTALL_TARGET_HOOKS  += LIBGRIDTOOLS_POST_INSTALL_TARGET_HOOK
else
	LIBGRIDTOOLS_POST_INSTALL_STAGING_HOOKS  += LIBGRIDTOOLS_POST_INSTALL_STAGING_HEADERS
endif
$(eval $(autotools-package))

