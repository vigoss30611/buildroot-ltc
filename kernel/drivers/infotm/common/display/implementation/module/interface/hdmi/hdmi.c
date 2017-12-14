/*
 * hdmi.c - display hdmi controller driver
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <dss_common.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/mach/irq.h>
#include <mach/imap-iomap.h>
#include <mach/pad.h>
#include <mach/power-gate.h>
#include <dss_common.h>
#include <ids_access.h>
#include "access.h"
#include "api.h"
#include "dtd.h"
#include "videoParams.h"
#include "audioParams.h"
#include "productParams.h"
#include "hdcpParams.h"
#include "control.h"
#include "hdcp.h"


#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	"[interface]"
#define DSS_SUB2	"[hdmi]"


#define SYS_HDMI_IIS_SPDIF_CLK_BASE	0x21E2801C

extern int hdmi_hpd_event(int hpd);
extern int dss_hdmi_config(int VIC);
static int set_enable(int idsx, int *params, int num);
static int set_vic(int idsx, int *params, int num);
static int get_connect(int idsx, int *params, int num);
static int dss_init_module(int idsx);
static struct mutex edid_lock;

static const char vendor_name[] = {"InfoTM"};
static const char product_name[] = {"Media Box"};
static unsigned char source_type = 0x0D;	// PMP
int hdmi_state = 0;
extern int pt;

static int vic_config = 0;
int hdmi_hplg_irq;

static struct module_attr hdmi_attr[] = {
	{TRUE, ATTR_ENABLE, set_enable},
	{TRUE, ATTR_VIC, set_vic},
	{TRUE, ATTR_GET_CONNECT, get_connect},
	{FALSE, ATTR_END, NULL},
};

struct module_node hdmi_module = {
	.name = "hdmi",
	.attributes = hdmi_attr,
	.init = dss_init_module,
};


static int get_connect(int idsx, int *params, int num)
{
	assert_num(1);

	*params = hdmi_state;

	return 0;
}

static int set_vic(int idsx, int *params, int num)
{
	int vic = *params;
	assert_num(1);
	dss_trace("ids%d, vic %d\n", idsx, vic);
	
	vic_config = vic;
	
	return 0;
}

static int hdmi_params_config(int VIC, videoParams_t * param_video, 
	audioParams_t * param_audio, productParams_t * param_product, hdcpParams_t * param_hdcp)
{
	dtd_t dtd;
	int hdmi = 1;
		
	/* Set videoParams */
	dss_dbg("Setting videoParams...\n");
	dtd_Fill(&dtd, VIC, 60000);
	videoParams_Reset(param_video);
	videoParams_SetHdmi(param_video, hdmi);	// HDMI->audio ON, DVI->audio OFF
	dss_dbg("Port type: %s, audio %s\n", hdmi?"HDMI":"DVI", hdmi?"ON":"OFF");
	videoParams_SetEncodingIn(param_video, RGB);
	videoParams_SetEncodingOut(param_video, RGB); 
	videoParams_SetColorResolution(param_video, 8);
	videoParams_SetDtd(param_video, &dtd);
	videoParams_SetPixelRepetitionFactor(param_video, 0);
	videoParams_SetPixelPackingDefaultPhase(param_video, 0);
	videoParams_SetColorimetry(param_video, ITU709);
	videoParams_SetHdmiVideoFormat(param_video, 0);	// 3D relation
	videoParams_Set3dStructure(param_video, 0);
	videoParams_Set3dExtData(param_video, 0); 
	
	/* Set audioParams */
	dss_dbg("Setting audioParams...\n");
	audioParams_Reset(param_audio);
	audioParams_SetInterfaceType(param_audio, I2S);
	audioParams_SetCodingType(param_audio, PCM);
	audioParams_SetChannelAllocation(param_audio, 0);
	audioParams_SetPacketType(param_audio, AUDIO_SAMPLE);
	audioParams_SetSampleSize(param_audio, 16);
	audioParams_SetSamplingFrequency(param_audio, 48000);
	audioParams_SetLevelShiftValue(param_audio, 0);
	audioParams_SetDownMixInhibitFlag(param_audio, 0);
	audioParams_SetClockFsFactor(param_audio, 64);// 64:I2S, 128:SPDIF

	/* Set Source Product Descripition */
	dss_dbg("Setting Source Product Descripition...\n");
	productParams_Reset(param_product);
	productParams_SetVendorName(param_product, vendor_name, sizeof(vendor_name));
	productParams_SetProductName(param_product, product_name, sizeof(product_name));
	productParams_SetSourceType(param_product, source_type);

	/* Set HDCP params */
	dss_dbg("Setting HDCP params...\n");
	hdcpParams_Reset(param_hdcp);
	hdcpParams_SetEnable11Feature(param_hdcp, 1);
	hdcpParams_SetEnhancedLinkVerification(param_hdcp, 1);
	hdcpParams_SetI2cFastMode(param_hdcp, 0);
	hdcpParams_SetRiCheck(param_hdcp, 1);
	
	return 0;
}

