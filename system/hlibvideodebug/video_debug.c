/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: video debug system library
 *
 * Author:
 *     davinci <davinci.liang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.0  08/12/2017 init
 *
 */

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h> /* for opendir */
#include <dirent.h>

#include <time.h> /*for clock_gettime */

#include "video_debug.h"
#include <sys/shm.h>

//#define TIME_PERF_MEASURE_ENABLE 1

static video_dbg_info* g_video_dbg_info = NULL;
static video_dbg_user_info* g_video_dbg_user_info = NULL;
static video_dbg_parser g_parser_list;

static audio_dbg_info_t *g_audio_dbg_info = NULL;

int g_log_level = _LOG_INFO;

static int video_dbg_strtol(char* str, int base)
{
	char *endptr;
	int val = 0;

	errno = 0; /* To distinguish success/failure after call */
	val = strtol(str, &endptr, base);

	/* Check for various possible errors */
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		VDBG_LOG_ERR("strtol erron:%d %s\n", errno, strerror(errno));
	}

	if (endptr == str) {
		VDBG_LOG_ERR("No digits were found\n");
	}

	VDBG_LOG_DBG("strtol() returned %ld\n", val);
	return val;
}

static const char*get_log_level_name(int level)
{
	switch(level) {
	case _LOG_ERR:
		return "ERR";
	case _LOG_WARN:
		return "WARN";
	case _LOG_INFO:
		return "INFO";
	case _LOG_DBG:
		return "DBG";
	default:
		return "NONE";
	}
}
void video_dbg_log(int level, const char* func, const int line, const char * fromat, ...)
{
	char buf[BUF_LEN];
	va_list args;

	memset(buf, 0, BUF_LEN);

	if (level > g_log_level)
		return ;

	if (level < _LOG_WARN)
		snprintf(buf, BUF_LEN, "[VDBG][%s][%s:%d]",
			get_log_level_name(level),func, line);
	else {
		(void)func;
		(void)line;
		snprintf(buf, BUF_LEN, "[VDBG][%s]",
			get_log_level_name(level));
	}

	va_start(args, fromat);
	vsnprintf(buf + strlen(buf), BUF_LEN - strlen(buf), fromat, args);
	va_end(args);

	printf("%s", buf);
}

#ifdef TIME_PERF_MEASURE_ENABLE
static struct timespec g_tm_start = {0};
inline void perf_measure_start(struct timespec *start)
{
	clockid_t clk_id = CLOCK_REALTIME;
	if (!start) {
		clock_gettime(clk_id, &g_tm_start);
	} else {
		clock_gettime(clk_id, start);
	}
}

inline void perf_measure_stop(const char *title, struct timespec *start)
{
	int usec;
	double result_time;
	static struct timespec ts_end, diff;

	clockid_t clk_id = CLOCK_REALTIME;
	clock_gettime(clk_id, &ts_end);

	if (!start) {
		diff.tv_sec = (ts_end.tv_sec - g_tm_start.tv_sec);
		diff.tv_nsec = (ts_end.tv_nsec - g_tm_start.tv_nsec);
	} else {
		diff.tv_sec = (ts_end.tv_sec - start->tv_sec);
		diff.tv_nsec = (ts_end.tv_nsec - start->tv_nsec);
	}

	if (diff.tv_nsec < 0) {
		diff.tv_sec--;
		diff.tv_nsec += 1000000000;
	}

	usec = diff.tv_nsec + diff.tv_sec * 100000000;
	result_time = (double)usec;

	printf("[%s] used time %16.4lf[ns] %8.4lf[ms]\n", title,
		result_time, result_time/1000000);
}
#else
inline void perf_measure_start(struct timespec *start) {}
inline void perf_measure_stop(const char *title, struct timespec *start){}
#endif

int video_dbg_is_dir_exist(const char* dir_path)
{
	DIR *dirp = NULL;

	if (dir_path == NULL)
		return -1;

	dirp = opendir(dir_path);
	if (dirp != NULL) {
		closedir(dirp);
		return 0;
	}
	return -1;
}

int video_dbg_read_file(const char *path, char *rbuf, int *size) __malloc__
{
	int ret = 0;
	FILE * fp = NULL;
	long fsize = 0;
	size_t r = 0;

	fp = fopen(path, "rb");
	if (fp == NULL) {
		VDBG_LOG_ERR("can't open file: %s errno:%d %s\n", path, errno, strerror(errno));
		ret = -1;
		goto err;
	}

	fseek(fp , 0, SEEK_END);
	fsize = ftell(fp);
	rewind(fp);

	if (*size < fsize) {
		free(rbuf);
		rbuf = NULL;

		rbuf = (char*) malloc(sizeof(char)*fsize);
		if (rbuf == NULL) {
			VDBG_LOG_ERR("malloc failed errno:%d %s\n", errno, strerror(errno));
			ret = -1;
			goto err1;
		}
		*size = fsize;
	}

	r = fread(rbuf, 1, fsize, fp);
	if (r != fsize) {
		VDBG_LOG_ERR("read %s failed errno:%d %s\n", path, errno, strerror(errno));
		ret = -1;
		goto err1;
	}

	VDBG_LOG_DBG("%s\n", rbuf);

err1:
	fclose(fp);
err:
	return ret;
}

int video_dbg_write_file(const char *path, const char *wbuf, int size)
{
	int ret = 0;
	FILE *fp = NULL;
	size_t w = 0;

	fp = fopen(path, "wb");
	if (fp == NULL) {
		VDBG_LOG_ERR("can't open file: %s errno:%d %s\n", path, errno, strerror(errno));
		ret = -1;
		goto err;
	}

	w = fwrite(wbuf, 1, size, fp);
	if (w != size) {
		VDBG_LOG_ERR("write %s failed errno:%d %s\n", path, errno, strerror(errno));
		ret = -1;
		goto err1;
	}

	VDBG_LOG_DBG("%s\n", wbuf);

err1:
	fclose(fp);
err:
	return ret;
}

int video_dbg_exec_cmd(const char * cmd)
{
	VDBG_LOG_DBG("%s\n", cmd);

	FILE * fp = NULL;
	fp = popen(cmd, "r");
	if (!fp) {
		VDBG_LOG_ERR("popen errno:%d %s\n", errno, strerror(errno));
		return -1;
	}

	pclose(fp);
	return 0;
}

static int  video_dbg_exec_cmd_to_buf(const char *cmd,
	char *result, int *size)
{
	FILE *pp = NULL;
	int psize = 0;
	int maxsize = *size;

	char tmp[512] = {0};
	char*p = result;

	pp = popen(cmd, "r");
	if (!pp) {
		VDBG_LOG_ERR("popen erron:%d %s\n", errno, strerror(errno));
		return -1;
	}

	while (fgets(tmp, sizeof(tmp), pp) != NULL) {

		VDBG_LOG_DBG("%s\n", tmp);
		if (tmp[strlen(tmp) - 1] == '\n') {
			tmp[strlen(tmp) - 1] = '\0';
		}

		psize += strlen(tmp);
		if (psize < maxsize) {
			VDBG_LOG_DBG("%s %s %d\n", p+strlen(p), tmp, strlen(tmp));
			strncpy(p+strlen(p), tmp, strlen(tmp));
		} else {
			VDBG_LOG_WARN("cmd:%s saving result is too large size:%d\n", cmd, psize);
			break;
		}
	}

	*size = strlen(result);
	pclose(pp);

	return 0;
}

int video_dbg_exec_cmd_to_file(const char *cmd, char *savefile)
{
	int ret = 0;
	FILE *pp = NULL;
	FILE *fp = NULL;
	size_t w;

	char tmp[1024] = {0};

	pp = popen(cmd, "r");
	if (!pp) {
		VDBG_LOG_ERR("popen erron:%d %s\n", errno, strerror(errno));
		ret = -1;
		goto err;
	}

	/* It will replace context last writing */
	fp = fopen(savefile, "w+");
	if (!fp) {
		VDBG_LOG_ERR("fopen erron:%d %s\n", errno, strerror(errno));
		ret = -1;
		goto err1;
	}

	while (fgets(tmp, sizeof(tmp), pp) != NULL) {
#if 0
		if (tmp[strlen(tmp) - 1] == '\n') {
			tmp[strlen(tmp) - 1] = '\0';
		}
#endif
		w = fwrite(tmp, 1, strlen(tmp), fp);
		if (w != strlen(tmp)) {
			VDBG_LOG_ERR("write %s failed errno:%d %s\n",
				savefile, errno, strerror(errno));
			ret = -1;
			goto err2;
		}
	}

	fflush(fp);

err2:
	fclose(fp);
err1:
	pclose(pp);
err:
	return ret;
}

int video_dbg_parser_register(char* obj,
	line_parser_cb line_parser_f, parser_cb parser_f,
	int src, int dst, void* result, int size)
{
	int ret = 0;
	static int id = 0;

	VDBG_LOG_DBG("obj =%s \n",obj);

	video_dbg_parser* parser = (video_dbg_parser*) \
		malloc(sizeof(video_dbg_parser));

	if (!parser) {
		VDBG_LOG_ERR("malloc failed !");
		return -1;
	}

	strncpy(parser->obj, obj, 256);
	parser->id = id++;
	parser->result = result;
	parser->size = size;

	parser->line_parser_f = line_parser_f;
	parser->parser_f = parser_f;
	parser->src =src;
	parser->dst = dst;

	parser->exist = 1;
	if (parser->src == SRC_FROM_FILE) {
		ret = access(obj, F_OK);
		if (ret < 0) {
			parser->exist = 0;
			VDBG_LOG_DBG("file:%s can't access\n", parser->obj);
			free(parser);
			return -1;
		}
	}

	list_add_tail(parser, &g_parser_list);

	return 0;
}

