#include "jpegutils.h"

void swcrop_dummy(void){

}

int doCrop(char *srcImage, int srcW, int srcH, int srcPixelFormat, char *dstImage,int dstX, int dstY, int dstW, int dstH, int dstPixelFormat)
{
    char args[512];
    int ret = -1;
    AVFormatContext *fmt_ctx = NULL;
    AVFilterContext *buffersink_ctx = NULL;
    AVFilterContext *buffersrc_ctx = NULL;
    AVFilterGraph *filter_graph = NULL;
    AVFilter *buffersrc  = NULL;
    AVFilter *buffersink = NULL;
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    AVFrame *frame_in = av_frame_alloc();
    AVFrame *frame_out = av_frame_alloc();
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_NV12, AV_PIX_FMT_NONE };

    avfilter_register_all();
    buffersrc  = avfilter_get_by_name("buffer");
    buffersink = avfilter_get_by_name("buffersink");
    filter_graph = avfilter_graph_alloc();

    if(buffersrc == NULL || buffersink == NULL || outputs == NULL || inputs == NULL || frame_in == NULL || frame_out == NULL || filter_graph == NULL) {
        av_log(NULL, AV_LOG_ERROR, "Cannot alloc! buffersrc:0x%x buffersink:0x%x inputs:0x%x outputs:0x%x frame_in:0x%x frame_out:0x%x  filter_graph:0x%x\n",buffersrc, buffersink, inputs, outputs, frame_in, frame_out, filter_graph);
        goto end;
    }
    snprintf(args, sizeof(args),
        "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
        srcW,srcH,AV_PIX_FMT_NV12,
        1, 25,1,1);
    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
        goto end;
    }
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, NULL, filter_graph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
        goto end;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
        goto end;
    }

    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    snprintf(args, sizeof(args),
        "crop=%d:%d:%d:%d",
        dstW, dstH,dstX,dstY);
    if ((ret = avfilter_graph_parse_ptr(filter_graph, args,
                                    &inputs, &outputs, NULL)) < 0)
        goto end;
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        goto end;

    av_image_fill_arrays(frame_in->data, frame_in->linesize, srcImage, AV_PIX_FMT_NV12, srcW, srcH, 1);
    frame_in->data[0] = srcImage;
    frame_in->data[1] = srcImage + srcW*srcH;
    frame_in->width=srcW;
    frame_in->height=srcH;
    frame_in->format=AV_PIX_FMT_NV12;

    av_image_fill_arrays(frame_out->data, frame_out->linesize, dstImage, AV_PIX_FMT_NV12, dstW, dstH,1);

    if ((ret = av_buffersrc_add_frame(buffersrc_ctx, frame_in)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
        goto end;
    }

    if ((ret = av_buffersink_get_frame(buffersink_ctx, frame_out)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while get frame from the filtergraph\n");
        goto end;
    }

    if(frame_out->format == AV_PIX_FMT_NV12){
        int offset = 0;
        for(int i=0;i<frame_out->height;i++){
            memcpy(dstImage + offset, frame_out->data[0] + frame_out->linesize[0]*i, frame_out->width);
            offset += frame_out->width;
        }
        for(int i=0;i<frame_out->height/2;i++){
            memcpy(dstImage + offset, frame_out->data[1] + frame_out->linesize[1]*i, frame_out->width);
            offset += frame_out->width;
        }
    }

    #if (ENABLE_DEBUG_FILE)
    static int slice = 1;
    char filepath[50];
    snprintf(filepath, sizeof(filepath),
        "/output_%d.yuv",
        slice);
    slice++;
    FILE *fp_out=fopen(filepath,"wb+");
    fwrite(dstImage, 1, frame_out->width*frame_out->height*3/2,fp_out);
    fclose(fp_out);
    #endif

 end:
    if(frame_in)
        av_frame_free(&frame_in);
    if(frame_out)
        av_frame_free(&frame_out);
    if(filter_graph)
        avfilter_graph_free(&filter_graph);
    if(inputs)
        avfilter_inout_free(&inputs);
    if(outputs)
        avfilter_inout_free(&outputs);
    if(fmt_ctx)
        avformat_close_input(&fmt_ctx);
    return ret;
}
