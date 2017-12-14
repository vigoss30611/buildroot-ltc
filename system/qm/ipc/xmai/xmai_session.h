#ifndef _XMAI_SESSION_H_
#define _XMAI_SESSION_H_

int lib_xmai_init(void *param);

int lib_xmai_stop();

int XMai_isActivated(int socketfd);

void *XMaiSessionThread(void *param);


#endif
