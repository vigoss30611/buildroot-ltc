/*
 *
 * Copyright (C) 2011 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*----------------------------------------
 * version log:
 * v2013.12.28: shaft.huang@infotm, first commit, framework of dmmu driver, support mmu operation for dma_buf
 * v2013.12.31: shaft.huang@infotm, release resource
 * v2014.01.04: leo.zhang@infotm, in imapx_dmmu_init(), if handle has been inited, it must call dmmulib_mmu_set_dtable()
 *                              again for the dtable address might be reset to 0 somewhere, it's a temporary processing, we
 *                              found this could happen in video decoding. in addition, add "dev" in some dbg_msg.
 *                          Notes:
 *                              open-issue: map and unmap buffer does not need to update dev's mmu-table, but when attach 
 *                              and detach happend, it should update.
 * v2014.01.06: shaft.huang@infotm, resource managed with different client
 * v2014.01.18: shaft.huang@infotm, dma_buf_fd not unique between processes, use buf_uid instead to map dev_mmu
 * v2014.01.20: shaft.huang@infotm, add map_lock; get phy_addr when buffer is contig
 *
 * ---------------------------------------
 */
#define INFOTM_DMMU_VERSION "v2014.01.20"

#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/dma-buf.h>
#include <linux/scatterlist.h>

#include <mach/devices.h>

#include <InfotmMedia.h>
#include <IM_devmmuapi.h>
#include <dmmu_lib.h>
#include "imapx_dev_mmu.h"
#include "dmmu_buf_descriptor_mapping.h"

#define SIZE_TO_PAGE(size)  ((size) >> PAGE_SHIFT)

struct dmmu_device {
	struct miscdevice dev;
	struct mutex buf_uid_map_lock;
	dmmu_buf_descriptor_mapping * buf_uid_map;
	struct list_head buffers;
	struct list_head mdevices;
	struct list_head attachments;
	struct mutex lock;
    struct mutex map_lock;
	struct list_head clients;
	dmmu_pagetable_t pgt_hnd;
	unsigned int dtablePhyBase;
};

struct dmmu_buffer_descriptor {
	struct list_head buffer;
	struct dmmu_device *dev;
	int buf_uid;
	struct mutex lock;

	struct dma_buf *dmabuf;
	struct dma_buf_attachment *attach;
	struct sg_table *sgt;
	unsigned int page_num;

	unsigned int dev_addr;
	unsigned int phy_addr;

	int ref_client;
	int attach_count;
};

struct dmmu_mdev_descriptor {
	struct list_head mdevice;
	struct dmmu_device *dev;
	struct mutex lock;
	int dev_id;
	bool enabled;
	int	ref_client;
	int attach_count;
};

struct dmmu_attachment_descriptor {
	struct list_head attachment;
	struct dmmu_device *dev;
	struct dmmu_buffer_descriptor *buffer;
	struct dmmu_mdev_descriptor *mdevice;
	struct mutex lock;
	int ref_client;
};

enum dmmu_client_type {
	DMMU_USER_TYPE_SYSTEM,
	DMMU_USER_TYPE_KERNEL,
};

struct dmmu_client {
	struct list_head client;
	struct dmmu_device *dev;
	struct mutex lock;
	enum dmmu_client_type type;
	struct list_head buffers;
	struct list_head mdevices;
	struct list_head attachments;
};

struct client_buffer_copy {
	struct dmmu_client *owner;
	struct dmmu_buffer_descriptor *buffer;
	int count;
	struct list_head buf;
};

struct client_mdevice_copy {
	struct dmmu_client *owner;
	struct dmmu_mdev_descriptor *mdevice;
	int count;
	struct list_head mdev;
};

struct client_attachment_copy {
	struct dmmu_client *owner;
	struct dmmu_attachment_descriptor *attachment;
	int count;
	struct list_head attach;
};

struct dmmu_device *dmmu_infotm;
struct platform_device *infotm_dmmu_platform_device = NULL;

//#define dbg_infotm_dmmu(cond, exp)   if(cond) exp
#define dbg_infotm_dmmu(cond, exp)

/***********************************************************************/
int imapx_dmmu_buf_mapping_allocate_mapping(void * target)
{
	int buf_uid = -1;/*-EFAULT;*/

	if (dmmu_infotm && dmmu_infotm->buf_uid_map)
	{
		mutex_lock(&dmmu_infotm->buf_uid_map_lock);
		buf_uid = dmmu_buf_descriptor_mapping_allocate_mapping(dmmu_infotm->buf_uid_map, target);
		mutex_unlock(&dmmu_infotm->buf_uid_map_lock);
	}

	return buf_uid;
}

int imapx_dmmu_buf_mapping_get(int buf_uid, void** target)
{
	int result = -1;/*-EFAULT;*/

	if (dmmu_infotm && dmmu_infotm->buf_uid_map)
		result = dmmu_buf_descriptor_mapping_get(dmmu_infotm->buf_uid_map, buf_uid, target);

	return result;
}

int imapx_dmmu_buf_mapping_set(int buf_uid, void * target)
{
	int result = -1;/*-EFAULT;*/

	if (dmmu_infotm && dmmu_infotm->buf_uid_map)
		result = dmmu_buf_descriptor_mapping_set(dmmu_infotm->buf_uid_map, buf_uid, target);

	return result;
}

void imapx_dmmu_buf_mapping_free(int buf_uid)
{
	if (dmmu_infotm && dmmu_infotm->buf_uid_map)
		dmmu_buf_descriptor_mapping_free(dmmu_infotm->buf_uid_map, buf_uid);
}
/***********************************************************************/

void infotm_dmmu_client_put_attachment_descriptor(struct dmmu_client *client, struct client_attachment_copy *client_attach);

struct dmmu_buffer_descriptor* get_dmmu_buffer_descriptor(struct dmmu_device *device, int buf_uid)
{
	struct dmmu_buffer_descriptor *buffer_descriptor;
	BUG_ON(!device);
    
    dbg_infotm_dmmu(1, printk("%s(%d)\n", __func__, buf_uid));

	mutex_lock(&device->lock);
	list_for_each_entry(buffer_descriptor, &device->buffers, buffer) {
		if (buffer_descriptor->buf_uid == buf_uid) {
			mutex_unlock(&device->lock);
			return buffer_descriptor;
		}
		else
			continue;
	}
	mutex_unlock(&device->lock);
	return NULL;
}

struct dmmu_mdev_descriptor* get_dmmu_mdev_descriptor(struct dmmu_device *device, int dev_id)
{
	struct dmmu_mdev_descriptor *mdev_descriptor;
	BUG_ON(!device);

    dbg_infotm_dmmu(1, printk("%s(%d)\n", __func__, dev_id));

