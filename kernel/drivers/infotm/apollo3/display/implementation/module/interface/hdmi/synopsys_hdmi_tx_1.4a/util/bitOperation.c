/*
 * @file:bitOperation.c
 *
 *  Created on: Jul 5, 2010
 * 
 * Synopsys Inc.
 * SG DWC PT02
 */

#include "bitOperation.h"

	u16 bitOperation_ConcatBits(u8 bHi, u8 oHi, u8 nHi, u8 bLo, u8 oLo, u8 nLo)
	{
		return (bitOperation_BitField(bHi, oHi, nHi) << nLo) | bitOperation_BitField(bLo, oLo, nLo);
	}

	u16 bitOperation_Bytes2Word(const u8 hi, const u8 lo)
	{
		return bitOperation_ConcatBits(hi, 0, 8, lo, 0, 8);
	}

	u8 bitOperation_BitField(const u16 data, u8 shift, u8 width)
	{
		return ((data >> shift) & ((((u16)1) << width) - 1));
	}

	u32 bitOperation_Bytes2Dword(u8 b3, u8 b2, u8 b1, u8 b0)
	{
		u32 retval = 0;
		retval |= b0 << (0 * 8);
		retval |= b1 << (1 * 8);
		retval |= b2 << (2 * 8);
		retval |= b3 << (3 * 8);
		return retval;
	}

