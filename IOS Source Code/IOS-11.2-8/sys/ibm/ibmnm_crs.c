/* $Id: ibmnm_crs.c,v 3.3.42.5 1996/08/26 15:07:56 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/ibmnm_crs.c,v $
 *------------------------------------------------------------------
 * ibmnm_crs.c -- IBM Network Management Protocol
 *                Configuration Report Server Functions
 *
 * January 1991, David R. Hampton
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibmnm_crs.c,v $
 * Revision 3.3.42.5  1996/08/26  15:07:56  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.3.42.4  1996/08/07  08:59:22  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.42.3  1996/07/09  05:57:47  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.3.42.2  1996/05/17  11:20:08  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.30.3  1996/04/26  15:19:33  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.30.2  1996/04/26  07:53:57  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.30.1  1996/04/03  14:33:40  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.42.1  1996/03/18  20:11:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.2  1996/03/07  09:41:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.12.1  1996/02/20  14:21:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/16  22:06:40  motto
 * CSCdi35920:  %ALIGN-3-CORRECT messages on 4500
 *
 * Revision 3.2  1995/11/17  09:22:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:01:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/13  20:32:56  jacobt
 * CSCdi36703:  replace malloc() by malloc_named()
 *
 * Revision 2.2  1995/06/15  22:00:38  hampton
 * Fix 24.5 day bugs in Lan Manager support.  While there, eliminate
 * direct references to msclock.  [CSCdi35914]
 *
 * Revision 2.1  1995/06/07 20:46:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *
 * Configuration Report Server Functions, as described in the IBM Token
 * Ring Network Architecture Reference Manual, Third Edition, September
 * 1989, Chapter 16.
 *
 * The Configuration Report Server serves several functions.
 *
 * 1) It notifies Lan Network Manager any time a stations upstream
 *    neighbor changes.  This will send notifications for all station
 *    insertions, in addition to the RPS message.  It will also send
 *    notification for all de-insertions from a ring.  This is the only
 *    notification that a station has left the ring.  (Upcall from
 *    lanmgr.c.)
 *
 * 2) It notifies Lan Network Manager any time the active monitor
 *    changes.  This will occur once for each time the ring goes through
 *    the ring recovery process.  (Upcall from lanmgr.c.)
 *
 * 3) Any time a station forwards a MAC packet on behalf of someone else.
 *    This is an unusual situation that rarely happens.  The only current
 *    use for the "Transmit Forward" MAC frame is in the Lan Network
 *    Manager "Path Test" command.  (Upcall from lanmgr.c.)
 *
 * 4) It responds to a Lan Network Manager's request for information on a
 *    ring station.  It does this by issuing the appropriate MAC frames
 *    to the station to gather the requested information. It then returns
 *    this information back to Lan Network Manager in the established LLC
 *    connection.
 *
 * 5) It removes a station from the ring.  It does so only at the behest
 *    of a controlling Lan Network Manager.  The actual sequence of
 *    events is to send a MAC death frame, followed by an MAC information
 *    query frame.  If there is no response to the query, then the
 *    station is assumed to have removed itself.
 *
 * 6) It sends the appropriate MAC frames to change the operational
 *    parameters in any given ring station.  Again, it does this at the
 *    behest of a controlling Lan Network Manager.
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
#include "lanmgr.h"
#include "ieee.h"
#include "../srt/srb_sb.h"
#include "ibmnm.h"
#include "../srt/srb_core.h"


/*
 * Local Storage
 */
static uchar crs_version[VERSIONLEN] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0};
static char *const crs_names[] = {
    "CRS 8300", "CRS Report New Active Monitor", "CRS Report NAUN Change",
    "CRS Report Transmit-Forward", "CRS Request Station Information",
    "CRS 8305", "CRS Report Station Information", "CRS Set Station Parameters",
    "CRS Station Parameters Set", "CRS Remove Ring Station",
    "CRS Ring Station Removed", "CRS Error"};


/**********************************************************************
 *
 *	       Convert Internal CRS info various ways.
 *
 **********************************************************************/

#define NAME(x) crs_names[(x) & COMMAND_MASK]

static char *crs_to_string (
    int command)
{
    if (command < CRS_LAST)
	return(crs_names[command & COMMAND_MASK]);
    return("unknown");
}

