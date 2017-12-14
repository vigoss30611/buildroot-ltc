/*****************************************************************************
** Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** Description: Frame Ring, ring-buffer of frames management.
**
** Author:
**     warits <warits.wang@infotm.com>
**
** Revision History:
** -----------------
** 1.1  02/20/2016
*****************************************************************************/

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/spinlock.h>
#include <linux/dma-mapping.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/fr.h>

#include <asm/ioctl.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>

#include <uapi/linux/fr-user.h>

#define fr_log(x...) printk(KERN_ERR "fr-user: " x)

static struct device* g_fr_dev;

static struct fr_priv_data priv_data;

static inline void
fr_copy_buf_info(struct fr_buf_info *info, struct fr_buf *buf)
{
    info->buf = buf;
    info->phys_addr = buf->phys_addr;
    info->timestamp = buf->timestamp;
    info->size = buf->size;
    info->serial = buf->serial;
    info->priv = buf->priv;
    info->map_size = buf->map_size;
    info->stAttr.s32Width = buf->s32Width;
    info->stAttr.s32Height = buf->s32Height;
    info->u32BasePhyAddr = buf->u32BasePhyAddr;
    info->s32TotalSize = buf->s32TotalSize;
}

#define fr_assert_info(p) do { \
	if(!virt_addr_valid(p) || ((uint32_t)p & 0x3)) {  \
	fr_log("wrong handle: %p, please check your app(%s)\n",  \
		p, current->comm);  \
	return FR_EHANDLE; } } while(0)

static long
fr_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct fr_info fr_info;
	struct fr_buf_info fr_buf_info;
	struct fr_buf *buf;
	struct fr *fr;
	int ret;