	mutex_lock(&device->lock);
	list_for_each_entry(mdev_descriptor, &device->mdevices, mdevice) {
		if (mdev_descriptor->dev_id == dev_id) {
			mutex_unlock(&device->lock);
			return mdev_descriptor;
		}
		else
			continue;
	}
	mutex_unlock(&device->lock);
	return NULL;
}

struct dmmu_attachment_descriptor* get_dmmu_attachment_descriptor(struct dmmu_device *device, int buf_uid, int dev_id)
{
	struct dmmu_attachment_descriptor *attachment_descriptor;
	BUG_ON(!device);

	dbg_infotm_dmmu(1, printk("%s(%d, %d)\n", __func__, buf_uid, dev_id));

	mutex_lock(&device->lock);
	list_for_each_entry(attachment_descriptor, &device->attachments, attachment) {
		if (attachment_descriptor->mdevice->dev_id == dev_id && attachment_descriptor->buffer->buf_uid == buf_uid) {
			mutex_unlock(&device->lock);
			return attachment_descriptor;
		}

		continue;
	}
	mutex_unlock(&device->lock);
	return NULL;
}

struct dmmu_buffer_descriptor *infotm_dmmu_buffer_descriptor_create(struct dmmu_device *device, int buf_uid, int dma_buf_fd)
{
	struct dmmu_buffer_descriptor *buffer_descriptor;
	struct dma_buf *dmabuf;
	struct dma_buf_attachment *attachment;
	struct sg_table *sgt;
	struct scatterlist *sg;
	int i;
	int k=0;
	dmmu_pt_map_t ptmap;
	unsigned int pageNum=0;
	unsigned int *pageList = NULL;
	imapx_dev_addr_t dev_addr;
	unsigned int tmp_phys;
	BUG_ON(!device);

    dbg_infotm_dmmu(1, printk("%s(%d)\n", __func__, buf_uid));

	buffer_descriptor = kzalloc(sizeof(struct dmmu_buffer_descriptor), GFP_KERNEL);
	if (!buffer_descriptor) {
		pr_err("Out of memory in %s, line %d\n", __func__, __LINE__);
		return NULL;
	}

	dmabuf = dma_buf_get(dma_buf_fd);
	if (IS_ERR_OR_NULL(dmabuf)) {
		pr_err("Failed to get dma_buf from fd %d\n", buf_uid);
		kfree(buffer_descriptor);
		return NULL;
	}

	attachment = dma_buf_attach(dmabuf, &infotm_dmmu_platform_device->dev);
	if (attachment == NULL)
	{
        pr_err("dma_buf_attach() failed\n");
		dma_buf_put(dmabuf);
		kfree(buffer_descriptor);
		return NULL;
	}

	sgt = dma_buf_map_attachment(attachment, DMA_BIDIRECTIONAL);
	if (IS_ERR_OR_NULL(sgt))
	{
        pr_err("dma_buf_map_attachment() failed\n");
		dma_buf_detach(dmabuf, attachment);
		dma_buf_put(dmabuf);
		kfree(buffer_descriptor);
		return NULL;
	}

	pageNum = SIZE_TO_PAGE(dmabuf->size);
	pageList = kmalloc(sizeof(unsigned int) * pageNum, GFP_KERNEL);
	if (pageList==NULL)
	{
        pr_err("kmalloc(pageList) failed\n");
		dma_buf_detach(dmabuf, attachment);
		dma_buf_put(dmabuf);
		kfree(buffer_descriptor);
		return NULL;
	}

	for_each_sg(sgt->sgl, sg, sgt->nents, i)
	{
		unsigned long size = sg_dma_len(sg);
		dma_addr_t phys = sg_dma_address(sg);
		int j;
		for (j=0; j<SIZE_TO_PAGE(size); j++)
			pageList[k++] = SIZE_TO_PAGE(phys) + j;
		tmp_phys = phys;
	}

	ptmap.pageNum = pageNum;
	ptmap.pageList = pageList;
	if (dmmulib_pagetable_map(device->pgt_hnd, &ptmap, &dev_addr) != 0)
	{
        pr_err("dmmulib_pagetable_map() failed\n");
		kfree(pageList);
		dma_buf_detach(dmabuf, attachment);
		dma_buf_put(dmabuf);
		kfree(buffer_descriptor);
		return NULL;
	}

	buffer_descriptor->dev = device;
	buffer_descriptor->buf_uid = buf_uid;
	buffer_descriptor->dmabuf = dmabuf;
	buffer_descriptor->attach = attachment;
	buffer_descriptor->sgt = sgt;
	buffer_descriptor->page_num = pageNum;
	buffer_descriptor->dev_addr = dev_addr;
	buffer_descriptor->phy_addr = (1==sgt->nents) ? tmp_phys : IMAPX_DEV_ADDR_INVALID;

	buffer_descriptor->attach_count = 0;
	buffer_descriptor->ref_client = 0;

	mutex_init(&buffer_descriptor->lock);
	mutex_lock(&device->lock);
	list_add(&buffer_descriptor->buffer, &device->buffers);
	mutex_unlock(&device->lock);

	kfree(pageList);
    dbg_infotm_dmmu(1, printk("%s got dev_addr=0x%x\n", __func__, dev_addr));
	return buffer_descriptor;
}

void infotm_dmmu_buffer_descriptor_destroy(struct dmmu_device *device, struct dmmu_buffer_descriptor *buffer_descriptor)
{
	BUG_ON(!device || !buffer_descriptor);
	BUG_ON(device != buffer_descriptor->dev);

    dbg_infotm_dmmu(1, printk("%s: buf_uid(%d)\n", __func__, buffer_descriptor->buf_uid));

	// Real unmap operation
	dmmulib_pagetable_unmap(device->pgt_hnd, buffer_descriptor->dev_addr, buffer_descriptor->page_num);

	//TODO: do here or after create?
	{
		dma_buf_unmap_attachment(buffer_descriptor->attach, buffer_descriptor->sgt, DMA_BIDIRECTIONAL);
		dma_buf_detach(buffer_descriptor->dmabuf, buffer_descriptor->attach);
		dma_buf_put(buffer_descriptor->dmabuf);
	}

	mutex_lock(&device->lock);
	list_del(&buffer_descriptor->buffer);
	kfree(buffer_descriptor);
	mutex_unlock(&device->lock);
	return;
}

