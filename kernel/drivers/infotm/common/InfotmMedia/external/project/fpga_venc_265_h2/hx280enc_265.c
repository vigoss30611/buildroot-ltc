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
#include "hx280enc_265.h"
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
#include <mach/items.h>
#include "mach/clock.h"
#include <linux/clk.h>

static void initDriverPower(void);
static void deInitDriverPower(void);
static void powerDownDriver(void);
static void powerOnDriver(void);
static void closeDriverPower(void);
static void openDriverPower(void);

#define HX280_HEVC_DEV_NAME "HX280_HEVC_ENC"

/* module description */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Verisilicon");
MODULE_DESCRIPTION("H2 Encoder driver");

#define SYSMGR_VENC_265_BASE    (IMAP_SYSMGR_BASE + 0x30c00)
/* this is ARM Integrator specific stuff */
//#define INTEGRATOR_LOGIC_MODULE0_BASE   0xA0600000
#define INTEGRATOR_LOGIC_MODULE0_BASE   0x25300000 //fengwu
/*
#define INTEGRATOR_LOGIC_MODULE1_BASE   0xD0000000
#define INTEGRATOR_LOGIC_MODULE2_BASE   0xE0000000
#define INTEGRATOR_LOGIC_MODULE3_BASE   0xF0000000
*/

//#define VP_PB_INT_LT                    9
#define VP_PB_INT_LT                    131 //fengwu
/*
#define INT_EXPINT1                     10
#define INT_EXPINT2                     11
#define INT_EXPINT3                     12
*/
/* these could be module params in the future */

#define ENC_IO_BASE                 INTEGRATOR_LOGIC_MODULE0_BASE
#define ENC_IO_SIZE                 (376 * 4)    /* bytes */

#define ENC_HW_ID1                  0x48320100

#define HX280ENC_BUF_SIZE           0

static unsigned long base_port = INTEGRATOR_LOGIC_MODULE0_BASE;
static int irq = VP_PB_INT_LT;

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
volatile unsigned int asic_status = 0;

static int g_h2_clk = 102;
module_param(g_h2_clk, int, 0);


DEFINE_SPINLOCK(owner_lock);
DECLARE_WAIT_QUEUE_HEAD(enc_wait_queue);

///////////////// begin ////////////////////////////////

static unsigned int driverOpenCount = 0;
static struct mutex driverOpLock;
static struct clk * driverClk = NULL;
static struct clk * driverbusClk = NULL;

#define DRIVER_NAME             "H2_H265_ENC"          //  "G2_H265_DEC" // "H1_H264_ENC"
#define CLK_GET_SYS_PARAM1      "imap-venc265"          //   "imap-venc264"   // "imap-vdec265"                
#define CLK_GET_SYS_PARAM2      "imap-venc265"          //   "imap-venc264"   // "imap-vdec265"
#define DRIVER_CLK_RATE         (g_h2_clk*1000000)     //  (g_g2_clk * 1000000)   // (g_h1_clk*1000000)
#define DRIVER_POWER_BASE       SYSMGR_VENC_265_BASE   //  SYSMGR_VDEC265_BASE    // SYSMGR_VENC264_BASE

static void initDriverPower(void)
{
    printk("~~~ %s initDriverPower\n", DRIVER_NAME);
    mutex_init(&driverOpLock);
    driverOpenCount = 0;

    driverClk = clk_get_sys(CLK_GET_SYS_PARAM1, CLK_GET_SYS_PARAM2);
    clk_set_rate(driverClk, DRIVER_CLK_RATE);
    module_power_down(DRIVER_POWER_BASE);
    clk_disable_unprepare(driverClk);
}

static void deInitDriverPower(void)
{
    mutex_lock(&driverOpLock);
    if((driverOpenCount > 0) && driverClk)
    {
        printk("~~~ %s deInitDriverPower\n", DRIVER_NAME);
        module_power_down(DRIVER_POWER_BASE);
        clk_disable_unprepare(driverClk);
	if(driverbusClk)
        	clk_disable_unprepare(driverbusClk);
    }
    driverOpenCount = 0;
    mutex_unlock(&driverOpLock);

    mutex_destroy(&driverOpLock); 
}

