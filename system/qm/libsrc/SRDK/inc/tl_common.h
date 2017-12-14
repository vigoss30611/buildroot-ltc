/******************************************************************************
******************************************************************************/

#ifndef _TL_COMMON_H
#define _TL_COMMON_H

#include <sys/prctl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <termios.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <pthread.h>
#include <linux/sem.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <linux/route.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <sys/prctl.h>

#include "basic.h"
#include "QMAPIType.h"
#include "tlevent.h"
#include "tl_input_osd_module.h"
#include "TLNVSDK.h"

#include "QMAPICommon.h"
#include "system_msg.h"
#include "QMAPINetSdk.h"
#include "QMAPI.h"

/* TL dvs project */
#define     BOARD_ONE_VIDEO_CHANNEL         1
#define     BOARD_TWO_VIDEO_CHANNEL         2
#define     BOARD_FOUR_VIDEO_CHANNEL        4

#define     ETH_DEV                         "eth0"

#define     TL_CONFIG_PATH                  "/config"
//#define     TL_CONFIG_PATH                  "."

/* watchdog */
#define TL_WTD_TIMEOUT                  60
#define TL_BASE_YEAR                1900

#if (defined(TL_HI3511_PLATFORM))
#define     TL_AUTHEN_DEV                   "/dev/misc/dms_authen0104"
#define TL_WDT_DEV                  "/dev/misc/watchdog"
#elif (defined(TL_HI3518_PLATFORM))
#define     TL_AUTHEN_DEV                   "/dev/dms_authen0104"
#define TL_WDT_DEV                  "/dev/watchdog"
#else
#define     TL_AUTHEN_DEV                   "/dev/dms_authen0104"
#define TL_WDT_DEV                  "/dev/watchdog"
#endif

/* audio */
#define TL_MAX_ADEC_CH              2
#define TL_MAX_AENC_LEN                 300*1024

#define BASENUM                                 10000   /* the offset of our command */

#define SEND_SERINFO_TO8051                 1


#define LOGBASE                                 900     /* the offset of log command */

#define BEGINTIME                               "00:00"     /* the default time of define time's begintime */
#define ENDTIME                                 "23:59"     /* the default time of define time's endtime */
#define DATENUM                             8

#define ADMIN                                   1       /* higher priority :admin */
#define USER                                        2       /* normal priority:user */
#define GUEST                                   3       /* lower priority:guest */
#define MAXPWDLEN                               32      /* the max passward lenth */

#define ALARMARRAYSIZE                          4       /* alarm array size */
#define MAXBEEPNUM                              6       /* max lenth of system alarm array */
#define MAXVIDEOCHANNEL                     4       /* max numbers of video channel */
#define MAXVIDEOALARMNUM                        4       /* max alarm numbers  per video channel */
#define MAXPROBENUM                         4       /* max number of probe */

#define MAXALARMSAVENUM                     10

#define ALARMOUTPUT                         0x0     /* from 8051's alarm output */
#define PANCTRL                                 0x01    /* control pan command */
#define RESETBSP                                    0x02    /* reset bsp15 */
#define INDICATORCTRL                           0x03    /* system alive message */
#define REQRIREDEFAULT                          0x04    /* set the system to default parameter */
#define GETSERVERINFO                           0x05 
//#define SET485RECV                                    0x06
#define GET485MSG                                   0x07
#define START485RECV                                0x08
#define STOP485RECV                                 0x09

#define ALARMCHANNEL1                           0x01  
#define ALARMCHANNEL2                           0x02  
#define ALARMCHANNEL3                           0x04
#define ALARMCHANNEL4                           0x08
#define MAXIOALARMNUM                          10      /* max number that in current system alarm array */
#define MOVETHRESHOLD                           100     /* video move alarm threshold */
#define VIDEOLOSTSHOLD                          100     /* video lost alarm threshold */

#define fFlagDefault                                  180
#define fFlagChange                                  181
#define fFlagSoftReset                               182
#define SOFTRESET                                    0
#define HARDRESET                                    1

typedef enum
{
    AUDIOTALK_NETPT_DMS = 1,
    AUDIOTALK_NETPT_TL,//
    AUDIOTALK_NETPT_MC,
    AUDIOTALK_NETPT_BUTT,
}AUDIOTALK_NETPT_TYPE_E;

/***********************************TL FIC8120 struct*******************************/
#define TL_VIDEO_VERSION    0x10000001
/* update dev */


