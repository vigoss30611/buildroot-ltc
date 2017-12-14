/*****************************************************************************
**
**  Name:           bta_rc_api.h
**
**  Description:    BTA stand-alone AVRC API (BTA_RC).
**
**                  BTA_RC is intended for use platforms that do not include
**                  AV source (BTA_AV) nor AV sink (BTA_AVK).
**
**                  For platforms that include the BTA_AV or BTA_AVK module,
**                  the RC APIs from those respective modules should be
**                  used instead.
**
**  Copyright (c) 2013, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_RC_API_H
#define BTA_RC_API_H

#include "avrc_api.h"
#include "avdt_api.h"
#include "bta_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* Feature mask definitions (for BTA_RcEnable) */
#define BTA_RC_FEAT_CONTROL         0x0001  /* controller role */
#define BTA_RC_FEAT_TARGET          0x0002  /* target role */
#define BTA_RC_FEAT_INT             0x0004  /* initiator */
#define BTA_RC_FEAT_ACP             0x0008  /* acceptor */
#define BTA_RC_FEAT_VENDOR          0x0010  /* vendor dependent commands */
#define BTA_RC_FEAT_METADATA        0x0020  /* metadata Transfer */
typedef UINT32  tBTA_RC_FEAT;

/* Reserved value for invalid bta_rc handle */
#define BTA_RC_INVALID_HANDLE   0xFF

/* Default subunit extension code for use with BTA_RcSubunitInfoCmd */
#define BTA_RC_SUBUNIT_DEFAULT_EXTENSION_CODE   0x07

/* Event definitions for tBTA_RC_CBACK (common CT/TG events) */
#define BTA_RC_ENABLE_EVT           1
#define BTA_RC_DISABLE_EVT          2
#define BTA_RC_OPEN_EVT             3       /* Connection to peer AVRC opened */
#define BTA_RC_CLOSE_EVT            4       /* Connection to peer AVRC closed */

/* Event definitons for tBTA_RC_CBACK (CT events) */
#define BTA_RC_UNIT_INFO_RSP_EVT    5       /* Received response for UNIT INFO command */
#define BTA_RC_SUBUNIT_INFO_RSP_EVT 6       /* Received response for SUBUNIT INFO command */
#define BTA_RC_PASS_THRU_RSP_EVT    7       /* Received response for PASSTHROUGH command */
#define BTA_RC_VENDOR_RSP_EVT       8       /* Received response for VENDOR DEPENDENT command */
#define BTA_RC_METADATA_RSP_EVT     9       /* Received response for METADATA command */
#define BTA_RC_CMD_TOUT_EVT         10      /* Timeout waiting for command response */

/* Event definitons for tBTA_RC_CBACK (TG events) */
#define BTA_RC_UNIT_INFO_CMD_EVT    11      /* Received UNIT INFO command */
#define BTA_RC_SUBUNIT_INFO_CMD_EVT 12      /* Received SUBUNIT INFO command */
#define BTA_RC_PASS_THRU_CMD_EVT    13      /* Received PASSTHROUGH command */
#define BTA_RC_VENDOR_CMD_EVT       14      /* Received VENDOR DEPENDENT command */
#define BTA_RC_METADATA_CMD_EVT     15      /* Received PASS THROUGH command */

#define BTA_RC_PEER_FEAT_EVT        16      /* Peer features notification (sdp results for peer initiated AVRC connection) */

typedef UINT8 tBTA_RC_EVT;

/* Info from peer's AVRC SDP record (included in BTA_RC_OPEN_EVT) */
typedef struct
{
    UINT16 version;         /* AVRCP version */
    UINT16 features;        /* Supported features (see AVRC_SUPF_* definitions in avrc_api.h) */
} tBTA_RC_PEER_INFO;


typedef struct
{
    tBTA_RC_FEAT        roles;  /* BTA_RC_FEAT_CONTROL or BTA_RC_FEAT_TARGET */
    tBTA_RC_PEER_INFO   tg;     /* Peer TG role info */
    tBTA_RC_PEER_INFO   ct;     /* Peer CT role info */
} tBTA_RC_PEER_FEAT;

