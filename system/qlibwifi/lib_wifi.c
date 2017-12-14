#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/vfs.h>
#include <unistd.h>
#include <poll.h>
#include <ctype.h>
#include <libintl.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include <qsdk/sys.h>
#include <qsdk/items.h>
#include "WPA/utils/wpa_ctrl.h"
#include "lib_wifi.h"

#define DNS_PATH		"etc/resolv.conf"
extern int ifc_get_hwaddr(const char *name, void *ptr);
extern int do_dhcp(char *iname);
extern void get_dhcp_info(uint32_t *ipaddr, uint32_t *gateway, uint32_t *prefixLength,
                   uint32_t *dns1, uint32_t *dns2, uint32_t *server, uint32_t *lease);
extern int ifc_add_address(const char *name, const char *address, int prefixlen);
extern int ifc_set_prefixLength(const char *name, int prefixLength);
extern int ifc_add_route(const char *ifname, const char *dst, int prefix_length, const char *gw);
extern int ifc_get_default_route(const char *ifname);
extern int ifc_set_default_route(const char *ifname, in_addr_t gateway);
extern int ifc_get_addr(const char *name, in_addr_t *addr);
extern int ifc_get_info(const char *name, in_addr_t *addr, int *prefixLength, unsigned *flags);
extern in_addr_t prefixLengthToIpv4Netmask(int prefix_length);

static struct wifi_hal_t *normal_wifi_hal = NULL;;
static int exit_sockets[MAX_CONNS][2];
static struct wpa_ctrl *ctrl_conn[MAX_CONNS];
static struct wpa_ctrl *monitor_conn[MAX_CONNS];
static const char primary_iface[]  = "wlan0";
static const char SUPP_CONFIG_TEMPLATE[]= "/config/wifi/wpa_supplicant.conf";
static const char SUPP_CONFIG_FILE[]    = "/config/wifi/wpa_supplicant.conf";
static const char P2P_SUPPLICANT_NAME[] = "p2p_supplicant";
static const char P2P_CONFIG_FILE[]     = "/config/wifi/p2p_supplicant.conf";
static const char SUPPLICANT_NAME[]     = "wpa_supplicant";
static char supplicant_name[64];

static unsigned char dummy_key[21] = { 0x02, 0x11, 0xbe, 0x33, 0x43, 0x35,
                                       0x68, 0x47, 0x84, 0x99, 0xa9, 0x2b,
                                       0x1c, 0xd3, 0xee, 0xff, 0xf1, 0xe2,
                                       0xf3, 0xf4, 0xf5 };

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(exp) ({         \
    typeof (exp) _rc;                      \
    do {                                   \
        _rc = (exp);                       \
    } while (_rc == -1 && errno == EINTR); \
    _rc; })
#endif

static int is_primary_interface(const char *ifname)
{
    if (ifname == NULL || !strncmp(ifname, primary_iface, strlen(primary_iface))) {
        return 1;
    }
    return 0;
}

void *load_file(const char *fn, unsigned *_sz)
{
    char *data;
    int sz;
    int fd;

    data = 0;
    fd = open(fn, O_RDONLY);
    if(fd < 0) return 0;

    sz = lseek(fd, 0, SEEK_END);
    if(sz < 0) goto oops;

    if(lseek(fd, 0, SEEK_SET) != 0) goto oops;

    data = (char*) malloc(sz + 1);
    if(data == 0) goto oops;

    if(read(fd, data, sz) != sz) goto oops;
    close(fd);
    data[sz] = 0;

    if(_sz) *_sz = sz;
    return data;

oops:
    close(fd);
    if(data != 0) free(data);
    return 0;
}

int insmod(const char *filename, const char *args)
{
    void *module;
    unsigned int size;
    int ret;

    module = load_file(filename, &size);
    if (!module)
        return -1;

    ret = init_module(module, size, args);

    free(module);

    return ret;
}

int rmmod(const char *modname)
{
    int ret = -1;
    int maxtry = 10;

    while (maxtry-- > 0) {
        ret = delete_module(modname, O_NONBLOCK | O_EXCL);
        if (ret < 0 && errno == EAGAIN)
            usleep(500000);
        else
            break;
    }

    if (ret != 0)
        printf("Unable to unload driver module %s: \n", modname);
    return ret;
}

void wifi_ap_generatepsk(char *ssid, char *passphrase, char *psk_str)
{
    unsigned char psk[SHA256_DIGEST_LENGTH];
    int j;

    PKCS5_PBKDF2_HMAC_SHA1(passphrase, strlen(passphrase), (const unsigned char *)(ssid), strlen(ssid),
            4096, SHA256_DIGEST_LENGTH, psk);

    for (j=0; j < SHA256_DIGEST_LENGTH; j++) {
        sprintf(&psk_str[j*2], "%02x", psk[j]);
    }
}

int is_wifi_driver_loaded(int mode) 
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
    FILE *proc;
    char line[sizeof(wifi_hal->driver_module_name)+10];

    if ((proc = fopen(MODULE_FILE, "r")) == NULL) {
        printf("Could not open %s: %s \n", MODULE_FILE, strerror(errno));
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

void wifi_set_if_up(int mode)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;

	if (ifc_init() < 0)
		return ;

	if (mode == STA) {
		if (ifc_up(wifi_hal->iface_name)) {
			printf("failed to bring up interface %s: %s\n", wifi_hal->iface_name, strerror(errno));
			return;
		}
	} else {
		if (ifc_up(wifi_hal->ap_iface_name)) {
			printf("failed to bring up interface %s: %s\n", wifi_hal->iface_name, strerror(errno));
			return;
		}
	}
	ifc_close();

	return;
}

