#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/prctl.h>
#include <errno.h>

#include "system_msg.h"
#include "tlnettools.h"
#include "q3_wireless.h"
#include "tlnettools.h"

#ifdef JIUAN_SUPPORT
#include "scfg.h"
#endif

#include "q3_gpio.h"
#define     _GNU_SOURCE

#define HOSTAPD_CONF_FILE "/tmp/rtl_hostapd.conf"
#define ETH_DEV "eth0"

static Wifi_State_t g_LocalWifiStatus;
void (*g_WiFiStateCB)(char* event, void *arg) = NULL;
static unsigned char gWifiServiceCounter = 0;
static QMAPI_NET_WIFI_CONFIG g_LocalWifiCfg;
static QMAPI_NET_WIFI_CONFIG g_LocalWifiCfg_bak;
//static int g_curGatewayDev = GATEWAY_DEV_ETH;

#ifdef JIUAN_SUPPORT
static unsigned char g_SonicMatchSuccess = 0;
#endif


#ifdef JIUAN_SUPPORT
static int g_SoundPairFlag = 0;
void sound_pair_callback(int status, const char *ssid, const char *passwd)
{
    if(!ssid)
        return;

    if(passwd)
    {
        strncpy(g_LocalWifiCfg.csWebKey, passwd, 32);
        strncpy(g_GlobalWifiCfg->csWebKey, passwd, 32);
    }
    printf("%s  %d, sound wave pair success!!!ssid:%s,passwd:%s\n",__FUNCTION__, __LINE__, ssid, passwd);
    g_LocalWifiCfg.bWifiEnable = 1;
    g_LocalWifiCfg.byWifiMode = 0;
    g_LocalWifiCfg.byNetworkType = 1;
    g_LocalWifiCfg.byEnableDHCP = 1;

    g_GlobalWifiCfg->bWifiEnable = 1;
    g_GlobalWifiCfg->byWifiMode = 0;
    g_GlobalWifiCfg->byNetworkType = 1;
    g_GlobalWifiCfg->byEnableDHCP= 1;
    g_SonicMatchSuccess = 1;
    g_StationConnectFail = 0;
    g_SoundPairFlag = 0;
    strncpy(g_LocalWifiCfg.csEssid, ssid, 32);
    strncpy(g_GlobalWifiCfg->csEssid, ssid, 32);
    g_bSetValue = 1;
    tl_write_wifi_config();
    TL_SysFun_Play_Audio(QMAPINOTIFY_TYPE_NETWORK_CONFIG_SUCC, 0);
}
#endif


static void show_wifi_state(Wifi_State_t state)
{
    printf("\n");
    if (state == WIFI_STATE_FAILED)
        printf("wifi state is WIFI_STATE_FAILED\n");
    else if (state == WIFI_STA_STATE_DISABLED)
        printf("wifi state is WIFI_STA_STATE_DISABLED\n");
    else if (state == WIFI_STA_STATE_ENABLED)
        printf("wifi state is WIFI_STA_STATE_ENABLED\n");
    else if (state == WIFI_STA_STATE_SCANED)
        printf("wifi state is WIFI_STA_STATE_SCANED\n");
    else if (state == WIFI_STA_STATE_CONNECTED)
        printf("wifi state is WIFI_STA_STATE_CONNECTED\n");
    else if (state == WIFI_STA_STATE_DISCONNECTED)
        printf("wifi state is WIFI_STA_STATE_DISCONNECTED\n");
    else if (state == WIFI_AP_STATE_DISABLED)
        printf("wifi state is WIFI_AP_STATE_DISABLED\n");
    else if (state == WIFI_AP_STATE_ENABLED)
        printf("wifi state is WIFI_AP_STATE_ENABLED\n");
    else if (state == WIFI_MONITOR_STATE_DISABLED)
        printf("wifi state is WIFI_MONITOR_STATE_DISABLED\n");
    else if (state == WIFI_MONITOR_STATE_ENABLED)
        printf("wifi state is WIFI_MONITOR_STATE_ENABLED\n");
    else if (state == WIFI_STATE_UNKNOWN)
        printf("wifi state is WIFI_STATE_UNKNOWN\n");
    else
        printf("wifi state error\n");
}

static void show_net_info(struct net_info_t *info)  
{
    printf("\nhwaddr is %s\n", info->hwaddr);
    printf("ipaddr is %s\n", info->ipaddr);
    printf("gateway is %s\n", info->gateway);
    printf("mask is %s\n", info->mask);
    printf("dns1 is %s\n", info->dns1);
    printf("dns2 is %s\n", info->dns2);
    printf("server is %s\n", info->server);
    printf("lease is %s\n\n", info->lease);
}

static void show_wifi_config(struct wifi_config_t *config) 
{ 
    printf("\nnetword_id is %d\n", config->network_id); 
    printf("status is %d \n", config->status); 
    printf("list_type is %d \n", config->list_type); 
    printf("level is %d \n", config->level); 
    printf("ssid is %s \n", config->ssid); 
    printf("bssid is %s \n", config->bssid); 
    printf("key_management is %s \n", config->key_management); 
    printf("pairwise_ciphers is %s \n", config->pairwise_ciphers); 
    printf("psk is %s \n\n", config->psk); 
}

