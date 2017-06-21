/* $Id: ibmnm_lbs.c,v 3.7.8.8 1996/09/04 23:55:30 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ibm/ibmnm_lbs.c,v $
 *------------------------------------------------------------------
 * ibmnm_lbs.c -- IBM Network Management Protocol
 *                LAN Bridge Server Functions
 *
 * January 1991, David R. Hampton
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibmnm_lbs.c,v $
 * Revision 3.7.8.8  1996/09/04  23:55:30  snyder
 * CSCdi68120:  save a little memory here and there
 *              declare things that are const, const
 *              savings 300 out of data section
 *                       64 out of image
 * Branch: California_branch
 *
 * Revision 3.7.8.7  1996/08/26  15:07:59  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.7.8.6  1996/08/07  08:59:23  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.7.8.5  1996/07/18  20:28:26  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.7.8.4  1996/07/09  05:57:51  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.7.8.3  1996/06/05  14:08:50  ppearce
 * CSCdi59100:  SRB drops directed frames w/broadcast indicators 010 or 011
 * Branch: California_branch
 *   Remove clear of RII bit from send_trace_report()
 *
 * Revision 3.7.8.2  1996/05/17  11:20:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.4.4  1996/04/26  15:19:36  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.4.3  1996/04/26  07:54:00  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.4.2  1996/04/03  14:33:42  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.4.1  1996/03/17  17:50:16  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.7.8.1  1996/03/18  20:11:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.14.6  1996/03/16  06:52:47  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.14.5  1996/03/14  01:04:00  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.5.14.4  1996/03/13  01:23:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.14.3  1996/03/07  09:41:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.14.2  1996/02/20  00:54:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/29  00:20:02  kchiu
 * CSCdi46631:  lanmgr reporting erroneous Non-Broadcast frame counters
 *
 * Revision 3.6  1996/02/08  17:57:18  ioakley
 * CSCdi47561: Resolve potential network congestion problems by adding
 *             a global configuration command to disable the automatic
 *             Report Path Trace function and also correct problems with
 *             Report Path Trace frames returned to originating stations.
 *
 * Revision 3.5.14.1  1996/01/24  21:49:08  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1995/12/17  18:28:02  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/16  22:06:42  motto
 * CSCdi35920:  %ALIGN-3-CORRECT messages on 4500
 *
 * Revision 3.3  1995/11/17  09:22:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:35:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:44:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:01:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/02  00:40:46  ioakley
 * CSCdi38962:  Correct logic error that is causing crash during
 *              insert_lbs_rte processing when AST is not configured.
 *
 * Revision 2.4  1995/08/04  03:54:00  ioakley
 * CSCdi30094:  Ensure that Automatic Spanning Tree Mode (AST) is
 *              reported correctly to the requesting LAN Manager.
 *
 * Revision 2.3  1995/07/13  20:32:58  jacobt
 * CSCdi36703:  replace malloc() by malloc_named()
 *
 * Revision 2.2  1995/06/15  22:00:41  hampton
 * Fix 24.5 day bugs in Lan Manager support.  While there, eliminate
 * direct references to msclock.  [CSCdi35914]
 *
 * Revision 2.1  1995/06/07 20:46:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * 
 * LAN Bridge Server Functions, as described in the IBM Token
 * Ring Network Architecture Reference Manual, Third Edition, September
 * 1989, Chapter 18.
 *
 * The LAN Bridge Server serves several functions.
 *
 * 1) It reports the current status of the SRB to a Lan Network Manager
 *    upon request.  This information includes all sorts of various
 *    numbers including bytes and frame counts, ring numbers, ring types,
 *    hop counts, spanning state, etc.
 *
 * 2) It changes any of the bridge specific parameters (including ring
 *    numbers) at the request of the controlling Lan Network Manager.
 *
 * 3) It notifies all other Lan Network Managers any time a parameter is
 *    changed, and tells exactly what parameters were changed and their
 *    new values.
 *
 * 4) It collects traffic statistics and periodically reports these to
 *    any interested Lan Network Managers.  This function is enabled and
 *    disabled by Lan Network Manager.  The bridge runs the timer and
 *    send periodic reports.
 *
 * 5) It notifies all Lan Network Managers whenever the performance
 *    threshold for a bridge is exceeded.  this threshold is the number
 *    of dropped per one thousand frames received.
 *
 * 6) It provides a "Path Trace" function for use by Lan Network
 *    Managers.  This function is enable by setting certain bits in the
 *    RIF of a non-explorer SRB frame.  This means that every frame must
 *    be examined to see if these bits are on.  If a frame is being
 *    traced, notify Lan Network Manager of the fact that it was
 *    forwarded.
 *
 **********************************************************************/

#include "master.h"
#include <ctype.h>
#undef   tolower                        /* use library function call */
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "lanmgr.h"
#include "ibmnm.h"
#include "../srt/srb.h"
#include "../srt/srb_core.h"
#include "../srt/srb_sb_private.h"
#include "../srt/span.h"
#include "../util/itemlist.h"

/*
 * Externals
 */
extern int lanmgr_global_enable;
extern boolean lanmgr_pathtrace_enable_lnm;
extern boolean lanmgr_pathtrace_enable_orig;

/*
 * Local Storage
 */
static uchar lbs_version[VERSIONLEN] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0};
static char *const lbs_names[] = {
    "LBS 8500", "Request Bridge Status", "Report Bridge Status",
    "Set Bridge Parameters", "Bridge Parameters Set", "Bridge Error", 
    "Bridge Parameters Changed Notification",
    "Bridge Performance Threshold Exceeded", "Bridge Path Trace Report",
    "Bridge Counter Report", "Bridge Single-Route Broadcast Status Change"};


/**********************************************************************
 *
 * Convert Internal LBS info various ways.
 *
 **********************************************************************/

#define NAME(x) lbs_names[(x) & COMMAND_MASK]

static char *lbs_to_string (
    int command)
{
    if (command < LBS_LAST)
	return(lbs_names[command & COMMAND_MASK]);
    return("unknown");
}

static int lbsid_to_bitmask (ushort command)
{
    switch (command) {
      case IBMNM_CORRELATOR:        return(LBS_B_CORRELATOR);
      case IBMNM_ALL:               return(LBS_B_ALL);
      case IBMNM_ADDRESS:	    return(LBS_B_PORT_IDENT);
      case IBMNM_PERMANENT:	    return(LBS_B_TEMP);

      case LBS_SV_PORT_INFO:	    return(LBS_B_PORT_INFO);
      case LBS_SV_RTE_ACTIVE:	    return(LBS_B_RTE_ACTIVE);
      case LBS_SV_LOSS_THRESH:	    return(LBS_B_LOSS_THRESH);
      case LBS_SV_NOTIF_INTERVAL:   return(LBS_B_NOTIF_INTERVAL);
      case LBS_SV_ROUTE_STATUS:	    return(LBS_B_ROUTE_STATUS);
      case LBS_SV_ROUTE_IDENT:	    return(LBS_B_ROUTE_IDENT);
      case LBS_SV_PORT_IDENT1:	    return(LBS_B_PORT_IDENT1);
      case LBS_SV_PORT_IDENT2:	    return(LBS_B_PORT_IDENT2);
      case LBS_SV_BR_IDENT:	    return(LBS_B_BR_IDENT);
      case LBS_SV_SRTE_ENABLED:	    return(LBS_B_SRTE_ENABLED);
      case LBS_SV_HOP_COUNT:	    return(LBS_B_HOP_COUNT);
      case LBS_SV_INTERNAL_STATUS:  return(LBS_B_INTERNAL_STATUS);
      case LBS_SV_SRTE_MODE:	    return(LBS_B_SRTE_MODE);
      case LBS_SV_ALL_LONG:	    return(LBS_B_ALL_LONG);
      default:			    return(0);
    }
}

