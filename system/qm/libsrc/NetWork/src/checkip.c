#include <netinet/if_ether.h> 
#include <net/if_arp.h> 
#include <sys/poll.h>
#include <errno.h>
//#include <net/if.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/prctl.h>
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <stdlib.h>

//#include <net/route.h>
#include "tlnettools.h"
#include "wireless.h"
#include "system_msg.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
    
#define uint8_t		unsigned char
#define uint16_t	unsigned short
#define uint32_t	unsigned int
    
//这里是arp包的格式,其中的数据格式都是宏了，比如uint_8_t为无符char. 
typedef struct arpMsg{
    /*Ethernet header*/ 
    uint8_t		h_dest[6];		/*00 destination ether addr*/
    uint8_t		h_source[6];	/*06 source ether addr*/
    uint16_t	h_proto;		/*0c packet type ID field*/
    
    /* ARP packet */ 
    uint16_t	htype;			/* 0e hardware type (must be ARPHRD_ETHER) */ 
    uint16_t	ptype;			/* 10 protocol type (must be ETH_P_IP) */ 
    uint8_t		hlen;			/* 12 hardware address length (must be 6) */ 
    uint8_t		plen;			/* 13 protocol address length (must be 4) */ 
    uint16_t	operation;		/* 14 ARP opcode */ 
    uint8_t		sHaddr[6];		/* 16 sender's hardware address */ 
    uint8_t		sInaddr[4];		/* 1c sender's IP address */ 
    uint8_t		tHaddr[6];		/* 20 target's hardware address */ 
    uint8_t		tInaddr[4];		/* 26 target's IP address */ 
    uint8_t		pad[18];		/* 2a pad for min. ethernet payload (60 bytes) */ 
}ARP_PACKED; 

enum { 
    ARP_MSG_SIZE = 0x2a 
}; 

int setsockopt_broadcast(int fd)
{
	const int const_int_1 = 1;
	return setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &const_int_1, sizeof(const_int_1));
} 

/*
char * safe_strncpy(char *dst, const char *src, size_t size)
{
    dst[size-1] = '\0';
    return strncpy(dst, src, size-1);
}
*/

 

#define SIOCETHTOOL		0x8946
#define ETHTOOL_GLINK	0x0000000a 
/* for passing single values */
typedef struct stget_net_value {
	unsigned int cmd;
	unsigned int data;
}get_net_value;


