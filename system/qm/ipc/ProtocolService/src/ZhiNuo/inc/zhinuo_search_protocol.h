#ifndef ZHINUO_SEARCH_PROTOCOL_H
#define ZHINUO_SEARCH_PROTOCOL_H

// void ZhiNuo_SetHandle(int handle);
// 
// int zhiNuo_search_start();
// int zhiNuo_search_stop();
// 
// int daHua_search_start();
// int daHua_search_stop();


//
int zhiNuo_CreateSearchSocket(int *socktype);
int zhiNuo_ProcessRequest(int handle, int sockfd, int socktype);
void zhiNuo_SendEndProc(int sockfd);

int daHua_CreateSearchSocket(int *socktype);
int daHua_ProcessRequest(int handle, int sockfd, int socktype);
void daHua_SendEndProc(int sockfd);

#endif

