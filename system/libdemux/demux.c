#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <assert.h>

#include "demux.h"

#define VIDEO_FRAME_I 0x101
#define VIDEO_FRAME_P 0x102
#define MAX_DEMUX_NUM		128
#define SKIP_BYTES			21
#define MAX_STREAM_NUM 5

static void demux_show_stream_info(struct stream_info_t *info)
{
    return;
	int j = 0;
	char *p = NULL;
	printf("\n\nstream info +++++++++++++++\n");
	printf("index\t->%d\n",info->stream_index);
	printf("type\t->%2X\n",info->codec_id);
	printf("sample_rate\t->%d\n",info->sample_rate);
	printf("bit_rate\t->%d\n",info->bit_rate);
	printf("bit_width\t->%d\n",info->bit_width);
	printf("width\t->%d\n",info->width);
	printf("height\t->%d\n",info->height);
	printf("fps\t->%d\n",info->fps);
	printf("timescale\t->%d\n",info->timescale);
	printf("channels\t->%d\n",info->channels);
	printf("extradata :%2d ->", info->extradata_size);
	p = info->extradata;
	if(p != NULL){
		for(j = 0;j < info->extradata_size; j++){
			printf("%3x", p[j]);
		}
	}
	printf("\n");
	printf("stream end\n");
}
/*
    AV_SAMPLE_FMT_NONE = -1,
    AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
    AV_SAMPLE_FMT_S16,         ///< signed 16 bits
    AV_SAMPLE_FMT_S32,         ///< signed 32 bits
    AV_SAMPLE_FMT_FLT,         ///< float
    AV_SAMPLE_FMT_DBL,         ///< double

    AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
    AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
    AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
    AV_SAMPLE_FMT_FLTP,        ///< float, planar
    AV_SAMPLE_FMT_DBLP,        ///< double, planar

    AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
 * */
static int demux_get_bit_width_from_sample_format(int sample_fmt){
	int bit_width = -1;
	switch(sample_fmt){
		case AV_SAMPLE_FMT_U8 :
		case AV_SAMPLE_FMT_U8P :
			bit_width = 8;
			break;
		case AV_SAMPLE_FMT_S16 :
		case AV_SAMPLE_FMT_S16P :
			bit_width = 16;
			break;
		case AV_SAMPLE_FMT_S32 :
		case AV_SAMPLE_FMT_S32P :
			bit_width = 32;
			break;
		case AV_SAMPLE_FMT_FLT :
		case AV_SAMPLE_FMT_DBL :
		case AV_SAMPLE_FMT_FLTP :
		case AV_SAMPLE_FMT_DBLP :
			bit_width = 32;
			break;
		default :
			bit_width = -1;

	}
	return bit_width ;
}

static int demux_photo_codec(struct demux_t *demux)
{
    AVCodecContext  *codec_context = NULL;
    enum AVCodecID     codec_id = AV_CODEC_ID_NONE;
    if ((demux->av_format_dtx->nb_streams == 1) && (demux->video_index == 0))
    {
        codec_context = demux->av_format_dtx->streams[demux->video_index]->codec;
        codec_id = codec_context->codec_id;
        if (codec_id == AV_CODEC_ID_MJPEG)
        {
            return 0;
        }
    }
    return -1;
}

struct demux_t *demux_init(char *file)
{
	struct demux_t *demux;
	AVStream *av_stream;
	int ret = 0;
	const char * containername = NULL;

	demux = malloc(sizeof(struct demux_t));
	if (!demux)
		return NULL;

	memset(demux, 0, sizeof(struct demux_t));
	av_register_all();
    if (strncasecmp(file, "http", 4) == 0)
    {
        avformat_network_init();
    }
	ret = avformat_open_input(&demux->av_format_dtx, file, NULL, NULL);
	if (ret < 0) {
		printf("demux init open failed %s %d \n", file, ret);
		free(demux);
		return NULL;
	}

