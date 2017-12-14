/******************************************************************************
******************************************************************************/


//#include "tl_common.h"
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include "tlnettools.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <sys/prctl.h>
#include <sys/socket.h> 

#include "QMAPIType.h"
#include "QMAPIDataType.h"
#include "QMAPICommon.h"
#include "QMAPINetSdk.h"
#include "QMAPIErrno.h"
#include "QMAPI.h"

//#define ARP_DEBUG

#ifdef ARP_DEBUG
#define arp_err()
#else
#define arp_err()		\
{				\
	printf("%s(%d) - %d , %s \n", __FILE__, __LINE__, errno, strerror(errno)); \
}
#endif
void initial_arping();

int g_WifiArpThExiting = 0;
int g_WifiArpThExited = 1;

typedef struct
{
	int sockfd;

	int count;
	int singleSentCount;
	int brdSentCount;

	int dadFlag;
	int unicasting;

	struct in_addr srcAddr;
	struct in_addr dstAddr;
	struct sockaddr_ll srcAddrII;
	struct sockaddr_ll dstAddrII;

}ARPHANDLE;

static int ARP_init(ARPHANDLE *handle, char *srcaddr, char *dstaddr, int count);
static int ARP_release(ARPHANDLE arpHandle);
static int send_arp_pack(ARPHANDLE arpHandle);
static int recv_arp_pack(ARPHANDLE arpHandle);

static int ARP_init(ARPHANDLE *arpHandle, char *srcaddr, char *dstaddr, int count)
{
	int ret = -1;
	struct ifreq ifr;
	int ifindex = 0;
	int i = 0;
	char srcAddr[20];
	char dstAddr[20];

	if (arpHandle==NULL || srcaddr==NULL || dstaddr==NULL)
	{
		return -2;
	}
	if (!strcmp(srcaddr, dstaddr))
	{
		return -3;
	}

	memset(arpHandle, 0, sizeof(ARPHANDLE));

	strcpy(srcAddr, srcaddr);
	strcpy(dstAddr, dstaddr);
	char strInterface[128] = {'\0'};
	char strAddr[128] = {'\0'};

	char szInterface[32] = {0};
        strcpy(szInterface, "eth0");
	ret = get_gateway_netif(strInterface);
	if(ret == 0)
	{
	    get_ip_addr(strInterface,strAddr);
	}

	if( (strlen(strAddr) != 0 ) && (strcmp(strAddr,srcaddr) != 0) )
	{
	    strcpy(srcAddr,strAddr);
            strcpy(szInterface, strInterface);
	}
	
	ret = socket(PF_PACKET, SOCK_DGRAM, 0);
	if (ret < 0)
	{
		arp_err();
		return -1;
	}
	arpHandle->sockfd = ret;
	
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, szInterface, IFNAMSIZ - 1);
	ret = ioctl(arpHandle->sockfd, SIOCGIFINDEX, &ifr);
	if (ret < 0) 
	{
		arp_err();
		return -1;
	}
	ifindex = ifr.ifr_ifindex;
	ret = ioctl(arpHandle->sockfd, SIOCGIFFLAGS, (char *)&ifr);
	if (ret < 0) 
	{
		arp_err();
		return -1;	
	}
	if (!(ifr.ifr_flags & IFF_UP)) 
	{
		arp_err();
		return -1;
	}
	if (ifr.ifr_flags & (IFF_NOARP | IFF_LOOPBACK)) 
	{
		arp_err();
		return -1;
	}
	
	if (!inet_aton(dstAddr, &arpHandle->dstAddr)) 
	{
		struct hostent *ent = NULL;

		ent = gethostbyname2(dstAddr, AF_INET);
		if (!ent) 
		{
			arp_err();
			return -1;
		}
		//memcpy(&arpHandle->dstAddr, &ent->h_addr, 4);
	}	
	if (!inet_aton(srcAddr, &arpHandle->srcAddr)) 
	{
		arp_err();
		return -1;
	}

	arpHandle->srcAddrII.sll_family = AF_PACKET;
	arpHandle->srcAddrII.sll_ifindex = ifindex;
	arpHandle->srcAddrII.sll_protocol = htons(ETH_P_ARP);
	ret = bind(arpHandle->sockfd, (struct sockaddr *) &arpHandle->srcAddrII, sizeof(arpHandle->srcAddrII));
	if (ret == -1) 
	{
		arp_err();	
		return -1;
	}
	i = sizeof(arpHandle->srcAddrII);
	ret = getsockname(arpHandle->sockfd, (struct sockaddr *) &arpHandle->srcAddrII, (socklen_t*)&i);
	if (ret == -1) 
	{
		arp_err();
		return -1;
	}
	if (arpHandle->srcAddrII.sll_halen == 0) 
	{
		arp_err();
		return -1;
	}

	arpHandle->dstAddrII = arpHandle->srcAddrII;
	memset(arpHandle->dstAddrII.sll_addr, -1, arpHandle->dstAddrII.sll_halen);

	arpHandle->count = count;
	arpHandle->dadFlag = 0;
	arpHandle->unicasting = 0;	// single

	return 0;
}

