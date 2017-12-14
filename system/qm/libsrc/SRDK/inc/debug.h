/*************************************************
  Copyright (C), 2007-2099,SZ-STREAMING Tech. Co., Ltd.
  File name:
	rmdebug.h
  Author:
  	杨学政
  Date:
  Description:
*************************************************/
#ifndef _DEBUG_H__
#define _DEBUG_H__
#include <stdio.h>
#include <stdlib.h>

#define OUTPUT_KEY_INFO/*关键性的打印信息*/
#define OUTPUT_ADDR_INFO/*打印文件名，函数名，行号信息*/
#define OUTPUT_DEBUG_INFO
#define OUTPUT_OSD_DEBUG
#define OUTPUT_GUI_DEBUG
#define OUTPUT_GUIPARSE_INFO
#define OUTPUT_ENCODER_INFO

#if defined(OUTPUT_GUIPARSE_INFO)
	#define GUIPAR_INFO(info) do{printf info;}while(0)
#else
	#define GUIPAR_INFO(info) do{}while(0)
#endif

#if defined(OUTPUT_GUI_DEBUG)
	#define GUI_DEBUG(info) do{printf info;}while(0)
#else
	#define GUI_DEBUG(info) do{}while(0)
#endif


#if defined(OUTPUT_OSD_DEBUG)
	#define OSD_DEBUG(info) do{printf info;}while(0)
#else
	#define OSD_DEBUG(info) do{}while(0)
#endif


#if defined(OUTPUT_KEY_INFO)
	#define KEY_INFO(info) do{printf info;}while(0)
#else
	#define KEY_INFO(info) do{}while(0)
#endif

#if defined(OUTPUT_DEBUG_INFO)
//	#define DEBUG_INFO(info) do{printf info;}while(0)
	#define DEBUG_INFO(str, arg...)do{printf("FILE:%s  FUNCTION:%s LINE:%d "str, __FILE__, __FUNCTION__, __LINE__, ##arg);}while(0)
#else
	#define DEBUG_INFO(info) do{}while(0)
#endif


#if defined(OUTPUT_ENCODER_INFO)
	#define ENCODER_INFO(info) do{printf info;}while(0)
#else
	#define ENCODER_INFO(info) do{}while(0)
#endif



#if defined(OUTPUT_ADDR_INFO)
	#define ADDR_INFO do{printf("%s(%s):%d\n", __func__, __FILE__, __LINE__);}while(0)
#else
	#define ADDR_INFO do{}while(0)
#endif


#endif/* _RMDEBUG_H__ */


