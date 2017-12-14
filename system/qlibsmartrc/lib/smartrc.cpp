#include <qsdk/event.h>
#include <smartrc.h>
#include <unistd.h>
#include <vector>
#include <list>
using namespace std;


/* *************************************************************************************************
 *                      function declartion
 * ************************************************************************************************* */

static void motion_handler(char *event, void *arg);
static void smartrc_ctrl_handler(char *event, void *arg);
static void va_blkinfo_handler(char *event, void *arg);
static void mv_blkinfo_handler(char *event, void *arg);

static int find_rc_template(char *stream, int bitrate,  unsigned int mode, stSmartRCSet *rc_set);

/* *************************************************************************************************
 *                      global variable
 * ************************************************************************************************* */

stSmartRCCtx gSmartRCContext;

typedef std::vector<stAdjoin>::iterator AIter;
std::vector<stAdjoin> alist;
AIter aiter;
stAdjoin join_temp;
struct v_roi_info r;
bool isABlockMove;
char value[7][7];
int roi_last_timestamp;

const stSmartRCMsgHandler smartrc_msg_handler_cb[] =
{
    {EVENT_VAMOVE, motion_handler},
    {EVENT_VAMOVE_BLK, va_blkinfo_handler},
    {EVENT_SMARTRC_CTRL, smartrc_ctrl_handler},
    {EVENT_MVMOVE, mv_blkinfo_handler}

};


/* *************************************************************************************************
 *                      macro define
 * ************************************************************************************************* */
#define BLKX (7)
#define BLKY (7)

#define MSG_HANDLER_NUM  (sizeof(smartrc_msg_handler_cb)/sizeof(smartrc_msg_handler_cb[0]))

/* *************************************************************************************************
 *                       function implemetation
 * ************************************************************************************************* */
static void smartrc_update_ratectrl()
{
    int ret;
    stSmartRCDefaultSetting *pstream;
    struct v_rate_ctrl_info  *rc;

    pthread_mutex_lock(&gSmartRCContext.mutex);
    for (int i = 0; i<smartrc_info.stream_num; i++)
    {
        pstream = smartrc_default_setting +i;
        rc = &pstream->new_set;
        if ( pstream->change_flag )
        {
            if ( rc->rc_mode == VENC_CBR_MODE )
            {
                if ( !rc->cbr.bitrate )
                {
                    continue;
                }
            }
            else if ( rc->rc_mode == VENC_VBR_MODE )
            {
                if ( !rc->vbr.max_bitrate )
                {
                    continue;
                }
            }
            LOGI("%s %s apply ratecrl: mode:%d bps:%d [%d, %d, %d]\n", __FUNCTION__, pstream->stream, rc->rc_mode, rc->cbr.bitrate, rc->cbr.qp_min, rc->cbr.qp_max, rc->cbr.qp_delta);
            ret = video_set_ratectrl_ex(pstream->stream, rc);
            if(ret < 0) {
                LOGE("%s line:%d ret:%d\n", __FUNCTION__, __LINE__, ret);
            }
            pstream->change_flag = false;
        }

    }
    pthread_mutex_unlock(&gSmartRCContext.mutex);

    return;
}

static int set_user_ratectrl(const char *stream, const struct v_rate_ctrl_info *rc)
{
    stSmartRCSet stRc;

    if (stream == NULL || rc ==NULL)
        return -1;

    stSmartRCDefaultSetting *pstream;
    for (int i = 0; i<smartrc_info.stream_num; i++)
    {
        pstream = smartrc_default_setting +i;
        if(!strcmp(stream, pstream->stream))
        {
            if ( rc->rc_mode == VENC_CBR_MODE )
            {
                pstream->s32Index = find_rc_template(pstream->stream, rc->cbr.bitrate, 0, &stRc);
                LOGD(" %s stream:%s bitrate:%d index:%d \n", __FUNCTION__, pstream->stream, rc->cbr.bitrate, pstream->s32Index);

            }
            pstream->user_set = *rc;
            return 0;
        }
    }

    LOGE("In %s line:%d fail \n", __FUNCTION__, __LINE__);
    return -1;
}


