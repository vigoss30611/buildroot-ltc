#ifndef _HIK_NETLOGIN_H_
#define _HIK_NETLOGIN_H_

#include "hik_command.h"
#include "hik_configtypes.h"

int challenge_login(int connfd, NET_LOGIN_REQ *reqdata, int *userid, struct sockaddr_in *pClientSockAddr);
int verifyUserPasswd(char *username, char *passwd, struct in_addr *clientIp, char *macAddr, UINT32 sdkVersion, int *userIdx);
int hik_UserInfoInit(USER UserInfo[32]);


#endif
