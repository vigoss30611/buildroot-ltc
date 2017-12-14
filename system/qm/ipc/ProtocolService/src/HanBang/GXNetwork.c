#include "GXNetwork.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "GXConfig.h"

extern int g_nw_search_run_index;
void GXPrintNetInfo(const unsigned int ip, const unsigned short port, const int socketfd)
{
	struct in_addr addrX;
	addrX.s_addr = ip;
	GX_DEBUG("client addr %s port %d socketfd %d", inet_ntoa(addrX), port, socketfd);
}

unsigned short GXGetSocketPort(const int socketfd)
{
	struct sockaddr_in sockAddr = { 0 };
	socklen_t iLen = sizeof(struct sockaddr);

	//getpeername(socketfd,(struct sockaddr *)&sockAddr,&iLen);//get remote IP and Port
	getsockname(socketfd, (struct sockaddr *)(&sockAddr), &iLen);//get local IP and Port
	//char* strAddr = inet_ntoa(sockAddr.sin_addr);//IP
	unsigned short uIPPort = ntohs(sockAddr.sin_port); //port;

	//GX_DEBUG("socket port:%d",uIPPort);
	return uIPPort;
}


int GXCreateSocketBrodcast(const char *host, const unsigned short port)
{
	GX_DEBUG("IP address is %s, port is %d", host, port);

	int socketfd = -1;
	struct sockaddr_in serverAddr;

	memset(&serverAddr, 0, sizeof(struct sockaddr_in));
	//serverAddr.sin_len = sizeof(serverAddr);//
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr(host);
	
	socketfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP /*0*/);
	if (-1 == socketfd) {
		GX_DEBUG("socket() error:%d", errno);
		return -1;
	}

	// 设置该套接字为广播类型，  
	int optval = 1;
	setsockopt(socketfd, SOL_SOCKET, SO_BROADCAST, (char *)&optval, sizeof(optval));

	if (-1 == bind(socketfd, (struct sockaddr *)(&serverAddr), sizeof(struct sockaddr)))
	{
		GX_DEBUG("bind fail !");
		return -2;
	}
	unsigned short bindPort = GXGetSocketPort(socketfd);
	GX_DEBUG("Bind current Port:%d", bindPort);

	return socketfd;
}

int GXCreateSocket(const char *host, const unsigned short port)
{
	int socketfd = -1;
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));

	//serverAddr.sin_len = sizeof(serverAddr);//
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(host);
	GX_DEBUG("IP address is %s port %d", inet_ntoa(serverAddr.sin_addr), port);

	socketfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP/*0*/);
	if (-1 == socketfd) {
		GX_DEBUG("socket() error:%d", errno);
		return -1;
	}	

	unsigned long non_blocking = 1;
	ioctl(socketfd, FIONBIO, &non_blocking);

	GX_DEBUG("connect socket ip:%x port:%x !", serverAddr.sin_addr.s_addr, serverAddr.sin_port);
	if (-1 == connect(socketfd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)))
	{
		int state = 0;
		struct timeval tv = { 15, 0 };  //10
		fd_set writefds;
		FD_ZERO(&writefds);
		FD_SET(socketfd, &writefds);
		if (select(socketfd + 1, NULL, &writefds, NULL, &tv) != 0) {
			if (FD_ISSET(socketfd, &writefds)) {
				int error;
				socklen_t len = sizeof(error);
				if (getsockopt(socketfd, SOL_SOCKET, SO_ERROR, (char *)&error, &len) < 0) {
					GX_DEBUG("GXCreateSocket1 errno:%d", errno);
					state = -1;
				}
				else if (error != 0){
					GX_DEBUG("GXCreateSocket2 errno:%d", errno);
					state = -1;
				}
			}
			else {
				GX_DEBUG("GXCreateSocket3 errno:%d", errno);
				state = -1;
			}
		}
		else {
			GX_DEBUG("GXCreateSocket4 errno:%d", errno);
			state = -1;		// timeout
		}
		unsigned long blocking = 0;
		ioctl(socketfd, FIONBIO, &blocking);

		if (0 == state && -1 != fcntl(socketfd, F_SETFL, O_NONBLOCK)) {
			return socketfd;
		}
		else {
			GX_DEBUG("state :%d errno:%d", state, errno);
			if (0 != state) {
				GX_DEBUG("0 != state:%d", errno);
			}
			else{
				GX_DEBUG("fcntl return -1 errno:%d", errno);
			}
		}
	}
	else {
		GX_DEBUG("connect() error:%d", errno);
	}

	shutdown(socketfd, SHUT_RDWR);
	close(socketfd);

	return -1;
}

