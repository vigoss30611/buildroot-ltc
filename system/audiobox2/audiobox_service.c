#include <fr/libfr.h>
#include <audiobox2.h>
#include <pthread.h>
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <log.h>
#include <list.h>
#include <base.h>
#include <audio_service.h>
#include <audio_hal.h>
#include <audiobox_listen.h>
#include <audio_softvol.h>
#include <sys/time.h>

#include <audio_ctl.h>
#include <qsdk/codecs.h>

#define CODEC_EXTRA_SIZE(x)	            ((x)->channels * (((x)->bitwidth > 32 ? 32 : (x)->bitwidth) >> 3) * 128)

//#define PLAYBACK_FILE
#define WRITE_FILE_PATH		            "/mnt/sd0/playback.wav"
//#define CAPTURE_FILE
#define READ_FILE_PATH		            "/mnt/sd0/capture.wav"
//#define CAPTURE_PREPROC_FILE
#define CAPTURE_RAW_FILE_PATH	        "/mnt/sd0/capture_raw.wav"		//ALSA audio data
#define CAPTURE_PREPROC_FILE_PATH	    "/mnt/sd0/capture_preproc.wav"  //after module processing, softvol, encoding
//#define PLAYBACK_PREPROC_FILE
#define PLAYBACK_RAW_FILE_PATH	        "/mnt/sd0/playback_raw.wav"     //User audio data
#define PLAYBACK_PREPROC_FILE_PATH	    "/mnt/sd0/playback_preproc.wav" //after decoding, softvol, module processing

extern int audiobox_get_dev_frame_size(audio_dev_attr_t *fmt);
extern int audiobox_get_chn_frame_size(audio_chn_fmt_t *fmt);
static audio_chn_t audio_get_first_chn_by_priority(audio_dev_t dev, enum channel_priority priority);
static int audio_put_chn(audio_chn_t chn);
static audio_chn_t audio_check_active_chn(audio_dev_t dev);

