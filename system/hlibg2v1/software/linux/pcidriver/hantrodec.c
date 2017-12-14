/* Copyright 2012 Google Inc. All Rights Reserved. */

#include "hantrodec.h"
#include "../dwl/dwl_defs.h"

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/pci.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/wait.h>
#include <linux/delay.h>

#define HANTRODEC_DEBUG

/* Address translation from CPU bus address to PCI bus address. */
/* TODO(mheikkinen) Now set separately in memalloc and kernel driver,
 * should this be set in a single place. */
#define HLINA_TRANSL_BASE               0x0

/* TODO(mheikkinen) These are the Xilinx defaults. */
#define PCI_VENDOR_ID_HANTRO            0x10ee
#define PCI_DEVICE_ID_HANTRO_PCI       0x7011
/* Base address got control register */
#define PCI_CONTROL_BAR                0
/* PCIe hantro driver offse in control register */
#define HANTRO_REG_OFFSET               0x0
/* Base address of PCI base address translation */
#define HLINA_ADDR_TRANSL_REG            0x20c/4

#define HXDEC_MAX_CORES                 1

#define HANTRO_DEC_REGS                 184
#define HANTRO_PP_REGS                  41  /* No separate PP regs. */

#define HANTRO_DEC_FIRST_REG            0
#define HANTRO_DEC_LAST_REG             HANTRO_DEC_REGS-1
#define HANTRO_PP_FIRST_REG             60
#define HANTRO_PP_LAST_REG              100

#define HXDEC_NO_IRQ                    -1

/* module defaults */
#define DEC_IO_SIZE             (HANTRO_DEC_REGS * 4) /* bytes, PP regs included
                                                         within dec regs. */
#define DEC_IRQ                 HXDEC_NO_IRQ

static const int DecHwId[] =
{
        0x6732
};

unsigned long base_port = -1;

u32 multicorebase[HXDEC_MAX_CORES] =
{
        -1,
};

int irq = DEC_IRQ;
int elements = 0;

/* module_param(name, type, perm) */
module_param(base_port, ulong, 0);
module_param(irq, int, 0);
module_param_array(multicorebase, uint, &elements, 0);

static int hantrodec_major = 0; /* dynamic allocation */

/* here's all the must remember stuff */
typedef struct
{
    char *buffer;
    unsigned int iosize;
    volatile u8 *hwregs[HXDEC_MAX_CORES];
    int irq;
    int cores;
    struct fasync_struct *async_queue_dec;
    struct fasync_struct *async_queue_pp;
} hantrodec_t;

static hantrodec_t hantrodec_data; /* dynamic allocation? */

static int ReserveIO(void);
static void ReleaseIO(void);

/* PCIe resources */
/* TODO(mheikkinen) Implement multicore support. */

struct pci_dev *gDev = NULL;    /* PCI device structure. */

u32 gBaseHdwr;                  /* PCI base register address (Hardware address) */
u32 gBaseLen;                   /* Base register address Length */
void *gBaseVirt = NULL;         /* Base register virtual address */
u32 gHantroRegBase = 0;         /* Base register for Hantro IP */
u32 *gHantroRegVirt = NULL;     /* Virtual register for Hantro IP */

static int PcieInit(void);
static void ResetAsic(hantrodec_t * dev);

#ifdef HANTRODEC_DEBUG
static void dump_regs(hantrodec_t *dev);
#endif

/* IRQ handler */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
static irqreturn_t hantrodec_isr(int irq, void *dev_id, struct pt_regs *regs);
#else
static irqreturn_t hantrodec_isr(int irq, void *dev_id);
#endif


static u32 dec_regs[HXDEC_MAX_CORES][DEC_IO_SIZE/4];
struct semaphore dec_core_sem;
struct semaphore pp_core_sem;

static int dec_irq = 0;
static int pp_irq = 0;

atomic_t irq_rx = ATOMIC_INIT(0);
atomic_t irq_tx = ATOMIC_INIT(0);

static struct file* dec_owner[HXDEC_MAX_CORES];
static struct file* pp_owner[HXDEC_MAX_CORES];

DEFINE_SPINLOCK(owner_lock);

DECLARE_WAIT_QUEUE_HEAD(dec_wait_queue);
DECLARE_WAIT_QUEUE_HEAD(pp_wait_queue);

DECLARE_WAIT_QUEUE_HEAD(hw_queue);

#define DWL_CLIENT_TYPE_PP               4U
#define DWL_CLIENT_TYPE_VP9_DEC          11U
#define DWL_CLIENT_TYPE_HEVC_DEC         12U

static u32 cfg[HXDEC_MAX_CORES];

