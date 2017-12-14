/*!
******************************************************************************
 @file   : fodet.c

 @brief	 fodet entry src file

 @Author godspeed.kuo Infotmic

 @date   Sep/08/2016

 <b>Description:</b>\n


 <b>Platform:</b>\n
	     Platform Independent

 @Version
         2.0 support irq for hw operation.
	     1.0 support polling for hw operation.

******************************************************************************/
//#define IMG_KERNEL_MODULE

#include <linux/platform_device.h>
#include <mach/imap-iomap.h>
#include <asm/irq.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <asm/io.h> // virt_to_phys()
#include <linux/gfp.h>
#include <linux/uaccess.h>
#include <linux/time.h>

#ifdef IMG_KERNEL_MODULE
#include <linux/kernel.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <mach/clock.h>
#include <mach/power-gate.h>
#include <mach/imap-iomap.h>
#include "ci_fodet.h"
#include "ci_fodet_reg.h"
#else
#include <common.h>
#include <asm/io.h>
#include "drv_fodet.h"
#endif //IMG_KERNEL_MODULE


#if !defined(IMG_KERNEL_MODULE)
#define ENABLE_CHECK_REG_WRITE_VALUE
#endif //IMG_KERNEL_MODULE

FODET_UINT32 g_ui32FodetRegAddr;
FODET_UINT32 g_ui32FodetRegExpectedData;
FODET_UINT32 g_ui32FodetRegData;
FODET_UINT32 g_ui32FodTemp;

#if defined(IMG_KERNEL_MODULE)
void __iomem * g_ioFodetRegVirtBaseAddr;
void __iomem * g_ioIRamVirtBaseAddr;
#endif //IMG_KERNEL_MODULE


#if defined(IMG_KERNEL_MODULE)
	#define FODET_READL(RegAddr)                                                \
		readl(g_ioFodetRegVirtBaseAddr + RegAddr)
	#define FODET_WRITEL(RegData, RegAddr)                                      \
		writel(RegData, g_ioFodetRegVirtBaseAddr + RegAddr)
#else
	#define FODET_READL(RegAddr)                                                \
		readl(RegAddr)
	#define FODET_WRITEL(RegData, RegAddr)                                      \
		writel(RegData, RegAddr)
#endif //IMG_KERNEL_MODULE

#if defined(ENABLE_CHECK_REG_WRITE_VALUE)
#define FODET_CHECK_REG_WRITE_VALUE(RegAddr, RegExpectedData, ErrCnt)       \
    {                                                                       \
        if ((g_ui32FodTemp = FODET_READL(RegAddr)) != RegExpectedData) {       \
            if (0 == ErrCnt) {                                              \
                /* store first failure. */                                  \
                g_ui32FodetRegAddr = RegAddr;                               \
                g_ui32FodetRegExpectedData = RegExpectedData;               \
                g_ui32FodetRegData = g_ui32FodTemp;                            \
            }                                                               \
            fodet_print_reg_error(RegAddr, RegExpectedData, g_ui32FodTemp);    \
            /* increment error cnt */                                       \
            ErrCnt++;                                                       \
        }                                                                   \
    }
#else
#define FODET_CHECK_REG_WRITE_VALUE(RegAddr, RegExpectedData, ErrCnt)
#endif

#if defined(ENABLE_CHECK_REG_WRITE_VALUE)
#define FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(RegAddr, RegExpectedData, ErrCnt, Mask) \
    {                                                                       \
        if (RegExpectedData != ((g_ui32FodTemp = FODET_READL(RegAddr)) & Mask)) {  \
            if (0 == ErrCnt) {                                              \
                /* store first failure. */                                  \
                g_ui32FodetRegAddr = RegAddr;                               \
                g_ui32FodetRegExpectedData = RegExpectedData;               \
                g_ui32FodetRegData = g_ui32FodTemp;                            \
            }                                                               \
            fodet_print_reg_error(RegAddr, RegExpectedData, g_ui32FodTemp);    \
            /* increment error cnt */                                       \
            ErrCnt++;                                                       \
        }                                                                   \
    }
#else
#define FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(RegAddr, RegExpectedData, ErrCnt, Mask) \
    {                                                                       \
        if (RegExpectedData != (FODET_READL(RegAddr) & Mask)) {             \
            ErrCnt++;                                                       \
        }                                                                   \
    }
#endif


OR_DATA stOR[]=
{
/*              SF	        EqWd	EqHt	        Step	RealWd	RealHt	*/    
/*0*/{	0x2766,	0xC5,	0xC5,	0xA,	0xD9,	0xD9},
/*1*/{	0x23D1,	0xB3,	0xB3,	0xA,	0xC5,	0xC5},
/*2*/{	0x208F,	0xA3,	0xA3,	0xA,	0xB3,	0xB3	},
/*3*/{	0x1D99,	0x94,	0x94,	0xA,	0xA3,	0xA3	},
/*4*/{	0x1AE8,	0x87,	0x87,	0xA,	0x94,	0x94	},
/*5*/{	0x1876,	0x7A,	0x7A,	0x8,	0x87,	0x87	},
/*6*/{	0x163D,	0x6F,	0x6F,	0x6,	0x7A,	0x7A	},
/*7*/{	0x1437,	0x65,	0x65,	0x4,	0x6F,	0x6F	},
/*8*/{	0x1261,	0x5C,	0x5C,	0x4,	0x65,	0x65	},
/*9*/{	0x10B5,	0x54,	0x54,	0x4,	0x5C,	0x5C},
/*10*/{	0xF30,	0x4C,	0x4C,	0x4,	0x54,	0x54	},
/*11*/{	0xDCF,	0x45,	0x45,	0x4,	0x4C,	0x4C},
/*12*/{	0xC8D,	0x3F,	0x3F,	0x4,	0x45,	0x45	},
/*13*/{	0xB69,	0x39,	0x39,	0x4,	0x3F,	0x3F	},
/*14*/{	0xA5F,	0x34,	0x34,	0x4,	0x39,	0x39	},
/*15*/{	0x96E,	0x2F,	0x2F,	0x4,	0x34,	0x34	},
/*16*/{	0x893,	0x2B,	0x2B,	0x4,	0x2F,	0x2F	},
/*17*/{	0x7CB,	0x27,	0x27,	0x4,	0x2B,	0x2B	},
/*18*/{	0x716,	0x23,	0x23,	0x4,	0x27,	0x27	},
/*19*/{	0x671,	0x20,	0x20,	0x4,	0x23,	0x23	},
/*20*/{	0x5DB,	0x1D,	0x1D,	0x4,	0x20,	0x20	},
/*21*/{	0x552,	0x1B,	0x1B,	0x4,	0x1D,	0x1D},
/*22*/{	0x4D7,	0x18,	0x18,	0x4,	0x1B,	0x1B	},
/*23*/{	0x466,	0x16,	0x16,	0x4,	0x18,	0x18	},
/*24*/{	0x400,	0x14,	0x14,	0x2,	0x16,	0x16	}
};



CvAvgComp avgComps1[MAX_ARY_SCAN_ALL];
FODET_INT16 iCandidateCount;


extern void fodet_wait_event(void);
extern void fodet_wait_event_timeout(FODET_UINT32 time);



void
fodet_print_reg_error(
	FODET_UINT32 ui32RegAddr,
	FODET_UINT32 ui32RegExpectedData,
	FODET_UINT32 ui32RegData
	)
{
	FODET_MSG_LOG("FODET register read error addr=%x data=%x expected=%x\n", ui32RegAddr, ui32RegData, ui32RegExpectedData);
}

int
fodet_ioremap(void)
{
    int ret=0;
    
    // FODET REG
    //g_ioFodetRegVirtBaseAddr = ioremap(IMAP_FODET_BASE, IMAP_FODET_BASE + IMAP_FODET_SIZE - 1);
    g_ioFodetRegVirtBaseAddr = ioremap(IMAP_FODET_BASE, IMAP_FODET_SIZE);
    if(g_ioFodetRegVirtBaseAddr == NULL){
        FODET_MSG_ERR("ioremap fail \n");
        //g_ioFodetRegVirtBaseAddr = ioremap_nocache(IMAP_FODET_BASE, IMAP_FODET_BASE + IMAP_FODET_SIZE - 1);
        g_ioFodetRegVirtBaseAddr = ioremap_nocache(IMAP_FODET_BASE, IMAP_FODET_SIZE);
        if(g_ioFodetRegVirtBaseAddr == NULL){
            ret += -10;
        }else{
            FODET_MSG_INFO("do ioremap_nocache \n");
        }
    }
    FODET_MSG_INFO("IO-REMAP g_ioFodetRegVirtBaseAddr=[%p]\n",g_ioFodetRegVirtBaseAddr);

    // IRAM
    g_ioIRamVirtBaseAddr = ioremap(IMAP_IRAM_BASE, IMAP_IRAM_SIZE);
     if(g_ioIRamVirtBaseAddr == NULL){
        FODET_MSG_ERR("IRAM ioremap fail \n");
        g_ioIRamVirtBaseAddr = ioremap_nocache(IMAP_IRAM_BASE, IMAP_IRAM_SIZE);
        if(g_ioIRamVirtBaseAddr == NULL){
            ret += -20;
        }else{
            FODET_MSG_INFO("do IRAM ioremap_nocache \n");
        }
    }
    FODET_MSG_INFO("IO-REMAP g_ioIRamVirtBaseAddr=[%p]\n",g_ioIRamVirtBaseAddr);

    return ret;
}

void
fodet_iounmap(void)
{
    FODET_MSG_LOG("IO-UNMAP g_ioFodetRegVirtBaseAddr=[%p]\n",g_ioFodetRegVirtBaseAddr);
    __arm_iounmap(g_ioFodetRegVirtBaseAddr);
}


void
fodet_module_enable(
	void
	)
{
#if defined(IMG_KERNEL_MODULE)
	struct clk * busclk;
	FODET_UINT32 ui32ClkRate;
//	FODET_UINT32 test_y_value;
        FODET_UINT32 mmu_value;
        
	busclk = clk_get_sys("imap-fodet", "imap-fodet");
	ui32ClkRate = clk_get_rate(busclk);
	FODET_MSG_LOG("fodet bus clock -> %d\n",ui32ClkRate);
        clk_set_rate(busclk, 200000000);
	ui32ClkRate = clk_get_rate(busclk);
        printk("@@@Set Fodet bus clock -> %d\n", ui32ClkRate);
	module_power_on(SYSMGR_FODET_BASE);
    	module_reset(SYSMGR_FODET_BASE, 0);
	module_reset(SYSMGR_FODET_BASE, 1);

        mmu_value=0x5a5a5a5a;
        mmu_value = readl(IO_ADDRESS(SYSMGR_FODET_BASE + 0x20));
        writel(mmu_value & 0xFFFFFFFE, IO_ADDRESS(SYSMGR_FODET_BASE + 0x20));
        mmu_value=0x5a5a5a5a;        
        mmu_value = readl(IO_ADDRESS(SYSMGR_FODET_BASE + 0x20));
        fodet_ioremap();
        
#else
	int clk;

	//module_set_clock("fodet", "dpll", 400, 0);
	clk =  module_get_clock("fodet");
	FODET_MSG_LOG("fodet bus clock -> %d\n", clk);
	printf("fodet bus clock -> %d\n", clk);
	module_enable("fodet");	// where is this module_enable srource? does it need to be modified for fodet?
	//module_reset("fodet");	// where is this module_reset source? does it need to be modified for fodet?
#endif //IMG_KERNEL_MODULE
}

void
fodet_module_disable(
	void
	)
{
#if defined(IMG_KERNEL_MODULE)
	//struct clk * busclk;
	//__arm_iounmap(g_ioFodetRegVirtBaseAddr);
	fodet_iounmap();
        
	module_power_down(SYSMGR_FODET_BASE);
	//busclk = clk_get_sys("imap-fodet", "imap-fodet");
	//if (busclk) {
	//	clk_disable_unprepare(host->busclk);
	//}
#else
	module_disable("fodet");	// where is this module_disable srource? does it need to be modified for fodet?
#endif //IMG_KERNEL_MODULE
}

FODET_INTGRL_IMG_CTRL
FodetIntgrlImgCtrlGet(
	void
	)
{
	FODET_UINT32 ui32Ctrl;

	// Get fodet integral image control value.
	ui32Ctrl = FODET_READL(FODET_FODIMCTRL);

	return ((FODET_INTGRL_IMG_CTRL)ui32Ctrl);
}

FODET_RESULT
FodetIntgrlImgCtrlSet(
	FODET_INTGRL_IMG_CTRL stFodetIntgrlImgCtrl
	)
{
	FODET_UINT32 ui32Ret = 0;
#if 0
	FODET_UINT32 uiCtrlTemp;
	FODET_UINT32 ui32Temp;

	// Setup fodet integral image control value.
	uiCtrlTemp = (FODET_READL(FODET_FODIMCTRL) & 0x00000001);
	ui32Temp = (stFodetIntgrlImgCtrl.ui32Ctrl & 0xfffffffe) | uiCtrlTemp;
	FODET_WRITEL(ui32Temp, FODET_FODIMCTRL);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMCTRL, stFodetIntgrlImgCtrl.ui32Ctrl, ui32Ret);
#else

	// Setup fodet integral image control value.
	FODET_WRITEL(stFodetIntgrlImgCtrl.ui32Ctrl, FODET_FODIMCTRL);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMCTRL, stFodetIntgrlImgCtrl.ui32Ctrl, ui32Ret);
#endif

	return (ui32Ret);
}

void
FodetIntgrlImgTrigger(
	FODET_INTGRL_IMG_CTRL stFodetIntgrlImgCtrl
	)
{
	FODET_UINT32 ui32Temp;

	// Start fodet module porcess.
	ui32Temp = stFodetIntgrlImgCtrl.ui32Ctrl & 0xfffffffe;
	FODET_WRITEL((ui32Temp | FODET_STATUS_ENABLE), FODET_FODIMCTRL);
}

FODET_RESULT
FodetStatusChk(
	FODET_UINT8 ui8StatusDoneBusy
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Check fodet module process status.
	if (FODET_STATUS_DONE == ui8StatusDoneBusy) {
		FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(FODET_FODIMCTRL, 0x00000000, ui32Ret, FODET_FODIMCTRL_EN_BUSY_MASK);
	} else {
		FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(FODET_FODIMCTRL, FODET_FODIMCTRL_EN_BUSY_MASK, ui32Ret, FODET_FODIMCTRL_EN_BUSY_MASK);
	}

	return (ui32Ret);
}

FODET_STATUS
FodetStatusGet(
	void
	)
{

	// Get fodet module process status.
	return ((FODET_STATUS)(FODET_READL(FODET_FODIMCTRL) & FODET_FODIMCTRL_EN_BUSY_MASK));
}