void *audio_playback_preproc_server(void* arg) {

	audio_dev_t dev = (audio_dev_t)arg;
	audio_chn_t chn = NULL;
    codec_addr_t decode_addr;
	char *decode_buf = NULL;
	static int decode_size = 0;
	char *softvol_buf = NULL;
	static int softvol_size = 0;
	char *in_buf = NULL;
	int in_len = 0;
	char *out_buf = NULL;
	int out_len = 0;
	int frame_size = 0;
	struct fr_buf_info out_ref;
#ifdef AUDIOBOX_DEBUG
	struct timeval t, last_t;
	float time;
#endif
	int err;
	char ret = -1;
	int len = 0;
#ifdef PLAYBACK_PREPROC_FILE
	int playback_preproc_fd = -1;
	int playback_raw_fd = -1;
#endif

	if(!dev)
		pthread_exit((void *)&ret);

#ifdef PLAYBACK_PREPROC_FILE
	playback_preproc_fd = open(PLAYBACK_PREPROC_FILE_PATH, O_CREAT | O_WRONLY | O_APPEND, 0666);
	if (playback_preproc_fd < 0)
		return NULL;

	playback_raw_fd = open(PLAYBACK_RAW_FILE_PATH, O_CREAT | O_WRONLY | O_APPEND, 0666);
	if (playback_raw_fd < 0) {
		close(playback_preproc_fd);
		return NULL;
	}
#endif

#if 0
	pthread_mutex_lock(&(dev->preproc.mutex));
	dev->preproc.server_running = 1;
	pthread_cond_broadcast(&dev->preproc.cond);
	pthread_mutex_unlock(&(dev->preproc.mutex));
#endif

	fr_INITBUF(&out_ref, NULL);

	decode_size = audiobox_get_dev_frame_size(&dev->attr) + CODEC_EXTRA_SIZE(&dev->attr);
	decode_buf = (char *)malloc(decode_size);	//output buf: allocate a temporary buffer for output
	if(!decode_buf) {
		AUD_ERR("%s: not enough memeory for decode_buf\n", __func__);
		return NULL;
	}

	softvol_size = audiobox_get_dev_frame_size(&dev->attr);
	softvol_buf = (char *)malloc(softvol_size);	//output buf: allocate a temporary buffer for output
	if(!softvol_buf) {
		AUD_ERR("%s: not enough memeory for softvol_buf\n", __func__);
		free(decode_buf);
		return NULL;
	}

#if 0
	struct sched_param sched;
    pthread_setschedparam(dev->serv_id, SCHED_RR, &sched);
#endif


#ifdef AUDIOBOX_DEBUG
	memset(&t, 0, sizeof(t));
	gettimeofday(&last_t, NULL);
#endif
	AUD_ERR("Entering %s\n", __func__);

	while(1) {

		// need to stop preproc server
		if (dev->preproc.stop_server)
			goto done;

		chn = audio_get_first_chn_by_priority(dev, CHANNEL_FOREGROUND);
		if (!chn) {
			chn = audio_get_first_chn_by_priority(dev, CHANNEL_BACKGROUND);
			if (!chn) {
				AUD_DBG("Audio playback can't get chn, try again\n");
				usleep(5 * 1000);
				continue;
			}
		}

		// TODO: just in case, test it again
		err = fr_test_new_by_name(chn->chnname, &chn->ref);
		if (err <= 0) {
			AUD_ERR("%s playback chn fr have no data\n", chn->chnname);
			usleep(5 * 1000);
			continue;
		}

		err = fr_get_ref_by_name(chn->chnname, &chn->ref);
		if (err < 0) {
			AUD_ERR("%s playback chn fr have no data\n", chn->chnname);
			continue;
		}
		AUD_DBG("%s:in_ref.virt_addr = %p, in_ref.phys_addr = %p, in_ref.size = %d\n",  __func__,(void*) (chn->ref.virt_addr), (void*) (chn->ref.phys_addr), chn->ref.size);
		err = fr_get_buf_by_name(dev->devname, &out_ref);
		if (err < 0) {
			AUD_ERR("%s playback dev fr have no data\n", dev->devname);
			fr_put_ref(&chn->ref);
			continue;
		}
		AUD_DBG("%s: out_ref.virt_addr = %p, out_ref.phys_addr = %p, out_ref.size = %d\n", __func__, (void*)(out_ref.virt_addr), (void*)(out_ref.phys_addr), out_ref.size);

		// get the size of frame again in case audiobox_set_dev_attr() is called recently
		frame_size = audiobox_get_dev_frame_size(&dev->attr) + CODEC_EXTRA_SIZE(&dev->attr);

		// decode
		out_buf = in_buf = (char *)chn->ref.virt_addr;
		out_len = in_len = chn->ref.size;

		if(chn->codec_handle) {
			AUD_DBG("%s: try to decode\n", __func__);

			if(frame_size > decode_size) {
				decode_buf = realloc(decode_buf, frame_size);
				if (!decode_buf) {
					AUD_ERR("Playback Server realloc err\n");
					goto next_loop;
				}
				decode_size = frame_size;
			}

			out_buf = decode_buf;
			out_len = decode_size;

			// decoder input & output information
			decode_addr.in = in_buf;
			decode_addr.len_in = in_len;
			decode_addr.out = out_buf;
			decode_addr.len_out = out_len;

			len = codec_decode(chn->codec_handle, &decode_addr);

			AUD_DBG("Audio playback %d decoded, (%d %d) ref_size:%d\n", len, in_len, out_len, out_ref.size);

			if (len < 0) {
				AUD_ERR("Audio playback chn %s can't decode, try again\n", chn->chnname);
				goto next_loop;
			}
			else {
				out_len = len;
				out_ref.size = len;
			}
		}

		// softvol
		in_buf = out_buf;
		in_len = out_len;

		if(chn->softvol_handle && (chn->volume != PRESET_RESOLUTION || dev->volume_p != VOLUME_DEFAULT)) {

			AUD_DBG("%s: try to softvol\n", __func__);
			if(frame_size > softvol_size) {
				softvol_buf = realloc(softvol_buf, frame_size);
				if (!softvol_buf) {
					AUD_ERR("Playback Server realloc err\n");
					goto next_loop;
				}
				softvol_size = frame_size;
			}

			out_buf = softvol_buf;
			out_len = softvol_size;

			AUD_DBG("Set volume %d\n", chn->volume * dev->volume_p / VOLUME_DEFAULT);
			softvol_set_volume(chn->softvol_handle, chn->volume * dev->volume_p / VOLUME_DEFAULT);

			len = softvol_convert(chn->softvol_handle, out_buf, out_len, in_buf, in_len);
			if (len < 0) {
				AUD_ERR("Playback softvol %d err\n", chn->volume);
				goto next_loop;
			}
			else {
				out_len = len;
			}

		}

		// module processing
		in_buf = out_buf;
		in_len = out_len;
		out_buf = out_ref.virt_addr;
		out_len = out_ref.size;

		pthread_rwlock_rdlock(&dev->preproc.rwlock);
		if(dev->preproc.module) {

			AUD_DBG("%s: try to module processing\n", __func__);
			err = vcp7g_capture_process(&(dev->preproc.vcpobj), in_buf, out_buf, in_len);
			if (err < 0) {
				AUD_ERR("vcp capture process failed: %d\n", err);
				pthread_rwlock_unlock(&(dev->preproc.rwlock));
				goto next_loop;
			}
		}
		else {
			AUD_DBG("%s: just copy from in_buf to out_buf\n", __func__);
			memcpy(out_buf, in_buf, in_len < out_len ? in_len : out_len);
		}
		pthread_rwlock_unlock(&(dev->preproc.rwlock));


#ifdef PLAYBACK_PREPROC_FILE
		if (write(playback_raw_fd, chn->ref.virt_addr, chn->ref.size) != chn->ref.size) {
			AUD_ERR("Audio Save playback Raw File err\n");
		}
		if (write(playback_preproc_fd, out_ref.virt_addr, out_ref.size) != out_ref.size) {
			AUD_ERR("Audio Save playback PREPROC File err\n");
		}
#endif

next_loop:
		audio_put_chn(chn);
		fr_put_ref(&chn->ref);
		fr_put_buf(&out_ref);


#ifdef AUDIOBOX_DEBUG
		gettimeofday(&t, NULL);
		time = (float)(t.tv_sec - last_t.tv_sec) + (((float)(t.tv_usec  - last_t.tv_usec))/1000000);
		AUD_DBG("%s: PREPROC_MODE interval between 2 process: %fs\n", __func__, time);
		memcpy(&last_t, &t, sizeof(t));
#endif //AUDIOBOX_DEBUG

		usleep(5 * 1000); //give a chance for other thread to change the state

	}

	if(decode_buf)
		free(decode_buf);
	if(softvol_buf)
		free(softvol_buf);
	
	//pthread_mutex_lock(&(dev->preproc.mutex));
#ifdef PLAYBACK_PREPROC_FILE
	if(playback_preproc_fd >= 0) {
		AUD_DBG("closing playback_preproc_fd\n");
		close(playback_preproc_fd);
	}
	if(playback_raw_fd >= 0)  {
		AUD_DBG("closing playback_raw_fd\n");
		close(playback_raw_fd);
	}
#endif

#if 0
	dev->preproc.server_running = 0;
	pthread_cond_broadcast(&dev->preproc.cond);
	pthread_mutex_unlock(&(dev->preproc.mutex));
#endif
done:

	AUD_ERR("exiting %s\n", __func__);
	pthread_exit((void *)&ret);
}

