
#include <tal_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TAL_TARGET_NAME "MMUDEV"
#define NO_IRQ TAL_NO_IRQ

#define TAL_MEM_SPACE_ARRAY_SIZE 2
#define TAL_DEV_OFF 0x00800000
#define TAL_DEV_DEF_MEM_SIZE 0x10000000
#define TAL_DEV_DEF_MEM_GUARD 0x00000000

static TAL_sMemSpaceInfo gasMemspace [TAL_MEM_SPACE_ARRAY_SIZE] =
{
	{
		"TOP",					/* Memory space name */
		TAL_MEMSPACE_MEMORY,	/* Memory space type: TAL_MEMSPACE_REGISTER or TAL_MEMSPACE_MEMORY */
		{
			0x00000000,			/* Base address of memory region */
			0x10000000,			/* Size of memory region - 256MB */
			0x00000000			/* Memory guard band */
		}
	},
	{
		"BOTTOM",				/* Memory space name */
		TAL_MEMSPACE_MEMORY,	/* Memory space type: TAL_MEMSPACE_REGISTER or TAL_MEMSPACE_MEMORY */
		{
			0x10000000,			/* Base address of memory region */
			0x20000000,			/* Size of memory region - 512MB */
			0x00000000			/* Memory guard band */
		}
	}
};

#ifdef TAL_NORMAL

static void setDeviceInfo_NULL(TAL_sDeviceInfo *pDevInfo)
{
	IMG_MEMSET(pDevInfo, 0, sizeof(TAL_sDeviceInfo));
	pDevInfo->pszDeviceName = TAL_TARGET_NAME;
	pDevInfo->ui64MemBaseAddr = TAL_DEV_OFF;
	pDevInfo->ui64DefMemSize = TAL_DEV_DEF_MEM_SIZE;
	pDevInfo->ui64MemGuardBand = TAL_DEV_DEF_MEM_GUARD;

	pDevInfo->sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.eDevifType = DEVIF_TYPE_NULL;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.ui32HostTargetCycleRatio = 20000;
}

/**
 * @brief Configure a device structure for TCP/IP
 */
static void setDeviceInfo_IP(TAL_sDeviceInfo *pDevInfo, IMG_UINT32 port, const char *pszIP)
{
	IMG_MEMSET(pDevInfo, 0, sizeof(TAL_sDeviceInfo));
	pDevInfo->pszDeviceName = TAL_TARGET_NAME;
	pDevInfo->ui64MemBaseAddr = TAL_DEV_OFF;
	pDevInfo->ui64DefMemSize = TAL_DEV_DEF_MEM_SIZE;
	pDevInfo->ui64MemGuardBand = TAL_DEV_DEF_MEM_GUARD;

	pDevInfo->sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.eDevifType = DEVIF_TYPE_SOCKET;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.sWrapCtrlInfo.ui32HostTargetCycleRatio = 20000;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.pszDeviceName = TAL_TARGET_NAME;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.ui32PortId = port;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.pszRemoteName = (char*)pszIP;
	pDevInfo->sDevIfDeviceCB.sDevIfSetup.u.sDeviceIpInfo.bUseBuffer = IMG_TRUE;
}

#endif

#ifdef __cplusplus
}
#endif
