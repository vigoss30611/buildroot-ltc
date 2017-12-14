// Copyright (C) 2016 InfoTM, warits.wang@infotm.com
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

#ifndef __VIDEOBOX_API_H__
#error YOU SHOULD NOT INCLUDE THIS FILE DIRECTLY, INCLUDE <videobox.h> INSTEAD
#endif

#ifndef __VB_VIDEO_API_H__
#define __VB_VIDEO_API_H__

#define VMAX_INFO_NAME_LENGTH 32
#define VMAX_FILE_NAME_LENGTH 128
#define VENC_H265_ROI_AREA_MAX_NUM        2
#define VENC_H265_INTRA_AREA_MAX_NUM      1
#define VIDEO_HEADER_MAXLEN               128
#define VIDEO_SEI_USER_DATA_MAXLEN        2048
#define VIDEO_SEI_USER_DATA_UUID          16
#define VIDEO_SEI_USER_DATA_VALIDATE_LEN  (VIDEO_SEI_USER_DATA_MAXLEN - VIDEO_SEI_USER_DATA_UUID)
#define VIDEOBOX_PROPERTY_BASE_OFFSET     0x100


typedef enum v_enc_profile {
    VENC_BASELINE,
    VENC_MAIN,
    VENC_HIGH,
}EN_VIDEO_ENCODE_PROFILE;

typedef enum v_stream_type {
    VENC_BYTESTREAM,
    VENC_NALSTREAM,
}EN_VIDEO_STREAM_TYPE;
typedef enum v_media_type {
    VIDEO_MEDIA_NONE,
    VIDEO_MEDIA_MJPEG,
    VIDEO_MEDIA_H264 ,
    VIDEO_MEDIA_HEVC
}EN_VIDEO_MEDIA_TYPE;

typedef enum v_rate_ctrl_type {
    VENC_CBR_MODE,
    VENC_VBR_MODE,
    VENC_FIXQP_MODE,
}EN_VIDEO_RATE_CTRL_TYPE;

typedef enum v_enc_frame_type {
    VENC_HEADER_NO_IN_FRAME_MODE = 0,
    VENC_HEADER_IN_FRAME_MODE,
}EN_VIDEO_ENCODE_FRAME_TYPE;

typedef struct v_basic_info {
    enum v_media_type   media_type ;
    enum v_enc_profile profile;
    enum v_stream_type stream_type;
}ST_VIDEO_BASIC_INFO;

typedef enum v_nvr_display_mode {
    VNVR_QUARTER_SCREEN_MODE = 0,
    VNVR_FULL_SCREEN_MODE,
    VNVR_ONE_THREE_MODE,
    VNVR_INVALID_MODE = 0xFF,
}EN_VIDEO_NVR_DISPLAY_MODE;


typedef struct v_vbr_info {
    int qp_max;
    int qp_min;
    int max_bitrate;
    int threshold;
    int qp_delta;
}ST_VIDEO_VBR_INFO;

typedef struct v_cbr_info {
    int qp_max;
    int qp_min;
    int bitrate;
    int qp_delta; //will affect i frame size
    int qp_hdr;   //when rc_mode is 0,affect first I frame qp,when rc_mode is 1,affect whole frame qp
}ST_VIDEO_CBR_INFO;

typedef struct v_fixqp_info {
    int qp_fix;
}ST_VIDEO_FIXQP_INFO;

typedef struct v_rate_ctrl_info {
    char name[32];
    enum v_rate_ctrl_type rc_mode; //rc_mode = 1(VBR Mode) rc_mode = 0(CBR Mode)
    int gop_length;
    int picture_skip;
    int idr_interval;
    int hrd;
    int hrd_cpbsize;
    int refresh_interval;//longterm case p_refresh internaval must  < idr_interval
    int mbrc;
    int mb_qp_adjustment;
    union {
        struct v_fixqp_info fixqp;
        struct v_vbr_info   vbr;
        struct v_cbr_info   cbr;
    };
}ST_VIDEO_RATE_CTRL_INFO;

