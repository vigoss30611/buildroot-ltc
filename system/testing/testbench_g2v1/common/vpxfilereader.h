/* Copyright 2013 Google Inc. All Rights Reserved. */

/* WebM/IVF/RAW file reading functionality. Supports only VPx codecs. */

#ifndef __VPXFILEREADER_H__
#define __VPXFILEREADER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "software/source/inc/basetype.h"
#include "software/test/common/demuxer_types.h"

typedef const void* VpxReaderInst;

VpxReaderInst VpxRdrOpen(const char* filename, u32 mode);
int VpxRdrIdentifyFormat(VpxReaderInst inst);
int VpxRdrReadFrame(VpxReaderInst inst, u8* buffer, i32* size);
void VpxRdrClose(VpxReaderInst inst);

#ifdef __cplusplus
}
#endif

#endif /* __VPXFILEREADER_H__ */
