/***************************************************************************** 
 * ** drivers/video/infotm/dss/imapx800_g2d.c
 * ** 
 * ** Copyright (c) 2012~2015 ShangHai Infotm Ltd all rights reserved. 
 * ** 
 * ** Use of Infotm's code is governed by terms and conditions 
 * ** stated in the accompanying licensing statement. 
 * ** 
 * ** Description: Implementation file of InfoTM imapx800 G2D driver for MediaBox.
 * **
 * ** Author:
 * **     William Smith (Qu Feng) <terminalnt@gmail.com>
 * **      
 * ** Revision History: 
 * ** ----------------- 
 * ** 1.0	2012/8/8	William Smith	A5-IDS-Structure
 * **
 * *****************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/dma-buf.h>    

#include <dss_common.h>

#include "imapx800_g2d.h"
//#include "imapx800_ids.h"

#include <mach/io.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
#include <mach/items.h>

//#define G2D_TIME
#ifdef G2D_TIME
#define G2D_TIME_PERIOD	120
#endif

//#define G2D_DEBUG
//#define G2D_SWAP

#ifdef G2D_DEBUG
#define g2d_dbg(format, arg...)	\
	printk(KERN_WARNING "[G2D]<DEBUG>: %s(): " format, __func__, ## arg) 
#else
#define g2d_dbg(format, arg...)	\
	({if (0) printk(KERN_WARNING "[G2D]<DEBUG>: %s(): "format, __func__, ## arg)  ; 0; })
#endif

#ifdef G2D_SWAP
#define g2d_swap(format, arg...)	\
	printk(KERN_WARNING "[G2D]<SWAP>: %s(): " format, __func__, ## arg) 
#else
#define g2d_swap(format, arg...)	\
	({if (0) printk(KERN_WARNING "[G2D]<SWAP>: %s(): "format, __func__, ## arg)  ; 0; })
#endif

#define g2d_err(format, arg...)	\
	printk(KERN_ERR "[G2D]<ERROR>: %s(): " format, __func__, ## arg) 
#define g2d_info(format, arg...)	\
	printk(KERN_ERR "[G2D]<INFO>: %s(): " format, __func__, ## arg) 

//extern unsigned int VICs_1080p[5];

extern int use_main_fb_resize;
extern unsigned int main_VIC;
unsigned int main_scaler_width = 100;
unsigned int main_scaler_height = 100;
extern unsigned int main_fb_phyaddr;
extern unsigned int main_fb_size;
extern void * fb_pmmHandle;
//extern int cur_fbx;
int rgb_720P = 0;
int g2d_complete = 0;

//extern struct ids_module m_G2D;

struct g2d * g2d_data_global;
EXPORT_SYMBOL(g2d_data_global);
extern struct class *fb_class;


static int g2d_suspend(struct device *dev);
static int g2d_resume(struct device *dev);
static int g2d_config(void *data, int idsx, int VIC, int factor_width, int factor_height);

int pmmdrv_open(void *phandle, char *owner);
//int pmmdrv_dmmu_init(void * handle, unsigned int devid);
//int pmmdrv_dmmu_deinit(void * handle);
int pmmdrv_dmmu_disable(void * handle);
//int pmmdrv_mm_alloc(void * handle, int size, int flag, alc_buffer_t *alcBuffer);

int g2dpwl_write_reg(struct g2d *g2d_data, unsigned int addr, unsigned int val)
{
	writel(val, (unsigned int)g2d_data->regbase_virtual + addr);
	return 0;
}

int g2dpwl_read_reg(struct g2d *g2d_data, unsigned int addr, unsigned int *val)
{
	*val = readl((unsigned int)g2d_data->regbase_virtual + addr);
	return 0;
}


void SetG2DRegister(unsigned int * regBase, unsigned int id, int value)
{

   unsigned int tmp;

    tmp = regBase[g2dRegSpec[id][0]];
    tmp &= ~(regMask[g2dRegSpec[id][1]] << g2dRegSpec[id][2]);
    tmp |= (value & regMask[g2dRegSpec[id][1]]) << g2dRegSpec[id][2];
    regBase[g2dRegSpec[id][0]] = tmp;

}


//==================================================
//==================================================
//==================================================
//==================================================
//==================================================
//==================================================
//==================================================


static ssize_t sharpen_en_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int count;
	count = sprintf(buf, "%d", g2d_data->sharpen_en);
	return count;
}

static ssize_t sharpen_en_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int idsx = g2d_data->idsx;
	int VIC = g2d_data->VIC;
	int factor_width  = g2d_data->factor_width;
	int factor_height = g2d_data->factor_height;
	int sharpen_en;
	
	sscanf(buf, "%d", &sharpen_en);
	g2d_data->sharpen_en = sharpen_en;
	g2d_dbg("sharpen_en = %d\n", sharpen_en);
	g2d_config(g2d_data, idsx, VIC, factor_width, factor_height);
	
	return count;
}

static ssize_t sharpen_denos_en_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int count;
	count = sprintf(buf, "%d", g2d_data->denos_en);
	return count;
}

static ssize_t sharpen_denos_en_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int idsx = g2d_data->idsx;
	int VIC = g2d_data->VIC;
	int factor_width  = g2d_data->factor_width;
	int factor_height = g2d_data->factor_height;
	int denos_en;
	
	sscanf(buf, "%d", &denos_en);
	g2d_data->denos_en = denos_en;
	g2d_dbg("denos_en = %d\n", denos_en);
	g2d_config(g2d_data, idsx, VIC, factor_width, factor_height);
	
	return count;
}

static ssize_t sharpen_coefw_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int count;
	count = sprintf(buf, "%d", g2d_data->coefw);
	return count;
}

static ssize_t sharpen_coefw_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int idsx = g2d_data->idsx;
	int VIC = g2d_data->VIC;
	int factor_width  = g2d_data->factor_width;
	int factor_height = g2d_data->factor_height;
	int coefw;

	sscanf(buf, "%d", &coefw);
	g2d_data->coefw = coefw;
	g2d_info("coefw = %d\n", coefw);
	g2d_config(g2d_data, idsx, VIC, factor_width, factor_height);
	
	return count;
}

static ssize_t sharpen_coefa_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int count;
	count = sprintf(buf, "%d", g2d_data->coefa);
	return count;
}

static ssize_t sharpen_coefa_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int idsx = g2d_data->idsx;
	int VIC = g2d_data->VIC;
	int factor_width  = g2d_data->factor_width;
	int factor_height = g2d_data->factor_height;
	int coefa;
	
	sscanf(buf, "%d", &coefa);
	g2d_data->coefa = coefa;
	g2d_dbg("coefa= %d\n", coefa);
	g2d_config(g2d_data, idsx, VIC, factor_width, factor_height);
	
	return count;
}

static ssize_t sharpen_errth_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int count;
	count = sprintf(buf, "%d", g2d_data->errTh);
	return count;
}

static ssize_t sharpen_errth_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int idsx = g2d_data->idsx;
	int VIC = g2d_data->VIC;
	int factor_width  = g2d_data->factor_width;
	int factor_height = g2d_data->factor_height;
	int errTh;
	
	sscanf(buf, "%d", &errTh);
	g2d_data->errTh = errTh;
	g2d_dbg("errTh = %d\n", errTh);
	g2d_config(g2d_data, idsx, VIC, factor_width, factor_height);
	
	return count;
}

static ssize_t sharpen_hvdth_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int count;
	count = sprintf(buf, "%d", g2d_data->hvdTh);
	return count;
}

static ssize_t sharpen_hvdth_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int idsx = g2d_data->idsx;
	int VIC = g2d_data->VIC;
	int factor_width  = g2d_data->factor_width;
	int factor_height = g2d_data->factor_height;
	int hvdTh;
	
	sscanf(buf, "%d", &hvdTh);
	g2d_data->hvdTh = hvdTh;
	g2d_dbg("hvdTh = %d\n", hvdTh);
	g2d_config(g2d_data, idsx, VIC, factor_width, factor_height);
	
	return count;
}

static ssize_t test_suspend_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	g2d_suspend(&g2d_data->pdev->dev);
	return count;
}

static ssize_t test_resume_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	g2d_resume(&g2d_data->pdev->dev);
	return count;

}

static DEVICE_ATTR(sharpen_en, 0660, sharpen_en_show, sharpen_en_store);
static DEVICE_ATTR(sharpen_denos_en, 0660, sharpen_denos_en_show, sharpen_denos_en_store);
static DEVICE_ATTR(sharpen_coefw, 0666, sharpen_coefw_show, sharpen_coefw_store);
static DEVICE_ATTR(sharpen_coefa, 0660, sharpen_coefa_show, sharpen_coefa_store);
static DEVICE_ATTR(sharpen_errth, 0660, sharpen_errth_show, sharpen_errth_store);
static DEVICE_ATTR(sharpen_hvdth, 0660, sharpen_hvdth_show, sharpen_hvdth_store);
static DEVICE_ATTR(suspend, 0220, NULL, test_suspend_store);
static DEVICE_ATTR(resume, 0220, NULL, test_resume_store);

static struct attribute *g2d_attrs[] = {
	&dev_attr_sharpen_en.attr,
	&dev_attr_sharpen_denos_en.attr,
	&dev_attr_sharpen_coefw.attr,
	&dev_attr_sharpen_coefa.attr,
	&dev_attr_sharpen_errth.attr,
	&dev_attr_sharpen_hvdth.attr,
	&dev_attr_suspend.attr,
	&dev_attr_resume.attr,
	NULL
};

static const struct attribute_group g2d_attr_group = {
	.attrs = g2d_attrs,
};


int g2d_get_sharpness(void)
{
	struct g2d * g2d_data = g2d_data_global;
	return g2d_data->coefw;
}
EXPORT_SYMBOL(g2d_get_sharpness);

int g2d_set_sharpness(int sharpness)
{
	struct g2d * g2d_data = g2d_data_global;
	g2d_data->coefw = sharpness;
	g2d_info("coefw = %d\n", g2d_data->coefw);
	return 0;
}
EXPORT_SYMBOL(g2d_set_sharpness);

static void g2d_handler_wk(struct work_struct *work)
{
	struct g2d *g2d_data = (struct g2d *)container_of(work, struct g2d, wk);

	g2d_swap("complete(&g2d_data->g2d_done).\n");

	complete(&g2d_data->g2d_done);
	enable_irq(g2d_data->irq);
	
	return;
}

static irqreturn_t g2d_handler(int irq, void *data)
{
	struct g2d *g2d_data = (struct g2d *)data;
	unsigned int status = 0;
	
	g2d_swap("G2D IRQ\n");
	
	g2dpwl_read_reg(g2d_data, G2D_GLB_INTS, &status);
	if(status & G2DGLBINTS_0VER) {
		disable_irq_nosync(irq);
		queue_work(g2d_data->wq, &g2d_data->wk);
	}
	g2dpwl_write_reg(g2d_data, G2D_GLB_INTS, status);

	return IRQ_HANDLED;
}

static int g2d_draw(struct g2d * g2d_data)
{
	struct g2d_params * outCh = &g2d_data->dst;
	int g2d_fbx, tmp, ret = 0;
#ifdef G2D_TIME
	struct timeval s, e;
	unsigned int time;
	static int count = 0;
	static unsigned int total_us = 0;
	static unsigned int data[G2D_TIME_PERIOD] = {0};
	//int i;
#endif

	/* Start to Draw G2D framebuffer */
	g2d_swap("Starting to Draw G2D framebuffer...\n");
	INIT_COMPLETION(g2d_data->g2d_done);
	
	/* Enable/Reset G2D module (falling edge enable). Start to work */
	SetG2DRegister(g2d_data->regVal, G2D_SRC_XYST_SOFTRST, 0);
	g2dpwl_write_reg(g2d_data, g2d_data->regOft[rG2D_SRC_XYST], g2d_data->regVal[rG2D_SRC_XYST]);

	SetG2DRegister(g2d_data->regVal, G2D_SRC_XYST_SOFTRST, 1);
	g2dpwl_write_reg(g2d_data, g2d_data->regOft[rG2D_SRC_XYST], g2d_data->regVal[rG2D_SRC_XYST]);

	SetG2DRegister(g2d_data->regVal, G2D_SRC_XYST_SOFTRST, 0);
	g2dpwl_write_reg(g2d_data, g2d_data->regOft[rG2D_SRC_XYST], g2d_data->regVal[rG2D_SRC_XYST]);