static int set_new_ratectrl(const char *stream, const struct v_rate_ctrl_info *rc)
{
    if (stream == NULL || rc ==NULL)
        return -1;

    stSmartRCDefaultSetting *pstream;
    for (int i = 0; i<smartrc_info.stream_num; i++)
    {
        pstream = smartrc_default_setting +i;
        if(!strcmp(stream, pstream->stream))
        {
            pstream->new_set = *rc;
            pstream->change_flag = true;
            return 0;
        }
    }

    LOGE("In %s line:%d fail \n", __FUNCTION__, __LINE__);
    return -1;
}
static int get_user_ratectrl(const char *stream,  struct v_rate_ctrl_info *rc)
{
    if (stream == NULL || rc ==NULL)
        return -1;

    stSmartRCDefaultSetting *pstream;
    for (int i = 0; i<smartrc_info.stream_num; i++)
    {
        pstream = smartrc_default_setting +i;
        if(!strcmp(stream, pstream->stream))
        {
            *rc = pstream->user_set;
            return 0;
        }
    }

    LOGE("In %s line:%d fail \n", __FUNCTION__, __LINE__);
    return -1;
}

static int get_new_ratectrl(const char *stream, struct v_rate_ctrl_info *rc)
{
    if (stream == NULL || rc ==NULL)
        return -1;

    stSmartRCDefaultSetting *pstream;
    for (int i = 0; i<smartrc_info.stream_num; i++)
    {
        pstream = smartrc_default_setting +i;
        if(!strcmp(stream, pstream->stream))
        {
            *rc = pstream->new_set;
            return 0;
        }
    }

    LOGE("In %s line:%d fail \n", __FUNCTION__, __LINE__);
    return -1;
}

static unsigned int get_mode()
{
    return gSmartRCContext.mode;
}

static int preprocess_bps_ex( int old_mode, int new_mode )
{
    struct v_rate_ctrl_info rate_ctrl;
    int ret;
    //stSmartRCSet rc_set;
    stSmartRCDefaultSetting *pstream;
    assert ( old_mode < 4);
    assert ( new_mode < 4);
    LOGD("In %s line:%d mode: %d-->%d \n", __FUNCTION__, __LINE__, old_mode, new_mode);

    for (int i = 0; i<smartrc_info.stream_num; i++)
    {
        pstream = smartrc_default_setting +i;
        //1. get user_set
        //ret = video_get_ratectrl(pstream->stream, &rate_ctrl);
        ret = get_user_ratectrl(pstream->stream, &rate_ctrl);
        if(ret < 0) {
            LOGE("%s line:%d %s failed with ret:%d\n", __FUNCTION__, __LINE__, pstream->stream, ret);
            continue;
        }
        if ( rate_ctrl.rc_mode == VENC_CBR_MODE )
        {

            //2. process data
            LOGD("%s  before:%d \n", pstream->stream, rate_ctrl.cbr.bitrate);
            rate_ctrl.cbr.bitrate =(int)( (long double)rate_ctrl.cbr.bitrate * smartrc_info.bps[new_mode] /smartrc_info.bps[0]);

            LOGD("%s  after:%d \n", pstream->stream, rate_ctrl.cbr.bitrate);
            //3. set to new_set
            //ret = video_set_ratectrl_ex(pstream->stream, &rate_ctrl);
            ret = set_new_ratectrl(pstream->stream, &rate_ctrl);
            if(ret < 0) {
                LOGE("%s line:%d ret:%d\n", __FUNCTION__, __LINE__, ret);
            }
        }
    }

    return 0;
}

int getcnt(char raw[7][7], int fix, int min, int max, stAdjoin *src)
{
    int m, cnt = 0;
    if(min < 0 || max > 6 || (fix&0x7f) < 0 || (fix&0x7f) > 6)
        return 0;
    for(m = min; m < max+1; m++)
        if((fix & 0x80) ? raw[fix&0x7f][m] : raw[m][fix]) {
            cnt++;
        }
    if(cnt) {
        if(fix & 0x80) // row
            if((fix & 0x80) == src->t-1)
                src->t -= 1;
            else
                src->b += 1;
        else
            if(fix == src->l-1)
                src->l -= 1;
            else
                src->r += 1;
        src->cnt += cnt;
    }
    return cnt;
}

int adjoin(char raw[7][7], stAdjoin *src, int i, int j)
{
    return
        (i == src->b+1 || i == src->t-1) && j <= src->r && j >= src->l ? getcnt(raw, i|0x80, src->l, src->r, src) :
        (j == src->r+1 || j == src->l-1) && i <= src->b && i >= src->t ? getcnt(raw, j, src->t, src->b, src) : 0;
}