int crsid_to_bitmask (ushort command)
{
    switch (command) {
      case IBMNM_CORRELATOR:	  return(CRS_B_CORRELATOR);
      case IBMNM_ADDRESS:	  return(CRS_B_ADDRESS);
      case IBMNM_RING:		  return(CRS_B_RING);
      case IBMNM_ALL:		  return(CRS_B_ALL);
      case IBMNM_LOCATION:	  return(CRS_B_LOCATION);

      case CRS_SV_ADDRESS_INFO_A: return(CRS_B_ADDRESS_INFO);
      case CRS_SV_STATE_INFO_A:	  return(CRS_B_STATE_INFO);
      case CRS_SV_ATTACH_INFO_A:  return(CRS_B_ATTACH_INFO);
      case CRS_SV_ENABLED_FUNC:	  return(CRS_B_ENABLED_FUNC);
      case CRS_SV_ALLOW_PRIO:	  return(CRS_B_ALLOW_PRIO);
      case CRS_SV_SOFTERR:	  return(CRS_B_SOFTERR);
      case CRS_SV_LOCALRING:	  return(CRS_B_LOCALRING);
      default:			  return(0);
    }
}

char *crsbit_to_string (int bit)
{
    switch (bit) {
      case CRS_B_CORRELATOR:    return("Correlator");
      case CRS_B_ADDRESS:	return("Address");
      case CRS_B_RING:		return("Ring");
      case CRS_B_ALL:           return("All");
      case CRS_B_LOCATION:	return("Location");

      case CRS_B_ADDRESS_INFO:	return("Address Info");
      case CRS_B_STATE_INFO:	return("State Info");
      case CRS_B_ATTACH_INFO:	return("Attachments Info");
      case CRS_B_ENABLED_FUNC:	return("Enabled Functions");
      case CRS_B_ALLOW_PRIO:	return("Allowed Priority");
      case CRS_B_SOFTERR:	return("Soft Error Timer");
      case CRS_B_LOCALRING:	return("Local Ring Number");
      default:                  return("unknown");
    }
}

/**********************************************************************
 *
 *			  Protocol Responses
 *
 **********************************************************************/

/*
 *
 * The following routines handle the Configuration Report Server report
 * station information frame.  A lan manager sends us a Request Station
 * Information frame.  The response to that message is either a Report
 * Station Information frame or an Error frame.
 *
 * CRS Request Station Information (I) -->
 * a) <-- CRS Report Station Information (I)
 * b) <-- CRS Error (I)
 *
 * This high level request will produce some or all of the following MAC
 * level sequence of messages.
 *
 *     MAC Request Ring Station Address -->
 * <-- MAC Report  Ring Station Address
 *     MAC Request Ring Station State   -->
 * <-- MAC Report  Ring Station State
 *     MAC Request Ring Station Attachments -->
 * <-- MAC Report  Ring Station Attachments
 *
 */

/*
 * crs_report_station (8306)
 *
 * Send a CRS Report Staion Frame.  This frame will contain varying
 * amounts of information, depending on what was requested by the Lan
 * Manager.
 */
void crs_report_station (station_info *info, uchar reason, ulong error)
{
    paktype *pak;
    ibmnm_vector *sv, *sv2;
    crs_data *data;
    idbtype  *swidb = firstsw_or_null(info->idb);

    if (!(data = info->request_data))
	return;

    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;

    sv = insert_major(pak, CRS_REP_STA_INFO);
    sv = insert_correlator(sv, data->correlator);
    sv = insert_address(sv, info->address);
    sv = insert_ring(sv, srbV.srb_sb_get_thisring(swidb));

    if (reason == CRS_REASON_NONE) {
	if (data->all || data->address_info) {
	    sv2 = insert_header(sv, CRS_SV_ADDRESS_INFO);
	    sv2 = insert_naun(sv2, info->naun);
	    sv2 = insert_location(sv2, info->phys_location);
	    sv2 = insert_long(sv2, CRS_SV_GROUP, info->group);
	    sv2 = insert_long(sv2, CRS_SV_FUNCTIONAL, info->functional);
	    sv = finish_subvector(sv, sv2);
	}
	if (data->all || data->state_info) {
	    sv2 = insert_header(sv, CRS_SV_STATE_INFO);
	    sv2 = insert_bytes(sv2, CRS_SV_UCODE_LEVEL, info->ucode, UCODELEN);
	    sv2 = insert_bytes(sv2, CRS_SV_STA_STATUS, info->status_vector, STATUSLEN);
	    sv2 = insert_macaddr(sv2, CRS_SV_UNIQUE_ID, info->station_id);
	    sv = finish_subvector(sv, sv2);
	}
	if (data->all || data->attach_info) {
	    sv2 = insert_header(sv, CRS_SV_ATTACH_INFO);
	    sv2 = insert_productid(sv2, info->product_id);
	    sv2 = insert_short(sv2, CRS_SV_ENABLED_FUNC, info->enabled_functions);
	    sv2 = insert_short(sv2, CRS_SV_ALLOW_PRIO, info->allowed_priority);
	    sv2 = insert_long(sv2, CRS_SV_FUNCTIONAL, info->functional);
	    sv = finish_subvector(sv, sv2);
	}
    } else {
	sv2 = insert_header(sv, CRS_SV_STATION_ERR);
	sv2 = insert_short(sv2, CRS_SV_REASON, reason);
	if (reason == CRS_REASON_NACK)
	    sv2 = insert_long(sv2, CRS_SV_RESPONSE, error);
	sv = finish_subvector(sv, sv2);
    }
    finish_vector(pak, sv);

    lrm_send_link(pak, data->bridge, data->link, NAME(CRS_REP_STA_INFO));
    info->request_data = NULL;
    free(data);
}