static int ARP_release(ARPHANDLE arpHandle)
{
	if (arpHandle.sockfd)
	{
		close(arpHandle.sockfd);
	}
					
	return 0;
}

static int send_arp_pack(ARPHANDLE arpHandle)
{
	int ret = -1;
	unsigned char buf[256];
	struct arphdr *ah = (struct arphdr *) buf;
	unsigned char *p = (unsigned char *) (ah + 1);
	
	ah->ar_hrd = htons(arpHandle.srcAddrII.sll_hatype);
	ah->ar_hrd = htons(ARPHRD_ETHER);
	ah->ar_pro = htons(ETH_P_IP);
	ah->ar_hln = arpHandle.srcAddrII.sll_halen;
	ah->ar_pln = 4;
	ah->ar_op = htons(ARPOP_REQUEST);	// ARPOP_REPLY

	memcpy(p, &arpHandle.srcAddrII.sll_addr, ah->ar_hln);
	p += ah->ar_hln;
	memcpy(p, &arpHandle.srcAddr, 4);
	p += 4;
	memcpy(p, &arpHandle.dstAddrII.sll_addr, ah->ar_hln);	
	p += ah->ar_hln;
	memcpy(p, &arpHandle.dstAddr, 4);
	p += 4;

	ret = sendto(arpHandle.sockfd, buf, p-buf, 0, (struct sockaddr *)&arpHandle.dstAddrII, sizeof(struct sockaddr_ll));
	if (ret == p - buf) 
	{		
		if (arpHandle.count)
		{
			arpHandle.singleSentCount++;
			if (!arpHandle.unicasting)
			{
				arpHandle.brdSentCount++;
			}
		}
	}
	else
	{
		arp_err();
	}
	
	return 0;
}

/*

void send_arp_pack_pthread()
{
	int ret = -1;
	int arp_broadcast_fs;
	unsigned char buf[256];
	struct arphdr *ah = (struct arphdr *) buf;
	unsigned char *p = (unsigned char *) (ah + 1);
	
	//arp_broadcast_fs=

	while(1)
	{
		ah->ar_hrd = htons(ARPHRD_ETHER);
		ah->ar_pro = htons(ETH_P_IP);
		ah->ar_hln = 4;
		ah->ar_pln = 4;
		ah->ar_op = htons(ARPOP_REQUEST);	// ARPOP_REPLY
	
		memcpy(p, &arpHandle.srcAddrII.sll_addr, ah->ar_hln);
		p += ah->ar_hln;
		memcpy(p, &arpHandle.srcAddr, 4);
		p += 4;
		memcpy(p, &arpHandle.dstAddrII.sll_addr, ah->ar_hln);	
		p += ah->ar_hln;
		memcpy(p, &arpHandle.dstAddr, 4);
		p += 4;
	
		ret = sendto(arpHandle.sockfd, buf, p-buf, 0, (struct sockaddr *)&arpHandle.dstAddrII, sizeof(struct sockaddr_ll));
		
		if (ret == p - buf) 
		{		
			if (arpHandle.count)
			{
				arpHandle.singleSentCount++;
				if (!arpHandle.unicasting)
				{
					arpHandle.brdSentCount++;
				}
			}
		}
		else
		{
			arp_err();
		}
	}
}

*/


