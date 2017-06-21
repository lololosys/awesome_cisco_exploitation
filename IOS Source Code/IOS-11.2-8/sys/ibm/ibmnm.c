/* Id: ibmnm.c,v 3.5.18.8 1996/08/28 12:46:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/ibmnm.c,v $
 *------------------------------------------------------------------
 * ibmnm.c -- IBM Network Management Protocol
 *
 * January 1991, David R. Hampton
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibmnm.c,v $
 * Revision 3.5.18.8  1996/08/28  12:46:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.18.7  1996/08/26  15:07:52  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.5.18.6  1996/08/07  08:59:18  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.18.5  1996/08/03  23:25:23  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.5.18.4  1996/07/23  13:23:15  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.5.18.3  1996/07/09  05:57:44  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.5.18.2  1996/05/17  11:20:05  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.4.4  1996/04/26  15:19:31  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.4.3  1996/04/26  07:53:55  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.4.2  1996/04/18  16:16:44  ioakley
 * CSCdi53954:  Update ibmnm_shutdown related error messages and
 *              handle llc2 disconnection timeout condition.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.4.1  1996/04/03  14:33:35  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.18.1  1996/03/18  20:11:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.3  1996/03/13  01:23:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.2.2  1996/03/07  09:41:28  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  00:54:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/08  17:57:16  ioakley
 * CSCdi47561: Resolve potential network congestion problems by adding
 *             a global configuration command to disable the automatic
 *             Report Path Trace function and also correct problems with
 *             Report Path Trace frames returned to originating stations.
 *
 * Revision 3.4  1996/01/18  22:51:34  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/16  22:06:37  motto
 * CSCdi35920:  %ALIGN-3-CORRECT messages on 4500
 *
 * Revision 3.2  1995/11/17  09:22:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:01:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/07/13  20:32:53  jacobt
 * CSCdi36703:  replace malloc() by malloc_named()
 *
 * Revision 2.5  1995/06/18  23:36:38  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/15  22:00:10  hampton
 * Fix 24.5 day bugs in Lan Manager support.  While there, eliminate
 * direct references to msclock.  [CSCdi35914]
 *
 * Revision 2.3  1995/06/10 00:02:03  alam
 * CSCdi35637:  LNM cannot set bridge parameters on FDDI
 * Verify port id on FDDI using bitswapped hardware address.
 *
 * Revision 2.2  1995/06/09  13:05:04  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:46:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *
 * IBM's Network Management Protocol, as described in the IBM Token
 * Ring Network Architecture Reference Manual, Third Edition, September
 * 1989, Chapters 14 through 18.
 *
 * This code is divided into five modules based on the five servers
 * described in the IBM Archivexture Manual.  These files are names
 * ibmnm_xxx.c.  These are two other files present.  THese are ibmnm.c
 * (this file) which contains core functions.  The other file is lanmgr.c
 * which andles the MAC layer interface for all five servers.
 *
 **********************************************************************/

/* #define LLC2_TEST */

#include "master.h"
#include <ctype.h>
#undef   tolower			/* use library function call */
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "../ibm/msg_lanmgr.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "ibmnm_debug.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "../if/rif.h"
#include "../if/network.h"
#include "lanmgr.h"
#include "ibmnm.h"
#include "../llc2/llc2.h"
#include "../srt/srb_core.h"
#include "../srt/srb.h"
#include "ibm_public.h"

/*
 * Externals
 */
extern queuetype ringQ;
extern queuetype lanmgrQ;
extern boolean lanmgr_global_enable;

/*
 * Global Storage for ibmnm_xxx.c.
 */
const char respvec[] =   "\nIBMNM: received response vector %#x as a command.";
#ifdef NOBODY_CALLS_ME
const char unimpvec[] =  "\nIBMNM: received unimplemented command vector %#x.";
#endif /* NOBODY_CALLS_ME */

const char unknvec[] =     "\nIBMNM: unknown subvector (%#2x) received.";
const char unxpctvec[] =   "\nIBMNM: unexpected %s subvector received.";
const char missvec[] =     "\nIBMNM: missing required subvector(s) %s.";
const char toosmallvec[] = "\nIBMNM: vector %#x length too small.";
const char allvec[] =      "\nIBMNM: vector %#x has information after 'all'.";
const char badveclen[] =   "\nIBMNM: bad length for (sub)vector %#x.";

const char nolink[] =    "\nIBMNM: No reporting link associated with this connection (%#x).";
const char illegalui[] = "\nIBMNM: UI frame contains illegal %s command (%#x).";

const char lnm_cb[]    = "LNM CB";
queuetype      ibmnmQ;
queuetype      lm_bridgeQ;
boolean	       ibmnm_no_changes;

#ifdef IBMNM_DEBUG
queuetype tibmnmQ;
static boolean ibmnm_testing = FALSE;
#endif


/**********************************************************************
 *
 *		       Miscellaneous Functions
 *
 **********************************************************************/

/*
 * ibmnm_getbuffer 
 *
 * Get a buffer for a new frame and set various common values.
 */
paktype *ibmnm_getbuffer (int size)
{
    paktype *pak;

    pak = getbuffer(size);
    if (pak) {
	pak->datagramstart = ibmnm_vector_headstart(pak);
	pak->datagramsize = size;
    }
    return(pak);
}

/*
 * ibmnm_enqueue 
 *
 * A frame was received that is destined for the Lan Network Manager SAP
 * (0xF4).  This may be either an Unnumber Information frame, or an
 * Information frame from an LLC2 connection.  Queue it for the
 * background task to process when it runs.
 */
