################################################################################
#
#
#
################################################################################

LIBH264BITSTREAM_VERSION = 0.1.9
LIBH264BITSTREAM_SOURCE =
LIBH264BITSTREAM_SITE  =

LIBH264BITSTREAM_LICENSE =
LIBH264BITSTREAM_LICENSE_FILES = README

LIBH264BITSTREAM_MAINTAINED = YES
LIBH264BITSTREAM_AUTORECONF = YES
LIBH264BITSTREAM_INSTALL_STAGING = YES
LIBH264BITSTREAM_DEPENDENCIES = host-pkgconf


$(eval $(autotools-package))

