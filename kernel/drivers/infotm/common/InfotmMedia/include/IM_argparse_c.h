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
** v1.3.0	leo@2012/03/29: Add extern "C" for C++ calling.
**
*****************************************************************************/ 

#ifndef __IM_ARGPARSE_C_H__
#define __IM_ARGPARSE_C_H__

#ifdef __cplusplus
extern "C"
{
#endif

//
// argtable
// ./test -w=640 --height=480
//
typedef struct{
	IM_INT32	key;
	IM_TCHAR	shortOpt;	// -x, 0 indicate no shortOpt.
	IM_TCHAR	*longOpt;	// --xxx, IM_NULL indicate no longOpt.
	IM_TCHAR	*desc;	// will display in "help".
}argp_key_table_entry_t;

IM_RET argp_get_key_int(IN argp_key_table_entry_t *kte, OUT IM_INT32 *val, IN IM_INT32 argc, IN IM_TCHAR *argv[]);
IM_RET argp_get_key_string(IN argp_key_table_entry_t *kte, OUT IM_TCHAR **str, IN IM_INT32 argc, IN IM_TCHAR *argv[]);

#ifdef __cplusplus
}
#endif

#endif	// __IM_ARGPARSE_C_H__