/* Structure used to pass a frame to the encoder */
typedef struct
{
    void *bitstream;                   /*< [out]
                                    *
                                    * Output MPEG4 bitstream buffer pointer (physical addr.) */
    int length;                        /*< [out]
                                    *
                                    * Output MPEG4 bitstream length (bytes) */
    unsigned char *quant_intra_matrix; /*< [in]
                                    *
                                    * Custom intra quantization matrix when MPEG4 quant is enabled*/
    unsigned char *quant_inter_matrix; /*< [in]
                                    *
                                    * Custom inter quantization matrix when MPEG4 quant is enabled */
    int quant;                         /*< [in]
                                    *
                                    * Frame quantizer :
                                    * <ul>
                                    * <li> 0 (zero) : Then the  rate controler chooses the right quantizer
                                    *                 for you.  Typically used in ABR encoding, or first pass of a VBR
                                    *                 encoding session.
                                    * <li> !=  0  :  Then you  force  the  encoder  to use  this  specific
                                    *                  quantizer   value.     It   is   clamped    in   the   interval
                                    *                  [1..31]. Tipically used  during the 2nd pass of  a VBR encoding
                                    *                  session. 
                                    * </ul> */
    int intra;                         /*< [in/out]
                                    *
                                    * <ul>
                                    * <li> [in] : tells Faraday if the frame must be encoded as an intra frame
                                    *     <ul>
                                    *     <li> 1: forces the encoder  to create a keyframe. Mainly used during
                                    *              a VBR 2nd pass.
                                    *     <li> 0:  forces the  encoder not to  create a keyframe.  Minaly used
                                    *               during a VBR second pass
                                    *     <li> -1: let   the  encoder   decide  (based   on   contents  and
                                    *              max_key_interval). Mainly  used in ABR  mode and during  a 1st
                                    *              VBR pass. 
                                    *     </ul>
                                    * <li> [out] : When first set to -1, the encoder returns the effective keyframe state
                                    *              of the frame. 
                                    * </ul>
                                    */
    
    /// The base address for input Y frame buffer.
    unsigned char *pu8YFrameBaseAddr;  /**< To set input Y frame buffer's base address.\n
                              *   <B>N.B.</B> : the input frame buffer address must be <B>physical address</B> with <B>8-byte aligned</B>.
                              *   @see pu8UFrameBaseAddr
                              *   @see pu8VFrameBaseAddr
                              *
                              *   Also, this variable can be set by utilizing the function FMpeg4EncSetYUVAddr().
                              *   @see FMpeg4EncSetYUVAddr
                              */
    /// The base address for input U frame buffer.  
    unsigned char *pu8UFrameBaseAddr;  /**< To set input U frame buffer's base address.\n
                              *   <B>N.B.</B> : the input frame buffer address must be <B>physical address</B> with <B>8-byte aligned</B>.
                              *   @see pu8YFrameBaseAddr
                              *   @see pu8VFrameBaseAddr
                              *
                              *   Also, this variable can be set by utilizing the function FMpeg4EncSetYUVAddr().
                              *   @see FMpeg4EncSetYUVAddr
                              */
    /// The base address for input V frame buffer.
    unsigned char *pu8VFrameBaseAddr;  /**< To set input V frame buffer's base address.\n
                              *   <B>N.B.</B> : the input frame buffer address must be <B>physical address</B> with <B>8-byte aligned</B>.
                              *   @see pu8YFrameBaseAddr
                              *   @see pu8UFrameBaseAddr
                              *
                              *   Also, this variable can be set by utilizing the function FMpeg4EncSetYUVAddr().
                              *   @see FMpeg4EncSetYUVAddr
                              */
     int    active0;        //the result of motion dection 
     int    active1;        //the result of motion dection 
     int    active2;        //the result of motion dection 
     int    frameindex;     //the frame index for motion dection
} Faraday_ENC_FRAME;

typedef struct
{
    uint8_t *data[4];
}TLAVFrame;



#define MOTION_ALARM_VALUE_SIZE   50
#define TL_OSD_DATE_FONT_4CIF_POS   0
#define TL_OSD_DATE_FONT_2CIF_POS   8352
#define TL_OSD_DATE_FONT_CIF_POS    (8352+4176)
#define TL_OSD_DATE_FONT_QCIF_POS   (8352+4176+2112)
#define TL_FIC8120_MAX_CHANNEL  1
#define TL_MAXSENSORNUM     8
///software watchdog fifo
#define         FIFO_SERVER "/tmp/fifoserver"


