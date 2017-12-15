#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <miner.h>
#include "knc-asic.h"
#include "knc-transport.h"
#include <logging.h>

int main(int argc, char **argv)
{
	void *ctx;
	char **args = &argv[1];
	const char *devname = NULL;
	
	for (;argc > 1 && *args[0] == '-'; argc--, args++) {
		if (strcmp(*args, "-i") == 0 && argc > 1)  {
			argc--;
			devname = *(++args);
			continue;
		}
		if (strcmp(*args, "-d") == 0)
			debug_level = LOG_DEBUG;
	}

	if (argc != 4) {
		fprintf(stderr, "Usage: %s [options] red green blue\n", argv[0]);
		fprintf(stderr, "	-i devname	Name of the transport device\n");
		exit(1);
	}

	uint8_t request[2];
	uint32_t red = strtoul(*args++, NULL, 0);
	uint32_t green = strtoul(*args++, NULL, 0);
	uint32_t blue = strtoul(*args++, NULL, 0);
	int len = knc_prepare_led(request, 0, sizeof(request), red, green, blue);
	ctx = knc_trnsp_new(devname);
	if (!ctx) {
		fprintf(stderr, "ERROR: Failed to open transport device\n");
		exit(1);
	}
	knc_syncronous_transfer_fpga(ctx, len, request, 0, NULL);

	knc_trnsp_free(ctx);
	
	return 0;
	
}
