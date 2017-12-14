#ifndef HB_SEARCH_H
#define HB_SEARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "GXThread.h"

#define HB_SEARCH_MCAST_PORT		27156
#define HBSEARCHHEADID				"HBneTautOf8inDp_@"		
#define BUFFER_LEN					256
//#define CMD_TYPE_9C	
#define HB_SERVICE_PORT				16923

#pragma pack (1)

typedef struct tagHBSearchHeadCmd{
	char			nfhead[18];
	char			nPcIdNum[6];
	unsigned char	cmd;
}HBSEARCHHEADCMD;

typedef struct tagHBDeviceBaseInfo{
	HBSEARCHHEADCMD	stHead;			/* head cmd */

	char			mac_addr[6];
	unsigned int	ip_addr;
	int				dev_type;				//4byte
	unsigned short	port;			//2byte  little
	char			host_id[32];
	char			host_type[16];

#ifdef CMD_TYPE_9C
	char	protocol_type;
	char	soft_version[64];
	char	dev_serial[16];

	char	dns[4];
	char	gateway[4];
	char	mask_addr[4];

	char	wan_onoff;
	char	dhcp_onoff;
	char	ydt_support;
	char	ydt_onoff;

	char	reserve[4];
#endif
}HBDEVICEBASEINFO;

typedef struct tagHBSearchSession{
	int			serverFD;
	int			runflag;				/* ctrl thread loop flag*/
	pthread_t	threadId;
}HBSearchSession;

int HBSearchCreateSocket(int *pSockType);
int HBSearchRequestProc(int dmsHandle, int socketfd, int );

#pragma pack ()

#ifdef __cplusplus
}
#endif

#endif
