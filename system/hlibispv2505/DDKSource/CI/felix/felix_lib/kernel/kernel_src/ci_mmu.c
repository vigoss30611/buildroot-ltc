/**
 ******************************************************************************
 @file ci_mmu.c

 @brief

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

 *****************************************************************************/
#include <ci_kernel/ci_kernel.h>

#include <img_defs.h>
#include <img_errors.h>

#include <mmulib/mmu.h>

#ifdef FELIX_FAKE  // CI_MMU_TAL_ALLOC

#include <talalloc/talalloc.h>

#define TAL_INT_VAR 1  // 0 is used in talalloc.h

#endif  // FELIX_FAKE uses CI_MMU_TAL_ALLOC

#include <target.h>
#include <reg_io2.h>

#include <felix_hw_info.h>

#include <registers/mmu.h>

// enable the MMU priority, see REQUEST_PRIOIRY_ENABLE register
#define USE_MMU_PRIORITY 1
// all requestors use this value, see REQUEST_PRIORITY_ENABLE register
#define USE_REQ_PRIORITY 1

void KRN_CI_MMUDirDestroy(struct MMUDirectory *pMMUDir)
{
    if (pMMUDir != NULL)
    {
        IMGMMU_DirectoryDestroy(pMMUDir);
    }
}

/* done a separated function to allow some flexibility when we will use
 * different allocators */
struct MMUDirectory* KRN_CI_MMUDirCreate(KRN_CI_DRIVER *pDriver,
    IMG_RESULT *ret)
{
    IMGMMU_Info sMMUInfo;

    IMG_MEMSET(&sMMUInfo, 0, sizeof(IMGMMU_Info));
#ifdef FELIX_FAKE /* CI_MMU_TAL_ALLOC */
    sMMUInfo.pfnPageAlloc = IMGMMU_TALPageAlloc;
    sMMUInfo.pfnPageFree = IMGMMU_TALPageFree;
    sMMUInfo.pfnPageWrite = IMGMMU_TALPageWrite;
    // no update otherwise addresses are in pdump
#if 0
    // enable only to debug pagefault and see state of pages before start
    sMMUInfo.pfnPageUpdate = IMGMMU_TALPageUpdate;
#endif

    if (pDriver != NULL)
    {
        IMGMMU_TALPageSetMemRegion(pDriver->sTalHandles.hMemHandle);
    }
#else  // normal system
    sMMUInfo.pfnPageAlloc = &Platform_MMU_MemPageAlloc;
    sMMUInfo.pfnPageFree = &Platform_MMU_MemPageFree;
    // needed to know if extended address range is enabled or not
    sMMUInfo.pfnPageWrite = &Platform_MMU_MemPageWrite;
    sMMUInfo.pfnPageUpdate = &Platform_MMU_MemPageUpdate;
#endif

    return IMGMMU_DirectoryCreate(&sMMUInfo, ret);
}

#if IMG_VIDEO_BUS4_MMU_MMU_TILE_MIN_ADDR_NO_ENTRIES != CI_MMU_HW_TILED_HEAPS
#error number of mmu addr entries changed!
#endif

IMG_RESULT KRN_CI_MMUConfigure(KRN_CI_MMU *pMMU, IMG_UINT8 ui8NDir,
    IMG_UINT8 ui8NRequestor, IMG_BOOL8 bBypass)
{
    IMG_UINT32 reg = 0;  // flush register
    IMG_UINT32 i;
    IMG_UINT32 uiMMUSize, uiVirtSize;
    IMG_UINT32 uiHWRequestors = 0;
    IMG_UINT8 bMMUSupported = 0;
    IMG_RESULT ret;

    IMG_ASSERT(pMMU != NULL);
    IMG_ASSERT(pMMU->hRegFelixMMU != NULL);

    CI_PDUMP_COMMENT(pMMU->hRegFelixMMU, "start");

    reg = 0;
    REGIO_WRITE_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_CONTROL1,
        MMU_SOFT_RESET, 1);  // reset the MMU
    TALREG_WriteWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONTROL1_OFFSET, reg);

#ifdef FELIX_UNIT_TESTS
    // clean the register
    REGIO_WRITE_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_CONTROL1,
        MMU_SOFT_RESET, 0);
    TALREG_WriteWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONTROL1_OFFSET, reg);
