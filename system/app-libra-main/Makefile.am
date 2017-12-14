AUTOMAKE_OPTIONS=foreign
lib_common_SOURCES=config_manager.c  spv_browser_folder.c  spv_item.c     \
					spv_playback_dialog.c  spv_static.c\
					properties.c      spv_folder_dialog.c   spv_language_res.c   \
					spv_scrollview.c       spv_toast.c\
					spv_adas.c        spv_icon_res.c        \
					spv_main.c           spv_set_info_dialog.c  \
					spv_utils.c spv_app.c         spv_info_dialog.c     \
					spv_media_scanner.c  spv_setup_dialog.c     spv_wav_res.c \
					wifi_softap.c \
					hwlib/api.c \
					hwlib/camera_spv.c \
					hwlib/media_play.c \
					hwlib/jpeg_dec.c

CUSTOM_CFLAGS=   -I$(srcdir)  -g -O0  -I./hwlib/include -DGUI_SMALL_SYSTEM_ENABLE\
				$(LIBMINIGUI_CFLAGS) \
				$(LIBITEMS_CFLAGS) \
				$(LIBVIDEOBOX_CFLAGS) \
				$(LIBAUDIOBOX_CFLAGS) \
				$(LIBAVCODEC_CFLAGS) \
				$(LIBG1V6_CFLAGS)



CUSTON_LINKS=  -lpthread \
		$(LIBMINIGUI_LIBS) \
		$(LIBITEMS_LIBS) \
		$(LIBVIDEOBOX_LIBS) \
		$(LIBAUDIOBOX_LIBS) \
		$(LIBAVCODEC_LIBS) \
		$(LIBG1V6_LIBS) \
		-lqvplay


##get libramain
bin_PROGRAMS=libramain
libramain_SOURCES=$(lib_common_SOURCES) 
libramain_CPPFLAGS = $(CUSTOM_CFLAGS)
libramain_LDFLAGS = $(CUSTON_LINKS)
