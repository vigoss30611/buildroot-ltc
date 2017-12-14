/*
 * =====================================================================================
 *
 *       Filename:  vcp7g.c
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
#include "vcp7g_dsp.h"
#include "vcp7g_arm.h"

#if defined(AEC_DSP_SUPPORT) && defined(AEC_ARM_SUPPORT)
int vcp7g_init(struct vcp_object *vcp_obj, int channels, int samplingrate, int bitwidth, int *use_dsp) { 

	int ret = -1;

	if (*use_dsp && !(ret = vcp7g_dsp_init(vcp_obj, channels, samplingrate, bitwidth))) {
		*use_dsp = 1;
	}
	else {
		*use_dsp = 0;
		ret = vcp7g_arm_init(vcp_obj, channels, samplingrate, bitwidth);
	}
	vcp_obj->use_dsp = *use_dsp;

	if(!ret)
		vcp_obj->initiated = 1;
	else
		vcp_obj->initiated = 0;

	return ret;
}

int vcp7g_deinit(struct vcp_object *vcp_obj) { 
	int ret = -1;

	if(vcp_obj->use_dsp) {
		ret = vcp7g_dsp_deinit(vcp_obj);
	}
	else {
		ret = vcp7g_arm_deinit(vcp_obj);
	}
	vcp_obj->initiated = 0;

	return ret;
}

int vcp7g_playback_process(struct vcp_object *vcp_obj, char * in, char * out, int len) {

	if(vcp_obj->use_dsp) {
		return vcp7g_dsp_playback_process(vcp_obj, in, out, len);
	}
	else {
		return vcp7g_arm_playback_process(vcp_obj, in, out, len);
	}
	return -1;
}

int vcp7g_capture_process(struct vcp_object *vcp_obj, char * in, char * out, int len) {

	if(vcp_obj->use_dsp) {
		return vcp7g_dsp_capture_process(vcp_obj, in, out, len);
	}
	else {
		return vcp7g_arm_capture_process(vcp_obj, in, out, len);
	}
	return -1;
}

#elif defined(AEC_DSP_SUPPORT)
int vcp7g_init(struct vcp_object *vcp_obj, int channels, int samplingrate, int bitwidth, int *use_dsp) { 

	int ret = -1;

	*use_dsp = 1;
	ret = vcp7g_dsp_init(vcp_obj, channels, samplingrate, bitwidth);
	vcp_obj->use_dsp = *use_dsp;

	if(!ret)
		vcp_obj->initiated = 1;
	else
		vcp_obj->initiated = 0;

	return ret;
}

int vcp7g_deinit(struct vcp_object *vcp_obj) { 
	vcp_obj->initiated = 0;
	return vcp7g_dsp_deinit(vcp_obj);
}

int vcp7g_playback_process(struct vcp_object *vcp_obj, char * in, char * out, int len) {

	return vcp7g_dsp_playback_process(vcp_obj, in, out, len);
}

int vcp7g_capture_process(struct vcp_object *vcp_obj, char * in, char * out, int len) {

	return vcp7g_dsp_capture_process(vcp_obj, in, out, len);
}

#elif defined(AEC_ARM_SUPPORT)
int vcp7g_init(struct vcp_object *vcp_obj, int channels, int samplingrate, int bitwidth, int *use_dsp) { 

	int ret = -1;

	*use_dsp = 0;
	ret = vcp7g_arm_init(vcp_obj, channels, samplingrate, bitwidth);
	vcp_obj->use_dsp = *use_dsp;

	if(!ret)
		vcp_obj->initiated = 1;
	else
		vcp_obj->initiated = 0;

	return ret;
}

int vcp7g_deinit(struct vcp_object *vcp_obj) { 
	vcp_obj->initiated = 0;
	return vcp7g_arm_deinit(vcp_obj);
}

int vcp7g_playback_process(struct vcp_object *vcp_obj, char * in, char * out, int len) {

	return vcp7g_arm_playback_process(vcp_obj, in, out, len);
}

int vcp7g_capture_process(struct vcp_object *vcp_obj, char * in, char * out, int len) {

	return vcp7g_arm_capture_process(vcp_obj, in, out, len);
}
#else
int vcp7g_init(struct vcp_object *vcp_obj, int channels, int samplingrate, int bitwidth, int *use_dsp) { 

	vcp_obj->initiated = 0;
	return -1;
}

int vcp7g_deinit(struct vcp_object *vcp_obj) { 

	vcp_obj->initiated = 0;
	return -1; 
}

int vcp7g_playback_process(struct vcp_object *vcp_obj, char * in, char * out, int len) {

	return -1;
}

int vcp7g_capture_process(struct vcp_object *vcp_obj, char * in, char * out, int len) {

	return -1;
}
#endif

int vcp7g_is_init(struct vcp_object *vcp_obj) {

	if(!vcp_obj)
		return -1;

	return vcp_obj->initiated;
}