void ibmnm_enqueue (paktype *pak)
{
    trrif_t *trh;
    paktype *newpak;
    idbtype *swidb = NULL;
    hwidbtype *idb = NULL;
    sap_hdr *sap;
    
#ifdef LLC2_TEST
    if (llc2_debug_test)
	test_lanmgr_enqueue(pak);
    else
#endif LLC2_TEST
#ifdef IBMNM_DEBUG
    if (ibmnm_testing) {
/*	lnmebuginf("\nIBMNM: Enqueued packet %#x", pak); */
	p_enqueue(&tibmnmQ, pak);
    } else
#endif IBMNM_DEBUG

    swidb = pak->if_input;
    idb = swidb->hwptr;
    /*
     * Determine whether this is a LLC frame or TR header.
     * If this is not a LLC frame, we need to check whether
     * this is a broadcast packet or not.
     */
    sap = (sap_hdr *)pak->info_start;
    if (pak->llc2_cb == NULL) {
	trh = (trrif_t *) pak->datagramstart;
	if ((trh->saddr[0] & TR_RII) && (trh->rc_blth & RC_BROADBIT)) {
	    newpak = pak_duplicate(pak);
	    if (newpak)
		srb_enq(srb_explorerQ, newpak);
	    if (lnm_e_debug) {
	        if (!idb)
		    buginf("\nIBMNM: NULL idb for IBMNM explorer");
		else if (newpak) {
                    srb_triplet_t srb_triplet;
                    srbV.srb_sb_get_triplet(swidb, &srb_triplet);
		    buginf("\nIBMNM%d: explorer enqueued (srn %d bn %d trn %d)",
                             idb->unit, 
                             srb_triplet.localRing,
                             srb_triplet.bridgeNum,
                             srb_triplet.remoteRing);
                }
	    }
	}
    }

    if (lanmgr_pid) {
	p_enqueue(&ibmnmQ, pak);
    } else {
	protocol_discard(pak, TRUE);;
    }
}

/*
 * create_link
 *
 * The bridge received a "Set Reporting Point" from from a Lan Network
 * Manager.  Create a new reporting link entry on the appropriate bridge
 * data structure.
 */
void create_link (
    bridge_info *bridge,
    int link,
    llctype *cookie,
    uchar *address,
    hwidbtype *idb)
{
    ieee_copy(address, bridge->links[link].address);
    bridge->links[link].idb = idb;
    bridge->links[link].cookie = cookie;
    bridge->links[link].reporting_idb1 = REPORT_ALL;
    bridge->links[link].reporting_idb2 = REPORT_ALL;
}

/*
 * destroy_link
 *
 * The bridge received a "Close Reporting Point" from from a Lan Network
 * Manager.  Destruy the existing reporting link entry on the appropriate
 * bridge data structure.
 */
void destroy_link (bridge_info *bridge, int link)
{
    ieee_zero(bridge->links[link].address);
    bridge->links[link].idb = NULL;
    bridge->links[link].cookie = NULL;
    bridge->links[link].controlling = FALSE;
}

/*
 * ibmnm_print_bridge_rcvd_msg
 *
 * Get the information and print out this message in one place instead
 * of cut and pasting the same glop in a lot of files.  Save some code
 * size while we are at it.
 */
void ibmnm_print_bridge_rcvd_msg(srb_triplet_t *srb_triplet,
				 char          *what,
				 bridge_info   *bridge,
				 int            linkno) 
{
    lnmllcbuginf("\nIBMNM: Bridge %03x-%x-%03x received %s from %e.",
             srb_triplet->localRing,
             srb_triplet->bridgeNum,
             srb_triplet->remoteRing,
             what, bridge->links[linkno].address);
}

/*
 * ibmnm_link_state_change
 *
 * LLC2 just notified us that the connection to a Lan Network Manager
 * disappeared.  Destroy all data structures related to this connection,
 * and then check to see if we have a new controlling Lan Network
 * Manager.
 */
void ibmnm_link_state_change (llctype *cookie, int reason)
{
    bridge_info *bridge;
    char *what;
    int linkno;
    idbtype *swidb;
    srb_triplet_t srb_triplet;

    what = llc2_prreason(reason);
    if (!cookie_to_link(cookie, &bridge, &linkno)) {
	if ((reason != LC_DISC) && (reason != LC_DISC_LAN) &&
	    (reason != LC_DISC_CNF)) {
	    /*
	     * Can't complain here.  This code tosses it's pointer to the
	     * LLCS2 connection as soon as it requests the disconnect.
	     */
	    lnmllcbuginf("\nIBMNM: LLC2 connection (%#x) closed by %s.",
			 cookie, what);
	}
	return;
    }

    swidb = firstsw_or_null(bridge->idb1);
    srbV.srb_sb_get_triplet(swidb, &srb_triplet);
    ibmnm_print_bridge_rcvd_msg(&srb_triplet, what, bridge, linkno);

    switch (reason) {
      case LC_DM:
      case LC_DISC_LAN:
      case LC_DISC_CNF:
      case LC_DISC_TIMEOUT:
      case LC_TIMEOUT:
      case LC_INTDOWN:
      case LC_FRMR:
	unset_rif(bridge->links[linkno].idb, 0, 
				    bridge->links[linkno].address, NULL, FALSE);
	destroy_link(bridge, linkno);
	lnmllcbuginf("\nIBMNM: LLC2 connection (%#x) closed by %s.",
			cookie, what);
	lrm_find_controlling(bridge, reason ? LRM_CNTL_ABORT: LRM_CNTL_CLOSE);
	break;

      case LC_RESET:
      case LC_ABORTED:
	break;

      default:
	destroy_link(bridge, linkno);
	errmsg(&msgsym(BADUPCALL, LANMGR), what, 
               srb_triplet.localRing,
               srb_triplet.bridgeNum,
               srb_triplet.remoteRing);
	return;
    }
}