static void powerDownDriver(void)
{
    mutex_lock(&driverOpLock);
    driverOpenCount--;
    if((driverOpenCount == 0) && driverClk)
    {
        module_power_down(DRIVER_POWER_BASE);
        clk_disable_unprepare(driverClk);
	if(driverbusClk)
        	clk_disable_unprepare(driverbusClk);
    }
    mutex_unlock(&driverOpLock);
}

static void powerOnDriver(void)
{
    mutex_lock(&driverOpLock);

    if(driverOpenCount == 0)
    {
        if(clk_prepare_enable(driverClk) != 0)
        {
            printk("ERROR: ~~~~%s driverClk clk_prepare_enable failed...\n", DRIVER_NAME);
        }
	if(clk_prepare_enable(driverbusClk) != 0)
	{
	    printk("ERROR: ~~~~%s driverbusClk clk_prepare_enable failed...\n", DRIVER_NAME);
	}
        module_power_on(DRIVER_POWER_BASE);

        ResetAsic(&hx280enc_data);  /* reset hardware */
        module_reset(SYSMGR_VENC_265_BASE, 1);
    }
    driverOpenCount++;
    mutex_unlock(&driverOpLock);
}

static void closeDriverPower(void)
{
    mutex_lock(&driverOpLock);
    if(driverOpenCount > 0)
    {
        //printk("~~~ %s closeDriverPower\n", DRIVER_NAME);
        module_power_down(DRIVER_POWER_BASE);
        clk_disable_unprepare(driverClk);
	if(driverbusClk)
       		clk_disable_unprepare(driverbusClk);
    }
    mutex_unlock(&driverOpLock);
}

static void openDriverPower(void)
{
    mutex_lock(&driverOpLock);
    if(driverOpenCount > 0)
    {
        //printk("~~~ %s openDriverPower\n", DRIVER_NAME);
        if(clk_prepare_enable(driverClk) != 0)
        {
            printk("ERROR: ~~~~%s driverClk clk_prepare_enable failed...\n", DRIVER_NAME);
        }
	if(clk_prepare_enable(driverbusClk) != 0)
	{
		printk("ERROR: ~~~~%s driverbusClk clk_prepare_enable failed...\n", DRIVER_NAME);
	}
        module_power_on(DRIVER_POWER_BASE);
    }
    mutex_unlock(&driverOpLock);
}

///////////////////////////////// end //////////////////////////////////

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
        __put_user(asic_status, (unsigned int *)arg);
        break;
    }
    case HX280ENC_IOCG_CORE_OFF:
    {
        closeDriverPower();
        break;
    }
    case HX280ENC_IOCG_CORE_ON:
    {
        openDriverPower();
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

    powerOnDriver();

    PDEBUG("dev opened\n");
    return result;
}
static int hx280enc_release(struct inode *inode, struct file *filp)
{
#ifdef HX280ENC_DEBUG
    hx280enc_t *dev = (hx280enc_t *) filp->private_data;

    dump_regs((unsigned long) dev); /* dump the regs */
#endif

    powerDownDriver();

    PDEBUG("dev closed\n");
    return 0;
}


