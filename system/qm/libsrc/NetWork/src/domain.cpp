#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <unistd.h> 

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <assert.h>


#include "QMAPIType.h"
#include "domain.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


const unsigned char DNS_REQ_DATA_HEAD[12] = {0x2b,0x50,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
const unsigned char DNS_REQ_DATA_TAIL[4]  = {0x00,0x01,0x00,0x01};
const unsigned short DNS_PORT = 53;

static void ReleaseHost(struct hostent *pHost)
{
	if (NULL != pHost)
	{
		delete[] pHost->h_addr_list[0];
		delete[] pHost->h_addr_list[1];
		delete[] pHost->h_addr_list[2];
		delete[] pHost->h_addr_list[3];
		delete[] pHost->h_addr_list;
		delete pHost;
	}
}

static bool ParseDomainName(char * pDNS, size_t &length)
{
	if (length < strlen(pDNS) + 2)
	{
		printf("%s:%s:%d, the length is too short\n", __FUNCTION__, __FILE__, __LINE__);
		return false;
	}
	char *pDNSTemp = new char [length];
	if(NULL == pDNSTemp)
	{
		printf("%s:%s:%d, New error\n", __FUNCTION__, __FILE__, __LINE__);
		return false;
	}
	else
	{
		memcpy(pDNSTemp, pDNS, length);
		memset(pDNS, 0, length);
		strcpy(pDNS+1, pDNSTemp);
		delete [] pDNSTemp;
		pDNSTemp = NULL;
	}

	char * pCurrent	= pDNS+1;
	char * pMark	= pDNS;
	*pMark = 0;

	while ('\0' != *pCurrent)
	{
		//只要当前字符不为字符串的结束标志
		//如果当前字符是 .
		if ('.' == *pCurrent)
		{
			//如果连着两个都是 . ,则表示输入格式错, 返回错误
			if ('.' == *(pCurrent-1))
			{
				printf("%s:%s:%d, The dns %s is invalid\n", __FUNCTION__, __FILE__, __LINE__, pDNS);
				assert(false);
				return false;
			}
			pMark = pCurrent;
			*pMark = 0;
		}
		else
		{
			(*pMark)++;	//计数
		}
		pCurrent++;		//指针后移
	}

	length = strlen(pDNS) + 1;

	return true;
}

struct hostent *SRDKGetHostbyname(const char * pDNS, char * pDnsServer)
{
	struct hostent * pHost = new hostent;
	if (NULL == pHost)
	{
		printf("%s:%s:%d, New error\n", __FUNCTION__, __FILE__, __LINE__);
		assert(false);
		return NULL;
	}
	else
	{
		memset(pHost, 0, sizeof(struct hostent));
		pHost->h_addr_list = new char*[4];
		assert(NULL != pHost->h_addr_list);
		for(int index1 = 0; index1 < 4; ++index1)
		{
			pHost->h_addr_list[index1] = new char[40];
			assert(NULL != pHost->h_addr_list[index1]);
			memset(pHost->h_addr_list[index1], 0, 40);
		}
	}

	if (inet_aton(pDNS, (struct in_addr*)pHost->h_addr))
	{
		//请求的域名本身就是IP地址
		return pHost;
	}

	//解析域名为DNS请求的格式
	size_t lengthParseName = strlen(pDNS) + 2;		//解析后字符串长度
	char * pParseName = new char[lengthParseName];	//用于存储解析后的域名
	assert(NULL != pParseName);
	memset(pParseName, 0, lengthParseName);
	strcpy(pParseName, pDNS);
	if (!ParseDomainName(pParseName, lengthParseName)) 
	{
		delete [] pParseName;
		pParseName = NULL;
		ReleaseHost(pHost);

		return NULL;
	}

	//建构DNS报文头部信息	 格式为[标示(2字节)][标志(2字节)][问题数(2字节)][应答资源数(2字节)][授权资源数][额外资源数]{查询问题}{应答}{授权}{额外}
	size_t lengthDatagram = 12 + lengthParseName + 4;
	char * pDadagram = new char[lengthDatagram];
	assert(NULL != pDadagram);
	memcpy(pDadagram, DNS_REQ_DATA_HEAD,12);							//考贝协议头
	memcpy(pDadagram + 12, pParseName, lengthParseName);				//考贝要解析的域名
	memcpy(pDadagram + 12 + lengthParseName, DNS_REQ_DATA_TAIL, 4);		//考贝协议尾
	delete [] pParseName;
	pParseName = NULL;

	//获得DNS服务器的IP地址
	struct sockaddr_in their_addr;
	memset(&their_addr, 0, sizeof(their_addr));
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(DNS_PORT);
	their_addr.sin_addr.s_addr = inet_addr(pDnsServer);
	if (INADDR_NONE == their_addr.sin_addr.s_addr)
	{
		delete [] pDadagram;
		pDadagram = NULL;

		ReleaseHost(pHost);

		return NULL;
	}

	//创建socket和DNS服务器通信
	int sockfd = -1;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) 
	{
		printf("(%s %d) Failed: open socket failed,%s!\n",__FILE__,__LINE__,strerror(errno));
		delete [] pDadagram;
		pDadagram = NULL;
		ReleaseHost(pHost);

		return NULL;
	}

	timeval timeout;
	timeout.tv_sec	= 5;	//超时时间为5s
	timeout.tv_usec = 0;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout))) 
	{
		close(sockfd);

		printf("(%s %d) Failed: setsockopt failed,%s\n",__FILE__,__LINE__,strerror(errno));
		delete [] pDadagram;
		pDadagram = NULL;
		ReleaseHost(pHost);
		return NULL;
	}

	//发送DNS数据并接收服务器回应
	char revBuffer[1024] = {0};
	int  revLength = 0;

	bool ret =  false;
	//进行5次通信
	for (int timer = 0; !ret && timer < 3; timer ++) 
	{
		revLength = sendto(sockfd, (const char*)pDadagram, lengthDatagram, 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr));
		if (-1 == revLength)
		{
			//printf("%s, %s %d Failed: send DNS data failed, timer=%d, %s!\n",__FUNCTION__, __FILE__,__LINE__,timer, strerror(errno));
			continue;
		}

		memset(revBuffer, 0, sizeof(revBuffer));
		socklen_t addr_len = sizeof(their_addr);

		revLength = recvfrom(sockfd, revBuffer, sizeof(revBuffer), 0, (sockaddr*)&their_addr, &addr_len);	
		if(12 < revLength)
		{
			//比较DNS id
			if(0 == memcmp(revBuffer, DNS_REQ_DATA_HEAD, 2)) 
			{
				//在回应报文中可能有多个返回地址, 得到其个数
				short temp = 0;
				memcpy(&temp, revBuffer + 6, sizeof(short));
				short nameNum = ntohs(temp);

				char * pAnser = revBuffer;
				pAnser = pAnser+ 12 + lengthParseName + 4;	//12为DNS协议的头长度//4为query字段的:type, class ,各为一个short类型
				int useLen = 12 + lengthParseName + 4;
				int index = 0;
				while( (nameNum > 0) && (useLen + 12 <= revLength) )
				{
					//得到其后内容的类型
					memcpy(&temp, pAnser + 2, sizeof(short));
					short nameType = ntohs(temp);
					memcpy(&temp, pAnser + 10, sizeof(short));
					short nameLength = ntohs(temp);

					//如果是IP地址, 返回
					if( (1==nameType) && (useLen + 16 <= revLength) )
					{
						memcpy(&pHost->h_addr_list[index][3],(pAnser+15),1);
						memcpy(&pHost->h_addr_list[index][2],(pAnser+14),1);
						memcpy(&pHost->h_addr_list[index][1],(pAnser+13),1);
						memcpy(&pHost->h_addr_list[index][0],(pAnser+12),1);
						pHost->h_addrtype = AF_INET;
						index ++;
						ret  = true;
						if (2 == index)
						{
							break;
						}
					}
					pAnser = pAnser + 12 + nameLength;
					useLen += 12 + nameLength;
					nameNum --;
				}			
			}
		}
	}

	close(sockfd);
	delete [] pDadagram;
	pDadagram  = NULL;

	if (ret) 
	{		
		return pHost;
	}
	else
	{
		ReleaseHost(pHost);

		return NULL;
	}
}

