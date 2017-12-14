/*
 * audioParams.h
 *
 *  Created on: Jul 2, 2010
 * 
 * Synopsys Inc.
 * SG DWC PT02
 */
/**
 * @file
 * Define audio parameters structure and functions to manipulate the
 * information held by the structure.
 * Audio interfaces, packet types and coding types are also defined here
 */
#ifndef AUDIOPARAMS_H_
#define AUDIOPARAMS_H_
#include "types.h"

typedef enum
{
	I2S = 0, SPDIF, HBR, GPA
} interfaceType_t;

typedef enum
{
	AUDIO_SAMPLE = 1, HBR_STREAM
} packet_t;

typedef enum
{
	PCM = 1,
	AC3,
	MPEG1,
	MP3,
	MPEG2,
	AAC,
	DTS,
	ATRAC,
	ONE_BIT_AUDIO,
	DOLBY_DIGITAL_PLUS,
	DTS_HD,
	MAT,
	DST,
	WMAPRO
} codingType_t;

/** For detailed handling of this structure, refer to documentation of the 
functions */
typedef struct
{
	interfaceType_t mInterfaceType;

	codingType_t mCodingType; /** (audioParams_t *params, see InfoFrame) */

	u8 mChannelAllocation; /** channel allocation (audioParams_t *params, 
						   see InfoFrame) */

	u8 mSampleSize; /**  sample size (audioParams_t *params, 16 to 24) */

	u32 mSamplingFrequency; /** sampling frequency (audioParams_t *params, Hz) */

	u8 mLevelShiftValue; /** level shift value (audioParams_t *params, 
						 see InfoFrame) */

	u8 mDownMixInhibitFlag; /** down-mix inhibit flag (audioParams_t *params, 
							see InfoFrame) */

	u32 mOriginalSamplingFrequency; /** Original sampling frequency */

	u8 mIecCopyright; /** IEC copyright */

	u8 mIecCgmsA; /** IEC CGMS-A */

	u8 mIecPcmMode; /** IEC PCM mode */

	u8 mIecCategoryCode; /** IEC category code */

	u8 mIecSourceNumber; /** IEC source number */

	u8 mIecClockAccuracy; /** IEC clock accuracy */ 

	packet_t mPacketType; /** packet type. currently only Audio Sample (AUDS) 
						  and High Bit Rate (HBR) are supported */

	u16 mClockFsFactor; /** Input audio clock Fs factor used at the audop 
						packetizer to calculate the CTS value and ACR packet insertion rate */
} audioParams_t;

/**
 * This method resets the parameters strucutre to a known state
 * SPDIF 16bits 32Khz Linear PCM
 * It is recommended to call this method before setting any parameters
 * to start from a stable and known condition avoid overflows.
 * @param params pointer to the audio parameters structure
 */