int injoin(int i, int j, stAdjoin *src)
{
    return i <= src->b && i >= src->t && j <= src->r && j >= src->l ? 1 : 0;
}

struct v_roi_info get_roi_info(struct event_vamovement *efd)
{
    int i, j;
    int max_cnt = 0;
    int max_vice = 0;
    char dst[7][7];

    if(efd->timestamp == roi_last_timestamp) {
        isABlockMove |= value[efd->y][efd->x] = (char)efd->block_count;
        r.roi[0].enable = r.roi[1].enable = 0;
    } else {
        if(isABlockMove) {
            memset(dst, 0, sizeof(char) * 49);
            memset(&g_roi, 0, sizeof(struct v_roi_info));
            memset(&r, 0, sizeof(struct v_roi_info));
            memcpy(dst, value, sizeof(char)*49);
            for(i = 0; i < 7; i++) {
                for(j = 0; j < 7; j++) {
                    if(value[i][j]) {
                        for(aiter = alist.begin(); aiter != alist.end(); aiter++) {
                            if(injoin(i, j, &(*aiter)))
                                break;
                            if(adjoin(value, &(*aiter), i, j))
                                break;
                        }
                        if(aiter == alist.end()) {
                            join_temp.l = join_temp.r = j;
                            join_temp.t = join_temp.b = i;
                            join_temp.cnt = 1;
                            alist.push_back(join_temp);

                            // |  a|
                            // |b c|
                            // fix this case ac and b will separate as c is not processed
                            adjoin(value, &alist.back(), i+1, j);
                        }
                    }
                }
            }
            for(aiter = alist.begin(); aiter != alist.end(); aiter++) {
                join_temp = *aiter;
                if(join_temp.cnt >= max_cnt) {
                    max_vice = max_cnt;
                    max_cnt = join_temp.cnt;
                }
            }
            for(aiter = alist.begin(); aiter != alist.end(); aiter++) {
                join_temp = *aiter;
                if((join_temp.cnt == max_cnt) && max_cnt) {
                    r.roi[0].x = join_temp.l;
                    r.roi[0].y = join_temp.t;
                    r.roi[0].w = join_temp.r - join_temp.l + 1;
                    r.roi[0].h = join_temp.b - join_temp.t + 1;
                    r.roi[0].qp_offset = -5;
                    r.roi[0].enable = 1;
                    if (max_cnt == max_vice) {
                        max_cnt = -1;
                    }
                } else if((join_temp.cnt == max_vice) && max_vice) {
                    r.roi[1].x = join_temp.l;
                    r.roi[1].y = join_temp.t;
                    r.roi[1].w = join_temp.r - join_temp.l + 1;
                    r.roi[1].h = join_temp.b - join_temp.t + 1;
                    r.roi[1].qp_offset = -5;
                    r.roi[1].enable = 1;
                }
            }

            alist.clear();
            memset(value, 0, sizeof(char)*49);
            isABlockMove = false;
        }
        //next frame's first move blk
        isABlockMove |= value[efd->y][efd->x] = (char)efd->block_count;
        roi_last_timestamp = efd->timestamp;
    }
    return r;
}

int setroi(struct v_roi_info r)
{
    int ret = 0;
    int w = 0, h = 0;
    unsigned int u32_resw = 0,u32_resh = 0;
    int i, j;
    struct v_roi_info r_temp = r;
    stSmartRCDefaultSetting *pstream;
    for (i = 0; i < smartrc_info.stream_num; i++) {
        r = r_temp;
        pstream = smartrc_default_setting + i;
        if(r.roi[0].enable || r.roi[1].enable) {
            ret = video_get_resolution(pstream->stream, &w, &h);
            if(ret) {
                printf("Get %s resolution failed: %d\n", pstream->stream, ret);
                return -1;
            }
            u32_resw = w;
            u32_resh = h;
            w /= BLKX;
            h /= BLKY;
            for(j = 0; j < 2; j++) {
                r.roi[j].x *= w;
                r.roi[j].y *= h;
                r.roi[j].w *= w;
                r.roi[j].h *= h;
            }
        }
        if ((r.roi[0].enable || r.roi[1].enable) && (r.roi[0].w*r.roi[0].h + r.roi[1].w*r.roi[1].h < u32_resw*u32_resh/5))
        {
            ret |= video_set_roi(pstream->stream, &r);
        }
    }
    return ret;
}

