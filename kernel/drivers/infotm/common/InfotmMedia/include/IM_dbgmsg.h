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
** v2.0.3	leo@2013/12/23: support android4.4.
**
*****************************************************************************/ 

#ifndef __IM_DBGMSG_H__
#define __IM_DBGMSG_H__

#ifdef IM_DEBUG	//=============================================================
/*----------------------------------------------------------------------------*/
#if (TARGET_SYSTEM == FS_WINCE)
	#define _IM_FUNC_				__FUNCTION__
	#define _IM_LINE_				__LINE__
	#define _IM_FILE_				__FILE__
	#define IM_ASSERT(exp)		if(!(exp))	\
			{RETAILMSG(1, (TEXT("ABORT: %s %s:%d"), TEXT(__FILE__), TEXT(__FUNCTION__), __LINE__)); DebugBreak();}	//ASSERT(exp)
	#define IM_DBGTAG(nbr)		RETAILMSG(1, (TEXT("@%d\r\n"), nbr))
	#define IM_INFOMSG(str)		RETAILMSG(DBGINFO, (TEXT("%s"), TEXT(INFOHEAD)));	\
							RETAILMSG(DBGINFO, str);	\
							RETAILMSG(DBGINFO, (TEXT("\r\n")));
	#define IM_WARNMSG(str)	RETAILMSG(DBGWARN, (TEXT("%s"), TEXT(WARNHEAD)));	\
							RETAILMSG(DBGWARN, str);	\
							RETAILMSG(DBGWARN, (TEXT("\r\n")));
	#define IM_TIPMSG(str)		RETAILMSG(DBGTIP, (TEXT("%s"), TEXT(TIPHEAD)));	\
							RETAILMSG(DBGTIP, str);	\
							RETAILMSG(DBGTIP, (TEXT("\r\n")));
	#define IM_ERRMSG(str)		RETAILMSG(DBGERR, (TEXT("%s file %s, line %d "), TEXT(ERRHEAD), TEXT(__FILE__), __LINE__));	\
							RETAILMSG(DBGERR, str);	\
							RETAILMSG(DBGERR, (TEXT("\r\n")));
/*----------------------------------------------------------------------------*/
#elif (TARGET_SYSTEM == FS_ANDROID)

    #if (TS_VER_MAJOR == 4) && (TS_VER_MINOR == 1)
        #define LOGE    ALOGE
    #elif (TS_VER_MAJOR == 4) && ((TS_VER_MINOR == 3) || (TS_VER_MINOR == 4))
        #define LOGE    ALOGE
    #endif

	#define _IM_FUNC_				__FUNCTION__
	#define _IM_FILE_				__FILE__
	#define _IM_LINE_				__LINE__
	#define IM_ASSERT(exp)			if(!(exp)){LOGE("ABORT: %s %s:%d", __FILE__, __FUNCTION__, __LINE__); abort();}
	#define IM_DBGTAG(nbr)			LOGE("@%d", nbr)
	#define IM_INFOMSG(str)		if(DBGINFO) {LOGE(INFOHEAD); LOGE str;}
	#define IM_WARNMSG(str)		if(DBGWARN) {LOGE(WARNHEAD); LOGE str;}
	#define IM_TIPMSG(str)		if(DBGTIP) {LOGE(TIPHEAD); LOGE str;}
	#define IM_ERRMSG(str)		if(DBGERR) {LOGE(ERRHEAD); LOGE("file %s, line %d ", _IM_FILE_, _IM_LINE_); LOGE str;}
/*----------------------------------------------------------------------------*/
#elif (TARGET_SYSTEM == FS_LINUX)
	#define _IM_FUNC_				__FUNCTION__
	#define _IM_FILE_				__FILE__
	#define _IM_LINE_				__LINE__
	#define IM_ASSERT(exp)		if(!(exp)){printf("ABORT: %s %s:%d", __FILE__, __FUNCTION__, __LINE__); abort();}
	#define IM_DBGTAG(nbr)		printf("@%d", nbr);
	#define IM_INFOMSG(str)		if(DBGINFO) {printf(INFOHEAD); printf str; printf("\r\n");}
	#define IM_WARNMSG(str)		if(DBGWARN) {printf(WARNHEAD); printf str; printf("\r\n");}
	#define IM_TIPMSG(str)		if(DBGTIP) {printf(TIPHEAD); printf str; printf("\r\n");}
	#define IM_ERRMSG(str)		if(DBGERR) {printf(ERRHEAD); printf str; printf("\r\n");}
