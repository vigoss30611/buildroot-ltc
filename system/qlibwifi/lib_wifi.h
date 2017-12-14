#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include "WPA/utils/wpa_ctrl.h"

#define AP 		0
#define STA 	1
#define MONITOR 	2
#define PRIMARY     0
#define SECONDARY   1
#define MAX_CONNS   2

static const char MODULE_FILE[]         = "/proc/modules";
static const char IFACE_DIR[]           = "/tmp/wpa_supplicant";
static const char CONTROL_IFACE_PATH[]  = "/config/wifi/sockets";
static const char HOSTAPD_CONF_FILE[]    = "/config/wifi/hostapd.conf";
static const char APDNS_CONF_FILE[]    = "/config/wifi/dnsmasq.conf";
#define WIFI_ENTROPY_FILE        "/config/wifi/entropy.bin"
#define CONFIG_CTRL_IFACE_CLIENT_PREFIX "wpa_ctrl_"
static const char SUPP_ENTROPY_FILE[]   = WIFI_ENTROPY_FILE;

struct wifi_hal_t;

extern int init_module(void *, unsigned long, const char *);
extern int delete_module(const char *, unsigned int);
extern int insmod(const char *filename, const char *args);
extern void *load_file(const char *fn, unsigned *_sz);
extern int rmmod(const char *modname);
extern void wifi_set_if_down(int mode);
extern int ifc_init(void);
extern void ifc_close(void);
extern int ifc_down(const char *name);
extern int ifc_up(const char *name);
extern void wifi_hal_init(struct wifi_hal_t *wifi_hal);

void wifi_normal_init(void);
void wifi_normal_deinit(void);
int wifi_load_driver(int mode, char *hwaddr);
int wifi_unload_driver(int mode);
int wifi_start_dhcpcd(int mode, int timeout);
void wifi_stop_dhcpcd(int mode);
int wifi_wait_for_event(int mode, char *buf, size_t buflen);
int wifi_start_supplicant(int p2p_supported);
int wifi_stop_supplicant(int p2p_supported);
int wifi_connect_to_supplicant(int mode);
int wifi_ap_start_dns(void);
int wifi_ap_stop_dns(void);
int wifi_ap_start_hostapd(void);
int wifi_ap_stop_hostapd(void);
int wifi_get_config(char *interface, char *ssid, char *security, char *passphrase, int *channel);
int wifi_set_config(char *interface, char *ssid, char *security, char *passphrase, int channel);
int wifi_command(int mode, const char *command, char *reply, size_t *reply_len);
int do_dhcp_request(uint32_t *ipaddr, uint32_t *gateway, uint32_t *mask,
                    uint32_t *dns1, uint32_t *dns2, uint32_t *server, uint32_t *lease);

struct wifi_hal_t {
	int stop_dhcp;
	char driver_module_name[16];
	char driver_module_path[64];
	char iface_name[16];
	char ap_iface_name[16];
	char supplicant_path[32];
	char hostapd_path[32];
	char apdns_path[32];
	int (*load_driver)(struct wifi_hal_t *wifi_hal, int mode);
	int (*unload_driver)(struct wifi_hal_t *wifi_hal, int mode);
	int (*is_driver_loaded)(struct wifi_hal_t *wifi_hal, int mode);
};

