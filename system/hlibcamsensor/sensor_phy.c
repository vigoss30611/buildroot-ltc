/**
******************************************************************************
@file sensor_phy.c

@brief Implementation phy control for IMG phy on fpga systems

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

#include <img_types.h>
#include <img_errors.h>
#include <ci/ci_api.h>
#include <stdlib.h>
#include <stdio.h>
#include "sensorapi/pciuser.h"
#include "sensorapi/sensor_phy.h"
#if !file_i2c
#include <sys/param.h>
#include <dirent.h>
#ifdef INFOTM_ISP
#include <fcntl.h>
#endif //INFOTM_ISP
#define IMG_I2C_NAME    "ImgTec SCB I2C"
#endif

#define LOG_TAG "SensorPHY"
#include <felixcommon/userlog.h>

#ifdef INFOTM_ISP
//#include <registers/context0.h>
enum FELIX_CONTEXT0_IMGR_CTRL_IMGR_BAYER_FORMAT_ENUM {
    /**
     * @brief Start on a line of Blue-Green followed by a line of Green-Red
     */
    FELIX_CONTEXT0_IMGR_BAYER_FORMAT_BGGR = 0x3,
    /**
     * @brief Start on a line of Green-Blue, followed by a line of Red-Green
     */
    FELIX_CONTEXT0_IMGR_BAYER_FORMAT_GBRG = 0x2,
    /**
     * @brief Start on a line of Green-Red followed by a line of Blue-Green
     */
    FELIX_CONTEXT0_IMGR_BAYER_FORMAT_GRBG = 0x1,
    /**
     * @brief Start on a line of Red-Green followed by a line of Green-Blue.
     */
    FELIX_CONTEXT0_IMGR_BAYER_FORMAT_RGGB = 0x0,
};
#endif //INFOTM_ISP

#define APOLLO_DEVICE_ID 0x1cf2
#define APOLLO_VENDOR_ID 0x1010
#define PHY_BAR 1
#define PHY_OFFSET_N 3
// offset for HW version 1, 2 and 3
static const IMG_UINT32 PHY_OFFSET[PHY_OFFSET_N] = {
    0xa000, // felix 1.x
    0x11000, // felix 2.x
    0x16000, // felix 3.x
};
#define MIPI_ENABLE_CNTR 0x0c
#define SENSOR_SELECT 0x18

#ifdef INFOTM_ISP

/* Local define. Add by feng.qu@infotm.com */
#define CI_BANK_CTX 2
#define IMGR_CTRL 0x00D0
#define BIT_IMGR_BAYER_FORMAT_SHIFT 16
#define BIT_IMGR_BAYER_FORMAT_WIDTH 2
#endif //INFOTM_ISP
SENSOR_PHY* SensorPhyInit(void)
{
    SENSOR_PHY *phy = (SENSOR_PHY*)IMG_CALLOC(1, sizeof(*phy));
    unsigned int phy_off = 0;

    if (!phy)
    {
        LOG_ERROR("Failed to allocate internal structure\n");
        return NULL;
    }

#ifdef FELIX_FAKE
    // when doing fake driver we can't access registers
    return phy;
#else

    if (CI_DriverInit(&(phy->psConnection)) != IMG_SUCCESS)
    {
        LOG_ERROR("Failed to open connection to Felix Driver\n");
        goto err_connection;
    }

    if ( phy->psConnection->sHWInfo.rev_ui8Major > PHY_OFFSET_N ||
         phy->psConnection->sHWInfo.rev_ui8Major < 1)
    {
        LOG_ERROR("unsupported HW version %d.%d, supported version 1.x to " \
            "%d.x\n", phy->psConnection->sHWInfo.rev_ui8Major,
            phy->psConnection->sHWInfo.rev_ui8Minor,
            PHY_OFFSET_N
        );
        goto err_version;
    }
    phy_off = phy->psConnection->sHWInfo.rev_ui8Major-1;
    if ( phy->psConnection->sHWInfo.rev_ui8Major == 2 &&
         phy->psConnection->sHWInfo.rev_ui8Minor >= 3 )
    {
        // 2.3 and 2.4 have same offsets than 3.0
        phy_off = phy->psConnection->sHWInfo.rev_ui8Major;
    }

    LOG_DEBUG("Mapping PHY to user space using phy offset 0x%x\n",
        PHY_OFFSET[phy_off]);
#ifndef INFOTM_ISP		
    phy->puiPhyRegs =
        UserGetMapping(APOLLO_DEVICE_ID, APOLLO_VENDOR_ID, PHY_BAR,
            PHY_OFFSET[phy_off],
            1*1024, &phy->sPhyRegs);
#endif //not INFOTM_ISP
#if defined(__LINUX__)
    if (!phy->puiPhyRegs)
    {
        LOG_ERROR("failed to get user-mapping of the PHY registers\n");
        goto err_version;
    }
#endif

    phy->psGasket = (CI_GASKET*)IMG_CALLOC(1, sizeof(CI_GASKET));
    if (!phy->psGasket)
    {
        LOG_ERROR("Failed to allocate gasket struct\n");
        goto err_gasket;
    }
    CI_GasketInit(phy->psGasket);

    phy->psGasket->bParallel = IMG_FALSE; // default
    return phy;

err_gasket:
    IMG_FREE(phy->psGasket);
    CloseUserMapping(&phy->sPhyRegs);
err_version:
    CI_DriverFinalise(phy->psConnection);
err_connection:
    IMG_FREE(phy);
    phy = NULL;
    return phy;
#endif /* FELIX_FAKE */
}