void dump_ids1(void)
{
	uint32_t regs[] = {
		0x0, 0x4, 0x8, 0xc, 0x10, 0x18, 0x30, 0x54, 0x58, 0x5c,
		0x5100, 0x5104, 0x5108, 0x510c,
	}, i;

	void __iomem * r;

	r = ioremap(0x23000000, 0x8000);

	if(!r) {
		printk(KERN_ERR "failed to remap ids1.\n");
		return ;
	}

	printk(KERN_ERR "ids1 regs:\n");
	for(i = 0; i < ARRAY_SIZE(regs); i++)
		printk(KERN_ERR "0x%08x: 0x%08x\n", 0x23000000 + regs[i],
				readl(r + regs[i]));

	iounmap(r);
}

static struct delayed_work hdmi_hplg_work;
static struct workqueue_struct *hdmi_hplg_queue;
static int hdmi_hplg_index, hdmi_hplg_enabled = 0;
struct clk *hdmi_clk = NULL, *clk_ids1, *clk_ids1_osd, *clk_ids1_eitf;

extern int hdmi_hplg_flow(int en);
static int set_enable(int idsx, int *params, int num)
{
	int enable = *params;
	videoParams_t param_video;
	audioParams_t param_audio;
	productParams_t param_product;
	hdcpParams_t param_hdcp;
	int ret;

	assert_num(1);
	dss_trace("ids%d, enable %d\n", idsx, enable);

	if (!hdmi_state && enable) 
		return -1;

	api_AvMute(!enable);
	if (enable) {	

		hdmi_params_config(vic_config, &param_video, &param_audio, &param_product, &param_hdcp);

#if 0
		msleep(20);
		dump_ids1();
#endif
		ret = api_Configure(&param_video, &param_audio, &param_product, NULL);
		if (ret == 0) {
			dss_err("api_Configure() failed %d\n", ret);
			return -1;
		}

	}	

	return 0;
}

int hdmi_wait_edid(void)
{
	int i;

	if (!hdmi_state) 
		return 1;

	if(api_GetState() < 4) {
		/* No edid */
		if(!api_EdidRead()) {
			dss_err("no EDID.\n");
			return 0;
		}
	}

	for(i = 0; i < 20; i++) {
		if(api_GetState() > 3)
			return 1;

		msleep(50);
	}

	return 0;
}

int hdmi_get_vic(int a[], int len)
{
	dtd_t d;
	int i, j, n, count = 0, pix[] = {2700, 2702, 7425, 7417,
		14850, 14835};
	shortVideoDesc_t svd;

	mutex_lock(&edid_lock);
	if(!hdmi_wait_edid())
		goto out;


	n = api_EdidSvdCount();
	for(i = 0; i < n && i < len; i++) {
		api_EdidSvd(i, &svd);

		/* ignore zero */
		if(!svd.mCode)
			continue;

		/* ignore reduplicated */
		for(j = 0; j < count; j++) {
			if(a[j] == svd.mCode) {
				dss_info("omit the reduplicated vic: %d\n", svd.mCode);
				goto duplicated;
			}
		}

		/* ignore the interlaced ones */
		vic_fill(&d, svd.mCode, 0);
		if(d.mInterlaced)
			continue;

		/* ignore small resolutions */
#if defined(CONFIG_MACH_IMAPX15)
        if(d.mVActive > 720)
            continue;
#else
		if(d.mVActive < 720)
			continue;
#endif

		/* ignore clocks unsupported */
		for(j = 0; j < ARRAY_SIZE(pix); j++)
		{
			if(d.mPixelClock == pix[j]) {
				printk(KERN_ERR "%d: %d (%d)\n", i, svd.mCode, svd.mNative);
				a[count++] = svd.mCode;
				break;
			}
		}

duplicated:
		;
	}

out:
	mutex_unlock(&edid_lock);
	if(count)
		return count;

	/* use default */
	dss_err("failed to get edid: use default 720p\n");
	a[0] = 4;
	a[1] = 0;
	return 1;
}

