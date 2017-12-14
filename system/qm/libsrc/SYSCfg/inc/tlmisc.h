
#ifndef __TLMISC_H__
#define __TLMISC_H__

#include "QMAPINetSdk.h"
typedef struct
{
    int     fatal_param_len;
    int     default_flag;  /* 180: fFlagDefault, 181: fFlagChange, 182: fFlagSoftReset */
    int     sys_err_restart_tick;
}tl_fatal_param_t;
tl_fatal_param_t                    g_fatal_param;


#define TL_FLASH_CONF_DEFAULT_PARAM      TL_CONFIG_PATH"/tl_default.config"
#define TL_FLASH_FATAL_CONF_DEV     TL_CONFIG_PATH"/tl_dvs.fatal.config"
#define TL_FLASH_CONF_SERVER_USER   TL_CONFIG_PATH"/tl_server_user.config"
#define TL_FLASH_CONF_SERVER_INFO   TL_CONFIG_PATH"/tl_server_info.config"
#define TL_FLASH_CONF_STREAM_INFO   TL_CONFIG_PATH"/tl_stream_info.config"
#define TL_FLASH_CONF_CHANNEL_COLOR TL_CONFIG_PATH"/tl_channel_color.config"
#define TL_FLASH_CONF_CHANNEL_ENHANCED_COLOR TL_CONFIG_PATH"/tl_channel_enchanced_color.config"
#define TL_FLASH_CONF_CHANNEL_LOGO  TL_CONFIG_PATH"/tl_channel_logo.config"
#define TL_FLASH_CONF_CHANNEL_SHELTER   TL_CONFIG_PATH"/tl_channel_shelter.config"
#define TL_FLASH_CONF_CHANNEL_MOTION_DETECT TL_CONFIG_PATH"/tl_channel_motion_detech.config"
#define TL_FLASH_CONF_CHANNEL_PROBE_ALARM   TL_CONFIG_PATH"/tl_channel_probe_alarm.config"
#define TL_FLASH_CONF_CHANNEL_VIDEO_LOST     TL_CONFIG_PATH"/tl_channel_video_lost.config"
#define TL_FLASH_CONF_CHANNEL_OSDINFO   TL_CONFIG_PATH"/tl_channel_osdinfo.config"
#define TL_FLASH_CONF_CHANNEL_OSD_DATA  TL_CONFIG_PATH"/tl_channel_osddata.config"
#define TL_FLASH_CONF_SERVER_COMINFO    TL_CONFIG_PATH"/tl_server_cominfo.config"
#define TL_FLASH_CONF_SERVER_COM2INFO   TL_CONFIG_PATH"/tl_server_com2info.config"
#define TL_FLASH_CONF_PLOAD_PTZ_PROTOCOL    TL_CONFIG_PATH"/tl_ptz_protocol.config"
#define TL_FLASH_CONF_FTPUPDATA_PARAM   TL_CONFIG_PATH"/tl_ftpupdata_param.config"
#define TL_FLASH_CONF_NOTIFY_SERVER TL_CONFIG_PATH"/tl_notify_server.config"
#define TL_FLASH_CONF_PRODUCER_PARAMETER_SETTING    TL_CONFIG_PATH"/tl_producer_parameter_setting.config"
#define TL_FLASH_CONF_DEFAULTE_PTZ_PROTOCOL TL_CONFIG_PATH"/tl_pelco_d.config"

#define TL_FLASH_CONF_CHANNEL_RECORD        TL_CONFIG_PATH"/tl_channel_record.config"
#define TL_FLASH_CONF_DDNS_INFO TL_CONFIG_PATH"/tl_ddns_info.config"
#define TL_FLASH_CONF_EMAIL_PARAM       TL_CONFIG_PATH"/tl_email_param.config"
#define TL_FLASH_CONF_WIRELESS_TDSCDMA_PARAM TL_CONFIG_PATH"/tl_wireless_tdscdma_param.config"
#define TL_FLASH_CONF_WIRELESS_WIFI_PARAM TL_CONFIG_PATH"/tl_wireless_wifi_param.config"
            //OTTO ADD 10-08-30 
#define TL_FLASH_CONF_CHANNEL_PIC   TL_CONFIG_PATH"/tl_channel_streampic.config"
#define TL_FLASH_CONF_TIMEZONE_PARAM    TL_CONFIG_PATH"/tl_timezone_param.config"
#define TL_FLASH_CONF_UPNP_PARAM        TL_CONFIG_PATH"/tl_upnp_param.config"
#define TL_FLASH_CONF_RTSP_PARAM        TL_CONFIG_PATH"/tl_rtsp_param.config"
#define TL_FLASH_CONF_SNAP_TIMER        TL_CONFIG_PATH"/tl_snap_timer.config"
#define TL_FLASH_CONF_NTP_PARAM         TL_CONFIG_PATH"/tl_ntp_param.config"
#define TL_FLASH_CONF_TIME_OSDPOS       TL_CONFIG_PATH"/tl_time_osdpos.config"
#define TL_FLASH_CONF_DMS_CHN_OSDINFO   TL_CONFIG_PATH"/tl_dms_channel_osdinfo.config"
#define DMS_SERVER_NETWORK_CONFIG       TL_CONFIG_PATH"/tl_server_network.config"
#define DMS_SERVER_DEVICE_CONFIG        TL_CONFIG_PATH"/tl_server_device.config"
#define DMS_SERVER_CB_DETECTION         TL_CONFIG_PATH"/tl_server_cb_detection.config"
#define TL_FLASH_CONF_CONSUMER_PARAM    TL_CONFIG_PATH"/tl_consumer_param.config"
#define TL_FLASH_CONF_WIFI_PARAM        TL_CONFIG_PATH"/tl_wifi_param.config"
#define TL_FLASH_CONF_MAINTAIN_PARAM    TL_CONFIG_PATH"/tl_maintain_param.config"
#define TL_FLASH_CONF_ONVIFINFO_PARAM   TL_CONFIG_PATH"/tl_onvifinfo_param.config"

