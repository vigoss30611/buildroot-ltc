/******************************************************************************
******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
//#include <net/if.h>
#include <linux/route.h>
#include <net/if_arp.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>     
#include <linux/if_ether.h>
#include <stddef.h>		/* offsetof */
#include <sys/stat.h>

#include <sys/socket.h>
#include <linux/if.h>
#include <linux/sockios.h>

#include "system_msg.h"
#include "tlnettools.h"
#include "QMAPINetWork.h"

#define PACKET_SIZE			4096
#define MAX_WAIT_TIME		5

#define ICMP_ECHO			8
#define ICMP_ECHOREPLY		0

int g_ping_nsend = 0;


struct arg1opt 
{
	const char *name;
	unsigned short selector;
	unsigned short ifr_offset;
};

struct options 
{
	const char *name;
	const unsigned char  flags;
	const unsigned char  arg_flags;
	const unsigned short selector;
};

#define ifreq_offsetof(x)  offsetof(struct ifreq, x)

#define IFF_DYNAMIC     0x8000  /* dialup device with changing addresses */

#define N_CLR            0x01
#define M_CLR            0x02
#define N_SET            0x04
#define M_SET            0x08
#define N_ARG            0x10
#define M_ARG            0x20

#define M_MASK           (M_CLR | M_SET | M_ARG)
#define N_MASK           (N_CLR | N_SET | N_ARG)
#define SET_MASK         (N_SET | M_SET)
#define CLR_MASK         (N_CLR | M_CLR)
#define SET_CLR_MASK     (SET_MASK | CLR_MASK)
#define ARG_MASK         (M_ARG | N_ARG)

/*
 * Here are the bit masks for the "arg_flags" member of struct options below.
 */

/*
 * cast type:
 *   00 int
 *   01 char *
 *   02 HOST_COPY in_ether
 *   03 HOST_COPY INET_resolve
 */
#define A_CAST_TYPE      0x03
/*
 * map type:
 *   00 not a map type (mem_start, io_addr, irq)
 *   04 memstart (unsigned long)
 *   08 io_addr  (unsigned short)
 *   0C irq      (unsigned char)
 */
#define A_MAP_TYPE       0x0C
#define A_ARG_REQ        0x10	/* Set if an arg is required. */
#define A_NETMASK        0x20	/* Set if netmask (check for multiple sets). */
#define A_SET_AFTER      0x40	/* Set a flag at the end. */
#define A_COLON_CHK      0x80	/* Is this needed?  See below. */

/*
 * These defines are for dealing with the A_CAST_TYPE field.
 */
#define A_CAST_CHAR_PTR  0x01
#define A_CAST_RESOLVE   0x01
#define A_CAST_HOST_COPY 0x02
#define A_CAST_HOST_COPY_IN_ETHER    A_CAST_HOST_COPY
#define A_CAST_HOST_COPY_RESOLVE     (A_CAST_HOST_COPY | A_CAST_RESOLVE)

/*
 * These defines are for dealing with the A_MAP_TYPE field.
 */
#define A_MAP_ULONG      0x04	/* memstart */
#define A_MAP_USHORT     0x08	/* io_addr */
#define A_MAP_UCHAR      0x0C	/* irq */

/*
 * Define the bit masks signifying which operations to perform for each arg.
 */

#define ARG_METRIC       (A_ARG_REQ /*| A_CAST_INT*/)
#define ARG_MTU          (A_ARG_REQ /*| A_CAST_INT*/)
#define ARG_TXQUEUELEN   (A_ARG_REQ /*| A_CAST_INT*/)
#define ARG_MEM_START    (A_ARG_REQ | A_MAP_ULONG)
#define ARG_IO_ADDR      (A_ARG_REQ | A_MAP_ULONG)
#define ARG_IRQ          (A_ARG_REQ | A_MAP_UCHAR)
#define ARG_DSTADDR      (A_ARG_REQ | A_CAST_HOST_COPY_RESOLVE)
#define ARG_NETMASK      (A_ARG_REQ | A_CAST_HOST_COPY_RESOLVE | A_NETMASK)
#define ARG_BROADCAST    (A_ARG_REQ | A_CAST_HOST_COPY_RESOLVE | A_SET_AFTER)
#define ARG_HW           (A_ARG_REQ | A_CAST_HOST_COPY_IN_ETHER)
#define ARG_POINTOPOINT  (A_CAST_HOST_COPY_RESOLVE | A_SET_AFTER)
#define ARG_KEEPALIVE    (A_ARG_REQ | A_CAST_CHAR_PTR)
#define ARG_OUTFILL      (A_ARG_REQ | A_CAST_CHAR_PTR)
#define ARG_HOSTNAME     (A_CAST_HOST_COPY_RESOLVE | A_SET_AFTER | A_COLON_CHK)

static const struct arg1opt Arg1Opt[] = {
	{"SIOCSIFMETRIC",  SIOCSIFMETRIC,  ifreq_offsetof(ifr_metric)},
	{"SIOCSIFMTU",     SIOCSIFMTU,     ifreq_offsetof(ifr_mtu)},
	{"SIOCSIFTXQLEN",  SIOCSIFTXQLEN,  ifreq_offsetof(ifr_qlen)},
	{"SIOCSIFDSTADDR", SIOCSIFDSTADDR, ifreq_offsetof(ifr_dstaddr)},
	{"SIOCSIFNETMASK", SIOCSIFNETMASK, ifreq_offsetof(ifr_netmask)},
	{"SIOCSIFBRDADDR", SIOCSIFBRDADDR, ifreq_offsetof(ifr_broadaddr)},
	{"SIOCSIFHWADDR",  SIOCSIFHWADDR,  ifreq_offsetof(ifr_hwaddr)},
	{"SIOCSIFDSTADDR", SIOCSIFDSTADDR, ifreq_offsetof(ifr_dstaddr)},
	{"SIOCSIFMAP",     SIOCSIFMAP,     ifreq_offsetof(ifr_map.mem_start)},
	{"SIOCSIFMAP",     SIOCSIFMAP,     ifreq_offsetof(ifr_map.base_addr)},
	{"SIOCSIFMAP",     SIOCSIFMAP,     ifreq_offsetof(ifr_map.irq)},
	/* Last entry if for unmatched (possibly hostname) arg. */
	{"SIOCSIFADDR",    SIOCSIFADDR,    ifreq_offsetof(ifr_addr)},
};


