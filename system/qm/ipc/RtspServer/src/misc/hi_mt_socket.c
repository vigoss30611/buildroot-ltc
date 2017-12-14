

#include "hi_mt_socket.h"

#if HI_OS_TYPE == HI_OS_LINUX
        #include <sys/types.h>
        #include <sys/socket.h>
        #include <sys/ioctl.h>
        #include <net/if.h>
        #include <net/if_arp.h>
        #include <arpa/inet.h>
        #include <unistd.h>
        #include <stdlib.h>
        #include <linux/sockios.h>
        #include <netinet/in.h>
        #include <net/route.h>
        #include <stdio.h>
        #include <string.h>
        #include <errno.h>
#elif HI_OS_TYPE == HI_OS_WIN32
        #include <winsock2.h>
        #include <Ws2tcpip.h>
        #pragma comment(lib,"ws2_32.lib")
        #define snprintf _snprintf
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

int MT_SOCKET_CloseSocket(int ulSocket)
{
#if HI_OS_TYPE == HI_OS_LINUX
    return close(ulSocket);
#elif HI_OS_TYPE == HI_OS_WIN32
  return closesocket((unsigned int)ulSocket);
#else
#error YOU MUST DEFINE HI OS TYPE HI_OS_TYPE == HI_OS_WIN32 OR HI_OS_LINUX !  
#endif    
}



int      MT_SOCKET_GetPeerIPPort(int s, char* ip, unsigned short *port)
{
    socklen_t  namelen = 0;
    struct sockaddr_in addr;
    
    namelen = sizeof(struct sockaddr_in);
    getpeername(s, (struct sockaddr *)&addr, &namelen);

    
    *port = (unsigned short)ntohs(addr.sin_port);
    strcpy(ip, inet_ntoa( addr.sin_addr ));
    
    return 0;
        
}
int      MT_SOCKET_GetPeerSockAddr(char* ip, unsigned short port,struct sockaddr_in *pSockAddr)
{
    pSockAddr->sin_family = AF_INET; 
    pSockAddr->sin_port = htons(port); 
    pSockAddr->sin_addr.s_addr = inet_addr(ip);
    memset(&(pSockAddr->sin_zero), '\0', 8);
    return 0;
    
}

int      MT_SOCKET_GetHostIP(int s, char* ip)
{
    socklen_t  namelen = 0;
    struct sockaddr_in addr;
    namelen = sizeof(struct sockaddr);
    getsockname(s, (struct sockaddr *)&addr, &namelen);
    strcpy(ip, inet_ntoa( addr.sin_addr ));

    return 0;
    
}


int   MT_SOCKET_Udp_Open(unsigned short port)
{
    HI_SOCKET fd;
    struct sockaddr_in addr;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
    	perror("MT_SOCKET_Udp_Open erro: \n");
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    memset(&(addr.sin_zero), '\0', 8); // zero the rest of the struct
    
    int socket_opt_value = 1;  
	setsockopt(fd, SOL_SOCKET,SO_REUSEADDR,&socket_opt_value,sizeof(int));  
    if (bind (fd, (struct sockaddr *)&addr, sizeof (addr)))
    {
    	perror("MT_SOCKET_Udp_Open bind erro: ");
        MT_SOCKET_CloseSocket(fd);
        return -1;
    }

    #if 0
    /* set to non-blocking */
    if (ioctlsocket(f, FIONBIO, &on) < 0)
    {
        return -1;
    }
    #endif
    return fd;        
}


int   MT_SOCKET_Multicast_Open(char *multicastAddr, unsigned short port)
{
    HI_SOCKET fd;
    struct sockaddr_in addr;
    struct ip_mreq command; 

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
    	perror("MT_SOCKET_Udp_Open erro: \n");
        return -1;
    }

    int opt = 1;
    int ret = setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &opt, sizeof(opt));
    if (ret < 0)
    {
        printf("%s  %d, setsockopt error!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
        close(fd);
        return -1;
    }

    command.imr_multiaddr.s_addr = inet_addr(multicastAddr); 
    command.imr_interface.s_addr = htonl(INADDR_ANY);
    if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &command, sizeof(command))<0)
    {
        printf("%s  %d, setsockopt failed.err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
        MT_SOCKET_CloseSocket(fd);
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    memset(&(addr.sin_zero), '\0', 8); // zero the rest of the struct

    if (bind (fd, (struct sockaddr *)&addr, sizeof (addr)))
    {
    	perror("MT_SOCKET_Udp_Open bind erro: ");
        setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &command, sizeof(command));
        MT_SOCKET_CloseSocket(fd);
        return -1;
    }

    #if 0
    /* set to non-blocking */
    if (ioctlsocket(f, FIONBIO, &on) < 0)
    {
        return -1;
    }
    #endif
    return fd;        
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
