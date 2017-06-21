/* $Id: ibmnm_rps.c,v 3.3.44.5 1996/08/26 15:08:09 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/ibmnm_rps.c,v $
 *------------------------------------------------------------------
 * ibmnm_rps.c -- IBM Network Management Protocol
 *                Ring Parameter Server Functions
 *
 * January 1991, David R. Hampton
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibmnm_rps.c,v $
 * Revision 3.3.44.5  1996/08/26  15:08:09  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.3.44.4  1996/08/07  08:59:29  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.44.3  1996/07/09  05:58:01  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.3.44.2  1996/05/17  11:20:22  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.32.3  1996/04/26  15:19:43  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.32.2  1996/04/26  07:54:07  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.32.1  1996/04/03  14:33:49  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.44.1  1996/03/18  20:11:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/07  09:41:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  14:21:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/16  22:06:46  motto
 * CSCdi35920:  %ALIGN-3-CORRECT messages on 4500
 *
 * Revision 3.2  1995/11/17  09:22:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:01:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/13  20:33:03  jacobt
 * CSCdi36703:  replace malloc() by malloc_named()
 *
 * Revision 2.2  1995/06/09  13:05:06  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:47:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *
 * Ring Parameter Server Functions, as described in the IBM Token Ring
 * Network Architecture Reference Manual, Third Edition, September 1989,
 * Chapter 17.
 *
 * The Ring Parameter Server serves three functions.
 *
 * 1) It insures that all stations on a ring are using the same
 *    parameters.  The settable parameters are the local ring number and
 *    the soft error reporting timer.  The physical location of a station
 *    is also setteble, but this should be different for each station.
 *    We currently simply increment this for each station.  Most of this
 *    work is performed in lanmgr.c.  The actual sequence of messages is:
 *
 *    a) Station broadcasts an "Initialize Request" message.
 *    b) RPS sends an "Initialize Ring Station" message in response.
 *    c) Station send an "Response" (ack/nack) message.
 *
 *    The RPS also broadcasts an "Initialize Ring Station" message when
 *    it is first initializing, again to make sure all stations are using
 *    the same parameters.  The RPS is not allowed to become active if
 *    there is another RPS already present on the ring, and they have
 *    conflicting parameters.
 *
 *    The only number currently provided by the IBM Bridge Program (v2.1)
 *    is the Local Ring Number.
 *
 * 2) It reports to Lan Network Manager any time a station enters the
 *    ring.  This happens at the end of the three step initialization
 *    described above.  When the RPS receives the response message, it
 *    forwards the value in this message to Lan Network Manager.  This
 *    indicates that the station attempted to enter the ring, and whether
 *    or not is was sucessful.  The three step initialization code is on
 *    lanmgr.c, which then performs an upcall to this file.
 *
 * 3) It reports the current operational state of the RPS.  This is done
 *    upon request of a Lan Network Manager.
 *
 **********************************************************************/

#include "master.h"
#include <ctype.h>
#undef   tolower                        /* use library function call */
#include <string.h>
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
static uchar rps_version[VERSIONLEN] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0};
static char *const rps_names[] = {
    "RPS 8200", "Request RPS Status", "Report RPS Status", "RPS Error",
    "RPS 8204", "RPS 8205", "RPS Report Station in Ring"};


/**********************************************************************
 *
 *	       Convert Internal RPS info various ways.
 *
 **********************************************************************/

#define NAME(x) rps_names[(x) & COMMAND_MASK]

static char *rps_to_string (
    int command)
{
    if (command < RPS_LAST)
	return(rps_names[command & COMMAND_MASK]);
    return("unknown");
}

int rpsid_to_bitmask (ushort command)
{
    switch (command) {
      case IBMNM_CORRELATOR:	  return(RPS_B_CORRELATOR);
      case IBMNM_RING:		  return(RPS_B_RING);
      case IBMNM_ALL:		  return(RPS_B_ALL);
      default:			  return(0);
    }
}