static int find_rc_template(char *stream, int bitrate,  unsigned int mode, stSmartRCSet *rc_set)
{
    stSmartRCSet rc_s;
    stSmartRCSet stRc_prev;
    stSmartRCDefaultSetting *pstream;
    stSmartRCSet *prc;
    int str_i;
    int bps;
    int s32Newbps;
    assert ( mode < 4);
    assert ( rc_set!=NULL);
    assert ( stream!=NULL);

    bps = bitrate;
    str_i = 0;
    for (int i = 0; i<smartrc_info.stream_num; i++)
    {
        pstream = smartrc_default_setting +i ;
        if(!strcmp(stream, pstream->stream))
        {
            str_i = i; break;
        }
    }
    LOGD("%s stream:%s bps:%d mode:%d\n",__FUNCTION__, pstream->stream, bps, mode );

    pstream = smartrc_default_setting +str_i ;
    //min
    prc = pstream->pSet + (mode+1)*smartrc_info.level_num -1;
    rc_s = *prc;
    if ( rc_s.bps >= bps )
    {
        *rc_set = rc_s;
        LOGD("%s line:%d bps:%d --> {rc: bps:%d [%d, %d], delta:%d}\n", __FUNCTION__, __LINE__,
                bps, rc_s.bps, rc_s.qpMin, rc_s.qpMax, rc_s.intraQpDelta);
        // return 0;
        return smartrc_info.level_num-1;
    }

    //max
    prc = pstream->pSet + mode*smartrc_info.level_num ;
    rc_s = *prc;
    if ( bps >= rc_s.bps )
    {
        *rc_set = rc_s;
        LOGD("%s line:%d bps:%d --> {rc: bps:%d [%d, %d], delta:%d}\n", __FUNCTION__, __LINE__,
                bps, rc_s.bps, rc_s.qpMin, rc_s.qpMax, rc_s.intraQpDelta);
        return 0;
    }

    for (int j = 1; j < smartrc_info.level_num; j++)
    {
        prc = pstream->pSet + mode*smartrc_info.level_num +j ;
        rc_s = *prc;
        prc = pstream->pSet + mode*smartrc_info.level_num +j-1 ;
        stRc_prev = *prc;
        s32Newbps = rc_s.bps + (stRc_prev.bps - rc_s.bps)*3/10;

        if ( bps > s32Newbps )
        {
            *rc_set = stRc_prev;
            LOGD("%s line:%d bps:%d --> { rc: bps:%d [%d, %d],delta:%d}\n", __FUNCTION__, __LINE__,
                    bps, rc_set->bps, rc_set->qpMin, rc_set->qpMax, rc_set->intraQpDelta);
            // return 0;
            return j-1;
        }
        else if ( bps > rc_s.bps )
        {
            *rc_set = rc_s;
            LOGD("%s line:%d bps:%d --> { rc: bps:%d [%d, %d],delta:%d}\n", __FUNCTION__, __LINE__,
                    bps, rc_set->bps, rc_set->qpMin, rc_set->qpMax, rc_set->intraQpDelta);
            // return 0;
            return j;

        }
    }

    LOGE("%s line:%d bps:%d [error] rc: bps:%d [%d, %d],delta:%d\n", __FUNCTION__, __LINE__,
                    bps, rc_s.bps, rc_s.qpMin, rc_s.qpMax, rc_s.intraQpDelta);
    return -1;
}