static char *lbsbit_to_string (int bit)
{
    switch (bit) {
      case LBS_B_CORRELATOR:	   return("Correlator");
      case LBS_B_ALL:		   return("All");
      case LBS_B_PORT_IDENT:	   return("Port Identifier");
      case LBS_B_TEMP:		   return("Permanent");
   
      case LBS_B_PORT_INFO:	   return("Port Info");
      case LBS_B_RTE_ACTIVE:	   return("Route Active");
      case LBS_B_LOSS_THRESH:	   return("Loss Thresh");
      case LBS_B_NOTIF_INTERVAL:   return("Notification Interval");
      case LBS_B_ROUTE_STATUS:	   return("Route Status");
      case LBS_B_ROUTE_IDENT:	   return("Route Identifier");
      case LBS_B_PORT_IDENT1:	   return("Port Identifier1");
      case LBS_B_PORT_IDENT2:	   return("Port Identifier2");
      case LBS_B_BR_IDENT:	   return("Br Identifier");
      case LBS_B_SRTE_ENABLED:	   return("Srte Enabled");
      case LBS_B_HOP_COUNT:	   return("Hop Count");
      case LBS_B_INTERNAL_STATUS:  return("Internal Status");
      case LBS_B_SRTE_MODE:	   return("Srte Mode");
      case LBS_B_ALL_LONG:	   return("All (long)");
      default:			   return("unknown");
    }
}

/**********************************************************************
 *
 *			   Vector Builders
 *
 **********************************************************************/

/*
 * insert_lbs_pi
 *
 * Add port specific information to the current vector.  The 'which'
 * argument is a bitfield that controls what information is actually
 * included by this routine.
 */
static ibmnm_vector *insert_lbs_pi (
    ibmnm_vector *vec,
    hwidbtype *idb,
    int which)
{
    ibmnm_vector *sv;
    hwidbtype *tmpidb;
    srb_sb_t *srb_sb;

    srb_sb = srb_get_sb_inline(firstsw_or_null(idb));
    if (!srb_sb) {
        if (srberr_debug)
          buginf("ibmnm_vector() - No SRB subblock\n");
    }

    PUTSHORT(&vec->command, LBS_SV_PORT_INFO);
    sv = (ibmnm_vector *)&vec->data;
    if (idb->status & IDB_FDDI)
	sv = insert_address(sv, idb->bit_swapped_hardware);
    else
	sv = insert_address(sv, idb->hardware);
    
    if (which & LBS_PI_RING)
	sv = insert_ring(sv, srb_sb->srb_thisring);
    if (which & LBS_PI_TYPE) {
	sv = insert_short(sv, LBS_SV_PORT_TYPE, PORT_RING_STATION);
	if (idb->status & IDB_FDDI)
	    /*
	     * For FDDI always report 16 Mbps ring speed value
	     */
	    sv = insert_short(sv, LBS_SV_DATA_RATE, 16);
	else
	    sv = insert_short(sv, LBS_SV_DATA_RATE, idb->tr_ring_speed);
    }
    if (which & LBS_PI_STATUS) {
	sv = insert_short(sv, LBS_SV_RING_STATUS, RING_OPERATIONAL);
	sv = insert_short(sv, LBS_SV_ADAPTER_STATUS, 
			  (idb->state == IDBS_UP)
			  ? ADAPTER_ATTACHED : ADAPTER_NOT_ATTACHED);
    }
    if (which & LBS_PI_HOP_COUNT)
	sv = insert_byte(sv, LBS_SV_HOP_COUNT, srb_sb->srb_max_hops);
	
    if (which & LBS_PI_SET1) {
	ulong prev_err_rcvcongestion;
	ONE_ITEM_LIST(tmp, TOKEN_HW_PREV_ERR_RCVCONGESTION, sizeof(ulong), 0);

	if (which & LBS_PI_COUNTERS_LONG)
	    sv = insert_long(sv, LBS_SV_FRAME_DISC_L, srb_sb->srb_tx_drops);
	if (which & LBS_PI_COUNTERS_SHORT)
	    sv = insert_short(sv, LBS_SV_FRAME_DISC, srb_sb->srb_tx_drops);
	
	tmpidb = (idb->tr_bridge_idb ? idb->tr_bridge_idb : idb->vidb_link);
	if (tmpidb) {
            srb_sb_t *tmp_srb_sb = srb_get_sb_inline(tmpidb->firstsw);
            if (tmp_srb_sb) {
	        sv = insert_long(sv, LBS_SV_B_FR_TRANS, 
                                 tmp_srb_sb->srb_tx_b_count);
	        sv = insert_longer(sv, LBS_SV_B_BY_TRANS,
	    		           tmp_srb_sb->srb_tx_b_bytes);
                sv = insert_long(sv, LBS_SV_NB_FR_TRANS, 
                                 srb_outputs_fast(tmpidb, tmp_srb_sb) +
                                 srb_outputs_auto(tmpidb, tmp_srb_sb) +
                                 tmp_srb_sb->srb_tx_nb_count);
                sv = insert_longer(sv, LBS_SV_NB_BY_TRANS,
                                   srb_tx_cumbytes_fast(tmpidb, tmp_srb_sb) +
                                   srb_tx_cumbytes_auto(tmpidb, tmp_srb_sb) +
                                   tmp_srb_sb->srb_tx_nb_bytes);
            }
	}
	
	(*idb->state_config)(idb, IDB_CONTROL_READ_STATE, &tmp_list);
	prev_err_rcvcongestion = tmp_item.u.l_value;
	if (which & LBS_PI_COUNTERS_LONG)
	    sv = insert_long(sv, LBS_SV_FR_NOT_REC_L, prev_err_rcvcongestion);
	if (which & LBS_PI_COUNTERS_SHORT)
	    sv = insert_short(sv, LBS_SV_FR_NOT_REC, prev_err_rcvcongestion);
	if (which & LBS_PI_COUNTERS_LONG)
	    sv = insert_long(sv, LBS_SV_FR_NOT_FORW_L, srb_sb->srb_tx_drops);
	if (which & LBS_PI_COUNTERS_SHORT)
	    sv = insert_short(sv, LBS_SV_FR_NOT_FORW, srb_sb->srb_tx_drops);
#ifdef notdef
	/*
	 * According to the IBM Architecture Manual, there should be
	 * three more vectors here.  IBM's Bridge Program v2.1, however,
	 * does not include them.
	 */
	sv = insert_long(sv, LBS_SV_FR_DISC_INT, 0);
	sv = insert_longer(sv, LBS_SV_BY_DISC_INT, 0);
	sv = insert_long(sv, LBS_SV_FR_NOT_ROUTED, 0);
#endif
    }

    if (which & LBS_PI_SET2) {
	sv = insert_short(sv, LBS_SV_T_FR_DISC, 0);	/* 'short' version */
	sv = insert_long(sv, LBS_SV_T_FR_DISC, 0);	/* 'long'  version */

	sv = insert_long(sv, LBS_SV_T_B_FR_TRANS, 0);
	sv = insert_long(sv, LBS_SV_T_NB_FR_TRANS, 0);

	sv = insert_short(sv, LBS_SV_T_FR_NOT_REC, 0);	/* 'short' version */
	sv = insert_long(sv, LBS_SV_T_FR_NOT_REC, 0);	/* 'long'  version */
	sv = insert_short(sv, LBS_SV_T_FR_NOT_FORW, 0);	/* 'short' version */
	sv = insert_long(sv, LBS_SV_T_FR_NOT_FORW, 0);	/* 'long'  version */

	sv = insert_long(sv, LBS_SV_T_FR_DISC_INT, 0);
	sv = insert_long(sv, LBS_SV_T_FR_NOT_ROUTED, 0);
	sv = insert_longer(sv, LBS_SV_T_B_BY_TRANS, 0);
	sv = insert_longer(sv, LBS_SV_T_NB_BY_TRANS, 0);
    }
    if (which & LBS_PI_SRTE_ENA) {
	sv = insert_byte(sv, LBS_SV_SRTE_ENABLED, 
                         srb_sb->srb_spanning_explorer);
    }
    
    vec = finish_subvector(vec, sv);
    return(vec);
}