/* Data for BTA_RC_OPEN_EVT */
typedef struct
{
    tBTA_STATUS         status;
    BD_ADDR             bd_addr;
    tBTA_RC_PEER_FEAT   peer_feat;
} tBTA_RC_OPEN;

/* Data for BTA_RC_UNIT_INFO_CMD_EVT */
typedef struct
{
    UINT8   label;
} tBTA_RC_UNIT_INFO_CMD;

/* Data for BTA_RC_UNIT_INFO_RSP_EVT */
typedef struct
{
    UINT8   label;
    UINT8   unit_type;
    UINT8   unit;
    UINT32  company_id;
} tBTA_RC_UNIT_INFO_RSP;

/* Data for BTA_RC_SUBUNIT_INFO_CMD_EVT */
typedef struct
{
    UINT8   label;
    UINT8   page;
} tBTA_RC_SUBUNIT_INFO_CMD;

/* Data for BTA_RC_SUBUNIT_INFO_RSP_EVT */
typedef struct
{
    UINT8   label;
    UINT8   page;
    UINT8   subunit_type[AVRC_SUB_TYPE_LEN];
    BOOLEAN panel;      /* TRUE if the panel subunit type is in the  subunit_type array, FALSE otherwise. */
} tBTA_RC_SUBUNIT_INFO_RSP;

/* Data for BTA_RC_PASS_THRU_CMD_EVT */
typedef struct
{
    UINT8           label;
    UINT8           op_id;          /* Operation ID (see AVRC_ID_* defintions in avrc_defs.h) */
    UINT8           key_state;      /* AVRC_STATE_PRESS or AVRC_STATE_RELEASE */
    tAVRC_HDR       hdr;            /* Message header. */
    UINT8           len;
    UINT8           *p_data;
} tBTA_RC_PASS_THRU_CMD;

/* Data for BTA_RC_PASS_THRU_RSP_EVT */
typedef struct
{
    UINT8           label;
    UINT8           op_id;          /* Operation ID (see AVRC_ID_* defintions in avrc_defs.h) */
    UINT8           key_state;      /* AVRC_STATE_PRESS or AVRC_STATE_RELEASE */
    UINT8           rsp_code;       /* AVRC response code (see AVRC_RSP_* definitions in avrc_defs.h) */
    UINT8           len;
    UINT8           *p_data;
} tBTA_RC_PASS_THRU_RSP;

/* Data for BTA_RC_CMD_TOUT_EVT */
typedef struct
{
    UINT8           label;          /* label of command that timed out */
} tBTA_RC_CMD_TOUT;

/* Data for BTA_RC_VENDOR_CMD_EVT / BTA_RC_VENDOR_RSP_EVT */
typedef struct
{
    UINT8           label;
    UINT32          company_id;
    tAVRC_HDR       hdr;            /* AVRC header (AVRC cmd/rsp type, subunit_type, subunit_id - see avrc_defs.h)  */
    UINT16          len;            /* Max vendor dependent message is 512 */
    UINT8           *p_data;
} tBTA_RC_VENDOR;

/* Data for BTA_RC_METADATA_CMD_EVT / BTA_RC_METADATA_RSP_EVT / */
typedef struct
{
    UINT8           label;
    UINT8           code;           /* AVRC command or response code (see AVRC_CMD_* or AVRC_RSP_* definitions in avrc_defs.h) */
    tAVRC_MSG       *p_msg;
    UINT16          len;
    UINT8           *p_data;
} tBTA_RC_METADATA;


