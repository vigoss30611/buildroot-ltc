#include <stdio.h>
#include <stdlib.h>
#ifndef __KERNEL__
#include <stdint.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
//#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <ceva/lib-tl421.h>
#include <ceva/test.h>
#include <ceva/tl421-user.h>

#define MAX_LEN	100*1024
#define MAX_LEN_OUT	705*4096
#ifdef DEBUG_INPUT_FILE
#define AACENC_SAMPLES	4096*255
#define AACENC_OUT_SAMPLES	768*100
#else
#define AACENC_SAMPLES	4096
#define AACENC_OUT_SAMPLES	768
#endif

#ifdef DEBUG_INPUT_FILE
#define AACENC_DEBUG_SAMPLES	100000
#endif

#define TEST_SINGLE_FRAME

struct codec_format {
	int codec_type;
	int effect;
	int channel;
	int sample_rate;
	int bitwidth;
	int bit_rate;
};
typedef struct codec_format codec_fmt_t;

struct codec_info {
	codec_fmt_t in;
	codec_fmt_t out;
};
typedef struct codec_info codec_info_t;

static int adts_get_single_frame(unsigned char *buf, unsigned int buf_size, 
				unsigned char *frame, int *frame_size)
{
	unsigned int size = 0;

	if (!buf || !frame || !frame_size) {
		printf("function params NULL\n");
		return -1;
	}

	while (1) {
		if (buf_size < 7)
			return -1;

		if ((buf[0]==0xff) && ((buf[1]&0xf0)==0xf0)) {
			size |= ((buf[3] & 0x03) << 11);
			size |= buf[4] << 3;
			size |= ((buf[5] & 0xe0) >> 5);
			break;
		}

		buf_size--;
		buf++;
	}

	if (buf_size < size) {
		printf("total size less than frame size\n");
		return -1;
	}

#ifdef TEST_SINGLE_FRAME
	memcpy(frame, buf, size);
#else
	memcpy(frame, buf, size);
#endif
	*frame_size = size;

	return 0;
}

