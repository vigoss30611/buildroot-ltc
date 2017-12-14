/******************************************************************************
******************************************************************************/




#ifndef __NET_COMM_HEAD_H
#define __NET_COMM_HEAD_H


//#include "tm3kapp.h"

/* ?????*/


#define NETCMD_NUMBER       6000

#define NETCMD_USER_MSG     	NETCMD_NUMBER+1
#define NETCMD_QUERY_RECDATE    NETCMD_NUMBER+2
#define	NETCMD_QUERY_RECFILE    NETCMD_NUMBER+3
#define	NETCMD_OPEN_RECFILE     NETCMD_NUMBER+4
#define NETCMD_CLOSE_RECFILE    NETCMD_NUMBER+5
#define	NETCMD_READ_RECFILE     NETCMD_NUMBER+6
#define	NETCMD_UPDATE_VER		NETCMD_NUMBER+7
//
#define	NETCMD_GET_DSP_CONFIG				NETCMD_NUMBER+8
#define	NETCMD_GET_SYSTEM_CONFIG			NETCMD_NUMBER+9
#define	NETCMD_GET_NORMAL_RECORD			NETCMD_NUMBER+10
#define	NETCMD_GET_TIMERECORDDLG_CHANNEL	NETCMD_NUMBER+11
#define	NETCMD_GET_RECORD_PARAM_DLG_CHANNEL	NETCMD_NUMBER+12
#define	NETCMD_GET_VIDEOMOVEDLG_CHANNEL		NETCMD_NUMBER+13
#define	NETCMD_GET_VIDEOLOSE_PARAM			NETCMD_NUMBER+14
#define	NETCMD_GET_DETECTORALARMDLG_ALARMINPUT		NETCMD_NUMBER+15
#define	NETCMD_GET_ONEPICDLG_PARAM			NETCMD_NUMBER+16
#define	NETCMD_GET_FOURPICDLG_PARAM			NETCMD_NUMBER+17
#define	NETCMD_GET_NINE_PICDLG_PARAM		NETCMD_NUMBER+18
#define	NETCMD_GET_SIXTEEN_PICDLG_PARAM		NETCMD_NUMBER+19
#define	NETCMD_GET_AUTO_PIC_SWITCH			NETCMD_NUMBER+20
#define	NETCMD_GET_HOSTSETUPDLG_PARAM		NETCMD_NUMBER+21
#define	NETCMD_GET_CHANNELSETUP_PARAM		NETCMD_NUMBER+22
#define	NETCMD_GET_COM_PARAM				NETCMD_NUMBER+23
#define	NETCMD_GET_NET_PARAM				NETCMD_NUMBER+24
#define	NETCMD_GET_CAMERA_TRACK_PARAM		NETCMD_NUMBER+25
#define	NETCMD_GET_USER_DATA_T				NETCMD_NUMBER+26

#define	NETCMD_SET_DSP_CONFIG				NETCMD_NUMBER+27
#define	NETCMD_SET_SYSTEM_CONFIG			NETCMD_NUMBER+28
#define	NETCMD_SET_NORMAL_RECORD			NETCMD_NUMBER+29
#define	NETCMD_SET_TIMERECORDDLG_CHANNEL	NETCMD_NUMBER+30
#define	NETCMD_SET_RECORD_PARAM_DLG_CHANNEL	NETCMD_NUMBER+31
#define	NETCMD_SET_VIDEOMOVEDLG_CHANNEL		NETCMD_NUMBER+32
#define	NETCMD_SET_VIDEOLOSE_PARAM			NETCMD_NUMBER+33
#define	NETCMD_SET_DETECTORALARMDLG_ALARMINPUT		NETCMD_NUMBER+34
#define	NETCMD_SET_ONEPICDLG_PARAM			NETCMD_NUMBER+35
#define	NETCMD_SET_FOURPICDLG_PARAM			NETCMD_NUMBER+36
#define	NETCMD_SET_NINE_PICDLG_PARAM		NETCMD_NUMBER+37
#define	NETCMD_SET_SIXTEEN_PICDLG_PARAM		NETCMD_NUMBER+38
#define	NETCMD_SET_AUTO_PIC_SWITCH			NETCMD_NUMBER+39
#define	NETCMD_SET_HOSTSETUPDLG_PARAM		NETCMD_NUMBER+40
#define	NETCMD_SET_CHANNELSETUP_PARAM		NETCMD_NUMBER+41
#define	NETCMD_SET_COM_PARAM				NETCMD_NUMBER+42
#define	NETCMD_SET_NET_PARAM				NETCMD_NUMBER+43
#define	NETCMD_SET_CAMERA_TRACK_PARAM		NETCMD_NUMBER+44
#define	NETCMD_ADD_USER_DATA_T				NETCMD_NUMBER+45
#define	NETCMD_MOD_USER_DATA_T				NETCMD_NUMBER+46
#define NETCMD_YUNTAI_CONTROL 				NETCMD_NUMBER+47


