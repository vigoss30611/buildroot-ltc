#include "HBSearch.h"
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/prctl.h>
#include "GXThread.h"
#include "GXNetwork.h"
#include "GXConfig.h"
//#include "HBMessageLayer.h"

#include "DMSType.h"
#include "DMSNetSdk.h"
#include "DMSCommon.h"
#include "DMSErrno.h"
#include "Dms_sysnetapi.h"

//static HBSearchSession hbSession;
extern int g_nw_search_run_index;

int LoadDeviceBaseInfoPacket(char *buffer, int size)
{
	if (NULL == buffer || size <= 0) return -1;
	g_nw_search_run_index = 5091;

	// 1. get config param
	DMS_NET_DEVICE_INFO stDeviceInfo;
	DMS_NET_NETWORK_CFG stNetworkCfg;
	bzero(&stDeviceInfo, sizeof(DMS_NET_DEVICE_INFO));
	bzero(&stNetworkCfg, sizeof(DMS_NET_NETWORK_CFG));

	int nRet = dms_sysnetapi_ioctrl(0, DMS_NET_GET_DEVICECFG, 0, &stDeviceInfo, sizeof(stDeviceInfo));
	if (nRet < 0){
		GX_DEBUG("dms_sysnetapi_ioctrl() DMS_NET_GET_DEVICECFG failed. %d", nRet);
	}
	nRet = dms_sysnetapi_ioctrl(0, DMS_NET_GET_NETCFG, 0, &stNetworkCfg, sizeof(stNetworkCfg));
	if (nRet < 0){
		GX_DEBUG("dms_sysnetapi_ioctrl() DMS_NET_GET_NETCFG failed. %d", nRet);
	}
	g_nw_search_run_index = 5092;

	char *macAddr = (char *)stNetworkCfg.stEtherNet[0].byMACAddr;
	char *ipAddr = stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4;
	char *devName = stDeviceInfo.csDeviceName;
	char *devType = "IPC-HB-IPC291B-";
//	GX_DEBUG("mac %s ip %s devName is %s devType is %s", macAddr, ipAddr, devName, devType);
	g_nw_search_run_index = 5093;

	// 2. resqond
	HBDEVICEBASEINFO objDeviceBaseInfo; // = { 0 };
	memcpy(objDeviceBaseInfo.stHead.nfhead, HBSEARCHHEADID, strlen(HBSEARCHHEADID));
	objDeviceBaseInfo.stHead.cmd = 0x8a;
	memcpy(objDeviceBaseInfo.mac_addr, macAddr /*"abcdef"*/, 6);
	// 	objDeviceBaseInfo.mac_addr[0] = 0xe4;
	// 	objDeviceBaseInfo.mac_addr[1] = 0x7d;
	// 	objDeviceBaseInfo.mac_addr[2] = 0x5a;
	// 	objDeviceBaseInfo.mac_addr[3] = 0x15;
	// 	objDeviceBaseInfo.mac_addr[4] = 0x60;
	// 	objDeviceBaseInfo.mac_addr[5] = 0x2c;

	objDeviceBaseInfo.ip_addr = htonl(inet_addr(ipAddr));
	objDeviceBaseInfo.port = HB_SERVICE_PORT;
	objDeviceBaseInfo.dev_type = 0;				//8000T
	memcpy(objDeviceBaseInfo.host_id, devName, strlen(devName));
	memcpy(objDeviceBaseInfo.host_type, devType, strlen(devType));

	int nSize = sizeof(HBDEVICEBASEINFO);
	memcpy(buffer, &objDeviceBaseInfo, nSize);

	g_nw_search_run_index = 5094;

	return nSize;
}

int HBSearchCreateSocket(int *pSockType)
{
	if (pSockType)
	{
		*pSockType = 0;
	}
	
	return GXCreateSocketBrodcast("127.0.0.1", HB_SEARCH_MCAST_PORT);
}

///////////////////////////////////////////////////////////////////////////
int HBSearchRequestProc(int dmsHandle, int socketfd, int socktype)
{
	struct sockaddr_in    from_addr;
	memset(&from_addr, 0, sizeof(struct sockaddr_in));
	g_nw_search_run_index = 500;

	char    buff[BUFFER_LEN];
	//while (pSession->runflag)
	{
		g_nw_search_run_index = 501;
		long timeout = 0;
		int nRet = GXRecvfromPacket(socketfd, timeout, buff, BUFFER_LEN, &from_addr);
		if (nRet < 0){
//			GX_DEBUG(" recv data failed.");
			return 0;
		}
		g_nw_search_run_index = 506;

		if (strstr(buff, HBSEARCHHEADID))
		{
			g_nw_search_run_index = 507;
			HBSEARCHHEADCMD objCmd; // = { 0 };
			memcpy(&objCmd, buff, sizeof(HBSEARCHHEADCMD));
//			GX_DEBUG("recv msg: head=%s cmd=%x (%d)", objCmd.nfhead, objCmd.cmd, (int)objCmd.cmd);
			g_nw_search_run_index = 508;

			// handle and respond
			if (objCmd.cmd == 0xa8){
				g_nw_search_run_index = 509;
				memset(buff, 0, sizeof(buff));
				int nLen = LoadDeviceBaseInfoPacket(buff, BUFFER_LEN);
				if (nLen < 0){
					GX_DEBUG("LoadDeviceBaseInfoPacket() error. ret %d", nLen);
				}
				else{
					GXSendtoPacket(socketfd, &from_addr, 3, buff, nLen);
					//int length = GXSendtoPacket(socketfd, &from_addr, 3, buff, nLen);
					//GX_DEBUG("sendto() buf len %d\n", length);
					//length = -1;
				}
			}
			g_nw_search_run_index = 510;
		}
	}

	return 0;
}
