/* $Id: ibmnm_rem.c,v 3.4.44.5 1996/08/26 15:08:06 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/ibmnm_rem.c,v $
 *------------------------------------------------------------------
 * ibmnm_rem.c -- IBM Network Management Protocol
 *                Ring Error Monitor Functions
 *
 * January 1991, David R. Hampton
 *
 * Copyright (c) 1991-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibmnm_rem.c,v $
 * Revision 3.4.44.5  1996/08/26  15:08:06  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.4.44.4  1996/08/07  08:59:28  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.44.3  1996/07/09  05:57:58  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.4.44.2  1996/05/17  11:20:19  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.32.3  1996/04/26  15:19:41  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.32.2  1996/04/26  07:54:05  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4.32.1  1996/04/03  14:33:47  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.44.1  1996/03/18  20:11:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/03/07  09:41:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  14:21:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/16  22:06:45  motto
 * CSCdi35920:  %ALIGN-3-CORRECT messages on 4500
 *
 * Revision 3.3  1995/11/17  09:22:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:35:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:45:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:01:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/13  20:33:02  jacobt
 * CSCdi36703:  replace malloc() by malloc_named()
 *
 * Revision 2.2  1995/06/15  22:00:45  hampton
 * Fix 24.5 day bugs in Lan Manager support.  While there, eliminate
 * direct references to msclock.  [CSCdi35914]
 *
 * Revision 2.1  1995/06/07 20:46:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/**********************************************************************
 *
 * Ring Error Monitor Functions, as described in the IBM Token Ring
 * Network Architecture Reference Manual, Third Edition, September 1989,
 * Chapter 15.
 *
 * The Ring Error Monitor serves several functions.
 *
 * 1) It reports the current operational status of a ring to Lan Network
 *    Manager upon request.
 *
 * 2) It listens to all MAC "soft error report" frames, and keeps track
 *    of errors for all stations on the ring.  It also ages out these
 *    errors over period of time.  (lanmgr.c)
 *
 * 3) It forwards the contents of any/all MAC "soft error report"
 *    frame(s) to interested Lan Network Managers.  It must be directed
 *    by the controlling Lan Network Manager to enter this mode, and told
 *    which class(es) of soft errors to forward.
 *
 * 4) It reports to any interested Lan Network Manager whenever a station
 *    becomes congested, and again when the congestion goes away.
 *    (Upcall from lanmgr.c)
 *
 * 5) It reports to any interested Lan Network Manager whenever various
 *    error thresholds are exceeded, and again when the error rate
 *    decreases.  (Upcall from lanmgr.c)
 *
 * 6) It reports to any interested Lan Network Manager whenever a ring
 *    exceeds various error thresholds are exceeded, and again when the
 *    error rate decreases.  (Upcall from lanmgr.c)
 *
 * 7) It monitors the operational state of the ring.  If the ring begins
 *    beaconing, then it attempts to determine the result of the
 *    automatic recovery process.  It then forwards whatever information
 *    it can determine to any Lan Network Manager.
 *
 **********************************************************************/

#include "master.h"
#include <ctype.h>
#undef   tolower                        /* use library function call */
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "ieee.h"
#include "logger.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "lanmgr.h"
#include "ibmnm.h"
#include "../partner/partner_registry.h"
#include "../srt/srb_core.h"


/*
 * Local Storage
 */
static uchar rem_version[VERSIONLEN] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0};
static char *const rem_names[] = {
    "REM 8100", "Request REM Status", "Report REM Status",
    "Set REM Parameters", "REM Parameters Set",
    "REM Parameters Changed Notification",
    "REM Error Rate Decaying Notification",
    "REM Pre-Weight Exceeded Notification", "REM Weight Exceeded Notification",
    "REM Non-Isolating Threshold Exceeded", "REM Forward MAC Frame",
    "REM 810B", "REM Error", "REM 810D",
    "REM Receiver Congestion Notification", "REM Receiver Congestion Ended",
    "REM Beaconing Condition on Ring", "REM Beaconing Condition Recovered"};


/**********************************************************************
 *
 *	       Convert Internal REM info various ways.
 *
 **********************************************************************/

#define NAME(x) rem_names[(x) & COMMAND_MASK]

static char *rem_to_string (
    int command)
{
    if (command < REM_LAST)
	return(rem_names[command & COMMAND_MASK]);
    return("unknown");
}

int remid_to_bitmask (ushort command)
{
    switch (command) {
      case IBMNM_CORRELATOR:	     return(REM_B_CORRELATOR);
      case IBMNM_RING:		     return(REM_B_RING);
      case REM_SV_NOTIF_ENABLE:	     return(REM_B_NOTIF_ENABLE);
      case REM_SV_INTEN_DATA:	     return(REM_B_INTEN_DATA);
      case REM_SV_RING_INTEN:	     return(REM_B_ID_RING);
      case REM_SV_AUTO_INTEN:	     return(REM_B_ID_AUTO);
      case REM_SV_ISO_TABLE:         return(REM_B_ISOLATING_TABLE);
      case REM_SV_RING_STATUS_A:     return(REM_B_RING_STATUS_A);
      case REM_SV_RESET:             return(REM_B_RESET);
      default:                       return(0);
    }
}

char *rembit_to_string (int bit)
{
    switch (bit) {
      case REM_B_CORRELATOR:       return("Correlator");
      case REM_B_RING:		   return("Ring");
      case REM_B_NOTIF_ENABLE:	   return("Notification Enable");
      case REM_B_INTEN_DATA:	   return("Intensive Data");
      case REM_B_ID_RING:	   return("Ring ID");
      case REM_B_ID_AUTO:	   return("Auto ID");
      case REM_B_ISOLATING_TABLE:  return("Isolating Table");
      case REM_B_RING_STATUS_A:    return("Ring Status");
      case REM_B_RESET:            return("Reset");
      default:                     return("unknown");
    }
}