/*
 * ibmnm_display_vector
 *
 * Print out a single vector from an IBM Network management frame.  This
 * routine will call itself recursively if the vector is a complex
 * vector.  It indentes the output two spaces for every level of
 * recursion to make these complex vectors easier to read.
 */
static void ibmnm_display_vector (
    ibmnm_vector *sv,
    int indent)
{
    int i, length, offset;
    uchar *ptr;
    char buffer[80];

    offset = sprintf(buffer, "\n%*c%04x %04x ", indent, ' ',
		     GETSHORT(&sv->length), GETSHORT(&sv->command));
    
    if (GETSHORT(&sv->command) & IBMNM_ATOMIC) {
	ptr = (uchar *)&sv->data;
	for (i = 0; i < GETSHORT(&sv->length) - 4; i++) {
	    sprintf(buffer+offset, "%02x", ptr[i]);
	    offset += 2;
	    if (offset > 75) {
		buginf(buffer);
		offset = sprintf(buffer, "\n%*c          ", indent, ' ');
	    }
	}
	buginf(buffer);
	return;
    }

    buginf(buffer);
    length = GETSHORT(&sv->length) - 4;
    if (length <= 0)
	return;
    sv = (ibmnm_vector *)&sv->data;
    for ( ; length > 0; length -= GETSHORT(&sv->length), sv = next_subvector(sv)) {
	ibmnm_display_vector(sv, indent+2);
    }
}

/*
 * ibmnm_display_packet
 *
 * Print out an entire IBM Network management frame.  This routine will
 * calls ibmnm_display_vector, which will iterate over the entire frame
 * until all vector have benn printed.
 */
static void ibmnm_display_packet (
    paktype *pak)
{
    uchar address[IEEEBYTES];
    int indent, dummy1;
    ibmnm_vector *vector, *dummy2;

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    indent = 0;
    find_802addr(pak, address, GET_SOURCE);
    buginf("\nFrame (%#x) received command %#x from %e.\n", pak,
	   GETSHORT(&vector->command), address);
    if (!ibmnm_sum_lengths(vector, &dummy1, &dummy2)) {
	buginf("\nVector length mismatch.\n", address);
	return;
    }
    if (GETSHORT(&vector->length) == 0) {
	buginf("\nZero length frame.\n", address);
	return;
    }
    ibmnm_display_vector(vector, 0);
}

/*
 * ibmnm_changes_allowed
 *
 * Are any changes allowed by this particular Lan Network Manager?  This
 * routine takes into consideration whether or not the requesting Lan
 * Network Manager is the controlling Lan Network Manager.  It also takes
 * into consideration whether or not this router is configured to accept
 * changes from Lan Network Managers, or only from SNMP stations.
 */
boolean ibmnm_changes_allowed (bridge_info *bridge, int link)
{
    if (ibmnm_no_changes)
	return(FALSE);
    if (!bridge->links[link].controlling)
	return(FALSE);
    return(TRUE);
}

/*
 * valid_port
 *
 * Is this port address (mac address) part of this bridge.  This should
 * be checked for any request from Lan Network Manager that is acting
 * upon a specific interface.
 */
hwidbtype *valid_port (bridge_info *bridge, uchar *address)
{
    /* address is MSB format */
    if (ieee_equal_sans_rii(address, 
			    is_fddi(bridge->idb1) ? 
			    bridge->idb1->bit_swapped_hardware :
			    bridge->idb1->hardware))
	return(bridge->idb1);
    if (ieee_equal_sans_rii(address, 
			    is_fddi(bridge->idb2) ?
			    bridge->idb2->bit_swapped_hardware :
			    bridge->idb2->hardware))
	return(bridge->idb2);
    lnmllcbuginf("\nIBMNM: invalid port id %e", address);
    return(NULL);
}

/**********************************************************************
 *
 *			 Generic Parsers Aids
 *
 **********************************************************************/

/*
 * ibmnm_sum_lengths
 *
 * Walk an entire vector computing the compating the length of each
 * complex subvector to the sum of the lengths of its component parts.
 * This is mainly a consistency check to insure that a bogus request does
 * not crash the system.  This routine should be called for every frame
 * received from a Lan Network Manager.
 */