static int recv_arp_pack(ARPHANDLE arpHandle)
{
	char packet[4096];
	struct arphdr *ah = (struct arphdr *) packet;
	unsigned char *p = (unsigned char *) (ah + 1);
	struct in_addr src_ip, dst_ip;
	
	struct sockaddr_ll from;
	int ret = -1;
	int len = 0;
	struct timeval tv;
	fd_set fdset;
	
	/* set timeout */
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	
	/* set fd_set */
	FD_ZERO(&fdset);
	FD_SET(arpHandle.sockfd, &fdset);
	
	if ((ret = select(arpHandle.sockfd+1, &fdset, NULL, NULL, &tv)) == -1)
	{
		return -1;
	}
	else if(ret == 0)
	{
		return -1;
	}
	else
	{	
		len = sizeof(from);
		ret = recvfrom(arpHandle.sockfd, packet, sizeof(packet), 0, (struct sockaddr *) &from, (socklen_t*)&len);
		if (ret < 0)
		{
			return -1;
		}
	}

	// Filter out wild packets
	if (from.sll_pkttype != PACKET_HOST &&
		from.sll_pkttype != PACKET_BROADCAST &&
		from.sll_pkttype != PACKET_MULTICAST)
	{
		return -1;
	}

	// Only these types are recognised
	if (ah->ar_op != htons(ARPOP_REQUEST) && ah->ar_op != htons(ARPOP_REPLY))
	{
		return -1;
	}

	// ARPHRD check and this darned FDDI hack here
	if (ah->ar_hrd != htons(from.sll_hatype) &&
		(from.sll_hatype != ARPHRD_FDDI || ah->ar_hrd != htons(ARPHRD_ETHER)))
	{
		return -1;
	}

	// Protocol must be IP.
	if (ah->ar_pro != htons(ETH_P_IP))
	{
		return -1;
	}
	if (ah->ar_pln != 4)
	{
		return -1;
	}
	if (ah->ar_hln != arpHandle.srcAddrII.sll_halen)
	{
		return -1;
	}
	if (len < sizeof(*ah) + 2 * (4 + ah->ar_hln))
	{
		//return -1;
	}
	memcpy(&src_ip, p + ah->ar_hln, 4);
	memcpy(&dst_ip, p + ah->ar_hln + 4 + ah->ar_hln, 4);
	
	if (!arpHandle.dadFlag) 
	{
		if (src_ip.s_addr != arpHandle.dstAddr.s_addr)
		{
			return -1;
		}
		if (arpHandle.srcAddr.s_addr != dst_ip.s_addr)
		{
			return -1;
		}
		if (memcmp(p + ah->ar_hln + 4, &arpHandle.srcAddrII.sll_addr, ah->ar_hln))
		{
			return -1;
		}
	} 
	else 
	{
		if (src_ip.s_addr != arpHandle.dstAddr.s_addr)
		{
			return -1;
		}
		if (memcmp(p, &arpHandle.srcAddrII.sll_addr, arpHandle.srcAddrII.sll_halen) == 0)
		{
			return -1;
		}
		if (arpHandle.srcAddr.s_addr && arpHandle.srcAddr.s_addr != dst_ip.s_addr)
		{
			return -1;
		}
	}
	
	return 0;
}

