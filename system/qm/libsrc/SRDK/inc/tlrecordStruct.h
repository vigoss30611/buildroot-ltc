/******************************************************************************
******************************************************************************/

#ifndef __TLVIDEORECORDSTRUCT_H_
#define __TLVIDEORECORDSTRUCT_H_

#define MAX_CHANNEL_NUM						4
#define MAX_DETECTOR_NUM						4
#define NETCMD_RECORD       					10000

#define NETCMD_QUERY_RECDATE    				NETCMD_RECORD+1
#define	NETCMD_QUERY_RECFILE    				NETCMD_RECORD+2
#define	NETCMD_OPEN_RECFILE     				NETCMD_RECORD+3
#define NETCMD_CLOSE_RECFILE    				NETCMD_RECORD+4
#define	NETCMD_READ_RECFILE     				NETCMD_RECORD+5
#define	NETCMD_SEND_FRAME	     				NETCMD_RECORD+6
#define NETCMD_PAUSE_RECORD 	   				NETCMD_RECORD+7
#define	NETCMD_RESUME_RECORD     			NETCMD_RECORD+8
#define NETCMD_DELETE_RECFILE					NETCMD_RECORD+9
#define NETCMD_BACKUP_RECFILE_LOCAL			NETCMD_RECORD+10
#define NETCMD_BACKUP_RECFILE_REMOTE		NETCMD_RECORD+11
#define NETCMD_QUERY_RECFILE_LIST				NETCMD_RECORD+12

#define	NETCMD_GET_RECORD_PARAM					NETCMD_RECORD+101
#define	NETCMD_GET_MANUAL_RECORD_PARAM			NETCMD_RECORD+102
#define	NETCMD_GET_TIMER_RECORD_PARAM			NETCMD_RECORD+103
#define	NETCMD_GET_DETECTOR_RECORD_PARAM		NETCMD_RECORD+104
#define	NETCMD_GET_VIDEOMOVE_RECORD_PARAM		NETCMD_RECORD+105
#define	NETCMD_GET_RECORD_STATUS				NETCMD_RECORD+106
#define	NETCMD_GET_HARDDISK_INFO				NETCMD_RECORD+107
#define	NETCMD_GET_FDISK_PROCESS				NETCMD_RECORD+108

#define	NETCMD_SET_RECORD_PARAM					NETCMD_RECORD+201
#define	NETCMD_SET_MANUAL_RECORD_PARAM			NETCMD_RECORD+202
#define	NETCMD_SET_TIMER_RECORD_PARAM			NETCMD_RECORD+203
#define	NETCMD_SET_DETECTOR_RECORD_PARAM		NETCMD_RECORD+204
#define	NETCMD_SET_VIDEOMOVE_RECORD_PARAM		NETCMD_RECORD+205
#define	NETCMD_SET_HARDDISK_FDISK				NETCMD_RECORD+206
#define	NETCMD_SET_HARDDISK_FORMAT				NETCMD_RECORD+207
#define NETCMD_SET_VIDEO_STATUS					NETCMD_RECORD+208
#define NETCMD_SET_DETECTOR_STATUS				NETCMD_RECORD+209

typedef struct
{
    int nChannelNo;
    int nYear;
    int nMonth;
    int nDisk;
}QUERY_RECDATE;

//ÍšµÀÄÚÂŒÏñÎÄŒþÓŠŽð
typedef struct
{
	unsigned long lYear;
	unsigned long lMonth;
	unsigned long lDay;
	unsigned long lDisk;
}ACK_RECDATE;


//²éÑ¯ÍšµÀÊ±Œä£šÄê£¬ÔÂ£¬ÈÕ£©ÄÚµÄËùÓÐÂŒÏñÎÄŒþ
typedef struct
{
    unsigned long lChannel;
    unsigned long lNum;
    unsigned long lType; // [zhb][add][2006-03-25]
    ACK_RECDATE recDate[2];
}QUERY_RECFILE;

typedef struct
{
	unsigned long nSize;
	unsigned long nPlayTime;
	char szFileName[128];

	unsigned long reserve;
}REC_FILE_ATTR;

