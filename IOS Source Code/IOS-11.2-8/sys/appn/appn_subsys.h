/* $Id: appn_subsys.h,v 3.5.10.4 1996/07/08 15:46:48 dbilling Exp $
 * $Source: /release/112/cvs/Xsys/appn/appn_subsys.h,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * May 1995, Steve Berl
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appn_subsys.h,v $
 * Revision 3.5.10.4  1996/07/08  15:46:48  dbilling
 * Respond to xid commands with xid responses.  Respond to a null
 * xid command with a null xid response.  Remove null-xid-poll
 * configuration as it is no longer necessary (CSCdi61157)
 * Branch: California_branch
 *
 * Revision 3.5.10.3  1996/06/11  14:00:39  mregan
 * CSCdi53126:  APPN needs to support VDLC
 * Branch: California_branch
 * added code review comments.
 *
 * Revision 3.5.10.2  1996/05/23  14:48:45  mregan
 * CSCdi45410:  Limited resource link auto-activation fails
 * Branch: California_branch
 * Fix support for auto-activate links.
 *
 * Revision 3.5.10.1  1996/05/17  10:39:04  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.2  1996/03/30  05:50:58  mregan
 * CSCdi53126:  APPN needs to support VDLC
 * Branch: IbuMod_Calif_branch
 * add VDLC support to APPN
 *
 * Revision 3.5.2.1  1996/03/30  01:35:36  pleung
 * Branch: IbuMod_Calif_branch
 * Central Resource Registration (CRR)
 *
 * Revision 3.5  1996/02/27  10:51:07  dcalia
 * APPN/RSRB needs a configurable bridge number (CSCdi42257)
 *
 * Revision 3.4  1996/02/22  14:46:09  mregan
 * CSCdi48499:  sho appn int filter does not work for unqualified LU names
 * corrected logic for filtering on names
 *
 * Revision 3.3  1996/02/18  18:28:22  pleung
 * CSCdi49290:  dynamic link station support for PU2.0 broken
 *
 * Revision 3.2  1995/11/17  08:37:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:52:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/11/08  20:48:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.12  1995/08/11  02:22:34  dmccowan
 * CSCdi30561:  need to support APING on our router
 *
 * Revision 2.11  1995/08/04  01:36:02  pleung
 * CSCdi37371:  APPN link-queuing custom only up to queue 10
 *
 * Revision 2.10  1995/08/02  00:36:33  vtseng
 * CSCdi29693:  no appn activate should not stop appn
 *
 * Revision 2.9  1995/07/31  19:46:49  dmccowan
 * CSCdi36614:  add memory options to appn
 * add minimum-memory control-point configuration command
 *
 * Revision 2.8  1995/07/27  22:28:44  dmccowan
 * CSCdi36612:  minor problems with appn commands
 *
 * Revision 2.7  1995/07/26  01:56:45  mregan
 * CSCdi28879:  appn show commands have minor problems
 * update 'sh appn * brief' commands
 *
 * Revision 2.6  1995/07/26  01:41:00  vtseng
 * CSCdi31824:  appn link station config command needs more checking.
 * Also piggyback fixes of CSCdi31836 that SAP should be divisible by 4
 * and partial fix of CSCdi29693 at the parser level.
 *
 * Revision 2.5  1995/07/19  22:32:38  vtseng
 * CSCdi31836:  appn port config command needs more checking
 *
 * Revision 2.4  1995/06/27  18:28:34  dmccowan
 * CSCdi36218:  sdlc-dest_address does not follow convention
 * use sdlc-dest-address and x25-dest-address
 *
 * Revision 2.3  1995/06/20  06:00:01  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * Incorporate code review comments.
 *
 * Revision 2.2  1995/06/09  19:25:54  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * Allow appn to specify the priority parameter for each link station.
 *
 * Revision 2.1  1995/06/07  20:03:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef APPN_SUBSYS_H_INCLUDED
#define APPN_SUBSYS_H_INCLUDED

/* enumerations */

typedef enum {DEL_ACTION_MODIFY, DEL_ACTION_DELETE} del_action_e;

/* Definitions for the appn start commands */

#define EXEC_SYSTEM      1
#define EXEC_LINK        2
#define EXEC_APPN_PORT   3
#define EXEC_CPNAME      4
#define EXEC_NETID       5
#define EXEC_APING       6

/* Definitions for the show appn commands */

#define APPN_SHOW_CN       0  
#define APPN_SHOW_COS	   1  
#define APPN_SHOW_DEF	   2  
#define APPN_SHOW_DIR	   3  
#define APPN_SHOW_ISR	   4  
#define APPN_SHOW_LINK_DEF 5  
#define APPN_SHOW_LINK	   6  
#define APPN_SHOW_LU62	   7  
#define APPN_SHOW_MODE	   8  
#define APPN_SHOW_NODE	   9  
#define APPN_SHOW_SESS	   10 
#define APPN_SHOW_SNA	   11 
#define APPN_SHOW_TRS      12
#define APPN_SHOW_PORT     13
#define APPN_SHOW_DLUS     14
#define APPN_SHOW_DLUR_PU  15
#define APPN_SHOW_DLUR_LU  16
#define APPN_SHOW_STATS    17
#define APPN_SHOW_MAX      17

/* Definitions for the show appn commands */

#define DUMP_APPN_DS     1
#define DUMP_APPN_ISR    2
#define DUMP_APPN_LINKS  3
#define DUMP_APPN_MEMORY 4
#define DUMP_APPN_NODE   5
#define DUMP_APPN_PROC   6
#define DUMP_APPN_SESS   7
#define DUMP_APPN_TRS    8
#define DUMP_APPN_DEBUG  9
#define DUMP_APPN_TREE  10
#define DUMP_APPN_BUFFER 11

/* Definitions for the [no] debug appn commands */

