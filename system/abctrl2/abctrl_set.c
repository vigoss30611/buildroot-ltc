#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <qsdk/audiobox2.h>
#include <qsdk/event.h>
#include <string.h>
#include "abctrl.h"

int set(int argc, char **argv)
{
	int option_index;
	char const short_options[] = "c:w:s:n:";
	struct option long_options[] = {
		{ "channel", 0, NULL, 'c' },
		{ "bit-width", 0, NULL, 'w' },
		{ "sample-rate", 0, NULL, 's' },
		{ "channel-count", 0, NULL, 'n' },
		{ 0, 0, 0, 0 },
	};
	int ch, ret;
	int handle;
	char pcm_name[32] = "default";
	audio_fmt_t fmt = {
		.channels = 2,
		.bitwidth = 24,
		.samplingrate = 48000,
		.sample_size = 1024,
	};
	audio_fmt_t set;
	
	while ((ch = getopt_long(argc, argv, short_options,
					long_options, &option_index)) != -1) {
		switch (ch) {
			case 'c':
				sprintf(pcm_name, "%s", optarg);
				break;
			case 'w':
				fmt.bitwidth = strtol(optarg, NULL, 0);
				break;
			case 's':
				fmt.samplingrate = strtol(optarg, NULL, 0);
				break;
			case 'n':
				fmt.channels = strtol(optarg, NULL, 0);
				break;
			default:
				ABCTL_ERR("Try `ctrlab --help' for more information.\n");
				return -1;
		}
	};

	ABCTL_DBG(
		   ("pcm_name: %s\n"
			"fmt.bitwidth: %d\n"
			"fmt.samplingrate: %d\n"
			"fmt.channels: %d\n"),
		   pcm_name, fmt.bitwidth, fmt.samplingrate, fmt.channels);

	handle = audio_get_channel(pcm_name, NULL, CHANNEL_BACKGROUND);
	if (handle < 0) {
		ABCTL_ERR("Get channel err: %d\n", handle);
		return -1;
	}
	
	ret = audio_get_chn_fmt(pcm_name, &set);
	if (ret < 0) {
		ABCTL_ERR("Get Audio format status err: %d\n", ret);
		goto err;
	}

	ABCTL_DBG(
		   ("pcm_name: %s\n"
			"fmt.bitwidth: %d\n"
			"fmt.samplingrate: %d\n"
			"fmt.channels: %d\n"),
		   pcm_name, set.bitwidth, set.samplingrate, set.channels);
	
	if (!memcmp(&set, &fmt, sizeof(audio_fmt_t))) {
		ABCTL_INFO("The same with default setup\n");
		goto err;
	} else {
		/* TODO */
		ret = audio_set_chn_fmt(pcm_name, &fmt);
		if (ret < 0)
			ABCTL_ERR("Set Audio format err\n");
	}

err:
	audio_put_channel(handle);
	return 0;
}