static const struct options OptArray[] = {
	{"metric",       N_ARG,         ARG_METRIC,      0},
    {"mtu",          N_ARG,         ARG_MTU,         0},
	{"txqueuelen",   N_ARG,         ARG_TXQUEUELEN,  0},
	{"dstaddr",      N_ARG,         ARG_DSTADDR,     0},
	{"netmask",      N_ARG,         ARG_NETMASK,     0},
	{"broadcast",    N_ARG | M_CLR, ARG_BROADCAST,   IFF_BROADCAST},
	{"hw",           N_ARG,         ARG_HW,          0},
	{"pointopoint",  N_ARG | M_CLR, ARG_POINTOPOINT, IFF_POINTOPOINT},
	{"mem_start",    N_ARG,         ARG_MEM_START,   0},
	{"io_addr",      N_ARG,         ARG_IO_ADDR,     0},
	{"irq",          N_ARG,         ARG_IRQ,         0},
	{"arp",          N_CLR | M_SET, 0,               IFF_NOARP},
	{"trailers",     N_CLR | M_SET, 0,               IFF_NOTRAILERS},
	{"promisc",      N_SET | M_CLR, 0,               IFF_PROMISC},
	{"multicast",    N_SET | M_CLR, 0,               IFF_MULTICAST},
	{"allmulti",     N_SET | M_CLR, 0,               IFF_ALLMULTI},
	{"dynamic",      N_SET | M_CLR, 0,               IFF_DYNAMIC},
	{"up",           N_SET        , 0,               (IFF_UP | IFF_RUNNING)},
	{"down",         N_CLR        , 0,               IFF_UP},
	{ NULL,          0,             ARG_HOSTNAME,    (IFF_UP | IFF_RUNNING)}
};

int check_IP_Valid(const char* p)
{
    int n[4];
    char c[4];
    if (sscanf(p, "%d%c%d%c%d%c%d%c",
        &n[0], &c[0], &n[1], &c[1],
        &n[2], &c[2], &n[3], &c[3])
        == 7)
    {
        int i;
        for(i = 0; i < 3; ++i)
            if (c[i] != '.')
                return 0;
        for(i = 0; i < 4; ++i)
            if (n[i] > 255 || n[i] < 0)
                return 0;
            return 1;
    }
    return 0;
}

int IsSubnetMask(char* pSubnet)  
{  
    if(check_IP_Valid(pSubnet))  
    {  
        unsigned int b = 0, i, n[4];  
        sscanf(pSubnet, "%u.%u.%u.%u", &n[3], &n[2], &n[1], &n[0]);  
        for(i = 0; i < 4; ++i) //将子网掩码存入32位无符号整型  
            b += n[i] << (i * 8);   
        b = ~b + 1;  
        if((b & (b - 1)) == 0)   //判断是否为2^n  
            return 1;  
    }  
    return 0;
}

int check_gateway(char *strIp, char *strMask, char *strGateway)
{
    unsigned long ip, mask, gateway;
    
    if(!strIp || !strMask || !strGateway)
        return -1;
    
    ip = (unsigned long)inet_addr(strIp);
    mask = (unsigned long)inet_addr(strMask);
    gateway = (unsigned long)inet_addr(strGateway);

    unsigned long net = ip & mask;

    if(net != (mask & gateway))
    {
        unsigned int n[4]={0};
        struct in_addr addr;
        char tmpbuf[32] = {0}, buf[32] = {0};

        addr.s_addr = net;

        strcpy(buf, inet_ntoa(addr));

        sscanf(buf, "%u.%u.%u.%u", &n[0], &n[1], &n[2], &n[3]);
        n[3] +=1;

        sprintf(tmpbuf, "%u.%u.%u.%u", n[0], n[1], n[2], n[3]);

        printf("%s  %d, Invalid gateway:%s,force to set a new gateway:%s\n",__FUNCTION__, __LINE__, strGateway,tmpbuf);
        strcpy(strGateway, tmpbuf);
        strGateway[strlen(tmpbuf)] = 0;
    }

    return 0;
}

/******************************************************************************
* oˉêy??3?￡oin_ether
* 1??ü?èê?￡oμ??·×a??
* ê?è?2?êy￡ochar *bufp				×?·???IPμ??·	
* ê?3?2?êy￡ostruct sockaddr *sap	×a??IPμ??·
* ·μ ?? ?μ￡o3é1?￡o0￡?ê§°ü￡o-1
* DT??????: ?T
* ?????μ?÷: ?T
********************************************************************************/
int in_ether(char *bufp, struct sockaddr *sap)
{
	unsigned char *ptr;
	int i, j;
	unsigned char val;
	unsigned char c;

	sap->sa_family = ARPHRD_ETHER;
	ptr = (unsigned char *)(sap->sa_data);

	i = 0;
	do {
		j = val = 0;

		/* We might get a semicolon here - not required. */
		if (i && (*bufp == ':')) {
			bufp++;
		}

		do {
			c = (unsigned char)(*bufp);
			if (((unsigned char)(c - '0')) <= 9) {
				c -= '0';
			} else if (((unsigned char)((c|0x20) - 'a')) <= 5) {
				c = (c|0x20) - ('a'-10);
			} else if (j && (c == ':' || c == 0)) {
				break;
			} else {
				return -1;
			}
			++bufp;
			val <<= 4;
			val += c;
		} while (++j < 2);
		*ptr++ = val;
	} while (++i < ETH_ALEN);

	return (int) (*bufp);	/* Error if we don't end at end of string. */
}

/******************************************************************************
* oˉêy??3?￡osafe_strncpy
* 1??ü?èê?￡o×?·???°2è???±?
* ê?è?2?êy￡oconst char *src	??×?·???
*			size_t size		òa??±?μ?×??úêy
* ê?3?2?êy￡ochar *dst		??±ê×?·???
* ·μ ?? ?μ￡o??D??μ￡?êμ?ê??±?×??úêy
* DT??????: ?T
* ?????μ?÷: ?T
********************************************************************************/
char * safe_strncpy(char *dst, const char *src, size_t size)
{
    dst[size-1] = '\0';
    return strncpy(dst, src, size-1);
}