static void ReadCoreConfig(hantrodec_t *dev)
{
    int c;
    u32 reg, tmp;

    memset(cfg, 0, sizeof(cfg));

    for(c = 0; c < dev->cores; c++)
    {
        /* Decoder configuration */
        reg = ioread32(dev->hwregs[c] + HANTRODEC_SYNTH_CFG_2 * 4);

        tmp = (reg >> DWL_HEVC_E) & 0x3U;
        if(tmp) printk(KERN_INFO "hantrodec: core[%d] has HEVC\n", c);
        cfg[c] |= tmp ? 1 << DWL_CLIENT_TYPE_HEVC_DEC : 0;

        tmp = (reg >> DWL_VP9_E) & 0x03U;
        if(tmp) printk(KERN_INFO "hantrodec: core[%d] has VP9\n", c);
        cfg[c] |= tmp ? 1 << DWL_CLIENT_TYPE_VP9_DEC : 0;

        /* Post-processor configuration */
        reg = ioread32(dev->hwregs[c] + HANTRODECPP_SYNTH_CFG * 4);

        tmp = (reg >> DWL_PP_E) & 0x01U;
        if(tmp) printk(KERN_INFO "hantrodec: core[%d] has PP\n", c);
        cfg[c] |= tmp ? 1 << DWL_CLIENT_TYPE_PP : 0;
    }
}

static int CoreHasFormat(const u32 *cfg, int core, u32 format)
{
    return (cfg[core] & (1 << format)) ? 1 : 0;
}

int GetDecCore(long core, hantrodec_t *dev, struct file* filp)
{
    int success = 0;
    unsigned long flags;

    PDEBUG("GetDecCore\n");
    spin_lock_irqsave(&owner_lock, flags);
    if(dec_owner[core] == NULL )
    {
        dec_owner[core] = filp;
        success = 1;
    }

    spin_unlock_irqrestore(&owner_lock, flags);
    PDEBUG("spin_lock_irqstore GetDecCore\n");

    return success;
}

int GetDecCoreAny(long *core, hantrodec_t *dev, struct file* filp,
        unsigned long format)
{
    int success = 0;
    long c;

    *core = -1;

    for(c = 0; c < dev->cores; c++)
    {
        /* a free core that has format */
        if(CoreHasFormat(cfg, c, format) && GetDecCore(c, dev, filp))
        {
            success = 1;
            *core = c;
            break;
        }
    }

    PDEBUG("GetCoreAny Success\n");
    return success;
}

long ReserveDecoder(hantrodec_t *dev, struct file* filp, unsigned long format)
{
    long core = -1;

    PDEBUG("Reserve core\n");
    /* reserve a core */
    if (down_interruptible(&dec_core_sem))
        return -ERESTARTSYS;

    /* lock a core that has specific format*/
    if(wait_event_interruptible(hw_queue,
            GetDecCoreAny(&core, dev, filp, format) != 0 ))
        return -ERESTARTSYS;

    PDEBUG("Reserve core, reserved\n");
    return core;
}

void ReleaseDecoder(hantrodec_t *dev, long core)
{
    u32 status;
    unsigned long flags;
    u32 counter = 0;


    status = ioread32(dev->hwregs[core] + HANTRODEC_IRQ_STAT_DEC_OFF);

    /* make sure HW is disabled */
    if(status & HANTRODEC_DEC_E)
    {
        printk(KERN_INFO "hantrodec: DEC[%li] still enabled -> reset\n", core);

        while(status & HANTRODEC_DEC_E)
        {
            if(!(counter & 0x7FF))
                PDEBUG("hantrodec: Killed, wait for HW finish\n", core);
            status = ioread32(dev->hwregs[core] + HANTRODEC_IRQ_STAT_DEC_OFF);
            if(++counter > 500000){

                printk(KERN_INFO "hantrodec: Killed, timeout\n", core);
                break;
            }
        }

        iowrite32(0, dev->hwregs[core] + HANTRODEC_IRQ_STAT_DEC_OFF);

    }

    spin_lock_irqsave(&owner_lock, flags);

    dec_owner[core] = NULL;

    spin_unlock_irqrestore(&owner_lock, flags);

    up(&dec_core_sem);

    wake_up_interruptible_all(&hw_queue);
}

long ReservePostProcessor(hantrodec_t *dev, struct file* filp)
{
    unsigned long flags;

    long core = 0;

    /* single core PP only */
    if (down_interruptible(&pp_core_sem))
        return -ERESTARTSYS;

    spin_lock_irqsave(&owner_lock, flags);

    pp_owner[core] = filp;

    spin_unlock_irqrestore(&owner_lock, flags);

    return core;
}

void ReleasePostProcessor(hantrodec_t *dev, long core)
{
    unsigned long flags;

    u32 status = ioread32(dev->hwregs[core] + HANTRODEC_IRQ_STAT_PP_OFF);

    /* make sure HW is disabled */
    if(status & HANTRODEC_PP_E)
    {
        printk(KERN_INFO "hantrodec: PP[%li] still enabled -> reset\n", core);

        /* disable IRQ */
        status |= HANTRODEC_PP_IRQ_DISABLE;

        /* disable postprocessor */
        status &= (~HANTRODEC_PP_E);
        iowrite32(0x10, dev->hwregs[core] + HANTRODEC_IRQ_STAT_PP_OFF);
    }

    spin_lock_irqsave(&owner_lock, flags);

    pp_owner[core] = NULL;

    spin_unlock_irqrestore(&owner_lock, flags);

    up(&pp_core_sem);
}

