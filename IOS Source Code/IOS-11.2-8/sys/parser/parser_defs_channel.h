/* $Id: parser_defs_channel.h,v 3.2.62.5 1996/08/29 01:49:14 mkirk Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_defs_channel.h,v $
 *------------------------------------------------------------------
 * parser_defs_channel.h:  Definitions for channel parser commands.
 *
 * February 1994, Stig Thormodsrud
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: parser_defs_channel.h,v $
 * Revision 3.2.62.5  1996/08/29  01:49:14  mkirk
 * CSCdi62250:  Change maximum-lus to 2100; add licence and threshold
 * messages
 * Branch: California_branch
 *
 * Revision 3.2.62.4  1996/06/20  07:22:17  mkirk
 * CSCdi59893: show ext x/2 tn ... commands had wrong help text, also min
 * and max LOCADDR constants were wrong (0-256 instead of 1-255)
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/05/28  07:44:58  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/05/28  06:22:11  bchan
 * CSCdi58128:  add timing mark configuration and change keepalive
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/05/17  11:39:39  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/03/28  11:29:33  bchan
 * CSCdi52724:  merge tn3280 server code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2  1995/11/17  18:49:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/13  19:34:05  motto
 * CSCdi36478:  Show interface statistical info is missing for Channel
 * interface
 *
 * Revision 2.4  1995/07/18  21:49:09  motto
 * Fixed filtering support for LLC2 show command
 * Update MIBs to current level of CIP support
 * Cleaned up CSNA show command support
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.3  1995/07/13  19:42:45  motto
 * Fixed all channel tcp/ip show commands to be consistent and work
 * properly.
 * CSCdi36252:  show ex ch <slot>/<port> tcp-connections needs parms to be
 * optional
 *
 * Revision 2.2  1995/06/21  20:27:07  motto
 * Fix 'show extended channel x/2 <llc2|csna|connection-map|
 * max-llc2-sessions> ....' commands for SNA support on the CIP card.
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.1  1995/06/07  22:29:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PARSER_DEFS_CHANNEL_H__
#define __PARSER_DEFS_CHANNEL_H__

#define CHANNEL_NODEID   0     /* not really a channel mode, but a dev_type */
#define CHANNEL_CLAW     1
#define CHANNEL_CSNA     2 
#define CHANNEL_LCS      3
#define CHANNEL_OFFLOAD  4
#define CHANNEL_IVORIE   5
#define CHANNEL_DP       6
#define CHANNEL_MAXMODE  7

/* Configuration codes sent to the CIP - other than subchannel types */
#define CHANNEL_CTA_TEST_TOOL     8    /* not a channel mode, but related to CHANNEL_LSA */
#define CHANNEL_CFG_MAX_LLC2      9
#define CHANNEL_CFG_VADAPTER     10 
#define CHANNEL_CFG_VADAPTER_UPD 11   

/* Add new configuration commands here, update CHANNEL_MAX_CFG_CMD and see note below */
#define CHANNEL_MAX_CFG_CMD      12

/* =========================== TN3270 =========================== */
/* TN3270-Server Code points     */
#define CIP_TN3270S_SVR          12
#define CIP_TN3270S_SVR_LU_DNS   13
#define CIP_TN3270S_SVR_MAX_LU   14  
#define CIP_TN3270S_SVR_SIFTDOWN 15 
#define CIP_TN3270S_PU_NEW       16
#define CIP_TN3270S_PU_UPD       17
#define CIP_TN3270S_DLUR_PU_NEW  18
#define CIP_TN3270S_DLUR_PU_UPD  19
#define CIP_TN3270S_DLUR         20
#define CIP_TN3270S_DLUR_DLUS    21
#define CIP_TN3270S_DLUR_NN_SVR  22
#define CIP_TN3270S_DLUR_SAP     23
#define CIP_TN3270S_DLUR_SAP_VRN 24
#define CIP_TN3270S_DLUR_SAP_LNK 25 
#define CIP_TN3270S_SVR_TIMING_MARK 26

#undef  CHANNEL_MAX_CFG_CMD
#define CHANNEL_MAX_CFG_CMD      27
/* =========================== TN3270 =========================== */

/*
 * Please make sure to update the table of string names in if_cip.c when 
 * adding new configuration command
 */

