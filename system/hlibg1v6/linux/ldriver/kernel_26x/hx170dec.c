/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : x170 Decoder device driver (kernel module)
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: hx170dec.c,v $
--  $Date: 2010/02/08 14:08:00 $
--  $Revision: 1.12 $
--
------------------------------------------------------------------------------*/

#include <linux/kernel.h>
#include <linux/module.h>
/* needed for __init,__exit directives */
#include <linux/init.h>
/* needed for remap_pfn_range
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

/* needed for virt_to_phys() */
#include <asm/io.h>
#include <linux/pci.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>

#include <asm/irq.h>

#include <linux/version.h>

/* our own stuff */
#include "hx170dec.h"

/* module description */
MODULE_LICENSE("Proprietary");
MODULE_AUTHOR("Hantro Products Oy");
MODULE_DESCRIPTION("driver module for 8170/81990 Hantro decoder/pp");

/* Decoder interrupt register */
#define X170_INTERRUPT_REGISTER_DEC     (1*4)
#define X170_INTERRUPT_REGISTER_PP      (60*4)

/* Logic module base address */
#define HXDEC_LOGIC_MODULE0_BASE   0xC0000000

#define VP_PB_INT_LT                    30

#define INT_EXPINT1                     10
#define INT_EXPINT2                     11
#define INT_EXPINT3                     12

/* these could be module params in the future */

#define DEC_IO_BASE                 HXDEC_LOGIC_MODULE0_BASE
#define DEC_IO_SIZE                 ((100+1) * 4)   /* bytes */
#define DEC_IRQ                     VP_PB_INT_LT

#define HX_DEC_INTERRUPT_BIT        0x100
#define HX_PP_INTERRUPT_BIT         0x100

static const int DecHwId[] = { 0x8190, 0x8170, 0x9170, 0x9190, 0x6731 };

static u32 hx_pp_instance = 0;
static u32 hx_dec_instance = 0;

unsigned long base_port = HXDEC_LOGIC_MODULE0_BASE;
int irq = DEC_IRQ;

/* module_param(name, type, perm) */
module_param(base_port, ulong, 0);
module_param(irq, int, 0);

/* and this is our MAJOR; use 0 for dynamic allocation (recommended)*/
static int hx170dec_major = 0;

/* here's all the must remember stuff */
typedef struct
{
    char *buffer;
    unsigned long iobaseaddr;
    unsigned int iosize;
    volatile u8 *hwregs;
    int irq;
    struct fasync_struct *async_queue_dec;
    struct fasync_struct *async_queue_pp;
} hx170dec_t;

static hx170dec_t hx170dec_data;    /* dynamic allocation? */

#ifdef HW_PERFORMANCE
static struct timeval end_time;
#endif

static int ReserveIO(void);
static void ReleaseIO(void);

static void ResetAsic(hx170dec_t * dev);

#ifdef HX170DEC_DEBUG
static void dump_regs(unsigned long data);
#endif

/* IRQ handler */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
static irqreturn_t hx170dec_isr(int irq, void *dev_id, struct pt_regs *regs);
#else
static irqreturn_t hx170dec_isr(int irq, void *dev_id);
#endif

/*------------------------------------------------------------------------------
    Function name   : hx170dec_ioctl
    Description     : communication method to/from the user space

    Return type     : int
------------------------------------------------------------------------------*/