long ReserveDecPp(hantrodec_t *dev, struct file* filp, unsigned long format)
{
    /* reserve core 0, DEC+PP for pipeline */
    unsigned long flags;

    long core = 0;

    /* check that core has the requested dec format */
    if(!CoreHasFormat(cfg, core, format))
        return -EFAULT;

    /* check that core has PP */
    if(!CoreHasFormat(cfg, core, DWL_CLIENT_TYPE_PP))
        return -EFAULT;

    /* reserve a core */
    if (down_interruptible(&dec_core_sem))
        return -ERESTARTSYS;

    /* wait until the core is available */
    if(wait_event_interruptible(hw_queue,
            GetDecCore(core, dev, filp) != 0))
    {
        up(&dec_core_sem);
        return -ERESTARTSYS;
    }


    if (down_interruptible(&pp_core_sem))
    {
        ReleaseDecoder(dev, core);
        return -ERESTARTSYS;
    }

    spin_lock_irqsave(&owner_lock, flags);
    pp_owner[core] = filp;
    spin_unlock_irqrestore(&owner_lock, flags);

    return core;
}

long DecFlushRegs(hantrodec_t *dev, struct core_desc *core)
{
    long ret = 0, i;

    u32 id = core->id;

    ret = copy_from_user(dec_regs[id], core->regs, HANTRO_DEC_REGS*4);
    if (ret)
    {
        PDEBUG("copy_from_user failed, returned %li\n", ret);
        return -EFAULT;
    }

    /* write all regs but the status reg[1] to hardware */
    for(i = 2; i <= HANTRO_DEC_LAST_REG; i++)
        iowrite32(dec_regs[id][i], dev->hwregs[id] + i*4);

    /* write the status register, which may start the decoder */
    iowrite32(dec_regs[id][1], dev->hwregs[id] + 4);

    PDEBUG("flushed registers on core %d %x\n", id, dec_regs[id][1]);

    return 0;
}

long DecRefreshRegs(hantrodec_t *dev, struct core_desc *core)
{
    long ret, i;
    u32 id = core->id;

    /* user has to know exactly what they are asking for */
    if(core->size != (HANTRO_DEC_REGS * 4)) {
        PDEBUG("DecRefreshRegs failed, wrong size %d\n", core->size);
        return -EFAULT;
    }
    /* read all registers from hardware */
    for(i = 0; i <= HANTRO_DEC_LAST_REG; i++)
        dec_regs[id][i] = ioread32(dev->hwregs[id] + i*4);

    /* put registers to user space*/
    ret = copy_to_user(core->regs, dec_regs[id], HANTRO_DEC_REGS*4);
    if (ret)
    {
        PDEBUG("copy_to_user failed, returned %li\n", ret);
        return -EFAULT;
    }

    return 0;
}

static int CheckDecIrq(hantrodec_t *dev, int id)
{
    unsigned long flags;
    int rdy = 0;

    const u32 irq_mask = (1 << id);

    spin_lock_irqsave(&owner_lock, flags);

    if(dec_irq & irq_mask)
    {
        /* reset the wait condition(s) */
        dec_irq &= ~irq_mask;
        rdy = 1;
    }

    spin_unlock_irqrestore(&owner_lock, flags);

    return rdy;
}

long WaitDecReadyAndRefreshRegs(hantrodec_t *dev, struct core_desc *core)
{
    u32 id = core->id;

    PDEBUG("wait_event_interruptible DEC[%d]\n", id);

    if(wait_event_interruptible(dec_wait_queue, CheckDecIrq(dev, id)))
    {
        PDEBUG("DEC[%d]  wait_event_interruptible interrupted\n", id);
        return -ERESTARTSYS;
    }

    atomic_inc(&irq_tx);

    /* refresh registers */
    return DecRefreshRegs(dev, core);
}

long PPFlushRegs(hantrodec_t *dev, struct core_desc *core)
{
    long ret = 0;
    u32 id = core->id;
    u32 i;

    ret = copy_from_user(dec_regs[id] + HANTRO_DEC_REGS, core->regs,
            HANTRO_PP_REGS*4);
    if (ret)
    {
        PDEBUG("copy_from_user failed, returned %li\n", ret);
        return -EFAULT;
    }

    /* write all regs but the status reg[1] to hardware */
    for(i = HANTRO_PP_FIRST_REG + 1; i <= HANTRO_PP_LAST_REG; i++)
        iowrite32(dec_regs[id][i], dev->hwregs[id] + i*4);

    /* write the stat reg, which may start the PP */
    iowrite32(dec_regs[id][HANTRO_PP_FIRST_REG],
            dev->hwregs[id] + HANTRO_PP_FIRST_REG * 4);

    return 0;
}

