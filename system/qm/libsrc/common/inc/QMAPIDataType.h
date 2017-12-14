#ifndef _QMAPI_DATATYPE_H
#define _QMAPI_DATATYPE_H


/*       参数配置相关子命令    前两个F用来做基础值，后面6个F具体命令值  */
#define QMAPI_SYSCFG_GETBASE						0XF0000000 /* GET 命令基础值 */
#define QMAPI_SYSCFG_SETBASE						0XE0000000 /* SET 命令基础值 */
#define QMAPI_SYS_FUN_BASE							0xD0000000 /* 配置回调函数   */
#define QMAPI_REC_SEARCH_FILE_BASE					0xC0000000 /* 查询文件基础值 */
#define QMAPI_PTZ_STA_BASE							0xB0000000 /* 云台基础值     */
#define QMAPI_TL_SERVER_BASE						0x0A000000 /* */



/********************************** PTZ **************************************/

#define QMAPI_PTZ_STA_UP			(QMAPI_PTZ_STA_BASE+1)
#define QMAPI_PTZ_STA_DOWN          (QMAPI_PTZ_STA_BASE+2)
#define QMAPI_PTZ_STA_LEFT			(QMAPI_PTZ_STA_BASE+3)
#define QMAPI_PTZ_STA_RIGHT			(QMAPI_PTZ_STA_BASE+4)
#define QMAPI_PTZ_STA_UP_LEFT		(QMAPI_PTZ_STA_BASE+5)
#define QMAPI_PTZ_STA_UP_RIGHT		(QMAPI_PTZ_STA_BASE+6)
#define QMAPI_PTZ_STA_DOWN_LEFT		(QMAPI_PTZ_STA_BASE+7)
#define QMAPI_PTZ_STA_DOWN_RIGHT	(QMAPI_PTZ_STA_BASE+8)

#define QMAPI_PTZ_STA_FOCUS_SUB		(QMAPI_PTZ_STA_BASE+10)		//Focus Far
#define QMAPI_PTZ_STA_FOCUS_ADD		(QMAPI_PTZ_STA_BASE+11)		//Focus Near
#define QMAPI_PTZ_STA_ZOOM_SUB		(QMAPI_PTZ_STA_BASE+12)		//Zoom Wide
#define QMAPI_PTZ_STA_ZOOM_ADD		(QMAPI_PTZ_STA_BASE+13)		//Zoom Tele
#define QMAPI_PTZ_STA_IRIS_SUB		(QMAPI_PTZ_STA_BASE+14)		//Iris Close
#define QMAPI_PTZ_STA_IRIS_ADD		(QMAPI_PTZ_STA_BASE+15)		//Iris Open
#define QMAPI_PTZ_STA_STOP			(QMAPI_PTZ_STA_BASE+16)
#define QMAPI_PTZ_STA_PRESET		(QMAPI_PTZ_STA_BASE+17)		//预置
#define QMAPI_PTZ_STA_CALL			(QMAPI_PTZ_STA_BASE+18)		//调用

#define QMAPI_PTZ_STA_AUTO_STRAT	(QMAPI_PTZ_STA_BASE+20)
#define QMAPI_PTZ_STA_AUTO_STOP		(QMAPI_PTZ_STA_BASE+21)
#define QMAPI_PTZ_STA_LIGHT_OPEN	(QMAPI_PTZ_STA_BASE+22)      //灯光
#define QMAPI_PTZ_STA_LIGHT_CLOSE	(QMAPI_PTZ_STA_BASE+23)      
#define QMAPI_PTZ_STA_BRUSH_START	(QMAPI_PTZ_STA_BASE+24)      //雨刷
#define QMAPI_PTZ_STA_BRUSH_STOP	(QMAPI_PTZ_STA_BASE+25)  
#define QMAPI_PTZ_STA_TRACK_START	(QMAPI_PTZ_STA_BASE+26)      //轨迹
#define QMAPI_PTZ_STA_TRACK_STOP	(QMAPI_PTZ_STA_BASE+27)
#define QMAPI_PTZ_STA_TRACK_RUN		(QMAPI_PTZ_STA_BASE+28)
#define QMAPI_PTZ_STA_PRESET_CLS	(QMAPI_PTZ_STA_BASE+29)      //清除预置点

#define QMAPI_PTZ_STA_ADD_POS_CRU	(QMAPI_PTZ_STA_BASE+30)		/* 将预置点加入巡航序列 */
#define QMAPI_PTZ_STA_DEL_POS_CRU	(QMAPI_PTZ_STA_BASE+31)		/* 将巡航点从巡航序列中删除 */
#define QMAPI_PTZ_STA_DEL_PRE_CRU	(QMAPI_PTZ_STA_BASE+32)		/* 将预置点从巡航序列中删除 */
#define QMAPI_PTZ_STA_MOD_POINT_CRU	(QMAPI_PTZ_STA_BASE+33)		/* 修改巡航点*/
#define QMAPI_PTZ_STA_START_CRU		(QMAPI_PTZ_STA_BASE+34)		/* 开始巡航 */
#define QMAPI_PTZ_STA_STOP_CRU		(QMAPI_PTZ_STA_BASE+35)		/* 停止巡航 */
#define QMAPI_PTZ_STA_CRU_STATUS	(QMAPI_PTZ_STA_BASE+36)		/* 巡航状态 */


