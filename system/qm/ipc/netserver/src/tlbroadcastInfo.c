
#include <sys/socket.h>
#include <netinet/in.h>
#include "tl_common.h"
#include "sysconfig.h"
#include "QMAPINetWork.h"
#include "TLNetCommon.h"

//#include "tlmisc.h"

//#include "bitoperation.h"
#ifdef  FUNCTION_UMSP_SUPPORT
#include "U1Net.h"
#endif

#define WIFI_DEV            "ce00"

#define     BRD_SET_SERVER_INFO     0x30000001
#define     BRD_GET_SERVER_INFO     0x40000001

#define     BRD_SEARCH_DEVICE       0x03404325
#define CMD_SET_MAC_ADDRESS                         0x000A0010

#define MULTI_BROADCAST_SREACH_IPADDR			"239.238.237.236"
//#define MULTI_BROADCAST_SREACH_SEND_PORT		0x9887
//#define MULTI_BROADCAST_SREACH_RECV_PORT		0x8887
//#define MULTI_BROADCAST_SREACH_RECV_PORT_ADD     0x7778
#define MULTI_BROADCAST_SREACH_SEND_PORT		17315
#define MULTI_BROADCAST_SREACH_RECV_PORT		17314
#define MULTI_BROADCAST_SREACH_RECV_PORT_ADD     0x7777


extern int get_dns_addr(char *address);

//extern int   g_bWifiStarted;    ///2009.6.15 zhang added
extern int get_net_phyaddr(char *name, char *addr , char *bMac) ;
extern int tl_get_pppoe_ipaddr(char *ipaddr);

int DoServerInfoBroadcast(int Broadcastfd);

pthread_mutex_t     broadcast_mutex;
int g_BroadSock = -1;
int g_BroadAddSock = -1;

typedef struct
{
        DWORD                             dwSize;
    DWORD               nCmd;
    DWORD               dwPackFlag; // == SERVER_PACK_FLAG
    DWORD               nErrorCode;
}TLNV_BROADCAST_HEADER;

//this is for servertools' "set network's addr" 
typedef struct tagTL_SERVER_SET_INFO{
    DWORD   dwSize;
    DWORD     dwIp;             //Server Ip
    DWORD   dwMediaPort;        //Media Port:8200
    DWORD   dwWebPort;      //Http Port:80

    DWORD   dwNetMask;
    DWORD   dwGateway;
    DWORD   dwDNS;
    DWORD   dwComputerIP;
    BOOL            bEnableDHCP;    
    BOOL            bEnableAutoDNS;
    BOOL            bEncodeAudio;

    char          szoldMac[6];
    char            szMac[6];
    char        szServerName[32];   
}TLNV_SET_SIGHT_SERVER_INFO;


//this is the broadcast package over the network
typedef struct tagTLNV_SERVER_INFO_BROADCAST{
        DWORD                                                   nBufSize;       /*sieze of TLNV_SET_SERVER_INFO_BROADCAST*/
    TLNV_BROADCAST_HEADER               hdr;
    TLNV_SET_SIGHT_SERVER_INFO                  setInfo;
    TLNV_NXSIGHT_SERVER_ADDR                nxServer;
}TLNV_SET_SERVER_INFO_BROADCAST;

typedef struct tagTLNV_SERVER_INFO_BROADCAST_V2{
        DWORD                                                   nBufSize;       /*sieze of TLNV_SET_SERVER_INFO_BROADCAST*/
    TLNV_BROADCAST_HEADER               hdr;
    TLNV_SET_SIGHT_SERVER_INFO                  setInfo;
    TLNV_NXSIGHT_SERVER_ADDR_V2           nxServer;
}TLNV_SET_SERVER_INFO_BROADCAST_V2;

typedef struct tagTLNV_SERVER_PACK_EX
{
    TLNV_SERVER_PACK jspack;
    BYTE            bMac[6];
    BOOL            bEnableDHCP;    
    BOOL        bEnableDNS;
    DWORD   dwNetMask;
    DWORD   dwGateway;
    DWORD   dwDNS;
    DWORD   dwComputerIP;   
    BOOL            bEnableCenter;
    DWORD   dwCenterIpAddress;
    DWORD   dwCenterPort;
    char        csServerNo[64];
    int     bEncodeAudio;
}TLNV_SERVER_PACK_EX;


typedef struct tagTLNV_SERVER_MSG_DATA_EX
{
    DWORD                   dwSize; // >= sizeof(TLNV_SERVER_PACK) + 8;
    DWORD                   dwPackFlag; // == SERVER_PACK_FLAG
    TLNV_SERVER_PACK_EX     tlServerPack;
}TLNV_SERVER_MSG_DATA_EX;

///luo add,thees structs I just use it for send the answer back to servertools
typedef struct tagTLNV_SERVER_PACK_EX_V2
{
    TLNV_SERVER_PACK jspack;
    BYTE            bMac[6];
    BOOL            bEnableDHCP;    
    BOOL        bEnableDNS;
    DWORD   dwNetMask;
    DWORD   dwGateway;
    DWORD   dwDNS;
    DWORD   dwComputerIP;   
    BOOL            bEnableCenter;
    DWORD   dwCenterIpAddress;
    DWORD   dwCenterPort;
    char        csServerNo[64];
    int     bEncodeAudio;
    char    csCenterIpAddress[64];
}TLNV_SERVER_PACK_EX_V2;


typedef struct tagTLNV_SERVER_MSG_DATA_EX_V2
{
    DWORD                   dwSize; // >= sizeof(TLNV_SERVER_PACK) + 8;
    DWORD                   dwPackFlag; // == SERVER_PACK_FLAG
    TLNV_SERVER_PACK_EX_V2  tlServerPack;
}TLNV_SERVER_MSG_DATA_EX_V2;

typedef struct tagTLNV_SERVER_PACK_EX_V3
{
	TLNV_SERVER_PACK_EX_V2 jspackex_v2;
	DWORD	dwVerYear;			//版本打包时间、年
	DWORD	dwVerMonth;			//版本打包时间、月
	DWORD	dwVerDay;			//版本打包时间、日
	DWORD	dwRunHour;			//设备运行时间、小时		
	DWORD	dwRunMin;			//设备运行时间、分钟
	DWORD	dwMemTotal;			//设备内存总大小(单位K)
	DWORD	dwMemFree;			//设备内存剩余大小(单位K)
	DWORD	dwFlaTotal;			//flash总大小(单位K)
	DWORD	dwFlaFree;			//flash剩余大小(单位K)
	FLOAT	fCPUutilization;	//CPU占用率
	DWORD	dwSensorType;		//sensor类型
	DWORD	dwFdnum;			//fd个数
	DWORD	dwTasknum;			//task个数
	DWORD	dwNetConum;			//IE客户端连接个数
	char	csBuildInfo[36];	//设备组件信息
	DWORD	dwCPUType;		//主芯片类型
	char	csRes[88];			//保留字节
}TLNV_SERVER_PACK_EX_V3;

typedef struct tagTLNV_SERVER_MSG_DATA_EX_V3
{
    DWORD                   dwSize; 	//结构体大小
    DWORD                   dwPackFlag; // == SERVER_PACK_FLAG
	TLNV_SERVER_PACK_EX_V3	jspackex_v3;
}TLNV_SERVER_MSG_DATA_EX_V3;


typedef struct tagTLNV_BROADCAST_PACK{
    DWORD   dwFlag;
    DWORD   dwServerIpaddr;
    WORD    wServerPort;
    WORD    wBufSize;
    DWORD   dwCommand;
    DWORD   dwChannel;
}TLNV_BROADCAST_PACK; 

typedef struct tagTLNV_MC_PACK{
    DWORD   dwFlag;
    DWORD   dwServerIpaddr;
    WORD    wServerPort;
    WORD    wBufSize;
    DWORD   dwCommand;
    DWORD   dwChannel;
    unsigned char mac[16];
}TLNV_MC_PACK; 


