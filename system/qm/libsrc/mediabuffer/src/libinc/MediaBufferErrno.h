

#ifndef __HI_COMMON_ERRNO_H__
#define __HI_COMMON_ERRNO_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

//#define HI_ERR_APPID         (0x80UL + 0x28UL)
/******************************************************************************
|----------------------------------------------------------------| 
| 1 |   APP_ID   |   MOD_ID    | ERR_LEVEL |   ERR_ID            | 
|----------------------------------------------------------------| 
|<--><--7bits----><----8bits---><--4bits---><------12bits------->|
******************************************************************************/

#define HI_COMMON_ERR(appid, modid, level, errid) \
    ( (HI_S32)((((HI_U32)(appid))<<24) | (((HI_U32)(modid) ) << 16) | (((HI_U32)(level))<<12) | (((HI_U32)(errid)) & 0xFFF )))


/*************************************************************************
  模块错误码定义方法
*************************************************************************/

/* 定义4个等级*/
typedef enum hiERRLEVEL_E
{
    HI_ERRLEVEL_DEBUG = 0,   /* debug-level                                  */
    HI_ERRLEVEL_INFO,        /* informational                                */
    HI_ERRLEVEL_ERROR,       /* error conditions                             */
    HI_ERRLEVEL_FATAL,       /* fatal error,make system disable */
    HI_ERRLEVEL_BUTT
} HI_ERRLEVEL_E;


/*----------------------------------------------*
 * 公共错误码定义                               *
 *----------------------------------------------*/

#define HI_ERRNO_COMMON_BASE    0x10000000
#define HI_ERRNO_COMMON_COUNT   0x100

#define HI_EUNKNOWN             (HI_ERRNO_COMMON_BASE + 1)    /* 错误原因未知，确定已经出错但是不能判定错误的原因 */
#define HI_EOTHER               (HI_ERRNO_COMMON_BASE + 2)    /* 其他错误，知道错误原因但是难以归类 */
#define HI_EINTER               (HI_ERRNO_COMMON_BASE + 3)    /* 内部错误，例如内部发生断言错误、某些内存崩溃、CPU自测错、I/O错、数学运算错、死锁等等 */
#define HI_EVERSION             (HI_ERRNO_COMMON_BASE + 4)    /* 版本错误 */
#define HI_EPAERM               (HI_ERRNO_COMMON_BASE + 5)    /* 参数错误，包括函数参数错误、配置参数错误、配置参数不一致或有冲突，不适当的编号、通道号、设备号、空指针、地址、操作码等等 */
#define HI_EINVAL               (HI_ERRNO_COMMON_BASE + 6)    /* 不支持的操作/功能/特性，可能其他版本或者未来的版本或者其他产品会有支持 */
#define HI_ENOTINIT             (HI_ERRNO_COMMON_BASE + 7)    /* 没有初始化，某些操作必须在先进行初始化后才能进行，例如未加锁 */
#define HI_ENOTREADY            (HI_ERRNO_COMMON_BASE + 8)    /* 没有准备好，某些操作必须在具备一定条件后才能进行，或者需要的环境不正确，或者缺乏必需的资源 */
#define HI_ENORES               (HI_ERRNO_COMMON_BASE + 9)    /* 没有资源，例如申请内存失败、没有空闲缓冲区、没有空闲端口、没有空闲通道等 */
#define HI_EEXIST               (HI_ERRNO_COMMON_BASE + 0xA)  /* 资源已存在，欲申请或新建的资源已经存在，容器中有容量 */
#define HI_ELOST                (HI_ERRNO_COMMON_BASE + 0xB)  /* 资源不存在，依赖的某种资源、地址、会话不存在 */
#define HI_ENOOP                (HI_ERRNO_COMMON_BASE + 0xC)  /* 资源不可操作，可能已损坏不可使用、校验错误、未发挥预期的作用、设备不兼容等等 */
#define HI_EBUSY                (HI_ERRNO_COMMON_BASE + 0xD)  /* 资源正忙碌，例如被加锁 */
#define HI_EIDLE                (HI_ERRNO_COMMON_BASE + 0xE)  /* 资源正空闲 */
#define HI_EFULL                (HI_ERRNO_COMMON_BASE + 0xF)  /* 满，某种容器中已经被充满 */
#define HI_EEMPTY               (HI_ERRNO_COMMON_BASE + 0x10) /* 空，某种容器中是空的 */
#define HI_EUNDERFLOW           (HI_ERRNO_COMMON_BASE + 0x11) /* 下溢，某种容器中的容量已经下降到下溢水线之下 */
#define HI_EOVERFLOW            (HI_ERRNO_COMMON_BASE + 0x12) /* 上溢，某种容器中的容量上升到上溢水线之上 */
#define HI_EACCES               (HI_ERRNO_COMMON_BASE + 0x13) /* 权限错误，例如没有权限、密码错误等等 */
#define HI_EINTR                (HI_ERRNO_COMMON_BASE + 0x14) /* 操作未完成，已经中断 */
#define HI_ECONTINUE            (HI_ERRNO_COMMON_BASE + 0x15) /* 操作未完成，仍在继续 */
#define HI_EOVER                (HI_ERRNO_COMMON_BASE + 0x16) /* 操作完成，已没有后续的操作对象 */
#define HI_ERRNO_COMMON_BUTTOM  (HI_ERRNO_COMMON_BASE + 0x17) /* 已定义的错误号个数 */
#define HI_ENOSYS               (HI_ERRNO_COMMON_BASE + 0x18) /* 系统不支持 */