boolean ibmnm_sum_lengths (
    ibmnm_vector *vector,
    int *error,
    ibmnm_vector **offset)
{
    ibmnm_vector *sv;
    int length;
    ushort svlen;

    length = GETSHORT(&vector->length) - IBMNM_MVID_LEN;
    sv = (ibmnm_vector *)&vector->data;

    for ( ; length > 0; ) {
	if (!(GETSHORT(&sv->command) & IBMNM_ATOMIC)) {
	    if (!ibmnm_sum_lengths(sv, error, offset))
		break;
	}
        /*
         * the length of an indiviual sub-vector MUST be bigger
         * than IBMNM_MVID_LEN (4 byte, len + code).
         * the overall length of all sub-vectors MUST also
         * be bigger or equal the indivdual sub-vector len
         */
        svlen = GETSHORT(&sv->length);
        if ((svlen < IBMNM_MVID_LEN) || (length < svlen)) {
            lnmllcbuginf(toosmallvec, GETSHORT(&sv->command));
            *error = ERR_BAD_LEN;
            *offset = vector;
            return(FALSE);
        }
        length -= svlen;
        sv = next_subvector(sv);
    }
    if (length != 0) {
	lnmllcbuginf(badveclen, GETSHORT(&vector->command));
	*error = ERR_BAD_LEN;
	*offset = vector;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * ibmnm_parse_correlator
 *
 * Search a vector for the correlator number, returning TRUE if found,
 * FALSE otherwise.  The correlator is returned in a passed argument.
 * This routine is called for any vector that fails to parse correctly so
 * that a complaint can be filed about a specific frame.
 */
boolean ibmnm_parse_correlator (ibmnm_vector *vector, ushort *correlator)
{
    ibmnm_vector *sv;
    int length;

    length = GETSHORT(&vector->length) - IBMNM_MVID_LEN;
    sv = (ibmnm_vector *)&vector->data;

    for ( ; length > 0; length -= GETSHORT(&sv->length), sv = next_subvector(sv)) {
	if (GETSHORT(&sv->command) == IBMNM_CORRELATOR) {
	    *correlator = GETSHORT(&sv->data);
	    return(TRUE);
	}
    }
    *correlator = 0;
    return(FALSE);
}

/*
 * ibmnm_parse_request_status
 *
 * Inspect any "Request Status" frame to make sure it is formatted
 * correctly.  All request status vectors must include a correlator.  If
 * they include an "all" vector, there must not be anything else present
 * in the frame after that vector.
 */
boolean ibmnm_parse_request_status (
    ibmnm_vector *vector,
    ushort *correlator,
    int *error,
    ibmnm_vector **offset)
{
    ibmnm_vector *sv;
    int length;
    boolean all;;

    all = FALSE;
    *correlator = 0;
    length = GETSHORT(&vector->length) - IBMNM_MVID_LEN;
    sv = (ibmnm_vector *)&vector->data;

    /*
     * Check total length
     */
    if (length <= 0) {
	lnmllcbuginf(toosmallvec, GETSHORT(&vector->command));
	*error = ERR_LEN_TOO_SMALL;
	*offset = vector;
	return(FALSE);
    }

    /*
     * Check top level subvectors for "Correlator" and "All".  The "all"
     * vector must be the last vector if present.
     */
    for ( ; !all && length > 0; length -= GETSHORT(&sv->length), sv = next_subvector(sv)) {
	switch (GETSHORT(&sv->command)) {
	  case IBMNM_CORRELATOR:
	    *correlator = GETSHORT(&sv->data);
	    break;
	    
	  case IBMNM_ALL:
	    all = TRUE;
	    break;

	  case LBS_SV_ALL_LONG:
	    if ((GETSHORT(&vector->command) & SERVER_MASK) == LBS_SERVER)
		all = TRUE;
	    break;
	    
	  default:
	    break;
	}
    }

    /*
     * Any problems so far?
     */
    if (!*correlator) {
	lnmllcbuginf(missvec, "correlator");
	*error = ERR_REQ_SV_MISSING;
	*offset = vector;
	return(FALSE);
    }
    if (all && (length > 0)) {
	lnmllcbuginf(allvec, GETSHORT(&vector->command));
	*error = REQ_ERR_NO_SV_ALLOWED;
	*offset = sv;
	return(FALSE);
    }

    /*
     * Chek the lengths of each and every subvector.
     */
    if (!ibmnm_sum_lengths(vector, error, offset))
	return(FALSE);
    return(TRUE);
}

/*
 * ibmnm_parse_set_request
 *
 * Inspect any "Set Parameters" request frame to make sure it is
 * formatted correctly.
 */
boolean ibmnm_parse_set_request (
    ibmnm_vector *vector,
    ushort *correlator,
    int *error,
    ibmnm_vector **offset)
{
    ibmnm_vector *sv;		/* working vector */
    int length;
    boolean setflag = FALSE;

    *correlator = 0;
    length = GETSHORT(&vector->length) - IBMNM_MVID_LEN;
    sv = (ibmnm_vector *)&vector->data;

    /*
     * Check total length
     */
    if (length <= 0) {
	*error = ERR_LEN_TOO_SMALL;
	*offset = vector;
	return(FALSE);
    }

    /*
     * Check top level subvectors for "Correlator".
     */
    for ( ; length > 0; length -= GETSHORT(&sv->length), sv = next_subvector(sv)) {
	switch (GETSHORT(&sv->command)) {
	  case IBMNM_CORRELATOR:
	    *correlator = GETSHORT(&sv->data);
	    setflag = TRUE;
	    break;

	  default:
	    break;
	}
    }

    /*
     * Any problems so far?
     */
    if (!setflag) {
	*error = ERR_REQ_SV_MISSING;
	*offset = vector;
	return(FALSE);
    }

    /*
     * Chek the lengths of each and every subvector.
     */
    if (!ibmnm_sum_lengths(vector, error, offset))
	return(FALSE);
    return(TRUE);
}

/**********************************************************************
 *
 *		  Generic Input Processing Routines
 *
 **********************************************************************/

/*
 * ibmnm_bad_input
 *
 * A frame was received for a non-existent server, or a UI frame was
 * received for a server then only processes I frames.  Discard the frame
 * and complain to Lan Network Manager.
 */
void ibmnm_bad_input (llctype *cookie, ibmnm_vector *vector)
{
    bridge_info *bridge;
    ushort correlator;
    int link;

    /*
     * Housekeeping
     */
    if (!cookie_to_link(cookie, &bridge, &link)) {
	buginf(nolink, cookie);
	return;
    }

    ibmnm_parse_correlator(vector, &correlator);
    lrm_invalid_request(cookie, correlator, LRM_INVALID_NOT_AVAIL);
}

/*
 * ibmnm_leg_input
 *
 *  This is the legacy llc2 input routine
 */
void ibmnm_leg_input (void)
{
    paktype *pak;
    ibmnm_vector *vector;
    hwidbtype *idb;
    llctype *cookie;
    uchar srcaddr[IEEEBYTES];
    lnm_t *lnm_ptr = NULL;

    while ((pak = p_dequeue(&ibmnmQ))) {
	cookie = pak->llc2_cb;
/*	buginf("\nIBMNM: Dequeued %s frame (pak %#x).", cookie ? "I" : "UI", pak);*/
	idb = pak->if_input->hwptr;
/*
	if (lanmgr_p_debug)
	    ibmnm_display_packet(pak);
*/

	vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);
	if (cookie) {
	    switch (GETSHORT(&vector->command) & SERVER_MASK) {
	      case REM_SERVER:
		ibmnm_rem_input(cookie, vector);
		break;
		
	      case RPS_SERVER:
		ibmnm_rps_input(cookie, vector);
		break;
		
	      case CRS_SERVER:
		ibmnm_crs_input(cookie, vector);
		break;
		
	      case LBS_SERVER:
		ibmnm_lbs_input(cookie, vector); 
		break;
		
	      case LRM_SERVER:
		ibmnm_lrm_input(cookie, vector);
		break;
		
	      case CNV_SERVER:
	      case ALT_SERVER:
		ibmnm_bad_input(cookie, vector);
		break;
	    }
	} else {
	    switch (GETSHORT(&vector->command) & SERVER_MASK) {
	      case LRM_SERVER:
		ibmnm_lrm_ui_input(pak, lnm_ptr);
		break;
		
	      default:
		if (lnm_llc_debug) {
		    ieee_zero(srcaddr);
		    find_802addr(pak, srcaddr, GET_SOURCE);
		    buginf("\nIBMNM: Rcvd LLC1 packet from %e not for LRM",
			   srcaddr);
		}
 	    }
	}
	retbuffer(pak);
    }
}

/*
 * ibmnm_input
 *
 * This routine drains the queue of packets send to the Lan Network
 * Manager SAP (0xF4).  It routes each packet to the proper server, or
 * discards it, as appropriate.
n */
void ibmnm_input (llctype *cookie, paktype *pak, lnm_t *lnm_ptr)
{
    ibmnm_vector *vector;
    uchar srcaddr[IEEEBYTES];

/*  lmpbuginf("\nIBMNM: Dequeued %s frame (pak %#x).", cookie ? "I" : "UI", pak); */
/*
    if (lanmgr_p_debug)
	    ibmnm_display_packet(pak);
*/

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);
    if (cookie) {
	switch (GETSHORT(&vector->command) & SERVER_MASK) {
	  case REM_SERVER:
	    ibmnm_rem_input(cookie, vector);
	    break;
		
	  case RPS_SERVER:
	    ibmnm_rps_input(cookie, vector);
	    break;
		
	  case CRS_SERVER:
	    ibmnm_crs_input(cookie, vector);
	    break;
		
	  case LBS_SERVER:
	    ibmnm_lbs_input(cookie, vector); 
	    break;
		
	  case LRM_SERVER:
	    ibmnm_lrm_input(cookie, vector);
	    break;
		
	  case CNV_SERVER:
	  case ALT_SERVER:
	    ibmnm_bad_input(cookie, vector);
	    break;
	}
	lnm_ptr->clsi_msg = NULL; /* make sure it's initialized */
    } else {
	switch (GETSHORT(&vector->command) & SERVER_MASK) {
	  case LRM_SERVER:
	    ibmnm_lrm_ui_input(pak, lnm_ptr);
	    lnm_ptr->clsi_msg = NULL; /* make sure it's initialized */
	    break;
		
	  default:
	    if (lnm_llc_debug) {
	        ieee_zero(srcaddr);
	        find_802addr(pak, srcaddr, GET_SOURCE);
		buginf("\nIBMNM: Rcvd LLC1 packet from %e not for LRM",
			   srcaddr);
	    }
	    /* delete entry if not for us, otherwise we leak it ...  */
            lnm_delete_entry(lnm_ptr);
 	}
    }
}