void wifi_set_if_down(int mode)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;

	if (ifc_init() < 0)
		return ;

	if (ifc_down(wifi_hal->iface_name)) {
		printf("failed to bring up interface %s: %s\n", wifi_hal->iface_name, strerror(errno));
		return;
	}
	ifc_close();

	return;
}

int detectwifiifnamefromproc(char *local_ifname)
{
    char linebuf[1024];
    FILE *f = fopen("/proc/net/wireless", "r");

    local_ifname[0] = '\0';
    if (f) {
		while(fgets(linebuf, sizeof(linebuf)-1, f)) {
	
			if (strchr(linebuf, ':')) {
				char *dest = local_ifname;
				char *p = linebuf;
				while(*p && isspace(*p))
					++p;
				while (*p && *p != ':') {
					*dest++ = *p++;
				}
				*dest = '\0';
				printf("DetectWifiIfNameFromProc: %s\n", local_ifname);
				fclose(f);
				return 0;
			}
		}
		fclose(f);
    }
    return 1;
}

void wifi_normal_init(void)
{
	if (normal_wifi_hal == NULL) {
		normal_wifi_hal = malloc(sizeof(struct wifi_hal_t));
		if (normal_wifi_hal == NULL)
			return;
		memset(normal_wifi_hal, 0, sizeof(struct wifi_hal_t));
		wifi_hal_init(normal_wifi_hal);
	}

	return;
}

void wifi_normal_deinit(void)
{
	if (normal_wifi_hal) {
		free(normal_wifi_hal);
		normal_wifi_hal = NULL;
	}

	return;
}

int wifi_load_driver(int mode, char *hwaddr)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
    int count_if = 10;
    char local_ifname[64];

    if (wifi_hal->is_driver_loaded(wifi_hal, mode)) {
		printf("%s called, driver is loaded \n", __func__);
        return 0;
    }

    if (wifi_hal->load_driver(wifi_hal, mode) < 0) {
		printf("%s called,insmod driver failed \n",__func__);
        return -1;
    }

    while (count_if-- > 0) {
		if(detectwifiifnamefromproc(local_ifname)) {
			printf("wifi_load_driver: getWifiIfname fail, count_if is %d \n", count_if);
		} else {
		    break;
		}
		usleep(500000);
    }

    if (count_if <= 0) {
		printf("wifi_load_driver: failed, try rmmod wifi driver \n");
		wifi_hal->unload_driver(wifi_hal, mode);
		return -1;
	} else {

		if (ifc_init() < 0) {
			return -1;
		}
		if (ifc_up(local_ifname)) {
			printf("failed to bring up interface %s: %s\n", local_ifname, strerror(errno));
			return -1;
		}
		ifc_get_hwaddr(local_ifname, hwaddr);
		ifc_close();
    }

    return 0;
}

int wifi_unload_driver(int mode)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
	int ret;

	ret = wifi_hal->unload_driver(wifi_hal, mode);
	return ret;
}

int wifi_send_command(int index, const char *cmd, char *reply, size_t *reply_len)
{
    int ret;

    if (ctrl_conn[index] == NULL) {
        printf("Not connected to wpa_supplicant - \"%s\" command dropped.\n", cmd);
        return -1;
    }
    ret = wpa_ctrl_request(ctrl_conn[index], cmd, strlen(cmd), reply, reply_len, NULL);
    if (ret == -2) {
        printf("'%s' command timed out.\n", cmd);
        TEMP_FAILURE_RETRY(write(exit_sockets[index][0], "T", 1));
        return -2;
    } else if (ret < 0 || strncmp(reply, "FAIL", 4) == 0) {
        return -1;
    }
    if (strncmp(cmd, "PING", 4) == 0) {
        reply[*reply_len] = '\0';
    }
    return 0;
}

int wifi_command(int mode, const char *command, char *reply, size_t *reply_len)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;

    if (is_primary_interface(wifi_hal->iface_name)) {
        return wifi_send_command(PRIMARY, command, reply, reply_len);
    } else {
        return wifi_send_command(SECONDARY, command, reply, reply_len);
    }
}

