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
#include <mach/items.h>
#else
#include <common.h>
#include <asm/io.h>
#include <ispost/ispost.h>
#endif //IMG_KERNEL_MODULE


//#ifdef INFOTM_ISPOST


//#define ISPOST_REG_DEBUG
#if !defined(IMG_KERNEL_MODULE)
#define ENABLE_CHECK_REG_WRITE_VALUE
#endif //IMG_KERNEL_MODULE


#if 0
const char g_chLbError[4][41] = {
			"No Error",
			"Pixel Coordinate Out of Image",
			"Pixel Pixel Value Expired (Overwritten)",
			"Pixel Coordinate Out of Range of a Line"
			};
#else
const char g_chLbError[4][24] = {
			"No Error",
			"Out of Image",
			"Expired (Overwritten)",
			"Out of Range of a Line"
			};
#endif
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
#if !defined(ISPOST_REG_DEBUG)
	#define ISPOST_WRITEL(RegData, RegAddr)                                     \
		writel(RegData, g_ioIspostRegVirtBaseAddr + RegAddr)
#else
	#define ISPOST_WRITEL(RegData, RegAddr)                                     \
	{                                                                           \
		writel(RegData, g_ioIspostRegVirtBaseAddr + RegAddr);                   \
		printk("ISPOST:writel(0x%08X, 0x%08X);\n", (ISPOST_UINT32)RegData, (ISPOST_UINT32)(g_ioIspostRegVirtBaseAddr + RegAddr)); \
	}
#endif
#else
	#define ISPOST_READL(RegAddr)                                               \
		readl(RegAddr)
  #if !defined(ISPOST_REG_DEBUG)
	#define ISPOST_WRITEL(RegData, RegAddr)                                     \
		writel(RegData, RegAddr);
  #else
	#define ISPOST_WRITEL(RegData, RegAddr)                                     \
    {                                                                           \
		writel(RegData, RegAddr);                                               \
		ISPOST_LOG("ISPOST:writel(0x%08X, 0x%08X);\n", RegData, RegAddr);       \
	}
  #endif
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

#define CHECK_REG_WRITE_VALUE_WITH_MASK_WO_MSG(RegAddr, RegExpectedData, ErrCnt, Mask) \
    {                                                                       \
        if (RegExpectedData != ((g_ui32Temp = ISPOST_READL(RegAddr)) & Mask)) {  \
            if (0 == ErrCnt) {                                              \
                /* store first failure. */                                  \
                g_ui32RegAddr = RegAddr;                                    \
                g_ui32RegExpectedData = RegExpectedData;                    \
                g_ui32RegData = g_ui32Temp;                                 \
            }                                                               \
            /* increment error cnt */                                       \
            ErrCnt++;                                                       \
        }                                                                   \
    }


ISPOST_VOID
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
	ISPOST_VOID
	)
{
#if defined(IMG_KERNEL_MODULE)
	int ret = 0;
	struct clk * busclk;
	ISPOST_UINT32 ui32ClkRate;
	ISPOST_UINT32 test_y_value;

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

	if(item_exist("ispost.freq"))
		ui32ClkRate = item_integer("ispost.freq", 0) * 1000000;
	else
		ui32ClkRate = clk_get_rate(busclk);

	printk("@@@ ispost bus clock -> %d\n", ui32ClkRate);
	ret = clk_set_rate(busclk, ui32ClkRate);
	if (0 > ret) {
		ISPOST_LOG("failed to set imap ispost clock\n");
	} else {
		ret = clk_prepare_enable(busclk);
		if(ret){
			printk("prepare ispost clock fail!\n");
			clk_disable_unprepare(busclk);
			return -1;
		}
		g_busClk = busclk;
	}

	module_power_on(SYSMGR_ISPOST_BASE);
	module_reset(SYSMGR_ISPOST_BASE, 0);
	module_reset(SYSMGR_ISPOST_BASE, 1);
	g_ioIspostRegVirtBaseAddr = ioremap(IMAP_ISPOST_BASE, IMAP_ISPOST_SIZE);
	printk("@@@ IMAP_ISPOST_BASE: 0x%X, IMAP_ISPOST_SIZE: %d\n", IMAP_ISPOST_BASE, IMAP_ISPOST_SIZE);
	printk("@@@ Ispost base address: %p\n", g_ioIspostRegVirtBaseAddr);
	if (NULL == g_ioIspostRegVirtBaseAddr)
		goto ioremap_fail;

	test_y_value = ISPOST_READL(ISPOST_LCSRCAY);
	printk("@@@ bef test_y_value=0x%08x\n", test_y_value);

	ISPOST_WRITEL(0xABC8, ISPOST_LCSRCAY);

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
	ISPOST_LOG("ISPOST module enable\n");
	module_enable("ispost");				// where is this module_enable srource? does it need to be modified for ispost?
	//module_reset("ispost");					// where is this module_reset source? does it need to be modified for ispost?
	return 0;
#endif //IMG_KERNEL_MODULE
}

ISPOST_VOID
ispost_module_disable(
	ISPOST_VOID
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
IspostLcSrcImgYAddrSet(
	ISPOST_UINT32 ui32YAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup source image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_LCSRCAY);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCAY, ui32YAddr, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcSrcImgUVAddrSet(
	ISPOST_UINT32 ui32UVAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup source image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_LCSRCAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCAUV, ui32UVAddr, ui32Ret);

	return (ui32Ret);
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
IspostLcSrcImgStrideSet(
	ISPOST_UINT16 ui16Stride
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup source image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_LCSRCS);
	CHECK_REG_WRITE_VALUE(ISPOST_LCSRCS, (ISPOST_UINT32)ui16Stride, ui32Ret);

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
	ISPOST_UINT8 ui8LineBufEnable,
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
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCGBSZ, LBEN, (ISPOST_UINT32)ui8LineBufEnable);
	ISPOST_WRITEL(ui32Temp, ISPOST_LCGBSZ);
	CHECK_REG_WRITE_VALUE(ISPOST_LCGBSZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

LC_LB_ERR
IspostLcLbErrGet(
	ISPOST_VOID
	)
{
	ISPOST_UINT32 ui32Error;

	ui32Error = ISPOST_READL(ISPOST_LCLBERR);
	return ((LC_LB_ERR)ui32Error);
}

ISPOST_RESULT
IspostLcPxlCacheModeSet(
	LC_PXL_CACHE_MODE stPxlCacheMode
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup LC pixel cache mode.
	ISPOST_WRITEL(stPxlCacheMode.ui32PxlCacheMode, ISPOST_LCPCM);
	CHECK_REG_WRITE_VALUE(ISPOST_LCPCM, stPxlCacheMode.ui32PxlCacheMode, ui32Ret);

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
IspostLcOutImgAddrSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup output image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_LCOAY);
	CHECK_REG_WRITE_VALUE(ISPOST_LCOAY, ui32YAddr, ui32Ret);

	// Setup output image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_LCOAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_LCOAUV, ui32UVAddr, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcOutImgStrideSet(
	ISPOST_UINT16 ui16Stride
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup source image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_LCOS);
	CHECK_REG_WRITE_VALUE(ISPOST_LCOS, (ISPOST_UINT32)ui16Stride, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcOutImgAddrAndStrideSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup output image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_LCOAY);
	CHECK_REG_WRITE_VALUE(ISPOST_LCOAY, ui32YAddr, ui32Ret);

	// Setup output image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_LCOAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_LCOAUV, ui32UVAddr, ui32Ret);

	// Setup source image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_LCOS);
	CHECK_REG_WRITE_VALUE(ISPOST_LCOS, (ISPOST_UINT32)ui16Stride, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcOutImgSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride,
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup output image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_LCOAY);
	CHECK_REG_WRITE_VALUE(ISPOST_LCOAY, ui32YAddr, ui32Ret);

	// Setup output image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_LCOAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_LCOAUV, ui32UVAddr, ui32Ret);

	// Setup source image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_LCOS);
	CHECK_REG_WRITE_VALUE(ISPOST_LCOS, (ISPOST_UINT32)ui16Stride, ui32Ret);

	// Setup LC pixel coordinator destination size.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCPGSZ, W, (ISPOST_UINT32)ui16Width);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCPGSZ, H, (ISPOST_UINT32)ui16Height);
	ISPOST_WRITEL(ui32Temp, ISPOST_LCPGSZ);
	CHECK_REG_WRITE_VALUE(ISPOST_LCPGSZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcLbLineGeometryIdxSet(
	ISPOST_BOOL8 bl8AutoInc,
	ISPOST_UINT16 ui16Idx
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup LC line buffer Line Geometry index.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCLBGEOIDX, AINC, (ISPOST_UINT32)bl8AutoInc);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCLBGEOIDX, IDX, (ISPOST_UINT32)ui16Idx);
	ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEOIDX);
	//CHECK_REG_WRITE_VALUE(ISPOST_LCLBGEOIDX, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcLbLineGeometryWithFixDataSet(
	ISPOST_UINT16 ui16LBegin,
	ISPOST_UINT16 ui16LEnd,
	ISPOST_UINT16 ui16VerticalLines
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Inx;
	ISPOST_UINT32 ui32Temp;

	// Setup LC line buffer Line Geometry.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCLBGEO, LSTART, (ISPOST_UINT32)ui16LBegin);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCLBGEO, LEND, (ISPOST_UINT32)ui16LEnd);
	for (ui32Inx=0; ui32Inx<ui16VerticalLines; ui32Inx++) {
		ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEO);
		//CHECK_REG_WRITE_VALUE(ISPOST_LCLBGEO, ui32Temp, ui32Ret);
	}

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcLbLineGeometrySet(
	ISPOST_BOOL8 bEnableIspLink,
	ISPOST_PUINT16 pui16LinGeometry,
	ISPOST_UINT16 ui16VerticalLines
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Inx;
	ISPOST_UINT32 ui32Temp = 0;

	// Setup LC line buffer line Geometry.
	for (ui32Inx=0; ui32Inx<ui16VerticalLines; ui32Inx++) {
		ui32Temp = 0;
		//ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCLBGEO, LSTART, (ISPOST_UINT32)(*pui16LinGeometry++));
		//ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCLBGEO, LEND, (ISPOST_UINT32)(*pui16LinGeometry++));
		ui32Temp = (ISPOST_UINT32)(*pui16LinGeometry++);
		ui32Temp <<= 16;

		if (bEnableIspLink) {
			ui32Temp |= ((ISPOST_UINT32)(*pui16LinGeometry++) + 8);
		} else {
			ui32Temp |= (ISPOST_UINT32)(*pui16LinGeometry++);
		}
		ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEO);
		//CHECK_REG_WRITE_VALUE(ISPOST_LCLBGEO, ui32Temp, ui32Ret);
	}
	if (bEnableIspLink) {
		ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEO);
		ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEO);
		ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEO);
		ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEO);
	}

	return (ui32Ret);
}

ISPOST_RESULT
IspostLcLbLineGeometryFrom0Set(
	ISPOST_BOOL8 bEnableIspLink,
	ISPOST_PUINT16 pui16LinGeometry,
	ISPOST_UINT16 ui16VerticalLines
	)
{

	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Inx;
	ISPOST_UINT32 ui32Temp = 0;

	// Setup LC line buffer line Geometry.
	ui32Temp = ISPOST_LCLBGEOIDX_AINC_MASK;
	ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEOIDX);
	//CHECK_REG_WRITE_VALUE(ISPOST_LCLBGEOIDX, ui32Temp, ui32Ret);

	for (ui32Inx=0; ui32Inx<ui16VerticalLines; ui32Inx++) {
		ui32Temp = 0;
		//ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCLBGEO, LSTART, (ISPOST_UINT32)(*pui16LinGeometry++));
		//ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, LCLBGEO, LEND, (ISPOST_UINT32)(*pui16LinGeometry++));
		ui32Temp = (ISPOST_UINT32)(*pui16LinGeometry++);
		ui32Temp <<= 16;
		if (bEnableIspLink) {
			ui32Temp |= ((ISPOST_UINT32)(*pui16LinGeometry++) + 8);
		} else {
			ui32Temp |= (ISPOST_UINT32)(*pui16LinGeometry++);
		}
		ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEO);
		//CHECK_REG_WRITE_VALUE(ISPOST_LCLBGEO, ui32Temp, ui32Ret);
	}
	if (bEnableIspLink) {
		ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEO);
		ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEO);
		ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEO);
		ISPOST_WRITEL(ui32Temp, ISPOST_LCLBGEO);
	}

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
	// Setup LC back fill color Y, CB and CR value.
	// ============================
	//
	ui32Ret += IspostLcBackFillColorSet(
		stLcInfo.stBackFillColor.ui8Y,
		stLcInfo.stBackFillColor.ui8CB,
		stLcInfo.stBackFillColor.ui8CR
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
		stLcInfo.stPxlCacheMode
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
		stLcInfo.stOutImgInfo.ui16Width,
		stLcInfo.stOutImgInfo.ui16Height
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
IspostLenCorrectionSetWithLcOutput(
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
	// Setup LC back fill color Y, CB and CR value.
	// ============================
	//
	ui32Ret += IspostLcBackFillColorSet(
			stLcInfo.stBackFillColor.ui8Y,
			stLcInfo.stBackFillColor.ui8CB,
			stLcInfo.stBackFillColor.ui8CR
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
		stLcInfo.stPxlCacheMode
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
	// Setup LC output addr and stride.
	// ============================
	//
	ui32Ret += IspostLcOutImgAddrAndStrideSet(
		stLcInfo.stOutImgInfo.ui32YAddr,
		stLcInfo.stOutImgInfo.ui32UVAddr,
		stLcInfo.stOutImgInfo.ui16Stride
		);

	//
	// Setup LC pixel coordinator destination size.
	// ============================
	//
	ui32Ret += IspostLcPxlDstSizeSet(
		stLcInfo.stOutImgInfo.ui16Width,
		stLcInfo.stOutImgInfo.ui16Height
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
IspostVsCtrl0Set(
	ISPOST_UINT32 ui32Ctrl
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup Video Stabilization control 0 register.
	ISPOST_WRITEL(ui32Ctrl, ISPOST_VSCTRL0);
	CHECK_REG_WRITE_VALUE(ISPOST_VSCTRL0, ui32Ctrl, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVsWinCoorSet(
	VS_WIN_COOR stWinCoor[4]
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup Video Stabilization window coordinate.
	ISPOST_WRITEL(stWinCoor[0].ui32WinCoor, ISPOST_VSWC0);
	CHECK_REG_WRITE_VALUE(ISPOST_VSWC0, stWinCoor[0].ui32WinCoor, ui32Ret);
	ISPOST_WRITEL(stWinCoor[1].ui32WinCoor, ISPOST_VSWC1);
	CHECK_REG_WRITE_VALUE(ISPOST_VSWC1, stWinCoor[1].ui32WinCoor, ui32Ret);
	ISPOST_WRITEL(stWinCoor[2].ui32WinCoor, ISPOST_VSWC2);
	CHECK_REG_WRITE_VALUE(ISPOST_VSWC2, stWinCoor[2].ui32WinCoor, ui32Ret);
	ISPOST_WRITEL(stWinCoor[3].ui32WinCoor, ISPOST_VSWC3);
	CHECK_REG_WRITE_VALUE(ISPOST_VSWC3, stWinCoor[3].ui32WinCoor, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVsSrcImgAddrSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup Video Stabilization source image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_VSRCAY);
	CHECK_REG_WRITE_VALUE(ISPOST_VSRCAY, ui32YAddr, ui32Ret);

	// Setup Video Stabilization source image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_VSRCAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_VSRCAUV, ui32UVAddr, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVsSrcImgStrideSet(
	ISPOST_UINT16 ui16Stride
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup Video Stabilization source image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_VSRCS);
	CHECK_REG_WRITE_VALUE(ISPOST_VSRCS, (ISPOST_UINT32)ui16Stride, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVsSrcImgSizeSet(
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup Video Stabilization source image size.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, VSRSZ, W, (ISPOST_UINT32)ui16Width);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, VSRSZ, H, (ISPOST_UINT32)ui16Height);
	ISPOST_WRITEL(ui32Temp, ISPOST_VSRSZ);
	CHECK_REG_WRITE_VALUE(ISPOST_VSRSZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVsSrcImgSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride,
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	)
{

	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup Video Stabilization source image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_VSRCAY);
	CHECK_REG_WRITE_VALUE(ISPOST_VSRCAY, ui32YAddr, ui32Ret);

	// Setup Video Stabilization source image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_VSRCAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_VSRCAUV, ui32UVAddr, ui32Ret);

	// Setup Video Stabilization source image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_VSRCS);
	CHECK_REG_WRITE_VALUE(ISPOST_VSRCS, (ISPOST_UINT32)ui16Stride, ui32Ret);

	// Setup Video Stabilization source image size.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, VSRSZ, W, (ISPOST_UINT32)ui16Width);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, VSRSZ, H, (ISPOST_UINT32)ui16Height);
	ISPOST_WRITEL(ui32Temp, ISPOST_VSRSZ);
	CHECK_REG_WRITE_VALUE(ISPOST_VSRSZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVsRegRollAndHoriOffsetSet(
	ISPOST_UINT16 ui16Roll,
	ISPOST_UINT16 ui16HOff
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup Video Stabilization Registration Vertical Offset.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, VSROFF, ROLL, (ISPOST_UINT32)ui16Roll);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, VSROFF, HOFF, (ISPOST_UINT32)ui16HOff);
	ISPOST_WRITEL(ui32Temp, ISPOST_VSROFF);
	CHECK_REG_WRITE_VALUE(ISPOST_VSROFF, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVsRegVertOffsetSet(
	ISPOST_UINT8 ui8VOffset
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup Video Stabilization Registration Vertical Offset.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, VSRVOFF, VOFF, (ISPOST_UINT32)ui8VOffset);
	ISPOST_WRITEL(ui32Temp, ISPOST_VSRVOFF);
	CHECK_REG_WRITE_VALUE(ISPOST_VSRVOFF, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVsRegRollAndOffsetSet(
	ISPOST_UINT16 ui16Roll,
	ISPOST_UINT16 ui16HOff,
	ISPOST_UINT8 ui8VOffset
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup Video Stabilization Registration Vertical Offset.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, VSROFF, ROLL, (ISPOST_UINT32)ui16Roll);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, VSROFF, HOFF, (ISPOST_UINT32)ui16HOff);
	ISPOST_WRITEL(ui32Temp, ISPOST_VSROFF);
	CHECK_REG_WRITE_VALUE(ISPOST_VSROFF, ui32Temp, ui32Ret);

	// Setup Video Stabilization Registration Vertical Offset.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, VSRVOFF, VOFF, (ISPOST_UINT32)ui8VOffset);
	ISPOST_WRITEL(ui32Temp, ISPOST_VSRVOFF);
	CHECK_REG_WRITE_VALUE(ISPOST_VSRVOFF, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVideoStabilizationSetForDenoiseOnly(
	VS_INFO stVsInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// VSIR setup.
	// ============================
	// set base address for 3D denoise input READ  and stride
	// Fill Video Stabilization Registration source image information.
	ui32Ret += IspostVsSrcImgSet(
			stVsInfo.stSrcImgInfo.ui32YAddr,
			stVsInfo.stSrcImgInfo.ui32UVAddr,
			stVsInfo.stSrcImgInfo.ui16Stride,
			stVsInfo.stSrcImgInfo.ui16Width,
			stVsInfo.stSrcImgInfo.ui16Height
			);

	// Fill Video Stabilization Registration roll, H and V offset.
	ui32Ret += IspostVsRegRollAndOffsetSet(
			stVsInfo.stRegOffset.ui16Roll,
			stVsInfo.stRegOffset.ui16HOff,
			stVsInfo.stRegOffset.ui8VOff
			);

	// Fill Video Stabilization Registration AXI ID.
	ui32Ret += IspostVsAxiIdSet(stVsInfo.stAxiId.ui8FWID, stVsInfo.stAxiId.ui8RDID);

	// Fill Video Stabilization Registration flip/rot/rolling shutter mode.
	ui32Ret += IspostVsCtrl0Set(stVsInfo.stCtrl0.ui32Ctrl);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVsAxiIdSet(
	ISPOST_UINT8 ui8FWID,
	ISPOST_UINT8 ui8RDID
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup VS AXI FWID and RDID.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, VSAXI, FWID, (ISPOST_UINT32)ui8FWID);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, VSAXI, RDID, (ISPOST_UINT32)ui8RDID);
	ISPOST_WRITEL(ui32Temp, ISPOST_VSAXI);
	CHECK_REG_WRITE_VALUE(ISPOST_VSAXI, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOvIdxSet(
	ISPOST_UINT8 ui8OvIdx
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup overlay index.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVIDX, IDX, (ISPOST_UINT32)ui8OvIdx);
	ISPOST_WRITEL(ui32Temp, ISPOST_OVIDX);
	CHECK_REG_WRITE_VALUE(ISPOST_OVIDX, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOvModeSet(
	ISPOST_UINT32 ui32OverlayMode
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup overlay mode.
	ISPOST_WRITEL(ui32OverlayMode, ISPOST_OVM);
	CHECK_REG_WRITE_VALUE(ISPOST_OVM, ui32OverlayMode, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOvImgYAddrSet(
	ISPOST_UINT32 ui32YAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	if (0x00000000 != ui32YAddr) {
		// Setup overlay image Y plane start address.
		ISPOST_WRITEL(ui32YAddr, ISPOST_OVRIAY);
		CHECK_REG_WRITE_VALUE(ISPOST_OVRIAY, ui32YAddr, ui32Ret);

		return (ui32Ret);
	}

	return (ISPOST_EXIT_FAILURE);
}

ISPOST_RESULT
IspostOvImgAddrSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	if ((0x00000000 != ui32YAddr) && (0x00000000 != ui32UVAddr)) {
		// Setup overlay image Y plane start address.
		ISPOST_WRITEL(ui32YAddr, ISPOST_OVRIAY);
		CHECK_REG_WRITE_VALUE(ISPOST_OVRIAY, ui32YAddr, ui32Ret);

		// Setup overlay image UV plane start address.
		ISPOST_WRITEL(ui32UVAddr, ISPOST_OVRIAUV);
		CHECK_REG_WRITE_VALUE(ISPOST_OVRIAUV, ui32UVAddr, ui32Ret);

		return (ui32Ret);
	}

	return (ISPOST_EXIT_FAILURE);
}

ISPOST_RESULT
IspostOvImgStrideSet(
	ISPOST_UINT16 ui16Stride
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup overlay image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_OVRIS);
	CHECK_REG_WRITE_VALUE(ISPOST_OVRIS, (ISPOST_UINT32)ui16Stride, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOvImgSizeSet(
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup overlay image size.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVISZ, W, (ISPOST_UINT32)ui16Width);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVISZ, H, (ISPOST_UINT32)ui16Height);
	ISPOST_WRITEL(ui32Temp, ISPOST_OVISZ);
	CHECK_REG_WRITE_VALUE(ISPOST_OVISZ, ui32Temp, ui32Ret);

	return (ui32Ret);
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
	} else {
		return (ISPOST_EXIT_FAILURE);
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
IspostOv2BitImgSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT16 ui16Stride,
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	if (0x00000000 != ui32YAddr) {
		// Setup overlay image Y plane start address.
		ISPOST_WRITEL(ui32YAddr, ISPOST_OVRIAY);
		CHECK_REG_WRITE_VALUE(ISPOST_OVRIAY, ui32YAddr, ui32Ret);
	} else {
		return (ISPOST_EXIT_FAILURE);
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
#if 0
	ISPOST_UINT32 ui32Temp;
#endif

	// Setup overlay alpha value 0.
#if 0
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA0, ALPHA3, (ISPOST_UINT32)stValue0.ui8Alpha3);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA0, ALPHA2, (ISPOST_UINT32)stValue0.ui8Alpha2);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA0, ALPHA1, (ISPOST_UINT32)stValue0.ui8Alpha1);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA0, ALPHA0, (ISPOST_UINT32)stValue0.ui8Alpha0);
	ISPOST_WRITEL(ui32Temp, ISPOST_OVALPHA0);
	CHECK_REG_WRITE_VALUE(ISPOST_OVALPHA0, ui32Temp, ui32Ret);
#else
	ISPOST_WRITEL(stValue0.ui32Alpha, ISPOST_OVALPHA0);
	CHECK_REG_WRITE_VALUE(ISPOST_OVALPHA0, stValue0.ui32Alpha, ui32Ret);
#endif

	// Setup overlay alpha value 1.
#if 0
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA1, ALPHA7, (ISPOST_UINT32)stValue1.ui8Alpha3);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA1, ALPHA6, (ISPOST_UINT32)stValue1.ui8Alpha2);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA1, ALPHA5, (ISPOST_UINT32)stValue1.ui8Alpha1);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA1, ALPHA4, (ISPOST_UINT32)stValue1.ui8Alpha0);
	ISPOST_WRITEL(ui32Temp, ISPOST_OVALPHA1);
	CHECK_REG_WRITE_VALUE(ISPOST_OVALPHA1, ui32Temp, ui32Ret);
#else
	ISPOST_WRITEL(stValue1.ui32Alpha, ISPOST_OVALPHA1);
	CHECK_REG_WRITE_VALUE(ISPOST_OVALPHA1, stValue1.ui32Alpha, ui32Ret);
#endif

	// Setup overlay alpha value 2.
#if 0
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA2, ALPHA11, (ISPOST_UINT32)stValue2.ui8Alpha3);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA2, ALPHA10, (ISPOST_UINT32)stValue2.ui8Alpha2);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA2, ALPHA9, (ISPOST_UINT32)stValue2.ui8Alpha1);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA2, ALPHA8, (ISPOST_UINT32)stValue2.ui8Alpha0);
	ISPOST_WRITEL(ui32Temp, ISPOST_OVALPHA2);
	CHECK_REG_WRITE_VALUE(ISPOST_OVALPHA2, ui32Temp, ui32Ret);
#else
	ISPOST_WRITEL(stValue2.ui32Alpha, ISPOST_OVALPHA2);
	CHECK_REG_WRITE_VALUE(ISPOST_OVALPHA2, stValue2.ui32Alpha, ui32Ret);
#endif

	// Setup overlay alpha value 3.
#if 0
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA3, ALPHA15, (ISPOST_UINT32)stValue3.ui8Alpha3);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA3, ALPHA14, (ISPOST_UINT32)stValue3.ui8Alpha2);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA3, ALPHA13, (ISPOST_UINT32)stValue3.ui8Alpha1);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, OVALPHA3, ALPHA12, (ISPOST_UINT32)stValue3.ui8Alpha0);
	ISPOST_WRITEL(ui32Temp, ISPOST_OVALPHA3);
	CHECK_REG_WRITE_VALUE(ISPOST_OVALPHA3, ui32Temp, ui32Ret);
#else
	ISPOST_WRITEL(stValue3.ui32Alpha, ISPOST_OVALPHA3);
	CHECK_REG_WRITE_VALUE(ISPOST_OVALPHA3, stValue3.ui32Alpha, ui32Ret);
#endif

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

	// Setup overlay index.
	ui32Ret += IspostOvIdxSet(0);

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
		stOvInfo.stOverlayMode.ui32OvMode
		);

	// Setup OV overlay offset.
	ui32Ret += IspostOvOffsetSet(
		stOvInfo.stOvOffset.ui16X,
		stOvInfo.stOvOffset.ui16Y
		);

	// Setup OV overlay alpha value.
	ui32Ret += IspostOvAlphaValueSet(
		stOvInfo.stOvAlpha.stValue0,
		stOvInfo.stOvAlpha.stValue1,
		stOvInfo.stOvAlpha.stValue2,
		stOvInfo.stOvAlpha.stValue3
		);

	// Setup OV AXI read ID.
	ui32Ret += IspostOvAxiIdSet(
		stOvInfo.stAxiId.ui8OVID
		);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOverlaySetByChannel(
	OV_INFO stOvInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup overlay index.
	ui32Ret += IspostOvIdxSet(stOvInfo.ui8OvIdx);

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
		stOvInfo.stOverlayMode.ui32OvMode
		);

	// Setup OV overlay offset.
	ui32Ret += IspostOvOffsetSet(
		stOvInfo.stOvOffset.ui16X,
		stOvInfo.stOvOffset.ui16Y
		);

	// Setup OV overlay alpha value.
	ui32Ret += IspostOvAlphaValueSet(
		stOvInfo.stOvAlpha.stValue0,
		stOvInfo.stOvAlpha.stValue1,
		stOvInfo.stOvAlpha.stValue2,
		stOvInfo.stOvAlpha.stValue3
		);

	// Setup OV AXI read ID.
	ui32Ret += IspostOvAxiIdSet(
		stOvInfo.stAxiId.ui8OVID
		);

	return (ui32Ret);
}

ISPOST_RESULT
IspostOverlaySetAll(
	OV_INFO_APOLLO3 stOvInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT8 ui8Idx;
	ISPOST_UINT8 uiChannel;

	// Setup OV Set overlay mode.
	ui32Ret += IspostOvModeSet(
		stOvInfo.stOverlayMode.ui32OvMode
		);

	uiChannel = (ISPOST_UINT8)((stOvInfo.stOverlayMode.ui32OvMode >> 8) & 0xff);
	for (ui8Idx=0; ui8Idx<OVERLAY_CHNL_MAX; ui8Idx++) {
		if (uiChannel & 0x01) {
			// Setup overlay index.
			//stOvInfo.ui8OvIdx[0] = ui8Idx;
			//ui32Ret += IspostOvIdxSet(stOvInfo.ui8OvIdx[0]);
			ui32Ret += IspostOvIdxSet(ui8Idx);

			// Setup OV overlay image information.
			if (0 == ui8Idx) {
				ui32Ret += IspostOvImgSet(
					stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr,
					stOvInfo.stOvImgInfo[ui8Idx].ui32UVAddr,
					stOvInfo.stOvImgInfo[ui8Idx].ui16Stride,
					stOvInfo.stOvImgInfo[ui8Idx].ui16Width,
					stOvInfo.stOvImgInfo[ui8Idx].ui16Height
					);
			} else {
				ui32Ret += IspostOv2BitImgSet(
					stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr,
					stOvInfo.stOvImgInfo[ui8Idx].ui16Stride,
					stOvInfo.stOvImgInfo[ui8Idx].ui16Width,
					stOvInfo.stOvImgInfo[ui8Idx].ui16Height
					);
			}

			// Setup OV overlay offset.
			ui32Ret += IspostOvOffsetSet(
				stOvInfo.stOvOffset[ui8Idx].ui16X,
				stOvInfo.stOvOffset[ui8Idx].ui16Y
				);

			// Setup OV overlay alpha value.
			ui32Ret += IspostOvAlphaValueSet(
				stOvInfo.stOvAlpha[ui8Idx].stValue0,
				stOvInfo.stOvAlpha[ui8Idx].stValue1,
				stOvInfo.stOvAlpha[ui8Idx].stValue2,
				stOvInfo.stOvAlpha[ui8Idx].stValue3
				);

			// Setup OV AXI read ID.
			ui32Ret += IspostOvAxiIdSet(
				stOvInfo.stAxiId[ui8Idx].ui8OVID
				);
		}
		uiChannel >>= 1;
	}

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
	} else {
		return (ISPOST_EXIT_FAILURE);
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

	// Setup output image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_DNROAY);
	CHECK_REG_WRITE_VALUE(ISPOST_DNROAY, ui32YAddr, ui32Ret);

	// Setup output image UV plane start address.
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

	// Setup output image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_DNROAY);
	CHECK_REG_WRITE_VALUE(ISPOST_DNROAY, ui32YAddr, ui32Ret);

	// Setup output image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_DNROAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_DNROAUV, ui32UVAddr, ui32Ret);

	// Setup output image stride.
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
	ISPOST_UINT32 ui32RegAddr;
	ISPOST_PUINT32 pMskCrvVal;

	// Setup DN mask curve.
	// Setup DN mask curve index.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, DNCTRL0, IDX, (ISPOST_UINT32)stDnMskCrvInfo.ui8MskCrvIdx);
	ISPOST_WRITEL(ui32Temp, ISPOST_DNCTRL0);
	CHECK_REG_WRITE_VALUE(ISPOST_DNCTRL0, ui32Temp, ui32Ret);
	// Setup DN mask curve 00-14 information.
#if 0
	pMskCrvVal = (ISPOST_PUINT32)&stDnMskCrvInfo.stMskCrv00;
#else
	pMskCrvVal = (ISPOST_PUINT32)&stDnMskCrvInfo.stMskCrv[0];
#endif
	ui32RegAddr = ISPOST_DNMC0;
	for (ui32Idx=0; ui32Idx<DN_MSK_CRV_DNMC_MAX; ui32Idx++,pMskCrvVal++,ui32RegAddr+=4) {
		//ISPOST_LOG("%02d - Addr = 0x%08X, Data = 0x%08X\n", ui32Idx, (ISPOST_UINT32)pMskCrvVal, (ISPOST_UINT32)*pMskCrvVal);
		ISPOST_WRITEL(*pMskCrvVal, ui32RegAddr);
		CHECK_REG_WRITE_VALUE(ui32RegAddr, *pMskCrvVal, ui32Ret);
	}
	//ISPOST_LOG("=========================================\n");

	return (ui32Ret);
}

