#include <smartrc.h> 


stSmartRCDefaultSetting *smartrc_default_setting;
stSmartRCInfo smartrc_info;
static int getInt(Json *js, const char* item, int *value)
{
    assert(js != NULL);
    Json *pSub;
    
    
    pSub = js->GetObject(item);
    if (pSub ==NULL) return -1;
    
    *value = pSub->valueint;
    return 0;
}

static int get_one_rcset(Json *js, stSmartRCSet *pRc )
{
    
    if (getInt(js, "bitrate", &pRc->bps) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "qp_min", &pRc->qpMin) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "qp_max", &pRc->qpMax) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "qp_delta", &pRc->intraQpDelta) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    
    return 0;
}

static int init_rcset(Json *js, stSmartRCSet *pRcset)
{
    stSmartRCSet rc = {0};
    stSmartRCSet *prc = pRcset;
    Json *pJs;
    Json *pJs1;
    
    pJs = js;
    for (int i = 0; i< smartrc_info.mode_num; i++)
    {
        pJs1 = pJs->Child();
        for (int j = 0;j< smartrc_info.level_num; j++)
        {
            get_one_rcset(pJs1, &rc);
            *prc = rc;
            prc++;
            pJs1 = pJs1->Next();
        }
        pJs = pJs->Next();
    
    }
    
    return 0;
}

static int init_template(Json *js)
{
    stSmartRCDefaultSetting *pstream;
    Json *pJs;
    
    if (getInt(js, "stream-num", &smartrc_info.stream_num) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "level-num", &smartrc_info.level_num) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "time", &smartrc_info.time_gap) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "day-move", &(smartrc_info.bps[0])) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "day-still", &(smartrc_info.bps[1])) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "night-move", &(smartrc_info.bps[2])) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "night-still", &(smartrc_info.bps[3])) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "threshold_over_numer", &(smartrc_info.s32ThrOverNumer)) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "threshold_over_denom", &(smartrc_info.s32ThrOverDenom)) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "threshold_normal_numer", &(smartrc_info.s32ThrNormalNumer)) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }
    if (getInt(js, "threshold_normal_denom", &(smartrc_info.s32ThrNormalDenom)) < 0)
    {
        LOGE(" %s %d fail\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //sanity check threshold value
    if ( smartrc_info.s32ThrOverNumer * smartrc_info.s32ThrNormalDenom < smartrc_info.s32ThrNormalNumer * smartrc_info.s32ThrOverDenom )
    {
        //invalid value, force to an default value
        smartrc_info.s32ThrOverNumer = 4;
        smartrc_info.s32ThrOverDenom = 3;
        smartrc_info.s32ThrNormalNumer = 1;
        smartrc_info.s32ThrNormalDenom = 1;
    }

    LOGI(" %s %d customize bps: %d %d %d %d\n", __FUNCTION__, __LINE__, smartrc_info.bps[0], smartrc_info.bps[1], smartrc_info.bps[2], smartrc_info.bps[3]);
    
    LOGI(" %s %d threshold over[%d %d] normal[%d %d]\n", __FUNCTION__, __LINE__, smartrc_info.s32ThrOverNumer, smartrc_info.s32ThrOverDenom, smartrc_info.s32ThrNormalNumer, smartrc_info.s32ThrNormalDenom);
    
    //alloc mem
    smartrc_default_setting = (stSmartRCDefaultSetting *)malloc(smartrc_info.stream_num*sizeof(stSmartRCDefaultSetting));
    if (smartrc_default_setting == NULL)
    {
        LOGE(" %s %d alloc mem failed \n", __FUNCTION__, __LINE__);
    }
    memset(smartrc_default_setting, 0, smartrc_info.stream_num*sizeof(stSmartRCDefaultSetting));
    
    pstream = smartrc_default_setting;
    pJs = js->Next();
    for (int i = 0; i< smartrc_info.stream_num; i++)
    {
        pstream->pSet = (stSmartRCSet*) malloc(smartrc_info.mode_num*smartrc_info.level_num*sizeof(stSmartRCSet));
        if (pstream->pSet == NULL)
        {
            LOGE(" %s %d alloc mem failed \n", __FUNCTION__, __LINE__);
            return -1;
        }
        strncpy(pstream->stream, pJs->string, strlen(pJs->string)+1);
        init_rcset(pJs->Child(), pstream->pSet);
        pJs = pJs->Next();
        pstream ++;
    }
    
    return 0;
    
}


int smartrc_defaultsetting_init(Json *js)
{
    assert( js != NULL );
    Json *pJs = js->Child();
    
    if ( strcmp(pJs->string, "args") != 0)
    {
        LOGE(" %s %d wrong rc file start \n ", __FUNCTION__, __LINE__);
        return -1;
    }

    
    memset(&smartrc_info, 0, sizeof(smartrc_info));
    smartrc_info.mode_num = 4; //currently only support 4 mode
    if (init_template(pJs) <0)
    {
        LOGE(" %s %d failed to init template \n ", __FUNCTION__, __LINE__);
        return -1;
    }
    
    return 0;
}

int smartrc_defaultsetting_deinit()
{

    stSmartRCDefaultSetting *pstream;

    if (smartrc_default_setting == NULL)
        return 0;

    pthread_mutex_destroy(&gSmartRCContext.mutex);
    pstream = smartrc_default_setting;
    for (int i = 0; i<smartrc_info.stream_num; i++)
    {
        if (pstream->pSet)
            free(pstream->pSet);

        pstream++;
    }

    free(smartrc_default_setting);
    smartrc_default_setting = NULL;

    return 0;
}



