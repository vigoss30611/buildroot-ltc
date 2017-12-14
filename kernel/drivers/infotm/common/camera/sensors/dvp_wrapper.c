#include <mach/imap-iomap.h>
#include <linux/types.h>
#include <linux/sizes.h>
#include <linux/kernel.h>
#include <asm/io.h>

typedef struct dvp_wrapper_config_t {
	void __iomem *base;
	__u8 debugon;
	__u8 invclk;
	__u8 invfield;
	__u8 invvsync;
	__u8 invhsync;
	u32 hmode;
	u32 vmode;
	u32 hnum;
	u32 vnum;
	__u8  syncmask;
	__u8  syncode0;
	__u8  syncode1;
	__u8  syncode2;
	__u8  syncode3;
	u32   hdlycnt;
	u32   vdlycnt;
} DVPWrapper;

static DVPWrapper dvp_wrapper_config = {
	.base = 0,
	.debugon = 1,
	.invclk = 0,
	.invfield = 0,
	.invvsync = 0,
	.invhsync = 0,
	.hmode = 0,
	.vmode = 0,
	.hnum = 0,
	.vnum = 0,
	.syncmask = 0,
	.syncode0 = 0,
	.syncode1 = 0,
	.syncode2 = 0,
	.syncode3 = 0,
	.hdlycnt = 0,
	.vdlycnt = 0,
};

void __isp_dvp_wrapper_init(DVPWrapper* config,
					unsigned int hmode, unsigned int width, unsigned int height,
					unsigned int hdly, unsigned int vdly)
{
	if (0x02 == hmode) { //system delay mode
		config->vmode = 0x01;
		config->hmode = hmode;

		config->hdlycnt = hdly;//114;//480;//480
		config->vdlycnt = vdly;//12;//400;//400

		config->hnum = width;
		config->vnum = height;
		/* wrapper bypass */
		if(((config->hmode != 0x2) || (config->hmode != 0x3))
		                && (config->vmode != 0x1)) {
			printk("isp DVP wrapper bypass!!\n");
			return ;
		}
		printk("Isp DVP wrapper begin to configuration!\n");

		writel((config->debugon << 3)  |
				(config->invclk << 2) |
				(config->invvsync) << 1 |
				config->invhsync , config->base + 0);

		writel((config->hmode << 4) |
				config->vmode, config->base + 0x4);

		writel(((config->hnum -1) << 16) |
				(config->vnum -1), config->base + 0x8);

		writel(config->syncmask, config->base + 0xc);
		writel(config->syncode0, config->base + 0x10);
		writel(config->syncode1, config->base + 0x14);
		writel(config->syncode2, config->base + 0x18);
		writel(config->syncode3, config->base+ 0x1c);

		writel(((config->hdlycnt) << 16) |
				(config->vdlycnt), config->base + 0x20);
	} else if (0x03 == hmode) { //sync code mode

	}
}
//for camif dvp wrapper
void __camif_dvp_wrapper_init(DVPWrapper* config,
                    unsigned int hmode, unsigned int width, unsigned int height,
                    unsigned int hdly, unsigned int vdly)
{
    if (0x02 == hmode) { //system delay mode
        config->vmode = 0x01;
        config->hmode = hmode;

        config->hdlycnt = hdly;//114;//480;//480
        config->vdlycnt = vdly;//12;//400;//400

        config->hnum = width;
        config->vnum = height;
        /* wrapper bypass */
        if(((config->hmode != 0x2) || (config->hmode != 0x3))
                && (config->vmode != 0x1)) {
            printk("isp DVP wrapper bypass!!\n");
            return ;
        }
        printk("Isp DVP wrapper begin to configuration!\n");

        writel((config->debugon << 4)  |
                (config->invfield << 3) |
                (config->invclk << 2) |
                (config->invvsync << 1) |
                config->invhsync , config->base + 0);

        writel((config->hmode << 4) |
                config->vmode, config->base + 0x4);

        writel(((config->hnum -1) << 16) |
                (config->vnum -1), config->base + 0x8);

        writel(config->syncmask, config->base + 0xc);
        writel(config->syncode0, config->base + 0x10);
        writel(config->syncode1, config->base + 0x14);
        writel(config->syncode2, config->base + 0x18);
        writel(config->syncode3, config->base+ 0x1c);

        writel(((config->hdlycnt) << 16) |
                (config->vdlycnt), config->base + 0x20);
    } else if (0x03 == hmode) { //sync code mode

    }
}


