#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <qsdk/event.h>
#include "wifi.h"

#define HOSTAP_STA_CONNECTED	"AP-STA-CONNECTED"
#define HOSTAP_STA_DISCONNECTED	"AP-STA-DISCONNECTED"

static int ap_get_connect_ip(struct net_info_t *net_info)
{
    char linebuf[1024], *temp, *dest, *p;
    FILE *f;
    int cnt = 0;

	do {
		f = fopen("/var/run/dnsmasq.leases", "r");
	    if (f) {
			while (fgets(linebuf, sizeof(linebuf)-1, f) != NULL) {

				temp = strstr(linebuf, net_info->hwaddr);
				if (temp != NULL) {
					p = temp + strlen(net_info->hwaddr);

					while (*p == ' ')
						p++;
					dest = p;
					while (*p != ' ')
						p++;
					*p = '\0';

					snprintf(net_info->ipaddr, sizeof(net_info->ipaddr), dest);
					printf("ap_get_connect_ip %s \n", net_info->ipaddr);
					fclose(f);
					return 0;
				}
			}
			fclose(f);
	    }
	    usleep(500000);
	} while (cnt++ < 30);

    return 1;
}

int main(int argc, char *argv[])
{
	struct net_info_t net_info;

	if (!strcmp((char *)argv[2], HOSTAP_STA_CONNECTED)) {
		event_send(WIFI_STATE_EVENT, STATE_AP_STA_CONNECTED, (strlen(STATE_AP_STA_CONNECTED) + 1));
		memset(&net_info, 0, sizeof(struct net_info_t));
		snprintf(net_info.hwaddr, sizeof(net_info.hwaddr), (char *)argv[3]);
		ap_get_connect_ip(&net_info);
		event_send(STATE_AP_STA_CONNECTED, (char *)&net_info, sizeof(struct net_info_t));
	} else if (!strcmp((char *)argv[2], HOSTAP_STA_DISCONNECTED)) {
		event_send(WIFI_STATE_EVENT, STATE_AP_STA_DISCONNECTED, (strlen(STATE_AP_STA_DISCONNECTED) + 1));
		memset(&net_info, 0, sizeof(struct net_info_t));
		snprintf(net_info.hwaddr, sizeof(net_info.hwaddr), (char *)argv[3]);
		event_send(STATE_AP_STA_DISCONNECTED, (char *)&net_info, sizeof(struct net_info_t));
	}

	return 0;
}

