#include <errno.h>
#include <time.h>
#include <linux/if_ether.h>

#include "hik_search_protocol.h"
#include "hik_command.h"
#include "hik_netfun.h"
#include "QMAPIType.h"
#include "QMAPINetSdk.h"
extern int g_nw_search_run_index;

static uint16_t Hik_Checksum(uint16_t * buffer, int size)
{
    unsigned long cksum = 0;
    while(size > 1)
    {
        cksum += *buffer++;
        size -= sizeof(uint16_t);
    }
    if(size)
    {
        cksum += *(uint8_t *)buffer;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff); 	// 将高 16bit 与低 16bit 相加
    cksum += (cksum >> 16);            			// 将进位到高位的 16bit 与低 16bit 再相加

    return (uint16_t)(~cksum);
}

static int Hik_Search_Handle(int sockfd, char *buf, QMAPI_NET_NETWORK_CFG *stNetworkCfg, struct sockaddr_ll *device, char *buf_fix)
{
	int ret = -1,num = 0;
	unsigned short cksum;
	time_t timep;
	struct tm *p;

	NET_CLIENT_BROADCAST stClientData;
	NET_BROADCAST stBroadcastData;
	MAC_FRAME_HEADER stFrameHeader;

	QMAPI_NET_DEVICE_INFO stDeviceInfo;
	g_nw_search_run_index = 204;
    printf("########################## %s %d\n", __FUNCTION__, __LINE__);

	memset(&stClientData, 0, sizeof(stClientData));
	memset(&stBroadcastData, 0, sizeof(stBroadcastData));
	memset(&stFrameHeader, 0, sizeof(stFrameHeader));
	
	memcpy(&stClientData, buf + 14, sizeof(stClientData));
	g_nw_search_run_index = 205;

#if 1 //当不确定是否有搜索命令发过来时，可以将此处打开
	printf("AAA1#### %s %d, %X:%X:%X:%X:%X:%X, %X:%X:%X:%X:%X:%X\n", __FUNCTION__, __LINE__,
		   buf[0], buf[1], buf[2], buf[3], buf[4], buf[5],
		   buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);

	printf("AAA2#### %s %d, %X:%X:%X:%X:%X:%X, %X:%X:%X:%X:%X:%X\n",
				__FUNCTION__, __LINE__,
				stClientData.m_SrcMac[0], stClientData.m_SrcMac[1], stClientData.m_SrcMac[2],
				stClientData.m_SrcMac[3], stClientData.m_SrcMac[4], stClientData.m_SrcMac[5],
				stClientData.m_DstMac[0], stClientData.m_DstMac[1], stClientData.m_DstMac[2],
				stClientData.m_DstMac[3], stClientData.m_DstMac[4], stClientData.m_DstMac[5]);
#endif

	if(0xff == stClientData.m_DstMac[0] && 0xff == stClientData.m_DstMac[1] && 0xff == stClientData.m_DstMac[2]
		&& 0xff == stClientData.m_DstMac[3] && 0xff == stClientData.m_DstMac[4] && 0xff == stClientData.m_DstMac[5])
	{
		//HPRINT_INFO("ipaddr:%s \n", stNetworkCfg->stEtherNet[0].strIPAddr.csIpV4);
		g_nw_search_run_index = 206;

		memset(stFrameHeader.m_cDstMacAddress, 0xff, 6);	
		memcpy(stFrameHeader.m_cSrcMacAddress, stNetworkCfg->stEtherNet[0].byMACAddr, 6);
		stFrameHeader.m_cType = htons(0x8033);

		stBroadcastData.m_ServiceType = htonl(0x210101f6);
		stBroadcastData.m_Seqnum = stClientData.m_Seqnum;
		stBroadcastData.m_CmdType = htonl(0x06040400);

		memcpy(stBroadcastData.m_SrcMac, stNetworkCfg->stEtherNet[0].byMACAddr, 6);
		stBroadcastData.m_SrcIp = inet_addr(stNetworkCfg->stEtherNet[0].strIPAddr.csIpV4);

		memset(stBroadcastData.m_DstMac, 0xff, 6);
		stBroadcastData.m_SrcNetmask = inet_addr(stNetworkCfg->stEtherNet[0].strIPMask.csIpV4);

		memset(&stDeviceInfo,0,sizeof(stDeviceInfo));
		ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, &stDeviceInfo, sizeof(stDeviceInfo)); 
		if(0 != ret)
		{
			HPRINT_ERR("\n");
			return -1;		
		}
		sprintf(stBroadcastData.m_DevSerialNum,"C001-JBNV%X",(unsigned int)stDeviceInfo.csSerialNumber);
		stBroadcastData.m_unknown1 = htonl(0x9a09);
		stBroadcastData.m_ServicePort = htonl(8000);
		stBroadcastData.m_ChannelNum = htonl(1);

#if 0
		sprintf(stBroadcastData.m_SoftwareVersion, "V%ld.%ld.%ld.%ld build %ld%ld%ld%ld%ld%ld",
					(stDeviceInfo.dwSoftwareVersion>>24)&0xff, (stDeviceInfo.dwSoftwareVersion>>16)&0xff,
					(stDeviceInfo.dwSoftwareVersion>>8)&0xff, stDeviceInfo.dwSoftwareVersion&0xff, 
					(stDeviceInfo.dwSoftwareBuildDate>>20)&0xf, (stDeviceInfo.dwSoftwareBuildDate>>16)&0xf,
					(stDeviceInfo.dwSoftwareBuildDate>>12)&0xf, (stDeviceInfo.dwSoftwareBuildDate>>8)&0xf,
					(stDeviceInfo.dwSoftwareBuildDate>>4)&0xf, stDeviceInfo.dwSoftwareBuildDate%0xf);

		sprintf(stBroadcastData.m_DspVersion, "V4.0, build %ld%ld%ld%ld%ld%ld",
					(stDeviceInfo.dwHardwareBuildDate>>20)&0xf, (stDeviceInfo.dwHardwareBuildDate>>16)&0xf,
					(stDeviceInfo.dwHardwareBuildDate>>12)&0xf, (stDeviceInfo.dwHardwareBuildDate>>8)&0xf,
					(stDeviceInfo.dwHardwareBuildDate>>4)&0xf, stDeviceInfo.dwHardwareBuildDate&0xf);

		time(&timep);
		p = gmtime(&timep);
		sprintf(stBroadcastData.m_Starttime, "%02d-%02d-%02d %02d:%02d:%02d",
					p->tm_year+1900, p->tm_mon+1, p->tm_mday, p->tm_hour+8, p->tm_min, p->tm_sec);

#else
		strcpy(stBroadcastData.m_SoftwareVersion, "V4.1.3 build 130925");
		strcpy(stBroadcastData.m_DspVersion, "V4.0, build 130800");
		strcpy(stBroadcastData.m_Starttime, "2015-09-02 12:01:49"); 	//"2015-06-11 18:43:02"
#endif
		stBroadcastData.m_unknown3 = htonl(0x029ce3a1);
		stBroadcastData.m_SrcGateway = inet_addr(stNetworkCfg->stGatewayIpAddr.csIpV4);
		stBroadcastData.m_unknown4 = htonl(0x070000);
		stBroadcastData.m_unknown5 = htonl(0x500000);
		memcpy(stBroadcastData.m_DeviceType, "CS-C1-11WPFR", strlen("CS-C1-11WPFR"));
		
		cksum = Hik_Checksum((uint16_t *)&stBroadcastData, sizeof(stBroadcastData) - 156);
		stBroadcastData.m_Checksum = (cksum >> 8) | ((cksum & 0xff) << 8);

		memset(buf, 0, 4096);	//临时解决编译问题
		memcpy(buf, &stFrameHeader, sizeof(stFrameHeader));
		memcpy(buf+sizeof(stFrameHeader), &stBroadcastData, sizeof(stBroadcastData));

		memset(buf_fix, 0, 1024);	//临时解决编译问题
		memcpy(buf_fix, &stFrameHeader, sizeof(stFrameHeader));
		memcpy(buf_fix + sizeof(stFrameHeader), &stBroadcastData, sizeof(stBroadcastData));
		num = sendto(sockfd, buf, sizeof(stFrameHeader)+sizeof(stBroadcastData), 0, (struct sockaddr*)device, sizeof(struct sockaddr_ll));
	}
	g_nw_search_run_index = 207;

	return num;
}

