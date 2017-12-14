#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <qsdk/audiobox.h>
#include <qsdk/event.h>
#include <alsa/asoundlib.h>
#include "abctrl.h"

void usage(char *cmd)
{
	printf(
			("Usage: %s [OPTION]... [FILE]...\n"
			 "start					Start audiobox service.\n"
			 "stop					Stop audiobox service.\n"
			 "list					List all available channels with the information for that channel\n"
			 "debug					List debuging information about audio devices and channels\n"
			 "record					Record PCM audio from an input channel\n"
			 "\t-c, --channel			Specify input channel to record sound from.\n"
			 "\t-v, --volume=0~100		Specify logic channel volume.\n"
			 "\t-o, --output=FILE		Specify the output file.\n"
			 "\t--no-ref			use audio_read_frame() instead of audio_get_frame()\n"
			 "\t--enable-aec			enable AEC algorithm to Cancel Echo when recording audio\n"
			 "\t-w, --bit-width			Specified bit width\n"
			 "\t-s, --sample-rate		Specified sampling rate\n"
			 "\t-n, --channel-count		Specified channel count\n"
			 "\t-t, --time			Specified record time\n"
			 "play					Playback PCM audio\n"
			 "\t-c, --channel			Specify output channel to playback sound to\n"
			 "\t-v, --volume=0~100		Specify logic channel volume.\n"
			 "\t-d, --input=FILE		Specify the input file.\n"
			 "\t-w, --bit-width			Specified bit width\n"
			 "\t-s, --sample-rate		Specified sampling rate\n"
			 "\t-n, --channel-count		Specified channel count\n"
			 "mute					Mute a physical channel.\n"
			 "\t-c, --channel			Specify mute channel\n"
			 "unmute					Unmute a physical channel.\n"
			 "\t-c, --channel			Specify unmute channel\n"
			 "set					Set channel format.\n"
			 "\t-c, --channel			Specified channel name\n"
			 "\t-w, --bit-width			Specified bit width\n"
			 "\t-s, --sample-rate		Specified sampling rate\n"
			 "\t-n, --channel-count		Specified channel count\n"
			 "encode					encode after record\n"
			 "\t-o, --output			output file\n"
			 "\t-d, --input			input file\n"
			 "\t--de-noise			denoising\n"
			 "\t-e, --de-echo			to echo\n"
			 "\t-w, --bit-width			Specified input/output stream bit width\n"
			 "\t-s, --sample-rate		Specified input/output stream sampling rate\n"
			 "\t-n, --channel-count		Specified input/output stream channel count\n"
			 "\t-t, --format			Specified format for encode\n"
			 "\t\tAUDIO_CODEC_PCM		0\n"
			 "\t\tAUDIO_CODEC_G711U	1\n"
			 "\t\tAUDIO_CODEC_G711A	2\n"
			 "\t\tAUDIO_CODEC_ADPCM	3\n"
			 "\t\tAUDIO_CODEC_G726	4\n"
			 "\t\tAUDIO_CODEC_MP3		5\n"
			 "\t\tAUDIO_CODEC_SPEEX	6\n"
			 "\t\tAUDIO_CODEC_AAC		7\n"
			 "decode					encode after record\n"
			 "\t-o, --output			output file\n"
			 "\t-d, --input			input file\n"
			 "\t--de-noise			denoising\n"
			 "\t-e, --de-echo			to echo\n"
			 "\t-w, --bit-width			Specified input/output stream bit width\n"
			 "\t-s, --sample-rate		Specified input/output stream sampling rate\n"
			 "\t-n, --channel-count		Specified input/output stream channel count\n"
			 "\t-t, --format			Specified format for encode\n"
			 "\t\tAUDIO_CODEC_PCM		0\n"
			 "\t\tAUDIO_CODEC_G711U	1\n"
			 "\t\tAUDIO_CODEC_G711A	2\n"
			 "\t\tAUDIO_CODEC_ADPCM	3\n"
			 "\t\tAUDIO_CODEC_G726 	4\n"
			 "\t\tAUDIO_CODEC_MP3		5\n"
			 "\t\tAUDIO_CODEC_SPEEX	6\n"
			 "\t\tAUDIO_CODEC_AAC		7\n"
			 )
	, cmd);
}

int audio_list_pcminfo(void)
{
	int card, err, i;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *hardwareParams = NULL;
	snd_output_t *log;

	card = -1;
	if (snd_card_next(&card) < 0 || card < 0) {
		ABCTL_ERR("no soundcards found...");
		return -1;
	}
	snd_output_stdio_attach(&log, stderr, 0);

	printf("**** List of Hardware Devices Pcminfo ****\n");
	while (card >= 0) {
		char name[32];
		sprintf(name, "hw:%d,0", card);
		printf("Card %d:\n", card);
		for (i = 0; i < 2; i++) {
			if ((err = snd_pcm_open(&handle, name, i, 0)) < 0) {
				ABCTL_ERR("pcm open (%i): %s", card, snd_strerror(err));
				goto next_card;
			}

			err = snd_pcm_hw_params_malloc(&hardwareParams);
			if (err < 0) {
				ABCTL_ERR("Failed to allocate ALSA hardware parameters!\n");
				snd_pcm_close(handle);
				goto next_card;
			}

			err = snd_pcm_hw_params_any(handle, hardwareParams);
			if (err < 0) {
				ABCTL_ERR("Unable to configure hardware: %s\n", snd_strerror(err));
				snd_pcm_hw_params_free(hardwareParams);
				snd_pcm_close(handle);
				goto next_card;
			}

			printf("%s:\n", (i == 0) ? "playback" : "capture");
			snd_pcm_hw_params_dump(hardwareParams, log);
			
			snd_pcm_hw_params_free(hardwareParams);
			snd_pcm_close(handle);
		}
next_card:
		if (snd_card_next(&card) < 0) {
			ABCTL_ERR("snd_card_next");
			break;
		}
	}

	snd_output_close(log);
	return 0;
}

int audio_list_debuginfo(void)
{
	audio_dbg_print_info();
	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		usage(argv[0]);
		return -1;
	}

	if (!strcmp(argv[1], "start"))
		audio_start_service();
	else if (!strcmp(argv[1], "stop"))
		audio_stop_service();
	else if (!strcmp(argv[1], "list"))
		audio_list_pcminfo();
	else if (!strcmp(argv[1], "debug"))
		audio_list_debuginfo();
	else if (!strcmp(argv[1], "play"))
		playback(--argc, ++argv);
	else if (!strcmp(argv[1], "record"))
		capture(--argc, ++argv);
	else if (!strcmp(argv[1], "mute"))
		mute(--argc, ++argv);
	else if (!strcmp(argv[1], "unmute"))
		unmute(--argc, ++argv);
	else if (!strcmp(argv[1], "set"))
		set(--argc, ++argv);
	else if (!strcmp(argv[1], "encode")) {
		encode(--argc, ++argv);
	}
	else if (!strcmp(argv[1], "decode"))
		decode(--argc, ++argv);
	else if (!strcmp(argv[1], "read"))
		readi(--argc, ++argv);

	return 0;
}