/**********************************************************************
 *
 *			   Vector Builders
 *
 **********************************************************************/

/*
 * insert_rem_ringstat
 *
 * Create a REM "ring status" vector.  This indicates the current
 * operational state of the ring.  f the ring in non-operational, this
 * vector also include information on who is beaconing, and what they are
 * beaconing.
 */
ibmnm_vector *insert_rem_ringstat (ibmnm_vector *vec, ring_info *ring)
{
    ibmnm_vector *sv, *sv2;
    station_info *info;
    hwidbtype *idb;

    idb = ring_to_idb(ring);
    info = find_station(ring->last_beacon_address, idb);

    sv = insert_header(vec, REM_SV_RING_STATUS);
    sv = insert_short(sv, REM_SV_RING_STATE, ring->rem_state);
    if (info) {
	sv2 = insert_header(sv, REM_SV_BEACON_DATA);
	sv2 = insert_short(sv2, REM_SV_BEACON_TYPE, ring->last_beacon_type);
	sv2 = insert_address(sv2, info->address);
	sv2 = insert_naun(sv2, ring->last_beacon_naun);
	sv2 = insert_location(sv2, info->phys_location);
	sv = finish_subvector(sv, sv2);
    }
    vec = finish_subvector(vec, sv);
    return(vec);
}

/*
 * insert_rem_iso_table
 *
 * Create an "isolating error table" in the format expected by Lan
 * Network Manager.  This format is not documented anywhere, except to
 * say that it is implementation dependant.  The following vector was
 * reconstructed by studying the IBM Bridge Program, v2.1.  It appears to
 * be the bridge's internal data structure copied into a vector.
 *
 * The structure starts with a header that contains the number of entries
 * with errors, the index of the first entry with errors, the index of
 * the first free entry, and one unknown value.  The rest of the
 * structure consist of fifteen entries in a table.  Each entry contains
 * a next poiner, the thresholds exceeded, the error count, the station
 * address and location, the stations NAUN, and the decay rate.  There
 * are also four unknown values.  Lan Network Manager assumes that the
 * entries in this table that contain errors are linked in descending
 * order of weight.
 */
ibmnm_vector *insert_rem_iso_table (ibmnm_vector *vec, ring_info *ring)
{
    iso_tbl_vector *tbl;
    station_info *info;
    int i, j, tmp, last;
    int sort[15];

    PUTSHORT(&vec->command, REM_SV_ISO_TABLE);
    PUTSHORT(&vec->length, 4 + sizeof(iso_tbl_vector));
    tbl = (iso_tbl_vector *)&vec->data;

    /*
     * Fill in as many entries as there on the list of stations with
     * errors (max 15).  Everything is filled in except for the link
     * numbers.
     */
    info = ring->station_errors;
    for (i = 0; i < 15; i++) {
	if (!info)
	    break;
	tbl->entry[i].next = 0;
	tbl->entry[i].pad1 = 0;
	tbl->entry[i].threshold = info->error_state & ERR_LANMGR;
	tbl->entry[i].softerr_count = min(info->weight, 0x80);
	ieee_copy(info->address, tbl->entry[i].address);
	ieee_copy(info->naun, tbl->entry[i].naun_address);
	tbl->entry[i].location = info->phys_location;
	tbl->entry[i].decay = info->decay_count;
	info = info->nexterror;
    }

    /*
     * Now do a simple sort and fill in all the link numbers.  Gag!  LNM
     * will not sort this table, it assumes it is already linked in
     * numerically descending order by weight.
     */
    last = i;
    if (i) {
	for (i = 0; i < last; i++)
	    sort[i] = i;
	for (i = 0; i < last-1; i++) {
	    for (j = i+1; j < last; j++) {
		if (tbl->entry[sort[j]].softerr_count > tbl->entry[sort[i]].softerr_count) {
		    tmp = sort[i];
		    sort[i] = sort[j];
		    sort[j] = tmp;
		}
	    }
	}
	tbl->header.head_err = sort[0]+1;
	for (i = 0; i < last-1; i++)
	    tbl->entry[sort[i]].next = sort[i+1]+1;
	tbl->entry[sort[i]].next = 0;
    } else {
	tbl->header.head_err = 0;
    }

    /*
     * Finish off the table header and say "bye bye".
     */
    tbl->header.pad1 = 0;
    tbl->header.num_err   = last;
    tbl->header.head_free = last+1;
    return(next_subvector(vec));
}

/**********************************************************************
 *
 *			  Protocol Responses
 *
 **********************************************************************/

/*
 * rem_error_frame (810C)
 *
 * Send a REM Error Frame.  This message indicates that a command
 * received from the Lan Network Manager contained a syntax error, or the
 * command could not be completed sucessfully.
 */
void rem_error_frame (
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

    sv = insert_major(pak, REM_ERROR);
    sv = insert_correlator(sv, correlator);
    sv = insert_short(sv, IBMNM_ERRCODE, code);
    sv = insert_short(sv, IBMNM_ERROFFSET, offset);
    sv = insert_bytes(sv, IBMNM_VERSION, rem_version, VERSIONLEN);
    finish_vector(pak, sv);

    lrm_send_link(pak, bridge, link, NAME(REM_ERROR));
}

/*
 * The following routines handle the Ring Error Monitor status reporting.
 * A Lan Network Manager sends us a Request REM Status frame.  The
 * response to that message is either a Report REM Status frame or an
 * Error frame.
 *
 * LRM Request Status (I) -->
 * a) <-- LRM Report Status (I)
 * b) <-- LRM Error (I)
 */

/*
 * rem_report_status (8102)
 *
 * Send a Report Status Message back to the requesting Lan Network
 * Manager.  This returns information for each of the items requested by
 * the Lan Network Manager.
 */