typedef union
{
    tBTA_STATUS             status;             /* BTA_RC_ENABLE_EVT, BTA_RC_DISABLE_EVT, BTA_RC_CLOSE_EVT */
    tBTA_RC_OPEN            open;               /* BTA_RC_OPEN_EVT */
    tBTA_RC_UNIT_INFO_CMD   unit_info_cmd;      /* BTA_RC_UNIT_INFO_CMD_EVT */
    tBTA_RC_UNIT_INFO_RSP   unit_info_rsp;      /* BTA_RC_UNIT_INFO_RSP_EVT */
    tBTA_RC_SUBUNIT_INFO_CMD subunit_info_cmd;  /* BTA_RC_SUBUNIT_INFO_CMD_EVT */
    tBTA_RC_SUBUNIT_INFO_RSP subunit_info_rsp;  /* BTA_RC_SUBUNIT_INFO_RSP_EVT */
    tBTA_RC_PASS_THRU_CMD   pass_thru_cmd;      /* BTA_RC_PASS_THRU_CMD_EVT */
    tBTA_RC_PASS_THRU_RSP   pass_thru_rsp;      /* BTA_RC_PASS_THRU_RSP_EVT */
    tBTA_RC_VENDOR          vendor_msg;         /* BTA_RC_VENDOR_CMD_EVT, BTA_RC_VENDOR_RSP_EVT */
    tBTA_RC_METADATA        meta_msg;           /* BTA_RC_METADATA_CMD_EVT, BTA_RC_METADATA_RSP_EVT */
    tBTA_RC_CMD_TOUT        cmd_tout;           /* BTA_RC_CMD_TOUT_EVT */
    tBTA_RC_PEER_FEAT       peer_feat;          /* BTA_RC_PEER_FEAT_EVT */
} tBTA_RC;

/* BTA_RC callback */
typedef void (tBTA_RC_CBACK)(tBTA_RC_EVT event, UINT8 handle, tBTA_RC *p_data);

/*****************************************************************************
**  Configuration structures for CT and TG
*****************************************************************************/
typedef struct
{

    UINT32  company_id;         /* Used for UNIT_INFO response */
    UINT16  mtu;

    /* SDP configuration for controller */
    UINT8   *ct_service_name;
    UINT8   *ct_provider_name;
    UINT16  ct_categories;

    /* SDP configuration for target */
    UINT8   *tg_service_name;   
    UINT8   *tg_provider_name;
    UINT16  tg_categories;
} tBTA_RC_CFG;

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/

	
/*****************************************************************************
**  Common Controller/Target role APIs
*****************************************************************************/

/*******************************************************************************
**
** Function         BTA_RcEnable
**
** Description      Enable the AVRC module.
**
**                  Initialize the BTA_RC module, registers callback for BTA_RC
**                  event notifications. Add AVRC service to SDP database.
**
**                  BTA_RC_ENABLE_EVT will report the status of the operation.
**
**                  NOTE:
**                  BTA_RC is intended for use platforms that do not include
**                  AV source (BTA_AV) nor AV sink (BTA_AVK).
**
**                  For platforms that include the BTA_AV or BTA_AVK module,
**                  the RC APIs from those respective modules should be
**                  used instead.
**                
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcEnable(tBTA_SEC sec_mask, tBTA_RC_FEAT features,
                          tBTA_RC_CBACK *p_cback);

/*******************************************************************************
**
** Function         BTA_RcDisable
**
** Description      Disable the stand-alone AVRC service.
**
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcDisable(void);

/*******************************************************************************
**
** Function         BTA_RcOpen
**
** Description      Open an AVRCP connection as initiator 
**
**                  Role may be controller (BTA_RC_FEAT_CONTROL) or
**                  target (BTA_RC_FEAT_TARGET).
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcOpen(BD_ADDR peer_addr, tBTA_RC_FEAT role);

/*******************************************************************************
**
** Function         BTA_RcClose
**
** Description      Close an AVRCP connection
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcClose(UINT8 handle);


/*****************************************************************************
**  Controller role APIs
*****************************************************************************/

