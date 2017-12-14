/*
 * @file:halFrameComposerAudioInfo.h
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALFRAMECOMPOSERAUDIOINFO_H_
#define HALFRAMECOMPOSERAUDIOINFO_H_

#include "types.h"
/*
 * Configure the channel count in the infoframe
 * @param noOfChannels
 */
void halFrameComposerAudioInfo_ChannelCount(u16 baseAddr, u8 noOfChannels);
/*
 * Configure the sample frequency in the infoframe - it can be set to zero in HDMI, although not for all types of audio encodings.
 * refer to table 18 CEA861-D
 * @param sf
 */
void halFrameComposerAudioInfo_SampleFreq(u16 baseAddr, u8 sf);
/*
 * Configure the channel allocation in the infoframe
 * refer to table 20 CEA-861-D spec
 * @param ca
 */
void halFrameComposerAudioInfo_AllocateChannels(u16 baseAddr, u8 ca);
/*
 * Configure the level shift value in the infoframe
 * @param lsv: value in dB
 */
void halFrameComposerAudioInfo_LevelShiftValue(u16 baseAddr, u8 lsv);
/*
 * Configure the down mix inhibt flag in the infoframe.
 * refer to table 22 in CEA 861 D
 * @param prohibited: true if prohibited
 */
void halFrameComposerAudioInfo_DownMixInhibit(u16 baseAddr, u8 prohibited);
/*
 * Configure the audio coding type
 * @param codingType
 * 0s for HDMI (refer to stream header)
 */
void halFrameComposerAudioInfo_CodingType(u16 baseAddr, u8 codingType);
/*
 * Configure the audio sampling size
 * @param ss
 * 0s for HDMI (refer to stream header)
 */
void halFrameComposerAudioInfo_SamplingSize(u16 baseAddr, u8 ss);

#endif /* HALFRAMECOMPOSERAUDIOINFO_H_ */