long PPRefreshRegs(hantrodec_t *dev, struct core_desc *core)
{
    long i, ret;
    u32 id = core->id;

    /* user has to know exactly what they are asking for */
    if(core->size != (HANTRO_PP_REGS * 4))
        return -EFAULT;

    /* read all registers from hardware */
    for(i = HANTRO_PP_FIRST_REG; i <= HANTRO_PP_LAST_REG; i++)
        dec_regs[id][i] = ioread32(dev->hwregs[id] + i*4);

    /* put registers to user space*/
    ret = copy_to_user(core->regs, dec_regs[id] + HANTRO_PP_FIRST_REG,
            HANTRO_PP_REGS * 4);
    if (ret)
    {
        PDEBUG("copy_to_user failed, returned %li\n", ret);
        return -EFAULT;
    }

    return 0;
}

static int CheckPPIrq(hantrodec_t *dev, int id)
{
    unsigned long flags;
    int rdy = 0;

    const u32 irq_mask = (1 << id);

    spin_lock_irqsave(&owner_lock, flags);

    if(pp_irq & irq_mask)
    {
        /* reset the wait condition(s) */
        pp_irq &= ~irq_mask;
        rdy = 1;
    }

    spin_unlock_irqrestore(&owner_lock, flags);

    return rdy;
}

long WaitPPReadyAndRefreshRegs(hantrodec_t *dev, struct core_desc *core)
{
    u32 id = core->id;

    PDEBUG("wait_event_interruptible PP[%d]\n", id);

    if(wait_event_interruptible(pp_wait_queue, CheckPPIrq(dev, id)))
    {
        PDEBUG("PP[%d]  wait_event_interruptible interrupted\n", id);
        return -ERESTARTSYS;
    }

    atomic_inc(&irq_tx);

    /* refresh registers */
    return PPRefreshRegs(dev, core);
}

static int CheckCoreIrq(hantrodec_t *dev, const struct file *filp, int *id)
{
    unsigned long flags;
    int rdy = 0, n = 0;

    do
    {
        u32 irq_mask = (1 << n);

        spin_lock_irqsave(&owner_lock, flags);

        if(dec_irq & irq_mask)
        {
            if (dec_owner[n] == filp)
            {
                /* we have an IRQ for our client */

                /* reset the wait condition(s) */
                dec_irq &= ~irq_mask;

                /* signal ready core no. for our client */
                *id = n;

                rdy = 1;

     //           break;
            }
            else if(dec_owner[n] == NULL)
            {
                /* zombie IRQ */
                printk(KERN_INFO "IRQ on core[%d], but no owner!!!\n", n);

                /* reset the wait condition(s) */
                dec_irq &= ~irq_mask;
            }
        }
        else
		PDEBUG("IRQ, dec_irq & irq_mask == 0 %d\n",dec_irq );
          

        spin_unlock_irqrestore(&owner_lock, flags);
        PDEBUG("end spin_lock_irqsave CheckDecCoreIrq\n");

        n++; /* next core */
    }
    while(n < 1);

    PDEBUG("CheckCoreIrq DONE! return %d\n", rdy);

    return rdy;
}

long WaitCoreReady(hantrodec_t *dev, const struct file *filp, int *id)
{
    PDEBUG("wait_event_interruptible CORE\n");

    if(wait_event_interruptible(dec_wait_queue, CheckCoreIrq(dev, filp, id)))
    {
        PDEBUG("CORE  wait_event_interruptible interrupted\n");
        return -ERESTARTSYS;
    }

    atomic_inc(&irq_tx);

    PDEBUG("wait_event_interruptible CORE end\n");
    return 0;
}

/*------------------------------------------------------------------------------
 Function name   : hantrodec_ioctl
 Description     : communication method to/from the user space

 Return type     : long
------------------------------------------------------------------------------*/

