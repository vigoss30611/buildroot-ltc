/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: VEncode header file
 *
 * Author:
 *     beca.zhang <beca.zhang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  09/12/2017 init
 */
#ifndef IPU_VENCODER_H
#define IPU_VENCODER_H
#include <IPU.h>
#include <VEncoderWrapper.h>
#include <assert.h>
#define VENCODER_RET_ERROR -1
#define VENCODER_RET_OK 0

#ifdef VENCODER_DEPEND_H2V4
/*
 *  E_GOPCFG1: I <- P <- P <- P
 *
 *             /------./------./------.
 *  E_GOPCFG2: I <- P  P <- P  P <- P  P ....
 *
 *              /-------------.  /------------.
 *             /-------.       |/------.       |
 *  COPCFG4: I <- P  P <- P  P <- P  P <- P  P  <- P ....
 */

typedef enum _setting_status
{
    E_STATUS_INIT = 0,
    E_STATUS_SETTING,
    E_STATUS_UPDATING,
}EN_SETTING_STATUS;
#endif

typedef enum
{
    E_GOPCFG1 = 0,
    E_GOPCFG2,
    E_GOPCFG4,
}EN_GOP_CFG;

typedef struct
{
    uint32_t u32OriginW;
    uint32_t u32OriginH;
    uint32_t u32StartX;
    uint32_t u32StartY;
    uint32_t u32OffsetW;
    uint32_t u32OffsetH;
    uint32_t u32Rotation;
    Pixel::Format enSrcFrameType;
}ST_PREPROCESS;

class IPU_VENCODER : public IPU
{
    private:
        static std::mutex *ms_pMutexHwLock;
        static int ms_s32InstanceCounter;
        static bool ms_bSmartrcStarted;
        std::mutex  *m_pMutexSettingLock;

        U_ENCODE                    m_unEncode;
        bENCODE                     *m_pEncode;
        ST_EncConfig                m_stEncConfig;
        ST_EncCodingCtrl            m_stEncCodingCtrl;
        ST_EncRateCtrl              m_stEncRateCtrl;
        ST_EncIn                    m_stEncIn;
        ST_EncOut                   m_stEncOut;
        ST_EncPreProcessingCfg      m_stEncPreProCfg;
        ST_VENC_INFO                m_stVencInfo;
        ST_VIDEO_SEI_USER_DATA_INFO *m_pstSei;
        ST_PREPROCESS               m_stVencPpCfg;
        ST_VIDEO_RATE_CTRL_INFO     m_stRateCtrlInfo;
        ST_FR_BUF_INFO              m_stVideoScenarioBuffer;
        ST_VIDEO_BASIC_INFO         m_stVencEncConfig;
        ST_VIDEO_BITRATE_INFO       m_stVencRateCtrl;
        ST_VIDEO_ROI_INFO           m_stVencRoiAreaInfo;
        ST_VIDEO_FRC_INFO           m_stFrcInfo;
        EN_VIDEO_SCENARIO           m_enVideoScenarioMode;
        EN_VIDEO_ENCODE_FRAME_TYPE  m_enFrameMode;
#ifdef VENCODER_DEPEND_H2V4
        EN_SETTING_STATUS           m_enCtbRcChange;
        HEVCGopPicConfig            *m_pstGopPicConfig;
#endif

        bool m_bIsH264Lib;
        bool m_bIsHeaderReady;
        bool m_bVencEncConfigUpdateFlag;
        bool m_bVencRateCtrlUpdateFlag;
        bool m_bVencCodingCtrlUpdateFlag;
        bool m_bSEIUserDataSetFlag;
        EN_SEI_TRIGGER_MODE m_enSEITriggerMode;
        bool m_bTriggerKeyFrameFlag;
        uint8_t m_u8RateCtrlReset; /* 0:not reset   1: need check other condition 2:force reset*/
        uint32_t m_s32RateCtrlDistance;
        char *m_ps8Header;
        int m_s32IdrInterval;
        int m_s32Infps;
        int m_s32CurFps;
        int m_s32AvgFps;
        int m_s32DebugCnt;
        int m_s32DebugFpsCnt;
        int m_s32HeaderLength;
        int m_s32SrcFrameSize;
        int m_s32InFpsRatio;
        int m_s32FrameDuration;
        int m_s32RefMode;// status of  Spatial  Layers  1:enable  0:disable
        int m_s32Smartrc;// status of  smartRC  1:enable  0:disable
#ifdef VENCODER_DEPEND_H1V6
        int m_s32EnableLongterm;// status of  smartP  1: enable  0:disable
        int m_s32RefreshCnt;
#endif
        int m_s32RefreshInterval;
        int m_s32ChromaQpOffset;
        int m_SliceSize;
        long m_s32FrameCnt;
        uint64_t m_u64DebugTotalSize;
        uint64_t m_u64DebugLastFrameTime;
        uint64_t m_u64RtBps;
        uint64_t m_u64MaxBitRate;

