/*
 * osd_x15.c - x15 display osd driver
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
#include <linux/delay.h>
#include <dss_common.h>
#include <implementation.h>
#include <ids_access.h>
#include <controller.h>


#define DSS_LAYER	"[implementation]"
#define DSS_SUB1	"[osd]"
#define DSS_SUB2	""




#define PRE_OVCDCR_ENREFETCH		0x1		// 0x1: Enable
#define PRE_OVCDCR_WAITTIME		0xF		// 0x0 ~ 0xF


extern int pt;

static int set_enable(int idsx, int *params, int num);
static int set_vic(int idsx, int *params, int num);
static int set_position(int idsx, int *params, int num);
static int set_multiple_layer(int idsx, int *params, int num);
static int set_rgb_bpp(int idsx, int *params, int num);
static int set_dma_addr(int idsx, int *params, int num);
static int set_swap(int idsx, int *params, int num);
static int dss_init_module(int idsx);



//static dss_limit osd_limitation[] = {
//	osd_bandwidth_limitation,
//	NULL,
//};

/* FALSE stand for optional attribute, and default disable */
static struct module_attr osd_attr[] = {
	{TRUE, ATTR_ENABLE, set_enable},
	{TRUE, ATTR_VIC, set_vic},
	//{TRUE, ATTR_POSITION, set_position},
	//{FALSE, ATTR_MULTI_LAYER, set_multiple_layer},
	//{TRUE, ATTR_RGB_BPP, set_rgb_bpp},
	//{TRUE, ATTR_DMA_ADDR, set_dma_addr},
	//{TRUE, ATTR_SWAP, set_swap},
	{FALSE, ATTR_END, NULL},
};

struct module_node osd_module[2] = {
	{
		.name = "osd",
		//.limitation = osd_limitation,
		.attr_ctrl = module_attr_ctrl,
		.attributes = osd_attr,
		.idsx = 0,
		.init = dss_init_module,
	},
	{
		.name = "osd",
		//.limitation = osd_limitation,
		.attr_ctrl = module_attr_ctrl,
		.attributes = osd_attr,
		.idsx = 1,
		.init = dss_init_module,
	},
};


/* Most [2] stand for ids0 and ids1 */
static dtd_t dtd[2];
static int scale_factor[2] = {100, 100};
static int width[2];
static int height[2];
static int center_x[2];
static int center_y[2];
/* (center point xoffset) /mHActive, respectively. dedicated use. */
static int ten_thousandths_x[2]= {5000, 5000};
static int ten_thousandths_y[2] = {5000, 5000};
static int xoffset[2];
static int yoffset[2];
static int multiple_layer[2] = {0};
static int osd_enable[2] = {0};
static dma_addr_t osd3_dma_addr[2][2];		/* [idsx][fbx] */
/* Refetch strategy */
static unsigned int enrefetch[2] = {PRE_OVCDCR_ENREFETCH, PRE_OVCDCR_ENREFETCH};
static unsigned int waittime[2] = {PRE_OVCDCR_WAITTIME, PRE_OVCDCR_WAITTIME};
/* Multiple layer edge */
static int osd_xedge[2][4][2] = {{{0}}};		/* [idsx][layer][xstart, xend] */


extern unsigned int main_VIC;

static dma_addr_t gdma_addr;
static int gframe_size;
static int logo_fbx;
static int fbx;

static int set_enable(int idsx, int *params, int num)
{
	int enable = *params;
	assert_num(1);
	dss_trace("ids%d, enable %d\n", idsx, enable);

	ids_write(idsx, OVCW0CR, OVCWxCR_ENWIN, 1, enable);

	return 0;
}

// Temp for lighten lcd
int osd_swap(int fbx)
{
	int idsx;
	                            
	if (pt == PRODUCT_MID)
		idsx = 0;
	else if (pt == PRODUCT_BOX)
		idsx = 1;

	return set_swap(idsx, &fbx, 1);
}

// Temp for lighten lcd
int osd_dma_addr(dma_addr_t addr, int frame_size, int fbx)
{
	int params[2] = {(s32)addr, frame_size};
	gdma_addr = addr;
	gframe_size = frame_size;
	logo_fbx = fbx;
	//set_dma_addr(idsx, params, 2);
	//ids_write(idsx, OVCW0CR, OVCWxCR_BUFSEL, 2, fbx);
	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
	//ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	return 0;
}

