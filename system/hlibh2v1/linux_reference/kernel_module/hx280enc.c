/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------
--
--  Abstract : H2 Encoder device driver (kernel module)
--
------------------------------------------------------------------------------*/

#include <linux/kernel.h>
#include <linux/module.h>
/* needed for __init,__exit directives */
#include <linux/init.h>
/* needed for remap_page_range 
	SetPageReserved
	ClearPageReserved
*/
#include <linux/mm.h>
/* obviously, for kmalloc */
#include <linux/slab.h>
/* for struct file_operations, register_chrdev() */
#include <linux/fs.h>
/* standard error codes */
#include <linux/errno.h>

#include <linux/moduleparam.h>
/* request_irq(), free_irq() */
#include <linux/interrupt.h>
#include <linux/sched.h>

#include <linux/semaphore.h>
#include <linux/spinlock.h>
/* needed for virt_to_phys() */
#include <asm/io.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>

#include <asm/irq.h>

#include <linux/version.h>

/* our own stuff */
#include "hx280enc.h"

/* module description */
MODULE_LICENSE("Proprietary");
MODULE_AUTHOR("infotm");
MODULE_DESCRIPTION("hx280 encoder driver");

/* this is ARM Integrator specific stuff */
#define INTEGRATOR_LOGIC_MODULE0_BASE   0xA0600000
/*
#define INTEGRATOR_LOGIC_MODULE1_BASE   0xD0000000
#define INTEGRATOR_LOGIC_MODULE2_BASE   0xE0000000
#define INTEGRATOR_LOGIC_MODULE3_BASE   0xF0000000
*/

#define VP_PB_INT_LT                    9
/*
#define INT_EXPINT1                     10
#define INT_EXPINT2                     11
#define INT_EXPINT3                     12
*/
/* these could be module params in the future */

/** h280 encodec */
#define HX280ENC_BASE_REG_PA        (0x25300000)

#define ENC_IO_BASE                 HX280ENC_BASE_REG_PA //INTEGRATOR_LOGIC_MODULE0_BASE
#define ENC_IO_SIZE                 (376 * 4)    /* bytes */

#define ENC_HW_ID1                  0x48320100

#define ENC_MEMORY_BASE             (0x00000000)   //TODO
#define ENC_MEMORY_SIZE                 (0x10000)
#define HX280ENC_BUF_SIZE           0

unsigned long base_port = INTEGRATOR_LOGIC_MODULE0_BASE;
int irq = VP_PB_INT_LT;


typedef struct
{
    char *buffer;
    unsigned int iosize;
    volatile u8 *hwregs[HXENC_MAX_CORES];
    int irq;
    int cores;
    struct fasync_struct *async_queue_enc;
}hx280enc_t;

hx280enc_t hx280enc_param;
static unsigned int hx280enc_open_count;		/* record open syscall times */
static struct mutex hx280enc_lock;		/* mainly lock for decode instance count */


#ifdef CONFIG_VDEC_POLL_MODE
static wait_queue_head_t wait_hx280enc;		/* a wait queue for poll system call */
static volatile unsigned int hx280enc_poll_mark = 0;
#endif	/* CONFIG_IMAP_DECODE_POLL_MODE */


/* module_param(name, type, perm) */
module_param(base_port, ulong, 0);
module_param(irq, int, 0);

/* and this is our MAJOR; use 0 for dynamic allocation (recommended)*/
static int hx280enc_major = 0;

/* here's all the must remember stuff */
typedef struct
{
    char *buffer;
    unsigned int buffsize;
    unsigned long iobaseaddr;
    unsigned int iosize;
    volatile u8 *hwregs;
    unsigned int irq;
    struct fasync_struct *async_queue;
} hx280enc_t;

/* dynamic allocation? */
static hx280enc_t hx280enc_data;

static int ReserveIO(void);
static void ReleaseIO(void);
static void ResetAsic(hx280enc_t * dev);

/** infotm kernel specific */
static void hx280enc_module_enable(void);
static void hx280enc_module_disable(void);
static void hx280enc_module_reset(void);
static void hx280enc_reset_asic(void);
//static irqreturn_t hx280enc_irq_handle(int irq, void *dev_id);


