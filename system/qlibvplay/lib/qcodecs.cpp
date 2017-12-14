#include <qcodecs.h>
#include <qsdk/vplay.h>
#include <assert.h>
#include <unistd.h>
#include <define.h>

QCodecs::QCodecs(){
    av_register_all();
    //init basic value

    this->pCodec =  NULL;
    this->pCodecContext =  NULL ;

    this->swrContext = NULL;

    this->resampleEnable = false ;
    this->resampleMemoryReady =  false ;
    this->resampleFr.virt_addr =  NULL;
    memset(&this->resampleFr ,0, sizeof(struct fr_buf_info));
}

QCodecs::~QCodecs(){
    //TODO ::release resource
    //
    if(this->pFrame){
        av_frame_free(&this->pFrame);
    }
    if(this->pCodecContext ){
        //TOTO :free pCodecContext
        avcodec_close(this->pCodecContext);
        av_free(this->pCodecContext);
    }
    if(this->resampleEnable){
        if(this->swrContext ){
            //TODO: free swrContext
            swr_free(&this->swrContext);
        }
    }
    if(this->resampleFr.virt_addr ){
        free(this->resampleFr.virt_addr);
    }
}
bool QCodecs::SetAudioConvertPara(audio_info_t  *in,audio_info_t *out,int frameSize){
    LOGGER_TRC("audio in  info -> %d %d %d %d %d\n",\
            in->type,\
            in->effect,\
            in->channels,\
            in->sample_rate,\
            in->sample_fmt
            );
    LOGGER_TRC("audio out info -> %d %d %d %d %d\n",\
            out->type,\
            out->effect,\
            out->channels,\
            out->sample_rate,\
            out->sample_fmt
            );
    LOGGER_TRC("frame size ->%d\n",frameSize);
    if (in->type < AUDIO_CODEC_TYPE_PCM || in->type > AUDIO_CODEC_TYPE_MAX )
    {
        LOGGER_ERR("in codec type error ->%d\n", in->type);
    }
    if (out->type < AUDIO_CODEC_TYPE_PCM || out->type > AUDIO_CODEC_TYPE_MAX )
    {
        LOGGER_ERR("out codec type error ->%d\n",out->type);
    }
    this->codecCopyEnable = false ;
    switch( out->type ){
        case AUDIO_CODEC_TYPE_PCM :
            switch(out->sample_fmt) {
                case   AUDIO_SAMPLE_FMT_S16:
                    this->codecID =  AV_CODEC_ID_PCM_S16LE;
                    this->codecCopyEnable = true ;
                    break ;
                case   AUDIO_SAMPLE_FMT_S32:
                    this->codecID =  AV_CODEC_ID_PCM_S32LE;
                    this->codecCopyEnable = true ;
                    break ;
                default :
                    LOGGER_ERR("pcm sample fmt not support:%d\n", out->sample_fmt);
                    return false ;
            }
            break;
        case AUDIO_CODEC_TYPE_G711U :
            this->codecID =  AV_CODEC_ID_PCM_MULAW;
            break;
        case AUDIO_CODEC_TYPE_G711A :
            this->codecID =  AV_CODEC_ID_PCM_ALAW;
            break;
        case AUDIO_CODEC_TYPE_AAC :
            this->codecID =  AV_CODEC_ID_AAC;
            break;
        case AUDIO_CODEC_TYPE_ADPCM :
        case AUDIO_CODEC_TYPE_G726 :
        case AUDIO_CODEC_TYPE_MP3 :
        case AUDIO_CODEC_TYPE_SPEEX :
            this->codecID =  AV_CODEC_ID_NONE;
            LOGGER_ERR("not support codec type now ->%d\n",in->type);
            return false ;
        default :
            LOGGER_ERR("not support codec type now ->%d\n",in->type);
            return false ;
    };
    //TODO :: add arg check function
    //
    if( in->sample_fmt <= AUDIO_SAMPLE_FMT_NONE || in->sample_fmt >= AUDIO_SAMPLE_FMT_MAX ){
        LOGGER_ERR("in auido sample fmt:%d error\n", in->sample_fmt);
        return false ;
    }
    if( out->sample_fmt <= AUDIO_SAMPLE_FMT_NONE || out->sample_fmt >= AUDIO_SAMPLE_FMT_MAX ){
        LOGGER_ERR("out auido sample fmt:%d error\n", out->sample_fmt);
        return false ;
    }

    if (in->channels == 1 ){
        inChannelsLayout =  AV_CH_LAYOUT_MONO ;
    }
    else if (in->channels == 2){

        inChannelsLayout =  AV_CH_LAYOUT_STEREO ;
    }
    else {
        printf("in not support more channels->%d\n",in->channels);
        return false ;
    }
    if (out->channels == 1 ){
        outChannelsLayout =  AV_CH_LAYOUT_MONO ;
    }
    else if (out->channels == 2){

        outChannelsLayout =  AV_CH_LAYOUT_STEREO ;
    }
    else {
        //TODO : may be add more to support
        LOGGER_ERR("out not support more channels->%d\n",out->channels);
        return false ;
    }
    switch(in->sample_fmt){
        case AUDIO_SAMPLE_FMT_S16 :
            inSampleFmt = AV_SAMPLE_FMT_S16 ;
            inSampleFmtBits = 2 ;
            inSampleFmtRawBits = 2 ;
            break;
        case AUDIO_SAMPLE_FMT_S24 :
            //TODO :: support 24
            inSampleFmt = AV_SAMPLE_FMT_S32 ;
            inSampleFmtBits = 3 ;
            inSampleFmtRawBits = 4 ;
            break;
        case AUDIO_SAMPLE_FMT_S32 :
            inSampleFmt = AV_SAMPLE_FMT_S32 ;
            inSampleFmtBits = 4 ;
            inSampleFmtRawBits = 4 ;
            break;
        case AUDIO_SAMPLE_FMT_FLTP :
            inSampleFmt = AV_SAMPLE_FMT_FLTP ;
            inSampleFmtBits = 4 ;
            inSampleFmtRawBits = 4 ;
            break;
        case AUDIO_SAMPLE_FMT_DBLP :
            inSampleFmt = AV_SAMPLE_FMT_DBLP ;
            inSampleFmtBits = 4 ;
            inSampleFmtRawBits = 4 ;
            break;
        default :
            printf("in not support the sample format->%d\n",inSampleFmt);
            return false ;
    }
    switch(out->sample_fmt){
        case AUDIO_SAMPLE_FMT_S16 :
            outSampleFmt = AV_SAMPLE_FMT_S16 ;
            outSampleFmtBits = 2 ;
            outSampleFmtRawBits = 2 ;
            break;
        case AUDIO_SAMPLE_FMT_S24 :
            //TODO :: support 24
            outSampleFmt = AV_SAMPLE_FMT_S32 ;
            outSampleFmtBits = 3 ;
            outSampleFmtRawBits = 4 ;
            break;
        case AUDIO_SAMPLE_FMT_S32 :
            outSampleFmt = AV_SAMPLE_FMT_S32 ;
            outSampleFmtBits = 4 ;
            outSampleFmtRawBits = 4 ;
            break;
        case AUDIO_SAMPLE_FMT_FLTP :
            outSampleFmt = AV_SAMPLE_FMT_FLTP ;
            outSampleFmtBits = 8 ;
            outSampleFmtRawBits = 8 ;
            break;
        case AUDIO_SAMPLE_FMT_DBLP :
            outSampleFmt = AV_SAMPLE_FMT_DBLP ;
            outSampleFmtBits = 8 ;
            outSampleFmtRawBits = 8 ;
            break;
        default :
            printf("out not support the sample format->%d\n",inSampleFmt);
            return false ;
    }
    if (in->channels != out->channels ) {
        LOGGER_DBG("enable sw resample on channels ->%d:%d\n",in->channels,out->channels);
        this->resampleEnable = true ;
    }
    if (in->sample_rate != out->sample_rate ){
        LOGGER_DBG("enable sw resample bit rate ->%d :%d\n",in->sample_rate,out->sample_rate);
        this->resampleEnable = true ;
    }
    if(in->sample_fmt != out->sample_fmt ){
        LOGGER_DBG("enable sw resample sample fmt ->%d:%d\n",in->sample_fmt ,out->sample_fmt);
        this->resampleEnable = true ;
    }
    this->inAudioInfo = *in;
    this->outAudioInfo = *out;

    this->inFrameSize = frameSize ;




    return true ;
}