static int Hik_IPModify_Handle(int sockfd, char *buf, QMAPI_NET_NETWORK_CFG *stNetworkCfg, struct sockaddr_ll *device, char *buf_fix)
{
	int ret = -1;
	unsigned short cksum;
	unsigned int nip[4]={0},nmask[4]={0};
	
	MAC_FRAME_HEADER stFrameHeader;
	NET_BROADCAST_FIX_IP stIPtData;			

	memset(&stIPtData, 0, sizeof(stIPtData));
	memset(&stFrameHeader, 0, sizeof(stFrameHeader));

	memcpy(&stFrameHeader, buf, sizeof(stFrameHeader));
	memcpy(&stIPtData, buf + 14, sizeof(stIPtData));
	
	if(strncmp(stIPtData.m_DstMac, (char *)stNetworkCfg->stEtherNet[0].byMACAddr, 6) == 0)
	{
		struct in_addr addrD;	
		addrD.s_addr = ((unsigned int)stIPtData.m_DstIp);
		printf("dst ip:%s \n", inet_ntoa(addrD));				
		strcpy(stNetworkCfg->stEtherNet[0].strIPAddr.csIpV4, inet_ntoa(addrD));
		
		struct in_addr addrMask;	
		addrMask.s_addr = (stIPtData.m_DstMaskip);
		printf("mask ip:%s \n", inet_ntoa(addrMask));
		strcpy(stNetworkCfg->stEtherNet[0].strIPMask.csIpV4, inet_ntoa(addrMask));

		printf("src gateway:%s\n",stNetworkCfg->stGatewayIpAddr.csIpV4);
		sscanf(stNetworkCfg->stEtherNet[0].strIPAddr.csIpV4,"%u.%u.%u.%u",&nip[0],&nip[1],&nip[2],&nip[3]);
		sscanf(stNetworkCfg->stEtherNet[0].strIPMask.csIpV4,"%u.%u.%u.%u",&nmask[0],&nmask[1],&nmask[2],&nmask[3]);
		memset(stNetworkCfg->stGatewayIpAddr.csIpV4,0,sizeof(stNetworkCfg->stGatewayIpAddr.csIpV4));
		sprintf(stNetworkCfg->stGatewayIpAddr.csIpV4,"%u.%u.%u.1",nip[0]&nmask[0],nip[1]&nmask[1],nip[2]&nmask[2]);
		printf("dst gateway:%s\n",stNetworkCfg->stGatewayIpAddr.csIpV4);

		MAC_FRAME_HEADER *pstFrameHeader = (MAC_FRAME_HEADER *)buf_fix;
		memcpy(pstFrameHeader->m_cDstMacAddress, stIPtData.m_SrcMac, 6);

		NET_BROADCAST *pstBroadcastData = (NET_BROADCAST *)(buf_fix + sizeof(stFrameHeader));
		pstBroadcastData->m_CmdType = htonl(0x06040702);
		pstBroadcastData->m_SrcIp = stIPtData.m_DstIp;
		pstBroadcastData->m_Seqnum = stIPtData.m_Seqnum;

		cksum = Hik_Checksum((uint16_t *)pstBroadcastData, sizeof(NET_BROADCAST)-156);
		pstBroadcastData->m_Checksum = (cksum >> 8) | ((cksum & 0xff) << 8);
		sendto(sockfd, buf_fix, sizeof(stFrameHeader)+sizeof(NET_BROADCAST), 0, (struct sockaddr*)device, sizeof(struct sockaddr_ll));

		stNetworkCfg->byEnableDHCP = 0;
		ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_NETCFG, 0, stNetworkCfg, sizeof(QMAPI_NET_NETWORK_CFG)); 
		ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, 0, NULL, 0);
	}
	else
	{
		printf("HKvs-not local device.\n");
	}			

	return ret;
}