void *audio_capture_preproc_server(void* arg) {

	audio_dev_t dev = (audio_dev_t)arg;
	audio_chn_t chn = NULL;
    codec_addr_t encode_addr;
	char *module_buf = NULL;
	static int module_size = 0;
	char *softvol_buf = NULL;
	static int softvol_size = 0;
	char *encode_buf = NULL;
	static int encode_size = 0;
	char *dev_in_buf = NULL;
	int dev_in_len;
	char *dev_out_buf = NULL;
	int dev_out_len;
	char *in_buf = NULL;
	int in_len;
	char *out_buf = NULL;
	int out_len;
	int frame_size = 0;
	struct fr_buf_info in_ref;
	struct list_head *pos, *npos = NULL;
#ifdef AUDIOBOX_DEBUG
	struct timeval t, last_t;
	float time;
#endif
	int err;
	char ret = -1;
	int len = 0;
	//int first_enter = 8;
#ifdef CAPTURE_PREPROC_FILE
	int capture_preproc_fd = -1;
	int capture_raw_fd = -1;
#endif

	if(!dev)
		pthread_exit((void *)&ret);

#ifdef CAPTURE_PREPROC_FILE
	capture_preproc_fd = open(CAPTURE_PREPROC_FILE_PATH, O_CREAT | O_WRONLY | O_APPEND, 0666);
	if (capture_preproc_fd < 0)
		return NULL;

	capture_raw_fd = open(CAPTURE_RAW_FILE_PATH, O_CREAT | O_WRONLY | O_APPEND, 0666);
	if (capture_raw_fd < 0) {
		close(capture_preproc_fd);
		return NULL;
	}
#endif

#if 0
	pthread_mutex_lock(&(dev->preproc.mutex));
	dev->preproc.server_running = 1;
	pthread_cond_broadcast(&dev->preproc.cond);
	pthread_mutex_unlock(&(dev->preproc.mutex));
#endif

	// promote capture thread to RealTime priority
	struct sched_param sched;
    pthread_setschedparam(dev->preproc.serv_id, SCHED_RR, &sched);

	fr_INITBUF(&in_ref, NULL);

	module_size = audiobox_get_dev_frame_size(&dev->attr);
	module_buf = (char *)malloc(module_size);	//output buf: allocate a temporary buffer for module processing
	if(!module_buf) {
		AUD_ERR("%s: not enough memeory for module_buf\n", __func__);
		return NULL;
	}

	softvol_size = audiobox_get_dev_frame_size(&dev->attr);
	softvol_buf = (char *)malloc(softvol_size);	//output buf: allocate a temporary buffer for softvol
	if(!softvol_buf) {
		AUD_ERR("%s: not enough memeory for softvol_buf\n", __func__);
		free(module_buf);
		return NULL;
	}

	encode_size = audiobox_get_dev_frame_size(&dev->attr) + CODEC_EXTRA_SIZE(&dev->attr);
	encode_buf = (char *)malloc(encode_size);	//output buf: allocate a temporary buffer for output
	if(!encode_buf) {
		AUD_ERR("%s: not enough memeory for encode_buf\n", __func__);
		return NULL;
	}

#ifdef AUDIOBOX_DEBUG
	memset(&t, 0, sizeof(t));
	gettimeofday(&last_t, NULL);
#endif
	AUD_ERR("Entering %s\n", __func__);

	while(1) {

		// need to stop preproc server
		if (dev->preproc.stop_server) {
			AUD_DBG("Audio capture preproc stop server");
			goto done;
		}

		chn = audio_check_active_chn(dev);
		if (!chn) {
			AUD_DBG("Audio Capture can't get chn, try again\n");
			usleep(5 * 1000);
			continue;
		}
		
		err = fr_get_ref_by_name(dev->devname, &in_ref);
		if (err < 0) {
			AUD_DBG("%s no in_buf\n", dev->devname);
			continue;
		}
		AUD_DBG("%s:in_ref.virt_addr = %p, in_ref.phys_addr = %p, in_ref.size = %d\n",  __func__,(void*) (in_ref.virt_addr), (void*) (in_ref.phys_addr), in_ref.size);
		// get the size of frame again in case audiobox_set_dev_attr() is called recently
		frame_size = audiobox_get_dev_frame_size(&dev->attr);

		// module processing
		dev_out_buf = dev_in_buf = (char *)in_ref.virt_addr;
		dev_out_len = dev_in_len = in_ref.size;

		pthread_rwlock_rdlock(&dev->preproc.rwlock);
		if(dev->preproc.module) {

			AUD_DBG("%s: try to module processing\n", __func__);
			if(frame_size > module_size) {
				module_buf = realloc(module_buf, frame_size);
				if (!module_buf) {
					AUD_ERR("Playback Server realloc err\n");
				    pthread_rwlock_unlock(&(dev->preproc.rwlock));
					goto next_dev_loop;
				}
				module_size = frame_size;
			}

			dev_out_buf = module_buf;
			dev_out_len = module_size;

			err = vcp7g_capture_process(&(dev->preproc.vcpobj), dev_in_buf, dev_out_buf, dev_in_len);
			if (err < 0) {
				AUD_ERR("vcp capture process failed: %d\n", err);
				pthread_rwlock_unlock(&(dev->preproc.rwlock));
				goto next_dev_loop;
			}
		}
		pthread_rwlock_unlock(&(dev->preproc.rwlock));


		pthread_mutex_lock(&(dev->mutex));
		list_for_each_safe(pos, npos, &dev->head) {

			chn = list_entry(pos, struct audio_chn, node);
			if (!chn || !chn->active) {
				continue;
			}
			pthread_mutex_lock(&(chn->mutex));
			chn->in_used++;

			err = fr_get_buf_by_name(chn->chnname, &chn->ref);
			if (err < 0) {
				AUD_ERR("%s no out_ref\n", chn->chnname);
				continue;
			}
			AUD_DBG("%s: out_ref.virt_addr = %p, out_ref.phys_addr = %p, out_ref.size = %d\n", __func__, (void*)(chn->ref.virt_addr), (void*)(chn->ref.phys_addr), chn->ref.size);

			// softvol
			in_buf = out_buf = dev_out_buf;
			in_len = out_len = dev_out_len;

			#if 0
			if(chn->softvol_handle == NULL)
				AUD_ERR("%s: 1 to softvol\n", __func__);
			else if(dev->volume_p == VOLUME_DEFAULT)
				AUD_ERR("%s: 2 to softvol: %d, %d\n", __func__, dev->volume_p, dev->volume_c);
			#endif

			if(chn->softvol_handle && (chn->volume != PRESET_RESOLUTION || dev->volume_c != VOLUME_DEFAULT)) {

				AUD_DBG("%s: try to softvol\n", __func__);
				if(frame_size > softvol_size) {
					softvol_buf = realloc(softvol_buf, frame_size);
					if (!softvol_buf) {
						AUD_ERR("Capture Server realloc err\n");
						goto next_chn_loop;
					}
					softvol_size = frame_size;
				}

				out_buf = softvol_buf;
				out_len = softvol_size;

				AUD_DBG("Set volume %d\n", dev->volume_c * chn->volume / VOLUME_DEFAULT);
				softvol_set_volume(chn->softvol_handle, dev->volume_c * chn->volume / VOLUME_DEFAULT);

				len = softvol_convert(chn->softvol_handle, out_buf, out_len, in_buf, in_len);
				if (len < 0) {
					AUD_ERR("Capture softvol %d err\n", dev->volume_c);
					goto next_chn_loop;
				}
				else {
					out_len = len;
				}

			}

			// encode
			in_buf = out_buf;
			in_len = out_len;

			frame_size = audiobox_get_chn_frame_size(&chn->fmt) + CODEC_EXTRA_SIZE(&chn->fmt);
			if(chn->codec_handle) {

				AUD_DBG("%s: try to encoding\n", __func__);
				if(frame_size > encode_size) {
					encode_buf = realloc(encode_buf, frame_size);
					if (!encode_buf) {
						AUD_ERR("Playback Server realloc err\n");
						goto next_chn_loop;
					}
					encode_size = frame_size;
				}

				out_buf = encode_buf;
				out_len = encode_size;

				// encoder input & output information
				encode_addr.in = in_buf;
				encode_addr.len_in = in_len;
				encode_addr.out = out_buf;
				encode_addr.len_out = out_len;

				len = codec_encode(chn->codec_handle, &encode_addr);

				if (len < 0) {
					AUD_ERR("Audio capture chn %s can't encode, try again\n", chn->chnname);
					goto next_chn_loop;
				}
				else {
					out_len = len;
					chn->ref.size = len;
				}
			}

			// memory copy
			in_buf = out_buf;
			in_len = out_len;
			out_buf = chn->ref.virt_addr;
			out_len = chn->ref.size;

			AUD_DBG("%s: try to copy from encode_buf to chn->ref\n", __func__);
			memcpy(out_buf, in_buf, in_len < out_len ? in_len : out_len);

#ifdef CAPTURE_PREPROC_FILE
			if (write(capture_raw_fd, in_ref.virt_addr, in_ref.size) != in_ref.size) {
				AUD_ERR("Audio Save capture Raw File err\n");
			}
			if (write(capture_preproc_fd, chn->ref.virt_addr, chn->ref.size) != chn->ref.size) {
				AUD_ERR("Audio Save capture PREPROC File err\n");
			}
#endif

			//copy the timestamp
			chn->ref.timestamp = in_ref.timestamp;
next_chn_loop:
			chn->active = 0; //only for once
			chn->in_used--;
			fr_put_buf(&chn->ref);
			pthread_mutex_unlock(&(chn->mutex));
	    }
		pthread_mutex_unlock(&(dev->mutex));

next_dev_loop:
		fr_put_ref(&in_ref);


#ifdef AUDIOBOX_DEBUG
		gettimeofday(&t, NULL);
		time = (float)(t.tv_sec - last_t.tv_sec) + (((float)(t.tv_usec  - last_t.tv_usec))/1000000);
		AUD_DBG("%s: PREPROC_MODE interval between 2 process: %fs\n", __func__, time);
		memcpy(&last_t, &t, sizeof(t));
#endif //AUDIOBOX_DEBUG

		usleep(5 * 1000); //give a chance for other thread to change the state

	}

	if(module_buf)
		free(module_buf);
	if(softvol_buf)
		free(softvol_buf);
	if(encode_buf)
		free(encode_buf);
	
	//pthread_mutex_lock(&(dev->preproc.mutex));
//out_locked:
#ifdef CAPTURE_PREPROC_FILE
	if(capture_preproc_fd >= 0) {
		AUD_DBG("closing capture_preproc_fd\n");
		close(capture_preproc_fd);
	}
	if(capture_raw_fd >= 0)  {
		AUD_DBG("closing capture_raw_fd\n");
		close(capture_raw_fd);
	}
#endif
#if 0
	dev->preproc.server_running = 0;
	pthread_cond_broadcast(&dev->preproc.cond);
	//pthread_mutex_unlock(&(dev->preproc.mutex));
#endif
done:
	AUD_DBG("exiting %s\n", __func__);
	pthread_exit((void *)&ret);
}

