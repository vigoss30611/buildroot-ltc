#include <stdio.h>

#include "hlib_ispost.h"


int main(int argc, char **argv)
{
	int ret;

	ret = ispost_open();
	if (ret < 0)
		return -1;

	ret = ispost_get_env();
	if (ret < 0)
		return -1;

	ispost_close();
	return 0;
}