int GXCreateSocket2(const char *host, const unsigned short port, const unsigned short maxNum)
{
	GX_DEBUG("IP address is %s, port is %d maxNum is %d", host, port, maxNum);

	int socketfd = -1;
	struct sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(struct sockaddr_in));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	socketfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == socketfd){
		GX_DEBUG("socket() error:%d", errno);
		return -1;
	}

	//setup opt
	int opt = 1;
	int nRet = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (nRet < 0){
		GX_DEBUG("setup SO_REUSEADDR");
		close(socketfd);
		return -4;
	}

	if (bind(socketfd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr))){
		GX_DEBUG("bind fail !");
		close(socketfd);
		return -2;
	}

	if (listen(socketfd, maxNum) != 0){
		GX_DEBUG("setup beyond support listen number.");
		close(socketfd);
		return -3;
	}
	return socketfd;
}

int GXCloseSocket(int socketfd)
{
	int nRet = -1;
	if (-1 != socketfd) {
		nRet = shutdown(socketfd, SHUT_RDWR);
//		GX_DEBUG("shutdown() nRet:%d errno:%d", nRet, errno);
		nRet = close(socketfd);
//		GX_DEBUG("close() nRet:%d", nRet);
	}
	return nRet;
}

static void GXSetnonblocking(int sock) 
{
	// TCP_CORK  TCP_NODELAY
	long o = 1;
	socklen_t ol = sizeof(long);
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&o, ol);

	// blocking
	int opts;
	opts = fcntl(sock, F_GETFL);
	if (opts < 0) {
		GX_DEBUG("fcntl(sock, GETFL)");
		//exit(1);
		return ;
	}
	opts = opts | O_NONBLOCK;
	if (fcntl(sock, F_SETFL, opts) < 0) {
		perror("fcntl(sock,SETFL,opts)");
		//exit(1);
		return;
	}
}

int GXAcceptClient(int socketfd, unsigned int *addr, unsigned short *port)
{
	int clientfd = -1;
	struct sockaddr_in clientAddr;
	bzero(&clientAddr, sizeof(struct sockaddr_in));
	int addrLen = sizeof(struct sockaddr_in);

	while(GXCheckSocket(socketfd, 5, 0, kCheckForRead) > 0)
	{
		clientfd = accept(socketfd, (struct sockaddr *)&clientAddr, (socklen_t *)&addrLen);
		if (errno == EINTR || clientfd < 0){
			GX_DEBUG("accept failed. errno %d", errno);
			return -1;
		}
//		GX_DEBUG("success accept client socket %d", clientfd);
				
		GXSetnonblocking(clientfd);

		*addr = clientAddr.sin_addr.s_addr;
		*port = clientAddr.sin_port;
		return clientfd;
	}

	return -2;
}

int GXCheckSocket(int socketfd, long sec, int usec, NETWORKCHECKTYPE checkType)
{
	if (socketfd <= 0) {
		return -1;
	}

	int	nRt = -2;
	fd_set fdset;
	struct timeval timeout;

	FD_ZERO(&fdset);
	FD_SET(socketfd, &fdset);
	timeout.tv_sec = sec;	//3L
	timeout.tv_usec = usec; //100L

	switch (checkType)
	{
	case kCheckForRead:
		nRt = select(socketfd + 1, &fdset, NULL, NULL, &timeout);
		break;
	case kCHeckForWrite:
		nRt = select(socketfd + 1, NULL, &fdset, NULL, &timeout);
		break;
	case kCheckForError:
		nRt = select(socketfd + 1, NULL, NULL, &fdset, &timeout);
		break;
	default:
		break;
	}

//	if (-1 == nRt) {
//		GX_DEBUG("select() error1:%d",errno);
//	}

	if (nRt > 0 && !FD_ISSET(socketfd, &fdset)) {
		GX_DEBUG("select() error2 nRt:%d errno:%d", nRt, errno);
		return -2;
	}
// 	if (nRt <= 0) {
// 		GX_DEBUG("select() error3 nRt:%d errno:%d", nRt, errno);
// 	}

	return nRt;
}