int hdmi_get_monitor(char *s, int len)
{
	int i;

	mutex_lock(&edid_lock);
	if(len < 64) {
		dss_err("get monitor with buffer longer than 64");
		mutex_unlock(&edid_lock);
		return -1;
	}

	if(!hdmi_wait_edid()) {
		/* use default */
		dss_err("failed to get edid: use default unknown\n");
		mutex_unlock(&edid_lock);
		return sprintf(s, "unknown");
	}

	api_EdidMonitorName(s, len);

	for(i = 0; i < 13; i++) {
		if(s[i] == 10 || s[i] == 13) {
			s[i] = 0;
			break;
		}
	}

	s[i] = 0;

	mutex_unlock(&edid_lock);
	return i;
}

int hdmi_hplg_flow(int en)
{
	static int ret, on = 0;

	/* already in state */
	if(on == !!en)
		return -1;
	else
		on = !!en;

	if(en) {
		/* enable clocks */
		clk_prepare_enable(clk_ids1);
		clk_prepare_enable(clk_ids1_osd);
		clk_prepare_enable(clk_ids1_eitf);
		clk_prepare_enable(hdmi_clk);

		module_power_down(SYSMGR_IDS1_BASE);
		msleep(10);
		module_power_on(SYSMGR_IDS1_BASE);
		ids_writeword(1, 0x5c, 0);
		msleep(10);
		ret = api_Initialize(0, 1, 2500, 1);
		if(ret == 0) {
			dss_err("api_Initialize() failed\n");
			return -1;
		}
	} else {
		
		/* disable clocks */
		if (pt == PRODUCT_MID) {
			clk_disable_unprepare(hdmi_clk);
			clk_disable_unprepare(clk_ids1_eitf);
			clk_disable_unprepare(clk_ids1_osd);
			clk_disable_unprepare(clk_ids1);

			module_power_down(SYSMGR_IDS1_BASE);
		}
	}

	if(item_exist("hdmi.hplg")) {
		hdmi_hpd_event(en);
	}

	return 0;
}

static void hdmi_hplg_work_func(struct work_struct *work)
{
	int in = gpio_get_value(hdmi_hplg_index);

	/* already in state */

	if(in) {
		hdmi_hplg_flow(1);
//		queue_delayed_work(hdmi_hplg_queue, &hdmi_hplg_work, 5 * HZ);
	} else {
		/* hplg-low, shutdown HDMI power dowmain
		 * and wait the next hplg-high.
		 */
		hdmi_hplg_flow(0);

		msleep(10);
		/* enable IRQ */
//		imapx_pad_irq_clear(hdmi_hplg_index);
//		imapx_pad_irq_unmask(hdmi_hplg_index);
//		enable_irq(hdmi_hplg_irq);
	}
}

static irqreturn_t hdmi_hplg_isr(int irq, void *data)
{
	printk(KERN_ERR "%s\n", __func__);
//	disable_irq_nosync(hdmi_hplg_irq);
//	imapx_pad_irq_mask(hdmi_hplg_index);
//	imapx_pad_irq_clear(hdmi_hplg_index);

	queue_delayed_work(hdmi_hplg_queue, &hdmi_hplg_work, 0);
	return IRQ_HANDLED;
}