        int      m_s32VbrPreRtnQuant;
        uint32_t m_u32VbrThreshold;
        uint32_t m_u32VbrPicSkip;
        uint64_t m_u64VbrTotalFrame;
        double   m_dVbrTotalSize;
        double   m_dVbrSeqQuality;
        double   m_dVbrAvgFrameSize;
        double   m_dpVbrQuantError[52];

        void BasicParaInit(void);
        int EncodeInstanceInit(void);
        int EncodeStartStream(Buffer &OutBuf);
        int EncodeStrmEncode(Buffer &InBuf, Buffer &OutBuf);
        int EncodeEndStream(Buffer *InBuf, Buffer *OutBuf);
        int EncodeSetRateCtrl(void);
        int EncodeSetCodingCfg(void);
        int EncodeSetPreProcessCfg(void);
        int EncodeInstanceRelease(void);
        bool GetLevelFromResolution(uint32_t s32X, uint32_t s32Y, int *level);
        int SwitchFormat(Pixel::Format enSrcFormat);
        int SaveHeader(char *ps8Buf, int s32Len);
        bool CheckFrcUpdate(void);
#ifdef VENCODER_DEPEND_H2V4
        const char * nextToken (const char *ps8Str);
        int ParseGopConfigString (const char *ps8Line, HEVCGopConfig *pstGopCfg,
                int s32FrameIndex, int s32GopSize);
        int SetGopConfigMode(uint32_t u32Mode);
#endif
        int SetBasicInfo(ST_VIDEO_BASIC_INFO *pstCfg);
        int GetBasicInfo(ST_VIDEO_BASIC_INFO *pstCfg);
        int SetBitrate(ST_VIDEO_BITRATE_INFO *pstCfg);
        int GetBitrate(ST_VIDEO_BITRATE_INFO *pstCfg);
        int SetRateCtrl(ST_VIDEO_RATE_CTRL_INFO *pstCfg);
        int GetRateCtrl(ST_VIDEO_RATE_CTRL_INFO *pstCfg);
        int SetROI(ST_VIDEO_ROI_INFO *pstCfg);
        int GetROI(ST_VIDEO_ROI_INFO *pstCfg);
        int SetFrameMode(EN_VIDEO_ENCODE_FRAME_TYPE enArg);
        int GetFrameMode();
        int SetFrc(ST_VIDEO_FRC_INFO *pstInfo);
        int GetFrc(ST_VIDEO_FRC_INFO *pstInfo);
        int SetScenario(EN_VIDEO_SCENARIO stMode);
        int SetSeiUserData(ST_VIDEO_SEI_USER_DATA_INFO *pstSei);
        int SetPipInfo(ST_VIDEO_PIP_INFO *pstPipInfo);
        int GetPipInfo(ST_VIDEO_PIP_INFO *pstPipInfo);
        EN_VIDEO_SCENARIO GetScenario(void);
        int GetRTBps();
        int TriggerKeyFrame(void);
        void VideoInfo(Buffer *dst);
        bool VbrProcess(Buffer *dst);
        bool TryLockSettingResource(void);
        bool LockSettingResource(void);
        bool UnlockSettingResource(void);
        bool TrylockHwResource(void);
        bool LockHwResource(void);
        bool UnlockHwResource(void);
        void UpdateDebugInfo(int s32Update);
        int GetDebugInfo(void *pInfo, int *ps32Size);
        void CheckCtbRcUpdate(void);
        int SetSliceHeight(int s32SliceHeight);
        int GetSliceHeight(int *ps32SliceHeight);
    public:
        IPU_VENCODER(std::string, Json *);
        ~IPU_VENCODER();
        void Prepare();
        void Unprepare();
        int UnitControl(std::string, void *);
        void WorkLoop();
};

#endif /* IPU_VENCODER_H */
