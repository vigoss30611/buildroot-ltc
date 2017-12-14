// Copyright (C) 2016 InfoTM, bright.jiang@infotm.com
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

#ifndef IPU_H2_H
#define IPU_H2_H
#include <IPU.h>
#include <hevcencapi.h>
#define IPY_H2_MAX_NAL_SIZE 200
#define DEFAULT_H2_PARA -255

/*
 *  GOPCFG1: I <- P <- P <- P
 *
 *           /------./------./------.
 *  GOPCFG2: I <- P  P <- P  P <- P  P ....
 *
 *            /-------------.  /------------.
 *           /-------.       |/------.       |
 *  COPCFG4: I <- P  P <- P  P <- P  P <- P  P  <- P ....
 */
typedef enum {
    GOPCFG1 = 0,
    GOPCFG2,
    GOPCFG4,
}gop_cfg_e;

typedef enum _setting_status{
    E_STATUS_INIT = 0,
    E_STATUS_SETTING,
    E_STATUS_UPDATING,
}EN_SETTING_STATUS;

typedef struct {
    uint32_t originW;
    uint32_t originH;
    uint32_t startX ;
    uint32_t startY ;
    uint32_t offsetW ;
    uint32_t offsetH ;
    uint32_t rotation ;
    Pixel::Format srcFrameType ;
}hevc_preprocess_t ;
//#define MAX_GOP_PIC_CONFIG_NUM 30
class IPU_H2 : public IPU {
    private:
        static std::mutex *MutexHwLock; //lock hw resource
        static int InstanceCounter ; //object instance counter
        static bool bSmartrcStarted;

        std::mutex  *MutexSettingLock ;



        std::thread WorkThread;

        bool HeaderReadyFlag ;
        EN_SETTING_STATUS enCtbRcChange;
        char *Header;
        int HeaderLength;
        HEVCEncInst EncInst;
        HEVCEncIn EncIn;
        HEVCEncOut EncOut;
        HEVCEncConfig EncConfig;
        HEVCEncCodingCtrl EncCodingCtrl;
        HEVCEncRateCtrl   EncRateCtrl;
        HEVCEncPreProcessingCfg EncPreProcessingCfg;
        hevc_preprocess_t  VencPpCfg;
        ST_VENC_INFO m_stVencInfo;
    //    Pixel::Format SrcFrameType;

        int FrameWidth ;
        int FrameHeight ;
        int FrameSize;
        int FrameCounter;
        int TotolFrameCounter;
        int IdrInterval;
        int Infps ;
        int CurFps;
        int AvgFps;
        bool IsFPSChange;
        bool IsHeaderReady;
        bool m_bSEIUserDataSetFlag;
        int counter;
        int      pre_rtn_quant;
        int      t_cnt, t_fps_cnt;
        int chroma_qp_offset;
        uint64_t total_time, total_sizes;
        uint64_t aver_time, aver_size;
        uint64_t last_frame_time;
        uint64_t t_total_size, t_last_frame_time;
        uint64_t rtBps;
        uint64_t MaxBitrate;
        uint32_t VBRThreshHold;
        uint32_t vbr_pic_skip;
        uint32_t EncNalMaxSize;
        uint64_t total_frames;
        double   total_size;
        double   sequence_quality;
        double   avg_framesize;
        double   quant_error[52];
        int s32RefMode;// status of  Spatial  Layers  2:GOPCFG4  1:GOPCFG2  0:GOPCFG1
        int smartrc;// status of  smartRC  1:enable  0:disable
        uint32_t m_s32RateCtrlDistance;
        uint8_t m_u8RateCtrlReset;
        struct fr_buf_info  RoiMapDeltaQpFrBuf;
        struct v_rate_ctrl_info rate_ctrl_info;
        ST_VIDEO_SEI_USER_DATA_INFO *m_pstSei;
        EN_SEI_TRIGGER_MODE m_enSEITriggerMode;

        bool VbrProcess(Buffer *dst);
        bool VbrAvgProcess(Buffer *dst);
        bool TrylockHwResource(void);
        bool LockHwResource(void);
        bool UnlockHwResource(void);

        bool TryLockSettingResource(void);
        bool LockSettingResource(void);
        bool UnlockSettingResource(void);

