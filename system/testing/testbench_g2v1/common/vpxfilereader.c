/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "software/test/common/vpxfilereader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vp9/ivf.h"
#ifdef WEBM_ENABLED
#include <stdarg.h>
#include "nestegg/nestegg.h"
#endif /* WEBM_ENABLED */
#include <sys/types.h>
/* Module defines */
#define VP8_FOURCC (0x30385056)
#define VP9_FOURCC (0x30395056)

/* Module datatypes */
enum RdrFileFormat {
  FF_VP7,
  FF_VP8,
  FF_VP9,
  FF_WEBP,
  FF_WEBM,
  FF_NONE
};

#ifdef WEBM_ENABLED
struct InputCtx {
  nestegg* nestegg_ctx;
  nestegg_packet* pkt;
  unsigned int chunk;
  unsigned int chunks;
  unsigned int video_track;
};
#endif /* WEBM_ENABLED */

struct FfReader {
  enum RdrFileFormat format;
  i32 bitstream_format;
  u32 ivf_headers_read;
  FILE* file;
#ifdef WEBM_ENABLED
  struct InputCtx input_ctx;
#endif /* WEBM_ENABLED */
};

/* Module local functions. */
static i32 ReadIvfFileHeader(FILE* fin);
static i32 ReadIvfFrameHeader(FILE* fin, u32* frame_size);
static i32 FfCheckFormat(struct FfReader* inst);
#ifdef WEBM_ENABLED
static int FileIsWebm(struct InputCtx* input, FILE* infile,
                      unsigned int* fourcc, unsigned int* width,
                      unsigned int* height, unsigned int* fps_den,
                      unsigned int* fps_num);
#endif /* WEBM_ENABLED */

VpxReaderInst VpxRdrOpen(const char* filename, u32 mode) {
  struct FfReader* inst = calloc(1, sizeof(struct FfReader));
  if (inst == NULL) return NULL;
  inst->format = FF_NONE;
  inst->file = fopen(filename, "rb");
  if (inst->file == NULL) {
    free(inst);
    return NULL;
  }
  inst->bitstream_format = FfCheckFormat(inst);
  if (inst->bitstream_format == 0) {
    fclose(inst->file);
    free(inst);
    return NULL;
  }
  return inst;
}

int VpxRdrIdentifyFormat(VpxReaderInst inst) {
  struct FfReader* reader = (struct FfReader*)inst;
  return reader->bitstream_format;
}

int VpxRdrReadFrame(VpxReaderInst inst, u8* buffer, i32* size) {
  u32 tmp;
  off_t frame_header_pos;
  u32 frame_size = 0;
  struct FfReader* ff = (struct FfReader*)inst;
  FILE* fin = ff->file;

  /* Read VPx IVF file header */
  if ((ff->format == FF_VP8 || ff->format == FF_VP9) && !ff->ivf_headers_read) {
    tmp = ReadIvfFileHeader(fin);
    if (tmp != 0) return tmp;
    ff->ivf_headers_read = 1;
  }

  frame_header_pos = ftello(fin);
  /* Read VPx IVF frame header */
  if (ff->format == FF_VP8 || ff->format == FF_VP9) {
    tmp = ReadIvfFrameHeader(fin, &frame_size);
    if (tmp != 0) return tmp;
  } else if (ff->format == FF_VP7) {
    u8 size[4];
    tmp = fread(size, sizeof(u8), 4, fin);
    if (tmp != 4) return -1;

    frame_size = size[0] + (size[1] << 8) + (size[2] << 16) + (size[3] << 24);
  } else if (ff->format == FF_WEBP) {
    char signature[] = "WEBP";
    char format[] = "VP8 ";
    char tmp[4];
    fseeko(fin, 8, SEEK_CUR);
    if (!fread(tmp, sizeof(u8), 4, fin)) return -1;
    if (strncmp(signature, tmp, 4)) return -1;
    if (!fread(tmp, sizeof(u8), 4, fin)) return -1;
    if (strncmp(format, tmp, 4)) return -1;
    if (!fread(tmp, sizeof(u8), 4, fin)) return -1;
    frame_size = tmp[0] + (tmp[1] << 8) + (tmp[2] << 16) + (tmp[3] << 24);
  }
#ifdef WEBM_ENABLED
  else /* FF_WEBM */
  {
    size_t buf_sz;
    u8* tmp_pkt;
    if (ff->input_ctx.chunk >= ff->input_ctx.chunks) {
      unsigned int track;

      do {
        /* End of this packet, get another. */
        if (ff->input_ctx.pkt) nestegg_free_packet(ff->input_ctx.pkt);

        if (nestegg_read_packet(ff->input_ctx.nestegg_ctx,
                                &ff->input_ctx.pkt) <= 0 ||
            nestegg_packet_track(ff->input_ctx.pkt, &track))
          return -1;

      } while (track != ff->input_ctx.video_track);

      if (nestegg_packet_count(ff->input_ctx.pkt, &ff->input_ctx.chunks))
        return -1;
      ff->input_ctx.chunk = 0;
    }

    if (nestegg_packet_data(ff->input_ctx.pkt, ff->input_ctx.chunk, &tmp_pkt,
                            &buf_sz))
      return -1;

    if (buf_sz > *size) {
      *size = buf_sz;
      return -1;
    }
    ff->input_ctx.chunk++;
    frame_size = buf_sz;

    memcpy((void*)buffer, tmp_pkt, frame_size);

    return frame_size;
  }
#endif /* WEBM_ENABLED */

  if (feof(fin)) {
    fprintf(stderr, "EOF: Input\n");
    return -1;
  }

  if (frame_size > *size) {
    fprintf(stderr, "Frame size %d > buffer size %d\n", frame_size, *size);
    fseeko(fin, frame_header_pos, SEEK_SET);
    *size = frame_size;
    return -1;
  }

  {
    size_t result = fread((u8*)buffer, sizeof(u8), frame_size, fin);

    if (result != frame_size) {
      /* fread failed. */
      return -1;
    }
  }

  return frame_size;
}

