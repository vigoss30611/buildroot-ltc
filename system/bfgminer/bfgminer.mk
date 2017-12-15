BFGMINER_VERSION = 5.4.2
BFGMINER_SOURCE =
BFGMINER_SITE  =

BFGMINER_LICENSE =
BFGMINER_LICENSE_FILES = README

BFGMINER_MAINTAINED = YES
BFGMINER_AUTORECONF = NO
BFGMINER_CONF_OPT = --enable-scrypt --enable-zeusminer CFLAGS="-I./uthash/src"
BFGMINER_INSTALL_STAGING = YES
BFGMINER_DEPENDENCIES = host-pkgconf

#define BFGMINER_CONFIGURE_CMDS
#	@echo "##########################"
#	@echo "configure bfgminer"
#	(cd $(@D) && \
#	 $(TARGET_CONFIGURE_OPTS) \
#	 $(TARGET_CONFIGURE_ARGS) \
#	 ./autogen.sh && \
#	 ./configure --prefix=/usr \
#	 --enable-scrypt \
#	 --enable-zeusminer)
#endef

define BFGMINER_PRE_CONFIGURE_FIXUP
	cd $(CGMINER_SRCDIR) && ./autogen.sh
endef

BFGMINER_PER_CONFIGURE_HOOKS += BFGMINER_PRE_CONFIGURE_FIXUP
BFGMINER_POST_INSTALL_STAGING_HOOKS  += BFGMINER_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))