/* Show extended command code points */
#define CHANNEL_STATISTICS    1
#define CHANNEL_SUBCHANNEL    2
#define CHANNEL_VC            3
#define CHANNEL_CTA_TT        4
#define CHANNEL_CSNA_DEVICE   5  
#define CHANNEL_CSNA_VLAN     6
#define CHANNEL_CSNA_LLC2     7
#define CHANNEL_MAX_LLC2_SESS 8
#define CHANNEL_CSNA_CONN_MAP 9
#define CHANNEL_CSNA_VLAN_INT 10

#define CHANNEL_STATS_DEFAULT  1
#define CHANNEL_STATS_PATH     2
#define CHANNEL_STATS_DEVICE   3

#define CSNA_CONN_MAP_LLC2     1
#define CSNA_CONN_MAP_TN3270   2

#define CSNA_SHO_EXT_TYPE_DFLT  0
#define CSNA_SHO_EXT_TYPE_ADMN  1
#define CSNA_SHO_EXT_TYPE_OPER  2
#define CSNA_SHO_EXT_TYPE_STAT  3

#define CSNA_VLAN_ETHER         0
#define CSNA_VLAN_TOKEN         1
#define CSNA_VLAN_FDDI          2
#define CSNA_VLAN_MAXTYPE       3

#define CSNA_LLC2_T1_ACK        1
#define CSNA_LLC2_T1_PF         2
#define CSNA_LLC2_T1_REJ        3
#define CSNA_LLC2_T1_BUSY       4
#define CSNA_LLC2_TI            5
#define CSNA_LLC2_TW            6
#define CSNA_LLC2_RW            7
#define CSNA_LLC2_N2            8
#define CSNA_LLC2_N1            9
#define CSNA_LLC2_T2           10
#define CSNA_LLC2_N3           11
#define CSNA_LLC2_NW           12

#define CSNA_LLC2_KEY_T1_ACK       "t1-time"
#define CSNA_LLC2_KEY_T1_PF        "tpf-time"
#define CSNA_LLC2_KEY_T1_REJ       "trej-time"
#define CSNA_LLC2_KEY_T1_BUSY      "tbusy-time"
#define CSNA_LLC2_KEY_TI           "idle-time"
#define CSNA_LLC2_KEY_TW           "local-window"
#define CSNA_LLC2_KEY_RW           "recv-window"
#define CSNA_LLC2_KEY_N2           "N2"
#define CSNA_LLC2_KEY_N1           "N1"
#define CSNA_LLC2_KEY_T2           "ack-delay-time"
#define CSNA_LLC2_KEY_N3           "ack-max"
#define CSNA_LLC2_KEY_NW           "Nw"

#define CSNA_LLC2_NEWKEY_T1_ACK    "t1-ack"
#define CSNA_LLC2_NEWKEY_T1_PF     "t1-pf"
#define CSNA_LLC2_NEWKEY_T1_REJ    "t1-rej"
#define CSNA_LLC2_NEWKEY_T1_BUSY   "t1-busy"
#define CSNA_LLC2_NEWKEY_TI        "ti"
#define CSNA_LLC2_NEWKEY_TW        "tw"
#define CSNA_LLC2_NEWKEY_RW        "rw"
#define CSNA_LLC2_NEWKEY_N2        "n2"
#define CSNA_LLC2_NEWKEY_N1        "n1"
#define CSNA_LLC2_NEWKEY_T2        "t2"
#define CSNA_LLC2_NEWKEY_N3        "n3"
#define CSNA_LLC2_NEWKEY_NW        "nw"

/* MIN, MAX VALUES */
/*
 * - - - - - - - - - - - - - - - - - - - - - -
 * LLC2 default values
 * - - - - - - - - - - - - - - - - - - - - - -
 */
#define CSNA_LLC2_DEF_T1_ACK    1000 
#define CSNA_LLC2_DEF_T1_PF     1000
#define CSNA_LLC2_DEF_T1_REJ    3200
#define CSNA_LLC2_DEF_T1_BUSY   9600
#define CSNA_LLC2_DEF_TI       60000
#define CSNA_LLC2_DEF_TW           7
#define CSNA_LLC2_DEF_RW           7
#define CSNA_LLC2_DEF_N2           8
#define CSNA_LLC2_DEF_N1        4105
#define CSNA_LLC2_DEF_T2         100
#define CSNA_LLC2_DEF_N3           3
#define CSNA_LLC2_DEF_NW           0

