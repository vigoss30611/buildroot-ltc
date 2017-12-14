/***************************************************************************** 
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
**      
** Revision History: 
** ----------------- 
** v1.0.1	leo@2012/03/15: first commit.
**
*****************************************************************************/ 

#ifndef __IM_TYPES_H__
#define __IM_TYPES_H__

#if (TARGET_SYSTEM == FS_WINCE)
	#include "windows.h"

	#define IM_INT8		char
	#define IM_INT16	short
	#define IM_INT32	int
	#define IM_INT64	LONGLONG
	#define IM_UINT8	unsigned char
	#define IM_UINT16	unsigned short
	#define IM_UINT32	unsigned int
	#define IM_ULONG	unsigned long
	#define IM_LONG		long
	
	#define IM_FLOAT	float
	#define IM_DOUBLE	double
	#define IM_BOOL		BOOL
	#define IM_TRUE		TRUE
	#define IM_FALSE	FALSE
	#define IM_NULL		NULL
	#define IM_STR		TEXT
	#define IM_CHAR		char
	#define IM_UCHAR	unsigned char
	#define IM_TCHAR	TCHAR
	#define IM_FILE		FILE

	#define IM_INLINE	
#elif (TARGET_SYSTEM == FS_ANDROID)
	#include <pthread.h>
	#include <stdbool.h>
	#include <stdint.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <errno.h>
	#include <signal.h>
	#include <unistd.h>
	#include <time.h>
	#include <sys/time.h>
	#include <sys/resource.h>
	#include <utils/Log.h>
	#include <linux/fb.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>
	#define IM_INT8		char
	#define IM_INT16	short
	#define IM_INT32	int
	#define IM_INT64	int64_t
	#define IM_UINT8	unsigned char
	#define IM_UINT16	unsigned short
	#define IM_UINT32	unsigned int
	#define IM_ULONG	unsigned long
	#define IM_LONG		long
	
	#define IM_FLOAT	float
	#define IM_DOUBLE	double
	#define IM_BOOL		bool
	#define IM_TRUE		true
	#define IM_FALSE	false
	#define IM_NULL		NULL
	#define IM_STR(str)	str
	#define IM_CHAR		char
	#define IM_UCHAR	unsigned char
	#define IM_TCHAR	char
	#define IM_FILE		FILE

	#define IM_INLINE	
#elif (TARGET_SYSTEM == FS_LINUX)
	#include <pthread.h>
	#include <stdbool.h>
	#include <stdint.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <errno.h>
	#include <signal.h>
	#include <unistd.h>
	#include <time.h>
	#include <sys/time.h>
	#include <sys/resource.h>
	#include <utils/Log.h>
	#include <linux/fb.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>

	typedef char		IM_INT8;
	typedef short		IM_INT16;
	typedef int		IM_INT32;
	typedef int64_t		IM_INT64;
	typedef unsigned char	IM_UINT8;
	typedef unsigned short	IM_UINT16;
	typedef unsigned int	IM_UINT32;
	typedef unsigned long	IM_ULONG;
	typedef long		IM_LONG;
	
	typedef float		IM_FLOAT;
	typedef double		IM_DOUBLE;
	typedef bool		IM_BOOL;
	#define IM_TRUE		true
	#define IM_FALSE	false
	#define IM_NULL		NULL
	#define IM_STR(str)	str
	typedef char		IM_CHAR;
	typedef unsigned char	IM_UCHAR;
	typedef char		IM_TCHAR;
	typedef FILE		IM_FILE;

	#define IM_INLINE	
#elif (TARGET_SYSTEM == KERNEL_LINUX)
	#include <linux/kernel.h>
	#include <linux/types.h>
	#include <linux/module.h>
	#include <linux/init.h>
	#include <linux/interrupt.h>
	#include <linux/device.h>
	#include <linux/miscdevice.h>
	#include <linux/platform_device.h>
	#include <linux/io.h>
	#include <linux/poll.h>
	#include <linux/wait.h>
	#include <linux/delay.h>
	#include <linux/errno.h>
	#include <linux/spinlock.h>
	#include <linux/mutex.h>
	#include <linux/mman.h>
	#include <linux/slab.h>

	#define IM_INT8		char
	#define IM_INT16	short
	#define IM_INT32	int
	#define IM_INT64	int64_t
	#define IM_UINT8	unsigned char
	#define IM_UINT16	unsigned short
	#define IM_UINT32	unsigned int
	#define IM_ULONG	unsigned long
	#define IM_LONG		long
	
	#define IM_FLOAT	float
	#define IM_DOUBLE	double
	#define IM_BOOL		bool
	#define IM_TRUE		true
	#define IM_FALSE	false
	#define IM_NULL		NULL
	#define IM_STR(str)	str
	#define IM_CHAR		char
	#define IM_UCHAR	unsigned char
	#define IM_TCHAR	char
	#define IM_FILE		FILE

	#define IM_INLINE	inline
#endif

// function param's type, only for view code.
#define IN
#define OUT
#define INOUT

// common return value.
typedef IM_INT32 IM_RET;
enum{
	// common, range [-99, 0], [0, 99]
	IM_RET_OK = 0,
	IM_RET_FALSE,
	IM_RET_RELAX,
	IM_RET_EOS,
	IM_RET_RETRY,
	
