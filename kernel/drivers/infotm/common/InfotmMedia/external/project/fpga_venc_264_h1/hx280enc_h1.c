/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Google Finland Oy.                              --
--                                                                            --
--                   (C) COPYRIGHT 2012 GOOGLE FINLAND OY                     --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : 6280/7280/8270/8290/H1 Encoder device driver (kernel module)
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

/* needed for virt_to_phys() */
#include <asm/io.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>

#include <asm/irq.h>

#include <linux/version.h>

/* our own stuff */
#include "hx280enc_h1.h"
#include <mach/power-gate.h> //fengwu
#include <linux/miscdevice.h> //fengwu
#include <linux/platform_device.h> //fengwu
#include "mach/clock.h"
#include <linux/clk.h>
#include <linux/poll.h>
static void initDriverPower(void);
static void deInitDriverPower(void);
static void powerDownDriver(void);
static void powerOnDriver(void);
static void closeDriverPower(void);
static void openDriverPower(void);

#define HX280_DEV_NAME "hx280"

/* module description */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Google Finland Oy");
MODULE_DESCRIPTION("Hantro 6280/7280/8270/8290/H1 Encoder driver");

/* this is ARM Integrator specific stuff */
//#define INTEGRATOR_LOGIC_MODULE0_BASE   0xC0000000
//#define INTEGRATOR_LOGIC_MODULE0_BASE     0x25100000 //fengwu
#define INTEGRATOR_LOGIC_MODULE0_BASE     IMAP_VENC264_BASE //fengwu
/*
#define INTEGRATOR_LOGIC_MODULE1_BASE   0xD0000000
#define INTEGRATOR_LOGIC_MODULE2_BASE   0xE0000000
#define INTEGRATOR_LOGIC_MODULE3_BASE   0xF0000000
*/

//#define VP_PB_INT_LT                    30
#define VP_PB_INT_LT                    GIC_VENC264_ID //fengwu
/*
#define INT_EXPINT1                     10
#define INT_EXPINT2                     11
#define INT_EXPINT3                     12
*/
/* these could be module params in the future */

#define ENC_IO_BASE                 INTEGRATOR_LOGIC_MODULE0_BASE
#define ENC_IO_SIZE                 (300 * 4)    /* bytes */

#define ENC_HW_ID1                  0x62800000
#define ENC_HW_ID2                  0x72800000
#define ENC_HW_ID3                  0x82700000
#define ENC_HW_ID4                  0x82900000
#define ENC_HW_ID5                  0x48310000

#define HX280ENC_BUF_SIZE           0

#define HX280ENC_IRAM_FLAG          1
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
//static void ResetAsic_venc(void);

#define HX280ENC_DEBUG
#ifdef HX280ENC_DEBUG
static void dump_regs(unsigned long data);
#endif

/* IRQ handler */
#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
static irqreturn_t hx280enc_isr(int irq, void *dev_id, struct pt_regs *regs);
#else
static irqreturn_t hx280enc_isr(int irq, void *dev_id);
#endif


static struct mutex venc_lock;
static spinlock_t dvfs_lock;
static struct semaphore venc_sema;
//static venc_param_t venc_param;     /* global variables structure */
/* for poll system call */
static wait_queue_head_t wait_encode;
static volatile unsigned int encode_poll_mark = 0;
static bool jpegDvfsClamp = 0;

typedef struct{
    bool reservedHW;
}venc_instance_t;

static int venc_probe(struct platform_device *pdev);
static int venc_remove(struct platform_device *pdev);
static int venc_suspend(struct platform_device *pdev, pm_message_t state);
static int venc_resume(struct platform_device *pdev);
static int hx280enc_mmap(struct file *filp, struct vm_area_struct *vma);
static long hx280enc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static int hx280enc_open(struct inode *inode, struct file *filp);
static int hx280enc_fasync(int fd, struct file *filp, int mode);
static int hx280enc_release(struct inode *inode, struct file *filp);
int __init hx280enc_init(void);
static unsigned int hx280venc_poll(struct file *file, poll_table *wait);

