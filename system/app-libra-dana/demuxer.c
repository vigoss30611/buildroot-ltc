#include <stdlib.h>

#include <dlfcn.h>
#include <info_log.h>

void *g_filemng_so_handle = 0;

typedef struct _DemuxMngT{
    int oldHandle;
    int isMp4;
    int isH265;
    char* pFrame;
}DemuxMngT;

typedef int (*t_DemuxInit)(const char* filePathName, 
                              int *pKeyFrameCount, 
                              int *pWidth, int *pHeight);
typedef void (*t_DemuxDeinit)(const long handle);

typedef int (*t_DemuxGetUsPerFrame)(const long handle);

typedef int (*t_DemuxGetNextFrame)(const int handle,
       int *pMediaType, unsigned char** pBufOut, int *pLen);

typedef int (*t_DemuxGetDurationMs)(const int handle);

typedef long long (*t_DemuxGetFrameTimestamp)(const int handle);

typedef int (*t_Av_DemuxGetFrame)(const int handle, int frmIdx,
       int *pMediaType, unsigned char** pBufOut, int *pLen);

typedef enum AV_VIDEO_TYPE{
    AV_VIDEO_NONE,
    AV_VIDEO_H264 = 1,
    AV_VIDEO_H265 = 2,
}AV_VIDEO_TYPE;

typedef AV_VIDEO_TYPE (*t_Av_DemuxGetVideoType)(const int handle);

void *g_mp4_so_handle = 0;

#define MP4_EXT_NAME_TAG ".mp4"
#define MOV_EXT_NAME_TAG ".mov"
#define AVI_EXT_NAME_TAG ".avi"

#define LOG_265_TAG "===MP4DLL "

#ifdef LOGE_VID
#undef LOGE_VID
#endif

#define LOGE_VID printf(LOG_265_TAG);printf

#ifdef LOGV_VID
#undef LOGV_VID
#endif

#define LOGV_VID(...)// printf


static void av_mp4_cpp_load()
{
    LOGE_VID("%s \n",__FUNCTION__);
    if(g_mp4_so_handle != 0 ) return;
    g_mp4_so_handle = dlopen("libdemuxer.so",RTLD_LAZY);
    if(!g_mp4_so_handle)
    {
        LOGE_VID("load libdemuxer.so fail: %s \n",dlerror());
        return;
    }
    dlerror();

    return ;
}

static int av_call_Av_DemuxInit(const char* filePathName, int *pKeyFrameCount, int *pWidth, int *pHeight)
{
    char *error;
    static t_DemuxInit s_Av_DemuxInit = 0;
    if(g_mp4_so_handle == 0) av_mp4_cpp_load();
    if(s_Av_DemuxInit == 0)
    {
        s_Av_DemuxInit=dlsym(g_mp4_so_handle,"Av_DemuxInit");
        if((error=dlerror()) != 0)
        {
            LOGE_VID("%s \n",error);
            return 0;
        }
    }
    return s_Av_DemuxInit(filePathName, pKeyFrameCount, pWidth, pHeight);
}

static void av_call_Av_DemuxDeinit(const long handle)
{
    char *error;
    static t_DemuxDeinit s_Av_DemuxDeinit = 0;
    if(g_mp4_so_handle == 0) av_mp4_cpp_load();
    if(s_Av_DemuxDeinit == 0)
    {
        s_Av_DemuxDeinit=dlsym(g_mp4_so_handle,"Av_DemuxDeinit");
        if((error=dlerror()) != 0)
        {
            LOGE_VID("%s \n",error);
            return;
        }
    }
    return s_Av_DemuxDeinit(handle);
}

static int av_call_Av_DemuxGetUsPerFrame(const long handle)
{
    char *error;
    static t_DemuxGetUsPerFrame s_Av_DemuxGetUsPerFrame = 0;
    if(g_mp4_so_handle == 0) av_mp4_cpp_load();
    if(s_Av_DemuxGetUsPerFrame == 0)
    {
        s_Av_DemuxGetUsPerFrame=dlsym(g_mp4_so_handle,"Av_DemuxGetUsPerFrame");
        if((error=dlerror()) != 0)
        {
            LOGE_VID("%s \n",error);
            return 0;
        }
    }
    return s_Av_DemuxGetUsPerFrame(handle);
}

