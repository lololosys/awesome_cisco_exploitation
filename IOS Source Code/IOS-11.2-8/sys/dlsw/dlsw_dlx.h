/* $Id: dlsw_dlx.h,v 3.2 1995/11/17 09:03:21 hampton Exp $
 *------------------------------------------------------------------
 * DLSw DLX Protocol Header File
 *
 * August 1994, Ravi Periasamy 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Cisco's enhancements to the DLSw standard protocol.
 *------------------------------------------------------------------
 * $Log: dlsw_dlx.h,v $
 * Revision 3.2  1995/11/17  09:03:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:24:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:27:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#ifndef __DLSW_DLX_H__
#define __DLSW_DLX_H__

/* dlx frame types */
enum dlx_op_type {
    DLX_CAN_U_REACH       = 0x01,
    DLX_I_CAN_REACH       = 0x02,
    DLX_I_CANNOT_REACH    = 0x03,
    DLX_START_DL          = 0x04,
    DLX_DL_STARTED        = 0x05,
    DLX_START_DL_FAILED   = 0x06,
    DLX_RESTART_DL        = 0x07,
    DLX_DL_RESTARTED      = 0x08,
    DLX_RESTART_DL_FAILED = 0x09,
    DLX_XIDFRAME          = 0x0A,
    DLX_CONTACT_STN       = 0x0B,
    DLX_STN_CONTACTED     = 0x0C,
    DLX_CL_DATA_FRAME     = 0x0D,
    DLX_INFO_FRAME        = 0x0E,
    DLX_HALT_DL           = 0x0F,
    DLX_DL_HALTED         = 0x10,
    DLX_SNA_DGRM          = 0x11,
    DLX_NB_DGRM           = 0x12,
    DLX_QUERY_DL          = 0x13,
    DLX_QUERY_DL_RESPONSE = 0x14,
    DLX_IPM_FRAME         = 0x15,
    DLX_CAP_XCHANGE       = 0x16,
    DLX_CLOSE_PEER        = 0x17,
    DLX_CLOSE_PEER_ACK    = 0x18,
    DLX_ENTER_PEER_BUSY   = 0x19,
    DLX_EXIT_PEER_BUSY    = 0x1A,
    DLX_XCAST_FRAME       = 0x1B,
    DLX_FRAME_REJ         = 0x1C,
    DLX_PEER_TEST_REQ     = 0x1D,
    DLX_PEER_TEST_RSP     = 0x1E,
    DLX_FAST_PASS         = 0x30
    };

/* misc items */
#define WRENCH_BIT_MASK      0x80
#define DLX_PROTO_VERSION    0x81    /* protocol ID and version no. */
#define DLX_MAX_RIF          (SRB_MAX_MAXRD * 2)

#define DLX_HDR_SZ           sizeof(dlx_header_t)
#define DLX_CUR_SZ           sizeof(dlx_cur_data_t)
#define DLX_ICR_SZ           sizeof(dlx_icr_data_t)
#define DLX_ICNR_SZ          sizeof(dlx_icnr_data_t)
#define DLX_STARTDL_SZ       sizeof(dlx_startdl_data_t)
#define DLX_DL_STARTED_SZ    sizeof(dlx_dlstarted_data_t)
#define DLX_STARTDLFAILED_SZ sizeof(dlx_startdlfailed_data_t)
#define DLX_CMD_FAILED_SZ    sizeof(dlx_cmd_failed_data_t)
#define DLX_DGRM_SZ          sizeof(dlx_dgrm_data_t)

/* dlx data structures */

struct dlx_header_ {
    uchar    proto_ver_id;     /* protocol id and version number */
    uchar    message_type;     /* dlx message type               */
    ushort   packet_length;    /* total size of DLX packet       */
    ulong    session_id;       /* dlx session id: DLC Correlator */
    uchar    ipm_flags;        /* IPM flags                      */
    uchar    return_code;      /* return codes                   */
    uchar    csco_reserved[2]; /* reserved for future use        */
    };
typedef struct dlx_header_ dlx_header_t;

struct dlx_cur_data_ {
    uchar    dst_mac[IEEEBYTES]; /* target mac address      */
    uchar    src_sap;            /* Origin sap              */
    uchar    reserved;               
    };
typedef struct dlx_cur_data_ dlx_cur_data_t;

struct dlx_icr_data_ {
    uchar    dst_mac[IEEEBYTES]; /* mac address                         */
    uchar    src_sap;            /* sap sent in in cur frame            */
    uchar    mtu_size;           /* maximum frame size                  */
    };
typedef struct dlx_icr_data_ dlx_icr_data_t;

struct dlx_icnr_data_ {
    uchar    dst_mac[IEEEBYTES]; /* mac address                             */
    uchar    reserved[2];        /* reason why the station is not reachable */
    };
