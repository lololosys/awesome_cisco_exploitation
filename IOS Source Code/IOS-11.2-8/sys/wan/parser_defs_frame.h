/*
 *	P A R S E R _ D E F S _ F R A M E . H
 *
 * $Id: parser_defs_frame.h,v 3.1.40.4 1996/07/03 01:57:23 chifei Exp $
 * $Source: /release/112/cvs/Xsys/wan/parser_defs_frame.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_frame.h,v $
 * Revision 3.1.40.4  1996/07/03  01:57:23  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.1.40.3  1996/05/20  05:37:50  skoh
 * CSCdi56032:  New SVC commands in 11.2 (partial)
 * Branch: California_branch
 * Syntax change in fr svc map-class subcommands.
 *
 * Revision 3.1.40.2  1996/05/15  05:55:05  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1.40.1  1996/04/25  23:24:32  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.44.1  1996/04/24  04:11:56  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1996/02/01  05:14:31  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.4  1996/01/29  07:00:55  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.3  1995/11/21  20:43:52  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * syncing code to 11.0
 *
 * Revision 3.2  1995/11/17  18:50:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:29:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Frame relay command identifiers.
 */
enum fr_command_set {
    FRAME_RELAY_DLCI_BITS,
    FRAME_RELAY_LMI_TYPE,
    FRAME_RELAY_LOCAL_DLCI,
    FRAME_RELAY_KEEPALIVE,
    FRAME_RELAY_MAP,
    FRAME_RELAY_MULTICAST,
    FRAME_RELAY_ZAP,
    FRAME_RELAY_SWITCHING,
    FRAME_RELAY_INTF,
    FRAME_RELAY_ROUTE,
    FRAME_RELAY_ENQUEUE,
    FR_LMI_N391DTE,
    FR_LMI_N392DTE,
    FR_LMI_N393DTE,
    FR_LMI_T392DCE,
    FR_LMI_N392DCE,
    FR_LMI_N393DCE,
    FR_IN_ARP,
    FR_CLEAR_INARP,
    FR_DLCI,
    FR_BQ,
    FR_TCP_COMPRESS,
    FR_PR_UDP,
    FR_PR_TCP,
    FR_PR_FRAGS,
    FR_PR_LIST,
    FR_PR_LT,
    FR_PR_GT,
    FR_PRICMD_PROTOCOL,
    FR_PRICMD_INTERFACE,
    FR_LISTNUM,
    FR_DEGROUP,
    FRAME_RELAY_SVC_ENABLE,
    FRAME_RELAY_SVC_B2B_ENABLE,
    FRAME_RELAY_SVC_EXT_INARP,
    FRAME_RELAY_SVC_LOCAL_ADDR,
    FRAME_RELAY_SVC_DIAG_SETUP,
    DIAG_L3_SETUP_PACK,
    FR_MAP_CLASS_BROAD,
    FR_MAP_CLASS_LOCAL_ADDR,
    FR_MAP_CLASS_DEST_ADDR,
    FR_MAP_CLASS_DCN,
    FR_MAP_CLASS_DLCI,
    FR_MAP_CLASS_CIR,
    FR_MAP_CLASS_IN_CIR,
    FR_MAP_CLASS_OUT_CIR,
    FR_MAP_CLASS_MIN_CIR,
    FR_MAP_CLASS_MIN_IN_CIR,
    FR_MAP_CLASS_MIN_OUT_CIR,
    FR_MAP_CLASS_CBS,
    FR_MAP_CLASS_IN_CBS,
    FR_MAP_CLASS_OUT_CBS,
    FR_MAP_CLASS_EBS,
    FR_MAP_CLASS_IN_EBS,
    FR_MAP_CLASS_OUT_EBS,
    FR_MAP_CLASS_IDLE_TIMER,
    FR_MAP_CLASS_HOLDQ,
    FR_MAP_CLASS_ENCAPS_IETF,
    FR_MAP_CLASS_FMIF,
    FR_MAP_CLASS_IN_FMIF,
    FR_MAP_CLASS_OUT_FMIF,
    FR_MAP_CLASS_T303,
    FR_MAP_CLASS_T308,
    FR_MAP_CLASS_T310,
    FR_MAP_CLASS_T322,
    FR_MAP_CLASS_T303_RETRY,
    FR_MAP_CLASS_T308_RETRY,
    FR_MAP_CLASS_STATUS_ENQ_CNT,
    FR_DLCI_EXCLUSIVE, 
    FR_DLCI_GROUP,
    FR_SVC_IDLE_TIMER_SET,
    FR_LAPF_CMD,
    FR_STAC_COMPRESS,
    FR_INT_CLASS,
    FR_DLCI_CLASS,
    FR_TRAFFIC_SWITCH,
    FR_TRAFFIC_RATE,
    FR_TRAFFIC_BECN
};

/*
 * As per page 76 of Q.931. Numbering Plan for Called Party IE.
 */
enum numbering_plan {
      FR_UNKNOWN_1,
      FR_E164_ADDR,
      FR_UNKNOWN_2,
      FR_X121_ADDR,
      FR_TELEX,
      FR_NATIONAL,
      FR_PRIVATE,
      FR_FUTURE
};


# define FR_MAP_INIT_OPTIONS           0x0
# define FR_MAP_PASSIVE                0x0001
# define FR_INTF_PASSIVE               0x0001	/* same as FR_MAP_PASSIVE */
# define FR_MAP_INHERIT_PASSIVE        0x0002
# define FR_MAP_ACTIVE                 0x0004
# define FR_MAP_COMPRESSION            0x0008
# define FR_MAP_INHERIT_COMPRESSION    0x0010
# define FR_MAP_COMP_PAYLOAD_STAC      0x0020
# define FR_MAP_COMP_PAYLOAD_PRED      0x0040
# define FR_MAP_NO_COMPRESS            0x0080

/* Frame Relay show commands */
enum {
    SHOW_FRAME_RELAY_MAP,
    SHOW_FRAME_RELAY_TRAFFIC,
    SHOW_FRAME_RELAY_PVC,
    SHOW_FRAME_RELAY_LMI,
    SHOW_FRAME_RELAY_ROUTE,
    SHOW_FRAME_RELAY_COMP,
    SHOW_FR_SVC_MAPLIST,
    SHOW_FRAME_RELAY_LAPF
};