///receive boradcast package to set device's ip addr and so on
void BroadcastSetIpaddr(TLNV_SET_SERVER_INFO_BROADCAST_V2 * tlBroadcastSetInfo)
{
    struct in_addr	NetIpAddr;
#if 1
    printf("tlBroadcastSetInfo->nBufSize == %lu\n", tlBroadcastSetInfo->nBufSize);

    printf("tlBroadcastSetInfo->hdr.dwSize == %lu\n", tlBroadcastSetInfo->hdr.dwSize);
    printf("tlBroadcastSetInfo->hdr.nCmd == %lu\n", tlBroadcastSetInfo->hdr.nCmd);
    printf("tlBroadcastSetInfo->hdr.dwPackFlag == %lu\n", tlBroadcastSetInfo->hdr.dwPackFlag);
    printf("tlBroadcastSetInfo->hdr.nErrorCode == %lu\n", tlBroadcastSetInfo->hdr.nErrorCode);

    printf("tlBroadcastSetInfo->setInfo.dwSize == %lu\n", tlBroadcastSetInfo->setInfo.dwSize);
    printf("tlBroadcastSetInfo->setInfo.dwIp == %lu\n", tlBroadcastSetInfo->setInfo.dwIp);
    printf("tlBroadcastSetInfo->setInfo.wMediaPort == %lu\n", tlBroadcastSetInfo->setInfo.dwMediaPort);
    printf("tlBroadcastSetInfo->setInfo.wWebPort == %lu\n", tlBroadcastSetInfo->setInfo.dwWebPort);
    printf("tlBroadcastSetInfo->setInfo.dwNetMask == %lu\n", tlBroadcastSetInfo->setInfo.dwNetMask);
    printf("tlBroadcastSetInfo->setInfo.dwGateway== %lu\n", tlBroadcastSetInfo->setInfo.dwGateway);
    printf("tlBroadcastSetInfo->setInfo.dwDNS == %lu\n", tlBroadcastSetInfo->setInfo.dwDNS);

    printf("tlBroadcastSetInfo->setInfo.szoldMac == %s\n", tlBroadcastSetInfo->setInfo.szoldMac);
    printf("tlBroadcastSetInfo->setInfo.szMac == %s\n", tlBroadcastSetInfo->setInfo.szMac);
    printf("tlBroadcastSetInfo->setInfo.szServerName == %s\n", tlBroadcastSetInfo->setInfo.szServerName);
    
    printf("tlBroadcastSetInfo->setInfo.bEnableDHCP == %d\n", tlBroadcastSetInfo->setInfo.bEnableDHCP);
    printf("tlBroadcastSetInfo->setInfo.bEnableAutoDNS == %d\n", tlBroadcastSetInfo->setInfo.bEnableAutoDNS);   
    printf("tlBroadcastSetInfo->setInfo.bEncodeAudio == %d\n", tlBroadcastSetInfo->setInfo.bEncodeAudio);

    printf("tlBroadcastSetInfo->nxServer.dwSize == %lu\n", tlBroadcastSetInfo->nxServer.dwSize);
    printf("tlBroadcastSetInfo->nxServer.bEnable == %d\n", tlBroadcastSetInfo->nxServer.bEnable);
    printf("tlBroadcastSetInfo->nxServer.dwCenterIp == %lu\n", tlBroadcastSetInfo->nxServer.dwCenterIp);
    printf("tlBroadcastSetInfo->nxServer.wCenterPort == %lu\n", tlBroadcastSetInfo->nxServer.wCenterPort);
    printf("tlBroadcastSetInfo->nxServer.csServerNo == %s\n", tlBroadcastSetInfo->nxServer.csServerNo); 
#endif
    NetIpAddr.s_addr=tlBroadcastSetInfo->setInfo.dwIp;
    strcpy(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4 , inet_ntoa(NetIpAddr));
    NetIpAddr.s_addr=tlBroadcastSetInfo->setInfo.dwNetMask;
    strcpy(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPMask.csIpV4 , inet_ntoa(NetIpAddr));
    NetIpAddr.s_addr=tlBroadcastSetInfo->setInfo.dwGateway;
    strcpy(g_tl_globel_param.stNetworkCfg.stGatewayIpAddr.csIpV4, inet_ntoa(NetIpAddr));
    g_tl_globel_param.stNetworkCfg.stEtherNet[0].wDataPort = (WORD)tlBroadcastSetInfo->setInfo.dwMediaPort;
    g_tl_globel_param.stNetworkCfg.wHttpPort = (WORD)tlBroadcastSetInfo->setInfo.dwWebPort;
    g_tl_globel_param.stNetworkCfg.byEnableDHCP = tlBroadcastSetInfo->setInfo.bEnableDHCP;
    NetIpAddr.s_addr=tlBroadcastSetInfo->setInfo.dwDNS;
    strcpy(g_tl_globel_param.stNetworkCfg.stDnsServer1IpAddr.csIpV4, inet_ntoa(NetIpAddr));
    memcpy(g_tl_globel_param.stNetworkCfg.stEtherNet[0].byMACAddr, tlBroadcastSetInfo->setInfo.szMac, 6);
    memcpy(g_tl_globel_param.stDevicInfo.csDeviceName, tlBroadcastSetInfo->setInfo.szServerName, 32);
    
    g_tl_globel_param.stNetPlatCfg.bEnable = tlBroadcastSetInfo->nxServer.bEnable;
    memcpy(g_tl_globel_param.stNetPlatCfg.csServerNo, tlBroadcastSetInfo->nxServer.csServerNo, 64);
    inet_aton(g_tl_globel_param.stNetPlatCfg.strIPAddr.csIpV4, (struct in_addr *)&tlBroadcastSetInfo->nxServer.dwCenterIp);
    g_tl_globel_param.stNetPlatCfg.dwCenterPort  = tlBroadcastSetInfo->nxServer.wCenterPort;
    //2013-6-19 9:52:59 增加upnp端口的设定
    g_tl_globel_param.tl_upnp_config.dwDataPort = g_tl_globel_param.stNetworkCfg.stEtherNet[0].wDataPort;
    g_tl_globel_param.tl_upnp_config.dwWebPort = g_tl_globel_param.stNetworkCfg.wHttpPort;

    strcpy(g_tl_globel_param.stNetPlatCfg.strIPAddr.csIpV4, tlBroadcastSetInfo->nxServer.csCenterIP);
}

void BroadcastSetWifiAddr(TLNV_SET_SERVER_INFO_BROADCAST_V2 * tlBroadcastSetInfo)
{
    g_tl_globel_param.tl_Wifi_config.dwNetIpAddr= tlBroadcastSetInfo->setInfo.dwIp;
    g_tl_globel_param.tl_Wifi_config.dwGateway = tlBroadcastSetInfo->setInfo.dwGateway;
    g_tl_globel_param.tl_Wifi_config.dwNetMask = tlBroadcastSetInfo->setInfo.dwNetMask;
    g_tl_globel_param.tl_Wifi_config.dwDNSServer = tlBroadcastSetInfo->setInfo.dwDNS;   
}

#if 1

static int get_ip_addr(char *name,char *net_ip)
{
	struct ifreq ifr;
	int ret = 0;
	int fd;
    struct sockaddr_in stSockAddrIn;

	if((name == NULL) || (net_ip== NULL)){
		printf("get_ip_addr: invalid argument!\n");
		ret=-1;
	}
	
	strcpy(ifr.ifr_name, name);
	ifr.ifr_addr.sa_family = AF_INET;
	
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) 
	{
		ret = -1;
	}

    memset(&stSockAddrIn, 0, sizeof(stSockAddrIn));
    memcpy(&stSockAddrIn, &ifr.ifr_addr, sizeof(stSockAddrIn));
	strcpy(net_ip,inet_ntoa(stSockAddrIn.sin_addr));	
	//strcpy(net_ip,inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));

	close(fd);

	return	ret;
}

//去掉传入字符串头为空格的部分
static char *Get_Valid_Data(char *p)
{
	if(p == NULL)
	{
		printf("%s  %d\n",__FUNCTION__, __LINE__);
		return NULL;
	}
	
	while(p[0] == ' ')
	{
		p++;
	}
	
	return p;
}