/******************************************************************************
* oˉêy??3?￡oset_net_phyaddr
* 1??ü?èê?￡oéè????????×?í???μ???àíμ??·
* ê?è?2?êy￡ochar *name	í?????×?(eth0)
* ê?3?2?êy￡ochar *addr	í???μ???àíμ??·(12:13:14:15:16:17)
* ·μ ?? ?μ￡o3é1?￡o0￡?ê§°ü￡o-1
* DT??????: ?T
* ?????μ?÷: ?T
********************************************************************************/
int set_net_phyaddr(char *name,char *addr)  // name : eth0 eth1 lo and so on 
{											// addr : 12:13:14:15:16:17
	char cmd[128] = {0};
	sprintf(cmd, "%s", "ifconfig eth0 down");
    SystemCall_msg(cmd, SYSTEM_MSG_NOBLOCK_RUN);
    
	memset(cmd, 0, 128);
	sprintf(cmd, "%s""%s", "ifconfig eth0 hw ether ", addr);
    SystemCall_msg(cmd, SYSTEM_MSG_NOBLOCK_RUN);
    printf("#### %s %d, cmd:%s\n", __FUNCTION__, __LINE__, cmd);

    memset(cmd, 0, 128);
	sprintf(cmd, "%s", "ifconfig eth0 up");
    SystemCall_msg(cmd, SYSTEM_MSG_NOBLOCK_RUN);

#if 0
	const struct arg1opt *a1op;
	const struct options *op;
	int 	sockfd;		
	struct 	ifreq ifr;
	struct 	sockaddr sa;
	unsigned char mask;
	char 	*p = "hw";
	char 	host[128];

	
	/* Create a channel to the NET kernel. */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		err();
		return -1;
	}

	/* get interface name */
	safe_strncpy(ifr.ifr_name, name, IFNAMSIZ);
	mask = N_MASK;
	
	for (op = OptArray ; op->name ; op++) 
	{		
			if (strcmp("hw",op->name) == 0) 
			{
				if ((mask &= op->flags)) 
				{ 
				    goto SET_NET_PHYADDR_FOUND_ARG;
				}
			}
	}
	err();
	return -4;
		
SET_NET_PHYADDR_FOUND_ARG:		
	
	a1op = Arg1Opt + 6;
	
	safe_strncpy(host, addr, (sizeof host));
		
	if (in_ether(host, &sa)) 
	{
		err();
		return -2;
	}
	p = (char *) &sa;
		
	memcpy((((char *) (&ifr)) + a1op->ifr_offset), p, sizeof(struct sockaddr));
		
	if (ioctl(sockfd, a1op->selector, &ifr) < 0) 
	{
		err();
		return -3;
	}
#endif
	return  0;
}

/******************************************************************************
* oˉêy??3?￡oifconfig_up_down
* 1??ü?èê?￡o?ó???òD?????????×?μ?í???
* ê?è?2?êy￡ochar *name	í?????×?(eth0)
*			char *action ?ó???òD???down up
* ê?3?2?êy￡o?T
* ·μ ?? ?μ￡o3é1?￡o0￡?ê§°ü￡o-1
* DT??????: ?T
* ?????μ?÷: ?T
********************************************************************************/
int ifconfig_up_down(char *name, char *action) // name :  eth0 eth1 lo and so on 
{											   // action: down up	
	const struct options *op;
	int 	sockfd;		
	int 	selector;
	struct 	ifreq ifr;
	unsigned char mask;
	
	/* Create a channel to the NET kernel. */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		return -1;
	}

	/* get interface name */
	safe_strncpy(ifr.ifr_name, name, IFNAMSIZ);
	mask = N_MASK;
	
	for (op = OptArray ; op->name ; op++) 
	{		
			if (strcmp(action,op->name) == 0) 
			{
				if ((mask &= op->flags)) 
				{ 
				    goto IFCONFIG_UP_DOWN_FOUND_ARG;
				}
			}
	}
	return -4;
		
IFCONFIG_UP_DOWN_FOUND_ARG:		
	
	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) 
	{
		return -2;
	} 
	else 
	{
		selector = op->selector;
		if (mask & SET_MASK) 
		{
			ifr.ifr_flags |= selector;
		} 
		else 
		{
			ifr.ifr_flags &= ~selector;
		}
		if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0) 
		{
			return -3;
		}
	}						

	return  0;
}


int pid;
int sock_fd;
char send_packet[PACKET_SIZE];
char recv_packet[PACKET_SIZE];
struct sockaddr_in dest_addr;

/******************************************************************************
* oˉêy??3?￡ocheck_sum
* 1??ü?èê?￡ooíD￡?é
* ê?è?2?êy￡ounsigned short *buf	òa??DDD￡?éμ?êy?Y
*			int len òa??DDD￡?éμ?êy?Yμ?3€?è
* ê?3?2?êy￡o?T
* ·μ ?? ?μ￡ooíD￡?éμ??μ
* DT??????: ?T
* ?????μ?÷: ?T
********************************************************************************/
unsigned short check_sum(unsigned short *buf,int len)
{
	int left = len;
	int sum = 0;
	unsigned short *addr = buf;
	unsigned short answer = 0;
	while(left > 1)
	{
		sum += *addr++;
		left -= 2;
	}
	if(left == 1)
	{
		*(unsigned char *)(&answer) = *(unsigned char *)addr;
		sum += answer;
	}
	 
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	
	return answer;
}

/******************************************************************************
* oˉêy??3?￡osend_icmp_packet
* 1??ü?èê?￡o·￠?íicmpêy?Y°üμ???±êIPμ??·
* ê?è?2?êy￡ochar *addr	??±êIPμ??·
* ê?3?2?êy￡o?T
* ·μ ?? ?μ￡o3é1?￡o0￡?ê§°ü￡o-1
* DT??????: ?T
* ?????μ?÷: ?T
********************************************************************************/
int send_icmp_packet(char *addr)
{
	int pack_size;
	int ret;
	struct icmp *icmp;
	struct timeval *tv;
	
	pid = getpid();
	pack_size = 64;
	
	/* create a raw socket for icmp */
	if((sock_fd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP)) < 0)
		return -1; 
	
	/* set destination host information */
	bzero(&dest_addr,sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = inet_addr(addr);
	
	/* fill in icmp packet */
	icmp = (struct icmp *)send_packet;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_cksum = 0;
	icmp->icmp_seq = 0;
	icmp->icmp_id = pid;
	tv = (struct timeval *)icmp->icmp_data;
	gettimeofday(tv,NULL);
	icmp->icmp_cksum = check_sum((unsigned short *)icmp,pack_size);
	
	ret = sendto(sock_fd,send_packet,pack_size,0,(struct sockaddr *)&dest_addr,sizeof(dest_addr));

	return ret;
}

