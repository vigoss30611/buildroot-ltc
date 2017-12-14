#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <sys/reboot.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <linux/input.h>
#include <fr/libfr.h>
#include <qsdk/items.h>
#include <qsdk/sys.h>
#include <qsdk/wifi.h>
#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>
#include <qsdk/videobox.h>
#include <qsdk/demux.h>
#include <qsdk/vplay.h>
#include "d304ft.h"

/*
*change scene
*/
static void video_change_scene(char *video_channel, int scene_mode)
{
	struct v_bitrate_info *bitrate_info = NULL;
	int fps = 15;

	switch (scene_mode) {
		case DAYTIME_STATIC_SCENE:
			if (!strcmp(video_channel, DEFAULT_HIGH_CHANNEL)) {
				bitrate_info = &gernel_scene_info[DAYTIME][STATIC_SCENE][VIDEO_1080P_CHANNEL];
			} else if (!strcmp(video_channel, DEFAULT_LOW_CHANNEL)) {
				bitrate_info = &gernel_scene_info[DAYTIME][STATIC_SCENE][VIDEO_VGA_CHANNEL];
			} else {
				bitrate_info = &gernel_scene_info[DAYTIME][STATIC_SCENE][VIDEO_720P_CHANNEL];
			}
			fps = 10;
			break;
		case DAYTIME_DYNAMIC_SCENE:
			if (!strcmp(video_channel, DEFAULT_HIGH_CHANNEL)) {
				bitrate_info = &gernel_scene_info[DAYTIME][DYNAMIC_SCENE][VIDEO_1080P_CHANNEL];
				fps = 10;
			} else if (!strcmp(video_channel, DEFAULT_LOW_CHANNEL)) {
				bitrate_info = &gernel_scene_info[DAYTIME][DYNAMIC_SCENE][VIDEO_VGA_CHANNEL];
			} else {
				bitrate_info = &gernel_scene_info[DAYTIME][DYNAMIC_SCENE][VIDEO_720P_CHANNEL];
			}
			break;
		case NIGHTTIME_STATIC_SCENE:
			if (!strcmp(video_channel, DEFAULT_HIGH_CHANNEL)) {
				bitrate_info = &gernel_scene_info[NIGHTTIME][STATIC_SCENE][VIDEO_1080P_CHANNEL];
			} else if (!strcmp(video_channel, DEFAULT_LOW_CHANNEL)) {
				bitrate_info = &gernel_scene_info[NIGHTTIME][STATIC_SCENE][VIDEO_VGA_CHANNEL];
			} else {
				bitrate_info = &gernel_scene_info[NIGHTTIME][STATIC_SCENE][VIDEO_720P_CHANNEL];
			}
			fps = 10;
			break;
		case NIGHTTIME_DYNAMIC_SCENE:
			if (!strcmp(video_channel, DEFAULT_HIGH_CHANNEL)) {
				bitrate_info = &gernel_scene_info[NIGHTTIME][DYNAMIC_SCENE][VIDEO_1080P_CHANNEL];
				fps = 10;
			} else if (!strcmp(video_channel, DEFAULT_LOW_CHANNEL)) {
				bitrate_info = &gernel_scene_info[NIGHTTIME][DYNAMIC_SCENE][VIDEO_VGA_CHANNEL];
			} else {
				bitrate_info = &gernel_scene_info[NIGHTTIME][DYNAMIC_SCENE][VIDEO_720P_CHANNEL];
			}
			break;
		default:
			if (!strcmp(video_channel, DEFAULT_HIGH_CHANNEL)) {
				bitrate_info = &gernel_scene_info[DAYTIME][STATIC_SCENE][VIDEO_1080P_CHANNEL];
			} else if (!strcmp(video_channel, DEFAULT_LOW_CHANNEL)) {
				bitrate_info = &gernel_scene_info[DAYTIME][STATIC_SCENE][VIDEO_VGA_CHANNEL];
			} else {
				bitrate_info = &gernel_scene_info[DAYTIME][STATIC_SCENE][VIDEO_720P_CHANNEL];
			}
			fps = 10;
			break;
	}

	if (bitrate_info) {
		video_set_fps(video_channel, fps);
		video_set_bitrate(video_channel, bitrate_info);
	}

	return;
}

inline long long timeval_to_ms(struct timeval timestamp)
{
    return timestamp.tv_sec * 1000LL + timestamp.tv_usec/1000LL;
}