static int audio_get_pbmode(audio_dev_t dev)
{
	return AUD_SINGLE_STREAM;
}

static int audio_put_chn(audio_chn_t chn)
{
	if(!chn)
		return -1;

	chn->in_used--;

	return 0;
}

static audio_chn_t audio_get_first_chn_by_priority(audio_dev_t dev, enum channel_priority priority)
{
	struct list_head *pos;
	audio_chn_t chn = NULL;

	if (!dev)
		return NULL;

	pthread_mutex_lock(&dev->mutex);
	list_for_each(pos, &dev->head) {
		chn = list_entry(pos, struct audio_chn, node);
		if (chn) {
			pthread_mutex_lock(&chn->mutex);
			if (chn->priority == priority) {
				if(!(chn->priority == CHANNEL_BACKGROUND && (fr_test_new_by_name(chn->chnname, &chn->ref) <= 0))) {
					chn->in_used++;
					pthread_mutex_unlock(&chn->mutex);
					pthread_mutex_unlock(&dev->mutex);
					AUD_DBG("found %s chn with %d available fr buffer\n",(chn->priority == CHANNEL_BACKGROUND ? "background" : "foregroudn"),  (fr_test_new_by_name(chn->chnname, &chn->ref)));
					return chn;
				}
			}
			pthread_mutex_unlock(&chn->mutex);
		}
	}
	pthread_mutex_unlock(&dev->mutex);
	return NULL;
}

