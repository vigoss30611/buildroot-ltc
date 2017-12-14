################################################################################
#
# libfreetype
#
################################################################################

LIBFREETYPE_VERSION = 2.4.10
LIBFREETYPE_SOURCE = 
LIBFREETYPE_SITE = 




LIBFREETYPE_MAINTAINED = YES
LIBFREETYPE_AUTORECONF = NO
LIBFREETYPE_INSTALL_STAGING = YES
LIBFREETYPE_MAKE_OPT = CCexe="$(HOSTCC)"
LIBFREETYPE_LICENSE = Dual FTL/GPLv2+
LIBFREETYPE_LICENSE_FILES = docs/FTL.TXT docs/GPLv2.TXT
LIBFREETYPE_DEPENDENCIES = host-pkgconf
#LIBFREETYPE_CONFIG_SCRIPTS = libfreetype-config

HOST_LIBFREETYPE_DEPENDENCIES = host-pkgconf
#HOST_LIBFREETYPE_CONF_OPT = --without-zlib --without-bzip2 --without-png

ifeq ($(BR2_PACKAGE_ZLIB),y)
LIBFREETYPE_DEPENDENCIES += zlib
LIBFREETYPE_CONF_OPT += --with-zlib
else
LIBFREETYPE_CONF_OPT += --without-zlib
endif

ifeq ($(BR2_PACKAGE_BZIP2),y)
LIBFREETYPE_DEPENDENCIES += bzip2
LIBFREETYPE_CONF_OPT += --with-bzip2
else
LIBFREETYPE_CONF_OPT += --without-bzip2
endif

LIBFREETYPE_CONF_OPT += --without-png


$(eval $(autotools-package))
$(eval $(host-autotools-package))
