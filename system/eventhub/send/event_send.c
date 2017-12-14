#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "event.h"

int main(int argc, char *argv[])
{
	if(argc >= 3) {
		event_send(argv[1], argv[2], strlen(argv[2])+1);
	} else {
		event_send(argv[1], "nodata", strlen("nodata") + 1);
	}

	return 0;
}