/*
 * crs_error_frame (830B)
 *
 * Send a CRS Error Frame.  This message indicates that a command
 * received from the Lan Manager contained a syntax error, or the command
 * could not be completed sucessfully.
 */
void crs_error_frame (
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

    sv = insert_major(pak, CRS_ERROR);
    sv = insert_correlator(sv, correlator);
    sv = insert_short(sv, IBMNM_ERRCODE, code);
    sv = insert_short(sv, IBMNM_ERROFFSET, offset);
    sv = insert_bytes(sv, IBMNM_VERSION, crs_version, VERSIONLEN);
    finish_vector(pak, sv);

    lrm_send_link(pak, bridge, link, NAME(CRS_ERROR));
}

/*
 *
 * The following routines handle the Configuration Report Server "Set
 * Station Parameters" frame.  A lan manager sends us a CRS Set Station
 * Parameters frame.  The response to that message is either a Station
 * Parameters Set frame or an Error frame.
 *
 * CRS Set Station Parameters (I) -->
 * a) <-- CRS Station Parameters Set (I)
 * b) <-- CRS Error (I)
 *
 */

/*
 * crs_parameters_set (8308)
 *
 * Send a CRS Station Parameters Set Frame.
 */
void crs_parameters_set (station_info *info, uchar reason, ulong error)
{
    paktype *pak;
    ibmnm_vector *sv, *sv2;
    crs_data *data;

    if (!(data = info->request_data))
	return;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, CRS_STATION_SET);
    sv = insert_correlator(sv, data->correlator);
    if (reason != CRS_REASON_NONE) {
	sv2 = insert_header(sv, CRS_SV_STATION_ERR);
	sv2 = insert_short(sv2, CRS_SV_REASON, reason);
	if (reason == CRS_REASON_NACK)
	    sv2 = insert_long(sv2, CRS_SV_RESPONSE, error);
	sv = finish_subvector(sv, sv2);
    }
    finish_vector(pak, sv);

    lrm_send_link(pak, data->bridge, data->link, NAME(CRS_STATION_SET));
    info->request_data = NULL;
    free(data);
}

/*
 *
 * The following routines handle the Configuration Report Server "Remove
 * Ring Station" frame.  A lan manager sends us a CRS Remove Ring Station
 * frame.  The response to that message is either a Ring Station Removed
 * frame or an Error frame.
 *
 * CRS Remove Ring Station (I) -->
 * a) <-- CRS Ring Station Removed (I)
 * b) <-- CRS Error (I)
 *
 */

/*
 * crs_station_removed (830A)
 *
 * Send a CRS Station Removed Frame.
 */
void crs_station_removed (
    station_info *info,
    uchar status,
    uchar reason,
    ulong response)
{
    paktype *pak;
    ibmnm_vector *sv, *sv2;
    crs_data *data;

    if (!(data = info->request_data))
	return;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, CRS_STA_REMOVED);
    sv = insert_correlator(sv, data->correlator);
    sv = insert_byte(sv, CRS_SV_STATUSCODE, status);
    if (status == CRS_STAT_NOT_REMOVED) {
	sv2 = insert_header(sv, CRS_SV_STATION_ERR);
	sv2 = insert_short(sv2, CRS_SV_REASON, reason);
	if (reason == CRS_REASON_NACK)
	    sv2 = insert_long(sv2, CRS_SV_RESPONSE, response);
	sv = finish_subvector(sv, sv2);
    }
    finish_vector(pak, sv);

    lrm_send_link(pak, data->bridge, data->link, NAME(CRS_STA_REMOVED));
    info->request_data = NULL;
    free(data);
}

/**********************************************************************
 *
 * Upcalls from the MAC level code.  These are produced by asynchronous
 * events ocurring on the ring.  These routines notify the attached Lan
 * Network Managers about these events.
 *
 **********************************************************************/

/*
 * crs_report_new_monitor (8301)
 *
 * A ring just established a new active monitor.  Forward this
 * information off to all Lan Managers that might be interested.
 */
