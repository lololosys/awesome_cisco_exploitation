/* $Id: ibmnm_lrm.c,v 3.10.8.10 1996/09/09 17:33:05 asethi Exp $
 * $Source: /release/112/cvs/Xsys/ibm/ibmnm_lrm.c,v $
 *------------------------------------------------------------------
 * ibmnm_lrm.c -- IBM Network Management Protocol
 *                LAN Reporting Manager Functions
 *
 * January 1991, David R. Hampton
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibmnm_lrm.c,v $
 * Revision 3.10.8.10  1996/09/09  17:33:05  asethi
 * CSCdi61211:  System crash with memory leak symtoms
 * Branch: California_branch
 *
 * Revision 3.10.8.9  1996/09/04  23:55:32  snyder
 * CSCdi68120:  save a little memory here and there
 *              declare things that are const, const
 *              savings 300 out of data section
 *                       64 out of image
 * Branch: California_branch
 *
 * Revision 3.10.8.8  1996/08/28  12:46:41  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.8.7  1996/08/26  15:08:03  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.10.8.6  1996/08/16  07:04:09  ioakley
 * CSCdi63385:  Prevent router crash caused by CLS Connect_Req failure
 *              and resolve LNM conn pending/reporting link issues.
 * Branch: California_branch
 *
 * Revision 3.10.8.5  1996/08/07  08:59:26  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.10.8.4  1996/07/09  05:57:54  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.10.8.3  1996/05/21  09:48:48  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.8.2  1996/05/17  11:20:15  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.4.5  1996/04/26  15:19:39  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.4.4  1996/04/26  07:54:03  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.8.4.3  1996/04/10  23:49:30  ioakley
 * CSCdi53954:  In ibmnm_shutdown processing invoke destroy_link
 *              to clean up LNM bridge link structures as needed.
 *              Corrects reporting link set-up failures that occur
 *              after "shut/no shut" interface commands are entered.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.4.2  1996/04/03  14:33:45  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.8.4.1  1996/03/17  17:50:18  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.10.8.1  1996/03/18  20:11:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.4  1996/03/16  06:52:51  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.3  1996/03/13  01:23:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/03/07  09:41:42  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  00:54:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/02/29  23:57:11  ioakley
 * CSCdi47582: Return Report LAN Manager Control Shift frames only on
 *             previously established reporting links and not to the
 *             LNM PC initiating the newly established reporting link
 *             - prevents failures in local LNM reporting link set up.
 *
 * Revision 3.9  1996/02/27  19:29:56  jrosen
 * CSCdi47898:  no lnm disabled crashes router if CIP internal lan exists
 * - Make sure that set_extra_addrs member of the hardware IDB is
 *   non-NULL before calling the function it points to.
 *
 * Revision 3.8  1996/02/08  17:57:20  ioakley
 * CSCdi47561: Resolve potential network congestion problems by adding
 *             a global configuration command to disable the automatic
 *             Report Path Trace function and also correct problems with
 *             Report Path Trace frames returned to originating stations.
 *
 * Revision 3.7  1996/01/18  22:51:37  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/18  22:15:22  ioakley
 * CSCdi41845:  Reliably associate reporting link and connection when
 *              establishing local clsi compliant LNM reporting links
 *
 * Revision 3.5  1995/12/17  18:28:04  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/16  22:06:43  motto
 * CSCdi35920:  %ALIGN-3-CORRECT messages on 4500
 *
 * Revision 3.3  1995/11/17  09:22:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:35:26  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:45:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:01:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/12  18:36:22  ioakley
 * CSCdi41997:  Check address_to_bridge return status and prevent
 *              crash when failing to establish reporting link.
 *
 * Revision 2.4  1995/08/07  05:35:00  richl
 * CSCdi38315:  makefile rework
 *              move utility routines to libibm_util.a
 *
 * Revision 2.3  1995/07/13  20:33:00  jacobt
 * CSCdi36703:  replace malloc() by malloc_named()
 *
 * Revision 2.2  1995/06/22  15:35:52  motto
 * CSCdi36240:  SRB fastswitching and process switching broken for CIP
 * v-interface
 *
 * Revision 2.1  1995/06/07  20:46:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *
 * LAN Reporting Manager Functions, as described in the IBM Token
 * Ring Network Architecture Reference Manual, Third Edition, September
 * 1989, Chapter 14.
 *
 * The LAN Reporting Manager serves several functions.
 *
 * 1) It oversees all connection to/from any Lan Network Manager.
 *    Whenever a connection is established or destroyed, it notifies all
 *    other Lan Network Managers present, and recomputes a new
 *    controlling Lan Network Manager.
 *
 * 2) It reports the curent status of the LRM functions to Lan Network
 *    Manager upon request.
 *
 * 3) It allows the controlling Lan Network Manager to change any of its
 *    operational parameters.
 *
 * 4) It notifies all Lan Network Managers whenever the bridge is being
 *    shut down.  This will occur whenever a bridge is de-configured, or
 *    the router is being reloaded.
 *
 * 5) Notify Lan Network Manager that a bridge is present.  It does this
 *    by sending four messages to the address4000.0000.2000.  This
 *    address just so happens to be the Lan Manager funcional address
 *    with the high order bit turned off.
 *
 **********************************************************************/

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   tolower                        /* use library function call */
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "../if/fddi.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../if/network.h"
#include "address.h"            /* Defines hwaddrtype for clsi_msg.h */
#include "../cls/clsi_msg.h"
#include "../cls/dlc_public.h"
#include "../cls/dlc.h"
#include "lnm_dlu.h"
#include "lanmgr.h"
#include "ibmnm.h"
#include "../llc2/llc2.h"
#include "../srt/rsrb.h"
#include "ibm_public.h"
#include "../srt/srb.h"
#include "../srt/srb_core.h"
#include "../partner/partner_registry.h"
#include "config.h"
#include "../parser/parser_actions.h"
#include "../if/rif_util.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../util/itemlist.h"

/*
 * Externals
 */
extern boolean lanmgr_global_enable;
extern void lnm_send_info_to_disp(lnm_t *lnm_ptr, paktype *pak);
extern void lnm_send_udata_to_disp(lnm_t *lnm_ptr, paktype *pak, boolean local_address, ushort length);
/*
 * Local Storage
 */
static uchar lrm_version[VERSIONLEN] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0};
static uchar bridge_version[VERSIONLEN] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0};
static uchar lanmgr_bogus_addr[IEEEBYTES]={0x40, 0x00, 0x00, 0x00, 0x20, 0x00};
static uchar lanmgr_bogus_rif[2] = {0xc2, 0x70};

static char *const lrm_names[] = {
    "Report LRM Parsing Error", "Request LRM Status", "Report LRM Status",
    "Set LRM Parameters", "LRM Parameters Set", "LRM Error", 
    "LRM Parameters Changed", "LRM Set Reporting Point", 
    "LRM LAN Manager Accepted", "LRM LAN Manager Rejected", 
    "LRM Report LAN Manager Rejection", "LRM New Reporting Link Established",
    "Report LAN Manager Control Shift", "Report LRM Control Breach Attempt",
    "LRM Close Reporting Link", "LRM Terminating", "LRM Invalid Request",
    "LRM Set Reporting Point", "LRM Congestion",
    "LRM Management Servers Present"};


/**********************************************************************
 *
 * Convert Internal LRM info various ways.
 *
 **********************************************************************/

#define NAME(x) lrm_names[(x) & COMMAND_MASK]

static char *lrm_to_string (
    int command)
{
    if (command < LRM_LAST)
	return(lrm_names[command & COMMAND_MASK]);
    return("unknown");
}

int lrmid_to_bitmask (ushort command)
{
    switch (command) {
      case IBMNM_CORRELATOR:     return(LRM_B_CORRELATOR);
      case IBMNM_ADDRESS:        return(LRM_B_PORT);
      case IBMNM_RING:           return(LRM_B_RING);
      case IBMNM_ALL:            return(LRM_B_ALL);
      case IBMNM_PRODUCTID:      return(LRM_B_TEMP);

      case LRM_SV_NUM_ALT_LM:	 return(LRM_B_NUM_ALT);
      case LRM_SV_ROUTING:       return(LRM_B_ROUTING);
      case LRM_SV_LINK_INFO:	 return(LRM_B_RLI);
      case LRM_SV_LINK_ID:	 return(LRM_B_RLI_ID);
      case LRM_SV_KEY:		 return(LRM_B_RLI_KEY);
/*    case LRM_SV_NEW_KEY:	 return(LRM_B_RLI_NEWKEY); */
      case LRM_SV_PORT_INFO:	 return(LRM_B_RLI_PI);
      case LRM_SV_REP_CLASS:	 return(LRM_B_RLI_PI_CLASS);
      case LRM_SV_ENAB_FUNC:	 return(LRM_B_RLI_PI_ENA);
      default:                   return(0);
    }
}

char *lrmbit_to_string(int bit)
{
    switch (bit) {
      case LRM_B_CORRELATOR:    return("Correlator");
      case LRM_B_ALL:           return("All");
      case LRM_B_PORT:		return("Port ID");
      case LRM_B_RING:		return("Ring");
      case LRM_B_NUM_ALT:	return("Number Alternate LM");
      case LRM_B_TEMP:		return("Temporary");
      case LRM_B_ROUTING:	return("Routing Info");
      case LRM_B_RLI:		return("Reporting Link Info");
      case LRM_B_RLI_ID:	return("Reporting Link ID");
      case LRM_B_RLI_KEY:	return("Reporting Link Key");
      case LRM_B_RLI_NEWKEY:	return("Reporting Link NewKey");
      case LRM_B_RLI_PI:	return("Port Info");
      case LRM_B_RLI_PI_ID:	return("Port ID");
      case LRM_B_RLI_PI_CLASS:	return("Port Reporting Classes");
      case LRM_B_RLI_PI_ENA:	return("Port Enabled Functional");
      default:                  return("unknown");
    }
}


void ibmnm_llc2_send(llctype *cookie, paktype *pak, int len)
{
    lnm_t *lnm_ptr = NULL;

    lnm_ptr = ucepid2lnm((uint) cookie);
    if (lnm_ptr != NULL) {
       pak->datagramsize = len;
       lnm_send_info_to_disp(lnm_ptr, pak);
    }
    else {
       if (llc_get_oqueue_status(cookie) == DLC_OQUEUE_FULL ) {
           datagram_done(pak);
	   return;
       }
       llc2_send(cookie, pak, len);
    }
}

/**********************************************************************
 *
 *			Data Structure Lookups
 *
 **********************************************************************/