/************************************ALARM IO************************************/
#define QMAPI_STATE_ALARMOUT_STOP	0
#define QMAPI_STATE_ALARMOUT_START	1
#define QMAPI_STATE_ALARMIN_NORMAL	0
#define QMAPI_STATE_ALARMIN_WARNN	1

/***********************************FILE SEARCH**********************************/

#define QMAPI_SEARCH_FILE_SUCCESS	(QMAPI_REC_SEARCH_FILE_BASE+1)	// 获取文件信息成功 
#define QMAPI_SEARCH_FILE_NOFIND	(QMAPI_REC_SEARCH_FILE_BASE+2)	// 未查找到文件 
#define QMAPI_SEARCH_ISFINDING		(QMAPI_REC_SEARCH_FILE_BASE+3)	// 正在查找请等待 
#define QMAPI_SEARCH_NOMOREFILE		(QMAPI_REC_SEARCH_FILE_BASE+4)	// 没有更多的文件，查找结束 
#define QMAPI_SEARCH_FILE_EXCEPTION	(QMAPI_REC_SEARCH_FILE_BASE+5)	// 查找文件时异常 

/************************************************************************/
/*       设备报警事件类型                                               */
/************************************************************************/
typedef enum __QMAPI_ALARM_TYPE_E
{
    QMAPI_ALARM_TYPE_ALARMIN = 0,		//0:信号量报警开始
    QMAPI_ALARM_TYPE_ALARMIN_RESUME,	//1:信号量报警恢复
    QMAPI_ALARM_TYPE_VMOTION,			//2:移动侦测
    QMAPI_ALARM_TYPE_VMOTION_RESUME,	//3:视频移动侦测报警恢复
    QMAPI_ALARM_TYPE_VLOST,			//4:信号丢失
    QMAPI_ALARM_TYPE_VLOST_RESUME,		//5:视频丢失报警恢复
    QMAPI_ALARM_TYPE_VSHELTER,			//6:遮挡报警
	QMAPI_ALARM_TYPE_VSHELTER_RESUME,	//7
    QMAPI_ALARM_TYPE_ILLEGAL_ACCESS,	//8:非法访问
    QMAPI_ALARM_TYPE_NET_BROKEN,		//9:网络断开
    QMAPI_ALARM_TYPE_IP_CONFLICT,		//10:IP冲突
    QMAPI_ALARM_TYPE_DISK_FULL,			//11:硬盘满
    QMAPI_ALARM_TYPE_DISK_UNFORMAT,		//12:硬盘未格式化
    QMAPI_ALARM_TYPE_DISK_RWERR,		//13:读写硬盘出错,
    QMAPI_ALARM_TYPE_BUTT,
}QMAPI_ALARM_TYPE_E;

typedef  enum
{
    QMAPI_REC_PLAYSTART = 0,
    QMAPI_REC_PLAYPAUSE,
    QMAPI_REC_PLAYRESTART,
    QMAPI_REC_FASTFORWARD2X,
    QMAPI_REC_FASTFORWARD4X,
    QMAPI_REC_FASTFORWARD8X,
    QMAPI_REC_FASTFORWARD16X,
    QMAPI_REC_FASTFORWARD32X,
    QMAPI_REC_SLOWPLAY2X,
    QMAPI_REC_SLOWPLAY4X,
    QMAPI_REC_SLOWPLAY8X,
    QMAPI_REC_SLOWPLAY16X,
    QMAPI_REC_SLOWPLAY32X,
    QMAPI_REC_PLAYNORMAL,
    QMAPI_REC_PLAYFRAME,
    QMAPI_REC_PLAYSETPOS,
    QMAPI_REC_PLAYGETPOS,
    QMAPI_REC_PLAYGETTIME,
    QMAPI_REC_PLAYGETFRAME,
    QMAPI_REC_GETTOTALFRAMES,
    QMAPI_REC_GETTOTALTIME,
    QMAPI_REC_THROWBFRAME,
	QMAPI_REC_PLAYJUMPTIME,
}QMAPI_PLAYBACK_CONTROL_E;

/************************************************************************/

//1) 设备（QMAPI_NET_DEVICE_INFO结构）
#define QMAPI_SYSCFG_GET_DEVICECFG				(QMAPI_SYSCFG_GETBASE+0xF000)		//获取设备参数
#define QMAPI_SYSCFG_SET_DEVICECFG				(QMAPI_SYSCFG_SETBASE+0xF001)		//设置设备参数
#define QMAPI_SYSCFG_GET_DEF_DEVICECFG			(QMAPI_SYSCFG_GETBASE+0xF002)		//设置设备参数

#define QMAPI_SYSCFG_GET_RESTORECFG				(QMAPI_SYSCFG_GETBASE+0xF010)		//恢复出厂值配置
#define QMAPI_SYSCFG_SET_RESTORECFG				(QMAPI_SYSCFG_SETBASE+0xF011)
#define QMAPI_SYSCFG_SET_SAVECFG				(QMAPI_SYSCFG_SETBASE+0xF012)		//保存配置
#define QMAPI_SYSCFG_SET_SAVECFG_ASYN			(QMAPI_SYSCFG_SETBASE+0xF013)		//保存配置,异步操作

