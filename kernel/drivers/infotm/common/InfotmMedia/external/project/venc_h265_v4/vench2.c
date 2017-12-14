/*------------------------------------------------------------------------------
  --                                                                         --
  --       This software is confidential and proprietary and may be used     --
  --        only as expressly authorized by a licensing agreement from       --
  --                                                                         --
  --                            Verisilicon.                                 --
  --                                                                         --
  --                   (C) COPYRIGHT 2014 VERISILICON                        --
  --                            ALL RIGHTS RESERVED                          --
  --                                                                         --
  --                 The entire notice above must be reproduced              --
  --                  on all copies and should not be removed.               --
  --                                                                         --
  -----------------------------------------------------------------------------
  --
  --  Abstract : H2 Encoder device driver (kernel module)
  --
  ----------------------------------------------------------------------------*/

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
#include <linux/platform_device.h>
#include <linux/miscdevice.h>


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
#include "vench2.h"
#include "mach/power-gate.h"
#include "mach/clk.h"

/* module description */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Verisilicon");
MODULE_DESCRIPTION("H2 Encoder driver");

/* this is ARM Integrator specific stuff */
#define INTEGRATOR_LOGIC_MODULE0_BASE   0x25000000
/*
#define INTEGRATOR_LOGIC_MODULE1_BASE   0xD0000000
#define INTEGRATOR_LOGIC_MODULE2_BASE   0xE0000000
#define INTEGRATOR_LOGIC_MODULE3_BASE   0xF0000000
*/

#define VP_PB_INT_LT                    130
/*
#define INT_EXPINT1                     10
#define INT_EXPINT2                     11
#define INT_EXPINT3                     12
*/
/* these could be module params in the future */

#define HX280ENC_NAME  "venc265"

#define ENC_IO_BASE                 INTEGRATOR_LOGIC_MODULE0_BASE
#define ENC_IO_SIZE                 (376 * 4)    /* bytes */


#define ENC_HW_ID1                  0x48320100

#define HX280ENC_BUF_SIZE           0

#define HX280ENC_CLK    100000000

static struct mutex driver_op_lock;

#define CLK_GET_SYS_PARAM1      "imap-venc265"          //   "imap-venc264"   // "imap-vdec265"
#define CLK_GET_SYS_PARAM2      "imap-venc265"          //   "imap-venc264"   // "imap-vdec265"

unsigned long base_port = INTEGRATOR_LOGIC_MODULE0_BASE;
int g_irq = VP_PB_INT_LT;
int g_clk = HX280ENC_CLK;

/* module_param(name, type, perm) */
module_param(base_port, ulong, 0);
module_param(g_irq, int, 0);
module_param(g_clk, int, 0);


/* here's all the must remember stuff */
typedef struct
{
    char *buffer;
    unsigned int buffsize;
    unsigned long iobaseaddr;
    unsigned int iosize;
    volatile u8 *hwregs;
    unsigned int irq;
    int32_t irq_state;
    struct fasync_struct *async_queue;
    struct class *cls;
    struct device *dev;
    int user_num;
} hx280enc_t;

/* dynamic allocation? */
static hx280enc_t hx280enc_data;

static int reserve_io(void);
static void release_io(void);
static int  venc_power_on(void);
static int  venc_power_off(void);
static void dumps_registers(int base, int num);
static void reset_hx280_asic(hx280enc_t * dev);



//#define HX280ENC_DEBUG
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

DEFINE_SPINLOCK(owner_lock);
DECLARE_WAIT_QUEUE_HEAD(enc_wait_queue);

static int check_enc_irq(hx280enc_t *dev)
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
unsigned int wait_enc_ready(hx280enc_t *dev)
{
    PDEBUG("wait_event_interruptible ENC\n");

    if(wait_event_interruptible(enc_wait_queue, check_enc_irq(dev)))
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
            PDEBUG("ioctl offset\n");
            __put_user(hx280enc_data.iobaseaddr, (unsigned long *) arg);
            break;

        case HX280ENC_IOCGHWIOSIZE:
            PDEBUG("ioctl size\n");
            __put_user(hx280enc_data.iosize, (unsigned int *) arg);
            break;

        case HX280ENC_IOCG_CORE_WAIT:
            PDEBUG("ioctl core wait\n");
            {
                tmp = wait_enc_ready(&hx280enc_data);
                __put_user(asic_status, (unsigned int *)arg);
                PDEBUG("ioctl core wait ok\n");
                break;
            }
        default:
            PDEBUG("ioctl unknown\n");
            break;
    }
    return 0;
}