static void *video_send(void *arg)
{
	int clientfd = *(char *)arg;
    int sendBytes;
	struct timeval latest_time;
	struct fr_buf_info fr_send_buf = FR_BUFINITIALIZER;
    int ret, head_len = 0, send_len;
    char header[256], channel_name[16] = {0};
    char *video_buf, *send_buf;
    unsigned char sendBuf[8];
    video_buf = (char *)malloc(VIDEO_BUF_SIZE);
	if (NULL == video_buf) {
		return 0;
	}
	memset(video_buf, 0, VIDEO_BUF_SIZE);

	while (test.video_process_run) {
		switch (test.video_cmd) {
			case CMD_SESSION_START_VIDEO:
				ret = video_enable_channel(DEFAULT_RECORD_CHANNEL);
				if (ret < 0) {
					test.video_cmd = CMD_SESSION_NONE;
					break;
				}

				video_set_bitrate(DEFAULT_RECORD_CHANNEL, &record_scene_info);

				if (strcmp(channel_name, DEFAULT_RECORD_CHANNEL)) {
					head_len = video_get_header(DEFAULT_RECORD_CHANNEL, header, 128);
					if (head_len < 0) {
						test.video_cmd = CMD_SESSION_STOP_VIDEO;
						break;
					}
					memcpy(video_buf, header, head_len);
					sprintf(channel_name, DEFAULT_RECORD_CHANNEL);
				}
				fr_INITBUF(&fr_send_buf, NULL);
				gettimeofday(&latest_time, NULL);
				test.video_cmd = CMD_SESSION_SEND_VIDEO;
				LOGD("%s video_send_thread %s start %d\n", __FUNCTION__, DEFAULT_RECORD_CHANNEL, head_len);
				break;
			case CMD_SESSION_STOP_VIDEO:
				if (channel_name[0] != 0) {
					video_disable_channel(channel_name);
					memset(channel_name, 0, 16);
				}
				test.video_cmd = CMD_SESSION_NONE;
				LOGD("%s video_send_thread %s stop %d \n", __FUNCTION__, DEFAULT_RECORD_CHANNEL, head_len);
				break;
			case CMD_SESSION_SEND_VIDEO:
				ret = video_get_frame(DEFAULT_RECORD_CHANNEL, &fr_send_buf);
				if ((ret < 0) || (fr_send_buf.size <= 0) || (fr_send_buf.virt_addr == NULL)) {
					LOGD("video_get_frame error %d %d \n", ret, fr_send_buf.size);
					break;
				}

				if (fr_send_buf.size > (VIDEO_BUF_SIZE - head_len)) {
					LOGE("video_get_frame big %d \n", fr_send_buf.size);
					video_put_frame(DEFAULT_RECORD_CHANNEL, &fr_send_buf);
					break;
				}

				if (fr_send_buf.priv == VIDEO_FRAME_I) {
					sendBuf[1] = 0;  //i
					memcpy(video_buf + head_len, fr_send_buf.virt_addr, fr_send_buf.size);
					send_buf = video_buf;
					send_len = head_len + fr_send_buf.size;
				} else {
					sendBuf[1] = 1;  //not i
					send_buf = fr_send_buf.virt_addr;
					send_len = fr_send_buf.size;
				}

            	if ((NULL != send_buf) && (0 != send_len)) {
					sendBuf[0] = 1;  //frame type :data
					//frame len
					sendBuf[2] = (send_len & 0xff);
					sendBuf[3] = (send_len & 0xff00) >> 8;
					sendBuf[4] = (send_len & 0xff0000) >> 16;
					sendBuf[5] = (send_len & 0xff000000) >> 24;

					if((sendBytes = sendto(clientfd, sendBuf, 6, 0, NULL, 0)) == -1) {
						LOGE("factory_test------>%d  sendto fail, errno=%d\n", __LINE__, errno);
						break;
	            	}

					int sendDataLen = 0;
					while(send_len >= 1024) {
						sendBytes = sendto(clientfd, send_buf + sendDataLen, 1024, 0, NULL, 0);
						if(sendBytes < 0) {
	            			LOGE("factory_test------>%d  sendto fail, errno=%d\n", __LINE__, errno);
							goto sensorstop;
						}

						if(!test.video_process_run) {
							LOGD("video send STOP2\n");
							goto sensorstop;
						}
						send_len -= sendBytes;
						sendDataLen += sendBytes;
	           	 	}

					if((sendBytes = sendto(clientfd, send_buf + sendDataLen, send_len, 0, NULL, 0)) < 0) {
						LOGE("factory_test------>%d  sendto data fail, errno=%d\n", __LINE__, errno);
						goto sensorstop;
					}
            	}
				video_put_frame(DEFAULT_RECORD_CHANNEL, &fr_send_buf);
				break;
			default:
				break;
		}
	}
sensorstop:
	LOGD("video send over\n");
	close(clientfd);
	if (video_buf) {
		free(video_buf);
	}
	return 0;
}

void *thread_sensor_test(void *arg)
{
	int sendBytes;
	unsigned char sendBuf[8];
    int i;
    int clientfd;
    struct sockaddr_in clientaddr;

    bzero(&clientaddr, sizeof(clientaddr));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = htons(INADDR_ANY);
    clientaddr.sin_port = htons(0);
    clientfd = socket(AF_INET, SOCK_STREAM, 0);

    if(bind(clientfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) < 0) {
        LOGE("bind error\n");
        return NULL;
    }

    struct sockaddr_in svraddr;
    bzero(&svraddr, sizeof(svraddr));
    if(0 == inet_aton(ipaddress, &svraddr.sin_addr)) {//IP地址来自程序的参数
        LOGE("inet_aton err\n");
        return NULL;
    }
    svraddr.sin_family = AF_INET;
    svraddr.sin_port = htons(19000);
    socklen_t svraddrlen = sizeof(svraddr);

    i=connect(clientfd, (struct sockaddr*)&svraddr, svraddrlen);
    while(i < 0) {
        LOGE("connect error ret = %d\n", i);
        sleep(1);
    }
    LOGD("connect success\n");

    sendBuf[0] = 0;	//frame type. 0:start 1:frame date
    sendBuf[1] = 0; //vedio fomate
    if((sendBytes = sendto(clientfd, sendBuf, 2, 0, NULL, 0)) == -1) {
        LOGE("sendto fail, errno=%d\n", errno);
        return NULL;
    }

	test.video_cmd = CMD_SESSION_START_VIDEO;
	video_send((void *)&clientfd);
	LOGD("video test end\n");

    return NULL;
}

static void record_audio(void *arg)
{
	char *file = (char *)arg;
	audio_fmt_t send_fmt;
	struct fr_buf_info fr_send_buf = FR_BUFINITIALIZER;
	struct codec_info send_info;
	struct codec_addr send_addr;
	int ret, send_handle = -1;
	void *codec = NULL;
	long fsize;

	send_fmt.channels = NORMAL_CODEC_CHANNELS;
	send_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
	send_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
	send_fmt.sample_size = 1000;

	memset((char *)&send_info, 0, sizeof(struct codec_info));
	memset((char *)&send_addr, 0, sizeof(struct codec_addr));

	int fd;
	fd = open(file, (O_CREAT | O_WRONLY));
	if (fd < 0) {
		LOGD("Cannot open file %s: %s \n", file, strerror(errno));
	}

	while (1) {
		if (test.audio_cmd == CMD_SESSION_STOP_AUDIO) {
			if (send_handle >= 0) {
				audio_put_channel(send_handle);
				send_handle = -1;
			}
			if (codec) {
				codec_close(codec);
				codec = NULL;
			}
			close(fd);

			LOGD("tutk %s audio send thread quit \n", __FUNCTION__);
			break;
		}

		if (send_handle < 0) {
			send_handle = audio_get_channel(NORMAL_CODEC_RECORD, &send_fmt, CHANNEL_BACKGROUND);
			if (send_handle < 0) {
				LOGE("audio send get %s chanel failed %d \n", NORMAL_CODEC_PLAY, send_handle);
				break;
			}
			audio_get_format(NORMAL_CODEC_RECORD, &send_fmt);
			fr_INITBUF(&fr_send_buf, NULL);
		}

		do {
			ret = audio_get_frame(send_handle, &fr_send_buf);
		} while(ret < 0);
	    if ((ret < 0) || (fr_send_buf.size <= 0) || (fr_send_buf.virt_addr == NULL)) {
			LOGE("tutk send audio get frame failed %d %d %d \n", send_handle, ret, fr_send_buf.size);
			break;
		}

		fsize = lseek(fd, 0L, SEEK_END);

		ret = lseek(fd, fsize, SEEK_SET);
		if (ret < 0) {
			LOGD("lseek failed with %s.\n", strerror(errno));
		}
		if (write(fd, fr_send_buf.virt_addr, fr_send_buf.size) < 0) {
			LOGD("Cannot write to \"%s\": %s \n", file, strerror(errno));
		}
		audio_put_frame(send_handle, &fr_send_buf);
	}
}