static int common_line_parser(video_dbg_parser*parser, char *lbuf, int lsize)
{
	VDBG_LOG_DBG("lsize =%d lbuf=%s \n", lsize, lbuf);

	char *str1, *str2, *token, *subtoken;
	char *saveptr1, *saveptr2;
	int i, j;

	const char *delim_y = ",";
	const char *delim_x = ":=";

	char val[256] = {0};
	char param[256] = {0};

	for (j = 1, str1 = lbuf; ; j++, str1 = NULL) {
		token = strtok_r(str1, delim_y, &saveptr1);
		if (token == NULL)
			break;

		VDBG_LOG_DBG("%d: %s\n", j, token);

		for (str2 = token, i = 0; ; str2 = NULL,i++) {
			subtoken = strtok_r(str2, delim_x, &saveptr2);
			if (subtoken == NULL)
				break;

			VDBG_LOG_DBG(" --> %s\n", subtoken);

			if (i == 0) {
				strncpy(param, subtoken,sizeof(param));

				if (parser->dst == DST_VAR && parser->parser_f)
					parser->parser_f(parser, param);

			} else if (i == 1) {
				strncpy(val, subtoken, sizeof(val));

				if (parser->dst == DST_STRUCT && parser->line_parser_f)
					parser->line_parser_f(parser, param, val);

			} else {
				VDBG_LOG_ERR("%d Novaild Value %s\n", i,subtoken);
			}
		}
	}

	return 0;
}

static int video_dbg_file_parser_update(video_dbg_parser* parser)
{
	int ret = 0;
	char cmd[256] = {0};
	char tmp[1024] = {0};
	FILE *pp = NULL;

	snprintf(cmd, sizeof(cmd), "cat %s", parser->obj);

	if (!parser->exist) {
		VDBG_LOG_DBG("file:%s can't access\n", parser->obj);
		goto err;
	}

	pp = popen(cmd, "r");
	if (!pp) {
		VDBG_LOG_ERR("popen erron:%d %s\n", errno, strerror(errno));
		ret = -1;
		goto err;
	}

	while (fgets(tmp, sizeof(tmp), pp) != NULL) {

		VDBG_LOG_DBG("%s\n", tmp);
		if (tmp[strlen(tmp) - 1] == '\n') {
			tmp[strlen(tmp) - 1] = '\0';
		}
		common_line_parser(parser, tmp, strlen(tmp));
	}

	pclose(pp);

err:
	return ret;
}

static int video_dbg_cmd_parser_update(video_dbg_parser* parser)
{
	int ret = 0;
	char tmp[1024] = {0};
	FILE *pp = NULL;

	if (parser->dst != DST_NONE) {
		pp = popen(parser->obj, "r");
		if (!pp) {
			VDBG_LOG_ERR("popen erron:%d %s\n", errno, strerror(errno));
			ret = -1;
			goto err;
		}

		while (fgets(tmp, sizeof(tmp), pp) != NULL) {
			VDBG_LOG_DBG("%s\n", tmp);
			if (tmp[strlen(tmp) - 1] == '\n') {
				tmp[strlen(tmp) - 1] = '\0';
			}
			common_line_parser(parser, tmp, strlen(tmp));
		}

		pclose(pp);
	}else {
		ret = video_dbg_exec_cmd(parser->obj);
	}
err:
	return ret;
}

int video_dbg_api_parser_update(video_dbg_parser* parser)
{
	if (parser->parser_f)
		parser->parser_f(parser, parser->obj);
	return 0;
}

static void video_dbg_update_user_info(
	video_dbg_info* info, video_dbg_user_info* uinfo)
{
	int i = 0;
	uinfo->out_w = info->isp_info.s32OutW;
	uinfo->out_h = info->isp_info.s32OutH;

	uinfo->cap_w = info->isp_info.s32CapW;
	uinfo->cap_h = info->isp_info.s32CapH;

	uinfo->ctx_num = info->isp_info.s32CtxN;

	for (i = 0; i < uinfo->ctx_num; i++) {
		uinfo->awb_en[i] = info->isp_info.s32AWBEn[i];
		uinfo->ae_en[i] = info->isp_info.s32AWBEn[i];
		uinfo->hw_awb_en[i] = info->isp_info.s32HWAWBEn[i];
		uinfo->tnm_static_curve[i] = info->isp_info.s32TNMStaticCurve[i];

		uinfo->day_mode[i] = info->isp_info.s32DayMode[i];
		uinfo->mirror_mode[i] = info->isp_info.s32MirrorMode[i];

		uinfo->fps[i] = info->isp_int.int_per_sec[i];
	}

	uinfo->isp_freq = info->base.clk_rate/1000000;
	uinfo->ddr_freq = info->ddr.clk_rate;

	if (info->cpu_type != CPU_TYPE_APOLLO) {
		if (info->ddr.port_wpriority_en > 0)
			uinfo->isp_ddr_wpriority = 1;
		else
			uinfo->isp_ddr_wpriority = 0;

		if (info->ddr.port_rpriority_en > 0)
			uinfo->isp_ddr_rpriority = 1;
		else
			uinfo->isp_ddr_rpriority = 0;
	} else {
		if (info->ddr.port2_wpriority > 0xc000)
			uinfo->isp_ddr_wpriority = 1;
		else
			uinfo->isp_ddr_wpriority = 0;

		if (info->ddr.port2_rpriority > 0xc000)
			uinfo->isp_ddr_rpriority = 1;
		else
			uinfo->isp_ddr_rpriority = 0;
	}

	uinfo->ispost_dn_en = info->ispost.dn.stat;

	uinfo->used_max_mem = info->mem.used_max_mem/1024;
	uinfo->used_mem = info->mem.used_mem/1024;

	memcpy(&uinfo->vinfo, &info->vinfo, sizeof(info->vinfo));
}

int video_dbg_update(void)
{
	struct timespec start;
	if (!g_video_dbg_info && !g_video_dbg_user_info)
		return -1;

	perf_measure_start(&start);
	video_dbg_parser* parser;
	list_for_each (parser, g_parser_list) {
		perf_measure_start(NULL);
		switch(parser->src) {
			case SRC_FROM_FILE:
				video_dbg_file_parser_update(parser);
				break;
			case SRC_FROM_API:
				video_dbg_api_parser_update(parser);
				break;
			case SRC_FROM_CMD:
				video_dbg_cmd_parser_update(parser);
				break;
			default:
				break;
		}
		perf_measure_stop(parser->obj, NULL);
	}

	video_dbg_update_user_info(g_video_dbg_info, g_video_dbg_user_info);
	perf_measure_stop(__func__, &start);
	return 0;
}

video_dbg_info* video_dbg_get_info(void)
{
	return g_video_dbg_info;
}

video_dbg_user_info* video_dbg_get_user_info(void)
{
	return g_video_dbg_user_info;
}

static int video_dbg_check_path(void)
{
	int ret = 0;
	int isp_ret = 0, ispost_ret = 0;

	isp_ret = video_dbg_is_dir_exist(ISP_DEBUGFS_DIR_PATH);
	if (isp_ret< 0)
		VDBG_LOG_ERR(" %s is not exist",ISP_DEBUGFS_DIR_PATH);

	ispost_ret = video_dbg_is_dir_exist(ISPOST_DEBUGFS_DIR_PATH);
	if (ispost_ret < 0)
		VDBG_LOG_ERR(" %s is not exist",ISP_DEBUGFS_DIR_PATH);

	if (isp_ret < 0 ||ispost_ret < 0)
		ret = video_dbg_exec_cmd(DEBUGFS_MOUNT_CMD);

	return ret;
}

void gasket_info_lparser_cb(
	video_dbg_parser *parser, char* param, char* val)
{
	int k = 0;
	char param_r[256] = {0};

	gasket_info* gasket = (gasket_info* )parser->result;

	for (k = 0; k < CI_N_IMAGERS; k++) {
		snprintf(param_r, sizeof(param_r), NGAKET_STATUS, k);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			gasket[k].enable = video_dbg_strtol(val,10);
			gasket[k].exist = 1;
			break;
		}

		snprintf(param_r, sizeof(param_r), NGAKET_TYPE, k);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			strncpy(gasket[k].type, val, 32);
			break;
		}

		snprintf(param_r, sizeof(param_r), NGAKET_FRAME_COUNT, k);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			gasket[k].frame_cnt =  video_dbg_strtol(val,10);
			break;
		}

		snprintf(param_r, sizeof(param_r), NGAKET_MIPI_FIFO_FULL, k);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			gasket[k].mipi_fifo_full= video_dbg_strtol(val,10);
			break;
		}

		snprintf(param_r, sizeof(param_r), NGAKET_MIPI_ENABLE_LANES, k);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			gasket[k].mipi_en_lanes= video_dbg_strtol(val,10);
			break;
		}

		snprintf(param_r, sizeof(param_r), NGAKET_MIPI_CRC_ERROR, k);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			gasket[k].mipi_crc_err = video_dbg_strtol(val,10);
			break;
		}

		snprintf(param_r, sizeof(param_r), NGAKET_MIPI_HDR_ERROR, k);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			gasket[k].mipi_hdr_err=  video_dbg_strtol(val,10);
			break;
		}

		snprintf(param_r, sizeof(param_r), NGAKET_MIPI_ECC_ERROR, k);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			gasket[k].mipi_ecc_err= video_dbg_strtol(val,10);
			break;
		}

		snprintf(param_r, sizeof(param_r), NGAKET_MIPI_ECC_CORRECTED, k);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			gasket[k].mipi_ecc_corr=  video_dbg_strtol(val,10);
			break;
		}
	}
}

