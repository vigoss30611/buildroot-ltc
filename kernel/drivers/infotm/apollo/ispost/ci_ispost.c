/*!
******************************************************************************
 @file   : ispost.c

 @brief	 ispost entry src file

 @Author godspeed.kuo Infotmic

 @date   01/27/2015

 <b>Description:</b>\n


 <b>Platform:</b>\n
	     Platform Independent

 @Version
	     1.0

******************************************************************************/

//#define IMG_KERNEL_MODULE
#define INFOTM_ENABLE_ISPOST_DEBUG

#ifdef IMG_KERNEL_MODULE
#include <linux/kernel.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <mach/clock.h>
#include <mach/power-gate.h>
#include <mach/imap-iomap.h>
#include <mach/imap-ispost.h>
#include "ci_ispost.h"
#include "ci_ispost_reg.h"
#else
#include <common.h>
#include <asm/io.h>
#include "drv_ispost.h"
#endif //IMG_KERNEL_MODULE



#if !defined(IMG_KERNEL_MODULE)
#define ENABLE_CHECK_REG_WRITE_VALUE
#endif //IMG_KERNEL_MODULE

const ISPOST_UINT32 g_dwGridSize[] = { 8, 16, 32 };
ISPOST_UINT32 g_ui32RegAddr;
ISPOST_UINT32 g_ui32RegExpectedData;
ISPOST_UINT32 g_ui32RegData;
ISPOST_UINT32 g_ui32Temp;
#if defined(IMG_KERNEL_MODULE)
static void __iomem *g_ioIspostRegVirtBaseAddr = NULL;
static struct clk *g_busClk = NULL;
#endif //IMG_KERNEL_MODULE


#if defined(IMG_KERNEL_MODULE)
	#define ISPOST_READL(RegAddr)                                               \
		readl(g_ioIspostRegVirtBaseAddr + RegAddr)
	#define ISPOST_WRITEL(RegData, RegAddr)                                     \
		writel(RegData, g_ioIspostRegVirtBaseAddr + RegAddr)
#else
	#define ISPOST_READL(RegAddr)                                               \
		readl(RegAddr)
	#define ISPOST_WRITEL(RegData, RegAddr)                                     \
		writel(RegData, RegAddr)
#endif //IMG_KERNEL_MODULE

#if defined(ENABLE_CHECK_REG_WRITE_VALUE)
#define CHECK_REG_WRITE_VALUE(RegAddr, RegExpectedData, ErrCnt)             \
    {                                                                       \
        if ((g_ui32Temp = ISPOST_READL(RegAddr)) != RegExpectedData) {      \
            if (0 == ErrCnt) {                                              \
                /* store first failure. */                                  \
                g_ui32RegAddr = RegAddr;                                    \
                g_ui32RegExpectedData = RegExpectedData;                    \
                g_ui32RegData = g_ui32Temp;                                 \
            }                                                               \
            ipsost_print_reg_error(RegAddr, RegExpectedData, g_ui32Temp);   \
            /* increment error cnt */                                       \
            ErrCnt++;                                                       \
        }                                                                   \
    }
#else
#define CHECK_REG_WRITE_VALUE(RegAddr, RegExpectedData, ErrCnt)
#endif

#define CHECK_REG_WRITE_VALUE_WITH_MASK(RegAddr, RegExpectedData, ErrCnt, Mask) \
    {                                                                       \
        if (RegExpectedData != ((g_ui32Temp = ISPOST_READL(RegAddr)) & Mask)) {  \
            if (0 == ErrCnt) {                                              \
                /* store first failure. */                                  \
                g_ui32RegAddr = RegAddr;                                    \
                g_ui32RegExpectedData = RegExpectedData;                    \
                g_ui32RegData = g_ui32Temp;                                 \
            }                                                               \
            ipsost_print_reg_error(RegAddr, RegExpectedData, g_ui32Temp);   \
            /* increment error cnt */                                       \
            ErrCnt++;                                                       \
        }                                                                   \
    }


void
ipsost_print_reg_error(
	ISPOST_UINT32 ui32RegAddr,
	ISPOST_UINT32 ui32RegExpectedData,
	ISPOST_UINT32 ui32RegData
	)
{
	ISPOST_LOG("ISPOST register read error addr=%x data=%x expected=%x\n", ui32RegAddr, ui32RegData, ui32RegExpectedData);
}

ISPOST_RESULT
ispost_module_enable(
	void
	)
{
#if defined(IMG_KERNEL_MODULE)
	struct clk * busclk;
	ISPOST_UINT32 ui32ClkRate;
	ISPOST_UINT32 test_y_value;
	int ret;

//	busclk = clk_get_sys("imap-ispost", "imap-ispost");
//	ui32ClkRate = 300000000;
//	int ret = clk_set_rate(busclk, ui32ClkRate);
//	if (0 > ret) {
//		ISPOST_LOG("failed to set imap ispost clock\n");
//	} else {
//		clk_prepare_enable(busclk);
//	}
//	ISPOST_LOG("ispost clock -> %d\n", ui32ClkRate);
	busclk = clk_get_sys("imap-ispost", "imap-ispost");
	ui32ClkRate = clk_get_rate(busclk);
	ret = clk_prepare_enable(busclk);
	if(ret){
		printk("prepare ispost clock fail!\n");
		clk_disable_unprepare(busclk);
		return -1;
	}
	g_busClk = busclk;
	printk("ispost bus clock -> %d\n",ui32ClkRate);
	module_power_on(SYSMGR_ISPOST_BASE);
	module_reset(SYSMGR_ISPOST_BASE, 0);
	module_reset(SYSMGR_ISPOST_BASE, 1);
	g_ioIspostRegVirtBaseAddr = ioremap(IMAP_ISPOST_BASE, IMAP_ISPOST_SIZE - 1);
	printk("@@@ Ispost base address: %p\n", g_ioIspostRegVirtBaseAddr);
	if (NULL == g_ioIspostRegVirtBaseAddr)
		goto ioremap_fail;

	test_y_value = ISPOST_READL(ISPOST_LCSRCAY);
	printk("@@@ bef test_y_value=0x%08x\n", test_y_value);

	ISPOST_WRITEL(0xFFFF, ISPOST_LCSRCAY);

	test_y_value = ISPOST_READL(ISPOST_LCSRCAY);
	printk("@@@ aft test_y_value=0x%08x\n", test_y_value);

	return 0;
ioremap_fail:
	if (g_busClk) {
		clk_disable_unprepare(busclk);
		g_busClk = NULL;
	}
	return -1;
#else
	int clk;

	//module_set_clock("ispost", "dpll", 400, 0);
	//clk =  module_get_clock("ispost");
	//ISPOST_LOG("ispost clock -> %d\n", clk);
	clk =  module_get_clock("bus3");	// what is the ISPOST bus?
	ISPOST_LOG("ispost bus clock -> %d\n", clk);
	module_enable("ispost");				// where is this module_enable srource? does it need to be modified for ispost?
	//module_reset("ispost");					// where is this module_reset source? does it need to be modified for ispost?
	return 0;
#endif //IMG_KERNEL_MODULE
}

void
ispost_module_disable(
	void
	)
{
#if defined(IMG_KERNEL_MODULE)
	//struct clk * busclk;

	if (g_ioIspostRegVirtBaseAddr) {
		iounmap(g_ioIspostRegVirtBaseAddr);
		g_ioIspostRegVirtBaseAddr = NULL;
	}
	if (g_busClk) {
		clk_disable_unprepare(g_busClk);
		g_busClk = NULL;
	}
	module_power_down(SYSMGR_ISPOST_BASE);
	//busclk = clk_get_sys("imap-ispost", "imap-ispost");
	//if (busclk) {
	//	clk_disable_unprepare(host->busclk);
	//}
#else
	module_disable("ispost");				// where is this module_disable srource? does it need to be modified for ispost?
#endif //IMG_KERNEL_MODULE
}

ISPOST_RESULT
IspostLcSrcImgAddrSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup source image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_LCSRCAY);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCAY, ui32YAddr, ui32Ret);

	// Setup source image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_LCSRCAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCAUV, ui32UVAddr, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcSrcImgSizeSet(
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup source image size.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCSRCSZ, W, (ISPOST_UINT32)ui16Width);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCSRCSZ, H, (ISPOST_UINT32)ui16Height);
	ISPOST_WRITEL(ui32Temp, ISPOST_LCSRCSZ);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCSZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcSrcImgSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride,
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup source image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_LCSRCAY);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCAY, ui32YAddr, ui32Ret);

	// Setup source image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_LCSRCAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCAUV, ui32UVAddr, ui32Ret);

	// Setup source image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_LCSRCS);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCS, (ISPOST_UINT32)ui16Stride, ui32Ret);

	// Setup source image size.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCSRCSZ, W, (ISPOST_UINT32)ui16Width);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCSRCSZ, H, (ISPOST_UINT32)ui16Height);
	ISPOST_WRITEL(ui32Temp, ISPOST_LCSRCSZ);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCSZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcBackFillColorSet(
	ISPOST_UINT8 ui8BackFillY,
	ISPOST_UINT8 ui8BackFillCB,
	ISPOST_UINT8 ui8BackFillCR
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup source image back fill color.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCSRCBFC, Y, (ISPOST_UINT32)ui8BackFillY);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCSRCBFC, CB, (ISPOST_UINT32)ui8BackFillCB);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCSRCBFC, CR, (ISPOST_UINT32)ui8BackFillCR);
	ISPOST_WRITEL(ui32Temp, ISPOST_LCSRCBFC);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCBFC, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_UINT32
IspostLcGridBufStrideCalc(
	ISPOST_UINT16 ui16DstWidth,
	ISPOST_UINT8 ui8Size
	)
{
	ISPOST_UINT32 ui32Stride;
	ISPOST_UINT32 ui32GridSize;
	ISPOST_UINT32 ui32Width;

	// Setup LC grid buffer stride.
	if (GRID_SIZE_MAX <= ui8Size) {
		ui8Size = GRID_SIZE_8_8;
	}
	ui32GridSize = g_dwGridSize[ui8Size];
	ui32Width = (ISPOST_UINT32)ui16DstWidth / ui32GridSize;
	if ((ISPOST_UINT32)ui16DstWidth > (ui32Width * ui32GridSize)) {
		ui32Width += 2;
	} else {
		ui32Width++;
	}
	ui32Stride = ui32Width * 16;

	return (ui32Stride);
}

ISPOST_UINT32
IspostLcGridBufLengthCalc(
	ISPOST_UINT16 ui16DstHeight,
	ISPOST_UINT32 ui32Stride,
	ISPOST_UINT8 ui8Size
	)
{
	ISPOST_UINT32 ui32GridImgBufLen;
	ISPOST_UINT32 ui32GridSize;
	ISPOST_UINT32 ui32Height;

	// Setup LC grid buffer stride.
	if (GRID_SIZE_MAX <= ui8Size) {
		ui8Size = GRID_SIZE_8_8;
	}
	ui32GridSize = g_dwGridSize[ui8Size];
	ui32Height = (ISPOST_UINT32)ui16DstHeight / ui32GridSize;
	if ((ISPOST_UINT32)ui16DstHeight > (ui32Height * ui32GridSize)) {
		ui32Height += 2;
	} else {
		ui32Height++;
	}
	ui32GridImgBufLen = ui32Height * ui32Stride;

	return (ui32GridImgBufLen);
}

