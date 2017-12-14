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
** v1.2.1	leo@2012/03/28: use IM_ERRMSG() to dmmulib_mmu_xxx() error log.
** v1.2.2	leo@2012/04/20: add dmmupwl_flush_cache() to sync the access of
**				mmu table from device-mmu.
** v2.0.0	leo@2012/09/11: hide pgspace_get_devaddr_pagenum() in compile.
** v3.0.0   leo@2013/11/14: add dmmulib_dump() function.
** v4.0.0   leo@2013/12/23: add dmmulib_version() function.
** v4.0.1   leo@2014/01/17: 1. printing version messing in dmmulib_pagetable_init().
**                          2. zap entire cache if defined DMMU_ZAP_ENTIRE_PAGES, and now defined in default.
**                          3. flush dtable and ptable memory when unmapping pagetable.
**                          4. some debug message modifications.
**
*****************************************************************************/ 

#include <InfotmMedia.h>
#include <IM_devmmuapi.h>
#include <dmmu_lib.h>
#include <dmmu_pwl.h>


#define DBGINFO		0
#define DBGWARN		1
#define DBGERR		1
#define DBGTIP		1

#define INFOHEAD	"DMMULIB_I:"
#define WARNHEAD	"DMMULIB_W:"
#define ERRHEAD		"DMMULIB_E:"
#define TIPHEAD		"DMMULIB_T:"

//
// mmu address bits:[31:22]DTE index, [21:12]PTE index, [11:0]Page offset. 
//
#define DMMU_DTE_BIT_OFFSET	22
#define DMMU_PTE_BIT_OFFSET	12
#define DMMU_PAGE_BIT_OFFSET	0

#define DMMU_DTE_BITS		10
#define DMMU_PTE_BITS		10
#define DMMU_PAGE_BITS		12

#define DMMU_DTE_ENTRIES	1024
#define DMMU_PTE_ENTRIES	1024

// devAddr from 0x80000000 to 0xffffffff, total 2GB.
#define PAGE_NUM_MAX		0x80000	// 2GB / 4KB 
#define PAGE_SPACE_BASE		(0x80000000 >> 12)	// caution: don't use 0x0 as the base, for more app use 0 to check invalid address.	

#define PAGE_TO_DEVADDR(pg)		(IM_UINT32)((pg) << DMMU_PAGE_BITS)
#define DEVADDR_TO_PAGE(devAddr)	(IM_UINT32)((devAddr) >> DMMU_PAGE_BITS)
#define DEVADDR_GET_DT_ENTRY(devAddr)	(((devAddr)>>DMMU_DTE_BIT_OFFSET) & 0x03FF)
#define DEVADDR_GET_PT_ENTRY(devAddr)	(((devAddr)>>DMMU_PTE_BIT_OFFSET) & 0x03FF)

typedef struct{
	IM_UINT32		base;
	IM_UINT32		num;
	IM_BOOL			idle;
}page_block_t;

typedef struct{
	IM_UINT32		totalFreePageNum;
	im_mempool_handle_t	mpl;
	im_list_handle_t	pgblkLst;	// list of mem_block_t.
}page_space_t;

static IM_RET pgspace_deinit(page_space_t *ps);

static void pgspace_print_list(page_space_t *ps)
{
#if DBGINFO
	page_block_t *pgblk;

	pgblk = (page_block_t *)im_list_begin(ps->pgblkLst);
	IM_ASSERT(pgblk != IM_NULL);
	while(pgblk != IM_NULL){
		IM_INFOMSG((IM_STR("base:0x%5x, num=0x%5x, idle=%d"), pgblk->base, pgblk->num, pgblk->idle));
		pgblk = (page_block_t *)im_list_next(ps->pgblkLst);
	}
#endif
}

static IM_RET pgspace_init(page_space_t *ps, im_mempool_handle_t mpl)
{
	page_block_t pgblk;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(ps != IM_NULL);

	IM_ASSERT(PAGE_SPACE_BASE != 0);

	//
	dmmupwl_memset((void *)ps, 0, sizeof(page_space_t));
	ps->mpl = mpl;

	ps->pgblkLst = im_list_init(sizeof(page_block_t), ps->mpl);
	if(ps->pgblkLst == IM_NULL){
		IM_ERRMSG((IM_STR("im_list_init() failed")));
		goto Fail;
	}
	pgblk.base = PAGE_SPACE_BASE;
	pgblk.num = PAGE_NUM_MAX;
	pgblk.idle = IM_TRUE;
	im_list_put_back(ps->pgblkLst, (void *)&pgblk);

	ps->totalFreePageNum = PAGE_NUM_MAX;
	return IM_RET_OK;
Fail:
	pgspace_deinit(ps);
	return IM_RET_FAILED;
}