//获取版本打包时间
static int SearchTools_GetVerDay(TLNV_SERVER_MSG_DATA_EX_V3 *jbsearchinfo)
{
	QMAPI_NET_DEVICE_INFO DeviceCfg;
	memset(&DeviceCfg,0,sizeof(DeviceCfg));
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, &DeviceCfg, sizeof(DeviceCfg));

	jbsearchinfo->jspackex_v3.dwVerYear = (DeviceCfg.dwSoftwareBuildDate >> 16) & 0xffff;
	jbsearchinfo->jspackex_v3.dwVerMonth = ((DeviceCfg.dwSoftwareBuildDate << 16) >> 24) & 0xffff;
	jbsearchinfo->jspackex_v3.dwVerDay = ((DeviceCfg.dwSoftwareBuildDate << 24) >> 24) & 0xffff;

	return 0;
}

//获取设备运行时间
static int SearchTools_GetDevUptime(TLNV_SERVER_MSG_DATA_EX_V3 *jbsearchinfo)
{
	struct stat st;
	FILE *fd;
	char buf[1024] = {0},buf1[512] = {0};
	char *p1 = NULL, *p2 = NULL;

	SystemCall_msg("cat /proc/uptime > /tmp/uptime",SYSTEM_MSG_BLOCK_RUN);	

	int ret = stat("/tmp/uptime", &st);
	if(ret != 0)
	{
		printf("%s  %d, no /proc/uptime \n",__FUNCTION__, __LINE__);
		ret = -1;
	}
	else
	{	
		fd = fopen("/tmp/uptime","r");
		if(NULL == fd)
		{
			printf("fopen faile.errno:%d\n",errno);
			ret = -2;
		}
		else
		{
			ret = fread(buf,st.st_size,1,fd);
			if(ret != 1)
			{
				printf("fread faile.ret=%d,buf=%s\n",ret,buf);
				ret = -3;
			}
			else
			{
				p1 = strstr(buf," ");
				if(p1 != NULL)
				{
					int totalsec = 0;
					memcpy(buf1,buf,p1-buf);
					p1 = strstr(buf1,".");
					if(p1 != NULL)
					{
						p2 = p1 + 1;
						if(atoi(p2) > 50)
							totalsec = atoi(buf1) + 1;
						else
							totalsec = atoi(buf1);
						jbsearchinfo->jspackex_v3.dwRunHour = totalsec / 3600;
						jbsearchinfo->jspackex_v3.dwRunMin = (totalsec % 3600)/60;
					}
				}
			}
			fclose(fd);
		}
	}

	return ret;
}

//获取内存总大小/剩余大小
static int SearchTools_GetMemInfo(unsigned int *TotalMem,unsigned int *FreeMem)
{
	struct stat st;
	FILE *fd;
	char buf[1024] = {0},buf1[512] = {0};
	char *p1 = NULL, *p2 = NULL;

	SystemCall_msg("free > /tmp/free",SYSTEM_MSG_BLOCK_RUN);
	int ret = stat("/tmp/free", &st);
	if(ret != 0)
	{
		printf("%s  %d, no /tmp/free \n",__FUNCTION__, __LINE__);
		ret = -1;
	}
	else
	{
		fd = fopen("/tmp/free","r");
		if(NULL == fd)
		{
			printf("fopen faile.errno:%d\n",errno);
			ret = -2;
		}
		else
		{
			memset(buf,0,sizeof(buf));
			ret = fread(buf,st.st_size,1,fd);
			if(ret != 1)
			{
				printf("fread faile.ret=%d,buf=%s\n",ret,buf);
				ret = -3;
			}
			else
			{
				p1 = strstr(buf,"Mem");
				if(p1 != NULL)
				{
 					p2 = p1 + 4;
					p1 = Get_Valid_Data(p2);
					p2 = strstr(p1," ");
					if(p2 != NULL)
					{
						memset(buf1,0,sizeof(buf1));
						memcpy(buf1,p1,p2-p1);
						*TotalMem = atoi(buf1);
						p1 = Get_Valid_Data(p2);
						p2 = strstr(p1," ");
						if(p2 != NULL)
						{
							p1 = Get_Valid_Data(p2);
							p2 = strstr(p1," ");
							if(p2 != NULL)
							{
								memset(buf1,0,sizeof(buf1));
								memcpy(buf1,p1,p2-p1);
								*FreeMem = atoi(p1);
							}
						}
					}
				}
			}
			fclose(fd);
		}
	}

	return ret;
}

//获取flash总大小和flash剩余大小
static int SearchTools_GetFlashInfo(TLNV_SERVER_MSG_DATA_EX_V3 *jbsearchinfo)
{
	struct stat st;
	FILE *fd;
	char buf[1024] = {0},buf1[512] = {0};
	char *p1 = NULL, *p2 = NULL;

	SystemCall_msg("df > /tmp/df",SYSTEM_MSG_BLOCK_RUN);
	int ret = stat("/tmp/df", &st);
	if(ret != 0)
	{
		printf("%s  %d, no /tmp/df \n",__FUNCTION__, __LINE__);
		ret = -1;
	}
	else
	{
		fd = fopen("/tmp/df","r");
		if(NULL == fd)
		{
			printf("fopen faile.errno:%d\n",errno);
			ret = -2;
		}
		else
		{
			memset(buf,0,sizeof(buf));
			ret = fread(buf,st.st_size,1,fd);
			if(ret != 1)
			{
				printf("fread faile.ret=%d,buf=%s\n",ret,buf);
				ret = -3;
			}
			else
			{
				p1 = strstr(buf,"/dev/root"); 
				if(p1 != NULL)
				{
					p2 = p1 + 9;
					p1 = Get_Valid_Data(p2);
					p2 = strstr(p1," ");
					if(p2 != NULL)
					{
						memset(buf1,0,sizeof(buf1));
						memcpy(buf1,p1,p2-p1); //这里的buf1应该为flash的全部可用空间
						if(atoi(buf1) > 7168)
							jbsearchinfo->jspackex_v3.dwFlaTotal = 16384;
						else
							jbsearchinfo->jspackex_v3.dwFlaTotal = 8192;
						p1 = Get_Valid_Data(p2);
						p2 = strstr(p1," ");
						if(p2 != NULL)
						{
							p1 = Get_Valid_Data(p2); 
							p2 = strstr(p1," ");
							if(p2 != NULL)
							{
								memset(buf1,0,sizeof(buf1));
								memcpy(buf1,p1,p2-p1); //这里的buf1应该为flash的剩余空间
								jbsearchinfo->jspackex_v3.dwFlaFree = atoi(buf1);
							}
						}
					}
				}
			}
			fclose(fd);
		}
	}

	return ret;
}

//获取CPU占用率
static int SearchTools_GetCpuUsedInfo(TLNV_SERVER_MSG_DATA_EX_V3 *jbsearchinfo)
{
	struct stat st;
	FILE *fd;
	char buf[1024] = {0},buf1[512] = {0};
	char *p1 = NULL, *p2 = NULL;
	char pidbuf[10] = {0};
	float utilization = 0.00;
	int cpu = 0;
	int pid = getpid();

	SystemCall_msg("top -n 1 | head -n 8 > /tmp/top",SYSTEM_MSG_BLOCK_RUN);
	int ret = stat("/tmp/top", &st);
	if(ret != 0)
	{
		printf("%s  %d, no /tmp/top \n",__FUNCTION__, __LINE__);
		ret = -1;
	}
	else
	{
		fd = fopen("/tmp/top","r");
		if(NULL == fd)
		{
			printf("fopen faile.errno:%d\n",errno);
			ret = -2;
		}
		else
		{
			memset(buf,0,sizeof(buf));
			ret = fread(buf,st.st_size,1,fd);
			if(ret != 1)
			{
				printf("fread faile.ret=%d,buf=%s\n",ret,buf);
				ret = -3;
			}
			else
			{
				sprintf(pidbuf,"%d",pid);
				p1 = strstr(buf,pidbuf);
				if(p1 != NULL)
				{
					p2 = strstr(p1,"m");
					if(p2 != NULL)
					{
						p1 = strstr(p2," ");
						if(p1 != NULL)
						{
							p2 = strstr(p1,"0");
							if(p2 != NULL)
							{
								p1 = strstr(p2,"{main} qm_app");
								if(p1 != NULL)
								{
									memset(buf1,0,sizeof(buf1));
									memcpy(buf1,p2,p1-p2);
									sscanf(buf1,"%d%f",&cpu,&utilization);
									jbsearchinfo->jspackex_v3.fCPUutilization = utilization;
								}
							}
						}
					}
				}
			}
			fclose(fd);
		}
	}

	return ret;
}