boolean rem_report_status (bridge_info *bridge, int link, rem_pb *result)
{
    paktype *pak;
    ibmnm_vector *sv, *sv2;
    ring_info *ring;

    ring = ringno_to_ring(*result->ring);
    if (!ring) return(FALSE);
    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return(FALSE);

    sv = insert_major(pak, REM_REP_STATUS);
    sv = insert_correlator(sv, *result->correlator);
    sv = insert_ring(sv, *result->ring);
    if (result->notification)
	sv = insert_short(sv, REM_SV_NOTIF_ENABLE, ring->notif_flags);
    if (result->intensive_mode) {
	sv2 = insert_header(sv, REM_SV_INTEN_DATA);
	if (result->id_ring)
	    sv2 = insert_short(sv2, REM_SV_RING_INTEN, ring->ring_inten_mask);
	if (result->id_auto)
	    sv2 = insert_short(sv2, REM_SV_AUTO_INTEN, ring->auto_inten_mask);
	sv = finish_subvector(sv, sv2);
    }
    if (result->iso_table)
	sv = insert_rem_iso_table(sv, ring);
    if (result->ring_status)
	sv = insert_rem_ringstat(sv, ring);
    finish_vector(pak, sv);

    lrm_send_link(pak, bridge, link, NAME(REM_REP_STATUS));
    return(TRUE);
}

/*
 * The following routines handle the Ring Error Monitor "Set Parameters"
 * frame.  A Lan Network Manager sends us a Set REM Parameters frame.
 * The response to that message is either a Report REM Status frame or an
 * Error frame.
 *
 * Set REM Parameters (I) -->
 * a) <-- REM Parameters Set (I)
 *        REM Parameters Changed (I)  -->
 * b) <-- REM Error (I)
 */

/*
 * rem_parameters_set (8104)
 *
 * Send a Parameters Set Message back to the requesting Lan Network
 * Manager.  No values are echoed, only the fact that the command
 * succeeded.  All values present (usually only one) must be set
 * sucessfully for this message to be returned.
 */
void rem_parameters_set (bridge_info *bridge, int link, ushort correlator)
{
    paktype *pak;
    ibmnm_vector *sv;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, REM_PARAM_SET);
    sv = insert_correlator(sv, correlator);
    finish_vector(pak, sv);

    lrm_send_link(pak, bridge, link, NAME(REM_PARAM_SET));
}

/*
 * rem_parameters_changed (8105)
 *
 * Send a Parameters Changed Message to all Lan Network Managers other
 * than the requesting Lan Network Manager, informing them of exactly
 * which parameters changed.  Usually only one parameter is changed at a
 * time, but there code doesn;t assume that will always be true.
 */
void rem_parameters_changed (bridge_info *bridge, int link, rem_pb *result)
{
    paktype *pak;
    ibmnm_vector *sv, *sv2;
    ring_info *ring;


    ring = ringno_to_ring(*result->ring);
    if (!ring) return;
    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, REM_PARAM_CHANGED);
    sv = insert_ring(sv, *result->ring);
    if (result->notification)
	sv = insert_short(sv, REM_SV_NOTIF_ENABLE, ring->notif_flags);
    if (result->intensive_mode) {
	sv2 = insert_header(sv, REM_SV_INTEN_DATA);
	if (result->id_ring)
	    sv2 = insert_short(sv2, REM_SV_RING_INTEN, ring->ring_inten_mask);
	if (result->id_auto)
	    sv2 = insert_short(sv2, REM_SV_AUTO_INTEN, ring->auto_inten_mask);
	sv = finish_subvector(sv, sv2);
    }
    if (result->reset)
	sv = insert_atomic(sv, REM_SV_RESET);
    finish_vector(pak, sv);

    lrm_send_all_but(pak, bridge, link, NAME(REM_PARAM_CHANGED));
}

/**********************************************************************
 *
 *		 Upcalls from MAC level packet exchange
 *
 **********************************************************************/

/*
 * rem_xxx_notification (8106,7,8)
 *
 * Send a XXX Notification Message back to all interested Lan Network
 * Managers.  This routine is called to send "Error Rate Decaying",
 * "Pre-Weight Exceeded", and "Weight Exceeded" Notifications.
 */
void rem_xxx_notification (ushort command, station_info *info)
{
    paktype *pak;
    ibmnm_vector *sv, *sv2, *sv3, *sv4;
    ring_info *ring;
    station_info *naun;

    if ((command == REM_WEIGHT_EXC) && info->error_xmt)
        return;

    ring = info->idb->ring_info;
    if (is_ieee_zero(info->naun))
	naun = find_station(info->naun, info->idb);
    else
	naun = NULL;
    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;

    if (command == REM_WEIGHT_EXC)
	info->error_xmt = TRUE;
    else if (command == REM_ERROR_DECAY)
	info->error_xmt = FALSE;

    sv = insert_major(pak, command);
    sv = insert_ring(sv, ring->ringno);
    sv = insert_short(sv, REM_SV_NOTIF_ENABLE, FALSE);

    {
	sv2 = insert_header(sv, REM_SV_ISOL_STATUS);
	sv2 = insert_byte(sv2, REM_SV_DEC_INTERVL, REM_DECREMENT_INTERVAL/ONESEC);
	sv2 = insert_byte(sv2, REM_SV_ENTRIES, ring->num_entries);
	
	{
	    sv3 = insert_header(sv2, REM_SV_FAULT_DOM);
	    {
		sv4 = insert_header(sv3, REM_SV_RECEIVER);
		sv4 = insert_byte(sv4, REM_SV_FLAGS, info->error_state & ERR_LANMGR);
		sv4 = insert_byte(sv4, REM_SV_WEIGHT, info->weight);
		sv4 = insert_address(sv4, info->address);
		sv4 = insert_naun(sv4, info->naun);
		sv4 = insert_location(sv4, info->phys_location);
		sv3 = finish_subvector(sv3, sv4);
	    }
	    if (naun) {
		sv4 = insert_header(sv3, REM_SV_TRANSMITTER);
		sv4 = insert_byte(sv4, REM_SV_FLAGS, naun->error_state);
		sv4 = insert_byte(sv4, REM_SV_WEIGHT, naun->weight);
		sv4 = insert_address(sv4, naun->address);
		sv4 = insert_naun(sv4, naun->naun);
		sv4 = insert_location(sv4, naun->phys_location);
		sv3 = finish_subvector(sv3, sv4);
	    }
	    sv2 = finish_subvector(sv2, sv3);
	}
	sv = finish_subvector(sv, sv2);
    }
    finish_vector(pak, sv);

    lrm_send_ring(pak, info->idb, rem_to_string(command));
}

