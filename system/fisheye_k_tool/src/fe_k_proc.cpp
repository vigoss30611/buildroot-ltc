#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <memory.h>
#include <errno.h>
#include <fstream>

#include <qsdk/videobox.h>

#include "libgridtools.h"
#include "fe_k_proc.h"
#include "fe_k_misc.h"

/*
* History
* 1.0 : base version
* 2.0 :
*	(1) update libgridtools.so for memory leakage
*	(2) modify the parameter structure
*	(3) modify the fisheye grid size height
* 2.1
*	(1) update libgridtools.so to v1.1.0,
*	    add errorThreshold variant for center deviation,
*	    and save src yuv data to file.
*	(2) optimize the generating data time.
*/

using namespace std;

#define FE_K_TOOL_VERSION  "2.1"


#define SRC_YUV_FROM_ISP      1
#define SRC_YUV_FILE          "/nfs/FEIN_1088x1088_mode7_NV12.yuv"
#define SRC_YUV_FILE_WIDTH    1088
#define SRC_YUV_FILE_HEIGHT   1088

#define DST_FILE_PATH         "/nfs/"
#define IS_LOCAL_COMBINE      0
#define IS_GLOBAL_COMBINE     0
#define GLOBAL_GRID_SIZE      32
#define GLOBAL_OUT_WIDTH      1280
#define GLOBAL_OUT_HEIGHT     720

#define IS_SAVE_GEO_FILE      0
#define ALIGN_SIZE            64

#define CENTER_DEVIATION_THRESHOLD  100
#define SAVE_SRC_YUV_PATH     "/tmp/"


typedef struct fe_k{
	fe_view_t *view_ptr;
	int is_local_combine;
}fe_k_t;

extern fe_view_t fv_2p360;
extern fe_view_t fv_downview;
extern fe_view_t fv_b1r;
extern fe_view_t fv_p1;
extern fe_view_t fv_ceiling;
extern fe_view_t fv_floor;
extern fe_view_t fv_org;

static fe_k_t k_list[] =
{
//	{&fv_2p360, IS_LOCAL_COMBINE},
//	{&fv_downview, IS_LOCAL_COMBINE},
	{&fv_b1r, IS_LOCAL_COMBINE},
//	{&fv_p1, IS_LOCAL_COMBINE},
//	{&fv_ceiling, IS_LOCAL_COMBINE},
//	{&fv_floor, IS_LOCAL_COMBINE},
//	{&fv_org, IS_LOCAL_COMBINE},
	{NULL, 0}
};


static void print_version()
{
	unsigned long version = 0;

	printf("fisheye tool version: %s\n", FE_K_TOOL_VERSION);

	version = FceGetVersion();
	printf("lib version: %ld.%ld.%ld \n", (version>>16) & 0xFF, (version>>8) & 0xFF, version & 0xFF);
}

static int get_src_yuv_from_file(char *file_name, fe_src_yuv_t *out_src_yuv_ptr)
{
	int ret = -1;
	FILE *fd = NULL;
	int size = 0;
	char *p_yuv = NULL;


	if (NULL == file_name || NULL == out_src_yuv_ptr) {
		printf("Param %p %p are NULL!!!\n", file_name, out_src_yuv_ptr);
		return -1;
	}

	fd = fopen(file_name, "rb");
	if (NULL == fd) {
		printf("Failed to open %s !!!\n", file_name);
		goto open_fail;
	}

	struct stat statbuf;

	stat(file_name, &statbuf);
	size = statbuf.st_size;

	printf("Input file size:%d \n", size);

	p_yuv = (char*)malloc(size);
	if (NULL == p_yuv) {
		printf("Failed to malloc size %d !!!\n", size);
		goto malloc_fail;
	}
	fread(p_yuv, size, 1, fd);
	fclose(fd);
	out_src_yuv_ptr->width = SRC_YUV_FILE_WIDTH;
	out_src_yuv_ptr->height = SRC_YUV_FILE_HEIGHT;
	out_src_yuv_ptr->buf_ptr = p_yuv;
	return 0;

malloc_fail:
	if (fd)
		fclose(fd);
open_fail:

	return ret;
}