static void play_audio(void *arg)
{
	char *file = (char *)arg;
	audio_fmt_t play_fmt;
	struct demux_frame_t demux_frame = {0};
	struct demux_t *demux = NULL;
	struct codec_info play_info;
	struct codec_addr play_addr;
	int play_handle = -1, ret, len, frame_size, offset;
	char play_buf[AUDIO_BUF_SIZE * 16 * 6];
	void *codec = NULL;
	int length;

	memset((char *)&play_info, 0, sizeof(struct codec_info));
	memset((char *)&play_addr, 0, sizeof(struct codec_addr));

	play_fmt.channels = NORMAL_CODEC_CHANNELS;
	play_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
	play_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
	play_fmt.sample_size = 1024;

	frame_size = test.play_fmt.sample_size;

	if (file == NULL) {
		memset(play_buf, 0, AUDIO_BUF_SIZE * 16 * 6);
		play_handle = audio_get_channel(NORMAL_CODEC_PLAY, &play_fmt, CHANNEL_BACKGROUND);
		if (play_handle < 0) {
			LOGE("audio play get %s chanel failed %d \n", NORMAL_CODEC_PLAY, play_handle);
		} else {
			frame_size = play_fmt.sample_size;
			for (len = 0; len < 8; len++) {
				audio_write_frame(play_handle, play_buf, frame_size);
			}
			usleep(200000);
			audio_put_channel(play_handle);
		}
	} else {
		demux = demux_init(file);
		if (demux) {
			do {
				if (play_handle < 0) {
					play_handle = audio_get_channel(NORMAL_CODEC_PLAY, &play_fmt, CHANNEL_BACKGROUND);
					if (play_handle < 0) {
						LOGE("audio play get %s chanel failed %d \n", NORMAL_CODEC_PLAY, play_handle);
						break;
					}
					frame_size = play_fmt.sample_size * 4;
				}

				ret = demux_get_frame(demux, &demux_frame);
				if ((ret < 0) || (demux_frame.data_size == 0)) {
					memset(play_buf, 0, AUDIO_BUF_SIZE * 16);
					len = AUDIO_BUF_SIZE * 16;
					do {
						audio_write_frame(play_handle, play_buf, frame_size);
						len -= frame_size;
						usleep(20000);
					} while (len > 0);
					usleep(80000);
					break;
				}

				if (!codec) {
					LOGE("factory_test-------->init codec \n");
					play_info.in.codec_type = AUDIO_CODEC_PCM;
					play_info.in.channel = demux_frame.channels;
					play_info.in.sample_rate = demux_frame.sample_rate;
					play_info.in.bitwidth = demux_frame.bitwidth;
					play_info.in.bit_rate = play_info.in.channel * play_info.in.sample_rate \
											* GET_BITWIDTH(play_info.in.bitwidth);
                    play_info.out.codec_type = AUDIO_CODEC_PCM;
					play_info.out.channel = play_fmt.channels;
					play_info.out.sample_rate = play_fmt.samplingrate;
					play_info.out.bitwidth = play_fmt.bitwidth;
					play_info.out.bit_rate = play_info.out.channel * play_info.out.sample_rate \
											* GET_BITWIDTH(play_info.out.bitwidth);
					codec = codec_open(&play_info);
					if (!codec) {
						LOGE("audio play open codec decoder failed \n");
						break;
					}
				}
				memset(play_buf, 0, AUDIO_BUF_SIZE * 16 * 6);
				play_addr.in = demux_frame.data;
				play_addr.len_in = demux_frame.data_size;
				play_addr.out = play_buf;
				play_addr.len_out = AUDIO_BUF_SIZE * 16 * 6;
				ret = codec_decode(codec, &play_addr);
				demux_put_frame(demux, &demux_frame);
				if (ret < 0) {
					LOGE("audio play codec decoder failed %d \n",  ret);
					break;
				}
				len = ret;
				if (len < frame_size) {
					len = frame_size;
				}
				offset = 0;
				do {
					ret = audio_write_frame(play_handle, play_buf + offset, frame_size);
					if (ret < 0) {
						LOGE("audio play codec failed %d \n",  ret);
					}
					offset += frame_size;
				} while (offset < len);
				usleep(50000);
				if(CMD_SESSION_STOP_AUDIO == test.audio_cmd) {
					break;
				}
			} while (ret >= 0);

			if (codec != NULL) {
				do {
					play_addr.out = play_buf;
					play_addr.len_out = AUDIO_BUF_SIZE * 16 * 6;
					ret = codec_flush(codec, &play_addr, &length);
					if (ret == CODEC_FLUSH_ERR) {
						break;
					}

					if (length > 0) {
						ret = audio_write_frame(play_handle, play_buf, length);
						if (ret < 0) {
							LOGE("audio play codec failed %d \n",  ret);
						}
					}
				} while (ret == CODEC_FLUSH_AGAIN);
				codec_close(codec);
				codec = NULL;
			}
			if (play_handle >= 0) {
				usleep(1500000);
				audio_put_channel(play_handle);
			}
			demux_deinit(demux);
		}
	}
}

