#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/page.h>
#include <linux/slab.h>		// for kmalloc
#include <linux/miscdevice.h>
#include <linux/pci.h>
#include <linux/version.h>

/*
 * gcc defines "linux" as "1".
 * [ http://stackoverflow.com/questions/19210935 ]
 * IMG_KERNEL_ION_HEADER can be <linux/ion.h>, which expands to <1/ion.h>
 */
#undef linux
#include IMG_KERNEL_ION_HEADER
#include IMG_KERNEL_ION_PRIV_HEADER

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("img");

int enable_system_heap = 0;
module_param(enable_system_heap, int, 0444);
MODULE_PARM_DESC(enable_system_heap, "heap 2: vmalloc");
int enable_contig_heap = 0;
module_param(enable_contig_heap, int, 0444);
MODULE_PARM_DESC(enable_contig_heap, "heap 1: kmalloc");
int carveout_size = 0;
module_param(carveout_size, int, 0444);
MODULE_PARM_DESC(carveout_size, "heap 4: how many pages to carveout (0 to use pci bar size)");
unsigned long carveout_phys_start = 0;
module_param(carveout_phys_start, ulong, 0444);
MODULE_PARM_DESC(carveout_phys_start, "carveout starting physical address (0 to use pci vendor/product");
unsigned long carveout_phys_offset = 0;
module_param(carveout_phys_offset, ulong, 0444);
MODULE_PARM_DESC(carveout_phys_offset, "offset to be added to carveout_phys_start: useful for sharing PCI memory with PALLOC");
int carveout_pci_vendor = 0;
int carveout_pci_product = 0;
int carveout_pci_bar = 0;
module_param(carveout_pci_vendor, int, 0444);
MODULE_PARM_DESC(carveout_pci_vendor, "carveout PCI vendor id");
module_param(carveout_pci_product, int, 0444);
MODULE_PARM_DESC(carveout_pci_product, "carveout PCI product id");
module_param(carveout_pci_bar, int, 0444);
MODULE_PARM_DESC(carveout_pci_bar, "carveout PCI BAR");


static ssize_t img_write(struct file *filep, const char __user *buf, size_t count, loff_t *ppos)
{
	int ret = count;
	char * str = kmalloc(count, GFP_KERNEL);
	printk("img_write %zd\n", count);
	if(str == 0) {
		printk(KERN_ERR "failed to malloc %zd\n", count);
		return -EFAULT;
	}
	if(copy_from_user(str, buf, count)) {
		kfree(str);
		return -EFAULT;
	}
	kfree(str);
	return ret;
}

static int img_open(struct inode *inode, struct file *filep)
{
	int minor = iminor(inode);
	struct miscdevice * imgmisc = filep->private_data;	// recent kernels only!!
	printk("img_open: minor:%d misc:%p\n", minor, imgmisc);

	return nonseekable_open(inode, filep);
}
static int img_release(struct inode *inode, struct file *filep)
{
	printk("img_release\n");

	return 0;
}
static ssize_t img_read(struct file *filep, char __user *buf, size_t count, loff_t *ppos)
{
	int ret = 0;
	char tmp[100];
	printk("img_read count:%zd \n", count);
	if(*ppos)
		return 0;

	snprintf(tmp, sizeof(tmp), "hello world\n");
	ret = copy_to_user(buf, tmp, strlen(tmp)+1);
	if(ret)
		return ret;

	count = strlen(tmp);
	*ppos += count;
	return count;
}
static int img_mmap(struct file * filep, struct vm_area_struct * vma)
{
	return -EINVAL;
}
#define IMG_IOCTL_MAGIC 'j'
#define IMG_IOC_1 _IOW(IMG_IOCTL_MAGIC, 1, int*)
#define IMG_IOC_MAX 1
static long img_ioctl(struct file * file, unsigned int cmd, unsigned long arg)
{
	char __user * argp = (char __user*)arg;

	if ((_IOC_TYPE(cmd) != IMG_IOCTL_MAGIC) ||
		(_IOC_NR(cmd) > IMG_IOC_MAX)
	)
	{
		return -ENOTTY;
	}

	switch(cmd) {
	char buf[4];
	case IMG_IOC_1:
		if(copy_from_user(buf, argp, sizeof(buf)))
			return -EFAULT;
	default:
		return -EINVAL;
	}

	return 0;
}
static const struct file_operations img_fops = {
	.owner   = THIS_MODULE,
	.open    = img_open,
	.release = img_release,
	.read    = img_read,
	.write   = img_write,
	.mmap    = img_mmap,
	.llseek  = no_llseek,
	.unlocked_ioctl = img_ioctl,
};