int do_dhcp_request(uint32_t *ipaddr, uint32_t *gateway, uint32_t *mask,
                    uint32_t *dns1, uint32_t *dns2, uint32_t *server, uint32_t *lease) 
{
#if 0
    if (ifc_init() < 0)
        return -1;

    if (do_dhcp((char *)primary_iface) < 0) {
        ifc_close();
        return -1;
    }
    ifc_close();
    get_dhcp_info(ipaddr, gateway, mask, dns1, dns2, server, lease);
#else
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
	FILE *fp;
	char line[128], *temp;
	int ret, dns_seg1 = 0, dns_seg2 = 0, dns_seg3 = 0, dns_seg4 = 0, count, retry = 0;
	if (ifc_init() < 0)
        return -1;

	if (ifc_get_info(wifi_hal->iface_name, ipaddr, &ret, NULL) < 0) {
		*ipaddr = 0;
		*mask = 0;
	}
	*mask = prefixLengthToIpv4Netmask(ret);
	ret = ifc_get_default_route(wifi_hal->iface_name);
	if (ret)
		*gateway = ret;

	ifc_close();

	*dns1 = 0;
	do {
		if ((fp = fopen(DNS_PATH, "r")) == NULL) {
			printf("Could not open %s: %s \n", DNS_PATH, strerror(errno));
			return 0;
	    }
	    count = 0;
		while ((fgets(line, sizeof(line), fp)) != NULL) {
			if (line[0] == '#')
				continue;
			if ((temp = strstr(line , "nameserver")) != NULL) {
				ret = strlen("nameserver");
				while ((temp[ret] == ' ') && (ret < (128 - strlen("nameserver"))))
					ret++;
				if (ret < (128 - strlen("nameserver"))) {
					sscanf(temp + ret, "%d.%d.%d.%d", &dns_seg1, &dns_seg2, &dns_seg3, &dns_seg4);
					if (count == 0)
						*dns1 = (((dns_seg4 & 0xff) << 24) | ((dns_seg3 & 0xff) << 16) | ((dns_seg2 & 0xff) << 8) | (dns_seg1 & 0xff));
					else if (count == 1)
						*dns2 = (((dns_seg4 & 0xff) << 24) | ((dns_seg3 & 0xff) << 16) | ((dns_seg2 & 0xff) << 8) | (dns_seg1 & 0xff));
					else
						break;
					count++;
				}
			}
		}
		fclose(fp);
		if (*dns1 == 0)
			usleep(50000);
	} while ((*dns1 == 0) && (retry++ < 40));
	*lease = 0;
#endif
    return 0;
}

int wifi_connect_on_socket_path(int index, const char *path)
{
    ctrl_conn[index] = wpa_ctrl_open(path);
    if (ctrl_conn[index] == NULL) {
        printf("Unable to open connection to supplicant on \"%s\": %s \n",
             path, strerror(errno));
        return -1;
    }
    monitor_conn[index] = wpa_ctrl_open(path);
    if (monitor_conn[index] == NULL) {
        wpa_ctrl_close(ctrl_conn[index]);
        ctrl_conn[index] = NULL;
        return -2;
    }
    if (wpa_ctrl_attach(monitor_conn[index]) != 0) {
        wpa_ctrl_close(monitor_conn[index]);
        wpa_ctrl_close(ctrl_conn[index]);
        ctrl_conn[index] = monitor_conn[index] = NULL;
        return -3;
    }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, exit_sockets[index]) == -1) {
        wpa_ctrl_close(monitor_conn[index]);
        wpa_ctrl_close(ctrl_conn[index]);
        ctrl_conn[index] = monitor_conn[index] = NULL;
        return -4;
    }

    return 0;
}

int wifi_connect_to_supplicant(int mode)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
    char path[256];

    if (is_primary_interface(wifi_hal->iface_name)) {
        if (access(IFACE_DIR, F_OK) == 0) {
            snprintf(path, sizeof(path), "%s/%s", IFACE_DIR, primary_iface);
        } else {
            strcpy(path, primary_iface);
        }
        return wifi_connect_on_socket_path(PRIMARY, path);
    } else {
        sprintf(path, "%s/%s", CONTROL_IFACE_PATH, wifi_hal->iface_name);
        return wifi_connect_on_socket_path(SECONDARY, path);
    }
}

int update_ctrl_interface(const char *config_file) {

    int srcfd, destfd;
    int nread;
    char ifc[64];
    char *pbuf;
    char *sptr;
    struct stat sb;
    int ret;

    if (stat(config_file, &sb) != 0)
        return -1;

    pbuf = malloc(sb.st_size + 64);
    if (!pbuf)
        return 0;
    srcfd = TEMP_FAILURE_RETRY(open(config_file, O_RDONLY));
    if (srcfd < 0) {
        printf("Cannot open \"%s\": %s \n", config_file, strerror(errno));
        free(pbuf);
        return 0;
    }
    nread = TEMP_FAILURE_RETRY(read(srcfd, pbuf, sb.st_size));
    close(srcfd);
    if (nread < 0) {
        printf("Cannot read \"%s\": %s \n", config_file, strerror(errno));
        free(pbuf);
        return 0;
    }

    if (!strcmp(config_file, SUPP_CONFIG_FILE)) {
        strcpy(ifc, primary_iface);
    } else {
        strcpy(ifc, CONTROL_IFACE_PATH);
    }
    /* Assume file is invalid to begin with */
    ret = -1;
    /*
     * if there is a "ctrl_interface=<value>" entry, re-write it ONLY if it is
     * NOT a directory.  The non-directory value option is an Android add-on
     * that allows the control interface to be exchanged through an environment
     * variable (initialized by the "init" program when it starts a service
     * with a "socket" option).
     *
     * The <value> is deemed to be a directory if the "DIR=" form is used or
     * the value begins with "/".
     */
    sptr = strstr(pbuf, "ctrl_interface=");
    if (sptr != NULL) {
        ret = 0;
        if ((!strstr(pbuf, "ctrl_interface=DIR=")) &&
                (!strstr(pbuf, "ctrl_interface=/"))) {
            char *iptr = sptr + strlen("ctrl_interface=");
            int ilen = 0;
            int mlen = strlen(ifc);
            if (strncmp(ifc, iptr, mlen) != 0) {
                printf("ctrl_interface != %s \n", ifc);
                while (((ilen + (iptr - pbuf)) < nread) && (iptr[ilen] != '\n'))
                    ilen++;
                mlen = ((ilen >= mlen) ? ilen : mlen) + 1;
                memmove(iptr + mlen, iptr + ilen + 1, nread - (iptr + ilen + 1 - pbuf));
                memset(iptr, '\n', mlen);
                memcpy(iptr, ifc, strlen(ifc));
                destfd = TEMP_FAILURE_RETRY(open(config_file, O_RDWR, 0660));
                if (destfd < 0) {
                    printf("Cannot update \"%s\": %s \n", config_file, strerror(errno));
                    free(pbuf);
                    return -1;
                }
                TEMP_FAILURE_RETRY(write(destfd, pbuf, nread + mlen - ilen -1));
                close(destfd);
            }
        }
    }
    free(pbuf);
    return ret;
}

