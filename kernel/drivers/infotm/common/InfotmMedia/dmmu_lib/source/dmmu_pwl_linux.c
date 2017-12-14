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
** v1.2.1	leo@2012/04/06: modify page_align_memory allocate and free.
** v1.2.2	leo@2012/04/20: rewrite alloc_linear_page_memory() and free_linear_
**				page_memory(), and add dmmupwl_flush_cache().
** v1.2.3   leo@2013/11/02: add support for imapx900.
** v3.0.0   leo@2013/11/14: move DMMU regbase definations from dmmu_pwl.h, and
**              add support for isp&venc&osd0&osd1, and add dmmupwl_dump() but
**              not impelemeted.
** v4.0.0   leo@2013/12/23: add dmmupwl_strcpy().
** v4.0.1   leo@2014/01/17: add GFP_KERNEL flag in alloc_page(). and in dmmupwl_flush_cache(),
**                          case vir_addr to int type in calculating.
**
*****************************************************************************/ 

#include <InfotmMedia.h>
#include <IM_devmmuapi.h>
#include <dmmu_lib.h>
#include <dmmu_pwl.h>

#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <asm/cacheflush.h>

#define DBGINFO		0
#define DBGWARN		1
#define DBGERR		1
#define DBGTIP		1

#define INFOHEAD	"DMMUPWL_I:"
#define WARNHEAD	"DMMUPWL_I:"
#define ERRHEAD		"DMMUPWL_I:"
#define TIPHEAD		"DMMUPWL_I:"


typedef struct{
	IM_UINT32	phyBase;
	IM_UINT32	offset;
	IM_UINT32	accessAddr;
}mmu_reg_t;

typedef struct{
	void *	kVirAddr;
	void *	kVirAddrRemap;
}ptable_mem_t;

typedef struct{
	mmu_reg_t	regs[DMMU_DEV_MAX];
	IM_INT32	refcnt;

	im_mempool_handle_t	mpl;
	im_list_handle_t	ptmlist;	// list of ptable_mem_t.
}dmmupwl_t;

static dmmupwl_t *gPwl = IM_NULL;


//
// BASE must 4KB alignment.
//
#define DMMU_REG_SIZE		(0x1000)
#define MMU_BASE_INVALID    (0xffffffff)

#ifdef SOC_IMAPX800
static mmu_reg_t gDmmuRegTable[DMMU_DEV_MAX] =
{
    {0x29000000, 0x800, 0}, // G2D
    {0x22006000, 0x200, 0}, // IDSO_W0
    {0x22006000, 0x400, 0}, // IDS0_W1
    {0x22006000, 0x600, 0}, // IDS0_W2
    {0x22006000, 0x800, 0}, // IDS0_CBCR
    {0x23006000, 0x200, 0}, // IDS1_W0
    {0x23006000, 0x400, 0}, // IDS1_W1
    {0x23006000, 0x600, 0}, // IDS1_W2
    {0x23006000, 0x800, 0}, // IDS1_CBCR
    {0x25040000, 0x0, 0}, // VDEC
    {MMU_BASE_INVALID, 0x0, 0}, // VENC(nouse)
    {MMU_BASE_INVALID, 0x0, 0}, // ISP(nouse)
    {MMU_BASE_INVALID, 0x0, 0}, // OSD0(nouse)
    {MMU_BASE_INVALID, 0x0, 0} // OSD1(nouse)
};
#endif
#ifdef SOC_IMAPX900
static mmu_reg_t gDmmuRegTable[DMMU_DEV_MAX] =
{
    {MMU_BASE_INVALID, 0x0, 0}, // G2D(nouse)
    {MMU_BASE_INVALID, 0x0, 0}, // IDSO_W0(nouse)
    {MMU_BASE_INVALID, 0x0, 0}, // IDS0_W1(nouse)
    {MMU_BASE_INVALID, 0x0, 0}, // IDS0_W2(nouse)
    {MMU_BASE_INVALID, 0x0, 0}, // IDS0_CBCR(nouse)
    {MMU_BASE_INVALID, 0x0, 0}, // IDS1_W0(nouse)
    {MMU_BASE_INVALID, 0x0, 0}, // IDS1_W1(nouse)
    {MMU_BASE_INVALID, 0x0, 0}, // IDS1_W2(nouse)
    {MMU_BASE_INVALID, 0x0, 0}, // IDS1_CBCR(nouse)
    {0x28140000, 0x0, 0}, // VDEC
    {0x28540000, 0x0, 0}, // VENC
    {0x28240000, 0x0, 0}, // ISP
    {0x22006000, 0x400, 0}, // OSD0
    {0x23006000, 0x400, 0} // OSD1
};
#endif