ISPOST_RESULT
Ispost3DDenoiseMskCrvSetAll(
	DN_MSK_CRV_INFO stDnMskCrvInfo[DN_MSK_CRV_IDX_MAX]
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;
	ISPOST_UINT32 ui32DnMskCrvIdx;
	ISPOST_UINT32 ui32Idx;
	ISPOST_UINT32 ui32RegAddr;
	ISPOST_PUINT32 pMskCrvVal;

	// Setup DN mask curve.
	// Setup DN mask curve index.
#if 0
	ISPOST_LOG("=========================================\n");
#endif
	for (ui32DnMskCrvIdx=0; ui32DnMskCrvIdx<DN_MSK_CRV_IDX_MAX; ui32DnMskCrvIdx++) {
		ui32Temp = 0;
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, DNCTRL0, IDX, (ISPOST_UINT32)stDnMskCrvInfo[ui32DnMskCrvIdx].ui8MskCrvIdx);
		ISPOST_WRITEL(ui32Temp, ISPOST_DNCTRL0);
		CHECK_REG_WRITE_VALUE(ISPOST_DNCTRL0, ui32Temp, ui32Ret);
		// Setup DN mask curve 00-14 information.
#if 0
		pMskCrvVal = (ISPOST_PUINT32)&stDnMskCrvInfo[ui32DnMskCrvIdx].stMskCrv00;
#else
		pMskCrvVal = (ISPOST_PUINT32)&stDnMskCrvInfo[ui32DnMskCrvIdx].stMskCrv[0];
#endif
		ui32RegAddr = ISPOST_DNMC0;
		for (ui32Idx=0; ui32Idx<DN_MSK_CRV_DNMC_MAX; ui32Idx++,pMskCrvVal++,ui32RegAddr+=4) {
#if 0
			ISPOST_LOG("%02d - Addr = 0x%08X, Data = 0x%08X\n", ui32Idx, (ISPOST_UINT32)pMskCrvVal, (ISPOST_UINT32)*pMskCrvVal);
#endif
			ISPOST_WRITEL(*pMskCrvVal, ui32RegAddr);
			CHECK_REG_WRITE_VALUE(ui32RegAddr, *pMskCrvVal, ui32Ret);
		}
#if 0
		ISPOST_LOG("=========================================\n");
#endif
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

	//
	// Setup DN mask curve.
	// ============================
	//
	ui32Ret += Ispost3DDenoiseMskCrvSetAll(
		stDnInfo.stDnMskCrvInfo
		);

	//
	// Setup DN AXI reference write and read ID.
	// ============================
	//
	ui32Ret += IspostDnAxiIdSet(
		stDnInfo.stAxiId.ui8RefWrID,
		stDnInfo.stAxiId.ui8RefRdID
		);

	return (ui32Ret);
}