//    fr_log("enter func %s at line %d \n", __func__, __LINE__);
//	fr_log("cmd_nr: %d\n", _IOC_NR(cmd));
	if(_IOC_TYPE(cmd) != FRING_MAGIC
	   || _IOC_NR(cmd) > FRING_IOCMAX)
	  return -ENOTTY;

	switch(cmd)
	{
		case FRING_GET_VER:
			return FRING_VERSION;
		case FRING_GET_FR:
			if(copy_from_user(&fr_info, (void __user *)arg, sizeof(fr_info)))
				return -EFAULT;

			fr = fr_get_fr_by_name(fr_info.name);
			if(!fr) return -1;

			fr_info.fr = fr;
			fr_info.flag =fr->flag;
			fr_info.u32BasePhyAddr = fr->ring->phys_addr;
			if(copy_to_user((void __user *)arg, &fr_info, sizeof(fr_info)))
				return -EFAULT;
			return 0;

		case FRING_ALLOC:
			if(copy_from_user(&fr_info, (void __user *)arg, sizeof(fr_info)))
				return -EFAULT;

			fr = __fr_alloc(fr_info.name, fr_info.size, fr_info.flag);
			if(fr) fr->fp = file;

			fr_info.fr = fr;
			if(fr)
			    fr_info.u32BasePhyAddr = fr->ring->phys_addr;
			if(copy_to_user((void __user *)arg, &fr_info, sizeof(fr_info)))
				return -EFAULT;
			return fr? 0: FR_ENOBUFFER;
		case FRING_SET_FLOATMAX:
			if(copy_from_user(&fr_info, (void __user *)arg, sizeof(fr_info)))
				return -EFAULT;

			fr = fr_info.fr;
			return fr_float_setmax(fr, fr_info.float_max);
		case FRING_FREE:
			fr_assert_info((void *)arg);
			return __fr_free((struct fr *)arg);

		case FRING_GET_BUF:
			if(copy_from_user(&fr_buf_info, (void __user *)arg,
						sizeof(fr_buf_info)))
				return -EFAULT;
			fr_assert_info(fr_buf_info.fr);
			ret = fr_get_buf(fr_buf_info.fr, &buf);
			if(ret < 0) return ret;
			fr_copy_buf_info(&fr_buf_info, buf);

			if(copy_to_user((void __user *)arg, &fr_buf_info,
						sizeof(fr_buf_info)))
				return -EFAULT;
			return 0;
		case FRING_PUT_BUF:
			if(copy_from_user(&fr_buf_info, (void __user *)arg,
						sizeof(fr_buf_info)))
				return -EFAULT;
			fr_assert_info(fr_buf_info.fr);
			fr_assert_info(fr_buf_info.buf);
			fr = fr_buf_info.fr;

			/* this buffer has been updated from userspace,
			 * so the timestamp may also be updated by userspace,
			 * sync it to kernel space.
			 */
			buf = (struct fr_buf *)fr_buf_info.buf;
			buf->timestamp = fr_buf_info.timestamp;
			buf->size = fr_buf_info.size;
			buf->priv = fr_buf_info.priv;
            buf->s32Width = fr_buf_info.stAttr.s32Width;
            buf->s32Height = fr_buf_info.stAttr.s32Height;
			/* for vacant fr, update phys each time */
			if(FR_ISVACANT(fr)) {
				buf->phys_addr = fr_buf_info.phys_addr;
				buf->virt_addr = phys_to_virt(buf->phys_addr);
				if(fr_buf_info.map_size < fr_buf_info.size)
				{
					buf->map_size = fr_buf_info.size;
				}
				else
				{
					buf->map_size = fr_buf_info.map_size;
				}
			}
			fr_put_buf(fr_buf_info.fr, fr_buf_info.buf);
			return 0;
		case FRING_FLUSH_BUF:
			if(copy_from_user(&fr_buf_info, (void __user *)arg,
						sizeof(fr_buf_info)))
				return -EFAULT;
			fr_assert_info(fr_buf_info.fr);
			fr = fr_buf_info.fr;
			//clean cache
			dma_sync_single_for_device(NULL, virt_to_phys(fr->cur->virt_addr), fr->cur->size, DMA_TO_DEVICE);
			return 0;
		case FRING_INV_BUF:
			if(copy_from_user(&fr_buf_info, (void __user *)arg,
						sizeof(fr_buf_info)))
				return -EFAULT;
			fr_assert_info(fr_buf_info.fr);
			fr = fr_buf_info.fr;
			//invalidate cache
			dma_sync_single_for_device(NULL, virt_to_phys(fr->cur->virt_addr), fr->cur->size, DMA_FROM_DEVICE);
			return 0;
		case FRING_SET_CACHE_POLICY:
			if(copy_from_user(&priv_data.cache_policy, (void __user*)arg, sizeof (&priv_data.cache_policy)))
				return -EFAULT;
			return 0;

		case FRING_GET_REF:
			if(copy_from_user(&fr_buf_info, (void __user *)arg,
						sizeof(fr_buf_info)))
				return -EFAULT;
			fr_assert_info(fr_buf_info.fr);

			buf = (struct fr_buf *)fr_buf_info.buf;
           ((struct fr *)fr_buf_info.fr)->en_oldest = fr_buf_info.oldest_flag;
			ret = fr_get_ref(fr_buf_info.fr, &buf);
			if(ret < 0) return ret;
			fr_copy_buf_info(&fr_buf_info, buf);

			if(copy_to_user((void __user *)arg, &fr_buf_info,
					sizeof(fr_buf_info)))
				return -EFAULT;
			return 0;
		case FRING_TST_NEW:
			if(copy_from_user(&fr_buf_info, (void __user *)arg,
						sizeof(fr_buf_info)))
				return -EFAULT;
			fr_assert_info(fr_buf_info.fr);
			fr = fr_buf_info.fr;

			return fr->serial_inc - fr_buf_info.serial;

		case FRING_PUT_REF:
			if(copy_from_user(&fr_buf_info, (void __user *)arg,
						sizeof(fr_buf_info)))
				return -EFAULT;
			fr_assert_info(fr_buf_info.fr);
			fr_assert_info(fr_buf_info.buf);
			fr_put_ref(fr_buf_info.fr, fr_buf_info.buf);
			return 0;
		case FRING_SET_TIMEOUT:
			if(copy_from_user(&fr_info, (void __user *)arg, sizeof(fr_info)))
				return -EFAULT;
			fr = fr_info.fr;
			fr_set_timeout(fr, fr_info.timeout);
			return 0;
	}

	return -EFAULT;
}

int fr_dev_open(struct inode *fi, struct file *fp)
{
	fp->private_data = g_fr_dev;
	return 0;
}