void crs_report_new_monitor (station_info *info)
{
    paktype *pak;
    ibmnm_vector *sv;
    idbtype      *swidb = firstsw_or_null(info->idb);

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, CRS_REP_NEWMON);
    sv = insert_ring(sv, srbV.srb_sb_get_thisring(swidb));
    sv = insert_address(sv, info->address);
    sv = insert_naun(sv, info->naun);
    sv = insert_location(sv, info->phys_location);
    sv = insert_productid(sv, info->product_id);
    finish_vector(pak, sv);

    lrm_send_ring(pak, info->idb, NAME(CRS_REP_NEWMON));
}

/*
 * crs_report_naun_change (8302)
 *
 * Someone just inserted into or left the ring.  Forward this information
 * off to all Lan Managers that might be interested.
 */
void crs_report_naun_change (station_info *info)
{
    paktype *pak;
    ibmnm_vector *sv;
    idbtype      *swidb = firstsw_or_null(info->idb);


    watch_ring_poll(info->idb, TRUE);

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, CRS_REP_NAUN_CHG);
    sv = insert_ring(sv, srbV.srb_sb_get_thisring(swidb));
    sv = insert_address(sv, info->address);
    sv = insert_naun(sv, info->naun);
    sv = insert_location(sv, info->phys_location);
    finish_vector(pak, sv);

    lrm_send_ring(pak, info->idb, NAME(CRS_REP_NAUN_CHG));
}

/*
 * crs_report_transmit_fwd (8303)
 *
 * Someone on the ring just forwarded a MAC packet on behalf of someone
 * else.  (MAC Transmit Forward) Forward this information off to all Lan
 * Managers that might be interested.
 */
void crs_report_transmit_fwd (station_info *info)
{
    paktype *pak;
    ibmnm_vector *sv;
    idbtype      *swidb = firstsw_or_null(info->idb);

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, CRS_REP_XMIT_FWD);
    sv = insert_ring(sv, srbV.srb_sb_get_thisring(swidb));
    sv = insert_address(sv, info->address);
    sv = insert_short(sv, CRS_SV_STATUS, info->transmit_status);
    finish_vector(pak, sv);

    lrm_send_ring(pak, info->idb, NAME(CRS_REP_XMIT_FWD));
}

/**********************************************************************
 *
 * More upcalls from the MAC level code.  These calls occur because of a
 * MAC level packet exchange initiated by this router, which was caused
 * by a query from a Lan Manager.  Forward the result off to the resuest
 * initiator.
 *
 **********************************************************************/

/*
 * crs_got_info
 *
 * This routine is involved in the lan manager "report station
 * information" command.  It is called for each of the valid intermediate
 * MAC level information responses, and determines whether more
 * information is needed.  If all information has been obtained, it calls
 * the routine to return the result.
 */
void crs_got_info (station_info *info, uchar vector)
{
    boolean success;
    crs_data *data;

    if (info->beaconing) {
	rem_success_fail(info, TRUE);
	return;
    }

    /*
     * O.K. What was it that just completed.
     */
    if (!(data = info->request_data))
	return;

    switch (vector) {
      case MVID_RPTADDRESS: data->address_done = TRUE; break;
      case MVID_RPTSTATE:   data->state_done   = TRUE; break;
      case MVID_RPTATTACH:  data->attach_done  = TRUE; break;
      }

    /*
     * Are we done yet?  If not, initiate the next query.
     */
    success = FALSE;
    if (!data->address_done && (data->address_info || data->all))
	success = send_crs_general(info, MVID_REQADDRESS, CRS_MAX_TRIES);
    else if (!data->state_done && (data->state_info || data->all))
	success = send_crs_general(info, MVID_REQSTATE, CRS_MAX_TRIES);
    else if (!data->attach_done && (data->attach_info || data->all))
	success = send_crs_general(info, MVID_REQATTACH, CRS_MAX_TRIES);
    else {
	/* Were done! */
	crs_report_station(info, CRS_REASON_NONE, 0);
	return;
    }

    if (!success) {
	/* Oops, something screwed up. */
	info->removing = 0;
	data = info->request_data;
	crs_error_frame(data->bridge, data->link, data->correlator, ERR_EXECUTION, 0);
    }
}

/*
 * crs_retry_remove
 *
 * Either we received a response to a "REMOVE" packet (actually the
 * "Request Address" packet sent immediately after) or else we have timed
 * out the packet.  Retry the sequence the proscribed number of times,
 * and then declare success.
 */