#ifdef HX280ENC_DEBUG
static void dump_regs(unsigned long data);
#endif

/* IRQ handler */
#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
static irqreturn_t hx280enc_isr(int irq, void *dev_id, struct pt_regs *regs);
#else
static irqreturn_t hx280enc_isr(int irq, void *dev_id);
#endif

static int enc_irq = 0;
volatile unsigned int h2_asic_status = 0;

static DEFINE_SPINLOCK(owner_lock);
DECLARE_WAIT_QUEUE_HEAD(enc_wait_queue);

static int CheckEncIrq(hx280enc_t *dev)
{
    unsigned long flags;
    int rdy = 0;
    spin_lock_irqsave(&owner_lock, flags);

    if(enc_irq)
    {
        /* reset the wait condition(s) */
        enc_irq = 0;
        rdy = 1;
    }

    spin_unlock_irqrestore(&owner_lock, flags);

    return rdy;
}
unsigned int WaitEncReady(hx280enc_t *dev)
{
    

    PDEBUG("wait_event_interruptible ENC\n");

	if(wait_event_interruptible(enc_wait_queue, CheckEncIrq(dev)))
   {
	   PDEBUG("ENC wait_event_interruptible interrupted\n");
	   return -ERESTARTSYS;
   }


    return 0;
}
static long hx280enc_ioctl(struct file *filp,
                          unsigned int cmd, unsigned long arg)
{
    int err = 0;

    unsigned int tmp;
    PDEBUG("ioctl cmd 0x%08ux\n", cmd);
    /*
     * extract the type and number bitfields, and don't encode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if(_IOC_TYPE(cmd) != HX280ENC_IOC_MAGIC)
        return -ENOTTY;
    if(_IOC_NR(cmd) > HX280ENC_IOC_MAXNR)
        return -ENOTTY;

    /*
     * the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user-oriented, while
     * access_ok is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if(_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void *) arg, _IOC_SIZE(cmd));
    else if(_IOC_DIR(cmd) & _IOC_WRITE)
        err = !access_ok(VERIFY_READ, (void *) arg, _IOC_SIZE(cmd));
    if(err)
        return -EFAULT;

    switch (cmd)
    {
    case HX280ENC_IOCGHWOFFSET:
        __put_user(hx280enc_data.iobaseaddr, (unsigned long *) arg);
        break;

    case HX280ENC_IOCGHWIOSIZE:
        __put_user(hx280enc_data.iosize, (unsigned int *) arg);
        break;
    
    case HX280ENC_IOCG_CORE_WAIT:
    {
		  
        tmp = WaitEncReady(&hx280enc_data);
        __put_user(h2_asic_status, (unsigned int *)arg);
        break;
    }
    }
    return 0;
}

static int hx280enc_open(struct inode *inode, struct file *filp)
{
    int result = 0;
    hx280enc_t *dev = &hx280enc_data;

    filp->private_data = (void *) dev;

    PDEBUG("dev opened\n");
    return result;
}
static int hx280enc_release(struct inode *inode, struct file *filp)
{
#ifdef HX280ENC_DEBUG
    hx280enc_t *dev = (hx280enc_t *) filp->private_data;

    dump_regs((unsigned long) dev); /* dump the regs */
#endif

    PDEBUG("dev closed\n");
    return 0;
}

/* VFS methods */
static struct file_operations hx280enc_fops = {
	.owner= THIS_MODULE,
	.open = hx280enc_open,
	.release = hx280enc_release,
	.unlocked_ioctl = hx280enc_ioctl,
	.fasync = NULL,
};

