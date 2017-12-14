/*
 * =====================================================================================
 *
 *       Filename:  vcp7g_dsp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年12月20日 19时03分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (); 
 *   Organization:  
 *
 * =====================================================================================
 */
/*
 * vcp7g_dsp.h
 * Copyright (C) 2016 Xiao Yongming <Xiao Yongming@ben-422-infotm>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef VCP7G_H
#define VCP7G_H
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <qsdk/codecs.h>

struct vcp_object {
	struct codec_addr in_resample_addr;
	void *in_resample_codec;
	char *in_resample_buf;
	int in_resample_size;

	struct codec_addr out_resample_addr;
	void *out_resample_codec;
	char *out_resample_buf;
	int out_resample_size;

	short *spk;
	short *mic;
	short *mout;
	short *sout;

	int sample_num;
	int sample_size;

	void *mem_obj;

	pthread_mutex_t mutex;
	int use_dsp;

	int  spk_off;
	int  mic_off;

	int  dsp_fd;

	int initiated;

	int logger_len;
	char * logger;

	int profile_len;
	char * profile;
};

int vcp7g_init(struct vcp_object *vcp_obj, int channels, int samplingrate, int bitwidth, int* use_dsp);
int vcp7g_deinit(struct vcp_object *vcp_obj); 
int vcp7g_playback_process(struct vcp_object *vcp_obj, char * in, char * out, int len);
int vcp7g_capture_process(struct vcp_object *vcp_obj, char * in, char * out, int len);
int vcp7g_is_init(struct vcp_object *vcp_obj);

#endif /* !VCP7G_H */