/*
//???????
typedef struct
{
    unsigned long  lYear;
    unsigned long  lMonth;
    unsigned long  lDay;
	unsigned long  lDisk;
}ACK_RECDATE;


//?????????????
typedef struct
{
    unsigned long   lChannel;
	unsigned long	lNum;
    ACK_RECDATE		recDate[2];
}QUERY_RECFILE;

//???
typedef struct _FILE_SEGMENT
{
	unsigned long	lChannel;
    unsigned long	lYear;
    unsigned long	lMonth;
    unsigned long	lDay;
	unsigned long	lDisk;
    unsigned long   nStartHour;
    unsigned long   nStartMin;
	unsigned long	nStartSecond;
    unsigned long   nEndHour;
    unsigned long   nEndMin;
	unsigned long	nEndSecond;
}FILE_SEGMENT;
*/


//TM9000???

#define MAX_ENC_DSP_NUM  		8
#define MAX_CHANNEL_NUM  		16
#define MAX_TIME_SEGMENT			4
#define MAX_WEEK_DAYS			7
#define MAX_ALARM_INPUT_NUM		8
#define MAX_ALARM_OUTPUT_NUM	4
#define MAX_CHANNEL_TITLE		36
#define MAX_PROTOCOL_TEXT		48
#define MAX_USERNAME_TEXT		48
#define MAX_PASSWORD_TEXT		48
#define MAX_COM_NUM				4


//1	????
//????
typedef struct __DSP_CONFIG
{
	int video_channel_num;				//????
	int audio_channel_num;				//????=????
	int audio_sample_rate;				//?????
	int audio_bitrate;					//????
	int video_format; 					// 0:D1, 1:HalfD1, 2:CIF, 3:QCIF
	int reserve[10];					//?
}DSP_CONFIG;

typedef struct __SYSTEM_CONFIG
{
	int dsp_num;						//DSP?
	int channel_num;					//???
	int reserve[6];						//6+2 =8
	DSP_CONFIG dsp_config[MAX_ENC_DSP_NUM];		//15*8=120
}SYSTEM_CONFIG;

//????
//static SYSTEM_CONFIG SYSTEM_config;


//2	????
//????
typedef struct __NORMAL_RECORD
{
	int record_flag;				//?-????
	int record_time;				//????
	int cur_record_status;			//?????
}NORMAL_RECORD;

//???
//NORMAL_RECORD NORMAL_RECORD_record[MAX_CHANNEL_NUM];

//3	????
typedef struct __TIMERECORDDLG_TIME_SEGMENT
{
	int start_hour;					
	int start_minute;
	int end_hour;
	int end_minute;
}TIMERECORDDLG_TIME_SEGMENT;
typedef struct __TIMERECORDDLG_WEEK
{
	TIMERECORDDLG_TIME_SEGMENT	time_segment[MAX_TIME_SEGMENT];
}TIMERECORDDLG_WEEK;

typedef struct __TIMERECORDDLG_CHANNEL
{
	TIMERECORDDLG_WEEK week[MAX_WEEK_DAYS];
	
}TIMERECORDDLG_CHANNEL;

//TIMERECORDDLG_CHANNEL channel[MAX_CHANNEL_NUM];


