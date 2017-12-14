#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <fr/libfr.h>
#include <qsdk/audiobox2.h>
#include <qsdk/event.h>
#include <sys/time.h>
#include "abctrl.h"
#include <sys/types.h>
#include <sys/stat.h> 

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define FORMAT_PCM 1

struct wav_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t riff_fmt;
    uint32_t fmt_id;
    uint32_t fmt_sz;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_sz;
};

static int get_frame_size(audio_fmt_t *fmt)
{
	int bitwidth;

	if (!fmt)
		return -1;
	
	bitwidth = ((fmt->bitwidth > 16) ? 32 : fmt->bitwidth);
	return (fmt->channels * (bitwidth >> 3));
}

#define OPT_NO_REF	1
#define OPT_EN_AEC	2
#define OPT_EN_TIME	3
int capture(int argc, char **argv)
{
	int option_index;
	char const short_options[] = "c:v:o:w:s:n:t:";
	struct option long_options[] = {
		{ "channel", 0, NULL, 'c' },
		{ "volume", 0, NULL, 'v' },
		{ "output", 0, NULL, 'o' },
		{ "no-ref", 0, NULL, OPT_NO_REF },
		{ "bit-width", 0, NULL, 'w' },
		{ "sample-rate", 0, NULL, 's' },
		{ "channel-count", 0, NULL, 'n' },
		{ "time", 0, NULL, 't' },
		{ "enable-aec", 0, NULL, OPT_EN_AEC	},
		{ "enable-time", 0, NULL, OPT_EN_TIME },
		{ 0, 0, 0, 0 },
	};
	struct wav_header header;
	int ch, ret = 0, volume = 100, use_ref = 1, en_aec = 0, en_time = 0;
	int handle, len, fd;
	char pcm_name[32] = "default_mic";
	char *wavfile = NULL;
	char *audiobuf = NULL;
	int time = 10;
	long total_len, needed;
	int timestamp = 0;
	uint64_t timestamp_before = 0, time_stamp = 0;
	struct fr_buf_info buf;
	audio_fmt_t fmt = {
		.channels = 2,
		.bitwidth = 32,
		.samplingrate = 48000,
		.sample_size = 1024,
	};
	audio_fmt_t real;
	int length;
	int module;
		
	while ((ch = getopt_long(argc, argv, short_options,
					long_options, &option_index)) != -1) {
		switch (ch) {
			case 'c':
				sprintf(pcm_name, "%s", optarg);
				break;
			case 'v':
				volume = strtol(optarg, NULL, 0);
				break;
			case 'o':
				wavfile = optarg;
				break;
			case OPT_NO_REF:
				use_ref = 0;
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
			case 't':
				time = strtol(optarg, NULL, 0);
				break;
			case OPT_EN_AEC:
				en_aec = 1;
				break;
			case OPT_EN_TIME:
				en_time = 1;
				break;
			default:
				ABCTL_ERR("Try `ctrlab --help' for more information.\n");
				return -1;
		}
	};

	ABCTL_DBG(
		   ("pcm_name: %s\n"
			"volume: %d\n"
			"wavfile: %s\n"
			"fmt.bitwidth: %d\n"
			"fmt.samplingrate: %d\n"
			"fmt.channels: %d\n"
			"time: %ds\n"),
		   pcm_name, volume, wavfile, fmt.bitwidth, 
		   fmt.samplingrate, fmt.channels, time);
	
	if (en_time) {
		timestamp = fmt.sample_size * 1000 / fmt.samplingrate;
		ABCTL_ERR("abctrl capture timestamp %d\n", timestamp);
	}
	
	audio_set_format(pcm_name, &fmt);

	handle = audio_get_channel(pcm_name, &fmt, CHANNEL_BACKGROUND);
	if (handle < 0) {
		ABCTL_ERR("Get channel err: %d\n", handle);
		return -1;
	}

	ret = audio_get_master_volume(pcm_name);
	if (ret < 0) {
		ABCTL_ERR("Get volume err: %d\n", ret);
		audio_put_channel(handle);
		return -1;
	}

	ABCTL_ERR("Get volume: %d\n", ret);
	if (ret != volume) {
		ret = audio_set_master_volume(pcm_name, volume);
		if (ret < 0) {
			ABCTL_ERR("Set volume err: %d\n", ret);
			audio_put_channel(handle);
			return -1;
		}
		ABCTL_ERR("Set volume: %d\n", volume);
	}

	ret = audio_get_chn_fmt(handle, &fmt);
	if (ret < 0) {
		ABCTL_ERR("Get format err: %d\n", ret);
		audio_put_channel(handle);
		return -1;
	}

	if (wavfile) {
		fd = open(wavfile, O_CREAT | O_WRONLY | O_TRUNC, 0666);
		if (fd < 0) {
			ABCTL_ERR("Please output right wav file path: %d\n", fd);
			audio_put_channel(handle);
			return -1;
		}
	} else
		fd = fileno(stdout);

	length = get_frame_size(&fmt) * fmt.sample_size;
	audiobuf = (char*)malloc(length);
	if(NULL == audiobuf) {
		ABCTL_ERR("audiobuf malloc failed!!\n");
		close(fd);
		audio_put_channel(handle);
		return -1;
	}

	if(en_aec) {
		module = audio_get_dev_module(pcm_name);
		if(module < 0) {
			audio_put_channel(handle);
			return -1;
		}
		audio_set_dev_module(pcm_name, module | MODULE_AEC);
	}

	total_len = 0;
	needed = get_frame_size(&fmt) * fmt.samplingrate * time / audio_get_codec_ratio(&fmt);
	
	header.riff_id = ID_RIFF;
	header.riff_sz = 0;
	header.riff_fmt = ID_WAVE;
	header.fmt_id = ID_FMT;
	header.fmt_sz = 16;
	header.audio_format = FORMAT_PCM;
	header.num_channels = fmt.channels;
	header.sample_rate = fmt.samplingrate;
	header.bits_per_sample = (fmt.bitwidth == 16) ? 16 : 32;
	header.byte_rate = (header.bits_per_sample / 8) * fmt.channels * fmt.samplingrate;
	header.block_align = fmt.channels * (header.bits_per_sample / 8);
	header.data_id = ID_DATA;
	header.data_sz = needed * header.block_align;
	header.riff_sz = header.data_sz + sizeof(header) - 8;

	ret = write(fd, &header, sizeof(struct wav_header));

	if (use_ref)
		fr_INITBUF(&buf, NULL);
	do {
#if 0
		if (use_ref) {
retry:
			ret = audio_get_frame(handle, &buf);
			if (ret < 0)
				goto retry;

			len = write(fd, buf.virt_addr, buf.size);
			if (len < 0)
				break;

			if (en_time) {
				if (timestamp_before) {
					time_stamp = buf.timestamp - timestamp_before;
					if ((time_stamp < (timestamp - 5)) | (time_stamp > (timestamp + 5)))
						ABCTL_ERR("audio frame timestamp err, timestamp %lld\n", time_stamp);
				}

				timestamp_before = buf.timestamp;
			}

			audio_put_frame(handle, &buf);
		} else {
#endif
			ret = audio_read_frame(handle, audiobuf, length);
			if (ret < 0)                               
				break;

			len = write(fd, audiobuf, ret);
			if (len < 0)
				break;
#if 0
		}
#endif
		total_len += len;
	} while (total_len < needed);
	
	if (wavfile)
		close(fd);
	
	free(audiobuf);
	if(en_aec) {
		module = audio_get_dev_module(pcm_name);
		if(module < 0) {
			audio_put_channel(handle);
			return -1;
		}
		audio_set_dev_module(pcm_name, module & ~(MODULE_AEC));
	}
	audio_put_channel(handle);
	return 0;
}