static void apply_new_default_setting(unsigned int mode)
{
    struct v_rate_ctrl_info rate_ctrl;
    int ret;
    stSmartRCSet rc_set;
    stSmartRCDefaultSetting *pstream;
    LOGD("IN %s line:%d mode:%d\n", __FUNCTION__, __LINE__, mode);
    assert ( mode < 4);

    for (int i=0; i<smartrc_info.stream_num; i++)
    {
        pstream = smartrc_default_setting +i;
        //1. get new set
        //ret = video_get_ratectrl(pstream->stream, &rate_ctrl);
        ret = get_new_ratectrl(pstream->stream, &rate_ctrl);
        if(ret < 0) {
            LOGE("%s line:%d ret:%d\n", __FUNCTION__, __LINE__, ret);
            goto ERR;
        }

        if ( rate_ctrl.rc_mode == VENC_CBR_MODE )
        {
            //2. process data
            pstream->s32Index = find_rc_template(pstream->stream, rate_ctrl.cbr.bitrate, mode, &rc_set);
            //rate_ctrl.cbr.bitrate = rc_set.bps;
            rate_ctrl.cbr.qp_min = rc_set.qpMin;
            rate_ctrl.cbr.qp_max = rc_set.qpMax;
            rate_ctrl.cbr.qp_delta = rc_set.intraQpDelta;
            //3. set to new set
            //ret = video_set_ratectrl_ex(pstream->stream, &rate_ctrl);
            ret = set_new_ratectrl(pstream->stream, &rate_ctrl);
            if(ret < 0) {
                LOGE("%s line:%d ret:%d\n", __FUNCTION__, __LINE__, ret);
            }
            LOGE("%s line:%d %s bps:%d qp:[%d, %d], delta:%d\n", __FUNCTION__, __LINE__, pstream->stream, rc_set.bps, rc_set.qpMin, rc_set.qpMax, rc_set.intraQpDelta);

        }
    }
ERR:
    return;
}


/*   monitor camera to judge day/night scenario */
static void monitor_camera_status()
{
    int bit1;
    int old_mode;
    enum cam_day_mode out_day_mode;
    int s32Monochrome_state = 0;

    s32Monochrome_state = camera_monochrome_is_enabled("isp");

    if ( 0 == s32Monochrome_state )
    {
        out_day_mode = CAM_DAY_MODE_DAY;
    }
    else if ( 1 == s32Monochrome_state )
    {
        out_day_mode = CAM_DAY_MODE_NIGHT;
    }
    else
    {
        LOGE(" %s %d invalid monochrome status:%d \n", __FUNCTION__, __LINE__, s32Monochrome_state );
        return;
    }

#if 0
    {//test code
        static int count = 0;
        count ++;
        if (count%10 == 0)
        {
            out_day_mode = CAM_DAY_MODE_NIGHT;
            LOGE(" #7777777 hack  out_day_mode:%d \n",  out_day_mode);
        }
    }
#endif

    pthread_mutex_lock(&gSmartRCContext.mutex);
    old_mode = gSmartRCContext.mode;
    bit1 = (gSmartRCContext.mode & 0x2) >>1;
    if ( out_day_mode == bit1 )
    {
        if (bit1)
            gSmartRCContext.mode &= 0x1;
        else
            gSmartRCContext.mode |= 0x2;

        preprocess_bps_ex(old_mode, gSmartRCContext.mode);
        apply_new_default_setting(gSmartRCContext.mode);
    }
    pthread_mutex_unlock(&gSmartRCContext.mutex);

    return;
}