#ifdef G2D_TIME
	do_gettimeofday(&s);
#endif

	/* Wait for interrupt */
	g2d_swap("Waiting for completion...\n");
	tmp = wait_for_completion_timeout(&g2d_data->g2d_done, G2D_DRAW_TIMEOUT);
	if (!tmp) {
		g2d_err("Critical Error: Time out while waiting for G2D scaling work. Do not test in pressure.\n");
		ret =  -1;
	}

#ifdef G2D_TIME
	do_gettimeofday(&e);
	time = e.tv_sec*1000000+e.tv_usec - (s.tv_sec*1000000+s.tv_usec);
	data[count] = time;
	total_us += (time);
	count++;
	if (count == G2D_TIME_PERIOD) {
		g2d_info("\n\n\n=================================================\n");
		g2d_info("\t\t Average G2D time = %d us. count = %d.\n", (total_us/count), count);
		g2d_info("=================================================\n\n\n\n");
	#if 0
		for (i = 0; i < G2D_TIME_PERIOD; i++) {
			printk("%d, ", data[i]);
			if ((i+1)%20 == 0)
				printk("\n");
		}
	#endif
		total_us = 0;
		count = 0;
	}	
#endif
	
	/* Set ourCh->phyaddr point to another G2D FB which is not use currently
	  * to avoid conflict between G2D DMA and IDS DMA that may have.  */
	g2d_swap("After G2D work done, change ourCh->phyaddr to another G2D FB which is not use currently.\n");
	g2d_fbx = g2d_data->g2d_fbx;
	outCh->phy_addr = g2d_data->main_fb_phyaddr + (g2d_fbx?0:1) * (g2d_data->main_fb_size/2);
	SetG2DRegister(g2d_data->regVal, G2D_IF_ADDR_ST, outCh->phy_addr);
	g2dpwl_write_reg(g2d_data, g2d_data->regOft[rG2D_IF_ADDR_ST], g2d_data->regVal[rG2D_IF_ADDR_ST]);

	return ret;
}

static int g2d_update_phyaddr(struct g2d * g2d_data, int main_fbx, int g2d_fbx)
{
	struct g2d_params * srcCh = &g2d_data->src;
	struct g2d_params * outCh = &g2d_data->dst;
	unsigned int pixelOffset, BuffOffset;
	
	/* Set srcCh and outCh FB phyaddr */
	outCh->phy_addr = g2d_data->main_fb_phyaddr + g2d_fbx * (g2d_data->main_fb_size/2);
	srcCh->phy_addr = g2d_data->g2d_fb_phyaddr + main_fbx * (g2d_data->g2d_fb_size/2);
	g2d_data->g2d_fbx = g2d_fbx;
	g2d_swap("srcCh->phy_addr = 0x%X, outCh->phy_addr = 0x%X\n", srcCh->phy_addr, outCh->phy_addr);
	
	/* Update srcCh physical address (include offset) registers */
	g2d_swap("Update srcCh phyaddr: 0x%X\n", srcCh->phy_addr);
	SetG2DRegister(g2d_data->regVal, G2D_Y_ADDR_ST, srcCh->phy_addr);
	g2dpwl_write_reg(g2d_data, g2d_data->regOft[rG2D_Y_ADDR_ST], g2d_data->regVal[rG2D_Y_ADDR_ST]);
	
	/* Update outCh physical address (include offset) registers */
	pixelOffset = outCh->yOffset * outCh->fulWidth + outCh->xOffset;
	BuffOffset = pixelOffset << 2;
	g2d_swap("Update outCh phyaddr: BuffOffset(0x%X) + phy_addr(0x%X) = 0x%X\n",
			BuffOffset, outCh->phy_addr, BuffOffset + outCh->phy_addr);
	SetG2DRegister(g2d_data->regVal, G2D_IF_ADDR_ST, BuffOffset + outCh->phy_addr);
	g2dpwl_write_reg(g2d_data, g2d_data->regOft[rG2D_IF_ADDR_ST], g2d_data->regVal[rG2D_IF_ADDR_ST]);

	return 0;
}

