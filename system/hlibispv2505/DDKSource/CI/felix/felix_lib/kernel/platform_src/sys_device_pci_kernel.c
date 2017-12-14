/**
******************************************************************************
@file sys_device_pci_kernel.c

@brief discovers and register FPGA PCI device

@note mostly adapted from sysdev_utils1-fpga.c from viddec

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include <ci_internal/sys_device.h>
#include <ci_internal/i2c-img.h>
#include <linux/delay.h>

#ifdef IMG_SCB_DRIVER
// we need to define the PCI_SCB_OFFSET in register bank

// offset in felix v1
#define PCI_SCB_OFFSET_V1 0x6000
// offset in felix v2
#define PCI_SCB_OFFSET_V2 0xB000
// offset in felix v3
#define PCI_SCB_OFFSET_V3 0x15000

#include <ci_kernel/ci_kernel.h>
#include <ci_internal/sys_device.h>
#include <registers/core.h>
#endif // IMG_SCB_DRIVER


#include <linux/pci.h>
#include <linux/module.h>

#include <img_defs.h>
#include <img_errors.h>

#if defined(IMGVIDEO_ALLOC)

#include <sysdev_utils.h>
#include <sysmem_utils.h>

#endif

/*
 * PCI cards definitions
 */

#define PCI_ATLAS_VENDOR_ID                     (0x1010)
#define PCI_ATLAS_DEVICE_ID                     (0x1CF1)        //!< Atlas V1 - FPGA device ID.
#define PCI_APOLLO_DEVICE_ID                    (0x1CF2)        //!< Atals V2 (Apollo) - FPGA device ID.

// from TCF Support FPGA.Technical Reference Manual.1.0.92.Internal Atlas GEN.External.doc:
#define PCI_ATLAS_CTRL_REGS_SIZE            (0xC000)        //!< Atlas - System control register size
#define PCI_ATLAS_PDP_REGS_SIZE             (0x2000)        //!< Atlas - size of PDP register area
#define PCI_ATLAS_CTRL_REGS_OFFSET          (0x0000)        //!< Altas - System control register offset

#define PCI_ATLAS_PDP1_REGS_OFFSET              (0xC000)        //!< Atlas - PDP1 register offset into bar
#define PCI_ATLAS_PDP2_REGS_OFFSET              (0xE000)        //!< Atlas - PDP2 register offset into bar

#define PCI_ATLAS_INTERRUPT_STATUS              (PCI_ATLAS_CTRL_REGS_OFFSET+0x00E0)        //!< Atlas - Offset of INTERRUPT_STATUS
#define PCI_ATLAS_INTERRUPT_ENABLE              (PCI_ATLAS_CTRL_REGS_OFFSET+0x00F0)        //!< Atlas - Offset of INTERRUPT_ENABLE
#define PCI_ATLAS_INTERRUPT_CLEAR               (PCI_ATLAS_CTRL_REGS_OFFSET+0x00F8)        //!< Atlas - Offset of INTERRUPT_CLEAR
#define PCI_ATLAS_TEST_CTRL                     (PCI_ATLAS_CTRL_REGS_OFFSET+0x00B0)        //!< Atlas - Offset of TEST_CTRL

#define PCI_ATLAS_MASTER_ENABLE                 (1<<31)         //!< Atlas - Master interrupt enable
// interrupt values (status)
#define PCI_ATLAS_DEVICE_INT                    (1<<13)         //!< Atlas - Device interrupt
#define PCI_ATLAS_PDP1_INT                      (1<<14)         //!< Atlas - PDP1 interrupt
#define PCI_ATLAS_PDP2_INT                      (1<<15)         //!< Atlas - PDP2 interrupt
#define PCI_ATLAS_CLK_GATE_CNTL                 (1<<16)         //!< Atlas - assert clock gating to Test Chip
#define PCI_ATLAS_DUT_DCM_RESET                 (1<<10)         //!< Atlas - soft reset PLLs/DCMs
#define PCI_ATLAS_SCB_RESET                     (1<<4)          //!< Atlas - SCB Logic soft reset
#define PCI_ATLAS_PDP2_RESET                    (1<<3)          //!< Atlas - PDP2 soft reset
#define PCI_ATLAS_PDP1_RESET                    (1<<2)          //!< Atlas - PDP1 soft reset
#define PCI_ATLAS_DDR_RESET                     (1<<1)          //!< Atlas - soft reset the DDR logic
#define PCI_ATLAS_DUT_RESET                     (1<<0)          //!< Atlas - soft reset the device under test

// SCB interrupt mask bits on Apollo PCI board
#define PCI_APOLLO_SCB_MST_HLT                  (1<<12)
#define PCI_APOLLO_SCB_SLV_EVENT                (1<<11)
#define PCI_APOLLO_SCB_TDONE_RX                 (1<<10)
#define PCI_APOLLO_SCB_SLV_WT_RD_DAT            (1<<9)
#define PCI_APOLLO_SCB_SLV_WT_PRV_RD            (1<<8)
#define PCI_APOLLO_SCB_SLV_WT_WR_DAT            (1<<7)
#define PCI_APOLLO_SCB_MST_WT_RD_DAT            (1<<6)
#define PCI_APOLLO_SCB_ADD_ACK_ERR              (1<<5)
#define PCI_APOLLO_SCB_WR_ACK_ERR               (1<<4)
#define PCI_APOLLO_SCB_SDAT_LO_TIM              (1<<3)
#define PCI_APOLLO_SCB_SCLK_LO_TIM              (1<<2)
#define PCI_APOLLO_SCB_UNEX_START_BIT           (1<<1)
#define PCI_APOLLO_SCB_BUS_INACTIVE             (1<<0)

