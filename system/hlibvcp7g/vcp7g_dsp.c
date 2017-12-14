/*
 * =====================================================================================
 *
 *       Filename:  vcp7g_dsp.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年12月20日 15时02分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ben.xiao 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "vcp7g_dsp.h"
#include "vcp7g_dbg.h"
#include <dsp/tl421-user.h>
#include <dsp/lib-tl421.h>

#undef VCP_CODEC_BYPASS
#undef VCP_RX_ENHANCE
#undef VCP_DEBUG
#undef VCP_PROFILING
#undef VCP_USE_DSP_LIB

#define VCP_CODEC_BYPASS
//#define VCP_RX_ENHANCE
//#define VCP_DEBUG
//#define VCP_PROFILING
#define VCP_USE_DSP_LIB

#define VCP_DSP_DEV TL421_NODE

#ifdef VCP_DEBUG
	#define VCP_DBG(x...) do{printf("VCP: " x);}while(0)
#else
	#define VCP_DBG(x...) do{}while(0)
#endif

#define VCP_ERR(x...) do{printf("VCP: " x);}while(0)

#define RESAMP_EXTRA_NUM			32   //sometime resample need extra dst mem to handle the resample 

#define VCP_INPUT_CHANNEL			2
//#define VCP_INPUT_SAMPLE_RATE		16000
#define VCP_INPUT_SAMPLE_RATE		8000	
#define VCP_INPUT_SAMPLE_NUM(x)		(x / 125) // how many sample in a frame
#define VCP_INPUT_BITWIDTH			16	
#define VCP_INPUT_BIT_RATE			(VCP_INPUT_CHANNEL * VCP_INPUT_SAMPLE_RATE * VCP_INPUT_BITWIDTH)

#define VCP_OUTPUT_CHANNEL			1
//#define VCP_OUTPUT_SAMPLE_RATE		16000	
#define VCP_OUTPUT_SAMPLE_RATE		8000	
#define VCP_OUTPUT_BITWIDTH			16	
#define VCP_OUTPUT_BIT_RATE			(VCP_INPUT_CHANNEL * VCP_INPUT_SAMPLE_RATE * VCP_INPUT_BITWIDTH)

#define VCP_FRAME_NUM				(8) // how many frames we will handle every procedure
#define VCP_SAMPLE_NUM				(VCP_INPUT_SAMPLE_NUM(VCP_INPUT_SAMPLE_RATE) * VCP_FRAME_NUM) //sample num for VCP library per channel:8kHz-2048; 16kHZ-4096. 
#define VCP_SAMPLE_SIZE				(VCP_SAMPLE_NUM	* (VCP_INPUT_BITWIDTH>>3)) //sample size for VCP library per channel

static struct vcp7g_dbg *dbg_config = NULL;

int vcp7g_dsp_init(struct vcp_object *vcp_obj, int channels, int samplingrate, int bitwidth) { 

	int ret = 0;
	int bit_rate = 0;
	unsigned int smem;
	struct aec_info info;
	struct aec_dbg_info dbg_info;
	struct codec_info in_resample_info;
	struct codec_info out_resample_info;


	vcp7g_dbg_init(&dbg_config);



	if(channels != VCP_INPUT_CHANNEL) {
		VCP_DBG("don't support in_channels = %d\n", channels);
		ret = -EINVAL;
		goto out;
	}
	if(VCP_INPUT_SAMPLE_NUM(VCP_INPUT_SAMPLE_RATE) == 0) {
		VCP_DBG("don't support sample rate = %d\n", VCP_INPUT_SAMPLE_RATE);
		ret = -EINVAL;
		goto out;
	}

	if(vcp_obj == NULL) {
		VCP_DBG("vcp_obj == NULL\n");
		ret = -EINVAL;
		goto out;
	}

	vcp_obj->sample_num = VCP_SAMPLE_NUM; 
	VCP_DBG("vcp_obj->sample_num = %d\n", vcp_obj->sample_num);
	vcp_obj->sample_size = vcp_obj->sample_num * (samplingrate / VCP_INPUT_SAMPLE_RATE) * channels * (bitwidth >> 3); //64 * 32 * 2 * 2
	VCP_DBG("vcp_obj->sample_size = %d\n", vcp_obj->sample_size);
	bit_rate = bitwidth * samplingrate * channels;

	if(samplingrate != VCP_INPUT_SAMPLE_RATE || bitwidth != VCP_INPUT_BITWIDTH) {

		//vcp_obj->in_resample_size = (int)((long long)vcp_obj->sample_size * (long long)VCP_INPUT_BIT_RATE / bit_rate);
		vcp_obj->in_resample_size = (int)((vcp_obj->sample_num + RESAMP_EXTRA_NUM) * VCP_INPUT_CHANNEL * (VCP_INPUT_BITWIDTH >> 3));
		VCP_DBG("vcp_obj->in_resample_size = %d\n", vcp_obj->in_resample_size);
		vcp_obj->in_resample_buf = calloc(vcp_obj->in_resample_size, sizeof(char));
		if(vcp_obj->in_resample_buf == NULL) {
			VCP_DBG("no enough space for in_resample_buf\n");
			ret = -ENOMEM;
			goto out_resample;
		}

		in_resample_info.in.codec_type = AUDIO_CODEC_PCM;
		in_resample_info.in.channel =channels;
		in_resample_info.in.sample_rate = samplingrate;
		in_resample_info.in.bitwidth = bitwidth;
		in_resample_info.in.bit_rate = bit_rate;
						
		in_resample_info.out.codec_type = AUDIO_CODEC_PCM;
		in_resample_info.out.channel = VCP_INPUT_CHANNEL;
		in_resample_info.out.sample_rate = VCP_INPUT_SAMPLE_RATE;
		in_resample_info.out.bitwidth = VCP_INPUT_BITWIDTH;
		in_resample_info.out.bit_rate = VCP_INPUT_BIT_RATE;

		vcp_obj->in_resample_codec = codec_open(&in_resample_info);

		if (!vcp_obj->in_resample_codec) {
			VCP_DBG("open resample_codec failed %x \n",  in_resample_info.in.codec_type);
			ret = -EBUSY;
			goto out_resample;
		}
	}
	else {
		vcp_obj->in_resample_codec = NULL;
		vcp_obj->in_resample_buf = NULL;
	}

	if(channels != VCP_OUTPUT_CHANNEL || samplingrate != VCP_OUTPUT_SAMPLE_RATE || bitwidth != VCP_OUTPUT_BITWIDTH) {

		vcp_obj->out_resample_size = (vcp_obj->sample_num + RESAMP_EXTRA_NUM) * channels * (bitwidth>>3) * samplingrate / VCP_OUTPUT_SAMPLE_RATE;
		VCP_DBG("vcp_obj->out_resample_size = %d\n", vcp_obj->out_resample_size);
		vcp_obj->out_resample_buf = calloc(vcp_obj->out_resample_size, sizeof(char));
		if(vcp_obj->out_resample_buf == NULL) {
			VCP_DBG("no enough space for out_resample_buf\n");
			ret = -ENOMEM;
			goto out_resample;
		}

		out_resample_info.in.codec_type = AUDIO_CODEC_PCM;
		out_resample_info.in.channel = VCP_OUTPUT_CHANNEL;
		out_resample_info.in.sample_rate = VCP_OUTPUT_SAMPLE_RATE;
		out_resample_info.in.bitwidth = VCP_OUTPUT_BITWIDTH;
		out_resample_info.in.bit_rate = VCP_OUTPUT_BIT_RATE;

		out_resample_info.out.codec_type = AUDIO_CODEC_PCM;
		out_resample_info.out.channel =channels;
		out_resample_info.out.sample_rate = samplingrate;
		out_resample_info.out.bitwidth = bitwidth;
		out_resample_info.out.bit_rate = bit_rate;

		vcp_obj->out_resample_codec = codec_open(&out_resample_info);

		if (!vcp_obj->out_resample_codec) {
			VCP_DBG("open resample_codec failed %x \n",  out_resample_info.in.codec_type);
			ret = -EBUSY;
			goto out_resample;
		}
	}
	else {
		vcp_obj->out_resample_codec = NULL;
		vcp_obj->out_resample_buf = NULL;
	}

#ifdef VCP_USE_DSP_LIB
	ret = ceva_tl421_open(NULL);
	if(ret < 0) {
		ret = -EBUSY;
		VCP_DBG("open dsp device failed!\n");
		goto out_resample;
	}
#else
	vcp_obj->dsp_fd = open(VCP_DSP_DEV, O_RDWR);
	if(vcp_obj->dsp_fd < 0) {
		ret = -EBUSY;
		VCP_DBG("open dsp device failed!\n");
		goto out_resample;
	}
	else {

		VCP_DBG("vcp_obj->dsp_fd = %d\n", vcp_obj->dsp_fd);
	}
#endif

	if(dbg_config) {
		memset(&dbg_info, 0, sizeof(dbg_info));
		if (dbg_config->mode_id == VCP_CI_ID) {
			dbg_info.fmt.dbg_mode = VCP_CI_ID;
			vcp_obj->profile_len = dbg_info.buf_addr.dbg_len = VCP_DBG_PROFILE_SIZE; //I don't know, what should be the size of a complete profile?
			if(ceva_tl421_aec_dbg_open(NULL, &dbg_info) == 0) {
				vcp_obj->profile = (char *)dbg_info.buf_addr.aec_dbg_out_virt;
				VCP_DBG("%d: vcp_obj->profile = 0x%x", __LINE__, vcp_obj->profile);
			}
			else  {
				VCP_ERR("enter dbg mode %s failed, back to normal\n", dbg_config->mode);
				vcp_obj->profile = NULL;
				vcp7g_dbg_deinit(&dbg_config);
				goto dbg_out;
			}

			while(1){
				int len;
				if((len = vcp7g_dbg_recv(dbg_config, vcp_obj->profile, vcp_obj->profile_len)) > 0) {
					if(vcp7g_dbg_ci_parse(dbg_config, vcp_obj->profile, len) > 0)
						goto dbg_out;
				}
			}

		}
		else if (dbg_config->mode_id == VCP_EXT_PROFILE_ID) {
			dbg_info.fmt.dbg_mode = VCP_CI_ID; //for ceva dsp driver, it is same as vcp_ci_mode
			vcp_obj->profile_len = dbg_info.buf_addr.dbg_len = VCP_DBG_PROFILE_SIZE; //I don't know, what should be the size of a complete profile?
			if(ceva_tl421_aec_dbg_open(NULL, &dbg_info) == 0) {
				vcp_obj->profile = (char *)dbg_info.buf_addr.aec_dbg_out_virt;
				VCP_DBG("%d: vcp_obj->profile = 0x%x", __LINE__, vcp_obj->profile);
			}
			else  {
				VCP_ERR("enter dbg mode %s failed, back to normal\n", dbg_config->mode);
				vcp_obj->profile = NULL;
				vcp7g_dbg_deinit(&dbg_config);
				goto dbg_out;
			}

			if(vcp7g_dbg_read_profile(dbg_config, vcp_obj->profile, vcp_obj->profile_len) < 0) {

				//already set dbg_info to profile buf so just use that
				//dbg_info.buf_addr.aec_dbg_out_virt = (char *)vcp_obj->profile;
				//dbg_info.buf_addr.dbg_len = vcp_obj->profile_len;
				ceva_tl421_aec_dbg_close(NULL, &dbg_info);
				vcp_obj->profile = NULL;
				vcp7g_dbg_deinit(&dbg_config);
				goto dbg_out;
			}
		}
		else if (dbg_config->mode_id == VCP_LOGGER_ID) {
			dbg_info.fmt.dbg_mode = VCP_LOGGER_ID;
			vcp_obj->logger_len = dbg_info.buf_addr.dbg_len = VCP_DBG_LOGGER_SIZE; //I don't know, what should be the size of a logger buffer?
			if(ceva_tl421_aec_dbg_open(NULL, &dbg_info) == 0) {
				vcp_obj->logger = (char *)dbg_info.buf_addr.aec_dbg_out_virt;
				VCP_DBG("%d: vcp_obj->logger = 0x%x", __LINE__, vcp_obj->logger);
			}
			else {
				VCP_ERR("enter dbg mode %s failed, back to normal\n", dbg_config->mode);
				vcp_obj->logger = NULL;
				vcp7g_dbg_deinit(&dbg_config);
				goto dbg_out;
			}
		}
	}
dbg_out:

	memset(&info, 0, sizeof(info));
	info.fmt.channel = VCP_INPUT_CHANNEL;
	info.fmt.sample_rate = VCP_INPUT_SAMPLE_RATE;
	info.fmt.bitwidth = VCP_INPUT_BITWIDTH;
	info.fmt.sample_size = VCP_SAMPLE_SIZE;
	VCP_DBG("info.fmt.channel = %d, info.fmt.sample_rate = %d, info.fmt.bitwidth = %d, info.fmt.sample_size = %d!\n",  info.fmt.channel, info.fmt.sample_rate, info.fmt.bitwidth, info.fmt.sample_size);

#ifdef VCP_USE_DSP_LIB
	ret = ceva_tl421_aec_set_format(NULL, &info);
	if(ret) {
		VCP_DBG("dsp set format failed!\n");
		goto out_close_dsp;
	}
	vcp_obj->spk = (short *)info.buf_addr.spk_in_virt;
	vcp_obj->mic = (short *)info.buf_addr.mic_in_virt;
	vcp_obj->mout = vcp_obj->sout = (short *)info.buf_addr.aec_out_virt;
#else
	VCP_DBG("vcp_obj->dsp_fd = %d\n", vcp_obj->dsp_fd);
	ret = ioctl(vcp_obj->dsp_fd, TL421_AEC_SET_FMT, &info);
	if(ret) {
		VCP_DBG("dsp set format failed!\n");
		goto out_close_dsp;
	}
	ret = mmap(0, info.buf_addr.data_len, PROT_READ | PROT_WRITE, MAP_SHARED, vcp_obj->dsp_fd, info.buf_addr.spk_in_phy);
	if(ret == MAP_FAILED) {
		vcp_obj->spk = NULL;
		VCP_DBG("couldn't mmap memory space for spk: %s\n", strerror(errno));
		ret = -ENOMEM;
		goto out_unmap_dsp;
	}
	vcp_obj->spk = (short *)ret;

	ret = mmap(0, info.buf_addr.data_len, PROT_READ | PROT_WRITE, MAP_SHARED, vcp_obj->dsp_fd, info.buf_addr.mic_in_phy);
	if(ret == MAP_FAILED) {
		vcp_obj->mic = NULL;
		VCP_DBG("couldn't mmap memory space for mic: %s\n", strerror(errno));
		ret = -ENOMEM;
		goto out_unmap_dsp;
	}
	vcp_obj->mic = (short *)ret;

	ret = mmap(0, info.buf_addr.data_len, PROT_READ | PROT_WRITE, MAP_SHARED, vcp_obj->dsp_fd, info.buf_addr.aec_out_phy); // they share the same buffer because they don't used simultaneously
	if(ret == MAP_FAILED) {
		vcp_obj->mout = NULL;
		vcp_obj->sout = NULL;
		VCP_DBG("couldn't mmap memory space for mout/sout: %s\n", strerror(errno));
		ret = -ENOMEM;
		goto out_unmap_dsp;
	}
	vcp_obj->mout = vcp_obj->sout = (short *)ret;
#endif
	
	VCP_DBG("vcp_obj->spk = 0x%p, vcp_obj->mic = 0x%p, vcp_obj->mout = 0x%p\n", vcp_obj->spk, vcp_obj->mic, vcp_obj->mout);



	return 0;

#ifdef VCP_USE_DSP_LIB
#else
out_unmap_dsp:
	if(vcp_obj->spk)
		munmap(vcp_obj->spk, info.buf_addr.data_len);
	if(vcp_obj->mic)
		munmap(vcp_obj->mic, info.buf_addr.data_len);
	if(vcp_obj->mout) //also ummap the sout
		munmap(vcp_obj->mout, info.buf_addr.data_len);
#endif

out_close_dsp:
#ifdef VCP_USE_DSP_LIB
	ceva_tl421_aec_close(NULL, NULL);
#else
	close(vcp_obj->dsp_fd);
#endif

out_resample:
	if(vcp_obj->in_resample_buf)
		free(vcp_obj->in_resample_buf);
	if(vcp_obj->out_resample_buf)
		free(vcp_obj->out_resample_buf);

	if(vcp_obj->in_resample_codec)
		codec_close(vcp_obj->in_resample_codec);
	if(vcp_obj->out_resample_codec)
		codec_close(vcp_obj->out_resample_codec);

out_dbg:
	if(dbg_config) {
		vcp7g_dbg_deinit(&dbg_config);
	}
out:
	return ret;
}

int vcp7g_dsp_deinit(struct vcp_object *vcp_obj) { 

	int length;
	int ret;
	struct aec_info info;
	struct aec_dbg_info dbg_info;

	if(vcp_obj == NULL)
		return -1;

	//pthread_mutex_lock(&vcp_obj->mutex);
	VCP_DBG("free in_reasmple_codec\n");
	if(vcp_obj->in_resample_codec) {

#if 0
		do {
			ret = codec_flush(vcp_obj->in_resample_codec, &vcp_obj->in_resample_addr, &length);
			if (ret == CODEC_FLUSH_ERR)
				break;

			/* TODO you need least data or not ?*/
		} while (ret == CODEC_FLUSH_AGAIN);