bool QCodecs::Prepare(void){
    switch(this->workingMode) {
        case QCodecsWorkingModeEncode :
            if(this->resampleEnable ) {
                if( this->ReSampleInit() == false){
                    LOGGER_ERR("init resample error\n");
                    return false ;
                }
            }
#if 1
            if( this->EncoderInit() == false ){
                LOGGER_ERR("init encoder error\n");
                return false ;
            }
#endif
            break;
        case QCodecsWorkingModeDecode :
            if( this->DecoderInit() == false ){
                return false ;
            }
            if(this->resampleEnable )
                if( this->ReSampleInit() == false){
                    return false ;
                }
            break;
        case QCodecsWorkingModeResample :
            if( this->ReSampleInit() == false){
                return false ;
            }
            break;
        default :
            LOGGER_ERR("not support workingMode \n");
            return false ;

    }
    return true;
}
bool QCodecs::ReSampleFrame(struct fr_buf_info *in, struct fr_buf_info *out){
//    printf("-->%s\n",__FUNCTION__);
#if 1
    int ret = 0;
    int outDataSize = 0;
    //printf("in audio -> %d\n",this->inAudioInfo.sample_rate);
    int dst_nb_samples = av_rescale_rnd(swr_get_delay(this->swrContext, this->inAudioInfo.sample_rate) +
            this->inNbSamples, this->outAudioInfo.sample_rate, this->inAudioInfo.sample_rate, AV_ROUND_UP);
    if (dst_nb_samples > this->maxNbSamples) {
#if 0
        av_freep(&dst_data[0]);
        ret = av_samples_alloc(dst_data, &dst_linesize, dst_nb_channels,
                dst_nb_samples, dst_sample_fmt, 1);
        if (ret < 0)
            break;
        max_dst_nb_samples = dst_nb_samples;
#endif
    }
/*
    printf("in  ->%p  %d %d\n",in->virt_addr,in->size,in->mapped_size);
    printf("swr ->%p %p  %d %p %d\n",this->swrContext,*this->outData ,this->outNbSamples,*this->inData ,this->inNbSamples);
    */
    if (  this->inAudioInfo.sample_fmt == AUDIO_SAMPLE_FMT_S24  ){
        if(this->resampleFr.virt_addr == NULL){
            this->resampleFr.virt_addr =  malloc(in->size + 1024);
        }
//        memcpy(this->resampleFr.virt_addr,in->virt_addr,in.size);
        this->resampleFr.mapped_size =  in->size+1024;
        this->resampleFr.size = in->size ;
        this->resampleFr.timestamp =  in->timestamp;
        uint32_t *p = (uint32_t *) this->resampleFr.virt_addr   ;
        uint32_t *q = (uint32_t *) in->virt_addr ;
    //    printf("--->resample in 24 audio ->%d %p\n",in->size,p);
        for(int i = 0 ;i < in->size/4 ;i++ ){
            p[i] = q[i] << 8 ;
        }
    }
    //save_tmp_file(3,in->virt_addr,in->size);
    *this->inData = (uint8_t *)this->resampleFr.virt_addr ;
//    printf("swr ->%p %p  %d %p %d\n",this->swrContext,*this->outData ,this->outNbSamples,*this->inData ,this->inNbSamples);
    ret = swr_convert(this->swrContext, (uint8_t **)this->outData, this->outNbSamples,
            (const uint8_t **)this->inData, this->inNbSamples);
    if (ret <= 0)
    {
        LOGGER_ERR("swr_convert error, ret:%d \n", ret);
        return -1;
    }
    outDataSize = av_samples_get_buffer_size(&this->outLineSize, this->outAudioInfo.channels,
            ret, (AVSampleFormat)this->outSampleFmt, 1);
    if (outDataSize <= 0)
    {
        printf("av_samples_get_buffer_size error \n");
        return -1;
    }
    //printf("datasize -> %d : %d\n",in->size ,outDataSize);
    out->virt_addr =  *this->outData ;
    out->size = outDataSize ;
    if (  this->outAudioInfo.sample_fmt == AUDIO_SAMPLE_FMT_S24  ){
        printf("--->resample out 24  audio\n");
        uint32_t *p = (uint32_t *) out->virt_addr ;
        for(int i = 0 ;i < out->size/4 ;i++ ){
            p[i] = p[i] >> 8 ;
        }
    }
#endif

    return true ;
}
extern void save_tmp_file(int index ,void *buf,int size );
int QCodecs::EncodeFrame(struct fr_buf_info *in ,struct fr_buf_info *out){
    if ( this->resampleEnable ){
        struct fr_buf_info resampledData ;
//        save_tmp_file(0 ,in->virt_addr ,in->size);
        if(this->ReSampleFrame(in,&resampledData) == false ){
            LOGGER_ERR("resame frame error\n");
            return false ;
        }
//        save_tmp_file(1 ,resampledData.virt_addr ,resampledData.size);
        if(this->codecCopyEnable ){
            memcpy(out->virt_addr ,resampledData.virt_addr ,resampledData.size);
            out->size = resampledData.size ;
            out->timestamp =  in->timestamp ;
        }
        else {
            this->InternalEncodeFrame(&resampledData,out);
            out->timestamp =  in->timestamp ;
        }
    }
    else {
            this->InternalEncodeFrame(in,out);
            out->timestamp =  in->timestamp ;
    }

    return 0;
}