char *rpsbit_to_string (int bit)
{
    switch (bit) {
      case RPS_B_CORRELATOR:    return("Correlator");
      case RPS_B_RING:		return("Ring");
      case RPS_B_ALL:           return("All");
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
 * The following routines handle the Ring Parameter Server status
 * reporting.  A lan manager sends the router a Request RPS Status frame.
 * The response to that message is either a Report RPS Status frame or an
 * Error frame.  The status indicates whether the ring is in an
 * operational state, or in a beaconing state.
 *
 * RPS Request Status (I) -->
 * a) <-- RPS Report Status (I)
 * b) <-- RPS Error (I)
 *
 */

/*
 * rps_report_status (8202)
 *
 * Send a Report Status Message back to the requesting lan manager.
 */
void rps_report_status (bridge_info *bridge, int link, rps_pb *result)
{
    paktype *pak;
    ibmnm_vector *vector, *sv;
    ring_info *ring;

    ring = ringno_to_ring(*result->ring);
    if (!ring) return;
    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    sv = insert_major(pak, RPS_REP_STATUS);
    sv = insert_correlator(sv, *result->correlator);
    sv = insert_ring(sv, *result->ring);
    sv = insert_bytes(sv, IBMNM_VERSION, rps_version, VERSIONLEN);
    sv = insert_short(sv, RPS_SV_TIMER, ring->softerr_timer);
    vector->length = (uchar *)sv - (uchar *)vector;

    lrm_send_link(pak, bridge, link, NAME(RPS_REP_STATUS));
}

/*
 * rps_error_frame (8203)
 *
 * Send a RPS Error Frame.  This message indicates that a command
 * received from the Lan Manager contained a syntax error, or the command
 * could not be completed sucessfully.
 */
void rps_error_frame (
    bridge_info *bridge,
    int link,
    int correlator,
    int code,
    int offset)
{
    paktype *pak;
    ibmnm_vector *vector, *sv;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    sv = insert_major(pak, RPS_ERROR);
    sv = insert_correlator(sv, correlator);
    sv = insert_short(sv, IBMNM_ERRCODE, code);
    sv = insert_short(sv, IBMNM_ERROFFSET, offset);
    sv = insert_bytes(sv, IBMNM_VERSION, rps_version, VERSIONLEN);
    vector->length = (uchar *)sv - (uchar *)vector;

    lrm_send_link(pak, bridge, link, NAME(RPS_ERROR));
}

/**********************************************************************
 *
 *			Upcalls from MAC level
 *
 **********************************************************************/

/*
 * rps_report_station (8206)
 *
 * Send a Report Station In Ring Message to any interested Lan Managers.
 * This is an upcall from the mac packet processing code.  The correct
 * status value (success/failure) has already been stored in the
 * sttach_status field, so it simply needs to be passed on.
 */
void rps_report_station (station_info *info)
{
    paktype *pak;
    ibmnm_vector *vector, *sv;
    idbtype      *swidb = firstsw_or_null(info->idb);

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    sv = insert_major(pak, RPS_REP_STATION);
    sv = insert_ring(sv, srbV.srb_sb_get_thisring(swidb));
    sv = insert_address(sv, info->address);
    sv = insert_naun(sv, info->naun);
    sv = insert_productid(sv, info->product_id);
    sv = insert_bytes(sv, RPS_SV_UCODE, info->ucode, UCODELEN);
    sv = insert_short(sv, RPS_SV_STATUS, info->attach_status);
    vector->length = (uchar *)sv - (uchar *)vector;

    lrm_send_ring(pak, info->idb, NAME(RPS_REP_STATION));
}

/**********************************************************************
 *
 *			       Parsers
 * 
 **********************************************************************/

/*
 * rps_parse_vectors
 *
 * Parse the subvectors in a major vector sent to the RPS.  This routine
 * checks to insure that all required vectors are present.  It also
 * insures that the major vector contains only the required and optional
 * subvectors that are permissible for this particular major vector.
 * This routine builds and returns a data structure that indicates which
 * subvectors were present, and what their values were.
 */
