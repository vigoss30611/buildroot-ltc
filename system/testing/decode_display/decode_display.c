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
#include <semaphore.h>

#include <sys/reboot.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/prctl.h>

#include <qsdk/items.h>
#include <qsdk/sys.h>
#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>
#include <qsdk/videobox.h>
#include <qsdk/demux.h>

#include <sys/ioctl.h>
#include <linux/fb.h>

typedef struct {
	uint32_t direction;
	uint32_t colval;
} imapfb_color_key_info_t;

#define IMAPFB_COLOR_KEY_SET_INFO			_IOW ('F', 304, imapfb_color_key_info_t)

static sem_t thread_sem;
static int play_stop = 0;
struct decode_para_t {
	char file[128];
	char channel[32];
};

typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef long LONG;

typedef struct {
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(push)
#pragma pack(1)

typedef struct {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

#pragma pack(pop)
//#define HDMI_DISPLAY 1
static int test_display(void)
{
	void *fbp = NULL;
	int screensize, i;
	char *data_buf;
#ifndef HDMI_DISPLAY	
	return 0;
#endif	
	int fd_osd1 = open("/dev/fb1", O_RDWR);
	struct fb_var_screeninfo vinfo;
	#ifdef HDMI_DISPLAY
	imapfb_color_key_info_t colorkey_info;
	colorkey_info.direction = 0;
	colorkey_info.colval = 0x1f;

	ioctl(fd_osd1, IMAPFB_COLOR_KEY_SET_INFO, &colorkey_info);
	#endif
	ioctl(fd_osd1, FBIOGET_VSCREENINFO, &vinfo);
	printf("osd1 info: bpp%d, std%d, x%d, y%d, vx%d, vy%d\n",
	 vinfo.bits_per_pixel, vinfo.nonstd, vinfo.xres, vinfo.yres,
	 vinfo.xres_virtual, vinfo.yres_virtual);

	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    fbp = mmap(0, screensize, PROT_READ|PROT_WRITE, MAP_SHARED, fd_osd1, 0);
    if (fbp == MAP_FAILED) {
		printf("Error: failed to map framebuffer device to memory.\n");
		return -1;
    }

    vinfo.yoffset = 0;
    ioctl(fd_osd1, FBIOPAN_DISPLAY, &vinfo);

	if (fbp != NULL) {
		data_buf = (char *)fbp;
	#ifdef HDMI_DISPLAY
	      //16bpp
		for (i=0; i<screensize/2; i++)
			*(uint16_t *)(data_buf + i * 2) = 0x1f;
	#else
		for (i=0; i<screensize/2; i++) {
                       if (i >= screensize / 4)
                               *(uint16_t *)(data_buf + i * 2) = 0xfc1f;
                       else
                               *(uint16_t *)(data_buf + i * 2) = 0xfc1f;
               }
	#endif							   
	}
	return 0;

	BITMAPFILEHEADER filehdr;
	BITMAPINFOHEADER bmphdr;
	char *pdata = NULL;

	FILE *pf = fopen("/mnt/sd0/background.bmp", "rb");
	if (pf) {
		if (fread(&filehdr, sizeof(BITMAPFILEHEADER), 1, pf) == 1) {
			if (fread(&bmphdr, sizeof(BITMAPINFOHEADER), 1, pf) == 1) {
				bmphdr.biSizeImage = filehdr.bfSize - 54;
				printf("data offset %u bpp %u size %u width %ld height %ld\n", filehdr.bfOffBits, bmphdr.biBitCount, bmphdr.biSizeImage, bmphdr.biWidth, bmphdr.biHeight);
				if (bmphdr.biBitCount == 16) {
					pdata = (char*)malloc(bmphdr.biSizeImage);
					if (pdata) {
						fseek(pf, filehdr.bfOffBits, SEEK_SET);
						fread(pdata, 1, bmphdr.biSizeImage, pf);
					} else {
						printf("malloc failed ++++++++++\n");
					}
				} else {
					printf("bmp format not match\n");
				}
			}
		}
		fclose(pf);
	} else {
		printf("%s open failed\n", "/mnt/sd0/background.bmp");
	}
	if (pdata) {
#if 1
		data_buf = (char*)malloc(bmphdr.biSizeImage);
		if (!data_buf) {
			printf("malloc failed ++++++++++\n");
			return -1;
		}
	    int count = 0;
	    char *psrc = pdata ;  
	    char *pdst = data_buf;  
	    char *p = psrc;  
	    //int value = 0x00;  
	    pdst += (bmphdr.biWidth * bmphdr.biHeight * (bmphdr.biBitCount / 8));  
	    for(i=0;i<bmphdr.biHeight;i++){  
	        p = psrc + i * bmphdr.biWidth * (bmphdr.biBitCount / 8);  
	        pdst -= bmphdr.biWidth * (bmphdr.biBitCount / 8);
	        memcpy(pdst, p, bmphdr.biWidth * (bmphdr.biBitCount / 8));
#if 0
	        for(j=0;j<bmphdr.biWidth;j++){  
	            pdst -= 4;  
	            p -= 3;  
	            pdst[0] = p[0];  
	            pdst[1] = p[1];  
	            pdst[2] = p[2];  
	            pdst[3] = 0x00;  

	            value = *((int*)pdst);  
	            value = pdst[0];  
	            if(value == 0x00){  
	                pdst[3] = 0x00;  
	            }else{  
	                pdst[3] = 0x00;  
	            }  
	        }  
#endif
	    }
#endif
		pf = fopen("/mnt/sd0/ffbb", "rb");
		if (pf) {
			fseek(pf, 0, SEEK_SET);
			fread(data_buf, 1, bmphdr.biSizeImage, pf);
			fclose(pf);
		}
		printf("pdata %x %x %x %x \n", pdata[0], pdata[1], pdata[2], pdata[3]);
		while (1) {
			if (fbp != NULL) {
				for (i=0; i<bmphdr.biSizeImage/2; i++) {
					if (i >= bmphdr.biSizeImage / 4)
						*(uint16_t *)(data_buf + i * 2) = 0xfc1f;
					else
						*(uint16_t *)(data_buf + i * 2) = 0xfc1f;
				}
				//memset((char *)fbp, 0x00, screensize);
	            memcpy(fbp, data_buf, bmphdr.biSizeImage);
			}
			sleep(1);
			count++;
			//if (fbp != NULL)
				//memset((char *)fbp, 0, screensize);
		}
	}

    return 0;
}

static void *play_thread(void *arg)
{
	struct decode_para_t *decode_para = (struct decode_para_t *)arg;
	audio_fmt_t play_fmt;
	struct demux_frame_t demux_frame = {0};
	struct demux_t *demux = NULL;
	struct codec_info play_info;
	struct codec_addr play_addr;
	int play_handle = -1, ret, len, frame_size, head_len = 0, head_set = 0, offset;
	char play_buf[1024 * 16 * 6], header[128];
	void *codec = NULL;
	int length;

	prctl(PR_SET_NAME, "play_display");
	play_fmt.channels = 2;
	#ifdef HDMI_DISPLAY	
	play_fmt.samplingrate = 48000;//hdmi audio
	#else
	play_fmt.samplingrate = 16000;
	#endif	
	play_fmt.bitwidth = 32;
	play_fmt.sample_size = 1024;
	frame_size = play_fmt.sample_size;

	demux = demux_init(decode_para->file);
	if (demux) {

		head_len = demux_get_head(demux, header, 128);
		do {
			if (play_stop)
				break;
			ret = demux_get_frame(demux, &demux_frame);
			if (((ret < 0) || (demux_frame.data_size == 0)) && (play_handle >= 0))
				break;

			if ((demux_frame.codec_id == DEMUX_VIDEO_H265) || (demux_frame.codec_id == DEMUX_VIDEO_H264)) {
				if (!head_set) {
					ret = video_set_header(decode_para->channel, header, head_len);
					if (ret < 0)
						printf("video set header failed %d %d \n",  ret, head_len);
					head_set = 1;
				}
				video_write_frame(decode_para->channel, demux_frame.data, demux_frame.data_size);
				demux_put_frame(demux, &demux_frame);
				continue;
			}

			if (play_handle < 0) {
				frame_size = play_fmt.sample_size;
				if (demux_frame.bitwidth == 16)
					frame_size = play_fmt.sample_size * 2;
				if (play_fmt.channels > demux_frame.channels)
					frame_size *= (play_fmt.channels / demux_frame.channels);
				if (play_fmt.samplingrate > demux_frame.sample_rate)
					frame_size *= (play_fmt.samplingrate / demux_frame.sample_rate);

				printf("play audio fmt %s %d %d %d \n", decode_para->file, play_fmt.channels, play_fmt.samplingrate, play_fmt.bitwidth);
				play_handle = audio_get_channel("default", &play_fmt, CHANNEL_BACKGROUND);
				if (play_handle < 0) {
					printf("audio play get chanel failed %d \n", play_handle);
					break;
				}
			}

			if (!codec) {
				if (demux_frame.codec_id == DEMUX_AUDIO_PCM_ALAW)
					play_info.in.codec_type = AUDIO_CODEC_G711A;
				else
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
					printf("audio play open codec decoder failed \n");
					break;
				}
			}
			memset(play_buf, 0, 1024 * 16 * 6);
			play_addr.in = demux_frame.data;
			play_addr.len_in = demux_frame.data_size;
			play_addr.out = play_buf;
			play_addr.len_out = 1024 * 16 * 6;
			ret = codec_decode(codec, &play_addr);
			demux_put_frame(demux, &demux_frame);
			if (ret < 0) {
				printf("audio play codec decoder failed %d \n",  ret);
				break;
			}
			len = ret;
			if (len < frame_size)
				len = frame_size;
			offset = 0;
			do {
				ret = audio_write_frame(play_handle, play_buf + offset, frame_size);
				if (ret < 0)
					printf("audio play codec failed %d \n",  ret);
				offset += frame_size;
			} while (offset < len);

			usleep(30000);
		} while (ret >= 0);

		if (codec != NULL) {
			do {
				play_addr.out = play_buf;
				play_addr.len_out = 1024 * 16 * 6;
				ret = codec_flush(codec, &play_addr, &length);
				if (ret == CODEC_FLUSH_ERR)
					break;

				if (length > 0) {
					ret = audio_write_frame(play_handle, play_buf, length);
					if (ret < 0)
						printf("audio play codec failed %d \n",  ret);
				}
			} while (ret == CODEC_FLUSH_AGAIN);
			codec_close(codec);
			codec = NULL;
		}

		if (play_handle >= 0) {
			usleep(150000);
			audio_put_channel(play_handle);
		}
		demux_deinit(demux);
	}

	sem_post(&thread_sem);
	pthread_exit(0);
}

static void display_signal_handler(int sig)
{
	char name[32];

	if ((sig == SIGQUIT) || (sig == SIGTERM)) {
		play_stop = 1;
		exit(1);
	} else if (sig == SIGSEGV) {
		prctl(PR_GET_NAME, name);
		printf("display Segmentation Fault %s \n", name);
		exit(1);
	}

	return;
}

static void init_signal(void)
{
	struct sigaction action;

    action.sa_handler = display_signal_handler;
    sigemptyset(&action.sa_mask);
    sigfillset(&action.sa_mask);
    action.sa_flags = SA_RESTART;

	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);
	sigaction(SIGTSTP, &action, NULL);
	sigaction(SIGSEGV, &action, NULL);

	return;
}

int main(int argc, char** argv)
{
	int ret;
	pthread_t thread;
	struct decode_para_t *decode_para;

	if (!argv[1]) {
		printf("need file path\n");
		printf("format is decode_display file_path  decode_channel \n");
		return 0;
	}

	decode_para = malloc(sizeof(struct decode_para_t));
	if (!decode_para)
		return 0;

	snprintf(decode_para->file, 128, argv[1]);
	if (!argv[2]) {
		printf("not set decoder channel we use default dec0-stream \n");
		sprintf(decode_para->channel, "dec0-stream");
	} else {
		snprintf(decode_para->channel, 32, argv[2]);
	}
	init_signal();
	sem_init(&thread_sem, 0, 0);
	ret = pthread_create(&thread, NULL, &play_thread, (void *)decode_para);
	if (ret)
		printf("play_thread creat failed %d \n", ret);
	else
		pthread_detach(thread);

	test_display();
	sem_wait(&thread_sem);
	free(decode_para);

	return 0;
}