FODET_RESULT
FodetIntgrlImgSrcAddrSet(
	FODET_UINT32 ui32Addr
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup integral image source start address.
	FODET_WRITEL(ui32Addr, FODET_FODIMSRC);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMSRC, ui32Addr, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetIntgrlImgSrcStrideSet(
	FODET_UINT16 ui16Stride
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup integral image source stride.
	FODET_WRITEL((FODET_UINT32)ui16Stride, FODET_FODIMSS);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMSS, (FODET_UINT32)ui16Stride, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetIntgrlImgSrcSizeSet(
	FODET_UINT16 ui16Width,
	FODET_UINT16 ui16Height
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup integral image source size.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMSZ, H, (FODET_UINT32)ui16Height);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMSZ, W, (FODET_UINT32)ui16Width);
	FODET_WRITEL(ui32Temp, FODET_FODIMSZ);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMSZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetIntgrlImgSrcSet(
	FODET_IMG_INFO stIntgrlImgSrcInfo
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup Integral image source start address.
	FODET_WRITEL(stIntgrlImgSrcInfo.ui32Addr, FODET_FODIMSRC);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMSRC, stIntgrlImgSrcInfo.ui32Addr, ui32Ret);

	// Setup integral image source stride.
	FODET_WRITEL((FODET_UINT32)stIntgrlImgSrcInfo.ui16Stride, FODET_FODIMSS);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMSS, (FODET_UINT32)stIntgrlImgSrcInfo.ui16Stride, ui32Ret);

	// Setup integral image source size.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMSZ, H, (FODET_UINT32)stIntgrlImgSrcInfo.ui16Height);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMSZ, W, (FODET_UINT32)stIntgrlImgSrcInfo.ui16Width);
	FODET_WRITEL(ui32Temp, FODET_FODIMSZ);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMSZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetIntgrlImgOutAddrSet(
	FODET_UINT32 ui32Addr
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup integral image output sum buffer start address.
	FODET_WRITEL(ui32Addr, FODET_FODIMOA);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMOA, ui32Addr, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetIntgrlImgOutStrideOffsetSet(
	FODET_UINT16 ui16Stride,
	FODET_UINT16 ui16Offset
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup integral image output sum buffer stride and offset.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMOS, IIS, (FODET_UINT32)ui16Stride);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMOS, IIO, (FODET_UINT32)ui16Offset);
	FODET_WRITEL(ui32Temp, FODET_FODIMOS);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMOS, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetIntgrlImgOutSet(
	FODET_IMG_INFO stIntgrlImgOutInfo
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup integral image output sum buffer start address.
	FODET_WRITEL(stIntgrlImgOutInfo.ui32Addr, FODET_FODIMOA);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMOA, stIntgrlImgOutInfo.ui32Addr, ui32Ret);

	// Setup integral image output sum buffer stride and offset.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMOS, IIS, (FODET_UINT32)stIntgrlImgOutInfo.ui16Stride);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMOS, IIO, (FODET_UINT32)stIntgrlImgOutInfo.ui16Offset);
	FODET_WRITEL(ui32Temp, FODET_FODIMOS);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMOS, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetIntgrlImgBusCtrlSet(
	FODET_UINT8 ui8MemWriteBurstSize,
	FODET_UINT8 ui8MemWriteThreshold,
	FODET_UINT8 ui8MemReadThreshold
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup fodet integral image bus control value.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMBCTRL, MWB, (FODET_UINT32)ui8MemWriteBurstSize);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMBCTRL, MWT, (FODET_UINT32)ui8MemWriteThreshold);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMBCTRL, MRT, (FODET_UINT32)ui8MemReadThreshold);
	FODET_WRITEL(ui32Temp, FODET_FODIMBCTRL);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMBCTRL, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_UINT32
FodetClsfrCtrlGet(
	void
	)
{

	// Get fodet classifier control value.
	return (FODET_READL(FODET_FODCCTRL));
}

FODET_RESULT
FodetClsfrCtrlSet(
	FODET_CLSFR_CTRL stFodetClsfrCtrl
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup fodet classifier control value.
	FODET_WRITEL(stFodetClsfrCtrl.ui32Ctrl, FODET_FODCCTRL);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCCTRL, stFodetClsfrCtrl.ui32Ctrl, ui32Ret);

	return (ui32Ret);
}

void
FodetClsfrReset(
	void
	)
{
	FODET_CLSFR_CTRL stFodetClsfrCtrl;

	// Reset fodet classifier module.
	// Get fodet classifier control value.
	stFodetClsfrCtrl.ui32Ctrl = FODET_READL(FODET_FODCCTRL);
	// Fodet classifier disable.
	stFodetClsfrCtrl.ui1EnBusy = 0;
	// Fodet classifier reset activate.
	stFodetClsfrCtrl.ui1ResetDetected = 1;
	FODET_WRITEL(stFodetClsfrCtrl.ui32Ctrl, FODET_FODCCTRL);
	// De-activate fodet classifier module.
	stFodetClsfrCtrl.ui1ResetDetected = 0;
	FODET_WRITEL(stFodetClsfrCtrl.ui32Ctrl, FODET_FODCCTRL);
}

void
FodetClsfrTrigger(
	FODET_CLSFR_CTRL stFodetClsfrCtrl
	)
{
	FODET_UINT32 ui32Temp;

	// Start fodet classifier module porcess.
	ui32Temp = stFodetClsfrCtrl.ui32Ctrl & 0xfffffff0; //0xfffffff8;
	FODET_WRITEL((ui32Temp | FODET_STATUS_ENABLE), FODET_FODCCTRL);
}

void
FodetClsfrResumeAndTrigger(
	FODET_CLSFR_CTRL stFodetClsfrCtrl
	)
{
	FODET_UINT32 ui32Temp;

	// Start fodet classifier module porcess.
	ui32Temp = stFodetClsfrCtrl.ui32Ctrl & 0xfffffff0; //0xfffffff8;
	FODET_WRITEL((ui32Temp | FODET_STATUS_ENABLE | FODET_STATUS_RESUME), FODET_FODCCTRL);
}

FODET_RESULT
FodetClsfrStatusDetectedChk(
	FODET_UINT8 ui8StatusResetDetected
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Check fodet classifier module process status.
	if (FODET_STATUS_DETECTED == ui8StatusResetDetected) {
		FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(FODET_FODCCTRL, FODET_FODCCTRL_RD_MASK, ui32Ret, FODET_FODCCTRL_RD_MASK);
	} else {
		FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(FODET_FODCCTRL, 0x00000000, ui32Ret, FODET_FODCCTRL_RD_MASK);
	}

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrStatusHoldChk(
	FODET_UINT8 ui8StatusResumeHold
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Check fodet classifier module process status.
	if (FODET_STATUS_HOLD == ui8StatusResumeHold) {
		FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(FODET_FODCCTRL, FODET_FODCCTRL_RH_MASK, ui32Ret, FODET_FODCCTRL_RH_MASK);
	} else {
		FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(FODET_FODCCTRL, 0x00000000, ui32Ret, FODET_FODCCTRL_RH_MASK);
	}

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrStatusChk(
	FODET_UINT8 ui8StatusDoneBusy
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Check fodet classifier module process status.
	if (FODET_STATUS_DONE == ui8StatusDoneBusy) {
		FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(FODET_FODCCTRL, 0x00000000, ui32Ret, FODET_FODCCTRL_EN_BUSY_MASK);
	} else {
		FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(FODET_FODCCTRL, FODET_FODCCTRL_EN_BUSY_MASK, ui32Ret, FODET_FODCCTRL_EN_BUSY_MASK);
	}

	return (ui32Ret);
}

FODET_STATUS
FodetClsfrStatusGet(
	void
	)
{

	// Get fodet classifier module process status.
	return ((FODET_STATUS)(FODET_READL(FODET_FODCCTRL) & FODET_FODCCTRL_EN_BUSY_MASK));
}

FODET_RESULT
FodetClsfrCascadeImgAddrSet(
	FODET_UINT32 ui32Addr
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup fodet classifier cascade image buffer start address.
	FODET_WRITEL(ui32Addr, FODET_FODCCA);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCCA, ui32Addr, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrHidCascadeOutAddrSet(
	FODET_UINT32 ui32Addr
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup classifier hid-cascade output sum buffer start address.
	FODET_WRITEL(ui32Addr, FODET_FODCHCA);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCHCA, ui32Addr, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrIntgrlImgOutAddrSet(
	FODET_UINT32 ui32Addr
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup classifier integral image output sum buffer start address.
	FODET_WRITEL(ui32Addr, FODET_FODCIMCA);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCIMCA, ui32Addr, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrIntgrlImgOutStrideOffsetSet(
	FODET_UINT16 ui16Stride,
	FODET_UINT16 ui16Offset
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup fodet classifier integral image output sum buffer stride and offset.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCIMS, IIS, (FODET_UINT32)ui16Stride);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCIMS, IIO, (FODET_UINT32)ui16Offset);
	FODET_WRITEL(ui32Temp, FODET_FODCIMS);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCIMS, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrIntgrlImgOutSet(
	FODET_IMG_INFO stClsfrIntgrlImgOutInfo
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup classifier integral image output sum buffer start address.
	FODET_WRITEL(stClsfrIntgrlImgOutInfo.ui32Addr, FODET_FODCIMCA);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCIMCA, stClsfrIntgrlImgOutInfo.ui32Addr, ui32Ret);

	// Setup fodet classifier integral image output sum buffer stride and offset.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCIMS, IIS, (FODET_UINT32)stClsfrIntgrlImgOutInfo.ui16Stride);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCIMS, IIO, (FODET_UINT32)stClsfrIntgrlImgOutInfo.ui16Offset);
	FODET_WRITEL(ui32Temp, FODET_FODCIMS);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCIMS, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrIntgrlImgBusCtrlSet(
	FODET_UINT8 ui8MemWriteBurstSize,
	FODET_UINT8 ui8MemWriteThreshold,
	FODET_UINT8 ui8MemReadThreshold
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup fodet classifier integral image bus control value.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCBCTRL, MWB, (FODET_UINT32)ui8MemWriteBurstSize);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCBCTRL, MWT, (FODET_UINT32)ui8MemWriteThreshold);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCBCTRL, MRT, (FODET_UINT32)ui8MemReadThreshold);
	FODET_WRITEL(ui32Temp, FODET_FODCBCTRL);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCBCTRL, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrIntgrlImgScaleSizeSet(
	FODET_UINT16 ui16ScalingFactor,
	FODET_UINT8 ui8EqWinWidth,
	FODET_UINT8 ui8EqWinHeight
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup fodet classifier integral image scale and size.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCSSZ, EWH, (FODET_UINT32)ui8EqWinHeight);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCSSZ, EWW, (FODET_UINT32)ui8EqWinWidth);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCSSZ, SF, (FODET_UINT32)ui16ScalingFactor);
	FODET_WRITEL(ui32Temp, FODET_FODCSSZ);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCSSZ, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrWndLmtSet(
	FODET_CLSFR_WND_LMT stClsfrWndLmt
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup fodet classifier window limit and step.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCWL, SH, (FODET_UINT32)stClsfrWndLmt.ui7StopHeight);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCWL, SW, (FODET_UINT32)stClsfrWndLmt.ui8StopWidth);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCWL, SP, (FODET_UINT32)stClsfrWndLmt.ui14Step);
	FODET_WRITEL(ui32Temp, FODET_FODCWL);
	//FODET_CHECK_REG_WRITE_VALUE(FODET_FODCWL, stClsfrWndLmt.ui32Value, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrReciprocalWndAreaSet(
	FODET_UINT16 ui16ReciprocalWndArea
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup fodet classifier reciprocal of window area.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCRC, RWA, (FODET_UINT32)ui16ReciprocalWndArea);
	FODET_WRITEL(ui32Temp, FODET_FODCRC);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCRC, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrStatusIdxSet(
	FODET_UINT8 ui8ClsfrStatusIdx
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup fodet classifier status index.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCSIDX, IDX, (FODET_UINT32)ui8ClsfrStatusIdx);
	FODET_WRITEL(ui32Temp, FODET_FODCSIDX);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCSIDX, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrStatusValChk(
	FODET_UINT32 ui32FodetClsfrStatusVal
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Check fodet classifier status value.
	FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(FODET_FODCSVAL, ui32FodetClsfrStatusVal, ui32Ret, FODET_FODCSVAL_VAL_MASK);

	return (ui32Ret);
}

FODET_UINT32
FodetClsfrStatusValGet(
	void
	)
{

	// Get fodet classifier status value.
#if 0
	return (FODET_READL(FODET_FODCSVAL) & FODET_FODCSVAL_VAL_MASK);
#else
	return (FODET_READL(FODET_FODCSVAL));
#endif
}

FODET_RESULT
FodetClsfrStatusValSet(
	FODET_UINT32 ui32FodetClsfrStatusVal
	)
{
	FODET_UINT32 ui32Ret = 0;
#if 0
	FODET_UINT32 ui32Temp;

	// Setup fodet classifier status value.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODCSVAL, VAL, ui32FodetClsfrStatusVal);
	FODET_WRITEL(ui32Temp, FODET_FODCSVAL);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCSVAL, ui32Temp, ui32Ret);
#else

	// Setup fodet classifier status value.
	FODET_WRITEL(ui32FodetClsfrStatusVal, FODET_FODCSVAL);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCSVAL, ui32FodetClsfrStatusVal, ui32Ret);
#endif

	return (ui32Ret);
}

FODET_RESULT
FodetAxiCtrl0Set(
	FODET_UINT8 ui8IMCID,
	FODET_UINT8 ui8HCID,
	FODET_UINT8 ui8CID,
	FODET_UINT8 ui8SRCID
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup fodet AXI control 0 IMCID, HCID, CID and SRCID.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODAXI0, IMCID, (FODET_UINT32)ui8IMCID);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODAXI0, HCID, (FODET_UINT32)ui8HCID);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODAXI0, CID, (FODET_UINT32)ui8CID);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODAXI0, SRCID, (FODET_UINT32)ui8SRCID);
	FODET_WRITEL(ui32Temp, FODET_FODAXI0);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODAXI0, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetAxiCtrl1Set(
	FODET_UINT8 ui8FWID,
	FODET_UINT8 ui8HCCID
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup fodet AXI control 1 FWID and HCCID.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODAXI1, FWID, (FODET_UINT32)ui8FWID);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODAXI1, HCCID, (FODET_UINT32)ui8HCCID);
	FODET_WRITEL(ui32Temp, FODET_FODAXI1);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODAXI1, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetAxiCtrlSet(
	FODET_AXI_ID stAxiId
	)
{
	FODET_UINT32 ui32Ret = 0;

	//
	// Setup fodet AXI control 0 IMCID, HCID, CID and SRCID.
	// ============================
	//
	ui32Ret += FodetAxiCtrl0Set(
		stAxiId.ui8IMCID,
		stAxiId.ui8HCID,
		stAxiId.ui8CID,
		stAxiId.ui8SRCID
		);

	//
	// Setup fodet AXI control 1 FWID and HCCID.
	// ============================
	//
	ui32Ret += FodetAxiCtrl1Set(
		stAxiId.ui8FWID,
		stAxiId.ui8HCCID
		);

	return (ui32Ret);
}

FODET_UINT32
FodetCacheCtrlGet(
	void
	)
{

	// Get fodet cache control value.
	return (FODET_READL(FODET_FODCHCTRL));
}

FODET_RESULT
FodetCacheCtrlSet(
	FODET_CACHE_CTRL stFodetCacheCtrl
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup fodet cache control value.
	FODET_WRITEL(stFodetCacheCtrl.ui32Ctrl, FODET_FODCHCTRL);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCHCTRL, stFodetCacheCtrl.ui32Ctrl, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetMasterReset(
	FODET_CACHE_CTRL stFodetCacheCtrl
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_CACHE_CTRL stTemp;

	stTemp.ui32Ctrl = stFodetCacheCtrl.ui32Ctrl;
	stTemp.ui1FodMstrEn = 0;
	stTemp.ui1FodMstrRst = 1;
	FODET_WRITEL(stTemp.ui32Ctrl, FODET_FODCHCTRL);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCHCTRL, stTemp.ui32Ctrl, ui32Ret);

	return (ui32Ret);
}

void
FodetMasterReset2(
	void
	)
{
	FODET_CACHE_CTRL stFodetCacheCtrl;

	// Reset fodet master module.
	// Get fodet cache control value.
	stFodetCacheCtrl.ui32Ctrl = FODET_READL(FODET_FODCHCTRL);
	// Fodet master disable.
	stFodetCacheCtrl.ui1FodMstrEn = 0;
	// Fodet master reset activate.
	stFodetCacheCtrl.ui1FodMstrRst = 1;
	FODET_WRITEL(stFodetCacheCtrl.ui32Ctrl, FODET_FODCHCTRL);
	// Fodet master disable.
	stFodetCacheCtrl.ui1FodMstrEn = 1;
	// De-activate fodet master module.
	stFodetCacheCtrl.ui1FodMstrRst = 0;
	FODET_WRITEL(stFodetCacheCtrl.ui32Ctrl, FODET_FODCHCTRL);
}