IM_RET dmmupwl_init(void)
{
    IM_INT32 i;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
	
	if(gPwl == IM_NULL){
		gPwl = (dmmupwl_t *)dmmupwl_malloc(sizeof(dmmupwl_t));
		if(gPwl == IM_NULL){
			IM_ERRMSG((IM_STR("malloc(gPwl) failed")));
			return IM_RET_FAILED;
		}
		dmmupwl_memset((void *)gPwl, 0, sizeof(dmmupwl_t));
		
		gPwl->mpl = im_mpool_init((func_mempool_malloc_t)dmmupwl_malloc, (func_mempool_free_t)dmmupwl_free);
		if(gPwl->mpl == IM_NULL){
			IM_ERRMSG((IM_STR("im_mpool_init() failed")));
			goto Fail;
		}

		gPwl->ptmlist = im_list_init(sizeof(ptable_mem_t), gPwl->mpl);
		if(gPwl->ptmlist == IM_NULL){
			IM_ERRMSG((IM_STR("im_list_init() failed")));
			goto Fail;
		}

        for(i=0; i<DMMU_DEV_MAX; i++){
            gPwl->regs[i].phyBase = gDmmuRegTable[i].phyBase;
            gPwl->regs[i].offset = gDmmuRegTable[i].offset;
        }

		gPwl->refcnt = 1;
	}else{
		gPwl->refcnt++;
	}

	IM_INFOMSG((IM_STR("gPwl->refcnt=%d"), gPwl->refcnt));

	return IM_RET_OK;

Fail:
	if(gPwl->ptmlist != IM_NULL){
		im_list_deinit(gPwl->ptmlist);
	}
	if(gPwl->mpl != IM_NULL){
		im_mpool_deinit(gPwl->mpl);
	}
	dmmupwl_free((void *)gPwl);
	gPwl = IM_NULL;
	return IM_RET_FAILED;
}

IM_RET dmmupwl_deinit(void)
{
	IM_INFOMSG((IM_STR("%s(gPwl->refcnt=%d)"), IM_STR(_IM_FUNC_), (gPwl != IM_NULL)?gPwl->refcnt:0));

	if(gPwl != IM_NULL){
		gPwl->refcnt--;
		if(gPwl->refcnt == 0){
			if(gPwl->ptmlist != IM_NULL){
				im_list_deinit(gPwl->ptmlist);
			}
			if(gPwl->mpl != IM_NULL){
				im_mpool_deinit(gPwl->mpl);
			}
			dmmupwl_free((void *)gPwl);
			gPwl = IM_NULL;
		}
	}

	return IM_RET_OK;
}

void *dmmupwl_malloc(IM_INT32 size)
{
	return kmalloc(size, GFP_KERNEL);
}

void dmmupwl_free(void *p)
{
	kfree(p);
}

void *dmmupwl_memcpy(void *dst, void *src, IM_INT32 size)
{
	return memcpy(dst, src, size);
}

void *dmmupwl_memset(void *p, IM_CHAR c, IM_INT32 size)
{
	return memset(p, c, size);
}

IM_TCHAR *dmmupwl_strcpy(IM_TCHAR *dst, IM_TCHAR *src)
{
	return strcpy(dst, src);
}

IM_RET dmmupwl_alloc_linear_page_align_memory(OUT IM_Buffer *buffer)
{
	struct page *pg;
	//ptable_mem_t ptm = {0};
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));

	IM_ASSERT(DMMU_PAGE_SHIFT == PAGE_SHIFT);
	IM_ASSERT(DMMU_PAGE_SIZE == PAGE_SIZE);