static int SearchTools_GetSensorType(TLNV_SERVER_MSG_DATA_EX_V3 *jbsearchinfo)
{
	//jbsearchinfo->jspackex_v3.dwSensorType = jb_get_video_input_type();

	return 0;
}

int SearchTools_GetFdNum(unsigned int *pFdNum)
{
	struct stat st;
	FILE *fd;
	char buf[10240] = {0};
	int num = 0;

	int pid = getpid();
	char cmd[50]={0};
	sprintf(cmd,"ls /proc/%d/fd > /tmp/fd",pid);
	SystemCall_msg(cmd,SYSTEM_MSG_BLOCK_RUN);
	int ret = stat("/tmp/fd", &st);
	if(ret != 0)
	{
		printf("%s  %d, no /tmp/fd \n",__FUNCTION__, __LINE__);
		ret = -1;
	}
	else
	{
		fd = fopen("/tmp/fd","r");
		if(NULL == fd)
		{
			printf("fopen faile.errno:%d\n",errno);
			ret = -2;
		}
		else
		{
			ret = fread(buf,st.st_size,1,fd);
			if(ret != 1)
			{
				printf("fread faile.ret=%d,buf=%s\n",ret,buf);
				ret = -3;
			}
			else
			{
				char *p = buf;
				while((p = strstr(p,"\n")) != NULL)
				{
					num++;
					while(p[0] == '\n')
						p++;
				}
				*pFdNum = num;
			}		
			fclose(fd);
		}
	}

	return ret;
}

int SearchTools_GetTaskNum(unsigned int *pTaskNum)
{
	struct stat st;
	FILE *fd;
	char buf[10240] = {0};
	int num = 0;

	int pid = getpid();
	char cmd[50]={0};

    memset(cmd, 0, sizeof(cmd));
	sprintf(cmd,"ls /proc/%d/task > /tmp/task",pid);
	SystemCall_msg(cmd,SYSTEM_MSG_BLOCK_RUN);
	int ret = stat("/tmp/task", &st);
	if(ret != 0)
	{
		printf("%s  %d, no /tmp/task \n",__FUNCTION__, __LINE__);
		ret = -1;
	}
	else
	{
		fd = fopen("/tmp/task","r");
		if(NULL == fd)
		{
			printf("fopen faile.errno:%d\n",errno);
			ret = -2;
		}
		else
		{
			ret = fread(buf,st.st_size,1,fd);
			if(ret != 1)
			{
				printf("fread faile.ret=%d,buf=%s\n",ret,buf);
				ret = -3;
			}
			else
			{
				char *p = buf;
				while((p = strstr(p,"\n")) != NULL)
				{
					num++;
					while(p[0] == '\n')
						p++;
				}
				*pTaskNum = num;
			}		
			fclose(fd);
		}
	}

	return ret;
}

static int SearchTools_GetNetConNum(TLNV_SERVER_MSG_DATA_EX_V3 *jbsearchinfo)
{
	//jbsearchinfo->jspackex_v3.dwNetConum = g_clientlist.nTotalNum;
	jbsearchinfo->jspackex_v3.dwNetConum = 0;

	return 0;
}

static int SearchTools_GetDevExInfo(TLNV_SERVER_MSG_DATA_EX_V3 *jbsearchinfo)
{
	//memcpy(jbsearchinfo->jspackex_v3.csBuildInfo,g_tl_globel_param.stDeviceExtInfo.csBuildInfo,strlen(g_tl_globel_param.stDeviceExtInfo.csBuildInfo));
	
	return 0;
}

static int SearchTools_GetCpuType(TLNV_SERVER_MSG_DATA_EX_V3 *jbsearchinfo)
{
	QMAPI_NET_DEVICE_INFO DeviceCfg;
	memset(&DeviceCfg,0,sizeof(DeviceCfg));
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, &DeviceCfg, sizeof(DeviceCfg));

	jbsearchinfo->jspackex_v3.dwCPUType = DeviceCfg.dwServerCPUType;

	return 0;
}

#endif

///TL Video Write to Video Server Information , fill broadcast package with server's configuration
int TL_Video_FillOut_Server_Info(TLNV_SERVER_MSG_DATA_EX_V2 *tlsearchinfo)
{
    if(tlsearchinfo==NULL)
    {
        return -1;
    }
    memset(tlsearchinfo , 0 , sizeof(TLNV_SERVER_MSG_DATA_EX_V2));

    char szmac[24];
    char szbmac[8];
    int ret = 0;
    char s8SecondIp[32];
    TLNV_SYSTEM_INFO sinfo;
    
    memset(&sinfo, 0, sizeof(TLNV_SYSTEM_INFO));
    printf("@@@@ %s. %d.\n",__FUNCTION__,__LINE__);
    QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ENCRYPT, 0, &sinfo, sizeof(TLNV_SYSTEM_INFO));
    
    get_net_phyaddr(QMAPI_ETH_DEV, szmac, szbmac);
    memcpy(tlsearchinfo->tlServerPack.bMac, szbmac, 6);

    strcpy(tlsearchinfo->tlServerPack.jspack.szIp, g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4);

    tlsearchinfo->dwSize                            = sizeof(TLNV_SERVER_MSG_DATA_EX_V2);
    tlsearchinfo->dwPackFlag                        = SERVER_PACK_FLAG;
    tlsearchinfo->tlServerPack.jspack.wMediaPort    = g_tl_globel_param.stNetworkCfg.stEtherNet[0].wDataPort;//Media Port
    tlsearchinfo->tlServerPack.jspack.wWebPort      = g_tl_globel_param.stNetworkCfg.wHttpPort;//Http Port
    tlsearchinfo->tlServerPack.jspack.wChannelCount = sinfo.byChannelNum ? sinfo.byChannelNum : 1;       //
    memcpy(tlsearchinfo->tlServerPack.jspack.szServerName , g_tl_globel_param.stDevicInfo.csDeviceName, 32); //
    tlsearchinfo->tlServerPack.jspack.dwDeviceType = CPUTYPE_Q3F;

	// need fixme yi.zhang
   // tlsearchinfo->tlServerPack.jspack.dwServerVersion = GetSystemVersionFlag();
    tlsearchinfo->tlServerPack.jspack.wChannelStatic  = sinfo.byChannelNum ? sinfo.byChannelNum : 1;      //(Is Lost Video)
    tlsearchinfo->tlServerPack.jspack.wSensorStatic   = sinfo.byAlarmInputNum;        //
    tlsearchinfo->tlServerPack.jspack.wAlarmOutStatic = sinfo.byAlarmOutputNum; //

    tlsearchinfo->tlServerPack.bEnableDHCP   =  g_tl_globel_param.stNetworkCfg.byEnableDHCP;
    tlsearchinfo->tlServerPack.bEnableDNS    = g_tl_globel_param.stNetworkCfg.byEnableDHCP;
    tlsearchinfo->tlServerPack.dwNetMask     = TL_GetIpValueFromString(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPMask.csIpV4);
    tlsearchinfo->tlServerPack.dwGateway     = TL_GetIpValueFromString(g_tl_globel_param.stNetworkCfg.stGatewayIpAddr.csIpV4);
    tlsearchinfo->tlServerPack.dwDNS         = TL_GetIpValueFromString(g_tl_globel_param.stNetworkCfg.stDnsServer1IpAddr.csIpV4);

    tlsearchinfo->tlServerPack.bEnableCenter = g_tl_globel_param.stNetPlatCfg.bEnable; 
    inet_aton(g_tl_globel_param.stNetPlatCfg.strIPAddr.csIpV4, (struct in_addr *)&tlsearchinfo->tlServerPack.dwCenterIpAddress);
    tlsearchinfo->tlServerPack.dwCenterPort = g_tl_globel_param.stNetPlatCfg.dwCenterPort; 
    memcpy(tlsearchinfo->tlServerPack.csServerNo, g_tl_globel_param.stDevicInfo.csSerialNumber, 64);
    strcpy(tlsearchinfo->tlServerPack.csCenterIpAddress, g_tl_globel_param.stNetPlatCfg.strIPAddr.csIpV4);
    tlsearchinfo->tlServerPack.bEncodeAudio = g_tl_globel_param.stChannelPicParam[0].stRecordPara.wEncodeAudio;
	
	memset(s8SecondIp , 0 , sizeof(s8SecondIp));
    ret = get_ip_addr("eth0:1", s8SecondIp);
    if(ret)
    {
    	tlsearchinfo->tlServerPack.dwComputerIP = 0; 	
    }
    else
    {
    	tlsearchinfo->tlServerPack.dwComputerIP = TL_GetIpValueFromString(s8SecondIp);
    }

    return 0;
}