void isp_dvp_wrapper_probe(void)
{
	dvp_wrapper_config.base =
		ioremap_nocache(IMAP_ISP_WRAPPER_BASE, IMAP_WRAP_SIZE);
	printk("dvp wrapper0  base = 0x%x\n", (unsigned int)dvp_wrapper_config.base);
}
EXPORT_SYMBOL(isp_dvp_wrapper_probe);

void camif_dvp_wrapper_probe(void)
{
    dvp_wrapper_config.base =
        ioremap_nocache(IMAP_CAMIF_WRAPPER_BASE, IMAP_WRAP_SIZE);
    printk("dvp wrapper0  base = 0x%x\n", (unsigned int)dvp_wrapper_config.base);
}
EXPORT_SYMBOL(camif_dvp_wrapper_probe);


void isp_dvp_wrapper_init(unsigned int hmode,
					unsigned int width, unsigned int height,
					unsigned int hdly, unsigned int vdly)
{
	__isp_dvp_wrapper_init(&dvp_wrapper_config, hmode, width, height, hdly, vdly);
}
EXPORT_SYMBOL(isp_dvp_wrapper_init);

void camif_dvp_wrapper_init(unsigned int hmode,
                    unsigned int width, unsigned int height,
                    unsigned int hdly, unsigned int vdly)
{
    __camif_dvp_wrapper_init(&dvp_wrapper_config, hmode, width, height, hdly, vdly);
}
EXPORT_SYMBOL(camif_dvp_wrapper_init);

void dvp_wrapper_debug(void)
{
    printk("dvp wrapper0 hcnt in one frame: 0x%x\n", readl(dvp_wrapper_config.base + 0x24));
    printk("dvp wrapper0 hcnt = 0x%x, vcnt = 0x%x\n", (readl(dvp_wrapper_config.base + 0x28) >> 16)&0xffff, readl(dvp_wrapper_config.base + 0x28)&0xffff);
}
EXPORT_SYMBOL(dvp_wrapper_debug);

#ifdef CONFIG_ARCH_APOLLO3
static DVPWrapper dvp_wrapper1_config = {
	.base = 0,
	.debugon = 1,
	.invclk = 0,
	.invvsync = 0,
	.invhsync = 0,
	.hmode = 0,
	.vmode = 0,
	.hnum = 0,
	.vnum = 0,
	.syncmask = 0,
	.syncode0 = 0,
	.syncode1 = 0,
	.syncode2 = 0,
	.syncode3 = 0,
	.hdlycnt = 0,
	.vdlycnt = 0,
};

void isp_dvp_wrapper1_probe(void)
{
	dvp_wrapper1_config.base =
		ioremap_nocache(IMAP_ISP_WRAPPER1_BASE, IMAP_WRAP_SIZE);
	printk("dvp wrapper1 base = 0x%x\n", dvp_wrapper1_config.base);
}
EXPORT_SYMBOL(isp_dvp_wrapper1_probe);

void isp_dvp_wrapper1_init(unsigned int hmode,
					unsigned int width, unsigned int height,
					unsigned int hdly, unsigned int vdly)
{
	__isp_dvp_wrapper_init(&dvp_wrapper1_config, hmode, width, height, hdly, vdly);
}
EXPORT_SYMBOL(isp_dvp_wrapper1_init);
#endif