#endif

    ret = TALREG_Poll32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONTROL1_OFFSET, TAL_CHECKFUNC_NOTEQUAL,
        1 << IMG_VIDEO_BUS4_MMU_MMU_CONTROL1_MMU_SOFT_RESET_SHIFT,
        IMG_VIDEO_BUS4_MMU_MMU_CONTROL1_MMU_SOFT_RESET_MASK,
        12, 100);
    // has to be at least 1000 cycles wait (asked by HW in BRN48444)

    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to reset the MMU! - POL error %s\n",
            ret == IMG_ERROR_TIMEOUT ? "timeout" : "fatal");
        return IMG_ERROR_FATAL;  // may need a MMU hard reset
    }

    reg = 0;
    TALREG_ReadWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONTROL0_OFFSET, &reg);
    REGIO_WRITE_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_CONTROL0,
        MMU_TILING_SCHEME, pMMU->uiTiledScheme == 512 ? 1 : 0);
    TALREG_WriteWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONTROL0_OFFSET, reg);

    reg = 0;
    TALREG_ReadWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONFIG0_OFFSET, &reg);
    uiMMUSize = 32 + REGIO_READ_FIELD(reg, IMG_VIDEO_BUS4_MMU,
        MMU_CONFIG0, EXTENDED_ADDR_RANGE);
    uiHWRequestors = REGIO_READ_FIELD(reg, IMG_VIDEO_BUS4_MMU,
        MMU_CONFIG0, NUM_REQUESTORS);
    bMMUSupported = REGIO_READ_FIELD(reg, IMG_VIDEO_BUS4_MMU,
        MMU_CONFIG0, MMU_SUPPORTED);

    reg = 0;
    REGIO_WRITE_FIELD(reg, IMG_VIDEO_BUS4_MMU, REQUEST_PRIORITY_ENABLE,
        CMD_MMU_PRIORITY_ENABLE, USE_MMU_PRIORITY);
    for (i = 0; i < uiHWRequestors; i++)
    {
        REGIO_WRITE_REPEATED_FIELD(reg, IMG_VIDEO_BUS4_MMU,
            REQUEST_PRIORITY_ENABLE, CMD_PRIORITY_ENABLE, i, USE_REQ_PRIORITY);
    }
    TALREG_WriteWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_REQUEST_PRIORITY_ENABLE_OFFSET, reg);

    if (bBypass)
    {
        KRN_CI_MMUBypass(pMMU, IMG_TRUE);

        ret = KRN_CI_MMUPause(pMMU, IMG_FALSE);
        // because may have been paused in previous rmmod
        if (IMG_SUCCESS != ret)
        {
            CI_FATAL("Failed to unpause the MMU after putting it into "\
                "bypass!\n");
            return IMG_ERROR_FATAL;
        }
        return IMG_SUCCESS;
    }

    // this is the size of the HW not the size used
    CI_INFO("Using MMU of %u Bits\n", uiMMUSize);

    // stop the MMU until it is programmed correctly
    ret = KRN_CI_MMUPause(pMMU, IMG_TRUE);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("Failed to pause the MMU before configuring it!\n");
        return IMG_ERROR_FATAL;
    }

    uiVirtSize = IMGMMU_GetVirtualSize();

    if (pMMU->bEnableExtAddress == IMG_FALSE)
    {
        uiMMUSize = 32;
    }

    if (bMMUSupported == 0)
    {
        CI_FATAL("HW MMU is not enabled - cannot configure the driver to "\
            "use it!\n");
        return IMG_ERROR_FATAL;
    }

#ifdef FELIX_FAKE /* TAL_ALLOC */
    // set the size of the MMU to be what was read in register
    IMGMMU_TALSetMMUSize(uiMMUSize);

    CI_PDUMP_COMMENT(pMMU->hRegFelixMMU, "check MMU HW");
    TALREG_ReadWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONFIG0_OFFSET, &reg);
    TALREG_Poll32(pMMU->hRegFelixMMU, IMG_VIDEO_BUS4_MMU_MMU_CONFIG0_OFFSET,
        TAL_CHECKFUNC_ISEQUAL, reg,
        (IMG_VIDEO_BUS4_MMU_MMU_CONFIG0_EXTENDED_ADDR_RANGE_MASK
        | IMG_VIDEO_BUS4_MMU_MMU_CONFIG0_MMU_SUPPORTED_MASK
        | IMG_VIDEO_BUS4_MMU_MMU_CONFIG0_NUM_REQUESTORS_MASK),
        CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);

    /*reg = 0;
    TALREG_ReadWord32(pMMU->hRegFelixMMU,
    IMG_VIDEO_BUS4_MMU_MMU_CONFIG1_OFFSET, &reg);
    TALREG_Poll32(pMMU->hRegFelixMMU,
    IMG_VIDEO_BUS4_MMU_MMU_CONFIG1_OFFSET, TAL_CHECKFUNC_ISEQUAL,
    reg, ~0, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);*/

