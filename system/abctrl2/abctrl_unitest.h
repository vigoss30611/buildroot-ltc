#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define OPT_NO_REF	1
#define OPT_EN_AEC	2
#define OPT_EN_TIME	3

#define FORMAT_PCM 1

#define     ADTS_HEADER_LEN     7

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

typedef int (*case_callback)(int argc, char **argv, int case_id);

enum _test_case_id{
    BASE_CASE = 0,
    VOLUME_CASE,
    MVOLUME_CASE,
    MUTE_CASE,
    ALTFMT_CASE,
    MULTI_CASE,
    MULTIMIX_CASE,
    CODEC_CASE,
    MAX_CASE,
};

typedef struct _thread_info{
    char    pcm_name[32];
    char    pcm_name_2[32];
    int     handle;
    audio_dev_attr_t dev_attr;
    audio_fmt_t fmt;
    int     time;
    int     priority;
    int     count;
    int     volume;
    int     case_id;
}thread_info_t;

typedef struct _unitest_info{
    char    pcm_name[32];
    char    pcm_name_2[32];
    int     handle;
    audio_dev_attr_t preset_attr;
    audio_dev_attr_t dev_attr;
    audio_fmt_t preset_fmt;
    audio_fmt_t fmt;
    int     time;
    int     priority;
    int     count;
    int     volume;
    int     case_id;
}unitest_info_t;

extern int	dev_samplerate[];
extern int	chan_samplerate[];
extern int	chan_tracks[];
extern int	chan_bitwidth[];
extern int	chan_codectype[];
extern int	chan_samplesize[];

