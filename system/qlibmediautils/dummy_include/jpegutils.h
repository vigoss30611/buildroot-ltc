#ifndef JPEGUTILS_H
#define JPEGUTILS_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <assert.h>
#define SWSCALE_DUMMY_HANDLE(x)  if(1){ printf("dummy funcion ->%s -->%s", __func__, x);assert(1);}

#pragma message "-------->dummy swscale api"

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

int doCrop(char *srcImage, int srcW, int srcH, int srcPixelFormat, char *dstImage,int dstX, int dstY, int dstW, int dstH, int dstPixelFormat){
    SWSCALE_DUMMY_HANDLE("message");
    return 0;
}
int doSliceScale(char *srcImage, int srcW, int srcH, int srcPixelFormat, char *dstImage, int dstW, int dstH, int dstPixelFormat, int *scaledSize){
    SWSCALE_DUMMY_HANDLE("message");
    return 0;
}
int swenc_yuv2jpg(char *yuv_data,int width, int height, int pixel_format, char *out_file){
    SWSCALE_DUMMY_HANDLE("message");
    return 0;
}
#ifdef __cplusplus
}
#endif
#endif