/*
 * insert_lbs_rte
 *
 * Add "route" specific information to the current vector.  A route is a
 * pair of interfaces that are being bridged to each other.  The 'which'
 * argument is a bitfield that controls what information is actually
 * included by this routine.
 */
static ibmnm_vector *insert_lbs_rte (
    ibmnm_vector *vec,
    bridge_info *bridge,
    int which)
{
    ibmnm_vector *sv, *sv2;
    int tr_max_len, i;
    spantype *span;
    idbtype *port;
    uchar ast_status = FALSE;
    
    sv = insert_header(vec, LBS_SV_ROUTE_STATUS);
    sv2 = insert_header(sv, LBS_SV_ROUTE_IDENT);
    if (bridge->idb1->status & IDB_FDDI)
	sv2 = insert_macaddr(sv2, LBS_SV_PORT_IDENT1, bridge->idb1->bit_swapped_hardware);
    else
	sv2 = insert_macaddr(sv2, LBS_SV_PORT_IDENT1, bridge->idb1->hardware);
    if (bridge->idb2->status & IDB_FDDI)
	sv2 = insert_macaddr(sv2, LBS_SV_PORT_IDENT2, bridge->idb2->bit_swapped_hardware);
    else
	sv2 = insert_macaddr(sv2, LBS_SV_PORT_IDENT2, bridge->idb2->hardware);
    sv = finish_subvector(sv, sv2);
    
    if (which & LBS_B_BR_IDENT)
	sv = insert_short(sv, LBS_SV_BR_IDENT, 
                          srb_sb_get_bridge_num(bridge->idb1->firstsw));
    if (which & LBS_B_LARGEST_FR) {
	if ((bridge->idb1->status & IDB_FDDI) ||
	    (bridge->idb2->status & IDB_FDDI)) {
	    /*
	     * When FDDI bridge ports are present
	     * ensure that largest frame size
	     * restrictions are satisfied
	     */
	    tr_max_len = RC_TR_LFDEF;
        }
	else {
	    if ((bridge->idb1->tr_ring_speed == 16) &&
		(bridge->idb2->tr_ring_speed == 16))
		  tr_max_len = RC_TR16_LFDEF;
	    else 
		tr_max_len = RC_TR_LFDEF;
	}
	sv = insert_byte(sv, LBS_SV_LARGEST_FR, tr_max_len >> 4);
    }
    if (which & LBS_B_RTE_ACTIVE)
	sv = insert_byte(sv, LBS_SV_RTE_ACTIVE, FALSE);
    if (which & LBS_B_SRTE_MODE) {
	/* SRB AST support:
	 * Check the spanning array for this bridge group
	 * & determine if AST is enabled for a spanning port
	 * that is associated with a bridge idb structure
	 */
	span = spanarray[srb_sb_get_bridge_num(bridge->idb1->firstsw)];
        if ((span) && (span->running)) {
	    for (i = 0; i < span->port_count; i++) {
		port = span->port[i];
		if (port->span_ptr && (port->span_ptr)->running) {
		    if ((bridge->idb1 == port->hwptr) ||
			(bridge->idb2 == port->hwptr)) {
			  ast_status = TRUE;
			  break;
		    }
                }
            }
        }
        sv = insert_byte(sv, LBS_SV_SRTE_MODE, ast_status);
    }
    vec = finish_subvector(vec, sv);
    return(vec);
}

/**********************************************************************
 *
 *			  Protocol Responses
 *
 **********************************************************************/

/*
 * lbs_error_frame (8505)
 *
 * Send a LBS Error Frame.  This message indicates that a command
 * received from the Lan Network Manager contained a syntax error, or the
 * command could not be completed sucessfully.
 */
static void lbs_error_frame (
    bridge_info *bridge,
    int link,
    int correlator,
    int code,
    int offset)
{
    paktype *pak;
    ibmnm_vector *sv;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, LBS_ERROR);
    sv = insert_correlator(sv, correlator);
    sv = insert_short(sv, IBMNM_ERRCODE, code);
    sv = insert_short(sv, IBMNM_ERROFFSET, offset);
    sv = insert_bytes(sv, IBMNM_VERSION, lbs_version, VERSIONLEN);
    finish_vector(pak, sv);

    lrm_send_link(pak, bridge, link, NAME(LBS_ERROR));
}

/*
 * The following routine handle the LBS status reporting.  A Lan Network
 * Manager sends us a Request LBS Status frame.  The response to that
 * message is either a Report LBS Status frame or an Error frame.
 *
 * LBS Request Status (I) -->
 * a) <-- LBS Report Status (I)
 * b) <-- LBS Error (I)
 */

/*
 * lbs_report_status (8502)
 *
 * Send a Report Status Message back to the requesting Lan Network
 * Manager.
 */
static void lbs_report_status (
    bridge_info *bridge,
    int link,
    int correlator,
    boolean all,
    boolean long_counters)
{
    paktype *pak;
    ibmnm_vector *sv;
    int which;

    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;

    sv = insert_major(pak, LBS_REP_STATUS);
    sv = insert_correlator(sv, correlator);
    sv = insert_short(sv, LBS_SV_TYPE, (BRIDGE_TR_TR | BRIDGE_LOCAL));
    sv = insert_bytes(sv, IBMNM_VERSION, lbs_version, VERSIONLEN);
    sv = insert_short(sv, LBS_SV_PORTS, 2);
    sv = insert_byte(sv, LBS_SV_PART_BITS, 4);
    sv = insert_byte(sv, LBS_SV_PATH_TRACE, TRUE);
    sv = insert_short(sv, LBS_SV_LOSS_THRESH, bridge->frame_lost_thresh);
    sv = insert_short(sv, LBS_SV_CALC_INT, bridge->calculation_interval);
    sv = insert_short(sv, LBS_SV_NOTIF_INTERVAL,bridge->notification_interval/ONESEC);

    if (all)
	which = (long_counters) ? LBS_PI_REP_STATUS_AL : LBS_PI_REP_STATUS_A;
    else
	which = LBS_PI_REP_STATUS;

    sv = insert_lbs_pi(sv, bridge->idb1, which);
    sv = insert_lbs_pi(sv, bridge->idb2, which);
    sv = insert_lbs_rte(sv, bridge, LBS_RTE_REP_STATUS);
    sv = insert_short(sv, LBS_SV_INTERNAL_STATUS, 0);
    finish_vector(pak, sv);

    lrm_send_link(pak, bridge, link, NAME(LBS_REP_STATUS));
}

