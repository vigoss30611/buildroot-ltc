/** 
************************************************************************** 
@file           img_types.h
 
@brief          Typedefs based on the basic IMG types
 
@copyright Imagination Technologies Ltd. All Rights Reserved. 
    
@license        <Strictly Confidential.> 
   No part of this software, either material or conceptual may be copied or 
   distributed, transmitted, transcribed, stored in a retrieval system or  
   translated into any human or computer language in any form by any means, 
   electronic, mechanical, manual or other-wise, or disclosed to third  
   parties without the express written permission of  
   Imagination Technologies Limited,  
   Unit 8, HomePark Industrial Estate, 
   King's Langley, Hertfordshire, 
   WD4 8LZ, U.K. 
**************************************************************************/

#ifndef __IMG_TYPES__
#define __IMG_TYPES__

#include "img_systypes.h" // system specific type definitions

typedef          IMG_WCHAR    *IMG_PWCHAR;

typedef	          IMG_INT8	*IMG_PINT8;
typedef	         IMG_INT16	*IMG_PINT16;
typedef	         IMG_INT32	*IMG_PINT32;
typedef	         IMG_INT64	*IMG_PINT64;

//typedef	    unsigned int	IMG_UINT;
typedef	 IMG_UINT8	*IMG_PUINT8;
typedef	IMG_UINT16	*IMG_PUINT16;
typedef	IMG_UINT32	*IMG_PUINT32;
typedef	IMG_UINT64	*IMG_PUINT64;

/*
 * Typedefs of void are synonymous with the void keyword in C, 
 * but not in C++. In order to support the use of IMG_VOID
 * in place of the void keyword to specify that a function takes no 
 * arguments, it must be a macro rather than a typedef.
 */
#define IMG_VOID void
typedef            void*    IMG_PVOID;
typedef            void*    IMG_HANDLE;
typedef        IMG_INT32    IMG_RESULT;

/*
 * integral types that are not system specific
 */
typedef             char    IMG_CHAR;
typedef	             int	IMG_INT;
typedef	    unsigned int	IMG_UINT;
typedef              int    IMG_BOOL;

/*
 * boolean
 */
//#ifndef __cplusplus
#define IMG_NULL NULL
//#else
//#define IMG_NULL 0
//#endif

typedef IMG_UINT8 IMG_BOOL8;

#define	IMG_FALSE             	0	/* IMG_FALSE is known to be zero */
#define	IMG_TRUE            	1   /* 1 so it's defined and it is not 0 */

/*
 * floating point
 */
typedef float IMG_FLOAT;
typedef double IMG_DOUBLE;


#define MAX_RES (5)

typedef struct _res_t
{
	int W;
	int H;
}res_t;

typedef struct _reslist_t
{
	res_t res[MAX_RES];
	int count;
}reslist_t;

#define INFOTM_LSH_FUNC
#define INFOTM_SENSOR_OTP_DATA_MODULE // added @20170110 to support read sensor otp calibration data(lens shading & AWB)

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
    //#define OTP_DBG_LOG
    #define INFOTM_LSH_CURVE_FITTING // added by linyun.xiong @2017-03-08
    #define INFOTM_LSH_AUTO_RETRY
    //#define INFOTM_LSH_CHANGE_BY_DUAL_RGB_DIFF
#endif

//#define INFOTM_SKIP_OTP_DATA
//#define INFOTM_CURVE_FITTING_ALGO
#define INFOTM_SUPPORT_JFIF

//#define INFOTM_SUPPORT_CCM_FACTOR
#define INFOTM_AUTO_CHANGE_CAPTURE_CCM

#ifdef INFOTM_SUPPORT_JFIF
#define INFOTM_SUPPORT_SWTICH_R2Y_MARTIX
#endif

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
//#define LIPIN_SENSOR_OTPDATA_V1_2
#define INFOTM_E2PROM_METHOD

#ifdef INFOTM_E2PROM_METHOD
//#define INFOTM_EXCHANGE_DIRECTION
#endif

//#define SMOOTH_CENTER_RGB

#define LIPIN_SENSOR_OTPDATA_V1_3
#define LIPIN_SENSOR_OTPDATA_V1_4
#endif


