#include "jpegutils.h"

int swenc_yuv2jpg(char *yuv_data,int width, int height, int pixel_format, char *out_file){
    AVFormatContext* pFormatCtx = NULL;
    AVOutputFormat* fmt = NULL;
    AVStream* video_st = NULL;
    AVCodecContext* pCodecCtx = NULL;
    AVCodec* pCodec = NULL;
    uint8_t* picture_buf = NULL;
    AVFrame* picture = NULL;
    AVPacket pkt;
    int lum_size = 0;
    int got_picture=0;
    int size = 0;
    int ret=0;
    int flag = 0;

    printf("enter yuv420_swenc_jpg\n");
    if(yuv_data == NULL || width <= 0 || height <= 0 || out_file == NULL) {
        printf("%s(%d),Error:args is wrong!\n",__FILE__, __LINE__);
        return -1;
    }

    //printf("AV_PIX_FMT_YUVJ420P:%d AV_PIX_FMT_NV12:%d pixel_format:%d\n", AV_PIX_FMT_YUVJ420P, AV_PIX_FMT_NV12, pixel_format);
    if (pixel_format != AV_PIX_FMT_YUVJ420P && pixel_format != AV_PIX_FMT_NV12) {
        printf("%s(%d),Error:pixel format do not support!\n",__FILE__, __LINE__);
        return -1;
    }
    if (pixel_format == AV_PIX_FMT_NV12) {
        pixel_format = AV_PIX_FMT_YUVJ420P;
        printf("Warning:change pixel format!\n");
        flag = 1;
    }
    av_register_all();

    //Method 1
    pFormatCtx = avformat_alloc_context();
    fmt = av_guess_format("mjpeg", NULL, NULL);
    if (fmt == NULL) {
        printf("%s(%d),Error:Can not find container!\n",__FILE__, __LINE__);
        ret = -1;
        goto err;
    }

    pFormatCtx->oformat = fmt;

    if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0){
        printf("%s(%d),Error:Couldn't open output file.\n",__FILE__, __LINE__);
        ret = -1;
        goto err;
    }

#if 0
    //Method 2. Guess format from out file
    avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
    fmt = pFormatCtx->oformat;
#endif

    video_st = avformat_new_stream(pFormatCtx, 0);
    if (video_st == NULL){
        printf("%s(%d),Error:Couldn't create stream.\n",__FILE__, __LINE__);
        ret = -1;
        goto err;
    }

    pCodecCtx = video_st->codec;
    pCodecCtx->codec_id = fmt->video_codec;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = pixel_format;
    pCodecCtx->width = width;
    pCodecCtx->height = height;
    video_st->time_base.num = 1;
    video_st->time_base.den = 25;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;

    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    if (!pCodec){
        printf("%s(%d),Error:Codec not found.\n",__FILE__, __LINE__);
        ret = -1;
        goto err;
    }
    if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0){
        printf("%s(%d),Error:Could not open codec.\n",__FILE__, __LINE__);
        ret = -1;
        goto err;
    }
    //Output some information
    av_dump_format(pFormatCtx, 0, out_file, 1);

    picture = av_frame_alloc();
    if (picture == NULL) {
        printf("%s(%d),Error:Alloc AVFrame fail.\n",__FILE__, __LINE__);
        ret = -1;
        goto err;
    }
    picture->format = pCodecCtx->pix_fmt;
    picture->width = pCodecCtx->width;
    picture->height = pCodecCtx->height;

    size = av_image_get_buffer_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, 16);

    picture_buf = (uint8_t *)av_malloc(size>>1);
    if (!picture_buf) {
        printf("%s(%d),Error:Could not allocate buffer.\n",__FILE__, __LINE__);
        ret = -1;
        goto err;
    }
    ret = av_image_fill_arrays(picture->data, picture->linesize, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, 16);
    if (ret < 0) {
        printf("%s(%d),Error:Could not allocate raw picture buffer.\n",__FILE__, __LINE__);
        ret = -1;
        goto err;
    }

    //printf("frame->linesize[0]:%d frame->linesize[1]:%d frame->linesize[2]:%d\n",picture->linesize[0], picture->linesize[1], picture->linesize[2]);
    lum_size = pCodecCtx->width*pCodecCtx->height;

    if (!flag) {
        picture->data[0] = yuv_data;
        picture->data[1] = yuv_data + lum_size;
        picture->data[2] = yuv_data + (lum_size*5>>2);
    } else {
        printf("translate pixformat!\n");
        //memcpy(picture_buf, yuv_data, lum_size); //Lum,just adjust UV oder
        int i,j,tmp_loop,tmp_size;
        tmp_loop = lum_size>>2;

        for (i=j=0;i<tmp_loop;) {
            *(picture_buf + i) =  *(yuv_data + lum_size + j);//U
            i++;
            j += 2;
        }

        tmp_size = lum_size>>2;
        for (i=j=0;i<tmp_loop;) {
            *(picture_buf + tmp_size + i) = *(yuv_data + lum_size + 1 + j);//V
            i++;
            j += 2;
        }
        picture->data[0] = yuv_data;
        picture->data[1] = picture_buf;
        picture->data[2] = picture_buf + (lum_size>>2);
    }

    #if 0
        FILE *fd = NULL;
        fd = fopen("/nfs/dump0.yuv", "wb+");
        fwrite(yuv_data, 1, lum_size*3/2, fd);
        fclose(fd);

        fd = fopen("/nfs/dump1.yuv", "wb+");
        fwrite(picture_buf, 1, lum_size*3/2, fd);
        fclose(fd);

    #endif
    //Write Header
    ret = avformat_write_header(pFormatCtx, NULL);

    if (ret < 0) {
        printf("%s(%d),Error:avformat_write_header fail!\n",__FILE__, __LINE__);
        goto err;
    }

    av_init_packet(&pkt);

    //Encode
    ret = avcodec_encode_video2(pCodecCtx, &pkt, picture, &got_picture);
    if(ret < 0){
        printf("%s(%d),Error:Encode Error.\n",__FILE__, __LINE__);
        av_packet_unref(&pkt);
        goto err;
    }
    if (got_picture==1){
        pkt.stream_index = video_st->index;
        pkt.dts = pkt.pts = 0;
        ret = av_write_frame(pFormatCtx, &pkt);
        if (ret < 0) {
            printf("%s(%d),Error:wirte frame error!\n",__FILE__, __LINE__);
            goto err;
        }
    }
    //Write Trailer
    av_write_trailer(pFormatCtx);
    av_packet_unref(&pkt);
    printf("Encode one Jpeg picture!\n");
err:
    if (video_st)
        avcodec_close(video_st->codec);
    if (picture)
        av_free(picture);
    if (picture_buf)
        av_free(picture_buf);
    if (pFormatCtx->pb)
        avio_close(pFormatCtx->pb);
    if (pFormatCtx)
        avformat_free_context(pFormatCtx);
    printf("ret:%d\n",ret);
    return ret;
}