#endif
		codec_close(vcp_obj->in_resample_codec);
		vcp_obj->in_resample_codec = NULL;
	}

	VCP_DBG("free out_reasmple_codec\n");
	if(vcp_obj->out_resample_codec) {

#if 0
		do {
			ret = codec_flush(vcp_obj->out_resample_codec, &vcp_obj->out_resample_addr, &length);
			if (ret == CODEC_FLUSH_ERR)
				break;

			/* TODO you need least data or not ?*/
		} while (ret == CODEC_FLUSH_AGAIN);
#endif
		codec_close(vcp_obj->out_resample_codec);
		vcp_obj->out_resample_codec = NULL;
	}

	VCP_DBG("free in_reasmple_buf = 0x%p\n", vcp_obj->in_resample_buf);
	if(vcp_obj->in_resample_buf) {
		free(vcp_obj->in_resample_buf);
		vcp_obj->in_resample_buf = NULL;
	}

	VCP_DBG("free out_reasmple_buf = 0x%p\n", vcp_obj->out_resample_buf);
	if(vcp_obj->out_resample_buf) {
		free(vcp_obj->out_resample_buf);
		vcp_obj->out_resample_buf = NULL;
	}
	
#ifdef VCP_USE_DSP_LIB	
	if(dbg_config) {
		memset(&dbg_info, 0, sizeof(dbg_info));
		if(dbg_config->mode_id == VCP_LOGGER_ID) {
			dbg_info.buf_addr.aec_dbg_out_virt = (char *)vcp_obj->logger;
			dbg_info.buf_addr.dbg_len = vcp_obj->logger_len;
			VCP_DBG("close aec dbg = 0x%p\n", dbg_info.buf_addr.aec_dbg_out_virt);
		}
		else if(dbg_config->mode_id == VCP_CI_ID || dbg_config->mode_id == VCP_EXT_PROFILE_ID) {
			dbg_info.buf_addr.aec_dbg_out_virt = (char *)vcp_obj->profile;
			dbg_info.buf_addr.dbg_len = vcp_obj->profile_len;
			VCP_DBG("close aec dbg = 0x%p\n", dbg_info.buf_addr.aec_dbg_out_virt);
		}
		ceva_tl421_aec_dbg_close(NULL, &dbg_info);
	}

	memset(&info, 0, sizeof(info));
	info.buf_addr.spk_in_virt = (char *)vcp_obj->spk;
	info.buf_addr.mic_in_virt = (char *)vcp_obj->mic;
	info.buf_addr.aec_out_virt = (char *)vcp_obj->mout;
	info.buf_addr.data_len = VCP_SAMPLE_SIZE;
	VCP_DBG("close aec - releasing spk= 0x%p, mic = 0x%p, mout = 0x%p\n", info.buf_addr.spk_in_virt, info.buf_addr.mic_in_virt, info.buf_addr.aec_out_virt);
	ceva_tl421_aec_close(NULL, &info);

	vcp_obj->spk = NULL;
	vcp_obj->mic = NULL;
	vcp_obj->mout = NULL;
	vcp_obj->sout = NULL;

