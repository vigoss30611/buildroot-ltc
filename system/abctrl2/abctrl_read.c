#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <alsa/asoundlib.h>
#include "abctrl.h"

#define TEST_BUFF_SIZE (32*1024) 

static unsigned int format_to_alsa(int format)
{
	switch (format) {
		case 32:
			return SND_PCM_FORMAT_S32_LE;
		case 24:
			return SND_PCM_FORMAT_S24_LE;
		case 16:
		default:
			return SND_PCM_FORMAT_S16_LE;
	}
}

int readi(int argc, char **argv)
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
	char* buff;
	int iBuff_size;
	int ch, err = 0, ret = 0, avail = 0;
	snd_pcm_t *phandle;
	snd_pcm_hw_params_t *hw_params;
	char pcm_name[32] = "default";
	int format = 32, channel = 2;
	uint32_t sampling_rate = 48000;

	while ((ch = getopt_long(argc, argv, short_options,
					long_options, &option_index)) != -1) {
		switch (ch) {
			case 'c':
				sprintf(pcm_name, "%s", optarg);
				break;
			case 'w':
				format = strtol(optarg, NULL, 0);
				break;
			case 's':
				sampling_rate = strtol(optarg, NULL, 0);
				break;
			case 'n':
				channel = strtol(optarg, NULL, 0);
				break;
			default:
				ABCTL_ERR("Try 'abctrl' for more information.\n");
				return -1;
		}
	};

	// open a PCM (Capture)
	if ((err = snd_pcm_open(&phandle, pcm_name, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		ABCTL_ERR("cannot open audio device(%s)\n", snd_strerror(err));
		return err;
	}

	// allocate an invalid snd_pcm_hw_params_t using standard malloc.
	snd_pcm_hw_params_alloca(&hw_params);

	if ((err = snd_pcm_hw_params_any(phandle, hw_params)) < 0) {
		ABCTL_ERR("cannot initialize hardware parameter structure\n");
		return err;
	}

	if ((err = snd_pcm_hw_params_set_access(phandle, hw_params, 
					SND_PCM_ACCESS_RW_INTERLEAVED)) < 0 ) {
		ABCTL_ERR("cannot set access type\n");
		return err;
	}

	if ((err = snd_pcm_hw_params_set_format(phandle, hw_params, 
					format_to_alsa(format))) < 0) {
		ABCTL_ERR("cannot set sample format\n");
		return err;
	}

	if ((err = snd_pcm_hw_params_set_channels(phandle, hw_params, channel)) < 0) {
		ABCTL_ERR("cannot set channel count\n");
		return err;
	}

	if ((err = snd_pcm_hw_params_set_rate_near(phandle, hw_params, &sampling_rate, 0)) < 0) {
		ABCTL_ERR("cannot set sample rate\n");
		return err;
	}

	if ((err = snd_pcm_hw_params(phandle, hw_params)) < 0) {
		ABCTL_ERR("cannot set parameters\n");
		return err;
	}

	snd_pcm_prepare(phandle);

	/* Audio Capture Data Buffer */
	iBuff_size = TEST_BUFF_SIZE;
	buff = (char *)malloc(iBuff_size);

	snd_pcm_start(phandle);

	while (1) {
		ret = snd_pcm_wait(phandle, 4000);
		ABCTL_ERR("snd_pcm_wait  ret: %d\n", ret);

		if (!ret) {
			printf("################# ERROR ######################\n");
			printf("################# ERROR ######################\n");
			printf("################# ERROR ######################\n");

			while(1);
		}

		avail = snd_pcm_avail(phandle);
		ABCTL_ERR("snd_pcm_avail  avail: %d\n", avail);

		err = snd_pcm_readi(phandle, buff, avail);
	}

	snd_pcm_drop(phandle);
	snd_pcm_close(phandle);

	free(buff);

	return 0;
}
