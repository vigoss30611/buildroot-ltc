/*!
******************************************************************************
 @file   : ci_hw_awb.c

 @brief	 awb entry src file

 @Author Fred.Jiang Infotm

 @date   2016.12.14

 <b>Description:</b>\n


 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/


#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/io.h>

#include <img_defs.h>
#include <img_errors.h>
#include <img_types.h>
#include <ci_internal/sys_device.h>

#include <ci/ci_hw_awb.h>
#include <registers/hw_awb_reg.h>


#ifdef IMG_KERNEL_MODULE

#include <asm/io.h>
#include <img_types.h>
#endif



#if defined(IMG_KERNEL_MODULE)
void __iomem * g_ioAwbRegVirtBaseAddr;
#endif //IMG_KERNEL_MODULE



#ifdef INFOTM_HW_AWB_METHOD

#define AWB_INT_ID	(81)				// AWB Interrupt.



#if defined(IMG_KERNEL_MODULE)
#define AWB_READL(RegAddr)	readl(g_ioAwbRegVirtBaseAddr + RegAddr)
#define AWB_WRITEL(RegData, RegAddr)	writel(RegData, g_ioAwbRegVirtBaseAddr + RegAddr)

#else

#define AWB_READL(RegAddr)	readl(RegAddr)
#define AWB_WRITEL(RegData, RegAddr)	writel(RegData, RegAddr)
#endif

static const AWB_UINT32 HW_AWB_WEIGHT_TABLE[13][13] =	{
#if 0
    //  0 0.25  0.5 0.75  1.0 1.25  1.5 1.75  2.0 2.25  2.5 2.75  3.0
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // 0
    {   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 0.25
    {   0,   1,   2,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4 }, // 0.5
    {   0,   1,   2,   3,   4,   4,   8,   8,   8,   8,   7,   4,   4 }, // 0.75
    {   0,   1,   2,   3,   4,   8,   8,   9,   9,   8,   7,   4,   2 }, // 1.0
    {   0,   1,   2,   3,   4,   8,   9,   9,   8,   8,   3,   4,   2 }, // 1.25
    {   0,   1,   3,   4,   8,   9,   9,   9,   8,   8,   4,   3,   0 }, // 1.5
    {   0,   1,   3,   4,   8,   9,   9,   9,   8,   4,   3,   3,   0 }, // 1.75
    {   0,   1,   4,   7,   8,   9,   9,   8,   8,   4,   3,   3,   0 }, // 2.0
    {   0,   1,   4,   7,   8,   8,   8,   8,   4,   3,   3,   3,   0 }, // 2.25
    {   0,   1,   4,   7,   7,   7,   7,   5,   4,   3,   3,   0,   0 }, // 2.5
    {   0,   1,   4,   4,   4,   3,   3,   3,   3,   2,   0,   0,   0 }, // 2.75
    {   0,   1,   4,   4,   4,   2,   2,   2,   0,   0,   0,   0,   0 }  // 3.0
#elif 0
    //  0 0.25  0.5 0.75  1.0 1.25  1.5 1.75  2.0 2.25  2.5 2.75  3.0
    {   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 0
    {   0,   1,   2,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4 }, // 0.25
    {   0,   1,   2,   3,   4,   4,   8,   8,   8,   8,   7,   4,   4 }, // 0.5
    {   0,   1,   2,   3,   4,   8,   8,   9,   9,   8,   7,   4,   2 }, // 0.75
    {   0,   1,   2,   3,   4,   8,   9,   9,   8,   8,   3,   4,   2 }, // 1.0
    {   0,   1,   3,   4,   8,   9,   9,   9,   8,   8,   4,   3,   0 }, // 1.25
    {   0,   1,   3,   4,   8,   9,   9,   9,   8,   4,   3,   3,   0 }, // 1.5
    {   0,   1,   4,   7,   8,   9,   9,   8,   8,   4,   3,   3,   0 }, // 1.75
    {   0,   1,   4,   7,   8,   8,   8,   8,   4,   3,   3,   3,   0 }, // 2.0
    {   0,   1,   4,   7,   7,   7,   7,   5,   4,   3,   3,   0,   0 }, // 2.25
    {   0,   1,   4,   4,   4,   3,   3,   3,   3,   2,   0,   0,   0 }, // 2.5
    {   0,   1,   4,   4,   4,   2,   2,   2,   0,   0,   0,   0,   0 }, // 2.75
    {   0,   1,   4,   4,   2,   2,   2,   0,   0,   0,   0,   0,   0 }  // 3.0
#elif 0
    //  0 0.25  0.5 0.75  1.0 1.25  1.5 1.75  2.0 2.25  2.5 2.75  3.0
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // 0
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // 0.25
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // 0.5
    {   0,   0,   0,   3,   4,   8,   8,   9,   9,   8,   7,   4,   2 }, // 0.75
    {   0,   0,   0,   3,   4,   8,   9,   9,   8,   8,   3,   4,   2 }, // 1.0
    {   0,   0,   0,   4,   8,   9,   9,   9,   8,   8,   4,   3,   1 }, // 1.25
    {   0,   0,   0,   4,   8,   9,   9,   9,   8,   4,   3,   3,   1 }, // 1.5
    {   0,   0,   0,   7,   8,   9,   9,   8,   8,   4,   3,   3,   1 }, // 1.75
    {   0,   0,   0,   7,   8,   8,   8,   8,   4,   3,   3,   3,   1 }, // 2.0
    {   0,   0,   0,   7,   7,   7,   7,   5,   4,   3,   3,   1,   1 }, // 2.25
    {   0,   0,   0,   4,   4,   3,   3,   3,   3,   2,   1,   1,   1 }, // 2.5
    {   0,   0,   0,   4,   4,   2,   2,   2,   1,   1,   1,   1,   1 }, // 2.75
    {   0,   0,   0,   4,   2,   2,   2,   1,   1,   1,   1,   1,   1 }  // 3.0
#elif 0
    //  0 0.25  0.5 0.75  1.0 1.25  1.5 1.75  2.0 2.25  2.5 2.75  3.0
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // 0.25
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // 0.5
    {   0,   0,   0,   0,   0,   1,   2,   4,   6,   5,   1,   0,   0 }, // 0.75
    {   0,   0,   0,   1,   2,   4,   8,  11,  15,  10,   7,   4,   0 }, // 1.0
    {   0,   0,   0,   1,   4,   6,   9,  13,  13,   8,   4,   4,   0 }, // 1.25
    {   0,   0,   0,   1,   6,   9,  11,  11,  10,   6,   3,   3,   0 }, // 1.5
    {   0,   0,   0,   1,   8,   9,   9,   9,   8,   4,   2,   1,   0 }, // 1.75
    {   0,   0,   0,   2,   8,   9,   9,   8,   6,   3,   1,   1,   0 }, // 2.0
    {   0,   0,   0,   2,   8,   8,   8,   6,   3,   2,   1,   0,   0 }, // 2.25
    {   0,   0,   0,   2,   7,   7,   6,   3,   2,   1,   0,   0,   0 }, // 2.5
    {   0,   0,   0,   1,   4,   3,   3,   2,   1,   0,   0,   0,   0 }, // 2.75
    {   0,   0,   0,   1,   2,   2,   1,   0,   0,   0,   0,   0,   0 }, // 3.0
    {   0,   0,   0,   1,   2,   2,   0,   0,   0,   0,   0,   0,   0 }  // 3.25
#elif 1
    // 6500K 1.367 0.959
    // 4150K 1.296 1.245
    // 4000K 1.188 1.209
    // 3000K 1.001 1.408
    // 2850K 1.011 1.399
    //  0 0.25  0.5 0.75  1.0 1.25  1.5 1.75  2.0 2.25  2.5 2.75  3.0
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // 0.25
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // 0.5
    {   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   0 }, // 0.75
    {   0,   0,   0,   0,   1,   4,   6,   8,   8,   8,   7,   4,   1 }, // 1
    {   0,   0,   0,   1,   4,   6,   9,   9,   9,   8,   7,   3,   2 }, // 1.25
    {   0,   0,   0,   2,   8,   9,  11,   9,   9,   8,   6,   3,   1 }, // 1.5
    {   0,   0,   0,   4,  11,  13,  11,   9,   8,   6,   3,   2,   0 }, // 1.75
    {   0,   0,   0,   6,  15,  13,  10,   8,   6,   3,   2,   1,   0 }, // 2
    {   0,   0,   0,   5,  10,   8,   6,   4,   3,   2,   1,   0,   0 }, // 2.25
    {   0,   0,   0,   4,   7,   4,   3,   2,   1,   1,   0,   0,   0 }, // 2.5
    {   0,   0,   0,   0,   4,   4,   3,   1,   1,   0,   0,   0,   0 }, // 2.75
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // 3
    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }  // 3.25
#elif 0
    //  0 0.25  0.5 0.75  1.0 1.25  1.5 1.75  2.0 2.25  2.5 2.75  3.0
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }, // 0.25
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }, // 0.5
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }, // 0.75
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }, // 1
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }, // 1.25
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }, // 1.5
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }, // 1.75
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }, // 2
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }, // 2.25
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }, // 2.5
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }, // 2.75
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }, // 3
    {   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8,   8 }  // 3.25
#else
    //  0 0.25  0.5 0.75  1.0 1.25  1.5 1.75  2.0 2.25  2.5 2.75  3.0
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 0
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 0.25
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 0.5
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 0.75
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 1.0
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 1.25
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 1.5
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 1.75
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 2.0
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 2.25
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 2.5
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }, // 2.75
    {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 }  // 3.0
#endif
	};


/*
[31:2] - Reserved
[1] SCD_ERROR SC data has to be moved to sdram in one row of time.
If SC data couldn't be dumped in time, this bit is set.
Users need to issue isp reset to clear this bit.
0: SC data is dumped correctly
1: SC data is not dumped in time and conflict happens.

[0] SCD_DONE SC dumping starts at the end of each row of SC sub-windows.
And it should ends before the next SC dumping starts.
This bit is cleared at the frame start.
0: SC dumping is not done yet
1: SC dumping is done
*/
AWB_UINT32 hw_awb_module_scd_status_error_get(void)
{
	return (AWB_READL(AWB_SCD_STATUS) & 0x00000002);
}

