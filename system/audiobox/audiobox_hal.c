#include <alsa/asoundlib.h>
#include <list.h>
#include <malloc.h>
#include <fr/libfr.h>
#include <audiobox.h>
#include <base.h>
#include <stdio.h>
#include <log.h>
#include <sys/time.h>
#include <audio_hal.h>

static unsigned int format_to_alsa(int format)
{
	switch (format) {
		case 32:
			return SND_PCM_FORMAT_S32_LE;
		case 24:
			return SND_PCM_FORMAT_S24_LE;
		case 16:
		default:
			return SND_PCM_FORMAT_S16_LE;
	}
}

static unsigned int alsa_to_format(int alsa_format)
{
	switch (alsa_format) {
		case SND_PCM_FORMAT_S32_LE:
			return 32;
		case SND_PCM_FORMAT_S24_LE:
			return 24;
		case SND_PCM_FORMAT_S16_LE:
		default:
			return 16;
	}
}

static int direct_to_alsa(int direct)
{
	switch (direct) {
		case DEVICE_OUT_ALL:
			return SND_PCM_STREAM_PLAYBACK;
		case DEVICE_IN_ALL:
		default:
			return SND_PCM_STREAM_CAPTURE;
	}
}

static int format_to_byte(int format)
{
	switch (format) {
		case 32:
		case 24:
			return 4;
		case 16:
		default:
			return 2;
	}
}
static int buflen_to_frames(channel_t handle, int len)
{
	audio_fmt_t *fmt;

	if (!handle)
		return -1;

	fmt = &(handle->fmt);
	return (len / (fmt->channels * format_to_byte(fmt->bitwidth)));
}

#define THR_NUM	2
threshold_t set[THR_NUM] = {
	{ 
		.sample_rate = 48000,
		.bitwidth = 32,
		.channels = 2,
		.value = 1,
	},	
	{
		.sample_rate = 8000,
		.bitwidth = 16,
		.channels = 1,
		.value = 2,
	}
};

static int get_threshold_val(audio_fmt_t *fmt)
{
	int i;

	if (!fmt)
		return 1;
	
	for (i = 0; i < THR_NUM; i++)
		if (set[i].sample_rate == fmt->samplingrate &&
			set[i].bitwidth == fmt->bitwidth &&
			set[i].channels == fmt->channels)
			return set[i].value;

	return 1;
}

static int setsoftwareparams(channel_t handle, audio_fmt_t *fmt)
{
	snd_pcm_sw_params_t * softwareParams;
	audio_fmt_t *format = &(handle->fmt);
	snd_pcm_uframes_t buffersize = 0;
	snd_pcm_uframes_t periodsize = 0;
	snd_pcm_uframes_t startThreshold, stopThreshold;
#ifdef AUDIOBOX_DEBUG
	static snd_output_t *log;
#endif
	int err;

	if (fmt)
		format = fmt;
#ifdef AUDIOBOX_DEBUG
	err = snd_output_stdio_attach(&log, stderr, 0);
#endif
	if (snd_pcm_sw_params_malloc(&softwareParams) < 0) {
	    AUD_ERR("Failed to allocate ALSA software parameters!\n");
	    return -1;
	}

	err = snd_pcm_sw_params_current(handle->handle, softwareParams);
	if (err < 0) {
	    AUD_ERR("Unable to get software parameters: %s\n", snd_strerror(err));
	    goto done;
	}

	snd_pcm_get_params(handle->handle, &buffersize, &periodsize);
	if (handle->direct == DEVICE_OUT_ALL) {
	    startThreshold = buffersize / get_threshold_val(format) - 1;
	    stopThreshold = buffersize;
	} else {
	    startThreshold = 1;
	    stopThreshold = buffersize;
	}

	err = snd_pcm_sw_params_set_start_threshold(handle->handle, softwareParams,
		startThreshold);
	if (err < 0) {
	    AUD_ERR("Unable to set start threshold to %lu frames: %s\n",
		    startThreshold, snd_strerror(err));
	    goto done;
	}

	err = snd_pcm_sw_params_set_stop_threshold(handle->handle, softwareParams,
		stopThreshold);
	if (err < 0) {
	    AUD_ERR("Unable to set stop threshold to %lu frames: %s\n",
		    stopThreshold, snd_strerror(err));
	    goto done;
	}

#if defined(AUDIOBOX_USE_AECV2)
	periodsize = 1024;
#endif
	err = snd_pcm_sw_params_set_avail_min(handle->handle, softwareParams, periodsize);
	if (err < 0) {
	    AUD_ERR("Unable to configure available minimum to %lu: %s\n",
		    periodsize, snd_strerror(err));
	    goto done;
	}

	// Commit the software parameters back to the device.
	err = snd_pcm_sw_params(handle->handle, softwareParams);
	if (err < 0) 
		AUD_ERR("Unable to configure software parameters: %s\n",
			snd_strerror(err));
#ifdef AUDIOBOX_DEBUG	
	snd_pcm_dump(handle->handle, log);
#endif
done:
	snd_pcm_sw_params_free(softwareParams);

	return err;
}

