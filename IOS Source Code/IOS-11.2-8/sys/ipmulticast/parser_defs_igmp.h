/* $Id: parser_defs_igmp.h,v 3.1.40.3 1996/08/26 06:49:02 dino Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/parser_defs_igmp.h,v $
 *------------------------------------------------------------------
 * parser_defs_igmp.h - IGMP parser constants.
 * 
 * November 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_igmp.h,v $
 * Revision 3.1.40.3  1996/08/26  06:49:02  dino
 * CSCdi67098:  Do not accept DVMRP neighbor probes/reports from
 * non-pruners
 * Branch: California_branch
 *
 * Revision 3.1.40.2  1996/06/28  07:48:22  dino
 * CSCdi61612:  Put in DVMRP route-limit, have it default to 7000, avoid
 *              MBONE meltdown.
 * Branch: California_branch
 *
 * Revision 3.1.40.1  1996/05/06  16:07:22  dino
 * CSCdi56516:  The sd proecss doesnt process sdr annoucements
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:58:37  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.5  1996/01/12  22:56:51  mleelani
 * CSCdi46802:  IGMP needs faster leave response
 * Miscellaneous v2 enhancements.
 *
 * Revision 3.4  1995/11/30  04:42:53  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.3  1995/11/21  23:12:58  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.2  1995/11/17  18:50:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/01  07:46:26  dino
 * CSCdi27921:  Border router needs to send Registers to RP. ISPs want PIM
 *              over DVMRP unicast routing.
 *
 * Revision 2.1  1995/06/07  22:30:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define IGMP_JOIN_GROUP     1
#define IGMP_QUERY_INTVL    2
#define IGMP_ACCESS_GROUP   3
#define IGMP_SHOW_GROUPS    4
#define IGMP_SHOW_INTERFACE 5
#define IGMP_VERSION        6
#define IGMP_QUERY_RESPONSE 7
#define IGMP_QUERY_TIMEOUT  8
#define IGMP_FORCE_LEAVE    9
#define IGMP_STATIC_GROUP  10

#define IGMP_DVMRP_METRIC             11
#define IGMP_DVMRP_FILTER             12
#define IGMP_DVMRP_DEFAULT            13
#define IGMP_DVMRP_UNICAST            14
#define IGMP_DVMRP_MOFFSET            15
#define IGMP_DVMRP_ROUTEHOG           16
#define IGMP_DVMRP_ROUTE_LIMIT        17
#define IGMP_DVMRP_REJECT_NON_PRUNERS 18
#define IGMP_DVMRP_DISTANCE           19

/*
 * idb->dvmrp_send_default settings.
 */
#define DVMRP_SEND_DEFAULT 1
#define DVMRP_DEFAULT_ONLY 2

/*
 * sd/sdr settings.
 */
#define SDR_LISTEN_COMMAND 1
#define SDR_CACHETIMEOUT_COMMAND 2

#define IGMP_QUERY_NAME		"host-query"
#define IGMP_REPORT_NAME	"host-report"
#define IGMP_DVMRP_NAME		"dvmrp"
#define IGMP_PIM_NAME		"pim"
#define IGMP_TRACE_NAME		"trace"

