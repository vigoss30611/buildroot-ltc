
/*
 * fr solution
 *
 * warits.wang@infotm.com
 * 2016/03/22
 */

#include <ci_internal/sys_mem.h>
#include <ci_kernel/ci_kernel.h>
#include <mmulib/mmu.h>
#include <mmulib/heap.h>
#include <linux/fr.h>
#include <linux/mm.h>
struct SYS_MEM_ALLOC {
	struct fr *fr;
	/**
	 * created at allocation time using vmalloc or kmalloc according to its
	 * size
	 */
	IMG_UINT64 *physList;
};

static int populatePhysList(struct SYS_MEM_ALLOC *pAlloc)
{
    unsigned int pg_i, n;
    IMG_UINT64 *physAddrs = NULL;


    IMG_ASSERT(pAlloc != NULL);
    IMG_ASSERT(pAlloc->physList == NULL);

	n = pAlloc->fr->size / PAGE_SIZE;
    physAddrs = (IMG_UINT64 *)IMG_BIGALLOC(n * (sizeof(*physAddrs)));

    if (!physAddrs)
    {
        CI_FATAL("failed to allocate list of physical addresses\n");
        return 1;
    }

	for(pg_i = 0; pg_i < n; pg_i++) {
		physAddrs[pg_i] = pAlloc->fr->ring->phys_addr + pg_i * PAGE_SIZE;
	}

	pAlloc->physList = physAddrs;
	return 0;
}

struct SYS_MEM_ALLOC* Platform_MemAllocContiguous(IMG_SIZE size, void *extra,
    IMG_RESULT *ret)
{
    struct SYS_MEM_ALLOC *pAlloc = NULL;
	struct fr_buf *buf;


    if(strncmp(extra, "ddk", 3))
    {
        extra = "ddk-alloc";
    }
    pAlloc = (struct SYS_MEM_ALLOC*)IMG_BIGALLOC(sizeof(*pAlloc));
    if (pAlloc == NULL )
    {
        *ret = IMG_ERROR_MALLOC_FAILED;
        CI_FATAL("Failed to allocate internal structure\n");
		return NULL;
    }

	pAlloc->fr = __fr_alloc(extra, size, FR_FLAG_RING(1));
	if(! pAlloc->fr) goto failed;

	fr_get_buf(pAlloc->fr, NULL);

	//populate to the physical address list
	if (populatePhysList(pAlloc) != 0) goto failed;

	*ret = IMG_SUCCESS;
	return pAlloc;
failed:
    if (pAlloc->fr)
    {
        fr_put_buf(pAlloc->fr, pAlloc->fr->ring);
        __fr_free(pAlloc->fr);
    }
    if (pAlloc)
        IMG_BIGFREE(pAlloc);
	*ret = IMG_ERROR_MALLOC_FAILED;
	return NULL;
}

struct SYS_MEM_ALLOC* Platform_MemAlloc(IMG_SIZE size, void *extra,
    IMG_RESULT *ret)
{
	if(strncmp(extra, "ddk-page", 8))
	{
		extra = "ddk-alloc";
	}
	return Platform_MemAllocContiguous(size, extra, ret);
}

void Platform_MemFree(struct SYS_MEM_ALLOC *mem)
{
	if(!mem->physList)
		goto free_fr;

	IMG_BIGFREE(mem->physList);

free_fr:
	fr_put_buf(mem->fr, mem->fr->ring);
	__fr_free(mem->fr);

	IMG_BIGFREE(mem);
	//printk("Is end int fr Platform_MemFree\n");
}

IMG_HANDLE Platform_MemGetTalHandle(IMG_CONST SYS_MEM *psMem, IMG_SIZE uiOffset,
		IMG_SIZE *pOff2)
{
    IMG_ASSERT(psMem);
    IMG_ASSERT(psMem->pAlloc);
    IMG_ASSERT(pOff2);
    *pOff2 = uiOffset;

    return (IMG_HANDLE)(psMem->pAlloc->fr->ring->virt_addr);
}

IMG_UINTPTR Platform_MemGetDevMem(const SYS_MEM *psMem, IMG_SIZE uiOffset)
{
    IMG_ASSERT(psMem);
    IMG_ASSERT(psMem->pAlloc);

    return psMem->pAlloc->fr->ring->phys_addr + uiOffset;
}

void* Platform_MemGetMemory(SYS_MEM *psMem)
{
  IMG_ASSERT(psMem);
  IMG_ASSERT(psMem->pAlloc);

	return (void*)(psMem->pAlloc->fr->ring->virt_addr);
}

