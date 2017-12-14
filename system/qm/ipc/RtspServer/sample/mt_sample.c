/******************************************************************************

  Copyright (C), 2007-, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : 
  Version       : Initial Draft
  Author        : 
  Created       : 
  Description   : 
  History       :
  1.Date        : 
    Author      : 
    Modification: 
******************************************************************************/



#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_mt.h"

int getmediainfo( int chn, MT_MEDIAINFO_S* pMediaInfo)
{
    return 0;
}

int userverify(int chn, const char* pUser, const char* pPwd)
{
    return 0;
}


int startenc(int chn)
{
    return 0;
}

int stopenc(int chn)
{
    return 0;
}

/*如, 请求IFrame*/
int prelive(int chn)
{
    /*must request I Frame*/
    return 0;
    
}
HI_S32   g_langtao_sock = 0;


int main(int argc, char* argv[])
{
    HI_S32 s32Ret = 0;
    MT_CONFIG_S stMtCfg;
    memset(&stMtCfg,0,sizeof(MT_CONFIG_S));

    /*配置httpserver, 目前库此参数必须配置为HI_FALSE*/
    stMtCfg.httpsvr.bEnable = HI_FALSE;
    
    /*配置httpserver*/
    stMtCfg.rtspsvr.bEnable = HI_TRUE;
    stMtCfg.rtspsvr.listen_port = 554;
    stMtCfg.rtspsvr.udp_send_port_min = 5000;
    stMtCfg.rtspsvr.udp_send_port_max = 6000;

    /*配置其他属性,如mbuf配置信息*/
    stMtCfg.packet_len = 300;   /*数据传输包payload长度，依据不同网络不同*/

    /*浪涛手机点播配置*/
    stMtCfg.langtaosvr.bEnable=HI_TRUE;
    stMtCfg.langtaosvr.max_connections = 1;

    /*D1/VGA 300k    cif/QVGA 75k      qcif/QQVGA 75k*/
    stMtCfg.chn_cnt = 1;
    /**/
    stMtCfg.mbuf[0].chnid    = 11;
    stMtCfg.mbuf[0].buf_size = 30*1024; /*buf size, byte*/
    

    /*初始化回调函数*/
    stMtCfg.pf_getmediainfo = getmediainfo;
    stMtCfg.pf_userverify = userverify;
    stMtCfg.pf_startenc = startenc;
    stMtCfg.pf_stopenc = stopenc;
    stMtCfg.pf_prelive = prelive;
    
    s32Ret =  HI_MT_Init(&stMtCfg);
    if(HI_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    HI_MT_StartSvr();

    /*启动浪涛注册*/
    #define CONNECTED 1
    #define LTVOD_MODULE_MASTER 1
    #define LTVOD_MODULE_PARSVE 2
    #define STREAMING_END "Streaming_End"

    #define STREAM_END_TIMEVAL_SEC 10
    #define STREAM_END_TIMEVAL_USEC 0
    #define STREAM_MAX_MSG_LEN  256

    HI_BOOL  g_SessionListIsInit = HI_FALSE;
    static int keepAliveTimeCont = 0;
    #define STREAM_TYPE_VIDEO 1
    #define STREAM_TYPE_VIDEO_AUDIO 2


    HI_S32 langtao_thread;
    MT_CONFIG_Langtao_Device_S struLangtaoDev;
    memset(&struLangtaoDev,0,sizeof(MT_CONFIG_Langtao_Device_S));
    
    struLangtaoDev.chnId = 11;
    strcpy(struLangtaoDev.ipaddr, "61.139.77.71");
    struLangtaoDev.Port = 15961;
    strcpy(struLangtaoDev.userName, "dvr");
    strcpy(struLangtaoDev.password, "dvr");
    struLangtaoDev.deviceId = 1909;
    struLangtaoDev.versionMajor = 1;
    struLangtaoDev.versionMinor = 1;
    struLangtaoDev.mediaType = STREAM_TYPE_VIDEO;
    struLangtaoDev.devideModule = LTVOD_MODULE_PARSVE;
    

	printf("<MTMNG>start register langtao before\n");
	s32Ret = HI_MT_RegisterLangtao(&struLangtaoDev,&langtao_thread);
	printf("<MTMNG>HI_MT_RegisterLangtao ret:%d\n",s32Ret);
	if(s32Ret == HI_SUCCESS)
	{
    	g_langtao_sock = langtao_thread;
    	printf("<MTMNG>HI_MT_RegisterLangtao threadId :%d\n",g_langtao_sock);
	}
        
    while(1)
    {
        sleep(10);
        /**/    
    }
    /*先unregister浪涛, 再做mt的去初始化*/
    HI_MT_UnRegisterLangtao(g_langtao_sock);
    HI_MT_StopSvr();
    HI_MT_Deinit();
    
    return HI_SUCCESS;

    
}