static audio_chn_t audio_check_active_chn(audio_dev_t dev)
{
	struct list_head *pos;
	audio_chn_t chn = NULL;

	if (!dev)
		return NULL;

	pthread_mutex_lock(&dev->mutex);
	list_for_each(pos, &dev->head) {
		chn = list_entry(pos, struct audio_chn, node);
		if(chn) {
			pthread_mutex_lock(&chn->mutex);
			if (chn->active) {
				pthread_mutex_unlock(&chn->mutex);
				pthread_mutex_unlock(&dev->mutex);
				return chn;
			}
		}
		pthread_mutex_unlock(&chn->mutex);
	}
	pthread_mutex_unlock(&dev->mutex);
	return NULL;
}

#define SOFT_BUF    4096

void *audio_playback_server(void *arg)
{
	int err, ret;

#ifdef PLAYBACK_FILE
	int fd;
#endif
	audio_dev_t dev = (audio_dev_t)arg;
	if (!dev)
		return NULL;

#ifdef PLAYBACK_FILE
	fd = open(WRITE_FILE_PATH, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	if (fd < 0)
		return NULL;	
#endif

	// create playback preproc thread
	err = pthread_create(&dev->preproc.serv_id, NULL, audio_playback_preproc_server, dev);
	if (err != 0 || dev->preproc.serv_id <= 0) {
		AUD_ERR("Create AudioBox preproc server err\n");
		return NULL;
	}

#if 0
	struct sched_param sched;
    pthread_setschedparam(dev->serv_id, SCHED_RR, &sched);
#endif

	AUD_ERR("Entering Audio Playback Service\n");
	do {
		ret = audio_get_pbmode(dev);
		switch (ret) {
			case AUD_SINGLE_STREAM:
				if (dev->stop_server){
					goto done;
				}else{
					AUD_DBG("%s no ref\n", dev->devname);
					usleep(5 * 1000);
				}

				while(fr_test_new_by_name(dev->devname, &dev->ref) > 0){
					ret = fr_get_ref_by_name(dev->devname, &dev->ref);
					if (ret < 0) {
						AUD_DBG("%s no ref\n", dev->devname);
						break;
					}
					AUD_DBG("%s: audio_hal_write: virtaddr 0x%p, phyaddr 0x%p, size %d\n", dev->devname, (void *)dev->ref.virt_addr, (void *)dev->ref.phys_addr, dev->ref.size);
					ret = audio_hal_write(dev, dev->ref.virt_addr, dev->ref.size);
					if (ret < 0) {
						fr_put_ref(&dev->ref);
						AUD_ERR("Audio hal cannot write: %d, fatal error!\n", ret);
						break;
					}
#ifdef PLAYBACK_FILE
					if (write(fd, dev->ref.virt_addr, dev->ref.size) != dev->ref.size) {
						AUD_ERR("Audio Save File err\n");
					}
#endif
					fr_put_ref(&dev->ref);
				}				
				break;
			case AUD_MIXER_STREAM:
			case AUD_MIXER_VOL_STREAM:
			default:
				AUD_ERR("Now, Do not support this playback mode: %d\n", ret);	
				break;
		}
	} while (1);

done:
#ifdef PLAYBACK_FILE
	close(fd);
#endif	
	dev->preproc.stop_server = 1;
	pthread_join(dev->preproc.serv_id, NULL);

	AUD_ERR("Exit Audio Playback Service\n");
	return NULL;
}

void audio_update_timestamp(struct fr_buf_info *fr)
{
	fr_STAMPBUF(fr);
	return;
}

#define timestamp_ratio  5
#define KERNEL_AUD_TIMESTAMP 1

void *audio_capture_server(void *arg)
{
#ifdef CAPTURE_FILE
	int fd;
#endif

	audio_dev_t dev = (audio_dev_t)arg;
	struct fr_buf_info ref;
	int ret;
	int err;
	int i;
	uint16_t buf_time;

	if (!dev)
		return NULL;

#ifdef CAPTURE_FILE
	fd = open(READ_FILE_PATH, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	if (fd < 0)
		return NULL;	
#endif
	fr_INITBUF(&ref, NULL);
	audio_update_timestamp(&ref);

	// create capture preproc thread
	err = pthread_create(&dev->preproc.serv_id, NULL, audio_capture_preproc_server, dev);
	if (err != 0 || dev->preproc.serv_id <= 0) {
		AUD_ERR("Create AudioBox preproc server err\n");
		return NULL;
	}

	// promote capture thread to RealTime priority
	struct sched_param sched;
    pthread_setschedparam(dev->serv_id, SCHED_RR, &sched);
	
	AUD_ERR("Entering Audio Capture Service\n");
	do {

		// need to stop server
		if (dev->stop_server) {
			AUD_ERR("Capture Service stop\n");
			goto done;
		}

#if 0
		// running only when dev->head is not empty
		if (list_empty(&dev->head)) {
			usleep(5 * 1000);
			continue;
		}
#endif

		//pthread_mutex_lock(&dev->mutex);
		AUD_DBG("fr_get_buf_by_name: name %s\n", dev->devname);
		ret = fr_get_buf_by_name(dev->devname, &ref);
		if (ret < 0) {
			AUD_DBG("%s no ref\n", dev->devname);
			//pthread_mutex_unlock(&dev->mutex);
			usleep(5 * 1000);
			continue;
		}

		AUD_DBG("%s: audio_hal_read: virtaddr 0x%p, phyaddr 0x%p, size %d\n", dev->devname, (void *)ref.virt_addr, (void *)ref.phys_addr, ref.size);
		ret = audio_hal_read(dev, ref.virt_addr, ref.size);
		if (ret < 0) {
			AUD_ERR("Audio hal cannot read: %d, fatal error!\n", ret);
			//pthread_mutex_unlock(&dev->mutex);
            //audiobox_inner_release_channel(dev); // fatal error happen, stop the dev
			continue;
		}
#ifdef KERNEL_AUD_TIMESTAMP
		if (strcmp(dev->devname, "btcodecmic")) {
			ref.timestamp = 0;
			for (i = 0; i < 4; i++) {
				buf_time = *(uint16_t*)(ref.virt_addr + i*4);
				*(uint16_t*)(ref.virt_addr + i*4) = *(uint16_t*)((ref.virt_addr + i*4 + 2));
				ref.timestamp |= (buf_time << i*16);
			}
		} else {
			audio_update_timestamp(&ref);
		}
#else
		audio_update_timestamp(&ref); //add by Bright, 16/04/27
#endif
		fr_put_buf(&ref);
		//pthread_mutex_unlock(&dev->mutex);
		usleep(5 * 1000);
	} while (1);

done:
#ifdef CAPTURE_FILE
	close(fd);
#endif
	dev->preproc.stop_server = 1;
	pthread_join(dev->preproc.serv_id, NULL);
	AUD_ERR("Exit Audio Capture Service\n");
	return NULL;
}

typedef void *(*audio_server)(void *);
static const audio_server server[] = {
	audio_playback_server,
	audio_capture_server,
};

int audio_create_chnserv(audio_dev_t dev)
{
	int ret;
	
	if (!dev)
		return -1;
	
	dev->stop_server = 0;
	ret = pthread_create(&dev->serv_id, NULL, server[dev->direct], dev);
	if (ret != 0) {
		AUD_ERR("Create AudioBox server err\n");
		return -1;
	}

	//pthread_detach(dev->serv_id);
	return 0;
}

void audio_release_chnserv(audio_dev_t dev)
{
	if (!dev)
		return;

	dev->stop_server = 1;
	/*This seemed halting audiobox process for a while*/
	pthread_join(dev->serv_id, NULL);
}