static void qm_wifi_state_callback(char *event, void *arg)
{
    if (g_WiFiStateCB != NULL) {
        g_WiFiStateCB(event, arg);
    }

    printf("QM: event of %s, arg of %s\n", event, (char *)arg);
    if (!strncmp(event, WIFI_STATE_EVENT, strlen(WIFI_STATE_EVENT))) {
        if (!strncmp((char *)arg, STATE_STA_ENABLED, strlen(STATE_STA_ENABLED))) {
            printf("----------- wifi sta enabled\n");
            g_LocalWifiStatus = WIFI_STA_STATE_ENABLED;
        } else if (!strncmp((char *)arg, STATE_SCAN_COMPLETE, strlen(STATE_SCAN_COMPLETE))) {
//            g_LocalWifiStatus = WIFI_STA_STATE_SCANED;
            printf("----------- wifi sta scan complete\n");
        } else if (!strncmp((char *)arg, STATE_STA_CONNECTED, strlen(STATE_STA_CONNECTED))) {
            g_LocalWifiStatus = WIFI_STA_STATE_CONNECTED;
            printf("----------- wifi sta conneted\n");
        } else if (!strncmp((char *)arg, STATE_STA_DISCONNECTED, strlen(STATE_STA_DISCONNECTED))) {
            g_LocalWifiStatus = WIFI_STA_STATE_DISCONNECTED;
            printf("----------- wifi sta disconneted\n");
        } else if (!strncmp((char *)arg, STATE_STA_DISABLED, strlen(STATE_STA_DISABLED))) {
            g_LocalWifiStatus = WIFI_STA_STATE_DISABLED;
            printf("----------- wifi sta disabled\n");
        } else if (!strncmp((char *)arg, STATE_AP_STA_CONNECTED, strlen(STATE_AP_STA_CONNECTED))) {
            printf("----------- wifi ap sta connected\n");
        } else if (!strncmp((char *)arg, STATE_AP_STA_DISCONNECTED, strlen(STATE_AP_STA_DISCONNECTED))) {
            printf("----------- wifi ap sta disconnected\n");
        } else if (!strncmp((char *)arg, STATE_AP_ENABLED, strlen(STATE_AP_ENABLED))) {
//            g_LocalWifiStatus = WIFI_AP_STATE_ENABLED;
            printf("----------- wifi ap enabled\n");
        } else if (!strncmp((char *)arg, STATE_AP_DISABLED, strlen(STATE_AP_DISABLED))) {
//            g_LocalWifiStatus = WIFI_AP_STATE_DISABLED;
            printf("----------- wifi ap disabled\n");
        } else if (!strncmp((char *)arg, STATE_MONITOR_ENABLED, strlen(STATE_MONITOR_ENABLED))) {
//            g_LocalWifiStatus = WIFI_MONITOR_STATE_ENABLED;
            printf("----------- wifi monitor enabled\n");
        } else if (!strncmp((char *)arg, STATE_MONITOR_DISABLED, strlen(STATE_MONITOR_DISABLED))) {
//            g_LocalWifiStatus = WIFI_MONITOR_STATE_DISABLED;
            printf("----------- wifi monitor disabled\n");
        }
        /*!< 以下两个事件会通过arg带回sta的mac地址,应用程序如果需要记录每一个接入的sta,需自行保存。 */
    } else if (!strncmp(event, STATE_AP_STA_CONNECTED , strlen(STATE_AP_STA_CONNECTED))) {
        printf("----------- wifi ap sta connected, sta hwaddr is %s\n", (char *)arg);
    } else if (!strncmp(event, STATE_AP_STA_DISCONNECTED , strlen(STATE_AP_STA_DISCONNECTED))) {
        printf("----------- wifi ap sta disconnected, sta hwaddr is %s\n", (char *)arg);
    } else {
        printf("----------- invalid event\n");
    }

    return;
}

#if 0
int wifi_start_service(void (*state_cb)(char* event, void *arg))
{
	if(state_cb)
    {
		event_register_handler(WIFI_STATE_EVENT, 0, state_cb);
		event_register_handler(STATE_AP_STA_CONNECTED, 0, state_cb);
		event_register_handler(STATE_AP_STA_DISCONNECTED, 0, state_cb);
	}

	return 0;
}

int wifi_stop_service(void)
{
	printf("wifi manager stoped\n");
	return 0;
}
#endif

int QMAPI_Wireless_Init(void (*state_cb)(char* event, void *arg))
{
    int ret = -1;

    if (gWifiServiceCounter > 0)
    {
        printf("gWifiServiceCounter = [%d]\n", gWifiServiceCounter);
        return 0;
    }

    ret = wifi_start_service(qm_wifi_state_callback);
    if (ret != 0)
    {
        printf("wifi_start_service() failed\n");
        return -1;
    }

    gWifiServiceCounter ++;
    g_LocalWifiStatus = WIFI_STA_STATE_DISABLED;

    if (state_cb != NULL)
    {
        g_WiFiStateCB = state_cb;
    }

    return 0;
}