int hdmi_hplg_init(void)
{
	int ret, irq, index;
	static int inited = 0;

	/* this is only available when the extra hdmi_hplg
	 * is configured
	 */
	if(!item_exist("hdmi.hplg"))
		return -1;

	index = item_integer("hdmi.hplg", 1);
	hdmi_hplg_index = index;
	printk(KERN_DEBUG "hdmi: extra hplg (%d))\n", index);

	if (!inited && gpio_is_valid(hdmi_hplg_index)) {
		ret = gpio_request(hdmi_hplg_index, "hdmi-hplg");
		if (ret) {
			pr_err("failed request gpio for hdmi-hplg\n");
			return -3;
		}
	}
	
	/* set as gpio */
//	imapx_pad_set_mode(1, 1, index);

	/* set as input */
//	imapx_pad_set_dir(1, 1, index);

	gpio_direction_input(index);

	/* disable pulling */
	imapx_pad_set_pull(index, "float");

	/* config the interrupt */
//	imapx_pad_irq_config(index, INTTYPE_HIGH, FILTER_MAX);
//	irq   = imapx_pad_irq_number(index);
	hdmi_hplg_irq = gpio_to_irq(index);

	if(!inited) {
		/* set flag */
		hdmi_hplg_enabled = 1;

		/* init work and queue */
		INIT_DELAYED_WORK(&hdmi_hplg_work, hdmi_hplg_work_func);
		hdmi_hplg_queue = create_singlethread_workqueue("hdmi_hplg");

		ret = request_irq(hdmi_hplg_irq, hdmi_hplg_isr,
					IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
					"hdmi_hplg", hdmi_hplg_queue);

		if(ret < 0) {
			printk(KERN_ERR "failed to request IRQ(%d) for hplg.\n", irq);
			return -2;
		}

		inited = 1;
	}

	/* cancel the original works */
	cancel_delayed_work_sync(&hdmi_hplg_work);
	if (pt == PRODUCT_BOX)
		hdmi_hplg_flow(1);
	if (pt == PRODUCT_MID)
		hdmi_hplg_flow(0);

	queue_delayed_work(hdmi_hplg_queue, &hdmi_hplg_work, 0);

//	imapx_pad_irq_unmask(index);

	/* give a chance to know disconnection */
	if(!gpio_get_value(index)) {
		hdmi_hpd_event(0);
	}

	return 0;
}

int hdmi_irq_init(void)
{
	int ret = 0;

	ret = hdmi_hplg_init();                                 
	if(ret) {                                               
		dss_dbg("hdmi: no extra HPLG (%d)\n", ret); 

		clk_prepare_enable(clk_ids1);                       
		clk_prepare_enable(clk_ids1_osd);                   
		clk_prepare_enable(clk_ids1_eitf);                  
		clk_prepare_enable(hdmi_clk);                       

		module_power_on(SYSMGR_IDS1_BASE);                  
		ids_writeword(1, 0x5c, 0);
		msleep(10);                                         
		ret = api_Initialize(0, 1, 2500, 1);                
		if(ret == 0) {                                      
			dss_err("api_Initialize() failed\n");           
			return -1;                                      
		}                                                   

		if(!api_HotPlugDetected()) {                        
			/* give chance to report disconnect event       
			 *                                              
			 * if HDMI is remove during suspend, no event   
			 * is provided on resume, the upper layer will  
			 * not know the issue and still gives out data. 
			 *                                              
			 * so, let them have a chance to know.          
			 */                                             
			hdmi_hpd_event(0);                              
		}                                                   
	}                                                       

}
EXPORT_SYMBOL(hdmi_irq_init);

static irqreturn_t hdmi_handler(int irq, void *data)
{
	int hpd;


	api_EventHandler(&hpd);
	if (!hpd)
		api_AvMute(hpd);
#if 0
	hpd = api_HotPlugDetected();
	dss_dbg("hpd = %d\n", hpd);
	state_hpd = control_InterruptPhyState(0);
	state_ddc = control_InterruptEdidState(0);
	state_hdcp = hdcp_InterruptStatus(0);
	control_InterruptPhyClear(0, state_hpd);
	control_InterruptEdidClear(0, state_ddc);
	hdcp_InterruptClear(0, state_hdcp);
#endif
	if(!item_exist("hdmi.hplg")) {
		hdmi_hpd_event(hpd);
	}
	
	hdmi_state = hpd;

	return IRQ_HANDLED;
}

static ssize_t
phy_conf0_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%d\n", access_CoreReadByte(0x3000));
}

static ssize_t
phy_conf0_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	return 0;
}

static ssize_t
phy_stat0_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%d\n", access_CoreReadByte(0x3004));
}

static ssize_t
phy_stat0_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	return 0;
}

static ssize_t
mc_phyrstz_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%d\n", access_CoreReadByte(0x4005));
}