/**
返回值0说明网络断开，返回0程序执行失败，返回 > 0说明网络正常
**/
int checkdev(const char *lpdev)
{
	struct ifreq ifr;
	get_net_value edata;
	int fd;
	int err;
	
	/* Setup our control structures. */
	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, lpdev);
	
	/* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		printf("Cannot get control socket, errno:%d, %s\n", errno, strerror(errno));
		return -1;
	}
	edata.cmd = ETHTOOL_GLINK;
	ifr.ifr_data = (caddr_t)&edata;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if (err != 0)
	{
		printf("%s ioctl error %d %s\n",__FUNCTION__,errno,strerror(errno));
		close(fd);
		return -1;
	}
	close(fd);
//	printf("## checkdev edata.data = %d \n",edata.data);
	return edata.data;
}

#ifndef TL_Q3_PLATFORM
extern int g_bWifiConnected;
extern unsigned char g_wifistatus;
#endif

int CheckIPConflict(unsigned int test_ip, unsigned char *from_mac, char *ifname, unsigned char IsWifi)
{
	int rv = 1;             /* "no reply received" yet */
	struct sockaddr addr;   /* for interface name */
	struct arpMsg arp;
	int s;

	//检测IP冲突
	s = socket(PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP));
	if (s == -1) {
		printf("can't create raw socket %s\n" , strerror(errno));
		return -1;
	}

	if (setsockopt_broadcast(s) == -1) {
		printf("cannot enable bcast on raw socket %s\n" , strerror(errno));
		goto ret;
	}
	//进行组包，由于是要广播，因此目的mac地址为全0. 
	/* send arp request */
	memset(&arp, 0, sizeof(arp));
	memset(arp.h_dest, 0xff, 6);                    /* MAC DA */
	memcpy(arp.h_source, from_mac, 6);              /* MAC SA */
	arp.h_proto = htons(ETH_P_ARP);                 /* protocol type (Ethernet) */
	if(IsWifi)
		arp.htype = htons(ARPHRD_IEEE802);
	else
		arp.htype = htons(ARPHRD_ETHER);                /* hardware type */
	
	arp.ptype = htons(ETH_P_IP);                    /* protocol type (ARP message) */
	arp.hlen = 6;                                   /* hardware address length */
	arp.plen = 4;                                   /* protocol address length */
	arp.operation = htons(ARPOP_REQUEST);           /* ARP op code */
	memcpy(arp.sHaddr, from_mac, 6);                /* source hardware address */
	//不能填源IP，不然冲突的IP不会发包给你
	//memcpy(arp.sInaddr, &from_ip, sizeof(from_ip)); /* source IP address */
	/* tHaddr is zero-fiiled */                     /* target hardware address */
	memcpy(arp.tInaddr, &test_ip, sizeof(test_ip)); /* target IP address */
	
	memset(&addr, 0, sizeof(addr));
	safe_strncpy(addr.sa_data, ifname, sizeof(addr.sa_data));
	//广播arp包. 
	if (sendto(s, &arp, sizeof(arp), 0, &addr, sizeof(addr)) < 0) {
		//  TODO: error message? caller didn't expect us to fail,
		// just returning 1 "no reply received" misleads it.
		printf("check ip fail send arp paket %s\n" , strerror(errno));
		goto ret;
	}
	
	//等待时间，超时则认为此ip地址可用
	fd_set struReadFdSet;
	struct timeval tv;
	struct timeval tv_end, tv_cur;
	long long remain_time;

	gettimeofday(&tv_end, NULL);
	tv_end.tv_sec += 2;
	
	do {

		gettimeofday(&tv_cur, NULL);
		remain_time = ((tv_end.tv_sec-tv_cur.tv_sec)*1000000+((tv_end.tv_usec-tv_cur.tv_usec)));
		if(remain_time<=0)
		{
			break;
		}

		/* set timeout */
		tv.tv_sec = remain_time/1000000;
		tv.tv_usec = remain_time%1000000;
		FD_ZERO(&struReadFdSet);
		FD_SET(s, &struReadFdSet);
		
		int r;
		r =  select(s+1, &struReadFdSet, NULL, NULL, &tv);
		if (r < 0)
		{
			printf("check ip fail select error %s\n" , strerror(errno));
			break;
		}
		else if(r == 0)
		{
			break;
		}
		
		if (r) {
			unsigned int arpInaddr;
			//读取返回数据. 
			memset(&arp, 0, sizeof(arp));
			r = read(s, &arp, sizeof(arp));
			if (r < 0)
			{
				printf("check ip fail read error %s\n" , strerror(errno));
				break;
			}
			memcpy(&arpInaddr,arp.sInaddr,sizeof(int));
			
			//struct in_addr	NetIpAddr;
			//NetIpAddr.s_addr = arpInaddr;
			//printf("%s  %d, dddddddd,0x%x(%s)\n",__FUNCTION__, __LINE__,arpInaddr,inet_ntoa(NetIpAddr));
			//printf("%s %x:%x:%x:%x:%x:%x\n",__FUNCTION__,arp.sHaddr[0],arp.sHaddr[1],arp.sHaddr[2],arp.sHaddr[3],arp.sHaddr[4],arp.sHaddr[5]);
			
			if (r >= ARP_MSG_SIZE && arp.operation == htons(ARPOP_REPLY)
				&& arpInaddr == test_ip
				&& memcmp(arp.sHaddr, from_mac, 6) != 0) 
			{
				rv = 0;
				break;
			}
			if (r >= ARP_MSG_SIZE && arp.operation == htons(ARPOP_REPLY)
				&& arpInaddr != test_ip
				&& memcmp(arp.sHaddr, from_mac, 6) == 0) 
			{
				rv = 0;
				break;
			}
		
		}
	} while (1);
	