#define CSNA_VLAN_SRB_MAX_BRIDGE_NUM   15
#define CSNA_VLAN_SRB_MAX_RING_NUM   4095
#define CSNA_VLAN_TB_MAX_BRIDGE_GRP   255
#define CSNA_VLAN_MAX_VADAPTER_NUM     31
#define CSNA_VLAN_MAX_LANID_NUM        31

#define CSNA_DEVICE_MIN_MAXPIU       4096
#define CSNA_DEVICE_MAX_MAXPIU      65535
#define CSNA_DEVICE_DEF_MAXPIU      20470
#define CSNA_DEVICE_MIN_TIME_D          0
#define CSNA_DEVICE_MAX_TIME_D        100
#define CSNA_DEVICE_DEF_TIME_D         10
#define CSNA_DEVICE_MIN_LENGTH_D        0
#define CSNA_DEVICE_MAX_LENGTH_D    65535
#define CSNA_DEVICE_DEF_LENGTH_D    20470

#define CSNA_VLAN_MIN_LLC2_SESSIONS     0
#define CSNA_VLAN_MAX_LLC2_SESSIONS  6000
#define CSNA_VLAN_DEF_LLC2_SESSIONS     0

#define CSNA_VLAN_LLC2_MAX_SAP_NUM    256

#define CSNA_DEVICE_KEY_MAXPIU    "maxpiu"
#define CSNA_DEVICE_KEY_TIME_D    "time-delay"
#define CSNA_DEVICE_KEY_LENGTH_D  "length-delay"

#define CSNA_VLAN_KEY_SRB     "source-bridge"
#define CSNA_VLAN_KEY_TB      "bridge-group"

#define LLC2_SHOW_EXT_CHANNEL_FILTER_NONE    0
#define LLC2_SHOW_EXT_CHANNEL_FILTER_LMAC    1
#define LLC2_SHOW_EXT_CHANNEL_FILTER_LSAP    2
#define LLC2_SHOW_EXT_CHANNEL_FILTER_RMAC    3
#define LLC2_SHOW_EXT_CHANNEL_FILTER_RSAP    4

/*
 * CIP  IP, TCP, UDP, and ICMP  show extended constants
 */
#define CHANNEL_SHOW_IP                    1
#define CHANNEL_SHOW_TCP                   2
#define CHANNEL_SHOW_TCP_CONNS             3
#define CHANNEL_SHOW_ICMP                  4
#define CHANNEL_SHOW_UDP                   5
#define CHANNEL_SHOW_UDP_LISTENERS         6

#define CHANNEL_SHOW_IPADDR_MATCH          1
#define CHANNEL_SHOW_IPADDR_ALL            2

/*
 * These defines must stay in this order */
#define CHANNEL_SHOW_TCPCONNS_DEFAULT      1
#define CHANNEL_SHOW_TCPCONNS_LOCAL_ADDR   2
#define CHANNEL_SHOW_TCPCONNS_LOCAL_PORT   3
#define CHANNEL_SHOW_TCPCONNS_FOREIGN_ADDR 4
#define CHANNEL_SHOW_TCPCONNS_FOREIGN_PORT 5

#define CHANNEL_SHOW_TCPCONNS_SUMMARY      1
#define CHANNEL_SHOW_TCPCONNS_DETAIL       2

/* =========================== TN3270 =========================== */

#define TN3270S_MODENAME_SVR       "tn3270s-svr"
#define TN3270S_MODENAME_PU        "tn3270s-pu"
#define TN3270S_MODENAME_DLUR      "tn3270s-dlur"
#define TN3270S_MODENAME_DLUR_PU   "tn3270s-dlur-pu"
#define TN3270S_MODENAME_DLUR_SAP  "tn3270s-dlur-sap"

/* --------------- Tn2370 Server Sub command modes --------------- */
#define TN3270S_MODE_INTERFACE 0
#define TN3270S_MODE_SVR       1
#define TN3270S_MODE_PU        2
#define TN3270S_MODE_DLUR      3
#define TN3270S_MODE_DLUR_PU   4
#define TN3270S_MODE_DLUR_SAP  5

/* TN3270 cfg  Command Codes   passed via EOLS                    */

