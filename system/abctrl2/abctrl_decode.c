#include <stdio.h>
#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <fr/libfr.h>
#include <qsdk/codecs.h>
#include "abctrl.h"

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

#define ENCODE_FRAME	4096

static int format_to_byte(int format)
{
	switch (format) {
		case 32:
		case 24:
			return 4;
		case 16:
			return 2;
		case 8:
			return 1;
		default:
			ABCTL_ERR("Format to byte err: %d\n", format);
			return -1;
	}
}

static int get_frame_size(codec_fmt_t *fmt)
{
	if (!fmt)
		return -1;
	
	return (fmt->channel * format_to_byte(fmt->bitwidth));
}

static float get_convert_ratio(int type)
{
	switch (type) {
	 	case AUDIO_CODEC_PCM:
			return 1;
		case AUDIO_CODEC_G711U:
		case AUDIO_CODEC_G711A:
			return 2;
		default:
			ABCTL_ERR("Get convert ratio err: %d\n", type);
			return -1;
	}
}

static int get_outlen(codec_info_t *fmt, int len)
{
	float ratio;

	if (!fmt)	
		return -1;
	
	ratio = get_convert_ratio(fmt->in.codec_type);
	if (ratio < 0)
		return -1;

	return (ratio * ((fmt->out.sample_rate * get_frame_size(&fmt->out) * len) / \
		(fmt->in.sample_rate * get_frame_size(&fmt->in))));
}

static int safe_read(int fd, void *buf, size_t count)
{
	int result = 0, res;

	while (count > 0) {
		if ((res = read(fd, buf, count)) == 0)
			break;
		if (res < 0)
			return result > 0 ? result : res;
		count -= res;
		result += res;
		buf = (char *)buf + res;
	}
	return result;
}

int invoke_dec_codec(char *outfile, char *infile, codec_info_t *fmt)
{
	void *handle;
	codec_addr_t addr;
	int fdin, fdout;
	int length, total_len;
	int tostdout = 0;
	int tostdin = 0;
	int ret;
	struct wav_header header;

	if (!fmt)
		return -1;

	handle = codec_open(fmt);
	if (!handle)
		return -1;
	
	addr.len_in = ENCODE_FRAME;
	addr.len_out = 60 * ENCODE_FRAME;//get_outlen(fmt, addr.len_in);
	addr.in = (char *)malloc(addr.len_in);
	addr.out = (char *)malloc(addr.len_out);
	
	if (infile) {
		fdin = open(infile, O_RDONLY);
		if (fdin < 0)
			return -1;
	} else {
		fdin = fileno(stdin);
		tostdin = 1;
	}

	if (outfile) {
		fdout = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0666);
		if (fdout < 0)
			return -1;
	} else {
		fdout = fileno(stdout);
		tostdout = 1;
	}

	//add the head for wav
	total_len = addr.len_out * 60;	
	header.riff_id = ID_RIFF;
	header.riff_sz = 0;
	header.riff_fmt = ID_WAVE;
	header.fmt_id = ID_FMT;
	header.fmt_sz = 16;
	header.audio_format = FORMAT_PCM;
	header.num_channels = fmt->out.channel;
	header.sample_rate = fmt->out.sample_rate;
	header.bits_per_sample = (fmt->out.bitwidth == 16) ? 16 : 32;
	header.byte_rate = (header.bits_per_sample / 8) * fmt->out.channel * fmt->out.sample_rate;
	header.block_align = fmt->out.channel * (header.bits_per_sample / 8);
	header.data_id = ID_DATA;
	header.data_sz = total_len * header.block_align;
	header.riff_sz = header.data_sz + sizeof(header) - 8;

	write(fdout, &header, sizeof(struct wav_header));

	do {
		addr.len_in = safe_read(fdin, addr.in, addr.len_in);
		length = codec_decode(handle, &addr);
		if (length < 0) {
			ABCTL_ERR("length %d\n", length);
			break;
		}
		
		if (write(fdout, addr.out, length) != length) {
			ABCTL_ERR("write: %d\n", length);
			break;
		}

	} while (addr.len_in == ENCODE_FRAME);

	do {
		ret = codec_flush(handle, &addr, &length);
		if (ret == CODEC_FLUSH_ERR)
			break;

		if (write(fdout, addr.out, length) != length)
			break;
	} while (ret == CODEC_FLUSH_AGAIN);

	if (!tostdin)
		close(fdin);
	if (!tostdout)
		close(fdout);
	codec_close(handle);
	return 0;
}