#define DEBUG_ALL           0xffffffff   
#define DEBUG_RM            0x800000ff
#define DEBUG_TRS           0x400000ff
#define DEBUG_PS            0x200000ff
#define DEBUG_SS            0x100000ff
#define DEBUG_DS            0x080000ff
#define DEBUG_PC            0x040000ff
#define DEBUG_SCM           0x020000ff
#define DEBUG_NOF           0x010000ff
#define DEBUG_CS            0x008000ff
#define DEBUG_SM            0x004000ff
#define DEBUG_HS            0x002000ff
#define DEBUG_DLC           0x001000ff
#define DEBUG_MS            0x000800ff
#define DEBUG_EGPE_TIMER    0x000400ff
#define DEBUG_EGPE_NONTIMER 0x000200ff
#define DEBUG_SSA           0x000100ff
#define DEBUG_LSR           0x000080ff
#define DEBUG_API           0x000040ff        
#define DEBUG_LU            0x000020ff
#define DEBUG_MODULE_TRACE  0x000010ff
#define DEBUG_ACS           0x000008ff
#define DEBUG_PSNAV         0x000004ff
#define DEBUG_DLUR          0x000002ff


	                      

/* APPN Names and Types */
#define APPN_NAME_CP        "control-point"
#define APPN_NAME_LINK      "link-station"
#define APPN_NAME_PORT      "port"
#define APPN_NAME_CN        "connection-network"
#define APPN_NAME_PLOC      "partner-lu-location"
#define APPN_NAME_COS       "class-of-service"
#define APPN_NAME_MODE      "mode"
#define APPN_NAME_START     "routing"

/* appn command mode prompts */
#define PROMPT_APPN_CP   "config-appn-cp"
#define PROMPT_APPN_LINK "config-appn-link"
#define PROMPT_APPN_PORT "config-appn-port"
#define PROMPT_APPN_CN   "config-appn-cn"
#define PROMPT_APPN_PLOC "config-appn-plu-loc"
#define PROMPT_APPN_COS  "config-appn-cos"
#define PROMPT_APPN_MODE "config-appn-mode"

/*
#define APPN_TYPE_CP         1
#define APPN_TYPE_LINK       2
#define APPN_TYPE_PORT       3
#define APPN_TYPE_CN         4
#define APPN_TYPE_PLOC       5
#define APPN_TYPE_COS        6
#define APPN_TYPE_MODE       7

#define APPN_FQ_NAME         1
#define APPN_CB_NAME         2
*/

#define APPN_NAME            1
#define APPN_COMPLETE        99
 
#define APPN_CP_XID          11
#define APPN_CP_BLOCK        12
#define APPN_CP_RESIST       13
#define APPN_CP_CACHE        14
#define APPN_CP_STORE        15
#define APPN_CP_TREES        16
#define APPN_CP_INTR_SW      17
#define APPN_CP_BUF_PERCENT  18
#define APPN_CP_MEM          19
#define APPN_TRACE_FLAGS     20
#define APPN_CP_CYCLES       21
#define APPN_CP_HOST         22
#define APPN_CP_DLUR	     23
#define APPN_CP_DLUS	     24
#define APPN_CP_BKUP_DLUS    25
#define APPN_CP_MIN          26
#define APPN_CP_CRR          27
#define APPN_CP_DLUS_RETRY_INT 28
#define APPN_CP_PREFER_ACT   29

#define APPN_CP_COMP         APPN_COMPLETE

#define APPN_CP_XID_KEY      "xid-id-number" 
#define APPN_CP_BLOCK_KEY    "xid-block-number"
#define APPN_CP_RESIST_KEY   "route-additional-resistance"
#define APPN_CP_CACHE_KEY    "max-cached-entries"
#define APPN_CP_STORE_KEY    "safe-store-interval"
#define APPN_CP_TREES_KEY    "max-cached-trees"          
#define APPN_CP_INTR_SW_KEY  "interrupt-switched"
#define APPN_CP_BUF_PERCENT_KEY  "buffer-percent"
#define APPN_CP_MEM_KEY          "maximum-memory"
#define APPN_CP_MIN_KEY          "minimum-memory"
#define APPN_TRACE_FLAGS_KEY     "trace-setting"
#define APPN_CP_CYCLES_KEY   "safe-store-cycles"
#define APPN_CP_HOST_KEY     "safe-store-host"
#define APPN_CP_HOST_IP_KEY  "ip-address"
#define APPN_CP_HOST_DIR_KEY "directory"
#define APPN_CP_DLUR_KEY	"dlur"
#define APPN_CP_DLUS_KEY	"dlus"
#define APPN_CP_BKUP_DLUS_KEY	"backup-dlus"
#define APPN_DLUR_MAXPUS_KEY    "max-pus"
#define APPN_CP_DLUS_RETRY_INT_KEY  "retry-dlus-interval"
#define APPN_CP_PREFER_ACT_KEY  "prefer-active-dlus"

#define APPN_CP_XID_KEY_HELP      "ID number for XID exchange."
#define APPN_CP_BLOCK_KEY_HELP    "Block ID for XID exchange."
#define APPN_CP_RESIST_KEY_HELP   "Node's resistance in route calculation."
#define APPN_CP_CACHE_KEY_HELP    "Maximum number of cached directory entires."
#define APPN_CP_STORE_KEY_HELP    "Interval (seconds) between safe-stores."
#define APPN_CP_TREES_KEY_HELP    "Maximum number of cached topology trees."
#define APPN_CP_XID_VAL_HELP      "ID number for XID exchange."
#define APPN_CP_INTR_SW_KEY_HELP  "Configure interrupt switched ISR"
#define APPN_CP_BUF_PERCENT_KEY_HELP  "Percentage of buffer memory to be used for APPN."
#define APPN_CP_MEM_KEY_HELP  "Maximum amount of memory available to APPN"
#define APPN_CP_MIN_KEY_HELP  "Amount of memory reserved for APPN"
#define APPN_TRACE_FLAGS_KEY_HELP  "Initial setting for internal tracing"
#define APPN_CP_CYCLES_KEY_HELP   "Number of Safe-Store file cycles"
#define APPN_CP_HOST_KEY_HELP     "Safe-Store Host Information"
#define APPN_CP_HOST_IP_KEY_HELP  "IP Address of Safe-Store Host"
#define APPN_CP_HOST_DIR_KEY_HELP "Storage Directory on Safe-Store Host"
#define APPN_CP_DLUR_KEY_HELP		"DLUR is supported at this CP"
#define APPN_CP_DLUS_KEY_HELP		"Default DLUS Name for this CP"
#define APPN_CP_BKUP_DLUS_KEY_HELP	"Default Backup DLUS name for this CP"
#define APPN_DLUR_MAXPUS_KEY_HELP    "Limit the total active PUs DLUR will support"
#define APPN_CP_DLUS_RETRY_INT_KEY_HELP  "Interval between retries for PUs with no DLUS"
#define APPN_CP_PREFER_ACT_KEY_HELP  "Allow connections only to the currently active DLUS"