void rtm_info_lparser_cb(
	video_dbg_parser *parser, char* param, char* val)
{
	int c, i;
	char param_r[256] = {0};

	VDBG_LOG_DBG("param =%s val=%s \n", param, val);

	rtm_ctx_info* rtm = (rtm_ctx_info* )parser->result;

	for (i = 0; i < FELIX_MAX_RTM_VALUES; i++) {
		snprintf(param_r, sizeof(param_r), RTM_NCORE, i);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			rtm->entries[i] = video_dbg_strtol(val,16);
			break;
		}

		for (c = 0; c < CI_N_CONTEXT; c++) {
			snprintf(param_r, sizeof(param_r), RTM_NCTX_NPOS,c, i);
			VDBG_LOG_DBG("%s\n", param_r);
			if (strncmp(param_r, param, sizeof(param_r)) == 0) {
				rtm->ctx_position[c][i] = video_dbg_strtol(val,16);
				break;
			}
		}
	}

	for (c = 0; c < CI_N_CONTEXT; c++) {
		snprintf(param_r, sizeof(param_r), RTM_NCTX_STATUS, c);
		VDBG_LOG_DBG("%s\n", param_r);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			rtm->ctx_status[c] = video_dbg_strtol(val,16);
			rtm->exist[c] = 1;
			break;
		}

		snprintf(param_r, sizeof(param_r), RTM_NCTX_LINKEDLISTEMPTYNESS, c);
		VDBG_LOG_DBG("%s\n", param_r);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			rtm->ctx_link_emptyness[c] = video_dbg_strtol(val,16);
			break;
		}
	}
}

void int_per_sec_lparser_cb(
	video_dbg_parser *parser, char* param, char* val)
{
	int c;
	char param_r[256] = {0};

	VDBG_LOG_DBG("param =%s val=%s \n", param, val);

	interrupt_info* isp_int = (interrupt_info*)parser->result;

	for (c = 0; c < CI_N_CONTEXT; c++) {
		snprintf(param_r, sizeof(param_r), N_CTX_INT, c);
		VDBG_LOG_DBG("%s\n", param_r);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			isp_int->ctx_int[c] = video_dbg_strtol(val,10);
			isp_int->exist[c] = 1;
			break;
		}

		snprintf(param_r, sizeof(param_r), N_CTX_IPS, c);
		VDBG_LOG_DBG("%s\n", param_r);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			isp_int->int_per_sec[c] = video_dbg_strtol(val,10);
			break;
		}
	}
}

void hwlist_lparser_cb(
	video_dbg_parser *parser, char* param, char* val)
{
	int c;
	char param_r[256] = {0};

	VDBG_LOG_DBG("param =%s val=%s \n", param, val);

	hwlist_info* hwlist = (hwlist_info*)parser->result;

	for (c = 0; c < CI_N_CONTEXT; c++) {
		snprintf(param_r, sizeof(param_r), N_CTX_AVAILABLE, c);
		VDBG_LOG_DBG("%s\n", param_r);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			hwlist[c].available_elements = video_dbg_strtol(val,10);
			hwlist[c].exist = 1;
			break;
		}

		snprintf(param_r, sizeof(param_r), N_CTX_PENDING, c);
		VDBG_LOG_DBG("%s\n", param_r);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			hwlist[c].pending_elements = video_dbg_strtol(val,10);
			break;
		}

		snprintf(param_r, sizeof(param_r), N_CTX_PROCESSED, c);
		VDBG_LOG_DBG("%s\n", param_r);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			hwlist[c].processed_elements = video_dbg_strtol(val,10);
			break;
		}

		snprintf(param_r, sizeof(param_r), N_CTX_SEND, c);
		VDBG_LOG_DBG("%s\n", param_r);
		if (strncmp(param_r, param, sizeof(param_r)) == 0) {
			hwlist[c].send_elements = video_dbg_strtol(val,10);
			break;
		}
	}
}

void efuse_lparser_cb(
	video_dbg_parser *parser, char* param, char* val)
{
	char param_r[256] = {0};

	VDBG_LOG_DBG("param =%s val=%s \n", param, val);

	chip_efuse_info* efuse = (chip_efuse_info*)parser->result;

	snprintf(param_r, sizeof(param_r), CHIP_EFUSE_TYPE);
	VDBG_LOG_DBG("%s\n", param_r);
	if (strncmp(param_r, param, sizeof(param_r)) == 0) {
		efuse->type= video_dbg_strtol(val,16);
		efuse->exist = 1;
		return ;
	}

	snprintf(param_r, sizeof(param_r), CHIP_EFUSE_MAC);
	VDBG_LOG_DBG("%s\n", param_r);
	if (strncmp(param_r, param, sizeof(param_r)) == 0) {
		efuse->mac = video_dbg_strtol(val,16);
		return ;
	}
}

void integer_dec_parser_cb(video_dbg_parser*parser, char *val)
{
	*((int*)parser->result) = video_dbg_strtol(val, 10);
}

void integer_hex_parser_cb(video_dbg_parser*parser, char *val)
{
	*((int*)parser->result) = video_dbg_strtol(val, 16);
}

int media_get_debug_info(const char *event, const char *ps8Type, 
		const char *ps8SubType, void *pInfo, const int s32MemSize)
{
	ST_SHM_INFO stShmInfo;
	struct event_scatter stEventScatter;
	int s32Ret = 0;
	int s32Key = 0;
	int s32ShmId = 0;
	int s32ShmSize = 0;
	char *pMap = NULL;
	char ps8ShmPath[32];

	snprintf(ps8ShmPath, sizeof(ps8ShmPath), "/proc/%d", getpid());
	strncpy(stShmInfo.ps8Type, ps8Type, TYPE_LENGTH);
	strncpy(stShmInfo.ps8SubType, ps8SubType, SUB_TYPE_LENGTH);
	s32ShmSize = s32MemSize;

	s32Key = ftok(ps8ShmPath, 0x05);

	if (s32Key == -1) {
		printf("(%s:L%d) ftok is error (path_name=%s) errono:%s!!!\n",
			__func__, __LINE__, ps8ShmPath, strerror(errno));
		return -1;
	}

	s32ShmId = shmget(s32Key, s32ShmSize, IPC_CREAT | IPC_EXCL | 0666);
	if (s32ShmId == -1) {
		s32ShmId = shmget(s32Key, s32ShmSize, 0666);
		if (s32ShmId == -1) {
			printf("(%s:L%d) shmget is error (key=%d, size=%d) errono:%s!!!\n",
				__func__, __LINE__, s32Key, s32ShmSize, strerror(errno));
			return -1;
		}
	}

	pMap = shmat(s32ShmId, NULL, 0);
	if ((int)pMap == -1) {
		printf("(%s:L%d) p_map is NULL (shm_id=%d) errono:%s !!!\n",
			__func__, __LINE__, s32ShmId, strerror(errno));
		return -1;
	} else {
		memcpy(pMap, (char *)pInfo, s32ShmSize);
	}

	stShmInfo.s32Key = s32Key;
	stShmInfo.s32Size = s32ShmSize;

	stEventScatter.buf = &stShmInfo;
	stEventScatter.len = sizeof(stShmInfo);
	s32Ret = event_rpc_call_scatter(event, &stEventScatter, 1);
	if (s32Ret < 0) {
		printf("(%s:L%d) get debug info fail\n", __func__, __LINE__);
		s32Ret = -1;
	} else {
		memcpy(pInfo, pMap, s32ShmSize);
	}

	if (shmdt(pMap) == -1) {
		printf("(%s:L%d) shmdt error (p_map=%p) !!!\n", __func__, __LINE__, pMap);
		s32Ret = -1;
	}

	if (shmctl(s32ShmId, IPC_RMID, NULL) == -1) {
		printf("(%s:L%d) shmctl error (shm_id=%d) !!!\n", __func__, __LINE__, s32ShmId);
		s32Ret = -1;
	}

	return s32Ret;
}

void video_get_debug_info_parser_cb(
	video_dbg_parser *parser, char *val)
{
	int ret = 0;
	(void)val;
	ret = media_get_debug_info(VB_RPC_INFO, "ipu_info",
			parser->obj, parser->result, parser->size);
	if (ret < 0) {
		VDBG_LOG_ERR("mediabox_get_debug_info has called failed\n");
	}
}

void audio_get_debug_info_parser_cb(
	video_dbg_parser *parser, char *val)
{
	int ret = 0;
	(void)val;
	ret = media_get_debug_info(AB_RPC_DEBUG_INFO, "audio_info",
			parser->obj, parser->result, parser->size);
	if (ret < 0) {
		VDBG_LOG_ERR("mediabox_get_debug_info has called failed\n");
	}
}

void ispost_dn_status_lparser_cb(
	video_dbg_parser *parser, char* param, char* val)
{
	char param_r[256] = {0};

	VDBG_LOG_DBG("param =%s val=%s \n", param, val);
	ispost_dn_info* dn_info = (ispost_dn_info*)parser->result;

	snprintf(param_r, sizeof(param_r), DN_STAT);
	VDBG_LOG_DBG("%s\n", param_r);
	if (strncmp(param_r, param, sizeof(param_r)) == 0) {
		dn_info->stat= video_dbg_strtol(val,10);
		return ;
	}

	snprintf(param_r, sizeof(param_r), DN_EN);
	VDBG_LOG_DBG("%s\n", param_r);
	if (strncmp(param_r, param, sizeof(param_r)) == 0) {
		dn_info->en_dn= video_dbg_strtol(val,10);
		return ;
	}

	snprintf(param_r, sizeof(param_r), DN_REF_Y);
	VDBG_LOG_DBG("%s\n", param_r);
	if (strncmp(param_r, param, sizeof(param_r)) == 0) {
		dn_info->ref_y_addr= video_dbg_strtol(val,16);
		return ;
	}
}