#define QMAPI_DEV_GET_DEVICEMAINTAINCFG			(QMAPI_SYSCFG_GETBASE+0xF020)
#define QMAPI_DEV_SET_DEVICEMAINTAINCFG			(QMAPI_SYSCFG_SETBASE+0xF021)

/*####################################### ADVANCE ###############################################*/
//用户密钥数据
#define QMAPI_SYSCFG_GET_USERKEY				(QMAPI_SYSCFG_GETBASE+0xF030)
#define QMAPI_SYSCFG_SET_USERKEY				(QMAPI_SYSCFG_SETBASE+0xF031)

#define QMAPI_SYSCFG_STA_AUTO_TEST				(QMAPI_SYSCFG_SETBASE+0xF040)
#define QMAPI_SYSCFG_GET_LOGINFO				(QMAPI_SYSCFG_GETBASE+0xF050)
/*###############################################################################################*/

//2）网络
//本地网络（QMAPI_NET_NETWORK_CFG结构）
#define QMAPI_SYSCFG_GET_NETCFG					(QMAPI_SYSCFG_GETBASE+0xF100)		//获取网络参数
#define QMAPI_SYSCFG_SET_NETCFG					(QMAPI_SYSCFG_SETBASE+0xF101)		//设置网络参数
#define QMAPI_SYSCFG_GET_DEF_NETCFG				(QMAPI_SYSCFG_GETBASE+0xF102)
#define QMAPI_SYSCFG_GET_NETSTATUS              (QMAPI_SYSCFG_GETBASE+0xF103)


//DDNS（QMAPI_NET_DDNSCFG结构）
#define QMAPI_SYSCFG_GET_DDNSCFG				(QMAPI_SYSCFG_GETBASE+0xF110)		//获取DDNS参数
#define QMAPI_SYSCFG_SET_DDNSCFG				(QMAPI_SYSCFG_SETBASE+0xF111)		//设置DDNS参数
/*####################################### ADVANCE ###############################################*/
//EMAIL（QMAPI_NET_EMAIL_PARAM结构）
#define QMAPI_SYSCFG_GET_EMAILCFG				(QMAPI_SYSCFG_GETBASE+0xF120)		//获取EMAIL参数
#define QMAPI_SYSCFG_SET_EMAILCFG				(QMAPI_SYSCFG_SETBASE+0xF121)		//设置EMAIL参数
//FTP（QMAPI_NET_FTP_PARAM结构）
#define QMAPI_SYSCFG_GET_FTPCFG					(QMAPI_SYSCFG_GETBASE+0xF130)		//获取FTP参数
#define QMAPI_SYSCFG_SET_FTPCFG					(QMAPI_SYSCFG_SETBASE+0xF131)		//设置FTP参数
//RTSP 
#define QMAPI_SYSCFG_GET_RTSPCFG				(QMAPI_SYSCFG_GETBASE+0xF133)
#define QMAPI_SYSCFG_SET_RTSPCFG				(QMAPI_SYSCFG_SETBASE+0xF134)		//rtsp设置
//P2P
#define QMAPI_SYSCFG_GET_P2PCFG					(QMAPI_SYSCFG_GETBASE+0xF135)
#define QMAPI_SYSCFG_SET_P2PCFG					(QMAPI_SYSCFG_SETBASE+0xF136)
//PPPOE（QMAPI_NET_PPPOECFG结构）
#define QMAPI_SYSCFG_GET_PPPOECFG				(QMAPI_SYSCFG_GETBASE+0xF137)		//获取PPPOE参数
#define QMAPI_SYSCFG_SET_PPPOECFG				(QMAPI_SYSCFG_SETBASE+0xF138)			//设置PPPOE参数

//中心管理平台（QMAPI_NET_PLATFORM_INFO_V2结构）
#define QMAPI_SYSCFG_GET_PLATFORMCFG            (QMAPI_SYSCFG_GETBASE+0xF160)        //获取中心管理平台参数
#define QMAPI_SYSCFG_SET_PLATFORMCFG            (QMAPI_SYSCFG_SETBASE+0xF161)        //设置中心管理平台参数
/*###############################################################################################*/

//WIFI（QMAPI_NET_WIFI_CONFIG结构）
#define QMAPI_SYSCFG_GET_WIFICFG				(QMAPI_SYSCFG_GETBASE+0xF170)		//获取WIFI参数
#define QMAPI_SYSCFG_SET_WIFICFG				(QMAPI_SYSCFG_SETBASE+0xF171)		//设置WIFI参数
#define QMAPI_SYSCFG_GET_WIFI_SITE_LIST			(QMAPI_SYSCFG_GETBASE+0xF172)		//获取WIFI 站点列表
#define QMAPI_SYSCFG_SET_WIFI_WPS_START			(QMAPI_SYSCFG_SETBASE+0xF173)		//设置WPS开始