/*
 * The following routines handle the Lan Bridge Server "Set Parameters"
 * frame.  A Lan Network Manager sends us a Set LBS Parameters frame.
 * The response to that message is either a Report LBS Status frame or an
 * Error frame.  If anything is changed, a "Parameters Changed" message
 * is also sent to all other Lan Network Managers present.
 *
 * Set LBS Parameters (I) -->
 * a) <-- LBS Parameters Set (I)
 *        LBS Parameters Changed (I)  -->
 * b) <-- LBS Error (I)
 */

/*
 * lbs_parameters_set (8504)
 *
 * Send a Parameters Set Message back to the requesting Lan Network
 * Manager.
 */
static void lbs_parameters_set (bridge_info *bridge, int link,
				ushort correlator)
{
    paktype *pak;
    ibmnm_vector *sv;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, LBS_PARAM_SET);
    sv = insert_correlator(sv, correlator);
    finish_vector(pak, sv);

    lrm_send_link(pak, bridge, link, NAME(LBS_PARAM_SET));
}

/*
 * lbs_parameters_changed (8506)
 *
 * Send a Parameters Changed Notification Message to all Lan Network
 * Managers other than the controlling LNM.
 */
static void lbs_param_changed (bridge_info *bridge, int link, lbs_pb *result)
{
    paktype *pak;
    ibmnm_vector *sv;
    int i, which;
    hwidbtype *idb;

    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;

    sv = insert_major(pak, LBS_PARAM_CHANGED);
    if (result->thresh)
	sv = insert_short(sv, LBS_SV_NOTIF_INTERVAL, 0);
    if (result->thresh)
	sv = insert_short(sv, LBS_SV_LOSS_THRESH, 0);

    if (result->num_ports) {
	for(i = 0; i < result->num_ports; i++) {
	    idb = valid_port(bridge, result->pt[i].port_id);
	    which = 0;
	    if (result->pt[i].hopcount)
		which |= LBS_PI_HOP_COUNT;
	    if (result->pt[i].srte_enabled)
		which |= LBS_PI_SRTE_ENA;
	    sv = insert_lbs_pi(sv, bridge->idb1, which);
	}
    }
    if (result->num_routes) {
	for (i = 0; i < result->num_routes; i++) {
	    which = 0;
	    if (result->rte[i].bridge_id)
		which |= LBS_B_BR_IDENT;
	    if (result->rte[i].route_active)
		which |= LBS_B_RTE_ACTIVE;
	    if (result->rte[i].srte_mode)
		which |= LBS_B_SRTE_MODE;
	    sv = insert_lbs_rte(sv, bridge, which);
	}
    }
    if (result->in_stats)
	sv = insert_short(sv, LBS_SV_INTERNAL_STATUS, 0);
    finish_vector(pak, sv);
 
    lrm_send_all_but(pak, bridge, link, NAME(LBS_PARAM_CHANGED));
}

/**********************************************************************
 *
 *		    Upcalls, sidecalls, whatever.
 *
 **********************************************************************/

#ifdef NOBODY_CALLS_ME
/*
 * lbs_thresh_exceeded (8507)
 *
 * Send a Performace Threshold Exceeded Message to all Lan Network
 * Managers.  This message will include all information used by the
 * bridge to compue the frames lost threshold.
 */
void lbs_thresh_exceeded (bridge_info *bridge)
{
    paktype *pak;
    ibmnm_vector *sv;

    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;

    sv = insert_major(pak, LBS_PERF_THRESH);
    sv = insert_short(sv, LBS_SV_LOSS_THRESH, bridge->frame_lost_thresh);
    sv = insert_short(sv, LBS_SV_PERC_FR_LOST, 0);
    sv = insert_lbs_pi(sv, bridge->idb1, LBS_PI_PERF_THRESH);
    finish_vector(pak, sv);
	 
    lrm_send_ring(pak, bridge->idb1, NAME(LBS_PERF_THRESH));
}
#endif /* NOBODY_CALLS_ME */

/*
 * lbs_counter_report (8509)
 *
 * Send a Bridge Counter Report Message to all Lan Network Managers.
 * This message will include all current counter values in the bridge.
 */
void lbs_counter_report (bridge_info *bridge)
{
    paktype *pak;
    ibmnm_vector *vector, *sv;

    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    sv = insert_major(pak, LBS_COUNTER_REP);
    sv = insert_lbs_pi(sv, bridge->idb1, LBS_PI_COUNTER_REP);
    sv = insert_lbs_pi(sv, bridge->idb2, LBS_PI_COUNTER_REP);
    vector->length = (uchar *)sv - (uchar *)vector;

    lrm_send(pak, bridge, LRM_ALL, NAME(LBS_COUNTER_REP));
}

/*
 * lbs_srte_bcast (850A)
 *
 * Send a Single Route Broadcast Status Change Message to all lan
 * managers.  This routine is invoked whenever the spanning tree state of
 * this interface changes.  This could either be a user configuring the
 * system, or the spanning tree reconfiguring.
 */
void lbs_srte_bcast (hwidbtype *idb)
{
    paktype *pak;
    ibmnm_vector *sv;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, LBS_SPAN_STATUS);
    sv = insert_lbs_pi(sv, idb, LBS_PI_SPAN_STATUS);
    finish_vector(pak, sv);
 
    lrm_send_ring(pak, idb, NAME(LBS_SPAN_STATUS));
}

/**********************************************************************
 *
 *		      Support for path tracing.
 *
 **********************************************************************/

/*
 * lbs_build_trace_report (8508)
 *
 * Build a Path Trace Report Message that will eventually be sent to a
 * Lan Network Manager.  This routine lies about the frame status byte of
 * the transmitted frame.  THe system currently has no way to get the
 * frame status value, so it inserts the value indicating a sucessful
 * transmission that was received by the destination.
 */
static paktype *lbs_build_trace_report (
    paktype *tracepak,
    bridge_info *bridge,
    station_info *target)
{
    tring_hdr *trh;
    paktype *pak;
    ibmnm_vector *sv;

    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) 
        return(NULL);

    trh = (tring_hdr *)tracepak->datagramstart;
    sv = insert_major(pak, LBS_TRACE_REP);
    sv = insert_bytes(sv, LBS_SV_FORW_ADDR, trh->daddr, 12);
    sv = insert_short(sv, LBS_SV_FORW_LEN, tracepak->datagramsize);
    sv = insert_bytes(sv, LBS_SV_FORW_DATA, trh->data, 14);
    sv = insert_short(sv, LBS_SV_FORW_STRIP, target ? 0x01CC : 0x0100);
    sv = insert_short(sv, LBS_SV_PART_BITS, 4);
    sv = insert_lbs_pi(sv, bridge->idb1, LBS_PI_TRACE_REP);
    sv = insert_lbs_pi(sv, bridge->idb2, LBS_PI_TRACE_REP);
    sv = insert_lbs_rte(sv, bridge, LBS_RTE_TRACE_REP);
    finish_vector(pak, sv);
    return(pak);
}