static void play_audio_pcm(void *arg)
{
	char *file = (char *)arg;
	audio_fmt_t play_fmt;
	struct codec_info play_info;
	struct codec_addr play_addr;
	int play_handle = -1, ret, len, frame_size, offset;
	char play_buf[AUDIO_BUF_SIZE * 16 * 6];
	void *codec = NULL;

	memset((char *)&play_info, 0, sizeof(struct codec_info));
	memset((char *)&play_addr, 0, sizeof(struct codec_addr));

	play_fmt.channels = NORMAL_CODEC_CHANNELS;
	play_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
	play_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
	play_fmt.sample_size = 1024;

	frame_size = test.play_fmt.sample_size ;

	if (file == NULL) {
		memset(play_buf, 0, AUDIO_BUF_SIZE * 16 * 6);
		play_handle = audio_get_channel(NORMAL_CODEC_PLAY, &play_fmt, CHANNEL_BACKGROUND);
		if (play_handle < 0) {
			LOGE("audio play get %s chanel failed %d \n", NORMAL_CODEC_PLAY, play_handle);
		} else {
			frame_size = play_fmt.sample_size;
			for (len = 0; len < 8; len++) {
				audio_write_frame(play_handle, play_buf, frame_size);
			}
			usleep(200000);
			audio_put_channel(play_handle);
		}
	} else {
		int fd;
		fd = open(file, (O_CREAT | O_RDWR));
		if (fd < 0) {
			LOGD("Cannot open file %s: %s \n", file, strerror(errno));
		} else {
			do {
				if (play_handle < 0) {
					play_handle = audio_get_channel(NORMAL_CODEC_PLAY, &play_fmt, CHANNEL_BACKGROUND);
					if (play_handle < 0) {
						LOGE("audio play get %s chanel failed %d \n", NORMAL_CODEC_PLAY, play_handle);
						break;
					}
					frame_size = play_fmt.sample_size * 2 * 4;
				}
				ret = read(fd, play_buf, AUDIO_BUF_SIZE * 16 * 6);
				if (ret == 0) {
					memset(play_buf, 0, AUDIO_BUF_SIZE * 16);
					len = AUDIO_BUF_SIZE * 16;
					do {
						audio_write_frame(play_handle, play_buf, frame_size);
						len -= frame_size;
						usleep(20000);
					} while (len > 0);
					usleep(80000);
					break;
				}

				if(ret > 0) {
					len = ret;
					if (len < frame_size) {
						len = frame_size;
					}
					offset = 0;
					do {
						ret = audio_write_frame(play_handle, play_buf + offset, frame_size);
						if (ret < 0) {
							LOGE("audio play codec failed %d \n",  ret);
						}
						offset += frame_size;
					} while (offset < len);
					usleep(50000);
				}
			} while (ret > 0);

			if (play_handle >= 0) {
				audio_put_channel(play_handle);
			}
			if (codec != NULL) {
				codec_close(codec);
				codec = NULL;
			}
		}
	}
}

/*
* record then stop and play the recorded audio file
*/
static int audiotestnormal(int mode)
{
	LOGD("doing audio test %d\n", mode);
    char AUDIO_FILENAME[50] = "/mnt/sd0/pcm_read.pcm";
    pthread_t id_audio;

    if (1  == mode) {
        if(0 == access(AUDIO_FILENAME, 0)) {
            unlink(AUDIO_FILENAME);
        }
        test.audio_cmd = CMD_SESSION_START_AUDIO;
        pthread_create(&id_audio, NULL, (void *) record_audio, (char *)"/mnt/sd0/pcm_read.pcm");
        pthread_join(id_audio, NULL);
    } else if (2 == mode) {
        test.audio_cmd = CMD_SESSION_STOP_AUDIO;
        sleep(1);

		pthread_create(&id_audio, NULL, (void *) play_audio_pcm, (char *)"/mnt/sd0/pcm_read.pcm");
		pthread_join(id_audio, NULL);
    }
	return 0;
}

/*
*record then stop and start bt codec read data then send to speak
*/
static int audiotestPCBA(int mode)
{
	LOGD("\n**************audiotestPCBA***************\n");
    char AUDIO_FILENAME[50] = "/mnt/sd0/testfolder/test_audio.pcm";
    if (1 == mode) {
        if(0 == access(AUDIO_FILENAME, 0)) {
            unlink(AUDIO_FILENAME);
        }
		LOGD("\n**************unlink AUDIO_FILENAME***************\n");
        record_audio(AUDIO_FILENAME);
        test.audio_cmd = CMD_SESSION_START_AUDIO;
		LOGD("\n**************start arecord***************\n");
    } else if (2 == mode) {
        test.audio_cmd = CMD_SESSION_STOP_AUDIO;
		LOGD("\n**************stop arecord***************\n");
		play_audio(AUDIO_FILENAME);

        if(0 == access(AUDIO_FILENAME, 0)) {
            unlink(AUDIO_FILENAME);
        }
    }
    return 0;
}

static int tftest(void)
{
	if(1 == test.tf_card_state) {
		char filename[] = "/mnt/sd0/testfolder/tftest";
    	char syscmd[128];
    	sprintf(syscmd, "touch %s", filename);
    	system(syscmd);

    	FILE *fp;
    	if((fp = fopen(filename,"r")) == NULL) {
        	LOGE("tftest error\n");
        	return -1;
    	}
    	fclose(fp);
    	sprintf(syscmd, "rm %s", filename);
   	 	system(syscmd);
		return 0;
	} else {
		return -1;
	}
}

static int videotest(int action)
{
	if (action) {
        pthread_t id_video;

		LOGD("video test start\n");
        test.video_process_run = 1;
        pthread_create(&id_video, NULL, (void *) thread_sensor_test, NULL);
		pthread_detach(id_video);
    } else {
        test.video_process_run = 0;
		LOGD("video test stop \n");
    }
    return 0;
}

static int speekertest(int mode)
{
	pthread_t id_audio;
    if (1 == mode) {
    	LOGD("\n**************start speekertest***************\n");
    	test.audio_cmd = CMD_SESSION_START_AUDIO;
    	pthread_create(&id_audio, NULL, (void *) play_audio, (char *)"/mnt/sd0/testfolder/248.wav");
    } else if (2 == mode) {
        test.audio_cmd = CMD_SESSION_STOP_AUDIO;
    }

    return 0;
}

static void led_test(void)
{
    while(1) {
        if(test.wifi_sta_connected == 1) {
        	system_set_led(BLUE_LED_NAME, 0, 0, 0);
			system_set_led(RED_LED_NAME, 0, 0, 0);
            system_set_led(GREEN_LED_NAME, 1, 0, 0);
            sleep(1);
            system_set_led(RED_LED_NAME, 0, 0, 0);
            system_set_led(GREEN_LED_NAME, 0, 0, 0);
            system_set_led(BLUE_LED_NAME, 1, 0, 0);
            sleep(1);
            system_set_led(BLUE_LED_NAME, 0, 0, 0);
            system_set_led(GREEN_LED_NAME, 0, 0, 0);
            system_set_led(RED_LED_NAME, 1, 0, 0);
            sleep(1);
        }
    }
}

