#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/cpumask.h>

#include <linux/spinlock.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
#include <mach/pad.h>
#include <linux/smp.h>
#include "emif.h"
#include "emif-reg.h"

//#define EMIF_DEBUG 1
#ifdef EMIF_DEBUG
#define emif_dbg(x...) printk(x)
#else
#define emif_dbg(x...)
#endif

static struct imapx_emif *i_emif;

static void emif_set_port_ll(uint32_t n, struct imapx_emif *emif)
{
    uint32_t tmp;
 
#if 0
    if(!emif->flag[n]){
        emif->flag[n] = 1;
#endif
        tmp = emif_port_get(emif->io_base, n);
        emif->port[n] = tmp;
        tmp &= ~0x3;
        tmp |= PORT_LL;
        emif_port_set(emif->io_base, n, tmp);
#if 0
    }   
    emif->count[n]++;
#endif
}

static void emif_clear_port_ll(uint32_t n, struct imapx_emif *emif)
{

#if 0
    if(emif->flag[n]){
        emif->count[n]--;
	if(!emif->count[n]){
#endif
        emif_port_set(emif->io_base, n, emif->port[n]);
#if 0
	    emif->flag[n] = 0;
	}
    }
#endif
}

/*
 * mode : IDS1_ENTRY  HDMI_EXIT
 */
uint32_t emif_control(uint32_t mode)
{
    struct imapx_emif *emif = i_emif;
    
    spin_lock(&emif->lock);

    switch(mode){
    case IDS1_ENTRY:
            emif_set_port_ll(PORT2, emif);
	    break;
    case IDS1_EXIT:
	    emif_clear_port_ll(PORT2, emif);
	    break;
    case IDS0_ENTRY:
            emif_set_port_ll(PORT1, emif);
	    break;
    case IDS0_EXIT:
	    emif_clear_port_ll(PORT1, emif);
	    break;
    default:
            spin_unlock(&emif->lock);
	    return 1;
    }

    spin_unlock(&emif->lock);

    return 0;
}

static int imapx_emif_probe(struct platform_device *pdev)
{
    int err = 0;
    uint32_t i;
    struct imapx_emif *emif;
    struct resource *res;
    struct device *dev = &pdev->dev;

    printk("imapx800 emif init.   \n"); 
 
    emif = kzalloc(sizeof(struct imapx_emif), GFP_KERNEL);
    if(emif == NULL){
	    dev_err(dev, "unable to alloc data struct \n");
	    err = -ENOMEM;
	    goto err_alloc;
    }
    platform_set_drvdata(pdev, emif);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if(!res){
	    dev_err(dev, "no MEM resource \n");
	    err = -ENODEV;
	    goto error;
    }

    emif->rsrc_start = res->start;
    emif->rsrc_len = resource_size(res);
    if(!request_mem_region(emif->rsrc_start, emif->rsrc_len, pdev->name)){
	    dev_err(dev, "requeset mem region err \n");
	    err = -EBUSY;
	    goto error;
    }

    emif->io_base = ioremap(emif->rsrc_start, emif->rsrc_len);
    if(!emif->io_base){
	    dev_err(dev, "ioremap error \n");
	    err = -ENXIO;
	    goto err_iomap;
    }

    emif->clk = clk_get_sys("imap-ddr-phy", "imap-ddr-phy");
    if(IS_ERR(emif->clk)){
            dev_err(dev, "clk error \n");
	    err = -ENODEV;
	    goto err_clk;
    }

    
    i_emif = emif;

    for(i = 0; i < 8; i++){
        emif->port[i] = 0x00100130;
#if 0
	emif->flag[i] = 0;
	emif->count[i] = 0;
#endif
    }

    spin_lock_init(&emif->lock);
    
    printk("imapx800 emif init done.    \n");

    return 0;
err_clk:

err_iomap:
    iounmap(emif->io_base);
error:
    platform_set_drvdata(pdev, NULL);
err_alloc:
    kfree(emif);

    return err;
}

static int imapx_emif_remove(struct platform_device *pdev)
{
    struct imapx_emif *emif = platform_get_drvdata(pdev);
    
    iounmap(emif->io_base);
    kfree(emif);
    platform_set_drvdata(pdev, NULL);

    return 0;
}

static struct platform_driver imapx_emif_driver = {
    .probe  = imapx_emif_probe,
    .remove = imapx_emif_remove,
    .driver = {
	    .name  = "imap-emif",
	    .owner = THIS_MODULE,
    },
};

static int __init imapx_emif_module_init(void)
{
    return platform_driver_register(&imapx_emif_driver);
}

static void __exit imapx_emif_module_exit(void)
{
    platform_driver_unregister(&imapx_emif_driver);
}

arch_initcall(imapx_emif_module_init);
module_exit(imapx_emif_module_exit);

MODULE_AUTHOR("Larry Liu");
MODULE_DESCRIPTION("imapx800 emif");
MODULE_LICENSE("GPL");

