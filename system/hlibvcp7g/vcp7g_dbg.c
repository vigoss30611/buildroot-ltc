/*
 * =====================================================================================
 *
 *       Filename:  vcp7g_dbg.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2017年04月24日 10时48分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>  
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
 * vcp7g_dbg.c
 * Copyright (C) 2017 Xiao Yongming <Xiao Yongming@ben-422-infotm>
 *
 * Distributed under terms of the MIT license.
 */

#include "vcp7g_dbg.h"

#define MAGIC0_STR     "magic0:infotm"
#define MAGIC1_STR	   "magic1:000670"
#define MAGIC0_LEN     (strlen(MAGIC0_STR))
#define MAGIC1_LEN     (strlen(MAGIC1_STR))

#define MODE_TAG	   "mode:"
#define MODE_TAG_LEN   (strlen(MODE_TAG))

#define CLIENT_IP_TAG		  "client_ip:"
#define CLIENT_IP_TAG_LEN     (strlen(CLIENT_IP_TAG))

#define SERVER_PORT_TAG		  "server_port:"
#define SERVER_PORT_TAG_LEN   (strlen(SERVER_PORT_TAG))


//#define VCP_DEBUG
#ifdef VCP_DEBUG
	#define VCP_DBG(x...) do{printf("VCP: " x);}while(0)
#else
	#define VCP_DBG(x...) do{}while(0)
#endif

#define VCP_ERR(x...) do{printf("VCP: " x);}while(0)

static char *mode_list[] = {
	VCP_NORMAL_MODE,
	VCP_LOGGER_MODE,
	VCP_CI_MODE,
	VCP_EXT_PROFILE_MODE,
};


static int vcp7g_dbg_read_config(char * config_file, struct vcp7g_dbg * dbg) { 

	FILE *fp;
	char line[128] = {0};
	char *val = NULL, *p = NULL;
	int i = 0;
	char found = 0;

	if((config_file == NULL) || (dbg == NULL)) {
		return -1;
	}

	fp = fopen(config_file, "r");
	if(fp == NULL) {
		VCP_ERR("%s not exist\n", config_file);
		return -1;
	}
	else {
		VCP_ERR("reading %s\n", config_file);
	}

	//magic0
	found = 0;
	while ((fgets(line, sizeof(line), fp)) != NULL) {
		if (!strncmp(line, MAGIC0_STR, MAGIC0_LEN)) {
			VCP_DBG("magic0 in %s: %s\n", config_file, line);
			found = 1;
			break;
		}
	}

	if (!found) {
		VCP_ERR("can't found magic0 in %s\n", config_file);
		return -1;
	}

	//magic1
	found = 0;
	while ((fgets(line, sizeof(line), fp)) != NULL) {
		if (!strncmp(line, MAGIC1_STR, MAGIC1_LEN)) {
			VCP_DBG("magic1 in %s: %s\n", config_file, line);
			found = 1;
			break;
		}
	}

	if (!found) {
		VCP_ERR("can't found magic1 in %s\n", config_file);
		return -1;
	}

	//mode
	found = 0;
	while ((fgets(line, sizeof(line), fp)) != NULL) {
		if(!strncmp(line, MODE_TAG, MODE_TAG_LEN)) {
			VCP_DBG("mode tag in %s: %s\n", config_file, line);
			found = 1;
			break;
		}
	}

	if (!found) {
		VCP_ERR("can't found mode in %s\n", config_file);
		return -1;
	}
	else {
		val = &line[MODE_TAG_LEN];

		p = strchr(val, '\r');
		if(!p) {
			p = strchr(val, '\n');
		}
		if(!p) {
			p = &val[strlen(val)];
		}
		*p = '\0'; //drop the '\r' or '\r\n' tail
		VCP_ERR("mode:%s\n", val);

		for(i = 0; i < sizeof(mode_list)/sizeof(mode_list[0]); i++) {
			VCP_DBG("mode_list[%d] = %s\n", i,  mode_list[i]);
			if(!strncmp(val, mode_list[i], strlen(mode_list[i])))
				break;
		}
		if(mode_list[i] == NULL) {
			VCP_ERR("not a valid mode(%s) in %s\n", val, config_file);
			return -1;
		} else {
			VCP_DBG("find a valid mode(%s) in %s\n", mode_list[i], config_file);
		}

		strncpy(dbg->mode, mode_list[i], MODE_MAX_LEN);
		dbg->mode_id = i;
	}

	//client_ip
	found = 0;
	while ((fgets(line, sizeof(line), fp)) != NULL) {
		if(!strncmp(line, CLIENT_IP_TAG, CLIENT_IP_TAG_LEN)) {
			VCP_DBG("ip tag in %s: %s\n", config_file, line);
			found = 1;
			break;
		}
	}

	if (!found) {
		VCP_ERR("can't found client ip in %s\n", config_file);
		return -1;
	}
	else {
		val = &line[CLIENT_IP_TAG_LEN];

		p = strchr(val, '\r');
		if(!p) {
			p = strchr(val, '\n');
		}
		if(!p) {
			p = &val[strlen(val)];
		}
		*p = '\0'; //drop the '\r' or '\r\n' tail

		strncpy(dbg->client_ip, val, IP_MAX_LEN); 
		VCP_ERR("client_ip in %s: %s\n", config_file, dbg->client_ip);
	}

	//server_port
	found = 0;
	while ((fgets(line, sizeof(line), fp)) != NULL) {
		if(!strncmp(line, SERVER_PORT_TAG, SERVER_PORT_TAG_LEN)) {
			VCP_DBG("port tag in %s: %s\n", config_file, line);
			found = 1;
			break;
		}
	}

	if (!found) {
		VCP_ERR("can't found client port in %s\n", config_file);
		return -1;
	}
	else {
		val = &line[SERVER_PORT_TAG_LEN];

		p = strchr(val, '\r');
		if(!p) {
			p = strchr(val, '\n');
		}
		if(!p) {
			p = &val[strlen(val)];
		}
		*p = '\0'; //drop the '\r' or '\r\n' tail

		strncpy(dbg->server_port, val, PORT_MAX_LEN);
		VCP_ERR("server_port in %s: %s\n", config_file, dbg->server_port);
	}

	return 0;
}

