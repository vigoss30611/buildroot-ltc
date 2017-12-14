#ifndef _REGISTER_LOGIN_H_
#define _REGISTER_LOGIN_H_

int lib_zhinuo_init(void *param);

int lib_zhinuo_start();
int lib_zhinuo_stop();

int ZhiNuo_isActivated(int socketfd);
void* ZhiNuo_Session_Thread(void *param);

#endif