static int hx170dec_ioctl(struct inode *inode, struct file *filp,
                          unsigned int cmd, unsigned long arg)
{
    int err = 0;

#ifdef HW_PERFORMANCE
    struct timeval *end_time_arg;
#endif

    PDEBUG("ioctl cmd 0x%08ux\n", cmd);
    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if(_IOC_TYPE(cmd) != HX170DEC_IOC_MAGIC)
        return -ENOTTY;
    if(_IOC_NR(cmd) > HX170DEC_IOC_MAXNR)
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
    case HX170DEC_IOC_CLI:
        disable_irq(hx170dec_data.irq);
        break;

    case HX170DEC_IOC_STI:
        enable_irq(hx170dec_data.irq);
        break;
    case HX170DEC_IOCGHWOFFSET:
        __put_user(hx170dec_data.iobaseaddr, (unsigned long *) arg);
        break;
    case HX170DEC_IOCGHWIOSIZE:
        __put_user(hx170dec_data.iosize, (unsigned int *) arg);
        break;
    case HX170DEC_PP_INSTANCE:
        filp->private_data = &hx_pp_instance;
        break;

#ifdef HW_PERFORMANCE
    case HX170DEC_HW_PERFORMANCE:
        end_time_arg = (struct timeval *) arg;
        end_time_arg->tv_sec = end_time.tv_sec;
        end_time_arg->tv_usec = end_time.tv_usec;
        break;
#endif
    }
    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : hx170dec_open
    Description     : open method

    Return type     : int
------------------------------------------------------------------------------*/

static int hx170dec_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &hx_dec_instance;

    PDEBUG("dev opened\n");
    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : hx170dec_fasync
    Description     : Method for signing up for a interrupt

    Return type     : int
------------------------------------------------------------------------------*/

static int hx170dec_fasync(int fd, struct file *filp, int mode)
{

    hx170dec_t *dev = &hx170dec_data;
    struct fasync_struct **async_queue;

    /* select which interrupt this instance will sign up for */

    if(((u32 *) filp->private_data) == &hx_dec_instance)
    {
        /* decoder */
        PDEBUG("decoder fasync called %d %x %d %x\n",
               fd, (u32) filp, mode, (u32) & dev->async_queue_dec);

        async_queue = &dev->async_queue_dec;
    }
    else
    {
        /* pp */
        PDEBUG("pp fasync called %d %x %d %x\n",
               fd, (u32) filp, mode, (u32) & dev->async_queue_pp);
        async_queue = &dev->async_queue_pp;
    }

    return fasync_helper(fd, filp, mode, async_queue);
}

/*------------------------------------------------------------------------------
    Function name   : hx170dec_release
    Description     : Release driver

    Return type     : int
------------------------------------------------------------------------------*/

static int hx170dec_release(struct inode *inode, struct file *filp)
{

    /* hx170dec_t *dev = &hx170dec_data; */

    if(filp->f_flags & FASYNC)
    {
        /* remove this filp from the asynchronusly notified filp's */
        hx170dec_fasync(-1, filp, 0);
    }

    PDEBUG("dev closed\n");
    return 0;
}

/* VFS methods */
static struct file_operations hx170dec_fops = {
  open:hx170dec_open,
  release:hx170dec_release,
  ioctl:hx170dec_ioctl,
  fasync:hx170dec_fasync,
};

/*------------------------------------------------------------------------------
    Function name   : hx170dec_init
    Description     : Initialize the driver

    Return type     : int
------------------------------------------------------------------------------*/

