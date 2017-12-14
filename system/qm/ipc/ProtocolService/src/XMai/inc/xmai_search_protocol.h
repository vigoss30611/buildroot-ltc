#ifndef XMAI_SEARCH_PROTOCOL_H
#define XMAI_SEARCH_PROTOCOL_H

int XMaiCreateSock(int *pSockType);
int XMaiSearchRequestProc(int dmsHandle, int sockfd, int );
void XMaiSearchSendEndMsg(int sockfd);

#endif