/******************************************************************************
* oˉêy??3?￡orecv_icmp_packet
* 1??ü?èê?￡o?óê?icmpêy?Y°ü
* ê?è?2?êy￡o?T
* ê?3?2?êy￡o?T
* ·μ ?? ?μ￡o3é1?￡o0￡?ê§°ü￡o-1
* DT??????: ?T
* ?????μ?÷: ?T
********************************************************************************/
int recv_icmp_packet()
{
	int ip_head_len;
	int recv_len;
	int ret;
	struct ip *ip;
	struct icmp *icmp;
	struct timeval tv;
	fd_set fdset;
	
	/* set timeout */
	tv.tv_sec = MAX_WAIT_TIME;
	tv.tv_usec = 0;
	
	/* set fd_set */
	FD_ZERO(&fdset);
	FD_SET(sock_fd,&fdset);
	
	if((ret = select(sock_fd+1,&fdset,NULL,NULL,&tv)) == -1)
	{
		close(sock_fd);
		printf("selec error !\n");
		return -1;
	}
	else if(ret == 0)
	{
		printf("receive timeout !\n");
		return -1;
	}
	else
	{
		ret = recvfrom(sock_fd, recv_packet,sizeof(recv_packet), 0, (struct sockaddr *)&dest_addr, (socklen_t *)&recv_len);

		ip = (struct ip *)recv_packet;
		ip_head_len = ip->ip_hl << 2;
		icmp = (struct icmp *)(recv_packet + ip_head_len);
		if(icmp->icmp_type == ICMP_ECHOREPLY && icmp->icmp_id == pid)
		{
			printf("receive ok !\n");
			close(sock_fd);
			return 0;
		} 
		else
		{
			close(sock_fd);
			return -1;
		}
	}
}



int get_brdcast_addr(char *name,char *net_brdaddr)
{
	struct ifreq ifr;
	int ret = 0;
	int fd;
    struct sockaddr_in stSockAddrIn;
	
	strcpy(ifr.ifr_name, name);
	ifr.ifr_addr.sa_family = AF_INET;
	
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	if (ioctl(fd, SIOCGIFBRDADDR, &ifr) < 0) 
	{
		ret = -1;
	}

    memset(&stSockAddrIn, 0, sizeof(stSockAddrIn));
    memcpy(&stSockAddrIn, &ifr.ifr_addr, sizeof(stSockAddrIn));
	strcpy(net_brdaddr,inet_ntoa(stSockAddrIn.sin_addr));

	close(fd);

	return	ret;
}



/******************************************************************************
********************************************************************************/
int get_gateway_addr(char *gateway_addr)
{
	char buff[256];
	int  nl = 0 ;
	struct in_addr gw;
	int flgs, ref, use, metric;
	unsigned long int d,g,m;
	unsigned long addr;
	unsigned long *pgw = &addr;

	FILE	 *fp = NULL;
	
	if (pgw == NULL)
		return -1;
		
	*pgw = 0;

	fp = fopen("/proc/net/route", "r");
	if (fp == NULL)
	{
		return -1;
	}
		
	nl = 0 ;
	while( fgets(buff, sizeof(buff), fp) != NULL ) 
	{
		if(nl) 
		{
			int ifl = 0;
			while(buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0')
				ifl++;
			buff[ifl]=0;    /* interface */
			if(sscanf(buff+ifl+1, "%lx%lx%X%d%d%d%lx",
				   &d, &g, &flgs, &ref, &use, &metric, &m)!=7) 
			{
				//continue;
				fclose(fp);
				return -2;
			}


			ifl = 0;        /* parse flags */
			if(flgs&RTF_UP) 
			{			
				gw.s_addr   = g;
					
				if(d==0)
				{
					*pgw = g;		
					strcpy(gateway_addr,inet_ntoa(gw));
					fclose(fp);
					return 0;
				}				
			}
		}
		nl++;
	}	
	
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	
	return	0;
}

/******************************************************************************
********************************************************************************/
int set_gateway_addr(char *gateway_addr)
{
    char old_gateway_addr[32];
    struct rtentry rt;
    unsigned long gw;
    int fd;
    int ret = 0;
    struct sockaddr_in stSockAddrIn;

    get_gateway_addr(old_gateway_addr);
    del_gateway_addr(old_gateway_addr);
	
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		ret = -1;
		return ret;
	}	

    gw = inet_addr(gateway_addr);
    memset((char *) &rt, 0, sizeof(struct rtentry));
    memset(&stSockAddrIn, 0, sizeof(stSockAddrIn));

    rt.rt_flags = RTF_UP | RTF_GATEWAY ;
    //rt.rt_flags = 0x03;
    stSockAddrIn.sin_addr.s_addr = 0;
    stSockAddrIn.sin_family = PF_INET;
    memcpy(&rt.rt_dst, &stSockAddrIn,sizeof(rt.rt_dst));
    //((struct sockaddr_in *)&(rt.rt_dst))->sin_addr.s_addr = 0;
    //((struct sockaddr_in *)&(rt.rt_dst))->sin_family = PF_INET;

    stSockAddrIn.sin_addr.s_addr = gw;
    stSockAddrIn.sin_family = PF_INET;
    memcpy(&rt.rt_gateway, &stSockAddrIn,sizeof(rt.rt_gateway));
    //((struct sockaddr_in *)&(rt.rt_gateway))->sin_addr.s_addr = gw;	
    //((struct sockaddr_in *)&(rt.rt_gateway))->sin_family = PF_INET;

    stSockAddrIn.sin_addr.s_addr = 0;
    stSockAddrIn.sin_family = PF_INET;
    memcpy(&rt.rt_genmask, &stSockAddrIn,sizeof(rt.rt_genmask));
    //((struct sockaddr_in *)&(rt.rt_genmask))->sin_addr.s_addr = 0;
    //((struct sockaddr_in *)&(rt.rt_genmask))->sin_family = PF_INET;
    //rt.rt_dev = NULL;
    rt.rt_dev = QMAPI_ETH_DEV;
		
	if (ioctl(fd, SIOCADDRT, &rt) < 0) 	
	{	
		printf("NetServer Set GateWay Addr(%d) failed:%s \n",__LINE__, strerror(errno));
		ret = -1;
	}
	close(fd);
	
	return	ret;
}