/*####################################### ADVANCE ###############################################*/
//广域网无线（QMAPI_NET_WANWIRELESS_CONFIG结构）
#define QMAPI_SYSCFG_GET_WANWIRELESSCFG			(QMAPI_SYSCFG_GETBASE+0xF180)		//获取WANWIRELESS参数
#define QMAPI_SYSCFG_SET_WANWIRELESSCFG			(QMAPI_SYSCFG_SETBASE+0xF181)		//设置WANWIRELESS参数
/*###############################################################################################*/

/*####################################### ADVANCE ###############################################*/
//UPNP (QMAPI_UPNP_CONFIG结构)
#define QMAPI_SYSCFG_GET_UPNPCFG                (QMAPI_SYSCFG_GETBASE+0xF190)		//获取UPNP参数
#define QMAPI_SYSCFG_SET_UPNPCFG				(QMAPI_SYSCFG_SETBASE+0xF191)		//设置UPNP参数
/*###############################################################################################*/

//3）音视频通道
//图象压缩（QMAPI_NET_CHANNEL_PIC_INFO结构）
#define QMAPI_SYSCFG_GET_PICCFG					(QMAPI_SYSCFG_GETBASE+0xF200)		//获取图象压缩参数
#define QMAPI_SYSCFG_SET_PICCFG					(QMAPI_SYSCFG_SETBASE+0xF201)		//设置图象压缩参数
#define QMAPI_SYSCFG_GET_DEF_PICCFG				(QMAPI_SYSCFG_GETBASE+0xF202)
//QMAPI_NET_SUPPORT_STREAM_FMT  图像能力
#define QMAPI_SYSCFG_GET_SUPPORT_STREAM_FMT		(QMAPI_SYSCFG_GETBASE+0xF203)		//获取系统支持的图像能力

#define QMAPI_SYSCFG_GET_VIEWMODE				(QMAPI_SYSCFG_GETBASE+0xF204)		//
#define QMAPI_SYSCFG_SET_VIEWMODE				(QMAPI_SYSCFG_SETBASE+0xF205)		//

//图像字符叠加（QMAPI_NET_CHANNEL_OSDINFO结构）
#define QMAPI_SYSCFG_GET_OSDCFG					(QMAPI_SYSCFG_GETBASE+0xF210)		//获取图象字符叠加参数
#define QMAPI_SYSCFG_SET_OSDCFG					(QMAPI_SYSCFG_SETBASE+0xF211)		//设置图象字符叠加参数
#define QMAPI_SYSCFG_GET_ROICFG					(QMAPI_SYSCFG_GETBASE+0xF212)		//获取图象字符叠加参数
#define QMAPI_SYSCFG_SET_ROICFG					(QMAPI_SYSCFG_SETBASE+0xF213)		//设置图象字符叠加参数

//图像色彩（QMAPI_NET_CHANNEL_COLOR结构）
#define QMAPI_SYSCFG_GET_COLORCFG				(QMAPI_SYSCFG_GETBASE+0xF220)		//获取图象色彩参数
#define QMAPI_SYSCFG_SET_COLORCFG				(QMAPI_SYSCFG_SETBASE+0xF221)		//设置图象色彩参数

//(QMAPI_NET_COLOR_SUPPORT结构)
#define QMAPI_SYSCFG_GET_COLOR_SUPPORT			(QMAPI_SYSCFG_GETBASE+0xF222)		//获取图象色彩参数
//图像色彩(QMAPI_NET_CHANNEL_COLOR_SINGLE结构)
#define QMAPI_SYSCFG_SET_COLORCFG_SINGLE		(QMAPI_SYSCFG_SETBASE+0xF223)		//单独设置某个图象色彩参数,
#define QMAPI_SYSCFG_GET_DEF_COLORCFG			(QMAPI_SYSCFG_GETBASE+0xF224)

//(QMAPI_NET_ENHANCED_COLOR_SUPPORT结构) 
#define QMAPI_SYSCFG_GET_ENHANCED_COLORSUPPORT	(QMAPI_SYSCFG_GETBASE+0xF225)		//获取图象色彩支持高级参数
//(QMAPI_NET_CHANNEL_ENHANCED_COLOR结构)
#define QMAPI_SYSCFG_GET_ENHANCED_COLOR			(QMAPI_SYSCFG_GETBASE+0xF226)		//获取图象色彩高级参数

/*####################################### ADVANCE ###############################################*/
#define QMAPI_SYSCFG_SET_COLOR_BLACK_DETECTION	(QMAPI_SYSCFG_SETBASE+0xF227)		//设置彩转黑检测参数
#define QMAPI_SYSCFG_GET_COLOR_BLACK_DETECTION	(QMAPI_SYSCFG_GETBASE+0xF228)		//获取彩转黑检测参数
/*###############################################################################################*/


//图像遮挡（QMAPI_NET_CHANNEL_SHELTER结构）
#define QMAPI_SYSCFG_GET_SHELTERCFG				(QMAPI_SYSCFG_GETBASE+0xF230)		//获取图象遮挡参数
#define QMAPI_SYSCFG_SET_SHELTERCFG				(QMAPI_SYSCFG_SETBASE+0xF231)		//设置图象遮挡参数
//图像移动侦测（QMAPI_NET_CHANNEL_MOTION_DETECT结构）
#define QMAPI_SYSCFG_GET_MOTIONCFG				(QMAPI_SYSCFG_GETBASE+0xF240)		//获取图象移动侦测参数
#define QMAPI_SYSCFG_SET_MOTIONCFG				(QMAPI_SYSCFG_SETBASE+0xF241)		//设置图象移动侦测参数
#define QMAPI_SYSCFG_DEF_MOTIONCFG				(QMAPI_SYSCFG_GETBASE+0xF242)

