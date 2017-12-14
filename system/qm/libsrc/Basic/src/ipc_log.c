#include "ipc.h"

#include <stdarg.h>

#define LOGTAG "IPC"

void ipc_panic(char const *what)
{
    ipclog_ooo("\n\n\n***** ipc_panic(%s) *****\n\n\n", what);
    exit(-1);
}

static const char kLogLevelMark[] = {
    'D', 'I', 'W', 'E', 'A', 'O'
};
static pthread_mutex_t sLogMutex = PTHREAD_MUTEX_INITIALIZER;

void ipclog_log(char *tag, int level, char *fmt, ...)
{
    va_list ap;

    pthread_mutex_lock(&sLogMutex);
    va_start(ap, fmt);
    printf("%s[%c]: ", tag, kLogLevelMark[level]);
    vprintf(fmt, ap);
    va_end(ap);
    pthread_mutex_unlock(&sLogMutex);
}