IMG_RESULT SensorPhyCtrl(SENSOR_PHY *psSensorPhy, IMG_BOOL bEnable,
        IMG_UINT8 uiMipiLanes, IMG_UINT8 ui8SensorNum)
{
    IMG_RESULT ret = IMG_SUCCESS;

#ifndef FELIX_FAKE
    if (psSensorPhy->psGasket == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = bEnable
        ? CI_GasketAcquire(psSensorPhy->psGasket, psSensorPhy->psConnection)
        : CI_GasketRelease(psSensorPhy->psGasket, psSensorPhy->psConnection);

    if (ret != IMG_SUCCESS)
    {
        LOG_ERROR("failed to %s the gasket %d!\n",
                bEnable ? "acquire" : "release", psSensorPhy->psGasket->uiGasket);
        return ret;
    }
    LOG_INFO("%s gasket %d, %s PHY ...\n",
            bEnable ? "Acquired" : "Released",
            psSensorPhy->psGasket->uiGasket,
            bEnable ? "enabling" : "disabling");
#ifndef INFOTM_ISP			
    psSensorPhy->puiPhyRegs[MIPI_ENABLE_CNTR / 4] = bEnable ? uiMipiLanes : 0;
    psSensorPhy->puiPhyRegs[SENSOR_SELECT / 4] = ui8SensorNum;
#endif //not INFOTM_ISP
#endif /* FELIX_FAKE */
    return ret;
}

#ifdef INFOTM_ISP
/* Add by feng.qu@infotm.com */
IMG_RESULT SensorPhyBayerFormat(SENSOR_PHY *psSensorPhy, enum MOSAICType bayerFmt)
{
    IMG_RESULT ret;
    enum FELIX_CONTEXT0_IMGR_CTRL_IMGR_BAYER_FORMAT_ENUM eBayerFormat = 
        FELIX_CONTEXT0_IMGR_BAYER_FORMAT_RGGB;
    IMG_UINT32 value;

    if (psSensorPhy->psGasket == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    switch(bayerFmt)
    {
    case MOSAIC_RGGB:
        eBayerFormat = FELIX_CONTEXT0_IMGR_BAYER_FORMAT_RGGB;
        break;
    case MOSAIC_GRBG:
        eBayerFormat = FELIX_CONTEXT0_IMGR_BAYER_FORMAT_GRBG;
        break;
    case MOSAIC_GBRG:
        eBayerFormat = FELIX_CONTEXT0_IMGR_BAYER_FORMAT_GBRG;
        break;
    case MOSAIC_BGGR:
        eBayerFormat = FELIX_CONTEXT0_IMGR_BAYER_FORMAT_BGGR;
        break;
    default:
        LOG_ERROR("user-side mosaic is unknown - using RGGB\n");
    }

    ret = CI_DriverDebugRegRead(psSensorPhy->psConnection, CI_BANK_CTX+0, IMGR_CTRL, &value);
    if (ret) {
        LOG_ERROR("CI_DriverDebugRegRead ebank=%d, offset=0x%x failed %d\n", CI_BANK_CTX+0, IMGR_CTRL, ret);
        return ret;
    }

    value &= ~(((1 << BIT_IMGR_BAYER_FORMAT_WIDTH) -1) << BIT_IMGR_BAYER_FORMAT_SHIFT);
    value |= (eBayerFormat << BIT_IMGR_BAYER_FORMAT_SHIFT);

    ret = CI_DriverDebugRegWrite(psSensorPhy->psConnection, CI_BANK_CTX+0, IMGR_CTRL, value);
    if (ret) {
        LOG_ERROR("CI_DriverDebugRegWrite ebank=%d, offset=0x%x failed %d\n", CI_BANK_CTX+0, IMGR_CTRL, ret);
        return ret;
    }

    return IMG_SUCCESS;
}
#endif //INFOTM_ISP

void SensorPhyDeinit(SENSOR_PHY *psSensorPhy)
{
    IMG_ASSERT(psSensorPhy);
#ifndef FELIX_FAKE
    IMG_ASSERT(psSensorPhy->psGasket);
    IMG_ASSERT(psSensorPhy->psConnection);

    CloseUserMapping(&psSensorPhy->sPhyRegs);
    IMG_FREE(psSensorPhy->psGasket);
    CI_DriverFinalise(psSensorPhy->psConnection);
#endif /* FELIX_FAKE */
    IMG_FREE(psSensorPhy);
}

#ifdef INFOTM_ISP
IMG_RESULT find_i2c_dev(char *i2c_dev_path, unsigned int len, unsigned int addr, const char *i2c_adaptor)
#else
IMG_RESULT find_i2c_dev(char *i2c_dev_path, unsigned int len)
#endif //INFOTM_ISP
{
#if !file_i2c
    FILE *file;
    DIR *dir;
    struct dirent *dirEntry;
    char sysfs[NAME_MAX], fstype[NAME_MAX], line[NAME_MAX];
#ifdef INFOTM_ISP	
	char temp[NAME_MAX];
#endif //INFOTM_ISP	
    unsigned found = 0;

    if (len < NAME_MAX) {
        fprintf(stderr, "%s: Wrong parameters, len = %u. Minimum length "
                "should be %u\n", __FILE__, len, NAME_MAX);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    /* Try to find out where sysfs was mounted */
    if ((file = fopen("/proc/mounts", "r")) == NULL) {
        return IMG_ERROR_DEVICE_NOT_FOUND;
    }
    while (fgets(line, NAME_MAX, file)) {
        sscanf(line, "%*[^ ] %[^ ] %[^ ] %*s\n", sysfs, fstype);
        if (strcasecmp(fstype, "sysfs") == 0) {
            found = 1;
            break;
        }
    }
    fclose(file);
    if (!found)
        return IMG_ERROR_DEVICE_NOT_FOUND;

    /* Get i2c-dev name */
    strcat(sysfs, "/class/i2c-dev");
    if(!(dir = opendir(sysfs)))
        return IMG_ERROR_DEVICE_NOT_FOUND;
    /* go through the busses */
    while ((dirEntry = readdir(dir)) != NULL) {
        if (!strcmp(dirEntry->d_name, "."))
            continue;
        if (!strcmp(dirEntry->d_name, ".."))
            continue;
#ifdef INFOTM_ISP
		sprintf(line, "%s/%s/device", sysfs, dirEntry->d_name);

		if (!strcmp(dirEntry->d_name, i2c_adaptor))
		{
			//strcat(line, "/4-0010");
			sprintf(temp, "/%d-%04x", i2c_adaptor[strlen(i2c_adaptor) - 1] - 48, addr);
			strcat(line, temp);

			if(-1 == access(line, F_OK))
			{
				//printf("i2c slave device driver find failed\n");
				closedir(dir);
				return IMG_ERROR_DEVICE_NOT_FOUND;
			}
			else
			{
				strcpy(i2c_dev_path, "/dev/");
				strcat(i2c_dev_path, dirEntry->d_name);
				//printf("i2c_dev_path = %s\n", i2c_dev_path);
				closedir(dir);
				return IMG_SUCCESS;
			}
		}
    }

#else
        sprintf(line, "%s/%s/name", sysfs, dirEntry->d_name);
        file = fopen(line, "r");
        if (file != NULL) {
            char *name;

            name = fgets(line, NAME_MAX, file);
            fclose(file);
            if (!name) {
                fprintf(stderr, "%s/%s: couldn't read i2c name.\n",
                        sysfs, dirEntry->d_name);
                continue;
            }
            if (strncmp(line, IMG_I2C_NAME, strlen(IMG_I2C_NAME)) == 0) {
                /* Found IMG SCB device */
                strcpy(i2c_dev_path, "/dev/");
                strcat(i2c_dev_path, dirEntry->d_name);
                closedir(dir);
                return IMG_SUCCESS;
            }
        }
    }	
#endif //INFOTM_ISP	
	closedir(dir);
	return IMG_ERROR_DEVICE_NOT_FOUND;

#else
    return IMG_SUCCESS;
#endif // file_i2c
}
