/* Copyright 2013 Google Inc. All Rights Reserved. */

/* Module to parse HEVC bytestream. */

#ifndef BYTESTREAM_PARSER_H_
#define BYTESTREAM_PARSER_H_

#include "software/source/inc/basetype.h"
#include "software/test/common/demuxer_types.h"

typedef const void* BSParserInst;

BSParserInst ByteStreamParserOpen(const char* fname, u32 mode);
int ByteStreamParserIdentifyFormat(BSParserInst inst);
void ByteStreamParserHeadersDecoded(BSParserInst inst);
int ByteStreamParserReadFrame(BSParserInst inst, u8* buffer, u8** stream, i32* size, u8 rb);
void ByteStreamParserClose(BSParserInst inst);

#endif /* BYTESTREAM_PARSER_H_ */
