#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "xmai_server_protocol.h"
#include "xmai_common.h"

static int xmai_CreateServerSocket()
{	
    //创建监听会话的socket
    int fListenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (fListenfd < 0) 
    {
        XMAIERR("socket error%s", strerror(errno));
		return -1;
    }

	struct sockaddr_in t_serv_addr;
	
    /*  配置监听会话的socket */
	bzero(&t_serv_addr, sizeof(t_serv_addr));
    t_serv_addr.sin_family = AF_INET;
    t_serv_addr.sin_port = htons(XMAI_TCP_LISTEN_PORT);
    t_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int opt = 1;
	if (setsockopt(fListenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		XMAIERR("reuse addr err%s", strerror(errno));
		goto end_v2;
	}
	
    /* 绑定监听会话的socket */
    if (bind(fListenfd, (struct sockaddr *)&t_serv_addr, sizeof(struct sockaddr)) < 0) 
    {
        XMAIERR("bind err%s\n", strerror(errno));
        goto end_v2;
    }
	
    /* 主动socket转为被动监听socket */
	if (listen(fListenfd, XMAI_MAX_LINK_NUM) < 0) 
    {
        XMAIERR("listen error%s", strerror(errno));
        goto end_v2;
    }
	
	return fListenfd;
end_v2: 
	XMAINFO("XMaiMainThread end !\n");
	  
	if(fListenfd > 0)
	{
		close(fListenfd); 
		fListenfd = -1;
	}
	
	return -1;
}

void xmai_Server_Cfg_Init(NW_Server_Cfg *para)
{
	para->CreateSocket = xmai_CreateServerSocket;
	para->InitPara = NULL;
	para->InitFuncName = "lib_xmai_init";
	para->UnInitFuncName = "lib_xmai_stop";
	para->ThreadFuncName = "XMaiSessionThread";
	para->LibPath = "/usr/lib/libxmai.so";	
}