//#endif

void set_default_record_config(void);
int tl_flash_erase_block(char *handle);
int tl_flash_read(char *handle, unsigned char *data, int length);
int tl_flash_write(char *handle, unsigned char *data, int length);
int tl_read_fatal_param( void );
int tl_read_Server_Network( void );
int tl_read_Server_User( void );
int tl_read_Server_Info( void );
int tl_read_Stream_Info( void );
int tl_read_Server_State( void );
int tl_read_Channel_Enhanced_Color( void );
int tl_read_Channel_Color( void );
int tl_read_Channel_Logo( void );
int tl_read_Channel_Shelter( void );
int tl_read_Channel_Motion_Detect( void );
int tl_read_Channel_Probe_Alarm( void );
int tl_read_Channel_Osd_Info( void );
int tl_read_Server_Osd_Data( void );
int tl_read_Server_Cominfo( void );
int tl_read_Server_Com2info( void );
int tl_read_Upload_PTZ_Protocol( void );
int tl_read_FtpUpdata_Param( void );
int tl_read_Notify_Servers( void );
//int tl_read_pppoe_config( void );
int tl_read_ddns_config( void );
int tl_read_producer_setting_config(void);
int tl_read_record_setting(void);
int tl_read_default_PTZ_Protocol( void );
int tl_read_wireless_wifi_param(void);
int tl_read_wireless_tdscdma_param(void);
int tl_read_ChannelPic_param(void);
int tl_read_timezone_config(void);
int tl_read_upnp_param(void);
int tl_read_p2p_param(void);
int tl_read_record_mode_param(void);
int tl_read_rtsp_param(void);
int tl_read_snap_timer(void);
int tl_read_record_param(void);
int tl_read_channel_reaord_param(void);
int tl_read_ntp_param(void);
int tl_read_sensor_type(void);
int tl_read_dms_channel_osd_config(void);
int tl_read_dms_channel_cb_detection_config(void);
int tl_read_consumer_config(void);
int tl_read_maintain_config(void);
int tl_read_wifi_config(void);
int tl_read_onviftestinfo_config();
void tl_read_all_parameter(void);

int tl_write_Server_Network( void );
int tl_write_Server_User( void );
int tl_write_Server_Info( void );
int tl_write_Stream_Info( void );
int tl_write_Server_State( void );
int tl_write_Channel_Color( void );
int tl_write_Channel_Enhanced_Color( void );
int tl_write_Channel_Logo( void );
int tl_write_Channel_Shelter( void );
int tl_write_Channel_Motion_Detect( void );
int tl_write_Channel_Probe_Alarm( void );
int tl_write_Channel_Osd_Info( void );
int tl_write_Server_Osd_Data( void );
int tl_write_Server_Cominfo( void );
int tl_write_Server_Com2info( void );
int tl_write_Upload_PTZ_Protocol( void );
int tl_write_FtpUpdata_Param( void );
int tl_write_Notify_Servers( void );
//int tl_write_pppoe_config( void );
int tl_write_ddns_config( void );
int tl_write_producer_setting_config(void);
int tl_write_record_setting(void);
int tl_write_channel_reaord_param(void);
int tl_write_producer_saved_param(void);
int tl_write_default_ptz_protocol(void);
int tl_write_wireless_wifi_param(void);
int tl_write_wireless_tdscdma_param(void);
int tl_write_ChannelPic_param( void );
int tl_write_upnp_param(void);
int tl_write_timezone_param(void);
int tl_write_record_param(void);
int tl_write_p2p_param(void);
int tl_write_record_mode_param(void);
int tl_write_rtsp_param(void);
int tl_write_snap_timer(void);
int tl_write_ntp_timer(void);
int tl_write_dms_channel_osd_config(void);
int tl_write_dms_channel_cb_detection_config(void);
int tl_write_sensor_type(void);
int tl_write_consumer_config(void);
int tl_write_maintain_config(void);
int tl_write_wifi_config(void);
int tl_write_onviftestinfo_config();
void Write_Reset_Log(int flagCode);
void tl_write_all_parameter(void);
void* tl_write_parameter_proc(void * para);
void tl_write_all_parameter_asyn(void );
int tl_write_fatal_param(tl_fatal_param_t fatal_param);
void tl_init_Channel_Color(int nChannel);

int tl_set_default_param(int flag);

void saveRecordParam();
void saveChannelRecordParam();

int tl_get_wdt_status(void);
int tl_get_wdt_timeout( void );
int tl_set_wdt_timeout(int cur_margin);

int GetNetSupportStreamFmt(int nChannel, QMAPI_NET_SUPPORT_STREAM_FMT   *lpstSupportFmt);
int tl_ipaddr_format( char * addr);
#endif
