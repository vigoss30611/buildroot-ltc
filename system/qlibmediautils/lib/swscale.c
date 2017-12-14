#include "jpegutils.h"

int doSliceScale(char *srcImage, int srcW, int srcH, int srcPixelFormat, char *dstImage, int dstW, int dstH, int dstPixelFormat, int *scaledSize) {

     struct SwsContext *scaleContext = NULL;

	 if (srcImage == NULL || dstImage == NULL) {
	 	 printf("%s:input args error!\n",__FUNCTION__);
         return -1;
	 }

     #if TIMEUSAGE
     struct timeval tmStart, tmEnd, tmScale;
	 struct timeval tmTmp_S, tmTmp_E;
	 tmScale.tv_sec = tmScale.tv_usec = 0;
	 gettimeofday(&tmStart, NULL);
	 #endif

	 enum AVPixelFormat src_pixfmt = AV_PIX_FMT_NV12;
	 int src_w = srcW;
	 int src_h = srcH;
	 enum AVPixelFormat dst_pixfmt = AV_PIX_FMT_NV12;
	 int dst_w = dstW;
	 int dst_h = dstH;

	 //printf("enter doSliceScale!\n");
	 int src_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(src_pixfmt));
	 int dst_bpp = av_get_bits_per_pixel(av_pix_fmt_desc_get(dst_pixfmt));

	 uint8_t *src_data[4];
	 int src_linesize[4];

	 uint8_t *dst_data[4];
	 int dst_linesize[4];

	 int scaleInterpolation = SWS_POINT;
	 int ret = 0;
	 ret = av_image_alloc(src_data, src_linesize, src_w, src_h, src_pixfmt, 1);

	 if (ret < 0) {
	 	 printf("src_w:%d src_h:%d size:%d\n",src_w, src_h, src_w*src_h*3/2);
         printf("error:alloc input image space failed!\n");
		 return -1;
	 }

	 ret = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pixfmt, 1);

	 if (ret < 0) {
	 	 printf("dst_w:%d dst_h:%d size:%d\n",dst_w, dst_h, dst_w*dst_h*3/2);
         printf("error:alloc output image space failed!\n");
		 av_freep(&src_data[0]);
		 return -1;
	 }

	 scaleContext = sws_getContext(src_w, src_h, AV_PIX_FMT_NV12,
	                               dst_w, dst_h, AV_PIX_FMT_NV12, SWS_POINT,
	                               NULL, NULL, NULL);

     switch (src_pixfmt) {
	 	case AV_PIX_FMT_NV12:
	     memcpy(src_data[0], srcImage, src_w*src_h);					 //Y
	     memcpy(src_data[1], srcImage+src_w*src_h, src_w*src_h/2); 	    //UV
	     break;
		default:
			break;
     }
#if TIMEUSAGE
     gettimeofday(&tmTmp_S, NULL);
#endif
	 sws_scale(scaleContext, (const uint8_t * const *)src_data, src_linesize, 0, src_h, dst_data, dst_linesize);
#if TIMEUSAGE
	 gettimeofday(&tmTmp_E, NULL);
	 tmScale.tv_sec =  tmTmp_E.tv_sec - tmTmp_S.tv_sec;
	 tmScale.tv_usec = tmTmp_E.tv_usec - tmTmp_S.tv_usec;
#endif

	 switch(dst_pixfmt) {
	 	case AV_PIX_FMT_NV12:
            memcpy(dstImage, dst_data[0], dst_w*dst_h);
			memcpy(dstImage + dst_w*dst_h, dst_data[1], dst_w*dst_h/2);
			break;
		default:
			break;
	 }

    sws_freeContext(scaleContext);
    av_freep(&src_data[0]);
	av_freep(&dst_data[0]);

#if TIMEUSAGE
	gettimeofday(&tmEnd, NULL);
	float total = tmEnd.tv_sec - tmStart.tv_sec + (tmEnd.tv_usec - tmStart.tv_usec)/(1000000.0);
	float swScale = tmScale.tv_sec + tmScale.tv_usec/(1000000.0);

    printf("Slice Scale time usage:\n");
	printf("                       total:%f swScale:%f\n",total, swScale);
	printf("                                 swScale:%f other:%f\n",swScale/total, (total - swScale)/total);
#endif
	return 0;
}