/*
 * ibmnm_background
 *
 * Perform periodic processing on a bridge data structure.  This routine
 * returns TRUE if it needs to be waken up again at the next interval, or
 * FALSE if all processing has completed and it does not need to be
 * awakened again.
 */
sys_timestamp ibmnm_background (void)
{
    bridge_info *bridge;
    sys_timestamp wakeuptime;

    TIMER_STOP(wakeuptime);
    for (bridge = (bridge_info *)lm_bridgeQ.qhead; bridge; bridge = bridge->next) {
	if (TIMER_RUNNING(bridge->announce_time)) {
	    if (!SLEEPING(bridge->announce_time)) {
		lrm_mgmt_servers_present(bridge);
		if (bridge->announce_count-- == 0) {
		    TIMER_STOP(bridge->announce_time);
		    continue;
		}
		TIMER_UPDATE(bridge->announce_time, LM_DEF_ANNOUNCE_TIME);
	    }
	    wakeuptime = TIMER_SOONEST(wakeuptime, bridge->announce_time);
	}
    }
    for (bridge = (bridge_info *)lm_bridgeQ.qhead; bridge; bridge = bridge->next) {
	if (TIMER_RUNNING(bridge->notification_time)) {
	    if (!SLEEPING(bridge->notification_time)) {
		lbs_counter_report(bridge);
		TIMER_UPDATE(bridge->notification_time, bridge->notification_interval);
	    }
	    wakeuptime = TIMER_SOONEST(wakeuptime, bridge->notification_time);
	}
    }
    return(wakeuptime);
}

/*
 * ibmnm_init
 *
 * Initialize bridge specific data structures.  Ring specific data
 * structures are initialized in lanmgr.c
 */
void ibmnm_init (void)
{
    /* Initialize debugging flags */
    ibmnm_debug_init();

    queue_init(&lm_bridgeQ, 0);
    queue_init(&ibmnmQ, 0);
    queue_init(&tibmnmQ, 0);
    queue_init(&lanmgrQ, 0);
    queue_init(&ringQ, 0);
    ibmnm_no_changes = FALSE;

    /*
     * Register some functions
     */
    reg_add_hwif_goingdown(ibmnm_shutdown, "ibmnm_shutdown");
    reg_add_hw_idb_init(ibmnm_init_hwidb, "ibmnm_init_hwidb");
    reg_add_will_reload(ibmnm_reload_message, "ibmnm_reload_message");
}

