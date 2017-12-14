#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <qsdk/audiobox2.h>
#include <qsdk/event.h>
#include "abctrl.h"

int unmute(int argc, char **argv)
{
	int option_index;
	char const short_options[] = "c:";
	struct option long_options[] = {
		{ "channel", 0, NULL, 'c' },
		{ 0, 0, 0, 0 },
	};
	int ch, mute, ret;
	int handle;
	char pcm_name[32] = "default";
		
	
	while ((ch = getopt_long(argc, argv, short_options,
					long_options, &option_index)) != -1) {
		switch (ch) {
			case 'c':
				sprintf(pcm_name, "%s", optarg);
				break;
			default:
				ABCTL_ERR("Try `ctrlab --help' for more information.\n");
				return -1;
		}
	};

	ABCTL_DBG("pcm_name %s\n", pcm_name);
	handle = audio_get_channel(pcm_name, NULL, CHANNEL_BACKGROUND);
	if (handle < 0) {
		ABCTL_ERR("Get channel err: %d\n", handle);
		return -1;
	}
	
	mute = audio_get_mute(handle);
	if (mute < 0) {
		ABCTL_ERR("Get mute status err: %d\n", mute);
		goto err;
	}

	if (!mute)
		ABCTL_INFO("Have already unmuted\n");
	else {
		ret = audio_set_mute(handle, 0);
		if (ret < 0)
			ABCTL_ERR("Mute channel %s err: %d\n", pcm_name, ret);
	}	
err:
	audio_put_channel(handle);
	return 0;
}