#if 0
int __init hx280enc_init(void)
{
    int result;

    printk(KERN_INFO "hx280enc: module init - base_port=0x%08lx irq=%i\n",
           base_port, irq);

    hx280enc_data.iobaseaddr = base_port;
    hx280enc_data.iosize = ENC_IO_SIZE;
    hx280enc_data.irq = irq;
    hx280enc_data.async_queue = NULL;
    hx280enc_data.hwregs = NULL;

    result = register_chrdev(hx280enc_major, "hx280enc", &hx280enc_fops);
    if(result < 0)
    {
        printk(KERN_INFO "hx280enc: unable to get major <%d>\n",
               hx280enc_major);
        return result;
    }
    else if(result != 0)    /* this is for dynamic major */
    {
        hx280enc_major = result;
    }

    result = ReserveIO();
    if(result < 0)
    {
        goto err;
    }

    ResetAsic(&hx280enc_data);  /* reset hardware */

    /* get the IRQ line */
    if(irq != -1)
    {
        result = request_irq(irq, hx280enc_isr,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
                             SA_INTERRUPT | SA_SHIRQ,
#else
                             IRQF_DISABLED | IRQF_SHARED,
#endif
                             "hx280enc", (void *) &hx280enc_data);
        if(result == -EINVAL)
        {
            printk(KERN_ERR "hx280enc: Bad irq number or handler\n");
            ReleaseIO();
            goto err;
        }
        else if(result == -EBUSY)
        {
            printk(KERN_ERR "hx280enc: IRQ <%d> busy, change your config\n",
                   hx280enc_data.irq);
            ReleaseIO();
            goto err;
        }
    }
    else
    {
        printk(KERN_INFO "hx280enc: IRQ not in use!\n");
    }

    printk(KERN_INFO "hx280enc: module inserted. Major <%d>\n", hx280enc_major);

    return 0;

  err:
    unregister_chrdev(hx280enc_major, "hx280enc");
    printk(KERN_INFO "hx280enc: module not inserted\n");
    return result;
}
#endif


static struct resource imap_hx280enc_resource[] = {
    [0] = {
        .start = 0,
        .end = 0,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = 0,
        .end = 0,
        .flags = IORESOURCE_IRQ,
    }
};

struct platform_device imap_hx280enc_device = {
    .name = "imap-hx280enc",
    .id = 0,
    .num_resources = ARRAY_SIZE(imap_vdec_resource),
};
/*
 * Tutorial:
 * Platform device driver provides suspend/resume support.
 */
static struct platform_driver hx280enc_plat = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "imap-hx280enc",
    },
    .probe = hx280enc_probe,
    .remove = hx280enc_remove,
    .suspend = hx280enc_suspend,
    .resume = hx280enc_resume,
};
static struct miscdevice hx280enc_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &hx280enc_fops,
    .name = "hx280enc",
};


int __init hx280enc_init(void)
{
    int ret = 0;
    DBGMSG(INFO, "%s()\n", __func__);
    
    /*Register vdec device */
    ret = platform_device_add_resources(&imap_hx280enc_device, imap_hx280enc_resource, ARRAY_SIZE(imap_hx280enc_resource));
    if(ret != 0) {
        printk( "add hx280enc device resource error \n");
        return ret;
    }

    ret = platform_device_register(&imap_hx280enc_device);
    if(ret != 0) {
        printk( "register vdec device error \n");
        return ret;
    }

    /* Register platform device driver. */
    ret = platform_driver_register(&hx280enc_plat);
    if (unlikely(ret != 0)) {
        printk("Register hx280enc platform device driver error, %d.\n", ret);
        return ret;
    }

    /* Register misc device driver. */
    ret = misc_register(&hx280enc_miscdev);
    if (unlikely(ret != 0)) {
        printk("Register hx280enc misc device driver error, %d.\n", ret);
        return ret;
    }
    return 0;

}

#if 0
void __exit hx280enc_cleanup(void)
{
    writel(0, hx280enc_data.hwregs + 0x14); /* disable HW */
    writel(0, hx280enc_data.hwregs + 0x04); /* clear enc IRQ */

    /* free the encoder IRQ */
    if(hx280enc_data.irq != -1)
    {
        free_irq(hx280enc_data.irq, (void *) &hx280enc_data);
    }

    ReleaseIO();

    unregister_chrdev(hx280enc_major, "hx280enc");

    printk(KERN_INFO "hx280enc: module removed\n");
    return;
}
#endif
void __exit hx280enc_cleanup(void)
{
    printk( "%s()\n", __func__);

    misc_deregister(&hx280enc_miscdev);
    platform_driver_unregister(&hx280enc_plat);

}