int ensure_config_file_exists(const char *config_file)
{
    char buf[2048];
    int srcfd, destfd;
    int nread;
    int ret;

    ret = access(config_file, R_OK|W_OK);
    if ((ret == 0) || (errno == EACCES)) {
        if ((ret != 0) &&
            (chmod(config_file, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) != 0)) {
            printf("Cannot set RW to \"%s\": %s \n", config_file, strerror(errno));
            return -1;
        }
        /* return if we were able to update control interface properly */
        if (update_ctrl_interface(config_file) >=0) {
            return 0;
        } else {
            /* This handles the scenario where the file had bad data
             * for some reason. We continue and recreate the file.
             */
        }
    } else if (errno != ENOENT) {
        printf("Cannot access \"%s\": %s \n", config_file, strerror(errno));
        return -1;
    }

    srcfd = TEMP_FAILURE_RETRY(open(SUPP_CONFIG_TEMPLATE, O_RDONLY));
    if (srcfd < 0) {
        printf("Cannot open \"%s\": %s \n", SUPP_CONFIG_TEMPLATE, strerror(errno));
        return -1;
    }

    destfd = TEMP_FAILURE_RETRY(open(config_file, O_CREAT|O_RDWR, 0660));
    if (destfd < 0) {
        close(srcfd);
        printf("Cannot create \"%s\": %s \n", config_file, strerror(errno));
        return -1;
    }

    while ((nread = TEMP_FAILURE_RETRY(read(srcfd, buf, sizeof(buf)))) != 0) {
        if (nread < 0) {
            printf("Error reading \"%s\": %s \n", SUPP_CONFIG_TEMPLATE, strerror(errno));
            close(srcfd);
            close(destfd);
            unlink(config_file);
            return -1;
        }
        TEMP_FAILURE_RETRY(write(destfd, buf, nread));
    }

    close(destfd);
    close(srcfd);

    /* chmod is needed because open() didn't set permisions properly */
    if (chmod(config_file, 0660) < 0) {
        printf("Error changing permissions of %s to 0660: %s \n",
             config_file, strerror(errno));
        unlink(config_file);
        return -1;
    }

    /*if (chown(config_file, AID_SYSTEM, AID_WIFI) < 0) {
        printf("Error changing group ownership of %s to %d: %s",
             config_file, AID_WIFI, strerror(errno));
        unlink(config_file);
        return -1;
    }*/
    return update_ctrl_interface(config_file);
}

int ensure_entropy_file_exists()
{
    int ret;
    int destfd;

    ret = access(SUPP_ENTROPY_FILE, R_OK|W_OK);
    if ((ret == 0) || (errno == EACCES)) {
        if ((ret != 0) &&
            (chmod(SUPP_ENTROPY_FILE, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) != 0)) {
            printf("Cannot set RW to \"%s\": %s \n", SUPP_ENTROPY_FILE, strerror(errno));
            return -1;
        }
        return 0;
    }
    destfd = TEMP_FAILURE_RETRY(open(SUPP_ENTROPY_FILE, O_CREAT|O_RDWR, 0660));
    if (destfd < 0) {
        printf("Cannot create \"%s\": %s \n", SUPP_ENTROPY_FILE, strerror(errno));
        return -1;
    }

    if (TEMP_FAILURE_RETRY(write(destfd, dummy_key, sizeof(dummy_key))) != sizeof(dummy_key)) {
        printf("Error writing \"%s\": %s \n", SUPP_ENTROPY_FILE, strerror(errno));
        close(destfd);
        return -1;
    }
    close(destfd);

    /* chmod is needed because open() didn't set permisions properly */
    if (chmod(SUPP_ENTROPY_FILE, 0660) < 0) {
        printf("Error changing permissions of %s to 0660: %s \n",
             SUPP_ENTROPY_FILE, strerror(errno));
        unlink(SUPP_ENTROPY_FILE);
        return -1;
    }

    /*if (chown(SUPP_ENTROPY_FILE, AID_SYSTEM, AID_WIFI) < 0) {
        printf("Error changing group ownership of %s to %d: %s",
             SUPP_ENTROPY_FILE, AID_WIFI, strerror(errno));
        unlink(SUPP_ENTROPY_FILE);
        return -1;
    }*/
    return 0;
}