/* Swap framebuffer */
int g2d_work(int idsx, int VIC, int factor_width, int factor_height, int main_fbx, int * g2d_fbx_p)
{
	struct g2d * g2d_data = g2d_data_global;

	g2d_swap("idsx = %d, VIC = %d, factor_width = %d, factor_height = %d, main_fbx = %d",
			idsx, VIC, factor_width, factor_height, main_fbx);

	if (g2d_data->power) {
		/* Switch G2D FB */
		g2d_data->g2d_fbx = g2d_data->g2d_fbx?0:1;
		*g2d_fbx_p = g2d_data->g2d_fbx;

		/* Update G2D phyaddr registers */
		g2d_update_phyaddr(g2d_data, main_fbx, *g2d_fbx_p);

		/* Draw G2D framebuffer */
		g2d_draw(g2d_data);
	}
	else {
		g2d_dbg("G2D is powered down.\n");
	}

	return 0;
}
EXPORT_SYMBOL(g2d_work);

static int g2d_params_init(struct g2d * g2d_data, int VIC, int factor_width, int factor_height)
{
	struct g2d_params * srcCh = &g2d_data->src;
	struct g2d_params * outCh = &g2d_data->dst;
	struct g2d_alpha_rop * alphaRop = &g2d_data->alpharop;
	struct g2d_sclee * sclee = &g2d_data->sclee;
	struct g2d_dither *dith = &g2d_data->dith;
	dtd_t dtd;
	unsigned int VIC_width, VIC_height;
	unsigned int full_width, full_height, width, height;
	//unsigned int size;
	int i;

	g2d_dbg("Initializing G2D params...\n");

	/* Reset all G2D registers using system reset */
	writel(0xff, (unsigned int)g2d_data->sysmgr_virtual);
	writel(0x0, (unsigned int)g2d_data->sysmgr_virtual);

	/* Calculate srcCh params */
//	dtd_Fill(&dtd, main_VIC, 60000);
	VIC_width = (1280/*dtd.mHActive*/ * main_scaler_width / 100) & width_align;
	VIC_height = (720/*dtd.mVActive*/ * main_scaler_height / 100) & (~0x3);

	/* Calculate outCh params */
//	dtd_Fill(&dtd, VIC, 60000);
	full_width = 1280;//dtd.mHActive;
	full_height = 720;//dtd.mVActive;
	width = (full_width * factor_width / 100) & width_align;
	height = (full_height * factor_height / 100) & (~0x3);
	//size = width * height * G2D_FB_BPP;
	if ((full_width - width) % 4 ||(full_height - height) % 4) {
		g2d_err("Width and Height must be align with 4. Program Mistake.\n");
		return -1;
	}

	/* Update params */
	alphaRop->ckEn = 0;
	alphaRop->blendEn = 0;
	alphaRop->matchMode = 0;
	alphaRop->mask = 0;
	alphaRop->color = 0;
	alphaRop->gAlphaEn = 0;
	alphaRop->gAlphaA = 0;
	alphaRop->gAlphaB = 0;
	alphaRop->arType = G2D_AR_TYPE_SCLSRC;
	alphaRop->ropCode = 0;

	sclee->verEnable = 1;	// Scale Enable
	sclee->horEnable = 1;	// Scale Enable
	sclee->vrdMode = 1;	// 1: Open
	sclee->hrdMode = 1;	// 1: Open
	sclee->paramType = 0;
	
	sclee->eeEnable = g2d_data->sharpen_en;
	sclee->order = 0;
	sclee->coefw = g2d_data->coefw;
	sclee->coefa = g2d_data->coefa;
	sclee->rdMode = 0;
	sclee->denosEnable = g2d_data->denos_en ;
	sclee->gasMode = 0;
	sclee->errTh = g2d_data->errTh;
	sclee->opMatType = 0;
	sclee->hTh = g2d_data->hvdTh;
	sclee->vTh = g2d_data->hvdTh;
	sclee->d0Th = g2d_data->hvdTh;
	sclee->d1Th = g2d_data->hvdTh;

	srcCh->imgFormat = G2D_IMAGE_RGB_BPP24_888;
	srcCh->brRevs = 0;
	srcCh->width = VIC_width;
	srcCh->height = VIC_height;
	srcCh->xOffset = 0;
	srcCh->yOffset = 0;
	srcCh->fulWidth = VIC_width;
	srcCh->fulHeight = VIC_height;
	srcCh->vir_addr = 0;	// Not use
	//srcCh->size = g2d_data->main_fb_size/2;	// Not use
	
	outCh->brRevs = 0;
	outCh->width = width;
	outCh->height = height;
	outCh->xOffset = 0;
	outCh->yOffset = 0;
	outCh->fulWidth = width;
	outCh->fulHeight = height;
	outCh->vir_addr = 0;	// Not use
	//outCh->size = size;	// Not use

	if (VIC == 3 || VIC == 18 || (VIC == 4 && rgb_720P == 0)) {
		g2d_dbg("Using RGB888.\n");
		outCh->imgFormat = G2D_IMAGE_RGB_BPP24_888;
		dith->dithEn = 0;
		dith->tempoEn = 0;
	}
	else {
		g2d_dbg("Using RGB565.\n");
		outCh->imgFormat = G2D_IMAGE_RGB_BPP16_565;
		dith->dithEn = 1;
		dith->tempoEn = 1;
	}
	dith->rChannel = 3;
	dith->gChannel = 2;
	dith->bChannel = 3;

	g2d_data->needKeyFlush = 1;
	g2d_data->needAllFlush = 1;

	g2d_dbg("width = %d, height = %d, xoffset = %d, yoffset = %d, fullWidth = %d, fullHeight = %d\n",
			outCh->width, outCh->height, outCh->xOffset, outCh->yOffset, outCh->fulWidth, outCh->fulHeight);

	/* Set srcCh FB phyaddr */
	outCh->phy_addr = g2d_data->main_fb_phyaddr;	/* No effect. Refresh later. */
	/* Set outCh FB phyaddr */
	srcCh->phy_addr = g2d_data->g2d_fb_phyaddr;	/* No effect. Refresh later. */

	/* Init shadow registers from real register */
	for(i=0; i<G2DREGNUM; i++){
		g2d_data->regOft[i] = regOffsetTable[i];
		g2d_data->regVal[i] = 0;
	}

	/* Refresh registers */
	for(i = 0; i < G2DREGNUM; i++){
		g2dpwl_read_reg(g2d_data, g2d_data->regOft[i], g2d_data->regVal+i);
	}

	return 0;
}