int infotm_dmmu_client_get_buffer_descriptor(struct dmmu_client *client, struct dmmu_buffer_descriptor *buffer_descriptor)
{
	struct client_buffer_copy *client_buf;
	bool found = false;
	BUG_ON(!client || !buffer_descriptor);
	BUG_ON(client->dev != buffer_descriptor->dev);

    dbg_infotm_dmmu(1, printk("%s(buf_uid=%d)\n", __func__, buffer_descriptor->buf_uid));

	mutex_lock(&client->lock);
	list_for_each_entry(client_buf, &client->buffers, buf) {
		if (client_buf->buffer == buffer_descriptor) {
			found = true;
			break;
		}
		else
			continue;
	}
	mutex_unlock(&client->lock);

	if (found) {
		client_buf->count++;
		return 0;
	}

	client_buf = kzalloc(sizeof(struct client_buffer_copy), GFP_KERNEL);
	if (!client_buf) {
		pr_err("Out of memory in %s, line %d\n", __func__, __LINE__);
		return -1;
	}

	client_buf->owner = client;
	client_buf->buffer = buffer_descriptor;
	client_buf->count = 1;

	mutex_lock(&client->lock);
	list_add(&client_buf->buf, &client->buffers);
	mutex_unlock(&client->lock);

	mutex_lock(&buffer_descriptor->lock);
	buffer_descriptor->ref_client++;
	mutex_unlock(&buffer_descriptor->lock);

	return 0;
}

void infotm_dmmu_client_put_buffer_descriptor(struct dmmu_client *client, struct client_buffer_copy *client_buf)
{
	struct dmmu_buffer_descriptor *buffer_descriptor = client_buf->buffer;
	BUG_ON(!client || !client_buf);
	BUG_ON(client != client_buf->owner);

    dbg_infotm_dmmu(1, printk("%s(buf_uid=%d)\n", __func__, client_buf->buffer->buf_uid));

	mutex_lock(&client->lock);
	list_del(&client_buf->buf);
	mutex_unlock(&client->lock);
	kfree(client_buf);

	mutex_lock(&buffer_descriptor->lock);
	buffer_descriptor->ref_client--;
	mutex_unlock(&buffer_descriptor->lock);

	dbg_infotm_dmmu(1, printk("%s: buffer %d: ref_client=%d\n", __func__, buffer_descriptor->buf_uid, buffer_descriptor->ref_client));

	if (buffer_descriptor->ref_client==0 && buffer_descriptor->attach_count==0) {
		infotm_dmmu_buffer_descriptor_destroy(client->dev, buffer_descriptor);
	}
}

struct dmmu_mdev_descriptor* infotm_dmmu_mdev_descriptor_create(struct dmmu_device *device, int dev_id)
{
	struct dmmu_mdev_descriptor *mdev_descriptor;
	BUG_ON(!device);

    dbg_infotm_dmmu(1, printk("%s(dev_id=%d)\n", __func__, dev_id));

	mdev_descriptor = kzalloc(sizeof(struct dmmu_mdev_descriptor), GFP_KERNEL);
	if (!mdev_descriptor) {
		pr_err("Out of memory in %s, line %d\n", __func__, __LINE__);
		return NULL;
	}

	if (dmmulib_mmu_init(dev_id) != 0) {
        pr_err("dmmulib_mmu_init(%d) failed\n", dev_id);
		kfree(mdev_descriptor);
		return NULL;
	}

	if (dmmulib_mmu_set_dtable(dev_id, device->dtablePhyBase) != 0){
        pr_err("dmmulib_mmu_set_dtable(%d, 0x%x) failed\n", dev_id, device->dtablePhyBase);
		dmmulib_mmu_deinit(dev_id);
		kfree(mdev_descriptor);
		return NULL;
	}

	mdev_descriptor->dev = device;
	mdev_descriptor->dev_id = dev_id;
	mdev_descriptor->enabled = false;
	mdev_descriptor->ref_client = 0;
	mdev_descriptor->attach_count = 0;

	mutex_init(&mdev_descriptor->lock);

	mutex_lock(&device->lock);
	list_add(&mdev_descriptor->mdevice, &device->mdevices);
	mutex_unlock(&device->lock);

	return mdev_descriptor;
}

void infotm_dmmu_mdev_descriptor_destroy(struct dmmu_device *device, struct dmmu_mdev_descriptor *mdev_descriptor)
{
	BUG_ON(!device || !mdev_descriptor);
	BUG_ON(device != mdev_descriptor->dev);

    dbg_infotm_dmmu(1, printk("%s(dev_id=%d)\n", __func__, mdev_descriptor->dev_id));

	dmmulib_mmu_deinit(mdev_descriptor->dev_id);
	mutex_lock(&device->lock);
	list_del(&mdev_descriptor->mdevice);
	kfree(mdev_descriptor);
	mutex_unlock(&device->lock);
	return;
}

struct client_mdevice_copy* infotm_dmmu_client_get_mdev_descriptor(struct dmmu_client *client, struct dmmu_mdev_descriptor *mdev_descriptor)
{
	struct client_mdevice_copy *client_mdev;
	bool found = false;
	BUG_ON(!client || !mdev_descriptor);
	BUG_ON(client->dev != mdev_descriptor->dev);

    dbg_infotm_dmmu(1, printk("%s(dev_id=%d)\n", __func__, mdev_descriptor->dev_id));

	mutex_lock(&client->lock);
	list_for_each_entry(client_mdev, &client->mdevices, mdev) {
		if (client_mdev->mdevice == mdev_descriptor) {
			found = true;
			break;
		}
		else
			continue;
	}
	mutex_unlock(&client->lock);

	if (found) {
		//TODO: it should be report fail when count > 1, in fact
		client_mdev->count++;
		return client_mdev;
	}
	
	client_mdev = kzalloc(sizeof(struct client_mdevice_copy), GFP_KERNEL);
	if (!client_mdev) {
		pr_err("Out of memory in %s, line %d\n", __func__, __LINE__);
		return NULL;
	}

	client_mdev->owner = client;
	client_mdev->mdevice = mdev_descriptor;
	client_mdev->count = 1;

	mutex_lock(&client->lock);
	list_add(&client_mdev->mdev, &client->mdevices);
	mutex_unlock(&client->lock);

	mutex_lock(&mdev_descriptor->lock);
	mdev_descriptor->ref_client++;
	mutex_unlock(&mdev_descriptor->lock);

	return client_mdev;
}

void infotm_dmmu_client_put_mdev_descriptor(struct dmmu_client *client, struct client_mdevice_copy *client_mdev)
{
	struct dmmu_mdev_descriptor *mdev_descriptor = client_mdev->mdevice;
	struct client_attachment_copy *client_attach, *tmp;
	int dev_id;
	BUG_ON(!client || !client_mdev);
	BUG_ON(client != client_mdev->owner);

	dev_id = client_mdev->mdevice->dev_id;
    dbg_infotm_dmmu(1, printk("%s(dev_id=%d)\n", __func__, client_mdev->mdevice->dev_id));

	list_for_each_entry_safe(client_attach, tmp, &client->attachments, attach) {
		if (client_attach->attachment->mdevice == mdev_descriptor)
			infotm_dmmu_client_put_attachment_descriptor(client, client_attach);
		else
			continue;
	}

	mutex_lock(&client->lock);
	list_del(&client_mdev->mdev);
	mutex_unlock(&client->lock);
	kfree(client_mdev);

	mutex_lock(&mdev_descriptor->lock);
	mdev_descriptor->ref_client--;
	mutex_unlock(&mdev_descriptor->lock);

	if (mdev_descriptor->ref_client==0) {
		infotm_dmmu_mdev_descriptor_destroy(client->dev, mdev_descriptor);
	}
}

