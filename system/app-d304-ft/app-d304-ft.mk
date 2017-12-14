################################################################################
#
# d304ft
#
################################################################################

APP_D304_FT_VERSION = 1.0.0
APP_D304_FT_SOURCE =
APP_D304_FT_SITE  =

APP_D304_FT_LICENSE =
APP_D304_FT_LICENSE_FILES = README

APP_D304_FT_MAINTAINED = YES
APP_D304_FT_AUTORECONF = YES
APP_D304_FT_INSTALL_STAGING = YES
APP_D304_FT_DEPENDENCIES = host-pkgconf hlibitems hlibfr qlibwifi qlibsys eventhub audiobox libcodecs videobox libdemux qlibvplay

APP_D304_FT_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_DIR)  install

define APP_D304_FT_POST_INSTALL_TARGET_SHARED
	echo "++++++++++++++++install target +++++++++++++++++++++++"
	cp -rfv $(@D)/S905ft $(TARGET_DIR)/etc/init.d/
endef
APP_D304_FT_POST_INSTALL_TARGET_HOOKS  += APP_D304_FT_POST_INSTALL_TARGET_SHARED
$(eval $(autotools-package))

