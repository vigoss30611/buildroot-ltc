################################################################################
#
# tutk
#
################################################################################

APP_D304_MAIN_VERSION = 1.0.0
APP_D304_MAIN_SOURCE = 
APP_D304_MAIN_SITE  = 

APP_D304_MAIN_LICENSE = 
APP_D304_MAIN_LICENSE_FILES = README

APP_D304_MAIN_MAINTAINED = YES
APP_D304_MAIN_AUTORECONF = YES
APP_D304_MAIN_INSTALL_STAGING = YES
APP_D304_MAIN_DEPENDENCIES = host-pkgconf hlibitems hlibfr qlibwifi qlibsys eventhub audiobox libcodecs videobox libdemux qlibvplay cep

APP_D304_MAIN_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_DIR)  install

define APP_D304_MAIN_POST_INSTALL_TARGET_SHARED
	echo "++++++++++++++++install target +++++++++++++++++++++++"
	mkdir -p $(TARGET_DIR)/root
	mkdir -p $(TARGET_DIR)/root/qiwo
	mkdir -p $(TARGET_DIR)/root/qiwo/wav
	cp -rfv $(@D)/S904tutk $(TARGET_DIR)/etc/init.d/
	cp -rfv $(@D)/res/*.wav $(TARGET_DIR)/root/qiwo/wav/
endef
APP_D304_MAIN_POST_INSTALL_TARGET_HOOKS  += APP_D304_MAIN_POST_INSTALL_TARGET_SHARED
$(eval $(autotools-package))