static int TL_SearchTools_Info_Ex(TLNV_SERVER_MSG_DATA_EX_V3 *jbsearchinfo)
{
	SearchTools_GetVerDay(jbsearchinfo);
	SearchTools_GetDevUptime(jbsearchinfo);
	SearchTools_GetMemInfo((unsigned int *)&jbsearchinfo->jspackex_v3.dwMemTotal,(unsigned int *)&jbsearchinfo->jspackex_v3.dwMemFree);
	SearchTools_GetFlashInfo(jbsearchinfo);
	//SearchTools_GetCpuUsedInfo(jbsearchinfo);
	SearchTools_GetSensorType(jbsearchinfo);
	SearchTools_GetFdNum((unsigned int *)&jbsearchinfo->jspackex_v3.dwFdnum);
	SearchTools_GetTaskNum((unsigned int *)&jbsearchinfo->jspackex_v3.dwTasknum);
	SearchTools_GetNetConNum(jbsearchinfo);
	SearchTools_GetDevExInfo(jbsearchinfo);
	SearchTools_GetCpuType(jbsearchinfo);

	return 0;
}
///JB Video Write to Video Server Information , fill broadcast package with server's configuration
int TL_Video_FillOut_Server_Info_V3(TLNV_SERVER_MSG_DATA_EX_V3 *jbsearchinfo)
{
    if(jbsearchinfo==NULL)
    {
        return -1;
    }
    memset(jbsearchinfo, 0, sizeof(TLNV_SERVER_PACK_EX_V3));

    char szmac[24];
    char szbmac[8];
    //TLNV_SYSTEM_INFO sinfo;
    
    get_net_phyaddr("eth0", szmac, szbmac);
    memcpy(jbsearchinfo->jspackex_v3.jspackex_v2.bMac, szbmac, 6);
    if(0 != get_ip_addr(ETH_DEV,jbsearchinfo->jspackex_v3.jspackex_v2.jspack.szIp))
    {
    	printf("csIpV4:%s\n",g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4);
        strcpy(jbsearchinfo->jspackex_v3.jspackex_v2.jspack.szIp, g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4);
    }

    //memset(&sinfo, 0, sizeof(TLNV_SYSTEM_INFO));
    //printf("@@@@ %s. %d.\n",__FUNCTION__,__LINE__);
    //QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ENCRYPT, 0, &sinfo, sizeof(TLNV_SYSTEM_INFO));
    jbsearchinfo->dwSize = sizeof(TLNV_SERVER_PACK_EX_V3);
    jbsearchinfo->dwPackFlag= SERVER_PACK_FLAG;
    jbsearchinfo->jspackex_v3.jspackex_v2.jspack.wMediaPort    = g_tl_globel_param.stNetworkCfg.stEtherNet[0].wDataPort;//Media Port
    jbsearchinfo->jspackex_v3.jspackex_v2.jspack.wWebPort      = g_tl_globel_param.stNetworkCfg.wHttpPort;//Http Port
    jbsearchinfo->jspackex_v3.jspackex_v2.jspack.wChannelCount = 1;//sinfo.byChannelNum;        //
    memcpy(jbsearchinfo->jspackex_v3.jspackex_v2.jspack.szServerName , g_tl_globel_param.stDevicInfo.csDeviceName, 32); //

    jbsearchinfo->jspackex_v3.jspackex_v2.jspack.dwDeviceType = CPUTYPE_Q3F;

    jbsearchinfo->jspackex_v3.jspackex_v2.jspack.dwServerVersion = GetSystemVersionFlag();
    jbsearchinfo->jspackex_v3.jspackex_v2.jspack.wChannelStatic  = 1;//sinfo.byChannelNum;       //(Is Lost Video)
    jbsearchinfo->jspackex_v3.jspackex_v2.jspack.wSensorStatic   = 0;//sinfo.byAlarmInputNum;        //
    jbsearchinfo->jspackex_v3.jspackex_v2.jspack.wAlarmOutStatic = 0;//sinfo.byAlarmOutputNum; //

    jbsearchinfo->jspackex_v3.jspackex_v2.bEnableDHCP =  g_tl_globel_param.stNetworkCfg.byEnableDHCP;
    jbsearchinfo->jspackex_v3.jspackex_v2.bEnableDNS  = g_tl_globel_param.stNetworkCfg.byEnableDHCP;
    jbsearchinfo->jspackex_v3.jspackex_v2.dwNetMask   = TL_GetIpValueFromString(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPMask.csIpV4);
    jbsearchinfo->jspackex_v3.jspackex_v2.dwGateway   = TL_GetIpValueFromString(g_tl_globel_param.stNetworkCfg.stGatewayIpAddr.csIpV4);
    jbsearchinfo->jspackex_v3.jspackex_v2.dwDNS       = TL_GetIpValueFromString(g_tl_globel_param.stNetworkCfg.stDnsServer1IpAddr.csIpV4);

    jbsearchinfo->jspackex_v3.jspackex_v2.bEnableCenter = g_tl_globel_param.stNetPlatCfg.bEnable; 
    inet_aton(g_tl_globel_param.stNetPlatCfg.strIPAddr.csIpV4, (struct in_addr *)&jbsearchinfo->jspackex_v3.jspackex_v2.dwCenterIpAddress);
    jbsearchinfo->jspackex_v3.jspackex_v2.dwCenterPort = g_tl_globel_param.stNetPlatCfg.dwCenterPort; 
    memcpy(jbsearchinfo->jspackex_v3.jspackex_v2.csServerNo, g_tl_globel_param.stDevicInfo.csSerialNumber, 48);
    //printf("#### %s %d, csSerialNumber:%s\n", __FUNCTION__, __LINE__, g_tl_globel_param.stDevicInfo.csSerialNumber);
    strcpy(jbsearchinfo->jspackex_v3.jspackex_v2.csCenterIpAddress, g_tl_globel_param.stNetPlatCfg.strIPAddr.csIpV4);
    jbsearchinfo->jspackex_v3.jspackex_v2.bEncodeAudio = g_tl_globel_param.stChannelPicParam[0].stRecordPara.wEncodeAudio; 

	TL_SearchTools_Info_Ex(jbsearchinfo);

    return 0;
}