#define TN3270S_CFG_SVR                1
#define TN3270S_CFG_SHUT               2
#define TN3270S_CFG_SVR_MAX_LU         3
#define TN3270S_CFG_SVR_LU_DNS         4

#define TN3270S_CFG_TCP_PORT           5
#define TN3270S_CFG_IDLE_TIME          6
#define TN3270S_CFG_KEEPALIVE          7
#define TN3270S_CFG_UNBIND_ACT         8
#define TN3270S_CFG_GPOOL              9

#define TN3270S_CFG_PU                10
#define TN3270S_CFG_SVR_TIMING_MARK   11
#define TN3270S_CFG_DLUR_PU           12

#define TN3270S_CFG_PU_DNS            13

#define TN3270S_CFG_DLUR              14
#define TN3270S_CFG_DLUR_DLUS         15
#define TN3270S_CFG_DLUR_NN_SVR       16
#define TN3270S_CFG_DLUR_SAP          17
#define TN3270S_CFG_DLUR_SAP_VRN      18
#define TN3270S_CFG_DLUR_SAP_LINK     19

/* TN3270 cfg  Keyword Strings                                    */
#define TN3270S_KEY_SVR             "tn3270-server"
#define TN3270S_KEY_SHUT            "shutdown"
#define TN3270S_KEY_EXIT            "exit"
#define TN3270S_KEY_SVR_MAX_LU      "maximum-lus"
#define TN3270S_KEY_SVR_LU_DNS      "lu-dns-domain-stem"

#define TN3270S_KEY_TCP_PORT        "tcp-port" 
#define TN3270S_KEY_IDLE_TIME       "idle-time"
#define TN3270S_KEY_KEEPALIVE       "keepalive"
#define TN3270S_KEY_UNBIND_ACT      "unbind-action"
#define TN3270S_KEY_UNBIND_KEEP        "keep"
#define TN3270S_KEY_UNBIND_DISC        "disconnect"
#define TN3270S_KEY_GPOOL           "generic-pool"
#define TN3270S_KEY_GPOOL_PERMIT       "permit"
#define TN3270S_KEY_GPOOL_DENY         "deny"

#define TN3270S_KEY_PU              "pu"
#define TN3270S_KEY_LMAC            "lmac"
#define TN3270S_KEY_LSAP            "lsap"
#define TN3270S_KEY_RMAC            "rmac"
#define TN3270S_KEY_RSAP            "rsap"
#define TN3270S_KEY_PU_LUSEED       "lu-seed"
#define TN3270S_KEY_PU_DNS          "pu-dns-name"

#define TN3270S_KEY_DLUR            "dlur"
#define TN3270S_KEY_DLUR_DLUS       "dlus-backup"
#define TN3270S_KEY_DLUR_NN_SVR     "preferred-NNserver"
#define TN3270S_KEY_DLUR_SAP        "lsap" 
#define TN3270S_KEY_DLUR_SAP_VRN    "vrn" 
#define TN3270S_KEY_DLUR_SAP_LINK   "link"


#define TN3270S_KEY_TOKEN_ADAPTER   "token-adapter"
#define TN3270S_KEY_ETHER_ADAPTER   "ether-adapter"
#define TN3270S_KEY_FDDI_ADAPTER    "fddi-adapter"

/* TN3270 cfg  MIN MAX and DEF Values */
#define TN3270S_SVR_MAX_LU_MIN           0
#define TN3270S_SVR_MAX_LU_MAX       32000
#define TN3270S_SVR_MAX_LU_DEF        2100

#define TN3270S_TCP_PORT_MIN             1
#define TN3270S_TCP_PORT_MAX         65534
#define TN3270S_TCP_PORT_DEF            23

#define TN3270S_IDLE_TIME_MIN            0
#define TN3270S_IDLE_TIME_MAX        65534
#define TN3270S_IDLE_TIME_DEF            0
#define TN3270S_NO_IDLE_TIME_VAL         0   /* Idle-time param val that means "no idle-time limit" */

#define TN3270S_KEEPALIVE_MIN            0
#define TN3270S_KEEPALIVE_MAX        65534
#define TN3270S_KEEPALIVE_DEF         1800
#define TN3270S_NO_KEEPALIVE_VAL         0   /* Keepalive param val that means "send no keepalives" */