static IM_RET pgspace_deinit(page_space_t *ps)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(ps != IM_NULL);
	
	if(ps->pgblkLst != IM_NULL){
		im_list_deinit(ps->pgblkLst);
	}
	dmmupwl_memset((void *)ps, 0, sizeof(page_space_t));

	return IM_RET_OK;
}

static IM_RET pgspace_alloc(page_space_t *ps, IM_UINT32 pageNum, dmmu_devaddr_t *devAddr)
{
	page_block_t *pgblk, newPgblk;	
	IM_INFOMSG((IM_STR("%s(pageNum=%d)"), IM_STR(_IM_FUNC_), pageNum));
	IM_ASSERT(ps != IM_NULL);

	//
	if(pageNum > ps->totalFreePageNum){
		IM_ERRMSG((IM_STR("no pgspace to alloc %d pages, totalFreePageNum is %d"), pageNum, ps->totalFreePageNum));
		return IM_RET_FAILED;
	}

	//
	pgblk = (page_block_t *)im_list_begin(ps->pgblkLst);
	IM_ASSERT(pgblk != IM_NULL);
	while(pgblk != IM_NULL){
		if((pgblk->idle == IM_TRUE) && (pgblk->num >= pageNum)){
			break;
		}
		pgblk = (page_block_t *)im_list_next(ps->pgblkLst);
	}

	if(pgblk == IM_NULL){
		IM_ERRMSG((IM_STR("no continuous pgspace to alloc %d pages"), pageNum));
		return IM_RET_FAILED;
	}

	//
	if(pgblk->num > pageNum){
		newPgblk.base = pgblk->base + pageNum;
		newPgblk.num = pgblk->num - pageNum;
		newPgblk.idle = IM_TRUE;
		im_list_insert(ps->pgblkLst, (void *)&newPgblk);

		pgblk->num = pageNum;
	}
	pgblk->idle = IM_FALSE;

	//
	*devAddr = PAGE_TO_DEVADDR(pgblk->base);

	//
	pgspace_print_list(ps);

	return IM_RET_OK;
}

/*
static IM_UINT32 pgspace_get_devaddr_pagenum(page_space_t *ps, dmmu_devaddr_t devAddr)
{
	page_block_t *pgblk;
	IM_UINT32 pgbase = DEVADDR_TO_PAGE(devAddr);
	IM_INFOMSG((IM_STR("%s(devAddr=0x%x)"), IM_STR(_IM_FUNC_), devAddr));

	pgblk = (page_block_t *)im_list_begin(ps->pgblkLst);
	IM_ASSERT(pgblk != IM_NULL);
	while(pgblk != IM_NULL){
		if(pgblk->base == pgbase){
			break;
		}
		pgblk = (page_block_t *)im_list_next(ps->pgblkLst);
	}

	IM_ASSERT(pgblk != IM_NULL);
	return pgblk->num;
}*/

static IM_RET pgspace_free(page_space_t *ps, dmmu_devaddr_t devAddr)
{
	page_block_t *prePgblk, *pgblk, *postPgblk;
	IM_UINT32 pgbase = DEVADDR_TO_PAGE(devAddr);

	IM_INFOMSG((IM_STR("%s(devAddr=0x%x)"), IM_STR(_IM_FUNC_), devAddr));
	IM_ASSERT(ps != IM_NULL);
	IM_ASSERT((devAddr & ((1<<DMMU_PAGE_BITS) - 1)) == 0);

	//
	pgblk = (page_block_t *)im_list_begin(ps->pgblkLst);
	IM_ASSERT(pgblk != IM_NULL);
	while(pgblk != IM_NULL){
		if(pgblk->base == pgbase){
			IM_ASSERT(pgblk->idle == IM_FALSE);
			break;
		}
		pgblk = (page_block_t *)im_list_next(ps->pgblkLst);
	}

	IM_ASSERT(pgblk != IM_NULL);
	if(pgblk == IM_NULL){
		IM_ERRMSG((IM_STR("not found this devAddr=0x%x, pgbase=%d"), devAddr, pgbase));
		return IM_RET_FAILED;
	}

	// first mark this blk is idle.
	pgblk->idle = IM_TRUE;

	// second connnect to pre and post block.
	prePgblk = (page_block_t *)im_list_prev(ps->pgblkLst);
	if(prePgblk != IM_NULL){
		im_list_next(ps->pgblkLst);	// go to pgblk and for then find the post.
	}
	postPgblk = (page_block_t *)im_list_next(ps->pgblkLst);

	if(prePgblk != IM_NULL){	// first connect to previous.
		if((prePgblk->idle == IM_TRUE)){
            IM_ASSERT(prePgblk->base + prePgblk->num == pgblk->base);
			prePgblk->num += pgblk->num;
			im_list_erase(ps->pgblkLst, pgblk);
			pgblk = prePgblk;	// to connect to post.
		}
	}
	if(postPgblk != IM_NULL){
		if((postPgblk->idle == IM_TRUE)){
            IM_ASSERT(pgblk->base + pgblk->num == postPgblk->base);
			pgblk->num += postPgblk->num;
			im_list_erase(ps->pgblkLst, postPgblk);
		}
	}

	//
	pgspace_print_list(ps);

	return IM_RET_OK;
}


