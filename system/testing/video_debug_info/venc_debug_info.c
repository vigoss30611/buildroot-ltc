#include <qsdk/videobox.h>

static const char *GetMetaType(EN_IPU_TYPE stType)
{
    switch(stType)
    {
        case IPU_H264:
            return "H264";
        case IPU_H265:
            return "H265";
        case IPU_JPEG:
            return "JPEG";
        default:
            return "N/A";
    }
}

static const char *GetMetaState(EN_CHANNEL_STATE stState)
{
    switch(stState)
    {
        case INIT:
            return "INIT";
        case RUN:
            return "RUNNING";
        case STOP:
            return "STOPED";
        case IDLE:
            return "IDLE";
        default:
            return "N/A";
    }
}

static const char *GetMetaRCMode(EN_RC_MODE stRCMode)
{
    switch (stRCMode)
    {
        case VBR:
            return "VBR";
        case CBR:
            return "CBR";
        case FIXQP:
            return "FIXQP";
        default:
            return "N/A";
    }
}

static const char *GetMetaRealQP(int s32QP)
{
    char ps8Buf[10];

    if (s32QP > 0)
    {
        sprintf(ps8Buf, "%d", s32QP);
        return ps8Buf;
    }
    else
    {
        return "N/A";
    }
}

void ShowVencInfo(ST_VENC_DBG_INFO *pstDbgInfo)
{
    int s32Index = 0;
    ST_VENC_INFO *pstVencInfo = pstDbgInfo->stVencInfo;

    printf("----Channel info------------------------------------------------------------------------------------\n");
    printf("%-25s%-6s%-8s%-6s%-8s%-6s%-16s%-16s%-16s\n",
            "ChannelName", "Type", "State", "Width", "Height", "FPS", "FrameIn", "FrameOut", "FrameDrop");

    for (s32Index = 0; s32Index < pstDbgInfo->s8Size; s32Index++)
    {
        printf("%-25s%-6s%-8s%-6d%-8d%-6.1f%-16lld%-16lld%-16d\n",
            (pstVencInfo + s32Index)->ps8Channel,
            GetMetaType((pstVencInfo + s32Index)->enType),
            GetMetaState((pstVencInfo + s32Index)->enState),
            (pstVencInfo + s32Index)->s32Width,
            (pstVencInfo + s32Index)->s32Height,
            (pstVencInfo + s32Index)->f32FPS,
            (pstVencInfo + s32Index)->u64InCount,
            (pstVencInfo + s32Index)->u64OutCount,
            (pstVencInfo + s32Index)->u32FrameDrop
        );

    }

    printf("\n----Rate control info--------------------------------------------------------------------------------\n");
    printf("%-25s%-8s%-8s%-8s%-8s%-8s%-12s%-8s%-8s%-16s%-14s%-16s%-16s\n",
            "ChannelName", "RCMode", "MinQP", "MaxQP", "QPDelta", "RealQP", "I-Interval", "GOP", "Mbrc", "ThreshHold(%)", "PictureSkip", "SetBR(kbps)", "BitRate(kbps)");

    for (s32Index = 0; s32Index < pstDbgInfo->s8Size; s32Index++)
    {
        if ((pstVencInfo + s32Index)->enType == IPU_H264
            || (pstVencInfo + s32Index)->enType == IPU_H265)
        {
            printf("%-25s%-8s%-8d%-8d%-8d%-8s%-12d%-8d%-8d%-16d%-14s%-16.1f%-16.1f\n",
                (pstVencInfo + s32Index)->ps8Channel,
                GetMetaRCMode((pstVencInfo + s32Index)->stRcInfo.enRCMode),
                (pstVencInfo + s32Index)->stRcInfo.s8MinQP,
                (pstVencInfo + s32Index)->stRcInfo.s8MaxQP,
                (pstVencInfo + s32Index)->stRcInfo.s32QPDelta,
                GetMetaRealQP((pstVencInfo + s32Index)->stRcInfo.s32RealQP),
                (pstVencInfo + s32Index)->stRcInfo.s32IInterval,
                (pstVencInfo + s32Index)->stRcInfo.s32Gop,
                (pstVencInfo + s32Index)->stRcInfo.s32Mbrc,
                (pstVencInfo + s32Index)->stRcInfo.u32ThreshHold,
                (pstVencInfo + s32Index)->stRcInfo.s32PictureSkip > 0 ?"Enable":"Disable",
                (pstVencInfo + s32Index)->stRcInfo.f32ConfigBitRate,
                (pstVencInfo + s32Index)->f32BitRate
            );
        }
    }
}

void main()
{
    ST_VENC_DBG_INFO stInfo;
    stInfo.s8Capcity = ARRAY_SIZE;
    videobox_get_debug_info("venc", &stInfo, sizeof(stInfo));
    ShowVencInfo(&stInfo);
}