typedef struct v_bitrate_info {
    enum v_rate_ctrl_type rc_mode; //rc_mode = 1(VBR Mode) rc_mode = 0(CBR Mode)
    int bitrate;
    int qp_max;
    int qp_min;
    int qp_delta; //will affect i frame size
    int qp_hdr;   //when rc_mode is 0,affect first I frame qp,when rc_mode is 1,affect whole frame qp
    int gop_length;
    int picture_skip;
    int p_frame; //I/P frame interval
    int hrd;
    int hrd_cpbsize;
    int refresh_interval;//longterm case p_refresh internaval must  < idr_interval
    int mbrc;
    int mb_qp_adjustment;
}ST_VIDEO_BITRATE_INFO;

typedef struct v_roi_area {
    int enable;
    uint32_t x, y;
    uint32_t w, h;
    int qp_offset;
}ST_VIDEO_ROI_AREA;

typedef struct v_pip_info{
    char portname[16];
    int x, y;
    int w, h;
    int pic_w, pic_h;
}ST_VIDEO_PIP_INFO;

#if 0
typedef struct v_intra_area {
    int enable;
    int x, y;
    int w, h;
    int qp_offset;
}ST_VIDEO_INTRA_INFO;
#endif

typedef struct v_roi_info {
    struct v_roi_area roi[2];
} ST_VIDEO_ROI_INFO;

typedef struct v_intra_info{
    struct v_roi_area intra[1];
} ST_VIDEO_INTRA_INFO;

typedef struct v_header_info{
    char header[128];
    int headerLen;
} ST_VIDEO_HEADER_INFO;

typedef struct v_frc_info{
    int framebase;
    int framerate;
} ST_VIDEO_FRC_INFO;//target fps = framerate/framebase

typedef enum sei_trigger_mode {
    EN_SEI_TRIGGER_ONCE = 0,
    EN_SEI_TRIGGER_IFRAME,
    EN_SEI_TRIGGER_EVERY_FRAME,
    EN_SEI_TRIGGER_MAX
} EN_SEI_TRIGGER_MODE;

typedef struct v_sei_user_data_info{
    char seiUserData[VIDEO_SEI_USER_DATA_MAXLEN];
    unsigned int length;
    EN_SEI_TRIGGER_MODE enMode;
} ST_VIDEO_SEI_USER_DATA_INFO;

typedef struct v_nvr_info {
    enum v_nvr_display_mode mode;
    unsigned char channel_enable[4];
} ST_VIDEO_NVR_INFO;

typedef enum video_scenario {
    VIDEO_DAY_MODE,
    VIDEO_NIGHT_MODE
} EN_VIDEO_SCENARIO;

static const char *const port_pixel_format[][1] =
{
    { "bpp2"        },
    { "nv12"        },
    { "nv21"        },
    { "rgb565"      },
    { "yuyv422"     },
    { "rgb888"      },
    { "rgba8888"    },
    { "yuv422sp"    },
    { "mjpeg"       },
    { "jpeg"        },
    { "h264"        }
};