void hw_awb_module_scd_status_done_set(AWB_UINT32 done_set)
{ 
	AWB_UINT32 m_done_set = done_set & 0x00000001;

	AWB_WRITEL(m_done_set, AWB_SCD_STATUS);
}
AWB_UINT32 hw_awb_module_scd_status_done_get(void)
{ 
	return (AWB_READL(AWB_SCD_STATUS) & 0x00000001);
}

void hw_awb_module_sc_enable_set(AWB_UINT32 enable)
{
	AWB_UINT32 m_sc_enable = enable;
	
	AWB_WRITEL(m_sc_enable, AWB_SC_ENABLE);
}

AWB_UINT32 hw_awb_module_sc_enable_get(void)
{
	return AWB_READL(AWB_SC_ENABLE);
}
																		
void hw_awb_module_scd_addr_set(AWB_UINT32 scd_addr)
{
	AWB_UINT32 m_scd_addr = 0x00000000;

	m_scd_addr = scd_addr;

//	printk("scd_addr=0x%08X\n", scd_addr);
//	printk("m_scd_addr=0x%08X\n", m_scd_addr);

	AWB_WRITEL(m_scd_addr, AWB_SCD_CONFIG_ADR);
}
AWB_UINT32 hw_awb_module_scd_addr_get(void)
{
	return AWB_READL(AWB_SCD_CONFIG_ADR);
}


void hw_awb_module_scd_id_set(AWB_UINT32 scd_id)
{
	//[7:0] SCD_ID AXI write address and data ID tag for SC dumping
	AWB_UINT32 m_scd_id = 0x00000000;

	m_scd_id = scd_id;

	printk("m_scd_id=0x%08X\n", m_scd_id);

	AWB_WRITEL(m_scd_id, AWB_SCD_CONFIG_ID);
}

void hw_awb_module_scd_irq_set(AWB_UINT32 scd_irq)
{
	/*
	[31:7] - Reserved
	[0] SCD_INT_DONE This bit is set when detecting rising edge of SCD_DONE.
	Users could still read or write this bit as normal register.
	It is also used as interrupt output to CPU.
	Writing 0 means to clear this flag.
	*/
	AWB_UINT32 m_scd_irq = 0x00000000;

	m_scd_irq = scd_irq;

//	printk("m_scd_irq=0x%08X\n", m_scd_irq);

	AWB_WRITEL(m_scd_irq, AWB_SCD_IRQ);
}
AWB_UINT32 hw_awb_module_scd_irq_get(void)
{
	return AWB_READL(AWB_SCD_IRQ);
}


void hw_awb_module_sc_pixel_set(AWB_UINT32 sc_pixel)
{
	//[0] SC_PIXEL_SHIFT 0: Left shift 1 bit to pixel data from ISP(Normal gamut) 1: No shift(wide gamut)
	AWB_UINT32 m_sc_pixel = 0x00000000;

	m_sc_pixel = sc_pixel;

	printk("m_sc_pixel=0x%08X\n", m_sc_pixel);

	AWB_WRITEL(m_sc_pixel, AWB_SC_CONFIG_PIXEL);
}