static struct miscdevice imgmisc = {
	.name = "imgion",
	.fops = &img_fops,
	.minor = MISC_DYNAMIC_MINOR,
};


#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
  static struct ion_platform_heap generic_heaps[] = {
	  {
	      .type = ION_HEAP_TYPE_SYSTEM_CONTIG,
	      .name = "System contig",
	      .id = ION_HEAP_TYPE_SYSTEM_CONTIG,
	  },
	  {
	      .type = ION_HEAP_TYPE_SYSTEM,
	      .name = "System",
	      .id = ION_HEAP_TYPE_SYSTEM,
	  },
	  {
	      .type = ION_HEAP_TYPE_CARVEOUT,
	      .name = "Carveout",
	      .id = ION_HEAP_TYPE_CARVEOUT,
	      .base = 0,
	      .size = 0,
	  }
  };

  static struct ion_platform_data generic_config = {
	  .nr = 3,
	  .heaps = generic_heaps
  };
#else
  static struct ion_platform_data generic_config = {
	.nr = 3,
	.heaps = {
        {
            .type = ION_HEAP_TYPE_SYSTEM_CONTIG,
            .name = "System contig",
            .id = ION_HEAP_TYPE_SYSTEM_CONTIG,
        },
        {
            .type = ION_HEAP_TYPE_SYSTEM,
            .name = "System",
            .id = ION_HEAP_TYPE_SYSTEM,
        },
        {
            .type = ION_HEAP_TYPE_CARVEOUT,
            .name = "Carveout",
            .id = ION_HEAP_TYPE_CARVEOUT,
            .base = 0,
            .size = 0,
        },
    }
  };
#endif

static struct ion_device *ion_dev;
struct ion_heap *ion_heaps[3];

static int ion_init(void)
{
    int ret = 0;
    int i;

    printk("%s\n", __FUNCTION__);
    ion_dev = ion_device_create(NULL);
    if(IS_ERR_OR_NULL(ion_dev)) {
        pr_err("ion_device_create failed\n");
        return -ENOSPC;
    }
	/* Register all the heaps */
	for (i = 0; i < generic_config.nr; i++)
	{
		struct ion_platform_heap *heap = &generic_config.heaps[i];

        switch(heap->id) {
        case ION_HEAP_TYPE_SYSTEM_CONTIG:
            if(!enable_contig_heap) continue;
            break;
        case ION_HEAP_TYPE_SYSTEM:
            if(!enable_system_heap) continue;
            break;
        case ION_HEAP_TYPE_CARVEOUT:
            if(carveout_pci_vendor && carveout_pci_product) {
                struct pci_dev * pcidev = pci_get_device(carveout_pci_vendor, carveout_pci_product, NULL);
                if(pcidev == NULL) 
                    return -EINVAL;
                carveout_phys_start = pci_resource_start(pcidev, carveout_pci_bar);
                if(carveout_size == 0)
                    carveout_size = pci_resource_len(pcidev, 0);
                pci_dev_put(pcidev);
            }
            if(carveout_size < PAGE_SIZE) continue;
            if(!carveout_phys_start) continue;
            heap->base = carveout_phys_start + carveout_phys_offset;
            heap->size = carveout_size;
            printk("IMG %s creating heap %d, phys:%lx size %zu\n", __func__, i, heap->base, heap->size);
            break;
        }
		ion_heaps[i] = ion_heap_create(heap);
		if (IS_ERR_OR_NULL(ion_heaps[i]))
		{
            pr_err("ion_heap_create failed\n");
			ret = -ENOSPC;
			goto fail;
		}
		ion_device_add_heap(ion_dev, ion_heaps[i]);
	}
	return ret;
fail:
	printk("Failed to initialize imgion!\n");
    // TOBEDONE: clearup
    return ret;
}

static void ion_deinit(void)
{
    int i;
    printk("%s\n", __FUNCTION__);
	for (i = 0; i < generic_config.nr; i++) {
		if (ion_heaps[i])
		{
            ion_heap_destroy(ion_heaps[i]);
		}
	}
	ion_device_destroy(ion_dev);
}

static int __init img_init(void)
{
	int ret = 0;
	printk("img_init\n");

	ret = misc_register(&imgmisc);
	if(ret) {
		pr_err("device_register failed\n");
	}
    ion_init();
	return ret;
}

static void __exit img_exit(void)
{
	printk("img exit\n");
    ion_deinit();
    misc_deregister(&imgmisc);
}

module_init(img_init);
module_exit(img_exit);