static int sethardwareparams(channel_t handle, audio_fmt_t *fmt)
{
	snd_pcm_hw_params_t *hardwareParams = NULL;
	audio_fmt_t *format = &(handle->fmt);
	snd_pcm_uframes_t buffersize, periodsize;
	unsigned int requestedrate;
	int err;
	
	if (fmt)
		format = fmt;

	err = snd_pcm_hw_params_malloc(&hardwareParams);
	if (err < 0) {
		AUD_ERR("Failed to allocate ALSA hardware parameters!\n");
		return -1;
	}

	err = snd_pcm_hw_params_any(handle->handle, hardwareParams);
	if (err < 0) {
		AUD_ERR("Unable to configure hardware: %s\n", snd_strerror(err));
		goto done;
	}
	
	err = snd_pcm_hw_params_set_access(handle->handle, hardwareParams,
			SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0) {
		AUD_ERR("Unable to configure PCM read/write format: %s\n",
				snd_strerror(err));
		goto done;
	}

	err = snd_pcm_hw_params_set_format(handle->handle, hardwareParams,
			format_to_alsa(format->bitwidth));
	if (err < 0) {
		AUD_ERR("Unable to configure PCM format %d : %s\n",
				format->bitwidth, snd_strerror(err));
		goto done;
	}

	err = snd_pcm_hw_params_set_channels(handle->handle, hardwareParams,
			format->channels);
	if (err < 0) {
		AUD_ERR("Unable to set channel count to %i: %s\n",
				format->channels, snd_strerror(err));
		goto done;
	}
	
	requestedrate = format->samplingrate;
	err = snd_pcm_hw_params_set_rate_near(handle->handle, hardwareParams,
			&requestedrate, 0);
	if (requestedrate != format->samplingrate) {
		AUD_ERR("Rate doesn't match (requested %iHz, get %iHz)\n",
				format->samplingrate, requestedrate);
		goto done;
	}
	
	periodsize = format->sample_size;	
#if defined(AUDIOBOX_USE_AECV2)
	periodsize = 128;
#endif
	err = snd_pcm_hw_params_set_period_size_near(handle->handle,
		hardwareParams, &periodsize, 0);
	if (err < 0) {
		AUD_ERR("Unable to set period_size %ld for playback: %s\n",
				periodsize, snd_strerror(err));
		goto done;
	}	
	
	buffersize = 4 * periodsize;
#if defined(AUDIOBOX_USE_AECV2)
	buffersize = 4096;
#endif
	err = snd_pcm_hw_params_set_buffer_size_near(handle->handle,
			hardwareParams, &buffersize);
	if (err < 0) {
		AUD_ERR("Unable to set buffer_size %ld for playback: %s\n",
				buffersize, snd_strerror(err));
		goto done;
	}

	err = snd_pcm_hw_params(handle->handle, hardwareParams);
	if (err < 0)
		AUD_ERR("Unable to set hardware parameters: %s\n", snd_strerror(err));

	AUD_DBG("AUDIO : channel %s\n", handle->nodename);
	snd_pcm_hw_params_get_period_size(hardwareParams, &periodsize, 0);
	snd_pcm_hw_params_get_buffer_size(hardwareParams, &buffersize);
	AUD_DBG("AUDIO : buffersize = %ld; periodsize = %ld\n", buffersize, periodsize);

done:
	snd_pcm_hw_params_free(hardwareParams);
	return err;
}

