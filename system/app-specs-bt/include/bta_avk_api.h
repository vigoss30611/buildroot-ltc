/*****************************************************************************
**
**  Name:           bta_avk_api.h
**
**  Description:    This is the public interface file for the advanced
**                  audio/video streaming (AV) subsystem of BTA, Widcomm's
**                  Bluetooth application layer for mobile phones.
**
**  Copyright (c) 2004-2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_AVK_API_H
#define BTA_AVK_API_H

#include "avrc_api.h"
#include "avdt_api.h"
#include "a2d_api.h"
#include "vdp_api.h"
#include "bta_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* AVK status values */
#define BTA_AVK_SUCCESS          0       /* successful operation */
#define BTA_AVK_FAIL             1       /* generic failure */
#define BTA_AVK_FAIL_SDP         2       /* service not found */
#define BTA_AVK_FAIL_STREAM      3       /* stream connection failed */
#define BTA_AVK_FAIL_RESOURCES   4       /* no resources */
#define BTA_AVK_FAIL_AVRCP_CTRL  5       /* AVRCP control channel failed */

typedef UINT8 tBTA_AVK_STATUS;

/* AVK features masks */
#define BTA_AVK_FEAT_RCTG        0x0001  /* remote control target */
#define BTA_AVK_FEAT_RCCT        0x0002  /* remote control controller */
#define BTA_AVK_FEAT_PROTECT     0x0004  /* streaming media content protection */
#define BTA_AVK_FEAT_VENDOR      0x0008  /* remote control vendor dependent commands */
#define BTA_AVK_FEAT_BROWSE      0x0010  /* browsing channel support */
#define BTA_AVK_FEAT_REPORT      0x0020  /* use reporting service for VDP */
#define BTA_AVK_FEAT_DELAY_RPT   0x0040  /* use delay reporting */
#define BTA_AVK_FEAT_METADATA    0x0080  /* AVRC Metadata transfer supported */

typedef UINT16 tBTA_AVK_FEAT;

/* AVK channel values */
#define BTA_AVK_CHNL_AUDIO       0  /* audio channel */
#define BTA_AVK_CHNL_VIDEO       1  /* video channel */
typedef UINT8 tBTA_AVK_CHNL;

/* handle index to mask */
#define BTA_AVK_HNDL_TO_MSK(h)       ((UINT8)(1 << (h)))

/* offset of codec type in codec info byte array */
//#define BTA_AVK_CODEC_TYPE_IDX       AVDT_CODEC_TYPE_INDEX   /* 2 */
#define BTA_AVK_CODEC_TYPE_IDX      2   /* AVDT_CODEC_TYPE_INDEX */



/* maximum number of streams created: 1 for audio, 1 for video */
#ifndef BTA_AVK_NUM_STRS
#define BTA_AVK_NUM_STRS         2
#endif

#ifndef BTA_AVK_MAX_SEPS
#define BTA_AVK_MAX_SEPS         2
#endif

#define BTA_AVK_MAX_MTU          1008

#define BTA_AVK_CO_METADATA      AVRC_CO_METADATA

/* codec type */
#define BTA_AVK_CODEC_SBC        A2D_MEDIA_CT_SBC        /* SBC media codec type */
#define BTA_AVK_CODEC_M12        A2D_MEDIA_CT_M12        /* MPEG-1, 2 Audio media codec type */
#define BTA_AVK_CODEC_M24        A2D_MEDIA_CT_M24        /* MPEG-2, 4 AAC media codec type */
#define BTA_AVK_CODEC_ATRAC      A2D_MEDIA_CT_ATRAC      /* ATRAC family media codec type */
#define BTA_AVK_CODEC_H263_P0    VDP_MEDIA_CT_H263_P0    /* H.263 baseline (profile 0) */
#define BTA_AVK_CODEC_MPEG4      VDP_MEDIA_CT_MPEG4      /* MPEG-4 Visual Simple Profile */
#define BTA_AVK_CODEC_H263_P3    VDP_MEDIA_CT_H263_P3    /* H.263 profile 3 */
#define BTA_AVK_CODEC_H263_P8    VDP_MEDIA_CT_H263_P8    /* H.263 profile 8 */
#define BTA_AVK_CODEC_VEND       VDP_MEDIA_CT_VEND       /* Non-VDP */