static int g2d_shadow_registers_init(struct g2d * g2d_data)
{
	struct g2d_params * srcCh = &g2d_data->src;
	struct g2d_params * outCh = &g2d_data->dst;
	struct g2d_alpha_rop * alphaRop = &g2d_data->alpharop;
	struct g2d_sclee * sclee = &g2d_data->sclee;
	struct g2d_dither *dith = &g2d_data->dith;
	int * opMat, * GASMat;
	unsigned int hscaling, vscaling;
	unsigned int pixelOffset, BuffOffset;
	unsigned int * dithMat;
	int i;

	g2d_dbg("Initializing G2D shadow registers...\n");
	
	/* Set global alpha */
	SetG2DRegister(g2d_data->regVal, G2D_ALPHAG_GA, alphaRop->gAlphaA);
	SetG2DRegister(g2d_data->regVal, G2D_ALPHAG_GB, alphaRop->gAlphaB);

	g2d_data->needKeyFlush = 1;

	/* Set imgFormat */
	SetG2DRegister(g2d_data->regVal, G2D_SRC_FORMAT_BPP, srcCh->imgFormat);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_RGB_YUVFMT, 0x0);//000, rgb , 001, yuv444 , 010, yuv422 , 100, yuv420
	SetG2DRegister(g2d_data->regVal, G2D_SRC_WIDOFFSET, 0);

	/* Set BR REVERSE and bitswap, all zero */
	SetG2DRegister(g2d_data->regVal, G2D_SRC_RGB_BRREV, 0);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_BITSWP_BITADR, 0);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_BITSWP_BITSWP, 0);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_BITSWP_BIT2SWP, 0);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_BITSWP_HFBSWP, 0);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_BITSWP_BYTSWP, 0);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_BITSWP_HFWSWP, 0);

	/* Set width and height */
	SetG2DRegister(g2d_data->regVal, G2D_SRC_SIZE_WIDTH, srcCh->width);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_SIZE_HEIGHT, srcCh->height);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_SIZE_1_WIDTH, srcCh->width - 1);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_SIZE_1_HEIGHT, srcCh->height - 1);

	/* Set position and size */
	SetG2DRegister(g2d_data->regVal, G2D_SRC_FULL_WIDTH, srcCh->fulWidth);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_FULL_WIDTH_1, srcCh->fulWidth - 1);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_XYST_XSTART, srcCh->xOffset);
	SetG2DRegister(g2d_data->regVal, G2D_SRC_XYST_YSTART, srcCh->yOffset);

	/* Update srcCh physical address (include offset) registers */
	SetG2DRegister(g2d_data->regVal, G2D_Y_ADDR_ST, srcCh->phy_addr);

	/* Set colorkey enable */
	SetG2DRegister(g2d_data->regVal, G2D_CK_CNTL_CK_EN, alphaRop->ckEn);
	/* Set blend enable */
	SetG2DRegister(g2d_data->regVal, G2D_CK_CNTL_CK_ABLD_EN, alphaRop->blendEn);		
	/* Set global alpha enable */
	SetG2DRegister(g2d_data->regVal, G2D_ALPHAG_GSEL, alphaRop->gAlphaEn);
	/* Set global alpha param */
	SetG2DRegister(g2d_data->regVal, G2D_ALPHAG_GA, alphaRop->gAlphaA);
	SetG2DRegister(g2d_data->regVal, G2D_ALPHAG_GB, alphaRop->gAlphaB);
	/* Set colorkey match mode */
	SetG2DRegister(g2d_data->regVal, G2D_CK_CNTL_CK_DIR, alphaRop->matchMode);
	/* Set colorkey mask and maskcolor */
	SetG2DRegister(g2d_data->regVal, G2D_CK_MASK, alphaRop->mask);
	SetG2DRegister(g2d_data->regVal, G2D_CK_KEY, alphaRop->color);
	/* Set alpha-rop3 type */
	SetG2DRegister(g2d_data->regVal, G2D_CK_CNTL_AR_CODE, alphaRop->arType);
	/* Set rop3 code */
	SetG2DRegister(g2d_data->regVal, G2D_CK_CNTL_ROP_CODE, alphaRop->ropCode);
	
	/* Set scaling */
	vscaling = (srcCh->height * 1024) / outCh->height;
	hscaling = (srcCh->width * 1024) / outCh->width;
	g2d_dbg("vscaling = %d, hscaling = %d\n", vscaling, hscaling);
	SetG2DRegister(g2d_data->regVal, G2D_SCL_IFSR_IVRES, srcCh->height-1);
	SetG2DRegister(g2d_data->regVal, G2D_SCL_IFSR_IHRES, srcCh->width-1);
	SetG2DRegister(g2d_data->regVal, G2D_SCL_OFSR_OVRES, outCh->height-1);
	SetG2DRegister(g2d_data->regVal, G2D_SCL_OFSR_OHRES, outCh->width-1);
	SetG2DRegister(g2d_data->regVal, G2D_SCL_SCR_VSCALING, vscaling);
	SetG2DRegister(g2d_data->regVal, G2D_SCL_SCR_HSCALING, hscaling);
	
	/* Set scl round mode*/
	SetG2DRegister(g2d_data->regVal, G2D_SCL_SCR_VROUND, sclee->vrdMode);
	SetG2DRegister(g2d_data->regVal, G2D_SCL_SCR_HROUND, sclee->hrdMode);
	
	/*Set scl param type*/
	for(i=0; i<6*64; i++){
		SetG2DRegister(g2d_data->regVal, i+G2D_SCL_HSCR0_0, scl_params[i]);
	}
	
	/* Set scl enable or bypass state*/
	SetG2DRegister(g2d_data->regVal, G2D_SCL_SCR_VPASSBY, sclee->verEnable?0:1);	// !!!
	SetG2DRegister(g2d_data->regVal, G2D_SCL_SCR_HPASSBY, sclee->horEnable?0:1);	// !!!
	/* Set scl ee order */
	SetG2DRegister(g2d_data->regVal, G2D_EE_CNTL_FIRST, sclee->order);
	/* Set ee coefw*/
	SetG2DRegister(g2d_data->regVal, G2D_EE_COEF_COEFW, sclee->coefw);
	/* Set ee coefa*/
	SetG2DRegister(g2d_data->regVal, G2D_EE_COEF_COEFA, sclee->coefa);
	/* Set ee round mode */
	SetG2DRegister(g2d_data->regVal, G2D_EE_CNTL_ROUND, sclee->rdMode);
	
	/* Set ee gauss operator matrix mode*/
	GASMat = GASMat0;
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT4_GAM00, GASMat[0]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT4_GAM01, GASMat[1]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT4_GAM02, GASMat[2]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT4_GAM10, GASMat[3]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT4_GAM11_HIGH, ((GASMat[4]&0x8)>>3));
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT4_GAM11_LOW, (GASMat[4]&0x7));
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT4_GAM12, GASMat[5]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT4_GAM20, GASMat[6]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT4_GAM21, GASMat[7]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT4_GAM22, GASMat[8]);
	
	/* Set ee gauss operator matrix mode*/
	opMat = OpMat0;
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT0_HM00, opMat[0]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT0_HM01, opMat[1]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT0_HM02, opMat[2]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT0_HM10, opMat[3]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT0_HM11, opMat[4]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT0_HM12, opMat[5]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT0_HM20, opMat[6]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT0_HM21, opMat[7]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT0_HM22, opMat[8]);
	
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT1_VM00, opMat[9]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT1_VM01, opMat[10]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT1_VM02, opMat[11]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT1_VM10, opMat[12]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT1_VM11, opMat[13]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT1_VM12, opMat[14]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT1_VM20, opMat[15]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT1_VM21, opMat[16]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT1_VM22, opMat[17]);
	
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT2_D0M00, opMat[18]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT2_D0M01, opMat[19]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT2_D0M02, opMat[20]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT2_D0M10, opMat[21]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT2_D0M11, opMat[22]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT2_D0M12, opMat[23]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT2_D0M20, opMat[24]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT2_D0M21, opMat[25]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT2_D0M22, opMat[26]);

	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT3_D1M00, opMat[27]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT3_D1M01, opMat[28]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT3_D1M02, opMat[29]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT3_D1M10, opMat[30]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT3_D1M11, opMat[31]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT3_D1M12, opMat[32]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT3_D1M20, opMat[33]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT3_D1M21, opMat[34]);
	SetG2DRegister(g2d_data->regVal, G2D_EE_MAT3_D1M22, opMat[35]);

	/* Set ee error threshold*/
	SetG2DRegister(g2d_data->regVal, G2D_EE_COEF_ERR, sclee->errTh);

	/* Set ee detect threshold matrix */
	SetG2DRegister(g2d_data->regVal, G2D_EE_THRE_H, sclee->hTh);
	SetG2DRegister(g2d_data->regVal, G2D_EE_THRE_V, sclee->vTh);
	SetG2DRegister(g2d_data->regVal, G2D_EE_THRE_D0, sclee->d0Th);
	SetG2DRegister(g2d_data->regVal, G2D_EE_THRE_D1, sclee->d1Th);

	/* Set ee gauss filter for flat region enable*/
	SetG2DRegister(g2d_data->regVal, G2D_EE_CNTL_DENOISE_EN, sclee->denosEnable);

	/* Set ee enable state*/
	SetG2DRegister(g2d_data->regVal, G2D_EE_CNTL_BYPASS, sclee->eeEnable?0:1);	// !!!
	
	/* Set imgFormat */
	SetG2DRegister(g2d_data->regVal, G2D_OUT_BITSWP_FORMAT_BPP, outCh->imgFormat);

	/* Set bitswap, all zero */
	SetG2DRegister(g2d_data->regVal, G2D_OUT_BITSWP_BITADR, 0);
	SetG2DRegister(g2d_data->regVal, G2D_OUT_BITSWP_BITSWP, 0);
	SetG2DRegister(g2d_data->regVal, G2D_OUT_BITSWP_BIT2SWP, 0);
	SetG2DRegister(g2d_data->regVal, G2D_OUT_BITSWP_HFBSWP, 0);
	SetG2DRegister(g2d_data->regVal, G2D_OUT_BITSWP_BYTSWP, 0);
	SetG2DRegister(g2d_data->regVal, G2D_OUT_BITSWP_HFWSWP, 0);

	/* Set width and height */
	SetG2DRegister(g2d_data->regVal, G2D_IF_WIDTH, outCh->width);
	SetG2DRegister(g2d_data->regVal, G2D_IF_WIDTH_1, outCh->width - 1);
	SetG2DRegister(g2d_data->regVal, G2D_IF_HEIGHT, outCh->height);
	SetG2DRegister(g2d_data->regVal, G2D_IF_HEIGHT_1, outCh->height - 1);

	/* Set position and size */
	SetG2DRegister(g2d_data->regVal, G2D_IF_FULL_WIDTH, outCh->fulWidth);
	SetG2DRegister(g2d_data->regVal, G2D_IF_FULL_WIDTH_1, outCh->fulWidth - 1);
	SetG2DRegister(g2d_data->regVal, G2D_IF_XYST_XSTART, outCh->xOffset);
	SetG2DRegister(g2d_data->regVal, G2D_IF_XYST_YSTART, outCh->yOffset);

	/* Update outCh physical address (include offset) registers */
	pixelOffset = outCh->yOffset * outCh->fulWidth + outCh->xOffset;
	BuffOffset = pixelOffset << 2;
	SetG2DRegister(g2d_data->regVal, G2D_IF_ADDR_ST, BuffOffset + outCh->phy_addr);

