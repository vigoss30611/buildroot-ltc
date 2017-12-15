#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "knc-asic.h"

static void print_usage(const char *prog)
{
	printf("Usage: %s [-v] [command,..]\n", prog);
	puts("  -b --beaglebone  Beaglebone\n"
	     "  -i --ioboard     I/O Board\n"
	     "  -a --asic=N      ASIC card\n"
	);
	exit(1);
}

enum {
	MODE_BEAGLEBONE = 1,
	MODE_IOBOARD,
	MODE_ASIC
} opt_mode = MODE_IOBOARD;

char *opt_device = NULL;

static void parse_opts(int argc, char *argv[])
{
	while (1) {
		static const struct option lopts[] = {
			{ "beaglebone",  0, 0, 'b' },
			{ "ioboard",     0, 0, 'i' },
			{ "asic",        1, 0, 'a' },
			{ "device",	 1, 0, 'd' },
			{ NULL,          0, 0, 0 },
		};
		int c;

		c = getopt_long(argc, argv, "bia:d:", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'b':
			opt_mode = MODE_BEAGLEBONE;
			break;
		case 'i':
			opt_mode = MODE_IOBOARD;
			break;
		case 'a':
			opt_mode = MODE_ASIC;
			static char asic_device[16];
			sprintf(asic_device, "%d-%s", atoi(optarg)+FIRST_ASIC_I2C_BUS, "0050");
			if (!opt_device)
				opt_device = asic_device;
			break;
		case 'd':
			opt_device = optarg;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}

int main(int argc, char **argv)
{
	char filename[PATH_MAX];
	unsigned char buf[32];
	int in = 0;
	
	parse_opts(argc, argv);

	if (!opt_device) {
		if(opt_mode == MODE_BEAGLEBONE)
			opt_device = "0-0050";
		if(opt_mode == MODE_IOBOARD)
			opt_device = "1-0054";
	}

	if (*opt_device == '-' || *opt_device == '/')
		strcpy(filename, opt_device);
	else
		sprintf(filename, "/sys/bus/i2c/devices/%s/eeprom", opt_device);
	
	if (*filename != '-')
		in = open(filename, O_RDONLY);
	if (in < 0) {
		perror("open eeprom");
		exit(1);
	}
	
	
	if (opt_mode == MODE_BEAGLEBONE) {
		read(in, buf, 4);

		if (buf[0] != 0xAA || buf[1] != 0x55 || buf[2] != 0x33 || buf[3] != 0xEE) {
			fprintf(stderr, "%s: Wrong header", argv[0]);
			exit(1);
		}
	}

	if (opt_mode == MODE_ASIC) {
		read(in, buf, 32);
	}

	if (read(in, buf, sizeof(buf)) != sizeof(buf)) {
		fprintf(stderr, "ERROR: No eeprom found\n");
		exit(1);
	}

	unsigned int i;

	for (i = 0; i < sizeof(buf)-1; i++) {
		if (buf[i] == 0xFF)
			break;
	}
	buf[i] = 0;
	if (!buf[0]) {
		fprintf(stderr, "ERROR: No serialnumber found\n");
		exit(1);
	}
	printf("%s\n", buf);

	if (opt_mode == MODE_ASIC) {
		for (i = (32 + sizeof(buf)) / sizeof(buf); i < 1024 / sizeof(buf); i++) {
			if (read(in, buf, sizeof(buf)) != sizeof(buf)) {
				fprintf(stderr, "ERROR: Short eeprom read\n");
				exit(1);
			}
			
		}
	}
	return 0;
}