static const char *const ipu_port_names[][2] =
{
    /*ipu name, port name*/
    { "badencoder",       "in"    },
    { "badencoder",       "out"   },
    { "bufsync",          "in"    },
    { "bufsync",          "out0"  },
    { "bufsync",          "out1"  },
    { "dg-frame",         "out"   },
    { "dg-math",          "out"   },
    { "dg-ov",            "out"   },
    { "dg-pixel",         "out"   },
    { "ffphoto",          "stream"},
    { "ffphoto",          "frame" },
    { "ffvdec",           "stream"},
    { "ffvdec",           "frame" },
    { "filesink",         "in"    },
    { "filesource",       "out"   },
    { "fodetv2",          "in"    },
    { "fodetv2",          "ORout" },
    { "g1264",            "stream"},
    { "g1264",            "frame" },
    { "g1jdec",           "stream"},
    { "g1jdec",           "frame" },
    { "g2",               "stream"},
    { "g2",               "frame" },
    { "h1264",            "frame" },
    { "h1264",            "stream"},
    { "h1264",            "mv"    },
    { "h1jpeg",           "in"    },
    { "h1jpeg",           "out"   },
    { "h2",               "frame" },
    { "h2",               "stream"},
    { "h2",               "frame" },
    { "h2",               "stream"},
    { "ids",              "osd0"  },
    { "ids",              "osd1"  },
    { "v2500",            "out"   },
    { "v2500",            "his"   },
    { "v2500",            "his1"  },
    { "v2500",            "cap"   },
    { "v2500",            "dsc"   },
    { "isplus",           "out"   },
    { "isplus",           "his"   },
    { "isplus",           "cap"   },
    { "isplus",           "in"    },
    { "isplus",           "ov0"   },
    { "isplus",           "ov1"   },
    { "isplus",           "ov2"   },
    { "isplus",           "ov3"   },
    { "isplus",           "ov4"   },
    { "isplus",           "ov5"   },
    { "isplus",           "ov6"   },
    { "isplus",           "ov7"   },
    { "isplus",           "uo"    },
    { "isplus",           "dn"    },
    { "isplus",           "ss0"   },
    { "isplus",           "ss1"   },
    { "ispost",           "in"    },
    { "ispost",           "ol"    },
    { "ispost",           "dn"    },
    { "ispost",           "ss0"   },
    { "ispost",           "ss1"   },
    { "ispostv2",         "in"    },
    { "ispostv2",         "ov0"   },
    { "ispostv2",         "ov1"   },
    { "ispostv2",         "ov2"   },
    { "ispostv2",         "ov3"   },
    { "ispostv2",         "ov4"   },
    { "ispostv2",         "ov5"   },
    { "ispostv2",         "ov6"   },
    { "ispostv2",         "ov7"   },
    { "ispostv2",         "uo"    },
    { "ispostv2",         "dn"    },
    { "ispostv2",         "ss0"   },
    { "ispostv2",         "ss1"   },
    { "jenc",             "in"    },
    { "jenc",             "out"   },
    { "marker",           "out"   },
    { "mvmovement",       "in"    },
    { "pp",               "in"    },
    { "pp",               "ol0"   },
    { "pp",               "ol1"   },
    { "pp",               "out"   },
    { "softlayer",        "out"   },
    { "softlayer",        "ol0"   },
    { "softlayer",        "ol1"   },
    { "softlayer",        "ol2"   },
    { "softlayer",        "ol3"   },
    { "swc",              "in0"   },
    { "swc",              "in1"   },
    { "swc",              "in2"   },
    { "swc",              "in3"   },
    { "swc",              "dn"    },
    { "swc",              "out"   },
    { "tfestitcher",      "in0"   },
    { "tfestitcher",      "in1"   },
    { "tfestitcher",      "out"   },
    { "v2500",            "out"   },
    { "v2500",            "his"   },
    { "v2500",            "his1"  },
    { "v2500",            "cap"   },
    { "v2505",            "out"   },
    { "v2505",            "his"   },
    { "v2505",            "cap"   },
    { "v2505",            "dsc"   },
    { "v4l2",             "out"   },
    { "vamovement",       "in"    },
    { "vaqrscanner",      "in"    },
    { "vencoder",         "frame" },
    { "vencoder",         "stream"},
    { "vencoder",         "mv"    }
};

static const char *const ipu_instance_names[][2] =
{
    /*ipu name, description*/
    { "badencoder",       "for test"                                                           },
    { "bufsync",          "buffer sync module, to bridge two diff ipu"                         },
    { "dg-frame",         "produce yuv format video stream, for debug only"                    },
    { "dg-math",          "produce yuv format video stream, support [palette] [cloth] [water]" },
    { "dg-ov",            "produce overlay, for debug"                                         },
    { "dg-pixel",         "produce yuv format video stream"                                    },
    { "ffphoto",          "pic software decoder using ffmpeg, only support jpeg pic"           },
    { "ffvdec",           "video software decoder using ffmpeg, only support h264 decoder"     },
    { "filesink",         "save data to file"                                                  },
    { "filesource",       "read data from file"                                                },
    { "fodetv2",          "face detect module"                                                 },
    { "g1264",            "h264 decoder"                                                       },
    { "g1jdec",           "jpeg decoder"                                                       },
    { "g2",               "h265 decoder"                                                       },
    { "h1264",            "video encoder h264"                                                 },
    { "h1jpeg",           "pic encoder module, support jpeg"                                   },
    { "h2",               "video encoder h265"                                                 },
    { "ids",              "display module"                                                     },
    { "v2500",            "isp"                                                                },
    { "isplus",           "include v2500 function, and support 3D denoise"                     },
    { "ispost",           "ispost in Apollo"                                                   },
    { "ispostv2",         "ispost in Apollo-2/ECO"                                             },
    { "jenc",             "pic encoder module, support by Apollo-ECO"                          },
    { "marker",           "make maker on raw image, support NV12/BPP2/RGBA8888 format marker"  },
    { "mvmovement",       "motion detect base on motion vector"                                },
    { "pp",               "video post process"                                                 },
    { "softlayer",        "overlay multi layers using sofeware, only support 4 layer"          },
    { "swc",              "converge video stream for NVR"                                      },
    { "tfestitcher",      "stitcher module"                                                    },
    { "v2500",            "image process core module"                                          },
    { "v4l2",             "output raw video stream through /dev/videox by bypassing v2500"     },
    { "vamovement",       "motion detect base on hisgram info"                                 },
    { "vaqrscanner",      "video analyze module for QRC(Quick Response Code) recognize"        },
    { "vencoder",         "video encoder after QSDK V2.3.0"                                    }
};

