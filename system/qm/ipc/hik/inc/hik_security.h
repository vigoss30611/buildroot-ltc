#ifndef _HIK_SECURITY_H_
#define _HIK_SECURITY_H_


int verifyUserID(int userID, struct in_addr *clientIp, struct in_addr *peerSockIp, char *macAddr, UINT32 op);
int userLogin(char *username, char *passwd,struct in_addr *clientIp, struct in_addr *socketIp,char *macAddr, int *userID, UINT32 sdkVersion);
int userLogout(BOOL bLocalUser, int userID, struct in_addr *clientIp, struct in_addr *peerSockIp, char *macAddr);
int sendNetRetval(int connfd, UINT32 retVal);
int userKeepAlive(int userID, struct in_addr *clientIp, struct in_addr *peerSockIp, char *macAddr);


#endif