int __init hx170dec_init(void)
{
    int result;

    PDEBUG("module init\n");

    printk(KERN_INFO "hx170dec: dec/pp kernel module. %s \n", "$Revision: 1.12 $");
    printk(KERN_INFO "hx170dec: supports 8170 and 8190 hardware \n");
    printk(KERN_INFO "hx170dec: base_port=0x%08lx irq=%i\n", base_port, irq);

    hx170dec_data.iobaseaddr = base_port;
    hx170dec_data.iosize = DEC_IO_SIZE;
    hx170dec_data.irq = irq;

    hx170dec_data.async_queue_dec = NULL;
    hx170dec_data.async_queue_pp = NULL;

    result = register_chrdev(hx170dec_major, "hx170dec", &hx170dec_fops);
    if(result < 0)
    {
        printk(KERN_INFO "hx170dec: unable to get major %d\n", hx170dec_major);
        goto err;
    }
    else if(result != 0)    /* this is for dynamic major */
    {
        hx170dec_major = result;
    }

    result = ReserveIO();
    if(result < 0)
    {
        goto err;
    }

    ResetAsic(&hx170dec_data);  /* reset hardware */
    /* get the IRQ line */
    if(irq > 0)
    {
        result = request_irq(irq, hx170dec_isr,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
                             SA_INTERRUPT | SA_SHIRQ,
#else
                             IRQF_DISABLED | IRQF_SHARED,
#endif
                             "hx170dec", (void *) &hx170dec_data);
        if(result != 0)
        {
            if(result == -EINVAL)
            {
                printk(KERN_ERR "hx170dec: Bad irq number or handler\n");
            }
            else if(result == -EBUSY)
            {
                printk(KERN_ERR "hx170dec: IRQ <%d> busy, change your config\n",
                       hx170dec_data.irq);
            }

            ReleaseIO();
            goto err;
        }
    }
    else
    {
        printk(KERN_INFO "hx170dec: IRQ not in use!\n");
    }

    printk(KERN_INFO "hx170dec: module inserted. Major = %d\n", hx170dec_major);

    return 0;

  err:
    printk(KERN_INFO "hx170dec: module not inserted\n");
    unregister_chrdev(hx170dec_major, "hx170dec");
    return result;
}

/*------------------------------------------------------------------------------
    Function name   : hx170dec_cleanup
    Description     : clean up

    Return type     : int
------------------------------------------------------------------------------*/

void __exit hx170dec_cleanup(void)
{
    hx170dec_t *dev = (hx170dec_t *) & hx170dec_data;

    /* clear dec IRQ */
    writel(0, dev->hwregs + X170_INTERRUPT_REGISTER_DEC);
    /* clear pp IRQ */
    writel(0, dev->hwregs + X170_INTERRUPT_REGISTER_PP);

#ifdef HX170DEC_DEBUG
    dump_regs((unsigned long) dev); /* dump the regs */
#endif

    /* free the IRQ */
    if(dev->irq != -1)
    {
        free_irq(dev->irq, (void *) dev);
    }

    ReleaseIO();

    unregister_chrdev(hx170dec_major, "hx170dec");

    printk(KERN_INFO "hx170dec: module removed\n");
    return;
}

module_init(hx170dec_init);
module_exit(hx170dec_cleanup);