int QMAPI_Wireless_Deinit()
{
    int ret = -1;

    gWifiServiceCounter --;
    ret = wifi_stop_service();
    if (ret != 0)
    {
        printf("wifi_stop_service() failed\n");
        return -1;
    }

    return 0;

#if 0
    int ret = -1;

    /* 获取wifi当前的状态 */
    Wifi_State_t state = WIFI_STATE_UNKNOWN;
    state = wifi_get_state();

    /* 停止 STA 链接 */
    if(WIFI_STA_STATE_DISABLED <= state || WIFI_STA_STATE_DISCONNECTED >= state)
    {
        ret = wifi_sta_disconnect(NULL);
        if (ret != 0)
        {
            printf("wifi_sta_disconnect() failed\n");
            //return -1;
        }
#if 1
        /* 关闭wifi sta */
        ret = wifi_sta_disable();
        if (ret != 0)
        {
            printf("wifi_sta_disable() failed\n");
            return -1;
        }
#endif
    }

    /* 关闭wifi ap */
    if(WIFI_AP_STATE_ENABLED == state)
    {
        ret = wifi_ap_disable();
        if (ret != 0)
        {
            printf("wifi_ap_disable() failed\n");
            return -1;
        }
    }
#endif
}

int QMAPI_Wireless_Start(QMAPI_NET_WIFI_CONFIG *pstNetWifiConfig)
{
    int ret = -1;
    struct wifi_config_t stWifiConfig;
//    QMAPI_NET_WIFI_CONFIG *pstNetWifiConfig = &g_LocalWifiCfg;

    printf("#### %s %d!\n", __FUNCTION__, __LINE__);

    if (0 == pstNetWifiConfig->bWifiEnable)
        return -1;
    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
    memset(&stWifiConfig, 0, sizeof(stWifiConfig));

    /* Enable STA/AP */
    //printf("#### %s %d, bWifiEnable;%d\n", __FUNCTION__, __LINE__, pstNetWifiConfig->bWifiEnable);
    if (0 == pstNetWifiConfig->byWifiMode)          //STA
    {
        /* 打开wifi sta,如果是第一次打开wifi,会触发一次扫描 */
        ret = wifi_sta_enable();
        if (ret != 0)
        {
            printf("wifi_sta_enable() failed\n");
            return -1;
        }

/*
        while (g_LocalWifiStatus != WIFI_STA_STATE_ENABLED)
        {
            printf("Waiting WIFI_STA_STATE_ENABLED\n");
            sleep(1);
        }
*/
        memcpy(&g_LocalWifiCfg, pstNetWifiConfig, sizeof(QMAPI_NET_WIFI_CONFIG));
        memcpy(&g_LocalWifiCfg_bak, pstNetWifiConfig, sizeof(QMAPI_NET_WIFI_CONFIG));

        /* Connect */
        if (QMAPI_Wireless_Connect() == -1)
        {
            printf("#### %s %d, QMAPI_Wireless_Connect FAILED!\n", __FUNCTION__, __LINE__);
        }
    }
    else if (1 == pstNetWifiConfig->byWifiMode)     //AP
    {
        /* 设置ap模式下ap的ssid和passwd,打开ap模式之前需要先设置好, 需要设置以下三个参数
        * ssid * key_management * psk */
        /* key_management 目前支持 wpa2-psk / wpa-psk / open 三种模式*/
        struct wifi_config_t stWifiConfig;
        memset(&stWifiConfig, 0, sizeof(stWifiConfig));

        if (NULL != pstNetWifiConfig->csEssid
            && NULL != pstNetWifiConfig->csWebKey)
        {
            strncpy(stWifiConfig.ssid, pstNetWifiConfig->csEssid, strlen(pstNetWifiConfig->csEssid));
            strncpy(stWifiConfig.psk, pstNetWifiConfig->csWebKey, strlen(pstNetWifiConfig->csWebKey));
        }
        else
        {
            printf("essid psk is null!\n");
            return -1;
        }
        
        if (0 == pstNetWifiConfig->nSecurity)           //open
            strncpy(stWifiConfig.key_management, "open", strlen("open"));
        else if (7 == pstNetWifiConfig->nSecurity || 8 == pstNetWifiConfig->nSecurity)      //wpa2-psk
            strncpy(stWifiConfig.key_management, "wpa2-psk", strlen("wpa2-psk"));
        else if (5 == pstNetWifiConfig->nSecurity || 6 == pstNetWifiConfig->nSecurity)      //wpa-psk
            strncpy(stWifiConfig.key_management, "wpa-psk", strlen("wpa-psk"));
        else
        {
            strncpy(stWifiConfig.key_management, "wpa2-psk", strlen("wpa2-psk"));
            //printf("unsupport security !\n");
            //return -1;
        }

        ret = wifi_ap_set_config(&stWifiConfig);
        if (ret != 0)
        {
            printf("wifi_ap_set_config() failed\n");
            return -1;
        }

        /* 开启AP 模式 */
        if(1 == pstNetWifiConfig->byWifiMode)
        {
            /* 打开wifi ap,打开ap模式之前需要通过wifi_ap_set_config()先设置好ap的参数 */
            ret = wifi_ap_enable();
            if (ret != 0)
            {
                printf("wifi_ap_enable() failed\n");
                return -1;
            }
        }
    }

    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
    return 0;
}


