AUTOMAKE_OPTIONS=foreign
sys_server_SOURCES= \
					sysserver/spv_wifi.c \
					sysserver/spv_utils.c \
					sysserver/spv_config_keys.c \
					sysserver/spv_server.c \
					sysserver/spv_properties.c \
					sysserver/spv_message.c \
					sysserver/spv_language_res.c \
					sysserver/spv_file_manager.c \
					sysserver/spv_file_receiver.c \
					sysserver/spv_launcher.c \
					sysserver/spv_event.c \
					sysserver/spv_indication.c \
					sysserver/spv_app.c \
					sysserver/spv_stream.c \
					sysserver/hwlib/camera_spv.c \
					sysserver/config_manager.c 

p2p_dana_SOURCES= \
				  p2p_dana/danaMain.c \
				  p2p_dana/base64.c

gui_SOURCES= \
					gui/gui.c \
					gui/spv_icon_res.c \
					gui/spv_info_dialog.c \
					gui/spv_item.c \
					gui/spv_scrollview.c \
					gui/spv_set_info_dialog.c \
					gui/spv_setup_dialog.c \
					gui/spv_static.c \
					gui/spv_toast.c

lib_common_SOURCES= \
					$(sys_server_SOURCES)

CUSTOM_CFLAGS=  -g -O0 \
				-I$(srcdir)/include/ -I$(srcdir)/sysserver/include/ \
				$(LIBITEMS_CFLAGS) \
				$(LIBVIDEOBOX_CFLAGS) \
				$(LIBAUDIOBOX_CFLAGS) \
				$(LIBAVCODEC_CFLAGS) \
				$(LIBMEDIAUTILS_CFLAGS) \
				${LIBQVPLAY_CFLAGS}\
				$(LIBFREETYPE_CFLAGS)

CUSTOM_LINKS=  -lpthread -lm \
		$(LIBITEMS_LIBS) \
		$(LIBVIDEOBOX_LIBS) \
		$(LIBAUDIOBOX_LIBS) \
		$(LIBAVCODEC_LIBS) \
		$(LIBSYSTEM_LIBS) \
		$(LIBWIFI_LIBS) \
		$(LIBFREETYPE_LIBS) \
		$(LIBMEDIAUTILS_LIBS) \
		${LIBQVPLAY_LIBS}\
		-ldemux


if HAVE_GUI
lib_common_SOURCES += \
					$(gui_SOURCES)
CUSTOM_CFLAGS += \
				-I$(srcdir)/gui/inc/ \
				$(LIBMINIGUI_CFLAGS)
CUSTOM_LINKS += \
				$(LIBMINIGUI_LIBS)
endif

if HAVE_UVC
lib_common_SOURCES += sysserver/hwlib/guvc.c
CUSTOM_CFLAGS += $(LIBGUVC_CFLAGS)
CUSTOM_LINKS += $(LIBGUVC_LIBS)
endif

if HAVE_P2P_DANA
lib_common_SOURCES += \
					  $(p2p_dana_SOURCES)
CUSTOM_CFLAGS += \
				 -I$(srcdir)/p2p_dana/inc/

launcher_LDADD= p2p_dana/lib/linux_arm_q3/libdana_video.a
#CUSTOM_LINKS += \
#				-L/home/zxqin/sd2/iot/dev_qsdk/system/app-launcher/p2p_dana/lib/linux_arm_q3/ \
#				-ldana_video
endif

if HAVE_PLAYBACK
lib_common_SOURCES += \
					sysserver/hwlib/media_play.c \
					sysserver/hwlib/jpeg_dec.c
CUSTOM_CFLAGS += \
				 $(LIBG1V6_CFLAGS)
CUSTOM_LINKS += \
				$(LIBG1V6_LIBS)
endif

SYSSERVER_CUSTOM_LINKS=  -lpthread \
		$(LIBITEMS_LIBS) \
		$(LIBVIDEOBOX_LIBS) \
		$(LIBAUDIOBOX_LIBS) \
		$(LIBAVCODEC_LIBS) \
		$(LIBG1V6_LIBS) \
		$(LIBSYSTEM_LIBS) \
		$(LIBWIFI_LIBS) \
		${LIBQVPLAY_LIBS}\
		${LIBDEMUX_LIBS}

##get launcher
bin_PROGRAMS=launcher upgrade pcbtest_server ota_upgrade
launcher_SOURCES=$(lib_common_SOURCES) 
launcher_CPPFLAGS = $(CUSTOM_CFLAGS)
launcher_LDFLAGS = $(CUSTOM_LINKS)

#launcher_LDADD= p2p_dana/lib/linux_arm_q3/libdana_video.a

upgrade_SOURCES= q360upgrade/upgrade.c
upgrade_CPPFLAGS = $(LIBUPGRADE_CFLAGS)
upgrade_LDFLAGS = \
		  $(LIBUPGRADE_LIBS) \
		  $(LIBSYSTEM_LIBS) \
		  $(LIBITEMS_LIBS)

ota_upgrade_SOURCES= q360upgrade/ota_upgrade.c
ota_upgrade_CPPFLAGS = $(LIBUPGRADE_CFLAGS)
ota_upgrade_LDFLAGS = \
		  $(LIBUPGRADE_LIBS) \
		  $(LIBSYSTEM_LIBS) \
		  $(LIBITEMS_LIBS) \
		  -lpthread

noinst_PROGRAMS=sysserver_api 
sysserver_api_SOURCES= sysserver/sysserver_api.c sysserver/sysserver_api_test.c
sysserver_api_CPPFLAGS = $(CUSTOM_CFLAGS)
sysserver_api_LDFLAGS = $(SYSSERVER_CUSTOM_LINKS)

pcbtest_server_SOURCES = sysserver/pcbtest_server.c
pcbtest_server_CPPFLAGS = $(CUSTOM_CFLAGS)
pcbtest_server_LDFLAGS = $(SYSSERVER_CUSTOM_LINKS)