/*
 * send_trace_report
 *
 * This packet is being traced by a Lan Network Manager.  Report the fact
 * that we have forwarded this packet.  Reports can either be sent to Lan
 * Network Manager over the existing LLC2 connections, or they may be
 * sent to the source of the frame as a Unnumbered Information frame.
 *
 * In an ideal world, this routine would be called after the frame had
 * been both transmited onto and stripped from the ring.  This would
 * provide the frame status value for the frame that should be included
 * as part of the report.  Instead, we perform our normal hackery.  We
 * check our internal tables to see whether or not the station is
 * currently present on the ring, and generate the appropriate frame
 * status value.
 */
void send_trace_report (
    paktype *tracepak,
    hwidbtype *idb,
    hwidbtype *outidb)
{
    station_info *target;
    trrif_t *trh;
    bridge_info *bridge;
    paktype *pak, *pak2;
    ibmnm_vector *vector;
    int length;
    uchar *temp;

    if (!tracepak || !lanmgr_global_enable || (!idb && !outidb) ||
	((!lanmgr_pathtrace_enable_lnm) && (!lanmgr_pathtrace_enable_orig)))
	return;

    trh = (trrif_t *)tracepak->datagramstart;
    temp = trh->daddr;
    if (outidb) {
	/*
	 * Onto a real ring
	 */
	bridge = idb_to_bridge(outidb);
	target = find_station(trh->daddr, outidb);
    } else {
	/*
	 * Onto a virtual ring.  Fake out the build routine to produce a
	 * good FS value.
	 */
	bridge = idb_to_bridge(idb);
	target = find_station(trh->saddr, idb);
    }
    if (!bridge)
	return;

    pak = lbs_build_trace_report(tracepak, bridge, target);
    if (!pak)
	return;

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);
    length = vector->length;
    pak->acct_proto = ACCT_PROTO_LANMAN;
    pak->linktype = LINK_IBMNM;
    pak->if_output = (outidb ? outidb->firstsw : idb->firstsw);

    switch (trh->rc_blth & RC_TRACE_FRAME) {
      case RC_TRACE_LM:
        if (lanmgr_pathtrace_enable_lnm) {      
            lrm_send_ring(pak, outidb ? outidb : idb, NAME(LBS_TRACE_REP));
        }
	else {
	    retbuffer(pak);
	}
	break;
      case RC_TRACE_ORIG:
	if ((((idb && (idb->state == IDBS_UP) &&
	    pak_encap(idb, pak, length, (long)trh->saddr))))
            && (lanmgr_pathtrace_enable_orig)) {
	    lnmllcbuginf("\nIBMNM: sending %s to origin %e.",
		     NAME(LBS_TRACE_REP), trh->saddr);
            trh = (trrif_t *)pak->datagramstart;
	    ieee_copy(temp, trh->saddr);
	    datagram_out(pak);
	} else {
	    retbuffer(pak);
	}
	break;
      case RC_TRACE_BOTH:
	if (!(pak2 = pak_duplicate(pak))) {
	    retbuffer(pak);
	    return;
	}
	if (lanmgr_pathtrace_enable_lnm) {
	    lrm_send_ring(pak, outidb ? outidb : idb, "lbs path trace report");
        }
	if ((((idb && (idb->state == IDBS_UP) &&
	    pak_encap(idb, pak2, length, (long)trh->saddr))))
            && (lanmgr_pathtrace_enable_orig)) {
	    lnmllcbuginf("\nIBMNM: sending %s to origin %e.",
		     NAME(LBS_TRACE_REP), trh->saddr);
            trh = (trrif_t *)pak2->datagramstart;
	    ieee_copy(temp, trh->saddr);
            pak2->if_output = (outidb ? outidb->firstsw : idb->firstsw);
	    datagram_out(pak2);
	} else {
	    retbuffer(pak2);
	}
	break;
      default:
        retbuffer((paktype *)pak); 
	break;
    }
}

/**********************************************************************
 *
 *			  Workhorse routines
 *
 **********************************************************************/

/*
 * do_set_lbs_parameters
 *
 * The router has been asked by Lan Network Manager to change one or more
 * of its iternal parameters.  All requests seen so far only change one
 * parameter, but it is safer not to make that assumption.  Check to see
 * which parameters should be changed, and change them.
 *
 * Currently, there are three parameters cannot be changed with this
 * command.  They are:
 *
 *   1) The internal status vector.  The documentation states that this
 *      vector is implementation dependant.  For the IBM Bridge Program
 *      v2.1 this is a short.
 *
 *   2) The route active status.  Turning off the route active status
 *      implies that all bridging functions are disabled, but the
 *      interface remains active for monitoring.  No cisco token ring
 *      interface currently provides the ability to disable bridging
 *      functions without a complete reset of the interface.
 *
 *   3) The single route broadcast mode.  This is the switch that changes
 *      between manual spanning tree configuration, and automatic
 *      spanning tree configuration.  Cisco does now (release 9.0)
 *      support the spanning tree protocol on token rings, but only when
 *      running SRT.  There is currently no provision for running
 *      automatic spanning tree with SRB only.
 */
static boolean do_set_lbs_parameters (
    bridge_info *bridge,
    lbs_pb *result,
    int *error)
{
    hwidbtype *idb;
    idbtype   *swidb;
    ulong spanning;
    int i;

    if (result->interval) {
	bridge->notification_interval = *result->interval * ONESEC;
	if (bridge->notification_interval) {
	    TIMER_START(bridge->notification_time, bridge->notification_interval);
	    set_lanmgr_timer(bridge->notification_time);
	} else
	    TIMER_STOP(bridge->notification_time);
    }
    if (result->thresh) {
	bridge->frame_lost_thresh = *result->thresh;
    }
    if (result->in_stats) {
	ulong *p;
	
	p = (ulong *)result->in_stats;
	buginf("\nIBMNM: internal status vector: %08x %08x %08x %08x", p[0], p[1], p[2], p[3]);
    }

    for (i = 0; i < result->num_ports; i++) {
	idb = valid_port(bridge, result->pt[i].port_id);
	if (!idb) {
	    continue;
        }
        swidb = firstsw_or_null(idb);
	if (idb->status & IDB_VIRTUAL) {
	    lnmllcbuginf("\nIBMNM: modifying vring parameters is not allowed.");
	    *error = ERR_EXECUTION;
	    return(FALSE);
	}
	if (result->pt[i].hopcount) {
	    if (((bridge->idb1->status & IDB_VIRTUAL) ||
		 (bridge->idb2->status & IDB_VIRTUAL)) &&
		(*result->pt[i].hopcount == 1)) {
		*error = SET_ERR_TOO_LOW;
		return(FALSE);
	    }
            srb_sb_set_max_hops(swidb, *result->pt[i].hopcount);
	}
	if (result->pt[i].srte_enabled) {
	    spanning = *result->pt[i].srte_enabled;
	    /* SRB AST support */
	    if (spanning) {
		srb_sb_enable_spanning_explorer(swidb);
	    } else {
		srb_sb_disable_spanning_explorer(swidb);
	    }
	    srbcore_modify_srb(idb, TR_MODIFY_SPAN, spanning);
	}
    }
    
    for (i = 0; i < result->num_routes; i++) {
	if (!ieee_equal_sans_rii(result->rte[i].port_id1, bridge->idb1->hardware) ||
	    !ieee_equal_sans_rii(result->rte[i].port_id2, bridge->idb2->hardware)) {
	        lnmllcbuginf("\nIBMNM: invalid route %e <-> %e",
			 result->rte[i].port_id1, result->rte[i].port_id2);
		continue;
	    }
	/*
	 * Changing bridge number.
	 */
	if (result->rte[i].bridge_id) {
            idbtype *swidb1 = firstsw_or_null(bridge->idb1);
            idbtype *swidb2 = firstsw_or_null(bridge->idb2);
	    srb_sb_set_bridge_num(swidb1, *result->rte[i].bridge_id);
	    srb_sb_set_bridge_num(swidb2, *result->rte[i].bridge_id);
	    srbcore_modify_srb(bridge->idb1, TR_MODIFY_BNUM, 
					*result->rte[i].bridge_id);
	    srbcore_modify_srb(bridge->idb2, TR_MODIFY_BNUM, 
					*result->rte[i].bridge_id);
	    do_bridge_number_setup(bridge->idb1, bridge->idb2);
	}
	if (result->rte[i].route_active && *(result->rte[i].route_active)) {
	    lnmllcbuginf("\nIBMNM: setting route active status is not supported.");
	    /*
	     * This implies that we turn off forwarding on the bridge.
	     * We currently do not have any way to do this without
	     * reconfiguring.  Reconfiguring causes the 'bridge' data
	     * structures to go away, and we close the connection to Lan
	     * Manager.  Fail it until something better arises.
	     */
	    *error = SET_ERR_UNAUTH_REQ;
	    return(FALSE);
	}
	/* SRB AST support */
	if (result->rte[i].srte_mode) {
	    if (!lnm_srb_span_command(bridge->idb1, bridge->idb2,
				      (boolean)(*result->rte[i].srte_mode))) {
		*error = ERR_EXECUTION;
		return(FALSE);
	    }
	}
    }
    
/* The following write NVRAM causes LNM to time out, therefore it is */
/* deleted */
/*
 *  if (!result->temp || (*result->temp == PERMANENT))
 *     nv_current_to_nv(FALSE, PRIV_ROOT);
*/
    return(TRUE);
}

