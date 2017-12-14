
#ifndef __QMAPI_NET_WORK_H__
#define __QMAPI_NET_WORK_H__

#include "QMAPI.h"

#define QMAPI_ETH_DEV   "eth0"

typedef int (*CBDhcpAddr)(char *IPAddr, char *IPMask, char *GatewayIpAddr, char *Dns1Addr, char *Dns2Addr);

/*************************************************************************
	初始化网络相关配置；IP地址 子网掩码 网关 DNS DHCP

	pNetWorkCfg [in]: 传入的参数
	Callback    [in]: DHCP 成功之后，需要更改网络配置参数，通过回调的方式告知应用层；只有开启DHCP的时候有用
*/
int QMAPI_InitNetWork(QMAPI_NET_NETWORK_CFG *pNetWorkCfg, CBDhcpAddr Callback);


/*************************************************************************
	修改网络相关配置；

	pNetWorkCfg [in]: 传入的参数
	Callback    [in]: DHCP 成功之后，需要更改网络配置参数，通过回调的方式告知应用层；只有开启DHCP的时候有用
*/
int QMAPI_SetNetWork(QMAPI_NET_NETWORK_CFG *pNetWorkCfg, CBDhcpAddr Callback);

#endif

