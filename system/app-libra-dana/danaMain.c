#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <signal.h>
#include <setjmp.h>
#include <fcntl.h>

#include "danavideo.h"
#include "danavideo_cmd.h"
#include "debug.h"
#include "mediatyp.h"
#include "errdefs.h"
#include "basetype.h"

#include "alarm_common.h"
#include "alarm_db_client.h"

#include "av_audio.h"
#include "cfg_shm.h"
#include "av_shm.h"
#include "ipc_shm.h"

#include "file_dir.h"
#include "avi_demux.h"
#include "wifi_operation.h"
#include "av_osa_ipc.h"
#include <qsdk/videobox.h>
#include <qsdk/audiobox.h>
#include <qsdk/codecs.h>

#define dbg(fmt, x...) printf("---->[dana] %s::%d\t"fmt, __func__, __LINE__, ##x);
#define ASSERT(x) do { if (!(x)) {  fprintf(stderr,(char*)"-----DANA_ASSERT: %s:%d----\n", __FILE__, __LINE__); \
                                  assert((x)); }}while(0)

jmp_buf jmpbuf;

static char playbackRecFile[256] = {0};
static volatile int gIsInPreview = 0;
static T_VIDEO_STREAM_ID g_StreamType = E_STREAM_ID_PREVIEW;

//#define PLAYBACK_RECODE 1
#define CHAN_FRONT 1
#define CHAN_REAR  2
#define CHAN_LOCK  3
#define CHAN_PIC   4
#define WIFI_CHN "encss0-stream"
#define VIDEO_BUF_SIZE 1024*600

#ifdef PLAYBACK_RECODE
#define CASE_PREVIEW  0
#define CASE_PLAYBACK 1
static int g_CaseType =CASE_PREVIEW;
#endif
void exit_sig(int32_t sig) 
{
    static int32_t called = 0;
    
    dbg("\x1b[31mtestdanavideo Leaving...\x1b[0m\n");

    if (called) return; else called = 1;
    
    longjmp(jmpbuf, 1);  
  
}

DWORD danale_gettickcount()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts); // CLOCK_MONOTONIC
	return 1000*ts.tv_sec + ts.tv_nsec/1000000;
}

typedef struct _MyData {
    pdana_video_conn_t *danavideoconn;


    volatile bool run_media;
    volatile bool exit_media;
    pthread_t thread_media;

    volatile bool run_talkback;
    volatile bool exit_talback;
    pthread_t thread_talkback;

    volatile bool run_audio_media;
    volatile bool exit_audio_media;
    pthread_t thread_audio_media;


    uint32_t chan_no;

    char appname[32]; 

} MYDATA;        

void* th_talkback(void *arg) {
    MYDATA *mydata = (MYDATA *)arg;
    
    uint32_t timeout_usec = 1000*1000; // 1000ms
    while (mydata->run_talkback)
    {
        dana_audio_packet_t *pmsg = lib_danavideoconn_readaudio(mydata->danavideoconn, timeout_usec);
        if (pmsg) {
            dbg("th_talkback read a audio frame\n");
            dbg("\t\x1b[31mth_talkback audio frame codec is %"PRId32"\x1b[0m\n", pmsg->codec);
            lib_danavideo_audio_packet_destroy(pmsg);
        } else {
            usleep(10*1000); // 10ms
        }
    }

    dbg("th_talkback exit\n");
    mydata->exit_talback = true;
    return NULL;
}

#define AUDIO_FORMAT_G711A

#ifdef AUDIO_FORMAT_PCM
#define AUDIO_FRAME_SIZE 640
#define AUDIO_FPS 30
#define AUDIO_CODEC MEDIA_CODEC_AUDIO_PCM

#elif defined (AUDIO_FORMAT_ADPCM)
#define AUDIO_FRAME_SIZE 160
#define AUDIO_FPS 30

#elif defined (AUDIO_FORMAT_SPEEX)
#define AUDIO_FRAME_SIZE 38
#define AUDIO_FPS 56

#elif defined (AUDIO_FORMAT_MP3)
#define AUDIO_FRAME_SIZE 380
#define AUDIO_FPS 32

#elif defined (AUDIO_FORMAT_SPEEX_ENC)
#define AUDIO_FRAME_SIZE 160
#define AUDIO_ENCODED_SIZE 160
#define AUDIO_FPS 56

#elif defined (AUDIO_FORMAT_G726_ENC)
#define AUDIO_FRAME_SIZE 320
#define AUDIO_ENCODED_SIZE 80
#define AUDIO_FPS 50

#elif defined (AUDIO_FORMAT_G726)
#define AUDIO_FRAME_SIZE 80
#define AUDIO_FPS 50

#elif defined (AUDIO_FORMAT_G711U)
#define AUDIO_FRAME_SIZE 320
#define AUDIO_FPS 50

#elif defined (AUDIO_FORMAT_G711A)
#define AUDIO_FRAME_SIZE 320
#define AUDIO_FPS 50
#endif

#if 0
static void *th_AudioFrameData(void *arg)
{
    MYDATA * mydata = (MYDATA *)arg;
    unsigned char  *audioData = NULL;
    int audioSz = 0;
    struct timeval audioTimestamp = {0};
    int  audioIsKey = 1;
    int *refSerial;
    int audioSerialBook = -1;

    int frameRate = AUDIO_FPS;
    int sleepTick = 1000000/frameRate;

    FRAMEINFO_t frameInfo;
    memset(&frameInfo, 0, sizeof(FRAMEINFO_t));
    frameInfo.codec_id = AUDIO_CODEC;
    frameInfo.flags = (AUDIO_SAMPLE_8K << 2) | 
                      (AUDIO_DATABITS_16 << 1) | 
                      AUDIO_CHANNEL_MONO;
    int streamTypeChanged=0;

    g_StreamType == ENTRY_AUDIO_PREVIEW;

    do {
        if(g_StreamType==ENTRY_PLAYBACK)
        {
           audioSerialBook = MMng_GetSerial_new(ENTRY_AUDIO_PLAYBACK,&refSerial);
        }else
        {
           audioSerialBook = MMng_GetSerial_new(ENTRY_AUDIO_PREVIEW,&refSerial);
        }

        if (audioSerialBook==SHM_INVALID_SERIAL)
        {
           dbg("**AUDIO: get invalid audio serial, try again.\n");
            usleep(10*1000);
        }
        else
        {
            dbg("**AUDIO: Init audio Serial number: %u\n", audioSerialBook);
             break;
        }
    } while (1);


   dbg("=================thread_AudioFrameData\n");

    const int MAX_BUF_FRAMES = 60;
    char* pAudioBuf = (char*)malloc(MAX_BUF_FRAMES*AUDIO_FRAME_SIZE);
    ASSERT(pAudioBuf);
    memset(pAudioBuf, 0, MAX_BUF_FRAMES*AUDIO_FRAME_SIZE);
    streamTypeChanged=g_StreamType;
   dbg("%s g_streamType:%d\n", __FUNCTION__, g_StreamType);
    while(mydata->run_media)
    {

        if (streamTypeChanged!=g_StreamType)
        {
           dbg("%s g_streamType:%d\n", __FUNCTION__, g_StreamType);

            streamTypeChanged=g_StreamType;
            if(g_StreamType==ENTRY_PLAYBACK)
            {
                audioSerialBook = MMng_GetSerial_new(ENTRY_AUDIO_PLAYBACK,&refSerial);
            }else
            {
                audioSerialBook = MMng_GetSerial_new(ENTRY_AUDIO_PREVIEW,&refSerial);
            }
        }
        
        if(g_StreamType==ENTRY_PLAYBACK)
        {
            MMng_Read(ENTRY_AUDIO_PLAYBACK, audioSerialBook, &audioData, &audioSz, &audioTimestamp, &audioIsKey);
        } else
        {
            MMng_Read(ENTRY_AUDIO_PREVIEW, audioSerialBook, &audioData, &audioSz, &audioTimestamp, &audioIsKey);
        }

        if (audioSz > 0) 
        {
            int N = audioSz/AUDIO_FRAME_SIZE;
            if ((MAX_BUF_FRAMES-1) < N)
            {
                DEBUG_AUDIO_ERR(stderr,"Too more audio data %d frames.\n", N);
                N = MAX_BUF_FRAMES-1;
            }
            char* tmp = pAudioBuf;            

            static int audio_send_fd = -1;
            static int soyo_cnt = 0;
            static int pcm_save_flag = 0;

            if (pcm_save_flag && (soyo_cnt < 160)) {
                soyo_cnt++;

                if (audio_send_fd == -1) {
                    audio_send_fd = open("/mnt/mmc/audio_send.wav", O_WRONLY|O_CREAT, 0777);
                }
                if (audio_send_fd != -1) {
                    write(audio_send_fd, tmp, audioSz);
                }
            } else {
                if (audio_send_fd != -1) {
                    close(audio_send_fd);
                    audio_send_fd = -1;
                }
            }

            int j;
            for(j = 0; j < (N+1); j++)
            {
                if(j==N)
                {
                 if((audioSz-N*AUDIO_FRAME_SIZE)<=0)
                 {
                    break;
                 }
                }
                //memset(tmp+j*AUDIO_FRAME_SIZE, 0, AUDIO_FRAME_SIZE);
                if(j==N)
                {
                    memcpy(tmp+j*AUDIO_FRAME_SIZE, 
                           audioData + j*AUDIO_FRAME_SIZE,
                           audioSz-N*AUDIO_FRAME_SIZE );
                }
                else
                {
                    memcpy(tmp+j*AUDIO_FRAME_SIZE, audioData + j*AUDIO_FRAME_SIZE, AUDIO_FRAME_SIZE);
                }
                frameInfo.timestamp = getTimeStamp();

                int i = 0;
                for(i = 0; i < 1; i++)
                {
                    if(0 == getClientProperty(i, CLIENT_STATUS).status)
                        continue;

                    int avIndex = getClientProperty(i, CLIENT_AV_INDEX).iVal;
                    unsigned int bEnableAudio = getClientProperty(i, CLIENT_AUDIO_STATUS).iVal;
                    if((0 > avIndex) || (0 == bEnableAudio))
                        continue;
                    
                   frameInfo.onlineNum = getOnlineNum();
                   int retAudio=0;

                   static int soyo_test = 0;
                   soyo_test++; 

                   if(j==N)
                   {
                         retAudio = avSendAudioData(avIndex, 
                                                    tmp+j*AUDIO_FRAME_SIZE, 
                                                    audioSz-N*AUDIO_FRAME_SIZE, 
                                                    &frameInfo, 
                                                    sizeof(FRAMEINFO_t));
            //printf("avSendAudioData .............. j==n \n");
                   }
                   else
                   {
                       if ((soyo_test & 0x7F) == 0X7F) {
                          dbg("avSendAudioData size:%d g_streamType:%d \n", AUDIO_FRAME_SIZE, g_StreamType);
                       }

                       retAudio = avSendAudioData(avIndex, 
                               tmp+j*AUDIO_FRAME_SIZE, 
                               AUDIO_FRAME_SIZE, 
                               &frameInfo, 
                               sizeof(FRAMEINFO_t));
                   }  

                    //DEBUG_INFO(stderr,"avIndex[%d] size[%d]\n", gClientInfo[i].avIndex, audio_data.size);
                    if(retAudio == AV_ER_EXCEED_MAX_SIZE) // means data not write to queue, send too slow, I want to skip it
                    {
                        DEBUG_AUDIO_ERR(stderr,"send audio fail AV_ER_EXCEED_MAX_SIZE[%d]\n", avIndex);
                        usleep(10*1000);
                        continue;
                    }
                    else if(retAudio == AV_ER_SESSION_CLOSE_BY_REMOTE)
                    {
                        DEBUG_AUDIO_ERR(stderr,"send audio fail AV_ER_SESSION_CLOSE_BY_REMOTE\n");
                        Client_Property_Param val = {0};
                        val.status = 0;
                        setClientProperty(i, CLIENT_AUDIO_STATUS,
                                          (const Client_Property_Param)val);
                        continue;
                    }
                    else if(retAudio == AV_ER_REMOTE_TIMEOUT_DISCONNECT)
                    {
                        DEBUG_AUDIO_ERR(stderr,"send audio fail AV_ER_REMOTE_TIMEOUT_DISCONNECT\n");
                        Client_Property_Param val = {0};
                        val.status = 0;
                        setClientProperty(i, CLIENT_AUDIO_STATUS,
                                          (const Client_Property_Param)val);
                        continue;
                    }
                    else if(retAudio == IOTC_ER_INVALID_SID)
                    {
                        DEBUG_AUDIO_ERR(stderr,"send audio fail IOTC_ER_INVALID_SID\n");
                        Client_Property_Param val = {0};
                        val.status = 0;
                        setClientProperty(i, CLIENT_AUDIO_STATUS,
                                          (const Client_Property_Param)val);
                        continue;
                    }
                    else if(retAudio < 0)
                    {
                        DEBUG_AUDIO_ERR(stderr,"avSendAudioData error[%d]\n", retAudio);
                        continue;
                        //unregedit_client_from_audio(i);
                    }
                }
                usleep(sleepTick);
            }
            
            audioSerialBook++;            
        }
        else 
        {
            if(g_StreamType==ENTRY_PLAYBACK)
            {
                if(MMng_IsValidSerial(ENTRY_AUDIO_PLAYBACK, audioSerialBook) == 0)
                {
                    // not valid
                    audioSerialBook = MMng_GetSerial_new(ENTRY_AUDIO_PLAYBACK, &refSerial);
                }
            }else
            {
                if(MMng_IsValidSerial(ENTRY_AUDIO_PREVIEW, audioSerialBook) == 0)
                {
                    // not valid
                    audioSerialBook = MMng_GetSerial_new(ENTRY_AUDIO_PREVIEW, &refSerial);
                }
            }
            usleep(sleepTick);
        }
    }
    free(pAudioBuf);
    pAudioBuf = NULL;  

    DEBUG_INFO(stderr,"[thread_AudioFrameData] exit\n");

    pthread_exit(0);
}
#endif

static int audioSerialBook = -1;