int GXSendPacket2(int socketfd, long timeout  /*second*/, const void *buffer, size_t length)
{
//	EINTR;
	long havedSend = 0;
	long oneSend = 0;

	while (GXCheckSocket(socketfd, timeout, 0, kCHeckForWrite) > 0)
	{
		oneSend = send(socketfd, (char*)buffer + havedSend, length - havedSend, 0);
		if (-1 == oneSend) {
			GX_DEBUG("send() socketfd:%d error:%d", socketfd, errno);
			return -1;
		}
		havedSend += oneSend;
		if (havedSend == length) {
//			GX_DEBUG("send success len %d", havedSend);
			return 0;
		}
	}
	GX_DEBUG("GXCheckSocket() error.");
	return -2;
}

int GXRecvPacket2(int socketfd, long timeout /*second*/, void *buffer, size_t length)
{
	long oneRecv = 0;
	long havedRecv = 0;

	while (GXCheckSocket(socketfd, timeout, 0, kCheckForRead) > 0)
	{
		oneRecv = recv(socketfd, (char*)buffer + havedRecv, length - havedRecv, 0);
		if (0 > oneRecv) {
			GX_DEBUG("0 > oneRecv recv() error:%d", errno);
			return -1;
		}
		else if (0 == oneRecv){
//			GX_DEBUG("0 == oneRecv recv() error:%d", errno);
			return -1;
		}
		havedRecv += oneRecv;
		if (havedRecv == length || havedRecv > 0) {		//modify
//			GX_DEBUG("havedRecv len %d", havedRecv);
			return 0;
		}
	}
	return -1;
}

int GXRecvPacket3(int socketfd, long timeout /*second*/, void *buffer, size_t length)
{
	long oneRecv = 0;
	long havedRecv = 0;

	while (GXCheckSocket(socketfd, timeout, 0, kCheckForRead) > 0)
	{
		oneRecv = recv(socketfd, (char*)buffer + havedRecv, length - havedRecv, 0);
		if (0 > oneRecv) {
			GX_DEBUG("0 > oneRecv recv() error:%d", errno);
			return -1;
		}
		else if (0 == oneRecv){
//			GX_DEBUG("0 == oneRecv recv() error:%d", errno);
			return -1;
		}
		havedRecv += oneRecv;
		if (havedRecv == length) {		//modify
			//GX_DEBUG("havedRecv3 len %d", havedRecv);
			return havedRecv;
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int GXSendtoPacket(int socketfd, struct sockaddr_in *to, long timeout, const void *buffer, size_t length)
{
	long havedSend = 0;
	long oneSend = 0;

	while (GXCheckSocket(socketfd, timeout, 0, kCHeckForWrite) > 0)
	{
		oneSend = sendto(socketfd, (char*)buffer + havedSend, length - havedSend, 0, (struct sockaddr*)to, sizeof(struct sockaddr_in));
		if (-1 == oneSend) {
			GX_DEBUG("send() error:%d",errno);
			return -1;
		}
		havedSend += oneSend;
		if (havedSend == length) {
			return 0;
		}
	}

	return -1;
}

int GXRecvfromPacket(int socketfd, long timeout, void *buffer, size_t length, struct sockaddr_in *from)
{
	int from_len = sizeof(struct sockaddr_in);
	long oneRecv = 0;
	long havedRecv = 0;

	g_nw_search_run_index = 502;
	while (GXCheckSocket(socketfd, timeout, 0, kCheckForRead) > 0)
	{
		g_nw_search_run_index = 503;
		oneRecv = recvfrom(socketfd, (char*)buffer + havedRecv, length - havedRecv, 0, (struct sockaddr*)from, (socklen_t *)&from_len);
		if (0 > oneRecv) {
			GX_DEBUG("0 > oneRecv recv() error:%d", errno);
			return -1;

		}else if (0 == oneRecv){
//			GX_DEBUG("0 == oneRecv recv() error:%d", errno);
			return -2;
		}
		g_nw_search_run_index = 504;
		//struct in_addr inaddr;
		//inaddr.s_addr = from->sin_addr.s_addr;
		//GX_DEBUG("recv msg from_addr %s", inet_ntoa(inaddr));

		from->sin_addr.s_addr = INADDR_BROADCAST;
		//inaddr.s_addr = from->sin_addr.s_addr;
		//GX_DEBUG("recv msg from_addr %s", inet_ntoa(inaddr));

		havedRecv += oneRecv;
		if (havedRecv == length || havedRecv > 0) {
			return 0;
		}
	}
	g_nw_search_run_index = 505;
	//GX_DEBUG("GXRecvPacket fail errno: %d",errno);
	return -3;
}