static int ARP_Wifi_init(ARPHANDLE *handle, char *srcaddr, char *dstaddr, int count);
static int ARP_Wifi_release(ARPHANDLE *arpHandle);
static int send_arp_Wifi_pack(ARPHANDLE *arpHandle);
static int recv_arp_Wifi_pack(ARPHANDLE *arpHandle);
static int ARP_Wifi_init(ARPHANDLE *arpHandle, char *srcaddr, char *dstaddr, int count)
{
	int ret = -1;
	struct ifreq ifr;
	int ifindex = 0;
	int i = 0;
	char srcAddr[20];
	char dstAddr[20];

	if (arpHandle==NULL || srcaddr==NULL || dstaddr==NULL)
	{
		return -2;
	}
	if (!strcmp(srcaddr, dstaddr))
	{
		return -3;
	}

	memset(arpHandle, 0, sizeof(ARPHANDLE));

	strcpy(srcAddr, srcaddr);
	strcpy(dstAddr, dstaddr);
	char strInterface[128] = {'\0'};
	char strAddr[128] = {'\0'};

	char szInterface[32] = {0};
	strcpy(szInterface, "ce00");
	ret = get_gateway_netif(strInterface);
	if(ret == 0)
	{
	    get_ip_addr(strInterface,strAddr);
	}

	if( (strlen(strAddr) != 0 ) && (strcmp(strAddr,srcaddr) != 0) )
	{
	    strcpy(srcAddr,strAddr);
            strcpy(szInterface, strInterface);
	}
	
	ret = socket(PF_PACKET, SOCK_DGRAM, 0);
	strcpy(szInterface,"ce00");
	strcpy(srcAddr,srcaddr);
	if (ret < 0)
	{
		arp_err();
		return -1;
	}
	arpHandle->sockfd = ret;
	
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, szInterface, IFNAMSIZ - 1);
	ret = ioctl(arpHandle->sockfd, SIOCGIFINDEX, &ifr);
	if (ret < 0) 
	{
		arp_err();
		return -1;
	}
	ifindex = ifr.ifr_ifindex;
	ret = ioctl(arpHandle->sockfd, SIOCGIFFLAGS, (char *)&ifr);
	if (ret < 0) 
	{
		arp_err();
		return -1;	
	}
	if (!(ifr.ifr_flags & IFF_UP)) 
	{
		arp_err();
		return -1;
	}
	if (ifr.ifr_flags & (IFF_NOARP | IFF_LOOPBACK)) 
	{
		arp_err();
		return -1;
	}
	
	if (!inet_aton(dstAddr, &arpHandle->dstAddr)) 
	{
		struct hostent *ent = NULL;

		ent = gethostbyname2(dstAddr, AF_INET);
		if (!ent) 
		{
			arp_err();
			return -1;
		}
		//memcpy(&arpHandle->dstAddr, &ent->h_addr, 4);
	}	
	if (!inet_aton(srcAddr, &arpHandle->srcAddr)) 
	{
		arp_err();
		return -1;
	}

	arpHandle->srcAddrII.sll_family = AF_PACKET;
	arpHandle->srcAddrII.sll_ifindex = ifindex;
	arpHandle->srcAddrII.sll_protocol = htons(ETH_P_ARP);
	ret = bind(arpHandle->sockfd, (struct sockaddr *) &arpHandle->srcAddrII, sizeof(arpHandle->srcAddrII));
	if (ret == -1) 
	{
		arp_err();	
		return -1;
	}
	i = sizeof(arpHandle->srcAddrII);
	ret = getsockname(arpHandle->sockfd, (struct sockaddr *) &arpHandle->srcAddrII, (socklen_t*)&i);
	if (ret == -1) 
	{
		arp_err();
		return -1;
	}
	if (arpHandle->srcAddrII.sll_halen == 0) 
	{
		arp_err();
		return -1;
	}

	arpHandle->dstAddrII = arpHandle->srcAddrII;
	memset(arpHandle->dstAddrII.sll_addr, -1, arpHandle->dstAddrII.sll_halen);

	arpHandle->count = count;
	arpHandle->dadFlag = 0;
	arpHandle->unicasting = 0;	// single

	return 0;
}

