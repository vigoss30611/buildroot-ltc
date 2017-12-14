################################################################################
#
# TESTING
#
################################################################################

TESTING_VERSION = 1.0.0
TESTING_SOURCE = 
TESTING_SITE  = 

TESTING_LICENSE = 
TESTING_LICENSE_FILES = README

TESTING_MAINTAINED = YES
TESTING_AUTORECONF = YES
TESTING_INSTALL_STAGING = YES
TESTING_DEPENDENCIES = host-pkgconf videobox hlibitems qlibwifi eventhub hlibfr libffmpeg audiobox \
					   libcodecs hlibvideodebug qlibupgrade librtsp

ifeq ($(BR2_PACKAGE_TESTING_TESTBENCH_G1V6),y)
        TESTING_DEPENDENCIES += hlibg1v6
endif

ifeq ($(BR2_PACKAGE_TESTING_TESTBENCH_H1V6),y)
        TESTING_DEPENDENCIES += hlibh1v6
endif

ifeq ($(BR2_PACKAGE_TESTING_TESTBENCH_H2V1),y)
        TESTING_DEPENDENCIES += hlibh2v1
endif

ifeq ($(BR2_PACKAGE_TESTING_TESTBENCH_JENC),y)
        TESTING_DEPENDENCIES += hlibjenc
endif

ifeq ($(BR2_PACKAGE_TESTING_TESTBENCH_G2V1),y)
        TESTING_DEPENDENCIES += hlibg2v1
endif

#install headers
TESTING_POST_INSTALL_STAGING_HOOKS  += TESTING_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))

