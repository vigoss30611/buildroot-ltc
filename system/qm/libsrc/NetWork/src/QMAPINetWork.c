
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#include "tlnettools.h"
#include "QMAPINetWork.h"
#include "dhcp_mng.h"
#include "system_msg.h"

int QMAPI_SetNetWork(QMAPI_NET_NETWORK_CFG *pNetWorkCfg, CBDhcpAddr Callback)
{
    //BOOL        bSetDNSSuccessed = FALSE;
    int ret = 0;
	char cmdbuf[256] = {0};
	char* pDnsBackupPath = "/tmp/dns.eth0";
	char* pDnsPath = "/tmp/resolv.conf";
	char* pGateWayPath = "/tmp/gw.eth0";
     
	if(!pNetWorkCfg || pNetWorkCfg->dwSize != sizeof(QMAPI_NET_NETWORK_CFG))
	{
		return -1;
	}
	
    if(pNetWorkCfg->byEnableDHCP)
    {
		dhcpc_start(Callback);
    }
	else
	{
		dhcpc_stop();
       // if(0 != strcmp(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4, pNetWorkCfg->stEtherNet[0].strIPAddr.csIpV4))
		if (check_IP_Valid(pNetWorkCfg->stEtherNet[0].strIPAddr.csIpV4))
        {
            set_ip_addr(QMAPI_ETH_DEV, pNetWorkCfg->stEtherNet[0].strIPAddr.csIpV4);
            printf( "Change ip address!\n");
        }
       // if(0 != strcmp(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPMask.csIpV4, pNetWorkCfg->stEtherNet[0].strIPMask.csIpV4))
		if (check_IP_Valid(pNetWorkCfg->stEtherNet[0].strIPMask.csIpV4))
        {
            set_mask_addr(QMAPI_ETH_DEV, pNetWorkCfg->stEtherNet[0].strIPMask.csIpV4);
            printf( "Change mask!\n");
        }

		sprintf(cmdbuf, "echo %s > %s", pNetWorkCfg->stGatewayIpAddr.csIpV4, pGateWayPath);
		SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);
		/// Set GateWay IP
		#if 0
        DWORD dwGate = change_ip_string_value(pNetWorkCfg->stGatewayIpAddr.csIpV4);
        DWORD dwAddr = change_ip_string_value(pNetWorkCfg->stEtherNet[0].strIPAddr.csIpV4);
        DWORD dwMask = change_ip_string_value(pNetWorkCfg->stEtherNet[0].strIPMask.csIpV4);

        SetNetWorkGateWay(dwGate, dwAddr, dwMask, QMPAI_ETH_DEV);
		#endif

		// unsigned char wifistatus = 0;
		// q3_Wireless_GetState(&wifistatus);
        // if(wifistatus && 0 != strcmp(g_tl_globel_param.stNetworkCfg.stGatewayIpAddr.csIpV4, pNetWorkCfg->stGatewayIpAddr.csIpV4))
        // need fixme yi.zhang
        if (check_IP_Valid(pNetWorkCfg->stGatewayIpAddr.csIpV4))
        {
            //set_gateway_addr(ServerNetCfg->stGatewayIpAddr.csIpV4);
            printf( "Change gateway! %s\n",pNetWorkCfg->stGatewayIpAddr.csIpV4);
			//ret = set_gateway_netif(pNetWorkCfg->stGatewayIpAddr.csIpV4, QMAPI_ETH_DEV);
			sprintf(cmdbuf, "ip route replace default via %s dev %s", pNetWorkCfg->stGatewayIpAddr.csIpV4, QMAPI_ETH_DEV);
			ret = SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);
			if(ret)
			{
				printf("%s  %d, set_gateway_netif eth0 failed!!\n",__FUNCTION__, __LINE__);
				sprintf(cmdbuf, "route del default;route add default dev %s", QMAPI_ETH_DEV);
				SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);
			}
			
        }
        
		sprintf(cmdbuf, "echo %s > %s", pNetWorkCfg->stGatewayIpAddr.csIpV4, pGateWayPath);
		SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);

		/// Set DNS IP
		int len = 0;

		if(strlen(pNetWorkCfg->stDnsServer1IpAddr.csIpV4) >=7 
			&& strcmp(pNetWorkCfg->stDnsServer1IpAddr.csIpV4, "0.0.0.0") != 0)
		{
			len += sprintf(cmdbuf+len, "echo nameserver %s > %s;echo nameserver %s > %s;", 
				pNetWorkCfg->stDnsServer1IpAddr.csIpV4, pDnsBackupPath,
				pNetWorkCfg->stDnsServer1IpAddr.csIpV4, pDnsPath);
		}

		if(strlen(pNetWorkCfg->stDnsServer2IpAddr.csIpV4) >=7 
			&& strcmp(pNetWorkCfg->stDnsServer2IpAddr.csIpV4, "0.0.0.0") != 0)
		{
			if(len == 0)
				len += sprintf(cmdbuf+len, "echo nameserver %s > %s;echo nameserver %s > %s;", 
					pNetWorkCfg->stDnsServer2IpAddr.csIpV4, pDnsBackupPath,
					pNetWorkCfg->stDnsServer2IpAddr.csIpV4, pDnsPath);
			else
				len += sprintf(cmdbuf+len, "echo nameserver %s >> %s;echo nameserver %s >> %s;", 
					pNetWorkCfg->stDnsServer2IpAddr.csIpV4, pDnsBackupPath,
					pNetWorkCfg->stDnsServer2IpAddr.csIpV4, pDnsPath);
		}

		if(len == 0)
		{
			len += sprintf(cmdbuf+len, "echo nameserver 8.8.8.8 > %s;echo nameserver 8.8.8.8 > %s;", pDnsBackupPath, pDnsPath);
			strcpy(pNetWorkCfg->stDnsServer1IpAddr.csIpV4, "8.8.8.8");
		}

        SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);
	}
	    
    return 0; 
}