#if 0
	//ptm.kVirAddr = kmalloc(pageNum << PMM_PAGE_SHIFT, GFP_KERNEL);
	ptm.kVirAddr = (void *)__get_free_page(GFP_KERNEL | __GFP_NOWARN | __GFP_COLD | __GFP_REPEAT);
	if(ptm.kVirAddr == IM_NULL){
		//IM_ERRMSG((IM_STR("kmalloc() failed")));
		IM_ERRMSG((IM_STR("__get_free_page() failed")));
		return IM_RET_FAILED;
	}

	buffer->phy_addr = __pa(ptm.kVirAddr);
	buffer->size = 1 << PMM_PAGE_SHIFT;
	buffer->flag = IM_BUFFER_FLAG_PHY;

	IM_INFOMSG((IM_STR("ptm.kVirAddr=0x%x"), (IM_INT32)ptm.kVirAddr));
	IM_INFOMSG((IM_STR("buffer->phy_addr=0x%x"), buffer->phy_addr));
	IM_INFOMSG((IM_STR("buffer->size=%d"), buffer->size));
	IM_INFOMSG((IM_STR("buffer->flag=0x%x"), buffer->flag));

	ptm.kVirAddrRemap = ioremap_nocache(buffer->phy_addr, buffer->size);
	if(ptm.kVirAddrRemap == IM_NULL){
		IM_ERRMSG((IM_STR("ioremap_nocache() failed")));
		goto Fail;
	}
	IM_INFOMSG((IM_STR("ptm.kVirAddrRemap=0x%x"), (IM_INT32)ptm.kVirAddrRemap));

	if(im_list_put_back(gPwl->ptmlist, &ptm) != IM_RET_OK){
		IM_ERRMSG((IM_STR("im_list_put_back() failed")));
		goto Fail;
	}

	buffer->vir_addr = ptm.kVirAddrRemap;
	return IM_RET_OK;

Fail:
	if(ptm.kVirAddrRemap != IM_NULL){
		iounmap(ptm.kVirAddrRemap);
	}
	//kfree(ptm.kVirAddr);
	free_page((IM_UINT32)ptm.kVirAddr);
	return IM_RET_FAILED;
#else
	pg = alloc_page(/*GFP_HIGHUSER | */GFP_KERNEL | __GFP_ZERO | __GFP_REPEAT | __GFP_NOWARN | __GFP_COLD);
	if(pg == IM_NULL){
		IM_ERRMSG((IM_STR("alloc_page() failed")));
		return IM_RET_FAILED;
	}
	buffer->vir_addr = page_address(pg);
	buffer->phy_addr = dma_map_page(NULL, pg, 0, PAGE_SIZE, DMA_BIDIRECTIONAL);
	buffer->size = DMMU_PAGE_SIZE;
	buffer->flag = IM_BUFFER_FLAG_PHY;

	IM_INFOMSG((IM_STR("buffer vir_addr=0x%x, phy_addr=0x%x"), (IM_UINT32)buffer->vir_addr, buffer->phy_addr));
	return IM_RET_OK;
#endif	
}

IM_RET dmmupwl_free_linear_page_align_memory(IN IM_Buffer *buffer)
{
	//ptable_mem_t *ptm;
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
#if 0
	ptm = (ptable_mem_t *)im_list_begin(gPwl->ptmlist);
	IM_ASSERT(ptm != IM_NULL);
	while(ptm != IM_NULL){
		if(ptm->kVirAddrRemap == buffer->vir_addr){
			break;
		}
		ptm = (ptable_mem_t *)im_list_next(gPwl->ptmlist);
	}
	IM_ASSERT(ptm != IM_NULL);
	IM_ASSERT(ptm->kVirAddrRemap != IM_NULL);
	IM_ASSERT(ptm->kVirAddr != IM_NULL);

	IM_INFOMSG((IM_STR("ptm->kVirAddr=0x%x"), (IM_INT32)ptm->kVirAddr));
	IM_INFOMSG((IM_STR("ptm->kVirAddrRemap=0x%x"), (IM_INT32)ptm->kVirAddrRemap));

	iounmap(ptm->kVirAddrRemap);
	//kfree(ptm->kVirAddr);
	free_page((IM_UINT32)ptm->kVirAddr);
	im_list_erase(gPwl->ptmlist, ptm);

	return IM_RET_OK;
#else	
	IM_INFOMSG((IM_STR("buffer vir_addr=0x%x, phy_addr=0x%x"), (IM_UINT32)buffer->vir_addr, buffer->phy_addr));
	dma_unmap_page(NULL, buffer->phy_addr, PAGE_SIZE, DMA_BIDIRECTIONAL);
	__free_page(virt_to_page(buffer->vir_addr));
	return IM_RET_OK;
#endif	
}