static int ARP_Wifi_release(ARPHANDLE *arpHandle)
{
	if (arpHandle->sockfd)
	{
		close(arpHandle->sockfd);
		memset(arpHandle, 0, sizeof(ARPHANDLE));
		arpHandle->sockfd = -1;
	}
					
	return 0;
}

static int send_arp_Wifi_pack(ARPHANDLE *arpHandle)
{
	int ret = -1;
	unsigned char buf[256];
	struct arphdr *ah = (struct arphdr *) buf;
	unsigned char *p = (unsigned char *) (ah + 1);

	if(arpHandle->sockfd < 0)
	{
		char Wifi_srcaddr[32];
		//struct in_addr	Wifi_NetIpAddr;
		QMAPI_NET_WIFI_CONFIG wconfig;
	
		if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_WIFICFG, 0, &wconfig, sizeof(QMAPI_NET_WIFI_CONFIG)) != QMAPI_SUCCESS)
		{
			err();
			return -1;
		}
		strcpy(Wifi_srcaddr , wconfig.dwNetIpAddr.csIpV4);
		ret = ARP_Wifi_init(arpHandle, Wifi_srcaddr, "255.255.255.255", 30);
		if(ret)
		{
			return -1;
		}
	}
	
	ah->ar_hrd = htons(arpHandle->srcAddrII.sll_hatype);
	ah->ar_hrd = htons(ARPHRD_IEEE802);
	ah->ar_pro = htons(ETH_P_IP);
	ah->ar_hln = arpHandle->srcAddrII.sll_halen;
	ah->ar_pln = 4;
	ah->ar_op = htons(ARPOP_REQUEST);	// ARPOP_REPLY

	memcpy(p, &arpHandle->srcAddrII.sll_addr, ah->ar_hln);
	p += ah->ar_hln;
	memcpy(p, &arpHandle->srcAddr, 4);
	p += 4;
	memcpy(p, &arpHandle->dstAddrII.sll_addr, ah->ar_hln);	
	p += ah->ar_hln;
	memcpy(p, &arpHandle->dstAddr, 4);
	p += 4;

	ret = sendto(arpHandle->sockfd, buf, p-buf, 0, (struct sockaddr *)&arpHandle->dstAddrII, sizeof(struct sockaddr_ll));
	if (ret == p - buf) 
	{		
		if (arpHandle->count)
		{
			arpHandle->singleSentCount++;
			if (!arpHandle->unicasting)
			{
				arpHandle->brdSentCount++;
			}
		}
	}
	else
	{
		arp_err();
	}
	
	return 0;
}