#define APPN_CP_XID_VAL_HELP      "ID number for XID exchange."
#define APPN_CP_BLOCK_VAL_HELP    "Block ID for XID exchange."
#define APPN_CP_RESIST_VAL_HELP   "Node's resistance in route calculation."
#define APPN_CP_CACHE_VAL_HELP    "Maximum number of cached directory entires."
#define APPN_CP_STORE_VAL_HELP    "Interval (seconds) between safe-stores."
#define APPN_CP_TREES_VAL_HELP    "Maximum number of cached topology trees."
#define APPN_CP_BUF_PERCENT_VAL_HELP "Percentage of memory (1 - 100%)"
#define APPN_CP_MEM_VAL_HELP "Amount of memory (3000000-64000000 bytes)"
#define APPN_CP_MIN_VAL_HELP "Amount of memory (1000000-64000000 bytes)"
#define APPN_TRACE_FLAGS_VAL_HELP ""
#define APPN_CP_CYCLES_VAL_HELP   "Number of Safe-Store file cycles (1 - 255)"
#define APPN_CP_HOST_IP_VAL_HELP   "IP Address of Safe-Store Host"
#define APPN_CP_HOST_DIR_VAL_HELP  "Storage Directory on Safe-Store Host"
#define APPN_CP_DLUS_VAL_HELP		"Default DLUS Name for this CP"
#define APPN_CP_BKUP_DLUS_VAL_HELP	"Default Backup DLUS name for this CP"
#define APPN_DLUR_MAXPUS_VAL_HELP    "Limit the total active PUs DLUR will support"
#define APPN_CP_DLUS_RETRY_INT_VAL_HELP   "Interval (seconds) between retries"


#define APPN_LINK_CPCP         21
#define APPN_LINK_NODTYP       22
#define APPN_LINK_LANADDR      23
#define APPN_LINK_SMDSADDR     24
#define APPN_LINK_PPPADDR      25
#define APPN_LINK_ATMADDR      26
#define APPN_LINK_FRADDR       27 
#define APPN_LINK_SDLCADDR     28
#define APPN_LINK_CPNM         29
#define APPN_LINK_EFFCAP       30
#define APPN_LINK_BYTE         31
#define APPN_LINK_CONNECT      32
#define APPN_LINK_DELAY        33
#define APPN_LINK_SECUR        34  
#define APPN_LINK_USER1        35
#define APPN_LINK_USER2        36
#define APPN_LINK_USER3        37
#define APPN_LINK_LIMIT        38
#define APPN_LINK_ROLE         39
#define APPN_LINK_TGNUM        40
#define APPN_LINK_RETRY        41
#define APPN_LINK_CALL         42
#define APPN_LINK_PORT         43
#define APPN_LINK_ADAPT        44

#define APPN_LINK_COMP         APPN_COMPLETE
#define APPN_LINK_RCVBTU       45
#define APPN_LINK_SNDBTU       46
#define APPN_LINK_DLUS         47
#define APPN_LINK_BKUP_DLUS    48
#define APPN_LINK_DSPU	       49
#define APPN_LINK_PU_TYPE_20   50
#define APPN_LINK_QLLCADDR     51
#define APPN_LINK_QUEUING      52
#define APPN_LINK_AUTO         53

