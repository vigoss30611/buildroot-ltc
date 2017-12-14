/**
  ******************************************************************************
  * @file       video.c
  * @author     InfoTM IPC Team
  * @brief
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 InfoTM</center></h2>
  *
  * This software is confidential and proprietary and may be used only as expressly
  * authorized by a licensing agreement from InfoTM, and ALL RIGHTS RESERVED.
  * 
  * The entire notice above must be reproduced on all copies and should not be
  * removed.
  ******************************************************************************
  */

#include "video.h"

#include <sys/prctl.h>
#include <qsdk/items.h>
#include <qsdk/sys.h>
#include <qsdk/event.h>
#include <fr/libfr.h>
#include <qsdk/videobox.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>


#include "ipc.h"
#include "looper.h"
#include "list.h"
#include "encode_controller.h"

#define LOGTAG  "VIDEO"

enum {
    VC_PREV_HIGH = 0,
    VC_PREV_LOW = 1,
    VCNUM = 2,
};

enum {
    PVSHIGH = 0,
    PVSLOW,
    PVSNUM
};

static char const * kVCName[] =
{
    "vc-prev-low",
    "vc-prev-high"
};

typedef struct {
    int w;
    int h;
} vcap_resolution_t;

#define VHDRBUF_SIZE    (128)
typedef struct {
    looper_t lp;
    int id;
    char const *chnl;
    bool enabled;
    struct v_basic_info basicInfo;
    
    vcap_resolution_t res;
    volatile bool resolutionSet;
    int fps;

    char header[VHDRBUF_SIZE];
    int hdrlen;
    
    int frcnt;
    int realFps;
    int realKbps;
    
    struct listnode strmlst;
} video_capture_t;

typedef struct stream_head_t {
    char name[16];
    struct listnode node;
    void (*process) (struct stream_head_t *shead, struct fr_buf_info *fr);
} stream_head_t;

typedef struct {
    pthread_mutex_t mutex;
    stream_head_t head;
    bool enabled;
    int frcnt;
    struct listnode clientList;
    video_capture_t const *vcapRef;
} preview_vstream_t;

typedef void (*frpost_fn_t) (void const *client, void const *fr);
typedef struct {
    struct listnode node;
    void const *client;
    frpost_fn_t frpost;
    int frcnt;
} preview_stream_client_t;

static char const *encChannelsName[] = 
{
    "enc1080p-stream",
    //"enc720p-stream",
    "encvga-stream",
};

static char const *jpegChannelsName[] = 
{
    "jpeg-out",
};

#define VIDEO_STREAM_MAX 3+1
typedef struct {
    int inited;
    int num;
    ven_info_t ven_info[VIDEO_STREAM_MAX];
} media_stream_info_t;

#undef MAX
#undef MIN
#define MAX(a, b)   ((a) > (b)) ? (a) : (b)
#define MIN(a, b)   ((a) > (b)) ? (b) : (a)

#define container_of(ptr, container, member)    \
    (container *) (((char*) (ptr)) - offsetof(container, member))


#define DUMP_LBUF_DEF(sz)   \
    char lbuf[sz] = {0};    \
    size_t ttsz = 0;    \
    int len = 0;
#define DUMP_LBUF(exp)  \
    exp;    \
    len = strlen(lbuf); \
    if (len < size) {   \
        memcpy(buf, lbuf, len); \
        buf += len; \
        ttsz += len;    \
    } else {    \
        goto Quit;  \
    }


static video_capture_t sVideoCap[VCNUM];
static preview_vstream_t sPrevVStrm[PVSNUM];
static media_stream_info_t sMediaStream = 
{
    .inited = 0,
    .num = 0,
};

void infotm_video_info_init(void);
int infotm_video_info_num_get(void);
ven_info_t * infotm_video_info_get(int id); 
char * infotm_video_name_get(int id);