static void *vcp7g_dbg_server(void* arg) {

	int socketfd;
	int connectfd;
	socklen_t addrlen;
	struct sockaddr_in servaddr;
	struct sockaddr_in clieaddr;

	char abuf[INET_ADDRSTRLEN];
	char * client_ip;
	int client_port;

	struct ifreq ifr;
	struct vcp7g_dbg * dbg = (struct vcp7g_dbg *)arg;

	int on, ret;

	dbg->serv_running = 1;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	dbg->socketfd = socketfd;

	if(socketfd == -1) {
		VCP_ERR("create socket failed !\n");
		goto out;
	}
	else {
		VCP_ERR("create socket fd = %d !\n", socketfd);
	}

	on = 1;
	ret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(ret < 0) {
		VCP_ERR("set socket option error: %s\n", strerror(errno));
		goto out;
	}

	strcpy(ifr.ifr_name, "wlan0");
	if(ioctl(socketfd, SIOCGIFADDR, &ifr) < 0) {
		perror("ioctl");
	}
	else {
		strncpy(dbg->server_ip, inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr), IP_MAX_LEN);
	}

	VCP_ERR("server_ip: %s\n", dbg->server_ip);

#if 0
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(dbg->port));
	if(inet_pton(AF_INET, dbg->ip, &servaddr.sin_addr) < 0) {
		VCP_ERR(" pton error for %s\n", dbg->ip);
		return -1;
	}
	if(connect(socketfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {

		VCP_ERR(" connect error(%s) for %s:%s\n", strerror(errno), dbg->ip, dbg->port);
		return -1;
	}
#else
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(dbg->server_port));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	/*
	if(inet_pton(AF_INET, dbg->ip, &servaddr.sin_addr) < 0) {
		VCP_ERR(" pton error for %s\n", dbg->ip);
		close(socketfd);
		dbg->socketfd = -1;
		return -1;
	}
	*/
	
	if(bind(socketfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		VCP_ERR("bind socket error: %s\n", strerror(errno));
		goto out;
	}

	if(listen(socketfd, 10) == -1) {
		VCP_ERR("listen socket error: %s\n", strerror(errno));
		goto out;
	}

	VCP_ERR("prepare to accept connect\n");
	addrlen = sizeof(clieaddr);
	memset(&clieaddr, 0, addrlen);
	while(1) {
		if((connectfd = accept(socketfd, (struct sockaddr*)&clieaddr, &addrlen)) != -1) { 
			client_ip = inet_ntop(AF_INET, &clieaddr.sin_addr, abuf, INET_ADDRSTRLEN);
			client_port = ntohs(clieaddr.sin_port);

			VCP_ERR("socket accepted: %d, ip=%s, port=%d\n", connectfd, client_ip, client_port);
			if(!strncmp(client_ip, dbg->client_ip, strlen(client_ip))) {
				snprintf(dbg->client_port, PORT_MAX_LEN, "%d", client_port);
				break;
			}
		}
		else {
			VCP_ERR("socket accepted error: %d\n", strerror(errno));
		}
		pthread_testcancel();
	}
	dbg->connectfd = connectfd;
	VCP_ERR("expected socket accepted: socket = %d, ip=%s, port=%d\n", connectfd, client_ip, client_port);

#endif
	VCP_ERR("Exiting %s: pthread_id = %u\n", __func__, dbg->serv_id);
	dbg->serv_running = 0;

	return 0;

out:

	if(dbg->socketfd >= 0) {
		close(dbg->socketfd);
		dbg->socketfd = -1;
	}

	if(dbg->connectfd >= 0) {
		close(dbg->connectfd);
		dbg->connectfd = -1;
	}

	dbg->serv_running = 0;
	return -1;
}


static int vcp7g_dbg_create_server(struct vcp7g_dbg * dbg) { 

	int err;

#if 0
    vcp7g_dbg_server((void*) dbg);
#else
	err = pthread_create(&dbg->serv_id, NULL, vcp7g_dbg_server, dbg);
	if (err != 0) {
		VCP_ERR("Create VCP dbg server err\n");
		return -1;
	}
#endif

	return 0;
}

int vcp7g_dbg_init(struct vcp7g_dbg ** pdbg) { 

	struct vcp7g_dbg *dbg = NULL;
	int ret = -1;

	if((dbg = (struct vcp_dbg *)calloc(sizeof(struct vcp7g_dbg), sizeof(char))) != NULL) {
		if(vcp7g_dbg_read_config(VCP_DBG_CONFIG_FILE, dbg) == 0) {
			if((dbg->mode_id == VCP_LOGGER_ID) || (dbg->mode_id == VCP_CI_ID)) {
				if(vcp7g_dbg_create_server(dbg) != 0) {
					free(dbg);
					dbg = NULL;
				}
				else {
					ret = 0;
				}
			}
			else if ((dbg->mode_id == VCP_NORMAL_ID) || (dbg->mode_id == VCP_EXT_PROFILE_ID)) {
				ret = 0;
			}
		}
		else {
			free(dbg);
			dbg = NULL;
		}
	}
	else {
		VCP_ERR("VCP:malloc for dbg_config fail\n");
		dbg = NULL;
	}

	*pdbg = dbg;
	return ret;
}

int vcp7g_dbg_deinit(struct vcp7g_dbg ** pdbg) { 

	int socketfd;
	struct sockaddr_in servaddr;
	struct vcp7g_dbg *dbg = *pdbg;

	if(dbg->serv_running == 1) {
		VCP_DBG("VCP:begin to wait for vcp7g_dbg_server(pid = %u) cancel\n", dbg->serv_id);
		pthread_cancel(dbg->serv_id); //may block here
		VCP_DBG("VCP:begin to wait for vcp7g_dbg_server(pid = %u) join\n", dbg->serv_id);
		pthread_join(dbg->serv_id, NULL); //may block here
		dbg->serv_running = 0;
	}

	if(dbg->connectfd >= 0) {
		close(dbg->connectfd);
		dbg->connectfd = -1;
	}

	if(dbg->socketfd >= 0) {
		close(dbg->socketfd);
		dbg->socketfd = -1;
	}

	free(dbg);
	*pdbg = NULL;
	return 0;
}


int vcp7g_dbg_send(struct vcp7g_dbg * dbg, char * buf, int len) { 
	int ret;
	int m;

	if(dbg->connectfd <= 0) {
		return -1;
	}

#if 0
	VCP_ERR(" %d send %d bytes logger(%p) to %s:%s\n", dbg->connectfd, len, buf, dbg->client_ip, dbg->client_port);
	for(m = 0; m < (len>64? 64 : len); m++) {
		printf("buf[%d] = 0x%x ", m, buf[m]);
		if((m % 8) == 0) {
			printf("\n");
		}
	}
#endif

	if((ret = send(dbg->connectfd, buf, len, 0)) < 0) {
		VCP_ERR(" %d send %d bytes logger(%p) to %s:%s failed(%s)\n", dbg->connectfd, len, buf, dbg->client_ip, dbg->client_port, strerror(errno));
		return -1;
	}
	//for debug, I hope 1s could spare the time needed for the send
	//sleep(1);
	return ret;
}

int vcp7g_dbg_recv(struct vcp7g_dbg * dbg, char * buf, int len) { 
	int ret;

	if(dbg->connectfd <= 0) {
		return -1;
	}

	VCP_DBG("socket(%d) begin to recv %d bytes profile(%p) from %s:%s\n", dbg->connectfd, len, buf, dbg->client_ip, dbg->client_port);

	if((ret = recv(dbg->connectfd, buf, len, 0)) < 0) {
		VCP_ERR(" %d recv %d bytes logger(%p) from %s:%s failed(%s)\n", dbg->connectfd, ret, buf, dbg->client_ip, dbg->client_port, strerror(errno));
		return -1;
	}
	return ret;
}

int vcp7g_dbg_ci_parse(struct vcp7g_dbg * dbg, char * buf, int len) {

	short ci_len = 0;
	short ci_opcode = 0;
	char send_buf[8];
	int i;

	VCP_ERR(" recv len = %d\n", len);
	for(i = 0; i < len; i++) {
		printf("buf[%d] = 0x%x\n", i, buf[i]);
	}

	if(len < 2)
		return -1;

	//ping
	if(len == 2) {
		ci_len = (short)(buf[0] | (buf[1] << 8));
		if(ci_len == 1) {
			send_buf[0] = 1;
			send_buf[1] = 0;
			vcp7g_dbg_send(dbg, send_buf, 2);
			return 0;
		}
		return -1;
	}

	//vcp version
	if(len == 4) {
		ci_len = (short)(buf[0] | (buf[1] << 8));
		if(ci_len == 2) {
			ci_opcode = (short)(buf[2] | (buf[3] << 8));
			if(ci_opcode == 0) {
				send_buf[0] = 0x02;
				send_buf[1] = 0x00;
				send_buf[2] = 0x33;
				send_buf[3] = 0x00;
				vcp7g_dbg_send(dbg, send_buf, 4);
				return 0;
			}
		}
		return -1;
	}

	//vcp apply profile
	if(len > 4) {
		ci_len = (short)(buf[0] | (buf[1] << 8));
		if((ci_len*2) <= len) {
			ci_opcode = (short)(buf[2] | (buf[3] << 8));
			if(ci_opcode == 1) {

				FILE * fp;
				char *profile_file = VCP_DBG_PROFILE_FILE;
				int ret;
				fp = fopen(profile_file, "wb");
				if(fp == NULL) {
					VCP_ERR("create %s fail\n", profile_file);
				}
				else {
					VCP_DBG(" writing to %s\n", profile_file);
					ret = fwrite(buf, sizeof(char), len, fp);
					if(ret != len) {
						VCP_ERR("write to %s fail: only write %d bytes, expected %d bytes\n", profile_file, ret, len);
						unlink(profile_file);
					}
					fclose(fp);
				}

				send_buf[0] = 0x04;
				send_buf[1] = 0x00;
				send_buf[2] = 0x00;
				send_buf[3] = 0x00;
				send_buf[4] = 0x00;
				send_buf[5] = 0x00;
				send_buf[6] = 0x00;
				send_buf[7] = 0x00;
				vcp7g_dbg_send(dbg, send_buf, 8);
				return (ci_len*2);
			}
		}

		return -1;
	}

}

int vcp7g_dbg_read_profile(struct vcp7g_dbg * dbg, char * buf, int len) {
	FILE * fp;
	char *profile_file = VCP_DBG_PROFILE_FILE;
	int ret, file_len;

	fp = fopen(profile_file, "rb");
	if(fp == NULL) {
		VCP_ERR("read profile: %s not exist\n", profile_file);
		return -1;
	}
	else {
		ret = fseek(fp, 0, SEEK_END);
		if(ret < 0) {
			perror("seeking failed:");
			goto err_out;
		}
		file_len = ftell(fp);
		if(len < file_len) {
			VCP_ERR(" buf is not big enough for %s\n", profile_file);
			goto err_out;
		}
		ret = fseek(fp, 0, SEEK_SET);
		if(ret < 0) {
			perror("seeking failed:");
			goto err_out;
		}

		VCP_DBG(" reading from %s\n", profile_file);
		ret = fread(buf, sizeof(char), file_len, fp);
		if(ret < file_len) {
			VCP_ERR("read from %s fail: only read %d bytes, expected %d bytes\n", profile_file, ret, file_len);
			goto err_out;
		}
		fclose(fp);
		return 0;
	}
err_out:
	fclose(fp);
	return -1;
}