static ssize_t
mc_phyrstz_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	return 0;
}

static struct kobj_attribute mc_phyrstz_state = 
	__ATTR(mc_phyrstz , 0644, mc_phyrstz_show, mc_phyrstz_store);
static struct kobj_attribute phy_conf0_state = 
	__ATTR(phy_conf0, 0644, phy_conf0_show, phy_conf0_store);
static struct kobj_attribute phy_stat0_state = 
	__ATTR(phy_stat0, 0644, phy_stat0_show, phy_stat0_store);

const struct attribute *hdmi_state_attributes[] = {
	&mc_phyrstz_state.attr,
	&phy_conf0_state.attr,
	&phy_stat0_state.attr,
	NULL,
};


static struct kobject *hdmi_phy_state_kobj;

static int dss_init_module(int idsx)
{
	static int ret, inited;
	unsigned char * addr;
	
	dss_trace("ids%d\n", idsx);

	access_Initialize((unsigned char *)IMAP_HDMI_REG_BASE);

	dss_dbg("Dealing with IIS_SPDIF_CLK\n");
	addr = (unsigned char *) ioremap_nocache(SYS_HDMI_IIS_SPDIF_CLK_BASE, 0x100);
	addr[0x00] = 0x03;
	iounmap(addr);

	if (imapx_pad_init("hdmi") == -1) {
		dss_err("imapx_pad_init(hdmi) failed\n");
		return -1;
	}

	if(!inited) {
		hdmi_clk = clk_get_sys("imap-hdmi", "imap-hdmi");
		if (!hdmi_clk) {
			dss_err("clk_get(hdmi) failed\n");
			return -1;
		}

		clk_ids1 = clk_get_sys("imap-ids.1", "imap-ids1");
		if (!clk_ids1) {
			dss_err("clk_get(ids.1) failed\n");
			return -1;
		}

		clk_ids1_osd = clk_get_sys("imap-ids1-ods", "imap-ids1");
		if (!clk_ids1_osd) {
			dss_err("clk_get(ids1_osd) failed\n");
			return -1;
		}

		clk_ids1_eitf = clk_get_sys("imap-ids1-eitf", "imap-ids1");
		if (!clk_ids1_eitf) {
			dss_err("clk_get(ids1_eitf) failed\n");
			return -1;
		}


		hdmi_phy_state_kobj = kobject_create_and_add("imapx_hdmi_state",
				kernel_kobj);              
		if (!hdmi_phy_state_kobj) {                                         
			pr_err("hdmi_state: \                            
					failed to create sysfs object");                   
				return -ENOMEM;                                        
		}                                                              

		if (sysfs_create_files(hdmi_phy_state_kobj, hdmi_state_attributes)) {
			pr_err("hdmi_state: \                            
					failed to create sysfs interface");                
				return -ENOMEM;                                            
		}
		mutex_init(&edid_lock);
	}

#ifndef CONFIG_IMAPX15_G2D 
	ret = hdmi_hplg_init();
	if(ret) {
		dss_dbg("hdmi: no extra HPLG (%d)\n", ret);

		clk_prepare_enable(clk_ids1);
		clk_prepare_enable(clk_ids1_osd);
		clk_prepare_enable(clk_ids1_eitf);
		clk_prepare_enable(hdmi_clk);

		module_power_on(SYSMGR_IDS1_BASE);
		ids_writeword(1, 0x5c, 0);
		msleep(10);
		ret = api_Initialize(0, 1, 2500, 1);
		if(ret == 0) {
			dss_err("api_Initialize() failed\n");
			return -1;
		}

		if(!api_HotPlugDetected()) {
			/* give chance to report disconnect event
			 *
			 * if HDMI is remove during suspend, no event
			 * is provided on resume, the upper layer will
			 * not know the issue and still gives out data.
			 *
			 * so, let them have a chance to know.
			 */
			hdmi_hpd_event(0);
		}
	}
#endif

	if (!inited) {
		ret =request_irq(97, hdmi_handler, 
				IRQF_DISABLED |IRQF_TRIGGER_HIGH,
				"hdmi_irq", NULL);
		if (ret) {
			dss_err("Failed to request irq %d\n", 97);
			return -1;
		}
		inited = 1;
	}

	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display hdmi controller driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");