struct dmmu_attachment_descriptor* infotm_dmmu_attachment_descriptor_create(struct dmmu_device *device, struct dmmu_buffer_descriptor *buffer_descriptor, struct dmmu_mdev_descriptor *mdev_descriptor)
{
	struct dmmu_attachment_descriptor *attachment_descriptor;
	int ret=0;
	BUG_ON(!device || !buffer_descriptor || !mdev_descriptor);
	BUG_ON(device != buffer_descriptor->dev);
	BUG_ON(device != mdev_descriptor->dev);

    dbg_infotm_dmmu(1, printk("%s(buf_uid=%d, dev_id=%d)\n", __func__, buffer_descriptor->buf_uid, mdev_descriptor->dev_id));

	attachment_descriptor = kzalloc(sizeof(struct dmmu_attachment_descriptor), GFP_KERNEL);
	if (!attachment_descriptor) {
		pr_err("Out of memory in %s, line %d\n", __func__, __LINE__);
		return NULL;
	}

	if (mdev_descriptor->enabled)
		ret = dmmulib_mmu_update_ptable(mdev_descriptor->dev_id, buffer_descriptor->dev_addr, buffer_descriptor->page_num);

	if ( ret!=0 ) {
		pr_err("mmu ptable update failed in %s\n", __func__);
		kfree(attachment_descriptor);
		return NULL;
	}

	attachment_descriptor->dev = device;
	attachment_descriptor->buffer = buffer_descriptor;
	attachment_descriptor->mdevice = mdev_descriptor;
	attachment_descriptor->ref_client = 0;

	mutex_init(&attachment_descriptor->lock);

	mutex_lock(&device->lock);
	list_add(&attachment_descriptor->attachment, &device->attachments);
	mutex_unlock(&device->lock);

	mutex_lock(&buffer_descriptor->lock);
	buffer_descriptor->attach_count++;
	mutex_unlock(&buffer_descriptor->lock);

	mutex_lock(&mdev_descriptor->lock);
	mdev_descriptor->attach_count++;
	mutex_unlock(&mdev_descriptor->lock);

	return attachment_descriptor;
}

void infotm_dmmu_attachment_descriptor_destroy(struct dmmu_device *device, struct dmmu_attachment_descriptor *attachment_descriptor)
{
	struct dmmu_mdev_descriptor *mdev_descriptor = attachment_descriptor->mdevice;
	struct dmmu_buffer_descriptor *buffer_descriptor = attachment_descriptor->buffer;
	int ret=0;
	BUG_ON(!device || !attachment_descriptor);
	BUG_ON(device != attachment_descriptor->dev);

    dbg_infotm_dmmu(1, printk("%s(buf_uid=%d, dev_id=%d)\n", __func__, buffer_descriptor->buf_uid, mdev_descriptor->dev_id));

	if (mdev_descriptor->enabled)
		ret = dmmulib_mmu_update_ptable(mdev_descriptor->dev_id, buffer_descriptor->dev_addr, buffer_descriptor->page_num);

	mutex_lock(&device->lock);
	list_del(&attachment_descriptor->attachment);
	mutex_unlock(&device->lock);

	mutex_lock(&mdev_descriptor->lock);
	mdev_descriptor->attach_count--;
	mutex_unlock(&mdev_descriptor->lock);

	mutex_lock(&buffer_descriptor->lock);
	buffer_descriptor->attach_count--;
	mutex_unlock(&buffer_descriptor->lock);

	if (buffer_descriptor->attach_count==0 && buffer_descriptor->ref_client==0) {
		infotm_dmmu_buffer_descriptor_destroy(device, buffer_descriptor);
	}
	
	kfree(attachment_descriptor);
}

int infotm_dmmu_client_get_attachment_descriptor(struct dmmu_client *client, struct dmmu_attachment_descriptor *attachment_descriptor)
{
	struct client_attachment_copy *client_attach;
	bool found = false;
	BUG_ON(!client || !attachment_descriptor);
	BUG_ON(client->dev != attachment_descriptor->dev);

    dbg_infotm_dmmu(1, printk("%s(buf_uid=%d, dev_id=%d)\n", __func__, attachment_descriptor->buffer->buf_uid, attachment_descriptor->mdevice->dev_id));

	mutex_lock(&client->lock);
	list_for_each_entry(client_attach, &client->attachments, attach) {
		if (client_attach->attachment == attachment_descriptor) {
			found = true;
			break;
		}
		else
			continue;
	}
	mutex_unlock(&client->lock);

	if (found) {
		client_attach->count++;
		return 0;
	}
	
	client_attach = kzalloc(sizeof(struct client_attachment_copy), GFP_KERNEL);
	if (!client_attach) {
		pr_err("Out of memory in %s, line %d\n", __func__, __LINE__);
		return -1;
	}

	client_attach->owner = client;
	client_attach->attachment = attachment_descriptor;
	client_attach->count = 1;

	mutex_lock(&client->lock);
	list_add(&client_attach->attach, &client->attachments);
	mutex_unlock(&client->lock);

	mutex_lock(&attachment_descriptor->lock);
	attachment_descriptor->ref_client++;
	mutex_unlock(&attachment_descriptor->lock);

	return 0;
}

void infotm_dmmu_client_put_attachment_descriptor(struct dmmu_client *client, struct client_attachment_copy *client_attach)
{
	struct dmmu_attachment_descriptor *attachment_descriptor = client_attach->attachment;

	BUG_ON(!client || !client_attach);
	BUG_ON(client != client_attach->owner);

    dbg_infotm_dmmu(1, printk("%s(buf_uid=%d, dev_id=%d)\n", __func__, client_attach->attachment->buffer->buf_uid, client_attach->attachment->mdevice->dev_id));

	mutex_lock(&client->lock);
	list_del(&client_attach->attach);
	mutex_unlock(&client->lock);
	kfree(client_attach);

	mutex_lock(&attachment_descriptor->lock);
	attachment_descriptor->ref_client--;
	mutex_unlock(&attachment_descriptor->lock);

	if (attachment_descriptor->ref_client==0) {
		infotm_dmmu_attachment_descriptor_destroy(client->dev, attachment_descriptor);
	}
}

/**
 * func: map a sg_table to the common devmmu page table.
 * params: 
 *		buf_uid, the fd of dma-buf.
 * return: devmmu address in success, else IMAPX_DEV_ADDR_INVALID.
 **/
