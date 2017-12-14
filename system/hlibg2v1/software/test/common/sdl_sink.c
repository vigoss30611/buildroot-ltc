/* Copyright 2013 Google Inc. All Rights Reserved. */

#include "software/test/common/sdl_sink.h"

#include <assert.h>
#include <SDL.h>
#include <SDL_version.h>
#include <stdlib.h>
#include <string.h>

#define FPS_WINDOW 15

struct SDLSink {
  SDL_Rect rect;
  SDL_Surface* screen;
  SDL_Overlay* overlay;
  u32 pic_num;
  u32 pic_times[FPS_WINDOW + 1];
};

static void DeInit(struct SDLSink* this) {
  if (this->overlay) SDL_FreeYUVOverlay(this->overlay);
}

static void ReInit(struct SDLSink* this) {
  u32 w = this->rect.w;
  u32 h = this->rect.h;
  this->screen = SDL_SetVideoMode(w, h, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
  this->overlay = SDL_CreateYUVOverlay(w, h, SDL_IYUV_OVERLAY, this->screen);
  assert(this->screen && this->overlay);
}

static void UpdateCaption(struct SDLSink* this) {
  char string[256];
  float fps;
  u32 w_begin;
  u32 w_len;
  u32 w_end = SDL_GetTicks();
  if (this->pic_num == 0) {
    w_begin = w_end;
    w_len = 1;
  } else if (this->pic_num < FPS_WINDOW) {
    w_begin = this->pic_times[0];
    w_len = this->pic_num;
  } else {
    u32 idx = (this->pic_num - FPS_WINDOW) % FPS_WINDOW;
    w_begin = this->pic_times[idx];
    w_len = FPS_WINDOW;
  }
  fps = (w_end - w_begin) / w_len;
  fps /= 1000;
  if (fps > 0.0) fps = 1 / fps;
  this->pic_times[this->pic_num % FPS_WINDOW] = w_end;
  sprintf(string, "PIC #%u - FPS %.1f", this->pic_num, fps);
  SDL_WM_SetCaption(string, string);
}

const void* SdlSinkOpen(const char* fname) {
  SDL_Init(SDL_INIT_VIDEO);
  return calloc(sizeof(struct SDLSink), 1);
}

void SdlSinkWrite(const void* inst, struct DecPicture pic) {
  struct SDLSink* this = (struct SDLSink*)inst;
  /* Displays full decoded image size, cropped output dimensions
   * are in pic.sequence_info.crop_params. */
  u32 w = pic.sequence_info.pic_width;
  u32 h = pic.sequence_info.pic_height;
  if (w != this->rect.w || h != this->rect.w) {
    DeInit(this);
    this->rect.w = w;
    this->rect.h = h;
    this->rect.x = 0;
    this->rect.y = 0;
    ReInit(this);
  }
  UpdateCaption(this);
  this->pic_num++;
  SDL_LockYUVOverlay(this->overlay);
  memcpy(this->overlay->pixels[0], pic.luma.virtual_address, w * h);
  /* round odd picture dimensions to next multiple of two for chroma */
  if (w & 1) w += 1;
  if (h & 1) h += 1;
  memcpy(this->overlay->pixels[1], pic.chroma.virtual_address, w * h / 4);
  memcpy(this->overlay->pixels[2],
         ((u8*)pic.chroma.virtual_address) + w * h / 4, w * h / 4);
  SDL_UnlockYUVOverlay(this->overlay);
  SDL_DisplayYUVOverlay(this->overlay, &this->rect);
  SDL_Flip(this->screen);
}

void SdlSinkClose(const void* inst) {
  struct SDLSink* this = (struct SDLSink*)inst;
  DeInit(this);
  free((void*)this);
  SDL_Quit();
}