static void monitor_realtime_bps()
{
    int rtbps;
    int s32Bps;
    int s32TargetBps;
    struct v_rate_ctrl_info rate_ctrl;
    struct v_rate_ctrl_info stTargetRC;
    int ret;
    stSmartRCSet rc_set;
    struct timeval stTimeCurr;
    stSmartRCDefaultSetting *pstream ;
    int mode = get_mode();
    gettimeofday(&stTimeCurr, NULL);

    pthread_mutex_lock(&gSmartRCContext.mutex);
    for (int i = 0; i<smartrc_info.stream_num; i++)
    {
        pstream = smartrc_default_setting + i;
        rtbps = video_get_rtbps(pstream->stream);
        LOGD(" %s  realtime kbps:%d\n", pstream->stream,  rtbps/1024);

        if ( !rtbps )
        {
            continue;
        }

        //ret = video_get_ratectrl(pstream->stream, &rate_ctrl);
        ret = get_new_ratectrl(pstream->stream, &rate_ctrl);
        if(ret < 0) {
            LOGE("%s line:%d ret:%d stream:%s\n", __FUNCTION__, __LINE__, ret, pstream->stream);
            goto END;
        }
        ret = get_user_ratectrl(pstream->stream, &stTargetRC);
        if(ret < 0) {
            LOGE("%s line:%d ret:%d stream:%s\n", __FUNCTION__, __LINE__, ret, pstream->stream);
            goto END;
        }

        if ( rate_ctrl.rc_mode != VENC_CBR_MODE )
        {
            continue;
        }

        s32TargetBps = stTargetRC.cbr.bitrate;

        if (  rtbps > s32TargetBps*smartrc_info.s32ThrOverNumer/smartrc_info.s32ThrOverDenom )
        { //consider reasonable fluctuation
            s32Bps = rate_ctrl.cbr.bitrate;
            find_rc_template(pstream->stream, s32Bps, mode, &rc_set);
            LOGD("%s stream:%s bitrate over target:%d real:%d \n", __FUNCTION__,  pstream->stream, s32TargetBps, rtbps);

            LOGD(" #55555 %s s32State: %d -> 2\n", pstream->stream, pstream->s32State);
            rc_set.qpMin = RC_QP_MIN;
            rc_set.qpMax = RC_QP_MAX;
            pstream->s32State = STAT_RC_OVERRATE;
        }
        else if ( rtbps > s32TargetBps*smartrc_info.s32ThrNormalNumer/smartrc_info.s32ThrNormalDenom )
        {
            //do nothing
            LOGD("%s stream:%s bitrate over target:%d real:%d, do nothing \n", __FUNCTION__,  pstream->stream, s32TargetBps, rtbps);
            continue;
        }
        else   //bps not over
        {
            s32Bps = rate_ctrl.cbr.bitrate;
            find_rc_template(pstream->stream, s32Bps, mode, &rc_set);

            if ( pstream->s32State == STAT_RC_ROLLBACK )
            {
                if ( stTimeCurr.tv_sec > pstream->stTimeLast.tv_sec + TIME_GAP_7S )
                {
                    pstream->s32State = STAT_RC_NORMAL;
                    LOGD(" #55555 %s s32State: 1 -> 0\n", pstream->stream);
                    //not modify default qp setting
                }
                else
                {
                    rc_set.qpMin = RC_QP_MIN + (rc_set.qpMin - RC_QP_MIN)/2;
                    rc_set.qpMax = RC_QP_MAX - (RC_QP_MAX - rc_set.qpMax)/2;
                }
            }
            else if ( pstream->s32State == STAT_RC_OVERRATE )
            {
                rc_set.qpMin = RC_QP_MIN + (rc_set.qpMin - RC_QP_MIN)/2;
                rc_set.qpMax = RC_QP_MAX - (RC_QP_MAX - rc_set.qpMax)/2;
                pstream->s32State = STAT_RC_ROLLBACK;
                pstream->stTimeLast = stTimeCurr;
                LOGD(" #55555 %s s32State: 2 -> 1\n", pstream->stream);
            }
            //else if ( pstream->s32State == 0 )  //do nothing
        }

        if ( ( rate_ctrl.cbr.qp_min != rc_set.qpMin) || (rate_ctrl.cbr.qp_max!=rc_set.qpMax) || (rate_ctrl.cbr.qp_delta != rc_set.intraQpDelta) )
        {
            LOGD("%s line:%d %s bps:%d  qp:[%d, %d, %d] -> [%d, %d, %d]\n", __FUNCTION__,  __LINE__,  pstream->stream, rate_ctrl.cbr.bitrate, rate_ctrl.cbr.qp_min, rate_ctrl.cbr.qp_max, rate_ctrl.cbr.qp_delta, rc_set.qpMin, rc_set.qpMax, rc_set.intraQpDelta);
            rate_ctrl.cbr.qp_min = rc_set.qpMin;
            rate_ctrl.cbr.qp_max = rc_set.qpMax;
            rate_ctrl.cbr.qp_delta = rc_set.intraQpDelta;
            ret = set_new_ratectrl(pstream->stream, &rate_ctrl);
            if(ret < 0) {
                LOGE("%s line:%d ret:%d stream:%s\n", __FUNCTION__, __LINE__, ret, pstream->stream);
            }
        }
    }

END:
    pthread_mutex_unlock(&gSmartRCContext.mutex);
    return;
}

bool CheckIfOverBitrate()
{
    stSmartRCDefaultSetting *pstStream ;
    for ( int i = 0; i < smartrc_info.stream_num; i++ )
    {
        pstStream = smartrc_default_setting + i;
        if ( pstStream->s32State != STAT_RC_NORMAL )
        {
            return true;
        }
    }

    return false;
}