int QMAPI_Wireless_Stop()
{
    int ret = -1;
    QMAPI_NET_WIFI_CONFIG *pstNetWifiConfig = &g_LocalWifiCfg_bak;
    unsigned char counter = 0;

    printf("#### %s %d!\n", __FUNCTION__, __LINE__);

    if (0 == pstNetWifiConfig->bWifiEnable)
    {
        return -1;
    }

    if (0 == pstNetWifiConfig->byWifiMode) /* STA */
    {
        if (QMAPI_Wireless_Disconnect() == -1)
        {
            printf("#### %s %d, QMAPI_Wireless_Disconnect FAILED!\n", __FUNCTION__, __LINE__);
        }

#if 1
        /* 关闭wifi sta */
        ret = wifi_sta_disable();
        if (ret != 0)
        {
            printf("wifi_sta_disable() failed\n");
            return -1;
        }

        while (g_LocalWifiStatus != WIFI_STA_STATE_DISABLED)
        {
            printf("waiting WiFi sta disable!\n");
            usleep(1000 * 1000);
            counter ++;

            if (counter > 10)
            {
                break;
            }
        }
#endif
    }

    if (1 == pstNetWifiConfig->byWifiMode) /* AP */
    {
        /* 关闭wifi ap */
        ret = wifi_ap_disable();
        if (ret != 0)
        {
            printf("wifi_ap_disable() failed\n");
            return -1;
        }
    }

    return 0;
}

int QMAPI_Wireless_Connect()
{
    int ret = -1;
    struct wifi_config_t stWifiConfig;
    QMAPI_NET_WIFI_CONFIG *pstNetWifiConfig = &g_LocalWifiCfg;

    if (0 == pstNetWifiConfig->bWifiEnable)
    {
        return -1;
    }
    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
    memset(&stWifiConfig, 0, sizeof(stWifiConfig));

    /* 连接指定的ap */
    if (0 == pstNetWifiConfig->byWifiMode)
    {
        if (0 == strlen(pstNetWifiConfig->csEssid) || 0 == strlen(pstNetWifiConfig->csWebKey))
        {
            printf("essid psk is null!\n");
            return -1;
        }

        strncpy(stWifiConfig.ssid, pstNetWifiConfig->csEssid, strlen(pstNetWifiConfig->csEssid));
        strncpy(stWifiConfig.psk, pstNetWifiConfig->csWebKey, strlen(pstNetWifiConfig->csWebKey));
        
        printf("#### %s %d, ssid:%s, psk:%s\n", __FUNCTION__, __LINE__, stWifiConfig.ssid, stWifiConfig.psk);

/*        while ((g_LocalWifiStatus != WIFI_STA_STATE_ENABLED) &&
                (g_LocalWifiStatus != WIFI_STA_STATE_DISCONNECTED))
        {
            printf("[%s]waiting enabled/disconnected\n", __FUNCTION__);
            sleep(1);
        }
*/
        ret = wifi_sta_connect(&stWifiConfig);

        if (ret != 0)
        {
            printf("wifi_sta_connect() failed\n");
            return -1;
        }

/*
        while (g_LocalWifiStatus != WIFI_STA_STATE_CONNECTED)
        {
            printf("Waiting WIFI_STA_STATE_CONNECTED\n");
            sleep(1);
        }
*/
    }

    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
    return 0;
}