imapx_dev_addr_t _imapx_dmmu_map(struct dmmu_client *client, int buf_uid, int *dma_buf_fd, int *size, unsigned int *phy_addr) 
{
	struct dmmu_device *dev = client->dev;
	struct dmmu_buffer_descriptor *buffer_descriptor;
	int ret;
	bool new_map = false;
	int share_fd;
	BUG_ON(!client);

    dbg_infotm_dmmu(1, printk("%s(buf_uid=%d)\n", __func__, buf_uid));

	share_fd = imapx_dmmu_share_dma_buf_fd(buf_uid);
    mutex_lock(&dev->map_lock);
	buffer_descriptor = get_dmmu_buffer_descriptor(dev, buf_uid);
	if (NULL == buffer_descriptor) {
		new_map = true;
		buffer_descriptor = infotm_dmmu_buffer_descriptor_create(dev, buf_uid, share_fd);
	}
	*dma_buf_fd = share_fd;

	if (NULL != buffer_descriptor) {
		ret = infotm_dmmu_client_get_buffer_descriptor(client, buffer_descriptor);
		if (ret==0) {
            *size = buffer_descriptor->dmabuf->size;
			*phy_addr = buffer_descriptor->phy_addr;
            mutex_unlock(&dev->map_lock);
			return buffer_descriptor->dev_addr;
        }

		if (new_map) {
			infotm_dmmu_buffer_descriptor_destroy(dev, buffer_descriptor);
		}
	}
    mutex_unlock(&dev->map_lock);

	return IMAPX_DEV_ADDR_INVALID;
}

/**
 * func: unmap a dma_buf from common page table.
 * params: 
 *		buf_uid, the fd of dma-buf.
 * return: none.
 **/
void _imapx_dmmu_unmap(struct dmmu_client *client, int buf_uid)
{
	struct client_buffer_copy *client_buf;
	bool found = false;
	BUG_ON(!client);

    dbg_infotm_dmmu(1, printk("imapx_dmmu_unmap(%d)\n", buf_uid));

	mutex_lock(&client->lock);
	list_for_each_entry(client_buf, &client->buffers, buf) {
		if (client_buf->buffer->buf_uid == buf_uid) {
			found = true;
			break;
		}
		else
			continue;
	}
	mutex_unlock(&client->lock);

    mutex_lock(&client->dev->map_lock);
	if (found) {
		client_buf->count--;

		if (client_buf->count==0) 
			infotm_dmmu_client_put_buffer_descriptor(client, client_buf);
	}
    mutex_unlock(&client->dev->map_lock);

	return;
}

/**
 * func: initialize a dmmu device.
 * params: dev, dmmu device, see IM_devmmuapi.h for details.
 * return: a handle of the dmmu device in success, else NULL will be returned.
 **/
imapx_dmmu_handle_t _imapx_dmmu_init(struct dmmu_client *client, int dev_id)
{
	struct dmmu_device *device = client->dev;
	struct dmmu_mdev_descriptor *mdev_descriptor;
	bool new_mdev = false;
	struct client_mdevice_copy *client_mdev;
	BUG_ON(!client);

    dbg_infotm_dmmu(1, printk("%s(dev_id=%d)\n", __func__, dev_id));

    mutex_lock(&device->map_lock);
	mdev_descriptor = get_dmmu_mdev_descriptor(device, dev_id);
	if (NULL == mdev_descriptor) {
		new_mdev = true;
		mdev_descriptor = infotm_dmmu_mdev_descriptor_create(device, dev_id);
	}

	if (NULL != mdev_descriptor) {
		client_mdev = infotm_dmmu_client_get_mdev_descriptor(client, mdev_descriptor);
		if (NULL!=client_mdev) {
			dmmulib_mmu_set_dtable(dev_id, device->dtablePhyBase); //TODO: remove this
            mutex_unlock(&device->map_lock);
			return (imapx_dmmu_handle_t)client_mdev;
		}

		if (new_mdev) {
			infotm_dmmu_mdev_descriptor_destroy(device, mdev_descriptor);
		}
	}
	mutex_unlock(&device->map_lock);

	return NULL;
}

/**
 * func: deinitialize the dmmu device.
 * params: dmmu_handle, the handle of the dmmu device.
 * return: 0 success, else failed.
 */
int _imapx_dmmu_deinit(struct dmmu_client *client, imapx_dmmu_handle_t dmmu_handle)
{
	struct client_mdevice_copy *client_mdev;
	struct dmmu_mdev_descriptor *mdev_descriptor;
	int dev_id;
	BUG_ON(!client);

	client_mdev = (struct client_mdevice_copy *)dmmu_handle;
	if (client_mdev==NULL)
		return -1;

	mdev_descriptor=client_mdev->mdevice;
	dev_id = mdev_descriptor->dev_id;

    dbg_infotm_dmmu(1, printk("%s(dev_id=%d)\n", __func__, dev_id));

    mutex_lock(&client->dev->map_lock);
	client_mdev->count--;
	if (client_mdev->count==0) {
		infotm_dmmu_client_put_mdev_descriptor(client, client_mdev);
	}
    mutex_unlock(&client->dev->map_lock);

	return 0;
}

/**
 * func: attach a dmmu device to common page table.
 * params: 
 *		dmmu_handle, the handle of the dmmu device.
 *		buf_uid, the fd of dma-buf.
 * return: dmmu address in success, else IMAPX_DEV_ADDR_INVALID.
 **/
imapx_dev_addr_t _imapx_dmmu_attach(struct dmmu_client *client, imapx_dmmu_handle_t dmmu_handle, int buf_uid)
{
	struct client_mdevice_copy *client_mdev = (struct client_mdevice_copy *)dmmu_handle;
	struct dmmu_device *device = client->dev;
	struct dmmu_mdev_descriptor *mdev_descriptor;
	struct dmmu_buffer_descriptor *buffer_descriptor;
	struct dmmu_attachment_descriptor *attachment_descriptor;
	int ret;
	int dev_id;
	bool new_attachment = false;
	BUG_ON(!client);

    dbg_infotm_dmmu(1, printk("imapx_dmmu_attach(%d)\n", buf_uid));

	if (NULL==client_mdev)
		return IMAPX_DEV_ADDR_INVALID;

	mdev_descriptor = client_mdev->mdevice;
	dev_id = mdev_descriptor->dev_id;

	buffer_descriptor = get_dmmu_buffer_descriptor(device, buf_uid);
	if (!buffer_descriptor) {
		pr_err("dma_buf %d had not been mapped in devmmu\n", buf_uid);
		return IMAPX_DEV_ADDR_INVALID;
	}

    mutex_lock(&device->map_lock);
	attachment_descriptor = get_dmmu_attachment_descriptor(device, buf_uid, dev_id); 
	if (NULL == attachment_descriptor) {
		new_attachment = true;
		attachment_descriptor = infotm_dmmu_attachment_descriptor_create(device, buffer_descriptor, mdev_descriptor);
	}

	if (NULL != attachment_descriptor) {
		ret = infotm_dmmu_client_get_attachment_descriptor(client, attachment_descriptor);
		if (ret==0) {
            mutex_unlock(&device->map_lock);
			return buffer_descriptor->dev_addr;
		}

		if (new_attachment) {
			infotm_dmmu_attachment_descriptor_destroy(device, attachment_descriptor);
		}
	}
	mutex_unlock(&device->map_lock);

	return IMAPX_DEV_ADDR_INVALID;
}