void ispost_hw_status_lparser_cb(
	video_dbg_parser *parser, char* param, char* val)
{
	char param_r[256] = {0};

	VDBG_LOG_DBG("param =%s val=%s \n", param, val);
	ispost_hw_status* status = (ispost_hw_status*)parser->result;

	snprintf(param_r, sizeof(param_r), ISPOST_COUNT);
	VDBG_LOG_DBG("%s\n", param_r);
	if (strncmp(param_r, param, sizeof(param_r)) == 0) {
		status->ispost_count = video_dbg_strtol(val,10);
		return ;
	}

	snprintf(param_r, sizeof(param_r), ISPOST_USER_CTRL0);
	VDBG_LOG_DBG("%s\n", param_r);
	if (strncmp(param_r, param, sizeof(param_r)) == 0) {
		status->user_ctrl0 = video_dbg_strtol(val,10);
		return ;
	}

	snprintf(param_r, sizeof(param_r), ISPOST_CTRL0);
	VDBG_LOG_DBG("%s\n", param_r);
	if (strncmp(param_r, param, sizeof(param_r)) == 0) {
		status->ctrl0 = video_dbg_strtol(val,16);
		return ;
	}

	snprintf(param_r, sizeof(param_r), ISPOST_STAT0);
	VDBG_LOG_DBG("%s\n", param_r);
	if (strncmp(param_r, param, sizeof(param_r)) == 0) {
		status->stat0 = video_dbg_strtol(val,16);
		return ;
	}
}

int video_dbg_csi_reg_parser_init(video_dbg_info* info)
{
	int ret = 0;
	char temp[512] = {0};
	int size = sizeof(temp);

	ret = video_dbg_exec_cmd_to_buf(SENSOR0_INTERFACE_TYPE_CMD, temp, &size);
	if (strncmp(temp, INTERFACE_MIPI, size) == 0) {
		ret = video_dbg_parser_register(CSI0_DEMP_FILE_CMD,
					NULL,
					NULL,
					SRC_FROM_CMD, DST_NONE,
					NULL, 0);
	} else {
		ret = video_dbg_exec_cmd_to_buf(
			SENSOR1_INTERFACE_TYPE_CMD, temp, &size);

		if (strncmp(temp, INTERFACE_MIPI, size) == 0) {
			ret = video_dbg_parser_register(CSI1_DEMP_FILE_CMD,
					NULL,
					NULL,
					SRC_FROM_CMD, DST_NONE,
					NULL, 0);
		} else {
			return 0;
		}
	}

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_VERSION_REG);
	ret = video_dbg_parser_register(temp,
				NULL,
				integer_hex_parser_cb,
				SRC_FROM_CMD, DST_VAR,
				&(info->csi.version), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_N_LANES_REG);
	ret = video_dbg_parser_register(temp,
				NULL,
				integer_hex_parser_cb,
				SRC_FROM_CMD, DST_VAR,
				&(info->csi.n_lanes), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_DPHY_SHUTDOWN_REG);
	ret = video_dbg_parser_register(temp,
				NULL,
				integer_hex_parser_cb,
				SRC_FROM_CMD, DST_VAR,
				&(info->csi.phy_shutdownz), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_DPHYRST_REG);
	ret = video_dbg_parser_register(temp,
				NULL,
				integer_hex_parser_cb,
				SRC_FROM_CMD, DST_VAR,
				&(info->csi.dphy_rstz), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_RESET_REG);
	ret = video_dbg_parser_register(temp,
				NULL,
				integer_hex_parser_cb,
				SRC_FROM_CMD, DST_VAR,
				&(info->csi.csi2_resetn), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_DPHY_STATE_REG);
	ret = video_dbg_parser_register(temp,
				NULL,
				integer_hex_parser_cb,
				SRC_FROM_CMD, DST_VAR,
				&(info->csi.phy_state), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_DATA_IDS_1_REG);
	ret = video_dbg_parser_register(temp,
				NULL,
				integer_hex_parser_cb,
				SRC_FROM_CMD, DST_VAR,
				&(info->csi.data_ids_1), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_DATA_IDS_2_REG);
	ret = video_dbg_parser_register(temp,
				NULL,
				integer_hex_parser_cb,
				SRC_FROM_CMD, DST_VAR,
				&(info->csi.data_ids_2), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_ERR1_REG);
	ret = video_dbg_parser_register(temp,
				NULL,
				integer_hex_parser_cb,
				SRC_FROM_CMD, DST_VAR,
				&(info->csi.err1), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_ERR2_REG);
	ret = video_dbg_parser_register(temp,
			NULL,
			integer_hex_parser_cb,
			SRC_FROM_CMD, DST_VAR,
			&(info->csi.err2), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_MASK1_REG);
	ret = video_dbg_parser_register(temp,
			NULL,
			integer_hex_parser_cb,
			SRC_FROM_CMD, DST_VAR,
			&(info->csi.mark1), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_MASK2_REG);
	ret = video_dbg_parser_register(temp,
			NULL,
			integer_hex_parser_cb,
			SRC_FROM_CMD, DST_VAR,
			&(info->csi.mark2), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_DPHY_TST_CRTL0_REG);
	ret = video_dbg_parser_register(temp,
			NULL,
			integer_hex_parser_cb,
			SRC_FROM_CMD, DST_VAR,
			&(info->csi.phy_test_ctrl0), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSI2_DPHY_TST_CRTL1_REG);
	ret = video_dbg_parser_register(temp,
			NULL,
			integer_hex_parser_cb,
			SRC_FROM_CMD, DST_VAR,
			&(info->csi.phy_test_ctrl1), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSIDMAREG_FRAMESIZE);
	ret = video_dbg_parser_register(temp,
			NULL,
			integer_hex_parser_cb,
			SRC_FROM_CMD, DST_VAR,
			&(info->csi.img_size), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSIDMAREG_DATATYPE);
	ret = video_dbg_parser_register(temp,
			NULL,
			integer_hex_parser_cb,
			SRC_FROM_CMD, DST_VAR,
			&(info->csi.mipi_data_type), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSIDMAREG_VSYNCCNT);
	ret = video_dbg_parser_register(temp,
			NULL,
			integer_hex_parser_cb,
			SRC_FROM_CMD, DST_VAR,
			&(info->csi.vsync_cnt), 0);

	memset(temp, 0, sizeof(temp));
	snprintf(temp, sizeof(temp), CSI0_REG_N_OFFSET_CMD, CSIDMAREG_HBLANK);
	ret = video_dbg_parser_register(temp,
			NULL,
			integer_hex_parser_cb,
			SRC_FROM_CMD, DST_VAR,
			&(info->csi.hblank), 0);

	return ret;
}

int video_dbg_ispost_parser_init(video_dbg_info* info)
{
	int ret = 0;

	if (info->mode == VIDEO_DEBUG_PE_MODE ||
		info->mode == VIDEO_DEBUG_ISP_PE_MODE) {
		ret = video_dbg_parser_register(ISPOST_HW_VSERION,
				NULL,
				integer_hex_parser_cb,
				SRC_FROM_FILE, DST_VAR,
				&(info->ispost.hw_version), 0);

		ret = video_dbg_parser_register(ISPOST_HW_STATUS,
			ispost_hw_status_lparser_cb,
			NULL,
			SRC_FROM_FILE, DST_STRUCT,
			&(info->ispost.hw_status), 0);
	}

	ret = video_dbg_parser_register(ISPOST_DN_STATUS,
			ispost_dn_status_lparser_cb,
			NULL,
			SRC_FROM_FILE, DST_STRUCT,
			&(info->ispost.dn), 0);

	return ret;
}

int video_dbg_isp_user_parser_init(video_dbg_info* info)
{
	int ret = 0;

	/*Get IntPerSec at first*/
	ret = video_dbg_parser_register(INT_PER_SEC_FILE,
			int_per_sec_lparser_cb,
			NULL,
			SRC_FROM_FILE, DST_STRUCT,
			&(info->isp_int), 0);

	ret = video_dbg_parser_register(ISP_CLKRETE_FILE,
			NULL,
			integer_dec_parser_cb,
			SRC_FROM_FILE, DST_VAR,
			&(info->base.clk_rate), 0);
	if (ret < 0) {
		ret = video_dbg_parser_register(ISP_V2500_CLKRATE_FILE,
				NULL,
				integer_dec_parser_cb,
				SRC_FROM_FILE, DST_VAR,
				&(info->base.clk_rate), 0);
	}

	//TODO, Get ISP channel name
	ret = video_dbg_parser_register("isp",
			NULL,
			video_get_debug_info_parser_cb,
			SRC_FROM_API, DST_STRUCT,
			&(info->isp_info), sizeof(info->isp_info));

	ret = video_dbg_parser_register(DEV_MEM_USED_FILE,
		NULL,
		integer_dec_parser_cb,
		SRC_FROM_FILE, DST_VAR,
		&(info->mem.used_mem), 0);

	ret = video_dbg_parser_register(DEV_MEM_MAX_USED_FILE,
			NULL,
			integer_dec_parser_cb,
			SRC_FROM_FILE, DST_VAR,
			&(info->mem.used_max_mem), 0);

	/*Get IntPerSec at second*/
	ret = video_dbg_parser_register(INT_PER_SEC_FILE,
			int_per_sec_lparser_cb,
			NULL,
			SRC_FROM_FILE, DST_STRUCT,
			&(info->isp_int), 0);

	return ret;
}