typedef UINT8 tBTA_AVK_CODEC;

/* Company ID in BT assigned numbers */
#define BTA_AVK_BT_VENDOR_ID     VDP_BT_VENDOR_ID        /* Broadcom Corporation */

/* vendor specific codec ID */
#define BTA_AVK_CODEC_ID_H264    VDP_CODEC_ID_H264       /* Non-VDP codec ID - H.264 */
#define BTA_AVK_CODEC_ID_IMG     VDP_CODEC_ID_IMG        /* Non-VDP codec ID - images/slideshow */

/* operation id list for BTA_AvkRemoteCmd */
#define BTA_AVK_RC_SELECT        AVRC_ID_SELECT      /* select */
#define BTA_AVK_RC_UP            AVRC_ID_UP          /* up */
#define BTA_AVK_RC_DOWN          AVRC_ID_DOWN        /* down */
#define BTA_AVK_RC_LEFT          AVRC_ID_LEFT        /* left */
#define BTA_AVK_RC_RIGHT         AVRC_ID_RIGHT       /* right */
#define BTA_AVK_RC_RIGHT_UP      AVRC_ID_RIGHT_UP    /* right-up */
#define BTA_AVK_RC_RIGHT_DOWN    AVRC_ID_RIGHT_DOWN  /* right-down */
#define BTA_AVK_RC_LEFT_UP       AVRC_ID_LEFT_UP     /* left-up */
#define BTA_AVK_RC_LEFT_DOWN     AVRC_ID_LEFT_DOWN   /* left-down */
#define BTA_AVK_RC_ROOT_MENU     AVRC_ID_ROOT_MENU   /* root menu */
#define BTA_AVK_RC_SETUP_MENU    AVRC_ID_SETUP_MENU  /* setup menu */
#define BTA_AVK_RC_CONT_MENU     AVRC_ID_CONT_MENU   /* contents menu */
#define BTA_AVK_RC_FAV_MENU      AVRC_ID_FAV_MENU    /* favorite menu */
#define BTA_AVK_RC_EXIT          AVRC_ID_EXIT        /* exit */
#define BTA_AVK_RC_0             AVRC_ID_0           /* 0 */
#define BTA_AVK_RC_1             AVRC_ID_1           /* 1 */
#define BTA_AVK_RC_2             AVRC_ID_2           /* 2 */
#define BTA_AVK_RC_3             AVRC_ID_3           /* 3 */
#define BTA_AVK_RC_4             AVRC_ID_4           /* 4 */
#define BTA_AVK_RC_5             AVRC_ID_5           /* 5 */
#define BTA_AVK_RC_6             AVRC_ID_6           /* 6 */
#define BTA_AVK_RC_7             AVRC_ID_7           /* 7 */
#define BTA_AVK_RC_8             AVRC_ID_8           /* 8 */
#define BTA_AVK_RC_9             AVRC_ID_9           /* 9 */
#define BTA_AVK_RC_DOT           AVRC_ID_DOT         /* dot */
#define BTA_AVK_RC_ENTER         AVRC_ID_ENTER       /* enter */
#define BTA_AVK_RC_CLEAR         AVRC_ID_CLEAR       /* clear */
#define BTA_AVK_RC_CHAN_UP       AVRC_ID_CHAN_UP     /* channel up */
#define BTA_AVK_RC_CHAN_DOWN     AVRC_ID_CHAN_DOWN   /* channel down */
#define BTA_AVK_RC_PREV_CHAN     AVRC_ID_PREV_CHAN   /* previous channel */
#define BTA_AVK_RC_SOUND_SEL     AVRC_ID_SOUND_SEL   /* sound select */
#define BTA_AVK_RC_INPUT_SEL     AVRC_ID_INPUT_SEL   /* input select */
#define BTA_AVK_RC_DISP_INFO     AVRC_ID_DISP_INFO   /* display information */
#define BTA_AVK_RC_HELP          AVRC_ID_HELP        /* help */
#define BTA_AVK_RC_PAGE_UP       AVRC_ID_PAGE_UP     /* page up */
#define BTA_AVK_RC_PAGE_DOWN     AVRC_ID_PAGE_DOWN   /* page down */
#define BTA_AVK_RC_POWER         AVRC_ID_POWER       /* power */
#define BTA_AVK_RC_VOL_UP        AVRC_ID_VOL_UP      /* volume up */
#define BTA_AVK_RC_VOL_DOWN      AVRC_ID_VOL_DOWN    /* volume down */
#define BTA_AVK_RC_MUTE          AVRC_ID_MUTE        /* mute */
#define BTA_AVK_RC_PLAY          AVRC_ID_PLAY        /* play */
#define BTA_AVK_RC_STOP          AVRC_ID_STOP        /* stop */
#define BTA_AVK_RC_PAUSE         AVRC_ID_PAUSE       /* pause */
#define BTA_AVK_RC_RECORD        AVRC_ID_RECORD      /* record */
#define BTA_AVK_RC_REWIND        AVRC_ID_REWIND      /* rewind */
#define BTA_AVK_RC_FAST_FOR      AVRC_ID_FAST_FOR    /* fast forward */
#define BTA_AVK_RC_EJECT         AVRC_ID_EJECT       /* eject */
#define BTA_AVK_RC_FORWARD       AVRC_ID_FORWARD     /* forward */
#define BTA_AVK_RC_BACKWARD      AVRC_ID_BACKWARD    /* backward */
#define BTA_AVK_RC_ANGLE         AVRC_ID_ANGLE       /* angle */
#define BTA_AVK_RC_SUBPICT       AVRC_ID_SUBPICT     /* subpicture */
#define BTA_AVK_RC_F1            AVRC_ID_F1          /* F1 */
#define BTA_AVK_RC_F2            AVRC_ID_F2          /* F2 */
#define BTA_AVK_RC_F3            AVRC_ID_F3          /* F3 */
#define BTA_AVK_RC_F4            AVRC_ID_F4          /* F4 */
#define BTA_AVK_RC_F5            AVRC_ID_F5          /* F5 */

