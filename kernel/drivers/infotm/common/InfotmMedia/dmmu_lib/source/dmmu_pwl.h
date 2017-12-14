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
** v1.1.0	leo@2012/03/17: first commit.
** v1.2.2	leo@2012/04/20: add dmmupwl_flush_cache() function.
** v1.2.3   leo@2013/11/02: add support for imapx900.
** v3.0.0   leo@2013/11/14: add dmmupwl_dump() interface, and move DMMU regbase
**              definations to dmmu_pwl_linux.c.
** v4.0.0   leo@2013/12/23: add dmmupwl_strcpy().
**
*****************************************************************************/ 

#ifndef __DMMU_PWL_H__
#define __DMMU_PWL_H__


IM_RET dmmupwl_init(void);
IM_RET dmmupwl_deinit(void);

void *dmmupwl_malloc(IM_INT32 size);
void dmmupwl_free(void *p);
void *dmmupwl_memcpy(void *dst, void *src, IM_INT32 size);
void *dmmupwl_memset(void *p, IM_CHAR c, IM_INT32 size);
IM_TCHAR *dmmupwl_strcpy(IM_TCHAR *dst, IM_TCHAR *src);
IM_RET dmmupwl_alloc_linear_page_align_memory(OUT IM_Buffer *buffer);	// must noncache.
IM_RET dmmupwl_free_linear_page_align_memory(IN IM_Buffer *buffer);
void dmmupwl_flush_cache(void *virAddr, IM_UINT32 phyAddr, IM_INT32 size);

IM_RET dmmupwl_write_reg(IM_INT32 devid, IM_UINT32 addr, IM_UINT32 val);
IM_UINT32 dmmupwl_read_reg(IM_INT32 devid, IM_UINT32 addr);
IM_RET dmmupwl_write_regbit(IM_INT32 devid, IM_UINT32 addr, IM_INT32 bit, IM_INT32 width, IM_UINT32 val);

void dmmupwl_udelay(IM_INT32 us);

IM_RET dmmupwl_mmu_init(IM_INT32 devid);
IM_RET dmmupwl_mmu_deinit(IM_INT32 devid);

IM_INT32 dmmupwl_dump(IM_TCHAR *buffer, IM_INT32 size);

#endif	// __DMMU_PWL_H__