void hw_awb_module_sc_cropping_set(AWB_UINT32 sc_vstart, AWB_UINT32 sc_hstart)
{
	//[31:16] SC_VSTART Vertical starting point for frame cropping
	//[15:0] SC_HSTART Horizontal starting point for frame cropping
	AWB_UINT32 m_sc_cropping = 0x00000000;

	m_sc_cropping = (m_sc_cropping | (sc_vstart & 0x0000FFFF) << 16) | (sc_hstart & 0x0000FFFF);

	printk("m_sc_cropping=0x%08X\n", m_sc_cropping);

	AWB_WRITEL(m_sc_cropping, AWB_SC_CONFIG_CROPPING);
}

void hw_awb_module_sc_decimation_set(AWB_UINT32 sc_dev_vkeep, AWB_UINT32 sc_dev_vperiod, AWB_UINT32 sc_dev_hkeep, AWB_UINT32 sc_dev_hperiod)
{
	/*
	[31:29] - Reserved
	[28:24] SC_DEC_VKEEP Number of rows to keep in vertical period (SC_DEC_VKEEP + 1)
	[23:21] - Reserved
	[20:16] SC_DEC_VPERIOD Vertical period(SC_DEC_VPERIOD + 1)
	[15:13] - Reserved
	[12:8] SC_DEC_HKEEP Number of columns to keep in horizontal period (SC_DEC_HKEEP + 1)
	[7:5] - Reserved
	[4:0] SC_DEC_HPERIOD Horizontal period(SC_DEC_HPERIOD + 1)
	*/
	AWB_UINT32 m_sc_decimation = 0x00000000;

	m_sc_decimation = m_sc_decimation | ((sc_dev_vkeep & 0x0000000F) << 24);

	m_sc_decimation = m_sc_decimation | ((sc_dev_vperiod & 0x0000000F) << 16);

	m_sc_decimation = m_sc_decimation | ((sc_dev_hkeep & 0x0000000F) << 8);

	m_sc_decimation = m_sc_decimation | (sc_dev_hperiod & 0x0000000F);

	printk("m_sc_decimation=0x%08X\n", m_sc_decimation);

	AWB_WRITEL(m_sc_decimation, AWB_SC_CONFIG_DECIMATION);
}

void hw_awb_module_sc_windowing_set(AWB_UINT32 sc_height, AWB_UINT32 sc_width)
{
	/*
	[31:13] - Reserved
	[12:8] SC_HEIGHT Height of SC sub-window(SC_HEIGHT + 1) Due to bayer cell is 2x2, even number of height is suggested
	[7:5] - Reserved
	[4:0] SC_WIDTH Width of SC sub-window(SC_WIDTH + 1) Due to bayer cell is 2x2, even number of width is suggested
	*/
	AWB_UINT32 m_sc_windowing = 0x00000000;

	m_sc_windowing = m_sc_windowing | ((sc_height & 0x0000001F) << 8);

	m_sc_windowing = m_sc_windowing | (sc_width & 0x0000001F);

	printk("m_sc_windowing=0x%08X\n", m_sc_windowing);

	AWB_WRITEL(m_sc_windowing, AWB_SC_CONFIG_WINDOWING);
}

//AWB_SC_AWB_PS_CONFIG_0 ~ AWB_SC_AWB_PS_CONFIG_5
void hw_awb_module_sc_ps_boundary_g_r_get(AWB_UINT8 *ps_upper_g, AWB_UINT8 *ps_lower_g, AWB_UINT8 *ps_upper_r, AWB_UINT8 *ps_lower_r)
{
    /*
    [31:24] SC_AWB_PS_GU    [0.8.0] Upper boundary of G value
    [23:16] SC_AWB_PS_GL    [0.8.0] Lower boundary of G value
    [15:8]  SC_AWB_PS_RU    [0.8.0] Upper boundary of R value
    [7:0] SC_AWB_PS_RL  [0.8.0] Lower boundary of R value
    */
    HW_AWB_UTDWORD m_sc_ps_boundary_g_r;

    m_sc_ps_boundary_g_r.dwValue = AWB_READL(AWB_SC_AWB_PS_CONFIG_0);
    *ps_upper_g = m_sc_ps_boundary_g_r.tb.byValue3;
    *ps_lower_g = m_sc_ps_boundary_g_r.tb.byValue2;
    *ps_upper_r = m_sc_ps_boundary_g_r.tb.byValue1;
    *ps_lower_r = m_sc_ps_boundary_g_r.tb.byValue0;

    printk("m_sc_ps_boundary_g_r=0x%08X\n", m_sc_ps_boundary_g_r.dwValue);
}

//AWB_SC_AWB_PS_CONFIG_0 ~ AWB_SC_AWB_PS_CONFIG_5
void hw_awb_module_sc_ps_boundary_g_r_set(AWB_UINT32 ps_upper_g, AWB_UINT32 ps_lower_g, AWB_UINT32 ps_upper_r, AWB_UINT32 ps_lower_r)
{
	/*
	[31:24]	SC_AWB_PS_GU	[0.8.0] Upper boundary of G value
	[23:16]	SC_AWB_PS_GL	[0.8.0] Lower boundary of G value
	[15:8]	SC_AWB_PS_RU	[0.8.0] Upper boundary of R value
	[7:0] SC_AWB_PS_RL	[0.8.0] Lower boundary of R value
	*/
	AWB_UINT32 m_sc_ps_boundary_g_r = 0x00000000;

	m_sc_ps_boundary_g_r = m_sc_ps_boundary_g_r | ((ps_upper_g & 0x000000FF) << 24);
	m_sc_ps_boundary_g_r = m_sc_ps_boundary_g_r | ((ps_lower_g & 0x000000FF) << 16);
	m_sc_ps_boundary_g_r = m_sc_ps_boundary_g_r | ((ps_upper_r & 0x000000FF) << 8);
	m_sc_ps_boundary_g_r = m_sc_ps_boundary_g_r | (ps_lower_r & 0x000000FF);

	printk("m_sc_ps_boundary_g_r=0x%08X\n", m_sc_ps_boundary_g_r);

	AWB_WRITEL(m_sc_ps_boundary_g_r, AWB_SC_AWB_PS_CONFIG_0);
}

