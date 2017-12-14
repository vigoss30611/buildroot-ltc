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

int g_s32EnableSta = 0;
int g_s32ConnectSta = 0;

/*
 * @brief Wifi signal handler
 * @param s32Signo  -IN: signal number
 * @return void
 */
void WifiKillHandler(int s32Signo)
{
    printf("Exiting signo:%d \n", s32Signo);
    if (g_s32EnableSta) {
        wifi_sta_disable();
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
    struct wifi_config_t *pstGetStaScanResult = NULL, *pstGetPrevStaScanResult = NULL, *pstGetStaConfig = NULL;

    printf("func %s, event is %s, arg is %s \n", __func__, event, (char *)arg);

    if (!strncmp(event, WIFI_STATE_EVENT, strlen(WIFI_STATE_EVENT))) {
        if (!strncmp((char *)arg, STATE_STA_ENABLED, strlen(STATE_STA_ENABLED))) {
            g_s32EnableSta = 1;
            printf("sta enabled \n");
        } else if (!strncmp((char *)arg, STATE_STA_DISABLED, strlen(STATE_STA_DISABLED))) {
            g_s32EnableSta = 0;
            printf("sta disabled \n");
        } else if (!strncmp((char *)arg, STATE_SCAN_COMPLETE, strlen(STATE_SCAN_COMPLETE))) {
            printf("scan complete, now get scan results \n");

            pstGetStaScanResult =  wifi_sta_get_scan_result();
            if (pstGetStaScanResult != NULL) {
                ShowWifiConfig(pstGetStaScanResult);
            }

            while (pstGetStaScanResult != NULL) {
                pstGetPrevStaScanResult = pstGetStaScanResult;

                pstGetStaScanResult = wifi_sta_iterate_config(pstGetPrevStaScanResult);
                free(pstGetPrevStaScanResult);
                if (pstGetStaScanResult != NULL) {
                    ShowWifiConfig(pstGetStaScanResult);
                }
            }

            if (pstGetStaScanResult != NULL) {
                free(pstGetStaScanResult);
            }
        } else if (!strncmp((char *)arg, STATE_STA_CONNECTED, strlen(STATE_STA_CONNECTED))) {
            g_s32ConnectSta = 1;
            printf("sta connected \n");
            pstGetStaConfig = wifi_sta_get_config();
            if (pstGetStaConfig != NULL) {
                ShowWifiConfig(pstGetStaConfig);
                free(pstGetStaConfig);
            }
        } else if (!strncmp((char *)arg, STATE_STA_DISCONNECTED, strlen(STATE_STA_DISCONNECTED))) {
            g_s32ConnectSta = 0;
            printf("sta disconnected \n");
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
    struct wifi_config_t stConnectStaConfig, stDisconnectStaConfig;
    struct net_info_t stNetInfo;
    int i = 0, s32Ret = 0, s32GetState = 0;
    char s8Buf[MAXLINE], s8Ssid[32], s8Psk[32]; 

    WifiSignal();

    printf("Please enter your command number: \n \
           1 sta_enable \n \
           2 sta_disable \n \
           3 sta_scan \n \
           4 sta_connect \n \
           5 sta_disconnect \n \
           6 get_wifi_state \n \
           7 get_net_info \n \
           8 exit \n");

    wifi_start_service(WifiStateHandle);

    printf("%% ");	/* print prompt (printf requires %% to print %) */

    while (fgets(s8Buf, MAXLINE, stdin) != NULL) {
        if (s8Buf[strlen(s8Buf) - 1] == '\n')
            s8Buf[strlen(s8Buf) - 1] = 0; /* replace newline with null */
        printf("your command is %s \n", s8Buf);

        switch (atoi(s8Buf)) {
        case 1:
            wifi_sta_enable();
            break;
        case 2:
            wifi_sta_disable();
            break;
        case 3:
            for (i = 0; i < 20; i++) {
                if (g_s32EnableSta) {
                    wifi_sta_scan();
                    break;
                }
                usleep(200000);
            }
            break;
        case 4:
            printf("Please enter SSID and PSK for sta_connect: \n");
            s8Ssid[0] = 0;
            s8Psk[0] = 0;
            scanf("%s%s", s8Ssid, s8Psk);
            printf("s8Ssid is %s, s8Psk is %s \n", s8Ssid, s8Psk);
            for (i = 0; i < 20; i++) {
                if (g_s32EnableSta) {
                    strcpy(stConnectStaConfig.ssid, s8Ssid);
                    strcpy(stConnectStaConfig.psk, s8Psk);
                    wifi_sta_connect(&stConnectStaConfig);
                    break;
                }
                usleep(200000);
            }
            break;
        case 5:
            printf("Please enter SSID for sta_disconnect: \n");
            s8Ssid[0] = 0;
            scanf("%s", s8Ssid);
            printf("s8Ssid is %s \n", s8Ssid);
            for (i = 0; i < 20; i++) {
                if (g_s32EnableSta) {
                    strcpy(stDisconnectStaConfig.ssid, s8Ssid);
                    wifi_sta_disconnect(&stDisconnectStaConfig);
                    break;
                }
                usleep(200000);
            }
            break;
        case 6:
            s32GetState = wifi_get_state();
            printf("s32GetState is 0x%x \n", s32GetState);
            break;
        case 7:
            for (i=0; i<20; i++) {
                if (g_s32ConnectSta) {
                    s32Ret = wifi_get_net_info(&stNetInfo);
                    if (!s32Ret)
                        ShowNetInfo(&stNetInfo);
                    break;
                }
                usleep(200000);
            }
            break;
        case 8:
            if (g_s32EnableSta) {
                wifi_sta_disable();
            }
            exit(0);

        default:
            break;
        }
        printf("%% ");
    }
    exit(0);
}