void wifi_wpa_ctrl_cleanup(void)
{
    DIR *dir;
    struct dirent entry;
    struct dirent *result;
    size_t dirnamelen;
    size_t maxcopy;
    char pathname[PATH_MAX];
    char *namep;
    const char *local_socket_dir = CONTROL_IFACE_PATH;
    char *local_socket_prefix = CONFIG_CTRL_IFACE_CLIENT_PREFIX;

    if ((dir = opendir(local_socket_dir)) == NULL)
        return;

    dirnamelen = (size_t)snprintf(pathname, sizeof(pathname), "%s/", local_socket_dir);
    if (dirnamelen >= sizeof(pathname)) {
        closedir(dir);
        return;
    }
    namep = pathname + dirnamelen;
    maxcopy = PATH_MAX - dirnamelen;
    while (readdir_r(dir, &entry, &result) == 0 && result != NULL) {
        if (strncmp(entry.d_name, local_socket_prefix, strlen(local_socket_prefix)) == 0) {
            if (strlen(strncpy(namep, entry.d_name, maxcopy)) < maxcopy) {
                unlink(pathname);
            }
        }
    }
    closedir(dir);
}

int wifi_set_ipfwd_enabled(int enable)
{
    int fd = open("/proc/sys/net/ipv4/ip_forward", O_WRONLY);
    if (fd < 0) {
        printf("Failed to open ip_forward (%s) \n", strerror(errno));
        return -1;
    }

    if (write(fd, (enable ? "1" : "0"), 1) != 1) {
        printf("Failed to write ip_forward (%s) \n", strerror(errno));
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int wifi_start_supplicant(int p2p_supported)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
	int i =0;
	char cmd[256];
	int fd;
	char *wbuf = NULL;

    if (p2p_supported) {
        strcpy(supplicant_name, P2P_SUPPLICANT_NAME);
        /* Ensure p2p config file is created */
        if (ensure_config_file_exists(P2P_CONFIG_FILE) < 0) {
            printf("Failed to create a p2p config file \n");
            return -1;
        }
    } else {
        strcpy(supplicant_name, SUPPLICANT_NAME);
    }

    /* Before starting the daemon, make sure its config file exists */
    if (ensure_config_file_exists(SUPP_CONFIG_FILE) < 0) {
        printf("Wi-Fi will not be enabled \n");
	    asprintf(&wbuf, "ctrl_interface=/var/run/wpa_supplicant\nupdate_config=1\nap_scan=1\n");

	    fd = open(SUPP_CONFIG_FILE, O_CREAT | O_TRUNC | O_WRONLY, 0660);
	    if (fd < 0) {
	        printf("Cannot update \"%s\": %s \n", SUPP_CONFIG_FILE, strerror(errno));
	        free(wbuf);
	        return -1;
	    }
	    if (write(fd, wbuf, strlen(wbuf)) < 0) {
	        printf("Cannot write to \"%s\": %s \n", SUPP_CONFIG_FILE, strerror(errno));
		    free(wbuf);
		    close(fd);
	        return -2;
	    }
	    free(wbuf);
	    close(fd);
    }

    if (ensure_entropy_file_exists() < 0) {
        printf("Wi-Fi entropy file was not created \n");
    }

    /* Clear out any stale socket files that might be left over. */
    wifi_wpa_ctrl_cleanup();

    /* Reset sockets used for exiting from hung state */
    for (i=0; i<MAX_CONNS; i++) {
        exit_sockets[i][0] = exit_sockets[i][1] = -1;
    }

	sprintf(cmd, "%s -Dnl80211 -i%s  -c %s &", wifi_hal->supplicant_path, wifi_hal->iface_name, SUPP_CONFIG_TEMPLATE);
	system(cmd);
	return 0;
}

static void ensure_ap_config(void)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
	FILE *fp;
	char pline[2] = {0}, cmd1[256], cmd2[128], hwaddr[16], ssid_addr[13] = {0};
	int pline_int = 0, i = 0;

	sprintf(cmd1, "grep -o \"%%[1-6]m\" %s | grep -o \"[1-6]\"", HOSTAPD_CONF_FILE);
	if ((fp = popen(cmd1, "r")) == NULL) {
		printf("popen error\n");
	} else {
		//if ssid strings in hostapd.conf file are ended with "%[1-6]m", we will replace "%[1-6]m" with corresponding wifi hwaddr.
		if (fread(pline, 1, 1, fp) == 1 && isdigit(pline[0])) {

			if (ifc_init() < 0) {
				return;
			}
			if (ifc_up(wifi_hal->iface_name)) {
				printf("failed to bring up interface %s: %s\n", wifi_hal->iface_name, strerror(errno));
				return;
			}
			ifc_get_hwaddr(wifi_hal->iface_name, hwaddr);
			ifc_close();

			pline_int = atoi(pline);
			for (i=(6-pline_int); i<6; i++)
				sprintf(ssid_addr+2*(pline_int+i-6), "%02x", (int)hwaddr[i]);

			sprintf(cmd2,"sed -i \'s/%%[1-6]m/%s/g\' %s", ssid_addr, HOSTAPD_CONF_FILE);
			if (system(cmd2) < 0)
				printf("system() error \n");
		}
		pclose(fp);
	}

	return;
}