void hw_awb_module_sc_ps_boundary_y_b_get(AWB_UINT8 *ps_upper_y, AWB_UINT8 *ps_lower_y, AWB_UINT8 *ps_upper_b, AWB_UINT8 *ps_lower_b)
{
    /*
    [31:24] SC_AWB_PS_YU    [0.8.0] Upper boundary of Y value
    [23:16] SC_AWB_PS_YL    [0.8.0] Lower boundary of Y value
    [15:8]  SC_AWB_PS_BU    [0.8.0] Upper boundary of B value
    [7:0] SC_AWB_PS_BL  [0.8.0] Lower boundary of B value
    */
    HW_AWB_UTDWORD m_sc_ps_boundary_y_b;

    m_sc_ps_boundary_y_b.dwValue = AWB_READL(AWB_SC_AWB_PS_CONFIG_1);
    *ps_upper_y = m_sc_ps_boundary_y_b.tb.byValue3;
    *ps_lower_y = m_sc_ps_boundary_y_b.tb.byValue2;
    *ps_upper_b = m_sc_ps_boundary_y_b.tb.byValue1;
    *ps_lower_b = m_sc_ps_boundary_y_b.tb.byValue0;

    printk("m_sc_ps_boundary_y_b=0x%08X\n", m_sc_ps_boundary_y_b.dwValue);
}

void hw_awb_module_sc_ps_boundary_y_b_set(AWB_UINT32 ps_upper_y, AWB_UINT32 ps_lower_y, AWB_UINT32 ps_upper_b, AWB_UINT32 ps_lower_b)
{
	/*
	[31:24]	SC_AWB_PS_YU	[0.8.0] Upper boundary of Y value
	[23:16]	SC_AWB_PS_YL	[0.8.0] Lower boundary of Y value
	[15:8]	SC_AWB_PS_BU	[0.8.0] Upper boundary of B value
	[7:0] SC_AWB_PS_BL	[0.8.0] Lower boundary of B value
	*/
	AWB_UINT32 m_sc_ps_boundary_y_b = 0x00000000;

	m_sc_ps_boundary_y_b = m_sc_ps_boundary_y_b | ((ps_upper_y & 0x000000FF) << 24);
	m_sc_ps_boundary_y_b = m_sc_ps_boundary_y_b | ((ps_lower_y & 0x000000FF) << 16);
	m_sc_ps_boundary_y_b = m_sc_ps_boundary_y_b | ((ps_upper_b & 0x000000FF) << 8);
	m_sc_ps_boundary_y_b = m_sc_ps_boundary_y_b | (ps_lower_b & 0x000000FF);

	printk("m_sc_ps_boundary_y_b=0x%08X\n", m_sc_ps_boundary_y_b);

	AWB_WRITEL(m_sc_ps_boundary_y_b, AWB_SC_AWB_PS_CONFIG_1);
}

void hw_awb_module_sc_ps_boundary_gr_get(AWB_UINT16 *ps_upper_gr_ratio, AWB_UINT16 *ps_lower_gr_ratio)
{
	/*
	[31:16]	SC_AWB_PS_GRU	[0.8.8] Upper boundary of G/R ratio
	[15:0]	SC_AWB_PS_GRL	[0.8.8] Lower boundary of G/R ratio
	*/
    HW_AWB_UTDWORD m_sc_ps_boundary_gr;

    m_sc_ps_boundary_gr.dwValue = AWB_READL(AWB_SC_AWB_PS_CONFIG_2);
    *ps_upper_gr_ratio = m_sc_ps_boundary_gr.tw.wValue1;
    *ps_lower_gr_ratio = m_sc_ps_boundary_gr.tw.wValue0;

	printk("m_sc_ps_boundary_gr=0x%08X\n", m_sc_ps_boundary_gr.dwValue);
}

void hw_awb_module_sc_ps_boundary_gr_set(AWB_UINT32 ps_upper_gr_ratio, AWB_UINT32 ps_lower_gr_ratio)
{
    /*
    [31:16] SC_AWB_PS_GRU   [0.8.8] Upper boundary of G/R ratio
    [15:0]  SC_AWB_PS_GRL   [0.8.8] Lower boundary of G/R ratio
    */
    AWB_UINT32 m_sc_ps_boundary_gr = 0x00000000;

    m_sc_ps_boundary_gr = m_sc_ps_boundary_gr | ((ps_upper_gr_ratio & 0x0000FFFF) << 16);
    m_sc_ps_boundary_gr = m_sc_ps_boundary_gr | (ps_lower_gr_ratio & 0x0000FFFF);

    printk("m_sc_ps_boundary_gr=0x%08X\n", m_sc_ps_boundary_gr);

    AWB_WRITEL(m_sc_ps_boundary_gr, AWB_SC_AWB_PS_CONFIG_2);
}

void hw_awb_module_sc_ps_boundary_gb_get(AWB_UINT16 *ps_upper_gb_ratio, AWB_UINT16 *ps_lower_gb_ratio)
{
	/*
	[31:16]	SC_AWB_PS_GBU	[0.8.8] Upper boundary of G/B ratio
	[15:0]	SC_AWB_PS_GBL	[0.8.8] Lower boundary of G/B ratio
	*/
    HW_AWB_UTDWORD m_sc_ps_boundary_gb;

    m_sc_ps_boundary_gb.dwValue = AWB_READL(AWB_SC_AWB_PS_CONFIG_3);
    *ps_upper_gb_ratio = m_sc_ps_boundary_gb.tw.wValue1;
    *ps_lower_gb_ratio = m_sc_ps_boundary_gb.tw.wValue0;

	printk("m_sc_ps_boundary_gb=0x%08X\n", m_sc_ps_boundary_gb.dwValue);
}

void hw_awb_module_sc_ps_boundary_gb_set(AWB_UINT32 ps_upper_gb_ratio, AWB_UINT32 ps_lower_gb_ratio)
{
    /*
    [31:16] SC_AWB_PS_GBU   [0.8.8] Upper boundary of G/B ratio
    [15:0]  SC_AWB_PS_GBL   [0.8.8] Lower boundary of G/B ratio
    */
    AWB_UINT32 m_sc_ps_boundary_gb = 0x00000000;

    m_sc_ps_boundary_gb = m_sc_ps_boundary_gb | ((ps_upper_gb_ratio & 0x0000FFFF) << 16);
    m_sc_ps_boundary_gb = m_sc_ps_boundary_gb | (ps_lower_gb_ratio & 0x0000FFFF);

    printk("m_sc_ps_boundary_gb=0x%08X\n", m_sc_ps_boundary_gb);

    AWB_WRITEL(m_sc_ps_boundary_gb, AWB_SC_AWB_PS_CONFIG_3);
}

void hw_awb_module_sc_ps_boundary_grb_get(AWB_UINT16 *ps_upper_grb_ratio, AWB_UINT16 *ps_lower_grb_ratio)
{
	/*
	[31:16]	SC_AWB_PS_GRBU	[0.8.8] Upper boundary of (Gr/R + b/a * Gb/B)
	[15:0]	SC_AWB_PS_GRBL	[0.8.8] Lower boundary of (Gr/R + b/a * Gb/B)
	*/
    HW_AWB_UTDWORD m_sc_ps_boundary_grb;

    m_sc_ps_boundary_grb.dwValue = AWB_READL(AWB_SC_AWB_PS_CONFIG_4);
    *ps_upper_grb_ratio = m_sc_ps_boundary_grb.tw.wValue1;
    *ps_lower_grb_ratio = m_sc_ps_boundary_grb.tw.wValue0;

	printk("m_sc_ps_boundary_grb=0x%08X\n", m_sc_ps_boundary_grb.dwValue);
}