#if 1
	/* Set dither */
	SetG2DRegister(g2d_data->regVal, G2D_DIT_FRAME_SIZE_HRES, (outCh->width-1));
	SetG2DRegister(g2d_data->regVal, G2D_DIT_FRAME_SIZE_VRES, (outCh->height-1));

	SetG2DRegister(g2d_data->regVal, G2D_DIT_DI_CNTL_RNB, dith->rChannel);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DI_CNTL_GNB, dith->gChannel);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DI_CNTL_BNB, dith->bChannel);

	dithMat = dithMat0;
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_0003_COEF00, dithMat[0]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_0003_COEF01, dithMat[1]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_0003_COEF02, dithMat[2]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_0003_COEF03, dithMat[3]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_0407_COEF04, dithMat[4]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_0407_COEF05, dithMat[5]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_0407_COEF06, dithMat[6]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_0407_COEF07, dithMat[7]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_1013_COEF10, dithMat[8]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_1013_COEF11, dithMat[9]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_1013_COEF12, dithMat[10]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_1013_COEF13, dithMat[11]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_1417_COEF14, dithMat[12]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_1417_COEF15, dithMat[13]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_1417_COEF16, dithMat[14]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_1417_COEF17, dithMat[15]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_2023_COEF20, dithMat[16]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_2023_COEF21, dithMat[17]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_2023_COEF22, dithMat[18]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_2023_COEF23, dithMat[19]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_2427_COEF24, dithMat[20]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_2427_COEF25, dithMat[21]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_2427_COEF26, dithMat[22]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_2427_COEF27, dithMat[23]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_3033_COEF30, dithMat[24]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_3033_COEF31, dithMat[25]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_3033_COEF32, dithMat[26]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_3033_COEF33, dithMat[27]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_3437_COEF34, dithMat[28]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_3437_COEF35, dithMat[29]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_3437_COEF36, dithMat[30]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DM_COEF_3437_COEF37, dithMat[31]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DE_COEF_COEF12, dithMat[32]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DE_COEF_COEF20, dithMat[33]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DE_COEF_COEF21, dithMat[34]);
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DE_COEF_COEF22, dithMat[35]);

	SetG2DRegister(g2d_data->regVal, G2D_DIT_DI_CNTL_TEMPO, (dith->tempoEn)?1:0);
	
	SetG2DRegister(g2d_data->regVal, G2D_DIT_DI_CNTL_BYPASS, (dith->dithEn)?0:1);
#endif	
	
	return 0;
}

static int g2d_registers_update(struct g2d *g2d_data)
{
	int i;

	/* Flush registers */
	g2d_dbg("Flushing registers ...\n");
	for(i = 0; i < MEMREGNUM; i++){
		g2dpwl_write_reg(g2d_data, g2d_data->regOft[i], g2d_data->regVal[i]);
	}
	
	/* May need to flush other key registers for other operation */  
	if(g2d_data->needKeyFlush == 1){
		g2d_dbg("needKeyFlush is TRUE.\n");
		g2d_data->needKeyFlush = 0;
		for(i = MEMREGNUM + 1; i < KEYREGNUM; i++){
			g2dpwl_write_reg(g2d_data, g2d_data->regOft[i], g2d_data->regVal[i]);
		}
	}

	/* May need to flush all registers as request. */
	if(g2d_data->needAllFlush == 1){
		g2d_dbg("needAllFlush is TRUE.\n");
		g2d_data->needAllFlush = 0;
	    	for(i = KEYREGNUM; i < G2DREGNUM; i++){
	    		g2dpwl_write_reg(g2d_data, g2d_data->regOft[i], g2d_data->regVal[i]);
	    	}
	}

	return 0;
}

#if 0
static int g2d_mmu_init(struct g2d * g2d_data)
{
	int ret;
	unsigned int temp;
	static int only_once = 0;

	g2d_dbg("running...\n");

	if (!only_once) {
		only_once = 1;
	
		/* Init G2D dmmu */
		ret = pmmdrv_dmmu_init(g2d_data->dmmu_handle, DMMU_DEV_G2D);
		if(ret != 0){
			g2d_err("pmmdrv_dmmu_init() failed!");
			return -1;
		}

		/* Disable G2D mmu */
		ret = pmmdrv_dmmu_disable(g2d_data->dmmu_handle);
		if(ret != 0){
			g2d_err("pmmdrv_release() failed!");
			return -1;
		}
		/* Disable G2D mmu write/read */
		temp = readl((unsigned int)g2d_data->mmu_virtual + G2D_MMU_SWITCH_OFFSET);
		temp &= 0xfffffffc;
		writel(temp, (unsigned int)g2d_data->mmu_virtual + G2D_MMU_SWITCH_OFFSET);
	}

	return 0;
}
#endif

#if 0
static int g2d_mmu_deinit(struct g2d * g2d_data)
{
	int ret;

	g2d_dbg("running...\n");
	
	/* Deinit G2D dmmu */
	ret = pmmdrv_dmmu_deinit(g2d_data->dmmu_handle);
	if(ret != 0){
		g2d_err("pmmdrv_dmmu_deinit() failed!");
		return -1;
	}
	return 0;
}
#endif

static int g2d_in_use(struct g2d * g2d_data, int VIC, int factor_width, int factor_height)
{
	g2d_dbg("VIC = %d, factor_width = %d, factor_height = %d\n", VIC, factor_width, factor_height);
	
	if (!g2d_data->power) {
		g2d_dbg("Power on G2D module.\n");
		g2d_data->power = 1;
		module_power_on(g2d_data->sysmgr_physical);
//		g2d_mmu_init(g2d_data);
	}
	g2d_params_init(g2d_data, VIC, factor_width, factor_height);
	g2d_shadow_registers_init(g2d_data);
	g2d_registers_update(g2d_data);

	return 0;
}

