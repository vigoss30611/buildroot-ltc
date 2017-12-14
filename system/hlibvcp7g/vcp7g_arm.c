/*
 * =====================================================================================
 *
 *       Filename:  vcp7g_arm.c
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
#include <sys/time.h>
#include <errno.h>
#include "vcpAPI.h"
#include "profile-new-8khz.h"
//#include "profile-new-16khz.h"
#include "vcp7g_arm.h"

#undef VCP_RX_ENHANCE
#undef VCP_DEBUG

//#define VCP_RX_ENHANCE
//#define VCP_DEBUG

#ifdef VCP_DEBUG
	#define VCP_DBG(x...) do{printf("VCP: " x);}while(0)
	//#define VCP_DBG(x...) do{}while(0)
#else
	#define VCP_DBG(x...) do{}while(0)
#endif

#define RESAMP_EXTRA_NUM			64   //sometime resample need extra dst mem to handle the resample 

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
#define VCP_SAMPLE_NUM				(VCP_INPUT_SAMPLE_NUM(VCP_INPUT_SAMPLE_RATE) * VCP_FRAME_NUM) //sample num for VCP library per channel: 8kHz-2048; 16kHZ-4096. 
#define VCP_SAMPLE_SIZE				(VCP_SAMPLE_NUM	* (VCP_INPUT_BITWIDTH>>3)) //sample size for VCP library per channel


int vcp7g_arm_init(struct vcp_object *vcp_obj, int channels, int samplingrate, int bitwidth) { 

	int ret = 0;
	int bit_rate = 0;
	vcpTError Error;
	unsigned int smem;
	struct codec_info in_resample_info;
	struct codec_info out_resample_info;

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

	memset(vcp_obj, 0, sizeof(vcp_obj));
//	vcp_obj->sample_num = VCP_INPUT_SAMPLE_NUM(VCP_INPUT_SAMPLE_RATE);
	vcp_obj->sample_num = VCP_SAMPLE_NUM;
	VCP_DBG("vcp_obj->sample_num = %d\n", vcp_obj->sample_num);
	vcp_obj->sample_size = vcp_obj->sample_num * (samplingrate / VCP_INPUT_SAMPLE_RATE) * channels * (bitwidth >> 3); //64 * 32 * 2 * 2
	//vcp_obj->sample_size = vcp_obj->sample_num * channels * (bitwidth >> 3);
	VCP_DBG("vcp_obj->sample_size = %d\n", vcp_obj->sample_size);
	bit_rate = bitwidth * samplingrate * channels;

	if(samplingrate != VCP_INPUT_SAMPLE_RATE || bitwidth != VCP_INPUT_BITWIDTH) {

		vcp_obj->in_resample_size = (int)((vcp_obj->sample_num + RESAMP_EXTRA_NUM) * VCP_INPUT_CHANNEL * (VCP_INPUT_BITWIDTH >> 3));
		VCP_DBG("vcp_obj->in_resample_size = %d\n", vcp_obj->in_resample_size);
		vcp_obj->in_resample_buf = calloc(vcp_obj->in_resample_size, sizeof(char));
		if(vcp_obj->in_resample_buf == NULL) {
			VCP_DBG("no enough space for in_resample_buf\n");
			ret = -ENOMEM;
			goto out_resample;
		}

		memset(&in_resample_info, 0, sizeof(in_resample_info));
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

		memset(&out_resample_info, 0, sizeof(out_resample_info));
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
	}

	Error = vcpObjMemSizeFlat(&profile, &smem);
	
	if( Error.ErrCode != VCP_NO_ERROR )
	{
		VCP_DBG("vcpObjMemSize() return ErrorCode %d, ErrInfo %d\n",Error.ErrCode,Error.ErrInfo);
		ret = -EAGAIN;
		goto out_resample;
	}
	else {
		VCP_DBG("vcpObjMemSize() return smem %d\n", smem);
	}
	
	vcp_obj->mem_obj = malloc(smem);
	vcp_obj->spk = calloc(VCP_SAMPLE_SIZE, sizeof(char));
	vcp_obj->mic = calloc(VCP_SAMPLE_SIZE, sizeof(char));
	vcp_obj->mout = calloc(VCP_SAMPLE_SIZE, sizeof(char));
	vcp_obj->sout = calloc(VCP_SAMPLE_SIZE, sizeof(char));

	if(!vcp_obj->mem_obj || !vcp_obj->spk || !vcp_obj->mic || !vcp_obj->mout || !vcp_obj->sout) {
		VCP_DBG("no enough space for mem_obj/spk/mic/mout/sout\n");
		ret = -ENOMEM;
		goto out_free_vcp;
	}
	
	Error = vcpInitObjFlat(vcp_obj->mem_obj, smem, &profile);
	if( Error.ErrCode != VCP_NO_ERROR )
	{
		VCP_DBG("vcpInitObj() return ErrorCode %d, ErrInfo %d\n",Error.ErrCode,Error.ErrInfo);
		ret = -ENOMEM;
		goto out_free_vcp;
	}

	VCP_DBG("Allocataed %d bytes\n", smem);
	pthread_mutex_init(&vcp_obj->mutex, NULL);
	vcp_obj->spk_off = 2 * 1024 + 512 +16;
	vcp_obj->mic_off = 0;

	return 0;

out_free_vcp:
	free(vcp_obj->mem_obj);
	free(vcp_obj->spk);
	free(vcp_obj->mic);
	free(vcp_obj->mout);
	free(vcp_obj->sout);

out_resample:
	if(vcp_obj->in_resample_buf)
		free(vcp_obj->in_resample_buf);
	if(vcp_obj->out_resample_buf)
		free(vcp_obj->out_resample_buf);

	if(vcp_obj->in_resample_codec)
		codec_close(vcp_obj->in_resample_codec);
	if(vcp_obj->out_resample_codec)
		codec_close(vcp_obj->out_resample_codec);

out:
	return ret;
}

int vcp7g_arm_deinit(struct vcp_object *vcp_obj) { 

	int length;
	int ret;

	if(vcp_obj == NULL)
		return -1;

	pthread_mutex_lock(&vcp_obj->mutex);

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
	}

	VCP_DBG("free in_reasmple_buf = 0x%p\n", vcp_obj->in_resample_buf);
	if(vcp_obj->in_resample_buf)
		free(vcp_obj->in_resample_buf);

	VCP_DBG("free out_reasmple_buf = 0x%p\n", vcp_obj->out_resample_buf);
	if(vcp_obj->out_resample_buf)
		free(vcp_obj->out_resample_buf);

	// vcp7g_arm_xxxx_process will check whether spk/mic/mout/sout is NULL or not, this will prevent the process from continuing
	VCP_DBG("free spk = 0x%p\n", vcp_obj->spk);
	free(vcp_obj->spk);
	vcp_obj->spk = NULL;
	VCP_DBG("free mic = 0x%p\n", vcp_obj->mic);
	free(vcp_obj->mic);
	vcp_obj->mic = NULL;
	VCP_DBG("free mout = 0x%p\n", vcp_obj->mout);
	free(vcp_obj->mout);
	vcp_obj->mout = NULL;
	VCP_DBG("free sout = 0x%p\n", vcp_obj->sout);
	free(vcp_obj->sout);
	vcp_obj->sout = NULL;
	pthread_mutex_unlock(&vcp_obj->mutex);
	VCP_DBG("vcp7g_arm_deinit finish\n");

	return 0;
}

int vcp7g_arm_playback_process(struct vcp_object *vcp_obj, char * in, char * out, int len) {
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

	pthread_mutex_lock(&vcp_obj->mutex);
	while(length > 0)
	{
		int m, n = vcp_obj->sample_num;
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
	pthread_mutex_unlock(&vcp_obj->mutex);
	#else
	memcpy(out, in, len);
	#endif

#endif
	return 0;
}

int vcp7g_arm_capture_process(struct vcp_object *vcp_obj, char * in, char * out, int len) {

	if(!vcp_obj) {
		VCP_DBG("vcp_obj == NULL\n");
		return -1;
	}

	int ret = 0;
	int length = 0;
	int comsumed_size = 0;
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
	int in_resample_size = 0;
	int out_resample_size = 0;



	if(!spk || !mic || !mout || !sout) {
		VCP_DBG("spk/mic/mout/sout == NULL\n");
		return -1;
	}

	length = len;

	pthread_mutex_lock(&vcp_obj->mutex);


	while(length > 0)
	{
		int n = 0;
		int i = 0;

		VCP_DBG("length = %d \n", length);
		comsumed_size = (length > vcp_obj->sample_size) ? vcp_obj->sample_size : length;
		if(vcp_obj->in_resample_codec) {
			VCP_DBG("in resampling \n");
			in_resample_addr->in = in_buf;
			in_resample_addr->len_in = comsumed_size;
			in_resample_addr->out = vcp_obj->in_resample_buf;
			in_resample_addr->len_out = vcp_obj->in_resample_size;
			VCP_DBG("in resampling: in_resample_addr->out = %p, in_resample_addr->len_out = %d, in_resample_addr->in = %p,in_resample_addr->len_in = %d\n", in_resample_addr->out, in_resample_addr->len_out, in_resample_addr->in, in_resample_addr->len_in);

			ret = codec_resample(vcp_obj->in_resample_codec, in_resample_addr->out, in_resample_addr->len_out, in_resample_addr->in, in_resample_addr->len_in);
			VCP_DBG("after in resample \n");
			if (ret < 0) {
				VCP_DBG("in resample failed %d \n",  ret);
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
		VCP_DBG("in_resample_size = %d \n",  in_resample_size);
		VCP_DBG("spk = %x \n",  spk);
		VCP_DBG("mic = %x \n",  mic);
		VCP_DBG("mout = %x \n",  mout);

		if(!in_resample_buf) {
			VCP_DBG("in_resample_buf == NULL\n");
			return -1;
		}

		n = in_resample_size / VCP_INPUT_CHANNEL / (VCP_INPUT_BITWIDTH>>3);
		VCP_DBG("n = %d \n", n);
		memset(spk, 0, (VCP_SAMPLE_SIZE));
		memset(mic, 0, (VCP_SAMPLE_SIZE));
		while(n--)
		{
			*(spk+i) = *(in_resample_buf+(i*4)) | *(in_resample_buf+(i*4)+1)<<8;
			*(mic+i) = *(in_resample_buf+(i*4)+2) | *(in_resample_buf+(i*4)+3)<<8;
			i++;
		}
#if 1
		for(n = 0; n < (VCP_SAMPLE_NUM); n += VCP_INPUT_SAMPLE_NUM(VCP_INPUT_SAMPLE_RATE)) {
			vcpTErrorCode ErrorCode = vcpProcessTx((vcpTObj)vcp_obj->mem_obj, spk+n, mic+n, mout+n, VCP_INPUT_SAMPLE_NUM(VCP_INPUT_SAMPLE_RATE));
	//		vcpTErrorCode ErrorCode = vcpProcessTx((vcpTObj)vcp_obj->mem_obj,spk,mic,mout,32);
	//		vcpTErrorCode ErrorCode = vcpProcessDebug((vcpTObj)vcp_obj->mem_obj,spk,mic,sout,mout,vcp_obj->sample_num);
			if( ErrorCode != VCP_NO_ERROR )
			{
				VCP_DBG("vcpProcessDebug() return ErrorCode %d\n",ErrorCode);
				return -1;
			}
		}
#endif
			
		if(vcp_obj->out_resample_codec) {
			VCP_DBG("out resampling \n");
			out_resample_addr->in = mout;
			out_resample_addr->len_in = VCP_SAMPLE_SIZE;
			out_resample_addr->out = vcp_obj->out_resample_buf;
			out_resample_addr->len_out = vcp_obj->out_resample_size;

			VCP_DBG("out resampling: out_resample_addr->len_out = %d, out_resample_addr->len_in = %d\n", out_resample_addr->len_out, out_resample_addr->len_in);
			ret = codec_resample(vcp_obj->out_resample_codec, out_resample_addr->out, out_resample_addr->len_out, out_resample_addr->in, out_resample_addr->len_in);
			VCP_DBG("after out resampling \n");
			if (ret < 0) {
				VCP_DBG("out resample failed %d \n",  ret);
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
			out_resample_size = VCP_SAMPLE_SIZE;
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
	pthread_mutex_unlock(&vcp_obj->mutex);
	return 0;
}
