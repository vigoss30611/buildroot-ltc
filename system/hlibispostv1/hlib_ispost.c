#include "hlib_ispost.h"

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

static int g_IspostFd = -1;

void ispost_debug(const char *function, int line, const char* format, ...)
{
	char _message_[256];
	va_list args;

	va_start(args, format);

	vsprintf(_message_, format, args);

	va_end(args);

	printf("%s(%d): %s", function, line, _message_);
}

int ispost_open(void)
{
	if (g_IspostFd > 0)
		return 0;

	g_IspostFd = open(ISPOST_NODE, O_RDWR);
	if (g_IspostFd < 0)
	{
		ISPOST_DEBUG("ispost open fail\n");
		return -1;
	}
	return 0;
}

int ispost_close(void)
{
	if (g_IspostFd < 0)
	{
		ISPOST_DEBUG("ispost is not open\n");
		return -1;
	}

	close(g_IspostFd);
	g_IspostFd = -1;

	return 0;
}

int ispost_get_env(void)
{
	int ret;
	int val = 0;

	if (g_IspostFd < 0)
	{
		ISPOST_DEBUG("ispost is not open\n");
		return -1;
	}

	ret = ioctl(g_IspostFd, ISPOST_ENV, &val);
	if (ret < 0)
	{
		ISPOST_DEBUG("IOCTL(ISPOST_ENV) fail\n");
		return -1;
	}
	return 0;
}

int ispost_trigger(ISPOST_USER* pIspostUser)
{
	int ret;

	if (g_IspostFd < 0)
	{
		ISPOST_DEBUG("ispost is not open\n");
		return -1;
	}

	ret = ioctl(g_IspostFd, ISPOST_FULL_TRIGGER, pIspostUser);
	if (ret < 0)
	{
		ISPOST_DEBUG("IOCTL(ISPOST_TRIGGER) fail\n");
		return -1;
	}

	return ret;
}

