################################################################################
#
# live555
#
################################################################################

LIBRTSP_VERSION = 2016.08.07
LIBRTSP_SOURCE =
LIBRTSP_SITE = $(TOPDIR)/system/librtsp/
LIBRTSP_SITE_METHOD = local
LIBRTSP_INSTALL_STAGING = YES
LIBRTSP_DEPENDENCIES = videobox audiobox hlibfr libcodecs libdemux eventhub

define LIBRTSP_CONFIGURE_CMDS
	echo 'COMPILE_OPTS = -I$(STAGING_DIR)/usr/local/include $$(INCLUDES) -I. -DSOCKLEN_T=socklen_t -D__STDC_CONSTANT_MACROS $(TARGET_CFLAGS)' >> $(@D)/config.linux #-DDEBUG
	echo 'C_COMPILER = $(TARGET_CC)' >> $(@D)/config.linux
	echo 'CPLUSPLUS_COMPILER = $(TARGET_CXX)' >> $(@D)/config.linux
	echo 'LINK = $(TARGET_CXX) -o' >> $(@D)/config.linux
	echo 'LINK_OPTS = -L. $(TARGET_LDFLAGS) -lfr -lvideobox -laudiobox -lcodecs -ldemux -levent' >> $(@D)/config.linux
	(cd $(@D); ./genMakefiles linux)
endef

define LIBRTSP_BUILD_CMDS
	$(MAKE) -C $(@D) all
endef

LIBRTSP_HEADERS_TO_INSTALL = rtspServerLib/rtsp_server.h

LIBRTSP_LIBS_TO_INSTALL = rtspServerLib/librtsp.so

#install staging
define LIBRTSP_INSTALL_STAGING_HEADERS_AND_SHARED_LIBRARY
	@echo "++++++++++++++++install staging++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/qsdk
	for i in $(LIBRTSP_HEADERS_TO_INSTALL); do \
		cp -rfv $(@D)/$$i $(STAGING_DIR)/usr/include/qsdk/`basename $$i`; \
	done; \
	for i in $(LIBRTSP_LIBS_TO_INSTALL); do \
		$(INSTALL) -D -m 0755 $(@D)/$$i $(STAGING_DIR)/usr/lib/`basename $$i`; \
	done
endef
LIBRTSP_POST_INSTALL_STAGING_HOOKS  += LIBRTSP_INSTALL_STAGING_HEADERS_AND_SHARED_LIBRARY

LIBRTSP_INSTALL_TARGET_OPT = DESTDIR=$(TARGET_INITRD_DIR) install
define LIBRTSP_POST_INSTALL_TARGET_SHARED
	@echo "++++++++++++++++install target +++++++++++++++++++++++"
	for i in $(LIBRTSP_LIBS_TO_INSTALL); do \
		cp -rfv $(@D)/$$i $(TARGET_DIR)/usr/lib/`basename $$i`; \
	done
endef
LIBRTSP_POST_INSTALL_TARGET_HOOKS  += LIBRTSP_POST_INSTALL_TARGET_SHARED

$(eval $(generic-package))