int swap_cvbs_buffer(u32 phy_addr)
{
	if (fbx == 0)
		ids_writeword(0, OVCW0B0SAR, phy_addr);
	else
		ids_writeword(0, OVCW0B1SAR, phy_addr);
	ids_write(0, OVCW0CR, OVCWxCR_BUFSEL, 2, fbx);
	ids_write(0, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	fbx = !fbx;
	wait_swap_timeout(0, IRQ_LCDINT, TIME_SWAP_INT);
	return 0;
}


int swap_hdmi_buffer(u32 phy_addr)
{
	if (fbx == 0)
		ids_writeword(1, OVCW0B0SAR, phy_addr);
	else
		ids_writeword(1, OVCW0B1SAR, phy_addr);
	ids_write(1, OVCW0CR, OVCWxCR_BUFSEL, 2, fbx);
	ids_write(1, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	fbx = !fbx;
	wait_swap_timeout(1, IRQ_LCDINT, TIME_SWAP_INT);
	return 0;
}

static int set_swap(int idsx, int *params, int num)
{
	int fbx = *params;

	assert_num(1);
	//dss_trace("ids%d, fbx %d\n", idsx, fbx);
	
	ids_write(idsx, OVCW0CR, OVCWxCR_BUFSEL, 2, fbx);
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	wait_swap_timeout(idsx, IRQ_LCDINT, TIME_SWAP_INT);
	
	return 0;
}

static int set_dma_addr(int idsx, int *params, int num)
{
	unsigned int addr = (u32)params[0];
	int frame_size = params[1];
	int i;	
	unsigned int ovcwxb0sar[4] = {OVCW0B0SAR, OVCW1B0SAR, OVCW2B0SAR};
	unsigned int ovcwxb1sar[4] = {OVCW0B1SAR, OVCW1B1SAR, OVCW2B1SAR};
	assert_num(2);
	dss_trace("ids%d, dma addr 0x%x, frame size %d\n", idsx, (u32)addr, frame_size);

	for (i = 0; i < 3; i++) {
		ids_writeword(idsx, ovcwxb0sar[i], addr + osd_xedge[idsx][i][0]);
		ids_writeword(idsx, ovcwxb1sar[i], addr + frame_size + osd_xedge[idsx][i][0]);
	}
	osd3_dma_addr[idsx][0] = addr + osd_xedge[idsx][3][0];
	osd3_dma_addr[idsx][1] = addr + frame_size + osd_xedge[idsx][3][0];

	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);
	
	return 0;
}


static int set_vic(int idsx, int *params, int num)
{
	int vic = *params;
	int tx, ty;
	int pa[2] = {(s32)gdma_addr, gframe_size};
	int ret, width, height, xoffset = 0, yoffset = 0, seq, factor = 100, bpp_mode;
	int scale_w;
	int scale_h;
	int scaled_width;
	int scaled_height;	
	dtd_t dtd;
	dtd_t pre_dtd;
	
	assert_num(1);
	dss_trace("ids%d, vic %d\n", idsx, vic);


	ret = vic_fill(&dtd, vic, 60000);
	if (ret != 0) {
		dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
		return ret;
	}	
	width = dtd.mHActive & (~0x3);
	height = dtd.mVActive & (~0x3);	
	if ((dtd.mHActive - width) % 4  || (dtd.mVActive - height) % 4) {
		dss_err("width = %d, height = %d, not align with 4. Program Mistake.\n", width, height);
		return -1;
	}
	dss_dbg("width = %d, height = %d, xoffset = %d, yoffset = %d\n", width, height, xoffset, yoffset);

	bpp_mode = 11;

	//ids_write(idsx, ENH_IECR, ENH_IECR_ACMPASSBY, 1 , 1); // Disable

	/* Set Overlay0 */
    ids_write(idsx, OVCW0CR, OVCWxCR_BUFAUTOEN, 1, 0);	// Disable Auto FB
    if(idsx ==1){
        ids_write(idsx, OVCW0CR, OVCWxCR_RBEXG, 1, 1); // due to Mali only support 0BGR888,R and B channel exchange workaround for CTS test
    }
    ids_write(idsx, OVCW0CR, OVCWxCR_BPPMODE, 5, bpp_mode);
    if ((pt == PRODUCT_BOX) && (idsx == 1)) {
        ret = vic_fill(&pre_dtd, main_VIC, 60000);
        if (ret != 0) {
            dss_err("Failed to fill dtd VIC = %d, ret = %d\n", vic, ret);
            return ret;
        }	
        ids_writeword(idsx, OVCW0VSSR, pre_dtd.mHActive);
        ids_writeword(idsx, OVCW0PCAR, 
                ((0) << OVCWxPCAR_LeftTopX) | ((0) << OVCWxPCAR_LeftTopY));
        ids_writeword(idsx, OVCW0PCBR,
                ((pre_dtd.mHActive-1) << OVCWxPCBR_RightBotX) | ((pre_dtd.mVActive-1) << OVCWxPCBR_RightBotY));	
    } else {
        ids_writeword(idsx, OVCW0VSSR, width);
        ids_writeword(idsx, OVCW0PCAR, 
                ((0) << OVCWxPCAR_LeftTopX) | ((0) << OVCWxPCAR_LeftTopY));
        ids_writeword(idsx, OVCW0PCBR,
                ((width-1) << OVCWxPCBR_RightBotX) | ((height-1) << OVCWxPCBR_RightBotY));	
    }

    /* Set DMA address */
    if ((idsx == 0) && (pt == PRODUCT_MID)) {
		ids_writeword(idsx, OVCW0B0SAR, gdma_addr);
		ids_writeword(idsx, OVCW0B1SAR, gdma_addr + gframe_size);
        ids_writeword(idsx, OVCW0B2SAR, gdma_addr + 2 * gframe_size);
		ids_write(idsx, OVCW0CR, OVCWxCR_BUFSEL, 2, logo_fbx);
		dss_dbg("ids%d, dma_addr[0~1] = 0x%X, 0x%X\n", idsx, gdma_addr, gdma_addr + gframe_size);
	} else if ((idsx == 1) && (pt == PRODUCT_BOX)) {
		ids_writeword(idsx, OVCW0B0SAR, gdma_addr);
		ids_writeword(idsx, OVCW0B1SAR, gdma_addr + gframe_size);
        ids_writeword(idsx, OVCW0B2SAR, gdma_addr + 2 * gframe_size);
		ids_write(idsx, OVCW0CR, OVCWxCR_BUFSEL, 2, logo_fbx);
		dss_dbg("ids%d, dma_addr[0~1] = 0x%X, 0x%X\n", idsx, gdma_addr, gdma_addr + gframe_size);
	}
	else {
		dss_dbg("ids%d, dma_addr[0~1] = 0x%X, 0x%X\n", 
				idsx, ids_readword(idsx, OVCW0B0SAR), ids_readword(idsx, OVCW0B1SAR));
	}

	/*
	if (idsx == 0) {
		ids_writeword(idsx, OVCW0B0SAR, gdma_addr);
		ids_writeword(idsx, OVCW0B1SAR, gdma_addr + gframe_size);
        ids_writeword(idsx, OVCW0B2SAR, gdma_addr + 2 * gframe_size);
		ids_write(idsx, OVCW0CR, OVCWxCR_BUFSEL, 2, logo_fbx);
		dss_dbg("ids%d, dma_addr[0~1] = 0x%X, 0x%X\n", idsx, gdma_addr, gdma_addr + gframe_size);
	}
	*/

	/* LCD ReFetch Strategy */
	ids_write(idsx, OVCDCR, 9, 1, PRE_OVCDCR_ENREFETCH);
	ids_write(idsx, OVCDCR, 4, 4, PRE_OVCDCR_WAITTIME);

	// FPGA X9 tune
	if (idsx == 1) {
		if ((pt == PRODUCT_BOX)) {
			ret = vic_fill(&pre_dtd, main_VIC, 60000);
			if (ret != 0) {
				dss_err("Failed to fill dtd VIC = %d, ret = %d\n", main_VIC, ret);
				return ret;
			}

			scaled_width = dtd.mHActive & (~0x3);
			scaled_height = dtd.mVActive & (~0x3);	
			if ((dtd.mHActive - scaled_width) % 4  || (dtd.mVActive - scaled_height) % 4) {
				dss_err("width = %d, height = %d, not align with 4. Program Mistake.\n", scaled_width, scaled_height);
				return -1;
			}

			ids_write(1, OVCDCR, OVCDCR_UpdateReg, 1, 0);
			ids_write(1, OVCDCR, 10, 1, 0);
			ids_write(1, OVCDCR, 22, 1, 0);
			ids_writeword(1, 0x100C, (pre_dtd.mVActive-1) << 12 | (pre_dtd.mHActive-1));
			ids_writeword(1, 0x5100, (pre_dtd.mVActive-1) << 16 | (pre_dtd.mHActive-1));
			ids_writeword(1, 0x5104, (scaled_height-1) << 16 | (scaled_width-1));
			ids_writeword(1, 0x5108, (0) << 16 | (0));

			scale_w = pre_dtd.mHActive*1024/dtd.mHActive;
			scale_h = pre_dtd.mVActive*1024/dtd.mVActive;

			ids_writeword(1, 0x510C, (scale_h<< 16) | scale_w | (0<<30) | (0<<14));

		} else {
			ids_writeword(idsx, 0x100C, (height-1) << 12 | (width-1));
			ids_writeword(idsx, 0x5100, (height-1) << 16 | (width-1));
			ids_writeword(idsx, 0x5104, (height-1) << 16 | (width-1));
			ids_writeword(idsx, 0x510C, (0x400<< 16) | 0x400 | (1<<30) | (1<<14));
			ids_write(idsx, OVCDCR, 22, 1, 1);
		}
	}

	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	/* Disable Overlay 1~3 */
	ids_write(idsx, OVCW1CR, OVCWxCR_ENWIN, 1, 0);
	ids_write(idsx, OVCW2CR, OVCWxCR_ENWIN, 1, 0);
	ids_write(idsx, OVCW3CR, OVCWxCR_ENWIN, 1, 0);

	/* Enabe Overlay 0 */
	ids_write(idsx, OVCW0CR, OVCWxCR_ENWIN, 1, 1);

	/* Update Overlay params. During dynamic changing, must update by write 1 to this bit */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	return 0;
}



/* ids module must be power on and clk on, before any register operation */
static int dss_init_module(int idsx)
{
	dss_trace("ids%d\n", idsx);

	/* Disable Auto FB */
	ids_write(idsx, OVCW0CR, OVCWxCR_BUFAUTOEN, 1, 0);
	ids_write(idsx, OVCW1CR, OVCWxCR_BUFAUTOEN, 1, 0);
	ids_write(idsx, OVCW2CR, OVCWxCR_BUFAUTOEN, 1, 0);
	ids_write(idsx, OVCW3CR, OVCWxCR_BUFAUTOEN, 1, 0);
	/* Set alpha */
	ids_writeword(idsx, OVCW1PCCR, 0xFFFFFF);
	ids_writeword(idsx, OVCW2PCCR, 0xFFFFFF);
	ids_writeword(idsx, OVCW3PCCR, 0xFFFFFF);
	/* Refetch strategy */
	dss_dbg("enrefetch[%d] = %d, waittime[%d] = %d\n", idsx, enrefetch[idsx], idsx, waittime[idsx]);
	ids_write(idsx, OVCDCR, 9, 1, enrefetch[idsx]);
	ids_write(idsx, OVCDCR, 4, 4, waittime[idsx]);
	/* Disable osd1~osd3 */
	ids_write(idsx, OVCW1CR, OVCWxCR_ENWIN, 1, 0);
	ids_write(idsx, OVCW2CR, OVCWxCR_ENWIN, 1, 0);
	ids_write(idsx, OVCW3CR, OVCWxCR_ENWIN, 1, 0);
	/* Update osd params */
	ids_write(idsx, OVCDCR, OVCDCR_UpdateReg, 1, 1);

	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAPx15 display osd driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
>>>>>>> 91c23f2...  RM#NONE (1)Uboot logo support