static int adts_get_4_frame(unsigned char *buf, unsigned int buf_size,
			unsigned char *frame, int *offset, int *frame_size)
{
	int i;
	int single_size = 0;

	*frame_size = 0;

	for (i = 0; i < 4; i++) {
		if (adts_get_single_frame(buf, buf_size, frame, &single_size) < 0) {
			return -1;
		}

		if (i == 0)
			*offset = single_size;
		buf += single_size;
		frame += single_size;
		buf_size -= single_size;
		*frame_size += single_size;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	uint8_t *in = NULL;
	uint8_t *out = NULL;
	uint16_t *p = NULL;
	uint16_t *q = NULL;
	int i, len;
	int ret = 0;

	if (argc == 1) {
		return -1;
	} else if (!strcmp(argv[1], "iotest")) {
		ceva_tl421_open(NULL);
		ceva_tl421_set_format(NULL, NULL);
		ceva_tl421_iotest();
		ceva_tl421_close(NULL);
	} else if (!strcmp(argv[1], "aac-decode")) {
		char infile[64], outfile[64];
		FILE *i_fd=NULL, *o_fd=NULL;
		unsigned char ibuf[4096*2], obuf[4096*4], buffer[MAX_LEN], outbuffer[MAX_LEN_OUT];
		int frame_size, output_size;
		unsigned int total_size, first_size;
		unsigned char *indata = NULL;
		int frame_cnt = 0;
		struct dsp_format df;

		ceva_tl421_open(NULL);

		strcpy(df.format, "aac");
		df.codec	= 0;
		df.br		= 64000;
		df.sr		= 48000;
		df.mode		= 1;
		ceva_tl421_set_format(NULL, &df);

		strcpy(infile, argv[2]);
		strcpy(outfile, argv[3]);

		i_fd = fopen(infile, "rb");
		o_fd = fopen(outfile, "wb");
		if (!i_fd || !o_fd) {
			printf("input or output file open failed!\n");
			return -1;
		}

		total_size = fread(buffer, 1, MAX_LEN, i_fd);
		printf("total_size = %d\n", total_size);

		indata = buffer;

#ifdef TEST_SINGLE_FRAME
		while (adts_get_single_frame(indata, total_size, ibuf, &frame_size) == 0) {
			printf("frame_count=%d, frame_size=%d\n", frame_cnt, frame_size);
			/*TODO: output_size is return by kernel, in bytes*/
			output_size = ceva_tl421_decode(NULL, obuf, ibuf, frame_size);
			//output_size = ceva_tl421_decode(NULL, outbuffer, buffer, total_size);
			if (output_size < 0) {
				printf("aac decoder error!\n");
				goto aacdec_exit;
			}

			//if (frame_cnt == 0)
				fwrite(obuf, 1, output_size, o_fd);
			//else
				//fwrite(obuf+4096*3, 1, 4096, o_fd);
			//fwrite(outbuffer, 1, output_size, o_fd);
			fflush(o_fd);
			//printf("output_size=%d\n", output_size);

			total_size -= frame_size;
			indata += frame_size;
			frame_cnt++;
		}
#else
		while (adts_get_4_frame(indata, total_size, ibuf, &first_size, &frame_size) == 0) {
			output_size = ceva_tl421_decode(NULL, obuf, ibuf, frame_size);
			if (output_size < 0) {
				printf("aac decoder error!\n");
				goto aacdec_exit;
			}

			if (frame_cnt == 0)
				fwrite(obuf, 1, output_size, o_fd);
			else
				fwrite(obuf+4096*3, 1, 4096, o_fd);
			fflush(o_fd);

			total_size -= first_size;
			indata += first_size;
			frame_cnt++;
		}
#endif

aacdec_exit:
		ceva_tl421_close(NULL);
		fclose(o_fd);
		fclose(i_fd);

		return 0;

	} else if (!strcmp(argv[1], "aac-encode")) {
		char infile[64], outfile[64], dbgfile[64];
		FILE *i_fd=NULL, *o_fd=NULL;
		codec_info_t fmt;
		uint8_t ibuf[AACENC_SAMPLES], obuf[AACENC_OUT_SAMPLES];
		int32_t input_size, output_size;
#ifdef	DEBUG_INPUT_FILE
		uint8_t dbuf[AACENC_DEBUG_SAMPLES];
		FILE *dbg_input = NULL;
#endif
		struct timeval tva, tvb;
		long usec_total = 0;
		long cnt_total = 0;
		long usec_avg = 0;

		ceva_tl421_open(NULL);

		memset(&fmt, 0, sizeof(codec_info_t));
		memset(ibuf, 0, sizeof(ibuf));
		memset(obuf, 0, sizeof(obuf));
#ifdef	DEBUG_INPUT_FILE
		memset(dbuf, 0, sizeof(dbuf));
#endif
		
		fmt.in.codec_type = 0;
		fmt.in.channel = 2;
		fmt.in.sample_rate = 48000;
		fmt.out.codec_type = 7;
		fmt.out.channel = 2;
		fmt.out.sample_rate = 48000;
		fmt.out.bit_rate = 64000;
		ceva_tl421_set_format(NULL, &fmt);

		strcpy(infile, argv[2]);
		strcpy(outfile, argv[3]);
#ifdef	DEBUG_INPUT_FILE
		strcpy(dbgfile, argv[4]);
#endif

		i_fd = fopen(infile, "rb");
		o_fd = fopen(outfile, "wb");
#ifdef	DEBUG_INPUT_FILE
		dbg_input = fopen(dbgfile, "w");
		if (!dbuf) {
			printf("debug file open failed!\n");
			return -1;
		}
#endif
		if (!i_fd || !o_fd) {
			printf("input or output file open failed!\n");
			return -1;
		}

		while (1) {

			input_size = fread(ibuf, 1, AACENC_SAMPLES, i_fd);
			//printf("input_size = %ld\n", input_size);
			if (input_size < AACENC_SAMPLES) {
				printf("End!\n");
				break;
			}

#ifdef DEBUG_INPUT_FILE
			output_size = ceva_tl421_encode(dbuf, obuf, ibuf, AACENC_SAMPLES);
#else
			output_size = ceva_tl421_encode(NULL, obuf, ibuf, AACENC_SAMPLES);
#endif
			if (output_size < 0)
				goto aacenc_exit;
			//printf("output_size = %ld\n", output_size);

			fwrite(obuf, 1, output_size, o_fd);
			fflush(o_fd);


#ifdef DEBUG_INPUT_FILE
			{
				int i, j;
				short *dbg_ptr = (short *)dbuf;
				short dbg_buf[92220/2];

				for (i = 0; i < (92220/2); i++) {
					dbg_buf[i] = *dbg_ptr++;
				}

				for (i = 0; i < (92220/6148); i++) {
					fprintf(dbg_input, "frame %d;\n", i+1);

					for (j = 0; j < 3074; j++) {
						fprintf(dbg_input, "input buffer[%d] = %d;\n", j, dbg_buf[i*3074+j]);
					}
				}

				fflush(dbg_input);
			}
#endif
		}

aacenc_exit:
		ceva_tl421_close(NULL);
		fclose(i_fd);
		fclose(o_fd);
#ifdef DEBUG_INPUT_FILE
		fclose(dbg_input);
#endif
		printf("aac encode finished\n");
		return 0;

	} else {
		printf("Unknown test command\n");
	}

__exit__:
	if (in)
		free(in);
	if (out)
		free(out);
	p = NULL;

	return 0;
}
