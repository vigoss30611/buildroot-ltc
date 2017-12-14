#ifndef _QMAPI_ERRNO_H_
#define _QMAPI_ERRNO_H_

/************************************************************************/
/* 错误码                                                      
   程序的错误码的值占用4个字节，最高位为1
*/
/************************************************************************/
#define		QMAPI_ERR_BASE						0xE0000000
#define 	QMAPI_SUCCESS						0x00000000					//成功
#define 	QMAPI_FAILURE						-1							//失败
#define 	QMAPI_ERR_ENCRYPT_IC_NO_MATCH		(QMAPI_ERR_BASE+0xF001)		//加密芯片校验失败
#define 	QMAPI_ERR_ENCRYPT_IC_NO_FIND		(QMAPI_ERR_BASE+0xF002)		//加密芯片没有找到
#define 	QMAPI_ERR_DEVICE_NOT_OPENED			(QMAPI_ERR_BASE+0xF003)		//设备没有打开
#define 	QMAPI_ERR_DEVICE_NOT_STARTED		(QMAPI_ERR_BASE+0xF004)		//设备没有启动
#define 	QMAPI_ERR_MEMORY_ALLOC				(QMAPI_ERR_BASE+0xF005)		//分配内存失败
#define 	QMAPI_ERR_INVALID_HANDLE			(QMAPI_ERR_BASE+0xF006)		//无效的句柄
#define 	QMAPI_ERR_INVALID_PARAM				(QMAPI_ERR_BASE+0xF007)		//无效的参数
#define 	QMAPI_ERR_UNKNOW_COMMNAD			(QMAPI_ERR_BASE+0xF008)		//无法识别的命令
#define 	QMAPI_ERR_TIMEOUT					(QMAPI_ERR_BASE+0xF009)		//超时退出
#define 	QMAPI_ERR_READDATA_FAILED			(QMAPI_ERR_BASE+0xF00a)		//读数据失败
#define 	QMAPI_ERR_DATA_TOO_BIG				(QMAPI_ERR_BASE+0xF00b)		//数据包太大
#define     QMAPI_ERR_INVALID_CHANNEL			(QMAPI_ERR_BASE+0xF00c)		//无效的通道索引
#define     QMAPI_ERR_FILE_OPEN_FAILED			(QMAPI_ERR_BASE+0xF00d)		//打开文件失败
#define     QMAPI_ERR_FILE_READ_FAILED			(QMAPI_ERR_BASE+0xF00e)		//读文件数据失败
#define     QMAPI_ERR_FILE_WRITE_FAILED			(QMAPI_ERR_BASE+0xF00f)		//写文件数据失败
#define 	QMAPI_ERR_LOGIN_USRNAME				(QMAPI_ERR_BASE+0xF010)		//登陆用户名错误
#define 	QMAPI_ERR_LOGIN_PSW					(QMAPI_ERR_BASE+0xF011)		//登陆密码错误
#define 	QMAPI_ERR_NOT_SUPPORT				(QMAPI_ERR_BASE+0xF012)		//系统不支持
#define 	QMAPI_ERR_CHANNEL_ID				(QMAPI_ERR_BASE+0xF013)		//错误的通道号
#define		QMAPI_ERR_SYSTEM_BUSY				(QMAPI_ERR_BASE+0xF014)		//系统忙
#define		QMAPI_ERR_NO_FILE					(QMAPI_ERR_BASE+0xF015)		//无此文件
#define		QMAPI_ERR_ILLEGAL_PARAM				(QMAPI_ERR_BASE+0xF016)		//参数超出合法范围
#define     QMAPI_ERR_UNINIT					(QMAPI_ERR_BASE+0xF017)		//模块未初始化
#define		QMAPI_ERR_LOGOUT					(QMAPI_ERR_BASE+0xF018)		//请确认要注销系统
#define		QMAPI_ERR_LOGOFF					(QMAPI_ERR_BASE+0xF019)		//请确认要关闭系统
#define     QMAPI_ERR_RESTART					(QMAPI_ERR_BASE+0xF020)		//请确认要重启系统
#define     QMAPI_ERR_RESTORE					(QMAPI_ERR_BASE+0xF021)		//请确认要恢复默认参数
#define     QMAPI_ERR_UPGRADE					(QMAPI_ERR_BASE+0xF022)		//请确认要升级设备
#define     QMAPI_ERR_INVALID_DATA				(QMAPI_ERR_BASE+0xF023) 
//视频编码、采集错误码
#define     QMAPI_ERR_INVALID_VENC_NUM			(QMAPI_ERR_BASE+0xF100)		//无效的编码通道
#define     QMAPI_ERR_VENC_NUM_EXCEED_SYS		(QMAPI_ERR_BASE+0xF101)		//超过系统最大通道数
#define     QMAPI_ERR_VENC_INVALID_STREAM_MODE	(QMAPI_ERR_BASE+0xF102)		//无效的流编码模式
#define     QMAPI_ERR_VENC_NOT_OPENED			(QMAPI_ERR_BASE+0xF103)		//编码器没有打开
#define     QMAPI_ERR_VENC_VB_SETCONF			(QMAPI_ERR_BASE+0xF104)		//媒体库参数设置失败
#define     QMAPI_ERR_VENC_VB_INIT				(QMAPI_ERR_BASE+0xF105)		//媒体库初始化失败
#define     QMAPI_ERR_VENC_SYS_SETCONF			(QMAPI_ERR_BASE+0xF106)		//编码通道参数设置失败
#define     QMAPI_ERR_VENC_SYS_INIT				(QMAPI_ERR_BASE+0xF107)		//编码通道初始化失败
#define     QMAPI_ERR_VENC_CREATE_GROUP			(QMAPI_ERR_BASE+0xF108)		//创建编码通道组失败
#define     QMAPI_ERR_VENC_CREATE_BINDINPUT		(QMAPI_ERR_BASE+0xF109)		//视频输入通道绑定失败
#define     QMAPI_ERR_VENC_VI_ENABLECHN			(QMAPI_ERR_BASE+0xF10a)		//启用视频输入通道失败
#define     QMAPI_ERR_VENC_MD_DISABLED			(QMAPI_ERR_BASE+0xF10b)		//视频移动侦测未启用
#define     QMAPI_ERR_VENC_NODATA				(QMAPI_ERR_BASE+0xF10c)		//视频暂无数据
//视频解码输出错误码
//音频编码、采集错误码
#define     QMAPI_ERR_INVALID_AENC_NUM			(QMAPI_ERR_BASE+0xF200)		//无效的音频编码通道
//音频解码、输出错误码
//串口错误码
//IO报警错误码


