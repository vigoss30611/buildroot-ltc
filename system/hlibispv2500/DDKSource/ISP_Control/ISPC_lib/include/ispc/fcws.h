#ifndef FCWS_H
#define FCWS_H

//#ifdef __cplusplus
//extern "C" {
//#endif

typedef enum fcws_err_e
{
	FCWS_ERR_NONE              = 0,
	FCWS_ERR_NULL_PARAMETER    = 1,
	FCWS_ERR_NOT_READY         = 2,
	FCWS_ERR_NOT_INIT          = 3,
	FCWS_ERR_NOT_SUPPROT       = 4,
	FCWS_ERR_INVALID_PARAMETER = 5,
	FCWS_ERR_OUT_OF_MEMORY     = 6
}fcws_err;

typedef enum _fcws_car_position_e
{
	FCWS_CAR_POSITION_FORWARD = 0,
	FCWS_CAR_POSITION_RIGHT   = 1,
	FCWS_CAR_POSITION_LEFT    = 2,
}fcws_car_position_e;

typedef enum _fcws_state_e
{
	FCWS_STATE_LOSE = 0,
	FCWS_STATE_FIND = 1
}fcws_state_e;

typedef struct _fcws_info_t {
	int state     [3];
	int position  [3];
	int car_x     [3];
	int car_y     [3];
	int car_width [3];
	int car_height[3];
	//int distance  [3];
	double distance  [3];
} fcws_info_t;

typedef struct _fcws_params_t {
	int image_width  ;
	int image_height  ;
	int image_rowsize;
	
	int    detect_upper;
	int    detect_lower;
	int    camera_position;
	double width_rate  ;
	
	int    camera_focal       ;
	double camera_height      ;
} fcws_params_t;


extern fcws_err fcws_init(fcws_params_t *params);
extern fcws_err fcws_deinit();
extern fcws_err fcws_set_roi(int  x, int y , int roi_width , int  roi_height);
extern fcws_err fcws_get_roi(int *x, int *y, int *roi_width, int *roi_height);
extern fcws_err fcws_set_params(fcws_params_t *params);
extern fcws_err fcws_get_params(fcws_params_t *params);
extern fcws_err fcws_get_result(fcws_info_t *info);
extern fcws_err fcws_process(const void *src, const void *integral,const void *ldws_info);

//#ifdef __cplusplus
//}
//#endif

#endif
