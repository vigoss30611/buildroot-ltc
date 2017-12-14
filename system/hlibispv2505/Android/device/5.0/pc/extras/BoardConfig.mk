ifeq ($(BOARD_WIFI),Wifi)
 include device/img/pc/extras/BoardConfigWifi.mk
else ifeq ($(BOARD_WIFI),WiredWifi)
 include device/img/pc/extras/BoardConfigWiredWifi.mk
endif

BOARD_USES_GENERIC_AUDIO := false
