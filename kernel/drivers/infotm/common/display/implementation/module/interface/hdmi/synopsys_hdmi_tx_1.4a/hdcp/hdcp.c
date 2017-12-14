/*
 * hdcp.c
 *
 *  Created on: Jul 21, 2010
 *      Author: klabadi & dlopo
 */
#include <linux/slab.h>
#include "hdcp.h"
#include "hdcpVerify.h"
#include "halHdcp.h"
//#include "stdlib.h"
#include "system.h"
#include "log.h"
#include "error.h"
/*
 * HDCP module registers offset
 */
static const u16 HDCP_BASE_ADDR = 0x5000;

int hdcp_Initialize(u16 baseAddr, int dataEnablePolarity)
{
	LOG_TRACE();
	halHdcp_RxDetected(baseAddr + HDCP_BASE_ADDR, 0);
	halHdcp_DataEnablePolarity(baseAddr + HDCP_BASE_ADDR, (dataEnablePolarity
			== TRUE) ? 1 : 0);
	halHdcp_DisableEncryption(baseAddr + HDCP_BASE_ADDR, 1);
	return TRUE;
}

int hdcp_Configure(u16 baseAddr, hdcpParams_t *params, int hdmi, int hsPol,
		int vsPol)
{
	LOG_TRACE();

	/* video related */
	halHdcp_DeviceMode(baseAddr + HDCP_BASE_ADDR, (hdmi == TRUE) ? 1 : 0);
	halHdcp_HSyncPolarity(baseAddr + HDCP_BASE_ADDR, (hsPol == TRUE) ? 1 : 0);
	halHdcp_VSyncPolarity(baseAddr + HDCP_BASE_ADDR, (vsPol == TRUE) ? 1 : 0);

	/* HDCP only */
	halHdcp_EnableFeature11(baseAddr + HDCP_BASE_ADDR,
			(hdcpParams_GetEnable11Feature(params) == TRUE) ? 1 : 0);
	halHdcp_RiCheck(baseAddr + HDCP_BASE_ADDR, (hdcpParams_GetRiCheck(params)
			== TRUE) ? 1 : 0);
	halHdcp_EnableI2cFastMode(baseAddr + HDCP_BASE_ADDR,
			(hdcpParams_GetI2cFastMode(params) == TRUE) ? 1 : 0);
	halHdcp_EnhancedLinkVerification(baseAddr + HDCP_BASE_ADDR,
			(hdcpParams_GetEnhancedLinkVerification(params) == TRUE) ? 1 : 0);

	/* fixed */
	halHdcp_EnableAvmute(baseAddr + HDCP_BASE_ADDR, 0);
	halHdcp_BypassEncryption(baseAddr + HDCP_BASE_ADDR, 0);
	halHdcp_DisableEncryption(baseAddr + HDCP_BASE_ADDR, 0);
	halHdcp_UnencryptedVideoColor(baseAddr + HDCP_BASE_ADDR, 0x00);
	halHdcp_OessWindowSize(baseAddr + HDCP_BASE_ADDR, 64);
	halHdcp_EncodingPacketHeader(baseAddr + HDCP_BASE_ADDR, 1);

	halHdcp_SwReset(baseAddr + HDCP_BASE_ADDR);
	/* enable KSV list SHA1 verification interrupt */
	halHdcp_InterruptClear(baseAddr + HDCP_BASE_ADDR, ~0);
	halHdcp_InterruptMask(baseAddr + HDCP_BASE_ADDR, (u8)(~(BIT(INT_KSV_SHA1))));
	return TRUE;
}

u8 hdcp_EventHandler(u16 baseAddr, int hpd, u8 state)
{
	size_t list = 0;
	size_t size = 0;
	size_t attempt = 0;
	size_t i = 0;
	int valid = FALSE;
	u8 *data = NULL;
	LOG_TRACE();
	LOG_DEBUG("HDCP Event Handler");
	if ((state & BIT(INT_KSV_SHA1)) != 0)
	{
		valid = FALSE;
		halHdcp_MemoryAccessRequest(baseAddr + HDCP_BASE_ADDR, 1);
		for (attempt = 20; attempt > 0; attempt--)
		{
			if (halHdcp_MemoryAccessGranted(baseAddr + HDCP_BASE_ADDR) == 1)
			{
				list = (halHdcp_KsvListRead(baseAddr + HDCP_BASE_ADDR, 0)
						& KSV_MSK) * KSV_LEN;
				size = list + HEADER + SHAMAX;
				data = (u8*) kmalloc(sizeof(u8) * size, GFP_KERNEL);
				if (data != 0)
				{
					for (i = 0; i < size; i++)
					{
						if (i < HEADER)
						{ /* BSTATUS & M0 */
							data[list + i] = halHdcp_KsvListRead(baseAddr
									+ HDCP_BASE_ADDR, i);
						}
						else if (i < (HEADER + list))
						{ /* KSV list */
							data[i - HEADER] = halHdcp_KsvListRead(baseAddr
									+ HDCP_BASE_ADDR, i);
						}
						else
						{ /* SHA */
							data[i] = halHdcp_KsvListRead(baseAddr
									+ HDCP_BASE_ADDR, i);
						}
					}
					valid = hdcpVerify_KSV(data, size);
/* #define HW_WA_HDCP_REP */
#ifdef HW_WA_HDCP_REP
					for (i = 0; !valid && i != (u8)(~0); i++)
					{
						data[14] = i; /* M0 - LSB */
						valid = hdcpVerify_KSV(data, size);
					}
#endif
				}
				else
				{
					error_Set(ERR_MEM_ALLOCATION);
					LOG_ERROR("cannot allocate memory");
					return FALSE;
				}
				break;
			}
		}
		halHdcp_MemoryAccessRequest(baseAddr + HDCP_BASE_ADDR, 0);

		halHdcp_UpdateKsvListState(baseAddr + HDCP_BASE_ADDR,
				(valid == TRUE) ? 0 : 1);
	}
	return TRUE;
}