static void monitor_motion_status()
{
    struct timeval timetv;
    int old_mode;   //in ms
    gettimeofday(&timetv, NULL);

    pthread_mutex_lock(&gSmartRCContext.mutex);
    //LOGE("  cur_time:[%d, %d] motion_record:[%d, %d]\n", __FUNCTION__, timetv.tv_sec, timetv.tv_usec, gSmartRCContext.movement_record.tv_sec, gSmartRCContext.movement_record.tv_usec);

    old_mode = gSmartRCContext.mode;
    if ( (timetv.tv_sec > gSmartRCContext.movement_record.tv_sec +smartrc_info.time_gap) && !(gSmartRCContext.mode & 0x01) )
    {
        //check if any stream is in over bitrate status, if yes, don't enter still mode
        if ( false == CheckIfOverBitrate() )
        {
            gSmartRCContext.mode |= 0x01; //set to still mode
            LOGD(" status change from motion -> still mode:%d\n", gSmartRCContext.mode);
            preprocess_bps_ex(old_mode, gSmartRCContext.mode);
            apply_new_default_setting(gSmartRCContext.mode);
        }
    }
    pthread_mutex_unlock(&gSmartRCContext.mutex);
}

void smartrc_monitor_system_status()
{

    monitor_motion_status();

    monitor_camera_status();

    monitor_realtime_bps();

    smartrc_update_ratectrl();
}

static void smartrc_ctrl_handler(char *event, void *arg)
{
    struct v_rate_ctrl_info rate_ctrl;
    stSmartRCSet rc_set;
    int ret;
    int bps;
    unsigned int mode = get_mode();
    assert(!strcmp(event, EVENT_SMARTRC_CTRL));
    LOGI(" %s  event:%s \n",  __func__,  event);

    memcpy(&rate_ctrl, (struct v_rate_ctrl_info*)arg, sizeof(struct v_rate_ctrl_info));

    pthread_mutex_lock(&gSmartRCContext.mutex);
    if ( rate_ctrl.rc_mode == VENC_CBR_MODE )
    {
        //1. set user_set and new_set both
        ret = set_user_ratectrl(rate_ctrl.name, &rate_ctrl);
        if(ret < 0) {
            LOGE("%s line:%d ret:%d\n", __FUNCTION__, __LINE__, ret);
        }

        //1. set new_set according current smartrc mode
        bps =(int)( (long double)rate_ctrl.cbr.bitrate * smartrc_info.bps[mode] /smartrc_info.bps[0]);
        rate_ctrl.cbr.bitrate = bps;
        find_rc_template(rate_ctrl.name, rate_ctrl.cbr.bitrate, mode, &rc_set);
        //rate_ctrl.cbr.bitrate = rc_set.bps;
        rate_ctrl.cbr.qp_min = rc_set.qpMin;
        rate_ctrl.cbr.qp_max = rc_set.qpMax;
        rate_ctrl.cbr.qp_delta = rc_set.intraQpDelta;
        //ret = video_set_ratectrl_ex(rate_ctrl.name, &rate_ctrl);
        ret = set_new_ratectrl(rate_ctrl.name, &rate_ctrl);
        if(ret < 0) {
            LOGE("%s line:%d ret:%d\n", __FUNCTION__, __LINE__, ret);
        }
    }
    else  //in case VBR mode, smartrc can know the status
    {

        //1. set user_set and new_set both
        ret = set_user_ratectrl(rate_ctrl.name, &rate_ctrl);
        if(ret < 0) {
            LOGE("%s line:%d ret:%d\n", __FUNCTION__, __LINE__, ret);
        }
        ret = set_new_ratectrl(rate_ctrl.name, &rate_ctrl);
        if(ret < 0) {
            LOGE("%s line:%d ret:%d\n", __FUNCTION__, __LINE__, ret);
        }

    }
    pthread_mutex_unlock(&gSmartRCContext.mutex);

    return;
}

static void motion_handler(char *event, void *arg)
{
    struct timeval timetv;
    static uint64_t time_prev = 0;   //in ms
    uint64_t time_curr;   //in ms
    int old_mode;
    assert(!strcmp(event, EVENT_VAMOVE));

    gettimeofday(&timetv, NULL);

    time_curr = timetv.tv_sec*1000 + timetv.tv_usec/1000;
    if (time_curr < time_prev+2000)
    {
        return;
    }
    time_prev = time_curr;

    LOGI(" %s  event:%s \n",  __func__,  event);

    pthread_mutex_lock(&gSmartRCContext.mutex);
    gSmartRCContext.movement_record = timetv;

    old_mode = gSmartRCContext.mode;
    if (gSmartRCContext.mode & 0x01)
    {
        gSmartRCContext.mode &= 0x02;  //set bit0 to 0

        preprocess_bps_ex(old_mode, gSmartRCContext.mode);
        apply_new_default_setting(gSmartRCContext.mode);
        LOGD(" status change from still -> motion, mode:%d \n", gSmartRCContext.mode);
    }
    pthread_mutex_unlock(&gSmartRCContext.mutex);
}