void hw_awb_module_sc_ps_boundary_grb_set(AWB_UINT32 ps_upper_grb_ratio, AWB_UINT32 ps_lower_grb_ratio)
{
    /*
    [31:16] SC_AWB_PS_GRBU  [0.8.8] Upper boundary of (Gr/R + b/a * Gb/B)
    [15:0]  SC_AWB_PS_GRBL  [0.8.8] Lower boundary of (Gr/R + b/a * Gb/B)
    */
    AWB_UINT32 m_sc_ps_boundary_grb = 0x00000000;

    m_sc_ps_boundary_grb = m_sc_ps_boundary_grb | ((ps_upper_grb_ratio & 0x0000FFFF) << 16);
    m_sc_ps_boundary_grb = m_sc_ps_boundary_grb | (ps_lower_grb_ratio & 0x0000FFFF);

    printk("m_sc_ps_boundary_grb=0x%08X\n", m_sc_ps_boundary_grb);

    AWB_WRITEL(m_sc_ps_boundary_grb, AWB_SC_AWB_PS_CONFIG_4);
}

void hw_awb_module_sc_ps_cofficient_ba_set(AWB_UINT32 ps_cofficient_ba)
{
	/*
	[31:8]	-	Reserved
	[7:0] SC_AWB_PS_GRB_BA	[0.4.4] Coefficient b/a for GRB range equation
	*/
	AWB_UINT32 m_sc_ps_cofficient_ba = 0x00000000;

	m_sc_ps_cofficient_ba = m_sc_ps_cofficient_ba | (ps_cofficient_ba & 0x000000FF);

	printk("m_sc_ps_cofficient_ba=0x%08X\n", m_sc_ps_cofficient_ba);

	AWB_WRITEL(m_sc_ps_cofficient_ba, AWB_SC_AWB_PS_CONFIG_5);
}

//SC_AWB_WS_CONFIG_0, SC_AWB_WS_CONFIG_1
void hw_awb_module_sc_ws_boundary_gr_r_get(AWB_UINT8 *ws_upper_gr, AWB_UINT8 *ws_lower_gr, AWB_UINT8 *ws_upper_r, AWB_UINT8 *ws_lower_r)
{
    /*
    [31:24] SC_AWB_WS_GRU   [0.8.0] Upper boundary of Gr value
    [23:16] SC_AWB_WS_GRL   [0.8.0] Lower boundary of Gr value
    [15:8]  SC_AWB_WS_RU    [0.8.0] Upper boundary of R value
    [7:0] SC_AWB_WS_RL  [0.8.0] Lower boundary of R value
    */
    HW_AWB_UTDWORD m_sc_ws_boundary_gr_r;

    m_sc_ws_boundary_gr_r.dwValue = AWB_READL(AWB_SC_AWB_WS_CONFIG_0);
    *ws_upper_gr = m_sc_ws_boundary_gr_r.tb.byValue3;
    *ws_lower_gr = m_sc_ws_boundary_gr_r.tb.byValue2;
    *ws_upper_r = m_sc_ws_boundary_gr_r.tb.byValue1;
    *ws_lower_r = m_sc_ws_boundary_gr_r.tb.byValue0;

    printk("m_sc_ws_boundary_gr_r=0x%08X\n", m_sc_ws_boundary_gr_r.dwValue);
}

//SC_AWB_WS_CONFIG_0, SC_AWB_WS_CONFIG_1
void hw_awb_module_sc_ws_boundary_gr_r_set(AWB_UINT32 ws_upper_gr, AWB_UINT32 ws_lower_gr, AWB_UINT32 ws_upper_r, AWB_UINT32 ws_lower_r)
{
	/*
	[31:24]	SC_AWB_WS_GRU	[0.8.0] Upper boundary of Gr value
	[23:16]	SC_AWB_WS_GRL	[0.8.0] Lower boundary of Gr value
	[15:8]	SC_AWB_WS_RU	[0.8.0] Upper boundary of R value
	[7:0] SC_AWB_WS_RL	[0.8.0] Lower boundary of R value
	*/
	AWB_UINT32 m_sc_ws_boundary_gr_r = 0x00000000;

	m_sc_ws_boundary_gr_r = m_sc_ws_boundary_gr_r | ((ws_upper_gr & 0x000000FF) << 24);
	m_sc_ws_boundary_gr_r = m_sc_ws_boundary_gr_r | ((ws_lower_gr & 0x000000FF) << 16);
	m_sc_ws_boundary_gr_r = m_sc_ws_boundary_gr_r | ((ws_upper_r & 0x000000FF) << 8);
	m_sc_ws_boundary_gr_r = m_sc_ws_boundary_gr_r | (ws_lower_r & 0x000000FF);

	printk("m_sc_ws_boundary_gr_r=0x%08X\n", m_sc_ws_boundary_gr_r);

	AWB_WRITEL(m_sc_ws_boundary_gr_r, AWB_SC_AWB_WS_CONFIG_0);
}

void hw_awb_module_sc_ws_boundary_b_gb_get(AWB_UINT8 *ws_upper_b, AWB_UINT8 *ws_lower_b, AWB_UINT8 *ws_upper_gb, AWB_UINT8 *ws_lower_gb)
{
	/*
	[31:24]	SC_AWB_WS_BU	[0.8.0] Upper boundary of B value
	[23:16]	SC_AWB_WS_BL	[0.8.0] Lower boundary of B value
	[15:8]	SC_AWB_WS_GBU	[0.8.0] Upper boundary of Gb value
	[7:0] SC_AWB_WS_GBL	[0.8.0] Lower boundary of Gb value
	*/
    HW_AWB_UTDWORD m_sc_ws_boundary_b_gb;

    m_sc_ws_boundary_b_gb.dwValue = AWB_READL(AWB_SC_AWB_WS_CONFIG_1);
    *ws_upper_b = m_sc_ws_boundary_b_gb.tb.byValue3;
    *ws_lower_b = m_sc_ws_boundary_b_gb.tb.byValue2;
    *ws_upper_gb = m_sc_ws_boundary_b_gb.tb.byValue1;
    *ws_lower_gb = m_sc_ws_boundary_b_gb.tb.byValue0;

	printk("m_sc_ws_boundary_b_gb=0x%08X\n", m_sc_ws_boundary_b_gb.dwValue);
}