	ret = avformat_find_stream_info(demux->av_format_dtx, NULL);
	if (ret < 0) {
		printf("demux init find stream failed %s %d \n", file, ret);
		free(demux);
		return NULL;
	}
	demux->time_pos = 0;
	demux->stream_num =  demux->av_format_dtx->nb_streams;
	demux->stream_info = (struct stream_info_t *)malloc(
			sizeof(struct stream_info_t) * demux->stream_num);
	if(demux->stream_info == NULL){
		goto demux_init_error_handler;
	}
	memset(demux->stream_info, 0,
			sizeof(struct stream_info_t) * demux->stream_num);

	int i = 0;
#if 0
	printf("\n\n");
	av_dump_format(demux->av_format_dtx, 0, file, 0);
	printf("\n\n");
#endif
    demux->video_index = -1;
    demux->audio_index = -1;
    demux->extra_index = -1;
    for (i = 0; i < demux->av_format_dtx->nb_streams; i++) {
        AVStream *stream = demux->av_format_dtx->streams[i];
		AVCodecContext  *codec_context = stream->codec;
		struct stream_info_t *stream_info = demux->stream_info + i;
		stream_info->frame_num = stream->frame_num;

		switch(codec_context->codec_id){
			case  AV_CODEC_ID_H264:
				stream_info->stream_index  = i;
				stream_info->codec_id = DEMUX_VIDEO_H264;
				stream_info->bit_rate = demux->av_format_dtx->bit_rate;
				stream_info->width  = codec_context->width;
				stream_info->height = codec_context->height;
				stream_info->fps = stream->avg_frame_rate.num/stream->avg_frame_rate.den;
                stream_info->timescale = stream->time_base.den / stream->time_base.num;
                demux->video_index =  demux->video_index == -1? i :demux->video_index;
				break;
			case  AV_CODEC_ID_H265:
				stream_info->stream_index  = i;
				stream_info->codec_id = DEMUX_VIDEO_H265;
				stream_info->bit_rate = demux->av_format_dtx->bit_rate;
				stream_info->width  = codec_context->width;
				stream_info->height = codec_context->height;
				stream_info->fps = stream->avg_frame_rate.num/stream->avg_frame_rate.den;
                stream_info->timescale = stream->time_base.den / stream->time_base.num;
                demux->video_index =  demux->video_index == -1? i :demux->video_index;
				break;
            case AV_CODEC_ID_MJPEG:
                stream_info->stream_index  = i;
                stream_info->codec_id = DEMUX_VIDEO_MJPEG;
                stream_info->bit_rate = demux->av_format_dtx->bit_rate;
                stream_info->width  = codec_context->width;
                stream_info->height = codec_context->height;

                if (stream->avg_frame_rate.den != 0)
                    stream_info->fps = stream->avg_frame_rate.num/stream->avg_frame_rate.den;
                if (stream->time_base.num != 0)
                    stream_info->timescale = stream->time_base.den / stream->time_base.num;
                demux->video_index =  demux->video_index == -1? i :demux->video_index;
                break;
			case  AV_CODEC_ID_ADPCM_G726:
				stream_info->stream_index  = i;
				stream_info->codec_id = DEMUX_AUDIO_ADPCM_G726;
				stream_info->width  = 0;
				stream_info->height = 0;
				stream_info->fps = 0;
				stream_info->channels =  codec_context->channels;
				stream_info->sample_rate =  codec_context->sample_rate;
				stream_info->bit_width = demux_get_bit_width_from_sample_format(codec_context->sample_fmt);
				stream_info->bit_rate = stream_info->channels * stream_info->bit_width *
							stream_info->channels *
							stream_info->sample_rate ;
			//	stream_info->bit_rate /= 2;
                stream_info->timescale = stream->time_base.den / stream->time_base.num;
                demux->audio_index =  demux->audio_index == -1? i :demux->audio_index;
				break;
			case  AV_CODEC_ID_PCM_MULAW:
				stream_info->stream_index  = i;
				stream_info->codec_id = DEMUX_AUDIO_PCM_ULAW;
				stream_info->width  = 0;
				stream_info->height = 0;
				stream_info->fps = 0;
				stream_info->channels =  codec_context->channels;
				stream_info->sample_rate =  codec_context->sample_rate;
				stream_info->bit_width = demux_get_bit_width_from_sample_format(codec_context->sample_fmt);
				stream_info->bit_rate = stream_info->channels * stream_info->bit_width *
							stream_info->channels *
							stream_info->sample_rate ;
				stream_info->bit_rate /= 2;
                stream_info->timescale = stream->time_base.den / stream->time_base.num;
                demux->audio_index =  demux->audio_index == -1? i :demux->audio_index;
				break;
			case  AV_CODEC_ID_PCM_ALAW:
				stream_info->stream_index  = i;
				stream_info->codec_id = DEMUX_AUDIO_PCM_ALAW;
				stream_info->width  = 0;
				stream_info->height = 0;
				stream_info->fps = 0;
				stream_info->channels =  codec_context->channels;
				stream_info->sample_rate =  codec_context->sample_rate;
				stream_info->bit_width = demux_get_bit_width_from_sample_format(codec_context->sample_fmt);
				stream_info->bit_rate = stream_info->channels * stream_info->bit_width *
							stream_info->channels *
							stream_info->sample_rate ;
				stream_info->bit_rate /= 2;
                stream_info->timescale = stream->time_base.den / stream->time_base.num;
                demux->audio_index =  demux->audio_index == -1? i :demux->audio_index;
				break;
			case  AV_CODEC_ID_AAC:
				stream_info->stream_index  = i;
				stream_info->codec_id = DEMUX_AUDIO_AAC;
				stream_info->width  = 0;
				stream_info->height = 0;
				stream_info->fps = 0;
				stream_info->channels =  codec_context->channels;
				stream_info->sample_rate =  codec_context->sample_rate;
				stream_info->bit_width = demux_get_bit_width_from_sample_format(codec_context->sample_fmt);
				stream_info->bit_rate = stream_info->channels * stream_info->bit_width *
							stream_info->channels *
							stream_info->sample_rate ;
			//	stream_info->bit_rate /= 2;
                stream_info->timescale = stream->time_base.den / stream->time_base.num;
                demux->audio_index =  demux->audio_index == -1? i :demux->audio_index;
				break;
			case  AV_CODEC_ID_PCM_S16LE:
				stream_info->stream_index  = i;
				stream_info->codec_id = DEMUX_AUDIO_PCM_S16E;
				stream_info->width  = 0;
				stream_info->height = 0;
				stream_info->fps = 0;
				stream_info->channels =  codec_context->channels;
				stream_info->sample_rate =  codec_context->sample_rate;
				stream_info->bit_width = 2;
				stream_info->bit_rate = stream_info->channels * stream_info->bit_width *
							stream_info->channels *
							stream_info->sample_rate ;
                stream_info->timescale = stream->time_base.den / stream->time_base.num;
            demux->audio_index =  demux->audio_index == -1? i :demux->audio_index;
			case  AV_CODEC_ID_PCM_S32LE:
				stream_info->stream_index  = i;
				stream_info->codec_id = DEMUX_AUDIO_PCM_S32E;
				stream_info->width  = 0;
				stream_info->height = 0;
				stream_info->fps = 0;
				stream_info->channels =  codec_context->channels;
				stream_info->sample_rate =  codec_context->sample_rate;
				stream_info->bit_width = 4;
				stream_info->bit_rate = stream_info->channels * stream_info->bit_width *
							stream_info->channels *
							stream_info->sample_rate ;
                stream_info->timescale = stream->time_base.den / stream->time_base.num;
                demux->audio_index =  demux->audio_index == -1? i :demux->audio_index;
				break;
            case AV_CODEC_ID_MP3:
                stream_info->stream_index  = i;
                stream_info->codec_id = DEMUX_AUDIO_MP3;
                stream_info->width  = 0;
                stream_info->height = 0;
                stream_info->fps = 0;
                stream_info->channels =  codec_context->channels;
                stream_info->sample_rate =  codec_context->sample_rate;
                stream_info->bit_width = demux_get_bit_width_from_sample_format(codec_context->sample_fmt);
                stream_info->bit_rate = stream_info->channels * stream_info->bit_width *
                            stream_info->channels *
                            stream_info->sample_rate ;
                stream_info->timescale = stream->time_base.den / stream->time_base.num;
                demux->audio_index =  demux->audio_index == -1? i :demux->audio_index;
                break;
			default :
				printf("demux may not support the current stream type ->%d\n",codec_context->codec_id);
				continue;
		}
		if(codec_context->extradata_size > 0){
			stream_info->extradata =  malloc(codec_context->extradata_size);
			if(stream_info->extradata == NULL){
				printf("malloc stream extra data error\n");
				goto demux_init_error_handler;
			}
			memcpy(stream_info->extradata, codec_context->extradata, codec_context->extradata_size);
			stream_info->extradata_size = codec_context->extradata_size;
		}
		else{
			stream_info->extradata = NULL;
			stream_info->extradata_size = 0;
		}
		stream_info->header = NULL;
		stream_info->header_size = 0;
	//	demux_show_stream_info(stream_info);
	}
    demux->media_type = DEMUX_MEDIA_TYPE_NONE;
    if ((demux->audio_index >= 0) && (demux->video_index < 0))
    {
        demux->media_type = DEMUX_MEDIA_TYPE_MUSIC;
    }
    else if (demux->video_index >= 0)
    {
        demux->media_type = DEMUX_MEDIA_TYPE_MOVIE;
        if (demux_photo_codec(demux) == 0)
        {
            demux->media_type = DEMUX_MEDIA_TYPE_PHOTO;
        }
    }
    else
    {
        printf("Unknow meida type!\n");
        goto demux_init_error_handler;
    }
    demux->time_length =  demux->av_format_dtx->duration + (demux->av_format_dtx->duration <= INT64_MAX - 5000 ? 5000 : 0);
    if((demux->time_length <  0) && (demux->media_type != DEMUX_MEDIA_TYPE_PHOTO)){
        printf("current media file is not correct ,please check it\n");
        return NULL;
    }
    demux->time_length /= 1000;
    demux->time_pos = 0;
	for(i = 0; i < demux->stream_num; i++)
	{
		demux_show_stream_info(demux->stream_info + i);
	}
//	demux->video_index = av_find_best_stream(demux->av_format_dtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
//	demux->audio_index = av_find_best_stream(demux->av_format_dtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	demux->extra_index = -1;
	printf("demux info ->%8lld v:%d a:%d e:%d t:%d\n",
            demux->time_length,
            demux->video_index,
            demux->audio_index,
            demux->extra_index,
            demux->media_type
            );

