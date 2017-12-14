/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: Wi-Fi demos by using qlibwifi
 *
 * Author:
 *     bob.yang <bob.yagn@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  09/25/2017 init
 */

#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <wifi.h>

#define MAXLINE 4096

int g_s32EnableAp = 0;

/*
 * @brief Wifi signal handler
 * @param s32Signo  -IN: signal number
 * @return void
 */
void WifiKillHandler(int s32Signo)
{
    printf("Exiting signo:%d \n", s32Signo);
    if (g_s32EnableAp) {
        wifi_ap_disable();
    }
    exit(0);
}

/*
 * @brief Setup wifi signal handler
 * @param void
 * @return void
 */
void WifiSignal()
{
    signal(SIGINT, WifiKillHandler);
    signal(SIGTERM, WifiKillHandler);
    signal(SIGHUP, WifiKillHandler);
    signal(SIGQUIT, WifiKillHandler);
    signal(SIGABRT, WifiKillHandler);
    signal(SIGKILL, WifiKillHandler);
    signal(SIGSEGV, WifiKillHandler);
}

/*
 * @brief Show wifi config
 * @param config  -IN: Wifi config pointer
 * @return void
 */
void ShowWifiConfig(struct wifi_config_t *config)
{
    printf("network_id is %d\n", config->network_id);
    printf("status is %d \n", config->status);
    printf("list_type is %d \n", config->list_type);
    printf("level is %d \n", config->level);
    printf("channel is %d \n", config->channel);
    printf("ssid is %s \n", config->ssid);
    printf("bssid is %s \n", config->bssid);
    printf("key_management is %s \n", config->key_management);
    printf("pairwise_ciphers is %s \n", config->pairwise_ciphers);
    printf("psk is %s \n", config->psk);
}

/*
 * @brief Show network information
 * @param info  -IN: Network information pointer
 * @return void
 */
void ShowNetInfo(struct net_info_t *info)
{
    printf("hwaddr is %s\n", info->hwaddr);
    printf("ipaddr is %s\n", info->ipaddr);
    printf("gateway is %s\n", info->gateway);
    printf("mask is %s\n", info->mask);
    printf("dns1 is %s\n", info->dns1);
    printf("dns2 is %s\n", info->dns2);
    printf("server is %s\n", info->server);
    printf("lease is %s\n", info->lease);
}

/*
 * @brief Callback function for wifi service, and we can get wifi state information by it.
 * @param event  -IN: Event information
 * @param arg  -IN: Wifi state information
 * @return void
 */
void WifiStateHandle(char *event, void *arg)
{
    printf("func %s, event is %s, arg is %s \n", __func__, event, (char *)arg);

    if (!strncmp(event, WIFI_STATE_EVENT, strlen(WIFI_STATE_EVENT))) {
        if (!strncmp((char *)arg, STATE_AP_ENABLED, strlen(STATE_AP_ENABLED))) {
            g_s32EnableAp = 1;
            printf("ap enabled \n");
        } else if (!strncmp((char *)arg, STATE_AP_DISABLED, strlen(STATE_AP_DISABLED))) {
            g_s32EnableAp = 0;
            printf("ap disabled \n");
        } else if (!strncmp(event, STATE_AP_STA_CONNECTED, strlen(STATE_AP_STA_CONNECTED))) {
            printf("ap sta connected info \n");
            ShowNetInfo((struct net_info_t *)arg);
        } else if (!strncmp(event, STATE_AP_STA_DISCONNECTED, strlen(STATE_AP_STA_DISCONNECTED))) {
            printf("ap sta disconected info \n");
            ShowNetInfo((struct net_info_t *)arg);
        }
    }
}

/*
 * @brief main function
 * @param argc      -IN: args' count
 * @param argv      -IN: args
 * @return 0: Success.
 */
int main(int argc, char *argv[])
{
    struct wifi_config_t stSetApConfig, stGetApConfig, *pstGetApConfig = &stGetApConfig;
    struct net_info_t stNetInfo;
    int i = 0, s32Ret = 0, s32GetState = 0;
    char s8Buf[MAXLINE]; 

    WifiSignal();

    printf("Please enter your command number: \n \
           1 ap_enable \n \
           2 ap_disable \n \
           3 ap_set_config \n \
           4 ap_get_config \n \
           5 get_wifi_state \n \
           6 get_net_info \n \
           7 exit \n");

    wifi_start_service(WifiStateHandle);

    printf("%% ");	/* print prompt (printf requires %% to print %) */

    while (fgets(s8Buf, MAXLINE, stdin) != NULL) {
        if (s8Buf[strlen(s8Buf) - 1] == '\n')
            s8Buf[strlen(s8Buf) - 1] = 0; /* replace newline with null */
        printf("your command is %s \n", s8Buf);

        switch (atoi(s8Buf)) {
        case 1:
            wifi_ap_enable();
            break;
        case 2:
            wifi_ap_disable();
            break;
        case 3:
            strcpy(stSetApConfig.ssid, "ap_test");
            strcpy(stSetApConfig.key_management, "wpa2-psk");
            strcpy(stSetApConfig.psk, "11111111");
            s32Ret = wifi_ap_set_config(&stSetApConfig);
            if (s32Ret) {
                printf("ap_set_config failed \n");
            }
            break;
        case 4:
            s32Ret = wifi_ap_get_config(pstGetApConfig);
            if (s32Ret) {
                printf("ap_get_config failed \n");
            }
            if (pstGetApConfig != NULL) {
                ShowWifiConfig(pstGetApConfig);
            }
            break;
        case 5:
            s32GetState = wifi_get_state();
            printf("s32GetState is 0x%x \n", s32GetState);
            break;
        case 6:
            for (i=0; i<20; i++) {
                if (g_s32EnableAp) {
                    s32Ret = wifi_get_net_info(&stNetInfo);
                    if (!s32Ret)
                        ShowNetInfo(&stNetInfo);
                    break;
                }
                usleep(200000);
            }
            break;
        case 7:
            if (g_s32EnableAp) {
                wifi_ap_disable();
            }
            exit(0);

        default:
            break;
        }
        printf("%% ");
    }
    exit(0);
}