void hw_awb_module_sc_ws_boundary_b_gb_set(AWB_UINT32 ws_upper_b, AWB_UINT32 ws_lower_b, AWB_UINT32 ws_upper_gb, AWB_UINT32 ws_lower_gb)
{
    /*
    [31:24] SC_AWB_WS_BU    [0.8.0] Upper boundary of B value
    [23:16] SC_AWB_WS_BL    [0.8.0] Lower boundary of B value
    [15:8]  SC_AWB_WS_GBU   [0.8.0] Upper boundary of Gb value
    [7:0] SC_AWB_WS_GBL [0.8.0] Lower boundary of Gb value
    */
    AWB_UINT32 m_sc_ws_boundary_b_gb = 0x00000000;

    m_sc_ws_boundary_b_gb = m_sc_ws_boundary_b_gb | ((ws_upper_b & 0x000000FF) << 24);
    m_sc_ws_boundary_b_gb = m_sc_ws_boundary_b_gb | ((ws_lower_b & 0x000000FF) << 16);
    m_sc_ws_boundary_b_gb = m_sc_ws_boundary_b_gb | ((ws_upper_gb & 0x000000FF) << 8);
    m_sc_ws_boundary_b_gb = m_sc_ws_boundary_b_gb | (ws_lower_gb & 0x000000FF);

    printk("m_sc_ws_boundary_b_gb=0x%08X\n", m_sc_ws_boundary_b_gb);

    AWB_WRITEL(m_sc_ws_boundary_b_gb, AWB_SC_AWB_WS_CONFIG_1);
}