	if (demux->video_index >= 0) {
		av_stream = demux->av_format_dtx->streams[demux->video_index];
		if (av_stream->codec->codec_id == AV_CODEC_ID_H264) {
			demux->bsfc = av_bitstream_filter_init("h264_mp4toannexb");
			if (demux->bsfc == NULL) {
				printf("demux init bitstream_filter failed \n");
			}
		} else if (av_stream->codec->codec_id == AV_CODEC_ID_H265) {
			demux->bsfc = av_bitstream_filter_init("hevc_mp4toannexb");
			if (demux->bsfc == NULL) {
				printf("demux init bitstream_filter failed \n");
			}
		}
		if((demux->av_format_dtx->iformat != NULL && demux->av_format_dtx->iformat->name != NULL)
			&&((av_stream->codec->codec_id == AV_CODEC_ID_H264) || (av_stream->codec->codec_id == AV_CODEC_ID_H265)))
		{
			printf("container name from demux:%s\n",demux->av_format_dtx->iformat->name);
			containername = demux->av_format_dtx->iformat->name;
			if(strncmp(containername,"matroska,webm",strlen(containername)) == 0 ||
			    strncmp(containername,"mov,mp4,m4a,3gp,3g2,mj2",strlen(containername)) == 0)
			{
				//mkv mp4
				demux->is_nalff = 1;
			}
		}
	}