static void crs_retry_remove (
    station_info *info,
    boolean answer)
{
    boolean success;
    crs_data *data;

    if (answer && info->removing++ < CRS_REMOVE_TRIES) {
	success =  send_crs_remove(info);
	success &= send_crs_general(info, MVID_REQADDRESS, 1);
	if (!success) {
	    info->removing = 0;
	    data = info->request_data;
	    crs_error_frame(data->bridge, data->link, data->correlator, ERR_EXECUTION, 0);
	}
    } else {
	info->removing = 0;
	crs_station_removed(info, CRS_STAT_REMOVED, 0, 0);
    }
}

/*
 * crs_success_fail
 *
 * The router either received a response to a given request, or we timed
 * out out the request.  Figure out the appropriate action based upon the
 * initial request, whether or not the station was present on the ring,
 * whether or not it answered the request, and the actual answer.
 *
 * This routine contains a gross, ugly hack.  It is necessary because the
 * Madge code does not have any provisions for reading back the Frame
 * Status byte from a MAC frame.  (This is possible with the TI code, but
 * was not implemented.)  If the MAC frame is not answered, the code
 * should then look at the frame status bytes of the transmitted frame.
 * If the "Address Regognized" bit is set, then the station *IS* present
 * on the ring, even though it did not respond.  Since there is no access
 * to the frame status byte, another way to get the same information is
 * need.  Since the router already has a list of which stations are
 * present on the ring (from eavesdropping on the ring polls), this was
 * used.  If a station is already known to be present on the ring, and it
 * does not answer a query, then it is marked as "no response".  If a
 * station is not known to be present (i.e. it is added to the list
 * because of a request and is marked as termporary), then the lack of an
 * answer to a query is concluded to mean that the station is "not
 * present" on the ring.
 *
 */
void crs_success_fail (
    station_info *info,
    uchar mvid,
    boolean answered,
    ulong response)
{
    int reason;
    idbtype *swidb = firstsw_or_null(info->idb);

    /*
     * Check for a "cross server" connection.
     */
    if (info->beaconing) {
	rem_success_fail(info, answered);
	return;
    }

    /*
     * Compute the reason value for Lan Network Manager.
     */
    if (answered)
	reason = (response&RESP_ERROR) ? CRS_REASON_NACK : CRS_REASON_NONE;
    else if (TIMER_RUNNING(info->temp)) /* the hack */
	reason = CRS_REASON_NOT_RECOG;
    else
	reason = CRS_REASON_TIMEOUT;

    /*
     * Figure out which upcall to make based upon the vector.  This is
     * the original vector set out, not the vector of the response (if
     * any).
     */
    switch (mvid) {
      case MVID_REQADDRESS:
      case MVID_REQSTATE:
      case MVID_REQATTACH:
	if (info->removing) {
	    crs_retry_remove(info,answered);
	    break;
	}
	if (answered)
	    lnmmacbuginf("\nLANMGR%d: Received bad positive response to %s",
			 srbV.srb_sb_get_thisring(swidb), mvid_to_str(mvid));
	crs_report_station(info, reason, response);
	break;

      case MVID_CHANGEPARAM:
	crs_parameters_set(info, reason, response);
	break;

      case MVID_REMOVE:
	/*
	 * There should never be a response to a remove.  If there is,
	 * handle it.
	 */
	info->removing = 0;
	if (answered)
	    lnmmacbuginf("\nLANMGR%d: Received a response to %s",
			 srbV.srb_sb_get_thisring(swidb), mvid_to_str(mvid));
	crs_station_removed(info, CRS_STAT_NOT_REMOVED, reason, response);
	break;

      default:
	break;
    }
}

/**********************************************************************
 *
 *			   Parsing Routines
 *
 **********************************************************************/

/*
 * crs_parse_vectors
 *
 * Parse the subvectors in a major vector sent to the CRS.  This routine
 * checks to insure that all required vectors are present.  It also
 * insures that the major vector contains only the required and optional
 * subvectors that are permissible for this particular major vector.
 * This routine builds and returns a data structure that indicates which
 * subvectors were present, and what their values were.
 */