FODET_RESULT
FodetMasterEnable(
	FODET_CACHE_CTRL *pstFodetCacheCtrl
	)
{
	FODET_UINT32 ui32Ret = 0;

	pstFodetCacheCtrl->ui1FodMstrEn = 1;
	FODET_WRITEL(pstFodetCacheCtrl->ui32Ctrl, FODET_FODCHCTRL);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODCHCTRL, pstFodetCacheCtrl->ui32Ctrl, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetIntStatusChk(
	FODET_UINT8 ui8IntOffOn,
	FODET_CACHE_CTRL stFodetCacheCtrl
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Mask;

	ui32Mask = stFodetCacheCtrl.ui32Ctrl | FODET_FODCHCTRL_INT_MASK;
	// Check fodet interrupt status.
	if (FODET_INT_STATUS_OFF == ui8IntOffOn) {
		FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(FODET_FODCHCTRL, stFodetCacheCtrl.ui32Ctrl, ui32Ret, ui32Mask);
	} else {
		FODET_CHECK_REG_WRITE_VALUE_WITH_MASK(FODET_FODCHCTRL, ui32Mask, ui32Ret, ui32Mask);
	}

	return (ui32Ret);
}

FODET_CACHE_CTRL
FodetIntStatusGet(
	void
	)
{
	FODET_CACHE_CTRL st32Temp;

	// Get fodet interrupr status.
	st32Temp.ui32Ctrl = FODET_READL(FODET_FODCHCTRL) & FODET_FODCHCTRL_INT_MASK;

	return (st32Temp);
}

void
FodetIntStatusClear(
	void
	)
{

	FODET_UINT32 ui32Ctrl;

	ui32Ctrl = FODET_READL(FODET_FODCHCTRL);

	ui32Ctrl &= 0xfeffffff;

	FODET_WRITEL(ui32Ctrl, FODET_FODCHCTRL);
}

FODET_RESULT
FodetIntgrlImgCacheAddrSet(
	FODET_UINT32 ui32Addr
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup fodet integral image cache start address.
	FODET_WRITEL(ui32Addr, FODET_FODIMCA);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMCA, ui32Addr, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetIntgrlImgCacheStrideOffsetSet(
	FODET_UINT16 ui16Stride,
	FODET_UINT16 ui16Offset
	)
{
	FODET_UINT32 ui32Ret = 0;
	FODET_UINT32 ui32Temp;

	// Setup fodet integral image cache stride and offset.
	ui32Temp = 0;
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMCS, IIS, (FODET_UINT32)ui16Stride);
	FODET_REGIO_WRITE_FIELD(ui32Temp, FODET, FODIMCS, IIO, (FODET_UINT32)ui16Offset);
	FODET_WRITEL(ui32Temp, FODET_FODIMCS);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMCS, ui32Temp, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetIntgrlImgCacheLoadAddrSet(
	FODET_UINT32 ui32Addr
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup fodet integral image cache load start address.
	FODET_WRITEL(ui32Addr, FODET_FODIMCLA);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMCLA, ui32Addr, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetIntgrlImgCacheLoadLocalAddrSet(
	FODET_UINT32 ui32Addr
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup fodet integral image cache load local start address.
	FODET_WRITEL(ui32Addr, FODET_FODIMCLLA);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMCLLA, ui32Addr, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetIntgrlImgCacheLoadLocalLengthSet(
	FODET_UINT32 ui32Length
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup fodet integral image cache load local length.
	FODET_WRITEL(ui32Length, FODET_FODIMCLLL);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODIMCLLL, ui32Length, ui32Ret);

	return (ui32Ret);
}

FODET_RESULT
FodetHidCascadeCacheAddrSet(
	FODET_UINT32 ui32Addr
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup fodet Hid-Cascade cache start address.
	FODET_WRITEL(ui32Addr, FODET_FODHCCA);
	FODET_CHECK_REG_WRITE_VALUE(FODET_FODHCCA, ui32Addr, ui32Ret);

	return (ui32Ret);
}


#if !defined(IMG_KERNEL_MODULE)
void
FodetRegDump(
	char* pszFilename
	)
{
#define TEMP_MEM_SIZE  (512)
#define DUMP_MEM_SIZE  (1024 * 40)

	FODET_UINT32 uiIdx;
	FODET_UINT32 ui32RegAddr;
	FILE * RegDumpfd = NULL;
	FODET_INT32 iWriteLen = 0;
	FODET_INT32 iWriteResult = 0;
	char szTemp[TEMP_MEM_SIZE];
	char szDump[DUMP_MEM_SIZE];

	FODET_MSG_LOG("Registers dump filename =  %s\n", pszFilename);
	RegDumpfd = fopen(pszFilename, "wb");
	if (NULL != RegDumpfd) {
		memset(szDump, 0, DUMP_MEM_SIZE);
		iWriteLen = 0;
		sprintf(szTemp, "===== FODET Integral Image control registers dump =====\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x000; uiIdx<=0x000; uiIdx+=4) {
			ui32RegAddr = FODET_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, FODET_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "===== FODET Integral Image registers dump =====\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x004; uiIdx<=0x018; uiIdx+=4) {
			ui32RegAddr = FODET_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, FODET_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "===== FODET Classifier control registers dump =====\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x020; uiIdx<=0x020; uiIdx+=4) {
			ui32RegAddr = FODET_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, FODET_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "===== FODET Classifier registers dump =====\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x024; uiIdx<=0x048; uiIdx+=4) {
			ui32RegAddr = FODET_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, FODET_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "===== FODET AXI control registers dump =====\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x080; uiIdx<=0x084; uiIdx+=4) {
			ui32RegAddr = FODET_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, FODET_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "===== FODET Cache control registers dump =====\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x090; uiIdx<=0x090; uiIdx+=4) {
			ui32RegAddr = FODET_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, FODET_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "===== FODET Integral Image Cache control registers dump =====\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x094; uiIdx<=0x0A4; uiIdx+=4) {
			ui32RegAddr = FODET_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, FODET_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "===== FODET HID-Cascade Cache control registers dump =====\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		for (uiIdx=0x0B0; uiIdx<=0x0B0; uiIdx+=4) {
			ui32RegAddr = FODET_PERI_BASE_ADDR + uiIdx;
			sprintf(szTemp, "0x%08X = 0x%08X\n", ui32RegAddr, FODET_READL(ui32RegAddr));
			strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
			iWriteLen += strlen(szTemp);
		}
		sprintf(szTemp, "===========================================\n\n");
		strncpy(&szDump[iWriteLen], szTemp, TEMP_MEM_SIZE);
		iWriteLen += strlen(szTemp);
		//============================================================
		iWriteResult = fwrite((void *)szDump, iWriteLen, 1, RegDumpfd);
		if (iWriteResult != iWriteLen) {
			printf("FODET registers dump file write failed!!!\n");
		}
		fclose(RegDumpfd);
	} else {
		printf("FODET registers dump file open failed!!!\n");
	}
}

void
FodetMemoryDump(
	char* pszFilename,
	FODET_UINT32 ui32StartAddress,
	FODET_UINT32 ui32Length
	)
{
	FILE * MemDumpfd = NULL;
	FODET_INT32 iWriteLen = 0;
	FODET_INT32 iWriteResult = 0;

	printf("Memory dump filename =  %s\n", pszFilename);
	MemDumpfd = fopen(pszFilename, "wb");
	if (NULL != MemDumpfd) {
		iWriteLen = ui32Length;
		iWriteResult = fwrite((void *)ui32StartAddress, iWriteLen, 1, MemDumpfd);
		if (iWriteResult != iWriteLen) {
			printf("FODET memory dump file write failed!!!\n");
		}
		fclose(MemDumpfd);
	} else {
		printf("FODET memory dump file open failed!!!\n");
	}
}
#endif



FODET_RESULT
FodetIntgrlImgSet(
	FODET_INTGRL_IMG_INFO stIntgrlImgInfo
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup integral image source address, stride, width and height.
	ui32Ret += FodetIntgrlImgSrcSet(stIntgrlImgInfo.stIntgrlImgSrcInfo);

	// Setup integral image output sum buffer address, stride and offset.
	ui32Ret += FodetIntgrlImgOutSet(stIntgrlImgInfo.stIntgrlImgOutInfo);

	// Setup integral image bus control.
	ui32Ret += FodetIntgrlImgBusCtrlSet(
				stIntgrlImgInfo.stIntgrlImgBusCtrl.ui8MemWriteBurstSize,
				stIntgrlImgInfo.stIntgrlImgBusCtrl.ui8MemWriteThreshold,
				stIntgrlImgInfo.stIntgrlImgBusCtrl.ui8MemReadThreshold
				);

	return (ui32Ret);
}

FODET_RESULT
FodetClsfrInfoSet(
	FODET_CLSFR_INFO stClsfrInfo
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup classifier cascade image buffer start address.
	ui32Ret += FodetClsfrCascadeImgAddrSet(
				stClsfrInfo.ui32ClsfrCscdImgAddr
				);

	// Setup classifier hid-cascade output buffer start address.
	ui32Ret += FodetClsfrHidCascadeOutAddrSet(
				stClsfrInfo.ui32ClsfrHidCscdOutAddr
				);

	// Setup classifier integral image sum out buffer address, stride and offset.
	ui32Ret += FodetClsfrIntgrlImgOutSet(
				stClsfrInfo.stClsfrIntgrlImgOut
				);

	// Setup classifier integral image bus control.
	ui32Ret += FodetClsfrIntgrlImgBusCtrlSet(
				stClsfrInfo.stClsfrIntgrlImgBusCtrl.ui8MemWriteBurstSize,
				stClsfrInfo.stClsfrIntgrlImgBusCtrl.ui8MemWriteThreshold,
				stClsfrInfo.stClsfrIntgrlImgBusCtrl.ui8MemReadThreshold
				);

	// Setup classifier scale and size.
	ui32Ret += FodetClsfrIntgrlImgScaleSizeSet(
				stClsfrInfo.stClsfrScaleSize.ui14ScalingFactor,
				stClsfrInfo.stClsfrScaleSize.ui8EqWinWidth,
				stClsfrInfo.stClsfrScaleSize.ui8EqWinHeight
				);

	// Setup classifier reciprocal of window area.
	ui32Ret += FodetClsfrReciprocalWndAreaSet(
				stClsfrInfo.ui16ReciprocalWndArea
				);

	// Setup classifier window limit.
	ui32Ret += FodetClsfrWndLmtSet(
				stClsfrInfo.stClsfrWndLmt
				);

	return (ui32Ret);
}

FODET_RESULT
FodetCacheInfoSet(
	FODET_CACHE_INFO stCacheInfo
	)
{
	FODET_UINT32 ui32Ret = 0;

	// Setup integral image cache start address.
	ui32Ret += FodetIntgrlImgCacheAddrSet(stCacheInfo.ui32Addr);

	// Setup integral image cache stride and offset.
	ui32Ret += FodetIntgrlImgCacheStrideOffsetSet(stCacheInfo.ui16Stride, stCacheInfo.ui16Offset);

	// Setup integral image cache load start address.
	ui32Ret += FodetIntgrlImgCacheLoadAddrSet(stCacheInfo.ui32LoadAddr);

	// Setup integral image cache load local start address.
	ui32Ret += FodetIntgrlImgCacheLoadLocalAddrSet(stCacheInfo.ui32LoadLocalAddr);

	// Setup integral image cache load local length.
	ui32Ret += FodetIntgrlImgCacheLoadLocalLengthSet(stCacheInfo.ui32LoadLocalLength);

	// Setup hid-cascade cache start address.
	ui32Ret += FodetHidCascadeCacheAddrSet(stCacheInfo.ui32HidCascadeCacheAddr);

	return (ui32Ret);
}

bool FodetIntegralImgCheckBusy(void)
{
    if((FODET_READL(0x0000) & FOD_STATUS_BUSY) == FOD_STATUS_DONE)
        return false;
    else
        return true;
}

void FodetClassifierClearINTandHAR(void)
{
        FODET_CACHE_CTRL stValue;

        stValue.ui32Ctrl = FODET_READL(FODET_FODCHCTRL) & 0xFEFFBFFF;
        FodetCacheCtrlSet(stValue);
}

void InitGroupingArray_Scanall(void)
{
    u_int8_t i=0;

    for(i=0; i<MAX_ARY_SCAN_ALL; i++)
    {
        avgComps1[i].rect.x = 0;
        avgComps1[i].rect.y = 0;
        avgComps1[i].rect.w = 0;
        avgComps1[i].rect.h = 0;
        avgComps1[i].neighbors = 0;
    }
}

FODET_UINT16 CheckStartIdx(FODET_USER* pFodetUser)
{
    FODET_UINT16 i;
    FODET_UINT16 FdIndex;
    FODET_UINT16 ui16ImgWd, ui16ImgHt;
    FODET_UINT16 ui16StartIdx, ui16EndIdx;

    if(pFodetUser->ui32ImgOffset !=0){
        ui16ImgWd = pFodetUser->ui16CropWidth; 
        ui16ImgHt = pFodetUser->ui16CropHeight;
    }else{
        ui16ImgWd = pFodetUser->ui16ImgWidth; 
        ui16ImgHt = pFodetUser->ui16ImgHeight;
    }
    ui16StartIdx = pFodetUser->ui16StartIndex;
    ui16EndIdx = pFodetUser->ui16EndIndex;    

    if((ui16ImgWd >= stOR[ui16StartIdx].eq_win_wd)
        && (ui16ImgHt >= stOR[ui16StartIdx].eq_win_ht))
        return ui16StartIdx;
        
    FdIndex  = ui16EndIdx;
    for(i=ui16StartIdx; i<ui16EndIdx; i++)
    {
        if(((stOR[i].eq_win_wd <= ui16ImgWd) && (ui16ImgWd < stOR[i+1].eq_win_wd))
            && ((stOR[i].eq_win_ht <= ui16ImgHt) && (ui16ImgHt < stOR[i+1].eq_win_ht)))
        {
            FdIndex = i;
            break;
        }
    }

    return FdIndex;
}

//#define TURN_ON_TILTED_SUM

FODET_INT32 fodet_IntegralImage(FODET_USER* pFodetUser)
{
    FODET_INT32 ret=0;
    FODET_UINT32 ui32LoopTimeOut=0;
    FODET_INTGRL_IMG_INFO stIntgrlImgInfo;
    FODET_AXI_ID stAxiId;

    stAxiId.ui8IMCID = 4;
    stAxiId.ui8HCID = 3;
    stAxiId.ui8CID = 2;
    stAxiId.ui8SRCID = 1;
    stAxiId.ui8FWID = 6;
    stAxiId.ui8HCCID = 5;
    FodetAxiCtrlSet(stAxiId);

    // Reset , Enable master
    stIntgrlImgInfo.stCacheCtrl.ui32Ctrl = 0;
    ret += FodetMasterReset(stIntgrlImgInfo.stCacheCtrl);
    ret += FodetMasterEnable(&stIntgrlImgInfo.stCacheCtrl);
    // Reset Classifier
    FodetClsfrReset();

    if(pFodetUser->ui32ImgOffset != 0)
    {
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui32Addr = pFodetUser->ui32SrcImgAddr + pFodetUser->ui32ImgOffset;
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui16Width = pFodetUser->ui16CropWidth;
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui16Height = pFodetUser->ui16CropHeight;
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui16Stride = pFodetUser->ui16ImgStride;
    }else{
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui32Addr = pFodetUser->ui32SrcImgAddr;
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui16Width = pFodetUser->ui16ImgWidth;
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui16Height = pFodetUser->ui16ImgHeight;
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui16Stride = pFodetUser->ui16ImgStride;
    }
    stIntgrlImgInfo.stIntgrlImgOutInfo.ui32Addr = pFodetUser->ui32IISumAddr;
    stIntgrlImgInfo.stIntgrlImgOutInfo.ui16Stride = pFodetUser->ui16IIoutStride;
    stIntgrlImgInfo.stIntgrlImgOutInfo.ui16Offset = pFodetUser->ui16IIoutOffset;
    stIntgrlImgInfo.stIntgrlImgBusCtrl.ui8MemWriteBurstSize = 15;
    stIntgrlImgInfo.stIntgrlImgBusCtrl.ui8MemWriteThreshold = 15;
    stIntgrlImgInfo.stIntgrlImgBusCtrl.ui8MemReadThreshold = 15;	
    ret += FodetIntgrlImgSet(stIntgrlImgInfo);

#if 0
    FODET_MSG_LOG("IntegralImage %s-%s\n",__DATE__, __TIME__ );
    FODET_MSG_LOG("<@04@>[%08X]\n",FODET_READL(0x0004));
    FODET_MSG_LOG("<@08@>[%08X]\n",FODET_READL(0x0008));
    FODET_MSG_LOG("<@0c@>[%08X]\n",FODET_READL(0x000c));
    FODET_MSG_LOG("<@10@>[%08X]\n",FODET_READL(0x0010));
    FODET_MSG_LOG("<@14@>[%08X]\n",FODET_READL(0x0014));
    FODET_MSG_LOG("<@18@>[%08X]\n",FODET_READL(0x0018));
#endif

    // Integral Image - Sum
    stIntgrlImgInfo.stIntgrlImgCtrl.ui32Ctrl = 0;
    stIntgrlImgInfo.stIntgrlImgCtrl.ui1SrcImgFmt = pFodetUser->ui16ImgFormat;
    ret += FodetIntgrlImgCtrlSet(stIntgrlImgInfo.stIntgrlImgCtrl);
    ui32LoopTimeOut = 1000;
    FodetIntgrlImgTrigger(stIntgrlImgInfo.stIntgrlImgCtrl);
    // fodet_wait_event();
    fodet_wait_event_timeout(ui32LoopTimeOut);
    if(FodetIntegralImgCheckBusy()){
        FODET_MSG_ERR("@@@ ERROR : II-Sum not done\n");
        return-10;
    }

    //Integral Image - Sqsum
    stIntgrlImgInfo.stIntgrlImgCtrl.ui2IntgrlImgMode = INTGRL_IMG_MODE_SQ_SUM;
    ret += FodetIntgrlImgCtrlSet(stIntgrlImgInfo.stIntgrlImgCtrl);
    ui32LoopTimeOut =1000;
    FodetIntgrlImgTrigger(stIntgrlImgInfo.stIntgrlImgCtrl);
    // fodet_wait_event();
    fodet_wait_event_timeout(ui32LoopTimeOut);
    if(FodetIntegralImgCheckBusy()){
        FODET_MSG_ERR("@@@ ERROR : II-Sqsum not done\n");
        return-20;
    }

#ifdef TURN_ON_TILTED_SUM
    //Integral Image - Tilted Sum
    stIntgrlImgInfo.stIntgrlImgCtrl.ui1IntgrlImgMode = INTGRL_IMG_MODE_TILTED_SUM;
    ret += FodetIntgrlImgCtrlSet(stIntgrlImgInfo.stIntgrlImgCtrl);
    ui32LoopTimeOut =1000;
    FodetIntgrlImgTrigger(stIntgrlImgInfo.stIntgrlImgCtrl);
    // fodet_wait_event();
    fodet_wait_event_timeout(ui32LoopTimeOut);
    if(FodetIntegralImgCheckBusy()){
        FODET_MSG_ERR("@@@ ERROR : II-Tilted sum not done\n");
        return-30;
    }
#endif  // TURN_ON_TILTED_SUM

    return ret;
}


FODET_INT32 fodet_RenewRunWithCache(FODET_USER* pFodetUser)
{
    FODET_INT32 ret=0;
    FODET_UINT32 ui32TimeOut;
//    FODET_UINT32 ui32Temp=0;
    FODET_UINT32 ui32Trigger=0x00000000;
    FODET_INT8 i8FDIndex=0;
    FODET_UINT16 ui16EqWinWd=0;
    FODET_UINT16 ui16EqWinHt=0;
    FODET_UINT32 ui32InvWin=0;
    FODET_UINT16 ui16StopWd, ui16StopHt;
    FODET_UINT16 ui16ShiftStep;
    FODET_UINT16 ui16RealWinWd=0;
    FODET_UINT16 ui16RealWinHt=0;
    FODET_UINT16 ui16Xost, ui16Yost;
    FODET_AXI_ID FodetAxi;
    FODET_CACHE_CTRL stFodetCacheCtrl;
    FODET_INTGRL_IMG_CTRL FodetIntgrlImgCtrl;
    FODET_UINT8 ui8ClsfrStatusIdx;
    FODET_UINT32 ui32ClsfrStatusVal;
    FODET_UINT32 ui32ClsfrCascadeImgAddr;
    FODET_UINT32 ui32ClsfrHidCascadeOutAddr;
    FODET_IMG_INFO ClsfrIntgralImgOut;
    FODET_BUS_CTRL ClsfrIntgralImgBusCtrl;
    FODET_CLSFR_SCALE_SIZE ClsfrScaleSize;
    FODET_UINT16 ui16ReciprocalWndArea;
    FODET_CLSFR_WND_LMT stClsfrWndLmt;
    FODET_CLSFR_CTRL stFodetClsfrCtrl;
    FODET_CACHE_INFO IntgrlImgCache;
    FODET_UINT8 ui8WinScanMode=FODET_WIN_SCAN_SMALL2LARGE;     // 1: from small to large, 0: from large to small
    FODET_UINT16 StartIdx, EndIdx;

    if(ui8WinScanMode){
        StartIdx = pFodetUser->ui16StartIndex;
        EndIdx = pFodetUser->ui16EndIndex;
    }else{
        StartIdx = pFodetUser->ui16EndIndex;
        EndIdx = pFodetUser->ui16StartIndex;
    }

    // AXI IDS
    FodetAxi.ui8IMCID = 4;
    FodetAxi.ui8HCID = 3;
    FodetAxi.ui8CID = 2;
    FodetAxi.ui8SRCID = 1;
    FodetAxi.ui8FWID = 6;
    FodetAxi.ui8HCCID = 5;
    ret += FodetAxiCtrlSet(FodetAxi);

    // Reset , Enable Master
    stFodetCacheCtrl.ui32Ctrl = 0;
    ret += FodetMasterReset(stFodetCacheCtrl);
    ret += FodetMasterEnable(&stFodetCacheCtrl);
    // Reset Classifier
    FodetClsfrReset();

#if 1
    /* Intgral Image cache */
    IntgrlImgCache.ui32Addr = pFodetUser->ui32IISumAddr+0x00018000;
    FodetIntgrlImgCacheAddrSet(IntgrlImgCache.ui32Addr);
    IntgrlImgCache.ui16Stride = pFodetUser->ui16IIoutStride;
    IntgrlImgCache.ui16Offset = pFodetUser->ui16IIoutOffset;
    FodetIntgrlImgCacheStrideOffsetSet(IntgrlImgCache.ui16Stride, IntgrlImgCache.ui16Offset);
    IntgrlImgCache.ui32LoadAddr = pFodetUser->ui32IISumAddr+0x00018000;
    FodetIntgrlImgCacheLoadAddrSet(IntgrlImgCache.ui32LoadAddr);
    IntgrlImgCache.ui32LoadLocalAddr = (FODET_UINT32)g_ioIRamVirtBaseAddr;
    FodetIntgrlImgCacheLoadLocalAddrSet(IntgrlImgCache.ui32LoadLocalAddr);
    IntgrlImgCache.ui32LoadLocalLength = 0x00018000;
    FodetIntgrlImgCacheLoadLocalLengthSet(IntgrlImgCache.ui32LoadLocalLength);
#if 0
    FODET_MSG_LOG("<@98@>=[%08X]\n",FODET_READL(0x0098));
    FODET_MSG_LOG("<@9C@>=[%08X]\n",FODET_READL(0x009C));
    FODET_MSG_LOG("<@A0@>=[%08X]\n",FODET_READL(0x00a0));
    FODET_MSG_LOG("<@A4@>=[%08X]\n",FODET_READL(0x00a4));
#endif
    stFodetCacheCtrl.ui32Ctrl = 0;
    stFodetCacheCtrl.ui1HoldAftrRenew = 1;	//[14] set renew hold
    stFodetCacheCtrl.ui1RstIntgrlImgCache = 1;	//[12] reset integral image cache
    stFodetCacheCtrl.ui1FodMstrRst = 1;		//[1] reset master (RD fifo)
    FodetCacheCtrlSet(stFodetCacheCtrl);
    ui32TimeOut = 1000;
    stFodetCacheCtrl.ui32Ctrl = 0;
    stFodetCacheCtrl.ui1HoldAftrRenew = 1;	//[14] set renew hold
    stFodetCacheCtrl.ui1LdIntgrlImgCache = 1;	//[10] enable load integral image cache
    stFodetCacheCtrl.ui1FodMstrEn = 1;		//[0] enable master (load)
    FodetCacheCtrlSet(stFodetCacheCtrl);
    //    fodet_wait_event();
    fodet_wait_event_timeout(ui32TimeOut);
    stFodetCacheCtrl.ui32Ctrl = 0;
    stFodetCacheCtrl.ui1HoldAftrRenew = 1;	//[14] set renew hold
    stFodetCacheCtrl.ui1RstIntgrlImgCache = 1;	//[12] reset integral image cache
    stFodetCacheCtrl.ui1FodMstrRst = 1;		//[1] reset master (RD fifo)
    FodetCacheCtrlSet(stFodetCacheCtrl);
    stFodetCacheCtrl.ui32Ctrl = 0;
    stFodetCacheCtrl.ui1HoldAftrRenew = 1;	//[14] set renew hold
    FodetCacheCtrlSet(stFodetCacheCtrl);
#endif  /* Intgral Image cache */

    FodetIntgrlImgCtrl.ui32Ctrl = 0;
    FodetIntgrlImgCtrl.ui1SrcImgFmt = pFodetUser->ui16ImgFormat;
    ret += FodetIntgrlImgCtrlSet(FodetIntgrlImgCtrl);
    if(ui32Trigger & 0x00000008){
        ui8ClsfrStatusIdx = 7;		// debug_index = 7.
        ret += FodetClsfrStatusIdxSet(ui8ClsfrStatusIdx);
        ui32ClsfrStatusVal = 0;		// debug_value = 0.
        ret += FodetClsfrStatusValChk(ui32ClsfrStatusVal);
        FODET_MSG_LOG("<@44@>=[07], <@48@>=[%08X]\n",FODET_READL(0x0048));
    }       
    ui32ClsfrCascadeImgAddr = pFodetUser->ui32DatabaseAddr;
    ret += FodetClsfrCascadeImgAddrSet(ui32ClsfrCascadeImgAddr);
    ui32ClsfrHidCascadeOutAddr = pFodetUser->ui32HIDCCAddr;
    ret += FodetClsfrHidCascadeOutAddrSet(ui32ClsfrHidCascadeOutAddr);
    ClsfrIntgralImgOut.ui32Addr = pFodetUser->ui32IISumAddr;
    ret += FodetClsfrIntgrlImgOutAddrSet(ClsfrIntgralImgOut.ui32Addr);
    ClsfrIntgralImgOut.ui16Stride = pFodetUser->ui16IIoutStride;
    ClsfrIntgralImgOut.ui16Offset = pFodetUser->ui16IIoutOffset;
    ret += FodetClsfrIntgrlImgOutStrideOffsetSet(ClsfrIntgralImgOut.ui16Stride, ClsfrIntgralImgOut.ui16Offset);
    ClsfrIntgralImgBusCtrl.ui8MemWriteBurstSize = 0;
    ClsfrIntgralImgBusCtrl.ui8MemWriteThreshold = 15;
    ClsfrIntgralImgBusCtrl.ui8MemReadThreshold = 15;
    ret += FodetClsfrIntgrlImgBusCtrlSet(ClsfrIntgralImgBusCtrl.ui8MemWriteBurstSize, ClsfrIntgralImgBusCtrl.ui8MemWriteThreshold, ClsfrIntgralImgBusCtrl.ui8MemReadThreshold);
#if 0
    FODET_MSG_LOG("<@24@>=[%08X]\n",FODET_READL(0x0024));
    FODET_MSG_LOG("<@28@>=[%08X]\n",FODET_READL(0x0028));
    FODET_MSG_LOG("<@2c@>=[%08X]\n",FODET_READL(0x002c));
    FODET_MSG_LOG("<@30@>=[%08X]\n",FODET_READL(0x0030));
    FODET_MSG_LOG("<@34@>=[%08X]\n",FODET_READL(0x0034));
#endif

    i8FDIndex = StartIdx;
    while(1)
    {
        if(ui8WinScanMode){
            if(i8FDIndex > EndIdx)
                break;
        }else{
            if(i8FDIndex < EndIdx)
                break;
        }
        ui16EqWinWd = stOR[i8FDIndex].eq_win_wd;
        ui16EqWinHt = stOR[i8FDIndex].eq_win_ht;
        ui16ShiftStep = stOR[i8FDIndex].step;
        ui32InvWin = 1048576 /(ui16EqWinWd * ui16EqWinHt);    // 2^20 =1048576
        if(pFodetUser->ui32ImgOffset != 0){
            ui16StopWd = (FODET_UINT16)(((((pFodetUser->ui16CropWidth - ui16EqWinWd) *10)  / ui16ShiftStep)+ 0) / 10);
            ui16StopHt = (FODET_UINT16)(((((pFodetUser->ui16CropHeight - ui16EqWinHt) *10)  / ui16ShiftStep)+ 0) / 10);        
        }else{
            ui16StopWd = (FODET_UINT16)(((((pFodetUser->ui16ImgWidth - ui16EqWinWd) *10)  / ui16ShiftStep)+ 0) / 10);
            ui16StopHt = (FODET_UINT16)(((((pFodetUser->ui16ImgHeight - ui16EqWinHt) *10)  / ui16ShiftStep)+ 0) / 10);
        }
        ui16RealWinWd= stOR[i8FDIndex].real_wd;
        ui16RealWinHt= stOR[i8FDIndex].real_ht;
        ui16Xost = pFodetUser->ui16CropXost;
        ui16Yost = pFodetUser->ui16CropYost;

        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1HoldAftrRenew = 1;
        stFodetCacheCtrl.ui1FodMstrEn = 1;
        FodetCacheCtrlSet(stFodetCacheCtrl);
        ClsfrScaleSize.ui14ScalingFactor = stOR[i8FDIndex].scaling_factor;  //g_wScalingFactor[i8FDIndex];
        ClsfrScaleSize.ui8EqWinWidth = ui16EqWinWd;
        ClsfrScaleSize.ui8EqWinHeight = ui16EqWinHt;
        FodetClsfrIntgrlImgScaleSizeSet(ClsfrScaleSize.ui14ScalingFactor, ClsfrScaleSize.ui8EqWinWidth, ClsfrScaleSize.ui8EqWinHeight);
        ui16ReciprocalWndArea = ui32InvWin;
        FodetClsfrReciprocalWndAreaSet(ui16ReciprocalWndArea);
        stClsfrWndLmt.ui14Step = (ui16ShiftStep<<10);
        stClsfrWndLmt.ui8StopWidth = ui16StopWd;
        stClsfrWndLmt.ui7StopHeight = ui16StopHt;
        FodetClsfrWndLmtSet(stClsfrWndLmt);
#if 0
        FODET_MSG_LOG("Index=(%d)\n",i8FDIndex);
        FODET_MSG_LOG("SF=[%X], EqWin=(%d), Step=(%d), InvWin=[%x], StopWH=(%d,%d), RealWin=(%d)\n",
           g_wScalingFactor[i8FDIndex], ui16EqWin, ui16ShiftStep, ui32InvWin, ui16StopWd,ui16StopHt, ui16RealWin);
        FODET_MSG_LOG("<@90@>=[%08X]\n",FODET_READL(0x0090));
        FODET_MSG_LOG("<@38@>=[%08X]\n",FODET_READL(0x0038));
        FODET_MSG_LOG("<@3C@>=[%08X]\n",FODET_READL(0x003c));
        FODET_MSG_LOG("<@40@>=[%08X]\n",FODET_READL(0x0040));
#endif
        stFodetClsfrCtrl.ui32Ctrl = 0;
        ui32TimeOut=1000;
        FodetClsfrTrigger(stFodetClsfrCtrl);
        //  fodet_wait_event();
        fodet_wait_event_timeout(ui32TimeOut);

#if 1
        /* HID cache */
        IntgrlImgCache.ui32HidCascadeCacheAddr = pFodetUser->ui32HIDCCAddr;
        FodetHidCascadeCacheAddrSet(IntgrlImgCache.ui32HidCascadeCacheAddr);
        IntgrlImgCache.ui32LoadAddr = pFodetUser->ui32HIDCCAddr;
        FodetIntgrlImgCacheLoadAddrSet(IntgrlImgCache.ui32LoadAddr);
        IntgrlImgCache.ui32LoadLocalAddr = (FODET_UINT32)(g_ioIRamVirtBaseAddr+0x00018000);
        FodetIntgrlImgCacheLoadLocalAddrSet(IntgrlImgCache.ui32LoadLocalAddr);
        IntgrlImgCache.ui32LoadLocalLength = 0x00002000;
        FodetIntgrlImgCacheLoadLocalLengthSet(IntgrlImgCache.ui32LoadLocalLength);
#if 0
        FODET_MSG_LOG("<@B0@>=[%08X]\n",FODET_READL(0x00B0));
        FODET_MSG_LOG("<@9C@>=[%08X]\n",FODET_READL(0x009C));
        FODET_MSG_LOG("<@A0@>=[%08X]\n",FODET_READL(0x00a0));
        FODET_MSG_LOG("<@A4@>=[%08X]\n",FODET_READL(0x00a4));
#endif
        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1HoldAftrRenew = 1;	        //[14] set renew hold
        stFodetCacheCtrl.ui1RstHidCascadeCache = 1;	//[13] reset Hid-Cascade cache
        stFodetCacheCtrl.ui1FodMstrRst = 1;		        //[1] reset master
        FodetCacheCtrlSet(stFodetCacheCtrl);
        ui32TimeOut = 1000;
        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1HoldAftrRenew = 1;	        //[14] set renew hold
        stFodetCacheCtrl.ui1LdHidCascadeCache = 1;	//[11] enable load Hid-Cascade cache
        stFodetCacheCtrl.ui1FodMstrEn = 1;		        //[0] enable master (load)
        FodetCacheCtrlSet(stFodetCacheCtrl);
        //  fodet_wait_event();
        fodet_wait_event_timeout(ui32TimeOut);
        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1HoldAftrRenew = 1;	        //[14] set renew hold
        stFodetCacheCtrl.ui1RstHidCascadeCache = 1;	//[13] reset Hid-Cascade cache
        stFodetCacheCtrl.ui1FodMstrRst = 1;		        //[1] reset master (RD fifo)
        FodetCacheCtrlSet(stFodetCacheCtrl);
        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1HoldAftrRenew = 1;	        //[14] set renew hold
        stFodetCacheCtrl.ui1FodMstrEn = 1;		        //[0] enable master (RD fifo)
        FodetCacheCtrlSet(stFodetCacheCtrl);
        // enable cache
        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1EnIntgrlImgCache = 1;	//[8] enable integral image cache
        stFodetCacheCtrl.ui1EnHidCascadeCache=1;     //[9] enable Cascade cache
        stFodetCacheCtrl.ui1FodMstrEn = 1;		        //[0] enable master (RD HID)
        FodetCacheCtrlSet(stFodetCacheCtrl);
#endif // cache

        stFodetClsfrCtrl.ui32Ctrl = FODET_READL(0x0020) & 0x000000ff;
        FODET_MSG_LOG("0x20[%08X]\n", stFodetClsfrCtrl.ui32Ctrl);
        ui32TimeOut = 500;  //300;  //500;
        FodetClsfrResumeAndTrigger(stFodetClsfrCtrl);
        while(1)
        {
            fodet_wait_event_timeout(ui32TimeOut);
            stFodetClsfrCtrl.ui32Ctrl = FodetClsfrCtrlGet();
            //==== Detected & Hold ======//
            if(stFodetClsfrCtrl.ui1ResumeHold || stFodetClsfrCtrl.ui1ResetDetected)
            {		
            	// detected valid when hold be set.
                if(stFodetClsfrCtrl.ui1ResumeHold && stFodetClsfrCtrl.ui1ResetDetected
				                                  && (iCandidateCount < MAX_ARY_SCAN_ALL)){
                    avgComps1[iCandidateCount].rect.x = (int)(stFodetClsfrCtrl.ui9XCoordinate + ui16Xost);
                    avgComps1[iCandidateCount].rect.y = (int)(stFodetClsfrCtrl.ui8YCoordinate + ui16Yost);
                    avgComps1[iCandidateCount].rect.w = (int)ui16RealWinWd;
                    avgComps1[iCandidateCount].rect.h = (int)ui16RealWinHt;
                    FODET_MSG_LOG("Index=(%d),Rect(%d)=(%d,%d,%d,%d)\n", i8FDIndex, iCandidateCount,
                    avgComps1[iCandidateCount].rect.x, avgComps1[iCandidateCount].rect.y,
                    avgComps1[iCandidateCount].rect.w, avgComps1[iCandidateCount].rect.h);

                    iCandidateCount++;
                }else if(stFodetClsfrCtrl.ui1ResumeHold && stFodetClsfrCtrl.ui1ResetDetected){
                    FODET_MSG_ERR("LOSS!!! Index=(%d),(%d,%d,%d,%d)\n", i8FDIndex, 
                                                        (int)(stFodetClsfrCtrl.ui9XCoordinate + ui16Xost), 
                                                        (int)(stFodetClsfrCtrl.ui8YCoordinate + ui16Yost),
                                                        ui16RealWinWd, ui16RealWinHt);
                }

                if(stFodetClsfrCtrl.ui1ResetDetected){
                    FodetCacheCtrlSet(stFodetCacheCtrl);
                    FodetClsfrResumeAndTrigger(stFodetClsfrCtrl);
                }else{
                    FodetClsfrResumeAndTrigger(stFodetClsfrCtrl);
                }
            }
            //==== Scan finish ======//
            if(stFodetClsfrCtrl.ui1EnBusy == FODET_STATUS_DONE)
            {
            	FODET_MSG_LOG("\n This scaling factor finish!!!\n Go on next eq window. \n");
            	break;
            }
        }
        if(ui8WinScanMode)
            i8FDIndex++;
        else
            i8FDIndex--;
    } // for FD Index
    return ret;
}


FODET_INT32 fodet_RenewRun(FODET_USER* pFodetUser)
{
    FODET_INT32 ret=0;
    FODET_UINT32 ui32TimeOut;
//    FODET_UINT32 ui32Temp=0;
    FODET_UINT32 ui32Trigger=0x00000000;
    FODET_INT8 i8FDIndex=0;
    FODET_UINT16 ui16EqWinWd=0;
    FODET_UINT16 ui16EqWinHt=0;
    FODET_UINT32 ui32InvWin=0;
    FODET_UINT16 ui16StopWd, ui16StopHt;
    FODET_UINT16 ui16ShiftStep;
    FODET_UINT16 ui16RealWinWd=0;
    FODET_UINT16 ui16RealWinHt=0;
    FODET_UINT16 ui16Xost, ui16Yost;
    FODET_AXI_ID FodetAxi;
    FODET_CACHE_CTRL stFodetCacheCtrl;
    FODET_INTGRL_IMG_CTRL FodetIntgrlImgCtrl;
    FODET_UINT8 ui8ClsfrStatusIdx;
    FODET_UINT32 ui32ClsfrStatusVal;
    FODET_UINT32 ui32ClsfrCascadeImgAddr;
    FODET_UINT32 ui32ClsfrHidCascadeOutAddr;
    FODET_IMG_INFO ClsfrIntgralImgOut;
    FODET_BUS_CTRL ClsfrIntgralImgBusCtrl;
    FODET_CLSFR_SCALE_SIZE ClsfrScaleSize;
    FODET_UINT16 ui16ReciprocalWndArea;
    FODET_CLSFR_WND_LMT stClsfrWndLmt;
    FODET_CLSFR_CTRL stFodetClsfrCtrl;
    FODET_UINT8 ui8WinScanMode=FODET_WIN_SCAN_SMALL2LARGE;     // 1: from small to large, 0: from large to small
    FODET_UINT16 StartIdx, EndIdx;

    if(ui8WinScanMode){
        StartIdx = pFodetUser->ui16StartIndex;
        EndIdx = pFodetUser->ui16EndIndex;
    }else{
        StartIdx = pFodetUser->ui16EndIndex;
        EndIdx = pFodetUser->ui16StartIndex;
    }

    // AXI IDS
    FodetAxi.ui8IMCID = 4;
    FodetAxi.ui8HCID = 3;
    FodetAxi.ui8CID = 2;
    FodetAxi.ui8SRCID = 1;
    FodetAxi.ui8FWID = 6;
    FodetAxi.ui8HCCID = 5;
    ret += FodetAxiCtrlSet(FodetAxi);

    // Reset , Enable Master
    stFodetCacheCtrl.ui32Ctrl = 0;
    ret += FodetMasterReset(stFodetCacheCtrl);
    ret += FodetMasterEnable(&stFodetCacheCtrl);
    // Reset Classifier
    FodetClsfrReset();

    FodetIntgrlImgCtrl.ui32Ctrl = 0;
    FodetIntgrlImgCtrl.ui1SrcImgFmt = pFodetUser->ui16ImgFormat;
    ret += FodetIntgrlImgCtrlSet(FodetIntgrlImgCtrl);
    if(ui32Trigger & 0x00000008){
        ui8ClsfrStatusIdx = 7;		// debug_index = 7.
        ret += FodetClsfrStatusIdxSet(ui8ClsfrStatusIdx);
        ui32ClsfrStatusVal = 0;	        // debug_value = 0.
        ret += FodetClsfrStatusValChk(ui32ClsfrStatusVal);
        FODET_MSG_LOG("<@44@>=[07], <@48@>=[%08X]\n",FODET_READL(0x0048));
    }       

    ui32ClsfrCascadeImgAddr = pFodetUser->ui32DatabaseAddr;
    ret += FodetClsfrCascadeImgAddrSet(ui32ClsfrCascadeImgAddr);
    ui32ClsfrHidCascadeOutAddr = pFodetUser->ui32HIDCCAddr;
    ret += FodetClsfrHidCascadeOutAddrSet(ui32ClsfrHidCascadeOutAddr);
    ClsfrIntgralImgOut.ui32Addr = pFodetUser->ui32IISumAddr;
    ret += FodetClsfrIntgrlImgOutAddrSet(ClsfrIntgralImgOut.ui32Addr);
    ClsfrIntgralImgOut.ui16Stride = pFodetUser->ui16IIoutStride;
    ClsfrIntgralImgOut.ui16Offset = pFodetUser->ui16IIoutOffset;
    ret += FodetClsfrIntgrlImgOutStrideOffsetSet(ClsfrIntgralImgOut.ui16Stride, ClsfrIntgralImgOut.ui16Offset);
    ClsfrIntgralImgBusCtrl.ui8MemWriteBurstSize = 0;
    ClsfrIntgralImgBusCtrl.ui8MemWriteThreshold = 15;
    ClsfrIntgralImgBusCtrl.ui8MemReadThreshold = 15;
    ret += FodetClsfrIntgrlImgBusCtrlSet(ClsfrIntgralImgBusCtrl.ui8MemWriteBurstSize, ClsfrIntgralImgBusCtrl.ui8MemWriteThreshold, ClsfrIntgralImgBusCtrl.ui8MemReadThreshold);
#if 0
    FODET_MSG_LOG("<@24@>=[%08X]\n",FODET_READL(0x0024));
    FODET_MSG_LOG("<@28@>=[%08X]\n",FODET_READL(0x0028));
    FODET_MSG_LOG("<@2c@>=[%08X]\n",FODET_READL(0x002c));
    FODET_MSG_LOG("<@30@>=[%08X]\n",FODET_READL(0x0030));
    FODET_MSG_LOG("<@34@>=[%08X]\n",FODET_READL(0x0034));
#endif

    i8FDIndex = StartIdx;
    while(1)
    {
        if(ui8WinScanMode){
            if(i8FDIndex > EndIdx)
                break;
        }else{
            if(i8FDIndex < EndIdx)
                break;
        }
        ui16EqWinWd=  stOR[i8FDIndex].eq_win_wd;
        ui16EqWinHt =  stOR[i8FDIndex].eq_win_ht;
        ui16ShiftStep = stOR[i8FDIndex].step;
        ui32InvWin = 1048576 /(ui16EqWinWd * ui16EqWinHt);    // 2^20 =1048576
        if(pFodetUser->ui32ImgOffset != 0){
            ui16StopWd = (FODET_UINT16)(((((pFodetUser->ui16CropWidth - ui16EqWinWd) *10)  / ui16ShiftStep)+ 0) / 10);
            ui16StopHt = (FODET_UINT16)(((((pFodetUser->ui16CropHeight - ui16EqWinHt) *10)  / ui16ShiftStep)+ 0) / 10);        
        }else{
            ui16StopWd = (FODET_UINT16)(((((pFodetUser->ui16ImgWidth - ui16EqWinWd) *10)  / ui16ShiftStep)+ 0) / 10);
            ui16StopHt = (FODET_UINT16)(((((pFodetUser->ui16ImgHeight - ui16EqWinHt) *10)  / ui16ShiftStep)+ 0) / 10);
        }
        ui16RealWinWd= stOR[i8FDIndex].real_wd;
        ui16RealWinHt= stOR[i8FDIndex].real_ht;
        ui16Xost = pFodetUser->ui16CropXost;
        ui16Yost = pFodetUser->ui16CropYost;

        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1HoldAftrRenew = 1;
        stFodetCacheCtrl.ui1FodMstrEn = 1;
        FodetCacheCtrlSet(stFodetCacheCtrl);
        ClsfrScaleSize.ui14ScalingFactor = stOR[i8FDIndex].scaling_factor;  //g_wScalingFactor[i8FDIndex];
        ClsfrScaleSize.ui8EqWinWidth = ui16EqWinWd;
        ClsfrScaleSize.ui8EqWinHeight = ui16EqWinHt;
        FodetClsfrIntgrlImgScaleSizeSet(ClsfrScaleSize.ui14ScalingFactor, ClsfrScaleSize.ui8EqWinWidth, ClsfrScaleSize.ui8EqWinHeight);
        ui16ReciprocalWndArea = ui32InvWin;
        FodetClsfrReciprocalWndAreaSet(ui16ReciprocalWndArea);
        stClsfrWndLmt.ui14Step = (ui16ShiftStep<<10);
        stClsfrWndLmt.ui8StopWidth = ui16StopWd;
        stClsfrWndLmt.ui7StopHeight = ui16StopHt;
        FodetClsfrWndLmtSet(stClsfrWndLmt);
#if 0
        FODET_MSG_LOG("Index=(%d)\n",i8FDIndex);
        FODET_MSG_LOG("SF=[%X], EqWin=(%d), Step=(%d), InvWin=[%x], StopWH=(%d,%d), RealWin=(%d)\n",
                       g_wScalingFactor[i8FDIndex], ui16EqWin, ui16ShiftStep, ui32InvWin, ui16StopWd,ui16StopHt, ui16RealWin);
        FODET_MSG_LOG("<@90@>=[%08X]\n",FODET_READL(0x0090));
        FODET_MSG_LOG("<@38@>=[%08X]\n",FODET_READL(0x0038));
        FODET_MSG_LOG("<@3C@>=[%08X]\n",FODET_READL(0x003c));
        FODET_MSG_LOG("<@40@>=[%08X]\n",FODET_READL(0x0040));
#endif
        
        stFodetClsfrCtrl.ui32Ctrl = 0;
        // wait renew hold interrupt
        ui32TimeOut=1000;
        FodetClsfrTrigger(stFodetClsfrCtrl);
        //  fodet_wait_event();
        fodet_wait_event_timeout(ui32TimeOut);

        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1FodMstrEn = 1;		        //[0] enable master (RD HID)
        FodetClassifierClearINTandHAR();
        stFodetClsfrCtrl.ui32Ctrl = FODET_READL(0x0020) & 0x000000ff;
        ui32TimeOut = 500;  //300;  //500;
        while(1)
        {
            fodet_wait_event_timeout(ui32TimeOut);
            stFodetClsfrCtrl.ui32Ctrl = FodetClsfrCtrlGet();
            //==== Detected & Hold ======//
            if(stFodetClsfrCtrl.ui1ResumeHold || stFodetClsfrCtrl.ui1ResetDetected)
            {		
            	// detected valid when hold be set.
                if(stFodetClsfrCtrl.ui1ResumeHold && stFodetClsfrCtrl.ui1ResetDetected
                                                                    && (iCandidateCount < MAX_ARY_SCAN_ALL)){
                    avgComps1[iCandidateCount].rect.x = (int)(stFodetClsfrCtrl.ui9XCoordinate + ui16Xost);
                    avgComps1[iCandidateCount].rect.y = (int)(stFodetClsfrCtrl.ui8YCoordinate + ui16Yost);
                    avgComps1[iCandidateCount].rect.w = (int)ui16RealWinWd;
                    avgComps1[iCandidateCount].rect.h = (int)ui16RealWinHt;
                    FODET_MSG_LOG("Index=(%d),Rect(%d)=(%d,%d,%d,%d)\n", i8FDIndex, iCandidateCount,
                    avgComps1[iCandidateCount].rect.x, avgComps1[iCandidateCount].rect.y,
                    avgComps1[iCandidateCount].rect.w, avgComps1[iCandidateCount].rect.h);

                    iCandidateCount++;
                }else if(stFodetClsfrCtrl.ui1ResumeHold && stFodetClsfrCtrl.ui1ResetDetected){
                    FODET_MSG_ERR("LOSS!!! Index=(%d),(%d,%d,%d,%d)\n", i8FDIndex, 
                                                        (int)(stFodetClsfrCtrl.ui9XCoordinate + ui16Xost), 
                                                        (int)(stFodetClsfrCtrl.ui8YCoordinate + ui16Yost),
                                                        ui16RealWinWd, ui16RealWinHt);
                }

                if(stFodetClsfrCtrl.ui1ResetDetected){
                    FodetCacheCtrlSet(stFodetCacheCtrl);
                    FodetClsfrResumeAndTrigger(stFodetClsfrCtrl);
                }else{
                    FodetClsfrResumeAndTrigger(stFodetClsfrCtrl);
                }
            }
            //==== Scan finish ======//
            if(stFodetClsfrCtrl.ui1EnBusy == FODET_STATUS_DONE)
            {
            	FODET_MSG_LOG("\n This scaling factor finish!!!\n Go on next eq window. \n");
            	break;
            }
        }
        if(ui8WinScanMode)
            i8FDIndex++;
        else
            i8FDIndex--;
    }
    return ret;
}


FODET_INT32 fodet_IntegralImage_polling(FODET_USER* pFodetUser)
{
    FODET_INT32 ret=0;
    FODET_UINT32 ui32LoopTimeOut=0;
    FODET_INTGRL_IMG_INFO stIntgrlImgInfo;
    FODET_AXI_ID stAxiId;

    stAxiId.ui8IMCID = 4;
    stAxiId.ui8HCID = 3;
    stAxiId.ui8CID = 2;
    stAxiId.ui8SRCID = 1;
    stAxiId.ui8FWID = 6;
    stAxiId.ui8HCCID = 5;
    FodetAxiCtrlSet(stAxiId);

    // Reset , Enable master
    stIntgrlImgInfo.stCacheCtrl.ui32Ctrl = 0;
    ret += FodetMasterReset(stIntgrlImgInfo.stCacheCtrl);
    ret += FodetMasterEnable(&stIntgrlImgInfo.stCacheCtrl);
    // Reset Classifier
    FodetClsfrReset();

    if(pFodetUser->ui32ImgOffset != 0)
    {
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui32Addr = pFodetUser->ui32SrcImgAddr + pFodetUser->ui32ImgOffset;
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui16Width = pFodetUser->ui16CropWidth;
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui16Height = pFodetUser->ui16CropHeight;
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui16Stride = pFodetUser->ui16ImgStride;
    }else{
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui32Addr = pFodetUser->ui32SrcImgAddr;
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui16Width = pFodetUser->ui16ImgWidth;
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui16Height = pFodetUser->ui16ImgHeight;
        stIntgrlImgInfo.stIntgrlImgSrcInfo.ui16Stride = pFodetUser->ui16ImgStride;
    }
    stIntgrlImgInfo.stIntgrlImgOutInfo.ui32Addr = pFodetUser->ui32IISumAddr;
    stIntgrlImgInfo.stIntgrlImgOutInfo.ui16Stride = pFodetUser->ui16IIoutStride;
    stIntgrlImgInfo.stIntgrlImgOutInfo.ui16Offset = pFodetUser->ui16IIoutOffset;
    stIntgrlImgInfo.stIntgrlImgBusCtrl.ui8MemWriteBurstSize = 15;
    stIntgrlImgInfo.stIntgrlImgBusCtrl.ui8MemWriteThreshold = 15;
    stIntgrlImgInfo.stIntgrlImgBusCtrl.ui8MemReadThreshold = 15;	
    ret += FodetIntgrlImgSet(stIntgrlImgInfo);

    #if 0
    FODET_MSG_LOG("IntegralImage %s-%s\n",__DATE__, __TIME__ );
    FODET_MSG_LOG("<@04@>[%08X]\n",FODET_READL(0x0004));
    FODET_MSG_LOG("<@08@>[%08X]\n",FODET_READL(0x0008));
    FODET_MSG_LOG("<@0c@>[%08X]\n",FODET_READL(0x000c));
    FODET_MSG_LOG("<@10@>[%08X]\n",FODET_READL(0x0010));
    FODET_MSG_LOG("<@14@>[%08X]\n",FODET_READL(0x0014));
    FODET_MSG_LOG("<@18@>[%08X]\n",FODET_READL(0x0018));
    #endif


    // Integral Image - Sum
    stIntgrlImgInfo.stIntgrlImgCtrl.ui32Ctrl = 0;
    stIntgrlImgInfo.stIntgrlImgCtrl.ui1SrcImgFmt = pFodetUser->ui16ImgFormat;
    ret += FodetIntgrlImgCtrlSet(stIntgrlImgInfo.stIntgrlImgCtrl);
    ui32LoopTimeOut=0x01000000;
    FodetIntgrlImgTrigger(stIntgrlImgInfo.stIntgrlImgCtrl);
    while(1){
        if((FODET_READL(0x0000) & FOD_STATUS_BUSY) == FOD_STATUS_DONE)
        {
            FODET_MSG_LOG("II-Sum finished!-check busy and done.\n");
            break;
        }
        ui32LoopTimeOut--;
        if(!ui32LoopTimeOut){
            FODET_MSG_ERR("\n ERROR : Sum Time Out \n");
            return-10;
        }
    }
    ret += FodetIntStatusChk(FODET_INT_STATUS_ON, stIntgrlImgInfo.stCacheCtrl);
    ret += FodetCacheCtrlSet(stIntgrlImgInfo.stCacheCtrl);
    ret += FodetIntStatusChk(FODET_INT_STATUS_OFF, stIntgrlImgInfo.stCacheCtrl);

    //Integral Image - Sqsum
    stIntgrlImgInfo.stIntgrlImgCtrl.ui2IntgrlImgMode = INTGRL_IMG_MODE_SQ_SUM;
    ret += FodetIntgrlImgCtrlSet(stIntgrlImgInfo.stIntgrlImgCtrl);
    ui32LoopTimeOut=0x01000000;
    FodetIntgrlImgTrigger(stIntgrlImgInfo.stIntgrlImgCtrl);
    while (1) {
        if((FODET_READL(0x0000) & FOD_STATUS_BUSY) == FOD_STATUS_DONE)
        {
            FODET_MSG_LOG("II-Sqsum finished!-check busy and done.\n");
            break;
        }
        ui32LoopTimeOut--;
        if(!ui32LoopTimeOut){
            FODET_MSG_ERR("\n ERROR : Sqsum Time Out \n");
            return-20;
        }
    }
    ret += FodetIntStatusChk(FODET_INT_STATUS_ON, stIntgrlImgInfo.stCacheCtrl);
    ret += FodetCacheCtrlSet(stIntgrlImgInfo.stCacheCtrl);
    ret += FodetIntStatusChk(FODET_INT_STATUS_OFF, stIntgrlImgInfo.stCacheCtrl);

#ifdef TURN_ON_TILTED_SUM
    //Integral Image - Tilted Sum
    stIntgrlImgInfo.stIntgrlImgCtrl.ui1IntgrlImgMode = INTGRL_IMG_MODE_TILTED_SUM;
    ret += FodetIntgrlImgCtrlSet(stIntgrlImgInfo.stIntgrlImgCtrl);
    ui32LoopTimeOut=0x01000000;
    FodetIntgrlImgTrigger(stIntgrlImgInfo.stIntgrlImgCtrl);
    while (1) {
        if((FODET_READL(0x0000) & FOD_STATUS_BUSY) == FOD_STATUS_DONE)
        {
            FODET_MSG_LOG("II-Tilted Sum finished!-check busy and done.\n");
            break;
        }
        ui32LoopTimeOut--;
        if(!ui32LoopTimeOut){
            FODET_MSG_ERR("\n ERROR : Tilted Sum Time Out \n");
            return-30;
        }
    }
    ret += FodetIntStatusChk(FODET_INT_STATUS_ON, stIntgrlImgInfo.stCacheCtrl);
    ret += FodetCacheCtrlSet(stIntgrlImgInfo.stCacheCtrl);
    ret += FodetIntStatusChk(FODET_INT_STATUS_OFF, stIntgrlImgInfo.stCacheCtrl);
#endif  // TURN_ON_TILTED_SUM

    return ret;
}


FODET_INT32 fodet_RenewRunWithCache_polling(FODET_USER* pFodetUser)
{
    FODET_INT32 ret=0;
    FODET_UINT32 ui32TimeOut;
    FODET_UINT32 ui32Temp=0;
    FODET_UINT32 ui32Trigger=0x00000000;
    FODET_INT8 i8FDIndex=0;
    FODET_UINT16 ui16EqWinWd=0;
    FODET_UINT16 ui16EqWinHt=0;
    FODET_UINT32 ui32InvWin=0;
    FODET_UINT16 ui16StopWd, ui16StopHt;
    FODET_UINT16 ui16ShiftStep;
    FODET_UINT16 ui16RealWinWd=0;
    FODET_UINT16 ui16RealWinHt=0;
    FODET_UINT16 ui16Xost, ui16Yost;
    FODET_AXI_ID FodetAxi;
    FODET_CACHE_CTRL stFodetCacheCtrl;
    FODET_INTGRL_IMG_CTRL FodetIntgrlImgCtrl;
    FODET_UINT8 ui8ClsfrStatusIdx;
    FODET_UINT32 ui32ClsfrStatusVal;
    FODET_UINT32 ui32ClsfrCascadeImgAddr;
    FODET_UINT32 ui32ClsfrHidCascadeOutAddr;
    FODET_IMG_INFO ClsfrIntgralImgOut;
    FODET_BUS_CTRL ClsfrIntgralImgBusCtrl;
    FODET_CLSFR_SCALE_SIZE ClsfrScaleSize;
    FODET_UINT16 ui16ReciprocalWndArea;
    FODET_CLSFR_WND_LMT stClsfrWndLmt;
    FODET_CLSFR_CTRL stFodetClsfrCtrl;
    FODET_CACHE_INFO IntgrlImgCache;
    FODET_UINT8 ui8WinScanMode=FODET_WIN_SCAN_SMALL2LARGE;     // 1: from small to large, 0: from large to small
    FODET_UINT16 StartIdx, EndIdx;

    if(ui8WinScanMode){
        StartIdx = pFodetUser->ui16StartIndex;
        EndIdx = pFodetUser->ui16EndIndex;
    }else{
        StartIdx = pFodetUser->ui16EndIndex;
        EndIdx = pFodetUser->ui16StartIndex;
    }

    // AXI IDS
    FodetAxi.ui8IMCID = 4;
    FodetAxi.ui8HCID = 3;
    FodetAxi.ui8CID = 2;
    FodetAxi.ui8SRCID = 1;
    FodetAxi.ui8FWID = 6;
    FodetAxi.ui8HCCID = 5;
    ret += FodetAxiCtrlSet(FodetAxi);

    // Reset , Enable Master
    stFodetCacheCtrl.ui32Ctrl = 0;
    ret += FodetMasterReset(stFodetCacheCtrl);
    ret += FodetMasterEnable(&stFodetCacheCtrl);
    // Reset Classifier
    FodetClsfrReset();

#if 1
    /* Intgral Image cache */
    IntgrlImgCache.ui32Addr = pFodetUser->ui32IISumAddr+0x00018000;
    FodetIntgrlImgCacheAddrSet(IntgrlImgCache.ui32Addr);
    IntgrlImgCache.ui16Stride = pFodetUser->ui16IIoutStride;
    IntgrlImgCache.ui16Offset = pFodetUser->ui16IIoutOffset;
    FodetIntgrlImgCacheStrideOffsetSet(IntgrlImgCache.ui16Stride, IntgrlImgCache.ui16Offset);
    IntgrlImgCache.ui32LoadAddr = pFodetUser->ui32IISumAddr+0x00018000;
    FodetIntgrlImgCacheLoadAddrSet(IntgrlImgCache.ui32LoadAddr);
    IntgrlImgCache.ui32LoadLocalAddr = (FODET_UINT32)g_ioIRamVirtBaseAddr;
    FodetIntgrlImgCacheLoadLocalAddrSet(IntgrlImgCache.ui32LoadLocalAddr);
    IntgrlImgCache.ui32LoadLocalLength = 0x00018000;
    FodetIntgrlImgCacheLoadLocalLengthSet(IntgrlImgCache.ui32LoadLocalLength);
    #if 0
    FODET_MSG_LOG("<@98@>=[%08X]\n",FODET_READL(0x0098));
    FODET_MSG_LOG("<@9C@>=[%08X]\n",FODET_READL(0x009C));
    FODET_MSG_LOG("<@A0@>=[%08X]\n",FODET_READL(0x00a0));
    FODET_MSG_LOG("<@A4@>=[%08X]\n",FODET_READL(0x00a4));
    #endif
    stFodetCacheCtrl.ui32Ctrl = 0;
    stFodetCacheCtrl.ui1HoldAftrRenew = 1;	//[14] set renew hold
    stFodetCacheCtrl.ui1RstIntgrlImgCache = 1;	//[12] reset integral image cache
    stFodetCacheCtrl.ui1FodMstrRst = 1;		//[1] reset master (RD fifo)
    FodetCacheCtrlSet(stFodetCacheCtrl);
    stFodetCacheCtrl.ui32Ctrl = 0;
    stFodetCacheCtrl.ui1HoldAftrRenew = 1;	//[14] set renew hold
    stFodetCacheCtrl.ui1LdIntgrlImgCache = 1;	//[10] enable load integral image cache
    stFodetCacheCtrl.ui1FodMstrEn = 1;		//[0] enable master (load)
    FodetCacheCtrlSet(stFodetCacheCtrl);
    ui32TimeOut=0x10000000;
    while(1)
    {
        ui32Temp = FODET_READL(FODET_FODCHCTRL);
        if((ui32Temp & FOD_STATUS_INT ) == FOD_STATUS_INT ){
            FODET_MSG_LOG("w-interrupt state <@90@>=[%08X]\n",FODET_READL(0x0090));
            break;
        }
        ui32TimeOut--;
        if(!ui32TimeOut)
        {
            FODET_MSG_ERR("\n ERROR : wait load II-cache interrupt Time Out [%x] \n", FodetCacheCtrlGet());
            return -10;
        }
    }
    stFodetCacheCtrl.ui32Ctrl = 0;
    stFodetCacheCtrl.ui1HoldAftrRenew = 1;	//[14] set renew hold
    stFodetCacheCtrl.ui1RstIntgrlImgCache = 1;	//[12] reset integral image cache
    stFodetCacheCtrl.ui1FodMstrRst = 1;		//[1] reset master (RD fifo)
    FodetCacheCtrlSet(stFodetCacheCtrl);
    stFodetCacheCtrl.ui32Ctrl = 0;
    stFodetCacheCtrl.ui1HoldAftrRenew = 1;	//[14] set renew hold
    FodetCacheCtrlSet(stFodetCacheCtrl);
#endif


    FodetIntgrlImgCtrl.ui32Ctrl = 0;
    FodetIntgrlImgCtrl.ui1SrcImgFmt = pFodetUser->ui16ImgFormat;
    ret += FodetIntgrlImgCtrlSet(FodetIntgrlImgCtrl);
    if(ui32Trigger & 0x00000008){
        ui8ClsfrStatusIdx = 7;		// debug_index = 7.
        ret += FodetClsfrStatusIdxSet(ui8ClsfrStatusIdx);
        ui32ClsfrStatusVal = 0;		// debug_value = 0.
        ret += FodetClsfrStatusValChk(ui32ClsfrStatusVal);
        FODET_MSG_LOG("<@44@>=[07], <@48@>=[%08X]\n",FODET_READL(0x0048));
    }       
    ui32ClsfrCascadeImgAddr = pFodetUser->ui32DatabaseAddr;
    ret += FodetClsfrCascadeImgAddrSet(ui32ClsfrCascadeImgAddr);
    ui32ClsfrHidCascadeOutAddr = pFodetUser->ui32HIDCCAddr;
    ret += FodetClsfrHidCascadeOutAddrSet(ui32ClsfrHidCascadeOutAddr);
    ClsfrIntgralImgOut.ui32Addr = pFodetUser->ui32IISumAddr;
    ret += FodetClsfrIntgrlImgOutAddrSet(ClsfrIntgralImgOut.ui32Addr);
    ClsfrIntgralImgOut.ui16Stride = pFodetUser->ui16IIoutStride;
    ClsfrIntgralImgOut.ui16Offset = pFodetUser->ui16IIoutOffset;
    ret += FodetClsfrIntgrlImgOutStrideOffsetSet(ClsfrIntgralImgOut.ui16Stride, ClsfrIntgralImgOut.ui16Offset);
    ClsfrIntgralImgBusCtrl.ui8MemWriteBurstSize = 0;
    ClsfrIntgralImgBusCtrl.ui8MemWriteThreshold = 15;
    ClsfrIntgralImgBusCtrl.ui8MemReadThreshold = 15;
    ret += FodetClsfrIntgrlImgBusCtrlSet(ClsfrIntgralImgBusCtrl.ui8MemWriteBurstSize, ClsfrIntgralImgBusCtrl.ui8MemWriteThreshold, ClsfrIntgralImgBusCtrl.ui8MemReadThreshold);
    #if 0
    FODET_MSG_LOG("<@24@>=[%08X]\n",FODET_READL(0x0024));
    FODET_MSG_LOG("<@28@>=[%08X]\n",FODET_READL(0x0028));
    FODET_MSG_LOG("<@2c@>=[%08X]\n",FODET_READL(0x002c));
    FODET_MSG_LOG("<@30@>=[%08X]\n",FODET_READL(0x0030));
    FODET_MSG_LOG("<@34@>=[%08X]\n",FODET_READL(0x0034));
    #endif

    i8FDIndex = StartIdx;
    while(1)
    {
        if(ui8WinScanMode){
            if(i8FDIndex > EndIdx)
                break;
        }else{
            if(i8FDIndex < EndIdx)
                break;
        }
        ui16EqWinWd=  stOR[i8FDIndex].eq_win_wd;
        ui16EqWinHt =  stOR[i8FDIndex].eq_win_ht;
        ui16ShiftStep = stOR[i8FDIndex].step;
        ui32InvWin = 1048576 /(ui16EqWinWd * ui16EqWinHt);    // 2^20 =1048576
        if(pFodetUser->ui32ImgOffset != 0){
            ui16StopWd = (FODET_UINT16)(((((pFodetUser->ui16CropWidth - ui16EqWinWd) *10)  / ui16ShiftStep)+ 0) / 10);
            ui16StopHt = (FODET_UINT16)(((((pFodetUser->ui16CropHeight - ui16EqWinHt) *10)  / ui16ShiftStep)+ 0) / 10);        
        }else{
            ui16StopWd = (FODET_UINT16)(((((pFodetUser->ui16ImgWidth - ui16EqWinWd) *10)  / ui16ShiftStep)+ 0) / 10);
            ui16StopHt = (FODET_UINT16)(((((pFodetUser->ui16ImgHeight - ui16EqWinHt) *10)  / ui16ShiftStep)+ 0) / 10);
        }
        ui16RealWinWd= stOR[i8FDIndex].real_wd;
        ui16RealWinHt= stOR[i8FDIndex].real_ht;
        ui16Xost = pFodetUser->ui16CropXost;
        ui16Yost = pFodetUser->ui16CropYost;

        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1HoldAftrRenew = 1;
        stFodetCacheCtrl.ui1FodMstrEn = 1;
        FodetCacheCtrlSet(stFodetCacheCtrl);
        ClsfrScaleSize.ui14ScalingFactor = stOR[i8FDIndex].scaling_factor;  //g_wScalingFactor[i8FDIndex];
        ClsfrScaleSize.ui8EqWinWidth = ui16EqWinWd;
        ClsfrScaleSize.ui8EqWinHeight = ui16EqWinHt;
        FodetClsfrIntgrlImgScaleSizeSet(ClsfrScaleSize.ui14ScalingFactor, ClsfrScaleSize.ui8EqWinWidth, ClsfrScaleSize.ui8EqWinHeight);
        ui16ReciprocalWndArea = ui32InvWin;
        FodetClsfrReciprocalWndAreaSet(ui16ReciprocalWndArea);
        stClsfrWndLmt.ui14Step = (ui16ShiftStep<<10);
        stClsfrWndLmt.ui8StopWidth = ui16StopWd;
        stClsfrWndLmt.ui7StopHeight = ui16StopHt;
        FodetClsfrWndLmtSet(stClsfrWndLmt);
#if 0
        FODET_MSG_LOG("Index=(%d)\n",i8FDIndex);
        FODET_MSG_LOG("SF=[%X], EqWin=(%d), Step=(%d), InvWin=[%x], StopWH=(%d,%d), RealWin=(%d)\n",
                       g_wScalingFactor[i8FDIndex], ui16EqWin, ui16ShiftStep, ui32InvWin, ui16StopWd,ui16StopHt, ui16RealWin);
        FODET_MSG_LOG("<@90@>=[%08X]\n",FODET_READL(0x0090));
        FODET_MSG_LOG("<@38@>=[%08X]\n",FODET_READL(0x0038));
        FODET_MSG_LOG("<@3C@>=[%08X]\n",FODET_READL(0x003c));
        FODET_MSG_LOG("<@40@>=[%08X]\n",FODET_READL(0x0040));
#endif
        // wait renew hold interrupt
        stFodetClsfrCtrl.ui32Ctrl = 0;
        ui32TimeOut=0x01000000;
        FodetClsfrTrigger(stFodetClsfrCtrl);
        while(1)
        {
            ui32Temp = FODET_READL(FODET_FODCCTRL);
            if((ui32Temp & FOD_STATUS_HOLD) == FOD_STATUS_HOLD){
                FODET_MSG_LOG("w-check hold<@20@>=[%08X]\n",FODET_READL(0x0020));
                break;
            }
            // check hold 0x90 = 0x01004001
            ui32Temp = FODET_READL(FODET_FODCHCTRL);
            if((ui32Temp & (FOD_STATUS_INT | FOD_STATUS_HAR)) == (FOD_STATUS_INT | FOD_STATUS_HAR)){
                FODET_MSG_LOG("w-interrupt state, renew hold<@90@>=[%08X]\n",FODET_READL(0x0090));
                break;
            }
            // time out
            ui32TimeOut--;
            if(!ui32TimeOut)
            {
                FODET_MSG_ERR("\n ERROR : wait  renew hold interrupt Time Out[%x] \n", FodetCacheCtrlGet());
                return -20;
            }
        }

#if 1  /* HID cache */
        IntgrlImgCache.ui32HidCascadeCacheAddr = pFodetUser->ui32HIDCCAddr;
        FodetHidCascadeCacheAddrSet(IntgrlImgCache.ui32HidCascadeCacheAddr);
        IntgrlImgCache.ui32LoadAddr = pFodetUser->ui32HIDCCAddr;
        FodetIntgrlImgCacheLoadAddrSet(IntgrlImgCache.ui32LoadAddr);
        IntgrlImgCache.ui32LoadLocalAddr = (FODET_UINT32)(g_ioIRamVirtBaseAddr+0x00018000);
        FodetIntgrlImgCacheLoadLocalAddrSet(IntgrlImgCache.ui32LoadLocalAddr);
        IntgrlImgCache.ui32LoadLocalLength = 0x00002000;
        FodetIntgrlImgCacheLoadLocalLengthSet(IntgrlImgCache.ui32LoadLocalLength);
#if 0
        FODET_MSG_LOG("<@B0@>=[%08X]\n",FODET_READL(0x00B0));
        FODET_MSG_LOG("<@9C@>=[%08X]\n",FODET_READL(0x009C));
        FODET_MSG_LOG("<@A0@>=[%08X]\n",FODET_READL(0x00a0));
        FODET_MSG_LOG("<@A4@>=[%08X]\n",FODET_READL(0x00a4));
#endif
        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1HoldAftrRenew = 1;	        //[14] set renew hold
        stFodetCacheCtrl.ui1RstHidCascadeCache = 1;	//[13] reset Hid-Cascade cache
        stFodetCacheCtrl.ui1FodMstrRst = 1;		        //[1] reset master
        FodetCacheCtrlSet(stFodetCacheCtrl);
        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1HoldAftrRenew = 1;	        //[14] set renew hold
        stFodetCacheCtrl.ui1LdHidCascadeCache = 1;	//[11] enable load Hid-Cascade cache
        stFodetCacheCtrl.ui1FodMstrEn = 1;		        //[0] enable master (load)
        FodetCacheCtrlSet(stFodetCacheCtrl);
        ui32TimeOut=0x10000000;
        while(1)
        {
            ui32Temp = FODET_READL(FODET_FODCHCTRL);
            if((ui32Temp & FOD_STATUS_INT ) == FOD_STATUS_INT){
                FODET_MSG_LOG("w-interrupt state <@90@>=[%08X]\n",FODET_READL(0x0090));
                break;
            }
            ui32TimeOut--;
            if(!ui32TimeOut)
            {
                FODET_MSG_ERR("\n ERROR : wait  load hid-cache interrupt Time Out[%x] \n", FodetCacheCtrlGet());
                return -30;
            }
        }
        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1HoldAftrRenew = 1;	        //[14] set renew hold
        stFodetCacheCtrl.ui1RstHidCascadeCache = 1;	//[13] reset Hid-Cascade cache
        stFodetCacheCtrl.ui1FodMstrRst = 1;		        //[1] reset master (RD fifo)
        FodetCacheCtrlSet(stFodetCacheCtrl);
        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1HoldAftrRenew = 1;	        //[14] set renew hold
        stFodetCacheCtrl.ui1FodMstrEn = 1;		        //[0] enable master (RD fifo)
        FodetCacheCtrlSet(stFodetCacheCtrl);
        // enable cache
        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1EnIntgrlImgCache = 1;	//[8] enable integral image cache
        stFodetCacheCtrl.ui1EnHidCascadeCache=1;     //[9] enable Cascade cache
        stFodetCacheCtrl.ui1FodMstrEn = 1;		        //[0] enable master (RD HID)
        FodetCacheCtrlSet(stFodetCacheCtrl);
#endif // cache

        ui32TimeOut = 0x00400000;
        while(1)
        {
            stFodetClsfrCtrl.ui32Ctrl = FodetClsfrCtrlGet();
            //==== Detected & Hold ======//
            if(stFodetClsfrCtrl.ui1ResumeHold || stFodetClsfrCtrl.ui1ResetDetected)
            {		
                // detected valid when hold be set.
                if(stFodetClsfrCtrl.ui1ResumeHold && stFodetClsfrCtrl.ui1ResetDetected
                                                                    && (iCandidateCount < MAX_ARY_SCAN_ALL)){
                    avgComps1[iCandidateCount].rect.x = (int)(stFodetClsfrCtrl.ui9XCoordinate + ui16Xost);
                    avgComps1[iCandidateCount].rect.y = (int)(stFodetClsfrCtrl.ui8YCoordinate + ui16Yost);
                    avgComps1[iCandidateCount].rect.w = (int)ui16RealWinWd;
                    avgComps1[iCandidateCount].rect.h = (int)ui16RealWinHt;
                    FODET_MSG_LOG("Index=(%d),Rect(%d)=(%d,%d,%d,%d)\n",i8FDIndex, iCandidateCount,
                            avgComps1[iCandidateCount].rect.x, avgComps1[iCandidateCount].rect.y,
                            avgComps1[iCandidateCount].rect.w, avgComps1[iCandidateCount].rect.h);
                    iCandidateCount++;
                }else if(stFodetClsfrCtrl.ui1ResumeHold && stFodetClsfrCtrl.ui1ResetDetected){
                    FODET_MSG_ERR("LOSS!!! Index=(%d),(%d,%d,%d,%d)\n",i8FDIndex, 
                    (int)(stFodetClsfrCtrl.ui9XCoordinate + ui16Xost), (int)(stFodetClsfrCtrl.ui8YCoordinate + ui16Yost),
                    ui16RealWinWd, ui16RealWinHt);
                }

                if(stFodetClsfrCtrl.ui1ResetDetected){
                    FodetCacheCtrlSet(stFodetCacheCtrl);
                    // resume
                    FodetClsfrResumeAndTrigger(stFodetClsfrCtrl);
                }else{
                    // Resume
                    FodetClsfrResumeAndTrigger(stFodetClsfrCtrl);
                }
            }
            //==== Scan finish ======//
            if(stFodetClsfrCtrl.ui1EnBusy == 0)
            {
            	FODET_MSG_LOG("\n This scaling factor finish!!!\n Go on next eq window. \n");
            	break;
            }                    
            //==== Time out ======//
            ui32TimeOut--;
            if(!ui32TimeOut)
            {
            	FODET_MSG_ERR("\n ERROR : wait  Time Out \n");
            	break;
            }
        }//while()
        if(ui8WinScanMode)
            i8FDIndex++;
        else
            i8FDIndex--;
    } // for FD Index
    return ret;
}


FODET_INT32 fodet_RenewRun_polling(FODET_USER* pFodetUser)
{
    FODET_INT32 ret=0;
    FODET_UINT32 ui32TimeOut;
    FODET_UINT32 ui32Temp=0;
    FODET_UINT32 ui32Trigger=0x00000000;
    FODET_INT8 i8FDIndex=0;
    FODET_UINT16 ui16EqWinWd=0;
    FODET_UINT16 ui16EqWinHt=0;
    FODET_UINT32 ui32InvWin=0;
    FODET_UINT16 ui16StopWd, ui16StopHt;
    FODET_UINT16 ui16ShiftStep;
    FODET_UINT16 ui16RealWinWd=0;
    FODET_UINT16 ui16RealWinHt=0;
    FODET_UINT16 ui16Xost, ui16Yost;
    FODET_AXI_ID FodetAxi;
    FODET_CACHE_CTRL stFodetCacheCtrl;
    FODET_INTGRL_IMG_CTRL FodetIntgrlImgCtrl;
    FODET_UINT8 ui8ClsfrStatusIdx;
    FODET_UINT32 ui32ClsfrStatusVal;
    FODET_UINT32 ui32ClsfrCascadeImgAddr;
    FODET_UINT32 ui32ClsfrHidCascadeOutAddr;
    FODET_IMG_INFO ClsfrIntgralImgOut;
    FODET_BUS_CTRL ClsfrIntgralImgBusCtrl;
    FODET_CLSFR_SCALE_SIZE ClsfrScaleSize;
    FODET_UINT16 ui16ReciprocalWndArea;
    FODET_CLSFR_WND_LMT stClsfrWndLmt;
    FODET_CLSFR_CTRL stFodetClsfrCtrl;
    FODET_UINT8 ui8WinScanMode=FODET_WIN_SCAN_SMALL2LARGE;     // 1: from small to large, 0: from large to small
    FODET_UINT16 StartIdx, EndIdx;

    if(ui8WinScanMode){
        StartIdx = pFodetUser->ui16StartIndex;
        EndIdx = pFodetUser->ui16EndIndex;
    }else{
        StartIdx = pFodetUser->ui16EndIndex;
        EndIdx = pFodetUser->ui16StartIndex;
    }
    // AXI IDS
    FodetAxi.ui8IMCID = 4;
    FodetAxi.ui8HCID = 3;
    FodetAxi.ui8CID = 2;
    FodetAxi.ui8SRCID = 1;
    FodetAxi.ui8FWID = 6;
    FodetAxi.ui8HCCID = 5;
    ret += FodetAxiCtrlSet(FodetAxi);
    // Reset , Enable Master
    stFodetCacheCtrl.ui32Ctrl = 0;
    ret += FodetMasterReset(stFodetCacheCtrl);
    ret += FodetMasterEnable(&stFodetCacheCtrl);
    // Reset Classifier
    FodetClsfrReset();
    FodetIntgrlImgCtrl.ui32Ctrl = 0;
    FodetIntgrlImgCtrl.ui1SrcImgFmt = pFodetUser->ui16ImgFormat;
    ret += FodetIntgrlImgCtrlSet(FodetIntgrlImgCtrl);
    if(ui32Trigger & 0x00000008){
        ui8ClsfrStatusIdx = 7;		// debug_index = 7.
        ret += FodetClsfrStatusIdxSet(ui8ClsfrStatusIdx);
        ui32ClsfrStatusVal = 0;	        // debug_value = 0.
        ret += FodetClsfrStatusValChk(ui32ClsfrStatusVal);
        FODET_MSG_LOG("<@44@>=[07], <@48@>=[%08X]\n",FODET_READL(0x0048));
    }       
    ui32ClsfrCascadeImgAddr = pFodetUser->ui32DatabaseAddr;
    ret += FodetClsfrCascadeImgAddrSet(ui32ClsfrCascadeImgAddr);
    ui32ClsfrHidCascadeOutAddr = pFodetUser->ui32HIDCCAddr;
    ret += FodetClsfrHidCascadeOutAddrSet(ui32ClsfrHidCascadeOutAddr);
    ClsfrIntgralImgOut.ui32Addr = pFodetUser->ui32IISumAddr;
    ret += FodetClsfrIntgrlImgOutAddrSet(ClsfrIntgralImgOut.ui32Addr);
    ClsfrIntgralImgOut.ui16Stride = pFodetUser->ui16IIoutStride;
    ClsfrIntgralImgOut.ui16Offset = pFodetUser->ui16IIoutOffset;
    ret += FodetClsfrIntgrlImgOutStrideOffsetSet(ClsfrIntgralImgOut.ui16Stride, ClsfrIntgralImgOut.ui16Offset);
    ClsfrIntgralImgBusCtrl.ui8MemWriteBurstSize = 0;
    ClsfrIntgralImgBusCtrl.ui8MemWriteThreshold = 15;
    ClsfrIntgralImgBusCtrl.ui8MemReadThreshold = 15;
    ret += FodetClsfrIntgrlImgBusCtrlSet(ClsfrIntgralImgBusCtrl.ui8MemWriteBurstSize, ClsfrIntgralImgBusCtrl.ui8MemWriteThreshold, ClsfrIntgralImgBusCtrl.ui8MemReadThreshold);
    #if 0
    FODET_MSG_LOG("<@24@>=[%08X]\n",FODET_READL(0x0024));
    FODET_MSG_LOG("<@28@>=[%08X]\n",FODET_READL(0x0028));
    FODET_MSG_LOG("<@2c@>=[%08X]\n",FODET_READL(0x002c));
    FODET_MSG_LOG("<@30@>=[%08X]\n",FODET_READL(0x0030));
    FODET_MSG_LOG("<@34@>=[%08X]\n",FODET_READL(0x0034));
    #endif
    i8FDIndex = StartIdx;
    while(1)
    {
        if(ui8WinScanMode){
            if(i8FDIndex > EndIdx)
                break;
        }else{
            if(i8FDIndex < EndIdx)
                break;
        }
        ui16EqWinWd=  stOR[i8FDIndex].eq_win_wd;
        ui16EqWinHt =  stOR[i8FDIndex].eq_win_ht;
        ui16ShiftStep = stOR[i8FDIndex].step;
        ui32InvWin = 1048576 /(ui16EqWinWd * ui16EqWinHt);    // 2^20 =1048576
        if(pFodetUser->ui32ImgOffset != 0){
            ui16StopWd = (FODET_UINT16)(((((pFodetUser->ui16CropWidth - ui16EqWinWd) *10)  / ui16ShiftStep)+ 0) / 10);
            ui16StopHt = (FODET_UINT16)(((((pFodetUser->ui16CropHeight - ui16EqWinHt) *10)  / ui16ShiftStep)+ 0) / 10);        
        }else{
            ui16StopWd = (FODET_UINT16)(((((pFodetUser->ui16ImgWidth - ui16EqWinWd) *10)  / ui16ShiftStep)+ 0) / 10);
            ui16StopHt = (FODET_UINT16)(((((pFodetUser->ui16ImgHeight - ui16EqWinHt) *10)  / ui16ShiftStep)+ 0) / 10);
        }
        ui16RealWinWd= stOR[i8FDIndex].real_wd;
        ui16RealWinHt= stOR[i8FDIndex].real_ht;
        ui16Xost = pFodetUser->ui16CropXost;
        ui16Yost = pFodetUser->ui16CropYost;

        stFodetCacheCtrl.ui32Ctrl = 0;
        stFodetCacheCtrl.ui1HoldAftrRenew = 1;
        stFodetCacheCtrl.ui1FodMstrEn = 1;
        FodetCacheCtrlSet(stFodetCacheCtrl);
        ClsfrScaleSize.ui14ScalingFactor = stOR[i8FDIndex].scaling_factor;  //g_wScalingFactor[i8FDIndex];
        ClsfrScaleSize.ui8EqWinWidth = ui16EqWinWd;
        ClsfrScaleSize.ui8EqWinHeight = ui16EqWinHt;
        FodetClsfrIntgrlImgScaleSizeSet(ClsfrScaleSize.ui14ScalingFactor, ClsfrScaleSize.ui8EqWinWidth, ClsfrScaleSize.ui8EqWinHeight);
        ui16ReciprocalWndArea = ui32InvWin;
        FodetClsfrReciprocalWndAreaSet(ui16ReciprocalWndArea);
        stClsfrWndLmt.ui14Step = (ui16ShiftStep<<10);
        stClsfrWndLmt.ui8StopWidth = ui16StopWd;
        stClsfrWndLmt.ui7StopHeight = ui16StopHt;
        FodetClsfrWndLmtSet(stClsfrWndLmt);
#if 0
        FODET_MSG_LOG("Index=(%d)\n",i8FDIndex);
        FODET_MSG_LOG("SF=[%X], EqWin=(%d), Step=(%d), InvWin=[%x], StopWH=(%d,%d), RealWin=(%d)\n",
        g_wScalingFactor[i8FDIndex], ui16EqWin, ui16ShiftStep, ui32InvWin, ui16StopWd,ui16StopHt, ui16RealWin);
        FODET_MSG_LOG("<@90@>=[%08X]\n",FODET_READL(0x0090));
        FODET_MSG_LOG("<@38@>=[%08X]\n",FODET_READL(0x0038));
        FODET_MSG_LOG("<@3C@>=[%08X]\n",FODET_READL(0x003c));
        FODET_MSG_LOG("<@40@>=[%08X]\n",FODET_READL(0x0040));
#endif
        stFodetClsfrCtrl.ui32Ctrl = 0;
        ui32TimeOut=0x01000000;
        FodetClsfrTrigger(stFodetClsfrCtrl);
        while(1){
            // check hold 0x20 = 0x02
            ui32Temp = FODET_READL(FODET_FODCCTRL);
            if((ui32Temp & FOD_STATUS_HOLD) == FOD_STATUS_HOLD){
                FODET_MSG_LOG("w-check hold<@20@>=[%08X]\n",FODET_READL(0x0020));
                break;
            }

            // check hold 0x90 = 0x01004001
            ui32Temp = FODET_READL(FODET_FODCHCTRL);
            if((ui32Temp & (FOD_STATUS_INT | FOD_STATUS_HAR)) == (FOD_STATUS_INT | FOD_STATUS_HAR)){
                FODET_MSG_LOG("w-interrupt state, renew hold<@90@>=[%08X]\n",FODET_READL(0x0090));
                break;
            }
            // time out
            ui32TimeOut--;
            if(!ui32TimeOut)
            {
                FODET_MSG_ERR("\n ERROR : wait  renew hold interrupt Time Out[%x] \n", FodetCacheCtrlGet());
                return -20;
            }
        }

        stFodetCacheCtrl.ui32Ctrl = FODET_READL(0x0090) & 0xFEFFBFFF;
        FodetCacheCtrlSet(stFodetCacheCtrl);
        stFodetClsfrCtrl.ui32Ctrl = FODET_READL(0x0020) & 0x000000ff;
        ui32TimeOut = 0x00400000;
        FodetClsfrResumeAndTrigger(stFodetClsfrCtrl);
        while(1)
        {
            stFodetClsfrCtrl.ui32Ctrl = FodetClsfrCtrlGet();
            //==== Detected & Hold ======//
            if(stFodetClsfrCtrl.ui1ResumeHold || stFodetClsfrCtrl.ui1ResetDetected)
            {
            	// detected valid when hold be set.
                if(stFodetClsfrCtrl.ui1ResumeHold && stFodetClsfrCtrl.ui1ResetDetected
                    && (iCandidateCount < MAX_ARY_SCAN_ALL)){
                        avgComps1[iCandidateCount].rect.x = (int)(stFodetClsfrCtrl.ui9XCoordinate + ui16Xost);
                        avgComps1[iCandidateCount].rect.y = (int)(stFodetClsfrCtrl.ui8YCoordinate + ui16Yost);
                        avgComps1[iCandidateCount].rect.w = (int)ui16RealWinWd;
                        avgComps1[iCandidateCount].rect.h = (int)ui16RealWinHt;
                        FODET_MSG_LOG("Index=(%d),Rect(%d)=(%d,%d,%d,%d)\n",i8FDIndex, iCandidateCount,
                        avgComps1[iCandidateCount].rect.x, avgComps1[iCandidateCount].rect.y,
                        avgComps1[iCandidateCount].rect.w, avgComps1[iCandidateCount].rect.h);

                        iCandidateCount++;
                }else if(stFodetClsfrCtrl.ui1ResumeHold && stFodetClsfrCtrl.ui1ResetDetected){
                        FODET_MSG_ERR("LOSS!!! Index=(%d),(%d,%d,%d,%d)\n",i8FDIndex, 
                                                        (int)(stFodetClsfrCtrl.ui9XCoordinate + ui16Xost), 
                                                        (int)(stFodetClsfrCtrl.ui8YCoordinate + ui16Yost),
                                                        ui16RealWinWd, ui16RealWinHt);
                }

                if(stFodetClsfrCtrl.ui1ResumeHold && stFodetClsfrCtrl.ui1ResetDetected){
                    FodetCacheCtrlSet(stFodetCacheCtrl);
                    FodetClsfrResumeAndTrigger(stFodetClsfrCtrl);
                }else{
                    FodetClsfrResumeAndTrigger(stFodetClsfrCtrl);
                }
            }
            //==== Scan finish ======//
            if(stFodetClsfrCtrl.ui1EnBusy == FODET_STATUS_DONE)
            {
                FODET_MSG_LOG("\n This scaling factor finish!!!\n Go on next eq window. \n");
                break;
            }
            //==== Time out ======//
            ui32TimeOut--;
            if(!ui32TimeOut)
            {
                FODET_MSG_ERR("\n ERROR : wait  Time Out \n");
                break;
            }
        }//while()
        if(ui8WinScanMode)
            i8FDIndex++;
        else
            i8FDIndex--;
    }
    return ret;
}


// INITIAL
FODET_RESULT FodetUserSetup(FODET_USER* pFodetUser)
{
    FODET_INT32 ret=0;
    ret = FODET_TRUE;

    if(copy_from_user((void*)stOR, (void*)pFodetUser->ui32ORDataAddr, sizeof(OR_DATA)*25))
    {
        FODET_MSG_ERR("FODET_INITIAL: copy from user space error\n");
        ret = FODET_FALSE;
    }
#if 0
    FODET_UINT16 i;
    for(i=0; i<=24; i++)
    {
        FODET_MSG_INFO("(%d)-(%08X, %03X, %03X, %02d, %03X, %03X)\n", i,
                stOR[i].scaling_factor, stOR[i].eq_win_wd, stOR[i].eq_win_ht,
                stOR[i].step, stOR[i].real_wd, stOR[i].real_ht);
    }
#endif
    
    return ret;
}

FODET_RESULT FodetUserObjectRecongtion(FODET_USER* pFodetUser)
{
    FODET_INT32 ret=0;
    FODET_CACHE_CTRL stModCtrl;
    
    #if 0
    FODET_MSG_LOG("======  FodetUserObjectRecongtion  ========\n");
    FODET_MSG_LOG("SrcImgAddr  =[%X], lenght(%d)\n", pFodetUser->ui32SrcImgAddr, pFodetUser->ui32SrcImgLength);
    FODET_MSG_LOG("IISumAddr   =[%X], lenght(%d)\n", pFodetUser->ui32IISumAddr, pFodetUser->ui32IISumLength);
    FODET_MSG_LOG("DatabaseAddr=[%X], lenght(%d)\n", pFodetUser->ui32DatabaseAddr, pFodetUser->ui32DatabaseLength);
    FODET_MSG_LOG("HIDCCAddr   =[%X], lenght(%d)\n", pFodetUser->ui32HIDCCAddr, pFodetUser->ui32HIDCCLength);

    FODET_MSG_LOG("SrcImg WHS  =(%d,%d,%d)\n", pFodetUser->ui16ImgWidth, pFodetUser->ui16ImgHeight, pFodetUser->ui16ImgStride);
    FODET_MSG_LOG("    Ost Fmt =(%d), [%x]\n", pFodetUser->ui32ImgOffset, pFodetUser->ui16ImgFormat);
    FODET_MSG_LOG("IIout offset=(%d)\n", pFodetUser->ui16IIoutOffset);
    FODET_MSG_LOG("IIout stride=(%d)\n", pFodetUser->ui16IIoutStride);

    FODET_MSG_LOG("Scan Index  =Start(%d)-End(%d)\n", pFodetUser->ui16StartIndex, pFodetUser->ui16EndIndex);
    FODET_MSG_LOG("Scan Win    =WdHt(%d, %d)\n", pFodetUser->ui16EqWinWd, pFodetUser->ui16EqWinHt);
    FODET_MSG_LOG("SF, Shift   =(%d, %d)\n", pFodetUser->ui16ScaleFactor, pFodetUser->ui16ShiftStep);
    FODET_MSG_LOG("Stop Win    =WdHt(%d, %d)\n", pFodetUser->ui16StopWd, pFodetUser->ui16StopHt);
    FODET_MSG_LOG("       RWA  =(%d)\n", pFodetUser->ui32RWA);
    #endif
    
    if((pFodetUser->ui32SrcImgAddr == 0xffffffff)
        || (pFodetUser->ui32SrcImgAddr == 0x00000000)){
        FODET_MSG_ERR("ERROR : Source Image Addr err !\n");
        return -1;
    }

#ifdef TEST_FD_PERFORMANCE_TIME_MEASURE
    unsigned long val_b, val_e;
    struct timeval begin, end;
    
    do_gettimeofday(&begin);
#endif

    iCandidateCount = 0;
    InitGroupingArray_Scanall();
   
    ret = fodet_IntegralImage(pFodetUser);
    if(0 > ret){
        // Reset , Enable Master
        stModCtrl.ui32Ctrl = 0;
        ret += FodetMasterReset(stModCtrl);
        ret += FodetMasterEnable(&stModCtrl);
        FODET_MSG_ERR("ERROR : II Error (%d)\n", ret);
    }else{
        pFodetUser->ui16StartIndex = CheckStartIdx(pFodetUser);
        if(pFodetUser->ui8CacheFlag)
            ret = fodet_RenewRunWithCache(pFodetUser);
        else
            ret= fodet_RenewRun(pFodetUser);

        if(0 > ret){
            // Reset Classifier
            FodetClsfrReset();
            // Reset , Enable Master
            stModCtrl.ui32Ctrl = 0;
            ret += FodetMasterReset(stModCtrl);
            ret += FodetMasterEnable(&stModCtrl);
            FODET_MSG_ERR("ERROR : RR Error (%d)\n", ret);
        }
    }

#ifdef TEST_FD_PERFORMANCE_TIME_MEASURE
    do_gettimeofday(&end);
    val_b = (begin.tv_sec * 1000) + (begin.tv_usec / 1000);
    val_e = (end.tv_sec * 1000) + (end.tv_usec /1000);
    FODET_MSG_INFO("FD : Idx(%d-%d) : %lu ms\n", 
                                        pFodetUser->ui16StartIndex, 
                                        pFodetUser->ui16EndIndex,
                                        (val_e-val_b));
#endif

    return ret;
}