IMG_UINT64* Platform_MemGetPhysPages(const SYS_MEM *psMem)
{
    IMG_ASSERT(psMem != NULL);
    IMG_ASSERT(psMem->pAlloc != NULL);
    IMG_ASSERT(psMem->pAlloc->physList != NULL);

    return psMem->pAlloc->physList;
}

IMG_RESULT Platform_MemMapUser(struct SYS_MEM *psMem,
    struct vm_area_struct *vma)
{
    IMG_ASSERT(psMem);

    vma->vm_pgoff = (unsigned long)((psMem->pAlloc->fr->ring->phys_addr) >> PAGE_SHIFT);
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
                vma->vm_end - vma->vm_start, vma->vm_page_prot))
    {
        return IMG_ERROR_INTERRUPTED;
    }

    return IMG_SUCCESS;
}

/* fr do not support DMMU & cache */
IMG_RESULT Platform_MemUpdateHost(SYS_MEM *psMem)
{
  /* Mapped in non-cached area */
    return IMG_SUCCESS;
}

IMG_RESULT Platform_MemUpdateDevice(SYS_MEM *psMem)
{
  /* Mapped in non-cached area */
    return IMG_SUCCESS;
}

struct MMUPage* Platform_MMU_MemPageAlloc(void)
{
	struct MMUPage *mmuPage = NULL;
	struct SYS_MEM_ALLOC *pAlloc = NULL;
	IMG_RESULT ret;

	mmuPage = (struct MMUPage *)IMG_BIGALLOC(sizeof(*mmuPage));
	if (NULL == mmuPage)
	{
		CI_FATAL("Failed to allocat memory for mmu page structure in fr.\n");
		goto err_alloc;
	}
    //should check pAlloc is NULL???
	pAlloc = Platform_MemAlloc(4096, "ddk-page", &ret);
	mmuPage->uiCpuVirtAddr =(IMG_UINTPTR) pAlloc->fr->ring->virt_addr;
	mmuPage->uiPhysAddr = (IMG_UINTPTR)pAlloc->fr->ring->phys_addr;
	mmuPage->priv = (void *)pAlloc;
	return mmuPage;

err_alloc:

  return NULL;
}

void Platform_MMU_MemPageFree(struct MMUPage *mmuPage)
{
//	printk("in mmu MemPageFree vitural: %x\n", mmuPage->uiCpuVirtAddr);
//	printk("in mmu MemPageFree physical: %llx\n", mmuPage->uiPhysAddr);
//	printk("in mmu MemPageFree PAGE_SHIFT:  %llx\n", (mmuPage->uiPhysAddr) >> PAGE_SHIFT);
//	struct page *page = pfn_to_page(mmuPage->uiPhysAddr >> PAGE_SHIFT);
	//__free_pages(page, 0);
	struct SYS_MEM_ALLOC *pAlloc = (struct SYS_MEM_ALLOC *)mmuPage->priv;
	Platform_MemFree(pAlloc);
	IMG_BIGFREE(mmuPage);

	//printk("Is end int fr Platform_MMUMemPageFree\n");
}

void Platform_MMU_MemPageWrite(struct MMUPage *pWriteTo, unsigned int offset,
    IMG_UINT64 uiToWrite, unsigned int flags)
{
	IMG_UINT32 *dirMemory = NULL;
	IMG_UINT64 currPhysAddr = uiToWrite;
	IMG_UINT32 extAddress = 0;

	IMG_ASSERT(pWriteTo != NULL);

	dirMemory = (IMG_UINT32 *)pWriteTo->uiCpuVirtAddr;

	/*
	 * Shifts to do to make the extended address fits into the entries of the
	 * MMU pages.
	 */
	extAddress = KRN_CI_MMUSize(&(g_psCIDriver->sMMU)) - 32;

	/*
	 * Assumes that the MMU HW has the extra-bits enabled (this simple function
	 * has no way of knowing).
	 */
	if (extAddress > 0) {
		CI_DEBUG("shift addresses for MMU by %d bits\n", extAddress);
		currPhysAddr >>= extAddress;
	}

	/*
	 * The IMGMMU_PAGE_SHIFT bottom bits should be masked because page
	 * allocation IMGMMU_PAGE_SHIFT - (IMGMMU_PHYS_SIZE - IMGMMU_VIRT_SIZE)
	 * are used for flags so it's ok.
	 */
	dirMemory[offset] = (IMG_UINT32)currPhysAddr | (flags);
}

struct SYS_MEM_ALLOC * Platform_MemImport(int fd, IMG_SIZE allocationSize,
    IMG_RESULT *ret)
{
  *ret = IMG_ERROR_NOT_SUPPORTED;
  return NULL;
}

void Platform_MemRelease(struct SYS_MEM_ALLOC *psMem)
{ }
