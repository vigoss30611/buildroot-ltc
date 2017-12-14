#ifndef __HW_AWB_REG_H__
#define __HW_AWB_REG_H__


#if defined(__cplusplus)
extern "C" {
#endif

#define AWB_BASE_ADDR 0x00000000 //fake define


// AWB processor registers offset.
//==========================================================
// AWB
/*
31:2 - Reserved
1 SCD_ERROR SC data has to be moved to sdram in one row of time.
If SC data couldn't be dumped in time, this bit is set.
Users need to issue isp reset to clear this bit.
0: SC data is dumped correctly
1: SC data is not dumped in time and conflict happens.
0 SCD_DONE SC dumping starts at the end of each row of SC sub-windows.
And it should ends before the next SC dumping starts.
This bit is cleared at the frame start.
0: SC dumping is not done yet
1: SC dumping is done
*/
#define AWB_SCD_STATUS_OFFSET			(0x0000)
#define AWB_SCD_STATUS					(AWB_BASE_ADDR + AWB_SCD_STATUS_OFFSET)

/*
31:1 - Reserved
0 SC_ENABLE 0: Enable SC
1: Disable SC
*/
#define AWB_SC_ENABLE_OFFSET			(0x0004)
#define AWB_SC_ENABLE					(AWB_BASE_ADDR + AWB_SC_ENABLE_OFFSET)

/*
31:7 SCD_ADR Starting address for dumped SC data
(Granularity is 128 bytes)
6:0 - Used for granularity
*/
#define AWB_SCD_CONFIG_ADR_OFFSET		(0x0008)
#define AWB_SCD_CONFIG_ADR				(AWB_BASE_ADDR + AWB_SCD_CONFIG_ADR_OFFSET)

/*
31:8 - Reserved
7:0 SCD_ID AXI write address and data ID tag for SC dumping
*/
#define AWB_SCD_CONFIG_ID_OFFSET		(0x000C)
#define AWB_SCD_CONFIG_ID				(AWB_BASE_ADDR + AWB_SCD_CONFIG_ID_OFFSET)

/*
31:7 - Reserved
0 SCD_INT_DONE This bit is set when detecting rising edge of SCD_DONE.
Users could still read or write this bit as normal register.
It is also used as interrupt output to CPU.
Writing 0 means to clear this flag.
*/
#define AWB_SCD_IRQ_OFFSET				(0x0010)
#define AWB_SCD_IRQ						(AWB_BASE_ADDR + AWB_SCD_IRQ_OFFSET)

/*
31:1 - Reserved
0 SC_PIXEL_SHIFT 0: Left shift 1 bit to pixel data from ISP(Normal gamut)
1: No shift(wide gamut)
*/
#define AWB_SC_CONFIG_PIXEL_OFFSET		(0x0014)
#define AWB_SC_CONFIG_PIXEL				(AWB_BASE_ADDR + AWB_SC_CONFIG_PIXEL_OFFSET)

/*
31:16 SC_VSTART Vertical starting point for frame cropping
15:0 SC_HSTART Horizontal starting point for frame cropping
*/
#define AWB_SC_CONFIG_CROPPING_OFFSET		(0x0018)
#define AWB_SC_CONFIG_CROPPING				(AWB_BASE_ADDR + AWB_SC_CONFIG_CROPPING_OFFSET)

/*
31:29 - Reserved
28:24 SC_DEC_VKEEP Number of rows to keep in vertical period
(SC_DEC_VKEEP + 1)
23:21 - Reserved
20:16 SC_DEC_VPERIOD Vertical period(SC_DEC_VPERIOD + 1)
15:13 - Reserved
12:8 SC_DEC_HKEEP Number of columns to keep in horizontal period
(SC_DEC_HKEEP + 1)
7:5 - Reserved
4:0 SC_DEC_HPERIOD Horizontal period(SC_DEC_HPERIOD + 1)
*/
#define AWB_SC_CONFIG_DECIMATION_OFFSET		(0x001C)
#define AWB_SC_CONFIG_DECIMATION			(AWB_BASE_ADDR + AWB_SC_CONFIG_DECIMATION_OFFSET)

/*
31:13 - Reserved
12:8 SC_HEIGHT Height of SC sub-window(SC_HEIGHT + 1)
Due to bayer cell is 2x2, even number of height is suggested
7:5 - Reserved
4:0 SC_WIDTH Width of SC sub-window(SC_WIDTH + 1)
Due to bayer cell is 2x2, even number of width is suggested
*/
#define AWB_SC_CONFIG_WINDOWING_OFFSET		(0x0020)
#define AWB_SC_CONFIG_WINDOWING				(AWB_BASE_ADDR + AWB_SC_CONFIG_WINDOWING_OFFSET)


/*
31:24 SC_AWB_PS_GU [0.8.0] Upper boundary of G value
23:16 SC_AWB_PS_GL [0.8.0] Lower boundary of G value
15:8 SC_AWB_PS_RU [0.8.0] Upper boundary of R value
7:0 SC_AWB_PS_RL [0.8.0] Lower boundary of R value
*/
#define AWB_SC_AWB_PS_CONFIG_0_OFFSET		(0x0024)
#define AWB_SC_AWB_PS_CONFIG_0				(AWB_BASE_ADDR + AWB_SC_AWB_PS_CONFIG_0_OFFSET)

/*
31:24 SC_AWB_PS_YU [0.8.0] Upper boundary of Y value
23:16 SC_AWB_PS_YL [0.8.0] Lower boundary of Y value
15:8 SC_AWB_PS_BU [0.8.0] Upper boundary of B value
7:0 SC_AWB_PS_BL [0.8.0] Lower boundary of B value
*/
#define AWB_SC_AWB_PS_CONFIG_1_OFFSET		(0x0028)
#define AWB_SC_AWB_PS_CONFIG_1				(AWB_BASE_ADDR + AWB_SC_AWB_PS_CONFIG_1_OFFSET)

/*
31:16 SC_AWB_PS_GRU [0.8.8] Upper boundary of G/R ratio
15:0 SC_AWB_PS_GRL [0.8.8] Lower boundary of G/R ratio
*/
#define AWB_SC_AWB_PS_CONFIG_2_OFFSET		(0x002C)
#define AWB_SC_AWB_PS_CONFIG_2				(AWB_BASE_ADDR + AWB_SC_AWB_PS_CONFIG_2_OFFSET)

/*
31:16 SC_AWB_PS_GBU [0.8.8] Upper boundary of G/B ratio
15:0 SC_AWB_PS_GBL [0.8.8] Lower boundary of G/B ratio
*/
#define AWB_SC_AWB_PS_CONFIG_3_OFFSET		(0x0030)
#define AWB_SC_AWB_PS_CONFIG_3				(AWB_BASE_ADDR + AWB_SC_AWB_PS_CONFIG_3_OFFSET)

/*
31:16 SC_AWB_PS_GRBU [0.8.8] Upper boundary of (Gr/R + b/a * Gb/B)
15:0 SC_AWB_PS_GRBL [0.8.8] Lower boundary of (Gr/R + b/a * Gb/B)
*/
#define AWB_SC_AWB_PS_CONFIG_4_OFFSET		(0x0034)
#define AWB_SC_AWB_PS_CONFIG_4				(AWB_BASE_ADDR + AWB_SC_AWB_PS_CONFIG_4_OFFSET)

/*
31:8 - Reserved
7:0 SC_AWB_PS_GRB_BA [0.4.4] Coefficient b/a for GRB range equation
*/
#define AWB_SC_AWB_PS_CONFIG_5_OFFSET		(0x0038)
#define AWB_SC_AWB_PS_CONFIG_5				(AWB_BASE_ADDR + AWB_SC_AWB_PS_CONFIG_5_OFFSET)


// Weight table 12 x 12
/*
31:28 SC_AWB_WS_CW_W_0_7 [0.4.0] Weighting value at point (0, 7)
27:24 SC_AWB_WS_CW_W_0_6 [0.4.0] Weighting value at point (0, 6)
23:20 SC_AWB_WS_CW_W_0_5 [0.4.0] Weighting value at point (0, 5)
19:16 SC_AWB_WS_CW_W_0_4 [0.4.0] Weighting value at point (0, 4)
15:12 SC_AWB_WS_CW_W_0_3 [0.4.0] Weighting value at point (0, 3)
11:8 SC_AWB_WS_CW_W_0_2 [0.4.0] Weighting value at point (0, 2)
7:4 SC_AWB_WS_CW_W_0_1 [0.4.0] Weighting value at point (0, 1)
3:0 SC_AWB_WS_CW_W_0_0 [0.4.0] Weighting value at point (0, 0)
*/
#define AWB_SC_AWB_WS_CONFIG_CW_0_GRP_0_OFFSET		(0x003C)
#define AWB_SC_AWB_WS_CONFIG_CW_0_GRP_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_0_GRP_0_OFFSET)

/*
31:20 - Reserved
19:16 SC_AWB_WS_CW_W_0_12 [0.4.0] Weighting value at point (0, 12)
15:12 SC_AWB_WS_CW_W_0_11 [0.4.0] Weighting value at point (0, 11)
11:8 SC_AWB_WS_CW_W_0_10 [0.4.0] Weighting value at point (0, 10)
7:4 SC_AWB_WS_CW_W_0_9 [0.4.0] Weighting value at point (0, 9)
3:0 SC_AWB_WS_CW_W_0_8 [0.4.0] Weighting value at point (0, 8) 
*/
#define AWB_SC_AWB_WS_CONFIG_CW_0_GRP_1_OFFSET		(0x0040)
#define AWB_SC_AWB_WS_CONFIG_CW_0_GRP_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_0_GRP_1_OFFSET)

// (1, 0) ~ (1, 12)
#define AWB_SC_AWB_WS_CONFIG_CW_1_GRP_0_OFFSET		(0x0044)
#define AWB_SC_AWB_WS_CONFIG_CW_1_GRP_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_1_GRP_0_OFFSET)
#define AWB_SC_AWB_WS_CONFIG_CW_1_GRP_1_OFFSET		(0x0048)
#define AWB_SC_AWB_WS_CONFIG_CW_1_GRP_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_1_GRP_1_OFFSET)

// (2, 0) ~ (2, 12)
#define AWB_SC_AWB_WS_CONFIG_CW_2_GRP_0_OFFSET		(0x004C)
#define AWB_SC_AWB_WS_CONFIG_CW_2_GRP_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_2_GRP_0_OFFSET)
#define AWB_SC_AWB_WS_CONFIG_CW_2_GRP_1_OFFSET		(0x0050)
#define AWB_SC_AWB_WS_CONFIG_CW_2_GRP_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_2_GRP_1_OFFSET)

// (3, 0) ~ (3, 12)
#define AWB_SC_AWB_WS_CONFIG_CW_3_GRP_0_OFFSET		(0x0054)
#define AWB_SC_AWB_WS_CONFIG_CW_3_GRP_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_3_GRP_0_OFFSET)
#define AWB_SC_AWB_WS_CONFIG_CW_3_GRP_1_OFFSET		(0x0058)
#define AWB_SC_AWB_WS_CONFIG_CW_3_GRP_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_3_GRP_1_OFFSET)

// (4, 0) ~ (4, 12)
#define AWB_SC_AWB_WS_CONFIG_CW_4_GRP_0_OFFSET		(0x005C)
#define AWB_SC_AWB_WS_CONFIG_CW_4_GRP_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_4_GRP_0_OFFSET)
#define AWB_SC_AWB_WS_CONFIG_CW_4_GRP_1_OFFSET		(0x0060)
#define AWB_SC_AWB_WS_CONFIG_CW_4_GRP_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_4_GRP_1_OFFSET)

// (5, 0) ~ (5, 12)
#define AWB_SC_AWB_WS_CONFIG_CW_5_GRP_0_OFFSET		(0x0064)
#define AWB_SC_AWB_WS_CONFIG_CW_5_GRP_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_5_GRP_0_OFFSET)
#define AWB_SC_AWB_WS_CONFIG_CW_5_GRP_1_OFFSET		(0x0068)
#define AWB_SC_AWB_WS_CONFIG_CW_5_GRP_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_5_GRP_1_OFFSET)

// (6, 0) ~ (6, 12)
#define AWB_SC_AWB_WS_CONFIG_CW_6_GRP_0_OFFSET		(0x006C)
#define AWB_SC_AWB_WS_CONFIG_CW_6_GRP_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_6_GRP_0_OFFSET)
#define AWB_SC_AWB_WS_CONFIG_CW_6_GRP_1_OFFSET		(0x0070)
#define AWB_SC_AWB_WS_CONFIG_CW_6_GRP_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_6_GRP_1_OFFSET)

// (7, 0) ~ (7, 12)
#define AWB_SC_AWB_WS_CONFIG_CW_7_GRP_0_OFFSET		(0x0074)
#define AWB_SC_AWB_WS_CONFIG_CW_7_GRP_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_7_GRP_0_OFFSET)
#define AWB_SC_AWB_WS_CONFIG_CW_7_GRP_1_OFFSET		(0x0078)
#define AWB_SC_AWB_WS_CONFIG_CW_7_GRP_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_7_GRP_1_OFFSET)

// (8, 0) ~ (8, 12)
#define AWB_SC_AWB_WS_CONFIG_CW_8_GRP_0_OFFSET		(0x007C)
#define AWB_SC_AWB_WS_CONFIG_CW_8_GRP_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_8_GRP_0_OFFSET)
#define AWB_SC_AWB_WS_CONFIG_CW_8_GRP_1_OFFSET		(0x0080)
#define AWB_SC_AWB_WS_CONFIG_CW_8_GRP_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_8_GRP_1_OFFSET)

// (9, 0) ~ (9, 12)
#define AWB_SC_AWB_WS_CONFIG_CW_9_GRP_0_OFFSET		(0x0084)
#define AWB_SC_AWB_WS_CONFIG_CW_9_GRP_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_9_GRP_0_OFFSET
#define AWB_SC_AWB_WS_CONFIG_CW_9_GRP_1_OFFSET		(0x0088)
#define AWB_SC_AWB_WS_CONFIG_CW_9_GRP_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_9_GRP_1_OFFSET)

// (10, 0) ~ (10, 12)
#define AWB_SC_AWB_WS_CONFIG_CW_10_GRP_0_OFFSET		(0x008C)
#define AWB_SC_AWB_WS_CONFIG_CW_10_GRP_0			(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_10_GRP_0_OFFSET)
#define AWB_SC_AWB_WS_CONFIG_CW_10_GRP_1_OFFSET		(0x0090)
#define AWB_SC_AWB_WS_CONFIG_CW_10_GRP_1			(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_10_GRP_1_OFFSET)

// (11, 0) ~ (11, 12)
#define AWB_SC_AWB_WS_CONFIG_CW_11_GRP_0_OFFSET		(0x0094)
#define AWB_SC_AWB_WS_CONFIG_CW_11_GRP_0			(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_11_GRP_0_OFFSET)
#define AWB_SC_AWB_WS_CONFIG_CW_11_GRP_1_OFFSET		(0x0098)
#define AWB_SC_AWB_WS_CONFIG_CW_11_GRP_1			(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_11_GRP_1_OFFSET)

// (12, 0) ~ (12, 12)
#define AWB_SC_AWB_WS_CONFIG_CW_12_GRP_0_OFFSET		(0x009C)
#define AWB_SC_AWB_WS_CONFIG_CW_12_GRP_0			(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_12_GRP_0_OFFSET)
#define AWB_SC_AWB_WS_CONFIG_CW_12_GRP_1_OFFSET		(0x00A0)
#define AWB_SC_AWB_WS_CONFIG_CW_12_GRP_1			(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_CW_12_GRP_1_OFFSET)


//v 0 8 15 15 15 15 15 15 15 15 15 15 15 14 10 6
/*
31:28 SC_AWB_WS_IW_V_7 [0.4.0] Weighting value at point (7)
27:24 SC_AWB_WS_IW_V_6 [0.4.0] Weighting value at point (6)
23:20 SC_AWB_WS_IW_V_5 [0.4.0] Weighting value at point (5)
19:16 SC_AWB_WS_IW_V_4 [0.4.0] Weighting value at point (4)
15:12 SC_AWB_WS_IW_V_3 [0.4.0] Weighting value at point (3)
11:8 SC_AWB_WS_IW_V_2 [0.4.0] Weighting value at point (2)
7:4 SC_AWB_WS_IW_V_1 [0.4.0] Weighting value at point (1)
3:0 SC_AWB_WS_IW_V_0 [0.4.0] Weighting value at point (0)
*/
#define AWB_SC_AWB_WS_CONFIG_IW_V_GRP_0_OFFSET		(0x00A4)
#define AWB_SC_AWB_WS_CONFIG_IW_V_GRP_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_IW_V_GRP_0_OFFSET)

/*
31:28 SC_AWB_WS_IW_V_15 [0.4.0] Weighting value at point (15)
27:24 SC_AWB_WS_IW_V_14 [0.4.0] Weighting value at point (14)
23:20 SC_AWB_WS_IW_V_13 [0.4.0] Weighting value at point (13)
19:16 SC_AWB_WS_IW_V_12 [0.4.0] Weighting value at point (12)
15:12 SC_AWB_WS_IW_V_11 [0.4.0] Weighting value at point (11)
11:8 SC_AWB_WS_IW_V_10 [0.4.0] Weighting value at point (10)
7:4 SC_AWB_WS_IW_V_9 [0.4.0] Weighting value at point (9)
3:0 SC_AWB_WS_IW_V_8 [0.4.0] Weighting value at point (8)
*/
#define AWB_SC_AWB_WS_CONFIG_IW_V_GRP_1_OFFSET		(0x00A8)
#define AWB_SC_AWB_WS_CONFIG_IW_V_GRP_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_IW_V_GRP_1_OFFSET)



//s 0.5 0.44 0 0 0 0 0 0 0 0 0 0 -0.07 -0.25 -0.25 -0.25
/*
31:24 SC_AWB_WS_IW_S_3 [1.2.5] Slope value at point (3)
23:16 SC_AWB_WS_IW_S_2 [1.2.5] Slope value at point (2)
15:8 SC_AWB_WS_IW_S_1 [1.2.5] Slope value at point (1)
7:0 SC_AWB_WS_IW_S_0 [1.2.5] Slope value at point (0)
*/
#define AWB_SC_AWB_WS_CONFIG_IW_S_GRP_0_OFFSET		(0x00AC)
#define AWB_SC_AWB_WS_CONFIG_IW_S_GRP_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_IW_S_GRP_0_OFFSET)

/*
31:24 SC_AWB_WS_IW_S_7 [1.2.5] Slope value at point (7)
23:16 SC_AWB_WS_IW_S_6 [1.2.5] Slope value at point (6)
15:8 SC_AWB_WS_IW_S_5 [1.2.5] Slope value at point (5)
7:0 SC_AWB_WS_IW_S_4 [1.2.5] Slope value at point (4)
*/
#define AWB_SC_AWB_WS_CONFIG_IW_S_GRP_1_OFFSET		(0x00B0)
#define AWB_SC_AWB_WS_CONFIG_IW_S_GRP_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_IW_S_GRP_1_OFFSET)

/*
31:24 SC_AWB_WS_IW_S_11 [1.2.5] Slope value at point (11)
23:16 SC_AWB_WS_IW_S_10 [1.2.5] Slope value at point (10)
15:8 SC_AWB_WS_IW_S_9 [1.2.5] Slope value at point (9)
7:0 SC_AWB_WS_IW_S_8 [1.2.5] Slope value at point (8)
*/
#define AWB_SC_AWB_WS_CONFIG_IW_S_GRP_2_OFFSET		(0x00B4)
#define AWB_SC_AWB_WS_CONFIG_IW_S_GRP_2				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_IW_S_GRP_2_OFFSET)

/*
31:24 SC_AWB_WS_IW_S_15 [1.2.5] Slope value at point (15)
23:16 SC_AWB_WS_IW_S_14 [1.2.5] Slope value at point (14)
15:8 SC_AWB_WS_IW_S_13 [1.2.5] Slope value at point (13)
7:0 SC_AWB_WS_IW_S_12 [1.2.5] Slope value at point (12)
*/
#define AWB_SC_AWB_WS_CONFIG_IW_S_GRP_3_OFFSET		(0x00B8)
#define AWB_SC_AWB_WS_CONFIG_IW_S_GRP_3				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_IW_S_GRP_3_OFFSET)


#define AWB_SC_AWB_WS_CONFIG_0_OFFSET		(0x00BC)
#define AWB_SC_AWB_WS_CONFIG_0				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_0_OFFSET)


#define AWB_SC_AWB_WS_CONFIG_1_OFFSET		(0x00C0)
#define AWB_SC_AWB_WS_CONFIG_1				(AWB_BASE_ADDR + AWB_SC_AWB_WS_CONFIG_1_OFFSET)



#if defined(__cplusplus)
}
#endif


#endif //__HW_AWB_REG_H__