/*
 * rem_noniso_thresh (8109)
 *
 * Send a Non-Isolating Threshold Exceeded Message to all interested Lan
 * Network Managers.  This also resets any over-limit counters to zero.
 */
void rem_noniso_thresh (hwidbtype *idb)
{
    paktype *pak;
    ibmnm_vector *sv, *sv2;
    ring_info *ring;
    int value1, value2, value3, value4, value5;

    ring = idb->ring_info;
    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;

    sv = insert_major(pak, REM_NONISO_THRESH);
    sv = insert_ring(sv, ring->ringno);
    sv = insert_short(sv, REM_SV_NOTIF_ENABLE, ring->notif_flags);

    /*
     * Compute whether or not any of the counters has rolled over.  Also
     * cap the counters at their limit.
     */
    if (ring->noniso.lost_frame >= REM_NONISO_LIMIT) {
	value1 = REM_NONISO_LIMIT;
	ring->noniso.lost_frame = 0;
    } else {
	value1 = ring->noniso.lost_frame;
    }
    if (ring->noniso.congestion >= REM_NONISO_LIMIT) {
	value2 = REM_NONISO_LIMIT;
	ring->noniso.congestion = 0;
    } else {
	value2 = ring->noniso.congestion;
    }
    if (ring->noniso.frame_copied >= REM_NONISO_LIMIT) {
	value3 = REM_NONISO_LIMIT;
	ring->noniso.frame_copied = 0;
    } else {
	value3 = ring->noniso.frame_copied;
    }
    if (ring->noniso.frequency >= REM_NONISO_LIMIT) {
	value4 = REM_NONISO_LIMIT;
	ring->noniso.frequency = 0;
    } else {
	value4 = ring->noniso.frequency;
    }
    if (ring->noniso.token >= REM_NONISO_LIMIT) {
	value5 = REM_NONISO_LIMIT;
	ring->noniso.token = 0;
    } else {
	value5 = ring->noniso.token;
    }

    /*
     * Add the non-isolating errors to the vector.  An IBM bridge always
     * includes all of the counters, regardless of whether or not they
     * have exceeded their limits.  Because the IBM bridge does this, the
     * router should do it as well.
     */
    sv2 = insert_header(sv, REM_SV_NONISO_NOTIF);
    sv2 = insert_short(sv2, REM_SV_LOST_FRAMES,     (value1 << 8) | REM_NONISO_LIMIT);
    sv2 = insert_short(sv2, REM_SV_RCVR_CONGESTION, (value2 << 8) | REM_NONISO_LIMIT);
    sv2 = insert_short(sv2, REM_SV_FC_ERRORS,       (value3 << 8) | REM_NONISO_LIMIT);
    sv2 = insert_short(sv2, REM_SV_FREQ_ERRORS,     (value4 << 8) | REM_NONISO_LIMIT);
    sv2 = insert_short(sv2, REM_SV_TOKEN_ERRORS,    (value5 << 8) | REM_NONISO_LIMIT);
    sv2 = insert_short(sv2, REM_SV_RESERVED_ERRORS,      (0 << 8) | REM_NONISO_LIMIT);
    sv2 = insert_short(sv2, REM_SV_TABLE_FULL_ERRORS,    (0 << 8) | REM_NONISO_LIMIT);
    sv2 = insert_short(sv2, REM_SV_MIN_DEC_ERRORS,       (0 << 8) | REM_NONISO_LIMIT);
    sv2 = insert_short(sv2, REM_SV_RCVR_CONG_TBL_FULL,   (0 << 8) | 8);
    sv = finish_subvector(sv, sv2);

    /*
     * Finish.
     */
    finish_vector(pak, sv);
    lrm_send_ring(pak, idb, NAME(REM_NONISO_THRESH));
}

/*
 * rem_forward_mac_frame (810A)
 *
 * Relay a MAC "Report Soft Error", "Report Neighbor Notification
 * Incomplete", or "Report Monitor Error" frame received from a ring
 * station.  This information is fowarded to any interested Lan Network
 * Managers.  This routine is only called when the controlling Lan
 * Network Manager has put the ring into "ring intensive" or "auto
 * intensive" mode, and the conditions for reporting have been met.
 */
void rem_forward_mac_frame (
    station_info *info,
    int mvid,
    uchar *iso,
    uchar *noniso)
{
    paktype *pak;
    ibmnm_vector *sv, *sv2;
    ring_info *ring;

    ring = info->idb->ring_info;
    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, REM_FORWARD);
    sv = insert_ring(sv, ring->ringno);
    sv = insert_address(sv, info->address);

    switch (mvid) {
      case MVID_SOFTERR:
	sv2 = insert_header(sv, REM_SV_SOFT_ERR_REPT);
	sv2 = insert_naun(sv2, info->naun);
	sv2 = insert_location(sv2, info->phys_location);
	sv2 = insert_bytes(sv2, REM_SV_ISO_COUNTS, iso, 6);
	sv2 = insert_bytes(sv2, REM_SV_NONISO_COUNTS, noniso, 6);
	sv = finish_subvector(sv, sv2);
	break;

      case MVID_NNINCOMP:
	sv2 = insert_header(sv, REM_SV_NNIF_REPT);
	sv2 = insert_address(sv2, ring->last_nnin_addr);
	sv = finish_subvector(sv, sv2);
	break;

      case MVID_RPTMONERR:
	sv2 = insert_header(sv, REM_SV_MON_ERR_REPT);
	sv2 = insert_error(sv2, ring->last_monerr_code);
	sv2 = insert_location(sv2, info->phys_location);
	sv2 = insert_naun(sv2, info->naun);
	sv = finish_subvector(sv, sv2);
	break;
    }
    finish_vector(pak, sv);

    lrm_send_ring(pak, info->idb, NAME(REM_FORWARD));
}