int QCodecs::InternalEncodeFrame(struct fr_buf_info *in ,struct fr_buf_info *out ){
    AVPacket pkt ;
    int ret = 0 ;
    int got_output = 0;
    out->size  = 0;
    int sampleBytes =  av_get_bytes_per_sample(this->outSampleFmt)* this->pCodecContext->channels ;
    this->pFrame->nb_samples =  in->size / sampleBytes ;
//    printf("sample bytes ->%d %d %d\n",in->size ,sampleBytes,this->pFrame->nb_samples);
    //int needed_size = av_samples_get_buffer_size(NULL, this->outAudioInfo.channels,
      //                                       this->pFrame->nb_samples, this->outSampleFmt,
       //                                       0);
//    printf("needed size ->%d\n",needed_size);
    ret = avcodec_fill_audio_frame(this->pFrame, this->outAudioInfo.channels, this->outSampleFmt,
                                   (const uint8_t*)in->virt_addr, in->size, 4);
    if (ret < 0) {
        LOGGER_ERR("Could not setup audio frame ->%d %d %d %p %d %d\n",this->outAudioInfo.channels,this->outSampleFmt,sampleBytes,out->virt_addr,in->size,ret);
        return false ;
    }
    this->pFrame->channel_layout =  this->pCodecContext->channel_layout ;
    this->pFrame->channels =  this->pCodecContext->channels ;
    this->pFrame->sample_rate =  this->pCodecContext->sample_rate ;
    av_init_packet(&pkt);
    pkt.data = NULL; // packet data will be allocated by the encoder
    pkt.size = 0;


    /* encode the samples */
    ret = avcodec_encode_audio2(this->pCodecContext, &pkt, this->pFrame, &got_output);
    if (ret < 0) {
        LOGGER_ERR("Error encoding audio frame, ret:%d\n", ret);
        exit(1);
    }
    //printf("encode ok -> %d  ->%d\n",pkt.size,got_output);
    if (got_output) {
        out->size = pkt.size ;
        if(pkt.size ){
            memcpy(out->virt_addr,pkt.data ,pkt.size);
            //save_tmp_file(2,pkt.data,  pkt.size);
        }
        av_packet_unref(&pkt);
    }

/* get the delayed frames */
    for (got_output = 1; got_output; ) {
        ret = avcodec_encode_audio2(this->pCodecContext, &pkt, NULL, &got_output);
        if (ret < 0) {
            LOGGER_ERR("Error encoding frame, ret:%d\n", ret);
            exit(1);
        }
    //    printf("encode ok 2 -> %d  ->%d\n",pkt.size,got_output);

        if (got_output) {
            if(pkt.size ){
                memcpy(( (char *)out->virt_addr) + out->size,pkt.data ,pkt.size);
                //save_tmp_file(3,pkt.data,  pkt.size);
            }
            av_packet_unref(&pkt);
        }
    }
    return 0;
}
int QCodecs::DecodeFrame(struct fr_buf_info *in ,struct fr_buf_info *out){

    return 0;
}
bool QCodecs::ReSampleInit(void){
    audio_info_t *in  = &this->inAudioInfo ;
    audio_info_t *out = &this->outAudioInfo ;
    if(this->swrContext != NULL) {
        LOGGER_DBG("qcodecs init already ,will free last one\n");
        swr_free(&this->swrContext);
    }

    this->swrContext =  swr_alloc();
    if(this->swrContext == NULL){
        printf("malloc swr context error\n");
        return false ;
    }
//    printf("set sw options\n");
    av_opt_set_int(this->swrContext,"in_channel_layout", inChannelsLayout  ,  0);
    av_opt_set_int(this->swrContext,"out_channel_layout", outChannelsLayout  ,  0);

    av_opt_set_int(this->swrContext,"in_sample_rate",  in->sample_rate ,  0);
    av_opt_set_int(this->swrContext,"out_sample_rate", out->sample_rate  ,  0);
    av_opt_set_sample_fmt(this->swrContext,"in_sample_fmt", inSampleFmt  ,  0);
    av_opt_set_sample_fmt(this->swrContext,"out_sample_fmt", outSampleFmt   ,  0);
#if 1
    if(in->sample_fmt == AUDIO_SAMPLE_FMT_S24 ){
        av_opt_set_int(this->swrContext ,"in_sample_bits" , 3 ,0 );
    }
    if(out->sample_fmt == AUDIO_SAMPLE_FMT_S24 ){
        av_opt_set_int(this->swrContext ,"out_sample_bits" , 3 ,0 );
    }
#endif
    swr_init(this->swrContext);

    int ret = 0 ;
       this->inNbSamples =  this->inFrameSize / (  this->inSampleFmtRawBits * this->inAudioInfo.channels );
//    printf("NB Samples ->%d %d %d %d \n",this->inNbSamples,this->inFrameSize,this->inSampleFmtRawBits,this->inAudioInfo.channels);
    ret = av_samples_alloc_array_and_samples((uint8_t ***)&this->inData, &this->inLineSize, this->inAudioInfo.channels,
                                             this->inNbSamples, this->inSampleFmt, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate source samples\n");
        return false ;
    }
    this->maxNbSamples = this->outNbSamples =  av_rescale_rnd(this->inNbSamples,this->outAudioInfo.sample_rate, this->inAudioInfo.sample_rate, AV_ROUND_UP);
//    printf("NB Samples ->%d :%d \n",this->inNbSamples,this->outNbSamples);
//    printf("out fmt ->%d : %d\n",this->inSampleFmt,this->outSampleFmt);
    ret = av_samples_alloc_array_and_samples((uint8_t *** )&this->outData, &this->outLineSize, this->outAudioInfo.channels ,
        this->outNbSamples, (AVSampleFormat)this->outSampleFmt, 0);
    if (ret < 0)
    {
        LOGGER_ERR("av_samples_alloc_array_and_samples error, ret:%d \n", ret);
        return false;
    }
    int inDataSize = 0 ;
    inDataSize = av_samples_get_buffer_size(NULL, this->inAudioInfo.channels,
        this->inNbSamples,
        this->inSampleFmt, 1);
    if (inDataSize <= 0)
    {
        LOGGER_ERR("av_samples_get_buffer_size error %d \n", inDataSize);
        return false ;
    }
//    printf("in data size -> %d %d\n",this->inFrameSize,inDataSize);
//    printf("linesize ->%d:%d\n",this->inLineSize,this->outLineSize);
    LOGGER_INFO("init resample ok \n");
//    printf("double size->%d\n",sizeof(double) );
    return true ;
}
bool QCodecs::ReSampleDeinit(void){
    return false;
}

