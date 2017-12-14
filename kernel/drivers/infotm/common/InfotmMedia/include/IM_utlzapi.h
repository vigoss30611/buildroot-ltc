/***************************************************************************** 
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** Revision History: 
** ----------------- 
** 1.0.1	leo@2012/05/04: first commit.
**
*****************************************************************************/
 
#ifndef __IM_UTLZAPI_H__
#define __IM_UTLZAPI_H__


#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WINCE_)
#ifdef UTLZ_EXPORTS
	#define UTLZ_API		__declspec(dllexport)	/* For dll lib */
#else
	#define UTLZ_API		__declspec(dllimport)	/* For application */
#endif
#else
	#define UTLZ_API
#endif	
//#############################################################################
//
// handle(instance).
//
typedef void *	UTLZINST;	// the handle of the "utilization statistic".

//
// supported module.
// please keep the order from 0 to UTLZ_MODULE_MAX - 1.
//
#define UTLZ_MODULE_G2D		(0)
#define UTLZ_MODULE_PP		(1)
#define UTLZ_MODULE_VDEC	(2)
#define UTLZ_MODULE_VENC	(3)
#define UTLZ_MODULE_G3D		(4)

#define UTLZ_MODULE_MAX		(5)


UTLZ_API IM_UINT32 utlz_version(OUT IM_TCHAR *verString);

UTLZ_API IM_RET utlz_open(OUT UTLZINST *utlz);
UTLZ_API IM_RET utlz_close(IN UTLZINST utlz);

/**
 * function: get the module utilizaton coefficient.
 * params: module, UTLZ_MODULE_xxx.
 * 	coeffi, the coefficient(percent).
 * return: IM_RET_OK succeed, else failed.
 */
UTLZ_API IM_RET utlz_get_coefficient(IN UTLZINST utlz, IN IM_INT32 module, OUT IM_INT32 *coeffi);



//#############################################################################
#ifdef __cplusplus
}
#endif

#endif	// __IM_UTLZAPI_H__


