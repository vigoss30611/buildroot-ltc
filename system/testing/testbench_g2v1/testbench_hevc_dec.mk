################################################################################
#
# testbench_hevc_dec
#
################################################################################

TESTBENCH_HEVC_DEC_VERSION = 1.0.0
TESTBENCH_HEVC_DEC_SOURCE = 
TESTBENCH_HEVC_DEC_SITE  = 

TESTBENCH_HEVC_DEC_LICENSE = 
TESTBENCH_HEVC_DEC_LICENSE_FILES = README

TESTBENCH_HEVC_DEC_MAINTAINED = YES
TESTBENCH_HEVC_DEC_AUTORECONF = YES
TESTBENCH_HEVC_DEC_INSTALL_STAGING = YES
TESTBENCH_HEVC_DEC_DEPENDENCIES =  host-pkgconf  hlibg2v1




$(eval $(autotools-package))

