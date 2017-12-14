################################################################################
#
# Fisheye calibration tool
#
################################################################################

FISHEYE_K_TOOL_VERSION = 1.0.0
FISHEYE_K_TOOL_SOURCE =
FISHEYE_K_TOOL_SITE  =

FISHEYE_K_TOOL_LICENSE =
FISHEYE_K_TOOL_LICENSE_FILES = README

FISHEYE_K_TOOL_MAINTAINED = YES
FISHEYE_K_TOOL_AUTORECONF = YES
FISHEYE_K_TOOL_INSTALL_STAGING = YES
FISHEYE_K_TOOL_DEPENDENCIES = host-pkgconf videobox hlibfr libgridtools

FISHEYE_K_TOOL_DEPENDENCIES += libmneon

#install headers
FISHEYE_K_TOOL_POST_INSTALL_STAGING_HOOKS  += FISHEYE_K_TOOL_POST_INSTALL_STAGING_HEADERS

$(eval $(cmake-package))