static void * vcapProcessor(void *p)
{
    int ret, req;
    bool isKeyFrame, waitKeyFrame = true;
    struct fr_buf_info frbuf = FR_BUFINITIALIZER;
    struct listnode *node;
    video_capture_t *vc = (video_capture_t *)p;
    stream_head_t *shead;

    {
    char name[64];
    sprintf(name, "vcapProcessor:%d", vc->id);
    prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0);
    }

    ipclog_debug("enter vcap %d stream processor\n", vc->id);

    /*!< loop of video stream processing */
    lp_lock(&vc->lp);
    lp_set_state(&vc->lp, STATE_IDLE);
    while (lp_get_req(&vc->lp) != REQ_QUIT) {
        /*!< wait for start/quit requst when in idle state */
        if (lp_get_state(&vc->lp) == STATE_IDLE) {
            req = lp_wait_request_l(&vc->lp);
            if (req == REQ_START) {
                fr_INITBUF(&frbuf, NULL);
                vc->frcnt = 0;
                waitKeyFrame = true;
            } else if (req == REQ_QUIT) {
                goto Quit;
            }
        } else {
            /*!< check stop request */
            if (lp_check_request_l(&vc->lp) == REQ_STOP) {
                continue;
            }
        }
        
        if (vc->resolutionSet) {
            ipclog_debug("vcap %d set resolution\n", vc->id, vc->res.w, vc->res.h);
            video_set_resolution(vc->chnl, vc->res.w, vc->res.h); 
            vc->hdrlen = video_get_header(vc->chnl, vc->header, VHDRBUF_SIZE);
            if (vc->hdrlen <= 0) {
                ipclog_ooo("video_get_header() failed, ret=%d\n", vc->hdrlen);
                ipclog_error("videobox crashed");
            }
            
            vc->frcnt = 0;
            fr_INITBUF(&frbuf, NULL);
            waitKeyFrame = true;

            vc->resolutionSet = false;
            usleep(20000);
        }
        lp_unlock(&vc->lp);

        /*!< get frame from videobox */
        //TMI(kVCName[vc->id], "video_get_frame()", 1000);
        ret = video_get_frame(vc->chnl, &frbuf);
        //TMO(kVCName[vc->id], "video_get_frame()");
        if ((ret < 0) || (frbuf.size <= 0) || (frbuf.virt_addr == NULL)) {
            if (ret < 0) {
                ipclog_error("vcap %d video_get_frame() failed, ret=%d\n", vc->id, ret);
                ipclog_info("vcap %d stream start wait for next KEY frame, frcnt=%d\n", 
                        vc->id, vc->frcnt);
                waitKeyFrame = true;
                usleep(1000);
            } else {
                video_put_frame(vc->chnl, &frbuf);
            } 
        }
        else {
            //ipclog_debug("vcap[%d]: frcnt %d buf 0x%x, size %d\n", vc->id, vc->frcnt, frbuf.buf, frbuf.size);
            if (vc->frcnt == 0) {
                ipclog_info("vcap[%d] get first frame, buf 0x%x, size %d\n", vc->id, frbuf.buf, frbuf.size);
            }
            encode_controller_count_bitrate(vc->id, vc->frcnt, frbuf.priv, frbuf.size);
            
            vc->frcnt++;
        
            /*!< check and wait KEY frame if neccessary */ 
            isKeyFrame = (frbuf.priv == VIDEO_FRAME_I) ? true : false;
            if (waitKeyFrame && !isKeyFrame) {
                ipclog_debug("preview %d stream video discard non-KEY frame\n", vc->id);
                video_trigger_key_frame(vc->chnl);
            } else {
                if (waitKeyFrame) {
                    ipclog_info("preview %d stream video stop wait for next KEY frame, frcnt=%d\n", 
                            vc->id, vc->frcnt);
                    waitKeyFrame = false;
                }

                /*!< post frame to stream process */
                lp_lock(&vc->lp);
                list_for_each(node, &vc->strmlst) {
                    shead = node_to_item(node, stream_head_t, node);
                    shead->process(shead, &frbuf);
                }
                lp_unlock(&vc->lp);
            }

            //TMI(kVCName[vc->id], "video_put_frame()", 1000);
            video_put_frame(vc->chnl, &frbuf);
            //TMO(kVCName[vc->id], "video_put_frame()");
        }

        /*!< for request checking */
        usleep(0);
        lp_lock(&vc->lp);
    }

Quit:
    lp_unlock(&vc->lp);
    ipclog_debug("leave vcap %d stream processor\n", vc->id);
    return NULL;
}

static void vcapReset(int id);
static int vcapInit(int id)
{
    video_capture_t *vc = &sVideoCap[id];

    ipclog_debug("vcapInit(%d)\n", id);
	struct v_bitrate_info brinfo = {0};

    memset((void *)vc, 0, sizeof(video_capture_t));
    vc->id = id;
    vc->enabled = false;

    /*!< create vcap looper */
    if (lp_init(&vc->lp, kVCName[id], vcapProcessor, (void *)vc) != 0) {
        ipclog_ooo("create video capture %d looper failed\n", id);
        return -1;
    }

    /*!< configure vcap channel */
    vc->chnl = infotm_video_get_name(id);
    vc->basicInfo.media_type    = VIDEO_MEDIA_H264;
    vc->basicInfo.profile       = VENC_BASELINE;
    vc->basicInfo.stream_type   = VENC_NALSTREAM;

    struct v_basic_info stBasicInfo;
    video_get_basicinfo(vc->chnl, &stBasicInfo);
    vc->basicInfo.media_type = stBasicInfo.media_type;

    /*!< take configuration in effect */
    /*!< FIXME: CANNOT */
    //video_set_basicinfo(vc->chnl, &vc->basicInfo);
    vc->fps = video_get_fps(vc->chnl);
    int fps = vc->fps; 
    
    struct v_bitrate_info br;
    video_set_fps(vc->chnl, vc->fps);
    video_get_bitrate(vc->chnl, &br);
    video_get_resolution(vc->chnl, &vc->res.w, &vc->res.h);
    vc->fps = video_get_fps(vc->chnl);
    ipclog_debug("resolution=%d*%d\n", vc->res.w, vc->res.h);
    ipclog_debug("fps=%d\n", vc->fps);
    ipclog_debug("bitrates=%d\n", br.bitrate);
    ipclog_debug("br.qp_max=%d\n", br.qp_max);
    ipclog_debug("br.qp_min=%d\n", br.qp_min);
    ipclog_debug("br.qp_delta=%d\n", br.qp_delta);
    ipclog_debug("br.qp_hdr=%d\n", br.qp_hdr);
    ipclog_debug("br.gop_length=%d\n", br.gop_length);
    ipclog_debug("br.p_frame=%d\n", br.p_frame);
    ipclog_debug("br.picture_skip=%d\n", br.picture_skip);
 
    /*!< disable channel first because default is enabled */
    //video_disable_channel(vc->chnl);

    /*!< initialize stream process list */
    list_init(&vc->strmlst);

    return 0;
}