	return demux;
demux_init_error_handler :
	if(demux){
		if (demux->bsfc)
			av_bitstream_filter_close(demux->bsfc);
		avformat_close_input(&demux->av_format_dtx);
		if(demux->stream_info ){
			int i = 0;
			struct stream_info_t *stream_info = NULL;
			for(i = 0; i < demux->stream_num; i++){
				stream_info = demux->stream_info + i;
				if(stream_info->header != NULL){
					free(stream_info->header);
					stream_info->header =  NULL;
				}
				if(stream_info->extradata != NULL){
					free(stream_info->extradata);
					stream_info->extradata = NULL;
				}
			}
			free(demux->stream_info);
		}
		free(demux);
	}

	return NULL;
}

void demux_deinit(struct demux_t *demux)
{
	if (demux->bsfc)
		av_bitstream_filter_close(demux->bsfc);
	avformat_close_input(&demux->av_format_dtx);
	//add header and extra data release
	if(demux->stream_info ){
		int i = 0;
		struct stream_info_t *stream_info = NULL;
		for(i = 0; i < demux->stream_num; i++){
			stream_info = demux->stream_info + i;
			if(stream_info->header != NULL){
				free(stream_info->header);
				stream_info->header =  NULL;
			}
			if(stream_info->extradata != NULL){
				free(stream_info->extradata);
				stream_info->extradata = NULL;
			}
		}
		free(demux->stream_info);
	}
	free(demux);

	return;
}