//
//
//
#define DTE_ENTRY_BITS	(0x1)	// present bit
#define PTE_ENTRY_BITS	(0x7)	// write, read, present bits

typedef IM_UINT32	dmmu_ptable_entry_t;

typedef struct{	// pte must 4KB agigned, [31:12] page address, [11:0] attributes.
	dmmu_ptable_entry_t	ptEntry[DMMU_PTE_ENTRIES];
}dmmu_ptable_t;

// dte must 4KB agigned, [31:12] pte address, [0] present bit.
typedef dmmu_ptable_t *	dmmu_dtable_entry_t;

typedef struct{
	im_mempool_handle_t	mpl;

	dmmu_dtable_entry_t	*dtEntry;	// array size is DMMU_DTE_ENTRIES. cpu virtual address, it map to the dtablePhyBase.
	IM_Buffer		dtableBuffer;// must 4KB alignment.
	IM_INT32		dtEntryRefcnt[DMMU_DTE_ENTRIES];// refcount max 1024.
	IM_Buffer		ptableBuffer[DMMU_DTE_ENTRIES];// must 4KB alignment.
}dmmu_table_t;

//
// create the full page-table, it needs dtable 4KB, ptable 4KB*1024, total 4MB+4KB.
// this mode can protect page-fault bug, cause our dev-mmu has no page fault interrupt,
// if user set a invalid page(not exist) will couse bus dead. So we first init the 
// table can all accessible(dte has present bit, pte has write/read/present bits).
//
//#define MMU_FULL_TABLE
#define DMMU_FLUSH_CACHE(buffer)	dmmupwl_flush_cache(buffer.vir_addr, buffer.phy_addr, buffer.size)

static IM_RET dmmu_page_table_create(IN dmmu_table_t *table, IN im_mempool_handle_t mpl, OUT IM_UINT32 *dtablePhyBase)
{
	IM_INT32 i;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(table != IM_NULL);
	IM_ASSERT(sizeof(dmmu_dtable_entry_t) == 4);
	IM_ASSERT(sizeof(dmmu_ptable_t) == 4096);
	IM_ASSERT(sizeof(dmmu_ptable_entry_t) == 4);

	//
	dmmupwl_memset((void *)table, 0, sizeof(dmmu_table_t));
	table->mpl = mpl;

	if(dmmupwl_alloc_linear_page_align_memory(&table->dtableBuffer) != IM_RET_OK){
		IM_ERRMSG((IM_STR("dmmupwl_alloc_linear_page_align_memory(dtable) failed")));
		return IM_RET_FAILED;
	}
	IM_INFOMSG((IM_STR("dtableBuffer: vir_addr=0x%x, phy_addr=0x%x"), (IM_INT32)table->dtableBuffer.vir_addr, table->dtableBuffer.phy_addr));

	table->dtEntry = (dmmu_dtable_entry_t *)(table->dtableBuffer.vir_addr);
	dmmupwl_memset((void *)table->dtEntry, 0, 4 * DMMU_DTE_ENTRIES);

#ifdef MMU_FULL_TABLE
	for(i=0; i<DMMU_DTE_ENTRIES; i++){
		if(dmmupwl_alloc_linear_page_align_memory(&table->ptableBuffer[i]) != IM_RET_OK){
			IM_ERRMSG((IM_STR("dmmupwl_alloc_linear_page_align_memory(ptable[%d]) failed"), i));
			goto Fail;
		}
		IM_INFOMSG((IM_STR("ptableBuffer[%d]: vir_addr=0x%x, phy_addr=0x%x"), i, (IM_INT32)table->ptableBuffer[i].vir_addr, table->ptableBuffer[i].phy_addr));
		dmmupwl_memset((void *)table->ptableBuffer[i].vir_addr, 0, 4 * DMMU_PTE_ENTRIES);
		DMMU_FLUSH_CACHE(table->ptableBuffer[i]);
		
		table->dtEntry[i] = (dmmu_dtable_entry_t)(table->ptableBuffer[i].phy_addr | DTE_ENTRY_BITS);
	}
#endif
	DMMU_FLUSH_CACHE(table->dtableBuffer);

	*dtablePhyBase = table->dtableBuffer.phy_addr;	
	return IM_RET_OK;	
#ifdef MMU_FULL_TABLE
Fail:
#endif
	for(i=0; i<DMMU_DTE_ENTRIES; i++){
		if(table->ptableBuffer[i].vir_addr != IM_NULL){
			dmmupwl_free_linear_page_align_memory(&table->ptableBuffer[i]);
		}
	}
	dmmupwl_free_linear_page_align_memory(&table->dtableBuffer);
	return IM_RET_FAILED;
}