#if 0  // re-enable when simulator is fixed
    reg = 0;
    TALREG_ReadWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_VERSION_OFFSET, &reg);
    TALREG_Poll32(pMMU->hRegFelixMMU, IMG_VIDEO_BUS4_MMU_MMU_VERSION_OFFSET,
        TAL_CHECKFUNC_ISEQUAL, reg, ~0, CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
#endif

    CI_PDUMP_COMMENT(pMMU->hRegFelixMMU, "check MMU done");
#endif /* FELIX_FAKE */

    reg = 0;
    for (i = 0; i < ui8NRequestor; i++)
    {
        REGIO_WRITE_REPEATED_FIELD(reg, IMG_VIDEO_BUS4_MMU,
            MMU_BANK_INDEX, MMU_BANK_INDEX, i, pMMU->aRequestors[i]);
    }
    TALREG_WriteWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_BANK_INDEX_OFFSET, reg);

    reg = 0;  // to be used for MMU_CONTROL1
    for (i = 0; i < ui8NDir; i++)
    {
#ifdef FELIX_FAKE /* using TAL alloc */
        IMG_HANDLE sTalDirAddr =
            IMGMMU_TALDirectoryGetMemHandle(pMMU->apDirectory[i]);

        // the address needs to be shifted if the MMU is more than 32b
        if (uiMMUSize > uiVirtSize)  // uiVirtSize is always 32
        {
            ret = TALINTVAR_WriteMemRef(sTalDirAddr, 0, pMMU->hRegFelixMMU,
                TAL_INT_VAR);

            if (IMG_SUCCESS == ret)
            {
                /* does a right shift to fit a 40b address in 32b
                 * - if not used and the MMU is more than 32b the
                 * "upper address" register of the MMU HW can be used to
                 * set which 4GB of the memory should be used */
                ret = TALINTVAR_RunCommand(TAL_PDUMPL_INTREG_SHR,
                    pMMU->hRegFelixMMU, TAL_INT_VAR, pMMU->hRegFelixMMU,
                    TAL_INT_VAR, pMMU->hRegFelixMMU,
                    (uiMMUSize - uiVirtSize), IMG_FALSE);
            }

            if (IMG_SUCCESS == ret)
            {
                TALINTVAR_WriteToReg32(pMMU->hRegFelixMMU,
                    REGIO_TABLE_OFF(IMG_VIDEO_BUS4_MMU, MMU_DIR_BASE_ADDR, i),
                    pMMU->hRegFelixMMU, TAL_INT_VAR);
            }
        }
        else
        {
            TALREG_WriteDevMemRef32(pMMU->hRegFelixMMU,
                REGIO_TABLE_OFF(IMG_VIDEO_BUS4_MMU, MMU_DIR_BASE_ADDR, i),
                sTalDirAddr, 0);
        }
#else
        IMGMMU_Page *pPage = IMGMMU_DirectoryGetPage(pMMU->apDirectory[i]);
        // has to write 32b to the HW
        IMG_UINT32 mmuAddress = (IMG_UINT32)(pPage->uiPhysAddr);

        if ( uiMMUSize > 32 )
        {
            /* the HW is expected a shifted value when mmu has extended
             * address enabled */
            mmuAddress = (IMG_UINT32)(pPage->uiPhysAddr>>(uiMMUSize-32));
        }

        CI_DEBUG("MMU dir %d phys=0x%x, written to mmu=0x%x (mmusize=%d)\n",
            i, (IMG_UINT32)pPage->uiPhysAddr, mmuAddress, uiMMUSize);
        TALREG_WriteWord32(pMMU->hRegFelixMMU,
            REGIO_TABLE_OFF(IMG_VIDEO_BUS4_MMU, MMU_DIR_BASE_ADDR, i),
            mmuAddress);
#endif
        REGIO_WRITE_REPEATED_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_CONTROL1,
            MMU_INVALDC, i, 1);
    }
    TALREG_WriteWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONTROL1_OFFSET, reg);

    // configure tiling ranges
    for (i = 0; i < CI_MMU_HW_TILED_HEAPS; i++)
    {
        TALREG_WriteWord32(pMMU->hRegFelixMMU,
            REGIO_TABLE_OFF(IMG_VIDEO_BUS4_MMU, MMU_TILE_MIN_ADDR, i),
            pMMU->aHeapStart[CI_TILED_IMAGE_HEAP0 + i]);
        TALREG_WriteWord32(pMMU->hRegFelixMMU,
            REGIO_TABLE_OFF(IMG_VIDEO_BUS4_MMU, MMU_TILE_MAX_ADDR, i),
            pMMU->aHeapStart[CI_TILED_IMAGE_HEAP0 + i]
            + pMMU->aHeapSize[CI_TILED_IMAGE_HEAP0 + i] - 1);
        // inclusive address
    }

    /* written to insure it is the value we need not the one left from the
     * driver running before */
    KRN_CI_MMUBypass(pMMU, IMG_FALSE);
    ret = KRN_CI_MMUPause(pMMU, IMG_FALSE);
    if (IMG_SUCCESS != ret)
    {
        CI_FATAL("failed to unpause the MMU after configuring it!\n");
        return IMG_ERROR_FATAL;
    }

    CI_PDUMP_COMMENT(pMMU->hRegFelixMMU, "done");

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_MMUFlushCache(KRN_CI_MMU *pMMU, IMG_UINT8 uiDirectory,
    IMG_BOOL8 bFlushAll)
{
    IMG_UINT32 reg = 0;
#ifdef FELIX_FAKE
    char message[64];

    snprintf(message, sizeof(message), "MMU dir %d: %s", uiDirectory,
        bFlushAll ? "INVALDC" : "FLUSH");
    CI_PDUMP_COMMENT(pMMU->hRegFelixMMU, message);
#endif

    IMG_ASSERT(pMMU != NULL);

    if (bFlushAll == IMG_TRUE)
    {
        REGIO_WRITE_REPEATED_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_CONTROL1,
            MMU_INVALDC, uiDirectory, 1);
    }
    else
    {
        REGIO_WRITE_REPEATED_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_CONTROL1,
            MMU_FLUSH, uiDirectory, 1);
    }
    TALREG_WriteWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONTROL1_OFFSET, reg);

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_MMUPause(KRN_CI_MMU *pMMU, IMG_BOOL8 bPause)
{
    IMG_UINT32 reg = 0;
    IMG_RESULT ret;
    IMG_ASSERT(pMMU != NULL);

    if (bPause)
    {
        REGIO_WRITE_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_CONTROL1,
            MMU_PAUSE_SET, 1);
        CI_PDUMP_COMMENT(pMMU->hRegFelixMMU, "Pause MMU");
    }
    else
    {
        REGIO_WRITE_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_CONTROL1,
            MMU_PAUSE_CLEAR, 1);
        CI_PDUMP_COMMENT(pMMU->hRegFelixMMU, "Un-pause MMU");
    }
    TALREG_WriteWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONTROL1_OFFSET, reg);

    ret = TALREG_Poll32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONTROL1_OFFSET, TAL_CHECKFUNC_ISEQUAL,
        bPause << IMG_VIDEO_BUS4_MMU_MMU_CONTROL1_MMU_PAUSE_CLEAR_SHIFT,
        IMG_VIDEO_BUS4_MMU_MMU_CONTROL1_MMU_PAUSE_CLEAR_MASK,
        CI_REGPOLL_TRIES, CI_REGPOLL_TIMEOUT);
    if (IMG_SUCCESS != ret)
    {
#if defined(FELIX_FAKE) || defined(ANDROID_EMULATOR)
        /* FELIX_FAKE because: BRN51166 sim did not implement auto-clear
         * for the pause bit */
        return IMG_SUCCESS;
#endif
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_MMUBypass(KRN_CI_MMU *pMMU, IMG_BOOL8 bBypass)
{
    IMG_UINT32 reg = 0;
    IMG_ASSERT(pMMU != NULL);

    CI_DEBUG("MMU bypass %d, ext address %d\n", bBypass,
        pMMU->bEnableExtAddress);

    REGIO_WRITE_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_ADDRESS_CONTROL,
        MMU_BYPASS, bBypass);
    REGIO_WRITE_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_ADDRESS_CONTROL,
        MMU_ENABLE_EXT_ADDRESSING, pMMU->bEnableExtAddress);
    TALREG_WriteWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_ADDRESS_CONTROL_OFFSET, reg);

    return IMG_SUCCESS;
}