static int av_call_Av_DemuxGetNextFrame(const int handle,
       int *pMediaType, unsigned char** pBufOut, int *pLen)
{
    char *error;
    static t_DemuxGetNextFrame s_Av_DemuxGetNextFrame = 0;
    if(g_mp4_so_handle == 0) av_mp4_cpp_load();
    if(s_Av_DemuxGetNextFrame == 0)
    {
        s_Av_DemuxGetNextFrame=dlsym(g_mp4_so_handle,"Av_DemuxGetNextFrame");
        if((error=dlerror()) != 0)
        {
            LOGE_VID("%s \n",error);
            return -1;
        }
    }
    return s_Av_DemuxGetNextFrame(handle, pMediaType, pBufOut, pLen);
}

static int av_call_Av_DemuxGetDurationMs(const int handle)
{
    char *error;
    static t_DemuxGetDurationMs s_Av_DemuxGetDurationMs = 0;
    if(g_mp4_so_handle == 0) av_mp4_cpp_load();
    if(s_Av_DemuxGetDurationMs == 0)
    {
        s_Av_DemuxGetDurationMs=dlsym(g_mp4_so_handle,"Av_DemuxGetDurationMs");
        if((error=dlerror()) != 0)
        {
            LOGE_VID("%s \n",error);
            return 0;
        }
    }
    return s_Av_DemuxGetDurationMs(handle);
}

/*
 * Description:
 *      get frame timestamp, must called after Av_DemuxGetNextFrame
 * Parameters:
 * Return:
 *      timestamp   microsecond
 */
//long long Av_DemuxGetFrameTimestamp();
extern long long mp4_call_Av_DemuxGetFrameTimestamp(const int handle)
{
    DemuxMngT *dm = (DemuxMngT *)handle;
    if (NULL == dm) {return 0;}
    
    char *error;
    LOGV_VID("before mp4_call_Av_DemuxGetFrameTimestamp...\n");
    static t_DemuxGetFrameTimestamp s_Av_DemuxGetFrameTimestamp = 0;
    if(g_mp4_so_handle == 0) av_mp4_cpp_load();
    if(s_Av_DemuxGetFrameTimestamp == 0)
    {
        s_Av_DemuxGetFrameTimestamp=dlsym(g_mp4_so_handle,"Av_DemuxGetFrameTimestamp");
        if((error=dlerror()) != 0)
        {
            LOGE_VID("%s \n",error);
            return 0;
        }
    }
    long long ret = s_Av_DemuxGetFrameTimestamp(dm->oldHandle);
    LOGV_VID("end mp4_call_Av_DemuxGetFrameTimestamp...\n");
    return ret;
}

// 1:264, 2:265
static int av_call_Av_DemuxGetVideoType(const int handle)
{
    char *error;
    static t_Av_DemuxGetVideoType s_Av_DemuxGetVideoType = 0;
    if(g_mp4_so_handle == 0) av_mp4_cpp_load();
    if(s_Av_DemuxGetVideoType == 0)
    {
        s_Av_DemuxGetVideoType=dlsym(g_mp4_so_handle,"Av_DemuxGetVideoType");
        if((error=dlerror()) != 0)
        {
            LOGE_VID("%s \n",error);
            return 0;
        }
    }
    return (int)s_Av_DemuxGetVideoType(handle);
}

/////////////////////////////////////////////////////////////////////////

static void av_cpp_load()
{
    LOGE_VID("%s \n",__FUNCTION__);
    if(g_filemng_so_handle != 0 ) return;
    g_filemng_so_handle = dlopen("libfilemng.so",RTLD_LAZY);
    if(!g_filemng_so_handle)
    {
        LOGE_VID("%s \n",dlerror());
        return;
    }
    dlerror();

    return ;
}

int av_call_isH265(const int handle)
{
    DemuxMngT *dm = (DemuxMngT *)handle;
    if (NULL == dm) {return 0;}

    return dm->isH265;
}

