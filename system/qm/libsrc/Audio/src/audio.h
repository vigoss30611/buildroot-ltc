#ifndef __INFOTM_AUDIO__
#define __INFOTM_AUDIO__
#include <stdint.h>

int infotm_audio_out_init(void);
int infotm_audio_out_deinit(int handle);
int infotm_audio_out_send(int handle, char *data, int len);
int infotm_audio_out_set_volume(int handle, int lvolume, int rvolume);


#define MEDIA_AFRAME_FLAG_EOS   (1<<0)
typedef struct {
    int flag;
    void *buf;
    int size;
    int64_t ts;
} media_aframe_t;

typedef void (*audio_post_fn_t) (void const *client, media_aframe_t const *afr);


int infotm_audio_in_init(audio_post_fn_t audio_callback);
int infotm_audio_in_deinit(void);
int infotm_audio_in_start(void);
int infotm_audio_in_stop(void);

#endif