static void vcapDeinit(int id)
{
    video_capture_t *vc = &sVideoCap[id];

    ipclog_debug("vcapDeinit(%d)\n", id);

    /*!< stop and deinit looper */
    vcapReset(id);
    lp_deinit(&vc->lp);
}

static int vcapSetResolution(int id, vcap_resolution_t *v_res)
{
    video_capture_t *vc = &sVideoCap[id];

    ipclog_debug("%s %d, %d*%d\n", __FUNCTION__, id, v_res->w, v_res->h);
    if (!vc->enabled) {
        ipclog_warn("%s id:%d not enable\n", __FUNCTION__, id);
        return -1; 
    }
    
    if ((vc->res.w == v_res->w) && (vc->res.h == v_res->h)) {
        ipclog_warn("%s id:%d resolution is %d*%d already\n", __FUNCTION__, id,
                v_res->w, v_res->h);
        return -1;
    }

    lp_lock(&vc->lp);
    vc->res.w = v_res->w;
    vc->res.h = v_res->h;
    vc->resolutionSet = true;
    lp_unlock(&vc->lp);

    do {
        usleep(10000);
        ipclog_debug("%s wait for done\n", __FUNCTION__);
    } while(vc->resolutionSet);

    return 0;
}


int vcapSetFPS(int id, int fps) 
{   
    ipclog_debug("%s %d, fps:%d\n", __FUNCTION__, id, fps);

    video_capture_t *vc = &sVideoCap[id];
    if (!vc->enabled) {
        ipclog_warn("%s id:%d not enable\n", __FUNCTION__, id);
        return -1; 
    }

    int ret = video_set_fps(vc->chnl, fps);
    if (ret) {
        ipclog_error("%s failed(ret:%d) channel%s,fps:%u\n",__FUNCTION__, ret, vc->chnl, fps);
        return -1;
    }
    //!< Update gop_length according to the fps.
    ret = encode_controller_refresh_bitrate(id);
    if (ret) {
        ipclog_error("%s failed to update gop_length(ret:%d)\n",__FUNCTION__, ret);
        return -1;
    }

    return 0;
}


static void vcapStart_l(int id)
{
    video_capture_t *vc = &sVideoCap[id];

    ipclog_debug("vcapStart_l(%d)\n", id);
    assert(!vc->enabled);

    /*!< enable channel */
    if (video_enable_channel(vc->chnl) < 0) {
        ipclog_ooo("video_enable_channel(%s) failed\n", vc->chnl);
        ipc_panic("videobox crashed");
        return;
    }

    /*!< get header */
    vc->hdrlen = video_get_header(vc->chnl, vc->header, VHDRBUF_SIZE);
    if (vc->hdrlen <= 0) {
        ipclog_ooo("video_get_header() failed, ret=%d\n", vc->hdrlen);
        ipc_panic("videobox crashed");
    }

    /*!< start vcap looper */
    vc->realFps = 0;
    vc->realKbps = 0;
    vc->enabled = true;
    lp_start(&vc->lp, true);
}

static void vcapStop_l(int id)
{
    video_capture_t *vc = &sVideoCap[id];

    ipclog_debug("vcapStop_l(%d)\n", id);
    /*!< stop looper */
    lp_stop(&vc->lp, true);

    /*!< disable channel */
    video_disable_channel(vc->chnl);
    vc->enabled = false;
}

static void vcapReset(int id)
{
    video_capture_t *vc = &sVideoCap[id];

    ipclog_debug("vcapReset(%d)\n", id);

    lp_lock(&vc->lp);
    assert(!vc->enabled);
    lp_unlock(&vc->lp);
}

static void vcapAddStream(int id, stream_head_t *shead)
{
    video_capture_t *vc = &sVideoCap[id];

    ipclog_debug("vcapAddStream(%d, %s)\n", id, shead->name);

    lp_lock(&vc->lp);
    list_add_tail(&vc->strmlst, &shead->node);
    if (!vc->enabled) {
        vcapStart_l(id);
        assert(vc->enabled);
    }
    lp_unlock(&vc->lp);
}