ret:
	close(s);
 
	return rv;
}

//#define WIFI_DEV "ce00"
#define ETH_DEV "eth0"
#define WL_IP_CONFLICT 6
#define WL_OK 0

void *check_ip_thread()
{
    int ret = 0;
    char ip[32] = {0};
    char *ifname = NULL;
    unsigned int IPAddr = 0;
    unsigned char IsWifi = 0;
    unsigned char MacAddr[6] = {0};
    prctl(PR_SET_NAME, (unsigned long)"checkip", 0,0,0);

    while(1)
    {
        //  need fixme yi.zhang
        //    if(g_tl_updateflash.bUpdatestatus)
        //    {
        //        break;
        //    }
        //	if(g_tl_globel_param.tl_Wifi_config.bWifiEnable>0)
        {

        }
        //	else
        {
            ifname = ETH_DEV;
            IsWifi = 0;
        }

        memset(ip, 0, 32);
        memset(MacAddr, 0, 6);

        ret = net_get_hwaddr(ifname, MacAddr);
        if(ret<0)
            goto gotosleep;
        ret = get_ip_addr(ifname, ip);
        if(ret<0)
            goto gotosleep;
        IPAddr = inet_addr(ip);

        //printf("%s  %d, aaaaaaaaaaa,%s  ,mac:%2x:%2x:%2x:%2x:%2x:%2x\n",__FUNCTION__, __LINE__,ip,MacAddr[0],MacAddr[1],MacAddr[2],MacAddr[3],MacAddr[4],MacAddr[5]);
        ret = CheckIPConflict(IPAddr, MacAddr, ifname, IsWifi);
        if(ret == 0)
        {
            printf("Warning !!!!!!! IP(%s) is conflit.\n", ip);
            // need fixme yi.zhang
            // g_tl_globel_param.stNetworkStatus.bIPConflict = 1;
        }
        else
        {
        }

        //check second ip confilt
        ret = get_ip_addr("eth0:1", ip);
        if(ret<0)
            goto gotosleep;

        IPAddr = inet_addr(ip);
        ret = CheckIPConflict(IPAddr, MacAddr, ifname, IsWifi);
        if(ret == 0)
        {
            printf("Warning !!!!!!! SecondIP(%s) is conflit.\n", ip);
            struct timeval tpstart;
            gettimeofday(&tpstart,NULL);
            srand(tpstart.tv_usec);

            char Sip[16] = {0};	
            char syscmd[64] = {0};
            snprintf(Sip , sizeof(Sip) , "10.%d.%d.%d" , rand() & 0xFF , rand() & 0xFF , rand() & 0xFF);
            printf("SRDK:Set Second IP addr to : %s \n" , Sip);
            snprintf(syscmd , sizeof(syscmd) , "ifconfig eth0:1 %s netmask 255.0.0.0" , Sip);
            SystemCall_msg(syscmd,SYSTEM_MSG_BLOCK_RUN);
        }

        if(checkdev(ETH_DEV) <= 0)
        {
            // need fixme yi.zhang
            //g_tl_globel_param.stNetworkStatus.bNetBroke = 1;
        }

gotosleep:
        sleep(8);
    }

	return (void *)0;
}

int check_ip_start()
{
	int ret;
	pthread_t tid;
	ret = pthread_create(&tid, NULL, (void *)&check_ip_thread, NULL);
	if(ret)
	{
		printf("%s  %d, create thread fail!ret=%d,%s(errno:%d)\n",
			__FUNCTION__, __LINE__, ret, strerror(errno), errno);
	}

	return ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