/*
 * cookie_to_link
 *
 * Given a cookie from the LLC Type 2 routines, find the reporting link
 * associated with this connection.
 */
boolean cookie_to_link (
    llctype *cookie,
    bridge_info **bridge,
    int *linkno)
{
    bridge_info *b;
    int link;

    for (b = (bridge_info *)lm_bridgeQ.qhead; b; b = b->next) {
	for (link = 0; link < MAXLINKS; link++) {
	    if (b->links[link].cookie == cookie) {
		*bridge = b;
		*linkno = link;
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}

bridge_info *address_to_bridge (uchar *address)
{
    bridge_info *b;

    for (b = (bridge_info *)lm_bridgeQ.qhead; b; b = b->next)
	if (ieee_equal_sans_rii(address, b->idb1->hardware) ||
	    ieee_equal_sans_rii(address, b->idb2->hardware))
	    return(b);
    return(NULL);
}

bridge_info *idb_to_bridge (hwidbtype *idb)
{
    bridge_info *b;

    if (!idb || !is_srb_hw(idb))
	return(NULL);

    for (b = (bridge_info *)lm_bridgeQ.qhead; b; b = b->next)
	if ((idb == b->idb1) || (idb == b->idb2))
	    return(b);
    return(NULL);
}

bridge_info *ring_to_bridge (ring_info *ring)
{
    bridge_info *b;

    for (b = (bridge_info *)lm_bridgeQ.qhead; b; b = b->next)
	if ((ring == b->idb1->ring_info) ||
	    (ring == b->idb2->ring_info))
	    return(b);
    return(NULL);
}

#ifdef NOBODY_CALLS_ME
bridge_info *ringno_to_bridge (
    int ringno)
{
    return(ring_to_bridge(ringno_to_ring(ringno)));
}
#endif /* NOBODY_CALLS_ME */

/**********************************************************************
 *
 *			  Sending Primitives
 *
 * The following routines are tha basic sending promitives used to
 * communicate to a Lan Manager.  These primitives will send to all Lan
 * Managers connected to a bridge.  They also check to see if a Lan
 * Manager is interested in hearing about the particular class of command
 * being reported.
 *
 **********************************************************************/

/*
 * interested_in
 *
 * Determine if this lan manager is interested in hearing reports from a
 * given server process.
 */
boolean interested_in (
    bridge_info *bridge,
    int link,
    hwidbtype *idb,
    int command)
{
    ulong reporting;

    if (idb == bridge->idb1)
	reporting = bridge->links[link].reporting_idb1;
    else if (idb == bridge->idb2)
	reporting = bridge->links[link].reporting_idb2;
    else
	reporting = 0;

    switch (command & SERVER_MASK) {
      case REM_SERVER: return(reporting & REPORT_REM);
      case RPS_SERVER: return(reporting & REPORT_RPS);
      case CRS_SERVER: return(reporting & REPORT_CRS);
      case LBS_SERVER: return(reporting & REPORT_LBS);
      case LRM_SERVER: return(reporting & REPORT_LRM);
      }
    return(FALSE);
}

/*
 * lrm_send
 *
 * Send a message to one/all lan managers.  Also checks to see whether
 * the lan managers are interested in hearing the particular class of
 * mesage.  This should be the most commonly used send routine in thie
 * file.
 */
void lrm_send (
    paktype *pak,
    bridge_info *bridge,
    int whom,
    char *description)
{
    int linkno;
    bridge_link_info *link_info;
    paktype *dup;
    ibmnm_vector *dup_vector;
    
    /*
     * First find one.
     */
    for (linkno = 0; linkno < MAXLINKS; linkno++) {
	link_info = &bridge->links[linkno];
	if (!link_info->cookie)
	    continue;
	if (link_info->controlling && (whom == LRM_OTHER))
	    continue;
	if (!link_info->controlling && (whom == LRM_CONTROLLING))
	    continue;

	/*
	 * Now report
	 */
	if (!(dup = pak_duplicate(pak)))
	    continue;

	dup_vector = (ibmnm_vector *)ibmnm_vector_headstart(dup);
	ibmnm_llc2_send(link_info->cookie, dup, dup_vector->length);
	lnmllcbuginf("\nIBMNM: Sending %s to %e on link %d.", description,
	       link_info->address, linkno);
    }
    retbuffer(pak);
}

/*
 * lrm_send_link
 *
 * This routine is used to send a given packet to the connected lan
 * managers on a specific link.
 */
void lrm_send_link (
    paktype *pak,
    bridge_info *bridge,
    int linkno,
    char *description)
{
    bridge_link_info *info;
    ibmnm_vector *vector;

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    info = &bridge->links[linkno];
    if (!(cookie_to_link(info->cookie, &bridge, &linkno)) ||
         (!info->cookie)) {
        lnmllcbuginf("\nIBMNM: llc2 down");
        datagram_done(pak);
        return;
    }
    ibmnm_llc2_send(info->cookie, pak, vector->length);
    lnmllcbuginf("\nIBMNM: Sending %s to %e on link %d.", description, 
	   info->address, linkno);
}

/*
 * lrm_send_all_but
 *
 * This routine is used to send a given packet to all connected lan
 * managers except the one on a specific link.  It is generally used for
 * thing like notification of a new reporting link becoming active.
 */
void lrm_send_all_but (
    paktype *pak,
    bridge_info *bridge,
    int linkno,
    char *description)
{
    int i;
    paktype *dup;
    ibmnm_vector *dup_vector;

    for (i = 0; i < MAXLINKS; i++) {
	if ((i == linkno) || (!bridge->links[i].cookie))
	    continue;
	if (!(dup = pak_duplicate(pak)))
	    continue;

	dup_vector = (ibmnm_vector *)ibmnm_vector_headstart(dup);
	ibmnm_llc2_send(bridge->links[i].cookie, dup, dup_vector->length);
	lnmllcbuginf("\nIBMNM: sending %s to %e on link %d.",
		 description, bridge->links[i].address, i);
    }
    retbuffer(pak);
}

/*
 * lrm_send_ring
 *
 * Send a message to one/all lan managers.  Also checks to see whether
 * the lan managers are interested in hearing the particular class of
 * mesage.  This should be the most commonly used send routine in thie
 * file.
 */
void lrm_send_ring (paktype *pak, hwidbtype *idb, char *description)
{
    int linkno;
    bridge_link_info *link_info;
    bridge_info *bridge;
    paktype *dup;
    ibmnm_vector *vector, *dup_vector;

    if (!idb) {
	buginf("\nIBMNM: lrm_send_ring called for %s without interface.",
	       description);
	if (pak)
	    datagram_done((paktype *)pak);
	return;
    }

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    /*
     * First find one.
     */
    if (!(bridge = idb_to_bridge(idb))) {
	if (pak) datagram_done((paktype *)pak);
	return;
    }
    for (linkno = 0; linkno < MAXLINKS; linkno++) {
	link_info = &bridge->links[linkno];
	if (!link_info->cookie)
	    continue;

	/*
	 * Now report
	 */
	if (!interested_in(bridge, linkno, idb, vector->command))
	    continue;
	if (!(dup = pak_duplicate(pak)))
	    continue;
	dup_vector = (ibmnm_vector *)ibmnm_vector_headstart(dup);
	ibmnm_llc2_send(link_info->cookie, dup, dup_vector->length);
	lnmllcbuginf("\nIBMNM: Sending %s to %e on link %d.", description,
	       link_info->address, linkno);
    }
    retbuffer(pak);
}

/**********************************************************************
 *
 *			   Vector Builders
 *
 **********************************************************************/

/*
 * insert_lrm_pi_one_ring
 *
 * Insert "Port Information" for a given ring to the current vector.  The
 * 'which' argument controls what information is actually included by
 * this routine.  They are:
 *
 *     RINGNO     - include the ring number
 *     REP_CLASS  - include reporting classes
 *     FUNCTIONAL - include the interface functional address
 *     REST       - include available and active classes
 */
ibmnm_vector *insert_lrm_pi_one_ring (
    ibmnm_vector *vec,
    bridge_info *bridge,
    hwidbtype *idb,
    int which,
    int reporting_value)
{
    ibmnm_vector *sv;
    ulong real_functional;

    sv = insert_header(vec, LRM_SV_PORT_INFO);
    if (idb->status & IDB_FDDI)
	sv = insert_address(sv, idb->bit_swapped_hardware);
    else
	sv = insert_address(sv, idb->hardware);
    
    if (which & LRM_PI_RINGNO)
	sv = insert_ring(sv, srbV.srb_sb_get_thisring(idb->firstsw));
    if (which & LRM_PI_REP_CLASS)
	sv = insert_long(sv, LRM_SV_REP_CLASS, reporting_value);
    if (which & LRM_PI_REST) {
	sv = insert_long(sv, LRM_SV_AVAIL_CLASS, REPORT_AVAIL);
	sv = insert_long(sv, LRM_SV_ACTIVE_CLASS, REPORT_AVAIL);
	                                /* bridge->active | REPORT_LRM */
    }
    if (which & LRM_PI_FUNCTIONAL) {
	ONE_ITEM_LIST(tmp, TOKEN_HW_FUNCTIONAL, 1, 0);

	(*idb->state_config)(idb, IDB_CONTROL_READ_CONFIG, &tmp_list);

	real_functional = tmp_item.u.l_value;
	if (srbV.srb_sb_get_bridging_enabled(idb->firstsw)) {
	    real_functional |= TRFUNC_BRIDGE;
        }
	sv = insert_long(sv,LRM_SV_ENAB_FUNC, real_functional);
    }
    
    vec = finish_subvector(vec, sv);
    return(vec);
}

/*
 * insert_lrm_pi
 *
 * Add port specific information to the current vector.  The 'which'
 * argument controls what information is actually included by this
 * routine.  They are:
 *
 *     RINGNO     - include the ring number
 *     REP_CLASS  - include reporting classes
 *     FUNCTIONAL - include the interface functional address
 *     REST       - include available and active classes
 */
ibmnm_vector *insert_lrm_pi (
    ibmnm_vector *vec,
    bridge_info *bridge,
    int link,
    int which)
{
    ibmnm_vector *sv;

    sv = insert_lrm_pi_one_ring(vec, bridge, bridge->idb1, which,
				bridge->links[link].reporting_idb1);
    sv = insert_lrm_pi_one_ring(sv,  bridge, bridge->idb2, which,
				bridge->links[link].reporting_idb2);
    return(sv);
}

/*
 * insert_rep_link
 *
 * Add reporting link information to the current vector.  The 'which'
 * argument controls what information is actually included by this
 * routine.  They are:
 *
 *     link_no - include link identifierr
 *     address - include address of lan manager
 *     key     - include current key
 *     newkey  - include new key being set
 *     pi_info - which set of port information to include
 */
ibmnm_vector *insert_rep_link (
    ibmnm_vector *vec,
    bridge_info *bridge,
    int linkno,
    int which)
{
    ibmnm_vector *sv, *old_sv;

    sv = insert_header(vec, LRM_SV_LINK_INFO);
    if (which & LRM_RLI_LINKNO) {
	sv = insert_short(sv, LRM_SV_LINK_ID, linkno);
	if (which & LRM_RLI_ADDRESS) {
            old_sv = sv;
	    sv = insert_address(sv, bridge->links[linkno].address);

            /*
             * Check if special mac address process is needed
             * for the local LNM
             */
            reg_invoke_local_lnm_set_mac_address((uchar *)old_sv,
                                                 bridge->links[linkno].idb);
        }
    }
    if (which & LRM_RLI_KEY)
	sv = insert_bytes(sv, LRM_SV_KEY, bridge->links[linkno].key, LMKEYLEN);
    if (which & LRM_RLI_NEWKEY)
	sv = insert_bytes(sv, LRM_SV_NEW_KEY, bridge->links[linkno].key, LMKEYLEN);
    if (which & LRM_PI_ALL)
	sv = insert_lrm_pi(sv, bridge, linkno, which);

    vec = finish_subvector(vec, sv);
    return(vec);
}

/**********************************************************************
 *
 *			  Protocol Responses
 *
 **********************************************************************/

/*
 * lrm_error_frame (8605)
 *
 * Send a LRM Error Frame.  This message indicates that a command
 * received from the Lan Manager contained a syntax error, or the command
 * could not be completed sucessfully.
 */
void lrm_error_frame (
    bridge_info *bridge,
    int linkno,
    int correlator,
    int code,
    int offset)
{
    paktype *pak;
    ibmnm_vector *sv;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, LRM_ERROR);
    sv = insert_correlator(sv, correlator);
    sv = insert_short(sv, IBMNM_ERRCODE, code);
    sv = insert_short(sv, IBMNM_ERROFFSET, offset);
    sv = insert_bytes(sv, IBMNM_VERSION, lrm_version, VERSIONLEN);
    finish_vector(pak, sv);

    lrm_send_link(pak, bridge, linkno, NAME(LRM_ERROR));
}

/*
 * The following routines handle changing any of the parameters
 * maintained by the LRM A lan manager sends us a Set Reporting Point
 * frame.  The response to that message is either an acceptance,
 * rejection, or error frame.  Set frames will only bea ccetped from the
 * controlling Lan Network Manager.
 *
 * Set Parameters (I) -->
 * a) <-- Lan Parameters Set (I)
 *        Lan Parameters Changed (I)  -->
 * b) <-- Report Control Breach (I)
 *        Report Control Breach (I)   -->
 * c) <-- LRM Error
 */

/*
 * lrm_params_set (8604)
 *
 * Send a Parameters Set frame to the controlling lan manager.  It is
 * only possible for us to send this frame to the controlling manager.
 * If any other manager had attempted to send a Set Parameters frame, it
 * would have resulted in a Control Breach frame instead of this one.
 */
void lrm_params_set (bridge_info *bridge, int correlator)
{
    paktype *pak;
    ibmnm_vector *sv;
    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, LRM_PARAM_SET);
    sv = insert_correlator(sv, correlator);
    finish_vector(pak, sv);

    lrm_send(pak, bridge, LRM_CONTROLLING, NAME(LRM_PARAM_SET));
}

/*
 * lrm_params_changed (8606)
 *
 * Send a Parameters Changed frame to the all lan managers except the
 * controlling lan manager.  We are guaranted that this is the case
 * because if any other manager had attempted to send a Set Parameters
 * frame, it would have resulted in a Control Breach frame instead of
 * this one.
 */
void lrm_params_changed (bridge_info *bridge, int linkno, lrm_pb *what)
{
    paktype *pak;
    ibmnm_vector *sv, *sv2;
    int i;

    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;

    sv = insert_major(pak, LRM_PARAM_CHANGED);
    if (what->port) {
	sv = insert_address(sv, what->port);
	sv = insert_ring(sv, *what->ring);
    }
    if (what->numalt)
	sv = insert_short(sv, LRM_SV_NUM_ALT_LM, *what->numalt);
    if (what->linkid) {
	sv2 = insert_header(sv, LRM_SV_LINK_INFO);
	sv2 = insert_short(sv2, LRM_SV_LINK_ID, *what->linkid);
	if (what->newkey)
	    sv2 = insert_bytes(sv2, LRM_SV_NEW_KEY, "\0\0\0\0\0\0\0\0", LMKEYLEN);
	for (i = 0; i < what->numports; i++) {
	    sv2 = insert_address(sv2, what->ports[i].portid);
	    if (what->ports[i].repclass)
		sv2=insert_long(sv2,LRM_SV_REP_CLASS, GETLONG(what->ports[i].repclass));
	    if (what->ports[i].enabled)
		sv2=insert_long(sv2,LRM_SV_ENAB_FUNC, GETLONG(what->ports[i].enabled));
	}
	sv = finish_subvector(sv, sv2);
    }
    finish_vector(pak, sv);

    lrm_send_all_but(pak, bridge, linkno, NAME(LRM_PARAM_CHANGED));
}

/*
 * lrm_control_breach (860D)
 *
 * Send a Report Control Breach frame to all lan managers.  This is the
 * result of a non-controlling lan manager attempting to modify
 * parameters.
 */
void lrm_control_breach (bridge_info *bridge, int linkno)
{
    paktype *pak;
    ibmnm_vector *sv;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, LRM_REP_CNTL_BREACH);
    sv = insert_rep_link(sv, bridge, linkno, LRM_RLI_CNTL_BREACH);
    finish_vector(pak, sv);

    lrm_send(pak, bridge, LRM_ALL, NAME(LRM_REP_CNTL_BREACH));
}