//录像错误码

//云台控制错误
#define     QMAPI_ERR_PTZ_COMMAND_NOT_SUPPORT	(QMAPI_ERR_BASE+0xF500)		//操作命令不支持
#define     QMAPI_ERR_PTZ_PROTOCOL_NOT_SUPPORT	(QMAPI_ERR_BASE+0xF501)		//云台协议不支持
#define     QMAPI_ERR_PTZ_PARSE_FAILED			(QMAPI_ERR_BASE+0xF502)		//文件解析失败
//用户认证、操作
#define     QMAPI_ERR_USER_INVALID_USERID		(QMAPI_ERR_BASE+0xF600)		//无效用户ID
#define     QMAPI_ERR_USER_NO_USER				(QMAPI_ERR_BASE+0xF601)		//没有该用户
#define     QMAPI_ERR_USER_INVALID_PASSWORD		(QMAPI_ERR_BASE+0xF602)		//密码错误
#define     QMAPI_ERR_USER_NO_RIGHT				(QMAPI_ERR_BASE+0xF603)		//没有操作权限
#define     QMAPI_ERR_USER_IP_NOT_MATCH			(QMAPI_ERR_BASE+0xF604)		//访问用户网络地址受限
#define     QMAPI_ERR_USER_MAC_NOT_MATCH		(QMAPI_ERR_BASE+0xF605)		//访问用户机器码受限

//网络错误码
#define     QMAPI_ERR_SOCKET_TIMEOUT			(QMAPI_ERR_BASE+0xF700)     
#define     QMAPI_ERR_SOCKET_ERR				(QMAPI_ERR_BASE+0xF701)		//7 creat socket error.
#define     QMAPI_ERR_BIND_ERR					(QMAPI_ERR_BASE+0xF702)		//8 connect to server error.		
#define     QMAPI_ERR_CONNECT_ERR				(QMAPI_ERR_BASE+0xF703)		//9 connect to server error.
#define     QMAPI_ERR_SEND_ERR					(QMAPI_ERR_BASE+0xF704)		//10 send data error.
#define     QMAPI_ERR_RECV_ERR					(QMAPI_ERR_BASE+0xF705)		//10 recv data error.  
#define     QMAPI_ERR_SOCKET_INVALID			(QMAPI_ERR_BASE+0xF706)

//磁盘
#define 	QMAPI_ERR_NOFIND_DISK				(QMAPI_ERR_BASE+0xF700) 	//系统找到硬盘
#define 	QMAPI_ERR_DISK_NOT_EXIST			(QMAPI_ERR_BASE+0xF701) 	//硬盘号不存在
#define 	QMAPI_ERR_DISK_NO_USBDISK			(QMAPI_ERR_BASE+0xF702) 	//无存储设备
#define 	QMAPI_ERR_DISK_FULL					(QMAPI_ERR_BASE+0xF703)
#define 	QMAPI_ERR_DISK_UNMOUNTED			(QMAPI_ERR_BASE+0xF704)
#define 	QMAPI_ERR_DISK_OPERATE				(QMAPI_ERR_BASE+0xF705)
#define 	QMAPI_ERR_DISK_READ_ONLY			(QMAPI_ERR_BASE+0xF706)
#define 	QMAPI_ERR_DISK_WRITE				(QMAPI_ERR_BASE+0xF707)

