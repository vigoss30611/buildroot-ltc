/* 
 * halAudioGpa.h
 * 
 * Created on Oct. 30th 2010
 * Synopsys Inc.
 */

#ifndef HALAUDIOGPA_H_
#define HALAUDIOGPA_H_

#include "types.h"
 
void halAudioGpa_ResetFifo(u16 baseAddress);
void halAudioGpa_ChannelEnable(u16 baseAddress, u8 enable, u8 channel);
void halAudioGpa_HbrEnable(u16 baseAddress, u8 enable);

u8 halAudioGpa_InterruptStatus(u16 baseAddress);
void halAudioGpa_InterruptMask(u16 baseAddress, u8 value);
void halAudioGpa_InterruptPolarity(u16 baseAddress, u8 value);

#endif /* HALAUDIOGPA_H_ */