static int hx280enc_open(struct inode *inode, struct file *filp)
{
    int result = 0;
    hx280enc_t *dev = &hx280enc_data;
    PDEBUG("%s\n", __func__);
    mutex_lock(&driver_op_lock);
    if(dev->user_num == 0){
        if( venc_power_on() <0 ){
            PERROR("open h280enc device error\n");
            result =  EBUSY;
        }
        else{
            reserve_io();
        }
    }
    if(result != EBUSY) {
        dev->user_num++;
         filp->private_data = (void *) dev;
    }
    PDEBUG("h265 encoder open ->%d\n", dev->user_num );

    mutex_unlock(&driver_op_lock);
    
    return result;
}
static int hx280enc_release(struct inode *inode, struct file *filp)
{
    int result = 0;
    hx280enc_t *dev = (hx280enc_t *) filp->private_data;
    PDEBUG("%s\n",  __func__);
    mutex_lock(&driver_op_lock);
    if(dev->user_num > 0){
            dev->user_num--;
    }

    if(dev->user_num  == 0){
        if( venc_power_off() <0 ){
            PERROR("close h280enc device error\n");
            result =  EBUSY;
        }
        release_io();
     }

    PDEBUG("h265 encoder close ->%d\n", dev->user_num );
    mutex_unlock(&driver_op_lock);

    PDEBUG("dev closed  ok\n");
    return result;
}

/* VFS methods */
static struct file_operations hx280enc_fops = {
    .owner= THIS_MODULE,
    .open = hx280enc_open,
    .release = hx280enc_release,
    .unlocked_ioctl = hx280enc_ioctl,
    .fasync = NULL,
};
static void dumps_registers(int base,int num){
#if 1
    return;
#else
    uint32_t  *point  = NULL;
    int i = 0;
    PDEBUG("h265 ->%s\n",  __func__);
    point = ioremap_nocache(base, num);
    if(NULL == point){
        PERROR("remap system manager error ->0x%X\n", base);
        return;
    }
    PDEBUG("system manager ->\n");
    for(i = 0; i< 10 ;i++){
        printk("reg: %3X -->%3X\n", i*4, point[i]);
    }
    iounmap(point);
#endif
}
static int power_on_counter = 0;
static int  venc_power_on(void){
    struct clk * driver_clk = NULL;
    PDEBUG("%s\n", __func__);
    driver_clk = clk_get_sys(CLK_GET_SYS_PARAM1, CLK_GET_SYS_PARAM2);
    if(driver_clk == NULL){
        PERROR(KERN_ERR"h265 get clk error\n");
        return -1;
    }
    clk_set_rate(driver_clk, g_clk);
    clk_prepare_enable(driver_clk);
    PMSG("clk set result ->%ld\n", clk_get_rate(driver_clk));
    clk_put(driver_clk);
    module_power_on(SYSMGR_VENC265_BASE);
    dumps_registers(SYSMGR_VENC265_BASE, 10);
    power_on_counter++;
    return 0;
}
static int  venc_power_off(void){
    struct clk * driver_clk = NULL;
    PDEBUG("%s\n", __func__);
    if(power_on_counter  > 0 ){
        driver_clk = clk_get_sys(CLK_GET_SYS_PARAM1, CLK_GET_SYS_PARAM2);
        if(driver_clk == NULL){
            PERROR("h265 get clk error\n");
            return -1;
        }PDEBUG("%s\n", __func__);
        clk_disable_unprepare(driver_clk);
        PDEBUG("%s\n", __func__);
        clk_put(driver_clk);
        PDEBUG("%s\n", __func__);
    }
    module_power_down(SYSMGR_VENC265_BASE);
    PDEBUG("%s\n", __func__);
    power_on_counter--;
    return 0;

}
static int hx280_265_probe(struct platform_device *pdev)
{
    int result ;
    PDEBUG("%s\n", __func__);
    //power on and init data
    if(venc_power_on() < 0){
        PERROR("power on module errror\n");
        goto err;
    }
    hx280enc_data.iobaseaddr = base_port;
    hx280enc_data.iosize = ENC_IO_SIZE;
    hx280enc_data.irq = g_irq;
    hx280enc_data.async_queue = NULL;
    hx280enc_data.hwregs = NULL;
    hx280enc_data.cls =  NULL;
    hx280enc_data.dev = NULL;
    hx280enc_data.irq_state = 0;
    hx280enc_data.user_num = 0;

    //reserve io and reset asic
    result = reserve_io();
    if(result < 0)
    {
        PERROR( "%s reserve_io error: %d\n", __func__, result);
        goto err;
    }
    reset_hx280_asic(&hx280enc_data);  /* reset hardware */

    /* get the IRQ line */
    if(g_irq != -1)
    {
        result = request_irq(g_irq, hx280enc_isr,
                             IRQF_DISABLED | IRQF_SHARED,
                             HX280ENC_NAME, (void *) &hx280enc_data);
        if(result == -EINVAL)
        {
            PERROR("hx280enc: Bad irq number or handler\n");
            release_io();
            goto err;
        }
        else if(result == -EBUSY)
        {
            PERROR("hx280enc: IRQ <%d> busy, change your config\n",
                   hx280enc_data.irq);
            release_io();
            goto err;
        }
        else{
            PMSG("irq ready\n");
        }
    }
    else
    {
        PMSG("hx280enc: IRQ not in use!\n");
    }
    hx280enc_data.irq_state = 1;
    release_io();
    venc_power_off();
    return 0;
err:
    PERROR("hx280enc: probe error. \n");
    if(hx280enc_data.irq != -1 && hx280enc_data.irq_state == 1){
        free_irq(hx280enc_data.irq, (void *) &hx280enc_data);
    }
    if(hx280enc_data.hwregs != NULL){
        release_io();
        };
    venc_power_off();
    return -1;
    
}