static int h265_head_data_parser(struct demux_t *demux, char *head, int head_len)
{
	char temp_buf[128], *temp_ptr;
	int i, j, num_arry, len, temp_len, num_nals, offset = 0;

	temp_len = head_len;
	memcpy(temp_buf, head, head_len);
	memset(head, 0, head_len);
	num_arry = temp_buf[SKIP_BYTES + 1];
	temp_len -= (SKIP_BYTES + 1);

	temp_len -= 2;
	temp_ptr = temp_buf + SKIP_BYTES + 2;
	for (i=0; i<num_arry; ++i) {

		temp_ptr++;
		temp_len--;
		num_nals = ((temp_ptr[0] << 8) | temp_ptr[1]);

		temp_ptr += 2;
		temp_len -= 2;

		for (j = 0; j < num_nals; ++j) {

			if (temp_len < 2) {
				printf("parser head len err %d \n", temp_len);
				return -1;
			}

			len = ((temp_ptr[0] << 8) | temp_ptr[1]);
			temp_ptr += 2;
			temp_len -= 2;

			memcpy(head + offset, "\x00\x00\x00\x01", 4);
			offset += 4;

			memcpy(head + offset, temp_ptr, len);
			offset += len;
			temp_ptr += len;
			temp_len -= len;
		}
	}

	return offset;
}