static void vcapDelStream(int id, stream_head_t *shead)
{
    video_capture_t *vc = &sVideoCap[id];

    ipclog_debug("vcapDelStream(%d, %s)\n", id, shead->name);

    lp_lock(&vc->lp);
    list_remove(&shead->node);
    if (list_empty(&vc->strmlst)) {
        assert(vc->enabled);
        vcapStop_l(id);
        assert(!vc->enabled);
    }
    lp_unlock(&vc->lp);
}

static void prevVideoStrmProc(stream_head_t *shead, struct fr_buf_info *fr)
{
    preview_vstream_t *vs = container_of(shead, preview_vstream_t, head);
    struct listnode *node;
    preview_stream_client_t *psc;
    media_vframe_t mvfr = {0};
    
    if (!(vs->frcnt++ % 100)) {
        //ipclog_info("prevVideoStrmProc(%s, %d)\n", shead->name, vs->frcnt);
    }

    if (fr->priv == VIDEO_FRAME_I) {
        mvfr.flag |= MEDIA_VFRAME_FLAG_KEY;
    }
    mvfr.header = (char *)vs->vcapRef->header;
    mvfr.hdrlen = vs->vcapRef->hdrlen;
    mvfr.buf = fr->virt_addr;
    mvfr.size = fr->size;
    mvfr.ts = fr->timestamp;

    pthread_mutex_lock(&vs->mutex);
    list_for_each(node, &vs->clientList) {
        psc = node_to_item(node, preview_stream_client_t, node);
        if ((psc->frcnt == 0) && (fr->priv != VIDEO_FRAME_I)) {
            ipclog_debug("prevstrm-video client %p discard frame to search the first I frame\n", psc->client);
            continue;
        }
        if (!(psc->frcnt++ % 100)) {
            //ipclog_info("prevstrm-video client %p frcnt %d\n", psc->client, psc->frcnt);
        }
        psc->frpost(psc->client, (void const *)&mvfr);
    }
    pthread_mutex_unlock(&vs->mutex);
}

static void prevVideoStrmReset(int id);
static int prevVideoStrmInit(int id)
{
    int vcapId;
    preview_vstream_t *vs = &sPrevVStrm[id];

    ipclog_debug("prevVideoStrmInit(%d)\n", id);

    memset((void *)vs, 0, sizeof(preview_vstream_t));
    sprintf(vs->head.name, "preview:%d", id);
    list_init(&vs->head.node);
    vs->head.process = prevVideoStrmProc;
    pthread_mutex_init(&vs->mutex, NULL);
    vs->enabled = false;
    list_init(&vs->clientList);

    /*!< bind PVS to VC */
    vcapId = id;
    vs->vcapRef = (video_capture_t const *)&sVideoCap[vcapId];

    return 0;
}

static void prevVideoStrmDeinit(int id)
{
    ipclog_debug("prevVideoStrmDeinit(%d)\n", id);
    prevVideoStrmReset(id);
}

static void prevVideoStrmStart_l(int id)
{
    preview_vstream_t *vs = &sPrevVStrm[id];

    ipclog_debug("prevVideoStrmStart_l(%d)\n", id);

    /*!< start vcap and stream proc */
    vcapAddStream(vs->vcapRef->id, &vs->head);  /*!< don't need lock before call vcapAddStream */
    vs->enabled = true;
    vs->frcnt = 0;
}

static void prevVideoStrmStop_l(int id)
{
    preview_vstream_t *vs = &sPrevVStrm[id];

    ipclog_debug("prevVideoStrmStop_l(%d)\n", id);
  
    /*!< unlock before call vcapDelStream to avoid deadlock, because vcap could call prevVideoStrmProc in now */
    pthread_mutex_unlock(&vs->mutex);

    vcapDelStream(vs->vcapRef->id, &vs->head);

    pthread_mutex_lock(&vs->mutex);
    vs->enabled = false;
}

static int prevVideoStrmAddClient(int id, void const *client, video_post_fn_t vfrpost)
{
    preview_vstream_t *vs = &sPrevVStrm[id];
    preview_stream_client_t *pvsClient;

    ipclog_debug("prevVideoStrmAddClient(%d, %p, %p)\n", id, client, vfrpost);

    pvsClient = (preview_stream_client_t *)malloc(sizeof(preview_stream_client_t));
    if (!pvsClient) {
        ipclog_ooo("malloc preview_stream_client_t failed\n");
        return -1;
    }
    list_init(&pvsClient->node);
    pvsClient->client = client;
    pvsClient->frpost = (frpost_fn_t)vfrpost;
    pvsClient->frcnt = 0;
    pthread_mutex_lock(&vs->mutex);
    list_add_tail(&vs->clientList, &pvsClient->node);
    if (!vs->enabled) {
        prevVideoStrmStart_l(id);
        assert(vs->enabled);
    }
    pthread_mutex_unlock(&vs->mutex);

    return 0;
}