#define APPN_LINK_CPCP_KEY     "cp-cp-sessions-supported"
#define APPN_LINK_NODTYP_KEY   "verify-adjacent-node-type"
#define APPN_LINK_NODTYP_LEARN "learn"
#define APPN_LINK_NODTYP_NN    "nn"
#define APPN_LINK_NODTYP_EN    "en"
#define APPN_LINK_NODTYP_LEN   "len"
#define APPN_LINK_LANADDR_KEY  "lan-dest-address"   
#define APPN_LINK_SMDSADDR_KEY "smds-dest-address"   
#define APPN_LINK_PPPADDR_KEY  "ppp-dest-address"   
#define APPN_LINK_ATMADDR_KEY  "atm-dest-address"
#define APPN_LINK_FRADDR_KEY   "fr-dest-address"
#define APPN_LINK_SDLCADDR_KEY "sdlc-dest-address"
#define APPN_LINK_QLLCADDR_KEY "x25-dest-address"
#define APPN_LINK_QLLC_PVC_KEY     "pvc"
#define APPN_LINK_QLLC_SVC_KEY     "svc"
#define APPN_LINK_CPNM_KEY     "adjacent-cp-name"
#define APPN_LINK_EFFCAP_KEY   "effective-capacity"
#define APPN_LINK_BYTE_KEY     "cost-per-byte"
#define APPN_LINK_CONNECT_KEY  "cost-per-connect-time"   
#define APPN_LINK_DELAY_KEY    "propagation-delay"
#define APPN_LINK_DELAY_MIN    "minimum"
#define APPN_LINK_DELAY_LAN    "lan"
#define APPN_LINK_DELAY_PHONE  "telephone"
#define APPN_LINK_DELAY_PACK   "packet-switched"
#define APPN_LINK_DELAY_SAT    "satellite"
#define APPN_LINK_DELAY_MAX    "maximum"
#define APPN_LINK_SECUR_KEY    "security"
#define APPN_LINK_SECUR_NON    "nonsecure"
#define APPN_LINK_SECUR_PUB    "public-switched"
#define APPN_LINK_SECUR_UND    "underground-cable"
#define APPN_LINK_SECUR_SEC    "secure-conduit"
#define APPN_LINK_SECUR_GUA    "guarded-conduit"
#define APPN_LINK_SECUR_ENC    "encrypted"
#define APPN_LINK_SECUR_RAD    "guarded-radiation"
#define APPN_LINK_USER1_KEY    "user-defined-1"
#define APPN_LINK_USER2_KEY    "user-defined-2" 
#define APPN_LINK_USER3_KEY    "user-defined-3"
#define APPN_LINK_LIMIT_KEY    "limited-resource"
#define APPN_LINK_LIMIT_YES    "yes"
#define APPN_LINK_LIMIT_NO     "no"
#define APPN_LINK_AUTO_KEY     "auto-activate-on-demand"
#define APPN_LINK_ROLE_KEY     "role"
#define APPN_LINK_ROLE_PRI     "primary"
#define APPN_LINK_ROLE_SEC     "secondary"
#define APPN_LINK_ROLE_NEG     "negotiable"
#define APPN_LINK_TGNUM_KEY    "tg-number"
#define APPN_LINK_QUEUING_KEY  "link-queuing"
#define APPN_LINK_QUEUING_PRIORITY_KEY  "priority"
#define APPN_LINK_QUEUING_CUSTOM_KEY    "custom"
#define APPN_LINK_RETRY_KEY    "retry-limit"
#define APPN_LINK_RETRY_INF    "infinite"
#define APPN_LINK_CALL_KEY     "connect-at-startup"
#define APPN_LINK_PORT_KEY     "port"
#define APPN_LINK_ADAPT_KEY    "adapter"
#define APPN_LINK_RCVBTU_KEY   "max-rcv-btu-size"
#define APPN_LINK_SNDBTU_KEY   "desired-max-send-btu-size"
#define APPN_LINK_DLUS_KEY         "dlus"
#define APPN_LINK_BKUP_DLUS_KEY    "backup-dlus"
#define APPN_LINK_PU_TYPE_20_KEY   "pu-type-20"
#define APPN_LINK_DSPU_KEY         "dlur-dspu-name"

#define APPN_LINK_CPCP_KEY_HELP          "Specify support of CP-CP sessions on this link-station"
#define APPN_LINK_NODTYP_KEY_HELP        "Specify verification of node type during link activation"
#define APPN_LINK_NODTYP_LEARN_KEY_HELP  "Accept any node type"
#define APPN_LINK_NODTYP_NN_KEY_HELP     "Accept only connections to NNs on this link"
#define APPN_LINK_NODTYP_EN_KEY_HELP     "Accept only connections to ENs on this link"
#define APPN_LINK_NODTYP_LEN_KEY_HELP    "Accept only connections to LENs on this link"
#define APPN_LINK_LANADDR_KEY_HELP       "LAN hardware address of remote node on this link"
#define APPN_LINK_SMDSADDR_KEY_HELP      "SMDS hardware address of remote node on this link"
#define APPN_LINK_ATMADDR_KEY_HELP        "ATM VC number that connect to remote node"
#define APPN_LINK_FRADDR_KEY_HELP        "Frame relay DLCI number that connect to remote node"
#define APPN_LINK_SDLCADDR_KEY_HELP      "SDLC address of remote node on this link"
#define APPN_LINK_PPPADDR_KEY_HELP       "Specify a destination SAP for a PPP link"
#define APPN_LINK_CPNM_KEY_HELP          "Verify of CP Name of remote node on this link"
#define APPN_LINK_EFFCAP_KEY_HELP        "Effective capacity of this link"
#define APPN_LINK_BYTE_KEY_HELP          "Specify relative cost per byte for this link"
#define APPN_LINK_CONNECT_KEY_HELP       "Specify relative cost per connect time for this link"
#define APPN_LINK_DELAY_KEY_HELP         "Specify propagation delay for this link"
#define APPN_LINK_DELAY_MIN_KEY_HELP     ""
#define APPN_LINK_DELAY_LAN_KEY_HELP     ""
#define APPN_LINK_DELAY_PHONE_KEY_HELP   ""
#define APPN_LINK_DELAY_PACK_KEY_HELP    ""
#define APPN_LINK_DELAY_SAT_KEY_HELP     ""
#define APPN_LINK_DELAY_MAX_KEY_HELP     ""
#define APPN_LINK_SECUR_KEY_HELP         ""
#define APPN_LINK_SECUR_NON_KEY_HELP     ""
#define APPN_LINK_SECUR_PUB_KEY_HELP     ""
#define APPN_LINK_SECUR_UND_KEY_HELP     ""
#define APPN_LINK_SECUR_SEC_KEY_HELP     ""
#define APPN_LINK_SECUR_GUA_KEY_HELP     ""
#define APPN_LINK_SECUR_ENC_KEY_HELP     ""
#define APPN_LINK_SECUR_RAD_KEY_HELP     ""
#define APPN_LINK_USER1_KEY_HELP         "Specify relative cost of a network unique characteristic"
#define APPN_LINK_USER2_KEY_HELP         "Specify relative cost of a network unique characteristic"
#define APPN_LINK_USER3_KEY_HELP         "Specify relative cost of a network unique characteristic"
#define APPN_LINK_LIMIT_KEY_HELP         "Indicate this link is a limited resource"
#define APPN_LINK_LIMIT_YES_KEY_HELP     ""
#define APPN_LINK_LIMIT_NO_KEY_HELP      ""
#define APPN_LINK_AUTO_KEY_HELP          "Indicate this link (TG) is auto-activatable on demand"
#define APPN_LINK_AUTO_YES_KEY_HELP      ""
#define APPN_LINK_AUTO_NO_KEY_HELP       ""
#define APPN_LINK_ROLE_KEY_HELP          "Specify link station role for XID exchange"
#define APPN_LINK_ROLE_PRI_KEY_HELP      "Predefine as primary"
#define APPN_LINK_ROLE_SEC_KEY_HELP      "Predefine as secondary"
#define APPN_LINK_ROLE_NEG_KEY_HELP      "Negotiate link station role during XID exchange"
#define APPN_LINK_TGNUM_KEY_HELP         "Specify a predefined TG number"
#define APPN_LINK_QUEUING_KEY_HELP       "Specify a queuing parameter for Cisco Priority/Custom Queueing"
#define APPN_LINK_QUEUING_PRIORITY_KEY_HELP "priority queuing parameter"
#define APPN_LINK_QUEUING_CUSTOM_KEY_HELP   "Custom queuing"
#define APPN_LINK_RETRY_KEY_HELP         "Specify number of retry attempts on a link-station outage"
#define APPN_LINK_RETRY_INF_KEY_HELP     "Retry indefinitely"
#define APPN_LINK_CALL_KEY_HELP          "Indicates local node initiates link activation"
#define APPN_LINK_PORT_KEY_HELP          "Specify which port this link-station will use"
#define APPN_LINK_RCVBTU_KEY_HELP        "Specify desired maximum size of BTUs sent on this link"
#define APPN_LINK_SNDBTU_KEY_HELP        "Specify maximum size of BTUs received on this link"