static void dmmu_page_table_destroy(IN dmmu_table_t *table)
{
	IM_INT32 i;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(table != IM_NULL);

	for(i=0; i<DMMU_DTE_ENTRIES; i++){
		if(table->ptableBuffer[i].vir_addr != IM_NULL){
			dmmupwl_free_linear_page_align_memory(&table->ptableBuffer[i]);
		}
	}
	if(table->dtableBuffer.vir_addr != IM_NULL){
		dmmupwl_free_linear_page_align_memory(&table->dtableBuffer);
	}

	dmmupwl_memset((void *)table, 0, sizeof(dmmu_table_t));
}

static IM_RET dmmu_page_table_map(IN dmmu_table_t *table, IN dmmu_devaddr_t devAddr, IN dmmu_pt_map_t *ptmap)
{
	dmmu_ptable_t *pte;
	IM_UINT32 i, dteIndex=0, pteIndex=0, tmpDevAddr=0;
	IM_UINT32 flushDteIndex=0;	// dteIndex never be 0.
    IM_BOOL flush=IM_FALSE;
	IM_INFOMSG((IM_STR("%s(devAddr=0x%x, pageNum=%d)"), IM_STR(_IM_FUNC_), devAddr, ptmap->pageNum));

	for(i=0; i<ptmap->pageNum; i++){
		tmpDevAddr = devAddr + PAGE_TO_DEVADDR(i);
		dteIndex = DEVADDR_GET_DT_ENTRY(tmpDevAddr);
		pteIndex = DEVADDR_GET_PT_ENTRY(tmpDevAddr);
		if(table->dtEntry[dteIndex] == IM_NULL){
			IM_ASSERT(table->ptableBuffer[dteIndex].vir_addr == IM_NULL);
			if(dmmupwl_alloc_linear_page_align_memory(&table->ptableBuffer[dteIndex]) != IM_RET_OK){
				IM_ERRMSG((IM_STR("dmmupwl_alloc_linear_page_align_memory() failed")));
				return IM_RET_FAILED;
			}
			IM_INFOMSG((IM_STR("ptableBuffer[%d]: vir_addr=0x%x, phy_addr=0x%x"), dteIndex, 
				(IM_INT32)table->ptableBuffer[dteIndex].vir_addr, table->ptableBuffer[dteIndex].phy_addr));
			dmmupwl_memset((void *)table->ptableBuffer[dteIndex].vir_addr, 0, DMMU_PAGE_SIZE);

			table->dtEntry[dteIndex] = (dmmu_dtable_entry_t)(table->ptableBuffer[dteIndex].phy_addr | DTE_ENTRY_BITS);
			table->dtEntryRefcnt[dteIndex] = 1;
			
			DMMU_FLUSH_CACHE(table->dtableBuffer);
		}else{
			table->dtEntryRefcnt[dteIndex]++;
		}
		
		IM_INFOMSG((IM_STR("tmpDevAddr=0x%x, dteIndex=%d(refcnt=%d), pteIndex=%d"), 
			tmpDevAddr, dteIndex, table->dtEntryRefcnt[dteIndex], pteIndex));

		pte = (dmmu_ptable_t *)table->ptableBuffer[dteIndex].vir_addr;
		pte->ptEntry[pteIndex] = (dmmu_ptable_entry_t)(PAGE_TO_DEVADDR(ptmap->pageList[i]) | PTE_ENTRY_BITS);
		
		//
        if(flushDteIndex == 0){
            flushDteIndex = dteIndex;
            flush = IM_TRUE;
        }
        else if(dteIndex != flushDteIndex){
            DMMU_FLUSH_CACHE(table->ptableBuffer[flushDteIndex]);
            flushDteIndex = 0;
            flush = IM_FALSE;
        }
	}

    if(flush){
        DMMU_FLUSH_CACHE(table->ptableBuffer[flushDteIndex]);
    }

	return IM_RET_OK;
}

