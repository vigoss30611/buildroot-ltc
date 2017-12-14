#include <fr/libfr.h>
#include <audiobox.h>
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

//#define PLAYBACK_FILE
#define WRITE_FILE_PATH		"/mnt/sd0/playback.wav"
//#define CAPTURE_FILE
#define READ_FILE_PATH		"/mnt/sd0/capture.wav"
//#define CAPTURE_AEC_FILE
#define AEC_RAW_FILE_PATH	"/mnt/sd0/aec_raw.wav"
#define AEC_FILE_PATH	    "/mnt/sd0/aec.wav"

extern int audiobox_get_buffer_size(audio_fmt_t *fmt);

void audiobox_aec_inner_start(void)
{
#if defined(AUDIOBOX_USE_AECV2)
    system("echo 1 > /sys/devices/platform/imapx-i2s.0/aecv2_ctrl");
    printf("==========> audibox-aec-start=============== \n");
#endif
}

void audiobox_aec_inner_stop(void)
{
#if defined(AUDIOBOX_USE_AECV2)
    system("echo 0 > /sys/devices/platform/imapx-i2s.0/aecv2_ctrl");
    printf("==========> audibox-aec-stop=============== \n");
#endif
}

int audiobox_cleanup_aec(channel_t chn)
{
	int err;
	struct timeval now;
	struct timespec timeout;

	if(!chn)
		return -1;

	if(chn->state == NORMAL_MODE)
		return 0;

	AUD_ERR("%s:Be Careful! have to clean aec!\n", __func__);
	chn->state = ENTERING_NORMAL_MODE;       //back to NORMAL MODE

	pthread_rwlock_wrlock(&(chn->aec.rwlock));
	if(!chn->aec.initiated) { //aec isn't initiated
		pthread_rwlock_unlock(&(chn->aec.rwlock));
		goto exit_thread;
	}

	if(chn->aec.fr.fr) {
		fr_free(&chn->aec.fr);
		chn->aec.fr.fr = NULL;
	}

	if(vcp7g_is_init(&chn->aec.vcpobj)) {
		vcp7g_deinit(&(chn->aec.vcpobj)); //it worth to deinit again even it have been done before
		memset(&(chn->aec.vcpobj), 0, sizeof(struct vcp_object));
	}

    audiobox_aec_inner_stop();
#if defined(AUDIOBOX_USE_AECV1)
	//normal status: L1AL=enable, R1AR=enable, DLAL=disable, L1AR=disable
	//err = audio_route_cset(chn, "name='Mic L1AL Enable Switch'", "on"); //not working
	err = audio_route_cset(chn, "numid=1", "on");
	//err |= audio_route_cset(chn, "name='Mic L2AL Enable Switch'", "on");//not working
	err |= audio_route_cset(chn, "numid=2", "on");

	//err |= audio_route_cset(chn, "name='Mic R1AR Enable Switch'", "on");//not working
	err |= audio_route_cset(chn, "numid=5", "on");
	//err |= audio_route_cset(chn, "name='Mic R2AR Enable Switch'", "on");//not working
	err |= audio_route_cset(chn, "numid=6", "on");

	//err |= audio_route_cset(chn, "name='Mic L1AR Enable Switch'", "off");//not working
	err |= audio_route_cset(chn, "numid=3", "off");
	//err |= audio_route_cset(chn, "name='Mic L2AR Enable Switch'", "off");//not working
	err |= audio_route_cset(chn, "numid=4", "off");

	//err |= audio_route_cset(chn, "name='Mic DLAL Enable Switch'", "off");//not working
	err |= audio_route_cset(chn, "numid=7", "off");
	if(err)
		AUD_ERR("back from AEC mode to NORMAL mode fail!\n"); //wrong again? ALSA or snd_card may broken down
#endif

	chn->aec.initiated = 0;
	pthread_rwlock_unlock(&(chn->aec.rwlock));

exit_thread:
	//exit thread
	pthread_mutex_lock(&(chn->aec.mutex));
	if(chn->aec.server_running) {
		//timeout = 1s
		gettimeofday(&now, NULL);
		timeout.tv_sec = now.tv_sec + 1;
		timeout.tv_nsec = (now.tv_usec * 1000);
		do {
			err = pthread_cond_timedwait(&chn->aec.cond, &chn->aec.mutex, &timeout);
		} while(chn->aec.server_running && err == 0);  // if timeout or server not running

		if(chn->aec.server_running) {
			AUD_ERR("waiting for server stoping timeout!\n");
		}
	}
	pthread_mutex_unlock(&(chn->aec.mutex));
	chn->state = NORMAL_MODE; //back to NORMAL MODE
	AUD_DBG("%s: change to NORMAL mode\n", __func__);
	return 0;
}