int QMAPI_Wireless_Disconnect()
{
    int ret = -1;
    struct wifi_config_t stWifiConfig;
    QMAPI_NET_WIFI_CONFIG *pstNetWifiConfig = &g_LocalWifiCfg_bak;
    printf("#### %s %d!\n", __FUNCTION__, __LINE__);

    if (0 == pstNetWifiConfig->bWifiEnable)
    {
        printf("#### %s %d!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //printf("#### %s %d!\n", __FUNCTION__, __LINE__);
    //SystemCall_msg("rm -f /tmp/gw.wlan0", SYSTEM_MSG_NOBLOCK_RUN);
    //printf("#### %s %d!\n", __FUNCTION__, __LINE__);
    memset(&stWifiConfig, 0, sizeof(stWifiConfig));

    if (0 == pstNetWifiConfig->byWifiMode)
    {
/*        if (g_LocalWifiStatus != WIFI_STA_STATE_CONNECTED)
        {
            printf("--[%s]: g_LocalWifiStatus != WIFI_STA_STATE_CONNECTED\n", __FUNCTION__);
            return -1;
        }
*/

        /* 断开当前连接 */
        ret = wifi_sta_disconnect(NULL);
        if (ret != 0)
        {
            printf("wifi_sta_disconnect() failed\n");
            return -1;
        }

        /* 删除连接过的某个ap的信息 */
        strncpy(stWifiConfig.ssid, pstNetWifiConfig->csEssid, strlen(pstNetWifiConfig->csEssid));
        strncpy(stWifiConfig.psk, pstNetWifiConfig->csWebKey, strlen(pstNetWifiConfig->csWebKey));

/*        while (g_LocalWifiStatus != WIFI_STA_STATE_DISCONNECTED)
        {
            printf("Waiting WIFI_STA_STATE_DISCONNECTED\n");
            sleep(1);
        }
*/

        printf("--[%s]: wifi_sta_forget:[%s]\n", __FUNCTION__, stWifiConfig.ssid);

        if (strlen(stWifiConfig.ssid) == 0)
        {
            printf("There is nothing to forget!\n");
            return -1;
        }

        ret = wifi_sta_forget(&stWifiConfig);
        if (ret != 0)
        {
            printf("wifi_sta_forget() failed\n");
            return -1;
        }
    }

    return 0;
}

int QMAPI_Wireless_GetState(QMAPI_NET_WIFI_CONFIG *pstNetWifiConfig)
{
    int ret = -1;
    WIRELESS_STATUS_E enWirelessStatus;
    static struct wifi_config_t *wifi_config = NULL, *wifi_config_prev = NULL;

    //printf("#### %s %d, bWifiEnable=%d\n", __FUNCTION__, __LINE__, pstNetWifiConfig->bWifiEnable);
    if(1 == pstNetWifiConfig->bWifiEnable)
    {
#if 0
        /* 扫描周围的ap,在sta打开后关闭前的任何时间都可以扫描 */
        ret = wifi_sta_scan();
        if (ret != 0)
        {
            printf("wifi_sta_scan() failed\n");
            return -1;
        }
#endif
        /* 获取wifi当前的状态 */
        Wifi_State_t state = WIFI_STATE_UNKNOWN;
        state = wifi_get_state();
        show_wifi_state(state);

        switch(state)
        {
        	case WIFI_STATE_FAILED:
            case WIFI_STA_STATE_DISABLED:
        	case WIFI_STA_STATE_ENABLED:
        	case WIFI_STA_STATE_SCANED:
                enWirelessStatus = WL_NOT_CONNECT;
                break;
        	case WIFI_STA_STATE_CONNECTED:
                enWirelessStatus = WL_OK;
                break;
        	case WIFI_STA_STATE_DISCONNECTED:
        	case WIFI_AP_STATE_DISABLED:
                enWirelessStatus = WL_NOT_CONNECT;
                break;
        	case WIFI_AP_STATE_ENABLED:
                enWirelessStatus = WL_OK;
                break;
        	case WIFI_MONITOR_STATE_DISABLED:
        	case WIFI_MONITOR_STATE_ENABLED:
            default:
                enWirelessStatus = WL_NOT_CONNECT;
                break;
        }

        pstNetWifiConfig->byStatus = enWirelessStatus;
#if 0
        /* 获取sta模式下的连接信息 */
        wifi_config = wifi_sta_get_config();
        while (wifi_config != NULL)
        {
            show_wifi_config(wifi_config);
            wifi_config_prev = wifi_config;
            /* 如果当前 wifi_config 还有下一个配置信息，则返回下一个 wifi_config_t，否则返回 NULL
            * 注意: 调用成功会返回指针，用完后指针需要自行释放 */
            wifi_config = wifi_sta_iterate_config(wifi_config_prev);
            free(wifi_config_prev);
        }
#endif

#if 1
        /* 获取网络相关的信息 */
        struct net_info_t net_info;

        memset(&net_info, 0, sizeof(struct net_info_t));
        ret = wifi_get_net_info(&net_info);
        if (ret != 0)
        {
            printf("wifi_get_net_info() failed\n");
            return -1;
        }

        if (pstNetWifiConfig->byWifiMode == 0) /* STA Mode */
        {
            /* RM#2755: get net info because wifi_get_net_info() not work.    henry.li  2017/03/14 */
            if (GetIPGateWay("wlan0", net_info.gateway) == 0)
            {
                get_ip_addr("wlan0", net_info.ipaddr);
                get_mask_addr("wlan0", net_info.mask);
                get_dns_addr_ext_wlan(net_info.dns1, net_info.dns2);
            }
            else
            {
                strcpy(net_info.ipaddr, "0.0.0.0");
                strcpy(net_info.mask, "0.0.0.0");
                strcpy(net_info.dns1, "0.0.0.0");
            }
        }
        else /* AP Mode */
        {
            get_ip_addr("wlan0", net_info.ipaddr);
            get_mask_addr("wlan0", net_info.mask);
            strcpy(net_info.gateway, net_info.ipaddr);
            strcpy(net_info.dns1, net_info.ipaddr);
        }

        show_net_info(&net_info);

        strcpy(pstNetWifiConfig->dwNetIpAddr.csIpV4, net_info.ipaddr);
        strcpy(pstNetWifiConfig->dwGateway.csIpV4, net_info.gateway);
        strcpy(pstNetWifiConfig->dwNetMask.csIpV4, net_info.mask);
        strcpy(pstNetWifiConfig->dwDNSServer.csIpV4, net_info.dns1);

        printf("#### net_info.hwaddr:%s \n", net_info.hwaddr);
        //sscanf(net_info.hwaddr, "%02X:%02X:%02X:%02X:%02X:%02X",
        //        &pstNetWifiConfig->byMacAddr[0], &pstNetWifiConfig->byMacAddr[1], &pstNetWifiConfig->byMacAddr[2],
        //        &pstNetWifiConfig->byMacAddr[3], &pstNetWifiConfig->byMacAddr[4], &pstNetWifiConfig->byMacAddr[5]);
#endif
    }

    return 0;
}

int QMAPI_Wireless_IOCtrl(int Handle, int nCmd, int nChannel, void *Param, int nSize)
{
    int ret = 0;
    QMAPI_NET_WIFI_CONFIG *pstNetWifiConfig = &g_LocalWifiCfg;
    QMAPI_NET_WIFI_SITE_LIST stWifiSiteList;

    /** 目前仅支持以下命令字，之后会扩充相关的回调接口 **/
    if(QMAPI_SYSCFG_GET_WIFICFG != nCmd
        && QMAPI_SYSCFG_SET_WIFICFG != nCmd
        && QMAPI_SYSCFG_GET_WIFI_SITE_LIST != nCmd
        && QMAPI_SYSCFG_SET_WIFI_WPS_START != nCmd)
    {
        printf("<fun:%s>-<line:%d>, unspport cmd!\n", __FUNCTION__, __LINE__);
        return QMAPI_ERR_UNKNOW_COMMNAD;
    }

    switch(nCmd)
    {
        case QMAPI_SYSCFG_GET_WIFICFG:
            QMAPI_Wireless_GetState(pstNetWifiConfig);

            memcpy(Param, &g_LocalWifiCfg, sizeof(QMAPI_NET_WIFI_CONFIG));
            //printf("#### %s %d, csEssid=%s\n", __FUNCTION__, __LINE__, g_LocalWifiCfg.csEssid);
            break;
        case QMAPI_SYSCFG_SET_WIFICFG:
            if (NULL != Param)
            {
                memcpy(&g_LocalWifiCfg, Param, sizeof(QMAPI_NET_WIFI_CONFIG));
                //printf("#### %s %d, csEssid=%s\n", __FUNCTION__, __LINE__, g_LocalWifiCfg.csEssid);
            }

            /* RM#2883: set wifi from web setting.  henry.li  20170328 */
            if (0 != memcmp(&g_LocalWifiCfg_bak, &g_LocalWifiCfg, sizeof(QMAPI_NET_WIFI_CONFIG)))
            {
                /* Disable WiFi */
                if ((g_LocalWifiCfg_bak.bWifiEnable == 1) && (g_LocalWifiCfg.bWifiEnable == 0))
                {
                    printf("[%s]Disable WiFi\n", __FUNCTION__);
                    QMAPI_Wireless_Stop();
                    QMAPI_Wireless_Deinit();
                }
                /* Enable WiFi */
                else if ((g_LocalWifiCfg_bak.bWifiEnable == 0) && (g_LocalWifiCfg.bWifiEnable == 1))
                {
                    printf("[%s]Enable WiFi\n", __FUNCTION__);
                    QMAPI_Wireless_Init(NULL);
                    QMAPI_Wireless_Start(&g_LocalWifiCfg);
                }
                /* Set new WiFi params */
                else if ((g_LocalWifiCfg_bak.bWifiEnable == 1) && (g_LocalWifiCfg.bWifiEnable == 1))
                {
                    /*switch to AP mode*/
                    if ((g_LocalWifiCfg_bak.byWifiMode == 0) && (g_LocalWifiCfg.byWifiMode == 1))
                    {
                        printf("[%s]switch STA mode to AP mode!\n", __FUNCTION__);
                        /*disable old mode*/
                        QMAPI_Wireless_Stop();
                        /*start new mode*/
                        QMAPI_Wireless_Start(&g_LocalWifiCfg);
                    }
                    /*switch to STA mode*/
                    else if ((g_LocalWifiCfg_bak.byWifiMode == 1) && (g_LocalWifiCfg.byWifiMode == 0))
                    {
                        printf("[%s]switch AP mode to STA mode!\n", __FUNCTION__);
                        /*disable AP mode*/
                        QMAPI_Wireless_Stop();
                        /*start new mode*/
                        QMAPI_Wireless_Start(&g_LocalWifiCfg);
                    }
                    /*change AP mode params*/
                    else if ((g_LocalWifiCfg_bak.byWifiMode == 1) && (g_LocalWifiCfg.byWifiMode == 1))
                    {
                        printf("[%s]change AP mode params!\n", __FUNCTION__);
                        QMAPI_Wireless_Stop();
                        QMAPI_Wireless_Start(&g_LocalWifiCfg);
                    }
                    /*change STA mode params*/
                    else if ((g_LocalWifiCfg_bak.byWifiMode == 0) && (g_LocalWifiCfg.byWifiMode == 0))
                    {
                        /* Connect new SSID */ /* Use new pwd */ /* Use new security */
                        if ((strcmp(g_LocalWifiCfg_bak.csEssid, g_LocalWifiCfg.csEssid) != 0) ||
                            (strcmp(g_LocalWifiCfg_bak.csWebKey, g_LocalWifiCfg.csWebKey) != 0) ||
                            (g_LocalWifiCfg_bak.nSecurity != g_LocalWifiCfg.nSecurity))
                        {
                            printf("#### %s %d, New SSID=%s\n", __FUNCTION__, __LINE__, g_LocalWifiCfg.csEssid);
                            QMAPI_Wireless_Disconnect();
                            QMAPI_Wireless_Connect();
                        }
                    }
                    else
                    {
                    }
                }
                else
                {
                    printf("[%s]WiFi Disabled, do nothing\n", __FUNCTION__);
                }
            }

            memcpy(&g_LocalWifiCfg_bak, &g_LocalWifiCfg, sizeof(QMAPI_NET_WIFI_CONFIG));
            //printf("#### %s %d, csEssid=%s\n", __FUNCTION__, __LINE__, g_LocalWifiCfg.csEssid);
            break;
        case QMAPI_SYSCFG_GET_WIFI_SITE_LIST:
            QMAPI_Wireless_GetSiteList(&stWifiSiteList);

            memcpy(Param, &stWifiSiteList, stWifiSiteList.dwSize);
            break;
        case QMAPI_SYSCFG_SET_WIFI_WPS_START:
            break;
        default:
            printf("<fun:%s>-<line:%d>, unspport cmd!\n", __FUNCTION__, __LINE__);
            return QMAPI_ERR_UNKNOW_COMMNAD;
    }

    return ret;	
}

//0:STA 1:AP
int QMAPI_Wireless_SwitchMode(int nMode)
{
    int ret = -1;
    struct wifi_config_t stWifiConfig;
    QMAPI_NET_WIFI_CONFIG stNetWifiConfig;
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_WIFICFG, 0, &stNetWifiConfig, sizeof(QMAPI_NET_WIFI_CONFIG));

    if(nMode == stNetWifiConfig.byWifiMode)
        return 1;

    if(stNetWifiConfig.byWifiMode)  //STA -->> AP
    {
        /* 断开当前连接 */
        ret = wifi_sta_disconnect(NULL);
        if (ret != 0)
        {
            printf("wifi_sta_disconnect() failed\n");
            return -1;
        }

        /* 删除连接过的某个ap的信息 */
        memset(&stWifiConfig, 0, sizeof(stWifiConfig));
        strncpy(stWifiConfig.ssid, stNetWifiConfig.csEssid, strlen(stNetWifiConfig.csEssid));
        strncpy(stWifiConfig.psk, stNetWifiConfig.csWebKey, strlen(stNetWifiConfig.csWebKey));
        ret = wifi_sta_forget(&stWifiConfig);
        if (ret != 0)
        {
            printf("wifi_sta_forget() failed\n");
            return -1;
        }

        /* 关闭wifi sta */
        ret = wifi_sta_disable();
        if (ret != 0)
        {
            printf("wifi_sta_disable() failed\n");
            return -1;
        }

        /* 设置ap模式下ap的ssid和passwd,打开ap模式之前需要先设置好, 需要设置以下三个参数
        * ssid * key_management * psk */
        /* key_management 目前支持 wpa2-psk / wpa-psk / open 三种模式*/
        memset(&stWifiConfig, 0, sizeof(stWifiConfig));

        if(NULL != stNetWifiConfig.csEssid
            && NULL != stNetWifiConfig.csWebKey)
        {
            strncpy(stWifiConfig.ssid, stNetWifiConfig.csEssid, strlen(stNetWifiConfig.csEssid));
            strncpy(stWifiConfig.psk, stNetWifiConfig.csWebKey, strlen(stNetWifiConfig.csWebKey));
        }
        else
        {
            printf("essid psk is null!\n");
            return -1;
        }
        
        if(0 == stNetWifiConfig.nSecurity)           //open
            strncpy(stWifiConfig.key_management, "open", strlen("open"));
        else if(0 == stNetWifiConfig.nSecurity)      //wpa2-psk
            strncpy(stWifiConfig.key_management, "wpa2-psk", strlen("wpa2-psk"));
        else if(0 == stNetWifiConfig.nSecurity)      //wpa-psk
            strncpy(stWifiConfig.key_management, "wpa-psk", strlen("wpa-psk"));
        else{
            printf("unsupport security !\n");
            return -1;
        }

        ret = wifi_ap_set_config(&stWifiConfig);
        if (ret != 0)
        {
            printf("wifi_ap_set_config() failed\n");
            return -1;
        }

        /* 打开wifi ap,打开ap模式之前需要通过wifi_ap_set_config()先设置好ap的参数 */
        ret = wifi_ap_enable();
        if (ret != 0)
        {
            printf("wifi_ap_enable() failed\n");
            return -1;
        }
    }
    else if(0 == stNetWifiConfig.byWifiMode)  //AP -->> STA
    {
        /* 关闭wifi ap */
        ret = wifi_ap_disable();
        if (ret != 0)
        {
            printf("wifi_ap_disable() failed\n");
            return -1;
        }

        /* 打开wifi sta,如果是第一次打开wifi,会触发一次扫描 */
        ret = wifi_sta_enable();
        if (ret != 0)
        {
            printf("wifi_sta_enable() failed\n");
            return -1;
        }

        /* 连接指定的ap */
        if(NULL != stNetWifiConfig.csEssid && NULL != stNetWifiConfig.csWebKey)
        {
            strncpy(stWifiConfig.ssid, stNetWifiConfig.csEssid, strlen(stNetWifiConfig.csEssid));
            strncpy(stWifiConfig.psk, stNetWifiConfig.csWebKey, strlen(stNetWifiConfig.csWebKey));
        }
        else
        {
            printf("essid psk is null!\n");
            return -1;
        }
        ret = wifi_sta_connect(&stWifiConfig);
        if (ret != 0)
        {
            printf("wifi_sta_connect() failed\n");
            return -1;
        }
    }

    return 0;
}

int QMAPI_Wireless_GetSiteList(QMAPI_NET_WIFI_SITE_LIST *pstWifiSiteList)
{
    struct wifi_config_t *wifi_config = NULL, *wifi_config_prev = NULL;
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    memset(pstWifiSiteList, 0, sizeof(QMAPI_NET_WIFI_SITE_LIST));
#if 0
    /* 获取sta模式下的连接信息 */
    wifi_config = wifi_sta_get_config();
    while (wifi_config != NULL)
    {
        show_wifi_config(wifi_config);
        pstWifiSiteList->nCount ++;
        pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].dwSize = sizeof(QMAPI_NET_WIFI_SITE);
        strcpy(pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].csEssid, wifi_config->ssid);
        pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].nRSSI = wifi_config->level;
        strcpy(pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].byMac, wifi_config->ssid);

        wifi_config_prev = wifi_config;
        /* 如果当前 wifi_config 还有下一个配置信息，则返回下一个 wifi_config_t，否则返回 NULL
         * 注意: 调用成功会返回指针，用完后指针需要自行释放 */
        wifi_config = wifi_sta_iterate_config(wifi_config_prev);
        free(wifi_config_prev);
    }