void hw_awb_module_init(void)
{
#if defined(IMG_KERNEL_MODULE)
	// io remap awb virtbaseaddr
	g_ioAwbRegVirtBaseAddr = ioremap(IMAP_ISP_BASE+ 0x00040000, IMAP_ISP_SIZE);

	printk("g_ioAwbRegVirtBaseAddr = 0x%p\n", g_ioAwbRegVirtBaseAddr);

//	printk("isp VirtBaseAddr = 0x%p\n", ioremap(IMAP_ISP_BASE, IMAP_ISP_SIZE));
	
#endif

	//test 
//	hw_awb_module_scd_addr_set(0x000d3624);
//	printk("AWB_SCD_CONFIG_ADR value = 0x%08X\n", AWB_READL(AWB_SCD_CONFIG_ADR));

	hw_awb_module_scd_id_set(0x00000002);
	printk("AWB_SCD_CONFIG_ID value = 0x%08X\n", AWB_READL(AWB_SCD_CONFIG_ID));

	hw_awb_module_sc_pixel_set(0x00000001);
	printk("AWB_SC_CONFIG_PIXEL value = 0x%08X\n", AWB_READL(AWB_SC_CONFIG_PIXEL));

	hw_awb_module_sc_cropping_set(0, 0);
	printk("AWB_SC_CONFIG_CROPPING value = 0x%08X\n", AWB_READL(AWB_SC_CONFIG_CROPPING));

//	hw_awb_module_sc_decimation_set(0, 3, 0, 3);
	hw_awb_module_sc_decimation_set(1, 7, 1, 7);
	printk("AWB_SC_CONFIG_DECIMATION value = 0x%08X\n", AWB_READL(AWB_SC_CONFIG_DECIMATION));

//	hw_awb_module_sc_windowing_set(5, 7);
//	hw_awb_module_sc_windowing_set(7, 13);
	hw_awb_module_sc_windowing_set(15, 29);
	printk("AWB_SC_CONFIG_WINDOWING value = 0x%08X\n", AWB_READL(AWB_SC_CONFIG_WINDOWING));

	//Pixel summation method config
//	hw_awb_module_sc_ps_boundary_g_r_set(0xfa, 0x01, 0xf0, 0x01);	//0x0024
//	hw_awb_module_sc_ps_boundary_y_b_set(0xf3, 0x07, 0xfa, 0x0c);	//0x0028
//	hw_awb_module_sc_ps_boundary_gr_set(0xfff7, 0x000a);	//0x002C
//	hw_awb_module_sc_ps_boundary_gb_set(0xfffa, 0x0008);	//0x0030
//	hw_awb_module_sc_ps_boundary_grb_set(0xfffc, 0x0000);	//0x0034
//	hw_awb_module_sc_ps_cofficient_ba_set(0x43);	//0x0038
#if 0
	hw_awb_module_sc_ps_boundary_g_r_set(0xf0, 0x0a, 0xf0, 0x0a);	//0x0024
	hw_awb_module_sc_ps_boundary_y_b_set(0xf0, 0x0a, 0xf0, 0x0a);	//0x0028
#elif 1
	hw_awb_module_sc_ps_boundary_g_r_set(0xf0, 0x05, 0xf0, 0x05);	//0x0024
	hw_awb_module_sc_ps_boundary_y_b_set(0xf0, 0x05, 0xf0, 0x05);	//0x0028
#else
	hw_awb_module_sc_ps_boundary_g_r_set(0xff, 0x01, 0xff, 0x01);	//0x0024
	hw_awb_module_sc_ps_boundary_y_b_set(0xff, 0x01, 0xff, 0x01);	//0x0028
#endif
#if 0
	hw_awb_module_sc_ps_boundary_gr_set(0x0300, 0x0060);	//0x002C
	hw_awb_module_sc_ps_boundary_gb_set(0x0300, 0x0060);	//0x0030
//	hw_awb_module_sc_ps_boundary_grb_set(0x0300, 0x0060);	//0x0034
	hw_awb_module_sc_ps_boundary_grb_set(0x0600, 0x0060);	//0x0034
#elif 0
	hw_awb_module_sc_ps_boundary_gr_set(0x0a00, 0x0020);	//0x002C
	hw_awb_module_sc_ps_boundary_gb_set(0x0a00, 0x0020);	//0x0030
	hw_awb_module_sc_ps_boundary_grb_set(0x0a00, 0x0020);	//0x0034
#else
	hw_awb_module_sc_ps_boundary_gr_set(0xffff, 0x0020);	//0x002C
	hw_awb_module_sc_ps_boundary_gb_set(0xffff, 0x0020);	//0x0030
	hw_awb_module_sc_ps_boundary_grb_set(0xffff, 0x0020);	//0x0034
#endif
//	hw_awb_module_sc_ps_cofficient_ba_set(0x01);	//0x0038
	hw_awb_module_sc_ps_cofficient_ba_set(0x10);	//0x0038

	//Weighted gain summation method config
//	hw_awb_module_sc_ws_boundary_gr_r_set(0xf2, 0x0d, 0xfd, 0x01);	//0x00BC
//	hw_awb_module_sc_ws_boundary_b_gb_set(0xf5, 0x05, 0xf5, 0x0d);	//0x00C0
	hw_awb_module_sc_ws_boundary_gr_r_set(0xFA, 0x0A, 0xFA, 0x0A);	//0x00BC
	hw_awb_module_sc_ws_boundary_b_gb_set(0xFA, 0x0A, 0xFA, 0x0A);	//0x00C0


	// SC Enable
//	AWB_WRITEL(AWB_ENABLE, AWB_SC_ENABLE);
//	printk("AWB_SC_ENABLE value = 0x%08X\n", AWB_READL(AWB_SC_ENABLE));

	printk("hw_awb_module_scd_status_error_get value = 0x%08X\n", hw_awb_module_scd_status_error_get());
	printk("hw_awb_module_scd_status_done_get value = 0x%08X\n", hw_awb_module_scd_status_done_get());

//	hw_awb_module_scd_irq_set(1);
#if 0
	int cnt = 0;
	while(hw_awb_module_scd_status_done_get() == 0)
	{
		printk("cnt:%d, hw_awb_module_scd_status_done_get value = 0x%08X\n", cnt, hw_awb_module_scd_status_done_get());

		cnt++;
		if (cnt > 20000)
			break;
	}
#endif
	hw_awb_module_scd_irq_set(0);

#if 1
{
	int i = 0;
	int j = 0;

	/* set weighting table */
	AWB_UINT32 HW_AWB_weight_table_value_0[13] = { 0 }; 
	AWB_UINT32 HW_AWB_weight_table_value_1[13] = { 0 };

	AWB_UINT32 HW_AWB_weight_table_value_0_check[13] = { 0 }; 
	AWB_UINT32 HW_AWB_weight_table_value_1_check[13] = { 0 };


	for(j=0; j<13; j++)
	{
		for (i=7; i>=0; i--)
		{
			HW_AWB_weight_table_value_0[j] = (HW_AWB_weight_table_value_0[j] << 4) | HW_AWB_WEIGHT_TABLE[j][i + 0];
		}

		for (i=4; i>=0; i--)
		{
			HW_AWB_weight_table_value_1[j] = (HW_AWB_weight_table_value_1[j] << 4) | HW_AWB_WEIGHT_TABLE[j][i + 8];
		}
	}

	AWB_UINT32 WEIGHT_START_ADDR = AWB_SC_AWB_WS_CONFIG_CW_0_GRP_0;
	AWB_UINT32 WEIGHT_OFFSET = 0x0004;

	for (i=0; i<13; i++)
	{
		AWB_WRITEL(HW_AWB_weight_table_value_0[i], WEIGHT_START_ADDR);
		WEIGHT_START_ADDR += WEIGHT_OFFSET;
		AWB_WRITEL(HW_AWB_weight_table_value_1[i], WEIGHT_START_ADDR);
		WEIGHT_START_ADDR += WEIGHT_OFFSET;
	}

	WEIGHT_START_ADDR = AWB_SC_AWB_WS_CONFIG_CW_0_GRP_0;
	for (i=0; i<13; i++)
	{
		HW_AWB_weight_table_value_0_check[i] = AWB_READL(WEIGHT_START_ADDR);
		WEIGHT_START_ADDR += WEIGHT_OFFSET;
		HW_AWB_weight_table_value_1_check[i] = AWB_READL(WEIGHT_START_ADDR);
		WEIGHT_START_ADDR += WEIGHT_OFFSET;

		printk("HW_AWB_weight_table_value_0_check[%d] = 0x%08X\n", i, HW_AWB_weight_table_value_0_check[i]);
		printk("HW_AWB_weight_table_value_1_check[%d] = 0x%08X\n", i, HW_AWB_weight_table_value_1_check[i]);
	}

	/* set v curve */
	static const AWB_UINT32 HW_AWB_v[16] = {0, 8, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 10, 6};

	AWB_UINT32 HW_AWB_v_value_0, HW_AWB_v_value_1;
	AWB_UINT32 HW_AWB_v_value_0_check, HW_AWB_v_value_1_check;


	HW_AWB_v_value_0 = 0x00000000;	//0xFFFFFF80
	HW_AWB_v_value_1 = 0x00000000;	//0x6AEFFFFF

	for (i=7; i>=0; i--)
	{
		HW_AWB_v_value_0 = (HW_AWB_v_value_0 << 4) | HW_AWB_v[i + 0];
		HW_AWB_v_value_1 = (HW_AWB_v_value_1 << 4) | HW_AWB_v[i + 8];		
	}
		AWB_WRITEL(HW_AWB_v_value_0, AWB_SC_AWB_WS_CONFIG_IW_V_GRP_0);
		AWB_WRITEL(HW_AWB_v_value_1, AWB_SC_AWB_WS_CONFIG_IW_V_GRP_1);

	printk("HW_AWB_v_value_0 = 0x%08X\n", HW_AWB_v_value_0);
	printk("HW_AWB_v_value_1 = 0x%08X\n", HW_AWB_v_value_1);

	HW_AWB_v_value_0_check = AWB_READL(AWB_SC_AWB_WS_CONFIG_IW_V_GRP_0);
	HW_AWB_v_value_1_check = AWB_READL(AWB_SC_AWB_WS_CONFIG_IW_V_GRP_1);

	printk("HW_AWB_v_value_0_check = 0x%08X\n", HW_AWB_v_value_0_check);
	printk("HW_AWB_v_value_1_check = 0x%08X\n", HW_AWB_v_value_1_check);

	/* set s curve */
	//s 0.5 0.44 0 0 0 0 0 0 0 0 0 0 -0.07 -0.25 -0.25 -0.25
	//static const float HW_AWB_s[16] = {0.5, 0.44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.07, -0.25, -0.25, -0.25};
	//0.5	= 0001,0000 = 0x10
	//0.44 	= 0000,1110 ~= 0.4375 = 0x0D
	//0		= 0000,0000  = 0x00
	//-0.07	= 1000,0010 ~= -0.0625 = 0x82
	//-0.25 = 1001,0000	=0x90
	
	AWB_UINT32 HW_AWB_s_value_0, HW_AWB_s_value_1, HW_AWB_s_value_2, HW_AWB_s_value_3;
	HW_AWB_s_value_0 = 0x00000D10;	//0.5, 0.44, 0, 0 => 0x00000D10
	HW_AWB_s_value_1 = 0x00000000;	//0, 0, 0, 0 => 0x00000000 
	HW_AWB_s_value_2 = 0x00000000;	//0, 0, 0, 0 => 0x00000000
	HW_AWB_s_value_3 = 0x90909082;	//-0.07, -0.25, -0.25, -0.25 => 0x90909082
	
	AWB_WRITEL(HW_AWB_s_value_0, AWB_SC_AWB_WS_CONFIG_IW_S_GRP_0);
	AWB_WRITEL(HW_AWB_s_value_1, AWB_SC_AWB_WS_CONFIG_IW_S_GRP_1);
	AWB_WRITEL(HW_AWB_s_value_2, AWB_SC_AWB_WS_CONFIG_IW_S_GRP_2);
	AWB_WRITEL(HW_AWB_s_value_3, AWB_SC_AWB_WS_CONFIG_IW_S_GRP_3);

	printk("AWB_SC_AWB_WS_CONFIG_IW_S_GRP_0 value = 0x%08X\n", AWB_READL(AWB_SC_AWB_WS_CONFIG_IW_S_GRP_0));
	printk("AWB_SC_AWB_WS_CONFIG_IW_S_GRP_1 value = 0x%08X\n", AWB_READL(AWB_SC_AWB_WS_CONFIG_IW_S_GRP_1));
	printk("AWB_SC_AWB_WS_CONFIG_IW_S_GRP_2 value = 0x%08X\n", AWB_READL(AWB_SC_AWB_WS_CONFIG_IW_S_GRP_2));
	printk("AWB_SC_AWB_WS_CONFIG_IW_S_GRP_3 value = 0x%08X\n", AWB_READL(AWB_SC_AWB_WS_CONFIG_IW_S_GRP_3));

}
#endif
};


