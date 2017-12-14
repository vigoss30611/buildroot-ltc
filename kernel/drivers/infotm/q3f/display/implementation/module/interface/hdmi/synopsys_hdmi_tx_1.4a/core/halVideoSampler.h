/*
 * @file:halVideoSampler.h
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALVIDEOSAMPLER_H_
#define HALVIDEOSAMPLER_H_

#include "types.h"

void halVideoSampler_InternalDataEnableGenerator(u16 baseAddr, u8 bit);

void halVideoSampler_VideoMapping(u16 baseAddr, u8 value);

void halVideoSampler_StuffingGy(u16 baseAddr, u16 value);

void halVideoSampler_StuffingRcr(u16 baseAddr, u16 value);

void halVideoSampler_StuffingBcb(u16 baseAddr, u16 value);

#endif /* HALVIDEOSAMPLER_H_ */