/*
 * rem_congestion_xxx (810E, F)
 *
 * Send a "Receiver Congestion Notification" or a "Receiver Congestion
 * Ended" message to any interested Lan Network Managers.  This routine
 * is called upon receipt of a soft error indicating receiver congestion,
 * or upon the REM background process noticing that the haven't been any
 * of these messages recently.
 */
void rem_congestion_xxx (station_info *info, int command)
{
    paktype *pak;
    ibmnm_vector *sv;
    idbtype      *swidb = firstsw_or_null(info->idb);

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, command);
    sv = insert_ring(sv, srbV.srb_sb_get_thisring(swidb));
    sv = insert_address(sv, info->address);
    finish_vector(pak, sv);

    lrm_send_ring(pak, info->idb, rem_to_string(command));
}

/**********************************************************************
 *
 *		  Beaconing Messages and Postmortem
 *
 **********************************************************************/

/*
 * rem_temp_beaconing (8110)
 *
 * This routine sends a "ring temporary beaconed" message to attached Lan
 * Network Managers.  Included is the ring status that tells whether or
 * not the ring is currently operational.  If the ring is operational,
 * then this indicates that it really was a temporary problem, and has
 * been recovered.  The problem station(s) will be listed in the message.
 * If the ring is not operational, then this is the indication that there
 * is an unrecoverable problem on the ring and manual intervention is
 * required.
 */
void rem_temp_beaconing (ring_info *ring, uchar *addr1, uchar *addr2)
{
    paktype *pak;
    ibmnm_vector *sv;
    hwidbtype *idb;

    /*
     * Build and send the packet
     */
    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, REM_BEACONING);
    sv = insert_ring(sv, ring->ringno);
    sv = insert_rem_ringstat(sv, ring);
    if (addr1)
	sv = insert_address(sv, addr1);
    if (addr2)
	sv = insert_address(sv, addr2);
    finish_vector(pak, sv);

    idb = ring_to_idb(ring);
    lrm_send_ring(pak, idb, NAME(REM_BEACONING));
}

/*
 * rem_beacon_recovered (8111)
 *
 * This routine sends a "beaconing ring recovered" message to attached
 * Lan Network Managers.  It indicates that a permanent beaconing
 * condition has been fixed, and the ring is now usable again.
 */
static void rem_beacon_recovered (
    ring_info *ring)
{
    paktype *pak;
    ibmnm_vector *sv;
    hwidbtype *idb;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, REM_BEACONEND);
    sv = insert_ring(sv, ring->ringno);
    finish_vector(pak, sv);

    idb = ring_to_idb(ring);
    lrm_send_ring(pak, idb, NAME(REM_BEACONEND));
}

/*
 * rem_success_fail
 *
 * A query sent by the CRS on behalf of the REM has either succeeded or
 * failed.  The contents of the frame is unimportant.  Only the fact that
 * there was or was not an answer is important.  This is used to
 * determine if a station that was part of a fault domain has removed
 * itself from the ring, or if it is still present on the ring.
 */
void rem_success_fail (station_info *info, boolean answered)
{
    ring_info *ring;

    if (!answered)
	return;

    info->beaconing = FALSE;
    
    ring = idb_to_ring(info->idb);
    lnmebuginf("\nIBMNM%d: Beaconing, heard from %e", ring->ringno, info->address);
    if (ring->rem_state == RS_TMP_BCN_ONE_PRES)
	ring->rem_state = RS_TMP_BCN_BOTH_PRES;
    if (ring->rem_state == RS_TMP_BCN_NONE_PRES)
	ring->rem_state = RS_TMP_BCN_ONE_PRES;
    
    if (!ring->beacon_info->beaconing && !ring->beacon_naun->beaconing) {
	/*
	 * They're both back.  Tell Lan Network Manager and shutdown
	 * postmortem.
	 */
	lnmebuginf("\nIBMNM%d: Beaconing, Postmortem Finished", ring->ringno);
	rem_temp_beaconing(ring, NULL, NULL);
	ring->rem_state = RS_NORMAL;
	ring->beaconing = BCN_NONE;
	rem_purge_softerr(ring);
    }
}

/*
 * rem_beacon_postmortem
 *
 * The ring is beaconing.  What to do?  What to do?  Well, here are the
 * rules.  If the ring beacons for more than one minute, give up and
 * declare a permanent error condition.  If the ring beacons for less
 * than one minute, query each station that was part of the fault domain
 * to see if they passed their self tests.  Then report the temporary
 * error to Lan Network Manager, including the addresses of the stations
 * that did not answer the query (i.e. left the ring).
 */