boolean rps_parse_vectors (
    ibmnm_vector *vector,
    ulong required,
    ulong optional,
    rps_pb *result,
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
	bit = rpsid_to_bitmask(GETSHORT(&sv->command));
	present |= bit;

	/*
	 * Is it allowed?
	 */
	if (!(required & bit) && !(optional & bit)) {
	    lnmllcbuginf(unxpctvec, rpsbit_to_string(bit));
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
	  case IBMNM_RING:	 result->ring	    = (ushort *)data; break;
	  case IBMNM_ALL:	 result->all	    = (ushort *)data; break;
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
	lnmllcbuginf(missvec, rpsbit_to_string(missing));
	*error = ERR_REQ_SV_MISSING;
	*offset = sv;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * ibmnm_rps_input
 *
 * All input frames that are directed to the Ring Parameter Server
 * ends up here.  Parse them, query the ring for information if necessary,
 * and then return the requested information to the Lan Manager.
 */
void ibmnm_rps_input (llctype *cookie, ibmnm_vector *vector)
{
    bridge_info *bridge;
    ushort correlator;
    int link;
    int error;
    rps_pb *result;
    ibmnm_vector *offset;
    srb_triplet_t srb_triplet;

    /*
     * Housekeeping
     */
    if (!cookie_to_link(cookie, &bridge, &link)) {
	buginf(nolink, cookie);
	return;
    }
    if (!(result = malloc_named(sizeof(rps_pb), lnm_cb)))
	return;
    srbV.srb_sb_get_triplet(bridge->idb1->firstsw, &srb_triplet);
    ibmnm_print_bridge_rcvd_msg(&srb_triplet,
				rps_to_string(GETSHORT(&vector->command)),
				bridge, link); 
    error = 0;

    switch (GETSHORT(&vector->command)) {
      case RPS_REQ_STATUS:
	if (!ibmnm_parse_request_status(vector, &correlator, &error, &offset)) {
	    rps_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	if (!rps_parse_vectors(vector, RPS_R_REQ_STATUS, RPS_O_REQ_STATUS,
			       result, &error, &offset)) {
	    rps_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	rps_report_status(bridge, link, result);
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
 * 		   Ring Parameter Server Test Code
 *
 * All code past here implements the Lan Network Manager side of this
 * protocol.
 *
 **********************************************************************/
#include "../llc2/llc2.h"
#include "ttysrv.h"

/*
 * Build and send a "Request RPS Status" message.  This should elicit a
 * "Report RPS Status" response.
 */
static void tibmnm_req_rps_status (
    jmp_buf *far)
{
    jmp_buf near;
    paktype *pak;
    ibmnm_vector *vector, *sv;
    int ring, all;

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

    sv = insert_major(pak, RPS_REQ_STATUS);
    if (yes_or_no2(&near, "Use Correlator", TRUE, TRUE))
	sv = insert_correlator(sv, test_correlator++);
    if (yes_or_no2(&near, "Use Ring", TRUE, TRUE)) {
	ring = get_num_in_range2(&near, "    Ring", 16, 0, 65535, 0);
	sv = insert_ring(sv, ring);
    }
    if (yes_or_no2(&near, "Include 'All'", TRUE, TRUE)) {
	all = get_num_in_range2(&near, "    All", 16, 0, 65535, 0);
	sv = insert_short(sv, IBMNM_ALL, all);
    }
   
    sv = tibmnm_add_arbitrary(&near, sv);
    vector->length = (uchar *)sv - (uchar *)vector;
    ibmnm_llc2_send(test_cookie, pak, vector->length);
}

/*
 * Ask which RPS command to send.
 */
void tibmnm_rps (jmp_buf *far)
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
	process_sleep_for(ONESEC);
	printf("What type of RPS packet would you like to send?\n\n");
	printf("0) quit\n");
	printf("1) Request RPS Status\n");
	if (!rdtty(buffer, IBMNM_MAXBUF))
	    longjmp(far, 1);
	
	if (null(buffer))
	    continue;

	switch (tolower(*buffer)) {
	  case '0': done = TRUE;                  break;
	  case '1': tibmnm_req_rps_status(&near); break;
	}
    }
}
#endif