int video_dbg_isp_parser_init(video_dbg_info* info)
{
	int i = 0;
	int ret = 0;
	char temp[256] = {0};

	ret = video_dbg_parser_register(ACTIVE_GASKET_INFO_FILE,
			gasket_info_lparser_cb,
			NULL,
			SRC_FROM_FILE, DST_STRUCT,
			&(info->gasket), 0);

	ret = video_dbg_parser_register(ISP_CLKRETE_FILE,
			NULL,
			integer_dec_parser_cb,
			SRC_FROM_FILE, DST_VAR,
			&(info->base.clk_rate), 0);
	if (ret < 0) {
		ret = video_dbg_parser_register(ISP_V2500_CLKRATE_FILE,
				NULL,
				integer_dec_parser_cb,
				SRC_FROM_FILE, DST_VAR,
				&(info->base.clk_rate), 0);
	}

	ret = video_dbg_parser_register(RTM_INFO_FILE,
			rtm_info_lparser_cb,
			NULL,
			SRC_FROM_FILE, DST_STRUCT,
			&(info->rtm_ctx), 0);

	//TODO, Get ISP channel name
	ret = video_dbg_parser_register("isp",
			NULL,
			video_get_debug_info_parser_cb,
			SRC_FROM_API, DST_STRUCT,
			&(info->isp_info), sizeof(info->isp_info));

	for (i = 0; i < CI_N_CONTEXT; i++) {
		snprintf(temp, sizeof(temp), DRIVER_NCTX_ACTIVE_FILE, i);
		ret = video_dbg_parser_register(temp,
				NULL,
				integer_dec_parser_cb,
				SRC_FROM_FILE, DST_VAR,
				&(info->isp_ctx_int[i].active), 0);

		snprintf(temp, sizeof(temp), DRIVER_NCTX_INT_FILE, i);
		ret = video_dbg_parser_register(temp,
				NULL,
				integer_dec_parser_cb,
				SRC_FROM_FILE, DST_VAR,
				&(info->isp_ctx_int[i].int_cnt), 0);

		snprintf(temp, sizeof(temp), DRIVER_NCTX_INT_DONE_FILE, i);
		ret = video_dbg_parser_register(temp,
				NULL,
				integer_dec_parser_cb,
				SRC_FROM_FILE, DST_VAR,
				&(info->isp_ctx_int[i].done_all_cnt), 0);

		snprintf(temp, sizeof(temp), DRIVER_NCTX_INT_IGNORE_FILE, i);
		ret = video_dbg_parser_register(temp,
				NULL,
				integer_dec_parser_cb,
				SRC_FROM_FILE, DST_VAR,
				&(info->isp_ctx_int[i].err_ignore_cnt), 0);

		snprintf(temp, sizeof(temp), DRIVER_NCTX_INT_START_FILE, i);
		ret = video_dbg_parser_register(temp,
				NULL,
				integer_dec_parser_cb,
				SRC_FROM_FILE, DST_VAR,
				&(info->isp_ctx_int[i].start_of_frame_cnt), 0);

		snprintf(temp, sizeof(temp), DRIVER_NCTX_TRIGGERED_HW_FILE, i);
		ret = video_dbg_parser_register(temp,
				NULL,
				integer_dec_parser_cb,
				SRC_FROM_FILE, DST_VAR,
				&(info->isp_ctx_int[i].hard_cnt), 0);

		snprintf(temp, sizeof(temp), DRIVER_NCTX_TRIGGERED_SW_FILE, i);
		ret = video_dbg_parser_register(temp,
				NULL,
				integer_dec_parser_cb,
				SRC_FROM_FILE, DST_VAR,
				&(info->isp_ctx_int[i].thread_cnt), 0);

		if (ret < 0)
			info->isp_ctx_int[i].exist = 0;
		else
			info->isp_ctx_int[i].exist = 1;
	}

	/*Get IntPerSec at first*/
	ret = video_dbg_parser_register(INT_PER_SEC_FILE,
			int_per_sec_lparser_cb,
			NULL,
			SRC_FROM_FILE, DST_STRUCT,
			&(info->isp_int), 0);

	ret = video_dbg_parser_register(DRIVER_NSERVICED_HARD_INT_FILE,
			NULL,
			integer_dec_parser_cb,
			SRC_FROM_FILE, DST_VAR,
			&(info->isp_int.serviced_hard_int), 0);

	ret = video_dbg_parser_register(DRIVER_NSERVICED_THREAD_INT_FILE,
			NULL,
			integer_dec_parser_cb,
			SRC_FROM_FILE, DST_VAR,
			&(info->isp_int.serviced_thread_int), 0);

	ret = video_dbg_parser_register(DRIVER_LONGEST_HARD_INIT_US_FILE,
			NULL,
			integer_dec_parser_cb,
			SRC_FROM_FILE, DST_VAR,
			&(info->isp_int.longest_hard_us), 0);

	ret = video_dbg_parser_register(DRIVER_LONGEST_THREAD_INIT_US_FILE,
			NULL,
			integer_dec_parser_cb,
			SRC_FROM_FILE, DST_VAR,
			&(info->isp_int.longest_thread_us), 0);

	ret = video_dbg_parser_register(DRIVER_N_CONNECTIONS_FILE,
			NULL,
			integer_dec_parser_cb,
			SRC_FROM_FILE, DST_VAR,
			&(info->isp_int.connections), 0);

	ret = video_dbg_parser_register(DEV_MEM_MAX_USED_FILE,
			NULL,
			integer_dec_parser_cb,
			SRC_FROM_FILE, DST_VAR,
			&(info->mem.used_max_mem), 0);

	ret = video_dbg_parser_register(DEV_MEM_USED_FILE,
			NULL,
			integer_dec_parser_cb,
			SRC_FROM_FILE, DST_VAR,
			&(info->mem.used_mem), 0);

	ret = video_dbg_parser_register(HW_LIST_ELEMENTS_FILE,
			hwlist_lparser_cb,
			NULL,
			SRC_FROM_FILE, DST_STRUCT,
			&(info->hwlist), 0);

	/*Get IntPerSec at second*/
	ret = video_dbg_parser_register(INT_PER_SEC_FILE,
			int_per_sec_lparser_cb,
			NULL,
			SRC_FROM_FILE, DST_STRUCT,
			&(info->isp_int), 0);

	return ret;
}

int video_dbg_ddr_parser_init(video_dbg_info* info)
{
	int ret = 0;

	if (info->mode == VIDEO_DEBUG_PE_MODE ||
		info->mode == VIDEO_DEBUG_ISP_PE_MODE) {
		ret = video_dbg_parser_register(DDR_FREQ_CMD,
				NULL,
				integer_dec_parser_cb,
				SRC_FROM_CMD, DST_VAR,
				&(info->ddr.clk_rate), 0);
	}

	if (info->cpu_type != CPU_TYPE_APOLLO) {
		ret = video_dbg_parser_register(DDR_PORT_R_PRIORITY_EN_CMD,
				NULL,
				integer_hex_parser_cb,
				SRC_FROM_CMD, DST_VAR,
				&(info->ddr.port_rpriority_en), 0);

		ret = video_dbg_parser_register(DDR_PORT_W_PRIORITY_EN_CMD,
				NULL,
				integer_hex_parser_cb,
				SRC_FROM_CMD, DST_VAR,
				&(info->ddr.port_wpriority_en), 0);
	}

	ret = video_dbg_parser_register(DDR_PORT2_R_PRIORITY_CMD,
			NULL,
			integer_hex_parser_cb,
			SRC_FROM_CMD, DST_VAR,
			&(info->ddr.port2_rpriority), 0);

	ret = video_dbg_parser_register(DDR_PORT2_W_PRIORITY_CMD,
			NULL,
			integer_hex_parser_cb,
			SRC_FROM_CMD, DST_VAR,
			&(info->ddr.port2_wpriority), 0);

	return ret;
}

int video_dbg_efuse_parser_init(video_dbg_info* info)
{
	int ret;
	ret = video_dbg_parser_register(CHIP_EFUSE_INFO,
			efuse_lparser_cb,
			NULL,
			SRC_FROM_FILE, DST_STRUCT,
			&(info->efuse), 0);
	return ret;
}

int video_dbg_venc_parser_init(video_dbg_info* info)
{
	int ret = 0;

	info->vinfo.s8Capcity = ARRAY_SIZE;
	ret = video_dbg_parser_register("venc",
			NULL,
			video_get_debug_info_parser_cb,
			SRC_FROM_API, DST_STRUCT,
			&(info->vinfo), sizeof(info->vinfo));

	return ret;
}

static const char *get_meta_type(EN_IPU_TYPE stType)
{
    switch(stType)
    {
        case IPU_H264:
            return "H264";
        case IPU_H265:
            return "H265";
        case IPU_JPEG:
            return "JPEG";
        default:
            return "N/A";
    }
}

static const char *get_meta_state(EN_CHANNEL_STATE stState)
{
	switch(stState) {
	case INIT:
		return "INIT";
	case RUN:
		return "RUNNING";
	case STOP:
		return "STOPED";
	case IDLE:
		return "IDLE";
	default:
		return "N/A";
	}
}

static const char *get_meta_rc_mode(EN_RC_MODE stRCMode)
{
	switch (stRCMode) {
	case VBR:
		return "VBR";
	case CBR:
		return "CBR";
	case FIXQP:
		return "FIXQP";
	default:
		return "N/A";
	}
}

static const char *get_meta_realqp(int s32QP)
{
	static char ps8Buf[10];

	if (s32QP > 0) {
		sprintf(ps8Buf, "%d", s32QP);
		return ps8Buf;
	} else {
		return "N/A";
	}
}