static int g2d_not_use(struct g2d * g2d_data)
{
	struct g2d_params * srcCh = &g2d_data->src;
	struct g2d_params * outCh = &g2d_data->dst;

	g2d_dbg("running...\n");

	if (g2d_data->power) {
		g2d_data->power = 0;

		/* When not use G2D FB, Set srcCh->phyaddr to G2D FB0, set outCh->phyaddr to G2D FB1,
		  * and power down G2D module to avoid confict with others DMA that may have */
		g2d_dbg("Use Main FB instead of G2D FB, change G2D phyaddr pointers.\n");
		srcCh->phy_addr = g2d_data->g2d_fb_phyaddr;
		outCh->phy_addr = g2d_data->g2d_fb_phyaddr + (g2d_data->g2d_fb_size/2);
		SetG2DRegister(g2d_data->regVal, G2D_Y_ADDR_ST, srcCh->phy_addr);
		g2dpwl_write_reg(g2d_data, g2d_data->regOft[rG2D_Y_ADDR_ST], g2d_data->regVal[rG2D_Y_ADDR_ST]);
		SetG2DRegister(g2d_data->regVal, G2D_IF_ADDR_ST, outCh->phy_addr);
		g2dpwl_write_reg(g2d_data, g2d_data->regOft[rG2D_IF_ADDR_ST], g2d_data->regVal[rG2D_IF_ADDR_ST]);

		//g2d_mmu_deinit(g2d_data);
		g2d_dbg("Power off G2D module.\n");
		module_power_down(g2d_data->sysmgr_physical);
	}

	return 0;
}

int g2d_convert_logo(unsigned int bmp_width, unsigned int bmp_height, int fbx)
{
	struct g2d *g2d_data = g2d_data_global;
	struct g2d_params * srcCh = &g2d_data->src;
	struct g2d_params * outCh = &g2d_data->dst;
	struct g2d_alpha_rop * alphaRop = &g2d_data->alpharop;
	struct g2d_sclee * sclee = &g2d_data->sclee;
	struct g2d_dither *dith = &g2d_data->dith;
	dtd_t dtd;
	unsigned int full_VIC_width, full_VIC_height, VIC_width, VIC_height;
	//unsigned int size;
	int i;

//	g2d_info("Converting Logo...\n");

	if (!g2d_data->power) {
		g2d_dbg("Power on G2D module.\n");
		g2d_data->power = 1;
		module_power_on(g2d_data->sysmgr_physical);
//		g2d_mmu_init(g2d_data);
	}

	/* Reset all G2D registers using system reset */
	writel(0xff, (unsigned int)g2d_data->sysmgr_virtual);
	writel(0x0, (unsigned int)g2d_data->sysmgr_virtual);

	/* Calculate outCh params */
//	dtd_Fill(&dtd, main_VIC, 60000);
	full_VIC_width = 1280;//dtd.mHActive;
	full_VIC_height = 720;//dtd.mVActive;

	VIC_width = (full_VIC_width * main_scaler_width / 100) & width_align;
	VIC_height = (full_VIC_height * main_scaler_height / 100) & (~0x3);
	if ((full_VIC_width - VIC_width) % 4 ||(full_VIC_height - VIC_height) % 4) {
		g2d_err("VIC_width and VIC_height must be align with 4. Program Mistake.\n");
		return -1;
	}

	/* Update params */
	alphaRop->ckEn = 0;
	alphaRop->blendEn = 0;
	alphaRop->matchMode = 0;
	alphaRop->mask = 0;
	alphaRop->color = 0;
	alphaRop->gAlphaEn = 0;
	alphaRop->gAlphaA = 0;
	alphaRop->gAlphaB = 0;
	alphaRop->arType = G2D_AR_TYPE_SCLSRC;
	alphaRop->ropCode = 0;

	sclee->verEnable = 1;	// Scale Enable
	sclee->horEnable = 1;	// Scale Enable
	sclee->vrdMode = 1;	// 1: Open
	sclee->hrdMode = 1;	// 1: Open
	sclee->paramType = 0;
	
	sclee->eeEnable = g2d_data->sharpen_en;
	sclee->order = 0;
	sclee->coefw = g2d_data->coefw;
	sclee->coefa = g2d_data->coefa;
	sclee->rdMode = 0;
	sclee->denosEnable = g2d_data->denos_en ;
	sclee->gasMode = 0;
	sclee->errTh = g2d_data->errTh;
	sclee->opMatType = 0;
	sclee->hTh = g2d_data->hvdTh;
	sclee->vTh = g2d_data->hvdTh;
	sclee->d0Th = g2d_data->hvdTh;
	sclee->d1Th = g2d_data->hvdTh;

	srcCh->imgFormat = G2D_IMAGE_RGB_BPP24_888;
	srcCh->brRevs = 0;
	srcCh->width = bmp_width;
	srcCh->height = bmp_height;
	srcCh->xOffset = 0;
	srcCh->yOffset = 0;
	srcCh->fulWidth = bmp_width;
	srcCh->fulHeight = bmp_height;
	srcCh->vir_addr = 0;	// Not use
	//srcCh->size = g2d_data->main_fb_size/2;	// Not use

	outCh->imgFormat = G2D_IMAGE_RGB_BPP24_888;
	outCh->brRevs = 0;
	outCh->width = VIC_width;
	outCh->height = VIC_height;
	outCh->xOffset = 0;
	outCh->yOffset = 0;
	outCh->fulWidth = VIC_width;
	outCh->fulHeight = VIC_height;
	outCh->vir_addr = 0;	// Not use
	//outCh->size = size;	// Not use

	dith->dithEn = 0;
	dith->tempoEn = 0;
	dith->rChannel = 3;
	dith->gChannel = 2;
	dith->bChannel = 3;

	g2d_data->needKeyFlush = 1;
	g2d_data->needAllFlush = 1;

	g2d_dbg("width = %d, height = %d, xoffset = %d, yoffset = %d, fullWidth = %d, fullHeight = %d\n",
			outCh->width, outCh->height, outCh->xOffset, outCh->yOffset, outCh->fulWidth, outCh->fulHeight);

	/* Set srcCh FB phyaddr */
	srcCh->phy_addr = g2d_data->g2d_fb_phyaddr;
	/* Set outCh FB phyaddr */
	outCh->phy_addr = g2d_data->main_fb_phyaddr + (g2d_data->main_fb_size/2) * fbx;

	/* Init shadow registers from real register */
	for(i=0; i<G2DREGNUM; i++) {
		g2d_data->regOft[i] = regOffsetTable[i];
		g2d_data->regVal[i] = 0;
	}

	/* Refresh registers */
	for(i = 0; i < G2DREGNUM; i++) {
		g2dpwl_read_reg(g2d_data, g2d_data->regOft[i], g2d_data->regVal+i);
	}

	g2d_shadow_registers_init(g2d_data);
	g2d_registers_update(g2d_data);

	/* Convert Logo */
	g2d_draw(g2d_data);

	return 0;
}
EXPORT_SYMBOL(g2d_convert_logo);

static int fb_size_match(int VIC)
{
	int i, j;

	/*
	if (main_VIC == VIC)
		return 1;

	for (i = 0; i < ARRAY_SIZE(VICs_1080p); i++) {
		if (main_VIC == VICs_1080p[i]) {
			for (j = 0; j < ARRAY_SIZE(VICs_1080p); j++)
				if (VIC == VICs_1080p[j])
					return 1;
			break;
		}
	}
	*/
	return 0;
}

/* G2D using strategy. Use Main FB or G2D FB according to VIC and factor */
int g2d_using_strategy(int VIC, int factor_width, int factor_height)
{
	if (use_main_fb_resize && fb_size_match(VIC) && main_scaler_width == factor_width &&  main_scaler_height == factor_height)
		return 0;		/* Not use G2D */
	else
		return 1;		/* Use G2D */
}
EXPORT_SYMBOL(g2d_using_strategy);

static int g2d_config(void *data, int idsx, int VIC, int factor_width, int factor_height)
{
	struct g2d * g2d_data = (struct g2d *)data;

	g2d_dbg("idsx = %d, VIC = %d, factor_width = %d, factor_height = %d\n", idsx, VIC, factor_width, factor_height);

	g2d_data->g2d_on = 1;
	g2d_data->idsx = idsx;
	g2d_data->VIC = VIC;
	g2d_data->factor_width = factor_width;
	g2d_data->factor_height = factor_height;

//	if (!g2d_using_strategy(VIC, factor_width, factor_height))
//		g2d_not_use(g2d_data);			/* Not use G2D FB */
//	else
		g2d_in_use(g2d_data, VIC, factor_width, factor_height);	/* Use G2D FB */

//	ids_refresh_buffer(idsx, VIC, factor_width, factor_height);

	return 0;
}

