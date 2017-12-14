################################################################################
#
# APP_DEMOS
#
################################################################################

APP_DEMOS_VERSION = 1.0.0
APP_DEMOS_SOURCE =
APP_DEMOS_SITE  =

APP_DEMOS_LICENSE =
APP_DEMOS_LICENSE_FILES = README

APP_DEMOS_MAINTAINED = YES
APP_DEMOS_AUTORECONF = YES
APP_DEMOS_INSTALL_STAGING = YES
APP_DEMOS_MAKE = make -s
APP_DEMOS_DEPENDENCIES = host-pkgconf videobox eventhub hlibfr hlibh1v6 hlibguvc audiobox qlibvplay qlibwifi qlibsys libdemux

$(eval $(cmake-package))