static int CheckHwId(hx170dec_t * dev)
{
    long int hwid;

    size_t numHw = sizeof(DecHwId) / sizeof(*DecHwId);

    hwid = readl(dev->hwregs);
    printk(KERN_INFO "hx170dec: HW ID=0x%08lx\n", hwid);

    hwid = (hwid >> 16) & 0xFFFF;   /* product version only */

    while(numHw--)
    {
        if(hwid == DecHwId[numHw])
        {
            printk(KERN_INFO "hx170dec: Compatible HW found at 0x%08lx\n",
                   dev->iobaseaddr);
            return 1;
        }
    }

    printk(KERN_INFO "hx170dec: No Compatible HW found at 0x%08lx\n",
           dev->iobaseaddr);
    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : ReserveIO
    Description     : IO reserve

    Return type     : int
------------------------------------------------------------------------------*/
static int ReserveIO(void)
{
    if(!request_mem_region
       (hx170dec_data.iobaseaddr, hx170dec_data.iosize, "hx170dec"))
    {
        printk(KERN_INFO "hx170dec: failed to reserve HW regs\n");
        return -EBUSY;
    }

    hx170dec_data.hwregs =
        (volatile u8 *) ioremap_nocache(hx170dec_data.iobaseaddr,
                                        hx170dec_data.iosize);

    if(hx170dec_data.hwregs == NULL)
    {
        printk(KERN_INFO "hx170dec: failed to ioremap HW regs\n");
        ReleaseIO();
        return -EBUSY;
    }

    /* check for correct HW */
    if(!CheckHwId(&hx170dec_data))
    {
        ReleaseIO();
        return -EBUSY;
    }

    return 0;
}

/*------------------------------------------------------------------------------
    Function name   : releaseIO
    Description     : release

    Return type     : void
------------------------------------------------------------------------------*/

static void ReleaseIO(void)
{
    if(hx170dec_data.hwregs)
        iounmap((void *) hx170dec_data.hwregs);
    release_mem_region(hx170dec_data.iobaseaddr, hx170dec_data.iosize);
}

/*------------------------------------------------------------------------------
    Function name   : hx170dec_isr
    Description     : interrupt handler

    Return type     : irqreturn_t
------------------------------------------------------------------------------*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
irqreturn_t hx170dec_isr(int irq, void *dev_id, struct pt_regs *regs)
#else
irqreturn_t hx170dec_isr(int irq, void *dev_id)
#endif
{
    unsigned int handled = 0;

    hx170dec_t *dev = (hx170dec_t *) dev_id;
    u32 irq_status_dec;
    u32 irq_status_pp;

    handled = 0;

    /* interrupt status register read */
    irq_status_dec = readl(dev->hwregs + X170_INTERRUPT_REGISTER_DEC);
    irq_status_pp = readl(dev->hwregs + X170_INTERRUPT_REGISTER_PP);

    if((irq_status_dec & HX_DEC_INTERRUPT_BIT) ||
       (irq_status_pp & HX_PP_INTERRUPT_BIT))
    {

        if(irq_status_dec & HX_DEC_INTERRUPT_BIT)
        {
#ifdef HW_PERFORMANCE
            do_gettimeofday(&end_time);
#endif
            /* clear dec IRQ */
            writel(irq_status_dec & (~HX_DEC_INTERRUPT_BIT),
                   dev->hwregs + X170_INTERRUPT_REGISTER_DEC);
            /* fasync kill for decoder instances */
            if(dev->async_queue_dec != NULL)
            {
                kill_fasync(&dev->async_queue_dec, SIGIO, POLL_IN);
            }
            else
            {
                printk(KERN_WARNING
                       "hx170dec: DEC IRQ received w/o anybody waiting for it!\n");
            }
            PDEBUG("decoder IRQ received!\n");
        }

        if(irq_status_pp & HX_PP_INTERRUPT_BIT)
        {
#ifdef HW_PERFORMANCE
            do_gettimeofday(&end_time);
#endif
            /* clear pp IRQ */
            writel(irq_status_pp & (~HX_PP_INTERRUPT_BIT),
                   dev->hwregs + X170_INTERRUPT_REGISTER_PP);

            /* kill fasync for PP instances */
            if(dev->async_queue_pp != NULL)
            {
                kill_fasync(&dev->async_queue_pp, SIGIO, POLL_IN);
            }
            else
            {
                printk(KERN_WARNING
                       "hx170dec: PP IRQ received w/o anybody waiting for it!\n");
            }
            PDEBUG("pp IRQ received!\n");
        }

        handled = 1;
    }
    else
    {
        PDEBUG("IRQ received, but not x170's!\n");
    }

    return IRQ_RETVAL(handled);
}

/*------------------------------------------------------------------------------
    Function name   : ResetAsic
    Description     : reset asic

    Return type     :
------------------------------------------------------------------------------*/

void ResetAsic(hx170dec_t * dev)
{
    int i;

    writel(0, dev->hwregs + 0x04);

    for(i = 4; i < dev->iosize; i += 4)
    {
        writel(0, dev->hwregs + i);
    }
}

/*------------------------------------------------------------------------------
    Function name   : dump_regs
    Description     : Dump registers

    Return type     :
------------------------------------------------------------------------------*/
#ifdef HX170DEC_DEBUG
void dump_regs(unsigned long data)
{
    hx170dec_t *dev = (hx170dec_t *) data;
    int i;

    PDEBUG("Reg Dump Start\n");
    for(i = 0; i < dev->iosize; i += 4)
    {
        PDEBUG("\toffset %02X = %08X\n", i, readl(dev->hwregs + i));
    }
    PDEBUG("Reg Dump End\n");
}
#endif