/*####################################### ADVANCE ###############################################*/
//图像视频丢失（QMAPI_NET_CHANNEL_VILOST结构）
#define QMAPI_SYSCFG_GET_VLOSTCFG				(QMAPI_SYSCFG_GETBASE+0xF250)		//获取图象视频丢失参数
#define QMAPI_SYSCFG_SET_VLOSTCFG				(QMAPI_SYSCFG_SETBASE+0xF251)		//设置图象视频丢失参数
#define QMAPI_SYSCFG_DEF_VLOSTCFG				(QMAPI_SYSCFG_GETBASE+0xF252)
//图像遮挡报警（QMAPI_NET_CHANNEL_HIDEALARM结构）
#define QMAPI_SYSCFG_GET_HIDEALARMCFG			(QMAPI_SYSCFG_GETBASE+0xF260)		//获取图象遮挡报警参数
#define QMAPI_SYSCFG_SET_HIDEALARMCFG			(QMAPI_SYSCFG_SETBASE+0xF261)		//设置图象遮挡报警参数
#define QMAPI_SYSCFG_DEF_HIDEALARMCFG			(QMAPI_SYSCFG_GETBASE+0xF262)
/*###############################################################################################*/

/*####################################### ADVANCE ###############################################*/
//图像录像（QMAPI_NET_CHANNEL_RECORD结构）
#define QMAPI_SYSCFG_GET_RECORDCFG				(QMAPI_SYSCFG_GETBASE+0xF300)		//获取图象录像参数
#define QMAPI_SYSCFG_SET_RECORDCFG				(QMAPI_SYSCFG_SETBASE+0xF301)		//设置图象录像参数
#define QMAPI_SYSCFG_GET_DEF_RECORDCFG			(QMAPI_SYSCFG_GETBASE+0xF302)

#define QMAPI_SYSCFG_GET_RECORD_ATTR		    		(QMAPI_SYSCFG_GETBASE+0xF303)		//获取图象录像属性
#define QMAPI_SYSCFG_SET_RECORD_ATTR		    		(QMAPI_SYSCFG_SETBASE+0xF304)		//设置图象录像属性


//图像手动录像（QMAPI_NET_CHANNEL_RECORD结构)
#define QMAPI_SYSCFG_GET_RECORDMODECFG			(QMAPI_SYSCFG_GETBASE+0xF320)		//获取图象手动录像参数
#define QMAPI_SYSCFG_SET_RECORDMODECFG			(QMAPI_SYSCFG_SETBASE+0xF321)		//设置图象手动录像参数

//图像定时抓拍（QMAPI_NET_SNAP_TIMER结构）
#define QMAPI_SYSCFG_GET_SNAPTIMERCFG			(QMAPI_SYSCFG_GETBASE+0xF330)		//获取图像定时抓拍参数
#define QMAPI_SYSCFG_SET_SNAPTIMERCFG			(QMAPI_SYSCFG_SETBASE+0xF331)		//设置图像定时抓拍参数
/*###############################################################################################*/

//4）串口（QMAPI_NET_RS232CFG结构）
/*####################################### ADVANCE ###############################################*/
//解码器（QMAPI_NET_DECODERCFG结构）
#define QMAPI_SYSCFG_GET_DECODERCFG				(QMAPI_SYSCFG_GETBASE+0xF400)		//获取解码器参数
#define QMAPI_SYSCFG_SET_DECODERCFG				(QMAPI_SYSCFG_SETBASE+0xF401)		//设置解码器参数
#define QMAPI_SYSCFG_GET_DEF_DECODERCFG			(QMAPI_SYSCFG_GETBASE+0xF402)		//设置解码器参数

#define QMAPI_SYSCFG_GET_RS232CFG				(QMAPI_SYSCFG_GETBASE+0xF410)		//获取232串口参数
#define QMAPI_SYSCFG_SET_RS232CFG				(QMAPI_SYSCFG_SETBASE+0xF411)		//设置232串口参数
#define QMAPI_SYSCFG_GET_DEF_SERIAL				(QMAPI_SYSCFG_GETBASE+0xF412)
//(QMAPI_NET_PTZ_PROTOCOLCFG结构)
#define QMAPI_SYSCFG_GET_PTZ_PROTOCOLCFG		(QMAPI_SYSCFG_GETBASE+0xF420)		//获取支持的云台协议信息
//(QMAPI_NET_PTZ_PROTOCOL_DATA 结构)
#define QMAPI_SYSCFG_ADD_PTZ_PROTOCOL			(QMAPI_SYSCFG_SETBASE+0xF421)		//添加云台协议
//(QMAPI_NET_PTZ_PROTOCOL_DATA 结构)
#define QMAPI_SYSCFG_GET_PTZ_PROTOCOL_DATA		(QMAPI_SYSCFG_GETBASE+0xF422)		//获取云台协议数据
//(QMAPI_NET_PTZ_POS 结构)
#define	QMAPI_SYSCFG_GET_PTZPOS					(QMAPI_SYSCFG_GETBASE+0xF430)		//云台获取PTZ位置
#define	QMAPI_SYSCFG_SET_PTZPOS					(QMAPI_SYSCFG_GETBASE+0xF431)		//云台设置PTZ位置
/*###############################################################################################*/

