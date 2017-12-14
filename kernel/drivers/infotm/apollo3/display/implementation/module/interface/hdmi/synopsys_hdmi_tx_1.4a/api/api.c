/*
 * @file:api.c
 *
 *  Synopsys Inc.
 *  SG DWC PT02
 */
#include <linux/kernel.h>
#include "api.h"
#include "log.h"
#include "access.h"
#include "system.h"
#include "board.h"
#include "mutex.h"
#include "control.h"
#include "video.h"
#include "audio.h"
#include "packets.h"
#include "hdcp.h"
#include "edid.h"
#include "phy.h"
#include "error.h"

extern edid_status_t edid_mStatus;

u16 api_mBaseAddress = 0;
state_t api_mCurrentState = API_NOT_INIT;
static u8 api_mHpd = FALSE;
static void* api_mMutex = NULL;
static handler_t api_mEventRegistry[DUMMY] = {NULL};
static u8 api_mDataEnablePolarity = FALSE;
/* Flag to check if sending the Gamut Metadata Packet is not legal by the HDMI protocol */
static u8 api_mSendGamutOk = FALSE;
static u16 api_mSfrClock = 0;

static int api_CheckParamsVideo(videoParams_t * video);
static int api_CheckParamsAudio(audioParams_t * audio);

int api_GetState(void)
{
	return api_mCurrentState;
}

int api_Initialize(u16 address, u8 dataEnablePolarity, u16 sfrClock, u8 force)
{
	dtd_t dtd;
	videoParams_t params;
	u16 pixelClock = 0;
	api_mDataEnablePolarity = dataEnablePolarity;
	api_mHpd = FALSE;
	api_mSendGamutOk = FALSE;
	api_mSfrClock = sfrClock;

	LOG_NOTICE("dwc_hdmi_tx_software_api_2.00");
	LOG_NOTICE(__DATE__);
	LOG_TRACE1(dataEnablePolarity);
	
	api_mBaseAddress = address;
	control_InterruptMute(api_mBaseAddress, ~0); /* disable interrupts */
	if ((api_mCurrentState < API_HPD) || (api_mCurrentState > API_EDID_READ))
	{
		/* if EDID read do not re-read, if HPDetected, keep in same state
		 * otherwise, NOT INIT */
		api_mCurrentState = API_NOT_INIT;
	}
	
	if (api_mMutex != NULL)
	{
		mutex_Initialize(api_mMutex);
	}
	/* VGA must be supported by all sinks
	 * so use it as default configuration
	 */
	dtd_Fill(&dtd, 1, 60000); /* VGA */
	videoParams_Reset(&params);
	videoParams_SetHdmi(&params, FALSE); /* DVI */
	videoParams_SetDtd(&params, &dtd);
	pixelClock = videoParams_GetPixelClock(&params);

	if (board_Initialize(api_mBaseAddress, pixelClock, 8) != TRUE)
	{
		return FALSE;
	}
	LOG_NOTICE2("Product Line", control_ProductLine(api_mBaseAddress));
	LOG_NOTICE2("Product Type", control_ProductType(api_mBaseAddress));
	if (control_SupportsCore(api_mBaseAddress) != TRUE)
	{
		error_Set(ERR_HW_NOT_SUPPORTED);
		LOG_ERROR("Unknown device: aborting...");
		return FALSE;
	}
	LOG_NOTICE2("HDMI TX Controller Design", control_Design(api_mBaseAddress));
	LOG_NOTICE2("HDMI TX Controller Revision", control_Revision(api_mBaseAddress));
	if (control_SupportsHdcp(api_mBaseAddress) == TRUE)
	{
		LOG_NOTICE("HDMI TX controller supports HDCP");
	}
	if (video_Initialize(api_mBaseAddress, &params, api_mDataEnablePolarity)
			!= TRUE)
	{
		return FALSE;
	}
	if (audio_Initialize(api_mBaseAddress) != TRUE)
	{
		return FALSE;
	}
	if (packets_Initialize(api_mBaseAddress) != TRUE)
	{
		return FALSE;
	}
	if (hdcp_Initialize(api_mBaseAddress, api_mDataEnablePolarity)
			!= TRUE)
	{
		return FALSE;
	}
	if (control_Initialize(api_mBaseAddress, api_mDataEnablePolarity,
			pixelClock) != TRUE)
	{
		return FALSE;
	}
	if (phy_Initialize(api_mBaseAddress, api_mDataEnablePolarity) != TRUE)
	{
		return FALSE;
	}
	control_InterruptClearAll(api_mBaseAddress);
#if 0	
	if (board_PixelClock(api_mBaseAddress, pixelClock, videoParams_GetColorResolution(&params))!= TRUE)
	{
		return FALSE;
	}
#endif

	/* William */
#if 0
	if (video_Configure(api_mBaseAddress, &params, api_mDataEnablePolarity, FALSE) != TRUE)
	{
		return FALSE;
	}
	/*
	 * By default no pixel repetition and color resolution is 8
	 */
	if (control_Configure(api_mBaseAddress, pixelClock, 0, 8, 0, 0, 0, 0) != TRUE)
	{
		return FALSE;
	}

	/* send AVMUTE SET (optional) */
	if (phy_Configure (api_mBaseAddress, pixelClock, 8, 0) != TRUE)
	{
		return FALSE;
	}
#endif
	/* William */
	if ((api_mCurrentState < API_HPD) || (api_mCurrentState > API_EDID_READ))
	{
		/* if EDID read do not re-read, if HPDetected, keep in same state
		 * otherwise, set to INIT */
		api_mCurrentState = API_INIT;
	}

	if (force)
	{
		LOG_WARNING("init force");
		api_mCurrentState = API_HPD;
	}
	phy_EnableHpdSense(api_mBaseAddress); /* enable HPD sending on phy */
	phy_InterruptEnable(api_mBaseAddress, ~0x02);
	LOG_NOTICE("Waiting for hot plug detection...");

	/* enable interrupts */
	control_InterruptMute(api_mBaseAddress, 0); 
	if (system_InterruptHandlerRegister(TX_INT, api_EventHandler, NULL) == TRUE)
	{
		return system_InterruptEnable(TX_INT);
	}
	return FALSE;
}

