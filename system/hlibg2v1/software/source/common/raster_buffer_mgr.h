/* Copyright 2013 Google Inc. All Rights Reserved. */
#ifndef RASTER_BUFFER_MGR_H
#define RASTER_BUFFER_MGR_H

#include "software/source/inc/basetype.h"
#include "software/source/inc/dwl.h"

typedef void* RasterBufferMgr;

struct RasterBufferParams {
  struct DWLLinearMem* tiled_buffers;
  u32 num_buffers;
  u32 width;
  u32 height;
#ifdef DOWN_SCALER
  u32 ds_width;
  u32 ds_height;
#endif
#ifdef USE_EXTERNAL_BUFFER
  u32 ext_buffer_config;
#endif
  const void* dwl;
};

RasterBufferMgr RbmInit(struct RasterBufferParams params);
struct DWLLinearMem RbmGetRasterBuffer(RasterBufferMgr instance,
                                       struct DWLLinearMem key);
struct DWLLinearMem RbmGetTiledBuffer(RasterBufferMgr instance,
                                      struct DWLLinearMem buffer);
#ifdef DOWN_SCALER
struct DWLLinearMem RbmGetDscaleBuffer(RasterBufferMgr instance,
                                       struct DWLLinearMem key);
#endif
void RbmRelease(RasterBufferMgr inst);

#ifdef USE_EXTERNAL_BUFFER
struct DWLLinearMem RbmReleaseBuffer(RasterBufferMgr inst);
void RbmAddRsBuffer(RasterBufferMgr instance, struct DWLLinearMem *raster_buffer, i32 i);
#ifdef DOWN_SCALER
void RbmAddDsBuffer(RasterBufferMgr instance, struct DWLLinearMem *dscale_buffer, i32 i);
#endif
#endif

#endif /* RASTER_BUFFER_MGR_H */
