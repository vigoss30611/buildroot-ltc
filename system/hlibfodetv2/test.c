#include <stdio.h>

#include "hlib_fodet.h"


int main(int argc, char **argv)
{
	int ret;

	ret = fodet_open();
	if (ret < 0)
		return -1;

	ret = fodet_get_env();
	if (ret < 0)
		return -1;

	fodet_close();
	return 0;
}