static int recv_arp_Wifi_pack(ARPHANDLE *arpHandle)
{
	char packet[4096];
	struct arphdr *ah = (struct arphdr *) packet;
	unsigned char *p = (unsigned char *) (ah + 1);
	struct in_addr src_ip, dst_ip;
	
	struct sockaddr_ll from;
	int ret = -1;
	int len = 0;
	struct timeval tv;
	fd_set fdset;
	
	/* set timeout */
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	
	/* set fd_set */
	FD_ZERO(&fdset);
	FD_SET(arpHandle->sockfd, &fdset);
	
	if ((ret = select(arpHandle->sockfd+1, &fdset, NULL, NULL, &tv)) == -1)
	{
		printf("%s(%d)\n", __FILE__, __LINE__);
		return -1;
	}
	else if(ret == 0)
	{		
		printf("%s(%d)\n", __FILE__, __LINE__);
		return -1;
	}
	else
	{	
		len = sizeof(from);
		ret = recvfrom(arpHandle->sockfd, packet, sizeof(packet), 0, (struct sockaddr *) &from, (socklen_t*)&len);
		if (ret < 0)
		{
			printf("%s(%d)\n", __FILE__, __LINE__);
			return -1;
		}
	}

	// Filter out wild packets
	if (from.sll_pkttype != PACKET_HOST &&
		from.sll_pkttype != PACKET_BROADCAST &&
		from.sll_pkttype != PACKET_MULTICAST)
	{	
		printf("%s(%d)\n", __FILE__, __LINE__);
		return -1;
	}

	// Only these types are recognised
	if (ah->ar_op != htons(ARPOP_REQUEST) && ah->ar_op != htons(ARPOP_REPLY))
	{
		printf("%s(%d)\n", __FILE__, __LINE__);
		return -1;
	}

	// ARPHRD check and this darned FDDI hack here
	if (ah->ar_hrd != htons(from.sll_hatype) &&
		(from.sll_hatype != ARPHRD_FDDI || ah->ar_hrd != htons(ARPHRD_ETHER)))
	{
		printf("%s(%d)\n", __FILE__, __LINE__);
		return -1;
	}

	// Protocol must be IP.
	if (ah->ar_pro != htons(ETH_P_IP))
	{	
		printf("%s(%d)\n", __FILE__, __LINE__);
		return -1;
	}
	if (ah->ar_pln != 4)
	{
		printf("%s(%d)\n", __FILE__, __LINE__);
		return -1;
	}
	if (ah->ar_hln != arpHandle->srcAddrII.sll_halen)
	{	
		printf("%s(%d)\n", __FILE__, __LINE__);
		return -1;
	}
	if (len < sizeof(*ah) + 2 * (4 + ah->ar_hln))
	{
		//return -1;
	}
	memcpy(&src_ip.s_addr, p + ah->ar_hln, 4);

	memcpy(&dst_ip.s_addr, p + ah->ar_hln + 4 + ah->ar_hln, 4);
	//printf("%s  %d, 00000000,%8x,%8x\n",__FUNCTION__, __LINE__,src_ip.s_addr,dst_ip.s_addr);
	
	if (!arpHandle->dadFlag) 
	{
		
		if (src_ip.s_addr != arpHandle->dstAddr.s_addr)
		{
			//printf("%s(%d)\n", __FILE__, __LINE__);
			return -1;
		}
		
		if (arpHandle->srcAddr.s_addr != dst_ip.s_addr)
		{
			//printf("%s(%d)\n", __FILE__, __LINE__);
			return -1;
		}
		
		if (memcmp(p + ah->ar_hln + 4, &arpHandle->srcAddrII.sll_addr, ah->ar_hln))
		{
			printf("%s(%d)\n", __FILE__, __LINE__);
			return -1;
		}
		
	} 
	else 
	{
		if (src_ip.s_addr != arpHandle->dstAddr.s_addr)
		{
			printf("%s(%d)\n", __FILE__, __LINE__);
			return -1;
		}
		if (memcmp(p, &arpHandle->srcAddrII.sll_addr, arpHandle->srcAddrII.sll_halen) == 0)
		{
			printf("%s(%d)\n", __FILE__, __LINE__);
			return -1;
		}
		if (arpHandle->srcAddr.s_addr && arpHandle->srcAddr.s_addr != dst_ip.s_addr)
		{
			printf("%s(%d)\n", __FILE__, __LINE__);
			return -1;
		}
	}
	
	return 0;
}


void *start_Wifi_arp()
{
	///char *srcaddr, char *dstaddr, int count
	int ret = -1;
	int i = 30;
	char dstaddr[32];
	char Wifi_srcaddr[32];
	//struct in_addr	Wifi_NetIpAddr;
	ARPHANDLE Wifi_arpHandle;
	prctl(PR_SET_NAME, (unsigned long)"WIFI ARP", 0,0,0);
	g_WifiArpThExiting = 0;
	g_WifiArpThExited  = 0;
	memset(dstaddr , 0 , 32);
	memset(Wifi_srcaddr , 0 , 32);
	QMAPI_NET_WIFI_CONFIG wconfig;
	
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_WIFICFG, 0, &wconfig, sizeof(QMAPI_NET_WIFI_CONFIG)) != QMAPI_SUCCESS)
	{
		err();
		return (void *)-1;
	}
	strcpy(Wifi_srcaddr , wconfig.dwNetIpAddr.csIpV4);
	
	sprintf(dstaddr , "%s" , "255.255.255.255");	
	printf("Wifi_NetIpAddr  [%s]\n",  Wifi_srcaddr);
	ret = ARP_Wifi_init(&Wifi_arpHandle, Wifi_srcaddr , dstaddr , 30);
	if (ret < 0)
	{
		ARP_Wifi_release(&Wifi_arpHandle);
		g_WifiArpThExited = 1;
		return (void *)-1;
	}