static int get_global_file_info(fe_view_param_t *view_param_ptr, int max_idx)
{
	int ret = 0;

	char id_name[] = "all";
	char *grid_name_ptr = NULL;
	char *geo_name_ptr = NULL;
	char func_name[30];
	FILE *global_grid_fd = NULL;
	FILE *global_geo_fd = NULL;
	int is_save_geo_file = 0;


	if (!view_param_ptr->common_param.is_global_combine)
		return 0;

	view_param_ptr->common_param.id_name = id_name;
	is_save_geo_file = view_param_ptr->common_param.is_save_geo_file;
	grid_name_ptr = view_param_ptr->global_grid_name;
	geo_name_ptr = view_param_ptr->global_geo_name;


	sprintf(func_name, "fisheyes");
	get_view_file_name(&view_param_ptr->common_param, 0, 0, func_name,
								NULL, max_idx, grid_name_ptr);
	global_grid_fd = fopen(grid_name_ptr, "wb");
	if (NULL == global_grid_fd) {
		printf("grid combine fd is NULL !!!\n");
		goto to_out;
	}

	if (is_save_geo_file) {
		sprintf(func_name, "geo");
		get_view_file_name(&view_param_ptr->common_param, 0, 0, func_name,
								NULL, max_idx, geo_name_ptr);
		global_geo_fd = fopen(geo_name_ptr, "wb");
		if (NULL == global_geo_fd) {
			printf("geo combine fd is NULL !!!\n");
			goto to_out;
		}
	}
	view_param_ptr->common_param.grid_fd = global_grid_fd;
	view_param_ptr->common_param.geo_fd = global_geo_fd;

	return 0;

to_out:
	if (global_grid_fd)
		fclose(global_grid_fd);
	if (global_geo_fd)
		fclose(global_geo_fd);
	return ret;
}

static int get_buffer_from_isp(fe_src_yuv *out_src_yuv_ptr)
{
	int ret = -1;
	int w = 0;
	int h = 0;

	char isp_port[] = "isp-out";
	struct fr_buf_info buf;

	char *v_addr = 0;
	uint32_t size  = 0;
	char *p_yuv = NULL;
	int cnt = 0;
	int max_cnt = 3;


	memset(&buf, 0, sizeof(buf));
	ret = video_get_resolution(isp_port, &w, &h);

	do {
		fr_INITBUF(&buf, NULL);
		ret = fr_get_ref_by_name(isp_port, &buf);
		if (0 == ret) {
			break;
		}
		usleep(25*1000);
	}while(cnt++ < max_cnt);
	if (ret) {
		printf("isp out buf is failed!!!\n");
		return ret;
	}

	v_addr = (char*)buf.virt_addr;
	size = buf.size;
	if (NULL == v_addr || 0 == size) {
		ret = -1;
		printf("ISP snap v_addr=%p or size:%d is error !!!\n", v_addr, size);
		goto to_exit;
	}
	p_yuv = (char*)malloc(size);
	if (NULL == p_yuv) {
		ret = -1;
		printf("malloc %d failed !!!\n", size);
		goto to_exit;
	}
#if 1
	memcpy(p_yuv, v_addr, size);
	fr_put_ref(&buf);
#else
{
	char file_name[] = "/nfs/fisheye_snap_1.yuv";
	FILE *fd = NULL;
	fd = fopen(file_name, "wb");
	if (fd) {
		fwrite((void*)p_yuv, size, 1, fd);
		fclose(fd);
	}
}
#endif

	out_src_yuv_ptr->width = w;
	out_src_yuv_ptr->height = h;
	out_src_yuv_ptr->buf_ptr = p_yuv;
	ret = 0;
	return 0;

to_exit:
	fr_put_ref(&buf);
	return ret;
}

static int get_src_yuv_from_isp(char *in_js_ptr, fe_src_yuv *out_src_yuv_ptr)
{
	int ret = 0;

//	tm_start();
	ret = videobox_repath(in_js_ptr);
	if (ret) {
		printf("videobox repath failed!!!\n");
		videobox_stop();
		return ret;
	}

	ret = get_buffer_from_isp(out_src_yuv_ptr);
	videobox_stop();
//	tm_print_diff_ms((char*)"yuv from isp");
	return ret;
}