int api_Configure(videoParams_t * video, audioParams_t * audio,
		productParams_t * product, hdcpParams_t * hdcp)
{
	hdmivsdb_t vsdb;
	//int audioOn = (audio != 0 && videoParams_GetHdmi(video));
	int audioOn = 1;
	int hdcpOn = (hdcp != 0 && (control_SupportsHdcp(api_mBaseAddress)
					== TRUE)) ? TRUE : FALSE;
	LOG_TRACE();
	LOG_NOTICE2("api_mCurrentState", api_mCurrentState);

	if (api_mCurrentState < API_HPD)
	{
		error_Set(ERR_HPD_LOST);
		LOG_ERROR("cable not connected");
		return FALSE;
	}
	else if (api_mCurrentState == API_HPD)
	{
		LOG_WARNING("E-EDID not read. Media may not be supported by sink");
	}
	else if (api_mCurrentState == API_EDID_READ)
	{
		api_CheckParamsVideo(video);
		if (api_EdidHdmivsdb(&vsdb))
		{
			if (!hdmivsdb_GetDeepColor30(&vsdb) && !hdmivsdb_GetDeepColor36(&vsdb)
					&& !hdmivsdb_GetDeepColor48(&vsdb))
			{
				videoParams_SetColorResolution(video, 0);
			}
		}
	}
	control_InterruptMute(api_mBaseAddress, ~0); /* disable interrupts */
	system_InterruptDisable(TX_INT);
	if (video_Configure(api_mBaseAddress, video, api_mDataEnablePolarity,
			hdcpOn) != TRUE)
	{
		return FALSE;
	}
	if (audioOn)
	{
		if (api_mCurrentState == API_EDID_READ)
		{
			api_CheckParamsAudio(audio);
		}
#if 0
		if (board_AudioClock(api_mBaseAddress, audioParams_AudioClock(audio))
					!= TRUE)
		{
			return FALSE;
		}
#endif
		if (audio_Configure(api_mBaseAddress, audio, videoParams_GetPixelClock(
				video), videoParams_GetRatioClock(video)) != TRUE)
		{
			return FALSE;
		}
		packets_AudioInfoFrame(api_mBaseAddress, audio);
	}
	else if (audio != 0 && videoParams_GetHdmi(video) != TRUE)
	{
		LOG_WARNING("DVI mode selected: audio not configured");
	}
	else
	{
		LOG_WARNING("No audio parameters provided: not configured");
	}
	if (videoParams_GetHdmi(video) == TRUE)
	{
		if (packets_Configure(api_mBaseAddress, video, product) != TRUE)
		{
			return FALSE;
		}
		api_mSendGamutOk = (videoParams_GetEncodingOut(video) == YCC444)
				|| (videoParams_GetEncodingOut(video) == YCC422);
		api_mSendGamutOk = api_mSendGamutOk && (videoParams_GetColorimetry(
				video) == EXTENDED_COLORIMETRY);
	}
	else
	{
		LOG_WARNING("DVI mode selected: packets not configured");
	}

	if (hdcpOn == TRUE)
	{
		/* HDCP is PHY independent */
		if (hdcp_Configure(api_mBaseAddress, hdcp, videoParams_GetHdmi(video),
				dtd_GetHSyncPolarity(videoParams_GetDtd(video)),
				dtd_GetVSyncPolarity(videoParams_GetDtd(video))) == FALSE)
		{
			LOG_WARNING("HDCP not configured");
			hdcpOn = FALSE;
		}
	}
	else if (hdcp != 0 && control_SupportsHdcp(api_mBaseAddress) != TRUE)
	{
		LOG_WARNING("HDCP is not supported by hardware");
	}
	else
	{
		LOG_WARNING("No HDCP parameters provided: not configured");
	}
#if 0
	if (board_PixelClock(api_mBaseAddress, videoParams_GetPixelClock(video),
			videoParams_GetColorResolution(video)) != TRUE)
	{
		return FALSE;
	}
#endif
	if (control_Configure(api_mBaseAddress, videoParams_GetPixelClock(video),
			videoParams_GetPixelRepetitionFactor(video),
			videoParams_GetColorResolution(video),
			videoParams_IsColorSpaceConversion(video), audioOn, FALSE, hdcpOn)
			!= TRUE)
	{
		return FALSE;
	}
	if (phy_Configure(api_mBaseAddress, videoParams_GetPixelClock(video),
		videoParams_GetColorResolution(video),
		videoParams_GetPixelRepetitionFactor(video)) != TRUE)
	{
		return FALSE;
	}
	/* disable blue screen transmission after turning on all necessary blocks (e.g. HDCP) */
	if (video_ForceOutput(api_mBaseAddress, FALSE) != TRUE)
	{
		return FALSE;
	}
	/* reports HPD state to HDCP */
	if (hdcpOn)
	{
		hdcp_RxDetected(api_mBaseAddress, phy_HotPlugDetected(
				api_mBaseAddress));
	}
	/* send AVMUTE CLEAR (optional) */
	api_mCurrentState = API_CONFIGURED;
	LOG_NOTICE("API_CONFIGURED");
	control_InterruptMute(api_mBaseAddress, 0); /* enable interrupts */
	system_InterruptEnable(TX_INT);
	return TRUE;
}