int av_call_AviDemuxInit(const char* filePathName, int *pKeyFrameCount, int *pWidth, int *pHeight)
{
    DemuxMngT *dm = malloc(sizeof(DemuxMngT));
    if (NULL == dm) {LOGE_VID("ERROR: malloc DemuxMngT fail...\n");return 0;}
    memset(dm, 0, sizeof(DemuxMngT));
    
    if (strstr(filePathName, MP4_EXT_NAME_TAG) || 
        strstr(filePathName, MOV_EXT_NAME_TAG))
    {
        dm->isMp4 = 1;
        LOGV_VID("++++before av_call_Av_DemuxInit: %s.\n",filePathName);
        dm->oldHandle = av_call_Av_DemuxInit(filePathName, pKeyFrameCount, pWidth, pHeight);
        LOGV_VID("---- end av_call_Av_DemuxInit: 0x%x.\n", dm->oldHandle);
        if (dm->oldHandle) {
            LOGE_VID("video type###%d(1:264, 2:265)\n", av_call_Av_DemuxGetVideoType(dm->oldHandle));
            dm->isH265 = ((2==av_call_Av_DemuxGetVideoType(dm->oldHandle)) ? 1 : 0);
        } else {
            printf("=== MP4 DEMUX fail.\n");
            free(dm);
            return 0;
        }
    } 
    else if (strstr(filePathName, AVI_EXT_NAME_TAG))
    {
        dm->isMp4 = 0;
        char *error;
        static t_DemuxInit s_AviDemuxInit = 0;
        if(g_filemng_so_handle == 0) av_cpp_load();
        if(s_AviDemuxInit == 0)
        {
            s_AviDemuxInit=dlsym(g_filemng_so_handle,"AviDemuxInit");
            if((error=dlerror()) != 0)
            {
                LOGE_VID("%s \n",error);
                free(dm);
                return 0;
            }
        }
        dm->oldHandle = s_AviDemuxInit(filePathName, pKeyFrameCount, pWidth, pHeight);
   }
   else 
   {
       LOGE_VID("ERROR: file name to play is error(not mp4 or avi)...\n");
   }

   if (0 == dm->oldHandle)
   {
       free(dm);
       dm = NULL;
       return 0;
   }

   LOGV_VID("----after av_call_AviDemuxInit:0x%x, old handle:0x%x\n", (int)dm, dm->oldHandle);
   
   return (int)dm;
}

extern int av_call_Av_DemuxGetFrame(const int handle, int frmIdx,
       int *pMediaType, unsigned char** pBufOut, int *pLen)
{           
    DemuxMngT *dm = (DemuxMngT *)handle;
    if (NULL == dm) {free(dm);return 0;}

    if (dm->isMp4)
    {
        if(0) printf("----before av_call_Av_DemuxGetFrame: 0x%x.\n", dm->oldHandle);
        if (dm->pFrame){ free(dm->pFrame); dm->pFrame = NULL;}
        char *error;
        static t_Av_DemuxGetFrame s_Av_DemuxGetFrame = 0;
        if(g_mp4_so_handle == 0) av_mp4_cpp_load();
        if(s_Av_DemuxGetFrame == 0)
        {
            s_Av_DemuxGetFrame=dlsym(g_mp4_so_handle,"Av_DemuxGetFrame");
            if((error=dlerror()) != 0)
            {
                LOGE_VID("%s \n",error);
                return -1;
            }
        }
        int ret = s_Av_DemuxGetFrame(dm->oldHandle, frmIdx, pMediaType, pBufOut, pLen);
        if (0 == ret) dm->pFrame = *pBufOut;
        else printf("$$$$$$ av_call_Av_DemuxGetFrame return ERROR, ret: %d\n", ret);
        
        return ret;
    }
    else 
    {
        return -1;
    }
}

extern int av_call_aviDemuxGetDurationMs(const long handle)
{
    DemuxMngT *dm = (DemuxMngT *)handle;
    if (NULL == dm) {free(dm);return 0;}

    if (dm->isMp4)
    {
        LOGV_VID("----before Av_DemuxGetDurationMs: 0x%x.\n", dm->oldHandle);
        int ret = av_call_Av_DemuxGetDurationMs(dm->oldHandle);
        LOGV_VID("----end Av_DemuxGetDurationMs.\n");
        return ret;
    }
    else 
    {
        char *error;
        static t_DemuxGetDurationMs s_AviDemuxGetDurationMs = 0;
        if(g_filemng_so_handle == 0) av_cpp_load();
        if(s_AviDemuxGetDurationMs == 0)
        {
            s_AviDemuxGetDurationMs=dlsym(g_filemng_so_handle,"AviDemuxGetDurationMs");
            if((error=dlerror()) != 0)
            {
                LOGE_VID("%s \n",error);
                free(dm);
                dm = NULL; return 0;
            }
        }
        return s_AviDemuxGetDurationMs(dm->oldHandle);
    }
}


