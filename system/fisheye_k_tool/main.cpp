#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "fe_k_proc.h"

using namespace std;


#define LOCAL_JSON    "/nfs/fe_k_tool_config.json"


int main(int argc, char *argv[])
{
	int ret = 0;

	ret = do_fe_k((char*)(argc > 1? argv[1]: LOCAL_JSON));
	if (ret)
		printf("Fisheye K Failed !!!\n");

	return 0;
}