int wifi_ap_start_hostapd(void)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
	char cmd[256];
	int pid, cnt = 0;

	ensure_ap_config();
	ensure_entropy_file_exists();
	sprintf(cmd, "%s %s &", wifi_hal->hostapd_path, HOSTAPD_CONF_FILE);
	system(cmd);

	sleep(1);
	//sprintf(cmd, "hostapd_cli -i %s -a ap_state &", wifi_hal->iface_name);
	sprintf(cmd, "hostapd_cli -i %s -a /usr/bin/ap_state &", wifi_hal->iface_name);
	do {
		system(cmd);
		usleep(400000);
		pid = system_check_process("hostapd_cli");
		if (pid > 0)
			break;
		cnt++;
	} while (cnt < 10);

	return 0;
}

int wifi_ap_stop_hostapd(void)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
	char cmd[256];

	sprintf(cmd, "killall %s", wifi_hal->hostapd_path);
	system(cmd);
	system_kill_process("hostapd_cli");

	return 0;
}

int wifi_ap_start_dns(void)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
	char cmd[256], dns_ip[32], dns_route[32];
	int prefixLength = 32;
    int fd, ret, ip1, ip2, ip3, ip4;
    char fbuf[4096];
	char split[] = "\n";
	char *result = NULL;
	long fsize, read_size = 0;

    fd = open(APDNS_CONF_FILE, O_RDWR);
    if (fd < 0) {
        printf("Cannot open \"%s\": %s \n", APDNS_CONF_FILE, strerror(errno));
        return -1;
    }
    fsize = lseek(fd, 0L, SEEK_END);

    ret = lseek(fd, 0, SEEK_SET);
    if (ret < 0) {
        printf("lseek failed with %s.\n", strerror(errno));
        return -1;
    }
    while (read_size < fsize) {
		ret = read(fd, fbuf + read_size, fsize - read_size);
		if (ret < 0) {
	        printf("Cannot read to \"%s\": %s \n", APDNS_CONF_FILE, strerror(errno));
		    close(fd);
			return -2;
		}
		read_size += ret;
    }

	sprintf(dns_ip, "192.168.0.1");
	sprintf(dns_route, "192.168.0.0");
	result = strtok(fbuf, split);
	while (result != NULL) {
		if (!strncmp(result, "listen-address=", strlen("listen-address="))) {
			snprintf(dns_ip, 32, "%s", result + strlen("listen-address="));
			sscanf(dns_ip, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
			sprintf(dns_ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
			sprintf(dns_route, "%d.%d.%d.0", ip1, ip2, ip3);
			break;
		}
		result = strtok(NULL, split);
	}
    close(fd);

	if (ifc_init() < 0) {
		return -1;
	}
	if (ifc_up(wifi_hal->ap_iface_name)) {
		printf("failed to bring up interface %s: %s\n", wifi_hal->iface_name, strerror(errno));
		return -1;
	}
	if (ifc_add_address(wifi_hal->ap_iface_name, dns_ip, prefixLength)) {
		printf("failed to conf ip interface %s: %s\n", wifi_hal->ap_iface_name, strerror(errno));
		return -1;
	}
	if (ifc_set_prefixLength(wifi_hal->ap_iface_name, 24)) {
		printf("failed to set mask interface %s: %s\n", wifi_hal->ap_iface_name, strerror(errno));
		return -1;
	}
	if (ifc_add_route(wifi_hal->ap_iface_name, dns_route, prefixLength, dns_ip)) {
		printf("failed to add route interface %s: %s\n", wifi_hal->ap_iface_name, strerror(errno));
		return -1;
	}
	ifc_close();

	wifi_set_ipfwd_enabled(1);
	sprintf(cmd, "%s -C %s &", wifi_hal->apdns_path, APDNS_CONF_FILE);
	system(cmd);

	return 0;
}

int wifi_ap_stop_dns(void)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
	char cmd[256];

	wifi_set_ipfwd_enabled(0);
	sprintf(cmd, "killall %s", wifi_hal->apdns_path);
	system(cmd);

	return 0;
}

int wifi_get_config(char *interface, char *ssid, char *security, char *passphrase, int *channel)
{
    int wpa = 0, ch = 1;
    int fd, ret;
    char fbuf[4096];
	char split[] = "\n";
	char *result = NULL;
	long fsize, read_size = 0;

    fd = open(HOSTAPD_CONF_FILE, O_RDWR);
    if (fd < 0) {
        printf("Cannot open \"%s\": %s \n", HOSTAPD_CONF_FILE, strerror(errno));
        return -1;
    }
    fsize = lseek(fd, 0L, SEEK_END);

    ret = lseek(fd, 0, SEEK_SET);
    if (ret < 0) {
        printf("lseek failed with %s.\n", strerror(errno));
        return -1;
    }
    while (read_size < fsize) {
		ret = read(fd, fbuf + read_size, fsize - read_size);
		if (ret < 0) {
	        printf("Cannot read to \"%s\": %s \n", HOSTAPD_CONF_FILE, strerror(errno));
		    close(fd);
			return -2;
		}
		read_size += ret;
    }

	result = strtok(fbuf, split);
	while (result != NULL) {
		if (!strncmp(result, "ssid=", strlen("ssid=")))
			snprintf(ssid, 32, "%s", result + strlen("ssid="));
		else if (!strncmp(result, "wpa_passphrase=", strlen("wpa_passphrase=")))
			snprintf(passphrase, 16, "%s", result + strlen("wpa_passphrase="));
		else if (!strncmp(result, "wpa=", strlen("wpa="))) {
			wpa = atoi(result + strlen("wpa="));
			if (wpa == 2)
				sprintf(security, "%s", "wpa2-psk");
			else
				sprintf(security, "%s", "wpa-psk");
		} else if (!strncmp(result, "channel=", strlen("channel="))) {
			ch = atoi(result + strlen("channel="));
			*channel = ch;
		}
		result = strtok(NULL, split);
	}
	if (wpa == 0)
		sprintf(security, "%s", "open");

    close(fd);
    return 0;
}