typedef UINT8 tBTA_AVK_RC;

/* state flag for pass through command */
#define BTA_AVK_STATE_PRESS      AVRC_STATE_PRESS    /* key pressed */
#define BTA_AVK_STATE_RELEASE    AVRC_STATE_RELEASE  /* key released */

typedef UINT8 tBTA_AVK_STATE;

/* command codes for BTA_AvkVendorCmd and BTA_AvkMetaCmd */
#define BTA_AVK_CMD_CTRL         AVRC_CMD_CTRL
#define BTA_AVK_CMD_STATUS       AVRC_CMD_STATUS
#define BTA_AVK_CMD_SPEC_INQ     AVRC_CMD_SPEC_INQ
#define BTA_AVK_CMD_NOTIF        AVRC_CMD_NOTIF
#define BTA_AVK_CMD_GEN_INQ      AVRC_CMD_GEN_INQ

typedef UINT8 tBTA_AVK_CMD;

/* response codes for BTA_AvkVendorRsp and BTA_AvkMetaRsp */
#define BTA_AVK_RSP_NOT_IMPL     AVRC_RSP_NOT_IMPL
#define BTA_AVK_RSP_ACCEPT       AVRC_RSP_ACCEPT
#define BTA_AVK_RSP_REJ          AVRC_RSP_REJ
#define BTA_AVK_RSP_IN_TRANS     AVRC_RSP_IN_TRANS
#define BTA_AVK_RSP_IMPL_STBL    AVRC_RSP_IMPL_STBL
#define BTA_AVK_RSP_CHANGED      AVRC_RSP_CHANGED
#define BTA_AVK_RSP_INTERIM      AVRC_RSP_INTERIM