void audioParams_Reset(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @return the Channel Allocation code from Table 20 in CEA-861-D
 */
u8 audioParams_GetChannelAllocation(audioParams_t *params);
/**
 * Set the audio channels' set up
 * @param params pointer to the audio parameters structure
 * @param value code of the channel allocation from Table 20 in CEA-861-D
 */
void audioParams_SetChannelAllocation(audioParams_t *params, u8 value);
/**
 * @param params pointer to the audio parameters structure
 * @return the audio coding type of audio stream
 */
u8 audioParams_GetCodingType(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param value the audio coding type of audio stream
 */
void audioParams_SetCodingType(audioParams_t *params, codingType_t value);
/**
 * @param params pointer to the audio parameters structure
 * @return 1 if the Down Mix Inhibit is allowed
 */
u8 audioParams_GetDownMixInhibitFlag(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param value 1 if the Down Mix Inhibit is allowed, 0 otherwise
 */
void audioParams_SetDownMixInhibitFlag(audioParams_t *params, u8 value);
/**
 * Category code ndicates the kind of equipment that generates the digital
 * audio interface signal.
 * @param params pointer to the audio parameters structure
 * @return the IEC audio category code
 * @note when trasnmitting L-PCM on I2S
 */
u8 audioParams_GetIecCategoryCode(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param value the IEC audio category code
 * @note when trasnmitting L-PCM on I2S
 */
void audioParams_SetIecCategoryCode(audioParams_t *params, u8 value);
/**
 * IEC CGMS-A indicated the copying permission
 * @param params pointer to the audio parameters structure
 * @return the IEC CGMS-A code
 * @note when trasnmitting L-PCM on I2S
 */
u8 audioParams_GetIecCgmsA(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param value the IEC CGMS-A code
 * @note when trasnmitting L-PCM on I2S
 */
void audioParams_SetIecCgmsA(audioParams_t *params, u8 value);
/**
 * @param params pointer to the audio parameters structure
 * @return the audio clock accuracy
 * @note when trasnmitting L-PCM on I2S
 */
u8 audioParams_GetIecClockAccuracy(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param value the audio clock accuracy
 * @note when trasnmitting L-PCM on I2S
 */
void audioParams_SetIecClockAccuracy(audioParams_t *params, u8 value);
/**
 * @param params pointer to the audio parameters structure
 * @return 1 if IEC Copyright is enable
 * @note when trasnmitting L-PCM on I2S
 */
u8 audioParams_GetIecCopyright(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param value 1 if IEC Copyright enable, 0 otherwise
 * @note when trasnmitting L-PCM on I2S
 */
void audioParams_SetIecCopyright(audioParams_t *params, u8 value);
/**
 * @param params pointer to the audio parameters structure
 * @return the IEC PCM audio mode (audioParams_t *params, pre-emphasis)
 * @note when trasnmitting L-PCM on I2S
 */
u8 audioParams_GetIecPcmMode(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param value the IEC PCM audio mode (audioParams_t *params, pre-emphasis)
 * @note when trasnmitting L-PCM on I2S
 */
void audioParams_SetIecPcmMode(audioParams_t *params, u8 value);
/**
 * @param params pointer to the audio parameters structure
 * @return the IEC Source Number
 * @note when trasnmitting L-PCM on I2S
 */
u8 audioParams_GetIecSourceNumber(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param value the IEC Source Number
 * @note when trasnmitting L-PCM on I2S
 */
void audioParams_SetIecSourceNumber(audioParams_t *params, u8 value);
/**
 * InterfaceType is an enumerate of the physical interfaces
 * of the HDMICTRL (audioParams_t *params, of which I2S is one)
 * @param params pointer to the audio parameters structure
 * @return the interface type to which the audio stream is inputted
 */
interfaceType_t audioParams_GetInterfaceType(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param value the interface type to which the audio stream is inputted
 */
void audioParams_SetInterfaceType(audioParams_t *params, interfaceType_t value);
/**
 * Level shift value is the value to which the sound level
 * is attenuated after down-mixing channels and speaker audio
 * @param params pointer to the audio parameters structure
 * @return the value in dB
 */
u8 audioParams_GetLevelShiftValue(audioParams_t *params);

void audioParams_SetLevelShiftValue(audioParams_t *params, u8 value);
/**
 * Original sampling frequency field may be used to indicate the sampling 
 * frequency of a signal prior to sampling frequency conversion in a consumer 
 * playback system.
 * @param params pointer to the audio parameters structure
 * @return a 32-bit word original sampling frequency
 * @note when trasnmitting L-PCM on I2S
 */
u32 audioParams_GetOriginalSamplingFrequency(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param value a 32-bit word original sampling frequency
 * @note when trasnmitting L-PCM on I2S
 */
void audioParams_SetOriginalSamplingFrequency(audioParams_t *params, u32 value);
/**
 * @param params pointer to the audio parameters structure
 * @return 8-bit sample size in bits
 */
u8 audioParams_GetSampleSize(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param value 8-bit sample size in bits
 */
void audioParams_SetSampleSize(audioParams_t *params, u8 value);
/**
 * 192000 Hz| 176400 Hz| 96000 Hz| 88200 Hz| 48000 Hz| 44100 Hz| 32000 Hz
 * @param params pointer to the audio parameters structure
 * @return 32-bit word sampling frequency
 * @note when trasnmitting L-PCM on I2S
 */
u32 audioParams_GetSamplingFrequency(audioParams_t *params);
/**
 * 192000 Hz| 176400 Hz| 96000 Hz| 88200 Hz| 48000 Hz| 44100 Hz| 32000 Hz
 * @param params pointer to the audio parameters structure
 * @param value 32-bit word sampling frequency
 * @note when trasnmitting L-PCM on I2S
 */
void audioParams_SetSamplingFrequency(audioParams_t *params, u32 value);
/**
 * [Hz]
 * @param params pointer to the audio parameters structure
 * @return audio clock in Hz
 */
u32 audioParams_AudioClock(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @return number of audio channels transmitted -1
 */
u8 audioParams_ChannelCount(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 */
u8 audioParams_IecOriginalSamplingFrequency(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 */
u8 audioParams_IecSamplingFrequency(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 */
u8 audioParams_IecWordLength(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @return 1 if it is linear PCM
 */
u8 audioParams_IsLinearPCM(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @return the type of audio packets received by controller
 */
packet_t audioParams_GetPacketType(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param packetType type of audio packets received by the controller
 * obselete for SPDIF
 */
void audioParams_SetPacketType(audioParams_t *params, packet_t packetType);
/** 
 * return if channel is enabled or not using the user's channel allocation
 * code 
 * @param params pointer to the audio parameters structure
 * @param channel in question
 * @return 1 if channel is to be enabled, 0 otherwise
 */
u8 audioParams_IsChannelEn(audioParams_t *params, u8 channel);
/**
 * @param params pointer to the audio parameters structure
 * @return the input audio clock Fs factor
 */
u16 audioParams_GetClockFsFactor(audioParams_t *params);
/**
 * @param params pointer to the audio parameters structure
 * @param clockFsFactor the input aidio clock Fs factor
 * this should be 512 for HBR
 * 128 or 512 for SPDIF
 * 64/128/256/512/1024 for I2S
 * obselete for GPA
 * @return the input audio clock fs factor
 */
void audioParams_SetClockFsFactor(audioParams_t *params, u16 clockFsFactor);

#endif /* AUDIOPARAMS_H_ */