int wifi_set_config(char *interface, char *ssid, char *security, char *passphrase, int channel)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
    char psk_str[2*SHA256_DIGEST_LENGTH+1];
    int fd, ch = 1;
    char *wbuf = NULL;
    char *fbuf = NULL;

	if (channel > 0)
		ch = channel;
    asprintf(&wbuf, "interface=%s\ndriver=nl80211\nctrl_interface="
            "/var/run/hostapd\nctrl_interface_group=0\nssid=%s\nchannel=%d\n"
            "hw_mode=g\nauth_algs=1\n",
            (interface != NULL) ? interface : wifi_hal->ap_iface_name, ssid, ch);

    if (passphrase != NULL) {
        if (!strcmp(security, "wpa-psk")) {
            wifi_ap_generatepsk(ssid, passphrase, psk_str);
            asprintf(&fbuf, "%swpa=1\nwpa_key_mgmt=WPA-PSK\nwpa_pairwise=TKIP CCMP\nwpa_psk=%s\nwpa_passphrase=%s\n", wbuf, psk_str, passphrase);
        } else if (!strcmp(security, "wpa2-psk")) {
            wifi_ap_generatepsk(ssid, passphrase, psk_str);
            asprintf(&fbuf, "%swpa=2\nwpa_key_mgmt=WPA-PSK\nrsn_pairwise=CCMP\nwpa_psk=%s\nwpa_passphrase=%s\n", wbuf, psk_str, passphrase);
        } else if (!strcmp(security, "open")) {
            asprintf(&fbuf, "%s", wbuf);
        } else {
        	asprintf(&fbuf, "%s", wbuf);
        }
    } else {
        asprintf(&fbuf, "%s", wbuf);
    }

    fd = open(HOSTAPD_CONF_FILE, O_CREAT | O_TRUNC | O_WRONLY, 0660);
    if (fd < 0) {
        printf("Cannot update \"%s\": %s \n", HOSTAPD_CONF_FILE, strerror(errno));
        free(wbuf);
        free(fbuf);
        return -1;
    }
    if (write(fd, fbuf, strlen(fbuf)) < 0) {
        printf("Cannot write to \"%s\": %s \n", HOSTAPD_CONF_FILE, strerror(errno));
	    free(wbuf);
	    free(fbuf);
	    close(fd);
        return -2;
    }
    free(wbuf);
    free(fbuf);

    close(fd);
    return 0;
}

int wifi_stop_supplicant(int p2p_supported)
{
	//struct wifi_hal_t *wifi_hal = normal_wifi_hal;
	//char cmd[256];

    if (p2p_supported) {
        strcpy(supplicant_name, P2P_SUPPLICANT_NAME);
    } else {
        strcpy(supplicant_name, SUPPLICANT_NAME);
    }

    //sprintf(cmd, "killall -9 %s", wifi_hal->supplicant_path);
    //system(cmd);

    return 0;
}

int wifi_start_dhcpcd(int mode, int timeout)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
	FILE *fp;
	char line[128], *temp;
	char cmd[64];
	int ret = 0, cnt = 0, pid = 0,dhcpcd_pid = 0;

	do {
		pid = system_check_process("dhcpcd");
		if (pid < 0)
			break;
		system_kill_process("dhcpcd");
		usleep(20000);
		cnt++;
	} while (cnt < 50);

	sprintf(cmd, "dhcpcd -d %s &", wifi_hal->iface_name);
	system(cmd);
	cnt = 0;
	do {
		pid = system_check_process("dhcpcd");
		if (pid > 0)
			break;
		usleep(20000);
		cnt++;
	} while (cnt < 10);
	wifi_hal->stop_dhcp = 0;

	cnt = 0;
	ret = 1;
	do {
		dhcpcd_pid = system_check_process("dhcpcd");
		if (dhcpcd_pid < 0) {
			if (!wifi_hal->stop_dhcp)
				ret = 2;
			else
				ret = -1;
			break;
                }
                //after dhcpcd is completed,pid of dhcpcd will change.then read dns.
                if (dhcpcd_pid != pid){
                    if ((fp = fopen(DNS_PATH, "r")) == NULL) {
                        printf("waiting dhcp file %s: \n", DNS_PATH);
                    } else {
                        while ((fgets(line, sizeof(line), fp)) != NULL) {
                            if (line[0] == '#')
                                continue;
                            if ((temp = strstr(line , "nameserver")) != NULL) {
                                ret = ifc_get_default_route(wifi_hal->iface_name);
                                printf("default_route %x \n", ret);
                                if (ret) {
                                    ifc_set_default_route(wifi_hal->iface_name, (in_addr_t)ret);
                                    ret = 0;
                                }
                                break;
                            }
                        }
                        fclose(fp);
                    }
                }
                else
                {
                    //printf("waiting dhcp file...pid=%d dhcpcd_pid=%d\n",pid,dhcpcd_pid);
                }
                if (ret)
                     usleep(200000);
    	} while ((cnt++ < 300) && (ret));

	return ret;
}