int TL_Video_FillOut_Wifi_Server_Info(TLNV_SERVER_MSG_DATA_EX_V2 *tlsearchinfo)
{
    struct in_addr  WifiAddr;
    char szmac[24];
    char szbmac[8];
    TLNV_SYSTEM_INFO sinfo;
    
    if(tlsearchinfo==NULL)
    {
        return -1;
    }
    memset(tlsearchinfo , 0 , sizeof(TLNV_SERVER_MSG_DATA_EX_V2));
    get_net_phyaddr(WIFI_DEV, szmac, szbmac);
    memcpy(tlsearchinfo->tlServerPack.bMac, szbmac, 6);

    memset(&sinfo, 0, sizeof(TLNV_SYSTEM_INFO));
    QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ENCRYPT, 0, &sinfo, sizeof(TLNV_SYSTEM_INFO));
    printf("@@@@ %s. %d.\n",__FUNCTION__,__LINE__);
    WifiAddr.s_addr = g_tl_globel_param.tl_Wifi_config.dwNetIpAddr;
    strcpy(tlsearchinfo->tlServerPack.jspack.szIp , inet_ntoa(WifiAddr));
    tlsearchinfo->dwSize=sizeof(TLNV_SERVER_MSG_DATA_EX_V2);
    tlsearchinfo->dwPackFlag=SERVER_PACK_FLAG;
    
    tlsearchinfo->tlServerPack.dwNetMask = g_tl_globel_param.tl_Wifi_config.dwNetMask;
    tlsearchinfo->tlServerPack.dwGateway = g_tl_globel_param.tl_Wifi_config.dwGateway;
    tlsearchinfo->tlServerPack.dwDNS     = g_tl_globel_param.tl_Wifi_config.dwDNSServer;

    tlsearchinfo->tlServerPack.jspack.wMediaPort    = g_tl_globel_param.stNetworkCfg.stEtherNet[0].wDataPort;//Media Port
    tlsearchinfo->tlServerPack.jspack.wWebPort      = g_tl_globel_param.stNetworkCfg.wHttpPort;//Http Port
    tlsearchinfo->tlServerPack.jspack.wChannelCount = sinfo.byChannelNum ? sinfo.byChannelNum : 1;        //
    memcpy(tlsearchinfo->tlServerPack.jspack.szServerName , g_tl_globel_param.stDevicInfo.csDeviceName, 32); //


    tlsearchinfo->tlServerPack.jspack.dwDeviceType    = CPUTYPE_Q3F;
    tlsearchinfo->tlServerPack.jspack.dwServerVersion = GetSystemVersionFlag();
    tlsearchinfo->tlServerPack.jspack.wChannelStatic  = sinfo.byChannelNum ? sinfo.byChannelNum : 1;       //(Is Lost Video)
    tlsearchinfo->tlServerPack.jspack.wSensorStatic   = sinfo.byAlarmInputNum;        //
    tlsearchinfo->tlServerPack.jspack.wAlarmOutStatic = sinfo.byAlarmOutputNum; //

    tlsearchinfo->tlServerPack.bEnableDHCP =  g_tl_globel_param.stNetworkCfg.byEnableDHCP;
    tlsearchinfo->tlServerPack.bEnableDNS  = g_tl_globel_param.stNetworkCfg.byEnableDHCP;

    tlsearchinfo->tlServerPack.bEnableCenter = g_tl_globel_param.stNetPlatCfg.bEnable; 
    inet_aton(g_tl_globel_param.stNetPlatCfg.strIPAddr.csIpV4, (struct in_addr *)&tlsearchinfo->tlServerPack.dwCenterIpAddress);
    tlsearchinfo->tlServerPack.dwCenterPort = g_tl_globel_param.stNetPlatCfg.dwCenterPort; 
    memcpy(tlsearchinfo->tlServerPack.csServerNo, g_tl_globel_param.stNetPlatCfg.csServerNo, 64);
    strcpy(tlsearchinfo->tlServerPack.csCenterIpAddress, g_tl_globel_param.stNetPlatCfg.strIPAddr.csIpV4);
    tlsearchinfo->tlServerPack.bEncodeAudio = g_tl_globel_param.stChannelPicParam[0].stRecordPara.wEncodeAudio; 

    return 0;
}


int sendBroadMsg(int sockFd , DWORD dwSrcAddr, void* Msg,int len,struct sockaddr_in destAddr)
{
    struct in_addr adr;
    int ret = 0;
    adr.s_addr = dwSrcAddr;
    ret = setsockopt(sockFd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&adr, sizeof(adr));
    if(ret != 0)
    {
        //printf("setsockopt sockfd:%d IP_MULTICAST_IF %s failed:%s\n", sockFd, inet_ntoa(*((struct in_addr *)&dwSrcAddr)), strerror(errno));
    }
    int sLen = sendto(sockFd , Msg, len, 0, (struct sockaddr*)&destAddr , sizeof(destAddr));
    char addrBuf[64];
    strcpy(addrBuf,inet_ntoa(destAddr.sin_addr));
    if(sLen != len)
    {
        //printf("IPC %s Broadcast MSG Send  to address %s failed \n ", inet_ntoa(*((struct in_addr *)&dwSrcAddr)), addrBuf);
    }
    /*   
    TLNV_SERVER_MSG_DATA_EX_V2 *pack = (TLNV_SERVER_MSG_DATA_EX_V2 *)Msg;   
    printf("IPC(%s) Broadcast MSG Send  to address %s(%d)  srcaddr %s\n ", pack->tlServerPack.jspack.szIp, addrBuf, ntohs(destAddr.sin_port), inet_ntoa(*((struct in_addr *)&dwSrcAddr)));
    */   
    return 0;
}

