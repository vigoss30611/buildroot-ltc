#include "ts_search_protocol.h"
#include "sys_fun_interface.h"

extern int g_nw_search_run_index;

int TsSearchRequsetProc(int dmsHandle, int sockfd, int sockType)
{
	char recv_buf[1024];	
    memset(recv_buf, 0, 1024);

	struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(struct sockaddr_in);

	void *handle = NULL;
	int (*pTSRespondSearch)(int ,char *,int );	
	g_nw_search_run_index = 300;

	do{
	    int num = recvfrom(sockfd, recv_buf, 1024, 0, (struct sockaddr*)&client_addr, &addrlen);
	    if (num < 0)  
	    {  
	        if(errno == EINTR || errno == EAGAIN)
	        {
	            break;
	        }
	        
			SYSTEMLOG(SLOG_ERROR, "recvfrom error: %s(%d)\n", strerror(errno), errno);
			break;
	    }
		g_nw_search_run_index = 301;
		handle = dlopen("/usr/lib/libts.so",RTLD_LAZY);
		if (!handle) 
		{	
			SYSTEMLOG(SLOG_ERROR, "%s  %d,err:%s\n", __FUNCTION__, __LINE__, dlerror());
			break;
		}  
		g_nw_search_run_index = 302;

		pTSRespondSearch = dlsym(handle, "TSRespondSearch");
		if(dlerror() != NULL)
		{
			SYSTEMLOG(SLOG_ERROR, "%s  %d,err:%s\n", __FUNCTION__, __LINE__, dlerror());
			//sys_fun_dlclose(handle);
			dlclose(handle);
			break;
		}
		g_nw_search_run_index = 303;

		int ret = (*pTSRespondSearch)(sockfd, recv_buf, num);
		if(ret != 0)
		{
			SYSTEMLOG(SLOG_ERROR, "%s  %d,search handle err:%s\n", __FUNCTION__, __LINE__, dlerror());
		}
		g_nw_search_run_index = 304;

		//sys_fun_dlclose(handle);
		dlclose(handle);
		g_nw_search_run_index = 305;
	}while(0);
	g_nw_search_run_index = 306;

	return 0;
}


int TsSearchCreateSocket(int *pSockType)
{
    struct sockaddr_in server;

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) 
	{
		SYSTEMLOG(SLOG_ERROR, "socket error%s\n", strerror(errno));
		return -1;
	}
	
	int opt = 1;
	int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (ret < 0)
	{
		SYSTEMLOG(SLOG_ERROR, "%s  %d, setsockopt error!err:%s(%d)\n",__FUNCTION__, __LINE__, strerror(errno), errno);
		goto err;
	}
	
	memset(&server,0,sizeof(server));	
	server.sin_family		= AF_INET;	
	server.sin_port 		= htons(3001);	
	server.sin_addr.s_addr	= htonl(INADDR_ANY);  
	if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1)	
	{  
		SYSTEMLOG(SLOG_ERROR, "%s  %d, bind error!err:%s(%d)\n", __FUNCTION__, __LINE__, strerror(errno), errno);
		goto err;
	}
	
	opt = 1;
	ret = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &opt, sizeof(opt));
	if (ret < 0)
	{
		SYSTEMLOG(SLOG_ERROR, "%s  %d, setsockopt error!err:%s(%d)\n", __FUNCTION__, __LINE__, strerror(errno), errno);
		goto err;
	}

		
	struct ip_mreq command;
	command.imr_multiaddr.s_addr = inet_addr("255.255.255.255");
	command.imr_interface.s_addr = inet_addr("0.0.0.0");
	ret = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &command, sizeof(command));
	if (ret < 0)
	{
		SYSTEMLOG(SLOG_ERROR, "%s  %d, setsockopt error!err:%s(%d)\n", __FUNCTION__, __LINE__, strerror(errno), errno);
	}
	
	int set = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char*)&set, sizeof(int))<0)
	{	
		SYSTEMLOG(SLOG_ERROR, "%s  %d, setsockopt SOL_SOCKET!err:%s(%d)\n", __FUNCTION__, __LINE__, strerror(errno), errno);
		goto err;
	}

	return sockfd;
err:
	close(sockfd);
	sockfd = -1;
	
	return -1;
}