static long hantrodec_ioctl(struct file *filp, unsigned int cmd,
        unsigned long arg)
{
    int err = 0;
    long tmp;

#ifdef HW_PERFORMANCE
    struct timeval *end_time_arg;
#endif

    PDEBUG("ioctl cmd 0x%08x\n", cmd);
    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if (_IOC_TYPE(cmd) != HANTRODEC_IOC_MAGIC)
        return -ENOTTY;
    if (_IOC_NR(cmd) > HANTRODEC_IOC_MAXNR)
        return -ENOTTY;

    /*
     * the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user-oriented, while
     * access_ok is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void *) arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err = !access_ok(VERIFY_READ, (void *) arg, _IOC_SIZE(cmd));

    if (err)
        return -EFAULT;

    switch (cmd)
    {
    case HANTRODEC_IOC_CLI:
        disable_irq(hantrodec_data.irq);
        break;
    case HANTRODEC_IOC_STI:
        enable_irq(hantrodec_data.irq);
        break;
    case HANTRODEC_IOCGHWOFFSET:
        __put_user(multicorebase[0], (unsigned long *) arg);
        break;
    case HANTRODEC_IOCGHWIOSIZE:
        __put_user(hantrodec_data.iosize, (unsigned int *) arg);
        break;
    case HANTRODEC_IOC_MC_OFFSETS:
    {
        tmp = copy_to_user((u32 *) arg, multicorebase, sizeof(multicorebase));
        if (err)
        {
            PDEBUG("copy_to_user failed, returned %li\n", tmp);
            return -EFAULT;
        }
        break;
    }
    case HANTRODEC_IOC_MC_CORES:
        __put_user(hantrodec_data.cores, (unsigned int *) arg);
        break;
    case HANTRODEC_IOCS_DEC_PUSH_REG:
    {
        struct core_desc core;

        /* get registers from user space*/
        tmp = copy_from_user(&core, (void*)arg, sizeof(struct core_desc));
        if (tmp)
        {
            PDEBUG("copy_from_user failed, returned %li\n", tmp);
            return -EFAULT;
        }

        DecFlushRegs(&hantrodec_data, &core);
        break;
    }
    case HANTRODEC_IOCS_PP_PUSH_REG:
    {
        struct core_desc core;

        /* get registers from user space*/
        tmp = copy_from_user(&core, (void*)arg, sizeof(struct core_desc));
        if (tmp)
        {
            PDEBUG("copy_from_user failed, returned %li\n", tmp);
            return -EFAULT;
        }

        PPFlushRegs(&hantrodec_data, &core);
        break;
    }
    case HANTRODEC_IOCS_DEC_PULL_REG:
    {
        struct core_desc core;

        PDEBUG("start HANTRODEC_IOCS_DEC_PULL_REG\n");
  
        /* get registers from user space*/
        tmp = copy_from_user(&core, (void*)arg, sizeof(struct core_desc));
        if (tmp)
        {
            PDEBUG("copy_from_user failed, returned %li\n", tmp);
            return -EFAULT;
        }

        return DecRefreshRegs(&hantrodec_data, &core);
    }
    case HANTRODEC_IOCS_PP_PULL_REG:
    {
        struct core_desc core;

        /* get registers from user space*/
        tmp = copy_from_user(&core, (void*)arg, sizeof(struct core_desc));
        if (tmp)
        {
            PDEBUG("copy_from_user failed, returned %li\n", tmp);
            return -EFAULT;
        }

        return PPRefreshRegs(&hantrodec_data, &core);
    }
    case HANTRODEC_IOCH_DEC_RESERVE:
    {
        PDEBUG("Reserve DEC core, format = %li\n", arg);
        return ReserveDecoder(&hantrodec_data, filp, arg);
    }
    case HANTRODEC_IOCT_DEC_RELEASE:
    {
        
        PDEBUG("Release DEC, core = %li\n", arg);
        if(arg >= hantrodec_data.cores || dec_owner[arg] != filp)
        {
            PDEBUG("bogus DEC release, core = %li\n", arg);
            return -EFAULT;
        }

        PDEBUG("Release DEC, core = %li\n", arg);

        ReleaseDecoder(&hantrodec_data, arg);

        break;
    }
    case HANTRODEC_IOCQ_PP_RESERVE:
        return ReservePostProcessor(&hantrodec_data, filp);
    case HANTRODEC_IOCT_PP_RELEASE:
    {
        if(arg != 0 || pp_owner[arg] != filp)
        {
            PDEBUG("bogus PP release %li\n", arg);
            return -EFAULT;
        }

        ReleasePostProcessor(&hantrodec_data, arg);

        break;
    }
    case HANTRODEC_IOCX_DEC_WAIT:
    {
        struct core_desc core;

        PDEBUG("HANTRODEC_IOCX_DEC_WAIT\n", tmp);
        /* get registers from user space */
        tmp = copy_from_user(&core, (void*)arg, sizeof(struct core_desc));
        if (tmp)
        {
            PDEBUG("copy_from_user failed, returned %li\n", tmp);
            return -EFAULT;
        }

        return WaitDecReadyAndRefreshRegs(&hantrodec_data, &core);
    }
    case HANTRODEC_IOCX_PP_WAIT:
    {
        struct core_desc core;

        /* get registers from user space */
        tmp = copy_from_user(&core, (void*)arg, sizeof(struct core_desc));
        if (tmp)
        {
            PDEBUG("copy_from_user failed, returned %li\n", tmp);
            return -EFAULT;
        }

        return WaitPPReadyAndRefreshRegs(&hantrodec_data, &core);
    }
    case HANTRODEC_IOCG_CORE_WAIT:
    {
        int id;
        PDEBUG("HANTRODEC_IOCG_CORE_WAIT\n");
        tmp = WaitCoreReady(&hantrodec_data, filp, &id);
        __put_user(id, (int *) arg);
        PDEBUG("HANTRODEC_IOCG_CORE_WAIT ready\n", tmp);
        return tmp;
    }
    case HANTRODEC_IOX_ASIC_ID:
    {
        u32 id;
        __get_user(id, (u32*)arg);

        if(id >= hantrodec_data.cores)
        {
            return -EFAULT;
        }
        id = ioread32(hantrodec_data.hwregs[id]);
        __put_user(id, (u32 *) arg);
    }
    case HANTRODEC_DEBUG_STATUS:
    {
        printk(KERN_INFO "hantrodec: dec_irq     = 0x%08x \n", dec_irq);
        printk(KERN_INFO "hantrodec: pp_irq      = 0x%08x \n", pp_irq);

        printk(KERN_INFO "hantrodec: IRQs received/sent2user = %d / %d \n",
                atomic_read(&irq_rx), atomic_read(&irq_tx));

        for (tmp = 0; tmp < hantrodec_data.cores; tmp++)
        {
            printk(KERN_INFO "hantrodec: dec_core[%li] %s\n",
                    tmp, dec_owner[tmp] == NULL ? "FREE" : "RESERVED");
            printk(KERN_INFO "hantrodec: pp_core[%li]  %s\n",
                    tmp, pp_owner[tmp] == NULL ? "FREE" : "RESERVED");
        }
    }
    default:
        return -ENOTTY;
    }

    return 0;
}