typedef enum
{
    //note:
    //a. This enum list should be matched with the array ps8xxxArgsInfos that
    //   defined in videobox/Include/Json.h
    //   eg: ps8IspArgsInfo: {name, type}
    //      { "nbuffers",               SJson_Number },
    //      { "skipframe",              SJson_Number },
    //      { "flip",                   SJson_Number }
    //b. Each IPU must start with 0x100*n and shoule only be add on the end
    E_VIDEOBOX_ISP_NBUFFERS = 0,
    E_VIDEOBOX_ISP_SKIPFRAME,
    E_VIDEOBOX_ISP_FLIP,
    E_VIDEOBOX_ISP_MAX,

    E_VIDEOBOX_ISPOST_NBUFFERS = 0x100,
    E_VIDEOBOX_ISPOST_LCGRIDFILENAME1,
    E_VIDEOBOX_ISPOST_LCENABLE,
    E_VIDEOBOX_ISPOST_LCGRIDTARGETINDEX,
    E_VIDEOBOX_ISPOST_DNENABLE,
    E_VIDEOBOX_ISPOST_FCEFILE,
    E_VIDEOBOX_ISPOST_FISHEYE,
    E_VIDEOBOX_ISPOST_LCGEOMETRYFILENAME1,
    E_VIDEOBOX_ISPOST_LCGRIDLINEBUFENABLE,
    E_VIDEOBOX_ISPOST_DNTARGETINDEX,
    E_VIDEOBOX_ISPOST_LINKMODE,
    E_VIDEOBOX_ISPOST_LINKBUFFERS,
    E_VIDEOBOX_ISPOST_MAX,

    E_VIDEOBOX_VENCODER_ENCODERTYPE = 0x200, /*0: h264 1:h265*/
    E_VIDEOBOX_VENCODER_BUFFERSIZE,
    E_VIDEOBOX_VENCODER_MINBUFFERNUM,
    E_VIDEOBOX_VENCODER_FRAMERATE,
    E_VIDEOBOX_VENCODER_FRAMEBASE,
    E_VIDEOBOX_VENCODER_R0TATE,
    E_VIDEOBOX_VENCODER_SMARTRC,
    E_VIDEOBOX_VENCODER_REFMODE,
    E_VIDEOBOX_VENCODER_MVW,
    E_VIDEOBOX_VENCODER_MVH,
    E_VIDEOBOX_VENCODER_SMARTP, /* enable_longterm */
    E_VIDEOBOX_VENCODER_CHROMQPOFFSET,
    E_VIDEOBOX_VENCODER_RATECTRL, /* rc_mode and rc paras It's value is a struct */
    E_VIDEOBOX_VENCODER_MAX,

    E_VIDEOBOX_H1JPEG_MODE = 0x300,
    E_VIDEOBOX_H1JPEG_ROTATE,
    E_VIDEOBOX_H1JPEG_BUFSIZE,
    E_VIDEOBOX_H1JPEG_USEBACKBUF,
    E_VIDEOBOX_H1JPEG_THUMBNAILMODE,
    E_VIDEOBOX_H1JPEG_THUMBNAILWIDTH,
    E_VIDEOBOX_H1JPEG_THUMBNAILHEIGHT,
    E_VIDEOBOX_H1JPEG_MAX,

    E_VIDEOBOX_JENC_MODE = 0x400,
    E_VIDEOBOX_JENC_BUFSIZE,
    E_VIDEOBOX_JENC_MAX,

    E_VIDEOBOX_PP_ROTATE = 0x500,
    E_VIDEOBOX_PP_MAX,

    E_VIDEOBOX_MARKER_MODE = 0x600,
    E_VIDEOBOX_MARKER_MAX,

    E_VIDEOBOX_IDS_NOOSD1 = 0x700,
    E_VIDEOBOX_IDS_MAX,

    E_VIDEOBOX_DGFRAME_NBUFFERS = 0x800,
    E_VIDEOBOX_DGFRAME_DATA,
    E_VIDEOBOX_DGFRAME_MODE,
    E_VIDEOBOX_DGFRAME_MAX,

    E_VIDEOBOX_FODETV2_IMGWIDTH = 0x900,
    E_VIDEOBOX_FODETV2_IMGHEIGHT,
    E_VIDEOBOX_FODETV2_IMGST,
    E_VIDEOBOX_FODETV2_CROPX,
    E_VIDEOBOX_FODETV2_CROPY,
    E_VIDEOBOX_FODETV2_CROPW,
    E_VIDEOBOX_FODETV2_CROPH,
    E_VIDEOBOX_FODETV2_DBFILENAME,
    E_VIDEOBOX_FODETV2_MAX,

    E_VIDEOBOX_VA_SENDBLKINFO = 0xA00,
    E_VIDEOBOX_VA_MAX,

    E_VIDEOBOX_FILESINK_DATAPATH = 0xB00,
    E_VIDEOBOX_FILESINK_MAXSAVECNT,
    E_VIDEOBOX_FILESINK_MAX,

    E_VIDEOBOX_FILESOURCE_DATAPATH = 0xC00,
    E_VIDEOBOX_FILESOURCE_FRAMESIZE,
    E_VIDEOBOX_FILESOURCE_MAX,

    E_VIDEOBOX_V4L2_DRIVER = 0xD00,
    E_VIDEOBOX_V4L2_CAMERA,
    E_VIDEOBOX_V4L2_PIXFMT,
    E_VIDEOBOX_V4L2_MAX,

    E_VIDEOBOX_ARGS_END
} EN_VIDEOBOX_PROPERTY;