//录像
#define 	QMAPI_ERR_NO_RECORD					(QMAPI_ERR_BASE+0xF800) 	//系统没有录像
#define 	QMAPI_ERR_NOT_IFRAME				(QMAPI_ERR_BASE+0xF801)		//非IFrame
#define 	QMAPI_ERR_PACKET_DATA				(QMAPI_ERR_BASE+0xF802)
//升级
#define 	QMAPI_ERR_UPGRADE_WAITING			(QMAPI_ERR_BASE+0xF900)		//正在等待升级
#define 	QMAPI_ERR_UPGRADING					(QMAPI_ERR_BASE+0xF901)		//正在升级
#define 	QMAPI_ERR_UPGRADE_FINDING_FILE		(QMAPI_ERR_BASE+0xF902)		//正在查找升级文件
#define 	QMAPI_ERR_UPGRADE_SERVERTYPE		(QMAPI_ERR_BASE+0xF903)		//服务器类型不匹配
#define 	QMAPI_ERR_UPGRADE_CHANNELNUM		(QMAPI_ERR_BASE+0xF904)		//通道个数不匹配
#define 	QMAPI_ERR_UPGRADE_UNSUPPORT			(QMAPI_ERR_BASE+0xF905)		//不支持的升级数据包类型
#define 	QMAPI_ERR_UPGRADE_NO_FILE			(QMAPI_ERR_BASE+0xF906)		//没有找到升级文件
#define 	QMAPI_ERR_UPGRADE_CRC_FAILED		(QMAPI_ERR_BASE+0xF907)		//CRC校验失败
#define 	QMAPI_ERR_UPGRADE_FILE_READ			(QMAPI_ERR_BASE+0xF908)		//无效升级文件
#define 	QMAPI_ERR_UPGRADE_NO_ITEM			(QMAPI_ERR_BASE+0xF909)		//没有包含升级数据
#define 	QMAPI_ERR_UPGRADE_NO_USBDISK		(QMAPI_ERR_BASE+0xF90a)		//没有插入升级硬盘
#define 	QMAPI_ERR_UPGRADE_DISK_READ			(QMAPI_ERR_BASE+0xF90b)		//硬盘不可读
#define 	QMAPI_ERR_UPGRADE_CPUTYPE			(QMAPI_ERR_BASE+0xF90c)		//CPU类型不匹配 
#define 	QMAPI_ERR_UPGRADE_SYSTEM_BUSY		(QMAPI_ERR_BASE+0xF90d)		//系统忙
#define 	QMAPI_ERR_UPGRADE_INVALID_FILE		(QMAPI_ERR_BASE+0xF90e)		//非法升级文件
#define 	QMAPI_ERR_UPGRADE_ALLOC				(QMAPI_ERR_BASE+0xF90f)		//分配内存失败
#define 	QMAPI_ERR_UPGRADE_DATA				(QMAPI_ERR_BASE+0xF910)		//数据错误
#define 	QMAPI_ERR_UPGRADE_OPEN				(QMAPI_ERR_BASE+0xF911)		//打开设备失败
#define 	QMAPI_ERR_UPGRADE_WRITE				(QMAPI_ERR_BASE+0xF912)		//写数据错误
#define 	QMAPI_ERR_UPGRADE_NO_SUPORT			(QMAPI_ERR_BASE+0xF913)		//写数据错误


//音视频数据缓冲操作
#define 	QMAPI_ERR_MBUF_NODATA				(QMAPI_ERR_BASE+0xFA00)		//缓冲中无数据

//DDNS
#define 	QMAPI_ERR_DDNS_CONNECT				(QMAPI_ERR_BASE+0xFB00)		// 网络连接失败
#define 	QMAPI_ERR_DDNS_DATA					(QMAPI_ERR_BASE+0xFB01)		// 更新失败
#define 	QMAPI_ERR_DDNS_RS					(QMAPI_ERR_BASE+0xFB02)		// 请求dns服务器出错
#define 	QMAPI_ERR_DDNS_EXISTED				(QMAPI_ERR_BASE+0xFB03)		// 域名已存在(注册的域名冲突)

//用户配置管理
#define		QMAPI_ERR_USER_CFG_NO_USER			(QMAPI_ERR_BASE+0xFC00)		//用户不存在

#endif //_QMAPI_ERRNO_H_