static struct file_operations hx280enc_fops = {
  mmap:hx280enc_mmap,
  open:hx280enc_open,
  release:hx280enc_release,
  unlocked_ioctl:hx280enc_ioctl,
  fasync:hx280enc_fasync,
  poll: hx280venc_poll, //fenwu 20150601
};


static struct miscdevice venc_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &hx280enc_fops,
    .name = HX280_DEV_NAME,
};

static struct platform_driver venc_plat = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "imap-venc",
    },
    .probe = venc_probe,
    .remove = venc_remove,
    .suspend = venc_suspend,
    .resume = venc_resume,
};

static int g_h1_clk = 300;
module_param(g_h1_clk, int, 0);

///////////////// begin ////////////////////////////////

static unsigned int driverOpenCount = 0;
static struct mutex driverOpLock;
static struct clk * driverClk = NULL;
static struct clk * driverbusClk = NULL;

#define DRIVER_NAME        "H1_H264_ENC"           // "H2_H265_ENC"          //  "G2_H265_DEC" 
#define CLK_GET_SYS_PARAM1 "imap-venc264"           //"imap-venc265"          //   "imap-venc264"                   
#define CLK_GET_SYS_PARAM2 "imap-venc264"          //"imap-venc265"          //   "imap-venc264"
#define DRIVER_CLK_RATE    (g_h1_clk*1000000)     // (g_h2_clk*1000000)     //  (g_g2_clk * 1000000)
#define DRIVER_POWER_BASE  SYSMGR_VENC264_BASE     // SYSMGR_VENC_265_BASE   //  SYSMGR_VDEC265_BASE

static void initDriverPower(void)
{
    mutex_init(&driverOpLock);
    driverOpenCount = 0;

    driverClk = clk_get_sys(CLK_GET_SYS_PARAM1, CLK_GET_SYS_PARAM2);
    clk_set_rate(driverClk, DRIVER_CLK_RATE);
    module_power_down(DRIVER_POWER_BASE);
    module_special_config(DRIVER_POWER_BASE, HX280ENC_IRAM_FLAG<<2); //switch iram to venc
    clk_disable_unprepare(driverClk);
}

static void deInitDriverPower(void)
{
    mutex_lock(&driverOpLock);
    if((driverOpenCount > 0) && driverClk)
    {
        module_power_down(DRIVER_POWER_BASE);
        module_special_config(DRIVER_POWER_BASE, 0); //switch iram to cpu
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
        module_special_config(DRIVER_POWER_BASE, 0); 
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
        module_special_config(DRIVER_POWER_BASE, HX280ENC_IRAM_FLAG<<2);
    }
    driverOpenCount++;
    mutex_unlock(&driverOpLock);
}