boolean crs_parse_vectors (
    ibmnm_vector *vector,
    ulong required,
    ulong optional,
    crs_pb *r,
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
	bit = crsid_to_bitmask(GETSHORT(&sv->command));
	present |= bit;

	/*
	 * Is it allowed?
	 */
	if (!(required & bit) && !(optional & bit)) {
	    lnmllcbuginf(unxpctvec, crsbit_to_string(bit));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}

	/*
	 * Record it.
	 */
	data = (uchar *)&sv->data;
	switch (GETSHORT(&sv->command)) {
	  case IBMNM_CORRELATOR:      r->correlator   = (ushort *)data; break;
	  case IBMNM_ADDRESS:	      r->address      = (uchar  *)data; break;
	  case IBMNM_RING:	      r->ring	      = (ushort *)data; break;
	  case IBMNM_ALL:	      r->all	      = (ushort *)data; break;
	  case IBMNM_LOCATION:	      r->location     = (ulong  *)data; break;
	  case CRS_SV_ADDRESS_INFO_A: r->address_info = (uchar  *)data; break;
	  case CRS_SV_STATE_INFO_A:   r->state_info   = (uchar  *)data; break;
	  case CRS_SV_ATTACH_INFO_A:  r->attach_info  = (uchar  *)data; break;
	  case CRS_SV_ENABLED_FUNC:   r->enabled_func = (ulong  *)data; break;
	  case CRS_SV_ALLOW_PRIO:     r->allow_prio   = (ushort *)data; break;
	  case CRS_SV_SOFTERR:	      r->softerr      = (ushort *)data; break;
	  case CRS_SV_LOCALRING:      r->localring    = (ushort *)data; break;
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
    missing = required & ~present;
    if (missing) {
	lnmllcbuginf(missvec, crsbit_to_string(missing));
	*error = ERR_REQ_SV_MISSING;
	*offset = sv;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * ibmnm_crs_input
 *
 * All input frames that are directed to the Configuration Report Server
 * ends up here.  Parse them, query the ring for information if necessary,
 * and then return the requested information to the Lan Manager.
 */
void ibmnm_crs_input (llctype *cookie, ibmnm_vector *vector)
{
    bridge_info *bridge;
    int link, error;
    ibmnm_vector *offset;
    crs_pb *result;
    station_info *info;
    crs_data *data;
    ushort correlator;
    hwidbtype *idb;
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
				crs_to_string(GETSHORT(&vector->command)),
				bridge, link); 

    if (!(result = malloc_named(sizeof(crs_pb), lnm_cb)))
	return;
    error = 0;

    switch (GETSHORT(&vector->command)) {
	/*
	 * parse
	 */
      case CRS_REQ_STA_INFO:
	if (!ibmnm_parse_request_status(vector, &correlator, &error, &offset)) {
	    crs_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	if (!crs_parse_vectors(vector, CRS_R_REQ_STA_INFO, CRS_O_REQ_STA_INFO,
			       result, &error, &offset)) {
	    crs_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}

	/*
	 * Must have 'all' or one of the three partial information requests.
	 */
	if (!result->all &&
	    !(result->address_info || result->state_info || result->attach_info)) {
	    crs_error_frame(bridge, link, correlator, ERR_REQ_SV_MISSING, OFFSET(offset, vector));
	    break;
	}

	/*
	 * Check the ring number.
	 */
	if (*result->ring == srb_triplet.localRing)
	    idb = bridge->idb1;
	else if (*result->ring == srb_triplet.remoteRing)
	    idb = bridge->idb2;
	else {
	    crs_error_frame(bridge, link, correlator, ERR_EXECUTION, 0);
	    break;
	}

	/*
	 * build data structures
	 */
	if (!(info = find_or_build_station(result->address, idb, TRUE))) {
	    crs_error_frame(bridge, link, correlator, ERR_EXECUTION, 0);
	    break;
	}
	if ((data = info->request_data)) {
	    info->request_data = NULL;
	    free(data);
	}
	if (!(data = malloc_named(sizeof(crs_data), lnm_cb))) {
	    crs_error_frame(bridge, link, correlator, ERR_EXECUTION, 0);
	    break;
	}
	data->bridge = bridge;
	data->link = link;
	data->correlator = correlator;
	if (result->all)
	    data->all = TRUE;
	if (result->address_info)
	    data->address_info = TRUE;
	if (result->state_info)
	    data->state_info = TRUE;
	if (result->attach_info)
	    data->attach_info = TRUE;
	info->request_data = data;

	/*
	 * Start the process of getting information from the ring station.
	 */
	crs_got_info(info, 0);
	break;

      case CRS_SET_STATION:
	/*
	 * parse
	 */
	if (!ibmnm_parse_set_request(vector, &correlator, &error, &offset)) {
	    crs_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	if (!ibmnm_changes_allowed(bridge, link)) {
	    lrm_error_frame(bridge, link, correlator, SET_ERR_UNAUTH_REQ, 0);
	    lrm_control_breach(bridge, link);
	    break;
	}
	if (!crs_parse_vectors(vector, CRS_R_SET_STATION, CRS_O_SET_STATION,
			       result, &error, &offset)) {
	    crs_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}

	if (*result->ring == srb_triplet.localRing)
	    idb = bridge->idb1;
	else if (*result->ring == srb_triplet.bridgeNum)
	    idb = bridge->idb2;
	else {
	    crs_error_frame(bridge, link, correlator, ERR_EXECUTION, 0);
	    break;
	}

	/*
	 * build data structures
	 */
	if (!(info = find_or_build_station(result->address, idb, TRUE))) {
	    crs_error_frame(bridge, link, correlator, ERR_EXECUTION, 0);
	    break;
	}
	if ((data = info->request_data)) {
	    info->request_data = NULL;
	    free(data);
	}
	if (!(data = malloc_named(sizeof(crs_data), lnm_cb))) {
	    crs_error_frame(bridge, link, correlator, ERR_EXECUTION, 0);
	    break;
	}
	data->bridge = bridge;
	data->link = link;
	data->correlator = correlator;
	if (result->location)
	    info->phys_location = *result->location;
	if (result->enabled_func)
	    info->enabled_functions = *result->enabled_func;
	if (result->allow_prio)
	    info->allowed_priority = *result->allow_prio;
	info->request_data = data;

	/*
	 * kick it off
	 */
	if (!send_crs_set_sta(info, result->localring, result->location,
			 result->softerr, result->enabled_func, result->allow_prio)) {
	    crs_error_frame(bridge, link, correlator, ERR_EXECUTION, 0);
	    break;
	}

	break;

      case CRS_REMOVE_STA:
	/*
	 * parse
	 */
	if (!crs_parse_vectors(vector, CRS_R_REMOVE_STA, CRS_O_REMOVE_STA,
			       result, &error, &offset)) {
	    crs_error_frame(bridge, link,
			    (result->correlator ? *result->correlator : 0),
			    error, OFFSET(offset, vector));
	    break;
	}

	correlator = *result->correlator;
	if (*result->ring == srb_triplet.localRing)
	    idb = bridge->idb1;
	else if (*result->ring == srb_triplet.remoteRing)
	    idb = bridge->idb2;
	else {
	    crs_error_frame(bridge, link, correlator, ERR_EXECUTION, 0);
	    break;
	}

	/*
	 * build data structures
	 */
	if (!(info = find_or_build_station(result->address, idb, TRUE))) {
	    crs_error_frame(bridge, link, correlator, ERR_EXECUTION, 0);
	    break;
	}
	if ((data = info->request_data)) {
	    info->request_data = NULL;
	    free(data);
	}
	if (!(data = malloc_named(sizeof(crs_data), lnm_cb))) {
	    crs_error_frame(bridge, link, correlator, ERR_EXECUTION, 0);
	    break;
	}
	data->bridge = bridge;
	data->link = link;
	data->correlator = correlator;
	info->request_data = data;
	info->removing = 1;

	/*
	 * kick it off
	 */
	if (!send_crs_remove(info) ||
	    !send_crs_general(info, MVID_REQADDRESS, 1)) {
	    crs_error_frame(bridge, link, correlator, ERR_EXECUTION, 0);
	    break;
	}

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
 *		Configuration Report Server Test Code
 *
 * All code past here implements the Lan Network Manager side of this
 * protocol.
 *
 **********************************************************************/
#include "../llc2/llc2.h"
#include "ttysrv.h"

/*
 * Build and send a "Request Station Information" message.  This should
 * elicit a "Report Station Information" response.
 */
static void tibmnm_req_station_info (
    jmp_buf *far)
{
    jmp_buf near;
    paktype *pak;
    ibmnm_vector *vector, *sv;
    int ring;
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

    /*
     * Info from whom?
     */
    sv = insert_major(pak, CRS_REQ_STA_INFO);
    if (yes_or_no2(&near, "Use Correlator", TRUE, TRUE))
	sv = insert_correlator(sv, test_correlator++);
    if (yes_or_no2(&near, "Use Address", TRUE, TRUE)) {
	get_macaddr(&near, "    Address", address, "0000.3000.5735");
	sv = insert_address(sv, address);
    }
    if (yes_or_no2(&near, "Use Ring", TRUE, TRUE)) {
	ring = get_num_in_range2(&near, "    Ring", 16, 0, 65535, 0);
	sv = insert_ring(sv, ring);
    }

    /*
     * What Info?
     */
    if (yes_or_no2(&near, "Use All", TRUE, TRUE)) {
	sv = insert_atomic(sv, IBMNM_ALL);
    }
    if (yes_or_no2(&near, "Use Addressing Info", FALSE, TRUE)) {
	sv = insert_atomic(sv, CRS_SV_ADDRESS_INFO_A);
    }
    if (yes_or_no2(&near, "Use State Info", FALSE, TRUE)) {
	sv = insert_atomic(sv, CRS_SV_STATE_INFO_A);
    }
    if (yes_or_no2(&near, "Use Attachment Info", FALSE, TRUE)) {
	sv = insert_atomic(sv, CRS_SV_ATTACH_INFO_A);
    }

    sv = tibmnm_add_arbitrary(&near, sv);
    vector->length = (uchar *)sv - (uchar *)vector;
    ibmnm_llc2_send(test_cookie, pak, vector->length);
}

/*
 * Build and send a "Set Station Parameters" message.  This should elicit
 * a "Station Parameters Set" response.
 */
static void tibmnm_set_station_parms (
    jmp_buf *far)
{
    jmp_buf near;
    paktype *pak;
    ibmnm_vector *vector, *sv;
    uchar address[IEEEBYTES];
    int ring, location, set, ef, prio;

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

    /*
     * Which station
     */
    sv = insert_major(pak, CRS_SET_STATION);
    if (yes_or_no2(&near, "Use Correlator", TRUE, TRUE))
	sv = insert_correlator(sv, test_correlator++);
    if (yes_or_no2(&near, "Use Address", TRUE, TRUE)) {
	get_macaddr(&near, "    Address", address, "0000.3000.5735");
	sv = insert_address(sv, address);
    }
    if (yes_or_no2(&near, "Use Ring", TRUE, TRUE)) {
	ring = get_num_in_range2(&near, "    Ring", 16, 0, 65535, 0);
	sv = insert_ring(sv, ring);
    }

    /*
     * What Info?
     */
    if (yes_or_no2(&near, "Use Local Ring", TRUE, TRUE)) {
	ring = get_num_in_range2(&near, "    Ring", 16, 0, 65535, 0);
	sv = insert_short(sv, CRS_SV_LOCALRING, ring);
    }
    if (yes_or_no2(&near, "Use Location", TRUE, TRUE)) {
	location = get_num_in_range2(&near, "    Location", 16, 0, -1L, 0);
	sv = insert_location(sv, location);
    }
    if (yes_or_no2(&near, "Use Soft Error Timer", TRUE, TRUE)) {
	set = get_num_in_range2(&near, "    Soft Error Timer", 16, 0, 65535, 0);
	sv = insert_short(sv, CRS_SV_SOFTERR, set);
    }
    if (yes_or_no2(&near, "Use Enabled Functions", TRUE, TRUE)) {
	ef = get_num_in_range2(&near, "    Enabled Functions", 16, 0, -1L, 0);
	sv = insert_long(sv, CRS_SV_ENABLED_FUNC, ef);
    }
    if (yes_or_no2(&near, "Use Allowed Priority", TRUE, TRUE)) {
	prio = get_num_in_range2(&near, "    Allowed Priority", 16, 0, 65535, 0);
	sv = insert_short(sv, CRS_SV_ALLOW_PRIO, prio);
    }

    sv = tibmnm_add_arbitrary(&near, sv);
    vector->length = (uchar *)sv - (uchar *)vector;
    ibmnm_llc2_send(test_cookie, pak, vector->length);
}

/*
 * Build and send a "Remove Station" message.  This should elicit a
 * "Station Removed" response.
 */
static void tibmnm_remove_station (
    jmp_buf *far)
{
    jmp_buf near;
    paktype *pak;
    ibmnm_vector *sv, *vector;
    int ring;
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

    /*
     * Which station
     */
    sv = insert_major(pak, CRS_REMOVE_STA);
    if (yes_or_no2(&near, "Use Correlator", TRUE, TRUE))
	sv = insert_correlator(sv, test_correlator++);
    if (yes_or_no2(&near, "Use Address", TRUE, TRUE)) {
	get_macaddr(&near, "    Address", address, "0000.3000.5735");
	sv = insert_address(sv, address);
    }
    if (yes_or_no2(&near, "Use Ring", TRUE, TRUE)) {
	ring = get_num_in_range2(&near, "    Ring", 16, 0, 65535, 0);
	sv = insert_ring(sv, ring);
    }

    sv = tibmnm_add_arbitrary(&near, sv);
    vector->length = (uchar *)sv - (uchar *)vector;
    ibmnm_llc2_send(test_cookie, pak, vector->length);
}

/*
 * Ask which CRS command to send.
 */
void tibmnm_crs (jmp_buf *far)
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
	printf("What type of CRS packet would you like to send?\n\n");
	printf("1) Request Station Information\n");
	printf("2) Set Station Parameters\n");
	printf("3) Remove Ring Station\n");
	if (!rdtty(buffer, IBMNM_MAXBUF))
	    longjmp(far, 1);
	
	if (null(buffer))
	    continue;

	switch (tolower(*buffer)) {
	  case '0': done = TRUE;                     break;
	  case '1': tibmnm_req_station_info(&near);  break;
	  case '2': tibmnm_set_station_parms(&near); break;
	  case '3': tibmnm_remove_station(&near);    break;
	}
    }
}
#endif
