

#ifndef  __QCODECS_H__
#define  __QCODECS_H__

extern "C"
{

//#ifndef __STDC_CONSTANT_MACROS
//#define __STDC_CONSTANT_MACROS
//#endif

#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>

}
#include <qsdk/vplay.h>
#include <qsdk/audiobox.h>
#include <fr-user.h>
typedef enum {
    QCodecsWorkingModeEncode ,
    QCodecsWorkingModeDecode ,
    QCodecsWorkingModeResample
}QCodecsWorkingMode ;
class QCodecs{
    public :
        QCodecs();
        ~QCodecs();
        bool SetAudioConvertPara(audio_info_t  *in,audio_info_t *out,int inFrameSize);
        int  EncodeFrame(struct fr_buf_info *in ,struct fr_buf_info *out);
        int  DecodeFrame(struct fr_buf_info *in ,struct fr_buf_info *out);
        bool Prepare(void);
        bool ReSampleFrame(struct fr_buf_info *in, struct fr_buf_info *out);
        bool CheckWorkingModePara(QCodecsWorkingMode mode );
        void ShowSupportCodecsInfo(void);
        void ShowCodecInfo(AVCodec *codec);
    private :

        audio_info_t     inAudioInfo ;
        audio_info_t     outAudioInfo ;

        //AVFormatContext *pFormatContext  ;
        //AVOutputFormat *pOutputFormat ;
        enum  AVCodecID  codecID ;
        AVCodec *pCodec ;
    //    AVStream *pStream ;
        AVCodecContext *pCodecContext ;
        AVFrame  *pFrame ;
        //AVPacket *pkt ;

        SwrContext *swrContext ;
        QCodecsWorkingMode workingMode ;
        enum AVSampleFormat  inSampleFmt ;
        enum AVSampleFormat  outSampleFmt ;


        int inSampleFmtBits ;
        int outSampleFmtBits ;
        int inSampleFmtRawBits ;
        int outSampleFmtRawBits  ;
        int inChannelsLayout;
        int outChannelsLayout;
        int inLineSize ;
        int outLineSize ;
        int inNbSamples ;
        int outNbSamples ;
        int maxNbSamples ;

        int inFrameSize ;
        int outFrameSize ;

        uint8_t **inData ;
        uint8_t **outData ;
        bool codecCopyEnable ;


        char tmpFile[32];

        struct fr_buf_info resampleFr ;
        bool resampleEnable ;
        bool resampleMemoryReady ;

        bool ReSampleDeinit(void);
        bool EncoderDeinit(void);
        bool DecoderDeinit(void);

        bool EncoderInit(void);
        bool DecoderInit(void);
        bool ReSampleInit(void);

        int  InternalEncodeFrame(struct fr_buf_info *in ,struct fr_buf_info *out );



        bool CheckSampleRate(AVCodec *codec ,int sampleRate);
        bool CheckChannelLayout(AVCodec *codec ,uint64_t layout);
        bool CheckSampleFormat(AVCodec *codec, enum AVSampleFormat sampleFmt) ;

        char *resample24buf;



};
#endif