int audio_hal_open(channel_t handle)
{
	int err = -1;

	if (!handle || !handle->nodename)
		return -1;

	/* Open PCM device for record. */
	err = snd_pcm_open(&(handle->handle), handle->nodename,
			direct_to_alsa(handle->direct), 0);
	if (err < 0) {
		AUD_ERR("Unable to open pcm device: %s\n", snd_strerror(err));
		return -1;
	}

	err = sethardwareparams(handle, NULL);
	if (err < 0) {
		AUD_ERR("Audio set hardware params err\n");	
		return -1;
	}

	err = setsoftwareparams(handle, NULL);
	if (err < 0) {
		AUD_ERR("Audio set software params err\n");
		return -1;
	}

	AUD_DBG("Audio Init Ok\n");
	return 0;
}

int audio_hal_set_format(channel_t handle, audio_fmt_t *fmt)
{
	int err;

	if (!handle || !fmt)
		return -1;

	err = snd_pcm_drop(handle->handle);
	if (err < 0) {
		AUD_ERR("Failed to drop ALSA!\n");
		return -1;
	}

	err = sethardwareparams(handle, fmt);
	if (err < 0) {
		AUD_ERR("Audio set hardware format err\n");
		return -1;
	}

	err = setsoftwareparams(handle, fmt);
	if (err < 0) {
		AUD_ERR("Audio set software params err\n");
		return -1;
	}
	
	AUD_DBG("Audio set format ok\n");
	return 0;
}

int audio_hal_get_format(channel_t handle, audio_ext_fmt_t *dev_fmt)
{
	snd_pcm_hw_params_t *hardwareParams = NULL;
	snd_pcm_access_t  access;
	snd_pcm_format_t  fmt_val;
	snd_pcm_uframes_t buffersize, periodsize;
	int err;

	err = snd_pcm_hw_params_malloc(&hardwareParams);
	if (err < 0) {
		AUD_ERR("Failed to allocate ALSA hardware parameters!\n");
		return -1;
	}

	err = snd_pcm_hw_params_current(handle->handle, hardwareParams);
	if (err < 0) {
		AUD_ERR("Unable to get hardware configure: %s\n", snd_strerror(err));
		goto done;
	}

	snd_pcm_hw_params_get_access(hardwareParams, &access);
	dev_fmt->access = (unsigned int)access;
	snd_pcm_hw_params_get_channels(hardwareParams, &dev_fmt->channels);
	snd_pcm_hw_params_get_format(hardwareParams, &fmt_val);
	dev_fmt->bitwidth = alsa_to_format(fmt_val);
	snd_pcm_hw_params_get_rate(hardwareParams, &dev_fmt->samplingrate, 0);
	snd_pcm_hw_params_get_period_size(hardwareParams, &periodsize, 0);
	dev_fmt->sample_size = (unsigned int)periodsize;
	snd_pcm_hw_params_get_buffer_size(hardwareParams, &buffersize);
	dev_fmt->pcm_max_frm = buffersize/periodsize;

done:
	snd_pcm_hw_params_free(hardwareParams);
	return err;
}

int audio_hal_close(channel_t handle)
{
	if (!handle)
		return -1;

	return snd_pcm_close(handle->handle);
}

