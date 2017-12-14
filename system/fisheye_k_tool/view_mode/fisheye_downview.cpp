#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <memory.h>

#include "libgridtools.h"
#include "fe_k_proc.h"
#include "fe_k_misc.h"


/*local parameters*/
#define VIEW_ID "down_view"
#define MODE_NAME "downview"
#define MAX_IDX 4
#define LOCAL_OUT_WIDTH   960
#define LOCAL_OUT_HEIGHT  544
#define LOCAL_GRID_SIZE   32
#define LOCAL_ANGLE_UNIT (360 / MAX_IDX)

static int dv_gen_griddata(fe_gd_param_t *gd_param_ptr)
{
	int ret = -1;
	int i = 0;
	TFceFindCenterParam center_param;
	TFceCalcGDSizeParam data_size;
	TFceGenGridDataParam grid_param;
	FILE *grid_fp = NULL;
	FILE *geo_fp = NULL;
	char grid_full_name[256];
	char geo_full_name[256];
	char func_name[30];
	FILE *grid_combine_fd = NULL;
	FILE *geo_combine_fd = NULL;
	int is_local_combine = 0;

	int align_size = 0;
	int is_save_geo_file = 0;
	char *dst_file_path = NULL;
	char *yuv_buf = NULL;
	int in_width = 0;
	int in_height = 0;
	int out_width = 0;
	int out_height = 0;
	int is_global_combine = 0;
	int grid_size = 0;
	double org_rotate_angle = 0.0;
	int center_error_threshold = 0;
	int save_file_ret = 0;
	char save_yuv_filename[256];


	if (NULL == gd_param_ptr || NULL == gd_param_ptr->common_param.dst_file_path) {
		printf("Param %p %p is NULL !!!\n", gd_param_ptr, dst_file_path);
		return ret;
	}

	if (NULL == gd_param_ptr->common_param.yuv_buf) {
		printf("YUV buf is NULL !!!\n");
		return ret;
	}

	memset(&center_param, 0, sizeof(center_param));
	memset(&data_size, 0, sizeof(data_size));
	memset(&grid_param, 0, sizeof(grid_param));

	is_local_combine = gd_param_ptr->common_param.is_local_combine;
	is_global_combine = gd_param_ptr->common_param.is_global_combine;
	is_save_geo_file = gd_param_ptr->common_param.is_save_geo_file;
	align_size = gd_param_ptr->common_param.align_size;
	dst_file_path = gd_param_ptr->common_param.dst_file_path;
	in_width = gd_param_ptr->common_param.in_width;
	in_height = gd_param_ptr->common_param.in_height;
	out_width = gd_param_ptr->common_param.out_width;
	out_height = gd_param_ptr->common_param.out_height;
	yuv_buf = gd_param_ptr->common_param.yuv_buf;
	grid_size = gd_param_ptr->common_param.grid_size;
	center_error_threshold = gd_param_ptr->common_param.center_error_threshold;


	printf("gd_parm in:%dx%d, out:%dx%d, buf=%p, angle=%d \n",
			in_width, in_height,
			out_width, out_height,
			yuv_buf, gd_param_ptr->angle_unit
		);


	//step 1. find center
	center_param.mode = 0;
	center_param.inWidth = in_width;
	center_param.inHeight = in_height;
	center_param.yBuf = (unsigned char*)yuv_buf;
	center_param.errorThreshold = center_error_threshold;
	ret = FceFindCenter(&center_param);
	if (ret) {
		printf("FceFindCenter() error ret=%d !!!\n", ret);
		save_file_ret = fe_save_file(&gd_param_ptr->common_param, (char*)"fisheye_src.yuv", save_yuv_filename);
		if (save_file_ret)
			printf("Save src yuv file failed (name:%p buf:%p) !!!\n", save_yuv_filename, yuv_buf);
		else
			printf("Saved src yuv file %s \n", save_yuv_filename);
		goto to_out;
	}
	printf("fisheyes center=(%d,%d)\n", center_param.ctX, center_param.ctY);


	//step 2. get data size
	data_size.gridSize = grid_size;
	data_size.inWidth = in_width;
	data_size.inHeight = in_height;
	data_size.outWidth = out_width;
	data_size.outHeight = out_height;

	ret = FceCalGDSize(&data_size);
	if (ret) {
		printf("FceCalGDSize() error!\n");
		goto to_out;
	}
	printf("gridBufLen = %d, geoBufLen = %d\n", data_size.gridBufLen, data_size.geoBufLen);

	//step 3. gen grid data
	grid_param.fisheyeMode = FISHEYE_MODE_PERSPECTIVE;
	grid_param.fisheyeImageWidth = data_size.inWidth;
	grid_param.fisheyeImageHeight = data_size.inHeight;
	grid_param.fisheyeGridSize = data_size.gridSize;
	grid_param.fisheyeCenterX = center_param.ctX;
	grid_param.fisheyeCenterY = center_param.ctY;

	grid_param.rectImageWidth = data_size.outWidth;
	grid_param.rectImageHeight = data_size.outHeight;

	grid_param.fisheyeRadius = 0;
	grid_param.fisheyeHeadingAngle = 90;
	grid_param.fisheyePitchAngle = 47.5;
	grid_param.fisheyeFovAngle = 70;
	grid_param.fisheyeRotateAngle = 45;

	grid_param.gridBufLen = (data_size.gridBufLen + align_size - 1) / align_size * align_size;
	grid_param.geoBufLen = (data_size.geoBufLen + align_size - 1) / align_size * align_size;

	grid_param.fisheyeGridbuf = (unsigned char*)calloc(1, grid_param.gridBufLen);
	if (NULL == grid_param.fisheyeGridbuf) {
		printf("Grid buf is NULL !!!\n");
		goto to_out;
	}
	if (is_save_geo_file) {
		grid_param.fisheyeGeobuf = (unsigned char*)calloc(1, grid_param.geoBufLen);
		if (NULL == grid_param.fisheyeGeobuf) {
			printf("Geo buf is NULL !!!\n");
			goto to_out;
		}
	}

	if (is_global_combine) {
		grid_combine_fd = gd_param_ptr->common_param.grid_fd;
		geo_combine_fd = gd_param_ptr->common_param.geo_fd;
	}else if (is_local_combine) {
		sprintf(func_name, "fisheyes");
		ret = get_view_file_name(&gd_param_ptr->common_param, 0, 0, func_name,
								(char*)MODE_NAME, MAX_IDX, grid_full_name);
		if (ret)
			goto to_out;
		grid_combine_fd = fopen(grid_full_name, "wb");
		if (NULL == grid_combine_fd) {
			printf("grid combine fd is NULL !!!\n");
			goto to_out;
		}

		if (is_save_geo_file) {
			sprintf(func_name, "geo");
			ret = get_view_file_name(&gd_param_ptr->common_param, 0, 0, func_name,
									(char*)MODE_NAME, MAX_IDX, geo_full_name);
			if (ret)
				goto to_out;
			geo_combine_fd = fopen(geo_full_name, "wb");
			if (NULL == geo_combine_fd) {
				printf("geo combine fd is NULL !!!\n");
				goto to_out;
			}
		}
	}

	org_rotate_angle = grid_param.fisheyeRotateAngle;
	for (i = 0; i < MAX_IDX; ++i) {
		grid_param.fisheyeRotateAngle = org_rotate_angle + i * gd_param_ptr->angle_unit;
		//printf("fisheyeRotateAngle=%f\n", grid_param.fisheyeRotateAngle);

		ret = FceGenGridData(&grid_param);
		if (ret) {
			printf("Genrating grid data error. Please check the parameters\n");
			goto to_out;
		}
		if (grid_combine_fd) {
			fwrite(grid_param.fisheyeGridbuf, grid_param.gridBufLen, 1, grid_combine_fd);
		} else {
			sprintf(func_name, "fisheyes");
			ret = get_view_file_name(&gd_param_ptr->common_param, 0, 0, func_name,
								(char*)MODE_NAME, i, grid_full_name);
			if (ret)
				goto to_out;
			grid_fp = fopen(grid_full_name, "wb");
			if (grid_fp) {
				fwrite(grid_param.fisheyeGridbuf, grid_param.gridBufLen, 1, grid_fp);
				fclose(grid_fp);
				printf("Save %s Success\n", grid_full_name);
			} else {
				printf("Create grid file failed %s !!!\n", grid_full_name);
				goto to_out;
			}
			grid_fp = NULL;
		}

		if (is_save_geo_file) {
			if (geo_combine_fd) {
				fwrite(grid_param.fisheyeGeobuf, grid_param.geoBufLen, 1, geo_combine_fd);
			} else {
				sprintf(func_name, "geo");
				ret = get_view_file_name(&gd_param_ptr->common_param, 0, 0, func_name,
										(char*)MODE_NAME, i, geo_full_name);
				if (ret)
					goto to_out;
				geo_fp = fopen(geo_full_name, "wb");
				if (geo_fp) {
					fwrite(grid_param.fisheyeGeobuf, grid_param.geoBufLen, 1, geo_fp);
					fclose(geo_fp);
					printf("Save %s Success\n", geo_full_name);
				} else {
					printf("Create geo file failed %s !!!\n", geo_full_name);
					goto to_out;
				}
				geo_fp = NULL;
			}
		}
	}

to_out:
	if (is_local_combine && grid_combine_fd) {
		fclose(grid_combine_fd);
		grid_combine_fd = NULL;
		printf("Save %s Success\n", grid_full_name);
	}
	if (is_local_combine && geo_combine_fd) {
		fclose(geo_combine_fd);
		geo_combine_fd = NULL;
		printf("Save %s Success\n", geo_full_name);
	}
	if (grid_param.fisheyeGridbuf)
		free(grid_param.fisheyeGridbuf);
	if (grid_param.fisheyeGeobuf)
		free(grid_param.fisheyeGeobuf);

	return ret;
}

static int dv_proc(fe_view_param_t *view_param_ptr)
{
	int ret = 0;

	fe_gd_param_t gd_param;


	memset(&gd_param, 0, sizeof(gd_param));
	gd_param.common_param = view_param_ptr->common_param;

	if (!view_param_ptr->common_param.is_global_combine) {
		gd_param.common_param.grid_size = LOCAL_GRID_SIZE;
		gd_param.common_param.out_width = LOCAL_OUT_WIDTH;
		gd_param.common_param.out_height = LOCAL_OUT_HEIGHT;

		print_split_line();
	}
	gd_param.angle_unit = LOCAL_ANGLE_UNIT;

	ret = dv_gen_griddata(&gd_param);

	return ret;
}

fe_view_t fv_downview = {
	.id_name = (char*)VIEW_ID,
	.max_idx = MAX_IDX,
	.ops = {
		.proc = dv_proc,
	}
};