#define APPN_LINK_DLUS_KEY_HELP		"Default DLUS Name For This Link"
#define APPN_LINK_BKUP_DLUS_KEY_HELP	"Default Backup DLUS Name For This Link"
#define APPN_LINK_PU_TYPE_20_KEY_HELP   "Indicate if PU Type 2.0 is defined"
#define APPN_LINK_DSPU_KEY_HELP         "Downstream PU name"

#define APPN_LINK_LANADDR_VAL_HELP    "LAN hardware address of remote node"
#define APPN_LINK_LANSAP_VAL_HELP     "SAP of remote node, should be a hex # divisible by 2"
#define APPN_LINK_SMDSADDR_VAL_HELP    "SMDS hardware address of remote node"
#define APPN_LINK_ATMADDR_VAL_HELP    "ATM VC number that connect to remote node"
#define APPN_LINK_FRADDR_VAL_HELP     "Frame relay DLCI number that connect to remote node"
#define APPN_LINK_SDLCADDR_VAL_HELP    "SDLC address of remote node"
#define APPN_LINK_CPNM_VAL_HELP       "CP Name of remote node"
#define APPN_LINK_EFFCAP_VAL_HELP     "Effective capacity of this link"
#define APPN_LINK_BYTE_VAL_HELP       ""
#define APPN_LINK_CONNECT_VAL_HELP    ""
#define APPN_LINK_USER1_VAL_HELP      ""
#define APPN_LINK_USER2_VAL_HELP      ""
#define APPN_LINK_USER3_VAL_HELP      ""
#define APPN_LINK_TGNUM_VAL_HELP      ""
#define APPN_LINK_QUEUING_CUSTOM_VAL_HELP   "Custom queue number"
#define APPN_LINK_RETRY_VAL_HELP      "Retry enabled on this link station or not"
#define APPN_LINK_RETRY_TRIES_HELP    "Number of times to retry link activation"
#define APPN_LINK_RETRY_TIME_HELP     "Seconds between link station retries"
#define APPN_LINK_PORT_VAL_HELP       ""
#define APPN_LINK_ADAPT_VAL_HELP      ""
#define APPN_LINK_RCVBTU_VAL_HELP     ""
#define APPN_LINK_SNDBTU_VAL_HELP     ""
#define APPN_LINK_DLUS_VAL_HELP	     "Fully-Qual DLUS Name For This Link"
#define APPN_LINK_BKUP_DLUS_VAL_HELP "Fully-Qual Backup DLUS Name For This Link"
#define APPN_LINK_DSPU_VAL_HELP	     "Downstream PU Name For This Link"
#define APPN_LINK_QLLC_KEY_HELP     ""
#define APPN_LINK_QLLC_PVC_KEY_HELP     ""
#define APPN_LINK_QLLC_SVC_KEY_HELP     ""
#define APPN_LINK_QLLC_PVC_VAL_HELP     ""
#define APPN_LINK_QLLC_SVC_VAL_HELP     ""


#define APPN_PORT_SAP          21
#define APPN_PORT_RCVBTU       22      
#define APPN_PORT_SNDBTU       23      
#define APPN_PORT_MAXL         24    
#define APPN_PORT_INB          25   
#define APPN_PORT_OUTB         26    
#define APPN_PORT_ANY          27   
#define APPN_PORT_EFFCAP       28
#define APPN_PORT_BYTE         29
#define APPN_PORT_CONNECT      30
#define APPN_PORT_DELAY        31
#define APPN_PORT_SECUR        32  
#define APPN_PORT_USER1        33
#define APPN_PORT_USER2        34
#define APPN_PORT_USER3        35
#define APPN_PORT_LIMIT        36
#define APPN_PORT_ROLE         37
#define APPN_PORT_TGNUM        38
#define APPN_PORT_RETRY        39
#define APPN_PORT_ADAPT        40
#define APPN_PORT_RSRB         41
#define APPN_PORT_SDLC         42
#define APPN_PORT_QLLC         43
#define APPN_PORT_VDLC         44
#define APPN_PORT_AUTO         45
#define APPN_PORT_IDLE         46
#define APPN_PORT_LLC2_DELAY   47
#define APPN_PORT_ACKMAX       48
#define APPN_PORT_LWIN         49
#define APPN_PORT_T1           50
#define APPN_PORT_LEN          51
#define APPN_PORT_ACTIVATE     56
#define APPN_PORT_COMP         APPN_COMPLETE

