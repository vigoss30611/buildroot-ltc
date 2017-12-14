/**
******************************************************************************
@file pciuser.c

@brief Implementation mapping PCI device to user-space

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
#if defined (__linux__)

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#endif

#include <stdlib.h>
#include "sensorapi/pciuser.h"

#define LOG_TAG "pciuser"
#include <felixcommon/userlog.h>

#define LOG_DEBUG_VERBOSE 0 // to enable verbose mapping logging

void* UserGetMapping(int ui16DeviceID, int ui16VendorID, char ui8Bar,
        unsigned long int ui32Offset, int ui16Size, user_device_mapping *psMapping )
{
#if defined (__linux__)
    struct stat stDirInfo;
    struct dirent * stFiles;
    DIR * stDirIn;
    char szFullName[MAXPATHLEN];
    char szVendorName[MAXPATHLEN];
    char szDeviceName[MAXPATHLEN];
    char szDirectory[MAXPATHLEN];
    char szBar1[MAXPATHLEN];
    struct stat stFileInfo;
    FILE *fpContents;
    char szContents[256];
    int device_id;
    int vendor_id;
    unsigned long int ui32Local32Offset = 0;
    short int mode;
    int slot, function, retval, bus;
    void *mappedAddr = NULL;

    psMapping->mapped = 0;
    strncpy(szDirectory, "/sys/bus/pci/devices", MAXPATHLEN - 1);

    if (lstat(szDirectory, &stDirInfo) < 0)
    {
        LOG_ERROR("Path '%s' is not accessible.\n", szDirectory);
        return NULL;
    }

    if (!S_ISDIR(stDirInfo.st_mode))
    {
        LOG_ERROR("'%s' is not directory.\n", szDirectory);
        return NULL;
    }

    if ((stDirIn = opendir(szDirectory)) == NULL)
    {
        LOG_ERROR("Can't open '%s' directory.\n", szDirectory);
        return NULL;
    }

    while ((stFiles = readdir(stDirIn)) != NULL)
    {
        sprintf(szFullName, "%s/%s", szDirectory, stFiles->d_name);

        if (lstat(szFullName, &stFileInfo) < 0)
        {
            LOG_ERROR("Path '%s' is not accessible.\n", szFullName);
            goto err_lstat_fullname;
        }

        if (S_ISLNK(stFileInfo.st_mode))
        {
            retval =
                sscanf(szFullName, "/sys/bus/pci/devices/0000:%x:%x.%x", &bus, &slot, &function);

            if (retval == 3)
            {
                // Attempt to locate the device we are interested in!
                sprintf(szDeviceName, "%s/device", szFullName);
                sprintf(szVendorName, "%s/vendor", szFullName);

                fpContents = fopen(szDeviceName, "r");
                if (fpContents)
                {
                    char* s = fgets(szContents, 255, fpContents);
                    sscanf(szContents, "0x%x", &device_id);
                    fclose(fpContents);
                }

                fpContents = fopen(szVendorName, "r");
                if (fpContents)
                {
                    char *s = fgets(szContents, 255, fpContents);
                    sscanf(szContents, "0x%x", &vendor_id);
                    fclose(fpContents);
                }

#if LOG_DEBUG_VERBOSE
				LOG_DEBUG("Deviceid = 0x%x, vendor_id = 0x%x, 0x%x, 0x%x, 0x%x\n",
						device_id, vendor_id, bus, slot, function);
#endif

				if ((vendor_id == ui16VendorID) && (device_id == ui16DeviceID))
				{
					LOG_DEBUG("card found at 0x%x, 0x%x, 0x%x\n",
							bus, slot, function);

                    sprintf(szBar1, "%s/resource%d", szFullName, ui8Bar);

                    if ((psMapping->fd = open(szBar1, O_RDWR | O_SYNC)) < 0)
                    {
                        LOG_ERROR("Unable to open %s, err = %d\n", szBar1, psMapping->fd);
                        goto err_file_open;
                    }

                    // The adjusted base address
                    ui32Local32Offset = ui32Offset & ~0xfff;
                    // Pad this to get enough!
                    psMapping->size = ui16Size + ui32Offset - ui32Local32Offset;

#if LOG_DEBUG_VERBOSE
					LOG_DEBUG("Mapping 0x%08lx->0x%08lx\n",
							ui32Offset, (ui32Offset + ui16Size) - 1);
#endif

                    psMapping->mapped = mmap(0, ui16Size, PROT_READ | PROT_WRITE,
                            MAP_SHARED, psMapping->fd, ui32Local32Offset);

                    if (psMapping->mapped == (void*)-1)
                    {
                        LOG_ERROR("Unable to map bar %d\n", ui8Bar);
                        goto err_mmap;
                    }
                    /* default pointer arith on void treated as on char */
                    mappedAddr = (char*)psMapping->mapped + (ui32Offset - ui32Local32Offset);
                    break;
                }
            }
        }
    }

    closedir(stDirIn);
    return mappedAddr;

err_mmap:
    close(psMapping->fd);
    psMapping->mapped = 0;
err_file_open:
err_lstat_fullname:
    closedir(stDirIn);
    return NULL;
#else
    psMapping->mapped=(void*)malloc(ui16Size);
    return psMapping->mapped;
#endif
}

void CloseUserMapping(user_device_mapping *psMapping)
{
#if defined (__linux__)
    LOG_DEBUG("Closing user mapping, psMapping = %p\n", psMapping);
    if (psMapping)
    {
        if (psMapping->mapped)
        {
            LOG_DEBUG("Unmapping ...\n");
            munmap(psMapping->mapped, psMapping->size);
        }
        close(psMapping->fd);
    }
#else
    free(psMapping->mapped);
#endif
}

void syncit(user_device_mapping *psMapping)
{
}
