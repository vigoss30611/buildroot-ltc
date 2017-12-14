/*
 * =====================================================================================
 *
 *       Filename:  vcp7g_dbg.h
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
 * vcp7g_dbg.h
 * Copyright (C) 2016 Xiao Yongming <Xiao Yongming@ben-422-infotm>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef VCP7G_DBG_H
#define VCP7G_DBG_H

#define VCP_DBG_CONFIG_FILE "/mnt/sd0/vcp_dbg.config"
#define VCP_DBG_PROFILE_FILE "/mnt/sd0/profile.bin"

#define VCP_EXT_PROFILE_MODE "vcp_ext_profile"
#define VCP_LOGGER_MODE "vcp_logger"
#define VCP_CI_MODE		"vcp_ci"
#define VCP_NORMAL_MODE "vcp_normal"

#define VCP_NORMAL_ID  0
#define VCP_LOGGER_ID  1
#define VCP_CI_ID	   2	
#define VCP_EXT_PROFILE_ID	   3

#define MODE_MAX_LEN 16
#define IP_MAX_LEN 16
#define PORT_MAX_LEN 16

#define VCP_DBG_PROFILE_SIZE (1024)
#define VCP_DBG_LOGGER_SIZE (10*1024)

struct vcp7g_dbg {
	char mode[MODE_MAX_LEN];
	int mode_id;
	char server_ip[IP_MAX_LEN];
	char server_port[PORT_MAX_LEN];
	char client_ip[IP_MAX_LEN];
	char client_port[PORT_MAX_LEN];
	int socketfd;
	int connectfd;
	pthread_t serv_id;
	char serv_running;
	char * profile_data;
};


int vcp7g_dbg_init(struct vcp7g_dbg ** dbg);
int vcp7g_dbg_deinit(struct vcp7g_dbg ** dbg);
int vcp7g_dbg_send(struct vcp7g_dbg * dbg, char * buf, int len);
int vcp7g_dbg_recv(struct vcp7g_dbg * dbg, char * buf, int len);
int vcp7g_dbg_ci_parse(struct vcp7g_dbg * dbg, char * buf, int len);

#endif /* !VCP7G_H */