/*
 * The following routine handles the lan manager status reporting.  A
 * lan manager sends us a Request LRM Status frame.  The response to
 * that message is either a Report LRM Status frame or an Error frame.
 *
 * LRM Request Status (I) -->
 * a) <-- LRM Report Status (I)
 * b) <-- LRM Error (I)
 */

/*
 * lrm_report_status (8602)
 *
 * Send a Report Status Message back to the requesting lan manager.
 */
void lrm_report_status (bridge_info *bridge, int linkno, int correlator)
{
    paktype *pak;
    ibmnm_vector *sv;
    int i;

    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;

    sv = insert_major(pak, LRM_REP_STATUS);
    sv = insert_correlator(sv, correlator);
    sv = insert_bytes(sv, IBMNM_VERSION, lrm_version, VERSIONLEN);
    sv = insert_short(sv, LRM_SV_NUM_ALT_LM, bridge->num_alt_lanmgrs);

    for (i = 0; i < MAXLINKS; i++)
	if (bridge->links[i].cookie)
	    sv = insert_rep_link(sv, bridge, i, LRM_RLI_REP_STATUS);
    sv = insert_lrm_pi(sv, bridge, linkno, LRM_PI_RINGNO);
    finish_vector(pak, sv);

    lrm_send_link(pak, bridge, linkno, NAME(LRM_REP_STATUS));
}

/*
 * The following routines handle the lan manager connection sequence.  A
 * lan manager sends us a Set Reporting Point frame.  The response to
 * that message is either an acceptance, rejection, or error frame.
 *
 * Set Reporting Point (UI) -->
 * a) <-- Lan Manager Accepted (I)
 *        New Reporting Link Established (I) -->
 *        LAN Manager Control Shift (I)      -->
 * b) <-- Lan Manager Rejected (UI)
 *        Report Lan Manager Rejection (I)   -->
 * c) <-- Set Reporting Point Error (UI)
 */

/*
 * lrm_accept (8608)
 *
 * Send a Lan Manager Accepted packet back to the lan manager who just
 * attempted to initiated a connection.  We will also be sending New
 * Reporting Link messages and a possible Control Shift messages from
 * other routines.
 */
void lrm_accept (bridge_info *bridge, int linkno, int correlator)
{
    paktype *pak;
    ibmnm_vector *sv;

    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;

    sv = insert_major(pak, LRM_MGR_ACCEPT);
    sv = insert_correlator(sv, correlator);
    sv = insert_short(sv, LRM_SV_NUM_ALT_LM, bridge->num_alt_lanmgrs);
    sv = insert_rep_link(sv, bridge, linkno, LRM_RLI_MGR_ACCEPT);
    finish_vector(pak, sv);

    lrm_send_link(pak, bridge, linkno, NAME(LRM_MGR_ACCEPT));
}

/*
 * lrm_reject (8609)
 *
 * Send a Lan Manager Rejected packet back to the lan manager who just
 * attempted to initiate a connection.  This is an Unnumbered
 * Information Frame.  We will also be sending Lan Manager Rejected
 * Report messages to all other lan managers from other routines.
 */
void lrm_reject (
    paktype *pak,
    bridge_info *bridge,
    int link,
    int code,
    int correlator,
    lnm_t *lnm_ptr)
{
    paktype *reply;
    ibmnm_vector *reply_vector, *sv;
    uchar address[IEEEBYTES];
    boolean local_address = FALSE;
    int length;
    if (lnm_ptr != NULL) {
	/*
         * This reject was generated by CLSI compliant LNM
	 */
        bcopy(&lnm_ptr->dlc_id.client_mac_address, address, IEEEBYTES);	
    }
    else {
	/*
         * This reject was generated by legacy LNM for RSRB
	 */
	find_802addr(pak, address, GET_SOURCE);
    }
    reply = ibmnm_getbuffer(MEDDATA);
    if (!reply) return;

    reply_vector = (ibmnm_vector *)ibmnm_vector_headstart(reply);

    reply->linktype = LINK_IBMNM;
    reply->acct_proto = ACCT_PROTO_LANMAN;
    reply->if_output = pak->if_input;
    reply->llc2_enctype = ET_SAP;
    sv = insert_major(reply, LRM_MGR_REJECT);
    sv = insert_correlator(sv, correlator);
    sv = insert_short(sv, LRM_SV_REJECT_CODE, code);
    if ((code != LRM_REJ_BAD_KEY) && (code != LRM_REJ_BAD_LINKNO)) {
	sv = insert_short(sv, LRM_SV_NUM_ALT_LM, bridge->num_alt_lanmgrs);
	sv = insert_rep_link(sv, bridge, link, LRM_RLI_MGR_REJECT);
    }
    finish_vector(reply, sv);

    /*
     * Try to send it.
     */
    if (lnm_ptr != NULL) {
        /*
         * CLSI compliant LNM tranmsission
	 */
        lnmllcbuginf("\nIBMNM: sending manager rejected code %d to %e.",
		     code, address);
        length = reply_vector->length;
        lnm_send_udata_to_disp(lnm_ptr, reply, local_address, length);
	datagram_done(reply);
    }
    else {
	/*
         * legacy llc2 transmission for rsrb/pcbus cases
	 */
        /* See if it is for the PCbus interface */
        if (reg_invoke_local_lnm_pcbus_send((llctype *)(reply->if_output->hwptr), reply)) {
            return;
        }
        length = reply_vector->length;
	reply->datagramsize = length;
        if (interface_up_simple(reply->if_output) &&
	    (*reply->if_output->hwptr->vencap)(reply, (long)address)) {
	    lnmllcbuginf("\nIBMNM: sending manager rejected code %d to %e.",
		     code, address);
	    datagram_out(reply);
        } else {
	    lnmllcbuginf("\nIBMNM: Cannot send manager rejected code %d to %e.", code, address);
	    retbuffer(reply);
        }
    }
    return;
}