int create_broadcast_sock()
{
	int opt;
	int ret, fd;
	struct sockaddr_in 	serverAddr;
	struct ip_mreq 		command;

	serverAddr.sin_family = AF_INET;       
	serverAddr.sin_port = htons(MULTI_BROADCAST_SREACH_SEND_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd<0)
	{
		printf("%s  %d, create socket error!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
		return -1;
	}

	opt = 1;
	ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (ret < 0)
	{
		printf("%s  %d, setsockopt error!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
		close(fd);
		return -2;
	}
	ret = bind(fd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr_in)); 
	if (ret < 0)
	{
		printf("%s  %d, bind error!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
		close(fd);
		return -3;
	}
	
	opt = 1;
	ret = setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &opt, sizeof(opt));
	if (ret < 0)
	{
		printf("%s  %d, setsockopt error!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
		close(fd);
		return -4;
	}

	command.imr_multiaddr.s_addr = inet_addr(MULTI_BROADCAST_SREACH_IPADDR);
	command.imr_interface.s_addr = htonl(INADDR_ANY);
#if 1
	int xtimess = 0;
	while (setsockopt(fd,IPPROTO_IP, IP_ADD_MEMBERSHIP, &command, sizeof(command))<0)
    {
		//在自动获取IP的，如果一开始不能取得IP，则会设置失败。
		printf("%s  %d, ret = %d setsockopt1 error!err:%s(%d)\n",__FUNCTION__, __LINE__, ret,strerror(errno), errno);
		sleep(1);
		if (xtimess >30)
			break;
		xtimess ++;
		
	}
#else
	ret = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &command, sizeof(command));
	if (ret < 0)
	{
		printf("%s  %d, setsockopt error!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
		//暂时使用，等待后期更完善的方案
		//return (void *)NULL;
	}
#endif
	return fd;
}


/*receive servertools' broadcast package , for set service's configuration , or send service's infomation to servertools*/
void ServerInfoMultiBroadcastThread(void)
{
    int iswifi = 0;
    int                 ret = -1;
    int             recv_bytes = 0;
    int             listenBroadfd = -1;
    //int                            nGetMacRet = -1;
    socklen_t           len;
    char                mesg[2048];
    char                          szmac[24];
    char                          szbmac[8];

    fd_set          fset;
    struct timeval          to;
    struct sockaddr_in  recvAddr;       /*In fact ,it is a send addr */

    TLNV_SERVER_MSG_DATA_EX_V2           tlsearch;
    TLNV_SET_SERVER_INFO_BROADCAST_V2    msg_data;
    TLNV_BROADCAST_PACK                 *lpBroadcastPacket=NULL;
//  TLNV_BROADCAST_HEADER                BroadcastPacketHdr; 
    prctl(PR_SET_NAME, (unsigned long)"ServerInfoBroadcast", 0, 0, 0);

    recvAddr.sin_family = AF_INET;       
    recvAddr.sin_port = htons(MULTI_BROADCAST_SREACH_RECV_PORT_ADD);
    recvAddr.sin_addr.s_addr = inet_addr(MULTI_BROADCAST_SREACH_IPADDR);
    
    len = sizeof(recvAddr);

    while (1)
    {
    	// need fixme yi.zhang
       // if (g_tl_updateflash.bUpdatestatus)
       //     return ;

        if(listenBroadfd<0)
        {
            listenBroadfd = create_broadcast_sock();
            if(listenBroadfd<0)
            {
                printf("%s  %d, create_broadcast_sock error:%d\n",__FUNCTION__, __LINE__,listenBroadfd);
                return;
            }
        }

        FD_ZERO(&fset);
        FD_SET(listenBroadfd, &fset);
        to.tv_sec = 5;
        to.tv_usec = 0;

        ret = select(listenBroadfd+1, &fset, NULL, NULL, &to);
        if ( ret == 0 || (ret == -1 && errno == EINTR))
        {
            //printf("## ServerInfoMultiBroadcastThread timeout, ret=%d \n", ret);
            ret = DoServerInfoBroadcast_V3(listenBroadfd);
            //ret = DoServerInfoBroadcast(listenBroadfd);
            if(ret > 0&&!iswifi)//wifi 启用的时候
            {
                close(listenBroadfd);
                listenBroadfd = -1;
                iswifi = 1;
            }
            else if(ret == 0 && iswifi == 1)//wifi关掉的时候
            {
                close(listenBroadfd);
                listenBroadfd = -1;
                iswifi = 0;
            }

            //DoServerInfoBroadcast(listenBroadfd);

            continue;
        }           

        if (ret == -1)
        {
            err();
            sleep(1);           
            continue;
        }

        if(!FD_ISSET(listenBroadfd, &fset))
        {
            err();
            sleep(1);
            continue;
        }

        ioctl(listenBroadfd, FIONREAD, &recv_bytes);
      	printf("recv_bytes == %d\n", recv_bytes);
        if (recv_bytes <= 0 || recv_bytes > 2048)
        {
            err();
            sleep(1);
            continue;
        }

        ret = recv(listenBroadfd, mesg, recv_bytes, 0);
        if (ret <= 0)
        {   
            err();
            sleep(1);
            continue;
        }       

        if(recv_bytes == 14)
		{
            mesg[ret] = '\0';
            if (strcmp(mesg, "StartSearchDev") == 0)
            {
                printf("search version v3\n");
				DoServerInfoBroadcast_V3(listenBroadfd);
            }
            continue;
		}
        else if( recv_bytes == sizeof(TLNV_SET_SERVER_INFO_BROADCAST) 
            || recv_bytes ==  sizeof(TLNV_SET_SERVER_INFO_BROADCAST_V2))
        {
            memcpy(&msg_data, mesg, recv_bytes);

            if( recv_bytes == sizeof(TLNV_SET_SERVER_INFO_BROADCAST) )
            {
                struct in_addr  NetIpAddr;
                NetIpAddr.s_addr= msg_data.nxServer.dwCenterIp;
                strcpy(msg_data.nxServer.csCenterIP, inet_ntoa(NetIpAddr));
                msg_data.nBufSize = sizeof(TLNV_SET_SERVER_INFO_BROADCAST_V2);
            }
            else
            {
                struct hostent* host;
                host = gethostbyname2(msg_data.nxServer.csCenterIP,AF_INET);
                if(host  != NULL)
                {
                    memcpy(&msg_data.nxServer.dwCenterIp, host->h_addr_list[0], sizeof(unsigned long));
                }
            }
                 
            if (msg_data.nBufSize <= 0)
            {
                continue;
            }
        
            if( msg_data.nBufSize !=  sizeof(TLNV_SET_SERVER_INFO_BROADCAST_V2))
            {
                printf("TLNV_SET_SERVER_INFO_BROADCAST_V2 's size is not suit for our protocol\n");
                continue;
            }

            //printf("#### %s %d, nCmd=0x%X\n", __FUNCTION__, __LINE__, msg_data.hdr.nCmd);
            switch(msg_data.hdr.nCmd)
            {
            case BRD_GET_SERVER_INFO:
            {
            	char s8SecondIp[32];
            	memset(s8SecondIp , 0 , sizeof(s8SecondIp));
            	int ret = 0;
                unsigned long ulValue = TL_GetIpValueFromString(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4);
                if(ulValue == msg_data.setInfo.dwIp && (DWORD)g_tl_globel_param.stNetworkCfg.stEtherNet[0].wDataPort == msg_data.setInfo.dwMediaPort)
                {            
                    pthread_mutex_lock(&broadcast_mutex);           
                    TL_Video_FillOut_Server_Info(&tlsearch);
                    tlsearch.dwPackFlag = BRD_SEARCH_DEVICE;
                    ret = get_ip_addr("eth0:1", s8SecondIp);
                    if(ret)
                    {
                    	tlsearch.tlServerPack.dwComputerIP = 0; 	
                    }
                    else
                    {
                    	tlsearch.tlServerPack.dwComputerIP = TL_GetIpValueFromString(s8SecondIp);
                    }
                    printf("tlsearch.dwPackFlag == %lu\n", tlsearch.dwPackFlag);
                    pthread_mutex_unlock(&broadcast_mutex);
                    ret=sendto(listenBroadfd, &tlsearch, sizeof(TLNV_SERVER_MSG_DATA_EX_V2), 0, (struct sockaddr*)&recvAddr, len);
                }
#ifndef TL_Q3_PLATFORM
                //for wifi
                if(0 == nGetMacRet && g_bWifiStarted)
                {
                    if(g_tl_globel_param.tl_Wifi_config.dwNetIpAddr == msg_data.setInfo.dwIp
                        && (DWORD)g_tl_globel_param.stNetworkCfg.stEtherNet[0].wDataPort == msg_data.setInfo.dwMediaPort)
                    {
                        pthread_mutex_lock(&broadcast_mutex);           
                        TL_Video_FillOut_Wifi_Server_Info(&tlsearch);
                        tlsearch.dwPackFlag = BRD_SEARCH_DEVICE;
                        ret = get_ip_addr("eth0:1", s8SecondIp);
	                    if(ret)
	                    {
	                    	tlsearch.tlServerPack.dwComputerIP = 0; 	
	                    }
	                    else
	                    {
	                    	tlsearch.tlServerPack.dwComputerIP = TL_GetIpValueFromString(s8SecondIp);
	                    }
                        pthread_mutex_unlock(&broadcast_mutex);
                        ret=sendto(listenBroadfd, &tlsearch, sizeof(TLNV_SERVER_MSG_DATA_EX_V2), 0, (struct sockaddr*)&recvAddr, len);
                    }
                }
#endif
            }
            break;
            case BRD_SET_SERVER_INFO:
            {
#ifndef TL_Q3_PLATFORM
                if(0 == nGetMacRet && g_bWifiStarted)
                {
                    get_net_phyaddr(WIFI_DEV, szmac, szbmac);

                    if(memcmp(szbmac, msg_data.setInfo.szoldMac, 6))
                    {
                        printf("Not my wifi's mac ,you can not set wifi info\n");
                        break;
                    }
                    pthread_mutex_lock(&broadcast_mutex);   
                    BroadcastSetWifiAddr((TLNV_SET_SERVER_INFO_BROADCAST_V2 *)&msg_data);
                    pthread_mutex_unlock(&broadcast_mutex);

                    tl_write_wireless_wifi_param();     
                }
#endif
                get_net_phyaddr(QMAPI_ETH_DEV, szmac, szbmac);
                if( memcmp(szbmac,  msg_data.setInfo.szoldMac, 6))
                {
                    printf("Not my mac ,you can not set my service info %x%x%x%x%x%x-%x%x%x%x%x%x\n", 
                        g_tl_globel_param.stNetworkCfg.stEtherNet[0].byMACAddr[0],
                        g_tl_globel_param.stNetworkCfg.stEtherNet[0].byMACAddr[1],
                        g_tl_globel_param.stNetworkCfg.stEtherNet[0].byMACAddr[2],
                        g_tl_globel_param.stNetworkCfg.stEtherNet[0].byMACAddr[3],
                        g_tl_globel_param.stNetworkCfg.stEtherNet[0].byMACAddr[4],
                        g_tl_globel_param.stNetworkCfg.stEtherNet[0].byMACAddr[5],
                        msg_data.setInfo.szoldMac[0],
                        msg_data.setInfo.szoldMac[1],
                        msg_data.setInfo.szoldMac[2],
                        msg_data.setInfo.szoldMac[3],
                        msg_data.setInfo.szoldMac[4],
                        msg_data.setInfo.szoldMac[5]);
                        break;
                }
                //2013-1-31 18:30:44--更新配置文件里的MAC信息
                {
                    char szNewMac[24] = {0};
                    sprintf(szNewMac,"%02x:%02x:%02x:%02x:%02x:%02x",
                        msg_data.setInfo.szMac[0],msg_data.setInfo.szMac[1],
                        msg_data.setInfo.szMac[2],msg_data.setInfo.szMac[3],
                        msg_data.setInfo.szMac[4],msg_data.setInfo.szMac[5]);
					// need fixme yi.zhang
                      //  TL_UpdateCfgFile(TL_FLASH_CONF_DEFAULT_PARAM,
                      //      "DEVICE_MAC","MAC",3,szNewMac,strlen(szNewMac));
                }
                pthread_mutex_lock(&broadcast_mutex);                       
                BroadcastSetIpaddr((TLNV_SET_SERVER_INFO_BROADCAST_V2*)&msg_data);
                pthread_mutex_unlock(&broadcast_mutex);
				QMapi_sys_ioctrl(0, QMAPI_NET_STA_REBOOT, 0, NULL, 0);
                break;
            }       
            case CMD_RESTORE:
            {
                unsigned long ulValue = TL_GetIpValueFromString(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4);
                if(!(ulValue == msg_data.setInfo.dwIp
                    && (DWORD)g_tl_globel_param.stNetworkCfg.stEtherNet[0].wDataPort == msg_data.setInfo.dwMediaPort))
                {
                    printf("Not my ip and port , so I am not going to soft reset\n");
                    break;
                }
                pthread_mutex_lock(&g_tl_codec_setting_mutex);
                printf("%s  call TL_initDefaultSet \n",__FUNCTION__);
                TL_initDefaultSet( SOFTRESET, 3);
                pthread_mutex_unlock(&g_tl_codec_setting_mutex);
				QMapi_sys_ioctrl(0, QMAPI_NET_STA_REBOOT, 0, NULL, 0);

                break;
            }

            default:
            {
                err();
            }
            break;
            }
        }
        else
        {
            lpBroadcastPacket = (TLNV_BROADCAST_PACK*)mesg;
            if(lpBroadcastPacket->dwFlag != 9000)
            {
                //printf("Not proctol\n");
                continue;
            }
            unsigned long ulValue = TL_GetIpValueFromString(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4);
            if(!(ulValue == msg_data.setInfo.dwIp
                && (DWORD)g_tl_globel_param.stNetworkCfg.stEtherNet[0].wDataPort == msg_data.setInfo.dwMediaPort))
            {
                printf("Not my ip and port(%lu,%u) , so I am not going to deal\n", lpBroadcastPacket->dwServerIpaddr, lpBroadcastPacket->wServerPort);
                continue;
            }
            printf("dwCommand:0x%x wBufSize:%d\n", (int)lpBroadcastPacket->dwCommand, lpBroadcastPacket->wBufSize);
            switch(lpBroadcastPacket->dwCommand)
            {
            case CMD_SET_MAC_ADDRESS:
            {
            }
            break;
            default:
            {
                printf("Error Command 0x%x\n",(int)lpBroadcastPacket->dwCommand);
            }
            break;
            }
            continue;
        }
    }
}


int startServerInfoBroadcast()
{
    int ret = -1;
    pthread_t id;

    pthread_mutex_init(&broadcast_mutex , NULL);

#if 1
    ret = pthread_create(&id, NULL,  (void *)&ServerInfoMultiBroadcastThread, NULL);
    if (ret < 0)
    {
        err();
    }
#endif  
    return 0;
}

void  GetBroadAddress(struct sockaddr_in *addr,DWORD dwIP,DWORD dwMask,WORD port)
{
    dwMask = ~dwMask;
    dwIP = dwIP | dwMask;
    addr->sin_addr.s_addr = INADDR_BROADCAST;//dwIP; 
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port); 
}

//extern unsigned char g_wifistatus;

int DoServerInfoBroadcast(int Broadcastfd)
{
    int ret = 0;
    TLNV_SERVER_MSG_DATA_EX_V2                    tlsearch;
    struct sockaddr_in  recvAddr;       /*In fact ,it is a send addr */
    unsigned long ulValue = TL_GetIpValueFromString(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4);

    recvAddr.sin_family = AF_INET;       
    recvAddr.sin_port = htons(MULTI_BROADCAST_SREACH_RECV_PORT);
    recvAddr.sin_addr.s_addr = inet_addr(MULTI_BROADCAST_SREACH_IPADDR);

    pthread_mutex_lock(&broadcast_mutex);
    TL_Video_FillOut_Server_Info(&tlsearch);
    pthread_mutex_unlock(&broadcast_mutex);
    sendBroadMsg(Broadcastfd, ulValue, &tlsearch, sizeof(TLNV_SERVER_MSG_DATA_EX_V2),recvAddr);
#ifndef TL_Q3_PLATFORM
    if( g_tl_globel_param.tl_Wifi_config.bWifiEnable && (g_wifistatus == 0 || g_wifistatus == 6))
    {
        sendBroadMsg(Broadcastfd, g_tl_globel_param.tl_Wifi_config.dwNetIpAddr, &tlsearch, sizeof(TLNV_SERVER_MSG_DATA_EX_V2),recvAddr);
        pthread_mutex_lock(&broadcast_mutex);
        TL_Video_FillOut_Wifi_Server_Info(&tlsearch);
        pthread_mutex_unlock(&broadcast_mutex);
        sendBroadMsg(Broadcastfd, ulValue, &tlsearch, sizeof(TLNV_SERVER_MSG_DATA_EX_V2),recvAddr);

        sendBroadMsg(Broadcastfd, g_tl_globel_param.tl_Wifi_config.dwNetIpAddr, &tlsearch, sizeof(TLNV_SERVER_MSG_DATA_EX_V2),recvAddr);       
    }   
#endif
    recvAddr.sin_family = AF_INET;       
    recvAddr.sin_port = htons(MULTI_BROADCAST_SREACH_RECV_PORT_ADD);
    recvAddr.sin_addr.s_addr = inet_addr(MULTI_BROADCAST_SREACH_IPADDR);

    pthread_mutex_lock(&broadcast_mutex);
    TL_Video_FillOut_Server_Info(&tlsearch);
    pthread_mutex_unlock(&broadcast_mutex);
    sendBroadMsg(Broadcastfd, ulValue, &tlsearch, sizeof(TLNV_SERVER_MSG_DATA_EX_V2),recvAddr);
#ifndef TL_Q3_PLATFORM
	if(g_tl_globel_param.tl_Wifi_config.bWifiEnable && (g_wifistatus == 0 || g_wifistatus == 6))
    {
        sendBroadMsg(Broadcastfd, g_tl_globel_param.tl_Wifi_config.dwNetIpAddr, &tlsearch, sizeof(TLNV_SERVER_MSG_DATA_EX_V2),recvAddr);
        pthread_mutex_lock(&broadcast_mutex);
        TL_Video_FillOut_Wifi_Server_Info(&tlsearch);
        pthread_mutex_unlock(&broadcast_mutex);
        sendBroadMsg(Broadcastfd, ulValue, &tlsearch, sizeof(TLNV_SERVER_MSG_DATA_EX_V2),recvAddr);

        sendBroadMsg(Broadcastfd, g_tl_globel_param.tl_Wifi_config.dwNetIpAddr, &tlsearch, sizeof(TLNV_SERVER_MSG_DATA_EX_V2),recvAddr);

        ret = 1;
    }
#endif
    return ret;
}

int DoServerInfoBroadcast_V3(int Broadcastfd)
{
    int ret = 0;
    TLNV_SERVER_MSG_DATA_EX_V3                    jbsearch;
    struct sockaddr_in  recvAddr;       /*In fact ,it is a send addr */
    unsigned long ulValue = TL_GetIpValueFromString(g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4);

    //if(1 == checkdev("eth0"))
    {
        pthread_mutex_lock(&broadcast_mutex);
        TL_Video_FillOut_Server_Info_V3(&jbsearch);
        pthread_mutex_unlock(&broadcast_mutex);

        recvAddr.sin_family = AF_INET;       
        recvAddr.sin_port = htons(MULTI_BROADCAST_SREACH_RECV_PORT);
        recvAddr.sin_addr.s_addr = inet_addr(MULTI_BROADCAST_SREACH_IPADDR);
        sendBroadMsg(Broadcastfd, ulValue, &jbsearch, sizeof(TLNV_SERVER_MSG_DATA_EX_V3),recvAddr);

        //recvAddr.sin_family = AF_INET;       
        //recvAddr.sin_port = htons(MULTI_BROADCAST_SREACH_RECV_PORT_ADD);
        //recvAddr.sin_addr.s_addr = inet_addr(MULTI_BROADCAST_SREACH_IPADDR);
        //sendBroadMsg(Broadcastfd, ulValue, &jbsearch, sizeof(TLNV_SERVER_MSG_DATA_EX_V3),recvAddr);
    }

    return ret;
}