static IM_RET dmmu_page_table_unmap(IN dmmu_table_t *table, IN dmmu_devaddr_t devAddr, IN IM_UINT32 pageNum)
{
	dmmu_ptable_t *pte;
	IM_UINT32 i, dteIndex, pteIndex, tmpDevAddr;
	IM_UINT32 flushDteIndex=0;	// dteIndex never be 0.
    IM_BOOL flush=IM_FALSE;
    IM_INFOMSG((IM_STR("%s(devAddr=0x%x, pageNum=%d)"), IM_STR(_IM_FUNC_), devAddr, pageNum));

    for(i=0; i<pageNum; i++){
		tmpDevAddr = devAddr + PAGE_TO_DEVADDR(i);
		dteIndex = DEVADDR_GET_DT_ENTRY(tmpDevAddr);
		pteIndex = DEVADDR_GET_PT_ENTRY(tmpDevAddr);
		
		IM_ASSERT(table->dtEntry[dteIndex] != IM_NULL);
		pte = (dmmu_ptable_t *)table->ptableBuffer[dteIndex].vir_addr;

		table->dtEntryRefcnt[dteIndex]--;
#ifdef MMU_FULL_TABLE
		table->dtEntry[dteIndex] = (dmmu_dtable_entry_t)(0 | DTE_ENTRY_BITS);	// only mark it's accessible, but no valid physical address.
		pte->ptEntry[pteIndex] = (dmmu_ptable_entry_t)(0 | PTE_ENTRY_BITS);
#else
		pte->ptEntry[pteIndex] = 0;	// cannot access.

		if(flushDteIndex == 0){
			flushDteIndex = dteIndex;
            flush = IM_TRUE;
		}
        else if(dteIndex != flushDteIndex){
            DMMU_FLUSH_CACHE(table->ptableBuffer[flushDteIndex]);
            flushDteIndex = 0;
            flush = IM_FALSE;
        }

		if(table->dtEntryRefcnt[dteIndex] == 0){
			dmmupwl_free_linear_page_align_memory(&table->ptableBuffer[dteIndex]);
			table->ptableBuffer[dteIndex].vir_addr = IM_NULL;
			table->dtEntry[dteIndex] = (dmmu_dtable_entry_t)0;
			DMMU_FLUSH_CACHE(table->dtableBuffer);
            flushDteIndex = 0;
            flush = IM_FALSE;   // for the ptableBuffer[dteIndex] has been released, so must set flush to IM_FALSE, else it could abort in next DMMU_FLUSH_CACHE.
		}
#endif
	}

    if(flush){
	    DMMU_FLUSH_CACHE(table->ptableBuffer[flushDteIndex]);
    }

	return IM_RET_OK;
}


//
//
//
#define DMMULIB_VER_MAJOR   4
#define DMMULIB_VER_MINOR   0
#define DMMULIB_VER_PATCH   1
#define DMMULIB_VER_STRING  "4.0.1"

IM_UINT32 dmmulib_version(OUT IM_TCHAR *verString)
{
    if(verString != IM_NULL){
        dmmupwl_strcpy(verString, (IM_TCHAR *)IM_STR(DMMULIB_VER_STRING));
    }
    return IM_MAKE_VERSION(DMMULIB_VER_MAJOR, DMMULIB_VER_MINOR, DMMULIB_VER_PATCH);
}


//
//
//
typedef struct{
	dmmu_table_t		table;
	page_space_t		ps;
	im_mempool_handle_t	mpl;
}dmmu_pagetable_internal_t;


IM_RET dmmulib_pagetable_init(OUT dmmu_pagetable_t *ppt, OUT IM_UINT32 *dtablePhyBase)
{
	dmmu_pagetable_internal_t *pti;

	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(ppt != IM_NULL);
	IM_ASSERT(dtablePhyBase != IM_NULL);

    IM_TIPMSG((IM_STR("dmmu_lib ver%s"), DMMULIB_VER_STRING));

	//
	if(dmmupwl_init() != IM_RET_OK){
		IM_ERRMSG((IM_STR("dmmupwl_init() failed")));
		return IM_RET_FAILED;
	}

	//
	pti = (dmmu_pagetable_internal_t *)dmmupwl_malloc(sizeof(dmmu_pagetable_internal_t));
	if(pti == IM_NULL){
		IM_ERRMSG((IM_STR("malloc(pti) failed")));
		dmmupwl_deinit();
		return IM_RET_FAILED;
	}
	dmmupwl_memset((void *)pti, 0, sizeof(dmmu_pagetable_internal_t));

	pti->mpl = im_mpool_init((func_mempool_malloc_t)dmmupwl_malloc, (func_mempool_free_t)dmmupwl_free);
	if(pti->mpl == IM_NULL){
		IM_ERRMSG((IM_STR("im_mpool_init() failed")));
		goto Fail;
	}

	if(dmmu_page_table_create(&pti->table, pti->mpl, dtablePhyBase) != IM_RET_OK){
		IM_ERRMSG((IM_STR("dmmu_page_table_create() failed")));
		goto Fail;
	}	

	if(pgspace_init(&pti->ps, pti->mpl) != IM_RET_OK){
		IM_ERRMSG((IM_STR("pgspace_init() failed")));
		goto Fail;
	}

	//
	*ppt = (dmmu_pagetable_t)pti;

	return IM_RET_OK;
Fail:
	dmmulib_pagetable_deinit((dmmu_pagetable_t)pti);
	return IM_RET_FAILED;
}