static int init_test(void)
{
	if(0 == test.ledThread) {
		pthread_create(&test.ledThread, NULL, (void *) led_test, NULL);
	}
	return 0;
}

static int msg_deal(char *smsg, int len, char *addr, int port)
{
	char msg[BUFFER_SIZE];
	int lenmsg = len;

	memset(msg, 0, sizeof(char)*BUFFER_SIZE);
	memcpy(msg, smsg, lenmsg);

    LOGD("factory_test----->Receive %s.\n", msg);

    if(0 == strncasecmp(msg, "request:check alive\r\n", lenmsg)) {
        sprintf(smsg, "respond:ipc alive\r\n");
        return 0;
    }

	if(0 == strncasecmp(msg, "request:mode PCBA\r\n", lenmsg)) {
		LOGD("factory_test----->PCBA_MODE\n");
        test.gTestMode = PCBA_MODE;
		sprintf(smsg, "respond:mode PCBA\r\n");
        return 0;
    } else if(0 == strncasecmp(msg, "request:mode normal\r\n", lenmsg)) {
    	LOGD("factory_test----->NORMAL_MODE\n");
    	test.gTestMode = NORMAL_MODE;
		sprintf(smsg, "respond:mode normal\r\n");
    	return 0;
    }

    if(0 == strncasecmp(msg, "request:init test\r\n", lenmsg)) {
        if(!init_test()) {
            char version[32] = {0};
            system_get_version(version);
            sprintf(smsg, "respond:version %s\r\n", version);
        } else {
            sprintf(smsg, "respond:init failed\r\n");
        }
        return 0;
    } else if(0 == strncasecmp(msg, "request:audio test\r\n", 18)) {
		if(PCBA_MODE != test.gTestMode) {
	        if(!audiotestnormal(1)) {
	            sprintf(smsg, "respond:audio test success\r\n");
	        } else {
	            sprintf(smsg, "respond:audio test failed\r\n");
	        }
		} else {
			if(!audiotestPCBA(1)) {
	            sprintf(smsg, "respond:audio test success\r\n");
	        } else {
	            sprintf(smsg, "respond:audio test failed\r\n");
	        }
		}
		LOGD("factory_test----->request:audio start over\n");
		return 0;
	} else if(0 == strncasecmp(msg, "request:video test\r\n", 18)) {
        if(!videotest(1)) {
            sprintf(smsg, "respond:video test success\r\n");
        }else{
            sprintf(smsg, "respond:video test failed\r\n");
        }
		return 0;
	} else if(0 == strncasecmp(msg, "request:audio stop\r\n", 18)) {
		if(PCBA_MODE != test.gTestMode) {
	        if(!audiotestnormal(2)) {
	            sprintf(smsg, "respond:audio test success\r\n");
	        } else {
	            sprintf(smsg, "respond:audio test failed\r\n");
	        }
		} else {
			if(!audiotestPCBA(2)) {
	            sprintf(smsg, "respond:audio test success\r\n");
	        } else {
	            sprintf(smsg, "respond:audio test failed\r\n");
	        }
		}
		LOGD("factory_test----->request:audio stop over\n");
		return 0;
    } else if(0 == strncasecmp(msg, "request:video stop\r\n", 18)) {
        if(!videotest(0)) {
            sprintf(smsg, "respond:video test success\r\n");
        } else {
            sprintf(smsg, "respond:video test failed\r\n");
        }
		return 0;
	} else if(0 == strncasecmp(msg, "request:bt test\r\n", lenmsg)) {
        if(1 == test.ble_new_connect) {
            sprintf(smsg, "respond:bt test success\r\n");
        } else {
            sprintf(smsg, "respond:bt test failed\r\n");
        }
        return 0;
    } else if(0 == strncasecmp(msg, "request:wifi test\r\n", lenmsg)) {
        if(1 == test.wifi_sta_connected) {
            sprintf(smsg, "respond:wifi test success\r\n");
        } else {
            sprintf(smsg, "respond:wifi test failed\r\n");
        }
        return 0;
    } else if(0 == strncasecmp(msg, "request:tf test\r\n", lenmsg)) {
        if(!tftest()) {
            sprintf(smsg, "respond:tf test success\r\n");
        } else {
            sprintf(smsg, "respond:tf test failed\r\n");
        }
        return 0;
    } else if(0 == strncasecmp(msg, "request:speeker test\r\n", lenmsg)) {
        if(!speekertest(1)) {
            sprintf(smsg, "respond:speeker test success\r\n");
        } else {
            sprintf(smsg, "respond:speeker test failed\r\n");
        }
        return 0;
    } else if(0 == strncasecmp(msg, "request:speeker stop\r\n", lenmsg)) {
        if(!speekertest(2)) {
            sprintf(smsg, "respond:speeker stop success\r\n");
        } else {
            sprintf(smsg, "respond:speeker stop failed\r\n");
        }
        return 0;
    }

    memset(smsg, 0, sizeof(char) * BUFFER_SIZE);
    return -1;
}

static void* process_info(void *args)
{
	LOGD("in process_info \n");
	struct mypara *para1;
	para1 = (struct mypara *)args;
	int s = (*para1).fd;
	int port = (*para1).port;
	char *addr_char = NULL;
	addr_char = (*para1).addr;

	LOGD("factory_test----->%s, fd = %d, addr = %s, port = %d \n", __FUNCTION__, s, addr_char, port);
	int recv_num;
	int send_num;
	int send_length;
	char recv_buf[BUFFER_SIZE];

	LOGD("factory_test----->begin recv: \n");
	memset(recv_buf, 0, sizeof(char)*BUFFER_SIZE);
	recv_num = read(s, recv_buf, sizeof(recv_buf));
	if(recv_num < 0) {
		LOGE("factory_test----->recv fail \n");
		return (NULL);
	} else {
		recv_buf[recv_num] = '\0';
		LOGD("factory_test----->recv sucessful:\n%s \n",recv_buf);
	}

	if(msg_deal(recv_buf, recv_num, addr_char, port)) {
		LOGE("factory_test----->msg_deal erro \n");
	}

    LOGD("factory_test----->begin send \n");

    LOGD("factory_test----->recv_buf = %s strlen = %d\n", recv_buf, strlen(recv_buf));
    send_length = strlen(recv_buf);
    send_num = write(s, recv_buf, send_length);
    LOGD("factory_test----->send_num = %d \n", send_num);
    if (send_num < 0) {
        LOGE("factory_test----->send \n");
        return(NULL);
    } else {
        LOGD("factory_test----->msg sendback sucess \n");
    }
    close(s);

	return(NULL);
}