	IM_RET_FAILED = -1,
	IM_RET_TIMEOUT = -2,
	IM_RET_NOMEMORY = -3,
	IM_RET_INVALID_PARAMETER = -4,
	IM_RET_OVERFLOW = -5,
	IM_RET_NOTSUPPORT = -6,
	IM_RET_NORESOURCE = -7,
	IM_RET_NOITEM = -8,

	// Jpeg enc, range [-100, -200], [100, 200]
	IM_RET_JENC_SLICED_OK = 100,

	// audio dec, range [-200, -300], [200, 300]
	IM_RET_ADEC_OK = IM_RET_OK,
	IM_RET_ADEC_DECODING = 200,
	
	IM_RET_ADEC_FAIL = IM_RET_FAILED,
	IM_RET_ADEC_INIT_FAIL = -200,
	IM_RET_ADEC_DEINIT_FAIL = -201,
	IM_RET_ADEC_FLUSH_FAIL = -202,
	IM_RET_ADEC_NOT_INITIALIZED = -203,
	IM_RET_ADEC_INVALID_PARAMETERS = -204,
	IM_RET_ADEC_MEM_FAIL = -205,
	IM_RET_ADEC_UNSUPPORT_INPUT_TYPE = -206,
	IM_RET_ADEC_UNSUPPORT_OUTPUT_TYPE = -207,
	IM_RET_ADEC_UNSUPPORT_TRANSFORM = -208,
	IM_RET_ADEC_BITSTREAM_ERROR = -209,

	
	//video dec range [-400,-300],[300,400]
	IM_RET_VDEC_OK = IM_RET_OK,
	IM_RET_VDEC_DECODING = 300,
	IM_RET_VDEC_PROCESSED= 301,
	IM_RET_VDEC_LACK_OUTBUFF = 302,
	IM_RET_VDEC_SKIP_FRAME = 303,
	IM_RET_VDEC_OUTPUT_TYPE_CHANGED = 304,
	IM_RET_VDEC_PROPERTY_CMD_NOT_EXECUTE = 305,
	IM_RET_VDEC_CHECK_INFOTM_EXTRA_DECODE = 306,
	IM_RET_VDEC_OUT_BUFFER_NEED_RECONFIG = 307,
	IM_RET_VDEC_NEW_SEGMENT_READY = 308,
	IM_RET_VDEC_HDRS_RDY = 309,
	IM_RET_VDEC_NO_PIC_OUT = 310,
	IM_RET_VDEC_MORE_PIC_OUT = 311,
	IM_RET_VDEC_MOSAIC_PIC_OUT = 312,
	IM_RET_VDEC_PIC_SKIPPED = 313,
	IM_RET_VDEC_DECINFO_CHANGED = 314,

	IM_RET_VDEC_FAIL = IM_RET_FAILED,
	IM_RET_VDEC_INIT_FAIL = -300,
	IM_RET_VDEC_DEINIT_FAIL = -301,
	IM_RET_VDEC_DEC_FAIL = -302,
	IM_RET_VDEC_FLUSH_FAIL = -303,
	IM_RET_VDEC_GET_HWDEC_RESOURCE_FAIL = -304,
	IM_RET_VDEC_RELEASE_HWDEC_RESOURCE_FAIL = -305,
	IM_RET_VDEC_INIT_HWDEC_FAIL = -306,
	IM_RET_VDEC_DEINIT_HWDEC_FAIL = -307,
	IM_RET_VDEC_FAIL_FOREVER = -308,
	IM_RET_VDEC_HW_TIME_OUT= -309,
	IM_RET_VDEC_NOT_INITIALIZED = -310,
	IM_RET_VDEC_INVALID_PARAMETERS= -311,
	IM_RET_VDEC_MEM_FAIL= -312,
	IM_RET_VDEC_RESOURCE_NOT_INIT = -313,
	IM_RET_VDEC_UNSUPPORT_INPUT_TYPE = -314,
	IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE = -315,
	IM_RET_VDEC_UNSUPPORT_OUTPUT_SIZE = -316,
	IM_RET_VDEC_UNSUPPORT_TRANSFORM = -317,
	IM_RET_VDEC_UNSUPPORT_STREAM = -318,
	IM_RET_VDEC_STRM_PROCESSED = -319,
	IM_RET_VDEC_STRM_ERROR = -320,

	//video enc range [-500,-400],[400,500]
	IM_RET_VENC_OK = IM_RET_OK,
	IM_RET_VENC_HDR_READY = 400,
	IM_RET_VENC_EOS_READY = 401,
	IM_RET_VENC_CMD_NOT_EXECUTE = 402,
	IM_RET_VENC_BUFFER_OVERFLOW = 403,


	IM_RET_VENC_FAIL = -400,
	IM_RET_VENC_NOT_INIT = -401,
	IM_RET_VENC_NOT_SUPPORT_TYPE = -402,
	IM_RET_VENC_INVALID_PARAMS = IM_RET_INVALID_PARAMETER,
	IM_RET_VENC_HW_INSTANCE_ERROR = -403,
	IM_RET_VENC_STREAM_HEAD_FAIL = -404,
	IM_RET_VENC_EOS_FAIL = -405,


};
#define IM_FAILED(ret)	((ret) < 0)?IM_TRUE:IM_FALSE


#endif	// __IM_TYPES_H__