int demux_get_head(struct demux_t *demux, char *head, int head_len)
{
	AVFormatContext *av_format_dtx;
	AVStream *av_stream;
	unsigned char *dummy=NULL;   
	int dummy_len;  
	int len = 0, i;

	if (demux == NULL)
		return -1;

	av_format_dtx = demux->av_format_dtx;
	for (i = 0; i < av_format_dtx->nb_streams; i++) {
		av_stream = av_format_dtx->streams[i];
		if ((av_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			&& (av_stream->codec->extradata != NULL)) {
				if (av_stream->codec->extradata_size > head_len) {
					len = -1;
					break;
				}
				if ((av_stream->codec->codec_id == AV_CODEC_ID_H265) && (av_stream->codec->extradata[0] || av_stream->codec->extradata[1] || (av_stream->codec->extradata[2] > 1))) {
					memcpy(head, av_stream->codec->extradata, av_stream->codec->extradata_size);
					len = h265_head_data_parser(demux, head, av_stream->codec->extradata_size);
					demux->is_nalff = 1;
				} else {
					if ((av_stream->codec->extradata[0] || av_stream->codec->extradata[1] || (av_stream->codec->extradata[2] > 1)))
						av_bitstream_filter_filter(demux->bsfc, av_stream->codec, NULL, &dummy, &dummy_len, NULL, 0, 0);
					len = av_stream->codec->extradata_size;
					memcpy(head, av_stream->codec->extradata, len);
				}
				break;
		}
	}
	demux->frame_counter = 1;
	return len;
}

int demux_get_track_header(struct demux_t *demux, int stream_index,char *head, int head_len)
{
    assert(demux != NULL);
    assert(stream_index >= 0);
    assert(head != NULL);
    assert(head_len > 0);
	AVFormatContext *av_format_dtx;
	AVStream *av_stream;
	unsigned char *dummy=NULL;   
	int dummy_len;  
	int len = 0;

	if (demux == NULL)
		return -1;
	av_format_dtx = demux->av_format_dtx;
    if(stream_index > av_format_dtx->nb_streams){
        printf("stream index error ->%d %d\n",
                stream_index, av_format_dtx->nb_streams);
        return -1;
    }
    if (demux->media_type == DEMUX_MEDIA_TYPE_PHOTO)
    {
        printf("photo decoder no need extradata\n");
        head_len = 0;
        demux->frame_counter = 1;
        return 1;
    }
	av_stream = av_format_dtx->streams[stream_index];
    if(av_stream->codec->extradata_size > 0){
        if (av_stream->codec->extradata_size > head_len) {
            printf("stream header buffer error ->%d %d\n",
                    head_len, av_stream->codec->extradata_size);
            return -1;
        }
    }
    else{
        printf("current stream index header is 0\n");
        head_len = 0;
        demux->frame_counter = 1;
        return 1;
    }
    switch(av_stream->codec->codec_id){
        case AV_CODEC_ID_H265:
            memcpy(head, av_stream->codec->extradata, av_stream->codec->extradata_size);
            len = h265_head_data_parser(demux, head, av_stream->codec->extradata_size);

            break;
        case AV_CODEC_ID_H264:
            av_bitstream_filter_filter(demux->bsfc, av_stream->codec, NULL, &dummy, &dummy_len, NULL, 0, 0);
            len = av_stream->codec->extradata_size;
            memcpy(head, av_stream->codec->extradata, len);

            break;
        default:
            len = av_stream->codec->extradata_size;
			memcpy(head, av_stream->codec->extradata, len);
    }
	demux->frame_counter = 1;
    return len;
}
static int h265_bitstream_filter_filter(struct demux_t *demux, uint8_t **poutbuf, int *poutbuf_size, uint8_t *buf, int buf_size, int keyframe)
{
	uint8_t *out_buf, *temp_buf;
	int offset = 0, len, out_length = 0;

	out_buf = malloc(buf_size);
	if (out_buf == NULL)
		return -1;

	temp_buf = buf;
	do {
		len = ((temp_buf[offset] << 24) | (temp_buf[offset + 1] << 16) | (temp_buf[offset + 2] << 8) | temp_buf[offset + 3]);
		if (len < 0 || (len > buf_size - offset)) {
			printf("get frame len error %d %d %d \n", len, offset, buf_size);
			return -2;
		}

		memcpy(out_buf + out_length , "\x00\x00\x01", 3);
		offset += 4;
		out_length += 3;
		memcpy(out_buf + out_length, temp_buf + offset, len);
		offset += len;
		out_length += len;
	
	} while (offset < buf_size);

	*poutbuf = out_buf;
	*poutbuf_size = out_length;

	return 0;
}

int demux_get_frame(struct demux_t *demux, struct demux_frame_t *demux_frame)
{
	AVFormatContext *av_format_dtx;
	AVPacket *pkt;
	int audio_fmt, ret;
    static int64_t lastPTS[MAX_STREAM_NUM] = {0};

	if (demux == NULL)
		return -1;

	av_format_dtx = demux->av_format_dtx;
	pkt = (AVPacket *)&demux->pkt;
	//pkt->buf = demux_frame->ref_buf;
	ret = av_read_frame(av_format_dtx, pkt);
	if (ret < 0)
		return ret;
    if(pkt->stream_index != demux->video_index &&
            pkt->stream_index != demux->audio_index &&
            pkt->stream_index != demux->extra_index){
        av_packet_unref(pkt);
        demux_frame->data_size = 0;
        demux_frame->data = NULL;
        return 0;
    }
	if (av_format_dtx->streams[pkt->stream_index]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {

		demux_frame->sample_rate = av_format_dtx->streams[pkt->stream_index]->codec->sample_rate;
		demux_frame->channels = av_format_dtx->streams[pkt->stream_index]->codec->channels;
		audio_fmt = av_format_dtx->streams[pkt->stream_index]->codec->sample_fmt;
		if ((audio_fmt == AV_SAMPLE_FMT_U8) || (audio_fmt == AV_SAMPLE_FMT_U8P))
			demux_frame->bitwidth = 8;
		else if ((audio_fmt == AV_SAMPLE_FMT_S16) || (audio_fmt == AV_SAMPLE_FMT_S16P))
			demux_frame->bitwidth = 16;
		else if ((audio_fmt == AV_SAMPLE_FMT_S32) || (audio_fmt == AV_SAMPLE_FMT_S32P))
			demux_frame->bitwidth = 32;
		else if (audio_fmt < 0)
			demux_frame->bitwidth = 24;
		else
			demux_frame->bitwidth = 8;
		if (av_format_dtx->streams[pkt->stream_index]->codec->codec_id == AV_CODEC_ID_AAC) {
			demux_frame->codec_id = DEMUX_AUDIO_AAC;
		}
        else if (av_format_dtx->streams[pkt->stream_index]->codec->codec_id == AV_CODEC_ID_MP3)
        {
            demux_frame->codec_id = DEMUX_AUDIO_MP3;
        }
        else if (av_format_dtx->streams[pkt->stream_index]->codec->codec_id == AV_CODEC_ID_PCM_S16LE) {
			demux_frame->codec_id = DEMUX_AUDIO_PCM_S16E;
			demux_frame->bitwidth = 16;
		} else if (av_format_dtx->streams[pkt->stream_index]->codec->codec_id == AV_CODEC_ID_PCM_ALAW) {
			demux_frame->codec_id = DEMUX_AUDIO_PCM_ALAW;
			demux_frame->bitwidth = 16;
		} else {
			demux_frame->codec_id = DEMUX_AUDIO_PCM_S32E;
			demux_frame->bitwidth = 32;
		}

		demux_frame->data = pkt->data;
		demux_frame->data_size = pkt->size;
		demux_frame->timestamp = pkt->pts;
	}
    else if (av_format_dtx->streams[pkt->stream_index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
    {
        if ((av_format_dtx->streams[pkt->stream_index]->codec->codec_id == AV_CODEC_ID_H264) || 
            (av_format_dtx->streams[pkt->stream_index]->codec->codec_id == AV_CODEC_ID_HEVC))
        {
            if (av_format_dtx->streams[pkt->stream_index]->codec->codec_id == AV_CODEC_ID_H264)
                demux_frame->codec_id = DEMUX_VIDEO_H264;
            else
                demux_frame->codec_id = DEMUX_VIDEO_H265;
            if (pkt->flags == AV_PKT_FLAG_KEY)
                demux_frame->flags = VIDEO_FRAME_I;
            else
                demux_frame->flags = VIDEO_FRAME_P;

            if ((demux_frame->codec_id == DEMUX_VIDEO_H265) && (demux->is_nalff)) {
                if (demux->bsfc != NULL) {
                    ret = av_bitstream_filter_filter(demux->bsfc, av_format_dtx->streams[pkt->stream_index]->codec, NULL, &demux_frame->data, &demux_frame->data_size, pkt->data, pkt->size, 0);
                    if (ret < 0)
                        return ret;
                } else {
                    ret = h265_bitstream_filter_filter(demux, &demux_frame->data, &demux_frame->data_size, pkt->data, pkt->size, 0);
                    if (ret < 0)
                        return ret;
                }
            } else if (demux_frame->codec_id == DEMUX_VIDEO_H264 && (demux->is_nalff)) {
                ret = av_bitstream_filter_filter(demux->bsfc, av_format_dtx->streams[pkt->stream_index]->codec, NULL, &demux_frame->data, &demux_frame->data_size, pkt->data, pkt->size, 0);
                if (ret < 0)
                    return ret;
            }
            else
            {
                demux_frame->data = pkt->data;
                demux_frame->data_size = pkt->size;
            }
            demux_frame->timestamp = pkt->pts;
            if (pkt->stream_index < MAX_STREAM_NUM)
            {
                if (pkt->pts < 0)
                {
                    demux_frame->timestamp = lastPTS[pkt->stream_index];
                }
                else
                {
                    lastPTS[pkt->stream_index] = pkt->pts;
                }
            }
        }
        else if(av_format_dtx->streams[pkt->stream_index]->codec->codec_id == AV_CODEC_ID_MJPEG)
        {
            demux_frame->codec_id = DEMUX_VIDEO_MJPEG;
            demux_frame->flags = VIDEO_FRAME_I;
            demux_frame->data_size = pkt->size;
            demux_frame->data = pkt->data;
        }
    }

    //demux_frame->ref_buf = pkt->buf;
    demux->frame_counter++;
    demux->time_pos = demux_frame->timestamp ;
    demux->frame_counter++;
    return demux_frame->data_size;
}

int demux_put_frame(struct demux_t *demux, struct demux_frame_t *demux_frame)
{
    assert(demux != NULL);
    assert(demux_frame != NULL);
	AVPacket *pkt;

	if (demux == NULL)
		return -1;

	pkt = (AVPacket *)&demux->pkt;
	if ((demux->bsfc == NULL) && (demux_frame->codec_id == DEMUX_VIDEO_H265))
		free(demux_frame->data);
	else if (((demux_frame->codec_id == DEMUX_VIDEO_H265) || (demux_frame->codec_id == DEMUX_VIDEO_H264)) 
		&& (demux_frame->data != pkt->data) && (demux->bsfc != NULL))
		free(demux_frame->data);
	//pkt->buf = demux_frame->ref_buf;

	av_packet_unref(pkt);

	return 0;
}
int demux_seek(struct demux_t *demux, int64_t offset, int whence)
{
    assert(demux != NULL);
	if(demux == NULL){
		printf("dmeux inst is null seek\n");
		return -1;
	}
	int ret = -1;
	int64_t time_ms = 0;
	printf("seek ->%d offset:%8lld total:%8lld pos:%8lld\n", whence, offset, demux->time_length, demux->time_pos);
	switch(whence){
		case SEEK_SET:
			if(offset < 0){
				printf("can not seek from start with < 0 value, and pos no changed\n");
				return demux->time_pos;
			}
			else if((offset + demux->time_pos) > demux->time_length ){
				printf("seek pos is exceed the max file length and the pos is not changed\n");
				return demux->time_pos;
			}
			time_ms = offset;
			//demux->time_pos =  offset;
			break;
		case SEEK_CUR:
			if((demux->time_pos + offset ) < 0 ){
				printf("can not seek from start with < 0 value, and pos no changed\n");
				return demux->time_pos;
			}
			else if((demux->time_pos + offset) > demux->time_length){
				printf("seek pos is exceed the max file length and the pos is not changed\n");
				return demux->time_pos;
			}
			time_ms = demux->time_pos + offset;
			break;

		case SEEK_END:
			if(offset > 0 ){
				printf("seek pos is exceed the max file length and the pos is not changed\n");
				return demux->time_pos;
			}
			else if((offset + demux->time_pos) < 0){
				printf("can not seek from start with < 0 value, and pos no changed\n");
				return demux->time_pos;
			}
			time_ms = demux->time_length + offset;
			break;
		default:
			printf("seek error,and the pos not changed\n");
			return demux->time_pos;
	}
	AVFormatContext *avFormatContext = demux->av_format_dtx;
//	printf("seek result ->%8lld\n", time_ms);
	//ret = avformat_seek_file(avFormatContext, -1 , INT64_MIN, time_ms*1000, INT64_MAX, AVSEEK_FLAG_BACKWARD);
	ret = avformat_seek_file(avFormatContext, -1 , INT64_MIN, time_ms * 1000, time_ms * 1000, AVSEEK_FLAG_BACKWARD);
	if(ret < 0 ){
		printf("avformat seek error\n");
		return demux->time_pos;
	}
	demux->time_pos = time_ms;
	return time_ms;
}
int demux_set_stream_filter(struct demux_t *demux, int video_index, int audio_index, int extra_index)
{
    assert(demux != NULL);
    if(video_index != -1)
        demux->video_index = video_index;
    if(audio_index != -1)
        demux->audio_index = audio_index;
    if(extra_index != -1)
        demux->extra_index = extra_index;
    printf("video demux filter ->v:%d a:%d e:%d\n",
            demux->video_index,
            demux->audio_index,
            demux->extra_index);
	return 1;
}
int demux_get_default_stream_index(struct demux_t *demux, int *video_index, int *audio_index, int *extra_index){
    assert(demux != NULL);
    *video_index = demux->video_index;
    *audio_index = demux->audio_index;
    *extra_index = demux->extra_index;
    return 1;
}