/*------------------------------------------------------------------------------
 Function name   : hantrodec_open
 Description     : open method

 Return type     : int
------------------------------------------------------------------------------*/

static int hantrodec_open(struct inode *inode, struct file *filp)
{
    PDEBUG("dev opened\n");
    return 0;
}

/*------------------------------------------------------------------------------
 Function name   : hantrodec_release
 Description     : Release driver

 Return type     : int
------------------------------------------------------------------------------*/

static int hantrodec_release(struct inode *inode, struct file *filp)
{
    int n;
    hantrodec_t *dev = &hantrodec_data;

    PDEBUG("closing ...\n");

    for(n = 0; n < dev->cores; n++)
    {
        if(dec_owner[n] == filp)
        {
            PDEBUG("releasing dec core %i lock\n", n);
            ReleaseDecoder(dev, n);
        }
    }

    for(n = 0; n < 1; n++)
    {
        if(pp_owner[n] == filp)
        {
            PDEBUG("releasing pp core %i lock\n", n);
            ReleasePostProcessor(dev, n);
        }
    }

    PDEBUG("closed\n");
    return 0;
}

/* VFS methods */
static struct file_operations hantrodec_fops =
{
        .owner = THIS_MODULE,
        .open = hantrodec_open,
        .release = hantrodec_release,
        .unlocked_ioctl = hantrodec_ioctl,
        .fasync = NULL
};

/*------------------------------------------------------------------------------
 Function name   : hantrodec_init
 Description     : Initialize the driver

 Return type     : int
------------------------------------------------------------------------------*/

int __init hantrodec_init(void)
{
    int result, i;

    PDEBUG("module init, debug prints enabled\n");

    printk(KERN_INFO "hantrodec: dec/pp kernel module. \n");

    result = PcieInit();
    if(result)
        goto err;
    multicorebase[0] = gHantroRegBase;
    elements = 1;
    if (irq > 0)
      irq = gDev->irq;
    printk(KERN_INFO "hantrodec: Init single core at 0x%08x IRQ=%i\n",
            multicorebase[0], irq);

    hantrodec_data.iosize = DEC_IO_SIZE;
    hantrodec_data.irq = irq;

    for(i=0; i< HXDEC_MAX_CORES; i++)
    {
        hantrodec_data.hwregs[i] = 0;
        /* If user gave less core bases that we have by default,
         * invalidate default bases
         */
        if(elements && i>=elements)
        {
            multicorebase[i] = -1;
        }
    }

    hantrodec_data.async_queue_dec = NULL;
    hantrodec_data.async_queue_pp = NULL;

    result = register_chrdev(hantrodec_major, "hantrodec", &hantrodec_fops);
    if(result < 0)
    {
        printk(KERN_INFO "hantrodec: unable to get major %d\n", hantrodec_major);
        goto err;
    }
    else if(result != 0)    /* this is for dynamic major */
    {
        hantrodec_major = result;
    }

    result = ReserveIO();
    if(result < 0)
    {
        goto err;
    }

    memset(dec_owner, 0, sizeof(dec_owner));
    memset(pp_owner, 0, sizeof(pp_owner));

    sema_init(&dec_core_sem, hantrodec_data.cores);
    sema_init(&pp_core_sem, 1);

    /* read configuration fo all cores */
    ReadCoreConfig(&hantrodec_data);

    /* reset hardware */
    ResetAsic(&hantrodec_data);

    /* get the IRQ line */
    if(irq > 0)
    {
        result = request_irq(irq, hantrodec_isr,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18))
                SA_INTERRUPT | SA_SHIRQ,
#else
                IRQF_SHARED,
#endif
                "hantrodec", (void *) &hantrodec_data);
        if(result != 0)
        {
            if(result == -EINVAL)
            {
                printk(KERN_ERR "hantrodec: Bad irq number or handler\n");
            }
            else if(result == -EBUSY)
            {
                printk(KERN_ERR "hantrodec: IRQ <%d> busy, change your config\n",
                        hantrodec_data.irq);
            }

            ReleaseIO();
            goto err;
        }
    }
    else
    {
        printk(KERN_INFO "hantrodec: IRQ not in use!\n");
    }

    printk(KERN_INFO "hantrodec: module inserted. Major = %d\n", hantrodec_major);

    return 0;

    err:
    printk(KERN_INFO "hantrodec: module not inserted\n");
    unregister_chrdev(hantrodec_major, "hantrodec");
    return result;
}

