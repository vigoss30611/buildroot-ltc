#ifndef __SYS_FUN_INTERFACE_H__
#define __SYS_FUN_INTERFACE_H__

#include <stdio.h>
#include <dlfcn.h>

int sys_fun_init();
void sys_fun_quit();

int sys_fun_gethostbyname(const char *pDNS);

#endif