/*******************************************************************************
**
** Function         BTA_RcUnitInfoCmd
**
** Description      Send a UNIT INFO command.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcUnitInfoCmd(UINT8 handle, UINT8 label);

/*******************************************************************************
**
** Function         BTA_RcSubunitInfoCmd
**
** Description      Send a SUBINIT INFO command.
**
**                  NOTE: extension_code is for future use, per AV/C specifications.
**                        BTA_RC_SUBUNIT_DEFAULT_EXTENSION_CODE may be used.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcSubunitInfoCmd(UINT8 handle, UINT8 label, UINT8 page, UINT8 extension_code);

/*******************************************************************************
**
** Function         BTA_RcVendorCmd
**
** Description      Send a vendor dependent remote control command.
**
**                  See AVRC_CMD_* in avrc_defs.h for available commands.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcVendorCmd(UINT8 handle, UINT8 label,  UINT8 avrc_cmd,
                                UINT32 company_id, UINT8 subunit_type, UINT8 subunit_id,
                                UINT8 *p_data, UINT16 len);

/*******************************************************************************
**
** Function         BTA_RcPassThroughCmd
**
** Description      Send a PASS_THROUGH command
**
**                  See AVRC_ID_* in avrc_defs.h for available opcode IDs
**                      AVRC_STATE_* for key states definitions
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcPassThroughCmd(UINT8 handle, UINT8 label, UINT8 op_id,
                            UINT8 key_state, UINT8 *p_data, UINT16 len);

/*******************************************************************************
**
** Function         BTA_RcMetaCmd
**
** Description      Send a Metadata/Advanced Control command. 
**
**                  See AVRC_CMD_* in avrc_defs.h for available commands.
**
**                  The message contained in p_pkt can be composed with AVRC 
**                  utility functions.
**
**                  This function can only be used if RC is enabled with feature
**                  BTA_RC_FEAT_METADATA.
**
**                  This message is sent only when the peer supports the TG role.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcMetaCmd(UINT8 handle, UINT8 label, UINT8 avrc_cmd, BT_HDR *p_pkt);


/*****************************************************************************
**  Target role APIs
*****************************************************************************/

/*******************************************************************************
**
** Function         BTA_RcUnitInfoRsp
**
** Description      Send a UNIT INFO response.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcUnitInfoRsp(UINT8 handle, UINT8 label, UINT8 unit_type, UINT8 unit);

/*******************************************************************************
**
** Function         BTA_RcSubunitInfoRsp
**
** Description      Send a SUBINIT INFO response.
**
**                  NOTE: extension_code is for future use, per AV/C specifications.
**                        BTA_RC_SUBUNIT_DEFAULT_EXTENSION_CODE may be used.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcSubunitInfoRsp(UINT8 handle, UINT8 label, UINT8 page, UINT8 extension_code,
                    UINT8 subunit_type[AVRC_SUB_TYPE_LEN]);

/*******************************************************************************
**
** Function         BTA_RcVendorRsp
**
** Description      Send a vendor dependent remote control response.
**
**                  See AVRC_RSP_* in avrc_defs.h for response code definitions
**
**                  This function must be called if a BTA_RC_VENDOR_CMD_EVT
**                  is received. This function can only be used if RC is
**                  enabled with feature BTA_RC_FEAT_VENDOR.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcVendorRsp(UINT8 handle, UINT8 label, UINT8 avrc_rsp,
                                UINT32 company_id, UINT8 subunit_type, UINT8 subunit_id,
                                UINT8 *p_data, UINT16 len);


/*******************************************************************************
**
** Function         BTA_RcPassThroughRsp
**
** Description      Send a PASS_THROUGH response
**
**                  See AVRC_ID_* in avrc_defs.h for available opcode IDs
**                      AVRC_RSP_* for response code definitions
**                      AVRC_STATE_* for key states definitions
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcPassThroughRsp(UINT8 handle, UINT8 label, UINT8 op_id,
                             UINT8 avrc_rsp, UINT8 key_state, UINT8 *p_data, UINT16 len);

/*******************************************************************************
**
** Function         BTA_RcMetaRsp
**
** Description      Send a Metadata response. 
**
**                  See AVRC_RSP_* in avrc_defs.h for response code definitions
**
**                  The message contained in p_pkt can be composed with AVRC
**                  utility functions.
**
**                  This function can only be used if RC is enabled with feature
**                  BTA_RC_FEAT_METADATA.
**
** Returns          void
**
*******************************************************************************/
BTA_API void BTA_RcMetaRsp(UINT8 handle, UINT8 label, UINT8 avrc_rsp,
                               BT_HDR *p_pkt);

#ifdef __cplusplus
}
#endif

#endif /* BTA_AV_API_H */