/**********************************************************************
 *
 *			   Parsing Routines
 * 
 **********************************************************************/

/*
 * lbs_parse_rte_ident
 *
 * Parse a route identifier.  This will uniquely identify the
 * ring-bridge-ring combination that the Lan Network Manager is querying
 * for information.  This seems redundant since the IBM definition of a
 * bridge is two token rings connected together (i.e. one route).
 */
static boolean lbs_parse_rte_ident (
    ibmnm_vector *vector,
    ulong required,
    ulong optional,
    lbs_pb *r,
    int *error,
    ibmnm_vector **offset)
{
    ulong bit, present, missing;
    ibmnm_vector *sv;
    int length, i;
    uchar *data;

    present = 0;
    i = r->num_routes;
    length = GETSHORT(&vector->length) - IBMNM_MVID_LEN;
    sv = (ibmnm_vector *)&vector->data;

    for ( ; length > 0; length -= GETSHORT(&sv->length), sv = next_subvector(sv)) {
	bit = lbsid_to_bitmask(GETSHORT(&sv->command));
	present |= bit;

	/*
	 * Is it allowed?
	 */
	if ((((required | optional) & LBS_B_ROUTEID_MASK) & bit) == 0) {
	    lnmllcbuginf(unxpctvec, lbsbit_to_string(bit));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}

	/*
	 * Record it.
	 */
	data = (uchar *)&sv->data;
	switch (GETSHORT(&sv->command)) {
	  case LBS_SV_PORT_IDENT1: r->rte[i].port_id1 = (uchar *)data; break;
	  case LBS_SV_PORT_IDENT2: r->rte[i].port_id2 = (uchar *)data; break;
	  default:
	    /* Shouldn't ever happen.  Should be caught above. */
	    lnmllcbuginf(unknvec, GETSHORT(&sv->command));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}
    }

    /*
     * Are any required vectors missing?
     */
    missing = (required & LBS_B_ROUTEID_MASK) & ~present;
    if (missing) {
	lnmllcbuginf(missvec, lbsbit_to_string(missing));
	*error = ERR_REQ_SV_MISSING;
	*offset = sv;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * lbs_parse_rte_status
 *
 * Parse a "route status".  These are the items associated with a route
 * that may be changed.  They include whether or not the route is active
 * (i.e. forwarding), whether the bridge uses automatic or manual
 * spanning tree, and the bridge number.  The ring numbers cannot be
 * changed from the LBS.  They must be changed by a request to the LRM.
 */
static boolean lbs_parse_rte_status (
   ibmnm_vector *vector,
    ulong required,
    ulong optional,
    lbs_pb *r,
    int *error,
    ibmnm_vector **offset)
{
    ulong bit, present, missing;
    ibmnm_vector *sv;
    int length, i;
    uchar *data;

    present = 0;
    i = r->num_routes;
    length = GETSHORT(&vector->length) - IBMNM_MVID_LEN;
    sv = (ibmnm_vector *)&vector->data;

    for ( ; length > 0; length -= GETSHORT(&sv->length), sv = next_subvector(sv)) {
	bit = lbsid_to_bitmask(GETSHORT(&sv->command));
	present |= bit;

	/*
	 * Is it allowed?
	 */
	if ((((required | optional) & LBS_B_ROUTE_MASK) & bit) == 0) {
	    lnmllcbuginf(unxpctvec, lbsbit_to_string(bit));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}

	/*
	 * Record it.
	 */
	data = (uchar *)&sv->data;
	switch (GETSHORT(&sv->command)) {
	  case LBS_SV_ROUTE_IDENT:
	    lbs_parse_rte_ident(sv, required, optional, r, error, offset);
	    if (*error) return(FALSE);
	    r->num_routes++;
	    break;

	  case LBS_SV_BR_IDENT:   r->rte[i].bridge_id    =(ushort *)data;break;
	  case LBS_SV_RTE_ACTIVE: r->rte[i].route_active =(uchar *)data; break;
	  case LBS_SV_SRTE_MODE:  r->rte[i].srte_mode    =(uchar *)data; break;

	  default:
	    /* Shouldn't ever happen.  Should be caught above. */
	    lnmllcbuginf(unknvec, GETSHORT(&sv->command));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}
    }

    /*
     * Are any required vectors missing?
     */
    missing = (required & LBS_B_ROUTE_MASK) & ~present;
    if (missing) {
	lnmllcbuginf(missvec, lbsbit_to_string(missing));
	*error = ERR_REQ_SV_MISSING;
	*offset = sv;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * lbs_parse_port
 *
 * Parse information associated with a particular port (interface) on a
 * bridge.  These include the address, hop count, and spanning tree
 * state.
 */
static boolean lbs_parse_port (
    ibmnm_vector *vector,
    ulong required,
    ulong optional,
    lbs_pb *r,
    int *error,
    ibmnm_vector **offset)
{
    ulong bit, present, missing;
    ibmnm_vector *sv;
    int length, i;
    uchar *data;

    present = 0;
    i = r->num_ports;
    length = GETSHORT(&vector->length) - IBMNM_MVID_LEN;
    sv = (ibmnm_vector *)&vector->data;

    for ( ; length > 0; length -= GETSHORT(&sv->length), sv = next_subvector(sv)) {
	bit = lbsid_to_bitmask(GETSHORT(&sv->command));
	present |= bit;

	/*
	 * Is it allowed?
	 */
	if ((((required | optional) & LBS_B_PORT_MASK) & bit) == 0) {
	    lnmllcbuginf(unxpctvec, lbsbit_to_string(bit));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}

	/*
	 * Record it.
	 */
	data = (uchar *)&sv->data;
	switch (GETSHORT(&sv->command)) {
	  case IBMNM_ADDRESS:       r->pt[i].port_id      =(uchar *)data;break;
	  case LBS_SV_SRTE_ENABLED: r->pt[i].srte_enabled =(uchar *)data;break;
	  case LBS_SV_HOP_COUNT:    r->pt[i].hopcount     =(uchar *)data;break;
	  default:
	    /* Shouldn't ever happen.  Should be caught above. */
	    lnmllcbuginf(unknvec, GETSHORT(&sv->command));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}
    }

    /*
     * Are any required vectors missing?
     */
    missing = (required & LBS_B_PORT_MASK) & ~present;
    if (missing) {
	lnmllcbuginf(missvec, lbsbit_to_string(missing));
	*error = ERR_REQ_SV_MISSING;
	*offset = sv;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * lbs_parse_vectors
 *
 * Parse the subvectors in a major vector sent to the LBS.  This routine
 * checks to insure that all required vectors are present.  It also
 * insures that the major vector contains only the required and optional
 * subvectors that are permissible for this particular major vector.
 * This routine builds and returns a data structure that indicates which
 * subvectors were present, and what their values were.
 */
static boolean lbs_parse_vectors (
    ibmnm_vector *vector,
    ulong required,
    ulong optional,
    lbs_pb *result,
    int *error,
    ibmnm_vector **offset)
{
    ulong bit, present, missing;
    ibmnm_vector *sv;
    uchar *data;
    int length;

    present = 0;
    length = GETSHORT(&vector->length) - IBMNM_MVID_LEN;
    sv = (ibmnm_vector *)&vector->data;

    for ( ; length > 0; length -= GETSHORT(&sv->length), sv = next_subvector(sv)) {
	bit = lbsid_to_bitmask(GETSHORT(&sv->command));
	present |= bit;

	/*
	 * Is it allowed?
	 */
	if ((((required | optional) & LBS_B_TOP_MASK) & bit) == 0) {
	    lnmllcbuginf(unxpctvec, lbsbit_to_string(bit));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}

	/*
	 * Record it.
	 */
	data = (uchar *)&sv->data;
	switch (GETSHORT(&sv->command)) {
	  case IBMNM_CORRELATOR: result->correlator = (ushort *)data; break; 
	  case IBMNM_ADDRESS:    result->address    = (uchar *)data;  break; 
	  case IBMNM_RING:       result->ring       = (ushort *)data; break; 
	  case IBMNM_ALL:        result->all        = (ushort *)data; break;
	  case IBMNM_PERMANENT:  result->temp       = (uchar *)data;  break;

	  case LBS_SV_ALL_LONG:        result->all_long = (ushort *)data;break;
	  case LBS_SV_NOTIF_INTERVAL:  result->interval = (ushort *)data;break;
	  case LBS_SV_LOSS_THRESH:     result->thresh   = (ushort *)data;break;
	  case LBS_SV_INTERNAL_STATUS: result->in_stats = (ushort *)data;break;

	  case LBS_SV_PORT_INFO:
	    lbs_parse_port(sv, required, optional, result, error, offset);
	    if (*error) return(FALSE);
	    result->num_ports++;
	    break;

	  case LBS_SV_ROUTE_STATUS:
	    lbs_parse_rte_status(sv, required, optional, result, error, offset);
	    if (*error) return(FALSE);
	    break;

	  default:
	    /* Shouldn't ever happen.  Should be caught above. */
	    lnmllcbuginf(unknvec, GETSHORT(&sv->command));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}
    }

    /*
     * Are any required vectors missing?
     */
    missing = (required & LBS_B_TOP_MASK) & ~present;
    if (missing) {
	lnmllcbuginf(missvec, lbsbit_to_string(missing));
	*error = ERR_REQ_SV_MISSING;
	*offset = sv;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * ibmnm_lbs_input
 *
 * All input frames that are directed to the LAN Bridge Server ends up
 * here.  Parse them, query the ring for information if necessary, and
 * then return the requested information to the Lan Network Manager.
 */
void ibmnm_lbs_input (llctype *cookie, ibmnm_vector *vector)
{
    bridge_info *bridge;
    int link, error;
    ushort correlator;
    ibmnm_vector *offset;
    lbs_pb *result;
    boolean all;
    srb_triplet_t srb_triplet;

    /*
     * Housekeeping
     */
    if (!cookie_to_link(cookie, &bridge, &link)) {
	buginf(nolink, cookie);
	return;
    }
    srb_sb_get_triplet(bridge->idb1->firstsw, &srb_triplet);
    ibmnm_print_bridge_rcvd_msg(&srb_triplet,
				lbs_to_string(GETSHORT(&vector->command)),
				bridge, link);
    if (!(result = malloc_named(sizeof(lbs_pb), lnm_cb)))
	return;
    error = 0;

    switch (GETSHORT(&vector->command)) {
      case LBS_REQ_STATUS:
	if (!ibmnm_parse_request_status(vector, &correlator, &error, &offset)) {
	    lbs_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	if (!lbs_parse_vectors(vector, LBS_R_REQ_STATUS, LBS_O_REQ_STATUS,
			       result, &error, &offset)) {
	    lbs_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	all = result->all || result->all_long;
	if (!all) {
	    lbs_error_frame(bridge, link, correlator, ERR_REQ_SV_MISSING, GETSHORT(&vector->length));
	    break;
	}
	lbs_report_status(bridge, link, correlator, all, (boolean)result->all_long);
	break;

      case LBS_SET_PARAM:
	if (!ibmnm_parse_set_request(vector, &correlator, &error, &offset)) {
	    lrm_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	if (!ibmnm_changes_allowed(bridge, link)) {
	    lrm_error_frame(bridge, link, correlator, SET_ERR_UNAUTH_REQ, GETSHORT(&vector->length));
	    lrm_control_breach(bridge, link);
	    break;
	}
	if (!lbs_parse_vectors(vector, LBS_R_SET_PARAM, LBS_O_SET_PARAM,
			       result, &error, &offset)) {
	    lbs_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	if (!do_set_lbs_parameters(bridge, result, &error)) {
	    lbs_error_frame(bridge, link, correlator, error, GETSHORT(&vector->length));
	    break;
	}
	lbs_parameters_set(bridge, link, correlator);
	lbs_param_changed(bridge, link, result);

	break;

      default:
	buginf(respvec, GETSHORT(&vector->command));
	lrm_parsing_error(bridge, link, GETSHORT(&vector->command), LRM_PARSE_UNKNOWN);
	break;
    }
    free(result);
}

#ifdef IBMNM_DEBUG
/**********************************************************************
 *
 *		     Lan Bridge Server Test Code
 *
 * All code past here implements the Lan Network Manager side of this
 * protocol.
 *
 **********************************************************************/
#include "../llc2/llc2.h"
#include "ttysrv.h"

/*
 * Build and send a "Request LBS Status" message.  This should elicit a
 * "Report LBS Status" response.
 */
static void tibmnm_req_lbs_status (
    jmp_buf *far)
{
    jmp_buf near;
    paktype *pak;
    ibmnm_vector *vector, *sv;
    int all;

    /*
     * Build a packet.
     */
    pak = ibmnm_getbuffer(100);
    if (!pak) {
	printf("Cannot get a packet buffer");
	longjmp(far, 1);
    }

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    /*
     * The abort code.
     */
    if (setjmp(&near)) {
	if (pak)
	    retbuffer(pak);
	longjmp(far, 1);
    }

    sv = insert_major(pak, LBS_REQ_STATUS);
    if (yes_or_no2(&near, "Use Correlator", TRUE, TRUE))
	sv = insert_correlator(sv, test_correlator++);
    if (yes_or_no2(&near, "Include 'All'", TRUE, TRUE)) {
	all = get_num_in_range2(&near, "    All", 16, 0, 65535, 0);
	sv = insert_short(sv, IBMNM_ALL, all);
    }
    if (yes_or_no2(&near, "Include 'All (long)'", TRUE, TRUE)) {
	all = get_num_in_range2(&near, "    All", 16, 0, 65535, 0);
	sv = insert_short(sv, LBS_SV_ALL_LONG, all);
    }
   
    sv = tibmnm_add_arbitrary(&near, sv);
    vector->length = (uchar *)sv - (uchar *)vector;
    ibmnm_llc2_send(test_cookie, pak, vector->length);
}

/*
 * Build and send a "Set LBS Parameters" message.  This should elicit a
 * "LBS Parameters Set" response.
 */
static void tibmnm_set_lbs_parms (
    jmp_buf *far)
{
    jmp_buf near;
    paktype *pak;
    ibmnm_vector *vector, *sv, *sv2, *sv3;
    int val;
    uchar address[IEEEBYTES];

    /*
     * Build a packet.
     */
    pak = ibmnm_getbuffer(100);
    if (!pak) {
	printf("Cannot get a packet buffer");
	longjmp(far, 1);
    }

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    /*
     * The abort code.
     */
    if (setjmp(&near)) {
	if (pak)
	    retbuffer(pak);
	longjmp(far, 1);
    }

    sv = insert_major(pak, LBS_SET_PARAM);
    if (yes_or_no2(&near, "Use Correlator", TRUE, TRUE))
	sv = insert_correlator(sv, test_correlator++);

    if (yes_or_no2(&near, "Use Notification Interval", FALSE, TRUE)) {
	val = get_num_in_range2(&near, "    Notification Interval (sec)", 16, 0, 65535, 0);
	sv = insert_short(sv, LBS_SV_NOTIF_INTERVAL, val);
    }
    if (yes_or_no2(&near, "Use Frame Lost Thresh", FALSE, TRUE)) {
	val = get_num_in_range2(&near, "    Frame Lost Thresh (pct)", 16, 0, 65535, 0);
	sv = insert_short(sv, LBS_SV_LOSS_THRESH, val);
    }

    if (yes_or_no2(&near, "Use Port Info", TRUE, TRUE)) {
	sv2 = insert_header(sv, LBS_SV_PORT_INFO);
	if (yes_or_no2(&near, "Use Port", TRUE, TRUE)) {
	    get_macaddr(&near, "    Port", address, "0000.3000.5735");
	    sv2 = insert_address(sv2, address);
	}
	if (yes_or_no2(&near, "Use Hop Count", TRUE, TRUE)) {
	    val = get_num_in_range2(&near, "    Hop Count", 16, 0, 255, 7);
	    sv2 = insert_byte(sv2, LBS_SV_HOP_COUNT, val);
	}
	if (yes_or_no2(&near, "Use SRTE Bcast (automatic)", TRUE, TRUE)) {
	    val = get_num_in_range2(&near, "    SRTE Bcast (automatic)", 16, 0, 255, 7);
	    sv2 = insert_byte(sv2, LBS_SV_SRTE_ENABLED, val);
	}
    
	PUTSHORT(&sv->length, (uchar *)sv2 - (uchar *)sv);
	sv = sv2;
    }

    if (yes_or_no2(&near, "Use Route Info", TRUE, TRUE)) {
	sv2 = insert_header(sv, LBS_SV_ROUTE_STATUS);
	if (yes_or_no2(&near, "Use Route ID", TRUE, TRUE)) {
	    sv3 = insert_header(sv2, LBS_SV_ROUTE_IDENT);
	    if (yes_or_no2(&near, "Use Port 1", TRUE, TRUE)) {
		get_macaddr(&near, "    Port 1", address, "0000.3000.5735");
		sv3 = insert_address(sv3, address);
	    }
	    if (yes_or_no2(&near, "Use Port 1", TRUE, TRUE)) {
		get_macaddr(&near, "    Port 1", address, "0000.3000.5735");
		sv3 = insert_address(sv3, address);
	    }
	    PUTSHORT(&sv2->length, (uchar *)sv3 - (uchar *)sv2);
	    sv2 = sv3;
	}
	if (yes_or_no2(&near, "Use Bridge ID", TRUE, TRUE)) {
	    val = get_num_in_range2(&near, "    Bridge ID", 16, 0, 255, 7);
	    sv2 = insert_byte(sv2, LBS_SV_BR_IDENT, val);
	}
	if (yes_or_no2(&near, "Use SRTE Active (manual)", TRUE, TRUE)) {
	    val = get_num_in_range2(&near, "    SRTE Bcast (manual)", 16, 0, 255, 7);
	    sv2 = insert_byte(sv2, LBS_SV_SRTE_MODE, val);
	}
    
	PUTSHORT(&sv->length, (uchar *)sv2 - (uchar *)sv);
	sv = sv2;
    }

    if (yes_or_no2(&near, "Use Internal Status", FALSE, TRUE)) {
	val = get_num_in_range2(&near, "    Internal Status", 16, 0, 255, 0);
	sv = insert_byte(sv, LBS_SV_INTERNAL_STATUS, val);
    }
    if (yes_or_no2(&near, "Use Permanent", FALSE, TRUE)) {
	val = get_num_in_range2(&near, "    Permanent", 16, 0, 255, 0);
	sv = insert_byte(sv, IBMNM_PERMANENT, val);
    }
    
    sv = tibmnm_add_arbitrary(&near, sv);
    vector->length = (uchar *)sv - (uchar *)vector;
    ibmnm_llc2_send(test_cookie, pak, vector->length);
}

/*
 * Ask which LBS command to send.
 */
void tibmnm_lbs (jmp_buf *far)
{
    jmp_buf near;
    char buffer[IBMNM_MAXBUF];
    boolean done;

    /*
     * The abort code.
     */
    if (setjmp(&near)) {
	longjmp(far, 1);
    }

    done = FALSE;
    while (!done) {
	printf("What type of LBS packet would you like to send?\n\n");
	printf("1) Request LBS Status\n");
	printf("2) Set LBS Parameters\n");
	if (!rdtty(buffer, IBMNM_MAXBUF))
	    longjmp(far, 1);
	
	if (null(buffer))
	    continue;

	switch (tolower(*buffer)) {
	  case '0': done = TRUE;                  break;
	  case '1': tibmnm_req_lbs_status(&near); break;
	  case '2': tibmnm_set_lbs_parms(&near);  break;
	}
    }
}
#endif