#define APPN_PORT_SAP_KEY      "local-sap"
#define APPN_PORT_RCVBTU_KEY   "max-rcv-btu-size"
#define APPN_PORT_SNDBTU_KEY   "desired-max-send-btu-size"
#define APPN_PORT_MAXL_KEY     "max-link-stations"
#define APPN_PORT_INB_KEY      "reserved-inbound"
#define APPN_PORT_OUTB_KEY     "reserved-outbound"
#define APPN_PORT_ANY_KEY      "service-any"
#define APPN_PORT_EFFCAP_KEY   "effective-capacity"
#define APPN_PORT_BYTE_KEY     "cost-per-byte"
#define APPN_PORT_CONNECT_KEY  "cost-per-connect-time"   
#define APPN_PORT_DELAY_KEY    "propagation-delay"
#define APPN_PORT_DELAY_MIN    "minimum"
#define APPN_PORT_DELAY_LAN    "lan"
#define APPN_PORT_DELAY_PHONE  "telephone"
#define APPN_PORT_DELAY_PACK   "packet-switched"
#define APPN_PORT_DELAY_SAT    "satellite"
#define APPN_PORT_DELAY_MAX    "maximum"
#define APPN_PORT_SECUR_KEY    "security"
#define APPN_PORT_SECUR_NON    "nonsecure"
#define APPN_PORT_SECUR_PUB    "public-switched"
#define APPN_PORT_SECUR_UND    "underground-cable"
#define APPN_PORT_SECUR_SEC    "secure-conduit"
#define APPN_PORT_SECUR_GUA    "guarded-conduit"
#define APPN_PORT_SECUR_ENC    "encrypted"
#define APPN_PORT_SECUR_RAD    "guarded-radiation"
#define APPN_PORT_USER1_KEY    "user-defined-1"
#define APPN_PORT_USER2_KEY    "user-defined-2" 
#define APPN_PORT_USER3_KEY    "user-defined-3"
#define APPN_PORT_LIMIT_KEY    "limited-resource"
#define APPN_PORT_LEN_KEY      "len-connection"
#define APPN_PORT_AUTO_KEY     "auto-activate-on-demand"
#define APPN_PORT_ROLE_KEY     "role"
#define APPN_PORT_ROLE_PRI     "primary"
#define APPN_PORT_ROLE_SEC     "secondary"
#define APPN_PORT_ROLE_NEG     "negotiable"
#define APPN_PORT_TGNUM_KEY    "tg-number"
#define APPN_PORT_RETRY_KEY    "retry-limit"
#define APPN_PORT_RETRY_INF    "infinite"
#define APPN_PORT_RSRB_KEY     "rsrb-virtual-station"
#define APPN_PORT_SDLC_KEY     "sdlc-sec-addr"
#define APPN_PORT_QLLC_KEY     "x25-subaddress"
#define APPN_PORT_QLLC_PVC_KEY "pvc"
#define APPN_PORT_QLLC_SVC_KEY "svc"
#define APPN_PORT_VDLC_KEY     "vdlc"
#define APPN_PORT_IDLE_KEY     "llc2-idle-time"
#define APPN_PORT_LLC2_DELAY_KEY "llc2-ack-delay-time"
#define APPN_PORT_ACKMAX_KEY   "llc2-ack-max"
#define APPN_PORT_LWIN_KEY     "llc2-local-window"
#define APPN_PORT_T1_KEY       "llc2-t1-time"
#define APPN_PORT_ACTIVATE_KEY "act-at-startup"

#define APPN_PORT_SAP_KEY_HELP          "Specify local SAP for this port"
#define APPN_PORT_RCVBTU_KEY_HELP       "Specify maximum size of BTUs received on this port"
#define APPN_PORT_SNDBTU_KEY_HELP       "Specify desired maximum size of BTUs send on this port"
#define APPN_PORT_MAXL_KEY_HELP         "Specify maximum number of link-station allowed on this port"
#define APPN_PORT_INB_KEY_HELP          "Reserve a number of link-stations for inbound links"
#define APPN_PORT_OUTB_KEY_HELP         "Reserve a number of link-stations for outbount links"
#define APPN_PORT_ANY_KEY_HELP          "Indicates that links to any remote node is allowed"
#define APPN_PORT_EFFCAP_KEY_HELP        "Effective capacity for this port"
#define APPN_PORT_BYTE_KEY_HELP        "Relative cost per byte for this port"
#define APPN_PORT_CONNECT_KEY_HELP        "Relative cost per connect time for this port"
#define APPN_PORT_DELAY_KEY_HELP        "Propagation delay for this port"
#define APPN_PORT_DELAY_MIN_KEY_HELP    ""
#define APPN_PORT_DELAY_LAN_KEY_HELP    ""
#define APPN_PORT_DELAY_PHONE_KEY_HELP    ""
#define APPN_PORT_DELAY_PACK_KEY_HELP    ""
#define APPN_PORT_DELAY_SAT_KEY_HELP    ""
#define APPN_PORT_DELAY_MAX_KEY_HELP    ""
#define APPN_PORT_SECUR_KEY_HELP        "Security for this port"
#define APPN_PORT_SECUR_NON_KEY_HELP     ""
#define APPN_PORT_SECUR_PUB_KEY_HELP     ""
#define APPN_PORT_SECUR_UND_KEY_HELP     ""
#define APPN_PORT_SECUR_SEC_KEY_HELP     ""
#define APPN_PORT_SECUR_GUA_KEY_HELP     ""
#define APPN_PORT_SECUR_ENC_KEY_HELP     ""
#define APPN_PORT_SECUR_RAD_KEY_HELP     ""
#define APPN_PORT_USER1_KEY_HELP         "Specify relative cost of a network unique characteristic"
#define APPN_PORT_USER2_KEY_HELP         "Specify relative cost of a network unique characteristic"
#define APPN_PORT_USER3_KEY_HELP         "Specify relative cost of a network unique characteristic"
#define APPN_PORT_LIMIT_KEY_HELP         "Indicates the links (TGs) using this port are a limited resource"
#define APPN_PORT_LEN_KEY_HELP           "Indicates the links (TGs) using this port are of LEN type"
#define APPN_PORT_AUTO_KEY_HELP          "Indicates the links (TGs) using this port are auto-activatable on demand"
#define APPN_PORT_ROLE_KEY_HELP        "Specifies link-station role for link-stations on this port"
#define APPN_PORT_ROLE_PRI_KEY_HELP    ""
#define APPN_PORT_ROLE_SEC_KEY_HELP    ""
#define APPN_PORT_ROLE_NEG_KEY_HELP    ""
#define APPN_PORT_TGNUM_KEY_HELP        "Predefine a TG number for links on this port"
#define APPN_PORT_ADAPT_KEY_HELP        "Specify adapter number associated with this port"
#define APPN_PORT_RETRY_INF_KEY_HELP    "Retry indefinitely"
#define APPN_PORT_RETRY_KEY_HELP        "Specify number of retry attempts on all link-stations using this port"
#define APPN_PORT_IDLE_KEY_HELP     "Frequency of polls during periods of idle traffic"
#define APPN_PORT_LLC2_DELAY_KEY_HELP    "Max time the router allows incoming I-frames to stay unacknowledged"
#define APPN_PORT_ACKMAX_KEY_HELP   "Max number of I-frames received before an acknowledgment must be sent"
#define APPN_PORT_LWIN_KEY_HELP     "Max number of I-frames to send before waiting for an acknowledgment"
#define APPN_PORT_T1_KEY_HELP       "How long router waits for an acknowledgment to transmitted I-frames"
#define APPN_PORT_ACTIVATE_KEY_HELP "Activate the port at appn startupo time (default: YES)"