#define PCI_ATLAS_RESET_REG_OFFSET              (0x0080)
#define PCI_ATLAS_RESET_BITS                    (PCI_ATLAS_DDR_RESET | PCI_ATLAS_DUT_RESET |PCI_ATLAS_PDP1_RESET| PCI_ATLAS_PDP2_RESET | PCI_ATLAS_SCB_RESET )
#define PCI_ATLAS_PLL_REG_OFFSET                (0x1000)
#define PCI_ATLAS_PLL_CORE_CLK0                    (PCI_ATLAS_PLL_REG_OFFSET + 0x48)
#define PCI_ATLAS_PLL_CORE_DRP_GO                (PCI_ATLAS_PLL_REG_OFFSET + 0x58)

#define IS_AVNET_DEVICE(devid)  ((devid)!=PCI_ATLAS_DEVICE_ID && (devid)!=PCI_APOLLO_DEVICE_ID)
#define IS_ATLAS_DEVICE(devid)  ((devid)==PCI_ATLAS_DEVICE_ID)
#define IS_APOLLO_DEVICE(devid) ((devid)==PCI_APOLLO_DEVICE_ID)

#define PCI_APOLLO_INTERRUPT_STATUS             (PCI_ATLAS_CTRL_REGS_OFFSET+0x00C8)        //!< Apollo - Offset of INTERRUPT_STATUS
#define PCI_APOLLO_INTERRUPT_ENABLE             (PCI_ATLAS_CTRL_REGS_OFFSET+0x00D8)        //!< Apollo - Offset of INTERRUPT_ENABLE
#define PCI_APOLLO_INTERRUPT_CLEAR              (PCI_ATLAS_CTRL_REGS_OFFSET+0x00E0)        //!< Apollo - Offset of INTERRUPT_CLEAR
#define PCI_APOLLO_TEST_CTRL                    (PCI_ATLAS_CTRL_REGS_OFFSET+0x0098)        //!< Apollo - Offset of TEST_CTRL

#define PCI_ATLAS_CTRL_REGS_BAR             (0) ///< Altas - System control register bar
#define PCI_ATLAS_PDP_REGS_BAR                  (0) ///< Altas - PDP register bar
#define PCI_ATLAS_REGISTER_BAR                  (1) ///< @brief PCI bar to access the device's register offset
#define PCI_ATLAS_MEMORY_BAR                    (2) ///< @brief PCI bar to access the memory offset

#define PCI_MAX_MEM (256<<20) ///< @brief Maximum of memory to map to kernel-space in bytes (256MB)

// extension for memory control (may have additional interrupts)
#define APOLLO_MEM_EXTENSION
#define APOLLO_MEM_EXTENSION_OFFSET (0x4000)

//#define SYS_DBG printk
#define SYS_DBG(...)

/*
 * local functions declaration
 */

/*
 * called when linux kernel finds a device that matches the given ids
 */
static int IMG_SYS_ProbePCI(struct pci_dev *dev, const struct pci_device_id *id);

/*
 * called when a registered device is unplugged or the driver unregistered
 */
static void IMG_SYS_RemovePCI(struct pci_dev *dev);

/*
 * reset HW at startup or shutdown
 */
static void IMG_SYS_ResetHW(void);

