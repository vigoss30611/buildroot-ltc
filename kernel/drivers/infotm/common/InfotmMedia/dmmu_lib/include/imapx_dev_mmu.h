/**
 *
 *
 *
 *
 *
 *
 *
 *
 **/


#ifndef __IMAPX_DEV_MMU_H__
#define __IMAPX_DEV_MMU_H__


/**
 * 
 *
 **/
typedef unsigned int imapx_dev_addr_t;

#define IMAPX_DEV_ADDR_INVALID	(imapx_dev_addr_t)0xbeadadde    // same as ALC_DEVADDR_INVALID which defined in IM_buffallocapi.h

typedef void * imapx_dmmu_handle_t;

struct _imapx_dev_mmu_mapping_dma_buf_data {
	int buf_uid;    // in

	int dma_buf_fd; // out, -1 invalid.
	int size;       // out
	imapx_dev_addr_t dev_address;   // out
    unsigned int phy_addr;  // out, it may be IMAPX_DEV_ADDR_INVALID when no phy address.
};

struct _imapx_dev_mmu_attach_data {
	int buf_uid;
	int size;
	int offset;
	imapx_dmmu_handle_t handle;
};

struct _imapx_dev_mmu_handle_data {
	int dev_id;
	imapx_dmmu_handle_t handle;
};

#define IMAPX_IOC_DEV_MMU_MAP_DMA_BUF       0x1000  // _IOWR(_IMAPX_DEV_MMU_IOC_MAGIC, 0, struct _imapx_dev_mmu_mapping_dma_buf_data)

#define IMAPX_IOC_DEV_MMU_UNMAP_DMA_BUF     0x1001  // _IOWR(_IMAPX_DEV_MMU_IOC_MAGIC, 1, struct _imapx_dev_mmu_mapping_dma_buf_data)

#define IMAPX_IOC_DEV_MMU_INIT_DEV          0x1002  // _IOWR(_IMAPX_DEV_MMU_IOC_MAGIC, 2, struct _imapx_dev_mmu_handle_data)

#define IMAPX_IOC_DEV_MMU_DEINIT_DEV        0x1003  // _IOW(_IMAPX_DEV_MMU_IOC_MAGIC, 3, struct _imapx_dev_mmu_handle_data)

#define IMAPX_IOC_DEV_MMU_ATTACH_DMA_BUF    0x1004  // _IOW(_IMAPX_DEV_MMU_IOC_MAGIC, 4, struct _imapx_dev_mmu_attach_data)

#define IMAPX_IOC_DEV_MMU_DETACH_DMA_BUF    0x1005  // _IOW(_IMAPX_DEV_MMU_IOC_MAGIC, 5, struct _imapx_dev_mmu_attach_data)

#define IMAPX_IOC_DEV_MMU_ENABLE_DEV        0x1006  // _IOW(_IMAPX_DEV_MMU_IOC_MAGIC, 6, struct _imapx_dev_mmu_handle_data)

#define IMAPX_IOC_DEV_MMU_DISABLE_DEV       0x1007  // _IOW(_IMAPX_DEV_MMU_IOC_MAGIC, 7, struct _imapx_dev_mmu_handle_data)

#define IMAPX_IOC_DEV_MMU_RESET_DEV         0x1008  // _IOW(_IMAPX_DEV_MMU_IOC_MAGIC, 8, struct _imapx_dev_mmu_handle_data)


#ifdef __KERNEL__

/**
 * func: map a sg_table to the common devmmu page table.
 * params: 
 *		buf_uid, the uid of dma-buf.
 * return: devmmu address in success, else IMAPX_DEV_ADDR_INVALID.
 **/
imapx_dev_addr_t imapx_dmmu_map(int buf_uid);


/**
 * func: unmap a dma_buf_fd from common page table.
 * params: 
 *		buf_uid, the uid of dma-buf.
 * return: none.
 **/
void imapx_dmmu_unmap(int buf_uid);


/**
 * func: initialize a dmmu device.
 * params: dev, dmmu device id, see IM_devmmuapi.h for details.
 * return: a handle of the dmmu device in success, else NULL will be returned.
 **/
imapx_dmmu_handle_t imapx_dmmu_init(int dev);


/**
 * func: deinitialize the dmmu device.
 * params: dmmu_handle, the handle of the dmmu device.
 * return: 0 success, else failed.
 */
int imapx_dmmu_deinit(imapx_dmmu_handle_t dmmu_handle);


/**
 * func: attach a dmmu device to common page table.
 * params: 
 *		dmmu_handle, the handle of the dmmu device.
 *		buf_uid, the uid of dma-buf.
 * return: dmmu address in success, else IMAPX_DEV_ADDR_INVALID.
 **/
imapx_dev_addr_t imapx_dmmu_attach(imapx_dmmu_handle_t dmmu_handle, int buf_uid);


/**
 * func: detach a dmmu device to common page table.
 * params: 
 *		dmmu_handle, the handle of the dmmu device.
 *		buf_uid, the uid of dma-buf.
 * return: none.
 **/
void imapx_dmmu_detach(imapx_dmmu_handle_t dmmu_handle, int buf_uid);


/**
 * func: operations of dmmu hardware. 
 * params: dmmu_handle, the handle of the dmmu device.
 * return: 0 is successful, else failed.
 */
int imapx_dmmu_enable(imapx_dmmu_handle_t dmmu_handle);
int imapx_dmmu_disable(imapx_dmmu_handle_t dmmu_handle);
int imapx_dmmu_reset(imapx_dmmu_handle_t dmmu_handle);

int imapx_dmmu_buf_mapping_allocate_mapping(void * target);
int imapx_dmmu_buf_mapping_get(int buf_uid, void** target);
int imapx_dmmu_buf_mapping_set(int buf_uid, void * target);
void imapx_dmmu_buf_mapping_free(int buf_uid);

int imapx_dmmu_share_dma_buf_fd(int buf_uid);

#endif /*__KERNEL__*/

#endif	// __IMAPX_DEV_MMU_H__