ISPOST_RESULT
IspostUnSOutImgAddrSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup Un-Scaled output image Y plane start address.
	ISPOST_WRITEL(ui32YAddr, ISPOST_UOAY);
	CHECK_REG_WRITE_VALUE(ISPOST_UOAY, ui32YAddr, ui32Ret);

	// Setup Un-Scaled output image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, ISPOST_UOAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_UOAUV, ui32UVAddr, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostUnSOutImgAddrSetWithStrc(
	UO_INFO stUoInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup Un-Scaled output image Y plane start address.
	ISPOST_WRITEL(stUoInfo.stOutImgInfo.ui32YAddr, ISPOST_UOAY);
	CHECK_REG_WRITE_VALUE(ISPOST_UOAY, stUoInfo.stOutImgInfo.ui32YAddr, ui32Ret);

	// Setup Un-Scaled output image UV plane start address.
	ISPOST_WRITEL(stUoInfo.stOutImgInfo.ui32UVAddr, ISPOST_UOAUV);
	CHECK_REG_WRITE_VALUE(ISPOST_UOAUV, stUoInfo.stOutImgInfo.ui32UVAddr, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostUnSOutImgStrideSet(
	ISPOST_UINT16 ui16Stride
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup Un-Scaled output image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_UOS);
	CHECK_REG_WRITE_VALUE(ISPOST_UOS, (ISPOST_UINT32)ui16Stride, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostUnsPxlScanModeSet(
	ISPOST_UINT8 ui8ScanH
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup Un-Scaled output pixel coordinator generator mode.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, UOPGM, SCANH, (ISPOST_UINT32)ui8ScanH);
	ISPOST_WRITEL(ui32Temp, ISPOST_UOPGM);
	CHECK_REG_WRITE_VALUE(ISPOST_UOPGM, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostUnSOutImgSet(
	ISPOST_UINT32 ui32YAddr,
	ISPOST_UINT32 ui32UVAddr,
	ISPOST_UINT16 ui16Stride,
	ISPOST_UINT8 ui8ScanH
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	if ((0x00000000 != ui32YAddr) && (0x00000000 != ui32UVAddr)) {
		// Setup Un-Scaled output image Y plane start address.
		ISPOST_WRITEL(ui32YAddr, ISPOST_UOAY);
		CHECK_REG_WRITE_VALUE(ISPOST_UOAY, ui32YAddr, ui32Ret);

		// Setup Un-Scaled output image UV plane start address.
		ISPOST_WRITEL(ui32UVAddr, ISPOST_UOAUV);
		CHECK_REG_WRITE_VALUE(ISPOST_UOAUV, ui32UVAddr, ui32Ret);
	} else {
		return (ISPOST_EXIT_FAILURE);
	}

	// Setup Un-Scaled output image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, ISPOST_UOS);
	CHECK_REG_WRITE_VALUE(ISPOST_UOS, (ISPOST_UINT32)ui16Stride, ui32Ret);

	// Setup Un-Scaled output pixel coordinator generator mode.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, UOPGM, SCANH, (ISPOST_UINT32)ui8ScanH);
	ISPOST_WRITEL(ui32Temp, ISPOST_UOPGM);
	CHECK_REG_WRITE_VALUE(ISPOST_UOPGM, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostUnSAxiIdSet(
	ISPOST_UINT8 ui8RefWrID
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup Un-Scaled AXI reference write and read ID.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, UOAXI, REFWID, (ISPOST_UINT32)ui8RefWrID);
	ISPOST_WRITEL(ui32Temp, ISPOST_UOAXI);
	CHECK_REG_WRITE_VALUE(ISPOST_UOAXI, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostUnScaleOutputSet(
	UO_INFO stUoInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	//
	// Setup Un-Scaled output image addr, stride and scan height.
	// ============================
	//
	ui32Ret += IspostUnSOutImgSet(
		stUoInfo.stOutImgInfo.ui32YAddr,
		stUoInfo.stOutImgInfo.ui32UVAddr,
		stUoInfo.stOutImgInfo.ui16Stride,
		stUoInfo.ui8ScanH
		);

	// Setup Un-Scaled AXI reference write ID.
	ui32Ret += IspostUnSAxiIdSet(
		stUoInfo.stAxiId.ui8RefWrID
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
	ISPOST_WRITEL(ui32YAddr, (ISPOST_SS0AY+ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0AY + ui32StreamOffset), ui32YAddr, ui32Ret);

	// Setup output image UV plane start address.
	ISPOST_WRITEL(ui32UVAddr, (ISPOST_SS0AUV+ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0AUV+ui32StreamOffset), ui32UVAddr, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostSSStrideSet(
	ISPOST_UINT8 ui8StreamSel,
	ISPOST_UINT16 ui16Stride
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32StreamOffset;

	ui32StreamOffset = 0x00000018 * ui8StreamSel;

	// Setup output image stride.
	ISPOST_WRITEL((ISPOST_UINT32)ui16Stride, (ISPOST_SS0S+ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0S+ui32StreamOffset), (ISPOST_UINT32)ui16Stride, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostSSSizeSet(
	ISPOST_UINT8 ui8StreamSel,
	ISPOST_UINT16 ui16Width,
	ISPOST_UINT16 ui16Height
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32StreamOffset;
	ISPOST_UINT32 ui32Temp;

	ui32StreamOffset = 0x00000018 * ui8StreamSel;

	// Setup output image size.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS0IW, W, (ISPOST_UINT32)ui16Width);
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS0IW, H, (ISPOST_UINT32)ui16Height);
	ISPOST_WRITEL(ui32Temp, (ISPOST_SS0IW+ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0IW+ui32StreamOffset), ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostSSHScalingFactorSet(
	ISPOST_UINT8 ui8StreamSel,
	ISPOST_UINT16 ui16Factor,
	ISPOST_UINT8 ui8Mode
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32StreamOffset;
	ISPOST_UINT32 ui32Temp;

	ui32StreamOffset = 0x00000018 * ui8StreamSel;

	// Setup output image H scaling factor.
	ui32Temp = 0;
	if (SCALING_STREAM_0 == ui8StreamSel) {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS0HF, SF, (ISPOST_UINT32)ui16Factor);
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS0HF, SM, (ISPOST_UINT32)ui8Mode);
	} else {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS1HF, SF, (ISPOST_UINT32)ui16Factor);
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS1HF, SM, (ISPOST_UINT32)ui8Mode);
	}
	ISPOST_WRITEL(ui32Temp, (ISPOST_SS0HF+ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0HF+ui32StreamOffset), ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostSSVScalingFactorSet(
	ISPOST_UINT8 ui8StreamSel,
	ISPOST_UINT16 ui16Factor,
	ISPOST_UINT8 ui8Mode
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32StreamOffset;
	ISPOST_UINT32 ui32Temp;

	ui32StreamOffset = 0x00000018 * ui8StreamSel;

	// Setup output image V scaling factor.
	ui32Temp = 0;
	if (SCALING_STREAM_0 == ui8StreamSel) {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS0VF, SF, (ISPOST_UINT32)ui16Factor);
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS0VF, SM, (ISPOST_UINT32)ui8Mode);
	} else {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS1VF, SF, (ISPOST_UINT32)ui16Factor);
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SS1VF, SM, (ISPOST_UINT32)ui8Mode);
	}
	ISPOST_WRITEL(ui32Temp, (ISPOST_SS0VF+ui32StreamOffset));
	CHECK_REG_WRITE_VALUE((ISPOST_SS0VF+ui32StreamOffset), ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostSSAxiIdSet(
	ISPOST_UINT8 ui8StreamSel,
	ISPOST_UINT8 ui8SSWID
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup AXI SSxWID.
	ui32Temp = 0;
	ui32Temp = (ISPOST_READL(ISPOST_SSAXI) & 0x0000FFFF);
	if (SCALING_STREAM_0 == ui8StreamSel) {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SSAXI, SS0WID, (ISPOST_UINT32)ui8SSWID);
	} else {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SSAXI, SS1WID, (ISPOST_UINT32)ui8SSWID);
	}
	ISPOST_WRITEL(ui32Temp, ISPOST_SSAXI);
	CHECK_REG_WRITE_VALUE(ISPOST_SSAXI, ui32Temp, ui32Ret);

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
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SSAXI, SS0WID, (ISPOST_UINT32)stSsInfo.stAxiId.ui8SSWID);
	} else {
		ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, SSAXI, SS1WID, (ISPOST_UINT32)stSsInfo.stAxiId.ui8SSWID);
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
	// Setup SS output image information.
	// ============================
	//
	ui32Ret += IspostSSOutImgSet(
		stSsInfo
		);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVLinkCtrlSet(
	ISPOST_UINT32 ui32Ctrl
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup Video link control register.
	ISPOST_WRITEL(ui32Ctrl, ISPOST_VLINK);
	CHECK_REG_WRITE_VALUE(ISPOST_VLINK, ui32Ctrl, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVLinkStartAddrSet(
	ISPOST_UINT32 ui32StartAddr
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup Video link start frame address.
	ISPOST_WRITEL(ui32StartAddr, ISPOST_VLINKSA);
	CHECK_REG_WRITE_VALUE(ISPOST_VLINKSA, ui32StartAddr, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostVLinkSet(
	V_LINK_INFO stVLinkInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup Video link control register.
	ISPOST_WRITEL(stVLinkInfo.stVLinkCtrl.ui32Ctrl, ISPOST_VLINK);
	CHECK_REG_WRITE_VALUE(ISPOST_VLINK, stVLinkInfo.stVLinkCtrl.ui32Ctrl, ui32Ret);

	// Setup Video link start frame address.
	ISPOST_WRITEL(stVLinkInfo.ui32StartAddr, ISPOST_VLINKSA);
	CHECK_REG_WRITE_VALUE(ISPOST_VLINKSA, stVLinkInfo.ui32StartAddr, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostIspLinkStartAddrMSWSet(
	ISPOST_UINT8 ui8StartAddrMSW
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup ISP link start frame address MSW.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, ILINKSAM, SADM, (ISPOST_UINT32)ui8StartAddrMSW);
	ISPOST_WRITEL(ui32Temp, ISPOST_ILINKSAM);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKSAM, ui32Temp, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostIspLinkStartAddrLSWSet(
	ISPOST_UINT32 ui32StartAddrLSW
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup ISP link start frame address LSW.
	ISPOST_WRITEL(ui32StartAddrLSW, ISPOST_ILINKSAL);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKSAL, ui32StartAddrLSW, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostIspLinkCtrl0Set(
	ISPOST_UINT32 ui32Ctrl
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup ISP link control 0 register.
	ISPOST_WRITEL(ui32Ctrl, ISPOST_ILINKCTRL0);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKCTRL0, ui32Ctrl, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostIspLinkCtrl1Set(
	ISPOST_UINT32 ui32Ctrl
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup ISP link control 1 register.
	ISPOST_WRITEL(ui32Ctrl, ISPOST_ILINKCTRL1);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKCTRL1, ui32Ctrl, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostIspLinkCtrl2Set(
	ISPOST_UINT32 ui32Ctrl
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup ISP link control 2 register.
	ISPOST_WRITEL(ui32Ctrl, ISPOST_ILINKCTRL2);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKCTRL2, ui32Ctrl, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostIspLinkCtrl3Set(
	ISPOST_UINT32 ui32Ctrl
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup ISP link control 1 register.
	ISPOST_WRITEL(ui32Ctrl, ISPOST_ILINKCTRL3);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKCTRL3, ui32Ctrl, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostIspLinkCtrlSet(
	ISP_LINK_CTRL stIspLinkCtrl
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup ISP link control register.
	if (REODER_MODE_BURST == stIspLinkCtrl.stCtrl1.ui1ReOrdMode) {
		ui32Ret += IspostIspLinkCtrl0Set(stIspLinkCtrl.stCtrl0Link2Method.ui32Ctrl);
	} else {
		ui32Ret += IspostIspLinkCtrl0Set(stIspLinkCtrl.stCtrl0Link3Method.ui32Ctrl);
	}
	ui32Ret += IspostIspLinkCtrl1Set(stIspLinkCtrl.stCtrl1.ui32Ctrl);
	ui32Ret += IspostIspLinkCtrl2Set(stIspLinkCtrl.stCtrl2.ui32Ctrl);
	//ui32Ret += IspostIspLinkCtrl3Set(stIspLinkCtrl.ui32Ctrl3);

	return (ui32Ret);
}

ISPOST_RESULT
IspostIspLinkFirstStartAddrSet(
	ISP_LINK_INFO stIspLinkInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup ISP link start frame address MSW.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, ILINKSAM, SADM, stIspLinkInfo.ui32FirstUVAddrMSW);
	ISPOST_WRITEL(ui32Temp, ISPOST_ILINKSAM);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKSAM, ui32Temp, ui32Ret);

	// Setup ISP link start frame address LSW.
	ISPOST_WRITEL(stIspLinkInfo.ui32FirstUVAddrLSW, ISPOST_ILINKSAL);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKSAL, stIspLinkInfo.ui32FirstUVAddrLSW, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostIspLinkNextStartAddrSet(
	ISP_LINK_INFO stIspLinkInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup ISP link start frame address MSW.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, ILINKSAM, SADM, stIspLinkInfo.ui32NextUVAddrMSW);
	ISPOST_WRITEL(ui32Temp, ISPOST_ILINKSAM);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKSAM, ui32Temp, ui32Ret);

	// Setup ISP link start frame address LSW.
	ISPOST_WRITEL(stIspLinkInfo.ui32NextUVAddrLSW, ISPOST_ILINKSAL);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKSAL, stIspLinkInfo.ui32NextUVAddrLSW, ui32Ret);

	return (ui32Ret);
}

ISPOST_RESULT
IspostIspLinkSet(
	ISP_LINK_INFO stIspLinkInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Temp;

	// Setup ISP link start frame address MSW.
	ui32Temp = 0;
	ISPOST_REGIO_WRITE_FIELD(ui32Temp, ISPOST, ILINKSAM, SADM, stIspLinkInfo.ui32FirstUVAddrMSW);
	ISPOST_WRITEL(ui32Temp, ISPOST_ILINKSAM);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKSAM, ui32Temp, ui32Ret);

	// Setup ISP link start frame address LSW.
	ISPOST_WRITEL(stIspLinkInfo.ui32FirstUVAddrLSW, ISPOST_ILINKSAL);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKSAL, stIspLinkInfo.ui32FirstUVAddrLSW, ui32Ret);

	// Setup ISP link control register.
	if (REODER_MODE_BURST == stIspLinkInfo.stCtrl1.ui1ReOrdMode) {
		ui32Ret += IspostIspLinkCtrl0Set(stIspLinkInfo.stCtrl0Link2Method.ui32Ctrl);
	} else {
		ui32Ret += IspostIspLinkCtrl0Set(stIspLinkInfo.stCtrl0Link3Method.ui32Ctrl);
	}
	ui32Ret += IspostIspLinkCtrl1Set(stIspLinkInfo.stCtrl1.ui32Ctrl);
	ui32Ret += IspostIspLinkCtrl2Set(stIspLinkInfo.stCtrl2.ui32Ctrl);
	//ui32Ret += IspostIspLinkCtrl3Set(stIspLinkInfo.ui32Ctrl3);

	return (ui32Ret);
}

ISPOST_UINT32
IspostIspLinkStat0Get(
	ISPOST_VOID
	)
{
	ISPOST_UINT32 ui32Status;

	// Get ISP link status 0 register.
	ui32Status = ISPOST_READL(ISPOST_ILINKSTAT0);
	return (ui32Status);
}

ISPOST_RESULT
IspostIspLinkStat0Set(
	ISPOST_UINT32 ui32State
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	// Setup ISP link status 0 register.
	ISPOST_WRITEL(ui32State, ISPOST_ILINKSTAT0);
	CHECK_REG_WRITE_VALUE(ISPOST_ILINKSTAT0, ui32State, ui32Ret);

	return (ui32Ret);
}

ISPOST_VOID
IspostActivateReset(
	ISPOST_VOID
	)
{
	ISPOST_CTRL0 stCtrl0;

	//stCtrl0 = (ISPOST_CTRL0)ISPOST_READL(ISPOST_ISPCTRL0);
	// Activate reset ispost module.
	stCtrl0.ui32Ctrl0 = 0;
	stCtrl0.ui1Rst = 1;
	ISPOST_WRITEL(stCtrl0.ui32Ctrl0, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostDeActivateReset(
	ISPOST_VOID
	)
{
	ISPOST_CTRL0 stCtrl0;

	//stCtrl0 = (ISPOST_CTRL0)ISPOST_READL(ISPOST_ISPCTRL0);
	// De-activate ispost module.
	//stCtrl0.ui1Rst = 0;
	stCtrl0.ui32Ctrl0 = 0;
	ISPOST_WRITEL(stCtrl0.ui32Ctrl0, ISPOST_ISPCTRL0);
}

ISPOST_VOID
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

ISPOST_VOID
IspostProcessReset(
	ISPOST_VOID
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

ISPOST_VOID
IspostProcessDisable(
	ISPOST_VOID
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
IspostLbErrIntChk(
	ISPOST_UINT8 ui8IntOffOn
	)
{
	ISPOST_UINT32 ui32Ret = 0;

	if (ISPOST_OFF == ui8IntOffOn) {
		CHECK_REG_WRITE_VALUE_WITH_MASK_WO_MSG(ISPOST_ISPCTRL0, 0x00000000, ui32Ret, ISPOST_ISPCTRL0_LBERRINT_MASK);
	} else {
		CHECK_REG_WRITE_VALUE_WITH_MASK_WO_MSG(ISPOST_ISPCTRL0, ISPOST_ISPCTRL0_LBERRINT_MASK, ui32Ret, ISPOST_ISPCTRL0_LBERRINT_MASK);
	}

	return (ui32Ret);
}

ISPOST_RESULT
IspostIntChk(
	ISPOST_UINT8 ui8IntOffOn
	)
{
	ISPOST_UINT32 ui32Ret = 0;

#if defined(ISPOST_REG_DEBUG)
	printf("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
	printf("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
	if (ISPOST_OFF == ui8IntOffOn) {
		CHECK_REG_WRITE_VALUE_WITH_MASK_WO_MSG(ISPOST_ISPCTRL0, 0x00000000, ui32Ret, ISPOST_ISPCTRL0_ALL_INT_MASK);
	} else {
		CHECK_REG_WRITE_VALUE_WITH_MASK_WO_MSG(ISPOST_ISPCTRL0, ISPOST_ISPCTRL0_INT_MASK, ui32Ret, ISPOST_ISPCTRL0_ALL_INT_MASK);
	}

	return (ui32Ret);
}

ISPOST_RESULT
IspostVsIntChk(
	ISPOST_UINT8 ui8IntOffOn
	)
{
	ISPOST_UINT32 ui32Ret = 0;

#if defined(ISPOST_REG_DEBUG)
	printf("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
	printf("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
	if (ISPOST_OFF == ui8IntOffOn) {
		CHECK_REG_WRITE_VALUE_WITH_MASK_WO_MSG(ISPOST_ISPCTRL0, 0x00000000, ui32Ret, ISPOST_ISPCTRL0_ALL_INT_MASK);
	} else {
		CHECK_REG_WRITE_VALUE_WITH_MASK_WO_MSG(ISPOST_ISPCTRL0, (ISPOST_ISPCTRL0_VSINT_MASK | ISPOST_ISPCTRL0_INT_MASK), ui32Ret, ISPOST_ISPCTRL0_ALL_INT_MASK);
	}

	return (ui32Ret);
}

ISPOST_RESULT
IspostVsfwIntChk(
	ISPOST_UINT8 ui8IntOffOn
	)
{
	ISPOST_UINT32 ui32Ret = 0;

#if defined(ISPOST_REG_DEBUG)
	printf("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
	printf("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
	if (ISPOST_OFF == ui8IntOffOn) {
		CHECK_REG_WRITE_VALUE_WITH_MASK_WO_MSG(ISPOST_ISPCTRL0, 0x00000000, ui32Ret, ISPOST_ISPCTRL0_ALL_INT_MASK);
	} else {
		CHECK_REG_WRITE_VALUE_WITH_MASK_WO_MSG(ISPOST_ISPCTRL0, (ISPOST_ISPCTRL0_VSFWINT_MASK | ISPOST_ISPCTRL0_INT_MASK), ui32Ret, ISPOST_ISPCTRL0_ALL_INT_MASK);
	}

	return (ui32Ret);
}

ISPOST_RESULT
IspostVsVsfwIntChk(
	ISPOST_UINT8 ui8IntOffOn
	)
{
	ISPOST_UINT32 ui32Ret = 0;

#if defined(ISPOST_REG_DEBUG)
	printf("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
	printf("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
	if (ISPOST_OFF == ui8IntOffOn) {
		CHECK_REG_WRITE_VALUE_WITH_MASK_WO_MSG(ISPOST_ISPCTRL0, 0x00000000, ui32Ret, ISPOST_ISPCTRL0_ALL_INT_MASK);
	} else {
		CHECK_REG_WRITE_VALUE_WITH_MASK_WO_MSG(ISPOST_ISPCTRL0, (ISPOST_ISPCTRL0_VSFWINT_MASK | ISPOST_ISPCTRL0_VSINT_MASK | ISPOST_ISPCTRL0_INT_MASK), ui32Ret, ISPOST_ISPCTRL0_ALL_INT_MASK);
	}

	return (ui32Ret);
}

ISPOST_CTRL0
IspostInterruptGet(
	ISPOST_VOID
	)
{
	ISPOST_UINT32 ui32Ctrl;

	ui32Ctrl = ISPOST_READL(ISPOST_ISPCTRL0);
	return ((ISPOST_CTRL0)ui32Ctrl);
}

ISPOST_VOID
IspostInterruptClear(
	ISPOST_VOID
	)
{
	ISPOST_UINT32 ui32Ctrl;

	ui32Ctrl = ISPOST_READL(ISPOST_ISPCTRL0);

#if 0
	// For IRQ mode.
	ui32Ctrl &= ~(ISPOST_ISPCTRL0_INT_MASK);
#else
	// For polling mode.
	ui32Ctrl &= ~(ISPOST_ISPCTRL0_INT_MASK | ISPOST_ISPCTRL0_RST_MASK | ISPOST_ISPCTRL0_EN_MASK);
#endif

	ISPOST_WRITEL(ui32Ctrl, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostInterruptVsIntClear(
	ISPOST_VOID
	)
{
	ISPOST_UINT32 ui32Ctrl;

	ui32Ctrl = ISPOST_READL(ISPOST_ISPCTRL0);

#if 0
	// For IRQ mode.
	ui32Ctrl &= ~(ISPOST_ISPCTRL0_VSINT_MASK | ISPOST_ISPCTRL0_INT_MASK);
#else
	// For polling mode.
	ui32Ctrl &= ~(ISPOST_ISPCTRL0_VSINT_MASK | ISPOST_ISPCTRL0_INT_MASK | ISPOST_ISPCTRL0_RST_MASK | ISPOST_ISPCTRL0_EN_MASK);
#endif

	ISPOST_WRITEL(ui32Ctrl, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostInterruptVsfwIntClear(
	ISPOST_VOID
	)
{
	ISPOST_UINT32 ui32Ctrl;

	ui32Ctrl = ISPOST_READL(ISPOST_ISPCTRL0);

#if 0
	// For IRQ mode.
	ui32Ctrl &= ~(ISPOST_ISPCTRL0_VSFWINT_MASK | ISPOST_ISPCTRL0_INT_MASK);
#else
	// For polling mode.
	ui32Ctrl &= ~(ISPOST_ISPCTRL0_VSFWINT_MASK | ISPOST_ISPCTRL0_INT_MASK | ISPOST_ISPCTRL0_RST_MASK | ISPOST_ISPCTRL0_EN_MASK);
#endif

	ISPOST_WRITEL(ui32Ctrl, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostInterruptVsVsfwIntClear(
	ISPOST_VOID
	)
{
	ISPOST_UINT32 ui32Ctrl;

	ui32Ctrl = ISPOST_READL(ISPOST_ISPCTRL0);

#if 0
	// For IRQ mode.
	ui32Ctrl &= ~(ISPOST_ISPCTRL0_VSFWINT_MASK | ISPOST_ISPCTRL0_VSINT_MASK | ISPOST_ISPCTRL0_INT_MASK);
#else
	// For polling mode.
	ui32Ctrl &= ~(ISPOST_ISPCTRL0_VSFWINT_MASK | ISPOST_ISPCTRL0_VSINT_MASK | ISPOST_ISPCTRL0_INT_MASK | ISPOST_ISPCTRL0_RST_MASK | ISPOST_ISPCTRL0_EN_MASK);
#endif

	ISPOST_WRITEL(ui32Ctrl, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostLcLbIntClear(
	ISPOST_VOID
	)
{
	ISPOST_UINT32 ui32Ctrl;

	ui32Ctrl = ISPOST_READL(ISPOST_ISPCTRL0);

	ui32Ctrl &= ~(ISPOST_ISPCTRL0_LBERRINT_MASK);

	ISPOST_WRITEL(ui32Ctrl, ISPOST_ISPCTRL0);
}

ISPOST_STAT0
IspostStatusGet(
	ISPOST_VOID
	)
{
	ISPOST_UINT32 ui32Status;

	ui32Status = ISPOST_READL(ISPOST_ISPSTAT0) & 0x000000ff;
	return ((ISPOST_STAT0)ui32Status);
	//return ((ui32Status & 0x0000000f) ? (ISPOST_BUSY) : (ISPOST_DONE));
}

ISPOST_STAT0
IspostStatusWithLbErrGet(
	ISPOST_VOID
	)
{
	ISPOST_UINT32 ui32Status;

	ui32Status = ISPOST_READL(ISPOST_ISPSTAT0) & 0x000001ff;
	return ((ISPOST_STAT0)ui32Status);
}

ISPOST_VOID
IspostDisable(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xfffffffc;
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostResetAndSetCtrl(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xfffffffc;
	// Activate reset ispost module.
	ISPOST_WRITEL((ui32Temp | ISPOST_RST), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostLcResetAndSetCtrl(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xfffffff0;
	// Activate reset ispost module.
	ISPOST_WRITEL((ui32Temp | ISPOST_RST_ILC | ISPOST_RST), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostVsResetAndSetCtrl(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xffffffcc;
	// Activate reset ispost module.
	ISPOST_WRITEL((ui32Temp | ISPOST_RST_VS_LMV | ISPOST_RST), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostLcVsResetAndSetCtrl(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xffffffc0;
	// Activate reset ispost module.
	ISPOST_WRITEL((ui32Temp | ISPOST_RST_VS_LMV | ISPOST_RST_ILC | ISPOST_RST), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostTrigger(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xfffffffc;
	// Start ispost module porcess.
	ISPOST_WRITEL((ui32Temp | ISPOST_EN), ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostResetSetCtrlAndTrigger(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xfffffffc;
	// Activate reset ispost module.
	ISPOST_WRITEL((ui32Temp | ISPOST_RST), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
	// Start ispost module porcess.
	ISPOST_WRITEL((ui32Temp | ISPOST_EN), ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostLcResetSetCtrlAndTrigger(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xfffffff0;
	// Activate reset ispost module.
	ISPOST_WRITEL((ui32Temp | ISPOST_RST), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
	// Start ispost module porcess.
	ISPOST_WRITEL((ui32Temp | ISPOST_EN_ILC | ISPOST_EN), ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostVsResetSetCtrlAndTrigger(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xffffffcc;
	// Activate reset ispost module.
	ISPOST_WRITEL((ui32Temp | ISPOST_RST_VS_LMV | ISPOST_RST), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
	// Start ispost module porcess.
	ISPOST_WRITEL((ui32Temp | ISPOST_EN_VS_LMV | ISPOST_EN), ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostLcVsResetSetCtrlAndTrigger(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xffffffc0;
	// Activate reset ispost module.
	ISPOST_WRITEL((ui32Temp | ISPOST_RST_VS_LMV | ISPOST_RST_ILC | ISPOST_RST), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
	// Start ispost module porcess.
	ISPOST_WRITEL((ui32Temp | ISPOST_EN_VS_LMV | ISPOST_EN_ILC | ISPOST_EN), ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostAndIspLinkResetAndSetCtrl(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xffffff3c;
	// Activate reset ispost module and ISP link.
	ISPOST_WRITEL((ui32Temp | ISPOST_RST_ISP_L | ISPOST_RST), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostAndVLinkResetAndSetCtrl(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xff7f7ffc;
	// Activate reset ispost module and Venc Link.
	ISPOST_WRITEL((ui32Temp | ISPOST_RST_VENC_L | ISPOST_RST), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostAndIspLinkAndVLinkResetAndSetCtrl(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xff7f7f3c;
	// Activate reset ispost module, ISP link and Venc Link.
	ISPOST_WRITEL((ui32Temp | ISPOST_RST_VENC_L | ISPOST_RST_ISP_L | ISPOST_RST), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostAndIspLinkTrigger(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xffffff3c;
	// Start ispost module porcess.
	ISPOST_WRITEL((ui32Temp | ISPOST_EN_ISP_L | ISPOST_EN), ISPOST_ISPCTRL0);
}

ISPOST_VOID
IspostAndIspLinkResetSetCtrlAndTrigger(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Temp;

	ui32Temp = stCtrl0.ui32Ctrl0 & 0xffffff3c;
	// Activate reset ispost module and ISP link.
	ISPOST_WRITEL((ui32Temp | ISPOST_RST_ISP_L | ISPOST_RST), ISPOST_ISPCTRL0);
	// De-activate ispost module.
	ISPOST_WRITEL(ui32Temp, ISPOST_ISPCTRL0);
	// Start ispost module and ISP link porcess.
	ISPOST_WRITEL((ui32Temp | ISPOST_EN_ISP_L | ISPOST_EN), ISPOST_ISPCTRL0);
}

ISPOST_UINT32
IspostWaitForComplete(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32TryCount = 500001;
	ISPOST_UINT32 ui32Status;
	LC_LB_ERR stLcLbErr;

	if (stCtrl0.ui1EnIspL) {
		do {
			ui32Status = IspostStatusGet().ui32Stat0;
			if (ui32Status) {
#if 0
				ISPOST_LOG("ISPOST REG RD waiting for reg 0x%08X = 0x%08X expected value: 0x%08X\n", ISPOST_ISPSTAT0, ui32Status, 0x00000000);
#else
				ISPOST_LOG("waiting reg 0x%08X = 0x%08X expected: 0x%08X\n", ISPOST_ISPSTAT0, ui32Status, 0x00000000);
#endif
			}
		} while (ui32Status);

		// Test if lb error occur.
		ui32Ret += IspostLbErrIntChk(ISPOST_OFF);

		// Read out error register.
		// Read until error fifo empty
		do {
			stLcLbErr = IspostLcLbErrGet();
			if (!stLcLbErr.ui1Empty) {
#if 0
				ISPOST_LOG("ISPOST REG RD waiting for reg 0x%08X = 0x%08X, expected value: 0x%08X\n", ISPOST_LCLBERR, stLcLbErr.ui32Error, 0x80000000);
#else
				//ISPOST_LOG("waiting reg 0x%08X = 0x%08X, expected: 0x%08X\n", ISPOST_LCLBERR, stLcLbErr.ui32Error, 0x80000000);
				ISPOST_LOG("LB ErrType = %s, Y0 = %d, X0 = %d\n", g_chLbError[stLcLbErr.ui3ErrorType], stLcLbErr.ui11y, stLcLbErr.ui12X);
#endif
			}
		} while (!stLcLbErr.ui1Empty);

		// Clear LclvInt flag.
		//IspostLcLbIntClear();

		// Disable (do not disable ISP link).
		IspostDisable(stCtrl0);
	} else {
		// Wait until busy status = 0.
		//while (IspostStatusGet().ui32Stat0) {
		//	ISPOST_LOG("ISPOST REG RD waiting for reg %x expected value: %x\n", ISPOST_ISPSTAT0, 0x00000000);
		//}
		if (stCtrl0.ui1EnILc && stCtrl0.ui1EnVS && stCtrl0.ui1EnVSR) {
			// Wait for interrupt set.
			while (--ui32TryCount && IspostVsVsfwIntChk(ISPOST_ON));
#if defined(ISPOST_REG_DEBUG)
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
			if (0 == ui32TryCount) {
				ui32Ret++;
				ISPOST_LOG("ISPOST REG RD waiting for reg %x expected value: %x\n", ISPOST_ISPCTRL0, 0x07000000);
			}
			IspostInterruptVsVsfwIntClear();
		} else if (stCtrl0.ui1EnILc && stCtrl0.ui1EnVSR) {
			// Wait for interrupt set.
			while (--ui32TryCount && IspostVsfwIntChk(ISPOST_ON));
#if defined(ISPOST_REG_DEBUG)
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
			if (0 == ui32TryCount) {
				ui32Ret++;
				ISPOST_LOG("ISPOST REG RD waiting for reg %x expected value: %x\n", ISPOST_ISPCTRL0, 0x05000000);
			}
			IspostInterruptVsfwIntClear();
		} else if (stCtrl0.ui1EnVS) {
			// Wait for interrupt set.
			while (--ui32TryCount && IspostVsIntChk(ISPOST_ON));
#if defined(ISPOST_REG_DEBUG)
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
			if (0 == ui32TryCount) {
				ui32Ret++;
				ISPOST_LOG("ISPOST REG RD waiting for reg %x expected value: %x\n", ISPOST_ISPCTRL0, 0x03000000);
			}
			IspostInterruptVsIntClear();
		} else {
			while (--ui32TryCount && IspostIntChk(ISPOST_ON));
#if defined(ISPOST_REG_DEBUG)
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
			if (0 == ui32TryCount) {
				ui32Ret++;
				ISPOST_LOG("ISPOST REG RD waiting for reg %x expected value: %x\n", ISPOST_ISPCTRL0, 0x01000000);
			}
			IspostInterruptClear();
		}
	}

	return (ui32Ret);
}

ISPOST_UINT32
IspostCompleteCheck(
	ISPOST_CTRL0 stCtrl0
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	LC_LB_ERR stLcLbErr;

	if (stCtrl0.ui1EnIspL) {
		// Test if lb error occur.
		ui32Ret += IspostLbErrIntChk(ISPOST_OFF);

		// Read out error register.
		// Read until error fifo empty
		do {
			stLcLbErr = IspostLcLbErrGet();
			if (!stLcLbErr.ui1Empty) {

#if defined(IMG_KERNEL_MODULE)
				printk("LB ErrType = %s, Y0 = %d, X0 = %d\n", g_chLbError[stLcLbErr.ui3ErrorType], stLcLbErr.ui11y, stLcLbErr.ui12X);
#else
				ISPOST_LOG("LB ErrType = %s, Y0 = %d, X0 = %d\n", g_chLbError[stLcLbErr.ui3ErrorType], stLcLbErr.ui11y, stLcLbErr.ui12X);
#endif
			}
		} while (!stLcLbErr.ui1Empty);

		// Clear LclvInt flag.
		//IspostLcLbIntClear();

		// Disable (do not disable ISP link).
		IspostDisable(stCtrl0);
	} else {
		if (stCtrl0.ui1EnILc && stCtrl0.ui1EnVS && stCtrl0.ui1EnVSR) {
#if defined(ISPOST_REG_DEBUG)
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
			IspostInterruptVsVsfwIntClear();
		} else if (stCtrl0.ui1EnILc && stCtrl0.ui1EnVSR) {
#if defined(ISPOST_REG_DEBUG)
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
			IspostInterruptVsfwIntClear();
		} else if (stCtrl0.ui1EnVS) {
#if defined(ISPOST_REG_DEBUG)
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
			IspostInterruptVsIntClear();
		} else {
#if defined(ISPOST_REG_DEBUG)
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
			IspostInterruptClear();
		}
	}

	return (ui32Ret);
}

//ISPOST_VOID
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
ISPOST_VOID
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

	ISPOST_LOG("Registers dump filename =  %s\n", pszFilename);
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
			ISPOST_LOG("ISPOST registers dump file write failed!!!\n");
		}
		fclose(RegDumpfd);
	} else {
		ISPOST_LOG("ISPOST registers dump file open failed!!!\n");
	}
}

ISPOST_VOID
IspostMemoryDump(
	char* pszFilename,
	ISPOST_UINT32 ui32StartAddress,
	ISPOST_UINT32 ui32Length
	)
{
	FILE * MemDumpfd = NULL;
	int iWriteLen = 0;
	int32_t iWriteResult = 0;

	ISPOST_LOG("Memory dump filename =  %s\n", pszFilename);
	MemDumpfd = fopen(pszFilename, "wb");
	if (NULL != MemDumpfd) {
		iWriteLen = ui32Length;
		iWriteResult = fwrite((void *)ui32StartAddress, iWriteLen, 1, MemDumpfd);
		if (iWriteResult != iWriteLen) {
			ISPOST_LOG("ISPOST memory dump file write failed!!!\n");
		}
		fclose(MemDumpfd);
	} else {
		ISPOST_LOG("ISPOST memory dump file open failed!!!\n");
	}
}
#endif //IMG_KERNEL_MODULE

ISPOST_RESULT
IspostLinkModeLcVsirDnOvSS0SS1Initial(
	ISPOST_INFO * pstIspostInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32SrcWidth = 0;
	ISPOST_UINT32 ui32SrcWidthRoundUp = 0;
	ISPOST_UINT32 ui32SrcHeight = 0;

	// ==================================================================
	// Setup lens correction information.
	//
	// Setup LC SRC addr, stride and size.
	// ============================
	//
	if ((pstIspostInfo->stCtrl0.ui1EnLC) || (pstIspostInfo->stCtrl0.ui1EnIspL)) {
		//
		// Set LB geometry index and data.
		// ============================
		//
		if (pstIspostInfo->stCtrl0.ui1EnIspL) {
			// LB geometry index.
			ui32Ret += IspostLcLbLineGeometryIdxSet(ISPOST_ENABLE, 0);

			// LB geometry data.
			if (REODER_MODE_BURST == pstIspostInfo->stIspLinkInfo.stCtrl1.ui1ReOrdMode) {
				ui32Ret += IspostLcLbLineGeometryWithFixDataSet(
					0x0000,
					pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Width + 7,
					pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Height + 4
					); // begin = 0, end = (isp_w + 7), lines = (isp_h + 4).
			} else {
				ui32Ret += IspostLcLbLineGeometryWithFixDataSet(
					0x0000,
					(((pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Width + 64 -1) & 0xffffc0) + 7),
					pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Height + 4
					); // begin = 0, end = (((isp_w + 64 - 1) & 0xffffc0) + 7), lines = (isp_h + 4).
			}
		} else {
			if (pstIspostInfo->stLcInfo.stGridBufInfo.ui8LineBufEnable) {
#if defined(IMG_KERNEL_MODULE)
				pstIspostInfo->stLcInfo.stGridBufInfo.ui32GeoBufAddr =
					(ISPOST_UINT32)phys_to_virt(pstIspostInfo->stLcInfo.stGridBufInfo.ui32GeoBufAddr);
#endif
				ui32Ret += IspostLcLbLineGeometryFrom0Set(
					pstIspostInfo->stCtrl0.ui1EnIspL,
					(ISPOST_PUINT16)(pstIspostInfo->stLcInfo.stGridBufInfo.ui32GeoBufAddr),
					pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Height
					);
			}
		}
		//
		// Fill LC source image infomation.
		// ============================
		//
		pstIspostInfo->stLcInfo.stSrcImgInfo.ui32YAddr = pstIspostInfo->ui32FirstLcSrcImgBufAddr;
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Stride = ui32LcSrcStride;
		//pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Width = ui32LcSrcWidth;
		//pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Height = ui32LcSrcHeight;
		if (pstIspostInfo->stCtrl0.ui1EnIspL) {
			ui32SrcWidthRoundUp = ((pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Width + 64 - 1) & 0xffffc0);
			ui32SrcWidth = ui32SrcWidthRoundUp + 64;
			pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Stride = ((ui32SrcWidth & 0xffc0) + ((ui32SrcWidth & 0x003f) ? 64 : 0));
			pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Width = ui32SrcWidth;
			if (REODER_MODE_BURST == pstIspostInfo->stIspLinkInfo.stCtrl1.ui1ReOrdMode) {
				pstIspostInfo->stLcInfo.stSrcImgInfo.ui32UVAddr = pstIspostInfo->stLcInfo.stSrcImgInfo.ui32YAddr +
					(pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Stride * pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Height);
			} else {
				pstIspostInfo->stLcInfo.stSrcImgInfo.ui32UVAddr = pstIspostInfo->ui32FirstLcSrcImgBufUvAddr;
			}
		} else {
			pstIspostInfo->stLcInfo.stSrcImgInfo.ui32UVAddr = pstIspostInfo->stLcInfo.stSrcImgInfo.ui32YAddr +
				(pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Stride * pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Height);
		}
		//
		// Setup LC back fill color Y, CB and CR value.
		// ============================
		//
		pstIspostInfo->stLcInfo.stBackFillColor.ui8Y = LC_BACK_FILL_Y;
		pstIspostInfo->stLcInfo.stBackFillColor.ui8CB = LC_BACK_FILL_CB;
		pstIspostInfo->stLcInfo.stBackFillColor.ui8CR = LC_BACK_FILL_CR;
		//
		// Setup LC grid buffer address, stride and grid size
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pstIspostInfo->stLcInfo.stGridBufInfo.ui32BufAddr = ui32LcGridBufAddr;
		//pstIspostInfo->stLcInfo.stGridBufInfo.ui8Size = LC_GRID_BUF_GRID_SIZE;
		if (pstIspostInfo->stCtrl0.ui1EnLC) {
			// Following mark parameters has been program in call function.
			//-------------------------------------------------------------
			//pstIspostInfo->stLcInfo.stGridBufInfo.ui16Stride = IspostLcGridBufStrideCalc(
			//	pstIspostInfo->stLcInfo.stOutImgInfo.ui16Width,
			//	pstIspostInfo->stLcInfo.stGridBufInfo.ui8Size
			//	);
		} else {
			pstIspostInfo->stLcInfo.stGridBufInfo.ui16Stride = 0;
		}
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pstIspostInfo->stLcInfo.stGridBufInfo.ui8LineBufEnable = ISPOST_DISABLE;
		//
		// Setup LC pixel cache mode
		// ============================
		//
		//pstIspostInfo->stLcInfo.stPxlCacheMode.ui32PxlCacheMode = 0;
		pstIspostInfo->stLcInfo.stPxlCacheMode.ui1WaitLineBufEnd = LC_CACHE_MODE_WLBE;
		pstIspostInfo->stLcInfo.stPxlCacheMode.ui1LineBufFifoType = LC_CACHE_MODE_FIFO_TYPE;
		pstIspostInfo->stLcInfo.stPxlCacheMode.ui2LineBufFormat = LC_CACHE_MODE_LBFMT;
		pstIspostInfo->stLcInfo.stPxlCacheMode.ui2CacheDataShapeSel = LC_CACHE_MODE_DAT_SHAPE;
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pstIspostInfo->stLcInfo.stPxlCacheMode.ui1BurstLenSet = LC_CACHE_MODE_BURST_LEN;	// For barrel distortion.
		pstIspostInfo->stLcInfo.stPxlCacheMode.ui2PreFetchCtrl = LC_CACHE_MODE_CTRL_CFG;
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pstIspostInfo->stLcInfo.stPxlCacheMode.ui1CBCRSwap = ui8CBCRSwap;
		//
		// Setup LC pixel coordinator generator mode.
		// ============================
		//
		pstIspostInfo->stLcInfo.stPxlScanMode.ui8ScanW = LC_SCAN_MODE_SCAN_W;
		pstIspostInfo->stLcInfo.stPxlScanMode.ui8ScanH = LC_SCAN_MODE_SCAN_H;
		pstIspostInfo->stLcInfo.stPxlScanMode.ui8ScanM = LC_SCAN_MODE_SCAN_M;
		//
		// Setup LC pixel coordinator destination size.
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pstIspostInfo->stLcInfo.stOutImgInfo.ui16Width = ui32LcDstWidth;
		//pstIspostInfo->stLcInfo.stOutImgInfo.ui16Height = ui32LcDstHeight;
		if (pstIspostInfo->stCtrl0.ui1EnVSRC) {
		}
		//
		// Setup LC AXI Read Request Limit, GBID and SRCID.
		// ============================
		//
		pstIspostInfo->stLcInfo.stAxiId.ui8SrcReqLmt = 4;
		pstIspostInfo->stLcInfo.stAxiId.ui8GBID = 1;
		pstIspostInfo->stLcInfo.stAxiId.ui8SRCID = 0;
		if (pstIspostInfo->stCtrl0.ui1EnVSRC) {
			ui32Ret += IspostLenCorrectionSetWithLcOutput(pstIspostInfo->stLcInfo);
		} else {
			ui32Ret += IspostLenCorrectionSet(pstIspostInfo->stLcInfo);
		}
	}
	// ==================================================================

	// ==================================================================
	// Setup video stabilization information.
	if ((pstIspostInfo->stCtrl0.ui1EnVSR) || (pstIspostInfo->stCtrl0.ui1EnVSRC)) {
		// VSIR setup.
		// ============================
		// set base address for 3D denoise input READ  and stride
		// Fill Video Stabilization Registration source image information.
		//pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Stride = 320;
		//pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Width = 320;
		//pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Height = 240;
		pstIspostInfo->stVsInfo.stSrcImgInfo.ui32YAddr = pstIspostInfo->ui32VsirSrcImgBufAddr;
		pstIspostInfo->stVsInfo.stSrcImgInfo.ui32UVAddr = pstIspostInfo->ui32VsirSrcImgBufAddr +
			(pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Stride * pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Height);
		//ui32Ret += IspostVsSrcImgSet(
		//	pstIspostInfo->stVsInfo.stSrcImgInfo.ui32YAddr,
		//	pstIspostInfo->stVsInfo.stSrcImgInfo.ui32UVAddr,
		//	pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Stride,
		//	pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Width,
		//	pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Height
		//	);

		// Fill Video Stabilization Registration roll, H and V offset.
		//pstIspostInfo->stVsInfo.stRegOffset.ui16Roll = 0;
		//pstIspostInfo->stVsInfo.stRegOffset.ui16HOff = 0;
		//pstIspostInfo->stVsInfo.stRegOffset.ui8VOff = 0;
		//ui32Ret += IspostVsRegRollAndOffsetSet(
		//	pstIspostInfo->stVsInfo.stRegOffset.ui16Roll,
		//	pstIspostInfo->stVsInfo.stRegOffset.ui16HOff,
		//	pstIspostInfo->stVsInfo.stRegOffset.ui8VOff
		//	);

		// Fill Video Stabilization Registration AXI ID.
		pstIspostInfo->stVsInfo.stAxiId.ui8FWID = 0;
		pstIspostInfo->stVsInfo.stAxiId.ui8RDID = 2;
		//ui32Ret += IspostVsAxiIdSet(pstIspostInfo->stVsInfo.stAxiId.ui8FWID, pstIspostInfo->stVsInfo.stAxiId.ui8RDID);
		if ((pstIspostInfo->stCtrl0.ui1EnDN) &&
			(ISPOST_FALSE == pstIspostInfo->stCtrl0.ui1EnLC) &&
			(ISPOST_FALSE == pstIspostInfo->stCtrl0.ui1EnIspL)) {
			ui32Ret += IspostVideoStabilizationSetForDenoiseOnly(pstIspostInfo->stVsInfo);
		} else {
			//ui32Ret += IspostVideoStabilizationSet(pstIspostInfo->stVsInfo);
		}
	}
	// ==================================================================


	// ==================================================================
	// Setup overlay information.
	if (pstIspostInfo->stCtrl0.ui1EnOV) {
#if 0
		// Setup OV overlay index.
		pstIspostInfo->stOvInfo.ui8OvIdx = 0;
		//
		// Setup OV overlay image information.
		//
		pstIspostInfo->stOvInfo.stOvImgInfo.ui32YAddr = pstIspostInfo->ui32OvImgBufAddr;
		pstIspostInfo->stOvInfo.stOvImgInfo.ui16Stride = OV_IMG_STRIDE;
		pstIspostInfo->stOvInfo.stOvImgInfo.ui16Width = OV_IMG_WIDTH;
		pstIspostInfo->stOvInfo.stOvImgInfo.ui16Height = OV_IMG_HEIGHT;
		pstIspostInfo->stOvInfo.stOvImgInfo.ui32UVAddr = pstIspostInfo->stOvInfo.stOvImgInfo.ui32YAddr +
			(pstIspostInfo->stOvInfo.stOvImgInfo.ui16Stride * pstIspostInfo->stOvInfo.stOvImgInfo.ui16Height);
		//
		// Setup OV Set overlay mode.
		//
		pstIspostInfo->stOvInfo.stOverlayMode.ui32OvMode = 0;
		pstIspostInfo->stOvInfo.stOverlayMode.ui1ENOV0 = ISPOST_ENABLE;
		pstIspostInfo->stOvInfo.stOverlayMode.ui1ENOV1 = ISPOST_DISABLE;
		pstIspostInfo->stOvInfo.stOverlayMode.ui1ENOV2 = ISPOST_DISABLE;
		pstIspostInfo->stOvInfo.stOverlayMode.ui1ENOV3 = ISPOST_DISABLE;
		pstIspostInfo->stOvInfo.stOverlayMode.ui1ENOV4 = ISPOST_DISABLE;
		pstIspostInfo->stOvInfo.stOverlayMode.ui1ENOV5 = ISPOST_DISABLE;
		pstIspostInfo->stOvInfo.stOverlayMode.ui1ENOV6 = ISPOST_DISABLE;
		pstIspostInfo->stOvInfo.stOverlayMode.ui1ENOV7 = ISPOST_DISABLE;
		pstIspostInfo->stOvInfo.stOverlayMode.ui1BCO = ISPOST_DISABLE;
		pstIspostInfo->stOvInfo.stOverlayMode.ui2OVM = OV_IMG_OV_MODE;
		//
		// Setup OV overlay offset.
		//
		pstIspostInfo->stOvInfo.stOvOffset.ui16X = OV_IMG_OV_OFFSET_X;
		pstIspostInfo->stOvInfo.stOvOffset.ui16Y = OV_IMG_OV_OFFSET_Y;
		//
		// Setup OV overlay alpha value 0.
		//
		pstIspostInfo->stOvInfo.stOvAlpha.stValue0.ui8Alpha3 = 0x33;
		pstIspostInfo->stOvInfo.stOvAlpha.stValue0.ui8Alpha2 = 0x22;
		pstIspostInfo->stOvInfo.stOvAlpha.stValue0.ui8Alpha1 = 0x11;
		pstIspostInfo->stOvInfo.stOvAlpha.stValue0.ui8Alpha0 = 0x00;
		// Setup OV overlay alpha value 1.
		pstIspostInfo->stOvInfo.stOvAlpha.stValue1.ui8Alpha3 = 0x77;
		pstIspostInfo->stOvInfo.stOvAlpha.stValue1.ui8Alpha2 = 0x66;
		pstIspostInfo->stOvInfo.stOvAlpha.stValue1.ui8Alpha1 = 0x55;
		pstIspostInfo->stOvInfo.stOvAlpha.stValue1.ui8Alpha0 = 0x44;
		// Setup OV overlay alpha value 2.
		pstIspostInfo->stOvInfo.stOvAlpha.stValue2.ui8Alpha3 = 0xbb;
		pstIspostInfo->stOvInfo.stOvAlpha.stValue2.ui8Alpha2 = 0xaa;
		pstIspostInfo->stOvInfo.stOvAlpha.stValue2.ui8Alpha1 = 0x99;
		pstIspostInfo->stOvInfo.stOvAlpha.stValue2.ui8Alpha0 = 0x88;
		// Setup OV overlay alpha value 3.
		pstIspostInfo->stOvInfo.stOvAlpha.stValue3.ui8Alpha3 = 0xff;
		pstIspostInfo->stOvInfo.stOvAlpha.stValue3.ui8Alpha2 = 0xee;
		pstIspostInfo->stOvInfo.stOvAlpha.stValue3.ui8Alpha1 = 0xdd;
		pstIspostInfo->stOvInfo.stOvAlpha.stValue3.ui8Alpha0 = 0xcc;
		// Setup OV AXI read ID.
		//
		pstIspostInfo->stOvInfo.stAxiId.ui8OVID = 8;
		ui32Ret += IspostOverlaySetByChannel(pstIspostInfo->stOvInfo);
#else
		ui32Ret += IspostOverlaySetAll(pstIspostInfo->stOvInfo);
#endif
	}
	// ==================================================================

	// ==================================================================
	// Setup 3D-denoise information.
	if (pstIspostInfo->stCtrl0.ui1EnDN) {
		//
		// Setup DNR reference input image addr, stride.
		// ============================
		//
		//if (pstIspostInfo->stCtrl0.ui1EnVSR) {
		//	pstIspostInfo->stDnInfo.stRefImgInfo.ui16Stride = pstIspostInfo->stVsInfo.stOutImgInfo.ui16Width;
		//	pstIspostInfo->stDnInfo.stRefImgInfo.ui16Width = pstIspostInfo->stVsInfo.stOutImgInfo.ui16Width;
		//	pstIspostInfo->stDnInfo.stRefImgInfo.ui16Height = pstIspostInfo->stVsInfo.stOutImgInfo.ui16Height;
		//} else {
		//	pstIspostInfo->stDnInfo.stRefImgInfo.ui16Stride = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Width;
		//	pstIspostInfo->stDnInfo.stRefImgInfo.ui16Width = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Width;
		//	pstIspostInfo->stDnInfo.stRefImgInfo.ui16Height = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Height;
		//}
		pstIspostInfo->stDnInfo.stRefImgInfo.ui32YAddr = pstIspostInfo->ui32DnRefImgBufAddr;
		pstIspostInfo->stDnInfo.stRefImgInfo.ui32UVAddr = pstIspostInfo->stDnInfo.stRefImgInfo.ui32YAddr +
			(pstIspostInfo->stDnInfo.stRefImgInfo.ui16Stride * pstIspostInfo->stDnInfo.stRefImgInfo.ui16Height);
		//ui32Ret += IspostDnRefImgSet(
		//	pstIspostInfo->stDnInfo.stRefImgInfo.ui32YAddr,
		//	pstIspostInfo->stDnInfo.stRefImgInfo.ui32UVAddr,
		//	pstIspostInfo->stDnInfo.stRefImgInfo.ui16Stride
		//	);

		//
		// Setup DNR output image addr, stride.
		// ============================
		//
		//if (pstIspostInfo->stCtrl0.ui1EnVSR) {
		//	pstIspostInfo->stDnInfo.stOutImgInfo.ui16Stride = pstIspostInfo->stVsInfo.stOutImgInfo.ui16Width;
		//	pstIspostInfo->stDnInfo.stOutImgInfo.ui16Width = pstIspostInfo->stVsInfo.stOutImgInfo.ui16Width;
		//	pstIspostInfo->stDnInfo.stOutImgInfo.ui16Height = pstIspostInfo->stVsInfo.stOutImgInfo.ui16Height;
		//} else {
		//	pstIspostInfo->stDnInfo.stOutImgInfo.ui16Stride = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Width;
		//	pstIspostInfo->stDnInfo.stOutImgInfo.ui16Width = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Width;
		//	pstIspostInfo->stDnInfo.stOutImgInfo.ui16Height = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Height;
		//}
		pstIspostInfo->stDnInfo.stOutImgInfo.ui32YAddr = pstIspostInfo->ui32DnOutImgBufAddr;
		pstIspostInfo->stDnInfo.stOutImgInfo.ui32UVAddr = pstIspostInfo->stDnInfo.stOutImgInfo.ui32YAddr +
			(pstIspostInfo->stDnInfo.stOutImgInfo.ui16Stride * pstIspostInfo->stDnInfo.stOutImgInfo.ui16Height);
		//ui32Ret += IspostDnOutImgSet(
		//	pstIspostInfo->stDnInfo.stOutImgInfo.ui32YAddr,
		//	pstIspostInfo->stDnInfo.stOutImgInfo.ui32UVAddr,
		//	pstIspostInfo->stDnInfo.stOutImgInfo.ui16Stride
		//	);

		//
		// Set DN mask curve Y.
		// ============================
		//
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].ui8MskCrvIdx = DN_MSK_CRV_IDX_Y;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_00].ui32MskCrv = 0x00118000;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_01].ui32MskCrv = 0x00228002;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_02].ui32MskCrv = 0x00338006;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_03].ui32MskCrv = 0x0152840e;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_04].ui32MskCrv = 0x0d129012;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_05].ui32MskCrv = 0x05c2eb16;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_06].ui32MskCrv = 0x00f3f71a;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_07].ui32MskCrv = 0x0037fb22;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_08].ui32MskCrv = 0x0007fda2;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_09].ui32MskCrv = 0x0007fdff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_10].ui32MskCrv = 0x0007fdff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_11].ui32MskCrv = 0x0007fdff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_12].ui32MskCrv = 0x0007fdff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_13].ui32MskCrv = 0x0007fdff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_14].ui32MskCrv = 0x0007fdff;
		//ui32Ret += Ispost3DDenoiseMskCrvSet(pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y]);

		// Set DN mask curve U.
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].ui8MskCrvIdx = DN_MSK_CRV_IDX_U;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_00].ui32MskCrv = 0x00118000;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_01].ui32MskCrv = 0x00228002;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_02].ui32MskCrv = 0x00338006;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_03].ui32MskCrv = 0x0152840e;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_04].ui32MskCrv = 0x0d129012;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_05].ui32MskCrv = 0x05c2eb16;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_06].ui32MskCrv = 0x00f3f71a;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_07].ui32MskCrv = 0x0037fb22;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_08].ui32MskCrv = 0x0007fda2;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_09].ui32MskCrv = 0x0007fdff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_10].ui32MskCrv = 0x0007fdff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_11].ui32MskCrv = 0x0007fdff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_12].ui32MskCrv = 0x0007fdff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_13].ui32MskCrv = 0x0007fdff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_14].ui32MskCrv = 0x0007fdff;
		//ui32Ret += Ispost3DDenoiseMskCrvSet(pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U]);

		// Set DN mask curve V.
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].ui8MskCrvIdx = DN_MSK_CRV_IDX_V;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_00].ui32MskCrv = 0x1ff18000;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_01].ui32MskCrv = 0x05c2ad02;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_02].ui32MskCrv = 0x00f3b906;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_03].ui32MskCrv = 0x0032bd0e;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_04].ui32MskCrv = 0x0012bd12;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_05].ui32MskCrv = 0x0012be16;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_06].ui32MskCrv = 0x0003be1a;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_07].ui32MskCrv = 0x0007be22;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_08].ui32MskCrv = 0x0007bfa2;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_09].ui32MskCrv = 0x0007bfff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_10].ui32MskCrv = 0x0007bfff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_11].ui32MskCrv = 0x0007bfff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_12].ui32MskCrv = 0x0007bfff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_13].ui32MskCrv = 0x0007bfff;
		pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_14].ui32MskCrv = 0x0007bfff;
		//ui32Ret += Ispost3DDenoiseMskCrvSet(pstIspostInfo->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V]);

		//
		// Setup DN AXI reference write and read ID.
		// ============================
		//
		pstIspostInfo->stDnInfo.stAxiId.ui8RefWrID = 3;
		pstIspostInfo->stDnInfo.stAxiId.ui8RefRdID = 3;
		//ui32Ret += IspostDnAxiIdSet(
		//	pstIspostInfo->stDnInfo.stAxiId.ui8RefWrID,
		//	pstIspostInfo->stDnInfo.stAxiId.ui8RefRdID
		//	);
		ui32Ret += Ispost3DDenoiseSet(pstIspostInfo->stDnInfo);
	}
	// ==================================================================

	// ==================================================================
	// Setup Un-Scaled output information.
	if (pstIspostInfo->stCtrl0.ui1EnUO) {
		//
		// Setup Un-Scaled output image information.
		// ============================
		//
		//if (pstIspostInfo->stCtrl0.ui1EnVSR) {
		//	pstIspostInfo->stUoInfo.stOutImgInfo.ui16Stride = pstIspostInfo->stVsInfo.stOutImgInfo.ui16Width;
		//	pstIspostInfo->stUoInfo.stOutImgInfo.ui16Width = pstIspostInfo->stVsInfo.stOutImgInfo.ui16Width;
		//	pstIspostInfo->stUoInfo.stOutImgInfo.ui16Height = pstIspostInfo->stVsInfo.stOutImgInfo.ui16Height;
		//} else {
		//	pstIspostInfo->stUoInfo.stOutImgInfo.ui16Stride = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Width;
		//	pstIspostInfo->stUoInfo.stOutImgInfo.ui16Width = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Width;
		//	pstIspostInfo->stUoInfo.stOutImgInfo.ui16Height = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Height;
		//}
		pstIspostInfo->stUoInfo.stOutImgInfo.ui32YAddr = pstIspostInfo->ui32UnScaleOutImgBufAddr;
		pstIspostInfo->stUoInfo.stOutImgInfo.ui32UVAddr = pstIspostInfo->stUoInfo.stOutImgInfo.ui32YAddr +
			(pstIspostInfo->stUoInfo.stOutImgInfo.ui16Stride * pstIspostInfo->stUoInfo.stOutImgInfo.ui16Height);
		pstIspostInfo->stUoInfo.ui8ScanH = UO_OUT_IMG_SCAN_H;
		pstIspostInfo->stUoInfo.stAxiId.ui8RefWrID = 4;
		ui32Ret += IspostUnScaleOutputSet(pstIspostInfo->stUoInfo);
	}
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 0 information.
	if (pstIspostInfo->stCtrl0.ui1EnSS0) {
		//
		// Setup Scaling stream 0 output image information.
		// ============================
		//
		// Fill SS0 output stream select.
		pstIspostInfo->stSS0Info.ui8StreamSel = SCALING_STREAM_0;
		// Fill SS0 output image information.
		pstIspostInfo->stSS0Info.stOutImgInfo.ui32YAddr = pstIspostInfo->ui32SS0OutImgBufAddr;
		pstIspostInfo->stSS0Info.stOutImgInfo.ui16Stride = SS0_OUT_IMG_STRIDE;
		pstIspostInfo->stSS0Info.stOutImgInfo.ui16Width = SS0_OUT_IMG_WIDTH;
		pstIspostInfo->stSS0Info.stOutImgInfo.ui16Height = SS0_OUT_IMG_HEIGHT;
		pstIspostInfo->stSS0Info.stOutImgInfo.ui32UVAddr = pstIspostInfo->stSS0Info.stOutImgInfo.ui32YAddr +
			(pstIspostInfo->stSS0Info.stOutImgInfo.ui16Stride * pstIspostInfo->stSS0Info.stOutImgInfo.ui16Height);
		if (pstIspostInfo->stCtrl0.ui1EnVSR) {
			ui32SrcWidth = pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Width;
			ui32SrcHeight = pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Height;
		} else {
			ui32SrcWidth = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Width;
			ui32SrcHeight = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Height;
		}
		if (ui32SrcWidth == pstIspostInfo->stSS0Info.stOutImgInfo.ui16Width) {
			pstIspostInfo->stSS0Info.stHSF.ui16SF = 1.0;
			pstIspostInfo->stSS0Info.stHSF.ui8SM = SCALING_MODE_NO_SCALING;
		} else {
			pstIspostInfo->stSS0Info.stHSF.ui16SF = IspostScalingFactotCal(ui32SrcWidth, pstIspostInfo->stSS0Info.stOutImgInfo.ui16Width);
			pstIspostInfo->stSS0Info.stHSF.ui8SM = SCALING_MODE_SCALING_DOWN;
		}
		if (ui32SrcHeight == pstIspostInfo->stSS0Info.stOutImgInfo.ui16Height) {
			pstIspostInfo->stSS0Info.stVSF.ui16SF = 1.0;
			pstIspostInfo->stSS0Info.stVSF.ui8SM = SCALING_MODE_NO_SCALING;
		} else {
			pstIspostInfo->stSS0Info.stVSF.ui16SF = IspostScalingFactotCal(ui32SrcHeight, pstIspostInfo->stSS0Info.stOutImgInfo.ui16Height);
			pstIspostInfo->stSS0Info.stVSF.ui8SM = SCALING_MODE_SCALING_DOWN;
		}
		// Fill SS0 AXI SSWID.
		pstIspostInfo->stSS0Info.stAxiId.ui8SSWID = 1;
		ui32Ret += IspostScaleStreamSet(pstIspostInfo->stSS0Info);
	}
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 1 information.
	if (pstIspostInfo->stCtrl0.ui1EnSS1) {
		//
		// Setup Scaling stream 1 output image information.
		// ============================
		//
		// Fill SS1 output stream select.
		pstIspostInfo->stSS1Info.ui8StreamSel = SCALING_STREAM_1;
		// Fill SS1 output image information.
		pstIspostInfo->stSS1Info.stOutImgInfo.ui32YAddr = pstIspostInfo->ui32SS1OutImgBufAddr;
		pstIspostInfo->stSS1Info.stOutImgInfo.ui16Stride = SS1_OUT_IMG_STRIDE;
		pstIspostInfo->stSS1Info.stOutImgInfo.ui16Width = SS1_OUT_IMG_WIDTH;
		pstIspostInfo->stSS1Info.stOutImgInfo.ui16Height = SS1_OUT_IMG_HEIGHT;
		pstIspostInfo->stSS1Info.stOutImgInfo.ui32UVAddr = pstIspostInfo->stSS1Info.stOutImgInfo.ui32YAddr +
			(pstIspostInfo->stSS1Info.stOutImgInfo.ui16Stride * pstIspostInfo->stSS1Info.stOutImgInfo.ui16Height);
		if (pstIspostInfo->stCtrl0.ui1EnVSR) {
			ui32SrcWidth = pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Width;
			ui32SrcHeight = pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Height;
		} else {
			ui32SrcWidth = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Width;
			ui32SrcHeight = pstIspostInfo->stLcInfo.stOutImgInfo.ui16Height;
		}
		if (ui32SrcWidth == pstIspostInfo->stSS1Info.stOutImgInfo.ui16Width) {
			pstIspostInfo->stSS1Info.stHSF.ui16SF = 1.0;
			pstIspostInfo->stSS1Info.stHSF.ui8SM = SCALING_MODE_NO_SCALING;
		} else {
			pstIspostInfo->stSS1Info.stHSF.ui16SF = IspostScalingFactotCal(ui32SrcWidth, pstIspostInfo->stSS1Info.stOutImgInfo.ui16Width);
			pstIspostInfo->stSS1Info.stHSF.ui8SM = SCALING_MODE_SCALING_DOWN;
		}
		if (ui32SrcHeight == pstIspostInfo->stSS1Info.stOutImgInfo.ui16Height) {
			pstIspostInfo->stSS1Info.stVSF.ui16SF = 1.0;
			pstIspostInfo->stSS1Info.stVSF.ui8SM = SCALING_MODE_NO_SCALING;
		} else {
			pstIspostInfo->stSS1Info.stVSF.ui16SF = IspostScalingFactotCal(ui32SrcHeight, pstIspostInfo->stSS1Info.stOutImgInfo.ui16Height);
			pstIspostInfo->stSS1Info.stVSF.ui8SM = SCALING_MODE_SCALING_DOWN;
		}
		// Fill SS1 AXI SSWID.
		pstIspostInfo->stSS1Info.stAxiId.ui8SSWID = 2;
		ui32Ret += IspostScaleStreamSet(pstIspostInfo->stSS1Info);
	}
	// ==================================================================

	if (pstIspostInfo->stCtrl0.ui1EnIspL) {
		//
		// Setup ISP link start of frame address.
		// ============================
		//
		pstIspostInfo->stIspLinkInfo.ui32FirstUVAddrMSW = pstIspostInfo->ui32FirstLcSrcImgBufUvAddrMSW;
		if (REODER_MODE_BURST == pstIspostInfo->stIspLinkInfo.stCtrl1.ui1ReOrdMode) {
			pstIspostInfo->stIspLinkInfo.ui32FirstUVAddrLSW = (pstIspostInfo->stLcInfo.stSrcImgInfo.ui32UVAddr & 0x7fffffff);
		} else {
			pstIspostInfo->stIspLinkInfo.ui32FirstUVAddrLSW = (pstIspostInfo->ui32FirstLcSrcImgBufUvAddr & 0x7fffffff);
		}

		//
		// Setup ISP link control register.
		// ============================
		//
		pstIspostInfo->stIspLinkInfo.stCtrl0Link2Method.ui32Ctrl = 0;
		pstIspostInfo->stIspLinkInfo.stCtrl0Link3Method.ui32Ctrl = 0;
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pstIspostInfo->stIspLinkInfo.stCtrl1.ui32Ctrl = 0;
		pstIspostInfo->stIspLinkInfo.stCtrl2.ui32Ctrl = 0;
		pstIspostInfo->stIspLinkInfo.ui32Ctrl3 = 0;
		//--------------------------------------------------------
		if (REODER_MODE_BURST == pstIspostInfo->stIspLinkInfo.stCtrl1.ui1ReOrdMode) {
			// ISP ctrl0 (258H)
			// ISP link blank - blank w = 64
			pstIspostInfo->stIspLinkInfo.stCtrl0Link2Method.ui16BlankW = 64;
		} else {
			// ISP ctrl0 (258H)
			// ISP ctrl0 use stride 4096 for Y0, Y1 discrimination
			// bit 29 for Y, U discrimination:
			// Y MASK = 0xa, UV MASK=0x8 (ignore stride)
			// Y0 val = 0010bin	(first addr is 100a1000)
			// Y1 val = 0000bin
			// UV val = 1000bin
//			pstIspostInfo->stIspLinkInfo.stCtrl0Link3Method.ui4UvAddrMask = 0x8;
//			pstIspostInfo->stIspLinkInfo.stCtrl0Link3Method.ui4UvAddrMatchValue = 0x8;
//			pstIspostInfo->stIspLinkInfo.stCtrl0Link3Method.ui4Y1AddrMask = 0xa;
//			pstIspostInfo->stIspLinkInfo.stCtrl0Link3Method.ui4Y1AddrMatchValue = 0x0;
//			pstIspostInfo->stIspLinkInfo.stCtrl0Link3Method.ui4Y0AddrMask = 0xa;
//			pstIspostInfo->stIspLinkInfo.stCtrl0Link3Method.ui4Y0AddrMatchValue = 0x2;
			pstIspostInfo->stIspLinkInfo.stCtrl0Link3Method.ui32Ctrl = pstIspostInfo->ui32FirstLcSrcImgBufUvAddrMask;
		}
		//--------------------------------------------------------
		// ISP ctrl1 (25CH)
		// bit 31 - invert ISP address (enable link)
		// bit 30 - pad all bursts to 4w
		// bit 29 - insert bursts enable
		// bit 28 - reorder enable
		// bit 27 - reorder mode: =0: burst order, =1: addr filter (reg 258H settings)
		// bit 23:16 - reorder sequence last
		// bit 15:8 - reorder sequence sel1
		// bit  7:0 - reorder sequence sel0
		// 420, UV, Y0, Y1(last)
		pstIspostInfo->stIspLinkInfo.stCtrl1.ui8ReOrdSeqSel0 = 4;
		pstIspostInfo->stIspLinkInfo.stCtrl1.ui8ReOrdSeqSel1 = 1;
		pstIspostInfo->stIspLinkInfo.stCtrl1.ui8ReOrdSeqLast = 4;
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pstIspostInfo->stIspLinkInfo.stCtrl1.ui1ReOrdMode = REODER_MODE_BURST;
		//pstIspostInfo->stIspLinkInfo.stCtrl1.ui1ReOrdMode = REODER_MODE_ADDR_FILTER;
		pstIspostInfo->stIspLinkInfo.stCtrl1.ui1ReOrdEn = ISPOST_ENABLE;
		pstIspostInfo->stIspLinkInfo.stCtrl1.ui1InsertBurstsEn = ISPOST_ENABLE;
		pstIspostInfo->stIspLinkInfo.stCtrl1.ui1PadAllBurstsTo4W = ISPOST_ENABLE;
		pstIspostInfo->stIspLinkInfo.stCtrl1.ui1InvertIspAddr = ISPOST_ENABLE;
		//--------------------------------------------------------
		// ISP ctrl2 (264H)
		// bit 23:16 - burst_insert_len - insert burst_insert_len 4word bursts
		// bit  11:0 - burst_insert_w  - insert burst_insert_len 4word bursts after burst_insert_w input bursts
		// 420: 3 bursts = 64 pixels
		// 422: 2 bursts = 64 pixels
		pstIspostInfo->stIspLinkInfo.stCtrl2.ui12BurstInsertW = (3 * (ui32SrcWidthRoundUp / 64));	// 3*($::isp_w_rup/64)) = (3 * (1920 / 64)) = 90 = 0x5a.
		pstIspostInfo->stIspLinkInfo.stCtrl2.ui8BurstInsertLen = 3;
		//--------------------------------------------------------
		ui32Ret += IspostIspLinkSet(pstIspostInfo->stIspLinkInfo);
	}

	if (pstIspostInfo->stCtrl0.ui1EnVencL) {
		//
		// Setup video link control register.
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pstIspostInfo->stVLinkInfo.stVLinkCtrl.ui32Ctrl = 0;
		pstIspostInfo->stVLinkInfo.stVLinkCtrl.ui5FifoThreshold = 2;
		pstIspostInfo->stVLinkInfo.stVLinkCtrl.ui1HMB = ISPOST_ENABLE;
		//pstIspostInfo->stVLinkInfo.stVLinkCtrl.ui5CmdFifoThreshold = 0;
		//-------------------------------------------------------------
		//pstIspostInfo->stVLinkInfo.stVLinkCtrl.ui1DataSel = DATA_SEL_UN_SCALED;

		//
		// Setup video link start of frame address.
		// ============================
		//
		pstIspostInfo->stVLinkInfo.ui32StartAddr = pstIspostInfo->stUoInfo.stOutImgInfo.ui32YAddr;
		ui32Ret += IspostVLinkSet(pstIspostInfo->stVLinkInfo);
	}

	return (ui32Ret);
}

ISPOST_RESULT
IspostLinkModeLcVsirDnOvSS0SS1SetBuf(
	ISPOST_INFO * pstIspostInfo
	)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Ctrl;
	ISPOST_UINT32 ui32YSize;
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
//	ISPOST_UINT32 ui32Data;
	ISPOST_UINT32 ui32StreamOffset;
#endif

#if 0
	// ==================================================================
	// Check ISPOST status.
	ISPOST_LOG("ISPOST:readl(0x%08X)(ISPOST Ctrl0) = 0x%08X\n", ISPOST_ISPCTRL0, IspostInterruptGet().ui32Ctrl0);
	ISPOST_LOG("ISPOST:readl(0x%08X)(ISPOST Stat0) = 0x%08X\n", ISPOST_ISPSTAT0, IspostStatusGet().ui32Stat0);
	// ==================================================================

#endif
	// ==================================================================
	// Setup lens correction information.
	//
	// Setup LC SRC addr.
	// ============================
	//
	if ((pstIspostInfo->stCtrl0.ui1EnLC) || (pstIspostInfo->stCtrl0.ui1EnIspL)) {
		pstIspostInfo->stLcInfo.stSrcImgInfo.ui32YAddr = pstIspostInfo->ui32FirstLcSrcImgBufAddr;
		if (pstIspostInfo->stCtrl0.ui1EnIspL) {
			if (REODER_MODE_BURST == pstIspostInfo->stIspLinkInfo.stCtrl1.ui1ReOrdMode) {
				pstIspostInfo->stLcInfo.stSrcImgInfo.ui32UVAddr = pstIspostInfo->stLcInfo.stSrcImgInfo.ui32YAddr +
					(pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Stride * pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Height);
			} else {
				pstIspostInfo->stLcInfo.stSrcImgInfo.ui32UVAddr = pstIspostInfo->ui32FirstLcSrcImgBufUvAddr;
			}
		} else {
			ui32YSize = pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Stride * pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Height;
			pstIspostInfo->stLcInfo.stSrcImgInfo.ui32UVAddr = pstIspostInfo->ui32FirstLcSrcImgBufAddr + ui32YSize;
		}
		ui32Ret += IspostLcSrcImgAddrSet(
			pstIspostInfo->stLcInfo.stSrcImgInfo.ui32YAddr,
			pstIspostInfo->stLcInfo.stSrcImgInfo.ui32UVAddr
			);
#if 0
		ISPOST_LOG("LcSrc Y_Addr=0x%08X, UV_Addr = 0x%08X\n", pstIspostInfo->stLcInfo.stSrcImgInfo.ui32YAddr, pstIspostInfo->stLcInfo.stSrcImgInfo.ui32UVAddr);
#endif
	}
	// ==================================================================

	// ==================================================================
	// Setup video stabilization information.
	if ((pstIspostInfo->stCtrl0.ui1EnVSR) || (pstIspostInfo->stCtrl0.ui1EnVSRC)) {
		ui32YSize = pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Stride * pstIspostInfo->stVsInfo.stSrcImgInfo.ui16Height;
		pstIspostInfo->stVsInfo.stSrcImgInfo.ui32YAddr = pstIspostInfo->ui32VsirSrcImgBufAddr;
		pstIspostInfo->stVsInfo.stSrcImgInfo.ui32UVAddr = pstIspostInfo->ui32VsirSrcImgBufAddr + ui32YSize;
		ui32Ret += IspostVsSrcImgAddrSet(
			pstIspostInfo->stVsInfo.stSrcImgInfo.ui32YAddr,
			pstIspostInfo->stVsInfo.stSrcImgInfo.ui32UVAddr
			);
	}
	// ==================================================================

	// ==================================================================
	// Setup 3D-denoise information.
	if ((pstIspostInfo->stCtrl0.ui1EnDN) || (pstIspostInfo->stCtrl0.ui1EnDNWR)) {
		//
		// Setup DNR reference input image addr.
		// ============================
		//
		ui32YSize = pstIspostInfo->stDnInfo.stRefImgInfo.ui16Stride * pstIspostInfo->stDnInfo.stRefImgInfo.ui16Height;
		pstIspostInfo->stDnInfo.stRefImgInfo.ui32YAddr = pstIspostInfo->ui32DnRefImgBufAddr;
		pstIspostInfo->stDnInfo.stRefImgInfo.ui32UVAddr = pstIspostInfo->ui32DnRefImgBufAddr + ui32YSize;
		ui32Ret += IspostDnRefImgAddrSet(
			pstIspostInfo->stDnInfo.stRefImgInfo.ui32YAddr,
			pstIspostInfo->stDnInfo.stRefImgInfo.ui32UVAddr
			);
		//
		// Setup DNR output image addr.
		// ============================
		//
		ui32YSize = pstIspostInfo->stDnInfo.stOutImgInfo.ui16Stride * pstIspostInfo->stDnInfo.stOutImgInfo.ui16Height;
		pstIspostInfo->stDnInfo.stOutImgInfo.ui32YAddr = pstIspostInfo->ui32DnOutImgBufAddr;
		pstIspostInfo->stDnInfo.stOutImgInfo.ui32UVAddr = pstIspostInfo->ui32DnOutImgBufAddr + ui32YSize;
		ui32Ret += IspostDnOutImgAddrSet(
			pstIspostInfo->stDnInfo.stOutImgInfo.ui32YAddr,
			pstIspostInfo->stDnInfo.stOutImgInfo.ui32UVAddr
			);
	}
	// ==================================================================

	// ==================================================================
	// Setup Un-Scaled output information.
	if (pstIspostInfo->stCtrl0.ui1EnUO) {
		//
		// Setup Un-Scaled output image information.
		// ============================
		//
		ui32YSize = pstIspostInfo->stUoInfo.stOutImgInfo.ui16Stride * pstIspostInfo->stUoInfo.stOutImgInfo.ui16Height;
		pstIspostInfo->stUoInfo.stOutImgInfo.ui32YAddr = pstIspostInfo->ui32UnScaleOutImgBufAddr;
		pstIspostInfo->stUoInfo.stOutImgInfo.ui32UVAddr = pstIspostInfo->stUoInfo.stOutImgInfo.ui32YAddr + ui32YSize;
		ui32Ret += IspostUnSOutImgAddrSet(
			pstIspostInfo->stUoInfo.stOutImgInfo.ui32YAddr,
			pstIspostInfo->stUoInfo.stOutImgInfo.ui32UVAddr
			);
	}
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 0 information.
	if (pstIspostInfo->stCtrl0.ui1EnSS0) {
		//
		// Setup Scaling stream 0 output image information.
		// ============================
		//
		ui32YSize = pstIspostInfo->stSS0Info.stOutImgInfo.ui16Stride * pstIspostInfo->stSS0Info.stOutImgInfo.ui16Height;
		pstIspostInfo->stSS0Info.stOutImgInfo.ui32YAddr = pstIspostInfo->ui32SS0OutImgBufAddr;
		pstIspostInfo->stSS0Info.stOutImgInfo.ui32UVAddr = pstIspostInfo->ui32SS0OutImgBufAddr + ui32YSize;
		ui32Ret += IspostSSOutImgAddrSet(
			SCALING_STREAM_0,
			pstIspostInfo->stSS0Info.stOutImgInfo.ui32YAddr,
			pstIspostInfo->stSS0Info.stOutImgInfo.ui32UVAddr
			);
	}
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 1 information.
	if (pstIspostInfo->stCtrl0.ui1EnSS1) {
		//
		// Setup Scaling stream 1 output image information.
		// ============================
		//
		ui32YSize = pstIspostInfo->stSS1Info.stOutImgInfo.ui16Stride * pstIspostInfo->stSS1Info.stOutImgInfo.ui16Height;
		pstIspostInfo->stSS1Info.stOutImgInfo.ui32YAddr = pstIspostInfo->ui32SS1OutImgBufAddr;
		pstIspostInfo->stSS1Info.stOutImgInfo.ui32UVAddr = pstIspostInfo->ui32SS1OutImgBufAddr + ui32YSize;
		ui32Ret += IspostSSOutImgAddrSet(
			SCALING_STREAM_1,
			pstIspostInfo->stSS1Info.stOutImgInfo.ui32YAddr,
			pstIspostInfo->stSS1Info.stOutImgInfo.ui32UVAddr
			);
	}
	// ==================================================================

	// ==================================================================
	if (pstIspostInfo->stCtrl0.ui1EnVencL) {
		//
		// Setup video link start of frame address.
		// ============================
		//
		pstIspostInfo->stVLinkInfo.ui32StartAddr = pstIspostInfo->stUoInfo.stOutImgInfo.ui32YAddr;
		ui32Ret += IspostVLinkStartAddrSet(pstIspostInfo->stVLinkInfo.ui32StartAddr);
	}
	// ==================================================================

	// ==================================================================
	// Reset ispost module and ISP link.
	if (pstIspostInfo->stCtrl0.ui1EnDN) {
		ui32Ctrl = ((pstIspostInfo->stCtrl0.ui32Ctrl0 | (ISPOST_ISPCTRL0_ENDNWR_MASK)) & ~(ISPOST_ISPCTRL0_ENDN_MASK));
	} else {
		ui32Ctrl = pstIspostInfo->stCtrl0.ui32Ctrl0;
	}
	if (((pstIspostInfo->stCtrl0.ui1EnIspL) || (pstIspostInfo->stCtrl0.ui1EnVencL)) && (pstIspostInfo->ui8IsFirstFrame)) {
		pstIspostInfo->ui8IsFirstFrame = ISPOST_FALSE;
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pstIspostInfo->stCtrl0.ui32Ctrl0 = 0;
		//pstIspostInfo->stCtrl0.ui1EnLC = ISPOST_ENABLE;
		//pstIspostInfo->stCtrl0.ui1EnLcLb = ISPOST_ENABLE;
		//pstIspostInfo->stCtrl0.ui1EnUO = ISPOST_ENABLE;
#if 0
		if ((pstIspostInfo->stCtrl0.ui1EnDN) && (pstIspostInfo->ui8IsFirstFrame)) {
			ui32Ctrl = pstIspostInfo->stCtrl0.ui32Ctrl0 | (ISPOST_ISPCTRL0_ENDNWR_MASK) & ~(ISPOST_ISPCTRL0_ENDN_MASK) & ~(ISPOST_ISPCTRL0_ENVENCL_MASK);
		} else {
			ui32Ctrl = pstIspostInfo->stCtrl0.ui32Ctrl0 & ~(ISPOST_ISPCTRL0_ENVENCL_MASK);
		}
#else
		ui32Ctrl = pstIspostInfo->stCtrl0.ui32Ctrl0 & ~(ISPOST_ISPCTRL0_ENVENCL_MASK);
#endif
		//IspostAndIspLinkResetAndSetCtrl((ISPOST_CTRL0)ui32Ctrl);
		IspostAndIspLinkAndVLinkResetAndSetCtrl((ISPOST_CTRL0)ui32Ctrl);
	} else {
		// Reset, enable (do not reset ISP link).
#if 0
		if ((pstIspostInfo->stCtrl0.ui1EnDN) && (pstIspostInfo->ui8IsFirstFrame)) {
			ui32Ctrl = pstIspostInfo->stCtrl0.ui32Ctrl0 | (ISPOST_ISPCTRL0_ENDNWR_MASK) & ~(ISPOST_ISPCTRL0_ENDN_MASK);
		} else {
			ui32Ctrl = pstIspostInfo->stCtrl0.ui32Ctrl0;
		}
#else
		ui32Ctrl = pstIspostInfo->stCtrl0.ui32Ctrl0;
#endif
		IspostResetAndSetCtrl((ISPOST_CTRL0)ui32Ctrl);
	}
	// ==================================================================

	// ==================================================================
	if (pstIspostInfo->stCtrl0.ui1EnIspL) {
		// set second frame ISP link frame start address (will be latched when first address matches - do it before enable)
		pstIspostInfo->stIspLinkInfo.ui32NextUVAddrMSW = pstIspostInfo->ui32NextLcSrcImgBufUvAddrMSW;
		if (REODER_MODE_BURST == pstIspostInfo->stIspLinkInfo.stCtrl1.ui1ReOrdMode) {
			pstIspostInfo->stIspLinkInfo.ui32NextUVAddrLSW = ((pstIspostInfo->ui32NextLcSrcImgBufAddr +
				(pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Stride * pstIspostInfo->stLcInfo.stSrcImgInfo.ui16Height)) & 0x7fffffff);
		} else {
			pstIspostInfo->stIspLinkInfo.ui32NextUVAddrLSW = (pstIspostInfo->ui32NextLcSrcImgBufUvAddr & 0x7fffffff);
		}
		ui32Ret += IspostIspLinkNextStartAddrSet(pstIspostInfo->stIspLinkInfo);
	}
	// ==================================================================

	return (ui32Ret);
}



ISPOST_RESULT IspostUserInit(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32SrcWidth = 0;
	ISPOST_UINT32 ui32SrcWidthRoundUp = 0;
	ISPOST_UINT32 ui32SrcHeight = 0;
	ISPOST_UINT32 ui32GridLength = 0;
	struct clk * busclk;
	ISPOST_UINT32 ui32ClkRate;

	if (pIspostUser->stCtrl0.ui1EnIspL)
	{
		// Change ISP clock.
		//printk("=== Change isp clock for isp-link mode ===\n");
		busclk = clk_get_sys("imap-isp", "imap-isp");
		ui32ClkRate = 80000000;
		ui32Ret = clk_set_rate(busclk, ui32ClkRate);
		if (0 > ui32Ret)
		{
			printk("@@@ failed to set isp bus clock\n");
		}
		else
		{
			clk_prepare_enable(busclk);
		}
		//printk("@@@ Set isp bus clock -> %d\n", ui32ClkRate);
		busclk = clk_get_sys("imap-isp", "imap-isp");
		ui32ClkRate = clk_get_rate(busclk);
		//printk("@@@ check isp bus clock -> %d\n",ui32ClkRate);
	}
	if (pIspostUser->stCtrl0.ui1EnIspL || pIspostUser->stCtrl0.ui1EnVencL)
	{
		// Change ISPOST clock.
		//printk("=== Change ispost clock for isp-link/vlink mode ===\n");
		busclk = clk_get_sys("imap-ispost", "imap-ispost");
		ui32ClkRate = 160000000;
		ui32Ret = clk_set_rate(busclk, ui32ClkRate);
		if (0 > ui32Ret)
		{
			printk("@@@ failed to set ispost bus clock\n");
		}
		else
		{
			clk_prepare_enable(busclk);
		}
		//printk("@@@ Set ispost bus clock -> %d\n", ui32ClkRate);
		busclk = clk_get_sys("imap-ispost", "imap-ispost");
		ui32ClkRate = clk_get_rate(busclk);
		//printk("@@@ check ispost bus clock -> %d\n",ui32ClkRate);
	}
	if (pIspostUser->stCtrl0.ui1EnVencL)
	{
		// Change VENC264 clock.
		//printk("=== Change venc clock for vlink mode ===\n");
		busclk = clk_get_sys("imap-venc264", "imap-venc264");
		ui32ClkRate = 320000000;
		ui32Ret = clk_set_rate(busclk, ui32ClkRate);
		if (0 > ui32Ret)
		{
			printk("@@@ failed to set venc264 bus clock\n");
		}
		else
		{
			clk_prepare_enable(busclk);
		}
		//printk("@@@ Set venc264 bus clock -> %d\n", ui32ClkRate);
		busclk = clk_get_sys("imap-venc264", "imap-venc264");
		ui32ClkRate = clk_get_rate(busclk);
		//printk("@@@ check venc264 bus clock -> %d\n",ui32ClkRate);
	}

#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_CTRL) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
	{
		printk("@@@ ===================================================================\n");
		printk("@@@ IspostUserInit:LC(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnLC, pIspostUser->stLcInfo.stSrcImgInfo.ui16Width, pIspostUser->stLcInfo.stSrcImgInfo.ui16Height);
		printk("@@@ IspostUserInit:OV0(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnOV, pIspostUser->stOvInfo.stOvImgInfo[0].ui16Width, pIspostUser->stOvInfo.stOvImgInfo[0].ui16Height);
		printk("@@@ IspostUserInit:DN(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnDN, pIspostUser->stLcInfo.stOutImgInfo.ui16Width, pIspostUser->stLcInfo.stOutImgInfo.ui16Height);
		printk("@@@ IspostUserInit:UO(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnUO, pIspostUser->stLcInfo.stOutImgInfo.ui16Width, pIspostUser->stLcInfo.stOutImgInfo.ui16Height);
		printk("@@@ IspostUserInit:SS0(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnSS0, pIspostUser->stSS0Info.stOutImgInfo.ui16Width, pIspostUser->stSS0Info.stOutImgInfo.ui16Height);
		printk("@@@ IspostUserInit:SS1(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnSS1, pIspostUser->stSS1Info.stOutImgInfo.ui16Width, pIspostUser->stSS1Info.stOutImgInfo.ui16Height);
		printk("@@@ IspostUserInit:IspLink(%d)\n", pIspostUser->stCtrl0.ui1EnIspL);
		printk("@@@ IspostUserInit:VLink(%d)\n", pIspostUser->stCtrl0.ui1EnVencL);
	}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
	// ==================================================================
	// Setup lens correction information.
	//
	// Setup LC SRC addr, stride and size.
	// ============================
	//
	if ((pIspostUser->stCtrl0.ui1EnLC) || (pIspostUser->stCtrl0.ui1EnIspL))
	{
		//
		// Set LB geometry index and data.
		// ============================
		//
		if (pIspostUser->stCtrl0.ui1EnIspL)
		{
			// LB geometry index.
			ui32Ret += IspostLcLbLineGeometryIdxSet(ISPOST_ENABLE, 0);

			// LB geometry data.
			if (REODER_MODE_BURST == pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdMode)
			{
				ui32Ret += IspostLcLbLineGeometryWithFixDataSet(
					0x0000,
					pIspostUser->stLcInfo.stSrcImgInfo.ui16Width + 7,
					pIspostUser->stLcInfo.stSrcImgInfo.ui16Height + 4
					); // begin = 0, end = (isp_w + 7), lines = (isp_h + 4).
			}
			else
			{
				ui32Ret += IspostLcLbLineGeometryWithFixDataSet(
					0x0000,
					(((pIspostUser->stLcInfo.stSrcImgInfo.ui16Width + 64 -1) & 0xffffc0) + 7),
					pIspostUser->stLcInfo.stSrcImgInfo.ui16Height + 4
					); // begin = 0, end = (((isp_w + 64 - 1) & 0xffffc0) + 7), lines = (isp_h + 4).
			}
		}
		else
		{
			if (pIspostUser->stLcInfo.stGridBufInfo.ui8LineBufEnable) {
#if defined(IMG_KERNEL_MODULE)
				pIspostUser->stLcInfo.stGridBufInfo.ui32GeoBufAddr =
					(ISPOST_UINT32)phys_to_virt(pIspostUser->stLcInfo.stGridBufInfo.ui32GeoBufAddr);
#endif
				ui32Ret += IspostLcLbLineGeometryFrom0Set(
					pIspostUser->stCtrl0.ui1EnIspL,
					(ISPOST_PUINT16)(pIspostUser->stLcInfo.stGridBufInfo.ui32GeoBufAddr),
					pIspostUser->stLcInfo.stSrcImgInfo.ui16Height
					);
			}
		} //end of if (pIspostUser->stCtrl0.ui1EnIspL)


		//
		// Fill LC source image information.
		// ============================
		//
		pIspostUser->stLcInfo.stSrcImgInfo.ui32YAddr = pIspostUser->ui32FirstLcSrcImgBufAddr;
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stLcInfo.stSrcImgInfo.ui16Stride = ui32LcSrcStride;
		//pIspostUser->stLcInfo.stSrcImgInfo.ui16Width = ui32LcSrcWidth;
		//pIspostUser->stLcInfo.stSrcImgInfo.ui16Height = ui32LcSrcHeight;
		if (pIspostUser->stCtrl0.ui1EnIspL)
		{
			ui32SrcWidthRoundUp = ((pIspostUser->stLcInfo.stSrcImgInfo.ui16Width + 64 - 1) & 0xffffc0);
			ui32SrcWidth = ui32SrcWidthRoundUp + 64;
			pIspostUser->stLcInfo.stSrcImgInfo.ui16Stride = ((ui32SrcWidth & 0xffc0) + ((ui32SrcWidth & 0x003f) ? 64 : 0));
			pIspostUser->stLcInfo.stSrcImgInfo.ui16Width = ui32SrcWidth;
			if (REODER_MODE_BURST == pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdMode)
			{
				pIspostUser->stLcInfo.stSrcImgInfo.ui32UVAddr = pIspostUser->stLcInfo.stSrcImgInfo.ui32YAddr +
					(pIspostUser->stLcInfo.stSrcImgInfo.ui16Stride * pIspostUser->stLcInfo.stSrcImgInfo.ui16Height);
			}
			else
			{
				pIspostUser->stLcInfo.stSrcImgInfo.ui32UVAddr = pIspostUser->ui32FirstLcSrcImgBufUvAddr;
			}
		}
		else
		{
			pIspostUser->stLcInfo.stSrcImgInfo.ui32UVAddr = pIspostUser->stLcInfo.stSrcImgInfo.ui32YAddr +
				(pIspostUser->stLcInfo.stSrcImgInfo.ui16Stride * pIspostUser->stLcInfo.stSrcImgInfo.ui16Height);
		} //end of if (pIspostUser->stCtrl0.ui1EnIspL)


		//
		// Setup LC back fill color Y, CB and CR value.
		// ============================
		//
		//pIspostUser->stLcInfo.stBackFillColor.ui8Y = LC_BACK_FILL_Y;
		//pIspostUser->stLcInfo.stBackFillColor.ui8CB = LC_BACK_FILL_CB;
		//pIspostUser->stLcInfo.stBackFillColor.ui8CR = LC_BACK_FILL_CR;
		//
		// Setup LC grid buffer address, stride and grid size
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stLcInfo.stGridBufInfo.ui32BufAddr = ui32LcGridBufAddr;
		//pIspostUser->stLcInfo.stGridBufInfo.ui8Size = LC_GRID_BUF_GRID_SIZE;
		pIspostUser->stLcInfo.stGridBufInfo.ui16Stride = IspostLcGridBufStrideCalc(
			pIspostUser->stLcInfo.stOutImgInfo.ui16Width,
			pIspostUser->stLcInfo.stGridBufInfo.ui8Size
			);
		ui32GridLength = IspostLcGridBufLengthCalc(
			pIspostUser->stLcInfo.stOutImgInfo.ui16Height,
			pIspostUser->stLcInfo.stGridBufInfo.ui16Stride,
			pIspostUser->stLcInfo.stGridBufInfo.ui8Size
			);
		ui32GridLength = ((ui32GridLength + 0x3F) & 0xFFFFFFC0);
		pIspostUser->stLcInfo.stGridBufInfo.ui32BufAddr +=
			pIspostUser->stLcInfo.stGridBufInfo.ui32GridTargetIdx * ui32GridLength;


		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stLcInfo.stGridBufInfo.ui8LineBufEnable = ISPOST_DISABLE;
		//
		// Setup LC pixel cache mode
		// ============================
		//
		//pIspostUser->stLcInfo.stPxlCacheMode.ui32PxlCacheMode = 0;
		//pIspostUser->stLcInfo.stPxlCacheMode.ui1WaitLineBufEnd = LC_CACHE_MODE_WLBE;
		//pIspostUser->stLcInfo.stPxlCacheMode.ui1LineBufFifoType = LC_CACHE_MODE_FIFO_TYPE;
		//pIspostUser->stLcInfo.stPxlCacheMode.ui2LineBufFormat = LC_CACHE_MODE_LBFMT;
		//pIspostUser->stLcInfo.stPxlCacheMode.ui2CacheDataShapeSel = LC_CACHE_MODE_DAT_SHAPE;
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stLcInfo.stPxlCacheMode.ui1BurstLenSet = LC_CACHE_MODE_BURST_LEN;	// For barrel distortion.
		//pIspostUser->stLcInfo.stPxlCacheMode.ui2PreFetchCtrl = LC_CACHE_MODE_CTRL_CFG;
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stLcInfo.stPxlCacheMode.ui1CBCRSwap = ui8CBCRSwap;
		//
		// Setup LC pixel coordinator generator mode.
		// ============================
		//
		//pIspostUser->stLcInfo.stPxlScanMode.ui8ScanW = LC_SCAN_MODE_SCAN_W;
		//pIspostUser->stLcInfo.stPxlScanMode.ui8ScanH = LC_SCAN_MODE_SCAN_H;
		//pIspostUser->stLcInfo.stPxlScanMode.ui8ScanM = LC_SCAN_MODE_SCAN_M;
		//
		// Setup LC pixel coordinator destination size.
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stLcInfo.stOutImgInfo.ui16Width = ui32LcDstWidth;
		//pIspostUser->stLcInfo.stOutImgInfo.ui16Height = ui32LcDstHeight;
		if (pIspostUser->stCtrl0.ui1EnVSRC)
		{
		}


		//
		// Setup LC AXI Read Request Limit, GBID and SRCID.
		// ============================
		//
		pIspostUser->stLcInfo.stAxiId.ui8SrcReqLmt = 4;
		pIspostUser->stLcInfo.stAxiId.ui8GBID = 1;
		pIspostUser->stLcInfo.stAxiId.ui8SRCID = 0;
		if (pIspostUser->stCtrl0.ui1EnVSRC)
		{
			ui32Ret += IspostLenCorrectionSetWithLcOutput(pIspostUser->stLcInfo);
		}
		else
		{
			ui32Ret += IspostLenCorrectionSet(pIspostUser->stLcInfo);
		}


#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_LC) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			printk("@@@ IspostUserInit:stLcInfo.stSrcImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stLcInfo.stSrcImgInfo.ui32YAddr);
			printk("@@@ IspostUserInit:stLcInfo.stSrcImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stLcInfo.stSrcImgInfo.ui32UVAddr);
			printk("@@@ IspostUserInit:stLcInfo.stSrcImgInfo.ui16Width: %d\n", pIspostUser->stLcInfo.stSrcImgInfo.ui16Width);
			printk("@@@ IspostUserInit:stLcInfo.stSrcImgInfo.ui16Height: %d\n", pIspostUser->stLcInfo.stSrcImgInfo.ui16Height);
			printk("@@@ IspostUserInit:stLcInfo.stSrcImgInfo.ui16Stride: %d\n", pIspostUser->stLcInfo.stSrcImgInfo.ui16Stride);
			printk("@@@ IspostUserInit:stLcInfo.stBackFillColor.ui8Y: %d\n", pIspostUser->stLcInfo.stBackFillColor.ui8Y);
			printk("@@@ IspostUserInit:stLcInfo.stBackFillColor.ui8CB: %d\n", pIspostUser->stLcInfo.stBackFillColor.ui8CB);
			printk("@@@ IspostUserInit:stLcInfo.stBackFillColor.ui8CR: %d\n", pIspostUser->stLcInfo.stBackFillColor.ui8CR);
			printk("@@@ IspostUserInit:stLcInfo.stGridBufInfo.ui32BufAddr: 0x%X\n", pIspostUser->stLcInfo.stGridBufInfo.ui32BufAddr);
			printk("@@@ IspostUserInit:stLcInfo.stGridBufInfo.ui32BufLen: %d\n", pIspostUser->stLcInfo.stGridBufInfo.ui32BufLen);
			printk("@@@ IspostUserInit:stLcInfo.stGridBufInfo.ui16Stride: %d\n", pIspostUser->stLcInfo.stGridBufInfo.ui16Stride);
			printk("@@@ IspostUserInit:stLcInfo.stGridBufInfo.ui8LineBufEnable: %d\n", pIspostUser->stLcInfo.stGridBufInfo.ui8LineBufEnable);
			printk("@@@ IspostUserInit:stLcInfo.stGridBufInfo.ui8Size: %d\n", pIspostUser->stLcInfo.stGridBufInfo.ui8Size);
			printk("@@@ IspostUserInit:stLcInfo.stGridBufInfo.ui32GeoBufAddr: 0x%X\n", pIspostUser->stLcInfo.stGridBufInfo.ui32GeoBufAddr);
			printk("@@@ IspostUserInit:stLcInfo.stGridBufInfo.ui32GeoBufLen: %d\n", pIspostUser->stLcInfo.stGridBufInfo.ui32GeoBufLen);
			printk("@@@ IspostUserInit:stLcInfo.stGridBufInfo.ui32GridTargetIdx: %d\n", pIspostUser->stLcInfo.stGridBufInfo.ui32GridTargetIdx);
			printk("@@@ IspostUserInit:stLcInfo.stPxlCacheMode.ui32PxlCacheMode: %d\n", pIspostUser->stLcInfo.stPxlCacheMode.ui32PxlCacheMode);
			printk("@@@ IspostUserInit:stLcInfo.stPxlScanMode.ui8ScanW: %d\n", pIspostUser->stLcInfo.stPxlScanMode.ui8ScanW);
			printk("@@@ IspostUserInit:stLcInfo.stPxlScanMode.ui8ScanH: %d\n", pIspostUser->stLcInfo.stPxlScanMode.ui8ScanH);
			printk("@@@ IspostUserInit:stLcInfo.stPxlScanMode.ui8ScanM: %d\n", pIspostUser->stLcInfo.stPxlScanMode.ui8ScanM);
			printk("@@@ IspostUserInit:stLcInfo.stOutImgInfo.ui16Width: %d\n", pIspostUser->stLcInfo.stOutImgInfo.ui16Width);
			printk("@@@ IspostUserInit:stLcInfo.stOutImgInfo.ui16Height: %d\n", pIspostUser->stLcInfo.stOutImgInfo.ui16Height);
			printk("@@@ IspostUserInit:stLcInfo.stAxiId.ui8SrcReqLmt: %d\n", pIspostUser->stLcInfo.stAxiId.ui8SrcReqLmt);
			printk("@@@ IspostUserInit:stLcInfo.stAxiId.ui8GBID: %d\n", pIspostUser->stLcInfo.stAxiId.ui8GBID);
			printk("@@@ IspostUserInit:stLcInfo.stAxiId.ui8SRCID: %d\n", pIspostUser->stLcInfo.stAxiId.ui8SRCID);
			printk("@@@ IspostUserInit:stLcInfo ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_LC;
	}
	// ==================================================================

#if defined(INFOTM_ISPOST_VSR_READY)
	// ==================================================================
	// Setup video stabilization information.
	if ((pIspostUser->stCtrl0.ui1EnVSR) || (pIspostUser->stCtrl0.ui1EnVSRC))
	{
		// VSIR setup.
		// ============================
		// set base address for 3D denoise input READ  and stride
		// Fill Video Stabilization Registration source image information.
		//pIspostUser->stVsInfo.stSrcImgInfo.ui16Stride = 320;
		//pIspostUser->stVsInfo.stSrcImgInfo.ui16Width = 320;
		//pIspostUser->stVsInfo.stSrcImgInfo.ui16Height = 240;
		pIspostUser->stVsInfo.stSrcImgInfo.ui32YAddr = pIspostUser->ui32VsirSrcImgBufAddr;
		pIspostUser->stVsInfo.stSrcImgInfo.ui32UVAddr = pIspostUser->ui32VsirSrcImgBufAddr +
			(pIspostUser->stVsInfo.stSrcImgInfo.ui16Stride * pIspostUser->stVsInfo.stSrcImgInfo.ui16Height);
		//ui32Ret += IspostVsSrcImgSet(
		//	pIspostUser->stVsInfo.stSrcImgInfo.ui32YAddr,
		//	pIspostUser->stVsInfo.stSrcImgInfo.ui32UVAddr,
		//	pIspostUser->stVsInfo.stSrcImgInfo.ui16Stride,
		//	pIspostUser->stVsInfo.stSrcImgInfo.ui16Width,
		//	pIspostUser->stVsInfo.stSrcImgInfo.ui16Height
		//	);

		// Fill Video Stabilization Registration roll, H and V offset.
		//pIspostUser->stVsInfo.stRegOffset.ui16Roll = 0;
		//pIspostUser->stVsInfo.stRegOffset.ui16HOff = 0;
		//pIspostUser->stVsInfo.stRegOffset.ui8VOff = 0;
		//ui32Ret += IspostVsRegRollAndOffsetSet(
		//	pIspostUser->stVsInfo.stRegOffset.ui16Roll,
		//	pIspostUser->stVsInfo.stRegOffset.ui16HOff,
		//	pIspostUser->stVsInfo.stRegOffset.ui8VOff
		//	);

		// Fill Video Stabilization Registration AXI ID.
		pIspostUser->stVsInfo.stAxiId.ui8FWID = 0;
		pIspostUser->stVsInfo.stAxiId.ui8RDID = 2;
		//ui32Ret += IspostVsAxiIdSet(pIspostUser->stVsInfo.stAxiId.ui8FWID, pIspostUser->stVsInfo.stAxiId.ui8RDID);
		if ((pIspostUser->stCtrl0.ui1EnDN) &&
			(ISPOST_FALSE == pIspostUser->stCtrl0.ui1EnLC) &&
			(ISPOST_FALSE == pIspostUser->stCtrl0.ui1EnIspL)) {
			ui32Ret += IspostVideoStabilizationSetForDenoiseOnly(pIspostUser->stVsInfo);
		} else {
			//ui32Ret += IspostVideoStabilizationSet(pIspostUser->stVsInfo);
		}
	} //end of if ((pIspostUser->stCtrl0.ui1EnVSR) || (pIspostUser->stCtrl0.ui1EnVSRC))
	// ==================================================================

#endif
	// ==================================================================
	// Setup overlay information.
	if (pIspostUser->stCtrl0.ui1EnOV)
	{
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		ISPOST_UINT8 ui8Idx;
		ISPOST_UINT8 ui8Channel;

#endif
		ui32Ret += IspostOverlaySetAll(pIspostUser->stOvInfo);
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_OV) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			printk("@@@ IspostUserInit:stOvInfo.ui8OverlayMode: 0x%X\n", pIspostUser->stOvInfo.stOverlayMode.ui32OvMode);
			ui8Channel = (ISPOST_UINT8)((pIspostUser->stOvInfo.stOverlayMode.ui32OvMode >> 8) & 0xff);
			for (ui8Idx=0; ui8Idx<OVERLAY_CHNL_MAX; ui8Idx++, ui8Channel>>=1)
			{
				if (ui8Channel & 0x01)
				{
					printk("@@@ IspostUserInit:stOvInfo.ui8OvIdx[0]: %d\n", ui8Idx);
					printk("@@@ IspostUserInit:stOvInfo.stOvImgInfo[%d].ui32YAddr: 0x%X\n", ui8Idx, pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr);
					printk("@@@ IspostUserInit:stOvInfo.stOvImgInfo[%d].ui32UVAddr: 0x%X\n", ui8Idx, pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui32UVAddr);
					printk("@@@ IspostUserInit:stOvInfo.stOvImgInfo[%d].ui16Stride: %d\n", ui8Idx, pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui16Stride);
					printk("@@@ IspostUserInit:stOvInfo.stOvImgInfo[%d].ui16Width: %d\n", ui8Idx, pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui16Width);
					printk("@@@ IspostUserInit:stOvInfo.stOvImgInfo[%d].ui16Height: %d\n", ui8Idx, pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui16Height);
					printk("@@@ IspostUserInit:stOvInfo.stOvOffset[%d].ui16X: %d\n", ui8Idx, pIspostUser->stOvInfo.stOvOffset[ui8Idx].ui16X);
					printk("@@@ IspostUserInit:stOvInfo.stOvOffset[%d].ui16Y: %d\n", ui8Idx, pIspostUser->stOvInfo.stOvOffset[ui8Idx].ui16Y);
					printk("@@@ IspostUserInit:stOvInfo.stOvAlpha[%d].stValue0: 0x%08X\n", ui8Idx, pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue0.ui32Alpha);
					printk("@@@ IspostUserInit:stOvInfo.stOvAlpha[%d].stValue1: 0x%08X\n", ui8Idx, pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue1.ui32Alpha);
					printk("@@@ IspostUserInit:stOvInfo.stOvAlpha[%d].stValue2: 0x%08X\n", ui8Idx, pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue2.ui32Alpha);
					printk("@@@ IspostUserInit:stOvInfo.stOvAlpha[%d].stValue3: 0x%08X\n", ui8Idx, pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue3.ui32Alpha);
					printk("@@@ IspostUserInit:stOvInfo.stAxiId[%d].ui8OVID: %d\n", ui8Idx, pIspostUser->stOvInfo.stAxiId[ui8Idx].ui8OVID);
					printk("@@@ IspostUserInit:stOvInfo ui32Ret: %d\n", ui32Ret);
					printk("@@@ .........................................\n");
				} //end of if (ui8Channel & 0x01)
			} //end of for (ui8Idx=0; ui8Idx<OVERLAY_CHNL_MAX; ui8Idx++, ui8Channel>>=1)
		} //end of if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_OV) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_OV;
	}
	// ==================================================================

	// ==================================================================
	// Setup 3D-denoise information.
	if (pIspostUser->stCtrl0.ui1EnDN)
	{
		//
		// Setup DNR reference input image addr, stride.
		// ============================
		//
		//if (pIspostUser->stCtrl0.ui1EnVSR) {
		//	pIspostUser->stDnInfo.stRefImgInfo.ui16Stride = pIspostUser->stVsInfo.stOutImgInfo.ui16Width;
		//	pIspostUser->stDnInfo.stRefImgInfo.ui16Width = pIspostUser->stVsInfo.stOutImgInfo.ui16Width;
		//	pIspostUser->stDnInfo.stRefImgInfo.ui16Height = pIspostUser->stVsInfo.stOutImgInfo.ui16Height;
		//} else {
		//	pIspostUser->stDnInfo.stRefImgInfo.ui16Stride = pIspostUser->stLcInfo.stOutImgInfo.ui16Width;
		//	pIspostUser->stDnInfo.stRefImgInfo.ui16Width = pIspostUser->stLcInfo.stOutImgInfo.ui16Width;
		//	pIspostUser->stDnInfo.stRefImgInfo.ui16Height = pIspostUser->stLcInfo.stOutImgInfo.ui16Height;
		//}
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stDnInfo.stRefImgInfo.ui32YAddr = pIspostUser->ui32DnRefImgBufAddr;
		//pIspostUser->stDnInfo.stRefImgInfo.ui32UVAddr = pIspostUser->stDnInfo.stRefImgInfo.ui32YAddr +
		//	(pIspostUser->stDnInfo.stRefImgInfo.ui16Stride * pIspostUser->stDnInfo.stRefImgInfo.ui16Height);
		//ui32Ret += IspostDnRefImgSet(
		//	pIspostUser->stDnInfo.stRefImgInfo.ui32YAddr,
		//	pIspostUser->stDnInfo.stRefImgInfo.ui32UVAddr,
		//	pIspostUser->stDnInfo.stRefImgInfo.ui16Stride
		//	);

		//
		// Setup DNR output image addr, stride.
		// ============================
		//
		//if (pIspostUser->stCtrl0.ui1EnVSR) {
		//	pIspostUser->stDnInfo.stOutImgInfo.ui16Stride = pIspostUser->stVsInfo.stOutImgInfo.ui16Width;
		//	pIspostUser->stDnInfo.stOutImgInfo.ui16Width = pIspostUser->stVsInfo.stOutImgInfo.ui16Width;
		//	pIspostUser->stDnInfo.stOutImgInfo.ui16Height = pIspostUser->stVsInfo.stOutImgInfo.ui16Height;
		//} else {
		//	pIspostUser->stDnInfo.stOutImgInfo.ui16Stride = pIspostUser->stLcInfo.stOutImgInfo.ui16Width;
		//	pIspostUser->stDnInfo.stOutImgInfo.ui16Width = pIspostUser->stLcInfo.stOutImgInfo.ui16Width;
		//	pIspostUser->stDnInfo.stOutImgInfo.ui16Height = pIspostUser->stLcInfo.stOutImgInfo.ui16Height;
		//}
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stDnInfo.stOutImgInfo.ui32YAddr = pIspostUser->ui32DnOutImgBufAddr;
		//pIspostUser->stDnInfo.stOutImgInfo.ui32UVAddr = pIspostUser->stDnInfo.stOutImgInfo.ui32YAddr +
		//	(pIspostUser->stDnInfo.stOutImgInfo.ui16Stride * pIspostUser->stDnInfo.stOutImgInfo.ui16Height);
		//ui32Ret += IspostDnOutImgSet(
		//	pIspostUser->stDnInfo.stOutImgInfo.ui32YAddr,
		//	pIspostUser->stDnInfo.stOutImgInfo.ui32UVAddr,
		//	pIspostUser->stDnInfo.stOutImgInfo.ui16Stride
		//	);

		//
		// Set DN mask curve Y.
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].ui8MskCrvIdx = DN_MSK_CRV_IDX_Y;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_00].ui32MskCrv = 0x00118000;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_01].ui32MskCrv = 0x00228002;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_02].ui32MskCrv = 0x00338006;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_03].ui32MskCrv = 0x0152840e;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_04].ui32MskCrv = 0x0d129012;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_05].ui32MskCrv = 0x05c2eb16;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_06].ui32MskCrv = 0x00f3f71a;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_07].ui32MskCrv = 0x0037fb22;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_08].ui32MskCrv = 0x0007fda2;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_09].ui32MskCrv = 0x0007fdff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_10].ui32MskCrv = 0x0007fdff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_11].ui32MskCrv = 0x0007fdff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_12].ui32MskCrv = 0x0007fdff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_13].ui32MskCrv = 0x0007fdff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y].stMskCrv[DN_MSK_CRV_DNMC_14].ui32MskCrv = 0x0007fdff;
		//ui32Ret += Ispost3DDenoiseMskCrvSet(pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_Y]);

		// Set DN mask curve U.
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].ui8MskCrvIdx = DN_MSK_CRV_IDX_U;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_00].ui32MskCrv = 0x00118000;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_01].ui32MskCrv = 0x00228002;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_02].ui32MskCrv = 0x00338006;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_03].ui32MskCrv = 0x0152840e;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_04].ui32MskCrv = 0x0d129012;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_05].ui32MskCrv = 0x05c2eb16;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_06].ui32MskCrv = 0x00f3f71a;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_07].ui32MskCrv = 0x0037fb22;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_08].ui32MskCrv = 0x0007fda2;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_09].ui32MskCrv = 0x0007fdff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_10].ui32MskCrv = 0x0007fdff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_11].ui32MskCrv = 0x0007fdff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_12].ui32MskCrv = 0x0007fdff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_13].ui32MskCrv = 0x0007fdff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U].stMskCrv[DN_MSK_CRV_DNMC_14].ui32MskCrv = 0x0007fdff;
		//ui32Ret += Ispost3DDenoiseMskCrvSet(pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_U]);

		// Set DN mask curve V.
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].ui8MskCrvIdx = DN_MSK_CRV_IDX_V;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_00].ui32MskCrv = 0x1ff18000;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_01].ui32MskCrv = 0x05c2ad02;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_02].ui32MskCrv = 0x00f3b906;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_03].ui32MskCrv = 0x0032bd0e;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_04].ui32MskCrv = 0x0012bd12;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_05].ui32MskCrv = 0x0012be16;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_06].ui32MskCrv = 0x0003be1a;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_07].ui32MskCrv = 0x0007be22;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_08].ui32MskCrv = 0x0007bfa2;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_09].ui32MskCrv = 0x0007bfff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_10].ui32MskCrv = 0x0007bfff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_11].ui32MskCrv = 0x0007bfff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_12].ui32MskCrv = 0x0007bfff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_13].ui32MskCrv = 0x0007bfff;
		//pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V].stMskCrv[DN_MSK_CRV_DNMC_14].ui32MskCrv = 0x0007bfff;
		//ui32Ret += Ispost3DDenoiseMskCrvSet(pIspostUser->stDnInfo.stDnMskCrvInfo[DN_MSK_CRV_IDX_V]);

		//
		// Setup DN AXI reference write and read ID.
		// ============================
		//
		pIspostUser->stDnInfo.stAxiId.ui8RefWrID = 3;
		pIspostUser->stDnInfo.stAxiId.ui8RefRdID = 3;
		//ui32Ret += IspostDnAxiIdSet(
		//	pIspostUser->stDnInfo.stAxiId.ui8RefWrID,
		//	pIspostUser->stDnInfo.stAxiId.ui8RefRdID
		//	);
		ui32Ret += Ispost3DDenoiseSet(pIspostUser->stDnInfo);
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_DN) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			ISPOST_UINT32 ui32CrvIdx;
			ISPOST_UINT32 ui32Idx;

			for (ui32CrvIdx=0; ui32CrvIdx<DN_MSK_CRV_IDX_MAX; ui32CrvIdx++)
			{
				printk("@@@ stDnInfo.stDnMskCrvInfo { ");
				for (ui32Idx=0; ui32Idx<DN_MSK_CRV_DNMC_MAX; ui32Idx++)
					printk("0x%08X, ", pIspostUser->stDnInfo.stDnMskCrvInfo[ui32CrvIdx].stMskCrv[ui32Idx].ui32MskCrv);
				printk(" }\n");
			}
			printk("@@@ IspostUserInit:stDnInfo.stOutImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stDnInfo.stOutImgInfo.ui32YAddr);
			printk("@@@ IspostUserInit:stDnInfo.stOutImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stDnInfo.stOutImgInfo.ui32UVAddr);
			printk("@@@ IspostUserInit:stDnInfo.stOutImgInfo.ui16Stride: %d\n", pIspostUser->stDnInfo.stOutImgInfo.ui16Stride);
			printk("@@@ IspostUserInit:stDnInfo.stRefImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stDnInfo.stRefImgInfo.ui32YAddr);
			printk("@@@ IspostUserInit:stDnInfo.stRefImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stDnInfo.stRefImgInfo.ui32UVAddr);
			printk("@@@ IspostUserInit:stDnInfo.stRefImgInfo.ui16Stride: %d\n", pIspostUser->stDnInfo.stRefImgInfo.ui16Stride);
			printk("@@@ IspostUserInit:stDnInfo.ui32DnTargetIdx: %d\n", pIspostUser->stDnInfo.ui32DnTargetIdx);
			printk("@@@ IspostUserInit:stDnInfo.stAxiId.ui8RefWrID: %d\n", pIspostUser->stDnInfo.stAxiId.ui8RefWrID);
			printk("@@@ IspostUserInit:stDnInfo.stAxiId.ui8RefRdID: %d\n", pIspostUser->stDnInfo.stAxiId.ui8RefRdID);
			printk("@@@ IspostUserInit:stDnInfo ui32Ret: %d\n", ui32Ret);
		} //end of if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_DN) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_DN;
	} //end of if (pIspostUser->stCtrl0.ui1EnDN)
	// ==================================================================

	// ==================================================================
	// Setup Un-Scaled output information.
	if (pIspostUser->stCtrl0.ui1EnUO)
	{
		//
		// Setup Un-Scaled output image information.
		// ============================
		//
		//if (pIspostUser->stCtrl0.ui1EnVSR) {
		//	pIspostUser->stUoInfo.stOutImgInfo.ui16Stride = pIspostUser->stVsInfo.stOutImgInfo.ui16Width;
		//	pIspostUser->stUoInfo.stOutImgInfo.ui16Width = pIspostUser->stVsInfo.stOutImgInfo.ui16Width;
		//	pIspostUser->stUoInfo.stOutImgInfo.ui16Height = pIspostUser->stVsInfo.stOutImgInfo.ui16Height;
		//} else {
		//	pIspostUser->stUoInfo.stOutImgInfo.ui16Stride = pIspostUser->stLcInfo.stOutImgInfo.ui16Width;
		//	pIspostUser->stUoInfo.stOutImgInfo.ui16Width = pIspostUser->stLcInfo.stOutImgInfo.ui16Width;
		//	pIspostUser->stUoInfo.stOutImgInfo.ui16Height = pIspostUser->stLcInfo.stOutImgInfo.ui16Height;
		//}
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr = pIspostUser->ui32UnScaleOutImgBufAddr;
		//pIspostUser->stUoInfo.stOutImgInfo.ui32UVAddr = pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr +
		//	(pIspostUser->stUoInfo.stOutImgInfo.ui16Stride * pIspostUser->stUoInfo.stOutImgInfo.ui16Height);
		//pIspostUser->stUoInfo.ui8ScanH = UO_OUT_IMG_SCAN_H;
		pIspostUser->stUoInfo.stAxiId.ui8RefWrID = 4;
		ui32Ret += IspostUnScaleOutputSet(pIspostUser->stUoInfo);
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_UO) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			printk("@@@ IspostUserInit:stUoInfo.stOutImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr);
			printk("@@@ IspostUserInit:stUoInfo.stOutImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stUoInfo.stOutImgInfo.ui32UVAddr);
			printk("@@@ IspostUserInit:stUoInfo.stOutImgInfo.ui16Stride: %d\n", pIspostUser->stUoInfo.stOutImgInfo.ui16Stride);
			printk("@@@ IspostUserInit:stUoInfo.ui8ScanH: %d\n", pIspostUser->stUoInfo.ui8ScanH);
			printk("@@@ IspostUserInit:stUoInfo.stAxiId.ui8RefWrID: %d\n", pIspostUser->stUoInfo.stAxiId.ui8RefWrID);
			printk("@@@ IspostUserInit:stUoInfo ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_UO;
	} //end of if (pIspostUser->stCtrl0.ui1EnUO)
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 0 information.
	if (pIspostUser->stCtrl0.ui1EnSS0)
	{
		//
		// Setup Scaling stream 0 output image information.
		// ============================
		//
		// Fill SS0 output stream select.
		pIspostUser->stSS0Info.ui8StreamSel = SCALING_STREAM_0;
		// Fill SS0 output image information.
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stSS0Info.stOutImgInfo.ui32YAddr = pIspostUser->ui32SS0OutImgBufAddr;
		//pIspostUser->stSS0Info.stOutImgInfo.ui16Stride = SS0_OUT_IMG_STRIDE;
		//pIspostUser->stSS0Info.stOutImgInfo.ui16Width = SS0_OUT_IMG_WIDTH;
		//pIspostUser->stSS0Info.stOutImgInfo.ui16Height = SS0_OUT_IMG_HEIGHT;
		//pIspostUser->stSS0Info.stOutImgInfo.ui32UVAddr = pIspostUser->stSS0Info.stOutImgInfo.ui32YAddr +
		//	(pIspostUser->stSS0Info.stOutImgInfo.ui16Stride * pIspostUser->stSS0Info.stOutImgInfo.ui16Height);
		if (pIspostUser->stCtrl0.ui1EnVSR)
		{
			ui32SrcWidth = pIspostUser->stVsInfo.stSrcImgInfo.ui16Width;
			ui32SrcHeight = pIspostUser->stVsInfo.stSrcImgInfo.ui16Height;
		}
		else
		{
			ui32SrcWidth = pIspostUser->stLcInfo.stOutImgInfo.ui16Width;
			ui32SrcHeight = pIspostUser->stLcInfo.stOutImgInfo.ui16Height;
		}

		if (ui32SrcWidth == pIspostUser->stSS0Info.stOutImgInfo.ui16Width)
		{
			pIspostUser->stSS0Info.stHSF.ui16SF = 1.0;
			pIspostUser->stSS0Info.stHSF.ui8SM = SCALING_MODE_NO_SCALING;
		}
		else
		{
			pIspostUser->stSS0Info.stHSF.ui16SF = IspostScalingFactotCal(ui32SrcWidth, pIspostUser->stSS0Info.stOutImgInfo.ui16Width);
			pIspostUser->stSS0Info.stHSF.ui8SM = SCALING_MODE_SCALING_DOWN;
		}

		if (ui32SrcHeight == pIspostUser->stSS0Info.stOutImgInfo.ui16Height)
		{
			pIspostUser->stSS0Info.stVSF.ui16SF = 1.0;
			pIspostUser->stSS0Info.stVSF.ui8SM = SCALING_MODE_NO_SCALING;
		}
		else
		{
			pIspostUser->stSS0Info.stVSF.ui16SF = IspostScalingFactotCal(ui32SrcHeight, pIspostUser->stSS0Info.stOutImgInfo.ui16Height);
			pIspostUser->stSS0Info.stVSF.ui8SM = SCALING_MODE_SCALING_DOWN;
		}
		// Fill SS0 AXI SSWID.
		pIspostUser->stSS0Info.stAxiId.ui8SSWID = 1;
		ui32Ret += IspostScaleStreamSet(pIspostUser->stSS0Info);
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_SS0) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			printk("@@@ IspostUserInit:stSS0Info ui32SrcWidth: %d\n", ui32SrcWidth);
			printk("@@@ IspostUserInit:stSS0Info ui32SrcHeight: %d\n", ui32SrcHeight);
			printk("@@@ IspostUserInit:stSS0Info.stOutImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stSS0Info.stOutImgInfo.ui32YAddr);
			printk("@@@ IspostUserInit:stSS0Info.stOutImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stSS0Info.stOutImgInfo.ui32UVAddr);
			printk("@@@ IspostUserInit:stSS0Info.stOutImgInfo.ui16Stride: %d\n", pIspostUser->stSS0Info.stOutImgInfo.ui16Stride);
			printk("@@@ IspostUserInit:stSS0Info.stOutImgInfo.ui16Width: %d\n", pIspostUser->stSS0Info.stOutImgInfo.ui16Width);
			printk("@@@ IspostUserInit:stSS0Info.stOutImgInfo.ui16Height: %d\n", pIspostUser->stSS0Info.stOutImgInfo.ui16Height);
			printk("@@@ IspostUserInit:stSS0Info.stHSF.ui16SF: %d\n", pIspostUser->stSS0Info.stHSF.ui16SF);
			printk("@@@ IspostUserInit:stSS0Info.stHSF.ui8SM: %d\n", pIspostUser->stSS0Info.stHSF.ui8SM);
			printk("@@@ IspostUserInit:stSS0Info.stVSF.ui16SF: %d\n", pIspostUser->stSS0Info.stVSF.ui16SF);
			printk("@@@ IspostUserInit:stSS0Info.stVSF.ui8SM: %d\n", pIspostUser->stSS0Info.stVSF.ui8SM);
			printk("@@@ IspostUserInit:stSS0Info.stAxiId.ui8SSWID: %d\n", pIspostUser->stSS0Info.stAxiId.ui8SSWID);
			printk("@@@ IspostUserInit:stSS0Info ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_SS0;
	} //end of if (pIspostUser->stCtrl0.ui1EnSS0)
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 1 information.
	if (pIspostUser->stCtrl0.ui1EnSS1)
	{
		//
		// Setup Scaling stream 1 output image information.
		// ============================
		//
		// Fill SS1 output stream select.
		pIspostUser->stSS1Info.ui8StreamSel = SCALING_STREAM_1;
		// Fill SS1 output image information.
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stSS1Info.stOutImgInfo.ui32YAddr = pIspostUser->ui32SS1OutImgBufAddr;
		//pIspostUser->stSS1Info.stOutImgInfo.ui16Stride = SS1_OUT_IMG_STRIDE;
		//pIspostUser->stSS1Info.stOutImgInfo.ui16Width = SS1_OUT_IMG_WIDTH;
		//pIspostUser->stSS1Info.stOutImgInfo.ui16Height = SS1_OUT_IMG_HEIGHT;
		//pIspostUser->stSS1Info.stOutImgInfo.ui32UVAddr = pIspostUser->stSS1Info.stOutImgInfo.ui32YAddr +
		//	(pIspostUser->stSS1Info.stOutImgInfo.ui16Stride * pIspostUser->stSS1Info.stOutImgInfo.ui16Height);
		if (pIspostUser->stCtrl0.ui1EnVSR)
		{
			ui32SrcWidth = pIspostUser->stVsInfo.stSrcImgInfo.ui16Width;
			ui32SrcHeight = pIspostUser->stVsInfo.stSrcImgInfo.ui16Height;
		}
		else
		{
			ui32SrcWidth = pIspostUser->stLcInfo.stOutImgInfo.ui16Width;
			ui32SrcHeight = pIspostUser->stLcInfo.stOutImgInfo.ui16Height;
		}

		if (ui32SrcWidth == pIspostUser->stSS1Info.stOutImgInfo.ui16Width)
		{
			pIspostUser->stSS1Info.stHSF.ui16SF = 1.0;
			pIspostUser->stSS1Info.stHSF.ui8SM = SCALING_MODE_NO_SCALING;
		}
		else
		{
			pIspostUser->stSS1Info.stHSF.ui16SF = IspostScalingFactotCal(ui32SrcWidth, pIspostUser->stSS1Info.stOutImgInfo.ui16Width);
			pIspostUser->stSS1Info.stHSF.ui8SM = SCALING_MODE_SCALING_DOWN;
		}

		if (ui32SrcHeight == pIspostUser->stSS1Info.stOutImgInfo.ui16Height)
		{
			pIspostUser->stSS1Info.stVSF.ui16SF = 1.0;
			pIspostUser->stSS1Info.stVSF.ui8SM = SCALING_MODE_NO_SCALING;
		}
		else
		{
			pIspostUser->stSS1Info.stVSF.ui16SF = IspostScalingFactotCal(ui32SrcHeight, pIspostUser->stSS1Info.stOutImgInfo.ui16Height);
			pIspostUser->stSS1Info.stVSF.ui8SM = SCALING_MODE_SCALING_DOWN;
		}

		// Fill SS1 AXI SSWID.
		pIspostUser->stSS1Info.stAxiId.ui8SSWID = 2;
		ui32Ret += IspostScaleStreamSet(pIspostUser->stSS1Info);
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_SS1) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			printk("@@@ IspostUserInit:stSS1Info ui32SrcWidth: %d\n", ui32SrcWidth);
			printk("@@@ IspostUserInit:stSS1Info ui32SrcHeight: %d\n", ui32SrcHeight);
			printk("@@@ IspostUserInit:stSS1Info.stOutImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stSS1Info.stOutImgInfo.ui32YAddr);
			printk("@@@ IspostUserInit:stSS1Info.stOutImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stSS1Info.stOutImgInfo.ui32UVAddr);
			printk("@@@ IspostUserInit:stSS1Info.stOutImgInfo.ui16Stride: %d\n", pIspostUser->stSS1Info.stOutImgInfo.ui16Stride);
			printk("@@@ IspostUserInit:stSS1Info.stOutImgInfo.ui16Width: %d\n", pIspostUser->stSS1Info.stOutImgInfo.ui16Width);
			printk("@@@ IspostUserInit:stSS1Info.stOutImgInfo.ui16Height: %d\n", pIspostUser->stSS1Info.stOutImgInfo.ui16Height);
			printk("@@@ IspostUserInit:stSS1Info.stHSF.ui16SF: %d\n", pIspostUser->stSS1Info.stHSF.ui16SF);
			printk("@@@ IspostUserInit:stSS1Info.stHSF.ui8SM: %d\n", pIspostUser->stSS1Info.stHSF.ui8SM);
			printk("@@@ IspostUserInit:stSS1Info.stVSF.ui16SF: %d\n", pIspostUser->stSS1Info.stVSF.ui16SF);
			printk("@@@ IspostUserInit:stSS1Info.stVSF.ui8SM: %d\n", pIspostUser->stSS1Info.stVSF.ui8SM);
			printk("@@@ IspostUserInit:stSS1Info.stAxiId.ui8SSWID: %d\n", pIspostUser->stSS1Info.stAxiId.ui8SSWID);
			printk("@@@ IspostUserInit:stSS1Info ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_SS1;
	} //end of if (pIspostUser->stCtrl0.ui1EnSS1)
	// ==================================================================

	if (pIspostUser->stCtrl0.ui1EnIspL)
	{
		//
		// Setup ISP link start of frame address.
		// ============================
		//
		pIspostUser->stIspLinkInfo.ui32FirstUVAddrMSW = pIspostUser->ui32FirstLcSrcImgBufUvAddrMSW;
		if (REODER_MODE_BURST == pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdMode)
		{
			pIspostUser->stIspLinkInfo.ui32FirstUVAddrLSW = (pIspostUser->stLcInfo.stSrcImgInfo.ui32UVAddr & 0x7fffffff);
		}
		else
		{
			pIspostUser->stIspLinkInfo.ui32FirstUVAddrLSW = (pIspostUser->ui32FirstLcSrcImgBufUvAddr & 0x7fffffff);
		}

		//
		// Setup ISP link control register.
		// ============================
		//
		pIspostUser->stIspLinkInfo.stCtrl0Link2Method.ui32Ctrl = 0;
		pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui32Ctrl = 0;
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stIspLinkInfo.stCtrl1.ui32Ctrl = 0;
		pIspostUser->stIspLinkInfo.stCtrl2.ui32Ctrl = 0;
		pIspostUser->stIspLinkInfo.ui32Ctrl3 = 0;
		//--------------------------------------------------------
		if (REODER_MODE_BURST == pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdMode)
		{
			// ISP ctrl0 (258H)
			// ISP link blank - blank w = 64
			pIspostUser->stIspLinkInfo.stCtrl0Link2Method.ui16BlankW = 64;
		}
		else
		{
			// ISP ctrl0 (258H)
			// ISP ctrl0 use stride 4096 for Y0, Y1 discrimination
			// bit 29 for Y, U discrimination:
			// Y MASK = 0xa, UV MASK=0x8 (ignore stride)
			// Y0 val = 0010bin	(first addr is 100a1000)
			// Y1 val = 0000bin
			// UV val = 1000bin
			// Following mark parameters has been program in call function.
			//-------------------------------------------------------------
			//pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui4UvAddrMask = 0x8;
			//pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui4UvAddrMatchValue = 0x8;
			//pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui4Y1AddrMask = 0xa;
			//pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui4Y1AddrMatchValue = 0x0;
			//pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui4Y0AddrMask = 0xa;
			//pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui4Y0AddrMatchValue = 0x2;
			pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui32Ctrl = pIspostUser->ui32FirstLcSrcImgBufUvAddrMask;
		} //end of if (REODER_MODE_BURST == pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdMode)

		//--------------------------------------------------------
		// ISP ctrl1 (25CH)
		// bit 31 - invert ISP address (enable link)
		// bit 30 - pad all bursts to 4w
		// bit 29 - insert bursts enable
		// bit 28 - reorder enable
		// bit 27 - reorder mode: =0: burst order, =1: addr filter (reg 258H settings)
		// bit 23:16 - reorder sequence last
		// bit 15:8 - reorder sequence sel1
		// bit  7:0 - reorder sequence sel0
		// 420, UV, Y0, Y1(last)
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stIspLinkInfo.stCtrl1.ui8ReOrdSeqSel0 = 4;
		//pIspostUser->stIspLinkInfo.stCtrl1.ui8ReOrdSeqSel1 = 1;
		//pIspostUser->stIspLinkInfo.stCtrl1.ui8ReOrdSeqLast = 4;
		////pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdMode = REODER_MODE_BURST;
		//pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdMode = REODER_MODE_ADDR_FILTER;
		//pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdEn = ISPOST_ENABLE;
		//pIspostUser->stIspLinkInfo.stCtrl1.ui1InsertBurstsEn = ISPOST_ENABLE;
		//pIspostUser->stIspLinkInfo.stCtrl1.ui1PadAllBurstsTo4W = ISPOST_ENABLE;
		//pIspostUser->stIspLinkInfo.stCtrl1.ui1InvertIspAddr = ISPOST_ENABLE;
		//--------------------------------------------------------
		// ISP ctrl2 (264H)
		// bit 23:16 - burst_insert_len - insert burst_insert_len 4word bursts
		// bit  11:0 - burst_insert_w  - insert burst_insert_len 4word bursts after burst_insert_w input bursts
		// 420: 3 bursts = 64 pixels
		// 422: 2 bursts = 64 pixels
		pIspostUser->stIspLinkInfo.stCtrl2.ui12BurstInsertW = (3 * (ui32SrcWidthRoundUp / 64));	// 3*($::isp_w_rup/64)) = (3 * (1920 / 64)) = 90 = 0x5a.
		pIspostUser->stIspLinkInfo.stCtrl2.ui8BurstInsertLen = 3;
		//--------------------------------------------------------
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_ISP_L) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			printk("@@@ IspostUserInit:stIspLinkInfo.ui32FirstUVAddrMSW: 0x%X\n", pIspostUser->stIspLinkInfo.ui32FirstUVAddrMSW);
			printk("@@@ IspostUserInit:stIspLinkInfo.ui32FirstUVAddrLSW: 0x%X\n", pIspostUser->stIspLinkInfo.ui32FirstUVAddrLSW);
			if (REODER_MODE_BURST == pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdMode)
			{
				printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl0Link2Method.ui16Ctrl0: 0x%X\n", pIspostUser->stIspLinkInfo.stCtrl0Link2Method.ui16Ctrl0);
				printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl0Link2Method.ui16BlankW: 0x%X\n", pIspostUser->stIspLinkInfo.stCtrl0Link2Method.ui16BlankW);
			}
			else
			{
				printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl0Link3Method.ui4UvAddrMatchValue: 0x%X\n", pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui4UvAddrMatchValue);
				printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl0Link3Method.ui4UvAddrMask: 0x%X\n", pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui4UvAddrMask);
				printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl0Link3Method.ui4Y1AddrMatchValue: 0x%X\n", pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui4Y1AddrMatchValue);
				printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl0Link3Method.ui4Y1AddrMask: 0x%X\n", pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui4Y1AddrMask);
				printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl0Link3Method.ui4Y0AddrMatchValue: 0x%X\n", pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui4Y0AddrMatchValue);
				printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl0Link3Method.ui4Y0AddrMask: 0x%X\n", pIspostUser->stIspLinkInfo.stCtrl0Link3Method.ui4Y0AddrMask);
			}
			printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl1.ui8ReOrdSeqSel0: %d\n", pIspostUser->stIspLinkInfo.stCtrl1.ui8ReOrdSeqSel0);
			printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl1.ui8ReOrdSeqSel1: %d\n", pIspostUser->stIspLinkInfo.stCtrl1.ui8ReOrdSeqSel1);
			printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl1.ui8ReOrdSeqLast: %d\n", pIspostUser->stIspLinkInfo.stCtrl1.ui8ReOrdSeqLast);
			printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl1.ui1ReOrdMode: %s\n", (REODER_MODE_BURST == pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdMode) ? ("Burst") : ("Address Filter"));
			printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl1.ui1ReOrdEn: %s\n", (ISPOST_ENABLE == pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdEn) ? ("Enable") : ("Disable"));
			printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl1.ui1InsertBurstsEn: %s\n", (ISPOST_ENABLE == pIspostUser->stIspLinkInfo.stCtrl1.ui1InsertBurstsEn) ? ("Enable") : ("Disable"));
			printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl1.ui1PadAllBurstsTo4W: %s\n", (ISPOST_ENABLE == pIspostUser->stIspLinkInfo.stCtrl1.ui1PadAllBurstsTo4W) ? ("Enable") : ("Disable"));
			printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl1.ui1InvertIspAddr: %s\n", (ISPOST_ENABLE == pIspostUser->stIspLinkInfo.stCtrl1.ui1InvertIspAddr) ? ("Enable") : ("Disable"));
			printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl2.ui12BurstInsertW: %d\n", pIspostUser->stIspLinkInfo.stCtrl2.ui12BurstInsertW);
			printk("@@@ IspostUserInit:stIspLinkInfo.stCtrl2.ui8BurstInsertLen: %d\n", pIspostUser->stIspLinkInfo.stCtrl2.ui8BurstInsertLen);
			printk("@@@ IspostUserInit:stIspLinkInfo.ui32Ctrl3: 0x%X\n", pIspostUser->stIspLinkInfo.ui32Ctrl3);
			printk("@@@ IspostUserInit:stIspLinkInfo ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		ui32Ret += IspostIspLinkSet(pIspostUser->stIspLinkInfo);
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_ISP_L;
	} //end of if (pIspostUser->stCtrl0.ui1EnIspL)

	if (pIspostUser->stCtrl0.ui1EnVencL)
	{
		//
		// Setup video link control register.
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stVLinkInfo.stVLinkCtrl.ui32Ctrl = 0;
		//pIspostUser->stVLinkInfo.stVLinkCtrl.ui5FifoThreshold = 2;
		//pIspostUser->stVLinkInfo.stVLinkCtrl.ui1HMB = ISPOST_ENABLE;
		//pIspostUser->stVLinkInfo.stVLinkCtrl.ui5CmdFifoThreshold = 0;
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stVLinkInfo.stVLinkCtrl.ui1DataSel = DATA_SEL_UN_SCALED;

		//
		// Setup video link start of frame address.
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stVLinkInfo.ui32StartAddr = pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr;
		ui32Ret += IspostVLinkSet(pIspostUser->stVLinkInfo);
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_VENC_L) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			printk("@@@ IspostUserInit:stVLinkInfo.stVLinkCtrl.ui5FifoThreshold: %d\n", pIspostUser->stVLinkInfo.stVLinkCtrl.ui5FifoThreshold);
			printk("@@@ IspostUserInit:stVLinkInfo.stVLinkCtrl.ui1HMB: %s\n", (ISPOST_ENABLE == pIspostUser->stVLinkInfo.stVLinkCtrl.ui1HMB) ? ("Enable") : ("Disable"));
			printk("@@@ IspostUserInit:stVLinkInfo.stVLinkCtrl.ui5CmdFifoThreshold: %d\n", pIspostUser->stVLinkInfo.stVLinkCtrl.ui5CmdFifoThreshold);
			printk("@@@ IspostUserInit:stVLinkInfo.stVLinkCtrl.ui1DataSel: %s\n", (DATA_SEL_UN_SCALED == pIspostUser->stVLinkInfo.stVLinkCtrl.ui1DataSel) ? ("UO") : ("SS0"));
			printk("@@@ IspostUserInit:stVLinkInfo.ui32StartAddr: 0x%X\n", pIspostUser->stVLinkInfo.ui32StartAddr);
			printk("@@@ IspostUserInit:stVLinkInfo ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_VENC_L;
	} //end of if (pIspostUser->stCtrl0.ui1EnVencL)

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserUpdate(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32Ctrl;
	ISPOST_UINT32 ui32YSize;
#if defined(DIRECTLY_PROGRAM_ADDR_REG)
//	ISPOST_UINT32 ui32Data;
	ISPOST_UINT32 ui32StreamOffset;
#endif

#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_CTRL) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
	{
		// ==================================================================
		// Check ISPOST status.
		//ISPOST_LOG("ISPOST:readl(0x%08X)(ISPOST Ctrl0) = 0x%08X\n", ISPOST_ISPCTRL0, IspostInterruptGet().ui32Ctrl0);
		//ISPOST_LOG("ISPOST:readl(0x%08X)(ISPOST Stat0) = 0x%08X\n", ISPOST_ISPSTAT0, IspostStatusGet().ui32Stat0);
		// ==================================================================
		printk("@@@ ===================================================================\n");
		printk("@@@ LC(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnLC, pIspostUser->stLcInfo.stSrcImgInfo.ui16Width, pIspostUser->stLcInfo.stSrcImgInfo.ui16Height);
		printk("@@@ OV0(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnOV, pIspostUser->stOvInfo.stOvImgInfo[0].ui16Width, pIspostUser->stOvInfo.stOvImgInfo[0].ui16Height);
		printk("@@@ DN(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnDN, pIspostUser->stLcInfo.stOutImgInfo.ui16Width, pIspostUser->stLcInfo.stOutImgInfo.ui16Height);
		printk("@@@ UO(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnUO, pIspostUser->stLcInfo.stOutImgInfo.ui16Width, pIspostUser->stLcInfo.stOutImgInfo.ui16Height);
		printk("@@@ SS0(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnSS0, pIspostUser->stSS0Info.stOutImgInfo.ui16Width, pIspostUser->stSS0Info.stOutImgInfo.ui16Height);
		printk("@@@ SS1(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnSS1, pIspostUser->stSS1Info.stOutImgInfo.ui16Width, pIspostUser->stSS1Info.stOutImgInfo.ui16Height);
		printk("@@@ EnVSR(%d), EnIspL(%d), EnVencL(%d)\n", pIspostUser->stCtrl0.ui1EnVSR, pIspostUser->stCtrl0.ui1EnIspL, pIspostUser->stCtrl0.ui1EnVencL);

	}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
	// ==================================================================
	// Setup lens correction information.
	//
	// Setup LC SRC addr.
	// ============================
	//
	if ((pIspostUser->stCtrl0.ui1EnLC) || (pIspostUser->stCtrl0.ui1EnIspL))
	{
		pIspostUser->stLcInfo.stSrcImgInfo.ui32YAddr = pIspostUser->ui32FirstLcSrcImgBufAddr;
		if (pIspostUser->stCtrl0.ui1EnIspL)
		{
			if (REODER_MODE_BURST == pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdMode)
			{
				pIspostUser->stLcInfo.stSrcImgInfo.ui32UVAddr = pIspostUser->stLcInfo.stSrcImgInfo.ui32YAddr +
					(pIspostUser->stLcInfo.stSrcImgInfo.ui16Stride * pIspostUser->stLcInfo.stSrcImgInfo.ui16Height);
			}
			else
			{
				pIspostUser->stLcInfo.stSrcImgInfo.ui32UVAddr = pIspostUser->ui32FirstLcSrcImgBufUvAddr;
			}
		}
		else
		{
			ui32YSize = pIspostUser->stLcInfo.stSrcImgInfo.ui16Stride * pIspostUser->stLcInfo.stSrcImgInfo.ui16Height;
			pIspostUser->stLcInfo.stSrcImgInfo.ui32UVAddr = pIspostUser->ui32FirstLcSrcImgBufAddr + ui32YSize;
		}
		ui32Ret += IspostLcSrcImgAddrSet(
			pIspostUser->stLcInfo.stSrcImgInfo.ui32YAddr,
			pIspostUser->stLcInfo.stSrcImgInfo.ui32UVAddr
			);
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_LC) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			printk("@@@ IspostUserUpdate:stLcInfo.stSrcImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stLcInfo.stSrcImgInfo.ui32YAddr);
			printk("@@@ IspostUserUpdate:stLcInfo.stSrcImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stLcInfo.stSrcImgInfo.ui32UVAddr);
			printk("@@@ IspostUserUpdate:stLcInfo ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_LC;
	} //end of if ((pIspostUser->stCtrl0.ui1EnLC) || (pIspostUser->stCtrl0.ui1EnIspL))
	// ==================================================================

	// ==================================================================
	// Setup overlay information.
	IspostUserSetupOV(pIspostUser);
#if 0
	if (pIspostUser->stCtrl0.ui1EnOV)
	{
		ISPOST_UINT8 ui8Idx;
		ISPOST_UINT8 ui8Channel;

		ui8Channel = (ISPOST_UINT8)((pIspostUser->stOvInfo.stOverlayMode.ui32OvMode >> 8) & 0xff);
		for (ui8Idx=0; ui8Idx<OVERLAY_CHNL_MAX; ui8Idx++, ui8Channel>>=1)
		{
			if (ui8Channel & 0x01)
			{
				//
				// Setup overlay index.
				// ============================
				//
				ui32Ret += IspostOvIdxSet(ui8Idx);
				//
				// Setup OV overlay image source address.
				// ============================
				//
				if (0 == ui8Idx)
				{
					ui32Ret += IspostOvImgAddrSet(
						pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr,
						pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui32UVAddr
						);
				}
				else
				{
					ui32Ret += IspostOvImgYAddrSet(
						pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr
						);
				}
#if 0
				//
				// Setup OV overlay alpha value.
				// ============================
				//
				ui32Ret += IspostOvAlphaValueSet(
					pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue0,
					pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue1,
					pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue2,
					pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue3
					);
#endif
				//
				// Setup OV AXI read ID.
				// ============================
				//
				ui32Ret += IspostOvAxiIdSet(
					pIspostUser->stOvInfo.stAxiId[ui8Idx].ui8OVID
					);
			}
			ui8Channel >>= 1;
		}
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_OV) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			ui8Channel = (ISPOST_UINT8)((pIspostUser->stOvInfo.stOverlayMode.ui32OvMode >> 8) & 0xff);
			for (ui8Idx=0; ui8Idx<OVERLAY_CHNL_MAX; ui8Idx++, ui8Channel>>=1)
			{
				if (ui8Channel & 0x01)
				{
					printk("@@@ IspostUserUpdate:stOvInfo.ui8OvIdx[0]: %d\n", ui8Idx);
					printk("@@@ IspostUserUpdate:stOvInfo.stOvImgInfo[%d].ui32YAddr: 0x%X\n", ui8Idx, pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr);
					printk("@@@ IspostUserUpdate:stOvInfo.stOvImgInfo[%d].ui32UVAddr: 0x%X\n", ui8Idx, pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui32UVAddr);
  #if 0
					printk("@@@ IspostUserUpdate:stOvInfo.stOvAlpha[%d].stValue0: 0x%08X\n", ui8Idx, pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue0.ui32Alpha);
					printk("@@@ IspostUserUpdate:stOvInfo.stOvAlpha[%d].stValue1: 0x%08X\n", ui8Idx, pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue1.ui32Alpha);
					printk("@@@ IspostUserUpdate:stOvInfo.stOvAlpha[%d].stValue2: 0x%08X\n", ui8Idx, pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue2.ui32Alpha);
					printk("@@@ IspostUserUpdate:stOvInfo.stOvAlpha[%d].stValue3: 0x%08X\n", ui8Idx, pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue3.ui32Alpha);
  #endif
					printk("@@@ IspostUserUpdate:stOvInfo.stAxiId[%d].ui8OVID: %d\n", ui8Idx, pIspostUser->stOvInfo.stAxiId[ui8Idx].ui8OVID);
					printk("@@@ IspostUserUpdate:stOvInfo ui32Ret: %d\n", ui32Ret);
					printk("@@@ .........................................\n");
				}
			}
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_OV;
	}
#endif //0
	// ==================================================================

	// ==================================================================
	// Setup 3D-denoise information.
	if ((pIspostUser->stCtrl0.ui1EnDN) || (pIspostUser->stCtrl0.ui1EnDNWR))
	{
		//
		// Setup DNR reference input image addr.
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//ui32YSize = pIspostUser->stDnInfo.stRefImgInfo.ui16Stride * pIspostUser->stDnInfo.stRefImgInfo.ui16Height;
		//pIspostUser->stDnInfo.stRefImgInfo.ui32YAddr = pIspostUser->ui32DnRefImgBufAddr;
		//pIspostUser->stDnInfo.stRefImgInfo.ui32UVAddr = pIspostUser->ui32DnRefImgBufAddr + ui32YSize;
		ui32Ret += IspostDnRefImgAddrSet(
			pIspostUser->stDnInfo.stRefImgInfo.ui32YAddr,
			pIspostUser->stDnInfo.stRefImgInfo.ui32UVAddr
			);
		//
		// Setup DNR output image addr.
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//ui32YSize = pIspostUser->stDnInfo.stOutImgInfo.ui16Stride * pIspostUser->stDnInfo.stOutImgInfo.ui16Height;
		//pIspostUser->stDnInfo.stOutImgInfo.ui32YAddr = pIspostUser->ui32DnOutImgBufAddr;
		//pIspostUser->stDnInfo.stOutImgInfo.ui32UVAddr = pIspostUser->ui32DnOutImgBufAddr + ui32YSize;
		ui32Ret += IspostDnOutImgAddrSet(
			pIspostUser->stDnInfo.stOutImgInfo.ui32YAddr,
			pIspostUser->stDnInfo.stOutImgInfo.ui32UVAddr
			);
		//
		// Setup DN mask curve.
		// ============================
		//
		ui32Ret += Ispost3DDenoiseMskCrvSetAll(
			pIspostUser->stDnInfo.stDnMskCrvInfo
			);

#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_DN) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			ISPOST_UINT32 ui32CrvIdx;
			ISPOST_UINT32 ui32Idx;

			for (ui32CrvIdx=0; ui32CrvIdx<DN_MSK_CRV_IDX_MAX; ui32CrvIdx++) {
				printk("@@@ IspostUserUpdate:stDnInfo.stDnMskCrvInfo { ");
				for (ui32Idx=0; ui32Idx<DN_MSK_CRV_DNMC_MAX; ui32Idx++)
					printk("0x%08X, ", pIspostUser->stDnInfo.stDnMskCrvInfo[ui32CrvIdx].stMskCrv[ui32Idx].ui32MskCrv);
				printk(" }\n");
			}
			printk("@@@ IspostUserUpdate:stDnInfo.stOutImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stDnInfo.stOutImgInfo.ui32YAddr);
			printk("@@@ IspostUserUpdate:stDnInfo.stOutImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stDnInfo.stOutImgInfo.ui32UVAddr);
			//printk("@@@ IspostUserUpdate:stDnInfo.stOutImgInfo.ui16Stride: %d\n", pIspostUser->stDnInfo.stOutImgInfo.ui16Stride);
			printk("@@@ IspostUserUpdate:stDnInfo.stRefImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stDnInfo.stRefImgInfo.ui32YAddr);
			printk("@@@ IspostUserUpdate:stDnInfo.stRefImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stDnInfo.stRefImgInfo.ui32UVAddr);
			//printk("@@@ IspostUserUpdate:stDnInfo.stRefImgInfo.ui16Stride: %d\n", pIspostUser->stDnInfo.stRefImgInfo.ui16Stride);
			printk("@@@ IspostUserUpdate:stDnInfo.ui32DnTargetIdx: %d\n", pIspostUser->stDnInfo.ui32DnTargetIdx);
			//printk("@@@ IspostUserUpdate:stDnInfo.stAxiId.ui8RefWrID: %d\n", pIspostUser->stDnInfo.stAxiId.ui8RefWrID);
			//printk("@@@ IspostUserUpdate:stDnInfo.stAxiId.ui8RefRdID: %d\n", pIspostUser->stDnInfo.stAxiId.ui8RefRdID);
			printk("@@@ IspostUserUpdate:stDnInfo ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_DN;
	}
	// ==================================================================

	// ==================================================================
	// Setup Un-Scaled output information.
	if (pIspostUser->stCtrl0.ui1EnUO)
	{
		//
		// Setup Un-Scaled output image information.
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//ui32YSize = pIspostUser->stUoInfo.stOutImgInfo.ui16Stride * pIspostUser->stUoInfo.stOutImgInfo.ui16Height;
		//pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr = pIspostUser->ui32UnScaleOutImgBufAddr;
		//pIspostUser->stUoInfo.stOutImgInfo.ui32UVAddr = pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr + ui32YSize;
		ui32Ret += IspostUnSOutImgAddrSet(
			pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr,
			pIspostUser->stUoInfo.stOutImgInfo.ui32UVAddr
			);
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_UO) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep))) {
			printk("@@@ IspostUserUpdate:stUoInfo.stOutImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr);
			printk("@@@ IspostUserUpdate:stUoInfo.stOutImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stUoInfo.stOutImgInfo.ui32UVAddr);
			printk("@@@ IspostUserUpdate:stUoInfo ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_UO;
	}
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 0 information.
	if (pIspostUser->stCtrl0.ui1EnSS0)
	{
		//
		// Setup Scaling stream 0 output image information.
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//ui32YSize = pIspostUser->stSS0Info.stOutImgInfo.ui16Stride * pIspostUser->stSS0Info.stOutImgInfo.ui16Height;
		//pIspostUser->stSS0Info.stOutImgInfo.ui32YAddr = pIspostUser->ui32SS0OutImgBufAddr;
		//pIspostUser->stSS0Info.stOutImgInfo.ui32UVAddr = pIspostUser->ui32SS0OutImgBufAddr + ui32YSize;
		ui32Ret += IspostSSOutImgAddrSet(
			SCALING_STREAM_0,
			pIspostUser->stSS0Info.stOutImgInfo.ui32YAddr,
			pIspostUser->stSS0Info.stOutImgInfo.ui32UVAddr
			);
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_SS0) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep))) {
			printk("@@@ IspostUserUpdate:stSS0Info.stOutImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stSS0Info.stOutImgInfo.ui32YAddr);
			printk("@@@ IspostUserUpdate:stSS0Info.stOutImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stSS0Info.stOutImgInfo.ui32UVAddr);
			printk("@@@ IspostUserUpdate:stSS0Info ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_SS0;
	}
	// ==================================================================

	// ==================================================================
	// Setup scaling stream 1 information.
	if (pIspostUser->stCtrl0.ui1EnSS1)
	{
		//
		// Setup Scaling stream 1 output image information.
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//ui32YSize = pIspostUser->stSS1Info.stOutImgInfo.ui16Stride * pIspostUser->stSS1Info.stOutImgInfo.ui16Height;
		//pIspostUser->stSS1Info.stOutImgInfo.ui32YAddr = pIspostUser->ui32SS1OutImgBufAddr;
		//pIspostUser->stSS1Info.stOutImgInfo.ui32UVAddr = pIspostUser->ui32SS1OutImgBufAddr + ui32YSize;
		ui32Ret += IspostSSOutImgAddrSet(
			SCALING_STREAM_1,
			pIspostUser->stSS1Info.stOutImgInfo.ui32YAddr,
			pIspostUser->stSS1Info.stOutImgInfo.ui32UVAddr
			);
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_SS1) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			printk("@@@ IspostUserUpdate:stSS1Info.stOutImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stSS1Info.stOutImgInfo.ui32YAddr);
			printk("@@@ IspostUserUpdate:stSS1Info.stOutImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stSS1Info.stOutImgInfo.ui32UVAddr);
			printk("@@@ IspostUserUpdate:stSS1Info ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_SS1;
	}
	// ==================================================================

	// ==================================================================
	if (pIspostUser->stCtrl0.ui1EnVencL)
	{
		//
		// Setup video link start of frame address.
		// ============================
		//
		// Following mark parameters has been program in call function.
		//-------------------------------------------------------------
		//pIspostUser->stVLinkInfo.ui32StartAddr = pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr;
		ui32Ret += IspostVLinkStartAddrSet(pIspostUser->stVLinkInfo.ui32StartAddr);
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_VENC_L) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep))) {
			printk("@@@ IspostUserUpdate:stVLinkInfo.ui32StartAddr: 0x%X\n", pIspostUser->stVLinkInfo.ui32StartAddr);
			printk("@@@ IspostUserUpdate:stVLinkInfo ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_VENC_L;
	}
	// ==================================================================

	// ==================================================================
	if ((0 == pIspostUser->ui32IspostCount) && (pIspostUser->stCtrl0.ui1EnDN))
	{
		ui32Ctrl = ((pIspostUser->stCtrl0.ui32Ctrl0 | (ISPOST_ISPCTRL0_ENDNWR_MASK)) & ~(ISPOST_ISPCTRL0_ENDN_MASK) & ~(ISPOST_ISPCTRL0_ENVENCL_MASK));
	}
	else
	{
		ui32Ctrl = pIspostUser->stCtrl0.ui32Ctrl0 & ~(ISPOST_ISPCTRL0_ENVENCL_MASK);
	}
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_CTRL) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
	{
		printk("@@@ IspostUserUpdate:stCtrl0.ui32Ctrl0: = 0x%X\n", ui32Ctrl);
	}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
	if ((0 == pIspostUser->ui32IspostCount) && ((pIspostUser->stCtrl0.ui1EnIspL) || (pIspostUser->stCtrl0.ui1EnVencL)))
	{
		// Reset ispost module, ISP link and vlink.
		if ((pIspostUser->stCtrl0.ui1EnIspL) && (pIspostUser->stCtrl0.ui1EnVencL))
		{
			IspostAndIspLinkAndVLinkResetAndSetCtrl((ISPOST_CTRL0)ui32Ctrl);
		}
		else if (pIspostUser->stCtrl0.ui1EnIspL)
		{
			IspostAndIspLinkResetAndSetCtrl((ISPOST_CTRL0)ui32Ctrl);
		}
		else if (pIspostUser->stCtrl0.ui1EnVencL)
		{
			IspostAndVLinkResetAndSetCtrl((ISPOST_CTRL0)ui32Ctrl);
		}
	}
	else
	{
		// Reset, enable (do not reset ISP link).
		IspostResetAndSetCtrl((ISPOST_CTRL0)ui32Ctrl);
	}
	// ==================================================================

	// ==================================================================
	if (pIspostUser->stCtrl0.ui1EnIspL)
	{
		// set second frame ISP link frame start address (will be latched when first address matches - do it before enable)
		pIspostUser->stIspLinkInfo.ui32NextUVAddrMSW = pIspostUser->ui32NextLcSrcImgBufUvAddrMSW;
		if (REODER_MODE_BURST == pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdMode)
		{
			pIspostUser->stIspLinkInfo.ui32NextUVAddrLSW = ((pIspostUser->ui32NextLcSrcImgBufAddr +
				(pIspostUser->stLcInfo.stSrcImgInfo.ui16Stride * pIspostUser->stLcInfo.stSrcImgInfo.ui16Height)) & 0x7fffffff);
		}
		else
		{
			pIspostUser->stIspLinkInfo.ui32NextUVAddrLSW = (pIspostUser->ui32NextLcSrcImgBufUvAddr & 0x7fffffff);
		}
		ui32Ret += IspostIspLinkNextStartAddrSet(pIspostUser->stIspLinkInfo);
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_ISP_L) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
		{
			printk("@@@ IspostUserUpdate:stIspLinkInfo.ui32NextUVAddrMSW: 0x%X\n", pIspostUser->stIspLinkInfo.ui32NextUVAddrMSW);
			printk("@@@ IspostUserUpdate:stIspLinkInfo.ui32NextUVAddrLSW: 0x%X\n", pIspostUser->stIspLinkInfo.ui32NextUVAddrLSW);
			printk("@@@ IspostUserUpdate:stIspLinkInfo ui32Ret: %d\n", ui32Ret);
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
		pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_ISP_L;
	}
	// ==================================================================

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}


ISPOST_RESULT IspostUserTrigger(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ctrl;

	// Enable ISPOST module and ISP link process.
	if ((0 == pIspostUser->ui32IspostCount) && (pIspostUser->stCtrl0.ui1EnDN))
	{
		ui32Ctrl = ((pIspostUser->stCtrl0.ui32Ctrl0 | (ISPOST_ISPCTRL0_ENDNWR_MASK)) & ~(ISPOST_ISPCTRL0_ENDN_MASK));
	}
	else
	{
		ui32Ctrl = pIspostUser->stCtrl0.ui32Ctrl0;
	}
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_CTRL) && (0 == (pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep)))
	{
		printk("@@@ IspostUserTrigger:stCtrl0.ui32Ctrl0: = 0x%X\n", ui32Ctrl);
	}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
	IspostTrigger((ISPOST_CTRL0)ui32Ctrl);

	return  ISPOST_TRUE;
}

ISPOST_RESULT IspostUserCompleteCheck(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	LC_LB_ERR stLcLbErr;

	if (pIspostUser->stCtrl0.ui1EnIspL)
	{
		// Test if lb error occur.
		ui32Ret += IspostLbErrIntChk(ISPOST_OFF);

		// Read out error register.
		// Read until error fifo empty
		do
		{
			stLcLbErr = IspostLcLbErrGet();
			if (!stLcLbErr.ui1Empty)
			{
#if 0
				ISPOST_LOG("ISPOST REG RD waiting for reg 0x%08X = 0x%08X, expected value: 0x%08X\n", ISPOST_LCLBERR, stLcLbErr.ui32Error, 0x80000000);
#else
  #if defined(IMG_KERNEL_MODULE)
				printk("LB ErrType = %s, Y0 = %d, X0 = %d\n", g_chLbError[stLcLbErr.ui3ErrorType], stLcLbErr.ui11y, stLcLbErr.ui12X);
  #else
				ISPOST_LOG("LB ErrType = %s, Y0 = %d, X0 = %d\n", g_chLbError[stLcLbErr.ui3ErrorType], stLcLbErr.ui11y, stLcLbErr.ui12X);
  #endif
#endif
			}
		} while (!stLcLbErr.ui1Empty);

		// Clear LclvInt flag.
		//IspostLcLbIntClear();

		// Disable (do not disable ISP link).
		IspostDisable(pIspostUser->stCtrl0);
	}
	else
	{
		if (pIspostUser->stCtrl0.ui1EnILc && pIspostUser->stCtrl0.ui1EnVS && pIspostUser->stCtrl0.ui1EnVSR)
		{
#if defined(ISPOST_REG_DEBUG)
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
			IspostInterruptVsVsfwIntClear();
		}
		else if (pIspostUser->stCtrl0.ui1EnILc && pIspostUser->stCtrl0.ui1EnVSR)
		{
#if defined(ISPOST_REG_DEBUG)
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
			IspostInterruptVsfwIntClear();
		}
		else if (pIspostUser->stCtrl0.ui1EnVS)
		{
#if defined(ISPOST_REG_DEBUG)
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
			IspostInterruptVsIntClear();
		}
		else
		{
#if defined(ISPOST_REG_DEBUG)
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPCTRL0, ISPOST_READL(ISPOST_ISPCTRL0));
			ISPOST_LOG("0x%08X = 0x%08x\n", ISPOST_ISPSTAT0, ISPOST_READL(ISPOST_ISPSTAT0));
#endif
			IspostInterruptClear();
		}
	}

	return (ui32Ret);
}

ISPOST_RESULT IspostUserSetup(ISPOST_USER* pIspostUser)
{
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_CTRL) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
	{
		printk("@@@ ===================================================================\n");
		printk("@@@ LC(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnLC, pIspostUser->stLcInfo.stSrcImgInfo.ui16Width, pIspostUser->stLcInfo.stSrcImgInfo.ui16Height);
		printk("@@@ VS(%d), W(%d) x H(%d)\n", (pIspostUser->stCtrl0.ui1EnVSR | pIspostUser->stCtrl0.ui1EnVSRC), pIspostUser->stVsInfo.stSrcImgInfo.ui16Width, pIspostUser->stVsInfo.stSrcImgInfo.ui16Height);
		printk("@@@ OV0(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnOV, pIspostUser->stOvInfo.stOvImgInfo[0].ui16Width, pIspostUser->stOvInfo.stOvImgInfo[0].ui16Height);
		printk("@@@ DN(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnDN, pIspostUser->stLcInfo.stOutImgInfo.ui16Width, pIspostUser->stLcInfo.stOutImgInfo.ui16Height);
		printk("@@@ UO(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnUO, pIspostUser->stLcInfo.stOutImgInfo.ui16Width, pIspostUser->stLcInfo.stOutImgInfo.ui16Height);
		printk("@@@ SS0(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnSS0, pIspostUser->stSS0Info.stOutImgInfo.ui16Width, pIspostUser->stSS0Info.stOutImgInfo.ui16Height);
		printk("@@@ SS1(%d), W(%d) x H(%d)\n", pIspostUser->stCtrl0.ui1EnSS1, pIspostUser->stSS1Info.stOutImgInfo.ui16Width, pIspostUser->stSS1Info.stOutImgInfo.ui16Height);
		printk("@@@ stCtrl0 = 0x%08X\n", pIspostUser->stCtrl0.ui32Ctrl0);
	}
#endif //INFOTM_ENABLE_ISPOST_DEBUG

	if (IspostUserSetupLC(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;
	if (IspostUserSetupVS(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;
	if (IspostUserSetupOV(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;
	if (IspostUserSetupDN(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;
	if (IspostUserSetupUO(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;
	if (IspostUserSetupSS0(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;
	if (IspostUserSetupSS1(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;
	if (IspostUserSetupVL(pIspostUser) != ISPOST_TRUE)
		return ISPOST_FALSE;

#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_CTRL) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
	{
		printk("@@@ IspostUserSetup() success !\n");
	}
#endif //INFOTM_ENABLE_ISPOST_DEBUG

return ISPOST_TRUE;
}

ISPOST_RESULT IspostUserSetupLC(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32GridFileSize = 0, ui32Stride = 0, ui32Length = 0;

	// ******************************************************************
	// Setup lens correction information.
	// ******************************************************************

	if (pIspostUser->stCtrl0.ui1EnLC)
	{
		// ==================================================================
		// Set LB geometry index and data.
		// ==================================================================
		if (pIspostUser->stCtrl0.ui1EnIspL)
		{
			// LB geometry index.
			ui32Ret += IspostLcLbLineGeometryIdxSet(ISPOST_ENABLE, 0);

			// LB geometry data.
			if (REODER_MODE_BURST == pIspostUser->stIspLinkInfo.stCtrl1.ui1ReOrdMode)
			{
				ui32Ret += IspostLcLbLineGeometryWithFixDataSet(
					0x0000,
					pIspostUser->stLcInfo.stSrcImgInfo.ui16Width + 7,
					pIspostUser->stLcInfo.stSrcImgInfo.ui16Height + 4
					); // begin = 0, end = (isp_w + 7), lines = (isp_h + 4).
			}
			else
			{
				ui32Ret += IspostLcLbLineGeometryWithFixDataSet(
					0x0000,
					(((pIspostUser->stLcInfo.stSrcImgInfo.ui16Width + 64 -1) & 0xffffc0) + 7),
					pIspostUser->stLcInfo.stSrcImgInfo.ui16Height + 4
					); // begin = 0, end = (((isp_w + 64 - 1) & 0xffffc0) + 7), lines = (isp_h + 4).
			}
		}
		else
		{
			if (pIspostUser->stLcInfo.stGridBufInfo.ui8LineBufEnable) {
#if defined(IMG_KERNEL_MODULE)
				pIspostUser->stLcInfo.stGridBufInfo.ui32GeoBufAddr =
					(ISPOST_UINT32)phys_to_virt(pIspostUser->stLcInfo.stGridBufInfo.ui32GeoBufAddr);
#endif
				ui32Ret += IspostLcLbLineGeometryFrom0Set(
					pIspostUser->stCtrl0.ui1EnIspL,
					(ISPOST_PUINT16)(pIspostUser->stLcInfo.stGridBufInfo.ui32GeoBufAddr),
					pIspostUser->stLcInfo.stSrcImgInfo.ui16Height
					);
			}
		}

		// ==================================================================
		// Setup LC grid buffer address, stride and grid size
		// ==================================================================
		ui32Stride = IspostLcGridBufStrideCalc(pIspostUser->stLcInfo.stOutImgInfo.ui16Width, pIspostUser->stLcInfo.stGridBufInfo.ui8Size);
		ui32Length = IspostLcGridBufLengthCalc(pIspostUser->stLcInfo.stOutImgInfo.ui16Height, ui32Stride, pIspostUser->stLcInfo.stGridBufInfo.ui8Size);
		ui32GridFileSize = (ui32Length + 63) & 0xFFFFFFC0;
		if (pIspostUser->stCtrl0.ui1EnLC)
		{
			pIspostUser->stLcInfo.stGridBufInfo.ui32BufAddr += pIspostUser->stLcInfo.stGridBufInfo.ui32GridTargetIdx * ui32GridFileSize;//34176; 34160 is 32x32 GridData one file size, alignment 64 is 34176
			pIspostUser->stLcInfo.stGridBufInfo.ui16Stride = ui32Stride;
		}
		else
		{
			pIspostUser->stLcInfo.stGridBufInfo.ui32BufAddr = 0;
			pIspostUser->stLcInfo.stGridBufInfo.ui16Stride = 0;
		}

		ui32Ret += IspostLcGridBufSetWithStride(
						pIspostUser->stLcInfo.stGridBufInfo.ui32BufAddr,
						pIspostUser->stLcInfo.stGridBufInfo.ui16Stride,
						pIspostUser->stLcInfo.stGridBufInfo.ui8LineBufEnable,
						pIspostUser->stLcInfo.stGridBufInfo.ui8Size
						);

	} //end of if (pIspostUser->stCtrl0.ui1EnLC)

	// ==================================================================
	// Setup LC SRC addr, stride and size.
	// ==================================================================
	ui32Ret += IspostLcSrcImgSet(
					pIspostUser->stLcInfo.stSrcImgInfo.ui32YAddr,
					pIspostUser->stLcInfo.stSrcImgInfo.ui32UVAddr,
					pIspostUser->stLcInfo.stSrcImgInfo.ui16Stride,
					pIspostUser->stLcInfo.stSrcImgInfo.ui16Width,
					pIspostUser->stLcInfo.stSrcImgInfo.ui16Height
					);

	// ==================================================================
	// Setup LC back fill color Y, CB and CR value.
	// ==================================================================
	ui32Ret += IspostLcBackFillColorSet(
					pIspostUser->stLcInfo.stBackFillColor.ui8Y,
					pIspostUser->stLcInfo.stBackFillColor.ui8CB,
					pIspostUser->stLcInfo.stBackFillColor.ui8CR
					);

	// ==================================================================
	// Setup LC pixel cache mode
	// ==================================================================
	ui32Ret += IspostLcPxlCacheModeSet(pIspostUser->stLcInfo.stPxlCacheMode);

	// ==================================================================
	// Setup LC pixel coordinator generator mode.
	// ==================================================================
	ui32Ret += IspostLcPxlScanModeSet(
					pIspostUser->stLcInfo.stPxlScanMode.ui8ScanW,
					pIspostUser->stLcInfo.stPxlScanMode.ui8ScanH,
					pIspostUser->stLcInfo.stPxlScanMode.ui8ScanM
					);

	// ==================================================================
	// Setup LC pixel coordinator destination size.
	// ==================================================================
	ui32Ret += IspostLcPxlDstSizeSet(
					pIspostUser->stLcInfo.stOutImgInfo.ui16Width,
					pIspostUser->stLcInfo.stOutImgInfo.ui16Height
					);

	// ==================================================================
	// Setup LC AXI Read Request Limit, GBID and SRCID.
	// ==================================================================
	if (!pIspostUser->stCtrl0.ui1EnLC && pIspostUser->stCtrl0.ui1EnDN) //Denoise only
		pIspostUser->stLcInfo.stAxiId.ui8SrcReqLmt = 96;
	else
		pIspostUser->stLcInfo.stAxiId.ui8SrcReqLmt = 4; //48 //56
	pIspostUser->stLcInfo.stAxiId.ui8GBID = 1;
	pIspostUser->stLcInfo.stAxiId.ui8SRCID = 0;

	ui32Ret += IspostLcAxiIdSet(
					pIspostUser->stLcInfo.stAxiId.ui8SrcReqLmt,
					pIspostUser->stLcInfo.stAxiId.ui8GBID,
					pIspostUser->stLcInfo.stAxiId.ui8SRCID
					);

#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_LC) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
	{
		printk("@@@ stLcInfo.stSrcImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stLcInfo.stSrcImgInfo.ui32YAddr);
		printk("@@@ stLcInfo.stSrcImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stLcInfo.stSrcImgInfo.ui32UVAddr);
		printk("@@@ stLcInfo.stSrcImgInfo.ui16Width: %d\n", pIspostUser->stLcInfo.stSrcImgInfo.ui16Width);
		printk("@@@ stLcInfo.stSrcImgInfo.ui16Height: %d\n", pIspostUser->stLcInfo.stSrcImgInfo.ui16Height);
		printk("@@@ stLcInfo.stSrcImgInfo.ui16Stride: %d\n", pIspostUser->stLcInfo.stSrcImgInfo.ui16Stride);
		printk("@@@ stLcInfo.stBackFillColor.ui8Y: %d\n", pIspostUser->stLcInfo.stBackFillColor.ui8Y);
		printk("@@@ stLcInfo.stBackFillColor.ui8CB: %d\n", pIspostUser->stLcInfo.stBackFillColor.ui8CB);
		printk("@@@ stLcInfo.stBackFillColor.ui8CR: %d\n", pIspostUser->stLcInfo.stBackFillColor.ui8CR);
		printk("@@@ stLcInfo.stGridBufInfo.ui32BufAddr: 0x%X\n", pIspostUser->stLcInfo.stGridBufInfo.ui32BufAddr);
		printk("@@@ stLcInfo.stGridBufInfo.ui32BufLen: %d\n", pIspostUser->stLcInfo.stGridBufInfo.ui32BufLen);
		printk("@@@ stLcInfo.stGridBufInfo.ui16Stride: %d\n", pIspostUser->stLcInfo.stGridBufInfo.ui16Stride);
		printk("@@@ stLcInfo.stGridBufInfo.ui8LineBufEnable: %d\n", pIspostUser->stLcInfo.stGridBufInfo.ui8LineBufEnable);
		printk("@@@ stLcInfo.stGridBufInfo.ui8Size: %d\n", pIspostUser->stLcInfo.stGridBufInfo.ui8Size);
		printk("@@@ stLcInfo.stGridBufInfo.ui32GeoBufAddr: 0x%X\n", pIspostUser->stLcInfo.stGridBufInfo.ui32GeoBufAddr);
		printk("@@@ stLcInfo.stGridBufInfo.ui32GeoBufLen: %d\n", pIspostUser->stLcInfo.stGridBufInfo.ui32GeoBufLen);
		printk("@@@ stLcInfo.stGridBufInfo.ui32GridTargetIdx: %d\n", pIspostUser->stLcInfo.stGridBufInfo.ui32GridTargetIdx);
		printk("@@@ stLcInfo.stPxlCacheMode.ui32PxlCacheMode: %d\n", pIspostUser->stLcInfo.stPxlCacheMode.ui32PxlCacheMode);
		printk("@@@ stLcInfo.stPxlScanMode.ui8ScanW: %d\n", pIspostUser->stLcInfo.stPxlScanMode.ui8ScanW);
		printk("@@@ stLcInfo.stPxlScanMode.ui8ScanH: %d\n", pIspostUser->stLcInfo.stPxlScanMode.ui8ScanH);
		printk("@@@ stLcInfo.stPxlScanMode.ui8ScanM: %d\n", pIspostUser->stLcInfo.stPxlScanMode.ui8ScanM);
		printk("@@@ stLcInfo.stOutImgInfo.ui16Width: %d\n", pIspostUser->stLcInfo.stOutImgInfo.ui16Width);
		printk("@@@ stLcInfo.stOutImgInfo.ui16Height: %d\n", pIspostUser->stLcInfo.stOutImgInfo.ui16Height);
		printk("@@@ stLcInfo.stAxiId.ui8SrcReqLmt: %d\n", pIspostUser->stLcInfo.stAxiId.ui8SrcReqLmt);
		printk("@@@ stLcInfo.stAxiId.ui8GBID: %d\n", pIspostUser->stLcInfo.stAxiId.ui8GBID);
		printk("@@@ stLcInfo.stAxiId.ui8SRCID: %d\n", pIspostUser->stLcInfo.stAxiId.ui8SRCID);
		printk("@@@ stLcInfo ui32Ret: %d\n", ui32Ret);
	}
#endif //INFOTM_ENABLE_ISPOST_DEBUG

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_LC;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserSetupVS(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	//ISPOST_UINT32 ui32Stride = 0, ui32Length = 0;

	// ******************************************************************
	// Setup video stabilization information.
	// ******************************************************************
	if (pIspostUser->stCtrl0.ui1EnVSR || pIspostUser->stCtrl0.ui1EnVSRC)
	{
		// ==================================================================
		if ((pIspostUser->stCtrl0.ui1EnDN || pIspostUser->stCtrl0.ui1EnUO ||
			pIspostUser->stCtrl0.ui1EnSS0 || pIspostUser->stCtrl0.ui1EnSS1) &&
			(ISPOST_FALSE == pIspostUser->stCtrl0.ui1EnLC) &&
			(ISPOST_FALSE == pIspostUser->stCtrl0.ui1EnIspL)) {
			// ==================================================================
			// Fill Video Stabilization Registration roll, H and V offset.
			// ==================================================================
			pIspostUser->stVsInfo.stRegOffset.ui16Roll = 0;
			pIspostUser->stVsInfo.stRegOffset.ui16HOff = 0;
			pIspostUser->stVsInfo.stRegOffset.ui8VOff = 0;

			// ==================================================================
			// Fill Video Stabilization Registration AXI ID.
			// ==================================================================
			pIspostUser->stVsInfo.stAxiId.ui8FWID = 0;
			pIspostUser->stVsInfo.stAxiId.ui8RDID = 2;

			// Fill Video Stabilization Registration flip/rot/rolling mode.
			pIspostUser->stVsInfo.stCtrl0.ui32Ctrl = 0;
			//pIspostUser->stVsInfo.stCtrl0.ui1FlipH = ISPOST_DISABLE;
			//pIspostUser->stVsInfo.stCtrl0.ui1FlipV = ISPOST_DISABLE;
			//pIspostUser->stVsInfo.stCtrl0.ui2Rot = ROT_MODE_NONE;
			//pIspostUser->stVsInfo.stCtrl0.ui1Ren = ISPOST_DISABLE;

			// ==================================================================
			// Setup video stabilization information for denoise module.
			// ==================================================================
			ui32Ret += IspostVideoStabilizationSetForDenoiseOnly(pIspostUser->stVsInfo);
		} else {
			//ui32Ret += IspostVideoStabilizationSet(pstIspostInfo->stVsInfo);
		}
	} //end of if (pIspostUser->stCtrl0.ui1EnVSR || pIspostUser->stCtrl0.ui1EnVSRC)

#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_LC) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
	{
		printk("@@@ stVsInfo.stSrcImgInfo.ui32YAddr: 0x%08X\n", pIspostUser->stVsInfo.stSrcImgInfo.ui32YAddr);
		printk("@@@ stVsInfo.stSrcImgInfo.ui32UVAddr: 0x%08X\n", pIspostUser->stVsInfo.stSrcImgInfo.ui32UVAddr);
		printk("@@@ stVsInfo.stSrcImgInfo.ui16Width: %d\n", pIspostUser->stVsInfo.stSrcImgInfo.ui16Width);
		printk("@@@ stVsInfo.stSrcImgInfo.ui16Height: %d\n", pIspostUser->stVsInfo.stSrcImgInfo.ui16Height);
		printk("@@@ stVsInfo.stSrcImgInfo.ui16Stride: %d\n", pIspostUser->stVsInfo.stSrcImgInfo.ui16Stride);
		printk("@@@ stVsInfo.stRegOffset.ui16Roll: %d\n", pIspostUser->stVsInfo.stRegOffset.ui16Roll);
		printk("@@@ stVsInfo.stRegOffset.ui16HOff: %d\n", pIspostUser->stVsInfo.stRegOffset.ui16HOff);
		printk("@@@ stVsInfo.stRegOffset.ui8VOff: %d\n", pIspostUser->stVsInfo.stRegOffset.ui8VOff);
		printk("@@@ stVsInfo.stAxiId.ui8FWID: %d\n", pIspostUser->stVsInfo.stAxiId.ui8FWID);
		printk("@@@ stVsInfo.stAxiId.ui8RDID: %d\n", pIspostUser->stVsInfo.stAxiId.ui8RDID);
		printk("@@@ stVsInfo.stCtrl0.ui32Ctrl: 0x%08X\n", pIspostUser->stVsInfo.stCtrl0.ui32Ctrl);
		printk("@@@ stVsInfo ui32Ret: %d\n", ui32Ret);
	}
#endif //INFOTM_ENABLE_ISPOST_DEBUG

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_VS;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserSetupOV(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT8 ui8Idx;
	ISPOST_UINT8 uiChannel;

	// ******************************************************************
	// Setup OV overlay image information.
	// ******************************************************************

	if (pIspostUser->stCtrl0.ui1EnOV)
	{
		// ==================================================================
		// Setup OV Set overlay mode.
		// ==================================================================
		#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_OV) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
		{
			printk("@@@ stOvInfo.ui8OverlayMode: 0x%X\n", pIspostUser->stOvInfo.stOverlayMode.ui32OvMode);
		}
		#endif //INFOTM_ENABLE_ISPOST_DEBUG
		ui32Ret += IspostOvModeSet(pIspostUser->stOvInfo.stOverlayMode.ui32OvMode);

		uiChannel = (ISPOST_UINT8)((pIspostUser->stOvInfo.stOverlayMode.ui32OvMode >> 8) & 0xff);
		for (ui8Idx = 0; ui8Idx < OVERLAY_CHNL_MAX; ui8Idx++, uiChannel >>= 1)
		{
			if (!(uiChannel & 0x01)) /* if channel is disable then continue */
				continue;
			// ==================================================================
			// Setup overlay index.
			// ==================================================================
			//stOvInfo.ui8OvIdx[0] = ui8Idx;
			//ui32Ret += IspostOvIdxSet(stOvInfo.ui8OvIdx[0]);
			ui32Ret += IspostOvIdxSet(ui8Idx);

			// ==================================================================
			// Setup OV overlay image information.
			// ==================================================================
			if (0 == ui8Idx)
			{
				ui32Ret += IspostOvImgSet(
								pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr,
								pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui32UVAddr,
								pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui16Stride,
								pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui16Width,
								pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui16Height
								);
			}
			else
			{
				ui32Ret += IspostOv2BitImgSet(
								pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr,
								pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui16Stride,
								pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui16Width,
								pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui16Height
								);
			}

			// ==================================================================
			// Setup OV overlay offset.
			// ==================================================================
			ui32Ret += IspostOvOffsetSet(
							pIspostUser->stOvInfo.stOvOffset[ui8Idx].ui16X,
							pIspostUser->stOvInfo.stOvOffset[ui8Idx].ui16Y
							);

			// ==================================================================
			// Setup OV overlay alpha value.
			// ==================================================================
			ui32Ret += IspostOvAlphaValueSet(
							pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue0,
							pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue1,
							pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue2,
							pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue3
							);

			// ==================================================================
			// Setup OV AXI read ID.
			// ==================================================================
			pIspostUser->stOvInfo.stAxiId[ui8Idx].ui8OVID = 8;
			ui32Ret += IspostOvAxiIdSet(
							pIspostUser->stOvInfo.stAxiId[ui8Idx].ui8OVID
							);

			#ifdef INFOTM_ENABLE_ISPOST_DEBUG
			if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_OV) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
			{
				printk("@@@ stOvInfo.ui8OvIdx[0]: %d\n", ui8Idx);
				printk("@@@ stOvInfo.stOvImgInfo[%d].ui32YAddr: 0x%X\n", ui8Idx, pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr);
				printk("@@@ stOvInfo.stOvImgInfo[%d].ui32UVAddr: 0x%X\n", ui8Idx, pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui32UVAddr);
				printk("@@@ stOvInfo.stOvImgInfo[%d].ui16Stride: %d\n", ui8Idx, pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui16Stride);
				printk("@@@ stOvInfo.stOvImgInfo[%d].ui16Width: %d\n", ui8Idx, pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui16Width);
				printk("@@@ stOvInfo.stOvImgInfo[%d].ui16Height: %d\n", ui8Idx, pIspostUser->stOvInfo.stOvImgInfo[ui8Idx].ui16Height);
				printk("@@@ stOvInfo.stOvOffset[%d].ui16X: %d\n", ui8Idx, pIspostUser->stOvInfo.stOvOffset[ui8Idx].ui16X);
				printk("@@@ stOvInfo.stOvOffset[%d].ui16Y: %d\n", ui8Idx, pIspostUser->stOvInfo.stOvOffset[ui8Idx].ui16Y);
				printk("@@@ stOvInfo.stOvAlpha[%d].stValue0: 0x%08X\n", ui8Idx, pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue0.ui32Alpha);
				printk("@@@ stOvInfo.stOvAlpha[%d].stValue1: 0x%08X\n", ui8Idx, pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue1.ui32Alpha);
				printk("@@@ stOvInfo.stOvAlpha[%d].stValue2: 0x%08X\n", ui8Idx, pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue2.ui32Alpha);
				printk("@@@ stOvInfo.stOvAlpha[%d].stValue3: 0x%08X\n", ui8Idx, pIspostUser->stOvInfo.stOvAlpha[ui8Idx].stValue3.ui32Alpha);
				printk("@@@ stOvInfo.stAxiId[%d].ui8OVID: %d\n", ui8Idx, pIspostUser->stOvInfo.stAxiId[ui8Idx].ui8OVID);
				printk("@@@ stOvInfo ui32Ret: %d\n", ui32Ret);
				printk("@@@ .........................................\n");
			}
			#endif //INFOTM_ENABLE_ISPOST_DEBUG
		} //end of for (ui8Idx = 0; ui8Idx < OVERLAY_CHNL_MAX; ui8Idx++)
	} //end of if (pIspostUser->stCtrl0.ui1EnOV)

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_OV;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserSetupDN(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 CrvIdx;

	// ******************************************************************
	// Setup 3D-denoise information.
	// ******************************************************************
	if (pIspostUser->stCtrl0.ui1EnDN)
	{
		// ==================================================================
		// Setup DNR reference input image addr, stride.
		// ==================================================================
		ui32Ret += IspostDnRefImgSet(
						pIspostUser->stDnInfo.stRefImgInfo.ui32YAddr,
						pIspostUser->stDnInfo.stRefImgInfo.ui32UVAddr,
						pIspostUser->stDnInfo.stRefImgInfo.ui16Stride
						);

		// ==================================================================
		// Setup denoise curve
		// ==================================================================
		for (CrvIdx = 0; CrvIdx < DN_MSK_CRV_IDX_MAX; CrvIdx++)
			ui32Ret += Ispost3DDenoiseMskCrvSet(pIspostUser->stDnInfo.stDnMskCrvInfo[CrvIdx]);
	}

	// ==================================================================
	// Setup DNR output image addr, stride.
	// ==================================================================
	ui32Ret += IspostDnOutImgSet(
					pIspostUser->stDnInfo.stOutImgInfo.ui32YAddr,
					pIspostUser->stDnInfo.stOutImgInfo.ui32UVAddr,
					pIspostUser->stDnInfo.stOutImgInfo.ui16Stride
					);

	// ==================================================================
	// Setup DN AXI reference write and read ID.
	// ==================================================================
	pIspostUser->stDnInfo.stAxiId.ui8RefWrID = 3;
	pIspostUser->stDnInfo.stAxiId.ui8RefRdID = 3;
	ui32Ret += IspostDnAxiIdSet(
					pIspostUser->stDnInfo.stAxiId.ui8RefWrID,
					pIspostUser->stDnInfo.stAxiId.ui8RefRdID
					);

#ifdef INFOTM_ENABLE_ISPOST_DEBUG
	if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_DN) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
	{
		ISPOST_UINT32 Idx;

		for (CrvIdx = 0; CrvIdx < DN_MSK_CRV_IDX_MAX; CrvIdx++)
		{
			printk("@@@ stDnInfo.stDnMskCrvInfo { ");
			for (Idx=0; Idx<DN_MSK_CRV_DNMC_MAX; Idx++)
				printk("0x%08X, ", pIspostUser->stDnInfo.stDnMskCrvInfo[CrvIdx].stMskCrv[Idx].ui32MskCrv);
			printk(" }\n");
		}
		printk("@@@ stDnInfo.stOutImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stDnInfo.stOutImgInfo.ui32YAddr);
		printk("@@@ stDnInfo.stOutImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stDnInfo.stOutImgInfo.ui32UVAddr);
		printk("@@@ stDnInfo.stOutImgInfo.ui16Stride: %d\n", pIspostUser->stDnInfo.stOutImgInfo.ui16Stride);
		printk("@@@ stDnInfo.stRefImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stDnInfo.stRefImgInfo.ui32YAddr);
		printk("@@@ stDnInfo.stRefImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stDnInfo.stRefImgInfo.ui32UVAddr);
		printk("@@@ stDnInfo.stRefImgInfo.ui16Stride: %d\n", pIspostUser->stDnInfo.stRefImgInfo.ui16Stride);
		printk("@@@ stDnInfo.ui32DnTargetIdx: %d\n", pIspostUser->stDnInfo.ui32DnTargetIdx);
		printk("@@@ stDnInfo.stAxiId.ui8RefWrID: %d\n", pIspostUser->stDnInfo.stAxiId.ui8RefWrID);
		printk("@@@ stDnInfo.stAxiId.ui8RefRdID: %d\n", pIspostUser->stDnInfo.stAxiId.ui8RefRdID);
		printk("@@@ stDnInfo ui32Ret: %d\n", ui32Ret);
	}
#endif //INFOTM_ENABLE_ISPOST_DEBUG

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_DN;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserSetupUO(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	// ******************************************************************
	// Setup Un-Scaled output information.
	// ******************************************************************

	if (pIspostUser->stCtrl0.ui1EnUO)
	{
		// ==================================================================
		// Setup Un-Scaled output image addr, stride and scan height.
		// ==================================================================
		ui32Ret += IspostUnSOutImgSet(
						pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr,
						pIspostUser->stUoInfo.stOutImgInfo.ui32UVAddr,
						pIspostUser->stUoInfo.stOutImgInfo.ui16Stride,
						pIspostUser->stUoInfo.ui8ScanH
						);

		// ==================================================================
		// Setup Un-Scaled AXI reference write ID.
		// ==================================================================
		pIspostUser->stUoInfo.stAxiId.ui8RefWrID = 4;
		ui32Ret += IspostUnSAxiIdSet(pIspostUser->stUoInfo.stAxiId.ui8RefWrID);

		#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_UO) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
		{
			printk("@@@ stUoInfo.stOutImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr);
			printk("@@@ stUoInfo.stOutImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stUoInfo.stOutImgInfo.ui32UVAddr);
			printk("@@@ stUoInfo.stOutImgInfo.ui16Stride: %d\n", pIspostUser->stUoInfo.stOutImgInfo.ui16Stride);
			printk("@@@ stUoInfo.ui8ScanH: %d\n", pIspostUser->stUoInfo.ui8ScanH);
			printk("@@@ stUoInfo.stAxiId.ui8RefWrID: %d\n", pIspostUser->stUoInfo.stAxiId.ui8RefWrID);
			printk("@@@ stUoInfo ui32Ret: %d\n", ui32Ret);
		}
		#endif //INFOTM_ENABLE_ISPOST_DEBUG
	} else {
		//uo disable need add
		ui32Ret += IspostUnsPxlScanModeSet(pIspostUser->stUoInfo.ui8ScanH);

		pIspostUser->stUoInfo.stAxiId.ui8RefWrID = 4;
		ui32Ret += IspostUnSAxiIdSet(pIspostUser->stUoInfo.stAxiId.ui8RefWrID);
	}

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_UO;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserSetupSS0(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32SrcWidth, ui32SrcHeight;

	// ******************************************************************
	// Setup scaling stream 0 output information.
	// ******************************************************************
	if (pIspostUser->stCtrl0.ui1EnSS0)
	{
		// ==================================================================
		// Fill SS0 output stream select.
		// ==================================================================
		pIspostUser->stSS0Info.ui8StreamSel = SCALING_STREAM_0;

		// ==================================================================
		// Fill SS0 output image information.
		// ==================================================================
		if (pIspostUser->stCtrl0.ui1EnVSR)
		{
			ui32SrcWidth = pIspostUser->stVsInfo.stSrcImgInfo.ui16Width;
			ui32SrcHeight = pIspostUser->stVsInfo.stSrcImgInfo.ui16Height;
		}
		else
		{
			ui32SrcWidth = pIspostUser->stLcInfo.stOutImgInfo.ui16Width;
			ui32SrcHeight = pIspostUser->stLcInfo.stOutImgInfo.ui16Height;
		}
		if (ui32SrcWidth == pIspostUser->stSS0Info.stOutImgInfo.ui16Width)
		{
			pIspostUser->stSS0Info.stHSF.ui16SF = 1;
			pIspostUser->stSS0Info.stHSF.ui8SM = SCALING_MODE_NO_SCALING;
		}
		else
		{
			pIspostUser->stSS0Info.stHSF.ui16SF = IspostScalingFactotCal(ui32SrcWidth, pIspostUser->stSS0Info.stOutImgInfo.ui16Width);
			pIspostUser->stSS0Info.stHSF.ui8SM = SCALING_MODE_SCALING_DOWN;
		}
		if (ui32SrcHeight == pIspostUser->stSS0Info.stOutImgInfo.ui16Height)
		{
			pIspostUser->stSS0Info.stVSF.ui16SF = 1;
			pIspostUser->stSS0Info.stVSF.ui8SM = SCALING_MODE_NO_SCALING;
		}
		else
		{
			pIspostUser->stSS0Info.stVSF.ui16SF = IspostScalingFactotCal(ui32SrcHeight, pIspostUser->stSS0Info.stOutImgInfo.ui16Height);
			pIspostUser->stSS0Info.stVSF.ui8SM = SCALING_MODE_SCALING_DOWN;
		}

		// Fill SS0 AXI SSWID.
		pIspostUser->stSS0Info.stAxiId.ui8SSWID = 1;
		#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_SS0) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
		{
			printk("@@@ stSS0Info ui32SrcWidth: %d\n", ui32SrcWidth);
			printk("@@@ stSS0Info ui32SrcHeight: %d\n", ui32SrcHeight);
			printk("@@@ stSS0Info.stOutImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stSS0Info.stOutImgInfo.ui32YAddr);
			printk("@@@ stSS0Info.stOutImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stSS0Info.stOutImgInfo.ui32UVAddr);
			printk("@@@ stSS0Info.stOutImgInfo.ui16Stride: %d\n", pIspostUser->stSS0Info.stOutImgInfo.ui16Stride);
			printk("@@@ stSS0Info.stOutImgInfo.ui16Width: %d\n", pIspostUser->stSS0Info.stOutImgInfo.ui16Width);
			printk("@@@ stSS0Info.stOutImgInfo.ui16Height: %d\n", pIspostUser->stSS0Info.stOutImgInfo.ui16Height);
			printk("@@@ stSS0Info.stHSF.ui16SF: %d\n", pIspostUser->stSS0Info.stHSF.ui16SF);
			printk("@@@ stSS0Info.stHSF.ui8SM: %d\n", pIspostUser->stSS0Info.stHSF.ui8SM);
			printk("@@@ stSS0Info.stVSF.ui16SF: %d\n", pIspostUser->stSS0Info.stVSF.ui16SF);
			printk("@@@ stSS0Info.stVSF.ui8SM: %d\n", pIspostUser->stSS0Info.stVSF.ui8SM);
			printk("@@@ stSS0Info.stAxiId.ui8SSWID: %d\n", pIspostUser->stSS0Info.stAxiId.ui8SSWID);
			printk("@@@ stSS0Info ui32Ret: %d\n", ui32Ret);
		}
		#endif //INFOTM_ENABLE_ISPOST_DEBUG
		ui32Ret += IspostScaleStreamSet(pIspostUser->stSS0Info);
	} //end of if (pIspostUser->stCtrl0.ui1EnSS0)

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_SS0;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserSetupSS1(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;
	ISPOST_UINT32 ui32SrcWidth, ui32SrcHeight;

	// ==================================================================
	// ==================================================================
	// Setup scaling stream 0 output information.
	// ==================================================================
	// ==================================================================
	if (pIspostUser->stCtrl0.ui1EnSS1)
	{
		// ==================================================================
		// Fill SS0 output stream select.
		// ==================================================================
		pIspostUser->stSS1Info.ui8StreamSel = SCALING_STREAM_1;

		// ==================================================================
		// Fill SS0 output image information.
		// ==================================================================
		if (pIspostUser->stCtrl0.ui1EnVSR)
		{
			ui32SrcWidth = pIspostUser->stVsInfo.stSrcImgInfo.ui16Width;
			ui32SrcHeight = pIspostUser->stVsInfo.stSrcImgInfo.ui16Height;
		}
		else
		{
			ui32SrcWidth = pIspostUser->stLcInfo.stOutImgInfo.ui16Width;
			ui32SrcHeight = pIspostUser->stLcInfo.stOutImgInfo.ui16Height;
		}
		if (ui32SrcWidth == pIspostUser->stSS0Info.stOutImgInfo.ui16Width)
		{
			pIspostUser->stSS1Info.stHSF.ui16SF = 1;
			pIspostUser->stSS1Info.stHSF.ui8SM = SCALING_MODE_NO_SCALING;
		}
		else
		{
			pIspostUser->stSS1Info.stHSF.ui16SF = IspostScalingFactotCal(ui32SrcWidth, pIspostUser->stSS1Info.stOutImgInfo.ui16Width);
			pIspostUser->stSS1Info.stHSF.ui8SM = SCALING_MODE_SCALING_DOWN;
		}
		if (ui32SrcHeight == pIspostUser->stSS1Info.stOutImgInfo.ui16Height)
		{
			pIspostUser->stSS1Info.stVSF.ui16SF = 1;
			pIspostUser->stSS1Info.stVSF.ui8SM = SCALING_MODE_NO_SCALING;
		}
		else
		{
			pIspostUser->stSS1Info.stVSF.ui16SF = IspostScalingFactotCal(ui32SrcHeight, pIspostUser->stSS1Info.stOutImgInfo.ui16Height);
			pIspostUser->stSS1Info.stVSF.ui8SM = SCALING_MODE_SCALING_DOWN;
		}

		// Fill SS0 AXI SSWID.
		pIspostUser->stSS1Info.stAxiId.ui8SSWID = 2;
		#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_SS1) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
		{
			printk("@@@ stSS1Info ui32SrcWidth: %d\n", ui32SrcWidth);
			printk("@@@ stSS1Info ui32SrcHeight: %d\n", ui32SrcHeight);
			printk("@@@ stSS1Info.stOutImgInfo.ui32YAddr: 0x%X\n", pIspostUser->stSS1Info.stOutImgInfo.ui32YAddr);
			printk("@@@ stSS1Info.stOutImgInfo.ui32UVAddr: 0x%X\n", pIspostUser->stSS1Info.stOutImgInfo.ui32UVAddr);
			printk("@@@ stSS1Info.stOutImgInfo.ui16Stride: %d\n", pIspostUser->stSS1Info.stOutImgInfo.ui16Stride);
			printk("@@@ stSS1Info.stOutImgInfo.ui16Width: %d\n", pIspostUser->stSS1Info.stOutImgInfo.ui16Width);
			printk("@@@ stSS1Info.stOutImgInfo.ui16Height: %d\n", pIspostUser->stSS1Info.stOutImgInfo.ui16Height);
			printk("@@@ stSS1Info.stHSF.ui16SF: %d\n", pIspostUser->stSS1Info.stHSF.ui16SF);
			printk("@@@ stSS1Info.stHSF.ui8SM: %d\n", pIspostUser->stSS1Info.stHSF.ui8SM);
			printk("@@@ stSS1Info.stVSF.ui16SF: %d\n", pIspostUser->stSS1Info.stVSF.ui16SF);
			printk("@@@ stSS1Info.stVSF.ui8SM: %d\n", pIspostUser->stSS1Info.stVSF.ui8SM);
			printk("@@@ stSS1Info.stAxiId.ui8SSWID: %d\n", pIspostUser->stSS1Info.stAxiId.ui8SSWID);
			printk("@@@ stSS1Info ui32Ret: %d\n", ui32Ret);
		}
		#endif //INFOTM_ENABLE_ISPOST_DEBUG
		ui32Ret += IspostScaleStreamSet(pIspostUser->stSS1Info);
	} //end of if (pIspostUser->stCtrl0.ui1EnSS1)

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_SS1;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}


ISPOST_RESULT IspostUserSetupVL(ISPOST_USER* pIspostUser)
{
	ISPOST_UINT32 ui32Ret = 0;

	if (pIspostUser->stCtrl0.ui1EnVencL)
	{
		if (0 == pIspostUser->ui32IspostCount)
		{
			pIspostUser->stVLinkInfo.stVLinkCtrl.ui5FifoThreshold = 2;
			pIspostUser->stVLinkInfo.stVLinkCtrl.ui1HMB = ISPOST_ENABLE;
			if (DATA_SEL_UN_SCALED == pIspostUser->stVLinkInfo.stVLinkCtrl.ui1DataSel)
			{
				pIspostUser->stVLinkInfo.ui32StartAddr = pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr;
			}
			else
			{
				pIspostUser->stVLinkInfo.ui32StartAddr = pIspostUser->stSS0Info.stOutImgInfo.ui32YAddr;
			}
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
			printk("@@@ ui32IspostCount = 0\n");
			if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_VENC_L) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
			{
				printk("@@@ stVLinkCtrl.ui5FifoThreshold: %d\n", pIspostUser->stVLinkInfo.stVLinkCtrl.ui5FifoThreshold);
				printk("@@@ stVLinkCtrl.ui1HMB: %d\n", pIspostUser->stVLinkInfo.stVLinkCtrl.ui1HMB);
				printk("@@@ stVLinkCtrl.ui32StartAddr: 0x%X\n", pIspostUser->stVLinkInfo.ui32StartAddr);
				printk("@@@ stVLinkCtrl ui32Ret: %d (%s)\n", ui32Ret, (ui32Ret > 0) ? "FALSE" : "TRUE");
			}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
			ui32Ret += IspostVLinkSet(pIspostUser->stVLinkInfo);
		}
		else
		{
			if (DATA_SEL_UN_SCALED == pIspostUser->stVLinkInfo.stVLinkCtrl.ui1DataSel)
			{
				pIspostUser->stVLinkInfo.ui32StartAddr = pIspostUser->stUoInfo.stOutImgInfo.ui32YAddr;
			}
			else
			{
				pIspostUser->stVLinkInfo.ui32StartAddr = pIspostUser->stSS0Info.stOutImgInfo.ui32YAddr;
			}
#ifdef INFOTM_ENABLE_ISPOST_DEBUG
			if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_VENC_L) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
			{
				printk("@@@ stVLinkCtrl.ui5FifoThreshold: %d\n", pIspostUser->stVLinkInfo.stVLinkCtrl.ui5FifoThreshold);
				printk("@@@ stVLinkCtrl.ui1HMB: %d\n", pIspostUser->stVLinkInfo.stVLinkCtrl.ui1HMB);
				printk("@@@ stVLinkCtrl.ui32StartAddr: 0x%X\n", pIspostUser->stVLinkInfo.ui32StartAddr);
				printk("@@@ stVLinkCtrl ui32Ret: %d (%s)\n", ui32Ret, (ui32Ret > 0) ? "FALSE" : "TRUE");
			}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
			ui32Ret += IspostVLinkStartAddrSet(pIspostUser->stVLinkInfo.ui32StartAddr);
		} //end of if (0 == pIspostUser->ui32IspostCount)

#ifdef INFOTM_ENABLE_ISPOST_DEBUG
		if ((pIspostUser->ui32PrintFlag & ISPOST_PRN_VENC_L) && ((pIspostUser->ui32IspostCount % pIspostUser->ui32PrintStep) == 0))
		{
			printk("@@@ stVLinkCtrl.ui5FifoThreshold: %d\n", pIspostUser->stVLinkInfo.stVLinkCtrl.ui5FifoThreshold);
			printk("@@@ stVLinkCtrl.ui1HMB: %d\n", pIspostUser->stVLinkInfo.stVLinkCtrl.ui1HMB);
			printk("@@@ stVLinkCtrl.ui32StartAddr: 0x%X\n", pIspostUser->stVLinkInfo.ui32StartAddr);
			printk("@@@ stVLinkCtrl ui32Ret: %d (%s)\n", ui32Ret, (ui32Ret > 0) ? "FALSE" : "TRUE");
		}
#endif //INFOTM_ENABLE_ISPOST_DEBUG
	} //end of if (pIspostUser->stCtrl0.ui1EnVencL)

	pIspostUser->ui32UpdateFlag &= ~ISPOST_UDP_VENC_L;

	return  ((ui32Ret > 0) ? ISPOST_FALSE : ISPOST_TRUE);
}

ISPOST_RESULT IspostUserFullTrigger(ISPOST_USER* pIspostUser)
{
	ISPOST_RESULT Ret;

	Ret = IspostUserSetup(pIspostUser);
	if (Ret ==  ISPOST_FALSE)
		return ISPOST_FALSE;

	if (pIspostUser->ui32IspostCount == 0 && pIspostUser->stCtrl0.ui1EnDN)
	{
		//write first reference frame
		pIspostUser->stCtrl0.ui1EnDN = ISPOST_DISABLE;
		pIspostUser->stCtrl0.ui1EnDNWR = ISPOST_ENABLE;
	}
	IspostResetSetCtrlAndTrigger(pIspostUser->stCtrl0);

	return ISPOST_TRUE;
}


ISPOST_RESULT IspostUserFullUpdate(ISPOST_USER* pIspostUser)
{

	if (pIspostUser->ui32UpdateFlag & ISPOST_UDP_LC)
		IspostUserSetupLC(pIspostUser);

	if (pIspostUser->ui32UpdateFlag & ISPOST_UDP_VS)
		IspostUserSetupVS(pIspostUser);

	if (pIspostUser->ui32UpdateFlag & ISPOST_UDP_OV)
		IspostUserSetupOV(pIspostUser);

	if (pIspostUser->ui32UpdateFlag & ISPOST_UDP_DN)
		IspostUserSetupDN(pIspostUser);

	if (pIspostUser->ui32UpdateFlag & ISPOST_UDP_UO)
		IspostUserSetupUO(pIspostUser);

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

//#endif //INFOTM_ISPOST