//5）报警
/*####################################### ADVANCE ###############################################*/
//输入（QMAPI_NET_ALARMINCFG结构）
#define QMAPI_SYSCFG_GET_ALARMINCFG				(QMAPI_SYSCFG_GETBASE+0xF500)		//获取报警输入参数
#define QMAPI_SYSCFG_SET_ALARMINCFG				(QMAPI_SYSCFG_SETBASE+0xF501)		//设置报警输入参数
#define QMAPI_SYSCFG_DEF_ALARMINCFG				(QMAPI_SYSCFG_GETBASE+0xF502)
//输出（QMAPI_NET_ALARMOUTCFG结构）
#define QMAPI_SYSCFG_GET_ALARMOUTCFG			(QMAPI_SYSCFG_GETBASE+0xF510)		//获取报警输出参数
#define QMAPI_SYSCFG_SET_ALARMOUTCFG			(QMAPI_SYSCFG_SETBASE+0xF511)		//设置报警输出参数
#define QMAPI_SYSCFG_DEF_ALARMOUTCFG			(QMAPI_SYSCFG_GETBASE+0xF512)

//RESET 按键状态获取
#define QMAPI_SYSCFG_GET_RESETSTATE             (QMAPI_SYSCFG_GETBASE+0xF520)
/*###############################################################################################*/

/*####################################### ADVANCE ###############################################*/
//默认配置
#define QMAPI_SYSCFG_GET_DEFAULTCFG_FILE		(QMAPI_SYSCFG_GETBASE+0xF600)
#define QMAPI_SYSCFG_SET_DEFAULTCFG_FILE		(QMAPI_SYSCFG_SETBASE+0xF601)
//logo文件
#define QMAPI_SYSCFG_GET_LOGO_FILE				(QMAPI_SYSCFG_GETBASE+0xF701)
#define QMAPI_SYSCFG_SET_LOGO_FILE				(QMAPI_SYSCFG_SETBASE+0xF702)
/*###############################################################################################*/

/*####################################### ADVANCE ###############################################*/
//获取所有已设置的预置点的信息-QMAPI_NET_PRESET_INFO
#define QMAPI_SYSCFG_GET_ALL_PRESET				(QMAPI_SYSCFG_GETBASE+0xF780)
//获取通道的所有已经设置的巡航组 QMAPI_NET_CRUISE_CFG
#define QMAPI_SYSCFG_GET_CRUISE_CFG				(QMAPI_SYSCFG_GETBASE+0xF781)
//设置一个巡航   QMAPI_NET_CRUISE_INFO
#define QMAPI_SYSCFG_SET_CRUISE_INFO			(QMAPI_SYSCFG_SETBASE+0xF782)
/*###############################################################################################*/


//6）时间
//时区和夏时制（QMAPI_NET_ZONEANDDST结构）
#define QMAPI_SYSCFG_GET_ZONEANDDSTCFG			(QMAPI_SYSCFG_GETBASE+0xF800)		//获取时区和夏时制参数
#define QMAPI_SYSCFG_SET_ZONEANDDSTCFG			(QMAPI_SYSCFG_SETBASE+0xF801)		//设置时区和夏时制参数
//NTP （QMAPI_NET_NTP_CFG结构）
#define QMAPI_SYSCFG_GET_NTPCFG					(QMAPI_SYSCFG_GETBASE+0xF802)		//获取NTP 参数      
#define QMAPI_SYSCFG_SET_NTPCFG					(QMAPI_SYSCFG_SETBASE+0xF803)		//设置NTP 参数      
#define QMAPI_SYSCFG_DEF_NTPCFG					(QMAPI_SYSCFG_GETBASE+0xF804)

