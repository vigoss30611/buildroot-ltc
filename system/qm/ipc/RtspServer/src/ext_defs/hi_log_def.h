
/***********************************************************************************
*              Copyright 2006 - 2006, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_log_level.h
* Description: HI3510 Demo's log level .
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.1       2008-11-12   t41030     NULL         Create this file.
***********************************************************************************/


#ifndef __HI_LOG_DEF_H__
#define __HI_LOG_DEF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */



typedef enum hiAPPMOD_ID_E
{
    /*板端*/
    HI_APPID_SERVER     = 0x00, /*server 公共错误码*/
    HI_APPID_INIT       = 0x01, /*INIT 模块*/
    HI_APPID_ARG        = 0x02, /*参数解析模块*/
    HI_APPID_INI        = 0x03, /*INI 模块*/
    HI_APPID_DEBUG      = 0x04, /*调试模块*/
    HI_APPID_LOG        = 0x05, /*日志模块*/
    HI_APPID_MEMMAP     = 0x06, /*MEMMAP 模块*/
    HI_APPID_OUTPUT     = 0x07, /*输出模块*/
    HI_APPID_RTP        = 0x08, /*RTP模块*/
    HI_APPID_SYSADAPTER = 0x09, /*操作系统适配模块*/
    HI_APPID_THREAD     = 0x0A, /*线程操作模块*/
    HI_APPID_GRAPH      = 0x0D, /*图形处理模块*/
    HI_APPID_CHN        = 0x0E, /*编码通道模块*/
    HI_APPID_CHNOSD     = 0x0F, /*OSD模块*/
    HI_APPID_CHNMD      = 0x10, /*MD模块*/
    HI_APPID_TUI        = 0x11, /*Telnet命令接口UI*/
    HI_APPID_STELNET    = 0x12, /*Telnet 服务器端*/
    HI_APPID_MODULE_LOAD= 0x14, /*模式加载模块*/
    HI_APPID_PTZ        = 0x15, /*云台控制模块*/
    HI_APPID_AUTHMNG    = 0x16, /*用户管理模块*/
    HI_APPID_UPGRADE    = 0x17, /*升级模块*/
    HI_APPID_REC        = 0x18, /*录制模块*/
    HI_APPID_AVI        = 0x19, /*音视频AVI复合模块*/
    HI_APPID_TRIGGER    = 0x1A, /*告警触发模块*/
    HI_APPID_THTTPD     = 0x1B, /*thttpd模块*/
    HI_APPID_CGI        = 0x1C, /*CGI模块*/
    HI_APPID_NSCONFIG   = 0x1D, /*网络系统配置*/
    HI_APPID_RESET      = 0x1E, /*恢复出场设置*/
    HI_APPID_RECAPP     = 0x1F, /*录像管理*/
    HI_APPID_CHNSNAP    = 0x20, /*通道编码抓拍*/
    HI_APPID_AUDIODEC   = 0x21, /*解码*/
    HI_APPID_CONFACCESS = 0x22, /*配置存取*/
    HI_APPID_DEVM_VI    = 0x23, /*外围设备-视频输入芯片*/
    HI_APPID_DEVM_SIO   = 0x24, /*外围设备-音频输入输出芯片*/
    HI_APPID_DEVM_EXTDISK   = 0x25, /*设备管理-外接磁盘*/
    HI_APPID_DEVM_SYSDEV= 0x26, /*设备管理-系统设备(设备信息、时间信息)*/
    HI_APPID_DEVM_NETDEV= 0x27, /*设备管理-网络设备(网卡)*/
    HI_APPID_DEVM_USBDEV= 0x28,
    HI_APPID_TALK       = 0x29,
    HI_APPID_MP         = 0x2A,
    HI_APPID_MPMNG      = 0x2B,
    HI_APPID_EMNG       = 0x2C,
    HI_APPID_NOTIFY     = 0x2D,
    HI_APPID_SNAP       = 0x2E,
    HI_APPID_LIVE_CLIENT = 0x2F,
    HI_APPID_MTRANS     = 0x30,
    HI_APPID_LIVE_HTTPSVR = 0x31,
    HI_APPID_VOD = 0x32,
    HI_APPID_LISNSVR = 0x33,
    HI_APPID_LIVE_RTSPSVR = 0x34,
    HI_APPID_LIVE_LANGTAOSVR = 0x35,
    HI_APPID_LIVE_RTSP_O_HTTPSVR = 0x36,    

    /*--- 私有协议 ---*/
    HI_APPID_VSCP_MCTP  = 0x50, /*私有协议-媒体控制传输协议*/
    HI_APPID_VSCP_DEVS  = 0x51, /*私有协议-设备搜索*/
    HI_APPID_VSCP_COMM  = 0x52, /*私有协议-传输、管理、数据包处理*/
    HI_APPID_VSCPUI     = 0x55, /*私有协议UI*/
    HI_APPID_VSCP_ES    = 0x54, /*私有协议-事件订阅*/
    HI_APPID_VSCP_CMD   = 0x55, /*私有协议-命令处理*/

    
    HI_APPID_BUTTY      = 0xFF  /**/

} APP_MOD_ID_E;
#if 0
typedef enum hiMOD_ID_E
{
    HI_ID_API = 0,
    HI_ID_MPI = 1,
    HI_ID_CMPI = 2,
    HI_ID_VIO = 3,
    HI_ID_VIU = 4,
    HI_ID_DSU = 5,
    HI_ID_VOU = 6,
    HI_ID_ZSP = 7,
    HI_ID_VENC = 8,
    HI_ID_VDEC = 9,
    HI_ID_DRV3510 = 10,
    HI_ID_AENC  = 11,
    HI_ID_ADEC  = 12,
    HI_ID_MD    = 13,
    HI_ID_OSD   = 14,
    HI_ID_AVENC = 15,
    HI_ID_AVDEC = 16,
    HI_ID_AI    = 17,
    HI_ID_AO    = 18
} MOD_ID_E;
#endif