static int fr_dev_read(struct file *filp,
   char __user *buf, size_t length, loff_t *offset)
{
	return length;
}

static ssize_t fr_dev_write(struct file *filp,
		const char __user *buf, size_t length,
		loff_t *offset)
{
	char cmd[128];
#include "fr-test.h"

	if(copy_from_user(cmd, buf, length))
		return -EFAULT;

	if(strncmp(cmd, "clean", 5) == 0) {
		__fr_clean();
	}
#ifdef CONFIG_INFOTM_FR_TEST
	else if(strncmp(cmd, "anf", 3) == 0)
		frt_allocandfree();
	else if(strncmp(cmd, "getput", 6) == 0)
		frt_getputbuf();
	else if(strncmp(cmd, "ref", 3) == 0)
		frt_refandbuf();
	else if(strncmp(cmd, "end", 3) == 0)
		frt_refandbuf_end();
	else if(strncmp(cmd, "float", 5) == 0)
		frt_refandbuf_float();
	else if(strncmp(cmd, "nodrop_ring", 11) == 0)
		frt_refandbuf_nodrop();
	else if(strncmp(cmd, "nodrop_float", 12) == 0)
		frt_refandbuf_nodrop_float();
#endif
	return length;
}

static int fr_dev_release(struct inode *fi, struct file *fp)
{
	/* FIXME: I need to free all memory here */
//	fr_log("fr_dev released by pid %d, fp %p\n",
//		current->pid, fp);
	/* see if there are any unreleased buffer from this pid */
	__fr_free_fp((void *)fp);
	return 0;
}

static int fr_dev_mmap(struct file *fp, struct vm_area_struct *vma)
{
	struct device* dev;
	struct fr_priv_data *data  = 0;
	dev = fp->private_data;
	if (dev) {
		data = dev_get_drvdata(dev);
	}
	if (unlikely(data == 0)) {
		vma->vm_page_prot = pgprot_dmacoherent(vma->vm_page_prot);
	}else {
		switch (data->cache_policy) {
			case CC_WRITETHROUGH:
				vma->vm_page_prot = __pgprot_modify(vma->vm_page_prot, L_PTE_MT_MASK, L_PTE_MT_WRITETHROUGH | L_PTE_XN);
				break;
			case CC_WRITEBACK:
				//use cache writeback page map
				vma->vm_page_prot = __pgprot_modify(vma->vm_page_prot, L_PTE_MT_MASK, L_PTE_MT_WRITEBACK |L_PTE_MT_BUFFERABLE| L_PTE_XN);
				break;
			default:
				vma->vm_page_prot = pgprot_dmacoherent(vma->vm_page_prot);
				break;
		}
	}
	//fr_log("remaping from %08lx to %08lx, prot %08x\n", vma->vm_start, vma->vm_end, vma->vm_page_prot);
	return remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
			vma->vm_end - vma->vm_start, vma->vm_page_prot);
}

static const struct file_operations fr_dev_fops = {
	.read  = fr_dev_read,
	.write = fr_dev_write,
	.open  = fr_dev_open,
	.release = fr_dev_release,
	.unlocked_ioctl = fr_dev_ioctl,
	.mmap = fr_dev_mmap,
};

static int __init fr_dev_init(void)
{
	struct class *cls;
	int ret;

	printk(KERN_INFO "fr driver (c) 2016, 2021 InfoTM\n");

	/* create fr dev node */
	ret = register_chrdev(FRING_MAJOR, FRING_NAME, &fr_dev_fops);
	if(ret < 0)
	{
		printk(KERN_ERR "Register char device for fr failed.\n");
		return -EFAULT;
	}

	cls = class_create(THIS_MODULE, FRING_NAME);
	if(!cls)
	{
		printk(KERN_ERR "Can not register class for fr .\n");
		return -EFAULT;
	}

	/* create device node */
	g_fr_dev = device_create(cls, NULL, MKDEV(FRING_MAJOR, 0), NULL, FRING_NAME);

	dev_set_drvdata(g_fr_dev, &priv_data);
	return 0;
}

static void __exit fr_dev_exit(void) {}

module_init(fr_dev_init);
module_exit(fr_dev_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("warits <warits.wang@infotm.com>");
MODULE_DESCRIPTION("fr manages ring-buffers of frames");