static int prevVideoStrmDelClient(int id, void const *client, bool all)
{
    preview_vstream_t *vs = &sPrevVStrm[id];
    struct listnode *node, *l;
    preview_stream_client_t *pvsClient;
    bool hit = false;

    ipclog_debug("prevVideoStrmDelClient(%d, %p, %d)\n", id, client, all);

    pthread_mutex_lock(&vs->mutex);
    list_for_each_safe(node, l, &vs->clientList) {
        pvsClient = node_to_item(node, preview_stream_client_t, node);
        if ((pvsClient->client == client) || all) {
            list_remove(node);
            free((void *)pvsClient);
            if (!all) {
                hit = true;
                break;
            }
        }
    }
    if (vs->enabled && list_empty(&vs->clientList)) {
        prevVideoStrmStop_l(id);
        assert(!vs->enabled);
    }
    pthread_mutex_unlock(&vs->mutex);

    return all ? 0 : (hit ? 0 : -1);
}

static void prevVideoStrmReset(int id)
{
    ipclog_debug("prevVideoStrmReset(%d)\n", id);
    prevVideoStrmDelClient(id, NULL, true);
}

#if 0
static size_t prevVideoStrmDump(int id, char *buf, size_t size)
{
    preview_vstream_t *vs = &sPrevVStrm[id];
    struct listnode *node;
    preview_stream_client_t *pvsClient;

    ipclog_debug("prevVideoStrmDump(%d, %p, %d)\n", id, buf, size);

    DUMP_LBUF_DEF(128);

    pthread_mutex_lock(&vs->mutex);
    DUMP_LBUF(sprintf(lbuf, "----- prevVideoStrmDump %d -----\n", id));
    DUMP_LBUF(sprintf(lbuf, "vcapRef %d, enabled %d, frcnt %d\n", vs->vcapRef->id, vs->enabled, vs->frcnt));

    DUMP_LBUF(sprintf(lbuf, "client list: "));
    list_for_each(node, &vs->clientList) {
        pvsClient = node_to_item(node, preview_stream_client_t, node);
        DUMP_LBUF(sprintf(lbuf, "(%p frcnt %d) ", pvsClient->client, pvsClient->frcnt));
    }
    DUMP_LBUF(sprintf(lbuf, "\n"));
Quit:
    pthread_mutex_unlock(&vs->mutex);
    return ttsz;
}
#endif

int infotm_video_preview_start(int id, void const *client, video_post_fn_t vfrpost)
{
    ipclog_debug("infotm_video_preview_start(%d, %p, %p)\n", id, client, vfrpost);
    assert(vfrpost);
    int num = infotm_video_info_num_get();
    if (id >= num) {
        ipclog_warn("%s invalid id:%d num:%d\n", __FUNCTION__, id, num);
        return -1;
    }

    if (prevVideoStrmAddClient(id, client, vfrpost) != 0) {
        ipclog_error("prevVideoStrmAddClient(%d) failed\n", id);
        return -1;
    }
    return 0;
}

int infotm_video_preview_stop(int id, void const *client)
{
    ipclog_debug("infotm_video_preview_stop(%d, %p)\n", id, client);
    int num = infotm_video_info_num_get();
    if (id >= num) {
        ipclog_warn("%s invalid id:%d num:%d\n", __FUNCTION__, id, num);
        return -1;
    }

    if (prevVideoStrmDelClient(id, client, false) != 0) {
        ipclog_error("prevVideoStrmDelClient(%d, %p) failed\n", id, client);
        return -1;
    }
    return 0;
}

void infotm_video_init()
{
    int id = 0;
    int num = 0;
    ipclog_debug("infotm_video_init()\n");
    
    infotm_video_info_init();
    num = infotm_video_info_num_get();
    
    /*!< initialize video capture */
    for (id=0; id < num; id++) {
        if (vcapInit(id) != 0) {
            ipclog_ooo("vcapInit(%d) failed\n", id);
            goto Fail;
        }
    }

    /*!< initialize preview video stream */
    for (id=0; id < num; id++) {
        if (prevVideoStrmInit(id) != 0) {
            ipclog_ooo("prevVideoStrmInit() failed\n");
            goto Fail;
        }
    }

    encode_controller_init();

    ipclog_debug("%s done\n", __FUNCTION__);

    return;
Fail:
    ipc_panic("infotm_video_init() crashed");
    return;
}

void infotm_video_deinit()
{
    int id;

    ipclog_debug("%s start\n", __FUNCTION__);
    int num = infotm_video_info_num_get();

    encode_controller_deinit();
    
    /*!< deinit preview video stream */
    for (id = 0; id < num; id++) {
        prevVideoStrmDeinit(id);
    }

    /*!< deinit video capture */
    for (id = 0; id < num; id++) {
        vcapDeinit(id);
    }
    

    ipclog_debug("%s done\n", __FUNCTION__);
}