//[2011-4-12 15:42:14] 
#define HI_ERROR_NO_FINDDEV		(HI_ERRNO_COMMON_BASE + 0x80)	//没有发现设备
#define HI_ERROR_NO_FINDFILE	(HI_ERRNO_COMMON_BASE + 0x81)	//没有发现文件
#define HI_ERROR_FILE_CRC		(HI_ERRNO_COMMON_BASE + 0x82)	//文件CRC校验错
#define HI_ERROR_FILE_READ		(HI_ERRNO_COMMON_BASE + 0x83)	//文件读错误
/*************************************************************************
  模块自定义错误码基
*************************************************************************/
#define HI_ERRNO_BASE           (-HI_ERRNO_COMMON_BASE + HI_ERRNO_COMMON_COUNT)

#define HI_ERR_AUTHMNG_PARAM    (HI_ERRNO_COMMON_BASE + 0x10000)	/*参数输入错误*/
#define HI_ERR_AUTHMNG_NO_EXIST (HI_ERRNO_COMMON_BASE + 0x10001)	/*用户不存在*/
#define HI_ERR_AUTHMNG_PASSWORD (HI_ERRNO_COMMON_BASE + 0x10002)	/*用户密码错误*/

#define HI_S2_ERR_APPID         (0x80UL + 0x38UL)

/* 定义4个等级*/
typedef enum hiS2_ERRLEVEL_E
{
    HI_S2_ERRLEVEL_DEBUG = 0,   /* debug-level                                  */
		HI_S2_ERRLEVEL_INFO,        /* informational                                */
		HI_S2_ERRLEVEL_ERROR,       /* error conditions                             */
		HI_S2_ERRLEVEL_FATAL,       /* just for compatibility with previous version */
		HI_S2_ERRLEVEL_BUTT
} HI_S2_ERRLEVEL_E;

/* ==为保持兼容==*/
typedef enum hiSVR_ERRLEVEL_E
{
    HI_SVR_ERRLEVEL_DEBUG = 0,  /* debug-level                                  */
		HI_SVR_ERRLEVEL_INFO,       /* informational                                */
		HI_SVR_ERRLEVEL_ERROR,      /* error conditions                             */
		HI_SVR_ERRLEVEL_FATAL,      /* just for compatibility with previous version */
		HI_SVR_ERRLEVEL_BUTT
} HI_SVR_ERRLEVEL_E;


#define HI_S2_ERR( modid, level, errid) \
( (HI_S32)((HI_S2_ERR_APPID)<<24) | ((modid) << 16 ) | ((level)<<12) | ((errid) & 0xFFF) )

/* ==为保持兼容==*/
#define HI_SVR_ERR( modid, level, errid) \
( (HI_S32)((HI_S2_ERR_APPID)<<24) | ((modid) << 16 ) | ((level)<<12) | ((errid) & 0xFFF) )


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_COMMON_ERRNO_H__ */