/*------------------------------------------------------------------------------
 Function name   : hantrodec_cleanup
 Description     : clean up

 Return type     : int
------------------------------------------------------------------------------*/

void __exit hantrodec_cleanup(void)
{
    hantrodec_t *dev = &hantrodec_data;

    /* reset hardware */
    ResetAsic(dev);

    /* free the IRQ */
    if(dev->irq != -1)
    {
        free_irq(dev->irq, (void *) dev);
    }

    printk(KERN_INFO "hantrodec: Release IO\n");
    ReleaseIO();

    unregister_chrdev(hantrodec_major, "hantrodec");

    printk(KERN_INFO "hantrodec: module removed\n");
    return;
}

/*------------------------------------------------------------------------------
 Function name   : PcieInit
 Description     : Initialize PCI Hw access

 Return type     : int
 ------------------------------------------------------------------------------*/

static int PcieInit(void)
{
    gDev = pci_get_device(PCI_VENDOR_ID_HANTRO, PCI_DEVICE_ID_HANTRO_PCI, gDev);
    if (NULL == gDev) {
        printk("Init: Hardware not found.\n");
        //return (CRIT_ERR);
        return (-1);
    }

    if (0 > pci_enable_device(gDev)) {
        printk("Init: Device not enabled.\n");
        return (-1);
    }

    // Get Base Address of registers from pci structure. Should come from pci_dev
    // structure, but that element seems to be missing on the development system.
    gBaseHdwr = pci_resource_start (gDev, PCI_CONTROL_BAR);
    if (0 > gBaseHdwr) {
        printk(KERN_INFO "Init: Base Address not set.\n");
        return (-1);
    }
    printk(KERN_INFO "Base hw val %X\n", (unsigned int)gBaseHdwr);

    gBaseLen = pci_resource_len (gDev, PCI_CONTROL_BAR);
    printk(KERN_INFO "Base hw len %d\n", (unsigned int)gBaseLen);

    // Remap the I/O register block so that it can be safely accessed.

    gBaseVirt = ioremap(gBaseHdwr, gBaseLen);
    if (!gBaseVirt) {
        printk(KERN_INFO "Init: Could not remap memory.\n");
        //return (CRIT_ERR);
        return (-1);
    }

    // Try to gain exclusive control of memory for demo hardware.
    if (0 > check_mem_region(gBaseHdwr, gBaseLen)) {
        printk(KERN_INFO ": Init: Memory in use.\n");
        //return (CRIT_ERR);
        return (-1);
    }

    if (!request_mem_region(gBaseHdwr,gBaseLen, "HantroDrv")) {
                printk(KERN_INFO "hantrodec: failed to reserve HW regs\n");
                return -1;
    }

    gHantroRegBase = gBaseHdwr + HANTRO_REG_OFFSET;
    gHantroRegVirt = (unsigned int *)gBaseVirt + HANTRO_REG_OFFSET/4;
    printk(KERN_INFO "hantrodec: Hantro register bus %X virtual %X\n",
            (unsigned int)gHantroRegBase,gHantroRegVirt );

    ((unsigned int*)gBaseVirt)[HLINA_ADDR_TRANSL_REG] = HLINA_TRANSL_BASE;
    printk("hantrodec: Address translation base for %x\n",
            (((unsigned int*)gBaseVirt)[HLINA_ADDR_TRANSL_REG]));
    return 0;
}

/*------------------------------------------------------------------------------
 Function name   : CheckHwId
 Return type     : int
------------------------------------------------------------------------------*/
static int CheckHwId(hantrodec_t * dev)
{
    long int hwid;
    int i;
    size_t numHw = sizeof(DecHwId) / sizeof(*DecHwId);

    int found = 0;

    for (i = 0; i < dev->cores; i++)
    {
        if (dev->hwregs[i] != NULL )
        {
            hwid = readl(dev->hwregs[i]);
            printk(KERN_INFO "hantrodec: Core %d HW ID=0x%08lx\n", i, hwid);
            hwid = (hwid >> 16) & 0xFFFF; /* product version only */

            while (numHw--)
            {
                if (hwid == DecHwId[numHw])
                {
                    printk(KERN_INFO "hantrodec: Supported HW found at 0x%08x\n",
                            multicorebase[i]);
                    found++;
                    break;
                }
            }
            if (!found)
            {
                printk(KERN_INFO "hantrodec: Unknown HW found at 0x%08x\n",
                        multicorebase[i]);
                return 0;
            }
            found = 0;
            numHw = sizeof(DecHwId) / sizeof(*DecHwId);
        }
    }

    return 1;
}