static void xrun(snd_pcm_t *handle, int stream)
{
	snd_pcm_status_t *status;    
	static snd_output_t *log;
	int res;               
	
snd_output_stdio_attach(&log, stderr, 0);

	snd_pcm_status_alloca(&status);                                                                
	if ((res = snd_pcm_status(handle, status))<0) {                                                
		AUD_ERR("status error: %s\n", snd_strerror(res));            
		return;
	}                                                                                              
	if (snd_pcm_status_get_state(status) == SND_PCM_STATE_XRUN) {                                  
		struct timeval now, diff, tstamp;                                                          
		gettimeofday(&now, 0);                                                                     
		snd_pcm_status_get_trigger_tstamp(status, &tstamp);                                        
		timersub(&now, &tstamp, &diff);                                                            
		AUD_ERR("%s!!! (at least %.3f ms long)\n",                                      
				stream == SND_PCM_STREAM_PLAYBACK ? "underrun" : "overrun",                      
				diff.tv_sec * 1000 + diff.tv_usec / 1000.0);                                           
			AUD_ERR("Status:\n");                                                       
			snd_pcm_status_dump(status, log);                                                      
		if ((res = snd_pcm_prepare(handle))<0) {                                                   
			AUD_ERR("xrun: prepare error: %s", snd_strerror(res));                                
			return;
		}                                                                                          
		return;     /* ok, data should be accepted again */                                        
	} if (snd_pcm_status_get_state(status) == SND_PCM_STATE_DRAINING) {                            
			AUD_ERR("Status(DRAINING):\n");                                             
			snd_pcm_status_dump(status, log);                                                      
		if (stream == SND_PCM_STREAM_CAPTURE) {                                                    
			AUD_ERR("capture stream format change? attempting recover...\n");           
			if ((res = snd_pcm_prepare(handle))<0) {                                               
				AUD_ERR("xrun(DRAINING): prepare error: %s", snd_strerror(res));                  
				return;
			}                                                                                      
			return;                                                                                
		}                                                                                          
	}    
	AUD_ERR("Status(R/W):\n");                                                      
		snd_pcm_status_dump(status, log);                                                          
	AUD_ERR("read/write error, state = %s", snd_pcm_state_name(snd_pcm_status_get_state(status)));
};

uint64_t audio_frame_times(channel_t handle, int length)
{
	audio_fmt_t *format = &(handle->fmt);
	int len;

	if (!handle || !format)
		return -1;
	
	len = format_to_byte(format->bitwidth) * format->channels * format->samplingrate;
	return (1000 * length) / len;
}

int audio_hal_read(channel_t handle,  void *buf, int len)
{
	int err;

	if (!handle)
		return -1;

	err = snd_pcm_readi(handle->handle, buf, buflen_to_frames(handle, len));
	if (err == -EPIPE) {
		AUD_ERR("Audio read xrun\n");
		xrun(handle->handle, SND_PCM_STREAM_CAPTURE);
		return buflen_to_frames(handle, len);
	} else if (err == -EAGAIN || (err >= 0 && (size_t)err < buflen_to_frames(handle, len)))
		snd_pcm_wait(handle->handle, 1000);
	else if (err == -ESTRPIPE)
		AUD_ERR("Audio Maybe Suspended.\n");
	else if (err < 0) {
		AUD_ERR("Audio read err: %d\n", err);
		return -1;
	}

	return err;
}

int audio_hal_write(channel_t handle,  void *buf, int len)
{
	int err;

	if (!handle)
		return -1;

	err = snd_pcm_writei(handle->handle, buf, buflen_to_frames(handle, len));
	if (err == -EPIPE) {
		AUD_ERR("Audio write xrun: %d\n", buflen_to_frames(handle, len));
		xrun(handle->handle, SND_PCM_STREAM_PLAYBACK);
		return buflen_to_frames(handle, len);
	} else if (err == -EAGAIN || (err >= 0 && (size_t)err < buflen_to_frames(handle, len))) {
		AUD_ERR("Audio need write %d, real write %d\n", buflen_to_frames(handle, len), err);
		snd_pcm_wait(handle->handle, 1000);
	} else if (err == -ESTRPIPE)
		AUD_ERR("Audio Maybe Suspended.\n");
	else if (err < 0) {
		AUD_ERR("Audio write err: %d\n", err);
		return -1;
	}

	return err;
}