typedef struct _READ_REC_FILE
{
    unsigned long   nFlag;
    unsigned long   nCommand;
    char szFileName[128];
    unsigned long   nFileLen;
    unsigned long   nReadPos;
    unsigned long   nReadLen;
    unsigned long   nReadResult;
}READ_REC_FILE;

typedef struct
{
    unsigned long nStartHour;
    unsigned long nStartMin;
    unsigned long nEndHour;
    unsigned long nEndMin;
    char		  szFileName[128];
}FILE_NAME;

//·µ»ØµÄ²éÑ¯œá¹û
typedef struct _FILE_SEGMENT
{
	unsigned long lChannel;
	unsigned long lYear;
	unsigned long lMonth;
	unsigned long lDay;
	unsigned long lDisk;
	unsigned long nStartHour;
	unsigned long nStartMin;
	unsigned long nStartSecond;
	unsigned long nEndHour;
	unsigned long nEndMin;
	unsigned long nEndSecond;
}FILE_SEGMENT;

typedef struct
{
    int			   nChannelNo;
    unsigned long  lYear;
    unsigned short lMonth;
    unsigned short lDay;
    
    unsigned long  lFileNum;
    unsigned long  lPacketNo;
}ACK_RECFILE;

typedef struct
{
	unsigned long nFlag;
	int			  bReadFile;
	int			  nPosition;
	int			  nGetLen;
}GET_DATA;


#define MAX_TIME_SEGMENT		2

// Ê±Œä¶Î
typedef struct __TIME_SEGMENT
{
	int start_hour;		//ÂŒÏñµÄ¿ªÊŒÊ±Œä-Ð¡Ê±
	int start_minute;	//ÂŒÏñµÄ¿ªÊŒÊ±Œä-·ÖÖÓ
	int end_hour;		//ÂŒÏñµÄœáÊøÊ±Œä-Ð¡Ê±
	int end_minute;		//ÂŒÏñµÄœáÊøÊ±Œä-·ÖÖÓ

}TIME_SEGMENT;

// Ò»ÌìÖÐµÄÊ±Œä¶Î
typedef struct __TIME_SEGMENTS
{
	int onFlag;	// [zhb][add][2006-04-17]
	TIME_SEGMENT time_segment[MAX_TIME_SEGMENT];

}TIME_SEGMENTS;	//Ã¿ÌìÓÐÁœžöÊ±Œä¶Î

// ¶šÊ±ÂŒÏñÍšµÀ²ÎÊý
typedef struct __TIMER_RECORD_CHANNEL
{
	TIME_SEGMENTS day[8];

}TIMER_RECORD_CHANNEL;

typedef struct __TIMER_RECORD_PARAM
{
	TIMER_RECORD_CHANNEL channel[MAX_CHANNEL_NUM];
	int reserve;
		
}TIMER_RECORD_PARAM;

// ÊÓÆµÒÆ¶¯±šŸ¯ÂŒÏñÍšµÀ²ÎÊý
typedef struct __VIDEOMOTION_TIME_SEGMENTS
{
	int record_time;
	int onFlag;	// [zhb][add][2006-04-17]
	int record_channel[MAX_CHANNEL_NUM];		// [zhb][add][2006-04-17]
	TIME_SEGMENT time_segment[MAX_TIME_SEGMENT];

}VIDEOMOTION_TIME_SEGMENTS;	//Ã¿ÌìÓÐÁœžöÊ±Œä¶Î

typedef struct __VIDEOMOTION_RECORD_CHANNEL
{
	VIDEOMOTION_TIME_SEGMENTS day[8];

}VIDEOMOTION_RECORD_CHANNEL;

typedef struct __VIDEOMOTION_RECORD_PARAM
{
	VIDEOMOTION_RECORD_CHANNEL channel[MAX_CHANNEL_NUM];
	int reserve;
		
}VIDEOMOTION_RECORD_PARAM;

