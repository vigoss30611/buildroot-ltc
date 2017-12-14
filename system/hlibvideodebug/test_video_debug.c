#include <unistd.h>
#include "video_debug.h"

int main(int argc, char**argv)
{
	int opt;
	int mode = 0;

	while ((opt = getopt(argc, argv, "m:")) != -1) {
		switch (opt) {
		case 'm':
			mode = atoi(optarg);
			break;

		default:
			fprintf(stderr, "Invalid option '-%c'\n", opt);
			return 1;
		}
	}

	struct timespec start = {0};
	perf_measure_start(&start);

	video_dbg_init(mode);
	video_dbg_update();

	video_dbg_mode_print();

	video_dbg_deinit();

	perf_measure_stop(__func__, &start);
}