/*####################################### ADVANCE ###############################################*/
//7）本地预览（QMAPI_NET_PREVIEWCFG结构）
#define QMAPI_SYSCFG_GET_PREVIEWCFG				(QMAPI_SYSCFG_GETBASE+0xF900)		//获取预览参数
#define QMAPI_SYSCFG_SET_PREVIEWCFG				(QMAPI_SYSCFG_SETBASE+0xF901)		//设置预览参数
#define QMAPI_SYSCFG_GET_DEF_PREVIEWCFG			(QMAPI_SYSCFG_GETBASE+0xF902)
//8）视频输出（QMAPI_NET_VIDEOOUT结构）
#define QMAPI_SYSCFG_GET_VIDEOOUTCFG			(QMAPI_SYSCFG_GETBASE+0xF910)		//获取视频输出参数
#define QMAPI_SYSCFG_SET_VIDEOOUTCFG			(QMAPI_SYSCFG_SETBASE+0xF911)		//设置视频输出参数
#define QMAPI_SYSCFG_GET_DEF_VIDEOOUTCFG		(QMAPI_SYSCFG_GETBASE+0xF912)		//设置视频输出参数
//本地硬盘信息（QMAPI_NET_HDCFG结构）
#define QMAPI_SYSCFG_GET_HDCFG					(QMAPI_SYSCFG_GETBASE+0xF920)		//获取硬盘参数
#define QMAPI_SYSCFG_SET_HDCFG					(QMAPI_SYSCFG_SETBASE+0xF921)		//设置(单个)硬盘参数
#define QMAPI_SYSCFG_HD_FORMAT					(QMAPI_SYSCFG_GETBASE+0xF922)		//格式化硬盘
#define QMAPI_SYSCFG_GET_HD_FORMAT_STATUS		(QMAPI_SYSCFG_GETBASE+0xF923)		//格式化硬盘状态以及进度
#define QMAPI_SYSCFG_UNLOAD_DISK				(QMAPI_SYSCFG_GETBASE+0xF924)		//卸载磁盘

//本地盘组信息配置（QMAPI_NET_HDGROUP_CFG结构）
#define QMAPI_SYSCFG_GET_HDGROUPCFG				(QMAPI_SYSCFG_GETBASE+0xF930)		//获取硬盘组参数
#define QMAPI_SYSCFG_SET_HDGROUPCFG				(QMAPI_SYSCFG_GETBASE+0xF931)		//设置硬盘组参数
/*###############################################################################################*/

//9) 用户、用户组
//用户（QMAPI_NET_USER_INFO结构）
#define QMAPI_SYSCFG_GET_USERCFG				(QMAPI_SYSCFG_GETBASE+0xFA00)		//获取用户参数
#define QMAPI_SYSCFG_SET_USERCFG				(QMAPI_SYSCFG_SETBASE+0xFA01)		//设置用户参数
/*####################################### ADVANCE ###############################################*/
//用户组（QMAPI_NET_USER_GROUP_INFO结构）
#define QMAPI_SYSCFG_GET_USERGROUPCFG			(QMAPI_SYSCFG_GETBASE+0xFA02)		//获取用户组参数
#define QMAPI_SYSCFG_SET_USERGROUPCFG			(QMAPI_SYSCFG_SETBASE+0xFA03)		//设置用户组参数
/*###############################################################################################*/

/*####################################### ADVANCE ###############################################*/
//10)异常（QMAPI_NET_EXCEPTION结构）
#define QMAPI_SYSCFG_GET_EXCEPTIONCFG			(QMAPI_SYSCFG_GETBASE+0xFB00)		//获取异常参数
#define QMAPI_SYSCFG_SET_EXCEPTIONCFG			(QMAPI_SYSCFG_SETBASE+0xFB01)		//设置异常参数
/*###############################################################################################*/

//消费类特有配置    //QMAPI_NET_CONSUMER_INFO
#define QMAPI_SYSCFG_GET_CONSUMER				(QMAPI_SYSCFG_GETBASE+0xFB02)
#define QMAPI_SYSCFG_SET_CONSUMER				(QMAPI_SYSCFG_GETBASE+0xFB03)

//ONVIF (QMAPI_SYSCFG_ONVIFTESTINFO)
#define QMAPI_SYSCFG_GET_ONVIFTESTINFO          (QMAPI_SYSCFG_GETBASE+0xFB05)       //获取ONVIF 测试验证参数
#define QMAPI_SYSCFG_SET_ONVIFTESTINFO          (QMAPI_SYSCFG_GETBASE+0xFB06)


//消息通知用户，结构体：QMAPI_NET_NOTIFY_USER_INFO
#define QMAPI_SYSCFG_NOTIFY_USER 				(QMAPI_SYSCFG_GETBASE+0xFC00)


/*####################################### ADVANCE ###############################################*/
// 请求视频流关键帧 
#define QMAPI_NET_STA_IFRAME_REQUEST          	(QMAPI_SYS_FUN_BASE+0xF001)
// 配置音频对讲数据回调函数(OnNetAudioStreamCallBack)
#define QMAPI_NET_REG_AUDIOCALLBACK				(QMAPI_SYS_FUN_BASE+0xF002)
#define QMAPI_NET_UNREG_AUDIOCALLBACK			(QMAPI_SYS_FUN_BASE+0xF003)
// 配置视频数据回调函数(QMAPI_NET_VIDEOCALLBACK)
#define QMAPI_NET_REG_VIDEOCALLBACK				(QMAPI_SYS_FUN_BASE+0xF004)
#define QMAPI_NET_UNREG_VIDEOCALLBACK			(QMAPI_SYS_FUN_BASE+0xF005)
// 配置报警消息回调函数(OnNetAlarmCallback)
#define QMAPI_NET_REG_ALARMCALLBACK				(QMAPI_SYS_FUN_BASE+0xF006)
#define QMAPI_NET_UNREG_ALARMCALLBACK			(QMAPI_SYS_FUN_BASE+0xF007)
// 配置串口数据回调函数(OnNetSerialDataCallback)
#define QMAPI_NET_REG_SERIALDATACALLBACK		(QMAPI_SYS_FUN_BASE+0xF008)
#define QMAPI_NET_UNREG_SERIALDATACALLBACK		(QMAPI_SYS_FUN_BASE+0xF009)
//注册回放回调函数
#define QMAPI_NET_REG_PLAYBACKCALLBACK			(QMAPI_SYS_FUN_BASE+0xF00A)
#define QMAPI_NET_UNREG_PLAYBACKCALLBACK		(QMAPI_SYS_FUN_BASE+0xF00B)