// ÌœÍ·±šŸ¯ÂŒÏñÍšµÀ²ÎÊý
typedef struct __DETECTOR_TIME_SEGMENTS
{
	int record_time;
	int onFlag;	// [zhb][add][2006-04-17]
	int record_channel[MAX_CHANNEL_NUM];
	TIME_SEGMENT time_segment[MAX_TIME_SEGMENT];

}DETECTOR_TIME_SEGMENTS;	//Ã¿ÌìÓÐÁœžöÊ±Œä¶Î

typedef struct __DETECTOR_RECORD_CHANNEL
{
	DETECTOR_TIME_SEGMENTS day[8];

}DETECTOR_RECORD_CHANNEL;

typedef struct __DETECTOR_RECORD_PARAM
{
	DETECTOR_RECORD_CHANNEL channel[MAX_DETECTOR_NUM];
	int reserve;
		
}DETECTOR_RECORD_PARAM;


// ÊÖ¶¯ÂŒÏñ
typedef struct __NORMALRECORD
{
	int channel;
	int record_flag;				//Æô¶¯-Í£Ö¹ÂŒÏñ
	int record_time;				//ÂŒÏñÊ±Œä
	int cur_record_status;			//µ±Ç°ÂŒÏñ×ŽÌ¬

	int reserve;	
}MANUAL_RECORD_PARAM;

// ÂŒÏñÎÄŒþ±ž·Ý
typedef struct __RECORDFILE_BACKUP
{
	unsigned long lChannel;
	unsigned long lYear;
	unsigned long lMonth;
	unsigned long lDay;
	unsigned long lDisk;
	unsigned long nStartHour;
	unsigned long nStartMin;
	unsigned long nStartSecond;
	unsigned long nEndHour;
	unsigned long nEndMin;
	unsigned long nEndSecond;	
	
	int reserve;	
}RECORDFILE_BACKUP;

// ÂŒÏñÎÄŒþÔ¶³Ì»Ø·Å
typedef struct __RECORDFILE_PLAYBACK
{
	unsigned long lChannel;
	unsigned long lYear;
	unsigned long lMonth;
	unsigned long lDay;
	unsigned long lDisk;
	unsigned long nStartHour;
	unsigned long nStartMin;
	unsigned long nStartSecond;
	unsigned long nEndHour;
	unsigned long nEndMin;
	unsigned long nEndSecond;
	
	int reserve;	
}RECORDFILE_PLAYBACK;

// ÂŒÏñ²ÎÊý
typedef struct __RECORD_PARAM_CHANNEL
{
	int cover_mode; 		/* 0:ž²žÇ,1:²»ž²žÇ */
	int video_standard;		/* 0:NTSC, 1:PAL */
	int video_format; 		/* 0:D1,1:HalfD1,2:CIF,3:QCIF */	
	int bit_flow_type; 		/* 0:VBR,    1:CBR */
	int record_quality; 	/* ÂŒÏñÖÊÁ¿:  0: --- 9 Œ¶ */
	int frame_rate; 		/* ÂŒÏñÖ¡ÂÊ:  1 ~ 25(PAL) 1~30(NTSC) */
	
	int audio_flag;			/* 0:²»ŽøÒôÆµÊýŸÝ ,1:ŽøÒôÆµÊýŸÝ */
	int audio_rate;
	int audio_channel;
	int audio_bits;
	
	int reserve[16];	
}RECORD_PARAM_CHANNEL;

typedef struct __RECORD_PARAM
{
	RECORD_PARAM_CHANNEL channel[MAX_CHANNEL_NUM];
	
}RECORD_PARAM;

// ÂŒÏñËùÓÐµÄÉèÖÃ²ÎÊý
typedef struct __RECORDER_SETUP
{
	RECORD_PARAM record_param;
	TIMER_RECORD_PARAM timer_record_param;
	VIDEOMOTION_RECORD_PARAM videomotion_record_param;
	DETECTOR_RECORD_PARAM detector_record_param;	
}RECORDER_SETUP;

// Ó²ÅÌÐÅÏ¢
typedef struct __HARDDISK_INFO
{
	unsigned int index;
	unsigned int totalSize;
	unsigned int usedSize;
	unsigned int availableSize;
	unsigned int usedPercent;
	
	unsigned int formatFlag[2];	
	
	int reserve;
}HARDDISK_INFO;

