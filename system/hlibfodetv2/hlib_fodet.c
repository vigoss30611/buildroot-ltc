#include "hlib_fodet.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

static int g_FodetFd = -1;

void fodet_debug(const char* function, int line, const char* format, ...)
{
    char _message_[256];
    va_list args;

    va_start(args, format);

    vsprintf(_message_, format, args);

    va_end(args);

	printf("[Hlib_fodet]%s(%d): %s", function, line, _message_);
	//printk("%s(%u): %s", function, line, _message_);
}

int fodet_open(void)
{
    if (g_FodetFd > 0)
        return 0;

    g_FodetFd = open(FODET_NODE, O_RDWR);
    if (g_FodetFd < 0)
    {
        FODET_DEBUG("fodet open fail\n");
        return -1;
    }
    return 0;
}

int fodet_close(void)
{
    if (g_FodetFd < 0)
    {
        FODET_DEBUG("fodet is not open\n");
        return -1;
    }

    close(g_FodetFd);
    g_FodetFd = -1;

    return 0;
}

int fodet_get_env(void)
{
    int ret;
    int val = 0;

    if (g_FodetFd < 0)
    {
        FODET_DEBUG("fodet is not open\n");
        return -1;
    }

    ret = ioctl(g_FodetFd, FODET_ENV, &val);
    if (ret < 0)
    {
        FODET_DEBUG("IOCTL(FODET_ENV) fail\n");
        return -1;
    }
    return 0;
}

int fodet_initial(FODET_USER* pFodetUser)
{
    int ret;

    if (g_FodetFd < 0)
    {
        FODET_DEBUG("fodet is not open\n");
        return -1;
    }

    ret = ioctl(g_FodetFd, FODET_INITIAL, pFodetUser);
    if (ret < 0)
    {
        FODET_DEBUG("IOCTL(FODET_INITIAL) fail\n");
        return -1;
    }
    return 0;
}

int fodet_object_detect(FODET_USER* pFodetUser)
{
    int ret;

    if (g_FodetFd < 0)
    {
        FODET_DEBUG("fodet is not open\n");
        return -1;
    }

    ret = ioctl(g_FodetFd, FODET_OBJECT_DETECT, pFodetUser);
    if (ret < 0)
    {
        FODET_DEBUG("IOCTL(FODET_OBJECT_DETECT) fail\n");
        return -1;
    }
    return 0;
}

int fodet_get_coordinate(FODET_USER* pFodetUser)
{
	int ret;

	if (g_FodetFd < 0)
	{
		FODET_DEBUG("fodet is not open\n");
		return -1;
	}

	ret = ioctl(g_FodetFd, FODET_GET_COORDINATE, pFodetUser);
	if (ret < 0)
	{
		FODET_DEBUG("IOCTL(FODET_GET_COORDINATE) fail\n");
		return -1;
	}
	return 0;
}