// define message struct to client which include server name server ipaddr etc

typedef struct _TL_FIC8120_Audio_nFrame
{
    unsigned char *Out_buf;
    unsigned int   nframeLen;
}TL_FIC8120_Audio_nFrame;


typedef struct _TL_FIC8120_Audio_Temp_Save
{
    unsigned char   *save_buf;
    unsigned char   *temp_buf;
    unsigned int   write_pos;
}TL_FIC8120_Audio_Temp_Save;

//FILE *g_tl_common_fonts_fs;
//FILE *g_tl_common_fonts_16_8_fs;
//FILE *g_tl_common_fonts_8_8_fs;


//TL_FIC8120_Audio_nFrame     g_tl_audio_frame;
//TL_FIC8120_Audio_Temp_Save  g_tl_audio_tmep_save;
//pthread_mutex_t         g_tl_avsync_mutex;
pthread_mutex_t         g_tl_codec_setting_mutex;

///TL Time Mutex
//pthread_mutex_t         g_tl_time_mutex;


///Video Osd Style Set
typedef struct _TL_OSD_SET_PARAMETER
{
    TL_CHANNEL_OSDINFO      tl_Osd_Style_Set;   
    TL_SERVER_OSD_DATA      tl_Osd_Fonts;
}TL_OSD_SET_PARAMETER;



///Connect Status Right
typedef struct _TL_TIME_TICK_INFO
{
    pthread_t       tltimeid; ///
    unsigned long       nowtime; ///
}TL_TIME_TICK_INFO , *PTL_TIME_TICK_INFO;
TL_TIME_TICK_INFO g_TL_system_timer;


///FTP Photo Send Buffer
typedef struct _TL_FTP_SEND_PIC_INFO
{
    BOOL            bSend;  ///whather need to send
    char            pic_name[256]; ///pic name
}TL_FTP_SEND_PIC_INFO , *PTL_FTP_SEND_PIC_INFO;

typedef struct _TL_PICTURE_SNAPSHOT_INFO
{
    sem_t       manual_snapshot_sem;
    sem_t       auto_snapshot_sem;
    int     manual_snapshot_num;
    int     auto_snapshot_num;
    MSG_HEAD    manual_msg[16];
    MSG_HEAD    auto_msg[16];
    
}TL_PICTURE_SNAPSHOT_INFO , *PTL_PICTURE_SNAPSHOT_INFO;

//TL_PICTURE_SNAPSHOT_INFO    g_picture_snapshot_info;

typedef struct _TL_SYSTEM_RTC_INFO
{
    TL_SERVER_OSD_DATA  Osd_Data[QMAPI_MAX_CHANNUM];
    pthread_mutex_t     rtc_mutex;
}TL_SYSTEM_RTC_INFO , *PTL_SYSTEM_RTC_INFO;

//TL_SYSTEM_RTC_INFO  g_system_rtc_info;

//static char g_Fonts_code_SongFonts[2];
//char g_Fonts_Date_Lib[16*1024]; //10464

///Audio Out File 
//int g_TL_Audio_Out_fs;
///Color Change Signel
//BOOL g_TL_Color_SetChange_Signel;
///Hardware Resume Record Times 
//unsigned int g_TL_Hardware_Reset_Record;
///Hardware GPIO5 system lamp dispaly type 
///if dispaly_type: 0 generl running status , 1 system start-up status 
///2: system update status
//unsigned int g_TL_system_lamp_dispaly_type;


typedef void (*TL_Audiocallback)(void *userdata, unsigned char *Audio_stream , int len);

typedef void (*fStopExtModuleCallBack)();

///TL Video New Protocol Jpge Shot
void TL_Video_jpegThread();

///TL Video New Protocol Auto Jpge Shot
void TL_Video_Auto_jpegThread();

///TL Video New Protocol Picture Snapshot Initial
void TL_Video_jpegThread_Initial();



///TL Video New Protocol Get Alarm Out State
///user struct TLNV_SENSOR_STATE
//int TL_Video_AlarmOut_Get_state(char *Buffer);

void mSleep(unsigned int  MilliSecond);

void StopSystemModule();