/**
 * func: detach a dmmu device to common page table.
 * params: 
 *		dmmu_handle, the handle of the dmmu device.
 *		buf_uid, the fd of dma-buf.
 * return: none.
 **/
void _imapx_dmmu_detach(struct dmmu_client *client, imapx_dmmu_handle_t dmmu_handle, int buf_uid)
{
	struct client_mdevice_copy *client_mdev = (struct client_mdevice_copy *)dmmu_handle;
	struct dmmu_device *device = client->dev;
	struct dmmu_mdev_descriptor *mdev_descriptor;
	struct dmmu_buffer_descriptor *buffer_descriptor;
	struct dmmu_attachment_descriptor *attachment_descriptor;
	int dev_id;
	struct client_attachment_copy *client_attach;
	bool found = false;
	BUG_ON(!client);

	if (NULL==client_mdev)
		return;

	mdev_descriptor = client_mdev->mdevice;
	dev_id = mdev_descriptor->dev_id;

    dbg_infotm_dmmu(1, printk("imapx_dmmu_detatch(%d)\n", buf_uid));

	buffer_descriptor = get_dmmu_buffer_descriptor(device, buf_uid);
	if (!buffer_descriptor) {
		pr_err("dma_buf %d had not been mapped in devmmu\n", buf_uid);
		return;
	}

	attachment_descriptor = get_dmmu_attachment_descriptor(device, buf_uid, dev_id);
	if (!attachment_descriptor) {
		pr_err("attachment (buf_uid=%d, dev_id=%d) had not been made\n", buf_uid, dev_id);
		return;
	}

	mutex_lock(&client->lock);
	list_for_each_entry(client_attach, &client->attachments, attach) {
		if (client_attach->attachment == attachment_descriptor) {
			found = true;
			break;
		}
		else
			continue;
	}
	mutex_unlock(&client->lock);

    mutex_lock(&device->map_lock);
	if (found) {
		client_attach->count--;

		if (client_attach->count==0)
			infotm_dmmu_client_put_attachment_descriptor(client, client_attach);
	}
    mutex_unlock(&device->map_lock);

	return;
}

/**
 * func: operations of dmmu hardware. 
 * params: dmmu_handle, the handle of the dmmu device.
 * return: 0 is successful, else failed.
 */
int _imapx_dmmu_enable(struct dmmu_client *client, imapx_dmmu_handle_t dmmu_handle)
{
	struct client_mdevice_copy *client_mdev;
	struct dmmu_mdev_descriptor *mdev_descriptor;
	int dev_id;
	BUG_ON(!client);

	client_mdev = (struct client_mdevice_copy *)dmmu_handle;
	if (client_mdev==NULL)
		return -1;

	mdev_descriptor = client_mdev->mdevice;
	dev_id = mdev_descriptor->dev_id;

    dbg_infotm_dmmu(1, printk("%s: dev %d\n", __func__, dev_id));

	if (mdev_descriptor->enabled)
		return 0;

	mutex_lock(&mdev_descriptor->lock);
	if (dmmulib_mmu_enable_paging(dev_id) != IM_RET_OK) {
        pr_err("dmmulib_mmu_enable_paging(%d) failed\n", dev_id);
		mutex_unlock(&mdev_descriptor->lock);
		return -1;
	}
	mdev_descriptor->enabled = true;
	mutex_unlock(&mdev_descriptor->lock);
	return 0;
}

int _imapx_dmmu_disable(struct dmmu_client *client, imapx_dmmu_handle_t dmmu_handle)
{
	struct client_mdevice_copy *client_mdev;
	struct dmmu_mdev_descriptor *mdev_descriptor;
	struct dmmu_device *device = client->dev;
	int dev_id;
	BUG_ON(!client);

	client_mdev = (struct client_mdevice_copy *)dmmu_handle;
	if (client_mdev==NULL)
		return -1;

	mdev_descriptor = client_mdev->mdevice;
	dev_id = mdev_descriptor->dev_id;

    dbg_infotm_dmmu(1, printk("%s: dev %d\n", __func__, dev_id));

	if (!mdev_descriptor->enabled)
		return 0;

	mutex_lock(&mdev_descriptor->lock);
	if (dmmulib_mmu_disable_paging(dev_id) != IM_RET_OK) {
        pr_err("dmmulib_mmu_disable_paging(%d) failed\n", dev_id);
		if (dmmulib_mmu_raw_reset(dev_id) != IM_RET_OK) {
            pr_err("dmmulib_mmu_raw_reset(%d) failed\n", dev_id);
			mutex_unlock(&mdev_descriptor->lock);
			return -1;
		}
		if (dmmulib_mmu_set_dtable(dev_id, device->dtablePhyBase) != IM_RET_OK) {
            pr_err("dmmulib_mmu_set_dtable(%d) failed\n", dev_id);
			mutex_unlock(&mdev_descriptor->lock);
			return -1;
		}
	}
	mdev_descriptor->enabled = false;
	mutex_unlock(&mdev_descriptor->lock);
	return 0;
}

int _imapx_dmmu_reset(struct dmmu_client *client, imapx_dmmu_handle_t dmmu_handle)
{
	struct client_mdevice_copy *client_mdev;
	struct dmmu_mdev_descriptor *mdev_descriptor;
	struct dmmu_device *device = client->dev;
	int dev_id;
	BUG_ON(!client);

    dbg_infotm_dmmu(1, printk("imapx_dmmu_reset()\n"));

	client_mdev = (struct client_mdevice_copy *)dmmu_handle;
	if (client_mdev==NULL)
		return -1;

	mdev_descriptor = client_mdev->mdevice;
	dev_id = mdev_descriptor->dev_id;

    dbg_infotm_dmmu(1, printk("%s: dev %d\n", __func__, dev_id));

	if (!mdev_descriptor->enabled)
		return 0;

	mutex_lock(&mdev_descriptor->lock);
	if (mdev_descriptor->enabled)
		dmmulib_mmu_disable_paging(dev_id);

	if (dmmulib_mmu_raw_reset(dev_id) != IM_RET_OK) {
		mutex_unlock(&mdev_descriptor->lock);
		return -1;
	}

	dmmulib_mmu_set_dtable(dev_id, device->dtablePhyBase);
	mdev_descriptor->enabled = false;
	mutex_unlock(&mdev_descriptor->lock);
	return 0;
}