static int hx280_265_probe(struct platform_device *pdev)
{
    printk( "%s -> hx280_265_probe\n", __func__);
    module_power_on(SYSMGR_VENC_265_BASE);

    int result ;
    hx280enc_data.iobaseaddr = base_port;
    hx280enc_data.iosize = ENC_IO_SIZE;
    hx280enc_data.irq = irq;
    hx280enc_data.async_queue = NULL;
    hx280enc_data.hwregs = NULL;

    printk( "%s -> power\n", __func__);

    result = ReserveIO();
    if(result < 0)
    {
        printk( "%s ReserveIO error: %d\n", __func__, result);
        goto err;
    }

    ResetAsic(&hx280enc_data);  /* reset hardware */

    /* get the IRQ line */
    if(irq != -1)
    {
        result = request_irq(irq, hx280enc_isr,
                             IRQF_DISABLED | IRQF_SHARED,
                             HX280_HEVC_DEV_NAME, (void *) &hx280enc_data);
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

    printk(KERN_INFO "hx280enc: probe inserted. \n");
    initDriverPower();
    return 0;

err:
    printk("hx280enc: probe error. \n");
    return -1;
    
}

static int hx280_265_remove(struct platform_device *pdev)
{
    printk("hx280enc: hx280_265_remove. \n");
    deInitDriverPower();
    return 0; 
}
static int hx280_265_suspend(struct platform_device *pdev, pm_message_t state)
{
    closeDriverPower();
    printk("hx280enc: suspend inserted. \n");
    return 0;
}

static int hx280_265_resume(struct platform_device *pdev)
{
    openDriverPower();
    printk("hx280enc: hx280_265_resume. \n");
    return 0;

}


static struct resource imap_hx280_265_res[] = {
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

static struct file_operations hx280enc_fops = {
	.owner= THIS_MODULE,
	.open = hx280enc_open,
	.release = hx280enc_release,
	.unlocked_ioctl = hx280enc_ioctl,
	.fasync = NULL,
};

static struct miscdevice hx280_265_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &hx280enc_fops,
    .name = HX280_HEVC_DEV_NAME,
};


static void platform_h2_release(struct device * dev) { return ; }
struct platform_device imap_hx280_265_device = {
    .name = "imap-hx280-265",
    .id = 0,
    .dev = {
        .release = platform_h2_release,
    }, 
    .num_resources = ARRAY_SIZE(imap_hx280_265_res),
};

static struct platform_driver hx280_265_plat = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "imap-hx280-265",
    },
    .probe = hx280_265_probe,
    .remove = hx280_265_remove,
    .suspend = hx280_265_suspend,
    .resume = hx280_265_resume,
};

int __init hx280enc_h2_265_init(void)
{
    printk( "%s enter\n", __func__);
    // enable clock
    int err;
    
    driverbusClk = clk_get_sys("imap-venc265-ctrl","imap-venc265-ctrl");
    if (IS_ERR(driverbusClk)) {
	    pr_err(" imap-venc265-ctrl clock not found: \n");
	    return -1;
    }
    err = clk_prepare_enable(driverbusClk);
    if (err) {
	    pr_err("imap-venc265-ctrl clock prepare enable fail!\n");
	    clk_disable_unprepare(driverbusClk);
	    clk_put(driverbusClk);
	    return err;
    }

    struct clk * clk = clk_get_sys("imap-venc265", "imap-venc265");		
    if (IS_ERR(clk)) {
	    pr_err(" imap-venc265 clock not found: \n");
	    return -1;
    }
    clk_set_rate(clk, g_h2_clk * 1000000);		
    err = clk_prepare_enable(clk);
    if (err) {
	    pr_err("imap-venc265 clock prepare enable fail!\n");
	    clk_disable_unprepare(clk);
	    clk_put(clk); 
	    return err;
    }

    int ret = platform_device_add_resources(&imap_hx280_265_device, imap_hx280_265_res, ARRAY_SIZE(imap_hx280_265_res));
    if(ret != 0) {
        printk( "add hx280_265 device resource error \n");
        return ret;
    }

    printk( "%s -> platform_device_register\n", __func__);
    ret = platform_device_register(&imap_hx280_265_device);
    if(ret != 0) {
        printk( "register hx280_265 device error \n");
        return ret;
    }

    /* Register platform device driver. */
    printk( "%s -> platform_driver_register\n", __func__);
    ret = platform_driver_register(&hx280_265_plat);
    if (unlikely(ret != 0)) {
        printk( "Register hx280_265 platform device driver error, %d.\n", ret);
        return ret;
    }

    /* Register misc device driver. */
    printk( "%s -> misc_register\n", __func__);
    ret = misc_register(&hx280_265_miscdev);
    if (unlikely(ret != 0)) {
        printk( "Register hx280_265 misc device driver error, %d.\n", ret);
        return ret;
    } 

    return 0;
}