typedef struct tl_cap_osd_covert
{
    TL_SERVER_OSD_DATA  osd_fonts;
    TL_SERVER_OSD_DATA  osd_date;
    TL_CHANNEL_SHELTER  osd_sheler;
    unsigned int    Osd_open_off;
    unsigned int    Osd_date_open_off;
    unsigned int    Osd_x;
    unsigned int    Osd_y;
    unsigned int    Date_x;
    unsigned int    Date_y;
}tl_cap_osd_covert_t;

typedef struct tagTL_VIDEO_PROFILE
{
    unsigned int    nChannel;
    unsigned int    bit_rate;
//  unsigned int    width;   ///length per dma buffer
//  unsigned int    height;
    unsigned int    framerate;
    unsigned int    frame_rate_base;
    unsigned int    gop_size;
    unsigned int    standard;///0:PAL 1:NTSC
    unsigned int    ImageQuality;
    unsigned int    RateType;
    unsigned int    StreamFormat;
    unsigned int    motoin_dection_on;
    unsigned int    itev_dection_on;
    unsigned int    change_encode_state;
    unsigned int    profile;   //h264 profile
      
    //audio param
    unsigned int    nEncode; 
    QMAPI_PAYLOAD_TYPE_E enCompressionType;  
    int         nSnapLevel;

    QMAPI_SYSTEMTIME  stVideoAlarmStartTime;
    
} TL_VIDEO_PROFILE;


typedef struct
{
    int     bencode_video[16];
    int     bencode_audio[16];
    unsigned int    motion_handle_type[16];
    unsigned int    lost_handle_type[16];
    unsigned int    sensor_handle_type[16];
    int     resverd[64];
}TL_NXSIGHT_SAVE_PARAM;



///TL Video Global Parameter Struct Save to hardware flash

typedef struct _TL_FIC8120_FLASH_SAVE_GLOBEL
{
    ///01
    QMAPI_NET_DEVICE_INFO          stDevicInfo;
    QMAPI_NET_NETWORK_CFG          stNetworkCfg;
    ///02
    TL_SERVER_USER               TL_Server_User[10];
    ///03
    TLNV_SERVER_INFO             TL_Server_Info;
    ///04
    TLNV_CHANNEL_INFO            TL_Stream_Info[QMAPI_MAX_CHANNUM];

    ///06
    QMAPI_NET_CHANNEL_COLOR        TL_Channel_Color[QMAPI_MAX_CHANNUM];

    QMAPI_NET_CHANNEL_ENHANCED_COLOR  TL_Channel_EnhancedColor[QMAPI_MAX_CHANNUM];
    ///08
    TL_CHANNEL_LOGO              TL_Channel_Logo[QMAPI_MAX_CHANNUM];
    ///09
    TL_CHANNEL_SHELTER           TL_Channel_Shelter[QMAPI_MAX_CHANNUM];
    ///10
    TL_CHANNEL_MOTION_DETECT     TL_Channel_Motion_Detect[QMAPI_MAX_CHANNUM];
    ///11
    TL_SENSOR_ALARM              TL_Channel_Probe_Alarm[TL_MAXSENSORNUM];
    ///12
    ///13
    TL_CHANNEL_OSDINFO           TL_Channel_Osd_Info[QMAPI_MAX_CHANNUM];
    ///14
    TL_SERVER_OSD_DATA           TL_Server_Osd_Data[QMAPI_MAX_CHANNUM];
    ///15
    TL_SERVER_COMINFO_V2         TL_Server_Cominfo[QMAPI_MAX_CHANNUM];
    ///16
    TL_SERVER_COM2INFO           TL_Server_Com2info[QMAPI_MAX_CHANNUM];
    ///17
    TL_UPLOAD_PTZ_PROTOCOL       TL_Upload_PTZ_Protocol[QMAPI_MAX_CHANNUM];
    ///18
    TL_FTPUPDATA_PARAM           TL_FtpUpdata_Param;
    ///19
    TL_NOTIFY_SERVER             TL_Notify_Servers;
    ///22
    QMAPI_NET_PLATFORM_INFO_V2   stNetPlatCfg; //
    ///23
    ///24
   // TL_SERVER_RECORD_SET         tlServerRecordSet;
    QMAPI_NET_EMAIL_PARAM          TL_Email_Param;
    ///25
    
    QMAPI_NET_DDNSCFG              stDDNSCfg;
    // 26
    TL_WIFI_CONFIG               tl_Wifi_config;
    //27
    TL_TDSCDMA_CONFIG            tl_Tdcdma_config;
    QMAPI_NET_CHANNEL_PIC_INFO     stChannelPicParam[QMAPI_MAX_CHANNUM];
    TL_UPNP_CONFIG               tl_upnp_config;    
    QMAPI_NET_ZONEANDDST           stTimezoneConfig;
    QMAPI_NET_RTSP_CFG             stRtspCfg;
    BOOL                         bRtspPortChanged;

    QMAPI_NET_CHANNEL_RECORD       stChannelRecord[QMAPI_MAX_CHANNUM];
    QMAPI_NET_SNAP_TIMER           stSnapTimer;
    QMAPI_NET_NTP_CFG           stNtpConfig;

    TL_TIME_OSD_POS              stTimeOsdPos[QMAPI_MAX_CHANNUM];
    QMAPI_NET_CHANNEL_OSDINFO      stChnOsdInfo[QMAPI_MAX_CHANNUM];

    QMAPI_NET_DAY_NIGHT_DETECTION_EX    stCBDetevtion[QMAPI_MAX_CHANNUM];

    QMAPI_NET_DEVICEMAINTAIN      stMaintainParam;
    QMAPI_NET_CONSUMER_INFO       stConsumerInfo;

    QMAPI_NET_NETWORK_STATUS      stNetworkStatus;
    QMAPI_NET_WIFI_CONFIG      stWifiConfig;

    QMAPI_SYSCFG_ONVIFTESTINFO  stONVIFInfo;
} TL_FIC8120_FLASH_SAVE_GLOBEL;


