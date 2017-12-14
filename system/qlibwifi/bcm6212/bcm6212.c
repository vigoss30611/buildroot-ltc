#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <lib_wifi.h>
#include <qsdk/items.h>

static int bcm6212_driver_loaded(struct wifi_hal_t *wifi_hal, int mode) 
{
    FILE *proc;
    char line[sizeof(wifi_hal->driver_module_name)+10];

    if ((proc = fopen(MODULE_FILE, "r")) == NULL) {
        printf("Could not open %s: %s", MODULE_FILE, strerror(errno));
        return 0;
    }
    while ((fgets(line, sizeof(line), proc)) != NULL) {
        if (strncmp(line, wifi_hal->driver_module_name, strlen(wifi_hal->driver_module_name)) == 0) {
            fclose(proc);
            return 1;
        }
    }
    fclose(proc);
    return 0;
}

static int bcm6212_load_driver(struct wifi_hal_t *wifi_hal, int mode)
{
	char bcm6212_module_arg[256];
	char cmd[256];

	if (mode == AP) {
		if (item_exist("wifi.model") && item_equal("wifi.model", "bcmdhd6210", 1))//bcmdhd6210
			sprintf(bcm6212_module_arg, "firmware_path=/wifi/firmware/fw_bcm40181a2_apsta.bin nvram_path=/wifi/firmware/nvram_ap6210.txt iface_name=wlan0");
		else if (item_exist("wifi.model") && item_equal("wifi.model", "bcmdhd6212", 1))//bcmdhd6212 and bcmdhd6212a
			sprintf(bcm6212_module_arg, "firmware_path=/wifi/firmware/fw_bcm43438a0_apsta.bin nvram_path=/wifi/firmware/nvram_ap6212.txt iface_name=wlan0");
        	else if (item_exist("wifi.model") && item_equal("wifi.model", "bcmdhd6214", 1))//bcmdhd6214
			sprintf(bcm6212_module_arg, "firmware_path=/wifi/firmware/fw_bcm43438a0_apsta.bin nvram_path=/wifi/firmware/nvram_ap6214.txt iface_name=wlan0");
              else if (item_exist("wifi.model") && item_equal("wifi.model", "bcmdhd6236", 1))//bcmdhd6236
			sprintf(bcm6212_module_arg, "firmware_path=/wifi/firmware/fw_bcm43436b0_apsta.bin nvram_path=/wifi/firmware/nvram_ap6236.txt iface_name=wlan0");
              else{
                    printf("%s called,wifi don't support",__func__);
    	             return -1;
              }
                    
	} else {
		if (item_exist("wifi.model") && item_equal("wifi.model", "bcmdhd6210", 1))
			sprintf(bcm6212_module_arg, "firmware_path=/wifi/firmware/fw_bcm40181a2.bin nvram_path=/wifi/firmware/nvram_ap6210.txt iface_name=wlan0");
		else if (item_exist("wifi.model") && item_equal("wifi.model", "bcmdhd6212", 1))//bcmdhd6212 and bcmdhd6212a
			sprintf(bcm6212_module_arg, "firmware_path=/wifi/firmware/fw_bcm43438a0.bin nvram_path=/wifi/firmware/nvram_ap6212.txt iface_name=wlan0");
        	else if (item_exist("wifi.model") && item_equal("wifi.model", "bcmdhd6214", 1))//bcmdhd6214
			sprintf(bcm6212_module_arg, "firmware_path=/wifi/firmware/fw_bcm43438a0.bin nvram_path=/wifi/firmware/nvram_ap6214.txt iface_name=wlan0");
             else if (item_exist("wifi.model") && item_equal("wifi.model", "bcmdhd6236", 1))//bcmdhd6236
			sprintf(bcm6212_module_arg, "firmware_path=/wifi/firmware/fw_bcm43436b0.bin nvram_path=/wifi/firmware/nvram_ap6236.txt iface_name=wlan0");
             else{
                    printf("%s called,wifi don't support",__func__);
    	             return -1;
              }
	}

	if (access(wifi_hal->driver_module_path, F_OK) == 0) {
	    if (insmod(wifi_hal->driver_module_path, bcm6212_module_arg) < 0) {
			printf("%s called,insmod driver failed",__func__);
	        return -1;
	    }
	} else {
		sprintf(cmd, "modprobe %s %s", wifi_hal->driver_module_name, bcm6212_module_arg);
		system(cmd);
	}

	return 0;
}

static int bcm6212_unload_driver(struct wifi_hal_t *wifi_hal, int mode)
{
	int count = 20;

    if (rmmod(wifi_hal->driver_module_name) == 0) {
        while (count-- > 0) {
            if (!wifi_hal->is_driver_loaded(wifi_hal, mode))
                break;
            usleep(500000);
        }
        usleep(500000);
        if (count)
            return 0;
    }

	return -1;
}

void wifi_hal_init(struct wifi_hal_t *wifi_hal)
{
	snprintf(wifi_hal->driver_module_name, sizeof(wifi_hal->driver_module_name), "bcmdhd");
	snprintf(wifi_hal->driver_module_path, sizeof(wifi_hal->driver_module_path), "/wifi/bcmdhd.ko");
	snprintf(wifi_hal->iface_name, sizeof(wifi_hal->iface_name), "wlan0");
	snprintf(wifi_hal->ap_iface_name, sizeof(wifi_hal->ap_iface_name), "wlan0");
	snprintf(wifi_hal->supplicant_path, sizeof(wifi_hal->supplicant_path), "wpa_supplicant");
	snprintf(wifi_hal->hostapd_path, sizeof(wifi_hal->hostapd_path), "hostapd");
	snprintf(wifi_hal->apdns_path, sizeof(wifi_hal->apdns_path), "dnsmasq");
	wifi_hal->load_driver = bcm6212_load_driver;
	wifi_hal->unload_driver = bcm6212_unload_driver;
	wifi_hal->is_driver_loaded = bcm6212_driver_loaded;

	return;
}

