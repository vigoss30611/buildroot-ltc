
#ifndef LDWS_H
#define LDWS_H

//#ifdef __cplusplus
//extern "C" {
//#endif

typedef enum ldws_err_e
{
	LDWS_ERR_NONE              = 0,
	LDWS_ERR_NULL_PARAMETER    = 1,
	LDWS_ERR_NOT_READY         = 2,
	LDWS_ERR_NOT_INIT          = 3,
	LDWS_ERR_NOT_SUPPROT       = 4,
	LDWS_ERR_INVALID_PARAMETER = 5,
	LDWS_ERR_OUT_OF_MEMORY     = 6
}ldws_err;

typedef enum _ldws_car_type_e
{
	LDWS_CAR_TYPE_SMALL = 0,
	LDWS_CAR_TYPE_LARGE = 1
}ldws_car_type_e;

typedef enum _ldws_state_e
{
	LDWS_STATE_NORMAL          = 0,
	LDWS_STATE_DEPARTURE_LEFT  = 1,
	LDWS_STATE_DEPARTURE_RIGHT = 2
}ldws_state_e;

typedef enum _ldws_lanemark_e
{
	LDWS_LANEMARK_ALL_NOT_FIND        = 0,
	LDWS_LANEMARK_LEFT_RIGHT_FIND     = 1,
	LDWS_LANEMARK_LEFT_FIND_RIGHT_NOT = 2,
	LDWS_LANEMARK_LEFT_NOT_RIGHT_FIND = 3
}ldws_lanemark_e;

typedef struct _ldws_info_t {
	ldws_lanemark_e flag;
	int left_lane_x1;
	int left_lane_y1;
	int left_lane_x2;
	int left_lane_y2;
	int right_lane_x1;
	int right_lane_y1;
	int right_lane_x2;
	int right_lane_y2;
	int camera_offset;
	int lane_center_x;
	ldws_state_e ldws_state;
} ldws_info_t;

typedef struct _ldws_params_t {
	int image_width;
	int image_height;
	int image_rowsize;
	int detect_upper;
	int detect_lower;
	int camera_position;
	int max_detect_lane_width;
	int min_detect_lane_width;
	int min_detect_lane_mark_length;
	int alarm_time_sec;
	float sensitivity;
} ldws_params_t;

extern ldws_err ldws_deinit();
extern ldws_err ldws_set_roi(int x, int y, int roi_width, int roi_height);
extern ldws_err ldws_get_roi(int *x, int *y, int *roi_width, int *roi_height);
extern ldws_err ldws_set_params(ldws_params_t *params);
extern ldws_err ldws_get_params(ldws_params_t *params);
extern ldws_err ldws_init(ldws_params_t *params);
extern ldws_err ldws_process(const void *src, const void *integral);
extern ldws_err ldws_get_result(ldws_info_t *info);


//#ifdef __cplusplus
//}
//#endif

#endif