static const char *get_module_enable(int able)
{
	switch (able) {
	case 1:
		return "Y";
	case 0:
		return "N";
	default:
		return "N/A";
	}
}

static const char *get_mem_priority(int priority)
{
	switch (priority) {
	case 0:
		return "Low";
	case 1:
		return "High";
	default:
		return "N/A";
	}
}

static const char *get_mirror_mode(int mirror)
{
	switch (mirror) {
	case CAM_MIRROR_NONE:
		return "None";
	case CAM_MIRROR_H:
		return "H";
	case CAM_MIRROR_V:
		return "V";
	case CAM_MIRROR_HV:
		return "HV";
	default:
		return "N/A";
	}
}

static const char *get_day_mode(int day)
{
	switch (day) {
	case 0:
		return "Night";
	case 1:
		return "Day";
	default:
		return "N/A";
	}
}

static const char *get_tnm_curve_mode(int tnm_curve)
{
	switch (tnm_curve) {
	case 0:
		return "Static";
	case 1:
		return "Dynamic";
	default:
		return "N/A";
	}
}

/**
  *Note: the function sets called count don't beyond 10 by printf,
  * in order to code seen artistic but hasn't efficient.
  */
static const char *get_freq_mhz_string(int freq)
{
	static int i = 0;
	static char buf[10][12] = {{0}};
	i = (i+1)%10;

	sprintf(buf[i], "%dMhz", freq);
	//printf("%s i:%d,%s\n", __func__, i, buf[i]);
	return buf[i];
}

static const char *get_size_kb_string(int size)
{
	static int i = 0;
	static char buf[10][32] =  {{0}};
	i = (i+1)%10;

	sprintf(buf[i], "%dKB", size);
	return buf[i];
}

static const char *get_16bits_hex_string(int hex)
{
	static int i = 0;
	static char buf[10][8] = {{0}};
	i = (i+1)%10;

	sprintf(buf[i], "0x%04x", hex);
	return buf[i];
}

static const char *get_32bits_hex_string(int hex)
{
	static int i = 0;
	static char buf[10][12] = {{0}};
	i = (i+1)%10;

	sprintf(buf[i], "0x%08x", hex);
	return buf[i];
}

static void print_venc_info(ST_VENC_DBG_INFO *vinfo)
{
	int s32Index = 0;
	ST_VENC_INFO *pstVencInfo = vinfo->stVencInfo;

	printf("-----Channel Info---------------------------------------"
	"---------------------------------------------\n");
	printf("%-25s%-6s%-8s%-6s%-8s%-6s%-16s%-16s%-16s\n",
		"ChannelName", "Type", "State", "Width",
		"Height", "FPS", "FrameIn", "FrameOut", "FrameDrop");

	for (s32Index = 0; s32Index < vinfo->s8Size; s32Index++)
	{
		printf("%-25s%-6s%-8s%-6d%-8d%-6.1f%-16lld%-16lld%-16d\n",
			(pstVencInfo + s32Index)->ps8Channel,
			get_meta_type((pstVencInfo + s32Index)->enType),
			get_meta_state((pstVencInfo + s32Index)->enState),
			(pstVencInfo + s32Index)->s32Width,
			(pstVencInfo + s32Index)->s32Height,
			(pstVencInfo + s32Index)->f32FPS,
			(pstVencInfo + s32Index)->u64InCount,
			(pstVencInfo + s32Index)->u64OutCount,
			(pstVencInfo + s32Index)->u32FrameDrop);
	}

	printf("\n-----Rate Control Info-----------------------------"
	"---------------------------------------------------\n");
	printf("%-25s%-8s%-8s%-8s%-8s%-8s%-12s%-8s%-8s%-16s%-14s%-16s%-16s\n",
		"ChannelName", "RCMode", "MinQP", "MaxQP", "QPDelta", "RealQP",
		"I-Interval", "GOP", "Mbrc", "ThreshHold(%)", "PictureSkip", "SetBR(kbps)", "BitRate(kbps)");

	for (s32Index = 0; s32Index < vinfo->s8Size; s32Index++) {
		if ((pstVencInfo + s32Index)->enType == IPU_H264
		|| (pstVencInfo + s32Index)->enType == IPU_H265) {
			printf("%-25s%-8s%-8d%-8d%-8d%-8s%-12d%-8d%"
				"-8d%-16d%-14s%-16.1f%-16.1f\n",
			(pstVencInfo + s32Index)->ps8Channel,
			get_meta_rc_mode((pstVencInfo + s32Index)->stRcInfo.enRCMode),
			(pstVencInfo + s32Index)->stRcInfo.s8MinQP,
			(pstVencInfo + s32Index)->stRcInfo.s8MaxQP,
			(pstVencInfo + s32Index)->stRcInfo.s32QPDelta,
			get_meta_realqp((pstVencInfo + s32Index)->stRcInfo.s32RealQP),
			(pstVencInfo + s32Index)->stRcInfo.s32IInterval,
			(pstVencInfo + s32Index)->stRcInfo.s32Gop,
			(pstVencInfo + s32Index)->stRcInfo.s32Mbrc,
			(pstVencInfo + s32Index)->stRcInfo.u32ThreshHold,
            (pstVencInfo + s32Index)->stRcInfo.s32PictureSkip > 0 ? "Enable":"Disable",
			(pstVencInfo + s32Index)->stRcInfo.f32ConfigBitRate,
			(pstVencInfo + s32Index)->f32BitRate);
		}
	}
}

static void print_isp_user_info(video_dbg_user_info* uinfo)
{
	int i = 0;
	printf("-----ISP Base info--------------------------------------"
		"----------------------------\n");
	printf("%-12s%-12s%-12s%-12s%-8s%-8s\n",
		"OutWidth", "OutHeight", "CapWidth", "CapHeight", "FPS", "ISPClk");

	printf("%-12d%-12d%-12d%-12d%-8d%-8s\n",
			uinfo->out_w, uinfo->out_h,
			uinfo->cap_w, uinfo->cap_h, uinfo->fps[0],
			get_freq_mhz_string(uinfo->isp_freq));
	printf("\n");

	printf("-----ISP Memory Info--------------------------------------"
		"----------------------------\n");
	printf("%-12s%-12s%-16s%-16s\n",
		"ISPUsed", "ISPMaxUsed", "MemWritePrio", "MemReadPrio");
	printf("%-12s%-12s%-16s%-16s\n",
			get_size_kb_string(uinfo->used_mem),
			get_size_kb_string(uinfo->used_max_mem),
			get_mem_priority(uinfo->isp_ddr_wpriority),
			get_mem_priority(uinfo->isp_ddr_rpriority));
	printf("\n");

	printf("-----ISP Context Info--------------------------------------"
		"----------------------------\n");

	printf("%-8s%-8s%-8s%-8s%-12s%-12s%-12s%-8s\n",
		"Context","AE","AWB","HWAWB","TNMCurve","DayMode","MirrorMode","3DN");
	for (i = 0; i < uinfo->ctx_num; i++) {
		printf("%-8d%-8s%-8s%-8s%-12s%-12s%-12s%-8s\n",
				i,
				get_module_enable(uinfo->ae_en[i]),
				get_module_enable(uinfo->awb_en[i]),
				get_module_enable(uinfo->hw_awb_en[i]),
				get_tnm_curve_mode(!uinfo->tnm_static_curve[i]),
				get_day_mode(uinfo->day_mode[i]),
				get_mirror_mode(uinfo->mirror_mode[i]),
				get_module_enable(uinfo->ispost_dn_en));
	}
}