TL_FIC8120_FLASH_SAVE_GLOBEL    g_tl_globel_param;
///Save Encrypt Info
//TLNV_SYSTEM_INFO        g_authen_ic_raw_information;


typedef enum
{
    NVSclose,
    NVSopen,
}NVSonoff_t;

typedef enum
{
    Sunday,
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Everyday,
}dateTime_t;

typedef struct
{
    nvsEventType_t          setSysTime;
    int             year;
    int             month;
    dateTime_t          week;
    int             day;
    int             hour;
    int             minute;
    int             second;

    int             result;
}sysTime_t;

typedef struct
{
    char                        prompt;
    int                     baudrate;
    char                        databit;
    char                        debug;
    char                        echo;
    char                        fctl;
    char                        tty;
    char                        parity;
    char                        stopbit;
    const int                   reserved;
}portinfo_t, *pportinfo_t;

typedef enum
{
    b1200,
    b2400,
    b4800,
    b9600,
    b19200,
    b38400,
    b43000,
    b56000,
    b57600,
    b115200,
}boundrate_t;


typedef struct 
{
    UInt16      addr;   /* device address */
    UInt16      waddr;  /* word address */
    short       len;    /* msg length */
    char        *buf;   /* pointer to msg data */
    int         clockdiv;   /* for clock div */
    int         multiread_once;
}tl_i2c_faraday_msg;

//pthread_t                   g_detecIOAlarmThread;


//Bool                                g_audioTalkFlag;
//Bool                                g_audioDecOpen;
//pthread_mutex_t                 g_audioTalkMutex;


//int                             g_SystemInitFinished;
//BOOL                         g_Reboot_bQuit;
//Bool                           g_updateSystem;
int                             g_serverPort;           // the media port which user can access to
//Int                             g_serverMediaPort;

int                             g_tl_wdt_fd;

//pthread_mutex_t                 g_tl_gpio_mutex;
//pthread_mutex_t                 g_tl_RS485_mutex;
int                             g_tl_authen_fd;
pthread_mutex_t                 g_tl_authen_mutex;

/* rs485 */
//int                             g_tl_rs485_fd;
//int                             g_tl_rs232_fd;
//int								g_tl_rsDebug_fd;
/* input osd */

//fStopExtModuleCallBack       g_pfunStopExtModule;
//int  								gs32ITEVAlarm;
//int  								gs32ITEVTripWireAlarm;
//int  								gs32ITEVVideoFoolAlarm;

/* system time */

/* system configure */
void AddSysFuncConfig(DWORD dwFlag);
void DelSysFuncConfig(DWORD dwFlag);
void InitSysFuncConfig();
int  CheckSysFuncConfig(DWORD dwFlag);

int initSysdate( void );



int ReceiveFun(MSG_HEAD *pMsgHead,char *pRecvBuf);



int TL_Video_Net_Send(MSG_HEAD  *pMsgHead, char *pSendBuf);