IM_RET dmmulib_pagetable_deinit(IN dmmu_pagetable_t pt)
{
	dmmu_pagetable_internal_t *pti = (dmmu_pagetable_internal_t *)pt;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	IM_ASSERT(pt != IM_NULL);

	pgspace_deinit(&pti->ps);
	dmmu_page_table_destroy(&pti->table);
	if(pti->ps.mpl != IM_NULL){
		im_mpool_deinit(pti->ps.mpl);
	}
	dmmupwl_free((void *)pti);
	
	dmmupwl_deinit();

	return IM_RET_OK;
}

IM_RET dmmulib_pagetable_map(IN dmmu_pagetable_t pt, IN dmmu_pt_map_t *ptmap, OUT dmmu_devaddr_t *devAddr)
{
#if DBGINFO
	IM_INT32 i;
#endif
	dmmu_pagetable_internal_t *pti = (dmmu_pagetable_internal_t *)pt;

	IM_INFOMSG((IM_STR("++%s(pageNum=%d)"), IM_STR(_IM_FUNC_), ptmap->pageNum));
	IM_ASSERT(pt != IM_NULL);
	IM_ASSERT(ptmap != IM_NULL);
	IM_ASSERT(devAddr != IM_NULL);

	//
#if DBGINFO
	for(i=0; i<ptmap->pageNum; i++){
		IM_INFOMSG((IM_STR("pageList[%d]=0x%x"), i, ptmap->pageList[i]));
	}
#endif

	//
	if(pgspace_alloc(&pti->ps, ptmap->pageNum, devAddr) != IM_RET_OK){
		IM_ERRMSG((IM_STR("pgspace_alloc() failed")));
		return IM_RET_FAILED;
	}

	//
	if(dmmu_page_table_map(&pti->table, *devAddr, ptmap) != IM_RET_OK){
		IM_ERRMSG((IM_STR("dmmu_page_table_map() failed")));
		pgspace_free(&pti->ps, *devAddr);
		return IM_RET_FAILED;
	}

	IM_INFOMSG((IM_STR("--devAddr=0x%x"), *devAddr));
	return IM_RET_OK;	
}

IM_RET dmmulib_pagetable_unmap(IN dmmu_pagetable_t pt, IN dmmu_devaddr_t devAddr, IN IM_INT32 pageNum)
{
	dmmu_pagetable_internal_t *pti = (dmmu_pagetable_internal_t *)pt;

	IM_INFOMSG((IM_STR("++%s(devAddr=0x%x, pageNum=%d)"), IM_STR(_IM_FUNC_), devAddr, pageNum));
	IM_ASSERT(pt != IM_NULL);

	IM_ASSERT(dmmu_page_table_unmap(&pti->table, devAddr, pageNum) == IM_RET_OK);
	IM_ASSERT(pgspace_free(&pti->ps, devAddr) == IM_RET_OK);	
	IM_INFOMSG((IM_STR("--%s()"), IM_STR(_IM_FUNC_)));

	return IM_RET_OK;	
}


//
// ############################################################################
//
#define DMMU_REGISTER_DTE_ADDR		0x0000	/**< Current Page Directory Pointer */
#define DMMU_REGISTER_STATUS		0x0004	/**< Status of the MMU */
#define DMMU_REGISTER_COMMAND		0x0008	/**< Command register, used to control the MMU */
#define DMMU_REGISTER_PAGE_FAULT_ADDR	0x000C	/**< Logical address of the last page fault */
#define DMMU_REGISTER_ZAP_ONE_LINE	0x0010	/**< Used to invalidate the mapping of a single page from the MMU */
#define DMMU_REGISTER_INT_RAWSTAT	0x0014	/**< Raw interrupt status, all interrupts visible */
#define DMMU_REGISTER_INT_CLEAR		0x0018	/**< Indicate to the MMU that the interrupt has been received */
#define DMMU_REGISTER_INT_MASK		0x001C	/**< Enable/disable types of interrupts */
#define DMMU_REGISTER_INT_STATUS	0x0020	/**< Interrupt status based on the mask */