void print_isp_info(video_dbg_info *info, video_dbg_user_info *uinfo)
{
	int i = 0;
	int mipi_info_head = 1;
	int dvp_info_head  = 1;

	if (info->efuse.exist) {
		printf("-----Chip Efuse Info----------------------------------"
			"----------------------------\n");
		printf("%-14s%-14s\n","Type","Mac");
		printf("%-14s%-14s\n",
			get_32bits_hex_string(info->efuse.type),
			get_32bits_hex_string(info->efuse.mac));
		printf("\n");
	}

	printf("-----DDR Info----------------------------------"
	"----------------------------\n");
	printf("%-8s%-18s%-18s%-18s%-18s\n",
		"Freq", "PortWritePrioEn","PortReadPrioEn","Port2WritePrio","Port2ReadPrio");
	printf("%-8s%-18s%-18s%-18s%-18s\n",
		get_freq_mhz_string(info->ddr.clk_rate),
		get_16bits_hex_string(info->ddr.port_wpriority_en),
		get_16bits_hex_string(info->ddr.port_rpriority_en),
		get_16bits_hex_string(info->ddr.port2_wpriority),
		get_16bits_hex_string(info->ddr.port2_rpriority));
	printf("\n");

	print_isp_user_info(uinfo);
	printf("\n");

	for (i = 0; i < CI_N_IMAGERS; i++) {

		if (!info->gasket[i].exist)
			continue;

		if (strncmp(info->gasket[i].type, "MIPI", 32) == 0) {
			if (!dvp_info_head && mipi_info_head)
				printf("\n");

			if (mipi_info_head) {
				printf("-----ISP MIPI Gaskt Info-----------------------------------"
					"--------------------------------\n");
				printf("%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s\n",
					"Gasket","Stat","Type","Frame",
					"Lanes","CrcErr","HdrErr",
					"EccErr","EccCorr","FiFoFull");
				mipi_info_head = 0;
			}

			printf("%-8d%-8d%-8s%-8d%-8d%-8d%-8d%-8d%-8d%-8d\n",
				i, info->gasket[i].enable, info->gasket[i].type,
				info->gasket[i].frame_cnt,info->gasket[i].mipi_en_lanes,
				info->gasket[i].mipi_crc_err, info->gasket[i].mipi_hdr_err,
				info->gasket[i].mipi_ecc_err,info->gasket[i].mipi_ecc_corr,
				info->gasket[i].mipi_fifo_full);
		} else {
			if (!mipi_info_head && dvp_info_head)
				printf("\n");

			if (dvp_info_head) {
				printf("-----ISP DVP Gaskt Info-----------------------------------"
					"-------------------------------\n");
				printf("%-8s%-8s%-8s%-8s\n",
					"Gasket","Stat","Type","Frame");
				dvp_info_head = 0;
			}

			printf("%-8d%-8d%-8s%-8d\n",
				i, info->gasket[i].enable, info->gasket[i].type,
				info->gasket[i].frame_cnt);
		}
	}
	printf("\n");

	printf("-----ISP RTM Info--------------------------------------"
		"----------------------------\n");

	printf("%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s\n",
		"Context","Stat","1D","2D","RGB","ENH0",
		"ENH1","ENC-L","ENC-C", "DISP");
	for (i = 0; i < info->isp_info.s32CtxN; i++) {
		if (!info->rtm_ctx.exist[i])
			continue;

		printf("%-8d%-8d%-8d%-8d%-8d%-8d%-8d%-8d%-8d%-8d\n",
			i, info->rtm_ctx.ctx_status[i], info->rtm_ctx.ctx_position[i][0],
			info->rtm_ctx.ctx_position[i][1], info->rtm_ctx.ctx_position[i][2],
			info->rtm_ctx.ctx_position[i][3], info->rtm_ctx.ctx_position[i][4],
			info->rtm_ctx.ctx_position[i][5], info->rtm_ctx.ctx_position[i][6],
			info->rtm_ctx.ctx_position[i][7]);
	}
	printf("\n");

	printf("-----ISP HWList Info----------------------------------"
		"----------------------------\n");
	printf("%-8s%-12s%-12s%-12s%-12s\n",
		"Context","Available","Pending","Processed","Send");
	for (i = 0; i < info->isp_info.s32CtxN; i++) {
		if (info->hwlist[i].exist) {
			printf("%-8d%-12d%-12d%-12d%-12d\n",
				i, info->hwlist[i].available_elements, info->hwlist[i].pending_elements,
				info->hwlist[i].processed_elements, info->hwlist[i].send_elements);
		}
	}
	printf("\n");

	printf("-----Sensor Info--------------------------------------"
		"----------------------------\n");
	printf("%-8s%-12s%-12s%-12s%-12s%-12s%-12s\n",
		"Sensor","Gain","MaxGain","MinGain","Exp","MaxExp","MinExp");
	for (i = 0; i < info->isp_info.s32CtxN; i++) {
		printf("%-8d%-12.6f%-12.6f%-12.6f%-12u%-12u%-12u\n",
				i, info->isp_info.f64Gain[i],
				info->isp_info.f64MaxGain[i],
				info->isp_info.f64MinGain[i],
				info->isp_info.u32Exp[i],
				info->isp_info.u32MaxExp[i],
				info->isp_info.u32MinExp[i]);
	}
	printf("\n");

	printf("-----ISP Context Interrupt Info--------------------------------"
		"----------------------------\n");

	printf("%-8s%-8s%-8s%-12s%-14s%-12s%-12s%-14s%-12s\n",
		"Context","Active","IntCnt",
		"DoneIntCnt","ErrIgnIntCnt","SOFIntCnt",
		"HardTriCnt","ThreadTriCnt","IntPerSec");

	for (i = 0; i <CI_N_CONTEXT; i++) {
		if (info->isp_ctx_int[i].exist) {
			printf("%-8d%-8d%-8d%-12d%-14d%-12d%-12d%-14d%-12d\n",
				i, info->isp_ctx_int[i].active, info->isp_ctx_int[i].int_cnt,
				info->isp_ctx_int[i].done_all_cnt, info->isp_ctx_int[i].err_ignore_cnt,
				info->isp_ctx_int[i].start_of_frame_cnt, info->isp_ctx_int[i].hard_cnt,
				info->isp_ctx_int[i].thread_cnt, info->isp_int.int_per_sec[i]);
		}
	}
	printf("\n");

	printf("-----ISP Interrupt Info----------------------------------"
		"----------------------------\n");
	printf("%-12s%-18s%-20s%-18s%-18s\n",
		"Connect",
		"LongestHardIntUS",
		"LongestThreadIntUS",
		"ServicedHardInt",
		"ServicedThreadInt");

	printf("%-12d%-18d%-20d%-18d%-18d\n",
			info->isp_int.connections,
			info->isp_int.longest_hard_us,
			info->isp_int.longest_thread_us,
			info->isp_int.serviced_hard_int,
			info->isp_int.serviced_thread_int);
	printf("\n");

	printf("-----ISPOST Info----------------------------------"
	"----------------------------\n");
	printf("%-12s%-12s%-12s%-12s%-12s%-12s\n",
		"HWVersion","TriggerCnt","Stat0","Ctrl0","DnStat", "DnRefYAddr");
	printf("%-12s%-12d%-12s%-12s%-12d%-12s\n",
		get_16bits_hex_string(info->ispost.hw_version),
		info->ispost.hw_status.ispost_count,
		get_32bits_hex_string(info->ispost.hw_status.stat0),
		get_32bits_hex_string(info->ispost.hw_status.ctrl0),
		info->ispost.dn.stat,
		get_32bits_hex_string(info->ispost.dn.ref_y_addr));
	printf("\n");

#if 0
	printf("-----CSI Reg Info----------------------------------"
	"----------------------------\n");
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_VERSION_REG, info->csi.version);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_N_LANES_REG, info->csi.n_lanes);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_DPHY_SHUTDOWN_REG, info->csi.phy_shutdownz);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_DPHYRST_REG, info->csi.dphy_rstz);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_RESET_REG, info->csi.csi2_resetn);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_DPHY_STATE_REG, info->csi.phy_state);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_DATA_IDS_1_REG, info->csi.data_ids_1);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_DATA_IDS_2_REG,info->csi.data_ids_2);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_ERR1_REG, info->csi.err1);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_ERR2_REG, info->csi.err2);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_MASK1_REG, info->csi.mark1);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_MASK2_REG, info->csi.mark2);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_DPHY_TST_CRTL0_REG, info->csi.phy_test_ctrl0);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSI2_DPHY_TST_CRTL1_REG, info->csi.phy_test_ctrl1);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSIDMAREG_FRAMESIZE, info->csi.img_size);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSIDMAREG_DATATYPE, info->csi.mipi_data_type);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSIDMAREG_VSYNCCNT, info->csi.vsync_cnt);
	printf("CSI_REG_OFFSET0x%03x =[0x%08x]\n",
		CSIDMAREG_HBLANK, info->csi.hblank);
#endif
}

void print_user_info(video_dbg_user_info* uinfo)
{
	print_isp_user_info(uinfo);
	printf("\n");
	print_venc_info(&(uinfo->vinfo));
}

void video_dbg_mode_print(void)
{
	video_dbg_info* info = NULL;
	info = video_dbg_get_info();
	if (!info) {
		VDBG_LOG_ERR("get video debug info failed\n");
		return ;
	}

	video_dbg_user_info* uinfo = NULL;
	uinfo = video_dbg_get_user_info();
	if (!uinfo) {
		VDBG_LOG_ERR("get video debug info failed\n");
		return ;
	}

	switch(info->mode){
	case VIDEO_DEBUG_UE_MODE:
		print_user_info(uinfo);
		break;

	case VIDEO_DEBUG_PE_MODE:
		print_isp_info(info, uinfo);
		print_venc_info(&info->vinfo);
		break;

	case VIDEO_DEBUG_ISP_UE_MODE:
		print_isp_user_info(uinfo);
		break;

	case VIDEO_DEBUG_ISP_PE_MODE:
		print_isp_info(info, uinfo);
		break;

	case VIDEO_DEBUG_VENC_MODE:
		print_venc_info(&(uinfo->vinfo));
		break;

	default:
		print_user_info(uinfo);
		break;
	}
}

int video_dbg_get_cpu_type(void)
{
	int ret = 0;
	int type;
	char result[64] = {0};
	int size = sizeof(result);

	ret = video_dbg_exec_cmd_to_buf(BOARD_CPU_CMD, result, &size);
	if (ret < 0) {
		type = CPU_TYPE_APOLLO2;
		return type;
	}

	VDBG_LOG_DBG("cpu:%s size:%d\n", result, size);

	if (strncmp(result, BOARD_CPU_APOLLO, size) == 0) {
		type = CPU_TYPE_APOLLO;
	} else if (strncmp(result, BOARD_CPU_APOLLO2,size) == 0) {
		type = CPU_TYPE_APOLLO2;
	} else if (strncmp(result, BOARD_CPU_APOLLO3, size) == 0) {
		type = CPU_TYPE_APOLLO3;
	} else {
		type = CPU_TYPE_APOLLO2;
	}

	VDBG_LOG_DBG("cpu:%s type:%d\n", result, type);
	return type;
}