IMG_RESULT KRN_CI_MMUSetTiling(KRN_CI_MMU *pMMU, IMG_BOOL8 bEnable,
    unsigned int uiTileStride, IMG_UINT32 ui32TilingHeap)
{
    IMG_UINT32 reg = 0;
    IMG_UINT8 tileStrideLog2 = 0;
    IMG_UINT32 tileStr = uiTileStride;

    IMG_ASSERT(pMMU != NULL);
    IMG_ASSERT(ui32TilingHeap < CI_MMU_HW_TILED_HEAPS);

    CI_PDUMP_COMMENT(pMMU->hRegFelixMMU, "start");

    while (tileStr >>= 1)  // compute log2()
    {
        tileStrideLog2++;
    }
    // tile stride has to be a power of 2
    IMG_ASSERT(1 << tileStrideLog2 == uiTileStride);

    // if using 256Bx16 stride of 512 = 0, if using 512Bx8 stride of 1024 = 0
    if (pMMU->uiTiledScheme == 512)
    {
        IMG_ASSERT(tileStrideLog2 >= 10);
        tileStrideLog2 -= 10;  // 9+tiling scheme
    }
    else
    {
        IMG_ASSERT(tileStrideLog2 >= 9);
        tileStrideLog2 -= 9;  // 9+tiling scheme
    }

    // tiling addesses are written in KRN_CI_MMUConfigure()
    REGIO_WRITE_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_TILE_CFG,
        TILE_ENABLE, bEnable);
    REGIO_WRITE_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_TILE_CFG,
        TILE_STRIDE, tileStrideLog2);
    TALREG_WriteWord32(pMMU->hRegFelixMMU,
        REGIO_TABLE_OFF(IMG_VIDEO_BUS4_MMU, MMU_TILE_CFG, ui32TilingHeap),
        reg);

    CI_PDUMP_COMMENT(pMMU->hRegFelixMMU, "done");

    return IMG_SUCCESS;
}

