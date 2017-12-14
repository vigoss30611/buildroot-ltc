#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>

#include "libgridtools.h"
#include "fe_k_proc.h"
#include "fe_k_misc.h"

using namespace std;

static struct timespec start_time;

void print_split_line()
{
	printf("===========================\n");
}


int get_view_file_name(fe_common_param_t *param_ptr, int x, int y,
                      char *func_name, char *mode_name, int idx,
                      char *out_name)
{
	char set_str[20];
	char mode_str[50];

	/*
	(1) lc_Name_CoordX-CoordY_hermiteGridSize_inputWxH_outputWXH_function_mode_GridIdx.bin
	(2) lc_Name_CoordX-CoordY_hermiteGridSize_inputWxH_outputWXH_function_GridIdxMax_set.bin
	(3) lc_Name_CoordX-CoordY_hermiteGridSize_inputWxH_outputWXH_function_mode_GridIdxMax_set.bin
	lc_d304_960-540_hermite32_1080x1080-960x540_fisheyes_CenterScaleUp_200.bin
	lc_d304_960-540_hermite32_1080x1080-960x540_geo_CenterScaleUp_200.bin
	*/
	if (mode_name) {
		sprintf(mode_str, "_%s",mode_name);
		if (param_ptr->is_local_combine)
			sprintf(set_str,"_set");
		else
			memset(set_str, 0, sizeof(set_str));
	} else {
		sprintf(set_str,"_set");
		memset(mode_str, 0, sizeof(mode_str));
	}

	sprintf(out_name, "%slc_%s_%d-%d_hermite%d_%dx%d-%dx%d_%s%s_%d%s.bin",
		param_ptr->dst_file_path,
		param_ptr->id_name, x, y, param_ptr->grid_size,
		param_ptr->in_width, param_ptr->in_height,
		param_ptr->out_width, param_ptr->out_height,
		func_name,
		mode_str,
		idx,
		set_str
	);

	return 0;
}

void tm_start()
{
	clock_gettime(CLOCK_MONOTONIC, &start_time);
}

long tm_get_ms()
{
	struct timespec end_time;
	long diff = 0;

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    diff = 1000 * (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000;

	return diff;
}

void tm_print_diff_ms(char *str)
{
	printf("%s (time:%ld ms)\n", str, tm_get_ms());
}

int fe_save_file(fe_common_param_t *param_ptr, char *part_filename_ptr, char *out_full_name)
{
	int ret = -1;
	FILE *fp = NULL;
	int width = 0;
	int height = 0;
	char *path_ptr = NULL;
	int size = 0;
	char *buf_ptr = NULL;
	int dot_index = 0;
	char *dot_after = (char*)".yuv";
	char before_str[256];


	if (!param_ptr || !part_filename_ptr || !out_full_name)
		return ret;

	path_ptr = param_ptr->save_src_yuv_path;
	width = param_ptr->in_width;
	height = param_ptr->in_height;
	size = width * height * 3 / 2;
	buf_ptr = param_ptr->yuv_buf;


	memset(before_str, 0, sizeof(before_str));
	dot_index = strcspn(part_filename_ptr, (char*)".");
	if (dot_index) {
		strncpy(before_str, part_filename_ptr, dot_index);
		dot_after = strchr(part_filename_ptr, '.');
	} else {
		sprintf(before_str, "fisheye_src");
	}

	sprintf(out_full_name, "%s%s_%dx%d%s", path_ptr, before_str, param_ptr->in_width, param_ptr->in_height, dot_after);
	fp = fopen(out_full_name, "wb");
	if (fp) {
		fwrite(buf_ptr, size, 1, fp);
		fclose(fp);
		ret = 0;
	}

	return ret;
}
