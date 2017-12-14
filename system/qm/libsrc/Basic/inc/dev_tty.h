#ifndef __DEV_TTY_H__
#define __DEV_TTY_H__

//#define __Dacheng_serial
//#endif
/************************************************************************/
/* 串口模块                                                             */
/* serial comm function, support select()								*/
/************************************************************************/

typedef struct tagDev_Inter_Serial_Attr
{
	BYTE    byMode;    //串口打开模式，0:232打开模式,1:485读写,2:485只写,3:485只读
	DWORD	dwBaudRate;/* 波特率(bps)，0－50；1－75；2－110；3－150；4－300；5－600；6－1200；7－2400；8－4800；9－9600；10－19200；11－38400；12－57600；13－76800；14－115.2k */
	BYTE	byDataBit; /* 数据有几位 0－5位，1－6位，2－7位，3－8位 */
	BYTE	byStopBit; /* 停止位 0－1位，1－2位  */
	BYTE	byParity;  /* 校验 0－无校验，1－奇校验，2－偶校验; */
	BYTE	byFlowcontrol; /* 0－无，1－软流控,2-硬流控 */
} DEV_INTER_SERIAL_ATTR;


/* 打开串口设备，返回串口句柄，<0失败错误码，成功返回>=0句柄*/
 int DMS_DEV_TTY_Open(int deviceno);
/* 关闭串口设备 */
 int DMS_DEV_TTY_Close(int fd);
/* 阻塞方式，读入串口数据 */
 int DMS_DEV_TTY_ReadBlock(int fd, void *data, int maxlen);
/* 非阻塞方式，读入串口数据 */
 int DMS_DEV_TTY_Read(int fd, void *data, int maxlen);
/* 阻塞方式，写入串口数据 */
 int DMS_DEV_TTY_Write(int fd, void *data, int len);
/* 设置串口参数 */
 int DMS_DEV_TTY_SetAttr(int fd, DEV_INTER_SERIAL_ATTR *attr);
/* 获取串口参数 */
 int DMS_DEV_TTY_GetAttr(int fd, DEV_INTER_SERIAL_ATTR *attr);


/************************宏定义****************************************/
/*串口设备号*/
#ifdef __Dacheng_serial					//大成485与打印串口调换
#define  COM_PORT_0		1    //打印串口
#define  COM_PORT_1		0   // 485 串口
#define  COM_PORT_2		2
#define  COM_PORT_3		3
#else
#define  COM_PORT_0		0    //打印串口
#define  COM_PORT_1		1   // 485 串口
#define  COM_PORT_2		2
#define  COM_PORT_3		3
#endif

/*串口打开方式*/
#define COM_OPEN_MODE_232       0
#define COM_OPEN_MODE_485_RW    1
#define COM_OPEN_MODE_485_W		2
#define COM_OPEN_MODE_485_R		3


#endif //__DEV_TTY_H__
