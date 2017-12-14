################################################################################
#
# libosa
#
################################################################################

APP_LIBRA_MAIN_VERSION = 1.0.0
APP_LIBRA_MAIN_SOURCE = 
APP_LIBRA_MAIN_SITE  = 

APP_LIBRA_MAIN_LICENSE = 
APP_LIBRA_MAIN_LICENSE_FILES = README

APP_LIBRA_MAIN_MAINTAINED = YES
APP_LIBRA_MAIN_AUTORECONF = YES
APP_LIBRA_MAIN_INSTALL_STAGING = YES
APP_LIBRA_MAIN_DEPENDENCIES =  libminigui hlibitems videobox qlibvplay audiobox libdemux libcodecs hlibg1v6


APP_LIBRA_MAIN_EXTRA_CFLAGS =
ifeq ($(BR2_INFOTM_BOARD_PATTERN), "sportdv")
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_SPORTDV_ENABLE
endif
ifeq ($(BR2_INFOTM_GUI_STYLE), "large")
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_LARGE_SCREEN
endif
ifeq ($(BR2_INFOTM_GUI_SHOW_STATUS_BAR), y)
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_SHOW_STATUS_BAR
endif
ifeq ($(BR2_INFOTM_LCD_ORIENTATION), "portrait")
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_ROTATE_SCREEN
endif
ifeq ($(BR2_INFOTM_RESOLUTION_SWITCH_MODULE), "isp")
        APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_SET_RESOLUTION_SEPARATELY
endif
ifeq ($(BR2_INFOTM_CAMERA_REAR_ENABLE), y)
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_CAMERA_REAR_ENABLE
endif
ifeq ($(BR2_INFOTM_CAMERA_PIP_ENABLE), y)
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_CAMERA_PIP_ENABLE
endif
ifeq ($(BR2_INFOTM_WIFI_ENABLE), y)
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_WIFI_ENABLE
endif
ifeq ($(BR2_INFOTM_WIFI_MODEL), "esp8089")
        APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_WIFI_ESP8089
endif
ifeq ($(BR2_INFOTM_SOUND_OUTPUT), "buzzer")
        APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_SOUND_BUZZER
endif
ifeq ($(BR2_INFOTM_WIFI_MODEL), "rtl8189es")
        APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_WIFI_8189ES
endif
ifeq ($(BR2_INFOTM_GPS_ENABLE), y)
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_GPS_ENABLE
endif
ifeq ($(BR2_INFOTM_ADAS_ENABLE), y)
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_ADAS_ENABLE
endif
ifeq ($(BR2_INFOTM_GUI_TIMER_RESET), y)
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_TIMER_RESET
endif
ifeq ($(BR2_INFOTM_GUI_AUTO_PLAY_ENABLE), y)
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_AUTO_PLAY
endif
ifeq ($(BR2_INFOTM_GUI_DEBUG_ENABLE), y)
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_DEBUG_ENABLE
endif
ifeq ($(BR2_INFOTM_VIDEO_WIFI_PREVIEW), y)
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_WIFI_PREVIEW
endif
ifeq ($(BR2_INFOTM_EVENT_HTTP_ENABLE), y)
	APP_LIBRA_MAIN_EXTRA_CFLAGS += -DGUI_EVENT_HTTP_ENABLE
endif

APP_LIBRA_MAIN_CONF_ENV = CFLAGS+="$(APP_LIBRA_MAIN_EXTRA_CFLAGS)"


#install headers
define APP_LIBRA_MAIN_POST_INSTALL_STAGING_HEADERS
	echo "++++++++++++++install staging+++++++++++++++++++++++++"
#	mkdir -p $(STAGING_DIR)/usr/include/minigui
#	cp -rfv $(@D)/include/*  $(STAGING_DIR)/usr/include/minigui
endef
APP_LIBRA_MAIN_POST_INSTALL_STAGING_HOOKS  += APP_LIBRA_MAIN_POST_INSTALL_STAGING_HEADERS


define APP_LIBRA_MAIN_POST_INSTALL_TARGET_SHARED
	echo "++++++++++++++++install target +++++++++++++++++++++++"
	-mkdir -p $(TARGET_DIR)/root/libra
	-mkdir -p $(TARGET_DIR)/etc/profile.d
	-mkdir -p $(TARGET_DIR)/mnt/sd0/DCIM/100CVR/
	cp -rfv $(@D)/res  $(TARGET_DIR)/root/libra
	cp -rfv $(@D)/config  $(TARGET_DIR)/root/libra
	cp -rfv $(@D)/wifi  $(TARGET_DIR)/root/libra
	cp -rfv $(@D)/S904spv_gui  $(TARGET_DIR)/etc/init.d/
	cp -rfv $(@D)/envsetup.sh  $(TARGET_DIR)/etc/profile.d/
	cp -rfv $(@D)/tools/mkfs.fat  $(TARGET_DIR)/usr/bin/
	cp -rfv $(@D)/tools/sd_format.sh  $(TARGET_DIR)/usr/bin/
endef
APP_LIBRA_MAIN_POST_INSTALL_TARGET_HOOKS  += APP_LIBRA_MAIN_POST_INSTALL_TARGET_SHARED
$(eval $(autotools-package))