int api_Standby()
{
	LOG_TRACE();
	system_InterruptDisable(TX_INT);
	system_InterruptHandlerUnregister(TX_INT);
	control_Standby(api_mBaseAddress);
	phy_Standby(api_mBaseAddress);
	return TRUE;
}

u8 api_CoreRead(u16 addr)
{
	return access_CoreReadByte(addr);
}

void api_CoreWrite(u8 data, u16 addr)
{
	access_CoreWriteByte(data, addr);
}

void api_EventHandler(void * param)
{
	//u8 i = 0;
	u8 state = 0;
	u8 handled = FALSE;
	u8 oldHpd = (api_mCurrentState >= API_HPD);
	LOG_TRACE();

	api_mHpd = (phy_HotPlugDetected(api_mBaseAddress) > 0);
	*(int *)param = api_mHpd;
#if 0
for (state = 0; state < 10; state++)
{
	LOG_NOTICE3("core read", 0x100 + state, access_CoreReadByte(api_mBaseAddress + 0x100 + state));
}								
#endif
	/* if HPD always enter - all states */
	state = control_InterruptPhyState(api_mBaseAddress);
	if ((state != 0) || (api_mHpd != oldHpd))
	{
		if (api_mHpd && !oldHpd)				
		{
			api_mCurrentState = API_HPD;
			LOG_NOTICE("Jitter: hot plug detected ++++++");
		}
		else if (!api_mHpd && oldHpd)
		{
			//api_mCurrentState = API_NOT_INIT;
			api_mCurrentState = API_INIT;			// William
			LOG_NOTICE("Jitter: hot plug lost    ------");
		}
		control_InterruptPhyClear(api_mBaseAddress, state);
		handled = TRUE;
#if 0
		/* report HPD state to HDCP (after configuration) */
		if (api_mCurrentState == API_CONFIGURED)
		{
			LOG_DEBUG("Report HPD state to HDCP (after configuration)");
			hdcp_RxDetected(api_mBaseAddress, api_mHpd);
		}
#endif
	}
	if (api_mCurrentState == API_CONFIGURED)
	{
		/* only report if configured 
		 * (for modules enabled only at configuration stage)*/
		/* HDCP module */
		state = hdcp_InterruptStatus(api_mBaseAddress);
		if ((state != 0) || (api_mHpd != oldHpd))
		{
			LOG_NOTICE("HDCP KSV List SHA verification(For repeater only).");
			hdcp_InterruptClear(api_mBaseAddress, state);
			handled = hdcp_EventHandler(api_mBaseAddress, api_mHpd, state);
		}
	}	
		/* EDID module */
		state = control_InterruptEdidState(api_mBaseAddress);
	if (api_mCurrentState >= API_HPD || edid_mStatus == EDID_READING)
	{
		/* only report if state is HPD, EDID_READ or CONFIGURED 
		   for modules that can be enabled anytime there is HPD */
		/* EDID module */
	//	state = control_InterruptEdidState(api_mBaseAddress);
		//if ((state != 0) || (api_mHpd != oldHpd))
		if (state != 0)		// William
		{
			control_InterruptEdidClear(api_mBaseAddress, state);
			if (edid_EventHandler(api_mBaseAddress, api_mHpd, state) == EDID_DONE)
			{
				edid_Standby(api_mBaseAddress);
				api_mCurrentState = API_EDID_READ;
				if (api_mEventRegistry[EDID_READ_EVENT] != NULL)
				{	
					api_mEventRegistry[EDID_READ_EVENT](&api_mHpd);
				}
			}
			/* even if it is an error reading, it is has been handled!*/
			handled = TRUE;
		}
	} else if (state) {
		control_InterruptEdidClear(api_mBaseAddress, state);
	}

#if 0
	for (i = HPD_EVENT; i < DUMMY; i++)
	{
		/* mutex_Lock(api_mMutex); */
		/* call registered user callbacks */
		if (api_mEventRegistry[i] != NULL)
		{
			if ((i == HPD_EVENT) && (api_mHpd == oldHpd))
			{
				/* if no difference in HPD, dont call callback */
				continue;
			}
			api_mEventRegistry[i](&api_mHpd);
		}
		/* mutex_Unlock(api_mMutex); */
	}
#endif
	if (!handled)
	{
		LOG_WARNING("interrupt not handled");
		printk(KERN_ERR "edid state: %x\n", control_InterruptEdidState(api_mBaseAddress));
		printk(KERN_ERR "phy  state: %x\n", control_InterruptPhyState(api_mBaseAddress));
		printk(KERN_ERR "hdcp state: %x\n", hdcp_InterruptStatus(api_mBaseAddress));
		printk(KERN_ERR "flag state: %d, %d, %d, %d\n", api_mHpd, oldHpd,
				api_mCurrentState, edid_mStatus);

	}
	system_InterruptAcknowledge(TX_INT);
}