void infotm_video_reset(void)
{
    int id = 0;
    ipclog_debug("%s start\n", __FUNCTION__);

    int num = infotm_video_info_num_get();

    /*!< reset preview video stream */
    for (id = 0; id < num; id++) {
        prevVideoStrmReset(id);
    }

    /*!< reset vcap */
    for (id = 0; id < num; id++) {
        vcapReset(id);
    }

    ipclog_debug("%s done\n", __FUNCTION__);
}

int infotm_video_set_resolution(int id, int w, int h)
{
    int ret = 0;
    int num = infotm_video_info_num_get();
    if (id >= num) {
        ipclog_warn("%s invalid id:%d num:%d\n", __FUNCTION__, id, num);
        return -1;
    }

    vcap_resolution_t v_res;
    v_res.w = w;
    v_res.h = h;
    ret = vcapSetResolution(id, &v_res);    

    if (ret == 0) {
        ven_info_t *info = infotm_video_info_get(id);
        info->width  = w;
        info->height = h;
    }
    ipclog_debug("%s id:%d %d*%d done\n", __FUNCTION__, id, w, h);

    return ret;
}

int infotm_video_set_viewmode(int id, int viewmode)
{
    int ret = 0;
    int i;
    struct listnode *node;
    preview_stream_client_t *psc;
    preview_vstream_t *vs;
    preview_stream_client_t client[2];

    ipclog_debug("%s start\n", __FUNCTION__);
    int num = infotm_video_info_num_get();
    if (id >= num) {
        ipclog_warn("%s invalid id:%d num:%d\n", __FUNCTION__, id, num);
        return -1;
    }

    for (i=0; i<num; i++) {
        vs = &sPrevVStrm[i];

        pthread_mutex_lock(&vs->mutex);
        list_for_each(node, &vs->clientList) {
            psc = node_to_item(node, preview_stream_client_t, node);

            client[i].frpost = psc->frpost;
            client[i].client = psc->client;
        }
        pthread_mutex_unlock(&vs->mutex);
    }

    /*!< deinit preview video stream */
    for (i = 0; i < num; i++) {
        prevVideoStrmDeinit(i);
    }

    /*!< deinit video capture */
    for (i = 0; i < num; i++) {
        vcapDeinit(i);
    }

    switch (viewmode) {
        case 0:
            videobox_repath("/root/.videobox/fe_k_isp_origin.json");
            break;
        case 1:
            videobox_repath("/root/.videobox/fe_k_frame_2_ispost_uo.json");
            break;
        case 2:
            videobox_repath("/root/.videobox/fe_k_isp_360full.json");
            break;
        case 3:
            videobox_repath("/root/.videobox/fe_k_isp_4_ispost_uo.json");
            break;
        default:
            videobox_repath("/root/.videobox/1080P.json");
            break;
    }
    usleep(100000);

    for (i=0; i < num; i++) {
        if (vcapInit(i) != 0) {
            ipclog_ooo("vcapInit(%d) failed\n", i);
            return -1;
        }
    }

    /*!< initialize preview video stream */
    for (i=0; i < num; i++) {
        if (prevVideoStrmInit(i) != 0) {
            ipclog_ooo("prevVideoStrmInit() failed\n");
            return -1;
        }
    }

    for (i=0; i < num; i++) {
        infotm_video_preview_start(i, client[i].client, client[i].frpost);
    }

    return ret;
}

int infotm_video_set_fps(int id, int fps) 
{
    int ret = 0; 
    int num = infotm_video_info_num_get();
    if (id >= num) {
        ipclog_warn("%s invalid id:%d num:%d\n", __FUNCTION__, id, num);
        return -1;
    }

    ret = vcapSetFPS(id, fps);
    if (ret == 0) {
        ven_info_t *info = infotm_video_info_get(id);
        info->fps = fps;
    }

    ipclog_debug("%s id:%d fps:%d done\n", __FUNCTION__, id, fps);

    return 0;
}

int infotm_video_set_bitrate(int id, int bitrate, int rc_mode, int p_frame, int quality)
{	
    int ret = 0;
    int num = infotm_video_info_num_get();
    if (id >= num) {
        ipclog_warn("%s invalid id:%d num:%d\n", __FUNCTION__, id, num);
        return -1;
    }

    ret = encode_controller_set_encode_param(id, bitrate, rc_mode, p_frame, quality);
    ipclog_info("%s: id:%d, bitrate=%d rc_mode:%d p_frame:%d quality:%d nRet:%d !\n", 
            __FUNCTION__, id, bitrate, rc_mode, p_frame, quality, ret);
    
    return ret;
}

static int videobox_json_name_get(char *json_name);
static int videobox_stream_info_get(const char *json_name, 
        const char *port, ven_info_t *ven_info);
static int videobox_jpeg_info_get(const char *json_name, 
        const char *port, ven_info_t *ven_info);