//-----------------------------------------------------------------------------
// Feature option
#define INFOTM_ISP  //Don't remove
#ifdef INFOTM_ISP
#define INFOTM_ISP_RETRY
#define INFOTM_ISP_TUNING 
#define INFOTM_AWB_PATCH  // AWB patch
#define INFOTM_TIMESTAMP_MULTI_LINE //Logo and multiform line
#define INFOTM_AWB_PATCH_GET_CCT

#define INFOTM_ISP_MOTION_DETECT
#define INFOTM_SW_AWB_METHOD
/*Note: The infotm AWB module is only used by Q3F*/
#ifdef INFOTM_HW_AWB
#define INFOTM_HW_AWB_METHOD
#endif


#ifdef INFOTM_SW_AWB_METHOD
#define INFOTM_SW_AWB_USING_RGB_888_24
//#define INFOTM_SW_AWB_USING_RGB_888_32
	#if defined(INFOTM_SW_AWB_USING_RGB_888_24) && defined(INFOTM_SW_AWB_USING_RGB_888_32)
		#undef INFOTM_SW_AWB_USING_RGB_888_32
	#endif
	
#define ENABLE_SW_AWB_DBG_MSG_OVERLAY_TO_IMG
	#if defined(ENABLE_SW_AWB_DBG_MSG_OVERLAY_TO_IMG)&& !defined(INFOTM_TIMESTAMP_MULTI_LINE)
		#define INFOTM_TIMESTAMP_MULTI_LINE
	#endif
#endif //INFOTM_SW_AWB_METHOD

#if defined(INFOTM_HW_AWB_METHOD)
//#define INFOTM_HW_AWB_METHOD_DEBUG_MESSAGE
  #if defined(INFOTM_HW_AWB_METHOD_DEBUG_MESSAGE)
#define INFOTM_HW_AWB_METHOD_DEBUG_CNT 20
  #endif //INFOTM_HW_AWB_METHOD_DEBUG_MESSAGE
#endif //INFOTM_HW_AWB_METHOD

#define INFOTM_ENABLE_COLOR_MODE_CHANGE
#define INFOTM_ENABLE_GAIN_LEVEL_IDX
#define TNMC_WDR_SEGMENT_CNT    (8)
#define TNMC_WDR_MIN            (0.0)
#define TNMC_WDR_MAX            (5.0)
#define USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST

#endif //INFOTM_ISP

//-----------------------------------------------------------------------------
// Function option
//#define INFOTM_DISABLE_AE
//#define INFOTM_DISABLE_AWB
//#define INFOTM_DISABLE_AF
#define INFOTM_ENABLE_GLOBAL_TONE_MAPPER
#define INFOTM_ENABLE_LOCAL_TONE_MAPPER
//#define INFOTM_ENABLE_LENS_SHADING_CORRECTION
//#define INFOTM_ENABLE_GPIO_TOGGLE_FRAME_COUNT
#define INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
#define INFOTM_DISABLE_INTDG
//#define INFOTM_DISABLE_INTERRUPT_DBGFS
#define INFOTM_DISABLE_INT_START_OF_FRAME_RECEIVED
//#define INFOTM_DISABLE_INT_EN_ERROR_IGNORE
#define INFOTM_ENABLE_THREAD_INTERRUPT_LOOP


//-----------------------------------------------------------------------------
// Debug option
#define INFOTM_ENABLE_ISP_DEBUG				//ISP debug message
//#define INFOTM_ENABLE_WARNING_DEBUG		//Warning or information debug message
//#define INFOTM_ENABLE_AE_DEBUG 			//AE debug message
//#define INFOTM_ENABLE_AWB_DEBUG 			//AWB debug message
//#define INFOTM_ENABLE_HISTOGRAM_DEBUG		//HISTOGRAM debug message
//#define INFOTM_ENABLE_LSH_DEBUG 			//LSH debug message
//#define INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG //TNM Equalization debug message
//#define INFOTM_ISP_EXPOSURE_GAIN_DEBUG
//#define INFOTM_GAIN_LEVEL_DEBUG_MESSAGE
//#define INFOTM_CMC_DEBUG_MESSAGE


//-----------------------------------------------------------------------------
// Parameter option
#ifdef INFOTM_ISP_TUNING
#define SCAN_ENV_BRIGHTNESS
#endif //INFOTM_ISP_TUNING


#endif /* __IMG_TYPES__ */
