#include <alsa/asoundlib.h>
#include <list.h>
#include <malloc.h>
#include <fr/libfr.h>
#include <audiobox2.h>
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
static int buflen_to_frames(audio_dev_t dev, int len)
{
	audio_dev_attr_t *attr;

	if (!dev)
		return -1;

	attr = &(dev->attr);
	return (len / (attr->channels * format_to_byte(attr->bitwidth)));
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

static int get_threshold_val(audio_dev_attr_t *attr)
{
	int i;

	if (!attr)
		return 1;
	
	for (i = 0; i < THR_NUM; i++)
		if (set[i].sample_rate == attr->samplingrate &&
			set[i].bitwidth == attr->bitwidth &&
			set[i].channels == attr->channels)
			return set[i].value;

	return 1;
}

static int setsoftwareparams(audio_dev_t dev, audio_dev_attr_t *attr)
{
	snd_pcm_sw_params_t * softwareParams;
	audio_dev_attr_t *dev_attr = &(dev->attr);
	snd_pcm_uframes_t buffersize = 0;
	snd_pcm_uframes_t periodsize = 0;
	snd_pcm_uframes_t startThreshold, stopThreshold;
#ifdef AUDIOBOX_DEBUG
	static snd_output_t *log;
#endif
	int err;

	if (attr)
		dev_attr = attr;
#ifdef AUDIOBOX_DEBUG
	err = snd_output_stdio_attach(&log, stderr, 0);
#endif
	if (snd_pcm_sw_params_malloc(&softwareParams) < 0) {
	    AUD_ERR("Failed to allocate ALSA software parameters!\n");
	    return -1;
	}

	err = snd_pcm_sw_params_current(dev->handle, softwareParams);
	if (err < 0) {
	    AUD_ERR("Unable to get software parameters: %s\n", snd_strerror(err));
	    goto done;
	}

	snd_pcm_get_params(dev->handle, &buffersize, &periodsize);
	if (dev->direct == DEVICE_OUT_ALL) {
	    startThreshold = buffersize / get_threshold_val(dev_attr) - 1;
	    stopThreshold = buffersize;
	} else {
	    startThreshold = 1;
	    stopThreshold = buffersize;
	}

	err = snd_pcm_sw_params_set_start_threshold(dev->handle, softwareParams,
		startThreshold);
	if (err < 0) {
	    AUD_ERR("Unable to set start threshold to %lu frames: %s\n",
		    startThreshold, snd_strerror(err));
	    goto done;
	}

	err = snd_pcm_sw_params_set_stop_threshold(dev->handle, softwareParams,
		stopThreshold);
	if (err < 0) {
	    AUD_ERR("Unable to set stop threshold to %lu frames: %s\n",
		    stopThreshold, snd_strerror(err));
	    goto done;
	}

	err = snd_pcm_sw_params_set_avail_min(dev->handle, softwareParams,
		periodsize);
	if (err < 0) {
	    AUD_ERR("Unable to configure available minimum to %lu: %s\n",
		    periodsize, snd_strerror(err));
	    goto done;
	}

	// Commit the software parameters back to the device.
	err = snd_pcm_sw_params(dev->handle, softwareParams);
	if (err < 0) 
		AUD_ERR("Unable to configure software parameters: %s\n",
			snd_strerror(err));
#ifdef AUDIOBOX_DEBUG	
	snd_pcm_dump(dev->handle, log);
#endif
done:
	snd_pcm_sw_params_free(softwareParams);

	return err;
}

static int sethardwareparams(audio_dev_t dev, audio_dev_attr_t *attr)
{
	snd_pcm_hw_params_t *hardwareParams = NULL;
	audio_dev_attr_t *dev_attr = &(dev->attr);
	snd_pcm_uframes_t buffersize, periodsize;
	unsigned int requestedrate;
	unsigned int		samplerate;
	int err;
	
	if (attr)
		dev_attr = attr;

	err = snd_pcm_hw_params_malloc(&hardwareParams);
	if (err < 0) {
		AUD_ERR("Failed to allocate ALSA hardware parameters!\n");
		return -1;
	}

	err = snd_pcm_hw_params_any(dev->handle, hardwareParams);
	if (err < 0) {
		AUD_ERR("Unable to configure hardware: %s\n", snd_strerror(err));
		goto done;
	}
	
	err = snd_pcm_hw_params_set_access(dev->handle, hardwareParams,
			SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0) {
		AUD_ERR("Unable to configure PCM read/write format: %s\n",
				snd_strerror(err));
		goto done;
	}

	err = snd_pcm_hw_params_set_format(dev->handle, hardwareParams,
			format_to_alsa(dev_attr->bitwidth));
	if (err < 0) {
		AUD_ERR("Unable to configure PCM format %d : %s\n",
				dev_attr->bitwidth, snd_strerror(err));
		goto done;
	}

	err = snd_pcm_hw_params_set_channels(dev->handle, hardwareParams,
			dev_attr->channels);
	if (err < 0) {
		AUD_ERR("Unable to set channel count to %i: %s\n",
				dev_attr->channels, snd_strerror(err));
		goto done;
	}
	
	requestedrate = dev_attr->samplingrate;
	err = snd_pcm_hw_params_set_rate_near(dev->handle, hardwareParams,
			&requestedrate, 0);
	if (requestedrate != dev_attr->samplingrate) {
		AUD_ERR("Rate doesn't match (requested %iHz, get %iHz)\n",
				dev_attr->samplingrate, requestedrate);
		goto done;
	}
	
	periodsize = dev_attr->sample_size;	
	err = snd_pcm_hw_params_set_period_size_near(dev->handle,
		hardwareParams, &periodsize, 0);
	if (err < 0) {
		AUD_ERR("Unable to set period_size %ld for playback: %s\n",
				periodsize, snd_strerror(err));
		goto done;
	}	
	
	buffersize = 4 * periodsize;
	err = snd_pcm_hw_params_set_buffer_size_near(dev->handle,
			hardwareParams, &buffersize);
	if (err < 0) {
		AUD_ERR("Unable to set buffer_size %ld for playback: %s\n",
				buffersize, snd_strerror(err));
		goto done;
	}

	err = snd_pcm_hw_params(dev->handle, hardwareParams);
	if (err < 0)
		AUD_ERR("Unable to set hardware parameters: %s\n", snd_strerror(err));

	AUD_DBG("AUDIO : dev %s\n", dev->nodename);
	snd_pcm_hw_params_get_period_size(hardwareParams, &periodsize, 0);
	snd_pcm_hw_params_get_buffer_size(hardwareParams, &buffersize);
	snd_pcm_hw_params_get_rate(hardwareParams, &samplerate, NULL);
	AUD_DBG("~~~~~~~~~~~~~~~~~~~~~~~AUDIO : sample_size: %d buffersize = %ld; periodsize = %ld, samplerate:(%d->%d)!\n"
		, dev_attr->sample_size, buffersize, periodsize, dev_attr->samplingrate, samplerate);

done:
	snd_pcm_hw_params_free(hardwareParams);
	return err;
}

int audio_hal_open(audio_dev_t dev)
{
	int err = -1;

	if (!dev || !dev->devname)
		return -1;

	/* Open PCM device for record. */
	err = snd_pcm_open(&(dev->handle), dev->devname,
			direct_to_alsa(dev->direct), 0);
	if (err < 0) {
		AUD_ERR("Unable to open pcm device: %s\n", snd_strerror(err));
		return -1;
	}

	err = sethardwareparams(dev, NULL);
	if (err < 0) {
		AUD_ERR("Audio set hardware params err\n");	
		return -1;
	}

	err = setsoftwareparams(dev, NULL);
	if (err < 0) {
		AUD_ERR("Audio set software params err\n");
		return -1;
	}

	AUD_DBG("Audio Init Ok\n");
	return 0;
}

int audio_hal_set_params(audio_dev_t dev, audio_dev_attr_t *attr)
{
	int err;

	if (!dev || !attr)
		return -1;

	err = snd_pcm_drop(dev->handle);
	if (err < 0) {
		AUD_ERR("Failed to drop ALSA!\n");
		return -1;
	}

	err = sethardwareparams(dev, attr);
	if (err < 0) {
		AUD_ERR("Audio set hardware format err\n");
		return -1;
	}

	err = setsoftwareparams(dev, attr);
	if (err < 0) {
		AUD_ERR("Audio set software params err\n");
		return -1;
	}
	
	AUD_DBG("Audio set params ok\n");
	return 0;
}


int audio_hal_set_format(audio_dev_t dev, audio_dev_attr_t *attr)
{
	int err;

	if (!dev || !attr)
		return -1;

	err = snd_pcm_drop(dev->handle);
	if (err < 0) {
		AUD_ERR("Failed to drop ALSA!\n");
		return -1;
	}

	err = sethardwareparams(dev, attr);
	if (err < 0) {
		AUD_ERR("Audio set hardware format err\n");
		return -1;
	}

	err = setsoftwareparams(dev, attr);
	if (err < 0) {
		AUD_ERR("Audio set software params err\n");
		return -1;
	}
	
	AUD_DBG("Audio set format ok\n");
	return 0;
}

int audio_hal_close(audio_dev_t dev)
{
	if (!dev)
		return -1;

	return snd_pcm_close(dev->handle);
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

uint64_t audio_frame_times(audio_dev_t dev, int length)
{
	audio_dev_attr_t *attr = &(dev->attr);
	int len;

	if (!dev || !attr)
		return -1;
	
	len = format_to_byte(attr->bitwidth) * attr->channels * attr->samplingrate;
	return (1000 * length) / len;
}

int audio_hal_read(audio_dev_t dev,  void *buf, int len)
{
	int err;

	if (!dev)
		return -1;

	err = snd_pcm_readi(dev->handle, buf, buflen_to_frames(dev, len));
	if (err == -EPIPE) {
		AUD_ERR("Audio read xrun\n");
		xrun(dev->handle, SND_PCM_STREAM_CAPTURE);
		return buflen_to_frames(dev, len);
	} else if (err == -EAGAIN || (err >= 0 && (size_t)err < buflen_to_frames(dev, len)))
		snd_pcm_wait(dev->handle, 1000);
	else if (err == -ESTRPIPE)
		AUD_ERR("Audio Maybe Suspended.\n");
	else if (err < 0) {
		AUD_ERR("Audio read err: %d\n", err);
		return -1;
	}

	return err;
}

int audio_hal_write(audio_dev_t dev, void *buf, int len)
{
	int err;

	if (!dev)
		return -1;

	err = snd_pcm_writei(dev->handle, buf, buflen_to_frames(dev, len));
	if (err == -EPIPE) {
		AUD_ERR("Audio write xrun: %d\n", buflen_to_frames(dev, len));
		xrun(dev->handle, SND_PCM_STREAM_PLAYBACK);
		return buflen_to_frames(dev, len);
	} else if (err == -EAGAIN || (err >= 0 && (size_t)err < buflen_to_frames(dev, len))) {
		AUD_ERR("Audio need write %d, real write %d\n", buflen_to_frames(dev, len), err);
		snd_pcm_wait(dev->handle, 1000);
	} else if (err == -ESTRPIPE)
		AUD_ERR("Audio Maybe Suspended.\n");
	else if (err < 0) {
		AUD_ERR("Audio write err: %d\n", err);
		return -1;
	}

	return err;
}