int readAudioFrame(int* mediaType, 
              int* timeStamp, 
              char* pBuff, 
              int* pSize, 
              int* pIsKeyFrame)
{
    unsigned char *mediaData = NULL;
    int mediaSz = 0;
    struct timeval timestamp= {0};
    int *refSerial = NULL;
    int tryAgain = 1; 



    ASSERT(mediaType && timeStamp && pBuff && pSize && pIsKeyFrame);
       
    ASSERT(pBuff); 
    
    while(tryAgain)
    { 
        int i = 1;
        SHM_ENTRY_TYPE shmType = ENTRY_AUDIO_PREVIEW;
        char* buf = pBuff;
            
        //printf("----: audo framedata i=%d,shmType=%d\n", i, shmType);
        MMng_Read(shmType, audioSerialBook, &mediaData, &mediaSz, &timestamp, pIsKeyFrame);
        if (mediaSz>0) {
            if (timeStamp) *timeStamp = (int)(timestamp.tv_sec*1000+timestamp.tv_usec/1000);
            //printf("---: read audio ok,SIZE:%d, shmtype:%d. ts: %u, tsSec:%ld\n", 
                //mediaSz, shmType, (unsigned int)*timeStamp, timestamp.tv_sec);
            if(mediaSz>300*1024)
            {
               dbg("MMng_Read AUDIO error:too big mediaSz=%d\n",mediaSz);
                continue;
            }
            memcpy(buf,mediaData,mediaSz);
            tryAgain = 0;
            audioSerialBook++;
{      

            static int audio_send_fd = -1;
            static int saveFrameCnt = 0;
            static int pcm_save_flag = 0;

            if (pcm_save_flag && (saveFrameCnt < 100*500)) {
                saveFrameCnt++;

                if (audio_send_fd == -1) {
                    audio_send_fd = open("/mnt/mmc/beforesend.wav", O_WRONLY|O_CREAT, 0777);
                }
                if (audio_send_fd != -1) {
                    write(audio_send_fd, mediaData, mediaSz);
                }
            } else {
                if (audio_send_fd != -1) {
                    close(audio_send_fd);
                    audio_send_fd = -1;
                }
            }
}            
        } else {
            //printf("***Mason: MMng_Read serial:%d fail.\n", audioSerialBooks);
            while (MMng_IsValidSerial(shmType, audioSerialBook) == 0)
            {
                audioSerialBook = MMng_GetSerial_new(shmType, &refSerial);
               dbg("***Mason:Mng_Read audio error update serial number to %d.\n", audioSerialBook);
                usleep(1000);
            } 
            //usleep(1000);
            //goto NEXT_TRY;
             return -1;
        }

        //usleep(20*1000);//(20*1000);
    }

    if (mediaType) *mediaType = MEDIATYPE_AUDIO_G711A;
    
    if (pSize) *pSize = mediaSz;
    return 0;
}

void* th_audio_media(void *arg)
{
    MYDATA *mydata = (MYDATA *)arg;

    uint32_t timeout_usec = 1000*1000; // 1000ms

    void *pRdd;
    BYTE *pBuff = NULL;
    DWORD timestamp, media_type, size, isKeyFrame;
    DWORD tick0;
    DWORD timestamp_base = 0;

    int *refSerial;
    DWORD firstSt = 0;

	int ret;	
	struct fr_buf_info fr_send_buf = FR_BUFINITIALIZER;
	struct codec_info send_info;
	struct codec_addr send_addr;
	void *codec = NULL;
	char enc_buf[AUDIO_BUF_SIZE] = {0};
	int send_handle = 1;

	send_info.in.codec_type = AUDIO_CODEC_PCM;
	send_info.in.channel = 2;
	send_info.in.sample_rate = 16000;
	send_info.in.bitwidth = 24;
	send_info.in.bit_rate = send_info.in.channel * send_info.in.sample_rate \
							* GET_BITWIDTH(send_info.in.bitwidth);
	send_info.out.codec_type = AUDIO_CODEC_G711A;
	send_info.out.channel = DEFAULT_CODEC_CHANNLES;
	send_info.out.sample_rate = DEFAULT_CODEC_SAMPLERATE;
	send_info.out.bitwidth = DEFAULT_CODEC_BITWIDTH;
	send_info.out.bit_rate = send_info.out.channel * send_info.out.sample_rate \
							 * GET_BITWIDTH(send_info.out.bitwidth);
	codec = codec_open(&send_info);
    pBuff = (BYTE*) calloc(1, 300*1024);
    if (NULL == pBuff) {
        dbg("th_audio_media cant't calloc pBuff\n");
        goto fin;
    }
    tick0 = danale_gettickcount();
    audioSerialBook = MMng_GetSerial_new(ENTRY_AUDIO_PREVIEW, &refSerial);
    while (mydata->run_audio_media) {
        size = 300*1024;
		DWORD err = 0;
     
		while(mydata->run_audio_media)
		{
			ret = audio_get_frame(send_handle, &fr_send_buf);
			if ((ret < 0) || (fr_send_buf.size <= 0) || (fr_send_buf.virt_addr == NULL)) {
				dbg("get audio frame failed %d, %d, size %d\n", send_handle, ret, fr_send_buf.size);
				break;
			}

			timestamp = fr_send_buf.timestamp;

			send_addr.in = fr_send_buf.virt_addr;
			send_addr.len_in = fr_send_buf.size;
			send_addr.out = pBuff;
			send_addr.len_out = sizeof(pBuff);
			ret = codec_encode(codec, &send_addr);
			if (ret < 0) {
				dbg("audio send codec encode failed\n");
			}
			audio_put_frame(send_handle, &fr_send_buf);
			media_type = MEDIATYPE_AUDIO_G711A;
			size = sizeof(pBuff);

	/*		err = readAudioFrame(&media_type, &timestamp, pBuff, &size, &isKeyFrame);
				if(err==0)
				{
				   break;  
				}
			    usleep(10*1000);
				*/
		    } 
			if(!mydata->run_audio_media)
			{
			  break;
			}


		
        if (0 == err) {

            if (firstSt == 0) firstSt = timestamp;
            timestamp = timestamp - firstSt;
            DWORD cur = danale_gettickcount()-tick0;

            while (cur < timestamp)
			{
			    if((cur+500)<timestamp)
				{
				   tick0=danale_gettickcount()-timestamp;
				}
                usleep(1*1000); // sleep 1 ms
                cur = danale_gettickcount()-tick0;
            }
			
            if (MEDIATYPE_AUDIO_G711A == media_type) {
                dbg("\t\x1b[33mth_audio_media audio frame codec is %"PRId32"\x1b[0m\n", media_type); 
                if (lib_danavideoconn_send(mydata->danavideoconn, audio_stream, 
                    G711A, mydata->chan_no, isKeyFrame, 
                    timestamp + timestamp_base, (const char*)pBuff, size, timeout_usec)) {
{
                char* tmp = pBuff;            

                static int audio_send_fd = -1;
                static int saveFrameCnt = 0;
                static int pcm_save_flag = 0;

                if (pcm_save_flag && (saveFrameCnt < 500)) {
                    saveFrameCnt++;

                    if (audio_send_fd == -1) {
                        audio_send_fd = open("/mnt/mmc/audio_send.wav", O_WRONLY|O_CREAT, 0777);
                    }
                    if (audio_send_fd != -1) {
                        write(audio_send_fd, tmp, size);
                    }
                } else {
                    if (audio_send_fd != -1) {
                        close(audio_send_fd);
                        audio_send_fd = -1;
                    }
                }
}                    
                    //printf("-----th_audio_media send size [%u] succeeded\n", size);
                } else {
                   dbg("======th_audio_media send size [%u] failed\n", size);
                    usleep(1000);
                    continue;
                }
            } else {
               dbg("====th_audio_media NOT_G711A ##########################\n");
            }
        } else if (E_EOF == err) {
            //OurReader.SeekKeyFrame(0, pRdd);
           dbg("==== th_audio_meida read frame error E_EOF.\n");
            tick0 = danale_gettickcount();
            firstSt = 0;
            usleep(40*1000);
            timestamp_base += timestamp;
        } else {
           dbg("=====th_audio_media read audio data error: %u\n", err);
           dbg("th_media read frame error: %u\n", err);
            if (!gIsInPreview) {
                //deinitDemux(handle);
                break;
            }
            usleep(30*1000);
        }
    }
    //OurReader.EndReading(pRdd);
fin:
    if (pBuff) {
        free(pBuff);
    }
    
   dbg("th_audio_media exit\n");
    mydata->exit_audio_media = true;
    mydata->run_audio_media = false;
    return NULL;
}

extern int deinitDemux(const int handle);

static int videoSerialBooks = {0};

static int gFrameCount = 0;

AV_VIDEO_StreamInfo *cfg_shm_getVideoStrmInfoByIndex(T_VIDEO_STREAM_ID id);


int readFrame(int* mediaType, // H265|264
              int* timeStamp, 
              char* pBuff, 
              int* pSize, // INPUT: BUFF SIZE, OUTPUT: DATA LEN.
              int* pIsKeyFrame)
{
    unsigned char *mediaData = NULL;
    int videoSz = 0;
    struct timeval timestamp= {0};
    char *hdrData = NULL;
    int hdrSz = 0;
    int *refSerial = NULL;
    int tryAgain = 1; 
	static int count=0;
	int ret = 0;


#if 0
    //AV_VIDEO_StreamInfo * stream = cfg_shm_getVideoStrmInfoByIndex(g_StreamType);
    SHM_ENTRY_TYPE shmType = MMng_GetShmEntryType(g_StreamType);

    ASSERT(mediaType && timeStamp && pBuff && pSize && pIsKeyFrame);
       
    ASSERT(pBuff); 
    
    while(tryAgain)
    { 
        int i = 1;
        //stream = cfg_shm_getVideoStrmInfoByIndex(g_StreamType);
        //SHM_ENTRY_TYPE curType = stream->resolutionType;
        char* buf = pBuff;

        /*if (curType != shmType) {
            videoSerialBooks = 0;
            while (MMng_IsValidSerial(curType, videoSerialBooks) == 0)
            {
                videoSerialBooks = MMng_GetSerial_I(curType, &refSerial);
               dbg("***try to switch from:%d to: %d serial number to %d.\n", curType, shmType, videoSerialBooks);
                usleep(1000);
            }

           dbg("***success switch from:%d to: %d serial number to %d.\n", curType, shmType, videoSerialBooks);
            shmType = curType;
        }*/
        
//NEXT_TRY:            
        //printf("***Mason: videoframedata i=%d,shmType=%d\n", i, shmType);
        MMng_Read(shmType, videoSerialBooks, &mediaData, &videoSz, &timestamp, pIsKeyFrame);
		if(((count++)%300)==0)
		{
				  dbg("MMng_Read count=%d,videoSerialBooks=%d,videoSz=%d\n",count,videoSerialBooks,videoSz);
		}
        if (videoSz>0) {
            //printf("***Mason: read video frame ok, type:%d.\n", shmType);
            if(videoSz>1024*1024)
            {
               dbg("MMng_Read error:too big videoSz=%d\n",videoSz);
				videoSerialBooks++;
                return -1;//continue;
            }
            hdrSz=0;
            /*if(*pIsKeyFrame)
            {
                MMng_Get_Extra(shmType,(unsigned char **) &hdrData, &hdrSz);
                memcpy(buf,hdrData,hdrSz);                
            }*/
            //printf("---Mason: i:%d, serial:%d, isKey:%d, size:%d\n", 
                     //i, videoSerialBooks, *pIsKeyFrame, hdrSz+videoSz);
            memcpy(buf+hdrSz,mediaData,videoSz);        
            //printf("sendDataToClients buf:%p size:%d videoIsKey:%d \n", 
                    //buf, hdrSz+videoSz, *pIsKeyFram); 
            videoSerialBooks++;
            tryAgain = 0;            
        } 
		else 
		{
            //printf("***Mason: MMng_Read serial:%d fail.\n", videoSerialBooks);
            // make sure serial is valid
            while (MMng_IsValidSerial(shmType, videoSerialBooks) == 0)
            {
                videoSerialBooks = MMng_GetSerial_I(shmType, &refSerial);
               dbg("***Mason:Mng_Read error update serial number to %d.\n", videoSerialBooks);
                usleep(200);
            } 
            //usleep(10*000);
			 tryAgain = 0;         
			 return -1;
            //goto NEXT_TRY;
        }

        //usleep(20*1000);//(20*1000);
    }
#endif
//    if (timeStamp) *timeStamp = (int)(timestamp.tv_sec*1000 + timestamp.tv_usec/1000);//gFrameCount*50;//33; //ms
    return 0;
}

extern int initDemux(char* fileName, 
                     int* keyFrameCount,
                     int* mediaWidth,
                     int* mediaHeight,
                     int* secs);

extern int readVideoFrame(int handle,
                   int* timeStamp, 
                   char* pBuff, 
                   int* pSize, // INPUT: BUFF SIZE, OUTPUT: DATA LEN.
                   int* pIsKeyFrame,AVI_DATA_TYPE *frameType);


int * readPic(char* fileName,int* size,char* pBuff)
{
    int ret = -1;
    int fd = -1;
    int file_size = -1;    
    int actual_len = -1;
     dbg("readPic open jpg file:%s \n", fileName);
	  fd = open(fileName, O_RDONLY);
	  if (fd < 0) {
		  dbg("can not open jpeg file (%s)\n", fileName);
		   *size=0;
		  return -1;
	  }
   
	  file_size = lseek(fd, 0L, SEEK_END);
	 dbg("jpg file size:%d \n", file_size);
   
	  lseek(fd, 0L, SEEK_SET);

	  actual_len = read(fd, pBuff, file_size);
	  if (actual_len != file_size) {
		  dbg("Warning!!! actual read %d, file_size:%d \n", actual_len, file_size);
		  *size=0;
		  if (fd > 0) 
		  {
       		 close(fd);
      	  }
		  return -1;
	  } 
	  *size =actual_len;
	   if (fd > 0)
	   {
         close(fd);
       }
   return 0;
}
//static FILE * videoFd=NULL;
void* th_media(void *arg)
{    
    MYDATA *mydata = (MYDATA *)arg;
    DWORD media_type, size, isKeyFrame;
    uint32_t timeout_usec = 1000*1000; // 1000ms

    DWORD firstSt = 0;
	DWORD cur = 0;
    DWORD tick0 = 0;
	DWORD timestamp = 0;
	DWORD timeinter = 0;

    int count =0;
	int whilecount =0 ;
    int state = 0;
    int ret = 0;
	int head_len = 0;
	int frame_flag = 0;

    danavidecodec_t codec=H265;
	danavideo_msg_type_t msg_type=video_stream;
	T_VIDEO_STREAM_ID streamType =g_StreamType;
	char header[256];

    BYTE *pBuff = NULL;
    pBuff = (BYTE*) calloc(1, VIDEO_BUF_SIZE);
    if (NULL == pBuff) {
        dbg("th_media cant't calloc pBuff\n");
        goto fin;
    }  

	struct fr_buf_info fr_send_buf = FR_BUFINITIALIZER;
	tick0 = danale_gettickcount();


    while (mydata->run_media) {
        isKeyFrame = 0;

		ret = video_enable_channel(WIFI_CHN);
		if (ret < 0) {
			dbg("open channel %s error\n", WIFI_CHN);
			break;
		}
		head_len = video_get_header(WIFI_CHN, header, 128);
		if (head_len < 0) {
			dbg("get header error\n");
			continue;
		}
		memcpy(pBuff, header, head_len);
		
		fr_INITBUF(&fr_send_buf, NULL);
		ret = video_get_frame(WIFI_CHN, &fr_send_buf);
		if ((ret < 0) || (fr_send_buf.size <= 0) || (fr_send_buf.virt_addr == NULL)) {
			dbg("video_get_frame error %d %d \n", ret, fr_send_buf.size);
			frame_flag = VIDEO_FRAME_P;
			continue;
		}
		if (fr_send_buf.size > (VIDEO_BUF_SIZE - head_len)) {
			dbg("video_get_frame big %d \n", fr_send_buf.size);
			if (fr_send_buf.priv == VIDEO_FRAME_I) 
				frame_flag = VIDEO_FRAME_P;
			video_put_frame(WIFI_CHN, &fr_send_buf);
			continue;
		}
		if(count%300 == 0)
			dbg("fr_send_buf.size: %d, head_len: %d\n", fr_send_buf.size, head_len);
		timestamp = fr_send_buf.timestamp;
		if (fr_send_buf.priv == VIDEO_FRAME_I) {
			isKeyFrame = 1;
			frame_flag = VIDEO_FRAME_I;
			memcpy(pBuff + head_len, fr_send_buf.virt_addr, fr_send_buf.size);
			size = head_len + fr_send_buf.size;
		} else {
			if (frame_flag == VIDEO_FRAME_P) {
				video_put_frame(WIFI_CHN, &fr_send_buf);
				video_trigger_key_frame(WIFI_CHN);
				continue;
			}
			isKeyFrame = 0;
			memcpy(pBuff, fr_send_buf.virt_addr, fr_send_buf.size);
			size = fr_send_buf.size;
		}

		if(!isKeyFrame) {
			video_put_frame(WIFI_CHN, &fr_send_buf);
			video_trigger_key_frame(WIFI_CHN);
			continue;
		}

        if (firstSt == 0) firstSt = timestamp;
        timeinter = timestamp - firstSt;
        cur = danale_gettickcount()-tick0;
		if(count%100 == 0)
			dbg("timeinter: %d, cur: %d\n", timeinter, cur);
        while (cur < timeinter)
		{
		    if((cur+500)<timeinter)
			{
			   tick0=danale_gettickcount()-timeinter;
			}
            usleep(1*1000); // sleep 1 ms
            cur = danale_gettickcount()-tick0;
        }


    	ret=lib_danavideoconn_send(mydata->danavideoconn, 
    	                           msg_type, codec, 
    	                           mydata->chan_no, 
    	                           isKeyFrame, 
    	                           timestamp, 
    	                           (const char*)pBuff, size, 
    	                           timeout_usec);
		if(count%300 == 0)
			dbg("send msg_type: %d, codec: %d, chan_no: %d, isKeyFrame: %d, timestamp: %u, size: %d, ret: %d\n" ,
				msg_type, codec, mydata->chan_no, isKeyFrame, timestamp, size, ret);
    	if(!ret){
    	   	dbg("th_media send video frame: %u, isKey: %d failed\n", size, isKeyFrame);
			video_put_frame(WIFI_CHN, &fr_send_buf);
    	    usleep(2*1000); // 200*1000
    	    continue;
    	}
		video_put_frame(WIFI_CHN, &fr_send_buf);

		if(count++ > 65530)
			count = 0;

    	    usleep(100*1000);
	}
fin:
    if (pBuff) {
        free(pBuff);
    }
   	dbg("th_media exit, flag:%d\n", mydata->run_media);
    mydata->exit_media = true;
    mydata->run_media = false;
    return NULL;
}

