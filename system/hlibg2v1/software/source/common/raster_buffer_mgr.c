/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "software/source/common/raster_buffer_mgr.h"
#include "hevc_container.h"

struct BufferPair {
  struct DWLLinearMem tiled_buffer;
  struct DWLLinearMem raster_buffer;
#ifdef DOWN_SCALER
  struct DWLLinearMem dscale_buffer;
#endif
};

typedef struct {
  u32 num_buffers;
  struct BufferPair* buffer_map;
  const void* dwl;
#ifdef USE_EXTERNAL_BUFFER
  u32 ext_buffer_config;
#endif
} RasterBufferMgrInst;

#ifndef USE_EXTERNAL_BUFFER
RasterBufferMgr RbmInit(struct RasterBufferParams params) {
  RasterBufferMgrInst* inst = DWLmalloc(sizeof(RasterBufferMgrInst));
  inst->buffer_map = DWLcalloc(params.num_buffers, sizeof(struct BufferPair));
  inst->num_buffers = params.num_buffers;
  inst->dwl = params.dwl;
  u32 size = params.width * params.height * 3 / 2;
#ifdef DOWN_SCALER
  u32 dscale_size = params.ds_width * params.ds_height * 3 / 2;
#endif
  struct DWLLinearMem empty = {0};

  for (int i = 0; i < inst->num_buffers; i++) {
    if (size) {
      if (DWLMallocLinear(inst->dwl, size, &inst->buffer_map[i].raster_buffer)) {
        RbmRelease(inst);
        return NULL;
      }
    } else {
      inst->buffer_map[i].raster_buffer = empty;
    }
    inst->buffer_map[i].tiled_buffer = params.tiled_buffers[i];
#ifdef DOWN_SCALER
    if (dscale_size) {
      if (DWLMallocLinear(inst->dwl, dscale_size, &inst->buffer_map[i].dscale_buffer)) {
        RbmRelease(inst);
        return NULL;
      }
    } else {
      inst->buffer_map[i].dscale_buffer = empty;
    }
#endif
  }
  return inst;
}

#else

/* Allocate internal buffers here. */
RasterBufferMgr RbmInit(struct RasterBufferParams params) {
  RasterBufferMgrInst* inst = DWLmalloc(sizeof(RasterBufferMgrInst));
  inst->buffer_map = DWLcalloc(params.num_buffers, sizeof(struct BufferPair));
  inst->num_buffers = params.num_buffers;
  inst->dwl = params.dwl;
  inst->ext_buffer_config = params.ext_buffer_config;
  u32 size = params.width * params.height * 3 / 2;
#ifdef DOWN_SCALER
  u32 dscale_size = params.ds_width * params.ds_height * 3 / 2;
#endif

  for (int i = 0; i < inst->num_buffers; i++) {
    if (!IS_EXTERNAL_BUFFER(inst->ext_buffer_config, RASTERSCAN_OUT_BUFFER) && size) {
      if (DWLMallocLinear(inst->dwl, size, &inst->buffer_map[i].raster_buffer)) {
        RbmRelease(inst);
        return NULL;
      }
    } 
    inst->buffer_map[i].tiled_buffer = params.tiled_buffers[i];
#ifdef DOWN_SCALER
    if (!IS_EXTERNAL_BUFFER(inst->ext_buffer_config, DOWNSCALE_OUT_BUFFER) && dscale_size) {
      if (DWLMallocLinear(inst->dwl, dscale_size, &inst->buffer_map[i].dscale_buffer)) {
        RbmRelease(inst);
        return NULL;
      }
    }
#endif
  }
  return inst;
}
#endif

struct DWLLinearMem RbmGetRasterBuffer(RasterBufferMgr instance,
                                       struct DWLLinearMem key) {
  RasterBufferMgrInst* inst = (RasterBufferMgrInst*)instance;
  for (int i = 0; i < inst->num_buffers; i++)
    if (inst->buffer_map[i].tiled_buffer.virtual_address == key.virtual_address)
      return inst->buffer_map[i].raster_buffer;
  struct DWLLinearMem empty = {0};
  return empty;
}

#ifdef DOWN_SCALER
struct DWLLinearMem RbmGetDscaleBuffer(RasterBufferMgr instance,
                                       struct DWLLinearMem key) {
  RasterBufferMgrInst* inst = (RasterBufferMgrInst*)instance;
  for (int i = 0; i < inst->num_buffers; i++)
    if (inst->buffer_map[i].tiled_buffer.virtual_address == key.virtual_address)
      return inst->buffer_map[i].dscale_buffer;
  struct DWLLinearMem empty = {0};
  return empty;
}
#endif