typedef struct dlx_icnr_data_ dlx_icnr_data_t;

struct dlx_startdl_data_ {
    uchar    src_mac[IEEEBYTES];
    uchar    dst_mac[IEEEBYTES];
    uchar    src_sap;
    uchar    dst_sap;
    uchar    mtu_size;
    uchar    session_type;
    ulong    src_session_id;
    ulong    reserved_1;
    uchar    src_rif[DLX_MAX_RIF];
    uchar    reserved_2[3];
    uchar    sub_command;
    ushort   sub_command_data_length;
    };
typedef struct dlx_startdl_data_ dlx_startdl_data_t;

struct dlx_dlstarted_data_ {
    uchar    src_mac[IEEEBYTES];
    uchar    dst_mac[IEEEBYTES];
    uchar    src_sap;
    uchar    dst_sap;
    uchar    mtu_size;
    uchar    session_type;
    ulong    src_session_id;
    ulong    dst_session_id;
    uchar    src_rif[DLX_MAX_RIF];
    uchar    reserved_2[3];
    uchar    sub_command;
    ushort   sub_command_data_length;
    };
typedef struct dlx_dlstarted_data_ dlx_dlstarted_data_t;

struct dlx_startdlfailed_data_ {
    uchar    src_mac[IEEEBYTES];
    uchar    dst_mac[IEEEBYTES];
    uchar    src_sap;
    uchar    dst_sap;
    uchar    mtu_size;
    uchar    session_type;
    ulong    reserved;
    ulong    dst_session_id;
    uchar    reserved_1[24];
    };
typedef struct dlx_startdlfailed_data_ dlx_startdlfailed_data_t;

struct dlx_dgrm_data_ {
    uchar    src_mac[IEEEBYTES];
    uchar    dst_mac[IEEEBYTES];
    uchar    src_sap;
    uchar    dst_sap;
    uchar    mtu_size;
    uchar    xcast_type;
    };
typedef struct dlx_dgrm_data_ dlx_dgrm_data_t;

struct dlx_closepeer_data_ {
    uchar    reason;
    uchar    data_1;
    uchar    reserved[2];
    };
typedef struct dlx_closepeer_data_ dlx_closepeer_data_t;

struct dlx_fast_hdr_ {
    uchar    proto_ver_id;     /* protocol id and version number */
    uchar    message_type;     /* dlx message type               */
    ushort   packet_length;    /* total size of DLX packet       */
    ulong    session_id;       /* receiver's session id          */
    ulong    src_session_id;   /* sender's dlx session id        */
    };
typedef struct dlx_fast_hdr_ dlx_fast_hdr_t;


/*
 * Border Peer Message Definitions
 */
enum dlx_border_peer_op_t {
    DLX_MEMBER_TO_BP      = 0xA0,
    DLX_BP_TO_MEMBER      = 0xA1,
    DLX_BP_TO_BP          = 0xA2,
    DLX_RELAY_RSP         = 0xA3
};

typedef struct _dlx_bp_hdr_t {
    uchar      proto_ver_id;
    uchar      message_type;
    ushort     packet_length;
    uchar      orig_group;
    uchar      dest_group;
    uchar      flags;
#define BP_FLAG_POD_FST   0x01   /* if not set, pod xport is TCP */

    uchar      reserved;
    ipaddrtype orig_ipaddr;
    ipaddrtype dest_ipaddr;
} dlx_bp_hdr_t;

/* Macros */
#define is_dlx_csm_op(dlx_ptr)                           \
       (((dlx_ptr)->message_type) == DLX_CAN_U_REACH   ||\
       ((dlx_ptr)->message_type) == DLX_I_CAN_REACH    ||\
       ((dlx_ptr)->message_type) == DLX_I_CANNOT_REACH ||\
       ((dlx_ptr)->message_type) == DLX_START_DL       ||\
       ((dlx_ptr)->message_type) == DLX_SNA_DGRM       ||\
       ((dlx_ptr)->message_type) == DLX_NB_DGRM          )

#define is_dlx_bp_op(type)             \
       ((type == DLX_MEMBER_TO_BP)    ||\
        (type == DLX_BP_TO_MEMBER)    ||\
        (type == DLX_BP_TO_BP)        ||\
        (type == DLX_RELAY_RSP))

/*
 * DLSw FST Constants
 */
#define DLX_FST_PROTO_VERSION    0x81000000
#define DLX_FST_PASS_OP          0x00300000
#define DLX_FST_VERS_AND_OP      0x81300000
#define DLX_FST_SHORT_VERS_AND_OP 0x8130

#endif    /* __DLSW_DLX_H__ */