static int view_proc(fe_view_param_t *view_param_ptr)
{
	int ret = -1;
	int i = 0;
	int list_cnt = ARRY_SIZE(k_list) - 1;
	fe_view_t *view_ptr = NULL;
	int max_idx = 0;
	int is_local_combine = 0;
	int global_out_width = GLOBAL_OUT_WIDTH;
	int global_out_height = GLOBAL_OUT_HEIGHT;
	int global_grid_size = GLOBAL_GRID_SIZE;


	if (NULL == view_param_ptr) {
		printf("%s(%d):Param is NULL!!!\n", __func__, __LINE__);
		return -1;
	}
	if (view_param_ptr->common_param.is_global_combine) {

		view_param_ptr->common_param.out_width = global_out_width;
		view_param_ptr->common_param.out_height = global_out_height;
		view_param_ptr->common_param.grid_size = global_grid_size;


		for (i = 0; i < list_cnt; ++i) {
			view_ptr = k_list[i].view_ptr;
			if (view_ptr) {
				max_idx += view_ptr->max_idx;
			}
		}

		ret = get_global_file_info(view_param_ptr, max_idx);
		if (ret)
			goto to_out;
	}

	for (i = 0; i < list_cnt; ++i) {
		if (0 == view_param_ptr->common_param.is_global_combine)
			is_local_combine = k_list[i].is_local_combine;
		view_ptr = k_list[i].view_ptr;
		if (view_ptr) {
			view_param_ptr->common_param.id_name = view_ptr->id_name;
			view_param_ptr->common_param.is_local_combine = is_local_combine;

			ret = view_ptr->ops.proc(view_param_ptr);
			if (ret) {
				printf("%s is failed!!! \n", view_ptr->id_name);
			}
		}
	}

to_out:
	return ret;
}

int do_fe_k(char *in_js_ptr)
{
	int ret = 0;
	char file_nane[256] = SRC_YUV_FILE;
	fe_src_yuv_t src_yuv;
	fe_view_param_t view_param;
	char dst_file_path[256] = DST_FILE_PATH;
	int is_global_combine = IS_GLOBAL_COMBINE;

	char cmd_str[256];
	int src_yuv_from_isp = SRC_YUV_FROM_ISP;
	int align_size = ALIGN_SIZE;
	int is_save_geo_file = IS_SAVE_GEO_FILE;

	int center_error_threshold  = CENTER_DEVIATION_THRESHOLD;
	char save_src_yuv_path[256] = SAVE_SRC_YUV_PATH;


	sprintf(cmd_str,"mkdir -p %s", DST_FILE_PATH);
	ret = system(cmd_str);
	if (ret < 0) {
	    printf("cmd: %s , error: %s\n", cmd_str, strerror(errno));
	    return -1;
	}


//	tm_start();
	print_version();

	memset(&src_yuv, 0, sizeof(src_yuv));
	if (src_yuv_from_isp) {
		ret = get_src_yuv_from_isp(in_js_ptr, &src_yuv);
	} else {
		ret = get_src_yuv_from_file(file_nane, &src_yuv);
	}
	if (ret
		|| (NULL == src_yuv.buf_ptr)
		|| (0 == src_yuv.width)
		|| (0 == src_yuv.height)
		) {
		goto to_out;
	}

	printf("src yuv w,h:%dx%d buf:%p\n", src_yuv.width, src_yuv.height, src_yuv.buf_ptr);

	memset(&view_param, 0, sizeof(view_param));
	view_param.common_param.in_width = src_yuv.width;
	view_param.common_param.in_height = src_yuv.height;
	view_param.common_param.yuv_buf = src_yuv.buf_ptr;
	view_param.common_param.dst_file_path = dst_file_path;
	view_param.common_param.is_global_combine = is_global_combine;
	view_param.common_param.align_size = align_size;
	view_param.common_param.is_save_geo_file = is_save_geo_file;
	view_param.common_param.center_error_threshold = center_error_threshold;
	view_param.common_param.save_src_yuv_path = save_src_yuv_path;

	ret = view_proc(&view_param);
	if (ret)
		goto to_out;
	if (is_global_combine && view_param.common_param.grid_fd) {
		printf("Save %s Success\n", view_param.global_grid_name);
	}
	if (is_global_combine && view_param.common_param.geo_fd) {
		printf("Save %s Success\n", view_param.global_geo_name);
	}
to_out:
	if (view_param.common_param.grid_fd)
		fclose(view_param.common_param.grid_fd);
	if (view_param.common_param.geo_fd)
		fclose(view_param.common_param.geo_fd);

	if (view_param.common_param.yuv_buf)
		free(view_param.common_param.yuv_buf);

//	tm_print_diff_ms((char*)"all proc");
	return ret;
}