/*
 * lrm_set_rep_error (8611)
 *
 * Send a Set Reporting Point Error frame back to the lan manager who
 * just attempted to initiated a connection.  This is an Unnumbered
 * Information Frame.
 */
void lrm_set_rep_error (
    paktype *pak,
    ushort correlator,
    int reason,
    int code,
    int offset,
    lnm_t *lnm_ptr)
{
    paktype *reply;
    ibmnm_vector *reply_vector, *sv;
    uchar address[IEEEBYTES];
    boolean local_address = FALSE;
    int length;

    if (lnm_ptr != NULL) {
	/*
         * This reject was generated by CLSI compliant LNM
	 */
        bcopy(&lnm_ptr->dlc_id.client_mac_address, address, IEEEBYTES);	
    }
    else {
	/*
         * This reject was generated by legacy LNM for RSRB/PCBUS
	 */
	find_802addr(pak, address, GET_SOURCE);
    }    
    reply = ibmnm_getbuffer(MEDDATA);
    if (!reply) return;

    reply_vector = (ibmnm_vector *)ibmnm_vector_headstart(reply);

    reply->linktype = LINK_IBMNM;
    reply->acct_proto = ACCT_PROTO_LANMAN;
    reply->if_output = pak->if_input;
    reply->llc2_enctype = ET_SAP;
    sv = insert_major(reply, LRM_SET_REP_ERR);
    sv = insert_correlator(sv, correlator);
    sv = insert_byte(sv, LRM_SV_SETREPPNT_REASON, (uchar) reason);
    sv = insert_short(sv, IBMNM_ERRCODE, code);
    sv = insert_short(sv, IBMNM_ERROFFSET, offset);
    sv = insert_bytes(sv, IBMNM_VERSION, lrm_version, VERSIONLEN);
    finish_vector(reply, sv);

    /*
     * Try to send it.
     */
    if (lnm_ptr != NULL) {
        /*
         * CLSI compliant LNM tranmsission
	 */	
        lnmllcbuginf("\nIBMNM: sending set reporting point error %d to %e.",
		     code, address);
        length = reply_vector->length;
        lnm_send_udata_to_disp(lnm_ptr, reply, local_address, length);
	datagram_done(reply);
    } else {
	/*
         * legacy llc2 transmission for rsrb/pcbus cases
	 */
	/* See if it is for the PCbus interface */
        if (reg_invoke_local_lnm_pcbus_send((llctype *)(reply->if_output->hwptr), reply)) {
            return;
        }
        length = reply_vector->length;
	reply->datagramsize = length;
        if (interface_up_simple(reply->if_output) &&
	    (*reply->if_output->hwptr->vencap)(reply, (long)address)) {
	    lnmllcbuginf("\nIBMNM: sending set reporting point error %d to %e.", code, address);
	    datagram_out(reply);
        } 
        else {
	    lnmllcbuginf("\nIBMNM: Cannot send set reporting point error %d to %e.", code, address);
	    retbuffer(reply);
        }
    }
}

/*
 * lrm_report_reject (860A)
 *
 * Send a Lan Manager Rejected message to all lan managers.  This is the
 * result of a bad connection attempt.
 */
void lrm_report_reject (bridge_info *bridge, uchar *address, ushort code)
{
    paktype *pak;
    ibmnm_vector *sv;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, LRM_REP_MGR_REJECT);
    sv = insert_short(sv, LRM_SV_REJECT_CODE, code);
    sv = insert_address(sv, address);
    finish_vector(pak, sv);

    lrm_send(pak, bridge, LRM_ALL, NAME(LRM_REP_MGR_REJECT));
}

/*
 * lrm_new_reporting_link (860B)
 *
 * Send a New Reporting Link message to all lan managers.  This is the
 * result of a valid connection attempt.  This message may also be
 * followed by a report control shift message generated by another
 * routine.
 *
 */
void lrm_new_reporting_link (bridge_info *bridge, int linkno)
{
    paktype *pak;
    ibmnm_vector *sv;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, LRM_REP_NEW_LINK);
    sv = insert_rep_link(sv, bridge, linkno, LRM_RLI_NEW_LINK);
    finish_vector(pak, sv);

    lrm_send_all_but(pak, bridge, linkno, NAME(LRM_REP_NEW_LINK));
}

/*
 * lrm_mgmt_servers_present  (8613)
 *
 * Send a Management Servers Present message to a unique locally
 * administered address (4000.0000.2000).  This address just so happens
 * to be the Lan Manager funcional address with the high order bit turned
 * off.  This was discovered by watching an IBM Bridge (v2.1) start up.
 *
 */
void lrm_mgmt_servers_present (bridge_info *bridge)
{
    paktype *pak;
    ibmnm_vector *vector, *sv;
    bridge_link_info *link_info;
    int link;
    int length;
    lnm_t *lnm_ptr = NULL;
    boolean local_addr = TRUE;

    if (bridge->idb1->status & IDB_CHANNEL)
	return;

    /*
     * We are sending to a locally administered address, but make sure
     * that this packet has an explorer on it.
     */
    rif_update(NULL, 0, lanmgr_bogus_addr, NULL, (srbroute_t *)lanmgr_bogus_rif,
	       RIF_AGED);
    sprintf((char *)&bridge_version[1], "%02d%02d%02d%03d", SYS_MAJORVERSION,
	    SYS_MINORVERSION, 0, 0);

    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);

    pak->linktype = LINK_IBMNM;
    pak->if_output = bridge->idb1->firstsw;
    sv = insert_major(pak, LRM_SRVRS_PRESENT);
    sv = insert_bytes(sv, IBMNM_VERSION, bridge_version, BRIDGEVERSIONLEN);
    sv = insert_rep_link(sv, bridge, 0, LRM_RLI_SRVRS_PRESENT);
    finish_vector(pak, sv);

    /* This is a UI Frame that simulates the behaviour of an IBM Bridge
     * Need to send it to CLS as appropriate.
     */
    for (link = 0; link < MAXLINKS; link++) {
	link_info = &bridge->links[link];
	if (link_info->idb == bridge->idb1) {		
	    lnm_ptr = ucepid2lnm((uint) link_info->cookie);
	    break;
	}
    }    
    if (lnm_ptr != NULL) {
	lnmllcbuginf("\nIBMNM: Sending servers present to %e.",
		        lanmgr_bogus_addr);
	length = vector->length;
	lnm_send_udata_to_disp(lnm_ptr, pak, local_addr, length);
	datagram_done(pak);
    }
    else {
	pak->datagramsize = vector->length;
        if (interface_up_simple(pak->if_output) &&
	    (*pak->if_output->hwptr->vencap)(pak, (long)lanmgr_bogus_addr)) {
	    lnmllcbuginf("\nIBMNM: Sending servers present to %e.",
		            lanmgr_bogus_addr);
	    datagram_out(pak);
	}
	else { 
	    lnmllcbuginf("\nIBMNM: Cannot send servers present to %e.",
		            lanmgr_bogus_addr);
	    retbuffer(pak);
        }
    }
}


/*
 * lrm_parsing_error (8600)
 *
 * Send a Lan Resource Manager Parsing Error message.  This message
 * indicates that an input frame could not be parsed correctly.  This can
 * occur because the input frame is too short to contain a MVID, the MVID
 * is not recognized as valid, or the MVID length is not equal to the
 * received length of the frame.
 */
void lrm_parsing_error (
    bridge_info *bridge,
    int linkno,
    ushort mvid,
    int code)
{
    paktype *pak;
    ibmnm_vector *sv;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, LRM_PARSE_ERROR);
    sv = insert_short(sv, LRM_SV_ERROR_MVID, mvid);
    sv = insert_bytes(sv, IBMNM_VERSION, lrm_version, VERSIONLEN);
    sv = insert_byte(sv, LRM_SV_PARSE_REASON, code);
    finish_vector(pak, sv);

    lrm_send_link(pak, bridge, linkno, NAME(LRM_PARSE_ERROR));
}

/*
 * lrm_invalid_request (8610)
 *
 * Send a LRM Invalid Request frame.  This message indicates that a
 * command received from the Lan Manager could not be sent to the
 * requested server for one of several reasons.  Either the server is not
 * available, is not currently active, or is in the wrong state for the
 * command.
 */
void lrm_invalid_request (llctype *cookie, ushort correlator, int code)
{
    paktype *pak;
    ibmnm_vector *sv;
    int linkno;
    bridge_info *bridge;

    if (!cookie_to_link(cookie, &bridge, &linkno)) {
	buginf(nolink, cookie);
	return;
    }

    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;

    sv = insert_major(pak, LRM_INVALID);
    sv = insert_correlator(sv, correlator);
    sv = insert_short(sv, LRM_SV_INVALID_REASON, code);
    sv = insert_rep_link(sv, bridge, linkno, LRM_RLI_INVALID);
    finish_vector(pak, sv);

    lrm_send_link(pak, bridge, linkno, NAME(LRM_INVALID));
}

/*
 * lrm_terminating (860F)
 *
 * Send a Lan Manager Terminating message to all lan managers.  The code
 * value will indicate if this is a normal shutdown, or is because of
 * some failure.
 */