/**********************************************************************
 * 
 *			Configuration Routines
 *
 **********************************************************************/

/*
 * ibmnm_add_bridge
 *
 * Build and initialize a data structure for a newly created bridge.  The
 * bridge must be a complete bridge.  (I.E. Both interfaces of a two port
 * bridge must have been configured.)  Passwords, et al, will be added by
 * comands found later in the configuration file, or typed later from the
 * user.
 */
void ibmnm_add_bridge (hwidbtype *idb)
{
    bridge_info *bridge;
    hwidbtype *idb2;
    int link;
    ushort srb_thisring, srb_thisring2;

    /*
     * Consistency checking.  Should the this be considered a hard failure?
     */
    if (!idb->ring_info) {
	buginf("\nIBMNM: internal error: no ring structure exists for %s",
	       idb->hw_namestring);
	return;
    }
    if (idb_to_bridge(idb))
	return;
    
    /*
     * Make sure we are a fully configured bridge.  This means that BOTH
     * SIDES of the bridge have been configured, i.e. both interfaces or
     * an interface and a vring.  Until that time, we will not touch it,
     * nor will the SRB code.
     */
    if (!srbV.srb_sb_get_bridging_enabled(idb->firstsw))
	return;
    if (!idb->tr_bridge_idb && !idb->vidb_link)
	return;
    idb2 = (idb->tr_bridge_idb ? idb->tr_bridge_idb : idb->vidb_link);

    /*
     * Finally, build and populate the data structure.  Make sure the
     * ordering is consistent.  The lower ring number (real or virtual)
     * is always the first port of a bridge.
     */
    bridge = malloc_named(sizeof(bridge_info), lnm_cb);
    if (!bridge) {
	return;
    }    
    srb_thisring  = srbV.srb_sb_get_thisring(firstsw_or_null(idb));
    srb_thisring2 = srbV.srb_sb_get_thisring(firstsw_or_null(idb2));
    bridge->idb1 = ((srb_thisring < srb_thisring2) ? idb  : idb2);
    bridge->idb2 = ((srb_thisring < srb_thisring2) ? idb2 : idb);

    /*
     * LRM Related numbers
     */
    bridge->num_alt_lanmgrs = LM_DEF_ALT_LMS;
    bridge->announce_count = LM_DEF_ANNOUNCE_COUNT;
    TIMER_START(bridge->announce_time, LM_DEF_ANNOUNCE_TIME);
    set_lanmgr_timer(bridge->announce_time);

    /*
     * LBS Related numbers
     */
    bridge->frame_lost_thresh = LM_DEF_LOSS_THRESH;
    bridge->notification_interval = LM_DEF_NOTIF_INT;
    bridge->calculation_interval = LM_DEF_CALC_INT;

    /*
     * Link dependant numbers
     */
    for (link = 0; link < MAXLINKS; link++) {
	bcopy("00000000", bridge->links[link].key, LMKEYLEN);
	bcopy("00000000", bridge->links[link].newkey, LMKEYLEN);
	/* LRM per link */
	bridge->links[link].reporting_idb1 = REPORT_ALL;
	bridge->links[link].reporting_idb2 = REPORT_ALL;
    }
    enqueue(&lm_bridgeQ, bridge);
}

/*
 *
 */
void set_ibmnm_interfaces (hwidbtype *newidb, boolean enable)
{
    bridge_info *b, *next;
    ring_info *ring;
    hwidbtype *idb;

    if (!lanmgr_global_enable)
	return;
    /*
     * Fix the ring modes on this interface
     */
    if (newidb) {
	if (enable) {
	    /*
	     * We cannot simply turn on the RPS.  We must make sure that all
	     * RPSs on the ring are using the same ring number.  The right
	     * thing will happen hen the interface is reset by our caller.
	     */
	    newidb->tr_ring_mode |= (RINGMODE_NETMGR | RINGMODE_REM | RINGMODE_RPS);
	    ring = idb_to_ring(newidb);
	    if (ring)
		ring->rps_state = RPS_STATE_DETERMINE;
	    lanmgr_start();
	} else {
	    newidb->tr_ring_mode &= ~(RINGMODE_NETMGR | RINGMODE_REM | RINGMODE_RPS);
	}
    }

    /*
     * Check interfaces and see if any ring numbers have changed.
     */
    FOR_ALL_HWIDBS(idb) {
	if (!is_srb_hw(idb))
	    continue;
	if ((idb->ring_info == NULL)  ||
	    (srbV.srb_sb_get_thisring(idb->firstsw) != idb->ring_info->ringno))
        {
	    lanmgr_destroy_ring(idb);
	    lanmgr_create_ring(idb);
	}
    }

    /*
     * Check current bridges to see if anything has changed.
     */
    for (b = (bridge_info *)lm_bridgeQ.qhead; b; ) {
        idbtype *swidb1 = firstsw_or_null(b->idb1);
        idbtype *swidb2 = firstsw_or_null(b->idb2);
        srb_triplet_t srb_triplet1;
        srb_triplet_t srb_triplet2;
        srbV.srb_sb_get_triplet(swidb1, &srb_triplet1);
        srbV.srb_sb_get_triplet(swidb2, &srb_triplet2);
	next = b->next;
	if ((srb_triplet1.localRing != srb_triplet2.remoteRing) ||
	    (srb_triplet2.localRing != srb_triplet1.remoteRing) ||
	    (srb_triplet1.bridgeNum != srb_triplet2.bridgeNum) ) {
	  lrm_terminating(b, LRM_TERM_NORMAL);
	  p_unqueue(&lm_bridgeQ, b);
	  free(b);
	}
	b = next;
    }

    /*
     * Now rebuild them;
     */
    FOR_ALL_HWIDBS(idb) {
	if (!is_srb_hw(idb))
	    continue;
	ibmnm_add_bridge(idb);
    }
}