void VpxRdrClose(VpxReaderInst inst) {
  struct FfReader* reader = (struct FfReader*)inst;
#ifdef WEBM_ENABLED
  if (reader->input_ctx.nestegg_ctx)
    nestegg_destroy(reader->input_ctx.nestegg_ctx);
#endif
  fclose(reader->file);
  free(reader);
}

static i32 ReadIvfFileHeader(FILE* fin) {
  IVF_HEADER ivf;
  u32 tmp;

  tmp = fread(&ivf, sizeof(char), sizeof(IVF_HEADER), fin);
  if (tmp == 0) return -1;

  return 0;
}

static i32 ReadIvfFrameHeader(FILE* fin, u32* frame_size) {
  union {
    IVF_FRAME_HEADER ivf;
    u8 p[12];
  } fh;
  u32 tmp;

  tmp = fread(&fh, sizeof(char), sizeof(IVF_FRAME_HEADER), fin);
  if (tmp == 0) return -1;

  *frame_size = fh.p[0] + (fh.p[1] << 8) + (fh.p[2] << 16) + (fh.p[3] << 24);

  return 0;
}

static i32 FfCheckFormat(struct FfReader* ff) {
  char id[5] = "DKIF";
  char id2[5] = "RIFF";
  char string[5] = "";
  u32 format = 0;

#ifdef WEBM_ENABLED
  u32 tmp;
  u32 four_cc = 0;
  if (FileIsWebm(&ff->input_ctx, ff->file, &four_cc, &tmp, &tmp, &tmp, &tmp)) {
    ff->format = FF_WEBM;
    if (four_cc == VP8_FOURCC)
      format = BITSTREAM_VP8;
    else if (four_cc == VP9_FOURCC)
      format = BITSTREAM_VP9;
    nestegg_track_seek(ff->input_ctx.nestegg_ctx, ff->input_ctx.video_track, 0);
  } else if (fread(string, 1, 5, ff->file) == 5)
#else
  if (fread(string, 1, 5, ff->file) == 5)
#endif
  {
    if (!strncmp(id, string, 5)) {
      fseeko(ff->file, 8, SEEK_SET);
      if (!fread(string, 1, 4, ff->file)) return 0;
      if (!strncmp("VP8\0", string, 4)) {
        ff->format = FF_VP8;
        format = BITSTREAM_VP8;
      } else if (!strncmp("VP90", string, 4)) {
        ff->format = FF_VP9;
        format = BITSTREAM_VP9;
      }
    } else if (!strncmp(id2, string, 4)) {
      ff->format = FF_WEBP;
      format = BITSTREAM_WEBP;
    } else {
      ff->format = FF_VP7;
      format = BITSTREAM_VP7;
    }
    rewind(ff->file);
  }
  return format;
}

#ifdef WEBM_ENABLED
static int NesteggReadCb(void* buffer, size_t length, void* userdata) {
  FILE* f = userdata;

  if (fread(buffer, 1, length, f) < length) {
    if (ferror(f)) return -1;
    if (feof(f)) return 0;
  }
  return 1;
}

static int NesteggSeekCb(int64_t offset, int whence, void* userdata) {
  switch (whence) {
    case NESTEGG_SEEK_SET:
      whence = SEEK_SET;
      break;
    case NESTEGG_SEEK_CUR:
      whence = SEEK_CUR;
      break;
    case NESTEGG_SEEK_END:
      whence = SEEK_END;
      break;
  };
  return fseeko(userdata, (off_t)offset, whence) ? -1 : 0;
}

static int64_t NesteggTellCb(void* userdata) { return (int64_t)ftello(userdata); }

static int FileIsWebm(struct InputCtx* input, FILE* infile,
                      unsigned int* fourcc, unsigned int* width,
                      unsigned int* height, unsigned int* fps_den,
                      unsigned int* fps_num) {
  unsigned int i, n;
  int track_type = -1;
  off_t file_size = 0;

  nestegg_io io = {NesteggReadCb, NesteggSeekCb, NesteggTellCb, infile};
  nestegg_video_params params;

  /* Get the file size for nestegg. */
  fseeko(infile, 0, SEEK_END);
  file_size = ftello(infile);
  fseeko(infile, 0, SEEK_SET);

  if (nestegg_init(&input->nestegg_ctx, io, NULL, file_size)) goto fail;

  if (nestegg_track_count(input->nestegg_ctx, &n)) goto fail;

  for (i = 0; i < n; i++) {
    track_type = nestegg_track_type(input->nestegg_ctx, i);

    if (track_type == NESTEGG_TRACK_VIDEO)
      break;
    else if (track_type < 0)
      goto fail;
  }

  if (nestegg_track_codec_id(input->nestegg_ctx, i) != NESTEGG_CODEC_VP9) {
    fprintf(stderr, "Not VPX video, quitting.\n");
    exit(1);
  }

  input->video_track = i;

  if (nestegg_track_video_params(input->nestegg_ctx, i, &params)) goto fail;

  *fps_den = 0;
  *fps_num = 0;
  *fourcc = VP9_FOURCC;
  *width = params.width;
  *height = params.height;
  return 1;
fail:
  input->nestegg_ctx = NULL;
  rewind(infile);
  return 0;
}
#endif /* WEBM_ENABLED */