static void test_server(void)
{
	char syscmd[60];
    sprintf(syscmd, "/mnt/sd0/testfolder/wifi/iperf -s &");
    system(syscmd);
	LOGD("Welcome! This is a IPC_SERVER, I can only received message from client and reply with the dealed message \n");

	while(1 != test.wifi_sta_connected) {
		usleep(200000);
	}
	int servfd, clifd, on;
	struct sockaddr_in servaddr, cliaddr;
	if ((servfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		LOGE("factory_test----->create socket error! \n");
		return;
	}

	on = 1;
	setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(servfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		LOGE("factory_test----->bind to port %d failure! \n", SERVER_PORT);
		return;
	}

	if (listen(servfd, LENGTH_OF_LISTEN_QUEUE) < 0) {
		LOGE("factory_test----->call listen failure! \n");
		return;
	}

	pthread_t t_processinfo;
	LOGD("factory_test----->now start to listen:===========================================\n");
	while (1) {
		char addr_char[20];
		int port;
		struct mypara para;
		socklen_t length = sizeof(cliaddr);
		clifd = accept(servfd, (struct sockaddr*)&cliaddr, &length);
        LOGD("factory_test----->receive data\n");
		if (clifd < 0) {
			LOGE("factory_test----->error comes when call accept! \n");
			usleep(100*1000);
			break;
		}
		sprintf(addr_char, "%s", inet_ntoa(cliaddr.sin_addr));
		port = ntohs(cliaddr.sin_port);
        sprintf(ipaddress, addr_char);

		para.fd = clifd;
		para.addr = addr_char;
		para.port = port;
		LOGD("factory_test----->fd = %d, addr = %s, port = %d \n", para.fd, para.addr, para.port);
		pthread_create(&t_processinfo, NULL, process_info, &(para));
		pthread_join(t_processinfo, NULL);
		LOGD("factory_test----->%s pthread_join process_info \n", __FUNCTION__);

		if(1 == gClose) {
			break;
		}
	}
	close(servfd);

	return;
}

static void test_client(void)
{
    LOGD("factory_test----->init my client and send broadcast \n");
    int loop = 0;
    int nlen = 0;
    char msg[BUFFER_SIZE] = {0};

    nlen = strlen(test.name);
    if (test.name[nlen - 1] == '\n' && test.name[nlen - 2] == '\r') {
        test.name[nlen - 2] = ':';
        test.name[nlen - 1] = '\0';
    }
    if (test.name[nlen - 1] == '\n') {
        test.name[nlen - 1] = ':';
        test.name[nlen] = '\0';
    }

    LOGD("factory_test:%d----->%s %s %s \n", __LINE__, test.name, test.ipaddr, test.hwaddr);

    while (1) {
    	if(1 == test.wifi_sta_connected) {
            int brdcFd;
            memset(msg, 0, BUFFER_SIZE);

            if((brdcFd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
                LOGE("factory_test----->socket fail\n");
                return;
            }
            int optval = 1;
            setsockopt(brdcFd, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &optval, sizeof(int));  
            struct sockaddr_in theirAddr;
            memset(&theirAddr, 0, sizeof(struct sockaddr_in));
            theirAddr.sin_family = AF_INET;
            theirAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
            theirAddr.sin_port = htons(18000);
            int sendBytes;
            if(1 == test.reset_key) { //send reset key press to pc
            	sprintf(msg, "reset key down");
            	test.reset_key = 0;
            	LOGD("factory_test----->msg=%s, msgLen=%d\n", msg, strlen(msg));
            } else {  //send wifi info to pc
            	sprintf(msg, "%s\r%s\r%s", test.name, test.ipaddr, test.hwaddr);
            }

            if((sendBytes = sendto(brdcFd, msg, strlen(msg), 0,
                   (struct sockaddr *)&theirAddr, sizeof(struct sockaddr))) == -1) {
                LOGE("factory_test----->sendto fail, errno=%d \n", errno);
                return;
            }

            if(0 == (loop % 5)) {
                LOGD("factory_test----->msg=%s, msgLen=%d, sendBytes=%d \n", msg, strlen(msg), sendBytes);
           	}
            close(brdcFd);
            sleep(1);
            loop++;
    	} else {
    		usleep(200000);
    	}
    }
    return;
}

static void test_event_handle(char *event, void *arg)
{
	struct event_key *reset_key;
	struct event_lightsence *event_brightness;
	int brightness;
	int check_bri = 0;

	if (!strncmp(event, EVENT_KEY, strlen(EVENT_KEY))) {
		reset_key = (struct event_key *)arg;
		LOGD("factory_test----->qiwo reset key handle %s %d %d \n", (char *)event, reset_key->key_code, reset_key->down);
		if ((reset_key->key_code == KEY_RESTART) && (reset_key->down == 1)) {
			test.reset_key = 1;
		}
	} else if (!strncmp(event, EVENT_LIGHTSENCE_B, strlen(EVENT_LIGHTSENCE_B))) {
		event_brightness = (struct event_lightsence *)arg;
		brightness = event_brightness->trigger_current;
		if (brightness <= LIGHT_SENSOR_DARK) {
			system_set_pin_value(D304_IRCUTAGPIO, 1);
			system_set_pin_value(D304_IRCUTBGPIO, 1);
			system_set_pin_value(D304_IRLEDGPIO, 1);
			system_set_pin_value(D304_IRPWMGPIO, 0);
		} else {
			system_set_pin_value(D304_IRLEDGPIO, 0);
			system_set_pin_value(D304_IRPWMGPIO, 0);
			system_set_pin_value(D304_IRCUTAGPIO, 0);
			system_set_pin_value(D304_IRCUTBGPIO, 0);
		}
		LOGD("factory_test----->qiwo light event %d check_bri=%d \n", camera_get_brightness(CAMERA_SENSOR_NAME, &check_bri), check_bri);
	} else if (!strncmp(event, EVENT_LIGHTSENCE_D, strlen(EVENT_LIGHTSENCE_D))) {

		event_brightness = (struct event_lightsence *)arg;
		brightness = event_brightness->trigger_current;
		if (brightness <= LIGHT_SENSOR_DARK) {
			system_set_pin_value(D304_IRCUTAGPIO, 1);
			system_set_pin_value(D304_IRCUTBGPIO, 1);
			system_set_pin_value(D304_IRLEDGPIO, 1);
			system_set_pin_value(D304_IRPWMGPIO, 0);
		} else {
			system_set_pin_value(D304_IRLEDGPIO, 0);
			system_set_pin_value(D304_IRPWMGPIO, 0);
			system_set_pin_value(D304_IRCUTAGPIO, 0);
			system_set_pin_value(D304_IRCUTBGPIO, 0);
		}

		LOGD("factory_test----->qiwo dark event %d \n", brightness);
	} else if (!strncmp(event, EVENT_MOUNT, strlen(EVENT_MOUNT))) {
		if (test.tf_card_state) {
			return;
		}
		test.tf_card_state = 1;
		sprintf(test.sd_mount_point, (char *)arg);
	} else if (!strncmp(event, EVENT_UNMOUNT, strlen(EVENT_UNMOUNT))) {
		test.tf_card_state = 0;
	}
	return;
}

static void test_ble_handle(char *event, void *arg)
{
	char *temp_addr;

	if (!strncmp(event, "qiwo_ble_data", strlen("qiwo_ble_data"))) {
		if (!memcmp((char *)arg, "reg_data/", strlen("reg_data/"))) {
			test.ble_new_connect = 1;
		} else if (!memcmp((char *)arg, "bt_addr/", strlen("bt_addr/"))) {
			temp_addr = arg + strlen("bt_addr/");
			sprintf(test.bt_addr, temp_addr);
			test.ble_new_connect = 1;
			LOGD("factory_test----->qiwo bt addr %s \n", test.bt_addr);
		} else if (!memcmp((char *)arg, "scan_device/", strlen("scan_device/"))) {
			temp_addr = arg + strlen("scan_device/");
			test.bt_scan_len = temp_addr[0];
			memcpy(test.bt_scan_result, temp_addr + 1, test.bt_scan_len);
			LOGD("factory_test----->qiwo scan result %d \n", test.bt_scan_len);
		} else if (!memcmp((char *)arg, "pair_state/", strlen("pair_state/"))) {
			temp_addr = arg + strlen("pair_state/");
			test.bt_scan_len = temp_addr[0];
			memcpy(test.bt_scan_result, temp_addr + 1, test.bt_scan_len);
			if (test.bt_scan_result[3]) {
				sprintf(test.play_audio_channel, BT_CODEC_PLAY);
				sprintf(test.rec_audio_channel, BT_CODEC_RECORD);
				test.capture_fmt.channels = BT_CODEC_CHANNELS;
				test.capture_fmt.samplingrate = BT_CODEC_SAMPLERATE;
				test.capture_fmt.bitwidth = BT_CODEC_BITWIDTH;
				test.capture_fmt.sample_size = 400;
				test.play_fmt.channels = BT_CODEC_CHANNELS;
				test.play_fmt.samplingrate = BT_CODEC_SAMPLERATE;
				test.play_fmt.bitwidth = BT_CODEC_BITWIDTH;
				test.play_fmt.sample_size = 400;
			} else {
				sprintf(test.play_audio_channel, NORMAL_CODEC_PLAY);
				sprintf(test.rec_audio_channel, NORMAL_CODEC_RECORD);
				test.capture_fmt.channels = NORMAL_CODEC_CHANNELS;
				test.capture_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
				test.capture_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
				test.capture_fmt.sample_size = 1000;
				test.play_fmt.channels = NORMAL_CODEC_CHANNELS;
				test.play_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
				test.play_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
				test.play_fmt.sample_size = 1024;
			}
			LOGD("factory_test----->qiwo pair state %d %d \n", test.bt_scan_len, test.bt_scan_result[3]);
		} else if (!memcmp((char *)arg, "inquiry_result/", strlen("inquiry_result/"))) {
			temp_addr = arg + strlen("inquiry_result/");
			test.bt_scan_len = temp_addr[0];
			memcpy(test.bt_scan_result, temp_addr + 1, test.bt_scan_len);
			LOGD("factory_test----->qiwo inquiry result %d %d \n", test.bt_scan_len, test.bt_scan_result[3]);
		}
	}
	return;
}

static void test_wifi_state_handle(char *event, void *arg)
{
	struct net_info_t net_info;

	if (!strncmp(event, WIFI_STATE_EVENT, strlen(WIFI_STATE_EVENT))) {

		if (!strncmp((char *)arg, STATE_AP_ENABLED, strlen(STATE_AP_ENABLED))) {
		    wifi_get_net_info(&net_info);
		    if (item_exist("bt.model")) {
				LOGD("factory_test----->bt model exist \n");
				if ((access("/usr/bin/bsa_server", F_OK) == 0) && (access("/usr/bin/d304bt", F_OK) == 0)) {
					LOGD("factory_test----->bsa_server and d304bt exist \n");
					event_register_handler("qiwo_ble_data", 0, test_ble_handle);
					if (system_check_process("bsa_server") < 0) {
						LOGD("factory_test----->start bsa_server and d304bt \n");
						system("bsa_server -d /dev/ttyAMA0 -p /bt/bcm43438a0.hcd -u /tmp/ -k /config/bt/ble_local_keys -all=0 -r 10 &");
						system("d304bt &");
					}
				}
			}
		} else if (!strncmp((char *)arg, STATE_STA_CONNECTED, strlen(STATE_STA_CONNECTED))) {
			if (!test.wifi_sta_connected) {
				test.wifi_sta_connected = 1;
				wifi_get_net_info(&net_info);
				sprintf(test.hwaddr, net_info.hwaddr);
		    	sprintf(test.ipaddr, net_info.ipaddr);
		    	system_set_led(BLUE_LED_NAME, 0, 0, 0);
		    	system_set_led(RED_LED_NAME, 0, 0, 0);
				system_set_led(GREEN_LED_NAME, 1, 0, 0);
				if(0 == test.socketThread) {
					pthread_create(&test.socketThread, NULL, (void *) test_client, NULL);
				}
			}
		    LOGD("factory_test----->wifi_state changed %s \n", (char *)arg);
		} else if (!strncmp((char *)arg, STATE_STA_DISCONNECTED, strlen(STATE_STA_DISCONNECTED))) {
			system_set_led(BLUE_LED_NAME, 0, 0, 0);
			system_set_led(GREEN_LED_NAME, 0, 0, 0);
			system_set_led(RED_LED_NAME, 1, 0, 0);
			test.wifi_sta_connected = 0;
			LOGD("factory_test----->DISCONNECTED \n");
		}
	}

	return;
}

static void factory_signal_handler(int sig)
{
	char name[32];

	if ((sig == SIGQUIT) || (sig == SIGTERM)) {

	} else if (sig == SIGSEGV) {
		prctl(PR_GET_NAME, name);
		LOGD("qiwo Segmentation Fault %s \n", name);
		exit(1);
	}

	return;
}

static void factory_init_signal(void)
{
	struct sigaction action;

    action.sa_handler = factory_signal_handler;
    sigemptyset(&action.sa_mask);
    sigfillset(&action.sa_mask);
    action.sa_flags = SA_RESTART;

	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);
	sigaction(SIGTSTP, &action, NULL);
	sigaction(SIGSEGV, &action, NULL);

	return;
}

static void wifi_init()
{
	struct wifi_config_t wifi_config;
	sprintf(wifi_config.ssid, test.ssid);
	sprintf(wifi_config.psk, test.pswd);
	wifi_sta_connect(&wifi_config);
}

static void init(struct test_common_t *test)
{
	struct font_attr tutk_attr;
	factory_init_signal();
	storage_info_t si;
	test->wifi_ap_set = 0;
	test->wifi_sta_set = 1;
	test->video_process_run = 0;

	system_request_pin(D304_IRLEDGPIO);
	system_request_pin(D304_IRCUTAGPIO);
	system_request_pin(D304_IRCUTBGPIO);
	system_request_pin(D304_IRPWMGPIO);

	test->capture_fmt.channels = NORMAL_CODEC_CHANNELS;
	test->capture_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
	test->capture_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
	test->capture_fmt.sample_size = 1000;
	test->play_fmt.channels = NORMAL_CODEC_CHANNELS;
	test->play_fmt.samplingrate = NORMAL_CODEC_SAMPLERATE;
	test->play_fmt.bitwidth = NORMAL_CODEC_BITWIDTH;
	test->play_fmt.sample_size = 1024;
	test->scene_mode = DAYTIME_STATIC_SCENE;
	test->reset_key = 0;
	test->video_cmd = CMD_SESSION_NONE;
	test->ble_new_connect = 0;
	test->wifi_sta_connected = 0;
	test->tf_card_state = 0;
	test->gTestMode = NORMAL_MODE;
	sprintf(test->sd_mount_point, "/mnt/sd0/");

	video_disable_channel(DEFAULT_HIGH_CHANNEL);
	video_disable_channel(DEFAULT_LOW_CHANNEL);
	video_change_scene(DEFAULT_RECORD_CHANNEL, test->scene_mode);

	event_register_handler(EVENT_MOUNT, 0, test_event_handle);
	event_register_handler(EVENT_UNMOUNT, 0, test_event_handle);
	event_register_handler(EVENT_KEY, 0, test_event_handle);
	event_register_handler(EVENT_LIGHTSENCE_B, 0, test_event_handle);
	event_register_handler(EVENT_LIGHTSENCE_D, 0, test_event_handle);
	event_register_handler(WIFI_STATE_EVENT, 0, test_wifi_state_handle);
	play_audio(NULL);

	memset(&tutk_attr, 0, sizeof(struct font_attr));
	sprintf((char *)tutk_attr.ttf_name, "arial");
	tutk_attr.font_color = 0x00ffffff;
	tutk_attr.back_color = 0x40cccccc;
	marker_set_mode("marker0", "auto", "%t%Y/%M/%D  %H:%m:%S", &tutk_attr);

	if(access(test->sd_mount_point, F_OK) == 0) {
		system_get_storage_info(test->sd_mount_point, &si);
		if (si.total > 0) {
			if (item_exist("board.disk") && item_equal("board.disk", "emmc", 0)) {
				if (access("/dev/mmcblk1", F_OK) == 0) {
					test->tf_card_state = 1;
				}
			} else {
				if (access("/dev/mmcblk0", F_OK) == 0) {
					test->tf_card_state = 1;
				}
			}
		}
	}

	if (test->wifi_ap_set) {
		wifi_start_service(test_wifi_state_handle);
		wifi_ap_enable();
	} else if (test->wifi_sta_set) {
		wifi_start_service(test_wifi_state_handle);
		wifi_sta_enable();
	}
}

static void parse_config(struct test_common_t *test)
{
	char filename[] = "/mnt/sd0/testfolder/config";
    FILE *fp;
    if(NULL == (fp = fopen(filename,"r"))) {
        LOGE("factory_test----->error!");
    }
    if (!feof(fp)) {
        fgets(test->name, 64, fp);
    }
    if (!feof(fp)) {
        fgets(test->ssid, 64, fp);
    }
    if (!feof(fp)) {
        fgets(test->pswd, 64, fp);
    }
    fclose(fp);
    int len = strlen(test->ssid);
    if (('\n' == test->ssid[len - 1]) && ('\r' == test->ssid[len - 2])) {
        test->ssid[len - 2] = '\0';
    }
    if ('\n' == test->ssid[len - 1] ) {
        test->ssid[len - 1] = '\0';
    }
    LOGD("factory_test----->ssid = %s\n", test->ssid);

    len = strlen(test->pswd);
    if (('\n' == test->pswd[len - 1]) && ('\r' == test->pswd[len - 2])) {
        test->pswd[len - 2] = '\0';
    }
    if ('\n' == test->pswd[len - 1]) {
        test->pswd[len - 1] = '\0';
    }
    LOGD("factory_test----->pswd = %s\n", test->pswd);
    len = strlen(test->name);
    if (('\n' == test->name[len - 1]) && ('\r' == test->name[len - 2])) {
        test->name[len - 2] = '\0';
    }
    if ('\n' == test->name[len - 1]) {
        test->name[len - 1] = '\0';
    }

    LOGD("factory_test----->name = %s\n", test->name);
    return;
}

int main(int argc, char** argv)
{
    LOGD("factory_test----->now start\n");

    memset(&test, 0, sizeof(struct test_common_t));
    system_set_led(GREEN_LED_NAME, 0, 0, 0);
    system_set_led(RED_LED_NAME, 0, 0, 0);
    system_set_led(BLUE_LED_NAME, 1, 0, 0);

    init(&test);

    if(0 == test.tf_card_state) {
   	 	LOGE("Please plug in SD card  \n");
   	}

   	do {
   		usleep(5000000);
   	} while(0 == test.tf_card_state);

   	parse_config(&test);

   	wifi_init();

    test_server();

	while(1) {
		sleep(10);
	}
    return 0;
}