#define DMMU_COMMAND_ENABLE_PAGING	0x00	/**< Enable paging (memory translation) */
#define DMMU_COMMAND_DISABLE_PAGING	0x01	/**< Disable paging (memory translation) */
#define DMMU_COMMAND_ENABLE_STALL	0x02	/**<  Enable stall on page fault */
#define DMMU_COMMAND_DISABLE_STALL	0x03	/**< Disable stall on page fault */
#define DMMU_COMMAND_ZAP_CACHE		0x04	/**< Zap the entire page table cache */
#define DMMU_COMMAND_PAGE_FAULT_DONE	0x05	/**< Page fault processed */
#define DMMU_COMMAND_SOFT_RESET		0x06	/**< Reset the MMU back to power-on settings */

#define DMMU_STATUS_BIT_PAGING_ENABLED		(1<<0)
#define DMMU_STATUS_BIT_PAGE_FAULT_ACTIVE	(1<<1)
#define DMMU_STATUS_BIT_STALL_ACTIVE		(1<<2)
#define DMMU_STATUS_BIT_IDLE			(1<<3)
#define DMMU_STATUS_BIT_REPLAY_BUFFER_EMPTY	(1<<4)
#define DMMU_STATUS_BIT_PAGE_FAULT_IS_WRITE	(1<<5)

#define DMMU_INVALID_PAGE 		((IM_UINT32)(~0))


//#define DMMU_DUMMY
#define DMMU_ZAP_ENTIRE_PAGES


IM_RET dmmulib_mmu_raw_reset(IM_UINT32 devid)
{
	const IM_INT32 max_loop_count = 100;
	const IM_INT32 delay_in_usecs = 50;
	IM_INT32 i;

	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), devid));
#ifndef DMMU_DUMMY
	dmmupwl_write_reg(devid, DMMU_REGISTER_DTE_ADDR, 0xCAFEBABE);
	dmmupwl_write_reg(devid, DMMU_REGISTER_COMMAND, DMMU_COMMAND_SOFT_RESET);

	for(i=0; i<max_loop_count; ++i){
		if(dmmupwl_read_reg(devid, DMMU_REGISTER_DTE_ADDR)==0){
			break;
		}
		dmmupwl_udelay(delay_in_usecs);
	}
	if(i >= max_loop_count){
		IM_ERRMSG((IM_STR("Reset request time out in %s(devid=%d)"), IM_STR(_IM_FUNC_), devid));
		return IM_RET_FAILED;
	}
#endif
	return IM_RET_OK;
}

IM_RET dmmulib_mmu_init(IM_UINT32 devid)
{
	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), devid));

	if(dmmupwl_init() != IM_RET_OK){
		IM_ERRMSG((IM_STR("dmmupwl_init() failed")));
		return IM_RET_FAILED;
	}

	if(dmmupwl_mmu_init(devid) != IM_RET_OK){
		IM_ERRMSG((IM_STR("dmmupwl_mmu_init(devid=%d) failed"), devid));
		dmmupwl_deinit();
		return IM_RET_FAILED;
	}
	return dmmulib_mmu_raw_reset(devid);
}

IM_RET dmmulib_mmu_deinit(IM_UINT32 devid)
{
	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), devid));
	if(dmmupwl_mmu_deinit(devid) != IM_RET_OK){
		IM_ERRMSG((IM_STR("dmmupwl_mmu_deinit(devid=%d) failed"), devid));
	}
	dmmupwl_deinit();

	return IM_RET_OK;
}

IM_RET dmmulib_mmu_enable_paging(IM_UINT32 devid)
{
	const IM_INT32 max_loop_count = 100;
	const IM_INT32 delay_in_usecs = 50;
	IM_INT32 i;

	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), devid));

#ifndef DMMU_DUMMY
	dmmupwl_write_reg(devid, DMMU_REGISTER_COMMAND, DMMU_COMMAND_ENABLE_PAGING);

	for( i=0; i<max_loop_count; ++i){
		if(dmmupwl_read_reg(devid, DMMU_REGISTER_STATUS) & DMMU_STATUS_BIT_PAGING_ENABLED){
			break;
		}
		dmmupwl_udelay(delay_in_usecs);
	}
	if(i >= max_loop_count){
		IM_ERRMSG((IM_STR("Enable paging request failed in %s(devid=%d), stat=0x%x"), IM_STR(_IM_FUNC_), devid, dmmupwl_read_reg(devid, DMMU_REGISTER_STATUS)));
		return IM_RET_FAILED;
	}
#endif
	return IM_RET_OK;	
}

IM_RET dmmulib_mmu_disable_paging(IM_UINT32 devid)
{
	const IM_INT32 max_loop_count = 100;
	const IM_INT32 delay_in_usecs = 50;
	IM_INT32 i;

	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), devid));