void dmmupwl_flush_cache(void *virAddr, IM_UINT32 phyAddr, IM_INT32 size)
{
	IM_INFOMSG((IM_STR("%s(virAddr=0x%x, phyAddr=0x%x, size=%d)"), IM_STR(_IM_FUNC_), (IM_INT32)virAddr, phyAddr, size));

	// Flush L1 using virtual address.
	dmac_flush_range(virAddr, (void *)((IM_INT32)virAddr + size - 1));

	// Flush L2 using physical address.
	outer_flush_range(phyAddr, phyAddr + size - 1);	
}

IM_RET dmmupwl_write_reg(IM_INT32 devid, IM_UINT32 addr, IM_UINT32 val)
{
	IM_INFOMSG((IM_STR("%s(devid=%d, addr=0x%x, val=0x%x)"), IM_STR(_IM_FUNC_), devid, addr, val));
	writel(val, (void *)(gPwl->regs[devid].accessAddr + addr));
	return IM_RET_OK;
}

IM_UINT32 dmmupwl_read_reg(IM_INT32 devid, IM_UINT32 addr)
{
	IM_INFOMSG((IM_STR("%s(devid=%d, addr=0x%x)"), IM_STR(_IM_FUNC_), devid, addr));
	return readl((void *)(gPwl->regs[devid].accessAddr + addr));
}

IM_RET dmmupwl_write_regbit(IM_INT32 devid, IM_UINT32 addr, IM_INT32 bit, IM_INT32 width, IM_UINT32 val)
{
	volatile IM_UINT32 tmp;
	tmp = dmmupwl_read_reg(devid, addr);
	tmp &= ~(((1 << width) - 1) << bit);
	tmp |= val << bit;
	dmmupwl_write_reg(devid, addr, tmp);
	return IM_RET_OK;
}

void dmmupwl_udelay(IM_INT32 us)
{
	udelay(us);
}

IM_RET dmmupwl_mmu_init(IM_INT32 devid)
{
	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), devid));

    if(gPwl->regs[devid].phyBase == MMU_BASE_INVALID){
        IM_ERRMSG((IM_STR("invalid device(%d) MMU phyBase(0x%x)"), devid, MMU_BASE_INVALID));
        return IM_RET_FAILED;
    }

	gPwl->regs[devid].accessAddr = (IM_UINT32)ioremap_nocache(gPwl->regs[devid].phyBase, DMMU_REG_SIZE);
	if(gPwl->regs[devid].accessAddr == 0){
		IM_ERRMSG((IM_STR("ioremap_nocache(devid=%d) failed"), devid));
		return IM_RET_FAILED;
	}

	gPwl->regs[devid].accessAddr += gPwl->regs[devid].offset;
	IM_INFOMSG((IM_STR("gPwl->regs[%d].accessAddr=0x%x"), devid, gPwl->regs[devid].accessAddr));
	return IM_RET_OK;
}

IM_RET dmmupwl_mmu_deinit(IM_INT32 devid)
{
	IM_INFOMSG((IM_STR("%s(devid=%d)"), IM_STR(_IM_FUNC_), devid));
	if(gPwl->regs[devid].accessAddr != 0){
		iounmap((void *)(gPwl->regs[devid].accessAddr - gPwl->regs[devid].offset));
		gPwl->regs[devid].accessAddr = 0;
	}
	return IM_RET_OK;
}

IM_INT32 dmmupwl_dump(IM_TCHAR *buffer, IM_INT32 size)
{
	IM_INFOMSG((IM_STR("%s()"), IM_STR(_IM_FUNC_)));
    return 0;
}

