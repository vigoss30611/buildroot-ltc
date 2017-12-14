/* Copyright 2012 Google Inc. All Rights Reserved.
 *
 * SW/HW interface related definitions and functions. */

#ifndef HEVC_ASIC_H_
#define HEVC_ASIC_H_

#include "basetype.h"
#include "dwl.h"
#include "hevc_container.h"
#include "hevc_storage.h"

#define ASIC_CABAC_INIT_BUFFER_SIZE 3 * 160
/* 8 dc + 8 to boundary + 6*16 + 2*6*64 + 2*64 -> 63 * 16 bytes */
#define ASIC_SCALING_LIST_SIZE 16 * 64

/* tile border coefficients of filter */
#define ASIC_VERT_SAO_RAM_SIZE 48 /* bytes per pixel */

#define X170_DEC_TIMEOUT 0x00FFU
#define X170_DEC_SYSTEM_ERROR 0x0FFFU
#define X170_DEC_HW_RESERVED 0xFFFFU

u32 AllocateAsicBuffers(struct HevcDecContainer *dec_cont,
                        struct HevcDecAsic *asic_buff);
#ifndef USE_EXTERNAL_BUFFER
void ReleaseAsicBuffers(const void *dwl, struct HevcDecAsic *asic_buff);
#else
i32 ReleaseAsicBuffers(struct HevcDecContainer *dec_cont,
                       struct HevcDecAsic *asic_buff);
#endif

u32 AllocateAsicTileEdgeMems(struct HevcDecContainer *dec_cont);
void ReleaseAsicTileEdgeMems(struct HevcDecContainer *dec_cont);

void HevcSetupVlcRegs(struct HevcDecContainer *dec_cont);

void HevcInitRefPicList(struct HevcDecContainer *dec_cont);

u32 HevcRunAsic(struct HevcDecContainer *dec_cont,
                struct HevcDecAsic *asic_buff);
void HevcSetRegs(struct HevcDecContainer *dec_cont);

#endif /* HEVC_ASIC_H_ */