#define OPT_DE_NOISE	1

int decode(int argc, char **argv)
{
	int option_index;
	char const short_options[] = "o:d:ew:s:n:t:b:";
	struct option long_options[] = {
		{ "output", 0, NULL, 'o' },
		{ "input", 0, NULL, 'd' },
		{ "de-noise", 0, NULL, OPT_DE_NOISE },
		{ "de-echo", 0, NULL, 'e' },
		{ "bit-width", 0, NULL, 'w' },
		{ "sample-rate", 0, NULL, 's' },
		{ "channel-count", 0, NULL, 'n' },
		{ "format", 0, NULL, 't' },
		{ "bit-rate", 0, NULL, 'b' },
		{ 0, 0, 0, 0 },
	};
	int ch;
	char *outfile = NULL;
	char *infile = NULL;
	char *p = NULL;
	int de_noise = 0, de_echo = 0;
	codec_info_t fmt = {
		.in = {
			.codec_type = AUDIO_CODEC_PCM,
			.effect = 0,
			.channel = 2,
			.sample_rate = 48000,
			.bitwidth = 32,
			.bit_rate = 64000,
		},
		.out = {
			.codec_type = AUDIO_CODEC_PCM,
			.effect = 0,
			.channel = 2,
			.sample_rate = 48000,
			.bitwidth = 32,
			.bit_rate = 64000,
		},
	};

	while ((ch = getopt_long(argc, argv, short_options,
					long_options, &option_index)) != -1) {
		switch (ch) {
			case 'o':
				outfile = optarg;
				break;
			case 'd':
				infile = optarg;
				break;
			case OPT_DE_NOISE:
				ABCTL_DBG("de_noise\n");
				de_noise = 1;
				break;
			case 'e':
				ABCTL_DBG("de_echo\n");
				de_echo = 1;
				break;
			case 'w':
				fmt.in.bitwidth = strtol(optarg, &p, 0);
				if (*p == ':') {
					p++;
					fmt.out.bitwidth = strtol(p, NULL, 0);
				}
				break;
			case 's':
				fmt.in.sample_rate = strtol(optarg, &p, 0);
				if (*p == ':') {
					p++;
					fmt.out.sample_rate = strtol(p, NULL, 0);
				}
				break;
			case 'n':
				fmt.in.channel = strtol(optarg, &p, 0);
				if (*p == ':') {
					p++;
					fmt.out.channel = strtol(p, NULL, 0);
				}
				break;
			case 't':
				fmt.in.codec_type = strtol(optarg, &p, 0);
				if (*p == ':') {
					p++;
					fmt.out.codec_type = strtol(p, NULL, 0);
				}
				break;
			case 'b':
				fmt.in.bit_rate = strtol(optarg, &p, 0);
				if (*p == ':') {
					p++;
					fmt.out.bit_rate = strtol(p, NULL, 0);
				}
				break;
			default:
				ABCTL_ERR("Try `ctrlab --help' for more information.\n");
				return -1;
		}
	};

	ABCTL_DBG(
		   ("outfile: %s\n"
		   	"infile: %s\n"
			"fmt.in.codec_type: %d\n"
			"fmt.in.bitwidth: %d\n"
			"fmt.in.samplingrate: %d\n"
			"fmt.in.channels: %d\n"
			"fmt.in.bitrate: %d\n"
			"fmt.out.codec_type: %d\n"
			"fmt.out.bitwidth: %d\n"
			"fmt.out.samplingrate: %d\n"
			"fmt.out.channels: %d\n"
			"fmt.out.bitrate: %d\n"),
		   outfile, infile, fmt.in.codec_type, fmt.in.bitwidth, fmt.in.sample_rate,
		   fmt.in.channel, fmt.in.bit_rate, fmt.out.codec_type, fmt.out.bitwidth, fmt.out.sample_rate,
		   fmt.out.channel, fmt.out.bit_rate);
	
	return invoke_dec_codec(outfile, infile, &fmt);
}