void lrm_terminating (bridge_info *bridge, int code)
{
    paktype *pak, *dup;
    ibmnm_vector *sv;
    int link;
    bridge_info *b;
    lnm_t *lnm_ptr;

    pak = ibmnm_getbuffer(SMALLDATA);
    if (!pak) return;

    sv = insert_major(pak, LRM_TERMINATING);
    sv = insert_byte(sv, LRM_SV_TERM_REASON, code);
    finish_vector(pak, sv);

    /*
     * Send to all links on all bridges.
     */
    for (b = (bridge_info *)lm_bridgeQ.qhead; b; b = b->next) {
	if (bridge && (bridge != b))
	    continue;
	if (!(dup = pak_duplicate(pak)))
	    continue;
	lrm_send(dup, b, LRM_ALL, NAME(LRM_TERMINATING));
	for (link = 0; link < MAXLINKS; link++) {
	    if (b->links[link].cookie) {
		lnm_ptr = ucepid2lnm((uint) b->links[link].cookie);
	        if (lnm_ptr != NULL) {
		    lnm_send_close_stn(lnm_ptr);
		}
		else {      
		    llc2_close(b->links[link].cookie);
		}
		unset_rif(b->links[link].idb, 0, b->links[link].address, NULL, FALSE);
	    }
	}
    }
    retbuffer(pak);
}

void ibmnm_shutdown (hwidbtype *idb)
{
    bridge_info *bridge;
    int link;

    if (!is_srb_hw(idb))
	return;
    bridge = idb_to_bridge(idb);
    if (!bridge)
	return;

    lrm_terminating(bridge, LRM_TERM_NORMAL);
    for (link = 0; link < MAXLINKS; link++) {
	if ((bridge->links[link].cookie) &&
	    (bridge->links[link].idb == idb)) {
	    destroy_link(bridge, link);
	}	
    }
}

/*
 * ibmnm_reload_message
 *
 * This routine is called by the reload command in order to send a lan
 * manager terminating message.
 */

void ibmnm_reload_message (void)
{
    if (!lanmgr_global_enable)
	return;

    lrm_terminating(NULL, LRM_TERM_NORMAL);
}

/**********************************************************************
 *
 *			  Workhorse routines
 *
 **********************************************************************/

/*
 * lrm_find_controlling
 *
 * Determine who is the current controlling terminal.  If it has changed,
 * then send out the appropriate messages to all links.  IBM's
 * documentation on this differs from it's implementation.  The
 * documentation states that the lowest numbered Lan Network Manager
 * present is the controlling Lan Network Manager.  There is a cutoff
 * point on this number, that initially restricts control to Lan Network
 * Manager 0 (zero).  In practice, this number cannot be changed to allow
 * Lan Network Managers other than 0 (zero) to be a controlling Lan
 * Network Manager.
 */
void lrm_find_controlling (bridge_info *bridge, ushort why)
{
    paktype *pak;
    ibmnm_vector *sv;
    int current, new, linkno;
    ushort why2;

    lnmllcbuginf("\nIBMNM: Determining new controlling LNM");
    current = new = -1;
    for (linkno = 0; linkno <= bridge->num_alt_lanmgrs; linkno++) {
	if (!bridge->links[linkno].cookie)
	    continue;
	if (new == -1)
	    new = linkno;
	if (bridge->links[linkno].controlling)
	    current = linkno;
    }

    if (current != -1)
	bridge->links[current].controlling = FALSE;
    if (new != -1)
	bridge->links[new].controlling = TRUE;
    if ((current == new) && (new != -1))
	return;
    if (why == LRM_CNTL_NEW && current == -1)
	why2 = LRM_CNTL_CLOSE;
    else
	why2 = why;

    pak = ibmnm_getbuffer(MEDDATA);
    if (!pak) return;
    sv = insert_major(pak, LRM_REP_CNTL_SHIFT);
    sv = insert_short(sv, LRM_SV_STATUS_CODE, (new != -1));
    sv = insert_short(sv, LRM_SV_CNTL_REASON, why2);
    sv = insert_short(sv, LRM_SV_NUM_ALT_LM, bridge->num_alt_lanmgrs);
    if (new != -1)
	sv = insert_rep_link(sv, bridge, new, LRM_RLI_CNTL_SHIFT);
    finish_vector(pak, sv);

    lrm_send_all_but(pak, bridge, new, NAME(LRM_REP_CNTL_SHIFT));
}

/*
 * do_set_lrm_parameters
 *
 * The router has been asked by Lan Network Manager to change one or more
 * of its iternal parameters.  All requests seen so far only change one
 * parameter, but it is safer not to make that assumption.  Check to see
 * which parameters should be changed, and change them.
 */
static boolean do_set_lrm_parameters (
    bridge_info *bridge,
    int link,
    lrm_pb *result,
    int *error)
{
    hwidbtype *idb, *other;
    ring_info *ring;
    int affects, i;

    /*
     * Validate parameters.
     */
    if ((!result->port &&  result->ring) ||
	( result->port && !result->ring)) {
	*error = ERR_REQ_SV_MISSING;
	return(FALSE);
    }

    /*
     * Validate ring numbers.  The ring number of a virtual ring may not
     * be changed.
     */
    if (result->port && result->ring) {
	idb = valid_port(bridge, result->port);
	if (idb) {
	    if (idb->status & IDB_VIRTUAL) {
		*error = SET_ERR_UNAUTH_REQ;
		return(FALSE);
	    }
	}
    }

    /*
     * Everything is O.K.  Store the new values.
     */
    if (result->numalt)
	bridge->num_alt_lanmgrs = *result->numalt;
    
    if (result->port && result->ring) {
	idb = valid_port(bridge, result->port);
	if (idb) {
	    ring = idb_to_ring(idb);
	    ring->ringno = *result->ring;
	    other = (idb == bridge->idb1) ? bridge->idb2 : bridge->idb1;
            srbV.srb_sb_set_thisring(idb->firstsw, *result->ring);
            srbV.srb_sb_set_targetring(other->firstsw, *result->ring);
	    srbcore_modify_srb(bridge->idb1, TR_MODIFY_RING, *result->ring);
	    srbcore_modify_srb(bridge->idb2, TR_MODIFY_TRING, *result->ring);
	    do_bridge_number_setup(bridge->idb1, bridge->idb2);
	}
    }
    if (result->linkid) {
	affects = *result->linkid;
	if (result->newkey)
	    bcopy(result->newkey, bridge->links[affects].key, LMKEYLEN);
	for (i = 0; i < result->numports; i++) {
	    idb = address_to_idb(result->ports[i].portid);
	    if (idb) {
		if (result->ports[i].repclass) {
		    if (idb == bridge->idb1)
			bridge->links[affects].reporting_idb1 = GETLONG(result->ports[i].repclass);
		    else if (idb == bridge->idb2)
			bridge->links[affects].reporting_idb2 = GETLONG(result->ports[i].repclass);
		}
		if (result->ports[i].enabled) {
		    set_new_ringmode(idb, GETLONG(result->ports[i].enabled));
		    if (idb->set_extra_addrs != NULL)
			(void)(*idb->set_extra_addrs)(idb);
		}
	    }
	}
    }
    
/* The following write NVRAM causes LNM to time out, therefore it is */
/* deleted */
/*    if (!result->temp || (*result->temp == PERMANENT)) {
 *	nv_current_to_nv(FALSE, PRIV_ROOT);
 *	errmsg(&msgsym(CONFIG_L, SYS), bridge->links[link].address);
 *   }
 */  
  return(TRUE);
}

/**********************************************************************
 *
 *			   Parsing Routines
 *
 **********************************************************************/

/*
 * lrm_parse_pi
 *
 *
 * Parse information associated with a particular port (interface) on a
 * bridge.  These include the address, reporting class, sne enabled
 * functions.
 */