typedef UINT8 tBTA_AVK_CODE;

/* error codes for BTA_AvkProtectRsp */
#define BTA_AVK_ERR_NONE             A2D_SUCCESS         /* Success, no error */
#define BTA_AVK_ERR_BAD_STATE        AVDT_ERR_BAD_STATE  /* Message cannot be processed in this state */
#define BTA_AVK_ERR_RESOURCE         AVDT_ERR_RESOURCE   /* Insufficient resources */
#define BTA_AVK_ERR_BAD_CP_TYPE      A2D_BAD_CP_TYPE     /* The requested Content Protection Type is not supported */
#define BTA_AVK_ERR_BAD_CP_FORMAT    A2D_BAD_CP_FORMAT   /* The format of Content Protection Data is not correct */

typedef UINT8 tBTA_AVK_ERR;

/* AV callback events */
#define BTA_AVK_ENABLE_EVT       0       /* AV enabled */
#define BTA_AVK_REGISTER_EVT     1       /* registered to AVDT */
#define BTA_AVK_OPEN_EVT         2       /* connection opened */
#define BTA_AVK_CLOSE_EVT        3       /* connection closed */
#define BTA_AVK_START_EVT        4       /* stream data transfer started */
#define BTA_AVK_STOP_EVT         5       /* stream data transfer stopped */
#define BTA_AVK_PROTECT_REQ_EVT  6       /* content protection request */
#define BTA_AVK_PROTECT_RSP_EVT  7       /* content protection response */
#define BTA_AVK_RC_OPEN_EVT      8       /* remote control channel open */
#define BTA_AVK_RC_CLOSE_EVT     9       /* remote control channel closed */
#define BTA_AVK_REMOTE_CMD_EVT   10      /* remote control command */
#define BTA_AVK_REMOTE_RSP_EVT   11      /* remote control response */
#define BTA_AVK_VENDOR_CMD_EVT   12      /* vendor dependent remote control command */
#define BTA_AVK_VENDOR_RSP_EVT   13      /* vendor dependent remote control response */
#define BTA_AVK_RECONFIG_EVT     14      /* reconfigure response */
#define BTA_AVK_SUSPEND_EVT      15      /* suspend response */
#define BTA_AVK_UPDATE_SEPS_EVT  16      /* update SEPS to available or unavailable */
#define BTA_AVK_META_MSG_EVT     17      /* metadata command/response received */
#define BTA_AVK_RC_CMD_TOUT_EVT  18      /* Timeout waiting for response for AVRC command */
#define BTA_AVK_RC_PEER_FEAT_EVT 19      /* Peer features notification (sdp results for peer initiated AVRC connection) */
#define BTA_AVK_BROWSE_MSG_EVT   20      /* browse command/response received */
typedef UINT8 tBTA_AVK_EVT;


#define BTA_AVK_CLOSE_STR_CLOSED    1
#define BTA_AVK_CLOSE_CONN_LOSS     2

typedef UINT8   tBTA_AVK_CLOSE_REASON;

/* Event associated with BTA_AVK_REGISTER_EVT */
typedef struct
{
    tBTA_AVK_CHNL    chnl;       /* audio/video */
    UINT8           app_id;     /* ID associated with call to BTA_AvkRegister() */
    tBTA_AVK_STATUS  status;
} tBTA_AVK_REGISTER;

/* Event associated with BTA_AVK_UPDATE_SEPS_EVT */
typedef struct
{
    UINT16  status;
    BOOLEAN available;
} tBTA_AVK_UPDATE_SEPS;

/* data associated with BTA_AVK_OPEN_EVT */
typedef struct
{
    tBTA_AVK_CHNL    chnl;
    BD_ADDR         bd_addr;
    tBTA_AVK_STATUS  status;
    BOOLEAN         starting;
} tBTA_AVK_OPEN;