int api_EventEnable(event_t idEvent, handler_t handler, u8 oneshot)
{
	u8 retval;
	LOG_TRACE();
	retval = idEvent < DUMMY;
	if (retval)
	{
		//mutex_Lock(api_mMutex);
		api_mEventRegistry[idEvent] = handler;
		//mutex_Unlock(api_mMutex);
		/* enable hardware */
	}
	return retval;
}

int api_EventClear(event_t idEvent)
{
	u8 retval;
	LOG_TRACE();
	retval = idEvent < DUMMY;
	if (retval)
	{
		/* clear hardware */
	}
	return retval;
}

int api_EventDisable(event_t idEvent)
{
	u8 retval;
	LOG_TRACE();
	retval = idEvent < DUMMY;
	if (retval)
	{
		/* disable hardware */
		//mutex_Lock(api_mMutex);
		api_mEventRegistry[idEvent] = NULL;
		//mutex_Unlock(api_mMutex);
	}
	return retval;
}
int api_EdidRead(void)
{
	if (api_mCurrentState >= API_HPD)
	{
		//api_mEventRegistry[EDID_READ_EVENT] = handler;
		if (edid_Initialize(api_mBaseAddress, api_mSfrClock) == TRUE)
		{
			return TRUE;
		}
	}
	LOG_ERROR("cannot read EDID");
	return FALSE;
}

int api_EdidDtdCount()
{
	LOG_TRACE();
	return edid_GetDtdCount(api_mBaseAddress);
}

int api_EdidDtd(unsigned int n, dtd_t * dtd)
{
	LOG_TRACE();
	return edid_GetDtd(api_mBaseAddress, n, dtd);
}

int api_EdidHdmivsdb(hdmivsdb_t * vsdb)
{
	LOG_TRACE();
	return edid_GetHdmivsdb(api_mBaseAddress, vsdb);
}

int api_EdidMonitorName(char * name, unsigned int length)
{
	LOG_TRACE();
	return edid_GetMonitorName(api_mBaseAddress, name, length);
}

int api_EdidMonitorRangeLimits(monitorRangeLimits_t * limits)
{
	LOG_TRACE();
	return edid_GetMonitorRangeLimits(api_mBaseAddress, limits);
}

int api_EdidSvdCount()
{
	LOG_TRACE();
	return edid_GetSvdCount(api_mBaseAddress);
}

int api_EdidSvd(unsigned int n, shortVideoDesc_t * svd)
{
	LOG_TRACE();
	return edid_GetSvd(api_mBaseAddress, n, svd);
}

int api_EdidSadCount()
{
	LOG_TRACE();
	return edid_GetSadCount(api_mBaseAddress);
}

int api_EdidSad(unsigned int n, shortAudioDesc_t * sad)
{
	LOG_TRACE();
	return edid_GetSad(api_mBaseAddress, n, sad);
}

int api_EdidVideoCapabilityDataBlock(videoCapabilityDataBlock_t * capability)
{
	LOG_TRACE();
	return edid_GetVideoCapabilityDataBlock(api_mBaseAddress, capability);
}

int api_EdidSpeakerAllocationDataBlock(
		speakerAllocationDataBlock_t * allocation)
{
	LOG_TRACE();
	return edid_GetSpeakerAllocationDataBlock(api_mBaseAddress, allocation);
}

int api_EdidColorimetryDataBlock(colorimetryDataBlock_t * colorimetry)
{
	LOG_TRACE();
	return edid_GetColorimetryDataBlock(api_mBaseAddress, colorimetry);
}