int video_dbg_init(int mode)
{
	int ret = 0;
	int type =CPU_TYPE_APOLLO2;

	list_init(g_parser_list);

	g_video_dbg_info = ( video_dbg_info* )malloc(sizeof( video_dbg_info));
	if (!g_video_dbg_info) {
		VDBG_LOG_ERR("malloc video_dbg_info failed \n");
		return -1;
	}
	memset(g_video_dbg_info, 0, sizeof( video_dbg_info));

	g_video_dbg_user_info =
		(video_dbg_user_info* )malloc(sizeof( video_dbg_user_info));
	if (!g_video_dbg_user_info) {
		VDBG_LOG_ERR("malloc video_dbg_info failed \n");
		return -1;
	}
	memset(g_video_dbg_user_info, 0, sizeof( video_dbg_user_info));

	ret =video_dbg_check_path();
	if (ret< 0)
		VDBG_LOG_ERR("init failed\n");

	g_video_dbg_info->mode = mode;

	type = video_dbg_get_cpu_type();
	g_video_dbg_info->cpu_type = type;

	switch(mode) {
	case VIDEO_DEBUG_UE_MODE:
	case VIDEO_DEBUG_ISP_UE_MODE:
		ret = video_dbg_isp_user_parser_init(g_video_dbg_info);
		ret = video_dbg_ispost_parser_init(g_video_dbg_info);
		ret = video_dbg_ddr_parser_init(g_video_dbg_info);

		if (mode == VIDEO_DEBUG_UE_MODE) {
			ret = video_dbg_venc_parser_init(g_video_dbg_info);
		}
		break;

	case VIDEO_DEBUG_PE_MODE:
	case VIDEO_DEBUG_ISP_PE_MODE:
		ret = video_dbg_efuse_parser_init(g_video_dbg_info);
		ret = video_dbg_isp_parser_init(g_video_dbg_info);
		ret = video_dbg_ispost_parser_init(g_video_dbg_info);
		ret = video_dbg_ddr_parser_init(g_video_dbg_info);
#if 0
		ret = video_dbg_csi_reg_parser_init(g_video_dbg_info);
#endif

		if (mode == VIDEO_DEBUG_PE_MODE) {
			ret = video_dbg_venc_parser_init(g_video_dbg_info);
		}
		break;

	case VIDEO_DEBUG_VENC_MODE:
		ret = video_dbg_venc_parser_init(g_video_dbg_info);
		if (ret< 0)
			VDBG_LOG_ERR("init failed\n");
		break;
	}

	return ret;
}

void video_dbg_deinit(void)
{
	video_dbg_parser *_p,* _q;
	list_del_all(_p, _q, g_parser_list);

	free(g_video_dbg_info);
	g_video_dbg_info = NULL;

	free(g_video_dbg_user_info);
	g_video_dbg_user_info = NULL;
}

void video_dbg_print_info(int mode)
{
	struct timespec start = {0};
	perf_measure_start(&start);

	video_dbg_init(mode);

	video_dbg_update();
	video_dbg_mode_print();

	video_dbg_deinit();

	perf_measure_stop(__func__, &start);
}

static const char *get_unit_string(int unit, const char* unit_name)
{
	static char buf[32];
	int offset;

	offset = snprintf(buf, sizeof(buf), "%d%s", unit, unit_name);
	buf[offset] = '\0';

	return buf;
}

static const char* get_taskname_by_pid(pid_t pid)
{
	char proc_pid_path[32];
	static char task_name[16];

	strcpy(task_name, "");
	snprintf(proc_pid_path, sizeof(proc_pid_path), "/proc/%d/cmdline", pid);
	FILE* fp = fopen(proc_pid_path, "r");
	if(NULL != fp){
		fgets(task_name, sizeof(task_name)-1, fp);
		fclose(fp);
	}
	return task_name;
}

int audio_dbg_init()
{
	int ret = 0;

	list_init(g_parser_list);

	g_audio_dbg_info = ( audio_dbg_info_t* )malloc(sizeof(audio_dbg_info_t));
	if (!g_audio_dbg_info) {
		VDBG_LOG_ERR("malloc video_dbg_info failed \n");
		return -1;
	}
	memset(g_audio_dbg_info, 0, sizeof( audio_dbg_info_t));

	ret = video_dbg_parser_register(PARSE_OBJ_AUDIO_DEV,
			NULL,
			audio_get_debug_info_parser_cb,
			SRC_FROM_API, DST_STRUCT,
			g_audio_dbg_info->audio_devinfo,
			sizeof(g_audio_dbg_info->audio_devinfo));

	ret = video_dbg_parser_register(PARSE_OBJ_AUDIO_CHAN,
			NULL,
			audio_get_debug_info_parser_cb,
			SRC_FROM_API, DST_STRUCT,
			g_audio_dbg_info->audio_chaninfo,
			sizeof(g_audio_dbg_info->audio_chaninfo));

	return ret;
}

int audio_dbg_update(void)
{
	struct timespec start;
	if (!g_audio_dbg_info)
		return -1;

	perf_measure_start(&start);
	video_dbg_parser* parser;
	list_for_each (parser, g_parser_list) {
		perf_measure_start(NULL);
		switch(parser->src) {
			case SRC_FROM_FILE:
				video_dbg_file_parser_update(parser);
				break;
			case SRC_FROM_API:
				video_dbg_api_parser_update(parser);
				break;
			case SRC_FROM_CMD:
				video_dbg_cmd_parser_update(parser);
				break;
			default:
				break;
		}
		perf_measure_stop(parser->obj, NULL);
	}

	perf_measure_stop(__func__, &start);
	return 0;
}

void print_audio_dbg_info(audio_dbg_info_t* info)
{
	audio_devinfo_t  *devinfo = NULL;
	audio_chaninfo_t *chaninfo = NULL;
	int		FrmTime, FrmRate;
	char	amute[8] = "";
	char	abaec[8] = "";
	int		i;

	printf("-----Audio DEV ATTR---------------------------------------"
		"---------------------------------------------\n");
	printf("%-12s%-8s%-12s%-10s%-10s%-12s%-9s%-9s%-12s\n",
		"DevName", "DevDir", "SampleRate", "BitWidth",
		"Channels", "SampleSize", "FrmTime", "FrmRate", "PCMMaxFrm");

	devinfo = info->audio_devinfo;
	for(i=0;i<MAX_AUDIO_DEV_NUM;i++){
		if(devinfo->active == 0)
			break;
		FrmTime = (1000*1000)/devinfo->fmt.samplingrate;
		FrmRate = 1000/FrmTime;

		printf("%-12s%-8s%-12d%-10d%-10s%-12d%-9s%-9d%-12d\n",
			devinfo->nodename,
			(devinfo->direct == DEVICE_IN_ALL)? "in":"out",
			devinfo->fmt.samplingrate,
			devinfo->fmt.bitwidth,
			(devinfo->fmt.channels == 1)? "mono":"stereo",
			devinfo->fmt.sample_size,
			get_unit_string(FrmTime, "ms"),
			FrmRate,
			devinfo->fmt.pcm_max_frm);
		devinfo++;
	}
	printf("\n");

	printf("-----Audio DEV STATUS---------------------------------------"
		"---------------------------------------------\n");
	printf("%-12s%-8s%-9s%-8s%-8s%-12s\n",
		"DevName", "DevDir", "ChanCnt",
		"bAec", "bMute", "MasterVol");

	devinfo = info->audio_devinfo;
	for(i=0;i<MAX_AUDIO_DEV_NUM;i++){
		if(devinfo->active == 0)
			break;
		FrmTime = (1000*1000)/devinfo->fmt.samplingrate;
		FrmRate = 1000/FrmTime;

		if(devinfo->mute == -1)
			snprintf(amute, sizeof(amute), "N/A");
		else if(devinfo->mute == 0)
			snprintf(amute, sizeof(amute), "N");
		else
			snprintf(amute, sizeof(amute), "Y");

		if(devinfo->ena_aec == -1)
			snprintf(abaec, sizeof(abaec), "N/A");
		else if(devinfo->ena_aec == 0)
			snprintf(abaec, sizeof(abaec), "N");
		else
			snprintf(abaec, sizeof(abaec), "Y");

		printf("%-12s%-8s%-9d%-8s%-8s%-12d\n",
			devinfo->nodename,
			(devinfo->direct == DEVICE_IN_ALL)? "in":"out",
			devinfo->chan_cnt,
			abaec,
			amute,
			devinfo->master_volume);
		devinfo++;
	}
	printf("\n");

	printf("-----Audio CHAN STATUS---------------------------------------"
			"---------------------------------------------\n");
	printf("%-8s%-12s%-8s%-8s%-14s%-10s%-16s\n",
			"ChnId", "DevName", "DevDir", "Volume",
			"Priority", "Pid", "TaskName");

	chaninfo = info->audio_chaninfo;
	for(i=0;i<MAX_AUDIO_CHAN_NUM;i++){
		if(chaninfo->active == 0)
			break;
		printf("%-8d%-12s%-8s%-8d%-14s%-10d%-16s\n",
			chaninfo->chan_id,
			chaninfo->devname,
			(chaninfo->direct == DEVICE_IN_ALL)? "in":"out",
			chaninfo->volume,
			(chaninfo->priority == CHANNEL_BACKGROUND)? "BACKGROUND":"FOREGROUND",
			chaninfo->chan_pid,
			get_taskname_by_pid(chaninfo->chan_pid)
		);
		chaninfo++;
	}

	printf("\n");
}

void audio_dbg_mode_print(void)
{
	print_audio_dbg_info(g_audio_dbg_info);
}

void audio_dbg_deinit(void)
{
	video_dbg_parser *_p,* _q;
	list_del_all(_p, _q, g_parser_list);

	if(g_audio_dbg_info) {
		free(g_audio_dbg_info);
		g_audio_dbg_info = NULL;
	}
}

void audio_dbg_print_info(void)
{
	struct timespec start = {0};

	perf_measure_start(&start);

	audio_dbg_init();

	audio_dbg_update();
	audio_dbg_mode_print();

	audio_dbg_deinit();

	perf_measure_stop(__func__, &start);
}

