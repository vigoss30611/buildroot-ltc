/* Copyright 2013 Google Inc. All Rights Reserved. */

/* Error simulator to be used with decoder testbench demuxers. */

#ifndef __ERROR_SIMULATOR_H__
#define __ERROR_SIMULATOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "software/source/inc/basetype.h"
#include "software/test/common/demuxer_types.h"

struct ErrorSimulationParams {
  u32 seed;
  u8 corrupt_headers;
  u8 truncate_stream;
  u8 swap_bits_in_stream;
  char swap_bit_odds[24];
  u8 lose_packets;
  char packet_loss_odds[24];
};

typedef const void* ErrorSimulator;

ErrorSimulator ErrorSimulatorInject(Demuxer* demuxer,
                                    struct ErrorSimulationParams params);
int ErrorSimulatorIdentifyFormat(ErrorSimulator inst);
void ErrorSimulatorHeadersDecoded(ErrorSimulator inst);
int ErrorSimulatorReadPacket(ErrorSimulator inst, u8* buffer, u8* stream[2], i32* size, u8 rb);
void ErrorSimulatorHeadersDecoded(ErrorSimulator inst);
void ErrorSimulatorClose(ErrorSimulator inst);

#ifdef __cplusplus
}
#endif

#endif /* __ERROR_SIMULATOR_H__ */
