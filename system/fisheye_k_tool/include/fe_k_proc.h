#ifndef __FE_K_TOOL_H__
#define __FE_K_TOOL_H__


#if 0
typedef enum fe_gd_show_type{
    FE_GD_SHOW_NONE,
    FE_GD_SHOW_DISPLAY,
    FE_GD_SHOW_JPEG
}fe_gd_show_type_e;
#endif



typedef struct {
	char *id_name;
	char *yuv_buf;
	int in_width;
	int in_height;
	int out_width;
	int out_height;
	int grid_size;

	FILE *grid_fd;
	FILE *geo_fd;
	char *dst_file_path;
	int is_save_geo_file;

	int is_local_combine;
	int is_global_combine;

	int align_size;
	char *save_src_yuv_path;
	int center_error_threshold;
}fe_common_param_t;

typedef struct fe_gd_param{
	fe_common_param_t common_param;
	int angle_unit;
}fe_gd_param_t;

typedef struct fe_src_yuv{
	int width;
	int height;
	char *buf_ptr;
}fe_src_yuv_t;


typedef struct {
	fe_common_param_t common_param;
	char global_grid_name[256];
	char global_geo_name[256];
}fe_view_param_t;


typedef struct {
	int (*proc)(fe_view_param_t *param_ptr);
}fe_ops_t;


typedef struct {
	char *id_name;
	int max_idx;
	fe_ops_t ops;
}fe_view_t;



int do_fe_k(char* in_js_ptr);

#endif