bool QCodecs::CheckWorkingModePara(QCodecsWorkingMode mode ){
    //TODO :: add para check
    this->workingMode = mode;
    if (mode == QCodecsWorkingModeEncode ){
        if (this->inAudioInfo.type !=     AUDIO_CODEC_TYPE_PCM ){
            LOGGER_ERR("encode input format(%d) must be pcm\n", this->inAudioInfo.type);
            return false ;
        }

        return true ;
    }
    else if(mode == QCodecsWorkingModeDecode ){
        if (this->outAudioInfo.type !=     AUDIO_CODEC_TYPE_PCM ){
            LOGGER_ERR("encode output format(%d) must be pcm\n", this->outAudioInfo.type);
            return false ;
        }
        return true ;
    }
    else if(mode == QCodecsWorkingModeResample ){
        return true ;
    }
    return false ;
}
bool QCodecs::EncoderDeinit(void){
    return true ;
}
bool QCodecs::DecoderDeinit(void){
    return true ;
}

bool QCodecs::EncoderInit(void){
    if(this->codecCopyEnable ){
        LOGGER_DBG("raw pcm encode ,just copy");
        return true ;
    }
#if 0
#if 0
    this->pFormatContext =  avformat_alloc_context();
    this->pFormat = av_guess_format(NULL, this->tmpFile, NULL);

    this->pFormatContext->oformat = this->pOutputFormat ;
#else
    avformat_alloc_output_context2(&this->pFormatContext, NULL, NULL, this->tmpFile);
    this->pOutputFormat =  this->pFormatContext->oformat ;
#endif
    if (avio_open(&this->pFormatContext->pb,this->tmpFile, AVIO_FLAG_READ_WRITE) < 0)
    {
        printf("open output file ok\n");
        return false;
    }
#endif
    //try get codec point
    this->pCodec = avcodec_find_encoder(this->codecID);
    if (this->pCodec == NULL) {
        LOGGER_ERR("can not find ->%s\n",avcodec_get_name(this->codecID));
        return false ;
    }
    else {
        //LOGGER_TRC("find ->%s ->%p\n",avcodec_get_name(this->codecID),this->pCodec);
    }
    //get codec context
#if 0
    this->pStream = avformat_new_stream( this->pFormatContext , this->pCodec );
    this->pCodecContext =  this->pStream->codec;
    av_dump_format(this->pFormatContext,0,this->tmpFile,1);
#else
    this->pCodecContext = avcodec_alloc_context3(this->pCodec);
#endif

    this->pCodecContext->bit_rate = this->outAudioInfo.sample_rate;

    /* check that the encoder supports s16 pcm input */
    if (!this->CheckSampleFormat(this->pCodec, this->outSampleFmt)) {
        LOGGER_ERR("Encoder does not support sample format %s",
                av_get_sample_fmt_name(this->outSampleFmt));
        return false ;
    }
    else {
        this->pCodecContext->sample_fmt = outSampleFmt;
    }

    /* select other audio parameters supported by the encoder */
    if(this->CheckSampleRate( this->pCodec,this->outAudioInfo.sample_rate )  ) {
        this->pCodecContext->sample_rate  = this->outAudioInfo.sample_rate;
    }
    else {
        LOGGER_ERR("target sample rate not support\n");
        return false ;
    }
    this->pCodecContext->channel_layout = this->outChannelsLayout;
    this->pCodecContext->channels       = av_get_channel_layout_nb_channels(this->pCodecContext->channel_layout);

    /* open it */
    if (avcodec_open2(this->pCodecContext, this->pCodec, NULL) < 0) {
        LOGGER_ERR( "Could not open codec\n");
        return false  ;
    }
//    LOGGER_INFO("open codec handler ok \n");

    this->pFrame = av_frame_alloc();
    if (!this->pFrame) {
        fprintf(stderr, "Could not allocate audio frame\n");
        exit(1);
    }
    //this->pFrame->nb_samples     = this->pCodecContext->frame_size;
    this->pFrame->nb_samples     = 1000;
    this->pFrame->format         = this->pCodecContext->sample_fmt;
    this->pFrame->channel_layout = this->pCodecContext->channel_layout;
    this->pCodecContext->frame_size = 1000;

    /* the codec gives us the frame size, in samples,
     * we calculate the size of the samples buffer in bytes */
#if 1
//    printf("-->%d %d %d\n",this->outAudioInfo.channels,this->pCodecContext->frame_size,this->outSampleFmt);
    int buffer_size = av_samples_get_buffer_size(NULL, this->outAudioInfo.channels, this->pCodecContext->frame_size,
                                             this->outSampleFmt, 0);
    if (buffer_size < 0) {
        fprintf(stderr, "Could not get sample buffer size\n");
        exit(1);
    }
    //LOGGER_TRC("avframe size ->%d\n",buffer_size);
#if 0
    void *samples = av_malloc(buffer_size);
    if (!samples) {
        fprintf(stderr, "Could not allocate %d bytes for samples buffer\n",
                buffer_size);
        exit(1);
    }
#endif
    /* setup the data pointers in the AVFrame */
#endif
    return true ;
}
bool QCodecs::DecoderInit(void){
    return true;
}
bool QCodecs::CheckSampleFormat(AVCodec *codec, enum AVSampleFormat sampleFmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sampleFmt)
            return true;
        p++;
    }
    return false;
}