/* data associated with BTA_AVK_CLOSE_EVT */
typedef struct
{
    tBTA_AVK_CHNL    chnl;
    tBTA_AVK_CLOSE_REASON   reason;
} tBTA_AVK_CLOSE;

/* data associated with BTA_AVK_START_EVT */
typedef struct
{
    tBTA_AVK_CHNL    chnl;
    tBTA_AVK_STATUS  status;
} tBTA_AVK_START;

/* data associated with BTA_AVK_SUSPEND_EVT */
typedef struct
{
    tBTA_AVK_CHNL    chnl;
    tBTA_AVK_STATUS  status;
} tBTA_AVK_SUSPEND;

/* data associated with BTA_AVK_RECONFIG_EVT */
typedef struct
{
    tBTA_AVK_CHNL    chnl;
    tBTA_AVK_STATUS  status;
} tBTA_AVK_RECONFIG;

/* data associated with BTA_AVK_PROTECT_REQ_EVT */
typedef struct
{
    tBTA_AVK_CHNL    chnl;
    UINT8           *p_data;
    UINT16          len;
} tBTA_AVK_PROTECT_REQ;

/* data associated with BTA_AVK_PROTECT_RSP_EVT */
typedef struct
{
    tBTA_AVK_CHNL    chnl;
    UINT8           *p_data;
    UINT16          len;
    tBTA_AVK_ERR     err_code;
} tBTA_AVK_PROTECT_RSP;

/* Info from peer's AVRC SDP record (included in BTA_AVK_RC_OPEN_EVT) */
typedef struct
{
    UINT16 version;         /* AVRCP version */
    UINT16 features;        /* Supported features (see AVRC_SUPF_* definitions in avrc_api.h) */
} tBTA_AVK_RC_INFO;

/* data associated with BTA_AVK_RC_OPEN_EVT */
typedef struct
{
    tBTA_AVK_STATUS  status;        /* whether RC open successes or fails */
    UINT8            rc_handle;
    BD_ADDR          bd_addr;
    tBTA_AVK_FEAT    peer_features;
    tBTA_AVK_RC_INFO peer_tg;       /* Peer TG role info */
    tBTA_AVK_RC_INFO peer_ct;       /* Peer CT role info */
} tBTA_AVK_RC_OPEN;

/* data associated with BTA_AVK_RC_PEER_FEAT_EVT */
typedef struct
{
    UINT8               rc_handle;
    tBTA_AVK_FEAT       roles;              /* BTA_AVK_FEAT_RCTG or BTA_AVK_FEAT_RCCT */
    tBTA_AVK_RC_INFO    tg;                 /* Peer TG role info */
    tBTA_AVK_RC_INFO    ct;                 /* Peer CT role info */
} tBTA_AVK_RC_PEER_FEAT;

/* data associated with BTA_AVK_RC_CLOSE_EVT */
typedef struct
{
    UINT8            rc_handle;
} tBTA_AVK_RC_CLOSE;

/* data associated with BTA_AVK_REMOTE_CMD_EVT */
typedef struct
{
    UINT8            rc_handle;
    tBTA_AVK_RC      rc_id;
    tBTA_AVK_STATE   key_state;
} tBTA_AVK_REMOTE_CMD;

/* data associated with BTA_AVK_REMOTE_RSP_EVT */
typedef struct
{
    UINT8            rc_handle;
    tBTA_AVK_RC      rc_id;
    tBTA_AVK_STATE   key_state;
    tBTA_AVK_CODE    rsp_code;
    UINT8           label;
} tBTA_AVK_REMOTE_RSP;

/* data associated with BTA_AVK_VENDOR_CMD_EVT, BTA_AVK_VENDOR_RSP_EVT */
typedef struct
{
    UINT8           rc_handle;
    UINT16          len;
    UINT8           label;
    tBTA_AVK_CODE    code;
    UINT32          company_id;
    UINT8           *p_data;
} tBTA_AVK_VENDOR;

