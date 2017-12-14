AUTOMAKE_OPTIONS=foreign
COMMON_SOURCES= gui.c \
					spv_static.c \
					minigui_need_api.c \
					spv_icon_res.c \
					spv_language_res.c \
					spv_setup_dialog.c \
					config_manager.c \
					spv_item.c \
					properties.c \
					spv_scrollview.c \
					spv_wav_res.c \
					spv_toast.c \
					spv_set_info_dialog.c \
					spv_info_dialog.c

CUSTOM_CFLAGS=   -I$(srcdir)  -g -O0  -I./include \
				$(LIBMINIGUI_CFLAGS)  



CUSTON_LINKS=  -lpthread \
		$(LIBMINIGUI_LIBS)  


##get q360gui
bin_PROGRAMS=q360gui
q360gui_SOURCES=$(COMMON_SOURCES) 
#q360gui_CPPFLAGS = $(CUSTOM_CFLAGS)
q360gui_CFLAGS = $(CUSTOM_CFLAGS)
q360gui_LDFLAGS = $(CUSTON_LINKS)