#ifndef DMMU_DUMMY
	dmmupwl_write_reg(devid, DMMU_REGISTER_COMMAND, DMMU_COMMAND_DISABLE_PAGING);

	for( i=0; i<max_loop_count; ++i){
		if((dmmupwl_read_reg(devid, DMMU_REGISTER_STATUS) & DMMU_STATUS_BIT_PAGING_ENABLED)==0){
			break;
		}
		dmmupwl_udelay(delay_in_usecs);
	}
	if(i==max_loop_count){
		IM_ERRMSG((IM_STR("Disable paging request failed in %s(devid=%d), stat=0x%x"), IM_STR(_IM_FUNC_), devid, dmmupwl_read_reg(devid, DMMU_REGISTER_STATUS)));
		return IM_RET_FAILED;
	}
#endif
	return IM_RET_OK;	
}

IM_RET dmmulib_mmu_enable_stall(IM_UINT32 devid)
{
	const IM_INT32 max_loop_count = 100;
	const IM_INT32 delay_in_usecs = 50;
	IM_INT32 i;

	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), devid));

#ifndef DMMU_DUMMY
	dmmupwl_write_reg(devid, DMMU_REGISTER_COMMAND, DMMU_COMMAND_ENABLE_STALL);

	for( i=0; i<max_loop_count; ++i){
		if(dmmupwl_read_reg(devid, DMMU_REGISTER_STATUS) & DMMU_STATUS_BIT_STALL_ACTIVE){
			break;
		}
		dmmupwl_udelay(delay_in_usecs);
	}
	if(i >= max_loop_count){
		IM_ERRMSG((IM_STR("Enable paging request failed in %s(devid=%d), stat=0x%x"), IM_STR(_IM_FUNC_), devid, dmmupwl_read_reg(devid, DMMU_REGISTER_STATUS)));
		return IM_RET_FAILED;
	}
#endif
	return IM_RET_OK;	
}

IM_RET dmmulib_mmu_disable_stall(IM_UINT32 devid)
{
	const IM_INT32 max_loop_count = 100;
	const IM_INT32 delay_in_usecs = 50;
	IM_INT32 i;

	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), devid));

#ifndef DMMU_DUMMY
	dmmupwl_write_reg(devid, DMMU_REGISTER_COMMAND, DMMU_COMMAND_DISABLE_STALL);

	for( i=0; i<max_loop_count; ++i){
		if((dmmupwl_read_reg(devid, DMMU_REGISTER_STATUS) & DMMU_STATUS_BIT_STALL_ACTIVE)==0){
			break;
		}
		dmmupwl_udelay(delay_in_usecs);
	}
	if(i >= max_loop_count){
		IM_ERRMSG((IM_STR("Enable paging request failed in %s(devid=%d), stat=0x%x"), IM_STR(_IM_FUNC_), devid, dmmupwl_read_reg(devid, DMMU_REGISTER_STATUS)));
		return IM_RET_FAILED;
	}
#endif
	return IM_RET_OK;	
}

IM_RET dmmulib_mmu_set_dtable(IM_UINT32 devid, IM_UINT32 dtablePhyBase)
{
	IM_INFOMSG((IM_STR("%s(devid=%d, dtablePhyBase=0x%x)"), IM_STR(_IM_FUNC_), devid, dtablePhyBase));
	
#ifndef DMMU_DUMMY
	dmmupwl_write_reg(devid, DMMU_REGISTER_DTE_ADDR, dtablePhyBase);
	dmmupwl_write_reg(devid, DMMU_REGISTER_COMMAND, DMMU_COMMAND_ZAP_CACHE);
#endif
	return IM_RET_OK;	
}

IM_RET dmmulib_mmu_update_ptable(IM_UINT32 devid, dmmu_devaddr_t devAddr, IM_INT32 pageNum)
{
#ifndef DMMU_ZAP_ENTIRE_PAGES
	IM_UINT32 i,tmpDevAddr;
#endif
	IM_INFOMSG((IM_STR("%s(devid=%d, devAddr=0x%x, pageNum=%d)"), IM_STR(_IM_FUNC_), devid, devAddr, pageNum));

#ifndef DMMU_DUMMY
#ifdef DMMU_ZAP_ENTIRE_PAGES
	dmmupwl_write_reg(devid, DMMU_REGISTER_COMMAND, DMMU_COMMAND_ZAP_CACHE);
#else
	for(i=0; i<pageNum; i++){
		tmpDevAddr = devAddr + PAGE_TO_DEVADDR(i);
		dmmupwl_write_reg(devid, DMMU_REGISTER_ZAP_ONE_LINE, tmpDevAddr); 
	}
#endif
#endif
	return IM_RET_OK;	
}

IM_INT32 dmmulib_dump(IM_TCHAR *buffer, IM_INT32 size)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
    return dmmupwl_dump(buffer, size);
}