/* Data for BTA_AVK_META_MSG_EVT / */
typedef struct
{
    UINT8           rc_handle;
    UINT8           label;
    tBTA_AVK_CODE   code;           /* AVRCP command or response code (see BTA_AVK_CMD_* or BTA_AVK_RSP_* definitions) */
    tAVRC_MSG       *p_msg;
    UINT16          len;
    UINT8           *p_data;
} tBTA_AVK_META_MSG;

/* Data for BTA_AVK_RC_CMD_TOUT_EVT */
typedef struct
{
    UINT8           rc_handle;
    UINT8           label;          /* label of command that timed out */
} tBTA_AVK_RC_CMD_TOUT;

/* union of data associated with AV callback */
typedef union
{
    tBTA_AVK_CHNL        chnl;
    tBTA_AVK_REGISTER    registr;
    tBTA_AVK_UPDATE_SEPS update_seps;
    tBTA_AVK_OPEN        open;
    tBTA_AVK_CLOSE       close;
    tBTA_AVK_START       start;
    tBTA_AVK_PROTECT_REQ protect_req;
    tBTA_AVK_PROTECT_RSP protect_rsp;
    tBTA_AVK_RC_OPEN     rc_open;
    tBTA_AVK_RC_CLOSE    rc_close;
    tBTA_AVK_RC_CMD_TOUT rc_cmd_tout;
    tBTA_AVK_REMOTE_CMD  remote_cmd;
    tBTA_AVK_REMOTE_RSP  remote_rsp;
    tBTA_AVK_VENDOR      vendor_cmd;
    tBTA_AVK_VENDOR      vendor_rsp;
    tBTA_AVK_META_MSG    meta_msg;
    tBTA_AVK_RECONFIG    reconfig;
    tBTA_AVK_SUSPEND     suspend;
    tBTA_AVK_RC_PEER_FEAT   rc_peer_feat;
} tBTA_AVK;

/* AV callback */
typedef void (tBTA_AVK_CBACK)(tBTA_AVK_EVT event, tBTA_AVK *p_data);

/* type for stream state machine action functions */
typedef void (*tBTA_AVK_ACT)(void *p_cb, void *p_data);

/* type for registering VDP */
typedef void (tBTA_AVK_REG) (tAVDT_CS *p_cs, char *p_service_name, void *p_data);

/* AV configuration structure */
typedef struct
{
    UINT32  company_id;         /* AVRCP Company ID */
    UINT16  avrc_mtu;           /* AVRCP MTU at L2CAP */
    UINT16  avrc_br_mtu;        /* AVRCP MTU at L2CAP for the Browsing channel */
    UINT16  avrc_ct_cat;        /* AVRCP controller categories */
    UINT16  avrc_tg_cat;        /* AVRCP target categories */
    UINT16  sig_mtu;            /* AVDTP signaling channel MTU at L2CAP */

    /* Audio channel configuration */
    UINT16  audio_mtu;          /* AVDTP audio transport channel MTU at L2CAP */
    UINT16  audio_flush_to;     /* AVDTP audio transport channel flush timeout */
    UINT16  audio_mqs;          /* AVDTP audio channel max data queue size */
    const tBTA_AVK_ACT *p_audio_act_tbl;/* the action function table for VDP stream */
    tBTA_AVK_REG       *p_audio_reg;    /* action function to register VDP */

    /* Video channel configuration */
    UINT16  video_mtu;          /* AVDTP video transport channel MTU at L2CAP */
    UINT16  video_flush_to;     /* AVDTP video transport channel flush timeout */
    const tBTA_AVK_ACT *p_video_act_tbl;/* the action function table for VDP stream */
    tBTA_AVK_REG       *p_video_reg;    /* action function to register VDP */
    char               avrc_controller_name[BTA_SERVICE_NAME_LEN]; /* Default AVRCP controller name */
    char               avrc_target_name[BTA_SERVICE_NAME_LEN];     /* Default AVRCP target name */
} tBTA_AVK_CFG;

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/