//4	???
typedef struct __RECORD_PARAM_DLG_CHANNEL
{
	int cover_mode; 			/* 0:?1:?*/
	int video_format; 			/* 0:D1,1:HalfD1,2:CIF,3:QCIF */
	int bit_flow_type; 			/* 0:VBR,1:CBR */
	int record_quality; 			/* ???:  0:??,1:??2:?3:??*/
									int frame_rate; 			/* ????:  1 ~ 25(PAL) 1~30(NTSC)  */
									int voice_flag; 			/* 0:????,1:????*/
									int date_flag; 				/* 0:??? 1:???*/
									int time_flag; 				/* 0:?? 1:??*/
									int week_flag; 			/* 0:??? 1:???*/
									int channel_flag; 			/* 0:???? 1:????*/
									int dual_encoder;			/* ??? : close, 1:QCIF, 2:CIF  */
									int alarm_record_quality;	/* ???:  0:??,1:??2:?3:??*/
									int reserve[20];
}RECORD_PARAM_DLG_CHANNEL;
//RECORD_PARAM_DLG_CHANNEL channel[MAX_CHANNEL_NUM];

//5	???
typedef struct __VIDEOMOVEDLG_TIME_SEGMENT
{	
	int start_hour;
	int start_minute;
	int end_hour;
	int end_minute;
}VIDEOMOVEDLG_TIME_SEGMENT;

typedef struct __VIDEOMOVEDLG_WEEK
{
	int sensitive;										//???	0 - 100
	int record_time;									//????
	int audio_flag;										//??
	int alarm_output[MAX_ALARM_OUTPUT_NUM];		//j??
	int clear_time;										//????
	char move_detect[9][11]; 	/* 44*36  CIF  16*16 */		//?????
	int record_channel;									//j???:?
	int reserve[7];			
	VIDEOMOVEDLG_TIME_SEGMENT	time_segment[MAX_TIME_SEGMENT];
}VIDEOMOVEDLG_WEEK;

typedef struct __VIDEOMOVEDLG_CHANNEL
{
	VIDEOMOVEDLG_WEEK week[MAX_WEEK_DAYS];
}VIDEOMOVEDLG_CHANNEL;
//VIDEOMOVEDLG_CHANNEL channel[MAX_CHANNEL_NUM];

//6	?????
typedef struct __VIDEOLOSE_PARAM
{
	int cur_channel_no;				//?????
	int copy_channel_no;			//?????
	int audio_alarm[MAX_CHANNEL_NUM];	
	int alarm_output[MAX_CHANNEL_NUM][MAX_ALARM_OUTPUT_NUM];
	int clear_time[MAX_CHANNEL_NUM];
	int reserve[6];
}VIDEOLOSE_PARAM;

//VIDEOLOSE_PARAM VIDEOLOSE_channel;

//7	 ?????
typedef struct __DETECTORALARMDLG_TIME_SEGMENT
{
	int start_hour;
	int start_minute;
	int end_hour;
	int end_minute;
}DETECTORALARMDLG_TIME_SEGMENT;

typedef struct __DETECTORALARMDLG_WEEK

{
	int clear_time;								//????
	int record_time;							//j????
	int audio_flag;								//??
	int record_channel[MAX_CHANNEL_NUM];	//j????
	int preset[MAX_CHANNEL_NUM];			//0:preset ??     1:track ?
	int position[MAX_CHANNEL_NUM];			//?? ???
	int output_channel[MAX_ALARM_OUTPUT_NUM];	//j???
	int reserve[9];					
	DETECTORALARMDLG_TIME_SEGMENT time_segment[MAX_TIME_SEGMENT];
}DETECTORALARMDLG_WEEK;

typedef struct __DETECTORALARMDLG_ALARMINPUT
{
	DETECTORALARMDLG_WEEK week[MAX_WEEK_DAYS];
}DETECTORALARMDLG_ALARMINPUT;

//	DETECTORALARMDLG_ALARMINPUT channel[MAX_ALARM_INPUT_NUM];




//8	 ????
//1 ?
typedef struct __ONEPICDLG_PARAM
{
	int use_flag;
	int value;
}ONEPICDLG_PARAM;
//		static ONEPICDLG_PARAM ONEPICDLG_param[20];
//2??
typedef struct __FOURPICDLG_PARAM
{
	int use_flag;
	int value;
}FOURPICDLG_PARAM;
//		static FOURPICDLG_PARAM FOURPICDLG_param[20];
//3??
typedef struct __NINE_PICDLG_PARAM