void *audio_aec_server(void* arg) {

	channel_t chn = (channel_t)arg;
	struct fr_buf_info in_ref, out_ref;
#ifdef AUDIOBOX_DEBUG
	struct timeval t, last_t;
	float time;
#endif
	int err;
	char ret = -1;
#ifdef CAPTURE_AEC_FILE
	int aec_fd = -1;
	int aec_raw_fd = -1;
#endif

	if(!chn)
		pthread_exit((void *)&ret);

#ifdef CAPTURE_AEC_FILE
	//aec_fd = open(AEC_FILE_PATH, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	aec_fd = open(AEC_FILE_PATH, O_CREAT | O_WRONLY | O_APPEND, 0666);
	if (aec_fd < 0)
		return NULL;

	//aec_raw_fd = open(AEC_RAW_FILE_PATH, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	aec_raw_fd = open(AEC_RAW_FILE_PATH, O_CREAT | O_WRONLY | O_APPEND, 0666);
	if (aec_raw_fd < 0) {
		close(aec_fd);
		return NULL;
	}
#endif

	pthread_mutex_lock(&(chn->aec.mutex));
	chn->aec.server_running = 1;
	pthread_cond_broadcast(&chn->aec.cond);
	pthread_mutex_unlock(&(chn->aec.mutex));

	fr_INITBUF(&in_ref, NULL);
	fr_INITBUF(&out_ref, NULL);

#ifdef AUDIOBOX_DEBUG
	memset(&t, 0, sizeof(t));
	gettimeofday(&last_t, NULL);
#endif
	AUD_DBG("Entering %s\n", __func__);

	while(1) {

		pthread_rwlock_rdlock(&chn->aec.rwlock);
		if(chn->state == AEC_MODE) {
			if(!chn->aec.initiated) {
				pthread_rwlock_unlock(&chn->aec.rwlock);
				continue;
			}

			err = fr_get_buf_by_name(chn->nodename, &out_ref);
			if (err < 0) {
				AUD_DBG("%s no out_buf\n", chn->nodename);
				goto check_state;
			}

			err = fr_get_ref_by_name(chn->aec.name, &in_ref);
			if (err < 0) {
				AUD_DBG("%s no in_ref\n", chn->aec.name);
				fr_put_buf(&out_ref);
				goto check_state;
			}

			AUD_DBG("%s:in_ref.virt_addr = %p, in_ref.phys_addr = %p, in_ref.size = %d\n",  __func__,(void*) (in_ref.virt_addr), (void*) (in_ref.phys_addr), in_ref.size);
			AUD_DBG("%s: out_ref.virt_addr = %p, out_ref.phys_addr = %p, out_ref.size = %d\n", __func__, (void*)(out_ref.virt_addr), (void*)(out_ref.phys_addr), out_ref.size);

			err = vcp7g_capture_process(&(chn->aec.vcpobj), in_ref.virt_addr, out_ref.virt_addr, in_ref.size);
			if (err < 0) {
				AUD_ERR("vcp capture process failed: %d\n", err);
			}
#ifdef CAPTURE_AEC_FILE
			if (write(aec_raw_fd, in_ref.virt_addr, in_ref.size) != in_ref.size) {
				AUD_ERR("Audio Save AEC Raw File err\n");
			}
			if (write(aec_fd, out_ref.virt_addr, out_ref.size) != out_ref.size) {
				AUD_ERR("Audio Save AEC File err\n");
			}
#endif

			out_ref.timestamp = in_ref.timestamp;
			fr_put_ref(&in_ref);
			fr_put_buf(&out_ref);
		}
check_state:
		pthread_rwlock_unlock(&chn->aec.rwlock);

		pthread_mutex_lock(&(chn->aec.mutex));
		if(chn->state == ENTERING_NORMAL_MODE || chn->state == NORMAL_MODE) {
			ret = 0;
			goto out_locked;
		}
		pthread_mutex_unlock(&(chn->aec.mutex));

#ifdef AUDIOBOX_DEBUG
		gettimeofday(&t, NULL);
		time = (float)(t.tv_sec - last_t.tv_sec) + (((float)(t.tv_usec  - last_t.tv_usec))/1000000);
		AUD_DBG("%s: AEC_MODE interval between 2 aec process: %fs\n", __func__, time);
		memcpy(&last_t, &t, sizeof(t));
#endif //AUDIOBOX_DEBUG

		usleep(5 * 1000); //give a chance for other thread to change the state

	}

	pthread_mutex_lock(&(chn->aec.mutex));
out_locked:
#ifdef CAPTURE_AEC_FILE
	if(aec_fd >= 0) {
		AUD_DBG("closing aec_fd\n");
		close(aec_fd);
	}
	if(aec_raw_fd >= 0)  {
		AUD_DBG("closing aec_raw_fd\n");
		close(aec_raw_fd);
	}
#endif
	chn->aec.server_running = 0;
	pthread_cond_broadcast(&chn->aec.cond);
	pthread_mutex_unlock(&(chn->aec.mutex));
	AUD_DBG("exiting %s\n", __func__);
	pthread_exit((void *)&ret);
}