//	while (i > 0)
	while (0 == g_WifiArpThExiting)
	{
	// need fixme yi.zhang
	//	if (g_tl_updateflash.bUpdatestatus)
		{
	//		break;
		}
		if (1 == g_WifiArpThExiting)
		{
			break;
		}
		//Wifi   arp
		ret = send_arp_Wifi_pack(&Wifi_arpHandle);
		if (ret == 0)
		{
		
		}
		else
		{	
			g_WifiArpThExited = 1;
			g_WifiArpThExiting = 0;
		}
		
		ret = recv_arp_Wifi_pack(&Wifi_arpHandle);
		if (ret == 0)	//ip³åÍ»
		{
			ARP_Wifi_release(&Wifi_arpHandle);
		}
		i--;
		sleep(3);
	}
	ARP_Wifi_release(&Wifi_arpHandle);
#ifdef ARP_DEBUG
	printf("Warning IP(%s) is not exist.\n", dstaddr);
#endif
	g_WifiArpThExited  = 1;
	g_WifiArpThExiting = 0;
	return (void *)-2;
}
int start_arp()
{
	///char *srcaddr, char *dstaddr, int count
	int ret = -1;
	int i = 30;
	char srcaddr[32] , dstaddr[32];
	//struct in_addr	NetIpAddr;
	ARPHANDLE arpHandle;
	prctl(PR_SET_NAME, (unsigned long)"ARP", 0,0,0);
	
	memset(srcaddr , 0 , 32);
	memset(dstaddr , 0 , 32);

	QMAPI_NET_NETWORK_CFG ninfo;
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, 0, &ninfo, sizeof(QMAPI_NET_NETWORK_CFG)) != QMAPI_SUCCESS)
	{
		err();
		return -1;
	}
    strcpy(srcaddr , ninfo.stEtherNet[0].strIPAddr.csIpV4);
	
	sprintf(dstaddr , "%s" , "255.255.255.255");	
	//printf("NetIpAddr [%d] [%s]\n", NetIpAddr.s_addr , srcaddr);
	//printf("[1]  ARP_init \n");
	ret = ARP_init(&arpHandle, srcaddr , dstaddr , 30);
	//printf("[2]  ARP_init [%d]\n" , ret);
	if (ret < 0)
	{
		ARP_release(arpHandle);
		return -1;
	}
	//while (i > 0)
	while (1)
	{
	// need fixme yi.zhang
	//	if (g_tl_updateflash.bUpdatestatus)
		{
	//		break;
		}
		ret = send_arp_pack(arpHandle);
		if (ret == 0)
		{	
		}
		else
		{	
		}
		
		ret = recv_arp_pack(arpHandle);
		if (ret == 0)
		{
			ARP_release(arpHandle);			
		}
		i--;
		sleep(3);
	}
	ARP_release(arpHandle);

	return -1;
}

void initial_arping()
{
	pthread_t arp_id;
	pthread_create(&arp_id , NULL , (void *)&start_arp , NULL);
	return ;
}
void initial_Wifi_arping()
{
	pthread_t arp_id;
	pthread_attr_t	attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	pthread_create(&arp_id, &attr, start_Wifi_arp, NULL);
	pthread_attr_destroy( &attr ); 
	return ;
}