//打开某通道视频
#define QMAPI_NET_STA_START_VIDEO				(QMAPI_SYS_FUN_BASE+0xF010)
//关闭某通道视频
#define QMAPI_NET_STA_STOP_VIDEO				(QMAPI_SYS_FUN_BASE+0xF011)
//开始音频对讲
#define QMAPI_NET_STA_START_TALKAUDIO			(QMAPI_SYS_FUN_BASE+0xF020)
//停止音频对讲
#define QMAPI_NET_STA_STOP_TALKAUDIO			(QMAPI_SYS_FUN_BASE+0xF021)
//获取视频编码信息
#define QMAPI_NET_STA_GET_SPSPPSENCODE_DATA		(QMAPI_SYS_FUN_BASE+0xF030)

// 打开透明串口传输
#define QMAPI_NET_STA_OPEN_SERIALTRANSP			(QMAPI_SYS_FUN_BASE+0xF100)
// 关闭透明串口传输
#define QMAPI_NET_STA_CLOSE_SERIALTRANSP		(QMAPI_SYS_FUN_BASE+0xF101)
// 云台控制( QMAPI_NET_PTZ_CONTROL 结构体)
#define QMAPI_NET_STA_PTZ_CONTROL				(QMAPI_SYS_FUN_BASE+0xF110)
// 报警输出控制(命令模式QMAPI_NET_ALARMOUT_CONTROL)
#define QMAPI_NET_STA_ALARMOUT_CONTROL			(QMAPI_SYS_FUN_BASE+0xF120)
// 获取报警输出状态(QMAPI_NET_SENSOR_STATE)
#define QMAPI_NET_GET_ALARMOUT_STATE			(QMAPI_SYS_FUN_BASE+0xF121) 
// 获取报警输入状态(QMAPI_NET_SENSOR_STATE)
#define QMAPI_NET_GET_ALARMIN_STATE				(QMAPI_SYS_FUN_BASE+0xF122)

// 重启设备
#define QMAPI_NET_STA_REBOOT					(QMAPI_SYS_FUN_BASE+0xF130)
// 设备关机
#define QMAPI_NET_STA_SHUTDOWN					(QMAPI_SYS_FUN_BASE+0xF131)
//获取系统时间
#define QMAPI_NET_STA_GET_SYSTIME				(QMAPI_SYS_FUN_BASE+0xF132)
//设置系统时间
#define QMAPI_NET_STA_SET_SYSTIME				(QMAPI_SYS_FUN_BASE+0xF133)

// 抓拍图片(QMAPI_NET_SNAP_DATA)
#define QMAPI_NET_STA_SNAPSHOT					(QMAPI_SYS_FUN_BASE+0xF140)
//录像控制(QMAPI_NET_REC_CONTROL)
#define QMAPI_NET_STA_REC_CONTROL				(QMAPI_SYS_FUN_BASE+0xF150)

#define QMAPI_NET_STA_CLOSE_RS485				(QMAPI_SYS_FUN_BASE+0xF200)
#define QMAPI_NET_STA_GET_VIDEO_STATE			(QMAPI_SYS_FUN_BASE+0xF201)

//文件系统升级请求(QMAPI_NET_UPGRADE_REQ)
#define QMAPI_NET_STA_UPGRADE_REQ				(QMAPI_SYS_FUN_BASE+0xF300)
//文件系统升级请求(QMAPI_NET_UPGRADE_RESP)
#define QMAPI_NET_STA_UPGRADE_RESP				(QMAPI_SYS_FUN_BASE+0xF301)
//文件系统升级数据(QMAPI_NET_UPGRADE_DATA)
#define QMAPI_NET_STA_UPGRADE_DATA				(QMAPI_SYS_FUN_BASE+0xF302)

//文件系统升级准备
#define QMAPI_NET_STA_UPGRADE_PREPARE           (QMAPI_SYS_FUN_BASE+0xF303)
//文件系统升级开始
#define QMAPI_NET_STA_UPGRADE_START             (QMAPI_SYS_FUN_BASE+0xF304)



/*###############################################################################################*/

/*########################################yi zhangy#######################################################*/
// 获取加密芯片信息
#define QMAPI_SYSCFG_GET_ENCRYPT				(QMAPI_TL_SERVER_BASE+0x0001)
#define QMAPI_SYSCFG_SET_ENCRYPT				(QMAPI_TL_SERVER_BASE+0x0002)

// 获取
#define QMAPI_SYSCFG_GET_TLSERVER				(QMAPI_TL_SERVER_BASE+0x0003)
#define QMAPI_SYSCFG_SET_TLSERVER				(QMAPI_TL_SERVER_BASE+0x0004)




#endif  //_QMAPI_DATATYPE_H