int api_EdidSupportsBasicAudio()
{
	LOG_TRACE();
	return edid_SupportsBasicAudio(api_mBaseAddress);
}

int api_EdidSupportsUnderscan()
{
	LOG_TRACE();
	return edid_SupportsUnderscan(api_mBaseAddress);
}

int api_EdidSupportsYcc422()
{
	LOG_TRACE();
	return edid_SupportsYcc422(api_mBaseAddress);
}

int api_EdidSupportsYcc444()
{
	LOG_TRACE();
	return edid_SupportsYcc444(api_mBaseAddress);
}

void api_PacketsAudioContentProtection(u8 type, u8 * fields, u8 length,
		u8 autoSend)
{
	hdmivsdb_t vsdb;

	LOG_TRACE();
	/* check if sink supports ACP packets */
	if (api_EdidHdmivsdb(&vsdb))
	{
		if (!hdmivsdb_GetSupportsAi(&vsdb))
		{
			LOG_ERROR("sink does NOT support ACP");
		}
	}
	else
	{
		LOG_WARNING("sink is NOT HDMI compliant: ACP may not work");
	}
	packets_AudioContentProtection(api_mBaseAddress, type, fields, length,
			autoSend);
}

void api_PacketsIsrc(u8 initStatus, u8 * codes, u8 length, u8 autoSend)
{
	hdmivsdb_t vsdb;

	LOG_TRACE();
	/* check if sink supports ISRC packets */
	if (api_EdidHdmivsdb(&vsdb))
	{
		if (!hdmivsdb_GetSupportsAi(&vsdb))
		{
			LOG_ERROR("sink does NOT support ISRC");
		}
	}
	else
	{
		LOG_WARNING("sink is NOT HDMI compliant: ISRC may not work");
	}
	packets_IsrcPackets(api_mBaseAddress, initStatus, codes, length, autoSend);
}

void api_PacketsIsrcStatus(u8 status)
{
	LOG_TRACE();
	packets_IsrcStatus(api_mBaseAddress, status);
}

void api_PacketsGamutMetadata(u8 * gbdContent, u8 length)
{
	LOG_TRACE();
	if (!api_mSendGamutOk)
	{
		LOG_WARNING("Gamut packets will be discarded by sink: Video Out must be YCbCr with Extended Colorimetry");
	}
	packets_GamutMetadataPackets(api_mBaseAddress, gbdContent, length);
}

void api_PacketsStopSendAcp()
{
	LOG_TRACE();
	packets_StopSendAcp(api_mBaseAddress);
}

void api_PacketsStopSendIsrc()
{
	LOG_TRACE();
	packets_StopSendIsrc1(api_mBaseAddress);
}

void api_PacketsStopSendSpd()
{
	LOG_TRACE();
	packets_StopSendSpd(api_mBaseAddress);
}

void api_PacketsStopSendVsd()
{
	LOG_TRACE();
	packets_StopSendVsd(api_mBaseAddress);
}

void api_AvMute(int enable)
{
	LOG_TRACE1(enable);
	if (enable == TRUE)
	{
		packets_AvMute(api_mBaseAddress, enable);
		hdcp_AvMute(api_mBaseAddress, enable);
	}
	else
	{
		hdcp_AvMute(api_mBaseAddress, enable);
		packets_AvMute(api_mBaseAddress, enable);
	}
}

void api_AudioMute(int enable)
{
	LOG_TRACE1(enable);
	audio_Mute(api_mBaseAddress, enable);
}

int api_HdcpEngaged()
{
	LOG_TRACE();
	return hdcp_Engaged(api_mBaseAddress);
}

u8 api_HdcpAuthenticationState()
{
	LOG_TRACE();
	return hdcp_AuthenticationState(api_mBaseAddress);
}

u8 api_HdcpCipherState()
{
	LOG_TRACE();
	return hdcp_CipherState(api_mBaseAddress);
}

u8 api_HdcpRevocationState()
{
	LOG_TRACE();
	return hdcp_RevocationState(api_mBaseAddress);
}

/* William Smith add */
int api_HdcpBypassEncryptionState(void)
{
	LOG_TRACE();
	return hdcp_BypassEncryptionState(api_mBaseAddress);
}

int api_HdcpBypassEncryption(int bypass)
{
	LOG_TRACE1(bypass);
	hdcp_BypassEncryption(api_mBaseAddress, bypass);
	return TRUE;
}

int api_HdcpDisableEncryption(int disable)
{
	LOG_TRACE1(disable);
	hdcp_DisableEncryption(api_mBaseAddress, disable);
	return TRUE;
}

int api_HdcpSrmUpdate(const u8 * data)
{
	LOG_TRACE();
	return hdcp_SrmUpdate(api_mBaseAddress, data);
}

int api_HotPlugDetected()
{
	return phy_HotPlugDetected(api_mBaseAddress);
}