boolean rem_beacon_postmortem (ring_info *ring)
{
    uchar *addr1, *addr2;

    /*
     * Is this ring in any sort of beaconing state?  If not, must be some
     * other ring that is having problems.  Don't keep the timer running
     * on this rings account.
     */
    if (ring->beaconing == BCN_NONE)
	return(FALSE);

    /*
     * Is this ring in a permanent error condition?  If so, keep the
     * timer running to make the detection of a recovery occur faster.
     * (Is this really necessary?  When the ring recovers, there will at
     * least be a new active monitor present message.  Will there be
     * something that starts the REM processing though?)
     */
    if (ring->beaconing == BCN_PERMANENT)
	return(TRUE);

    /*
     * The ring is still actively beaconing.  Is it time to give up yet?
     */
    if (ring->beaconing == BCN_BEACONING) {
	if (TIMER_RUNNING(ring->last_beacon_time) &&
	    CLOCK_OUTSIDE_INTERVAL(ring->last_beacon_time, ONEMIN)) {
	    ring->rem_state = RS_PRM_BCN;
	    ring->beaconing = BCN_PERMANENT;
	    rem_temp_beaconing(ring, NULL, NULL);
	    /*
	     * This will be reset by a "UP" ring status.
	     */
	}
	return(TRUE);
    }

    /*
     * Ah.  The ring has finally stopped beaconing.  Time to look and see
     * if either of the stations in the fault domain removed themselved,
     * and then report to Lan Network Manager.
     */
    if (ring->beaconing == BCN_POSTMORTEM) {
	/*
	 * Was the ring out long enough to declare this a permanent
	 * error?  Execution should never really get here, as beaconing
	 * should be discovered while it is still happening.  Just in
	 * case...
	 */
	if (TIMER_RUNNING(ring->last_beacon_time) &&
	    CLOCK_OUTSIDE_INTERVAL(ring->last_beacon_time, ONEMIN)) {
	    /*
	     * This was a permanent error
	     */
	    lnmebuginf("\nIBMNM%d: Beaconing, Permanent", ring->ringno);
	    rem_beacon_recovered(ring);
	    ring->beaconing = BCN_NONE;
	    ring->rem_state = RS_NORMAL;
	    rem_purge_softerr(ring);
	    return(FALSE);
	}
	/*
	 * Temporary error.
	 */
	lnmebuginf("\nIBMNM%d: Beaconing, Postmortem Started", ring->ringno);
	/*
	 * Something Wrong, terminate the Beaconing Postmortem process
	 */
	if (!(ring->beacon_info = find_station(ring->last_beacon_address, NULL)) ||
	    !(ring->beacon_naun = find_station(ring->last_beacon_naun, NULL))) {

	    if (!ring->beacon_info)
	        lnmebuginf("\nIBMNM%d: Beaconing Postmortem can not find station %e", 
						ring->ringno, ring->last_beacon_address);
	    if (!ring->beacon_naun)
	        lnmebuginf("\nIBMNM%d: Beaconing Postmortem can not find station %e", 
						ring->ringno, ring->last_beacon_naun);
		
	    ring->beaconing = BCN_NONE;
	    addr1 = NULL;
	    addr2 = NULL;
	    if (ring->beacon_info) {
	        free_crs_pak(ring->beacon_info);
	        ring->beacon_info->beaconing = FALSE;
	        addr1 = ring->beacon_info->address;
	    }
	    if (ring->beacon_naun) {
	        free_crs_pak(ring->beacon_naun);
	        ring->beacon_naun->beaconing = FALSE;
	        addr2 = ring->beacon_naun->address;
	    }
	    rem_temp_beaconing(ring, addr1, addr2);
	    ring->rem_state = RS_NORMAL;
	    rem_purge_softerr(ring);
	    return(FALSE);
	}
	ring->beacon_info->beaconing = TRUE;
	ring->beacon_naun->beaconing = TRUE;
	ring->rem_state = RS_TMP_BCN_NONE_PRES;
    }

    /*
     * Have we retried the proscribed number of times without getting an
     * answer back from both stations in the fault domain?  It's time to
     * quit trying and let Lan Network Manager know that the ring has
     * recovered, and that one or more of the station(s) removed
     * themselves.
     */
    if (ring->beaconing == BCN_POSTMORTEM3) {
	/*
	 * Last call.  Either they are back or forget 'em.
	 */
	lnmebuginf("\nIBMNM%d: Beaconing, Postmortem Timed Out", ring->ringno);
	ring->beaconing = BCN_NONE;
	addr1 = NULL;
	addr2 = NULL;
	if (ring->beacon_info->beaconing) {
	    free_crs_pak(ring->beacon_info);
	    ring->beacon_info->beaconing = FALSE;
	    addr1 = ring->beacon_info->address;
	}
	if (ring->beacon_naun->beaconing) {
	    free_crs_pak(ring->beacon_naun);
	    ring->beacon_naun->beaconing = FALSE;
	    addr2 = ring->beacon_naun->address;
	}
	rem_temp_beaconing(ring, addr1, addr2);
	ring->rem_state = RS_NORMAL;
	rem_purge_softerr(ring);
	return(FALSE);
    }

    /*
     * Send queries to see if the two adapters are present.
     */
    if (ring->beacon_info->beaconing)
	send_crs_general(ring->beacon_info, MVID_REQADDRESS, 1);
    if (ring->beacon_naun->beaconing)
	send_crs_general(ring->beacon_naun, MVID_REQADDRESS, 1);

    /*
     * Step to the next phase.  This is essentially the retry count.
     */
    lnmebuginf("\nIBMNM%d: Beaconing, Postmortem Next Stage", ring->ringno);
    ring->beaconing++;
    return(TRUE);
}


/**********************************************************************
 *
 *                    Parsers
 * 
 **********************************************************************/

/*
 * rem_parse_imd
 *
 * Parse an "Intensive Mode Data" vector.  The subvectors of this vector
 * are masks indicating which MAC frames should be forwarded on to Lan
 * Network Manager.
 */
