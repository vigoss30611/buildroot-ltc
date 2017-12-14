#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <linux/if_packet.h>
#include <netinet/if_ether.h>

//#include "openssl/evp.h"
//#include "openssl/hmac.h"
//#include "openssl/err.h"

//#include "net_config.h"
//#include "ptz_control.h"

#include "cgiinterface.h"
#include "iconv.h"
#include "soapH.h"
#include "QMAPIType.h"
#include "QMAPICommon.h"
#include "onvif.h"

#if 0
#define ONVIF_DBP(fmt, args...) do { \
	time_t ttt = time(NULL);		\
	struct tm *ptm = localtime(&ttt);	\
	fprintf(stderr, "\n=========time %04d-%02d-%02dT%02d:%02d:%02dZ [%s ] %s line %d " fmt "\n", \
	ptm->tm_year+1900, ptm->tm_mon +1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, \
	__FILE__, __FUNCTION__, __LINE__, ##args ); \
	fflush(stderr);  \
} while ( 0 )
#else
#define ONVIF_DBP(fmt, args...) 
#endif

//#include "sysconf.h"
//#include <sys_env_type.h>
//#include <sysctrl.h>
//#include <system_default.h>
//#include <ipnc_ver.h>
//#include <file_msg_drv.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <asm/types.h>
#include <asm/byteorder.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <asm/types.h>
//#include "mtd-user.h"

//#include <linux/jffs2.h>
#include <pthread.h>
#include <sys/sem.h>

#include <dirent.h>//目录操作

//int g_sock = 0;

#include "factandverinfo.h"
#include "trace.h"
#include "onvif.nsmap"

#include "sys_fun_interface.h" /* RM#2291: used sys_fun_gethostbyname, henry.li    2017/03/01*/

//#define PROFILE_EXTERN

static const float EPSINON = 0.000001;

typedef union __NET_IPV4 {
	__u32	int32;
	__u8	str[4];
} NET_IPV4;

//#define FALSE 0
//#define TRUE !FALSE

//int tempdnssearchdomainsize=0;

#define YEAR_IN_SEC (31556926)
#define MONTH_IN_SEC (2629744)
#define DAY_IN_SEC (86400)
#define HOURS_IN_SEC (3600)
#define MIN_IN_SEC (60)
#define MAX_EVENTS (3)
#define KEY_NOTIFY_SEM (1729)
#define MOTION_DETECTED_MASK (1) 	
#define NO_MOTION_MASK (2)	


#define  INFO_LENGTH 128
#define MID_INFO_LENGTH 64
#define SMALL_INFO_LENGTH 32

#define ETH_NAME "eth0"
#define WIFI_NAME "wlan0"

//static char *searchdomain = NULL;
//static char *dns_addr[2] = {NULL, NULL};
//static char *ntp_addr = NULL;
//static char *dns_name = NULL;

#define DEFAULT_SCOPE "onvif://www.onvif.org"
#define DEFAULT_ONVIF_SCOPES "onvif://www.onvif.org/type/Network_Video_Transmitter onvif://www.onvif.org/type/video_encoder onvif://www.onvif.org/type/audio_encoder onvif://www.onvif.org/type/ptz onvif://www.onvif.org/name/IPNC368 onvif://www.onvif.org/location/country/china onvif://www.onvif.org/hardware/00E00ADD0000"

#define ONVIF_EVENT_MOTION "tns1:RuleEngine/CellMotionDetector/Motion"
#define ONVIF_EVENT_IOALARM "tns1:Device/Trigger/DigitalInput"

#define ACOUNT_NUM				16 		///< How many acounts which are stored in system.
#define USER_LEN				32 		///< Maximum of acount username length.
#define PASSWORD_LEN			16 		///< Maximum of acount password length.

#define LARGE_INFO_LENGTH 1024

#define MAX_FILE_NAME			128		///< Maximum length of file name.
#define MAX_STREAM_NUM 2
#define GS_DEFAULT_GATEWAY	"192.168.1.1"

#define RTP_MULTICAST_IP     	"239.1.2.3"
#define RTP_MULTICAST_VPORT     4321
#define RTP_MULTICAST_APORT     4323
#define AUDIO_M_PORT(_c, _s)	(RTP_MULTICAST_APORT+(_c)*MAX_STREAM_NUM+(_s)*4)
#define VIDEO_M_PORT(_c, _s)	(RTP_MULTICAST_VPORT+(_c)*MAX_STREAM_NUM+(_s)*4)

//static int onvif_port = 80;

#include "uuid/uuid.h"
//static char uuid[64] = {0};

static int get_netcfg_fromsock(struct soap *soap, QMAPI_NET_NETWORK_CFG *network_cfg, QMAPI_NET_WIFI_CONFIG *wifi_cfg, int *isWIFI)
{
	if (!soap->cgihc)
	{
		return -1;
	}
	
	httpd_conn *hc = (httpd_conn *)soap->cgihc;

	char ip_buf[128];
	struct sockaddr_in me;	
	socklen_t len = sizeof(me); 

	//memset(ip_buf, 0, 128);
	ip_buf[0] = '\0';
	getsockname(hc->conn_fd, (struct sockaddr *)&me, &len);
	inet_ntop(AF_INET, &me.sin_addr, ip_buf, 128);   
	ONVIF_DBP("local address:%s\n", ip_buf);

	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, 0, network_cfg, sizeof(QMAPI_NET_NETWORK_CFG));
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_WIFICFG, 0, wifi_cfg, sizeof(QMAPI_NET_WIFI_CONFIG));
#ifdef _SUPPORT_ETH_DEV_
	*isWIFI = TRUE;
#else
	if(0 == strcmp(ip_buf, wifi_cfg->dwNetIpAddr.csIpV4))
	{
		*isWIFI = TRUE;
	}
	else
	{
		*isWIFI = FALSE;
	}
#endif
	return 0;
}

static void get_addr(struct soap *soap, char *ipaddr, int buflen)
{
	if (!soap->cgihc)
	{
		return ;
	}
	
	struct sockaddr_in me;
	socklen_t len = sizeof(me);

	httpd_conn *hc = (httpd_conn *)soap->cgihc;
	
	ipaddr[0] = '\0';
	getsockname(hc->conn_fd, (struct sockaddr *)&me, &len);
	inet_ntop(AF_INET, &me.sin_addr, ipaddr, buflen);   		
}

static int net_get_hwaddr(char *ifname, char *mac)
{
	struct ifreq ifr;
	int skfd;

	if ( (skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		printf("%s  %d, socket error\n",__FUNCTION__, __LINE__);
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		printf("%s  %d, ioctl SIOCGIFHWADDR error!\n",__FUNCTION__, __LINE__);
		close(skfd);
		return -1;
	}
	close(skfd);

	memcpy(mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, IFHWADDRLEN);
	return 0;
}

static int get_ip_addr(char *name,char *net_ip)
{
	struct ifreq ifr;
	int ret = 0;
	int fd;	

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
	
	strcpy(net_ip,inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));

	close(fd);

	return	ret;
}

#if 0
static int netsplit( char *pAddress, void *ip )
{
	unsigned int ret;
	NET_IPV4 *ipaddr = (NET_IPV4 *)ip;

	if ((ret = atoi(pAddress + 9)) > 255)
		return FALSE;
	ipaddr->str[3] = ret;

	*( pAddress + 9 ) = '\x0';
	if ((ret = atoi(pAddress + 6)) > 255)
		return FALSE;
	ipaddr->str[2] = ret;

	*( pAddress + 6 ) = '\x0';
	if ((ret = atoi(pAddress + 3)) > 255)
		return FALSE;
	ipaddr->str[1] = ret;

	*( pAddress + 3 ) = '\x0';
	if ((ret = atoi(pAddress + 0)) > 255)
		return FALSE;
	ipaddr->str[0] = ret;

	return TRUE;
}


int ipv4_str_to_num(char *data, struct in_addr *ipaddr)
{
	if ( strchr(data, '.') == NULL )
		return netsplit(data, ipaddr);
	return inet_aton(data, ipaddr);
}

int set_dismod_conf( unsigned char dismode )
{

	if ( dismode == 0 || dismode  )
		ControlSystemData(SFIELD_DIS_MODE, &dismode, sizeof(dismode));

	return 0;
}
#endif

static int check_ipaddress(const char *ipstr)
{
	int ip0,ip1,ip2,ip3;
	if(4 != sscanf(ipstr,"%d.%d.%d.%d",&ip0,&ip1,&ip2,&ip3))
		return(-1);
	if((ip0<0||ip0>223||ip0==127)||(ip1<0||ip1>255)||(ip2<0||ip2>255)||(ip3<0||ip3>254))
		return(-1);
	return(1);
}


static int check_gatewayaddress(const char *ipstr)
{
	int ip0,ip1,ip2,ip3;
	if(4 != sscanf(ipstr,"%d.%d.%d.%d",&ip0,&ip1,&ip2,&ip3))
		return(-1);
	if((ip0<0||ip0>255)||(ip1<0||ip1>255)||(ip2<0||ip2>255)||(ip3<0||ip3>255))
		return(-1);

	return(1);
}


static int check_hostname(const char *str)
{	
	int flag=0;
	if((*str>='a' && *str<='z') || (*str>='A' && *str<='Z'))
	{
		flag=1;
		str++;
	}
	while(*str && flag)
	{
		if((*str>='a' && *str<='z') || (*str>='A' && *str<='Z') || *str=='-' || *str=='.' || (*str>='0' && *str<='9'))
		{
			flag=1;
			str++;
		}
		else
			flag=0;
	}
	return flag;
}
#if 0
static int check_timezone(char *str)
{
	char *p=str;
	char *q=str;

	if((*p>='a' && *p<='z') || (*p>='A' && *p<='Z'))
		p++;
	else
		return (-1);
	
	while(*p)
	{
		if((*p=='+')||((*p=='-')))
		{
			if((p-q)>=3 &&(p-q)<=6)
			{
				p++;
				 if(*p>='0'&&*p<='9')
					return 0;
				 else
					return (-1);
			}		
			else 
				return (-1);
		}
		else if(*p>='0'&&*p<='9')
		{
			if((p-q)>=3 &&(p-q)<=6)
				return 0;
			else 
				return (-1);
		}
		else
			p++;
	}
	return (-1);
}
#endif
static long epoch_convert_switch(int value, char convert, int time)
{
	long seconds = 0;
	switch(convert)
	{
		case 'Y': seconds = value * YEAR_IN_SEC ;
			  break;
		case 'M':
			  if(time == 1)
			  {
				  seconds = value * MIN_IN_SEC;
			  }
			  else
			  {
				  seconds = value * MONTH_IN_SEC;
			  }
			  break;
		case 'D': seconds = value * DAY_IN_SEC;
			  break;
		case 'H': seconds = value * HOURS_IN_SEC;
			  break;
		case 'S': seconds = value;
			  break;
	}
	return seconds;
}

/*
 *  @brief Converts XML period format (PnYnMnDTnHnMnS) to long data type
 */

static long periodtol(char *ptr)
{
	char buff[10];
	char convert;
	int i = 0; 
	int value = 0;
	int time = 0;
	int minus = 0;
	long cumulative = 0;

	buff[0] = '\0';
	if(*ptr == '-')
	{
		ptr++;
		minus = 1;
	}
	while(*ptr != '\0')
	{
		//if(*ptr != 'P' || *ptr != 'T' || *ptr >= '0' || *ptr <= '9')
		//{
		//	ONVIF_DBP("===========periodtol return -1 ptr %c \n", *ptr);
		
		//	return -1;
		//}

		
		if(*ptr == 'P' || *ptr == 'T')
		{
			ptr++;
			if(*ptr == 'T')
			{
				time = 1;
				ptr++;
			}
		}
		else
		{
			if(*ptr >= '0' && *ptr <= '9')
			{
				buff[i] = *ptr;
				i++;
				ptr++;				
			}
			else
			{
				buff[i] = 0;
				value = atoi(buff);
				memset(buff, 0, sizeof(buff));
				i = 0;
				convert = *ptr;
				ptr++;
				cumulative = cumulative + epoch_convert_switch(value, convert, time);
			}
		}
	}
	if(minus == 1)
	{
		return -cumulative;
	}
	else
	{
		return cumulative;
	}
}
#if 0
static unsigned long iptol(char *ip)
{
	unsigned char o1,o2,o3,o4; /* The 4 ocets */
	char tmp[13] = "000000000000\0";
	short i = 11; /* Current Index in tmp */
	short j = (strlen(ip) - 1);
	do 
	{
		if ((ip[--j] == '.'))
		{
			i -= (i % 3);
		}
		else 
		{
			tmp[--i] = ip[j];
		}
	}while (i > -1);

	o1 = (tmp[0] * 100) + (tmp[1] * 10) + tmp[2];
	o2 = (tmp[3] * 100) + (tmp[4] * 10) + tmp[5];
	o3 = (tmp[6] * 100) + (tmp[7] * 10) + tmp[8];
	o4 = (tmp[9] * 100) + (tmp[10] * 10) + tmp[11];
	return (o1 * 16777216) + (o2 * 65536) + (o3 * 256) + o4;
}
#endif
static unsigned int packbits(unsigned char *src, unsigned char *dst, unsigned int n)
{
    unsigned char *p, *q, *run, *dataend;  
    int count, maxrun;  
  
    dataend = src + n;  
    for( run = src, q = dst; n > 0; run = p, n -= count )
	{
        // A run cannot be longer than 128 bytes.  
        maxrun = n < 128 ? n : 128;  
        if(run <= (dataend-3) && run[1] == run[0] && run[2] == run[0])
		{
            // 'run' points to at least three duplicated values.  
            // Step forward until run length limit, end of input,  
            // or a non matching byte:  
            for( p = run+3; p < (run+maxrun) && *p == run[0]; )  
                ++p;  
            count = p - run;  
  
            // replace this run in output with two bytes:  
            *q++ = 1+256-count; /* flag byte, which encodes count (129..254) */  
  
            *q++ = run[0];      /* byte value that is duplicated */  
  
        }
		else
		{  
            // If the input doesn't begin with at least 3 duplicated values,  
            // then copy the input block, up to the run length limit,  
            // end of input, or until we see three duplicated values:  
            for( p = run; p < (run+maxrun); )  
                if(p <= (dataend-3) && p[1] == p[0] && p[2] == p[0])  
                    break; // 3 bytes repeated end verbatim run  
                else  
                    ++p;  
            count = p - run;  
            *q++ = count-1;        /* flag byte, which encodes count (0..127) */  
            memcpy(q, run, count); /* followed by the bytes in the run */  
            q += count;  
        }  
    }
	
    return q - dst;  
}  

static unsigned int unpackbits(unsigned char *outp, unsigned char *inp, unsigned int outlen, unsigned int inlen)  
{  
    unsigned int i, len;  
    int val;  
  
    /* i counts output bytes; outlen = expected output size */  
    for(i = 0; inlen > 1 && i < outlen;)
	{
        /* get flag byte */  
        len = *inp++;  
        --inlen;  
  
        if(len == 128) /* ignore this flag value */  
            ; // warn_msg("RLE flag byte=128 ignored");  
        else
		{
            if(len > 128)
			{
                len = 1+256-len;  
  
                /* get value to repeat */  
                val = *inp++;  
                --inlen;  
  
                if((i+len) <= outlen)  
                    memset(outp, val, len);  
                else
                {
                    memset(outp, val, outlen-i); // fill enough to complete row  
                    printf("unpacked RLE data would overflow row (run)\n");  
                    len = 0; // effectively ignore this run, probably corrupt flag byte  
                }  
            }
			else
			{  
                ++len;  
                if((i+len) <= outlen)
				{
                    if(len > inlen)  
                        break; // abort - ran out of input data  
                    /* copy verbatim run */  
                    memcpy(outp, inp, len);  
                    inp += len;  
                    inlen -= len;  
                }
				else
				{  
                    memcpy(outp, inp, outlen-i); // copy enough to complete row  
                    printf("unpacked RLE data would overflow row (copy)\n");  
                    len = 0; // effectively ignore  
                }  
            }  
			
            outp += len;  
            i += len;  
        }  
    }
	
    if(i < outlen)  
        printf("not enough RLE data for row\n"); 
	
    return i;  
}  

static unsigned char ReverseByte(unsigned char m) 
{
	unsigned char result;
	result = m>>7;
	result |= ((m>>6)&1)<<1;
	result |= ((m>>5)&1)<<2;
	result |= ((m>>4)&1)<<3;
	result |= ((m>>3)&1)<<4;
	result |= ((m>>2)&1)<<5;
	result |= ((m>>1)&1)<<6;
	result |= (m&1)<<7;

	return result;
}

static void pack_motion_cells(const unsigned char motionArea[QMAPI_MD_STRIDE_SIZE], unsigned char packMotionArea[50])
{
	memset(packMotionArea, 0, sizeof(unsigned char) * 50);

	int i;
	int pos;
	int x, y;
	for (i = 0; i < 22 * 18; i++)
	{
		x = i % 22;
		y = i / 22;

		pos = x * 2 + y * 44 * 2;
		if (motionArea[pos / 8] & (1<<(pos % 8)))
		{
			packMotionArea[i / 8] |= 1 << (i % 8);
		}
	}
}

static void unpack_motion_cells(const unsigned char packMotionArea[50], unsigned char motionArea[QMAPI_MD_STRIDE_SIZE])
{
	memset(motionArea, 0, sizeof(unsigned char) * QMAPI_MD_STRIDE_SIZE);

	int i;
	int x, y;
	int pos;

	for (i = 0; i < 22 * 18; i++)
	{
		if (packMotionArea[i / 8]&(1<<(i % 8)))
		{
			x = i % 22;
			y = i / 22;

			pos = x * 2 + y * 2 * 44;
			motionArea[pos / 8] |= (1<<(pos % 8));

			++pos;
			motionArea[pos / 8] |= (1<<(pos % 8));

			pos = x * 2 + (y * 2 + 1) * 44;
			motionArea[pos / 8] |= (1<<(pos % 8));

			++pos;
			motionArea[pos / 8] |= (1<<(pos % 8));
		}
	}
}

int  send_soap_fault_msg(struct soap *soap, int errnum, 
		char *mg0, char *mg1, char *mg2, char *res_txt)
{
	int i = 0;
	FAULT_MG fault_mg;
	memset(&fault_mg, 0, sizeof(FAULT_MG));
	soap->error = errnum;

	if (mg0) {
		strncpy(fault_mg.mg[0], mg0, LEN_32_SIZE - 1);
	}	

	
	if (mg1) {
		strncpy(fault_mg.mg[1], mg1, LEN_32_SIZE - 1);
	}	

	
	if ( mg2 ) {
		strncpy(fault_mg.mg[2], mg2, LEN_32_SIZE - 1);	
	}	

	if ( res_txt ) {
		strncpy(fault_mg.reason_txt, res_txt, LEN_128_SIZE - 1);
	}	
	
	
	if (!soap->fault)
	{
		soap->fault = (struct SOAP_ENV__Fault*)soap_malloc_v2(soap,sizeof(struct SOAP_ENV__Fault));
	}
	struct SOAP_ENV__Code *penv_code = NULL;
	soap->fault->SOAP_ENV__Code = (struct SOAP_ENV__Code*)soap_malloc_v2(soap,sizeof(struct SOAP_ENV__Code));
	penv_code = soap->fault->SOAP_ENV__Code;
	
	do
	{
		{
			penv_code->SOAP_ENV__Value = (char*)soap_malloc_v2(soap, LEN_32_SIZE);
		}

		strcpy(penv_code->SOAP_ENV__Value, fault_mg.mg[i]);
		i++;
	}while(i<MG_INDEX &&
	(penv_code->SOAP_ENV__Subcode = (struct SOAP_ENV__Code*)soap_malloc_v2(soap,sizeof(struct SOAP_ENV__Code)))&&
			(penv_code = penv_code->SOAP_ENV__Subcode));
	
	if(!soap->fault->SOAP_ENV__Reason)
	{
		soap->fault->SOAP_ENV__Reason = soap_malloc_v2(soap, sizeof(struct SOAP_ENV__Reason));
	}
	soap->fault->SOAP_ENV__Reason->SOAP_ENV__Text = soap_malloc(soap, LEN_128_SIZE);
	//memset(soap->fault->SOAP_ENV__Reason->SOAP_ENV__Text, 0, LEN_128_SIZE);
	strcpy(soap->fault->SOAP_ENV__Reason->SOAP_ENV__Text, fault_mg.reason_txt);
	
	//soap_set_fault(soap);
	
	return soap->error;	
}
 
 int  __ns9__Renew(struct soap *soap, struct _wsnt__Renew *wsnt__Renew, struct _wsnt__RenewResponse *wsnt__RenewResponse)
 { 
 
 	ONVIF_DBP(""); 
 	return SOAP_OK; 
	
 }
 int  __ns9__Unsubscribe(struct soap *soap, struct _wsnt__Unsubscribe *wsnt__Unsubscribe, struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse)
 { 
 	ONVIF_DBP(""); 
 	return SOAP_OK; 	
 }
 
 int __tad__GetServiceCapabilities(struct soap *soap, struct _tad__GetServiceCapabilities *tad__GetServiceCapabilities, struct _tad__GetServiceCapabilitiesResponse *tad__GetServiceCapabilitiesResponse)
 {	 
	 ONVIF_DBP(""); 
	 return SOAP_OK;
 }
 
 int  __tad__DeleteAnalyticsEngineControl(struct soap *soap, struct _tad__DeleteAnalyticsEngineControl *tad__DeleteAnalyticsEngineControl, struct _tad__DeleteAnalyticsEngineControlResponse *tad__DeleteAnalyticsEngineControlResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}

int  __tad__CreateAnalyticsEngineControl(struct soap *soap, struct _tad__CreateAnalyticsEngineControl *tad__CreateAnalyticsEngineControl, struct _tad__CreateAnalyticsEngineControlResponse *tad__CreateAnalyticsEngineControlResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__SetAnalyticsEngineControl(struct soap *soap, struct _tad__SetAnalyticsEngineControl *tad__SetAnalyticsEngineControl, struct _tad__SetAnalyticsEngineControlResponse *tad__SetAnalyticsEngineControlResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__GetAnalyticsEngineControl(struct soap *soap, struct _tad__GetAnalyticsEngineControl *tad__GetAnalyticsEngineControl, struct _tad__GetAnalyticsEngineControlResponse *tad__GetAnalyticsEngineControlResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__GetAnalyticsEngineControls(struct soap *soap, struct _tad__GetAnalyticsEngineControls *tad__GetAnalyticsEngineControls, struct _tad__GetAnalyticsEngineControlsResponse *tad__GetAnalyticsEngineControlsResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__GetAnalyticsEngine(struct soap *soap, struct _tad__GetAnalyticsEngine *tad__GetAnalyticsEngine, struct _tad__GetAnalyticsEngineResponse *tad__GetAnalyticsEngineResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__GetAnalyticsEngines(struct soap *soap, struct _tad__GetAnalyticsEngines *tad__GetAnalyticsEngines, struct _tad__GetAnalyticsEnginesResponse *tad__GetAnalyticsEnginesResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__SetVideoAnalyticsConfiguration(struct soap *soap, struct _tad__SetVideoAnalyticsConfiguration *tad__SetVideoAnalyticsConfiguration, struct _tad__SetVideoAnalyticsConfigurationResponse *tad__SetVideoAnalyticsConfigurationResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__SetAnalyticsEngineInput(struct soap *soap, struct _tad__SetAnalyticsEngineInput *tad__SetAnalyticsEngineInput, struct _tad__SetAnalyticsEngineInputResponse *tad__SetAnalyticsEngineInputResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__GetAnalyticsEngineInput(struct soap *soap, struct _tad__GetAnalyticsEngineInput *tad__GetAnalyticsEngineInput, struct _tad__GetAnalyticsEngineInputResponse *tad__GetAnalyticsEngineInputResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__GetAnalyticsEngineInputs(struct soap *soap, struct _tad__GetAnalyticsEngineInputs *tad__GetAnalyticsEngineInputs, struct _tad__GetAnalyticsEngineInputsResponse *tad__GetAnalyticsEngineInputsResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__GetAnalyticsDeviceStreamUri(struct soap *soap, struct _tad__GetAnalyticsDeviceStreamUri *tad__GetAnalyticsDeviceStreamUri, struct _tad__GetAnalyticsDeviceStreamUriResponse *tad__GetAnalyticsDeviceStreamUriResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__GetVideoAnalyticsConfiguration(struct soap *soap, struct _tad__GetVideoAnalyticsConfiguration *tad__GetVideoAnalyticsConfiguration, struct _tad__GetVideoAnalyticsConfigurationResponse *tad__GetVideoAnalyticsConfigurationResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__CreateAnalyticsEngineInputs(struct soap *soap, struct _tad__CreateAnalyticsEngineInputs *tad__CreateAnalyticsEngineInputs, struct _tad__CreateAnalyticsEngineInputsResponse *tad__CreateAnalyticsEngineInputsResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__DeleteAnalyticsEngineInputs(struct soap *soap, struct _tad__DeleteAnalyticsEngineInputs *tad__DeleteAnalyticsEngineInputs, struct _tad__DeleteAnalyticsEngineInputsResponse *tad__DeleteAnalyticsEngineInputsResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}
int  __tad__GetAnalyticsState(struct soap *soap, struct _tad__GetAnalyticsState *tad__GetAnalyticsState, struct _tad__GetAnalyticsStateResponse *tad__GetAnalyticsStateResponse)
{ 
	ONVIF_DBP(""); 
	return SOAP_OK; 	
}

int  __tds__GetDeviceInformation(struct soap *soap, 
								struct _tds__GetDeviceInformation *tds__GetDeviceInformation, 
								struct _tds__GetDeviceInformationResponse *tds__GetDeviceInformationResponse)
{ 		
	char progv[LEN_16_SIZE];

	memset(progv,0,sizeof(progv));
	if(get_progver(progv)<0)
		return SOAP_ERR;

	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;
	QMAPI_NET_DEVICE_INFO stDeviceInfo;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));
	memset(&stDeviceInfo, 0, sizeof(QMAPI_NET_DEVICE_INFO));
	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, &stDeviceInfo, sizeof(QMAPI_NET_DEVICE_INFO));

	tds__GetDeviceInformationResponse->Manufacturer = (char *)soap_malloc(soap, LEN_12_SIZE);
	//strcpy(tds__GetDeviceInformationResponse->Manufacturer,"IPCAM");

	tds__GetDeviceInformationResponse->Model = (char *)soap_malloc(soap, LEN_32_SIZE);
	//strcpy(tds__GetDeviceInformationResponse->Model,hw_model);
	//strcpy(tds__GetDeviceInformationResponse->Model, "IPCAM");

	if(IPC_3420P == stDeviceInfo.dwServerType)
	{
		strcpy(tds__GetDeviceInformationResponse->Manufacturer, "IPC-3420P");
		strcpy(tds__GetDeviceInformationResponse->Model, "IPC-3420P");
	}
	else if(IPC_3520P == stDeviceInfo.dwServerType)
	{
		strcpy(tds__GetDeviceInformationResponse->Manufacturer, "IPC-3520P");
		strcpy(tds__GetDeviceInformationResponse->Model, "IPC-3520P");
	}
	else
	{
		strcpy(tds__GetDeviceInformationResponse->Manufacturer, "IPC-3420P");
		strcpy(tds__GetDeviceInformationResponse->Model, "IPC-3420P");
	}

	tds__GetDeviceInformationResponse->FirmwareVersion = (char *)soap_malloc(soap, LEN_32_SIZE);
	strcpy(tds__GetDeviceInformationResponse->FirmwareVersion,progv);

	tds__GetDeviceInformationResponse->SerialNumber = (char *)soap_malloc(soap, LEN_32_SIZE);
	sprintf(tds__GetDeviceInformationResponse->SerialNumber,
						"%02X%02X%02X%02X%02X%02X", net_conf.stEtherNet[0].byMACAddr[0],
						net_conf.stEtherNet[0].byMACAddr[1], net_conf.stEtherNet[0].byMACAddr[2],
						net_conf.stEtherNet[0].byMACAddr[3], net_conf.stEtherNet[0].byMACAddr[4],
						net_conf.stEtherNet[0].byMACAddr[5]);

	tds__GetDeviceInformationResponse->HardwareId = (char *)soap_malloc(soap, LEN_64_SIZE);
	sprintf(tds__GetDeviceInformationResponse->HardwareId, "1419d68a-1dd2-11b2-a105-%02X%02X%02X%02X%02X%02X", 
						net_conf.stEtherNet[0].byMACAddr[0], net_conf.stEtherNet[0].byMACAddr[1], net_conf.stEtherNet[0].byMACAddr[2], 
						net_conf.stEtherNet[0].byMACAddr[3], net_conf.stEtherNet[0].byMACAddr[4], net_conf.stEtherNet[0].byMACAddr[5]);
	//strcpy(tds__GetDeviceInformationResponse->HardwareId,partmun);

	return SOAP_OK;
}
 

static int ParseTimeZone( char *zonename, int *hour, int *min)
{

	char *p;
	char *q = NULL;

	char a;
	int zone = 100;
	int Signed = 0;

	*min = 0;
	*hour = 0;

	q = strchr(zonename, '-');
	if(q)
	{
		Signed = 1;
		a = *(q+1);
		if(a >= '0' && a <= '9')
			zone = atoi(q+1);
		else
			return -1;
	}
	else
	{
		for(p=zonename; *p !='\0'; p++)
		{
			if(*p>='0'&&*p<='9')
			{
				zone = -atoi(p);
				break;
			}
		}
	}

	q = strchr(zonename, ':');
	if(q)
	{
		a = *(q+1);
		if(a >= '0' && a <= '9')
		{
			*min = atoi(q+1);
			if(*min >= 60)
				return -2;

			if(!Signed)
				*min = -*min;
		}
		else
			return -3;
	}

	if(zone != 100)
	{
		*hour = zone;
		return 0;
	}
	
	if (strncmp(zonename, "IDLW", 4)==0)
		zone = -12;
	else if (strncmp(zonename, "NT", 2)==0)
		zone = -11;
	else if ((strncmp(zonename, "AHST", 4)==0)||(strncmp(zonename, "CAT", 3)==0)
		||(strncmp(zonename, "HST", 3)==0)||strncmp(zonename,"HAST",4) == 0)
		zone = -10;
	else if ((strncmp(zonename, "HDT", 3)==0) || (strncmp(zonename, "YST", 3)==0))
		zone = -9;
	else if ((strncmp(zonename, "PST", 3)==0) ||(strncmp(zonename, "YDT", 3)==0))
		zone = -8;
	else if ((strncmp(zonename, "MST", 3)==0) ||(strncmp(zonename, "PDT", 3)==0))
		zone = -7;
	else if ((strncmp(zonename, "CST", 3)==0) ||(strncmp(zonename, "MDT", 3)==0))
		zone = -6;
	else if ((strncmp(zonename, "EST", 3)==0) ||(strncmp(zonename, "CDT", 3)==0))
		zone = -5;
	else if ((strncmp(zonename, "AST", 3)==0) ||(strncmp(zonename, "EDT", 3)==0))
		zone = -4;
	else if (strncmp(zonename, "ADT", 3)==0)
		zone = -3;
	else if (strncmp(zonename, "AT", 2)==0)
		zone = -2;
	else if (strncmp(zonename, "WAT", 3)==0)
		zone = -1;
	else if ((strncmp(zonename, "GMT", 3)==0)||(strncmp(zonename, "UT", 2)==0)||
		(strncmp(zonename, "UTC", 3)==0)||(strncmp(zonename, "WET", 3)==0))
		zone = 0;
	else if ((strncmp(zonename, "CET", 3)==0)||(strncmp(zonename, "MET", 3)==0)\
		||(strncmp(zonename, "MEWT", 4)==0)||(strncmp(zonename, "SWT", 3)==0)\
		||(strncmp(zonename, "MESZ", 4)==0)||(strncmp(zonename, "FST", 3)==0)||
		(strncmp(zonename, "DNT", 3)==0) || (strncmp(zonename, "MEZ", 3)==0) ||
		(strncmp(zonename, "NOR", 3)==0) || (strncmp(zonename, "SET", 3)==0) ||
		(strncmp(zonename, "WETDST", 6)==0) || (strncmp(zonename, "BST", 3)==0))
		zone = 1;
	else if (strncmp(zonename, "EET", 3)==0 ||(strncmp(zonename, "FWT", 3)==0) ||
		(strncmp(zonename, "MEST", 4)==0) ||(strncmp(zonename, "SST", 3)==0)||
		(strncmp(zonename, "CETDST", 6)==0) || (strncmp(zonename, "IST", 3)==0)||
		(strncmp(zonename, "METDST", 6)==0))
		zone = 2;
	else if (strncmp(zonename, "BT", 2)==0 || (strncmp(zonename, "EETDST", 6)==0))
		zone = 3;//3
	else if (strncmp(zonename, "ZP4", 3)==0)
		zone = 4;//4
	else if (strncmp(zonename, "ZP5", 3)==0)
		zone = 5;
	else if (strncmp(zonename, "ZP6", 3)==0)
		zone = 6;
	else if (strncmp(zonename, "ZP7", 3)==0)
		zone = 7;
	else if (strncmp(zonename, "AWST", 4)==0 || (strncmp(zonename, "WST", 3)==0)||
		(strncmp(zonename, "CCT", 3)==0) || strncmp(zonename,"CST-8",5) == 0)
		zone = 8;
	else if (strncmp(zonename, "JST", 3)==0 || strncmp(zonename,"WDT",3) == 0 ||
		strncmp(zonename,"AWSST",5) == 0)
		zone = 9;
	else if (strncmp(zonename, "ACT", 3)==0 ||strncmp(zonename,"EST",3) == 0 ||
		strncmp(zonename,"EAST",4) == 0 ||strncmp(zonename,"GST",3) == 0 ||
		strncmp(zonename,"LIGT",4) == 0)
		zone = 10;
	else if (strncmp(zonename, "AESST", 5)==0 ||strncmp(zonename,"SBT",3) == 0)
		zone = 11;
	else if (strncmp(zonename, "IDLE", 4)==0 || strncmp(zonename,"NZST",4) == 0 ||
		strncmp(zonename,"NZT",3) == 0)
		zone = 12;
	else 
	{
		printf("%s  %d,  unknow time zone : %s\n",__FUNCTION__, __LINE__, zonename);
		return -4;
	}


	*hour = zone;
	return 0;
}


static int ConvertTimeZone_Time2DMS(int hour, int min)
{
	switch (hour)
	{
		case -12:
			return QMAPI_GMT_N12;
		case -11:
			return QMAPI_GMT_N11;
		case -10:
			return QMAPI_GMT_N10;
		case -9:
			return QMAPI_GMT_N09;
		case -8:
			return QMAPI_GMT_N08;
		case -7:
			return QMAPI_GMT_N07;
		case -6:
			return QMAPI_GMT_N06;
		case -5:
			return QMAPI_GMT_N05;
		case -4:
			if(min)
				return QMAPI_GMT_N0430;
			
			return QMAPI_GMT_N04;
		case -3:
			if(min)
				return QMAPI_GMT_N0330;
			
			return QMAPI_GMT_N03;
		case -2:
			return QMAPI_GMT_N02;
		case -1:
			return QMAPI_GMT_N01;
		case 0:
			return QMAPI_GMT_00;
		case 1:
			return QMAPI_GMT_01;
		case 2:
			return QMAPI_GMT_02;
		case 3:
			if(min)
				return QMAPI_GMT_0330;
			
			return QMAPI_GMT_03;
		case 4:
			if(min)
				return QMAPI_GMT_0430;
			
			return QMAPI_GMT_04;
		case 5:
			if(min == 30)
				return QMAPI_GMT_0530;
			else if(min == 45)
				return QMAPI_GMT_0545;
				
			return QMAPI_GMT_05;
		case 6:
			if(min)
				return QMAPI_GMT_0630;
			return QMAPI_GMT_06;
		case 7:
			return QMAPI_GMT_07;
		case 8:
			return QMAPI_GMT_08;
		case 9:
			if(min)
				return QMAPI_GMT_0930;
			
			return QMAPI_GMT_09;
		case 10:
			return QMAPI_GMT_10;
		case 11:
			return QMAPI_GMT_11;
		case 12:
			return QMAPI_GMT_12;
		case 13:
			return QMAPI_GMT_13;
		default:
			return 100;
	}

	return 100;
}
//int _DateTimeType = 0;

static int isVaildGMTTimeZone(const char *timezone, int *zonevalue1, int *zonevalue2)
{
	if (!timezone)
	{
		return 0;
	}

	*zonevalue1 = -1;
	*zonevalue2 = -1;
	sscanf(timezone, "GMT%d:%d", zonevalue1, zonevalue2);

	if (*zonevalue1 == -1 || *zonevalue2 == -1)
	{
		return 0;
	}

	if (*zonevalue1 > 13 || *zonevalue1 < -12)
	{
		return 0;
	}

	if (*zonevalue2)
	{
		if (*zonevalue2 == 45)
		{
			if (*zonevalue1 != 5)
			{
				return 0;
			}
		}
		else if (*zonevalue2 == 30)
		{
			if ((*zonevalue1 != -4) && (*zonevalue1 != -3) && (*zonevalue1 != 3)
				&& (*zonevalue1 != 4) && (*zonevalue1 != 5) && (*zonevalue1 != 6) && (*zonevalue1 != 9))
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	
	return 1;
}

int __tds__SetSystemDateAndTime(struct soap *soap, struct _tds__SetSystemDateAndTime *tds__SetSystemDateAndTime, struct _tds__SetSystemDateAndTimeResponse *tds__SetSystemDateAndTimeResponse)
{
	int dms_tz = 100;
	char *_TZ = NULL;
	int tz_hour = 0, tz_min = 0;
	int bSetZone = 0;
	
	QMAPI_SYSTEMTIME dms_time;
	QMAPI_NET_ZONEANDDST zone_info;

	memset(&dms_time, 0, sizeof(QMAPI_SYSTEMTIME));
	memset(&zone_info, 0, sizeof(QMAPI_NET_ZONEANDDST));

	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ZONEANDDSTCFG, 0, &zone_info, sizeof(QMAPI_NET_ZONEANDDST));
	if (zone_info.dwEnableDST != tds__SetSystemDateAndTime->DaylightSavings)
	{
		zone_info.dwEnableDST = tds__SetSystemDateAndTime->DaylightSavings;
		bSetZone = 1;
	}
	
	if (tds__SetSystemDateAndTime->TimeZone)
	{
		if (!tds__SetSystemDateAndTime->TimeZone->TZ)
		{
			return(send_soap_fault_msg(soap, 400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:InvalidTimeZone",
				"time zone null"));
		}
		printf("time zone:%s\n",tds__SetSystemDateAndTime->TimeZone->TZ);

		_TZ  = tds__SetSystemDateAndTime->TimeZone->TZ;		
		strncpy(soap->pConfig->sys_time_zone, tds__SetSystemDateAndTime->TimeZone->TZ, 128);

		printf("setsystem datetime:%s\n", soap->pConfig->sys_time_zone);
		if (ParseTimeZone(_TZ, &tz_hour, &tz_min) < 0 && !isVaildGMTTimeZone(_TZ, &tz_hour, &tz_min))
		{
			printf("%s  %d, unknow time zone name: %s\n",__FUNCTION__, __LINE__, _TZ);
			return(send_soap_fault_msg(soap, 400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:InvalidTimeZone",
				"the matching rule specified is not supported"));
		}
		
		dms_tz = ConvertTimeZone_Time2DMS(tz_hour, tz_min);
		if(dms_tz == 100)
		{
			printf("timezone error!tz_hour=%d, tz_min=%d\n", tz_hour, tz_min);
			return(send_soap_fault_msg(soap, 400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:InvalidTimeZone",
				"the matching rule specified is not supported"));
		}

		if(zone_info.nTimeZone != dms_tz)
		{
			zone_info.nTimeZone = dms_tz;
			bSetZone = 1;
		}

		soap->pConfig->ConvertTimeZone(dms_tz, &tz_hour, &tz_min);
	}
	else
	{
		soap->pConfig->ConvertTimeZone(zone_info.nTimeZone, &tz_hour, &tz_min);
	}

	QMAPI_NET_NTP_CFG ntp_info;
	memset(&ntp_info, 0, sizeof(QMAPI_NET_NTP_CFG));
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NTPCFG, 0, &ntp_info, sizeof(QMAPI_NET_NTP_CFG));
	
	if (tds__SetSystemDateAndTime->DateTimeType == tt__SetDateTimeType__NTP)
	{
		if (ntp_info.byEnableNTP != tds__SetSystemDateAndTime->DateTimeType)
		{
			ntp_info.byEnableNTP = 1;
			QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_NTPCFG, 0, &ntp_info, sizeof(QMAPI_NET_NTP_CFG));			
		}

		if (bSetZone)
		{
			QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_ZONEANDDSTCFG, 0, &zone_info, sizeof(QMAPI_NET_ZONEANDDST));
		}
		
		return SOAP_OK;
	}
	else if (ntp_info.byEnableNTP) //设置为本地时间时关闭NTP，以此区分当前的时间模式
	{
		printf("close ntp!\n");
		ntp_info.byEnableNTP = 0;
		QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_NTPCFG, 0, &ntp_info, sizeof(QMAPI_NET_NTP_CFG));	
	}

	if ( tds__SetSystemDateAndTime->UTCDateTime) {
		if (tds__SetSystemDateAndTime->UTCDateTime->Date->Year < 1900
			|| (tds__SetSystemDateAndTime->UTCDateTime->Date->Month > 12 || tds__SetSystemDateAndTime->UTCDateTime->Date->Month < 1)
			|| (tds__SetSystemDateAndTime->UTCDateTime->Date->Day > 31 || tds__SetSystemDateAndTime->UTCDateTime->Date->Day < 1)
			|| (tds__SetSystemDateAndTime->UTCDateTime->Time->Hour > 23 || tds__SetSystemDateAndTime->UTCDateTime->Time->Hour < 0)
			|| (tds__SetSystemDateAndTime->UTCDateTime->Time->Minute > 59 || tds__SetSystemDateAndTime->UTCDateTime->Time->Minute < 0)
			|| (tds__SetSystemDateAndTime->UTCDateTime->Time->Second > 59 || tds__SetSystemDateAndTime->UTCDateTime->Time->Second < 0))
		{
			printf("check date time failed!\n");
			return(send_soap_fault_msg(soap, 400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:InvalidDateTime",
				"An invalid date or time was specified."));
		}
		
		int year, mon, day, hour, min, sec;
		year = tds__SetSystemDateAndTime->UTCDateTime->Date->Year;
		mon = tds__SetSystemDateAndTime->UTCDateTime->Date->Month;
		day = tds__SetSystemDateAndTime->UTCDateTime->Date->Day;
		hour = tds__SetSystemDateAndTime->UTCDateTime->Time->Hour;
		min = tds__SetSystemDateAndTime->UTCDateTime->Time->Minute;
		sec = tds__SetSystemDateAndTime->UTCDateTime->Time->Second;
		
		struct tm tm_temp;
		time_t ttt = 0;
		ttt = time(NULL);		
		struct tm tm1={0}, tm2={0};
		localtime_r(&ttt, &tm1);	
		memcpy(&tm_temp, &tm1, sizeof(struct tm));
	
		tm_temp.tm_sec = sec;
		tm_temp.tm_min = min;
		tm_temp.tm_hour = hour;
		tm_temp.tm_mday = day;
		tm_temp.tm_mon = mon > 1 ? mon - 1 : 0;
		tm_temp.tm_year = year > 1900 ? year - 1900 : 0;
		tm_temp.tm_isdst = tds__SetSystemDateAndTime->DaylightSavings;		

		if ((ttt = mktime(&tm_temp)) >= 0)
		{
			ttt += (tz_hour*3600 + tz_min*60);
			
			localtime_r(&ttt, &tm2);
			year = tm2.tm_year + 1900;
			mon = tm2.tm_mon + 1;
			day = tm2.tm_mday;
			hour = tm2.tm_hour;
			min = tm2.tm_min;
			sec = tm2.tm_sec;
		}		

		dms_time.wYear = year;
		dms_time.wMonth = mon;
		dms_time.wDay = day;
		dms_time.wHour = hour;
		dms_time.wMinute = min;
		dms_time.wSecond = sec;

		QMapi_sys_ioctrl(0, QMAPI_NET_STA_SET_SYSTIME, 0, &dms_time, sizeof(QMAPI_SYSTEMTIME));
	}	

	if (bSetZone)
	{
		QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_ZONEANDDSTCFG, 0, &zone_info, sizeof(QMAPI_NET_ZONEANDDST));
	}
	return SOAP_OK;

}


#if 0
char *_TZ_name[25] = {
	"GMT-12",// Eniwetok, Kwajalein", 
	"GMT-11",// Midway Island, Samoa",
	"GMT-10",// Hawaii", // HAW10
	"GMT-09",// Alaska", // AKST9AKDT,M3.2.0,M11.1.0
	"GMT-08",// Pacific Time (US & Canada), Tijuana", // PST8PDT,M3.2.0,M11.1.0
	"GMT-07",// Mountain Time (US & Canada), Arizona", // MST7
	"GMT-06",// Central Time (US & Canada), Mexico City, Tegucigalpa, Saskatchewan", // CST6CDT,M3.2.0,M11.1.0
	"GMT-05",// Eastern Time (US & Canada), Indiana(East), Bogota, Lima", // EST5EDT,M3.2.0,M11.1.0
	"GMT-04",// Atlantic Time (Canada), Caracas, La Paz", // AST4ADT,M4.1.0/00:01:00,M10.5.0/00:01:00
	"GMT-03",// Brasilia, Buenos Aires, Georgetown",
	"GMT-02",// Mid-Atlantic",
	"GMT-01",// Azores, Cape Verdes Is.",
	"GMT+00",// GMT, Dublin, Edinburgh, London, Lisbon, Monrovia, Casablanca", // GMT+0BST-1,M3.5.0/01:00:00,M10.5.0/02:00:00
	"GMT+01",// Berlin, Stockholm, Rome, Bern, Brussels, Vienna, Paris, Madrid, Amsterdam, Prague, Warsaw, Budapest", // CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
	"GMT+02",// Athens, Helsinki, Istanbul, Cairo, Eastern Europe, Harare, Pretoria, Israel", // EET-2EEST-3,M3.5.0/03:00:00,M10.5.0/04:00:00
	"GMT+03",// Baghdad, Kuwait, Nairobi, Riyadh, Moscow, St. Petersburg, Kazan, Volgograd", // MSK-3MSD,M3.5.0/2,M10.5.0/3
	"GMT+04",// Abu Dhabi, Muscat, Tbilisi",
	"GMT+05",// Islamabad, Karachi, Ekaterinburg, Tashkent",
	"GMT+06",// Alma Ata, Dhaka",
	"GMT+07",// Bangkok, Jakarta, Hanoi",
	"GMT+08",// Taipei, Beijing, Chongqing, Urumqi, Hong Kong, Perth, Singapore",
	"GMT+09",// Tokyo, Osaka, Sapporo, Seoul, Yakutsk",
	"GMT+10",// Brisbane, Melbourne, Sydney, Guam, Port Moresby, Vladivostok, Hobart",
	"GMT+11",// Magadan, Solomon Is., New Caledonia",
	"GMT+12",// Fiji, Kamchatka, Marshall Is., Wellington, Auckland"
};

char *oget_timezone(int no)
{
	return _TZ_name[no];
}
#endif


static char *dms_TZ_item[] = {
				"GMT12",     	// 0	
				"SST11", 	
				"HAST10", 	
				"AKST9", 	
				"PST8", 	
				"MST7", 	
				"CST6", 	
				"EST5", 	
				"VET4:30", 	
				"AST4", 	
				"NST3:30", 	
				"BRT3", 
				"GST2", 	
				"CVT1", 	
				"GMT0",		
				"CET-1", 	
				"EET-2", 	
				"AST-3", 	
				"IRT-3:30",	
				"GMT-4",  	
				"AFT-4:30",  	
				"PKT-5",     	
				"IST-5:30",  	
				"NPT-5:45", 
				"BDT-6",     	
				"MMT-6:30",
				"ICT-7",     	
				"CST-8",     	//27
				"JST-9",     	
				"CST-9:30",  	
				"EST-10", 	
				"SBT-11", 	
				"NZST-12", 		
				"TOT-13",		//33

				/* daylight saving time */
				"AKST9AKDT\n", 
				"PST8PDT,M3.2.0/02:00:00,M11.1.0/02:00:00\n", // 35
				"MST7MDT\n", 	
				"GMT+0IST-1,M3.5.0/01:00:00,M10.5.0/02:00:00\n", // 36
				"WET-0WEST-1,M3.5.0/01:00:00,M10.5.0/02:00:00\n",
				"GMT+0BST-1,M3.5.0/01:00:00,M10.5.0/02:00:00\n", // 38
				"CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00\n", 		
				"CFT-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00\n",// 40 
				"EET-2EEST-3,M3.5.0/03:00:00,M10.5.0/04:00:00\n", 		
				"MSK-3MSD,M3.5.0/2,M10.5.0/3\n", 		 // 42
				"MST-3MDT,M3.5.0/2,M10.5.0/3\n", 	
				"CST-9:30CDT-10:30,M10.5.0/02:00:00,M3.5.0/03:00:00\n", // 44
				"EST-10EDT-11,M10.5.0/02:00:00,M3.5.0/03:00:00\n", 		
				"EST-10EDT-11,M10.1.0/02:00:00,M3.5.0/03:00:00\n", 	// 46
				"NZST-12NZDT-13,M10.1.0/02:00:00,M3.3.0/03:00:00\n", 
				"\0"};

int  __tds__GetSystemDateAndTime(struct soap *soap, 
struct _tds__GetSystemDateAndTime *tds__GetSystemDateAndTime, 
struct _tds__GetSystemDateAndTimeResponse *tds__GetSystemDateAndTimeResponse)
{	
	QMAPI_NET_NTP_CFG ntp_info;
	memset(&ntp_info, 0, sizeof(QMAPI_NET_NTP_CFG));
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NTPCFG, 0,&ntp_info, sizeof(QMAPI_NET_NTP_CFG));

/*             生成本地时间       */
	QMAPI_SYSTEMTIME curTime = {0};
	QMAPI_SYSTEMTIME utcTime = {0};

	memset(&curTime, 0, sizeof(QMAPI_SYSTEMTIME));
	QMapi_sys_ioctrl(0, QMAPI_NET_STA_GET_SYSTIME, 0, &curTime, sizeof(curTime));

	QMAPI_NET_ZONEANDDST zone_info;
	int tz_hour, tz_min;

	memset(&zone_info, 0, sizeof(QMAPI_NET_ZONEANDDST));
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ZONEANDDSTCFG, 0, &zone_info, sizeof(QMAPI_NET_ZONEANDDST));
	soap->pConfig->ConvertTimeZone(zone_info.nTimeZone, &tz_hour, &tz_min);

	struct tm tmTime = {0};
	tmTime.tm_year = curTime.wYear - 1900;
	tmTime.tm_mon = curTime.wMonth - 1;
	tmTime.tm_mday = curTime.wDay;
	tmTime.tm_hour = curTime.wHour;
	tmTime.tm_min = curTime.wMinute;
	tmTime.tm_sec = curTime.wSecond;
	tmTime.tm_isdst = zone_info.dwEnableDST;
	
	time_t ts = mktime(&tmTime) + tz_hour * 60 + tz_min;
	gmtime_r(&ts, &tmTime);
	
	utcTime.wYear = tmTime.tm_year + 1900;
	utcTime.wMonth = tmTime.tm_mon + 1;
	utcTime.wDay = tmTime.tm_mday;
	utcTime.wHour = tmTime.tm_hour;
	utcTime.wMinute = tmTime.tm_min;
	utcTime.wSecond = tmTime.tm_sec;
	printf("curtime is %d-%d-%d:%d-%d-%d utc time is %d-%d-%d:%d-%d-%d\n", curTime.wYear, curTime.wMonth, curTime.wDay,
		curTime.wHour, curTime.wMinute, curTime.wSecond, utcTime.wYear, utcTime.wMonth, utcTime.wDay,
		utcTime.wHour, utcTime.wMinute, utcTime.wSecond);
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime = 
		soap_malloc_v2(soap,sizeof(struct tt__SystemDateTime));
	
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->DateTimeType = 
			ntp_info.byEnableNTP;

	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->DaylightSavings = zone_info.dwEnableDST;
	
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->TimeZone = 
		soap_malloc(soap,sizeof(struct tt__TimeZone));
	
	printf("getsystem datetime:%s\n", soap->pConfig->sys_time_zone);
	if (soap->pConfig->sys_time_zone[0])
	{
		tds__GetSystemDateAndTimeResponse->SystemDateAndTime->TimeZone->TZ = soap->pConfig->sys_time_zone;
	}
	else
	{
		tds__GetSystemDateAndTimeResponse->SystemDateAndTime->TimeZone->TZ = soap_malloc(soap, LEN_128_SIZE);	
		strncpy(tds__GetSystemDateAndTimeResponse->SystemDateAndTime->TimeZone->TZ,  dms_TZ_item[zone_info.nTimeZone],LEN_128_SIZE);
	}
	
	/************************UTC time****************************/
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime = 
		soap_malloc(soap,sizeof(struct tt__DateTime));
	
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Time = 
		soap_malloc(soap,sizeof(struct tt__Time));
	
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Time->Hour = 
		utcTime.wHour;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Time->Minute = 
		utcTime.wMinute;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Time->Second = 
		utcTime.wSecond;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Date = 
		soap_malloc(soap,sizeof(struct tt__Date));
	
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Date->Year = 
		utcTime.wYear;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Date->Month = 
		utcTime.wMonth;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->UTCDateTime->Date->Day = 
		utcTime.wDay;
	
	/************************local time****************************/
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime = 
		soap_malloc(soap,sizeof(struct tt__DateTime));
	
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Time = 
		soap_malloc(soap,sizeof(struct tt__Time));
	
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Time->Hour = 
		curTime.wHour;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Time->Minute = 
		curTime.wMinute;	
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Time->Second = 
		curTime.wSecond;
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Date = 
		soap_malloc(soap,sizeof(struct tt__Date));
	
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Date->Year = 
		curTime.wYear;	
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Date->Month = 
		curTime.wMonth;	
	tds__GetSystemDateAndTimeResponse->SystemDateAndTime->LocalDateTime->Date->Day = 
		curTime.wDay;	

	return SOAP_OK; 

}

int  __tds__SetSystemFactoryDefault(struct soap *soap, 
	struct _tds__SetSystemFactoryDefault *tds__SetSystemFactoryDefault, 
	struct _tds__SetSystemFactoryDefaultResponse *tds__SetSystemFactoryDefaultResponse)
{ 	

	if(tds__SetSystemFactoryDefault->FactoryDefault == tt__FactoryDefaultType__Soft)
		soap->pConfig->system_reboot_flag = RESTORE_REBOOT;
	else
		soap->pConfig->system_reboot_flag = RESET_REBOOT;

	return SOAP_OK; 	

}

int  __tds__UpgradeSystemFirmware(struct soap *soap, struct _tds__UpgradeSystemFirmware *tds__UpgradeSystemFirmware, struct _tds__UpgradeSystemFirmwareResponse *tds__UpgradeSystemFirmwareResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __tds__SystemReboot(struct soap *soap, 
	struct _tds__SystemReboot *tds__SystemReboot, 
	struct _tds__SystemRebootResponse *tds__SystemRebootResponse)
{
	tds__SystemRebootResponse->Message = soap_strdup(soap, "PT50S");
	
	soap->pConfig->system_reboot_flag = NORMAL_REBOOT;

	printf("***system reboot***\n");
	
	return SOAP_OK; 
}
 
 int  __tds__RestoreSystem(struct soap *soap, struct _tds__RestoreSystem *tds__RestoreSystem, struct _tds__RestoreSystemResponse *tds__RestoreSystemResponse){ return SOAP_OK; }
 int  __tds__GetSystemBackup(struct soap *soap, struct _tds__GetSystemBackup *tds__GetSystemBackup, struct _tds__GetSystemBackupResponse *tds__GetSystemBackupResponse){ return SOAP_OK; }

int __tds__GetSystemLog(struct soap* soap, 
	struct _tds__GetSystemLog *tds__GetSystemLog, 
	struct _tds__GetSystemLogResponse *tds__GetSystemLogResponse)
{
	ONVIF_DBP("");


	tds__GetSystemLogResponse->SystemLog = (struct tt__SystemLog*)soap_malloc_v2(soap,sizeof(struct tt__SystemLog));

	tds__GetSystemLogResponse->SystemLog->String = (char*)soap_malloc(soap, LEN_128_SIZE);
	strcpy(tds__GetSystemLogResponse->SystemLog->String,  "no log");//sys_log_conf.server);	

	return SOAP_OK;
}

int  __tds__GetSystemSupportInformation(struct soap *soap, struct _tds__GetSystemSupportInformation *tds__GetSystemSupportInformation, struct _tds__GetSystemSupportInformationResponse *tds__GetSystemSupportInformationResponse)
{ 

	ONVIF_DBP("");

	return SOAP_OK; 

}

int __tds__GetScopes(struct soap *soap, struct _tds__GetScopes *tds__GetScopes, struct _tds__GetScopesResponse *tds__GetScopesResponse)
{
	char csChannelName[64];

	QMAPI_NET_CHANNEL_OSDINFO osd_info;
	memset(&osd_info, 0, sizeof(QMAPI_NET_CHANNEL_OSDINFO));
	int ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_OSDCFG, 0, &osd_info, sizeof(QMAPI_NET_CHANNEL_OSDINFO));
	if(ret)
	{
		//printf("Get Osd config failed!\n");
		return SOAP_ERR;
	}

	tds__GetScopesResponse->__sizeScopes = 6;

	tds__GetScopesResponse->Scopes = soap_malloc_v2(soap, 9 * sizeof(struct tt__Scope));
	tds__GetScopesResponse->Scopes[0].ScopeDef = tt__ScopeDefinition__Fixed;
	tds__GetScopesResponse->Scopes[0].ScopeItem = (char *)soap_malloc(soap,LEN_64_SIZE);
	strcpy(tds__GetScopesResponse->Scopes[0].ScopeItem, "onvif://www.onvif.org/type/video_encoder");
	tds__GetScopesResponse->Scopes[1].ScopeDef = tt__ScopeDefinition__Fixed;
	tds__GetScopesResponse->Scopes[1].ScopeItem = (char *)soap_malloc(soap,LEN_64_SIZE);
	strcpy(tds__GetScopesResponse->Scopes[1].ScopeItem, "onvif://www.onvif.org/type/audio_encoder");
	tds__GetScopesResponse->Scopes[2].ScopeDef = tt__ScopeDefinition__Fixed;
	tds__GetScopesResponse->Scopes[2].ScopeItem = (char *)soap_malloc(soap,LEN_64_SIZE);
	strcpy(tds__GetScopesResponse->Scopes[2].ScopeItem, "onvif://www.onvif.org/type/ptz");
	tds__GetScopesResponse->Scopes[3].ScopeDef = tt__ScopeDefinition__Fixed;
	tds__GetScopesResponse->Scopes[3].ScopeItem = (char *)soap_malloc(soap,LEN_64_SIZE);
	strcpy(tds__GetScopesResponse->Scopes[3].ScopeItem, "onvif://www.onvif.org/hardware/IPCAM");
	tds__GetScopesResponse->Scopes[4].ScopeDef = tt__ScopeDefinition__Configurable;
	tds__GetScopesResponse->Scopes[4].ScopeItem = (char *)soap_malloc(soap,LEN_64_SIZE);

	int dwchannelnamelen = sizeof(csChannelName);
	soap->pConfig->CodeConvert("GB2312", "UTF-8", osd_info.csChannelName, strlen(osd_info.csChannelName), csChannelName, (size_t *)&dwchannelnamelen);
	printf("#### %s %d, %s-%s, %d-%d\n",
				__FUNCTION__, __LINE__, osd_info.csChannelName, csChannelName, strlen(osd_info.csChannelName), strlen(csChannelName));
	sprintf(tds__GetScopesResponse->Scopes[4].ScopeItem, "onvif://www.onvif.org/name/%s", csChannelName);

	tds__GetScopesResponse->Scopes[5].ScopeItem = (char *)soap_malloc(soap,LEN_64_SIZE);
	strcpy(tds__GetScopesResponse->Scopes[5].ScopeItem, "onvif://www.onvif.org/Profile/Streaming");
	tds__GetScopesResponse->Scopes[5].ScopeDef = tt__ScopeDefinition__Fixed;

	
	QMAPI_SYSCFG_ONVIFTESTINFO info;
	memset(&info, 0, sizeof(info));
	info.dwSize = sizeof(info);
	
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ONVIFTESTINFO, 0, &info, sizeof(info));
	
	int k = 5;
	if (info.szDeviceScopes1[0])
	{
		//printf("scope is %s\n", info.szDeviceScopes1);
		tds__GetScopesResponse->Scopes[++k].ScopeItem = (char *)soap_malloc(soap,LEN_128_SIZE);
		sprintf(tds__GetScopesResponse->Scopes[k].ScopeItem, "onvif://www.onvif.org/%s", info.szDeviceScopes1);
		tds__GetScopesResponse->Scopes[k].ScopeDef = tt__ScopeDefinition__Configurable;

		++tds__GetScopesResponse->__sizeScopes;
	}

	if (info.szDeviceScopes2[0])
	{
		//printf("scope is %s\n", info.szDeviceScopes1);
		tds__GetScopesResponse->Scopes[++k].ScopeItem = (char *)soap_malloc(soap,LEN_128_SIZE);
		sprintf(tds__GetScopesResponse->Scopes[k].ScopeItem, "onvif://www.onvif.org/%s", info.szDeviceScopes2);
		tds__GetScopesResponse->Scopes[k].ScopeDef = tt__ScopeDefinition__Configurable;

		++tds__GetScopesResponse->__sizeScopes;
	}
	
	if (info.szDeviceScopes3[0])
	{
		//printf("scope is %s\n", info.szDeviceScopes1);
		tds__GetScopesResponse->Scopes[++k].ScopeItem = (char *)soap_malloc(soap,LEN_128_SIZE);
		sprintf(tds__GetScopesResponse->Scopes[k].ScopeItem, "onvif://www.onvif.org/%s", info.szDeviceScopes3);
		tds__GetScopesResponse->Scopes[k].ScopeDef = tt__ScopeDefinition__Configurable;

		++tds__GetScopesResponse->__sizeScopes;
	}
	
	return SOAP_OK;
}
#if 0
static char *strparser(char *str)
{
	int i = 0;
	int j = 0;
	char *retstr = malloc(sizeof(char) * INFO_LENGTH);
	while(i < 3)
	{
		if(*str == '/')
		{
			i++;
		}
		str++;
	}
	while(*str != '/' && *str != '\0')
	{
		*(retstr + j) = *str;
		j++;
		str++;
	}
	*(retstr + j) = '\0';
	if(((strcmp(retstr, "type") == 0)) || (strcmp(retstr, "name") == 0) || (strcmp(retstr, "location") == 0) || (strcmp(retstr, "hardware") == 0))
	{
		return retstr;
	}
	else
	{
		return NULL;
	}
}
#endif
int  __tds__AddScopes(struct soap *soap, struct _tds__AddScopes *tds__AddScopes, struct _tds__AddScopesResponse *tds__AddScopesResponse)
{ 
	if (!tds__AddScopes->__sizeScopeItem)
	{
		return SOAP_OK;
	}

	QMAPI_SYSCFG_ONVIFTESTINFO info;
	memset(&info, 0, sizeof(info));
	info.dwSize = sizeof(info);
	
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ONVIFTESTINFO, 0, &info, sizeof(info));
	
	int i;
	int err = 0;
	const char *p = NULL;
	for (i = 0; i < tds__AddScopes->__sizeScopeItem; i++)
	{
		err = 1;
		if (!strncmp(tds__AddScopes->ScopeItem[i], "onvif://www.onvif.org/", strlen("onvif://www.onvif.org/")))
		{
			p = tds__AddScopes->ScopeItem[i] + strlen("onvif://www.onvif.org/");
			if (strncmp(p, "type/", strlen("type/")) && strncmp(p, "hardware/", strlen("hardware/"))
				&& strncmp(p, "Profile/", strlen("Profile/")) && strncmp(p, "name", strlen("name")))
			{
				if (!strcmp(p, info.szDeviceScopes1) || !strcmp(p, info.szDeviceScopes2) || !strcmp(p, info.szDeviceScopes3))
				{
					continue;
				}

				if (!info.szDeviceScopes1[0])
				{
					strncpy(info.szDeviceScopes1, p, 64);
					err = 0;
				}
				else if (!info.szDeviceScopes2[0])
				{
					strncpy(info.szDeviceScopes2, p, 64);
					err = 0;
				}
				else if (!info.szDeviceScopes3[0])
				{
					strncpy(info.szDeviceScopes3, p, 64);
					err = 0;
				}
			}
		}

		if (err == 1)
		{
			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:OperationProhibited",
				"ter:ScopeOverwrite", "Trying overwriting permanent device scope setting");			
		}
	}

	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_ONVIFTESTINFO, 0, &info, sizeof(info));
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, 0, NULL, 0);

	soap->pConfig->system_reboot_flag = DISCOVERY_REBOOT;	
	return SOAP_OK;
}

int __tds__SetScopes(struct soap *soap, struct _tds__SetScopes *tds__SetScopes, struct _tds__SetScopesResponse *tds__SetScopesResponse)
{	
	//printf("#### %s %d, sizeScopes=%d\n", __FUNCTION__, __LINE__, tds__SetScopes->__sizeScopes);
	if (!tds__SetScopes->__sizeScopes)
	{
		return SOAP_OK;
	}

	QMAPI_SYSCFG_ONVIFTESTINFO info;
	memset(&info, 0, sizeof(info));
	info.dwSize = sizeof(info);
	
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ONVIFTESTINFO, 0, &info, sizeof(info));

	QMAPI_NET_CHANNEL_OSDINFO osd_info;
	memset(&osd_info, 0, sizeof(QMAPI_NET_CHANNEL_OSDINFO));

	if(QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_OSDCFG, 0, &osd_info, sizeof(QMAPI_NET_CHANNEL_OSDINFO)))
	{
		printf("get osdinfo failed!\n");
		return SOAP_ERR;
	}

	int bSetOsd = 0;
	
	int i;
	int err = 0;
	int n = 0;
	char *ptr = NULL;
	for (i = 0; i < tds__SetScopes->__sizeScopes; i++)
	{
		err = 1;
		if (!strncmp(tds__SetScopes->Scopes[i], "onvif://www.onvif.org/", strlen("onvif://www.onvif.org/")))
		{
			ptr = tds__SetScopes->Scopes[i] + strlen("onvif://www.onvif.org/");
			if (strncmp(ptr, "type/", strlen("type/")) && strncmp(ptr, "hardware/", strlen("hardware/"))
				&& strncmp(ptr, "Profile/", strlen("Profile/")))
    		{
    			err = 0;
    			if (!strncmp(ptr, "name", strlen("name")))
    			{
    				char * csChannelName = ptr + strlen("name");
    				if (*csChannelName == '/')
    				{
    					++csChannelName;
    				}
    
    				if (*csChannelName)
    				{
    					char temp[64];
                        int dwchannelnamelen = sizeof(temp);
    					soap->pConfig->CodeConvert("UTF-8", "GB2312", csChannelName, strlen(csChannelName), temp, (size_t *)&dwchannelnamelen);
    					strcpy(osd_info.csChannelName, temp);
    				}
    				else
    				{
    					osd_info.csChannelName[0] = '\0';
    				}
    				
    				osd_info.byShowChanName = QMAPI_TRUE;
    				bSetOsd = 1;
    			}
    			else
    			{
    				printf("set scopes:%s\n", tds__SetScopes->Scopes[i]);
    				switch (n)
    				{
    				case 0:
    					strncpy(info.szDeviceScopes1, ptr, 64);
    					break;
    				case 1:
    					strncpy(info.szDeviceScopes2, ptr, 64); 
    					break;
    				case 2:
    					strncpy(info.szDeviceScopes3, ptr, 64); 
    					break;
    				default:
    					err = 1;
    					break;
    				}
					
    				++n;
    			}
    		}
		}
		
		if (err == 1)
		{
			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:OperationProhibited",
				"ter:ScopeOverwrite", "Trying overwriting permanent device scope setting");			
		}
	}
	
	//printf("*******************************************size = %d\n", tds__SetScopes->__sizeScopes);
	if (bSetOsd)
	{
		QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_OSDCFG, 0, &osd_info, sizeof(QMAPI_NET_CHANNEL_OSDINFO));	
	}
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_ONVIFTESTINFO, 0, &info, sizeof(info));
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, 0, NULL, 0);
	return SOAP_OK;
}

int __tds__RemoveScopes(struct soap *soap, struct _tds__RemoveScopes *tds__RemoveScopes, struct _tds__RemoveScopesResponse *tds__RemoveScopesResponse)
{
	if (!tds__RemoveScopes->__sizeScopeItem)
	{
		return SOAP_OK;
	}

	QMAPI_SYSCFG_ONVIFTESTINFO info;
	memset(&info, 0, sizeof(info));
	info.dwSize = sizeof(info);
	
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ONVIFTESTINFO, 0, &info, sizeof(info));
	
	int i;
	int err = 0;
	char *ptr = NULL;
	for (i = 0; i < tds__RemoveScopes->__sizeScopeItem; i++)
	{
		err = 1;
		if (!strncmp(tds__RemoveScopes->ScopeItem[i], "onvif://www.onvif.org/", strlen("onvif://www.onvif.org/")))
		{
			ptr = tds__RemoveScopes->ScopeItem[i] + strlen("onvif://www.onvif.org/");
			if (strncmp(ptr, "type/", strlen("type/")) && strncmp(ptr, "hardware/", strlen("hardware/"))
				&& strncmp(ptr, "Profile/", strlen("Profile/")) && strncmp(ptr, "name", strlen("name")))
			{
				if (info.szDeviceScopes1[0] && !strcmp(ptr, info.szDeviceScopes1))
				{
					info.szDeviceScopes1[0] = '\0';
					err = 0;
				}
				else if (info.szDeviceScopes2[0] && !strcmp(ptr, info.szDeviceScopes2))
				{
					info.szDeviceScopes2[0] = '\0';
					err = 0;
				}
				else if (info.szDeviceScopes3[0] && !strcmp(ptr, info.szDeviceScopes3))
				{
					info.szDeviceScopes3[0] = '\0';
					err = 0;
				}
			}
		}

		if (err == 1)
		{
			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:OperationProhibited",
				"ter:ScopeOverwrite", "Trying overwriting permanent device scope setting");			
		}
	}

	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_ONVIFTESTINFO, 0, &info, sizeof(info));
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, 0, NULL, 0);

	soap->pConfig->system_reboot_flag = DISCOVERY_REBOOT;	
	return SOAP_OK;
}

int  __tds__GetDiscoveryMode(struct soap *soap, struct _tds__GetDiscoveryMode *tds__GetDiscoveryMode, struct _tds__GetDiscoveryModeResponse *tds__GetDiscoveryModeResponse)
{
	QMAPI_SYSCFG_ONVIFTESTINFO info;
	memset(&info, 0, sizeof(info));
	info.dwSize = sizeof(info);
	
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ONVIFTESTINFO, 0, &info, sizeof(info));

	tds__GetDiscoveryModeResponse->DiscoveryMode = !info.dwDiscoveryMode;
	return SOAP_OK;
}

int  __tds__SetDiscoveryMode(struct soap *soap, struct _tds__SetDiscoveryMode *tds__SetDiscoveryMode, struct _tds__SetDiscoveryModeResponse *tds__SetDiscoveryModeResponse)
{
	QMAPI_SYSCFG_ONVIFTESTINFO info;
	memset(&info, 0, sizeof(info));
	info.dwSize = sizeof(info);
	
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_ONVIFTESTINFO, 0, &info, sizeof(info));

	info.dwDiscoveryMode = !tds__SetDiscoveryMode->DiscoveryMode;
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_ONVIFTESTINFO, 0, &info, sizeof(info));
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, 0, 0, 0);

	soap->pConfig->discoveryMode = info.dwDiscoveryMode;	
	return SOAP_OK;	
}

int  __tds__GetRemoteDiscoveryMode(struct soap *soap, struct _tds__GetRemoteDiscoveryMode *tds__GetRemoteDiscoveryMode, struct _tds__GetRemoteDiscoveryModeResponse *tds__GetRemoteDiscoveryModeResponse)
{ 
	ONVIF_DBP("");

	return SOAP_OK; 
}
int  __tds__SetRemoteDiscoveryMode(struct soap *soap, struct _tds__SetRemoteDiscoveryMode *tds__SetRemoteDiscoveryMode, struct _tds__SetRemoteDiscoveryModeResponse *tds__SetRemoteDiscoveryModeResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetDPAddresses(struct soap *soap, struct _tds__GetDPAddresses *tds__GetDPAddresses, struct _tds__GetDPAddressesResponse *tds__GetDPAddressesResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetEndpointReference(struct soap *soap, struct _tds__GetEndpointReference *tds__GetEndpointReference, struct _tds__GetEndpointReferenceResponse *tds__GetEndpointReferenceResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetRemoteUser(struct soap *soap, struct _tds__GetRemoteUser *tds__GetRemoteUser, struct _tds__GetRemoteUserResponse *tds__GetRemoteUserResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__SetRemoteUser(struct soap *soap, struct _tds__SetRemoteUser *tds__SetRemoteUser, struct _tds__SetRemoteUserResponse *tds__SetRemoteUserResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int __tds__GetUsers(struct soap *soap, struct _tds__GetUsers *tds__GetUsers, struct _tds__GetUsersResponse *tds__GetUsersResponse)
{
	ONVIF_DBP("");

	int ret;
	QMAPI_NET_USER_INFO     user_info;
	memset(&user_info,0,sizeof(QMAPI_NET_USER_INFO));
	//默认只获取管理员密码
	ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_USERCFG,0,&user_info,sizeof(QMAPI_NET_USER_INFO));
	if(ret)
	{
		return SOAP_ERR;
	}

	tds__GetUsersResponse->__sizeUser = 1;
	tds__GetUsersResponse->User = soap_malloc_v2(soap,sizeof(struct tt__User) * 4);
	tds__GetUsersResponse->User[0].Username = soap_malloc(soap, LEN_64_SIZE);
	strcpy(tds__GetUsersResponse->User[0].Username, user_info.csUserName);
	tds__GetUsersResponse->User[0].UserLevel = tt__UserLevel__Administrator;

	int i;
	for (i = 0; i < 3; i ++)
	{
		if (soap->pConfig->onvif_username[i][0])
		{			
			tds__GetUsersResponse->User[tds__GetUsersResponse->__sizeUser].Username = soap_malloc(soap, LEN_64_SIZE);
			strcpy(tds__GetUsersResponse->User[tds__GetUsersResponse->__sizeUser].Username, soap->pConfig->onvif_username[i]);
			tds__GetUsersResponse->User[tds__GetUsersResponse->__sizeUser].UserLevel = (enum tt__UserLevel)soap->pConfig->onvif_userLevel[i];
			++tds__GetUsersResponse->__sizeUser;
		}
	}
	
	return SOAP_OK;
}

int __tds__CreateUsers(struct soap *soap, struct _tds__CreateUsers *tds__CreateUsers, struct _tds__CreateUsersResponse *tds__CreateUsersResponse)
{
	int i, j;

	QMAPI_NET_USER_INFO user_info;
	memset(&user_info,0,sizeof(QMAPI_NET_USER_INFO));
	//默认只获取管理员密码
	QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_USERCFG, 0, &user_info, sizeof(QMAPI_NET_USER_INFO));
	
	for (i = 0; i < tds__CreateUsers->__sizeUser && i < 3; i++)
	{
		if (!strcmp(user_info.csUserName, tds__CreateUsers->User[i].Username))
		{
			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:OperationProhibited",
				"ter:UsernameClash", "username is existing.");		
		}
		
		for (j = 0; j < 3; j++)
		{
			if (!strcmp(soap->pConfig->onvif_username[j], tds__CreateUsers->User[i].Username))
			{
				return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:OperationProhibited",
					"ter:UsernameClash", "username is existing.");
			}
		}
	}
	
	for (i = 0; i < tds__CreateUsers->__sizeUser && i < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (!soap->pConfig->onvif_username[j][0])
			{
				strncpy(soap->pConfig->onvif_username[j], tds__CreateUsers->User[i].Username, 32);
				strncpy(soap->pConfig->onvif_password[j], tds__CreateUsers->User[i].Password, 32);
				soap->pConfig->onvif_userLevel[j] = tds__CreateUsers->User[i].UserLevel;

				break;
			}
		}
	}

	return SOAP_OK;
}

int __tds__DeleteUsers(struct soap *soap, struct _tds__DeleteUsers *tds__DeleteUsers, struct _tds__DeleteUsersResponse *tds__DeleteUsersResponse)
{
	int map_index[3] = {-1, -1, -1};
	
	int i, j;
	int index = 0;
	for (i = 0; i < tds__DeleteUsers->__sizeUsername && index < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (!strcmp(tds__DeleteUsers->Username[i], soap->pConfig->onvif_username[j]))
			{
				map_index[index++] = j;
				break;
			}
		}
	}

	if (index == tds__DeleteUsers->__sizeUsername)
	{
		for (i = 0; i < 3; i++)
		{
			if (map_index[i] != -1)
			{
				soap->pConfig->onvif_username[map_index[i]][0] = '\0';
			}
		}
	}
	else
	{
		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
			"ter:UsernameMissing", "username is not existing.");
	}
	
	return SOAP_OK;
}


int __tds__SetUser(struct soap *soap, struct _tds__SetUser *tds__SetUser, struct _tds__SetUserResponse *tds__SetUserResponse)
{
	int map_index[3] = {-1, -1, -1};
	
	int i, j;
	int index = 0;
	for (i = 0; i < tds__SetUser->__sizeUser && index < 3; i++)
	{
		for (j = 0; j < 3; j++)
		{
			if (!strcmp(tds__SetUser->User[i].Username, soap->pConfig->onvif_username[j]))
			{
				map_index[index++] = j;
				break;
			}
		}
	}

	if (index == tds__SetUser->__sizeUser)
	{
		for (i = 0; i < 3; i++)
		{
			if (map_index[i] != -1)
			{
				if (tds__SetUser->User[i].Password)
				{
					strncpy(soap->pConfig->onvif_password[map_index[i]], tds__SetUser->User[i].Password, 32);
				}

				//printf("Set user:%d\n", tds__SetUser->User[i].UserLevel);
				soap->pConfig->onvif_userLevel[map_index[i]] = tds__SetUser->User[i].UserLevel;
			}
		}
	}
	else
	{
		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
			"ter:UsernameMissing", "username is not existing.");
	}

	return SOAP_OK;
}

int  __tds__GetWsdlUrl(struct soap *soap, 
struct _tds__GetWsdlUrl *tds__GetWsdlUrl, 
struct _tds__GetWsdlUrlResponse *tds__GetWsdlUrlResponse)
{
	tds__GetWsdlUrlResponse->WsdlUrl = (char*)soap_strdup(soap, "http://www.onvif.org/");

	return SOAP_OK; 
	
}

static void AddDeviceCapbilities(struct soap *soap, const char *ipaddr, int webport, 
	struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse)
{
	if (!tds__GetCapabilitiesResponse->Capabilities->Device)
	{
		tds__GetCapabilitiesResponse->Capabilities->Device = (struct tt__DeviceCapabilities *)soap_malloc_v2(soap, sizeof(struct tt__DeviceCapabilities));
		
		tds__GetCapabilitiesResponse->Capabilities->Device->XAddr = (char *)soap_malloc(soap, LEN_64_SIZE);
		if(webport != 80)
			sprintf(tds__GetCapabilitiesResponse->Capabilities->Device->XAddr,
				"http://%s:%d/onvif/device_service",ipaddr,webport);
		else
			sprintf(tds__GetCapabilitiesResponse->Capabilities->Device->XAddr,
				"http://%s/onvif/device_service",ipaddr);
		
		tds__GetCapabilitiesResponse->Capabilities->Device->System = (struct tt__SystemCapabilities *)soap_malloc_v2(soap, sizeof(struct tt__SystemCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->Device->System->DiscoveryBye = xsd__boolean__true_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->__sizeSupportedVersions = 5;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions = (struct tt__OnvifVersion*)soap_malloc(soap, 5 * sizeof(struct tt__OnvifVersion));
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions[0].Major = 2;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions[0].Minor = 40;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions[1].Major = 2;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions[1].Minor = 30;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions[2].Major = 2;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions[2].Minor = 20;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions[3].Major = 2;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions[3].Minor = 10;		
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions[4].Major = 2;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions[4].Minor = 0;

		tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension = (struct tt__SystemCapabilitiesExtension *)soap_malloc_v2(soap, sizeof(struct tt__SystemCapabilitiesExtension));
		tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpFirmwareUpgrade = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpFirmwareUpgrade = xsd__boolean__true_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSupportInformation = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSupportInformation = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSystemBackup = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSystemBackup = xsd__boolean__true_;
		tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSystemLogging = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
		*tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSystemLogging = xsd__boolean__true_;
		
		tds__GetCapabilitiesResponse->Capabilities->Device->IO = (struct tt__IOCapabilities *)soap_malloc_v2(soap, sizeof(struct tt__IOCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->Device->IO->InputConnectors = (int *)soap_malloc(soap, sizeof(int));
		/*报警输入个数*/
		*(tds__GetCapabilitiesResponse->Capabilities->Device->IO->InputConnectors) = 1; 
		tds__GetCapabilitiesResponse->Capabilities->Device->IO->RelayOutputs = (int *)soap_malloc(soap, sizeof(int));
		/*报警输出个数*/
		*(tds__GetCapabilitiesResponse->Capabilities->Device->IO->RelayOutputs) = 1;
		
		tds__GetCapabilitiesResponse->Capabilities->Device->Network = (struct tt__NetworkCapabilities*)soap_malloc_v2(soap, sizeof(struct tt__NetworkCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->DynDNS = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
		*(tds__GetCapabilitiesResponse->Capabilities->Device->Network->DynDNS) = xsd__boolean__true_;
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPFilter = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
		*(tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPFilter) = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->ZeroConfiguration = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
		*(tds__GetCapabilitiesResponse->Capabilities->Device->Network->ZeroConfiguration) = xsd__boolean__false_;
		tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPVersion6 = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
		*(tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPVersion6) = xsd__boolean__false_;
		
		tds__GetCapabilitiesResponse->Capabilities->Device->Security = (struct tt__SecurityCapabilities*)soap_malloc_v2(soap, sizeof(struct tt__SecurityCapabilities));
	}
}

static void AddAnalyticsCapbilities(struct soap *soap, const char *ipaddr, int webport, 
	struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse)
{
	if (!tds__GetCapabilitiesResponse->Capabilities->Analytics)
	{
		tds__GetCapabilitiesResponse->Capabilities->Analytics = (struct tt__AnalyticsCapabilities *)soap_malloc_v2(soap, sizeof(struct tt__AnalyticsCapabilities));
		
		tds__GetCapabilitiesResponse->Capabilities->Analytics->XAddr = (char *)soap_malloc(soap, LEN_64_SIZE);
		if(webport != 80)
			sprintf(tds__GetCapabilitiesResponse->Capabilities->Analytics->XAddr,
				"http://%s:%d/onvif/Analytics",ipaddr,webport);
		else
			sprintf(tds__GetCapabilitiesResponse->Capabilities->Analytics->XAddr,
				"http://%s/onvif/Analytics",ipaddr);
		tds__GetCapabilitiesResponse->Capabilities->Analytics->RuleSupport= xsd__boolean__true_;
		tds__GetCapabilitiesResponse->Capabilities->Analytics->AnalyticsModuleSupport = xsd__boolean__true_;
	}
}

static void AddMediaCapbilities(struct soap *soap, const char *ipaddr, int webport, 
	struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse)
{
	if (!tds__GetCapabilitiesResponse->Capabilities->Media)
	{
		tds__GetCapabilitiesResponse->Capabilities->Media = (struct tt__MediaCapabilities *)soap_malloc_v2(soap, sizeof(struct tt__MediaCapabilities));
		
		tds__GetCapabilitiesResponse->Capabilities->Media->XAddr = (char *)soap_malloc(soap, LEN_64_SIZE);
		if(webport != 80)
			sprintf(tds__GetCapabilitiesResponse->Capabilities->Media->XAddr, "http://%s:%d/onvif/Media",ipaddr,webport);
		else
			sprintf(tds__GetCapabilitiesResponse->Capabilities->Media->XAddr, "http://%s/onvif/Media",ipaddr);
		
		tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities = (struct tt__RealTimeStreamingCapabilities *)soap_malloc_v2(soap, sizeof(struct tt__RealTimeStreamingCapabilities));
		
		tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTPMulticast = (enum xsd__boolean *)
			soap_malloc(soap, sizeof(enum xsd__boolean));
		*(tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTPMulticast) = xsd__boolean__false_;	
		tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORETCP = (enum xsd__boolean *)
			soap_malloc(soap, sizeof(enum xsd__boolean));
		*(tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORETCP) = xsd__boolean__true_;	
		tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = (enum xsd__boolean *)
			soap_malloc(soap, sizeof(enum xsd__boolean));
		*(tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP) = xsd__boolean__true_;
	}
}

static void AddEventCapbilities(struct soap *soap, const char *ipaddr, int webport, 
	struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse)
{
	if (!tds__GetCapabilitiesResponse->Capabilities->Events)
	{
		tds__GetCapabilitiesResponse->Capabilities->Events = (struct tt__EventCapabilities *)soap_malloc_v2(soap, sizeof(struct tt__EventCapabilities));
		
		tds__GetCapabilitiesResponse->Capabilities->Events->XAddr = (char *)soap_malloc(soap, LEN_64_SIZE);
 		if(webport != 80)
			sprintf(tds__GetCapabilitiesResponse->Capabilities->Events->XAddr, "http://%s:%d/onvif/Events",ipaddr,webport);
		else
			sprintf(tds__GetCapabilitiesResponse->Capabilities->Events->XAddr, "http://%s/onvif/Events",ipaddr);
		
		tds__GetCapabilitiesResponse->Capabilities->Events->WSSubscriptionPolicySupport = xsd__boolean__true_;
		tds__GetCapabilitiesResponse->Capabilities->Events->WSPullPointSupport = xsd__boolean__true_;
		tds__GetCapabilitiesResponse->Capabilities->Events->WSPausableSubscriptionManagerInterfaceSupport = xsd__boolean__false_;
	}
}

static void AddImagingCapbilities(struct soap *soap, const char *ipaddr, int webport, 
	struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse)
{
	if (!tds__GetCapabilitiesResponse->Capabilities->Imaging)
	{
		tds__GetCapabilitiesResponse->Capabilities->Imaging = (struct tt__ImagingCapabilities *)soap_malloc_v2(soap, sizeof(struct tt__ImagingCapabilities));
		
		tds__GetCapabilitiesResponse->Capabilities->Imaging->XAddr = (char *)soap_malloc(soap, LEN_64_SIZE);
		if(webport != 80)
			sprintf(tds__GetCapabilitiesResponse->Capabilities->Imaging->XAddr, "http://%s:%d/onvif/Imaging",ipaddr,webport);
		else
			sprintf(tds__GetCapabilitiesResponse->Capabilities->Imaging->XAddr, "http://%s/onvif/Imaging",ipaddr);
	}
}

static void AddPtzCapbilities(struct soap *soap, const char *ipaddr, int webport, 
	struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse)
{
	if (!tds__GetCapabilitiesResponse->Capabilities->PTZ)
	{
		tds__GetCapabilitiesResponse->Capabilities->PTZ = (struct tt__PTZCapabilities*)soap_malloc_v2(soap, sizeof(struct tt__PTZCapabilities));
		tds__GetCapabilitiesResponse->Capabilities->PTZ->XAddr = (char *)soap_malloc(soap, LEN_64_SIZE);
		if(webport != 80)
			sprintf(tds__GetCapabilitiesResponse->Capabilities->PTZ->XAddr, "http://%s:%d/onvif/PTZ",ipaddr,webport);
		else
			sprintf(tds__GetCapabilitiesResponse->Capabilities->PTZ->XAddr, "http://%s/onvif/PTZ",ipaddr);
	}
}

int  __tds__GetCapabilities(struct soap *soap, 
 	struct _tds__GetCapabilities *tds__GetCapabilities, 
 	struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse)
{
	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));
	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);

	int webport = net_conf.wHttpPort;
	char *ipaddr = ipaddr = net_conf.stEtherNet[0].strIPAddr.csIpV4;

	if(TRUE == isWIFI)
		ipaddr = wifi_cfg.dwNetIpAddr.csIpV4;
	
	tds__GetCapabilitiesResponse->Capabilities = (struct tt__Capabilities *)soap_malloc_v2(soap,sizeof(struct tt__Capabilities));

	int size = tds__GetCapabilities->__sizeCategory;
	enum tt__CapabilityCategory *pCategory = tds__GetCapabilities->Category;
	enum tt__CapabilityCategory temp = tt__CapabilityCategory__All;
	if (!size)
	{
		size = 1;
		
		pCategory = &temp;
	}
	
	int i;
	for (i = 0; i < size; i++)
	{
		switch(pCategory[i])
		{
		case tt__CapabilityCategory__Device:
			AddDeviceCapbilities(soap, ipaddr, webport, tds__GetCapabilitiesResponse);
			break;
		case tt__CapabilityCategory__Analytics:
			AddAnalyticsCapbilities(soap, ipaddr, webport, tds__GetCapabilitiesResponse);
			break;
		case tt__CapabilityCategory__Media:
			AddMediaCapbilities(soap, ipaddr, webport, tds__GetCapabilitiesResponse);
			break;
		case tt__CapabilityCategory__Events:
			AddEventCapbilities(soap, ipaddr, webport, tds__GetCapabilitiesResponse);
			break;
		case tt__CapabilityCategory__Imaging:
			AddImagingCapbilities(soap, ipaddr, webport, tds__GetCapabilitiesResponse);
			break;
		case tt__CapabilityCategory__PTZ:
			AddPtzCapbilities(soap, ipaddr, webport, tds__GetCapabilitiesResponse);
			break;
		case tt__CapabilityCategory__All:
			AddAnalyticsCapbilities(soap, ipaddr, webport, tds__GetCapabilitiesResponse);	
			AddDeviceCapbilities(soap, ipaddr, webport, tds__GetCapabilitiesResponse);
			AddMediaCapbilities(soap, ipaddr, webport, tds__GetCapabilitiesResponse);
			AddEventCapbilities(soap, ipaddr, webport, tds__GetCapabilitiesResponse);
			AddImagingCapbilities(soap, ipaddr, webport, tds__GetCapabilitiesResponse);
			AddPtzCapbilities(soap, ipaddr, webport, tds__GetCapabilitiesResponse);
		#if 1	
			if (!tds__GetCapabilitiesResponse->Capabilities->Extension)
			{
				tds__GetCapabilitiesResponse->Capabilities->Extension = (struct tt__CapabilitiesExtension*)soap_malloc_v2(soap, sizeof(struct tt__CapabilitiesExtension));
				tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO = (struct tt__DeviceIOCapabilities*)soap_malloc_v2(soap, sizeof(struct tt__DeviceIOCapabilities));
				tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->XAddr = (char *)soap_malloc(soap, LEN_64_SIZE);
				if(webport != 80)
					sprintf(tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->XAddr,
						"http://%s:%d/onvif/DeviceIO",ipaddr,webport);
				else
					sprintf(tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->XAddr,
						"http://%s/onvif/DeviceIO",ipaddr);
					
				tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->VideoSources = soap->pConfig->g_channel_num;
				tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->VideoOutputs = 0;
				tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->AudioSources = 1;
				tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->AudioOutputs = 1;
				tds__GetCapabilitiesResponse->Capabilities->Extension->DeviceIO->RelayOutputs = 1;

#if 0
				tds__GetCapabilitiesResponse->Capabilities->Extension->Extensions = (struct tt__CapabilitiesExtension2*)
				soap_malloc_v2(soap, sizeof(struct tt__CapabilitiesExtension2));
		
				tds__GetCapabilitiesResponse->Capabilities->Extension->Extensions->__size = 1;
				tds__GetCapabilitiesResponse->Capabilities->Extension->Extensions->__any = (char **)soap_malloc(soap, sizeof(char *) * 1);
				tds__GetCapabilitiesResponse->Capabilities->Extension->Extensions->__any[0] = (char*)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);
				
				char *p = tds__GetCapabilitiesResponse->Capabilities->Extension->Extensions->__any[0];
				sprintf(p, "<tt:TelexCapabilities><tt:XAddr>http://%s:%d/onvif/telecom_service</tt:XAddr><tt:MotionDetectorSupport>true</tt:MotionDetectorSupport></tt:TelexCapabilities>", ipaddr, webport);
		#endif		
			}
		#endif
			break;
		default:
			tds__GetCapabilitiesResponse->Capabilities = NULL;
			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:ActionNotSupported",
				"ter:NoSuchService", "The matching rule specified is not supported");
			break;
		}
	}

	return SOAP_OK;
}


#define ALL  0
#define ANALYTICS 1
#define DEVICE 2
#define EVENTS 3
#define IMAGING 4
#define MEDIA 5
#define PANTILTZOOM 6

int __tds__SetDPAddresses(struct soap* soap, 
	struct _tds__SetDPAddresses *tds__SetDPAddresses, 
	struct _tds__SetDPAddressesResponse *tds__SetDPAddressesResponse)
{

	if (tds__SetDPAddresses->DPAddress) {
		if (tds__SetDPAddresses->DPAddress->Type == tt__NetworkHostType__IPv4) {
			//printf("DP ipv4 addr is %s\n",
			//	*(tds__SetDPAddresses->DPAddress->IPv4Address));
		}
	}

	return SOAP_OK;
}

int __tds__GetHostname(struct soap *soap, struct _tds__GetHostname *tds__GetHostname, struct _tds__GetHostnameResponse *tds__GetHostnameResponse)
{ 	 
	char hostname[LEN_64_SIZE] = {0};
	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));
	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);
	
	tds__GetHostnameResponse->HostnameInformation = soap_malloc_v2(soap,sizeof(struct tt__HostnameInformation));

	if(TRUE == isWIFI)
	{
		tds__GetHostnameResponse->HostnameInformation->FromDHCP = (enum xsd__boolean)wifi_cfg.bWifiEnable;
	}
	else
	{
		tds__GetHostnameResponse->HostnameInformation->FromDHCP = (enum xsd__boolean)net_conf.byEnableDHCP;
	}
	
	if ((gethostname(hostname, sizeof(hostname)) < 0) || (!hostname[0] || !strcmp(hostname, "(none)"))) 
	{
		//printf("Can't get hostname\n");
		tds__GetHostnameResponse->HostnameInformation->Name = soap_strdup(soap, "IPCAM"
);
	}
	else
	{
		tds__GetHostnameResponse->HostnameInformation->Name = soap_strdup(soap, hostname);
	}
	
	return SOAP_OK;

}

#if 0
static int checkhostname(char *hostname)
{

	while(*hostname != '\0')
	{
		if(*hostname=='_')return 1;
		hostname++;
	}
	return 0; //No error	
}
#endif
 
int __tds__SetHostname(struct soap *soap, struct _tds__SetHostname *tds__SetHostname, struct _tds__SetHostnameResponse *tds__SetHostnameResponse)
{
	if (tds__SetHostname->Name && check_hostname(tds__SetHostname->Name))
	{
		sethostname(tds__SetHostname->Name, strlen(tds__SetHostname->Name));
		return SOAP_OK;
	}

	return(send_soap_fault_msg(soap,400,
		"SOAP-ENV:Sender",
		"ter:InvalidArgVal",
		"ter:InvalidHostname",
		"The requested hostname cannot be accepted by the NVT."));	
	
}

static int get_dns_conf(char *file, void * net_conf)
{
	NET_CONF * netconf;
	netconf = (NET_CONF *)net_conf;
	char line_buf[32] = {0};
	FILE * fd;
	char *p = NULL;
	char *q = NULL;
	int i=0;

	if ( NULL == (fd=fopen(file,"r")) )
	{
		printf("open %s Error!",file);
		return 1;
	}

	while(NULL != fgets(line_buf,32,fd))
	{		
		if (((p = strtok(line_buf, " \r\n\t")) != (char *)NULL) && ((q = strtok(NULL, " \r\n\t")) != (char *)NULL))
		{
			 if(0 == strcmp(p,"nameserver"))
			{
				i++;
				 if(i==1)
					strncpy(netconf->dns0,q,16);
				 else if(i==2)
					strncpy(netconf->dns1,q,16);
			}
		}/* end of if */
	}/* end of while */
	fclose(fd);
	return 0;
}

int __tds__GetDNS(struct soap *soap, struct _tds__GetDNS *tds__GetDNS, struct _tds__GetDNSResponse *tds__GetDNSResponse)
{
	ONVIF_DBP("");
	int dhcp_enable;
	NET_CONF dns_conf;
	
	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&dns_conf, 0, sizeof(NET_CONF));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));

	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);
	if(TRUE == isWIFI)
		dhcp_enable = wifi_cfg.bWifiEnable;
	else
		dhcp_enable = net_conf.byEnableDHCP;

	if(get_dns_conf(DNSCFG_FIlE,&dns_conf))
	{
		//printf("get dns conf error\n");
		return SOAP_OK;
	}
		
	tds__GetDNSResponse->DNSInformation =soap_malloc_v2(soap,sizeof(struct tt__DNSInformation));

	//if(tempdnssearchdomainsize > 0)
	{
		tds__GetDNSResponse->DNSInformation->__sizeSearchDomain = 1;
		tds__GetDNSResponse->DNSInformation->SearchDomain = (char **)soap_malloc(soap, sizeof(char *));
		tds__GetDNSResponse->DNSInformation->SearchDomain[0] = (char*)soap_strdup(soap, soap->pConfig->domainname);
	}
	
	if(dhcp_enable)
	{
		tds__GetDNSResponse->DNSInformation->FromDHCP = xsd__boolean__true_;
		
		if(strlen(dns_conf.dns1)>0)
		{
			tds__GetDNSResponse->DNSInformation->__sizeDNSFromDHCP = 2;
			tds__GetDNSResponse->DNSInformation->DNSFromDHCP = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress)*2);
			tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
			tds__GetDNSResponse->DNSInformation->DNSFromDHCP[1].IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
			tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			tds__GetDNSResponse->DNSInformation->DNSFromDHCP[1].IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			//printf("***************dns is %s **************************\n", dns_conf.dns0);
			strcpy(tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].IPv4Address[0], dns_conf.dns0);
			strcpy(tds__GetDNSResponse->DNSInformation->DNSFromDHCP[1].IPv4Address[0], dns_conf.dns1);
			tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].Type = tt__IPType__IPv4;
			tds__GetDNSResponse->DNSInformation->DNSFromDHCP[1].Type = tt__IPType__IPv4;
		}
		else if(strlen(dns_conf.dns0)>0)
		{
			tds__GetDNSResponse->DNSInformation->__sizeDNSFromDHCP = 1;
			tds__GetDNSResponse->DNSInformation->DNSFromDHCP = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
			tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
			tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy(tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].IPv4Address[0], dns_conf.dns0);
			tds__GetDNSResponse->DNSInformation->DNSFromDHCP[0].Type = tt__IPType__IPv4;
		}
		else
			tds__GetDNSResponse->DNSInformation->__sizeDNSFromDHCP = 0;

	}
	else
	{
		tds__GetDNSResponse->DNSInformation->FromDHCP = xsd__boolean__false_;
		tds__GetDNSResponse->DNSInformation->__sizeDNSManual = 1;
		
		if(strlen(net_conf.stDnsServer2IpAddr.csIpV4)>0)
		{
			tds__GetDNSResponse->DNSInformation->DNSManual = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress)*2);
		
			tds__GetDNSResponse->DNSInformation->__sizeDNSManual = 2;

			tds__GetDNSResponse->DNSInformation->DNSManual[0].IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
			tds__GetDNSResponse->DNSInformation->DNSManual[1].IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
			tds__GetDNSResponse->DNSInformation->DNSManual[0].IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			tds__GetDNSResponse->DNSInformation->DNSManual[1].IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy(tds__GetDNSResponse->DNSInformation->DNSManual[0].IPv4Address[0], net_conf.stDnsServer1IpAddr.csIpV4);
			strcpy(tds__GetDNSResponse->DNSInformation->DNSManual[1].IPv4Address[0], net_conf.stDnsServer2IpAddr.csIpV4);
			tds__GetDNSResponse->DNSInformation->DNSManual[0].Type = tt__IPType__IPv4;
			tds__GetDNSResponse->DNSInformation->DNSManual[1].Type = tt__IPType__IPv4;
		}
		else if(strlen(net_conf.stDnsServer1IpAddr.csIpV4)>0)
		{
			tds__GetDNSResponse->DNSInformation->DNSManual = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
		
			tds__GetDNSResponse->DNSInformation->__sizeDNSManual = 1;

			tds__GetDNSResponse->DNSInformation->DNSManual[0].IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
			tds__GetDNSResponse->DNSInformation->DNSManual[0].IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
			strcpy(tds__GetDNSResponse->DNSInformation->DNSManual[0].IPv4Address[0], net_conf.stDnsServer1IpAddr.csIpV4);
			tds__GetDNSResponse->DNSInformation->DNSManual[0].Type = tt__IPType__IPv4;
		}
		else
			tds__GetDNSResponse->DNSInformation->__sizeDNSManual = 0;
	}

	return SOAP_OK;	 
}

 
int __tds__SetDNS(struct soap *soap, struct _tds__SetDNS *tds__SetDNS, struct _tds__SetDNSResponse *tds__SetDNSResponse)
{
	ONVIF_DBP("");

	int change_flag=0;
	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));

	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);
	
	change_flag=0;

	if (tds__SetDNS->SearchDomain)
	{
		if ( *tds__SetDNS->SearchDomain )
		{
			strncpy(soap->pConfig->domainname, *tds__SetDNS->SearchDomain, 20 - 1);
			soap->pConfig->domainname[19] = '\0';
		}	
	}

	if(tds__SetDNS->DNSManual )
	{
		if(tds__SetDNS->DNSManual->Type == tt__IPType__IPv4)
		{
			if(check_ipaddress(*(tds__SetDNS->DNSManual->IPv4Address))<0)
			{
				return(send_soap_fault_msg(soap,400,
					"SOAP-ENV:Sender",
					"ter:InvalidArgVal",
					"ter:InvalidIPv4Address",
					"The suggested IPv4 address is invalid."));
			}
			else
			{
				if(TRUE == isWIFI)
				{
					if(tds__SetDNS->__sizeDNSManual>0)
						strncpy(wifi_cfg.dwDNSServer.csIpV4,*(tds__SetDNS->DNSManual->IPv4Address),LEN_16_SIZE);
				}
				else
				{
					if(tds__SetDNS->__sizeDNSManual>0)
						strncpy(net_conf.stDnsServer1IpAddr.csIpV4,*(tds__SetDNS->DNSManual->IPv4Address),LEN_16_SIZE);
					
	                if(tds__SetDNS->__sizeDNSManual>1)
					{
						strncpy(net_conf.stDnsServer2IpAddr.csIpV4, *((tds__SetDNS->DNSManual+1)->IPv4Address),LEN_16_SIZE);
					}
					else
						memset(net_conf.stDnsServer2IpAddr.csIpV4,0,LEN_16_SIZE);
				}
				change_flag=1;
			}
		}
	}
	 
	if(change_flag)
	{
		if(TRUE == isWIFI)
			QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_WIFICFG, 0, &wifi_cfg, sizeof(QMAPI_NET_WIFI_CONFIG));
		else
		{
			if(set_network_cfg(&net_conf)<0)
			{
				//printf("set eth0 conf error\n");
				return SOAP_ERR;
			}
		}
	}
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, 0, 0, 0);

	return SOAP_OK;
}

int __tds__GetNTP(struct soap *soap, struct _tds__GetNTP *tds__GetNTP, struct _tds__GetNTPResponse *tds__GetNTPResponse)
{

	QMAPI_NET_NTP_CFG ntp_info;
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NTPCFG, 0, &ntp_info, sizeof(QMAPI_NET_NTP_CFG));

	tds__GetNTPResponse->NTPInformation = (struct tt__NTPInformation *)soap_malloc_v2(soap, sizeof(struct tt__NTPInformation));

	struct tt__NetworkHost *pInfo = (struct tt__NetworkHost *)soap_malloc_v2(soap, sizeof(struct tt__NetworkHost));
	pInfo->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));

    /* RM#2291: used sys_fun_gethostbyname, henry.li    2017/03/01*/
	//struct hostent *p = gethostbyname((const char *)ntp_info.csNTPServer);
	struct hostent *p = sys_fun_gethostbyname((const char *)ntp_info.csNTPServer);
	if (p != NULL)
	{
		char ipaddr[20] = "";
		inet_ntop(AF_INET, p->h_addr_list[0], ipaddr, 20);
		*pInfo->IPv4Address = soap_strdup(soap, ipaddr);
	}
	else
	{
		*pInfo->IPv4Address = soap_strdup(soap, "64.4.10.33");
	}
		
	if (soap->pConfig->isNTPFromDHCP)
	{
		tds__GetNTPResponse->NTPInformation->FromDHCP = xsd__boolean__true_;
		tds__GetNTPResponse->NTPInformation->__sizeNTPFromDHCP = 1;
		tds__GetNTPResponse->NTPInformation->NTPFromDHCP = pInfo;
	}
	else
	{
		tds__GetNTPResponse->NTPInformation->__sizeNTPManual = 1;
		tds__GetNTPResponse->NTPInformation->NTPManual = pInfo;
	}
	
	return SOAP_OK;
	
}
#if 0
static int isValidIp4(char *str) 
{
	int segs = 0;   /* Segment count. */     
	int chcnt = 0;  /* Character count within segment. */     
	int accum = 0;  /* Accumulator for segment. */      
	/* Catch NULL pointer. */      
	if (str == NULL) return 0;      
	/* Process every character in string. */      
	while (*str != '\0') 
	{         
		/* Segment changeover. */          
		if (*str == '.') 
		{             
			/* Must have some digits in segment. */              
			if (chcnt == 0) return 0;              
			/* Limit number of segments. */              
			if (++segs == 4) return 0;              
			/* Reset segment values and restart loop. */              
			chcnt = accum = 0;             
			str++;             
			continue;         
		}  

		/* Check numeric. */          
		if ((*str < '0') || (*str > '9')) return 0;
		/* Accumulate and check segment. */          
		if ((accum = accum * 10 + *str - '0') > 255) return 0;
		/* Advance other segment specific stuff and continue loop. */          
		chcnt++;         
		str++;     
	}      
	/* Check enough segments and enough characters in last segment. */      
	if (segs != 3) return 0;      
	if (chcnt == 0) return 0;      
	/* Address okay. */      
	return 1; 
}
#endif
int __tds__SetNTP(struct soap *soap, struct _tds__SetNTP *tds__SetNTP, struct _tds__SetNTPResponse *tds__SetNTPResponse)
{
	if (tds__SetNTP->FromDHCP)
	{
		soap->pConfig->isNTPFromDHCP = 1;
		return SOAP_OK;
	}

	soap->pConfig->isNTPFromDHCP = 0;
	if (tds__SetNTP->NTPManual)
	{	
		struct in_addr s; // IPv4地址结构体	
		inet_pton(AF_INET, tds__SetNTP->NTPManual->IPv4Address[0], &s);
#if 0
		if (tds__SetNTP->FromDHCP || !tds__SetNTP->NTPManual || tds__SetNTP->NTPManual->IPv6Address || !tds__SetNTP->NTPManual->IPv4Address
			|| !isValidIp4(tds__SetNTP->NTPManual->IPv4Address[0]))
		{
			send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal", "ter:InvalidIPv4Address",
				"the matching rule specified is not supported");
			return SOAP_FAULT;
		}
#endif
		QMAPI_NET_NTP_CFG ntp_info;
		QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NTPCFG, 0, &ntp_info, sizeof(ntp_info));
		strcpy(ntp_info.csNTPServer, (const char *)tds__SetNTP->NTPManual->IPv4Address[0]);
		QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_NTPCFG, 0, &ntp_info, sizeof(ntp_info));
		
		QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, 0, 0, 0);
	}
	
	return SOAP_OK;	
}


int __tds__GetDynamicDNS(struct soap *soap, struct _tds__GetDynamicDNS *tds__GetDynamicDNS, struct _tds__GetDynamicDNSResponse *tds__GetDynamicDNSResponse)
{
	ONVIF_DBP("");

	send_soap_fault_msg(soap,400,
		"SOAP-ENV:Sender",
		"ter:InvalidArgVal",
		"ter:DynamicDNSNotSupported",
		"DynamicDNSNotSupported");

	return SOAP_FAULT;  

 
}
 
int __tds__SetDynamicDNS(struct soap *soap, struct _tds__SetDynamicDNS *tds__SetDynamicDNS, struct _tds__SetDynamicDNSResponse *tds__SetDynamicDNSResponse)
{	

	ONVIF_DBP("");
	
	send_soap_fault_msg(soap,400,
		"SOAP-ENV:Sender",
		"ter:InvalidArgVal",
		"ter:DynamicDNSNotSupported",
		"DynamicDNSNotSupported");

	return SOAP_FAULT;  
}


#if 0
int ReadDhcpEnable( )
{
	int enable = 0;
	char *filename = "/mnt/nand/dhcpcd.conf";
	FILE *fp = NULL;
	if ((fp = fopen(filename, "r")) == NULL) {			
		if ((fp = fopen(filename, "w")) == NULL) {
			ONVIF_DBP("Can't open config file\n");
			return 0;
		}

		fwrite(&enable, sizeof(enable), 1, fp);				
		fclose(fp);			
		return enable;			
	}

	fread(&enable, sizeof(enable), 1, fp);			
	fclose(fp);	

	return enable;		
	
		
}

int WriteHDCPEnable( int enable )
{

	char *filename = "/mnt/nand/dhcpcd.conf";
	FILE *fp = NULL;
	if ((fp = fopen(filename, "r+")) == NULL) {			
		if ((fp = fopen(filename, "w")) == NULL) {				
			ONVIF_DBP("Can't open config file\n");
			return 0;
		}					
	}
		
	fwrite(&enable, sizeof(enable), 1, fp);							
	fclose(fp);		

	//ReadDhcpEnable();

	return enable;

		
}
#endif

static int CalcPrfixLength(const char *ipaddr)
{
	int maskAddr[4] = {0};
	sscanf(ipaddr, "%d.%d.%d.%d", &maskAddr[0], &maskAddr[1], &maskAddr[2], &maskAddr[3]);

	int len = 0;
	unsigned char temp;
	int i, j;
	for (i = 0; i < 4; i++)
	{
		temp = maskAddr[i];
		for (j = 7; j >= 0; j--)
		{
			if (!(temp&(1<<j)))
			{
				//printf("len = %d\n", len);
				return len;
			}

			++len;
		}
	}

	//printf("len = %d\n", len);
	return len;
}

int  __tds__GetNetworkInterfaces(struct soap *soap, 
	struct _tds__GetNetworkInterfaces *tds__GetNetworkInterfaces, 
	struct _tds__GetNetworkInterfacesResponse *tds__GetNetworkInterfacesResponse)
{ 
	char hostip[24];
	char token[6]={0};
	int dhcp_enable;

	memset(hostip,0,sizeof(hostip));
	//get_ip_addr(NET_DEVICE, hostip);
	if(get_ip_addr(ETH_NAME, hostip))
	{
		if(get_ip_addr(WIFI_NAME, hostip))
		{
			if(get_ip_addr("ppp0", hostip) == 0)
				strcpy(token,"ppp0");
		}
		else
		{
			get_ip_addr(WIFI_NAME, hostip);
			strcpy(token, WIFI_NAME);
		}
	}else
	{
		strcpy(token, ETH_NAME);
	}

	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));

	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);
	if(TRUE == isWIFI)
		dhcp_enable = wifi_cfg.bWifiEnable;
	else
		dhcp_enable = net_conf.byEnableDHCP;


	tds__GetNetworkInterfacesResponse->__sizeNetworkInterfaces = 1;
	tds__GetNetworkInterfacesResponse->NetworkInterfaces = (struct tt__NetworkInterface *)soap_malloc_v2(soap,sizeof(struct tt__NetworkInterface));

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->token = (char *)soap_malloc(soap,6);
	strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->token, token);

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Enabled = xsd__boolean__true_;

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info = (struct tt__NetworkInterfaceInfo *)soap_malloc_v2(soap,sizeof(struct tt__NetworkInterfaceInfo));

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->Name =  (char *)soap_malloc(soap,24);
	strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->Name, token);

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->HwAddress =  (char *)soap_malloc(soap,24);

	if(TRUE == isWIFI)
	{
		int sockfd;		
		struct ifreq ifr;
		unsigned char buff[6];
		
		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
		{		
			return -1;
		}
		
		strcpy(ifr.ifr_name, WIFI_NAME);
		if(ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1)
			printf("hwaddr error.\n");
		close(sockfd);
		
		memcpy(buff, ifr.ifr_hwaddr.sa_data, sizeof(buff));
		printf("HWaddr: %02X:%02X:%02X:%02X:%02X:%02X\n", buff[0], buff[1], buff[2], buff[3], buff[4], buff[5]);

		memcpy(net_conf.stEtherNet[1].byMACAddr, buff, 6);	//使用第二个网卡来保存无线网卡的MAC地址
		
		sprintf(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->HwAddress, "%02X:%02X:%02X:%02X:%02X:%02X",
									net_conf.stEtherNet[1].byMACAddr[0], net_conf.stEtherNet[1].byMACAddr[1],
									net_conf.stEtherNet[1].byMACAddr[2], net_conf.stEtherNet[1].byMACAddr[3],
									net_conf.stEtherNet[1].byMACAddr[4], net_conf.stEtherNet[1].byMACAddr[5]);
	}
	else
	{
		sprintf(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->HwAddress, "%02X:%02X:%02X:%02X:%02X:%02X",
									net_conf.stEtherNet[0].byMACAddr[0], net_conf.stEtherNet[0].byMACAddr[1],
									net_conf.stEtherNet[0].byMACAddr[2], net_conf.stEtherNet[0].byMACAddr[3],
									net_conf.stEtherNet[0].byMACAddr[4], net_conf.stEtherNet[0].byMACAddr[5]);
	}
	
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->MTU =  (int *)soap_malloc(soap,sizeof(int));
	*(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->MTU) = net_conf.stEtherNet[0].wMTU;

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link = (struct tt__NetworkInterfaceLink *)soap_malloc_v2(soap,sizeof(struct tt__NetworkInterfaceLink));

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->InterfaceType = 6;
	
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->AdminSettings = (struct tt__NetworkInterfaceConnectionSetting *)soap_malloc_v2(soap,sizeof(struct tt__NetworkInterfaceConnectionSetting));

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->AdminSettings->AutoNegotiation = xsd__boolean__true_;
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->AdminSettings->Speed= 100;
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->AdminSettings->Duplex= tt__Duplex__Full;

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->OperSettings = (struct tt__NetworkInterfaceConnectionSetting *)soap_malloc_v2(soap,sizeof(struct tt__NetworkInterfaceConnectionSetting));

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->OperSettings->AutoNegotiation = xsd__boolean__true_;
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->OperSettings->Speed= 100;
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->OperSettings->Duplex= tt__Duplex__Full;

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4 = (struct tt__IPv4NetworkInterface *)soap_malloc_v2(soap,sizeof(struct tt__IPv4NetworkInterface));

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Enabled = xsd__boolean__true_;

	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config  = (struct tt__IPv4Configuration *)soap_malloc_v2(soap,sizeof(struct tt__IPv4Configuration));
	//if(0 == net_conf.dhcp_ip)
		tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->__sizeManual = 1;
	//else
	//	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->__sizeManual = 0;
	tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->DHCP = dhcp_enable;
	//if(0 == net_conf.dhcp_ip)
	if(!dhcp_enable)
	{
		tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual = (struct tt__PrefixedIPv4Address *)soap_malloc_v2(soap,sizeof(struct tt__PrefixedIPv4Address));
		tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual->Address = (char *)soap_malloc(soap,24);
		if(TRUE == isWIFI)
		{
			strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual->Address, wifi_cfg.dwNetIpAddr.csIpV4);
			tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual->PrefixLength = CalcPrfixLength(wifi_cfg.dwNetMask.csIpV4);
		}
		else
		{
			strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual->Address, net_conf.stEtherNet[0].strIPAddr.csIpV4);
			tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual->PrefixLength = CalcPrfixLength(net_conf.stEtherNet[0].strIPMask.csIpV4);			
		}
	}
	else
	{
		tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->FromDHCP = (struct tt__PrefixedIPv4Address *)soap_malloc_v2(soap,sizeof(struct tt__PrefixedIPv4Address));
		tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->FromDHCP->Address = (char *)soap_malloc(soap,24);
		strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->FromDHCP->Address, hostip);
		tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->FromDHCP->PrefixLength = 24;
	}

	return SOAP_OK;
	 
 }


#if 0
int WriteNetConfigFile(void)     //dehone   111208
{
	int ret = 0;
	FILE *fp;
	SysInfo *pSysInfo = GetSysInfo();

	if ( !pSysInfo ) {
		return -1;		
	}
	
	//char stringbuffer[128];
	char *filename = NET_CONF_FILE;
	if ((fp = fopen(filename, "wb")) == NULL) {
		ONVIF_DBP("Can't create config file\n");
		ret = -1;
	} else {
		//fwrite(stringbuffer, 1, sizeof(stringbuffer), fp)
		struct in_addr ip;
		//ip.s_addr = net_get_ifaddr(ETH_NAME);
		ip.s_addr = pSysInfo->lan_config.net.ip.s_addr;		
		fprintf(fp,"LocalIP=%s\n",inet_ntoa(ip));
		//ip.s_addr = net_get_netmask(ETH_NAME);
		ip.s_addr = pSysInfo->lan_config.net.netmask.s_addr;
		fprintf(fp,"NetMask=%s\n",inet_ntoa(ip));
		//ip.s_addr = net_get_gateway();
		ip.s_addr = pSysInfo->lan_config.net.gateway.s_addr;		
		fprintf(fp,"Gateway=%s\n",inet_ntoa(ip));
		//ip.s_addr = net_get_dns();
		ip.s_addr = pSysInfo->lan_config.net.dns.s_addr;
		fprintf(fp,"DNS=%s\n",inet_ntoa(ip));
		//fprintf(fp,"DHCPConfig=%d\n",pSysInfo->lan_config.net.dhcp_config);
		fprintf(fp,"DHCPEnable=%d\n", 0); //pSysInfo->lan_config.net.dhcp_enable);
		fprintf(fp,"HTTPPort=%d\n",pSysInfo->lan_config.net.http_port);
		fprintf(fp,"Title=%s\n",pSysInfo->lan_config.title);
		__u8 mac[6];
		net_get_hwaddr(ETH_NAME, mac);
		fprintf(fp,"MAC=%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		fclose(fp);
	}
	return ret;
}

int WriteHDCPNetConfigFile(void)     //dehone   111208
{
	int ret = 0;
	FILE *fp;
	SysInfo *pSysInfo = GetSysInfo();
	if ( !pSysInfo) {
		return -1;
	}
	
	//char stringbuffer[128];
	char *filename = NET_CONF_FILE;
	if ((fp = fopen(filename, "wb")) == NULL) {
		ONVIF_DBP("Can't create config file\n");
		ret = -1;
	} else {
		//fwrite(stringbuffer, 1, sizeof(stringbuffer), fp)
		struct in_addr ip;
		ip.s_addr = net_get_ifaddr(ETH_NAME);		
		
		fprintf(fp,"LocalIP=%s\n",inet_ntoa(ip));
		ControlSystemData(SFIELD_SET_IP, (void *)&ip.s_addr, sizeof(ip.s_addr));
		
		ip.s_addr = net_get_netmask(ETH_NAME);
		fprintf(fp,"NetMask=%s\n",inet_ntoa(ip));
		ip.s_addr = net_get_gateway();
		fprintf(fp,"Gateway=%s\n",inet_ntoa(ip));
		//ip.s_addr = net_get_dns();
		ip.s_addr = pSysInfo->lan_config.net.dns.s_addr;
		fprintf(fp,"DNS=%s\n", inet_ntoa(ip));
		//fprintf(fp,"DHCPConfig=%d\n",pSysInfo->lan_config.net.dhcp_config);
		fprintf(fp,"DHCPEnable=%d\n", 0); //pSysInfo->lan_config.net.dhcp_enable);
		fprintf(fp,"HTTPPort=%d\n",pSysInfo->lan_config.net.http_port);
		fprintf(fp,"Title=%s\n",pSysInfo->lan_config.title);
		__u8 mac[6];
		net_get_hwaddr(ETH_NAME, mac);
		fprintf(fp,"MAC=%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		fclose(fp);
	}
	return ret;
}
#endif

void ONVIF_GetNetworkConfig(struct soap *soap, QMAPI_NET_NETWORK_CFG *network_cfg, QMAPI_NET_WIFI_CONFIG *wifi_cfg, int *isWIFI)
{
	memset(network_cfg, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));

	get_netcfg_fromsock(soap, network_cfg, wifi_cfg, isWIFI);
}

void ONVIF_MoidfyNetworkInterface(struct soap *soap, QMAPI_NET_NETWORK_CFG *net_conf, QMAPI_NET_WIFI_CONFIG *wifi_cfg, int isWIFI, int isDhcp, const char *ipaddr, const char *ipMask)
{
	int isModify = 0;
	
	if(TRUE == isWIFI)
	{
		if (isDhcp != -1)
		{
			wifi_cfg->bWifiEnable = isDhcp;
			isModify = 1;
		}

		if (ipaddr[0])
		{
			strncpy(wifi_cfg->dwNetIpAddr.csIpV4, ipaddr, LEN_16_SIZE);
			strncpy(wifi_cfg->dwNetMask.csIpV4, ipMask, LEN_16_SIZE);
			isModify = 1;
		}

		if (isModify)
		{
			QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_WIFICFG, 0, wifi_cfg, sizeof(QMAPI_NET_WIFI_CONFIG));
			QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, 0, 0, 0);

			soap->pConfig->system_reboot_flag = NORMAL_REBOOT;
		}
	}
	else
	{
		if (isDhcp != -1)
		{
			net_conf->byEnableDHCP = isDhcp;
			isModify = 1;
		}

		if (ipaddr[0])
		{
			strncpy(net_conf->stEtherNet[0].strIPAddr.csIpV4, ipaddr, LEN_16_SIZE);
			strncpy(net_conf->stEtherNet[0].strIPMask.csIpV4, ipMask, LEN_16_SIZE);
			isModify = 1;
		}

		if (isModify)
		{
			if (!set_network_cfg(net_conf))
			{
				QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, 0, 0, 0);
			}

			soap->pConfig->system_reboot_flag = NORMAL_REBOOT;			
		}
	}

}

static void ConvertPrfixLength(int len, char *ipaddr)
{
	int i = 1, j = 0;
	int count;
	for (count = 0; count < len; count++)
	{
		ipaddr[j] |= 1<<(8 - i);
		
		if (i % 8 == 0)
		{
			++j;
			i = 1;
		}
		else
		{
			++i;
		}
	}

	sprintf(ipaddr, "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
	//printf("mask = %s\n", ipaddr);
}

int __tds__SetNetworkInterfaces(struct soap *soap, struct _tds__SetNetworkInterfaces *tds__SetNetworkInterfaces, struct _tds__SetNetworkInterfacesResponse *tds__SetNetworkInterfacesResponse, int *dhcp, char *ip, char *ipMask)
{
	//tds__SetNetworkInterfacesResponse->RebootNeeded = xsd__boolean__true_;
	
	ONVIF_DBP("============= __tds__SetNetworkInterfaces\n");
	
	if(!tds__SetNetworkInterfaces->NetworkInterface)
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:InvalidNetworkInterface",
			"The supplied network interface token does not exists."));
	
	if(tds__SetNetworkInterfaces->NetworkInterface->IPv4)
	{
		if(tds__SetNetworkInterfaces->NetworkInterface->IPv4->DHCP)
		{
			*dhcp = *(tds__SetNetworkInterfaces->NetworkInterface->IPv4->DHCP);
			/*
			memset(net_conf.ip,0,LEN_16_SIZE);
			strncpy(net_conf.ip,STATIC_DEV_IP, LEN_16_SIZE);
			if(4 != sscanf(net_conf.ip,"%d.%d.%d.%d",&ipaddr[0],&ipaddr[1],&ipaddr[2],&ipaddr[3]))
				return SOAP_ERR;	
			memset(net_conf.gateway,0,sizeof(net_conf.gateway));
			sprintf(net_conf.gateway,"%d.%d.%d.%d",ipaddr[0],ipaddr[1],ipaddr[2],1);
			*/
		}

		if(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual &&
			tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual[0].Address)
		{
			if((strlen(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual[0].Address) > 0)&&
				(1 == check_ipaddress(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual[0].Address)))
			{	
				strncpy(ip, tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual[0].Address, LEN_16_SIZE);
				ConvertPrfixLength(tds__SetNetworkInterfaces->NetworkInterface->IPv4->Manual->PrefixLength, ipMask);
			}
			else
			{
				return(send_soap_fault_msg(soap,400,
					"SOAP-ENV:Sender",
					"ter:InvalidArgVal",
					"ter:InvalidIPv4Address",
					"The suggested IPv4 address is invalid."));
			}
		}
	}
	
	printf("###### __tds__SetNetworkInterfaces ip:%s \n", ip);
	return SOAP_OK;
}

int __tds__GetNetworkProtocols(struct soap *soap, struct _tds__GetNetworkProtocols *tds__GetNetworkProtocols, struct _tds__GetNetworkProtocolsResponse *tds__GetNetworkProtocolsResponse)
{
	ONVIF_DBP("");

	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));

	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);


	tds__GetNetworkProtocolsResponse->__sizeNetworkProtocols = 2;
	tds__GetNetworkProtocolsResponse->NetworkProtocols = soap_malloc_v2(soap,sizeof(struct tt__NetworkProtocol)*2);

	tds__GetNetworkProtocolsResponse->NetworkProtocols[0].Name = tt__NetworkProtocolType__HTTP;
	tds__GetNetworkProtocolsResponse->NetworkProtocols[0].Enabled = soap->pConfig->networkProtocol[0];

	tds__GetNetworkProtocolsResponse->NetworkProtocols[0].__sizePort=1;
	tds__GetNetworkProtocolsResponse->NetworkProtocols[0].Port = (int *)soap_malloc(soap, sizeof(int));
	*(tds__GetNetworkProtocolsResponse->NetworkProtocols[0].Port) = net_conf.wHttpPort;

	tds__GetNetworkProtocolsResponse->NetworkProtocols[1].Name = tt__NetworkProtocolType__RTSP;
	tds__GetNetworkProtocolsResponse->NetworkProtocols[1].Enabled = soap->pConfig->networkProtocol[1];

	tds__GetNetworkProtocolsResponse->NetworkProtocols[1].__sizePort=1;
	tds__GetNetworkProtocolsResponse->NetworkProtocols[1].Port = (int *)soap_malloc(soap, sizeof(int));
	*(tds__GetNetworkProtocolsResponse->NetworkProtocols[1].Port) = get_rtsp_port();
	 
	 return SOAP_OK; 
 }


int __tds__SetNetworkProtocols(struct soap *soap, struct _tds__SetNetworkProtocols *tds__SetNetworkProtocols, struct _tds__SetNetworkProtocolsResponse *tds__SetNetworkProtocolsResponse)
{
	int i;
	for (i = 0; i < tds__SetNetworkProtocols->__sizeNetworkProtocols; i++)
	{
		if (tds__SetNetworkProtocols->NetworkProtocols[i].Name == tt__NetworkProtocolType__HTTPS)
		{
			return(send_soap_fault_msg(soap,400, "SOAP-ENV:Sender", "ter:InvalidArgVal", "ter:ServiceNotSupported",
				"not supported."));
		}
	}

	for (i = 0; i < tds__SetNetworkProtocols->__sizeNetworkProtocols; i++)
	{
		if (tds__SetNetworkProtocols->NetworkProtocols[i].Name == tt__NetworkProtocolType__HTTP)
		{
			soap->pConfig->networkProtocol[0] = tds__SetNetworkProtocols->NetworkProtocols[i].Enabled;
		}
		else
		{
			soap->pConfig->networkProtocol[1] = tds__SetNetworkProtocols->NetworkProtocols[i].Enabled;
		}
	}	

	return SOAP_OK;
	
}


int __tds__GetNetworkDefaultGateway(struct soap *soap, struct _tds__GetNetworkDefaultGateway *tds__GetNetworkDefaultGateway, struct _tds__GetNetworkDefaultGatewayResponse *tds__GetNetworkDefaultGatewayResponse)
{
	ONVIF_DBP("");

	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));

	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);

	tds__GetNetworkDefaultGatewayResponse->NetworkGateway = (struct tt__NetworkGateway *)soap_malloc_v2(soap,sizeof(struct tt__NetworkGateway));

	tds__GetNetworkDefaultGatewayResponse->NetworkGateway->__sizeIPv4Address = 1;
	tds__GetNetworkDefaultGatewayResponse->NetworkGateway->IPv4Address =  (char **)soap_malloc(soap,sizeof(char *));
	tds__GetNetworkDefaultGatewayResponse->NetworkGateway->IPv4Address[0] = (char *)soap_malloc(soap,LEN_16_SIZE);

	if(TRUE == isWIFI)
		strncpy(tds__GetNetworkDefaultGatewayResponse->NetworkGateway->IPv4Address[0], wifi_cfg.dwGateway.csIpV4, LEN_16_SIZE);
	else
		strncpy(tds__GetNetworkDefaultGatewayResponse->NetworkGateway->IPv4Address[0], net_conf.stGatewayIpAddr.csIpV4, LEN_16_SIZE);

	return SOAP_OK; 
} 

int __tds__SetNetworkDefaultGateway(struct soap *soap, struct _tds__SetNetworkDefaultGateway *tds__SetNetworkDefaultGateway, struct _tds__SetNetworkDefaultGatewayResponse *tds__SetNetworkDefaultGatewayResponse)
{
	char token[6]={0};
	char hostip[24]={0};

	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));

	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);

	memset(hostip, 0, sizeof(hostip));
	if(TRUE == isWIFI)
		strcpy(token, WIFI_NAME);
	else
		strcpy(token, ETH_NAME);

	//printf("#### %s %d, token:%s, IPv4Address:%s\n", __FUNCTION__, __LINE__, token, *(tds__SetNetworkDefaultGateway->IPv4Address));

	if(tds__SetNetworkDefaultGateway->IPv4Address)
		printf("#### %s %d\n", __FUNCTION__, __LINE__);
	
	if((tds__SetNetworkDefaultGateway->__sizeIPv4Address) &&
		(tds__SetNetworkDefaultGateway->IPv4Address) &&
		*(tds__SetNetworkDefaultGateway->IPv4Address) &&
		(check_gatewayaddress(*(tds__SetNetworkDefaultGateway->IPv4Address))>0))
	{
		if(TRUE == isWIFI)
		{
			if(strcmp(wifi_cfg.dwGateway.csIpV4, *(tds__SetNetworkDefaultGateway->IPv4Address)))
			{
				strcpy(wifi_cfg.dwGateway.csIpV4, *(tds__SetNetworkDefaultGateway->IPv4Address));
				QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_WIFICFG, 0, &wifi_cfg, sizeof(QMAPI_NET_WIFI_CONFIG));
			}
		}
		else
		{
			if(strcmp(net_conf.stGatewayIpAddr.csIpV4, *(tds__SetNetworkDefaultGateway->IPv4Address)))
			{
				strcpy(net_conf.stGatewayIpAddr.csIpV4, *(tds__SetNetworkDefaultGateway->IPv4Address));
				QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_NETCFG, 0, &net_conf, sizeof(QMAPI_NET_NETWORK_CFG));
			}
		}
	}
	else
	{
		return(send_soap_fault_msg(soap,400,
					"SOAP-ENV:Sender",
					"ter:InvalidArgVal",
					"ter:InvalidGatewayAddress",
					"The suggested IPv4 address is invalid."));
	}
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, 0, 0, 0);

	ONVIF_DBP("=============Leave \n");

	return SOAP_OK;
}


int  __tds__GetZeroConfiguration(struct soap *soap, 
	struct _tds__GetZeroConfiguration *tds__GetZeroConfiguration, 
	struct _tds__GetZeroConfigurationResponse *tds__GetZeroConfigurationResponse)
{ 

	tds__GetZeroConfigurationResponse->ZeroConfiguration = 
	(struct tt__NetworkZeroConfiguration *)soap_malloc_v2(soap,sizeof(struct tt__NetworkZeroConfiguration));
	tds__GetZeroConfigurationResponse->ZeroConfiguration->InterfaceToken = (char *)soap_malloc(soap, strlen(ETH_NAME) + 1);
	strcpy(tds__GetZeroConfigurationResponse->ZeroConfiguration->InterfaceToken, ETH_NAME);
	tds__GetZeroConfigurationResponse->ZeroConfiguration->Enabled = xsd__boolean__true_;
	tds__GetZeroConfigurationResponse->ZeroConfiguration->__sizeAddresses = 1;
	tds__GetZeroConfigurationResponse->ZeroConfiguration->Addresses = 
	(char **)soap_malloc(soap,sizeof(char *) * tds__GetZeroConfigurationResponse->ZeroConfiguration->__sizeAddresses);

	tds__GetZeroConfigurationResponse->ZeroConfiguration->Addresses[0] = (char*)soap_malloc_v2(soap, INFO_LENGTH);

	
	strcpy(tds__GetZeroConfigurationResponse->ZeroConfiguration->Addresses[0], "192.168.1.168");

	//tds__GetZeroConfigurationResponse->ZeroConfiguration->Addresses[0] = "192.168.1.168";	

	return SOAP_OK; 

}

 int  __tds__SetZeroConfiguration(struct soap *soap, 
 	struct _tds__SetZeroConfiguration *tds__SetZeroConfiguration, 
 	struct _tds__SetZeroConfigurationResponse *tds__SetZeroConfigurationResponse)
{ 
#if 0
	NET_CONF net_conf;
	if(tds__SetZeroConfiguration->InterfaceToken&&
		(0 == strcmp(tds__SetZeroConfiguration->InterfaceToken,NET_DEVICE)))
	{
		if(tds__SetZeroConfiguration->Enabled == xsd__boolean__true_)
		{
			memset(&net_conf,0,sizeof(net_conf));
			if(get_eth0_conf(IFCFG_ETH0_FILE,&net_conf) < 0)
			{
				return SOAP_ERR;
			}
			net_conf.dhcp_ip = 1;
			net_conf.dhcp_dns = 1;
			//memset(net_conf.ip,0,sizeof(net_conf.ip));
			//strcpy(net_conf.ip,GS_DEFAULT_IPADDR);
			//memset(net_conf.gateway,0,sizeof(net_conf.gateway));
			//strcpy(net_conf.gateway,GS_DEFAULT_GATEWAY);
			if(set_net_conf(&net_conf) < 0)
				return SOAP_ERR;
		}
	}
#endif 	
	return SOAP_OK; 
}


#if 0
 int  __tds__GetIPAddressFilter(struct soap *soap, 
 	struct _tds__GetIPAddressFilter *tds__GetIPAddressFilter, 
 	struct _tds__GetIPAddressFilterResponse *tds__GetIPAddressFilterResponse)
{ 
	NET_CONF	net_conf;
	memset(&net_conf,0,sizeof(net_conf));
	
	//if(0 != get_eth0_conf(IFCFG_ETH0_FILE,&net_conf))
	//{
	//	return SOAP_OK;
	//}
	
	tds__GetIPAddressFilterResponse->IPAddressFilter = soap_malloc_v2(soap,sizeof(struct tt__IPAddressFilter));
	memset(tds__GetIPAddressFilterResponse->IPAddressFilter,0,sizeof(struct tt__IPAddressFilter));
	
	//tt__IPAddressFilterType__Allow = 0, tt__IPAddressFilterType__Deny = 1
	tds__GetIPAddressFilterResponse->IPAddressFilter->Type = tt__IPAddressFilterType__Allow;
	tds__GetIPAddressFilterResponse->IPAddressFilter->__sizeIPv4Address = 1;
	tds__GetIPAddressFilterResponse->IPAddressFilter->IPv4Address = 
		soap_malloc_v2(soap,sizeof(struct tt__PrefixedIPv4Address));
	memset(tds__GetIPAddressFilterResponse->IPAddressFilter->IPv4Address, 0, 
		sizeof(struct tt__PrefixedIPv4Address));
	tds__GetIPAddressFilterResponse->IPAddressFilter->IPv4Address->Address = 
		soap_malloc_v2(soap,LEN_16_SIZE);
	memset(tds__GetIPAddressFilterResponse->IPAddressFilter->IPv4Address->Address, 0, 
		LEN_16_SIZE);
	strcpy(tds__GetIPAddressFilterResponse->IPAddressFilter->IPv4Address->Address,net_conf.ip);
	
	return SOAP_OK; 
}

 int  __tds__SetIPAddressFilter(struct soap *soap, 
 	struct _tds__SetIPAddressFilter *tds__SetIPAddressFilter, 
 	struct _tds__SetIPAddressFilterResponse *tds__SetIPAddressFilterResponse)
{ 
	NET_CONF	net_conf;
	memset(&net_conf,0,sizeof(net_conf));
	
	if(0 != get_eth0_conf(IFCFG_ETH0_FILE,&net_conf))
	{
		return SOAP_OK;
	}
	
	if(tds__SetIPAddressFilter->IPAddressFilter->__sizeIPv4Address&&
		(tds__SetIPAddressFilter->IPAddressFilter->IPv4Address->Address)&&
		(check_ipaddress(tds__SetIPAddressFilter->IPAddressFilter->IPv4Address->Address)>0))
	{	
		strncpy(net_conf.ip,
			tds__SetIPAddressFilter->IPAddressFilter->IPv4Address->Address,LEN_16_SIZE);
		set_net_conf(&net_conf);
	}
	return SOAP_OK; 
}


 #endif


 
 
int __tds__GetIPAddressFilter(struct soap *soap, struct _tds__GetIPAddressFilter *tds__GetIPAddressFilter, struct _tds__GetIPAddressFilterResponse *tds__GetIPAddressFilterResponse)
{
	return SOAP_OK;	
}
 
int __tds__SetIPAddressFilter(struct soap *soap, struct _tds__SetIPAddressFilter *tds__SetIPAddressFilter, struct _tds__SetIPAddressFilterResponse *tds__SetIPAddressFilterResponse)
{
	 return SOAP_OK; 
}

int  __tds__AddIPAddressFilter(struct soap *soap, struct _tds__AddIPAddressFilter *tds__AddIPAddressFilter, struct _tds__AddIPAddressFilterResponse *tds__AddIPAddressFilterResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__RemoveIPAddressFilter(struct soap *soap, struct _tds__RemoveIPAddressFilter *tds__RemoveIPAddressFilter, struct _tds__RemoveIPAddressFilterResponse *tds__RemoveIPAddressFilterResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetAccessPolicy(struct soap *soap, struct _tds__GetAccessPolicy *tds__GetAccessPolicy, struct _tds__GetAccessPolicyResponse *tds__GetAccessPolicyResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__SetAccessPolicy(struct soap *soap, struct _tds__SetAccessPolicy *tds__SetAccessPolicy, struct _tds__SetAccessPolicyResponse *tds__SetAccessPolicyResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __tds__CreateCertificate(struct soap *soap, struct _tds__CreateCertificate *tds__CreateCertificate, struct _tds__CreateCertificateResponse *tds__CreateCertificateResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetCertificates(struct soap *soap, struct _tds__GetCertificates *tds__GetCertificates, struct _tds__GetCertificatesResponse *tds__GetCertificatesResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetCertificatesStatus(struct soap *soap, struct _tds__GetCertificatesStatus *tds__GetCertificatesStatus, struct _tds__GetCertificatesStatusResponse *tds__GetCertificatesStatusResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__SetCertificatesStatus(struct soap *soap, struct _tds__SetCertificatesStatus *tds__SetCertificatesStatus, struct _tds__SetCertificatesStatusResponse *tds__SetCertificatesStatusResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__DeleteCertificates(struct soap *soap, struct _tds__DeleteCertificates *tds__DeleteCertificates, struct _tds__DeleteCertificatesResponse *tds__DeleteCertificatesResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetPkcs10Request(struct soap *soap, struct _tds__GetPkcs10Request *tds__GetPkcs10Request, struct _tds__GetPkcs10RequestResponse *tds__GetPkcs10RequestResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__LoadCertificates(struct soap *soap, struct _tds__LoadCertificates *tds__LoadCertificates, struct _tds__LoadCertificatesResponse *tds__LoadCertificatesResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetClientCertificateMode(struct soap *soap, struct _tds__GetClientCertificateMode *tds__GetClientCertificateMode, struct _tds__GetClientCertificateModeResponse *tds__GetClientCertificateModeResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __tds__SetClientCertificateMode(struct soap *soap, 
struct _tds__SetClientCertificateMode *tds__SetClientCertificateMode, 
struct _tds__SetClientCertificateModeResponse *tds__SetClientCertificateModeResponse)
{
	ONVIF_DBP("");

	if (tds__SetClientCertificateMode->Enabled == xsd__boolean__true_)
	{
		//printf("Certificate true\n");
	}

	return SOAP_OK;

}

#define GIO_ALARMOUT 			80
#define GIO_ALARMIN 			82

int  __tds__GetRelayOutputs(struct soap *soap, 
	struct _tds__GetRelayOutputs *tds__GetRelayOutputs, 
	struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse)
{	

	ONVIF_DBP("");

	tds__GetRelayOutputsResponse->__sizeRelayOutputs = 1;
	tds__GetRelayOutputsResponse->RelayOutputs = (struct tt__RelayOutput *)soap_malloc_v2(soap,sizeof(struct tt__RelayOutput));
	tds__GetRelayOutputsResponse->RelayOutputs->token = (char *)soap_malloc(soap,8);
	strcpy(tds__GetRelayOutputsResponse->RelayOutputs->token, "do_i0");

	tds__GetRelayOutputsResponse->RelayOutputs->Properties = (struct tt__RelayOutputSettings *)soap_malloc_v2(soap,sizeof(struct tt__RelayOutputSettings));
	//tds__GetRelayOutputsResponse->RelayOutputs->Properties->Mode = tt__RelayMode__Monostable;
	tds__GetRelayOutputsResponse->RelayOutputs->Properties->Mode = soap->pConfig->relayMode;
	
	tds__GetRelayOutputsResponse->RelayOutputs->Properties->DelayTime = (char *)soap_malloc(soap, LEN_128_SIZE);
	sprintf(tds__GetRelayOutputsResponse->RelayOutputs->Properties->DelayTime, "PT%dS", soap->pConfig->relayDelayTime);

	
	//tds__GetRelayOutputsResponse->RelayOutputs->Properties->IdleState = tt__RelayIdleState__closed;
	tds__GetRelayOutputsResponse->RelayOutputs->Properties->IdleState = soap->pConfig->relayIdleState;
		
	return SOAP_OK;
	
}

 
int  __tds__SetRelayOutputSettings(struct soap *soap, struct _tds__SetRelayOutputSettings *tds__SetRelayOutputSettings, struct _tds__SetRelayOutputSettingsResponse *tds__SetRelayOutputSettingsResponse)
{
	if (!tds__SetRelayOutputSettings->RelayOutputToken || strcmp(tds__SetRelayOutputSettings->RelayOutputToken, "do_i0"))
	{
		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
			"ter:RelayToken", "error relay token");
	}
	 
	int alarmout_id,delayTime,state;
	alarmout_id = tds__SetRelayOutputSettings->RelayOutputToken[strlen(tds__SetRelayOutputSettings->RelayOutputToken)-1]-'0';
	delayTime = atoi(tds__SetRelayOutputSettings->Properties->DelayTime+2);  //PT10S
	//tt__RelayIdleState__closed = 0, tt__RelayIdleState__open = 1
	state = tds__SetRelayOutputSettings->Properties->IdleState;
	//printf("outnum:%d delayTime:%s intdelayTime:%d state:%d \n",alarmout_id,tds__SetRelayOutputSettings->Properties->DelayTime,delayTime,state);

	//After setting the state, the relay remains in this state
	soap->pConfig->relayMode = tds__SetRelayOutputSettings->Properties->Mode;
	soap->pConfig->relayIdleState = tds__SetRelayOutputSettings->Properties->IdleState;
	if(tds__SetRelayOutputSettings->Properties->Mode == tt__RelayMode__Bistable)
	{
		soap->pConfig->relayDelayTime = 0;
		AlarmOut_control(alarmout_id,state,0xffffffff);
	}
	//After setting the state, the relay returns to its idle state after the specified time.
	else if(tds__SetRelayOutputSettings->Properties->Mode == tt__RelayMode__Monostable)
	{
		AlarmOut_control(alarmout_id,state,delayTime);
		soap->pConfig->relayDelayTime = delayTime;
	}
	 return SOAP_OK; 
 
 }
 
 
  
int  __tds__SetRelayOutputState(struct soap *soap, struct _tds__SetRelayOutputState *tds__SetRelayOutputState, struct _tds__SetRelayOutputStateResponse *tds__SetRelayOutputStateResponse)
{	 
	if (!tds__SetRelayOutputState->RelayOutputToken || strcmp(tds__SetRelayOutputState->RelayOutputToken, "do_i0"))
	{
	   return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
		   "ter:RelayToken", "error relay token");
	}

	int alarmout_id,state;
	alarmout_id = tds__SetRelayOutputState->RelayOutputToken[strlen(tds__SetRelayOutputState->RelayOutputToken)-1]-'0';

	//tt__RelayLogicalState__active = 0, tt__RelayLogicalState__inactive = 1
	state = tds__SetRelayOutputState->LogicalState;

	if(state == tt__RelayLogicalState__inactive)
		AlarmOut_control(alarmout_id,!state,0);

	return SOAP_OK; 

}
 
int  __tds__SendAuxiliaryCommand(struct soap *soap, struct _tds__SendAuxiliaryCommand *tds__SendAuxiliaryCommand, struct _tds__SendAuxiliaryCommandResponse *tds__SendAuxiliaryCommandResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetCACertificates(struct soap *soap, struct _tds__GetCACertificates *tds__GetCACertificates, struct _tds__GetCACertificatesResponse *tds__GetCACertificatesResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__LoadCertificateWithPrivateKey(struct soap *soap, struct _tds__LoadCertificateWithPrivateKey *tds__LoadCertificateWithPrivateKey, struct _tds__LoadCertificateWithPrivateKeyResponse *tds__LoadCertificateWithPrivateKeyResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetCertificateInformation(struct soap *soap, struct _tds__GetCertificateInformation *tds__GetCertificateInformation, struct _tds__GetCertificateInformationResponse *tds__GetCertificateInformationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__LoadCACertificates(struct soap *soap, struct _tds__LoadCACertificates *tds__LoadCACertificates, struct _tds__LoadCACertificatesResponse *tds__LoadCACertificatesResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__CreateDot1XConfiguration(struct soap *soap, struct _tds__CreateDot1XConfiguration *tds__CreateDot1XConfiguration, struct _tds__CreateDot1XConfigurationResponse *tds__CreateDot1XConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__SetDot1XConfiguration(struct soap *soap, struct _tds__SetDot1XConfiguration *tds__SetDot1XConfiguration, struct _tds__SetDot1XConfigurationResponse *tds__SetDot1XConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetDot1XConfiguration(struct soap *soap, struct _tds__GetDot1XConfiguration *tds__GetDot1XConfiguration, struct _tds__GetDot1XConfigurationResponse *tds__GetDot1XConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetDot1XConfigurations(struct soap *soap, struct _tds__GetDot1XConfigurations *tds__GetDot1XConfigurations, struct _tds__GetDot1XConfigurationsResponse *tds__GetDot1XConfigurationsResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__DeleteDot1XConfiguration(struct soap *soap, struct _tds__DeleteDot1XConfiguration *tds__DeleteDot1XConfiguration, struct _tds__DeleteDot1XConfigurationResponse *tds__DeleteDot1XConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetDot11Capabilities(struct soap *soap, struct _tds__GetDot11Capabilities *tds__GetDot11Capabilities, struct _tds__GetDot11CapabilitiesResponse *tds__GetDot11CapabilitiesResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetDot11Status(struct soap *soap, struct _tds__GetDot11Status *tds__GetDot11Status, struct _tds__GetDot11StatusResponse *tds__GetDot11StatusResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__ScanAvailableDot11Networks(struct soap *soap, struct _tds__ScanAvailableDot11Networks *tds__ScanAvailableDot11Networks, struct _tds__ScanAvailableDot11NetworksResponse *tds__ScanAvailableDot11NetworksResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tds__GetSystemUris(struct soap *soap, struct _tds__GetSystemUris *tds__GetSystemUris, struct _tds__GetSystemUrisResponse *tds__GetSystemUrisResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}


 int  __tds__StartFirmwareUpgrade(struct soap *soap, struct _tds__StartFirmwareUpgrade *tds__StartFirmwareUpgrade, struct _tds__StartFirmwareUpgradeResponse *tds__StartFirmwareUpgradeResponse)
 { 
	return SOAP_OK; 

 }

 
int  __tds__StartSystemRestore(struct soap *soap, struct _tds__StartSystemRestore *tds__StartSystemRestore, struct _tds__StartSystemRestoreResponse *tds__StartSystemRestoreResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}



int __timg__GetServiceCapabilities(struct soap *soap, struct _timg__GetServiceCapabilities *timg__GetServiceCapabilities, struct _timg__GetServiceCapabilitiesResponse *timg__GetServiceCapabilitiesResponse)
{	

	ONVIF_DBP("");
	return SOAP_OK;
}



int  __timg__GetImagingSettings(struct soap *soap, 
	 struct _timg__GetImagingSettings *timg__GetImagingSettings, 
	 struct _timg__GetImagingSettingsResponse *timg__GetImagingSettingsResponse)
{ 	

	ONVIF_DBP("");

	int ret;
	int chno;
	char *p;

	if(!timg__GetImagingSettings->VideoSourceToken)
		chno = 0;
	else
	{
		if(0!=strncmp(timg__GetImagingSettings->VideoSourceToken,"VideoSource", strlen("VideoSource")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested configuration does not exist. "));
		}	
		p = timg__GetImagingSettings->VideoSourceToken+strlen("VideoSource");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested configuration does not exist. "));
		}
	}

	QMAPI_NET_CHANNEL_COLOR colorcfg;
	QMAPI_NET_CHANNEL_ENHANCED_COLOR enhanced_color;
	memset(&colorcfg, 0, sizeof(QMAPI_NET_CHANNEL_COLOR));
	memset(&enhanced_color, 0, sizeof(QMAPI_NET_CHANNEL_ENHANCED_COLOR));
	colorcfg.dwSize = sizeof(QMAPI_NET_CHANNEL_COLOR);
	colorcfg.dwChannel = chno;
	enhanced_color.dwSize = sizeof(QMAPI_NET_CHANNEL_ENHANCED_COLOR);
	enhanced_color.dwChannel = chno;
	
	ret = get_color_param(chno, &colorcfg);
	if(ret < 0)
	{
		printf("%s get_color_param error! ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	ret = get_enhanced_color(chno, &enhanced_color);
	if(ret < 0)
	{
		printf("%s get_color_param error! ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}
		
	timg__GetImagingSettingsResponse->ImagingSettings = (struct tt__ImagingSettings20 *)soap_malloc_v2(soap,sizeof(struct tt__ImagingSettings20));

	timg__GetImagingSettingsResponse->ImagingSettings->Focus = (struct tt__FocusConfiguration20 *)soap_malloc_v2(soap,sizeof(struct tt__FocusConfiguration20));

	timg__GetImagingSettingsResponse->ImagingSettings->Focus->AutoFocusMode = tt__AutoFocusMode__AUTO;
	
	timg__GetImagingSettingsResponse->ImagingSettings->IrCutFilter = (enum tt__IrCutFilterMode *)soap_malloc_v2(soap,sizeof(enum tt__IrCutFilterMode));
	*timg__GetImagingSettingsResponse->ImagingSettings->IrCutFilter = tt__IrCutFilterMode__AUTO;
	
	timg__GetImagingSettingsResponse->ImagingSettings->Brightness = (float *)soap_malloc(soap,sizeof(float));
	*(timg__GetImagingSettingsResponse->ImagingSettings->Brightness) = (float)(colorcfg.nBrightness);
	
	timg__GetImagingSettingsResponse->ImagingSettings->Contrast = (float *)soap_malloc(soap,sizeof(float));
	*(timg__GetImagingSettingsResponse->ImagingSettings->Contrast) =  (float)(colorcfg.nContrast);
	
	timg__GetImagingSettingsResponse->ImagingSettings->ColorSaturation = (float *)soap_malloc(soap,sizeof(float));
	*(timg__GetImagingSettingsResponse->ImagingSettings->ColorSaturation) =  (float)(colorcfg.nSaturation);

	timg__GetImagingSettingsResponse->ImagingSettings->Sharpness = (float *)soap_malloc(soap,sizeof(float));
	*(timg__GetImagingSettingsResponse->ImagingSettings->Sharpness) = (float)(colorcfg.nDefinition);

	timg__GetImagingSettingsResponse->ImagingSettings->WideDynamicRange = (struct tt__WideDynamicRange20 *)soap_malloc_v2(soap,sizeof(struct tt__WideDynamicRange20));
	timg__GetImagingSettingsResponse->ImagingSettings->WideDynamicRange->Mode = enhanced_color.bWideDynamic;
	timg__GetImagingSettingsResponse->ImagingSettings->WideDynamicRange->Level = (float *)soap_malloc(soap,sizeof(float));
	*timg__GetImagingSettingsResponse->ImagingSettings->WideDynamicRange->Level = enhanced_color.nWDLevel;
	timg__GetImagingSettingsResponse->ImagingSettings->WhiteBalance = (struct tt__WhiteBalance20 *)soap_malloc_v2(soap,sizeof(struct tt__WhiteBalance20));
	timg__GetImagingSettingsResponse->ImagingSettings->WhiteBalance->Mode = tt__WhiteBalanceMode__AUTO;

	return SOAP_OK;
}
 
int  __timg__SetImagingSettings(struct soap *soap, 
	struct _timg__SetImagingSettings *timg__SetImagingSettings, 
	struct _timg__SetImagingSettingsResponse *timg__SetImagingSettingsResponse)
{
	int ret = 0;
	int chno;
	char *p;

	if(!timg__SetImagingSettings->VideoSourceToken)
		chno = 0;
	else
	{
		if(0!=strncmp(timg__SetImagingSettings->VideoSourceToken,"VideoSource", strlen("VideoSource")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested configuration does not exist. "));
		}	
		p = timg__SetImagingSettings->VideoSourceToken+strlen("VideoSource");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested configuration does not exist. "));
		}
	}

	QMAPI_NET_CHANNEL_COLOR config;
	QMAPI_NET_CHANNEL_ENHANCED_COLOR enhanced_color;
	memset(&config, 0, sizeof(QMAPI_NET_CHANNEL_COLOR));
	memset(&enhanced_color, 0, sizeof(enhanced_color));
	config.dwChannel = chno;
	config.dwSize = sizeof(QMAPI_NET_CHANNEL_COLOR);
	enhanced_color.dwChannel = chno;
	enhanced_color.dwSize = sizeof(QMAPI_NET_CHANNEL_ENHANCED_COLOR);
	
	ret = get_color_param(chno, &config);
	if(ret < 0)
	{
		printf("%s get_color_param error! ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	ret = get_enhanced_color(chno, &enhanced_color);
	if(ret < 0)
	{
		printf("%s get_color_param error! ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}	
	config.dwSize = sizeof(QMAPI_NET_CHANNEL_COLOR);
	config.dwChannel = chno;
	
	if(timg__SetImagingSettings->ForcePersistence_x0020 && 
		*timg__SetImagingSettings->ForcePersistence_x0020 == xsd__boolean__false_)
		config.dwSetFlag = 0;
	else
		config.dwSetFlag = 1;
	
	config.nHue = config.nHue;

	int wideDynamic = -1;
	int WDLevel = -1;

	do
	{
		int temp;
		ret = 0;
		if (timg__SetImagingSettings->ImagingSettings->ColorSaturation)
		{
			temp = *timg__SetImagingSettings->ImagingSettings->ColorSaturation;
			if (temp < 0 || temp > 255)
			{
				ret = -1;
				break;
			}
		
			config.nSaturation = temp;
		}

		if (timg__SetImagingSettings->ImagingSettings->Contrast)
		{
			temp = (int)(*timg__SetImagingSettings->ImagingSettings->Contrast);
			if (temp < 0 || temp > 255)
			{
				ret = -1;
				break;
			}
		
			config.nContrast = temp;
		}

		if (timg__SetImagingSettings->ImagingSettings->Brightness)
		{
			temp = (int)(*timg__SetImagingSettings->ImagingSettings->Brightness);
			if (temp < 0 || temp > 255)
			{
				ret = -1;
				break;
			}
		
			config.nBrightness = temp;
		}
		
		if (timg__SetImagingSettings->ImagingSettings->Sharpness)
		{
			temp = (int)(*timg__SetImagingSettings->ImagingSettings->Sharpness);
			if (temp < 0 || temp > 255)
			{
				ret = -1;
			}
		
			config.nDefinition = temp;
		}
		
		if (timg__SetImagingSettings->ImagingSettings->WideDynamicRange)
		{
			wideDynamic = timg__SetImagingSettings->ImagingSettings->WideDynamicRange->Mode;

			if (timg__SetImagingSettings->ImagingSettings->WideDynamicRange->Level)
			{
				WDLevel = (int)(*timg__SetImagingSettings->ImagingSettings->WideDynamicRange->Level);

				if (WDLevel < 0 || WDLevel > 255)
				{
					ret = -1;
				}
			}
		}
	} while(0);

	if (ret < 0)
	{
		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
			"ter:SettingsInvalid", "The requested configuration not supported.");	
	}

	ret = set_color_param(chno, &config);
	if(ret < 0)
	{
		//printf("%s set_color_param error! ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	QMAPI_NET_CHANNEL_COLOR_SINGLE SingleCfg = {0};
	SingleCfg.dwSize = sizeof(SingleCfg);
	SingleCfg.dwChannel = chno;
	SingleCfg.dwSaveFlag = QMAPI_COLOR_SAVE;

	if (wideDynamic != -1)
	{
		SingleCfg.dwSetFlag = QMAPI_COLOR_SET_EWD;
		SingleCfg.nValue = wideDynamic;
		QMapi_sys_ioctrl(0,QMAPI_SYSCFG_SET_COLORCFG_SINGLE,0,&SingleCfg,sizeof(SingleCfg));
	}

	if (WDLevel != -1)
	{
		SingleCfg.dwSetFlag = QMAPI_COLOR_SET_WD;
		SingleCfg.nValue = WDLevel;
		QMapi_sys_ioctrl(0,QMAPI_SYSCFG_SET_COLORCFG_SINGLE,0,&SingleCfg,sizeof(SingleCfg));
	}

	return SOAP_OK;
}

int __timg__GetOptions(struct soap *soap, struct _timg__GetOptions *timg__GetOptions, struct _timg__GetOptionsResponse *timg__GetOptionsResponse)
{

	ONVIF_DBP("");

	int chno;
	char *p;

	if(!timg__GetOptions->VideoSourceToken)
		chno = 0;
	else
	{
		if(0!=strncmp(timg__GetOptions->VideoSourceToken,"VideoSource", strlen("VideoSource")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoSource",
				"The requested VideoSource does not exist."));
		}	
		p = timg__GetOptions->VideoSourceToken+strlen("VideoSource");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoSource",
				"The requested VideoSource does not exist."));
		}
	}
	
	int ret = 0;
	QMAPI_NET_COLOR_SUPPORT color_range;
	ret = get_color_param_range(&color_range);
	if(ret)
	{
		printf("%s get_color_param_range error! ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}
	
	timg__GetOptionsResponse->ImagingOptions = (struct tt__ImagingOptions20 *)soap_malloc_v2(soap,sizeof(struct tt__ImagingOptions20));
		
	timg__GetOptionsResponse->ImagingOptions->Brightness = (struct tt__FloatRange *)soap_malloc_v2(soap,sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->Contrast = (struct tt__FloatRange *)soap_malloc_v2(soap,sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->ColorSaturation = (struct tt__FloatRange *)soap_malloc_v2(soap,sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->Sharpness = (struct tt__FloatRange *)soap_malloc_v2(soap,sizeof(struct tt__FloatRange));

	timg__GetOptionsResponse->ImagingOptions->Brightness->Min = (float)color_range.strBrightness.nMin;
	timg__GetOptionsResponse->ImagingOptions->Brightness->Max = (float)color_range.strBrightness.nMax;
	timg__GetOptionsResponse->ImagingOptions->Contrast->Min = (float)color_range.strContrast.nMin;
	timg__GetOptionsResponse->ImagingOptions->Contrast->Max = (float)color_range.strContrast.nMax;
	timg__GetOptionsResponse->ImagingOptions->ColorSaturation->Min = (float)color_range.strSaturation.nMin;
	timg__GetOptionsResponse->ImagingOptions->ColorSaturation->Max = (float)color_range.strSaturation.nMax;
	timg__GetOptionsResponse->ImagingOptions->Sharpness->Min = (float)color_range.strDefinition.nMin;
	timg__GetOptionsResponse->ImagingOptions->Sharpness->Max = (float)color_range.strDefinition.nMax;
		
	timg__GetOptionsResponse->ImagingOptions->Focus = (struct tt__FocusOptions20 *)soap_malloc_v2(soap,sizeof(struct tt__FocusOptions20));
	
	timg__GetOptionsResponse->ImagingOptions->Focus->__sizeAutoFocusModes = 1;
	timg__GetOptionsResponse->ImagingOptions->Focus->AutoFocusModes = (enum tt__AutoFocusMode *)soap_malloc_v2(soap,sizeof(enum tt__AutoFocusMode));
	timg__GetOptionsResponse->ImagingOptions->Focus->AutoFocusModes[0] = tt__AutoFocusMode__AUTO;
	
	timg__GetOptionsResponse->ImagingOptions->__sizeIrCutFilterModes = 1;
	timg__GetOptionsResponse->ImagingOptions->IrCutFilterModes = (enum tt__IrCutFilterMode *)soap_malloc_v2(soap,sizeof(enum tt__IrCutFilterMode));
	timg__GetOptionsResponse->ImagingOptions->IrCutFilterModes[0] = tt__IrCutFilterMode__AUTO;
	
	timg__GetOptionsResponse->ImagingOptions->Sharpness = (struct tt__FloatRange *)soap_malloc_v2(soap,sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->Sharpness->Min = (float)color_range.strDefinition.nMin;
	timg__GetOptionsResponse->ImagingOptions->Sharpness->Max = (float)color_range.strDefinition.nMax;

	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange = (struct tt__WideDynamicRangeOptions20 *)soap_malloc_v2(soap,sizeof(struct tt__WideDynamicRangeOptions20));
	
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->__sizeMode = 2;
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->Mode = (enum tt__WideDynamicMode *)soap_malloc_v2(soap,sizeof(enum tt__WideDynamicMode) * 2);
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->Mode[0] = tt__WideDynamicMode__OFF;
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->Mode[1] = tt__WideDynamicMode__ON;	

	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->Level = (struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->Level->Min = 0;
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->Level->Max = 255;
	
	timg__GetOptionsResponse->ImagingOptions->WhiteBalance = (struct tt__WhiteBalanceOptions20 *)soap_malloc_v2(soap,sizeof(struct tt__WhiteBalanceOptions20));
	timg__GetOptionsResponse->ImagingOptions->WhiteBalance->__sizeMode = 1;
	timg__GetOptionsResponse->ImagingOptions->WhiteBalance->Mode =  (enum tt__WhiteBalanceMode *)soap_malloc_v2(soap,sizeof(enum tt__WhiteBalanceMode));
	timg__GetOptionsResponse->ImagingOptions->WhiteBalance->Mode[0] = tt__WhiteBalanceMode__AUTO;
	
	return SOAP_OK;	
}


int __timg__Move(struct soap *soap, 
struct _timg__Move *timg__Move, 
struct _timg__MoveResponse *timg__MoveResponse)
{

	ONVIF_DBP("");

    int ret;
	int chno;
	char *p;
	float speed = 0.0;

	if(!timg__Move->VideoSourceToken)
		chno = 0;
	else
	{
		if(0!=strncmp(timg__Move->VideoSourceToken,"VideoSource", strlen("VideoSource")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoSource",
				"The requested VideoSource does not exist."));
		}	
		p = timg__Move->VideoSourceToken+strlen("VideoSource");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoSource",
				"The requested VideoSource does not exist."));
		}
	}

	if(timg__Move->Focus)
	{
	#if 0
		if(timg__Move->Focus->Absolute)
		{
            printf("%s, Focus mode:Absolute\n",__FUNCTION__);
		}
		if(timg__Move->Focus->Relative)
		{
            printf("%s, Focus mode:Relative\n",__FUNCTION__);
		}
	#endif
		if(timg__Move->Focus->Continuous)
		{
			speed = timg__Move->Focus->Continuous->Speed;
			printf("%s, Focus mode:Continuous speed:%f\n", __FUNCTION__, speed);
		}
		else
		{
			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
				"ter:NoImagingForSource", "The requested VideoSource does not support imaging settings.");		
		}

		QMAPI_NET_PTZ_CONTROL ptz_ctrl;
		memset(&ptz_ctrl, 0, sizeof(QMAPI_NET_PTZ_CONTROL));

		ptz_ctrl.dwSize = sizeof(QMAPI_NET_PTZ_CONTROL);
		ptz_ctrl.nChannel = chno;
		int sp = (int)speed;
		if(sp >= 1&& sp <= 64)
			ptz_ctrl.dwCommand = QMAPI_PTZ_STA_FOCUS_ADD;
		else if(sp <= -1 && sp >= -64)
			ptz_ctrl.dwCommand = QMAPI_PTZ_STA_FOCUS_SUB;
		else if (!sp) 
			return SOAP_OK;
		else
		{
			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal", 
				"ter:NoImagingForSource", "The requested VideoSource does not support imaging settings.");
		}

		ptz_ctrl.dwParam = sp;

		ret = QMapi_sys_ioctrl(0,QMAPI_NET_STA_PTZ_CONTROL, chno,&ptz_ctrl,sizeof(QMAPI_NET_PTZ_CONTROL));
		if(ret<0)
		{
			printf("%s, QMAPI_NET_STA_PTZ_CONTROL error!ret=%d\n", __FUNCTION__, ret);
		}
	}
	return SOAP_OK;
}

int __timg__Stop(struct soap *soap, 
	struct _timg__Stop *timg__Stop, 
	struct _timg__StopResponse *timg__StopResponse)
{	

	ONVIF_DBP("");

	int ret;
	int chno;
	char *p;

	if(!timg__Stop->VideoSourceToken)
		chno = 0;
	else
	{
		if(0!=strncmp(timg__Stop->VideoSourceToken,"VideoSource", strlen("VideoSource")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoSource",
				"The requested VideoSource does not exist."));
		}	
		p = timg__Stop->VideoSourceToken+strlen("VideoSource");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoSource",
				"The requested VideoSource does not exist."));
		}
	}

	QMAPI_NET_PTZ_CONTROL ptz_ctrl;
	memset(&ptz_ctrl, 0, sizeof(QMAPI_NET_PTZ_CONTROL));

	ptz_ctrl.dwSize = sizeof(QMAPI_NET_PTZ_CONTROL);
	ptz_ctrl.nChannel = chno;
	ptz_ctrl.dwCommand = QMAPI_PTZ_STA_STOP;

	ret = QMapi_sys_ioctrl(0,QMAPI_NET_STA_PTZ_CONTROL, chno,&ptz_ctrl,sizeof(QMAPI_NET_PTZ_CONTROL));
	if(ret<0)
	{
		printf("%s QMAPI_NET_STA_PTZ_CONTROL error!ret=%d\n", __FUNCTION__, ret);
	}


	return SOAP_OK;
}

int __timg__GetStatus(struct soap *soap, 
	struct _timg__GetStatus *timg__GetStatus, 
	struct _timg__GetStatusResponse *timg__GetStatusResponse)
{
	if (timg__GetStatus->VideoSourceToken)
	{
		do
		{
			if (!strncmp(timg__GetStatus->VideoSourceToken,"VideoSource", strlen("VideoSource")))
			{
				int chno = atoi(timg__GetStatus->VideoSourceToken + strlen("VideoSource"));
				if ((chno + 1) <= soap->pConfig->g_channel_num)
				{
					break;
				}
			}

			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoSource",
				"The requested VideoSource does not exist."));
		
		} while(0);
	}

	timg__GetStatusResponse->Status = 
	(struct tt__ImagingStatus20 *)soap_malloc_v2(soap, sizeof(struct tt__ImagingStatus20));

	timg__GetStatusResponse->Status->FocusStatus20 = 
	(struct tt__FocusStatus20 *)soap_malloc_v2(soap,sizeof(struct tt__FocusStatus20));
	timg__GetStatusResponse->Status->FocusStatus20->Position = 1;
	timg__GetStatusResponse->Status->FocusStatus20->MoveStatus = tt__MoveStatus__MOVING;

	return SOAP_OK;
}

int __timg__GetMoveOptions(struct soap *soap, 
	struct _timg__GetMoveOptions *timg__GetMoveOptions, 
	struct _timg__GetMoveOptionsResponse *timg__GetMoveOptionsResponse)
{
	if (timg__GetMoveOptions->VideoSourceToken)
	{
		do
		{
			if (!strncmp(timg__GetMoveOptions->VideoSourceToken,"VideoSource", strlen("VideoSource")))
			{
				int chno = atoi(timg__GetMoveOptions->VideoSourceToken + strlen("VideoSource"));
				if ((chno + 1) <= soap->pConfig->g_channel_num)
				{
					break;
				}
			}

			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoSource",
				"The requested VideoSource does not exist."));
		
		} while(0);
	}
	
	timg__GetMoveOptionsResponse->MoveOptions = 
		(struct tt__MoveOptions20 *)soap_malloc_v2(soap,sizeof(struct tt__MoveOptions20));
	timg__GetMoveOptionsResponse->MoveOptions->Continuous = 
		(struct tt__ContinuousFocusOptions *)soap_malloc(soap,sizeof(struct tt__ContinuousFocusOptions));
	timg__GetMoveOptionsResponse->MoveOptions->Continuous->Speed = 
		(struct tt__FloatRange *)soap_malloc(soap,sizeof(struct tt__FloatRange));

	timg__GetMoveOptionsResponse->MoveOptions->Continuous->Speed->Min = -64.0;
	timg__GetMoveOptionsResponse->MoveOptions->Continuous->Speed->Max = 64.0;
	
	return SOAP_OK;
	
}


int __timg10__GetImagingSettings(struct soap* soap, struct _timg10__GetImagingSettings *timg10__GetImagingSettings, struct _timg10__GetImagingSettingsResponse *timg10__GetImagingSettingsResponse)
{ 

	ONVIF_DBP("");

	int ret;
	int chno;
	char *p;

	if(!timg10__GetImagingSettings->VideoSourceToken)
		chno = 0;
	else
	{
		if(0!=strncmp(timg10__GetImagingSettings->VideoSourceToken,"VideoSource", strlen("VideoSource")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested configuration does not exist. "));
		}	
		p = timg10__GetImagingSettings->VideoSourceToken+strlen("VideoSource");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested configuration does not exist. "));
		}
	}

	QMAPI_NET_CHANNEL_COLOR colorcfg;
	QMAPI_NET_CHANNEL_ENHANCED_COLOR enhanced_color;
	memset(&colorcfg, 0, sizeof(QMAPI_NET_CHANNEL_COLOR));
	memset(&enhanced_color, 0, sizeof(QMAPI_NET_CHANNEL_ENHANCED_COLOR));
	colorcfg.dwSize = sizeof(QMAPI_NET_CHANNEL_COLOR);
	colorcfg.dwChannel = chno;
	enhanced_color.dwSize = sizeof(QMAPI_NET_CHANNEL_ENHANCED_COLOR);
	enhanced_color.dwChannel = chno;
	
	ret = get_color_param(chno, &colorcfg);
	if(ret < 0)
	{
		printf("%s get_color_param error! ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	ret = get_enhanced_color(chno, &enhanced_color);
	if(ret < 0)
	{
		printf("%s get_color_param error! ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}
		
	timg10__GetImagingSettingsResponse->ImagingSettings = (struct tt__ImagingSettings *)soap_malloc_v2(soap,sizeof(struct tt__ImagingSettings));
	
	timg10__GetImagingSettingsResponse->ImagingSettings->Brightness = (float *)soap_malloc(soap,sizeof(float));
	*(timg10__GetImagingSettingsResponse->ImagingSettings->Brightness) = (float)(colorcfg.nBrightness);
	
	timg10__GetImagingSettingsResponse->ImagingSettings->Contrast = (float *)soap_malloc(soap,sizeof(float));
	*(timg10__GetImagingSettingsResponse->ImagingSettings->Contrast) =  (float)(colorcfg.nContrast);
	
	timg10__GetImagingSettingsResponse->ImagingSettings->ColorSaturation = (float *)soap_malloc(soap,sizeof(float));
	*(timg10__GetImagingSettingsResponse->ImagingSettings->ColorSaturation) =  (float)(colorcfg.nSaturation);

	timg10__GetImagingSettingsResponse->ImagingSettings->Sharpness = (float *)soap_malloc(soap,sizeof(float));
	*(timg10__GetImagingSettingsResponse->ImagingSettings->Sharpness) = (float)(colorcfg.nDefinition);

	timg10__GetImagingSettingsResponse->ImagingSettings->WideDynamicRange = (struct tt__WideDynamicRange *)soap_malloc_v2(soap,sizeof(struct tt__WideDynamicRange));
	timg10__GetImagingSettingsResponse->ImagingSettings->WideDynamicRange->Mode = enhanced_color.bWideDynamic;
	timg10__GetImagingSettingsResponse->ImagingSettings->WideDynamicRange->Level = enhanced_color.nWDLevel;
	timg10__GetImagingSettingsResponse->ImagingSettings->WhiteBalance = (struct tt__WhiteBalance *)soap_malloc_v2(soap,sizeof(struct tt__WhiteBalance));
	timg10__GetImagingSettingsResponse->ImagingSettings->WhiteBalance->Mode = enhanced_color.bEnableAWB?tt__WhiteBalanceMode__AUTO:tt__WhiteBalanceMode__MANUAL;
	timg10__GetImagingSettingsResponse->ImagingSettings->WhiteBalance->CrGain = (float)enhanced_color.nRed;
	timg10__GetImagingSettingsResponse->ImagingSettings->WhiteBalance->CbGain = (float)enhanced_color.nBlue;

	return SOAP_OK; 
}

 
int __timg10__SetImagingSettings(struct soap* soap, struct _timg10__SetImagingSettings *timg10__SetImagingSettings, struct _timg10__SetImagingSettingsResponse *timg10__SetImagingSettingsResponse)
{ 

	ONVIF_DBP("");

	int ret = 0;
	int chno;
	char *p;

	if(!timg10__SetImagingSettings->VideoSourceToken)
		chno = 0;
	else
	{
		if(0!=strncmp(timg10__SetImagingSettings->VideoSourceToken,"VideoSource", strlen("VideoSource")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested configuration does not exist. "));
		}	
		p = timg10__SetImagingSettings->VideoSourceToken+strlen("VideoSource");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested configuration does not exist. "));
		}
	}

	QMAPI_NET_CHANNEL_COLOR w_config;
	QMAPI_NET_CHANNEL_COLOR r_config;
	memset(&w_config, 0, sizeof(QMAPI_NET_CHANNEL_COLOR));
	memset(&r_config, 0, sizeof(QMAPI_NET_CHANNEL_COLOR));
	r_config.dwChannel = chno;
	r_config.dwSize = sizeof(QMAPI_NET_CHANNEL_COLOR);
	
	ret = get_color_param(chno, &r_config);
	if(ret < 0)
	{
		printf("%s get_color_param error! ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}
	
	w_config.dwSize = sizeof(QMAPI_NET_CHANNEL_COLOR);
	w_config.dwChannel = chno;

	if(timg10__SetImagingSettings->ForcePersistence_x0020 && 
		*timg10__SetImagingSettings->ForcePersistence_x0020 == xsd__boolean__false_)
		w_config.dwSetFlag = 0;
	else
		w_config.dwSetFlag = 1;
	
	w_config.dwSetFlag = 1;
	w_config.nHue = r_config.nHue;
	w_config.nSaturation = (int)(*timg10__SetImagingSettings->ImagingSettings->ColorSaturation);
	w_config.nContrast = (int)(*timg10__SetImagingSettings->ImagingSettings->Contrast);
	w_config.nBrightness = (int)(*timg10__SetImagingSettings->ImagingSettings->Brightness);
	if(timg10__SetImagingSettings->ImagingSettings->Sharpness)
		w_config.nDefinition = (int)(*timg10__SetImagingSettings->ImagingSettings->Sharpness);
	else
		w_config.nDefinition = r_config.nDefinition;
	ret = set_color_param(chno,&w_config);
	if(ret < 0)
	{
		printf("%s set_color_param error! ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	return SOAP_OK; 
	
}
 
 
int __timg10__GetOptions(struct soap* soap, struct _timg10__GetOptions *timg10__GetOptions, struct _timg10__GetOptionsResponse *timg10__GetOptionsResponse)
{ 
	ONVIF_DBP("");

	timg10__GetOptionsResponse->ImagingOptions =  (struct tt__ImagingOptions *)soap_malloc_v2(soap, sizeof(struct tt__ImagingOptions));
	timg10__GetOptionsResponse->ImagingOptions->BacklightCompensation = (struct tt__BacklightCompensationOptions *)soap_malloc_v2(soap, sizeof(struct tt__BacklightCompensationOptions));
	timg10__GetOptionsResponse->ImagingOptions->BacklightCompensation->__sizeMode = 1;

	timg10__GetOptionsResponse->ImagingOptions->BacklightCompensation->Mode = (enum tt__WideDynamicMode *) soap_malloc_v2(soap, sizeof(enum tt__WideDynamicMode));
	*timg10__GetOptionsResponse->ImagingOptions->BacklightCompensation->Mode = 0;
	timg10__GetOptionsResponse->ImagingOptions->BacklightCompensation->Level = NULL;
	timg10__GetOptionsResponse->ImagingOptions->Brightness = (struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	timg10__GetOptionsResponse->ImagingOptions->ColorSaturation = (struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	timg10__GetOptionsResponse->ImagingOptions->Contrast = (struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	timg10__GetOptionsResponse->ImagingOptions->Brightness->Min = 1;
	timg10__GetOptionsResponse->ImagingOptions->Brightness->Max = 100;
	timg10__GetOptionsResponse->ImagingOptions->Contrast->Min = 1;
	timg10__GetOptionsResponse->ImagingOptions->Contrast->Max = 100;
	timg10__GetOptionsResponse->ImagingOptions->Sharpness = (struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	timg10__GetOptionsResponse->ImagingOptions->Sharpness->Min = 1;
	timg10__GetOptionsResponse->ImagingOptions->Sharpness->Max = 100;
	timg10__GetOptionsResponse->ImagingOptions->ColorSaturation->Min = 1;
	timg10__GetOptionsResponse->ImagingOptions->ColorSaturation->Max = 100;
	timg10__GetOptionsResponse->ImagingOptions->__sizeIrCutFilterModes = 0;
	timg10__GetOptionsResponse->ImagingOptions->IrCutFilterModes = (enum tt__IrCutFilterMode *)soap_malloc_v2(soap, sizeof(enum tt__IrCutFilterMode));
	*timg10__GetOptionsResponse->ImagingOptions->IrCutFilterModes = 2;
	timg10__GetOptionsResponse->ImagingOptions->Exposure = NULL;
	timg10__GetOptionsResponse->ImagingOptions->Focus = NULL;
	timg10__GetOptionsResponse->ImagingOptions->WideDynamicRange = NULL;
	timg10__GetOptionsResponse->ImagingOptions->WhiteBalance = NULL;

	int chno;
	char *p;

	if(!timg10__GetOptions->VideoSourceToken)
		chno = 0;
	else
	{
		if(0!=strncmp(timg10__GetOptions->VideoSourceToken,"VideoSource", strlen("VideoSource")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested configuration does not exist. "));
		}	
		p = timg10__GetOptions->VideoSourceToken+strlen("VideoSource");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested configuration does not exist. "));
		}
	}
	
	int ret = 0;
	QMAPI_NET_COLOR_SUPPORT color_range;
	ret = get_color_param_range(&color_range);
	if(ret)
	{
		printf("%s get_color_param_range error! ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}
	
	timg10__GetOptionsResponse->ImagingOptions = (struct tt__ImagingOptions *)soap_malloc_v2(soap,sizeof(struct tt__ImagingOptions));
		
	timg10__GetOptionsResponse->ImagingOptions->Brightness = (struct tt__FloatRange *)soap_malloc_v2(soap,sizeof(struct tt__FloatRange));
	timg10__GetOptionsResponse->ImagingOptions->Contrast = (struct tt__FloatRange *)soap_malloc_v2(soap,sizeof(struct tt__FloatRange));
	timg10__GetOptionsResponse->ImagingOptions->ColorSaturation = (struct tt__FloatRange *)soap_malloc_v2(soap,sizeof(struct tt__FloatRange));

	timg10__GetOptionsResponse->ImagingOptions->Brightness->Min = (float)color_range.strBrightness.nMin;
	timg10__GetOptionsResponse->ImagingOptions->Brightness->Max = (float)color_range.strBrightness.nMax;
	timg10__GetOptionsResponse->ImagingOptions->Contrast->Min = (float)color_range.strContrast.nMin;
	timg10__GetOptionsResponse->ImagingOptions->Contrast->Max = (float)color_range.strContrast.nMax;
	timg10__GetOptionsResponse->ImagingOptions->ColorSaturation->Min = (float)color_range.strSaturation.nMin;
	timg10__GetOptionsResponse->ImagingOptions->ColorSaturation->Max = (float)color_range.strSaturation.nMax;
	
	timg10__GetOptionsResponse->ImagingOptions->Focus = (struct tt__FocusOptions *)soap_malloc_v2(soap,sizeof(struct tt__FocusOptions));
	
	timg10__GetOptionsResponse->ImagingOptions->Focus->__sizeAutoFocusModes = 1;
	timg10__GetOptionsResponse->ImagingOptions->Focus->AutoFocusModes = (enum tt__AutoFocusMode *)soap_malloc_v2(soap,sizeof(enum tt__AutoFocusMode));
	timg10__GetOptionsResponse->ImagingOptions->Focus->AutoFocusModes[0] = tt__AutoFocusMode__AUTO;
	
	
	timg10__GetOptionsResponse->ImagingOptions->__sizeIrCutFilterModes = 1;
	timg10__GetOptionsResponse->ImagingOptions->IrCutFilterModes = (enum tt__IrCutFilterMode *)soap_malloc_v2(soap,sizeof(enum tt__IrCutFilterMode));
	timg10__GetOptionsResponse->ImagingOptions->IrCutFilterModes[0] = tt__IrCutFilterMode__ON;
	
	timg10__GetOptionsResponse->ImagingOptions->Sharpness = (struct tt__FloatRange *)soap_malloc_v2(soap,sizeof(struct tt__FloatRange));
	timg10__GetOptionsResponse->ImagingOptions->Sharpness->Min = (float)color_range.strDefinition.nMin;
	timg10__GetOptionsResponse->ImagingOptions->Sharpness->Max = (float)color_range.strDefinition.nMax;

	timg10__GetOptionsResponse->ImagingOptions->WideDynamicRange = (struct tt__WideDynamicRangeOptions *)soap_malloc_v2(soap,sizeof(struct tt__WideDynamicRangeOptions));
	
	timg10__GetOptionsResponse->ImagingOptions->WideDynamicRange->__sizeMode = 1;
	timg10__GetOptionsResponse->ImagingOptions->WideDynamicRange->Mode = (enum tt__WideDynamicMode *)soap_malloc_v2(soap,sizeof(enum tt__WideDynamicMode));
	timg10__GetOptionsResponse->ImagingOptions->WideDynamicRange->Mode[0] = tt__WideDynamicMode__OFF;
	
	timg10__GetOptionsResponse->ImagingOptions->WhiteBalance = (struct tt__WhiteBalanceOptions *)soap_malloc_v2(soap,sizeof(struct tt__WhiteBalanceOptions));
	timg10__GetOptionsResponse->ImagingOptions->WhiteBalance->__sizeMode = 1;
	timg10__GetOptionsResponse->ImagingOptions->WhiteBalance->Mode =  (enum tt__WhiteBalanceMode *)soap_malloc_v2(soap,sizeof(enum tt__WhiteBalanceMode));
	timg10__GetOptionsResponse->ImagingOptions->WhiteBalance->Mode[0] = tt__WhiteBalanceMode__AUTO;
	

	return SOAP_OK; 
}

 
int __timg10__Move(struct soap* soap, struct _timg10__Move *timg10__Move, struct _timg10__MoveResponse *timg10__MoveResponse)
{ 
    ONVIF_DBP("");

    return SOAP_OK; 
}


int __timg10__Stop(struct soap* soap, struct _timg10__Stop *timg10__Stop, struct _timg10__StopResponse *timg10__StopResponse)
{ 
    ONVIF_DBP("");
    return SOAP_OK; 
}

int __timg10__GetStatus(struct soap* soap, struct _timg10__GetStatus *timg10__GetStatus, struct _timg10__GetStatusResponse *timg10__GetStatusResponse)
{   
    ONVIF_DBP("");
    return SOAP_OK; 
}

int __timg10__GetMoveOptions(struct soap* soap, struct _timg10__GetMoveOptions *timg10__GetMoveOptions, struct _timg10__GetMoveOptionsResponse *timg10__GetMoveOptionsResponse)
{
    return SOAP_OK; 
}

int __tls__GetServiceCapabilities(struct soap *soap, struct _tls__GetServiceCapabilities *tls__GetServiceCapabilities, struct _tls__GetServiceCapabilitiesResponse *tls__GetServiceCapabilitiesResponse)
{	 
    ONVIF_DBP("");
    return SOAP_OK;
}


int __tmd__GetRelayOutputOptions(struct soap *soap, struct _tmd__GetRelayOutputOptions *tmd__GetRelayOutputOptions, struct _tmd__GetRelayOutputOptionsResponse *tmd__GetRelayOutputOptionsResponse)
{	 	
    ONVIF_DBP("");
    return SOAP_OK;
}


int __tmd__GetDigitalInputs(struct soap *soap, struct _tmd__GetDigitalInputs *tmd__GetDigitalInputs, struct _tmd__GetDigitalInputsResponse *tmd__GetDigitalInputsResponse)
{	 
    tmd__GetDigitalInputsResponse->__sizeDigitalInputs = 1;
    tmd__GetDigitalInputsResponse->DigitalInputs = (struct tt__DigitalInput *)soap_malloc_v2(soap, sizeof(struct tt__DigitalInput));
    tmd__GetDigitalInputsResponse->DigitalInputs->token = soap_strdup(soap, "DigitInputToken0");
    return SOAP_OK;
}


int __tmd__GetSerialPorts(struct soap *soap, struct _tmd__GetSerialPorts *tmd__GetSerialPorts, struct _tmd__GetSerialPortsResponse *tmd__GetSerialPortsResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int __tmd__GetSerialPortConfiguration(struct soap *soap, struct _tmd__GetSerialPortConfiguration *tmd__GetSerialPortConfiguration, struct _tmd__GetSerialPortConfigurationResponse *tmd__GetSerialPortConfigurationResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int __tmd__SetSerialPortConfiguration(struct soap *soap, struct _tmd__SetSerialPortConfiguration *tmd__SetSerialPortConfiguration, struct _tmd__SetSerialPortConfigurationResponse *tmd__SetSerialPortConfigurationResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int __tmd__GetSerialPortConfigurationOptions(struct soap *soap, struct _tmd__GetSerialPortConfigurationOptions *tmd__GetSerialPortConfigurationOptions, struct _tmd__GetSerialPortConfigurationOptionsResponse *tmd__GetSerialPortConfigurationOptionsResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int __tmd__SendReceiveSerialCommand(struct soap *soap, struct _tmd__SendReceiveSerialCommand *tmd__SendReceiveSerialCommand, struct _tmd__SendReceiveSerialCommandResponse *tmd__SendReceiveSerialCommandResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __tls__GetLayout(struct soap *soap, struct _tls__GetLayout *tls__GetLayout, struct _tls__GetLayoutResponse *tls__GetLayoutResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}
int  __tls__SetLayout(struct soap *soap, struct _tls__SetLayout *tls__SetLayout, struct _tls__SetLayoutResponse *tls__SetLayoutResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}
int  __tls__GetDisplayOptions(struct soap *soap, struct _tls__GetDisplayOptions *tls__GetDisplayOptions, struct _tls__GetDisplayOptionsResponse *tls__GetDisplayOptionsResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __tls__GetPaneConfigurations(struct soap *soap, struct _tls__GetPaneConfigurations *tls__GetPaneConfigurations, struct _tls__GetPaneConfigurationsResponse *tls__GetPaneConfigurationsResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __tls__GetPaneConfiguration(struct soap *soap, struct _tls__GetPaneConfiguration *tls__GetPaneConfiguration, struct _tls__GetPaneConfigurationResponse *tls__GetPaneConfigurationResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __tls__SetPaneConfigurations(struct soap *soap, struct _tls__SetPaneConfigurations *tls__SetPaneConfigurations, struct _tls__SetPaneConfigurationsResponse *tls__SetPaneConfigurationsResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}
 
int  __tls__SetPaneConfiguration(struct soap *soap, struct _tls__SetPaneConfiguration *tls__SetPaneConfiguration, struct _tls__SetPaneConfigurationResponse *tls__SetPaneConfigurationResponse)
{	 
	ONVIF_DBP("");
	return SOAP_OK;
}
 
int  __tls__CreatePaneConfiguration(struct soap *soap, struct _tls__CreatePaneConfiguration *tls__CreatePaneConfiguration, struct _tls__CreatePaneConfigurationResponse *tls__CreatePaneConfigurationResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __tls__DeletePaneConfiguration(struct soap *soap, struct _tls__DeletePaneConfiguration *tls__DeletePaneConfiguration, struct _tls__DeletePaneConfigurationResponse *tls__DeletePaneConfigurationResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
} 	

int __tmd__GetServiceCapabilities(struct soap *soap, struct _tmd__GetServiceCapabilities *tmd__GetServiceCapabilities, struct _tmd__GetServiceCapabilitiesResponse *tmd__GetServiceCapabilitiesResponse)
{	
	tmd__GetServiceCapabilitiesResponse->Capabilities = (struct tmd__Capabilities *)soap_malloc_v2(soap, sizeof(struct tmd__Capabilities));
	tmd__GetServiceCapabilitiesResponse->Capabilities->VideoSources = soap->pConfig->g_channel_num;
	tmd__GetServiceCapabilitiesResponse->Capabilities->VideoOutputs = 0;
	tmd__GetServiceCapabilitiesResponse->Capabilities->AudioSources = 1;
	tmd__GetServiceCapabilitiesResponse->Capabilities->AudioOutputs = 1;
	tmd__GetServiceCapabilitiesResponse->Capabilities->RelayOutputs = 1;
	return SOAP_OK;
}


int __tmd__GetAudioSources(struct soap *soap, struct _trt__GetAudioSources *trt__GetAudioSources, struct _trt__GetAudioSourcesResponse *trt__GetAudioSourcesResponse)
{
	ONVIF_DBP("");

	trt__GetAudioSourcesResponse->__sizeAudioSources = soap->pConfig->g_channel_num;
	trt__GetAudioSourcesResponse->AudioSources =soap_malloc_v2(soap,sizeof(struct tt__AudioSource) * soap->pConfig->g_channel_num);
	
	int i;
	for(i = 0;i<soap->pConfig->g_channel_num;i++)
	{
		trt__GetAudioSourcesResponse->AudioSources[i].token = (char *)soap_malloc(soap, 32);
		//strcpy(trt__GetAudioSourcesResponse->AudioSources[i].token,"axxon0");
		sprintf(trt__GetAudioSourcesResponse->AudioSources[i].token,"AudioSource%d",i);

		trt__GetAudioSourcesResponse->AudioSources[i].Channels = 1;
	}

	return SOAP_OK; 
}


int  __tmd__GetAudioOutputs(struct soap *soap, struct _trt__GetAudioOutputs *trt__GetAudioOutputs, struct _trt__GetAudioOutputsResponse *trt__GetAudioOutputsResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}

int __tmd__GetVideoSources(struct soap *soap, struct _trt__GetVideoSources *trt__GetVideoSources, struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse)
{
	ONVIF_DBP("");

	int ret;
	VIDEO_CONF video[MAX_STREAM_NUM];
	memset(video,0,sizeof(VIDEO_CONF)*MAX_STREAM_NUM);

	//int nstreamNum = soap->pConfig->g_channel_num*MAX_STREAM_NUM;
	QMAPI_NET_CHANNEL_COLOR colorcfg;
	QMAPI_NET_CHANNEL_ENHANCED_COLOR enhanced_color;
	trt__GetVideoSourcesResponse->__sizeVideoSources = soap->pConfig->g_channel_num;
	trt__GetVideoSourcesResponse->VideoSources = soap_malloc_v2(soap,sizeof(struct tt__VideoSource)*soap->pConfig->g_channel_num);

	int i;
	for(i=0;i<soap->pConfig->g_channel_num;i++)
	{
		if(get_channel_param(i,video) != 0)
		{
			return SOAP_ERR;
		}
		ret = get_color_param(i, &colorcfg);
		if(ret < 0)
		{
			printf("%s get_color_param error! ret=%d\n", __FUNCTION__, ret);
			return SOAP_ERR;
		}

		ret = get_enhanced_color(i, &enhanced_color);
		if(ret < 0)
		{
			printf("%s get_color_param error! ret=%d\n", __FUNCTION__, ret);
			return SOAP_ERR;
		}
		
		trt__GetVideoSourcesResponse->VideoSources[i].token = (char *)soap_malloc(soap, 16);
		//memset(trt__GetVideoSourcesResponse->VideoSources[i].token,0,16);
		sprintf(trt__GetVideoSourcesResponse->VideoSources[i].token,"VideoSource%d",i);

		trt__GetVideoSourcesResponse->VideoSources[i].Framerate = video[0].fps;
		trt__GetVideoSourcesResponse->VideoSources[i].Resolution = (struct tt__VideoResolution *)soap_malloc_v2(soap,sizeof(struct tt__VideoResolution));
		trt__GetVideoSourcesResponse->VideoSources[i].Resolution->Height = video[0].height;
		trt__GetVideoSourcesResponse->VideoSources[i].Resolution->Width = video[0].width;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging = (struct tt__ImagingSettings *)soap_malloc_v2(soap,sizeof(struct tt__ImagingSettings));
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Brightness = (float *)soap_malloc(soap,sizeof(float));
		*(trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Brightness) = (float)colorcfg.nBrightness;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->ColorSaturation = (float *)soap_malloc(soap,sizeof(float));
		*(trt__GetVideoSourcesResponse->VideoSources[i].Imaging->ColorSaturation) = (float)colorcfg.nSaturation;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Contrast = (float *)soap_malloc(soap,sizeof(float));
		*(trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Contrast) = (float)colorcfg.nContrast;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Sharpness = (float *)soap_malloc(soap,sizeof(float));
		*(trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Sharpness) = (float)colorcfg.nDefinition;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WideDynamicRange = (struct tt__WideDynamicRange *)soap_malloc_v2(soap,sizeof(struct tt__WideDynamicRange));
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WideDynamicRange->Mode = enhanced_color.bWideDynamic;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WideDynamicRange->Level = enhanced_color.nWDLevel;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WhiteBalance = (struct tt__WhiteBalance *)soap_malloc_v2(soap,sizeof(struct tt__WhiteBalance));
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WhiteBalance->Mode = enhanced_color.bEnableAWB?tt__WhiteBalanceMode__AUTO:tt__WhiteBalanceMode__MANUAL;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WhiteBalance->CrGain = (float)enhanced_color.nRed;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WhiteBalance->CbGain = (float)enhanced_color.nBlue;
		
	}

	return SOAP_OK; 
}

int  __tmd__GetVideoOutputs(struct soap *soap, struct _tmd__GetVideoOutputs *tmd__GetVideoOutputs, struct _tmd__GetVideoOutputsResponse *tmd__GetVideoOutputsResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __tmd__GetVideoSourceConfiguration(struct soap *soap, struct _tmd__GetVideoSourceConfiguration *tmd__GetVideoSourceConfiguration, struct _tmd__GetVideoSourceConfigurationResponse *tmd__GetVideoSourceConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __tmd__GetVideoOutputConfiguration(struct soap *soap, struct _tmd__GetVideoOutputConfiguration *tmd__GetVideoOutputConfiguration, struct _tmd__GetVideoOutputConfigurationResponse *tmd__GetVideoOutputConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __tmd__GetAudioSourceConfiguration(struct soap *soap, struct _tmd__GetAudioSourceConfiguration *tmd__GetAudioSourceConfiguration, struct _tmd__GetAudioSourceConfigurationResponse *tmd__GetAudioSourceConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __tmd__GetAudioOutputConfiguration(struct soap *soap, struct _tmd__GetAudioOutputConfiguration *tmd__GetAudioOutputConfiguration, struct _tmd__GetAudioOutputConfigurationResponse *tmd__GetAudioOutputConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __tmd__SetVideoSourceConfiguration(struct soap *soap, struct _tmd__SetVideoSourceConfiguration *tmd__SetVideoSourceConfiguration, struct _tmd__SetVideoSourceConfigurationResponse *tmd__SetVideoSourceConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __tmd__SetVideoOutputConfiguration(struct soap *soap, struct _tmd__SetVideoOutputConfiguration *tmd__SetVideoOutputConfiguration, struct _tmd__SetVideoOutputConfigurationResponse *tmd__SetVideoOutputConfigurationResponse)
{
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __tmd__SetAudioSourceConfiguration(struct soap *soap,
struct _tmd__SetAudioSourceConfiguration *tmd__SetAudioSourceConfiguration,
struct _tmd__SetAudioSourceConfigurationResponse *tmd__SetAudioSourceConfigurationResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __tmd__SetAudioOutputConfiguration(struct soap *soap, struct _tmd__SetAudioOutputConfiguration *tmd__SetAudioOutputConfiguration, struct _tmd__SetAudioOutputConfigurationResponse *tmd__SetAudioOutputConfigurationResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __tmd__GetVideoSourceConfigurationOptions(struct soap *soap,
	struct _tmd__GetVideoSourceConfigurationOptions *tmd__GetVideoSourceConfigurationOptions,
         struct _tmd__GetVideoSourceConfigurationOptionsResponse *tmd__GetVideoSourceConfigurationOptionsResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __tmd__GetVideoOutputConfigurationOptions(struct soap *soap, struct _tmd__GetVideoOutputConfigurationOptions *tmd__GetVideoOutputConfigurationOptions, struct _tmd__GetVideoOutputConfigurationOptionsResponse *tmd__GetVideoOutputConfigurationOptionsResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}
 
int  __tmd__GetAudioSourceConfigurationOptions(struct soap *soap, struct _tmd__GetAudioSourceConfigurationOptions *tmd__GetAudioSourceConfigurationOptions, struct _tmd__GetAudioSourceConfigurationOptionsResponse *tmd__GetAudioSourceConfigurationOptionsResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}
 
int  __tmd__GetAudioOutputConfigurationOptions(struct soap *soap, struct _tmd__GetAudioOutputConfigurationOptions *tmd__GetAudioOutputConfigurationOptions, struct _tmd__GetAudioOutputConfigurationOptionsResponse *tmd__GetAudioOutputConfigurationOptionsResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}
 
int  __tmd__GetRelayOutputs(struct soap *soap, struct _tds__GetRelayOutputs *tds__GetRelayOutputs, struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __tmd__SetRelayOutputSettings(struct soap *soap, struct _tmd__SetRelayOutputSettings *tmd__SetRelayOutputSettings, struct _tmd__SetRelayOutputSettingsResponse *tmd__SetRelayOutputSettingsResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}
 
int  __tmd__SetRelayOutputState(struct soap *soap, struct _tds__SetRelayOutputState *tds__SetRelayOutputState, struct _tds__SetRelayOutputStateResponse *tds__SetRelayOutputStateResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}

#if 0
Presets g_preSets; 

void GetPresetsFromFile( )
{

	FILE *fp = NULL;
	if ( (fp = fopen(PRESET_FILE, "r+")) != NULL ) {
		if (sizeof(g_preSets) != fread(&g_preSets, 1, sizeof(g_preSets), fp ) ) {
			memset(&g_preSets, 0, sizeof(g_preSets));
			fwrite(&g_preSets, 1, sizeof(g_preSets), fp);
		}
		fclose(fp);
	}

} 
 
void SavePresetsToFile()
{

	FILE *fp = NULL;
	if ( (fp = fopen(PRESET_FILE, "w+")) != NULL ) {		

		//fseek(fp, 0, SEEK_SET);
		
		if ( sizeof(g_preSets) != fwrite(&g_preSets, 1, sizeof(g_preSets), fp ) ) {
			ONVIF_DBP("=========== SavePresetsToFile failed\n");
		}   

		fclose(fp);
	}
	
}
#endif

int __tptz__GetServiceCapabilities(struct soap *soap, struct _tptz__GetServiceCapabilities *tptz__GetServiceCapabilities, struct _tptz__GetServiceCapabilitiesResponse *tptz__GetServiceCapabilitiesResponse)
{
	tptz__GetServiceCapabilitiesResponse->Capabilities = (struct tptz__Capabilities *)soap_malloc_v2(soap, sizeof(struct tptz__Capabilities));
	tptz__GetServiceCapabilitiesResponse->Capabilities->EFlip = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tptz__GetServiceCapabilitiesResponse->Capabilities->EFlip = xsd__boolean__false_;

	tptz__GetServiceCapabilitiesResponse->Capabilities->Reverse = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tptz__GetServiceCapabilitiesResponse->Capabilities->Reverse = xsd__boolean__false_;

	return SOAP_OK;
}

#if 0
long g_ptz_duration = 3;
int  g_ptz_status = tt__MoveStatus__MOVING;
int  g_ptzThreadExit = 0;
pthread_t g_threadPtz = 0; 

void PtzCtrl_thread( void* arg)
{
	time_t time_start = time(NULL);
	time_t time_now = time_start;
	
	while ( time_now < g_ptz_duration + time_start - 1 ) {
		time_now = time(NULL);
		usleep(80000);

		if ( g_ptzThreadExit ) {
			ONVIF_DBP("  exit \n");
			g_ptz_status = tt__MoveStatus__IDLE;
			return;
		}	
	}

	send_ptz_cmd(PTZ_STOP_ALL);	
	
	g_ptz_status = tt__MoveStatus__IDLE;

	ONVIF_DBP("  g_ptz_status tt__MoveStatus__IDLE \n");	
	
	return;
}
#endif

int __tptz__GetPresetTours(struct soap *soap, struct _tptz__GetPresetTours *tptz__GetPresetTours, struct _tptz__GetPresetToursResponse *tptz__GetPresetToursResponse)
{	 
	ONVIF_DBP("");
	int i, ret;
	int chno;
	char *p;

	if(!tptz__GetPresetTours->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoProfile",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz__GetPresetTours->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__GetPresetTours->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz__GetPresetTours->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}

	QMAPI_NET_CRUISE_CFG stCruiseCfg;
	memset(&stCruiseCfg, 0, sizeof(stCruiseCfg));
	stCruiseCfg.dwSize = sizeof(stCruiseCfg);
	stCruiseCfg.nChannel = chno;

	ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_CRUISE_CFG, chno,&stCruiseCfg,sizeof(QMAPI_NET_CRUISE_CFG));
	if(ret<0)
	{
		printf("%s QMAPI_SYSCFG_GET_CRUISE_CFG error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	/*
	tptz__GetPresetToursResponse->__sizePresetTour = 0;
	for(i=0;i<QMAPI_MAX_CRUISE_GROUP_NUM;i++)
	{
		if(stCruiseCfg.struCruise[i].byPointNum>0 
			|| stCruiseCfg.struCruise[i].byRes[0]>0)
			tptz__GetPresetToursResponse->__sizePresetTour++;
	}

	if(tptz__GetPresetToursResponse->__sizePresetTour == 0)
	{
		tptz__GetPresetToursResponse->PresetTour = NULL;
		return SOAP_OK;
	}
	*/
	tptz__GetPresetToursResponse->__sizePresetTour = QMAPI_MAX_CRUISE_GROUP_NUM;

	tptz__GetPresetToursResponse->PresetTour = 
		(struct tt__PresetTour *)soap_malloc_v2(soap, 
		sizeof(struct tt__PresetTour)*tptz__GetPresetToursResponse->__sizePresetTour);
	//memset(tptz__GetPresetToursResponse->PresetTour, 0, 
	//	sizeof(struct tt__PresetTour)*tptz__GetPresetToursResponse->__sizePresetTour);

	int j=0;
	for(i=0;i<QMAPI_MAX_CRUISE_GROUP_NUM;i++)
	{
		//if(stCruiseCfg.struCruise[i].byPointNum>0 || stCruiseCfg.struCruise[i].byRes[0]>0)
		{
			tptz__GetPresetToursResponse->PresetTour[j].Name = (char *)soap_malloc(soap, 64);
			sprintf(tptz__GetPresetToursResponse->PresetTour[j].Name, "tour%d", i);
			tptz__GetPresetToursResponse->PresetTour[j].token = (char *)soap_malloc(soap, 64);
			sprintf(tptz__GetPresetToursResponse->PresetTour[j].token, "tour%d", i);
			tptz__GetPresetToursResponse->PresetTour[j].Status = 
				(struct tt__PTZPresetTourStatus *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourStatus));
			//memset(tptz__GetPresetToursResponse->PresetTour[j].Status, 0, sizeof(struct tt__PTZPresetTourStatus));
			if(stCruiseCfg.byIsCruising && stCruiseCfg.byCruisingIndex == i)
				tptz__GetPresetToursResponse->PresetTour[j].Status->State = tt__PTZPresetTourState__Touring;
			else
				tptz__GetPresetToursResponse->PresetTour[j].Status->State = tt__PTZPresetTourState__Idle;

			if(tptz__GetPresetToursResponse->PresetTour[j].Status->State == tt__PTZPresetTourState__Touring)
			{
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot = 
					(struct tt__PTZPresetTourSpot *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourSpot));
				//memset(tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot, 0, sizeof(struct tt__PTZPresetTourSpot));
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->PresetDetail = 
					(struct tt__PTZPresetTourPresetDetail *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourPresetDetail));
				//memset(tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->PresetDetail, 0, sizeof(struct tt__PTZPresetTourPresetDetail));
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->PresetDetail->__union_PTZPresetTourPresetDetail = 
					SOAP_UNION__tt__union_PTZPresetTourPresetDetail_PresetToken;
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->PresetDetail->union_PTZPresetTourPresetDetail.PresetToken = 
					(char *)soap_malloc(soap, 32);
				sprintf(tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->PresetDetail->union_PTZPresetTourPresetDetail.PresetToken,
					"%d", stCruiseCfg.struCruise[i].struCruisePoint[stCruiseCfg.byPointIndex].byPresetNo);
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->Speed = 
					(struct tt__PTZSpeed *)soap_malloc_v2(soap, sizeof(struct tt__PTZSpeed));
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->Speed->PanTilt = 
					(struct tt__Vector2D *)soap_malloc_v2(soap, sizeof(struct tt__Vector2D));
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->Speed->Zoom = 
					(struct tt__Vector1D *)soap_malloc_v2(soap, sizeof(struct tt__Vector1D));
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->Speed->PanTilt->x =
					(float)stCruiseCfg.struCruise[i].struCruisePoint[stCruiseCfg.byPointIndex].bySpeed;
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->Speed->PanTilt->y =
					(float)stCruiseCfg.struCruise[i].struCruisePoint[stCruiseCfg.byPointIndex].bySpeed;
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->Speed->Zoom->x =
					(float)stCruiseCfg.struCruise[i].struCruisePoint[stCruiseCfg.byPointIndex].bySpeed;
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->Speed->PanTilt->space =NULL;
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->Speed->Zoom->space =NULL;
				tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->StayTime = 
					(char *)soap_malloc(soap, 16);
				sprintf(tptz__GetPresetToursResponse->PresetTour[j].Status->CurrentTourSpot->StayTime, 
					"PT%dS", stCruiseCfg.struCruise[i].struCruisePoint[stCruiseCfg.byPointIndex].byRemainTime);
				
			}

			tptz__GetPresetToursResponse->PresetTour[j].AutoStart = xsd__boolean__false_;
			/*
			tptz__GetPresetToursResponse->PresetTour[j].StartingCondition = 
				(struct tt__PTZPresetTourStartingCondition *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourStartingCondition));
			memset(tptz__GetPresetToursResponse->PresetTour[j].StartingCondition, 0, sizeof(struct tt__PTZPresetTourStartingCondition));
			tptz__GetPresetToursResponse->PresetTour[j].StartingCondition->RecurringTime = (int *)soap_malloc(soap, sizeof(int));
			*tptz__GetPresetToursResponse->PresetTour[j].StartingCondition->RecurringTime = 
				stCruiseCfg.struCruise[i].struCruisePoint[stCruiseCfg.struCruise[i].byPointNum-1].byRemainTime;
			*/

			tptz__GetPresetToursResponse->PresetTour[j].__sizeTourSpot = stCruiseCfg.struCruise[i].byPointNum;
			if(tptz__GetPresetToursResponse->PresetTour[j].__sizeTourSpot>0)
			{
				tptz__GetPresetToursResponse->PresetTour[j].TourSpot = 
					(struct tt__PTZPresetTourSpot *)soap_malloc_v2(soap, 
					sizeof(struct tt__PTZPresetTourSpot)*tptz__GetPresetToursResponse->PresetTour[j].__sizeTourSpot);
				//memset(tptz__GetPresetToursResponse->PresetTour[j].TourSpot, 
				//	0, sizeof(struct tt__PTZPresetTourSpot)*tptz__GetPresetToursResponse->PresetTour[j].__sizeTourSpot);
				int presetindex;
				for(presetindex=0;presetindex<tptz__GetPresetToursResponse->PresetTour[j].__sizeTourSpot;presetindex++)
				{
					tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].PresetDetail = 
						(struct tt__PTZPresetTourPresetDetail *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourPresetDetail));
					//memset(tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].PresetDetail, 0, sizeof(struct tt__PTZPresetTourPresetDetail));
					tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].PresetDetail->__union_PTZPresetTourPresetDetail = 
						SOAP_UNION__tt__union_PTZPresetTourPresetDetail_PresetToken;
					tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].PresetDetail->union_PTZPresetTourPresetDetail.PresetToken = 
						(char *)soap_malloc(soap, 32);
					sprintf(tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].PresetDetail->union_PTZPresetTourPresetDetail.PresetToken,
						"%d", stCruiseCfg.struCruise[i].struCruisePoint[presetindex].byPresetNo);
					tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].Speed = 
						(struct tt__PTZSpeed *)soap_malloc_v2(soap, sizeof(struct tt__PTZSpeed));
					tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].Speed->PanTilt = 
						(struct tt__Vector2D *)soap_malloc_v2(soap, sizeof(struct tt__Vector2D));
					tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].Speed->Zoom = 
						(struct tt__Vector1D *)soap_malloc_v2(soap, sizeof(struct tt__Vector1D));
					tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].Speed->PanTilt->x =
						(float)stCruiseCfg.struCruise[i].struCruisePoint[presetindex].bySpeed;
					tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].Speed->PanTilt->y =
						(float)stCruiseCfg.struCruise[i].struCruisePoint[presetindex].bySpeed;
					tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].Speed->Zoom->x =
						(float)stCruiseCfg.struCruise[i].struCruisePoint[presetindex].bySpeed;
					tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].Speed->PanTilt->space =NULL;
					tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].Speed->Zoom->space =NULL;
					tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].StayTime = (char *)soap_malloc(soap, 16);
					sprintf(tptz__GetPresetToursResponse->PresetTour[j].TourSpot[presetindex].StayTime, 
						"PT%dS", stCruiseCfg.struCruise[i].struCruisePoint[presetindex].byRemainTime);
				}
			}
			
			j++;
		}
	}
	
	return SOAP_OK;
} 
 
int __tptz__GetPresetTour(struct soap *soap, struct _tptz__GetPresetTour *tptz__GetPresetTour, struct _tptz__GetPresetTourResponse *tptz__GetPresetTourResponse)
{ 
	ONVIF_DBP("");
	int ret;
	int chno;
	char *p;

	if(!tptz__GetPresetTour->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoProfile",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz__GetPresetTour->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__GetPresetTour->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoProfile",
				"The requested profile does not exist. "));
		}	
		p = tptz__GetPresetTour->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoProfile",
				"The requested profile does not exist. "));
		}
	}

	int TourIndex = 0;
	if(!tptz__GetPresetTour->PresetTourToken || 
		sscanf(tptz__GetPresetTour->PresetTourToken, "tour%d", &TourIndex) !=1 ||
		TourIndex<0 || TourIndex>=QMAPI_MAX_CRUISE_GROUP_NUM)
	{
		return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoToken",
				"The requested PresetTourToken does not exist."));
	}

	QMAPI_NET_CRUISE_CFG stCruiseCfg;
	memset(&stCruiseCfg, 0, sizeof(stCruiseCfg));
	stCruiseCfg.dwSize = sizeof(stCruiseCfg);
	stCruiseCfg.nChannel = chno;

	ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_CRUISE_CFG, chno,&stCruiseCfg,sizeof(QMAPI_NET_CRUISE_CFG));
	if(ret<0)
	{
		printf("%s QMAPI_SYSCFG_GET_CRUISE_CFG error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	tptz__GetPresetTourResponse->PresetTour = 
		(struct tt__PresetTour *)soap_malloc_v2(soap, sizeof(struct tt__PresetTour));
	//memset(tptz__GetPresetTourResponse->PresetTour, 0, sizeof(struct tt__PresetTour));

	if(stCruiseCfg.struCruise[TourIndex].byPointNum>0)
	{
		tptz__GetPresetTourResponse->PresetTour->Name = (char *)soap_malloc(soap, 64);
		sprintf(tptz__GetPresetTourResponse->PresetTour->Name, "tour%d", TourIndex);
		tptz__GetPresetTourResponse->PresetTour->token = (char *)soap_malloc(soap, 64);
		sprintf(tptz__GetPresetTourResponse->PresetTour->token, "tour%d", TourIndex);
		tptz__GetPresetTourResponse->PresetTour->Status = 
			(struct tt__PTZPresetTourStatus *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourStatus));
		//memset(tptz__GetPresetTourResponse->PresetTour->Status, 0, sizeof(struct tt__PTZPresetTourStatus));
		if(stCruiseCfg.byIsCruising && stCruiseCfg.byCruisingIndex == TourIndex)
			tptz__GetPresetTourResponse->PresetTour->Status->State = tt__PTZPresetTourState__Touring;
		else
			tptz__GetPresetTourResponse->PresetTour->Status->State = tt__PTZPresetTourState__Idle;

		if(tptz__GetPresetTourResponse->PresetTour->Status->State == tt__PTZPresetTourState__Touring)
		{
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot = 
				(struct tt__PTZPresetTourSpot *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourSpot));
			//memset(tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot, 0, sizeof(struct tt__PTZPresetTourSpot));
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->PresetDetail = 
				(struct tt__PTZPresetTourPresetDetail *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourPresetDetail));
			//memset(tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->PresetDetail, 0, sizeof(struct tt__PTZPresetTourPresetDetail));
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->PresetDetail->__union_PTZPresetTourPresetDetail = 
				SOAP_UNION__tt__union_PTZPresetTourPresetDetail_PresetToken;
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->PresetDetail->union_PTZPresetTourPresetDetail.PresetToken = 
				(char *)soap_malloc(soap, 32);
			sprintf(tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->PresetDetail->union_PTZPresetTourPresetDetail.PresetToken,
				"%d", stCruiseCfg.struCruise[TourIndex].struCruisePoint[stCruiseCfg.byPointIndex].byPresetNo);
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->Speed = 
				(struct tt__PTZSpeed *)soap_malloc_v2(soap, sizeof(struct tt__PTZSpeed));
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->Speed->PanTilt = 
				(struct tt__Vector2D *)soap_malloc_v2(soap, sizeof(struct tt__Vector2D));
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->Speed->Zoom = 
				(struct tt__Vector1D *)soap_malloc_v2(soap, sizeof(struct tt__Vector1D));
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->Speed->PanTilt->x =
				(float)stCruiseCfg.struCruise[TourIndex].struCruisePoint[stCruiseCfg.byPointIndex].bySpeed;
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->Speed->PanTilt->y =
				(float)stCruiseCfg.struCruise[TourIndex].struCruisePoint[stCruiseCfg.byPointIndex].bySpeed;
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->Speed->Zoom->x =
				(float)stCruiseCfg.struCruise[TourIndex].struCruisePoint[stCruiseCfg.byPointIndex].bySpeed;
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->Speed->PanTilt->space =NULL;
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->Speed->Zoom->space =NULL;
			tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->StayTime = 
				(char *)soap_malloc(soap, 16);
			sprintf(tptz__GetPresetTourResponse->PresetTour->Status->CurrentTourSpot->StayTime, 
				"PT%dS", stCruiseCfg.struCruise[TourIndex].struCruisePoint[stCruiseCfg.byPointIndex].byRemainTime);
			
		}

		tptz__GetPresetTourResponse->PresetTour->AutoStart = xsd__boolean__false_;
		/*
		tptz__GetPresetTourResponse->PresetTour->StartingCondition = 
			(struct tt__PTZPresetTourStartingCondition *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourStartingCondition));
		memset(tptz__GetPresetTourResponse->PresetTour->StartingCondition, 0, sizeof(struct tt__PTZPresetTourStartingCondition));
		tptz__GetPresetTourResponse->PresetTour->StartingCondition->RecurringTime = (int *)soap_malloc(soap, sizeof(int));
		*tptz__GetPresetTourResponse->PresetTour->StartingCondition->RecurringTime = 
			stCruiseCfg.struCruise[TourIndex].struCruisePoint[stCruiseCfg.struCruise[TourIndex].byPointNum-1].byRemainTime;
		*/

		tptz__GetPresetTourResponse->PresetTour->__sizeTourSpot = stCruiseCfg.struCruise[TourIndex].byPointNum;
		tptz__GetPresetTourResponse->PresetTour->TourSpot = 
			(struct tt__PTZPresetTourSpot *)soap_malloc_v2(soap, 
			sizeof(struct tt__PTZPresetTourSpot)*tptz__GetPresetTourResponse->PresetTour->__sizeTourSpot);
		//memset(tptz__GetPresetTourResponse->PresetTour->TourSpot, 
		//	0, sizeof(struct tt__PTZPresetTourSpot)*tptz__GetPresetTourResponse->PresetTour->__sizeTourSpot);
		int presetindex;
		for(presetindex=0;presetindex<tptz__GetPresetTourResponse->PresetTour->__sizeTourSpot;presetindex++)
		{
			tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].PresetDetail = 
				(struct tt__PTZPresetTourPresetDetail *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourPresetDetail));
			//memset(tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].PresetDetail, 0, sizeof(struct tt__PTZPresetTourPresetDetail));
			tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].PresetDetail->__union_PTZPresetTourPresetDetail = 
				SOAP_UNION__tt__union_PTZPresetTourPresetDetail_PresetToken;
			tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].PresetDetail->union_PTZPresetTourPresetDetail.PresetToken = 
				(char *)soap_malloc(soap, 32);
			sprintf(tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].PresetDetail->union_PTZPresetTourPresetDetail.PresetToken,
				"%d", stCruiseCfg.struCruise[TourIndex].struCruisePoint[presetindex].byPresetNo);
			tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].Speed = 
				(struct tt__PTZSpeed *)soap_malloc_v2(soap, sizeof(struct tt__PTZSpeed));
			tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].Speed->PanTilt = 
				(struct tt__Vector2D *)soap_malloc_v2(soap, sizeof(struct tt__Vector2D));
			tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].Speed->Zoom = 
				(struct tt__Vector1D *)soap_malloc_v2(soap, sizeof(struct tt__Vector1D));
			tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].Speed->PanTilt->x =
				(float)stCruiseCfg.struCruise[TourIndex].struCruisePoint[presetindex].bySpeed;
			tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].Speed->PanTilt->y =
				(float)stCruiseCfg.struCruise[TourIndex].struCruisePoint[presetindex].bySpeed;
			tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].Speed->Zoom->x =
				(float)stCruiseCfg.struCruise[TourIndex].struCruisePoint[presetindex].bySpeed;
			tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].Speed->PanTilt->space =NULL;
			tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].Speed->Zoom->space =NULL;
			tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].StayTime = (char *)soap_malloc(soap, 16);
			sprintf(tptz__GetPresetTourResponse->PresetTour->TourSpot[presetindex].StayTime, 
				"PT%dS", stCruiseCfg.struCruise[TourIndex].struCruisePoint[presetindex].byRemainTime);
		}
	}
	
	return SOAP_OK;
}
 
int __tptz__GetPresetTourOptions(struct soap *soap, struct _tptz__GetPresetTourOptions *tptz__GetPresetTourOptions, struct _tptz__GetPresetTourOptionsResponse *tptz__GetPresetTourOptionsResponse)
{ 
	ONVIF_DBP("");
	int i, ret;
	int chno;
	char *p;

	if(!tptz__GetPresetTourOptions->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoProfile",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz__GetPresetTourOptions->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__GetPresetTourOptions->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoProfile",
				"The requested profile does not exist. "));
		}	
		p = tptz__GetPresetTourOptions->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoProfile",
				"The requested profile does not exist. "));
		}
	}

	tptz__GetPresetTourOptionsResponse->Options = 
		(struct tt__PTZPresetTourOptions *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourOptions));
	//memset(tptz__GetPresetTourOptionsResponse->Options, 0, sizeof(struct tt__PTZPresetTourOptions));
	tptz__GetPresetTourOptionsResponse->Options->AutoStart = xsd__boolean__false_;
	tptz__GetPresetTourOptionsResponse->Options->StartingCondition = 
		(struct tt__PTZPresetTourStartingConditionOptions *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourStartingConditionOptions));
	//memset(tptz__GetPresetTourOptionsResponse->Options->StartingCondition, 0, sizeof(struct tt__PTZPresetTourStartingConditionOptions));
	tptz__GetPresetTourOptionsResponse->Options->StartingCondition->RecurringTime = 
		(struct tt__IntRange *)soap_malloc_v2(soap, sizeof(struct tt__IntRange));
	tptz__GetPresetTourOptionsResponse->Options->StartingCondition->RecurringTime->Min = 32;
	tptz__GetPresetTourOptionsResponse->Options->StartingCondition->RecurringTime->Max = 1920;//60X32

	tptz__GetPresetTourOptionsResponse->Options->TourSpot = 
		(struct tt__PTZPresetTourSpotOptions *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourSpotOptions));
	//memset(tptz__GetPresetTourOptionsResponse->Options->TourSpot, 0, sizeof(struct tt__PTZPresetTourSpotOptions));

	tptz__GetPresetTourOptionsResponse->Options->TourSpot->PresetDetail = 
		(struct tt__PTZPresetTourPresetDetailOptions *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourPresetDetailOptions));
	//memset(tptz__GetPresetTourOptionsResponse->Options->TourSpot->PresetDetail, 0, sizeof(struct tt__PTZPresetTourPresetDetailOptions));

	QMAPI_NET_PRESET_INFO presetinfo;

	memset(&presetinfo, 0, sizeof(QMAPI_NET_PRESET_INFO));
	presetinfo.nChannel= chno;
	presetinfo.dwSize = sizeof(QMAPI_NET_PRESET_INFO);

	ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_ALL_PRESET, chno,&presetinfo,sizeof(QMAPI_NET_PRESET_INFO));
	if(ret<0)
	{
		printf("%s QMAPI_SYSCFG_GET_ALL_PRESET error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	tptz__GetPresetTourOptionsResponse->Options->TourSpot->PresetDetail->__sizePresetToken = presetinfo.nPresetNum;
	if(presetinfo.nPresetNum>0)
	{
		tptz__GetPresetTourOptionsResponse->Options->TourSpot->PresetDetail->PresetToken = 
			(char **)soap_malloc(soap, sizeof(char *)*presetinfo.nPresetNum);
		for(i=0;i<presetinfo.nPresetNum;i++)
		{
			tptz__GetPresetTourOptionsResponse->Options->TourSpot->PresetDetail->PresetToken[i] = 
				(char *)soap_malloc(soap, 8);
			sprintf(tptz__GetPresetTourOptionsResponse->Options->TourSpot->PresetDetail->PresetToken[i], "%d", presetinfo.no[i]);
		}
	}

	tptz__GetPresetTourOptionsResponse->Options->TourSpot->StayTime = 
		(struct tt__DurationRange *)soap_malloc_v2(soap, sizeof(struct tt__DurationRange));
	tptz__GetPresetTourOptionsResponse->Options->TourSpot->StayTime->Min = (char *)soap_malloc(soap, 8);
	tptz__GetPresetTourOptionsResponse->Options->TourSpot->StayTime->Max = (char *)soap_malloc(soap, 8);
	strcpy(tptz__GetPresetTourOptionsResponse->Options->TourSpot->StayTime->Min, "PT1S");
	strcpy(tptz__GetPresetTourOptionsResponse->Options->TourSpot->StayTime->Min, "PT60S");
	
	return SOAP_OK;
}
 
int __tptz__CreatePresetTour(struct soap *soap, struct _tptz__CreatePresetTour *tptz__CreatePresetTour, struct _tptz__CreatePresetTourResponse *tptz__CreatePresetTourResponse)
{	 
	ONVIF_DBP("");
	int i, ret;
	int chno;
	char *p;

	if(!tptz__CreatePresetTour->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoProfile",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz__CreatePresetTour->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__CreatePresetTour->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz__CreatePresetTour->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}

	QMAPI_NET_CRUISE_CFG stCruiseCfg;
	memset(&stCruiseCfg, 0, sizeof(stCruiseCfg));
	stCruiseCfg.dwSize = sizeof(stCruiseCfg);
	stCruiseCfg.nChannel = chno;

	ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_CRUISE_CFG, chno,&stCruiseCfg,sizeof(QMAPI_NET_CRUISE_CFG));
	if(ret<0)
	{
		printf("%s, QMAPI_SYSCFG_GET_CRUISE_CFG failed!\n", __FUNCTION__);
		return SOAP_ERR;
	}

	tptz__CreatePresetTourResponse->PresetTourToken = NULL;

	int find=0;
	for(i=0;i<QMAPI_MAX_CRUISE_GROUP_NUM;i++)
	{
		if(stCruiseCfg.struCruise[i].byPointNum == 0 && stCruiseCfg.struCruise[i].byRes[0] == 0)
		{
			tptz__CreatePresetTourResponse->PresetTourToken = (char *)soap_malloc(soap,16);
			sprintf(tptz__CreatePresetTourResponse->PresetTourToken, "tour%d", i);
			find = 1;
			break;
		}
	}

	if(!find)
	{
		return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:TooManyPresetTours",
				"There is not enough space in the device to create the new preset tour to the profile"));
	}

	QMAPI_NET_CRUISE_INFO stCruiseInfo;
	memset(&stCruiseInfo, 0, sizeof(stCruiseInfo));

	stCruiseInfo.byRes[0] = 1;
	stCruiseInfo.byCruiseIndex = i;

	ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_CRUISE_INFO, chno, &stCruiseInfo, sizeof(QMAPI_NET_CRUISE_INFO));
	if(ret)
	{
		printf("%s, QMAPI_SYSCFG_SET_CRUISE_INFO failed!\n",__FUNCTION__);
	}
	
	return SOAP_OK;
} 
 
int __tptz__ModifyPresetTour(struct soap *soap, struct _tptz__ModifyPresetTour *tptz__ModifyPresetTour, struct _tptz__ModifyPresetTourResponse *tptz__ModifyPresetTourResponse)
{	 
	ONVIF_DBP("");

	int i, ret;
	int chno;
	char *p;

	if(!tptz__ModifyPresetTour->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoProfile",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz__ModifyPresetTour->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__ModifyPresetTour->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoProfile",
				"The requested profile does not exist. "));
		}	
		p = tptz__ModifyPresetTour->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoProfile",
				"The requested profile does not exist. "));
		}
	}

	int TourIndex=0;
	if(!tptz__ModifyPresetTour->PresetTour || 
		!tptz__ModifyPresetTour->PresetTour->token ||
		sscanf(tptz__ModifyPresetTour->PresetTour->token, "tour%d", &TourIndex) != 1 ||
		TourIndex<0 || TourIndex>=QMAPI_MAX_CRUISE_GROUP_NUM)
	{
		printf("%s, Invalid parameter!\n",__FUNCTION__);
		return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoToken",
				"The requested PresetTourToken does not exist."));
	}

	if(tptz__ModifyPresetTour->PresetTour->__sizeTourSpot<0 
		|| tptz__ModifyPresetTour->PresetTour->__sizeTourSpot>=QMAPI_MAX_CRUISE_POINT_NUM)
	{
		printf("%s, Too many TourSpots are included in the PresetTour!num:%d\n",__FUNCTION__,tptz__ModifyPresetTour->PresetTour->__sizeTourSpot);
		return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Receiver",
				"ter:InvalidArgVal",
				"ter:TooManyPresets",
				"Too many TourSpots are included in the PresetTour."));
	}

	QMAPI_NET_CRUISE_INFO stCruiseInfo;
	memset(&stCruiseInfo, 0, sizeof(stCruiseInfo));
	stCruiseInfo.byCruiseIndex = TourIndex;
	stCruiseInfo.byPointNum = tptz__ModifyPresetTour->PresetTour->__sizeTourSpot;

	int PresetNo = 0;
	for(i=0;i<tptz__ModifyPresetTour->PresetTour->__sizeTourSpot;i++)
	{
		if(!tptz__ModifyPresetTour->PresetTour->TourSpot[i].PresetDetail  
			//|| tptz__ModifyPresetTour->PresetTour->TourSpot[i].PresetDetail->__union_PTZPresetTourPresetDetail!= 
			//	SOAP_UNION__tt__union_PTZPresetTourPresetDetail_PresetToken 
			|| !tptz__ModifyPresetTour->PresetTour->TourSpot[i].PresetDetail->union_PTZPresetTourPresetDetail.PresetToken)
		{
			printf("%s, Invalid TourSpots type!\n",__FUNCTION__);
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:InvalidPresetTour",
				"The suggested PresetTour includes  invalid parameter(s)"));
		}

		PresetNo = atoi(tptz__ModifyPresetTour->PresetTour->TourSpot[i].PresetDetail->union_PTZPresetTourPresetDetail.PresetToken);
		if(PresetNo<=0 || PresetNo>QMAPI_MAX_PRESET)
		{
			printf("%s, Invalid PresetNo:%d!\n",__FUNCTION__,PresetNo);
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:InvalidPresetTour",
				"The suggested PresetTour includes  invalid parameter(s)"));
		}
		
		stCruiseInfo.struCruisePoint[i].byPresetNo = PresetNo;

		if(tptz__ModifyPresetTour->PresetTour->TourSpot[i].Speed)
		{
			if(tptz__ModifyPresetTour->PresetTour->TourSpot[i].Speed->PanTilt)
			{
				if(tptz__ModifyPresetTour->PresetTour->TourSpot[i].Speed->PanTilt->x>1.0 && 
					tptz__ModifyPresetTour->PresetTour->TourSpot[i].Speed->PanTilt->x<8.0)
				{
					stCruiseInfo.struCruisePoint[i].bySpeed = 
						(int)tptz__ModifyPresetTour->PresetTour->TourSpot[i].Speed->PanTilt->x;
				}
				else
				{
					stCruiseInfo.struCruisePoint[i].bySpeed = 4;
				}
			}
			else if(tptz__ModifyPresetTour->PresetTour->TourSpot[i].Speed->Zoom)
			{
				if(tptz__ModifyPresetTour->PresetTour->TourSpot[i].Speed->Zoom->x>1.0 && 
					tptz__ModifyPresetTour->PresetTour->TourSpot[i].Speed->Zoom->x<8.0)
				{
					stCruiseInfo.struCruisePoint[i].bySpeed = 
						(int)tptz__ModifyPresetTour->PresetTour->TourSpot[i].Speed->Zoom->x;
				}
				else
				{
					stCruiseInfo.struCruisePoint[i].bySpeed = 4;
				}
			}
			else
			{
				stCruiseInfo.struCruisePoint[i].bySpeed = 4;
			}
		}
		else
		{
			stCruiseInfo.struCruisePoint[i].bySpeed = 4;
		}

		if(tptz__ModifyPresetTour->PresetTour->TourSpot[i].StayTime)
		{
			long time = 0;
			time = periodtol(tptz__ModifyPresetTour->PresetTour->TourSpot[i].StayTime);
			if(time<1 || time> 60)
				stCruiseInfo.struCruisePoint[i].byRemainTime = 10;
			else
				stCruiseInfo.struCruisePoint[i].byRemainTime = time;
		}
		else
		{
			stCruiseInfo.struCruisePoint[i].byRemainTime = 10;
		}
	}

	ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_CRUISE_INFO, chno, &stCruiseInfo, sizeof(QMAPI_NET_CRUISE_INFO));
	if(ret)
	{
		printf("%s, QMAPI_SYSCFG_SET_CRUISE_INFO failed!\n",__FUNCTION__);
	}
	
	return SOAP_OK;
}  

int __tptz__OperatePresetTour(struct soap *soap, struct _tptz__OperatePresetTour *tptz__OperatePresetTour, struct _tptz__OperatePresetTourResponse *tptz__OperatePresetTourResponse)
{	
	ONVIF_DBP("");

	int ret;
	int chno;
	char *p;

	if(!tptz__OperatePresetTour->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoProfile",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz__OperatePresetTour->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__OperatePresetTour->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoProfile",
				"The requested profile does not exist. "));
		}	
		p = tptz__OperatePresetTour->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoProfile",
				"The requested profile does not exist. "));
		}
	}

	int TourIndex=0;
	if(!tptz__OperatePresetTour->PresetTourToken || 
		sscanf(tptz__OperatePresetTour->PresetTourToken, "tour%d", &TourIndex) != 1 ||
		TourIndex<0 || TourIndex>=QMAPI_MAX_CRUISE_GROUP_NUM)
	{
		printf("%s, Invalid PresetTourToken!\n",__FUNCTION__);
		return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoToken",
				"The requested PresetTourToken does not exist."));
	}

	QMAPI_NET_PTZ_CONTROL ptz_ctrl;
	memset(&ptz_ctrl, 0, sizeof(QMAPI_NET_PTZ_CONTROL));

	if(tptz__OperatePresetTour->Operation == tt__PTZPresetTourOperation__Start)
		ptz_ctrl.dwCommand = QMAPI_PTZ_STA_START_CRU;
	else if(tptz__OperatePresetTour->Operation == tt__PTZPresetTourOperation__Stop)
		ptz_ctrl.dwCommand = QMAPI_PTZ_STA_STOP_CRU;
	else
	{
		return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:ActivationFailed",
				"The requested operation is not supported."));
	}
	
	ptz_ctrl.dwSize = sizeof(QMAPI_NET_PTZ_CONTROL);
	ptz_ctrl.nChannel = chno;
	ptz_ctrl.dwParam = TourIndex;

	ret = QMapi_sys_ioctrl(0,QMAPI_NET_STA_PTZ_CONTROL, chno,&ptz_ctrl,sizeof(QMAPI_NET_PTZ_CONTROL));
	if(ret<0)
	{
		printf("%s QMAPI_NET_STA_PTZ_CONTROL failed!\n", __FUNCTION__);
		return SOAP_ERR;
	}
	
	return SOAP_OK;
}
  
int __tptz__RemovePresetTour(struct soap *soap, struct _tptz__RemovePresetTour *tptz__RemovePresetTour, struct _tptz__RemovePresetTourResponse *tptz__RemovePresetTourResponse)
{	 	
	ONVIF_DBP("");

	int ret;
	int chno;
	char *p;

	if(!tptz__RemovePresetTour->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoProfile",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz__RemovePresetTour->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__RemovePresetTour->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoProfile",
				"The requested profile does not exist. "));
		}	
		p = tptz__RemovePresetTour->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoProfile",
				"The requested profile does not exist. "));
		}
	}

	int TourIndex=0;
	if(!tptz__RemovePresetTour->PresetTourToken || 
		sscanf(tptz__RemovePresetTour->PresetTourToken, "tour%d", &TourIndex) != 1 ||
		TourIndex<0 || TourIndex>=QMAPI_MAX_CRUISE_GROUP_NUM)
	{
		printf("%s, Invalid PresetTourToken!\n",__FUNCTION__);
		return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoToken",
				"The requested PresetTourToken does not exist."));
	}

	QMAPI_NET_CRUISE_INFO stCruiseInfo;
	memset(&stCruiseInfo, 0, sizeof(stCruiseInfo));
	stCruiseInfo.byCruiseIndex = TourIndex;

	ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_CRUISE_INFO, chno, &stCruiseInfo, sizeof(QMAPI_NET_CRUISE_INFO));
	if(ret)
	{
		printf("%s, QMAPI_SYSCFG_SET_CRUISE_INFO failed!\n",__FUNCTION__);
	}
	
	return SOAP_OK;
}


int  __tptz__GetConfigurations(struct soap *soap, 
	 struct _tptz__GetConfigurations *tptz__GetConfigurations, 
	 struct _tptz__GetConfigurationsResponse *tptz__GetConfigurationsResponse)
{ 
    ONVIF_DBP(""); 

    tptz__GetConfigurationsResponse->__sizePTZConfiguration = soap->pConfig->g_channel_num;

	tptz__GetConfigurationsResponse->PTZConfiguration = soap_malloc_v2(soap,sizeof(struct tt__PTZConfiguration)*soap->pConfig->g_channel_num);

	int i;
	for(i=0;i<soap->pConfig->g_channel_num;i++)
	{
		tptz__GetConfigurationsResponse->PTZConfiguration[i].Name = (char *)soap_malloc(soap,32);
		sprintf(tptz__GetConfigurationsResponse->PTZConfiguration[i].Name,"Anv_ptz_%d",i);
		
		tptz__GetConfigurationsResponse->PTZConfiguration[i].token = (char *)soap_malloc(soap,32);
		sprintf(tptz__GetConfigurationsResponse->PTZConfiguration[i].token,"Anv_ptz_%d",i);
		
		tptz__GetConfigurationsResponse->PTZConfiguration[i].UseCount = MAX_STREAM_NUM;
		
		tptz__GetConfigurationsResponse->PTZConfiguration[i].NodeToken = (char *)soap_malloc(soap,32);
		sprintf(tptz__GetConfigurationsResponse->PTZConfiguration[i].NodeToken,"Anv_ptz_%d",i);

		tptz__GetConfigurationsResponse->PTZConfiguration[i].DefaultContinuousPanTiltVelocitySpace = 
			(char *)soap_malloc(soap,128);
		strcpy(tptz__GetConfigurationsResponse->PTZConfiguration[i].DefaultContinuousPanTiltVelocitySpace,
			"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
		
		tptz__GetConfigurationsResponse->PTZConfiguration[i].DefaultContinuousZoomVelocitySpace = 
			(char *)soap_malloc(soap,128);
		strcpy(tptz__GetConfigurationsResponse->PTZConfiguration[i].DefaultContinuousZoomVelocitySpace,
			"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");
		
		tptz__GetConfigurationsResponse->PTZConfiguration[i].DefaultPTZTimeout= 
			(char *)soap_malloc(soap, 8);
		strcpy(tptz__GetConfigurationsResponse->PTZConfiguration[i].DefaultPTZTimeout,"PT1S");
	
	}

    return SOAP_OK;
}


int  __tptz__GetPresets(struct soap *soap, 
 	struct _tptz__GetPresets *tptz__GetPresets, 
 	struct _tptz__GetPresetsResponse *tptz__GetPresetsResponse)
{ 
	ONVIF_DBP("");
	
	int i, ret;
	int chno;
	char *p,buf[64];
	QMAPI_NET_PRESET_INFO presetinfo;

	memset(&presetinfo, 0, sizeof(QMAPI_NET_PRESET_INFO));

	if(!tptz__GetPresets->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoProfile",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz__GetPresets->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__GetPresets->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz__GetPresets->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}

	presetinfo.nChannel= chno;
	presetinfo.dwSize = sizeof(QMAPI_NET_PRESET_INFO);

	ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_ALL_PRESET, chno,&presetinfo,sizeof(QMAPI_NET_PRESET_INFO));
	if(ret<0)
	{
		printf("%s QMAPI_SYSCFG_GET_ALL_PRESET error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}
	
	tptz__GetPresetsResponse->__sizePreset = presetinfo.nPresetNum;
	printf("%s  presetNum=%d\n",__FUNCTION__,presetinfo.nPresetNum);
	tptz__GetPresetsResponse->Preset = 
		(struct tt__PTZPreset *)soap_malloc_v2(soap,sizeof(struct tt__PTZPreset)*(presetinfo.nPresetNum));
	//memset(tptz__GetPresetsResponse->Preset,0,sizeof(struct tt__PTZPreset)*(presetinfo.nPresetNum));

	for(i = 0;i<presetinfo.nPresetNum;i++)
	{
		tptz__GetPresetsResponse->Preset[i].Name = (char *)soap_malloc(soap,128);
		
		if(strlen(presetinfo.csName[i])<1)
		{
			sprintf(buf,"preset%d",presetinfo.no[i]);
			strcpy(tptz__GetPresetsResponse->Preset[i].Name, buf);
		}
		else
			strcpy(tptz__GetPresetsResponse->Preset[i].Name, presetinfo.csName[i]);

		#if 0
		tptz__GetPresetsResponse->Preset[i].PTZPosition = 
			(struct tt__PTZVector *)soap_malloc_v2(soap,sizeof(struct tt__PTZVector));
		
		tptz__GetPresetsResponse->Preset[i].PTZPosition->PanTilt = 
			(struct tt__Vector2D *)soap_malloc_v2(soap,sizeof(struct tt__Vector2D));
		tptz__GetPresetsResponse->Preset[i].PTZPosition->PanTilt->x = 0.25;
		tptz__GetPresetsResponse->Preset[i].PTZPosition->PanTilt->y = 0.25;
		tptz__GetPresetsResponse->Preset[i].PTZPosition->PanTilt->space = 
			(char *)soap_malloc(soap,128);
		memset(tptz__GetPresetsResponse->Preset[i].PTZPosition->PanTilt->space,0,128);
		strcpy(tptz__GetPresetsResponse->Preset[i].PTZPosition->PanTilt->space, 
					"http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
		
		tptz__GetPresetsResponse->Preset[i].PTZPosition->Zoom= 
			(struct tt__Vector1D *)soap_malloc_v2(soap,sizeof(struct tt__Vector1D));
		tptz__GetPresetsResponse->Preset[i].PTZPosition->Zoom->x = 0.25;
		tptz__GetPresetsResponse->Preset[i].PTZPosition->Zoom->space = 
			(char *)soap_malloc(soap,128);
		memset(tptz__GetPresetsResponse->Preset[i].PTZPosition->Zoom->space,0,128);
		strcpy(tptz__GetPresetsResponse->Preset[i].PTZPosition->Zoom->space, 
					"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");
		#endif
		
		tptz__GetPresetsResponse->Preset[i].token = (char *)soap_malloc(soap,128);
		
		sprintf(tptz__GetPresetsResponse->Preset[i].token, "%d", presetinfo.no[i]);
		//printf("%s ,presetNum=%d,no=%d,name=%s\n",__FUNCTION__,presetinfo.presetNum,presetinfo.no[i],presetinfo.csName[i]);
	}

	return SOAP_OK;
}



int  __tptz__SetPreset(struct soap *soap, 
 	struct _tptz__SetPreset *tptz__SetPreset, 
 	struct _tptz__SetPresetResponse *tptz__SetPresetResponse)
{ 
	ONVIF_DBP("");

	int point = 0;

	int ret, chno;
	char *p, buf[64]={0};
	QMAPI_NET_PRESET_INFO presetinfo;

	memset(&presetinfo, 0, sizeof(QMAPI_NET_PRESET_INFO));

	if(!tptz__SetPreset->ProfileToken)
	{
		chno = 0;
		printf("ProfileToken is NULL!\n");
	}
	else if(strncmp(tptz__SetPreset->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__SetPreset->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz__SetPreset->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}

	if(!tptz__SetPreset->PresetToken)
	{
		int i,j,used=0;
		presetinfo.nChannel= chno;
		presetinfo.dwSize = sizeof(QMAPI_NET_PRESET_INFO);

		ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_ALL_PRESET, chno,&presetinfo,sizeof(QMAPI_NET_PRESET_INFO));
		if(ret<0)
		{
			printf("%s dms:%d error!ret=%d\n", __FUNCTION__, QMAPI_SYSCFG_GET_ALL_PRESET, ret);
			return SOAP_ERR;
		}

		for(j=1;j<QMAPI_MAX_PRESET;j++)
		{
			used=0;
			for(i=0;i<presetinfo.nPresetNum;i++)
			{
				if(j ==presetinfo.no[i])
				{
					used = 1;
					break;
				}
			}

			if(!used)
				break;
		}

		if(used)
		{
			printf("%s ,cannot set more preset!\n", __FUNCTION__);
			return SOAP_ERR;
		}

		point = j;
		//printf("%s 00000000000,presetNum=%d,point=%d\n",__FUNCTION__,presetinfo.presetNum,point);
	}
	else
	{
		point = atoi(tptz__SetPreset->PresetToken);
	}

	QMAPI_NET_PTZ_CONTROL ptz_ctrl;
	if(!tptz__SetPreset->PresetName)
	{
		sprintf(buf,"preset%d",point);
		strcpy((char *)ptz_ctrl.byRes, buf);
	}
	else
		strcpy((char *)ptz_ctrl.byRes, tptz__SetPreset->PresetName);

	ptz_ctrl.dwSize = sizeof(QMAPI_NET_PTZ_CONTROL);
	ptz_ctrl.nChannel = chno;
	ptz_ctrl.dwCommand = QMAPI_PTZ_STA_PRESET;
	ptz_ctrl.dwParam = point;

	ret = QMapi_sys_ioctrl(0,QMAPI_NET_STA_PTZ_CONTROL, chno,&ptz_ctrl,sizeof(QMAPI_NET_PTZ_CONTROL));
	if(ret<0)
	{
		printf("%s QMAPI_NET_STA_PTZ_CONTROL error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	printf("Set preset, point:%d, name:%s\n", point,ptz_ctrl.byRes);

	tptz__SetPresetResponse->PresetToken = (char *)soap_malloc(soap,16);
	sprintf(tptz__SetPresetResponse->PresetToken, "%d", point);
	//printf("%s 11111111,point=%d,name=%s\n",__FUNCTION__,point,ptz_ctrl.byRes);
	
	return SOAP_OK;
}



int  __tptz__RemovePreset(struct soap *soap, 
 	struct _tptz__RemovePreset *tptz__RemovePreset, 
 	struct _tptz__RemovePresetResponse *tptz__RemovePresetResponse)
{ 

	ONVIF_DBP("");

	int point = 0;
	int find = 0;
	int i,ret, chno;
	char *p;
	QMAPI_NET_PTZ_CONTROL ptz_ctrl;
	QMAPI_NET_PRESET_INFO presetinfo;

	if(!tptz__RemovePreset->ProfileToken)
	{
		chno = 0;
		printf("ProfileToken is NULL!\n");
	}
	else if(strncmp(tptz__RemovePreset->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__RemovePreset->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoProfile",
				"The requested profile does not exist. "));
		}	
		p = tptz__RemovePreset->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}
	
	if(!tptz__RemovePreset->PresetToken)
	{
		return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoToken",
				"The requested preset token does not exist."));
	}

	point = atoi(tptz__RemovePreset->PresetToken);

	
	printf("ch is %d\n", chno);
	memset(&presetinfo, 0, sizeof(QMAPI_NET_PRESET_INFO));

	presetinfo.nChannel= chno;
	presetinfo.dwSize = sizeof(QMAPI_NET_PRESET_INFO);

	ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_ALL_PRESET, chno,&presetinfo,sizeof(QMAPI_NET_PRESET_INFO));
	if(ret<0)
	{
		printf("%s QMAPI_SYSCFG_GET_ALL_PRESET error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	printf("nodenum is %d\n", presetinfo.nPresetNum);
	
	for(i = 0;i<presetinfo.nPresetNum;i++)
	{
		printf("remove node is %d cur node is %d\n", point, presetinfo.no[i]);
		if(point == presetinfo.no[i])
		{
			find = 1;
			break;
		}
	}

	if(!find)
		return SOAP_OK;

	printf("%s  %d, remove preset:%d\n",__FUNCTION__, __LINE__,point);
	memset(&ptz_ctrl, 0, sizeof(ptz_ctrl));
	ptz_ctrl.dwSize = sizeof(ptz_ctrl);
	ptz_ctrl.dwParam = point;
	ptz_ctrl.nChannel = chno;
	ptz_ctrl.dwCommand = QMAPI_PTZ_STA_PRESET_CLS;
	ret = QMapi_sys_ioctrl(0,QMAPI_NET_STA_PTZ_CONTROL, chno,&ptz_ctrl,sizeof(QMAPI_NET_PTZ_CONTROL));
	if(ret<0)
	{
		printf("%s QMAPI_NET_STA_PTZ_CONTROL error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}
	
	return SOAP_OK;
}

int  __tptz__GotoPreset(struct soap *soap, 
 	struct _tptz__GotoPreset *tptz__GotoPreset, 
 	struct _tptz__GotoPresetResponse *tptz__GotoPresetResponse)
{ 
	ONVIF_DBP("  ");

	int point = 0;

	int ret, chno;
	char *p;
	QMAPI_NET_PRESET_INFO presetinfo;

	memset(&presetinfo, 0, sizeof(QMAPI_NET_PRESET_INFO));

	if(!tptz__GotoPreset->ProfileToken)
	{
		chno = 0;
		printf("ProfileToken is NULL!\n");
	}
	else if(strncmp(tptz__GotoPreset->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__GotoPreset->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz__GotoPreset->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}
	
	if(!tptz__GotoPreset->PresetToken)
	{
		return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoToken",
				"The requested profile does not exist. "));
	}

	point = atoi(tptz__GotoPreset->PresetToken);

	QMAPI_NET_PTZ_CONTROL ptz_ctrl;
	memset(&ptz_ctrl, 0, sizeof(QMAPI_NET_PTZ_CONTROL));

	ptz_ctrl.dwSize = sizeof(QMAPI_NET_PTZ_CONTROL);
	ptz_ctrl.nChannel = chno;
	ptz_ctrl.dwCommand = QMAPI_PTZ_STA_CALL;
	ptz_ctrl.dwParam = point;

	ret = QMapi_sys_ioctrl(0,QMAPI_NET_STA_PTZ_CONTROL, chno,&ptz_ctrl,sizeof(QMAPI_NET_PTZ_CONTROL));
	if(ret<0)
	{
		printf("%s QMAPI_NET_STA_PTZ_CONTROL error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}
	
	return SOAP_OK;
}

int  __tptz__GetStatus(struct soap *soap, 
 	struct _tptz__GetStatus *tptz__GetStatus, 
 	struct _tptz__GetStatusResponse *tptz__GetStatusResponse)
{ 
	ONVIF_DBP("");	
	tptz__GetStatusResponse->PTZStatus = 
		(struct tt__PTZStatus *)soap_malloc_v2(soap, sizeof(struct tt__PTZStatus));
	//memset(tptz__GetStatusResponse->PTZStatus, 0, sizeof(struct tt__PTZStatus));
#if 1
	int nRes = 0;
	QMAPI_NET_PTZ_POS   stNetPtzPos;
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PTZPOS, 0, &stNetPtzPos, sizeof(QMAPI_NET_PTZ_POS));
	if(nRes == 0)
	{
		tptz__GetStatusResponse->PTZStatus = 
			(struct tt__PTZStatus *)soap_malloc_v2(soap, sizeof(struct tt__PTZStatus));
		//memset(tptz__GetStatusResponse->PTZStatus, 0, sizeof(struct tt__PTZStatus));
		
		tptz__GetStatusResponse->PTZStatus->Position = 
			(struct tt__PTZVector *)soap_malloc_v2(soap, sizeof(struct tt__PTZVector));
		
		tptz__GetStatusResponse->PTZStatus->Position->PanTilt = 
			(struct tt__Vector2D *)soap_malloc_v2(soap,sizeof(struct tt__Vector2D));
		//memset(tptz__GetStatusResponse->PTZStatus->Position->PanTilt,0,sizeof(struct tt__Vector2D));
		tptz__GetStatusResponse->PTZStatus->Position->PanTilt->x = stNetPtzPos.nPanPos;
		tptz__GetStatusResponse->PTZStatus->Position->PanTilt->y = stNetPtzPos.nTiltPos;
		tptz__GetStatusResponse->PTZStatus->Position->PanTilt->space = 
			(char *)soap_malloc(soap,128);
		//memset(tptz__GetStatusResponse->PTZStatus->Position->PanTilt->space,0,128);
		strcpy(tptz__GetStatusResponse->PTZStatus->Position->PanTilt->space,
			"http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
		
		tptz__GetStatusResponse->PTZStatus->Position->Zoom= 
			(struct tt__Vector1D *)soap_malloc_v2(soap,sizeof(struct tt__Vector1D));
		//memset(tptz__GetStatusResponse->PTZStatus->Position->Zoom,0,sizeof(struct tt__Vector1D));
		tptz__GetStatusResponse->PTZStatus->Position->Zoom->x = stNetPtzPos.nZoomPos;
		tptz__GetStatusResponse->PTZStatus->Position->Zoom->space = 
			(char *)soap_malloc(soap,128);
		//memset(tptz__GetStatusResponse->PTZStatus->Position->Zoom->space,0,128);
		strcpy(tptz__GetStatusResponse->PTZStatus->Position->Zoom->space, 
			"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");
			
		tptz__GetStatusResponse->PTZStatus->MoveStatus = 
			(struct tt__PTZMoveStatus *)soap_malloc_v2(soap, sizeof(struct tt__PTZMoveStatus));
		
		tptz__GetStatusResponse->PTZStatus->MoveStatus->PanTilt = 
			(enum tt__MoveStatus *)soap_malloc(soap,sizeof(enum tt__MoveStatus));
		//memset(tptz__GetStatusResponse->PTZStatus->MoveStatus->PanTilt,0,sizeof(enum tt__MoveStatus));
		*(tptz__GetStatusResponse->PTZStatus->MoveStatus->PanTilt) = tt__MoveStatus__IDLE;
		
		tptz__GetStatusResponse->PTZStatus->MoveStatus->Zoom= 
			(enum tt__MoveStatus *)soap_malloc(soap,sizeof(enum tt__MoveStatus));
		//memset(tptz__GetStatusResponse->PTZStatus->MoveStatus->Zoom,0,sizeof(enum tt__MoveStatus));
		*(tptz__GetStatusResponse->PTZStatus->MoveStatus->Zoom) = tt__MoveStatus__IDLE; //tt__MoveStatus__MOVING;
		}
#endif		
	tptz__GetStatusResponse->PTZStatus->UtcTime=time((time_t *)NULL);

	return SOAP_OK;
}

int  __tptz__GetConfiguration(struct soap *soap, 
 	struct _tptz__GetConfiguration *tptz__GetConfiguration, 
 	struct _tptz__GetConfigurationResponse *tptz__GetConfigurationResponse)
{
	ONVIF_DBP("");

	int chno;
	char *p;
	/*
	if(!tptz__GetConfiguration->PTZConfigurationToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}
	
	if(0!=strncmp(tptz__GetConfiguration->PTZConfigurationToken,"Anv_ptz_", strlen("Anv_ptz_")))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}	
	*/
	if(tptz__GetConfiguration->PTZConfigurationToken
		&& !strncmp(tptz__GetConfiguration->PTZConfigurationToken,"Anv_ptz_", strlen("Anv_ptz_")))
	{
		p = tptz__GetConfiguration->PTZConfigurationToken+strlen("Anv_ptz_");
		chno = atoi(p);
	}
	else
		chno = 0;
	
	if(soap->pConfig->g_channel_num<(chno+1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}
	
	tptz__GetConfigurationResponse->PTZConfiguration = soap_malloc_v2(soap,sizeof(struct tt__PTZConfiguration));
	
	tptz__GetConfigurationResponse->PTZConfiguration->Name = (char *)soap_malloc(soap,32);
	sprintf(tptz__GetConfigurationResponse->PTZConfiguration->Name,"Anv_ptz_%d",chno);
	
	tptz__GetConfigurationResponse->PTZConfiguration->token = (char *)soap_malloc(soap,32);
	sprintf(tptz__GetConfigurationResponse->PTZConfiguration->token,"Anv_ptz_%d",chno);
	
	tptz__GetConfigurationResponse->PTZConfiguration->UseCount = MAX_STREAM_NUM;
	
	tptz__GetConfigurationResponse->PTZConfiguration->NodeToken = (char *)soap_malloc(soap,32);
	sprintf(tptz__GetConfigurationResponse->PTZConfiguration->NodeToken,"Anv_ptz_%d",chno);

	tptz__GetConfigurationResponse->PTZConfiguration->DefaultContinuousPanTiltVelocitySpace = 
		(char *)soap_malloc(soap,128);
	strcpy(tptz__GetConfigurationResponse->PTZConfiguration->DefaultContinuousPanTiltVelocitySpace,
		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
	
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultContinuousZoomVelocitySpace = 
		(char *)soap_malloc(soap,128);
	strcpy(tptz__GetConfigurationResponse->PTZConfiguration->DefaultContinuousZoomVelocitySpace,
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");
	tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZTimeout= 
		(char *)soap_malloc(soap, 8);
	strcpy(tptz__GetConfigurationResponse->PTZConfiguration->DefaultPTZTimeout,"PT1S");
	
	return SOAP_OK; 
}


int  __tptz__GetNodes(struct soap *soap, 
	 struct _tptz__GetNodes *tptz__GetNodes, 
	 struct _tptz__GetNodesResponse *tptz__GetNodesResponse)
{
	ONVIF_DBP("");

	int i;
	tptz__GetNodesResponse->__sizePTZNode = soap->pConfig->g_channel_num;
	
	tptz__GetNodesResponse->PTZNode = (struct tt__PTZNode *)soap_malloc_v2(soap, sizeof(struct tt__PTZNode)*soap->pConfig->g_channel_num);
	//memset(tptz__GetNodesResponse->PTZNode,0,sizeof(struct tt__PTZNode)*soap->pConfig->g_channel_num);

	for(i=0;i<soap->pConfig->g_channel_num;i++)
	{
		tptz__GetNodesResponse->PTZNode[i].token = (char *)soap_malloc(soap,32);
		//memset(tptz__GetNodesResponse->PTZNode[i].token,0,32);
		sprintf(tptz__GetNodesResponse->PTZNode[i].token,"Anv_ptz_%d",i);
		
		tptz__GetNodesResponse->PTZNode[i].Name = (char *)soap_malloc(soap,32);
		//memset(tptz__GetNodesResponse->PTZNode[i].Name,0,32);
		sprintf(tptz__GetNodesResponse->PTZNode[i].Name,"Anv_ptz_%d",i);
		
		tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces = 
			(struct tt__PTZSpaces *)soap_malloc_v2(soap, sizeof(struct tt__PTZSpaces));
		//memset(tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces,0,sizeof(struct tt__PTZSpaces));

		tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->__sizeContinuousPanTiltVelocitySpace = 2;
		tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousPanTiltVelocitySpace = 
			(struct tt__Space2DDescription *)soap_malloc_v2(soap, sizeof(struct tt__Space2DDescription)*2);
		//memset(tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousPanTiltVelocitySpace,
		//	0,sizeof(struct tt__Space2DDescription)*2);

		int j;
		for(j=0;j<2;j++)
		{
			tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[j].URI = 
				(char *)soap_malloc(soap, 128);
			//memset(tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[j].URI,0,128);
			//if(i==0)
				strcpy(tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[j].URI,
					"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
			//else
			//	strcpy(tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[j].URI,
			//		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocitySpaceFOV");
			
			tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[j].XRange = 
				(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
			tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[j].XRange->Max = 1.0;
			tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[j].XRange->Min = -1.0;

			tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[j].YRange = 
				(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
			tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[j].YRange->Max = 1.0;
			tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[j].YRange->Min = -1.0;
		}

		tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->__sizeContinuousZoomVelocitySpace = 2;
		tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousZoomVelocitySpace = 
			(struct tt__Space1DDescription *)soap_malloc_v2(soap, sizeof(struct tt__Space1DDescription)*2);
		//memset(tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousZoomVelocitySpace,
		//	0,sizeof(struct tt__Space1DDescription)*2);

		for(j=0;j<2;j++)
		{
			tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousZoomVelocitySpace[j].URI = 
				(char *)soap_malloc(soap, 128);
			//memset(tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousZoomVelocitySpace[j].URI,0,128);
			//if(i==0)
				strcpy(tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousZoomVelocitySpace[j].URI,
					"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");
			//else
			//	strcpy(tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousZoomVelocitySpace[j].URI,
			//		"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocitySpaceMillimeter");
			
			tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousZoomVelocitySpace[j].XRange = 
				(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
			tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousZoomVelocitySpace[j].XRange->Max = 1.0;
			tptz__GetNodesResponse->PTZNode[i].SupportedPTZSpaces->ContinuousZoomVelocitySpace[j].XRange->Min = -1.0;
		}

		tptz__GetNodesResponse->PTZNode[i].MaximumNumberOfPresets = QMAPI_MAX_PRESET;
		tptz__GetNodesResponse->PTZNode[i].HomeSupported = xsd__boolean__false_;
		tptz__GetNodesResponse->PTZNode[i].__sizeAuxiliaryCommands = 0;

		tptz__GetNodesResponse->PTZNode[i].Extension = (struct tt__PTZNodeExtension *)soap_malloc_v2(soap, sizeof(struct tt__PTZNodeExtension));
		//memset(tptz__GetNodesResponse->PTZNode[i].Extension, 0, sizeof(struct tt__PTZNodeExtension));
		tptz__GetNodesResponse->PTZNode[i].Extension->SupportedPresetTour = 
			(struct tt__PTZPresetTourSupported *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourSupported));
		//memset(tptz__GetNodesResponse->PTZNode[i].Extension->SupportedPresetTour, 0, sizeof(struct tt__PTZPresetTourSupported));
		tptz__GetNodesResponse->PTZNode[i].Extension->SupportedPresetTour->MaximumNumberOfPresetTours = QMAPI_MAX_CRUISE_GROUP_NUM;
		tptz__GetNodesResponse->PTZNode[i].Extension->SupportedPresetTour->__sizePTZPresetTourOperation = 2;
		tptz__GetNodesResponse->PTZNode[i].Extension->SupportedPresetTour->PTZPresetTourOperation = 
			(enum tt__PTZPresetTourOperation *)soap_malloc_v2(soap, sizeof(enum tt__PTZPresetTourOperation)*2);
		tptz__GetNodesResponse->PTZNode[i].Extension->SupportedPresetTour->PTZPresetTourOperation[0] = tt__PTZPresetTourOperation__Start;
	 	tptz__GetNodesResponse->PTZNode[i].Extension->SupportedPresetTour->PTZPresetTourOperation[1] = tt__PTZPresetTourOperation__Stop;
	}
 
    return SOAP_OK; 
}


int  __tptz__GetNode(struct soap *soap, 
	 struct _tptz__GetNode *tptz__GetNode, 
	 struct _tptz__GetNodeResponse *tptz__GetNodeResponse)
{ 
	ONVIF_DBP("");

	if(!tptz__GetNode->NodeToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoEntity",
			"No such PTZNode on the device ."));
	}
	
	if(0!=strncmp(tptz__GetNode->NodeToken,"Anv_ptz_",strlen("Anv_ptz_")))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoEntity",
			"No such PTZNode on the device ."));
	}

	int chno;
	char *p;
	p = tptz__GetNode->NodeToken+strlen("Anv_ptz_");
	chno = atoi(p);
	if(soap->pConfig->g_channel_num<(chno+1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}

	tptz__GetNodeResponse->PTZNode = (struct tt__PTZNode *)soap_malloc_v2(soap, sizeof(struct tt__PTZNode));
	//memset(tptz__GetNodeResponse->PTZNode,0,sizeof(struct tt__PTZNode));
	
	tptz__GetNodeResponse->PTZNode->token = (char *)soap_malloc(soap,32);
	//memset(tptz__GetNodeResponse->PTZNode->token,0,32);
	strcpy(tptz__GetNodeResponse->PTZNode->token,tptz__GetNode->NodeToken);
	
	tptz__GetNodeResponse->PTZNode->Name = (char *)soap_malloc(soap,32);
	//memset(tptz__GetNodeResponse->PTZNode->Name,0,32);
	strcpy(tptz__GetNodeResponse->PTZNode->Name,tptz__GetNode->NodeToken);
	
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces = 
		(struct tt__PTZSpaces *)soap_malloc_v2(soap, sizeof(struct tt__PTZSpaces));
	//memset(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces,0,sizeof(struct tt__PTZSpaces));

	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->__sizeContinuousPanTiltVelocitySpace = 2;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace = 
		(struct tt__Space2DDescription *)soap_malloc_v2(soap, sizeof(struct tt__Space2DDescription)*2);
	//memset(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace,
	//	0,sizeof(struct tt__Space2DDescription)*2);

	int i;
	for(i=0;i<2;i++)
	{
		tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[i].URI = 
			(char *)soap_malloc(soap, 128);
		//memset(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[i].URI,0,128);
		if(i==0)
			strcpy(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[i].URI,
				"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
		else
			strcpy(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[i].URI,
				"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocitySpaceFOV");
		
		tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[i].XRange = 
			(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
		tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[i].XRange->Max = 1.0;
		tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[i].XRange->Min = -1.0;

		tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[i].YRange = 
			(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
		tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[i].YRange->Max = 1.0;
		tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[i].YRange->Min = -1.0;
	}

	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->__sizeContinuousZoomVelocitySpace = 2;
	tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace = 
		(struct tt__Space1DDescription *)soap_malloc_v2(soap, sizeof(struct tt__Space1DDescription)*2);
	//memset(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace,
	//	0,sizeof(struct tt__Space1DDescription)*2);

	for(i=0;i<2;i++)
	{
		tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace[i].URI = 
			(char *)soap_malloc(soap, 128);
		//memset(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace[i].URI,0,128);
		if(i==0)
			strcpy(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace[i].URI,
				"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");
		else
			strcpy(tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace[i].URI,
				"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocitySpaceMillimeter");
		
		tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace[i].XRange = 
			(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
		tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace[i].XRange->Max = 1.0;
		tptz__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace[i].XRange->Min = -1.0;
	}

	tptz__GetNodeResponse->PTZNode->MaximumNumberOfPresets = QMAPI_MAX_PRESET;
	tptz__GetNodeResponse->PTZNode->HomeSupported = xsd__boolean__false_;
	tptz__GetNodeResponse->PTZNode->__sizeAuxiliaryCommands = 0;

	tptz__GetNodeResponse->PTZNode->Extension = (struct tt__PTZNodeExtension *)soap_malloc_v2(soap, sizeof(struct tt__PTZNodeExtension));
	//memset(tptz__GetNodeResponse->PTZNode->Extension, 0, sizeof(struct tt__PTZNodeExtension));
	tptz__GetNodeResponse->PTZNode->Extension->SupportedPresetTour = 
		(struct tt__PTZPresetTourSupported *)soap_malloc_v2(soap, sizeof(struct tt__PTZPresetTourSupported));
	//memset(tptz__GetNodeResponse->PTZNode->Extension->SupportedPresetTour, 0, sizeof(struct tt__PTZPresetTourSupported));
	tptz__GetNodeResponse->PTZNode->Extension->SupportedPresetTour->MaximumNumberOfPresetTours = QMAPI_MAX_CRUISE_GROUP_NUM;
	tptz__GetNodeResponse->PTZNode->Extension->SupportedPresetTour->__sizePTZPresetTourOperation = 2;
	tptz__GetNodeResponse->PTZNode->Extension->SupportedPresetTour->PTZPresetTourOperation = 
		(enum tt__PTZPresetTourOperation *)soap_malloc_v2(soap, sizeof(enum tt__PTZPresetTourOperation)*2);
	tptz__GetNodeResponse->PTZNode->Extension->SupportedPresetTour->PTZPresetTourOperation[0] = tt__PTZPresetTourOperation__Start;
 	tptz__GetNodeResponse->PTZNode->Extension->SupportedPresetTour->PTZPresetTourOperation[1] = tt__PTZPresetTourOperation__Stop;
 
	return SOAP_OK; 
}


int  __tptz__SetConfiguration(struct soap *soap, 
 	struct _tptz__SetConfiguration *tptz__SetConfiguration, 
 	struct _tptz__SetConfigurationResponse *tptz__SetConfigurationResponse)
{ 
	ONVIF_DBP("");

	if(!tptz__SetConfiguration->PTZConfiguration->Name||
		!tptz__SetConfiguration->PTZConfiguration->token||!tptz__SetConfiguration->PTZConfiguration->NodeToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The configuration does not exist. "));
	}
	
	if((0!=strncmp(tptz__SetConfiguration->PTZConfiguration->Name,"Anv_ptz_",strlen("Anv_ptz_")))
		||(0!=strncmp(tptz__SetConfiguration->PTZConfiguration->token,"Anv_ptz_",strlen("Anv_ptz_")))
		||(0!=strncmp(tptz__SetConfiguration->PTZConfiguration->NodeToken,"Anv_ptz_",strlen("Anv_ptz_"))))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The  configuration token is invalid."));
	}

	return SOAP_OK;
}

int  __tptz__GetConfigurationOptions(struct soap *soap, 
	 struct _tptz__GetConfigurationOptions *tptz__GetConfigurationOptions, 
	 struct _tptz__GetConfigurationOptionsResponse *tptz__GetConfigurationOptionsResponse)
{
    ONVIF_DBP(""); 

	int chno;
	char *p;
/*
	if(!tptz__GetConfigurationOptions->ConfigurationToken
		||0!=strncmp(tptz__GetConfigurationOptions->ConfigurationToken,"Anv_ptz_", strlen("Anv_ptz_")))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}	
*/
	if(tptz__GetConfigurationOptions->ConfigurationToken
		&& 0!=strncmp(tptz__GetConfigurationOptions->ConfigurationToken,"Anv_ptz_", strlen("Anv_ptz_")))
	{
		p = tptz__GetConfigurationOptions->ConfigurationToken+strlen("Anv_ptz_");
		chno = atoi(p);
	}
	else
		chno = 0;
	
	if(soap->pConfig->g_channel_num<(chno+1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}
	
	
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions = 
		(struct tt__PTZConfigurationOptions *)soap_malloc_v2(soap,sizeof(struct tt__PTZConfigurationOptions));
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces = 
		(struct tt__PTZSpaces *)soap_malloc_v2(soap,sizeof(struct tt__PTZSpaces));

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__sizeContinuousPanTiltVelocitySpace = 2;	
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace = 
		(struct tt__Space2DDescription *)soap_malloc_v2(soap,sizeof(struct tt__Space2DDescription)*2);
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[0].URI = 
		(char *)soap_malloc(soap, 128);
	strcpy(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[0].URI,
		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[0].XRange = 
		(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[0].XRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[0].XRange->Min = -1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[0].YRange = 
		(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[0].YRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[0].YRange->Min = -1.0;

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[1].URI = 
		(char *)soap_malloc(soap, 128);
	strcpy(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[1].URI,
		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocitySpaceFOV");
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[1].XRange = 
		(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[1].XRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[1].XRange->Min = -1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[1].YRange = 
		(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[1].YRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace[1].YRange->Min = -1.0;

	
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__sizeContinuousZoomVelocitySpace = 2;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace = 
		(struct tt__Space1DDescription *)soap_malloc_v2(soap,sizeof(struct tt__Space1DDescription)*2);
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace[0].URI = 
		(char *)soap_malloc(soap, 128);
	strcpy(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace[0].URI,
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace[0].XRange = 
		(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace[0].XRange->Max = 1.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace[0].XRange->Min = -1.0;

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace[1].URI = 
		(char *)soap_malloc(soap, 128);
	strcpy(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace[1].URI,
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocitySpaceMillimeter");
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace[1].XRange = 
		(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace[1].XRange->Max = 7.0;
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace[1].XRange->Min = -7.0;

	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout = 
		(struct tt__DurationRange *)soap_malloc_v2(soap,sizeof(struct tt__DurationRange));
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout->Max = 
		(char *)soap_malloc(soap,32);
	strcpy(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout->Max, "PT1S");
	tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout->Min= 
		(char *)soap_malloc(soap,32);
	strcpy(tptz__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout->Min, "PT1S");
 
    return SOAP_OK;
}


int  __tptz__GotoHomePosition(struct soap *soap, 
 	struct _tptz__GotoHomePosition *tptz__GotoHomePosition, 
 	struct _tptz__GotoHomePositionResponse *tptz__GotoHomePositionResponse)
{
	ONVIF_DBP("");
	//send_ptz_cmd(PTZ_CAMERA_RESET, 0);

	if(!tptz__GotoHomePosition->ProfileToken)
	{
		goto _exit_goto_home_position;
	}
	if(!tptz__GotoHomePosition->Speed)
	{
		goto _exit_goto_home_position;
	}
	if(tptz__GotoHomePosition->Speed->PanTilt)
	{
		if(tptz__GotoHomePosition->Speed->PanTilt->space)
		{
			
		}
		//tptz__GotoHomePosition->Speed->PanTilt->x;
		//tptz__GotoHomePosition->Speed->PanTilt->y;
	}
	if(tptz__GotoHomePosition->Speed->Zoom)
	{
		if(tptz__GotoHomePosition->Speed->Zoom->space)
		{
			
		}
		//tptz__GotoHomePosition->Speed->Zoom->x;
	}
_exit_goto_home_position:

	return SOAP_OK;
}
 
int  __tptz__SetHomePosition(struct soap *soap, 
                            struct _tptz__SetHomePosition *tptz__SetHomePosition, 
                            struct _tptz__SetHomePositionResponse *tptz__SetHomePositionResponse)
{
	ONVIF_DBP("");

	return (send_soap_fault_msg(soap, 400,
			"SOAP-ENV:Receiver",
			"ter:Action",
			"ter:CannotOverwriteHome",
			"set homepositon not support"));

	return SOAP_OK;
}

int  __tptz__ContinuousMove(struct soap *soap, 
 	struct _tptz__ContinuousMove *tptz__ContinuousMove, 
 	struct _tptz__ContinuousMoveResponse *tptz__ContinuousMoveResponse)
{ 
	ONVIF_DBP("");

	//int direction = QMAPI_PTZ_STA_LEFT;
	int chno;
	char *p;

	if(!tptz__ContinuousMove->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz__ContinuousMove->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__ContinuousMove->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz__ContinuousMove->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}

	if(tptz__ContinuousMove->Velocity)
	{
		if(tptz__ContinuousMove->Velocity->PanTilt)
		{
			double x, y;
			int a, b;
			x = (tptz__ContinuousMove->Velocity->PanTilt->x*1000)/(66.667);
			y = (tptz__ContinuousMove->Velocity->PanTilt->y*1000)/(66.667);
			a = x;
			b = y;
			a = (a*8 + 1);
			b = (b*8 + 1);
			printf("pantilt x:%f y:%f, x=%f y=%f, a=%d, b=%d\n",
						tptz__ContinuousMove->Velocity->PanTilt->x, tptz__ContinuousMove->Velocity->PanTilt->y,
						x, y, a, b);


			//if(tptz__ContinuousMove->Velocity->Zoom)
			//	printf("Zoom x:%f, space:%s \n", tptz__ContinuousMove->Velocity->Zoom->x, tptz__ContinuousMove->Velocity->Zoom->space);

			if(((tptz__ContinuousMove->Velocity->PanTilt->x >= - EPSINON) &&(tptz__ContinuousMove->Velocity->PanTilt->x <= EPSINON)) &&
				((tptz__ContinuousMove->Velocity->PanTilt->y >= - EPSINON) &&(tptz__ContinuousMove->Velocity->PanTilt->y <= EPSINON)))
			{
				//printf("PTZ X:%f Y:%f \n",tptz__ContinuousMove->Velocity->PanTilt->x, tptz__ContinuousMove->Velocity->PanTilt->y);
				if(tptz__ContinuousMove->Velocity->Zoom
					&&tptz__ContinuousMove->Velocity->Zoom->x <0.000001
					&&tptz__ContinuousMove->Velocity->Zoom->x >-0.000001)
				{
					printf("Zoom X:%f \n",tptz__ContinuousMove->Velocity->Zoom->x);
					ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
					printf("======PTZ stop======\n");
					return SOAP_OK;
				}
			}
			else
			{
				if((tptz__ContinuousMove->Velocity->PanTilt->x>180.0 ||
					tptz__ContinuousMove->Velocity->PanTilt->x<-180.0)||
					(tptz__ContinuousMove->Velocity->PanTilt->y>90.0 ||
					tptz__ContinuousMove->Velocity->PanTilt->y<-90.0))
				{
					goto _exit_continuous_move_error;
				}
				
				if(tptz__ContinuousMove->Velocity->PanTilt->x > 0.0)
				{
					if(tptz__ContinuousMove->Velocity->PanTilt->y > 0.0)
					{
						if(tptz__ContinuousMove->Velocity->PanTilt->x > tptz__ContinuousMove->Velocity->PanTilt->y)
						{
							//right
							ptz_control(QMAPI_PTZ_STA_RIGHT, 0, chno);
							printf("======PTZ RIGHT======\n\n");
						}
						else
						{
							//up
							ptz_control(QMAPI_PTZ_STA_UP, 0, chno);
							printf("======PTZ up======\n\n");
						}
					}
					else if(tptz__ContinuousMove->Velocity->PanTilt->y <= 0.0)
					{
						if(tptz__ContinuousMove->Velocity->PanTilt->x > -tptz__ContinuousMove->Velocity->PanTilt->y)
						{
							//right
							ptz_control(QMAPI_PTZ_STA_RIGHT, 0,chno);
							printf("======PTZ right======\n\n");
						}
						else
						{
							//down
							ptz_control(QMAPI_PTZ_STA_DOWN, 0,chno);
							printf("======PTZ down======\n\n");
						}
					}
				}
				else if(tptz__ContinuousMove->Velocity->PanTilt->x < 0.000001
					&&tptz__ContinuousMove->Velocity->PanTilt->x >- 0.000001
					&& tptz__ContinuousMove->Velocity->PanTilt->y<0.000001
					&& tptz__ContinuousMove->Velocity->PanTilt->y>-0.000001
					&&tptz__ContinuousMove->Velocity->Zoom
					&&tptz__ContinuousMove->Velocity->Zoom->x <0.000001
					&&tptz__ContinuousMove->Velocity->Zoom->x >-0.000001)
				{
					ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
					printf("00======PTZ stop======\n");
				}
				else
				{
					if(tptz__ContinuousMove->Velocity->PanTilt->y > 0.0)
					{
						if(-tptz__ContinuousMove->Velocity->PanTilt->x > tptz__ContinuousMove->Velocity->PanTilt->y)
						{
							//left
							ptz_control(QMAPI_PTZ_STA_LEFT,0,chno);
							printf("======PTZ left======\n\n");
						}
						else
						{
							//up
							ptz_control(QMAPI_PTZ_STA_UP,0,chno);
							printf("======PTZ up======\n\n");
						}
					}
					else if(tptz__ContinuousMove->Velocity->PanTilt->y <= 0.0)
					{
						if(-tptz__ContinuousMove->Velocity->PanTilt->x > -tptz__ContinuousMove->Velocity->PanTilt->y)
						{
							//left
							ptz_control(QMAPI_PTZ_STA_LEFT,0,chno);
							printf("======PTZ left======\n\n");
						}
						else
						{
							//down
							ptz_control(QMAPI_PTZ_STA_DOWN,0,chno);
							printf("======PTZ down======\n\n");
						}
					}
				}
			}
		}
		if(tptz__ContinuousMove->Velocity->Zoom)
		{
			printf("Zoom x:%f, y:%f \n", tptz__ContinuousMove->Velocity->Zoom->x, tptz__ContinuousMove->Velocity->Zoom->x);
			if((tptz__ContinuousMove->Velocity->Zoom->x >= - EPSINON) &&(tptz__ContinuousMove->Velocity->Zoom->x <= EPSINON))
			{
				//printf("Zoom X:%f \n",tptz__ContinuousMove->Velocity->Zoom->x);
			}
			else
			{
				if(tptz__ContinuousMove->Velocity->Zoom->x>1000.0 ||
					tptz__ContinuousMove->Velocity->Zoom->x<-1000.0)
				{
					goto _exit_continuous_move_error;
				}
				if(tptz__ContinuousMove->Velocity->Zoom->x > EPSINON)
				{
					ptz_control(QMAPI_PTZ_STA_ZOOM_ADD,32,chno);
					printf("Zoom ++++++++ \n\n");
				}
				else
				{
					ptz_control(QMAPI_PTZ_STA_ZOOM_SUB,32,chno);
					printf("Zoom -------- \n\n");
				} 
			}
		}
	}

	return SOAP_OK;

_exit_continuous_move_error:
	return(send_soap_fault_msg(soap,400,
		"SOAP-ENV:Sender",
		"ter:InvalidArgVal",
		"ter:InvalidVelocity  ",
		"The requested velocity is out of bounds. "));
}

int  __tptz__RelativeMove(struct soap *soap, 
 	struct _tptz__RelativeMove *tptz__RelativeMove, 
 	struct _tptz__RelativeMoveResponse *tptz__RelativeMoveResponse)
{ 
	ONVIF_DBP("");

	int direction = 0;
	float x = 0.0, y = 0.0;	

	if ( tptz__RelativeMove->ProfileToken ) {
		ONVIF_DBP("tptz__RelativeMove->ProfileToken %s \n", tptz__RelativeMove->ProfileToken);
	}

	int chno;
	char *p;

	if(!tptz__RelativeMove->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz__RelativeMove->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__RelativeMove->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz__RelativeMove->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}

	if ( tptz__RelativeMove->Translation ) {

		if ( tptz__RelativeMove->Translation->PanTilt ) {	
			
			x = tptz__RelativeMove->Translation->PanTilt->x;
			y = tptz__RelativeMove->Translation->PanTilt->y;	

			if ( x > 1.000001 || x < -1.000001 || y > 1.000001 || y < -1.000001 ) {
				return(send_soap_fault_msg(soap,400,
					"SOAP-ENV:Sender",
					"ter:InvalidArgVal",
					"InvalidPosition",
					"The postion is invalid."));
			}
			
			if ((x >= - EPSINON) && (x <= EPSINON)) {	// x = 0;
		
				if ((y >= - EPSINON) && (y <= EPSINON)) {
		
				} else if ( y < - EPSINON ) {
					direction = QMAPI_PTZ_STA_DOWN;
					ONVIF_DBP("PTZ_DOWN\n");
					
				} else if ( y > EPSINON ) {
					direction = QMAPI_PTZ_STA_UP;
					ONVIF_DBP("PTZ_UP\n");
				}
			} else if ( x > EPSINON ) { //	x > 0, 

				if ((y >= - EPSINON) && (y <= EPSINON)) {
					direction = QMAPI_PTZ_STA_RIGHT;
					ONVIF_DBP("PTZ_RIGHT\n");
		
				} else if ( y > EPSINON) {
					direction = QMAPI_PTZ_STA_RIGHT;
					ONVIF_DBP("PTZ_RIGHT_UP\n");
				} else {
					direction = QMAPI_PTZ_STA_RIGHT;
					ONVIF_DBP("PTZ_RIGHT_DOWN\n");
				}
			} else if ( x < - EPSINON) { // x  <  0

				if ((y >= - EPSINON) && (y <= EPSINON)) {
					direction = QMAPI_PTZ_STA_LEFT;
					ONVIF_DBP("PTZ_LEFT\n");
		
				} else if ( y > EPSINON) {
					direction = QMAPI_PTZ_STA_LEFT;
					ONVIF_DBP("PTZ_LEFT_UP\n");
				} else {
					direction = QMAPI_PTZ_STA_LEFT;
					ONVIF_DBP("PTZ_LEFT_DOWN\n");
				}			
			}
		
			//send_ptz_cmd(direction, 0); 
			ptz_control(direction,0,chno);
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 80000;
			select(0, NULL, NULL, NULL, &tv);
			//send_ptz_cmd(PTZ_STOP, 0);
			ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
		}

		if ( tptz__RelativeMove->Translation->Zoom ) {

			if ( tptz__RelativeMove->Translation->Zoom->space ) {
				ONVIF_DBP(" space %s ", tptz__RelativeMove->Translation->Zoom->space);
			}

			if ( tptz__RelativeMove->Translation->Zoom->x ) {
				ONVIF_DBP(" x %f\n", tptz__RelativeMove->Translation->Zoom->x);
			}

			if ( ( tptz__RelativeMove->Translation->Zoom->x >= - EPSINON ) && 
				tptz__RelativeMove->Translation->Zoom->x <= EPSINON ) {

			} else {

				if ( tptz__RelativeMove->Translation->Zoom->x > EPSINON ) {
					//send_ptz_cmd(PTZ_ZOOM_IN);					
					ptz_control(QMAPI_PTZ_STA_ZOOM_ADD,0,chno);
					printf("Zoom ++++++++ \n\n");
				} else {
					//send_ptz_cmd(PTZ_ZOOM_OUT);					
					ptz_control(QMAPI_PTZ_STA_ZOOM_SUB,0,chno);
					printf("Zoom -------- \n\n");
				}

				struct timeval tv;
				tv.tv_sec = 0;
				tv.tv_usec = 80000;
				select(0, NULL, NULL, NULL, &tv);
				//send_ptz_cmd(PTZ_STOP, 0);
				ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
			}	
		}		
	}

	if ( tptz__RelativeMove->Speed ) {
		if ( tptz__RelativeMove->Speed->PanTilt ) {
			if ( tptz__RelativeMove->Speed->PanTilt->space ) {
				ONVIF_DBP("=========== tptz__RelativeMove->Speed PanTilt space %s ",
					tptz__RelativeMove->Speed->PanTilt->space);
			}

			if ( tptz__RelativeMove->Speed->PanTilt->x ) {
				ONVIF_DBP("x %f", tptz__RelativeMove->Speed->PanTilt->x);
			}

			if ( tptz__RelativeMove->Speed->PanTilt->y ) {
				ONVIF_DBP("y %f",	tptz__RelativeMove->Translation->PanTilt->y);
			}
		}

		if ( tptz__RelativeMove->Speed->Zoom ) {
			if ( tptz__RelativeMove->Speed->Zoom->space ) {
				ONVIF_DBP(" space %s ", tptz__RelativeMove->Speed->Zoom->space);
			}
			if ( tptz__RelativeMove->Speed->Zoom->x ) {
				ONVIF_DBP("   x %f\n",
					tptz__RelativeMove->Speed->Zoom->x);
			}
		}		
	}

	if(!tptz__RelativeMove->ProfileToken)
	{
		goto _exit_relative_move;
	}
	if(!tptz__RelativeMove->Translation)
	{
		goto _exit_relative_move;
	}
	if(!tptz__RelativeMove->Speed)
	{
		goto _exit_relative_move;
	}
	if(tptz__RelativeMove->Translation->PanTilt)
	{
		if(tptz__RelativeMove->Translation->PanTilt->space)
		{
			
		}
		//tptz__RelativeMove->Translation->PanTilt->x;
		//tptz__RelativeMove->Translation->PanTilt->y;
	}
	if(tptz__RelativeMove->Translation->Zoom)
	{
		if(tptz__RelativeMove->Translation->Zoom->space)
		{
			
		}
		//tptz__RelativeMove->Translation->Zoom->x;
	}
	if(tptz__RelativeMove->Speed->PanTilt)
	{
		if(tptz__RelativeMove->Speed->PanTilt->space)
		{
			
		}
		//tptz__RelativeMove->Speed->PanTilt->x;
		//tptz__RelativeMove->Speed->PanTilt->y;
	}
	if(tptz__RelativeMove->Speed->Zoom)
	{
		if(tptz__RelativeMove->Speed->Zoom->space)
		{
			
		}
		//tptz__RelativeMove->Speed->Zoom->x;
	}
_exit_relative_move:
	return SOAP_OK;
}

int  __tptz__SendAuxiliaryCommand(struct soap *soap, 
 	struct _tptz__SendAuxiliaryCommand *tptz__SendAuxiliaryCommand, 
 	struct _tptz__SendAuxiliaryCommandResponse *tptz__SendAuxiliaryCommandResponse)
{
	ONVIF_DBP("");

	if (tptz__SendAuxiliaryCommand->ProfileToken) {
		
	}
	
	if (tptz__SendAuxiliaryCommand->AuxiliaryData) {
	
	}
	
	tptz__SendAuxiliaryCommandResponse->AuxiliaryResponse = (char *)soap_malloc_v2(soap,32);
	//memset(tptz__SendAuxiliaryCommandResponse->AuxiliaryResponse, 0, 32);
	
	return SOAP_OK;
}

int  __tptz__AbsoluteMove(struct soap *soap, 
 	struct _tptz__AbsoluteMove *tptz__AbsoluteMove, 
 	struct _tptz__AbsoluteMoveResponse *tptz__AbsoluteMoveResponse)
{ 
	ONVIF_DBP("");

	int direction = 0;
	float x = 0.0, y = 0.0;	

	if (!tptz__AbsoluteMove->ProfileToken)
	{
		goto _exit_absolute_move;
	}

	int chno;
	char *p;

	if(!tptz__AbsoluteMove->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz__AbsoluteMove->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__AbsoluteMove->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz__AbsoluteMove->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}

#if 1
	if ( tptz__AbsoluteMove->Position )
	{
		if ( tptz__AbsoluteMove->Position->PanTilt ) 
		{

			x = tptz__AbsoluteMove->Position->PanTilt->x;
			y = tptz__AbsoluteMove->Position->PanTilt->y;

			if ( x > 1.000001 || x < -1.000001 || y > 1.000001 || y < -1.000001 ) {
				return(send_soap_fault_msg(soap,400,
					"SOAP-ENV:Sender",
					"ter:InvalidArgVal",
					"InvalidPosition",
					"The postion is invalid."));
			}

			if ((x >= - EPSINON) && (x <= EPSINON)) {	// x = 0;
				if ((y >= - EPSINON) && (y <= EPSINON)) {

					
				} else if ( y < - EPSINON ) {
					direction = QMAPI_PTZ_STA_DOWN;						
				} else if ( y > EPSINON ) {
					direction = QMAPI_PTZ_STA_UP;					
				}
				
			} else if ( x > EPSINON ) { //  x > 0, 
				if ((y >= - EPSINON) && (y <= EPSINON)) {
					direction = QMAPI_PTZ_STA_RIGHT;
				} else if ( y > EPSINON) {
					direction = QMAPI_PTZ_STA_RIGHT;					
				} else {
					direction = QMAPI_PTZ_STA_RIGHT;					
				}
			} else if ( x < - EPSINON) { // x  <  0

				if ((y >= - EPSINON) && (y <= EPSINON)) {
					direction = QMAPI_PTZ_STA_LEFT;	
				} else if ( y > EPSINON) {
					direction = QMAPI_PTZ_STA_LEFT;					
				} else {
					direction = QMAPI_PTZ_STA_LEFT;				
				}
			}
/*
			send_ptz_cmd(direction, 0);	
			usleep(80000);	
			send_ptz_cmd(PTZ_STOP, 0);	
*/
			ptz_control(direction,0,chno);
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 80000;
			select(0, NULL, NULL, NULL, &tv);
			ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
		}		

		if ( tptz__AbsoluteMove->Position->Zoom )
		{
			if ( tptz__AbsoluteMove->Position->Zoom->space ) {
				ONVIF_DBP(" space %s ", tptz__AbsoluteMove->Position->Zoom->space);
			}

			if ( tptz__AbsoluteMove->Position->Zoom->x ) {
				ONVIF_DBP(" x %f\n", tptz__AbsoluteMove->Position->Zoom->x);
			}

			if ( ( tptz__AbsoluteMove->Position->Zoom->x >= - EPSINON ) && 
				tptz__AbsoluteMove->Position->Zoom->x <= EPSINON ) {
			} else {

				if ( tptz__AbsoluteMove->Position->Zoom->x > EPSINON ) {
					//send_ptz_cmd(PTZ_ZOOM_IN);					
					ptz_control(QMAPI_PTZ_STA_ZOOM_ADD,0,chno);
					printf("Zoom ++++++++ \n\n");
				} else {
					//send_ptz_cmd(PTZ_ZOOM_OUT);					
					ptz_control(QMAPI_PTZ_STA_ZOOM_SUB,0,chno);
					printf("Zoom -------- \n\n");
				}		

				struct timeval tv;
				tv.tv_sec = 0;
				tv.tv_usec = 80000;
				select(0, NULL, NULL, NULL, &tv);
				//send_ptz_cmd(PTZ_STOP, 0);				
				ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
				printf("======PTZ stop======\n");
			}
		}		
	}
#else	
	int ret;
	QMAPI_NET_PTZ_POS stNetPtzPos;
	memset(&stNetPtzPos, 0, sizeof(stNetPtzPos));
	stNetPtzPos.dwSize = sizeof(QMAPI_NET_PTZ_POS);

	if(tptz__AbsoluteMove->Position) 
	{
		if(tptz__AbsoluteMove->Position->PanTilt)
		{
			x = tptz__AbsoluteMove->Position->PanTilt->x;
			y = tptz__AbsoluteMove->Position->PanTilt->y;

			if(x > 1.000001 || x < -1.000001 || y > 1.000001 || y < -1.000001)
			{
				return(send_soap_fault_msg(soap,400,
					"SOAP-ENV:Sender",
					"ter:InvalidArgVal",
					"InvalidPosition",
					"The postion is invalid."));
			}

			stNetPtzPos.nPanPos = x;
			stNetPtzPos.nTiltPos = y;
		}		

		if(tptz__AbsoluteMove->Position->Zoom)
		{
			if(tptz__AbsoluteMove->Position->Zoom->space)
			{
				ONVIF_DBP(" space %s ", tptz__AbsoluteMove->Position->Zoom->space);
			}

			if(tptz__AbsoluteMove->Position->Zoom->x)
			{
				ONVIF_DBP(" x %f\n", tptz__AbsoluteMove->Position->Zoom->x);
			}

			if((tptz__AbsoluteMove->Position->Zoom->x >= - EPSINON)
				&& tptz__AbsoluteMove->Position->Zoom->x <= EPSINON)
			{
			}
			else
			{
				stNetPtzPos.nZoomPos = tptz__AbsoluteMove->Position->Zoom->x;
			}
		}
	}
	ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_PTZPOS, 0, &stNetPtzPos, sizeof(QMAPI_NET_PTZ_POS));
	if(0 != ret)
	{
		printf("# [%s]:line=%d\n", __FUNCTION__, __LINE__);
	}
#endif
	if(tptz__AbsoluteMove->Position)
	{
		if(tptz__AbsoluteMove->Position->PanTilt)
		{
			if(tptz__AbsoluteMove->Position->PanTilt->space)
			{

			}
			//tptz__AbsoluteMove->Position->PanTilt->x;
			//tptz__AbsoluteMove->Position->PanTilt->y;
			//ptz_cmd
		}
		
		if(tptz__AbsoluteMove->Position->Zoom)
		{
			if(tptz__AbsoluteMove->Position->Zoom->space)
			{

			}
			//tptz__AbsoluteMove->Position->Zoom->x;
		}
	}

	if(tptz__AbsoluteMove->Speed)
	{
		ONVIF_DBP("================ tptz__AbsoluteMove->Speed \n");

		if(tptz__AbsoluteMove->Speed->PanTilt)
		{
			if(tptz__AbsoluteMove->Speed->PanTilt->space)
			{

			}			
			ONVIF_DBP("============ x %f \n", tptz__AbsoluteMove->Speed->PanTilt->x);
			ONVIF_DBP("============== y %f \n", tptz__AbsoluteMove->Speed->PanTilt->y);
		}
		
		if(tptz__AbsoluteMove->Speed->Zoom)
		{
			if(tptz__AbsoluteMove->Speed->Zoom->space)
			{

			}		
			ONVIF_DBP("====== zoom x %f \n", tptz__AbsoluteMove->Speed->Zoom->x);
		}
	}

_exit_absolute_move:
	return SOAP_OK;
}

int  __tptz__Stop(struct soap *soap, 
	struct _tptz__Stop *tptz__Stop, 
	struct _tptz__StopResponse *tptz__StopResponse)
{
	ONVIF_DBP("");	

	int chno;
	char *p;

	if(!tptz__Stop->ProfileToken)
	{
		/*
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested profile does not exist. "));
		*/
		chno = 0;
	}

	if(strncmp(tptz__Stop->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz__Stop->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz__Stop->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}
	ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
	printf("00======PTZ stop======\n");

	return SOAP_OK;
}

int __tptz10__GetConfigurations(struct soap* soap, struct _tptz10__GetConfigurations *tptz10__GetConfigurations, struct _tptz10__GetConfigurationsResponse *tptz10__GetConfigurationsResponse)
{ 
 	ONVIF_DBP("");

	tptz10__GetConfigurationsResponse->__sizePTZConfigurations = soap->pConfig->g_channel_num;
	
	tptz10__GetConfigurationsResponse->PTZConfigurations = soap_malloc_v2(soap,sizeof(struct tt__PTZConfiguration)*soap->pConfig->g_channel_num);
	//memset(tptz10__GetConfigurationsResponse->PTZConfigurations,0,sizeof(struct tt__PTZConfiguration)*soap->pConfig->g_channel_num);

	int i;
	for(i=0;i<soap->pConfig->g_channel_num;i++)
	{
		tptz10__GetConfigurationsResponse->PTZConfigurations[i].Name = (char *)soap_malloc(soap,32);
		sprintf(tptz10__GetConfigurationsResponse->PTZConfigurations[i].Name,"Anv_ptz_%d",i);
		
		tptz10__GetConfigurationsResponse->PTZConfigurations[i].token = (char *)soap_malloc(soap,32);
		sprintf(tptz10__GetConfigurationsResponse->PTZConfigurations[i].token,"Anv_ptz_%d",i);
		
		tptz10__GetConfigurationsResponse->PTZConfigurations[i].UseCount = 1;
		
		tptz10__GetConfigurationsResponse->PTZConfigurations[i].NodeToken = (char *)soap_malloc(soap,32);
		sprintf(tptz10__GetConfigurationsResponse->PTZConfigurations[i].NodeToken,"Anv_ptz_%d",i);

		tptz10__GetConfigurationsResponse->PTZConfigurations[i].DefaultContinuousPanTiltVelocitySpace = 
			(char *)soap_malloc(soap,128);
		//memset(tptz10__GetConfigurationsResponse->PTZConfigurations[i].DefaultContinuousPanTiltVelocitySpace,0,128);
		strcpy(tptz10__GetConfigurationsResponse->PTZConfigurations[i].DefaultContinuousPanTiltVelocitySpace,
			"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocitySpaceDegrees");
		
		tptz10__GetConfigurationsResponse->PTZConfigurations[i].DefaultContinuousZoomVelocitySpace = 
			(char *)soap_malloc(soap,128);
		//memset(tptz10__GetConfigurationsResponse->PTZConfigurations[i].DefaultContinuousZoomVelocitySpace,0,128);
		strcpy(tptz10__GetConfigurationsResponse->PTZConfigurations[i].DefaultContinuousZoomVelocitySpace,
			"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocitySpaceMillimeter");
		
		tptz10__GetConfigurationsResponse->PTZConfigurations[i].DefaultPTZTimeout= 
			(char *)soap_malloc(soap, 8);
		strcpy(tptz10__GetConfigurationsResponse->PTZConfigurations[i].DefaultPTZTimeout,"PT1S");
	
	}
 	return SOAP_OK; 
}


int __tptz10__GetPresets(struct soap* soap, struct _tptz10__GetPresets *tptz10__GetPresets, struct _tptz10__GetPresetsResponse *tptz10__GetPresetsResponse) 
{ 
 	ONVIF_DBP("");

	int i, ret;
	int chno;
	char *p,buf[64];
	QMAPI_NET_PRESET_INFO presetinfo;

	memset(&presetinfo, 0, sizeof(QMAPI_NET_PRESET_INFO));

	if(!tptz10__GetPresets->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoProfile",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz10__GetPresets->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz10__GetPresets->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz10__GetPresets->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}

	presetinfo.nChannel= chno;
	presetinfo.dwSize = sizeof(QMAPI_NET_PRESET_INFO);

	ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_ALL_PRESET, chno,&presetinfo,sizeof(QMAPI_NET_PRESET_INFO));
	if(ret<0)
	{
		printf("%s QMAPI_SYSCFG_GET_ALL_PRESET error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}
	
	tptz10__GetPresetsResponse->__sizePTZPreset = presetinfo.nPresetNum;
	printf("%s  presetNum=%d\n",__FUNCTION__,presetinfo.nPresetNum);
	tptz10__GetPresetsResponse->PTZPreset = 
		(struct tt__PTZPreset *)soap_malloc_v2(soap,sizeof(struct tt__PTZPreset)*(presetinfo.nPresetNum));
	//memset(tptz10__GetPresetsResponse->PTZPreset,0,sizeof(struct tt__PTZPreset)*(presetinfo.nPresetNum));

	for(i = 0;i<presetinfo.nPresetNum;i++)
	{
		tptz10__GetPresetsResponse->PTZPreset[i].Name = (char *)soap_malloc(soap,128);
		
		if(strlen(presetinfo.csName[i])<1)
		{
			sprintf(buf,"preset%d",presetinfo.no[i]);
			strcpy(tptz10__GetPresetsResponse->PTZPreset[i].Name, buf);
		}
		else
			strcpy(tptz10__GetPresetsResponse->PTZPreset[i].Name, presetinfo.csName[i]);

		//#if 0
		tptz10__GetPresetsResponse->PTZPreset[i].PTZPosition = 
			(struct tt__PTZVector *)soap_malloc_v2(soap,sizeof(struct tt__PTZVector));
		
		tptz10__GetPresetsResponse->PTZPreset[i].PTZPosition->PanTilt = 
			(struct tt__Vector2D *)soap_malloc_v2(soap,sizeof(struct tt__Vector2D));
		tptz10__GetPresetsResponse->PTZPreset[i].PTZPosition->PanTilt->x = 0.25;
		tptz10__GetPresetsResponse->PTZPreset[i].PTZPosition->PanTilt->y = 0.25;
		tptz10__GetPresetsResponse->PTZPreset[i].PTZPosition->PanTilt->space = 
			(char *)soap_malloc(soap,128);
		//memset(tptz10__GetPresetsResponse->PTZPreset[i].PTZPosition->PanTilt->space,0,128);
		strcpy(tptz10__GetPresetsResponse->PTZPreset[i].PTZPosition->PanTilt->space, 
					"http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace");
		
		tptz10__GetPresetsResponse->PTZPreset[i].PTZPosition->Zoom= 
			(struct tt__Vector1D *)soap_malloc_v2(soap,sizeof(struct tt__Vector1D));
		tptz10__GetPresetsResponse->PTZPreset[i].PTZPosition->Zoom->x = 0.25;
		tptz10__GetPresetsResponse->PTZPreset[i].PTZPosition->Zoom->space = 
			(char *)soap_malloc(soap,128);
		//memset(tptz10__GetPresetsResponse->PTZPreset[i].PTZPosition->Zoom->space,0,128);
		strcpy(tptz10__GetPresetsResponse->PTZPreset[i].PTZPosition->Zoom->space, 
					"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace");
		//#endif
		
		tptz10__GetPresetsResponse->PTZPreset[i].token = (char *)soap_malloc(soap,128);
		
		sprintf(tptz10__GetPresetsResponse->PTZPreset[i].token, "%d", presetinfo.no[i]);
		//printf("%s ,presetNum=%d,no=%d,name=%s\n",__FUNCTION__,presetinfo.presetNum,presetinfo.no[i],presetinfo.csName[i]);
	}
	
 	return SOAP_OK;
}


 

int __tptz10__SetPreset(struct soap* soap, struct _tptz10__SetPreset *tptz10__SetPreset, struct _tptz10__SetPresetResponse *tptz10__SetPresetResponse)
{ 
	ONVIF_DBP("");

	int point = 0;

	int ret, chno;
	char *p, buf[64]={0};
	QMAPI_NET_PRESET_INFO presetinfo;

	memset(&presetinfo, 0, sizeof(QMAPI_NET_PRESET_INFO));

	if(!tptz10__SetPreset->ProfileToken)
	{
		chno = 0;
		printf("ProfileToken is NULL!\n");
	}
	else if(strncmp(tptz10__SetPreset->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz10__SetPreset->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz10__SetPreset->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}

	if(!tptz10__SetPreset->PresetToken)
	{
		int i,j,used=0;
		presetinfo.nChannel= chno;
		presetinfo.dwSize = sizeof(QMAPI_NET_PRESET_INFO);

		ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_ALL_PRESET, chno,&presetinfo,sizeof(QMAPI_NET_PRESET_INFO));
		if(ret<0)
		{
			printf("%s QMAPI_SYSCFG_GET_ALL_PRESET error!ret=%d\n", __FUNCTION__, ret);
			return SOAP_ERR;
		}

		for(j=1;j<QMAPI_MAX_PRESET;j++)
		{
			used=0;
			for(i=0;i<presetinfo.nPresetNum;i++)
			{
				if(j ==presetinfo.no[i])
				{
					used = 1;
					break;
				}
			}

			if(!used)
				break;
		}

		if(used)
		{
			printf("%s ,cannot set more preset!\n", __FUNCTION__);
			return SOAP_ERR;
		}

		point = j;
		//printf("%s 00000000000,presetNum=%d,point=%d\n",__FUNCTION__,presetinfo.presetNum,point);
	}
	else
	{
		point = atoi(tptz10__SetPreset->PresetToken);
	}

	QMAPI_NET_PTZ_CONTROL ptz_ctrl;
	if(!tptz10__SetPreset->PresetName)
	{
		sprintf(buf,"preset%d",point);
		strcpy((char *)ptz_ctrl.byRes, buf);
	}
	else
		strcpy((char *)ptz_ctrl.byRes, tptz10__SetPreset->PresetName);

	ptz_ctrl.dwSize = sizeof(QMAPI_NET_PTZ_CONTROL);
	ptz_ctrl.nChannel = chno;
	ptz_ctrl.dwCommand = QMAPI_PTZ_STA_PRESET;
	ptz_ctrl.dwParam = point;

	ret = QMapi_sys_ioctrl(0,QMAPI_NET_STA_PTZ_CONTROL, chno,&ptz_ctrl,sizeof(QMAPI_NET_PTZ_CONTROL));
	if(ret<0)
	{
		printf("%s QMAPI_NET_STA_PTZ_CONTROL error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	printf("Set preset, point:%d, name:%s\n", point,ptz_ctrl.byRes);

	//tptz__SetPresetResponse->PresetToken = (char *)soap_malloc(soap,16);
	//sprintf(tptz__SetPresetResponse->PresetToken, "%d", point);
	//printf("%s 11111111,point=%d,name=%s\n",__FUNCTION__,point,ptz_ctrl.byRes);


	return SOAP_OK; 
}

int __tptz10__RemovePreset(struct soap* soap, struct _tptz10__RemovePreset *tptz10__RemovePreset, struct _tptz10__RemovePresetResponse *tptz10__RemovePresetResponse)
{ 
	ONVIF_DBP("");

	return SOAP_OK;
	int point = 0;
	int find = 0;
	int i,ret, chno;
	char *p;
	QMAPI_NET_PTZ_CONTROL ptz_ctrl;
	QMAPI_NET_PRESET_INFO presetinfo;
	
	if(!tptz10__RemovePreset->ProfileToken)
	{
		chno = 0;
		printf("ProfileToken is NULL!\n");
	}
	else if(strncmp(tptz10__RemovePreset->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz10__RemovePreset->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoProfile",
				"The requested profile does not exist. "));
		}	
		p = tptz10__RemovePreset->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}
	
	if(!tptz10__RemovePreset->PresetToken)
	{
		return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoToken",
				"The requested preset token does not exist."));
	}

	point = atoi(tptz10__RemovePreset->PresetToken);

	

	memset(&presetinfo, 0, sizeof(QMAPI_NET_PRESET_INFO));

	presetinfo.nChannel= chno;
	presetinfo.dwSize = sizeof(QMAPI_NET_PRESET_INFO);

	ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_ALL_PRESET, chno,&presetinfo,sizeof(QMAPI_NET_PRESET_INFO));
	if(ret<0)
	{
		printf("%s QMAPI_SYSCFG_GET_ALL_PRESET error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	
	for(i = 0;i<presetinfo.nPresetNum;i++)
	{
		if(point == presetinfo.no[i])
		{
			find = 1;
			break;
		}
	}

	if(!find)
		return SOAP_OK;

	printf("%s  %d, remove preset:%d\n",__FUNCTION__, __LINE__,point);
	memset(&ptz_ctrl, 0, sizeof(ptz_ctrl));
	ptz_ctrl.dwSize = sizeof(ptz_ctrl);
	ptz_ctrl.dwParam = point;
	ptz_ctrl.nChannel = chno;
	ptz_ctrl.dwCommand = QMAPI_PTZ_STA_PRESET_CLS;
	ret = QMapi_sys_ioctrl(0,QMAPI_NET_STA_PTZ_CONTROL, chno,&ptz_ctrl,sizeof(QMAPI_NET_PTZ_CONTROL));
	if(ret<0)
	{
		printf("%s QMAPI_NET_STA_PTZ_CONTROL error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}

	return SOAP_OK; 
}

int __tptz10__GotoPreset(struct soap* soap, struct _tptz10__GotoPreset *tptz10__GotoPreset, struct _tptz10__GotoPresetResponse *tptz10__GotoPresetResponse)
{ 
 	ONVIF_DBP("");

	int point = 0;

	int ret, chno;
	char *p;
	QMAPI_NET_PRESET_INFO presetinfo;

	memset(&presetinfo, 0, sizeof(QMAPI_NET_PRESET_INFO));

	if(!tptz10__GotoPreset->ProfileToken)
	{
		chno = 0;
		printf("ProfileToken is NULL!\n");
	}
	else if(strncmp(tptz10__GotoPreset->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz10__GotoPreset->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz10__GotoPreset->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}
	
	if(!tptz10__GotoPreset->PresetToken)
	{
		return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoToken",
				"The requested profile does not exist. "));
	}

	point = atoi(tptz10__GotoPreset->PresetToken);

	QMAPI_NET_PTZ_CONTROL ptz_ctrl;
	memset(&ptz_ctrl, 0, sizeof(QMAPI_NET_PTZ_CONTROL));

	ptz_ctrl.dwSize = sizeof(QMAPI_NET_PTZ_CONTROL);
	ptz_ctrl.nChannel = chno;
	ptz_ctrl.dwCommand = QMAPI_PTZ_STA_CALL;
	ptz_ctrl.dwParam = point;

	ret = QMapi_sys_ioctrl(0,QMAPI_NET_STA_PTZ_CONTROL, chno,&ptz_ctrl,sizeof(QMAPI_NET_PTZ_CONTROL));
	if(ret<0)
	{
		printf("%s QMAPI_NET_STA_PTZ_CONTROL error!ret=%d\n", __FUNCTION__, ret);
		return SOAP_ERR;
	}
	
 
 
 	return SOAP_OK; 
 }

int __tptz10__GetStatus(struct soap* soap, struct _tptz10__GetStatus *tptz10__GetStatus, struct _tptz10__GetStatusResponse *tptz10__GetStatusResponse)
{  
 	ONVIF_DBP("");
	tptz10__GetStatusResponse->PTZStatus = 
		(struct tt__PTZStatus *)soap_malloc_v2(soap, sizeof(struct tt__PTZStatus));
	//memset(tptz10__GetStatusResponse->PTZStatus, 0, sizeof(struct tt__PTZStatus));
#if 1
	if(!tptz10__GetStatus->ProfileToken)
	{
		
	}
	int nRes = 0;
	QMAPI_NET_PTZ_POS   stNetPtzPos;
	stNetPtzPos.dwSize = sizeof(QMAPI_NET_PTZ_POS);
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PTZPOS, 0, &stNetPtzPos, sizeof(QMAPI_NET_PTZ_POS));

	if(nRes == 0)
	{
		tptz10__GetStatusResponse->PTZStatus = 
			(struct tt__PTZStatus *)soap_malloc_v2(soap, sizeof(struct tt__PTZStatus));
		//memset(tptz10__GetStatusResponse->PTZStatus, 0, sizeof(struct tt__PTZStatus));
		
		tptz10__GetStatusResponse->PTZStatus->Position = 
			(struct tt__PTZVector *)soap_malloc_v2(soap, sizeof(struct tt__PTZVector));
		
		tptz10__GetStatusResponse->PTZStatus->Position->PanTilt = 
			(struct tt__Vector2D *)soap_malloc_v2(soap,sizeof(struct tt__Vector2D));
		//memset(tptz10__GetStatusResponse->PTZStatus->Position->PanTilt,0,sizeof(struct tt__Vector2D));
		tptz10__GetStatusResponse->PTZStatus->Position->PanTilt->x = stNetPtzPos.nPanPos;
		tptz10__GetStatusResponse->PTZStatus->Position->PanTilt->y = stNetPtzPos.nTiltPos;
		tptz10__GetStatusResponse->PTZStatus->Position->PanTilt->space = 
			(char *)soap_malloc(soap,128);
		//memset(tptz10__GetStatusResponse->PTZStatus->Position->PanTilt->space,0,128);
		
		tptz10__GetStatusResponse->PTZStatus->Position->Zoom= 
			(struct tt__Vector1D *)soap_malloc_v2(soap,sizeof(struct tt__Vector1D));
		//memset(tptz10__GetStatusResponse->PTZStatus->Position->Zoom,0,sizeof(struct tt__Vector1D));
		tptz10__GetStatusResponse->PTZStatus->Position->Zoom->x = stNetPtzPos.nZoomPos;
		tptz10__GetStatusResponse->PTZStatus->Position->Zoom->space = 
			(char *)soap_malloc_v2(soap,128);
		//memset(tptz10__GetStatusResponse->PTZStatus->Position->Zoom->space,0,128);
		
		tptz10__GetStatusResponse->PTZStatus->MoveStatus = 
			(struct tt__PTZMoveStatus *)soap_malloc_v2(soap, sizeof(struct tt__PTZMoveStatus));
		
		tptz10__GetStatusResponse->PTZStatus->MoveStatus->PanTilt = 
			(enum tt__MoveStatus *)soap_malloc(soap,sizeof(enum tt__MoveStatus));
		//memset(tptz10__GetStatusResponse->PTZStatus->MoveStatus->PanTilt,0,sizeof(enum tt__MoveStatus));
		*(tptz10__GetStatusResponse->PTZStatus->MoveStatus->PanTilt) = tt__MoveStatus__IDLE;
		
		tptz10__GetStatusResponse->PTZStatus->MoveStatus->Zoom= 
			(enum tt__MoveStatus *)soap_malloc(soap,sizeof(enum tt__MoveStatus));
		//memset(tptz10__GetStatusResponse->PTZStatus->MoveStatus->Zoom,0,sizeof(enum tt__MoveStatus));
		*(tptz10__GetStatusResponse->PTZStatus->MoveStatus->Zoom) = tt__MoveStatus__IDLE;
	}
#endif		
	tptz10__GetStatusResponse->PTZStatus->UtcTime=time((time_t *)NULL);
	
 	return SOAP_OK; 
}

int __tptz10__GetConfiguration(struct soap* soap, struct _tptz10__GetConfiguration *tptz10__GetConfiguration, struct _tptz10__GetConfigurationResponse *tptz10__GetConfigurationResponse)
{  
	ONVIF_DBP("");

	int chno;
	char *p;
	if(!tptz10__GetConfiguration->ConfigurationToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}
	
	if(0!=strncmp(tptz10__GetConfiguration->ConfigurationToken,"Anv_ptz_", strlen("Anv_ptz_")))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}	
	p = tptz10__GetConfiguration->ConfigurationToken+strlen("Anv_ptz_");
	chno = atoi(p);
	if(soap->pConfig->g_channel_num<(chno+1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}
	
	tptz10__GetConfigurationResponse->PTZConfiguration = soap_malloc_v2(soap,sizeof(struct tt__PTZConfiguration));
	//memset(tptz10__GetConfigurationResponse->PTZConfiguration,0,sizeof(struct tt__PTZConfiguration));
	
	tptz10__GetConfigurationResponse->PTZConfiguration->Name = (char *)soap_malloc(soap,32);
	//memset(tptz10__GetConfigurationResponse->PTZConfiguration->Name,0,32);
	sprintf(tptz10__GetConfigurationResponse->PTZConfiguration->Name,"Anv_ptz_%d",chno);
	
	tptz10__GetConfigurationResponse->PTZConfiguration->token = (char *)soap_malloc(soap,32);
	//memset(tptz10__GetConfigurationResponse->PTZConfiguration->token,0,32);
	sprintf(tptz10__GetConfigurationResponse->PTZConfiguration->token,"Anv_ptz_%d",chno);
	
	tptz10__GetConfigurationResponse->PTZConfiguration->UseCount = 1;
	
	tptz10__GetConfigurationResponse->PTZConfiguration->NodeToken = (char *)soap_malloc(soap,32);
	//memset(tptz10__GetConfigurationResponse->PTZConfiguration->NodeToken,0,32);
	sprintf(tptz10__GetConfigurationResponse->PTZConfiguration->NodeToken,"Anv_ptz_%d",chno);

	tptz10__GetConfigurationResponse->PTZConfiguration->DefaultContinuousPanTiltVelocitySpace = 
		(char *)soap_malloc(soap,128);
	//memset(tptz10__GetConfigurationResponse->PTZConfiguration->DefaultContinuousPanTiltVelocitySpace,0,128);
	strcpy(tptz10__GetConfigurationResponse->PTZConfiguration->DefaultContinuousPanTiltVelocitySpace,
		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/SpeedSpaceDegrees");
	
	tptz10__GetConfigurationResponse->PTZConfiguration->DefaultContinuousZoomVelocitySpace = 
		(char *)soap_malloc(soap,128);
	//memset(tptz10__GetConfigurationResponse->PTZConfiguration->DefaultContinuousZoomVelocitySpace,0,128);
	strcpy(tptz10__GetConfigurationResponse->PTZConfiguration->DefaultContinuousZoomVelocitySpace,
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/SpeedSpaceMillimeter");
	tptz10__GetConfigurationResponse->PTZConfiguration->DefaultPTZTimeout= 
		(char *)soap_malloc(soap, 8);
	strcpy(tptz10__GetConfigurationResponse->PTZConfiguration->DefaultPTZTimeout,"PT1S");
	
 	return SOAP_OK; 
}

int __tptz10__GetNodes(struct soap* soap, struct _tptz10__GetNodes *tptz10__GetNodes, struct _tptz10__GetNodesResponse *tptz10__GetNodesResponse)
{ 
 	ONVIF_DBP("");

	int i;
	tptz10__GetNodesResponse->__sizePTZNode = soap->pConfig->g_channel_num;
	
	tptz10__GetNodesResponse->PTZNode = (struct tt__PTZNode *)soap_malloc_v2(soap, sizeof(struct tt__PTZNode)*soap->pConfig->g_channel_num);
	//memset(tptz10__GetNodesResponse->PTZNode,0,sizeof(struct tt__PTZNode)*soap->pConfig->g_channel_num);

	for(i=0;i<soap->pConfig->g_channel_num;i++)
	{
		tptz10__GetNodesResponse->PTZNode->token = (char *)soap_malloc(soap,32);
		//memset(tptz10__GetNodesResponse->PTZNode->token,0,32);
		sprintf(tptz10__GetNodesResponse->PTZNode->token,"Anv_ptz_%d",i);
		
		tptz10__GetNodesResponse->PTZNode->Name = (char *)soap_malloc(soap,32);
		//memset(tptz10__GetNodesResponse->PTZNode->Name,0,32);
		sprintf(tptz10__GetNodesResponse->PTZNode->Name,"Anv_ptz_%d",i);
		
		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces = 
			(struct tt__PTZSpaces *)soap_malloc_v2(soap, sizeof(struct tt__PTZSpaces));
		//memset(tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces,0,sizeof(struct tt__PTZSpaces));

		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->__sizeContinuousPanTiltVelocitySpace = 1;
		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace = 
			(struct tt__Space2DDescription *)soap_malloc_v2(soap, sizeof(struct tt__Space2DDescription));
		//memset(tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace,
		//	0,sizeof(struct tt__Space2DDescription));

		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->URI = 
			(char *)soap_malloc(soap, 128);
		//memset(tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->URI,0,128);
		
		strcpy(tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->URI,
			"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocitySpaceDegrees");
	
		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange = 
			(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange->Max = 1.0;
		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange->Min = -1.0;

		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange = 
			(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange->Max = 1.0;
		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange->Min = -1.0;
		

		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->__sizeContinuousZoomVelocitySpace = 1;
		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace = 
			(struct tt__Space1DDescription *)soap_malloc_v2(soap, sizeof(struct tt__Space1DDescription));
		//memset(tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace,
		//	0,sizeof(struct tt__Space1DDescription));

		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->URI = 
			(char *)soap_malloc(soap, 128);
		//memset(tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->URI,0,128);
		strcpy(tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->URI,
			"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocitySpaceMillimeter");
	
		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange = 
			(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange->Max = 1.0;
		tptz10__GetNodesResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange->Min = -1.0;

		tptz10__GetNodesResponse->PTZNode->MaximumNumberOfPresets = 255;
		tptz10__GetNodesResponse->PTZNode->HomeSupported = xsd__boolean__true_;
		tptz10__GetNodesResponse->PTZNode->__sizeAuxiliaryCommands = 0;
	}
	
 	return SOAP_OK; 
}


int __tptz10__GetNode(struct soap* soap, struct _tptz10__GetNode *tptz10__GetNode, struct _tptz10__GetNodeResponse *tptz10__GetNodeResponse)
{ 
	ONVIF_DBP("");

	if(!tptz10__GetNode->NodeToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoEntity",
			"No such PTZNode on the device ."));
	}
	
	if(0!=strncmp(tptz10__GetNode->NodeToken,"Anv_ptz_",strlen("Anv_ptz_")))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoEntity",
			"No such PTZNode on the device ."));
	}

	int chno;
	char *p;
	p = tptz10__GetNode->NodeToken+strlen("Anv_ptz_");
	chno = atoi(p);
	if(soap->pConfig->g_channel_num<(chno+1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}

	tptz10__GetNodeResponse->PTZNode = (struct tt__PTZNode *)soap_malloc_v2(soap, sizeof(struct tt__PTZNode));
	//memset(tptz10__GetNodeResponse->PTZNode,0,sizeof(struct tt__PTZNode));
	
	tptz10__GetNodeResponse->PTZNode->token = (char *)soap_malloc(soap,32);
	//memset(tptz10__GetNodeResponse->PTZNode->token,0,32);
	strcpy(tptz10__GetNodeResponse->PTZNode->token,tptz10__GetNode->NodeToken);
	
	tptz10__GetNodeResponse->PTZNode->Name = (char *)soap_malloc(soap,32);
	//memset(tptz10__GetNodeResponse->PTZNode->Name,0,32);
	strcpy(tptz10__GetNodeResponse->PTZNode->Name,tptz10__GetNode->NodeToken);
	
	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces = 
		(struct tt__PTZSpaces *)soap_malloc_v2(soap, sizeof(struct tt__PTZSpaces));
	//memset(tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces,0,sizeof(struct tt__PTZSpaces));

	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->__sizeContinuousPanTiltVelocitySpace = 1;
	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace = 
		(struct tt__Space2DDescription *)soap_malloc_v2(soap, sizeof(struct tt__Space2DDescription));
	//memset(tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace,
	//	0,sizeof(struct tt__Space2DDescription));

	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->URI = 
		(char *)soap_malloc(soap, 128);
	//memset(tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->URI,0,128);
	
	strcpy(tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->URI,
		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocitySpaceDegrees");

	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange = 
		(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange->Max = 1.0;
	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange->Min = -1.0;

	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange = 
		(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange->Max = 1.0;
	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange->Min = -1.0;

	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->__sizeContinuousZoomVelocitySpace = 1;
	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace = 
		(struct tt__Space1DDescription *)soap_malloc_v2(soap, sizeof(struct tt__Space1DDescription));
	//memset(tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace,
	//	0,sizeof(struct tt__Space1DDescription));

	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->URI = 
		(char *)soap_malloc(soap, 128);
	//memset(tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->URI,0,128);
	strcpy(tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->URI,
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocitySpaceMillimeter");

	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange = 
		(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange->Max = 1.0;
	tptz10__GetNodeResponse->PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange->Min = -1.0;

	tptz10__GetNodeResponse->PTZNode->MaximumNumberOfPresets = 255;
	tptz10__GetNodeResponse->PTZNode->HomeSupported = xsd__boolean__true_;
	tptz10__GetNodeResponse->PTZNode->__sizeAuxiliaryCommands = 0;
 
 	return SOAP_OK; 
}

int __tptz10__SetConfiguration(struct soap* soap, struct _tptz10__SetConfiguration *tptz10__SetConfiguration, struct _tptz10__SetConfigurationResponse *tptz10__SetConfigurationResponse)
{
 	ONVIF_DBP(""); 	
 	return SOAP_OK; 
}

int __tptz10__GetConfigurationOptions(struct soap* soap, struct _tptz10__GetConfigurationOptions *tptz10__GetConfigurationOptions, struct _tptz10__GetConfigurationOptionsResponse *tptz10__GetConfigurationOptionsResponse)
{ 
	int chno;
	char *p;
	if(!tptz10__GetConfigurationOptions->ConfigurationToken
		||0!=strncmp(tptz10__GetConfigurationOptions->ConfigurationToken,"Anv_ptz_", strlen("Anv_ptz_")))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}	
	p = tptz10__GetConfigurationOptions->ConfigurationToken+strlen("Anv_ptz_");
	chno = atoi(p);
	if(soap->pConfig->g_channel_num<(chno+1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}
	
	
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions = 
		(struct tt__PTZConfigurationOptions *)soap_malloc_v2(soap,sizeof(struct tt__PTZConfigurationOptions));
	//memset(tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions, 0, sizeof(struct tt__PTZConfigurationOptions));
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces = 
		(struct tt__PTZSpaces *)soap_malloc_v2(soap,sizeof(struct tt__PTZSpaces));
	//memset(tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces, 0, sizeof(struct tt__PTZSpaces));

	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__sizeContinuousPanTiltVelocitySpace = 1;	
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace = 
		(struct tt__Space2DDescription *)soap_malloc_v2(soap,sizeof(struct tt__Space2DDescription));
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->URI = 
		(char *)soap_malloc(soap, 128);
	strcpy(tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->URI,
		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocitySpaceDegrees");
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->XRange = 
		(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->XRange->Max = 1.0;
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->XRange->Min = -1.0;
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->YRange = 
		(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->YRange->Max = 1.0;
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousPanTiltVelocitySpace->YRange->Min = -1.0;
	
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->__sizeContinuousZoomVelocitySpace = 1;
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace = 
		(struct tt__Space1DDescription *)soap_malloc_v2(soap,sizeof(struct tt__Space1DDescription));
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace->URI = 
		(char *)soap_malloc(soap, 128);
	strcpy(tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace->URI,
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocitySpaceMillimeter");
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace->XRange = 
		(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace->XRange->Max = 1.0;
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->Spaces->ContinuousZoomVelocitySpace->XRange->Min = -1.0;

	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout = 
		(struct tt__DurationRange *)soap_malloc_v2(soap,sizeof(struct tt__DurationRange));
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout->Max = 
		(char *)soap_malloc(soap,32);
	strcpy(tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout->Max, "PT3M");
	tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout->Min= 
		(char *)soap_malloc(soap,32);
	strcpy(tptz10__GetConfigurationOptionsResponse->PTZConfigurationOptions->PTZTimeout->Min, "PT30S");
 
	 
 	return SOAP_OK; 
}

int __tptz10__GotoHomePosition(struct soap* soap, struct _tptz10__GotoHomePosition *tptz10__GotoHomePosition, struct _tptz10__GotoHomePositionResponse *tptz10__GotoHomePositionResponse)
{ 

	ONVIF_DBP(""); 


	//send_ptz_cmd(PTZ_CAMERA_RESET, 0);


	if(!tptz10__GotoHomePosition->ProfileToken)
	{
		goto _exit_goto_home_position;
	}
	if(!tptz10__GotoHomePosition->Speed)
	{
		goto _exit_goto_home_position;
	}
	if(tptz10__GotoHomePosition->Speed->PanTilt)
	{
		if(tptz10__GotoHomePosition->Speed->PanTilt->space)
		{
			
		}		
	}
	if (tptz10__GotoHomePosition->Speed->Zoom)
	{
		if(tptz10__GotoHomePosition->Speed->Zoom->space)
		{
			
		}
		
	}
_exit_goto_home_position:

	
 	return SOAP_OK; 
}

int __tptz10__SetHomePosition(struct soap* soap, struct _tptz10__SetHomePosition *tptz10__SetHomePosition, char **tptz10__SetHomePositionResponse)
{  
	ONVIF_DBP("");

	#ifdef PTZ_ON	
 
 	char *position = NULL;
	if(tptz10__SetHomePosition->ProfileToken)
	{
		position = (char *)soap_malloc(soap,128);
		tptz10__SetHomePositionResponse = &position;
	}

#endif
 
 	return SOAP_OK; 
}

int __tptz10__ContinuousMove(struct soap* soap, struct _tptz10__ContinuousMove *tptz10__ContinuousMove, struct _tptz10__ContinuousMoveResponse *tptz10__ContinuousMoveResponse)
{

	ONVIF_DBP("");	

	//int direction = QMAPI_PTZ_STA_LEFT;
	int chno;
	char *p;

	if(!tptz10__ContinuousMove->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz10__ContinuousMove->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz10__ContinuousMove->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz10__ContinuousMove->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}

	if(tptz10__ContinuousMove->Velocity)
	{
		if(tptz10__ContinuousMove->Velocity->PanTilt)
		{
			printf("pantilt x:%f y:%f \n",tptz10__ContinuousMove->Velocity->PanTilt->x,
				tptz10__ContinuousMove->Velocity->PanTilt->y);
			if(((tptz10__ContinuousMove->Velocity->PanTilt->x >= - EPSINON) &&(tptz10__ContinuousMove->Velocity->PanTilt->x <= EPSINON)) &&
				((tptz10__ContinuousMove->Velocity->PanTilt->y >= - EPSINON) &&(tptz10__ContinuousMove->Velocity->PanTilt->y <= EPSINON)))
			{
				//printf("PTZ X:%f Y:%f \n",tptz__ContinuousMove->Velocity->PanTilt->x,tptz__ContinuousMove->Velocity->PanTilt->y);
				if(tptz10__ContinuousMove->Velocity->Zoom
					&&tptz10__ContinuousMove->Velocity->Zoom->x <0.000001
					&&tptz10__ContinuousMove->Velocity->Zoom->x >-0.000001)
				{
					printf("Zoom X:%f \n",tptz10__ContinuousMove->Velocity->Zoom->x);
					ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
					printf("======PTZ stop======\n");
					return SOAP_OK;
				}
			}
			else
			{
				if((tptz10__ContinuousMove->Velocity->PanTilt->x>180.0 ||
					tptz10__ContinuousMove->Velocity->PanTilt->x<-180.0)||
					(tptz10__ContinuousMove->Velocity->PanTilt->y>90.0 ||
					tptz10__ContinuousMove->Velocity->PanTilt->y<-90.0))
				{
					goto _exit_continuous_move_error;
				}
				
				if(tptz10__ContinuousMove->Velocity->PanTilt->x > 0.0)
				{
					if(tptz10__ContinuousMove->Velocity->PanTilt->y > 0.0)
					{
						if(tptz10__ContinuousMove->Velocity->PanTilt->x > tptz10__ContinuousMove->Velocity->PanTilt->y)
						{
							//right
							ptz_control(QMAPI_PTZ_STA_RIGHT,0,chno);
							printf("======PTZ RIGHT======\n\n");
						}
						else
						{
							//up
							ptz_control(QMAPI_PTZ_STA_UP,0,chno);
							printf("======PTZ up======\n\n");
						}
					}
					else if(tptz10__ContinuousMove->Velocity->PanTilt->y <= 0.0)
					{
						if(tptz10__ContinuousMove->Velocity->PanTilt->x > -tptz10__ContinuousMove->Velocity->PanTilt->y)
						{
							//right
							ptz_control(QMAPI_PTZ_STA_RIGHT,0,chno);
							printf("======PTZ right======\n\n");
						}
						else
						{
							//down
							ptz_control(QMAPI_PTZ_STA_DOWN,0,chno);
							printf("======PTZ down======\n\n");
						}
					}
				}
				else if(tptz10__ContinuousMove->Velocity->PanTilt->x < 0.000001
					&&tptz10__ContinuousMove->Velocity->PanTilt->x >- 0.000001
					&& tptz10__ContinuousMove->Velocity->PanTilt->y<0.000001
					&& tptz10__ContinuousMove->Velocity->PanTilt->y>-0.000001
					&&tptz10__ContinuousMove->Velocity->Zoom
					&&tptz10__ContinuousMove->Velocity->Zoom->x <0.000001
					&&tptz10__ContinuousMove->Velocity->Zoom->x >-0.000001)
				{
					ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
					printf("00======PTZ stop======\n");
				}
				else
				{
					if(tptz10__ContinuousMove->Velocity->PanTilt->y > 0.0)
					{
						if(-tptz10__ContinuousMove->Velocity->PanTilt->x > tptz10__ContinuousMove->Velocity->PanTilt->y)
						{
							//left
							ptz_control(QMAPI_PTZ_STA_LEFT,0,chno);
							printf("======PTZ left======\n\n");
						}
						else
						{
							//up
							ptz_control(QMAPI_PTZ_STA_UP,0,chno);
							printf("======PTZ up======\n\n");
						}
					}
					else if(tptz10__ContinuousMove->Velocity->PanTilt->y <= 0.0)
					{
						if(-tptz10__ContinuousMove->Velocity->PanTilt->x > -tptz10__ContinuousMove->Velocity->PanTilt->y)
						{
							//left
							ptz_control(QMAPI_PTZ_STA_LEFT,0,chno);
							printf("======PTZ left======\n\n");
						}
						else
						{
							//down
							ptz_control(QMAPI_PTZ_STA_DOWN,0,chno);
							printf("======PTZ down======\n\n");
						}
					}
				}
			}
		}
		if(tptz10__ContinuousMove->Velocity->Zoom)
		{
			printf("Zoom x:%f \n",tptz10__ContinuousMove->Velocity->Zoom->x);
			if((tptz10__ContinuousMove->Velocity->Zoom->x >= - EPSINON) &&(tptz10__ContinuousMove->Velocity->Zoom->x <= EPSINON))
			{
				//printf("Zoom X:%f \n",tptz__ContinuousMove->Velocity->Zoom->x);
			}
			else
			{
				if(tptz10__ContinuousMove->Velocity->Zoom->x>1000.0 ||
					tptz10__ContinuousMove->Velocity->Zoom->x<-1000.0)
				{
					goto _exit_continuous_move_error;
				}
				if(tptz10__ContinuousMove->Velocity->Zoom->x > EPSINON)
				{
					ptz_control(QMAPI_PTZ_STA_ZOOM_ADD,0,chno);
					printf("Zoom ++++++++ \n\n");
				}
				else
				{
					ptz_control(QMAPI_PTZ_STA_ZOOM_SUB,0,chno);
					printf("Zoom -------- \n\n");
				} 
			}
		}
	}

	return SOAP_OK;

_exit_continuous_move_error:
	return(send_soap_fault_msg(soap,400,
		"SOAP-ENV:Sender",
		"ter:InvalidArgVal",
		"ter:InvalidVelocity  ",
		"The requested velocity is out of bounds. "));
	
  	return SOAP_OK; 
}

int __tptz10__RelativeMove(struct soap* soap, struct _tptz10__RelativeMove *tptz10__RelativeMove, struct _tptz10__RelativeMoveResponse *tptz10__RelativeMoveResponse)
{ 
	ONVIF_DBP("");

	int direction = 0;
	float x = 0.0, y = 0.0;
	

	if ( tptz10__RelativeMove->ProfileToken ) {
		ONVIF_DBP("=tptz10__RelativeMove->ProfileToken %s \n", tptz10__RelativeMove->ProfileToken);
	}	

	int chno;
	char *p;

	if(!tptz10__RelativeMove->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz10__RelativeMove->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz10__RelativeMove->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz10__RelativeMove->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}


	if ( tptz10__RelativeMove->Translation ) {

		if ( tptz10__RelativeMove->Translation->PanTilt ) {

			if ( tptz10__RelativeMove->Translation->PanTilt->x ) {				
				x = tptz10__RelativeMove->Translation->PanTilt->x;
			}

			if ( tptz10__RelativeMove->Translation->PanTilt->y ) {			
				y = tptz10__RelativeMove->Translation->PanTilt->y;
			}

			if ((x >= - EPSINON) && (x <= EPSINON)) {	// x = 0;

				if ((y >= - EPSINON) && (y <= EPSINON)) {

				} else if ( y < - EPSINON ) {
					direction = QMAPI_PTZ_STA_DOWN;					
				} else if ( y > EPSINON ) {
					direction = QMAPI_PTZ_STA_UP;					
				}
			} else if ( x > EPSINON ) { //  x > 0, 

				if ((y >= - EPSINON) && (y <= EPSINON)) {
					direction = QMAPI_PTZ_STA_RIGHT;

				} else if ( y > EPSINON) {
					direction = QMAPI_PTZ_STA_RIGHT;					
				} else {
					direction = QMAPI_PTZ_STA_RIGHT;					
				}
			} else if ( x < - EPSINON) { // x  <  0

				if ((y >= - EPSINON) && (y <= EPSINON)) {
					direction = QMAPI_PTZ_STA_LEFT;

				} else if ( y > EPSINON) {
					direction = QMAPI_PTZ_STA_LEFT;					
				} else {
					direction = QMAPI_PTZ_STA_LEFT;					
				}
			}
			/*
			send_ptz_cmd(direction, 0);	
			usleep(80000);	
			send_ptz_cmd(PTZ_STOP, 0);
			*/
			ptz_control(direction,0,chno);
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 80000;
			select(0, NULL, NULL, NULL, &tv);
			ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
			
		}

		if ( tptz10__RelativeMove->Translation->Zoom ) {

			if ( ( tptz10__RelativeMove->Translation->Zoom->x >= - EPSINON ) && 
				tptz10__RelativeMove->Translation->Zoom->x <= EPSINON ) {

			} else {

				if ( tptz10__RelativeMove->Translation->Zoom->x > EPSINON ) {			
					//send_ptz_cmd(PTZ_ZOOM_IN);				
					ptz_control(QMAPI_PTZ_STA_ZOOM_ADD,0,chno);
				} else {			
					//send_ptz_cmd(PTZ_ZOOM_OUT);				
					ptz_control(QMAPI_PTZ_STA_ZOOM_SUB,0,chno);
				}	

				struct timeval tv;
				tv.tv_sec = 0;
				tv.tv_usec = 80000;
				select(0, NULL, NULL, NULL, &tv);
				//send_ptz_cmd(PTZ_STOP, 0);
				ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
			}
			
		}		

	}



	if ( tptz10__RelativeMove->Speed ) {

		if ( tptz10__RelativeMove->Speed->PanTilt ) {

			if ( tptz10__RelativeMove->Speed->PanTilt->space ) {
				ONVIF_DBP("=========== tptz10__RelativeMove->Speed PanTilt space %s ",
					tptz10__RelativeMove->Speed->PanTilt->space);
			}

			if ( tptz10__RelativeMove->Speed->PanTilt->x ) {
				ONVIF_DBP("Speed x %f", tptz10__RelativeMove->Speed->PanTilt->x);
			}

			if ( tptz10__RelativeMove->Speed->PanTilt->y ) {
				ONVIF_DBP("Speed y %f",	tptz10__RelativeMove->Translation->PanTilt->y);
			}
		}

		if ( tptz10__RelativeMove->Speed->Zoom ) {

			if ( tptz10__RelativeMove->Speed->Zoom->space ) {
				ONVIF_DBP("Zoom space %s ", tptz10__RelativeMove->Speed->Zoom->space);
			}

			if ( tptz10__RelativeMove->Speed->Zoom->x ) {
				ONVIF_DBP("Zoom   x %f\n", tptz10__RelativeMove->Speed->Zoom->x);
			}
		}		

	}


	

	if(!tptz10__RelativeMove->ProfileToken)
	{
		goto _exit_relative_move;
	}
	if(!tptz10__RelativeMove->Translation)
	{
		goto _exit_relative_move;
	}
	if(!tptz10__RelativeMove->Speed)
	{
		goto _exit_relative_move;
	}
	if(tptz10__RelativeMove->Translation->PanTilt)
	{
		if(tptz10__RelativeMove->Translation->PanTilt->space)
		{
			
		}	
	}
	if(tptz10__RelativeMove->Translation->Zoom)
	{
		if(tptz10__RelativeMove->Translation->Zoom->space)
		{
			
		}		
	}
	if(tptz10__RelativeMove->Speed->PanTilt)
	{
		if(tptz10__RelativeMove->Speed->PanTilt->space)
		{
			
		}
	
	}
	if(tptz10__RelativeMove->Speed->Zoom)
	{
		if(tptz10__RelativeMove->Speed->Zoom->space)
		{
			
		}		
	}
_exit_relative_move:

 
 	return SOAP_OK; 
	
	
 }

 int __tptz10__SendAuxiliaryCommand(struct soap* soap, struct _tptz10__SendAuxiliaryCommand *tptz10__SendAuxiliaryCommand, struct _tptz10__SendAuxiliaryCommandResponse *tptz10__SendAuxiliaryCommandResponse)
 { 
 	ONVIF_DBP("");
 
 	return SOAP_OK; 
 }

int __tptz10__AbsoluteMove(struct soap* soap, struct _tptz10__AbsoluteMove *tptz10__AbsoluteMove, struct _tptz10__AbsoluteMoveResponse *tptz10__AbsoluteMoveResponse)
{ 
 	ONVIF_DBP("");		

	int direction = 0;
	float x = 0.0, y = 0.0;	

	if(!tptz10__AbsoluteMove->ProfileToken)
	{
		goto _exit_absolute_move;
	}

	int chno;
	char *p;

	if(!tptz10__AbsoluteMove->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested profile does not exist. "));
	}
	
	if(strncmp(tptz10__AbsoluteMove->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz10__AbsoluteMove->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz10__AbsoluteMove->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}

#if 1
	if (  tptz10__AbsoluteMove->ProfileToken ) {
		ONVIF_DBP("=== tptz10__AbsoluteMove->ProfileToken %s \n", tptz10__AbsoluteMove->ProfileToken);
	}
	
	if ( tptz10__AbsoluteMove->Destination) {
		if ( tptz10__AbsoluteMove->Destination->PanTilt ) {	
			x = tptz10__AbsoluteMove->Destination->PanTilt->x;
			y = tptz10__AbsoluteMove->Destination->PanTilt->y;
		
			if ((x >= - EPSINON) && (x <= EPSINON)) {	// x = 0;

				if ((y >= - EPSINON) && (y <= EPSINON)) {

				} else if ( y < - EPSINON ) {
					direction = QMAPI_PTZ_STA_DOWN;									
				} else if ( y > EPSINON ) {
					direction = QMAPI_PTZ_STA_UP;					
				}
			} else if ( x > EPSINON ) { //  x > 0, 
				if ((y >= - EPSINON) && (y <= EPSINON)) {
					direction = QMAPI_PTZ_STA_RIGHT;
				}else if ( y > EPSINON) {
					direction = QMAPI_PTZ_STA_RIGHT;					
				} else {
					direction = QMAPI_PTZ_STA_RIGHT;					
				}
			} else if ( x < - EPSINON) { // x  <  0
				if ((y >= - EPSINON) && (y <= EPSINON)) {
					direction = QMAPI_PTZ_STA_LEFT;
				} else if ( y > EPSINON) {
					direction = QMAPI_PTZ_STA_LEFT;					
				} else {
					direction = QMAPI_PTZ_STA_LEFT;				
				}
			}

			/*
			send_ptz_cmd(direction, 0);	
			usleep(80000);	
			send_ptz_cmd(PTZ_STOP, 0);	
			*/
			ptz_control(direction,0,chno);
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 80000;
			select(0, NULL, NULL, NULL, &tv);
			ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
		}
		
		if ( tptz10__AbsoluteMove->Destination->Zoom ) {	

			if ( ( tptz10__AbsoluteMove->Destination->Zoom->x >= - EPSINON ) && 
				tptz10__AbsoluteMove->Destination->Zoom->x <= EPSINON ) {

			} else  {

				if ( tptz10__AbsoluteMove->Destination->Zoom->x > EPSINON ) {
					//send_ptz_cmd(PTZ_ZOOM_IN);					
					ptz_control(QMAPI_PTZ_STA_ZOOM_ADD,0,chno);
				} else {
					//send_ptz_cmd(PTZ_ZOOM_OUT);					
					ptz_control(QMAPI_PTZ_STA_ZOOM_SUB,0,chno);
				}	

				struct timeval tv;
				tv.tv_sec = 0;
				tv.tv_usec = 80000;
				select(0, NULL, NULL, NULL, &tv);
				//send_ptz_cmd(PTZ_STOP, 0);	
				ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
			}
		}		
	}
#else
	int ret;
	QMAPI_NET_PTZ_POS stNetPtzPos;
	memset(&stNetPtzPos, 0, sizeof(stNetPtzPos));
	stNetPtzPos.dwSize = sizeof(QMAPI_NET_PTZ_POS);

	if (tptz10__AbsoluteMove->ProfileToken)
	{
		ONVIF_DBP("=== tptz10__AbsoluteMove->ProfileToken %s \n", tptz10__AbsoluteMove->ProfileToken);
	}
	
	if(tptz10__AbsoluteMove->Destination)
	{
		if(tptz10__AbsoluteMove->Destination->PanTilt)
		{	
			x = tptz10__AbsoluteMove->Destination->PanTilt->x;
			y = tptz10__AbsoluteMove->Destination->PanTilt->y;

			stNetPtzPos.nPanPos = x;
			stNetPtzPos.nTiltPos = y;			
		}
		
		if(tptz10__AbsoluteMove->Destination->Zoom)
		{	
			if((tptz10__AbsoluteMove->Destination->Zoom->x >= - EPSINON)
				&& tptz10__AbsoluteMove->Destination->Zoom->x <= EPSINON)
			{

			} 
			else
			{
				stNetPtzPos.nZoomPos = tptz10__AbsoluteMove->Destination->Zoom->x;
			}
		}		
	}
	
	ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_PTZPOS, 0, &stNetPtzPos, sizeof(QMAPI_NET_PTZ_POS));
	if(0 != ret)
	{
		printf("# [%s]:line=%d\n", __FUNCTION__, __LINE__);
	}
#endif
	if(tptz10__AbsoluteMove->Destination)
	{
		if(tptz10__AbsoluteMove->Destination->PanTilt)
		{
			if(tptz10__AbsoluteMove->Destination->PanTilt->space)
			{
			}
		}
		if(tptz10__AbsoluteMove->Destination->Zoom)
		{
			if(tptz10__AbsoluteMove->Destination->Zoom->space)
			{
			}		
		}
	}

	if(tptz10__AbsoluteMove->Speed)
	{
		if(tptz10__AbsoluteMove->Speed->PanTilt)
		{
			if(tptz10__AbsoluteMove->Speed->PanTilt->space)
			{				
			}			
		}
		if(tptz10__AbsoluteMove->Speed->Zoom)
		{
			if(tptz10__AbsoluteMove->Speed->Zoom->space)
			{
			}			
		}
	}

_exit_absolute_move:
 	return SOAP_OK; 
}

 int __tptz10__Stop(struct soap* soap, struct _tptz10__StopRequest *tptz10__StopRequest, struct _tptz10__StopResponse *tptz10__StopResponse)
 { 

 	ONVIF_DBP("");

	int chno;
	char *p;

	if(!tptz10__StopRequest->ProfileToken)
	{
		/*
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested profile does not exist. "));
		*/
		chno = 0;
	}

	if(strncmp(tptz10__StopRequest->ProfileToken,"axxon", strlen("axxon")) == 0)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(tptz10__StopRequest->ProfileToken,"profile", strlen("profile")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}	
		p = tptz10__StopRequest->ProfileToken+strlen("profile");
		chno = atoi(p);
		if(soap->pConfig->g_channel_num<(chno+1))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested profile does not exist. "));
		}
	}
	ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
	printf("00======PTZ stop======\n");
 
 	return SOAP_OK; 
 }


int __trc__GetServiceCapabilities(struct soap *soap, struct _trc__GetServiceCapabilities *trc__GetServiceCapabilities, struct _trc__GetServiceCapabilitiesResponse *trc__GetServiceCapabilitiesResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}
 
int  __trc__CreateRecording(struct soap *soap, struct _trc__CreateRecording *trc__CreateRecording, struct _trc__CreateRecordingResponse *trc__CreateRecordingResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__DeleteRecording(struct soap *soap, struct _trc__DeleteRecording *trc__DeleteRecording, struct _trc__DeleteRecordingResponse *trc__DeleteRecordingResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__GetRecordings(struct soap *soap, struct _trc__GetRecordings *trc__GetRecordings, struct _trc__GetRecordingsResponse *trc__GetRecordingsResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__SetRecordingConfiguration(struct soap *soap, struct _trc__SetRecordingConfiguration *trc__SetRecordingConfiguration, struct _trc__SetRecordingConfigurationResponse *trc__SetRecordingConfigurationResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__GetRecordingConfiguration(struct soap *soap, struct _trc__GetRecordingConfiguration *trc__GetRecordingConfiguration, struct _trc__GetRecordingConfigurationResponse *trc__GetRecordingConfigurationResponse) 
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__CreateTrack(struct soap *soap, struct _trc__CreateTrack *trc__CreateTrack, struct _trc__CreateTrackResponse *trc__CreateTrackResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__DeleteTrack(struct soap *soap, struct _trc__DeleteTrack *trc__DeleteTrack, struct _trc__DeleteTrackResponse *trc__DeleteTrackResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__GetTrackConfiguration(struct soap *soap, struct _trc__GetTrackConfiguration *trc__GetTrackConfiguration, struct _trc__GetTrackConfigurationResponse *trc__GetTrackConfigurationResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__SetTrackConfiguration(struct soap *soap, struct _trc__SetTrackConfiguration *trc__SetTrackConfiguration, struct _trc__SetTrackConfigurationResponse *trc__SetTrackConfigurationResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__CreateRecordingJob(struct soap *soap, struct _trc__CreateRecordingJob *trc__CreateRecordingJob, struct _trc__CreateRecordingJobResponse *trc__CreateRecordingJobResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}


int  __trc__DeleteRecordingJob(struct soap *soap, struct _trc__DeleteRecordingJob *trc__DeleteRecordingJob, struct _trc__DeleteRecordingJobResponse *trc__DeleteRecordingJobResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__GetRecordingJobs(struct soap *soap, struct _trc__GetRecordingJobs *trc__GetRecordingJobs, struct _trc__GetRecordingJobsResponse *trc__GetRecordingJobsResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__SetRecordingJobConfiguration(struct soap *soap, struct _trc__SetRecordingJobConfiguration *trc__SetRecordingJobConfiguration, struct _trc__SetRecordingJobConfigurationResponse *trc__SetRecordingJobConfigurationResponse)	
{	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__GetRecordingJobConfiguration(struct soap *soap, struct _trc__GetRecordingJobConfiguration *trc__GetRecordingJobConfiguration, struct _trc__GetRecordingJobConfigurationResponse *trc__GetRecordingJobConfigurationResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__SetRecordingJobMode(struct soap *soap, struct _trc__SetRecordingJobMode *trc__SetRecordingJobMode, struct _trc__SetRecordingJobModeResponse *trc__SetRecordingJobModeResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trc__GetRecordingJobState(struct soap *soap, struct _trc__GetRecordingJobState *trc__GetRecordingJobState, struct _trc__GetRecordingJobStateResponse *trc__GetRecordingJobStateResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trp__GetReplayUri(struct soap *soap, struct _trp__GetReplayUri *trp__GetReplayUri, struct _trp__GetReplayUriResponse *trp__GetReplayUriResponse)
{	
	ONVIF_DBP("");
	return SOAP_OK;
}

int __trp__GetServiceCapabilities(struct soap *soap, struct _trp__GetServiceCapabilities *trp__GetServiceCapabilities, struct _trp__GetServiceCapabilitiesResponse *trp__GetServiceCapabilitiesResponse)
{	 
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trp__GetReplayConfiguration(struct soap *soap, struct _trp__GetReplayConfiguration *trp__GetReplayConfiguration, struct _trp__GetReplayConfigurationResponse *trp__GetReplayConfigurationResponse)
{	  
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trp__SetReplayConfiguration(struct soap *soap, struct _trp__SetReplayConfiguration *trp__SetReplayConfiguration, struct _trp__SetReplayConfigurationResponse *trp__SetReplayConfigurationResponse)
{	  
	ONVIF_DBP("");
	return SOAP_OK;
}

 int  __trt__GetVideoSources(struct soap *soap,
	 struct _trt__GetVideoSources *trt__GetVideoSources,
	 struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse)
 {
 
	ONVIF_DBP("");

	int ret;
	VIDEO_CONF video[MAX_STREAM_NUM];
	memset(video,0,sizeof(VIDEO_CONF)*MAX_STREAM_NUM);

	//int nstreamNum = soap->pConfig->g_channel_num*MAX_STREAM_NUM;
	QMAPI_NET_CHANNEL_COLOR colorcfg;
	QMAPI_NET_CHANNEL_ENHANCED_COLOR enhanced_color;
	trt__GetVideoSourcesResponse->__sizeVideoSources = soap->pConfig->g_channel_num;
	trt__GetVideoSourcesResponse->VideoSources = (struct tt__VideoSource *)soap_malloc_v2(soap,sizeof(struct tt__VideoSource)*soap->pConfig->g_channel_num);

	int i;
	for(i=0;i<soap->pConfig->g_channel_num;i++)
	{
		if(get_channel_param(i,video) != 0)
		{
			return SOAP_ERR;
		}
		ret = get_color_param(i, &colorcfg);
		if(ret < 0)
		{
			printf("%s get_color_param error! ret=%d\n", __FUNCTION__, ret);
			return SOAP_ERR;
		}

		ret = get_enhanced_color(i, &enhanced_color);
		if(ret < 0)
		{
			printf("%s get_color_param error! ret=%d\n", __FUNCTION__, ret);
			return SOAP_ERR;
		}
		
		trt__GetVideoSourcesResponse->VideoSources[i].token = (char *)soap_malloc(soap, 16);
		sprintf(trt__GetVideoSourcesResponse->VideoSources[i].token,"VideoSource%d",i);

		trt__GetVideoSourcesResponse->VideoSources[i].Framerate = video[0].fps;
		trt__GetVideoSourcesResponse->VideoSources[i].Resolution = (struct tt__VideoResolution *)soap_malloc_v2(soap,sizeof(struct tt__VideoResolution));
		trt__GetVideoSourcesResponse->VideoSources[i].Resolution->Height = soap->pConfig->videosource_height;
		trt__GetVideoSourcesResponse->VideoSources[i].Resolution->Width = soap->pConfig->videosource_width;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging = (struct tt__ImagingSettings *)soap_malloc_v2(soap,sizeof(struct tt__ImagingSettings));
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Brightness = (float *)soap_malloc(soap,sizeof(float));
		*(trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Brightness) = (float)colorcfg.nBrightness;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->ColorSaturation = (float *)soap_malloc(soap,sizeof(float));
		*(trt__GetVideoSourcesResponse->VideoSources[i].Imaging->ColorSaturation) = (float)colorcfg.nSaturation;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Contrast = (float *)soap_malloc(soap,sizeof(float));
		*(trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Contrast) = (float)colorcfg.nContrast;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Sharpness = (float *)soap_malloc(soap,sizeof(float));
		*(trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Sharpness) = (float)colorcfg.nDefinition;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WideDynamicRange = (struct tt__WideDynamicRange *)soap_malloc_v2(soap,sizeof(struct tt__WideDynamicRange));
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WideDynamicRange->Mode = enhanced_color.bWideDynamic ? tt__WideDynamicMode__ON : tt__WideDynamicMode__OFF;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WideDynamicRange->Level = enhanced_color.nWDLevel;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WhiteBalance = (struct tt__WhiteBalance *)soap_malloc_v2(soap,sizeof(struct tt__WhiteBalance));
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WhiteBalance->Mode = enhanced_color.bEnableAWB?tt__WhiteBalanceMode__AUTO:tt__WhiteBalanceMode__MANUAL;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WhiteBalance->CrGain = (float)enhanced_color.nRed;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WhiteBalance->CbGain = (float)enhanced_color.nBlue;
		
	}
	 
 
	 return SOAP_OK;
 
	 
 }


 int __trt__GetAudioSources(struct soap *soap, struct _trt__GetAudioSources *trt__GetAudioSources, struct _trt__GetAudioSourcesResponse *trt__GetAudioSourcesResponse)
 {

 	ONVIF_DBP("");

	int i;
	trt__GetAudioSourcesResponse->__sizeAudioSources = soap->pConfig->g_channel_num;
	
	trt__GetAudioSourcesResponse->AudioSources = (struct tt__AudioSource *)soap_malloc_v2(soap,sizeof(struct tt__AudioSource)*soap->pConfig->g_channel_num);

	for(i=0;i<soap->pConfig->g_channel_num;i++)
	{
		trt__GetAudioSourcesResponse->AudioSources[i].token = (char *)soap_malloc(soap, 32);
		//strcpy(trt__GetAudioSourcesResponse->AudioSources->token,"axxon0");
		sprintf(trt__GetAudioSourcesResponse->AudioSources[i].token,"AudioSource%d",i);

		trt__GetAudioSourcesResponse->AudioSources[i].Channels = 1;
	}

	return SOAP_OK;
 
 }


int  __trt__GetAudioOutputs(struct soap *soap, struct _trt__GetAudioOutputs *trt__GetAudioOutputs, struct _trt__GetAudioOutputsResponse *trt__GetAudioOutputsResponse)
{
#if 0
	trt__GetAudioOutputsResponse->__sizeAudioOutputs = 1;
	trt__GetAudioOutputsResponse->AudioOutputs = (struct tt__AudioOutput *)soap_malloc(soap, sizeof(struct tt__AudioOutput));
	
#endif
	return SOAP_OK;
}


int __trt__CreateProfile(struct soap *soap, struct _trt__CreateProfile *trt__CreateProfile, struct _trt__CreateProfileResponse *trt__CreateProfileResponse)
{

	ONVIF_DBP("");

	if(!trt__CreateProfile->Name || soap->pConfig->profile_cfg.isVaild)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Receiver",
 			"ter:Action",
 			"ter:CreateProfile",
 			"The CreateProfile Name is NULL"));
	}

	int ret, chno= 0, stream_index = 0;
	if(strncmp(trt__CreateProfile->Name, "profile", strlen("profile")))
	{
		ret = sscanf(trt__CreateProfile->Name, "profile%d_%d",&chno, &stream_index);
		if(ret == 2 && chno >0 && stream_index>0 &&(chno <= (soap->pConfig->g_channel_num-1)) && 
			(stream_index <= (MAX_STREAM_NUM -1)))
		{
			
			return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
                          	"ter:InvalidArgVal",
                          	"ter:ProfileExists",
                          	"The profile is already exists."));
		}
	}

	trt__CreateProfileResponse->Profile = (struct tt__Profile*)soap_malloc_v2(soap, sizeof(struct tt__Profile));
	
	trt__CreateProfileResponse->Profile->Name = soap_strdup(soap, trt__CreateProfile->Name);
	strncpy(soap->pConfig->profile_cfg.name, trt__CreateProfile->Name, 64);
	
	if (trt__CreateProfile->Token == NULL)
	{
		trt__CreateProfileResponse->Profile->token = soap_strdup(soap, "");
		soap->pConfig->profile_cfg.token[0] = '\0';
	}
	else
	{
		trt__CreateProfileResponse->Profile->token = soap_strdup(soap, trt__CreateProfile->Token);
		strncpy(soap->pConfig->profile_cfg.token, trt__CreateProfile->Token, 64);
	}

	strcpy(soap->pConfig->profile_cfg.audioSoureToken, "AudioSourceConfiguration0");
	strcpy(soap->pConfig->profile_cfg.audioEncoderToken, "AudioEncoderConfiguration0");
	strcpy(soap->pConfig->profile_cfg.videoSoureToken, "VideoSourceConfiguration0");
	strcpy(soap->pConfig->profile_cfg.videoEncoderToken, "VideoEncoderConfiguration0_0");	
	soap->pConfig->profile_cfg.isVaild = 1;
	
	return SOAP_OK;
}

static int GetTempProfile(struct soap *soap, struct _trt__GetProfileResponse *trt__GetProfileResponse)
{
	int chn;
	
	trt__GetProfileResponse->Profile = (struct tt__Profile*)soap_malloc_v2(soap, sizeof(struct tt__Profile));

	trt__GetProfileResponse->Profile->fixed = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetProfileResponse->Profile->fixed = xsd__boolean__false_;
		
	trt__GetProfileResponse->Profile->Name = soap->pConfig->profile_cfg.name;
	trt__GetProfileResponse->Profile->token = soap->pConfig->profile_cfg.token;
	
	int ch, index;
	if (soap->pConfig->profile_cfg.audioSoureToken[0])
	{
		sscanf(soap->pConfig->profile_cfg.audioSoureToken, "AudioSourceConfiguration%d", &chn);
		
		trt__GetProfileResponse->Profile->AudioSourceConfiguration =  (struct tt__AudioSourceConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioSourceConfiguration));
	
		trt__GetProfileResponse->Profile->AudioEncoderConfiguration =  (struct tt__AudioEncoderConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioEncoderConfiguration));
		trt__GetProfileResponse->Profile->AudioSourceConfiguration->Name = soap->pConfig->profile_cfg.audioSoureToken;
		trt__GetProfileResponse->Profile->AudioSourceConfiguration->UseCount = MAX_STREAM_NUM + 1;
		trt__GetProfileResponse->Profile->AudioSourceConfiguration->token = soap->pConfig->profile_cfg.audioSoureToken;
		trt__GetProfileResponse->Profile->AudioSourceConfiguration->SourceToken =  (char *)soap_malloc(soap, 16);
		sprintf(trt__GetProfileResponse->Profile->AudioSourceConfiguration->SourceToken,"AudioSource%d",chn);
	}

	if (soap->pConfig->profile_cfg.audioEncoderToken[0])
	{
		sscanf(soap->pConfig->profile_cfg.audioSoureToken, "AudioEncoderConfiguration%d", &chn);	
		trt__GetProfileResponse->Profile->AudioEncoderConfiguration = (struct tt__AudioEncoderConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioEncoderConfiguration));
		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Name = soap->pConfig->profile_cfg.audioEncoderToken;
		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->UseCount = MAX_STREAM_NUM + 1;
		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->token = soap->pConfig->profile_cfg.audioEncoderToken;

		QMAPI_NET_CHANNEL_PIC_INFO info;
		memset(&info, 0, sizeof(info));
		info.dwSize = sizeof(info);
		QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, chn, &info, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Encoding = info.stRecordPara.wFormatTag == QMAPI_PT_G726 ? tt__AudioEncoding__G726 : tt__AudioEncoding__G711;
		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Bitrate = 8;

		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->SampleRate=16;

		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));
		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
		*trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address->IPv4Address = soap_strdup(soap, RTP_MULTICAST_IP);
		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Port = 0;
		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->TTL = 1;

		trt__GetProfileResponse->Profile->AudioEncoderConfiguration->SessionTimeout = soap_strdup(soap, "PT30M");		
	}
	
	if (soap->pConfig->profile_cfg.videoSoureToken[0])
	{
		sscanf(soap->pConfig->profile_cfg.videoSoureToken, "VideoSourceConfiguration%d", &ch);
		printf("ch = %d\n", ch);

		trt__GetProfileResponse->Profile->VideoSourceConfiguration = (struct tt__VideoSourceConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__VideoSourceConfiguration));
		trt__GetProfileResponse->Profile->VideoSourceConfiguration->Name = soap->pConfig->profile_cfg.videoSoureToken;
		trt__GetProfileResponse->Profile->VideoSourceConfiguration->UseCount = MAX_STREAM_NUM + 1;
		trt__GetProfileResponse->Profile->VideoSourceConfiguration->token = soap->pConfig->profile_cfg.videoSoureToken;
		trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken = (char *)soap_malloc(soap, 32);
		sprintf(trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken,"VideoSource%d",chn);
		trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds = (struct tt__IntRectangle *)soap_malloc_v2(soap, sizeof(struct tt__IntRectangle));
		trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->x = 0;
		trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->y =  0;
		trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->width = 1920;
		trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->height = 1080;
	}
	
	if (soap->pConfig->profile_cfg.videoEncoderToken[0])
	{
		sscanf(soap->pConfig->profile_cfg.videoEncoderToken, "VideoEncoderConfiguration%d_%d", &ch, &index);
		printf("ch = %d index = %d\n", ch, index);

		VIDEO_CONF video[MAX_STREAM_NUM];
		memset(video,0,sizeof(VIDEO_CONF)*MAX_STREAM_NUM);
		
		if(get_channel_param(chn,video) != 0)
		{
			return SOAP_ERR;
		}

		trt__GetProfileResponse->Profile->VideoEncoderConfiguration = (struct tt__VideoEncoderConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__VideoEncoderConfiguration));
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Name = soap->pConfig->profile_cfg.videoEncoderToken;
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->UseCount = 2;
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token = soap->pConfig->profile_cfg.videoEncoderToken;
	
		if(QMAPI_PT_JPEG == video[index].entype
				|| QMAPI_PT_MJPEG == video[index].entype)
		{
			trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Encoding =  tt__VideoEncoding__JPEG;
		}
		else if(QMAPI_PT_H264 == video[index].entype 
			|| QMAPI_PT_H264_HIGHPROFILE == video[index].entype
			|| QMAPI_PT_H264_BASELINE == video[index].entype
			|| QMAPI_PT_H264_MAINPROFILE == video[index].entype
			|| QMAPI_PT_H265 == video[index].entype)
		{
			trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Encoding =  tt__VideoEncoding__H264;
	
			trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264 =
				soap_malloc_v2(soap,sizeof(struct tt__H264Configuration));
			trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->GovLength = video[index].gop;
	
			switch(video[index].entype)
			{
				case QMAPI_PT_H264_MAINPROFILE:
				case QMAPI_PT_H264:
					trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Main;
					break;
				case QMAPI_PT_H264_BASELINE:
					trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Baseline;
					break;
				default:
					trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__High;
					break;
			}
		}
	
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution = (struct tt__VideoResolution *)soap_malloc_v2(soap, sizeof(struct tt__VideoResolution));
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Width = video[index].width;
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Height = video[index].height;
		
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Quality = 5 - video[index].piclevel;
		
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));
	
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
	
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
		*trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address->IPv4Address = soap_strdup(soap, RTP_MULTICAST_IP);
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Port = 0;
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->TTL = 1;
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->SessionTimeout = soap_strdup(soap, "PT30M");
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl = (struct tt__VideoRateControl *)soap_malloc_v2(soap, sizeof(struct tt__VideoRateControl));
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->BitrateLimit = video[index].bitrate;
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->EncodingInterval = 1;
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->FrameRateLimit = video[index].fps;
	}
#if 0	
		/* PTZ */
		trt__GetProfileResponse->Profile->PTZConfiguration= soap_malloc_v2(soap,sizeof(struct tt__PTZConfiguration));
		
		trt__GetProfileResponse->Profile->PTZConfiguration->Name = (char *)soap_malloc(soap,32);
		sprintf(trt__GetProfileResponse->Profile->PTZConfiguration->Name,"Anv_ptz_%d",chn);
		
		trt__GetProfileResponse->Profile->PTZConfiguration->token = (char *)soap_malloc(soap,32);
		sprintf(trt__GetProfileResponse->Profile->PTZConfiguration->token,"Anv_ptz_%d",chn);
		
		trt__GetProfileResponse->Profile->PTZConfiguration->UseCount = MAX_STREAM_NUM;
		
		trt__GetProfileResponse->Profile->PTZConfiguration->NodeToken = (char *)soap_malloc(soap,32);
		sprintf(trt__GetProfileResponse->Profile->PTZConfiguration->NodeToken,"Anv_ptz_%d",chn);
	
		trt__GetProfileResponse->Profile->PTZConfiguration->DefaultContinuousPanTiltVelocitySpace = soap_strdup(soap, 
			"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
		
		trt__GetProfileResponse->Profile->PTZConfiguration->DefaultContinuousZoomVelocitySpace = soap_strdup(soap,
			"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");
	
		trt__GetProfileResponse->Profile->PTZConfiguration->DefaultPTZTimeout = soap_strdup(soap,"PT10S");	
		
		trt__GetProfileResponse->Profile->fixed= (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
		*(trt__GetProfileResponse->Profile->fixed) = xsd__boolean__true_;
		
#if 0
		trt__GetProfileResponse->Profile->Extension =  (struct tt__ProfileExtension *)soap_malloc_v2(soap, sizeof(struct tt__ProfileExtension));
		memset(trt__GetProfileResponse->Profile->Extension,0,sizeof(struct tt__ProfileExtension));
		trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration=	(struct tt__AudioOutputConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioOutputConfiguration));
		memset(trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration,0,sizeof(struct tt__AudioOutputConfiguration));
		trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->token =	(char *)soap_malloc(soap, 32);
		strcpy(trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->token,
				"AudioOutputConfigToken");
		trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->UseCount = 1;
		trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->Name =  (char *)soap_malloc(soap, 32);
		strcpy(trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->Name,
				"AudioOutputConfigName");
		trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->Name =  (char *)soap_malloc(soap, 32);
		strcpy(trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->Name,
				"AudioOutputToken");
		trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->SendPrimacy =  (char *)soap_malloc(soap, 128);
		strcpy(trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->SendPrimacy,
				"www.onvif.org/ver20/HalfDuplex/Client");
		trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->OutputLevel = 10;
		
		trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration=  (struct tt__AudioDecoderConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioDecoderConfiguration));
		memset(trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration,0,sizeof(struct tt__AudioDecoderConfiguration));
		trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration->token =  (char *)soap_malloc(soap, 32);
		strcpy(trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration->token,
				"AudioDecoderConfigToken");
		trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration->Name =	(char *)soap_malloc(soap, 32);
		strcpy(trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration->Name,
				"AudioDecoderConfig");
		trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration->UseCount = 1;
#endif
	
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration = (struct tt__VideoAnalyticsConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__VideoAnalyticsConfiguration));
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->Name = (char *)soap_malloc(soap, LEN_32_SIZE);
		sprintf(trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->Name, "VideoAnalyticsToken%d",chn);
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->token = (char *)soap_malloc(soap, LEN_32_SIZE);
		sprintf(trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->token, "VideoAnalyticsToken%d",chn);
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->UseCount = MAX_STREAM_NUM;
	
		QMAPI_NET_CHANNEL_MOTION_DETECT motion_config = {0};
		motion_config.dwChannel = chn;
		motion_config.dwSize = sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT);
		if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, chn, &motion_config, sizeof(motion_config)))
		{
			return SOAP_ERR;
		}
	
		int level;
		if (motion_config.bEnable == 0)
		{
			level = 0;
		}
		else
		{
			level= 100 - motion_config.dwSensitive;
		}
	
	
		unsigned char region[50];
		pack_motion_cells(motion_config.byMotionArea, region);
	
		printf("******************************************\n");
	
		int i;
		for ( i = 0; i < 50; i++)
		{
			region[i] = ReverseByte(region[i]);
			printf("%X ", region[i]);
		}
		printf("******************************************\n");
	
		unsigned char packRegion[50] = {0};
		unsigned int packLen = packbits(region, packRegion, 50);
		char *outputBuffer = (char *)soap_malloc(soap, sizeof(char) * 100); 
		soap_s2base64(soap, packRegion, outputBuffer, packLen);
	
	printf("base64 encode string is %s\n", outputBuffer);
		
		//AnalyticsEngineConfiguration
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration = (struct tt__AnalyticsEngineConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AnalyticsEngineConfiguration));
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->__sizeAnalyticsModule = 1;
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule = (struct tt__Config *)soap_malloc_v2(soap, sizeof(struct tt__Config));
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Name = soap_strdup(soap, "MyCellMotionModule");
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Type = soap_strdup(soap, "tt:CellMotionEngine");
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters = (struct tt__ItemList *)soap_malloc_v2(soap, sizeof(struct tt__ItemList));
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__sizeSimpleItem = 1;
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc_v2(soap, sizeof(struct _tt__ItemList_SimpleItem));
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Name =  soap_strdup(soap, "Sensitivity");
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Value = (char *)soap_malloc(soap, LEN_24_SIZE);
		sprintf(trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Value, "%d", level);
	
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__sizeElementItem = 1;
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem = (struct _tt__ItemList_ElementItem *)soap_malloc_v2(soap, sizeof(struct _tt__ItemList_ElementItem));
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem->Name = soap_strdup(soap, "Layout");
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem->__any = (char *)soap_malloc(soap, LEN_512_SIZE);
		sprintf(trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem->__any, "<tt:CellLayout Columns=\"22\" Rows=\"18\"><tt:Transformation><tt:Translate x=\"-1.000000\" y=\"-1.000000\"/>"
			"<tt:Scale x=\"0.001953\" y=\"0.002604\"/></tt:Transformation></tt:CellLayout>");
		
		//RuleEngineConfiguration
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration = (struct tt__RuleEngineConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__RuleEngineConfiguration));
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->__sizeRule = 1;
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule = (struct tt__Config *)soap_malloc_v2(soap, sizeof(struct tt__Config));
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Name = soap_strdup(soap, "MyMotionDetectorRule");
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Type = soap_strdup(soap, "tt:CellMotionDetector");
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters = (struct tt__ItemList *)soap_malloc_v2(soap, sizeof(struct tt__ItemList));
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->__sizeSimpleItem = 4;
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc_v2(soap, sizeof(struct _tt__ItemList_SimpleItem)*4);
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[0].Name = soap_strdup(soap, "MinCount");
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[0].Value = soap_strdup(soap, "2");
	
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[1].Name = soap_strdup(soap, "AlarmOnDelay");
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[1].Value = soap_strdup(soap, "1000");
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[2].Name = soap_strdup(soap, "AlarmOffDelay");
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[2].Value = soap_strdup(soap, "1000");
		
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[3].Name = soap_strdup(soap, "ActiveCells");
		trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[3].Value = outputBuffer;	
#endif

	return SOAP_OK;
}

int __trt__GetProfile(struct soap *soap, struct _trt__GetProfile *trt__GetProfile, struct _trt__GetProfileResponse *trt__GetProfileResponse)
{

	ONVIF_DBP( "  __trt__GetProfile \n");

	int ret;
	VIDEO_CONF video[MAX_STREAM_NUM];
	memset(video,0,sizeof(VIDEO_CONF)*MAX_STREAM_NUM);
	
//	int profilefd;

	if(NULL == trt__GetProfile->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Receiver",
			"ter:Action",
			"ter:ProfileToken",
			"The ProfileToken is NULL"));
	}

	int stream_index,chn;
	//FILE *fp=NULL;
	//char filename[LEN_64_SIZE]={0};
	if(!strncmp(trt__GetProfile->ProfileToken, "profile", strlen("profile")))
	{
		ret = sscanf(trt__GetProfile->ProfileToken, "profile%d_%d",&chn, &stream_index);
		if(ret != 2)
			return SOAP_ERR;
	}
	else if(!strncmp(trt__GetProfile->ProfileToken, "axxon", strlen("axxon")))
	{
		stream_index = 0;
		chn = 0;
	}
	else if (!strcmp(trt__GetProfile->ProfileToken, soap->pConfig->profile_cfg.token))
	{
		printf("token is %s %s\n", trt__GetProfile->ProfileToken, soap->pConfig->profile_cfg.token);
		if (!soap->pConfig->profile_cfg.isVaild || GetTempProfile(soap, trt__GetProfileResponse))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Receiver",
				"ter:Action",
				"ter:ProfileToken",
				"The ProfileToken is not existing"));
		}
		else
		{
			return SOAP_OK;
		}
	}
	else 
	{
		stream_index = 0;
		chn = 0;
	}

	if(stream_index >= MAX_STREAM_NUM || chn >= soap->pConfig->g_channel_num)
	{
		return SOAP_ERR;
	}

	if(get_channel_param(chn,video) != 0)
	{
		return SOAP_ERR;
	}

	trt__GetProfileResponse->Profile = (struct tt__Profile*)soap_malloc_v2(soap, sizeof(struct tt__Profile));
	
	trt__GetProfileResponse->Profile->Name = soap_strdup(soap, trt__GetProfile->ProfileToken);
	trt__GetProfileResponse->Profile->token = soap_strdup(soap, trt__GetProfile->ProfileToken);

	trt__GetProfileResponse->Profile->VideoSourceConfiguration = (struct tt__VideoSourceConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__VideoSourceConfiguration));

	trt__GetProfileResponse->Profile->VideoEncoderConfiguration = (struct tt__VideoEncoderConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__VideoEncoderConfiguration));

	/* PTZ */
	trt__GetProfileResponse->Profile->PTZConfiguration= (struct tt__PTZConfiguration *)soap_malloc_v2(soap,sizeof(struct tt__PTZConfiguration));
	
	trt__GetProfileResponse->Profile->PTZConfiguration->Name = (char *)soap_malloc(soap,32);
	sprintf(trt__GetProfileResponse->Profile->PTZConfiguration->Name,"Anv_ptz_%d",chn);
	
	trt__GetProfileResponse->Profile->PTZConfiguration->token = (char *)soap_malloc(soap,32);
	sprintf(trt__GetProfileResponse->Profile->PTZConfiguration->token,"Anv_ptz_%d",chn);
	
	trt__GetProfileResponse->Profile->PTZConfiguration->UseCount = MAX_STREAM_NUM;
	
	trt__GetProfileResponse->Profile->PTZConfiguration->NodeToken = (char *)soap_malloc(soap,32);
	sprintf(trt__GetProfileResponse->Profile->PTZConfiguration->NodeToken,"Anv_ptz_%d",chn);

	trt__GetProfileResponse->Profile->PTZConfiguration->DefaultContinuousPanTiltVelocitySpace = soap_strdup(soap, 
		"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
	
	trt__GetProfileResponse->Profile->PTZConfiguration->DefaultContinuousZoomVelocitySpace = soap_strdup(soap,
		"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");

	trt__GetProfileResponse->Profile->PTZConfiguration->DefaultPTZTimeout = soap_strdup(soap,"PT1S");

	/* VS */
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Name = (char *)soap_malloc(soap, 32);
	sprintf(trt__GetProfileResponse->Profile->VideoSourceConfiguration->Name,"VideoSourceConfiguration%d",chn);
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->UseCount = MAX_STREAM_NUM;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->token = (char *)soap_malloc(soap, 32);
	sprintf(trt__GetProfileResponse->Profile->VideoSourceConfiguration->token,"VideoSourceConfiguration%d",chn);
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken = (char *)soap_malloc(soap, 32);
	sprintf(trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken,"VideoSource%d",chn);
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds = (struct tt__IntRectangle *)soap_malloc_v2(soap, sizeof(struct tt__IntRectangle));
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->x = 0;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->y =  0;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->width = 1920;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->height = 1080;

	/* VE */
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Name = (char *)soap_malloc(soap, 32);
	sprintf(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Name,"VideoEncoderConfiguration%d_%d",chn,stream_index);
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->UseCount = 1;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token = (char *)soap_malloc(soap, 32);
	sprintf(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token,"VideoEncoderConfiguration%d_%d",chn,stream_index);

	if(QMAPI_PT_JPEG == video[stream_index].entype
			|| QMAPI_PT_MJPEG == video[stream_index].entype)
	{
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Encoding =  tt__VideoEncoding__JPEG;
	}
	else if(QMAPI_PT_H264 == video[stream_index].entype 
		|| QMAPI_PT_H264_HIGHPROFILE == video[stream_index].entype
		|| QMAPI_PT_H264_BASELINE == video[stream_index].entype
		|| QMAPI_PT_H264_MAINPROFILE == video[stream_index].entype
		|| QMAPI_PT_H265 == video[stream_index].entype)
	{
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Encoding =  tt__VideoEncoding__H264;

		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264 =
			soap_malloc_v2(soap,sizeof(struct tt__H264Configuration));
		trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->GovLength = video[stream_index].gop;

		switch(video[stream_index].entype)
		{
			case QMAPI_PT_H264_MAINPROFILE:
			case QMAPI_PT_H264:
				trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Main;
				break;
			case QMAPI_PT_H264_BASELINE:
				trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Baseline;
				break;
			default:
				trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__High;
				break;
		}
	}

	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution = (struct tt__VideoResolution *)soap_malloc_v2(soap, sizeof(struct tt__VideoResolution));
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Width = video[stream_index].width;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Height = video[stream_index].height;
	
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Quality = 5 - video[stream_index].piclevel;

#if 1
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));

	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));

	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
	*trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Address->IPv4Address = soap_strdup(soap, RTP_MULTICAST_IP);
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->Port = 0;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast->TTL = 1;
#endif	
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->SessionTimeout = soap_strdup(soap, "PT30M");
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl = (struct tt__VideoRateControl *)soap_malloc_v2(soap, sizeof(struct tt__VideoRateControl));
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->BitrateLimit = video[stream_index].bitrate;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->EncodingInterval = 1;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->FrameRateLimit = video[stream_index].fps;

	/* AS */
	trt__GetProfileResponse->Profile->AudioSourceConfiguration =  (struct tt__AudioSourceConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioSourceConfiguration));

	trt__GetProfileResponse->Profile->AudioEncoderConfiguration =  (struct tt__AudioEncoderConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioEncoderConfiguration));
	trt__GetProfileResponse->Profile->AudioSourceConfiguration->Name =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetProfileResponse->Profile->AudioSourceConfiguration->Name,"AudioSourceConfiguration%d",chn);
	trt__GetProfileResponse->Profile->AudioSourceConfiguration->UseCount=MAX_STREAM_NUM;
	trt__GetProfileResponse->Profile->AudioSourceConfiguration->token =  (char *)soap_malloc(soap, 16);
	sprintf(trt__GetProfileResponse->Profile->AudioSourceConfiguration->token,"AudioSourceConfiguration%d",chn);
	trt__GetProfileResponse->Profile->AudioSourceConfiguration->SourceToken =  (char *)soap_malloc(soap, 16);
	sprintf(trt__GetProfileResponse->Profile->AudioSourceConfiguration->SourceToken,"AudioSource%d",chn);

	/* AE */
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Name =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Name,"AudioEncoderConfiguration%d",chn);
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->UseCount = MAX_STREAM_NUM;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->token =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetProfileResponse->Profile->AudioEncoderConfiguration->token,"AudioEncoderConfiguration%d",chn);

	QMAPI_NET_CHANNEL_PIC_INFO info;
	memset(&info, 0, sizeof(info));
	info.dwSize = sizeof(info);
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, chn, &info, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Encoding = info.stRecordPara.wFormatTag == QMAPI_PT_G726 ? tt__AudioEncoding__G726 : tt__AudioEncoding__G711;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Bitrate=8;

	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->SampleRate=16;
#if 1
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
	*trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Address->IPv4Address = soap_strdup(soap, RTP_MULTICAST_IP);
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->Port = 0;
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->Multicast->TTL = 1;
#endif	
	trt__GetProfileResponse->Profile->AudioEncoderConfiguration->SessionTimeout = soap_strdup(soap, "PT30M");

	
	trt__GetProfileResponse->Profile->fixed= (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*(trt__GetProfileResponse->Profile->fixed) = xsd__boolean__true_;
	
#if 0
	trt__GetProfileResponse->Profile->Extension =  (struct tt__ProfileExtension *)soap_malloc_v2(soap, sizeof(struct tt__ProfileExtension));
	memset(trt__GetProfileResponse->Profile->Extension,0,sizeof(struct tt__ProfileExtension));
	trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration=  (struct tt__AudioOutputConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioOutputConfiguration));
	memset(trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration,0,sizeof(struct tt__AudioOutputConfiguration));
	trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->token =  (char *)soap_malloc(soap, 32);
	strcpy(trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->token,
			"AudioOutputConfigToken");
	trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->UseCount = 1;
	trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->Name =  (char *)soap_malloc(soap, 32);
	strcpy(trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->Name,
			"AudioOutputConfigName");
	trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->Name =  (char *)soap_malloc(soap, 32);
	strcpy(trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->Name,
			"AudioOutputToken");
	trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->SendPrimacy =  (char *)soap_malloc(soap, 128);
	strcpy(trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->SendPrimacy,
			"www.onvif.org/ver20/HalfDuplex/Client");
	trt__GetProfileResponse->Profile->Extension->AudioOutputConfiguration->OutputLevel = 10;
	
	trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration=  (struct tt__AudioDecoderConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioDecoderConfiguration));
	memset(trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration,0,sizeof(struct tt__AudioDecoderConfiguration));
	trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration->token =  (char *)soap_malloc(soap, 32);
	strcpy(trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration->token,
			"AudioDecoderConfigToken");
	trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration->Name =  (char *)soap_malloc(soap, 32);
	strcpy(trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration->Name,
			"AudioDecoderConfig");
	trt__GetProfileResponse->Profile->Extension->AudioDecoderConfiguration->UseCount = 1;
#endif

	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration = (struct tt__VideoAnalyticsConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__VideoAnalyticsConfiguration));
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->Name = (char *)soap_malloc(soap, LEN_32_SIZE);
	sprintf(trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->Name, "VideoAnalyticsToken%d",chn);
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->token = (char *)soap_malloc(soap, LEN_32_SIZE);
	sprintf(trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->token, "VideoAnalyticsToken%d",chn);
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->UseCount = MAX_STREAM_NUM;

	QMAPI_NET_CHANNEL_MOTION_DETECT motion_config = {0};
	motion_config.dwChannel = chn;
	motion_config.dwSize = sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT);
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, chn, &motion_config, sizeof(motion_config)))
	{
		return SOAP_ERR;
	}

	int level;
	if (motion_config.bEnable == 0)
	{
		level = 0;
	}
	else
	{
		level= 100 - motion_config.dwSensitive;
	}


	unsigned char region[50];
	pack_motion_cells(motion_config.byMotionArea, region);

	int i;
	for ( i = 0; i < 50; i++)
	{
		region[i] = ReverseByte(region[i]);
	}

	unsigned char packRegion[50] = {0};
	unsigned int packLen = packbits(region, packRegion, 50);
	char *outputBuffer = (char *)soap_malloc(soap, sizeof(char) * 100);	
	soap_s2base64(soap, packRegion, outputBuffer, packLen);

	
	//AnalyticsEngineConfiguration
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration = (struct tt__AnalyticsEngineConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AnalyticsEngineConfiguration));
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->__sizeAnalyticsModule = 1;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule = (struct tt__Config *)soap_malloc_v2(soap, sizeof(struct tt__Config));
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Name = soap_strdup(soap, "MyCellMotionModule");
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Type = soap_strdup(soap, "tt:CellMotionEngine");
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters = (struct tt__ItemList *)soap_malloc_v2(soap, sizeof(struct tt__ItemList));
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__sizeSimpleItem = 1;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc_v2(soap, sizeof(struct _tt__ItemList_SimpleItem));
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Name =  soap_strdup(soap, "Sensitivity");
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Value = (char *)soap_malloc(soap, LEN_24_SIZE);
	sprintf(trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Value, "%d", level);

	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__sizeElementItem = 1;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem = (struct _tt__ItemList_ElementItem *)soap_malloc_v2(soap, sizeof(struct _tt__ItemList_ElementItem));
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem->Name = soap_strdup(soap, "Layout");
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem->__any = (char *)soap_malloc(soap, LEN_512_SIZE);
	sprintf(trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem->__any, "<tt:CellLayout Columns=\"22\" Rows=\"18\"><tt:Transformation><tt:Translate x=\"-1.000000\" y=\"-1.000000\"/>"
		"<tt:Scale x=\"0.001953\" y=\"0.002604\"/></tt:Transformation></tt:CellLayout>");
	
	//RuleEngineConfiguration
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration = (struct tt__RuleEngineConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__RuleEngineConfiguration));
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->__sizeRule = 1;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule = (struct tt__Config *)soap_malloc_v2(soap, sizeof(struct tt__Config));
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Name = soap_strdup(soap, "MyMotionDetectorRule");
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Type = soap_strdup(soap, "tt:CellMotionDetector");
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters = (struct tt__ItemList *)soap_malloc_v2(soap, sizeof(struct tt__ItemList));
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->__sizeSimpleItem = 4;
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc_v2(soap, sizeof(struct _tt__ItemList_SimpleItem)*4);
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[0].Name = soap_strdup(soap, "MinCount");
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[0].Value = soap_strdup(soap, "2");

	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[1].Name = soap_strdup(soap, "AlarmOnDelay");
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[1].Value = soap_strdup(soap, "1000");
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[2].Name = soap_strdup(soap, "AlarmOffDelay");
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[2].Value = soap_strdup(soap, "1000");
	
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[3].Name = soap_strdup(soap, "ActiveCells");
	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[3].Value = outputBuffer;	

	if (soap->pConfig->profile_cfg.isVaild)
	{
		if (soap->pConfig->profile_cfg.videoSoureToken[0] && !strcmp(soap->pConfig->profile_cfg.videoSoureToken, trt__GetProfileResponse->Profile->VideoSourceConfiguration->token))
		{
			++trt__GetProfileResponse->Profile->VideoSourceConfiguration->UseCount;
		}
	
		if (soap->pConfig->profile_cfg.videoEncoderToken[0] && !strcmp(soap->pConfig->profile_cfg.videoEncoderToken, trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token))
		{
			++trt__GetProfileResponse->Profile->VideoEncoderConfiguration->UseCount;
		}

		if (soap->pConfig->profile_cfg.audioEncoderToken[0] && !strcmp(soap->pConfig->profile_cfg.audioEncoderToken, trt__GetProfileResponse->Profile->AudioEncoderConfiguration->token))
		{
			++trt__GetProfileResponse->Profile->AudioEncoderConfiguration->UseCount;
		}

		if (soap->pConfig->profile_cfg.audioSoureToken[0] && !strcmp(soap->pConfig->profile_cfg.audioSoureToken, trt__GetProfileResponse->Profile->AudioSourceConfiguration->token))
		{
			++trt__GetProfileResponse->Profile->AudioSourceConfiguration->UseCount;
		}
	}
	return SOAP_OK;
	
}

int __trt__GetProfiles(struct soap *soap, struct _trt__GetProfiles *trt__GetProfiles, struct _trt__GetProfilesResponse *trt__GetProfilesResponse)
{
	ONVIF_DBP("");
	
	int i = 0;
	int chn, stream_index;

	if (soap->pConfig->profile_cfg.isVaild)
	{
		/* profile个数 */
		trt__GetProfilesResponse->__sizeProfiles =	soap->pConfig->g_channel_num*MAX_STREAM_NUM + 1;
		trt__GetProfilesResponse->Profiles = (struct tt__Profile*)soap_malloc_v2(soap, sizeof(struct tt__Profile)*(soap->pConfig->g_channel_num*MAX_STREAM_NUM + 1));
	}
	else
	{
		/* profile个数 */
		trt__GetProfilesResponse->__sizeProfiles =  soap->pConfig->g_channel_num*MAX_STREAM_NUM;
		trt__GetProfilesResponse->Profiles = (struct tt__Profile*)soap_malloc_v2(soap, sizeof(struct tt__Profile)*soap->pConfig->g_channel_num*MAX_STREAM_NUM);
	}
	
	VIDEO_CONF video[MAX_STREAM_NUM];
	QMAPI_NET_CHANNEL_PIC_INFO info;

	//每个通道上报2种码流
	for(i=0; i<soap->pConfig->g_channel_num*MAX_STREAM_NUM; i++)
	{
		chn = i/MAX_STREAM_NUM;
		stream_index = i%MAX_STREAM_NUM;
		memset(video,0,sizeof(VIDEO_CONF)*MAX_STREAM_NUM);
		if( 0 != get_channel_param(chn,video))
		{
			return SOAP_ERR;
		}

		trt__GetProfilesResponse->Profiles[i].Name = (char *)soap_malloc(soap, 12);
		sprintf(trt__GetProfilesResponse->Profiles[i].Name,"profile%d_%d",chn,stream_index);
		
		trt__GetProfilesResponse->Profiles[i].token =(char *)soap_malloc(soap, 12);
		sprintf(trt__GetProfilesResponse->Profiles[i].token,"profile%d_%d",chn,stream_index);

		trt__GetProfilesResponse->Profiles[i].fixed= (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
		*( trt__GetProfilesResponse->Profiles[i].fixed) = xsd__boolean__true_;

		/* VS */
		if (!stream_index)
		{
			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration = (struct tt__VideoSourceConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__VideoSourceConfiguration));

			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Name = (char *)soap_malloc(soap, 32);
			sprintf(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Name,"VideoSourceConfiguration%d",chn);
			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->UseCount = MAX_STREAM_NUM;
			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->token = (char *)soap_malloc(soap, 32);
			sprintf(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->token,"VideoSourceConfiguration%d",chn);
			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->SourceToken = (char *)soap_malloc(soap, 32);
			sprintf(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->SourceToken,"VideoSource%d",chn);
			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds = (struct tt__IntRectangle *)soap_malloc_v2(soap, sizeof(struct tt__IntRectangle));
			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->width = 1920;	//VideoSourceConfiguration需要填最大码流的信息
			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->height = 1080;
		}
		else
		{
			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration = trt__GetProfilesResponse->Profiles[i - 1].VideoSourceConfiguration;
		}
		
		/* VE */
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration = (struct tt__VideoEncoderConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__VideoEncoderConfiguration));

		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Name = (char *)soap_malloc(soap, 32);
		sprintf(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Name,"VideoEncoderConfiguration%d_%d",chn,stream_index);
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->UseCount = 1;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->token = (char *)soap_malloc(soap, 32);
		sprintf(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->token,"VideoEncoderConfiguration%d_%d",chn,stream_index);

		if(QMAPI_PT_JPEG == video[stream_index].entype
			|| QMAPI_PT_MJPEG == video[stream_index].entype)
		{
			trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Encoding =  tt__VideoEncoding__JPEG;
		}
		else if(QMAPI_PT_H264 == video[stream_index].entype 
			|| QMAPI_PT_H264_HIGHPROFILE == video[stream_index].entype
			|| QMAPI_PT_H264_BASELINE == video[stream_index].entype
			|| QMAPI_PT_H264_MAINPROFILE == video[stream_index].entype
			|| QMAPI_PT_H265 == video[stream_index].entype)
		{
			trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Encoding =  tt__VideoEncoding__H264;
			trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264 =
				soap_malloc_v2(soap,sizeof(struct tt__H264Configuration));
			trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264->GovLength = video[stream_index].gop;
			switch(video[stream_index].entype)
			{
				case QMAPI_PT_H264_MAINPROFILE:
				case QMAPI_PT_H264:		
					trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Main;
					break;
				case QMAPI_PT_H264_BASELINE:
					trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__Baseline;
					break;
				default:
					trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__High;
					break;
			}
		}

		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution = (struct tt__VideoResolution *)soap_malloc_v2(soap, sizeof(struct tt__VideoResolution));
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Width = video[stream_index].width;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Height = video[stream_index].height;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Quality = 5 - video[stream_index].piclevel;

		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl = (struct tt__VideoRateControl *)soap_malloc_v2(soap, sizeof(struct tt__VideoRateControl));
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->BitrateLimit = video[stream_index].bitrate;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->EncodingInterval = 1;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->FrameRateLimit = video[stream_index].fps;
#if 1
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
		*trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, 32);
		strcpy(*trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv4Address, RTP_MULTICAST_IP);
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Port = 0;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->TTL = 1;
#endif		
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->SessionTimeout = soap_strdup(soap, "PT30M");

		if (!stream_index)
		{
			trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration =  (struct tt__AudioSourceConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioSourceConfiguration));

			trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->Name =  (char *)soap_malloc(soap, 32);
			sprintf(trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->Name,"AudioSourceConfiguration%d",chn);
			trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->UseCount=MAX_STREAM_NUM;
			trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->token =  (char *)soap_malloc(soap, 32);
			sprintf(trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->token,"AudioSourceConfiguration%d",chn);
			trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->SourceToken =  (char *)soap_malloc(soap, 16);
			sprintf(trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->SourceToken,"AudioSource%d",chn);
		}
		else
		{
			trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration = trt__GetProfilesResponse->Profiles[i - 1].AudioSourceConfiguration;
		}
		
		/* AE */
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration =  (struct tt__AudioEncoderConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioEncoderConfiguration));
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Name =  (char *)soap_malloc(soap, 32);
		sprintf(trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Name,"AudioEncoderConfiguration%d",chn);
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->UseCount=MAX_STREAM_NUM;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->token =  (char *)soap_malloc(soap, 32);
		sprintf(trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->token,"AudioEncoderConfiguration%d",chn);

		memset(&info, 0, sizeof(info));
		info.dwSize = sizeof(info);
		QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, chn, &info, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Encoding = info.stRecordPara.wFormatTag == QMAPI_PT_G726 ? tt__AudioEncoding__G726: tt__AudioEncoding__G711;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Bitrate=8;

		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->SampleRate=16;
#if 1
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
		*trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->Address->IPv4Address = soap_strdup(soap, RTP_MULTICAST_IP);
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->Port = 0;
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->Multicast->TTL = 1;
#endif		
		trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->SessionTimeout = soap_strdup(soap, "PT30M");

		QMAPI_NET_CHANNEL_MOTION_DETECT motion_config = {0};
		motion_config.dwChannel = chn;
		motion_config.dwSize = sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT);
		if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, chn, &motion_config, sizeof(motion_config)))
		{
			return SOAP_ERR;
		}

		int level;
		if (motion_config.bEnable == 0)
		{
			level = 0;
		}
		else
		{
			level = 100 - motion_config.dwSensitive;
		}

		unsigned char region[50];
		pack_motion_cells(motion_config.byMotionArea, region);
		
		int n;
		for (n = 0; n < 50; n++)
		{
			if (region[n] && (region[n] != 0xFF))
			{
				region[n] = ReverseByte(region[n]);
			}
		}

		unsigned char packRegion[50] = {0};		
		unsigned int packLen = packbits(region, packRegion, 50);
		char *outputBuffer = (char *)soap_malloc(soap, sizeof(char) * 100);
		soap_s2base64(soap, packRegion, outputBuffer, packLen);

		/* VIDEO ANALYTIC */ //motion
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration = (struct tt__VideoAnalyticsConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__VideoAnalyticsConfiguration));
		
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->Name = (char *)soap_malloc(soap, LEN_32_SIZE);
		sprintf(trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->Name, "VideoAnalyticsToken%d",chn);

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->token = (char *)soap_malloc(soap, LEN_32_SIZE);
		sprintf(trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->token, "VideoAnalyticsToken%d",chn);

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->UseCount = MAX_STREAM_NUM;

		//AnalyticsEngineConfiguration
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration = (struct tt__AnalyticsEngineConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AnalyticsEngineConfiguration));

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->__sizeAnalyticsModule = 1;
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule = (struct tt__Config *)soap_malloc_v2(soap, sizeof(struct tt__Config));

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Name = soap_strdup(soap, "MyCellMotionModule");
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Type = soap_strdup(soap, "tt:CellMotionEngine");

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters = (struct tt__ItemList *)soap_malloc_v2(soap, sizeof(struct tt__ItemList));

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__sizeSimpleItem = 1;
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc_v2(soap, sizeof(struct _tt__ItemList_SimpleItem));
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Name= soap_strdup(soap, "Sensitivity");

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Value = (char *)soap_malloc(soap, LEN_32_SIZE);
		sprintf(trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->SimpleItem->Value, "%d", level);

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->__sizeElementItem = 1;
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem = (struct _tt__ItemList_ElementItem *)soap_malloc_v2(soap, sizeof(struct _tt__ItemList_ElementItem));
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem->Name = soap_strdup(soap, "Layout");
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->AnalyticsEngineConfiguration->AnalyticsModule->Parameters->ElementItem->__any = soap_strdup(soap, "<tt:CellLayout Columns=\"22\" Rows=\"18\"><tt:Transformation><tt:Translate x=\"-1.000000\" y=\"-1.000000\"/>"
			"<tt:Scale x=\"0.001953\" y=\"0.002604\"/></tt:Transformation></tt:CellLayout>");

		//RuleEngineConfiguration
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration = (struct tt__RuleEngineConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__RuleEngineConfiguration));

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->__sizeRule = 1;
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule = (struct tt__Config *)soap_malloc_v2(soap, sizeof(struct tt__Config));

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Name = soap_strdup(soap, "MyMotionDetectorRule");
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Type = soap_strdup(soap, "tt:CellMotionDetector");

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters = (struct tt__ItemList *)soap_malloc_v2(soap, sizeof(struct tt__ItemList));

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->__sizeSimpleItem = 4;
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc_v2(soap, sizeof(struct _tt__ItemList_SimpleItem)*4);

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[0].Name = soap_strdup(soap, "MinCount");
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[0].Value = soap_strdup(soap, "2");

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[1].Name = soap_strdup(soap, "AlarmOnDelay");
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[1].Value = soap_strdup(soap, "1000");

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[2].Name = soap_strdup(soap, "AlarmOffDelay");
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[2].Value = soap_strdup(soap, "1000");

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[3].Name = soap_strdup(soap, "ActiveCells");
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->SimpleItem[3].Value = outputBuffer;
#if 0
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule++;
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Name = (char *)soap_malloc(soap, LEN_32_SIZE);
		strcpy(trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Name, "tt:TamperDetector");
		ONVIF_DBP("");

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Type = (char *)soap_malloc(soap, LEN_32_SIZE);
		strcpy(trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Type, "tt:TamperDetector");
		ONVIF_DBP("");

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->__sizeElementItem = 1;
		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->ElementItem = (struct _tt__ItemList_ElementItem *)soap_malloc_v2(soap, sizeof(struct _tt__ItemList_ElementItem));
		ONVIF_DBP("");

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->ElementItem->Name = (char *)soap_malloc(soap, LEN_32_SIZE);
		strcpy(trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->ElementItem->Name, "Field");
		ONVIF_DBP("");

		trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->ElementItem->__any = (char *)soap_malloc(soap, LEN_256_SIZE);
		strcpy(trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration->RuleEngineConfiguration->Rule->Parameters->ElementItem->__any, "<ns1:PolygonConfiguration><ns1:Polygon><Point x=\"0\" y=\"0\"/><Point x=\"0\" y=\"0\"/><Point x=\"0\" y=\"0\"/><Point x=\"0\" y=\"0\"/></ns1:Polygon></ns1:PolygonConfiguration>");
		ONVIF_DBP("");
#endif

/////////////////////////////////////////////////////
		/* PTZ */
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration= soap_malloc_v2(soap,sizeof(struct tt__PTZConfiguration));
		
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->Name = (char *)soap_malloc(soap,32);
		sprintf(trt__GetProfilesResponse->Profiles[i].PTZConfiguration->Name,"Anv_ptz_%d",chn);
		
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->token = (char *)soap_malloc(soap,32);
		sprintf(trt__GetProfilesResponse->Profiles[i].PTZConfiguration->token,"Anv_ptz_%d",chn);
		
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->UseCount = MAX_STREAM_NUM;
		
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->NodeToken = (char *)soap_malloc(soap,32);
		sprintf(trt__GetProfilesResponse->Profiles[i].PTZConfiguration->NodeToken,"Anv_ptz_%d",chn);

		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultContinuousPanTiltVelocitySpace = 
			(char *)soap_malloc(soap,128);
		strcpy(trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultContinuousPanTiltVelocitySpace,
			"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace");
		
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultContinuousZoomVelocitySpace = 
			(char *)soap_malloc(soap,128);
		strcpy(trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultContinuousZoomVelocitySpace,
			"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace");
		//
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultPTZTimeout= 
			(char *)soap_malloc(soap, 8);
		strcpy(trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultPTZTimeout,"PT1S");

#if 0
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultPTZSpeed = 
			(struct tt__PTZSpeed *)soap_malloc_v2(soap, sizeof(struct tt__PTZSpeed));
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultPTZSpeed->PanTilt = 
			(struct tt__Vector2D *)soap_malloc_v2(soap, sizeof(struct tt__Vector2D));
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultPTZSpeed->PanTilt->space = 
			(char *)soap_malloc(soap,128);
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultPTZSpeed->Zoom = 
			(struct tt__Vector1D *)soap_malloc_v2(soap, sizeof(struct tt__Vector1D));
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultPTZSpeed->Zoom->space = 
			(char *)soap_malloc(soap,128);
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultPTZSpeed->PanTilt->x = 0.0;
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultPTZSpeed->PanTilt->y = 0.0;
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultPTZSpeed->Zoom->x = 0.0;
		strcpy(trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultPTZSpeed->PanTilt->space,
			"http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace");
		strcpy(trt__GetProfilesResponse->Profiles[i].PTZConfiguration->DefaultPTZSpeed->Zoom->space,
			"http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace");

		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->PanTiltLimits = 
			(struct tt__PanTiltLimits *)soap_malloc_v2(soap, sizeof(struct tt__PanTiltLimits));
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->PanTiltLimits->Range = 
			(struct tt__Space2DDescription *)soap_malloc_v2(soap, sizeof(struct tt__Space2DDescription));
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->PanTiltLimits->Range->URI = 
			(char *)soap_malloc(soap,128);
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->PanTiltLimits->Range->XRange = 
			(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->PanTiltLimits->Range->YRange = 
			(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->ZoomLimits = 
			(struct tt__ZoomLimits *)soap_malloc_v2(soap, sizeof(struct tt__ZoomLimits));
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->ZoomLimits->Range = 
			(struct tt__Space1DDescription *)soap_malloc_v2(soap, sizeof(struct tt__Space1DDescription));
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->ZoomLimits->Range->URI = 
			(char *)soap_malloc(soap,128);
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->ZoomLimits->Range->XRange = 
			(struct tt__FloatRange *)soap_malloc_v2(soap, sizeof(struct tt__FloatRange));

		strcpy(trt__GetProfilesResponse->Profiles[i].PTZConfiguration->PanTiltLimits->Range->URI,
			"http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace");
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->PanTiltLimits->Range->XRange->Min = -1.0;
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->PanTiltLimits->Range->XRange->Max = 1.0;
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->PanTiltLimits->Range->YRange->Min = -1.0;
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->PanTiltLimits->Range->YRange->Max = 1.0;
		strcpy(trt__GetProfilesResponse->Profiles[i].PTZConfiguration->ZoomLimits->Range->URI,
			"http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace");
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->ZoomLimits->Range->XRange->Min = -1.0;
		trt__GetProfilesResponse->Profiles[i].PTZConfiguration->ZoomLimits->Range->XRange->Max = 1.0;
#endif		

#if 0
		trt__GetProfilesResponse->Profiles[i].Extension =  (struct tt__ProfileExtension *)soap_malloc_v2(soap, sizeof(struct tt__ProfileExtension));
		trt__GetProfilesResponse->Profiles[i].Extension->AudioOutputConfiguration=  (struct tt__AudioOutputConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioOutputConfiguration));
		trt__GetProfilesResponse->Profiles[i].Extension->AudioOutputConfiguration->token =  (char *)soap_malloc(soap, 32);
		strcpy(trt__GetProfilesResponse->Profiles[i].Extension->AudioOutputConfiguration->token,
				"AudioOutputConfigToken");
		trt__GetProfilesResponse->Profiles[i].Extension->AudioOutputConfiguration->UseCount = 1;
		trt__GetProfilesResponse->Profiles[i].Extension->AudioOutputConfiguration->Name =  (char *)soap_malloc(soap, 32);
		strcpy(trt__GetProfilesResponse->Profiles[i].Extension->AudioOutputConfiguration->Name,
				"AudioOutputConfigName");
		trt__GetProfilesResponse->Profiles[i].Extension->AudioOutputConfiguration->OutputToken =  (char *)soap_malloc(soap, 32);
		strcpy(trt__GetProfilesResponse->Profiles[i].Extension->AudioOutputConfiguration->OutputToken,
				"AudioOutputToken");
		trt__GetProfilesResponse->Profiles[i].Extension->AudioOutputConfiguration->SendPrimacy =  (char *)soap_malloc(soap, 128);
		strcpy(trt__GetProfilesResponse->Profiles[i].Extension->AudioOutputConfiguration->SendPrimacy,
				"www.onvif.org/ver20/HalfDuplex/Client");
		trt__GetProfilesResponse->Profiles[i].Extension->AudioOutputConfiguration->OutputLevel = 10;
		
		trt__GetProfilesResponse->Profiles[i].Extension->AudioDecoderConfiguration=  (struct tt__AudioDecoderConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioDecoderConfiguration));
		trt__GetProfilesResponse->Profiles[i].Extension->AudioDecoderConfiguration->token =  (char *)soap_malloc(soap, 32);
		strcpy(trt__GetProfilesResponse->Profiles[i].Extension->AudioDecoderConfiguration->token,
				"AudioDecoderConfigToken");
		trt__GetProfilesResponse->Profiles[i].Extension->AudioDecoderConfiguration->Name =  (char *)soap_malloc(soap, 32);
		strcpy(trt__GetProfilesResponse->Profiles[i].Extension->AudioDecoderConfiguration->Name,
				"AudioDecoderConfig");
		trt__GetProfilesResponse->Profiles[i].Extension->AudioDecoderConfiguration->UseCount = 1;
#endif		
	}

	if (soap->pConfig->profile_cfg.isVaild)
	{
		trt__GetProfilesResponse->Profiles[i].fixed = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
		*trt__GetProfilesResponse->Profiles[i].fixed = xsd__boolean__false_;
		
		trt__GetProfilesResponse->Profiles[i].Name = soap->pConfig->profile_cfg.name;
		trt__GetProfilesResponse->Profiles[i].token = soap->pConfig->profile_cfg.token;

		if (soap->pConfig->profile_cfg.audioSoureToken[0])
		{
			sscanf(soap->pConfig->profile_cfg.audioSoureToken, "AudioSourceConfiguration%d", &chn);
			printf("ch = %d\n", chn);
			trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration = trt__GetProfilesResponse->Profiles[chn * MAX_STREAM_NUM].AudioSourceConfiguration;
			++trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration->UseCount;
			++trt__GetProfilesResponse->Profiles[chn * MAX_STREAM_NUM + 1].AudioSourceConfiguration->UseCount;
		}

		if (soap->pConfig->profile_cfg.audioEncoderToken[0])
		{
			sscanf(soap->pConfig->profile_cfg.audioEncoderToken, "AudioEncoderConfiguration%d", &chn);
			printf("AudioEncoderConfiguration ch = %d\n", chn);
			trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration = trt__GetProfilesResponse->Profiles[chn * MAX_STREAM_NUM].AudioEncoderConfiguration;
			++trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration->UseCount;
			++trt__GetProfilesResponse->Profiles[chn * MAX_STREAM_NUM + 1].AudioEncoderConfiguration->UseCount;
		}
		
		if (soap->pConfig->profile_cfg.videoSoureToken[0])
		{
			sscanf(soap->pConfig->profile_cfg.videoSoureToken, "VideoSourceConfiguration%d", &chn);
			printf("VideoSourceConfiguration ch = %d\n", chn);
			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration = trt__GetProfilesResponse->Profiles[chn * MAX_STREAM_NUM].VideoSourceConfiguration;
			++trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->UseCount;
			++trt__GetProfilesResponse->Profiles[chn * MAX_STREAM_NUM + 1].VideoSourceConfiguration->UseCount;
		}
		
		if (soap->pConfig->profile_cfg.videoEncoderToken[0])
		{
			sscanf(soap->pConfig->profile_cfg.videoEncoderToken, "VideoEncoderConfiguration%d_%d", &chn, &stream_index);
			printf("ch = %d index = %d\n", chn, stream_index);
			trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration = trt__GetProfilesResponse->Profiles[chn * MAX_STREAM_NUM + stream_index].VideoEncoderConfiguration;
			++trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->UseCount;
			++trt__GetProfilesResponse->Profiles[chn * MAX_STREAM_NUM + 1].VideoEncoderConfiguration->UseCount;
		}	
	}
	return SOAP_OK;
}


int  __trt__AddVideoEncoderConfiguration(struct soap *soap,
                                         struct _trt__AddVideoEncoderConfiguration *trt__AddVideoEncoderConfiguration,
                                         struct _trt__AddVideoEncoderConfigurationResponse *trt__AddVideoEncoderConfigurationResponse)
{
	printf("token is %s*******************%s\n", trt__AddVideoEncoderConfiguration->ProfileToken, soap->pConfig->profile_cfg.token);

	if (!soap->pConfig->profile_cfg.isVaild || strcmp(trt__AddVideoEncoderConfiguration->ProfileToken, soap->pConfig->profile_cfg.token)
		|| strncmp(trt__AddVideoEncoderConfiguration->ConfigurationToken, "VideoEncoderConfiguration", strlen("VideoEncoderConfiguration")))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}
	
	int chno;
	int stream_index;
	

	int ret = sscanf(trt__AddVideoEncoderConfiguration->ConfigurationToken, "VideoEncoderConfiguration%d_%d", &chno, &stream_index);
	if(ret != 2)
	{
		return SOAP_ERR;
	}
	
	if (stream_index >= MAX_STREAM_NUM || chno >= soap->pConfig->g_channel_num)
	{
		return SOAP_ERR;
	}

	strncpy(soap->pConfig->profile_cfg.videoEncoderToken, trt__AddVideoEncoderConfiguration->ConfigurationToken, 64);
	return SOAP_OK;

}

int  __trt__AddVideoSourceConfiguration(struct soap *soap,
                                        struct _trt__AddVideoSourceConfiguration *trt__AddVideoSourceConfiguration,
                                        struct _trt__AddVideoSourceConfigurationResponse *trt__AddVideoSourceConfigurationResponse)
{
	if (!soap->pConfig->profile_cfg.isVaild || strcmp(trt__AddVideoSourceConfiguration->ProfileToken, soap->pConfig->profile_cfg.token)
		|| strncmp(trt__AddVideoSourceConfiguration->ConfigurationToken, "VideoSourceConfiguration", strlen("VideoSourceConfiguration")))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}
		
	int chno = atoi(trt__AddVideoSourceConfiguration->ConfigurationToken + strlen("VideoSourceConfiguration"));
	
	if (soap->pConfig->g_channel_num < (chno + 1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}

	strncpy(soap->pConfig->profile_cfg.videoSoureToken, trt__AddVideoSourceConfiguration->ConfigurationToken, 64);
	return SOAP_OK;	
}

int  __trt__AddAudioEncoderConfiguration(struct soap *soap,
                                         struct _trt__AddAudioEncoderConfiguration *trt__AddAudioEncoderConfiguration,
                                         struct _trt__AddAudioEncoderConfigurationResponse *trt__AddAudioEncoderConfigurationResponse)
{
	int chno;

	if (!soap->pConfig->profile_cfg.isVaild || strcmp(trt__AddAudioEncoderConfiguration->ProfileToken, soap->pConfig->profile_cfg.token)
		|| strncmp(trt__AddAudioEncoderConfiguration->ConfigurationToken, "AudioEncoderConfiguration", strlen("AudioEncoderConfiguration")))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}
		
	chno = atoi(trt__AddAudioEncoderConfiguration->ConfigurationToken + strlen("AudioEncoderConfiguration"));
	
	if (soap->pConfig->g_channel_num < (chno + 1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}

	strncpy(soap->pConfig->profile_cfg.audioEncoderToken, trt__AddAudioEncoderConfiguration->ConfigurationToken, 64);
	return SOAP_OK;
}

int __trt__AddAudioSourceConfiguration(struct soap *soap, struct _trt__AddAudioSourceConfiguration *trt__AddAudioSourceConfiguration, struct _trt__AddAudioSourceConfigurationResponse *trt__AddAudioSourceConfigurationResponse)
{
	int chno;
	char *p;

	if (!soap->pConfig->profile_cfg.isVaild || strcmp(trt__AddAudioSourceConfiguration->ProfileToken, soap->pConfig->profile_cfg.token)
		|| strncmp(trt__AddAudioSourceConfiguration->ConfigurationToken, "AudioSourceConfiguration", strlen("AudioSourceConfiguration")))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}
		
	p = trt__AddAudioSourceConfiguration->ConfigurationToken + strlen("AudioSourceConfiguration");
	chno = atoi(p);
	
	if (soap->pConfig->g_channel_num < (chno + 1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}

	strncpy(soap->pConfig->profile_cfg.audioSoureToken, trt__AddAudioSourceConfiguration->ConfigurationToken, 64);
	return SOAP_OK;
}

 int  __trt__AddPTZConfiguration(struct soap *soap, 
 	struct _trt__AddPTZConfiguration *trt__AddPTZConfiguration, 
 	struct _trt__AddPTZConfigurationResponse *trt__AddPTZConfigurationResponse)
{ 

	
	ONVIF_DBP("");	 
	 
	if(trt__AddPTZConfiguration->ProfileToken)
	{
		return SOAP_OK;
	}
	return SOAP_OK;
}

int  __trt__AddVideoAnalyticsConfiguration(struct soap *soap, struct _trt__AddVideoAnalyticsConfiguration *trt__AddVideoAnalyticsConfiguration, struct _trt__AddVideoAnalyticsConfigurationResponse *trt__AddVideoAnalyticsConfigurationResponse)
{ 

	ONVIF_DBP("");

	return SOAP_OK; 
}

int  __trt__AddMetadataConfiguration(struct soap *soap,
	struct _trt__AddMetadataConfiguration *trt__AddMetadataConfiguration,
         struct _trt__AddMetadataConfigurationResponse *trt__AddMetadataConfigurationResponse)
{
	ONVIF_DBP("");
	return SOAP_OK;
}

int  __trt__AddAudioOutputConfiguration(struct soap *soap, struct _trt__AddAudioOutputConfiguration *trt__AddAudioOutputConfiguration, struct _trt__AddAudioOutputConfigurationResponse *trt__AddAudioOutputConfigurationResponse)
{ 



	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __trt__AddAudioDecoderConfiguration(struct soap *soap, struct _trt__AddAudioDecoderConfiguration *trt__AddAudioDecoderConfiguration, struct _trt__AddAudioDecoderConfigurationResponse *trt__AddAudioDecoderConfigurationResponse)
{
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __trt__RemoveVideoEncoderConfiguration(struct soap *soap,
	struct _trt__RemoveVideoEncoderConfiguration *trt__RemoveVideoEncoderConfiguration,
         struct _trt__RemoveVideoEncoderConfigurationResponse *trt__RemoveVideoEncoderConfigurationResponse)
{
	if (!strcmp(trt__RemoveVideoEncoderConfiguration->ProfileToken, soap->pConfig->profile_cfg.token))
	{
		soap->pConfig->profile_cfg.videoEncoderToken[0] = '\0';
	}

	return SOAP_OK; 
}

int  __trt__RemoveVideoSourceConfiguration(struct soap *soap,
	struct _trt__RemoveVideoSourceConfiguration *trt__RemoveVideoSourceConfiguration,
         struct _trt__RemoveVideoSourceConfigurationResponse *trt__RemoveVideoSourceConfigurationResponse)
{
	if (!strcmp(trt__RemoveVideoSourceConfiguration->ProfileToken, soap->pConfig->profile_cfg.token))
	{
		soap->pConfig->profile_cfg.videoSoureToken[0] = '\0';
	}

	return SOAP_OK;
}

int  __trt__RemoveAudioEncoderConfiguration(struct soap *soap,
	struct _trt__RemoveAudioEncoderConfiguration *trt__RemoveAudioEncoderConfiguration,
         struct _trt__RemoveAudioEncoderConfigurationResponse *trt__RemoveAudioEncoderConfigurationResponse)
{
	if (!strcmp(trt__RemoveAudioEncoderConfiguration->ProfileToken, soap->pConfig->profile_cfg.token))
	{
		soap->pConfig->profile_cfg.audioEncoderToken[0] = '\0';
	}
	
   	return SOAP_OK;
}

int  __trt__RemoveAudioSourceConfiguration(struct soap *soap,
	struct _trt__RemoveAudioSourceConfiguration *trt__RemoveAudioSourceConfiguration,
         struct _trt__RemoveAudioSourceConfigurationResponse *trt__RemoveAudioSourceConfigurationResponse)
{
	if (!strcmp(trt__RemoveAudioSourceConfiguration->ProfileToken, soap->pConfig->profile_cfg.token))
	{
		soap->pConfig->profile_cfg.audioSoureToken[0] = '\0';
	}
	
    return SOAP_OK;
}

 int  __trt__RemovePTZConfiguration(struct soap *soap, 
 	struct _trt__RemovePTZConfiguration *trt__RemovePTZConfiguration, 
 	struct _trt__RemovePTZConfigurationResponse *trt__RemovePTZConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __trt__RemoveVideoAnalyticsConfiguration(struct soap *soap, struct _trt__RemoveVideoAnalyticsConfiguration *trt__RemoveVideoAnalyticsConfiguration, struct _trt__RemoveVideoAnalyticsConfigurationResponse *trt__RemoveVideoAnalyticsConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __trt__RemoveMetadataConfiguration(struct soap *soap, struct _trt__RemoveMetadataConfiguration *trt__RemoveMetadataConfiguration, struct _trt__RemoveMetadataConfigurationResponse *trt__RemoveMetadataConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __trt__RemoveAudioOutputConfiguration(struct soap *soap, struct _trt__RemoveAudioOutputConfiguration *trt__RemoveAudioOutputConfiguration, struct _trt__RemoveAudioOutputConfigurationResponse *trt__RemoveAudioOutputConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __trt__RemoveAudioDecoderConfiguration(struct soap *soap, struct _trt__RemoveAudioDecoderConfiguration *trt__RemoveAudioDecoderConfiguration, struct _trt__RemoveAudioDecoderConfigurationResponse *trt__RemoveAudioDecoderConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int __trt__DeleteProfile(struct soap *soap, struct _trt__DeleteProfile *trt__DeleteProfile, struct _trt__DeleteProfileResponse *trt__DeleteProfileResponse)
{

	ONVIF_DBP("");

	//char cmd[128] = {0};

	if(strcmp(trt__DeleteProfile->ProfileToken, soap->pConfig->profile_cfg.token))
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Receiver",
			"ter:Action",
			"ter:ProfileToken",
			"The ProfileToken is NULL"));
	/*
	sprintf(cmd,"/bin/rm -f /tmp/%s",trt__DeleteProfile->ProfileToken);
	system(cmd);
	*/

	soap->pConfig->profile_cfg.isVaild = 0;
	return SOAP_OK;

	
}


int __trt__GetVideoSourceConfigurations(struct soap *soap, struct _trt__GetVideoSourceConfigurations *trt__GetVideoSourceConfigurations, struct _trt__GetVideoSourceConfigurationsResponse *trt__GetVideoSourceConfigurationsResponse)
{
	trt__GetVideoSourceConfigurationsResponse->__sizeConfigurations = soap->pConfig->g_channel_num;
	trt__GetVideoSourceConfigurationsResponse->Configurations = soap_malloc_v2(soap,sizeof(struct tt__VideoSourceConfiguration)*soap->pConfig->g_channel_num);

	VIDEO_CONF video[MAX_STREAM_NUM];
	memset(video,0,sizeof(VIDEO_CONF));

	int i = 0;
	for(i=0;i<soap->pConfig->g_channel_num;i++)
	{
		if(get_channel_param(i,video) != 0)
		{
			return SOAP_ERR;
		}

		trt__GetVideoSourceConfigurationsResponse->Configurations[i].Name = (char *)soap_malloc(soap, 32);
		sprintf(trt__GetVideoSourceConfigurationsResponse->Configurations[i].Name,"VideoSourceConfiguration%d",i);
		trt__GetVideoSourceConfigurationsResponse->Configurations[i].UseCount = MAX_STREAM_NUM;
		trt__GetVideoSourceConfigurationsResponse->Configurations[i].token = (char *)soap_malloc(soap, 32);
		sprintf(trt__GetVideoSourceConfigurationsResponse->Configurations[i].token,"VideoSourceConfiguration%d",i);
		trt__GetVideoSourceConfigurationsResponse->Configurations[i].SourceToken = (char *)soap_malloc(soap, 32);
		sprintf(trt__GetVideoSourceConfigurationsResponse->Configurations[i].SourceToken,"VideoSource%d",i);
		trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds = (struct tt__IntRectangle *)soap_malloc_v2(soap, sizeof(struct tt__IntRectangle));
		trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds->x = 0;
		trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds->y =  0;
		trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds->width = 1920;
		trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds->height = 1080;
	}
	return SOAP_OK;
}

int __trt__GetVideoEncoderConfigurations(struct soap *soap, struct _trt__GetVideoEncoderConfigurations *trt__GetVideoEncoderConfigurations, struct _trt__GetVideoEncoderConfigurationsResponse *trt__GetVideoEncoderConfigurationsResponse)
{ 

	ONVIF_DBP("");

	trt__GetVideoEncoderConfigurationsResponse->__sizeConfigurations = soap->pConfig->g_channel_num*MAX_STREAM_NUM;
	trt__GetVideoEncoderConfigurationsResponse->Configurations = soap_malloc_v2(soap,sizeof(struct tt__VideoEncoderConfiguration)*soap->pConfig->g_channel_num*MAX_STREAM_NUM);

	VIDEO_CONF video[MAX_STREAM_NUM];

	int i = 0;
	int chno, stream_index;
	for(i=0;i<soap->pConfig->g_channel_num*MAX_STREAM_NUM;i++)
	{
		chno= i/MAX_STREAM_NUM;
		stream_index = i%MAX_STREAM_NUM;
		memset(video,0,sizeof(VIDEO_CONF)*MAX_STREAM_NUM);
		if(get_channel_param(chno,video) != 0)
		{
			return SOAP_ERR;
		}

		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Name = (char *)soap_malloc(soap, 32);
		sprintf(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Name,"VideoEncoderConfiguration%d_%d",chno, stream_index);
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].UseCount = 1;//panhn  :0
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].token = (char *)soap_malloc(soap, 32);
		sprintf(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].token,"VideoEncoderConfiguration%d_%d",chno, stream_index);
		if(QMAPI_PT_JPEG == video[stream_index].entype
			|| QMAPI_PT_MJPEG == video[stream_index].entype) //26
			trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Encoding =  tt__VideoEncoding__JPEG;
		else if(QMAPI_PT_H264 == video[stream_index].entype 
			|| QMAPI_PT_H264_HIGHPROFILE == video[stream_index].entype
			|| QMAPI_PT_H264_BASELINE == video[stream_index].entype
			|| QMAPI_PT_H264_MAINPROFILE == video[stream_index].entype
			|| QMAPI_PT_H265 == video[stream_index].entype)
		{
			trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Encoding =  tt__VideoEncoding__H264;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264 = (struct tt__H264Configuration *)soap_malloc_v2(soap, sizeof(struct tt__H264Configuration));
			trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264->GovLength = video[stream_index].gop;
			switch(video[stream_index].entype)
			{
				case QMAPI_PT_H264_BASELINE:
					trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264->H264Profile = tt__H264Profile__Baseline;
					break;
				case QMAPI_PT_H264_MAINPROFILE:
				case QMAPI_PT_H264:
					trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264->H264Profile = tt__H264Profile__Main;
					break;
				default:
					trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264->H264Profile = tt__H264Profile__High;
					break;
			}
		}
		else if(99 == video[stream_index].entype)
		{
			trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Encoding =  tt__VideoEncoding__MPEG4;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[i].MPEG4 = (struct tt__Mpeg4Configuration *)soap_malloc_v2(soap, sizeof(struct  tt__Mpeg4Configuration));
			trt__GetVideoEncoderConfigurationsResponse->Configurations[i].MPEG4->GovLength = video[stream_index].gop;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[i].MPEG4->Mpeg4Profile = tt__Mpeg4Profile__SP;
		}
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution = (struct tt__VideoResolution *)soap_malloc_v2(soap, sizeof(struct tt__VideoResolution));
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution->Width = video[stream_index].width;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution->Height = video[stream_index].height;
		//modified by panhn
		//trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Quality = (float)video[i%MAX_STREAM_NUM].piclevel;

		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Quality = 5.0 - video[i%MAX_STREAM_NUM].piclevel;

		//end modify

		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl = (struct tt__VideoRateControl *)soap_malloc_v2(soap, sizeof(struct tt__VideoRateControl));
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl->BitrateLimit = video[stream_index].bitrate;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl->EncodingInterval = 1;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl->FrameRateLimit = video[stream_index].fps;
#if 1
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->Type = tt__IPType__IPv4;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
		*trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address = (char *)soap_malloc(soap, 32);
		strcpy(*trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address, RTP_MULTICAST_IP);
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Port = 0;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->TTL = 1;
#endif
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].SessionTimeout = (char *)soap_malloc(soap, 8);
		strcpy(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].SessionTimeout, "PT30M");
	}
	
	return SOAP_OK; 
}

int __trt__GetAudioSourceConfigurations(struct soap *soap, struct _trt__GetAudioSourceConfigurations *trt__GetAudioSourceConfigurations, struct _trt__GetAudioSourceConfigurationsResponse *trt__GetAudioSourceConfigurationsResponse)
{
	int i;
	trt__GetAudioSourceConfigurationsResponse->__sizeConfigurations = soap->pConfig->g_channel_num;
	trt__GetAudioSourceConfigurationsResponse->Configurations = (struct tt__AudioSourceConfiguration*)soap_malloc_v2(soap, sizeof(struct tt__AudioSourceConfiguration)*soap->pConfig->g_channel_num);

	for(i=0;i<soap->pConfig->g_channel_num;i++)
	{
		trt__GetAudioSourceConfigurationsResponse->Configurations[i].Name =  (char *)soap_malloc(soap, 32);
		sprintf(trt__GetAudioSourceConfigurationsResponse->Configurations[i].Name, "AudioSourceConfiguration%d", i);
		trt__GetAudioSourceConfigurationsResponse->Configurations[i].UseCount=MAX_STREAM_NUM;
		trt__GetAudioSourceConfigurationsResponse->Configurations[i].token =  (char *)soap_malloc(soap, 32);
		sprintf(trt__GetAudioSourceConfigurationsResponse->Configurations[i].token, "AudioSourceConfiguration%d", i);
		trt__GetAudioSourceConfigurationsResponse->Configurations[i].SourceToken =  (char *)soap_malloc(soap, 16);
		sprintf(trt__GetAudioSourceConfigurationsResponse->Configurations[i].SourceToken, "AudioSource%d", i);
	}
	
	return SOAP_OK;
 }

 int __trt__GetAudioEncoderConfigurations(struct soap *soap, struct _trt__GetAudioEncoderConfigurations *trt__GetAudioEncoderConfigurations, struct _trt__GetAudioEncoderConfigurationsResponse *trt__GetAudioEncoderConfigurationsResponse)
 {
 
	 ONVIF_DBP("");

	int i;
	/* Configuration个数 */
	trt__GetAudioEncoderConfigurationsResponse->__sizeConfigurations = soap->pConfig->g_channel_num;
	trt__GetAudioEncoderConfigurationsResponse->Configurations = (struct tt__AudioEncoderConfiguration*)soap_malloc_v2(soap, sizeof(struct tt__AudioEncoderConfiguration)*soap->pConfig->g_channel_num);

	QMAPI_NET_CHANNEL_PIC_INFO info;
	for(i=0;i<soap->pConfig->g_channel_num;i++)
	{
		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].Name =  (char *)soap_malloc(soap, 32);
		sprintf(trt__GetAudioEncoderConfigurationsResponse->Configurations[i].Name,"AudioEncoderConfiguration%d", i);
		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].UseCount=MAX_STREAM_NUM;
		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].token =  (char *)soap_malloc(soap, 32);
		sprintf(trt__GetAudioEncoderConfigurationsResponse->Configurations[i].token,"AudioEncoderConfiguration%d", i);

		memset(&info, 0, sizeof(info));
		info.dwSize = sizeof(info);
		QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, i, &info, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].Encoding = info.stRecordPara.wFormatTag == QMAPI_PT_G726 ? tt__AudioEncoding__G726 : tt__AudioEncoding__G711;
		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].Bitrate = 8;

		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].SampleRate = 16;
#if 1
		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));
		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].Multicast->Address->Type = tt__IPType__IPv4;
		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
		*trt__GetAudioEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address = (char *)soap_malloc(soap, 32);
		strcpy(*trt__GetAudioEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address, RTP_MULTICAST_IP);
		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].Multicast->Port = 0;
		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].Multicast->TTL = 1;
#endif		
		trt__GetAudioEncoderConfigurationsResponse->Configurations[i].SessionTimeout = (char *)soap_malloc(soap, 8);
		sprintf(trt__GetAudioEncoderConfigurationsResponse->Configurations[i].SessionTimeout, "PT30M");
	}
	
	 ONVIF_DBP("======================== Leave \n");
	 
	 return SOAP_OK;
 }


int  __trt__GetVideoAnalyticsConfigurations(struct soap *soap, struct _trt__GetVideoAnalyticsConfigurations *trt__GetVideoAnalyticsConfigurations, struct _trt__GetVideoAnalyticsConfigurationsResponse *trt__GetVideoAnalyticsConfigurationsResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK;
}

 int __trt__GetMetadataConfigurations(struct soap *soap, struct _trt__GetMetadataConfigurations *trt__GetMetadataConfigurations, struct _trt__GetMetadataConfigurationsResponse *trt__GetMetadataConfigurationsResponse)
 {
 
	ONVIF_DBP("");
 #if 0
	 SysInfo *oSysInfo = GetSysInfo();

	 if ( !oSysInfo ) {
		return(send_soap_fault_msg(soap, 400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:OutOfMemory",
			"get system config failed"));
	}

	 
	 int i = 0;
	 int token_exist = 0;
	 int j = 0;
	 int num_token = 0;
	 int index = 0;
	 int num = 0;
	 char _IPAddr[INFO_LENGTH];
	 NET_IPV4 ip;
	 ip.int32 = net_get_ifaddr(ETH_NAME);

	sprintf(_IPAddr, "http://%03d.%03d.%03d.%03d:8999/onvif/device_service", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
 
	 for (i = 0; i < oSysInfo->nprofile; i++){
		 token_exist = 0;
		 for (j = 0; j< i ; j++) {
			 if (strcmp(oSysInfo->Oprofile[j].MDC.MDtoken, oSysInfo->Oprofile[i].MDC.MDtoken) == 0) {
				 token_exist = 1;
				 break;
			 } 
		 }
		 
		 if (!token_exist) {
			 num_token++;
		 }
	 }
	 
	 trt__GetMetadataConfigurationsResponse->__sizeConfigurations = num_token;	
 
	 trt__GetMetadataConfigurationsResponse->Configurations = (struct tt__MetadataConfiguration *)soap_malloc_v2(soap, num_token * sizeof(struct tt__MetadataConfiguration));
	 memset(trt__GetMetadataConfigurationsResponse->Configurations, 0, num_token * sizeof(struct tt__MetadataConfiguration) );	 
	 
	 for (i = 0; i< oSysInfo->nprofile; i++)
	 {
		 token_exist = 0;
		 for (j = 0; j< i ; j++) {
			 if (strcmp(oSysInfo->Oprofile[j].MDC.MDtoken, oSysInfo->Oprofile[i].MDC.MDtoken) == 0) {
				 token_exist = 1;
			 }
		 }
		 
		 if (!token_exist && oSysInfo->Oprofile[i].MDC.MDtoken[0] ) {			
 
			// trt__GetMetadataConfigurationsResponse->Configurations[num].Name = oSysInfo->Oprofile[i].MDC.MDname;//"Face Detection";
			trt__GetMetadataConfigurationsResponse->Configurations[num].Name = (char*)soap_malloc_v2(soap, INFO_LENGTH);
			snprintf(trt__GetMetadataConfigurationsResponse->Configurations[num].Name, INFO_LENGTH - 1, "%s", oSysInfo->Oprofile[i].MDC.MDname);	
			trt__GetMetadataConfigurationsResponse->Configurations[num].Name[INFO_LENGTH - 1] = '\0';
			
			 trt__GetMetadataConfigurationsResponse->Configurations[num].UseCount = oSysInfo->Oprofile[i].MDC.MDusecount;//1;
			 
			// trt__GetMetadataConfigurationsResponse->Configurations[num].token = oSysInfo->Oprofile[i].MDC.MDtoken;//"Face Detection";
			trt__GetMetadataConfigurationsResponse->Configurations[num].token = (char*)soap_malloc_v2(soap, INFO_LENGTH);
			snprintf(trt__GetMetadataConfigurationsResponse->Configurations[num].token, INFO_LENGTH - 1, "%s", oSysInfo->Oprofile[i].MDC.MDtoken);
			trt__GetMetadataConfigurationsResponse->Configurations[num].token[INFO_LENGTH - 1] = '\0';

			 
			// trt__GetMetadataConfigurationsResponse->Configurations[num].SessionTimeout = "PT30M"; //oSysInfo->Oprofile[i].MDC.sessiontimeout;
			trt__GetMetadataConfigurationsResponse->Configurations[num].SessionTimeout = (char*)soap_malloc_v2(soap, INFO_LENGTH);
			sprintf(trt__GetMetadataConfigurationsResponse->Configurations[num].SessionTimeout, "%s", "PT1Y");
						 
			 num ++;
		 }
	 }
#endif

	 return SOAP_OK;
 }

int  __trt__GetAudioOutputConfiguration(struct soap *soap, struct _trt__GetAudioOutputConfiguration *trt__GetAudioOutputConfiguration, struct _trt__GetAudioOutputConfigurationResponse *trt__GetAudioOutputConfigurationResponse)
{
	if (!trt__GetAudioOutputConfiguration->ConfigurationToken || strcmp(trt__GetAudioOutputConfiguration->ConfigurationToken, "G711")) 
	{
		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
			"ter:NoVideoSource", "The requested VideoSource does not exist.");
	}
 
	trt__GetAudioOutputConfigurationResponse->Configuration = (struct tt__AudioOutputConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioOutputConfiguration));

 	char *token = soap_strdup(soap, "G711");
	trt__GetAudioOutputConfigurationResponse->Configuration->Name = token;
	trt__GetAudioOutputConfigurationResponse->Configuration->OutputToken = token;
	trt__GetAudioOutputConfigurationResponse->Configuration->token = token;
	trt__GetAudioOutputConfigurationResponse->Configuration->SendPrimacy = soap_strdup(soap, "www.onvif.org/ver20/HalfDuplex/Auto");
	 
	trt__GetAudioOutputConfigurationResponse->Configuration->OutputLevel = soap->pConfig->outputLevel;
	trt__GetAudioOutputConfigurationResponse->Configuration->UseCount = 1;
	 
	 return SOAP_OK; 
}
 int  __trt__GetAudioDecoderConfiguration(struct soap *soap, struct _trt__GetAudioDecoderConfiguration *trt__GetAudioDecoderConfiguration, struct _trt__GetAudioDecoderConfigurationResponse *trt__GetAudioDecoderConfigurationResponse)
 { 
 
	 ONVIF_DBP("");
 
	 if ( trt__GetAudioDecoderConfiguration->ConfigurationToken) {
		 ONVIF_DBP("ConfigurationToken %s \n", trt__GetAudioDecoderConfiguration->ConfigurationToken);
	 }
 
	 trt__GetAudioDecoderConfigurationResponse->Configuration = 
		 (struct tt__AudioDecoderConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioDecoderConfiguration));
	 
	 trt__GetAudioDecoderConfigurationResponse->Configuration->Name = (char*)soap_malloc_v2(soap, 32);
	 strcpy(trt__GetAudioDecoderConfigurationResponse->Configuration->Name, "G711");
 
	 trt__GetAudioDecoderConfigurationResponse->Configuration->token = (char*)soap_malloc_v2(soap, 32);
	 strcpy(trt__GetAudioDecoderConfigurationResponse->Configuration->token, "G711");
 
	 trt__GetAudioDecoderConfigurationResponse->Configuration->UseCount = 1;
 
	 
	 return SOAP_OK; 
 }

int  __trt__GetAudioOutputConfigurations(struct soap *soap, struct _trt__GetAudioOutputConfigurations *trt__GetAudioOutputConfigurations, struct _trt__GetAudioOutputConfigurationsResponse *trt__GetAudioOutputConfigurationsResponse)
{
	trt__GetAudioOutputConfigurationsResponse->__sizeConfigurations = 1;

	trt__GetAudioOutputConfigurationsResponse->Configurations = (struct tt__AudioOutputConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioOutputConfiguration));

 	char *token = soap_strdup(soap, "G711");
	trt__GetAudioOutputConfigurationsResponse->Configurations->Name = token;
 
	trt__GetAudioOutputConfigurationsResponse->Configurations->OutputToken = token;
 
	trt__GetAudioOutputConfigurationsResponse->Configurations->token = token;
 
	trt__GetAudioOutputConfigurationsResponse->Configurations->SendPrimacy = soap_strdup(soap, "www.onvif.org/ver20/HalfDuplex/Auto");
	 
	trt__GetAudioOutputConfigurationsResponse->Configurations->OutputLevel = soap->pConfig->outputLevel;
	trt__GetAudioOutputConfigurationsResponse->Configurations->UseCount = 1;
	
	return SOAP_OK; 
}
int  __trt__GetAudioDecoderConfigurations(struct soap *soap, struct _trt__GetAudioDecoderConfigurations *trt__GetAudioDecoderConfigurations, struct _trt__GetAudioDecoderConfigurationsResponse *trt__GetAudioDecoderConfigurationsResponse)
{ 
	ONVIF_DBP("");

	trt__GetAudioDecoderConfigurationsResponse->__sizeConfigurations = 1;

	trt__GetAudioDecoderConfigurationsResponse->Configurations = 
		(struct tt__AudioDecoderConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__AudioDecoderConfiguration));

	trt__GetAudioDecoderConfigurationsResponse->Configurations->Name = (char*)soap_malloc_v2(soap, 32);
	strcpy(trt__GetAudioDecoderConfigurationsResponse->Configurations->Name, "G711");

	trt__GetAudioDecoderConfigurationsResponse->Configurations->token = (char*)soap_malloc_v2(soap, 32);
	strcpy(trt__GetAudioDecoderConfigurationsResponse->Configurations->token, "G711");

	trt__GetAudioDecoderConfigurationsResponse->Configurations->UseCount = 1;

	return SOAP_OK; 
}

int __trt__GetVideoSourceConfiguration(struct soap *soap, struct _trt__GetVideoSourceConfiguration *trt__GetVideoSourceConfiguration, struct _trt__GetVideoSourceConfigurationResponse *trt__GetVideoSourceConfigurationResponse)
{
	ONVIF_DBP("");

	int     chno;
	
	VIDEO_CONF video[MAX_STREAM_NUM];
	memset(video,0,sizeof(VIDEO_CONF)*MAX_STREAM_NUM);

	if(trt__GetVideoSourceConfiguration->ConfigurationToken)
	{
		if(strncmp(trt__GetVideoSourceConfiguration->ConfigurationToken,"VideoSourceConfiguration", strlen("VideoSourceConfiguration")) == 0)
			chno = trt__GetVideoSourceConfiguration->ConfigurationToken[strlen(trt__GetVideoSourceConfiguration->ConfigurationToken)-1]-'0';
		else
			chno = 0;
	}
	else
		chno = 0;
	
	if((chno<0)||(chno>=soap->pConfig->g_channel_num))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                  "ter:InvalidArgVal",
	                  "ter:NoVideoSource",
	                  "The requested VideoSource does not exist."));
	}
#if 0
	if(get_channel_param(chno,video) != 0)
	{
		return SOAP_ERR;
	}
#endif
	trt__GetVideoSourceConfigurationResponse->Configuration = (struct tt__VideoSourceConfiguration*)soap_malloc_v2(soap, sizeof(struct tt__VideoSourceConfiguration));
	trt__GetVideoSourceConfigurationResponse->Configuration->Name =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetVideoSourceConfigurationResponse->Configuration->Name,"VideoSourceConfiguration%d",chno);
	trt__GetVideoSourceConfigurationResponse->Configuration->UseCount=MAX_STREAM_NUM;
	trt__GetVideoSourceConfigurationResponse->Configuration->token =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetVideoSourceConfigurationResponse->Configuration->token,"VideoSourceConfiguration%d",chno);
	trt__GetVideoSourceConfigurationResponse->Configuration->SourceToken =  (char *)soap_malloc(soap, 16);
	sprintf(trt__GetVideoSourceConfigurationResponse->Configuration->SourceToken,"VideoSource%d",chno);
	trt__GetVideoSourceConfigurationResponse->Configuration->Bounds = (struct tt__IntRectangle*)soap_malloc_v2(soap, sizeof(struct tt__IntRectangle));
	trt__GetVideoSourceConfigurationResponse->Configuration->Bounds->height = soap->pConfig->videosource_height;
	trt__GetVideoSourceConfigurationResponse->Configuration->Bounds->width = soap->pConfig->videosource_width;
	trt__GetVideoSourceConfigurationResponse->Configuration->Bounds->x = 0;
	trt__GetVideoSourceConfigurationResponse->Configuration->Bounds->y = 0;

	if (soap->pConfig->profile_cfg.isVaild && !strcmp(soap->pConfig->profile_cfg.videoSoureToken, trt__GetVideoSourceConfigurationResponse->Configuration->token))
	{
		++trt__GetVideoSourceConfigurationResponse->Configuration->UseCount;
	}
	return SOAP_OK; 
	
}


int __trt__GetVideoEncoderConfiguration(struct soap *soap, struct _trt__GetVideoEncoderConfiguration *trt__GetVideoEncoderConfiguration, struct _trt__GetVideoEncoderConfigurationResponse *trt__GetVideoEncoderConfigurationResponse)
{

	ONVIF_DBP("");

	VIDEO_CONF video[MAX_STREAM_NUM];
	memset(video,0,sizeof(VIDEO_CONF)*MAX_STREAM_NUM);

	int chno = soap->pConfig->g_channel_num;
/*
	if(trt__GetVideoEncoderConfiguration->ConfigurationToken==NULL)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                  "ter:InvalidArgVal",
	                  "ter:ConfigurationToken",
	                  "The ConfigurationToken is NULL. "));
	}
*/

	int stream_index, ret;
	if(trt__GetVideoEncoderConfiguration->ConfigurationToken==NULL)
	{
		chno = 0;
		stream_index = 0;
	}
	else if(!strncmp(trt__GetVideoEncoderConfiguration->ConfigurationToken, "VideoEncoderConfiguration", strlen("VideoEncoderConfiguration")))
	{
		ret = sscanf(trt__GetVideoEncoderConfiguration->ConfigurationToken, "VideoEncoderConfiguration%d_%d",&chno, &stream_index);
		if(ret != 2)
			return SOAP_ERR;
	}
	else 
	{
		chno = 0;
		stream_index = 0;
	}

	if(stream_index >= MAX_STREAM_NUM || chno >= soap->pConfig->g_channel_num)
	{
		return SOAP_ERR;
	}

	if(get_channel_param(chno,video) != 0)
	{
		return SOAP_ERR;
	}

	printf("get video encodertype is %d\n", video[stream_index].entype);
	trt__GetVideoEncoderConfigurationResponse->Configuration =  (struct tt__VideoEncoderConfiguration *)soap_malloc_v2(soap,sizeof(struct tt__VideoEncoderConfiguration));

	trt__GetVideoEncoderConfigurationResponse->Configuration->Name = (char *)soap_malloc(soap, 32);
	sprintf(trt__GetVideoEncoderConfigurationResponse->Configuration->Name,"VideoEncoderConfiguration%d_%d",chno, stream_index);
	trt__GetVideoEncoderConfigurationResponse->Configuration->UseCount = 1;
	trt__GetVideoEncoderConfigurationResponse->Configuration->token = (char *)soap_malloc(soap, 32);
	sprintf(trt__GetVideoEncoderConfigurationResponse->Configuration->token,"VideoEncoderConfiguration%d_%d",chno, stream_index);

	if(QMAPI_PT_JPEG == video[stream_index].entype
		|| QMAPI_PT_MJPEG == video[stream_index].entype)  //26
	{
	    	trt__GetVideoEncoderConfigurationResponse->Configuration->Encoding =  tt__VideoEncoding__JPEG;
	}
	else if(QMAPI_PT_H264 == video[stream_index].entype
		|| QMAPI_PT_H264_HIGHPROFILE == video[stream_index].entype
		|| QMAPI_PT_H264_BASELINE == video[stream_index].entype
		|| QMAPI_PT_H264_MAINPROFILE == video[stream_index].entype
		|| QMAPI_PT_H265 == video[stream_index].entype)
	{
	    	trt__GetVideoEncoderConfigurationResponse->Configuration->Encoding =  tt__VideoEncoding__H264;
	    	trt__GetVideoEncoderConfigurationResponse->Configuration->H264 = (struct tt__H264Configuration *)soap_malloc_v2(soap, sizeof(struct tt__H264Configuration));
	    	trt__GetVideoEncoderConfigurationResponse->Configuration->H264->GovLength = video[stream_index].gop;
		switch(video[stream_index].entype)
		{
			case QMAPI_PT_H264_BASELINE:
				trt__GetVideoEncoderConfigurationResponse->Configuration->H264->H264Profile = tt__H264Profile__Baseline;
				break;
			case QMAPI_PT_H264_MAINPROFILE:
			case QMAPI_PT_H264:			
				trt__GetVideoEncoderConfigurationResponse->Configuration->H264->H264Profile = tt__H264Profile__Main;
				break;
			default:
				trt__GetVideoEncoderConfigurationResponse->Configuration->H264->H264Profile = tt__H264Profile__High;
				break;
		}
	}
#if 0	
	else if(99 == video[stream_index].entype)
	{
	    	trt__GetVideoEncoderConfigurationResponse->Configuration->Encoding =  tt__VideoEncoding__MPEG4;
	    	trt__GetVideoEncoderConfigurationResponse->Configuration->MPEG4= (struct tt__Mpeg4Configuration *)soap_malloc_v2(soap, sizeof(struct tt__Mpeg4Configuration));
	    	trt__GetVideoEncoderConfigurationResponse->Configuration->MPEG4->GovLength = video[stream_index].gop;
	    	trt__GetVideoEncoderConfigurationResponse->Configuration->MPEG4->Mpeg4Profile = tt__Mpeg4Profile__SP;
	}
#endif
	trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution = (struct tt__VideoResolution *)soap_malloc_v2(soap, sizeof(struct tt__VideoResolution));
	trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution->Width = video[stream_index].width;
	trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution->Height = video[stream_index].height;

	trt__GetVideoEncoderConfigurationResponse->Configuration->Quality = 5.0 - video[stream_index].piclevel;

	trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl = (struct tt__VideoRateControl *)soap_malloc_v2(soap, sizeof(struct tt__VideoRateControl));
	trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->BitrateLimit = video[stream_index].bitrate;
	trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->EncodingInterval = 1;
	trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->FrameRateLimit = video[stream_index].fps;
#if 1
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->Type = tt__IPType__IPv4;
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
	*trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, 32);
	strcpy(*trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address, RTP_MULTICAST_IP);
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Port = 0;
	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->TTL = 1;
#endif	
	trt__GetVideoEncoderConfigurationResponse->Configuration->SessionTimeout = (char *)soap_malloc(soap, 8);
	strcpy(trt__GetVideoEncoderConfigurationResponse->Configuration->SessionTimeout, "PT30M");

	if (soap->pConfig->profile_cfg.isVaild && !strcmp(soap->pConfig->profile_cfg.videoEncoderToken, trt__GetVideoEncoderConfigurationResponse->Configuration->token))
	{
		++trt__GetVideoEncoderConfigurationResponse->Configuration->UseCount;
	}	
	return SOAP_OK;
	
}

int __trt__GetAudioSourceConfiguration(struct soap *soap, struct _trt__GetAudioSourceConfiguration *trt__GetAudioSourceConfiguration, struct _trt__GetAudioSourceConfigurationResponse *trt__GetAudioSourceConfigurationResponse)
{

	ONVIF_DBP("");

	int chno;
	char *p;
/*
	if(!trt__GetAudioSourceConfiguration->ConfigurationToken||
		0!=strncmp(trt__GetAudioSourceConfiguration->ConfigurationToken,"AudioSourceConfiguration", strlen("AudioSourceConfiguration")))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}	
*/
	if(!trt__GetAudioSourceConfiguration->ConfigurationToken)
	{
		chno = 0;
	}
	else
	{
		if(0!=strncmp(trt__GetAudioSourceConfiguration->ConfigurationToken,"AudioSourceConfiguration", strlen("AudioSourceConfiguration")))
		{
			return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
		}
		
		p = trt__GetAudioSourceConfiguration->ConfigurationToken+strlen("AudioSourceConfiguration");
		chno = atoi(p);
	}
	
	if(soap->pConfig->g_channel_num<(chno+1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}
	

	/* 务必先置0,否则gsoap出现段错误或死循环 */
	trt__GetAudioSourceConfigurationResponse->Configuration = (struct tt__AudioSourceConfiguration*)soap_malloc_v2(soap, sizeof(struct tt__AudioSourceConfiguration));

	trt__GetAudioSourceConfigurationResponse->Configuration->Name =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetAudioSourceConfigurationResponse->Configuration->Name,"AudioSourceConfiguration%d",chno);
	trt__GetAudioSourceConfigurationResponse->Configuration->UseCount=MAX_STREAM_NUM;
	trt__GetAudioSourceConfigurationResponse->Configuration->token =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetAudioSourceConfigurationResponse->Configuration->token,"AudioSourceConfiguration%d",chno);
	trt__GetAudioSourceConfigurationResponse->Configuration->SourceToken =  (char *)soap_malloc(soap, 16);
	sprintf(trt__GetAudioSourceConfigurationResponse->Configuration->SourceToken,"AudioSource%d",chno);

	if (soap->pConfig->profile_cfg.isVaild && !strcmp(soap->pConfig->profile_cfg.audioSoureToken, trt__GetAudioSourceConfigurationResponse->Configuration->token))
	{
		++trt__GetAudioSourceConfigurationResponse->Configuration->UseCount;
	}

	return SOAP_OK; 
}

int __trt__GetAudioEncoderConfiguration(struct soap *soap, struct _trt__GetAudioEncoderConfiguration *trt__GetAudioEncoderConfiguration, struct _trt__GetAudioEncoderConfigurationResponse *trt__GetAudioEncoderConfigurationResponse)
{

	ONVIF_DBP("");

	int chno;
/*
	if(!trt__GetAudioEncoderConfiguration->ConfigurationToken||
		0!=strncmp(trt__GetAudioEncoderConfiguration->ConfigurationToken,"AudioEncoderConfiguration", strlen("AudioEncoderConfiguration")))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}	
*/

	printf("%d %s %s\n", soap->pConfig->profile_cfg.isVaild, trt__GetAudioEncoderConfiguration->ConfigurationToken, soap->pConfig->profile_cfg.audioEncoderToken);
	if(!trt__GetAudioEncoderConfiguration->ConfigurationToken)
	{
		chno = 0;
	}
	else
	{
		if (soap->pConfig->profile_cfg.isVaild && !strcmp(trt__GetAudioEncoderConfiguration->ConfigurationToken, soap->pConfig->profile_cfg.audioEncoderToken))
		{
			chno = 0;
		}
		else if(!strncmp(trt__GetAudioEncoderConfiguration->ConfigurationToken,"AudioEncoderConfiguration", strlen("AudioEncoderConfiguration")))
		{
			chno = atoi(trt__GetAudioEncoderConfiguration->ConfigurationToken + strlen("AudioEncoderConfiguration"));
		}
		else
		{
			return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));			
		}
	}

	printf("chno is %d\n", chno);
	if(soap->pConfig->g_channel_num<(chno+1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}

	/* 务必先置0,否则gsoap出现段错误或死循环 */
	trt__GetAudioEncoderConfigurationResponse->Configuration = (struct tt__AudioEncoderConfiguration*)soap_malloc_v2(soap, sizeof(struct tt__AudioEncoderConfiguration));

	trt__GetAudioEncoderConfigurationResponse->Configuration->Name =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetAudioEncoderConfigurationResponse->Configuration->Name, "AudioEncoderConfiguration%d", chno);
	trt__GetAudioEncoderConfigurationResponse->Configuration->UseCount=MAX_STREAM_NUM;
	trt__GetAudioEncoderConfigurationResponse->Configuration->token =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetAudioEncoderConfigurationResponse->Configuration->token,"AudioEncoderConfiguration%d",chno);

	QMAPI_NET_CHANNEL_PIC_INFO info;
	memset(&info, 0, sizeof(info));
	info.dwSize = sizeof(info);
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, chno, &info, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
	trt__GetAudioEncoderConfigurationResponse->Configuration->Encoding =  info.stRecordPara.wFormatTag == QMAPI_PT_G726 ? tt__AudioEncoding__G726 : tt__AudioEncoding__G711;
	trt__GetAudioEncoderConfigurationResponse->Configuration->Bitrate = 8;

	trt__GetAudioEncoderConfigurationResponse->Configuration->SampleRate = 16;
#if 1
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->Address->Type = tt__IPType__IPv4;
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
	*trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, 32);
	strcpy(*trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address, RTP_MULTICAST_IP);
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->Port = 0;
	trt__GetAudioEncoderConfigurationResponse->Configuration->Multicast->TTL = 1;
#endif	
	trt__GetAudioEncoderConfigurationResponse->Configuration->SessionTimeout = (char *)soap_malloc(soap, 8);
	sprintf(trt__GetAudioEncoderConfigurationResponse->Configuration->SessionTimeout, "PT30M");

	if (soap->pConfig->profile_cfg .isVaild && !strcmp(trt__GetAudioEncoderConfiguration->ConfigurationToken, soap->pConfig->profile_cfg.audioEncoderToken))
	{
		++trt__GetAudioEncoderConfigurationResponse->Configuration->UseCount;
	}
	
	return SOAP_OK;	 
}

int  __trt__GetVideoAnalyticsConfiguration(struct soap *soap, struct _trt__GetVideoAnalyticsConfiguration *trt__GetVideoAnalyticsConfiguration, struct _trt__GetVideoAnalyticsConfigurationResponse *trt__GetVideoAnalyticsConfigurationResponse)
{ 

	ONVIF_DBP("");

	return SOAP_OK; 
}

 int __trt__GetMetadataConfiguration(struct soap *soap, struct _trt__GetMetadataConfiguration *trt__GetMetadataConfiguration, struct _trt__GetMetadataConfigurationResponse *trt__GetMetadataConfigurationResponse)
 {

  	ONVIF_DBP("");
 #if 0		 
	 SysInfo *oSysInfo = GetSysInfo();

	  if ( !oSysInfo ) {
		return(send_soap_fault_msg(soap, 400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:OutOfMemory",
			"get system config failed"));
	}

	 
	 char _name[INFO_LENGTH] = { 0 };
	 int i = 0,token_exist = 0;
	 char _IPAddr[INFO_LENGTH] = { 0 };
	 NET_IPV4 ip;
	 ip.int32 = net_get_ifaddr(ETH_NAME);
 
	 sprintf(_IPAddr, "http://%03d.%03d.%03d.%03d/onvif/services", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	 for(i = 0; i< oSysInfo->nprofile; i++)
	 {
		 if(strcmp(trt__GetMetadataConfiguration->ConfigurationToken, oSysInfo->Oprofile[i].MDC.MDtoken) == 0)
		 {
			 token_exist = 1;
			 break;
		 }
	 }
	 if(!token_exist)
	 {
		 //onvif_fault(soap,"ter:InvalidArgVal", "ter:NoProfile");
		 return SOAP_FAULT;
	 }
 
	 trt__GetMetadataConfigurationResponse->Configuration = (struct tt__MetadataConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MetadataConfiguration));
	 memset(trt__GetMetadataConfigurationResponse->Configuration, 0, sizeof(struct tt__MetadataConfiguration));	 

	 trt__GetMetadataConfigurationResponse->Configuration[0].Name = (char*)soap_malloc_v2(soap, INFO_LENGTH);
	 snprintf(trt__GetMetadataConfigurationResponse->Configuration[0].Name, INFO_LENGTH - 1, "%s", oSysInfo->Oprofile[i].MDC.MDname);
	 trt__GetMetadataConfigurationResponse->Configuration[0].Name[INFO_LENGTH - 1] = '\0';
	 
	 trt__GetMetadataConfigurationResponse->Configuration[0].UseCount = oSysInfo->Oprofile[i].MDC.MDusecount;//1; 

	 trt__GetMetadataConfigurationResponse->Configuration[0].token = (char*)soap_malloc_v2(soap, INFO_LENGTH);
	 snprintf(trt__GetMetadataConfigurationResponse->Configuration[0].token, INFO_LENGTH - 1, "%s", oSysInfo->Oprofile[i].MDC.MDtoken);
	 trt__GetMetadataConfigurationResponse->Configuration[0].token[INFO_LENGTH - 1] = '\0'; 
	 
	 trt__GetMetadataConfigurationResponse->Configuration[0].PTZStatus = NULL;// (struct tt__PTZFilter*)soap_malloc_v2(soap,sizeof(struct tt__PTZFilter));
	 trt__GetMetadataConfigurationResponse->Configuration[0].Events = NULL;// (struct tt__EventSubscription*)soap_malloc_v2(soap,sizeof(struct tt__EventSubscription));
	 trt__GetMetadataConfigurationResponse->Configuration[0].Analytics = NULL;//(int *)soap_malloc(soap,sizeof(int));
	 
	 trt__GetMetadataConfigurationResponse->Configuration[0].Analytics = (int *)soap_malloc(soap, sizeof(int));
	 
	 *trt__GetMetadataConfigurationResponse->Configuration[0].Analytics = 1;
	 
	 trt__GetMetadataConfigurationResponse->Configuration[0].Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));
	 memset(trt__GetMetadataConfigurationResponse->Configuration[0].Multicast, 0, sizeof(struct tt__MulticastConfiguration) ); 
	 
	 trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
	 trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Address->Type = oSysInfo->Oprofile[i].MDC.VMC.nIPType;
		 trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
	 trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Address->IPv4Address[0] = (char *)soap_malloc(soap, sizeof(char) * INFO_LENGTH);
	 strcpy(*trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Address->IPv4Address, _IPAddr);
	 trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Address->IPv6Address = NULL;
	 trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->Port = oSysInfo->Oprofile[i].MDC.VMC.port;
	 trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->TTL = oSysInfo->Oprofile[i].MDC.VMC.ttl;
	 trt__GetMetadataConfigurationResponse->Configuration[0].Multicast->AutoStart = oSysInfo->Oprofile[i].MDC.VMC.autostart;
#endif

	 return SOAP_OK;
 }




int  __trt__GetCompatibleVideoEncoderConfigurations(struct soap *soap,
	struct _trt__GetCompatibleVideoEncoderConfigurations *trt__GetCompatibleVideoEncoderConfigurations,
	struct _trt__GetCompatibleVideoEncoderConfigurationsResponse *trt__GetCompatibleVideoEncoderConfigurationsResponse)
{

	ONVIF_DBP("");

	int i = 0;
	VIDEO_CONF video[MAX_STREAM_NUM];
	memset(video,0,sizeof(VIDEO_CONF)*MAX_STREAM_NUM);
	
#if 1   //panhn added for axxon
	if(trt__GetCompatibleVideoEncoderConfigurations->ProfileToken==NULL)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                  "ter:InvalidArgVal",
	                  "ter:ConfigurationToken",
	                  "The ConfigurationToken is NULL. "));
	}

	int ret, stream_index,chn;
	if(!strncmp(trt__GetCompatibleVideoEncoderConfigurations->ProfileToken, "profile", strlen("profile")))
	{
		ret = sscanf(trt__GetCompatibleVideoEncoderConfigurations->ProfileToken, "profile%d_%d",&chn, &stream_index);
		if(ret != 2)
			return SOAP_ERR;
	}
	else if(!strncmp(trt__GetCompatibleVideoEncoderConfigurations->ProfileToken, "axxon", strlen("axxon")))
	{
		stream_index = 0;
		chn = 0;
	}
	else if (!strcmp(trt__GetCompatibleVideoEncoderConfigurations->ProfileToken, soap->pConfig->profile_cfg.token))
	{
		if (soap->pConfig->profile_cfg.isVaild)
		{
			sscanf(soap->pConfig->profile_cfg.videoEncoderToken, "VideoEncoderConfiguration%d_%d", &chn, &stream_index);
			stream_index = 0;
		}
		else
		{
			return SOAP_OK;
		}
	}	
	else 
		return SOAP_ERR;

	if(stream_index >= MAX_STREAM_NUM || chn >= soap->pConfig->g_channel_num)
	{
		return SOAP_ERR;
	}
	
	if(get_channel_param(chn,video) != 0)
	{
		return SOAP_ERR;
	}

	trt__GetCompatibleVideoEncoderConfigurationsResponse->__sizeConfigurations =  1;
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations = (struct tt__VideoEncoderConfiguration*)soap_malloc_v2(soap, sizeof(struct tt__VideoEncoderConfiguration));

	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->Name = (char *)soap_malloc(soap, 32);
	sprintf(trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->Name,"VideoEncoderConfiguration%d_%d",chn, stream_index);
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->UseCount = 1;
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->token = (char *)soap_malloc(soap, 32);
	sprintf(trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->token,"VideoEncoderConfiguration%d_%d",chn, stream_index);
	
	if(QMAPI_PT_JPEG == video[stream_index].entype
		|| QMAPI_PT_MJPEG == video[stream_index].entype)  //26
	{
	    	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->Encoding =  tt__VideoEncoding__JPEG;
	}
	else if(QMAPI_PT_H264 == video[stream_index].entype
		|| QMAPI_PT_H264_HIGHPROFILE == video[stream_index].entype
		|| QMAPI_PT_H264_BASELINE == video[stream_index].entype
		|| QMAPI_PT_H264_MAINPROFILE == video[stream_index].entype
		|| QMAPI_PT_H265 == video[stream_index].entype)
	{
	    	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->Encoding =  tt__VideoEncoding__H264;
	    	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->H264 = (struct tt__H264Configuration *)soap_malloc_v2(soap, sizeof(struct tt__H264Configuration));
	    	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->H264->GovLength = video[stream_index].gop;
		switch(video[stream_index].entype)
		{
			case QMAPI_PT_H264_BASELINE:
				trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->H264->H264Profile = tt__H264Profile__Baseline;
				break;
			case QMAPI_PT_H264_MAINPROFILE:
			case QMAPI_PT_H264:
				trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->H264->H264Profile = tt__H264Profile__Main;
				break;
			default:
				trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->H264->H264Profile = tt__H264Profile__High;
				break;
		}
	}
	else if(99 == video[stream_index].entype)
	{
	    	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->Encoding =  tt__VideoEncoding__MPEG4;
	    	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->MPEG4= (struct tt__Mpeg4Configuration *)soap_malloc_v2(soap, sizeof(struct tt__Mpeg4Configuration));
	    	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->MPEG4->GovLength = video[stream_index].gop;
	    	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->MPEG4->Mpeg4Profile = tt__Mpeg4Profile__SP;
	}

	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->Resolution = (struct tt__VideoResolution *)soap_malloc_v2(soap, sizeof(struct tt__VideoResolution));
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->Resolution->Width = video[stream_index].width;
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations->Resolution->Height = video[stream_index].height;

	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Quality = 5 - video[stream_index].piclevel;

	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].RateControl = (struct tt__VideoRateControl *)soap_malloc_v2(soap, sizeof(struct tt__VideoRateControl));
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].RateControl->BitrateLimit = video[stream_index].bitrate;
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].RateControl->EncodingInterval = 1;
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].RateControl->FrameRateLimit = video[stream_index].fps;
#if 1
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->Type = tt__IPType__IPv4;
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
	*trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address = (char *)soap_malloc(soap, 32);
	strcpy(*trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address, RTP_MULTICAST_IP);
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Port = 0;
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].Multicast->TTL = 1;
#endif
	
	trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].SessionTimeout = (char *)soap_malloc(soap, 8);
	strcpy(trt__GetCompatibleVideoEncoderConfigurationsResponse->Configurations[i].SessionTimeout, "PT30M");
#endif	

	return SOAP_OK;
}

int  __trt__GetCompatibleVideoSourceConfigurations(struct soap *soap,
	struct _trt__GetCompatibleVideoSourceConfigurations *trt__GetCompatibleVideoSourceConfigurations,
	struct _trt__GetCompatibleVideoSourceConfigurationsResponse *trt__GetCompatibleVideoSourceConfigurationsResponse)
{

	ONVIF_DBP("");

	if(trt__GetCompatibleVideoSourceConfigurations->ProfileToken==NULL)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                  "ter:InvalidArgVal",
	                  "ter:ConfigurationToken",
	                  "The ConfigurationToken is NULL. "));
	}

	int ret, stream_index,chn;
	if(!strncmp(trt__GetCompatibleVideoSourceConfigurations->ProfileToken, "profile", strlen("profile")))
	{
		ret = sscanf(trt__GetCompatibleVideoSourceConfigurations->ProfileToken, "profile%d_%d",&chn, &stream_index);
		if(ret != 2)
			return SOAP_ERR;
	}
	else if(!strncmp(trt__GetCompatibleVideoSourceConfigurations->ProfileToken, "axxon", strlen("axxon")))
	{
		stream_index = 0;
		chn = 0;
	}
	else if (!strcmp(trt__GetCompatibleVideoSourceConfigurations->ProfileToken, soap->pConfig->profile_cfg.token))
	{
		if (!soap->pConfig->profile_cfg.isVaild)
		{
			int ch, index;
			sscanf(soap->pConfig->profile_cfg.videoEncoderToken, "VideoEncoderConfiguration%d_%d", &ch, &index);
		}
		else
		{
			return SOAP_OK;
		}
	}	
	else 
		return SOAP_ERR;

	if(stream_index >= MAX_STREAM_NUM || chn >= soap->pConfig->g_channel_num)
	{
		return SOAP_ERR;
	}

	VIDEO_CONF video[MAX_STREAM_NUM];
	memset(video,0,sizeof(VIDEO_CONF)*MAX_STREAM_NUM);

	if(get_channel_param(chn, video) != 0)
	{
		return SOAP_ERR;
	}

	trt__GetCompatibleVideoSourceConfigurationsResponse->__sizeConfigurations = 1;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations = (struct tt__VideoSourceConfiguration*)soap_malloc_v2(soap, sizeof(struct tt__VideoSourceConfiguration));
	
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations->Name = (char *)soap_malloc(soap, 32);
	sprintf(trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations->Name,"VideoSourceConfiguration%d", chn); 
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations->UseCount = MAX_STREAM_NUM;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations->token = (char *)soap_malloc(soap, 32);
	sprintf(trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations->token,"VideoSourceConfiguration%d", chn);
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations->SourceToken = (char *)soap_malloc(soap, 32);
	sprintf(trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations->SourceToken,"VideoSource%d", chn);
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations->Bounds = (struct tt__IntRectangle *)soap_malloc_v2(soap, sizeof(struct tt__IntRectangle));
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations->Bounds->x = 0;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations->Bounds->y =  0;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations->Bounds->width = video[0].width;
	trt__GetCompatibleVideoSourceConfigurationsResponse->Configurations->Bounds->height = video[0].height;
	
	return SOAP_OK;
}


 int __trt__GetCompatibleAudioEncoderConfigurations(struct soap *soap, struct _trt__GetCompatibleAudioEncoderConfigurations *trt__GetCompatibleAudioEncoderConfigurations, struct _trt__GetCompatibleAudioEncoderConfigurationsResponse *trt__GetCompatibleAudioEncoderConfigurationsResponse)
 {

 	ONVIF_DBP("");

	int ret, stream_index,chn;

	if(trt__GetCompatibleAudioEncoderConfigurations->ProfileToken==NULL)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                  "ter:InvalidArgVal",
	                  "ter:ConfigurationToken",
	                  "The ConfigurationToken is NULL. "));
	}
	
	if(!strncmp(trt__GetCompatibleAudioEncoderConfigurations->ProfileToken, "profile", strlen("profile")))
	{
		ret = sscanf(trt__GetCompatibleAudioEncoderConfigurations->ProfileToken, "profile%d_%d",&chn, &stream_index);
		if(ret != 2)
			return SOAP_ERR;
	}
	else if(!strncmp(trt__GetCompatibleAudioEncoderConfigurations->ProfileToken, "axxon", strlen("axxon")))
	{
		stream_index = 0;
		chn = 0;
	}
	else if (!strcmp(trt__GetCompatibleAudioEncoderConfigurations->ProfileToken, soap->pConfig->profile_cfg.token))
	{
		if (soap->pConfig->profile_cfg.isVaild)
		{
			sscanf(soap->pConfig->profile_cfg.audioEncoderToken, "AudioEncoderConfiguration%d", &chn);
			stream_index = 0;
		}
		else
		{
			return SOAP_OK;
		}
	}
	else 
		return SOAP_ERR;
	
	/* profile个数 */
	trt__GetCompatibleAudioEncoderConfigurationsResponse->__sizeConfigurations = 1;
	/* 务必先置0,否则gsoap出现段错误或死循环 */
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations = 
		(struct tt__AudioEncoderConfiguration*)soap_malloc_v2(soap, sizeof(struct tt__AudioEncoderConfiguration));

	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Name =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Name,"AudioEncoderConfiguration%d", chn);
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->UseCount=MAX_STREAM_NUM;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->token =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->token,"AudioEncoderConfiguration%d", chn);

	QMAPI_NET_CHANNEL_PIC_INFO info;
	memset(&info, 0, sizeof(info));
	info.dwSize = sizeof(info);
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, chn, &info, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Encoding = info.stRecordPara.wFormatTag == QMAPI_PT_G726 ? tt__AudioEncoding__G726 : tt__AudioEncoding__G711;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Bitrate = 8;

	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->SampleRate = 16;
#if 1
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast = (struct tt__MulticastConfiguration *)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->Address = (struct tt__IPAddress *)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->Address->Type = tt__IPType__IPv4;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));
	*trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, 32);
	strcpy(*trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->Address->IPv4Address, RTP_MULTICAST_IP);
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->Port = 0;
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->Multicast->TTL = 1;
#endif	
	trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->SessionTimeout = (char *)soap_malloc(soap, 8);
	sprintf(trt__GetCompatibleAudioEncoderConfigurationsResponse->Configurations->SessionTimeout, "PT30M");

	return SOAP_OK; 
 }



 int __trt__GetCompatibleAudioSourceConfigurations(struct soap *soap, struct _trt__GetCompatibleAudioSourceConfigurations *trt__GetCompatibleAudioSourceConfigurations, struct _trt__GetCompatibleAudioSourceConfigurationsResponse *trt__GetCompatibleAudioSourceConfigurationsResponse)
 {
	ONVIF_DBP("");

	int ret, stream_index,chn;

	if(trt__GetCompatibleAudioSourceConfigurations->ProfileToken==NULL)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                  "ter:InvalidArgVal",
	                  "ter:ConfigurationToken",
	                  "The ConfigurationToken is NULL. "));
	}
	
	if(!strncmp(trt__GetCompatibleAudioSourceConfigurations->ProfileToken, "profile", strlen("profile")))
	{
		ret = sscanf(trt__GetCompatibleAudioSourceConfigurations->ProfileToken, "profile%d_%d",&chn, &stream_index);
		if(ret != 2)
			return SOAP_ERR;
	}
	else if(!strncmp(trt__GetCompatibleAudioSourceConfigurations->ProfileToken, "axxon", strlen("axxon")))
	{
		stream_index = 0;
		chn = 0;
	}
	else if (!strcmp(trt__GetCompatibleAudioSourceConfigurations->ProfileToken, soap->pConfig->profile_cfg.token))
	{
		if (soap->pConfig->profile_cfg.isVaild)
		{
			sscanf(soap->pConfig->profile_cfg.videoSoureToken, "AudioSourceConfiguration%d", &chn);
			stream_index = 0;
		}
		else
		{
			return SOAP_OK;
		}
	}	
	else 
		return SOAP_ERR;
	
	/* profile个数 */
	trt__GetCompatibleAudioSourceConfigurationsResponse->__sizeConfigurations = 1;
	/* 务必先置0,否则gsoap出现段错误或死循环 */
	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations = (struct tt__AudioSourceConfiguration*)soap_malloc_v2(soap, sizeof(struct tt__AudioSourceConfiguration));

	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->Name =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->Name,"AudioSourceConfiguration%d",chn);
	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->UseCount=0;
	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->token =  (char *)soap_malloc(soap, 32);
	sprintf(trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->token,"AudioSourceConfiguration%d",chn);
	trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->SourceToken =  (char *)soap_malloc(soap, 16);
	//sprintf(trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->SourceToken,"axxon0");
	sprintf(trt__GetCompatibleAudioSourceConfigurationsResponse->Configurations->SourceToken,"AudioSource%d",chn);
	return SOAP_OK;
	 
 }



int  __trt__GetCompatibleVideoAnalyticsConfigurations(struct soap *soap, struct _trt__GetCompatibleVideoAnalyticsConfigurations *trt__GetCompatibleVideoAnalyticsConfigurations, struct _trt__GetCompatibleVideoAnalyticsConfigurationsResponse *trt__GetCompatibleVideoAnalyticsConfigurationsResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __trt__GetCompatibleMetadataConfigurations(struct soap *soap,
	struct _trt__GetCompatibleMetadataConfigurations *trt__GetCompatibleMetadataConfigurations,
         struct _trt__GetCompatibleMetadataConfigurationsResponse *trt__GetCompatibleMetadataConfigurationsResponse)
{	
	ONVIF_DBP("");
	
	int i;
	trt__GetCompatibleMetadataConfigurationsResponse->__sizeConfigurations = 2;

	trt__GetCompatibleMetadataConfigurationsResponse->Configurations = (struct tt__MetadataConfiguration*)soap_malloc_v2(soap, sizeof(struct tt__MetadataConfiguration)*2);

	for(i=0; i<2; i++)
	{
		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].Name = (char *)soap_malloc(soap, 8);
		sprintf(trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].Name,"user%d",i);

		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].UseCount = 0;

		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].token = (char *)soap_malloc(soap, 8);
		sprintf(trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].token,"user%d",i);
#if 1
		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].Multicast = (struct tt__MulticastConfiguration*)soap_malloc_v2(soap, sizeof(struct tt__MulticastConfiguration));

		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].Multicast->Address = (struct tt__IPAddress*)soap_malloc_v2(soap, sizeof(struct tt__IPAddress));
		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].Multicast->Address->Type = tt__IPType__IPv4;
		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address = (char **)soap_malloc(soap, sizeof(char *));  // change
		*(trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address) = (char *)soap_malloc(soap, 16);
		sprintf(*(trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address),"0.0.0.0");

		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].Multicast->Port = 0;
		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].Multicast->TTL = 5;
		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].Multicast->AutoStart = xsd__boolean__false_;
#endif
		trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].SessionTimeout = (char *)soap_malloc(soap, 8);
		sprintf(trt__GetCompatibleMetadataConfigurationsResponse->Configurations[i].SessionTimeout,"PT1Y");

	}

	return SOAP_OK;
}


int  __trt__GetCompatibleAudioOutputConfigurations(struct soap *soap, struct _trt__GetCompatibleAudioOutputConfigurations *trt__GetCompatibleAudioOutputConfigurations, struct _trt__GetCompatibleAudioOutputConfigurationsResponse *trt__GetCompatibleAudioOutputConfigurationsResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}
int  __trt__GetCompatibleAudioDecoderConfigurations(struct soap *soap, struct _trt__GetCompatibleAudioDecoderConfigurations *trt__GetCompatibleAudioDecoderConfigurations, struct _trt__GetCompatibleAudioDecoderConfigurationsResponse *trt__GetCompatibleAudioDecoderConfigurationsResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int __trt__SetVideoSourceConfiguration(struct soap *soap, struct _trt__SetVideoSourceConfiguration *trt__SetVideoSourceConfiguration, struct _trt__SetVideoSourceConfigurationResponse *trt__SetVideoSourceConfigurationResponse)
{

	ONVIF_DBP("");

#if 0	
	SysInfo* oSysInfo = GetSysInfo();

	if ( !oSysInfo ) {
		return(send_soap_fault_msg(soap, 400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:OutOfMemory",
			"get system config failed"));
	}

	

	
	video_source_conf set_video_source_t = { 0 }; 
	int i;
	int j = 0;
	int flg = 0;
	int Ptoken_exist = 0;
	int ret;
	int num_token = 0;
	__u8 codec_combo;
	__u8 codec_res;
	__u8 mode = 0;
	 int _width;
	 int _height;

	if ( ! trt__SetVideoSourceConfiguration->Configuration ) {		
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:ConfigModify",
			"The Set VideoSourceConfiguration error"));
	}

	if (trt__SetVideoSourceConfiguration->Configuration->Bounds != NULL) {
		_width = trt__SetVideoSourceConfiguration->Configuration->Bounds->width;
		_height = trt__SetVideoSourceConfiguration->Configuration->Bounds->height;

	}

	if (((trt__SetVideoSourceConfiguration->Configuration->Bounds->height) %2) != 0) {
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:ConfigModify",
			"The Set VideoSourceConfiguration error"));		
	}	
		
	for (i = 0; i < oSysInfo->nprofile ; i++) {
		for (j = 0; j <= i; j++) {
			if (strcmp(oSysInfo->Oprofile[j].AVSC.Vtoken, oSysInfo->Oprofile[i].AVSC.Vtoken)==0){
				flg = 1;		
				break;
			}
		}
		if ( flg == 0){
			num_token++;
		}
	}

	if (trt__SetVideoSourceConfiguration->Configuration->token != NULL) {
		strncpy(oSysInfo->Oprofile[0].AVSC.Vtoken, trt__SetVideoSourceConfiguration->Configuration->token, MAX_PROF_TOKEN - 1);	
		for (i = 0; i <= oSysInfo->nprofile; i++) {
			if(strcmp(trt__SetVideoSourceConfiguration->Configuration->token, oSysInfo->Oprofile[i].AVSC.Vtoken) == 0)
			{
				Ptoken_exist = 1;
				break;
			}
		}
	}
	/* if ConfigurationToken does not exist */
	if (!Ptoken_exist) {	
		return (send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
		         "ter:InvalidArgVal",
		         "ter:NoVideoSource",
		         "no VideoSource Configuration error"));		
	}

	/* check for width and height */
	if (!((_width >= 176 && _width <= 2048) && (_height >= 144 && _height <= 1536))){
		return (send_soap_fault_msg(soap, 400,
			"SOAP-ENV:Sender",
		         "ter:InvalidArgVal",
		         "ter:ConfigModify",
		         "invalid width or height"));		
		
	}
	
	if (trt__SetVideoSourceConfiguration->Configuration->token != NULL)
	{
		for(i = 0; i< oSysInfo->nprofile; i++)
		{
			Ptoken_exist = 0;
			if (strcmp(trt__SetVideoSourceConfiguration->Configuration->token, oSysInfo->Oprofile[i].AVSC.Vtoken) == 0)
			{
				Ptoken_exist = 1;
			}

			if (Ptoken_exist)
			{
				set_video_source_t.position = i; 
				strncpy(set_video_source_t.video_source_conf_in_t.Vtoken_t, 
					trt__SetVideoSourceConfiguration->Configuration->token, MAX_PROF_TOKEN - 1 );
				set_video_source_t.video_source_conf_in_t.Vtoken_t[MAX_PROF_TOKEN - 1] = '\0';

				if (  trt__SetVideoSourceConfiguration->Configuration->Name ) {				
					strncpy(set_video_source_t.video_source_conf_in_t.Vname_t, 
						trt__SetVideoSourceConfiguration->Configuration->Name, MAX_PROF_TOKEN - 1);
				}
			
				set_video_source_t.video_source_conf_in_t.Vname_t[MAX_PROF_TOKEN - 1] = '\0';

				if ( trt__SetVideoSourceConfiguration->Configuration->SourceToken ) {				
					strncpy(set_video_source_t.video_source_conf_in_t.Vsourcetoken_t, 
						trt__SetVideoSourceConfiguration->Configuration->SourceToken, MAX_PROF_TOKEN - 1);
				}	
				set_video_source_t.video_source_conf_in_t.Vsourcetoken_t[MAX_PROF_TOKEN - 1] = '\0'; 
				
				set_video_source_t.video_source_conf_in_t.Vcount_t = trt__SetVideoSourceConfiguration->Configuration->UseCount;	
				if(trt__SetVideoSourceConfiguration->Configuration->Bounds != NULL)
				{
					set_video_source_t.video_source_conf_in_t.windowx_t = trt__SetVideoSourceConfiguration->Configuration->Bounds->x;
					set_video_source_t.video_source_conf_in_t.windowy_t = trt__SetVideoSourceConfiguration->Configuration->Bounds->y;
					set_video_source_t.video_source_conf_in_t.windowwidth_t = _width;
					set_video_source_t.video_source_conf_in_t.windowheight_t = _height;
				}				

				ret = ControlSystemData(SFIELD_SET_VIDEOSOURCE_CONF, (void *)&set_video_source_t, sizeof(set_video_source_t));
				ret = SUCCESS;
			}
		}
	}
	
	if (ret != SUCCESS) {	
		return (send_soap_fault_msg(soap, 400,
			"SOAP-ENV:Sender",
		         "ter:InvalidArgVal",
		         "ter:ConfigModify",
		         "set video source error"));	
	}


#if 0
	
	if(oSysInfo->Oprofile[i].AESC.Vencoder == 0)
	{
		codec_combo = 2;
		if((_width == 1600) && (_height == 1200)) // 2MP = 1600x1200
		{
			codec_res = 1;                        
		}
		else if((_width == 2048) && (_height == 1536)) // 3MP = 2048x1536
		{
			codec_res = 3;
		}
		else if(((_width == 2592) || (_height == 2560)) && (_height == 1920)) // 5MP = 2592x1920
		{
			codec_res = 5;
		}
		else 
		{
			codec_res = 1;		
		}
	}
	else if(oSysInfo->Oprofile[i].AESC.Vencoder == 1)
	{
		codec_combo = 1;
		if((_width == 1280) && (_height == 720)) // 720 = 1280x720
		{
			codec_res = 0;
		}
		else if((_width == 720) && (_height == 480)) // D1 = 720x480
		{
			codec_res = 1;
		}
		else if((_width == 1280) && (_height == 960)) // SXVGA = 1280x960
		{
			codec_res = 2;
		}
		else if((_width == 1920) && (_height == 1080)) // 1080 = 1920x1080
		{
			codec_res = 3;
		}
		else if((_width == 1280) && (_height == 720)) // 720MAX60 = 1280x720
		{
			codec_res = 4;
		}
		else codec_res = 0;
	}	
	else if(oSysInfo->Oprofile[i].AESC.Vencoder == 2)
	{
		codec_combo = 0;
		if((_width == 1280) && (_height == 720)) // 720 = 1280x720
		{
			codec_res = 0;
		}
		else if((_width == 720) && (_height == 480)) // D1 = 720x480
		{
			codec_res = 1;
		}
		else if((_width == 1280) && (_height == 960)) // SXVGA = 1280x960
		{
			codec_res = 2;
		}
		else if((_width == 1920) && (_height == 1080)) // 1080 = 1920x1080
		{
			codec_res = 3;
		}
		else if((_width == 1280) && (_height == 720)) // 720MAX60 = 1280x720
		{
			codec_res = 4;
		}
	}
	else
	{
		//onvif_fault(soap,"ter:InvalidArgVal", "ter:UnabletoSetParams");
		//return SOAP_FAULT;

		return (send_soap_fault_msg(soap, 400,
				"SOAP-ENV:Sender",
		         "ter:InvalidArgVal",
		         "ter:UnabletoSetParams",
		         "unable to set params"));	
	}

	ControlSystemData(SFIELD_SET_VIDEO_MODE, (void *)&mode, sizeof(mode));	
	if(oSysInfo->lan_config.nVideocombo != codec_combo)
	{
		ControlSystemData(SFIELD_SET_VIDEOCODECCOMBO, (void *)&codec_combo, sizeof(codec_combo));	
	}
	if(oSysInfo->lan_config.nVideocodecres != codec_res)
	{
		ControlSystemData(SFIELD_SET_VIDEOCODECRES, (void *)&codec_res, sizeof(codec_res));	
	}

#endif
#endif

	if(((trt__SetVideoSourceConfiguration->Configuration->Bounds->height) %2) != 0
		|| ((trt__SetVideoSourceConfiguration->Configuration->Bounds->width) %2) != 0)
	{
		goto __trt__SetAudioSourceConfiguration_Err_Hander;
	}

	soap->pConfig->videosource_width = trt__SetVideoSourceConfiguration->Configuration->Bounds->width;
	soap->pConfig->videosource_height = trt__SetVideoSourceConfiguration->Configuration->Bounds->height;
	//添加设置视频源配置
	return SOAP_OK;

	__trt__SetAudioSourceConfiguration_Err_Hander:

	return(send_soap_fault_msg(soap,400,
		"SOAP-ENV:Sender",
	         "ter:InvalidArgVal",
	         "ter:ConfigModify",
	         "The Set VideoSourceConfiguration error"));
	
	return SOAP_OK; 
}

static int video_to_resolution(int width, int height)
{
	int res = -1;
	
	if ((width == 352 && height == 288) || (width == 360 && height == 240))
	{
		res = 0;
	}
	else if ((width == 704 && height == 576) || (width == 720 && height == 480))
	{
		res = 1;
	}
	else if ((width == 704 && height == 288) || (width == 720 && height == 240))
	{
		res = 2;
	}
	else if ((width == 176 && height == 144) || (width == 180 && height == 120))
	{
		res = 3;
	}
	else if (width == 640 && height == 480)
	{
		res = 4;
	}
	else if (width == 320 && height == 240)
	{
		res = 5;
	}
	else if (width == 1280 && height == 720)
	{
		res = 6;
	}
	else if (width == 1280 && height == 1024)
	{
		res = 7;
	}
	else if (width == 2048 && height == 1536)
	{
		res = 8;
	}
	else if (width == 160 && height == 120)
	{
		res = 9;
	}
	else if (width == 1600 && height == 1200)
	{
		res = 10;
	}
	else if (width == 800 && height == 600)
	{
		res = 11;
	}
	else if (width == 1920 && height == 1080)
	{
		res = 12;
	}
	else if (width == 1024 && height == 768)
	{
		res = 13;
	
	}
	else if (width == 1280 && height == 960)
	{
		res = 14;
	}
	else if (width == 640 && height == 360)
	{
		res = 15;
	}

	return res;
}

int __trt__SetVideoEncoderConfiguration(struct soap *soap, struct _trt__SetVideoEncoderConfiguration *trt__SetVideoEncoderConfiguration, struct _trt__SetVideoEncoderConfigurationResponse *trt__SetVideoEncoderConfigurationResponse,
	int *bChanged, void *pStream_info)
{
	*bChanged = 0;
	
	struct tt__VideoEncoderConfiguration *pConfig = trt__SetVideoEncoderConfiguration->Configuration;
	if (!pConfig)
	{
		return SOAP_ERR;
	}

	int chn = 0, stream_index = 0;
	if (pConfig->token && !strncmp(pConfig->token, "VideoEncoderConfiguration", strlen("VideoEncoderConfiguration")))
	{
		chn = -1;
		sscanf(pConfig->token, "VideoEncoderConfiguration%d_%d", &chn, &stream_index);
	}
	else if (pConfig->Name && !strncmp(pConfig->Name, "VideoEncoderConfiguration", strlen("VideoEncoderConfiguration")))
	{
		chn = -1;
		sscanf(pConfig->Name, "VideoEncoderConfiguration%d_%d", &chn, &stream_index);
	}

	if ((stream_index >= MAX_STREAM_NUM) || (chn >= soap->pConfig->g_channel_num || chn < 0))
	{
		return SOAP_ERR;
	}
	
	QMAPI_NET_CHANNEL_PIC_INFO *pic_info = (QMAPI_NET_CHANNEL_PIC_INFO *)pStream_info;
	QMAPI_NET_COMPRESSION_INFO *stream_info = (stream_index == 0) ? &pic_info->stRecordPara : &pic_info->stNetPara;

	memset(pic_info, 0, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
	pic_info->dwSize = sizeof(QMAPI_NET_CHANNEL_PIC_INFO);
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, chn, pic_info, sizeof(QMAPI_NET_CHANNEL_PIC_INFO)) != 0)
	{
		printf("%s:%d set para failed!\n", __FUNCTION__, __LINE__);
	}

	QMAPI_NET_SUPPORT_STREAM_FMT support_fmt;
	if (get_support_fmt(&support_fmt))
	{
		printf("%s:%d get support message para failed!\n", __FUNCTION__, __LINE__);
		return SOAP_ERR;
	}
	
	if (pConfig->RateControl)
	{
		int max = 30;
		
		if (pConfig->RateControl->FrameRateLimit >= 1 && pConfig->RateControl->FrameRateLimit <= max)
		{
			stream_info->dwFrameRate = pConfig->RateControl->FrameRateLimit;
			*bChanged = 1;
		}

		if (pConfig->RateControl->BitrateLimit >= support_fmt.stVideoBitRate[stream_index].nMin / 1000 && pConfig->RateControl->BitrateLimit <= support_fmt.stVideoBitRate[stream_index].nMax / 1000)
		{
 			stream_info->dwBitRate = pConfig->RateControl->BitrateLimit * 1000;
			*bChanged = 1;
		}
	}

	int i = 0;
	if (pConfig->Resolution)
	{
		printf("111set width:%d height:%d\n", pConfig->Resolution->Width, pConfig->Resolution->Height);
	
		int res = video_to_resolution(pConfig->Resolution->Width, pConfig->Resolution->Height);
		if (res == -1)
		{
			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
				"ter:ConfigModify", "Parameters can not be set."); 	
		}
		
		for (i = 0; i < 10 && support_fmt.stVideoSize[stream_index][i].nWidth; i++)
		{
			if (res == video_to_resolution(support_fmt.stVideoSize[stream_index][i].nWidth, support_fmt.stVideoSize[stream_index][i].nHeight))
			{
				printf("set width:%d height:%d\n", support_fmt.stVideoSize[stream_index][i].nWidth, support_fmt.stVideoSize[stream_index][i].nHeight);
				stream_info->dwStreamFormat = res;
				*bChanged = 1;
				break;
			}
		}
	}
	else
	{
		return SOAP_OK;
	}

	if (pConfig->Encoding == tt__VideoEncoding__H264)
	{
		stream_info->dwCompressionType = QMAPI_PT_H264;
		*bChanged = 1;
		
		if (pConfig->H264)
		{
			if (pConfig->H264->GovLength >= 1 && pConfig->H264->GovLength <= 100)
			{
				stream_info->dwMaxKeyInterval = pConfig->H264->GovLength;
			}
			else
			{
    				return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal", "ter:ConfigModify",
        				"The configuration parameters are not possible to set.");			
			}

			if ( pConfig->H264->H264Profile != tt__H264Profile__Main)
			{
				for (i = 0; i < 4; i++)
				{
					if (support_fmt.dwVideoSupportFmt[stream_index][i] == QMAPI_PT_H264_HIGHPROFILE)
					{
						if (pConfig->H264->H264Profile == tt__H264Profile__High)
						{
							stream_info->dwCompressionType = QMAPI_PT_H264_HIGHPROFILE;
							break;
						}
					}
					else if (support_fmt.dwVideoSupportFmt[stream_index][i] == QMAPI_PT_H264_BASELINE)
					{
						if (pConfig->H264->H264Profile == tt__H264Profile__Baseline)
						{
							stream_info->dwCompressionType = QMAPI_PT_H264_BASELINE;
							break;
						}
					}
				}
			}
		}
	}
	else if (pConfig->Encoding == tt__VideoEncoding__JPEG)
	{
		stream_info->dwCompressionType = QMAPI_PT_MJPEG;
		*bChanged = 1;
	}
	else
	{
    		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal", "ter:ConfigModify",
        		"The configuration parameters are not possible to set.");
	}

	if (pConfig->Quality >= 1.0 && pConfig->Quality <= 5.0)
	{
		stream_info->dwImageQuality = 5 - (int)pConfig->Quality;
		*bChanged = 1;
	}

	return SOAP_OK;
}

int __trt__SetAudioSourceConfiguration(struct soap *soap, struct _trt__SetAudioSourceConfiguration *trt__SetAudioSourceConfiguration, struct _trt__SetAudioSourceConfigurationResponse *trt__SetAudioSourceConfigurationResponse)
{
	int chno = -1;

	do
	{
		printf("token is %s\n", trt__SetAudioSourceConfiguration->Configuration->token);
		if (trt__SetAudioSourceConfiguration->Configuration->token)
		{
			if (!strncmp(trt__SetAudioSourceConfiguration->Configuration->token, "AudioSourceConfiguration", strlen("AudioSourceConfiguration")))
			{
				chno = atoi(trt__SetAudioSourceConfiguration->Configuration->token + strlen("AudioSourceConfiguration"));
			}
			else
			{
				break;
			}
		}

		if (trt__SetAudioSourceConfiguration->Configuration->SourceToken)
		{
			chno = -1;
			if (!strncmp(trt__SetAudioSourceConfiguration->Configuration->SourceToken, "AudioSource", strlen("AudioSource")))
			{
				chno = atoi(trt__SetAudioSourceConfiguration->Configuration->SourceToken + strlen("AudioSource"));
			}
		}
	} while(0);
	
	if (chno == -1 || soap->pConfig->g_channel_num < (chno + 1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:ConfigModify",
			"The requested configuration does not exist. "));
	}
	
	return SOAP_OK;
	
}


int __trt__SetAudioEncoderConfiguration(struct soap *soap, struct _trt__SetAudioEncoderConfiguration *trt__SetAudioEncoderConfiguration, struct _trt__SetAudioEncoderConfigurationResponse *trt__SetAudioEncoderConfigurationResponse)
{
	int chno = 0;
	
	if (!trt__SetAudioEncoderConfiguration->Configuration->token)
	{
		if(0!=strncmp(trt__SetAudioEncoderConfiguration->Configuration->token,"AudioEncoderConfiguration", strlen("AudioEncoderConfiguration")))
		{
			return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:NoConfig",
				"The requested configuration does not exist. "));
		}

		chno = atoi(trt__SetAudioEncoderConfiguration->Configuration->token + strlen("AudioEncoderConfiguration"));
	}
	
	if (soap->pConfig->g_channel_num < (chno + 1))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));
	}

	if ((trt__SetAudioEncoderConfiguration->Configuration->Encoding != tt__AudioEncoding__G711 && trt__SetAudioEncoderConfiguration->Configuration->Encoding != tt__AudioEncoding__G726)
		 || (trt__SetAudioEncoderConfiguration->Configuration->Bitrate != 8)
		 || (trt__SetAudioEncoderConfiguration->Configuration->SampleRate != 16))
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:NoConfig",
			"The requested configuration does not exist. "));	
	}

	QMAPI_NET_CHANNEL_PIC_INFO info;
	memset(&info, 0, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
	info.dwSize = sizeof(QMAPI_NET_CHANNEL_PIC_INFO);
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, chno, &info, sizeof(QMAPI_NET_CHANNEL_PIC_INFO)) != 0)
	{
		printf("%s:%d set para failed!\n", __FUNCTION__, __LINE__);
	}

	DWORD pt = (trt__SetAudioEncoderConfiguration->Configuration->Encoding == tt__AudioEncoding__G726) ? QMAPI_PT_G726 : QMAPI_PT_G711A;
	if (info.stRecordPara.wFormatTag != pt)
	{
		info.stRecordPara.wFormatTag = pt;
		QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_PICCFG, chno, &info, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
		QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, chno, NULL, 0);
	}
	return SOAP_OK; 
}


int  __trt__SetVideoAnalyticsConfiguration(struct soap *soap, struct _trt__SetVideoAnalyticsConfiguration *trt__SetVideoAnalyticsConfiguration, struct _trt__SetVideoAnalyticsConfigurationResponse *trt__SetVideoAnalyticsConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __trt__SetMetadataConfiguration(struct soap *soap,
	struct _trt__SetMetadataConfiguration *trt__SetMetadataConfiguration,
         struct _trt__SetMetadataConfigurationResponse *trt__SetMetadataConfigurationResponse)
{
	ONVIF_DBP("");

	if (strcmp(trt__SetMetadataConfiguration->Configuration->SessionTimeout,"invalid") == 0)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
   			"ter:InvalidArgVal",
                     	"ter:ConfigModify",
   			"The ConfigurationToken error"));
	}

    return SOAP_OK;
}

int  __trt__SetAudioOutputConfiguration(struct soap *soap, struct _trt__SetAudioOutputConfiguration *trt__SetAudioOutputConfiguration, struct _trt__SetAudioOutputConfigurationResponse *trt__SetAudioOutputConfigurationResponse)
{
	if (!trt__SetAudioOutputConfiguration->Configuration || strcmp(trt__SetAudioOutputConfiguration->Configuration->token, "G711"))
	{
		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal", 
			"ter:ConfigurationToken", "The ConfigurationToken is error.");	
	}

	if (trt__SetAudioOutputConfiguration->Configuration->OutputLevel > 100 || trt__SetAudioOutputConfiguration->Configuration->OutputLevel < 0)
	{
		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal", 
			"ter:ConfigurationToken", "The ConfigurationToken is error.");			
	}

	soap->pConfig->outputLevel = trt__SetAudioOutputConfiguration->Configuration->OutputLevel;
	return SOAP_OK; 
}

int  __trt__SetAudioDecoderConfiguration(struct soap *soap, struct _trt__SetAudioDecoderConfiguration *trt__SetAudioDecoderConfiguration, struct _trt__SetAudioDecoderConfigurationResponse *trt__SetAudioDecoderConfigurationResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK;
}


int __trt__GetVideoSourceConfigurationOptions(struct soap *soap, struct _trt__GetVideoSourceConfigurationOptions *trt__GetVideoSourceConfigurationOptions, struct _trt__GetVideoSourceConfigurationOptionsResponse *trt__GetVideoSourceConfigurationOptionsResponse)
{
	ONVIF_DBP("");
/*
	if(trt__GetVideoSourceConfigurationOptions->ConfigurationToken==NULL && trt__GetVideoSourceConfigurationOptions->ProfileToken==NULL)
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                  "ter:InvalidArgVal",
	                  "ter:ConfigurationToken",
	                  "The ConfigurationToken is NULL. "));
*/
	int chno=0;
	//int stream_index=0;
	int ret;
	int stream_index_p, chn_p,chn_c;
	//int stream_index_c;
	if(trt__GetVideoSourceConfigurationOptions->ProfileToken)
	{
		if(!strncmp(trt__GetVideoSourceConfigurationOptions->ProfileToken, "profile", strlen("profile")))
		{
			ret = sscanf(trt__GetVideoSourceConfigurationOptions->ProfileToken, "profile%d_%d",&chn_p, &stream_index_p);
			if(ret != 2)
				return SOAP_ERR;
		}
		else if(!strncmp(trt__GetVideoSourceConfigurationOptions->ProfileToken, "axxon", strlen("axxon")))
		{
			stream_index_p = 0;
			chn_p = 0;
		}
		else 
			return SOAP_ERR;
	}

	if(trt__GetVideoSourceConfigurationOptions->ConfigurationToken)
	{
		if(!strncmp(trt__GetVideoSourceConfigurationOptions->ConfigurationToken, "VideoSourceConfiguration", strlen("VideoSourceConfiguration")))
		{
			ret = sscanf(trt__GetVideoSourceConfigurationOptions->ConfigurationToken, "VideoSourceConfiguration%d",&chn_c);
			if(ret != 1)
				return SOAP_ERR;
		}
		else 
			return SOAP_ERR;
	}

	if(trt__GetVideoSourceConfigurationOptions->ProfileToken && trt__GetVideoSourceConfigurationOptions->ConfigurationToken)
	{
		if(chn_c != chn_p)
		{
			printf("%s  configuration does not exist!\n",__FUNCTION__);
			return SOAP_ERR;
		}
		chno = chn_p;
		//stream_index = stream_index_p;
		
	}
	else if(trt__GetVideoSourceConfigurationOptions->ProfileToken)
	{
		chno = chn_p;
		//stream_index = stream_index_p;
	}
	else if(trt__GetVideoSourceConfigurationOptions->ConfigurationToken)
	{
		chno = chn_c;
		//stream_index = stream_index_c;
	}
	else
	{
		chno = 0;
		//stream_index= 0;
	}

	if(chno >= soap->pConfig->g_channel_num)
	{
		return SOAP_ERR;
	}

	/* 务必先置0,否则gsoap出现段错误或死循环 */
	trt__GetVideoSourceConfigurationOptionsResponse->Options = (struct tt__VideoSourceConfigurationOptions*)soap_malloc_v2(soap, sizeof(struct tt__VideoSourceConfigurationOptions));

	trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange = (struct tt__IntRectangleRange*)soap_malloc_v2(soap, sizeof(struct tt__IntRectangleRange));

	trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->HeightRange = (struct tt__IntRange*)soap_malloc_v2(soap, sizeof(struct tt__IntRange));
	trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->HeightRange->Max = 1080;

	trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->WidthRange = (struct tt__IntRange*)soap_malloc_v2(soap, sizeof(struct tt__IntRange));
	trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->WidthRange->Max = 1920;

	trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->XRange = (struct tt__IntRange*)soap_malloc_v2(soap, sizeof(struct tt__IntRange));

	trt__GetVideoSourceConfigurationOptionsResponse->Options->BoundsRange->YRange = (struct tt__IntRange*)soap_malloc_v2(soap, sizeof(struct tt__IntRange));

	trt__GetVideoSourceConfigurationOptionsResponse->Options->__sizeVideoSourceTokensAvailable = 1;

	trt__GetVideoSourceConfigurationOptionsResponse->Options->VideoSourceTokensAvailable = (char **)soap_malloc(soap, sizeof(char *));

	*(trt__GetVideoSourceConfigurationOptionsResponse->Options->VideoSourceTokensAvailable) = (char *)soap_malloc(soap, 16);
	sprintf(*(trt__GetVideoSourceConfigurationOptionsResponse->Options->VideoSourceTokensAvailable),"VideoSource%d",chno);
	
	return SOAP_OK; 
}


int __trt__GetVideoEncoderConfigurationOptions(struct soap *soap, struct _trt__GetVideoEncoderConfigurationOptions *trt__GetVideoEncoderConfigurationOptions, struct _trt__GetVideoEncoderConfigurationOptionsResponse *trt__GetVideoEncoderConfigurationOptionsResponse)
{
	ONVIF_DBP("");

	int chno = 0;
	//int profileindex = 0;
	int stream_index=0;
/*
	if(!trt__GetVideoEncoderConfigurationOptions->ConfigurationToken&&!trt__GetVideoEncoderConfigurationOptions->ProfileToken)
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                  "ter:InvalidArgVal",
	                  "ter:ConfigurationToken",
	                  "The ConfigurationToken is NULL. "));
*/
	int ret;
	int stream_index_p, stream_index_c, chn_p,chn_c;
	if(trt__GetVideoEncoderConfigurationOptions->ProfileToken)
	{
		if(!strncmp(trt__GetVideoEncoderConfigurationOptions->ProfileToken, "profile", strlen("profile")))
		{
			ret = sscanf(trt__GetVideoEncoderConfigurationOptions->ProfileToken, "profile%d_%d",&chn_p, &stream_index_p);
			if(ret != 2)
				return SOAP_ERR;
		}
		else if(!strncmp(trt__GetVideoEncoderConfigurationOptions->ProfileToken, "axxon", strlen("axxon")))
		{
			stream_index_p = 0;
			chn_p = 0;
		}
		else 
			return SOAP_ERR;
	}

	if(trt__GetVideoEncoderConfigurationOptions->ConfigurationToken)
	{
		if(!strncmp(trt__GetVideoEncoderConfigurationOptions->ConfigurationToken, "VideoEncoderConfiguration", strlen("VideoEncoderConfiguration")))
		{
			ret = sscanf(trt__GetVideoEncoderConfigurationOptions->ConfigurationToken, "VideoEncoderConfiguration%d_%d",&chn_c, &stream_index_c);
			if(ret != 2)
				return SOAP_ERR;
		}
		else 
			return SOAP_ERR;
	}

	if(trt__GetVideoEncoderConfigurationOptions->ProfileToken && trt__GetVideoEncoderConfigurationOptions->ConfigurationToken)
	{
		if(chn_c != chn_p ||stream_index_p!= stream_index_c)
		{
			printf("%s  configuration does not exist!\n",__FUNCTION__);
			return SOAP_ERR;
		}
		chno = chn_p;
		stream_index = stream_index_p;
		
	}
	else if(trt__GetVideoEncoderConfigurationOptions->ProfileToken)
	{
		chno = chn_p;
		stream_index = stream_index_p;
	}
	else if(trt__GetVideoEncoderConfigurationOptions->ConfigurationToken)
	{
		chno = chn_c;
		stream_index = stream_index_c;
	}
	else
	{
		chno = 0;
		stream_index = 0;
	}

	if(stream_index >= MAX_STREAM_NUM || chno >= soap->pConfig->g_channel_num)
	{
		return SOAP_ERR;
	}

	QMAPI_NET_SUPPORT_STREAM_FMT support_fmt;
	if(get_support_fmt(&support_fmt))
	{
		return SOAP_ERR;
	}
	
	trt__GetVideoEncoderConfigurationOptionsResponse->Options = (struct tt__VideoEncoderConfigurationOptions *)soap_malloc_v2(soap,sizeof(struct tt__VideoEncoderConfigurationOptions));

	trt__GetVideoEncoderConfigurationOptionsResponse->Options->QualityRange = (struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->QualityRange->Min = 1;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->QualityRange->Max = 5;

	trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension = (struct tt__VideoEncoderOptionsExtension *)soap_malloc_v2(soap, sizeof(struct tt__VideoEncoderOptionsExtension));

	QMAPI_NET_DEVICE_INFO stDeviceInfo;
	memset(&stDeviceInfo, 0, sizeof(QMAPI_NET_DEVICE_INFO));
	
	QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, &stDeviceInfo, sizeof(QMAPI_NET_DEVICE_INFO));

	int i;
	int ResolutionNum = 0;
	for(i=0;i<10;i++)
	{
		if(support_fmt.stVideoSize[stream_index][i].nWidth>0)
			ResolutionNum++;
		else if(support_fmt.stVideoSize[stream_index][i].nWidth == 0)
			break;
	}

	int h264_profile[3] = {0};
	int isSupportJPEG = 0;
	for (i = 0; i < 4; i++)
	{
		switch(support_fmt.dwVideoSupportFmt[stream_index][i])
		{
			case QMAPI_PT_H264_HIGHPROFILE:
			case QMAPI_PT_H265:
				h264_profile[0] = 1;
				break;
			case QMAPI_PT_H264_BASELINE:
				h264_profile[1] = 1;
				break;				
			case QMAPI_PT_H264_MAINPROFILE:
			case QMAPI_PT_H264:
				h264_profile[2] = 1;		
				break;
			case QMAPI_PT_JPEG:
			case QMAPI_PT_MJPEG:
				isSupportJPEG = 1;
				break;
			default:
				break;
		}
	}

	if (h264_profile[0] || h264_profile[1] || h264_profile[2])
	{
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264 = (struct tt__H264Options *)soap_malloc_v2(soap,sizeof(struct tt__H264Options));
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->__sizeResolutionsAvailable = ResolutionNum;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable = (struct tt__VideoResolution *)soap_malloc_v2(soap,sizeof(struct tt__VideoResolution)*ResolutionNum);
		for (i = 0; i < ResolutionNum; i++)
		{
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable[i].Width = support_fmt.stVideoSize[stream_index][i].nWidth;
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable[i].Height = support_fmt.stVideoSize[stream_index][i].nHeight;
		}
					
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->GovLengthRange = (struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->GovLengthRange->Min = 1;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->GovLengthRange->Max = 100;

		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->FrameRateRange = (struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->FrameRateRange->Min = 1;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->FrameRateRange->Max = 30;

		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->EncodingIntervalRange = (struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->EncodingIntervalRange->Min = 1;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->EncodingIntervalRange->Max = 1;

		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->__sizeH264ProfilesSupported = h264_profile[0] + h264_profile[1] + h264_profile[2];
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->H264ProfilesSupported = (enum tt__H264Profile *)soap_malloc(soap,sizeof(enum tt__H264Profile) * trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->__sizeH264ProfilesSupported);

		i = 0;
		if (h264_profile[0])
		{
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->H264ProfilesSupported[i++] = tt__H264Profile__High;
		}
		
		if (h264_profile[1])
		{
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->H264ProfilesSupported[i++] = tt__H264Profile__Baseline;
		}

		if (h264_profile[2])
		{
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->H264ProfilesSupported[i++] = tt__H264Profile__Main;
		}

		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264 = (struct tt__H264Options2 *)soap_malloc_v2(soap, sizeof(struct tt__H264Options2));	
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->__sizeResolutionsAvailable = ResolutionNum;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->ResolutionsAvailable = trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable;

		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->GovLengthRange = trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->GovLengthRange;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->FrameRateRange = trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->FrameRateRange;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->EncodingIntervalRange = trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->EncodingIntervalRange;

		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->__sizeH264ProfilesSupported = trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->__sizeH264ProfilesSupported;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->H264ProfilesSupported = trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->H264ProfilesSupported;

		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->BitrateRange = (struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->BitrateRange->Min = support_fmt.stVideoBitRate[stream_index].nMin / 1000;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->H264->BitrateRange->Max = support_fmt.stVideoBitRate[stream_index].nMax / 1000;
	}

	if (isSupportJPEG)
	{				
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG = (struct tt__JpegOptions *)soap_malloc_v2(soap,sizeof(struct tt__JpegOptions));

		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->__sizeResolutionsAvailable = ResolutionNum;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable = (struct tt__VideoResolution *)soap_malloc_v2(soap,sizeof(struct tt__VideoResolution)*ResolutionNum);
		for (i = 0; i < ResolutionNum; i++)
		{
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable[i].Width = support_fmt.stVideoSize[stream_index][i].nWidth;
			trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable[i].Height = support_fmt.stVideoSize[stream_index][i].nHeight;
		}
		
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->FrameRateRange = (struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->FrameRateRange->Min = 1;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->FrameRateRange->Max = 30;

		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->EncodingIntervalRange = (struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->EncodingIntervalRange->Min = 1;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->EncodingIntervalRange->Max = 1;

		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->JPEG = (struct tt__JpegOptions2 *)soap_malloc_v2(soap, sizeof(struct tt__JpegOptions2));
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->JPEG->__sizeResolutionsAvailable = ResolutionNum;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->JPEG->ResolutionsAvailable = trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable;
		
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->JPEG->FrameRateRange = trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->FrameRateRange;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->JPEG->EncodingIntervalRange = trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->EncodingIntervalRange;

		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->JPEG->BitrateRange = (struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->JPEG->BitrateRange->Min = support_fmt.stVideoBitRate[stream_index].nMin / 1000;
		trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension->JPEG->BitrateRange->Max = support_fmt.stVideoBitRate[stream_index].nMax / 1000;
	}
	
	return SOAP_OK; 
}


int __trt__GetAudioSourceConfigurationOptions(struct soap *soap, struct _trt__GetAudioSourceConfigurationOptions *trt__GetAudioSourceConfigurationOptions, struct _trt__GetAudioSourceConfigurationOptionsResponse *trt__GetAudioSourceConfigurationOptionsResponse)
{
	ONVIF_DBP("");

	int ret;
	int stream_index_p, chn_p,chn_c;
	//int stream_index_c;
	int chno = 0;
	//int stream_index;
/*
	if(!trt__GetAudioSourceConfigurationOptions->ConfigurationToken && !trt__GetAudioSourceConfigurationOptions->ProfileToken)
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                  "ter:InvalidArgVal",
	                  "ter:ConfigurationToken",
	                  "The ConfigurationToken is NULL. "));
*/	
	if(trt__GetAudioSourceConfigurationOptions->ProfileToken)
	{
		if(!strncmp(trt__GetAudioSourceConfigurationOptions->ProfileToken, "profile", strlen("profile")))
		{
			ret = sscanf(trt__GetAudioSourceConfigurationOptions->ProfileToken, "profile%d_%d",&chn_p, &stream_index_p);
			if(ret != 2)
				return SOAP_ERR;
		}
		else if(!strncmp(trt__GetAudioSourceConfigurationOptions->ProfileToken, "axxon", strlen("axxon")))
		{
			stream_index_p = 0;
			chn_p = 0;
		}
		else 
			return SOAP_ERR;
	}

	if(trt__GetAudioSourceConfigurationOptions->ConfigurationToken)
	{
		if(!strncmp(trt__GetAudioSourceConfigurationOptions->ConfigurationToken, "AudioSourceConfiguration", strlen("AudioSourceConfiguration")))
		{
			ret = sscanf(trt__GetAudioSourceConfigurationOptions->ConfigurationToken, "AudioSourceConfiguration%d",&chn_c);
			if(ret != 1)
				return SOAP_ERR;
		}
		else 
			return SOAP_ERR;
	}

	if(trt__GetAudioSourceConfigurationOptions->ProfileToken && trt__GetAudioSourceConfigurationOptions->ConfigurationToken)
	{
		if(chn_c != chn_p)
		{
			printf("%s  configuration does not exist!\n",__FUNCTION__);
			return SOAP_ERR;
		}
		chno = chn_p;
		//stream_index = stream_index_p;
		
	}
	else if(trt__GetAudioSourceConfigurationOptions->ProfileToken)
	{
		chno = chn_p;
		//stream_index = stream_index_p;
	}
	else if(trt__GetAudioSourceConfigurationOptions->ConfigurationToken)
	{
		chno = chn_c;
		//stream_index = stream_index_c;
	}
	else
	{
		chno = 0;
		//stream_index = 0;
	}
	
	/* 务必先置0,否则gsoap出现段错误或死循环 */
	trt__GetAudioSourceConfigurationOptionsResponse->Options = (struct tt__AudioSourceConfigurationOptions*)soap_malloc_v2(soap, sizeof(struct tt__AudioSourceConfigurationOptions));

	trt__GetAudioSourceConfigurationOptionsResponse->Options->__sizeInputTokensAvailable = 1;
	trt__GetAudioSourceConfigurationOptionsResponse->Options->InputTokensAvailable =  (char **)soap_malloc(soap, sizeof(char *));

	trt__GetAudioSourceConfigurationOptionsResponse->Options->InputTokensAvailable[0] = (char *)soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
	sprintf(trt__GetAudioSourceConfigurationOptionsResponse->Options->InputTokensAvailable[0], "AudioSource%d", chno);

	//sprintf(*trt__GetAudioSourceConfigurationOptionsResponse->Options->InputTokensAvailable,"AudioSource%d",chno);	
	
	return SOAP_OK; 
}


int  __trt__GetAudioEncoderConfigurationOptions(struct soap *soap,
	struct _trt__GetAudioEncoderConfigurationOptions *trt__GetAudioEncoderConfigurationOptions,
         struct _trt__GetAudioEncoderConfigurationOptionsResponse *trt__GetAudioEncoderConfigurationOptionsResponse)
{
	ONVIF_DBP("");	

	int ret;
	int stream_index_p, chn_p,chn_c;
	//int stream_index_c;
	//int chno = 0;
	//int stream_index;
/*
	if(!trt__GetAudioEncoderConfigurationOptions->ConfigurationToken && !trt__GetAudioEncoderConfigurationOptions->ProfileToken)
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                  "ter:InvalidArgVal",
	                  "ter:ConfigurationToken",
	                  "The ConfigurationToken is NULL. "));
*/
	if(trt__GetAudioEncoderConfigurationOptions->ProfileToken)
	{
		if(!strncmp(trt__GetAudioEncoderConfigurationOptions->ProfileToken, "profile", strlen("profile")))
		{
			ret = sscanf(trt__GetAudioEncoderConfigurationOptions->ProfileToken, "profile%d_%d",&chn_p, &stream_index_p);
			if(ret != 2)
				return SOAP_ERR;
		}
		else if(!strncmp(trt__GetAudioEncoderConfigurationOptions->ProfileToken, "axxon", strlen("axxon")))
		{
			stream_index_p = 0;
			chn_p = 0;
		}
		else if (!strcmp(trt__GetAudioEncoderConfigurationOptions->ProfileToken, soap->pConfig->profile_cfg.token))
		{
			if (!soap->pConfig->profile_cfg.isVaild)
			{
				return(send_soap_fault_msg(soap,400,
					"SOAP-ENV:Receiver",
					"ter:Action",
					"ter:ProfileToken",
					"The ProfileToken is not existing"));
			}
			else
			{
				sscanf(soap->pConfig->profile_cfg.audioEncoderToken, "AudioEncoderConfiguration%d", &chn_p);
				stream_index_p = 0;
			}
		}		
		else 
			return SOAP_ERR;
	}

	if(trt__GetAudioEncoderConfigurationOptions->ConfigurationToken)
	{
		if(!strncmp(trt__GetAudioEncoderConfigurationOptions->ConfigurationToken, "AudioEncoderConfiguration", strlen("AudioEncoderConfiguration")))
		{
			ret = sscanf(trt__GetAudioEncoderConfigurationOptions->ConfigurationToken, "AudioEncoderConfiguration%d",&chn_c);
			if(ret != 1)
				return SOAP_ERR;
		}
		else 
			return SOAP_ERR;
	}

	if(trt__GetAudioEncoderConfigurationOptions->ProfileToken && trt__GetAudioEncoderConfigurationOptions->ConfigurationToken)
	{
		if(chn_c != chn_p)
		{
			printf("%s  configuration does not exist!\n",__FUNCTION__);
			return SOAP_ERR;
		}
		//chno = chn_p;
		//stream_index = stream_index_p;
		
	}
	else if(trt__GetAudioEncoderConfigurationOptions->ProfileToken)
	{
		//chno = chn_p;
		//stream_index = stream_index_p;
	}
	else if(trt__GetAudioEncoderConfigurationOptions->ConfigurationToken)
	{
		//chno = chn_c;
		//stream_index = stream_index_c;
	}
	else
	{
		//chno = 0;
		//stream_index = 0;
	}
	
	trt__GetAudioEncoderConfigurationOptionsResponse->Options = (struct tt__AudioEncoderConfigurationOptions *)soap_malloc_v2(soap, sizeof(struct tt__AudioEncoderConfigurationOptions));

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->__sizeOptions = 2;

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options = (struct tt__AudioEncoderConfigurationOption *)soap_malloc_v2(soap, sizeof(struct tt__AudioEncoderConfigurationOption) * 2);

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].Encoding = tt__AudioEncoding__G711;

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].BitrateList = (struct tt__IntList *)soap_malloc_v2(soap, sizeof(struct tt__IntList));
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].BitrateList->__sizeItems = 1;

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].BitrateList->Items = (int *)soap_malloc(soap, sizeof(int));
	*trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].BitrateList->Items = 8;

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].SampleRateList = (struct tt__IntList *)soap_malloc_v2(soap, sizeof(struct tt__IntList));
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].SampleRateList->__sizeItems = 1;

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].SampleRateList->Items = (int *)soap_malloc(soap, sizeof(int));
	*trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[0].SampleRateList->Items = 16;

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[1].Encoding = tt__AudioEncoding__G726;

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[1].BitrateList = (struct tt__IntList *)soap_malloc_v2(soap, sizeof(struct tt__IntList));
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[1].BitrateList->__sizeItems = 1;

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[1].BitrateList->Items = (int *)soap_malloc(soap, sizeof(int));
	*trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[1].BitrateList->Items = 8;

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[1].SampleRateList = (struct tt__IntList *)soap_malloc_v2(soap, sizeof(struct tt__IntList));
	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[1].SampleRateList->__sizeItems = 1;

	trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[1].SampleRateList->Items = (int *)soap_malloc(soap, sizeof(int));
	*trt__GetAudioEncoderConfigurationOptionsResponse->Options->Options[1].SampleRateList->Items = 16;	
	return SOAP_OK;
}

int __trt__GetMetadataConfigurationOptions(struct soap *soap, struct _trt__GetMetadataConfigurationOptions *trt__GetMetadataConfigurationOptions, struct _trt__GetMetadataConfigurationOptionsResponse *trt__GetMetadataConfigurationOptionsResponse)
{
 	ONVIF_DBP("");	
 #if 0
	 SysInfo *oSysInfo = GetSysInfo();

	 if ( !oSysInfo ) {
		return(send_soap_fault_msg(soap, 400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:OutOfMemory",
			"get system config failed"));
	}

	 
	 int i;
	 int num_token = 0;
	 int j;
	 int flag = 0;
	 int flg = 0;
	 int index = 0;
	 int num = 0;
	 for(i = 0; i < oSysInfo->nprofile; i++)
	 {
		 for(j = 0; j <= i; j++)
		 {
			 if(strcmp(oSysInfo->Oprofile[j].AVSC.Vtoken, oSysInfo->Oprofile[i].AVSC.Vtoken)==0);
			 {
				 flg = 1;		 
				 break;
			 }
		 }
		 if(flg == 0)
		 {
			 num_token++;
		 }
	 }
	 if(trt__GetMetadataConfigurationOptions->ConfigurationToken != NULL)
	 {
		 for(i = 0; i <= oSysInfo->nprofile ; i++)
		 {
			 if(strcmp(trt__GetMetadataConfigurationOptions->ConfigurationToken, oSysInfo->Oprofile[i].AVSC.Vtoken)==0);
			 {
				 flag = 1;
				 index = j;
				 break;
			 }
		 }
	 }
 
	 if(trt__GetMetadataConfigurationOptions->ProfileToken != NULL)
	 {
		 for(i = 0; i <= oSysInfo->nprofile ; i++)
		 {
			 if(strcmp(trt__GetMetadataConfigurationOptions->ProfileToken, oSysInfo->Oprofile[i].profiletoken)==0);
			 {
				 flag = 1;
				 index = j;
				 break;
			 }
		 }
	 }
 
	 if(!flag)
	 {
		 return(send_soap_fault_msg(soap,400,
				"SOAP-ENV:Sender",
				"ter:NoConfig",
				"ter:InvalidArgValue",
				"no config"));		
	 }
	
	 trt__GetMetadataConfigurationOptionsResponse->Options = (struct tt__MetadataConfigurationOptions *)soap_malloc_v2(soap, sizeof(struct tt__MetadataConfigurationOptions));
	 memset(trt__GetMetadataConfigurationOptionsResponse->Options, 0, sizeof(struct tt__MetadataConfigurationOptions));		
	 
	 trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions = (struct tt__PTZStatusFilterOptions *)soap_malloc_v2(soap, sizeof(struct tt__PTZStatusFilterOptions));
	 memset(trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions, 0, sizeof(struct tt__PTZStatusFilterOptions));
	 
	 trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->PanTiltStatusSupported = 0;
	 trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->ZoomStatusSupported = 0;

	 trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->PanTiltPositionSupported = (int *)soap_malloc(soap, sizeof(int));
	 trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->PanTiltPositionSupported[0] = 0;
	 trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->ZoomPositionSupported = (int *)soap_malloc(soap, sizeof(int));
	 trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->ZoomPositionSupported[0] = 0;
	
	 trt__GetMetadataConfigurationOptionsResponse->Options->PTZStatusFilterOptions->Extension = NULL;
#endif	
 
	 return SOAP_OK;
}

int  __trt__GetAudioOutputConfigurationOptions(struct soap *soap, struct _trt__GetAudioOutputConfigurationOptions *trt__GetAudioOutputConfigurationOptions, struct _trt__GetAudioOutputConfigurationOptionsResponse *trt__GetAudioOutputConfigurationOptionsResponse)
{
	if (!trt__GetAudioOutputConfigurationOptions->ConfigurationToken && !trt__GetAudioOutputConfigurationOptions->ProfileToken)
	{
		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
	    	"ter:ConfigurationToken", "The ConfigurationToken is NULL.");		
	}
	
	if ((trt__GetAudioOutputConfigurationOptions->ConfigurationToken && strcmp(trt__GetAudioOutputConfigurationOptions->ConfigurationToken, "G711"))
		|| (trt__GetAudioOutputConfigurationOptions->ProfileToken && strcmp(trt__GetAudioOutputConfigurationOptions->ProfileToken, "G711")))
	{
		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
	    	"ter:ConfigurationToken",	"The ConfigurationToken is NULL.");			
	}

	trt__GetAudioOutputConfigurationOptionsResponse->Options = (struct tt__AudioOutputConfigurationOptions *)soap_malloc_v2(soap, sizeof(struct tt__AudioOutputConfigurationOptions));
	trt__GetAudioOutputConfigurationOptionsResponse->Options->OutputLevelRange = (struct tt__IntRange *)soap_malloc_v2(soap, sizeof(struct tt__IntRange));
	trt__GetAudioOutputConfigurationOptionsResponse->Options->OutputLevelRange->Min = 0;
	trt__GetAudioOutputConfigurationOptionsResponse->Options->OutputLevelRange->Max = 100;
	trt__GetAudioOutputConfigurationOptionsResponse->Options->__sizeOutputTokensAvailable = 1;
	trt__GetAudioOutputConfigurationOptionsResponse->Options->OutputTokensAvailable = (char **)soap_malloc(soap, sizeof(char *));
	*trt__GetAudioOutputConfigurationOptionsResponse->Options->OutputTokensAvailable = soap_strdup(soap, "G711");
	trt__GetAudioOutputConfigurationOptionsResponse->Options->__sizeSendPrimacyOptions = 1;
	trt__GetAudioOutputConfigurationOptionsResponse->Options->SendPrimacyOptions = (char **)soap_malloc(soap, sizeof(char *));
	*trt__GetAudioOutputConfigurationOptionsResponse->Options->SendPrimacyOptions = soap_strdup(soap, "nothing");
	return SOAP_OK; 
}

int  __trt__GetAudioDecoderConfigurationOptions(struct soap *soap, struct _trt__GetAudioDecoderConfigurationOptions *trt__GetAudioDecoderConfigurationOptions, struct _trt__GetAudioDecoderConfigurationOptionsResponse *trt__GetAudioDecoderConfigurationOptionsResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __trt__GetGuaranteedNumberOfVideoEncoderInstances(struct soap *soap, 
 	struct _trt__GetGuaranteedNumberOfVideoEncoderInstances *trt__GetGuaranteedNumberOfVideoEncoderInstances, 
 	struct _trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse *trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse)
{ 
	trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse->TotalNumber = 2;
 	return SOAP_OK; 
}


#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <net/if.h> 
#include <netinet/in.h> 
#include <net/if_arp.h> 

int __trt__GetStreamUri(struct soap *soap, struct _trt__GetStreamUri *trt__GetStreamUri, struct _trt__GetStreamUriResponse *trt__GetStreamUriResponse)
{	
	int ret, stream_index,chn;

	if(!trt__GetStreamUri->ProfileToken ||!trt__GetStreamUri->StreamSetup)
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                  "ter:InvalidArgVal",
	                  "ter:ConfigurationToken",
	                  "The ConfigurationToken is NULL. "));

	if(!strncmp(trt__GetStreamUri->ProfileToken, "profile", strlen("profile")))
	{
		ret = sscanf(trt__GetStreamUri->ProfileToken, "profile%d_%d",&chn, &stream_index);
		if(ret != 2)
			return SOAP_ERR;
	}
	else if(!strncmp(trt__GetStreamUri->ProfileToken, "axxon", strlen("axxon")))
	{
		stream_index = 0;
		chn = 0;
	}
	else if (soap->pConfig->profile_cfg.isVaild && !strcmp(trt__GetStreamUri->ProfileToken, soap->pConfig->profile_cfg.token))
	{
		stream_index = 0;
		chn = 0;	
	}
	else 
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                	"ter:InvalidArgVal",
	                	"ter:NoProfile",
	                	"The media profile does not exist. "));

	if(stream_index >= MAX_STREAM_NUM || chn >= soap->pConfig->g_channel_num)
	{
		return SOAP_ERR;
	}

	//int webport=0;	
	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));

	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);

	//webport = net_conf.wHttpPort;

	trt__GetStreamUriResponse->MediaUri  =  (struct tt__MediaUri*)soap_malloc_v2(soap, sizeof(struct tt__MediaUri));

	trt__GetStreamUriResponse->MediaUri->Uri = (char *)soap_malloc(soap, 64);

	if(TRUE == isWIFI)
	{
		if(tt__TransportProtocol__HTTP == trt__GetStreamUri->StreamSetup->Transport->Protocol)
		    sprintf(trt__GetStreamUriResponse->MediaUri->Uri,"rtsp://%s/ROH/channel/%d%d", wifi_cfg.dwNetIpAddr.csIpV4, chn+1, stream_index+1);
		else
		{
			if(tt__StreamType__RTP_Unicast == trt__GetStreamUri->StreamSetup->Stream)
				sprintf(trt__GetStreamUriResponse->MediaUri->Uri,"rtsp://%s:%d/ucast/%d%d", wifi_cfg.dwNetIpAddr.csIpV4, get_rtsp_port(), chn+1,stream_index+1);
			else
				sprintf(trt__GetStreamUriResponse->MediaUri->Uri,"rtsp://%s:%d/mcast/%d%d", wifi_cfg.dwNetIpAddr.csIpV4, get_rtsp_port(), chn+1,stream_index+1);
		}
	}
	else
	{
		if(tt__TransportProtocol__HTTP == trt__GetStreamUri->StreamSetup->Transport->Protocol)
		    sprintf(trt__GetStreamUriResponse->MediaUri->Uri,"rtsp://%s/ROH/channel/%d%d",net_conf.stEtherNet[0].strIPAddr.csIpV4, chn+1, stream_index+1);
		else
		{
			if(tt__StreamType__RTP_Unicast == trt__GetStreamUri->StreamSetup->Stream)
				sprintf(trt__GetStreamUriResponse->MediaUri->Uri,"rtsp://%s:%d/ucast/%d%d",net_conf.stEtherNet[0].strIPAddr.csIpV4, get_rtsp_port(), chn+1,stream_index+1);
				//sprintf(trt__GetStreamUriResponse->MediaUri->Uri,"rtsp://%s:%d/ucast/%d%d","113.97.180.188", 554, chn+1,stream_index+1);
			else
				sprintf(trt__GetStreamUriResponse->MediaUri->Uri,"rtsp://%s:%d/mcast/%d%d",net_conf.stEtherNet[0].strIPAddr.csIpV4, get_rtsp_port(), chn+1,stream_index+1);
		}
	}

	trt__GetStreamUriResponse->MediaUri->InvalidAfterConnect = xsd__boolean__false_;
	trt__GetStreamUriResponse->MediaUri->InvalidAfterReboot = xsd__boolean__false_;

	trt__GetStreamUriResponse->MediaUri->Timeout = (char *)soap_malloc(soap, sizeof(8));
	strcpy(trt__GetStreamUriResponse->MediaUri->Timeout, "PT60S");
	
	return SOAP_OK; 	
}


int  __trt__StartMulticastStreaming(struct soap *soap, struct _trt__StartMulticastStreaming *trt__StartMulticastStreaming, struct _trt__StartMulticastStreamingResponse *trt__StartMulticastStreamingResponse)
{ 
	return send_soap_fault_msg(soap, 400, "SOAP-ENV:Receiver", "ter:Action",
		"ter:NoProfile", "Not supported.");
}

int  __trt__StopMulticastStreaming(struct soap *soap, struct _trt__StopMulticastStreaming *trt__StopMulticastStreaming, struct _trt__StopMulticastStreamingResponse *trt__StopMulticastStreamingResponse)
{ 
	ONVIF_DBP("");
	return SOAP_OK; 
}

int  __trt__SetSynchronizationPoint(struct soap *soap, 
 	struct _trt__SetSynchronizationPoint *trt__SetSynchronizationPoint, 
 	struct _trt__SetSynchronizationPointResponse *trt__SetSynchronizationPointResponse)
{ 
	ONVIF_DBP("");

	int ret;

	if(NULL == trt__SetSynchronizationPoint->ProfileToken)
	{
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Receiver",
			"ter:Action",
			"ter:ProfileToken",
			"The ProfileToken is NULL"));
	}

	int stream_index,chn;
	if(!strncmp(trt__SetSynchronizationPoint->ProfileToken, "profile", strlen("profile")))
	{
		ret = sscanf(trt__SetSynchronizationPoint->ProfileToken, "profile%d_%d",&chn, &stream_index);
		if(ret != 2)
			return SOAP_ERR;
	}
	else if(!strncmp(trt__SetSynchronizationPoint->ProfileToken, "axxon", strlen("axxon")))
	{
		stream_index = 0;
		chn = 0;
	}
	else 
	{
		stream_index = 0;
		chn = 0;
	}

	if(stream_index >= MAX_STREAM_NUM || chn >= soap->pConfig->g_channel_num)
	{
		return SOAP_ERR;
	}

	ret = QMapi_sys_ioctrl(0, QMAPI_NET_STA_IFRAME_REQUEST, chn, &stream_index, sizeof(int));
	if(ret)
	{
		return SOAP_ERR;
	}

	return SOAP_OK; 
}

int __trt__GetSnapshotUri(struct soap *soap, struct _trt__GetSnapshotUri *trt__GetSnapshotUri, struct _trt__GetSnapshotUriResponse *trt__GetSnapshotUriResponse)
{
	ONVIF_DBP("");

	int     chno;
	int ret, stream_index;
	if(!trt__GetSnapshotUri->ProfileToken)
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                  "ter:InvalidArgVal",
	                  "ter:ProfileToken",
	                  "The ProfileToken is NULL. "));
	
	if(!strncmp(trt__GetSnapshotUri->ProfileToken, "profile", strlen("profile")))
	{
		ret = sscanf(trt__GetSnapshotUri->ProfileToken, "profile%d_%d",&chno, &stream_index);
		if(ret != 2)
			return SOAP_ERR;
	}
	else if(!strncmp(trt__GetSnapshotUri->ProfileToken, "axxon", strlen("axxon")))
	{
		stream_index = 0;
		chno = 0;
	}
	else 
		return(send_soap_fault_msg(soap,400,
			"SOAP-ENV:Sender",
	                	"ter:InvalidArgVal",
	                	"ter:NoProfile",
	                	"The media profile does not exist. "));

	int webport=0;
	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));

	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);
	webport = net_conf.wHttpPort;
	
	QMAPI_NET_USER_INFO user_info;
	memset(&user_info,0,sizeof(QMAPI_NET_USER_INFO));
	ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_USERCFG,0,&user_info,sizeof(QMAPI_NET_USER_INFO));
	if(ret)
	{
		return SOAP_ERR;
	}
	
	trt__GetSnapshotUriResponse->MediaUri  =  (struct tt__MediaUri*)soap_malloc_v2(soap, sizeof(struct tt__MediaUri));

	trt__GetSnapshotUriResponse->MediaUri->Uri = (char *)soap_malloc(soap, LEN_256_SIZE);

	if(TRUE == isWIFI)
		sprintf(trt__GetSnapshotUriResponse->MediaUri->Uri, "http://%s:%d/cgi-bin/anv/images_cgi?channel=%d&user=%s&pwd=%s",
								wifi_cfg.dwNetIpAddr.csIpV4, webport, chno, user_info.csUserName, user_info.csPassword);
	else
		sprintf(trt__GetSnapshotUriResponse->MediaUri->Uri, "http://%s:%d/cgi-bin/anv/images_cgi?channel=%d&user=%s&pwd=%s",
								net_conf.stEtherNet[0].strIPAddr.csIpV4, webport, chno, user_info.csUserName, user_info.csPassword);

	trt__GetSnapshotUriResponse->MediaUri->InvalidAfterConnect = xsd__boolean__false_;
	trt__GetSnapshotUriResponse->MediaUri->InvalidAfterReboot = xsd__boolean__false_;

	trt__GetSnapshotUriResponse->MediaUri->Timeout = (char *)soap_malloc(soap, sizeof(8));
	strcpy(trt__GetSnapshotUriResponse->MediaUri->Timeout, "P1Y");
	
	return SOAP_OK;
}

int __trt__GetServiceCapabilities(struct soap *soap, struct _trt__GetServiceCapabilities *trt__GetServiceCapabilities, struct _trt__GetServiceCapabilitiesResponse *trt__GetServiceCapabilitiesResponse)
{
	trt__GetServiceCapabilitiesResponse->Capabilities = (struct trt__Capabilities *)soap_malloc_v2(soap, sizeof(struct trt__Capabilities));
	
	trt__GetServiceCapabilitiesResponse->Capabilities->ProfileCapabilities = (struct trt__ProfileCapabilities *)soap_malloc_v2(soap, sizeof(struct trt__ProfileCapabilities));

	trt__GetServiceCapabilitiesResponse->Capabilities->ProfileCapabilities->MaximumNumberOfProfiles = (int *)soap_malloc(soap, sizeof(int));
	*trt__GetServiceCapabilitiesResponse->Capabilities->ProfileCapabilities->MaximumNumberOfProfiles = soap->pConfig->g_channel_num*MAX_STREAM_NUM;

	trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities = (struct trt__StreamingCapabilities *)soap_malloc_v2(soap, sizeof(struct trt__StreamingCapabilities));

	trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->RTPMulticast = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->RTPMulticast =  xsd__boolean__false_;
	trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->RTP_USCORETCP = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->RTP_USCORETCP =  xsd__boolean__true_;
	trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP =  xsd__boolean__true_;
	trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->NonAggregateControl = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetServiceCapabilitiesResponse->Capabilities->StreamingCapabilities->NonAggregateControl =  xsd__boolean__false_;

	trt__GetServiceCapabilitiesResponse->Capabilities->SnapshotUri = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetServiceCapabilitiesResponse->Capabilities->SnapshotUri =  xsd__boolean__true_;

	trt__GetServiceCapabilitiesResponse->Capabilities->Rotation = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*trt__GetServiceCapabilitiesResponse->Capabilities->Rotation =  xsd__boolean__false_;
	return SOAP_OK;
}

int __trv__GetServiceCapabilities(struct soap *soap, struct _trv__GetServiceCapabilities *trv__GetServiceCapabilities, struct _trv__GetServiceCapabilitiesResponse *trv__GetServiceCapabilitiesResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __trv__GetReceivers(struct soap *soap, struct _trv__GetReceivers *trv__GetReceivers, struct _trv__GetReceiversResponse *trv__GetReceiversResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __trv__GetReceiver(struct soap *soap, struct _trv__GetReceiver *trv__GetReceiver, struct _trv__GetReceiverResponse *trv__GetReceiverResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __trv__CreateReceiver(struct soap *soap, struct _trv__CreateReceiver *trv__CreateReceiver, struct _trv__CreateReceiverResponse *trv__CreateReceiverResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __trv__DeleteReceiver(struct soap *soap, struct _trv__DeleteReceiver *trv__DeleteReceiver, struct _trv__DeleteReceiverResponse *trv__DeleteReceiverResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __trv__ConfigureReceiver(struct soap *soap, struct _trv__ConfigureReceiver *trv__ConfigureReceiver, struct _trv__ConfigureReceiverResponse *trv__ConfigureReceiverResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __trv__SetReceiverMode(struct soap *soap, struct _trv__SetReceiverMode *trv__SetReceiverMode, struct _trv__SetReceiverModeResponse *trv__SetReceiverModeResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __trv__GetReceiverState(struct soap *soap, struct _trv__GetReceiverState *trv__GetReceiverState, struct _trv__GetReceiverStateResponse *trv__GetReceiverStateResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int __tse__GetServiceCapabilities(struct soap *soap, struct _tse__GetServiceCapabilities *tse__GetServiceCapabilities, struct _tse__GetServiceCapabilitiesResponse *tse__GetServiceCapabilitiesResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__GetRecordingSummary(struct soap *soap, struct _tse__GetRecordingSummary *tse__GetRecordingSummary, struct _tse__GetRecordingSummaryResponse *tse__GetRecordingSummaryResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__GetRecordingInformation(struct soap *soap, struct _tse__GetRecordingInformation *tse__GetRecordingInformation, struct _tse__GetRecordingInformationResponse *tse__GetRecordingInformationResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__GetMediaAttributes(struct soap *soap, struct _tse__GetMediaAttributes *tse__GetMediaAttributes, struct _tse__GetMediaAttributesResponse *tse__GetMediaAttributesResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__FindRecordings(struct soap *soap, struct _tse__FindRecordings *tse__FindRecordings, struct _tse__FindRecordingsResponse *tse__FindRecordingsResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__GetRecordingSearchResults(struct soap *soap, struct _tse__GetRecordingSearchResults *tse__GetRecordingSearchResults, struct _tse__GetRecordingSearchResultsResponse *tse__GetRecordingSearchResultsResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__FindEvents(struct soap *soap, struct _tse__FindEvents *tse__FindEvents, struct _tse__FindEventsResponse *tse__FindEventsResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__GetEventSearchResults(struct soap *soap, struct _tse__GetEventSearchResults *tse__GetEventSearchResults, struct _tse__GetEventSearchResultsResponse *tse__GetEventSearchResultsResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__FindPTZPosition(struct soap *soap, struct _tse__FindPTZPosition *tse__FindPTZPosition, struct _tse__FindPTZPositionResponse *tse__FindPTZPositionResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__GetPTZPositionSearchResults(struct soap *soap, struct _tse__GetPTZPositionSearchResults *tse__GetPTZPositionSearchResults, struct _tse__GetPTZPositionSearchResultsResponse *tse__GetPTZPositionSearchResultsResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__GetSearchState(struct soap *soap, struct _tse__GetSearchState *tse__GetSearchState, struct _tse__GetSearchStateResponse *tse__GetSearchStateResponse)
{	  
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__EndSearch(struct soap *soap, struct _tse__EndSearch *tse__EndSearch, struct _tse__EndSearchResponse *tse__EndSearchResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__FindMetadata(struct soap *soap, struct _tse__FindMetadata *tse__FindMetadata, struct _tse__FindMetadataResponse *tse__FindMetadataResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __tse__GetMetadataSearchResults(struct soap *soap, struct _tse__GetMetadataSearchResults *tse__GetMetadataSearchResults, struct _tse__GetMetadataSearchResultsResponse *tse__GetMetadataSearchResultsResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __d3__Hello(struct soap *soap, 
                struct d__HelloType *dn__Hello, 
                struct d__ResolveType *dn__HelloResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __d3__Bye(struct soap *soap, 
                struct d__ByeType *dn__Bye, 
                struct d__ResolveType *dn__ByeResponse)
{
    ONVIF_DBP("");
    return SOAP_OK;
}

int  __d4__Probe(struct soap* soap, struct d__ProbeType *ns1__Probe, struct d__ProbeMatchesType *ns1__ProbeResponse )
{ 
    ONVIF_DBP("	 __ns14__Probe \n"); 
    return SOAP_OK; 
}

#define FLG_UI_EXT 				(1 << 0)
#define FLG_UI_MOTION 			(1 << 1)
#define FLG_UI_ETH 				(1 << 2)
#define FLG_UI_AUDIO			(1 << 3)
#define FLG_UI_RECORD 			(1 << 4)
#define FLG_UI_AVI 				(1 << 5)
#define FLG_UI_JPG 				(1 << 6)

//海康的IPC也是直接返回当前状态
int  __ns2__PullMessages(struct soap* soap, struct _tev__PullMessages *tev__PullMessages, struct _tev__PullMessagesResponse *tev__PullMessagesResponse )
{
	if (!soap->header)
	{
		soap->header = (struct SOAP_ENV__Header *)soap_malloc_v2(soap, sizeof(struct SOAP_ENV__Header));
	}

	soap->header->wsa__MessageID = NULL;
	soap->header->wsa__Action = soap_strdup(soap, "http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/PullMessagesResponse");

	time_t tnow = time(NULL);
	time_t termTime = 0;
	if (soap->path)
	{
		char *p = strstr(soap->path, "/onvif/Pull?Idx=");
		if (p)
		{
			unsigned int id = strtoul(p + strlen("/onvif/Pull?Idx="), 0, 10);
			
			int i;
			unsigned char bNoFirst = 0;
			//unsigned char bNoIoFirst = 0;
			unsigned char bEnableMotion = 0;
			unsigned char bEnableIO = 0;
			pthread_mutex_lock(&soap->pConfig->onvif_pullLock);
			for (i = 0; i < 10; i++)
			{
				if (soap->pConfig->onvif_PullNode[i].id)
				{
					if (soap->pConfig->onvif_PullNode[i].endTime < tnow)
					{
						//printf("%ul __ns2__PullMessages: pull id%d is timeout!!!\n", tnow, onvif_PullNode[i].id);
					
						soap->pConfig->onvif_PullNode[i].id = 0;
						soap->pConfig->onvif_PullNode[i].endTime = 0;
						soap->pConfig->onvif_PullNode[i].status = soap->pConfig->onvif_PullNode[i].ioStatus = 0;
						soap->pConfig->onvif_PullNode[i].bNoFirst = soap->pConfig->onvif_PullNode[i].bNoIoFirst = 0;
					}
					else if (soap->pConfig->onvif_PullNode[i].id == id)
					{
						bNoFirst = soap->pConfig->onvif_PullNode[i].bNoFirst;
						//bNoIoFirst = soap->pConfig->onvif_PullNode[i].bNoIoFirst;
						soap->pConfig->onvif_PullNode[i].bNoFirst = soap->pConfig->onvif_PullNode[i].bNoIoFirst = 1;
						bEnableMotion = soap->pConfig->onvif_PullNode[i].bEnableMotion;
						bEnableIO = soap->pConfig->onvif_PullNode[i].bEnableIO;
						soap->pConfig->onvif_PullNode[i].endTime += 10;
						termTime = soap->pConfig->onvif_PullNode[i].endTime;
						break;
					}
				}
			}
			pthread_mutex_unlock(&soap->pConfig->onvif_pullLock);

			if (i >= 10)
			{
				//printf("************************************find out id is error**********************\n");
				return SOAP_OK;
			}

			unsigned int status = soap->pConfig->MAlarm_status;
			unsigned int ioStatus = soap->pConfig->IOAlarm_status;

			QMAPI_SYSTEMTIME ts;
			(*soap->pConfig->GetUtcTime)(&tev__PullMessagesResponse->CurrentTime, &ts);
			tev__PullMessagesResponse->TerminationTime = tev__PullMessagesResponse->CurrentTime + (termTime - tnow);

			//printf("curtime is %lu endtime is %lu\n", tev__PullMessagesResponse->CurrentTime, tev__PullMessagesResponse->TerminationTime);
			tev__PullMessagesResponse->wsnt__NotificationMessage = (struct wsnt__NotificationMessageHolderType *)soap_malloc_v2(soap, sizeof(struct wsnt__NotificationMessageHolderType) * tev__PullMessages->MessageLimit);
			
			int isalarm = 0;
			int flag = 0;
			for (i = 0; i < tev__PullMessages->MessageLimit && i < soap->pConfig->g_channel_num && flag < tev__PullMessages->MessageLimit; i++)
			{
				if (bEnableMotion)
				{
					isalarm = status&(1<<i);
		
					tev__PullMessagesResponse->wsnt__NotificationMessage[flag].wsnt__Topic = (struct wsnt__TopicExpressionType *)soap_malloc_v2(soap, sizeof(struct wsnt__TopicExpressionType));
					tev__PullMessagesResponse->wsnt__NotificationMessage[flag].wsnt__Topic->__any = soap_strdup(soap, ONVIF_EVENT_MOTION); 
					tev__PullMessagesResponse->wsnt__NotificationMessage[flag].wsnt__Topic->Dialect = soap_strdup(soap, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet"); 

					tev__PullMessagesResponse->wsnt__NotificationMessage[flag].Message.__any = (char *)soap_malloc(soap, sizeof(char) * 640);			
					sprintf(tev__PullMessagesResponse->wsnt__NotificationMessage[flag].Message.__any, "<tt:Message UtcTime=\"%d-%02d-%02dT%02d:%02d:%02dZ\" PropertyOperation=\"%s\"><tt:Source>"
						"<tt:SimpleItem Name=\"VideoSourceConfigurationToken\" Value=\"VideoSourceConfiguration%d\"/><tt:SimpleItem Name=\"VideoAnalyticsConfigurationToken\" Value=\"VideoAnalyticsToken%d\"/>"
						"<tt:SimpleItem Name=\"Rule\" Value=\"MyMotionDetectorRule\"/></tt:Source><tt:Data><tt:SimpleItem Name=\"IsMotion\" Value=\"%s\"/>"
						"</tt:Data></tt:Message>",
						ts.wYear, ts.wMonth, ts.wDay, ts.wHour, ts.wMinute, ts.wSecond, bNoFirst ? "Changed" : "Initialized", i, i, isalarm ? "true" : "false");
				
					++flag;
				}

				if (bEnableIO)
				{
					isalarm = ioStatus&(1<<i);
					if (flag < tev__PullMessages->MessageLimit)
					{
						tev__PullMessagesResponse->wsnt__NotificationMessage[flag].wsnt__Topic = (struct wsnt__TopicExpressionType *)soap_malloc_v2(soap, sizeof(struct wsnt__TopicExpressionType));
						tev__PullMessagesResponse->wsnt__NotificationMessage[flag].wsnt__Topic->__any = soap_strdup(soap, ONVIF_EVENT_IOALARM); 
						tev__PullMessagesResponse->wsnt__NotificationMessage[flag].wsnt__Topic->Dialect = soap_strdup(soap, "http://docs.oasis-open.org/wsn/t-1/TopicExpression/Concrete"); 
					
						tev__PullMessagesResponse->wsnt__NotificationMessage[flag].Message.__any = (char *)soap_malloc(soap, sizeof(char) * 500);			
						sprintf(tev__PullMessagesResponse->wsnt__NotificationMessage[flag].Message.__any, "<tt:Message UtcTime=\"%d-%02d-%02dT%02d:%02d:%02dZ\" PropertyOperation=\"%s\">"
							"<tt:Source><tt:SimpleItem Name=\"InputToken\" Value=\"VideoSourceConfiguration%d\"/>"
							"</tt:Source><tt:Data><tt:SimpleItem Name=\"LogicalState\" Value=\"%s\"/>"
							"</tt:Data></tt:Message>",
							ts.wYear, ts.wMonth, ts.wDay, ts.wHour, ts.wMinute, ts.wSecond, bNoFirst ? "Changed" : "Initialized", i, isalarm ? "true" : "false");

						++flag;
					}
				}
			}

			tev__PullMessagesResponse->__sizeNotificationMessage = flag;
		}
	}
    
	return SOAP_OK;	 
}
 
int  __ns2__SetSynchronizationPoint(struct soap* soap, struct _tev__SetSynchronizationPoint *tev__SetSynchronizationPoint, struct _tev__SetSynchronizationPointResponse *tev__SetSynchronizationPointResponse )
{
#if 0
	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));
	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);
#endif

	if(!soap->header)
	{
		soap->header = (struct SOAP_ENV__Header *)soap_malloc_v2(soap, sizeof(struct SOAP_ENV__Header));
	}
	
#if 0
	if (!soap->header->wsa__To)
	{
		soap->header->wsa__To = (char *)soap_malloc(soap, 128);
	}

	if(TRUE == isWIFI)
	{
		if(net_conf.wHttpPort != 80)
			sprintf(soap->header->wsa__To, "http://%s:%d/onvif/Events", wifi_cfg.dwNetIpAddr.csIpV4, net_conf.wHttpPort);
		else
			sprintf(soap->header->wsa__To, "http://%s/onvif/Events", wifi_cfg.dwNetIpAddr.csIpV4);
	}
	else
	{
		if(net_conf.wHttpPort != 80)
			sprintf(soap->header->wsa__To, "http://%s:%d/onvif/Events", net_conf.stEtherNet[0].strIPAddr.csIpV4, net_conf.wHttpPort);
		else
			sprintf(soap->header->wsa__To, "http://%s/onvif/Events", net_conf.stEtherNet[0].strIPAddr.csIpV4);
	}
#endif

	soap->header->wsa__Action = soap_strdup(soap, "http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/SetSynchronizationPointResponse");
	return SOAP_OK;
}
 
int  __ns3__GetServiceCapabilities(struct soap* soap, struct _tev__GetServiceCapabilities *tev__GetServiceCapabilities, struct _tev__GetServiceCapabilitiesResponse *tev__GetServiceCapabilitiesResponse )
{
	if (!soap->header)
	{
		soap->header = (struct SOAP_ENV__Header *)soap_malloc(soap, sizeof(struct SOAP_ENV__Header));
	}
	soap->header->wsa__Action = soap_strdup(soap, "http://www.onvif.org/ver10/events/wsdl/EventPortType/GetServiceCapabilities");

	tev__GetServiceCapabilitiesResponse->Capabilities = (struct tev__Capabilities *)soap_malloc_v2(soap, sizeof(struct tev__Capabilities));
	tev__GetServiceCapabilitiesResponse->Capabilities->WSSubscriptionPolicySupport = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tev__GetServiceCapabilitiesResponse->Capabilities->WSSubscriptionPolicySupport = xsd__boolean__true_;
	tev__GetServiceCapabilitiesResponse->Capabilities->WSPullPointSupport = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tev__GetServiceCapabilitiesResponse->Capabilities->WSPullPointSupport = xsd__boolean__true_;
	tev__GetServiceCapabilitiesResponse->Capabilities->WSPausableSubscriptionManagerInterfaceSupport = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tev__GetServiceCapabilitiesResponse->Capabilities->WSPausableSubscriptionManagerInterfaceSupport = xsd__boolean__false_;

	tev__GetServiceCapabilitiesResponse->Capabilities->MaxNotificationProducers = (int *)soap_malloc(soap, sizeof(int));
	*tev__GetServiceCapabilitiesResponse->Capabilities->MaxNotificationProducers = 10;
	tev__GetServiceCapabilitiesResponse->Capabilities->MaxPullPoints = (int *)soap_malloc(soap, sizeof(int));
	*tev__GetServiceCapabilitiesResponse->Capabilities->MaxPullPoints = 10;
	return SOAP_OK; 
}

  
char *anyparser(char *str, char* retstr)
{
	 // char *retstr = malloc(sizeof(char)* INFO_LENGTH);
    while(*str != '>')
    {
        str++;
    }
    if(strstr(str,"tns1:VideoAnalytics/tns1:MotionDetection/") != NULL ||  strstr(str,"tns1:VideoAnalytics/tns1:FaceDetection/") != NULL)
    {
        return str;
    }
    else if(strstr(str,"@Name") != NULL)
    {
        str = strstr(str,"@Name=");
        while(*str != '"')
        {
            str ++;
        }
        str++;
        int i=0;
        while(*str != ']')
        {
            *(retstr+i) = *str;
            str ++;
            i++;
        }
        *(retstr + (--i)) = '\0';
        return retstr;
    }
    else
    {
        return NULL;
    }
}

#if 0
 void *eventtracker_thr(void *arg)
{
	int sem_value = 0;
	union semun{
                int val;
                struct semid_ds *buf;
                short *array;
        }argument;
	int id;
	time_t current;
	time_t time_tm;
	unsigned char value = 1;
	int ret;

	g_time_out = *(long *) arg;
	canSubscribe_pullmessages = 0;

	/* Saving current Time */
	current = time(NULL);

	/* Get semaphore descriptor */
	id = semget(KEY_NOTIFY_SEM, 1, 0666);
	if(id < 0)
	{
		ONVIF_DBP( "Unable to obtain semaphore.");
    }
	
	/* Initialize Motion Detection */
	ret = ControlSystemData(SFIELD_SET_MOTIONENABLE, (void *)&value, sizeof(value));
	if (ret < 0)
	{
		//ONVIF_DBG("Failed to enable motion.");
	}

	/* Ignore any previous motion */
	argument.val = 0;
	semctl(id, 0, SETVAL, argument);

	/* TODO:: Send Initialization Notification */	

	/* Loop till Termination Time */
	while(g_time_out > 0)
	{	
		/* semaphore get value */
		/* Semaphore value set in proc_alarm.c when motion is detected */
		sem_value = semctl(id, 0, GETVAL, 0);
		/* Check if any motion is detected */
		if(sem_value & MOTION_DETECTED_MASK)
		{
			argument.val = 0;
			semctl(id, 0, SETVAL, argument);
			eventstatus[0] |= MOTION_DETECTED_MASK;
		}
		else if(sem_value & NO_MOTION_MASK)
		{
			argument.val = 0;
			semctl(id, 0, SETVAL, argument); 
		}
		
	       	time_tm = time(NULL);
		if((time_tm - current) > g_time_out)
		{
			g_time_out = 0;
		}
	}
	canSubscribe_pullmessages = 1;
	pthread_exit(NULL);
	return NULL;
}
#endif

static void CheckEventFilter(const struct wsnt__FilterType *pFliter, int *pEnableMotionAlarm, int *pEnableIOAlarm)
{
	*pEnableMotionAlarm = *pEnableIOAlarm = 0;

	int i;
	const char *p;
	for (i = 0; i < pFliter->__size; i++)
	{
		//printf("fliter is %s\n", tev__CreatePullPointSubscription->Filter->__any[i]);
		p = strstr(pFliter->__any[i], "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		if (p != NULL)
		{
		#if 1
			while (*p && *p != '>')
			{
				++p;
			}
	
			if (*p)
			{
				++p;
				while (isspace(*p))
				{
					++p;
				}
	
				if (!strncmp(p, ONVIF_EVENT_MOTION, strlen(ONVIF_EVENT_MOTION)))
				{
		#endif		
					printf("enable motion alarm!\n");
					*pEnableMotionAlarm = 1;
		#if 1		
				}
			}
		#endif	
		}
		else if ((p = strstr(pFliter->__any[i], "http://docs.oasis-open.org/wsn/t-1/TopicExpression/Concrete")) != NULL)
		{
		#if 1
			while (*p && *p != '>')
			{
				++p;
			}
	
			if (*p)
			{
				++p;
				while (isspace(*p))
				{
					++p;
				}
	
				if (!strncmp(p, ONVIF_EVENT_IOALARM, strlen(ONVIF_EVENT_IOALARM)))
				{
		#endif		
					printf("enable io alarm!\n");
					*pEnableIOAlarm = 1;
		#if 1		
				}
			}
		#endif	
		}
	}
}

int  __ns3__CreatePullPointSubscription(struct soap* soap, struct _tev__CreatePullPointSubscription *tev__CreatePullPointSubscription, struct _tev__CreatePullPointSubscriptionResponse *tev__CreatePullPointSubscriptionResponse )
{
	if (!soap->header)
	{
		soap->header = (struct SOAP_ENV__Header *)soap_malloc_v2(soap, sizeof(struct SOAP_ENV__Header));
	}

	int EnableMotionAlarm = 1;
	int EnableIOAlarm = 1;
	if (tev__CreatePullPointSubscription->Filter && tev__CreatePullPointSubscription->Filter->__size)
	{
		CheckEventFilter(tev__CreatePullPointSubscription->Filter, &EnableMotionAlarm, &EnableIOAlarm);

		if (!EnableMotionAlarm && !EnableIOAlarm)
		{	
			soap->header->wsa__Action = soap_strdup(soap, "http://www.w3.org/2005/08/addressing/soap/fault"); 

			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
				"ter:InvalidFilterFaultType", "invalid filter fault");
		}
	}

	time_t tnow;
	soap->pConfig->GetUtcTime(&tnow, NULL);

	time_t curTime = time(NULL);
	tev__CreatePullPointSubscriptionResponse->SubscriptionReference.Address = (char *)soap_malloc(soap, sizeof(char) * LARGE_INFO_LENGTH);

	unsigned int k = 0;

	struct pullnode *pTemp = NULL;
	pthread_mutex_lock(&soap->pConfig->onvif_pullLock);
	printf("start create pull\n");
	for (k = 0; k < 10; k++)
	{
		printf("id is %d endtime is %d\n", soap->pConfig->onvif_PullNode[k].id, (int)soap->pConfig->onvif_PullNode[k].endTime);
	}
	
	if (!pTemp)
	{
		for (k = 0; k < 10; k++)
		{
			if (soap->pConfig->onvif_PullNode[k].id && (soap->pConfig->onvif_PullNode[k].endTime < curTime))
			{
				printf("pull id%d is timeout!!!\n", soap->pConfig->onvif_PullNode[k].id);
				
				soap->pConfig->onvif_PullNode[k].id = 0;
				pTemp = &soap->pConfig->onvif_PullNode[k];
				break;
			}
			else if (!soap->pConfig->onvif_PullNode[k].id)
			{
				pTemp = &soap->pConfig->onvif_PullNode[k];
				break;
			}
		}
	}
	printf("find id%d\n", k);

	if (pTemp)
	{
		while (1)
		{
			if (++soap->pConfig->pullInex == 0)
			{
				soap->pConfig->pullInex = 1;
			}
			printf("cur pullindex id is %d\n", soap->pConfig->pullInex);

			for (k = 0; k < 10; k++)
			{
				if (soap->pConfig->onvif_PullNode[k].id && soap->pConfig->pullInex == soap->pConfig->onvif_PullNode[k].id)
				{
					break;
				}
			}

			printf("find pullindex id is %d\n", soap->pConfig->pullInex);
			if (k >= 10)
			{
				break;
			}
		}
	}
	pthread_mutex_unlock(&soap->pConfig->onvif_pullLock);
	printf("end create pull\n");

	if (pTemp)
	{	
		int pts = 10;
		if (tev__CreatePullPointSubscription->InitialTerminationTime)
		{
			pts = periodtol(tev__CreatePullPointSubscription->InitialTerminationTime);
			pTemp->endTime = curTime + pts;
		}
		else
		{
			pTemp->endTime = curTime + 10;
		}

		printf("time out is %s\n", tev__CreatePullPointSubscription->InitialTerminationTime);
		
		pTemp->id = soap->pConfig->pullInex;
		pTemp->bEnableMotion = EnableMotionAlarm;
		pTemp->bEnableIO = EnableIOAlarm;

		char addr[32];
		get_addr(soap, addr, 32);

		sprintf(tev__CreatePullPointSubscriptionResponse->SubscriptionReference.Address, "http://%s:60987/onvif/Pull?Idx=%u", addr, soap->pConfig->pullInex);	

		tev__CreatePullPointSubscriptionResponse->wsnt__CurrentTime = tnow;
		tev__CreatePullPointSubscriptionResponse->wsnt__TerminationTime = tnow + pts;
		
		soap->header->wsa__Action = soap_strdup(soap, "http://www.onvif.org/ver10/events/wsdl/EventPortType/CreatePullPointSubscriptionResponse");
	}
	
	return SOAP_OK;	
}
 
int  __ns3__GetEventProperties(struct soap* soap, struct _tev__GetEventProperties *tev__GetEventProperties, struct _tev__GetEventPropertiesResponse *tev__GetEventPropertiesResponse )
{
#if 0
	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));

	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);
#endif	
	if(!soap->header)
	{
		soap->header = (struct SOAP_ENV__Header *)soap_malloc_v2(soap, sizeof(struct SOAP_ENV__Header));
	}
#if 0
	if (!soap->header->wsa__To)
	{
		soap->header->wsa__To = (char *)soap_malloc(soap, sizeof(char) * 128);
	}
	
	if(TRUE == isWIFI)
	{
		if(net_conf.wHttpPort != 80)
			sprintf(soap->header->wsa__To, "http://%s:%d/onvif/Events", wifi_cfg.dwNetIpAddr.csIpV4, net_conf.wHttpPort);
		else
			sprintf(soap->header->wsa__To, "http://%s/onvif/Events", wifi_cfg.dwNetIpAddr.csIpV4);
	}
	else
	{
		if(net_conf.wHttpPort != 80)
			sprintf(soap->header->wsa__To, "http://%s:%d/onvif/Events", net_conf.stEtherNet[0].strIPAddr.csIpV4, net_conf.wHttpPort);
		else
			sprintf(soap->header->wsa__To, "http://%s/onvif/Events", net_conf.stEtherNet[0].strIPAddr.csIpV4);
	}
#endif
	soap->header->wsa__Action = soap_strdup(soap, "http://www.onvif.org/ver10/events/wsdl/EventPortType/GetEventPropertiesResponse"); 		 

    tev__GetEventPropertiesResponse->__sizeTopicNamespaceLocation = 1;
    tev__GetEventPropertiesResponse->TopicNamespaceLocation = (char **)soap_malloc(soap, sizeof(char *));
    *tev__GetEventPropertiesResponse->TopicNamespaceLocation = soap_strdup(soap, "http://www.onvif.org/onvif/ver10/topics/topicns.xml");

    tev__GetEventPropertiesResponse->wsnt__FixedTopicSet = xsd__boolean__true_;
    tev__GetEventPropertiesResponse->wstop__TopicSet = (struct wstop__TopicSetType *)soap_malloc_v2(soap, sizeof(struct wstop__TopicSetType)); 
	tev__GetEventPropertiesResponse->wstop__TopicSet->documentation = (struct wstop__Documentation *)soap_malloc_v2(soap,sizeof(struct wstop__Documentation));
    tev__GetEventPropertiesResponse->wstop__TopicSet->documentation->__size = 0;
    tev__GetEventPropertiesResponse->wstop__TopicSet->documentation->__any = NULL;
    tev__GetEventPropertiesResponse->wstop__TopicSet->documentation->__mixed = NULL;
    tev__GetEventPropertiesResponse->wstop__TopicSet->__size = 1;
    tev__GetEventPropertiesResponse->wstop__TopicSet->__any = (char **)soap_malloc(soap, sizeof(char *));
    *tev__GetEventPropertiesResponse->wstop__TopicSet->__any = soap_strdup(soap, 
		"<tns1:RuleEngine wstop:topic=\"true\">"
		"<CellMotionDetector wstop:topic=\"true\">"
		"<Motion wstop:topic=\"true\">"
		"<tt:MessageDescription IsProperty=\"true\"><tt:Source>"
		"<tt:SimpleItemDescription Name=\"VideoSourceConfigurationToken\" Type=\"tt:ReferenceToken\"/>"
		"<tt:SimpleItemDescription Name=\"VideoAnalyticsConfigurationToken\" Type=\"tt:ReferenceToken\"/>"
		
"<tt:SimpleItemDescription Name=\"Rule\" Type=\"xs:string\"/></tt:Source>"
		"<tt:Data><tt:SimpleItemDescription Name=\"IsMotion\" Type=\"xs:boolean\"/>"
		"</tt:Data>"
		"</tt:MessageDescription>"
		"</Motion>"
		"</CellMotionDetector>"
		"</tns1:RuleEngine>"
		"<tns1:Device wstop:topic=\"true\">"
		"<Trigger wstop:topic=\"true\">"
		"<DigitalInput wstop:topic=\"true\">"
		"<tt:MessageDescription IsProperty=\"true\"><tt:Source>"
		"<tt:SimpleItemDescription Name=\"InputToken\" Type=\"tt:ReferanceToken\"/></tt:Source>"
		"<tt:Data>"
		"<tt:SimpleItemDescription Name=\"LogicalState\" Type=\"xs:boolean\"/>"
		"</tt:Data>"
		"</tt:MessageDescription>"
		"</DigitalInput>"
		"</Trigger>"
		"</tns1:Device>");

	tev__GetEventPropertiesResponse->__sizeTopicExpressionDialect = 2;
    tev__GetEventPropertiesResponse->wsnt__TopicExpressionDialect = (char **)soap_malloc(soap, sizeof(char *) * 2); 
    tev__GetEventPropertiesResponse->wsnt__TopicExpressionDialect[0] = soap_strdup(soap, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet"); 
    tev__GetEventPropertiesResponse->wsnt__TopicExpressionDialect[1] = soap_strdup(soap, "http://docs.oasis-open.org/wsn/t-1/TopicExpression/Concrete"); 

    tev__GetEventPropertiesResponse->__sizeMessageContentFilterDialect = 1;
    tev__GetEventPropertiesResponse->MessageContentFilterDialect = (char **)soap_malloc(soap, sizeof(char *));
   	*tev__GetEventPropertiesResponse->MessageContentFilterDialect = soap_strdup(soap, "http://www.onvif.org/ver10/tev/messageContentFilter/ItemFilter");
    tev__GetEventPropertiesResponse->__sizeProducerPropertiesFilterDialect = 0;
    tev__GetEventPropertiesResponse->ProducerPropertiesFilterDialect = NULL;
    tev__GetEventPropertiesResponse->__sizeMessageContentSchemaLocation = 1;
    tev__GetEventPropertiesResponse->MessageContentSchemaLocation = (char **)soap_malloc(soap, sizeof(char *));
    *tev__GetEventPropertiesResponse->MessageContentSchemaLocation = soap_strdup(soap, "http://www.onvif.org/ver10/schema/onvif.xsd");

	return SOAP_OK;
}

int  __ns4__Renew(struct soap* soap, struct _wsnt__Renew *wsnt__Renew, struct _wsnt__RenewResponse *wsnt__RenewResponse )
{
	ONVIF_DBP(" wsnt__Renew->TerminationTime %s  idx:%d\n", wsnt__Renew->TerminationTime, soap->pidx); 
	//http://192.168.1.159:8999/onvif/Subscription?Idx=1
	
	long NotifyEndTime = 0;	
	time_t time_now = time(NULL);
	int bFind = 0;
	int k;

	time_t rtcTermTime = 0;
	
	time_t ts = 0;
	soap->pConfig->GetUtcTime(&ts, NULL);

	//<TerminationTime>2012-10-10T10:03:15Z</TerminationTime>
	//<TerminationTime>PT10S</TerminationTime>
	
	char *p = strstr(wsnt__Renew->TerminationTime, "Z");
	if (p)
	{
		struct tm TTerm = {0};
		
		if (6 != sscanf(wsnt__Renew->TerminationTime, "%d-%d-%dT%d:%d:%dZ", &TTerm.tm_year, &TTerm.tm_mon, &TTerm.tm_mday, 
			&TTerm.tm_hour, &TTerm.tm_min, &TTerm.tm_sec ) )
		{
			return(send_soap_fault_msg(soap, SOAP_FAULT,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:InvalidFilterFaultType",
				"InvalidTerminationTime"));
		}

		TTerm.tm_year = (TTerm.tm_year > 1900 ? TTerm.tm_year - 1900 : 0) ;
		TTerm.tm_mon = (TTerm.tm_mon > 1 ? TTerm.tm_mon - 1 : 0);

		rtcTermTime = NotifyEndTime = mktime(&TTerm);
		NotifyEndTime = time_now + (NotifyEndTime - ts);

		//ONVIF_DBP(" g_NotifyInfo.notify[%d].endTime %d \n", k, g_NotifyInfo.notify[k].endTime);
	}
	else
	{
		long pts = periodtol(wsnt__Renew->TerminationTime);
		NotifyEndTime = time_now + pts;
		rtcTermTime = ts + pts;
	}

	ONVIF_DBP(" NotifyEndTime %d \n", NotifyEndTime);

	//ONVIF_DBP("before set g_NotifyInfo.notify[%d].currentTime %d \n", k, g_NotifyInfo.notify[k].currentTime);

	if (soap->path)
	{
		ONVIF_DBP("soap->header->to %s \n", soap->path);

		char *p = strstr(soap->path, "onvif/Subscription?Idx=");
		if (p)
		{
			int id = atoi(p + strlen("onvif/Subscription?Idx="));

			pthread_mutex_lock(&soap->pConfig->gNotifyLock);
			for ( k = 0; k < MAX_NOTIFY_CLIENT; k++ ) 
			{
				if (soap->pConfig->g_NotifyInfo.notify[k].id == id) 
				{
					soap->pConfig->g_NotifyInfo.notify[k].endTime = NotifyEndTime;
					bFind = 1;
					break;
				}
			}
			pthread_mutex_unlock(&soap->pConfig->gNotifyLock);
		}
		else if ((p = strstr(soap->path, "onvif/Pull?Idx=")) != NULL)
		{
			int id = atoi(p + strlen("onvif/Pull?Idx="));

			pthread_mutex_lock(&soap->pConfig->onvif_pullLock);
			for ( k = 0; k < 10; k++ ) 
			{
				if (soap->pConfig->onvif_PullNode[k].id)
				{
					if (soap->pConfig->onvif_PullNode[k].endTime < time_now)
					{
						//printf("__ns4__Renew pull id%d is timeout!!!\n", onvif_PullNode[k].id);
						
						soap->pConfig->onvif_PullNode[k].id = 0;
						soap->pConfig->onvif_PullNode[k].endTime = 0;
						soap->pConfig->onvif_PullNode[k].status = soap->pConfig->onvif_PullNode[k].ioStatus = 0;
						soap->pConfig->onvif_PullNode[k].bNoFirst = soap->pConfig->onvif_PullNode[k].bNoIoFirst = 0;
					}
					else if (soap->pConfig->onvif_PullNode[k].id == id) 
					{
						//printf("%ul ******************__ns4__Renew pull id%d is update ************\n", time_now, onvif_PullNode[k].id);
						soap->pConfig->onvif_PullNode[k].endTime = NotifyEndTime;
						bFind = 1;
						break;
					}
				}
			}
			pthread_mutex_unlock(&soap->pConfig->onvif_pullLock);
		}
	}

	wsnt__RenewResponse->wsnt__TerminationTime = rtcTermTime;
	wsnt__RenewResponse->wsnt__CurrentTime = (time_t *) soap_malloc(soap, sizeof(time_t));
	*wsnt__RenewResponse->wsnt__CurrentTime = ts; 

	if(!soap->header)
	{
		soap->header = (struct SOAP_ENV__Header *)soap_malloc_v2(soap, sizeof(struct SOAP_ENV__Header));
	}
	soap->header->wsa__To = NULL;
	soap->header->wsa__ReplyTo = NULL;
	
	if (!bFind)
	{
		soap->header->wsa__Action = soap_strdup(soap, "http://www.w3.org/2005/08/addressing/soap/fault"); 				

		return send_soap_fault_msg(soap,SOAP_FAULT,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:ResourceUnknownFaultType",
			"NoSubscribe");		
	}
		
	soap->header->wsa__Action = soap_strdup(soap, "http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/RenewResponse");
	return SOAP_OK;
}

//#ifdef SUPPORT_DAHUA_MOTION_DETECT 
#if 0

int  __ns4__Unsubscribe(struct soap* soap, struct _wsnt__Unsubscribe *wsnt__Unsubscribe, struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse )
{
 
	ONVIF_DBP("");  

	memset(csEventIP ,0, 32);
	eventPort = 0;

	memset(wsnt__UnsubscribeResponse,0, sizeof(struct _wsnt__UnsubscribeResponse)); 

/*
	 if(!soap->header)
	{
		soap->header = (struct SOAP_ENV__Header *)soap_malloc_v2(soap, sizeof(struct SOAP_ENV__Header));
		memset(soap->header, 0, sizeof(struct SOAP_ENV__Header));
	}
	
	if ( ! soap->header->wsa__Action) { 
		soap->header->wsa__Action = (char*)soap_malloc_v2(soap,sizeof(char)* 128);		  
	}   
	strcpy(soap->header->wsa__Action, "http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/UnsubscribeResponse"); 	  

	memset(wsnt__UnsubscribeResponse, 0, sizeof(struct _wsnt__UnsubscribeResponse));
 
	return SOAP_OK; 
*/
 
}

int  __ns5__Subscribe(struct soap* soap, struct _wsnt__Subscribe *wsnt__Subscribe, struct _wsnt__SubscribeResponse *wsnt__SubscribeResponse )
{ 	

	ONVIF_DBP("");

	char * eventAddress = NULL;
	memset(csEventIP,0,32);
	
	eventIP = (char *)soap_malloc(soap,LEN_128_SIZE);
	memset(eventIP,0,LEN_128_SIZE);

	eventAddress = wsnt__Subscribe->ConsumerReference.Address;

	if(!eventAddress)
	{
		//printf("%s  %d, aaaaaaaaa\n",__FUNCTION__, __LINE__);
		return SOAP_ERR;
	}
	
	char *p = NULL;
	p = strstr(eventAddress,"//");
	if(p!= NULL)
		eventAddress += 7; //偏掉 http://
	

	p = strstr(eventAddress,":");
	if(p != NULL)
	{
		memcpy(eventIP,eventAddress,p-eventAddress);
		memcpy(csEventIP,eventIP,32);
		//printf("@@@@@@ %s csEventIP:%s \n",eventIP,csEventIP);
		p+= 1;
		eventAddress = p;
	}

	char csPort[16];
	memset(csPort,0,16);
	p = strstr(eventAddress,"/");
	if(p!= NULL)
	{
		memcpy(csPort,eventAddress,p-eventAddress);
		eventPort = atoi(csPort);
		//printf("@@@@@@ %d \n",eventPort);
	}


	

	char macaddr[6];	
	char _IPv4Address[LEN_128_SIZE] = { 0 };
	char buf[32] = { 0 };
	//char *p = NULL;
	char initial_time[SMALL_INFO_LENGTH] = "PT10S";

	time_t currentTime;
	time_t endTime;

	//NET_IPV4 ip;
	struct tm endTerm = { 0 };
	int zoneIndex = 0;

	QMAPI_NET_NETWORK_CFG 	net_conf;
	memset(&net_conf,0,sizeof(QMAPI_NET_NETWORK_CFG));
	if(get_network_cfg(&net_conf) != 0)
	{
		return SOAP_ERR;
	}

	currentTime = time(NULL);	
	net_get_hwaddr(ETH_NAME, macaddr);

	int ret;
	QMAPI_NET_ZONEANDDST tz_conf;
	memset(&tz_conf, 0, sizeof(QMAPI_NET_ZONEANDDST));
	tz_conf.nTimeZone = 8;
/*
	ret = QMapi_sys_ioctrl(0,QMAPI_SYSCFG_GET_ZONEANDDSTCFG, 0,&tz_conf,sizeof(QMAPI_NET_ZONEANDDST));
	if(ret<0)
	{
		printf("%s QMAPI_SYSCFG_GET_ZONEANDDSTCFG error!\n", __FUNCTION__);
		return(send_soap_fault_msg(soap, 400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:OutOfMemory",
			"get system config failed"));
	}
	*/

	if (wsnt__Subscribe->Filter != NULL) {
		return(send_soap_fault_msg(soap, 400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:InvalidFilterFault",
			"invalid filter fault"));
	}

	//<TerminationTime>2012-10-10T10:03:15Z</TerminationTime>
	//<TerminationTime>PT10S</TerminationTime>

	if (wsnt__Subscribe->InitialTerminationTime != NULL) {		
		p = strstr(wsnt__Subscribe->InitialTerminationTime, "Z");
		
		if ( p ) {		
			strcpy(buf, wsnt__Subscribe->InitialTerminationTime);
			if ( 6 != sscanf(buf, "%d-%d-%dT%d:%d:%dZ", &endTerm.tm_year, &endTerm.tm_mon, &endTerm.tm_mday, 
				&endTerm.tm_hour, &endTerm.tm_min, &endTerm.tm_sec ) ) {
				return(send_soap_fault_msg(soap,400,
					"SOAP-ENV:Sender",
					"ter:InvalidArgVal",
					"ter:InvalidTerminationTime",
					"InvalidTerminationTime"));
			}

			endTerm.tm_year = (endTerm.tm_year > 1900 ? endTerm.tm_year - 1900 : 0) ;
			endTerm.tm_mon = (endTerm.tm_mon > 1 ? endTerm.tm_mon - 1 : 0 );			
			endTime = mktime(&endTerm);
			//zoneIndex = oSysInfo->lan_config.net.ntp_timezone;
			//zoneIndex = tz_conf.nTimeZone;
			zoneIndex = 20;//东八区
			if ( zoneIndex < 25 && zoneIndex >= 0 ) {				
				endTime += ((zoneIndex - 12) * 3600);			
			}			

		} else {
			endTime = periodtol(wsnt__Subscribe->InitialTerminationTime);			
			currentTime = 0;
		}	

		ONVIF_DBP("wsnt__Subscribe->InitialTerminationTime %s\n", wsnt__Subscribe->InitialTerminationTime );
	} else {
		endTime = periodtol(initial_time);
		currentTime = 0;
	}
	
	memset(&wsnt__SubscribeResponse->SubscriptionReference, 0, sizeof(struct wsa__EndpointReferenceType));
	wsnt__SubscribeResponse->SubscriptionReference.Address = (char *) soap_malloc_v2(soap, sizeof(char) * INFO_LENGTH);

	//ip.int32 = net_get_ifaddr("eth0");
	//static unsigned int SubscriptionIndex = 0;	
	sprintf(_IPv4Address, "http://%s:%d/onvif/Subscription?Idx=%d", net_conf.stEtherNet[0].strIPAddr.csIpV4, net_conf.wHttpPort, uuid);	
	//sprintf(_IPv4Address, "http://%s:%d/onvif/Subscription?Idx=%d", net_conf.stEtherNet[0].strIPAddr.csIpV4, onvif_port, SubscriptionIndex++);	
	
	strcpy(wsnt__SubscribeResponse->SubscriptionReference.Address, _IPv4Address);	
	wsnt__SubscribeResponse->SubscriptionReference.ReferenceParameters = NULL;	
	wsnt__SubscribeResponse->SubscriptionReference.__size = 0;
	wsnt__SubscribeResponse->SubscriptionReference.__any = NULL;
	wsnt__SubscribeResponse->SubscriptionReference.__anyAttribute = NULL;
	wsnt__SubscribeResponse->wsnt__CurrentTime = (time_t *) soap_malloc_v2(soap, sizeof(time_t));
	wsnt__SubscribeResponse->wsnt__CurrentTime[0] = currentTime;
	wsnt__SubscribeResponse->wsnt__TerminationTime = (time_t *) soap_malloc_v2(soap, sizeof(time_t));	
	wsnt__SubscribeResponse->wsnt__TerminationTime[0] = endTime;
	wsnt__SubscribeResponse->__size = 0;
	wsnt__SubscribeResponse->__any = NULL;

	return SOAP_OK;

}


#else 
int  __ns4__Unsubscribe(struct soap* soap, struct _wsnt__Unsubscribe *wsnt__Unsubscribe, struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse )
{
 	int bClear = 0;

	if (!soap->header)
	{
		soap->header = (struct SOAP_ENV__Header *)soap_malloc_v2(soap, sizeof(struct SOAP_ENV__Header));
	}
	
	soap->header->wsa__To = NULL;
	soap->header->wsa__ReplyTo = NULL;	
	if (soap->path)
	{
		//printf("soap->header->to %s \n", soap->path); 

		int k = 0;
		char *p = strstr(soap->path, "onvif/Subscription?Idx=");
		if (p)
		{
			unsigned int id = atoi(p + strlen("onvif/Subscription?Idx="));
			printf("id is %d\n", id);
			if (id == 0)
			{
				soap->header->wsa__Action = soap_strdup(soap, "http://www.w3.org/2005/08/addressing/soap/fault"); 
	
				return send_soap_fault_msg(soap, SOAP_FAULT, "SOAP-ENV:Sender", "ter:InvalidArgVal",
					"ter:InvalidFilterFaultType", "get system config failed");			
			}

			pthread_mutex_lock(&soap->pConfig->gNotifyLock);
			for (k = 0; k < MAX_NOTIFY_CLIENT; k++)
			{
				//printf("id is %d %d\n", g_NotifyInfo.notify[k].id, id);
				if (soap->pConfig->g_NotifyInfo.notify[k].id == id)
				{
					soap->pConfig->g_NotifyInfo.notify[k].id = 0;
					soap->pConfig->g_NotifyInfo.notify[k].status = soap->pConfig->g_NotifyInfo.notify[k].ioStatus = 0;

					if (soap->pConfig->g_NotifyInfo.notify[k].endpoint)
					{
						free(soap->pConfig->g_NotifyInfo.notify[k].endpoint);
						soap->pConfig->g_NotifyInfo.notify[k].endpoint = NULL;
					}
					soap->pConfig->g_NotifyInfo.notify[k].endTime = 0;
					soap->pConfig->g_NotifyInfo.notify[k].bNoFirst = soap->pConfig->g_NotifyInfo.notify[k].bIoNoFirst = 0;

					--soap->pConfig->g_NotifyInfo.totalActiveClient;
					bClear = 1;

					printf("is clear!\n");
					break;
				}
			}
			pthread_mutex_unlock(&soap->pConfig->gNotifyLock);
		}
		else if ((p = strstr(soap->path, "onvif/Pull?Idx=")) != NULL)
		{
			unsigned int id = atoi(p + strlen("onvif/Pull?Idx="));
			printf("id is %d\n", id);
			if (id == 0)
			{
				soap->header->wsa__Action = soap_strdup(soap, "http://www.w3.org/2005/08/addressing/soap/fault"); 
	
				return send_soap_fault_msg(soap, SOAP_FAULT, "SOAP-ENV:Sender", "ter:InvalidArgVal",
					"ter:InvalidFilterFaultType", "get system config failed");			
			}
			
			time_t curTime = time(NULL);
			
			pthread_mutex_lock(&soap->pConfig->onvif_pullLock);
			for (k = 0; k < 10; k++)
			{
				//printf("pull id is %d %d\n", onvif_PullNode[k].id, id);
    			if (soap->pConfig->onvif_PullNode[k].id)
    			{
    				if (soap->pConfig->onvif_PullNode[k].endTime < curTime)
    				{
    					soap->pConfig->onvif_PullNode[k].id = 0;
    					soap->pConfig->onvif_PullNode[k].status = soap->pConfig->onvif_PullNode[k].ioStatus = 0;
    					soap->pConfig->onvif_PullNode[k].endTime = 0;
    					soap->pConfig->onvif_PullNode[k].bNoFirst = soap->pConfig->onvif_PullNode[k].bNoIoFirst = 0;
    				}
					else if (soap->pConfig->onvif_PullNode[k].id == id)
					{
    					bClear = 1;
    					soap->pConfig->onvif_PullNode[k].id = 0;
    					soap->pConfig->onvif_PullNode[k].status = soap->pConfig->onvif_PullNode[k].ioStatus = 0;
    					soap->pConfig->onvif_PullNode[k].endTime = 0;
    					soap->pConfig->onvif_PullNode[k].bNoFirst = soap->pConfig->onvif_PullNode[k].bNoIoFirst = 0;
    
    					printf("%d is clear!\n", id);
    					break;
					}
    			}
			}
			pthread_mutex_unlock(&soap->pConfig->onvif_pullLock);		
		}
	}

	//printf("bclear is %d\n", bClear);
	if (!bClear)
	{
		soap->header->wsa__Action = soap_strdup(soap, "http://www.w3.org/2005/08/addressing/soap/fault"); 
	
		return send_soap_fault_msg(soap, SOAP_FAULT, "SOAP-ENV:Sender", "ter:InvalidArgVal",
			"ter:InvalidFilterFaultType", "get system config failed");
	}
	
	soap->header->wsa__Action = soap_strdup(soap, "http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/UnsubscribeResponse");
	return SOAP_OK;
}

int  __ns5__Subscribe(struct soap* soap, struct _wsnt__Subscribe *wsnt__Subscribe, struct _wsnt__SubscribeResponse *wsnt__SubscribeResponse )
{
	//int ret;
	char *p = NULL;
	time_t endTime;
	
	if(!wsnt__Subscribe->ConsumerReference.Address)
	{
		return SOAP_ERR;
	}

	char *eventAddress = wsnt__Subscribe->ConsumerReference.Address;
	p = strstr(eventAddress,"//");
	if(p!= NULL)
	{
		//printf("subscribe request from:%s\n", eventAddress);
		eventAddress += strlen("http://"); //偏掉 http://
	}
	else
	{
		soap->header->wsa__Action = soap_strdup(soap, "http://www.w3.org/2005/08/addressing/soap/fault"); 

		return(send_soap_fault_msg(soap, 400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:InvalidFilterFaultType",
			"invalid filter fault"));	
	}

	if(!soap->header)
	{
		soap->header = (struct SOAP_ENV__Header *)soap_malloc_v2(soap, sizeof(struct SOAP_ENV__Header));
	}
	
	soap->header->wsa__To = NULL;
	soap->header->wsa__ReplyTo = NULL;

	int EnableMotionAlarm = 1;
	int EnableIOAlarm = 1;	
	if (wsnt__Subscribe->Filter && wsnt__Subscribe->Filter->__size)
	{
		CheckEventFilter(wsnt__Subscribe->Filter, &EnableMotionAlarm, &EnableIOAlarm);

		if (!EnableIOAlarm && !EnableMotionAlarm)
		{
			soap->header->wsa__Action = soap_strdup(soap, "http://www.w3.org/2005/08/addressing/soap/fault"); 

			return(send_soap_fault_msg(soap, 400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:InvalidFilterFaultType",
				"invalid filter fault"));
		}
	}

	time_t ts = 0;
	soap->pConfig->GetUtcTime(&ts, NULL);

	time_t rtcTermTime;
	time_t currentTime = time(NULL);

	if (wsnt__Subscribe->InitialTerminationTime != NULL)
	{
		p = strstr(wsnt__Subscribe->InitialTerminationTime, "Z");
		if (p)
		{
			struct tm endTerm = { 0 };
			
			if (6 != sscanf(wsnt__Subscribe->InitialTerminationTime, "%d-%d-%dT%d:%d:%dZ", &endTerm.tm_year, &endTerm.tm_mon, &endTerm.tm_mday, 
				&endTerm.tm_hour, &endTerm.tm_min, &endTerm.tm_sec ) ) 
			{
				soap->header->wsa__Action = soap_strdup(soap, "http://www.w3.org/2005/08/addressing/soap/fault");				
				return(send_soap_fault_msg(soap,400,
					"SOAP-ENV:Sender",
					"ter:InvalidArgVal",
					"ter:InvalidTerminationTime",
					"InvalidTerminationTime"));
			}

			endTerm.tm_year = (endTerm.tm_year > 1900 ? endTerm.tm_year - 1900 : 0) ;
			endTerm.tm_mon = (endTerm.tm_mon > 1 ? endTerm.tm_mon - 1 : 0 );			
			rtcTermTime = endTime = mktime(&endTerm);

			endTime = currentTime + (endTime - ts);
		}
		else 
		{				
			endTime = currentTime + periodtol(wsnt__Subscribe->InitialTerminationTime);
			rtcTermTime = ts + periodtol(wsnt__Subscribe->InitialTerminationTime);
		}

		ONVIF_DBP("wsnt__Subscribe->InitialTerminationTime %s\n", wsnt__Subscribe->InitialTerminationTime );
	}
	else
	{
		endTime = currentTime + 10;
		rtcTermTime = ts + 10;
	}

	//ret = 0;
	pthread_mutex_lock(&soap->pConfig->gNotifyLock);
	if (soap->pConfig->g_NotifyInfo.totalActiveClient >= MAX_NOTIFY_CLIENT)
	{
		pthread_mutex_unlock(&soap->pConfig->gNotifyLock);
			
		soap->header->wsa__Action = soap_strdup(soap, "http://www.w3.org/2005/08/addressing/soap/fault");
		return(send_soap_fault_msg(soap, 400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:OutOfMemory",
			"get system config failed"));
	}

	NOTIFY_ITEM *pTemp = NULL;	

	int k;
	while (1)
	{
		if (++soap->pConfig->SubscriptionIndex == 0)
		{
			soap->pConfig->SubscriptionIndex = 1;
		}
		
		for (k = 0;  k < MAX_NOTIFY_CLIENT; k++)
		{
			if (soap->pConfig->SubscriptionIndex == soap->pConfig->g_NotifyInfo.notify[k].id)
			{
				printf("subscibe id is %d notify id is %d\n", soap->pConfig->SubscriptionIndex, soap->pConfig->g_NotifyInfo.notify[k].id);
				break;
			}

			if (!pTemp && !soap->pConfig->g_NotifyInfo.notify[k].id)
			{
				pTemp = &soap->pConfig->g_NotifyInfo.notify[k];
				printf("get node is %d %X %X\n", k, pTemp->status, pTemp->ioStatus);
			}			
		}

		if (k >= MAX_NOTIFY_CLIENT)
		{
			printf("find a id %d\n", k);
			break;
		}
	}

	if (pTemp)
	{
		pTemp->id = soap->pConfig->SubscriptionIndex;
		printf("address is %s\n", wsnt__Subscribe->ConsumerReference.Address);
		pTemp->endpoint = strdup(wsnt__Subscribe->ConsumerReference.Address);
		pTemp->endTime = endTime;
		pTemp->bEnableMotion = EnableMotionAlarm;
		pTemp->bEnableIO = EnableIOAlarm;
		++soap->pConfig->g_NotifyInfo.totalActiveClient;
	}
	else
	{
		pthread_mutex_unlock(&soap->pConfig->gNotifyLock);
		
		soap->header->wsa__Action = soap_strdup(soap, "http://www.w3.org/2005/08/addressing/soap/fault");
		return(send_soap_fault_msg(soap, 400,
			"SOAP-ENV:Sender",
			"ter:InvalidArgVal",
			"ter:OutOfMemory",
			"get system config failed"));
	}
	pthread_mutex_unlock(&soap->pConfig->gNotifyLock);

	char addr[32];
	get_addr(soap, addr, 32);

	wsnt__SubscribeResponse->SubscriptionReference.Address = (char *)soap_malloc(soap, sizeof(char) * 200);
	sprintf(wsnt__SubscribeResponse->SubscriptionReference.Address, "http://%s:60987/onvif/Subscription?Idx=%d", addr, soap->pConfig->SubscriptionIndex);	

	//memset(&wsnt__SubscribeResponse->SubscriptionReference, 0, sizeof(struct wsa__EndpointReferenceType));
	wsnt__SubscribeResponse->wsnt__CurrentTime = (time_t *)soap_malloc(soap, sizeof(time_t));
	*wsnt__SubscribeResponse->wsnt__CurrentTime = ts;
	wsnt__SubscribeResponse->wsnt__TerminationTime = (time_t *)soap_malloc(soap, sizeof(time_t));	
	*wsnt__SubscribeResponse->wsnt__TerminationTime = rtcTermTime;

	//printf("address is %s\n", wsnt__SubscribeResponse->SubscriptionReference.Address);
	
	soap->header->wsa__Action = soap_strdup(soap, "http://docs.oasis-open.org/wsn/bw-2/NotificationProducer/SubscribeResponse");  
#if 0
	if (!soap->header->wsa__To)
	{
		soap->header->wsa__To = (char*)soap_malloc(soap, sizeof(char) * 128);				
	}

	if(TRUE == isWIFI)
	{
		sprintf(soap->header->wsa__To, "http://%s:60987/onvif/Events", wifi_config.dwNetIpAddr.csIpV4);
	}
	else
	{
		sprintf(soap->header->wsa__To, "http://%s:60987/onvif/Events", net_conf.stEtherNet[0].strIPAddr.csIpV4);
	}
#endif
	return SOAP_OK;
}
#endif

int  __ns5__GetCurrentMessage(struct soap* soap, struct _wsnt__GetCurrentMessage *wsnt__GetCurrentMessage, struct _wsnt__GetCurrentMessageResponse *wsnt__GetCurrentMessageResponse )
{ 	 

	ONVIF_DBP("");

 	return SOAP_OK; 

}
 
int  __ns6__Notify(struct soap* soap, struct _wsnt__Notify *wsnt__Notify )
{ 

	ONVIF_DBP("");
	
	char macaddr[6] = { 0 };
	char _HwId[INFO_LENGTH] = { 0 };

	wsnt__Notify->__sizeNotificationMessage = 1;
	wsnt__Notify->wsnt__NotificationMessage = (struct wsnt__NotificationMessageHolderType *)soap_malloc_v2(soap, sizeof(struct wsnt__NotificationMessageHolderType));
	wsnt__Notify->wsnt__NotificationMessage->wsnt__SubscriptionReference	= (struct wsa__EndpointReferenceType *)soap_malloc_v2(soap, sizeof(struct wsa__EndpointReferenceType));

	net_get_hwaddr(ETH_NAME, macaddr);
	sprintf(_HwId,"urn:uuid:1419d68a-1dd2-11b2-a105-%02X%02X%02X%02X%02X%02X",macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
	wsnt__Notify->wsnt__NotificationMessage->wsnt__SubscriptionReference->Address = (char *) soap_malloc_v2(soap, sizeof(char) * INFO_LENGTH);
	strcpy(wsnt__Notify->wsnt__NotificationMessage->wsnt__SubscriptionReference->Address, _HwId);
	wsnt__Notify->wsnt__NotificationMessage->wsnt__SubscriptionReference->ReferenceParameters = NULL;

	//wsnt__Notify->wsnt__NotificationMessage->wsnt__SubscriptionReference->Metadata = NULL;
	wsnt__Notify->wsnt__NotificationMessage->wsnt__SubscriptionReference->__size = 0;
	wsnt__Notify->wsnt__NotificationMessage->wsnt__SubscriptionReference->__any = NULL;

	wsnt__Notify->wsnt__NotificationMessage->wsnt__SubscriptionReference->__size = 1;
	wsnt__Notify->wsnt__NotificationMessage->wsnt__SubscriptionReference->__any = (char**)soap_malloc_v2(soap, sizeof(char*));
	wsnt__Notify->wsnt__NotificationMessage->wsnt__SubscriptionReference->__any[0] = (char*)soap_malloc_v2(soap, INFO_LENGTH);
	sprintf(wsnt__Notify->wsnt__NotificationMessage->wsnt__SubscriptionReference->__any[0], "%s", "__ns6__Notify");

	wsnt__Notify->wsnt__NotificationMessage->wsnt__SubscriptionReference->__anyAttribute = NULL;
	wsnt__Notify->wsnt__NotificationMessage->wsnt__Topic = NULL;
	wsnt__Notify->wsnt__NotificationMessage->wsnt__ProducerReference = NULL;

	return SOAP_OK;

}

int  __ns7__GetMessages(struct soap* soap, struct _wsnt__GetMessages *wsnt__GetMessages, struct _wsnt__GetMessagesResponse *wsnt__GetMessagesResponse )
{ 
	ONVIF_DBP("");

	return SOAP_OK;  
}
 
int  __ns7__DestroyPullPoint(struct soap* soap, struct _wsnt__DestroyPullPoint *wsnt__DestroyPullPoint, struct _wsnt__DestroyPullPointResponse *wsnt__DestroyPullPointResponse )
{ 
	ONVIF_DBP("");

	return SOAP_OK; 
}

int  __ns7__Notify(struct soap* soap, struct _wsnt__Notify *wsnt__Notify )
{ 

	ONVIF_DBP("");
	
	return SOAP_OK; 

}
 
int  __ns8__CreatePullPoint(struct soap* soap, struct _wsnt__CreatePullPoint *wsnt__CreatePullPoint, struct _wsnt__CreatePullPointResponse *wsnt__CreatePullPointResponse )
{  
	ONVIF_DBP("");

	return SOAP_OK;

}


int  __ns10__Renew(struct soap* soap , struct _wsnt__Renew *wsnt__Renew, struct _wsnt__RenewResponse *wsnt__RenewResponse )
{ 

	ONVIF_DBP("");
	
	time_t time_tm;
	time_t termtime_tm;
	time_t timeout;

	timeout = periodtol(wsnt__Renew->TerminationTime);
	time_tm = time(NULL);
	termtime_tm = time_tm + timeout;
	wsnt__RenewResponse->wsnt__TerminationTime = termtime_tm;
	wsnt__RenewResponse->wsnt__CurrentTime = (time_t *) soap_malloc_v2(soap, sizeof(time_t)); 
	wsnt__RenewResponse->wsnt__CurrentTime[0] = time_tm; 
	wsnt__RenewResponse->__size = 0;
	wsnt__RenewResponse->__any = NULL;

	return SOAP_OK;

}

int  __ns10__Unsubscribe(struct soap* soap, struct _wsnt__Unsubscribe *wsnt__Unsubscribe, struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse )
{ 

	ONVIF_DBP("");


#if 0
	g_time_out = TERMINATE_THREAD;
	t_out = TERMINATE_THREAD;
	
	wsnt__UnsubscribeResponse->__size = 0;
	wsnt__UnsubscribeResponse->__any = NULL;

#endif

	return SOAP_OK;
	
}

int __ns10__GetSupportedRules(struct soap* soap, struct _tan10__GetSupportedRules *tan10__GetSupportedRules, struct _tan10__GetSupportedRulesResponse *tan10__GetSupportedRulesResponse)
{ 
	ONVIF_DBP("");
	

	return SOAP_OK; 

}

int  __ns10__CreateRule(struct soap* soap, struct _tan10__CreateRules *tan10__CreateRules, char **tan10__CreateRulesResponse)
{ 
	ONVIF_DBP("");

	return SOAP_OK; 

}

int  __ns10__DeleteRule(struct soap* soap, struct _tan10__DeleteRules *tan10__DeleteRules, char **tan10__DeleteRulesResponse)
{ 
	ONVIF_DBP("");	

	return SOAP_OK; 

}

int  __ns10__GetRules(struct soap* soap, struct _tan10__GetRules *tan10__GetRules, struct _tan10__GetRulesResponse *tan10__GetRulesResponse)
{ 

	ONVIF_DBP("");

	return SOAP_OK; 

}

int __ns10__ModifyRule(struct soap* soap, struct _tan10__ModifyRules *tan10__ModifyRules, char **tan10__ModifyRulesResponse)
{ 

	ONVIF_DBP("");

	return SOAP_OK; 

}


int __ns11__GetSupportedAnalytics(struct soap* soap, struct _tan10__GetSupportedAnalyticsModules *tan10__GetSupportedAnalyticsModules, struct _tan10__GetSupportedAnalyticsModulesResponse *tan10__GetSupportedAnalyticsModulesResponse)
{ 

	ONVIF_DBP("");

	return SOAP_OK; 

}	

int __ns11__CreateAnalytics(struct soap* soap, struct _tan10__CreateAnalyticsModules *tan10__CreateAnalyticsModules, char **tan10__CreateAnalyticsModulesResponse)
{ 

	ONVIF_DBP("");

	return SOAP_OK; 

}

int __ns11__DeleteAnalytics(struct soap* soap, struct _tan10__DeleteAnalyticsModules *tan10__DeleteAnalyticsModules, char **tan10__DeleteAnalyticsModulesResponse)
{ 

	ONVIF_DBP("");
	
	return SOAP_OK; 

}

int  __ns11__GetAnalytics(struct soap* soap, struct _tan10__GetAnalyticsModules *tan10__GetAnalyticsModules, struct _tan10__GetAnalyticsModulesResponse *tan10__GetAnalyticsModulesResponse)
{ 

	ONVIF_DBP("");

	return SOAP_OK; 

}

int  __ns11__ModifyAnalytics(struct soap* soap, struct _tan10__ModifyAnalyticsModules *tan10__ModifyAnalyticsModules, char **tan10__ModifyAnalyticsModulesResponse)
{ 

	ONVIF_DBP("");

	return SOAP_OK; 

}



 int  __ns9__PauseSubscription(struct soap* soap, struct _wsnt__PauseSubscription *wsnt__PauseSubscription, struct _wsnt__PauseSubscriptionResponse *wsnt__PauseSubscriptionResponse )
{ 

	ONVIF_DBP("");

	return SOAP_OK; 
	
}

 int  __ns9__ResumeSubscription(struct soap* soap, struct _wsnt__ResumeSubscription *wsnt__ResumeSubscription, struct _wsnt__ResumeSubscriptionResponse *wsnt__ResumeSubscriptionResponse )
{ 

	ONVIF_DBP("");

	return SOAP_OK; 
	
}

int  __ns12__GetSupportedRules(struct soap* soap, struct _tan__GetSupportedRules *tan__GetSupportedRules, struct _tan__GetSupportedRulesResponse *tan__GetSupportedRulesResponse )
{
	int ret;
	int chn = 0;
	if(!strncmp(tan__GetSupportedRules->ConfigurationToken, "VideoAnalyticsToken", strlen("VideoAnalyticsToken")))
	{
		ret = sscanf(tan__GetSupportedRules->ConfigurationToken, "VideoAnalyticsToken%d", &chn);
		if(ret != 1)
		{
 			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
				"ter:NoConfig", "CreateRules is not supported");		
		}
	}
	else
	{
 		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
			"ter:NoConfig", "CreateRules is not supported");	
	}

	tan__GetSupportedRulesResponse->SupportedRules = (struct tt__SupportedRules *)soap_malloc_v2(soap, sizeof(struct tt__SupportedRules));
	
	tan__GetSupportedRulesResponse->SupportedRules->__sizeRuleContentSchemaLocation = 1;
	tan__GetSupportedRulesResponse->SupportedRules->RuleContentSchemaLocation = (char **)soap_malloc(soap, sizeof(char *));
	*tan__GetSupportedRulesResponse->SupportedRules->RuleContentSchemaLocation = soap_strdup(soap, "http://www.w3.org/2001/XMLSchema");

	tan__GetSupportedRulesResponse->SupportedRules->__sizeRuleDescription = 1;
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription = (struct tt__ConfigDescription *)soap_malloc_v2(soap, sizeof(struct tt__ConfigDescription));

	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Name = soap_strdup(soap, "tt:CellMotionDetector");
	
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Parameters = (struct tt__ItemListDescription *)soap_malloc_v2(soap, sizeof(struct tt__ItemListDescription));

	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Parameters->__sizeSimpleItemDescription = 4;
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Parameters->SimpleItemDescription = (struct _tt__ItemListDescription_SimpleItemDescription *)soap_malloc_v2(soap, 4 * sizeof(struct _tt__ItemListDescription_SimpleItemDescription));
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Parameters->SimpleItemDescription[0].Name = soap_strdup(soap, "MinCount");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Parameters->SimpleItemDescription[0].Type = soap_strdup(soap, "xs:integer");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Parameters->SimpleItemDescription[1].Name = soap_strdup(soap, "AlarmOnDelay");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Parameters->SimpleItemDescription[1].Type = soap_strdup(soap, "xs:integer");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Parameters->SimpleItemDescription[2].Name = soap_strdup(soap, "AlarmOffDelay");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Parameters->SimpleItemDescription[2].Type = soap_strdup(soap, "xs:integer");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Parameters->SimpleItemDescription[3].Name = soap_strdup(soap, "ActiveCells");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Parameters->SimpleItemDescription[3].Type = soap_strdup(soap, "xs:base64Binary");	

	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->__sizeMessages = 1;
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages = (struct _tt__ConfigDescription_Messages *)soap_malloc_v2(soap, sizeof(struct _tt__ConfigDescription_Messages));

	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->IsProperty = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->IsProperty = xsd__boolean__true_;

	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Source = (struct tt__ItemListDescription *)soap_malloc_v2(soap, sizeof(struct tt__ItemListDescription));

	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Source->__sizeSimpleItemDescription = 3;
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Source->SimpleItemDescription = (struct _tt__ItemListDescription_SimpleItemDescription *)soap_malloc_v2(soap, 3 * sizeof(struct _tt__ItemListDescription_SimpleItemDescription));
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Source->SimpleItemDescription[0].Name = soap_strdup(soap, "VideoSourceConfigurationToken");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Source->SimpleItemDescription[0].Type = soap_strdup(soap, "tt:ReferenceToken");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Source->SimpleItemDescription[1].Name = soap_strdup(soap, "VideoAnalyticsConfigurationToken");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Source->SimpleItemDescription[1].Type = soap_strdup(soap, "tt:ReferenceToken");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Source->SimpleItemDescription[2].Name = soap_strdup(soap, "Rule");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Source->SimpleItemDescription[2].Type = soap_strdup(soap, "xs:string");

	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Data = (struct tt__ItemListDescription *)soap_malloc_v2(soap, sizeof(struct tt__ItemListDescription));

	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Data->__sizeSimpleItemDescription = 1;
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Data->SimpleItemDescription = (struct _tt__ItemListDescription_SimpleItemDescription *)soap_malloc_v2(soap, sizeof(struct _tt__ItemListDescription_SimpleItemDescription));
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Data->SimpleItemDescription->Name = soap_strdup(soap, "IsMotion");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->Data->SimpleItemDescription->Type = soap_strdup(soap, "xs:boolean");
	tan__GetSupportedRulesResponse->SupportedRules->RuleDescription->Messages->ParentTopic = soap_strdup(soap, ONVIF_EVENT_MOTION);

	return SOAP_OK;
}
 int  __ns12__CreateRules(struct soap* soap, struct _tan__CreateRules *tan__CreateRules, struct _tan__CreateRulesResponse *tan__CreateRulesResponse )
 { 

 	ONVIF_DBP("");
	
 	send_soap_fault_msg(soap, 400,
				"SOAP-ENV:Sender",
				"ter:InvalidArgVal",
				"ter:InvalidArgVal",
				"CreateRules is not supported");

 	return SOAP_FAULT;  

 }

 int  __ns12__DeleteRules(struct soap* soap, struct _tan__DeleteRules *tan__DeleteRules, struct _tan__DeleteRulesResponse *tan__DeleteRulesResponse )
 {

 	ONVIF_DBP("");

	return SOAP_OK; 
}

int  __ns12__GetRules(struct soap* soap, struct _tan__GetRules *tan__GetRules, struct _tan__GetRulesResponse *tan__GetRulesResponse )
{
	printf("Getrules token is %s\n", tan__GetRules->ConfigurationToken);
	int ret;
	int chn = 0;
	if(!strncmp(tan__GetRules->ConfigurationToken, "VideoAnalyticsToken", strlen("VideoAnalyticsToken")))
	{
		ret = sscanf(tan__GetRules->ConfigurationToken, "VideoAnalyticsToken%d", &chn);
		if(ret != 1)
		{
 			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
				"ter:NoConfig", "CreateRules is not supported");		
		}
	}
	else
	{
 		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
			"ter:NoConfig", "CreateRules is not supported");	
	}

	QMAPI_NET_CHANNEL_MOTION_DETECT motion_config = {0};
	motion_config.dwChannel = chn;
	motion_config.dwSize = sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT);
	ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, chn, &motion_config, sizeof(motion_config));
	if(ret)
	{
		return SOAP_ERR;
	}

	unsigned char region[50];
	pack_motion_cells(motion_config.byMotionArea, region);
	
	//printf("*************************************\n");	
	int i;
	for ( i = 0; i < 50; i++)
	{
		region[i] = ReverseByte(region[i]);
		//printf("%X ", region[i]);
	}
	//printf("*************************************\n");

	unsigned char packRegion[50] = {0};	
	unsigned int outlen = packbits(region, packRegion, 50);
	char *outputBuffer = (char *)soap_malloc_v2(soap, sizeof(char) * 100);	
	soap_s2base64(soap, packRegion, outputBuffer, outlen);

	tan__GetRulesResponse->__sizeRule = 1;
	tan__GetRulesResponse->Rule = (struct tt__Config *)soap_malloc_v2(soap, sizeof(struct tt__Config));
	
	tan__GetRulesResponse->Rule->Name = soap_strdup(soap, "MyMotionDetectorRule");
	tan__GetRulesResponse->Rule->Type = soap_strdup(soap, "tt:CellMotionDetector");
	
	tan__GetRulesResponse->Rule->Parameters = (struct tt__ItemList *)soap_malloc_v2(soap, sizeof(struct tt__ItemList));
	
	tan__GetRulesResponse->Rule->Parameters->__sizeSimpleItem = 4;
	tan__GetRulesResponse->Rule->Parameters->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc(soap, 4 * sizeof(struct _tt__ItemList_SimpleItem));
	tan__GetRulesResponse->Rule->Parameters->SimpleItem[0].Name = soap_strdup(soap, "MinCount");
	tan__GetRulesResponse->Rule->Parameters->SimpleItem[0].Value = soap_strdup(soap, "2");
	tan__GetRulesResponse->Rule->Parameters->SimpleItem[1].Name = soap_strdup(soap, "AlarmOnDelay");
	tan__GetRulesResponse->Rule->Parameters->SimpleItem[1].Value = soap_strdup(soap, "1000");
	tan__GetRulesResponse->Rule->Parameters->SimpleItem[2].Name = soap_strdup(soap, "AlarmOffDelay");
	tan__GetRulesResponse->Rule->Parameters->SimpleItem[2].Value = soap_strdup(soap, "1000");	
	tan__GetRulesResponse->Rule->Parameters->SimpleItem[3].Name = soap_strdup(soap, "ActiveCells");
	tan__GetRulesResponse->Rule->Parameters->SimpleItem[3].Value = outputBuffer;
	
	return SOAP_OK;
} 

int  __ns12__ModifyRules(struct soap* soap, struct _tan__ModifyRules *tan__ModifyRules, struct _tan__ModifyRulesResponse *tan__ModifyRulesResponse )
{
	int ret;
	int chn = 0;
	
	if (!strncmp(tan__ModifyRules->ConfigurationToken, "VideoAnalyticsToken", strlen("VideoAnalyticsToken")))
	{
		ret = sscanf(tan__ModifyRules->ConfigurationToken, "VideoAnalyticsToken%d", &chn);
		if (ret != 1)
		{
 			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
				"ter:NoConfig", "CreateRules is not supported");		
		}
	}
	else
	{
 		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
			"ter:NoConfig", "CreateRules is not supported");	
	}

	QMAPI_NET_CHANNEL_MOTION_DETECT motion_config = {0};
	motion_config.dwChannel = chn;
	motion_config.dwSize = sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT);
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, chn, &motion_config, sizeof(motion_config)))
	{
		return SOAP_ERR;
	}

	if (tan__ModifyRules->__sizeRule && tan__ModifyRules->Rule)
	{
		if (strcmp(tan__ModifyRules->Rule->Name, "MyMotionDetectorRule"))
		{
 			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
				"ter:InvalidRule", "No such rule");
		}

		if (strcmp(tan__ModifyRules->Rule->Type, "tt:CellMotionDetector"))
		{
 			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
				"ter:InvalidRule", "No such rule");		
		}

		if (tan__ModifyRules->Rule->Parameters && tan__ModifyRules->Rule->Parameters->__sizeSimpleItem)
		{
			int i;
			for (i = 0; i < tan__ModifyRules->Rule->Parameters->__sizeSimpleItem; i++)
			{
				if (!strcmp(tan__ModifyRules->Rule->Parameters->SimpleItem[i].Name, "ActiveCells"))
				{

					printf("base64 encode string is %s\n", tan__ModifyRules->Rule->Parameters->SimpleItem[i].Value);
				
					int packlen = 0;
					int len = 0;
					char buffer[200] = {0};
					unsigned char region[50] = {0};
					
					soap_base642s(soap, tan__ModifyRules->Rule->Parameters->SimpleItem[i].Value, buffer, strlen(tan__ModifyRules->Rule->Parameters->SimpleItem[i].Value), &len);
					if (!len || ((packlen = unpackbits(region, (unsigned char *)buffer, 50, len) != 50)))
					{
 						return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
							"ter:InvalidRule", "No such rule");					
					}

					int n;
					for (n = 0; n < 50; n++)
					{
						if (region[n] && region[n] != 0xFF)
						{
							region[n] = ReverseByte(region[n]);
						}
						printf("%02X ", region[n]);
					}
					printf("***************************\n");

					unpack_motion_cells(region, motion_config.byMotionArea);
					
					QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_MOTIONCFG, chn, &motion_config, sizeof(motion_config));
					QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, chn, NULL, 0);
					break;
				}
			}
		}
		else
		{
 			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
				"ter:InvalidRule", "No such rule");				
		}
	}
	 
	return SOAP_OK;  
} 


 int __ns13__GetServiceCapabilities(struct soap* soap, struct _tan__GetServiceCapabilities *tan__GetServiceCapabilities, struct _tan__GetServiceCapabilitiesResponse *tan__GetServiceCapabilitiesResponse)
	 { 
	 
		 ONVIF_DBP("");
	 
		 return SOAP_OK;  
	 
	 } 


int  __ns13__GetSupportedAnalyticsModules(struct soap* soap, struct _tan__GetSupportedAnalyticsModules *tan__GetSupportedAnalyticsModules, struct _tan__GetSupportedAnalyticsModulesResponse *tan__GetSupportedAnalyticsModulesResponse )
{ 
	int ret;
	int chn = 0;
	
	if(!strncmp(tan__GetSupportedAnalyticsModules->ConfigurationToken, "VideoAnalyticsToken", strlen("VideoAnalyticsToken")))
	{
		ret = sscanf(tan__GetSupportedAnalyticsModules->ConfigurationToken, "VideoAnalyticsToken%d", &chn);
		if(ret != 1)
		{
 			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
				"ter:NoConfig", "CreateRules is not supported");		
		}
	}
	else
	{
 		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
			"ter:NoConfig", "CreateRules is not supported");	
	}

	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules = (struct tt__SupportedAnalyticsModules *)soap_malloc_v2(soap, sizeof(struct tt__SupportedAnalyticsModules));

	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->__sizeAnalyticsModuleContentSchemaLocation = 1;
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleContentSchemaLocation = (char **)soap_malloc(soap, sizeof(char *));
	*tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleContentSchemaLocation = soap_strdup(soap, "http://www.w3.org/2001/XMLSchema");

	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->__sizeAnalyticsModuleDescription = 1;
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription = (struct tt__ConfigDescription *)soap_malloc_v2(soap, sizeof(struct tt__ConfigDescription));
	
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Name = soap_strdup(soap, "tt:CellMotionEngine");
	
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Parameters = (struct tt__ItemListDescription *)soap_malloc_v2(soap, sizeof(struct tt__ItemListDescription));
	
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Parameters->__sizeSimpleItemDescription = 1;
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Parameters->SimpleItemDescription = (struct _tt__ItemListDescription_SimpleItemDescription *)soap_malloc_v2(soap, sizeof(struct _tt__ItemListDescription_SimpleItemDescription));
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Parameters->SimpleItemDescription->Name = soap_strdup(soap, "Sensitivity");
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Parameters->SimpleItemDescription->Type = soap_strdup(soap, "xs:integer");
	
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Parameters->__sizeElementItemDescription = 1;
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Parameters->ElementItemDescription = (struct _tt__ItemListDescription_ElementItemDescription *)soap_malloc_v2(soap, sizeof(struct _tt__ItemListDescription_ElementItemDescription));
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Parameters->ElementItemDescription->Name = soap_strdup(soap, "Layout");
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Parameters->ElementItemDescription->Type = soap_strdup(soap, "tt:CellLayout");
	
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->__sizeMessages = 1;
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages = (struct _tt__ConfigDescription_Messages *)soap_malloc_v2(soap, sizeof(struct _tt__ConfigDescription_Messages));
	
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->IsProperty = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->IsProperty = xsd__boolean__true_;
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Source = (struct tt__ItemListDescription *)soap_malloc_v2(soap, sizeof(struct tt__ItemListDescription));
	
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Source->__sizeSimpleItemDescription = 3;
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Source->SimpleItemDescription = (struct _tt__ItemListDescription_SimpleItemDescription *)soap_malloc_v2(soap, 3 * sizeof(struct _tt__ItemListDescription_SimpleItemDescription));
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Source->SimpleItemDescription[0].Name = soap_strdup(soap, "VideoSourceConfigurationToken");
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Source->SimpleItemDescription[0].Type = soap_strdup(soap, "tt:ReferenceToken");
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Source->SimpleItemDescription[1].Name = soap_strdup(soap, "VideoAnalyticsConfigurationToken");
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Source->SimpleItemDescription[1].Type = soap_strdup(soap, "tt:ReferenceToken");
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Source->SimpleItemDescription[2].Name = soap_strdup(soap, "Rule");
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Source->SimpleItemDescription[2].Type = soap_strdup(soap, "xs:string");
	
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Data = (struct tt__ItemListDescription *)soap_malloc_v2(soap, sizeof(struct tt__ItemListDescription));
	
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Data->__sizeSimpleItemDescription = 1;
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Data->SimpleItemDescription = (struct _tt__ItemListDescription_SimpleItemDescription *)soap_malloc_v2(soap, sizeof(struct _tt__ItemListDescription_SimpleItemDescription));
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Data->SimpleItemDescription->Name = soap_strdup(soap, "IsMotion");
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->Data->SimpleItemDescription->Type = soap_strdup(soap, "xs:boolean");
	tan__GetSupportedAnalyticsModulesResponse->SupportedAnalyticsModules->AnalyticsModuleDescription->Messages->ParentTopic = soap_strdup(soap, ONVIF_EVENT_MOTION);
	return SOAP_OK;
}

 int  __ns13__CreateAnalyticsModules(struct soap* soap, struct _tan__CreateAnalyticsModules *tan__CreateAnalyticsModules, struct _tan__CreateAnalyticsModulesResponse *tan__CreateAnalyticsModulesResponse )
	 { 
	 
		 ONVIF_DBP("");
	 
		 return SOAP_OK;  
	 
	 } 


 int  __ns13__DeleteAnalyticsModules(struct soap* soap, struct _tan__DeleteAnalyticsModules *tan__DeleteAnalyticsModules, struct _tan__DeleteAnalyticsModulesResponse *tan__DeleteAnalyticsModulesResponse )
 { 

	ONVIF_DBP("");

 	return SOAP_OK;  

} 

int  __ns13__GetAnalyticsModules(struct soap* soap, struct _tan__GetAnalyticsModules *tan__GetAnalyticsModules, struct _tan__GetAnalyticsModulesResponse *tan__GetAnalyticsModulesResponse )
{
	int ret;
	int chn = 0;
	
	if(!strncmp(tan__GetAnalyticsModules->ConfigurationToken, "VideoAnalyticsToken", strlen("VideoAnalyticsToken")))
	{
		ret = sscanf(tan__GetAnalyticsModules->ConfigurationToken, "VideoAnalyticsToken%d", &chn);
		if(ret != 1)
		{
 			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
				"ter:NoConfig", "CreateRules is not supported");		
		}
	}
	else
	{
 		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
			"ter:NoConfig", "CreateRules is not supported");	
	}

	QMAPI_NET_CHANNEL_MOTION_DETECT motion_config = {0};
	motion_config.dwChannel = chn;
	motion_config.dwSize = sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT);
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, chn, &motion_config, sizeof(motion_config)))
	{
		return SOAP_ERR;
	}

	int level;
	if (motion_config.bEnable == 0)
	{
		level = 0;
	}
	else
	{
		level= 100 - motion_config.dwSensitive;
	}
	
	tan__GetAnalyticsModulesResponse->__sizeAnalyticsModule = 1;
	tan__GetAnalyticsModulesResponse->AnalyticsModule = (struct tt__Config *)soap_malloc_v2(soap, sizeof(struct tt__Config));
	
	tan__GetAnalyticsModulesResponse->AnalyticsModule->Name = soap_strdup(soap, "MyCellMotionModule");
	tan__GetAnalyticsModulesResponse->AnalyticsModule->Type = soap_strdup(soap, "tt:CellMotionEngine");
	
	tan__GetAnalyticsModulesResponse->AnalyticsModule->Parameters = (struct tt__ItemList *)soap_malloc_v2(soap, sizeof(struct tt__ItemList));
	
	tan__GetAnalyticsModulesResponse->AnalyticsModule->Parameters->__sizeSimpleItem = 1;
	tan__GetAnalyticsModulesResponse->AnalyticsModule->Parameters->SimpleItem = (struct _tt__ItemList_SimpleItem *)soap_malloc_v2(soap, sizeof(struct _tt__ItemList_SimpleItem));
	tan__GetAnalyticsModulesResponse->AnalyticsModule->Parameters->SimpleItem->Name = soap_strdup(soap, "Sensitivity");
	tan__GetAnalyticsModulesResponse->AnalyticsModule->Parameters->SimpleItem->Value = (char *)soap_malloc(soap, LEN_24_SIZE * sizeof(char));
	sprintf(tan__GetAnalyticsModulesResponse->AnalyticsModule->Parameters->SimpleItem->Value, "%d", level);
	
	tan__GetAnalyticsModulesResponse->AnalyticsModule->Parameters->__sizeElementItem = 1;
	tan__GetAnalyticsModulesResponse->AnalyticsModule->Parameters->ElementItem= (struct _tt__ItemList_ElementItem *)soap_malloc_v2(soap, sizeof(struct _tt__ItemList_ElementItem));
	tan__GetAnalyticsModulesResponse->AnalyticsModule->Parameters->ElementItem->Name = soap_strdup(soap, "Layout");
	tan__GetAnalyticsModulesResponse->AnalyticsModule->Parameters->ElementItem->__any = (char *)soap_malloc(soap, sizeof(char) * LEN_256_SIZE);
	sprintf(tan__GetAnalyticsModulesResponse->AnalyticsModule->Parameters->ElementItem->__any, "<tt:CellLayout Columns=\"22\" Rows=\"18\"><tt:Transformation><tt:Translate x=\"-1.000000\" y=\"-1.000000\"/>"
			"<tt:Scale x=\"0.001953\" y=\"0.002604\"/></tt:Transformation></tt:CellLayout>");

	return SOAP_OK;
} 

int  __ns13__ModifyAnalyticsModules(struct soap* soap, struct _tan__ModifyAnalyticsModules *tan__ModifyAnalyticsModules, struct _tan__ModifyAnalyticsModulesResponse *tan__ModifyAnalyticsModulesResponse )
{
	int ret;
	int chn = 0;
	
	if(!strncmp(tan__ModifyAnalyticsModules->ConfigurationToken, "VideoAnalyticsToken", strlen("VideoAnalyticsToken")))
	{
		ret = sscanf(tan__ModifyAnalyticsModules->ConfigurationToken, "VideoAnalyticsToken%d", &chn);
		if(ret != 1)
		{
 			return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
				"ter:NoConfig", "CreateRules is not supported");		
		}
	}
	else
	{
 		return send_soap_fault_msg(soap, 400, "SOAP-ENV:Sender", "ter:InvalidArgVal",
			"ter:NoConfig", "CreateRules is not supported");	
	}

	QMAPI_NET_CHANNEL_MOTION_DETECT motion_config = {0};
	motion_config.dwChannel = chn;
	motion_config.dwSize = sizeof(QMAPI_NET_CHANNEL_MOTION_DETECT);
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_MOTIONCFG, chn, &motion_config, sizeof(motion_config)))
	{
		return SOAP_ERR;
	}

	int i;
	for (i = 0; i < tan__ModifyAnalyticsModules->__sizeAnalyticsModule; i++)
	{
		if (!tan__ModifyAnalyticsModules->AnalyticsModule[i].Name || !tan__ModifyAnalyticsModules->AnalyticsModule[i].Type)
		{
			continue;
		}
		
		if (!strcmp(tan__ModifyAnalyticsModules->AnalyticsModule[i].Name, "MyCellMotionModule") && !strcmp(tan__ModifyAnalyticsModules->AnalyticsModule[i].Type, "tt:CellMotionEngine")
			&& tan__ModifyAnalyticsModules->AnalyticsModule[i].Parameters)
		{
			int j;
			for  (j = 0; j < tan__ModifyAnalyticsModules->AnalyticsModule[i].Parameters->__sizeSimpleItem; j++)
			{
				if (!strcmp(tan__ModifyAnalyticsModules->AnalyticsModule[i].Parameters->SimpleItem[j].Name, "Sensitivity"))
				{
					int value = atoi(tan__ModifyAnalyticsModules->AnalyticsModule[i].Parameters->SimpleItem[j].Value);
					printf("Value = %d\n", value);
					if (value)
					{
						motion_config.dwSensitive = 100 - value;
						motion_config.bEnable = 1;
					}
					else
					{
						motion_config.bEnable = 0;
					}

					printf("Set motion flag is %lu\n", motion_config.dwSensitive);
					if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_MOTIONCFG, chn, &motion_config, sizeof(motion_config)))
						printf("save motion para failed!\n");
					QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, chn, NULL, 0);
					return SOAP_OK;
				}
			}
		}
	}

 	return SOAP_OK;  

} 

int  __tds__GetServices(struct soap* soap, struct _tds__GetServices *tds__GetServices, struct _tds__GetServicesResponse *tds__GetServicesResponse )
{
	int isWIFI;
	QMAPI_NET_NETWORK_CFG net_conf;
	QMAPI_NET_WIFI_CONFIG wifi_cfg;

	memset(&net_conf, 0, sizeof(QMAPI_NET_NETWORK_CFG));
	memset(&wifi_cfg, 0, sizeof(QMAPI_NET_WIFI_CONFIG));
	get_netcfg_fromsock(soap, &net_conf, &wifi_cfg, &isWIFI);

	int webport = net_conf.wHttpPort;
	char *ip = net_conf.stEtherNet[0].strIPAddr.csIpV4;

	if(TRUE == isWIFI)
	{
		ip = wifi_cfg.dwNetIpAddr.csIpV4;
	}
	
	tds__GetServicesResponse->__sizeService = 7;
	tds__GetServicesResponse->Service = (struct tds__Service *)soap_malloc_v2(soap, sizeof(struct tds__Service)*7);

	int i;
	for (i = 0; i < 7; i++)
	{
		tds__GetServicesResponse->Service[i].XAddr = (char *)soap_malloc(soap, LEN_64_SIZE);
		tds__GetServicesResponse->Service[i].Version = (struct tt__OnvifVersion *)soap_malloc_v2(soap, sizeof(struct tt__OnvifVersion));
	}

	tds__GetServicesResponse->Service[0].Namespace = soap_strdup(soap, "http://www.onvif.org/ver10/device/wsdl");
	tds__GetServicesResponse->Service[1].Namespace = soap_strdup(soap, "http://www.onvif.org/ver10/media/wsdl");
	tds__GetServicesResponse->Service[2].Namespace = soap_strdup(soap, "http://www.onvif.org/ver10/events/wsdl");
	tds__GetServicesResponse->Service[3].Namespace = soap_strdup(soap, "http://www.onvif.org/ver20/ptz/wsdl");
	tds__GetServicesResponse->Service[4].Namespace = soap_strdup(soap, "http://www.onvif.org/ver20/imaging/wsdl");
	tds__GetServicesResponse->Service[5].Namespace = soap_strdup(soap, "http://www.onvif.org/ver10/deviceIO/wsdl");
	tds__GetServicesResponse->Service[6].Namespace = soap_strdup(soap, "http://www.onvif.org/ver20/analytics/wsdl");
	
	char addr[32];
	sprintf(addr, "http://%s:%d/onvif", ip, webport);
	sprintf(tds__GetServicesResponse->Service[0].XAddr, "%s/device_service", addr);
	sprintf(tds__GetServicesResponse->Service[1].XAddr, "%s/Media", addr);
	sprintf(tds__GetServicesResponse->Service[2].XAddr, "%s/onvif/Events", addr);
	sprintf(tds__GetServicesResponse->Service[3].XAddr, "%s/onvif/PTZ", addr);
	sprintf(tds__GetServicesResponse->Service[4].XAddr, "%s/Imaging", addr);
	sprintf(tds__GetServicesResponse->Service[5].XAddr, "%s/DeviceIO", addr);
	sprintf(tds__GetServicesResponse->Service[6].XAddr, "%s/Analytics", addr);

	tds__GetServicesResponse->Service[0].Version->Major = 2;
	tds__GetServicesResponse->Service[0].Version->Minor = 40;
	tds__GetServicesResponse->Service[1].Version->Major = 2;
	tds__GetServicesResponse->Service[1].Version->Minor = 40;
	tds__GetServicesResponse->Service[2].Version->Major = 2;
	tds__GetServicesResponse->Service[2].Version->Minor = 40;
	tds__GetServicesResponse->Service[3].Version->Major = 2;
	tds__GetServicesResponse->Service[3].Version->Minor = 40;
	tds__GetServicesResponse->Service[4].Version->Major = 2;
	tds__GetServicesResponse->Service[4].Version->Minor = 40;
	tds__GetServicesResponse->Service[5].Version->Major = 2;
	tds__GetServicesResponse->Service[5].Version->Minor = 21;
	tds__GetServicesResponse->Service[6].Version->Major = 2;
	tds__GetServicesResponse->Service[6].Version->Minor = 20;

	if (tds__GetServices->IncludeCapability)
	{
		tds__GetServicesResponse->Service[0].Capabilities = (struct _tds__Service_Capabilities *)soap_malloc_v2(soap, sizeof(struct _tds__Service_Capabilities));
		tds__GetServicesResponse->Service[0].Capabilities->__any = soap_strdup(soap, "<tds:Capabilities><tds:Network NTP=\"1\" HostnameFromDHCP=\"false\" Dot11Configuration=\"false\" "
			"DynDNS=\"true\" IPVersion6=\"false\" ZeroConfiguration=\"false\" IPFilter=\"false\"></tds:Network><tds:Security RELToken=\"false\" HttpDigest=\"false\" "
			"UsernameToken=\"true\" KerberosToken=\"false\" SAMLToken=\"false\" X.509Token=\"false\" RemoteUserHandling=\"false\" Dot1X=\"false\" DefaultAccessPolicy=\"false\" "
			"AccessPolicyConfig=\"false\" OnboardKeyGeneration=\"false\" TLS1.2=\"false\" TLS1.1=\"false\" TLS1.0=\"false\"></tds:Security>"
			"<tds:System HttpSupportInformation=\"false\" HttpSystemLogging=\"true\" HttpSystemBackup=\"true\" HttpFirmwareUpgrade=\"true\" FirmwareUpgrade=\"false\" "
			"SystemLogging=\"false\" SystemBackup=\"false\" RemoteDiscovery=\"false\" DiscoveryBye=\"true\" DiscoveryResolve=\"false\"></tds:System></tds:Capabilities>");

		tds__GetServicesResponse->Service[1].Capabilities = (struct _tds__Service_Capabilities *)soap_malloc_v2(soap, sizeof(struct _tds__Service_Capabilities));
		tds__GetServicesResponse->Service[1].Capabilities->__any = (char *)soap_malloc(soap, sizeof(char) * 400);
		sprintf(tds__GetServicesResponse->Service[1].Capabilities->__any, "<trt:Capabilities SnapshotUri=\"true\" Rotation=\"false\" OSD=\"true\"><trt:ProfileCapabilities MaximumNumberOfProfiles=\"%d\">"
			"</trt:ProfileCapabilities><trt:StreamingCapabilities RTPMulticast=\"false\" RTP_TCP=\"true\" RTP_RTSP_TCP=\"true\" NonAggregateControl=\"false\"></trt:StreamingCapabilities></trt:Capabilities>",
			soap->pConfig->g_channel_num*MAX_STREAM_NUM);

		tds__GetServicesResponse->Service[2].Capabilities = (struct _tds__Service_Capabilities *)soap_malloc_v2(soap, sizeof(struct _tds__Service_Capabilities));
		tds__GetServicesResponse->Service[2].Capabilities->__any = soap_strdup(soap, "<tev:Capabilities WSSubscriptionPolicySupport=\"true\" WSPullPointSupport=\"true\" WSPausableSubscriptionManagerInterfaceSupport=\"false\" "
			"MaxNotificationProducers=\"10\" MaxPullPoints=\"10\"></tev:Capabilities>");

		tds__GetServicesResponse->Service[3].Capabilities = (struct _tds__Service_Capabilities *)soap_malloc_v2(soap, sizeof(struct _tds__Service_Capabilities));
		tds__GetServicesResponse->Service[3].Capabilities->__any = soap_strdup(soap, "<tptz:Capabilities tt:EFlip=\"false\" tt:Reverse=\"false\"></tptz:Capabilities>");

		tds__GetServicesResponse->Service[4].Capabilities = (struct _tds__Service_Capabilities *)soap_malloc_v2(soap, sizeof(struct _tds__Service_Capabilities));
		tds__GetServicesResponse->Service[4].Capabilities->__any = soap_strdup(soap, "<timg:Capabilities ImageStabilization=\"false\"></timg:Capabilities>");

		tds__GetServicesResponse->Service[5].Capabilities = (struct _tds__Service_Capabilities *)soap_malloc_v2(soap, sizeof(struct _tds__Service_Capabilities));
		tds__GetServicesResponse->Service[5].Capabilities->__any = soap_strdup(soap, "<tmd:Capabilities VideoSources=\"1\" VideoOutputs=\"0\" AudioSources=\"1\" AudioOutputs=\"1\" "
			"RelayOutputs=\"1\" DigitalInputs=\"1\" SerialPorts=\"0\"></tmd:Capabilities>");

		tds__GetServicesResponse->Service[6].Capabilities = (struct _tds__Service_Capabilities *)soap_malloc_v2(soap, sizeof(struct _tds__Service_Capabilities));
		tds__GetServicesResponse->Service[6].Capabilities->__any = soap_strdup(soap, "<tan:Capabilities RuleSupport=\"true\" AnalyticsModuleSupport=\"true\" CellBasedSceneDescriptionSupported=\"true\"/>");
	}
	
	return SOAP_OK;  

} 

int  __tds__GetServiceCapabilities(struct soap* soap, struct _tds__GetServiceCapabilities *tds__GetServiceCapabilities, struct _tds__GetServiceCapabilitiesResponse *tds__GetServiceCapabilitiesResponse )
{ 
	
 	tds__GetServiceCapabilitiesResponse->Capabilities = (struct tds__DeviceServiceCapabilities *)soap_malloc_v2(soap, sizeof(struct tds__DeviceServiceCapabilities));

	tds__GetServiceCapabilitiesResponse->Capabilities->Network = (struct tds__NetworkCapabilities*)soap_malloc_v2(soap, sizeof(struct tds__NetworkCapabilities));
	
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->IPFilter = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->IPFilter = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->Network->ZeroConfiguration = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->ZeroConfiguration = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Network->IPVersion6 = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->IPVersion6 = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Network->DynDNS = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->DynDNS = xsd__boolean__true_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Network->Dot11Configuration = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->Dot11Configuration = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Network->HostnameFromDHCP = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->HostnameFromDHCP = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Network->NTP = (int *)soap_malloc(soap, sizeof(int));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Network->NTP = 1;

	tds__GetServiceCapabilitiesResponse->Capabilities->Security = (struct tds__SecurityCapabilities *)soap_malloc_v2(soap, sizeof(struct tds__SecurityCapabilities));

 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e0 = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e0 = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e1 = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e1 = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e2 = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->TLS1_x002e2 = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->OnboardKeyGeneration = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->OnboardKeyGeneration = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->AccessPolicyConfig = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->AccessPolicyConfig = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->DefaultAccessPolicy = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->DefaultAccessPolicy = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->Dot1X = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->Dot1X = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->RemoteUserHandling = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->RemoteUserHandling = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->X_x002e509Token = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->X_x002e509Token = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->SAMLToken = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->SAMLToken = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->KerberosToken = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->KerberosToken = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->UsernameToken = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->UsernameToken = xsd__boolean__true_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->HttpDigest = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->HttpDigest = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->Security->RELToken = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->Security->RELToken = xsd__boolean__false_;

	tds__GetServiceCapabilitiesResponse->Capabilities->System = (struct tds__SystemCapabilities *)soap_malloc_v2(soap, sizeof(struct tds__SystemCapabilities));
	
	tds__GetServiceCapabilitiesResponse->Capabilities->System->DiscoveryResolve = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->DiscoveryResolve = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->DiscoveryBye = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->DiscoveryBye = xsd__boolean__true_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->RemoteDiscovery = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->RemoteDiscovery = xsd__boolean__false_;
 	tds__GetServiceCapabilitiesResponse->Capabilities->System->SystemBackup = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->SystemBackup = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->SystemLogging = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->SystemLogging = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->FirmwareUpgrade = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->FirmwareUpgrade = xsd__boolean__false_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpFirmwareUpgrade = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpFirmwareUpgrade = xsd__boolean__true_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSystemBackup = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSystemBackup = xsd__boolean__true_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSystemLogging = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSystemLogging = xsd__boolean__true_;
	tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSupportInformation = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*tds__GetServiceCapabilitiesResponse->Capabilities->System->HttpSupportInformation = xsd__boolean__false_;

	tds__GetServiceCapabilitiesResponse->Capabilities->Misc = NULL;
	return SOAP_OK;

 
 } 

int  __tds__SetHostnameFromDHCP(struct soap* soap, struct _tds__SetHostnameFromDHCP *tds__SetHostnameFromDHCP, struct _tds__SetHostnameFromDHCPResponse *tds__SetHostnameFromDHCPResponse )
{
	ONVIF_DBP("");

	return SOAP_OK;  

}

 //added by panhn for axxon
int Onvif_StopRequest(int chno)
{
	ptz_control(QMAPI_PTZ_STA_STOP,0,chno);
	printf("%s  ======PTZ stop======\n",__FUNCTION__);
	return SOAP_OK;
}

#if 0
int main(int argc, char **argv)
{ 
/*
	SysDrvInit( SYS_BOA_MSG ); 

	if (InitFileMsgDrv(FILE_MSG_KEY, FILE_BOA_MSG) < 0) {
		ONVIF_DBP("================ InitFileMsgDrv failed \n");		
	}
	*/
	SOAP_SOCKET m, s; /* master and slave sockets */
	struct soap soap;
	soap_init(&soap);

#if 0

//	if (argc < 2)
//	{
		soap_serve(&soap);  /* serve as CGI application */
		/*check    reboot	 restort	reset  gsdvs restart*/
		
		switch(system_reboot_flag)
		{
			case NORMAL_REBOOT:
				onvif_send_disc_msg(dis_bye);
				break;
			case RESTORE_REBOOT:
				onvif_send_disc_msg(system_restore);
				break;
			case RESET_REBOOT:
				onvif_send_disc_msg(hard_reset);
				break;
			case AVSERVER_REBOOT:
				//system("killall avserver");
				break;
			case DISCOVERY_REBOOT:
				onvif_send_disc_msg(dis_mode);
				break;
			default:
				break;
		}	
			
//	}

	//else
	//{

#endif

		//m = soap_bind(&soap, NULL, atoi(argv[1]), SO_REUSEADDR);
		m = soap_bind(&soap, NULL, 8999, SO_REUSEADDR);

		if (!soap_valid_socket(m))
		{ 
			soap_print_fault(&soap, stderr);
			exit(-1);
		}


		InitNotifyManager();


		for ( ; ; )
		{
	
			s = soap_accept(&soap);
			//g_sock = s;
			
			if (!soap_valid_socket(s))
			{ 
				soap_print_fault(&soap, stderr);
				exit(-1);
			} 			
			
			//ONVIF_DBP("Accepted connection from IP= %d.%d.%d.%d socket = %d ",
			//	((soap.ip)>>24)&&0xFF,((soap.ip)>>16)&0xFF,((soap.ip)>>8)&0xFF,(soap.ip)&0xFF,(soap.socket));
			
			soap_serve(&soap);			
			soap_destroy(&soap);  			
			soap_end(&soap);	


			// for drop caches
			static int count = 0;
			count++;
			if (100 ==  count ) {
				count = 0;				
				
				FILE *fp = NULL;
				if ((fp = fopen("/proc/sys/vm/drop_caches", "r+"))) {								
					//ONVIF_DBP("/proc/sys/vm/drop_caches  ok\n");
					fwrite("3", strlen("3"), 1, fp);							
					fclose(fp);				
				}					
			}						
			
			/*check    reboot	 restort	reset  gsdvs restart*/
			switch(system_reboot_flag)
			{
				case NORMAL_REBOOT:
					onvif_send_disc_msg(dis_bye);
					break;
				case RESTORE_REBOOT:
					onvif_send_disc_msg(system_restore);
					break;
				case RESET_REBOOT:
					onvif_send_disc_msg(hard_reset);
					break;
				case AVSERVER_REBOOT:
					//system("killall gsdvs");
					break;
				case DISCOVERY_REBOOT:
					onvif_send_disc_msg(dis_mode);
					break;
				default:
					break;
			}	
	
		}

		soap_done(&soap);	
		
	//}
	

	return 0;
} 
#endif

#if 0
void *onvif_thread(void *arg)
{ 
	SOAP_SOCKET m, s; /* master and slave sockets */
	struct soap soap;
	soap_init(&soap);
	//m = soap_bind(&soap, NULL, atoi(argv[1]), SO_REUSEADDR);
	m = soap_bind(&soap, NULL, 8999, SO_REUSEADDR);

	if (!soap_valid_socket(m))
	{ 
		soap_print_fault(&soap, stderr);
		pthread_exit(NULL);
	}

	soap->pConfig->g_channel_num = get_soap->pConfig->g_channel_num();	//panhn

	InitNotifyManager();


	for ( ; ; )
	{

		s = soap_accept(&soap);
		//g_sock = s;
		
		if (!soap_valid_socket(s))
		{ 
			soap_print_fault(&soap, stderr);
			exit(-1);
		} 			
		
		//ONVIF_DBP("Accepted connection from IP= %d.%d.%d.%d socket = %d ",
		//	((soap.ip)>>24)&&0xFF,((soap.ip)>>16)&0xFF,((soap.ip)>>8)&0xFF,(soap.ip)&0xFF,(soap.socket));
		
		soap_serve(&soap);			
		soap_destroy(&soap);  			
		soap_end(&soap);	


		// for drop caches
		static int count = 0;
		count++;
		if (100 ==  count ) {
			count = 0;				
			
			FILE *fp = NULL;
			if ((fp = fopen("/proc/sys/vm/drop_caches", "r+"))) {								
				//ONVIF_DBP("/proc/sys/vm/drop_caches  ok\n");
				fwrite("3", strlen("3"), 1, fp);							
				fclose(fp);				
			}					
		}						
		
		/*check    reboot	 restort	reset  gsdvs restart*/
		switch(system_reboot_flag)
		{
			case NORMAL_REBOOT:
				onvif_send_disc_msg(dis_bye);
				break;
			case RESTORE_REBOOT:
				onvif_send_disc_msg(system_restore);
				break;
			case RESET_REBOOT:
				onvif_send_disc_msg(hard_reset);
				break;
			case AVSERVER_REBOOT:
				//system("killall gsdvs");
				break;
			case DISCOVERY_REBOOT:
				onvif_send_disc_msg(dis_mode);
				break;
			default:
				break;
		}	

	}

	soap_done(&soap);	
	
	return (void *)0;
} 

#endif

#if 0
static pthread_mutex_t pullmessage_thread_lock;
static int thread_count = 0;

void *process_pullmessage_thread(void *arg)
{
	//printf("#### %s %d\n", __FUNCTION__, __LINE__);
	struct soap *psoap;
	if(!arg)
	{
		return (void *)-1;
	}

	pthread_mutex_lock(&pullmessage_thread_lock);
	thread_count++;
	pthread_mutex_unlock(&pullmessage_thread_lock);
	psoap = (struct soap *)arg;
	httpd_conn* hc = (httpd_conn*)psoap->cgihc;

	soap_serve(psoap);
	hc->delay_close = 0;
	soap_destroy(psoap);  			
	soap_end(psoap);
	//soap_done(psoap);	
	soap_free(psoap);
	pthread_mutex_lock(&pullmessage_thread_lock);
	thread_count--;
	pthread_mutex_unlock(&pullmessage_thread_lock);
	
	return (void *)0;
	
}
#endif

static int fxmloutput(struct soap *soap, char *s, size_t n,void *cgihc)
{
	int ret = 0;
	httpd_conn* hc = (httpd_conn *)cgihc;	
	hc->responselen = 0;	
	if(hc->conn_fd<0 || hc->delay_close<0 
		|| hc->closeflag<0)
		return -1;

	httpd_clear_ndelay( hc->conn_fd );
	ret = httpd_write_fully( hc->conn_fd, s, n );

	if(ret < 0)
		return ret;

	return n;
}

int Onvif_Server(httpd_conn* hc, struct onvif_config *pConfig)
{
	//printf("#### %s %d, method=%d, conn_fd=%d\n", __FUNCTION__, __LINE__, hc->method, hc->conn_fd);

	int ret;
	int u32RtnNum = 0;

	struct soap soap;
	soap_init(&soap);
	soap_set_mode(&soap, SOAP_C_UTFSTRING);
	soap.fxmloutput = fxmloutput;
	soap.cgihc = hc;
	soap.pConfig = pConfig;
	soap.socket = hc->conn_fd;

	memcpy(&soap.peer, &hc->client_addr.sa_in, sizeof(struct sockaddr_in));

#if 0
	if( hc->method == METHOD_GET )
	{
	    if( hc->query[0] != '\0' )
	    {
	        if( strlen(hc->query) >= MAX_CNT_CGI )
	        {
	            *(hc->query + MAX_CNT_CGI) = '\0';
	        }
	        HI_CGI_strdecode( query, hc->query );  
	     }
	 }
	else
#endif		
	if(hc->method == METHOD_POST)
	{
	    int c, r;
	    char buf[MAX_CNT_CGI]={0};
	    struct timeval tm;
	    c=hc->read_idx - hc->checked_idx;
	    if(c>0)
	    {
	        strncpy(buf, &(hc->read_buf[hc->checked_idx]), MIN(c,MAX_CNT_CGI-1));  
           // printf("~~~~ %s %d, buf:%s\n", __FUNCTION__, __LINE__, buf);
	        //cpynum = c;
	    }
		int count = 0;

	    while ( ( c < (int)hc->contentlength ) && ( c < MAX_CNT_CGI-1 ) )
	    {
	    	r = read( hc->conn_fd, buf+c, MIN( (int)(hc->contentlength - c), (int)(MAX_CNT_CGI-1 - c )));
	    	if ( r < 0 && ( errno == EINTR || errno == EAGAIN ) )
	    	{
				if(strstr(buf, ":Envelope>"))
				{
					printf("%s, Invalid http content-length:%d.In fact we received %d bytes\n",__FUNCTION__,hc->contentlength, c);
					break;
				}
				
				//printf("%s, while count:%d\n",__FUNCTION__,count);
				tm.tv_sec = 0;
				tm.tv_usec = 100000;
				select(0,NULL,NULL,NULL,&tm);
				//sleep( 1 );//1秒太长
				count++;
				if(count>30)
					break;
				
				continue;
	    	}
	    	if ( r <= 0 )
	    	{
	            break;
	    	}
	    	c += r;
	    }
	    u32RtnNum=strlen(buf);      
	    if(u32RtnNum>0)
	    {
	        HI_CGI_strdecode(soap.buf, buf);
			soap.buflen = u32RtnNum;
	    }
	    else
	    {
	        printf("have not the string in content\n");
	    }
	}
	else
	{
		soap_destroy(&soap);
		soap_end(&soap);
		soap_done(&soap);
	
		printf("%s, Invalid method type :%d\n",__FUNCTION__, hc->method);
		hc->delay_close = 0;
		hc->closeflag = 0;
		hc->should_linger = 0;
		return -1;
	}
#if 0
	int needsleep = 0;
	if(strstr(query, "PullMessages>"))
	{
		needsleep = 1;
		hc->delay_close = 2100;
		hc->closeflag = 1;
		hc->responselen = 0;
	}
#endif
	//printf("soap string is %s\n", soap.buf);
	ret = soap_serve(&soap);  /* serve as CGI application */

	soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);

	hc->delay_close = 0;
	hc->closeflag = 0;
	hc->should_linger = 0;

	return ret;
}

static struct soap *event_soap = NULL;

int ONVIF_EventServer(int sockfd, struct onvif_config *pConfig)
{
	if (!event_soap)
	{
		event_soap = soap_new();
		printf("[ONVIF EVENT SERVER]ts:%lu malloc soap:%p.\n", time(NULL), event_soap);
	}
	
	soap_init(event_soap);
	event_soap->recv_timeout = event_soap->send_timeout = 3;
	event_soap->master = sockfd;
	event_soap->accept_timeout = -10000;
	event_soap->pConfig = pConfig;
	event_soap->fxmloutput = NULL;
	event_soap->cgihc = NULL;
	
	if (soap_accept(event_soap) != SOAP_INVALID_SOCKET)
	{
		//printf("soap serverevent request!\n");
		soap_serve_event(event_soap);
	}

	event_soap->master = SOAP_INVALID_SOCKET;
	soap_destroy(event_soap);
	soap_end(event_soap);
	soap_done(event_soap);
	return 0;
}

void ONVIF_EventServerUnInit()
{
	if (event_soap)
	{
		printf("[ONVIF EVENT SERVER]ts:%lu free soap:%p.\n", time(NULL), event_soap);
		soap_free(event_soap);
		event_soap = NULL;
	}
}

