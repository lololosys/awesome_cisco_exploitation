/* $Id: dlsw_cap_exchg.h,v 3.3.6.1 1996/06/11 15:00:50 ravip Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_cap_exchg.h,v $
 *------------------------------------------------------------------
 * DLSw Capabilities Exchange Header File
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contains declarations for the DLSw capabilities exchange. This 
 * includes typedefs, constants, data structures and function 
 * prototypes.
 *------------------------------------------------------------------
 * $Log: dlsw_cap_exchg.h,v $
 * Revision 3.3.6.1  1996/06/11  15:00:50  ravip
 * CSCdi58441:  xid3 flooding in csna/dlsw environment cause access
 *              router to reboot
 *                  - Carry xid command response bit in the message
 *                  - Use Capex to exchange information about this
 *                    support
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/06  23:43:32  fbordona
 * CSCdi50868:  Dlsw Interop: Support vendor context cap exchg control
 *              vector.
 *
 * Revision 3.2  1995/11/17  09:02:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:22:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  18:48:40  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *              Clean up DLSw header files
 *
 * Revision 2.1  1995/06/07  20:25:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _DLSW_CAP_EXCHG_H_
#define _DLSW_CAP_EXCHG_H_

/*
 * DLSw Standard Control Vector IDs
 */
#define CAP_VENDOR_ID        0x81
#define CAP_VERS_NUM         0x82
#define CAP_IPW              0x83
#define CAP_VERS_STRING      0x84
#define CAP_MAC_EXCL         0x85
#define CAP_SAP_LIST         0x86
#define CAP_NUM_TCP          0x87
#define CAP_NETBIOS_EXCL     0x88
#define CAP_MAC_ADDR         0x89
#define CAP_NB_NAME          0x8A
#define CAP_VENDOR_CNTXT     0x8B

/*
 * cisco Control Vector IDs
 * (valid range of CVs is X'D0' through X'FD') 
 */
#define CAP_LACK             0xD0
#define CAP_PRIORITY         0xD1
#define CAP_PEER_GRP         0xD2
#define CAP_BORDER           0xD3
#define CAP_COST             0xD4
#define CAP_CISCO_VERS       0xD5
#define CAP_SPOOF_AND_FRAG   0xD6
#define CAP_XID_OP_SUPPORT   0xD7
#define CAP_PEER_TYPE        0xD8

/*
 * Negative Response Reasons
 */
typedef enum _rsp_reason_t {
    INVALID_GDS_LEN   =  0x0001,
    INVALID_GDS_ID    =  0x0002,
    NO_VENDOR_ID      =  0x0003,
    NO_VERS_NUM       =  0x0004,
    NO_IPW            =  0x0005,
    INVALID_MSG_LEN   =  0x0006,
    INVALID_CV_ID     =  0x0007,
    INVALID_CV_LEN    =  0x0008,
    INVALID_CV_DATA   =  0x0009,
    INVALID_DUP_CV    =  0x000A,
    OUT_OF_SEQ_CV     =  0x000B,
    NO_SAP_LIST       =  0x000C,
    POD_FAIL          =  0x000D
} rsp_reason_t;

/*
 * Capabilities Control Vector constants
 */
#define AIW_CP_VERS             0x01
#define AIW_CP_REL              0x00 
#define CISCO_VERSION_1            1 

#define CISCO_NUM_TCP_PIPES        1
#define DLSW_DEF_NUM_TCP_PIPES     2
#define NOT_EXCL_RESOURCES      0x00
#define EXCL_RESOURCES          0x01

/*
 * Capabilities GDS variable headers
 */

typedef struct _cap_gds_t {
    ushort gds_len;
    ushort gds_id;
} cap_gds_t;

#define cap_pos_rsp_t cap_gds_t

typedef struct _cap_neg_rsp_t {
    cap_gds_t gds_hdr; 
    ushort offset;
    ushort reason;
} cap_neg_rsp_t; 

/*
 * Control Vector Header
 */

typedef struct _cv_hdr_t {
    uchar cv_len;
    uchar cv_id ;
} cv_hdr_t;

/*
 * DLSw Standard Control vectors
 */

typedef struct _cv_vendor_id_t {
    cv_hdr_t cv_hdr;
    uchar vendor_id[SNAP_OUIBYTES];
} cv_vendor_id_t;

typedef struct _cv_vers_num_t {
    cv_hdr_t cv_hdr;
    uchar  vers_num;
    uchar  rel_num;
} cv_vers_num_t;

typedef struct _cv_ipw_t {
    cv_hdr_t cv_hdr;
    ushort window;
} cv_ipw_t;

typedef struct _cv_sap_t {
    cv_hdr_t cv_hdr;
    uchar sap_vector[SAP_CV_BYTES];
} cv_sap_t;

typedef struct _cv_vstr_t {
    cv_hdr_t cv_hdr;
    char vers_string[1];
} cv_vstr_t;

typedef struct _cv_tcp_t {
    cv_hdr_t cv_hdr;
    uchar num_tcp_sup;
} cv_tcp_t;

typedef struct _cv_excl_t {
    cv_hdr_t cv_hdr;
    uchar excl;
} cv_excl_t;

typedef struct _cv_mac_t {
    cv_hdr_t cv_hdr;
    uchar macaddr[6];
    uchar macmask[6];
} cv_mac_t;

typedef struct _cv_nbname_t {
    cv_hdr_t cv_hdr;
    uchar nbname[1];
} cv_nbname_t;

typedef struct _cv_vendor_ctxt_t {
    cv_hdr_t cv_hdr;
    uchar vendor_id[SNAP_OUIBYTES];
} cv_vctxt_t;

/*
 * cisco to cisco Control vectors
 */

typedef struct _cv_cisco_vers_t {
    cv_hdr_t cv_hdr;
    ushort cisco_version;
} cv_cisco_vers_t;

typedef struct _cv_lack_t {
    cv_hdr_t cv_hdr;
    uchar lack;
} cv_lack_t;

typedef struct _cv_priority_t {
    cv_hdr_t cv_hdr;
    uchar priority;
} cv_pri_t;

typedef struct _cv_peer_group_t {
    cv_hdr_t cv_hdr;
    ushort group;
} cv_pgrp_t;

typedef struct _cv_border_t {
    cv_hdr_t cv_hdr;
    uchar border;
} cv_border_t;

typedef struct _cv_cost_t {
    cv_hdr_t cv_hdr;
    uchar cost;
} cv_cost_t;

typedef struct _cv_biu_seg_t {
    cv_hdr_t cv_hdr;
    uchar spoof_and_seg;
} cv_biu_seg_t;

typedef struct _cv_xid_op_t {
    cv_hdr_t cv_hdr;
    uchar xid_op_support;
} cv_xid_op_t;

typedef struct _cv_peer_type_t {
    cv_hdr_t cv_hdr;
    uchar peer_type;
} cv_peer_type_t;

/*
 * External Function Prototypes
 */
extern boolean send_capabilities (peerentry *);
extern boolean send_runtime_capabilities (void);
extern void peer_cap_exchg_input (peerentry *peer, paktype *pak);
extern paktype *get_ssp_pak (ulong pak_size);
extern boolean parse_cap_ex_id(paktype *, peerentry *);
extern void parse_runtime_cap_ex_id(paktype *, peerentry *);

#endif
