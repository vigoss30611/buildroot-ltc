// Copyright (C) 2016 InfoTM, yong.yan@infotm.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef IPU_H1264_H
#define IPU_H1264_H

#include <IPU.h>
#include <h264encapi.h>
#include <h264encapi_ext.h>
#include <atomic>

#define H264RET_OK 0
#define H264RET_ERR -1

typedef struct {
    int originW;
    int originH;
    int startX;
    int startY;
    int offsetW;
    int offsetH;
    int rotate;
}h264LocPpCfg;


class IPU_H1264 : public IPU{
    private:
        ST_VENC_INFO m_stVencInfo;
        int Infps;
        int CurFps;
        int AvgFps;
        bool IsFPSChange;
        bool IsHeaderReady;
        static int InstanceCnt;
        static bool bSmartrcStarted;
        bool TriggerKeyFrameFlag;
        std::mutex *MutexSettingLock;
        int InfpsRatio;
        int Frc;
        int FrameDuration;
        int counter;
        uint64_t total_time, total_sizes;
        uint64_t aver_time, aver_size;
        uint64_t last_frame_time;
        uint64_t MaxBitrate;
        uint32_t VBRThreshHold;
        uint32_t vbr_pic_skip;
        uint64_t t_total_size, t_last_frame_time;
        uint64_t rtBps;
        int chroma_qp_offset;
        int t_cnt, t_fps_cnt;
        int refresh_interval;
        int enable_longterm;// status of  smartP  1: enable  0:disable
        int refresh_cnt;
        int smartrc;// status of  smartRC  1:enable  0:disable
        int s32RefMode;// status of  Spatial  Layers  1:enable  0:disable
        struct fr_buf_info  VideoScenarioBuffer;
        enum video_scenario  VideoScenarioMode;
        int      pre_rtn_quant;
        uint64_t total_frames;
        double   total_size;
        double   sequence_quality;
        double   avg_framesize;
        double   quant_error[52];
        v_frc_info FrcInfo;
        v_enc_frame_type frame_mode;
        struct v_rate_ctrl_info rate_ctrl_info;
        ST_VIDEO_SEI_USER_DATA_INFO *m_pstSei;
        EN_SEI_TRIGGER_MODE m_enSEITriggerMode;
        uint8_t m_u8RateCtrlReset; /* 0:not reset   1: need check other condition 2:force reset*/
        uint32_t m_s32RateCtrlDistance;

        bool CheckFrcUpdate(void);
        bool VbrProcess(Buffer *dst);
        bool VbrAvgProcess(Buffer *dst);
        void VideoInfo(Buffer *dst);
        int SetPipInfo(struct v_pip_info *vpinfo);
        int GetPipInfo(struct v_pip_info *vpinfo);

    public:
        long FrameCnt;
        int pIdrInterval;
        char *pH264Header;
        int pH264HeaderLen;
        H264EncIn pH264EncIn;
        H264EncOut pH264EncOut;
        H264EncInst pH264EncInst;
        H264EncConfig pH264EncCfg;
        h264LocPpCfg pH264LocPpCfg;
        Pixel::Format pH264InFrameType;
        H264EncRateCtrl pH264EncRateCtrl;
        H264EncCodingCtrl pH264EncCodingCfg;
        H264EncPreProcessingCfg pH264EncPreProcCfg;

        struct v_basic_info pH264EncLocBasicCfg;
        struct v_bitrate_info pH264EncLocBRCfg;
        struct v_roi_info pH264EncLocRoiAreaInfo;

        bool H264EncCfgUpdateFlag;
        bool H264EncRateCtrlUpateFlag;
        bool H264EncCodingCtrlUpdateFlag;
        bool m_bSEIUserDataSetFlag;

        IPU_H1264(std::string, Json *);
        ~IPU_H1264();
        void Prepare();
        void Unprepare();
        void WorkLoop();
        bool H264SaveHeader(char *buf, int len);
        void H264EncParamsInit();
        bool IsH264InstReady();
        void ParamCheck();
        int SetScenario(enum video_scenario);
        enum video_scenario GetScenario(void);
        int SetROI(struct v_roi_info *info);
        int GetROI(struct v_roi_info *info);
        int GetBasicInfo(struct v_basic_info *info);
        int SetBasicInfo(struct v_basic_info *info);
        int GetBitrate(struct v_bitrate_info *info);
        int SetBitrate(struct v_bitrate_info *info);
        int SetRateCtrl(struct v_rate_ctrl_info *cfg);
        int GetRateCtrl(struct v_rate_ctrl_info *cfg);
        int GetRTBps();
        int SetFrameMode(enum v_enc_frame_type arg);
        int GetFrameMode();
        int ExH264EncInit();
        void ExH264EncUpdateFPS();
        int ExH264EncSetRateCtrl();
        int ExH264EncSetCodingCtrl();
        int ExH264EncSetPreProcessing();
        int ExH264EncStrmStart(Buffer *bfout);
        int ExH264EncStrmEncode(Buffer *bfIn, Buffer *bfOut);
        int ExH264EncStrmEnd(Port *pin, Port *pout);
        int TriggerKeyFrame();
        bool ExH264EncRelease();

        int SetFrc(v_frc_info *info);
        int GetFrc(v_frc_info *info);
        int UnitControl(std::string, void *);
        void UpdateDebugInfo(int s32Update);

        /*
        * @brief set sei user data to encoder
        * @param stSei              -IN: the sei info
        * @return 0: Success
        * @return <0: Failure
        */
        int SetSeiUserData(ST_VIDEO_SEI_USER_DATA_INFO *pstSei);
        int GetDebugInfo(void *pInfo, int *ps32Size);
};
#endif