// ·ÖÇø²ÎÊý
typedef struct __HD_FDISK_PARAM
{
	unsigned int index;
	unsigned int dataPartionSize;
	unsigned int backupPartionSize;
	
	int reserve;
}HD_FDISK_PARAM;

// žñÊœ»¯²ÎÊý
typedef struct __HD_FORMAT_PARAM
{
	unsigned int index;
	unsigned int partionNo;
	
	int reserve;
}HD_FORMAT_PARAM;

// ÎÄŒþÁÐ±í
typedef struct __REC_FILE_LIST
{
    unsigned long   nFlag;
    unsigned long   nFileNum;
    char            szFileName[60][128];
}REC_FILE_LIST;

typedef struct __MP4_FRAME
{
    void *image;	
	void *bitstream;
	int length;			           
    int iFrameType;
    int bMotion;
    int iChannel;
    
}MP4_FRAME;

#define KILOBYTE			1024	//K  1024
#define MILOBYTE			1048576	//M  1024 * 1024

#define MAX_DISK_NUM		8
#define MAX_PARTITION_NUM	2

typedef struct __FDISK_INFO
{
	int type;						//0: fdisk  1:format
	int disk_no;
	int partition_no;
	int data_partition;
	int backup_partition;
}FDISK_INFO,*PFDISK_INFO;

/* ÏµÍ³ËùÓÐÓ²ÅÌŒ°·ÖÇøÐÅÏ¢ */

typedef struct __PARTITION_INFO
{
	int mount_flag;					// MOUNT
	unsigned int total_size;		// ×ÜÓÐÈÝÁ¿£¬µ¥Î»ÎªM
	unsigned int used_size;			// ÒÑÓÃÈÝÁ¿£¬µ¥Î»ÎªM
	unsigned int availabe_size;		// ¿ÉÓÃÈÝÁ¿£¬µ¥Î»ÎªM
	unsigned int used_parent;		//
	unsigned int type;   			/* 1: ÊýŸÝ·ÖÇø 2: ±ž·Ý·ÖÇø 3: ÆäËü  */
}PARTITION_INFO,*PPARTITION_INFO;

typedef struct __DISK_INFO
{
	PARTITION_INFO	partition_info[MAX_PARTITION_NUM];
	int	formated;					/* ÊÇ·ñÒÑŸ­žñÊœ»¯0:no 1:yes*/
}DISK_INFO,*PDISK_INFO;

typedef struct __HARD_DISK_INFO
{
	int cur_disk_no; 				/* ÕûžöÏµÍ³×î¶à8¿éÓ²ÅÌ :0-7 */
	int cur_partition_no; 			/* Ò»žöÓ²ÅÌ×î¶à2žö·ÖÇø: 0-1 */
	int hard_disk_num; 				/* ÏµÍ³×Ü¹²ÓÐÐ§Ó²ÅÌÊý 	   */
	DISK_INFO disk_info[MAX_DISK_NUM];
}HARD_DISK_INFO,*PHARD_DISK_INFO;

typedef struct __RECORDER_CMD_PARAM
{
	int iChannel;
	int opt;
	
	union 
	{
		ACK_RECDATE recDate;
		QUERY_RECFILE recFile;
		FILE_SEGMENT fileSegment;
				
		//unsigned char frameBuffer[100*1024];
		unsigned char *frameBuffer;				// [zhb][add][2006-03-15]
		
		MANUAL_RECORD_PARAM manualRecord;
		RECORD_PARAM_CHANNEL recordParam;
		TIMER_RECORD_CHANNEL timerRecord;
		DETECTOR_RECORD_CHANNEL detectorRecord;
		VIDEOMOTION_RECORD_CHANNEL videoMotionRecord;
		
		HARD_DISK_INFO hdInfo;		
		FDISK_INFO fdiskInfo;
		
		int videoStatus;

		int detectorStatus;
		
	}param;

	int iReserve;
}RECORDER_CMD_PARAM;


typedef int  (*ForceIFrameFun)(int flag, int channelId);
#endif
