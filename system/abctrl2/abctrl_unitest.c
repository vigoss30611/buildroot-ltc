#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <fr/libfr.h>
#include <qsdk/audiobox2.h>
#include <qsdk/event.h>
#include <sys/time.h>
#include "abctrl_unitest.h"
#include "abctrl.h"
#include <sys/types.h>
#include <sys/stat.h> 
#include <fr/libfr.h>

int	dev_samplerate[] = {
	//8000, 16000, 32000, 48000, -1
	16000, 48000, -1, -1
};
#if 0
int	chan_samplerate[] = {
	44100, 22050, 11025, -1,
};
#else
int	chan_samplerate[] = {
	//8000, 16000, 32000, 48000, -1,
	16000, -1, 32000, -1,
};
#endif
int	chan_tracks[] = {
	2, -1
};
int	chan_bitwidth[] = {
	16, 32, -1, 32, -1
};
int	chan_codectype[] = {
	AUDIO_CODEC_PCM, -1, AUDIO_CODEC_G711A, -1, AUDIO_CODEC_AAC, -1, AUDIO_CODEC_PCM, AUDIO_CODEC_G711U, AUDIO_CODEC_G711A, AUDIO_CODEC_G726, -1
};

/* static int	chan_codectype[] = {
	AUDIO_CODEC_PCM, AUDIO_CODEC_G711U, AUDIO_CODEC_G711A, 
	AUDIO_CODEC_ADPCM, AUDIO_CODEC_G726, AUDIO_CODEC_MP3, 
	AUDIO_CODEC_SPEEX, AUDIO_CODEC_AAC, -1
}; */

static pthread_t	  thread_id[32];
static thread_info_t  thread_info[32];

static unitest_info_t unitest_info = {	
	.pcm_name_2 = "default",
	.preset_attr = {
		.channels = -1,
		.bitwidth = -1,
		.samplingrate = -1,
		.sample_size = -1,
	},
	.dev_attr = {
		.channels = 2,
		.bitwidth = 32,
		.samplingrate = 48000,
		.sample_size = 1024,
	},
	.preset_fmt = {
		.codec_type = -1,
		.channels = -1,
		.bitwidth = -1,
		.samplingrate = -1,
		.sample_size = -1,
	},
	.fmt = {
		.codec_type = AUDIO_CODEC_PCM,
		.channels = 2,
		.bitwidth = 32,
		.samplingrate = 48000,
		.sample_size = 1024,
	},
	.time = 10,
	.volume = 100,
};

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

static int get_frame_size(audio_fmt_t *fmt)
{
	int bitwidth;

	if (!fmt)
		return -1;
	
	//bitwidth = ((fmt->bitwidth > 16) ? 32 : fmt->bitwidth);
	bitwidth = fmt->bitwidth;
	return (fmt->channels * (bitwidth >> 3));
}

int	get_aac_frame_len(unsigned char* buf)
{
	if((buf[0] == 0xff) && ((buf[1] & 0xf0) == 0xf0))
		return (((buf[3] & 0x3) << 11) | (buf[4] << 3) | (buf[5] >> 5));
	else
		return 0;
}

static int audio_get_dev_sample_size(const char *dev)
{
	audio_fmt_t	dev_attr;
	
	audio_get_format(dev, &dev_attr);
	
	return dev_attr.sample_size;
}

