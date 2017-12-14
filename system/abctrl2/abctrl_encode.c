#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <qsdk/codecs.h>
#include <malloc.h>
#include "abctrl.h"

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
			return 0.5;
		case AUDIO_CODEC_AAC:
			return 1;
		default:
			ABCTL_ERR("Get convert ratio err: %d\n", type);
			return -1;
	}
}

#define divup(a, b) ((a + b - 1) / b)

static int get_outlen(codec_info_t *fmt, int len)
{
	float ratio;
	int src_frame_size;
	int dst_frame_size;

	if (!fmt)	
		return -1;
	
	ratio = get_convert_ratio(fmt->out.codec_type);
	if (ratio < 0)
		return -1;
	
	src_frame_size = divup(len, get_frame_size(&fmt->in));
	
	dst_frame_size = divup(src_frame_size * fmt->out.sample_rate, fmt->in.sample_rate);
	return (ratio * dst_frame_size * get_frame_size(&fmt->out));
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

static int invoke_co_codec(char *outfile, char *infile, codec_info_t *fmt)
{
	void *handle;
	codec_addr_t addr;
	int fdin, fdout;
	int length;
	int tostdout = 0;
	int tostdin = 0;
	int ret;

	if (!fmt)
		return -1;

	handle = codec_open(fmt);
	if (!handle)
		return -1;
	
	addr.len_in = ENCODE_FRAME;
	addr.len_out = 10*4096;//get_outlen(fmt, addr.len_in);
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

	do {
		addr.len_in = read(fdin, addr.in, addr.len_in);
		if (!addr.len_in)
			break;

		length = codec_encode(handle, &addr);
		if (length < 0)
			break;

		if (write(fdout, addr.out, length) != length)
			break;

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

int encode(int argc, char **argv)
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
			.bit_rate = 128000,
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
	
	return invoke_co_codec(outfile, infile, &fmt);
}