module_init(hx280enc_init);
module_exit(hx280enc_cleanup);

static int ReserveIO(void)
{
    long int hwid;

    if(!request_mem_region
       (hx280enc_data.iobaseaddr, hx280enc_data.iosize, "hx280enc"))
    {
        printk(KERN_INFO "hx280enc: failed to reserve HW regs\n");
        return -EBUSY;
    }

    hx280enc_data.hwregs =
        (volatile u8 *) ioremap_nocache(hx280enc_data.iobaseaddr,
                                        hx280enc_data.iosize);

    if(hx280enc_data.hwregs == NULL)
    {
        printk(KERN_INFO "hx280enc: failed to ioremap HW regs\n");
        ReleaseIO();
        return -EBUSY;
    }

    hwid = readl(hx280enc_data.hwregs);

#if 1
    /* check for encoder HW ID */
    if((((hwid >> 16) & 0xFFFF) != ((ENC_HW_ID1 >> 16) & 0xFFFF)))
    {
        printk(KERN_INFO "hx280enc: HW not found at 0x%08lx\n",
               hx280enc_data.iobaseaddr);
#ifdef HX280ENC_DEBUG
        dump_regs((unsigned long) &hx280enc_data);
#endif
        ReleaseIO();
        return -EBUSY;
    }
#endif
    printk(KERN_INFO
           "hx280enc: HW at base <0x%08lx> with ID <0x%08lx>\n",
           hx280enc_data.iobaseaddr, hwid);

    return 0;
}