static long infotm_dmmu_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct dmmu_client *client = filp->private_data;

    dbg_infotm_dmmu(1, printk("infotm_dmmu_ioctl(%d)\n", cmd));

	switch (cmd) {
	case IMAPX_IOC_DEV_MMU_MAP_DMA_BUF:
		{
			struct _imapx_dev_mmu_mapping_dma_buf_data data;
			imapx_dev_addr_t dev_addr;
			int dma_buf_fd;
			int size;
			unsigned int phy_addr;

			if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
				return -EFAULT;
			dev_addr = _imapx_dmmu_map(client, data.buf_uid, &dma_buf_fd, &size, &phy_addr);
			data.dev_address = dev_addr;
			data.dma_buf_fd = dma_buf_fd;
			data.size = size; 
			data.phy_addr = phy_addr;

			if (copy_to_user((void __user *)arg, &data, sizeof(data))) {
				_imapx_dmmu_unmap(client, data.buf_uid);
				return -EFAULT;
			}
		}
		break;

	case IMAPX_IOC_DEV_MMU_UNMAP_DMA_BUF:
		{
			struct _imapx_dev_mmu_mapping_dma_buf_data data;
			if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
				return -EFAULT;

			_imapx_dmmu_unmap(client, data.buf_uid);
			data.dev_address = IMAPX_DEV_ADDR_INVALID;

			if (copy_to_user((void __user *)arg, &data, sizeof(data))) {
				return -EFAULT;
			}
		}
		break;

	case IMAPX_IOC_DEV_MMU_INIT_DEV:
		{
			struct _imapx_dev_mmu_handle_data data;
			imapx_dmmu_handle_t handle;

			if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
				return -EFAULT;

			handle = _imapx_dmmu_init(client, data.dev_id);
			data.handle = handle;

			if (copy_to_user((void __user *)arg, &data, sizeof(data))) {
				return -EFAULT;
			}
		}
		break;

	case IMAPX_IOC_DEV_MMU_DEINIT_DEV:
		{
			struct _imapx_dev_mmu_handle_data data;
			imapx_dmmu_handle_t handle;

			if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
				return -EFAULT;

			handle = data.handle;
			_imapx_dmmu_deinit(client, handle);
		}
		break;

	case IMAPX_IOC_DEV_MMU_ATTACH_DMA_BUF:
		{
			struct _imapx_dev_mmu_attach_data data;
			imapx_dmmu_handle_t handle;
			imapx_dev_addr_t dev_addr;

			if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
				return -EFAULT;

			handle = data.handle;
			dev_addr = _imapx_dmmu_attach(client, handle, data.buf_uid);
			if (dev_addr == IMAPX_DEV_ADDR_INVALID)
				return -EFAULT;
		}
		break;

	case IMAPX_IOC_DEV_MMU_DETACH_DMA_BUF:
		{
			struct _imapx_dev_mmu_attach_data data;
			imapx_dmmu_handle_t handle;

			if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
				return -EFAULT;

			handle = data.handle;
			_imapx_dmmu_detach(client, handle, data.buf_uid);
		}
		break;

	case IMAPX_IOC_DEV_MMU_ENABLE_DEV:
		{
			struct _imapx_dev_mmu_handle_data data;
			imapx_dmmu_handle_t handle;

			if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
				return -EFAULT;

			handle = data.handle;
			
			_imapx_dmmu_enable(client, handle);
		}
		break;

	case IMAPX_IOC_DEV_MMU_DISABLE_DEV:
		{
			struct _imapx_dev_mmu_handle_data data;
			imapx_dmmu_handle_t handle;

			if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
				return -EFAULT;

			handle = data.handle;
			
			_imapx_dmmu_disable(client, handle);
		}
		break;

	case IMAPX_IOC_DEV_MMU_RESET_DEV:
		{
			struct _imapx_dev_mmu_handle_data data;
			imapx_dmmu_handle_t handle;

			if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
				return -EFAULT;

			handle = data.handle;
			
			_imapx_dmmu_reset(client, handle);
		}
		break;

	default:
		return -EFAULT;
	}
	return 0;
}

struct dmmu_client *infotm_dmmu_client_create(struct dmmu_device *dev, enum dmmu_client_type type)
{
	struct dmmu_client *client;

    dbg_infotm_dmmu(1, printk("%s)\n", __func__));

	client = kzalloc(sizeof(struct dmmu_client), GFP_KERNEL);
	if (!client) {
		return ERR_PTR(-ENOMEM);
	}

	client->dev = dev;
	client->type = type;
	mutex_init(&client->lock);
	INIT_LIST_HEAD(&client->buffers);
	INIT_LIST_HEAD(&client->mdevices);
	INIT_LIST_HEAD(&client->attachments);

	mutex_lock(&dev->lock);
	list_add(&client->client, &dev->clients);
	mutex_unlock(&dev->lock);

	return client;
}

void infotm_dmmu_client_destroy(struct dmmu_client *client)
{
	struct dmmu_device *dev = client->dev;
	struct client_attachment_copy *client_attach, *tmp_attach;
	struct client_mdevice_copy *client_mdev, *tmp_mdev;
	struct client_buffer_copy *client_buf, *tmp_buf;
	BUG_ON(!client);

    dbg_infotm_dmmu(1, printk("%s)\n", __func__));

	list_for_each_entry_safe(client_attach, tmp_attach, &client->attachments, attach) {
		infotm_dmmu_client_put_attachment_descriptor(client, client_attach);
	}

	list_for_each_entry_safe(client_mdev, tmp_mdev, &client->mdevices, mdev) {
		infotm_dmmu_client_put_mdev_descriptor(client, client_mdev);
	}

	list_for_each_entry_safe(client_buf, tmp_buf, &client->buffers, buf) {
		infotm_dmmu_client_put_buffer_descriptor(client, client_buf);
	}

	mutex_lock(&dev->lock);
	list_del(&client->client);
	mutex_unlock(&dev->lock);

	kfree(client);
}

struct dmmu_client *infotm_kernel_client = NULL;

inline struct dmmu_client *get_kernel_user_client(void)
{
	if (!infotm_kernel_client)
		pr_err("dmmu_client for kernel user is destroyed\n");

	return infotm_kernel_client;
}
	
imapx_dev_addr_t imapx_dmmu_map(int buf_uid) 
{
	struct dmmu_client *client = get_kernel_user_client();
	int dma_buf_fd;
	int size;
	unsigned int phy_addr;

	if (!client)
		return IMAPX_DEV_ADDR_INVALID;

	return _imapx_dmmu_map(client, buf_uid, &dma_buf_fd, &size, &phy_addr);
}