#define TN3270S_UNBIND_ACT_KEEP          0
#define TN3270S_UNBIND_ACT_DISC          1
#define TN3270S_UNBIND_ACT_DEF  TN3270S_UNBIND_ACT_DISC

#define TN3270S_GPOOL_PERMIT             0
#define TN3270S_GPOOL_DENY               1
#define TN3270S_GPOOL_DEF       TN3270S_GPOOL_PERMIT

#define TN3270S_LSAP_MIN                 4
#define TN3270S_LSAP_MAX              0xFC
#define TN3270S_LSAP_DEF              0xC0

#define TN3270S_RSAP_MIN                 4
#define TN3270S_RSAP_MAX              0xFC
#define TN3270S_RSAP_DEF              0x04

/* TN3270 cfg  Help Text Strings                                    */
#define TN3270S_HELP_STR_TOKEN_ADAPTER "Uses a csna tokenring adapter"
#define TN3270S_HELP_STR_ETHER_ADAPTER "Uses a csna ethernet adapter"
#define TN3270S_HELP_STR_FDDI_ADAPTER  "Uses a csna fddi adapter"

/* #define TN3270 show Command Codes          */
#define TN3270S_SHOW_SVR                 0  
#define TN3270S_SHOW_PU_THIS             1
#define TN3270S_SHOW_PU_THIS_LU_THIS     2
#define TN3270S_SHOW_DLUR                3
#define TN3270S_SHOW_CLIENT_THIS         4
#define TN3270S_SHOW_DLURLINK_THIS       5
  
/* TN3270 show client specific codes */ 
#define TN3270S_SHOW_CLIENT_THIS_DISCONNECTED 0x01
#define TN3270S_SHOW_CLIENT_THIS_PENDING      0x02
#define TN3270S_SHOW_CLIENT_THIS_INSESSION    0x04
#define TN3270S_SHOW_CLIENT_THIS_ALL          0xFF

/* TN3270 show Keyword Strings (Parse keywords) */
#define TN3270S_SHOW_KEY_SVR           TN3270S_KEY_SVR
#define TN3270S_SHOW_KEY_PU            TN3270S_KEY_PU
#define TN3270S_SHOW_KEY_LU            "lu"
#define TN3270S_SHOW_KEY_DLUR          TN3270S_KEY_DLUR
#define TN3270S_SHOW_KEY_DLURLINK      "dlurlink"
#define TN3270S_SHOW_KEY_CLIENT        "client-ip-address"
#define TN3270S_SHOW_KEY_CLIENT_INSESSION    "in-session"
#define TN3270S_SHOW_KEY_CLIENT_DISCONNECTED "disconnected"
#define TN3270S_SHOW_KEY_CLIENT_PENDING      "pending"
 
/* TN3270 show strings (used in show command output string) */

#define TN3270S_SHOW_STR_SVR_MAX_LU      TN3270S_KEY_SVR_MAX_LU
#define TN3270S_SHOW_STR_SVR_LU_DNS      TN3270S_KEY_SVR_LU_DNS

#define TN3270S_SHOW_STR_TCP_PORT        TN3270S_KEY_TCP_PORT
#define TN3270S_SHOW_STR_IDLE_TIME       TN3270S_KEY_IDLE_TIME
#define TN3270S_SHOW_STR_KEEPALIVE       TN3270S_KEY_KEEPALIVE
#define TN3270S_SHOW_STR_UNBIND_ACT      TN3270S_KEY_UNBIND_ACT
#define TN3270S_SHOW_STR_UNBIND_KEEP     TN3270S_KEY_UNBIND_KEEP
#define TN3270S_SHOW_STR_UNBIND_DISC     TN3270S_KEY_UNBIND_DISC
#define TN3270S_SHOW_STR_GPOOL           TN3270S_KEY_GPOOL
#define TN3270S_SHOW_STR_GPOOL_PERMIT    TN3270S_KEY_GPOOL_PERMIT
#define TN3270S_SHOW_STR_GPOOL_DENY      TN3270S_KEY_GPOOL_DENY

#define TN3270S_SHOW_STR_PU              TN3270S_KEY_PU
#define TN3270S_SHOW_STR_LMAC            TN3270S_KEY_LMAC
#define TN3270S_SHOW_STR_LSAP            TN3270S_KEY_LSAP
#define TN3270S_SHOW_STR_RMAC            TN3270S_KEY_RMAC
#define TN3270S_SHOW_STR_RSAP            TN3270S_KEY_RSAP