static DEFINE_PCI_DEVICE_TABLE(pci_ids) =
{
    { PCI_ATLAS_VENDOR_ID, PCI_ATLAS_DEVICE_ID,  PCI_ANY_ID, PCI_ANY_ID },
    { PCI_ATLAS_VENDOR_ID, PCI_APOLLO_DEVICE_ID, PCI_ANY_ID, PCI_ANY_ID },
    { 0 }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

struct SYS_PCI_DEVICE {
    struct pci_driver sDriver;
    struct pci_dev *pDev;

    // interrupt registers
    IMG_SIZE uiEnableOffset;
    IMG_SIZE uiStatusOffset;
    IMG_SIZE uiClearOffset;
    IMG_SIZE uiTestCtrlOffset;

    SYS_DEVICE *pSysDevice;
};

#define CI_DEVICE_NAME "isp_ci_pci"

static struct SYS_PCI_DEVICE g_PCIDriver = {
    .sDriver = {
        .name = CI_DEVICE_NAME,
        .id_table = pci_ids,
        .probe = IMG_SYS_ProbePCI,
        .remove = IMG_SYS_RemovePCI,
    },
    .pDev = NULL,
    .pSysDevice = NULL,
};

/*
 * imgvideo functions
 */
#if defined(IMGVIDEO_ALLOC)

// defined in sys_mem_imgvideo.c
IMG_PHYSADDR Platform_paddr_to_devpaddr(struct SYSDEVU_sInfo *sysdev,
    IMG_PHYSADDR paddr);

IMG_RESULT FELIXKM_fnDevRegister (DMANKM_sDevRegister *psDevRegister)
{
    psDevRegister->ui32ConnFlags    = DMAN_CFLAG_SHARED;
    psDevRegister->pfnDevInit       = NULL;
    psDevRegister->pfnDevDeinit     = NULL;
    psDevRegister->pfnDevDisconnect = NULL;
    psDevRegister->pfnDevConnect    = NULL;
    psDevRegister->pfnDevKmHisr     = NULL;
    psDevRegister->pfnDevKmLisr     = NULL;
    psDevRegister->pfnDevPowerPreS5  = NULL;
    psDevRegister->pfnDevPowerPostS0 = NULL;
    return IMG_SUCCESS;
}

static SYSDEVU_sInfo sysdevu_device = {
    NULL,
    SYS_DEVICE(CI_DEVICE_NAME, FELIX, IMG_FALSE)
};
static struct SYSDEV_ops sysdevu_ops = {
    .free_device = NULL,
    .resume_device = NULL,
    .suspend_device = NULL,
    .paddr_to_devpaddr = Platform_paddr_to_devpaddr
};

#endif // IMGVIDEO_ALLOC

/*
 * local function implementation
 */

static void IMG_SYS_ResetHW(void)
{
    // disable interrupts

    if ( g_PCIDriver.pSysDevice )
    {
        // disable interrupts that could be enabled
        iowrite32(0, (void __iomem*)g_PCIDriver.pSysDevice->uiBoardCPUVirtual+g_PCIDriver.uiEnableOffset);
    }
}

#ifdef FPGA_INSMOD_RECLOCK
/**
 * @param freq in MHz
 */
static void IMG_SYS_setupRefClock(unsigned int freq)
{
    void __iomem *board =
        (void __iomem*)g_PCIDriver.pSysDevice->uiBoardCPUVirtual;

    printk(KERN_ERR "Setting FPGA clock to %d MHz.\n", freq);
    /* Setup PLL registers */
    iowrite32(freq, board + PCI_ATLAS_PLL_CORE_CLK0);
    iowrite32(0x01, board + PCI_ATLAS_PLL_CORE_DRP_GO);

    /* Reset ATLAS board */
    iowrite32(
            PCI_ATLAS_CLK_GATE_CNTL    |
            PCI_ATLAS_SCB_RESET        |
            PCI_ATLAS_PDP2_RESET    |
            PCI_ATLAS_PDP1_RESET,
            board + PCI_ATLAS_RESET_REG_OFFSET);
    mdelay(100);
    iowrite32(
            PCI_ATLAS_CLK_GATE_CNTL    |
            PCI_ATLAS_DUT_DCM_RESET    |
            PCI_ATLAS_SCB_RESET        |
            PCI_ATLAS_PDP2_RESET    |
            PCI_ATLAS_PDP1_RESET    |
            PCI_ATLAS_DDR_RESET,
            board + PCI_ATLAS_RESET_REG_OFFSET);
    mdelay(100);
    iowrite32(
            PCI_ATLAS_CLK_GATE_CNTL    |
            PCI_ATLAS_SCB_RESET        |
            PCI_ATLAS_PDP2_RESET    |
            PCI_ATLAS_PDP1_RESET    |
            PCI_ATLAS_DDR_RESET        |
            PCI_ATLAS_DUT_RESET,
            board + PCI_ATLAS_RESET_REG_OFFSET);
    mdelay(100);
    iowrite32(
            PCI_ATLAS_CLK_GATE_CNTL    |
            PCI_ATLAS_DUT_DCM_RESET    |
            PCI_ATLAS_SCB_RESET        |
            PCI_ATLAS_PDP2_RESET    |
            PCI_ATLAS_PDP1_RESET    |
            PCI_ATLAS_DDR_RESET        |
            PCI_ATLAS_DUT_RESET,
            board + PCI_ATLAS_RESET_REG_OFFSET);
    mdelay(100);
}
#endif

#ifdef IMG_SCB_DRIVER
static void img_enable_scb_irq(void)
{
#ifndef IMG_SCB_POLLING_MODE
    iowrite32(PCI_ATLAS_MASTER_ENABLE|
            PCI_ATLAS_DEVICE_INT,
            (void __iomem*)g_PCIDriver.pSysDevice->uiBoardCPUVirtual +
            g_PCIDriver.uiEnableOffset);
#endif
}

static int IMG_SYS_ProbeSCB(struct pci_dev *dev)
{
        uintptr_t scb_off = 0;
        // read Felix's version
        uint32_t felix_v = ioread32((void*)
        g_PCIDriver.pSysDevice->uiRegisterCPUVirtual +
        FELIX_CORE_CORE_REVISION_OFFSET);
        uint32_t felix_M = (felix_v >> 16)&0xff; // major
        uint32_t felix_m = (felix_v >> 8) & 0xff; // minor

        SYS_DBG("Found felix version 0x%x = %d.%d\n",
            felix_v, felix_M, felix_m);
        switch (felix_M)
        {
        case 1:
            scb_off = PCI_SCB_OFFSET_V1;
            break;
        case 2:
            if (3 <= felix_m)
            {
                // 2.3 and 2.4 also use that SCB offset
                scb_off = PCI_SCB_OFFSET_V3;
            }
            else
            {
                scb_off = PCI_SCB_OFFSET_V2;
            }
            break;
        case 3:
            scb_off = PCI_SCB_OFFSET_V3;
            break;
        default:
            printk(KERN_ERR "Unknow SCB offset for felix version %d.%d!\n",
                felix_M, felix_m);
            return -IMG_ERROR_DEVICE_NOT_FOUND;
        }
        img_enable_scb_irq();
        i2c_img_probe(dev, g_PCIDriver.pSysDevice->uiRegisterCPUVirtual
            + scb_off, dev->irq);
        SYS_DBG("i2c offset used: 0x%x\n", scb_off);
        i2c_img_suspend_noirq(dev);
        return IMG_SUCCESS;
}
#endif


static int IMG_SYS_ProbePCI(struct pci_dev *dev, const struct pci_device_id *id)
{
    
    g_PCIDriver.pDev = dev;

    /* enable the HW */
    if (pci_enable_device(dev))
    {
        printk(KERN_ERR "failed to enable pci device\n");
        goto enable_error;
    }

    /* Reserve PCI I/O and memory resources */
    if (pci_request_regions(dev, "imgpci"))
    {
        printk(KERN_ERR "failed to request region\n");
        goto request_error;
    }

    //
    // find interrupts
    //
    {
        IMG_UINT8 read = 0;
        if ( pci_read_config_byte(dev, PCI_INTERRUPT_LINE, &read) != 0 )
        {
            goto interrupt_error;
        }
        //g_PCIDriver.pSysDevice->irq_line = read;
        SYS_DBG(KERN_INFO "pci_read_config_byte PCI_INTERRUPT_LINE gives %u\n", read);
        g_PCIDriver.pSysDevice->irq_line = dev->irq;
        SYS_DBG(KERN_INFO "PCI device gives interrupt %u\n", dev->irq);
    }

    // find the location of the registers and memory for the device
    // note that pci_resource_end is the last usable address
    // ioremap() should be mapping into a non-cached part of memory

    g_PCIDriver.pSysDevice->uiRegisterSize = pci_resource_len(dev, PCI_ATLAS_REGISTER_BAR);
    g_PCIDriver.pSysDevice->uiRegisterPhysical = (IMG_UINTPTR)pci_resource_start(dev, PCI_ATLAS_REGISTER_BAR);
    g_PCIDriver.pSysDevice->uiRegisterCPUVirtual = (IMG_UINTPTR)ioremap(g_PCIDriver.pSysDevice->uiRegisterPhysical, g_PCIDriver.pSysDevice->uiRegisterSize);
    if ( g_PCIDriver.pSysDevice->uiRegisterCPUVirtual == 0 ) // ioremap return NULL on failure
    {
        printk(KERN_ERR "failed to map registers to kernel memory\n");
        g_PCIDriver.pSysDevice->uiRegisterSize = 0;
        goto map_error;
    }

#ifndef FPGA_BUS_MASTERING
    g_PCIDriver.pSysDevice->uiMemorySize = pci_resource_len(dev, PCI_ATLAS_MEMORY_BAR);
    if ( g_PCIDriver.pSysDevice->uiMemorySize > PCI_MAX_MEM )
    {
        SYS_DBG(KERN_INFO "limit card memory from %llu MB to %llu MB\n",
                (unsigned long long)g_PCIDriver.pSysDevice->uiMemorySize >> 20,
                (unsigned long long)PCI_MAX_MEM >> 20);
        g_PCIDriver.pSysDevice->uiMemorySize = PCI_MAX_MEM;
    }
#if defined(SYSMEM_DMABUF_IMPORT)
    /*
     * In configuration where we import dma_buf handles, we limit the size 
     * of pci memory space used by imgvideo carveout to half
     * This is because dmabuf_exporter may have been configured to use carveout as well
     */
    g_PCIDriver.pSysDevice->uiMemorySize/=2;
    printk(KERN_INFO "dma_buf (carveout) imports set : using only 0x%lx bytes " \
                     "for felix carveout",
        g_PCIDriver.pSysDevice->uiMemorySize);
#endif

    /*
     * Hardware sees memory starting from 0 as it's not system wide memory, it's PCI memory.
     * But if we put 0 in start of physical memory external data generator does not work
     * WARNING: affects memory allocator
     */
    g_PCIDriver.pSysDevice->uiDevMemoryPhysical = 0;
    
    g_PCIDriver.pSysDevice->uiMemoryPhysical = pci_resource_start(dev, PCI_ATLAS_MEMORY_BAR);
    
    g_PCIDriver.pSysDevice->uiMemoryCPUVirtual =
        (IMG_UINTPTR)ioremap(
                g_PCIDriver.pSysDevice->uiMemoryPhysical,
                g_PCIDriver.pSysDevice->uiMemorySize);
    if ( g_PCIDriver.pSysDevice->uiMemoryCPUVirtual == 0 ) // ioremap return NULL on failure
    {
        printk(KERN_ERR "failed to map memory to kernel memory (%llu MB)\n",
               (unsigned long long)g_PCIDriver.pSysDevice->uiMemorySize >> 20);
        g_PCIDriver.pSysDevice->uiMemorySize = 0;
        goto map_error;
    }
#else
    // when using bus mastering the device memory is not accessible - we can save a bit of vmalloc by not doing ioremap
    g_PCIDriver.pSysDevice->uiMemorySize = 0;
    g_PCIDriver.pSysDevice->uiDevMemoryPhysical = 0;
    g_PCIDriver.pSysDevice->uiMemoryPhysical = 0;
    g_PCIDriver.pSysDevice->uiMemoryCPUVirtual = 0;
#endif

    g_PCIDriver.pSysDevice->uiBoardSize = pci_resource_len(dev, PCI_ATLAS_CTRL_REGS_BAR);
    g_PCIDriver.pSysDevice->uiBoardPhysical = (IMG_UINTPTR)pci_resource_start(dev, PCI_ATLAS_CTRL_REGS_BAR);
    g_PCIDriver.pSysDevice->uiBoardCPUVirtual = (IMG_UINTPTR)ioremap(g_PCIDriver.pSysDevice->uiBoardPhysical, g_PCIDriver.pSysDevice->uiBoardSize);
    if ( g_PCIDriver.pSysDevice->uiBoardCPUVirtual == 0 ) // ioremap return NULL on failure
    {
        printk(KERN_ERR "failed to map board to kernel memory\n");
        g_PCIDriver.pSysDevice->uiBoardSize = 0;
        goto map_error;
    }

    if ( IS_APOLLO_DEVICE(g_PCIDriver.pDev->device) )
    {
        g_PCIDriver.uiClearOffset = PCI_APOLLO_INTERRUPT_CLEAR;
        g_PCIDriver.uiStatusOffset = PCI_APOLLO_INTERRUPT_STATUS;
        g_PCIDriver.uiEnableOffset = PCI_APOLLO_INTERRUPT_ENABLE;
        g_PCIDriver.uiTestCtrlOffset = PCI_APOLLO_TEST_CTRL;
    }
    else if ( IS_ATLAS_DEVICE(g_PCIDriver.pDev->device) )
    {
        g_PCIDriver.uiClearOffset = PCI_ATLAS_INTERRUPT_CLEAR;
        g_PCIDriver.uiStatusOffset = PCI_ATLAS_INTERRUPT_STATUS;
        g_PCIDriver.uiEnableOffset = PCI_ATLAS_INTERRUPT_ENABLE;
        g_PCIDriver.uiTestCtrlOffset = PCI_ATLAS_TEST_CTRL;
    }
    else
    {
        printk(KERN_ERR "invalid device id %d\n", g_PCIDriver.pDev->device);
        goto map_error;
    }

    // enabling interrupts done when requesting interrupts
    //iowrite32(PCI_ATLAS_MASTER_ENABLE|PCI_ATLAS_DEVICE_INT, (void*)g_PCIDriver.pSysDevice->uiBoardCPUVirtual+g_PCIDriver.uiEnableOffset);

    SYS_DBG(KERN_INFO "%s card found\n", IS_APOLLO_DEVICE(g_PCIDriver.pDev->device) ? "Apollo" : "Atlas");
    SYS_DBG(KERN_INFO "PCI device registers cpu: 0x%llx phys: 0x%llx\n",
            (unsigned long long)g_PCIDriver.pSysDevice->uiRegisterCPUVirtual,
            (unsigned long long)g_PCIDriver.pSysDevice->uiRegisterPhysical);
#ifndef FPGA_BUS_MASTERING
    SYS_DBG(KERN_INFO "PCI memory location cpu: 0x%llx phys: 0x%llx (%llu MBytes)\n",
            (unsigned long long)g_PCIDriver.pSysDevice->uiMemoryCPUVirtual,
            (unsigned long long)g_PCIDriver.pSysDevice->uiMemoryPhysical,
            (unsigned long long)g_PCIDriver.pSysDevice->uiMemorySize >> 20);
#else
    SYS_DBG(KERN_INFO "PCI memory not mapped - board configured with bus mastering\n");
#endif
    SYS_DBG(KERN_INFO "PCI board control registers cpu: 0x%llx phys: 0x%llx\n",
            (unsigned long long)g_PCIDriver.pSysDevice->uiBoardCPUVirtual,
            (unsigned long long)g_PCIDriver.pSysDevice->uiBoardPhysical);
    SYS_DBG(KERN_INFO "    enable=0x%"IMG_SIZEPR"X clear=0x%"IMG_SIZEPR"X status=0x%"IMG_SIZEPR"X\n",
            g_PCIDriver.uiEnableOffset, g_PCIDriver.uiClearOffset, g_PCIDriver.uiStatusOffset);
    SYS_DBG(KERN_INFO "PCI interrupt line %d\n", g_PCIDriver.pSysDevice->irq_line);

#ifdef FPGA_BUS_MASTERING
    /* TCF Support FPGA TRM - clear ADDRESS_FORCE bit in TEST_CTRL register */
    iowrite32(0x0, (void*)(g_PCIDriver.pSysDevice->uiBoardCPUVirtual + g_PCIDriver.uiTestCtrlOffset));
#else
    /* forces to use addresses from the board in case master mode was enabled before - also clears other test options */
    iowrite32(0x1, (void*)(g_PCIDriver.pSysDevice->uiBoardCPUVirtual + g_PCIDriver.uiTestCtrlOffset));
#endif

#ifdef FPGA_INSMOD_RECLOCK
    /* Setup FPGA clock to given value */
    IMG_SYS_setupRefClock(FPGA_INSMOD_RECLOCK);
#endif

    
#if defined(IMGVIDEO_ALLOC)
    {
        IMG_RESULT ui32Result;
    
        // populate sysdevu_device
        SYSDEVU_SetDevMap(&sysdevu_device, 0, 0, 0,
                        (IMG_PHYSADDR)g_PCIDriver.pSysDevice->uiMemoryPhysical,
                        (IMG_UINT32 *)g_PCIDriver.pSysDevice->uiMemoryCPUVirtual,
                        (IMG_UINT32)g_PCIDriver.pSysDevice->uiMemorySize,
                        0);
        SYSDEVU_SetDeviceOps(&sysdevu_device, &sysdevu_ops);
        sysdevu_device.pPrivate = g_PCIDriver.pSysDevice;
        sysdevu_device.native_device = &dev->dev;

    #if defined(IMGVIDEO_ION)
        ui32Result = SYSMEMKM_AddIONMemory(&sysdevu_device, &sysdevu_device.sMemPool);
    #elif defined(FPGA_BUS_MASTERING)
        ui32Result = SYSMEMKM_AddSystemMemory(&sysdevu_device, &sysdevu_device.sMemPool);
    #else
        ui32Result = SYSMEMKM_AddCarveoutMemory(&sysdevu_device,
                                                (IMG_UINTPTR)sysdevu_device.pui32KmMemBase,
                                                sysdevu_device.paPhysMemBase,
                                                sysdevu_device.ui32MemSize,
                                                &sysdevu_device.sMemPool);
    #endif

        if (ui32Result != IMG_SUCCESS)
        {
            printk(KERN_ERR "SYSMEMKM_AddCarveoutMemory failed %d\n", ui32Result);
            goto map_error;
        }

    #if defined(SYSMEM_DMABUF_IMPORT)
        /* FIXME temporary use of secure pool for testing dmabuf import/export */
        ui32Result = SYSMEMKM_AddDMABufMemory(&sysdevu_device,
                &sysdevu_device.secureMemPool);
        if (ui32Result != IMG_SUCCESS)
        {
            printk(KERN_ERR "SYSMEMKM_AddDMABufMemory failed %d\n", ui32Result);
            goto map_error;
        }
    #endif

        ui32Result = SYSDEVU_RegisterDevice(&sysdevu_device);
        if (ui32Result != IMG_SUCCESS)
        {
            printk(KERN_ERR "SYSDEVU_RegisterDevice failed %d\n", ui32Result);
            goto sysdevu_register_error;
        }

        g_PCIDriver.pSysDevice->data = &sysdevu_device;
    }
#endif // IMGVIDEO_ALLOC
    
    // and at last call the probe success callback
    if ( g_PCIDriver.pSysDevice->probeSuccess )
    {
        int ret;
        SYS_DBG(KERN_INFO "Calling probe...\n");
        if ( (ret=g_PCIDriver.pSysDevice->probeSuccess(g_PCIDriver.pSysDevice)) != 0 )
        {
            //error - this is not our device
            printk(KERN_WARNING "Driver found but refused device!\n");
            goto probe_success_error;
        }

#ifdef IMG_SCB_DRIVER
        if ( IMG_SYS_ProbeSCB(dev) != IMG_SUCCESS )
        {
            goto probe_success_error;
        }
#endif

        IMG_SYS_ResetHW(); // also disables any interrupt line that was enabled
        SYS_DevClearInterrupts(NULL); // clear whatever interrupts was there
    }
    else
    {
        printk(KERN_ERR "Driver found but not probe success handle given!\n");
        goto probe_success_error;
    }

    return 0;
    
probe_success_error:
#if defined(IMGVIDEO_ALLOC)
    g_PCIDriver.pSysDevice->data = NULL;
    SYSDEVU_UnRegisterDevice(&sysdevu_device);
#endif
#if defined(IMGVIDEO_ALLOC)
sysdevu_register_error:
    SYSMEMU_RemoveMemoryHeap(sysdevu_device.sMemPool);
#endif
map_error:
    if ( g_PCIDriver.pSysDevice->uiRegisterSize != 0 )
    {
        iounmap((void __iomem*)g_PCIDriver.pSysDevice->uiRegisterCPUVirtual);
        g_PCIDriver.pSysDevice->uiRegisterSize = 0;
    }
    if ( g_PCIDriver.pSysDevice->uiMemorySize != 0 )
    {
        iounmap((void __iomem*)g_PCIDriver.pSysDevice->uiMemoryCPUVirtual);
        g_PCIDriver.pSysDevice->uiMemorySize = 0;
    }
    if ( g_PCIDriver.pSysDevice->uiBoardSize != 0 )
    {
        iounmap((void __iomem*)g_PCIDriver.pSysDevice->uiBoardCPUVirtual);
        g_PCIDriver.pSysDevice->uiBoardSize = 0;
    }
interrupt_error:
    pci_release_regions(dev);
request_error:
    printk(KERN_WARNING "failed to request HW\n");
    g_PCIDriver.pDev = NULL;
    pci_disable_device(dev);
enable_error:
    return -ENODEV;
}

static void IMG_SYS_RemovePCI(struct pci_dev *dev)
{
    SYS_DBG(KERN_INFO "Remove iomap PCI memory\n");

#ifdef IMG_SCB_DRIVER
    i2c_img_remove(dev);
#endif

    if ( g_PCIDriver.pSysDevice->uiRegisterSize != 0 )
    {
        // disabling interrupts done when free the irq line
        //iowrite32(0, (void*)g_PCIDriver.pSysDevice->uiBoardCPUVirtual+g_PCIDriver.uiEnableOffset);
        // re-enable "slave address" mode which is default mode for FPGA
        iowrite32(0x1, (void*)(g_PCIDriver.pSysDevice->uiBoardCPUVirtual + g_PCIDriver.uiTestCtrlOffset));

        iounmap((void __iomem*)g_PCIDriver.pSysDevice->uiRegisterCPUVirtual);
        g_PCIDriver.pSysDevice->uiRegisterCPUVirtual = 0;
        g_PCIDriver.pSysDevice->uiRegisterPhysical = 0;
        g_PCIDriver.pSysDevice->uiRegisterSize = 0;
    }
    if ( g_PCIDriver.pSysDevice->uiMemorySize != 0 )
    {
        iounmap((void __iomem*)g_PCIDriver.pSysDevice->uiMemoryCPUVirtual);
        g_PCIDriver.pSysDevice->uiMemoryCPUVirtual = 0;
        g_PCIDriver.pSysDevice->uiMemoryPhysical = 0;
        g_PCIDriver.pSysDevice->uiMemorySize = 0;
    }
    if ( g_PCIDriver.pSysDevice->uiBoardSize != 0 )
    {
        iounmap((void __iomem*)g_PCIDriver.pSysDevice->uiBoardCPUVirtual);
        g_PCIDriver.pSysDevice->uiBoardCPUVirtual = 0;
        g_PCIDriver.pSysDevice->uiBoardPhysical = 0;
        g_PCIDriver.pSysDevice->uiBoardSize = 0;
    }

    if( g_PCIDriver.pDev != NULL )
    {
        SYS_DBG(KERN_INFO "Remove PCI device\n");

        pci_release_regions(dev);
        pci_disable_device(dev);
        g_PCIDriver.pDev = NULL;
    }
    else
    {
        SYS_DBG(KERN_ERR "PCI remove callback called when its device is NULL\n");
    }
}

static int img_suspend(struct pci_dev * pdev, pm_message_t state)
{
    int ret = 0;
    /* note: state is one of these:
       PMSG_INVALID PMSG_ON PMSG_FREEZE PMSG_QUIESCE PMSG_SUSPEND PMSG_HIBERNATE PMSG_RESUME
       PMSG_THAW PMSG_RESTORE PMSG_RECOVER PMSG_USER_SUSPEND PMSG_USER_RESUME PMSG_REMOTE_RESUME
       PMSG_AUTO_SUSPEND PMSG_AUTO_RESUME
    */
    dev_info(&pdev->dev, "%s state:%d\n", __func__, state.event);

#ifdef IMG_SCB_DRIVER
    i2c_img_suspend_noirq(pdev);
#endif

    if ( g_PCIDriver.pSysDevice && g_PCIDriver.pSysDevice->suspend )
    {
        ret=g_PCIDriver.pSysDevice->suspend(g_PCIDriver.pSysDevice, state);
    }

    return ret;
}

static int img_resume(struct pci_dev * pdev)
{
    int ret = 0;
    dev_info(&pdev->dev, "%s\n", __func__);
    IMG_ASSERT(pdev);

#ifdef IMG_SCB_DRIVER
    i2c_img_resume(pdev);
#endif

    if ( g_PCIDriver.pSysDevice && g_PCIDriver.pSysDevice->resume )
    {
        ret=g_PCIDriver.pSysDevice->resume(g_PCIDriver.pSysDevice);
    }

    return ret;
}

/*
 * functions accessible from outside
 */

IMG_RESULT SYS_DevRegister(SYS_DEVICE *pDevice)
{
    if ( g_PCIDriver.pSysDevice != NULL )
    {
        printk(KERN_ERR "device already detected\n");
        return IMG_ERROR_ALREADY_INITIALISED;
    }

    g_PCIDriver.sDriver.name = pDevice->pszDeviceName;
    g_PCIDriver.sDriver.suspend = img_suspend;
    g_PCIDriver.sDriver.resume = img_resume;
    g_PCIDriver.pSysDevice = pDevice;

    if ( pci_register_driver(&(g_PCIDriver.sDriver)) != 0 )
    {
        printk(KERN_ERR "failed to register PCI driver\n");
        return IMG_ERROR_FATAL;
    }

    SYS_DBG(KERN_INFO "PCI driver registered\n"); // can add vendor and device id found

    if ( !g_PCIDriver.pDev )
    {
        printk(KERN_WARNING "%s: PCI board not found\n", pDevice->pszDeviceName);
    }

    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevClearInterrupts(SYS_DEVICE *pDevice)
{
    if ( pDevice != NULL )
    {
        /// @ use container_of to get the PCIDriver rather than globals
        IMG_UINT32 val = ioread32((void __iomem*)pDevice->uiBoardCPUVirtual+g_PCIDriver.uiStatusOffset);
        SYS_DBG(KERN_INFO "FPGA int status before clear 0x%x\n", val);
        if (0 == val)
        {
            val = 0xFFFF;
        }
        iowrite32(val, (void __iomem*)pDevice->uiBoardCPUVirtual+g_PCIDriver.uiClearOffset);

        val = ioread32((void __iomem*)pDevice->uiBoardCPUVirtual+g_PCIDriver.uiStatusOffset);
        SYS_DBG(KERN_INFO "FPGA int status after clear 0x%x\n", val);

#if defined(APOLLO_MEM_EXTENSION)
        // read additional interrupt status
        val = ioread32((void __iomem*)pDevice->uiBoardCPUVirtual+APOLLO_MEM_EXTENSION_OFFSET+0xC0);
        SYS_DBG(KERN_INFO "FPGA int2 status before clear 0x%x\n", val);
        if (0 == val)
        {
            val = 0xFFFF;
        }
        iowrite32(val, (void __iomem*)pDevice->uiBoardCPUVirtual+APOLLO_MEM_EXTENSION_OFFSET+0xD0);

        val = ioread32((void __iomem*)pDevice->uiBoardCPUVirtual+APOLLO_MEM_EXTENSION_OFFSET+0xC0);
        SYS_DBG(KERN_INFO "FPGA int2 status after clear 0x%x\n", val);
#endif

        return IMG_SUCCESS;
    }
    return IMG_ERROR_NOT_INITIALISED;
}

IMG_RESULT SYS_DevRemove(SYS_DEVICE *pDevice)
{
    IMG_SYS_ResetHW(); // also disables any interrupt line that was enabled - normally SYS_DevFreeIRQ should have been called beforehand!

    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevDeregister(SYS_DEVICE *pDevice)
{
#if defined(IMGVIDEO_ALLOC)
    SYSDEVU_sInfo *psSysdev = g_PCIDriver.pSysDevice->data;
#endif
    
    SYS_DBG(KERN_INFO "Unregister PCI driver\n");

#if defined(IMGVIDEO_ALLOC)
    if (psSysdev)
    {
        SYSDEVU_UnRegisterDevice(psSysdev);
        SYSMEMU_RemoveMemoryHeap(psSysdev->sMemPool);
    }
#endif
    
    /* Unregister the driver from the OS */
    pci_unregister_driver(&(g_PCIDriver.sDriver));
    g_PCIDriver.pSysDevice = NULL; // the PCI
    /// @ use container_of to get the PCIDriver rather than using global

    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevRequestIRQ(SYS_DEVICE *pDevice, const char *pszName)
{
    int result = 1;

    if ( pDevice != NULL ) /// @ use container_of to get the PCIDriver rather than using global
    {
        // shared with others, no entropy
        result = request_threaded_irq(
            pDevice->irq_line,
            pDevice->irqHardHandle, pDevice->irqThreadHandle,
            IRQF_SHARED, pszName, pDevice->handlerParam);

        if ( result == 0 )
        {
            // enable interrupts
            iowrite32(PCI_ATLAS_MASTER_ENABLE|PCI_ATLAS_DEVICE_INT,
                    (void __iomem*)pDevice->uiBoardCPUVirtual +
                    g_PCIDriver.uiEnableOffset);

#if defined(APOLLO_MEM_EXTENSION)
            // write enable additional interrupts to 0
            iowrite32(0, (void __iomem*)pDevice->uiBoardCPUVirtual+APOLLO_MEM_EXTENSION_OFFSET+0xC8);
#endif
        }
    }

    if ( result != 0)
    {

        return IMG_ERROR_UNEXPECTED_STATE;
    }
    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevFreeIRQ(SYS_DEVICE *pDevice)
{
    if ( pDevice != NULL )
    {
        // disable interrupts
        /// @ use container_of to fing the PCIDriver rather than the global
        iowrite32(0, (void __iomem*)pDevice->uiBoardCPUVirtual+g_PCIDriver.uiEnableOffset);

        i2c_img_suspend_noirq(g_PCIDriver.pDev);
        SYS_DevClearInterrupts(pDevice);

        free_irq(pDevice->irq_line, pDevice->handlerParam);
        printk(KERN_INFO "disabling interrupts for PCI");
    }

    return IMG_SUCCESS;
}

IMG_RESULT SYS_DevPowerControl(SYS_DEVICE *pDevice, IMG_BOOL8 bDeviceActive)
{
    return IMG_SUCCESS;
}