/*
 * ibmnm_init_hwidb
 * Here to initialize a new hardware idb
 */

void ibmnm_init_hwidb (hwidbtype *hwidb)
{
    set_ibmnm_interfaces(hwidb, FALSE);
}

/**********************************************************************
 * 
 *			   Display Routines
 *
 **********************************************************************/

/*
 * ibmnm_display_bridge
 *
 * Display all of the 'bridge' data structures.  This also displays all
 * information about the bridge excpet the passwords.  It currently
 * assumes that there can only be four links from Lan Network Managers to
 * a bridge.  This is the limit imposed by the IBM Bridge Program v2.1.
 */
void ibmnm_display_bridge (hwidbtype *idb, char *args)
{
    int link;
    bridge_info *b;
    int c[4];
    srb_triplet_t srb_triplet;

    for (b = (bridge_info *)lm_bridgeQ.qhead; b; b = b->next) {
	if (idb && (b->idb1 != idb) && (b->idb2 != idb))
	    continue;
	c[0] = ' '; c[1] = ' '; c[2] = ' '; c[3] = ' ';
	for (link = 0; link < MAXLINKS; link++) {
	    if (link < b->num_alt_lanmgrs)
		c[link] = 'A';
	    if (b->links[link].cookie && b->links[link].controlling)
		c[link] = '*';
	}
        srbV.srb_sb_get_triplet(b->idb1->firstsw, &srb_triplet);
	printf("\nBridge %03x-%x-%03x, Ports %e, %e\n", 
              srb_triplet.localRing,
              srb_triplet.bridgeNum,
              srb_triplet.remoteRing,
              b->idb1->hardware, b->idb2->hardware);
	printf("Active Links: %c%e %c%e %c%e %c%e\n",
	       c[0], b->links[0].address, c[1], b->links[1].address,
	       c[2], b->links[2].address, c[3], b->links[3].address);
	printf("Notification: %d min, Threshold %02d.%02d%%\n", 
	       b->notification_interval/ONEMIN,
	       b->frame_lost_thresh/100, b->frame_lost_thresh%100);
    }
}

/*
 * ibmnm_display_config
 *
 * Display a list of all bridges currrently configured in the router.
 * This will list only those bridge that can support the Lan Network
 * Manager functionality.  This is intended as an aid for Network
 * Managers attempting to configure any kind of bridg that involves a
 * virtual ring.
 */
void ibmnm_display_config (void)
{
    bridge_info *b;

    if (!lm_bridgeQ.qhead) {
	printf("There are no bridges currently configured.\n");
	return;
    }
    
    printf("Bridge(s) currently configured:\n\n");
    for (b = (bridge_info *)lm_bridgeQ.qhead; b; b = b->next) {
	printf("    From     ring %03x, address %e\n",
	       srbV.srb_sb_get_thisring(b->idb1->firstsw), b->idb1->hardware);
	printf("    Across bridge %03x\n", 
               srbV.srb_sb_get_bridge_num(b->idb1->firstsw));
	printf("    To       ring %03x, address %e\n\n",
	       srbV.srb_sb_get_thisring(b->idb2->firstsw), b->idb2->hardware);
    }
}

#ifdef IBMNM_DEBUG
/**********************************************************************
 *
 *	  Generic IBM Network Management Protocol Test Code
 *
 * All code past here implements the Lan Network Manager side of this
 * protocol.
 *
 **********************************************************************/
#include "ttysrv.h"

llctype *test_cookie = NULL;
ulong test_correlator;
int test_process;

/*
 * get_macaddr
 *
 * Read and parse a MAC address.
 */
void get_macaddr (
    jmp_buf *catch,
    char *prompt,
    uchar *address,
    char *default_address)
{
    char buffer[IBMNM_MAXBUF], *ptr;

    while(TRUE) {
	ptr = read_string2(catch, buffer, prompt, default_address);
	if (parse_ether_address(ptr, address))
	    break;
	printf("Malformed address: %s\n", ptr);
    }
}

/*
 * tibmnm_conn_opened
 *
 * Upcall from LLC2 whan a connection is opened from the remote end.
 */
static void tibmnm_conn_opened (
    llctype *cookie)
{
    printf("\nConnection opened with cookie %#x", cookie);
    test_cookie = cookie;
}

#ifdef NOBODY_CALLS_ME
/*
 * tibmnm_link_state_change
 *
 * Upcall from LLC2 whan a connection is closed from the remote end.
 */
void tibmnm_link_state_change (
    llctype *cookie,
    int reason)
{
    char *what;

    what = llc2_prreason(reason);
    printf("\nConnection closed by %s", what);
    test_cookie = NULL;
}
#endif /* NOBODY_CALLS_ME */

/*
 * lanmgr_BLOCK
 * Block lanmgr_background process if no work available.
 */

static boolean tibmnm_BLOCK (void)
{
    if (tibmnmQ.qhead)
	return(FALSE);
    return(TRUE);
}

/*
 * tibmnm_drain_queue
 *
 * Empty the queue of packets returned from one of the Lan Network
 * Manager Servers in a bridge.
 */
static forktype tibmnm_drain_queue (void)
{
    paktype *pak;

    while (TRUE) {
	edisms((blockproc *)tibmnm_BLOCK, 0);
	while ((pak = p_dequeue(&tibmnmQ))) {
	    ibmnm_display_packet(pak);
	    retbuffer(pak);
	}
    }
}