BOOL TL_Video_Net_SendAllMsg(char *pMsgBuf, int nLen);
int getUserId(unsigned long ipAddr, unsigned long port);

unsigned long GetDataCRC(Byte *lpData, int dwSize);

/*nvs local record */
int GetFormatProcess( void );
Int vadcDrv_Setup_Debug( void );


int tl_flash_erase_block(char *handle);
int tl_flash_read(char *handle, unsigned char *data, int length);
int tl_flash_write(char *handle, unsigned char *data, int length);
int tl_read_sys_config( void );
int tl_write_sys_config( void );


int tl_wdt_startup(void);
int tl_wdt_keep_alive( void );
int tl_get_wdt_status(void);

int tl_set_default_PICinfo();

int tl_ipaddr_format( char * addr);

void mSleep(unsigned int  MilliSecond);



int TL_SetDataPort_Script( void );

void tl_get_io_alarm_num(int *max_in_num, int *max_out_num);

int tl_input_osd_open( void );
int tl_input_osd_set( int cmd, tl_osdreq *data);

void TL_AVCaptureThread(void *param); 
void TL_AEncFrameThread(void *param); 
//mobile video encoding 
void TL_MobileAVCaptureThread(void *param); 
void DumpAVCaptureStatus();
//Bool          g_bVideoEncoderEnable[MAXCHANNELNUM][QMAPI_MAX_STREAMNUM];
//int           g_audio_enc_channelId[SER_MAX_CHANNEL];
//Bool          g_bAencEncoderEnable[SER_MAX_CHANNEL];
//int           g_phoneAudio_enc_channelId[SER_MAX_CHANNEL];
//Bool          g_bPhoneAencEncoderEnable[SER_MAX_CHANNEL];
//Bool            g_bVideoEncoderEnable[QMAPI_MAX_CHANNUM][QMAPI_MAX_STREAMNUM];
//int         g_audio_enc_channelId[QMAPI_MAX_CHANNUM];
//Bool            g_bAencEncoderEnable[QMAPI_MAX_CHANNUM];
//int         g_phoneAudio_enc_channelId[QMAPI_MAX_CHANNUM];
//Bool            g_bPhoneAencEncoderEnable[QMAPI_MAX_CHANNUM];

//int                  g_nVEncCaptureStep[QMAPI_MAX_CHANNUM][QMAPI_MAX_STREAMNUM];
//int                  g_nOSDTimeUpdateStep;
typedef struct
{
    int nChannel;
    int nStreamChn;
}av_thread_param_t;

/* video audio enc */
//pthread_mutex_t                     g_tl_video_audio_mutex[QMAPI_MAX_CHANNUM];
typedef struct
{
    pthread_mutex_t             aenc_mutex;
    int                         frame_num;
    unsigned long                   stream_len;
    unsigned long                   write_index;
    unsigned char                   *stream_buf;
}tl_aenc_stream_t;

typedef struct
{
    unsigned long s32AiDev;
    unsigned long s32AiChn;
    unsigned long s32AencChn; 
} DMS_AENC_CTRL_S;
//tl_aenc_stream_t                g_aenc_stream[QMAPI_MAX_CHANNUM];

//tl_aenc_stream_t                g_aenc_small_stream[QMAPI_MAX_CHANNUM];

//tl_aenc_stream_t                g_aenc_mobile_stream[QMAPI_MAX_CHANNUM];

//int                         g_md_sensitive[QMAPI_MAX_CHANNUM];
//int                         g_h264_scale;

void TL_MailSend_Thread();


#define MAX_CODE_LENGTH                         256
#define IPADDR_LENGTH                           16
typedef struct
{
    UInt32      nNodeID;
    UInt32      nReserved;                  //
    UInt32      nSubCmd;                    //
    UInt32      nCMSServerPort;             //
    char        sCMSServerIP[IPADDR_LENGTH];
    char        sDVSIP[IPADDR_LENGTH];
    char        sCodingID[MAX_CODE_LENGTH];
}CMSINFO_RESPONSE_BODY;

typedef struct
{
    int         fad_addr;
    int         fad_alive;
    int         temp_sign;
    int         temp;
    int         tempext;
    int         hum;
    int         humext;
}tl_nsid_t;

typedef struct
{
    int         sign;
    int         temp;
    int         tempext;
    int         hum;
    int         humext;
}tl_nsid_info_t;

typedef struct
{
    int             dv_chId;
    unsigned char           dv_name[32];
    int             status; 
}nsid_dv_channel_t;