/* RM#2291: Get DNS Server from config file: /tmp/resolv.conf.    henry.li    2017/03/01 */
static int GetDnsServer(char *dnsServer, unsigned int len)
{
	FILE *fp = NULL;
	char line[32] = {0};

	fp = fopen("/tmp/resolv.conf", "r");

	if (NULL == fp)
	{
		printf("Open dns server file failed!\n");
		return 0;
	}

	if (fgets(line, 32, fp) == NULL)
	{
		printf("Read dns server failed!\n");
		fclose(fp);
		return 0;
	}

	/* cat /tmp/resolv.conf
	nameserver 202.96.199.133 */
	strcpy(dnsServer, line + (strlen("nameserver ")));
	dnsServer[strlen(dnsServer) - 1] = '\0';
	//printf("--\nDnsServer:[%s]------\n", dnsServer);

	fclose(fp);
	return 1;
}


/*
* 返回0 解析失败
*/
UINT32 dms_gethostbyname(const char *pDNS)
{
	int i;
	hostent * pHost = NULL;
	UINT32 ip_addr = 0;
	//char nameserver[4][16] = {"114.114.114.114"/*114*/, "8.8.8.8"/*google*/, "180.76.76.76"/*baidu*/, "202.45.84.58"/*hk*/};
	char nameserver[2][16] = {"114.114.114.114"/*114*/, "8.8.8.8"/*google*/};

	/* RM#2291: used Primary DNS from resolv.conf(Web setting).    henry.li    2017/03/01  */
	char dnsserver[32] = {0};

	if (GetDnsServer(dnsserver, 32) == 1)
	{
		strcpy(nameserver[0], dnsserver);
	}

	//printf("dnsserver[0]=[%s], dnsserver[1]=[%s]\n", nameserver[0], nameserver[1]);
	if(NULL == pDNS)
		return 0;

	for(i = 0; i < 2; i++)
	{
		pHost = SRDKGetHostbyname(pDNS, nameserver[i]);
		if (NULL != pHost)
		{
			ip_addr = *((unsigned long*)pHost->h_addr);

			ReleaseHost(pHost);
			
			break;
		}
	}
/*
 	if(2 == i)
	{
		ip_addr = gethostbyname_fromserver(pDNS);
	}
*/
	return ip_addr;
}








#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


