/* Copyright 2013 Google Inc. All Rights Reserved. */

#ifndef __VP9_OUTPUT_H__
#define __VP9_OUTPUT_H__

void Vp9PicToOutput(struct Vp9DecContainer *dec_cont);
void Vp9SetupPicToOutput(struct Vp9DecContainer *dec_cont);
i32 VP9SyncAndOutput(struct Vp9DecContainer *dec_cont);

#endif