static int g2d_disable(void *data, int idsx)
{
	struct g2d * g2d_data = (struct g2d *)data;
	g2d_dbg("idsx = %d\n", idsx);
	g2d_data->g2d_on = 0;
	g2d_not_use(g2d_data);
	return 0;
}

static int g2d_initialize(struct g2d *g2d_data)
{
	struct platform_device *pdev = g2d_data->pdev;
	struct resource *res;
	int ret;
	unsigned int size,  tmp;
	
	g2d_info("Initializing G2D module...\n");

	/*Get platform resource REGBASE */
	g2d_dbg("Getting platform resource REGBASE...\n");
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
	{
		g2d_err("Failed to get mem resource\n");
		ret = -ENXIO;
		goto out;
	}
	size = (res->end - res->start) + 1;
	g2d_dbg("REGBASE: res->start = 0x%X, size = 0x%X\n", res->start, size);
	g2d_data->regbase_physical = res->start;
	g2d_data->regbase_virtual = ioremap_nocache(res->start, size);

	/*Get platform resource SYSMGR */
	g2d_dbg("Getting platform resource SYSMGR...\n");
	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!res)
	{
		g2d_err("Failed to get mem resource\n");
		ret = -ENXIO;
		goto out;
	}
	size = (res->end - res->start) + 1;
	g2d_dbg("SYSMGR: res->start = 0x%X, size = 0x%X\n", res->start, size);
	g2d_data->sysmgr_physical = res->start;
	g2d_data->sysmgr_virtual = ioremap_nocache(res->start, size);
		
	/*Get platform resource MMU */
	g2d_dbg("Getting platform resource MMU...\n");
	res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
	if (!res)
	{
		g2d_err("Failed to get mem resource\n");
		ret = -ENXIO;
		goto out;
	}
	size = (res->end - res->start) + 1;
	g2d_dbg("MMU: res->start = 0x%X, size = 0x%X\n", res->start, size);
	g2d_data->mmu_physical = res->start;
	g2d_data->mmu_virtual = ioremap_nocache(res->start, size);	

	/*Get platform resource IRQ */
	g2d_dbg("Getting platform resource IRQ...\n");
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res)
	{
		g2d_err("Failed to get irq resources\n");
		ret = -ENXIO;
		goto out;
	}
	g2d_data->irq = res->start;
	g2d_dbg("IRQ: g2d_data->irq = %d\n", g2d_data->irq);

	/* Open G2D pmm handle */
	/*
	g2d_dbg("Opening G2D pmm handle...\n");
	ret = pmmdrv_open(&g2d_data->dmmu_handle, "G2D");
	if(ret != 0){
		g2d_err("pmmdrv_open() failed!");
		return -1;
	}
	*/

	/* Request g2d irq */
	g2d_dbg("Requesting G2D irq...\n");
	ret = request_irq(g2d_data->irq, g2d_handler, IRQF_DISABLED, "g2d_irq", (void *)g2d_data);
	if (ret) {
		g2d_err("Failed to request irq %d\n", g2d_data->irq);
		goto out;
	}

	/* Set interrupt mask disable */
	g2d_dbg("Cleaning G2D interrupt mask...\n");
	g2dpwl_read_reg(g2d_data, G2D_GLB_INTM, &tmp);
	tmp &= (~G2DGLBINTM_OVER);
	g2dpwl_write_reg(g2d_data, G2D_GLB_INTM, tmp);

	g2d_data->power = 0;
	g2d_data->needAllFlush = 1;

#if 0
	/* Init ids_module */
	INIT_LIST_HEAD(&m_G2D.list);
	m_G2D.config = g2d_config;
	m_G2D.disable = g2d_disable;
	m_G2D.private = g2d_data;
	m_G2D.level = 1;
	m_G2D.name = M_G2D;
#endif

	g2d_complete = 1;
	
	g2d_info("G2D module successfully initialized!		@_@\n");

	return 0;
		
out:
	return ret;
}

static int g2d_suspend(struct device *dev)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);

	g2d_info("Suspending..., do nothing now!\n");
	return 0;

	g2d_not_use(g2d_data);
	return 0;
}