void wifi_stop_dhcpcd(int mode)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;
	int pid = 0;

	wifi_hal->stop_dhcp = 1;
	system("rm -f /var/run/dhcpcd-wlan0.pid");
	while (1) {
		pid = system_check_process("dhcpcd");
		if (pid < 0)
			break;
		system_kill_process("dhcpcd");
		usleep(20000);
	}
	return;
}

void wifi_close_sockets(int index)
{
    if (ctrl_conn[index] != NULL) {
        wpa_ctrl_close(ctrl_conn[index]);
        ctrl_conn[index] = NULL;
    }

    if (monitor_conn[index] != NULL) {
        wpa_ctrl_close(monitor_conn[index]);
        monitor_conn[index] = NULL;
    }

    if (exit_sockets[index][0] >= 0) {
        close(exit_sockets[index][0]);
        exit_sockets[index][0] = -1;
    }

    if (exit_sockets[index][1] >= 0) {
        close(exit_sockets[index][1]);
        exit_sockets[index][1] = -1;
    }
}

void wifi_close_supplicant_connection(const char *ifname)
{
    if (is_primary_interface(ifname)) {
        wifi_close_sockets(PRIMARY);
    } else {
        /* p2p socket termination needs unblocking the monitor socket
         * STA connection does not need it since supplicant gets shutdown
         */
        TEMP_FAILURE_RETRY(write(exit_sockets[SECONDARY][0], "T", 1));
        /* p2p sockets are closed after the monitor thread
         * receives the terminate on the exit socket
         */
        return;
    }
}

int wifi_ctrl_recv(int index, char *reply, size_t *reply_len)
{
    int res;
    int ctrlfd = wpa_ctrl_get_fd(monitor_conn[index]);
    struct pollfd rfds[2];

    memset(rfds, 0, 2 * sizeof(struct pollfd));
    rfds[0].fd = ctrlfd;
    rfds[0].events |= POLLIN;
    rfds[1].fd = exit_sockets[index][1];
    rfds[1].events |= POLLIN;
    res = TEMP_FAILURE_RETRY(poll(rfds, 2, -1));
    if (res < 0) {
        printf("Error poll = %d \n", res);
        return res;
    }
    if (rfds[0].revents & POLLIN) {
        return wpa_ctrl_recv(monitor_conn[index], reply, reply_len);
    } else if (rfds[1].revents & POLLIN) {
        /* Close only the p2p sockets on receive side
         * see wifi_close_supplicant_connection()
         */
        if (index == SECONDARY) {
            printf("close sockets %d \n", index);
            wifi_close_sockets(index);
        }
    }
    return -2;
}

int wifi_wait_on_socket(int index, char *buf, size_t buflen)
{
    size_t nread = buflen - 1;
    int result;

    if (monitor_conn[index] == NULL) {
        printf("Connection closed\n");
        strncpy(buf, WPA_EVENT_TERMINATING " - connection closed", buflen-1);
        buf[buflen-1] = '\0';
        return strlen(buf);
    }

    result = wifi_ctrl_recv(index, buf, &nread);

    /* Terminate reception on exit socket */
    if (result == -2) {
        strncpy(buf, WPA_EVENT_TERMINATING " - connection closed", buflen-1);
        buf[buflen-1] = '\0';
        return strlen(buf);
    }

    if (result < 0) {
        printf("wifi_ctrl_recv failed: %s\n", strerror(errno));
        strncpy(buf, WPA_EVENT_TERMINATING " - recv error", buflen-1);
        buf[buflen-1] = '\0';
        return strlen(buf);
    }
    buf[nread] = '\0';
    /* Check for EOF on the socket */
    if (result == 0 && nread == 0) {
        /* Fabricate an event to pass up */
        printf("Received EOF on supplicant socket\n");
        strncpy(buf, WPA_EVENT_TERMINATING " - signal 0 received", buflen-1);
        buf[buflen-1] = '\0';
        return strlen(buf);
    }
    /*
     * Events strings are in the format
     *
     *     <N>CTRL-EVENT-XXX 
     *
     * where N is the message level in numerical form (0=VERBOSE, 1=DEBUG,
     * etc.) and XXX is the event name. The level information is not useful
     * to us, so strip it off.
     */
    if (buf[0] == '<') {
        char *match = strchr(buf, '>');
        if (match != NULL) {
            nread -= (match+1-buf);
            memmove(buf, match+1, nread+1);
        }
    }

    return nread;
}

int wifi_wait_for_event(int mode, char *buf, size_t buflen)
{
	struct wifi_hal_t *wifi_hal = normal_wifi_hal;

    if (is_primary_interface(wifi_hal->iface_name)) {
        return wifi_wait_on_socket(PRIMARY, buf, buflen);
    } else {
        return wifi_wait_on_socket(SECONDARY, buf, buflen);
    }
}