boolean lrm_parse_pi (
    ibmnm_vector *vector,
    ulong required,
    ulong optional,
    lrm_pb *r,
    int *error,
    ibmnm_vector **offset)
{
    ulong bit, present, missing;
    ibmnm_vector *sv;
    int length, port;
    uchar *data;

    present = 0;
    port = r->numports - 1;
    length = GETSHORT(&vector->length) - IBMNM_MVID_LEN;
    sv = (ibmnm_vector *)&vector->data;

    for ( ; length > 0; length -= GETSHORT(&sv->length), sv = next_subvector(sv)) {
	bit = lrmid_to_bitmask(GETSHORT(&sv->command));
	if (bit == LRM_B_PORT)
	    bit = LRM_B_RLI_PI_ID;
	present |= bit;

	/*
	 * Is it allowed?
	 */
	if ((((required | optional) & LRM_B_RLI_PI_MASK) & bit) == 0) {
	    lnmllcbuginf(unxpctvec, lrmbit_to_string(bit));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}

	/*
	 * Record it.
	 */
	data = (uchar *)&sv->data;
	switch (GETSHORT(&sv->command)) {
	  case IBMNM_ADDRESS:    r->ports[port].portid   = (uchar *)data;break;
	  case LRM_SV_REP_CLASS: r->ports[port].repclass = (ulong *)data;break;
	  case LRM_SV_ENAB_FUNC: r->ports[port].enabled  = (ulong *)data;break;
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
    missing = (required & LRM_B_RLI_PI_MASK) & ~present;
    if (missing) {
	lnmllcbuginf(missvec, lrmbit_to_string(missing));
	*error = ERR_REQ_SV_MISSING;
	*offset = sv;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * lrm_parse_rli
 *
 * Parse information associated with a particular "Reporting Link".  This
 * information includes the link number, old key value, and new key
 * value.
 */
boolean lrm_parse_rli (
    ibmnm_vector *vector,
    ulong required,
    ulong optional,
    lrm_pb *result,
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
	bit = lrmid_to_bitmask(GETSHORT(&sv->command));
	if (bit == LRM_B_ROUTING)
	    bit = LRM_B_RLI_NEWKEY;
	present |= bit;

	/*
	 * Is it allowed?
	 */
	if ((((required | optional) & LRM_B_RLI_MASK) & bit) == 0) {
	    lnmllcbuginf(unxpctvec, lrmbit_to_string(bit));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}

	/*
	 * Record it.
	 */
	data = (uchar *)&sv->data;
	switch (GETSHORT(&sv->command)) {
	  case LRM_SV_LINK_ID: result->linkid = (ushort *)data; break;
	  case LRM_SV_KEY:     result->key    = (char *)data;   break;
	  case LRM_SV_NEW_KEY: result->newkey = (char *)data;   break;
	    
	  case LRM_SV_PORT_INFO:
	      if (result->numports < RSRB_MAXRINGS) {
		  result->numports++;
		  lrm_parse_pi(sv, required, optional, result, error, offset);
	      } else {
		  lnmllcbuginf("\nIBMNM: Too many ports on PORT_INFO request");
		  *error = ERR_BAD_SV_VALUE;
		  *offset = sv;
		  return(FALSE);
	      }
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
    missing = (required & LRM_B_RLI_MASK) & ~present;
    if (missing) {
	lnmllcbuginf(missvec, lrmbit_to_string(missing));
	*error = ERR_REQ_SV_MISSING;
	*offset = sv;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * lrm_parse_vectors
 *
 * Parse the subvectors in a major vector sent to the LRM.  This routine
 * checks to insure that all required vectors are present.  It also
 * insures that the major vector contains only the required and optional
 * subvectors that are permissible for this particular major vector.
 * This routine builds and returns a data structure that indicates which
 * subvectors were present, and what their values were.
 */
boolean lrm_parse_vectors (
    ibmnm_vector *vector,
    ulong required,
    ulong optional,
    lrm_pb *result,
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
	bit = lrmid_to_bitmask(GETSHORT(&sv->command));
	present |= bit;

	/*
	 * Is it allowed?
	 */
	if ((((required | optional) & LRM_B_TOP_MASK) & bit) == 0) {
	    lnmllcbuginf(unxpctvec, lrmbit_to_string(bit));
	    *error = ERR_BAD_SV_VALUE;
	    *offset = sv;
	    return(FALSE);
	}

	/*
	 * Record it.
	 */
	data = (uchar *)&sv->data;
	switch (GETSHORT(&sv->command)) {
	  case IBMNM_CORRELATOR:  result->correlator = (ushort *)data; break;
	  case IBMNM_ADDRESS:     result->port	     = (uchar *)data;  break; 
	  case IBMNM_RING:        result->ring	     = (ushort *)data; break; 
	  case IBMNM_ALL:         result->all	     = (ushort *)data; break;
	  case IBMNM_PERMANENT:   result->temp	     = (uchar *)data;  break;
	  case LRM_SV_NUM_ALT_LM: result->numalt     = (ushort *)data; break;

	  case LRM_SV_LINK_INFO:
	    lrm_parse_rli(sv, required, optional, result, error, offset);
	    break;
	    
	  case LRM_SV_ROUTING:
	    result->routing = (GETSHORT(&sv->length) == 4) ? (uchar *)-1L : data;
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
    missing = (required & LRM_B_TOP_MASK) & ~present;
    if (missing) {
	lnmllcbuginf(missvec, lrmbit_to_string(missing));
	*error = ERR_REQ_SV_MISSING;
	*offset = sv;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * ibmnm_lrm_ui_input 
 *
 * This routine handles all UI packets addresses to the Lan Reporting
 * Manager.  The only legal UI is a "Set Reporting Point" command.
 */
void ibmnm_lrm_ui_input (paktype *pak, lnm_t *lnm_ptr)
{
    llctype *cookie;
    int link, error, i;
    paktype *dup;
    ushort correlator; /* , slot_number, port_number, port_type; */
    uchar mgr_address[IEEEBYTES], my_address[IEEEBYTES], addr[IEEEBYTES];
    char rif_string[100];
    ibmnm_vector *vector, *offset;
    lrm_pb *result;
    bridge_info *bridge;
    hwidbtype *idb;
    srbroute_t *rif;
    ulong rng_br_rng_b, rng_br_rng_f;
    uchar *ptr;
    ulong rd_info;
    boolean found;
    int length;
    hwaddrtype mgrAddr, myAddr;
    lnm_clsi_t *lnm_clsi_tmp;
    tr_vgrouptype *vrg;
    ushort vrnf, vrnb;

    vector = (ibmnm_vector *)ibmnm_vector_headstart(pak);
    if (lnm_ptr != NULL) {
	/*
         *  LNM CLSI compliant path taken by all local LNM reporting links
	 *  and those reporting links established via VDLC over DLSw
         */
        lnm_clsi_tmp = lnm_ptr->lnm_port_id_dgra;

	if (!(lnm_clsi_tmp)) {
	    lnmllcbuginf("\nIBMNM_LRM: lnm_enqueue - cannot find lnm_clsi_t from port_id");
	    return;
	}
	if ((lnm_clsi_tmp->swidb) && (lnm_clsi_tmp->swidb->hwptr)) {
	    pak->if_input = lnm_clsi_tmp->swidb;
	}
	else {
	    lnmllcbuginf("\nIBMNM_LRM lnm_enqueue: no lnm_clsi_t swidb or swidb->hwptr!");
	}
        lrm_find_ui_server_addr(lnm_ptr, my_address);
        bcopy(my_address, &lnm_ptr->dlc_id.server_mac_address, IEEEBYTES);
        lrm_find_ui_client_addr(lnm_ptr, mgr_address);
        bcopy(mgr_address, &lnm_ptr->dlc_id.client_mac_address, IEEEBYTES);
    }
    else {
	/*
	 * LLC2 legacy path required to support LNM over RSRB
	 */
	find_802addr(pak, my_address, GET_DEST);
	find_802addr(pak, mgr_address, GET_SOURCE);
    }

    lnmllcbuginf("\nIBMNM: Received %s frame from %e.",
		 lrm_to_string(GETSHORT(&vector->command)), mgr_address);

    /*
     * This must be a Set Reporting Point command!
     */
    if (GETSHORT(&vector->command) != LRM_SET_REP_PNT) {
	lnmllcbuginf(illegalui, "LRM", GETSHORT(&vector->command));
	return;
    }
    
    error = 0;
    result = malloc_named(sizeof(lrm_pb) +
			      RSRB_MAXRINGS * sizeof(lrm_pi_pb), lnm_cb);
    if (!result) return;

    /*
     * parse
     */
    if (!lrm_parse_vectors(vector, LRM_R_SET_REP_PNT, LRM_O_SET_REP_PNT,
			   result, &error, &offset)) {
	lrm_set_rep_error(pak, (result->correlator ? *result->correlator : 0),
			  LRM_SETREPPNT_ERROR, error,
			  OFFSET(offset, vector), lnm_ptr);
	free(result);
	return;
    }
    
    /*
     * Get basic information from the received frame.
     */
    link = *result->linkid;
    correlator = *result->correlator;
    bridge = address_to_bridge(my_address);
    if (bridge) {
        srb_triplet_t srb_triplet;
        srbV.srb_sb_get_triplet(bridge->idb1->firstsw, &srb_triplet);
        lnmllcbuginf("\nIBMNM: found bridge: %03x-%x-%03x, addresses: %e %e",
	     srb_triplet.localRing,
	     srb_triplet.bridgeNum,
	     srb_triplet.remoteRing,
             bridge->idb1->hardware, bridge->idb2->hardware);
    }
    /*
     * Check for various failure conditions.  These include a bad link
     * number, bad key (password), link already in use, and link crosses
     * this bridge.
     */
    else {
	lrm_set_rep_error(pak, correlator, LRM_SETREPPNT_ERROR, ERR_REQ_SV_MISSING, 0, lnm_ptr);
        free(result);
	return;
    }
    if ((bridge->idb1->state != IDBS_UP) || (bridge->idb2->state != IDBS_UP)) {
	lnmllcbuginf("\nIBMNM: both interfaces are not operational.");
	/* Do not send an error message.  An IBM bridge would not be operational
	   at all unless both interfaces were up. */
	free(result);
	return;
    }
    if (link != 0 && result->numalt) {
	lrm_set_rep_error(pak, correlator, LRM_SETREPPNT_ERROR, SET_ERR_UNAUTH_REQ, 0, lnm_ptr);
	free(result);
	return;
    }
    if (link > MAXLINKS) {
	lrm_reject(pak, bridge, link, LRM_REJ_BAD_LINKNO, correlator, lnm_ptr);
	lrm_report_reject(bridge, mgr_address, LRM_REJ_BAD_LINKNO);
	free(result);
	return;
    }
    if (strncmp(bridge->links[link].key, result->key, LMKEYLEN) != 0) {
	lrm_reject(pak, bridge, link, LRM_REJ_BAD_KEY, correlator, lnm_ptr);
	lrm_report_reject(bridge, mgr_address, LRM_REJ_BAD_KEY);
	free(result);
	return;
    }
    if (bridge->links[link].cookie) {
	lrm_reject(pak, bridge, link, LRM_REJ_INUSE, correlator, lnm_ptr);
	lrm_report_reject(bridge, mgr_address, LRM_REJ_INUSE);
	free(result);
	return;
    }
    if (result->routing) {
	if (result->routing != (uchar *)-1L) {
	    rif = (srbroute_t *)result->routing;
	    length = rif->rc_blth & RC_LTHMASK;
	    if (length > 2) {
                srb_triplet_t srb_triplet;
		/*
		 * There is a real RIF.  We must see if we are part of the path.
		 */
		found = FALSE;
		ptr = (uchar *)rif->rd;
                srbV.srb_sb_get_triplet(bridge->idb1->firstsw, &srb_triplet);
		rng_br_rng_f = (((srb_triplet.remoteRing & 0xfff) << 20) |
				((srb_triplet.bridgeNum  & 0x00f) << 16) |
				((srb_triplet.localRing  & 0xfff) << 4));
		rng_br_rng_b = (((srb_triplet.localRing  & 0xfff) << 20) |
				((srb_triplet.bridgeNum  & 0x00f) << 16) |
				((srb_triplet.remoteRing & 0xfff) << 4));
		get_rif_str((uchar *)rif, rif_string);
		for (i = 2; i < length/2; i++, ptr += 2) {
		    rd_info = GETLONG(ptr);
		    if ((!RD_MISMATCH(rng_br_rng_f, rd_info))  ||
			(!RD_MISMATCH(rng_br_rng_b, rd_info))) {
			if (lnm_ptr != NULL) {
			    vrnf = srb_triplet.remoteRing;
			    vrnb = srb_triplet.localRing;
			    for (vrg = (tr_vgrouptype *) vringQ.qhead;
				 vrg; vrg = vrg->next) {
			        if ((vrg->vrn == vrnf) || (vrg->vrn == vrnb)) {
				    found = FALSE;
				    break;
				}
			    }
			    break;
			}
			found = TRUE;
			break;
		    }
		}
		if (found) {
		    lrm_reject(pak, bridge, link, LRM_REJ_CROSSES_ME, correlator, lnm_ptr);
		    lrm_report_reject(bridge, mgr_address, LRM_REJ_CROSSES_ME);
	            free(result);
		    return;
		}
	    }
	    /*
	     * Insert the specified RIF into the RIF cache.  This RIF is
	     * marked as HANDCRAFTed to make sure that it does not go
	     * away while the LLC2 connection is active.  We will kill it
	     * when we are done with it.
	     */
	    rif = (srbroute_t *)result->routing;
	    rif->rc_dlf ^= RC_D;
	    if (is_fddi(pak->if_input->hwptr))
		ieee_swap(mgr_address, addr);	
	    else
		ieee_copy(mgr_address, addr);
	    rif_update(pak->if_input->hwptr, 0, addr, NULL, rif, RIF_LNM);
	} else {
	    if (is_fddi(pak->if_input->hwptr))
		ieee_swap(mgr_address, addr);
	    else
		ieee_copy(mgr_address, addr);
	    rif_update(pak->if_input->hwptr, 0, addr, NULL, NULL, RIF_LNM);
	}
    }
    
    /*
     * Open the LLC2 connection back to the Lan Network Manager.
     */

    if (is_fddi(pak->if_input->hwptr))
	mgr_address[0] &= ~FDDIT_RII;
    else
	mgr_address[0] &= ~TR_RII;

    lnmllcbuginf("\nIBMNM: Opening connection to %e on %s", mgr_address, 
	   pak->if_input->namestring);
    
    snpa_init(&mgrAddr, STATION_IEEE48, TARGET_UNICAST, mgr_address);
    snpa_clear(&myAddr);		/* llc will use interface address */
    
    if (lnm_ptr != NULL) {
        /* CLSI compliant path.  Here initiate the open_req - to prepare
         * for connection establishment via CLS save the result pointer
	 * within the lnm_t structure.
         */
        if ((dup = pak_duplicate(pak))) {
	    lnm_ptr->conn_pend = TRUE;
            lnm_ptr->cs_result = (uchar *)result;
	    lnm_ptr->initial_correlator = *result->correlator;
	    if (result->numalt)
	        lnm_ptr->initial_num_alt_lnm = *result->numalt;
	    lnm_ptr->linkid = link;
            lnm_ptr->cs_pak = dup;
            lnm_send_req_open_stn(lnm_ptr);
        }
        else { 
            free(result);
        }
    }
    else {
	/*
         * LLC2 legacy path for RSRB support
	 */
        cookie = llc2_open(SAP_IBMNM, SAP_IBMNM, &mgrAddr, &myAddr,
		           pak->if_input, LINK_IBMNM, 8,
			   ibmnm_link_state_change, TRUE, FALSE);
        if (!cookie) {
            lnmllcbuginf("\nIBMNM: Creation of the LLC connection failed.");
	    lrm_reject(pak, bridge, link, LRM_REJ_CANT_ESTAB,
		       correlator, lnm_ptr);
	    lrm_report_reject(bridge, mgr_address, LRM_REJ_CANT_ESTAB);
	    free(result);
	    return;
        }
        create_link(bridge, link, cookie, mgr_address, pak->if_input->hwptr);
        /*
         * Set reporting values specified in the initial frame.
         */
        for (i = 0; i < result->numports; i++) {
            idb = address_to_idb(result->ports[i].portid);
	    if (result->ports[i].repclass) {
	        if (idb == bridge->idb1)
	            bridge->links[link].reporting_idb1 =
		        GETLONG(result->ports[i].repclass);
	        else if (idb == bridge->idb2)
		    bridge->links[link].reporting_idb2 =
		        GETLONG(result->ports[i].repclass);
	    }
        }
        if (result->numalt)
	    bridge->num_alt_lanmgrs = *result->numalt;
        lrm_accept(bridge, link, correlator);
        lrm_new_reporting_link(bridge, link);
        lrm_find_controlling(bridge, LRM_CNTL_NEW);
        free(result);
     }    
}

void ibmnm_lrm_conn_fail(lnm_t *lnm_ptr)
{    
    int link;
    paktype *pak;
    lrm_pb *result;
    bridge_info *bridge;
    uchar mgr_address[IEEEBYTES];
    uchar my_address[IEEEBYTES];
    ushort correlator;
    
    pak = lnm_ptr->cs_pak;
    result = (lrm_pb *)lnm_ptr->cs_result;
    bcopy(&lnm_ptr->dlc_id.client_mac_address, mgr_address, IEEEBYTES);
    bcopy(&lnm_ptr->dlc_id.server_mac_address, my_address, IEEEBYTES);
    bridge = address_to_bridge(my_address);
    link = lnm_ptr->linkid;
    correlator = lnm_ptr->initial_correlator;;

    lnmllcbuginf("\nIBMNM: Creation of the LLC connection failed.");
    lrm_reject(pak, bridge, link, LRM_REJ_CANT_ESTAB, correlator, lnm_ptr);
    lrm_report_reject(bridge, mgr_address, LRM_REJ_CANT_ESTAB);
    lnm_ptr->cs_result = NULL;
    free(result);
    lnm_ptr->cs_pak = NULL;    
    retbuffer(pak);
    return;    
}

void ibmnm_lrm_conn_estab(lnm_t *lnm_ptr)
{    
    llctype *cookie;
    int link, i;
    uchar mgr_address[IEEEBYTES];
    uchar my_address[IEEEBYTES];
    ushort correlator;
    hwidbtype *idb;
    lrm_pb *result;
    bridge_info *bridge;
    paktype *pak;

    pak = lnm_ptr->cs_pak;
    bcopy(&lnm_ptr->dlc_id.client_mac_address, mgr_address, IEEEBYTES);
    bcopy(&lnm_ptr->dlc_id.server_mac_address, my_address, IEEEBYTES);
    bridge = address_to_bridge(my_address);
    result = (lrm_pb *)lnm_ptr->cs_result;

    link = lnm_ptr->linkid;
    correlator = lnm_ptr->initial_correlator;
    cookie = (llctype *)lnm_ptr->u_cep_id;
 
    create_link(bridge, link, cookie, mgr_address, pak->if_input->hwptr);    
    /*
     * Set reporting values specified in the initial frame.
     */
    for (i = 0; i < result->numports; i++) {
	idb = address_to_idb(result->ports[i].portid);
	if (result->ports[i].repclass) {
	    if (idb == bridge->idb1)
		bridge->links[link].reporting_idb1 = GETLONG(result->ports[i].repclass);
	    else if (idb == bridge->idb2)
		bridge->links[link].reporting_idb2 = GETLONG(result->ports[i].repclass);
	}
    }
    if (result->numalt)
	bridge->num_alt_lanmgrs = lnm_ptr->initial_num_alt_lnm;

    lrm_accept(bridge, link, correlator);
    lrm_new_reporting_link(bridge, link);
    lrm_find_controlling(bridge, LRM_CNTL_NEW);

    lnm_ptr->cs_result = NULL;
    free(result);
    lnm_ptr->cs_pak = NULL;
    retbuffer(pak);
    return;
}

/*
 * ibmnm_lrm_input 
 *
 * This is the main parsing routine for all I frames directed at the Lan
 * Reporting Mechanism.  It will call the necessary secondary parsing
 * routines to determine whether or not all required parameters are
 * present.  If all parameters are present, it will then proceed to
 * precess and respond to the frame.
 */
void ibmnm_lrm_input (llctype *cookie, ibmnm_vector *vector)
{
    int link, error;
    ushort correlator;
    ibmnm_vector *offset;
    lrm_pb *result;
    bridge_info *bridge;
    lnm_t *lnm_ptr;
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
				lrm_to_string(GETSHORT(&vector->command)),
				bridge, link); 
    if (!(result = malloc_named(sizeof(lrm_pb) +
				    RSRB_MAXRINGS * sizeof(lrm_pi_pb), lnm_cb)))
	return;
    error = 0;

    switch (GETSHORT(&vector->command)) {
      case LRM_REQ_STATUS:
	if (!ibmnm_parse_request_status(vector, &correlator, &error, &offset)) {
	    lrm_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	if (!lrm_parse_vectors(vector, LRM_R_REQ_STATUS, LRM_O_REQ_STATUS, result, &error, &offset)) {
	    lrm_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	lrm_report_status(bridge, link, correlator);
	break;

      case LRM_SET_PARAM:
	/*
	 * Parse the set parameters command, and check the privilege of
	 * the sender.
	 */
	if (!ibmnm_parse_set_request(vector, &correlator, &error, &offset)) {
	    lrm_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	if (!ibmnm_changes_allowed(bridge, link)) {
	    lrm_error_frame(bridge, link, correlator, SET_ERR_UNAUTH_REQ, GETSHORT(&vector->length));
	    lrm_control_breach(bridge, link);
	    break;
	}
	if (!lrm_parse_vectors(vector, LRM_R_SET_PARAM, LRM_O_SET_PARAM, result, &error, &offset)){
	    lrm_error_frame(bridge, link, correlator, error, OFFSET(offset, vector));
	    break;
	}
	if (!do_set_lrm_parameters(bridge, link, result, &error)) {
	    lrm_error_frame(bridge, link, correlator, error, GETSHORT(&vector->length));
	    break;
	}
	lrm_params_set(bridge, correlator);
	lrm_params_changed(bridge, link, result);
	break;

      case LRM_SET_REP_PNT:
	/*
	 * This packet MUST be a UI packet, and not on an existing link.
	 */
	ibmnm_parse_correlator(vector, &correlator);
	lrm_invalid_request(cookie, correlator, LRM_INVALID_BAD_STATE);
	break;

      case LRM_CLOSE_LINK:
	/*
	 * Bye.
	 */
	destroy_link(bridge, link);
	lnm_ptr = ucepid2lnm((uint) cookie);
	if (lnm_ptr != NULL) {
	    lnm_ptr->linkid = 0;
	    lnm_send_close_stn(lnm_ptr);
	}
	else {
	    llc2_close(cookie);
        }
	lrm_find_controlling(bridge, LRM_CNTL_CLOSE);
	break;

      default:
	lnmllcbuginf(respvec, GETSHORT(&vector->command));
	lrm_parsing_error(bridge, link, GETSHORT(&vector->command), LRM_PARSE_UNKNOWN);
	break;
    }
    free(result);
}

void lrm_find_ui_client_addr(lnm_t *lnm_ptr, uchar *client)
{
    UDataStnInd_t *udata_stn;
    
    udata_stn = (UDataStnInd_t *)lnm_ptr->clsi_msg;
    
    bcopy((uchar *)lnm_ptr->clsi_msg + udata_stn->fRemoteMacOffset,
	  client, udata_stn->fLocalMacLength);
}

void lrm_find_ui_server_addr(lnm_t *lnm_ptr, uchar *server)
{
    UDataStnInd_t *udata_stn;
    
    udata_stn = (UDataStnInd_t *)lnm_ptr->clsi_msg;
    
    bcopy((uchar *)lnm_ptr->clsi_msg + udata_stn->fLocalMacOffset,
	  server, udata_stn->fRemoteMacLength);
}


#ifdef IBMNM_DEBUG
/**********************************************************************
 *
 *		Lan Reporting Manager Server Test Code
 *
 * All code past here implements the Lan Network Manager side of this
 * protocol.
 *
 * Warning: Until this test code to simulate the IBM PC Lan Network
 *          Manager is made CLSI compliant it is dysfunctional and
 *          can no longer be used to test out the LNM server code!!!
 *
 **********************************************************************/
#include "ttysrv.h"

/*
 * Build and send a "Set Reporting Point" message.  This should elicit a
 * "Lan Manager Accepted" response, or a "Lan Manager Rejected" response.
 */
void tibmnm_set_reporting_point(
    jmp_buf *far,
    uchar *bridge_address,
    hwidbtype *idb)
{
    jmp_buf near;
    int numalt, link, fc1, fc2, riflen;
    char buffer[IBMNM_MAXBUF], key[10], *ptr;
    uchar route[32], other_address[IEEEBYTES];
    ibmnm_vector *vector, *sv, *sv2, *sv3;
    paktype *pak;

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

    sv = insert_major(pak, LRM_SET_REP_PNT);

    if (yes_or_no2(&near, "Use Correlator", TRUE, TRUE))
	sv = insert_correlator(sv, test_correlator++);

    if (yes_or_no2(&near, "Use Num Alt", TRUE, TRUE)) {
	numalt = get_num_in_range2(&near, "    Num Alt", 10, 0, -1L, 0);
	sv = insert_short(sv, LRM_SV_NUM_ALT_LM, numalt);
    }

    if (yes_or_no2(&near, "Use Routing", TRUE, TRUE)) {
	ptr = read_string2(&near, buffer, "    Routing", ".");
	riflen = 0;
	if (ptr && !term_char(*ptr))
	    for ( ; ; riflen += 2) {
		ptr = get_hex_num(ptr, (ushort *)&route[riflen]);
		if (term_char(*ptr)) {
		    riflen += 2;
		    break;
		}
		ptr++;
	    }
	sv = insert_bytes(sv, LRM_SV_ROUTING, route, riflen);
    }

    sv2 = insert_header(sv, LRM_SV_LINK_INFO);
    if (yes_or_no2(&near, "Use Link No", TRUE, TRUE)) {
	link = get_num_in_range2(&near, "    Link No", 10, 0, -1L, 0);
	sv2 = insert_short(sv2, LRM_SV_LINK_ID, link);
    }

    if (yes_or_no2(&near, "Use Key", TRUE, TRUE)) {
	ptr = read_string2(&near, key, "    Key", "00000000");
	sv2 = insert_bytes(sv2, LRM_SV_KEY, ptr, 8);
    }

    printf("Use Port 1: Yes\n");
    printf("    Port 1: %e", bridge_address);
    fc1 = get_num_in_range2(&near, "    Function Class", 16, 0, -1L, 0x8000011a);
    sv3 = insert_header(sv2, LRM_SV_PORT_INFO);
    sv3 = insert_address(sv3, bridge_address);
    sv3 = insert_long(sv3, LRM_SV_REP_CLASS, fc1);
    PUTSHORT(&sv2->length, (uchar *)sv3 - (uchar *)sv2);
    sv2 = sv3;

    if (yes_or_no2(&near, "Use Port 2", TRUE, TRUE)) {
	get_macaddr(&near, "    Port 2", other_address, "0000.3000.5735");
	fc2 = get_num_in_range2(&near, "    Function Class", 16, 0, -1L, 0x8000011a);
	sv3 = insert_header(sv2, LRM_SV_PORT_INFO);
	sv3 = insert_address(sv3, other_address);
	sv3 = insert_long(sv3, LRM_SV_REP_CLASS, fc2);
	PUTSHORT(&sv2->length, (uchar *)sv3 - (uchar *)sv2);
	sv2 = sv3;
    }

    PUTSHORT(&sv->length, (uchar *)sv2 - (uchar *)sv);
    sv = sv2;

    sv = tibmnm_add_arbitrary(&near, sv);
    vector->length = (uchar *)sv - (uchar *)vector;
    pak->linktype = LINK_IBMNM;
    pak->if_output = idb->firstsw;
    /* It may be a problem ???? */

    if (pak_encap(idb, pak, vector->length, (long)bridge_address)) {
	printf("Sending set reporting point to %e.\n", bridge_address);
	datagram_out(pak);
    } else {
	printf("Cannot send set reporting point to %e.\n", bridge_address);
	retbuffer(pak);
    }
}

/*
 * Build and send a "Request LRM Status" message.  This should elicit a
 * "Report LRM Status" response.
 */
static void tibmnm_req_lrm_status (
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

    sv = insert_major(pak, LRM_REQ_STATUS);
    if (yes_or_no2(&near, "Use Correlator", TRUE, TRUE))
	sv = insert_correlator(sv, test_correlator++);
    if (yes_or_no2(&near, "Include 'All'", TRUE, TRUE)) {
	all = get_num_in_range2(&near, "    All", 16, 0, 65535, 0);
	sv = insert_short(sv, IBMNM_ALL, all);
    }
    
    sv = tibmnm_add_arbitrary(&near, sv);
    vector->length = (uchar *)sv - (uchar *)vector;
    ibmnm_llc2_send(test_cookie, pak, vector->length);
}

/*
 * Build and send a "Set LRM Parameters" message.  This should elicit a
 * "LRM Parameters Set" response.
 */
static void tibmnm_set_lrm_parms (
    jmp_buf *far)
{
    jmp_buf near;
    paktype *pak;
    ibmnm_vector *vector, *sv, *sv2, *sv3;
    int ring, fc, ef, numalt, link, perm;
    uchar address[IEEEBYTES];
    char key[30];

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

    sv = insert_major(pak, LRM_SET_PARAM);
    if (yes_or_no2(&near, "Use Correlator", TRUE, TRUE))
	sv = insert_correlator(sv, test_correlator++);

    /*
     * Changing Ring Number
     */
    if (yes_or_no2(&near, "Use Port", FALSE, TRUE)) {
	get_macaddr(&near, "    Port", address, "0000.3000.5735");
	sv = insert_address(sv, address);
    }
    if (yes_or_no2(&near, "Use Ring", FALSE, TRUE)) {
	ring = get_num_in_range2(&near, "    Ring", 16, 0, 65535, 0);
	sv = insert_ring(sv, ring);
    }

    /*
     * Changing other things.
     */
    if (yes_or_no2(&near, "Use Num Alt", TRUE, TRUE)) {
	numalt = get_num_in_range2(&near, "    Num Alt", 10, 0, 65535, 0);
	sv = insert_short(sv, LRM_SV_NUM_ALT_LM, numalt);
    }

    if (yes_or_no2(&near, "Use Reporting Link", TRUE, TRUE)) {
	sv2 = insert_header(sv, LRM_SV_LINK_INFO);
	if (yes_or_no2(&near, "Use Link No", TRUE, TRUE)) {
	    link = get_num_in_range2(&near, "    Link No", 10, 0, 65535, 0);
	    sv2 = insert_short(sv2, LRM_SV_LINK_ID, link);
	}
	
	if (yes_or_no2(&near, "Use Key", TRUE, TRUE)) {
	    read_string2(&near, key, "    Key", "00000000");
	    sv2 = insert_bytes(sv2, LRM_SV_NEW_KEY, key, 8);
	}
	
	if (yes_or_no2(&near, "Use Port Info", TRUE, TRUE)) {
	    sv3 = insert_header(sv2, LRM_SV_PORT_INFO);
	    if (yes_or_no2(&near, "Use Port", TRUE, TRUE)) {
		get_macaddr(&near, "    Port", address, "0000.3000.5735");
		sv3 = insert_address(sv3, address);
	    }
	    if (yes_or_no2(&near, "Use Function Class", TRUE, TRUE)) {
		fc = get_num_in_range2(&near, "    Function Class", 16, 0, -1L, 0x8000011a);
		sv3 = insert_long(sv3, LRM_SV_REP_CLASS, fc);
	    }
	    if (yes_or_no2(&near, "Use Enabled Functions", TRUE, TRUE)) {
		ef = get_num_in_range2(&near, "    Enabled Functions", 16, 0, -1L, 0x8000011a);
		sv3 = insert_long(sv3, LRM_SV_ENAB_FUNC, ef);
	    }
	    PUTSHORT(&sv2->length, (uchar *)sv3 - (uchar *)sv2);
	    sv2 = sv3;
	}
	PUTSHORT(&sv->length, (uchar *)sv2 - (uchar *)sv);
	sv = sv2;
    }

    if (yes_or_no2(&near, "Use Permanent", FALSE, TRUE)) {
	perm = get_num_in_range2(&near, "    Permanent", 16, 0, 255, 0);
	sv = insert_byte(sv, IBMNM_PERMANENT, perm);
    }
    
    sv = tibmnm_add_arbitrary(&near, sv);
    vector->length = (uchar *)sv - (uchar *)vector;
    ibmnm_llc2_send(test_cookie, pak, vector->length);
}

/*
 * Build and send a "Close Reporting Link" message.  This should not
 * elicit any response, except the closing of the LLC2 connection.
 */
static void tibmnm_close_link (
    jmp_buf *far)
{
    jmp_buf near;
    paktype *pak;
    ibmnm_vector *vector, *sv;

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

    sv = insert_major(pak, LRM_CLOSE_LINK);
    sv = tibmnm_add_arbitrary(&near, sv);
    vector->length = (uchar *)sv - (uchar *)vector;
    ibmnm_llc2_send(test_cookie, pak, vector->length);
}

/*
 * Ask which LRM command to send.
 */
void tibmnm_lrm (jmp_buf *far)
{
    jmp_buf near;
    volatile boolean done;
    char buffer[IBMNM_MAXBUF];

    /*
     * The abort code.
     */
    if (setjmp(&near)) {
	longjmp(far, 1);
    }

    done = FALSE;
    while (!done) {
	printf("What type of LRM packet would you like to send?\n\n");
	printf("1) Request LRM Status\n");
	printf("2) Set LRM Parameters\n");
	printf("3) Close Reporting Link\n");
	if (!rdtty(buffer, IBMNM_MAXBUF))
	    longjmp(far, 1);
	
	if (null(buffer))
	    continue;

	switch (tolower(*buffer)) {
	  case '0': done = TRUE;                  break;
	  case '1': tibmnm_req_lrm_status(&near); break;
	  case '2': tibmnm_set_lrm_parms(&near);  break;
	  case '3': tibmnm_close_link(&near);     break;
	}
    }
}

#endif
