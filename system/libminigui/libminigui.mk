################################################################################
#
# libminigui
#
################################################################################

LIBMINIGUI_VERSION = 1.0.0
LIBMINIGUI_SOURCE = 
LIBMINIGUI_SITE  = 

LIBMINIGUI_LICENSE = 
LIBMINIGUI_LICENSE_FILES = README

LIBMINIGUI_MAINTAINED = YES
LIBMINIGUI_AUTORECONF = YES
LIBMINIGUI_INSTALL_STAGING = YES
LIBMINIGUI_DEPENDENCIES =  host-pkgconf libfreetype

LIBMINIGUI_MAKE_OPT += "CFLAGS=-g -D__linux -g -DHAVE_SYS_TIME_H -mfloat-abi=hard -mfpu=vfp"
LIBMINIGUI_CONF_OPT +=--with-ttfsupport=ft2 \
			--with-ft2-includes=$(STAGING_DIR)/usr/include/freetype2/ \
			--libdir=$(STAGING_DIR)/usr/lib/ \
			--enable-adv2dapi=yes    \
			--disable-splash         \
			--disable-screensaver    \
			--disable-flatlf         \
			--disable-skinlf         \
			--enable-fixedmath=yes   \
			--disable-debug          \
			--disable-dblclk         \
			--disable-cursor         \
			--disable-clipboard      \
			--disable-dlcustomial    \
			--disable-consoleial     \
			--disable-tslibial       \
			--enable-dummyial=yes    \
			--disable-consoleps2     \
			--disable-consoleimps2   \
			--disable-consolems      \
			--disable-consolems3     \
			--disable-textmode       \
			--enable-rbfsupport=yes  \
			--enable-rbfvgaoem=yes  \
			--disable-rbfterminal    \
			--disable-rbffixedsys    \
			--disable-vbfsupport     \
			--disable-fontsserif     \
			--disable-fontcourier    \
			--disable-fontsystem     \
			--disable-upfsupport     \
			--disable-fonttimes      \
			--disable-bmpfsupport    \
			--disable-latin2support  \
			--disable-latin3support  \
			--disable-latin4support  \
			--disable-cyrillicsupport \
			--disable-arabicsupport  \
			--disable-greeksupport   \
			--disable-hebrewsupport  \
			--disable-latin5support  \
			--disable-latin6support  \
			--disable-thaisupport    \
			--disable-latin7support  \
			--disable-latin8support  \
			--disable-latin9support  \
			--disable-latin10support \
			--disable-gbsupport      \
			--disable-gbksupport     \
			--disable-big5support    \
			--disable-euckrsupport   \
			--disable-eucjpsupport   \
			--disable-shiftjissupport\
			--disable-unicodesupport \
			--disable-savebitmap     \
			--enable-gifsupport=yes     \
			--disable-jpgsupport     \
			--disable-pngsupport     \
			--disable-menu           \
			--disable-mousecalibrate \
			--disable-aboutdlg       \
			--disable-savescreen     \
			--enable-ctrlstatic=yes  \
			--enable-ctrlbutton=yes  \
			--disable-ctrlsledit     \
			--disable-ctrlbidisledit \
			--disable-ctrllistbox    \
			--disable-ctrlpgbar      \
			--disable-ctrlnewtoolbar \
			--disable-ctrlmenubtn    \
			--disable-ctrltrackbar   \
			--disable-ctrlcombobox   \
			--disable-ctrlpropsheet  \
			--enable-ctrlscrollview=yes  \
			--disable-ctrlmonthcal   \
			--disable-ctrltreeview   \
			--disable-ctrlspinbox    \
			--disable-ctrlcoolbar    \
			--disable-ctrllistview   \
			--disable-ctrliconview   \
			--disable-ctrlanimation  \
			--enable-ctrlscrollbar=yes   \
			--disable-newtextedit    \
			--disable-videodummy     \
			--enable-videofbcon=yes  \
			--disable-videoqvfb      \
			--disable-pcxvfb

#install headers
define LIBMINIGUI_POST_INSTALL_STAGING_HEADERS
	echo "+++++++++++++++install staging++++++++++++++++++++++++"
	mkdir -p $(STAGING_DIR)/usr/include/minigui
	mkdir -p $(STAGING_DIR)/usr/include/minigui/control
	cp -rfv $(@D)/*.h  $(STAGING_DIR)/usr/include/minigui
	cp -rfv $(@D)/include/*  $(STAGING_DIR)/usr/include/minigui
	cp -rfv $(@D)/src/include/*  $(STAGING_DIR)/usr/include/minigui
	cp -rfv $(@D)/libminigui.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	echo "???????????????????add private header files ,junk work around"
	
	cp -rfv $(@D)/src/control/*.h  $(STAGING_DIR)/usr/include/minigui/control
endef
LIBMINIGUI_POST_INSTALL_STAGING_HOOKS  += LIBMINIGUI_POST_INSTALL_STAGING_HEADERS


define LIBMINIGUI_POST_INSTALL_TARGET_SHARED
	echo "++++++++++++++++install target +++++++++++++++++++++++"
	cp -rfv $(@D)/minigui.pc  $(STAGING_DIR)/usr/lib/pkgconfig/
	cp -rfv $(@D)/src/.libs/libminigui_ths*so*			  $(TARGET_DIR)/usr/lib/
	cp -rfv $(@D)/etc/MiniGUI.cfg $(TARGET_DIR)/etc
	-rm -rfv $(TARGET_DIR)/usr/lib/libminigui*.a
	-rm -rfv $(TARGET_DIR)/usr/lib/libminigui*.la
	-rm -rfv $(TARGET_DIR)/home/*
endef
LIBMINIGUI_POST_INSTALL_TARGET_HOOKS  += LIBMINIGUI_POST_INSTALL_TARGET_SHARED
$(eval $(autotools-package))
$(eval $(host-autotools-package))
