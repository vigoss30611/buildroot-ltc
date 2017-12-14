#ifndef JPEGUTILS_H
#define JPEGUTILS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>

#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/avutil.h>

#include <sys/time.h>
#define TIMEUSAGE 0
#define ENABLE_DEBUG_FILE 0
#include "libavfilter/avfilter.h"

int doCrop(char *srcImage, int srcW, int srcH, int srcPixelFormat, char *dstImage,int dstX, int dstY, int dstW, int dstH, int dstPixelFormat);
int doSliceScale(char *srcImage, int srcW, int srcH, int srcPixelFormat, char *dstImage, int dstW, int dstH, int dstPixelFormat, int *scaledSize);
/*
  funciton: support software encode YUV raw data to Jpeg.
  input:
        yuv_data: pointer to yuv raw data.
        width: yuv_data width
        height: yuv_data height
        pixel_format: yuv_data pixel format.Just support:YUV420P(12) & NV12(25)
  output:
        out_file: output jpeg file path.
  return:
        < 0:means fail
        >=0:means sucess

   note:If pixel format is YUV420P,speed will good.NV12 format will transfer to YUV420P
*/
int swenc_yuv2jpg(char *yuv_data,int width, int height, int pixel_format, char *out_file);

#ifdef __cplusplus
}
#endif
#endif