#define APPN_PORT_SAP_VAL_HELP      "A hex number divisible by 2, default is 4"
#define APPN_PORT_RCVBTU_VAL_HELP   ""
#define APPN_PORT_SNDBTU_VAL_HELP   ""
#define APPN_PORT_MAXL_VAL_HELP     ""
#define APPN_PORT_INB_VAL_HELP      ""
#define APPN_PORT_OUTB_VAL_HELP     ""
#define APPN_PORT_ANY_VAL_HELP      ""
#define APPN_PORT_EFFCAP_VAL_HELP    ""
#define APPN_PORT_BYTE_VAL_HELP    ""
#define APPN_PORT_CONNECT_VAL_HELP    ""
#define APPN_PORT_USER1_VAL_HELP    ""
#define APPN_PORT_USER2_VAL_HELP    ""
#define APPN_PORT_USER3_VAL_HELP    ""
#define APPN_PORT_TGNUM_VAL_HELP    ""
#define APPN_PORT_RETRY_VAL_HELP    "Retry enabled for link stations on this port or not"
#define APPN_PORT_RETRY_TRIES_HELP  "Number of times to retry link stations on this port"
#define APPN_PORT_RETRY_TIME_HELP   "Seconds between link station retries for this port"
#define APPN_PORT_RSRB_KEY_HELP    "Configure RSRB Port Parameters"
#define APPN_PORT_RSRB_VADDR_VAL_HELP    "RSRB Station Virtual Mac Address"
#define APPN_PORT_RSRB_LRING_VAL_HELP    "RSRB Local Virtual Ring Number"
#define APPN_PORT_RSRB_BRIDGE_NUM_VAL_HELP "RSRB Virtual Bridge Number"
#define APPN_PORT_RSRB_RRING_VAL_HELP    "RSRB Target Ring Number"
#define APPN_PORT_SDLC_KEY_HELP      "Configure SDLC Port Paramaters"
#define APPN_PORT_SDLC_ADDR_VAL_HELP "SDLC Secondary Address"
#define APPN_PORT_QLLC_KEY_HELP      "Configure QLLC Port Parameters"
#define APPN_PORT_QLLC_PVC_KEY_HELP  "Specify QLLC Primary Virtual Circuit"
#define APPN_PORT_QLLC_SVC_KEY_HELP  "Specify QLLC Secondary Virtual Circuit"
#define APPN_PORT_QLLC_PVC_VAL_HELP  "Specify QLLC Subaddress"
#define APPN_PORT_QLLC_SVC_VAL_HELP  "Specify QLLC Subaddress"
#define APPN_PORT_VDLC_KEY_HELP          "Configure VDLC Port Parameters"
#define APPN_PORT_VDLC_VADDR_VAL_HELP    "VDLC Virtual MAC Address"
#define APPN_PORT_VDLC_RING_VAL_HELP     "VDLC Ring Group Number"
#define APPN_PORT_IDLE_VAL_HELP     "Frequency of polls in milliseconds"
#define APPN_PORT_LLC2_DELAY_VAL_HELP    "Ack delay time in milliseconds"
#define APPN_PORT_ACKMAX_VAL_HELP   "Nubmer of I-frames"
#define APPN_PORT_LWIN_VAL_HELP     "Number of I-frames"
#define APPN_PORT_T1_VAL_HELP       "T1 time in milliseconds"

#define APPN_PLOC_COMP          APPN_COMPLETE
#define APPN_PLOC_OWNNN         62
#define APPN_PLOC_LOCAL         63
#define APPN_PLOC_OWNCP         64
#define APPN_PLOC_WILD          65

#define APPN_PLOC_OWNNN_KEY     "serving-nn"
#define APPN_PLOC_LOCAL_KEY     "local-nn-serves"
#define APPN_PLOC_OWNCP_KEY     "owning-cp"
#define APPN_PLOC_WILD_KEY      "wildcard"

#define APPN_PLOC_OWNNN_VAL_HELP  ""
#define APPN_PLOC_OWNNN_KEY_HELP  "Fully qualified name of the NN serving this LU"
#define APPN_PLOC_LOCAL_KEY_HELP  "Indicates the local node is the owning NN"
#define APPN_PLOC_OWNCP_VAL_HELP  ""
#define APPN_PLOC_OWNCP_KEY_HELP  "Fully qualified name of the CP owning this LU"
#define APPN_PLOC_WILD_KEY_HELP   "Indicates this definition is wildcard entry"

#define APPN_MODE_COMP         APPN_COMPLETE
#define APPN_MODE_COS          82

#define APPN_MODE_COS_KEY      "class-of-service"

#define APPN_MODE_COS_KEY_HELP  "Specify which class of service this mode uses"
#define APPN_MODE_COS_VAL_HELP  ""

#define APPN_COS_COMP           APPN_COMPLETE
#define APPN_COS_TG             72 
#define APPN_COS_NODE           73
#define APPN_COS_PRIOR          74

