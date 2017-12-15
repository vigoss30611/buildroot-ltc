#include <stdarg.h>
#include <stdio.h>
#include "logging.h"

#define UNUSED __attribute__((unused))

int debug_level = LOG_NOTICE;

void applog(int level, char *fmt, ...)
{
	if (level > debug_level)
		return;
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	printf("\n");
	va_end(ap);
}