#else 

	VCP_DBG("unmap spk = 0x%p\n", vcp_obj->spk);
	if(vcp_obj->spk)
		munmap(vcp_obj->spk, VCP_SAMPLE_SIZE);
	vcp_obj->spk = NULL;

	VCP_DBG("unmap mic = 0x%p\n", vcp_obj->mic);
	if(vcp_obj->mic)
		munmap(vcp_obj->mic, VCP_SAMPLE_SIZE);
	vcp_obj->mic = NULL;

	VCP_DBG("unmap mout = 0x%p\n", vcp_obj->mout);
	if(vcp_obj->mout) //also ummap the sout
		munmap(vcp_obj->mout, VCP_SAMPLE_SIZE);
	vcp_obj->mout = NULL;
	vcp_obj->sout = NULL;

	VCP_DBG("close dsp_fd: %d\n", vcp_obj->dsp_fd);
	close(vcp_obj->dsp_fd); //the memory alloc inside dsp driver will release here

#endif
	if(dbg_config) {
	    VCP_DBG("deinit dbg_config: %p\n", dbg_config);
		vcp7g_dbg_deinit(&dbg_config);
	}
	//pthread_mutex_unlock(&vcp_obj->mutex);

	return 0;
}



int vcp7g_dsp_playback_process(struct vcp_object *vcp_obj, char * in, char * out, int len) {

#if 0
	if(!vcp_obj) {
		VCP_DBG("vcp_obj == NULL\n");
		return -1;
	}

	int ret = 0;
	int length = 0;
	char *in_buf = in;
	char *out_buf = out;
	short *spk = (vcp_obj->spk);
	short *mout = (vcp_obj->mout);
	struct codec_addr *in_resample_addr = &(vcp_obj->in_resample_addr);
	struct codec_addr *out_resample_addr = &(vcp_obj->out_resample_addr);
	char* in_resample_buf = NULL;
	char* out_resample_buf = NULL;
	int out_resample_size = 0;
	struct aec_codec aec_addr;
	int comsumed_size = 0;
	int in_resample_size = 0;

	#ifdef VCP_RX_ENHANCE
	if(!spk || !mout) {
		VCP_DBG("spk/mout == NULL\n");
		return -1;
	}

	if(len % vcp_obj->sample_size)
		VCP_DBG("Warning len \% sample_size != 0, blank padding data will be appended\n");

	length = len;

	//pthread_mutex_lock(&vcp_obj->mutex);
	while(length > 0)
	{
		int m,n = vcp_obj->sample_num;
		int i = 0;

		VCP_DBG("length = %d \n", length);
		comsumed_size = (length > vcp_obj->sample_size) ? vcp_obj->sample_size : length;
		if(vcp_obj->in_resample_codec) {
			VCP_DBG("in resampling \n");
			in_resample_addr->in = in_buf;
			in_resample_addr->len_in = comsumed_size;
			in_resample_addr->out = vcp_obj->in_resample_buf;
			in_resample_addr->len_out = vcp_obj->in_resample_size;

			ret = codec_resample(vcp_obj->in_resample_codec, in_resample_addr->out, in_resample_addr->len_out, in_resample_addr->in, in_resample_addr->len_in);
			VCP_DBG("after in resample \n");
			if (ret < 0) {
				VCP_DBG("resample failed %d \n",  ret);
				do {
					ret = codec_flush(vcp_obj->in_resample_codec, in_resample_addr, &length);
					if (ret == CODEC_FLUSH_ERR)
						break;

					/* TODO you need least data or not ?*/
				} while (ret == CODEC_FLUSH_AGAIN);
				return -1;
			}
			in_resample_buf = (vcp_obj->in_resample_buf);
			in_resample_size = ret;
		}
		else {
			in_resample_buf = in_buf;
			in_resample_size = comsumed_size;
		}

		VCP_DBG("in_resample_buf = %x \n",  in_resample_buf);
		VCP_DBG("spk = %x \n",  spk);

		if(!in_resample_buf) {
			VCP_DBG("in_resample_buf == NULL\n");
			return -1;
		}

		m = n = in_resample_size / VCP_INPUT_CHANNEL / (VCP_INPUT_BITWIDTH>>3);
		while(i < n)
		{
			*(spk+i) = *(in_resample_buf+(i*4)) | *(in_resample_buf+(i*4)+1)<<8;
			i++;
		}

		vcpTErrorCode ErrorCode = vcpProcessRx((vcpTObj)vcp_obj->mem_obj,spk,mout,vcp_obj->sample_num);
		if( ErrorCode != VCP_NO_ERROR )
		{
			VCP_DBG("vcpProcessDebug() return ErrorCode %d\n",ErrorCode);
			return -1;
		}
			
		if(vcp_obj->out_resample_codec) {
			VCP_DBG("out resampling \n");
			out_resample_addr->in = mout;
			out_resample_addr->len_in = m * sizeof(short);
			out_resample_addr->out = vcp_obj->out_resample_buf;
			out_resample_addr->len_out = vcp_obj->out_resample_size;

			ret = codec_resample(vcp_obj->out_resample_codec, out_resample_addr->out, out_resample_addr->len_out, out_resample_addr->in, out_resample_addr->len_in);
			VCP_DBG("after out resampling \n");
			if (ret < 0) {
				VCP_DBG("resample failed %d \n",  ret);
				do {
					ret = codec_flush(vcp_obj->out_resample_codec, &out_resample_addr, &length);
					if (ret == CODEC_FLUSH_ERR)
						break;

					/* TODO you need least data or not ?*/
				} while (ret == CODEC_FLUSH_AGAIN);
				return -1;
			}

			out_resample_buf = (vcp_obj->out_resample_buf);
	        out_resample_size = ret;
		}
		else {
			out_resample_buf = (char *)mout;
			out_resample_size = m * sizeof(short);
		}
		VCP_DBG("out_resample_buf = %x \n",  out_resample_buf);
		VCP_DBG("out_resample_size = %d \n",  out_resample_size);

		if(!out_resample_buf) {
			VCP_DBG("out_resample_buf == NULL\n");
			return -1;
		}

		memcpy(out_buf, out_resample_buf, out_resample_size);

		in_buf += comsumed_size;
		out_buf += out_resample_size;
		length -= comsumed_size;

	}
	//pthread_mutex_unlock(&vcp_obj->mutex);
	#else
	memcpy(out, in, len);
	#endif

#endif
	return 0;
}