static void ReleaseIO(void)
{
    if(hx280enc_data.hwregs)
        iounmap((void *) hx280enc_data.hwregs);
    release_mem_region(hx280enc_data.iobaseaddr, hx280enc_data.iosize);
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
irqreturn_t hx280enc_isr(int irq, void *dev_id, struct pt_regs *regs)
#else
irqreturn_t hx280enc_isr(int irq, void *dev_id)
#endif
{
    unsigned int handled = 0;
    hx280enc_t *dev = (hx280enc_t *) dev_id;
    u32 irq_status;
	
    irq_status = readl(dev->hwregs + 0x04);
	
    if(irq_status & 0x01)
    {
        /* clear enc IRQ and slice ready interrupt bit */
        //writel(irq_status & (~0x101), dev->hwregs + 0x04);
        writel(0x0, dev->hwregs + 0x04);
        enc_irq = 1;
        h2_asic_status = irq_status & (~0x01);
        wake_up_interruptible_all(&enc_wait_queue);
        handled++;
    }
    if(!handled)
    {
        PDEBUG("IRQ received, but not x170's!\n");
    }
    return IRQ_HANDLED;
}

void ResetAsic(hx280enc_t * dev)
{
    int i;

    writel(0, dev->hwregs + 0x14);
    for(i = 4; i < dev->iosize; i += 4)
    {
        writel(0, dev->hwregs + i);
    }
}

#ifdef HX280ENC_DEBUG
void dump_regs(unsigned long data)
{
    hx280enc_t *dev = (hx280enc_t *) data;
    int i;

    PDEBUG("Reg Dump Start\n");
    for(i = 0; i < dev->iosize; i += 4)
    {
        PDEBUG("\toffset %02X = %08X\n", i, readl(dev->hwregs + i));
    }
    PDEBUG("Reg Dump End\n");
}
#endif

static int hx280enc_probe(struct platform_device *pdev)
{
    int		ret;

    //struct resource	res;
    printk( "%s()\n", __func__);
    /* initualize open count */
    hx280enc_open_count = 0;

#ifdef CONFIG_VDEC_POLL_MODE
    /* initualize wait queue */
    init_waitqueue_head(&wait_hx280enc);
#endif	/* CONFIG_IMAP_DECODE_POLL_MODE */

#ifdef CONFIG_IMAP_DECODE_SIGNAL_MODE
    /* initualize async queue */
    hx280enc_param.async_queue_dec	= NULL;
    //vdec_param.async_queue_pp	= NULL;
#endif	/* CONFIG_IMAP_DECODE_SIGNAL_MODE */

    /* request memory region for registers */
    hx280_param.reg_reserved_size = ENC_IO_SIZE;
    hx280_param.reg_base_phys_addr = ENC_IO_BASE;

    if(!request_mem_region
            (hx280enc_param.reg_base_phys_addr, hx280enc_param.reg_reserved_size, HX280ENC_MODULE_NAME))
    {   
        printk(KERN_ALERT "hx280enc_drv: failed to reserve HW regs\n");
        return -EBUSY;
    }   

    hx280enc_param.reg_base_virt_addr =
        (volatile u8 *) ioremap_nocache(hx280enc_param.reg_base_phys_addr,
                hx280enc_param.reg_reserved_size);

    if(hx280enc_param.reg_base_virt_addr == NULL)
    {   
        printk(KERN_ALERT  "hx280enc_drv: failed to ioremap HW regs\n");
        return -EBUSY;
    }   

    /* decoder and pp shared one irq line, so we must register irq in share mode */
    ret = request_irq(IRQ_HX280ENC, hx280enc_isr, IRQF_DISABLED | IRQF_SHARED, pdev->name, (void *)(&hx280_param));
    if(ret)
    {
        printk("Fail to request irq for hx280enc\n");
        return ret;
    }

    /* init vdec mutex lock */
    mutex_init(&hx280enc_lock);

    /* init sema for race condition */
    sema_init(&hx280enc_param.dec_sema, 1);
    sema_init(&hx280enc_param.pp_sema, 1);
    sema_init(&hx280enc_param.mmu_sema, 0);

    /* map mempool addr */
    hx280enc_param.mempool_virt_addr = ioremap_nocache(ENC_MEMORY_BASE, ENC_MEMORY_SIZE);

    /* bus clock */
    /*hx280enc_param.bus_clock = clk_get_sys("imap-vdec", "imap-vdec");
    if(IS_ERR(vdec_param.bus_clock)){
        DBGMSG(ERR, "clk_get(vdec) failed\n");
        return -EFAULT;	
    }
    */
    hx280enc_param.clk_enable = 0;
    hx280enc_module_disable();


    return VDEC_RET_OK;
}

static int hx280enc_remove(struct platform_device *pdev)
{
    printk( "%s()\n", __func__);

    /* unmap register base */
    iounmap((void *)(hx280enc_param.mempool_virt_addr));
    iounmap((void *)(hx280enc_param.reg_base_virt_addr));
    release_mem_region(hx280enc_param.reg_base_phys_addr, hx280enc_param.reg_reserved_size);

    /* release irq */
    free_irq(IRQ_HX280ENC, pdev);
    
    mutex_destroy(&hx280enc_lock);

    return HX280ENC_RET_OK;
}


static int hx280enc_suspend(struct platform_device *pdev, pm_message_t state)
{
    //TODO:
    printk( "%s(hx280enc_open_count=%d)\n", __func__, hx280enc_open_count);

    mutex_lock(&hx280enc_lock);
    if(hx280enc_open_count > 0)
    {
       hx280enc_module_disable();
    }
    mutex_unlock(&hx280enc_lock);
    return HX280ENC_RET_OK;
}

static int hx280enc_remove(struct platform_device *pdev)
{
    //TODO:
    printk( "%s()\n", __func__);

    /* unmap register base */
    iounmap((void *)(vdec_param.mempool_virt_addr));
    iounmap((void *)(vdec_param.reg_base_virt_addr));
    release_mem_region(vdec_param.reg_base_phys_addr, vdec_param.reg_reserved_size);

    /* release irq */
    free_irq(IRQ_HX280ENC, pdev);
    mutex_destroy(&hx280enc_lock);

    return HX280ENC_RET_OK;
}

/**TODO: */
static void hx280enc_module_enable(void)
{
}
static void hx280enc_module_disable(void)
{
}
static void hx280enc_module_reset(void)
{
}
static void hx280enc_reset_asic(void)
{
}