int set_gateway_addr_ext(char *gateway_addr)
{
	//char old_gateway_addr[32];
	struct rtentry rt;
	unsigned long gw;
	int fd;
	int ret = 0;
    struct sockaddr_in stSockAddrIn;
	
	//get_gateway_addr(old_gateway_addr);
	//del_gateway_addr(old_gateway_addr);
	
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		ret = -1;
                return ret;
	}	

	gw = inet_addr(gateway_addr);
	memset((char *) &rt, 0, sizeof(struct rtentry));
    memset(&stSockAddrIn, 0, sizeof(stSockAddrIn));
	
	rt.rt_flags = RTF_UP | RTF_GATEWAY ;
	//rt.rt_flags = 0x03;

    stSockAddrIn.sin_addr.s_addr = 0;
    stSockAddrIn.sin_family = PF_INET;
    memcpy(&rt.rt_dst, &stSockAddrIn,sizeof(rt.rt_dst));
    //((struct sockaddr_in *)&(rt.rt_dst))->sin_addr.s_addr = 0;
	//((struct sockaddr_in *)&(rt.rt_dst))->sin_family = PF_INET;

    stSockAddrIn.sin_addr.s_addr = gw;
    stSockAddrIn.sin_family = PF_INET;
    memcpy(&rt.rt_gateway, &stSockAddrIn,sizeof(rt.rt_gateway));
	//((struct sockaddr_in *)&(rt.rt_gateway))->sin_addr.s_addr = gw;	
	//((struct sockaddr_in *)&(rt.rt_gateway))->sin_family = PF_INET;

    stSockAddrIn.sin_addr.s_addr = 0;
    stSockAddrIn.sin_family = PF_INET;
    memcpy(&rt.rt_genmask, &stSockAddrIn,sizeof(rt.rt_genmask));
	//((struct sockaddr_in *)&(rt.rt_genmask))->sin_addr.s_addr = 0;
	//((struct sockaddr_in *)&(rt.rt_genmask))->sin_family = PF_INET;
	
	rt.rt_dev = NULL;
		
	if (ioctl(fd, SIOCADDRT, &rt) < 0) 	
	{	
	    printf("set_gateway_addr_ext failed %s \n",strerror(errno));
		ret = -1;
	}
	close(fd);
	
	return	ret;
}

/******************************************************************************
********************************************************************************/
int del_gateway_addr(char *gateway_addr)
{
	struct rtentry rt;
	unsigned long gw;
	int ret = 0;
	int fd;
    struct sockaddr_in stSockAddrIn;
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
	
	gw = inet_addr(gateway_addr);
	memset((char *) &rt, 0, sizeof(struct rtentry));
    memset(&stSockAddrIn, 0, sizeof(stSockAddrIn));

	rt.rt_flags = RTF_UP | RTF_GATEWAY ;

    stSockAddrIn.sin_family = PF_INET;
    memcpy(&rt.rt_dst, &stSockAddrIn,sizeof(rt.rt_dst));
	//((struct sockaddr_in *)&(rt.rt_dst))->sin_family = PF_INET;

    stSockAddrIn.sin_addr.s_addr = gw;
    stSockAddrIn.sin_family = PF_INET;
    memcpy(&rt.rt_gateway, &stSockAddrIn,sizeof(rt.rt_gateway));
	//((struct sockaddr_in *)&(rt.rt_gateway))->sin_addr.s_addr = gw;	
	//((struct sockaddr_in *)&(rt.rt_gateway))->sin_family = PF_INET;

    stSockAddrIn.sin_addr.s_addr = 0;
    stSockAddrIn.sin_family = PF_INET;
    memcpy(&rt.rt_genmask, &stSockAddrIn,sizeof(rt.rt_genmask));
	//((struct sockaddr_in *)&(rt.rt_genmask))->sin_addr.s_addr = 0;
	//((struct sockaddr_in *)&(rt.rt_genmask))->sin_family = PF_INET;
	
	rt.rt_dev = NULL;
		
	if (ioctl(fd, SIOCDELRT, &rt) < 0) 	
	{
		ret = -1;
	}
	
	close(fd);

	return	ret;
}



int get_net_phyaddr(char *name, char *addr , char *bMac)  // name : eth0 eth1 lo and so on 
{											// addr : 12:13:14:15:16:17
	int 		sockfd;		
	struct 	ifreq ifr;
	char 	buff[24];
	
	/* Create a channel to the NET kernel. */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{		
		return -1;
	}

	/* get net physical address */
	strcpy(ifr.ifr_name, name);
	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0)
	{	
		close(sockfd);
		return -1;
	}
	strcpy(ifr.ifr_name, name);
	if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) < 0)
	{
		close(sockfd);
		return -1;	
	}
	else
	{
		memcpy(buff, ifr.ifr_hwaddr.sa_data, 8);
		memcpy(bMac , ifr.ifr_hwaddr.sa_data , 8);
		snprintf(addr, sizeof(buff), "%02X:%02X:%02X:%02X:%02X:%02X",
			 (buff[0] & 0377), (buff[1] & 0377), (buff[2] & 0377),
			 (buff[3] & 0377), (buff[4] & 0377), (buff[5] & 0377));
		
		//printf("HW address: %s\n", addr);
		
		close(sockfd);
		return  0;
	}
}