int unitest_capture(char *pcm_name, int handle, audio_fmt_t	*dev_attr, audio_chn_fmt_t *fmt, int time, int case_id)
{
	struct wav_header header;
	int ch, ret = 0;
	int len, fd;
	char wavfile[128];
	char *audiobuf = NULL;
	long total_len, needed;
	int	dev_vol = 100, chan_vol = 256, chan_mute = 0;
	int length, offset;
	int module;
	char	sample_dir[32];

	if(case_id == MULTIMIX_CASE){
		mkdir("/mnt/mix", 777);
		snprintf(sample_dir, sizeof(sample_dir), "/mnt/mix/%05d", dev_attr->samplingrate);
		mkdir(sample_dir, 777);
	}else{
		snprintf(sample_dir, sizeof(sample_dir), "/mnt/%05d", dev_attr->samplingrate);
		mkdir(sample_dir, 777);
	}

	if(fmt->codec_type == AUDIO_CODEC_AAC)
		snprintf(wavfile, sizeof(wavfile), "%s/rec_%05d_%05d_%d_%02d_%d.aac"
			,sample_dir
			,dev_attr->samplingrate, fmt->samplingrate, fmt->channels
			,fmt->bitwidth, fmt->codec_type);
	else
		snprintf(wavfile, sizeof(wavfile), "%s/rec_%05d_%05d_%d_%02d_%d.wav"
		,sample_dir
		,dev_attr->samplingrate, fmt->samplingrate, fmt->channels
		,fmt->bitwidth, fmt->codec_type);

	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~Recording %s, time:%d!\n", wavfile, time);
	fmt->sample_size = SAMPLE_SIZE(fmt->samplingrate);

	#if 0
	audio_set_format(pcm_name, dev_attr);
	handle = audio_get_channel_codec(pcm_name, fmt, CHANNEL_BACKGROUND);
	if (handle < 0) {
		ABCTL_ERR("Get channel err: %d\n", handle);
		return -1;
	}
	#endif
	fd = open(wavfile, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	if (fd < 0) {
		ABCTL_ERR("Please output right wav file path: %d\n", fd);
		audio_put_channel(handle);
		return -1;
	}
	
	length = get_frame_size(fmt) * fmt->sample_size / audio_get_codec_ratio(fmt);
	audiobuf = (char*)malloc(length);
	if(NULL == audiobuf) {
		ABCTL_ERR("audiobuf malloc failed!!\n");
		close(fd);
		audio_put_channel(handle);
		return -1;
	}

	needed = fmt->samplingrate * time * get_frame_size(fmt);
	needed = needed - (needed%length);

	if(fmt->codec_type != AUDIO_CODEC_AAC){
		header.riff_id = ID_RIFF;
		header.riff_sz = 0;
		header.riff_fmt = ID_WAVE;
		header.fmt_id = ID_FMT;
		header.fmt_sz = 16;
		header.audio_format = FORMAT_PCM;
		header.num_channels = fmt->channels;
		header.sample_rate = fmt->samplingrate;
		header.bits_per_sample = fmt->bitwidth;//(real.bitwidth == 16) ? 16 : 32;
		header.byte_rate = get_frame_size(fmt) * fmt->samplingrate;
		header.block_align = fmt->channels * (header.bits_per_sample >> 3);
		header.data_id = ID_DATA;
		header.data_sz = needed;
		header.riff_sz = header.data_sz + sizeof(header) - 8;
		
		ret = write(fd, &header, sizeof(struct wav_header));
	}
	total_len = 0;

	ABCTL_DBG("cature_1, handle:%d, case_id:%d, time:%d, frame len: %d, need: %d, sample_size: (dev:%d, chan:%d)!\n", handle, case_id, time, length, needed, audio_get_dev_sample_size(pcm_name), fmt->sample_size);
	do {
		ret = audio_read_frame(handle, audiobuf, length);
		if (ret < 0)							   
			break;

		if(fmt->codec_type == AUDIO_CODEC_AAC){
			offset = 0;
			do{
				ret = get_aac_frame_len(audiobuf+offset);
				if(ret == 0)
					break;
				ABCTL_DBG("aac frame len: %d, at line %d\n", ret, __LINE__);
				len = write(fd, audiobuf+offset, ret);
				offset += ret;
				total_len += ret;
			}while(1);
		}else{
			len = write(fd, audiobuf, ret);
			if (len < 0)
				break;
			total_len += len;
		}

		switch(case_id){
			case VOLUME_CASE:
				if(((total_len*3/needed) > 1) && (chan_vol != 1)){
					chan_vol = 1;
					audio_set_volume(handle, chan_vol);
				}else if(((total_len*3/needed) > 0) && (chan_vol != 192)){
					chan_vol = 192;
					audio_set_volume(handle, chan_vol);
				}
			break;
			case MVOLUME_CASE:
				if(((total_len*3/needed) > 1) && (dev_vol != 1)){
					dev_vol = 1;
					audio_set_master_volume(pcm_name, dev_vol);
				}else if(((total_len*3/needed) > 0) && (dev_vol != 64)){
					dev_vol = 64;
					audio_set_master_volume(pcm_name, dev_vol);
				}
			break;
			case MUTE_CASE:
				if(((total_len*2/needed) > 0) && (chan_mute != 1)){
					chan_mute = 1;
					audio_set_mute(handle, chan_mute);
				}
			break;
			default:
			break;
		}
	} while (total_len < needed);
	
	free(audiobuf);
	close(fd);
	//audio_put_channel(handle);

	return 0;
}

int unitest_playback(char *pcm_name, int handle, audio_fmt_t	*dev_attr, audio_fmt_t *fmt, int time, int case_id)
{
	struct wav_header header;
	int ch, ret = 0;
	int len, fd;
	char wavfile[128];
	char *audiobuf = NULL;
	long total_len = 0, file_size = 0;
	int	dev_vol = 100, chan_vol = 256, chan_mute = 0;
	int length;
	int module;
	char	sample_dir[32];
	char	adts_header[ADTS_HEADER_LEN];

	if(case_id == MULTIMIX_CASE){
		mkdir("/mnt/mix", 777);
		snprintf(sample_dir, sizeof(sample_dir), "/mnt/mix/%05d", dev_attr->samplingrate);
		mkdir(sample_dir, 777);
	}else{
		snprintf(sample_dir, sizeof(sample_dir), "/mnt/%05d", dev_attr->samplingrate);
		mkdir(sample_dir, 777);
	}

	if(fmt->codec_type == AUDIO_CODEC_AAC)
		snprintf(wavfile, sizeof(wavfile), "%s/rec_%05d_%05d_%d_%02d_%d.aac"
			,sample_dir
			,dev_attr->samplingrate, fmt->samplingrate, fmt->channels
			,fmt->bitwidth, fmt->codec_type);
	else
		snprintf(wavfile, sizeof(wavfile), "%s/rec_%05d_%05d_%d_%02d_%d.wav"
		,sample_dir
		,dev_attr->samplingrate, fmt->samplingrate, fmt->channels
		,fmt->bitwidth, fmt->codec_type);

	fmt->sample_size = SAMPLE_SIZE(fmt->samplingrate);
	#if 0
	audio_set_format(pcm_name, dev_attr);
	handle = audio_get_channel_codec(pcm_name, fmt, CHANNEL_BACKGROUND);
	if (handle < 0) {
		ABCTL_ERR("Get channel err: %d\n", handle);
		return -1;
	}
	#endif
	audio_set_master_volume(pcm_name, dev_vol);
	audio_set_volume(handle, chan_vol);
	audio_set_mute(handle, chan_mute);
	
	fd = open(wavfile, O_RDONLY);
	if (fd < 0) {
		ABCTL_ERR("Please output right wav file path: %d\n", fd);
		audio_put_channel(handle);
		return -1;
	}

	total_len = 0;
	file_size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	if(fmt->codec_type != AUDIO_CODEC_AAC){
		safe_read(fd, &header, sizeof(struct wav_header));
		length = get_frame_size(fmt) * fmt->sample_size / audio_get_codec_ratio(fmt);
		audiobuf = (char*)malloc(length);
	}else{
		length = 8096;//get it from frame header.
		audiobuf = (char*)malloc(8096);
	}
	ABCTL_ERR("~~~~~~~~~~~~~~~~~~~~~~~~~~~Playing:%s, frame len: %d, sample_size: (dev:%d, chan:%d)!\n", wavfile, length, audio_get_dev_sample_size(pcm_name), fmt->sample_size);
	
	if(NULL == audiobuf) {
		ABCTL_ERR("audiobuf malloc failed!!\n");
		close(fd);
		audio_put_channel(handle);
		return -1;
	}
	
	do {
		if(fmt->codec_type == AUDIO_CODEC_AAC){
			ret = safe_read(fd, adts_header, ADTS_HEADER_LEN);
			if(ret <= 0){
				printf("aac frame len: %d, at line %d\n", ret, __LINE__);
				break;
			}
			ret = get_aac_frame_len(adts_header);
			//ABCTL_ERR("aac frame len: %d, at line %d\n", ret, __LINE__);
			memcpy(audiobuf, adts_header, ADTS_HEADER_LEN);
			safe_read(fd, audiobuf+ADTS_HEADER_LEN, ret- ADTS_HEADER_LEN);
			len = ret;
		}else{
			len = safe_read(fd, audiobuf, length);
		}	
		
		if (len <= 0) {
			ABCTL_DBG("read file end!\n");
			break;
		}

		ret = audio_write_frame(handle, audiobuf, len);
		if (ret < 0){                               
			ABCTL_ERR("audio_write_frame err: %d\n", ret);
			break;
		}
		total_len += ret;
		switch(case_id){
			case VOLUME_CASE:
				if(((total_len*3/file_size) > 1) && (chan_vol != 1)){
					chan_vol = 1;
					audio_set_volume(handle, chan_vol);
				}else if(((total_len*3/file_size) > 0) && (chan_vol != 192)){
					chan_vol = 192;
					audio_set_volume(handle, chan_vol);
				}
			break;
			case MVOLUME_CASE:
				if(((total_len*3/file_size) > 1) && (dev_vol != 1)){
					dev_vol = 1;
					audio_set_master_volume(pcm_name, dev_vol);
				}else if(((total_len*3/file_size) > 0) && (dev_vol != 72)){
					dev_vol = 72;
					audio_set_master_volume(pcm_name, dev_vol);
				}
			break;
			case MUTE_CASE:
				if(((total_len*2/file_size) > 0) && (chan_mute != 1)){
					chan_mute = 1;
					audio_set_mute(handle, chan_mute);
				}
			break;
			case 0:
			case 4:
			case 5:
			default:
			break;
		}
	} while (length > 0);
	
	free(audiobuf);
	close(fd);
	//audio_put_channel(handle);

	return 0;
}

void* thread_record(void* arg)
{
	thread_info_t*	info = (thread_info_t*)arg;	
	unitest_capture(info->pcm_name, info->handle, &info->dev_attr, &info->fmt, info->time, info->case_id);
	
	return NULL;
}

void* thread_play(void* arg)
{
	thread_info_t*	info = (thread_info_t*)arg;	
	unitest_playback(info->pcm_name, info->handle, &info->dev_attr, &info->fmt, info->time, info->case_id);
	
	return NULL;
}

int	create_thread(char *pcm_name, int handle, audio_dev_attr_t	*dev_attr, audio_fmt_t *fmt, int time, int case_id)
{
	int ret;

	memcpy(&thread_info[handle].pcm_name, pcm_name, sizeof(thread_info[handle].pcm_name));
	thread_info[handle].handle = handle;
	memcpy(&thread_info[handle].dev_attr, dev_attr, sizeof(audio_dev_attr_t));
	memcpy(&thread_info[handle].fmt, fmt, sizeof(audio_fmt_t));
	thread_info[handle].time = time;
	thread_info[handle].case_id = case_id;

	if(strstr(pcm_name, "mic") != NULL){
		ret = pthread_create(&thread_id[handle], NULL, thread_record, &thread_info[handle]);
		if (ret != 0) {
			ABCTL_ERR("Create unitest_playback thread failed!\n");
			return -1;
		}
	}else{
		ret = pthread_create(&thread_id[handle], NULL, thread_play, &thread_info[handle]);
		if (ret != 0) {
			ABCTL_ERR("Create unitest_playback thread failed!\n");
			return -1;
		}
	}

	return 0;
}

void unitest_proc_base(unitest_info_t* info)
{
	int	i, chn_para1, chn_para2, chn_para3, chn_para4;

	for(i=0;dev_samplerate[i]!=-1;++i){
		if(info->preset_attr.samplingrate != -1){
			if(info->preset_attr.samplingrate != dev_samplerate[i])
				continue;
			else
				info->dev_attr.samplingrate = dev_samplerate[i];
		}else{
			info->dev_attr.samplingrate = dev_samplerate[i];
		}
		audio_set_format(info->pcm_name, &info->dev_attr);
		for(chn_para1=0;chan_samplerate[chn_para1]!=-1;++chn_para1){
			for(chn_para2=0;chan_tracks[chn_para2]!=-1;++chn_para2){
				for(chn_para3=0;chan_bitwidth[chn_para3]!=-1;++chn_para3){
					for(chn_para4=0;chan_codectype[chn_para4]!=-1;++chn_para4){
						info->fmt.samplingrate = chan_samplerate[chn_para1];
						info->fmt.channels = chan_tracks[chn_para2];
						info->fmt.bitwidth = chan_bitwidth[chn_para3];
						info->fmt.codec_type = chan_codectype[chn_para4];
						info->fmt.sample_size = SAMPLE_SIZE(info->fmt.samplingrate);
						if(info->fmt.sample_size > 1024){
							continue;
						}
						info->handle = audio_get_channel_codec(info->pcm_name, &info->fmt, CHANNEL_BACKGROUND);
						if (info->handle < 0) {
							ABCTL_ERR("Get channel err: %d\n", info->handle);
							continue;
						}
						if(strstr(info->pcm_name, "mic") != NULL)
							unitest_capture(info->pcm_name, info->handle, &info->dev_attr, &info->fmt, info->time, info->case_id);
						else
							unitest_playback(info->pcm_name, info->handle, &info->dev_attr, &info->fmt, info->time, info->case_id);
						audio_put_channel(info->handle);
					}
				}
			}
		}
	}
}

void unitest_proc_altfmt(unitest_info_t* info)
{
	int	i, chn_para1, chn_para2, chn_para3, chn_para4;

	for(i=0;dev_samplerate[i]!=-1;++i){
		if(info->preset_attr.samplingrate != -1){
			if(info->preset_attr.samplingrate != dev_samplerate[i])
				continue;
			else
				info->dev_attr.samplingrate = dev_samplerate[i];
		}else{
			info->dev_attr.samplingrate = dev_samplerate[i];
		}
		audio_set_format(info->pcm_name, &info->dev_attr);
		info->handle = audio_get_channel_codec(info->pcm_name, &info->fmt, CHANNEL_BACKGROUND);
		if (info->handle < 0) {
			ABCTL_ERR("Get channel err: %d\n", info->handle);
			continue;
		}
		
		for(chn_para1=0;chan_samplerate[chn_para1]!=-1;++chn_para1){
			for(chn_para2=0;chan_tracks[chn_para2]!=-1;++chn_para2){
				for(chn_para3=0;chan_bitwidth[chn_para3]!=-1;++chn_para3){
					for(chn_para4=0;chan_codectype[chn_para4]!=-1;++chn_para4){
						info->fmt.samplingrate = chan_samplerate[chn_para1];
						info->fmt.channels = chan_tracks[chn_para2];
						info->fmt.bitwidth = chan_bitwidth[chn_para3];
						info->fmt.codec_type = chan_codectype[chn_para4];
						info->fmt.sample_size = SAMPLE_SIZE(info->fmt.samplingrate);						
						
						audio_set_chn_fmt(info->handle, &info->fmt);
						if(strstr(info->pcm_name, "mic") != NULL){
							unitest_capture(info->pcm_name, info->handle, &info->dev_attr, &info->fmt, info->time, info->case_id);
						}
						else{
							unitest_playback(info->pcm_name, info->handle, &info->dev_attr, &info->fmt, info->time, info->case_id);
						}
					}
				}
			}
		}

		audio_put_channel(info->handle);
	}
}

void unitest_proc_multi_thread(unitest_info_t* info)
{
	int	i, chn_para1, chn_para2, chn_para3, chn_para4;
	int	handle;

	for(i=0;dev_samplerate[i]!=-1;++i){
		if(info->preset_attr.samplingrate != -1){
			if(info->preset_attr.samplingrate != dev_samplerate[i])
				continue;
			else
				info->dev_attr.samplingrate = dev_samplerate[i];
		}else{
			info->dev_attr.samplingrate = dev_samplerate[i];
		}
		audio_set_format(info->pcm_name, &info->dev_attr);
		for(chn_para1=0;chan_samplerate[chn_para1]!=-1;++chn_para1){
			for(chn_para2=0;chan_tracks[chn_para2]!=-1;++chn_para2){
				for(chn_para3=0;chan_bitwidth[chn_para3]!=-1;++chn_para3){
					for(chn_para4=0;chan_codectype[chn_para4]!=-1;++chn_para4){
						info->fmt.samplingrate = chan_samplerate[chn_para1];
						info->fmt.channels = chan_tracks[chn_para2];
						info->fmt.bitwidth = chan_bitwidth[chn_para3];
						info->fmt.codec_type = chan_codectype[chn_para4];
						info->fmt.sample_size = SAMPLE_SIZE(info->fmt.samplingrate);
						info->handle = audio_get_channel_codec(info->pcm_name, &info->fmt, CHANNEL_BACKGROUND);
						if (info->handle < 0) {
							ABCTL_ERR("Get channel err: %d\n", info->handle);
							continue;
						}
						create_thread(info->pcm_name, info->handle, &info->dev_attr, &info->fmt, info->time, info->case_id);
					}
				}
			}
		}
		for(handle=0;thread_id[handle]!=-1;++handle){
			pthread_join(thread_id[handle], NULL);
			audio_put_channel(handle);
		}
	}
}

void unitest_proc_multi_mix_thread(unitest_info_t* info)
{
	int	i, chn_para1, chn_para2, chn_para3, chn_para4;
	int	handle;

	for(i=0;dev_samplerate[i]!=-1;++i){
		if(info->preset_attr.samplingrate != -1){
			if(info->preset_attr.samplingrate != dev_samplerate[i])
				continue;
			else
				info->dev_attr.samplingrate = dev_samplerate[i];
		}else{
			info->dev_attr.samplingrate = dev_samplerate[i];
		}
		audio_set_format(info->pcm_name, &info->dev_attr);
		audio_set_format(info->pcm_name_2, &info->dev_attr);
		for(chn_para1=0;chan_samplerate[chn_para1]!=-1;++chn_para1){
			for(chn_para2=0;chan_tracks[chn_para2]!=-1;++chn_para2){
				for(chn_para3=0;chan_bitwidth[chn_para3]!=-1;++chn_para3){
					for(chn_para4=0;chan_codectype[chn_para4]!=-1;++chn_para4){
						info->fmt.samplingrate = chan_samplerate[chn_para1];
						info->fmt.channels = chan_tracks[chn_para2];
						info->fmt.bitwidth = chan_bitwidth[chn_para3];
						info->fmt.codec_type = chan_codectype[chn_para4];
						info->fmt.sample_size = SAMPLE_SIZE(info->fmt.samplingrate);
						info->handle = audio_get_channel_codec(info->pcm_name, &info->fmt, CHANNEL_BACKGROUND);
						if (info->handle < 0) {
							ABCTL_ERR("Get channel err: %d\n", info->handle);
							continue;
						}
						ABCTL_ERR("~~~~~get handle: %d\n", info->handle);
						create_thread(info->pcm_name, info->handle, &info->dev_attr, &info->fmt, info->time, MULTIMIX_CASE);
						
						info->handle = audio_get_channel_codec(info->pcm_name_2, &info->fmt, CHANNEL_BACKGROUND);
						if (info->handle < 0) {
							ABCTL_ERR("Get channel err: %d\n", info->handle);
							continue;;
						}
						ABCTL_ERR("~~~~~get handle: %d\n", info->handle);
						create_thread(info->pcm_name_2, info->handle, &info->dev_attr, &info->fmt, info->time, BASE_CASE);

					}
				}
			}
		}
		for(handle=0;thread_id[handle]!=-1;++handle){
			pthread_join(thread_id[handle], NULL);
			audio_put_channel(handle);
		}
	}
}

int unitest_record(int argc, char **argv, int case_id)
{
	int ch, option_index;
	char const short_options[] = "c:v:o:w:s:n:t:";
	struct option long_options[] = {
		{ "channel", 0, NULL, 'c' },
		{ "volume", 0, NULL, 'v' },
		{ "output", 0, NULL, 'o' },
		{ "bit-width", 0, NULL, 'w' },
		{ "sample-rate", 0, NULL, 's' },
		{ "channel-count", 0, NULL, 'n' },
		{ "time", 0, NULL, 't' },
		{ 0, 0, 0, 0 },
	};
	
	memcpy(unitest_info.pcm_name, "default_mic", sizeof("default_mic"));
	unitest_info.case_id = case_id;

	while ((ch = getopt_long(argc, argv, short_options,
					long_options, &option_index)) != -1) {
		switch (ch) {
			case 'c':
				sprintf(unitest_info.pcm_name, "%s", optarg);
				break;
			case 'v':
				unitest_info.volume = strtol(optarg, NULL, 0);
				break;
			case 'w':
				unitest_info.preset_attr.bitwidth = strtol(optarg, NULL, 0);
				break;
			case 's':
				unitest_info.preset_attr.samplingrate = strtol(optarg, NULL, 0);
				break;
			case 'n':
				unitest_info.preset_attr.channels = strtol(optarg, NULL, 0);
				break;
			case 't':
				unitest_info.time = strtol(optarg, NULL, 0);
				break;
			default:
				ABCTL_ERR("Try `ctrlab --help' for more information.\n");
				return -1;
		}
	};

	ABCTL_ERR("Goto line: %d, case_id: %d\n", __LINE__, case_id);
	switch(case_id){
		case BASE_CASE:
		case VOLUME_CASE:
		case MVOLUME_CASE:
		case MUTE_CASE:
		case CODEC_CASE:
			unitest_proc_base(&unitest_info);
		break;
		case ALTFMT_CASE:
			unitest_proc_altfmt(&unitest_info);
		break;
		case MULTI_CASE:
			unitest_proc_multi_thread(&unitest_info);
		break;
		case MULTIMIX_CASE:
			unitest_proc_multi_mix_thread(&unitest_info);
		break;
		default:
		break;
	}

	return 0;
}

int unitest_play(int argc, char **argv, int 	case_id)
{
	int ch, option_index;
	char const short_options[] = "c:v:d:w:s:n:t:p:";
	struct option long_options[] = {
		{ "channel", 0, NULL, 'c' },
		{ "volume", 0, NULL, 'v' },
		{ "input", 0, NULL, 'd' },
		{ "bit-width", 0, NULL, 'w' },
		{ "sample-rate", 0, NULL, 's' },
		{ "channel-count", 0, NULL, 'n' },
		{ "count", 0, NULL, 't'},
		{ "priority", 0, NULL, 'p'},
		{ 0, 0, 0, 0 },
	};
	
	int count = 1;
	int priority = CHANNEL_BACKGROUND;

	memcpy(unitest_info.pcm_name, "default", sizeof("default"));
	unitest_info.case_id = case_id;

	while ((ch = getopt_long(argc, argv, short_options,
					long_options, &option_index)) != -1) {
		switch (ch) {
			case 'c':
				sprintf(unitest_info.pcm_name, "%s", optarg);
				break;
			case 'v':
				unitest_info.volume = strtol(optarg, NULL, 0);
				break;
			case 'd':
				//wavfile = optarg;
				break;
			case 'w':
				unitest_info.preset_attr.bitwidth = strtol(optarg, NULL, 0);
				break;
			case 's':
				unitest_info.preset_attr.samplingrate = strtol(optarg, NULL, 0);
				break;
			case 'n':
				unitest_info.preset_attr.channels = strtol(optarg, NULL, 0);
				break;
			case 't':
				unitest_info.count = strtol(optarg, NULL, 0);
				break;
			case 'p':
				unitest_info.priority = strtol(optarg, NULL, 0);
				break;
			default:
				ABCTL_ERR("Try `ctrlab --help' for more information.\n");
				return -1;
		}
	};

	switch(case_id){
		case BASE_CASE:
		case VOLUME_CASE:
		case MVOLUME_CASE:
		case MUTE_CASE:
			unitest_proc_base(&unitest_info);
		break;
		case ALTFMT_CASE:
			unitest_proc_altfmt(&unitest_info);
		break;
		case MULTI_CASE:
			unitest_proc_multi_thread(&unitest_info);
		break;
		case MULTIMIX_CASE:
			unitest_proc_multi_thread(&unitest_info);
		break;
		case CODEC_CASE:
			unitest_proc_base(&unitest_info);
		break;
		default:
		break;
	}

	return 0;
}

int analysis_subcase(int argc, char **argv, case_callback cb)
{
	int	case_id;

	if (!strcmp(argv[1], "base"))
		case_id = BASE_CASE;
	else if (!strcmp(argv[1], "volume"))
		case_id = VOLUME_CASE;
	else if (!strcmp(argv[1], "mvolume"))
		case_id = MVOLUME_CASE;
	else if (!strcmp(argv[1], "mute"))
		case_id = MUTE_CASE;
	else if (!strcmp(argv[1], "altfmt"))
		case_id = ALTFMT_CASE;
	else if (!strcmp(argv[1], "multi"))
		case_id = MULTI_CASE;
	else if (!strcmp(argv[1], "multimix"))
		case_id = MULTIMIX_CASE;
	else if (!strcmp(argv[1], "codec"))
		case_id = CODEC_CASE;
	else
		return -1;

	cb(--argc, ++argv, case_id);
	return 0;
}

int unitest_main(int argc, char **argv)
{
	memset(thread_id, -1, sizeof(thread_id));
	memset(thread_info, 0, sizeof(thread_info));
	
	if (!strcmp(argv[1], "record"))
		analysis_subcase(--argc, ++argv, unitest_record);
	else if (!strcmp(argv[1], "play"))
		analysis_subcase(--argc, ++argv, unitest_play);

	return 0;
}

