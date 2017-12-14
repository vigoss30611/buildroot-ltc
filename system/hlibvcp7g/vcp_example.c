/*
 * =====================================================================================
 *
 *       Filename:  vcp7g_example.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年12月21日 09时55分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
/*
 * vcp7g_example.c
 * Copyright (C) 2016 Xiao Yongming <Xiao Yongming@ben-422-infotm>
 *
 * Distributed under terms of the MIT license.
 */
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "vcp7g.h"

#define SAMPLE_BLOCK_SIZE 8*1024
void usage(char *cmd)
{
	printf(
			("Usage: %s <INPUT_FILE> <OUTPUT_FILE> [OPTION]...\n"
			 "\t-w, --bit-width			Specified bit width\n"
			 "\t-s, --sample-rate		Specified sampling rate\n"
			 "\t-n, --channel-count		Specified channel count\n"
			 "\t-d, --use-dsp			Specified use dsp\n"
			 )
	, cmd);
}

int main(int argc, char **argv)
{

	int i, j;
	int ret;
	int len, total_len = 0;
	char *input, *output;
	int in_fd, ou_fd;
	int use_dsp = 1;
	char play_buf[SAMPLE_BLOCK_SIZE] = {0};
	char vcp_buf[SAMPLE_BLOCK_SIZE] = {0};
	struct vcp_object vcp_obj;
	struct codec_format fmt = {
		.channel = 2,
		.bitwidth = 16,
		.sample_rate = 8000,
	};

	int option_index, ch;
	char const short_options[] = "h:w:s:n:d:";
	struct option long_options[] = {
		{ "help", 0, NULL, 'h' },
		{ "bit-width", 0, NULL, 'w' },
		{ "sample-rate", 0, NULL, 's' },
		{ "channel-count", 0, NULL, 'n' },
		{ "use-dsp", 0, NULL, 'd' },
		{ 0, 0, 0, 0 },
	};

	if(argc < 3) {
		usage(argv[0]);
		return -EINVAL;
	}

	input = argv[1];
	output = argv[2];

	while ((ch = getopt_long(argc, argv, short_options,
					long_options, &option_index)) != -1) {
		switch (ch) {
			case 'w':
				fmt.bitwidth = strtol(optarg, NULL, 0);
				break;
			case 's':
				fmt.sample_rate = strtol(optarg, NULL, 0);
				break;
			case 'n':
				fmt.channel = strtol(optarg, NULL, 0);
				break;
			case 'd':
				use_dsp = strtol(optarg, NULL, 0);
				break;
			case 'h':
			default:
				usage(argv[0]);
				return -EINVAL;
		}
	};

	printf(
		   ("input file: %s\n"
			"output file: %s\n"
			"fmt.bitwidth: %d\n"
			"fmt.sample_rate: %d\n"
			"fmt.channel: %d\n"
			"use dsp?: %d\n"),
		    input, output, fmt.bitwidth, fmt.sample_rate, fmt.channel, use_dsp);

	in_fd = open(input, O_RDONLY);
	if(in_fd < 0) {
		printf("%s don't exist\n", input);
		ret = -EINVAL;
		goto out;
	}

	ou_fd = open(output, O_RDWR | O_CREAT | O_TRUNC);
	if(ou_fd < 0) {
		printf("open %s fail\n", output);
		ret = -EIO;
		goto out_close_file;
	}

	ret = vcp7g_init(&vcp_obj, fmt.channel, fmt.sample_rate, fmt.bitwidth, &use_dsp);
	//printf("use dsp = %d, ret = %d\n", use_dsp, ret);
	if(ret < 0) {
		printf("open vcp fail, %d\n", ret);
		goto out_put_channel;
	}


	printf("begin to read %s\n", input);
	while((len = read(in_fd, play_buf, SAMPLE_BLOCK_SIZE)) > 0)
	{

#if 0
		for(i = 0; i < len; i++) {
			for(j = 0; j < 16; j++) {
				if(16*i+j > len)
					break;

				printf("0x%02x ", play_buf[16*i + j]);
			}
			printf("\n");
		}
#endif
		//printf("len = %d\n", len);
		//while (1) {

			ret = vcp7g_capture_process(&vcp_obj, play_buf, vcp_buf, len);
			if(ret < 0) {
				printf("vcp process failed %d\n", ret);
				continue;
				//return -1;
			}


			ret = write(ou_fd, vcp_buf, len);
			if(ret < 0) {
				printf("write back to %s failed %d\n", output, ret);
				continue;
			}
			total_len += len;
		//}

	}
	printf("process %d data from %s\n", total_len, input);
	ret = 0;

	vcp7g_deinit(&vcp_obj);

out_put_channel:

out_close_file:
	if(in_fd >= 0) {
		close(in_fd);
	}

	if(ou_fd >= 0) {
		close(ou_fd);
	}

out:
	return ret;	// no errors
}