/* TODO: enum video info from videobox */
void infotm_video_info_init(void)
{   
    ipclog_info("%s \n", __FUNCTION__);
    if (sMediaStream.inited == 0) { 
        char json_name[64];
        ven_info_t ven_info;
        printf("%s json_name:%s \n", __FUNCTION__, json_name);
        if (videobox_json_name_get(json_name) < 0) {
            printf("%s can not get json_name\n", __FUNCTION__);
            return -1;
        }
        
        int index = 0;
        if (videobox_stream_info_get(json_name, "dn", &ven_info) == 0) {
            memcpy((void *)(&sMediaStream.ven_info[index]), 
                    (void *)(&ven_info), 
                    sizeof(ven_info_t));
            index++;
        }

        if (videobox_stream_info_get(json_name, "uo", &ven_info) == 0) {
            memcpy((void *)(&sMediaStream.ven_info[index]), 
                    (void *)(&ven_info), 
                    sizeof(ven_info_t));
            index++;
        }

        if (videobox_stream_info_get(json_name, "ss0", &ven_info) == 0) {
            memcpy((void *)(&sMediaStream.ven_info[index]), 
                    (void *)(&ven_info), 
                    sizeof(ven_info_t));
            index++;
        }

        if (videobox_stream_info_get(json_name, "ss1", &ven_info) == 0) {
            memcpy((void *)(&sMediaStream.ven_info[index]), 
                    (void *)(&ven_info), 
                    sizeof(ven_info_t));
            index++;
        }
        sMediaStream.num = index;
        
        if (videobox_jpeg_info_get(json_name, "jpeg", &ven_info) == 0) {
            if (sMediaStream.num) {
                memcpy((void *)(&sMediaStream.ven_info[index]), 
                        (void *)(&sMediaStream.ven_info[0]), 
                        sizeof(ven_info_t));
                sprintf(sMediaStream.ven_info[index].bindfd, "%s-stream", "jpeg");
            }
        }

        //get fps & media_type 
        index = 0;
        int i = 0;
        for (i = 0; i < sMediaStream.num; i++) {
            struct v_basic_info stBasicInfo;
            if (video_get_basicinfo(sMediaStream.ven_info[i].bindfd, &stBasicInfo) < 0) {
                ipclog_warn("%s video_get_basicinfo:%s failed\n", __FUNCTION__, 
                        sMediaStream.ven_info[i].bindfd);
            } else {
                int fps = 0;
                int w;
                int h;             
                fps = video_get_fps(sMediaStream.ven_info[i].bindfd);
                video_get_resolution(sMediaStream.ven_info[i].bindfd, &w, &h);
                
                sMediaStream.ven_info[index].media_type   = stBasicInfo.media_type;
                sMediaStream.ven_info[index].width  = w;
                sMediaStream.ven_info[index].height = h;
                sMediaStream.ven_info[index].fps    = fps;
                ipclog_info("%s ven_obj[%d].ven_attr.%d*%d %d \n", __FUNCTION__, index, 
                        sMediaStream.ven_info[index].width,
                        sMediaStream.ven_info[index].height,
                        sMediaStream.ven_info[index].fps);
                index++;
            }
        }

        //JPEG alway bind to first channel
        if (index) {
            sMediaStream.ven_info[index].width  = sMediaStream.ven_info[0].width;
            sMediaStream.ven_info[index].height = sMediaStream.ven_info[0].height;
            sMediaStream.ven_info[index].fps    = 1;
        }

        sMediaStream.inited = 1;
    }

    ipclog_info("%s done\n", __FUNCTION__);
    return;
}

int infotm_video_info_num_get(void)
{
    if (!sMediaStream.inited) {
        ipclog_warn("%s not inited already\n", __FUNCTION__);
        return 0;
    }

    return sMediaStream.num;
}

ven_info_t * infotm_video_info_get(int id) 
{
    if (!sMediaStream.inited) {
        ipclog_warn("%s not inited already\n", __FUNCTION__);
        return NULL;
    }

    if (id >= sMediaStream.num) {
        ipclog_warn("%s Invalid id:%d num:%d \n", __FUNCTION__, id, sMediaStream.num);
        return NULL;
    }
    
    return &sMediaStream.ven_info[id];
}

char *infotm_video_get_name(int id)
{
    if (!sMediaStream.inited) {
        ipclog_warn("%s not inited already\n", __FUNCTION__);
        return NULL;
    }

    if (id >= sMediaStream.num) {
        ipclog_warn("%s Invalid id:%d num:%d \n", __FUNCTION__, id, sMediaStream.num);
        return NULL;
    }

    return &sMediaStream.ven_info[id].bindfd;
}