/*
 * string_to_hex
 *
 * Convert a parsed string to a kex number.
 */
static int string_to_hex (
    char *input,
    void *output)
{
    int length;
    char *iptr, *iptr2;
    ushort *optr;

    iptr = deblank(input);
    optr = (ushort *)output;
    length = 0;

    if (!iptr || null(iptr) || term_char(*iptr))
	return(0);

    for (length = 0; ; length += 2, iptr = iptr2 + 1) {
	iptr2 = get_hex_num(iptr, &optr[length/2]);
	if (term_char(*iptr)) {
	    if (iptr2 - iptr > 2) {
		length += 2;
		break;
	    } else {
		optr[length/2] = optr[length/2] << 8;
		length += 1;
		break;
	    }
	}
    }
    return(length);
}

/*
 * tibmnm_add_arbitrary
 *
 * Create an arbitrary subvector and add it the the vector currently
 * being built.
 */
ibmnm_vector *tibmnm_add_arbitrary (jmp_buf *far, ibmnm_vector *sv)
{
    char buffer[IBMNM_MAXBUF], hex[30], *c;
    jmp_buf near;
    int vector, value, length;

    if (!yes_or_no2(&near, "Add arbitrary vectors", FALSE, TRUE))
	return(sv);

    while (TRUE) {
	vector = get_num_in_range2(&near, "    Vector", 16, 0, 65535, 0);
	if (vector == 0)
	    break;

	c = read_string2(far, buffer, "        Atomic, Byte, Short, Long, sTring, Hex", NULL);
	if (null(c))
	    break;
	switch (tolower(*c)) {
	  case 'a':
	    sv = insert_atomic(sv, vector);
	    break;
	  case 'b':
	    value = get_num_in_range2(&near, "        Value", 16, 0, 255, 0);
	    sv = insert_byte(sv, vector, value);
	    break;
	  case 's':
	    value = get_num_in_range2(&near, "        Value", 16, 0, 65535, 0);
	    sv = insert_short(sv, vector, value);
	    break;
	  case 'l':
	    value = get_num_in_range2(&near, "        Value", 16, 0, -1L, 0);
	    sv = insert_long(sv, vector, value);
	    break;
	  case 't':
	    c = read_string2(far, buffer, "        Value", NULL);
	    sv = insert_bytes(sv, vector, c, strlen(c));
	    break;
	  case 'h':
	    c = read_string2(far, buffer, "        Value", NULL);
	    length = string_to_hex(buffer, hex);
	    sv = insert_bytes(sv, vector, hex, length);
	    break;
	  default:
	    break;
	}
    }
    return(sv);
}

/*
 * tibmnm_top_level
 *
 * Ask which server should be the destination of all frames.
 */
static void tibmnm_top_level (
    jmp_buf *far)
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
	printf("What type of packet would you like to send?\n\n");
	printf("1) Configuration Report Server\n");
	printf("2) LAN Bridge Server\n");
	printf("3) LAN Reporting Manager\n");
	printf("4) Ring Error Monitor\n");
	printf("5) Ring Parameter Server\n");
	if (!rdtty(buffer, IBMNM_MAXBUF))
	    longjmp(far, 1);
	
	if (null(buffer))
	    continue;

	switch (tolower(*buffer)) {
	  case '0': done = TRUE;       break;
	  case '1': tibmnm_crs(&near); break;
	  case '2': tibmnm_lbs(&near); break;
	  case '3': tibmnm_lrm(&near); break;
	  case '4': tibmnm_rem(&near); break;
	  case '5': tibmnm_rps(&near); break;
	}
    }
}

/*
 * ibmnm_test 
 *
 * Query for the information necessary to create a "Set Reporting Point"
 * frame, send it to the bridge, and then enter a loop querying for frame
 * to send.
 */
void ibmnm_test (char *args)
{
    jmp_buf    catch;
    char buffer[IBMNM_MAXBUF];
    uchar bridge_address[IEEEBYTES];
    idbtype *idb;

    /*
     * The abort code.
     */
    if (setjmp(&catch)) {
	ibmnm_testing = FALSE;
	printf("\nAborting IBMNM testing.");
	llc2_register(SAP_IBMNM, 0, NULL);
	if (test_cookie) {
	    llc2_close(test_cookie);
	    test_cookie = NULL;
	}
	if (test_process) {
	    process_kill(test_process);
	    test_process = 0;
	}
	return;
    }

    /*
     *
     */
    llc2_start();
    test_process = cfork(tibmnm_drain_queue, 0L, 1000, "IBMNM Testing", stdio->ttynum);
    llc2_register(SAP_IBMNM, LINK_IBMNM, tibmnm_conn_opened);

    /*
     * Get started
     */
    test_correlator = 1;
    ibmnm_testing = TRUE;
    printf("Creating connection to a bridge\n");
    get_macaddr(&catch, "Bridge address", bridge_address, "0000.3000.ABC4");
    printf("Bridge interface: ");
    if (!rdtty(buffer, IBMNM_MAXBUF))
	longjmp(&catch, 1);
    parse_interface(buffer, &idb);
    if (!idb)
	longjmp(&catch, 1);
    tibmnm_set_reporting_point(&catch, bridge_address, idb->hwptr);

    /*
     * Body
     */
    tibmnm_top_level(&catch);

    /*
     * Clean Up
     */
    llc2_register(SAP_IBMNM, 0, NULL);
    if (test_cookie) {
	llc2_close(test_cookie);
	test_cookie = NULL;
    }
    if (test_process) {
	process_kill(test_process);
	test_process = 0;
    }
}
#endif