/* 1010 0000b
 * VTOP use APPID from 0~31
 * so, hisilicon use APPID based on 32
 * HI3510 APP Turnkey APPID based on 33
 */
#define HI_ERR_APPID  (0x80000000L + 0x20000000L)
#define HI_ERR_APP_APPID  (0x80000000L + 0x21000000L)

#if 0
/* 定义8个等级*/
typedef enum hiLOG_ERRLEVEL_E
{
    HI_LOG_LEVEL_DEBUG = 0,  /* debug-level                                  */
    HI_LOG_LEVEL_INFO,       /* informational                                */
    HI_LOG_LEVEL_NOTICE,     /* normal but significant condition             */
    HI_LOG_LEVEL_WARNING,    /* warning conditions                           */
    HI_LOG_LEVEL_ERROR,      /* error conditions                             */
    HI_LOG_LEVEL_CRIT,       /* critical conditions                          */
    HI_LOG_LEVEL_ALERT,      /* action must be taken immediately             */
    HI_LOG_LEVEL_FATAL,      /* just for compatibility with previous version */
    HI_LOG_LEVEL_BUTT
} LOG_ERRLEVEL_E;
#endif

/******************************************************************************
|----------------------------------------------------------------| 
| 1 |   APP_ID   |   MOD_ID    | ERR_LEVEL |   ERR_ID            | 
|----------------------------------------------------------------| 
|<--><--7bits----><----8bits---><--3bits---><------13bits------->|
******************************************************************************/

#define HI_DEF_ERR( module, level, errid) \
    ((HI_S32)( (HI_ERR_APPID) | ((module) << 16 ) | ((level)<<13) | (errid) ))

#define HI_APP_DEF_ERR( module, level, errid) \
    ((HI_S32)( (HI_ERR_APP_APPID) | ((module) << 16 ) | ((level)<<13) | (errid) ))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif // __HI_LOG_LEVEL_H__