ISPOST_RESULT
IspostLcGridBufSetWithStride(
	ISPOST_UINT32 ui32BufAddr,
	ISPOST_UINT16 ui16Stride,
	ISPOST_UINT8 ui8LineBufEnable,
	ISPOST_UINT8 ui8Size
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup LC grid buffer start address.
	ISPOST_WRITEL(ui32BufAddr, ISPOST_LCGBA);
	CHECK_REG_WRITE_VALUE(ISPOST_LCGBA, ui32BufAddr, ui32Ret);

	// Setup LC grid buffer stride.
	ISPOST_WRITEL((ISPOST_UINT32)(ui16Stride), ISPOST_LCGBS);
	CHECK_REG_WRITE_VALUE(ISPOST_LCGBS, (ISPOST_UINT32)(ui16Stride), ui32Ret);

	// Setup LC grid size.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCGBSZ, SIZE, (ISPOST_UINT32)ui8Size);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCGBSZ, LBEN, (ISPOST_UINT32)ui8LineBufEnable);
	ISPOST_WRITEL(ui32Temp, ISPOST_LCGBSZ);
	CHECK_REG_WRITE_VALUE(ISPOST_LCGBSZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcGridBufSetWithDestSize(
	ISPOST_UINT32 ui32BufAddr,
	ISPOST_UINT16 ui16DstWidth,
	ISPOST_UINT16 ui16DstHeight,
	ISPOST_UINT8 ui8Size
	)
{
	ISPOST_UINT32 ui32Stride;
//	ISPOST_UINT32 ui32GridImgBufLen;
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;
//	ISPOST_UINT32 ui32GridSize;
//	ISPOST_UINT32 ui32Width;
//	ISPOST_UINT32 ui32Height;

	// Setup LC grid buffer start address.
	ISPOST_WRITEL(ui32BufAddr, ISPOST_LCGBA);
	CHECK_REG_WRITE_VALUE(ISPOST_LCGBA, ui32BufAddr, ui32Ret);

	// Setup LC grid buffer stride.
//	if (GRID_SIZE_MAX <= ui8Size) {
//		ui8Size = GRID_SIZE_8_8;
//	}
//	ui32GridSize = g_dwGridSize[ui8Size];
//	ui32Width = (ISPOST_UINT32)ui16DstWidth / ui32GridSize;
//	if ((ISPOST_UINT32)ui16DstWidth > (ui32Width * ui32GridSize)) {
//		ui32Width += 2;
//	} else {
//		ui32Width++;
//	}
//	ui32Stride = ui32Width * 16;
	ui32Stride = IspostLcGridBufStrideCalc(ui16DstWidth, ui8Size);
//	ui32Height = (ISPOST_UINT32)ui16DstHeight / ui32GridSize;
//	if ((ISPOST_UINT32)ui16DstHeight > (ui32Height * ui32GridSize)) {
//		ui32Height += 2;
//	} else {
//		ui32Height++;
//	}
//	//ui32GridImgBufLen = ui32Height * ui32Stride;
//	ui32GridImgBufLen = IspostLcGridBufLengthCalc(ui16DstHeight, ui32Stride, ui8Size);
	ISPOST_WRITEL((ISPOST_UINT32)(ui32Stride), ISPOST_LCGBS);
	CHECK_REG_WRITE_VALUE(ISPOST_LCGBS, (ISPOST_UINT32)(ui32Stride), ui32Ret);

	// Setup LC grid size.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCGBSZ, SIZE, (ISPOST_UINT32)ui8Size);
	ISPOST_WRITEL(ui32Temp, ISPOST_LCGBSZ);
	CHECK_REG_WRITE_VALUE(ISPOST_LCGBSZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcPxlCacheModeSet(
	ISPOST_UINT8 ui8CtrlCfg,
	ISPOST_UINT8 ui8CBCRSwap
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup LC pixel cache mode.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCPCM, PREF, (ISPOST_UINT32)ui8CtrlCfg);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCPCM, CBCRSWAP, (ISPOST_UINT32)ui8CBCRSwap);
	ISPOST_WRITEL(ui32Temp, ISPOST_LCPCM);
	CHECK_REG_WRITE_VALUE(ISPOST_LCPCM, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcPxlScanModeSet(
	ISPOST_UINT8 ui8ScanW,
	ISPOST_UINT8 ui8ScanH,
	ISPOST_UINT8 ui8ScanM
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup LC pixel coordinator generator mode.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCPGM, SCANW, (ISPOST_UINT32)ui8ScanW);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCPGM, SCANH, (ISPOST_UINT32)ui8ScanH);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCPGM, SCANM, (ISPOST_UINT32)ui8ScanM);
	ISPOST_WRITEL(ui32Temp, ISPOST_LCPGM);
	CHECK_REG_WRITE_VALUE(ISPOST_LCPGM, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcPxlDstSizeSet(
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup LC pixel coordinator destination size.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCPGSZ, W, (ISPOST_UINT32)ui16Width);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCPGSZ, H, (ISPOST_UINT32)ui16Height);
	ISPOST_WRITEL(ui32Temp, ISPOST_LCPGSZ);
	CHECK_REG_WRITE_VALUE(ISPOST_LCPGSZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcAxiIdSet(
	ISPOST_UINT8 ui8SrcReqLmt,
	ISPOST_UINT8 ui8GBID,
	ISPOST_UINT8 ui8SRCID
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup LC AXI Read Request Limit, GBID and SRCID.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCAXI, SRCRDL, (ISPOST_UINT32)ui8SrcReqLmt);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCAXI, GBID, (ISPOST_UINT32)ui8GBID);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCAXI, SRCID, (ISPOST_UINT32)ui8SRCID);
	ISPOST_WRITEL(ui32Temp, ISPOST_LCAXI);
	CHECK_REG_WRITE_VALUE(ISPOST_LCAXI, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLenCorrectionSet(
	LC_INFO stLcInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	//
	// Setup LC SRC addr, stride and size.
	// ============================
	//
	ui32Ret += IspostLcSrcImgSet(
		stLcInfo.stSrcImgInfo.ui32YAddr,
		stLcInfo.stSrcImgInfo.ui32UVAddr,
		stLcInfo.stSrcImgInfo.ui16Stride,
		stLcInfo.stSrcImgInfo.ui16Width,
		stLcInfo.stSrcImgInfo.ui16Height
		);

	//
	// Setup LC grid address, stride and grid size
	// ============================
	//
	ui32Ret += IspostLcGridBufSetWithStride(
		stLcInfo.stGridBufInfo.ui32BufAddr,
		stLcInfo.stGridBufInfo.ui16Stride,
		stLcInfo.stGridBufInfo.ui8LineBufEnable,
		stLcInfo.stGridBufInfo.ui8Size
		);

	//
	// Setup LC pixel cache mode
	// ============================
	//
	ui32Ret += IspostLcPxlCacheModeSet(
		stLcInfo.stPxlCacheMode.ui8CtrlCfg,
		stLcInfo.stPxlCacheMode.ui8CBCRSwap
		);

	//
	// Setup LC pixel coordinator generator mode.
	// ============================
	//
	ui32Ret += IspostLcPxlScanModeSet(
		stLcInfo.stPxlScanMode.ui8ScanW,
		stLcInfo.stPxlScanMode.ui8ScanH,
		stLcInfo.stPxlScanMode.ui8ScanM
		);

	//
	// Setup LC pixel coordinator destination size.
	// ============================
	//
	ui32Ret += IspostLcPxlDstSizeSet(
		stLcInfo.stPxlDstSize.ui16Width,
		stLcInfo.stPxlDstSize.ui16Height
		);

	//
	// Setup LC AXI Read Request Limit, GBID and SRCID.
	// ============================
	//
	ui32Ret += IspostLcAxiIdSet(
		stLcInfo.stAxiId.ui8SrcReqLmt,
		stLcInfo.stAxiId.ui8GBID,
		stLcInfo.stAxiId.ui8SRCID
		);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOvImgAddrSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	)
{
	if ((0x00000000 != ui32YAddr) && (0x00000000 != ui32UVAddr)) {
		// Setup overlay image Y plane start address.
		ISPOST_WRITEL(ui32YAddr, ISPOST_OVRIAY);
		CHECK_REG_WRITE_VALUE(ISPOST_OVRIAY, ui32YAddr, ui32Ret);

		// Setup overlay image UV plane start address.
		ISPOST_WRITEL(ui32UVAddr, ISPOST_OVRIAUV);
		CHECK_REG_WRITE_VALUE(ISPOST_OVRIAUV, ui32UVAddr, ui32Ret);
		return ISPOST_EXIT_SUCCESS;
	}

	return ISPOST_EXIT_FAILURE;
}

ISPOST_RESULT
IspostOvImgSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride,
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	if ((0x00000000 != ui32YAddr) && (0x00000000 != ui32UVAddr)) {
		// Setup overlay image Y plane start address.
		ISPOST_WRITEL(ui32YAddr, ISPOST_OVRIAY);
		CHECK_REG_WRITE_VALUE(ISPOST_OVRIAY, ui32YAddr, ui32Ret);

		// Setup overlay image UV plane start address.
		ISPOST_WRITEL(ui32UVAddr, ISPOST_OVRIAUV);
		CHECK_REG_WRITE_VALUE(ISPOST_OVRIAUV, ui32UVAddr, ui32Ret);
	}

	// Setup overlay image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_OVRIS);
	CHECK_REG_WRITE_VALUE(ISPOST_OVRIS, (ISPOST_UINT32)ui16Stride, ui32Ret);

	// Setup overlay image size.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVISZ, W, (ISPOST_UINT32)ui16Width);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVISZ, H, (ISPOST_UINT32)ui16Height);
	ISPOST_WRITEL(ui32Temp, ISPOST_OVISZ);
	CHECK_REG_WRITE_VALUE(ISPOST_OVISZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOvModeSet(
	ISPOST_UINT8 ui8OverlayMode
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup overlay mode.
	ISPOST_WRITEL((ISPOST_UINT32)ui8OverlayMode, ISPOST_OVM);
	CHECK_REG_WRITE_VALUE(ISPOST_OVM, (ISPOST_UINT32)ui8OverlayMode, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOvOffsetSet(
	ISPOST_UINT16 ui16X,
	ISPOST_UINT16 ui16Y
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup overlay offset.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVOFF, X, (ISPOST_UINT32)ui16X);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVOFF, Y, (ISPOST_UINT32)ui16Y);
	ISPOST_WRITEL(ui32Temp, ISPOST_OVOFF);
	CHECK_REG_WRITE_VALUE(ISPOST_OVOFF, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOvAlphaValueSet(
	OV_ALPHA_VALUE stValue0,
	OV_ALPHA_VALUE stValue1,
	OV_ALPHA_VALUE stValue2,
	OV_ALPHA_VALUE stValue3
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup overlay alpha value 0.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA0, ALPHA3, (ISPOST_UINT32)stValue0.ui8Alpha3);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA0, ALPHA2, (ISPOST_UINT32)stValue0.ui8Alpha2);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA0, ALPHA1, (ISPOST_UINT32)stValue0.ui8Alpha1);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA0, ALPHA0, (ISPOST_UINT32)stValue0.ui8Alpha0);
	ISPOST_WRITEL(ui32Temp, ISPOST_OVALPHA0);
	CHECK_REG_WRITE_VALUE(ISPOST_OVALPHA0, ui32Temp, ui32Ret);

	// Setup overlay alpha value 1.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA1, ALPHA7, (ISPOST_UINT32)stValue1.ui8Alpha3);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA1, ALPHA6, (ISPOST_UINT32)stValue1.ui8Alpha2);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA1, ALPHA5, (ISPOST_UINT32)stValue1.ui8Alpha1);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA1, ALPHA4, (ISPOST_UINT32)stValue1.ui8Alpha0);
	ISPOST_WRITEL(ui32Temp, ISPOST_OVALPHA1);
	CHECK_REG_WRITE_VALUE(ISPOST_OVALPHA1, ui32Temp, ui32Ret);

	// Setup overlay alpha value 2.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA2, ALPHA11, (ISPOST_UINT32)stValue2.ui8Alpha3);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA2, ALPHA10, (ISPOST_UINT32)stValue2.ui8Alpha2);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA2, ALPHA9, (ISPOST_UINT32)stValue2.ui8Alpha1);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA2, ALPHA8, (ISPOST_UINT32)stValue2.ui8Alpha0);
	ISPOST_WRITEL(ui32Temp, ISPOST_OVALPHA2);
	CHECK_REG_WRITE_VALUE(ISPOST_OVALPHA2, ui32Temp, ui32Ret);

	// Setup overlay alpha value 3.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA3, ALPHA15, (ISPOST_UINT32)stValue3.ui8Alpha3);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA3, ALPHA14, (ISPOST_UINT32)stValue3.ui8Alpha2);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA3, ALPHA13, (ISPOST_UINT32)stValue3.ui8Alpha1);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA3, ALPHA12, (ISPOST_UINT32)stValue3.ui8Alpha0);
	ISPOST_WRITEL(ui32Temp, ISPOST_OVALPHA3);
	CHECK_REG_WRITE_VALUE(ISPOST_OVALPHA3, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOvAxiIdSet(
	ISPOST_UINT8 ui8OVID
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup overlay AXI read ID.
	ISPOST_WRITEL((ISPOST_UINT32)ui8OVID, ISPOST_OVAXI);
	CHECK_REG_WRITE_VALUE(ISPOST_OVAXI, (ISPOST_UINT32)ui8OVID, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOverlayAlphaValueSet(
	OV_ALPHA stOvAlpha
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup OV overlay alpha value.
	ui32Ret += IspostOvAlphaValueSet(
		stOvAlpha.stValue0,
		stOvAlpha.stValue1,
		stOvAlpha.stValue2,
		stOvAlpha.stValue3
		);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOverlaySet(
	OV_INFO stOvInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup OV overlay image information.
	ui32Ret += IspostOvImgSet(
		stOvInfo.stOvImgInfo.ui32YAddr,
		stOvInfo.stOvImgInfo.ui32UVAddr,
		stOvInfo.stOvImgInfo.ui16Stride,
		stOvInfo.stOvImgInfo.ui16Width,
		stOvInfo.stOvImgInfo.ui16Height
		);

	// Setup OV Set overlay mode.
	ui32Ret += IspostOvModeSet(
		stOvInfo.ui8OverlayMode
		);

	// Setup OV overlay offset.
	ui32Ret += IspostOvOffsetSet(
		stOvInfo.stOvOffset.ui16X,
		stOvInfo.stOvOffset.ui16Y
		);

	// Setup OV AXI read ID.
	ui32Ret += IspostOvAxiIdSet(
		stOvInfo.ui8OVID
		);

	return (ui32Ret);
}

ISPOST_RESULT
IspostDnRefImgAddrSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup input image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_DNRIAY);
	CHECK_REG_WRITE_VALUE(ISPOST_DNRIAY, ui32YAddr, ui32Ret);

	// Setup input image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_DNRIAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_DNRIAUV, ui32UVAddr, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostDnRefImgSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	if ((0x00000000 != ui32YAddr) && (0x00000000 != ui32UVAddr)) {
		// Setup input image Y plane start address.
		ISPOST_WRITEL(ui32YAddr, ISPOST_DNRIAY);
		CHECK_REG_WRITE_VALUE(ISPOST_DNRIAY, ui32YAddr, ui32Ret);

		// Setup input image UV plane start address.
		ISPOST_WRITEL(ui32UVAddr, ISPOST_DNRIAUV);
		CHECK_REG_WRITE_VALUE(ISPOST_DNRIAUV, ui32UVAddr, ui32Ret);
	}

	// Setup input image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_DNRIS);
	CHECK_REG_WRITE_VALUE(ISPOST_DNRIS, (ISPOST_UINT32)ui16Stride, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostDnOutImgAddrSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup input image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_DNROAY);
	CHECK_REG_WRITE_VALUE(ISPOST_DNROAY, ui32YAddr, ui32Ret);

	// Setup input image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_DNROAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_DNROAUV, ui32UVAddr, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostDnOutImgSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup input image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_DNROAY);
	CHECK_REG_WRITE_VALUE(ISPOST_DNROAY, ui32YAddr, ui32Ret);

	// Setup input image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_DNROAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_DNROAUV, ui32UVAddr, ui32Ret);

	// Setup input image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_DNROS);
	CHECK_REG_WRITE_VALUE(ISPOST_DNROS, (ISPOST_UINT32)ui16Stride, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
Ispost3DDenoiseMskCrvSet(
	DN_MSK_CRV_INFO stDnMskCrvInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;
	ISPOST_UINT32 ui32Idx;
	ISPOST_PUINT32 pMskCrvVal;

	// Setup DN mask curve.
	// Setup DN mask curve index.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, DNCTRL0, IDX, (ISPOST_UINT32)stDnMskCrvInfo.ui8MskCrvIdx);
	ISPOST_WRITEL(ui32Temp, ISPOST_DNCTRL0);
	CHECK_REG_WRITE_VALUE(ISPOST_DNCTRL0, ui32Temp, ui32Ret);
	// Setup DN mask curve 00-14 information.

	pMskCrvVal = (ISPOST_PUINT32)&stDnMskCrvInfo.stMskCrv[0];
	for (ui32Idx=0; ui32Idx<DN_MSK_CRV_MAX; ui32Idx++,pMskCrvVal++)
	{
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		//ISPOST_LOG("%02d - Addr = 0x%08X, Data = 0x%08X\n", ui32Idx, (ISPOST_UINT32)pMskCrvVal, (ISPOST_UINT32)*pMskCrvVal);
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		ISPOST_WRITEL(*pMskCrvVal, (ISPOST_DNMC0 + (ui32Idx * 4)));
		CHECK_REG_WRITE_VALUE((ISPOST_DNMC0 + (ui32Idx * 4)), *pMskCrvVal, ui32Ret);
	}

	return (ui32Ret);
}

ISPOST_RESULT
IspostDnAxiIdSet(
	ISPOST_UINT8 ui8RefWrID,
	ISPOST_UINT8 ui8RefRdID
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup DN AXI reference write and read ID.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, DNAXI, REFWID, (ISPOST_UINT32)ui8RefWrID);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, DNAXI, REFRID, (ISPOST_UINT32)ui8RefRdID);
	ISPOST_WRITEL(ui32Temp, ISPOST_DNAXI);
	CHECK_REG_WRITE_VALUE(ISPOST_DNAXI, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
Ispost3DDenoiseSet(
	DN_INFO stDnInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	//
	// Setup DNR reference input image addr, stride.
	// ============================
	//
	ui32Ret += IspostDnRefImgSet(
		stDnInfo.stRefImgInfo.ui32YAddr,
		stDnInfo.stRefImgInfo.ui32UVAddr,
		stDnInfo.stRefImgInfo.ui16Stride
		);

	//
	// Setup DNR output image addr, stride.
	// ============================
	//
	ui32Ret += IspostDnOutImgSet(
		stDnInfo.stOutImgInfo.ui32YAddr,
		stDnInfo.stOutImgInfo.ui32UVAddr,
		stDnInfo.stOutImgInfo.ui16Stride
		);

	// Setup DN AXI reference write and read ID.
	ui32Ret += IspostDnAxiIdSet(
		stDnInfo.stAxiId.ui8RefWrID,
		stDnInfo.stAxiId.ui8RefRdID
		);

	return (ui32Ret);
}

ISPOST_UINT32
IspostScalingFactotCal(
	ISPOST_UINT32 ui32SrcSize,
	ISPOST_UINT32 ui32DstSize
	)
{
	ISPOST_UINT32 DstSizeCal;
	ISPOST_UINT32 ui32SaclingFactor;

	ui32SaclingFactor = (ui32DstSize * ISPOST_SS_SF_MAX) / ui32SrcSize;
	DstSizeCal = (ui32SrcSize * ui32SaclingFactor) / ISPOST_SS_SF_MAX;
	if (DstSizeCal < ui32DstSize) {
		ui32SaclingFactor++;
	}

	return (ui32SaclingFactor);
}

ISPOST_UINT32
IspostScalingDstSizeCal(
	ISPOST_UINT32 ui32SrcSize,
	ISPOST_UINT32 ui32SaclingFactor
	)
{
	ISPOST_UINT32 DstSizeCal;

	DstSizeCal = (ui32SrcSize * ui32SaclingFactor) / ISPOST_SS_SF_MAX;

	return (DstSizeCal);
}

ISPOST_RESULT
IspostSSOutImgAddrSet(
	ISPOST_UINT8 ui8StreamSel,
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32StreamOffset;

	ui32StreamOffset = 0x00000018 * ui8StreamSel;

	// Setup output image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, (ISPOST_SS0AY + ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0AY + ui32StreamOffset), ui32YAddr, ui32Ret);

	// Setup output image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, (ISPOST_SS0AUV + ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0AUV + ui32StreamOffset), ui32UVAddr, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostSSOutImgUpdate(
	ISPOST_UINT8 ui8StreamSel,
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32StreamOffset;

	ui32StreamOffset = 0x00000018 * ui8StreamSel;

	// Setup output image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, (ISPOST_SS0AY + ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0AY + ui32StreamOffset), ui32YAddr, ui32Ret);

	// Setup output image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, (ISPOST_SS0AUV + ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0AUV + ui32StreamOffset), ui32UVAddr, ui32Ret);


	// Setup output image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, (ISPOST_SS0S + ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0S + ui32StreamOffset), (ISPOST_UINT32)ui16Stride, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostSSOutImgSet(
	SS_INFO stSsInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;
	ISPOST_UINT32 ui32StreamOffset;

	ui32StreamOffset = 0x00000018 * stSsInfo.ui8StreamSel;

	// Setup output image Y plane start address.
	ISPOST_WRITEL(stSsInfo.stOutImgInfo.ui32YAddr, (ISPOST_SS0AY+ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0AY + ui32StreamOffset), stSsInfo.stOutImgInfo.ui32YAddr, ui32Ret);

	// Setup output image UV plane start address.
	ISPOST_WRITEL(stSsInfo.stOutImgInfo.ui32UVAddr, (ISPOST_SS0AUV+ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0AUV+ui32StreamOffset), stSsInfo.stOutImgInfo.ui32UVAddr, ui32Ret);

	// Setup output image stride.
	ISPOST_WRITEL((ISPOST_UINT32)stSsInfo.stOutImgInfo.ui16Stride, (ISPOST_SS0S+ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0S+ui32StreamOffset), (ISPOST_UINT32)stSsInfo.stOutImgInfo.ui16Stride, ui32Ret);

	// Setup output image size.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS0IW, W, (ISPOST_UINT32)stSsInfo.stOutImgInfo.ui16Width);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS0IW, H, (ISPOST_UINT32)stSsInfo.stOutImgInfo.ui16Height);
	ISPOST_WRITEL(ui32Temp, (ISPOST_SS0IW+ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0IW+ui32StreamOffset), ui32Temp, ui32Ret);

	// Setup output image H scaling factor.
	ui32Temp = 0;
	if (SCALING_STREAM_0 == stSsInfo.ui8StreamSel) {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS0HF, SF, (ISPOST_UINT32)stSsInfo.stHSF.ui16SF);
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS0HF, SM, (ISPOST_UINT32)stSsInfo.stHSF.ui8SM);
	} else {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS1HF, SF, (ISPOST_UINT32)stSsInfo.stHSF.ui16SF);
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS1HF, SM, (ISPOST_UINT32)stSsInfo.stHSF.ui8SM);
	}
	ISPOST_WRITEL(ui32Temp, (ISPOST_SS0HF+ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0HF+ui32StreamOffset), ui32Temp, ui32Ret);

	// Setup output image V scaling factor.
	ui32Temp = 0;
	if (SCALING_STREAM_0 == stSsInfo.ui8StreamSel) {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS0VF, SF, (ISPOST_UINT32)stSsInfo.stVSF.ui16SF);
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS0VF, SM, (ISPOST_UINT32)stSsInfo.stVSF.ui8SM);
	} else {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS1VF, SF, (ISPOST_UINT32)stSsInfo.stVSF.ui16SF);
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS1VF, SM, (ISPOST_UINT32)stSsInfo.stVSF.ui8SM);
	}
	ISPOST_WRITEL(ui32Temp, (ISPOST_SS0VF+ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0VF+ui32StreamOffset), ui32Temp, ui32Ret);

	// Setup AXI SSxWID.
	ui32Temp = 0;
	ui32Temp = (ISPOST_READL(ISPOST_SSAXI) & 0x0000FFFF);
	if (SCALING_STREAM_0 == stSsInfo.ui8StreamSel) {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SSAXI, SS0WID, (ISPOST_UINT32)stSsInfo.ui8SSWID);
	} else {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SSAXI, SS1WID, (ISPOST_UINT32)stSsInfo.ui8SSWID);
	}
	ISPOST_WRITEL(ui32Temp, ISPOST_SSAXI);
	CHECK_REG_WRITE_VALUE(ISPOST_SSAXI, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostScaleStreamSet(
	SS_INFO stSsInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	//
	// Setup SS output image information..
	// ============================
	//
	ui32Ret += IspostSSOutImgSet(
		stSsInfo
		);

	return (ui32Ret);
}

void
IspostActivateReset(
	void
	)
{
	ISPOST_CTRL0 stCtrl0;

	//stCtrl0 = (ISPOST_CTRL0)ISPOST_READL(ISPOST_ISPCTRL0);
	// Activate reset ispost module.
	stCtrl0.ui32Ctrl0 = 0;
	stCtrl0.ui1Rst = 1;
	ISPOST_WRITEL(stCtrl0.ui32Ctrl0, ISPOST_ISPCTRL0);
}

void
IspostDeActivateReset(
	void
	)
{
	ISPOST_CTRL0 stCtrl0;

	//stCtrl0 = (ISPOST_CTRL0)ISPOST_READL(ISPOST_ISPCTRL0);
	// De-activate ispost module.
	//stCtrl0.ui1Rst = 0;
	stCtrl0.ui32Ctrl0 = 0;
	ISPOST_WRITEL(stCtrl0.ui32Ctrl0, ISPOST_ISPCTRL0);
}

void
IspostSetCtrl(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 uiCtrl0Temp;
	ISPOST_UINT32 ui32Temp;

	uiCtrl0Temp = (ISPOST_READL(ISPOST_ISPCTRL0) & 0x00000003);
	ui32Temp = (stCtrl0.ui32Ctrl0 & 0xfffffffc) | uiCtrl0Temp;
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
}

void
IspostProcessReset(
	void
	)
{
	ISPOST_CTRL0 stCtrl0;

	stCtrl0 = (ISPOST_CTRL0)ISPOST_READL(ISPOST_ISPCTRL0);
	// Activate reset ispost module.
	stCtrl0.ui1Rst = 1;
	ISPOST_WRITEL(stCtrl0.ui32Ctrl0, ISPOST_ISPCTRL0);
	// De-activate ispost module.
	stCtrl0.ui1Rst = 0;
	ISPOST_WRITEL(stCtrl0.ui32Ctrl0, ISPOST_ISPCTRL0);
}

void
IspostProcessDisable(
	void
	)
{

	ISPOST_WRITEL(0x00000000, ISPOST_ISPCTRL0);
}

ISPOST_RESULT
IspostCtrl0Chk(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	CHECK_REG_WRITE_VALUE(ISPOST_ISPCTRL0, stCtrl0.ui32Ctrl0, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostIntChk(
	ISPOST_UINT8 ui8IntOffOn
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	if (ISPOST_OFF == ui8IntOffOn) {
		CHECK_REG_WRITE_VALUE_WITH_MASK(ISPOST_ISPCTRL0, 0x00000000, ui32Ret, ISPOST_ISPCTRL0_INT_MASK);
	} else {
		CHECK_REG_WRITE_VALUE_WITH_MASK(ISPOST_ISPCTRL0, ISPOST_ISPCTRL0_INT_MASK, ui32Ret, ISPOST_ISPCTRL0_INT_MASK);
	}

	return (ui32Ret);
}

ISPOST_CTRL0
IspostInterruptGet(
	void
	)
{
	ISPOST_UINT32 ui32Ctrl;

	ui32Ctrl = ISPOST_READL(ISPOST_ISPCTRL0);
	return ((ISPOST_CTRL0)ui32Ctrl);
}

void
IspostInterruptClear(
	void
	)
{
	ISPOST_UINT32 ui32Ctrl;

	ui32Ctrl = ISPOST_READL(ISPOST_ISPCTRL0);

	ui32Ctrl &= 0xfefffffc;

	ISPOST_WRITEL(ui32Ctrl, ISPOST_ISPCTRL0);
}

ISPOST_STAT0
IspostStatusGet(
	void
	)
{
	ISPOST_UINT32 ui32Status;

	ui32Status = ISPOST_READL(ISPOST_ISPSTAT0);
	return ((ISPOST_STAT0)ui32Status);
	//return ((ui32Status & 0x0000000f) ? (ISPOST_BUSY) : (ISPOST_DONE));
}

void
IspostResetAndSetCtrl(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xfffffffc;
	// Activate reset ispost module.
	ISPOST_WRITEL((ui32Temp | 0x00000002), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
}

void
IspostTrigger(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xfffffffc;
	// Start ispost module porcess.
	ISPOST_WRITEL((ui32Temp | 0x00000001), ISPOST_ISPCTRL0);
}


void
IspostResetSetCtrlAndTrigger(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xfffffffc;
	// Activate reset ispost module.
	ISPOST_WRITEL((ui32Temp | 0x00000002), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
	// Start ispost module porcess.
	ISPOST_WRITEL((ui32Temp | 0x00000001), ISPOST_ISPCTRL0);
}

//void
//IspostDumpImage(
//	ISPOST_PVOID pDumpBuf,
//	ISPOST_UINT32 ui32DumpSize,
//	ISPOST_UINT8 ui8FileID
//	)
//{
//
//	// Dumnp image to memory.
//	// Set starting address.
//	ISPOST_WRITEL((ISPOST_UINT32)(pDumpBuf), ISPOST_DUMPA);
//	//  Setup size.
//	ISPOST_WRITEL(ui32DumpSize, ISPOST_DUMPSZ);
//	// Setup file id.
//	ISPOST_WRITEL((ISPOST_UINT32)ui8FileID, ISPOST_DUMPFID);
//	// Setup dump trigger on.
//	ISPOST_WRITEL(ISPOST_DUMP_TRIG_ON, ISPOST_DUMPTRIG);
//	// Setup dump trigger off.
//	ISPOST_WRITEL(ISPOST_DUMP_TRIG_OFF, ISPOST_DUMPTRIG);
//}
//
#if !defined(IMG_KERNEL_MODULE)
void
IspostRegDump(
	char* pszFilename
	)
{
#define TEMP_MEM_SIZE  (512)
#define DUMP_MEM_SIZE  (1024 * 40)

	ISPOST_UINT32 uiIdx;
	ISPOST_UINT32 ui32Stride;
	ISPOST_UINT32 ui32Width;
	ISPOST_UINT32 ui32Height;
	ISPOST_UINT32 ui32BufferEnd;
	ISPOST_UINT32 ui32RegAddr;
	FILE * RegDumpfd = NULL;
	int iWriteLen = 0;
	int32_t iWriteResult = 0;
	char szTemp[TEMP_MEM_SIZE];
	char szDump[DUMP_MEM_SIZE];

	printf("Registers dump filename =  %s\n", pszFilename);
	RegDumpfd = fopen(pszFilename, "wb");
	if (NULL != RegDumpfd) {
		memset(szDump, 0, DUMP_MEM_SIZE);
		iWriteLen = 0;
		sprintf(szTemp, "========== ISPOST process registers dump ==========\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x000; uiIdx<=0x004; uiIdx+=4) {
			ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "========== ISPOST LC registers dump ==========\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x008; uiIdx<=0x038; uiIdx+=4) {
			if ((0x00C == uiIdx) || (0x02C == uiIdx)) {
				continue;
			}
			if (0x008 == uiIdx) {
				ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
				ui32Stride = ISPOST_READL(ISPOST_LCSRCS);
				ui32Stride = (ui32Stride >> ISPOST_LCSRCS_SD_SHIFT) & ISPOST_LCSRCS_SD_LSBMASK;
				ui32Width = ISPOST_READL(ISPOST_LCSRCSZ);
				ui32Height = (ui32Width >> ISPOST_LCSRCSZ_H_SHIFT) & ISPOST_LCSRCSZ_H_LSBMASK;
				ui32Width = (ui32Width >> ISPOST_LCSRCSZ_W_SHIFT) & ISPOST_LCSRCSZ_W_LSBMASK;
				ui32BufferEnd = ISPOST_READL(ui32RegAddr) + (ui32Stride * ui32Height);
				sprintf(szTemp, "0x%08X = 0x%08X - 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr), ui32BufferEnd);
				strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
				iWriteLen += strlen(szTemp);
			} else if (0x010 == uiIdx) {
				ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
				ui32Stride = ISPOST_READL(ISPOST_LCSRCS);
				ui32Stride = (ui32Stride >> ISPOST_LCSRCS_SD_SHIFT) & ISPOST_LCSRCS_SD_LSBMASK;
				ui32Width = ISPOST_READL(ISPOST_LCSRCSZ);
				ui32Height = (ui32Width >> ISPOST_LCSRCSZ_H_SHIFT) & ISPOST_LCSRCSZ_H_LSBMASK;
				ui32Width = (ui32Width >> ISPOST_LCSRCSZ_W_SHIFT) & ISPOST_LCSRCSZ_W_LSBMASK;
				ui32BufferEnd = ISPOST_READL(ui32RegAddr) + (ui32Stride * ui32Height / 2);
				sprintf(szTemp, "0x%08X = 0x%08X - 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr), ui32BufferEnd);
				strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
				iWriteLen += strlen(szTemp);
			} else if (0x020 == uiIdx) {
				ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
				ui32BufferEnd = ISPOST_READL(ui32RegAddr) + LC_GRID_BUF_LENGTH;
				sprintf(szTemp, "0x%08X = 0x%08X - 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr), ui32BufferEnd);
				strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
				iWriteLen += strlen(szTemp);
			} else {
				ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
				sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
				strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
				iWriteLen += strlen(szTemp);
			}
		}
		ui32RegAddr = ISPOST_PERI_BASE_ADDR + 0x05C;
		sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		sprintf(szTemp, "========== ISPOST OV registers dump ==========\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x060; uiIdx<=0x088; uiIdx+=4) {
			if (0x064 == uiIdx) {
				ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
				ui32Stride = ISPOST_READL(ISPOST_OVRIS);
				ui32Stride = (ui32Stride >> ISPOST_OVRIS_SD_SHIFT) & ISPOST_OVRIS_SD_LSBMASK;
				ui32Width = ISPOST_READL(ISPOST_OVISZ);
				ui32Height = (ui32Width >> ISPOST_OVISZ_H_SHIFT) & ISPOST_OVISZ_H_LSBMASK;
				ui32Width = (ui32Width >> ISPOST_OVISZ_W_SHIFT) & ISPOST_OVISZ_W_LSBMASK;
				ui32BufferEnd = ISPOST_READL(ui32RegAddr) + (ui32Stride * ui32Height);
				sprintf(szTemp, "0x%08X = 0x%08X - 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr), ui32BufferEnd);
				strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
				iWriteLen += strlen(szTemp);
			} else if (0x068 == uiIdx) {
				ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
				ui32Stride = ISPOST_READL(ISPOST_OVRIS);
				ui32Stride = (ui32Stride >> ISPOST_OVRIS_SD_SHIFT) & ISPOST_OVRIS_SD_LSBMASK;
				ui32Width = ISPOST_READL(ISPOST_OVISZ);
				ui32Height = (ui32Width >> ISPOST_OVISZ_H_SHIFT) & ISPOST_OVISZ_H_LSBMASK;
				ui32Width = (ui32Width >> ISPOST_OVISZ_W_SHIFT) & ISPOST_OVISZ_W_LSBMASK;
				ui32BufferEnd = ISPOST_READL(ui32RegAddr) + (ui32Stride * ui32Height / 2);
				sprintf(szTemp, "0x%08X = 0x%08X - 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr), ui32BufferEnd);
				strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
				iWriteLen += strlen(szTemp);
			} else {
				ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
				sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
				strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
				iWriteLen += strlen(szTemp);
			}
		}
		sprintf(szTemp, "========== ISPOST DN registers dump ==========\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x0A0; uiIdx<=0x0B0; uiIdx+=4) {
			ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "----- ISPOST DN Y curve registers dump -----\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		ISPOST_WRITEL(0x01000000, ISPOST_DNCTRL0);
		for (uiIdx=0x0B8; uiIdx<=0x0F0; uiIdx+=4) {
			ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "----- ISPOST DN U curve registers dump -----\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		ISPOST_WRITEL(0x02000000, ISPOST_DNCTRL0);
		for (uiIdx=0x0B8; uiIdx<=0x0F0; uiIdx+=4) {
			ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "----- ISPOST DN V curve registers dump -----\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		writel(0x03000000, ISPOST_DNCTRL0);
		for (uiIdx=0x0B8; uiIdx<=0x0F0; uiIdx+=4) {
			ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		ui32RegAddr = ISPOST_PERI_BASE_ADDR + 0x0F8;
		sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		ui32RegAddr = ISPOST_PERI_BASE_ADDR + 0x0FC;
		sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		sprintf(szTemp, "========== ISPOST SS0 registers dump ==========\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x100; uiIdx<=0x114; uiIdx+=4) {
			ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "========== ISPOST SS1 registers dump ==========\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x118; uiIdx<=0x12C; uiIdx+=4) {
			ui32RegAddr = ISPOST_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "========== ISPOST SS AXI registers dump ==========\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		ui32RegAddr = ISPOST_PERI_BASE_ADDR + 0x13C;
		sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, ISPOST_READL(ui32RegAddr));
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		sprintf(szTemp, "===========================================\n\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		//============================================================
		iWriteResult = fwrite((void *)szDump, iWriteLen, 1, RegDumpfd);
		if (iWriteResult != iWriteLen) {
			printf("ISPOST registers dump file write failed!!!\n");
		}
		fclose(RegDumpfd);
	} else {
		printf("ISPOST registers dump file open failed!!!\n");
	}
}

void
IspostMemoryDump(
	char* pszFilename,
	ISPOST_UINT32 ui32StartAddress,
	ISPOST_UINT32 ui32Length
	)
{
	FILE * MemDumpfd = NULL;
	int iWriteLen = 0;
	int32_t iWriteResult = 0;

	printf("Memory dump filename =  %s\n", pszFilename);
	MemDumpfd = fopen(pszFilename, "wb");
	if (NULL != MemDumpfd) {
		iWriteLen = ui32Length;
		iWriteResult = fwrite((void *)ui32StartAddress, iWriteLen, 1, MemDumpfd);
		if (iWriteResult != iWriteLen) {
			printf("ISPOST memory dump file write failed!!!\n");
		}
		fclose(MemDumpfd);
	} else {
		printf("ISPOST memory dump file open failed!!!\n");
	}
}
#endif //IMG_KERNEL_MODULE

ISPOST_RESULT
IspostLc480pOvDn1080pSS0800x480SS1720pSet(
	ISPOST_CTRL0 stCtrl0,
	LC_INFO * pstLcInfo,
	OV_INFO * pstOvInfo,
	DN_INFO * pstDnInfo,
	SS_INFO * pstSS0Info,
	SS_INFO * pstSS1Info
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	OV_ALPHA stOvAlpha;
	DN_MSK_CRV_INFO DnMskCrvInfo;
	ISPOST_INT16 curIdx = 0;
	ISPOST_INT16 crvItem = 0;
	ISPOST_UINT32 ui32CrvInfo[DN_MSK_CRV_IDX_MAX][DN_MSK_CRV_MAX] =
	{
		{0x00118000, 0x00228002, 0x00338006, 0x0152840e, 0x0d129012,
		0x05c2eb16, 0x00f3f71a, 0x0037fb22, 0x0007fda2, 0x0007fdff,
		0x0007fdff, 0x0007fdff, 0x0007fdff, 0x0007fdff, 0x0007fdff
		},
		{0x00118000, 0x00228002, 0x00338006, 0x0152840e, 0x0d129012,
		0x05c2eb16, 0x00f3f71a, 0x0037fb22, 0x0007fda2, 0x0007fdff,
		0x0007fdff, 0x0007fdff, 0x0007fdff, 0x0007fdff, 0x0007fdff
		},
		{0x1ff18000, 0x05c2ad02, 0x00f3b906, 0x0032bd0e, 0x0012bd12,
		0x0012be16, 0x0003be1a, 0x0007be22, 0x0007bfa2, 0x0007bfff,
		0x0007bfff, 0x0007bfff, 0x0007bfff, 0x0007bfff, 0x0007bfff
		}
	};


	// ==================================================================
	// Setup lens correction information.
	//
	// Setup LC SRC addr, stride and size.
	// ============================
	//
	//pstLcInfo->stSrcImgInfo.ui32YAddr = (ISPOST_UINT32)LcInImg.vBuffer;
	pstLcInfo->stSrcImgInfo.ui16Stride = 640;
	if (stCtrl0.ui1EnLC) {
		pstLcInfo->stSrcImgInfo.ui16Width = pstLcInfo->stSrcImgInfo.ui16Stride;
		pstLcInfo->stSrcImgInfo.ui16Height = 480;
	} else {
		pstLcInfo->stSrcImgInfo.ui16Width = 0;
		pstLcInfo->stSrcImgInfo.ui16Height = 0;
	}
	//pstLcInfo->stSrcImgInfo.ui32UVAddr = pstLcInfo->stSrcImgInfo.ui32YAddr +
	//	(pstLcInfo->stSrcImgInfo.ui16Width * pstLcInfo->stSrcImgInfo.ui16Height);
	pstLcInfo->stSrcImgInfo.ui32UVAddr = pstLcInfo->stSrcImgInfo.ui32YAddr +
		(pstLcInfo->stSrcImgInfo.ui16Stride * 480);
	ui32Ret += IspostLcSrcImgSet(
		pstLcInfo->stSrcImgInfo.ui32YAddr,
		pstLcInfo->stSrcImgInfo.ui32UVAddr,
		pstLcInfo->stSrcImgInfo.ui16Stride,
		pstLcInfo->stSrcImgInfo.ui16Width,
		pstLcInfo->stSrcImgInfo.ui16Height
		);
	//
	// Setup LC grid address, stride and grid size
	// ============================
	//
	//stLcInfo.stGridBufInfo.ui32BufAddr = (ISPOST_UINT32)LcGridData.vBuffer;
	if (stCtrl0.ui1EnLC) {
		pstLcInfo->stGridBufInfo.ui16Stride = 1936;
	} else {
		pstLcInfo->stGridBufInfo.ui16Stride = 0;
	}
	pstLcInfo->stGridBufInfo.ui8LineBufEnable = 0;
	pstLcInfo->stGridBufInfo.ui8Size = GRID_SIZE_16_16;
	ui32Ret += IspostLcGridBufSetWithStride(
		pstLcInfo->stGridBufInfo.ui32BufAddr,
		pstLcInfo->stGridBufInfo.ui16Stride,
		pstLcInfo->stGridBufInfo.ui8LineBufEnable,
		pstLcInfo->stGridBufInfo.ui8Size
		);
	//
	// Setup LC pixel cache mode
	// ============================
	//
	pstLcInfo->stPxlCacheMode.ui8CtrlCfg = CACHE_MODE_2;
	pstLcInfo->stPxlCacheMode.ui8CBCRSwap = CBCR_SWAP_MODE_SWAP;
	ui32Ret += IspostLcPxlCacheModeSet(
		pstLcInfo->stPxlCacheMode.ui8CtrlCfg,
		pstLcInfo->stPxlCacheMode.ui8CBCRSwap
		);
	//
	// Setup LC pixel coordinator generator mode.
	// ============================
	//
	pstLcInfo->stPxlScanMode.ui8ScanW = SCAN_WIDTH_16;
	pstLcInfo->stPxlScanMode.ui8ScanH = SCAN_HEIGHT_16;
	pstLcInfo->stPxlScanMode.ui8ScanM = SCAN_MODE_TB_LR;
	ui32Ret += IspostLcPxlScanModeSet(
		pstLcInfo->stPxlScanMode.ui8ScanW,
		pstLcInfo->stPxlScanMode.ui8ScanH,
		pstLcInfo->stPxlScanMode.ui8ScanM
		);
	//
	// Setup LC pixel coordinator destination size.
	// ============================
	//
	pstLcInfo->stPxlDstSize.ui16Width = 1920;
	pstLcInfo->stPxlDstSize.ui16Height = 1080;
	ui32Ret += IspostLcPxlDstSizeSet(
		pstLcInfo->stPxlDstSize.ui16Width,
		pstLcInfo->stPxlDstSize.ui16Height
		);
	//
	// Setup LC AXI Read Request Limit, GBID and SRCID.
	// ============================
	//
	//pstLcInfo->stAxiId.ui8SrcReqLmt = 4;
	pstLcInfo->stAxiId.ui8SrcReqLmt = 48;
	//pstLcInfo->stAxiId.ui8SrcReqLmt = 56;
	pstLcInfo->stAxiId.ui8GBID = 1;
	pstLcInfo->stAxiId.ui8SRCID = 0;
	ui32Ret += IspostLcAxiIdSet(
		pstLcInfo->stAxiId.ui8SrcReqLmt,
		pstLcInfo->stAxiId.ui8GBID,
		pstLcInfo->stAxiId.ui8SRCID
		);
	// ==================================================================

	// ==================================================================
	// Setup overlay information.
	//
	// Setup OV overlay image information.
	//
	pstOvInfo->stOvImgInfo.ui16Stride = 128;
	pstOvInfo->stOvImgInfo.ui16Width = 80;
	pstOvInfo->stOvImgInfo.ui16Height = 64;
	pstOvInfo->stOvImgInfo.ui32UVAddr = (ISPOST_UINT32)(pstOvInfo->stOvImgInfo.ui32YAddr + 9216);
	pstOvInfo->ui8OverlayMode = OVERLAY_MODE_YUV;
	pstOvInfo->stOvOffset.ui16X = 80;
	pstOvInfo->stOvOffset.ui16Y = 80;
	ui32Ret += IspostOvImgSet(
		pstOvInfo->stOvImgInfo.ui32YAddr,
		pstOvInfo->stOvImgInfo.ui32UVAddr,
		pstOvInfo->stOvImgInfo.ui16Stride,
		pstOvInfo->stOvImgInfo.ui16Width,
		pstOvInfo->stOvImgInfo.ui16Height
		);
	//
	// Setup OV Set overlay mode.
	//
	ui32Ret += IspostOvModeSet(
		pstOvInfo->ui8OverlayMode
		);
	//
	// Setup OV overlay offset.
	//
	ui32Ret += IspostOvOffsetSet(
		pstOvInfo->stOvOffset.ui16X,
		pstOvInfo->stOvOffset.ui16Y
		);
	//
	// Setup OV AXI read ID.
	//
	pstOvInfo->ui8OVID = 2;
	ui32Ret += IspostOvAxiIdSet(
		pstOvInfo->ui8OVID
		);
	if (stCtrl0.ui1EnOV) {
		//
		// Setup OV overlay alpha value 0.
		//
		stOvAlpha.stValue0.ui8Alpha3 = 0x33;
		stOvAlpha.stValue0.ui8Alpha2 = 0x22;
		stOvAlpha.stValue0.ui8Alpha1 = 0x11;
		stOvAlpha.stValue0.ui8Alpha0 = 0x00;
		// Setup OV overlay alpha value 1.
		stOvAlpha.stValue1.ui8Alpha3 = 0x77;
		stOvAlpha.stValue1.ui8Alpha2 = 0x66;
		stOvAlpha.stValue1.ui8Alpha1 = 0x55;
		stOvAlpha.stValue1.ui8Alpha0 = 0x44;
		// Setup OV overlay alpha value 2.
		stOvAlpha.stValue2.ui8Alpha3 = 0xbb;
		stOvAlpha.stValue2.ui8Alpha2 = 0xaa;
		stOvAlpha.stValue2.ui8Alpha1 = 0x99;
		stOvAlpha.stValue2.ui8Alpha0 = 0x88;
		// Setup OV overlay alpha value 3.
		stOvAlpha.stValue3.ui8Alpha3 = 0xff;
		stOvAlpha.stValue3.ui8Alpha2 = 0xee;
		stOvAlpha.stValue3.ui8Alpha1 = 0xdd;
		stOvAlpha.stValue3.ui8Alpha0 = 0xcc;
		ui32Ret += IspostOverlayAlphaValueSet(stOvAlpha);
	}
	// ==================================================================

	// ==================================================================
	// Setup 3D-denoise information.
	if (stCtrl0.ui1EnDN) {
		//
		// Setup DNR reference input image addr, stride.
		// ============================
		//
		//pstDnInfo->stRefImgInfo.ui32YAddr = (ISPOST_UINT32)DnRefImg.vBuffer;
		pstDnInfo->stRefImgInfo.ui32UVAddr = pstDnInfo->stOutImgInfo.ui32YAddr +
			(pstLcInfo->stPxlDstSize.ui16Width * pstLcInfo->stPxlDstSize.ui16Height);
		pstDnInfo->stRefImgInfo.ui16Stride = pstLcInfo->stPxlDstSize.ui16Width;
		ui32Ret += IspostDnRefImgSet(
			pstDnInfo->stRefImgInfo.ui32YAddr,
			pstDnInfo->stRefImgInfo.ui32UVAddr,
			pstDnInfo->stRefImgInfo.ui16Stride
			);

		// Set DN mask curve
		for (curIdx = 0; curIdx < DN_MSK_CRV_IDX_MAX; ++curIdx)
		{
			DnMskCrvInfo.ui8MskCrvIdx = curIdx;
			for (crvItem = 0; crvItem < DN_MSK_CRV_MAX; ++crvItem)
				DnMskCrvInfo.stMskCrv[crvItem].ui32MskCrv = ui32CrvInfo[curIdx][crvItem];
			ui32Ret += Ispost3DDenoiseMskCrvSet(DnMskCrvInfo);
		}
	}
	//
	// Setup DNR output image addr, stride.
	// ============================
	//
	//pstDnInfo->stOutImgInfo.ui32YAddr = (ISPOST_UINT32)DnOutImg.vBuffer;
	pstDnInfo->stOutImgInfo.ui32UVAddr = pstDnInfo->stOutImgInfo.ui32YAddr +
		(pstLcInfo->stPxlDstSize.ui16Width * pstLcInfo->stPxlDstSize.ui16Height);
	pstDnInfo->stOutImgInfo.ui16Stride = pstLcInfo->stPxlDstSize.ui16Width;
	ui32Ret += IspostDnOutImgSet(
		pstDnInfo->stOutImgInfo.ui32YAddr,
		pstDnInfo->stOutImgInfo.ui32UVAddr,
		pstDnInfo->stOutImgInfo.ui16Stride
		);
	// Setup DN AXI reference write and read ID.
	pstDnInfo->stAxiId.ui8RefWrID = 3;
	pstDnInfo->stAxiId.ui8RefRdID = 3;
	ui32Ret += IspostDnAxiIdSet(
		pstDnInfo->stAxiId.ui8RefWrID,
		pstDnInfo->stAxiId.ui8RefRdID
		);
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 0 information.
	if (stCtrl0.ui1EnSS0) {
		//
		// Setup Scaling stream 0 output image information..
		// ============================
		//
		// Fill SS0 output stream select.
		pstSS0Info->ui8StreamSel = SCALING_STREAM_0;
		// Fill S0S0 output image information.
		//pstSS0Info->stOutImgInfo.ui32YAddr = (ISPOST_UINT32)SS0OutImg.vBuffer;
		pstSS0Info->stOutImgInfo.ui16Stride = 800;
		pstSS0Info->stOutImgInfo.ui16Width = 800;
		pstSS0Info->stOutImgInfo.ui16Height = 480;
		pstSS0Info->stOutImgInfo.ui32UVAddr = pstSS0Info->stOutImgInfo.ui32YAddr +
			(pstSS0Info->stOutImgInfo.ui16Width * pstSS0Info->stOutImgInfo.ui16Height);
		pstSS0Info->stHSF.ui16SF = 1708;
		pstSS0Info->stHSF.ui8SM = SCALING_MODE_SCALING_DOWN_1;
		pstSS0Info->stVSF.ui16SF = 1824;
		pstSS0Info->stVSF.ui8SM = SCALING_MODE_SCALING_DOWN_1;
		// Fill SS0 AXI SSWID.
		pstSS0Info->ui8SSWID = 1;
		ui32Ret += IspostScaleStreamSet(*pstSS0Info);
	}
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 1 information.
	if (stCtrl0.ui1EnSS1) {
		//
		// Setup Scaling stream 1 output image information..
		// ============================
		//
		// Fill SS1 output stream select.
		pstSS1Info->ui8StreamSel = SCALING_STREAM_1;
		// Fill SS1 output image information.
		//pstSS1Info->stOutImgInfo.ui32YAddr = (ISPOST_UINT32)SS1OutImg.vBuffer;
		pstSS1Info->stOutImgInfo.ui16Stride = 1280;
		pstSS1Info->stOutImgInfo.ui16Width = 1280;
		pstSS1Info->stOutImgInfo.ui16Height = 720;
		pstSS1Info->stOutImgInfo.ui32UVAddr = pstSS1Info->stOutImgInfo.ui32YAddr +
			(pstSS1Info->stOutImgInfo.ui16Width * pstSS1Info->stOutImgInfo.ui16Height);
		pstSS1Info->stHSF.ui16SF = 2732;
		pstSS1Info->stHSF.ui8SM = SCALING_MODE_SCALING_DOWN_1;
		pstSS1Info->stVSF.ui16SF = 2734;
		pstSS1Info->stVSF.ui8SM = SCALING_MODE_SCALING_DOWN_1;
		// Fill SS1 AXI SSWID.
		pstSS1Info->ui8SSWID = 2;
		ui32Ret += IspostScaleStreamSet(*pstSS1Info);
	}
	// ==================================================================

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcOvDn1080pSS0800x480SS1720pInitial(
	ISPOST_UINT32 ui32LcSrcWidth,
	ISPOST_UINT32 ui32LcSrcHeight,
	ISPOST_CTRL0 stCtrl0,
	ISPOST_UINT8 ui8CBCRSwap,
	ISPOST_UINT32 ui32LcSrcImgBufAddr,
	ISPOST_UINT32 ui32LcGridBufAddr,
	ISPOST_UINT32 ui32OvImgBufAddr,
	ISPOST_UINT32 ui32DnRefImgBufAddr,
	ISPOST_UINT32 ui32DnOutImgBufAddr,
	ISPOST_UINT32 ui32SS0OutImgBufAddr,
	ISPOST_UINT32 ui32SS1OutImgBufAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	LC_INFO stLcInfo;
	OV_INFO stOvInfo;
	OV_ALPHA stOvAlpha;
	DN_INFO stDnInfo;
	DN_MSK_CRV_INFO stDnMskCrvInfo;
	SS_INFO stSS0Info;
	SS_INFO stSS1Info;
	ISPOST_INT16 curIdx = 0;
	ISPOST_INT16 crvItem = 0;
	ISPOST_UINT32 ui32CrvInfo[DN_MSK_CRV_IDX_MAX][DN_MSK_CRV_MAX] =
	{
		{0x00013300, 0x00063302, 0x00533742, 0x02123d4a, 0x14f2504e,
		0x0942e052, 0x0183f456, 0x0047fa5e, 0x0007fede, 0x0007feff,
		0x0007feff, 0x0007feff, 0x0007feff, 0x0007feff, 0x0007feff
		},
		{0x00013300, 0x00063302, 0x00533742, 0x02123d4a, 0x14f2504e,
		0x0942e052, 0x0183f456, 0x0047fa5e, 0x0007fede, 0x0007feff,
		0x0007feff, 0x0007feff, 0x0007feff, 0x0007feff, 0x0007feff
		},
		{0x02000000, 0x00013300, 0x00063302, 0x00533742, 0x02123d4a,
		0x14f2504e, 0x0942e052, 0x0183f456, 0x0047fa5e, 0x0007fede,
		0x0007feff, 0x0007feff, 0x0007feff, 0x0007feff, 0x0007feff
		}
	};

	// ==================================================================
	// Setup lens correction information.
	//
	// Setup LC SRC addr, stride and size.
	// ============================
	//
	stLcInfo.stSrcImgInfo.ui32YAddr = ui32LcSrcImgBufAddr;
	stLcInfo.stSrcImgInfo.ui16Stride = ui32LcSrcWidth;
	stLcInfo.stSrcImgInfo.ui16Width = ui32LcSrcWidth;
	stLcInfo.stSrcImgInfo.ui16Height = ui32LcSrcHeight;
	//stLcInfo.stSrcImgInfo.ui32UVAddr = stLcInfo.stSrcImgInfo.ui32YAddr +
	//	(stLcInfo.stSrcImgInfo.ui16Stride * stLcInfo.stSrcImgInfo.ui16Height);
	stLcInfo.stSrcImgInfo.ui32UVAddr = stLcInfo.stSrcImgInfo.ui32YAddr +
		(ui32LcSrcWidth * ui32LcSrcHeight);
	ui32Ret += IspostLcSrcImgSet(
		stLcInfo.stSrcImgInfo.ui32YAddr,
		stLcInfo.stSrcImgInfo.ui32UVAddr,
		stLcInfo.stSrcImgInfo.ui16Stride,
		stLcInfo.stSrcImgInfo.ui16Width,
		stLcInfo.stSrcImgInfo.ui16Height
		);
	//
	// Setup LC grid buffer address, stride and grid size
	// ============================
	//
	stLcInfo.stGridBufInfo.ui32BufAddr = ui32LcGridBufAddr;
	stLcInfo.stGridBufInfo.ui8LineBufEnable = 0;
	stLcInfo.stGridBufInfo.ui8Size = LC_GRID_BUF_GRID_SIZE;
	if (stCtrl0.ui1EnLC) {
		//stLcInfo.stGridBufInfo.ui16Stride = LC_GRID_BUF_STRIDE;
		stLcInfo.stGridBufInfo.ui16Stride = IspostLcGridBufStrideCalc(
			stLcInfo.stSrcImgInfo.ui16Width,
			stLcInfo.stGridBufInfo.ui8Size
			);
	} else {
		stLcInfo.stGridBufInfo.ui16Stride = 0;
	}
	ui32Ret += IspostLcGridBufSetWithStride(
		stLcInfo.stGridBufInfo.ui32BufAddr,
		stLcInfo.stGridBufInfo.ui16Stride,
		stLcInfo.stGridBufInfo.ui8LineBufEnable,
		stLcInfo.stGridBufInfo.ui8Size
		);
	//
	// Setup LC pixel cache mode
	// ============================
	//
	stLcInfo.stPxlCacheMode.ui8CtrlCfg = LC_CACHE_MODE_CTRL_CFG;
	//stLcInfo.stPxlCacheMode.ui8CBCRSwap = LC_CACHE_MODE_CBCR_SWAP;
	stLcInfo.stPxlCacheMode.ui8CBCRSwap = ui8CBCRSwap;
	ui32Ret += IspostLcPxlCacheModeSet(
		stLcInfo.stPxlCacheMode.ui8CtrlCfg,
		stLcInfo.stPxlCacheMode.ui8CBCRSwap
		);
	//
	// Setup LC pixel coordinator generator mode.
	// ============================
	//
	stLcInfo.stPxlScanMode.ui8ScanW = LC_SCAN_MODE_SCAN_W;
	stLcInfo.stPxlScanMode.ui8ScanH = LC_SCAN_MODE_SCAN_H;
	stLcInfo.stPxlScanMode.ui8ScanM = LC_SCAN_MODE_SCAN_M;
	ui32Ret += IspostLcPxlScanModeSet(
		stLcInfo.stPxlScanMode.ui8ScanW,
		stLcInfo.stPxlScanMode.ui8ScanH,
		stLcInfo.stPxlScanMode.ui8ScanM
		);
	//
	// Setup LC pixel coordinator destination size.
	// ============================
	//
	stLcInfo.stPxlDstSize.ui16Width = LC_DST_SIZE_WIDTH;
	stLcInfo.stPxlDstSize.ui16Height = LC_DST_SIZE_HEIGHT;
	ui32Ret += IspostLcPxlDstSizeSet(
		stLcInfo.stPxlDstSize.ui16Width,
		stLcInfo.stPxlDstSize.ui16Height
		);
	//
	// Setup LC AXI Read Request Limit, GBID and SRCID.
	// ============================
	//
	//stLcInfo.stAxiId.ui8SrcReqLmt = 4;
	if ((0 == stCtrl0.ui1EnLC) && (stCtrl0.ui1EnDN)) {
		stLcInfo.stAxiId.ui8SrcReqLmt = 96;
	} else {
		stLcInfo.stAxiId.ui8SrcReqLmt = 48;
		//stLcInfo.stAxiId.ui8SrcReqLmt = 56;
	}
	stLcInfo.stAxiId.ui8GBID = 1;
	stLcInfo.stAxiId.ui8SRCID = 0;
	ui32Ret += IspostLcAxiIdSet(
		stLcInfo.stAxiId.ui8SrcReqLmt,
		stLcInfo.stAxiId.ui8GBID,
		stLcInfo.stAxiId.ui8SRCID
		);

	// Fill LC back fill color Y, CB and CR value.
	ui32Ret += IspostLcBackFillColorSet(
		LC_BACK_FILL_Y,
		LC_BACK_FILL_CB,
		LC_BACK_FILL_CR
		);
	// ==================================================================

	// ==================================================================
	// Setup overlay information.
	//
	// Setup OV overlay image information.
	//
	if (stCtrl0.ui1EnLC) {
		stOvInfo.stOvImgInfo.ui32YAddr = ui32OvImgBufAddr;
		stOvInfo.stOvImgInfo.ui16Stride = OV_IMG_STRIDE;
		stOvInfo.stOvImgInfo.ui16Width = OV_IMG_OV_WIDTH;
		stOvInfo.stOvImgInfo.ui16Height = OV_IMG_OV_HEIGHT;
		stOvInfo.stOvImgInfo.ui32UVAddr = stOvInfo.stOvImgInfo.ui32YAddr +
			(OV_IMG_Y_SIZE);
		stOvInfo.ui8OverlayMode = OV_IMG_OV_MODE;
		stOvInfo.stOvOffset.ui16X = OV_IMG_OV_OFFSET_X;
		stOvInfo.stOvOffset.ui16Y = OV_IMG_OV_OFFSET_Y;
		ui32Ret += IspostOvImgSet(
			stOvInfo.stOvImgInfo.ui32YAddr,
			stOvInfo.stOvImgInfo.ui32UVAddr,
			stOvInfo.stOvImgInfo.ui16Stride,
			stOvInfo.stOvImgInfo.ui16Width,
			stOvInfo.stOvImgInfo.ui16Height
			);
	} else {
		stOvInfo.stOvImgInfo.ui32YAddr = ui32LcSrcImgBufAddr;
		stOvInfo.stOvImgInfo.ui16Stride = ui32LcSrcWidth;
		stOvInfo.stOvImgInfo.ui16Width = ui32LcSrcWidth;
		stOvInfo.stOvImgInfo.ui16Height = ui32LcSrcHeight;
		stOvInfo.stOvImgInfo.ui32UVAddr = stLcInfo.stSrcImgInfo.ui32YAddr +
			(ui32LcSrcWidth * ui32LcSrcHeight);
		stOvInfo.ui8OverlayMode = OV_IMG_OV_MODE;
		stOvInfo.stOvOffset.ui16X = 0;
		stOvInfo.stOvOffset.ui16Y = 0;
		ui32Ret += IspostOvImgSet(
			stOvInfo.stOvImgInfo.ui32YAddr,
			stOvInfo.stOvImgInfo.ui32UVAddr,
			stOvInfo.stOvImgInfo.ui16Stride,
			stOvInfo.stOvImgInfo.ui16Width,
			stOvInfo.stOvImgInfo.ui16Height
			);
	}
	//
	// Setup OV Set overlay mode.
	//
	ui32Ret += IspostOvModeSet(
		stOvInfo.ui8OverlayMode
		);
	//
	// Setup OV overlay offset.
	//
	ui32Ret += IspostOvOffsetSet(
		stOvInfo.stOvOffset.ui16X,
		stOvInfo.stOvOffset.ui16Y
		);
	//
	// Setup OV AXI read ID.
	//
	stOvInfo.ui8OVID = 2;
	ui32Ret += IspostOvAxiIdSet(
		stOvInfo.ui8OVID
		);
	if (stCtrl0.ui1EnOV) {
		//
		// Setup OV overlay alpha value 0.
		//
		stOvAlpha.stValue0.ui8Alpha3 = 0x33;
		stOvAlpha.stValue0.ui8Alpha2 = 0x22;
		stOvAlpha.stValue0.ui8Alpha1 = 0x11;
		stOvAlpha.stValue0.ui8Alpha0 = 0x00;
		// Setup OV overlay alpha value 1.
		stOvAlpha.stValue1.ui8Alpha3 = 0x77;
		stOvAlpha.stValue1.ui8Alpha2 = 0x66;
		stOvAlpha.stValue1.ui8Alpha1 = 0x55;
		stOvAlpha.stValue1.ui8Alpha0 = 0x44;
		// Setup OV overlay alpha value 2.
		stOvAlpha.stValue2.ui8Alpha3 = 0xbb;
		stOvAlpha.stValue2.ui8Alpha2 = 0xaa;
		stOvAlpha.stValue2.ui8Alpha1 = 0x99;
		stOvAlpha.stValue2.ui8Alpha0 = 0x88;
		// Setup OV overlay alpha value 3.
		stOvAlpha.stValue3.ui8Alpha3 = 0xff;
		stOvAlpha.stValue3.ui8Alpha2 = 0xee;
		stOvAlpha.stValue3.ui8Alpha1 = 0xdd;
		stOvAlpha.stValue3.ui8Alpha0 = 0xcc;
		ui32Ret += IspostOverlayAlphaValueSet(stOvAlpha);
	}
	// ==================================================================

	// ==================================================================
	// Setup 3D-denoise information.
	if (stCtrl0.ui1EnDN) {
		//
		// Setup DNR reference input image addr, stride.
		// ============================
		//
		stDnInfo.stRefImgInfo.ui32YAddr = ui32DnRefImgBufAddr;
		stDnInfo.stRefImgInfo.ui32UVAddr = stDnInfo.stRefImgInfo.ui32YAddr +
			(stLcInfo.stPxlDstSize.ui16Width * stLcInfo.stPxlDstSize.ui16Height);
		stDnInfo.stRefImgInfo.ui16Stride = stLcInfo.stPxlDstSize.ui16Width;
		ui32Ret += IspostDnRefImgSet(
			stDnInfo.stRefImgInfo.ui32YAddr,
			stDnInfo.stRefImgInfo.ui32UVAddr,
			stDnInfo.stRefImgInfo.ui16Stride
			);

#if 1 //For James
		// Set DN mask curve
		for (curIdx = 0; curIdx < DN_MSK_CRV_IDX_MAX; ++curIdx)
		{
			stDnMskCrvInfo.ui8MskCrvIdx = curIdx;
			for (crvItem = 0; crvItem < DN_MSK_CRV_MAX; ++crvItem)
				stDnMskCrvInfo.stMskCrv[crvItem].ui32MskCrv = ui32CrvInfo[curIdx][crvItem];
			ui32Ret += Ispost3DDenoiseMskCrvSet(stDnMskCrvInfo);
		}
#else
		// Set DN mask curve Y.
		stDnMskCrvInfo.ui8MskCrvIdx = DN_MSK_CRV_IDX_Y;
		stDnMskCrvInfo.stMskCrv00.ui32MskCrv = 0x00118000;
		stDnMskCrvInfo.stMskCrv01.ui32MskCrv = 0x00228002;
		stDnMskCrvInfo.stMskCrv02.ui32MskCrv = 0x00338006;
		stDnMskCrvInfo.stMskCrv03.ui32MskCrv = 0x0152840e;
		stDnMskCrvInfo.stMskCrv04.ui32MskCrv = 0x0d129012;
		stDnMskCrvInfo.stMskCrv05.ui32MskCrv = 0x05c2eb16;
		stDnMskCrvInfo.stMskCrv06.ui32MskCrv = 0x00f3f71a;
		stDnMskCrvInfo.stMskCrv07.ui32MskCrv = 0x0037fb22;
		stDnMskCrvInfo.stMskCrv08.ui32MskCrv = 0x0007fda2;
		stDnMskCrvInfo.stMskCrv09.ui32MskCrv = 0x0007fdff;
		stDnMskCrvInfo.stMskCrv10.ui32MskCrv = 0x0007fdff;
		stDnMskCrvInfo.stMskCrv11.ui32MskCrv = 0x0007fdff;
		stDnMskCrvInfo.stMskCrv12.ui32MskCrv = 0x0007fdff;
		stDnMskCrvInfo.stMskCrv13.ui32MskCrv = 0x0007fdff;
		stDnMskCrvInfo.stMskCrv14.ui32MskCrv = 0x0007fdff;
		ui32Ret += Ispost3DDenoiseMskCrvSet(stDnMskCrvInfo);

		// Set DN mask curve U.
		stDnMskCrvInfo.ui8MskCrvIdx = DN_MSK_CRV_IDX_U;
		stDnMskCrvInfo.stMskCrv00.ui32MskCrv = 0x00118000;
		stDnMskCrvInfo.stMskCrv01.ui32MskCrv = 0x00228002;
		stDnMskCrvInfo.stMskCrv02.ui32MskCrv = 0x00338006;
		stDnMskCrvInfo.stMskCrv03.ui32MskCrv = 0x0152840e;
		stDnMskCrvInfo.stMskCrv04.ui32MskCrv = 0x0d129012;
		stDnMskCrvInfo.stMskCrv05.ui32MskCrv = 0x05c2eb16;
		stDnMskCrvInfo.stMskCrv06.ui32MskCrv = 0x00f3f71a;
		stDnMskCrvInfo.stMskCrv07.ui32MskCrv = 0x0037fb22;
		stDnMskCrvInfo.stMskCrv08.ui32MskCrv = 0x0007fda2;
		stDnMskCrvInfo.stMskCrv09.ui32MskCrv = 0x0007fdff;
		stDnMskCrvInfo.stMskCrv10.ui32MskCrv = 0x0007fdff;
		stDnMskCrvInfo.stMskCrv11.ui32MskCrv = 0x0007fdff;
		stDnMskCrvInfo.stMskCrv12.ui32MskCrv = 0x0007fdff;
		stDnMskCrvInfo.stMskCrv13.ui32MskCrv = 0x0007fdff;
		stDnMskCrvInfo.stMskCrv14.ui32MskCrv = 0x0007fdff;
		ui32Ret += Ispost3DDenoiseMskCrvSet(stDnMskCrvInfo);

		// Set DN mask curve V.
		stDnMskCrvInfo.ui8MskCrvIdx = DN_MSK_CRV_IDX_V;
		stDnMskCrvInfo.stMskCrv00.ui32MskCrv = 0x1ff18000;
		stDnMskCrvInfo.stMskCrv01.ui32MskCrv = 0x05c2ad02;
		stDnMskCrvInfo.stMskCrv02.ui32MskCrv = 0x00f3b906;
		stDnMskCrvInfo.stMskCrv03.ui32MskCrv = 0x0032bd0e;
		stDnMskCrvInfo.stMskCrv04.ui32MskCrv = 0x0012bd12;
		stDnMskCrvInfo.stMskCrv05.ui32MskCrv = 0x0012be16;
		stDnMskCrvInfo.stMskCrv06.ui32MskCrv = 0x0003be1a;
		stDnMskCrvInfo.stMskCrv07.ui32MskCrv = 0x0007be22;
		stDnMskCrvInfo.stMskCrv08.ui32MskCrv = 0x0007bfa2;
		stDnMskCrvInfo.stMskCrv09.ui32MskCrv = 0x0007bfff;
		stDnMskCrvInfo.stMskCrv10.ui32MskCrv = 0x0007bfff;
		stDnMskCrvInfo.stMskCrv11.ui32MskCrv = 0x0007bfff;
		stDnMskCrvInfo.stMskCrv12.ui32MskCrv = 0x0007bfff;
		stDnMskCrvInfo.stMskCrv13.ui32MskCrv = 0x0007bfff;
		stDnMskCrvInfo.stMskCrv14.ui32MskCrv = 0x0007bfff;
		ui32Ret += Ispost3DDenoiseMskCrvSet(stDnMskCrvInfo);
#endif
	}
	//
	// Setup DNR output image addr, stride.
	// ============================
	//
	stDnInfo.stOutImgInfo.ui32YAddr = ui32DnOutImgBufAddr;
	stDnInfo.stOutImgInfo.ui32UVAddr = stDnInfo.stOutImgInfo.ui32YAddr +
		(stLcInfo.stPxlDstSize.ui16Width * stLcInfo.stPxlDstSize.ui16Height);
	stDnInfo.stOutImgInfo.ui16Stride = stLcInfo.stPxlDstSize.ui16Width;
	ui32Ret += IspostDnOutImgSet(
		stDnInfo.stOutImgInfo.ui32YAddr,
		stDnInfo.stOutImgInfo.ui32UVAddr,
		stDnInfo.stOutImgInfo.ui16Stride
		);
	// Setup DN AXI reference write and read ID.
	stDnInfo.stAxiId.ui8RefWrID = 3;
	stDnInfo.stAxiId.ui8RefRdID = 3;
	ui32Ret += IspostDnAxiIdSet(
		stDnInfo.stAxiId.ui8RefWrID,
		stDnInfo.stAxiId.ui8RefRdID
		);
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 0 information.
	if (stCtrl0.ui1EnSS0) {
		//
		// Setup Scaling stream 0 output image information..
		// ============================
		//
		// Fill SS0 output stream select.
		stSS0Info.ui8StreamSel = SCALING_STREAM_0;
		// Fill SS0 output image information.
		stSS0Info.stOutImgInfo.ui32YAddr = ui32SS0OutImgBufAddr;
		stSS0Info.stOutImgInfo.ui16Stride = SS0_OUT_IMG_STRIDE;
		stSS0Info.stOutImgInfo.ui16Width = SS0_OUT_IMG_WIDTH;
		stSS0Info.stOutImgInfo.ui16Height = SS0_OUT_IMG_HEIGHT;
		stSS0Info.stOutImgInfo.ui32UVAddr = stSS0Info.stOutImgInfo.ui32YAddr +
			(stSS0Info.stOutImgInfo.ui16Width * stSS0Info.stOutImgInfo.ui16Height);
		stSS0Info.stHSF.ui16SF = SS0_OUT_IMG_HSF_SF;
		stSS0Info.stHSF.ui8SM = SS0_OUT_IMG_HSF_SM;
		stSS0Info.stVSF.ui16SF = SS0_OUT_IMG_VSF_SF;
		stSS0Info.stVSF.ui8SM = SS0_OUT_IMG_VSF_SM;
		// Fill SS0 AXI SSWID.
		stSS0Info.ui8SSWID = 1;
		ui32Ret += IspostScaleStreamSet(stSS0Info);
	}
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 1 information.
	if (stCtrl0.ui1EnSS1) {
		//
		// Setup Scaling stream 1 output image information..
		// ============================
		//
		// Fill SS1 output stream select.
		stSS1Info.ui8StreamSel = SCALING_STREAM_1;
		// Fill SS1 output image information.
		stSS1Info.stOutImgInfo.ui32YAddr = ui32SS1OutImgBufAddr;
		stSS1Info.stOutImgInfo.ui16Stride = SS1_OUT_IMG_STRIDE;
		stSS1Info.stOutImgInfo.ui16Width = SS1_OUT_IMG_WIDTH;
		stSS1Info.stOutImgInfo.ui16Height = SS1_OUT_IMG_HEIGHT;
		stSS1Info.stOutImgInfo.ui32UVAddr = stSS1Info.stOutImgInfo.ui32YAddr +
			(stSS1Info.stOutImgInfo.ui16Width * stSS1Info.stOutImgInfo.ui16Height);
		stSS1Info.stHSF.ui16SF = SS1_OUT_IMG_HSF_SF;
		stSS1Info.stHSF.ui8SM = SS1_OUT_IMG_HSF_SM;
		stSS1Info.stVSF.ui16SF = SS1_OUT_IMG_VSF_SF;
		stSS1Info.stVSF.ui8SM = SS1_OUT_IMG_VSF_SM;
		// Fill SS1 AXI SSWID.
		stSS1Info.ui8SSWID = 2;
		ui32Ret += IspostScaleStreamSet(stSS1Info);
	}
	// ==================================================================

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcOvDn1080pSS0800x480SS1720pSetBuf(
	ISPOST_UINT32 ui32LcSrcWidth,
	ISPOST_UINT32 ui32LcSrcHeight,
	ISPOST_CTRL0 stCtrl0,
	ISPOST_UINT32 ui32LcSrcImgBufAddr,
	ISPOST_UINT32 ui32DnRefImgBufAddr,
	ISPOST_UINT32 ui32DnOutImgBufAddr,
	ISPOST_UINT32 ui32SS0OutImgBufAddr,
	ISPOST_UINT32 ui32SS1OutImgBufAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32LcSrcYSize;
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
//	ISPOST_UINT32 ui32Data;
	ISPOST_UINT32 ui32StreamOffset;
#endif

	// ==================================================================
	// Setup lens correction information.
	//
	// Setup LC SRC addr.
	// ============================
	//
	ui32LcSrcYSize = ui32LcSrcWidth * ui32LcSrcHeight;
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
	// Setup source image Y plane start address.
	ISPOST_WRITEL(ui32LcSrcImgBufAddr, ISPOST_LCSRCAY);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCAY, ui32LcSrcImgBufAddr, ui32Ret);

	// Setup source image UV plane start address.
	ISPOST_WRITEL((ui32LcISPOST_WRITELBufAddr + ui32LcSrcYSize), ISPOST_LCSRCAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCAUV, (ui32LcSrcImgBufAddr + ui32LcSrcYSize), ui32Ret);
#else
	ui32Ret += IspostLcSrcImgAddrSet(
		ui32LcSrcImgBufAddr,
		(ui32LcSrcImgBufAddr + ui32LcSrcYSize)
		);
#endif
	// ==================================================================

	// ==================================================================
	// Setup 3D-denoise information.
	if (stCtrl0.ui1EnDN) {
		//
		// Setup DNR reference input image addr.
		// ============================
		//
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
		// Setup input image Y plane start address.
		ISPOST_WRITEL(ui32DnRefImgBufAddr, ISPOST_DNRIAY);
		CHECK_REG_WRITE_VALUE(ISPOST_DNRIAY, ui32DnRefImgBufAddr, ui32Ret);

		// Setup input image UV plane start address.
		ISPOST_WRITEL((ui32DnRefImgBufAddr + DN_REF_IMG_Y_SIZE), ISPOST_DNRIAUV);
		CHECK_REG_WRITE_VALUE(ISPOST_DNRIAUV, (ui32DnRefImgBufAddr + DN_REF_IMG_Y_SIZE), ui32Ret);
#else
		ui32Ret += IspostDnRefImgAddrSet(
			ui32DnRefImgBufAddr,
			(ui32DnRefImgBufAddr + DN_REF_IMG_Y_SIZE)
			);
#endif
	}
	//
	// Setup DNR output image addr.
	// ============================
	//
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
	// Setup input image Y plane start address.
	ISPOST_WRITEL(ui32DnOutImgBufAddr, ISPOST_DNROAY);
	CHECK_REG_WRITE_VALUE(ISPOST_DNROAY, ui32DnOutImgBufAddr, ui32Ret);

	// Setup input image UV plane start address.
	ISPOST_WRITEL((ui32DnOutImgBufAddr + DN_OUT_IMG_Y_SIZE), ISPOST_DNROAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_DNROAUV, (ui32DnOutImgBufAddr + DN_OUT_IMG_Y_SIZE), ui32Ret);
#else
	ui32Ret += IspostDnOutImgAddrSet(
		ui32DnOutImgBufAddr,
		(ui32DnOutImgBufAddr + DN_OUT_IMG_Y_SIZE)
		);
#endif
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 0 information.
	if (stCtrl0.ui1EnSS0) {
		//
		// Setup Scaling stream 0 output image information..
		// ============================
		//
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
		ui32StreamOffset = 0x00000018 * SCALING_STREAM_0;
		// Setup output image Y plane start address.
		ISPOST_WRITEL(ui32SS0OutImgBufAddr, (ISPOST_SS0AY+ui32StreamOffset));
		CHECK_REG_WRITE_VALUE((ISPOST_SS0AY + ui32StreamOffset), ui32SS0OutImgBufAddr, ui32Ret);

		// Setup output image UV plane start address.
		ISPOST_WRITEL((ui32SS0OutImgBufAddr + SS0_OUT_IMG_Y_SIZE), (ISPOST_SS0AUV+ui32StreamOffset));
		CHECK_REG_WRITE_VALUE((ISPOST_SS0AUV+ui32StreamOffset), (ui32SS0OutImgBufAddr + SS0_OUT_IMG_Y_SIZE), ui32Ret);
#else
		ui32Ret += IspostSSOutImgAddrSet(
			SCALING_STREAM_0,
			ui32SS0OutImgBufAddr,
			(ui32SS0OutImgBufAddr + SS0_OUT_IMG_Y_SIZE)
			);
#endif
	}
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 1 information.
	if (stCtrl0.ui1EnSS1) {
		//
		// Setup Scaling stream 1 output image information..
		// ============================
		//
		//
		// Setup Scaling stream 0 output image information..
		// ============================
		//
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
		ui32StreamOffset = 0x00000018 * SCALING_STREAM_1;
		// Setup output image Y plane start address.
		ISPOST_WRITEL(ui32SS1OutImgBufAddr, (ISPOST_SS0AY+ui32StreamOffset));
		CHECK_REG_WRITE_VALUE((ISPOST_SS0AY + ui32StreamOffset), ui32SS1OutImgBufAddr, ui32Ret);

		// Setup output image UV plane start address.
		ISPOST_WRITEL((ui32SS1OutImgBufAddr + SS1_OUT_IMG_Y_SIZE), (ISPOST_SS0AUV+ui32StreamOffset));
		CHECK_REG_WRITE_VALUE((ISPOST_SS0AUV+ui32StreamOffset), (ui32SS1OutImgBufAddr + SS1_OUT_IMG_Y_SIZE), ui32Ret);
#else
		ui32Ret += IspostSSOutImgAddrSet(
			SCALING_STREAM_1,
			ui32SS1OutImgBufAddr,
			(ui32SS1OutImgBufAddr + SS1_OUT_IMG_Y_SIZE)
			);
#endif
	}
	// ==================================================================

	return (ui32Ret);
}

ISPOST_RESULT
IspostLc480pOvDn1080pSS0800x480SS1720pInitial(
	ISPOST_CTRL0 stCtrl0,
	ISPOST_UINT8 ui8CBCRSwap,
	ISPOST_UINT32 ui32LcSrcImgBufAddr,
	ISPOST_UINT32 ui32LcGridBufAddr,
	ISPOST_UINT32 ui32OvImgBufAddr,
	ISPOST_UINT32 ui32DnRefImgBufAddr,
	ISPOST_UINT32 ui32DnOutImgBufAddr,
	ISPOST_UINT32 ui32SS0OutImgBufAddr,
	ISPOST_UINT32 ui32SS1OutImgBufAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	LC_INFO stLcInfo;
	OV_INFO stOvInfo;
	OV_ALPHA stOvAlpha;
	DN_INFO stDnInfo;
	DN_MSK_CRV_INFO stDnMskCrvInfo;
	SS_INFO stSS0Info;
	SS_INFO stSS1Info;
	ISPOST_INT16 curIdx = 0;
	ISPOST_INT16 crvItem = 0;
	ISPOST_UINT32 ui32CrvInfo[DN_MSK_CRV_IDX_MAX][DN_MSK_CRV_MAX] =
	{
		{0x00118000, 0x00228002, 0x00338006, 0x0152840e, 0x0d129012,
		0x05c2eb16, 0x00f3f71a, 0x0037fb22, 0x0007fda2, 0x0007fdff,
		0x0007fdff, 0x0007fdff, 0x0007fdff, 0x0007fdff, 0x0007fdff
		},
		{0x00118000, 0x00228002, 0x00338006, 0x0152840e, 0x0d129012,
		0x05c2eb16, 0x00f3f71a, 0x0037fb22, 0x0007fda2, 0x0007fdff,
		0x0007fdff, 0x0007fdff, 0x0007fdff, 0x0007fdff, 0x0007fdff
		},
		{0x1ff18000, 0x05c2ad02, 0x00f3b906, 0x0032bd0e, 0x0012bd12,
		0x0012be16, 0x0003be1a, 0x0007be22, 0x0007bfa2, 0x0007bfff,
		0x0007bfff, 0x0007bfff, 0x0007bfff, 0x0007bfff, 0x0007bfff
		}
	};

	// ==================================================================
	// Setup lens correction information.
	//
	// Setup LC SRC addr, stride and size.
	// ============================
	//
	stLcInfo.stSrcImgInfo.ui32YAddr = ui32LcSrcImgBufAddr;
	stLcInfo.stSrcImgInfo.ui16Stride = LC_SRC_IMG_STRIGE;
	stLcInfo.stSrcImgInfo.ui16Width = LC_SRC_IMG_WIDTH;
	stLcInfo.stSrcImgInfo.ui16Height = LC_SRC_IMG_HEIGHT;
	//stLcInfo.stSrcImgInfo.ui32UVAddr = stLcInfo.stSrcImgInfo.ui32YAddr +
	//	(stLcInfo.stSrcImgInfo.ui16Stride * stLcInfo.stSrcImgInfo.ui16Height);
	stLcInfo.stSrcImgInfo.ui32UVAddr = stLcInfo.stSrcImgInfo.ui32YAddr +
		(LC_SRC_IMG_STRIGE * LC_SRC_IMG_HEIGHT);
	ui32Ret += IspostLcSrcImgSet(
		stLcInfo.stSrcImgInfo.ui32YAddr,
		stLcInfo.stSrcImgInfo.ui32UVAddr,
		stLcInfo.stSrcImgInfo.ui16Stride,
		stLcInfo.stSrcImgInfo.ui16Width,
		stLcInfo.stSrcImgInfo.ui16Height
		);
	//
	// Setup LC grid buffer address, stride and grid size
	// ============================
	//
	stLcInfo.stGridBufInfo.ui32BufAddr = ui32LcGridBufAddr;
	if (stCtrl0.ui1EnLC) {
		stLcInfo.stGridBufInfo.ui16Stride = LC_GRID_BUF_STRIDE;
	} else {
		stLcInfo.stGridBufInfo.ui16Stride = 0;
	}
	stLcInfo.stGridBufInfo.ui8LineBufEnable = 0;
	stLcInfo.stGridBufInfo.ui8Size = LC_GRID_BUF_GRID_SIZE;
	ui32Ret += IspostLcGridBufSetWithStride(
		stLcInfo.stGridBufInfo.ui32BufAddr,
		stLcInfo.stGridBufInfo.ui16Stride,
		stLcInfo.stGridBufInfo.ui8LineBufEnable,
		stLcInfo.stGridBufInfo.ui8Size
		);
	//
	// Setup LC pixel cache mode
	// ============================
	//
	stLcInfo.stPxlCacheMode.ui8CtrlCfg = LC_CACHE_MODE_CTRL_CFG;
	//stLcInfo.stPxlCacheMode.ui8CBCRSwap = LC_CACHE_MODE_CBCR_SWAP;
	stLcInfo.stPxlCacheMode.ui8CBCRSwap = ui8CBCRSwap;
	ui32Ret += IspostLcPxlCacheModeSet(
		stLcInfo.stPxlCacheMode.ui8CtrlCfg,
		stLcInfo.stPxlCacheMode.ui8CBCRSwap
		);
	//
	// Setup LC pixel coordinator generator mode.
	// ============================
	//
	stLcInfo.stPxlScanMode.ui8ScanW = LC_SCAN_MODE_SCAN_W;
	stLcInfo.stPxlScanMode.ui8ScanH = LC_SCAN_MODE_SCAN_H;
	stLcInfo.stPxlScanMode.ui8ScanM = LC_SCAN_MODE_SCAN_M;
	ui32Ret += IspostLcPxlScanModeSet(
		stLcInfo.stPxlScanMode.ui8ScanW,
		stLcInfo.stPxlScanMode.ui8ScanH,
		stLcInfo.stPxlScanMode.ui8ScanM
		);
	//
	// Setup LC pixel coordinator destination size.
	// ============================
	//
	stLcInfo.stPxlDstSize.ui16Width = LC_DST_SIZE_WIDTH;
	stLcInfo.stPxlDstSize.ui16Height = LC_DST_SIZE_HEIGHT;
	ui32Ret += IspostLcPxlDstSizeSet(
		stLcInfo.stPxlDstSize.ui16Width,
		stLcInfo.stPxlDstSize.ui16Height
		);
	//
	// Setup LC AXI Read Request Limit, GBID and SRCID.
	// ============================
	//
	//stLcInfo.stAxiId.ui8SrcReqLmt = 4;
	if ((0 == stCtrl0.ui1EnLC) && (stCtrl0.ui1EnDN)) {
		stLcInfo.stAxiId.ui8SrcReqLmt = 96;
	} else {
		stLcInfo.stAxiId.ui8SrcReqLmt = 48;
		//stLcInfo.stAxiId.ui8SrcReqLmt = 56;
	}
	stLcInfo.stAxiId.ui8GBID = 1;
	stLcInfo.stAxiId.ui8SRCID = 0;
	ui32Ret += IspostLcAxiIdSet(
		stLcInfo.stAxiId.ui8SrcReqLmt,
		stLcInfo.stAxiId.ui8GBID,
		stLcInfo.stAxiId.ui8SRCID
		);

	// Fill LC back fill color Y, CB and CR value.
	ui32Ret += IspostLcBackFillColorSet(
		LC_BACK_FILL_Y,
		LC_BACK_FILL_CB,
		LC_BACK_FILL_CR
		);
	// ==================================================================

	// ==================================================================
	// Setup overlay information.
	//
	// Setup OV overlay image information.
	//
	if (stCtrl0.ui1EnLC) {
		stOvInfo.stOvImgInfo.ui32YAddr = ui32OvImgBufAddr;
		stOvInfo.stOvImgInfo.ui16Stride = OV_IMG_STRIDE;
		stOvInfo.stOvImgInfo.ui16Width = OV_IMG_OV_WIDTH;
		stOvInfo.stOvImgInfo.ui16Height = OV_IMG_OV_HEIGHT;
		stOvInfo.stOvImgInfo.ui32UVAddr = stOvInfo.stOvImgInfo.ui32YAddr +
			(OV_IMG_Y_SIZE);
		stOvInfo.ui8OverlayMode = OV_IMG_OV_MODE;
		stOvInfo.stOvOffset.ui16X = OV_IMG_OV_OFFSET_X;
		stOvInfo.stOvOffset.ui16Y = OV_IMG_OV_OFFSET_Y;
		ui32Ret += IspostOvImgSet(
			stOvInfo.stOvImgInfo.ui32YAddr,
			stOvInfo.stOvImgInfo.ui32UVAddr,
			stOvInfo.stOvImgInfo.ui16Stride,
			stOvInfo.stOvImgInfo.ui16Width,
			stOvInfo.stOvImgInfo.ui16Height
			);
	} else {
		stOvInfo.stOvImgInfo.ui32YAddr = ui32LcSrcImgBufAddr;
		stOvInfo.stOvImgInfo.ui16Stride = LC_SRC_IMG_STRIGE;
		stOvInfo.stOvImgInfo.ui16Width = LC_SRC_IMG_WIDTH;
		stOvInfo.stOvImgInfo.ui16Height = LC_SRC_IMG_HEIGHT;
		stOvInfo.stOvImgInfo.ui32UVAddr = stOvInfo.stOvImgInfo.ui32YAddr +
			(LC_SRC_IMG_STRIGE * LC_SRC_IMG_HEIGHT);
		stOvInfo.ui8OverlayMode = OV_IMG_OV_MODE;
		stOvInfo.stOvOffset.ui16X = 0;
		stOvInfo.stOvOffset.ui16Y = 0;
		ui32Ret += IspostOvImgSet(
			stOvInfo.stOvImgInfo.ui32YAddr,
			stOvInfo.stOvImgInfo.ui32UVAddr,
			stOvInfo.stOvImgInfo.ui16Stride,
			stOvInfo.stOvImgInfo.ui16Width,
			stOvInfo.stOvImgInfo.ui16Height
			);
	}
	//
	// Setup OV Set overlay mode.
	//
	ui32Ret += IspostOvModeSet(
		stOvInfo.ui8OverlayMode
		);
	//
	// Setup OV overlay offset.
	//
	ui32Ret += IspostOvOffsetSet(
		stOvInfo.stOvOffset.ui16X,
		stOvInfo.stOvOffset.ui16Y
		);
	//
	// Setup OV AXI read ID.
	//
	stOvInfo.ui8OVID = 2;
	ui32Ret += IspostOvAxiIdSet(
		stOvInfo.ui8OVID
		);
	if (stCtrl0.ui1EnOV) {
		//
		// Setup OV overlay alpha value 0.
		//
		stOvAlpha.stValue0.ui8Alpha3 = 0x33;
		stOvAlpha.stValue0.ui8Alpha2 = 0x22;
		stOvAlpha.stValue0.ui8Alpha1 = 0x11;
		stOvAlpha.stValue0.ui8Alpha0 = 0x00;
		// Setup OV overlay alpha value 1.
		stOvAlpha.stValue1.ui8Alpha3 = 0x77;
		stOvAlpha.stValue1.ui8Alpha2 = 0x66;
		stOvAlpha.stValue1.ui8Alpha1 = 0x55;
		stOvAlpha.stValue1.ui8Alpha0 = 0x44;
		// Setup OV overlay alpha value 2.
		stOvAlpha.stValue2.ui8Alpha3 = 0xbb;
		stOvAlpha.stValue2.ui8Alpha2 = 0xaa;
		stOvAlpha.stValue2.ui8Alpha1 = 0x99;
		stOvAlpha.stValue2.ui8Alpha0 = 0x88;
		// Setup OV overlay alpha value 3.
		stOvAlpha.stValue3.ui8Alpha3 = 0xff;
		stOvAlpha.stValue3.ui8Alpha2 = 0xee;
		stOvAlpha.stValue3.ui8Alpha1 = 0xdd;
		stOvAlpha.stValue3.ui8Alpha0 = 0xcc;
		ui32Ret += IspostOverlayAlphaValueSet(stOvAlpha);
	}
	// ==================================================================

	// ==================================================================
	// Setup 3D-denoise information.
	if (stCtrl0.ui1EnDN) {
		//
		// Setup DNR reference input image addr, stride.
		// ============================
		//
		stDnInfo.stRefImgInfo.ui32YAddr = ui32DnRefImgBufAddr;
		stDnInfo.stRefImgInfo.ui32UVAddr = stDnInfo.stRefImgInfo.ui32YAddr +
			(stLcInfo.stPxlDstSize.ui16Width * stLcInfo.stPxlDstSize.ui16Height);
		stDnInfo.stRefImgInfo.ui16Stride = stLcInfo.stPxlDstSize.ui16Width;
		ui32Ret += IspostDnRefImgSet(
			stDnInfo.stRefImgInfo.ui32YAddr,
			stDnInfo.stRefImgInfo.ui32UVAddr,
			stDnInfo.stRefImgInfo.ui16Stride
			);

		// Set DN mask curve
		for (curIdx = 0; curIdx < DN_MSK_CRV_IDX_MAX; ++curIdx)
		{
			stDnMskCrvInfo.ui8MskCrvIdx = curIdx;
			for (crvItem = 0; crvItem < DN_MSK_CRV_MAX; ++crvItem)
				stDnMskCrvInfo.stMskCrv[crvItem].ui32MskCrv = ui32CrvInfo[curIdx][crvItem];
			ui32Ret += Ispost3DDenoiseMskCrvSet(stDnMskCrvInfo);
		}
	}
	//
	// Setup DNR output image addr, stride.
	// ============================
	//
	stDnInfo.stOutImgInfo.ui32YAddr = ui32DnOutImgBufAddr;
	stDnInfo.stOutImgInfo.ui32UVAddr = stDnInfo.stOutImgInfo.ui32YAddr +
		(stLcInfo.stPxlDstSize.ui16Width * stLcInfo.stPxlDstSize.ui16Height);
	stDnInfo.stOutImgInfo.ui16Stride = stLcInfo.stPxlDstSize.ui16Width;
	ui32Ret += IspostDnOutImgSet(
		stDnInfo.stOutImgInfo.ui32YAddr,
		stDnInfo.stOutImgInfo.ui32UVAddr,
		stDnInfo.stOutImgInfo.ui16Stride
		);
	// Setup DN AXI reference write and read ID.
	stDnInfo.stAxiId.ui8RefWrID = 3;
	stDnInfo.stAxiId.ui8RefRdID = 3;
	ui32Ret += IspostDnAxiIdSet(
		stDnInfo.stAxiId.ui8RefWrID,
		stDnInfo.stAxiId.ui8RefRdID
		);
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 0 information.
	if (stCtrl0.ui1EnSS0) {
		//
		// Setup Scaling stream 0 output image information..
		// ============================
		//
		// Fill SS0 output stream select.
		stSS0Info.ui8StreamSel = SCALING_STREAM_0;
		// Fill SS0 output image information.
		stSS0Info.stOutImgInfo.ui32YAddr = ui32SS0OutImgBufAddr;
		stSS0Info.stOutImgInfo.ui16Stride = SS0_OUT_IMG_STRIDE;
		stSS0Info.stOutImgInfo.ui16Width = SS0_OUT_IMG_WIDTH;
		stSS0Info.stOutImgInfo.ui16Height = SS0_OUT_IMG_HEIGHT;
		stSS0Info.stOutImgInfo.ui32UVAddr = stSS0Info.stOutImgInfo.ui32YAddr +
			(stSS0Info.stOutImgInfo.ui16Width * stSS0Info.stOutImgInfo.ui16Height);
		stSS0Info.stHSF.ui16SF = SS0_OUT_IMG_HSF_SF;
		stSS0Info.stHSF.ui8SM = SS0_OUT_IMG_HSF_SM;
		stSS0Info.stVSF.ui16SF = SS0_OUT_IMG_VSF_SF;
		stSS0Info.stVSF.ui8SM = SS0_OUT_IMG_VSF_SM;
		// Fill SS0 AXI SSWID.
		stSS0Info.ui8SSWID = 1;
		ui32Ret += IspostScaleStreamSet(stSS0Info);
	}
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 1 information.
	if (stCtrl0.ui1EnSS1) {
		//
		// Setup Scaling stream 1 output image information..
		// ============================
		//
		// Fill SS1 output stream select.
		stSS1Info.ui8StreamSel = SCALING_STREAM_1;
		// Fill SS1 output image information.
		stSS1Info.stOutImgInfo.ui32YAddr = ui32SS1OutImgBufAddr;
		stSS1Info.stOutImgInfo.ui16Stride = SS1_OUT_IMG_STRIDE;
		stSS1Info.stOutImgInfo.ui16Width = SS1_OUT_IMG_WIDTH;
		stSS1Info.stOutImgInfo.ui16Height = SS1_OUT_IMG_HEIGHT;
		stSS1Info.stOutImgInfo.ui32UVAddr = stSS1Info.stOutImgInfo.ui32YAddr +
			(stSS1Info.stOutImgInfo.ui16Width * stSS1Info.stOutImgInfo.ui16Height);
		stSS1Info.stHSF.ui16SF = SS1_OUT_IMG_HSF_SF;
		stSS1Info.stHSF.ui8SM = SS1_OUT_IMG_HSF_SM;
		stSS1Info.stVSF.ui16SF = SS1_OUT_IMG_VSF_SF;
		stSS1Info.stVSF.ui8SM = SS1_OUT_IMG_VSF_SM;
		// Fill SS1 AXI SSWID.
		stSS1Info.ui8SSWID = 2;
		ui32Ret += IspostScaleStreamSet(stSS1Info);
	}
	// ==================================================================

	return (ui32Ret);
}

ISPOST_RESULT
IspostLc480pOvDn1080pSS0800x480SS1720pSetBuf(
	ISPOST_CTRL0 stCtrl0,
	ISPOST_UINT32 ui32LcSrcImgBufAddr,
	ISPOST_UINT32 ui32DnRefImgBufAddr,
	ISPOST_UINT32 ui32DnOutImgBufAddr,
	ISPOST_UINT32 ui32SS0OutImgBufAddr,
	ISPOST_UINT32 ui32SS1OutImgBufAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32LcSrcYSize;
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
//	ISPOST_UINT32 ui32Data;
	ISPOST_UINT32 ui32StreamOffset;
#endif

	// ==================================================================
	// Setup lens correction information.
	//
	// Setup LC SRC addr.
	// ============================
	//
	ui32LcSrcYSize = LC_SRC_IMG_Y_SIZE;
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
	// Setup source image Y plane start address.
	ISPOST_WRITEL(ui32LcSrcImgBufAddr, ISPOST_LCSRCAY);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCAY, ui32LcSrcImgBufAddr, ui32Ret);

	// Setup source image UV plane start address.
	ISPOST_WRITEL((ui32LcSrcImgBufAddr + ui32LcSrcYSize), ISPOST_LCSRCAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCAUV, (ui32LcSrcImgBufAddr + ui32LcSrcYSize), ui32Ret);
#else
	ui32Ret += IspostLcSrcImgAddrSet(
		ui32LcSrcImgBufAddr,
		(ui32LcSrcImgBufAddr + ui32LcSrcYSize)
		);
#endif
	// ==================================================================

	// ==================================================================
	// Setup 3D-denoise information.
	if (stCtrl0.ui1EnDN) {
		//
		// Setup DNR reference input image addr.
		// ============================
		//
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
		// Setup input image Y plane start address.
		ISPOST_WRITEL(ui32DnRefImgBufAddr, ISPOST_DNRIAY);
		CHECK_REG_WRITE_VALUE(ISPOST_DNRIAY, ui32DnRefImgBufAddr, ui32Ret);

		// Setup input image UV plane start address.
		ISPOST_WRITEL((ui32DnRefImgBufAddr + DN_REF_IMG_Y_SIZE), ISPOST_DNRIAUV);
		CHECK_REG_WRITE_VALUE(ISPOST_DNRIAUV, (ui32DnRefImgBufAddr + DN_REF_IMG_Y_SIZE), ui32Ret);
#else
		ui32Ret += IspostDnRefImgAddrSet(
			ui32DnRefImgBufAddr,
			(ui32DnRefImgBufAddr + DN_REF_IMG_Y_SIZE)
			);
#endif
	}
	//
	// Setup DNR output image addr.
	// ============================
	//
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
	// Setup input image Y plane start address.
	ISPOST_WRITEL(ui32DnOutImgBufAddr, ISPOST_DNROAY);
	CHECK_REG_WRITE_VALUE(ISPOST_DNROAY, ui32DnOutImgBufAddr, ui32Ret);

	// Setup input image UV plane start address.
	ISPOST_WRITEL((ui32DnOutImgBufAddr + DN_OUT_IMG_Y_SIZE), ISPOST_DNROAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_DNROAUV, (ui32DnOutImgBufAddr + DN_OUT_IMG_Y_SIZE), ui32Ret);
#else
	ui32Ret += IspostDnOutImgAddrSet(
		ui32DnOutImgBufAddr,
		(ui32DnOutImgBufAddr + DN_OUT_IMG_Y_SIZE)
		);
#endif
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 0 information.
	if (stCtrl0.ui1EnSS0) {
		//
		// Setup Scaling stream 0 output image information..
		// ============================
		//
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
		ui32StreamOffset = 0x00000018 * SCALING_STREAM_0;
		// Setup output image Y plane start address.
		ISPOST_WRITEL(ui32SS0OutImgBufAddr, (ISPOST_SS0AY+ui32StreamOffset));
		CHECK_REG_WRITE_VALUE((ISPOST_SS0AY + ui32StreamOffset), ui32SS0OutImgBufAddr, ui32Ret);

		// Setup output image UV plane start address.
		ISPOST_WRITEL((ui32SS0OutImgBufAddr + SS0_OUT_IMG_Y_SIZE), (ISPOST_SS0AUV+ui32StreamOffset));
		CHECK_REG_WRITE_VALUE((ISPOST_SS0AUV+ui32StreamOffset), (ui32SS0OutImgBufAddr + SS0_OUT_IMG_Y_SIZE), ui32Ret);
#else
		ui32Ret += IspostSSOutImgAddrSet(
			SCALING_STREAM_0,
			ui32SS0OutImgBufAddr,
			(ui32SS0OutImgBufAddr + SS0_OUT_IMG_Y_SIZE)
			);
#endif
	}
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 1 information.
	if (stCtrl0.ui1EnSS1) {
		//
		// Setup Scaling stream 1 output image information..
		// ============================
		//
		//
		// Setup Scaling stream 0 output image information..
		// ============================
		//
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
		ui32StreamOffset = 0x00000018 * SCALING_STREAM_1;
		// Setup output image Y plane start address.
		ISPOST_WRITEL(ui32SS1OutImgBufAddr, (ISPOST_SS0AY+ui32StreamOffset));
		CHECK_REG_WRITE_VALUE((ISPOST_SS0AY + ui32StreamOffset), ui32SS1OutImgBufAddr, ui32Ret);

		// Setup output image UV plane start address.
		ISPOST_WRITEL((ui32SS1OutImgBufAddr + SS1_OUT_IMG_Y_SIZE), (ISPOST_SS0AUV+ui32StreamOffset));
		CHECK_REG_WRITE_VALUE((ISPOST_SS0AUV+ui32StreamOffset), (ui32SS1OutImgBufAddr + SS1_OUT_IMG_Y_SIZE), ui32Ret);
#else
		ui32Ret += IspostSSOutImgAddrSet(
			SCALING_STREAM_1,
			ui32SS1OutImgBufAddr,
			(ui32SS1OutImgBufAddr + SS1_OUT_IMG_Y_SIZE)
			);
#endif
	}
	// ==================================================================

	return (ui32Ret);
}


ISPOST_RESULT IspostUserSetup(ISPOST_USER* pIspostUser)
{
	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_CTRL)
	{
		printk("@@@ Ispost Ctrl: %d %d %d %d %d\n",
						pIspostUser->stCtrl0.ui1EnLC,
						pIspostUser->stCtrl0.ui1EnOV,
						pIspostUser->stCtrl0.ui1EnDN,
						pIspostUser->stCtrl0.ui1EnSS0,
						pIspostUser->stCtrl0.ui1EnSS1);
		printk("@@@ LC Width x Height: %d x %d\n", pIspostUser->ui32LcSrcWidth, pIspostUser->ui32LcSrcHeight);
		printk("@@@ OV Width x Height: %d x %d\n", pIspostUser->ui32OvImgWidth, pIspostUser->ui32OvImgHeight);
		printk("@@@ DN Width x Height: %d x %d\n", pIspostUser->ui32LcDstWidth, pIspostUser->ui32LcDstHeight);
		printk("@@@ SS0 Width x Height: %d x %d\n", pIspostUser->ui32SS0OutImgDstWidth, pIspostUser->ui32SS0OutImgDstHeight);
		printk("@@@ SS1 Width x Height: %d x %d\n", pIspostUser->ui32SS1OutImgDstWidth, pIspostUser->ui32SS1OutImgDstHeight);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG

	if (IspostUserSetupLC(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;
	if (IspostUserSetupOV(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;
	if (IspostUserSetupDN(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;
	if (IspostUserSetupSS0(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;
	if (IspostUserSetupSS1(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;

	return ISPOST_TRUE;
}

ISPOST_RESULT IspostUserSetupLC(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	LC_INFO stLcInfo;
	ISPOST_UINT32 value = 0, stride = 0, length = 0;

	// ==================================================================
	// Setup lens correction information.
	//
	// Setup LC SRC addr, stride and size.
	// ==================================================================
	//

	stLcInfo.stSrcImgInfo.ui16Width = pIspostUser->ui32LcSrcWidth;
	stLcInfo.stSrcImgInfo.ui16Height = pIspostUser->ui32LcSrcHeight;
	stLcInfo.stSrcImgInfo.ui16Stride = pIspostUser->ui32LcSrcWidth;
	stLcInfo.stSrcImgInfo.ui32YAddr = pIspostUser->ui32LcSrcBufYaddr;
	if (pIspostUser->ui32LcSrcBufUVaddr == 0)
		stLcInfo.stSrcImgInfo.ui32UVAddr =  stLcInfo.stSrcImgInfo.ui32YAddr + (stLcInfo.stSrcImgInfo.ui16Stride * stLcInfo.stSrcImgInfo.ui16Height);
	else
		stLcInfo.stSrcImgInfo.ui32UVAddr =  pIspostUser->ui32LcSrcBufUVaddr;

	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_LC)
	{
		printk("@@@ stLcInfo.stSrcImgInfo.ui32YAddr: 0x%X\n", stLcInfo.stSrcImgInfo.ui32YAddr);
		printk("@@@ stLcInfo.stSrcImgInfo.ui32UVAddr: 0x%X\n", stLcInfo.stSrcImgInfo.ui32UVAddr);
		printk("@@@ stLcInfo.stSrcImgInfo.ui16Width: %d\n", stLcInfo.stSrcImgInfo.ui16Width);
		printk("@@@ stLcInfo.stSrcImgInfo.ui16Height: %d\n", stLcInfo.stSrcImgInfo.ui16Height);
		printk("@@@ stLcInfo.stSrcImgInfo.ui16Stride: %d\n", stLcInfo.stSrcImgInfo.ui16Stride);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG
	ui32Ret += IspostLcSrcImgSet(
		stLcInfo.stSrcImgInfo.ui32YAddr,
		stLcInfo.stSrcImgInfo.ui32UVAddr,
		stLcInfo.stSrcImgInfo.ui16Stride,
		stLcInfo.stSrcImgInfo.ui16Width,
		stLcInfo.stSrcImgInfo.ui16Height
		);

	// ==================================================================
	// Setup LC grid buffer address, stride and grid size
	// ==================================================================
	//
	if (pIspostUser->stCtrl0.ui1EnLC)
		stLcInfo.stGridBufInfo.ui32BufAddr = pIspostUser->ui32LcGridBufAddr;
	else
		stLcInfo.stGridBufInfo.ui32BufAddr = 0;

	stride = IspostLcGridBufStrideCalc((ISPOST_UINT16)pIspostUser->ui32LcDstWidth, (ISPOST_UINT8)pIspostUser->ui32LcGridSize);
	length = IspostLcGridBufLengthCalc((ISPOST_UINT16)pIspostUser->ui32LcDstHeight, stride, (ISPOST_UINT8)pIspostUser->ui32LcGridSize);
	value = (length+63)&0xFFFFFFC0;
	stLcInfo.stGridBufInfo.ui32BufAddr += pIspostUser->ui32LcGridTargetIdx * value;//34176;//19648 //34160 is 32x32 GridData one file size, alignment 64 is 34176
	stLcInfo.stGridBufInfo.ui8LineBufEnable = (ISPOST_UINT8)pIspostUser->ui32LcGridLineBufEn;
	stLcInfo.stGridBufInfo.ui8Size = (ISPOST_UINT8)pIspostUser->ui32LcGridSize;
	if (pIspostUser->stCtrl0.ui1EnLC)
		stLcInfo.stGridBufInfo.ui16Stride = stride;//IspostLcGridBufStrideCalc((ISPOST_UINT16)pIspostUser->ui32LcDstWidth, stLcInfo.stGridBufInfo.ui8Size);
	else
		stLcInfo.stGridBufInfo.ui16Stride = 0;

	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_LC)
	{
		printk("@@@ stLcInfo.stGridBufInfo.ui32BufAddr: 0x%X\n", stLcInfo.stGridBufInfo.ui32BufAddr);
		printk("@@@ stLcInfo.stGridBufInfo.ui8LineBufEnable: %d\n", stLcInfo.stGridBufInfo.ui8LineBufEnable);
		printk("@@@ stLcInfo.stGridBufInfo.ui8Size: %d\n", stLcInfo.stGridBufInfo.ui8Size);
		printk("@@@ stLcInfo.stGridBufInfo.ui16Stride: %d\n", stLcInfo.stGridBufInfo.ui16Stride);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG
	ui32Ret += IspostLcGridBufSetWithStride(stLcInfo.stGridBufInfo.ui32BufAddr,	stLcInfo.stGridBufInfo.ui16Stride, stLcInfo.stGridBufInfo.ui8LineBufEnable, stLcInfo.stGridBufInfo.ui8Size);


	// ==================================================================
	// Setup LC pixel cache mode
	// ==================================================================
	//
	stLcInfo.stPxlCacheMode.ui8CtrlCfg = (ISPOST_UINT8)pIspostUser->ui32LcCacheModeCtrlCfg;
	stLcInfo.stPxlCacheMode.ui8CBCRSwap = (ISPOST_UINT8)pIspostUser->ui32LcCBCRSwap;
	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_LC)
	{
		printk("@@@ stLcInfo.stPxlCacheMode.ui8CtrlCfg: %d\n", stLcInfo.stPxlCacheMode.ui8CtrlCfg);
		printk("@@@ stLcInfo.stPxlCacheMode.ui8CBCRSwap: %d\n", stLcInfo.stPxlCacheMode.ui8CBCRSwap);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG
	ui32Ret += IspostLcPxlCacheModeSet(stLcInfo.stPxlCacheMode.ui8CtrlCfg, stLcInfo.stPxlCacheMode.ui8CBCRSwap);


	// ==================================================================
	// Setup LC pixel coordinator generator mode.
	// ==================================================================
	//
	stLcInfo.stPxlScanMode.ui8ScanW = (ISPOST_UINT8)pIspostUser->ui32LcScanModeScanW;
	stLcInfo.stPxlScanMode.ui8ScanH = (ISPOST_UINT8)pIspostUser->ui32LcScanModeScanH;
	stLcInfo.stPxlScanMode.ui8ScanM = (ISPOST_UINT8)pIspostUser->ui32LcScanModeScanM;
	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_LC)
	{
		printk("@@@ stLcInfo.stPxlScanMode.ui8ScanW: %d\n", stLcInfo.stPxlScanMode.ui8ScanW);
		printk("@@@ stLcInfo.stPxlScanMode.ui8ScanH: %d\n", stLcInfo.stPxlScanMode.ui8ScanH);
		printk("@@@ stLcInfo.stPxlScanMode.ui8ScanM: %d\n", stLcInfo.stPxlScanMode.ui8ScanM);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG
	ui32Ret += IspostLcPxlScanModeSet(stLcInfo.stPxlScanMode.ui8ScanW, stLcInfo.stPxlScanMode.ui8ScanH,	stLcInfo.stPxlScanMode.ui8ScanM);


	// ==================================================================
	// Setup LC pixel coordinator destination size.
	// ==================================================================
	//
	stLcInfo.stPxlDstSize.ui16Width = (ISPOST_UINT16)pIspostUser->ui32LcDstWidth;
	stLcInfo.stPxlDstSize.ui16Height = (ISPOST_UINT16)pIspostUser->ui32LcDstHeight;
	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_LC)
	{
		printk("@@@ stLcInfo.stPxlDstSize.ui16Width: %d\n", stLcInfo.stPxlDstSize.ui16Width);
		printk("@@@ stLcInfo.stPxlDstSize.ui16Height: %d\n", stLcInfo.stPxlDstSize.ui16Height);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG
	ui32Ret += IspostLcPxlDstSizeSet(stLcInfo.stPxlDstSize.ui16Width, stLcInfo.stPxlDstSize.ui16Height);

	// ==================================================================
	// Setup LC AXI Read Request Limit, GBID and SRCID.
	// ==================================================================
	//
	if ((ISPOST_DISABLE == pIspostUser->stCtrl0.ui1EnLC) && (pIspostUser->stCtrl0.ui1EnDN)) //Denoise only
		stLcInfo.stAxiId.ui8SrcReqLmt = 96;
	else
		stLcInfo.stAxiId.ui8SrcReqLmt = 4; //48 //56

	stLcInfo.stAxiId.ui8GBID = 2;
	stLcInfo.stAxiId.ui8SRCID = 1;
	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_LC)
	{
		printk("@@@ stLcInfo.stAxiId.ui8SrcReqLmt: %d\n", stLcInfo.stAxiId.ui8SrcReqLmt);
		printk("@@@ stLcInfo.stAxiId.ui8GBID: %d\n", stLcInfo.stAxiId.ui8GBID);
		printk("@@@ stLcInfo.stAxiId.ui8SRCID: %d\n", stLcInfo.stAxiId.ui8SRCID);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG
	ui32Ret += IspostLcAxiIdSet(stLcInfo.stAxiId.ui8SrcReqLmt, stLcInfo.stAxiId.ui8GBID, stLcInfo.stAxiId.ui8SRCID);


	// Fill LC back fill color Y, CB and CR value.
	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_LC)
	{
		printk("@@@ stLcInfo.ui32LcFillY: %d\n", pIspostUser->ui32LcFillY);
		printk("@@@ stLcInfo.ui32LcFillCB: %d\n", pIspostUser->ui32LcFillCB);
		printk("@@@ stLcInfo.ui32LcFillCR: %d\n", pIspostUser->ui32LcFillCR);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG
	ui32Ret += IspostLcBackFillColorSet(
					(ISPOST_UINT8)pIspostUser->ui32LcFillY,
					(ISPOST_UINT8)pIspostUser->ui32LcFillCB,
					(ISPOST_UINT8)pIspostUser->ui32LcFillCR);

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_LC;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserSetupOV(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	OV_INFO stOvInfo;
	OV_ALPHA stOvAlpha;

	// ==================================================================
	// Setup OV overlay image information.
	// ==================================================================
	//
	if (pIspostUser->stCtrl0.ui1EnLC)
	{
		stOvInfo.stOvImgInfo.ui16Width = (ISPOST_UINT16)pIspostUser->ui32OvImgWidth;
		stOvInfo.stOvImgInfo.ui16Height = (ISPOST_UINT16)pIspostUser->ui32OvImgHeight;
		stOvInfo.stOvImgInfo.ui16Stride = (ISPOST_UINT16)pIspostUser->ui32OvImgStride;
		if (pIspostUser->stCtrl0.ui1EnOV)
			stOvInfo.stOvImgInfo.ui32YAddr = pIspostUser->ui32OvImgBufYaddr;
		else
			stOvInfo.stOvImgInfo.ui32YAddr = 0;
		if (pIspostUser->ui32OvImgBufUVaddr == 0)
			stOvInfo.stOvImgInfo.ui32UVAddr = stOvInfo.stOvImgInfo.ui32YAddr + (stOvInfo.stOvImgInfo.ui16Stride * stOvInfo.stOvImgInfo.ui16Height);
		else
			stOvInfo.stOvImgInfo.ui32UVAddr = pIspostUser->ui32OvImgBufUVaddr;
		stOvInfo.ui8OverlayMode = (ISPOST_UINT8)pIspostUser->ui32OvMode;
		stOvInfo.stOvOffset.ui16X = (ISPOST_UINT16)pIspostUser->ui32OvImgOffsetX;
		stOvInfo.stOvOffset.ui16Y = (ISPOST_UINT16)pIspostUser->ui32OvImgOffsetY;
		#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if (pIspostUser->ui32PrintFlag & ISPOST_PRN_OV)
		{
			printk("@@@ stOvInfo.stOvImgInfo.ui32YAddr: 0x%X\n", stOvInfo.stOvImgInfo.ui32YAddr);
			printk("@@@ stOvInfo.stOvImgInfo.ui32UVAddr: 0x%X\n", stOvInfo.stOvImgInfo.ui32UVAddr);
			printk("@@@ stOvInfo.stOvImgInfo.ui16Width: %d\n", stOvInfo.stOvImgInfo.ui16Width);
			printk("@@@ stOvInfo.stOvImgInfo.ui16Height: %d\n", stOvInfo.stOvImgInfo.ui16Height);
			printk("@@@ stOvInfo.stOvImgInfo.ui16Stride: %d\n", stOvInfo.stOvImgInfo.ui16Stride);
		}
		#endif //INFOTM_ENABLE_ISPOST_DEBUG
		ui32Ret += IspostOvImgSet(
			stOvInfo.stOvImgInfo.ui32YAddr,
			stOvInfo.stOvImgInfo.ui32UVAddr,
			stOvInfo.stOvImgInfo.ui16Stride,
			stOvInfo.stOvImgInfo.ui16Width,
			stOvInfo.stOvImgInfo.ui16Height
			);
	}
	else //input from overlay
	{
		stOvInfo.stOvImgInfo.ui16Width = (ISPOST_UINT16)pIspostUser->ui32LcSrcWidth;
		stOvInfo.stOvImgInfo.ui16Height = (ISPOST_UINT16)pIspostUser->ui32LcSrcHeight;
		stOvInfo.stOvImgInfo.ui16Stride = (ISPOST_UINT16)pIspostUser->ui32LcSrcWidth;
		stOvInfo.stOvImgInfo.ui32YAddr = pIspostUser->ui32OvImgBufYaddr;
		if (pIspostUser->ui32OvImgBufUVaddr == 0)
			stOvInfo.stOvImgInfo.ui32UVAddr = stOvInfo.stOvImgInfo.ui32YAddr + (stOvInfo.stOvImgInfo.ui16Stride * stOvInfo.stOvImgInfo.ui16Height);
		else
			stOvInfo.stOvImgInfo.ui32UVAddr = pIspostUser->ui32OvImgBufUVaddr;

		stOvInfo.ui8OverlayMode = OV_IMG_OV_MODE;
		stOvInfo.stOvOffset.ui16X = 0;
		stOvInfo.stOvOffset.ui16Y = 0;
		ui32Ret += IspostOvImgSet(
			stOvInfo.stOvImgInfo.ui32YAddr,
			stOvInfo.stOvImgInfo.ui32UVAddr,
			stOvInfo.stOvImgInfo.ui16Stride,
			stOvInfo.stOvImgInfo.ui16Width,
			stOvInfo.stOvImgInfo.ui16Height
			);
	}

	// Setup OV Set overlay mode.
	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_OV)
	{
		printk("@@@ stOvInfo.ui8OverlayMode: %d\n", stOvInfo.ui8OverlayMode);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG
	ui32Ret += IspostOvModeSet(stOvInfo.ui8OverlayMode);

	// Setup OV overlay offset.
	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_OV)
	{
		printk("@@@ stOvInfo.stOvOffset.ui16X: %d\n", stOvInfo.stOvOffset.ui16X);
		printk("@@@ stOvInfo.stOvOffset.ui16Y: %d\n", stOvInfo.stOvOffset.ui16Y);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG
	ui32Ret += IspostOvOffsetSet(stOvInfo.stOvOffset.ui16X,	stOvInfo.stOvOffset.ui16Y);

	// Setup OV AXI read ID.
	stOvInfo.ui8OVID = 2;
	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_OV)
	{
		printk("@@@ stOvInfo.ui8OVID: %d\n", stOvInfo.ui8OVID);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG
	ui32Ret += IspostOvAxiIdSet(stOvInfo.ui8OVID);
	if (pIspostUser->stCtrl0.ui1EnOV)
	{
		// Setup OV overlay alpha value
//		stOvAlpha.stValue0.ui32Alpha = 0x33221100;
//		stOvAlpha.stValue1.ui32Alpha = 0x77665544;
//		stOvAlpha.stValue2.ui32Alpha = 0xBBAA9988;
//		stOvAlpha.stValue3.ui32Alpha = 0xFFEEDDCC;
		stOvAlpha.stValue0.ui32Alpha = pIspostUser->ui32OvAlphaValue0;
		stOvAlpha.stValue1.ui32Alpha = pIspostUser->ui32OvAlphaValue1;
		stOvAlpha.stValue2.ui32Alpha = pIspostUser->ui32OvAlphaValue2;
		stOvAlpha.stValue3.ui32Alpha = pIspostUser->ui32OvAlphaValue3;

		if (pIspostUser->ui32PrintFlag & ISPOST_PRN_OV)
		{
			printk("@@@ stOvAlpha.stValue0.ui32Alpha: %08X\n", stOvAlpha.stValue0.ui32Alpha);
			printk("@@@ stOvAlpha.stValue1.ui32Alpha: %08X\n", stOvAlpha.stValue1.ui32Alpha);
			printk("@@@ stOvAlpha.stValue2.ui32Alpha: %08X\n", stOvAlpha.stValue2.ui32Alpha);
			printk("@@@ stOvAlpha.stValue3.ui32Alpha: %08X\n", stOvAlpha.stValue3.ui32Alpha);
		}
		ui32Ret += IspostOverlayAlphaValueSet(stOvAlpha);
	}

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_OV;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserSetupDN(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	DN_INFO stDnInfo;
	ISPOST_UINT32 CrvIdx;


	// ==================================================================
	// Setup 3D-denoise information.
	// ==================================================================
	if (pIspostUser->stCtrl0.ui1EnDN)
	{
		//
		// Setup DNR reference input image addr, stride.
		// ============================
		//
		stDnInfo.stRefImgInfo.ui32YAddr = pIspostUser->ui32DnRefImgBufYaddr;
		if (pIspostUser->ui32DnRefImgBufUVaddr == 0xFFFFFFFF)
			stDnInfo.stRefImgInfo.ui32UVAddr = stDnInfo.stRefImgInfo.ui32YAddr + (pIspostUser->ui32LcDstWidth * pIspostUser->ui32LcDstHeight);
		else
			stDnInfo.stRefImgInfo.ui32UVAddr = pIspostUser->ui32DnRefImgBufUVaddr;
		if (pIspostUser->ui32DnRefImgBufStride == 0)
			stDnInfo.stRefImgInfo.ui16Stride = (ISPOST_UINT16)pIspostUser->ui32LcDstWidth;
		else
			stDnInfo.stRefImgInfo.ui16Stride = (ISPOST_UINT16)pIspostUser->ui32DnRefImgBufStride;

		#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if (pIspostUser->ui32PrintFlag & ISPOST_PRN_DN)
		{
			printk("@@@ stDnInfo.stRefImgInfo.ui32YAddr: 0x%X\n", stDnInfo.stRefImgInfo.ui32YAddr);
			printk("@@@ stDnInfo.stRefImgInfo.ui32UVAddr: 0x%X\n", stDnInfo.stRefImgInfo.ui32UVAddr);
			printk("@@@ stDnInfo.stRefImgInfo.ui16Stride: %d\n", stDnInfo.stRefImgInfo.ui16Stride);
		}
		#endif //INFOTM_ENABLE_ISPOST_DEBUG
		ui32Ret += IspostDnRefImgSet(stDnInfo.stRefImgInfo.ui32YAddr, stDnInfo.stRefImgInfo.ui32UVAddr,	stDnInfo.stRefImgInfo.ui16Stride);


		for (CrvIdx = 0; CrvIdx < DN_MSK_CRV_IDX_MAX; CrvIdx++)
			ui32Ret += Ispost3DDenoiseMskCrvSet(pIspostUser->stDnMskCrvInfo[CrvIdx]);
	}

	// Setup DNR output image addr, stride.
	stDnInfo.stOutImgInfo.ui32YAddr = pIspostUser->ui32DnOutImgBufYaddr;
	if (pIspostUser->ui32DnOutImgBufUVaddr == 0xFFFFFFFF)
		stDnInfo.stOutImgInfo.ui32UVAddr = stDnInfo.stOutImgInfo.ui32YAddr + (pIspostUser->ui32LcDstWidth * pIspostUser->ui32LcDstHeight);
	else
		stDnInfo.stOutImgInfo.ui32UVAddr = pIspostUser->ui32DnOutImgBufUVaddr;
	if (pIspostUser->ui32DnOutImgBufStride == 0)
		stDnInfo.stOutImgInfo.ui16Stride = (ISPOST_UINT16)pIspostUser->ui32LcDstWidth;
	else
		stDnInfo.stOutImgInfo.ui16Stride = pIspostUser->ui32DnOutImgBufStride;

	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_DN)
	{
		printk("@@@ stDnInfo.stOutImgInfo.ui32YAddr: 0x%X\n", stDnInfo.stOutImgInfo.ui32YAddr);
		printk("@@@ stDnInfo.stOutImgInfo.ui32UVAddr: 0x%X\n", stDnInfo.stOutImgInfo.ui32UVAddr);
		printk("@@@ stDnInfo.stOutImgInfo.ui16Stride: %d\n", stDnInfo.stOutImgInfo.ui16Stride);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG
	ui32Ret += IspostDnOutImgSet(stDnInfo.stOutImgInfo.ui32YAddr, stDnInfo.stOutImgInfo.ui32UVAddr,	stDnInfo.stOutImgInfo.ui16Stride);

	// Setup DN AXI reference write and read ID.
	stDnInfo.stAxiId.ui8RefWrID = 3;
	stDnInfo.stAxiId.ui8RefRdID = 3;
	#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if (pIspostUser->ui32PrintFlag & ISPOST_PRN_DN)
	{
		printk("@@@ stDnInfo.stAxiId.ui16Stride: %d\n", stDnInfo.stAxiId.ui8RefWrID);
		printk("@@@ stDnInfo.stAxiId.ui8RefRdID: %d\n", stDnInfo.stAxiId.ui8RefRdID);
	}
	#endif //INFOTM_ENABLE_ISPOST_DEBUG
	ui32Ret += IspostDnAxiIdSet(stDnInfo.stAxiId.ui8RefWrID, stDnInfo.stAxiId.ui8RefRdID);

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_DN;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserSetupSS0(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	SS_INFO stSS0Info;

	// ==================================================================
	// Setup scaling stream 0 information.
	// ==================================================================
	if (pIspostUser->stCtrl0.ui1EnSS0)
	{
		//
		// Setup Scaling stream 0 output image information..
		// ============================
		//
		if (pIspostUser->ui32SS0OutImgDstWidth >= pIspostUser->ui32SS0OutImgMaxWidth)
			pIspostUser->ui32SS0OutImgDstWidth = pIspostUser->ui32SS0OutImgMaxWidth;
		if (pIspostUser->ui32SS0OutImgDstHeight >= pIspostUser->ui32SS0OutImgMaxHeight)
			pIspostUser->ui32SS0OutImgDstHeight = pIspostUser->ui32SS0OutImgMaxHeight;

		// Fill SS0 output stream select.
		stSS0Info.ui8StreamSel = SCALING_STREAM_0;
		// Fill SS0 output image information.
		stSS0Info.stOutImgInfo.ui16Width = (ISPOST_UINT16)pIspostUser->ui32SS0OutImgDstWidth;
		stSS0Info.stOutImgInfo.ui16Height = (ISPOST_UINT16)pIspostUser->ui32SS0OutImgDstHeight;
		stSS0Info.stOutImgInfo.ui16Stride = (ISPOST_UINT16)pIspostUser->ui32SS0OutImgBufStride;
		stSS0Info.stOutImgInfo.ui32YAddr = pIspostUser->ui32SS0OutImgBufYaddr;
		if (pIspostUser->ui32SS0OutImgBufUVaddr == 0)
			stSS0Info.stOutImgInfo.ui32UVAddr = stSS0Info.stOutImgInfo.ui32YAddr + (stSS0Info.stOutImgInfo.ui16Width * stSS0Info.stOutImgInfo.ui16Height);
		else
			stSS0Info.stOutImgInfo.ui32UVAddr = pIspostUser->ui32SS0OutImgBufUVaddr;
		stSS0Info.stHSF.ui16SF = IspostScalingFactotCal(pIspostUser->ui32LcDstWidth, stSS0Info.stOutImgInfo.ui16Width);
		stSS0Info.stHSF.ui8SM = (ISPOST_UINT8)pIspostUser->ui32SS0OutHorzScaling;
		stSS0Info.stVSF.ui16SF = IspostScalingFactotCal(pIspostUser->ui32LcDstHeight, stSS0Info.stOutImgInfo.ui16Height);
		stSS0Info.stVSF.ui8SM = (ISPOST_UINT8)pIspostUser->ui32SS0OutVertScaling;

		if (pIspostUser->ui32SS0OutImgDstWidth >= pIspostUser->ui32LcDstWidth)
			stSS0Info.stHSF.ui8SM = SCALING_MODE_NO_SCALING;
		if (pIspostUser->ui32SS0OutImgDstHeight >= pIspostUser->ui32LcDstHeight)
			stSS0Info.stVSF.ui8SM = SCALING_MODE_NO_SCALING;


		// Fill SS0 AXI SSWID.
		stSS0Info.ui8SSWID = 1;
		#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if (pIspostUser->ui32PrintFlag & ISPOST_PRN_SS0)
		{
			printk("@@@ stSS0Info.stOutImgInfo.ui32YAddr: 0x%X\n", stSS0Info.stOutImgInfo.ui32YAddr);
			printk("@@@ stSS0Info.stOutImgInfo.ui32UVAddr: 0x%X\n", stSS0Info.stOutImgInfo.ui32UVAddr);
			printk("@@@ stSS0Info.stOutImgInfo.ui16Stride: %d\n", stSS0Info.stOutImgInfo.ui16Stride);
			printk("@@@ stSS0Info.stOutImgInfo.ui16Width: %d\n", stSS0Info.stOutImgInfo.ui16Width);
			printk("@@@ stSS0Info.stOutImgInfo.ui16Height: %d\n", stSS0Info.stOutImgInfo.ui16Height);
			printk("@@@ stSS0Info.stHSF.ui16SF: %d\n", stSS0Info.stHSF.ui16SF);
			printk("@@@ stSS0Info.stHSF.ui8SM: %d\n", stSS0Info.stHSF.ui8SM);
			printk("@@@ stSS0Info.stVSF.ui16SF: %d\n", stSS0Info.stVSF.ui16SF);
			printk("@@@ stSS0Info.stVSF.ui8SM: %d\n", stSS0Info.stVSF.ui8SM);
			printk("@@@ stSS0Info.ui8SSWID: %d\n", stSS0Info.ui8SSWID);
		}
		#endif //INFOTM_ENABLE_ISPOST_DEBUG
		ui32Ret += IspostScaleStreamSet(stSS0Info);
	}

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_SS0;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserSetupSS1(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	SS_INFO stSS1Info;

	// ==================================================================
	// Setup scaling stream 1 information.
	// ==================================================================
	if (pIspostUser->stCtrl0.ui1EnSS1)
	{
		//
		// Setup Scaling stream 1 output image information..
		// ============================
		//
		if (pIspostUser->ui32SS1OutImgDstWidth >= pIspostUser->ui32SS1OutImgMaxWidth)
			pIspostUser->ui32SS1OutImgDstWidth = pIspostUser->ui32SS1OutImgMaxWidth;
		if (pIspostUser->ui32SS1OutImgDstHeight >= pIspostUser->ui32SS1OutImgMaxHeight)
			pIspostUser->ui32SS1OutImgDstHeight = pIspostUser->ui32SS1OutImgMaxHeight;

		// Fill SS1 output stream select.
		stSS1Info.ui8StreamSel = SCALING_STREAM_1;
		// Fill SS1 output image information.
		stSS1Info.stOutImgInfo.ui16Width = (ISPOST_UINT16)pIspostUser->ui32SS1OutImgDstWidth;
		stSS1Info.stOutImgInfo.ui16Height = (ISPOST_UINT16)pIspostUser->ui32SS1OutImgDstHeight;
		stSS1Info.stOutImgInfo.ui16Stride = (ISPOST_UINT16)pIspostUser->ui32SS1OutImgBufStride;
		stSS1Info.stOutImgInfo.ui32YAddr = pIspostUser->ui32SS1OutImgBufYaddr;
		if (pIspostUser->ui32SS1OutImgBufUVaddr == 0)
			stSS1Info.stOutImgInfo.ui32UVAddr = stSS1Info.stOutImgInfo.ui32YAddr + (stSS1Info.stOutImgInfo.ui16Width * stSS1Info.stOutImgInfo.ui16Height);
		else
			stSS1Info.stOutImgInfo.ui32UVAddr = pIspostUser->ui32SS1OutImgBufUVaddr;
		stSS1Info.stHSF.ui16SF = IspostScalingFactotCal(pIspostUser->ui32LcDstWidth, stSS1Info.stOutImgInfo.ui16Width);
		stSS1Info.stHSF.ui8SM = (ISPOST_UINT8)pIspostUser->ui32SS1OutHorzScaling;
		stSS1Info.stVSF.ui16SF = IspostScalingFactotCal(pIspostUser->ui32LcDstHeight, stSS1Info.stOutImgInfo.ui16Height);
		stSS1Info.stVSF.ui8SM = (ISPOST_UINT8)pIspostUser->ui32SS1OutVertScaling;

		if (pIspostUser->ui32SS1OutImgDstWidth >= pIspostUser->ui32LcDstWidth)
			stSS1Info.stHSF.ui8SM = SCALING_MODE_NO_SCALING;
		if (pIspostUser->ui32SS1OutImgDstHeight >= pIspostUser->ui32LcDstHeight)
			stSS1Info.stVSF.ui8SM = SCALING_MODE_NO_SCALING;

		// Fill SS1 AXI SSWID.
		stSS1Info.ui8SSWID = 2;
		#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if (pIspostUser->ui32PrintFlag & ISPOST_PRN_SS1)
		{
			printk("@@@ stSS1Info.stOutImgInfo.ui32YAddr: 0x%X\n", stSS1Info.stOutImgInfo.ui32YAddr);
			printk("@@@ stSS1Info.stOutImgInfo.ui32UVAddr: 0x%X\n", stSS1Info.stOutImgInfo.ui32UVAddr);
			printk("@@@ stSS1Info.stOutImgInfo.ui16Stride: %d\n", stSS1Info.stOutImgInfo.ui16Stride);
			printk("@@@ stSS1Info.stOutImgInfo.ui16Width: %d\n", stSS1Info.stOutImgInfo.ui16Width);
			printk("@@@ stSS1Info.stOutImgInfo.ui16Height: %d\n", stSS1Info.stOutImgInfo.ui16Height);
			printk("@@@ stSS1Info.stHSF.ui16SF: %d\n", stSS1Info.stHSF.ui16SF);
			printk("@@@ stSS1Info.stHSF.ui8SM: %d\n", stSS1Info.stHSF.ui8SM);
			printk("@@@ stSS1Info.stVSF.ui16SF: %d\n", stSS1Info.stVSF.ui16SF);
			printk("@@@ stSS1Info.stVSF.ui8SM: %d\n", stSS1Info.stVSF.ui8SM);
			printk("@@@ stSS1Info.ui8SSWID: %d\n", stSS1Info.ui8SSWID);
		}
		#endif //INFOTM_ENABLE_ISPOST_DEBUG
		ui32Ret += IspostScaleStreamSet(stSS1Info);
	}

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_SS1;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserTrigger(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 YAddr;
    ISPOST_UINT32 UVAddr;
    ISPOST_UINT16 Stride;

    if (pIspostUser->stCtrl0.ui1EnOV)
    {
        YAddr = pIspostUser->ui32OvImgBufYaddr;
        if (pIspostUser->ui32OvImgBufUVaddr == 0)
        	UVAddr = YAddr + (pIspostUser->ui32OvImgWidth * pIspostUser->ui32OvImgHeight);
        else
        	UVAddr = pIspostUser->ui32OvImgBufUVaddr;
    	ui32Ret += IspostOvImgAddrSet(YAddr, UVAddr);
    }

	// Setup LC SRC addr.
    YAddr = pIspostUser->ui32LcSrcBufYaddr;
    if (pIspostUser->ui32LcSrcBufUVaddr == 0)
    	UVAddr = YAddr + (pIspostUser->ui32LcSrcWidth * pIspostUser->ui32LcSrcHeight);
    else
    	UVAddr = pIspostUser->ui32LcSrcBufUVaddr;

    //printk("@@@ UDP Ispost SrcY=0x%X, SrcUV=0x%X\n", YAddr, UVAddr);
    if (pIspostUser->stCtrl0.ui1EnLC)
    	ui32Ret += IspostLcSrcImgAddrSet(YAddr, UVAddr);
    else
    	ui32Ret += IspostOvImgAddrSet(YAddr, UVAddr);

    // Setup DN output image addr.
    YAddr =  pIspostUser->ui32DnOutImgBufYaddr;
    if (pIspostUser->ui32DnOutImgBufUVaddr == 0)
    	UVAddr = YAddr + (pIspostUser->ui32LcDstWidth * pIspostUser->ui32LcDstHeight);
    else
    	UVAddr = pIspostUser->ui32DnOutImgBufUVaddr;
    if (pIspostUser->ui32DnOutImgBufStride == 0)
    	Stride = (ISPOST_UINT16)pIspostUser->ui32LcDstWidth;
    else
    	Stride = pIspostUser->ui32DnOutImgBufStride;
    //printk("@@@ UDP Ispost 3DDnY=0x%X, 3DDnUV=0x%X\n", YAddr, UVAddr);
	ui32Ret += IspostDnOutImgSet(YAddr, UVAddr, Stride);

    if (pIspostUser->stCtrl0.ui1EnDN && pIspostUser->ui32DnOutImgBufYaddr != 0xFFFFFFFF)
    {
		// Setup DNR reference input image addr.
		if (pIspostUser->ui32DnRefImgBufYaddr != 0xFFFFFFFF)
		{
			YAddr = pIspostUser->ui32DnRefImgBufYaddr;
			if (pIspostUser->ui32DnRefImgBufUVaddr == 0)
				UVAddr = YAddr + (pIspostUser->ui32LcDstWidth * pIspostUser->ui32LcDstHeight);
			else
				UVAddr = pIspostUser->ui32DnRefImgBufUVaddr;
		    if (pIspostUser->ui32DnRefImgBufStride == 0)
		    	Stride = (ISPOST_UINT16)pIspostUser->ui32LcDstWidth;
		    else
		    	Stride = pIspostUser->ui32DnRefImgBufStride;
			ui32Ret += IspostDnRefImgSet(YAddr, UVAddr, Stride);
		}
    }

    if (pIspostUser->stCtrl0.ui1EnSS0 && pIspostUser->ui32SS0OutImgBufYaddr != 0xFFFFFFFF)
    {
        YAddr = pIspostUser->ui32SS0OutImgBufYaddr;
        if (pIspostUser->ui32SS0OutImgBufUVaddr == 0)
        	UVAddr = YAddr + (pIspostUser->ui32SS0OutImgDstWidth * pIspostUser->ui32SS0OutImgDstHeight);
        else
        	UVAddr = pIspostUser->ui32SS0OutImgBufUVaddr;
    	Stride = pIspostUser->ui32SS0OutImgBufStride;
		ui32Ret += IspostSSOutImgUpdate(SCALING_STREAM_0, YAddr, UVAddr, Stride);
    }

    if (pIspostUser->stCtrl0.ui1EnSS1 && pIspostUser->ui32SS1OutImgBufYaddr != 0xFFFFFFFF)
    {
        YAddr = pIspostUser->ui32SS1OutImgBufYaddr;
        if (pIspostUser->ui32SS1OutImgBufUVaddr == 0)
        	UVAddr = YAddr + (pIspostUser->ui32SS1OutImgDstWidth * pIspostUser->ui32SS1OutImgDstHeight);
        else
        	UVAddr = pIspostUser->ui32SS1OutImgBufUVaddr;
    	Stride = pIspostUser->ui32SS1OutImgBufStride;
		ui32Ret += IspostSSOutImgUpdate(SCALING_STREAM_1, YAddr, UVAddr, Stride);
    }

	if (pIspostUser->ui32IspostCount)
		IspostResetSetCtrlAndTrigger(pIspostUser->stCtrl0);
	else
		IspostResetSetCtrlAndTrigger((ISPOST_CTRL0)(pIspostUser->stCtrl0.ui32Ctrl0 & 0xFFFFFBFF));

	return ISPOST_TRUE;
}


ISPOST_RESULT IspostUserUpdate(ISPOST_USER* pIspostUser)
{

	if (pIspostUser->ui32UpdateFlag & ISPOST_UDP_LC)
		IspostUserSetupLC(pIspostUser);

	if (pIspostUser->ui32UpdateFlag & ISPOST_UDP_OV)
		IspostUserSetupOV(pIspostUser);

	if (pIspostUser->ui32UpdateFlag & ISPOST_UDP_DN)
		IspostUserSetupDN(pIspostUser);

	if (pIspostUser->ui32UpdateFlag & ISPOST_UDP_SS0)
		IspostUserSetupSS0(pIspostUser);

	if (pIspostUser->ui32UpdateFlag & ISPOST_UDP_SS1)
		IspostUserSetupSS1(pIspostUser);

	pIspostUser->ui32UpdateFlag = 0;

    return ISPOST_TRUE;
}


ISPOST_RESULT IspostUserIsBusy(void)
{
	ISPOST_STAT0 ui32Stat0;

	ui32Stat0 = IspostStatusGet();
	if (ui32Stat0.ui32Stat0 != 0) //Ispost not processed finish
		return ISPOST_TRUE;
	else
		return ISPOST_FALSE;
}

ISPOST_UINT32 IspostRegRead(ISPOST_UINT32 RegAddr)
{
	return ISPOST_READL(RegAddr);
}

ISPOST_UINT32 IspostRegWrite(ISPOST_UINT32 RegAddr, ISPOST_UINT32 RegData)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_WRITEL(RegData ,RegAddr);
	CHECK_REG_WRITE_VALUE(RegAddr, RegData, ui32Ret);
	return ui32Ret;
}
