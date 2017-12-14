#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <libavformat/avformat.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus */

#define DEMUX_VIDEO_H264		0x01
#define DEMUX_VIDEO_H265		0x02
#define DEMUX_VIDEO_MJPEG           0x03

#define DEMUX_AUDIO_PCM_ULAW	0x10
#define DEMUX_AUDIO_PCM_ALAW	0x20
#define DEMUX_AUDIO_AAC			0x30
#define DEMUX_AUDIO_PCM_S32E	0x40
#define DEMUX_AUDIO_PCM_S16E	0x50
#define DEMUX_AUDIO_ADPCM_G726	0x60
#define DEMUX_AUDIO_MP3         0x70


typedef
enum{
    DEMUX_MEDIA_TYPE_NONE,
    DEMUX_MEDIA_TYPE_MOVIE,
    DEMUX_MEDIA_TYPE_MUSIC,
    DEMUX_MEDIA_TYPE_PHOTO
}demux_media_type_en;

struct demux_frame_t {
	uint8_t *data;
	int data_size;
	uint64_t timestamp;
	int flags;
	int codec_id;
	int channels;
	int bitwidth;
	int sample_rate;
	void *ref_buf;
};
//extra data add by Bright
struct stream_info_t {
	int stream_index;
	int codec_id;
	int sample_rate;
	int bit_rate;
	int width;
	int height;
	int fps;
	int channels;
	int bit_width;
    int timescale;
	char *extradata;
	int extradata_size;
	char *header;
	int header_size;
	int frame_num;
};
struct demux_t {
	AVFormatContext *av_format_dtx;
	AVBitStreamFilterContext *bsfc;
	AVPacket pkt;
	int video_index;
	int audio_index;
	int extra_index;
	int is_nalff;
	int index;
	int64_t time_pos ;
	int64_t time_length ;
	int frame_counter ;
	int stream_num ;
	struct stream_info_t *stream_info;
    demux_media_type_en media_type;
};


struct demux_t *demux_init(char *file);
void demux_deinit(struct demux_t *demux);
int demux_get_head(struct demux_t *demux, char *head, int head_len);
int demux_get_track_header(struct demux_t *demux, int stream_index,char *head, int head_len);
int demux_get_frame(struct demux_t *demux, struct demux_frame_t *demux_frame);
int demux_put_frame(struct demux_t *demux, struct demux_frame_t *demux_frame);
/*
 *use video as default seek ,if no video ,use audio
 * whence : SEEK_SET/SEEK_CUR/SEEK_END
 * */
int demux_seek(struct  demux_t *demux , int64_t offset_ms ,int whence);
/*
 * if the media file contains more than one video or audio ,must set filter to sellect target stream index
 * no need set when there is only one video and audio
 */
int demux_set_stream_filter(struct demux_t *demux, int video_index, int audio_index, int extra_index);
int demux_get_default_stream_index(struct demux_t *demux, int *video_index, int *audio_index, int *extra_index);

#ifdef __cplusplus
}
#endif /*__cplusplus */
