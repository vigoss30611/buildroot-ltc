################################################################################
#
# SmartRC moudule 
# 
################################################################################

QLIBSMARTRC_VERSION = 1.0.0
QLIBSMARTRC_SOURCE =
QLIBSMARTRC_SITE  =

QLIBSMARTRC_LICENSE =
QLIBSMARTRC_LICENSE_FILES = README

QLIBSMARTRC_MAINTAINED = YES
QLIBSMARTRC_AUTORECONF = YES
QLIBSMARTRC_INSTALL_STAGING = YES
QLIBSMARTRC_DEPENDENCIES = host-pkgconf videobox 


#install headers
QLIBSMARTRC_POST_INSTALL_STAGING_HOOKS  += QLIBSMARTRC_POST_INSTALL_STAGING_HEADERS

$(eval $(cmake-package))