static void va_blkinfo_handler(char *event, void *arg)
{
    struct event_vamovement efd;
    struct v_roi_info roi;

    LOGE(" %s  event:%s \n",  __func__,  event);
    if(!strcmp(event, EVENT_VAMOVE_BLK)) {
        memcpy(&efd, (struct event_vamovement*)arg, sizeof(struct event_vamovement));
        roi = get_roi_info(&efd);
        if(roi.roi[0].enable || roi.roi[1].enable) {
            alarm(0);
            g_roi = roi;
            g_tosetroi = true;
            alarm(1);
        }
    }
}

static void mv_blkinfo_handler(char *event, void *arg)
{
    ST_EVENT_DATA st_event;
    struct event_vamovement efd;
    struct v_roi_info roi;

    if(!strcmp(event, EVENT_MVMOVE))
    {
        memcpy(&st_event, (ST_EVENT_DATA *)arg, sizeof(ST_EVENT_DATA));
        //printf("smartrc recive EVENT_MVMOVE x:%d y:%d w:%d h:%d mb_num:%d\n",
        //    st_event.mx, st_event.my, st_event.blk_x, st_event.blk_y, st_event.mbNum);
        efd.x = st_event.mx;
        efd.y = st_event.my;
        efd.block_count = st_event.mbNum;
        efd.timestamp = st_event.timestamp;
        roi = get_roi_info(&efd);
        if(roi.roi[0].enable || roi.roi[1].enable) {
            alarm(0);
            g_roi = roi;
            g_tosetroi = true;
            alarm(1);
        }
    }

}

void roi_alarm_handler(int signal)
{
    memset(&g_roi, 0, sizeof(struct v_roi_info));
    g_tosetroi = true;
}


static void smartrc_message_cb(char *event, void *arg)
{
    int num = MSG_HANDLER_NUM;

    for (int i = 0; i < num; i++)
    {
        if (!strncmp(event, smartrc_msg_handler_cb[i].msg, strlen(event)+1))
        {
            //LOGI(" %s hit event:%s\n", __FUNCTION__, event);
            smartrc_msg_handler_cb[i].handler(event, arg);
            return;
        }
    }
    LOGE(" %s miss hit event:%s\n", __FUNCTION__, event);
    return;
}

int smartrc_register_msg_handler_cb()
{
    int ret;

    memset(&gSmartRCContext, 0, sizeof(gSmartRCContext));
    pthread_mutex_init(&(gSmartRCContext.mutex), NULL);

    ret = event_register_handler(EVENT_VAMOVE, 0, smartrc_message_cb);
    if (ret !=0) LOGE(" %s EVENT_VAMOVE ret:%d\n", __FUNCTION__, ret);
    ret = event_register_handler(EVENT_VAMOVE_BLK, 0, smartrc_message_cb);
    if (ret !=0) LOGE(" %s EVENT_VAMOVE_BLK ret:%d\n", __FUNCTION__, ret);
    ret = event_register_handler(EVENT_SMARTRC_CTRL, 0, smartrc_message_cb);
    if (ret !=0) LOGE(" %s EVENT_SMARTRC_CTRL ret:%d\n", __FUNCTION__, ret);
    ret = event_register_handler(EVENT_MVMOVE, 0, smartrc_message_cb);
    if (ret !=0) LOGE(" %s EVENT_VAMOVE_BLK ret:%d\n", __FUNCTION__, ret);

    return ret;
}

int smartrc_unregister_msg_handler_cb()
{
    int ret = 0;
    ret |= event_unregister_handler(EVENT_VAMOVE, smartrc_message_cb);
    ret |= event_unregister_handler(EVENT_VAMOVE_BLK, smartrc_message_cb);
    ret |= event_unregister_handler(EVENT_SMARTRC_CTRL, smartrc_message_cb);
    if (ret !=0) LOGE(" %s event unregister vamove failed:%d\n", __FUNCTION__, ret);
    return ret;
}