// 处理成功返回0
// 失败返回-1
void start_record_cmd()
{
    char buf[IPC_MBX_MAX_SIZE];
   dbg("start_record_cmd IPC_MSG_CMD_FILE_SAVE_AVI\n");
	snprintf(buf, IPC_MBX_MAX_SIZE, "%d", INFT_CAMERA_FRONT_REAR);
//	start_video();
}

void stop_record_cmd()
{
    char buf[IPC_MBX_MAX_SIZE];
   dbg("stop_record_cmd IPC_MSG_CMD_FILE_SAVE_EXIT\n");
	snprintf(buf, IPC_MBX_MAX_SIZE, "%d", INFT_CAMERA_FRONT_REAR);
    //AV_OSA_ipcMbxSendStr(IPC_MSG_SYS_SERVER, IPC_MSG_TUTK, IPC_MSG_CMD_FILE_SAVE_EXIT, buf);
	//AV_OSA_ipcMbxSendStr(IPC_MSG_AV_SERVER, IPC_MSG_TUTK, IPC_MSG_CMD_FILE_SAVE_EXIT, buf);
}

void start_preview_cmd(uint32_t videoquality)
{
        T_VIDEO_STREAM_ID id = E_STREAM_ID_PREVIEW;
	    T_VIDEO_RESOLUTION rtype = E_VIDEO_RESOLUTION_1080P;

		
	    if (videoquality < 26) {
	        rtype = E_VIDEO_RESOLUTION_VGA;
	    } else if (videoquality < 51) {
	        rtype = E_VIDEO_RESOLUTION_720P;
	    } else if (videoquality < 56) {
	        rtype = E_VIDEO_RESOLUTION_1080P;
	    }
        if(videoquality < 56)
		{
		    g_StreamType=E_STREAM_ID_PREVIEW;
			//cfg_shm_setVideoStrmResolution(id, rtype);
		}
		else
		{
	        g_StreamType=E_STREAM_ID_MJPEG;
			//AV_OSA_ipcMbxSendI(IPC_MSG_AV_SERVER,IPC_MSG_TUTK, IPC_MSG_CMD_AV_SVR_MJPEG_START, 0);
	    }
	    
}

void stop_preview_cmd()
{
		  if(g_StreamType==E_STREAM_ID_PREVIEW)
		  {
		     //cfg_shm_setVideoStrmResolution(E_STREAM_ID_PREVIEW, E_VIDEO_RESOLUTION_CNT);
		  }
		  else if(g_StreamType==E_STREAM_ID_MJPEG)
		  {
		     //AV_OSA_ipcMbxSendI(IPC_MSG_AV_SERVER,IPC_MSG_TUTK, IPC_MSG_CMD_AV_SVR_MJPEG_STOP, 0);
		  }
		  g_StreamType=E_STREAM_ID_PREVIEW;
}

uint32_t danavideoconn_created(void *arg) // pdana_video_conn_t
{
    dbg("TEST danavideoconn_created\n");
    //return -1;
    pdana_video_conn_t *danavideoconn = (pdana_video_conn_t *)arg; 
    // set user data  
    MYDATA *mydata = (MYDATA*) calloc(1, sizeof(MYDATA));
    if (NULL == mydata) {
        dbg("TEST danavideoconn_created failed calloc\n");
        return -1;
    }


    mydata->thread_media = 0;

    mydata->thread_talkback = 0;
    mydata->danavideoconn = danavideoconn;
    mydata->chan_no = 0;
    strncpy(mydata->appname, "libdanavideo_di3", strlen("libdanavideo_di3"));


    if (0 != lib_danavideo_set_userdata(danavideoconn, mydata)) {
        dbg("TEST danavideoconn_created lib_danavideo_set_userdata failed\n");
        free(mydata);
        return -1;
    }

    dbg("TEST danavideoconn_created succeeded\n");


    // 测试发送 extend_data
#if 1
    char *extend_data_byte = "test_di3"; // 可以是自定义的二进制数据
    size_t extend_data_size = strlen(extend_data_byte);
    uint32_t timeout_usec = 4000*1000;
    if (lib_danavideoconn_send(mydata->danavideoconn, extend_data, 0, mydata->chan_no, 0, 0, (const char*)extend_data_byte, extend_data_size, timeout_usec)) {
        dbg("TEST send extend_data[%u] succeeded\n", extend_data_size);
    } else {
        dbg("TEST send extend_data[%u] failed\n", extend_data_size);
    }
#endif

    return 0; 
}

void danavideoconn_aborted(void *arg) // pdana_video_conn_t
{
   dbg("\n\n danavideoconn_aborted...\\n\n");
    pdana_video_conn_t *danavideoconn = (pdana_video_conn_t *)arg;
    MYDATA *mydata;
    if (0 != lib_danavideo_get_userdata(danavideoconn, (void **)&mydata)) {
        dbg("TEST danavideoconn_aborted lib_danavideo_get_userdata failed, mem-leak\n");
        return;
    }
    
    if (NULL != mydata) {

        // stop video thread
        mydata->run_media = false;
       dbg("====abort close play thread.\n");
        if (0 != mydata->thread_media) {
			if(gIsInPreview ==1)
			{
			  stop_preview_cmd();
			}
#ifdef PLAYBACK_RECODE			
			else(gIsInPreview ==2)
			{ 
			    unsigned int enable=0;
                char buf[IPC_MBX_MAX_SIZE];
		        snprintf(buf, IPC_MBX_MAX_SIZE, "%d|%s", enable, "no.mp4");
		        //AV_OSA_ipcMbxSendStr(IPC_MSG_AV_SERVER,IPC_MSG_TUTK, IPC_MSG_CMD_AV_SVR_PLAYBACK, buf);
				   if(g_CaseType ==CASE_PLAYBACK)
					{
					  start_record_cmd();
					}
					g_CaseType =CASE_PREVIEW;
			}
#endif
            void *end;
            if (0 != pthread_join(mydata->thread_media, &end)) {
                perror("danavideoconn_aborted thread_media join faild!\n");
            }
			gIsInPreview = 0;
        }
        memset(&(mydata->thread_media), 0, sizeof(mydata->thread_media));

        mydata->run_talkback = false; 
        if (0 != mydata->thread_talkback) {
            void *end;
            if (0 != pthread_join(mydata->thread_talkback, &end)) {
                perror("danavideoconn_aborted thread_talkback join faild!\n");
            }
        }
        memset(&(mydata->thread_talkback), 0, sizeof(mydata->thread_talkback));

        mydata->run_audio_media = false;
        if (0 != mydata->thread_audio_media) {
            if (0 != pthread_join(mydata->thread_audio_media, NULL)) {
                perror("danavideoconn_aborted thread_audio_media join faild!\n");
            }
        }    
        memset(&(mydata->thread_audio_media), 0, sizeof(mydata->thread_audio_media));    

        dbg("TEST danavideoconn_aborted APPNAME: %s\n", mydata->appname);

        free(mydata);
    }
    
    dbg("TEST danavideoconn_aborted succeeded\n");
    // get user data
    return;
}

#include "playback.h"
extern LocalFileList g_media_files[MEDIA_COUNT];

extern int AcountMediaFiles(void);

extern int getAlarms(T_AlarmDBRecord* alarms,MEDIA_TYPE fileType);

extern int getFileTotal(MEDIA_TYPE fileType);


static void updateList()
{
    static long lastTime = 0;
    struct timeval timeval = {0};
    gettimeofday(&timeval, NULL);
    long curTime = timeval.tv_sec;
   dbg("handleRecListReq curTime: %ld , last: %ld.\n", curTime, lastTime);
    if (lastTime==0 || (lastTime+60)<curTime) {
        AcountMediaFiles();
        lastTime = curTime;
    }
}

#define RECORD_LIST_MAX 40

static void handleRecListReq(void *arg, dana_video_cmd_t cmd, uint32_t trans_id, void *cmd_arg, uint32_t cmd_arg_len)
    //int clientIndex, int avIndex, char *buf, int type)
{
    uint32_t error_code;
    char *code_msg;
    pdana_video_conn_t *danavideoconn = (pdana_video_conn_t *)arg;
    int total = 0;
    uint32_t rec_lists_count = 0;
    libdanavideo_reclist_recordinfo_t rec_lists[RECORD_LIST_MAX] = {0};    
    DANAVIDEOCMD_RECLIST_ARG *reclist_arg = (DANAVIDEOCMD_RECLIST_ARG *)cmd_arg;
    int i = 0;
    
   dbg("handleRecListReq ch_no: %"PRIu32"\n", reclist_arg->ch_no);
    if (DANAVIDEO_REC_GET_TYPE_NEXT == reclist_arg->get_type) {
       dbg("get_type: DANAVIDEO_REC_GET_TYPE_NEXT\n");
    } else if (DANAVIDEO_REC_GET_TYPE_PREV == reclist_arg->get_type) {
       dbg("get_type: DANAVIDEO_REC_GET_TYPE_PREV\n");
    } else {
       dbg("Unknown get_type: %"PRIu32"\n", reclist_arg->get_type);
    }
   dbg("get_num: %"PRIu32"\n", reclist_arg->get_num);
   dbg("last_time: %"PRId64"\n", reclist_arg->last_time);
    error_code = 0;
    code_msg = "";
    static T_AlarmDBRecord* out = NULL;
    updateList();
	total = getFileTotal(reclist_arg->ch_no-1);

    out = malloc(sizeof(T_AlarmDBRecord)*total);
    memset(out, 0, sizeof(T_AlarmDBRecord)*total);
    getAlarms(out,reclist_arg->ch_no-1);                

    T_AlarmDBRecord* pTotalAlarms = out;
   dbg("***Mason: total files=%d\n",total);    

    for(i=0;i<total;i++)
    {
        int64_t t = (int64_t)pTotalAlarms[total-i-1].fileKey;
        
        t -= 8 * 60 * 60;

        if ((DANAVIDEO_REC_GET_TYPE_NEXT == reclist_arg->get_type) &&
            (t > reclist_arg->last_time)){            
            rec_lists[rec_lists_count].length = pTotalAlarms[total-i-1].deleted; // ? min | sec
            rec_lists[rec_lists_count].record_type = DANAVIDEO_REC_TYPE_NORMAL;
            rec_lists[rec_lists_count].start_time = t;
            rec_lists_count++;
           dbg("***Mason: send ts: %u. request last: %u\n", (unsigned int)t, (unsigned int)(reclist_arg->last_time));
            if ((rec_lists_count>=reclist_arg->get_num)||(rec_lists_count==RECORD_LIST_MAX)) break;
            
        } else if ((DANAVIDEO_REC_GET_TYPE_PREV == reclist_arg->get_type) &&
                    (t < reclist_arg->last_time)){
            rec_lists[rec_lists_count].length =  pTotalAlarms[total-i-1].deleted; // ? min | sec
            rec_lists[rec_lists_count].record_type = DANAVIDEO_REC_TYPE_NORMAL;
            rec_lists[rec_lists_count].start_time = t;
            rec_lists_count++;
            if ((rec_lists_count>=reclist_arg->get_num)||(rec_lists_count==RECORD_LIST_MAX)) break;           
        }
    }

    if (lib_danavideo_cmd_reclist_response(danavideoconn, trans_id, error_code, code_msg, rec_lists_count, rec_lists)) {
       dbg("TEST DANAVIDEOCMD_RECLIST send %d response succeeded\n", rec_lists_count);
    } else {
       dbg("TEST DANAVIDEOCMD_RECLIST send %d response failed\n", rec_lists_count);
    }
	if (out!=NULL)free(out); 
	out =NULL;
}

extern int findPlaybackFile(char* request, char* fileName,MEDIA_TYPE fileType);