void imapx_dmmu_unmap(int buf_uid)
{
	struct dmmu_client *client = get_kernel_user_client();

	if (!client)
		return;

	_imapx_dmmu_unmap(client, buf_uid);
	return;
}

imapx_dmmu_handle_t imapx_dmmu_init(int dev_id)
{
	struct dmmu_client *client = get_kernel_user_client();

	if (!client)
		return NULL;

	return _imapx_dmmu_init(client, dev_id);
}

int imapx_dmmu_deinit(imapx_dmmu_handle_t dmmu_handle)
{
	struct dmmu_client *client = get_kernel_user_client();

	if (!client)
		return -1;

	return _imapx_dmmu_deinit(client, dmmu_handle);
}

imapx_dev_addr_t imapx_dmmu_attach(imapx_dmmu_handle_t dmmu_handle, int buf_uid)
{
	struct dmmu_client *client = get_kernel_user_client();

	if (!client)
		return IMAPX_DEV_ADDR_INVALID;

	return _imapx_dmmu_attach(client, dmmu_handle, buf_uid);
}

void imapx_dmmu_detach(imapx_dmmu_handle_t dmmu_handle, int buf_uid)
{
	struct dmmu_client *client = get_kernel_user_client();

	if (!client)
		return;

	_imapx_dmmu_detach(client, dmmu_handle, buf_uid);
	return;
}

int imapx_dmmu_enable(imapx_dmmu_handle_t dmmu_handle)
{
	struct dmmu_client *client = get_kernel_user_client();

	if (!client)
		return -1;

	return _imapx_dmmu_enable(client, dmmu_handle);
}

int imapx_dmmu_disable(imapx_dmmu_handle_t dmmu_handle)
{
	struct dmmu_client *client = get_kernel_user_client();

	if (!client)
		return -1;

	return _imapx_dmmu_disable(client, dmmu_handle);
}

int imapx_dmmu_reset(imapx_dmmu_handle_t dmmu_handle)
{
	struct dmmu_client *client = get_kernel_user_client();

	if (!client)
		return -1;

	return _imapx_dmmu_reset(client, dmmu_handle);
}

static int infotm_dmmu_open(struct inode *inode, struct file *file)
{
	struct miscdevice *miscdev = file->private_data;
	struct dmmu_device *dev = container_of(miscdev, struct dmmu_device, dev);
	struct dmmu_client *client;

	client = infotm_dmmu_client_create(dev, DMMU_USER_TYPE_SYSTEM);
	if (IS_ERR(client))
		return PTR_ERR(client);
	file->private_data = client;

	return 0;
}

static int infotm_dmmu_release(struct inode *inode, struct file *file)
{
	struct dmmu_client *client = file->private_data;
	infotm_dmmu_client_destroy(client);
	return 0;
}

static const struct file_operations infotm_dmmu_fops = {
	.owner          = THIS_MODULE,
	.open           = infotm_dmmu_open,
	.release        = infotm_dmmu_release,
	.unlocked_ioctl = infotm_dmmu_ioctl,
};

struct dmmu_device *infotm_dmmu_device_create(void)
{
	struct dmmu_device *idev;
	dmmu_buf_descriptor_mapping * buf_uid_map;
	int ret;

	idev = kzalloc(sizeof(struct dmmu_device), GFP_KERNEL);
	if (!idev)
		return ERR_PTR(-ENOMEM);

	idev->dev.minor = MISC_DYNAMIC_MINOR;
	idev->dev.name = "dmmu";
	idev->dev.fops = &infotm_dmmu_fops;
	idev->dev.parent = NULL;
	ret = misc_register(&idev->dev);
	if (ret) {
		pr_err("infotm dmmu failed to register misc device.\n");
		return ERR_PTR(ret);
	}
	
	buf_uid_map = dmmu_buf_descriptor_mapping_create(32, 4096);
	if (NULL == buf_uid_map) {
		kfree(idev);
		return ERR_PTR(-ENOMEM);
	}

	ret = dmmulib_pagetable_init(&idev->pgt_hnd, &idev->dtablePhyBase);
	if (ret) {
		pr_err("infotm dmmu failed in dmmulib_pagetable_init()\n");
		dmmu_buf_descriptor_mapping_destroy(buf_uid_map);
		kfree(idev);
		return ERR_PTR(ret);
	}

	idev->buf_uid_map = buf_uid_map;
	mutex_init(&idev->buf_uid_map_lock);
	mutex_init(&idev->lock);
    mutex_init(&idev->map_lock);

	INIT_LIST_HEAD(&idev->buffers);
	INIT_LIST_HEAD(&idev->mdevices);
	INIT_LIST_HEAD(&idev->attachments);
	INIT_LIST_HEAD(&idev->clients);

	return idev;
}

void infotm_dmmu_device_destroy(struct dmmu_device *dev)
{
	struct dmmu_client *d_client;

	list_for_each_entry(d_client, &dev->clients, client) {
		infotm_dmmu_client_destroy(d_client);
	}

	misc_deregister(&dev->dev);
	dmmu_buf_descriptor_mapping_destroy(dev->buf_uid_map);
	dmmulib_pagetable_deinit(dev->pgt_hnd);
	kfree(dev);
}

static int infotm_dmmu_probe(struct platform_device *pdev)
{
	dbg_infotm_dmmu(1, printk("infotm_dmmu_probe!\n"));

	dmmu_infotm = infotm_dmmu_device_create();
	if (IS_ERR_OR_NULL(dmmu_infotm)) {
		return PTR_ERR(dmmu_infotm);
	}

	platform_set_drvdata(pdev, dmmu_infotm);
	infotm_dmmu_platform_device = pdev;
	infotm_kernel_client = infotm_dmmu_client_create(dmmu_infotm, DMMU_USER_TYPE_KERNEL);

	printk("Infotm dmmu device create OK! version %s\n", INFOTM_DMMU_VERSION);
	return 0;
}

int infotm_dmmu_remove(struct platform_device *pdev)
{
	struct dmmu_device *idev = platform_get_drvdata(pdev);
	
	if (infotm_kernel_client)
		infotm_dmmu_client_destroy(infotm_kernel_client);
	infotm_kernel_client = NULL;

	infotm_dmmu_device_destroy(idev);
	infotm_dmmu_platform_device = NULL;
	return 0;
}

static struct platform_driver infotm_dmmu_driver = {
	.probe = infotm_dmmu_probe,
	.remove = infotm_dmmu_remove,
	.driver = { .name = "infotm-dmmu" }
};

static int __init infotm_dmmu_init(void)
{
	return platform_driver_register(&infotm_dmmu_driver);
}

static void __exit infotm_dmmu_exit(void)
{
	platform_driver_unregister(&infotm_dmmu_driver);
}

module_init(infotm_dmmu_init);
module_exit(infotm_dmmu_exit);