struct DWLLinearMem RbmGetTiledBuffer(RasterBufferMgr instance,
                                      struct DWLLinearMem key) {
  RasterBufferMgrInst* inst = (RasterBufferMgrInst*)instance;
  for (int i = 0; i < inst->num_buffers; i++)
    if (inst->buffer_map[i].raster_buffer.virtual_address ==
        key.virtual_address
#ifdef DOWN_SCALER
        || inst->buffer_map[i].dscale_buffer.virtual_address ==
        key.virtual_address
#endif
        )
      return inst->buffer_map[i].tiled_buffer;
  struct DWLLinearMem empty = {0};
  return empty;
}

#ifndef USE_EXTERNAL_BUFFER
void RbmRelease(RasterBufferMgr instance) {
  RasterBufferMgrInst* inst = (RasterBufferMgrInst*)instance;
  for (int i = 0; i < inst->num_buffers; i++) {
    if (inst->buffer_map[i].raster_buffer.virtual_address != NULL)
      DWLFreeLinear(inst->dwl, &inst->buffer_map[i].raster_buffer);
#ifdef DOWN_SCALER
    if (inst->buffer_map[i].dscale_buffer.virtual_address != NULL)
      DWLFreeLinear(inst->dwl, &inst->buffer_map[i].dscale_buffer);
#endif
  }
  DWLfree(inst->buffer_map);
  DWLfree(inst);
}
#else
/* Make sure all the external buffers have been released before calling RbmRelease. */
void RbmRelease(RasterBufferMgr instance) {
  RasterBufferMgrInst* inst = (RasterBufferMgrInst*)instance;
  for (int i = 0; i < inst->num_buffers; i++) {
    if (inst->buffer_map[i].raster_buffer.virtual_address != NULL &&
        !IS_EXTERNAL_BUFFER(inst->ext_buffer_config, RASTERSCAN_OUT_BUFFER))
      DWLFreeLinear(inst->dwl, &inst->buffer_map[i].raster_buffer);
#ifdef DOWN_SCALER
    if (inst->buffer_map[i].dscale_buffer.virtual_address != NULL &&
        !IS_EXTERNAL_BUFFER(inst->ext_buffer_config, DOWNSCALE_OUT_BUFFER))
      DWLFreeLinear(inst->dwl, &inst->buffer_map[i].dscale_buffer);
#endif
  }
  DWLfree(inst->buffer_map);
  DWLfree(inst);
}

/* Return next external buffer to be released, otherwise return empty buffer. */
struct DWLLinearMem RbmReleaseBuffer(RasterBufferMgr instance) {
  struct DWLLinearMem buf = {0};
  RasterBufferMgrInst* inst = (RasterBufferMgrInst*)instance;
  for (int i = 0; i < inst->num_buffers; i++) {
    if (inst->buffer_map[i].raster_buffer.virtual_address != NULL &&
        IS_EXTERNAL_BUFFER(inst->ext_buffer_config, RASTERSCAN_OUT_BUFFER)) {
      buf = inst->buffer_map[i].raster_buffer;
      DWLmemset(&inst->buffer_map[i].raster_buffer, 0, sizeof(struct DWLLinearMem));
      return buf;
    }

#ifdef DOWN_SCALER
    if (inst->buffer_map[i].dscale_buffer.virtual_address != NULL&&
        IS_EXTERNAL_BUFFER(inst->ext_buffer_config, DOWNSCALE_OUT_BUFFER)) {
      buf = inst->buffer_map[i].dscale_buffer;
      DWLmemset(&inst->buffer_map[i].dscale_buffer, 0, sizeof(struct DWLLinearMem));
      return buf;
    }
#endif
  }

  return buf;
}

void RbmAddRsBuffer(RasterBufferMgr instance, struct DWLLinearMem *raster_buffer, i32 i) {
  RasterBufferMgrInst* inst = (RasterBufferMgrInst*)instance;

  if (i < inst->num_buffers)
    inst->buffer_map[i].raster_buffer = *raster_buffer;
}

#ifdef DOWN_SCALER
void RbmAddDsBuffer(RasterBufferMgr instance, struct DWLLinearMem *dscale_buffer, i32 i) {
  RasterBufferMgrInst* inst = (RasterBufferMgrInst*)instance;

  if (i < inst->num_buffers)
    inst->buffer_map[i].dscale_buffer = *dscale_buffer;
}

#endif

#endif