/* just pick the highest supported samplerate */
bool QCodecs::CheckSampleRate(AVCodec *codec ,int sampleRate)
{
    const int *p;

    if (!codec->supported_samplerates)
        return true;

    p = codec->supported_samplerates;
    while (*p) {
           if (sampleRate == *p){
            return true ;
        }
        p++;
    }
    return false ;
}

/* select layout with the highest channel count */
bool  QCodecs::CheckChannelLayout(AVCodec *codec ,uint64_t layout)
{
    const uint64_t *p;

    if (!codec->channel_layouts)
        return true;

    p = codec->channel_layouts;
    while (*p) {
        if(*p == layout){
            return true ;
        }
        p++;
    }
    return false;
}
void QCodecs::ShowCodecInfo(AVCodec *codec){
    LOGGER_DBG("codecs ->%s\n",codec->name);
}
void QCodecs::ShowSupportCodecsInfo(void){

    int id = AV_CODEC_ID_FIRST_AUDIO ;
    int endId = AV_CODEC_ID_FIRST_SUBTITLE ;
    AVCodec *codec = NULL;
    printf("support encode codec ->\n");
    for( ;id < endId;id++) {
        codec = avcodec_find_encoder((enum AVCodecID )id);
        if (codec == NULL) {
            continue ;
        }
        else {
            this->ShowCodecInfo(codec);
        }
    }
    printf("---------------------\n");
    printf("support decode codec ->\n");
    for( ;id < endId;id++) {
        codec = avcodec_find_decoder((enum AVCodecID )id);
        if (codec == NULL) {
            continue ;
        }
        else {
            this->ShowCodecInfo(codec);
        }
    }
    printf("---------------------\n");
}