int refresh_gateway_if(char * netif,char *newif)
{
	char buff[256];
	int  nl = 0 ;
	struct in_addr gw;
	int flgs, ref, use, metric;
	unsigned long int d,g,m;
	unsigned long addr;
	unsigned long *pgw = &addr;

	FILE	 *fp = NULL;
	
	if (pgw == NULL)
		return -1;
		
	*pgw = 0;

	fp = fopen("/proc/net/route", "r");
	
	if (fp == NULL)
	{
		return -1;
	}
		
	nl = 0 ;
	while( fgets(buff, sizeof(buff), fp) != NULL ) 
	{
		if(nl) 
		{
		    
			int ifl = 0;
			while(buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0')
				ifl++;
			buff[ifl]=0;	/* interface */
			if(sscanf(buff+ifl+1, "%lx%lx%X%d%d%d%lx",
				   &d, &g, &flgs, &ref, &use, &metric, &m)!=7) 
			{
				//continue;
				fclose(fp);
				return -2;
			}
			
            if(strstr(buff,netif) != NULL)
            {
                char strDest[64] = {0};
				char strMask[64] = {0};
				gw.s_addr   = d;
				strcpy(strDest,inet_ntoa(gw));
				gw.s_addr   = m;
				strcpy(strMask,inet_ntoa(gw));
				char strCmd[128] = {0};
			    sprintf(strCmd,"route del -net %s netmask %s %s",strDest,strMask,newif);	
//				system(strCmd);
                SystemCall_msg(strCmd,SYSTEM_MSG_BLOCK_RUN);

                memset(strCmd,0,sizeof(strCmd));
				sprintf(strCmd,"route add -net %s netmask %s %s",strDest,strMask,newif);	
//				system(strCmd);
                SystemCall_msg(strCmd,SYSTEM_MSG_BLOCK_RUN);

				if(fp)
            	{
	             	fclose(fp);
		             fp = NULL;
	            }
	
				return 0;
            }
			ifl = 0;		/* parse flags */
			
		}
		nl++;
	}	
	
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	
	return	0;
}



char* nslookup_raw(char* host_name)
{
 
	struct sockaddr_in sa = {0};
	struct hostent* phe = gethostbyname2(host_name,AF_INET);
	if(!phe)
	{
		
		return 0;
	}
	int i = 0;
	for( i=0;i<4;i++) // Copy resolved address
		((unsigned char*)&sa.sin_addr.s_addr)[i] = phe->h_addr_list[0][i];
	return inet_ntoa(sa.sin_addr);
}

int set_ip_addr(char *name,char *net_ip) /* name : eth0 eth1 lo and so on */
{	
	//struct sockaddr	addr;
	struct ifreq ifr;
	char gateway_addr[32];
	int ret = 0;
	int fd;
    struct sockaddr_in stSockAddrIn;

    memset(&stSockAddrIn, 0, sizeof(stSockAddrIn));
    stSockAddrIn.sin_family = PF_INET;
    stSockAddrIn.sin_addr.s_addr = inet_addr(net_ip);
	//((struct sockaddr_in *)&(addr))->sin_family = PF_INET;
	//((struct sockaddr_in *)&(addr))->sin_addr.s_addr = inet_addr(net_ip);

    memcpy(&ifr.ifr_addr,&stSockAddrIn,sizeof(ifr.ifr_addr));
	//ifr.ifr_addr = addr; 
	strcpy(ifr.ifr_name,name);

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	get_gateway_addr(gateway_addr); 

	if (ioctl(fd, SIOCSIFADDR, &ifr) != 0) 
	{
		printf("%s(%d) failed:%s \n",__FUNCTION__, __LINE__, strerror(errno));

		ret = -1;
	}

	close(fd);

	set_gateway_addr(gateway_addr); 

	return	ret;
}

int set_mask_addr(char *name,char *mask_ip) /* name : eth0 eth1 lo and so on */
{	
	//struct sockaddr	addr;
	struct ifreq ifr;
	char gateway_addr[32];
	int ret = 0;
	int fd;
    struct sockaddr_in stSockAddrIn;

    memset(&stSockAddrIn, 0, sizeof(stSockAddrIn));
    stSockAddrIn.sin_family = PF_INET;
    stSockAddrIn.sin_addr.s_addr = inet_addr(mask_ip);
	//((struct sockaddr_in *)&(addr))->sin_family = PF_INET;
	//((struct sockaddr_in *)&(addr))->sin_addr.s_addr = inet_addr(mask_ip);

    memcpy(&ifr.ifr_addr,&stSockAddrIn,sizeof(ifr.ifr_addr));
	//ifr.ifr_addr = addr;
	strcpy(ifr.ifr_name,name);

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	get_gateway_addr(gateway_addr);

	if (ioctl(fd, SIOCSIFNETMASK, &ifr) != 0) 
	{
		printf("%s(%d) failed:%s \n",__FUNCTION__, __LINE__, strerror(errno));

		ret = -1;
	}

	close(fd);

	set_gateway_addr(gateway_addr);

	return	ret;
}

int del_gateway_if(char * netif)
{
    char buff[256];
	int  nl = 0 ;
	struct in_addr gw;
	int flgs, ref, use, metric;
	unsigned long int d,g,m;
	unsigned long addr;
	unsigned long *pgw = &addr;

	FILE	 *fp = NULL;
	
	if (pgw == NULL)
		return -1;
		
	*pgw = 0;

	fp = fopen("/proc/net/route", "r");
	
	if (fp == NULL)
	{
		return -1;
	}
		
	nl = 0 ;
	while( fgets(buff, sizeof(buff), fp) != NULL ) 
	{
		if(nl) 
		{
		    
			int ifl = 0;
			while(buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0')
				ifl++;
			buff[ifl]=0;	/* interface */
			if(sscanf(buff+ifl+1, "%lx%lx%X%d%d%d%lx",
				   &d, &g, &flgs, &ref, &use, &metric, &m)!=7) 
			{
				//continue;
				fclose(fp);
				return -2;
			}
			
			if(strstr(buff,netif) != NULL)
			{
				char strDest[64] = {0};
				char strMask[64] = {0};
				gw.s_addr   = d;
				strcpy(strDest,inet_ntoa(gw));
				gw.s_addr   = m;
				strcpy(strMask,inet_ntoa(gw));
				char strCmd[128] = {0};
				sprintf(strCmd,"route del -net %s netmask %s %s",strDest,strMask,netif);	
//				system(strCmd);					
                SystemCall_msg(strCmd,SYSTEM_MSG_BLOCK_RUN);

			}
			ifl = 0;		/* parse flags */
			
		}
		nl++;
	}	
	
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	
	return	0;
   
}

/**
 * @brief	get mac address of an interface
 * @param	"char *ifname" : [IN]interface name
 * @param	"unsigned char *mac" : [OUT]mac address
 * @retval	0 : success ; -1 : fail
 */
int net_get_hwaddr(char *ifname, unsigned char *mac)
{
    int ret;
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		printf("net_get_hwaddr: socket error\n");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ret = ioctl(skfd, SIOCGIFHWADDR, &ifr);
	if (ret < 0) {
		printf("net_get_hwaddr: ioctl SIOCGIFHWADDR error,ret:%d,errno:%d! ifname:%s,\n",ret, errno, ifname);
		close(skfd);
		return -1;
	}
	close(skfd);

	memcpy(mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, IFHWADDRLEN);
	return 0;
}


int get_ip_addr(char *name,char *net_ip)
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

int get_mask_addr(char *name,char *net_mask)
{
	struct ifreq ifr;
	int ret = 0;
	int fd;
    struct sockaddr_in stSockAddrIn;
	
	strcpy(ifr.ifr_name, name);
	ifr.ifr_addr.sa_family = AF_INET;
	
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	if (ioctl(fd, SIOCGIFNETMASK, &ifr) < 0) 
	{
		ret = -1;
	}

    memset(&stSockAddrIn, 0, sizeof(stSockAddrIn));
    memcpy(&stSockAddrIn, &ifr.ifr_addr, sizeof(stSockAddrIn));
	strcpy(net_mask,inet_ntoa(stSockAddrIn.sin_addr));	
	//strcpy(net_mask,inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));

	close(fd);

	return	ret;
}

int set_dns_addr(char *address)
{
	FILE *fp = NULL;
	char buffer[16];
	if (address != NULL)
	{
		memcpy(buffer, address, sizeof(buffer));
	}
	else
	{
		return -1;
	}

	fp = fopen("/etc/resolv.conf", "w+");
	if (fp == NULL)
	{
		return -1;
	}		
	fprintf(fp, "nameserver %s", buffer);
	fclose(fp);	
	
	return 0;		
}


int get_dns_addr_wlan(char *address)
{
	FILE *fp = NULL;
	char	cmpStr[16];
	char	cmpStr2[16];
	char csBuf[256] = {'\0'};

	//system("cat /etc/resolv.conf");

	fp = fopen("/tmp/dns.wlan0", "r");
	if (fp == NULL)
	{
		return -1;
	}

	strcpy(cmpStr, "nameserver");

	while(fgets(csBuf, 256, fp) != NULL)
	{
		memcpy(cmpStr2, csBuf,strlen(cmpStr));
		if (strcmp(cmpStr, cmpStr2) != 0)
		{
			continue;
		}
		else
			break;
	}

	memcpy(address, csBuf + 11, 16);
	address[15] = 0;
	fclose(fp);	

	return 0;
}


int get_dns_addr_ext_wlan(char *dns1, char *dns2)
{
	FILE *fp = NULL;
	char csBuf[64] = {0};

	if(!dns1 || !dns2)
		return -1;

	fp = fopen("/tmp/dns.wlan0", "r");
	if (fp == NULL)
	{
		dns1[0] = 0;
		dns2[0] = 0;
		return -1;
	}

	if(fgets(csBuf, 64, fp))
	{
		sscanf(csBuf, "nameserver %s", dns1);

		memset(csBuf, 0, 64);
		if(fgets(csBuf, 64, fp))
		{
			sscanf(csBuf, "nameserver %s", dns2);
		}
		else
		{
			dns2[0] = 0;
		}
	}
	else
	{
		fclose(fp);	
		dns1[0] = 0;
		dns2[0] = 0;
		return -1;
	}

	fclose(fp);	

	return 0;
}


int get_dns_addr(char *address)
{
	FILE *fp = NULL;
	char	cmpStr[16];
	char	cmpStr2[16];
	char csBuf[256] = {'\0'};

	//system("cat /etc/resolv.conf");

	fp = fopen("/etc/resolv.conf", "r");
	if (fp == NULL)
	{
		return -1;
	}

	strcpy(cmpStr, "nameserver");

	while(fgets(csBuf, 256, fp) != NULL)
	{
		memcpy(cmpStr2, csBuf,strlen(cmpStr));
		if (strcmp(cmpStr, cmpStr2) != 0)
		{
			continue;
		}
		else
			break;
	}

	memcpy(address, csBuf + 11, 16);
	address[15] = 0;
	fclose(fp);	

	return 0;
}

int get_dns_addr_ext(char *dns1, char *dns2)
{
	FILE *fp = NULL;
	char csBuf[64] = {0};

	if(!dns1 || !dns2)
		return -1;

	fp = fopen("/etc/resolv.conf", "r");
	if (fp == NULL)
	{
		dns1[0] = 0;
		dns2[0] = 0;
		return -1;
	}

	if(fgets(csBuf, 64, fp))
	{
		sscanf(csBuf, "nameserver %s", dns1);

		memset(csBuf, 0, 64);
		if(fgets(csBuf, 64, fp))
		{
			sscanf(csBuf, "nameserver %s", dns2);
		}
		else
		{
			dns2[0] = 0;
		}
	}
	else
	{
		fclose(fp);	
		dns1[0] = 0;
		dns2[0] = 0;
		return -1;
	}

	fclose(fp);	

	return 0;
}


int set_gateway_netif(char *gateway,char *netif)
{
	struct rtentry rt;
	unsigned long gw;
	int fd;
	int ret = 0;
    struct sockaddr_in stSockAddrIn;
	
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		ret = -1;
	}	

	gw = inet_addr(gateway);
	memset((char *) &rt, 0, sizeof(struct rtentry));
    memset(&stSockAddrIn, 0, sizeof(stSockAddrIn));

    rt.rt_flags = RTF_UP | RTF_GATEWAY ;
	//rt.rt_flags = 0x03;
	stSockAddrIn.sin_addr.s_addr = 0;
    stSockAddrIn.sin_family = PF_INET;
    memcpy(&rt.rt_dst, &stSockAddrIn,sizeof(rt.rt_dst));
	//((struct sockaddr_in *)&(rt.rt_dst))->sin_addr.s_addr = 0;
	//((struct sockaddr_in *)&(rt.rt_dst))->sin_family = PF_INET;

    stSockAddrIn.sin_addr.s_addr = gw;
    stSockAddrIn.sin_family = PF_INET;
    memcpy(&rt.rt_gateway, &stSockAddrIn,sizeof(rt.rt_gateway));
	//((struct sockaddr_in *)&(rt.rt_gateway))->sin_addr.s_addr = gw; 
	//((struct sockaddr_in *)&(rt.rt_gateway))->sin_family = PF_INET;

    stSockAddrIn.sin_addr.s_addr = 0;
    stSockAddrIn.sin_family = PF_INET;
    memcpy(&rt.rt_genmask, &stSockAddrIn,sizeof(rt.rt_genmask));
	//((struct sockaddr_in *)&(rt.rt_genmask))->sin_addr.s_addr = 0;
	//((struct sockaddr_in *)&(rt.rt_genmask))->sin_family = PF_INET;
	
	rt.rt_dev = netif;
		
	if (ioctl(fd, SIOCADDRT, &rt) < 0)	
	{	
		printf("set_gateway_addr_ext failed %s \n",strerror(errno));
		ret = -1;
	}
	close(fd);
	
	return	ret;

}


int set_gateway_host_netif(char *gateway,  char *netif)
{
	int ret = 0;

	del_gateway_netif(netif);
	
	char bufCmd[128] = {0};
	sprintf(bufCmd ,"route add -host %s dev %s", gateway, netif);
//	ret = system(bufCmd);
    ret =  SystemCall_msg(bufCmd,SYSTEM_MSG_BLOCK_RUN);
	if(ret != 0)
	{
	    return -1;
	}
    memset(bufCmd,0,sizeof(bufCmd));
	sprintf(bufCmd ,"route add default gw %s dev %s", gateway, netif);
//	ret = system(bufCmd);
    ret =  SystemCall_msg(bufCmd,SYSTEM_MSG_BLOCK_RUN);
	if(ret != 0)
	{
	    return -1;
	}
	return	0;
}


//cw5s
int del_gateway_netif(char *netif)
{
	char bufCmd[128] = {0};
	sprintf(bufCmd,"route del default dev %s",netif);
//	system(bufCmd);
     SystemCall_msg(bufCmd,SYSTEM_MSG_BLOCK_RUN);

	return 0;
}

int set_route_netif( unsigned int dwAddr, unsigned int dwMask, char *netif)
{
	struct in_addr	NetIpAddr , NetMask ;
	char		NetAddr[24];
	char        MaskAddr[24];
	/// Set GateWay IP
	memset(NetAddr , 0 , 24);
	NetIpAddr.s_addr = dwAddr&dwMask;
	strcpy(NetAddr , inet_ntoa(NetIpAddr));
	NetMask.s_addr = dwMask;
	strcpy(MaskAddr , inet_ntoa(NetMask));

	char bufCmd[128] = {0};
	sprintf(bufCmd,"route add -net %s netmask %s dev %s", NetAddr, MaskAddr, netif);
//	system(bufCmd);
    SystemCall_msg(bufCmd,SYSTEM_MSG_BLOCK_RUN);
	printf("%s\n", bufCmd);
	return 0;
}
int del_route_netif(unsigned int dwAddr, unsigned int dwMask, char *netif)
{
	struct in_addr	NetIpAddr , NetMask ;
	char		NetAddr[24];
	char        MaskAddr[24];
	/// Set GateWay IP
	memset(NetAddr , 0 , 24);
	NetIpAddr.s_addr = dwAddr&dwMask;
	strcpy(NetAddr , inet_ntoa(NetIpAddr));
	NetMask.s_addr = dwMask;
	strcpy(MaskAddr , inet_ntoa(NetMask));

	char bufCmd[128] = {0};
	sprintf(bufCmd,"route del -net %s netmask %s dev %s", NetAddr, MaskAddr, netif);
//	system(bufCmd);
    SystemCall_msg(bufCmd,SYSTEM_MSG_BLOCK_RUN);

	printf("%s\n", bufCmd);
	return	0;
}

int get_gateway_netif(char * netif)
{
	char buff[256];
	int  nl = 0 ;
	FILE	 *fp = NULL;
	fp = fopen("/proc/net/route", "r");
	if (fp == NULL)
	{
		return -1;
	}

	int flgs, ref, use, metric;
	unsigned long int d,g,m;

	while( fgets(buff, sizeof(buff), fp) != NULL )
	{
		if(nl)
		{

			int ifl = 0;
			while(buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0')
				ifl++;
			buff[ifl]=0;	/* interface */
			if(sscanf(buff+ifl+1, "%lx%lx%X%d%d%d%lx",
					  &d, &g, &flgs, &ref, &use, &metric, &m)!=7)
			{
				//continue;
				fclose(fp);
				return -2;
			}
			if(d == 0)
			{
				strcpy(netif,buff);
				fclose(fp);
				return 0;
			}

		}
		nl++;
	}

	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}

	return	-1;
}

int SetNetWorkGateWay(unsigned int dwGateway, unsigned int dwAddr, unsigned int dwMask, char *netif)
{
	struct in_addr	Gateway;
	char		NetAddr[24];
	/// Set GateWay IP
	memset(NetAddr , 0 , 24);
	Gateway.s_addr = dwGateway;
	strcpy(NetAddr , inet_ntoa(Gateway));
    
	if(NetAddr[0] == 0 )
	{
		printf("Gate address is NULL !\n");
	}
	else
	{
		if( (dwGateway & dwMask) == (dwAddr & dwMask) )
		{
			if(set_gateway_netif(NetAddr, netif)<0)
			{
				printf("set gateway to %s failed \n",NetAddr);
			}
			
		}
		else
		{
			if(set_gateway_host_netif(NetAddr, netif)<0)
			{	
				printf("set gateway to %s failed \n",NetAddr);
			}
		}
	}
	return 0;
}



// if_name like "ath0", "eth0". Notice: call this function
// need root privilege.
// return value:
// -1 -- error , details can check errno
// 1 -- interface link up
// 0 -- interface link down.
#define ETHTOOL_GLINK       0x0000000a /* Get link status (ethtool_value) */
struct ethtool_value {

            unsigned int   cmd;

                unsigned int   data; 

}; 

int get_netlink_status(const char *netif)
{
    int skfd;
    struct ifreq ifr;
    struct ethtool_value edata;

    edata.cmd = ETHTOOL_GLINK;
    edata.data = 0;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, netif, sizeof(ifr.ifr_name) - 1);
    ifr.ifr_data = (char *) &edata;

    if (( skfd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0)
        return -1;

    if(ioctl( skfd, SIOCETHTOOL, &ifr ) == -1)
    {
        close(skfd);
        return -1;
    }

    close(skfd);
    return edata.data;
}

int GetIPGateWay(const char *netif, char *gateway)
{
	char filename[64] = {0};
    int fd_gw = -1;
    char gwStr[16] = {0};

	sprintf(filename, "/tmp/gw.%s", netif);
	strcpy(gateway, "0.0.0.0");

    fd_gw = open(filename, O_RDONLY);
    if(fd_gw < 0)
        return -1;

    read(fd_gw, gwStr, sizeof(gwStr));
    close(fd_gw);

    if(strlen(gwStr)<=7)
        return -1;

	strcpy(gateway, gwStr);

    return 0;
}


//����pIpΪxxx.xxx.xxx.xxx��ʽ,ʧ�ܷ���0
unsigned long change_ip_string_value(const char *pIp)
{
    unsigned long value = 0;
    int ret = 0;
    unsigned long uiV1 = 0,uiV2 = 0,uiV3 = 0,uiV4 = 0;

    if(NULL == pIp)
    {
        return value;
    }
    ret = sscanf(pIp,"%ld.%ld.%ld.%ld",&uiV1,&uiV2,&uiV3,&uiV4);
    if(ret == 4)
    {
        value = (uiV4 << 24) | (uiV3 << 16) | (uiV2 << 8) | (uiV1 << 0);
    }
    return value;
}