typedef struct
{
    unsigned int u32Width;
    unsigned int u32Height;
    unsigned int u32PipX;
    unsigned int u32PipY;
    unsigned int u32PiPWidth;
    unsigned int u32PipHeight;
    unsigned int u32LinkMode;
    unsigned int u32LockRatio;
    unsigned int u32TimeOutValue;
    unsigned int u32MinFps;
    unsigned int u32MaxFps;
    unsigned int u32Export;
    char s8PixelFormat[10];
} ST_VIDEOBOX_PORT_ATTRIBUTE;

typedef enum
{
    E_VIDEOBOX_OK = 0,
    E_VIDEOBOX_ERROR = -1,
    E_VIDEOBOX_NULL_ARGUMENT = -2,
    E_VIDEOBOX_INVALID_ARGUMENT = -3,
    E_VIDEOBOX_INVALID_PORT_ATTRIBUTE = -4,
    E_VIDEOBOX_MEMORY_ERROR = -5,
    E_VIDEOBOX_INVALID_PORT = -7,
    E_VIDEOBOX_PORT_ERROR = -8,
    E_VIDEOBOX_TIMEOUT = -9
} EN_VIDEOBOX_RET;

typedef struct
{
    const unsigned char u8Name[VMAX_INFO_NAME_LENGTH];
    unsigned int u32Size; /*can not change this value if allocate success*/
    unsigned int u32PhyAddr;
    unsigned int u32Width;
    unsigned int u32Height;
    unsigned int u32Stride;
    void* pVirtualAddr;
} ST_VIDEOBOX_BUFFER;

typedef struct {
    EN_VIDEOBOX_PROPERTY enProperty;
    char PropertyInfo[VMAX_FILE_NAME_LENGTH];
} ST_VIDEOBOX_ARGS_INFO;

typedef struct {
    char s8PortName[VMAX_INFO_NAME_LENGTH];
    ST_VIDEOBOX_PORT_ATTRIBUTE stPortAttribute;
} ST_VIDEOBOX_PORT_INFO;

typedef struct {
    char s8SrcPortName[VMAX_INFO_NAME_LENGTH];
    char s8SinkIpuID[VMAX_INFO_NAME_LENGTH];
    char s8SinkPortName[VMAX_INFO_NAME_LENGTH];
} ST_VIDEOBOX_BIND_EMBEZZLE_INFO;