        //unit control setting info
        struct v_basic_info     VencEncodeConfig ;
        //VENC_ENCODE_CONFIG_T     VencEncodeConfig ;
        struct v_bitrate_info   VencRateCtrl ;
        //VENC_RATE_CTRL_T         VencRateCtrl ;
        struct v_roi_area       VencRoiAreaList[VENC_H265_ROI_AREA_MAX_NUM];
        struct v_roi_info       VencRoiAreaInfo;
        //VENC_ROI_AREA_T         VencRoiAreaList[VENC_H265_ROI_AREA_MAX_NUM];
        struct v_roi_area         VencIntraAreaInfo;
        //VENC_INTRA_AREA_T         VencIntraArea;

        bool VencEncodeConfigUpdateFlag ;
        bool VencRateCtrlUpateFlag ;
        bool VencCodingCtrlUpdateFlag ;

        bool GetLevelFromResolution(uint32_t  x,uint32_t y ,int *level) ;
        bool CheckAreaValid(struct v_roi_area *cfg);
        bool TriggerKeyFrameFlag ;

        int PrepareCounter;
        int UnPrepareCounter;
        v_frc_info FrcInfo;

        struct fr_buf_info  VideoScenarioBuffer;
        enum video_scenario  VideoScenarioMode;
        enum v_enc_frame_type frame_mode;
        int InfpsRatio;
        int Frc;
        int FrameDuration;
        uint32_t GopConfigMode;
        const char * nextToken (const char *str);
        int ParseGopConfigString(const char *line, HEVCGopConfig *gopCfg, int frame_idx, int gopSize);
        int SetGopConfigMode(uint32_t mode);
        void ShowHevcEncIn(HEVCEncIn *cfg);
        bool CheckFrcUpdate(void);
        void VideoInfo(Buffer *dst);
        void HevcEncodeUpdateFPS(void);
        void CheckCtbRcUpdate(void);
        int SetPipInfo(struct v_pip_info *vpinfo);
        int GetPipInfo(struct v_pip_info *vpinfo);

    public:

    int GetBasicInfo(struct v_basic_info *info);
    int SetBasicInfo(struct v_basic_info *info);
    int GetBitrate(struct v_bitrate_info *info);
    int SetBitrate(struct v_bitrate_info *info);
    int GetROI(struct v_roi_info *info);
    int SetROI(struct v_roi_info *info);
    int SetRateCtrl(struct v_rate_ctrl_info *cfg);
    int GetRateCtrl(struct v_rate_ctrl_info *cfg);
    int GetRTBps();

    int SetScenario(enum video_scenario);
    void UpdateDebugInfo(int s32Update);
    enum video_scenario GetScenario(void);

        IPU_H2(std::string, Json *);
        ~IPU_H2();
        // IPU_H2(fdsafs);
        void Prepare();
        void Unprepare();
        void WorkLoop();
        int UnitControl(std::string, void *);
        int Index ;

        bool IsHevcReady(void);
        void HevcBasicParaInit(void);
        int HevcInstanceInit(void);
        int HevcEncodeStartStream(Buffer &OutBuf);
        int HevcEncodePictureStream(Buffer &InBuf, Buffer &OutBuf);
        int HevcEncodeEndStream(Buffer *InBuf, Buffer *OutBuf);
        int HevcInstanceRelease(void);
        int HevcEncodeSetCodingCfg(void);
        int HevcEncodeSetPreProcessCfg(void);
        int HevcEncodeSetRateCtrl(void);
        int SetFrameMode(enum v_enc_frame_type arg);
        int GetFrameMode();
        void SetIndex(int Index);
        int SaveHeader(char *buf, int len);
        int TriggerKeyFrame(void);
        int SetFrc(v_frc_info *info);
        int GetFrc(v_frc_info *info);
        /*
        * @brief set sei user data to encoder
        * @param stSei              -IN: the sei info
        * @return 0: Success
        * @return <0: Failure
        */
        int SetSeiUserData(ST_VIDEO_SEI_USER_DATA_INFO *pstSei);
        int GetDebugInfo(void *pInfo, int *ps32Size);
};

#define H2_RET_ERROR -1
#define H2_RET_OK 0


#endif /* IPU_H2_H */