/*******************************************************************************
**
** Function         BTA_AvkEnable
**
** Description      Enable the advanced audio/video service. When the enable
**                  operation is complete the callback function will be
**                  called with a BTA_AVK_ENABLE_EVT. This function must
**                  be called before other function in the AV API are
**                  called.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkEnable(tBTA_SEC sec_mask, tBTA_AVK_FEAT features,
                          tBTA_AVK_CBACK *p_cback);

/*******************************************************************************
**
** Function         BTA_AvkDisable
**
** Description      Disable the advanced audio/video service.
**
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkDisable(void);

/*******************************************************************************
**
** Function         BTA_AvkRegister
**
** Description      Register the audio or video service to stack. When the
**                  operation is complete the callback function will be
**                  called with a BTA_AVK_REGISTER_EVT. This function must
**                  be called before AVDT stream is open.
**
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkRegister(tBTA_AVK_CHNL chnl, const char *p_service_name,
                            UINT8 app_id);

/*******************************************************************************
**
** Function         BTA_AvkDeregister
**
** Description      Deregister the audio or video service
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkDeregister(tBTA_AVK_CHNL chnl);

/*******************************************************************************
**
** Function         BTA_AvkOpen
**
** Description      Opens an advanced audio/video connection to a peer device.
**                  When connection is open callback function is called
**                  with a BTA_AVK_OPEN_EVT for each channel.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkOpen(BD_ADDR bd_addr, tBTA_AVK_CHNL chnl, tBTA_SEC sec_mask);

/*******************************************************************************
**
** Function         BTA_AvkClose
**
** Description      Close the current streams.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkClose(void);

/*******************************************************************************
**
** Function         BTA_AvkStart
**
** Description      Start audio/video stream data transfer.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkStart(void);

/*******************************************************************************
**
** Function         BTA_AvkStop
**
** Description      Stop audio/video stream data transfer.
**                  If suspend is TRUE, this function sends AVDT suspend signal
**                  to the connected peer(s).
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkStop(BOOLEAN suspend);

/*******************************************************************************
**
** Function         BTA_AvkReconfig
**
** Description      Reconfigure the audio/video stream.
**                  If suspend is TRUE, this function tries the suspend/reconfigure
**                  procedure first.
**                  If suspend is FALSE or when suspend/reconfigure fails,
**                  this function closes and re-opens the AVDT connection.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkReconfig(tBTA_AVK_CHNL chnl, BOOLEAN suspend, UINT8 sep_info_idx,
                            UINT8 *p_codec_info, UINT8 num_protect, UINT8 *p_protect_info);

/*******************************************************************************
**
** Function         BTA_AvkProtectReq
**
** Description      Send a content protection request.  This function can only
**                  be used if AV is enabled with feature BTA_AVK_FEAT_PROTECT.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkProtectReq(tBTA_AVK_CHNL chnl, UINT8 *p_data, UINT16 len);

/*******************************************************************************
**
** Function         BTA_AvkProtectRsp
**
** Description      Send a content protection response.  This function must
**                  be called if a BTA_AVK_PROTECT_REQ_EVT is received.
**                  This function can only be used if AV is enabled with
**                  feature BTA_AVK_FEAT_PROTECT.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkProtectRsp(tBTA_AVK_CHNL chnl, UINT8 error_code, UINT8 *p_data,
                              UINT16 len);

/*******************************************************************************
**
** Function         BTA_AvkRemoteCmd
**
** Description      Send a remote control command.  This function can only
**                  be used if AV is enabled with feature BTA_AVK_FEAT_RCCT.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkRemoteCmd(UINT8 rc_handle, UINT8 label, tBTA_AVK_RC rc_id,
                             tBTA_AVK_STATE key_state);

/*******************************************************************************
**
** Function         BTA_AvkVendorCmd
**
** Description      Send a vendor dependent remote control command.  This
**                  function can only be used if AV is enabled with feature
**                  BTA_AVK_FEAT_VENDOR.
**
**                  Note - If compnay id is 0, the default company ID will be used
**                         from bta_avk_cfg.c. Company id could be BT_SIG_COMP_ID (0x001958)
**                         for AVRCP vendor specific command or assigned company id.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkVendorCmd(UINT8 rc_handle, UINT8 label, tBTA_AVK_CODE cmd_code,
                             UINT8 *p_data, UINT8 len, UINT32 company_id);

/*******************************************************************************
**
** Function         BTA_AvkVendorRsp
**
** Description      Send a vendor dependent remote control response.
**                  This function must be called if a BTA_AVK_VENDOR_CMD_EVT
**                  is received. This function can only be used if AV is
**                  enabled with feature BTA_AVK_FEAT_VENDOR.
**
**                  Note - If compnay id is 0, the default company ID will be used
**                         from bta_avk_cfg.c. Company id could be BT_SIG_COMP_ID (0x001958)
**                         for AVRCP vendor specific command or assigned company id.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkVendorRsp(UINT8 rc_handle, UINT8 label, tBTA_AVK_CODE rsp_code,
                             UINT8 *p_data, UINT8 len, UINT32 company_id);

/*******************************************************************************
**
** Function         BTA_AvkUpdateStreamEndPoints
**
** Description      Change all the sink SEPs to available or unavailable
**                  When the update operation is complete the callback function
**                  will be called with a BTA_AVK_UPDATE_SEPS_EVT
**
** Parameter        available: True Set all SEPs to available
**                             FALSE Set all SEPs to unavailable
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkUpdateStreamEndPoints(BOOLEAN available);


/*******************************************************************************
**
** Function         BTA_AvkMetaCmd
**
** Description      Send an avrc metadata command.
**
**                  Can only be called if AVK was enabled with feature mask
**                  BTA_AVK_FEAT_METADATA and BTA_AVK_FEAT_RCCT set.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkMetaCmd(UINT8 rc_handle, UINT8 label, tBTA_AVK_CMD cmd_code, BT_HDR *p_pkt);

/*******************************************************************************
**
** Function         BTA_AvkMetaRsp
**
** Description      Send an avrc metadata response (after receiving a
**                  metadata command)
**
**                  Can only be called if AVK was enabled with feature mask
**                  BTA_AVK_FEAT_METADATA and BTA_AVK_FEAT_RCTG set.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkMetaRsp(UINT8 rc_handle, UINT8 label, tBTA_AVK_CODE rsp_code, BT_HDR *p_pkt);

/*******************************************************************************
**
** Function         BTA_AvkDelayReport
**
** Description      Send a delay report.  This function can only
**                  be used if AV is enabled with feature BTA_AVK_FEAT_DELAY_RPT.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkDelayReport(tBTA_AVK_CHNL chnl, UINT16 delay);

/*******************************************************************************
**
** Function         BTA_AvkOpenRc
**
** Description      Opens an AVRCP connection to a peer device.
**                  When connection is open callback function is called
**                  with a BTA_AVK_RC_OPEN_EVT.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkOpenRc(void);

/*******************************************************************************
**
** Function         BTA_AvkCloseRc
**
** Description      Closes an AVRCP connection to a peer device.
**                  When connection is open callback function is called
**                  with a BTA_AVK_RC_CLOSE_EVT.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkCloseRc(UINT8 rc_handle);

/* BSA_SPECIFIC */
#if (defined(BTA_AVK_AV_AUDIO_RELAY) && (BTA_AVK_AV_AUDIO_RELAY==TRUE))
/*******************************************************************************
**
** Function         BTA_AvkRelayAudio
**
** Description      Relay AVK audio to AV
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_AvkRelayAudio(BOOLEAN relay);

#endif

#ifdef __cplusplus
}
#endif

#endif /* BTA_AVK_API_H */