#define CTRL_MAGIC	('O')
#define SEQ_NO (1)
#define GETI2CSENSORNAME    _IOW(CTRL_MAGIC, SEQ_NO + 1, char*)
#define ITEM_SENSOR "camera.sensor.model"
static int videobox_json_name_get(char *json_name)
{
    int ret = -1;
    char item_sensor[64] = {0};
    char item_name[64] = {0};
    char item_resolution[64] = {0};

    if (!json_name)
        return -1;
#if 0
	if((item_exist("camera.sensor.autodetect")) && item_integer("camera.sensor.autodetect", 0)){
        int fd = -1;
        fd = open("/dev/ddk_sensor", O_RDWR);
        if(fd <0)
        {
            printf("open /dev/ddk_sensor error\n");
            close(fd);
			return -1;
        }

        ioctl(fd, GETI2CSENSORNAME, item_sensor);
        close(fd);

    }else{
        if (!item_exist(ITEM_SENSOR)) {
            printf("item %s NOT found!\n", ITEM_SENSOR);
            return -1;
        }
        item_string(item_sensor, ITEM_SENSOR, 0);
    }

	sprintf(item_name, "sensor.%s.resolution", item_sensor);
	if (!item_exist(item_name)) {
        printf("item %s NOT found!\n", item_name);
        return -1;
    }

	item_string(item_resolution, item_name, 0);
#endif
    sprintf(item_resolution, "%s", "path");
    sprintf(json_name, "/root/.videobox/%s.json", item_resolution);
	printf("%s  %s\n",__FUNCTION__, json_name);
    return 0;
}

static int videobox_stream_info_get(const char *json_name, const char *port, ven_info_t *ven_info)
{
    char *buf = NULL;
    int w=0, h=0, fd=0, ret;
    cJSON *c=NULL, *tmp, *tmpp;
    int allOk = 0;
    
    memset(ven_info, 0, sizeof(ven_info_t));
    
    do {
        buf = malloc(20*1024);
        if (!buf) {
            break;
        }

        fd = open(json_name, O_RDONLY);
        if (fd <= 0) {
            break;
        }

        ret = read(fd, buf, 20*1024);
        if (ret <= 0) {
            break;
        }

        c = cJSON_Parse((const char *)buf);
        if (!c) {
            break;
        }

        tmp = cJSON_GetObjectItem(c, (const char *)"ispost");
        if (!tmp) {
            ipclog_warn("%s cannot get ispost\n", __FUNCTION__);
            tmp = cJSON_GetObjectItem(c, (const char *)"ispost0");
            if (!tmp) {
                ipclog_warn("%s cannot get ispost0\n", __FUNCTION__);
                break;
            }
        }

        tmp = cJSON_GetObjectItem(tmp, (const char *)"port");
        if (!tmp) {
            break;
        }

        tmp = cJSON_GetObjectItem(tmp, port);
        if (!tmp) {
            break;
        }
#if 1
        tmpp = cJSON_GetObjectItem(tmp, (const char *)"w");
        if (tmpp) {
            w = tmpp->valueint;
        }

        tmpp = cJSON_GetObjectItem(tmp, (const char *)"h");
        if (tmpp) {
            h = tmpp->valueint;
        }
#endif
        cJSON *bindArray = cJSON_GetObjectItem(tmp, (const char *)"bind");
        if (!bindArray) {
            break;
        }
        cJSON *bindlist = bindArray->child;
        if (bindlist != NULL) {
            printf("%s \n", bindlist->string);
            sprintf(ven_info->bindfd,"%s-stream", bindlist->string);
            allOk = 1;
        }
    } while(0);

    if (allOk) {
        ven_info->width = w;
        ven_info->height = h;
        printf("%s, %s: %d*%d.\n", __FUNCTION__, ven_info->bindfd, w, h);
    }

    if (c) {
        cJSON_Delete(c);
    }
    
    if (fd > 0) {
        close(fd);
    }

    if (buf) {
        free(buf);
    }
    
    printf("%s %s allOk:%d \n", __FUNCTION__, port, allOk);
    return allOk?0:-1;
}


static int videobox_jpeg_info_get(const char *json_name, const char *port, ven_info_t *ven_info)
{
    char *buf = NULL;
    int w=0, h=0, fd=0, ret;
    cJSON *c=NULL, *tmp, *tmpp;
    int allOk = 0;
    
    memset(ven_info, 0, sizeof(ven_info_t));
    
    do {
        buf = malloc(20*1024);
        if (!buf) {
            break;
        }

        fd = open(json_name, O_RDONLY);
        if (fd <= 0) {
            break;
        }

        ret = read(fd, buf, 20*1024);
        if (ret <= 0) {
            break;
        }

        c = cJSON_Parse((const char *)buf);
        if (!c) {
            break;
        }

        tmp = cJSON_GetObjectItem(c, (const char *)port);
        if (!tmp) {
            break;
        }

        allOk = 1;
    } while(0);

    if (c) {
        cJSON_Delete(c);
    }
    
    if (fd > 0) {
        close(fd);
    }

    if (buf) {
        free(buf);
    }
    
    printf("%s %s allOk:%d \n", __FUNCTION__, port, allOk);
    return allOk?0:-1;
}