void hw_awb_module_register_dump(void)
{
	printk("/* HW AWB REGISTER DUMP*/\n");
	printk("register:0x0000 value:0x%08X\n", AWB_READL(0x0000));
	printk("register:0x0004 value:0x%08X\n", AWB_READL(0x0004));
	printk("register:0x0008 value:0x%08X\n", AWB_READL(0x0008));
	printk("register:0x000C value:0x%08X\n", AWB_READL(0x000C));
	
	printk("register:0x0010 value:0x%08X\n", AWB_READL(0x0010));
	
	printk("register:0x0014 value:0x%08X\n", AWB_READL(0x0014));
	printk("register:0x0018 value:0x%08X\n", AWB_READL(0x0018));
	printk("register:0x001C value:0x%08X\n", AWB_READL(0x001C));
	printk("register:0x0020 value:0x%08X\n\n", AWB_READL(0x0020));
	
	printk("register:0x0024 value:0x%08X\n", AWB_READL(0x0024));
	printk("register:0x0028 value:0x%08X\n", AWB_READL(0x0028));
	printk("register:0x002C value:0x%08X\n", AWB_READL(0x002C));
	printk("register:0x0030 value:0x%08X\n", AWB_READL(0x0030));
	printk("register:0x0034 value:0x%08X\n", AWB_READL(0x0034));
	printk("register:0x0038 value:0x%08X\n\n", AWB_READL(0x0038));

	printk("//weight table\n");
	AWB_UINT32 offset = 0x0000;
	int i = 0;
	//weight table
	for (i=0; i<26; i++)
	{
		printk("register:0x%04X value:0x%08X\n", 0x003C+offset, AWB_READL(0x003C+offset) );
		offset += 0x0004;
	}

	//v curve	
	printk("\n//v curve\n");
	printk("register:0x00A4 value:0x%08X\n", AWB_READL(0x00A4));
	printk("register:0x00A8 value:0x%08X\n\n", AWB_READL(0x00A8));

	//s curve
	printk("//s curve\n");	
	printk("register:0x00AC value:0x%08X\n", AWB_READL(0x00AC));
	printk("register:0x00B0 value:0x%08X\n", AWB_READL(0x00B0));
	printk("register:0x00B4 value:0x%08X\n", AWB_READL(0x00B4));
	printk("register:0x00B8 value:0x%08X\n", AWB_READL(0x00B8));
	printk("register:0x00BC value:0x%08X\n\n", AWB_READL(0x00BC));

	printk("register:0x00BC value:0x%08X\n", AWB_READL(0x00BC));
	printk("register:0x00C0 value:0x%08X\n", AWB_READL(0x00C0));
}




#if 0

// IRQ
void hw_awb_module_irq_handler(int irq, void *x)
{
	printk("\n"); 
	printk("==============================================\n");
	printk("awb IRQ is triggered and then cleared!!!\n");
	printk("==============================================\n");
	
	// clear IRQ
//	writel(0x00000000, AWB_BASE_ADDR + 0x00000010);

	// disable IRQ
	disable_irq(AWB_INT_ID);
}

AWB_INT32 hw_awb_module_register_irq(void)
{
    AWB_INT32 ret = AWB_FALSE;

	// register IRQ and enable by default
	ret = (AWB_INT32)request_irq(AWB_INT_ID, (irq_handler_t)hw_awb_module_irq_handler, "awb_irq");
	if (ret) {
		printk("Request awb IRQ failed!\n");
	} else {
		printk("Request awb IRQ OK!\n");
	}

        return ret;
}

AWB_INT32 awb_module_irq(void)
{
	AWB_INT32 data = 0;
	AWB_INT32 ret = 0;
	AWB_INT32 reg_addr = 0;
	AWB_INT32 reg_data = 0;
	AWB_INT32 reg_expected_data = 0;

	// register IRQ and enable by default
	ret = hw_awb_module_register_irq();
	if(ret)	return ret;

	// check IRQ(0)
	if( ( (data = AWB_READL(AWB_SCD_IRQ)) & 0xffffffff) != (0x00000000 & 0xffffffff) )
		{
		if(!ret)
			{	// store first failure
			reg_addr = AWB_SCD_IRQ;
			reg_expected_data = 0x00000000;
			reg_data= data;
			}
//		printk(AWB_BASE_ADDR + 0x00000010, 0x00000000, data);
		ret++;	// increment error cnt
		}

	// trigger IRQ
	AWB_WRITEL(0x00000001, AWB_SCD_IRQ);

	// check IRQ(1)
	if( ( (data = AWB_READL(AWB_SCD_IRQ)) & 0xffffffff) != (0x00000001 & 0xffffffff) )
		{
		if(!ret)
			{	// store first failure
			reg_addr = AWB_SCD_IRQ;
			reg_expected_data = 0x00000001;
			reg_data= data;
			}
//		printk(AWB_BASE_ADDR + 0x00000010, 0x00000001, data);
		ret++;	// increment error cnt
		}

	// clear IRQ
	AWB_WRITEL(0x00000000, AWB_SCD_IRQ);

	// check IRQ(0)
	if( ( (data = AWB_READL(AWB_SCD_IRQ)) & 0xffffffff) != (0x00000000 & 0xffffffff) )
		{
		if(!ret)
			{	// store first failure
			reg_addr = AWB_SCD_IRQ;
			reg_expected_data = 0x00000000;
			reg_data= data;
			}
//		printk(AWB_BASE_ADDR + 0x00000010, 0x00000000, data);
		ret++;	// increment error cnt
		}

        return ret;
}

#endif



#endif //INFOTM_HW_AWB_METHOD

