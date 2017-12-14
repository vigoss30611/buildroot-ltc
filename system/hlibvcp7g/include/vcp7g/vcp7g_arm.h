/*
 * =====================================================================================
 *
 *       Filename:  vcp7g_arm.h
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
 * vcp7g_arm.h
 * Copyright (C) 2016 Xiao Yongming <Xiao Yongming@ben-422-infotm>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef VCP7G_ARM_H
#define VCP7G_ARM_H
#include <errno.h>
#include <fcntl.h>
#include "vcp7g.h"

int vcp7g_arm_init(struct vcp_object *vcp_obj, int channels, int samplingrate, int bitwidth);
int vcp7g_arm_deinit(struct vcp_object *vcp_obj); 
int vcp7g_arm_playback_process(struct vcp_object *vcp_obj, char * in, char * out, int len);
int vcp7g_arm_capture_process(struct vcp_object *vcp_obj, char * in, char * out, int len);

#endif /* !VCP7G_ARM_H */