static int hx280_265_remove(struct platform_device *pdev)
{
    PDEBUG("%s\n", __func__);
    
    if(hx280enc_data.hwregs != NULL){
        reset_hx280_asic(&hx280enc_data);
        release_io();
     }
    venc_power_off();
    
    PDEBUG("%s\n" , __func__);

    if(hx280enc_data.irq != -1  && hx280enc_data.irq_state == 1)
    {
        free_irq(hx280enc_data.irq, (void *) &hx280enc_data);
    }
    return 0; 
}
static int hx280_265_suspend(struct platform_device *pdev, pm_message_t state)
{
    PDEBUG("h265 ->%d ->%s\n",  __LINE__, __func__);
    if(hx280enc_data.hwregs != NULL){
        release_io();
    }
    venc_power_off();
    return 0;
}

static int hx280_265_resume(struct platform_device *pdev)
{
    PDEBUG("h265 ->%s\n", __func__);
    if(venc_power_on() < 0) {
        PERROR(" %s error\n", __func__);
        return -1;
    }
    reserve_io();
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



static struct miscdevice hx280_265_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &hx280enc_fops,
    .name = HX280ENC_NAME,
};


static void platform_h2_release(struct device * dev) { return ; }
struct platform_device imap_hx280_265_device = {
    //.name = "imap-hx280-265",
     .name =HX280ENC_NAME,
    .id = 0,
    .dev = {
        .release = platform_h2_release,
    }, 
    .num_resources = ARRAY_SIZE(imap_hx280_265_res),
};

static struct platform_driver hx280_265_plat = {
    .driver = {
        .owner = THIS_MODULE,
        //.name = "imap-hx280-265",
        .name =HX280ENC_NAME,
    },
    .probe = hx280_265_probe,
    .remove = hx280_265_remove,
    .suspend = hx280_265_suspend,
    .resume = hx280_265_resume,
};