int vcp7g_dsp_capture_process(struct vcp_object *vcp_obj, char * in, char * out, int len) {


	if(!vcp_obj) {
		VCP_DBG("vcp_obj == NULL\n");
		return -1;
	}

	int ret = 0;
	int length = 0;
	int comsumed_size = 0;
#ifndef VCP_USE_DSP_LIB
	int dsp_fd = vcp_obj->dsp_fd;
#endif
	char *in_buf = in;
	char *out_buf = out;
	int *spk_off = &(vcp_obj->spk_off);
	int *mic_off = &(vcp_obj->mic_off);
	short *spk = (vcp_obj->spk);
	short *mic = (vcp_obj->mic);
	short *mout = (vcp_obj->mout);
	short *sout = (vcp_obj->sout);
	struct codec_addr *in_resample_addr = &(vcp_obj->in_resample_addr);
	struct codec_addr *out_resample_addr = &(vcp_obj->out_resample_addr);
	char* in_resample_buf = NULL;
	char* out_resample_buf = NULL;
	int out_resample_size = 0;
	struct aec_codec aec_addr;
	int in_resample_size = 0;
	char *logger = (vcp_obj->logger);

#ifdef VCP_PROFILING
	struct timeval ts, te, ts1, te1;
	float endurance;
	gettimeofday(&ts, NULL);
#endif

#ifndef VCP_USE_DSP_LIB
	if(dsp_fd < 0) {
		VCP_DBG("dsp_fd < 0\n");
		return -1;
	}
#endif

	if((!spk) || (!mic) || (!mout) || (!sout)) {
		VCP_DBG("spk/mic/mout/sout == NULL\n");
		return -1;
	}

	length = len;

	//pthread_mutex_lock(&vcp_obj->mutex);


	while(length > 0)
	{
		int n = 0;
		int i = 0;

		VCP_DBG("length = %d \n", length);
		comsumed_size = (length > vcp_obj->sample_size) ? vcp_obj->sample_size : length;
		VCP_DBG("comsumed_size = %d \n", comsumed_size);
		if(vcp_obj->in_resample_codec) {
			VCP_DBG("in resampling \n");
			in_resample_addr->in = in_buf;
			in_resample_addr->len_in = comsumed_size;
			in_resample_addr->out = vcp_obj->in_resample_buf;
			in_resample_addr->len_out = vcp_obj->in_resample_size;

			VCP_DBG("in_resample_addr->in = 0x%x \n",  in_resample_addr->in);
			VCP_DBG("in_resample_addr->len_in = %d \n",  in_resample_addr->len_in);
			VCP_DBG("in_resample_addr->out = 0x%x \n",  in_resample_addr->out);
			VCP_DBG("in_resample_addr->len_out = %d \n",  in_resample_addr->len_out);

			ret = codec_resample(vcp_obj->in_resample_codec, in_resample_addr->out, in_resample_addr->len_out, in_resample_addr->in, in_resample_addr->len_in);
			VCP_DBG("after in resample \n");
			if (ret < 0) {
				VCP_DBG("resample failed %d \n",  ret);
				do {
					ret = codec_flush(vcp_obj->in_resample_codec, in_resample_addr, &length);
					if (ret == CODEC_FLUSH_ERR)
						break;

					/* TODO you need least data or not ?*/
				} while (ret == CODEC_FLUSH_AGAIN);
				return -1;
			}
			in_resample_buf = (vcp_obj->in_resample_buf);
			in_resample_size = ret;
		}
		else {
			in_resample_buf = in_buf;
			in_resample_size = comsumed_size;
		}

		VCP_DBG("in_resample_buf = 0x%x \n",  in_resample_buf);
		VCP_DBG("in_resample_size = %d \n",  in_resample_size);
		VCP_DBG("spk = 0x%x \n",  spk);
		VCP_DBG("mic = 0x%x \n",  mic);

		if(!in_resample_buf) {
			VCP_DBG("in_resample_buf == NULL\n");
			return -1;
		}

		n = in_resample_size / VCP_INPUT_CHANNEL / (VCP_INPUT_BITWIDTH>>3);
		memset(spk, 0, (VCP_SAMPLE_SIZE));
		memset(mic, 0, (VCP_SAMPLE_SIZE));
		while(n--)
		{
#if 1
			*(spk+i) = *(in_resample_buf+(i*4)) | *(in_resample_buf+(i*4)+1)<<8;
			*(mic+i) = *(in_resample_buf+(i*4)+2) | *(in_resample_buf+(i*4)+3)<<8;
#else
			*(mic+i) = *(in_resample_buf+(i*4)) | *(in_resample_buf+(i*4)+1)<<8;
			*(spk+i) = *(in_resample_buf+(i*4)+2) | *(in_resample_buf+(i*4)+3)<<8;
#endif

			i++;
		}

		memset(&aec_addr, 0, sizeof(aec_addr));
		aec_addr.spk_src = spk;
		aec_addr.mic_src = mic;
		aec_addr.dst = mout;
		aec_addr.len = in_resample_size / VCP_INPUT_CHANNEL;
		if(dbg_config) {
			if(dbg_config->mode_id == VCP_LOGGER_ID){
				aec_addr.dbg_dst = logger;
			}
		}
#ifdef VCP_USE_DSP_LIB
#ifdef VCP_PROFILING
		gettimeofday(&ts1, NULL);
		endurance = (float)(ts1.tv_sec - ts.tv_sec) + ((float)(ts1.tv_usec - ts.tv_usec))/1000000;
		VCP_ERR("in resample total spend %fs\n", endurance);
#endif

		ret = ceva_tl421_aec_process(NULL, &aec_addr);
//		vcpTErrorCode ErrorCode = vcpProcessTx((vcpTObj)vcp_obj->mem_obj,spk,mic,mout,vcp_obj->sample_num);
		if( ret < 0 )
		{
			VCP_DBG("ioctl PROCESS fail\n");
			return -1;
		}
		VCP_DBG("aec_addr.len = %d return from driver\n", aec_addr.len);

#ifdef VCP_PROFILING
		gettimeofday(&te1, NULL);
		endurance = (float)(te1.tv_sec - ts1.tv_sec) + ((float)(te1.tv_usec - ts1.tv_usec))/1000000;
		VCP_ERR("ceva_tl421_aec_process total spend %fs\n", endurance);
#endif
#else
		VCP_DBG("vcp_obj->dsp_fd = %d\n", vcp_obj->dsp_fd);
		VCP_DBG("aec_addr.len = %d\n", aec_addr.len);
		ret = ioctl(dsp_fd, TL421_AEC_PROCESS, &aec_addr);
//		vcpTErrorCode ErrorCode = vcpProcessTx((vcpTObj)vcp_obj->mem_obj,spk,mic,mout,vcp_obj->sample_num);
		if( ret < 0 )
		{
			VCP_DBG("ioctl PROCESS fail\n");
			return -1;
		}
		VCP_DBG("aec_addr.len = %d return from driver\n", aec_addr.len);
#endif
			
		if(vcp_obj->out_resample_codec) {
			VCP_DBG("out resampling \n");
			out_resample_addr->in = mout;
			out_resample_addr->len_in = aec_addr.len / VCP_OUTPUT_CHANNEL;
			out_resample_addr->out = vcp_obj->out_resample_buf;
			out_resample_addr->len_out = vcp_obj->out_resample_size;

			VCP_DBG("out_resample_addr->out = 0x%x \n",  out_resample_addr->in);
			VCP_DBG("out_resample_addr->len_out = %d \n",  out_resample_addr->len_in);
			VCP_DBG("out_resample_addr->out = 0x%x \n",  out_resample_addr->out);
			VCP_DBG("out_resample_addr->len_out = %d \n",  out_resample_addr->len_out);

			ret = codec_resample(vcp_obj->out_resample_codec, out_resample_addr->out, out_resample_addr->len_out, out_resample_addr->in, out_resample_addr->len_in);
			VCP_DBG("after out resampling \n");
			if (ret < 0) {
				VCP_DBG("resample failed %d \n",  ret);
				do {
					ret = codec_flush(vcp_obj->out_resample_codec, &out_resample_addr, &length);
					if (ret == CODEC_FLUSH_ERR)
						break;

					/* TODO you need least data or not ?*/
				} while (ret == CODEC_FLUSH_AGAIN);
				return -1;
			}

			out_resample_buf = (vcp_obj->out_resample_buf);
	        out_resample_size = ret;
		}
		else {
			out_resample_buf = (char *)mout;
			out_resample_size = aec_addr.len;
		}
		VCP_DBG("out_resample_buf = %x \n",  out_resample_buf);
		VCP_DBG("out_resample_size = %d \n",  out_resample_size);

		if(!out_resample_buf) {
			VCP_DBG("out_resample_buf == NULL\n");
			return -1;
		}

		memcpy(out_buf, out_resample_buf, out_resample_size);

		in_buf += comsumed_size;
		out_buf += out_resample_size;
		length -= comsumed_size;

		if(dbg_config) {

			if(dbg_config->mode_id == VCP_LOGGER_ID) {
				vcp7g_dbg_send(dbg_config, logger, aec_addr.dbg_len);
			}

		}
	}
	//pthread_mutex_unlock(&vcp_obj->mutex);

#ifdef VCP_PROFILING
	gettimeofday(&te, NULL);

	endurance = (float)(te.tv_sec - te1.tv_sec) + ((float)(te.tv_usec - te1.tv_usec))/1000000;
	VCP_ERR("out resample total spend %fs\n", endurance);

	endurance = (float)(te.tv_sec - ts.tv_sec) + ((float)(te.tv_usec - ts.tv_usec))/1000000;
	//VCP_DBG("%s total spend %fs\n", __func__, endurance);
	VCP_ERR("%s total spend %fs\n", __func__, endurance);
#endif
	return 0;
}