u8 api_ReadConfig()
{
	u16 addr = 0;
	LOG_TRACE();
	log_SetVerboseWrite(1);
	for (addr = 0; addr < 0x8000; addr++)
	{
		if ((addr == 0x01FF || addr <= 0x0003) || /* ID */
		(addr >= 0x0100 && addr <= 0x0107) || /* IH */
		(addr >= 0x0200 && addr <= 0x0207) || /* TX */
		(addr >= 0x0800 && addr <= 0x0808) || /* VP */
		(addr >= 0x1000 && addr <= 0x10E0) || /* FC */
		(addr >= 0x1100 && addr <= 0x1120) || /* FC */
		(addr >= 0x1200 && addr <= 0x121B) || /* FC */
		(addr >= 0x3000 && addr <= 0x3007) || /* PHY */
		(addr >= 0x3100 && addr <= 0x3102) || /* AUD */
		(addr >= 0x3200 && addr <= 0x3206) || /* AUD */
		(addr >= 0x3300 && addr <= 0x3302) || /* AUD */
		(addr >= 0x3400 && addr <= 0x3404) || /* AUD */
		(addr >= 0x4000 && addr <= 0x4006) || /* MC */
		(addr >= 0x4100 && addr <= 0x4119) || /* CSC */
		(addr >= 0x5000 && addr <= 0x501A) || /* A */
		(addr >= 0x7000 && addr <= 0x7026) || /* VG */
		(addr >= 0x7100 && addr <= 0x7116) || /* AG */
		(addr >= 0x7D00 && addr <= 0x7D31) || /* CEC */
		(addr >= 0x7E00 && addr <= 0x7E0A)) /* EDID */
		{
			/* SRM and I2C slave not required */
			LOG_WRITE(addr, access_CoreReadByte(addr));
		}
	}
	log_SetVerboseWrite(0);
	return TRUE;
}

/**
 * Check audio parameters against read EDID structure to ensure
 * compatibility with sink.
 * @param audio audio parameters data structure
 */
static int api_CheckParamsAudio(audioParams_t * audio)
{
	unsigned i = 0;
	int bitSupport = -1;
	shortAudioDesc_t sad;
	speakerAllocationDataBlock_t allocation;
	u8 errorCode = TRUE;
	int valid = FALSE;
	/* array to translate from AudioInfoFrame code to EDID Speaker Allocation data block bits */
	const u8 sadb[] =
	{ 1, 3, 5, 7, 17, 19, 21, 23, 9, 11, 13, 15, 25, 27, 29, 31, 73, 75, 77,
			79, 33, 35, 37, 39, 49, 51, 53, 55, 41, 43, 45, 47 };

	LOG_TRACE();
	if (!api_EdidSupportsBasicAudio())
	{
		error_Set(ERR_SINK_DOES_NOT_SUPPORT_AUDIO);
		LOG_WARNING("Sink does NOT support audio");
		return FALSE;
	}
	/* check if audio type supported */
	for (i = 0; i < api_EdidSadCount(); i++)
	{
		api_EdidSad(i, &sad);
		if (audioParams_GetCodingType(audio) == shortAudioDesc_GetFormatCode(
				&sad))
		{
			bitSupport = i;
			break;
		}
	}
	if (bitSupport >= 0)
	{
		api_EdidSad(bitSupport, &sad);
		/* 192 kHz| 176.4 kHz| 96 kHz| 88.2 kHz| 48 kHz| 44.1 kHz| 32 kHz */
		switch (audioParams_GetSamplingFrequency(audio))
		{
		case 32000:
			valid = shortAudioDesc_Support32k(&sad);
			break;
		case 44100:
			valid = shortAudioDesc_Support44k1(&sad);
			break;
		case 48000:
			valid = shortAudioDesc_Support48k(&sad);
			break;
		case 88200:
			valid = shortAudioDesc_Support88k2(&sad);
			break;
		case 96000:
			valid = shortAudioDesc_Support96k(&sad);
			break;
		case 176400:
			valid = shortAudioDesc_Support176k4(&sad);
			break;
		case 192000:
			valid = shortAudioDesc_Support192k(&sad);
			break;
		default:
			valid = FALSE;
			break;
		}
		if (!valid)
		{
			error_Set(ERR_SINK_DOES_NOT_SUPPORT_AUDIO_SAMPLING_FREQ);
			LOG_WARNING2("Sink does NOT support audio sampling frequency", audioParams_GetSamplingFrequency(audio));
			errorCode = FALSE;
		}
		if (audioParams_GetCodingType(audio) == PCM)
		{
			/* 24 bit| 20 bit| 16 bit */
			switch (audioParams_GetSampleSize(audio))
			{
			case 16:
				valid = shortAudioDesc_Support16bit(&sad);
				break;
			case 20:
				valid = shortAudioDesc_Support20bit(&sad);
				break;
			case 24:
				valid = shortAudioDesc_Support24bit(&sad);
				break;
			default:
				valid = FALSE;
				break;
			}
			if (!valid)
			{
				error_Set(ERR_SINK_DOES_NOT_SUPPORT_AUDIO_SAMPLE_SIZE);
				LOG_WARNING2("Sink does NOT support audio sample size", audioParams_GetSampleSize(audio));
				errorCode = FALSE;
			}
		}
		/* check Speaker Allocation */
		if (api_EdidSpeakerAllocationDataBlock(&allocation) == TRUE)
		{
			LOG_DEBUG2("Audio channel allocation sent", sadb[audioParams_GetChannelAllocation(audio)]);
			LOG_DEBUG2("Audio channel allocation accepted", speakerAllocationDataBlock_GetSpeakerAllocationByte(&allocation));
			valid = (sadb[audioParams_GetChannelAllocation(audio)]
					& speakerAllocationDataBlock_GetSpeakerAllocationByte(
							&allocation))
					== sadb[audioParams_GetChannelAllocation(audio)];
			if (!valid)
			{
				error_Set(ERR_SINK_DOES_NOT_SUPPORT_ATTRIBUTED_CHANNELS);
				LOG_WARNING("Sink does NOT have attributed speakers");
				errorCode = FALSE;
			}
		}
	}
	else
	{
		error_Set(ERR_SINK_DOES_NOT_SUPPORT_AUDIO_TYPE);
		LOG_WARNING("Sink does NOT support audio type");
		errorCode = FALSE;
	}
	return errorCode;
}