void av_call_AviDemuxDeinit(const long handle)
{
    DemuxMngT *dm = (DemuxMngT *)handle;
    if (NULL == dm) {return;}

    LOGV_VID("===before av_call_AviDemuxDeinit:0x%x, old handle:0x%x\n", handle, dm->oldHandle);

    if (dm->isMp4)
    {
        if (dm->pFrame){ free(dm->pFrame); dm->pFrame = NULL;}
        av_call_Av_DemuxDeinit(dm->oldHandle);
    }
    else 
    {
        char *error;
        static t_DemuxDeinit s_AviDemuxDeinit = 0;
        if(g_filemng_so_handle == 0) av_cpp_load();
        if(s_AviDemuxDeinit == 0)
        {
            s_AviDemuxDeinit=dlsym(g_filemng_so_handle,"AviDemuxDeinit");
            if((error=dlerror()) != 0)
            {
                LOGE_VID("%s \n",error);
                free(dm);
                return;
            }
        }
        s_AviDemuxDeinit(dm->oldHandle);
    }

    LOGV_VID("=== after av_call_AviDemuxDeinit\n");

    free(dm);
}

int av_call_AviDemuxGetUsPerFrame(const long handle)
{
    DemuxMngT *dm = (DemuxMngT *)handle;
    if (NULL == dm) {printf("Error: invalid handle...\n");return 0;}

    if (dm->isMp4)
    {        
        LOGV_VID("---- before Av_DemuxGetUsPerFrame: 0x%x\n", dm->oldHandle);
        int ret = av_call_Av_DemuxGetUsPerFrame(dm->oldHandle);
        LOGV_VID("---- end Av_DemuxGetUsPerFrame ret: %d\n", ret);
        return ret;
    }
    else 
    {
        char *error;
        static t_DemuxGetUsPerFrame s_AviDemuxGetUsPerFrame = 0;
        if(g_filemng_so_handle == 0) av_cpp_load();
        if(s_AviDemuxGetUsPerFrame == 0)
        {
            s_AviDemuxGetUsPerFrame=dlsym(g_filemng_so_handle,"AviDemuxGetUsPerFrame");
            if((error=dlerror()) != 0)
            {
                LOGE_VID("%s \n",error);
                free(dm);
                return 0;
            }
        }
        return s_AviDemuxGetUsPerFrame(dm->oldHandle);
    }
}

int av_call_AviDemuxGetNextFrame(const int handle,
       int *pMediaType, unsigned char** pBufOut, int *pLen)
{
    DemuxMngT *dm = (DemuxMngT *)handle;
    if (NULL == dm) {printf("Error: invalid handle...\n");return;}

    if (dm->isMp4)
    {
        if (dm->pFrame){ free(dm->pFrame); dm->pFrame = NULL;}
        LOGV_VID("+++++++ before Av_DemuxGetNextFrame: 0x%x\n", dm->oldHandle);
        int ret = av_call_Av_DemuxGetNextFrame(dm->oldHandle, pMediaType, pBufOut, pLen);
        if (0 == ret) {
            dm->pFrame = *pBufOut;
            unsigned int *ival = (unsigned int *)*pBufOut;
            LOGV_VID("--end Av_DemuxGetNextFrame,size:%d, data:%.8x %.8x\n", 
                    *pLen, *ival,*(ival+1));
        } else {
            printf("$$$$$$ end Av_DemuxGetNextFrame return ERROR, ret: %d\n", ret);
        }
        
        return ret;
    }
    else 
    {
        char *error;
        static t_DemuxGetNextFrame s_AviDemuxGetNextFrame = 0;
        if(g_filemng_so_handle == 0) av_cpp_load();
        if(s_AviDemuxGetNextFrame == 0)
        {
            s_AviDemuxGetNextFrame=dlsym(g_filemng_so_handle,"AviDemuxGetNextFrame");
            if((error=dlerror()) != 0)
            {
                LOGE_VID("%s \n",error);
                free(dm);
                return 0;
            }
        }
        return s_AviDemuxGetNextFrame(dm->oldHandle, pMediaType, pBufOut, pLen);
    }
}
