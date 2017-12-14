/*
 * mipi_dsi.c - display mipi dsi controller driver
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
#include <linux/clk.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <dss_common.h>
#include <ids_access.h>
#include <mach/pad.h>
#include <mach/imap-iomap.h>
#include <mach/power-gate.h>
#include <mipi_dsih_api.h>
#include <mipi_dsih_hal.h>
#include <mipi_dsih_dphy.h>


#define DSS_LAYER	"[dss]"
#define DSS_SUB1	"[interface]"
#define DSS_SUB2	"[mipi dsi]"

#define MIPI_DSI_BASE_ADDR			0x22040000
#define MODULE_DSI_REG_SIZE		0x1000
#define DSI_HOST_BASE_ADDRESS		MIPI_DSI_BASE_ADDR
#define DPHY_BASE_ADDRESS			DSI_HOST_BASE_ADDRESS

extern int lcdc_output_en(int idsx, int enable);
extern int lcd_default_enable(int enable);
static int set_enable(int idsx, int *params, int num);
static int set_vic(int idsx, int *params, int num);
static int dss_init_module(int idsx);

dsih_ctrl_t *instance = NULL;
dphy_t phy;
dsih_dpi_video_t *video = NULL;
unsigned int * dsi_regbase;
static int first = 1;
struct delayed_work wk;
struct workqueue_struct *wq;



static struct module_attr mipi_dsi_attr[] = {
	{TRUE, ATTR_ENABLE, set_enable},
	{TRUE, ATTR_VIC, set_vic},
	{FALSE, ATTR_END, NULL},
};

struct module_node mipi_dsi_module = {
		.name = "mipi_dsi",
		.attributes = mipi_dsi_attr,
		.init = dss_init_module,	
};

int dsilib_send_short_packet(unsigned int vc, unsigned char *cmd_buffer, unsigned int length)
{
	dsih_error_t error_code;
	dss_dbg("vc = %d , length=%d", vc, length);
	
	if (!cmd_buffer || length != 2){
		dss_err("params error");
		return -1;
	}

	error_code = mipi_dsih_gen_wr_cmd(instance, vc , 0x05, cmd_buffer, 2);
	if(error_code) {
		return -1;
	}
	return 0;
}

static int set_vic(int idsx, int *params, int num)
{
	int vic = *params;
	dtd_t dtd;
	unsigned int clkfreq;
	int ret;
	char buf[32] = {0};
	uint32_t status_1;
	int i;
	
	assert_num(1);
	dss_trace("ids%d, vic %d\n", idsx, vic);

	vic_fill(&dtd, vic, 60000);

	//clkfreq = 64843200;
	//clkfreq = 66000000;
	clkfreq = 49500000;
	
	video->no_of_lanes = 4;
	video->virtual_channel = 0;
	video->color_coding = COLOR_CODE_24BIT;
	video->byte_clock = clkfreq/1000; /* KHz  */
	video->video_mode = VIDEO_BURST_WITH_SYNC_PULSES;	// ?
	video->receive_ack_packets = 1;		/* enable receiving of ack packets */
	video->pixel_clock = clkfreq/1000;   /* dpi_clock*/
	video->is_18_loosely = 1;
	video->data_en_polarity = 1;
	video->h_polarity = dtd.mHSyncPolarity;
	video->h_active_pixels = dtd.mHActive;
	video->h_sync_pixels = dtd.mHSyncPulseWidth; /* min 4 pixels */
	video->h_back_porch_pixels = dtd.mHBlanking - dtd.mHSyncPulseWidth - dtd.mHSyncOffset;
	video->h_total_pixels = dtd.mHActive + dtd.mHBlanking;
	video->v_active_lines = dtd.mVActive;
	video->v_polarity = dtd.mVSyncPolarity;
	video->v_sync_lines = dtd.mVSyncPulseWidth;
	video->v_back_porch_lines = dtd.mVBlanking - dtd.mVSyncPulseWidth - dtd.mVSyncOffset;
	video->v_total_lines = dtd.mVActive + dtd.mVBlanking;

	ret = mipi_dsih_open(instance);
	if (ret) {
		dss_err("mipi dsih open failed %d\n", ret);
		return -1;
	}

	ret = mipi_dsih_dpi_video(instance, video);
	if (ret) {
		dss_err("mipi dsih dpi video failed %d\n", ret);
		return -1;
	}

	mipi_dsih_shutdown_controller(instance, 0);
	mipi_dsih_dphy_open(&phy);

	mipi_dsih_dphy_configure(&phy, video->no_of_lanes, video->byte_clock * 8);

	// case POWER ON
	lcd_default_enable(1);
	msleep(100);


	// case CMD ON	
	status_1 = mipi_dsih_hal_error_status_1(instance, 0xffffffff);
	dss_dbg("Status_1 = 0x%X\n", status_1);

	// io reset