/**
 * Check Video parameters against allowed values and read EDID structure
 * to ensure compatibility with sink.
 * @param video video parameters
 */
static int api_CheckParamsVideo(videoParams_t * video)
{
	u8 errorCode = TRUE;
	hdmivsdb_t vsdb;
	dtd_t dtd;
	unsigned i = 0;
	shortVideoDesc_t svd;
	videoCapabilityDataBlock_t capability;
	int valid = FALSE;
	colorimetryDataBlock_t colorimetry;

	LOG_TRACE();
	colorimetryDataBlock_Reset(&colorimetry);
	shortVideoDesc_Reset(&svd);
	dtd_Fill(&dtd, 1, 60000);
	videoCapabilityDataBlock_Reset(&capability);

	if (videoParams_GetHdmi(video))
	{ /* HDMI mode */
		if (api_EdidHdmivsdb(&vsdb) == TRUE)
		{
			if (videoParams_GetColorResolution(video) == 10
					&& !hdmivsdb_GetDeepColor30(&vsdb))
			{
				error_Set(ERR_SINK_DOES_NOT_SUPPORT_30BIT_DC);
				LOG_WARNING("Sink does NOT support 30bits/pixel deep colour mode");
				errorCode = FALSE;
			}
			else if (videoParams_GetColorResolution(video) == 12
					&& !hdmivsdb_GetDeepColor36(&vsdb))
			{
				error_Set(ERR_SINK_DOES_NOT_SUPPORT_36BIT_DC);
				LOG_WARNING("Sink does NOT support 36bits/pixel deep colour mode");
				errorCode = FALSE;
			}
			else if (videoParams_GetColorResolution(video) == 16
					&& !hdmivsdb_GetDeepColor48(&vsdb))
			{
				error_Set(ERR_SINK_DOES_NOT_SUPPORT_48BIT_DC);
				LOG_WARNING("Sink does NOT support 48bits/pixel deep colour mode");
				errorCode = FALSE;
			}
			if (videoParams_GetEncodingOut(video) == YCC444
					&& !hdmivsdb_GetDeepColorY444(&vsdb))
			{
				error_Set(ERR_SINK_DOES_NOT_SUPPORT_YCC444_DC);
				LOG_WARNING("Sink does NOT support YCC444 in deep colour mode");
				errorCode = FALSE;
			}
			if (videoParams_GetTmdsClock(video) > 16500)
			{
				/*
				 * Sink must specify MaxTMDSClk when supports frequencies over 165MHz
				 * GetMaxTMDSClk() is TMDS clock / 5MHz and GetTmdsClock() returns [0.01MHz]
				 * so GetMaxTMDSClk() value must be multiplied by 500 in order to be compared
				 */
				if (videoParams_GetTmdsClock(video) > (hdmivsdb_GetMaxTmdsClk(
						&vsdb) * 500))
				{
					error_Set(ERR_SINK_DOES_NOT_SUPPORT_TMDS_CLOCK);
					LOG_WARNING2("Sink does not support TMDS clock", videoParams_GetTmdsClock(video));
					LOG_WARNING2("Maximum TMDS clock supported is", hdmivsdb_GetMaxTmdsClk(&vsdb) * 500);
					errorCode = FALSE;
				}
			}
		}
		else
		{
			error_Set(ERR_SINK_DOES_NOT_SUPPORT_HDMI);
			LOG_WARNING("Sink does not support HDMI");
			errorCode = FALSE;
		}
		/* video out encoding (YCC/RGB) */
		if (videoParams_GetEncodingOut(video) == YCC444
				&& !api_EdidSupportsYcc444())
		{
			error_Set(ERR_SINK_DOES_NOT_SUPPORT_YCC444_ENCODING);
			LOG_WARNING("Sink does NOT support YCC444 encoding");
			errorCode = FALSE;
		}
		else if (videoParams_GetEncodingOut(video) == YCC422
				&& !api_EdidSupportsYcc422())
		{
			error_Set(ERR_SINK_DOES_NOT_SUPPORT_YCC422_ENCODING);
			LOG_WARNING("Sink does NOT support YCC422 encoding");
			errorCode = FALSE;
		}
		/* check extended colorimetry data and if supported by sink */
		if (videoParams_GetColorimetry(video) == EXTENDED_COLORIMETRY)
		{
			if (api_EdidColorimetryDataBlock(&colorimetry) == TRUE)
			{
				if (!colorimetryDataBlock_SupportsXvYcc601(&colorimetry)
						&& videoParams_GetExtColorimetry(video) == 0)
				{
					error_Set(ERR_SINK_DOES_NOT_SUPPORT_XVYCC601_COLORIMETRY);
					LOG_WARNING("Sink does NOT support xvYcc601 extended colorimetry");
					errorCode = FALSE;
				}
				else if (!colorimetryDataBlock_SupportsXvYcc709(&colorimetry)
						&& videoParams_GetExtColorimetry(video) == 1)
				{
					error_Set(ERR_SINK_DOES_NOT_SUPPORT_XVYCC709_COLORIMETRY);
					LOG_WARNING("Sink does NOT support xvYcc709 extended colorimetry");
					errorCode = FALSE;
				}
				else
				{
					error_Set(ERR_EXTENDED_COLORIMETRY_PARAMS_INVALID);
					LOG_WARNING("Invalid extended colorimetry parameters");
					errorCode = FALSE;
				}
			}
			else
			{
				error_Set(ERR_SINK_DOES_NOT_SUPPORT_EXTENDED_COLORIMETRY);
				errorCode = FALSE;
			}
		}
	}
	else
	{ /* DVI mode */
		if (videoParams_GetEncodingOut(video) != RGB)
		{
			error_Set(ERR_DVI_MODE_WITH_YCC_ENCODING);
			LOG_WARNING("DVI mode does not support video encoding other than RGB");
			errorCode = FALSE;
		}
		if (videoParams_IsPixelRepetition(video))
		{
			error_Set(ERR_DVI_MODE_WITH_PIXEL_REPETITION);
			LOG_WARNING("DVI mode does not support pixel repetition");
			errorCode = FALSE;
		}
		if (videoParams_GetTmdsClock(video) > 16500)
		{
			LOG_WARNING("Sink must support DVI dual-link");
		}
		/* check colorimetry data */
		if ((videoParams_GetColorimetry(video) == EXTENDED_COLORIMETRY))
		{
			error_Set(ERR_DVI_MODE_WITH_EXTENDED_COLORIMETRY);
			LOG_WARNING("DVI does not support extended colorimetry");
			errorCode = FALSE;
		}
	}
	/*
	 * DTD check
	 * always checking VALID to minimise code execution (following probability)
	 * this video code is to be supported by all dtv's
	 */
	valid = (dtd_GetCode(videoParams_GetDtd(video)) == 1) ? TRUE : FALSE;
	if (!valid)
	{
		for (i = 0; i < api_EdidDtdCount(); i++)
		{
			api_EdidDtd(i, &dtd);
#if 0
			if (dtd_GetCode(videoParams_GetDtd(video)) != (u8) (-1))
			{
				if (dtd_GetCode(videoParams_GetDtd(video)) == dtd_GetCode(&dtd))
				{
					valid = TRUE;
					break;
				}
			}
#endif
			//else if (dtd_IsEqual(videoParams_GetDtd(video), &dtd))
			if (dtd_IsEqual(videoParams_GetDtd(video), &dtd))
			{
				valid = TRUE;
				break;
			}
		}
	}
	if (!valid)
	{
		if (dtd_GetCode(videoParams_GetDtd(video)) != (u8) (-1))
		{
			for (i = 0; i < api_EdidSvdCount(); i++)
			{
				api_EdidSvd(i, &svd);
				if ((unsigned) (dtd_GetCode(videoParams_GetDtd(video)))
						== shortVideoDesc_GetCode(&svd))
				{
					valid = TRUE;
					break;
				}
			}
		}
	}
	if (!valid)
	{
		error_Set(ERR_SINK_DOES_NOT_SUPPORT_SELECTED_DTD);
		LOG_WARNING("Sink does NOT indicate support for selected DTD");
		errorCode = FALSE;
	}
	/* check quantization range */
	if (api_EdidVideoCapabilityDataBlock(&capability) == TRUE)
	{
		if (!videoCapabilityDataBlock_GetQuantizationRangeSelectableRGB(
				&capability) && videoParams_GetRgbQuantizationRange(video) > 1)
		{
			LOG_WARNING("Sink does NOT indicate support for selected quantization range");
		}
	}
	return errorCode;
}