{
	int use_flag;
	int value[4];			
}NINE_PICDLG_PARAM;
//		static NINE_PICDLG_PARAM NINE_PICDLG_param[20];
//4???
typedef struct __SIXTEEN_PICDLG_PARAM
{
	int use_flag;
	int value[4];			
}SIXTEEN_PICDLG_PARAM;
//static SIXTEEN_PICDLG_PARAM SIXTEEN_PICDLG_param[20];

//5???
typedef struct __AUTO_PIC_SWITCH
{
	int one_pic_switch_time;  	
	int four_pic_switch_time; 
	int nine_pic_switch_time;
	int sixteen_pic_switch_time;
}AUTO_PIC_SWITCH;
/* 0:?,1:5??л,2:10??л,3:15??л,4:20??л,5:25??л */
//		static AUTO_PIC_SWITCH g_auto_pic_switch;


//9	???
typedef struct __HOST_TIME_SEGMENT
{
	int poweron_hour;
	int poweron_minute;
	int poweroff_hour;
	int poweroff_minute;
}HOST_TIME_SEGMENT;

typedef struct __HOSTSETUPDLG_PARAM
{
	int host_no;			//??
	int brightness;			//GUI ??
	int gui_transparent;		//GUI ???
	int language;			//0: china  1:English	
	int display_mode;		//0: pal    1:ntsc			
	int timer_poweron_en;	//0: close  1:open			??????
	int password_en;		//0: no password            1: enable password
	int reserve[5];	
	HOST_TIME_SEGMENT poweron[MAX_WEEK_DAYS][4];				
}HOSTSETUPDLG_PARAM;
//static HOSTSETUPDLG_PARAM HOSTSETUPDLG_param;
//? ?????????????

//10	???
typedef struct __CHANNELSETUP_PARAM
{
	int brightness;								//??
	int contrast;								//???
	int hue;									//??
	int saturation;								//??
	int videoprotect;							//???

	int baudrate;								//?????
	int reserve[10];							
	char title_text[MAX_CHANNEL_TITLE];		//????
	char protocol_text[MAX_PROTOCOL_TEXT];	//????
}CHANNELSETUP_PARAM;		
//	static CHANNELSETUP_PARAM CHANNELSETUP_channel_param[MAX_CHANNEL_NUM];
/*	

  **?????????????
  {
  
	}
*/

//11	???
typedef struct __COM_PARAM
{	
	int baud_rate;				//??
	int data_bit;				//??
	int stop_bit;				//??λ
	int check_type;			//У?
	int flow_ctrl; 				/* ?????????*/
	int reserve[11];			
}COM_PARAM;

//static COM_PARAM com_param[MAX_COM_NUM];
//?????????

//12	????
typedef struct __NET_PARAM
{
	char phy_addr[24];			//???
	char dns_addr[20];			//DNS IP
	char host_addr[20];			//?IP
	char pppoe_addr[20];		//PPPOE IP
	char username[MAX_USERNAME_TEXT];	//??
	char password[MAX_PASSWORD_TEXT];		//??
	char check_data;							//У?
	char ip_addr[20];							//IP
	char brd_addr[20];							//?IP
	char mask_addr[20];						//mask
	char gateway_addr[20];						//??
	char reserve[19];			
	int  port_no;								//???
	int  host_port;							//????
}NET_PARAM;
//????? 
//static NET_PARAM  net_param;

//13	???????
typedef struct __CAMERA_TRACK_PARAM
{
	int use_flag;			//
	int keep_time[16];
	int value[16];			//???
}CAMERA_TRACK_PARAM;

//		CAMERA_TRACK PARAM  camera_track[64];


//14	???
#define MAX_NAME_LEN	48
#define MAX_PWD_LEN	24
#define MAX_USER_NUM	20
typedef struct __USER_DATA_T
{
	char name[MAX_NAME_LEN];		//??
	char password[MAX_PWD_LEN];	//??
	int 	local_right;					//??
	int 	remote_right;					//???
	int 	use_flag;
}USER_DATA_T;

//USER_DATA_T  user_data[MAX_USER_NUM];

typedef struct _UNDERLAY_CTRL
{
    int     nChannel;//
    int     nCmdType;//1 up,2 down ,3 left ,4 right ...;
    int     nPreset;//
}UNDERLAY_CTRL;











#endif /* __REMOTECTRL_H */