#define APPN_COS_USER3_KEY           "user3"
#define APPN_COS_USER2_KEY           "user2"
#define APPN_COS_USER1_KEY           "user1"
#define APPN_COS_SECURITY_KEY        "security"
#define APPN_COS_DELAY_KEY           "delay"
#define APPN_COS_CAPACITY_KEY        "capacity"
#define APPN_COS_TIME_KEY            "time"
#define APPN_COS_BYTE_KEY            "byte"
#define APPN_COS_WEIGHT_KEY          "weight"
#define APPN_COS_TG_KEY              "tg-row"
#define APPN_COS_ROUTE_KEY           "route-additional-resistance"
#define APPN_COS_YES_KEY             "yes"
#define APPN_COS_NO_KEY              "no"
#define APPN_COS_CONGEST_KEY         "congestion"
#define APPN_COS_NODE_KEY            "node-row"
#define APPN_COS_PRIOR_LOW           "low"
#define APPN_COS_PRIOR_MED           "medium"
#define APPN_COS_PRIOR_HIGH          "high"
#define APPN_COS_PRIOR_NET           "network"
#define APPN_COS_PRIOR_KEY           "transmission-priority"

#define APPN_COS_USER3_HI_VAL_HELP        "Maximum user-defined-3 for this row"
#define APPN_COS_USER3_LOW_VAL_HELP       "Minimum user-defined-3 for this row"
#define APPN_COS_USER3_KEY_HELP           "Define user-defined-3 range for TGs in this row"
#define APPN_COS_USER2_HI_VAL_HELP        "Maximum user-defined-2 for this row"
#define APPN_COS_USER2_LOW_VAL_HELP       "Minimum user-defined-2 for this row"
#define APPN_COS_USER2_KEY_HELP           "Define user-defined-2 range for TGs in this row"
#define APPN_COS_USER1_HI_VAL_HELP        "Maximum user-defined-1 for this row"
#define APPN_COS_USER1_LOW_VAL_HELP       "Minimum user-defined-1 for this row"
#define APPN_COS_USER1_KEY_HELP           "Define user-defined-1 range for TGs in this row"
#define APPN_COS_SECURITY_HI_VAL_HELP     "Maximum security for this row"
#define APPN_COS_SECURITY_LOW_VAL_HELP    "Minimum security for this row"
#define APPN_COS_SECURITY_KEY_HELP           "Define security range for TGs in this row"
#define APPN_COS_DELAY_HI_VAL_HELP           "Maximum propagation-delay for this row"
#define APPN_COS_DELAY_LOW_VAL_HELP          "Minimum propagation-delay for this row"
#define APPN_COS_DELAY_KEY_HELP              "Define propagation-delay range for TGs in this row"
#define APPN_COS_CAPACITY_HI_VAL_HELP        "Maximum effective-capacity for this row"
#define APPN_COS_CAPACITY_LOW_VAL_HELP       "Minimum effective-capacity for this row"
#define APPN_COS_CAPACITY_KEY_HELP           "Define effective-capacity range for TGs in this row"
#define APPN_COS_TIME_HI_VAL_HELP            "Maximum cost-per-connect-time for this row"
#define APPN_COS_TIME_LOW_VAL_HELP           "Minimum cost-per-connect-time for this row"
#define APPN_COS_TIME_KEY_HELP               "Define cost-per-connect-time range for TGs in this row"
#define APPN_COS_BYTE_HI_VAL_HELP            "Maximum cost-per-byte for this row"
#define APPN_COS_BYTE_LOW_VAL_HELP           "Minimum cost-per-byte for this row"
#define APPN_COS_BYTE_KEY_HELP               "Define cost-per-byte range of TGs that fit in this row"
#define APPN_COS_TG_WEIGHT_VAL_HELP          "Specify the weight of TGs that fit in this row"
#define APPN_COS_TG_WEIGHT_KEY_HELP          "Specify the weight of TGs that fit in this row"
#define APPN_COS_TG_INDEX_VAL_HELP           "Specify which TG row"
#define APPN_COS_TG_KEY_HELP                 "Define a TG row for this COS"
#define APPN_COS_ROUTE_HI_VAL_HELP            "Maximum route-additional-resistance for this row"
#define APPN_COS_ROUTE_LOW_VAL_HELP           "Minimum route-additional-resistance for this row"
#define APPN_COS_ROUTE_KEY_HELP        "Define route-additional-resistance of node that fit in this row"
#define APPN_COS_CONGEST_HI_YES_KEY_HELP            "Maximum congestion for this row" 
#define APPN_COS_CONGEST_HI_NO_KEY_HELP             "Maximum congestion for this row" 
#define APPN_COS_CONGEST_LOW_YES_KEY_HELP           "Minimum congestion for this row" 
#define APPN_COS_CONGEST_LOW_NO_KEY_HELP           "Minimum congestion for this row"
#define APPN_COS_CONGEST_KEY_HELP              "Define congestion of nodes that fit in this row"
#define APPN_COS_NODE_WEIGHT_VAL_HELP           "Specify the weight of nodes that fit in this row"
#define APPN_COS_NODE_WEIGHT_KEY_HELP           "Specify the weight of nodes that fit in this row"
#define APPN_COS_NODE_INDEX_VAL_HELP             "Specify which row number"
#define APPN_COS_NODE_KEY_HELP               "Define a node row for this COS"
#define APPN_COS_PRIOR_LOW_KEY_HELP           ""
#define APPN_COS_PRIOR_MED_KEY_HELP           ""
#define APPN_COS_PRIOR_HIGH_KEY_HELP          ""
#define APPN_COS_PRIOR_NET_KEY_HELP           ""
#define APPN_COS_PRIOR_KEY_HELP              "Specify the network priority of this COS"

#define APPN_CN_COMP              APPN_COMPLETE
#define APPN_CN_PORT              52

#define APPN_CN_PORT_KEY          "port"

#define APPN_CN_PORT_VAL_HELP     ""
#define APPN_CN_PORT_KEY_HELP     "Specify which ports this connection network uses"

#endif