const char **video_get_channels(void);
int video_enable_channel(const char *channel);
int video_disable_channel(const char *channel);
int video_get_basicinfo(const char *channel, struct v_basic_info *info);
int video_set_basicinfo(const char *channel, struct v_basic_info *info);
int video_get_bitrate(const char *channel, struct v_bitrate_info *info); //will abondon later
int video_set_bitrate(const char *channel, struct v_bitrate_info *info); //will abondon later
int video_get_ratectrl(const char *channel, struct v_rate_ctrl_info *info);
int video_set_ratectrl(const char *channel, struct v_rate_ctrl_info *info);
int video_set_ratectrl_ex(const char *channel, struct v_rate_ctrl_info *info);
int video_get_rtbps(const char *channel);
int video_test_frame(const char *channel, struct fr_buf_info *ref);
int video_get_frame(const char *channel, struct fr_buf_info *ref);
int video_put_frame(const char *channel, struct fr_buf_info *ref);
int video_get_fps(const char *channel);
int video_set_fps(const char *channel, int fps);
int video_get_resolution(const char *channel, int *w, int *h);
int video_set_resolution(const char *channel, int w, int h);
int video_get_roi_count(const char *channel);
int video_get_roi(const char *channel, struct v_roi_info *roi);
int video_set_roi(const char *channel, struct v_roi_info *roi);
int video_set_snap_quality(const char *channel, int qlevel);
int video_get_snap(const char *channel, struct fr_buf_info *ref);
int video_put_snap(const char *channel, struct fr_buf_info *ref);
int video_get_header(const char *channel, char *buf, int len);
int video_set_header(const char *channel, char *buf, int len);
int video_get_tail(const char *channel, char *buf, int len);
int video_trigger_key_frame(const char *chn);
int video_write_frame(const char *chn, void *data, int len);
int video_set_header(const char *chn, char *buf, int len);
enum v_enc_frame_type video_get_frame_mode(const char *channel);
int video_set_frame_mode(const char *channel, enum v_enc_frame_type mode);

int video_set_scenario(const char *channel, enum video_scenario mode);
int video_set_pip_info(const char *channel, struct v_pip_info *pip_info);
int video_get_pip_info(const char *channel, struct v_pip_info *pip_info);
int video_set_nvr_mode(const char *channel,struct v_nvr_info *nvr_mode);

enum video_scenario video_get_scenario(const char *channel);

int video_set_frc(const char *channel, struct v_frc_info *frc);
int video_get_frc(const char *channel, struct v_frc_info *frc);
int video_get_thumbnail(const char *chn, struct fr_buf_info *ref);
int video_put_thumbnail(const char *chn, struct fr_buf_info *ref);
int thumbnail_set_resolution(const char *chn, int w, int h);

/* set and get ipu property, different type with different property, use void*
 * to unify, the property value please reference enum EN_VIDEOBOX_PROPERTY and
 * doc Videobox */
EN_VIDEOBOX_RET videobox_ipu_set_property(const char *ps8IpuID, EN_VIDEOBOX_PROPERTY enProperty, void *pPropertyInfo, int s32Size);
EN_VIDEOBOX_RET videobox_ipu_get_property(const char *ps8IpuID, EN_VIDEOBOX_PROPERTY enProperty, void *pPropertyInfo, int s32Size);

/* set and get ipu port attribute */
EN_VIDEOBOX_RET videobox_ipu_set_attribute(const char *ps8IpuID, const char *ps8PortName, ST_VIDEOBOX_PORT_ATTRIBUTE *pstPortAttribute);
EN_VIDEOBOX_RET videobox_ipu_get_attribute(const char *ps8IpuID, const char *ps8PortName, ST_VIDEOBOX_PORT_ATTRIBUTE *pstPortAttribute);

/*
 *  Bind: connect two IPU with compatible port attribute
 *  unbind : Disable Two Connected IPU, can be any node in path
 *  Input argument: source ipu ID and port name/dest ipu ID and port name
 */
EN_VIDEOBOX_RET videobox_ipu_bind(const char *ps8SrcIpuID, const char *ps8SrcPortName, const char *ps8SinkIpuID, const char *ps8SinkPortName);
EN_VIDEOBOX_RET videobox_ipu_unbind(const char *ps8SrcIpuID, const char *ps8SrcPortName, const char *ps8SinkIpuID, const char *ps8SinkPortName);