/*----------------------------------------------------------------------------*/
#elif (TARGET_SYSTEM == RVDS)
	#define _IM_FUNC_			__FUNCTION__
	#define _IM_LINE_				__LINE__
	#define _IM_FILE_				__FILE__
	#define IM_ASSERT(exp)			if(!(exp)){log_printf("ABORT: %s %s:%d\r\n", __FILE__, __FUNCTION__, __LINE__); *(unsigned int *)0 = 0;}
	#define IM_DBGTAG(nbr)			log_printf("@%d\r\n", nbr)
	#define IM_INFOMSG(str)			if(DBGINFO) {log_printf("%s", INFOHEAD); log_printf str; log_printf("\r\n");}
	#define IM_WARNMSG(str)			if(DBGWARN) {log_printf("%s", WARNHEAD); log_printf str; log_printf("\r\n");}
	#define IM_TIPMSG(str)			if(DBGTIP) {log_printf("%s", TIPHEAD); log_printf str; log_printf("\r\n");}
	#define IM_ERRMSG(str)			if(DBGERR) {log_printf("%s", ERRHEAD); log_printf("file %s, line %d ", __FILE__, __LINE__); log_printf str; log_printf("\r\n");}
/*----------------------------------------------------------------------------*/
#elif (TARGET_SYSTEM == KERNEL_LINUX)
	#define _IM_FUNC_				__FUNCTION__
	#define _IM_LINE_				__LINE__
	#define _IM_FILE_				__FILE__

	#define IM_INFOMSG(str) 		if(DBGINFO) { printk("%s", INFOHEAD); printk str; printk("\r\n");}
	#define IM_WARNMSG(str)			if(DBGWARN) { printk("%s", WARNHEAD); printk str; printk("\r\n");}
	#define IM_ERRMSG(str)			if(DBGERR) 	{ printk("%s", ERRHEAD); printk str; printk("\r\n");}
	#define IM_TIPMSG(str)			if(DBGTIP) 	{ printk("%s", TIPHEAD); printk str; printk("\r\n");}
	#define IM_ASSERT(exp)			if(!(exp)){printk("ABORT: %s %s:%d\r\n", __FILE__, __FUNCTION__, __LINE__); *(unsigned int *)0 = 0;}
	#define IM_DBGTAG(nbr)			printk("@%d\r\n", nbr);
/*----------------------------------------------------------------------------*/
#endif	//=====================================================================
#else	//=====================================================================
/*----------------------------------------------------------------------------*/
#if (TARGET_SYSTEM == FS_WINCE)
	#define _IM_FUNC_				__FUNCTION__
	#define _IM_LINE_				__LINE__
	#define _IM_FILE_				__FILE__
	#define IM_ASSERT(exp)			if(!(exp)){}
	#define IM_DBGTAG(nbr)
	#define IM_INFOMSG(str)	
	#define IM_WARNMSG(str)
	#define IM_TIPMSG(str)
	#define IM_ERRMSG(str)
/*----------------------------------------------------------------------------*/
#elif (TARGET_SYSTEM == FS_ANDROID)
	#define _IM_FUNC_				__FUNCTION__
	#define _IM_LINE_				__LINE__
	#define _IM_FILE_				__FILE__
	#define IM_ASSERT(exp)			if(!(exp)){}
	#define IM_DBGTAG(nbr)
	#define IM_INFOMSG(str)	
	#define IM_WARNMSG(str)
	#define IM_TIPMSG(str)
	#define IM_ERRMSG(str)
/*----------------------------------------------------------------------------*/
#elif (TARGET_SYSTEM == FS_LINUX)
	#define _IM_FUNC_				__FUNCTION__
	#define _IM_LINE_				__LINE__
	#define _IM_FILE_				__FILE__
	#define IM_ASSERT(exp)			if(!(exp)){}
	#define IM_DBGTAG(nbr)
	#define IM_INFOMSG(str)	
	#define IM_WARNMSG(str)
	#define IM_TIPMSG(str)
	#define IM_ERRMSG(str)
/*----------------------------------------------------------------------------*/
#elif (TARGET_SYSTEM == RVDS)
	#define _IM_FUNC_		__FUNCTION__
	#define _IM_LINE_			__LINE__
	#define _IM_FILE_			__FILE__
	#define IM_ASSERT(exp)			if(!(exp)){}
	#define IM_DBGTAG(nbr)
	#define IM_INFOMSG(str)
	#define IM_WARNMSG(str)
	#define IM_TIPMSG(str)
	#define IM_ERRMSG(str)
/*----------------------------------------------------------------------------*/
#elif (TARGET_SYSTEM == KERNEL_LINUX)
	#define _IM_FUNC_				__FUNCTION__
	#define _IM_LINE_				__LINE__
	#define _IM_FILE_				__FILE__

	#define IM_INFOMSG(str)
	#define IM_WARNMSG(str)
	#define IM_ERRMSG(str)
	#define IM_TIPMSG(str)
	#define IM_ASSERT(exp)			if(!(exp)){}
	#define IM_DBGTAG(nbr)
/*----------------------------------------------------------------------------*/
#endif	//=====================================================================
#endif	// IM_DEBUG

#endif	// __IM_DBGMSG_H__