#endif
    /* 获取扫描结果,在sta打开后关闭前的任何时间都可以获取。
     * 如果前面扫描到有可连接的AP，则返回一个指向 wifi_config_t 结构体的指针，否则返回 NULL */

    if (g_LocalWifiCfg.byWifiMode == 1)
        return -1;

    wifi_sta_scan();
    sleep(2);

    wifi_config = wifi_sta_get_scan_result();
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    while (wifi_config != NULL)
    {
        printf(">>>>> %-32s, signallevel:%d, key_management:%-32s, mac addr:%s\n",
                wifi_config->ssid, wifi_config->level, wifi_config->key_management, wifi_config->bssid);
        pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].dwSize = sizeof(QMAPI_NET_WIFI_SITE);
        strcpy(pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].csEssid, wifi_config->ssid);
        pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].nRSSI = wifi_config->level;
        sscanf(wifi_config->bssid, "%2x:%2x:%2x:%2x:%2x:%2x",
                pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].byMac,
                pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].byMac + 1,
                pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].byMac + 2,
                pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].byMac + 3,
                pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].byMac + 4,
                pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].byMac + 5
                );
        //strcpy(pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].byMac, wifi_config->bssid);

        if(0 == strncmp(wifi_config->key_management, "WPA-PSK-CCMP", strlen("WPA-PSK-CCMP")))
            pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].bySecurity = 6;    //WPAPSK-AES
        else if(0 == strncmp(wifi_config->key_management, "WPA-PSK-TKIP", strlen("WPA-PSK-TKIP")))
            pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].bySecurity = 5;    //WPAPSK-TKIP 
        else if(0 == strncmp(wifi_config->key_management, "WPA2-PSK-TKIP", strlen("WPA2-PSK-TKIP")))
            pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].bySecurity = 7;    //WPA2PSK-TKIP
        else if(0 == strncmp(wifi_config->key_management, "WPA2-PSK-CCMP", strlen("WPA2-PSK-CCMP")))
            pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].bySecurity = 8;    //WPA2PSK-AES
        else if(0 == strncmp(wifi_config->key_management, "WPS", strlen("WPS")))
            pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].bySecurity = 10;    //WPS
        else
            pstWifiSiteList->stWifiSite[pstWifiSiteList->nCount].bySecurity = 0;

        wifi_config_prev = wifi_config;
        /* 如果当前 wifi_config 还有下一个配置信息，则返回下一个 wifi_config_t，否则返回 NULL
         * 注意: 调用成功会返回指针，用完后指针需要自行释放 */
        wifi_config = wifi_sta_iterate_config(wifi_config_prev);
        free(wifi_config_prev);
        pstWifiSiteList->nCount ++;
    }
    pstWifiSiteList->dwSize = sizeof(QMAPI_NET_WIFI_SITE)*pstWifiSiteList->nCount;
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    return 0;
}