/*
 *   Embezzle and unEmbezzle Videobox IPU The usage need check
 *   Embezzle : Embezzle two IPU with compatible port attribute
 *   unEmbezzle  : Disable Two Connected Embezzling IPU
 */
EN_VIDEOBOX_RET videobox_ipu_embezzle(const char *ps8SrcIpuID, const char *ps8SrcPortName, const char *ps8SinkIpuID, const char *ps8SinkPortName);
EN_VIDEOBOX_RET videobox_ipu_unembezzle(const char *ps8SrcIpuID, const char *ps8SrcPortName, const char *ps8SinkIpuID, const char *ps8SinkPortName);

/* create videobox process */
EN_VIDEOBOX_RET videobox_create_path(void);
/* delete path and then kill videobox process */
EN_VIDEOBOX_RET videobox_delete_path(void);

/*  start stop the whole process */
EN_VIDEOBOX_RET videobox_start_path(void);
EN_VIDEOBOX_RET videobox_stop_path(void);

/* trigger to run, change status from init to running conflict with previous
 * start change to trigger */
EN_VIDEOBOX_RET videobox_trigger(void);
/* pending to stop, change status from running to stop, conflict with previous
 * stop change to suspend */
EN_VIDEOBOX_RET videobox_suspend(void);

/* create ipu then add property and attribute with other function
 * ps8IpuID -In ipuname, format: ipuname-(self define name). eg: v2500-isp, v2500-0
 * Ex:  “v2500” that define in DYNAMIC_IPU(IPU_V2500, "v2500")
 *      ipu_instance_names list all ipu instance name in pair of name and description */
EN_VIDEOBOX_RET videobox_ipu_create(const char *ps8IpuID);
EN_VIDEOBOX_RET videobox_ipu_delete(const char *ps8IpuID);
//EN_VIDEOBOX_RET videobox_ipu_port_enable(char *pu8IpuID, char *pu8PortName, ST_VIDEOBOX_PORT_ATTRIBUTE *pstPortAttribute);
//EN_VIDEOBOX_RET videobox_ipu_port_disable(char *pu8IpuID, char *pu8PortName);
//EN_VIDEOBOX_RET videobox_get_block(ST_VIDEOBOX_BUFFER *pstBuffer);
//EN_VIDEOBOX_RET videobox_release_block(ST_VIDEOBOX_BUFFER *pstBuffer);

/*
 * @brief set sei user data to encoder
 * @param ps8Chn           -IN: the channel of encoder
 * @param ps8Buf           -IN: the sei data buffer
 * @param s32Len           -IN: the length of sei data buffer
 * @return 0: Success
 * @return <0: Failure
 */
int video_set_sei_user_data(const char *ps8Chn, char *ps8Buf, int s32Len, EN_SEI_TRIGGER_MODE enMode);

/*
 * @brief set slice height to encoder
 * @param ps8Chn                   -IN: the channel of encoder
 * @param ps32SliceHeight    -IN: slice height in frame with unit pixel,encode one frame with
                                                       (frame_height/slice_height)slices,  0(default) to encode each picture in one slice
                                                       slice height will auto align (16/h264)/(64/h265)
 * @return <0: Failure
 */
int video_set_slice_height(const char *ps8Chn, int s32SliceHeight);
/*
 * @brief get slice height from encoder
 * @param ps8Chn                   -IN: the channel of encoder
 * @param ps32SliceHeight    -IN/OUT: slice height in frame with unit pixel 16
                                                       may be not the same value as set becauseof align(16/h264)/(64/h265)
 * @return <0: Failure
 */
int video_get_slice_height(const char *ps8Chn, int *ps32SliceHeight);

/*
 * @brief set enable app1 encoded in photo jpeg, only support app1 xmp ID
 * @param ps8Chn                   -IN: the channel of encoder
 * @param s32Enable              -IN: 1 enable 0 disable
 * @return <0: Failure
 */
int video_set_panoramic(const char *ps8Chn, int s32Enable);


#define VIDEO_FRAME_HEADER 0x100
#define VIDEO_FRAME_I   0x101
#define VIDEO_FRAME_P   0x102
#define VIDEO_FRAME_B   0x103
#define VIDEO_FRAME_DUMMY   0x110

#define AUDIO_FRAME_HEADER 0x110
#define AUDIO_FRAME_RAW    0x111

#endif /* __VB_VIDEO_API_H__ */