typedef struct
{
    unsigned char       dvsid[32];
    unsigned char       CorpID[32];
    unsigned char       dvsName[32];
    unsigned int        dvsIp;
    unsigned int        webPort;
    unsigned int        crtlPort;
    unsigned int        protocol;
    unsigned char       userId[32];
    unsigned char       userPasw[32];
    unsigned char       Model[32];
    unsigned int        postFreq;
    unsigned char       version[32];
    unsigned int        status; 
    unsigned int        serverIp;
    unsigned int        serverPort; 
    unsigned int            transfervtdu;//
}SENSORCHN_DATA_EXT;

typedef struct
{
    unsigned int    id;
    char      type[32];
    char      name[32];     
    float     minvalue;
    float     maxvalue;
    float     value;
    unsigned int    status; //0:normal 1:offline
}SENSORCHN_DATA;


typedef struct
{
    unsigned int                size;
    unsigned int                DVcount;
    unsigned int                SensorCount;
    SENSORCHN_DATA_EXT          SensorChnExt;
    nsid_dv_channel_t       DVChn[4];
    SENSORCHN_DATA              SensorChn[4];
}SENSOR_DATA;

typedef struct
{
    unsigned int    id;
    char      type[32]; 
    char      name[32];
    float    minvalue;
    float    maxvalue;
    float     value;
    unsigned int    status; //0:normal 1:offline
}SENSORCHN_CONFIG;


typedef struct
{
    unsigned long   dwSize;
    unsigned int    valid;   //0:invalid, 1:valid
    char        version[32];
    char            dvswanip[32];//internet ip
    char            dvsid[32];
    char        corpid[32];
    char            model[32];
    unsigned int    transfer; //0:don't use nxsight 1:use nxsight default:1
    char        serverIp[32];//nxsight ip
    unsigned int    serverPort; //

    char            postip[32];//default(dvs.nsid.info)
    unsigned long           postport;//default(80)
    char            posturl[32];///post/post.aspx
    unsigned int    postfreq;//default(180)

    char            secretpostip[32];//default(dvs.860000.info)
    unsigned long           secretpostport;//default(80)
    char            secretposturl[32];//default(/post/post.aspx)
    unsigned int    secretpostfreq;// default(180)
    
    unsigned int    type;    //the device type, 1:FAD, 2:.......
    unsigned int    address; //the device address
    unsigned int    videochn;//relational video channel,add the osd
    unsigned int    count;
    SENSORCHN_CONFIG SensorChn[4];
}SENSOR_CONFIG;

//tl_nsid_t           g_nsid_context[4];
//SENSOR_CONFIG           g_sensor_cfg;

//#endif


/* dhcp */
//int                         g_dhcp_start;
//int                         g_bkill_dhcp;


void tl_ddns_thread(void);

int  g_bNeedRestart;
int Dev_Get_System_TimezoneFromPara();
//void TL_rtc_to_system_time();
//int  TL_VideoSnapShot(int nChannel, int nLevel, char **buf, int *maxlen);
//int  TL_VideoSnapShot_V2(int nChannel,int nLevel,int iWidth,int iHeight,char **buf,int *maxlen);
//int TL_Video_Update_OSD(char *pRecvBuf);

int tl_write_Server_Network();

typedef enum __CHIP_TYPE_E
{
    CHIP_Hi3510 = 1,
	CHIP_Hi3511,
	CHIP_Hi3512,
	CHIP_Hi3515,
	CHIP_Hi3515A = 5,
	CHIP_Hi3520,
	CHIP_Hi3520A,
	CHIP_Hi3520D,
	CHIP_Hi3521,
	CHIP_Hi3531 = 10,
	CHIP_Hi3518C,
	CHIP_Hi3518A,
	CHIP_Hi3516C,
	CHIP_Ti365,
	CHIP_Ti368 = 17,
	CHIP_Hi3516,
	CHIP_BUTT
}AN_CHIP_TYPE_E;
int TL_GetChipType();
int CheckIPConflict(unsigned int test_ip, unsigned char *from_mac, char *ifname, unsigned char IsWifi);
int TL_GetAddrNum(const char *pIp, unsigned int *num1,unsigned int *num2,unsigned int *num3,unsigned int *num4);


char *TL_GetChannelName(int s32Chn);
void SetSysNetworkFun(void *pPar);

int startServerInfoBroadcast();

#endif