void hdcp_RxDetected(u16 baseAddr, int detected)
{
	LOG_TRACE1(detected);
	LOG_DEBUG2("detected", detected);
	halHdcp_RxDetected(baseAddr + HDCP_BASE_ADDR, (detected == TRUE) ? 1 : 0);
}

void hdcp_AvMute(u16 baseAddr, int enable)
{
	LOG_TRACE1(enable);
	halHdcp_EnableAvmute(baseAddr + HDCP_BASE_ADDR, (enable == TRUE) ? 1 : 0);
}

/* William Smith add */
int hdcp_BypassEncryptionState(u16 baseAddr)
{
	return halHdcp_BypassEncryptionState(baseAddr + HDCP_BASE_ADDR);
}

void hdcp_BypassEncryption(u16 baseAddr, int bypass)
{
	LOG_TRACE1(bypass);
	halHdcp_BypassEncryption(baseAddr + HDCP_BASE_ADDR, (bypass == TRUE) ? 1
			: 0);
}

void hdcp_DisableEncryption(u16 baseAddr, int disable)
{
	LOG_TRACE1(disable);
	halHdcp_DisableEncryption(baseAddr + HDCP_BASE_ADDR, (disable == TRUE) ? 1
			: 0);
}

int hdcp_Engaged(u16 baseAddr)
{
	LOG_TRACE();
	return (halHdcp_HdcpEngaged(baseAddr + HDCP_BASE_ADDR) != 0);
}

u8 hdcp_AuthenticationState(u16 baseAddr)
{
	/* hardware recovers automatically from a lost authentication */
	LOG_TRACE();
	return halHdcp_AuthenticationState(baseAddr + HDCP_BASE_ADDR);
}

u8 hdcp_CipherState(u16 baseAddr)
{
	LOG_TRACE();
	return halHdcp_CipherState(baseAddr + HDCP_BASE_ADDR);
}

u8 hdcp_RevocationState(u16 baseAddr)
{
	LOG_TRACE();
	return halHdcp_RevocationState(baseAddr + HDCP_BASE_ADDR);
}

int hdcp_SrmUpdate(u16 baseAddr, const u8 * data)
{
	size_t size = 0;
	size_t i = 0;
	size_t j = 0;
	size_t attempt = 10;
	int success = FALSE;
	LOG_TRACE();

	for (i = 0; i < VRL_NUMBER; i++)
	{
		size <<= i * 8;
		size |= data[VRL_LENGTH + i];
	}
	size += VRL_HEADER;

	if (hdcpVerify_SRM(data, size) == TRUE)
	{
		halHdcp_MemoryAccessRequest(baseAddr + HDCP_BASE_ADDR, 1);
		for (attempt = 20; attempt > 0; attempt--)
		{
			if (halHdcp_MemoryAccessGranted(baseAddr + HDCP_BASE_ADDR) == 1)
			{
				/* TODO fix only first-generation is handled */
				size -= (VRL_HEADER + VRL_NUMBER + 2 * DSAMAX);
				/* write number of KSVs */
				halHdcp_RevocListWrite(baseAddr + HDCP_BASE_ADDR, 0,
						(size == 0)? 0 : data[VRL_LENGTH + VRL_NUMBER]);
				halHdcp_RevocListWrite(baseAddr + HDCP_BASE_ADDR, 1, 0);
				/* write KSVs */
				for (i = 1; i < size; i += KSV_LEN)
				{
					for (j = 1; j <= KSV_LEN; j++)
					{
						halHdcp_RevocListWrite(baseAddr + HDCP_BASE_ADDR,
								i + j, data[VRL_LENGTH + VRL_NUMBER + i
										+ (KSV_LEN - j)]);
					}
				}
				success = TRUE;
				LOG_NOTICE("SRM successfully updated");
				break;
			}
		}
		halHdcp_MemoryAccessRequest(baseAddr + HDCP_BASE_ADDR, 0);
		if (!success)
		{
			error_Set(ERR_HDCP_MEM_ACCESS);
			LOG_ERROR("cannot access memory");
		}
	}
	else
	{
		error_Set(ERR_SRM_INVALID);
		LOG_ERROR("SRM invalid");
	}
	return success;
}

u8 hdcp_InterruptStatus(u16 baseAddr)
{
		return halHdcp_InterruptStatus(baseAddr + HDCP_BASE_ADDR);
}

int hdcp_InterruptClear(u16 baseAddr, u8 value)
{
	halHdcp_InterruptClear(baseAddr + HDCP_BASE_ADDR, value);
	return TRUE;
}