void danavideoconn_command_handler(void *arg, dana_video_cmd_t cmd, uint32_t trans_id, void *cmd_arg, uint32_t cmd_arg_len) // pdana_video_conn_t
{
    dbg("TEST danavideoconn_command_handler\n");
    pdana_video_conn_t *danavideoconn = (pdana_video_conn_t *)arg;
    // 响应不同的命令
    MYDATA *mydata; 
    if (0 != lib_danavideo_get_userdata(danavideoconn, (void**)&mydata)) {
       dbg("lib_danavideo_get_userdata failed\n");
        return;
    }
   dbg("TEST danavideoconn_command_handler APPNAME: %s, cmd: %d\n", mydata->appname, cmd);

    uint32_t i;
    uint32_t error_code;
    char *code_msg;
    
    // 发送命令回应的接口进行中
    switch (cmd) {
        case DANAVIDEOCMD_DEVDEF:
            {
               dbg(" DANAVIDEOCMD_DEVDEF\n");
                DANAVIDEOCMD_DEVDEF_ARG *devdef_arg = (DANAVIDEOCMD_DEVDEF_ARG *)cmd_arg;
               dbg("devdef_arg\n");
               dbg("ch_no: %"PRIu32"\n", devdef_arg->ch_no);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg(" DANAVIDEOCMD_DEVDEF send response succeeded\n");
                } else {
                   dbg(" DANAVIDEOCMD_DEVDEF send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_DEVREBOOT:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_DEVREBOOT\n");
                DANAVIDEOCMD_DEVREBOOT_ARG *devreboot_arg = (DANAVIDEOCMD_DEVREBOOT_ARG *)cmd_arg;
                dbg("devreboot_arg\n");
                dbg("ch_no: %"PRIu32"\n", devreboot_arg->ch_no);
                dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_DEVREBOOT send response succeeded\n");
                } else {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_DEVREBOOT send response failed\n");
                } 
            }
            break; 
        case DANAVIDEOCMD_GETSCREEN:
            {
               dbg(" DANAVIDEOCMD_GETSCREEN\n");
                DANAVIDEOCMD_GETSCREEN_ARG *getscreen_arg = (DANAVIDEOCMD_GETSCREEN_ARG *)cmd_arg;
                dbg("getcreen_arg\n");
                dbg("ch_no: %"PRIu32"\n", getscreen_arg->ch_no);
                dbg("\n");
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, "", trans_id, error_code, code_msg)) {
                   dbg(" DANAVIDEOCMD_GETSCREEN send response succeeded\n");
                } else {
                   dbg(" DANAVIDEOCMD_GETSCREEN send response failed\n");
                }
                // 获取一副图片调用lib_danavideoconn_send()方法发送

            }
            break;
        case DANAVIDEOCMD_GETALARM:
            {
               dbg(" DANAVIDEOCMD_GETALARM\n");
                DANAVIDEOCMD_GETALARM_ARG *getalarm_arg = (DANAVIDEOCMD_GETALARM_ARG *)cmd_arg;
                dbg("getalarm_arg\n");
                dbg("ch_no: %"PRIu32"\n", getalarm_arg->ch_no);
                dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                uint32_t motion_detection = 1;
                uint32_t opensound_detection = 1;
                uint32_t openi2o_detection = 1;
                uint32_t smoke_detection = 1;
                uint32_t shadow_detection = 1;
                uint32_t other_detection = 1;
                if (lib_danavideo_cmd_getalarm_response(danavideoconn, trans_id, error_code, code_msg, motion_detection, opensound_detection, openi2o_detection, smoke_detection, shadow_detection, other_detection)) {
                   dbg(" DANAVIDEOCMD_GETALARM send response succeeded\n");
                } else {
                   dbg(" DANAVIDEOCMD_GETALARM send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_GETBASEINFO:
            {
               dbg(" DANAVIDEOCMD_GETBASEINFO\n");
                DANAVIDEOCMD_GETBASEINFO_ARG *getbaseinfo_arg = (DANAVIDEOCMD_GETBASEINFO_ARG *)cmd_arg;
               dbg("getbaseinfo_arg\n");
               dbg("ch_no: %"PRIu32"\n", getbaseinfo_arg->ch_no);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                char *dana_id = (char *)"di3_1";
                char *api_ver = (char *)"di3_2";
                char *sn      = (char *)"di3_3";
                char *device_name = (char *)"di3_4";
                char *rom_ver = (char *)"di3_5";
                uint32_t device_type = 1;
                uint32_t ch_num = 25;
                uint64_t sdc_size = 1024;
                uint64_t sdc_free = 512;
                if (lib_danavideo_cmd_getbaseinfo_response(danavideoconn, trans_id, error_code, code_msg, dana_id, api_ver, sn, device_name, rom_ver, device_type, ch_num, sdc_size, sdc_free)) {
                   dbg(" DANAVIDEOCMD_GETBASEINFO send response succeeded\n");
                } else {
                   dbg(" DANAVIDEOCMD_GETBASEINFO send response failed\n");
                }
            }
            break;
        case DANAVIDEOCMD_GETCOLOR:
            {
               dbg(" DANAVIDEOCMD_GETCOLOR\n");
                DANAVIDEOCMD_GETCOLOR_ARG *getcolor_arg = (DANAVIDEOCMD_GETCOLOR_ARG *)cmd_arg;
               dbg("getcolor_arg\n");
               dbg("ch_no: %"PRIu32"\n", getcolor_arg->ch_no);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                uint32_t brightness = 1;
                uint32_t contrast = 1;
                uint32_t saturation = 1;
                uint32_t hue = 1;
                if (lib_danavideo_cmd_getcolor_response(danavideoconn, trans_id, error_code, code_msg, brightness, contrast, saturation, hue)) {
                   dbg(" DANAVIDEOCMD_GETCOLOR send response succeeded\n");
                } else {
                   dbg(" DANAVIDEOCMD_GETCOLOR send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_GETFLIP:
            {
               dbg("DANAVIDEOCMD_GETFLIP\n");
                DANAVIDEOCMD_GETFLIP_ARG *getflip_arg = (DANAVIDEOCMD_GETFLIP_ARG *)cmd_arg;
               dbg("getflip_arg_arg\n");
               dbg("ch_no: %"PRIu32"\n", getflip_arg->ch_no);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                uint32_t flip_type = 1;
                if (lib_danavideo_cmd_getflip_response(danavideoconn, trans_id, error_code, code_msg, flip_type)) {
                   dbg("DANAVIDEOCMD_GETFLIP send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_GETFLIP send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_GETFUNLIST:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETFUNLIST\n");
                DANAVIDEOCMD_GETFUNLIST_ARG *getfunlist_arg = (DANAVIDEOCMD_GETFUNLIST_ARG *)cmd_arg;
                dbg("getfunlist_arg\n");
                dbg("ch_no: %"PRIu32"\n", getfunlist_arg->ch_no);
                dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                uint32_t methodes_count = 49;
                char *methods[] = { (char *)"DANAVIDEOCMD_DEVDEF", 
                    (char *)"DANAVIDEOCMD_DEVREBOOT", 
                    (char *)"DANAVIDEOCMD_GETSCREEN",
                    (char *)"DANAVIDEOCMD_GETALARM",
                    (char *)"DANAVIDEOCMD_GETBASEINFO",
                    (char *)"DANAVIDEOCMD_GETCOLOR",
                    (char *)"DANAVIDEOCMD_GETFLIP",
                    (char *)"DANAVIDEOCMD_GETFUNLIST",
                    (char *)"DANAVIDEOCMD_GETNETINFO",
                    (char *)"DANAVIDEOCMD_GETPOWERFREQ",
                    (char *)"DANAVIDEOCMD_GETTIME",
                    (char *)"DANAVIDEOCMD_GETWIFIAP",
                    (char *)"DANAVIDEOCMD_GETWIFI",
                    (char *)"DANAVIDEOCMD_PTZCTRL",
                    (char *)"DANAVIDEOCMD_SDCFORMAT",
                    (char *)"DANAVIDEOCMD_SETALARM",
                    (char *)"DANAVIDEOCMD_SETCHAN",
                    (char *)"DANAVIDEOCMD_SETCOLOR",
                    (char *)"DANAVIDEOCMD_SETFLIP",
                    (char *)"DANAVIDEOCMD_SETNETINFO",
                    (char *)"DANAVIDEOCMD_SETPOWERFREQ",
                    (char *)"DANAVIDEOCMD_SETTIME",
                    (char *)"DANAVIDEOCMD_SETVIDEO",
                    (char *)"DANAVIDEOCMD_SETWIFIAP",
                    (char *)"DANAVIDEOCMD_SETWIFI",
                    (char *)"DANAVIDEOCMD_STARTAUDIO",
                    (char *)"DANAVIDEOCMD_STARTTALKBACK",
                    (char *)"DANAVIDEOCMD_STARTVIDEO",
                    (char *)"DANAVIDEOCMD_STOPAUDIO",
                    (char *)"DANAVIDEOCMD_STOPTALKBACK",
                    (char *)"DANAVIDEOCMD_STOPVIDEO",
                    (char *)"DANAVIDEOCMD_RECLIST",
                    (char *)"DANAVIDEOCMD_RECPLAY",
                    (char *)"DANAVIDEOCMD_RECSTOP",
                    (char *)"DANAVIDEOCMD_RECACTION",
                    (char *)"DANAVIDEOCMD_RECSETRATE",
                    (char *)"DANAVIDEOCMD_RECPLANGET",
                    (char *)"DANAVIDEOCMD_RECPLANSET",
                    (char *)"DANAVIDEOCMD_EXTENDMETHOD",
                    (char *)"DANAVIDEOCMD_SETOSD",
                    (char *)"DANAVIDEOCMD_GETOSD",
                    (char *)"DANAVIDEOCMD_SETCHANNAME",
                    (char *)"DANAVIDEOCMD_GETCHANNAME",
                    (char *)"DANAVIDEOCMD_CALLPSP",
                    (char *)"DANAVIDEOCMD_GETPSP",
                    (char *)"DANAVIDEOCMD_SETPSP",
                    (char *)"DANAVIDEOCMD_SETPSPDEF",
                    (char *)"DANAVIDEOCMD_GETLAYOUT",
                    (char *)"DANAVIDEOCMD_SETCHANADV" };
                if (lib_danavideo_cmd_getfunlist_response(danavideoconn, trans_id, error_code, code_msg, methodes_count, (const char**)methods)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETFUNLIST send response succeeded\n");
                } else {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETFUNLIST send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_GETNETINFO:
            {
               dbg("DANAVIDEOCMD_GETNETINFO\n");
                DANAVIDEOCMD_GETNETINFO_ARG *getnetinfo_arg = (DANAVIDEOCMD_GETNETINFO_ARG *)cmd_arg;
               dbg("getnetinfo_arg\n");
               dbg("ch_no: %"PRIu32"\n", getnetinfo_arg->ch_no);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                uint32_t ip_type = 1; // 1 "fixed"; 2 "dhcp"
                char *ipaddr = (char *)"192.168.0.189";
                char *netmask = (char *)"255.255.255.255";
                char *gateway = (char *)"192.168.0.19";
                uint32_t dns_type = 1;
                char *dns_name1 = (char *)"8.8.8.8";
                char *dns_name2 = (char *)"114.114.114.114";
                uint32_t http_port = 21045;
                if (lib_danavideo_cmd_getnetinfo_response(danavideoconn, trans_id, error_code, code_msg, ip_type, ipaddr, netmask, gateway, dns_type, dns_name1, dns_name2, http_port)) {
                   dbg("DANAVIDEOCMD_GETNETINFO send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_GETNETINFO send response failed\n");
                }
            }
            break;
        case DANAVIDEOCMD_GETPOWERFREQ:
            {
               dbg("DANAVIDEOCMD_GETPOWERFREQ\n");
                DANAVIDEOCMD_GETPOWERFREQ_ARG *getpowerfreq_arg = (DANAVIDEOCMD_GETPOWERFREQ_ARG *)cmd_arg;
               dbg("getpowerfreq_arg\n");
               dbg("ch_no: %"PRIu32"\n", getpowerfreq_arg->ch_no);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                uint32_t freq = DANAVIDEO_POWERFREQ_50HZ;
                if (lib_danavideo_cmd_getpowerfreq_response(danavideoconn, trans_id, error_code, code_msg, freq)) {
                   dbg("DANAVIDEOCMD_GETPOWERFREQ send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_GETPOWERFREQ send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_GETTIME:
            {
               dbg(" DANAVIDEOCMD_GETTIME\n");
                DANAVIDEOCMD_GETTIME_ARG *gettime_arg = (DANAVIDEOCMD_GETTIME_ARG *)cmd_arg;
               dbg("gettime_arg_arg\n");
               dbg("ch_no: %"PRIu32"\n", gettime_arg->ch_no);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                int64_t now_time = 1;
                char *time_zone = (char *)"shanghai";
                char *ntp_server_1 = "NTP_SRV_1";
                char *ntp_server_2 = "NTP_SRV_2";
                if (lib_danavideo_cmd_gettime_response(danavideoconn, trans_id, error_code, code_msg, now_time, time_zone, ntp_server_1, ntp_server_2)) {
                   dbg(" DANAVIDEOCMD_GETTIME send response succeeded\n");
                } else {
                   dbg(" DANAVIDEOCMD_GETTIME send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_GETWIFIAP:
            {
               dbg("DANAVIDEOCMD_GETWIFIAP\n");
                DANAVIDEOCMD_GETWIFIAP_ARG *getwifiap_arg = (DANAVIDEOCMD_GETWIFIAP_ARG *)cmd_arg;
               dbg("getwifiap_arg\n");
               dbg("ch_no: %"PRIu32"\n", getwifiap_arg->ch_no);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                uint32_t wifi_device = 1;
                uint32_t wifi_list_count = 3;
                libdanavideo_wifiinfo_t wifi_list[] = {{"danasz", 1, 5}, {"danasz5G", 1, 6}, {"李尹abc123", 1, 6}};
                if (lib_danavideo_cmd_getwifiap_response(danavideoconn, trans_id, error_code, code_msg, wifi_device, wifi_list_count, wifi_list)) {
                   dbg("DANAVIDEOCMD_GETWIFIAP send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_GETWIFIAP send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_GETWIFI:
            {
               dbg("DANAVIDEOCMD_GETWIFI\n");
                DANAVIDEOCMD_GETWIFI_ARG *getwifi_arg = (DANAVIDEOCMD_GETWIFI_ARG *)cmd_arg;
               dbg("getwifi_arg\n");
               dbg("ch_no: %"PRIu32"\n", getwifi_arg->ch_no);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                char *essid = (char *)"danasz";
                char *auth_key = (char *)"wps";
                uint32_t enc_type = 1;
                if (lib_danavideo_cmd_getwifi_response(danavideoconn, trans_id, error_code, code_msg, essid, auth_key, enc_type)) {
                   dbg("DANAVIDEOCMD_GETWIFI send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_GETWIFI send response failed\n");
                }
            }
            break;
        case DANAVIDEOCMD_PTZCTRL:
            {
               dbg(" DANAVIDEOCMD_PTZCTRL\n");
                DANAVIDEOCMD_PTZCTRL_ARG *ptzctrl_arg = (DANAVIDEOCMD_PTZCTRL_ARG *)cmd_arg;
               dbg("ptzctrl_arg\n");
               dbg("ch_no: %"PRIu32"\n", ptzctrl_arg->ch_no);
               dbg("code: %"PRIu32"\n", ptzctrl_arg->code);
               dbg("para1: %"PRIu32"\n", ptzctrl_arg->para1);
               dbg("para2: %"PRIu32"\n", ptzctrl_arg->para2);
               dbg("\n");
                switch (ptzctrl_arg->code) {
                    case DANAVIDEO_PTZ_CTRL_MOVE_UP:
                        dbg("DANAVIDEO_PTZ_CTRL_MOVE_UP\n");
                        break;
                    case DANAVIDEO_PTZ_CTRL_MOVE_DOWN:
                        dbg("DANAVIDEO_PTZ_CTRL_MOVE_DOWN\n");
                        break;
                    case DANAVIDEO_PTZ_CTRL_MOVE_LEFT:
                        dbg("DANAVIDEO_PTZ_CTRL_MOVE_LEFT\n");
                        break;
                    case DANAVIDEO_PTZ_CTRL_MOVE_RIGHT:
                        dbg("DANAVIDEO_PTZ_CTRL_MOVE_RIGHT\n");
                        break;
                        // ...
                }
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg(" DANAVIDEOCMD_PTZCTRL send response succeeded\n");
                } else {
                   dbg(" DANAVIDEOCMD_PTZCTRL send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_SDCFORMAT:
            {
               dbg("DANAVIDEOCMD_SDCFORMAT\n");
                DANAVIDEOCMD_SDCFORMAT_ARG *sdcformat_arg = (DANAVIDEOCMD_SDCFORMAT_ARG *)cmd_arg; 
               dbg("sdcformat_arg\n");
               dbg("ch_no: %"PRIu32"\n", sdcformat_arg->ch_no);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_SDCFORMAT send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_SDCFORMAT send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_SETALARM:
            {
               dbg("DANAVIDEOCMD_SETALARM\n");
                DANAVIDEOCMD_SETALARM_ARG *setalarm_arg = (DANAVIDEOCMD_SETALARM_ARG *)cmd_arg;
               dbg("setalarm_arg\n");
               dbg("ch_no: %"PRIu32"\n", setalarm_arg->ch_no);
               dbg("motion_detection: %"PRIu32"\n", setalarm_arg->motion_detection);
               dbg("opensound_detection: %"PRIu32"\n", setalarm_arg->opensound_detection);
               dbg("openi2o_detection: %"PRIu32"\n", setalarm_arg->openi2o_detection);
               dbg("smoke_detection: %"PRIu32"\n", setalarm_arg->smoke_detection);
               dbg("shadow_detection: %"PRIu32"\n", setalarm_arg->shadow_detection);
               dbg("other_detection: %"PRIu32"\n", setalarm_arg->other_detection);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_SETALARM send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_SETALARM send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_SETCHAN:
            {
               dbg("DANAVIDEOCMD_SETCHAN\n");
                DANAVIDEOCMD_SETCHAN_ARG *setchan_arg = (DANAVIDEOCMD_SETCHAN_ARG *)cmd_arg; 
               dbg("setchan_arg\n");
               dbg("ch_no: %"PRIu32"\n", setchan_arg->ch_no);
               dbg("chans_count: %zd\n", setchan_arg->chans_count);
                for (i=0; i<setchan_arg->chans_count; i++) {
                    dbg("chans[%"PRIu32"]: %"PRIu32"\n", i, setchan_arg->chans[i]);
                }
                dbg("\n");
                error_code = 0;
                code_msg = (char *)"";

                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_SETCHAN send response succeeded\n");
                } else {
                    dbg("DANAVIDEOCMD_SETCHAN send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_SETCOLOR:
            {
               dbg(" DANAVIDEOCMD_SETCOLOR\n"); 
                DANAVIDEOCMD_SETCOLOR_ARG *setcolor_arg = (DANAVIDEOCMD_SETCOLOR_ARG *)cmd_arg;
               dbg("setcolor_arg\n");
               dbg("ch_no: %"PRIu32"\n", setcolor_arg->ch_no);
               dbg("video_rate: %"PRIu32"\n", setcolor_arg->video_rate);
               dbg("brightness: %"PRIu32"\n", setcolor_arg->brightness);
               dbg("contrast: %"PRIu32"\n", setcolor_arg->contrast);
               dbg("saturation: %"PRIu32"\n", setcolor_arg->saturation);
               dbg("hue: %"PRIu32"\n", setcolor_arg->hue);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_SETCOLOR send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_SETCOLOR send response failed\n");
                }
            }
            break;
        case DANAVIDEOCMD_SETFLIP:
            {
               dbg("danavideoconn_command_handler DANAVIDEOCMD_SETFLIP\n"); 
                DANAVIDEOCMD_SETFLIP_ARG *setflip_arg = (DANAVIDEOCMD_SETFLIP_ARG *)cmd_arg;
               dbg("setflip_arg\n");
               dbg("ch_no: %"PRIu32"\n", setflip_arg->ch_no);
               dbg("flip_type: %"PRIu32"\n", setflip_arg->flip_type);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg("danavideoconn_command_handler DANAVIDEOCMD_SETFLIP send response succeeded\n");
                } else {
                   dbg("danavideoconn_command_handler DANAVIDEOCMD_SETFLIP send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_SETNETINFO:
            {
               dbg("DANAVIDEOCMD_SETNETINFO\n"); 
                DANAVIDEOCMD_SETNETINFO_ARG *setnetinfo_arg = (DANAVIDEOCMD_SETNETINFO_ARG *)cmd_arg;
               dbg("setnetinfo_arg\n");
               dbg("ch_no: %"PRIu32"\n", setnetinfo_arg->ch_no);
               dbg("ip_type: %"PRIu32"\n", setnetinfo_arg->ip_type);
               dbg("ipaddr: %s\n", setnetinfo_arg->ipaddr);
               dbg("netmask: %s\n", setnetinfo_arg->netmask);
               dbg("gateway: %s\n", setnetinfo_arg->gateway);
               dbg("dns_type: %"PRIu32"\n", setnetinfo_arg->dns_type);
               dbg("dns_name1: %s\n", setnetinfo_arg->dns_name1);
               dbg("dns_name2: %s\n", setnetinfo_arg->dns_name2);
               dbg("http_port: %"PRIu32"\n", setnetinfo_arg->http_port);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_SETNETINFO send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_SETNETINFO send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_SETPOWERFREQ:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETPOWERFREQ\n"); 
                DANAVIDEOCMD_SETPOWERFREQ_ARG *setpowerfreq_arg = (DANAVIDEOCMD_SETPOWERFREQ_ARG *)cmd_arg;
                dbg("setpowerfreq_arg\n");
                dbg("ch_no: %"PRIu32"\n", setpowerfreq_arg->ch_no);
                if (DANAVIDEO_POWERFREQ_50HZ == setpowerfreq_arg->freq) {
                    dbg("freq: DANAVIDEO_POWERFREQ_50HZ\n");
                } else if (DANAVIDEO_POWERFREQ_60HZ == setpowerfreq_arg->freq) {
                    dbg("freq: DANAVIDEO_POWERFREQ_60HZ\n");
                } else {
                    dbg("UnKnown freq: %"PRIu32"\n", setpowerfreq_arg->freq);
                }
                dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETPOWERFREQ send response succeeded\n");
                } else {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETPOWERFREQ send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_SETTIME:
            {
               dbg("DANAVIDEOCMD_SETTIME\n"); 
                DANAVIDEOCMD_SETTIME_ARG *settime_arg = (DANAVIDEOCMD_SETTIME_ARG *)cmd_arg;
               dbg("settime_arg\n");
               dbg("ch_no: %"PRIu32"\n", settime_arg->ch_no);
               dbg("now_time: %"PRId64"\n", settime_arg->now_time);
               dbg("time_zone: %s\n", settime_arg->time_zone);
                if (settime_arg->has_ntp_server1) {
                   dbg("ntp_server_1: %s\n", settime_arg->ntp_server1);
                }
                if (settime_arg->has_ntp_server2) {
                   dbg("ntp_server_2: %s\n", settime_arg->ntp_server2);
                }
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_SETTIME send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_SETTIME send response failed\n");
                }
            }
            break;
        case DANAVIDEOCMD_SETVIDEO:
            {
                
               dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETVIDEO\n"); 
                DANAVIDEOCMD_SETVIDEO_ARG *setvideo_arg = (DANAVIDEOCMD_SETVIDEO_ARG *)cmd_arg;
               dbg("setvideo_arg\n");
               dbg("ch_no: %"PRIu32"\n", setvideo_arg->ch_no);
               dbg("video_quality: %"PRIu32"\n", setvideo_arg->video_quality);
               dbg("\n"); 

			   
                if (setvideo_arg->video_quality < 56)
				{
				    if(g_StreamType!=E_STREAM_ID_PREVIEW)
					{
					   stop_preview_cmd();
					}
					start_preview_cmd(setvideo_arg->video_quality);
                } else 
                {
                    if(g_StreamType!=E_STREAM_ID_MJPEG)
					{
					  stop_preview_cmd();
					  start_preview_cmd(setvideo_arg->video_quality);
					}
                }
                 
                error_code = 0;
                code_msg = (char *)"";
                uint32_t set_video_fps = 30;
                if (lib_danavideo_cmd_setvideo_response(danavideoconn, trans_id, error_code, code_msg, set_video_fps)) {
                   dbg(" DANAVIDEOCMD_SETVIDEO send response succeeded\n");
                } else {
                   dbg(" DANAVIDEOCMD_SETVIDEO send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_SETWIFIAP:
            {
               dbg("DANAVIDEOCMD_SETWIFIAP\n"); 
                DANAVIDEOCMD_SETWIFIAP_ARG *setwifiap_arg = (DANAVIDEOCMD_SETWIFIAP_ARG *)cmd_arg;
               dbg("setwifiap_arg\n");
               dbg("ch_no: %"PRIu32"\n", setwifiap_arg->ch_no);
               dbg("ip_type: %"PRIu32"\n", setwifiap_arg->ip_type);
               dbg("ipaddr: %s\n", setwifiap_arg->ipaddr);
               dbg("netmask: %s\n", setwifiap_arg->netmask);
               dbg("gateway: %s\n", setwifiap_arg->gateway);
               dbg("dns_name1: %s\n", setwifiap_arg->dns_name1);
               dbg("dns_name2: %s\n", setwifiap_arg->dns_name2);
               dbg("essid: %s\n", setwifiap_arg->essid);
               dbg("auth_key: %s\n", setwifiap_arg->auth_key);
               dbg("enc_type: %"PRIu32"\n", setwifiap_arg->enc_type);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_SETWIFIAP send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_SETWIFIAP send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_SETWIFI:
            {
               dbg("DANAVIDEOCMD_SETWIFI\n");
                DANAVIDEOCMD_SETWIFI_ARG *setwifi_arg = (DANAVIDEOCMD_SETWIFI_ARG *)cmd_arg; 
               dbg("setwifi_arg\n");
               dbg("ch_no: %"PRIu32"\n", setwifi_arg->ch_no);
               dbg("essid: %s\n", setwifi_arg->essid);
               dbg("auth_key: %s\n", setwifi_arg->auth_key);
               dbg("enc_type: %"PRIu32"\n", setwifi_arg->enc_type);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_SETWIFI send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_SETWIFI send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_STARTAUDIO:
            {
               dbg("----DANAVIDEOCMD_STARTAUDIO\n"); 
                DANAVIDEOCMD_STARTAUDIO_ARG *startaudio_arg = (DANAVIDEOCMD_STARTAUDIO_ARG *)cmd_arg;
               dbg("startaudio_arg\n");
               dbg("ch_no: %"PRIu32"\n", startaudio_arg->ch_no);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";

                uint32_t audio_codec = G711A; // danavidecodec_t
                uint32_t sample_rate = 48000; // 单位Hz
                uint32_t sample_bit  = 16; // 单位bit
                uint32_t track = 2; // (1 mono; 2 stereo)

                if (lib_danavideo_cmd_startaudio_response(danavideoconn, trans_id, error_code, code_msg, NULL, &sample_rate, NULL, &track)) {
                   dbg(" DANAVIDEOCMD_STARTAUDIO send response succeeded\n");
                } else {
                   dbg(" DANAVIDEOCMD_STARTAUDIO send response failed\n");
                }
                // 开启音频生产者线程
                if (mydata->run_audio_media) {
                   dbg("audio ch_no[%u] is already started, enjoy\n", startaudio_arg->ch_no);
                } else {
                    mydata->run_audio_media = true;
                    mydata->exit_audio_media = false;
                    mydata->chan_no = startaudio_arg->ch_no;
                    if (0 != pthread_create(&(mydata->thread_audio_media), NULL, &th_audio_media, mydata)) {
                        memset(&(mydata->thread_audio_media), 0, sizeof(mydata->thread_audio_media));
                       dbg("---- pthread_create th_audio_media failed\n");
                        // 发送无法开启视频线程到对端
                    } else {
                       dbg("---- th_audio_media is started, enjoy!\n");
                    }
                }
            }
            break;
        case DANAVIDEOCMD_STARTTALKBACK:
            {
               dbg("DANAVIDEOCMD_STARTTALKBACK\n"); 
                DANAVIDEOCMD_STARTTALKBACK_ARG *starttalkback_arg = (DANAVIDEOCMD_STARTTALKBACK_ARG *)cmd_arg;
               dbg("starttalkback_arg\n");
               dbg("ch_no: %"PRIu32"\n", starttalkback_arg->ch_no);
               dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                uint32_t audio_codec = G711A;
                if (lib_danavideo_cmd_starttalkback_response(danavideoconn, trans_id, error_code, code_msg, audio_codec)) {
                   dbg("DANAVIDEOCMD_STARTTALKBACK send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_STARTTALKBACK send response failed\n");
                }
                // 开启读取音频数据的线程
                if (mydata->run_talkback) {
                   dbg("ch_no[%u] is already started, enjoy\n", starttalkback_arg->ch_no);
                } else {
                    mydata->run_talkback = true;
                    mydata->exit_talback = false;
                    if (0 != pthread_create(&(mydata->thread_talkback), NULL, &th_talkback, mydata)) {
                        memset(&(mydata->thread_talkback), 0, sizeof(mydata->thread_talkback));
                       dbg("pthread_create th_talkback failed\n"); 
                    } else {
                       dbg("th_talkback is started, enjoy!\n");
                    }
                }
            }
            break; 
        case DANAVIDEOCMD_STARTVIDEO:
            {
                DANAVIDEOCMD_STARTVIDEO_ARG *startvideo_arg = (DANAVIDEOCMD_STARTVIDEO_ARG *)cmd_arg;
                dbg("startvideo_arg\n");
                dbg("ch_no: %"PRIu32"\n", startvideo_arg->ch_no);
                dbg("client_type: %"PRIu32"\n", startvideo_arg->client_type);
               dbg("DANAVIDEOCMDSTARTVIDEO video_quality: %"PRIu32"\n", startvideo_arg->video_quality);
                dbg("vstrm: %"PRIu32"\n", startvideo_arg->vstrm);
                dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                uint32_t start_video_fps = 30;
				#ifdef PLAYBACK_RECODE
                	if(g_CaseType ==CASE_PLAYBACK)
					{
					  start_record_cmd();
					}
					g_CaseType =CASE_PREVIEW;
                #endif
                // 开启视频生产者线程
                if (mydata->run_media) {
                   dbg("ch_no[%u] is already started, enjoy\n", startvideo_arg->ch_no);
                } else {
                    start_preview_cmd(startvideo_arg->video_quality);
                    mydata->run_media = true;
                    mydata->exit_media = false;
                    mydata->chan_no = startvideo_arg->ch_no;   
                    gIsInPreview = 1;
                    if (0 != pthread_create(&(mydata->thread_media), NULL, &th_media, mydata)) {
                        memset(&(mydata->thread_media), 0, sizeof(mydata->thread_media));
                        dbg("danavideoconn_command_handler pthread_create th_media failed\n");
                        // 发送无法开启视频线程到对端
                        mydata->run_media = false;
                        mydata->exit_media = true;
                        gIsInPreview = 0;
						stop_preview_cmd();
                    } else {
                        dbg("danavideoconn_command_handler th_media is started, enjoy!\n");
                    }
                }


                if (lib_danavideo_cmd_startvideo_response(danavideoconn, trans_id, error_code, code_msg, start_video_fps)) {
                   dbg(" DANAVIDEOCMDSTARTVIDEO send response succeeded\n");
                } else {
                   dbg(" DANAVIDEOCMDSTARTVIDEO send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_STOPAUDIO:
            {
               dbg("--- DANAVIDEOCMD_STOPAUDIO\n");
                DANAVIDEOCMD_STOPAUDIO_ARG *stopaudio_arg = (DANAVIDEOCMD_STOPAUDIO_ARG *)cmd_arg;
               dbg("stopaudio_arg\n");
               dbg("ch_no: %"PRIu32"\n", stopaudio_arg->ch_no);
               dbg("\n");
                // 关闭音频生产者线程
                dbg("TEST danavideoconn_command_handler stop th_audio_media\n");
                mydata->run_audio_media = false;
                
                if (0 != mydata->thread_audio_media) {
                    if (0 != pthread_join(mydata->thread_audio_media, NULL)) {
                        perror("danavideoconn_command_handler thread_audio_media join faild!\n");
                    }
                }
                memset(&(mydata->thread_audio_media), 0, sizeof(mydata->thread_audio_media));
                
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg("---- DANAVIDEOCMD_STOPAUDIO send response succeeded\n");
                } else {
                   dbg("==== DANAVIDEOCMD_STOPAUDIO send response failed\n");
                }
            }
            break; 
        case DANAVIDEOCMD_STOPTALKBACK:
            {
               dbg("----- DANAVIDEOCMD_STOPTALKBACK\n");
                DANAVIDEOCMD_STOPTALKBACK_ARG *stoptalkback_arg = (DANAVIDEOCMD_STOPTALKBACK_ARG *)cmd_arg; 
               dbg("stoptalkback_arg\n");
               dbg("ch_no: %"PRIu32"\n", stoptalkback_arg->ch_no);
               dbg("\n");
                // 关闭音频读取线程
                dbg("TEST danavideoconn_command_handler stop th_talkback\n");
                mydata->run_talkback = false;
                if (0 != mydata->thread_talkback) {
                    if (0 != pthread_join(mydata->thread_talkback, NULL)) {
                        perror("danavideoconn_command_handler thread_talkback join faild!\n");
                    }
                }
                memset(&(mydata->thread_talkback), 0, sizeof(mydata->thread_talkback));
                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg("---- DANAVIDEOCMD_STOPTALKBACK send response succeeded\n");
                } else {
                   dbg("==== DANAVIDEOCMD_STOPTALKBACK send response failed\n");
                }
            }
            break;
        case DANAVIDEOCMD_STOPVIDEO:
            {
               dbg("danavideoconn_command_handler DANAVIDEOCMDSTOPVIDEO\n");
                DANAVIDEOCMD_STOPVIDEO_ARG *stopvideo_arg = (DANAVIDEOCMD_STOPVIDEO_ARG *)cmd_arg;
                dbg("stopvideo_arg\n");
               dbg("ch_no: %"PRIu32"\n", stopvideo_arg->ch_no);
                dbg("\n"); 
                // 关闭视频生产者线程
 
                dbg("danavideoconn_command_handler stop th_media\n");
                mydata->run_media = false;
               
                if (0 != mydata->thread_media) {
                    if (0 != pthread_join(mydata->thread_media, NULL)) {
                        perror("danavideoconn_command_handler thread_media join faild!\n");
                    }
                }
				if(gIsInPreview==1)
				{
				  stop_preview_cmd();
				}
				gIsInPreview = 0;
                memset(&(mydata->thread_media), 0, sizeof(mydata->thread_media));

                error_code = 0;
                code_msg = (char *)"";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                   dbg("danavideoconn_command_handler DANAVIDEOCMDSTOPVIDEO send response succeeded\n");
                } else {

                   dbg("danavideoconn_command_handler DANAVIDEOCMDSTOPVIDEO send response failed\n");
                } 
            }
            break;
        case DANAVIDEOCMD_RECLIST:
            {
                handleRecListReq(arg, cmd, trans_id, cmd_arg, cmd_arg_len); 
            }
            break;
        case DANAVIDEOCMD_RECPLAY:
            {
				updateList();
               dbg("danavideoconn_command_handler DANAVIDEOCMD_RECPLAY\n");
                DANAVIDEOCMD_RECPLAY_ARG *recplay_arg = (DANAVIDEOCMD_RECPLAY_ARG *)cmd_arg;
               dbg("ch_no: %"PRIu32"\n", recplay_arg->ch_no);
               dbg("time_stamp: %"PRId64"\n", recplay_arg->time_stamp);

                {
                    int64_t fromT = recplay_arg->time_stamp;
                    char sbuf[128];
                    int isPicture =0;
                    char videoFileName[256]={0};
                    
                    fromT += 8 * 60 * 60;
        
                    struct tm* pTm = localtime(&fromT);        
                    if (!pTm) return;
        
                    snprintf(sbuf, 128-1, "%04d_%02d%02d_%02d%02d%02d",
                               (pTm->tm_year+1900),(pTm->tm_mon+1),pTm->tm_mday,pTm->tm_hour,pTm->tm_min,pTm->tm_sec);
                   dbg("***Mason: play request file:%s\n",sbuf); 
                    if (0 == findPlaybackFile(sbuf, videoFileName,recplay_arg->ch_no-1)) {
                       dbg("***Mason: ERROR: find playback fail.\n");
                        return;
                    }

                    snprintf(playbackRecFile, 100-1, "%s/%s", "/mnt/mmc/DCIM/100CVR", videoFileName);
                    //strcpy(playInfo->videoFileName,sbuf);
                   dbg("***Mason: to play videoFileName=%s .....\n",playbackRecFile);

                   dbg("*** record play, run media:%d, exit:%d, old ch:%d, rec ch:%d\n",
                        mydata->run_media, mydata->exit_media, 
                        mydata->chan_no, recplay_arg->ch_no);
						#ifdef PLAYBACK_RECODE
                    if(g_CaseType ==CASE_PREVIEW)
					{
					  stop_record_cmd();
					}
					g_CaseType =CASE_PLAYBACK;
					#endif
                    if (mydata->run_media) {
                       dbg("ch_no[%u] is already started, enjoy\n", recplay_arg->ch_no);
								mydata->run_media = false;
               
			                if (0 != mydata->thread_media) {
			                    if (0 != pthread_join(mydata->thread_media, NULL)) {
			                        perror("thread_media join faild!\n");
			                    }
			                }
						
							if(gIsInPreview == 1)
							{
							   stop_preview_cmd();
							}
							#ifdef PLAYBACK_RECODE
							else if(gIsInPreview == 2)
							{
							  	unsigned int enable=0;
			                	char buf[IPC_MBX_MAX_SIZE];
					            snprintf(buf, IPC_MBX_MAX_SIZE, "%d|%s", enable, "no.mp4");
					           //AV_OSA_ipcMbxSendStr(IPC_MSG_AV_SERVER,IPC_MSG_TUTK, IPC_MSG_CMD_AV_SVR_PLAYBACK, buf);
			                }
							#endif
							gIsInPreview = 0;
			                memset(&(mydata->thread_media), 0, sizeof(mydata->thread_media));
                    } 
				
					 #ifdef PLAYBACK_RECODE
                        unsigned int enable=1;
	                    char buf[IPC_MBX_MAX_SIZE];
				        snprintf(buf, IPC_MBX_MAX_SIZE, "%d|%s", enable, videoFileName);
				        //AV_OSA_ipcMbxSendStr(IPC_MSG_AV_SERVER,IPC_MSG_TUTK, IPC_MSG_CMD_AV_SVR_PLAYBACK, buf);
	                    gIsInPreview = 2;
					 #endif
                        mydata->run_media = true;
                        mydata->exit_media = false;
                        mydata->chan_no = recplay_arg->ch_no;
                        if (0 != pthread_create(&(mydata->thread_media), NULL, &th_media, mydata)) {
                            memset(&(mydata->thread_media), 0, sizeof(mydata->thread_media));
                           dbg("danavideoconn_command_handler pthread_create th_media failed\n");
                            // 发送无法开启视频线程到对端
                            mydata->run_media = false;
                            mydata->exit_media = true;
							gIsInPreview = 0;
                        } else {
                           dbg("danavideoconn_command_handler th_media is started, enjoy!\n");
                        }
                    
                }
                
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, "", trans_id, error_code, code_msg)) {
                   dbg("danavideoconn_command_handler DANAVIDEOCMD_RECPLAY send response succeeded\n");
                } else {
                   dbg("danavideoconn_command_handler DANAVIDEOCMD_RECPLAY send response failed\n");
                } 
            }
            break;
        case DANAVIDEOCMD_RECSTOP:
            {
               dbg("DANAVIDEOCMD_RECSTOP\n");
                DANAVIDEOCMD_RECSTOP_ARG *recstop_arg = (DANAVIDEOCMD_RECSTOP_ARG *)cmd_arg;
               dbg("ch_no: %"PRIu32"\n", recstop_arg->ch_no);
				mydata->run_media = false;
               
                if (0 != mydata->thread_media) {
                    if (0 != pthread_join(mydata->thread_media, NULL)) {
                        perror("DANAVIDEOCMD_RECSTOP thread_media join faild!\n");
                    }
                }
				#ifdef PLAYBACK_RECODE
				unsigned int enable=0;
                char buf[IPC_MBX_MAX_SIZE];
		        snprintf(buf, IPC_MBX_MAX_SIZE, "%d|%s", enable, "no.mp4");
				if(gIsInPreview ==2)
		        //AV_OSA_ipcMbxSendStr(IPC_MSG_AV_SERVER,IPC_MSG_TUTK, IPC_MSG_CMD_AV_SVR_PLAYBACK, buf);
                #endif
				gIsInPreview = 0;
                memset(&(mydata->thread_media), 0, sizeof(mydata->thread_media));
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, "", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_RECSTOP send response succeeded\n");
                } else {

                   dbg("DANAVIDEOCMD_RECSTOP send response failed\n");
                }
            } 
            break;
        case DANAVIDEOCMD_RECACTION:
            {
               dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_RECACTION\n");
                DANAVIDEOCMD_RECACTION_ARG *recaction_arg = (DANAVIDEOCMD_RECACTION_ARG *)cmd_arg;
               dbg("recaction_arg\n");
               dbg("ch_no: %"PRIu32"\n", recaction_arg->ch_no);
                if (DANAVIDEO_REC_ACTION_PAUSE == recaction_arg->action) {
                   dbg("action: DANAVIDEO_REC_ACTION_PAUSE\n");
                } else if (DANAVIDEO_REC_ACTION_PLAY == recaction_arg->action) {
                   dbg("action: DANAVIDEO_REC_ACTION_PLAY\n");
                } else {
                   dbg("Unknown action: %"PRIu32"\n", recaction_arg->action);
                }
               dbg("\n"); 
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, "", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_RECACTION send response succeeded\n");
                } else {

                   dbg("DANAVIDEOCMD_RECACTION send response failed\n");
                } 
            }
            break;
        case DANAVIDEOCMD_RECSETRATE:
            {
               dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_RECSETRATE\n");
                DANAVIDEOCMD_RECSETRATE_ARG *recsetrate_arg = (DANAVIDEOCMD_RECSETRATE_ARG *)cmd_arg;
               dbg("recsetrate_arg\n");
               dbg("ch_no: %"PRIu32"\n", recsetrate_arg->ch_no);
                if (DANAVIDEO_REC_RATE_HALF == recsetrate_arg->rec_rate) {
                   dbg("rec_rate: DANAVIDEO_REC_RATE_HALF\n");
                } else if (DANAVIDEO_REC_RATE_NORMAL == recsetrate_arg->rec_rate) {
                   dbg("rec_rate: DANAVIDEO_REC_RATE_NORMAL\n");
                } else if (DANAVIDEO_REC_RATE_DOUBLE == recsetrate_arg->rec_rate) {
                   dbg("rec_rate: DANAVIDEO_REC_RATE_DOUBLE\n");
                } else if (DANAVIDEO_REC_RATE_FOUR == recsetrate_arg->rec_rate) {
                   dbg("rec_rate: DANAVIDEO_REC_RATE_FOUR\n");
                } else {
                   dbg("Unknown rec_rate: %"PRIu32"\n", recsetrate_arg->rec_rate);
                }
               dbg("\n"); 
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, "", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_RECSETRATE send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_RECSETRATE send response failed\n");
                } 
            }
            break;
        case DANAVIDEOCMD_RECPLANGET:
            {
               dbg("DANAVIDEOCMD_RECPLANGET\n");
                DANAVIDEOCMD_RECPLANGET_ARG *recplanget_arg = (DANAVIDEOCMD_RECPLANGET_ARG *)cmd_arg;
               dbg("recplanget_arg\n");
               dbg("ch_no: %"PRIu32"\n", recplanget_arg->ch_no);
               dbg("\n"); 
                error_code = 0;
                code_msg = "";
                uint32_t rec_plans_count = 2;
                libdanavideo_recplanget_recplan_t rec_plans[] = {{0, 2, {DANAVIDEO_REC_WEEK_MON, DANAVIDEO_REC_WEEK_SAT}, "12:23:34", "15:56:01", DANAVIDEO_REC_PLAN_OPEN}, {1, 3, {DANAVIDEO_REC_WEEK_MON, DANAVIDEO_REC_WEEK_SAT, DANAVIDEO_REC_WEEK_SUN}, "22:23:24", "23:24:25", DANAVIDEO_REC_PLAN_CLOSE}};
                if (lib_danavideo_cmd_recplanget_response(danavideoconn, trans_id, error_code, code_msg, rec_plans_count, rec_plans)) {
                   dbg("DANAVIDEOCMD_RECPLANGET send response succeeded\n");
                } else {

                   dbg("DANAVIDEOCMD_RECPLANGET send response failed\n");
                }
            } 
            break;
        case DANAVIDEOCMD_RECPLANSET:
            {
               dbg("DANAVIDEOCMD_RECPLANSET\n");
                DANAVIDEOCMD_RECPLANSET_ARG *recplanset_arg = (DANAVIDEOCMD_RECPLANSET_ARG *)cmd_arg;
               dbg("recplanset_arg\n");
               dbg("ch_no: %"PRIu32"\n", recplanset_arg->ch_no);
               dbg("record_no: %"PRIu32"\n", recplanset_arg->record_no);
                size_t i;
                for (i=0; i<recplanset_arg->week_count; i++) {
                    if (DANAVIDEO_REC_WEEK_MON == recplanset_arg->week[i]) {
                       dbg("week: DANAVIDEO_REC_WEEK_MON\n");
                    } else if (DANAVIDEO_REC_WEEK_TUE == recplanset_arg->week[i]) {
                       dbg("week: DANAVIDEO_REC_WEEK_TUE\n");
                    } else if (DANAVIDEO_REC_WEEK_WED == recplanset_arg->week[i]) {
                       dbg("week: DANAVIDEO_REC_WEEK_WED\n");
                    } else if (DANAVIDEO_REC_WEEK_THU == recplanset_arg->week[i]) {
                       dbg("week: DANAVIDEO_REC_WEEK_THU\n");
                    } else if (DANAVIDEO_REC_WEEK_FRI == recplanset_arg->week[i]) {
                       dbg("week: DANAVIDEO_REC_WEEK_FRI\n");
                    } else if (DANAVIDEO_REC_WEEK_SAT == recplanset_arg->week[i]) {
                       dbg("week: DANAVIDEO_REC_WEEK_SAT\n");
                    } else if (DANAVIDEO_REC_WEEK_SUN == recplanset_arg->week[i]) {
                       dbg("week: DANAVIDEO_REC_WEEK_SUN\n");
                    } else {
                       dbg("Unknown week: %"PRIu32"\n", recplanset_arg->week[i]);
                    } 
                } 
               dbg("start_time: %s\n", recplanset_arg->start_time);
               dbg("end_time: %s\n", recplanset_arg->end_time);
               dbg("status: %"PRIu32"\n", recplanset_arg->status);
               dbg("\n"); 
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, "", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_RECPLANSET send response succeeded\n");
                } else {

                   dbg("DANAVIDEOCMD_RECPLANSET send response failed\n");
                } 
            }
            break;
        case DANAVIDEOCMD_EXTENDMETHOD:
            {
               dbg("DANAVIDEOCMD_EXTENDMETHOD\n");
                DANAVIDEOCMD_EXTENDMETHOD_ARG *extendmethod_arg = (DANAVIDEOCMD_EXTENDMETHOD_ARG *)cmd_arg;
               dbg("extendmethod_arg\n");
               dbg("ch_no: %"PRIu32"\n", extendmethod_arg->ch_no);
               dbg("extend_data_size: %zd\n", extendmethod_arg->extend_data.size);
                // extend_data_bytes access via extendmethod_arg->extend_data.bytes
               dbg("\n"); 
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, "", trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_EXTENDMETHOD send response succeeded\n");
                } else {

                   dbg("DANAVIDEOCMD_EXTENDMETHOD send response failed\n");
                }
            } 
            break;
        case DANAVIDEOCMD_SETOSD:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETOSD\n");
                DANAVIDEOCMD_SETOSD_ARG *setosd_arg = (DANAVIDEOCMD_SETOSD_ARG *)cmd_arg;
                dbg("ch_no: %"PRIu32"\n", setosd_arg->ch_no);
                dbg("osd_info:\n");
                if (DANAVIDEO_OSD_SHOW_OPEN == setosd_arg->osd.chan_name_show) {
                    dbg("chan_name_show OPEN\n");
                    if (setosd_arg->osd.has_show_name_x) {
                        dbg("show_name_x: %"PRIu32"\n", setosd_arg->osd.show_name_x);
                    }
                    if (setosd_arg->osd.has_show_name_y) {
                        dbg("show_name_y: %"PRIu32"\n", setosd_arg->osd.show_name_y);
                    } 
                } else if (DANAVIDEO_OSD_SHOW_CLOSE == setosd_arg->osd.chan_name_show) {
                    dbg("chan_name_show CLOSE\n");
                } else {
                    dbg("chan_name_show unknown type[%"PRIu32"]\n", setosd_arg->osd.chan_name_show);
                }
                if (DANAVIDEO_OSD_SHOW_OPEN == setosd_arg->osd.datetime_show) {
                    dbg("datetime_show OPEN\n");
                    if (setosd_arg->osd.has_show_datetime_x) {
                        dbg("show_datetime_x: %"PRIu32"\n", setosd_arg->osd.show_datetime_x);
                    }
                    if (setosd_arg->osd.has_show_datetime_y) {
                        dbg("show_datetime_y: %"PRIu32"\n", setosd_arg->osd.show_datetime_y);
                    }
                    if (setosd_arg->osd.has_show_format) {
                        dbg("show_format:\n");
                        switch (setosd_arg->osd.show_format) {
                            case DANAVIDEO_OSD_DATE_FORMAT_YYYY_MM_DD:
                                dbg("DANAVIDEO_OSD_DATE_FORMAT_YYYY_MM_DD\n");
                                break;
                            case DANAVIDEO_OSD_DATE_FORMAT_MM_DD_YYYY:
                                dbg("DANAVIDEO_OSD_DATE_FORMAT_MM_DD_YYYY\n");
                                break;
                            case DANAVIDEO_OSD_DATE_FORMAT_YYYY_MM_DD_CH:
                                dbg("DANAVIDEO_OSD_DATE_FORMAT_YYYY_MM_DD_CH\n");
                                break;
                            case DANAVIDEO_OSD_DATE_FORMAT_MM_DD_YYYY_CH:
                                dbg("DANAVIDEO_OSD_DATE_FORMAT_MM_DD_YYYY_CH\n");
                                break;
                            case DANAVIDEO_OSD_DATE_FORMAT_DD_MM_YYYY:
                                dbg("DANAVIDEO_OSD_DATE_FORMAT_DD_MM_YYYY\n");
                                break;
                            case DANAVIDEO_OSD_DATE_FORMAT_DD_MM_YYYY_CH:
                                dbg("DANAVIDEO_OSD_DATE_FORMAT_DD_MM_YYYY_CH\n");
                                break;
                            default:
                                dbg("DANAVIDEO_OSD_DATE_FORMAT_XXXX\n");
                                break;
                        }
                    }
                    if (setosd_arg->osd.has_hour_format) {
                        dbg("hour_format:\n");
                        switch (setosd_arg->osd.hour_format) {
                            case DANAVIDEO_OSD_TIME_24_HOUR:
                                dbg("DANAVIDEO_OSD_TIME_24_HOUR\n");
                                break;
                            case DANAVIDEO_OSD_TIME_12_HOUR:
                                dbg("DANAVIDEO_OSD_TIME_12_HOUR\n");
                                break;
                            default:
                                dbg("DANAVIDEO_OSD_TIME_XXXX\n");
                                break;
                        }
                    }
                    if (setosd_arg->osd.has_show_week) {
                        dbg("show_week:\n");
                        switch (setosd_arg->osd.show_week) {
                            case DANAVIDEO_OSD_SHOW_CLOSE:
                                dbg("DANAVIDEO_OSD_SHOW_CLOSE\n");
                                break;
                            case DANAVIDEO_OSD_SHOW_OPEN:
                                dbg("DANAVIDEO_OSD_SHOW_OPEN\n");
                                break;
                            default:
                                dbg("DANAVIDEO_OSD_SHOW_XXXX\n");
                                break;
                        }
                    }
                    if (setosd_arg->osd.has_datetime_attr) {
                        dbg("datetime_attr:\n");
                        switch (setosd_arg->osd.datetime_attr) {
                            case DANAVIDEO_OSD_DATETIME_TRANSPARENT:
                                dbg("DANAVIDEO_OSD_DATETIME_TRANSPARENT\n");
                                break;
                            case DANAVIDEO_OSD_DATETIME_DISPLAY:
                                dbg("DANAVIDEO_OSD_DATETIME_DISPLAY\n");
                                break;
                            default:
                                dbg("DANAVIDEO_OSD_DATETIME_XXXX\n");
                                break;
                        }
                    }
                } else if (DANAVIDEO_OSD_SHOW_CLOSE == setosd_arg->osd.datetime_show) {
                    dbg("datetime_show CLOSE\n");
                } else {
                    dbg("datetime_show unknown type[%"PRIu32"]\n", setosd_arg->osd.datetime_show);
                }
                if (DANAVIDEO_OSD_SHOW_OPEN == setosd_arg->osd.custom1_show) {
                    dbg("custom1_show OPEN\n");
                    if (setosd_arg->osd.has_show_custom1_str) {
                        dbg("show_custom1_str: %s\n", setosd_arg->osd.show_custom1_str);
                    }
                    if (setosd_arg->osd.has_show_custom1_x) {
                        dbg("show_custom1_x: %"PRIu32"\n", setosd_arg->osd.show_custom1_x);
                    }
                    if (setosd_arg->osd.has_show_custom1_y) {
                        dbg("show_custom1_y: %"PRIu32"\n", setosd_arg->osd.show_custom1_y);
                    }
                } else if (DANAVIDEO_OSD_SHOW_CLOSE == setosd_arg->osd.custom1_show) {
                    dbg("custom1_show CLOSE\n");
                } else {
                    dbg("custom1_show unknown type[%"PRIu32"]\n", setosd_arg->osd.custom1_show);
                }
                if (DANAVIDEO_OSD_SHOW_OPEN == setosd_arg->osd.custom2_show) {
                    dbg("custom2_show OPEN\n");
                    if (setosd_arg->osd.has_show_custom2_str) {
                        dbg("show_custom2_str: %s\n", setosd_arg->osd.show_custom2_str);
                    }
                    if (setosd_arg->osd.has_show_custom2_x) {
                        dbg("show_custom2_x: %"PRIu32"\n", setosd_arg->osd.show_custom2_x);
                    }
                    if (setosd_arg->osd.has_show_custom2_y) {
                        dbg("show_custom2_y: %"PRIu32"\n", setosd_arg->osd.show_custom2_y);
                    }
                } else if (DANAVIDEO_OSD_SHOW_CLOSE == setosd_arg->osd.custom2_show) {
                    dbg("custom2_show CLOSE\n");
                } else {
                    dbg("custom2_show unknown type[%"PRIu32"]\n", setosd_arg->osd.custom2_show);
                }
                //
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, "", trans_id, error_code, code_msg)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETOSD send response succeeded\n");
                } else {

                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETOSD send response failed\n");
                }
            }
            break;
        case DANAVIDEOCMD_GETOSD:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETOSD\n");
                DANAVIDEOCMD_GETOSD_ARG *getosd_arg = (DANAVIDEOCMD_GETOSD_ARG *)cmd_arg;
                dbg("ch_no: %"PRIu32"\n", getosd_arg->ch_no);
                dbg("\n");
                error_code = 0;
                code_msg = (char *)"";
                libdanavideo_osdinfo_t osdinfo;

                osdinfo.chan_name_show = DANAVIDEO_OSD_SHOW_CLOSE;
                osdinfo.show_name_x = 1;
                osdinfo.show_name_y = 2;

                osdinfo.datetime_show = DANAVIDEO_OSD_SHOW_CLOSE;
                osdinfo.show_datetime_x = 3;
                osdinfo.show_datetime_y = 4;
                osdinfo.show_format = DANAVIDEO_OSD_DATE_FORMAT_YYYY_MM_DD_CH;
                osdinfo.hour_format = DANAVIDEO_OSD_TIME_24_HOUR;
                osdinfo.show_week = DANAVIDEO_OSD_SHOW_OPEN;
                osdinfo.datetime_attr = DANAVIDEO_OSD_DATETIME_DISPLAY;

                osdinfo.custom1_show = DANAVIDEO_OSD_SHOW_OPEN;
                strncpy(osdinfo.show_custom1_str, "show_custom1_str", sizeof(osdinfo.show_custom1_str) -1);
                osdinfo.show_custom1_x = 5;
                osdinfo.show_custom1_y = 6;

                osdinfo.custom2_show = DANAVIDEO_OSD_SHOW_CLOSE;
                strncpy(osdinfo.show_custom2_str, "show_custom2_str", sizeof(osdinfo.show_custom2_str) -1);
                osdinfo.show_custom2_x = 7;
                osdinfo.show_custom2_y = 8;

                if (lib_danavideo_cmd_getosd_response(danavideoconn, trans_id, error_code, code_msg, &osdinfo)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETOSD send response succeeded\n");
                } else {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETOSD send response failed\n");
                }
            }
            break;
        case DANAVIDEOCMD_SETCHANNAME:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETCHANNAME\n");
                DANAVIDEOCMD_SETCHANNAME_ARG *setchanname_arg = (DANAVIDEOCMD_SETCHANNAME_ARG *)cmd_arg;
                dbg("setchanname_arg\n");
                dbg("ch_no: %"PRIu32"\n", setchanname_arg->ch_no);
                dbg("chan_name: %s\n", setchanname_arg->chan_name);
                dbg("\n"); 
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETCHANNAME send response succeeded\n");
                } else {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETCHANNAME send response failed\n");
                }
            } 
            break;
        case DANAVIDEOCMD_GETCHANNAME:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETCHANNAME\n");
                DANAVIDEOCMD_GETCHANNAME_ARG *getchanname_arg = (DANAVIDEOCMD_GETCHANNAME_ARG *)cmd_arg;
                dbg("getchanname_arg\n");
                dbg("ch_no: %"PRIu32"\n", getchanname_arg->ch_no);
                dbg("\n"); 
                error_code = 0;
                code_msg = "";

                char *chan_name = "Di3";
                if (lib_danavideo_cmd_getchanname_response(danavideoconn, trans_id, error_code, code_msg, chan_name)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETCHANNAME send response succeeded\n");
                } else {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETCHANNAME send response failed\n");
                }
            } 
            break;
        case DANAVIDEOCMD_CALLPSP:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_CALLPSP\n");
                DANAVIDEOCMD_CALLPSP_ARG *callpsp_arg = (DANAVIDEOCMD_CALLPSP_ARG *)cmd_arg;
                dbg("callpsp_arg\n");
                dbg("ch_no: %"PRIu32"\n", callpsp_arg->ch_no);
                dbg("psp_id: %"PRIu32"\n", callpsp_arg->psp_id);
                dbg("\n"); 
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_CALLPSP send response succeeded\n");
                } else {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_CALLPSP send response failed\n");
                }
            } 
            break;
        case DANAVIDEOCMD_GETPSP:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETPSP\n");
                DANAVIDEOCMD_GETPSP_ARG *getpsp_arg = (DANAVIDEOCMD_GETPSP_ARG *)cmd_arg;
                dbg("getpsp_arg\n");
                dbg("ch_no: %"PRIu32"\n", getpsp_arg->ch_no);
                dbg("page: %"PRIu32"\n", getpsp_arg->page);
                dbg("page_size: %"PRIu32"\n", getpsp_arg->page_size);
                dbg("\n"); 
                error_code = 0;
                code_msg = "";

                uint32_t total = 20;
                uint32_t psp_count = 2;
                libdanavideo_pspinfo_t psp[] = {{1, "Psp_1", true, true}, {2, "Psp_2", false, true}};
                if (lib_danavideo_cmd_getpsp_response(danavideoconn, trans_id, error_code, code_msg, total, psp_count, psp)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETPSP send response succeeded\n");
                } else {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETPSP send response failed\n");
                }
            } 
            break;
        case DANAVIDEOCMD_SETPSP:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETPSP\n");
                DANAVIDEOCMD_SETPSP_ARG *setpsp_arg = (DANAVIDEOCMD_SETPSP_ARG *)cmd_arg;
                dbg("setpsp_arg\n");
                dbg("ch_no: %"PRIu32"\n", setpsp_arg->ch_no);
                dbg("psp_id: %"PRIu32"\n", setpsp_arg->psp.psp_id);
                dbg("psp_name: %s\n", setpsp_arg->psp.psp_name);
                dbg("psp_default: %s\n", setpsp_arg->psp.psp_default?"true":"false");
                dbg("is_set: %s\n", setpsp_arg->psp.is_set?"true":"false");
                dbg("\n"); 
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETPSP send response succeeded\n");
                } else {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETPSP send response failed\n");
                }
            } 
            break;
        case DANAVIDEOCMD_SETPSPDEF:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETPSPDEF\n");
               DANAVIDEOCMD_SETPSPDEF_ARG *setpspdef_arg = (DANAVIDEOCMD_SETPSPDEF_ARG *)cmd_arg;
                dbg("setpspdef_arg\n");
                dbg("ch_no: %"PRIu32"\n", setpspdef_arg->ch_no);
                dbg("psp_id: %"PRIu32"\n", setpspdef_arg->psp_id);
                dbg("\n"); 
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETPSPDEF send response succeeded\n");
                } else {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETPSPDEF send response failed\n");
                }
            } 
            break; 
        case DANAVIDEOCMD_GETLAYOUT:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETLAYOUT\n");
                DANAVIDEOCMD_GETLAYOUT_ARG *getlayout_arg = (DANAVIDEOCMD_GETLAYOUT_ARG *)cmd_arg;
                dbg("getlayout_arg\n");
                dbg("ch_no: %"PRIu32"\n", getlayout_arg->ch_no);
                dbg("\n"); 
                error_code = 0;
                code_msg = "";

                uint32_t matrix_x = 4; 
                uint32_t matrix_y = 4;
                size_t chans_count = 16;
                uint32_t chans[] = {1, 1, 2, 3, 1, 1, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0};
                uint32_t layout_change = 0;
                uint32_t chan_pos_change = 0;
                size_t use_chs_count = 16;
                uint32_t use_chs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
                
                if (lib_danavideo_cmd_getlayout_response(danavideoconn, trans_id, error_code, code_msg, matrix_x, matrix_y, chans_count, chans, layout_change, chan_pos_change, use_chs_count, use_chs)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETLAYOUT send response succeeded\n");
                } else {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_GETLAYOUT send response failed\n");
                }
            } 
            break;
        case DANAVIDEOCMD_SETCHANADV:
            {
                dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETCHANADV\n");
                DANAVIDEOCMD_SETCHANADV_ARG *setchanadv_arg = (DANAVIDEOCMD_SETCHANADV_ARG *)cmd_arg;
                dbg("setchanadv_arg\n");
                dbg("ch_no: %"PRIu32"\n", setchanadv_arg->ch_no);
                dbg("matrix_x: %"PRIu32"\n", setchanadv_arg->matrix_x);
                dbg("matrix_y: %"PRIu32"\n", setchanadv_arg->matrix_y);
                size_t i;
                for (i=0; i<setchanadv_arg->chans_count; i++) {
                    dbg("chans[%zd]: %"PRIu32"\n", i, setchanadv_arg->chans[i]);
                }
                dbg("\n"); 
                error_code = 0;
                code_msg = "";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)"", trans_id, error_code, code_msg)) {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETCHANADV send response succeeded\n");
                } else {
                    dbg("TEST danavideoconn_command_handler DANAVIDEOCMD_SETCHANADV send response failed\n");
                }
            } 
            break;
        
        case DANAVIDEOCMD_RESOLVECMDFAILED:
            {
               dbg("DANAVIDEOCMD_RESOLVECMDFAILED\n");
                // 根据不同的method,调用lib_danavideo_cmd_response
                error_code = 20145;
                code_msg = (char *)"danavideocmd_resolvecmdfailed";
                if (lib_danavideo_cmd_exec_response(danavideoconn, cmd, (char *)cmd_arg, trans_id, error_code, code_msg)) {
                   dbg("DANAVIDEOCMD_RESOLVECMDFAILED send response succeeded\n");
                } else {
                   dbg("DANAVIDEOCMD_RESOLVECMDFAILED send response failed\n");
                }
            }
            break;
        default:
            {
               dbg("------UnKnown DANA CMD: %"PRId32"\n", cmd);
            }
            break;
    }

    return;
}

dana_video_callback_funs_t danavideocallbackfuns = {
    .danavideoconn_created = danavideoconn_created,
    .danavideoconn_aborted = danavideoconn_aborted,
    .danavideoconn_command_handler = danavideoconn_command_handler,
};


void danavideo_hb_is_ok(void)
{
    dbg("THE CONN TO P2P SERVER IS OK\n");
}

void danavideo_hb_is_not_ok(void) 
{
    dbg("THE CONN TO P2P SERVER IS NOT OK\n");
}

void danavideo_upgrade_rom(const char* rom_path,  const char *rom_md5, const uint32_t rom_size)
{
    dbg("NEED UPGRADE ROM rom_path: %s\trom_md5: %s\trom_size: %"PRIu32"\n", rom_path, rom_md5, rom_size);
}

void danavideo_autoset(const uint32_t power_freq, const int64_t now_time, const char *time_zone, const char *ntp_server1, const char *ntp_server2)
{
    dbg("AUTOSET\n\tpower_freq: %"PRIu32"\n\tnow_time: %"PRId64"\n\ttime_zone: %s\n\tntp_server1: %s\n\tntp_server2: %s\n", power_freq, now_time, time_zone, ntp_server1, ntp_server2);
}

volatile bool danaairlink_set_wifi_cb_called = false;

void danavideo_setwifiap(const uint32_t ch_no, const uint32_t ip_type, const char *ipaddr, const char *netmask, const char *gateway, const char *dns_name1, const char *dns_name2, const char *essid, const char *auth_key, const uint32_t enc_type)
{
    dbg("SETWIFIAP\n\tch_no: %"PRIu32"\n\tip_type: %"PRIu32"\n\tipaddr: %s\n\tnetmask: %s\n\tgateway: %s\n\tdns_name1: %s\n\tdns_name2: %s\n\tessid: %s\n\tauth_key: %s\n\tenc_type: %"PRIu32"\n", ch_no, ip_type, ipaddr, netmask, gateway, dns_name1, dns_name2, essid, auth_key, enc_type);
    switch (enc_type) {
        case DANAVIDEO_WIFI_ENCTYPE_NONE:
            dbg("DANAVIDEO_WIFI_ENCTYPE_NONE\n");
            break;
        case DANAVIDEO_WIFI_ENCTYPE_WEP:
            dbg("DANAVIDEO_WIFI_ENCTYPE_WEP\n");
            break;
        case DANAVIDEO_WIFI_ENCTYPE_WPA_TKIP:
            dbg("DANAVIDEO_WIFI_ENCTYPE_WPA_TKIP\n");
            break;
        case DANAVIDEO_WIFI_ENCTYPE_WPA_AES:
            dbg("DANAVIDEO_WIFI_ENCTYPE_WPA_AES\n");
            break;
        case DANAVIDEO_WIFI_ENCTYPE_WPA2_TKIP:
            dbg("DANAVIDEO_WIFI_ENCTYPE_WPA2_TKIP\n");
            break;
        case DANAVIDEO_WIFI_ENCTYPE_WPA2_AES:
            dbg("DANAVIDEO_WIFI_ENCTYPE_WPA2_AES\n");
            break;
        case DANAVIDEO_WIFI_ENCTYPE_INVALID:
        default:
            dbg("DANAVIDEO_WIFI_ENCTYPE_INVALID\n");
            break;
    }

    dbg("配置WiFi...\n");
    sleep(3);
    danaairlink_set_wifi_cb_called = true; // 主线程会再次进入配置状态
}

// 0 succeeded
// 1 failed
uint32_t danavideo_local_auth(const char *user_name, const char *user_pass)
{
    dbg("LocalAuth\n\tuser_name: %s\n\tuser_pass: %s\n", user_name, user_pass);
    if(strcasecmp(user_name, "xiaohuozi") == 0 && strcasecmp(user_pass, "daguniang") == 0){
        return 0;
    }

    return 0;
    // 用户名不区分大小写
    // 密码区分大小写
}
   
void danavideo_conf_create_or_updated(const char *conf_absolute_pathname)
{
    dbg("CONF_create_or_updated  conf_absolute_pathname: %s\n", conf_absolute_pathname);
}

uint32_t danavideo_get_connected_wifi_quality()
{
    uint32_t wifi_quality = 45;
    return wifi_quality;
}

// 生产工具可以设置这些信息
void danavideo_productsetdeviceinfo(const char *model, const char *sn, const char *hw_mac)
{
    dbg("danavideo_productsetdeviceinfo\n");
    dbg("model: %s\tsn: %s\thw_mac: %s\n", model, sn, hw_mac);
}

int32_t main () {
    dbg_off();

    volatile bool lib_danavideo_inited = false, lib_danavideo_started = false;
    volatile bool danaairlink_inited = false, danaairlink_started = false;

    signal(SIGINT, &exit_sig);
    signal(SIGTERM, &exit_sig);

    if (0 != setjmp(jmpbuf)) {
        goto testdanavideoexit; 
    } 

//    MMng_GetShMem();
    //AV_OSA_ipcMbxOpen();


	dbg("testdanavideo start\n");

    dbg("testdanavideo USING LIBDANAVIDEO_VERSION %s\n", lib_danavideo_linked_version_str(lib_danavideo_linked_version()));

    lib_danavideo_set_hb_is_ok(danavideo_hb_is_ok);
    lib_danavideo_set_hb_is_not_ok(danavideo_hb_is_not_ok);

    lib_danavideo_set_upgrade_rom(danavideo_upgrade_rom);

    lib_danavideo_set_autoset(danavideo_autoset);
    lib_danavideo_set_local_setwifiap(danavideo_setwifiap);

    lib_danavideo_set_local_auth(danavideo_local_auth);

    lib_danavideo_set_conf_created_or_updated(danavideo_conf_create_or_updated);

    lib_danavideo_set_get_connected_wifi_quality(danavideo_get_connected_wifi_quality);

    lib_danavideo_set_productsetdeviceinfo(danavideo_productsetdeviceinfo);

    char *danale_path = "/opt/ipnc/";
    uint32_t channel_num = 1;

    // 智能声控
    // 方法1: lib_danavideo_smart_conf_init()
    // 方法2: lib_danavideo_smart_conf_parse();
    // 方法3: lib_danavideo_smart_conf_set_danalepath();
    // 方法4: lib_danavideo_smart_conf_set_volume();
    // 方法5: lib_danavideo_set_smart_conf_response();
    //
    // 音频系统按 采样频率44100/采样精度16位 (有符号数) 设定
    // 调用lib_danavideo_smart_conf_set_danalepath()设置配置文件路径或者调用一次lib_danavideo_init
    // 调用lib_danavideo_set_smart_conf_response()设置回馈播放回调函数,回调函数类型见`danavideo.h`
    // 调用lib_danavideo_smart_conf_set_volume()设置播放音量,该值需要各厂家自己调试
    //
    // 调用lib_danavideo_smart_conf_init() 初始化
    // 循环将采集的PCM数据传给lib_danavideo_smart_conf_parse(...) 当识别到声音中的数据时,会调用lib_danavideo_set_local_setwifiap()设置的回调函数
    // 当配置成功,停止采集,并将音频系统设置为正常状态




    // 参数说明
    // 参数获得请联系大拿.
    // 如果配置文件danale.conf 已经存在或者本地部署生产服务器,则参数agent_user agent_pass的赋值可以为空.
    char *agent_user = NULL;
    char *agent_pass = NULL;
    char *chip_code = "TEST_chip";
    char *schema_code = "TEST_schema";
    char *product_code = "TEST_product";

    int32_t maximum_size = 3*1024*1024;
    lib_danavideo_set_maximum_buffering_data_size(maximum_size);

    uint32_t libdanavideo_startup_timeout_sec = 30;
    lib_danavideo_set_startup_timeout(libdanavideo_startup_timeout_sec);

    // 现在也可以在初始化之前调用了
    dbg("testdanavideo lib_danavideo_deviceid: %s\n", lib_danavideo_deviceid_from_conf(danale_path));

#if 1 //  在lib_danavideo_start之前调用 默认是 fixed 34102
    bool listen_port_fixed = true; // false
    uint16_t listen_port = 12349;
    lib_danavideo_set_listen_port(listen_port_fixed, listen_port);
#endif

    dbg("testdanavideo 1 lib_danavideo_get_listen_port: %"PRIu16"\n", lib_danavideo_get_listen_port());

    while (!lib_danavideo_init(danale_path, agent_user, agent_pass, chip_code, schema_code, product_code, &danavideocallbackfuns)) {
        dbg("testdanavideo lib_danavideo_init failed, try again\n");
        sleep(2);
    }
    dbg("testdanavideo lib_danavideo_init succeeded\n");
    lib_danavideo_inited = true;

    dbg("testdanavideo 2 lib_danavideo_get_listen_port: %"PRIu16"\n", lib_danavideo_get_listen_port());


    // lib_danavideo_init成功之后可以调用lib_danavideo_deviceid()获取设备标识码
    dbg("testdanavideo lib_danavideo_deviceid: %s\n", lib_danavideo_deviceid());

    while (!lib_danavideo_start()) {
        dbg("testdanavideo lib_danavideo_start failed, try again\n");
        sleep(2);
    }
    dbg("testdanavideo lib_danavideo_start succeeded\n");
    lib_danavideo_started = true;

    dbg("testdanavideo 3 lib_danavideo_get_listen_port: %"PRIu16"\n", lib_danavideo_get_listen_port());

    //////// test 工具类  注意,使用工具类的前提时 lib_danavideo_start() 完成//////
 
    danavideo_device_type_t device_type = DANAVIDEO_DEVICE_IPC;
    channel_num = 1;
    char *rom_ver = "rom_ver";
    char *api_ver = "api_ver";
    char *rom_md5 = "rom_md5";
    if (lib_danavideo_util_setdeviceinfo(device_type, channel_num, rom_ver, api_ver, rom_md5)) {
        dbg("\x1b[32mtestdanavideo TEST lib_danavideo_util_setdeviceinfo success\x1b[0m\n");
    } else {
        dbg("\x1b[34mtestdanavideo TEST lib_danavideo_util_setdeviceinfo failed\x1b[0m\n");
    }

#if 1 // lib_danavideo_util_pushmsg
    uint32_t ch_no = 1;
    uint32_t alarm_level = DANA_VIDEO_PUSHMSG_ALARM_LEVEL_2;
    uint32_t msg_type = DANA_VIDEO_PUSHMSG_MSG_TYPE_DOOR_SENSOR;
    char     *msg_title = "TEST danavideo_utili";
    char     *msg_body  = "lib_danavideo_util_pushmsg";
    int64_t  cur_time = 0;

#endif // lib_danavideo_util_pushmsg


    // 测试searchapp接口

    while (1) {
        sleep(300000);
    }

testdanavideoexit:

    if (danaairlink_started) {
        danaairlink_stop();
        danaairlink_started = false;
    }
    if (danaairlink_inited) {
        danaairlink_deinit();
    }
    danaairlink_inited = false;

    if (lib_danavideo_started) {
        lib_danavideo_stop();
        lib_danavideo_started = false;
    }
    usleep(800*1000);
    if (lib_danavideo_inited) {
        lib_danavideo_deinit();
        lib_danavideo_inited = false;
    }

    usleep(800*1000);

    dbg("testdanavideo exit\n");

    return 0;
}