IMG_UINT32 KRN_CI_MMUTilingAlignment(IMG_UINT32 ui32TilingStride)
{
    IMG_UINT32 tilingAlign = 0;  // 1<<(x_tile_stride+8+5)

    if (0 == ui32TilingStride)
    {
        return 0;
    }

    switch (ui32TilingStride)
    {
    case 512:
        tilingAlign = 1 << 13;
        break;
    case 1024:
        tilingAlign = 1 << 14;
        break;
    case 2048:
        tilingAlign = 1 << 15;
        break;
    case 4096:
        tilingAlign = 1 << 16;
        break;
    case 8192:
        tilingAlign = 1 << 17;
        break;
    case 16384:
        tilingAlign = 1 << 18;
        break;
    default:
    {
        IMG_UINT8 tileStrideLog2 = 0;
        IMG_UINT32 tileStr = ui32TilingStride;

        while (tileStr >>= 1)  // compute log2()
        {
            tileStrideLog2++;
        }
        // tile stride has to be a power of 2
        IMG_ASSERT(1 << tileStrideLog2 == ui32TilingStride);

        IMG_ASSERT(tileStrideLog2 >= 9);
        // x_tile_stride register value as if 256x16 is used
        tileStrideLog2 -= 9;

        tileStrideLog2 = tileStrideLog2 + 8 + 5;
        tilingAlign = 1 << tileStrideLog2;
    }
    }

    if (512 == g_psCIDriver->sMMU.uiTiledScheme)
    {
        // because the tiling scheme is 2x more than the value for the switch
        tilingAlign <<= 1;
    }

    return tilingAlign;
}

IMG_UINT32 KRN_CI_MMUSize(KRN_CI_MMU *pMMU)
{
    IMG_UINT32 reg = 0;
    IMG_UINT32 uiMMUSize = 0;

    TALREG_ReadWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_CONFIG0_OFFSET, &reg);
    uiMMUSize = 32 + REGIO_READ_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_CONFIG0,
        EXTENDED_ADDR_RANGE);

    TALREG_ReadWord32(pMMU->hRegFelixMMU,
        IMG_VIDEO_BUS4_MMU_MMU_ADDRESS_CONTROL_OFFSET, &reg);
    if (REGIO_READ_FIELD(reg, IMG_VIDEO_BUS4_MMU, MMU_ADDRESS_CONTROL,
        MMU_ENABLE_EXT_ADDRESSING) == 0)
    {
        uiMMUSize = 32;
    }
    return uiMMUSize;
}