static int ReserveIO(void)
{

        hantrodec_data.hwregs[0] = (volatile u8 *) gHantroRegVirt;

        if (hantrodec_data.hwregs[0] == NULL )
        {
            printk(KERN_INFO "hantrodec: failed to ioremap HW regs\n");
            ReleaseIO();
            return -EBUSY;
        }

        hantrodec_data.cores = 1;
        /* check for correct HW */
        if (!CheckHwId(&hantrodec_data))
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

    if (gBaseVirt != NULL)
        iounmap((void *) gBaseVirt);
    release_mem_region( gBaseHdwr ,gBaseLen);

}

/*------------------------------------------------------------------------------
 Function name   : hantrodec_isr
 Description     : interrupt handler

 Return type     : irqreturn_t
------------------------------------------------------------------------------*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
irqreturn_t hantrodec_isr(int irq, void *dev_id, struct pt_regs *regs)
#else
irqreturn_t hantrodec_isr(int irq, void *dev_id)
#endif
{
    unsigned long flags;
    unsigned int handled = 0;
    int i;
    volatile u8 *hwregs;

    hantrodec_t *dev = (hantrodec_t *) dev_id;
    u32 irq_status_dec;
    u32 irq_status_pp;

    spin_lock_irqsave(&owner_lock, flags);

    for(i=0; i<dev->cores; i++)
    {
        volatile u8 *hwregs = dev->hwregs[i];

        /* interrupt status register read */
        irq_status_dec = ioread32(hwregs + HANTRODEC_IRQ_STAT_DEC_OFF);

        if(irq_status_dec & HANTRODEC_DEC_IRQ)
        {
            /* clear dec IRQ */
            irq_status_dec &= (~HANTRODEC_DEC_IRQ);
            iowrite32(irq_status_dec, hwregs + HANTRODEC_IRQ_STAT_DEC_OFF);

            PDEBUG("ISR: decoder IRQ received! core %d\n", i);

            atomic_inc(&irq_rx);

            dec_irq |= (1 << i);

            wake_up_interruptible_all(&dec_wait_queue);
            handled++;
        }
		else
		{
			PDEBUG("ISR: NO IRQ %x\n", irq_status_dec);
		}
    }

    spin_unlock_irqrestore(&owner_lock, flags);
    PDEBUG("ISR end\n");

    if(!handled)
    {
        PDEBUG("IRQ received, but not hantrodec's!\n");
    }

    return IRQ_RETVAL(handled);
}

/*------------------------------------------------------------------------------
 Function name   : ResetAsic
 Description     : reset asic

 Return type     :
------------------------------------------------------------------------------*/
void ResetAsic(hantrodec_t * dev)
{
    int i, j;
    u32 status;

    for (j = 0; j < dev->cores; j++)
    {
        status = ioread32(dev->hwregs[j] + HANTRODEC_IRQ_STAT_DEC_OFF);

        if( status & HANTRODEC_DEC_E)
        {
        /* abort with IRQ disabled */
            status = HANTRODEC_DEC_ABORT | HANTRODEC_DEC_IRQ_DISABLE;
            iowrite32(status, dev->hwregs[j] + HANTRODEC_IRQ_STAT_DEC_OFF);
        }

        /* reset PP */
        iowrite32(0, dev->hwregs[j] + HANTRODEC_IRQ_STAT_PP_OFF);

        for (i = 4; i < dev->iosize; i += 4)
        {
            iowrite32(0, dev->hwregs[j] + i);
        }
    }
}

/*------------------------------------------------------------------------------
 Function name   : dump_regs
 Description     : Dump registers

 Return type     :
------------------------------------------------------------------------------*/
#ifdef HANTRODEC_DEBUG
void dump_regs(hantrodec_t *dev)
{
    int i,c;

    PDEBUG("Reg Dump Start\n");
    for(c = 0; c < dev->cores; c++)
    {
        for(i = 0; i < dev->iosize; i += 4*4)
        {
            PDEBUG("\toffset %04X: %08X  %08X  %08X  %08X\n", i,
                    ioread32(dev->hwregs[c] + i),
                    ioread32(dev->hwregs[c] + i + 4),
                    ioread32(dev->hwregs[c] + i + 16),
                    ioread32(dev->hwregs[c] + i + 24));
        }
    }
    PDEBUG("Reg Dump End\n");
}
#endif


module_init( hantrodec_init);
module_exit( hantrodec_cleanup);

/* module description */
MODULE_LICENSE("Proprietary");
MODULE_AUTHOR("Google Finland Oy");
MODULE_DESCRIPTION("Driver module for Hantro Decoder/Post-Processor");