int HikSearchCreateSocket(int *pSockType)
{
    printf("########################## %s %d\n", __FUNCTION__, __LINE__);
	int sockfd = socket(PF_PACKET, SOCK_RAW, htons(0x8033));	//0x8033 属于HIK私有网络协议类型，使用该类型可有效降低系统资源消耗
	if(sockfd<0)
	{  
		perror("Creatingsocket failed.");  
		return -1;
	}

	if (*pSockType)
	{
		*pSockType = 1;
	}
    printf("########################## %s %d\n", __FUNCTION__, __LINE__);
	return sockfd;
}

int HikSearchRequsetProc(int dmsHandle, int sockfd, int sockType)
{	
	struct sockaddr_in clinet;
    char buf[4096];
	char buf_fix[1024];
    int num,ret = 0;
	struct sockaddr_ll device;
	QMAPI_NET_NETWORK_CFG stNetworkCfg;

	bzero(&clinet, sizeof(clinet));	
	socklen_t addrlen=sizeof(struct sockaddr_in);  
	memset(buf, 0, 4096);
	memset(buf_fix, 0, 1024);
	g_nw_search_run_index = 200;
    printf("########################## %s %d\n", __FUNCTION__, __LINE__);
	num = recvfrom(sockfd, buf, 4096, 0, (struct sockaddr*)&clinet, &addrlen); 									
	if (num <= 0)  
	{  
		if(errno == EINTR || errno == EAGAIN)
		{
			HPRINT_INFO("start recv 1\n");
			return 0;
		}		
		printf("%s	%d, recvfrom error: %s(%d)\n",__FUNCTION__, __LINE__, strerror(errno),errno);
	}
	g_nw_search_run_index = 201;
	//HPRINT_INFO("recv len:%d \n",num);
	memset(&stNetworkCfg,0,sizeof(stNetworkCfg));
	ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, 0, &stNetworkCfg, sizeof(stNetworkCfg)); 
	if(0 != ret)
	{
		HPRINT_ERR("\n");
		return 0;		
	}

	memset(&device, 0, sizeof(device));
	if((device.sll_ifindex = if_nametoindex("eth0")) == 0)
	{
		perror("if_nametoindex() failed to obtain interface index ");
		return 0;
	}

	device.sll_family = AF_PACKET;
	memcpy(device.sll_addr,stNetworkCfg.stEtherNet[0].byMACAddr,6);				
	device.sll_halen = htons(6);
	g_nw_search_run_index = 202;
    printf("########################## %s %d, num=%d\n", __FUNCTION__, __LINE__, num);
	//if(num == 80) //搜索处理
	if(num == 84) //搜索处理    //fix peter, 2017.1.8
	{
		g_nw_search_run_index = 203;
        printf("########################## %s %d\n", __FUNCTION__, __LINE__);
		ret = Hik_Search_Handle(sockfd,buf,&stNetworkCfg,&device,buf_fix);	
		if(ret < 0)
		{
			HPRINT_ERR("search handle faile\n");
			return 0;
		}
	}
	if(236 == num) // modify ip address,
	{
        printf("########################## %s %d\n", __FUNCTION__, __LINE__);
		ret = Hik_IPModify_Handle(sockfd,buf,&stNetworkCfg,&device,buf_fix);
		if(ret < 0)
		{
			HPRINT_ERR("modify ip address faile\n");
			return 0;
		}
	}

    return 0;
}