int __init hx280enc_265_init2(void)
{
    int result;

    printk(KERN_INFO "hx280enc: module init - base_port=0x%08lx irq=%i\n",
           base_port, irq);

    hx280enc_data.iobaseaddr = base_port;
    hx280enc_data.iosize = ENC_IO_SIZE;
    hx280enc_data.irq = irq;
    hx280enc_data.async_queue = NULL;
    hx280enc_data.hwregs = NULL;

    result = register_chrdev(hx280enc_major, HX280_HEVC_DEV_NAME, &hx280enc_fops);
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
                             HX280_HEVC_DEV_NAME, (void *) &hx280enc_data);
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
    unregister_chrdev(hx280enc_major, HX280_HEVC_DEV_NAME);
    printk(KERN_INFO "hx280enc: module not inserted\n");
    return result;
}

void __exit hx280enc_h2_cleanup(void)
{
    writel(0, hx280enc_data.hwregs + 0x14); /* disable HW */
    writel(0, hx280enc_data.hwregs + 0x04); /* clear enc IRQ */

    /* free the encoder IRQ */
    if(hx280enc_data.irq != -1)
    {
        free_irq(hx280enc_data.irq, (void *) &hx280enc_data);
    }

    ReleaseIO();

    //unregister_chrdev(hx280enc_major, HX280_HEVC_DEV_NAME);

    misc_deregister(&hx280_265_miscdev);
    platform_driver_unregister(&hx280_265_plat);
    platform_device_unregister(&imap_hx280_265_device);

    printk(KERN_INFO "hx280enc: module removed\n");
    return;
}

module_init(hx280enc_h2_265_init);
module_exit(hx280enc_h2_cleanup);

static int ReserveIO(void)
{
    
    printk( "%s request_mem_region iobase:%x, size:%d, hevc name:%s\n", __func__, hx280enc_data.iobaseaddr, hx280enc_data.iosize, HX280_HEVC_DEV_NAME);
    long int hwid;

    if(!request_mem_region
       (hx280enc_data.iobaseaddr, hx280enc_data.iosize, HX280_HEVC_DEV_NAME))
    {
        printk(KERN_INFO "hx280enc: failed to reserve HW regs\n");
        return -EBUSY;
    }

    printk( "%s ioremap_nocache iobase:%x, size:%d\n", __func__, hx280enc_data.iobaseaddr, hx280enc_data.iosize );
    hx280enc_data.hwregs =
        (volatile u8 *) ioremap_nocache(hx280enc_data.iobaseaddr,
                                        hx280enc_data.iosize);

    if(hx280enc_data.hwregs == NULL)
    {
        printk(KERN_INFO "hx280enc: failed to ioremap HW regs\n");
        ReleaseIO();
        return -EBUSY;
    }

    printk( "%s readl hx280enc_data.hwregs:%x\n", __func__, hx280enc_data.hwregs);
    hwid = readl(hx280enc_data.hwregs);

    /* check for encoder HW ID */
    if((((hwid >> 16) & 0xFFFF) != ((ENC_HW_ID1 >> 16) & 0xFFFF)))
    {
        printk(KERN_INFO "hx280enc: HW not found at 0x%08lx, id:%x, wantid:%x cont\n",
               hx280enc_data.iobaseaddr, hwid, ENC_HW_ID1);
        //dump_regs((unsigned long) &hx280enc_data);
        //ReleaseIO();
        //return -EBUSY;
    }

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
        asic_status = irq_status & (~0x01);
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

