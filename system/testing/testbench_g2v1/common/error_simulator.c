/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "software/test/common/error_simulator.h"

#include <stdio.h>
#include <stdlib.h>

#include "software/test/common/swhw/tb_stream_corrupt.h"

#define DEBUG_PRINT(str) printf str

struct ErrorSim {
  Demuxer concrete_demuxer;
  struct ErrorSimulationParams params;
};

ErrorSimulator ErrorSimulatorInject(Demuxer* demuxer,
                                    struct ErrorSimulationParams params) {
  /* Our error simulator injects it's own function in place of the bitstream
     read function. */
  struct ErrorSim* sim = calloc(1, sizeof(struct ErrorSim));
  sim->concrete_demuxer = *demuxer;
  demuxer->GetVideoFormat = ErrorSimulatorIdentifyFormat;
  demuxer->ReadPacket = ErrorSimulatorReadPacket;
  demuxer->close = ErrorSimulatorClose;
  TBInitializeRandom(params.seed);
  sim->params = params;
  return sim;
}

int ErrorSimulatorIdentifyFormat(ErrorSimulator inst) {
  struct ErrorSim* sim = (struct ErrorSim*)inst;
  return sim->concrete_demuxer.GetVideoFormat(sim->concrete_demuxer.inst);
}

int ErrorSimulatorReadPacket(ErrorSimulator inst, u8* buffer, i32* size) {
  struct ErrorSim* sim = (struct ErrorSim*)inst;
  u8* orig_buffer = buffer;
  i32 orig_size = *size;
  int rv = sim->concrete_demuxer.ReadPacket(sim->concrete_demuxer.inst, buffer,
                                            size);
  if (rv < 0) return rv;

  /* TODO(vmr): Add header corruption feature. */
  if (sim->params.lose_packets) {
    u8 lose_packet = 0;
    if (TBRandomizePacketLoss(sim->params.packet_loss_odds, &lose_packet)) {
      DEBUG_PRINT(("Packet loss simulator error (wrong config?)\n"));
    }
    if (lose_packet) {
      *size = orig_size;
      return ErrorSimulatorReadPacket(inst, orig_buffer, size);
    }
  }
  if (sim->params.swap_bits_in_stream) {
    if (TBRandomizeBitSwapInStream(buffer, *size, sim->params.swap_bit_odds)) {
      DEBUG_PRINT(("Bitswap randomizer error (wrong config?)\n"));
    }
  }
  if (sim->params.truncate_stream) {
    u32 random_size = rv;
    if (TBRandomizeU32(&random_size)) {
      DEBUG_PRINT(("Truncate randomizer error (wrong config?)\n"));
    }
    rv = random_size;
  }
  return rv;
}

void ErrorSimulatorClose(ErrorSimulator inst) {
  struct ErrorSim* sim = (struct ErrorSim*)inst;
  sim->concrete_demuxer.close(sim->concrete_demuxer.inst);
  free(sim);
}