int QMAPI_InitNetWork(QMAPI_NET_NETWORK_CFG *pNetWorkCfg, CBDhcpAddr Callback)
{
    Char        mac_addr[24] ={0};
    BOOL        bSetDNSSuccessed = FALSE;
    int ret = 0;
	char cmdbuf[256] = {0};
	char* pDnsBackupPath = "/tmp/dns.eth0";
	char* pDnsPath = "/tmp/resolv.conf";
	char* pGateWayPath = "/tmp/gw.eth0";
     
    memset(mac_addr, 0, 24);
	
    ///get user set mac
    if(pNetWorkCfg->stEtherNet[0].byMACAddr[0] == 0 && pNetWorkCfg->stEtherNet[0].byMACAddr[1] == 0)
    {
        char mac[8] = {0};
        get_net_phyaddr(QMAPI_ETH_DEV, mac_addr, mac);
        printf("get mac:%02x %02x %02x %02x %02x %02x \n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
        memcpy(pNetWorkCfg->stEtherNet[0].byMACAddr, mac, 6);
    }
	else
	{
		sprintf(mac_addr, "%02x:%02x:%02x:%02x:%02x:%02x", pNetWorkCfg->stEtherNet[0].byMACAddr[0],
			pNetWorkCfg->stEtherNet[0].byMACAddr[1], pNetWorkCfg->stEtherNet[0].byMACAddr[2],
			pNetWorkCfg->stEtherNet[0].byMACAddr[3], pNetWorkCfg->stEtherNet[0].byMACAddr[4],
			pNetWorkCfg->stEtherNet[0].byMACAddr[5]);
		set_net_phyaddr(QMAPI_ETH_DEV, mac_addr);
	}

    if (pNetWorkCfg->byEnableDHCP)
    {
    	//首先设置默认IP
		if (check_IP_Valid(pNetWorkCfg->stEtherNet[0].strIPAddr.csIpV4))
        {
        	ret = set_ip_addr(QMAPI_ETH_DEV, pNetWorkCfg->stEtherNet[0].strIPAddr.csIpV4);
	        if(ret < 0)
	        {
	            printf("#### %s %d, set_ip_addr ret=%d\n", __FUNCTION__, __LINE__, ret);
	        }
		}
        /// Set Mask IP
        if (check_IP_Valid(pNetWorkCfg->stEtherNet[0].strIPMask.csIpV4))
        {
	        ret =  set_mask_addr(QMAPI_ETH_DEV, pNetWorkCfg->stEtherNet[0].strIPMask.csIpV4);
	        if(ret < 0)
	        {
	            printf("#### %s %d, set_mask_addr ret=%d\n", __FUNCTION__, __LINE__, ret);
	        }
        }
		
		#if 0
        DWORD dwGate = change_ip_string_value(pNetWorkCfg->stGatewayIpAddr.csIpV4);
        DWORD dwAddr = change_ip_string_value(pNetWorkCfg->stEtherNet[0].strIPAddr.csIpV4);
        DWORD dwMask = change_ip_string_value(pNetWorkCfg->stEtherNet[0].strIPMask.csIpV4);
        ret = SetNetWorkGateWay(dwGate, dwAddr, dwMask, ETH_DEV);
		#endif

		sprintf(cmdbuf, "ip route add default via %s dev %s", pNetWorkCfg->stGatewayIpAddr.csIpV4, QMAPI_ETH_DEV);
		SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);
		
		sprintf(cmdbuf, "echo %s > %s", pNetWorkCfg->stGatewayIpAddr.csIpV4, pGateWayPath);
		SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);

		sprintf(cmdbuf, "echo nameserver %s >> %s;echo nameserver %s >> %s;", 
				pNetWorkCfg->stDnsServer1IpAddr.csIpV4, pDnsBackupPath,
				pNetWorkCfg->stDnsServer1IpAddr.csIpV4, pDnsPath);
		SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);
		bSetDNSSuccessed = TRUE;

		dhcpc_start(Callback);
    }
	else
	{
        /// Set Network IP
        printf("%s  %d, wire:static mode.\n",__FUNCTION__, __LINE__);
        printf("SRDK:Device Ip Is: %s .Mask Is: %s #####################\n", pNetWorkCfg->stEtherNet[0].strIPAddr.csIpV4,pNetWorkCfg->stEtherNet[0].strIPMask.csIpV4);
		if (check_IP_Valid(pNetWorkCfg->stEtherNet[0].strIPAddr.csIpV4))
        {
        	ret = set_ip_addr(QMAPI_ETH_DEV, pNetWorkCfg->stEtherNet[0].strIPAddr.csIpV4);
	        if(ret < 0)
	        {
	            printf("#### %s %d, set_ip_addr ret=%d\n", __FUNCTION__, __LINE__, ret);
	        }
		}
        /// Set Mask IP
        if (check_IP_Valid(pNetWorkCfg->stEtherNet[0].strIPMask.csIpV4))
        {
	        ret =  set_mask_addr(QMAPI_ETH_DEV, pNetWorkCfg->stEtherNet[0].strIPMask.csIpV4);
	        if(ret < 0)
	        {
	            printf("#### %s %d, set_mask_addr ret=%d\n", __FUNCTION__, __LINE__, ret);
	        }
        }
       
		/// Set GateWay IP
        DWORD dwGate = change_ip_string_value(pNetWorkCfg->stGatewayIpAddr.csIpV4);
        DWORD dwAddr = change_ip_string_value(pNetWorkCfg->stEtherNet[0].strIPAddr.csIpV4);
        DWORD dwMask = change_ip_string_value(pNetWorkCfg->stEtherNet[0].strIPMask.csIpV4);

        printf("SRDK:Device GateWay Is: %s #####################\n", pNetWorkCfg->stGatewayIpAddr.csIpV4);
        SetNetWorkGateWay(dwGate, dwAddr, dwMask, QMAPI_ETH_DEV);
        
		sprintf(cmdbuf, "echo %s > %s", pNetWorkCfg->stGatewayIpAddr.csIpV4, pGateWayPath);
		SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);
	}
	
    if(bSetDNSSuccessed == FALSE)
    {
        /// Set DNS IP
		int len = 0;

		if(strlen(pNetWorkCfg->stDnsServer1IpAddr.csIpV4) >=7 && strcmp(pNetWorkCfg->stDnsServer1IpAddr.csIpV4, "0.0.0.0") != 0)
		{
			len += sprintf(cmdbuf+len, "echo nameserver %s > %s;echo nameserver %s > %s;", 
				pNetWorkCfg->stDnsServer1IpAddr.csIpV4, pDnsBackupPath,	pNetWorkCfg->stDnsServer1IpAddr.csIpV4, pDnsPath);
		}

		if(strlen(pNetWorkCfg->stDnsServer2IpAddr.csIpV4) >=7 && strcmp(pNetWorkCfg->stDnsServer2IpAddr.csIpV4, "0.0.0.0") != 0)
		{
			len += sprintf(cmdbuf+len, "echo nameserver %s >> %s;echo nameserver %s >> %s;", 
				pNetWorkCfg->stDnsServer2IpAddr.csIpV4, pDnsBackupPath,	pNetWorkCfg->stDnsServer2IpAddr.csIpV4, pDnsPath);
		}

		if(len == 0)
		{
			len += sprintf(cmdbuf+len, "echo nameserver 8.8.8.8 > %s;echo nameserver 8.8.8.8 > %s;", pDnsBackupPath, pDnsPath);
			strcpy(pNetWorkCfg->stDnsServer1IpAddr.csIpV4, "8.8.8.8");
		}

        SystemCall_msg(cmdbuf, SYSTEM_MSG_BLOCK_RUN);
    }
    
    //设置隐藏IP
    {
        SystemCall_msg("ifconfig eth0:1 192.168.168.168",SYSTEM_MSG_BLOCK_RUN);
    }
	
    return 1; 
}