int __init hx280enc_init(void)
{
    int result = 0;
    PDEBUG("h265  ->%s\n" , __func__);
    mutex_init(&driver_op_lock);

    PMSG("hx280enc: module init - base_port=0x%08lx irq=%i\n",
            base_port, g_irq);

    
    result = platform_device_add_resources(&imap_hx280_265_device, imap_hx280_265_res, ARRAY_SIZE(imap_hx280_265_res));
    if(result != 0) {
        PERROR( "add hx280_265 device resource error \n");
        goto err;
    }

    PDEBUG( "%s -> platform_device_register\n", __func__);
    result = platform_device_register(&imap_hx280_265_device);
    if(result != 0) {
        PERROR( "register hx280_265 device error \n");
        goto err;
    }

    /* Register platform device driver. */
    PDEBUG( "%s -> platform_driver_register\n", __func__);
    result = platform_driver_register(&hx280_265_plat);
    if (unlikely(result != 0)) {
        PERROR( "Register hx280_265 platform device driver error, %d.\n", result);
        goto err;
    }
    /* Register misc device driver. */
    PDEBUG( "%s -> misc_register\n", __func__);
    result = misc_register(&hx280_265_miscdev);
    if (unlikely(result != 0)) {
        PERROR( "Register hx280_265 misc device driver error, %d.\n", result);
        goto err;
    }
    return 0;

err:

     PERROR( "%s -> misc_deregister\n", __func__);
    misc_deregister(&hx280_265_miscdev);
    PERROR( "%s -> platform_driver_unregister\n", __func__);
    platform_driver_unregister(&hx280_265_plat);
    PERROR( "%s -> platform_device_unregister\n", __func__);
    platform_device_unregister(&imap_hx280_265_device);


    return result;
}

void __exit hx280enc_cleanup(void)
{

    PDEBUG("%s\n" , __func__);

    //writel(0, hx280enc_data.hwregs + 0x14); /* disable HW */
    //writel(0, hx280enc_data.hwregs + 0x04); /* clear enc IRQ */
    
    PDEBUG( "%s -> misc_deregister\n", __func__);
    misc_deregister(&hx280_265_miscdev);
    PDEBUG( "%s -> platform_driver_unregister\n", __func__);
    platform_driver_unregister(&hx280_265_plat);
    PDEBUG( "%s -> platform_device_unregister\n", __func__);
    platform_device_unregister(&imap_hx280_265_device);
    venc_power_off();

    PDEBUG(KERN_INFO "hx280enc: module removed\n");
    return;
}



static int reserve_io(void)
{
    long int hwid;

    PDEBUG( "hx280enc: %lX %d\n", hx280enc_data.iobaseaddr, hx280enc_data.iosize);
    if(hx280enc_data.hwregs != NULL){
        PERROR("hw regs have be mapped ,please check it\n");
        return -1;
    }
    if(!request_mem_region
            (hx280enc_data.iobaseaddr, hx280enc_data.iosize, "hx280enc"))
    {
        PERROR("hx280enc: failed to reserve HW regs\n");
        return -EBUSY;
    }

    hx280enc_data.hwregs =
        (volatile u8 *) ioremap_nocache(hx280enc_data.iobaseaddr,
                hx280enc_data.iosize);

    if(hx280enc_data.hwregs == NULL)
    {
        PERROR("hx280enc: failed to ioremap HW regs\n");
        return -EBUSY;
    }

    PDEBUG("hx280enc: %d  0x%p\n", __LINE__, hx280enc_data.hwregs);
    hwid = readl(hx280enc_data.hwregs);
#if 1
    /* check for encoder HW ID */
    if((((hwid >> 16) & 0xFFFF) != ((ENC_HW_ID1 >> 16) & 0xFFFF)))
    {
        PERROR("hx280enc: HW not found at 0x%08lx\n",
                hx280enc_data.iobaseaddr);

        release_io();
        return -EBUSY;
    }
#endif
    PDEBUG(
            "hx280enc: HW at base <0x%08lx> with ID <0x%08lx>\n",
            hx280enc_data.iobaseaddr, hwid);

    return 0;
}

static void release_io(void)
{
    PDEBUG("%s\n",__func__);
    if(hx280enc_data.hwregs)
        iounmap((void *) hx280enc_data.hwregs);
    release_mem_region(hx280enc_data.iobaseaddr, hx280enc_data.iosize);
    hx280enc_data.hwregs = NULL;
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
    PDEBUG("%s ->%X\n",__func__, dev->hwregs);
    irq_status = readl(dev->hwregs + 0x04);
    PDEBUG("%s\n",__func__);
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

void reset_hx280_asic(hx280enc_t * dev)
{
    int i;
    PDEBUG("%s\n", __func__);
    if(dev->hwregs == NULL){
        PERROR("error, no regs mapped\n");
        return ;
    }
    writel(0, dev->hwregs + 0x14);
    for(i = 4; i < dev->iosize; i += 4)
    {
        writel(0, dev->hwregs + i);
    }
}

#ifdef HX280ENC_DEBUG
void dump_regs(unsigned long data)
{
    return;
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


module_init(hx280enc_init);
module_exit(hx280enc_cleanup);