#if 1
	imapx_pad_set_mode(1, 1, 72);/*gpio*/
	imapx_pad_set_dir(0, 1, 72);/*output*/
	imapx_pad_set_outdat(0, 1, 72);/*output 0*/
	msleep(10);
	imapx_pad_set_outdat(1, 1, 72);/*output 1*/
	msleep(7);
#endif

	mipi_dsih_dphy_if_control(&phy, 1);

	buf[0] = 0x11;	// exit_sleep_mode
	buf[1] = 0;
	dsilib_send_short_packet(0, buf, 2);
	for(i = 1; i < 10; i ++){
		status_1 = mipi_dsih_hal_error_status_1(instance, 0xffffffff);
		printk("sam : sleep out val (0x48) = 0x%x \n",status_1);
		msleep(2);
	}
	msleep(10);

	buf[0] = 0x29;	// set_display_on
	buf[1] = 0;
	dsilib_send_short_packet(0, buf, 2);
	for(i = 1; i < 10; i ++){
		status_1 = mipi_dsih_hal_error_status_1(instance, 0xffffffff);
		printk("sam : sleep out val (0x48) = 0x%x \n",status_1);
		msleep(2);
	}
	msleep(10);

	// case RGB
	mipi_dsih_dphy_if_control(&phy, 1);
	msleep(1);
	// video mode
	if (!mipi_dsih_hal_gen_is_video_mode(instance))
	{
		mipi_dsih_hal_gen_cmd_mode_en(instance, 0);
		mipi_dsih_hal_dpi_video_mode_en(instance, 1);
	}
	// output rgb data
	lcdc_output_en(0, 1);
	msleep(50);


	
//##########################################################


	return 0;
}

static int set_enable(int idsx, int *params, int num)
{
	int enable = *params;
	assert_num(1);
	dss_trace("ids%d, enable %d\n", idsx, enable);

	return 0;
}

static void mipi_dsi_work(struct work_struct *work)
{
	uint32_t status_0 = mipi_dsih_hal_error_status_0(instance, 0xffffffff);
	uint32_t status_1 = mipi_dsih_hal_error_status_1(instance, 0xffffffff);
	if (status_0 || status_1) {
		dss_dbg("irq status0: 0x%X, status1: 0x%X\n", status_0, status_1);
		mipi_dsih_dump_register_configuration(instance, 1, NULL, 0);
	}
	queue_delayed_work(wq, &wk, HZ*10);
}

void mipi_dsi_event(void * type)
{
	dss_err("#### Interrupt #### mipi dsi interrupt type: %d", *(s32 *)type);
}


static irqreturn_t mipi_dsi_irq_handler(int irq, void *data)
{
	uint32_t status_0 = mipi_dsih_hal_error_status_0(instance, 0xffffffff);
	uint32_t status_1 = mipi_dsih_hal_error_status_1(instance, 0xffffffff);
	dss_dbg("irq status0: 0x%X, status1: 0x%X\n", status_0, status_1);
	//mipi_dsih_event_handler(instance);

	if (first) {		//test
		first = 0;
		mipi_dsih_dump_register_configuration(instance, 1, NULL, 0);
	}
	
	return IRQ_HANDLED;
}

void io_bus_write32(uint32_t base_address, uint32_t offset, uint32_t  data)
{
	/* printf("%lx: %lx\n", offset, data); */ /* D E B U G */
	*(volatile u32 *)((u32)dsi_regbase + base_address + offset) = data;
}

uint32_t io_bus_read32(uint32_t base_address, uint32_t  offset)
{
	return *(volatile u32 *)((u32)dsi_regbase + base_address + offset);	
}

void log_error(const char * message)
{
	printk("[MIPI DSI ERROR]: %s\n", message);
}

void log_notice(const char *fmt, ...)
{
	va_list args;
	
	printk("[MIPI DSI NOTICE]: ");
	va_start(args, fmt);
	vprintk(fmt, args);
	va_end(args);
	printk("\n");
}