#if 0
void q3_wireless_start_swave()
{
	if(g_moduleExist == 0)
        return;
	
#ifdef JIUAN_SUPPORT
    if(g_SoundPairFlag == 0)
    {
        Audio_pause();
        printf("%s  %d, swave pair start!!\n",__FUNCTION__, __LINE__);
        int ret = startSwavePair(sound_pair_callback);
        if(ret == 0)
            g_SoundPairFlag = 1;
    }

    DMS_DEV_GPIO_SetWifiLed(LED_STATE_FAST_BLINK);
    TL_SysFun_Play_Audio(QMAPINOTIFY_TYPE_START_SONIC_MATCHING, 0);
#endif
    memset(&g_GlobalWifiCfg->dwNetIpAddr, 0, sizeof(QMAPI_NET_IPADDR));
    memset(&g_GlobalWifiCfg->dwNetMask, 0, sizeof(QMAPI_NET_IPADDR));
    memset(&g_GlobalWifiCfg->dwGateway, 0, sizeof(QMAPI_NET_IPADDR));
    memset(&g_GlobalWifiCfg->dwDNSServer, 0, sizeof(QMAPI_NET_IPADDR));
    memset(&g_GlobalWifiCfg->csEssid, 0, sizeof(g_GlobalWifiCfg->csEssid));
    memset(&g_GlobalWifiCfg->csWebKey, 0, sizeof(g_GlobalWifiCfg->csWebKey));
	//memset(&g_LocalWifiCfg, 0, sizeof(g_LocalWifiCfg));

	g_LocalWifiCfg.bWifiEnable = 0;
	g_LocalWifiCfg.byWifiMode = 0;

	g_LocalWifiCfg.csEssid[0] = 0;
	g_LocalWifiCfg.csWebKey[0] = 0;

	g_bSetValue = 1;
}
#endif