static void closeDriverPower(void)
{
    mutex_lock(&driverOpLock);
    if(driverOpenCount > 0)
    {
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

/* VM operations */
#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28))
static struct page *hx280enc_vm_nopage(struct vm_area_struct *vma,
                                       unsigned long address, int *type)
{
    PDEBUG("hx280enc_vm_nopage: problem with mem access\n");
    return NOPAGE_SIGBUS;   /* send a SIGBUS */
}
#else
static int hx280enc_vm_fault(struct vm_area_struct *vma,
                                      struct vm_fault *vmf)
{
    PDEBUG("hx280enc_vm_fault: problem with mem access\n");
    return VM_FAULT_SIGBUS; /* send a SIGBUS */
}
#endif

static void hx280enc_vm_open(struct vm_area_struct *vma)
{
    PDEBUG("hx280enc_vm_open:\n");
}

static void hx280enc_vm_close(struct vm_area_struct *vma)
{
    PDEBUG("hx280enc_vm_close:\n");
}

static struct vm_operations_struct hx280enc_vm_ops = {
  open:hx280enc_vm_open,
  close:hx280enc_vm_close,
#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28))
  nopage:hx280enc_vm_nopage,
#else
  fault:hx280enc_vm_fault,
#endif
};

/* the device's mmap method. The VFS has kindly prepared the process's
 * vm_area_struct for us, so we examine this to see what was requested.
 */

static int hx280enc_mmap(struct file *filp, struct vm_area_struct *vma)
{
    int result = -EINVAL;

    result = -EINVAL;

    vma->vm_ops = &hx280enc_vm_ops;

    return result;
}


static long hx280enc_ioctl(struct file *filp,
                          unsigned int cmd, unsigned long arg)
{
    int err = 0;
    unsigned int linkMode = 0; 
    /*
     * extract the type and number bitfields, and don't encode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if(_IOC_TYPE(cmd) != HX280ENC_IOC_MAGIC)
    {
        printk("Error! hx280enc_ioctl HX280ENC_IOC_MAGIC:%x, _IOC_TYPE(cmd):%x \n", HX280ENC_IOC_MAGIC, _IOC_TYPE(cmd));
        return -ENOTTY;
    }
    if(_IOC_NR(cmd) > HX280ENC_IOC_MAXNR)
    {
        printk("Error! hx280enc_ioctl HX280ENC_IOC_MAXNR:%x, _IOC_NR(cmd):%x \n", HX280ENC_IOC_MAXNR, _IOC_NR(cmd));
        return -ENOTTY;
    }

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
    {
        printk("hx280 ioctl read/write error!\n");
        return -EFAULT;
    }

    switch (cmd)
    {
    case HX280ENC_IOC_LINKMODE:
        if (copy_from_user(&linkMode, (unsigned int*) arg, 4))
        {
            return -EFAULT;
        }

        module_special_config(DRIVER_POWER_BASE, HX280ENC_IRAM_FLAG<<2 | (linkMode&0x01)<<1); 
        break;

    case HX280ENC_IOCGHWOFFSET:
        __put_user(hx280enc_data.iobaseaddr, (unsigned long *) arg);
        break;

    case HX280ENC_IOCGHWIOSIZE:
        __put_user(hx280enc_data.iosize, (unsigned int *) arg);
        break;

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

    default:
        printk("hx280 ioctl default::%d\n", cmd);
        break;
    }
    return 0;
}

static int hx280enc_open(struct inode *inode, struct file *filp)
{
    powerOnDriver();
    int result = 0;
    hx280enc_t *dev = &hx280enc_data;

    filp->private_data = (void *) dev;

    return result;
}

static int hx280enc_fasync(int fd, struct file *filp, int mode)
{
    hx280enc_t *dev = (hx280enc_t *) filp->private_data;

    return fasync_helper(fd, filp, mode, &dev->async_queue);
}

static unsigned int hx280venc_poll(struct file *file, poll_table *wait)
{
    unsigned int mask = 0;
    poll_wait(file, &wait_encode, wait);

    if (encode_poll_mark != 0) {
        mask = POLLIN | POLLRDNORM;
        encode_poll_mark = 0;
    }

    return mask;
}

static int hx280enc_release(struct inode *inode, struct file *filp)
{
#ifdef HX280ENC_DEBUG
    hx280enc_t *dev = (hx280enc_t *) filp->private_data;

    dump_regs((unsigned long) dev); /* dump the regs */
#endif

    /* remove this filp from the asynchronusly notified filp's */
    hx280enc_fasync(-1, filp, 0);

    powerDownDriver();

    PDEBUG("dev closed\n");
    return 0;
}


/* VFS methods */
int __init hx280enc_init(void)
{
    int result;

    /* Register platform device driver. */
    int ret = platform_driver_register(&venc_plat);
    if (unlikely(ret != 0)) {
        printk(KERN_ALERT "Register venc platform device driver error, %d.\n", ret);
        return ret;
    }

#if 0
    /* Register misc device driver. */
    ret = misc_register(&venc_miscdev);
    if (unlikely(ret != 0)) {
        printk(KERN_ALERT "Register venc misc device driver error, %d.\n", ret);
        return ret;
    }
#endif

    result = register_chrdev(hx280enc_major, HX280_DEV_NAME, &hx280enc_fops);
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

    hx280enc_data.iobaseaddr = base_port;
    hx280enc_data.iosize = ENC_IO_SIZE;
    hx280enc_data.irq = irq;
    hx280enc_data.async_queue = NULL;
    hx280enc_data.hwregs = NULL;
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
                             HX280_DEV_NAME, (void *) &hx280enc_data);
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

    return 0;

  err:
    //unregister_chrdev(hx280enc_major, HX280_DEV_NAME);
    printk(KERN_INFO "hx280enc: module not inserted\n");
    return result;
}

void __exit hx280enc_cleanup(void)
{
    writel(0, hx280enc_data.hwregs + 0x38); /* disable HW */
    writel(0, hx280enc_data.hwregs + 0x04); /* clear enc IRQ */

    /* free the encoder IRQ */
    if(hx280enc_data.irq != -1)
    {
        free_irq(hx280enc_data.irq, (void *) &hx280enc_data);
    }

    ReleaseIO();

    unregister_chrdev(hx280enc_major, HX280_DEV_NAME);

    printk(KERN_INFO "hx280enc: module removed\n");
    return;
}


static int ReserveIO(void)
{
    long int hwid;

    if( hx280enc_data.hwregs != 0)
    {
        printk("h1 no need ReserveIO. hx280enc_data.hwregs is none 0:%x\n", hx280enc_data.hwregs);
        return 0;
    }
    if(!request_mem_region
       (hx280enc_data.iobaseaddr, hx280enc_data.iosize, HX280_DEV_NAME))
    {
        printk(KERN_INFO "hx280enc: failed to reserve HW regs %x\n", hx280enc_data.iobaseaddr);
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
    if((((hwid >> 16) & 0xFFFF) != ((ENC_HW_ID1 >> 16) & 0xFFFF)) &&
       (((hwid >> 16) & 0xFFFF) != ((ENC_HW_ID2 >> 16) & 0xFFFF)) &&
       (((hwid >> 16) & 0xFFFF) != ((ENC_HW_ID3 >> 16) & 0xFFFF)) &&
       (((hwid >> 16) & 0xFFFF) != ((ENC_HW_ID4 >> 16) & 0xFFFF)) &&
       (((hwid >> 16) & 0xFFFF) != ((ENC_HW_ID5 >> 16) & 0xFFFF)))
    {
        printk(KERN_INFO "hx280enc: HW not found at 0x%08lx. hwid:%x(%x)\n",
               hx280enc_data.iobaseaddr, hwid, hwid >> 16);
        printk("hx280enc: valid hwid:%x %x %x %x %x\n", 
            ENC_HW_ID1,
            ENC_HW_ID2,
            ENC_HW_ID3,
            ENC_HW_ID4,
            ENC_HW_ID5);

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
    hx280enc_data.hwregs = 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
irqreturn_t hx280enc_isr(int irq, void *dev_id, struct pt_regs *regs)
#else
irqreturn_t hx280enc_isr(int irq, void *dev_id)
#endif
{
    hx280enc_t *dev = (hx280enc_t *) dev_id;
    u32 irq_status;

    irq_status = readl(dev->hwregs + 0x04);

    if(irq_status & 0x01)
    {

        /* clear enc IRQ and slice ready interrupt bit */
        writel(irq_status & (~0x101), dev->hwregs + 0x04);

        /* Handle slice ready interrupts. The reference implementation
         * doesn't signal slice ready interrupts to EWL.
         * The EWL will poll the slices ready register value. */
        if ((irq_status & 0x1FE) == 0x100)
        {
            PDEBUG("Slice ready IRQ handled!\n");
            return IRQ_HANDLED;
        }

        /* All other interrupts will be signaled to EWL. */
#if 0
        if(dev->async_queue)
            kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
        else
        {
            printk(KERN_WARNING
                   "hx280enc: IRQ received w/o anybody waiting for it!\n");
        }
#endif //fengwu 20150601 delete

        encode_poll_mark = 1;
        wake_up(&wait_encode);
        PDEBUG("IRQ handled!\n");
        return IRQ_HANDLED;
    }
    else
    {
        PDEBUG("IRQ received, but NOT handled!\n");
        return IRQ_NONE;
    }

}

void ResetAsic(hx280enc_t * dev)
{
    printk("hx280 ResetAsic\n");
    int i;

    writel(0, dev->hwregs + 0x38);

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

static int venc_probe(struct platform_device *pdev)
{
    int ret;

    module_power_on(SYSMGR_VENC264_BASE);

    init_waitqueue_head(&wait_encode);

    hx280enc_data.iobaseaddr = base_port;
    hx280enc_data.iosize = ENC_IO_SIZE;
    hx280enc_data.irq = irq;
    hx280enc_data.async_queue = NULL;
    hx280enc_data.hwregs = NULL;
    int result = ReserveIO();
    if(result < 0)
    {
        printk("venc_probe ReserveIO error!\n");
        return 0;
    }

    ResetAsic(&hx280enc_data);  /* reset hardware */

    /* get the IRQ line */
    if(irq != -1)
    {
        result = request_irq(irq, hx280enc_isr, IRQF_DISABLED | IRQF_SHARED,
                             HX280_DEV_NAME, (void *) &hx280enc_data);
        if(result == -EINVAL)
        {
            printk("hx280enc: Bad irq number or handler\n");
            ReleaseIO();
        }
        else if(result == -EBUSY)
        {
            printk(KERN_ERR "hx280enc: IRQ <%d> busy, change your config\n",
                   hx280enc_data.irq);
            ReleaseIO();
        }
    }
    else
    {
        printk(KERN_INFO "hx280enc: IRQ not in use!\n");
    }

    initDriverPower();

///////////////////// begin /////////////////////

   // enable clock 		
       /* struct clk * clk = clk_get_sys("imap-venc264", "imap-venc264");		
	if (IS_ERR(clk)) {		
	pr_err(" imap-venc264 clock not found: \n");		
	return -1;		
	}		
			
	clk_prepare_enable(clk);
      */
///////////////////// end //////////////////////////    
    
    return 0;
}

static int venc_remove(struct platform_device *pdev)
{
    printk("hx280 venc_remove \n" );
    deInitDriverPower();
    return 0;
}

static int venc_suspend(struct platform_device *pdev, pm_message_t state)
{
    closeDriverPower();
    return 0;
}

static int venc_resume(struct platform_device *pdev)
{
    openDriverPower();
    return 0;
}

static int __init venc_h1v6_init(void)
{
    int ret = 0;

#ifdef CONFIG_ARCH_APOLLO
    driverbusClk = clk_get_sys("imap-venc264-ctrl","imap-venc264-ctrl");
    if (IS_ERR(driverbusClk)) {
	    pr_err(" imap-venc264-ctrl clock not found! \n");
	    return -1;
    } 
    int err = clk_prepare_enable(driverbusClk);
    if (err) {
	    pr_err("imap-venc264-ctrl clock prepare enable fail!\n");
	    clk_disable_unprepare(driverbusClk);
	    clk_put(driverbusClk);
	    return err;
    }
#endif
 ////////////////////////////begin //////////////////////
    struct clk * clk = clk_get_sys("imap-venc264", "imap-venc264");
    int clkFre = clk_get_rate(clk);

    clk_set_rate(clk, g_h1_clk * 1000000);
    clk_prepare_enable(clk);

    clkFre = clk_get_rate(clk);
    printk("new h1 clock frequency:%d\n", clkFre);

  ///////////////////////////////end /////////////////////

    /* Register platform device driver. */
    ret = platform_driver_register(&venc_plat);
    if (unlikely(ret != 0)) {
        printk(KERN_ALERT "Register venc platform device driver error, %d.\n", ret);
        return ret;
    }

    /* Register misc device driver. */
    ret = misc_register(&venc_miscdev);
    if (unlikely(ret != 0)) {
        printk(KERN_ALERT "Register venc misc device driver error, %d.\n", ret);
        return ret;
    }

    return 0;
}


static void __exit venc_h1v6_exit(void)
{
    misc_deregister(&venc_miscdev);
    platform_driver_unregister(&venc_plat);

    /* free the encoder IRQ */
    if(hx280enc_data.irq != -1)
    {
        free_irq(hx280enc_data.irq, (void *) &hx280enc_data);
    }

    ReleaseIO();
}

module_init(venc_h1v6_init);
module_exit(venc_h1v6_exit);
//module_init(hx280enc_init);
//module_exit(hx280enc_cleanup);


