#ifndef __ADAS_GUI_BINDER_H__
#define __ADAS_GUI_BINDER_H__

#include "stdlib.h"
#include "stdio.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>

#define ADAS_GUI_BINDER_KEY    	0x5512345d
#define ADAS_FCWS_TARGET_MAX	3

typedef enum ___ADAS_GUI_COMMAND
{
	ADAS_GUI_CMD_LDWS_START,
	ADAS_GUI_CMD_LDWS_STOP,
    ADAS_GUI_CMD_LDWS_CONFIG,
    ADAS_GUI_CMD_FCWS_START,
    ADAS_GUI_CMD_FCWS_STOP,
    ADAS_GUI_CMD_FCWS_CONFIG,
    ADAS_GUI_CMD_ADAS_RESULT,
    ADAS_GUI_CMD_SHOW_RESOLUTION,
    ADAS_GUI_CMD_ADAS_RESTART,
    ADAS_GUI_CMD_MAX,
}ADAS_GUI_COMMAND;

typedef enum _ADAS_GUI_PORT
{
    GUI_PORT = 1,
    ADAS_PORT,
    PORT_MAX,
}ADAS_GUI_PORT;

typedef enum __ADAS_LDWS_STATE
{
	ADAS_LDWS_STATE_NORMAL          = 0,
	ADAS_LDWS_STATE_DEPARTURE_LEFT,
	ADAS_LDWS_STATE_DEPARTURE_RIGHT,
	ADAS_LDWS_STATE_MAX,
}ADAS_LDWS_STATE;

typedef enum __ADAS_LDWS_LANEMARK
{
	ADAS_LDWS_LANEMARK_NOT_FIND        	= 0,
	ADAS_LDWS_LANEMARK_LEFT_FIND 		= 1,
	ADAS_LDWS_LANEMARK_RIGHT_FIND 		= 2,
	ADAS_LDWS_LANEMARK_BOTH_FIND 		= 3,
	ADAS_LDWS_LANEMARK_MAX,
}ADAS_LDWS_LANEMARK;

typedef struct __ADAS_SHOW_RESOLUTION{
	unsigned int 	screen_width;
	unsigned int 	screen_height;
}ADAS_SHOW_RESOLUTION;

typedef struct __ADAS_LDWS_CONFIG{
	unsigned int 	camera_position_x;
	unsigned int 	camera_position_y;
	unsigned int 	veh_hood_top_y;
}ADAS_LDWS_CONFIG;

typedef struct __ADAS_LDWS_RESULT{
    ADAS_LDWS_STATE 		veh_ldws_state;
	ADAS_LDWS_LANEMARK  	ldws_lanemark;
	int 					left_lane_x1;
	int 					left_lane_y1;
	int 					left_lane_x2;
	int 					left_lane_y2;
	int 					right_lane_x1;
	int 					right_lane_y1;
	int 					right_lane_x2;
	int 					right_lane_y2;
}ADAS_LDWS_RESULT;

typedef struct __ADAS_FCWS_CONFIG{
	unsigned int 			camera_position_x;
	unsigned int 			camera_position_y;
	unsigned int 			veh_hood_top_y;
	double					camera_height;
	double					camera_focal;
}ADAS_FCWS_CONFIG;

typedef struct __ADAS_FCWS_TARGET_INFO{
	unsigned int			target_distance;
	unsigned int			target_left;
	unsigned int			target_top;
	unsigned int			target_right;
	unsigned int			target_bottom;
}ADAS_FCWS_TARGET_INFO;

typedef struct __ADAS_FCWS_RESULT{
	unsigned int			target_num;
	ADAS_FCWS_TARGET_INFO	target_info[ADAS_FCWS_TARGET_MAX];
}ADAS_FCWS_RESULT;

typedef struct __ADAS_PROC_RESULT{
	ADAS_LDWS_RESULT		adas_ldws_result;
	ADAS_FCWS_RESULT		adas_fcws_result;
}ADAS_PROC_RESULT;

typedef struct __ADAS_GUI_MSG_INFO  {
    long 					adas_gui_msg_to;   	/* 1 to gui, 2 to adas */
    long 					adas_gui_msg_from;  /* 1 to gui, 2 to adas */

    ADAS_GUI_COMMAND 		adas_gui_command;
    union{
        ADAS_LDWS_CONFIG 		adas_ldws_config;
        ADAS_FCWS_CONFIG 		adas_fcws_config;
        ADAS_PROC_RESULT		adas_proc_result;
		ADAS_SHOW_RESOLUTION	adas_show_resolution;
    };
}ADAS_GUI_MSG_INFO;

#define ADAS_GUI_MSG_SIZE (sizeof(ADAS_GUI_MSG_INFO) - sizeof(long))

#endif