boolean rem_parse_imd (
    ibmnm_vector *vector,
    ulong required,
    ulong optional,
    rem_pb *result,
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
	bit = remid_to_bitmask(GETSHORT(&sv->command));
	present |= bit;
	
	/*
	 * Is it allowed?
	 */
	if ((((required | optional) & REM_B_ID_MASK) & bit) == 0) {
	    lnmllcbuginf(unxpctvec, rembit_to_string(bit));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}

	/*
	 * Record it.
	 */
	data = (uchar *)&sv->data;
	switch (GETSHORT(&sv->command)) {
	  case REM_SV_RING_INTEN: result->id_ring = (ushort *)data; break;
	  case REM_SV_AUTO_INTEN: result->id_auto = (ushort *)data; break;
	  default:
	      /* Shouldn't ever happen.  Should be caught above. */
	      lnmllcbuginf(unknvec, GETSHORT(&sv->command));
	      *error = ERR_BAD_SV_VALUE;
	      *offset = sv;
	      break;
	  }
    }

    /*
     * Are any required vectors missing?
     */
    missing = (required & REM_B_ID_MASK) & ~present;
    if (missing) {
	lnmllcbuginf(missvec, rembit_to_string(missing));
	*error = ERR_REQ_SV_MISSING;
	*offset = sv;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * rem_parse_vectors
 *
 * Parse the subvectors in a major vector sent to the REM.  This routine
 * checks to insure that all required vectors are present.  It also
 * insures that the major vector contains only the required and optional
 * subvectors that are permissible for this particular major vector.
 * This routine builds and returns a data structure that indicates which
 * subvectors were present, and what their values were.
 */
boolean rem_parse_vectors (
    ibmnm_vector *vector,
    ulong required,
    ulong optional,
    rem_pb *r,
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
	bit = remid_to_bitmask(GETSHORT(&sv->command));
	present |= bit;

	/*
	 * Is it allowed?
	 */
	if ((((required | optional) & REM_B_TOP_MASK) & bit) == 0) {
	    lnmllcbuginf(unxpctvec, rembit_to_string(bit));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}

	/*
	 * Record it.
	 */
	data = (uchar *)&sv->data;
	switch (GETSHORT(&sv->command)) {
	  case IBMNM_CORRELATOR:     r->correlator   = (ushort *)data; break;
	  case IBMNM_RING:           r->ring         = (ushort *)data; break;
	  case REM_SV_NOTIF_ENABLE:  r->notification = (ushort *)data; break;
	  case REM_SV_ISO_TABLE:     r->iso_table    = (ushort *)data; break;
	  case REM_SV_RING_STATUS_A: r->ring_status  = (ushort *)data; break;
	  case REM_SV_RESET:         r->reset        = TRUE;           break;
	  case REM_SV_INTEN_DATA: 
	    r->intensive_mode = TRUE;
	    rem_parse_imd(sv, required, optional, r, error, offset);
	    if (*error)
		return(FALSE);
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
    missing = (required & REM_B_TOP_MASK) & ~present;
    if (missing) {
	lnmllcbuginf(missvec, rembit_to_string(missing));
	*error = ERR_REQ_SV_MISSING;
	*offset = sv;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * ibmnm_rem_input
 *
 * All input frames that are directed to the Ring Error Monitor ends up
 * here.  Parse them, query the ring for information if necessary, and
 * then return the requested information to the Lan Network Manager.
 */
void ibmnm_rem_input (llctype *cookie, ibmnm_vector *vector)
{
    bridge_info *bridge;
    ring_info *ring;
    int link, error;
    ushort correlator;
    ibmnm_vector *offset;
    rem_pb *result;
    srb_triplet_t srb_triplet;

    /*
     * Housekeeping
     */
    if (!cookie_to_link(cookie, &bridge, &link)) {
	buginf(nolink, cookie);
	return;
    }
    srbV.srb_sb_get_triplet(bridge->idb1->firstsw, &srb_triplet);
    ibmnm_print_bridge_rcvd_msg(&srb_triplet,
				rem_to_string(GETSHORT(&vector->command)),
				bridge, link); 
    if (!(result = malloc_named(sizeof(rem_pb), lnm_cb)))
	return;
    error = 0;

    switch (GETSHORT(&vector->command)) {
      case REM_REQ_STATUS:
	if (!ibmnm_parse_request_status(vector, &correlator, &error, &offset)) {
	    rem_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	if (!rem_parse_vectors(vector, REM_R_REQ_STATUS, REM_O_REQ_STATUS,
			       result, &error, &offset)) {
	    rem_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	if (!rem_report_status(bridge, link, result)) {
	    rem_error_frame(bridge, link, correlator, ERR_EXECUTION, 0);
	    break;
	}
	break;

      case REM_SET_PARAM:
	if (!ibmnm_parse_set_request(vector, &correlator, &error, &offset)) {
	    rem_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	if (!ibmnm_changes_allowed(bridge, link)) {
	    lrm_error_frame(bridge, link, correlator, SET_ERR_UNAUTH_REQ, 0);
	    lrm_control_breach(bridge, link);
	    break;
	}
	if (!rem_parse_vectors(vector, REM_R_SET_PARAM, REM_O_SET_PARAM,
			       result, &error, &offset)) {
	    rem_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}

	/*
	 * Must have a ring number.
	 */
	if (!*result->ring) {
            /*
             * If the ring number is zero, check to see if it is for the
             * local lnm feature before log an error.
             */
            if (!reg_invoke_local_lnm_ringno(*result->ring)) {
	        rem_error_frame(bridge, link, correlator, ERR_REQ_SV_MISSING, 0);
	        break;
            }
	}
	ring = ringno_to_ring(*result->ring);

	/*
	 * Set the values
	 */
	if (result->notification)
	    ring->notif_flags = *result->notification;
	if (result->id_ring)
	    ring->ring_inten_mask = *result->id_ring;
	if (result->id_auto)
	    ring->auto_inten_mask = *result->id_auto;
	if (result->reset) {
	    ring->notif_flags     = REM_NOTIF_DEF;
	    ring->ring_inten_mask = REM_RINGIN_DEF;
	    ring->auto_inten_mask = REM_AUTOIN_DEF;
	    rem_purge_softerr(ring);
	}

	/*
	 * Notify everyone
	 */
	rem_parameters_set(bridge, link, correlator);
	rem_parameters_changed(bridge, link, result);
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
 * 		   Ring Error Monitor Test Code
 *
 * All code past here implements the Lan Network Manager side of this
 * protocol.
 *
 **********************************************************************/
#include "../llc2/llc2.h"
#include "ttysrv.h"

/*
 * Build and send a "Request REM Status" message.  This should elicit a
 * "Report REM Status" response.
 */
static void tibmnm_req_rem_status (
    jmp_buf *far)
{
    jmp_buf near;
    paktype *pak;
    ibmnm_vector *vector, *sv, *sv2;
    int ring, id;

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

    sv = insert_major(pak, REM_REQ_STATUS);
    if (yes_or_no2(&near, "Use Correlator", TRUE, TRUE))
	sv = insert_correlator(sv, test_correlator++);
    if (yes_or_no2(&near, "Use Ring", TRUE, TRUE)) {
	ring = get_num_in_range2(&near, "    Ring", 16, 0, 65535, 0);
	sv = insert_ring(sv, ring);
    }
    if (yes_or_no2(&near, "Use Notification Enable", FALSE, TRUE)) {
	sv = insert_atomic(sv, REM_SV_NOTIF_ENABLE);
    }

    if (yes_or_no2(&near, "Use Intensive Mode", FALSE, TRUE)) {
	sv2 = insert_header(sv, REM_SV_INTEN_DATA);
	if (yes_or_no2(&near, "Use Ring Intensive", FALSE, TRUE)) {
	    id = get_num_in_range2(&near, "    Ring Intensive", 10, 0, 65535, 0);
	    sv2 = insert_short(sv2, REM_SV_RING_INTEN, id);
	}
	if (yes_or_no2(&near, "Use Auto Intensive", FALSE, TRUE)) {
	    id = get_num_in_range2(&near, "    Auto Intensive", 10, 0, 65535, 0);
	    sv2 = insert_short(sv2, REM_SV_AUTO_INTEN, id);
	}
	PUTSHORT(&sv->length, (uchar *)sv2 - (uchar *)sv);
	sv = sv2;
    }

    if (yes_or_no2(&near, "Use Isolating Table", FALSE, TRUE)) {
	sv = insert_atomic(sv, REM_SV_ISO_TABLE);
    }
    if (yes_or_no2(&near, "Use Ring Status", TRUE, TRUE)) {
	sv = insert_atomic(sv, REM_SV_RING_STATUS_A);
    }
   
    sv = tibmnm_add_arbitrary(&near, sv);
    vector->length = (uchar *)sv - (uchar *)vector;
    ibmnm_llc2_send(test_cookie, pak, vector->length);
}

/*
 * Build and send a "Set REM Parameters" message.  This should elicit a
 * "REM Parameters Set" response.
 */
static void tibmnm_set_rem_parms (
    jmp_buf *far)
{
    jmp_buf near;
    paktype *pak;
    ibmnm_vector *vector, *sv, *sv2;
    int ring, ne, id;

    /*
     * Build a packet.
     */
    pak = ibmnm_getbuffer(100);
    if (!pak) {
	printf("Cannot get a packet buffer");
	longjmp(far, 1);
    }

    /*
     * The abort code.
     */
    if (setjmp(&near)) {
	if (pak)
	    retbuffer(pak);
	longjmp(far, 1);
    }

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    sv = insert_major(pak, REM_SET_PARAM);
    if (yes_or_no2(&near, "Use Correlator", TRUE, TRUE))
	sv = insert_correlator(sv, test_correlator++);
    if (yes_or_no2(&near, "Use Ring", FALSE, TRUE)) {
	ring = get_num_in_range2(&near, "    Ring", 16, 0, 65535, 0);
	sv = insert_ring(sv, ring);
    }
    if (yes_or_no2(&near, "Use Notification Enable", FALSE, TRUE)) {
	ne = get_num_in_range2(&near, "    Ring Intensive", 10, 0, 65535, 0);
	sv = insert_short(sv, REM_SV_NOTIF_ENABLE, ne);
    }

    if (yes_or_no2(&near, "Use Intensive Mode", TRUE, TRUE)) {
	sv2 = insert_header(sv, REM_SV_INTEN_DATA);
	if (yes_or_no2(&near, "Use Ring Intensive", TRUE, TRUE)) {
	    id = get_num_in_range2(&near, "    Ring Intensive", 10, 0, 65535, 0);
	    sv2 = insert_short(sv2, REM_SV_RING_INTEN, id);
	}
	if (yes_or_no2(&near, "Use Auto Intensive", TRUE, TRUE)) {
	    id = get_num_in_range2(&near, "    Auto Intensive", 10, 0, 65535, 0);
	    sv2 = insert_short(sv2, REM_SV_AUTO_INTEN, id);
	}
	PUTSHORT(&sv->length, (uchar *)sv2 - (uchar *)sv);
	sv = sv2;
    }

    if (yes_or_no2(&near, "Use Reset", TRUE, TRUE)) {
	sv = insert_atomic(sv, REM_SV_RESET);
    }
   
    sv = tibmnm_add_arbitrary(&near, sv);
    vector->length = (uchar *)sv - (uchar *)vector;
    ibmnm_llc2_send(test_cookie, pak, vector->length);
}

/*
 * Ask which RPS command to send.
 */
void tibmnm_rem (jmp_buf *far)
{
    jmp_buf near;
    boolean done;
    char buffer[IBMNM_MAXBUF];

    /*
     * The abort code.
     */
    if (setjmp(&near)) {
	longjmp(far, 1);
    }

    done = FALSE;
    while (!done) {
	printf("What type of REM packet would you like to send?\n\n");
	printf("1) Request REM Status\n");
	printf("2) Set REM Parameters\n");
	if (!rdtty(buffer, IBMNM_MAXBUF))
	    longjmp(far, 1);
	
	if (null(buffer))
	    continue;

	switch (tolower(*buffer)) {
	  case '0': done = TRUE;                  break;
	  case '1': tibmnm_req_rem_status(&near); break;
	  case '2': tibmnm_set_rem_parms(&near);  break;
	}
    }
}
#endif
