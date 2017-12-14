#include "hik_server_protocol.h"

static HikUser *pGlobalUserInfo;

static int hik_ServiceInit()
{
	int i;

	pGlobalUserInfo = (HikUser *)memalign(BUFF_ALIGNMENT, sizeof(HikUser));
	memset(pGlobalUserInfo, 0, sizeof(HikUser));
	for(i = 0; i  <MAX_LOGIN_USERS; i++)
	{
		pGlobalUserInfo->puserinfo[i].userID = NO_USER;
	}

    pGlobalUserInfo->hikhandle = QMapi_sys_open(QMAPI_NETPT_HIK);

	return 0;
}

static int hik_CreateServerSocket()
{
	int ret = -1;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if(sockfd < 0)
	{
		HPRINT_ERR("dvrNetServer socket failed\n");
		return -1;
	}

	int opt = 1;
	ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (0 != ret)
	{
		HPRINT_ERR("\n");
		goto err;
	}
	opt = 1;
	ret = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
	if (0 != ret)
	{
		HPRINT_ERR("\n");
		goto err;
	}
	opt = (32 * 1024);
	ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &opt, sizeof(opt));
	if (0 != ret)
	{
		HPRINT_ERR("\n");
		goto err;
	}
	opt = (32 * 1024);
	ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt));
	if (0 != ret)
	{
		HPRINT_ERR("\n");
		goto err;
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr,0,sizeof(struct sockaddr_in));
	serverAddr.sin_family	  = AF_INET;
	serverAddr.sin_port 	  = htons(8000);	
	serverAddr.sin_addr.s_addr= htonl(INADDR_ANY);

	if(bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr_in))!=0)
	{
		HPRINT_ERR("bind failed\n");
		goto err;
	}

	if(listen(sockfd, 10) != 0) //最大支持10个用户
	{
		HPRINT_ERR("listen failed\n");
		goto err;
	}

	return sockfd;
err:
	close(sockfd);
	sockfd = -1;
	
	return -1;
}

void Hik_Server_Cfg_Init(NW_Server_Cfg *para)
{
	hik_ServiceInit();

	para->CreateSocket = hik_CreateServerSocket;
	para->InitPara = pGlobalUserInfo;
	para->InitFuncName = "lib_hik_init";
	para->UnInitFuncName = "lib_hik_uninit";
	para->ThreadFuncName = "hik_Session_Thread";
	para->LibPath = "/usr/lib/libhik.so";	
}


