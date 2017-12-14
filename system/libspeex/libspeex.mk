################################################################################
#
# libspeex
#
################################################################################

LIBSPEEX_VERSION = 1.2beta2
LIBSPEEX_SOURCE = 
LIBSPEEX_SITE  = 

LIBSPEEX_LICENSE = 
LIBSPEEX_LICENSE_FILES = README

LIBSPEEX_MAINTAINED = YES
LIBSPEEX_AUTORECONF = YES
LIBSPEEX_INSTALL_STAGING = YES
LIBSPEEX_DEPENDENCIES = host-pkgconf 

LIBSPEEX_CONF_OPT = --with-ogg-libraries=$(STAGING_DIR)/usr/lib \
		 --with-ogg-includes=$(STAGING_DIR)/usr/include \
		 --enable-fixed-point

#install headers
define LIBSPEEX_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/speex
	cp -rfv $(@D)/include/speex/*.h  $(STAGING_DIR)/usr/include/speex
	#cp -rfv $(@D)/speex.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef
LIBSPEEX_POST_INSTALL_STAGING_HOOKS  += LIBSPEEX_POST_INSTALL_STAGING_HEADERS

define LIBSPEEX_LIBTOOL_FIXUP
	$(SED) 's|^hardcode_libdir_flag_spec=.*|hardcode_libdir_flag_spec=""|g' $(@D)/libtool
	$(SED) 's|^runpath_var=LD_RUN_PATH|runpath_var=DIE_RPATH_DIE|g' $(@D)/libtool
endef

LIBSPEEX_POST_CONFIGURE_HOOKS += LIBSPEEX_LIBTOOL_FIXUP

define LIBSPEEX_BUILD_CMDS
	$($(PKG)_MAKE_ENV) $(MAKE) $($(PKG)_MAKE_OPT) -C $(@D)/$($(PKG)_SUBDIR)
endef

$(eval $(autotools-package))