static int audio_get_pbmode(channel_t chn)
{
	return AUD_SINGLE_STREAM;
}

static audio_t audio_get_logchn(channel_t chn)
{
	struct list_head *pos;
	audio_t dev = NULL;

	if (!chn)
		return NULL;

	list_for_each(pos, &chn->head) {
		dev = list_entry(pos, struct audio_dev, node);
		if (dev) {
			//AUD_DBG("fr_test_new_by_name: %d\n", num);
			return dev;
		}
	}
	return NULL;
}

static audio_t audio_get_logchn_by_priority(channel_t chn, enum channel_priority priority)
{
	struct list_head *pos;
	audio_t dev = NULL;

	if (!chn)
		return NULL;

	list_for_each(pos, &chn->head) {
		dev = list_entry(pos, struct audio_dev, node);
		if (dev && (dev->priority == priority)) {
			//AUD_DBG("fr_test_new_by_name: %d\n", num);
			if(dev->priority == CHANNEL_BACKGROUND && (fr_test_new_by_name(dev->devname, &dev->ref) <= 0))
				continue;
			return dev;
		}
	}
	return NULL;
}

#define SOFT_BUF    4096

void *audio_playback_server(void *arg)
{
	channel_t chn = (channel_t)arg;
	struct list_head *pos;
	audio_t dev = NULL;
	int ret;
	int note = 0, time_out = 0;
	char *buf = NULL;
	int bufsize = 0, need = 0, len = 0;
#ifdef PLAYBACK_FILE
	int fd;
#endif

	if (!chn)
		return NULL;
	/* for softvol */
	buf = malloc(SOFT_BUF);
	if (!buf)
		return NULL;
	bufsize = SOFT_BUF;

#ifdef PLAYBACK_FILE
	fd = open(WRITE_FILE_PATH, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	if (fd < 0)
		return NULL;	
#endif
	AUD_DBG("Enter Audio Playback Service\n");
	do {
		ret = audio_get_pbmode(chn);
		switch (ret) {
			case AUD_SINGLE_STREAM:

				list_for_each(pos, &chn->head) {
					dev = list_entry(pos, struct audio_dev, node);
					if (dev && dev->req_stop && (fr_test_new_by_name(dev->devname, &dev->ref) <= 0))
						audiobox_inner_put_channel(dev);
				}

				/* wait fr buf empty */
				if (chn->stop_server)
					goto done;

				dev = audio_get_logchn_by_priority(chn, CHANNEL_FOREGROUND);
				if (!dev) {
					dev = audio_get_logchn_by_priority(chn, CHANNEL_BACKGROUND);
					if (!dev) {
						AUD_DBG("Audio playback can't get dev, try again\n");
						usleep(5 * 1000);
						continue;
					}
				}

				ret = fr_test_new_by_name(dev->devname, &dev->ref);
				if (ret <= 0) {
					AUD_DBG("Audio playback can't get ref, try again\n");
					usleep(5000);
					if (note == 0)
						time_out ++;
					if ((dev->timeout != 0) && (time_out > (dev->timeout*1000/5))){ //default: <PLAYBACK_TIMEOUT> seconds
						AUD_ERR("%ds elapsed to get data from productor, kill this comsumer\n", dev->timeout);
						audiobox_inner_release_channel(dev); // long time no data, stop the dev
					}
					note = ret;
					continue;
				}
				note = ret;
				time_out = 0;

				if (pthread_mutex_trylock(&dev->mutex)) {
					AUD_DBG("Audio playback can not lock, try again\n");
					continue;
				}

				ret = fr_get_ref_by_name(dev->devname, &dev->ref);
				if (ret < 0) {
					AUD_DBG("%s no ref\n", dev->devname);
					pthread_mutex_unlock(&dev->mutex);
					usleep(5 * 1000);
					continue;
				}
				if (dev->volume != PRESET_RESOLUTION || chn->volume_p != VOLUME_DEFAULT) {
					if (bufsize < dev->ref.size) {
						buf = realloc(buf, dev->ref.size);
						if (!buf) {
							AUD_ERR("Playback Server realloc err\n");
							pthread_mutex_unlock(&dev->mutex);
							need = 0;
							continue;
						}
						bufsize = dev->ref.size;
					}
					AUD_DBG("Set volume %d\n", dev->volume * chn->volume_p / VOLUME_DEFAULT);
					softvol_set_volume(dev->handle, dev->volume * chn->volume_p / VOLUME_DEFAULT);

					len = softvol_convert(dev->handle, buf, bufsize, dev->ref.virt_addr, dev->ref.size);
					if (len < 0) {
						AUD_ERR("Playback softvol %d err\n", dev->volume);
						pthread_mutex_unlock(&dev->mutex);
						need = 0;
						continue;
					}
					need = 1;
				}
#if 0
				if(chn->state == AEC_MODE) {

					//AUD_DBG("%s: chn->state = AEC\n", __func__);
					if(!chn->aec.buf || chn->aec.buf_size == 0) {
						AUD_ERR("AEC need a temporary buffer\n");
						goto done;
					}

					if (ret < 0) {
						AUD_ERR("Audio hal cannot read: %d\n", ret);
						goto done;
					}

					ret = vcp7g_playback_process(&(chn->aec.vcpobj), (need ? buf : dev->ref.virt_addr), chn->aec.buf, (need ? len : dev->ref.size));
					if (ret < 0) {
						AUD_ERR("vcp playback process failed: %d\n", ret);
						goto done;
					}
					ret = audio_hal_write(chn, chn->aec.buf,
									(need ? len : dev->ref.size));

					if (ret < 0) {
						AUD_ERR("Audio hal cannot write: %d\n", ret);
						pthread_mutex_unlock(&dev->mutex);
						goto done;
					}
				}
				else {
					//AUD_ERR("%s: phyaddr 0x%x, size %d\n", dev->devname, dev->ref.phys_addr, dev->ref.size);
					ret = audio_hal_write(chn, (need ? buf : dev->ref.virt_addr), (need ? len : dev->ref.size));
				}
#else
				AUD_DBG("%s: audio_hal_write: virtaddr 0x%p, phyaddr 0x%p, size %d\n", dev->devname, (void *)dev->ref.virt_addr, (void *)dev->ref.phys_addr, dev->ref.size);
				pthread_mutex_lock(&chn->mutex);
				ret = audio_hal_write(chn, (need ? buf : dev->ref.virt_addr), (need ? len : dev->ref.size));
				pthread_mutex_unlock(&chn->mutex);
#endif
				if (ret < 0) {
					AUD_ERR("Audio hal cannot write: %d, fatal error!\n", ret);
					pthread_mutex_unlock(&dev->mutex);
					audiobox_inner_release_channel(dev); // fatal error happen, stop the dev
					continue;
				}

#ifdef PLAYBACK_FILE
				if (write(fd, (need ? buf : dev->ref.virt_addr),
							(need ? len : dev->ref.size)) != dev->ref.size) {
					AUD_ERR("Audio Save File err\n");
				}
#endif
				fr_put_ref(&dev->ref);

				pthread_mutex_unlock(&dev->mutex);
#if 1 // for permit overrun
				usleep(5 * 1000);
#endif
				need = 0;
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
	if (chn)
		free(chn);

	if (buf)
		free(buf);
	AUD_DBG("Exit Audio Playback Service\n");
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
	channel_t chn = (channel_t)arg;
	struct list_head *pos;
	audio_t dev = NULL;
	struct fr_buf_info ref;
	//uint64_t time0 = 0, subtime, ptime;
	uint16_t buf_time;
	int ret, i;
	char *buf = NULL;
	int bufsize = 0, len = 0;
#ifdef CAPTURE_FILE
	int fd;
#endif

	if (!chn)
		return NULL;
	/* for softvol */
	buf = malloc(SOFT_BUF);
	if (!buf)
		return NULL;
	bufsize = SOFT_BUF;
#ifdef CAPTURE_FILE
	fd = open(READ_FILE_PATH, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	if (fd < 0)
		return NULL;	
#endif
	fr_INITBUF(&ref, NULL);
	audio_update_timestamp(&ref);
	AUD_DBG("Enter Audio Capture Service\n");
	do {

		list_for_each(pos, &chn->head) {
			dev = list_entry(pos, struct audio_dev, node);
			if (dev && dev->req_stop)
				audiobox_inner_put_channel(dev);
		}

		if (chn->stop_server)
			goto done;

		dev = audio_get_logchn(chn);
		if (!dev) {
			AUD_DBG("Audio capture can't get dev, try again\n");
			usleep(5 * 1000);
			continue;
		}

		pthread_mutex_lock(&chn->mutex);
		if(chn->state == NORMAL_MODE) {

			AUD_DBG("NORMAL_MODE : fr_get_buf_by_name: name %s\n", chn->nodename);
			ret = fr_get_buf_by_name(chn->nodename, &ref);
			if (ret < 0) {
				AUD_DBG("%s no ref\n", chn->nodename);
				pthread_mutex_unlock(&chn->mutex);
				usleep(5 * 1000);
				continue;
			}

			if (dev->volume != PRESET_RESOLUTION || chn->volume_c != VOLUME_DEFAULT) {
				if (bufsize < ref.size) {
					buf = realloc(buf, ref.size);
					if (!buf) {

						AUD_ERR("Capture Server realloc err\n");
						fr_put_buf(&ref);
						pthread_mutex_unlock(&chn->mutex);
						continue;
					}
					bufsize = ref.size;
				}

				AUD_DBG("%s: NORMAL_MODE(soft volume): audio_hal_read: addr_virt %p, addr_phy %p, len %d\n", __func__, ref.virt_addr,(void*) (ref.phys_addr), ref.size);
				ret = audio_hal_read(chn, buf, bufsize);
				if (ret < 0) {
					AUD_ERR("Audio hal cannot read: %d, fatal error!\n", ret);
					pthread_mutex_unlock(&chn->mutex);
					audiobox_inner_release_channel(dev); // fatal error happen, stop the dev
					continue;
				}

#ifdef KERNEL_AUD_TIMESTAMP
				if (strcmp(chn->nodename, "btcodecmic")) {
					ref.timestamp = 0;
					for (i = 0; i < 4; i++) {
						buf_time = *(uint16_t*)(buf + i*4);
						*(uint16_t*)(buf + i*4) = *(uint16_t*)((buf + i*4 + 2));
						ref.timestamp |= (buf_time << i*16);
					}
				} else {
					audio_update_timestamp(&ref);
				}
#else
				audio_update_timestamp(&ref); //add by Bright, 16/04/27
#endif

				AUD_DBG("dev->volume=%d, chn->volume=%d\n", dev->volume, chn->volume_c);
				softvol_set_volume(dev->handle, dev->volume * chn->volume_c / VOLUME_DEFAULT);

				len = softvol_convert(dev->handle, ref.virt_addr, ref.size, buf, bufsize);
				if (len < 0) {
					AUD_ERR("Capture softvol %d err\n", dev->volume);
					fr_put_buf(&ref);
					pthread_mutex_unlock(&chn->mutex);
					continue;
				}
			} else {
				AUD_DBG("%s: NORMAL_MODE: audio_hal_read: addr_virt %p, addr_phy %p, len %d\n", __func__, ref.virt_addr,(void*) (ref.phys_addr), ref.size);
				ret = audio_hal_read(chn, ref.virt_addr, ref.size);
				if (ret < 0) {
					AUD_ERR("Audio hal cannot read: %d, fatal error!\n", ret);
					pthread_mutex_unlock(&chn->mutex);
					audiobox_inner_release_channel(dev); // fatal error happen, stop the dev
					continue;
				}
#ifdef KERNEL_AUD_TIMESTAMP
				if (strcmp(chn->nodename, "btcodecmic")) {
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
			}

#ifdef CAPTURE_FILE
			if (write(fd, ref.virt_addr, ref.size) != ref.size) {
				AUD_ERR("Audio Save File err\n");
			}
#endif

			fr_put_buf(&ref);
		}

		pthread_rwlock_rdlock(&chn->aec.rwlock);
		if(chn->state == AEC_MODE) {

			AUD_DBG("%s:AEC_MODE : fr_get_buf_by_name: name %s\n", __func__, chn->aec.name);
			ret = fr_get_buf_by_name(chn->aec.name, &ref);
			if (ret < 0) {
				AUD_DBG("%s no ref\n", chn->aec.name);
				pthread_rwlock_unlock(&chn->aec.rwlock);
				pthread_mutex_unlock(&chn->mutex);
				usleep(5 * 1000);
				continue;
			}

			if (dev->volume != PRESET_RESOLUTION || chn->volume_c != VOLUME_DEFAULT) {
				if (bufsize < ref.size) {
					buf = realloc(buf, ref.size);
					if (!buf) {

						AUD_ERR("Capture Server realloc err\n");
						fr_put_buf(&ref);
						pthread_rwlock_unlock(&chn->aec.rwlock);
						pthread_mutex_unlock(&chn->mutex);
						continue;
					}
					bufsize = ref.size;
				}

				AUD_DBG("%s: AEC_MODE(soft volume): audio_hal_read: addr_virt %p, addr_phys %p,len %d\n", __func__, ref.virt_addr, (void*)(ref.phys_addr), ref.size);
				ret = audio_hal_read(chn, buf, bufsize);
				if (ret < 0) {
					AUD_ERR("Audio hal cannot read: %d, fatal error!\n", ret);
					pthread_rwlock_unlock(&chn->aec.rwlock);
					pthread_mutex_unlock(&chn->mutex);
					audiobox_inner_release_channel(dev); // fatal error happen, stop the dev
					continue;
				}
#ifdef KERNEL_AUD_TIMESTAMP
				ref.timestamp = 0;
				for (i = 0; i < 4; i++) {
					buf_time = *(uint16_t*)(buf + i*4);
					*(uint16_t*)(buf + i*4) = *(uint16_t*)((buf + i*4 + 2));
					ref.timestamp |= (buf_time << i*16);
				}
#else
				audio_update_timestamp(&ref); //add by Bright, 16/04/27
#endif

				AUD_DBG("dev->volume=%d, chn->volume=%d\n", dev->volume, chn->volume_c);
				softvol_set_volume(dev->handle, dev->volume * chn->volume_c / VOLUME_DEFAULT);

				len = softvol_convert(dev->handle, ref.virt_addr, ref.size, buf, bufsize);
				if (len < 0) {
					AUD_ERR("Capture softvol %d err\n", dev->volume);
					fr_put_buf(&ref);
					pthread_rwlock_unlock(&chn->aec.rwlock);
					pthread_mutex_unlock(&chn->mutex);
					continue;
				}
			} else {
				AUD_DBG("%s: AEC_MODE: audio_hal_read: addr_virt %p, addr_phys %p,len %d\n", __func__, ref.virt_addr, (void*)(ref.phys_addr), ref.size);
				ret = audio_hal_read(chn, ref.virt_addr, ref.size);
				if (ret < 0) {
					AUD_ERR("Audio hal cannot read: %d, fatal error!\n", ret);
					pthread_rwlock_unlock(&chn->aec.rwlock);
					pthread_mutex_unlock(&chn->mutex);
					audiobox_inner_release_channel(dev); // fatal error happen, stop the dev
					continue;
				}
#ifdef KERNEL_AUD_TIMESTAMP
				ref.timestamp = 0;
				for (i = 0; i < 4; i++) {
					buf_time = *(uint16_t*)(ref.virt_addr + i*4);
					*(uint16_t*)(ref.virt_addr + i*4) = *(uint16_t*)((ref.virt_addr + i*4 + 2));
					ref.timestamp |= (buf_time << i*16);
				}
#else
				audio_update_timestamp(&ref); //add by Bright, 16/04/27
#endif
			}

			fr_put_buf(&ref);
		}
		pthread_rwlock_unlock(&chn->aec.rwlock);
		pthread_mutex_unlock(&chn->mutex);
		usleep(5 * 1000);

		/* For alsa read data time inaccurately */
		/*
		uint64_t temp = ref.timestamp;
		if (!time0)
			time0 = ref.timestamp;
		else {
			ptime = audio_frame_times(dev, ref.size);

			time0 += ptime;
			subtime = time0 - ref.timestamp;

			if (subtime > (ptime / timestamp_ratio)
			 || subtime < (-ptime / timestamp_ratio))
				ref.timestamp = time0;
		}
		printf("audiobox: %8lld  %8lld\n", ref.timestamp, temp);
		fr_put_buf(&ref);
		*/

	} while (1);

done:
#ifdef CAPTURE_FILE
	close(fd);
#endif
	if (chn)
		free(chn);

	if (buf)
		free(buf);
	AUD_DBG("Exit Audio Capture Service\n");
	return NULL;
}

typedef void *(*audio_server)(void *);
static const audio_server server[] = {
	audio_playback_server,
	audio_capture_server,
};

int audio_create_chnserv(channel_t dev)
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

	pthread_detach(dev->serv_id);
	return 0;
}

void audio_release_chnserv(channel_t dev)
{
	if (!dev)
		return;
	
	dev->stop_server = 1;
	//pthread_join(dev->serv_id, NULL);
}