static int g2d_resume(struct device *dev)
{
	struct g2d *g2d_data = dev_get_drvdata(dev);
	int idsx = g2d_data->idsx;
	int VIC = g2d_data->VIC;
	int factor_width = g2d_data->factor_width;
	int factor_height = g2d_data->factor_height;

	g2d_info("Resuming..., do nothing now!\n");
	return 0;

	if (g2d_data->g2d_on)
		g2d_config(g2d_data, idsx, VIC, factor_width, factor_height);
	return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void g2d_early_suspend(struct early_suspend *h)
{
	struct g2d *g2d_data;
	g2d_dbg("running...\n");
	g2d_data = container_of(h, struct g2d, early_suspend);
	g2d_suspend(&g2d_data->pdev->dev);
}

static void g2d_late_resume(struct early_suspend *h)
{
	struct g2d *g2d_data;
	g2d_dbg("running...\n");
	g2d_data = container_of(h, struct g2d, early_suspend);
	g2d_resume(&g2d_data->pdev->dev);
}
#endif

static int g2d_fb_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int g2d_fb_release(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t g2d_fb_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct g2d *g2d_data= g2d_data_global;
	int width, height, bytes;
	unsigned long total_size;
	unsigned long p = *ppos;
	u8 __iomem *src;
	u8 *buffer, *dst;
	int c, cnt = 0, err = 0;

	width = g2d_data->dst.fulWidth;
	height = g2d_data->dst.fulHeight;
	if (g2d_data->dst.imgFormat == G2D_IMAGE_RGB_BPP24_888)
		bytes = 4;
	else if (g2d_data->dst.imgFormat == G2D_IMAGE_RGB_BPP16_565)
		bytes = 2;
	else {
		g2d_err("Invalid imgFormat %d\n", g2d_data->dst.imgFormat);
		return -1;
	}
	total_size = width * height * bytes;

	if (p >= total_size)
		return 0;

	if (count >= total_size)
		count = total_size;

	if (count + p > total_size)
		count = total_size - p;

	buffer = kmalloc((count > PAGE_SIZE) ? PAGE_SIZE : count,
			 GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;

	src = (u8 __iomem *) ((u32)(g2d_data->g2d_fb_viraddr) + p);

	while (count) {
		c  = (count > PAGE_SIZE) ? PAGE_SIZE : count;
		dst = buffer;
		memcpy(dst, src, c);
		dst += c;
		src += c;

		if (copy_to_user(buf, buffer, c)) {
			err = -EFAULT;
			break;
		}
		*ppos += c;
		buf += c;
		cnt += c;
		count -= c;
	}

	kfree(buffer);

	return (err) ? err : cnt;
}

static long g2d_fb_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct g2d *g2d_data= g2d_data_global;
	unsigned int var;
	void __user *argp = (void __user *)arg;
	long ret = 0;

	switch (cmd) {
		case G2D_FB_GET_WIDTH:
			var = g2d_data->dst.fulWidth;
			ret = copy_to_user(argp, &var, sizeof(var)) ? -EFAULT : 0;
			break;
		case G2D_FB_GET_HEIGHT:
			var = g2d_data->dst.fulHeight;
			ret = copy_to_user(argp, &var, sizeof(var)) ? -EFAULT : 0;
			break;
		case G2D_FB_GET_BPP:
			var = (g2d_data->dst.imgFormat == G2D_IMAGE_RGB_BPP24_888) ? 32 : 16;
			ret = copy_to_user(argp, &var, sizeof(var)) ? -EFAULT : 0;
			break;
		default:
			g2d_err("Invalid cmd 0x%X\n", cmd);
			return -EINVAL;
	}
	
	return ret;	
}

static const struct file_operations g2d_fb_fops = {
	.owner =		THIS_MODULE,
	.read =		g2d_fb_read,
	.unlocked_ioctl = g2d_fb_ioctl,
	.open =		g2d_fb_open,
	.release =	g2d_fb_release,
	.llseek =		default_llseek,
};


extern int hdmi_irq_init(void);
extern dma_addr_t      gmap_dma_f1;
extern u_char *        gmap_cpu_f1;
static u64 imapx800_g2d_dma_mask = 0xffffffffUL;
/* Assume g2d_probe() will run after imapfb_probe() */
static int g2d_probe(struct platform_device *pdev)
{
	int ret;
	struct g2d *g2d_data;
//	alc_buffer_t alcBuffer;
	unsigned int flag;
	dtd_t dtd;
	unsigned int *addr;

	g2d_info("G2D Probe running...\n");
	g2d_dbg("G2D DEBUG ON.\n");
	
	/* Allocate struct g2d */
	g2d_dbg("Allocating struct g2d...\n");
	g2d_data = kzalloc(sizeof(struct g2d), GFP_KERNEL);
	if (!g2d_data) {
		g2d_err("Failed to allocate memory\n");
		ret = -ENOMEM;
		goto out;
	}
	g2d_data_global = g2d_data;

	/* Sharpen */
	g2d_data->sharpen_en = SHARPEN_ENABLE;
	g2d_data->denos_en = SHARPEN_DENOS_EN;
	g2d_data->coefw = SHARPEN_COEFW;
	g2d_data->coefa = SHARPEN_COEFA;
	g2d_data->errTh = SHARPEN_ERRTH;
	g2d_data->hvdTh = SHARPEN_HVDTH;

	if (item_exist(P_SHP)) {
		g2d_data->coefw = item_integer(P_SHP, 0);
		g2d_dbg("Got saved param: sharpness = %d\n", g2d_data->coefw);
	}
	
//	dtd_Fill(&dtd, G2D_VIC, 60000);
	g2d_data->g2d_fb_size = 1280*720/*dtd.mHActive *  dtd.mVActive*/ * G2D_FB_BPP  * 2;
	g2d_dbg("G2D Framebuffer size = 0x%X\n", g2d_data->g2d_fb_size);
	
	/* Allocate G2D framebuffer. Total Size is 1080P */
	g2d_dbg("Allocating G2D framebuffer...\n");
//	flag = ALC_FLAG_PHY_MUST | ALC_FLAG_DEVADDR;
//	ret = pmmdrv_mm_alloc(fb_pmmHandle, g2d_data->g2d_fb_size, flag, &alcBuffer);
	pdev->dev.coherent_dma_mask = DMA_BIT_MASK(32);
	pdev->dev.dma_mask = &imapx800_g2d_dma_mask;
	addr = dma_alloc_writecombine(&pdev->dev, 
			PAGE_ALIGN(720*1280*4*2) , &g2d_data->main_fb_phyaddr, GFP_KERNEL);
	if (!addr){
		g2d_err("pmmdrv_mm_alloc() failed\n");
		return -ENOMEM;
	}
	g2d_data->g2d_fb_viraddr = gmap_dma_f1;
	g2d_data->g2d_fb_phyaddr = gmap_cpu_f1;
	g2d_data->main_fb_size = 1280*720*2*4;//main_fb_size;
//	ids_set_g2d_fb_addr(g2d_data->g2d_fb_phyaddr, g2d_data->g2d_fb_size, (u32)g2d_data->g2d_fb_viraddr);

	g2d_dbg("Main FB phyaddr = 0x%X, size = 0x%X\n", g2d_data->main_fb_phyaddr, g2d_data->main_fb_size);
	g2d_dbg("G2D FB phyaddr = 0x%X, viraddr = 0x%X\n", g2d_data->g2d_fb_phyaddr, (u32)g2d_data->g2d_fb_viraddr);

	/* Set private data */
	g2d_data->pdev = pdev;
	dev_set_drvdata(&pdev->dev, g2d_data);

	/* Creating workqueue */
	g2d_dbg("Creating workqueue...\n");
	g2d_data->wq = create_singlethread_workqueue("g2d_wq");
	if (!g2d_data->wq) {
		g2d_err("create_singlethread_workqueue() failed\n");
		ret = -ENOMEM;
		goto out_data;
	}

	/* Initialize work and completion */
	g2d_dbg("Initializing work and completion...\n");
	INIT_WORK(&g2d_data->wk, g2d_handler_wk);
	init_completion(&g2d_data->g2d_done);

	/* Create sysfs attribute files */
	g2d_dbg("Creating sysfs attribute files...\n");
	ret = sysfs_create_link(NULL, &pdev->dev.kobj, G2D_SYSFS_LINK_NAME);
	if (ret) {
		g2d_err("sysfs_create_link(%s) failed\n", G2D_SYSFS_LINK_NAME);
		goto out_data;
	}
		
	ret = sysfs_create_group(&pdev->dev.kobj, &g2d_attr_group);
	if (ret) {
		g2d_err("sysfs_create_group() failed\n");
		goto out_sysfs_link;
	}

	/* Create dev node for g2d framebuffer */
	g2d_dbg("Creating dev node for g2d framebuffer...\n");
	g2d_data->devno = MKDEV(G2D_DEV_MAJOR, 0);
	register_chrdev_region(g2d_data->devno, 1, G2D_FB_NAME);
	g2d_data->cdev = cdev_alloc();
	cdev_init(g2d_data->cdev, &g2d_fb_fops);
	g2d_data->cdev->owner = THIS_MODULE;
	ret = cdev_add(g2d_data->cdev, g2d_data->devno, 1);
	if(ret) {
		g2d_err("Failed to add cdev %d\n", ret);
		goto out_chrdev;
	}

	g2d_data->dev = device_create(fb_class, &g2d_data->pdev->dev, g2d_data->devno, g2d_data, "%s", G2D_FB_NAME);
	if (IS_ERR(g2d_data->dev )) {
		g2d_err("Unable to create device for g2d framebuffer, errno = %ld\n", PTR_ERR(g2d_data->dev));
		goto out_cdev_del;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	/* Register early_suspend */
	g2d_dbg("Registering g2d early_suspend...\n");
	g2d_data->early_suspend.level = EARLY_SUSPEND_LEVEL_STOP_DRAWING;
	g2d_data->early_suspend.suspend = g2d_early_suspend;
	g2d_data->early_suspend.resume = g2d_late_resume;
	register_early_suspend(&g2d_data->early_suspend);
#endif

	/* Initialize g2d module */
	ret = g2d_initialize(g2d_data);
	if (ret)
		g2d_err("g2d_initialize() failed %d\n", ret);

	ret = hdmi_irq_init();
	if (ret) {
		g2d_err("int hdmi irq failed %d\n", ret);
	}

	g2d_dbg("G2D probe End Successfully.\n");
	
	return 0;

out_cdev_del:
	cdev_del(g2d_data->cdev);
out_chrdev:
	unregister_chrdev_region(g2d_data->devno, 1);
out_sysfs_link:
	sysfs_remove_link(NULL, G2D_SYSFS_LINK_NAME);
out_data:
	kfree(g2d_data);
out:
	return ret;
}


static int g2d_remove(struct platform_device *pdev)
{
/*
	struct g2d *g2d_data;
	g2d_data = i2c_get_clientdata(client);
	
	unregister_early_suspend(&g2d_data->early_suspend);
	sysfs_remove_group(&client->dev.kobj, &g2d_attr_group);
	sysfs_remove_link(NULL, SYSFS_LINK_NAME);
	free_irq(imapx_gpio_to_irq(g2d_data->irq_gpio), g2d_data);
	flush_workqueue(g2d_data->wq);
	destroy_workqueue(g2d_data->wq);
	kfree(g2d_data);
*/	
	return 0;
}


#ifndef CONFIG_HAS_EARLYSUSPEND
static const struct dev_pm_ops g2d_pm_ops = {
	.suspend		= g2d_suspend,
	.resume		= g2d_resume,
};
#endif

static struct platform_driver g2d_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "imapx-g2d",
#ifndef CONFIG_HAS_EARLYSUSPEND
		.pm = &g2d_pm_ops,
#endif
	},
	.probe = g2d_probe,
	.remove = g2d_remove,
};

static int __init g2d_i800_init(void)
{
	g2d_dbg("g2d_i800_init()\n");
	return platform_driver_register(&g2d_driver);
}

static void __exit g2d_i800_exit(void)
{
	g2d_dbg("g2d_i800_exit()\n");
	platform_driver_unregister(&g2d_driver);
}

late_initcall(g2d_i800_init);
module_exit(g2d_i800_exit);

MODULE_DESCRIPTION("InfoTM iMAPx800 G2D driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL");
