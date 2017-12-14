#include "sys_fun_interface.h"
#include "iconv.h"
#include "domain.h"
#include "QMAPI.h"
#include <sys_fun_interface.h>

static pthread_mutex_t dl_lock;
//static int m_nIconvHandCount = 0;
//static void* g_pIconvHandle = NULL;
static pthread_mutex_t sys_md5_lock;
static pthread_mutex_t sys_fun_dl_lock;

int sys_fun_init()
{
    pthread_mutex_init(&dl_lock, NULL);
    pthread_mutex_init(&sys_md5_lock, NULL);
    pthread_mutex_init(&sys_fun_dl_lock, NULL);
    
    return 0;
}

void sys_fun_quit()
{
    pthread_mutex_destroy(&sys_fun_dl_lock);
    pthread_mutex_destroy(&sys_md5_lock);
    pthread_mutex_destroy(&dl_lock);
}


int sys_fun_gethostbyname(const char *pDNS)
{
	return dms_gethostbyname(pDNS);
}




