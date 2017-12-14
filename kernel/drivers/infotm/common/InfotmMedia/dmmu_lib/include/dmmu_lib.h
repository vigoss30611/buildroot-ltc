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
** v1.2.2	leo@2012/04/13: define DMMU_PAGE_SIZE and DMMU_PAGE_SHIFT.
** v3.0.0   leo@2013/11/14: add dmmulib_dump() interface.
** v4.0.0   leo@2013/12/23: add dmmulib_version() interface.
**
*****************************************************************************/ 

#ifndef __DMMU_LIB_H__
#define __DMMU_LIB_H__

//
//
//
#define DMMU_PAGE_SHIFT		(12)
#define DMMU_PAGE_SIZE		(4096)

//
// dmmulib_pagetable
//
typedef void *		dmmu_pagetable_t;
typedef IM_UINT32	dmmu_devaddr_t;	// it's ensured 4KB aligned.

typedef struct{
	IM_UINT32		pageNum;
	IM_UINT32		*pageList;
}dmmu_pt_map_t;


IM_UINT32 dmmulib_version(OUT IM_TCHAR *verString);
IM_RET dmmulib_pagetable_init(OUT dmmu_pagetable_t *ppt, OUT IM_UINT32 *dtablePhyBase);
IM_RET dmmulib_pagetable_deinit(IN dmmu_pagetable_t pt);
IM_RET dmmulib_pagetable_map(IN dmmu_pagetable_t pt, IN dmmu_pt_map_t *ptmap, OUT dmmu_devaddr_t *devAddr);
IM_RET dmmulib_pagetable_unmap(IN dmmu_pagetable_t pt, IN dmmu_devaddr_t devAddr, IN IM_INT32 pageNum);

//
// dmmulib_mmu
//
IM_RET dmmulib_mmu_raw_reset(IM_UINT32 devid);
IM_RET dmmulib_mmu_init(IM_UINT32 devid);
IM_RET dmmulib_mmu_deinit(IM_UINT32 devid);
IM_RET dmmulib_mmu_enable_paging(IM_UINT32 devid);
IM_RET dmmulib_mmu_disable_paging(IM_UINT32 devid);
IM_RET dmmulib_mmu_enable_stall(IM_UINT32 devid);
IM_RET dmmulib_mmu_disable_stall(IM_UINT32 devid);
IM_RET dmmulib_mmu_set_dtable(IM_UINT32 devid, IM_UINT32 dtablePhyBase);
IM_RET dmmulib_mmu_update_ptable(IM_UINT32 devid, dmmu_devaddr_t devAddr, IM_INT32 pageNum);

IM_INT32 dmmulib_dump(IM_TCHAR *buffer, IM_INT32 size);

#endif	// __DMMU_LIB_H__