#define TN3270S_SHOW_STR_DLUR            TN3270S_KEY_DLUR
#define TN3270S_SHOW_STR_DLUR_SAP        TN3270S_KEY_DLUR_SAP 
#define TN3270S_SHOW_STR_DLUR_SAP_VRN    TN3270S_KEY_DLUR_SAP_VRN 
#define TN3270S_SHOW_STR_DLUR_SAP_LINK   TN3270S_KEY_DLUR_SAP_LINK

#define TN3270S_SHOW_STR_STATUS          "status"
#define TN3270S_SHOW_STR_NOT_CFGD        "<not configured>"
#define TN3270S_SHOW_STR_STEM            "stem"
#define TN3270S_SHOW_STR_REVERSE         "reverse"
#define TN3270S_SHOW_STR_LOOPBACK        "(loopback)"

/* show ext c 2/2 vc */
#define TN3270S_SHOW_STR_VC_VCNUM        "VC"
#define TN3270S_SHOW_STR_VC_IPADDRESS    "IP address"
#define TN3270S_SHOW_STR_VC_CFGD         "Cfgd"
#define TN3270S_SHOW_STR_VC_ACTV         "Route"
#define TN3270S_SHOW_STR_DASHES          "----------"

#define TN3270S_SHOW_STR_PU_ENTRIES      "PU entries"
#define TN3270S_SHOW_STR_PU_DIRECT       "Direct"
#define TN3270S_SHOW_STR_PU_DLUR         "DLUR"
#define TN3270S_SHOW_STR_PU_XID          "xid"

#define TN3270S_SHOW_STR_PU_LUSEED       TN3270S_KEY_PU_LUSEED
#define TN3270S_SHOW_STR_PU_DNS          TN3270S_KEY_PU_DNS
#define TN3270S_SHOW_STR_PU_LINK         TN3270S_SHOW_STR_DLUR_SAP_LINK
#define TN3270S_SHOW_STR_PU_LSAP         TN3270S_SHOW_STR_LSAP
#define TN3270S_SHOW_STR_PU_RMAC         TN3270S_SHOW_STR_RMAC
#define TN3270S_SHOW_STR_PU_RSAP         TN3270S_SHOW_STR_RSAP

#define TN3270S_SHOW_STR_PU_LINK_VIA     "link via DLUR"
#define TN3270S_SHOW_STR_PU_NAME_AT_HOST "PU name at host:"

/* show ext c 2/2 tn dlur */
#define TN3270S_SHOW_STR_CURRENT         "current"
#define TN3270S_SHOW_STR_PREFERRED       "preferred"
#define TN3270S_SHOW_STR_BACKUP          "backup"

#define TN3270S_SHOW_STR_SERVER          "server"
#define TN3270S_SHOW_STR_DLUS            "dlus"
#define TN3270S_SHOW_STR_DLUR_DLUS       "dlur-dlus"
#define TN3270S_SHOW_STR_CP_CP           "cp-cp"

#define TN3270S_SHOW_STR_REMOTE          "remote"

#define TN3270S_SHOW_STR_PARTNER         "partner"
#define TN3270S_SHOW_STR_TGN             "tgn"
#define TN3270S_SHOW_STR_SESSIONS        "sessions"
#define TN3270S_SHOW_STR_MAXDATA         "maxdata"
#define TN3270S_SHOW_STR_MAXIN           "maxin"
#define TN3270S_SHOW_STR_MAXOUT          "maxout"

/* show ext c 2/2 tn pu <pu-name> lu <lu-number> */
#define TN3270S_SHOW_STR_LU              TN3270S_SHOW_KEY_LU
#define TN3270S_SHOW_STR_CLIENT_AT       "client at"
#define TN3270S_SHOW_STR_MODEL           "model"
#define TN3270S_SHOW_STR_ASSOC           "assoc"
#define TN3270S_SHOW_STR_LAST_USED       "last-used"

/* TN3270 show Flags and Values       */
#define TN3270S_LU_NUM_MIN               1
#define TN3270S_LU_NUM_MAX             255
/* =========================== TN3270 =========================== */

#endif	/* __PARSER_DEFS_CHANNEL_H__ */