static int dss_init_module(int idsx)
{
	int ret;
	//int i;
	unsigned char tmp;
	unsigned char * addr;
	struct clk *clksrc;
	dss_trace("ids%d\n", idsx);

	module_power_on(SYSMGR_MIPI_BASE);
#if 0
	addr = (unsigned char *) ioremap_nocache(0x21E24000 , 0x100);
	tmp = addr[0x00];
	addr[0x00] = tmp | 0x80;
	tmp = addr[0x04];
	addr[0x04] = tmp | 0x0C;
	addr[0x1C] = 0x07;
	addr[0x28] = 0x00;
	iounmap(addr);
#else
	addr = (unsigned char *) ioremap_nocache(0x21E2401C , 0x100);
	addr[0x00] = 0x07;
	iounmap(addr);
	addr = (unsigned char *) ioremap_nocache(0x21E24000 , 0x100);
	tmp = addr[0x00];
	addr[0x00] = tmp | 0x08;
	iounmap(addr);
#endif
	dsi_regbase = (unsigned int *)ioremap_nocache(MIPI_DSI_BASE_ADDR, MODULE_DSI_REG_SIZE);
	if(!dsi_regbase){
		dss_err("ioremap(DSI) regBaseVir failed");
		return -1;
	}
	dss_dbg("mipi dsi base addr ioremap 0x%X\n", (u32)dsi_regbase);

	clksrc = clk_get_sys("imap-mipi-dphy-con", "imap-mipi-dsi");
	if (clksrc == NULL) {
		dss_err("clk_get(mipi-dphy-con) failed");
		return -1;
	}
	clk_prepare_enable(clksrc);
	ret = clk_set_rate(clksrc,27000);
	if (ret < 0){
		dss_err(" clk_set_rate(mipi-dphy-con) failed");
		return -1;
	}
	dss_info("mipi-dphy-con actual clk is %ld\n", clk_get_rate(clksrc));

	clksrc = clk_get_sys("imap-mipi-dphy-ref","imap-mipi-dsi");
	if (clksrc == NULL) {
		dss_err("clk_get(mipi-dphy-ref) failed");
		return -1;
	}
	clk_prepare_enable(clksrc);
	ret = clk_set_rate(clksrc,27000);
	if (ret < 0){
		dss_err(" clk_set_rate(mipi-dphy-ref) failed");
		return -1;
	}
	dss_info("mipi-dphy-ref actual clk is %ld\n", clk_get_rate(clksrc));

	instance = (dsih_ctrl_t*)kzalloc(sizeof(dsih_ctrl_t), GFP_KERNEL);
	if (!instance) {
		dss_err("kzalloc instance failed.\n");
		return -1;
	}
	
	phy.address = 0;
	phy.reference_freq = 27844; /*KHz*/
	phy.status = NOT_INITIALIZED;
	phy.core_read_function = io_bus_read32;
	phy.core_write_function = io_bus_write32;
	phy.log_error = log_error;
	phy.log_info = log_notice;

	instance->address = 0;
	instance->phy_instance = phy;
	instance->max_lanes = 4; /* DWC MIPI D-PHY Bidir TSMC40LP has only 2 lanes */
	instance->color_mode_polarity = 1;
	instance->shut_down_polarity = 1;
	instance->max_hs_to_lp_cycles = 100;
	instance->max_lp_to_hs_cycles = 40;
	instance->max_bta_cycles = 4095;
	instance->status = NOT_INITIALIZED;
	instance->core_read_function = io_bus_read32;
	instance->core_write_function = io_bus_write32;
	instance->log_error = log_error;
	instance->log_info = log_notice;

	video = (dsih_dpi_video_t*)kzalloc(sizeof(dsih_dpi_video_t), GFP_KERNEL);
	if (!video) {
		dss_err("kzalloc video failed\n");
		return -1;
	}
#if 0
	//for (i = 0; i < DUMMY; i++)
	//	mipi_dsih_register_event(instance, i, mipi_dsi_event);

	mipi_dsih_hal_error_mask_0(instance, 0xFFFFFFFF);
	mipi_dsih_hal_error_mask_1(instance, 0xFFFFFFFF);

	ret = request_irq(81, mipi_dsi_irq_handler, IRQF_DISABLED, "mipi_dsi", NULL);	// IRQF_ONESHOT ?
	if (ret) {
		dss_err("request irq failed %d\n", ret);
		return -1;
	}
#endif

	wq = create_singlethread_workqueue("dsi_wq");
	if (!wq) {
		dss_err("create_singlethread_workqueue() failed\n");
		return -1;
	}

	INIT_DELAYED_WORK(&wk, mipi_dsi_work);
	queue_delayed_work(wq, &wk, HZ*10);
	
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display lcd controller driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
