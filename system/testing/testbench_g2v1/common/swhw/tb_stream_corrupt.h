/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * This module declares functions for corrupting data in streams. */

#ifndef TB_STREAM_CORRUPT_H
#define TB_STREAM_CORRUPT_H

#include "basetype.h"

void TBInitializeRandom(u32 seed);

u32 TBRandomizeBitSwapInStream(u8* stream, u32 stream_len, char* odds);

u32 TBRandomizePacketLoss(char* odds, u8* packet_loss);

u32 TBRandomizeU32(u32* value);

/* u32 RandomizeBitLossInStream(u8* stream,
                                               u32* stream_len,
                                               char* odds); */

#endif /* TB_STREAM_CORRUPT_H */
