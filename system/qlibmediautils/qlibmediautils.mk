################################################################################
#
#
#
################################################################################

QLIBMEDIAUTILS_VERSION = 1.0.0
QLIBMEDIAUTILS_SOURCE =
QLIBMEDIAUTILS_SITE  =
QLIBMEDIAUTILS_LICENSE =
QLIBMEDIAUTILS_LICENSE_FILES = README

QLIBMEDIAUTILS_MAINTAINED = YES
QLIBMEDIAUTILS_AUTORECONF = YES
QLIBMEDIAUTILS_INSTALL_STAGING = YES
ifeq ($(CONFIG_QLIBMEDIAUTILS_SWSCALE),y)
QLIBMEDIAUTILS_DEPENDENCIES += libffmpeg
QLIBMEDIAUTILS_CONF_OPT += --enable-swscale
QLIBMEDIAUTILS_HEADERS+=jpegutils.h
QLIBMEDIAUTILS_LINKS+=-lavformat -lavutil -lswscale
endif

ifeq ($(CONFIG_QLIBMEDIAUTILS_SWCROP),y)
QLIBMEDIAUTILS_DEPENDENCIES += libffmpeg
QLIBMEDIAUTILS_CONF_OPT += --enable-swcrop
QLIBMEDIAUTILS_HEADERS+=jpegutils.h
QLIBMEDIAUTILS_LINKS+=-lavformat -lavutil -lavfilter 
endif

ifeq ($(CONFIG_QLIBMEDIAUTILS_CALIBRATION),y)
QLIBMEDIAUTILS_DEPENDENCIES += libexif
QLIBMEDIAUTILS_CONF_OPT += --enable-calibration
QLIBMEDIAUTILS_LINKS+=-lexif
QLIBMEDIAUTILS_HEADERS+=jpegexif.h
endif

ifeq ($(CONFIG_QLIBMEDIAUTILS_TESTING),y)
QLIBMEDIAUTILS_CONF_OPT += --enable-testing
endif
#install headers
define QLIBMEDIAUTILS_POST_INSTALL_STAGING_HEADERS
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	cp -rfv $(@D)/dummy_include/*.h $(STAGING_DIR)/usr/include/qsdk
	@echo "copy real api file"
	headers=`echo $QLIBMEDIAUTILS_HEADERS`
	$(foreach header, $(QLIBMEDIAUTILS_HEADERS), 
		@cp -rfv $(@D)/include/$(header)  $(STAGING_DIR)/usr/include/qsdk 
	)
	echo "link libs -> "$(QLIBMEDIAUTILS_LINKS)
	#sed -i 's/LINKS/$(QLIBMEDIAUTILS_LINKS)/g' $(@D)/qlibmediautils.pc
	sed -i 's/LINKS\=/LINKS\=$(QLIBMEDIAUTILS_LINKS)/g' $(@D)/qlibmediautils.pc
	cp -rfv $(@D)/qlibmediautils.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
endef
QLIBMEDIAUTILS_POST_INSTALL_STAGING_HOOKS  += QLIBMEDIAUTILS_POST_INSTALL_STAGING_HEADERS

$(eval $(autotools-package))
