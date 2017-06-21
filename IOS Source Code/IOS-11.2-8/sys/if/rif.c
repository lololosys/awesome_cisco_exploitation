/* $Id: rif.c,v 3.11.10.13 1996/09/11 17:39:03 snyder Exp $
 * $Source: /release/112/cvs/Xsys/if/rif.c,v $
 *------------------------------------------------------------------
 * rif.c -- 802.5 Token Ring RIF Cacheing
 *
 * 13-June-1989, David R. Hampton/Eric B. Decker
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rif.c,v $
 * Revision 3.11.10.13  1996/09/11  17:39:03  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.11.10.12  1996/08/28  12:48:47  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.10.11  1996/08/26  15:09:10  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.11.10.10  1996/07/23  18:45:45  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.11.10.9  1996/07/23  13:23:53  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.11.10.8  1996/07/22  01:32:44  psandova
 * CSCdi59090:  Non-explorer packets are being flushed with explorers
 * Branch: California_branch
 * Valid multicast packets that should be handed to the protocol stack
 * are instead being diverted to the srb module and are flushed by the
 * explorer control mechanism.
 *
 * Revision 3.11.10.7  1996/07/09  05:58:49  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.11.10.6  1996/06/17  23:31:38  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.11.10.5  1996/06/01  01:31:49  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.11.10.4  1996/05/23  12:57:26  rbatz
 * CSCdi56744:  Spurious Accesses in rif_loop_check().
 * Branch: California_branch
 *
 * Revision 3.11.10.3  1996/05/17  11:23:26  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.2.3  1996/05/14  18:16:58  asethi
 * CSCdi55032:  Duplicated ring# in rif found at Ameritech, see CSCdi48577
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.2  1996/04/26  15:20:29  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.1  1996/04/03  14:38:43  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.10.2  1996/05/09  14:30:21  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.11.10.1.18.1  1996/04/27  07:04:04  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.11.10.1.8.1  1996/04/08  01:54:53  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.11.10.1  1996/03/18  20:14:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.14.1  1996/03/22  09:37:27  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.11  1996/02/24  05:24:02  asethi
 * CSCdi48577:  Spanning configd on 2 tr ports connected to same ring
 * causes loops
 * rif_loop_check modified to check for loop
 *
 * Revision 3.10.6.1  1996/03/05  06:14:02  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.10.2.1  1996/02/27  21:08:31  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.10  1996/02/07  16:12:44  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.9  1996/02/01  06:04:02  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.8.2.1  1996/01/24  22:18:55  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.8  1996/01/18  23:01:00  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7.4.1  1996/02/15  19:13:58  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.7  1995/12/16  22:48:14  motto
 * CSCdi35413:  Alignment errors in R/SRB process level code
 *
 * Revision 3.6  1995/12/13  02:28:28  ppearce
 * CSCdi45473:  %SYS-3-SUPNONE: Registry 6 doesnt exist output during boot
 *   (1) Rename fddi_srb_in() to fddi_to_tr()
 *         to parallel the tr_to_fddi() routine that already exists
 *   (2) Remove srt_fddi_in() registry from srt/srt_registry.reg
 *         and add media_fddi_to_tr() registry into h/media_registry.reg
 *
 * Revision 3.5  1995/12/11  10:20:21  ppearce
 * CSCdi44095:  SRB proxy-explorer causes FDDI packet drops
 *   rif_explore() should call tr_to_media_out() rather than datagram_out()
 *     so that packets can be correctly formatted for output to FDDI
 *
 *     - Rename srb_out() to tr_media_out()
 *     - Change srb_out() to invoke tr_fddi_out() for FDDI output
 *     - Move adjust_pak() code into tr_fddi_out()
 *
 * Revision 3.4  1995/12/08  15:53:24  motto
 * CSCdi45258:  ALIGN-3-SPURIOUS: Spurious memory access made at
 * 0x60075fe0 reading
 *
 * Revision 3.3  1995/11/17  09:30:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:59  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:52:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:03:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/08/10  14:35:52  richl
 * CSCdi38547:  [no] multiring [ip|ipx] should reset fddi
 *
 * Revision 2.7  1995/08/07  05:36:43  richl
 * CSCdi38315:  makefile rework
 *              move inlines to tring_inline and utilities to rif_util.c
 *
 * Revision 2.6  1995/07/25  21:50:24  jacobt
 * CSCdi37644:  cleanup in rif_explore()
 *
 * Revision 2.5  1995/07/18  06:46:23  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.4  1995/07/14 02:48:32  jacobt
 * CSCdi36703:  replace malloc() by malloc_named()
 *
 * Revision 2.3  1995/07/06  04:32:28  bchan
 * CSCdi36758:  qllc crashes on accessing null pointer
 *
 * Revision 2.2  1995/06/13  03:28:16  alam
 * CSCdi35570:  Remove extraneous functions
 * Use llc_process_test and llc_process_xid for FDDI as well as ethernet
 * and FR.
 * Remove FDDI packet debug.
 *
 * Revision 2.1  1995/06/07  20:54:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "ether.h"
#include "tring.h"
#include "fddi.h"
#include "network.h"
#include "../srt/srb_core.h"
#include "../srt/srb_multiring.h"
#include "rif.h"
#include "rif_inline.h"
#include "rif_util.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_rif.h"
#include "../srt/srt_debug.h"
#include "../ibm/sdlc.h"
#include "../llc2/llc2.h"
#include "../srt/lnx.h"
#include "../srt/srt_registry.h"
#include "../if/msg_tr.c"		/* Not a typo, see logger.h */

static const char OnlyOnSRB[] = "\n%%\"%s\" only valid on Token Rings and FDDI";


/*
 * Storage
 */
#define MAX_RIF_VALIDATE_PROBE_SENT 60
static int rif_validate_probe_sent;

riftype *riftable[NETHASHLEN];  /* table of riftype pointers */
static int rif_timeout;		/* max age of rif entries    */
int rif_validation_age;
static int rif_timeout_update;
static int rif_timeout_lookup;
static int rif_timeout_explore;
static int rif_timeout_age_long;
static boolean rif_validate_enable_age;
boolean rif_validate_enable;
boolean rif_validate_route_cache;
static uchar xid_rif[2]    = {RC_DEF_ALLRINGS, RC_TR_LFDEF};
static uchar xid_rif_16[2] = {RC_DEF_ALLRINGS, RC_TR16_LFDEF};
static queuetype free_rifQ;
static boolean createrifbuffer;
static riftype allroutes_rif, spanning_rif, dec_func_rif;
boolean tr_rif_xid_explorer;

/*
 * Forward references
 */
static paktype *tr_setup_test_cmd(uchar *, uchar *, hwidbtype *, uchar *);
static paktype *tr_setup_loop(uchar *daddr, uchar *saddr,
			      hwidbtype *interface);

/*
 * If true, rif_trunacate will always force rifs to be in the forward
 * direction (sigh)
 */
boolean rif_always_forward;
boolean global_proxy_explorer_enable;

#define RIF_DEBUG(interface, da, sa) \
               (rif_debug && rif_debug_filter(&rif_debug, interface, da, sa))

static boolean rif_debug_filter (
    boolean *pflag,
    hwidbtype *interface,
    uchar *da,
    uchar *sa)
{
    debug_list_t *list;

    list = debug_listq_find(pflag);
    if (list == NULL) {
	return (TRUE);
    }
    if (list->idb != NULL && interface->firstsw != NULL && 
	list->idb != interface->firstsw) {
	return (FALSE);
    }
    if (list->access_list == 0) {
	return (TRUE);
    }
    if (da == NULL) 
	da = (uchar *)zeromac;
    if (sa == NULL)
	sa = (uchar *)zeromac;
    return(reg_invoke_xmacaccesscheck_gen(list->access_list,
			       (mac_address_type *)da,
			       (mac_address_type*)sa,
			       NULL, 0, 0));
}

/*
 * add_rif_buffers
 * Pre-initialize buffers for entries in the rif cache.  We must do this
 * because they cannot be malloced at interrupt level as the packets
 * arrive.
 */

static void add_rif_buffers (void)
{
    int i;
    riftype *entry;
    leveltype level;

    createrifbuffer = FALSE;
    level = raise_interrupt_level(NETS_DISABLE);
    for (i = free_rifQ.count; i < FREE_RIF_HW; i++) {
	entry =  malloc_named(sizeof(riftype), "RIF Cache");
	if (entry)
	    enqueue(&free_rifQ, entry);
    }
    reset_interrupt_level(level);
}

/*
 * rif_explore
 */

static void rif_explore (
    uchar *address,
    hwidbtype *interface,
    uchar *rif,
    int flags,
    int linktype,
    idbtype *idb,
    long protocol_address)
{
    paktype *pak;

    if (!interface)
	return;
    
    if (!rif && linktype != LINK_ILLEGAL && reg_used_no_rif(linktype)) {
	if (RIF_DEBUG(interface, address, NULL))
	    buginf("\nRIF: L Sending explorer linktype %d for %e",
		   linktype, address);
	reg_invoke_no_rif(linktype,
 			  idb, protocol_address);
	return;
    }
    if (rif == NULL ) {
	rif = ((interface->status & IDB_TR) ||
	       (interface->status & IDB_FDDI)) ? xid_rif : xid_rif_16;
    }
    if (!(flags & RIF_USE_LOOP)) {
	if (!tr_rif_xid_explorer) {
	    if (is_fddi(interface))
		pak = tr_setup_test_cmd(address,
					interface->bit_swapped_hardware,
					interface, rif);
	    else
		pak = tr_setup_test_cmd(address, interface->hardware,
					interface, rif);
	    if (!pak) {
		return;
	    }
	}  else {
	    if (is_fddi(interface))
		pak = tr_setup_xid(address, interface->bit_swapped_hardware,
				   interface, rif, FALSE, 0, SAP_NULL,
				   SAP_SMF, TRUE);
	    else
		pak = tr_setup_xid(address, interface->hardware, interface,
				   rif, FALSE, 0, SAP_NULL, SAP_SMF, TRUE);
	    if (!pak)
		return;
	}
	if (RIF_DEBUG(interface, address, NULL))
	    buginf("\nRIF: L Sending XID/TEST for %e", address);
 	if (flags & RIF_USE_PAK_PRIORITY) {
 	    pak->flags |= PAK_PRIORITY;
 	}
	tr_to_media_out(pak);
    } else {
	pak = tr_setup_loop(address, interface->hardware, interface);
	if (!pak)
	    return;
	if (RIF_DEBUG(interface, address, NULL))
	    buginf("\nRIF: L Sending LOOP for %e", address);
	tr_to_media_out(pak);
    }
}

#define RIF_VALIDATE_DEBUG(entry, string) \
          if (RIF_DEBUG(entry->idb, entry->hardware, NULL)) \
	      buginf("\nRIF: %s da=%e", string, entry->hardware);

static inline void rif_validate_clear_timers (riftype *entry)
{
    GET_TIMESTAMP(entry->update_time);
    TIMER_STOP(entry->validate_time);
    TIMER_STOP(entry->explorer_time);
    TIMER_STOP(entry->piggyback_lookup_time);
}

/* rif_validate_update
 * 
 * Called if a packet with a different route received
 *
 * If the entry has  been updated recently (rif_timeout_update),
 *    ignore a new route.
 *
 * If the validation timer is already  running and received packet 
 *    is XID/TEST response we have several choices:
 *    1. ignore a new route.
 *    2. reset timers and ignore a new route. It's safe to assume that 
 *       the old route is reachable.
 *    3. overwrite  the old route in order to get symmetric routes.
 * Let's pickup  the option #2.
 *
 * If the validation timer is not started,
 *    Send out specifically routed explorer and start the validation timer.
 * 
 */

static void rif_validate_update (riftype *entry, srbroute_t *new_route,
				 sap_hdr *llc)
{

    int length;

    if (entry->flags != RIF_AGED) {
	return;
    }
    if (entry->idb == NULL) {
	return;
    }
    if (!rif_validate_enable) {
	return ;
    }
    if (TIMER_RUNNING_AND_SLEEPING(entry->validate_time)) {
	if (llc &&
            (llc->ssap & SAP_RESPONSE) &&
	    ((llc->control == (LLC1_XID | P_BIT)) || 
	    (llc->control ==  (LLC1_TEST | P_BIT)))) {
	    RIF_VALIDATE_DEBUG(entry, "V-U got response via asymmetric route");
	    rif_validate_clear_timers(entry);
	}
	return;
    }
    length = new_route ? (new_route->rc_blth & RC_LTHMASK) : 0;
    if (length <= 2) 
	length = 0;
    if (TIMER_RUNNING_AND_AWAKE(entry->validate_time)) {
	entry->validate_flags |= RIF_VALIDATE_NEW_ROUTE;
	memset(entry->rif, 0, SRB_MAXRIF);
	entry->length = length;
	if (length) {
	    bcopy(new_route, entry->rif, length);
	} 
	RIF_VALIDATE_DEBUG(entry, "V-U use new route");
	rif_validate_clear_timers(entry);
	return;
    }
    if (rif_contains_rif((srbroute_t *)entry->rif, new_route)) {
	entry->validate_flags |= RIF_VALIDATE_NEW_ROUTE;
	entry->length = length;
	memset(entry->rif, 0, SRB_MAXRIF);
	if (length) {
	    bcopy(new_route, entry->rif, length);
	} else {
	    memset(entry->rif, 0, SRB_MAXRIF);
	}
	RIF_VALIDATE_DEBUG(entry, "V-U old route contains new");
	rif_validate_clear_timers(entry);
	return;
    }
    if (CLOCK_IN_INTERVAL(entry->update_time, rif_timeout_update)) {
	RIF_VALIDATE_DEBUG(entry, "V-U old route is fresh ");
	return;
    }
    RIF_VALIDATE_DEBUG(entry,
		       "V-U new route received, starting probe old route");
    TIMER_START(entry->validate_time, rif_validation_age);
    rif_explore(entry->hardware, entry->idb, entry->rif, 
		RIF_USE_PAK_PRIORITY, 0, NULL, 0);
    
}
/* rif_validate_lookup
 * 
 * Called from rif_lookup
 * If the validation timer is already  running,
 *    return
 * If the entry has not been updated recently (rif_timeout_lookup),
 *    send out a specifically routed explorer and start the validation timer.
 */

static boolean rif_validate_lookup (riftype *entry, int flags, int linktype,
				    idbtype *idb, long protocol_address)
{
    
    if (entry->flags != RIF_AGED) {
	return (TRUE);
    }
    if (entry->idb == NULL) {
	return (TRUE);
    }
    if (!rif_validate_enable) {
	return (TRUE);
    }
    if (CLOCK_IN_INTERVAL(entry->update_time, rif_timeout_lookup)) {
	RIF_VALIDATE_DEBUG(entry, "V-L fresh");
	return (TRUE);
    }
    if (TIMER_RUNNING_AND_AWAKE(entry->validate_time)) {
	RIF_VALIDATE_DEBUG(entry, "V-L expired");
	if (!TIMER_RUNNING_AND_SLEEPING(entry->explorer_time)) {
	    TIMER_START(entry->explorer_time, rif_timeout_explore);
	    rif_explore(entry->hardware, entry->idb, NULL, flags, 
			linktype, idb, protocol_address);
	}
	if (!rif_validate_enable_age) {
	    return (TRUE);
	}
	return (FALSE);
	
    }
    if (TIMER_RUNNING_AND_SLEEPING(entry->validate_time)) {
	RIF_VALIDATE_DEBUG(entry, "V-L probing");
	return (TRUE);
    }
    if (!TIMER_RUNNING(entry->piggyback_lookup_time)) {
	RIF_VALIDATE_DEBUG(entry, "V-L start piggybacking");
    	TIMER_START(entry->piggyback_lookup_time, rif_validation_age);
	return (TRUE);
    }
    if (TIMER_RUNNING_AND_SLEEPING(entry->piggyback_lookup_time)) {
	RIF_VALIDATE_DEBUG(entry, "V-L piggybacking");
	return (TRUE);
    }
    RIF_VALIDATE_DEBUG(entry, "V-L start probing");
    TIMER_START(entry->validate_time, rif_validation_age);
    rif_explore(entry->hardware, entry->idb, entry->rif,
		flags |	RIF_USE_PAK_PRIORITY, 0, NULL, 0);
    return (TRUE);
}

static inline void 
rif_validate_invalidate_route_cache (hwidbtype *interface,uchar *address,
				     uchar  *rif)
{
    idbtype *swidb = firstsw_or_null(interface);
    if (!rif_validate_route_cache) {
	return;
    }
    if (srb_multiring_none_enabled(swidb)) {
	return;
    }
    reg_invoke_bump_cache_version(interface, NULL, FALSE);
}

/* rif_validate_age
 * 
 * Return TRUE if the entry should be removed,
 * otherwise return FALSE.
 *
 * If the validate timer expired, 
 * let the ager remove the entry.
 *
 * If the update timer expired,
 *    try to re-probe the entry first,  before removing the entry.
 *
 * The user can configure an option, 
 * "don't remove the entry  until a new route became available".
 * Some of NEC cards would not respond to explorers 
 * if they think that a router shouldn't
 * send explorers at that time.
 * So, we will keep  and use "unvalidated" entry
 * until we get another route.
 */
static boolean rif_validate_age (riftype *entry)
{    
    if ((entry->flags != RIF_AGED) || (entry->idb == NULL)) {
	if (CLOCK_OUTSIDE_INTERVAL(entry->update_time, rif_timeout)) {
	    return (TRUE);
	} else {
	    return FALSE;
	}
    }
    if (!rif_validate_enable) {
	if (CLOCK_OUTSIDE_INTERVAL(entry->update_time, rif_timeout)) {
	    return (TRUE);
	} else {
	    return FALSE;
	}
    }
    if (entry->validate_flags & RIF_VALIDATE_NEW_ROUTE) {
	entry->validate_flags &= ~RIF_VALIDATE_NEW_ROUTE;
	rif_validate_invalidate_route_cache(entry->idb, entry->hardware,
					    entry->rif);
	RIF_VALIDATE_DEBUG(entry, "V-A new route, tell protocols");
    }
    if (!rif_validate_enable_age) {
	return (FALSE);
    }
    if (TIMER_RUNNING_AND_AWAKE(entry->validate_time)) {
	RIF_VALIDATE_DEBUG(entry, "V-A probe failed");
	rif_validate_invalidate_route_cache(entry->idb, entry->hardware, NULL);
	return (TRUE);
    }
    if (CLOCK_IN_INTERVAL(entry->update_time, rif_timeout)) {
	RIF_VALIDATE_DEBUG(entry, "V-A fresh");
	return (FALSE);
    }
    if (TIMER_RUNNING_AND_SLEEPING(entry->validate_time)) {
	RIF_VALIDATE_DEBUG(entry, "V-A probing");
	return (FALSE);
    }
    
    if (CLOCK_OUTSIDE_INTERVAL(entry->create_time, rif_timeout_age_long)) {
	RIF_VALIDATE_DEBUG(entry, "V-A very old");
	rif_validate_invalidate_route_cache(entry->idb, entry->hardware, NULL);
	return (TRUE);
    }
    if (rif_validate_probe_sent > MAX_RIF_VALIDATE_PROBE_SENT) {
	return (FALSE);
    }
    rif_validate_probe_sent++;
    TIMER_START(entry->validate_time, rif_validation_age);
    RIF_VALIDATE_DEBUG(entry, "V-A start probing");
    rif_explore(entry->hardware, entry->idb, entry->rif,
		RIF_USE_PAK_PRIORITY, 0, NULL, 0);
    return (FALSE);
}


/* This is a null address for easy coding in rif_lookup */
static uchar *rif_null_addr = (uchar *) "\0\0\0\0\0\0";

/*
 * rif_lookup_callback
 * Search the RIF cache for an entry for this address.  If not found,
 * then send out an XID packet.  It assumes that it is never called
 * with a multicast address.
 *
 * If interface is NULL an XID is not generated.
 *
 * This routine should only be called if srb_multiring is set.
 * The last comment is not correct. See lack2.c 
 */
riftype *rif_lookup_callback (
    uchar *address,
    hwidbtype *interface,
    uchar *sa,
    int flags,
    int linktype,
	idbtype *idb,
    long protocol_address)
{
    riftype *entry;
    int hash_entry;

    if (sa == NULL)
	sa = rif_null_addr;

    if (RIF_DEBUG(interface,address, sa))
	buginf("\nRIF: L checking da=%e, sa=%e", address, sa);

    if (is_ieee_bcast(address)) {
	if (flags & RIF_USE_ALLROUTES)
	    return(&allroutes_rif);
	return(&spanning_rif);
    }
    if (GETSHORT(address) == TRFUNC_UPPER) {
	if (flags & RIF_USE_DN_FUNC)
	    return(&dec_func_rif);
	else if (flags & RIF_USE_ALLROUTES)
	    return(&allroutes_rif);
	return(&spanning_rif);
    }

    /*
     * Pick a bucket and search it.  Look first for an explicit match by
     * interface (or, if the input interface is NULL, and the 'sa' is also
     * NULL, accept any non-virtual (real) interface).  If you can't find
     * one, then look again for an entry that doesn't specify an interface
     * at all. However, do the 'or' clause shown above only if proxy explorer
     * has been enabled in the router somewhere. This minimizes the effect
     * of this extra search effort to the cases that need it.
     */
    hash_entry = rifhash(address);
    /* paranoia code */
    entry = riftable[hash_entry];
    if (entry && !validmem(entry)) {
	riftable[hash_entry] = NULL;
	errmsg(&msgsym(RIFTBLTRASH, TR), "head", entry);
	return(NULL);
    }

    if (entry && entry->magic != RIF_MAGIC) {
	errmsg(&msgsym(RIFBADMAGIC, TR), entry->magic, "head", entry);
	riftable[hash_entry] = NULL;
	return(NULL);
    }

    while (entry) {
	/* more paranoia code */
	if (entry->next && !validmem(entry->next)) {
	    errmsg(&msgsym(RIFTBLTRASH, TR), "intermediate", entry->next);
	    entry->next = NULL;
	}
	if (entry->next && entry->next->magic != RIF_MAGIC) {
	    errmsg(&msgsym(RIFBADMAGIC, TR), entry->next->magic,
		   "intermediate", entry->next);
	    entry->next = NULL;
	}
	if (global_proxy_explorer_enable) {
	    if (ieee_equal_sans_rii(address, entry->hardware) &&
		ieee_equal_sans_rii(sa, entry->src_addr) &&
		((interface == entry->idb) ||
		 (!interface && (entry->idb) && (sa == rif_null_addr) &&
		  (!(entry->idb->status & IDB_VIRTUAL))))) {
		if (rif_validate_lookup(entry, flags, linktype, idb,
					protocol_address)) {
		    return(entry);
		} else {
		    return(NULL);
		}
	    }
	} else {
	    if (ieee_equal_sans_rii(address, entry->hardware) &&
		ieee_equal_sans_rii(sa, entry->src_addr) &&
		(interface == entry->idb)) {
		if (rif_validate_lookup(entry, flags, linktype, idb,
					protocol_address)) {
		    return(entry);
		} else {
		    return(NULL);
		}
	    }
	}
	entry = entry->next;
    }

    entry = riftable[hash_entry];
    while (entry) {
	if (ieee_equal_sans_rii(address, entry->hardware) &&
	    ieee_equal_sans_rii(sa, entry->src_addr) &&
	    (!entry->idb)) {
		if (rif_validate_lookup(entry, flags, linktype, idb,
					protocol_address)) {
		    return(entry);
		} else {
		    return(NULL);
		}
	}
	entry = entry->next;
    }

    /*
     * Nope, not here. Start route exploration and dump.
     */
    rif_explore(address, interface, NULL, flags, linktype, idb,
		protocol_address);
    return(NULL);
}

int rif_count_bridges (uchar *address, hwidbtype *interface)
{
    idbtype *swidb = firstsw_or_null(interface);
    riftype *rif;
    
    if (srb_multiring_none_enabled(swidb))
	return(0);
    rif = rif_lookup(address, interface, NULL, 0);
    if (!rif)
	return(0);
    if (rif->length < 6)
	return(0);
    return((rif->length - 4) / 2);
}


/*
 * f o r c e _ r i f _ f l a g
 *
 * force a rif entry to have a particular flag
 */

void force_rif_flag (uchar *da, uchar *sa, int flags)
{
    riftype *entry;

    entry = rif_lookup(da, NULL, sa, 0);
    if (!entry)
	return;
    GET_TIMESTAMP(entry->update_time);
    /*
     * We do not have to verify if the flag type is aged since
     * rif_update sets the time irrespective of the flag type.
     */
    entry->flags = flags;
}

static paktype *tr_setup_test_cmd (
    uchar *daddr,
    uchar *saddr,
    hwidbtype *interface,
    uchar *rif)
{
    tring_hdr *trframe;
    paktype *pak;
    llc1_test_t *test;
    int testsize;
    int riflen;

    riflen = rif ? (rif[0] & RC_LTHMASK) : 0;
    testsize = TRING_ENCAPBYTES + riflen + 3; /* dsap, ssap, control */
    if ((pak = getbuffer(testsize)) == NULL) {
	return(NULL);
    }

    /*
     * write the MAC header.
     */
    trframe = (tring_hdr *)
	(pak->network_start - TRING_ENCAPBYTES - riflen);
    trframe->ac = AC_PRI0;
    trframe->fc = FC_TYPE_LLC;
    ieee_copy_sans_rii(daddr, trframe->daddr);
    ieee_copy(saddr, trframe->saddr);

    /*
     * Write the RIF.
     */
    if (riflen) {
	bcopy(rif, trframe->data, riflen);
	trframe->saddr[0] |= TR_RII;
    }

    test = (llc1_test_t *)(pak->data_area + ENCAPBYTES);
    test->control      = LLC1_TEST | LLC1_P; 	/* assume cmd/poll */
    test->dsap = 0;
    test->ssap = 0;
    
    pak->if_output     = interface->firstsw;
    pak->enctype       = interface->enctype;
    pak->datagramstart = (uchar *) trframe;
    pak->datagramsize  = testsize;

    return (pak);
}

/*
 * tr_setup_xid
 * Setup an xid packet.  Mainly a matter of getting a buffer and
 * stuffing a lot of constants.
 * 
 * daddr is where this packet will go.  Saddr is who to say it is
 * from.
 *
 * withinsdllc indicates the frame is within an SDLLC interface
 * addrbyte is the "SDLC address" for the connection
 */

paktype *tr_setup_xid (
    uchar *daddr,
    uchar *saddr,
    hwidbtype *interface,
    uchar *rif,
    boolean withinsdllc,
    int addrbyte,
    uint dsap,
    uint ssap,
    boolean longxid)
{
    tring_hdr *trframe;
    paktype *pak;
    llc1_xid_t *xid;
    int riflen;
    uchar *addrp;
    boolean xidextended;
    sdlc_data_block *sdb = NULL;
    int xidsize;
    
    if (withinsdllc) {
	if (interface->vidb_link->sdlc_data == NULL ||
            (sdb = interface->vidb_link->sdlc_data[addrbyte]) == NULL)
	    return(NULL);
	xidextended = TRUE;
    } else 
	xidextended = FALSE;
    

    xidsize = sizeof(llc1_xid_t) + (xidextended ? (longxid ? 3: -3) : 0);

    if ((pak = getbuffer(xidsize)) == NULL) {
	return(NULL);
    }

    riflen = rif ? (rif[0] & RC_LTHMASK) : 0;

    /*
     * write the MAC header.
     */
    trframe = (tring_hdr *)
	(pak->network_start - TRING_ENCAPBYTES - riflen);
    trframe->ac = AC_PRI0;
    trframe->fc = FC_TYPE_LLC;
    ieee_copy_sans_rii(daddr, trframe->daddr);
    ieee_copy(saddr, trframe->saddr);
    if (withinsdllc)
	trframe->saddr[5] = addrbyte;

    /*
     * Write the RIF.
     */
    if (riflen) {
	bcopy(rif, trframe->data, riflen);
	trframe->saddr[0] |= TR_RII;
	
	/*
	 * for SDLLC limit max frame size to largest size given on interface.
	 */
	if (interface->sdllc_active)
	    trframe->data[1] = (trframe->data[1] & ~(RC_LFMASK)) |
		interface->sdllc_largest_token_frame;
    }

    /*
     * Now write the XID data.  pak->dsap and pak->ssap are set by the
     * caller.
     */
    xid = (llc1_xid_t *)pak->network_start;
    xid->control      = LLC1_XID | LLC1_P; 	/* assume cmd/poll */
    xid->dsap = (uchar)dsap;
    xid->ssap = (uchar)ssap;

    if ((xidextended) && (longxid)) {
          xid->data[0]  = XID_TYPE0_FMT2;   	/* Type 0 -- PU2 end host */
	  xid->data[1]  = 0x0;   	/* Reserved for fixed format */
	  addrp = &(xid->data[2]);	/* Node ID -- will be final four bytes
			      	 	   of output interface mac addr */
          bcopy(&(sdb->sdlc_xid), addrp, SDLC_XID_SIZE);
       } else {
          xid->data[0]  = XID_FMT_STD;
       xid->data[1]  = XID_CLASS_TYPE1 | XID_CLASS_TYPE2;
       xid->data[2]  = XID_WIN_TYPE2;
    }

    pak->if_output     = interface->firstsw;
    pak->enctype       = interface->enctype;
    pak->datagramstart = (uchar *) trframe;
    if (!longxid)
       xidsize = 3;
    pak->datagramsize  = xidsize + TRING_ENCAPBYTES + riflen; 
    /*
     * Because idb->oqueue is overloaded for cls->rsrb and 
     * rsrb->cls, we need to specify direction here...
     */
    if (interface->status & IDB_VIRTUAL)
    {
	pak->linktype = LINK_CLS; /* from CLS to rsrb transport */
    }

    return(pak);
}

/*
 * tr_setup_lnx_xid
 * Setup an xid packet for qllc.  Mainly a matter of getting a buffer and
 * stuffing a lot of constants.
 * This variant of  tr_setup_xid is always called from within qllc.
 * 
 * daddr is where this packet will go.  Saddr is who to say it is
 * from.
 *
 */

paktype *tr_setup_lnx_xid (uchar *daddr, uchar *saddr, lnxtype *lnxp, 
			   boolean response,
			   hwidbtype *vidb, uchar *rif, boolean longxid)
{
    tring_hdr	*trframe;
    paktype 	*pak;
    llc1_xid_t 	*xid;
    int 	riflen;
    uchar 	*addrp;
    int 	xidsize;
    
    xidsize = sizeof(llc1_xid_t)  + 
	(lnxp->xid_valid ?   (longxid ? 3: -3) : 0);

    if ((pak = getbuffer(xidsize)) == NULL) {
	return(NULL);
    }

    riflen = rif ? (rif[0] & RC_LTHMASK) : 0;

    /* write the mac header */

    trframe = (tring_hdr *)
	(pak->network_start - TRING_ENCAPBYTES - riflen);
    trframe->ac       = AC_PRI0;
    trframe->fc       = FC_TYPE_LLC;
    ieee_copy_sans_rii(daddr, trframe->daddr);
    ieee_copy(saddr, trframe->saddr);

    /* write the RIF */
    if (riflen) {
	bcopy(rif, trframe->data, riflen);
	trframe->saddr[0] |= TR_RII;
    }

    /* pak->dsap and pak->ssap are set by the caller. */
    xid = (llc1_xid_t *)pak->network_start;

    /*
     * Now write the XID data.
     */
    xid->control      = LLC1_XID | LLC1_P; 	/* assume cmd/poll */
    xid->dsap = lnxp->dsap;
    if (response)
	xid->ssap = lnxp->ssap | SAP_RESPONSE;
    else
	xid->ssap = lnxp->ssap;

    if (lnxp->xid_valid) {
       if (longxid) {
          xid->data[0]  = 0x02;  /* Type 0 Pu Type 2 -- PU2 end host */
	  xid->data[1]  = 0x0;   /* Reserved for fixed format */
	  addrp = &(xid->data[2]);  /* Node ID -- will be final four bytes
					 of output interface mac addr */
          bcopy(&lnxp->xid, addrp, LNX_XID_SIZE);
       }
       else {
          xid->data[0]  = XID_FMT_STD;
          xid->data[1]  = XID_CLASS_TYPE1;
          xid->data[2]  = XID_WIN_TYPE1;
       }
    } else {
       xid->data[0]  = XID_FMT_STD;
       xid->data[1]  = XID_CLASS_TYPE1;
       xid->data[2]  = XID_WIN_TYPE1;
    }

    pak->if_output     = vidb->firstsw;
    pak->enctype       = vidb->enctype;
    pak->datagramstart = (uchar *) trframe;
    if (!longxid)
       xidsize = 3;
    pak->datagramsize  = xidsize + TRING_ENCAPBYTES + riflen; 
    return(pak);
}



/*
 * tr_setup_loop
 * Setup a loopback packet for DECnet route exploration.
 *
 * Daddr is where this packet will go.  Saddr is who to say it is
 * from.
 */

static paktype *tr_setup_loop (uchar *daddr, uchar *saddr,
			       hwidbtype *interface)
{
    int i, riflen;
    tring_hdr *trframe;
    loop_t *loop;
    paktype *pak;
    
    if ((pak = getbuffer(sizeof(llc1_xid_t))) == NULL) {
	return(NULL);
    }
    riflen = 2;
    trframe = (tring_hdr *)(pak->network_start - TRING_ENCAPBYTES - riflen);
    trframe->ac = AC_PRI0;
    trframe->fc = FC_TYPE_LLC;
    ieee_copy_sans_rii(daddr, trframe->daddr);
    ieee_copy(saddr, trframe->saddr);
    trframe->data[0] = RC_DEF_SPAN;
    trframe->data[1] = RC_LFINITIAL;
    trframe->saddr[0] |= TR_RII;

    loop = (loop_t *)pak->network_start;
    loop->dsap = loop->ssap = SAP_SNAP;
    loop->control = LLC1_UI;
    memset(loop->oui, 0, sizeof(loop->oui));
    *(ushort *)&loop->oui[3] = TYPE_LOOP;
    loop->skip = h2lit(0);
    loop->func1 = h2lit(LOOP_FORWARD);
    for (i = 0; i < IEEEBYTES; i++)
	loop->mymac[i] = BITSWAP(saddr[i]);
    loop->func2 = h2lit(LOOP_REPLY);

    pak->if_output     = interface->firstsw;
    pak->enctype       = interface->enctype;
    pak->datagramstart = (uchar *) trframe;
    pak->datagramsize  = sizeof(loop_t) + TRING_ENCAPBYTES + riflen;
    return(pak);
}

/*
 * rif_truncate
 *
 * Truncate rifs to include everything "before" (but nothing after) the
 * ring number passed in. Do not truncate existing one, but truncate the
 * one after this one
 */
static void rif_truncate (
    srbroute_t *oldroute,
    ulong lastring,
    srbroute_t *newroute)
{
    ushort *ornp, *nrnp, newlength = 2;
    short oldlength, oldrn = 0;

    /*
     * Copy the dlf field, but always make the direction come from us
     */
    if (rif_always_forward) {
	newroute->rc_dlf = (oldroute->rc_dlf & ~(RC_D));
    } else {
	newroute->rc_dlf = (oldroute->rc_dlf | (RC_D));
    }
    oldlength = ((oldroute->rc_blth & RC_LTHMASK) - 2) / 2;

    /* Essentially, do the same thing, but handle both directions of source */
    nrnp = &(newroute->rd[0]);
    if (rif_always_forward) {
	if (!(oldroute->rc_dlf & RC_D)) {
	    /* Reverse to get to source... */
	    /* First find the oldrn ring... */
	    for (ornp = &(oldroute->rd[oldlength - 1]);
		 (ornp > &(oldroute->rd[0])); ornp--) {
		oldrn = ((GETSHORT(ornp) & RD_RNMASK) >> RD_RNSHIFT);
		if (oldrn == lastring)
		  break;
	    }
	    if (ornp > &(oldroute->rd[0])) {
		while (ornp >= &(oldroute->rd[0])) {
		    *nrnp = (GETSHORT(ornp--) & RD_RNMASK);
		    *nrnp++ |= (GETSHORT(ornp) & RD_BNMASK);
		    newlength += 2;
		}
	    }
	} else {
	    /* Forward to get to source... */
	    /* First find the oldrn ring... */
	    for (ornp = &(oldroute->rd[0]);
		 (ornp < &(oldroute->rd[oldlength - 1])); ornp++) {
		oldrn = ((GETSHORT(ornp) & RD_RNMASK) >> RD_RNSHIFT);
		if (oldrn == lastring)
		  break;
	    }
	    if (ornp < &(oldroute->rd[oldlength - 1])) {
		while (ornp <= &(oldroute->rd[oldlength - 1])) {
		    *nrnp++ = GETSHORT(ornp++);
		    newlength += 2;
		}
	    }
	}
    } else {
	if (!(oldroute->rc_dlf & RC_D)) {
	    /* Forward to get to source... */
	    ornp = &(oldroute->rd[0]);
	    while ((oldlength-- > 0) && (oldrn != lastring)) {
		*nrnp++ = GETSHORT(ornp);
		oldrn = ((GETSHORT(ornp++) & RD_RNMASK) >> RD_RNSHIFT);
		newlength += 2;
	    }
	} else {
	    /* Reverse to get to source... */
	    ornp = &(oldroute->rd[oldlength - 1]);
	    while (((newlength / 2) < (oldlength + 1)) && (oldrn != lastring)) {
		*nrnp = GETSHORT(ornp);
		oldrn = ((GETSHORT(ornp--) & RD_RNMASK) >> RD_RNSHIFT);
		*nrnp++ = (*nrnp & RD_RNMASK) | (GETSHORT(ornp) & RD_BNMASK);
		newlength += 2;
	    }
	}
    }
    /* The last ring entry should not have a bridge number */
    if (newlength > 2) {
	nrnp--;
	*nrnp &= RD_RNMASK;
    }
    /* Put in the new length... */
    newroute->rc_blth = newlength;
}

/*
 * rif_update_flags
 * Update the RIF cache entry's flags. Call only by rif_update.
 *
 * stoopid code overloads rif_update() to change
 * flags as well.  current (09/29/93) operation is
 * to allow modification from anytype to RIF_NOAGE.
 * Once at RIF_NOAGE, flags are "locked".  This causes
 * a problem with LNM, so "lock" it as well...
 */
static inline void rif_update_flags (riftype *entry, int flags)
{
    if (entry->flags & (RIF_AGED | RIF_AGED_REM)) {
	entry->flags =flags;
    }
}

/*
 * rif_update_llc
 * Update the RIF cache entry for this address.  If not already
 * present, then add one.
 *
 * RIF_AGED and RIF_NOAGE routes are assumed to originate from outside the
 * system.  We guarantee that these routes are valid for the system to use by
 * converting broadcast routes into specific and flipping the direction bit.
 * This means we eavesdrop on routes from other machines.  We are a router and
 * a bridge. This eavesdropping will tend to reduce the number of explorers
 * we generate (a good thing) at the expense of more entries in the cache.
 * This certainly could be made a configuration parameter.
 *
 * Normally, RIFs for nodes on the same ring are forced to be nothing.  If
 * however the configuration cell idb->tr_always_rif is TRUE than we force
 * all entries to have a rif.  Same ring rifs than become 0270.
 *
 * INTERFACE entries are guaranteed to be unique.  This is accomplished by
 * pulling the entry when a reset to the interface happens and putting it
 * back when it comes back.  This handles the case of the changing mac
 * address quite nicely.
 * 
 * idb		interface that this entry was heard on if any
 * vrn		virtual (ring-group) ring associated with this entry if any
 * addr		a pointer to the 48 bit TR mac address for this entry
 * route	a pointer to a RIF for this entry.  Maybe NULL
 * flags	what kind of entry
 *
 *
 * entry type		idb	vrn	addr	route
 *
 * INTERFACE		 +	 x	 +	maybe
 * HANDCRAFT		maybe	 x	 +	maybe
 * HANDCRAFT_REM	 x	 +	 +	  +
 * AGED			 +	 x	 +	maybe
 * AGED_REM		 x	 +	 +	  +
 * NOAGE                 +       x       +        +
 *
 * + = must be there, x = never there (don't care)
 */

void rif_update_llc (
    hwidbtype *idb,
    ushort vrn,
    uchar *addr,
    uchar *sa,
    srbroute_t *route,
    sap_hdr *llc,
    int flags)
{
    idbtype *swidb = firstsw_or_null(idb);
    riftype *entry;
    int length, hash_entry;
    char rif_string[SRB_MAXRIF*2+SRB_MAXRIF/2];
    leveltype status;
    srbroute_t *old_route;
    uchar new_route[SRB_MAXRIF], source[IEEEBYTES];

    if (sa == NULL) {
	ieee_copy(rif_null_addr, source);
	sa = rif_null_addr;
    } else {
	ieee_copy_sans_rii(sa, source);
    }
 
    length = route ? route->rc_blth & RC_LTHMASK : 0;

    /*
     * look for bogus rifs.  this is a bandaid in case we happen
     * to get something that looks like a rif, but is not.
     */
    if (length > SRB_MAXRIF) {
	if (RIF_DEBUG(idb, addr, source))
            buginf("\nRIF: U rej da=%e,sa=%e too big [%s]", addr, source,
                   get_rif_str((uchar *) route, rif_string));
        return;
    }

    if ((flags & (RIF_HANDCRAFT_REM | RIF_AGED_REM | RIF_NOAGE | RIF_LNM)) &&
	(length < 6)) {
	if (RIF_DEBUG(idb, addr, source))
	    buginf("\nRIF: U remote rif too small [%s] for da=%e,sa=%e",
		   (route ? get_rif_str((uchar *) route, rif_string) : ""),
		   addr, source);
	return;
    }

    if (length < 6 && !(flags & (RIF_HANDCRAFT | RIF_HANDCRAFT_REM)))
        length = 0;

    /*
     * Make a private copy of the rif and munge it so that it is in the
     * form that it will be stored.  This consolidates all the rif
     * munging logic, and allows for easy comparisons of rifs.
     *
     * AGED and AGED_REM routes get shorted to include only the path from
     * this router on, and only if the sa == rif_null_addr (for AGED, it
     * is to this IDB's srb_thisring, if a AGED_REM, it is to this vrn
     * passed into us.
     */
    if (route && (length > 2)) {
	/* If the interface is real, then look at srb_proxy_explorer
	   directly. */
	if (((!idb && global_proxy_explorer_enable) ||
	     (idb && srbV.srb_sb_get_proxy_explorer(swidb)))
	    && (sa == rif_null_addr)) {
	    if (flags & (RIF_AGED_REM)) {
		if (vrn) {
		    old_route = route;
		    route = (srbroute_t *) new_route;
		    rif_truncate(old_route, vrn, route);
		}
	    } else if (flags & (RIF_AGED)) {
                ushort srb_thisring = srbV.srb_sb_get_thisring(swidb);
		if ((idb) && (srb_thisring)) {
		    old_route = route;
		    route = (srbroute_t *) new_route;
		    rif_truncate(old_route, srb_thisring, route);
		}
	    }
	    /* new rif length */
	    length = route->rc_blth;
	} else {
	    bcopy(route, new_route, length);
	    route = (srbroute_t *)new_route;
	    route->rc_blth &= ~RC_RTMASK;
	    if (flags & (RIF_AGED | RIF_AGED_REM | RIF_NOAGE)) 
		route->rc_dlf ^= RC_D;
	}
	if (flags & (RIF_AGED | RIF_AGED_REM | RIF_NOAGE)) 
	    route->rc_blth &= RC_LTHMASK;
    }

    if (RIF_DEBUG(idb, addr, source))
      buginf("\nRIF: U chk da=%e,sa=%e [%s] type %d on %s/%d", addr, source,
	     get_rif_str((uchar *) route, rif_string), flags,
	     (idb ?  idb->hw_namestring : "static/remote"), vrn);

    if ((flags & (RIF_HANDCRAFT_REM | RIF_AGED_REM | RIF_NOAGE | RIF_LNM)) &&
        (length < 6)) {
	if (RIF_DEBUG(idb, addr, source))
            buginf("\nRIF: U remote rif too small [%s] for da=%e,sa=%e",
                   (route ? get_rif_str((uchar *) route, rif_string) : ""),
                   addr, source);
        return;
    }
    if (length < 6 && !(flags & (RIF_HANDCRAFT | RIF_HANDCRAFT_REM)))
	length = 0;
    if (length > SRB_MAXRIF) {
	if (RIF_DEBUG(idb, addr, source))
	    buginf("\nRIF: U rej da=%e,sa=%e too big [%s]", addr, source,
		   get_rif_str((uchar *) route, rif_string));
	return;
    }

    /*
     * INTERFACE entries guarenteed of not being there if changing Mac addrs
     */
    hash_entry = rifhash(addr);
    /* paranoia code */
    if (riftable[hash_entry] && !validmem(riftable[hash_entry])) {
	errmsg(&msgsym(RIFTBLTRASH, TR), "head", riftable[hash_entry]);
	riftable[hash_entry] = NULL;
    }
    if (riftable[hash_entry] && riftable[hash_entry]->magic != RIF_MAGIC) {
	errmsg(&msgsym(RIFBADMAGIC, TR), riftable[hash_entry]->magic,
	       "head", riftable[hash_entry]);
	riftable[hash_entry] = NULL;
    }

    for (entry = riftable[hash_entry]; entry; entry = entry->next) {
	/* more paranoia code */
	if (entry->next && !validmem(entry->next)) {
	    errmsg(&msgsym(RIFTBLTRASH, TR), "intermediate", entry->next);
	    entry->next = NULL;
	}
	if (entry->next && entry->next->magic != RIF_MAGIC) {
	    errmsg(&msgsym(RIFBADMAGIC, TR), entry->next->magic,
		   "intermediate", entry->next);
	    entry->next = NULL;
	}
	if ( ieee_equal_sans_rii(addr, entry->hardware) &&
	     ieee_equal_sans_rii(source, entry->src_addr) &&
	     idb == entry->idb) { 
	    break;
	}
    }

    if (entry) {
	switch (flags) {
	  case RIF_INTERFACE:
            /* CSCdi13146 */
            if (entry->flags & (RIF_HANDCRAFT | RIF_HANDCRAFT_REM)) {
		if (RIF_DEBUG(idb, addr, source))
                    buginf("\nRIF: U ign da=%e,sa=%e static entry",
                        entry->hardware, entry->src_addr);
                return;
            }    
	    entry->flags = flags;
	    entry->length = length;
	    entry->idb = idb;
	    if (length > 0)
		bcopy((uchar *)route, entry->rif, length);
	    rif_validate_clear_timers(entry);
	    TIMER_STOP(entry->add_name_query_time);
	    entry->add_name_query_correl = 0;
	    entry->netb_saved = 0;
	    bcopy("                ", entry->nq_name, 16);
	    if (RIF_DEBUG(idb, addr, source))
		buginf("\nRIF: U upd interface da=%e,sa=%e", entry->hardware,
		       entry->src_addr);
	    return;	    

	  case RIF_HANDCRAFT:
	  case RIF_HANDCRAFT_REM:
	    if (entry->flags & RIF_INTERFACE) {
		if (RIF_DEBUG(idb, addr, source))
		    buginf("\nRIF: U ign da=%e,sa=%e interface update",
			   entry->hardware, entry->src_addr);
		return;
	    }
	    entry->flags = flags;
	    entry->length = length;
	    entry->idb = idb;
	    entry->vrn = vrn;
	    if (length > 0)
		bcopy((uchar *)route, entry->rif, length);
	    GET_TIMESTAMP(entry->update_time);
	    TIMER_STOP(entry->add_name_query_time);
	    entry->add_name_query_correl = 0;
	    entry->netb_saved = 0;
	    bcopy("                ", entry->nq_name, 16);
	    if (RIF_DEBUG(idb, addr, source))
		buginf("\nRIF: U upd da=%e,sa=%e [%s]", entry->hardware,
		       entry->src_addr, get_rif_str((uchar *) route,
						    rif_string));
	    return;
	
	  case RIF_NOAGE:
	    if (length) {
		/* called with route , update rif cache */
		bzero(entry->rif, SRB_MAXRIF);
		entry->length = length;
		bcopy(new_route, entry->rif, length);
		rif_validate_clear_timers(entry);
		rif_update_flags(entry, flags);
		if(rif_debug)
		    buginf("\nRIF: U upd(no-age)new_rif da=%e,sa=%e [%s]", entry->hardware,
			   entry->src_addr, get_rif_str((uchar *) route,
							rif_string));
		return;
	    }
	    /* someone called with rif_noage length zero, so no route provided
	     *  update timers and flags . 
	     */
	    rif_validate_clear_timers(entry);
	    rif_update_flags(entry, flags);
	    if(rif_debug)
		buginf("\nRIF: U upd(no-age)existing_rif da=%e,sa=%e [%s]", entry->hardware,
		       entry->src_addr, get_rif_str((uchar *) route,
						    rif_string));
	    return;

	  case RIF_AGED:
	  case RIF_AGED_REM:
	  case RIF_LNM:
	  
	    /*
	     * Quick hack attempt to solve problem at Morgan Stanley.
	     * Don't update RIF timer if RIF coming in is different
	     * than the one in storage... to get quick turnaround
	     * if a machine's path has changed, set the rif timeout
	     * to a low value. (The hope here is that two-way
	     * communication will happen often enough to not have
	     * rifs constantly changing based on "d" bit usage.)
	     */
	    if ((length == entry->length) &&
		((length == 0) ||
		 (!bcmp((uchar *) route, (char *) entry->rif, length)))) {
	        rif_validate_clear_timers(entry);
		rif_update_flags(entry, flags);
		if (RIF_DEBUG(idb, addr, source))
		    buginf("\nRIF: U upd da=%e,sa=%e [%s]", entry->hardware,
			   entry->src_addr, get_rif_str((uchar *) route,
							rif_string));
		return;
	      }
            
             /*
              * If the direction bit is different, flip the RIF before 
              * compare.  i.e.  0630.0015.0020 should be treated the
              * same as 06B0.0025.0010
              */
	    if ( length == entry->length && ( (length > 2)  &&
                     ((route->rc_dlf & RC_D) ^ (entry->rif[1] & RC_D)))) {
                flip_rif( route);
                if ( (!bcmp((uchar *) route, (char *) entry->rif, length))) {
	            rif_validate_clear_timers(entry);
		    rif_update_flags(entry, flags);
		    if (RIF_DEBUG(idb, addr, source))
                        buginf("\nRIF: U upd(flip) da=%e,sa=%e [%s]",
			       entry->hardware, entry->src_addr,
			       get_rif_str((uchar *) route, rif_string));
		    return;
                }
		flip_rif( route);
	    }
	    rif_update_flags(entry, flags);
	    if (RIF_DEBUG(idb, addr, source))
	      buginf("\nRIF: U upd(flags only)da=%e,sa=%e [%s]",
		     entry->hardware,
		     entry->src_addr, get_rif_str((uchar *) route,
						  rif_string));
	    rif_validate_update(entry, route, llc);
	    return;

	  default:
	    break;

	}
    }

    /*
     * Nope, add one.
     */
    if (free_rifQ.count < FREE_RIF_LW) {
	createrifbuffer = TRUE;
	critical_background_wake();
    }
    entry = p_dequeue(&free_rifQ);
    if (entry) {

	riftype *ptr_tail;

        memset(entry, 0, sizeof(riftype));
	entry->magic       = RIF_MAGIC;
	entry->flags       = flags;
	ieee_copy_sans_rii(addr, entry->hardware);
	ieee_copy(source, entry->src_addr);
	entry->length      = length;
	entry->idb	   = idb;
	entry->vrn	   = vrn;
	if (length > 0)
	    bcopy((uchar *)route, entry->rif, length);
	GET_TIMESTAMP(entry->update_time);
	GET_TIMESTAMP(entry->create_time);
	TIMER_STOP(entry->validate_time);
	entry->next = NULL;
	status = raise_interrupt_level(NETS_DISABLE);
	/* CSCdi28103 */
        ptr_tail = riftable[hash_entry];
        if (!ptr_tail) {
            riftable[hash_entry] = entry;
        } else {
            while (ptr_tail->next) {
                ptr_tail = ptr_tail->next;
            }
            ptr_tail->next = entry;
        }
	reset_interrupt_level(status);
	if (RIF_DEBUG(idb, addr, source))
	  buginf("\nRIF: U add %e [%s] type %d",
		 entry->hardware, get_rif_str(entry->rif, rif_string),
		 flags);
	return;
    }

    if (rif_debug)
      buginf("\nRIF: U no memory to add rif for %e", addr);
    return;
}

/*
 * tr_rif_command
 * Manually insert an entry into the RIF table.  Command is of the form:
 * 
 *     "rif 1234.5678.9abc c280.0011.0021.... [<interface> | ring-group <n>]"
 */
static const char *badrifcmd = "\nBad RIF command - %s%s";

void tr_rif_command (parseinfo *csb)
{
    uchar tringaddress[IEEEBYTES];
    uchar rif[SRB_MAXRIF], riflen;

    if (csb->nvgen) {
 
	register riftype *rifp;
        register int i,j;
        boolean first;

	switch (csb->which) {
	  case RIF_ALWAYS_FORWARD:
	    nv_write(rif_always_forward, csb->nv_command);
	    break;
	  case RIF_ADDRESS:
	    for (i = 0; i < NETHASHLEN; i++)
		for (rifp = riftable[i]; rifp; rifp = rifp->next) {
		    if ((rifp->flags &
			 (RIF_HANDCRAFT | RIF_HANDCRAFT_REM)) == 0){
			continue;
		    }

		    first = TRUE;
		    nv_write(TRUE, "%s %e", csb->nv_command, rifp->hardware);
		    for (j = 0; j < rifp->length; j += 2) {
			nv_add(TRUE, "%c%-02x%-02x", first ? ' ' : '.',
			       rifp->rif[j], rifp->rif[j+1]);
			first = FALSE;
		    }
		    if (rifp->flags & RIF_HANDCRAFT_REM){
			nv_add(TRUE, " ring-group %d", rifp->vrn);
		    }else{
			if (rifp->idb){
			    nv_add(TRUE, " %s", rifp->idb->hw_namestring);
			}
		    }
		}
	    break;
	  case RIF_TIMEOUT:
	    nv_write(rif_timeout != RIF_MAXAGE, "%s %d", csb->nv_command,
		     rif_timeout/ONEMIN);
	    break;
	  case RIF_VALIDATE:
	    nv_write(rif_validation_age != RIF_VALIDATE_MAXAGE, "%s %d", 
		     csb->nv_command, rif_validation_age/ONESEC);
	    break;
      case RIF_VALIDATE_ENABLE:
	    nv_write(!rif_validate_enable , "no %s", csb->nv_command);
	    break;
      case RIF_VALIDATE_ENABLE_AGE:
	    nv_write(!rif_validate_enable_age , "no %s", csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case RIF_ALWAYS_FORWARD:
	rif_always_forward = csb->sense;
	break;
      case RIF_ADDRESS:
	bcopy((char *)GETOBJ(hwaddr,1)->addr, tringaddress,
	      GETOBJ(hwaddr,1)->length);
	if (csb->sense) {
	    /*
             * rif <tr address> [<hex rif field> | -] [<interface> | ring-group <n>]
             */
            memset(rif, 0, SRB_MAXRIF);
	    riflen = 0;
            for (riflen = 0; riflen < GETOBJ(int,1); riflen ++) {
                rif[(riflen*2)+0] = (csb->uiarray[riflen] >> 8) & 0xff;
                rif[(riflen*2)+1] = (csb->uiarray[riflen] >> 0) & 0xff;
#ifdef  THE_DEBUGGING_OUTPUT_FROM_HELL
                printf("\ncsb->uiarray[%d] = 0x%04x rif[%d] = 0x%02x rif[%d] = 0x%02x",
                       riflen, csb->uiarray[riflen],
                       riflen*2  , rif[riflen*2],
                       riflen*2+1, rif[riflen*2+1]);
#endif  THE_DEBUGGING_OUTPUT_FROM_HELL
            }
            riflen = riflen * 2;

            /*
             * Sanity check
             */
            if (riflen && ((rif[0] & RC_LTHMASK) != riflen)) {
                printf(badrifcmd, "length mismatch in RIF or too big", "");
                return;
            }

            rif_update(hwidb_or_null(GETOBJ(idb,1)), (ushort) GETOBJ(int,3), 
		       tringaddress, NULL,
                       (srbroute_t *)(riflen ? rif : NULL),
                       (GETOBJ(int,3) ? RIF_HANDCRAFT_REM : RIF_HANDCRAFT));
	}else{
            /*
             * no rif <tr address>
             */
	    if (!unset_rif(hwidb_or_null(GETOBJ(idb,1)), 
			   (ushort) GETOBJ(int,3),
			   tringaddress, NULL, FALSE)) {
                printf(badrifcmd, "cannot remove interface rif","");
            }
        }
        break;

      case RIF_TIMEOUT:
	if (csb->sense) {
	    rif_timeout = GETOBJ(int,1) * ONEMIN;
	} else {
	    rif_timeout = RIF_MAXAGE;
	}
	break;
      case RIF_VALIDATE:
	if (csb->sense) {
	    rif_validation_age = GETOBJ(int,1) * ONESEC;
	} else {
	    rif_validation_age = RIF_VALIDATE_MAXAGE;
	}
	break;
      case RIF_VALIDATE_ENABLE:
	rif_validate_enable = csb->set_to_default || csb->sense;
	if (!rif_validate_enable) {
	    rif_validate_enable_age = TRUE;  /* set default */
	    rif_validate_route_cache = FALSE; 
	}
	break;
      case RIF_VALIDATE_ENABLE_AGE:
	if (csb->sense && !csb->set_to_default && !rif_validate_enable) {
	    bad_parser_subcommand(csb, csb->which);
	}
	rif_validate_enable_age = csb->set_to_default || csb->sense;
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * Generic RIF configuration commands.
 *
 * Basic configuration such as ip address and the
 * like are handled by the basic ip subcommands.  The commands
 * implemented here are specific to SRB but do not fall into
 * one of the other specific token ring categories.  (like source
 * bridging - see srb.c).
 *
 * Summary:
 *
 *   Global:
 *
 * 	[no] rif <mac daddr> <rif specification> [<intf.> | ring-group <vrn>]
 *
 *
 *   Interface: (token ring and FDDI only)
 *
 *   	[no] multiring <protocol> [all-routes | spanning]
 *      (default off for all protocols)
 *      (default for explorer type is 'spanning')
 *
 * Global commands:
 *
 * 	[no] rif <mac daddr> <rif specification> [<intf.> | ring-group <vrn>]
 *
 * Inserts or removes an entry in the rif cache.  The rif cache is used when
 * encapsulating a packet that is about to be transmitted on a token ring
 * interface.  The rif specification is limited to a maximum of
 * RSRB_MAX_MAXRD routing descriptors.  The max in bytes is (maxrd * 2) + 2.
 * It will only be used on a packet if the outgoing interface's max is greater
 * than or equal to this entry's length.  Otherwise the packet will be
 * dropped.  The exec command "show rif" can be used to display the contents
 * of the rif cache.
 *
 * The rif cache is also used when a bridge performs proxy explorer functions.
 * For this function to work the entry must have either an interface or
 * a ring group.  These are optional parameters to the rif command.
 *
 * 	ie.  rif 0010.0140.06cc 0630.001a.0100
 * 	     rif 0010.0140.06cc 0630.001a.0100 tokenring 1
 * 	     rif 0010.0140.06cc 0630.001a.0100 ring-group 5
 *
 * 	denotes that packets to mac address 0010.0140.06cc should be
 * 	sent with a routing field set to 0630.001a.0100 which means
 * 	length 6, go from ring 1 using bridge 10 (0xa) to ring 16
 * 	0x10.
 *
 * Interface commands: (token ring and FDDI only)
 *
 *   	[no] multiring <protocol>		(default off for all protocols)
 *
 * This command either enables or disables this interface from participating
 * in multi-ring protocols for the protocol given.  If this is disabled
 * than routing information fields are not used on any packets leaving this
 * interface nor are any explorers generated (for the protocol specified).
 * If the command is given with no parameters, all ([no] == none) will be
 * "multi-ring"ed.
 *
 */
void srb_multiring_command (parseinfo *csb)
{
    hwidbtype *idb;
    idbtype   *swidb;
    boolean all_present, all_same;
    ulong bits;
    ulong srb_multiring;
    ulong srb_allroutes;

    swidb = csb->interface;
    idb = swidb->hwptr;
    if (!is_srb_hw(idb)) {
	if (!csb->nvgen) {
	    printf(OnlyOnSRB, "multiring");
	}
	return;
    }
  
    switch (GETOBJ(int,1)) {
      case MULTIRING_APOLLO: bits = MULTIRING_BIT_APOLLO; break;
      case MULTIRING_APPLE:  bits = MULTIRING_BIT_APPLE;  break;
      case MULTIRING_CLNS:   bits = MULTIRING_BIT_CLNS;   break;
      case MULTIRING_DECNET: bits = MULTIRING_BIT_DECNET; break;
      case MULTIRING_IP:     bits = MULTIRING_BIT_IP;     break;
      case MULTIRING_NOVELL: bits = MULTIRING_BIT_NOVELL; break;
      case MULTIRING_OTHER:  bits = MULTIRING_BIT_OTHER;  break;
      case MULTIRING_VINES:  bits = MULTIRING_BIT_VINES;  break;
      case MULTIRING_XNS:    bits = MULTIRING_BIT_XNS;    break;
      case MULTIRING_ALL:    bits = MULTIRING_BITS_ALL;   break;
      default:               bits = 0;                    break;
    }

    srb_multiring = srb_get_multiring_bits(swidb);
    srb_allroutes = srb_get_allroutes_bits(swidb);

    if (csb->nvgen) {
	all_present = (srb_multiring == MULTIRING_BITS_ALL);
	all_same = ((srb_allroutes == MULTIRING_BITS_ALL) ||
		    (srb_allroutes == MULTIRING_BIT_NONE));

	if (GETOBJ(int,1) == MULTIRING_ALL) {
	    if (all_present && all_same) {
		nv_write(TRUE, "%s %s", csb->nv_command,
			 (srb_allroutes == MULTIRING_BITS_ALL) ? "all-routes" : "");
	    } else {
		/*
		 * Do nothing here.  Need to list the individual
		 * possibilities with oor without the all-routes keyword,
		 * as appropriate.
		 */
	    }
	} else {
	    if (all_present && all_same) {
		/*
		 * Do nothing.  'Multiring all' has already been printed.
		 */
	    } else if (srb_multiring & bits) {
		nv_write(TRUE, "%s %s", csb->nv_command,
			 (srb_allroutes & bits) ? "all-routes" : "");
	    }
	}
	return;
    }

    if (csb->sense) {
	if (is_fddi(idb)) {
	    if ((bits != MULTIRING_BIT_IP) &&
		(bits != MULTIRING_BIT_NOVELL)) {
		printf("\n%%Multiring over FDDI supports IP and IPX only.");
		return;
	    }
	}
	    
	srb_multiring |= bits;
	if (GETOBJ(int,2) && (GETOBJ(int,2) == EXPLORER_ALLROUTES)) {
	    srb_allroutes |= bits;
	} else {
	    srb_allroutes &= ~bits;
	}
    } else {
	srb_multiring &= ~bits;
    }

    srb_set_multiring_bits(swidb, srb_multiring);
    srb_set_allroutes_bits(swidb, srb_allroutes);

    if (is_fddi(idb))
	(*(idb->reset))(idb);
}


/*
 * unset_rif(interface, vrn, address, sa, force)
 * Remove a particular RIF entry.  Used by "no rif ..."
 * If force is set than also allow removal of INTERFACE
 * rif entries.
 */

boolean unset_rif (
    hwidbtype *interface,
    ushort vrn,
    uchar *address,
    uchar *sa,
    boolean force)
{
    uint bucket;
    riftype *ptr;
    riftype *pred;
    leveltype status;

    if (sa == NULL)
	sa = rif_null_addr;

    /*
     * See if address is in the table
     */
    bucket = rifhash(address);		/* determine hash bucket */
    ptr = riftable[bucket];		/* set pointer to hash chain */
    /* paranoia code */
    if (ptr && !validmem(ptr)) {
	errmsg(&msgsym(RIFTBLTRASH, TR), "head", ptr);
	riftable[bucket] = NULL;
	return(FALSE);
    }
    if (ptr && ptr->magic != RIF_MAGIC) {
	errmsg(&msgsym(RIFBADMAGIC, TR), ptr->magic, "head", ptr);
	riftable[bucket] = NULL;
	return(FALSE);
    }
    status = raise_interrupt_level(NETS_DISABLE);
    pred = (riftype *) &riftable[bucket];
    while (ptr) {
	/* more paranoia code */
	if (ptr && !validmem(ptr)) {
	    errmsg(&msgsym(RIFTBLTRASH, TR), "intermediate", ptr);
	    pred->next = NULL;
	    reset_interrupt_level(status);
	    return(FALSE);
	}
	if (ptr && ptr->magic != RIF_MAGIC) {
	    errmsg(&msgsym(RIFBADMAGIC, TR), ptr->magic, "intermediate", ptr);
	    pred->next = NULL;
	    reset_interrupt_level(status);
	    return(FALSE);
	}
	if (ieee_equal_sans_rii(address, ptr->hardware) &&
	    ieee_equal_sans_rii(sa, ptr->src_addr)) {
	    if (interface && (interface != ptr->idb)) {
		pred = ptr;		/* remember predecessor */
		ptr = ptr->next;
		continue;
	    }
	    if (vrn && (vrn != ptr->vrn)) {
		pred = ptr;		/* remember predecessor */
		ptr = ptr->next;
		continue;
	    }
	    if (!force && (ptr->flags & RIF_INTERFACE)) {
		reset_interrupt_level(status);
		return(FALSE);
	    }
	    if (RIF_DEBUG(ptr->idb, address, sa)) {
		if (ptr->idb)
		    buginf("\nRIF: removing rif entry for %e on %s, type %d", 
			   address, ptr->idb->hw_namestring, ptr->flags);
		else
		    buginf("\nRIF: removing rif entry for %e, type %d",
			   address, ptr->flags);
	    }
	    pred->next = ptr->next;	/* link out current entry */
	    ptr->next = NULL;
            enqueue(&free_rifQ, ptr);   /* flush entry */
	    reset_interrupt_level(status);
	    return(TRUE);
	}
	pred = ptr;			/* remember predecessor */
	ptr = ptr->next;
    }
    reset_interrupt_level(status);
    return(TRUE);			/* couldn't find is okay */
}

/*
 * rif_flush_interface(idb)
 *
 * Remove all RIFs that go through a particular interface.  This is
 * called when an interface is reset.
 */

static void rif_flush_interface (hwidbtype *idb)
{
    int i;
    riftype *ptr, *pred;
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);
    for (i = 0; i < NETHASHLEN; i++) {
	pred = (riftype *) &riftable[i]; /* assumes next is a zero offset */
	ptr = pred->next;		 /* really "ptr = riftable[i]" */
	/* paranoia code */
	if (ptr && !validmem(ptr)) {
	    errmsg(&msgsym(RIFTBLTRASH, TR), "head", ptr);
	    riftable[i] = NULL;
	    continue;
	}
	if (ptr && ptr->magic != RIF_MAGIC) {
	    errmsg(&msgsym(RIFBADMAGIC, TR), ptr->magic, "head", ptr);
	    riftable[i] = NULL;
	    continue;
	}
	while (ptr) {
	    /* more paranoia code */
	    if (ptr && !validmem(ptr)) {
		errmsg(&msgsym(RIFTBLTRASH, TR), "intermediate", ptr);
		pred->next = NULL;
		break;
	    }
	    if (ptr && ptr->magic != RIF_MAGIC) {
		errmsg(&msgsym(RIFBADMAGIC, TR), ptr->magic, "intermediate",
		       ptr);
		pred->next = NULL;
		break;
	    }
	    switch (ptr->flags) {
	      case RIF_AGED:
	      case RIF_AGED_REM:
	      case RIF_INTERFACE:
		if (ptr->idb == idb) {
		    if (RIF_DEBUG(idb, NULL, NULL))
		      buginf("\nRIF: flushed %e on %s",
			     ptr->hardware, idb->hw_namestring);
		    pred->next = ptr->next;	/* link out current entry */
		    ptr->next = NULL;
		    enqueue(&free_rifQ, ptr);
		    ptr = pred;			/* point to predecessor */
		}
		break;
	      case RIF_HANDCRAFT:
	      default:
		break;
	    }
	    pred = ptr;			/* remember predecessor */
	    ptr = ptr->next;		/* point to next entry, if any */
	}
    }
    reset_interrupt_level(status);
}

/*
 * rif_ager
 * Timeout old entries or flush RIF cache entries.
 */

void rif_ager (boolean flushflag, boolean remotesonly)
{
    int i;
    riftype *ptr, *pred;
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);
    rif_validate_probe_sent = 0;
    for (i = 0; i < NETHASHLEN; i++) {
	pred = (riftype *) &riftable[i]; /* assumes next is a zero offset */
	ptr = pred->next;		 /* really "ptr = riftable[i]" */
	/* paranoia code */
	if (ptr && !validmem(ptr)) {
	    errmsg(&msgsym(RIFTBLTRASH, TR), "head", ptr);
	    riftable[i] = NULL;
	    continue;
	}
	if (ptr && ptr->magic != RIF_MAGIC) {
	    errmsg(&msgsym(RIFBADMAGIC, TR), ptr->magic, "head", ptr);
	    riftable[i] = NULL;
	    continue;
	}
	while (ptr) {
	    /* more paranoia code */
	    if (ptr && !validmem(ptr)) {
		errmsg(&msgsym(RIFTBLTRASH, TR), "intermediate", ptr);
		pred->next = NULL;
		break;
	    }
	    if (ptr && ptr->magic != RIF_MAGIC) {
		errmsg(&msgsym(RIFBADMAGIC, TR), ptr->magic, "intermediate",
		       ptr);
		pred->next = NULL;
		break;
	    }
	    switch (ptr->flags) {
	    case RIF_AGED:
	    case RIF_AGED_REM:
		if ((!remotesonly) || (ptr->flags == RIF_AGED_REM)) {
		    if (!flushflag && !rif_validate_age(ptr)) {
			break;
		    }
		    if (flushflag && ptr->idb) {
			rif_validate_invalidate_route_cache(ptr->idb,
							    ptr->hardware,
							    NULL);
		    }
		    if (RIF_DEBUG(ptr->idb, NULL, NULL))
			buginf("\nRIF: %s %e on %s",
			       (flushflag ? "flushed" : "expired"),
			       ptr->hardware,
			       (ptr->idb ? ptr->idb->hw_namestring : "static/remote"));
		    pred->next = ptr->next;	/* link out current entry */
		    ptr->next = NULL;
		    enqueue(&free_rifQ, ptr); /* back on free Q */
		    ptr = pred;			/* point to predecessor */
		}
		break;
	    case RIF_INTERFACE:
	    case RIF_HANDCRAFT:
	    case RIF_NOAGE:
	    case RIF_LNM:
	    default:
		break;
	    }
	    pred = ptr;			/* remember predecessor */
	    ptr = ptr->next;		/* point to next entry, if any */
	}
    }
    reset_interrupt_level(status);
}

/*
 * rif_age_cache
 * Cache aging called by SERVICE_ONEMIN
 */

static void rif_age_cache (void)
{
    rif_ager(FALSE,FALSE);
}

/*
 * tr_rif_clear_command
 * Support the EXEC "clear rif" command
 */

void tr_rif_clear_command (parseinfo *csb)
{
    rif_ager(TRUE, FALSE);
}

/*
 * display_tr_rif
 * Print out contents of RIF hash table
 */

static const char rifhead[] =
   "Dst HW Addr    Src HW Addr    How     Idle (min)  Routing Information Field";

void display_tr_rif (parseinfo *csb)
{
    int hash_entry, i;
    riftype *ptr;
    boolean printed;
    boolean first;

    automore_enable(NULL);
    if ((rif_timeout != RIF_MAXAGE) || (rif_always_forward)) {
	if (rif_timeout != RIF_MAXAGE)
	    printf("\nrif timeout %d minutes",
		   rif_timeout/ONEMIN);
	if (rif_always_forward)
	    printf("\nrifs will always be stored in a forward direction");
	printf("\n");
    }
    printf("\nCodes: * interface, - static, + remote\n");
    printed = FALSE;
    for (hash_entry = 0; hash_entry < NETHASHLEN; hash_entry++) {
	ptr = riftable[hash_entry];
	while (ptr) {
	    if (printed == FALSE) {
		automore_header(rifhead);
		printed = TRUE;
	    }
	    printf("\n%e ", ptr->hardware);
	    if (ieee_equal_sans_rii(ptr->src_addr, rif_null_addr)) {
		printf("N/A            ");
	    } else {
		printf("%e ", ptr->src_addr);
	    }
	    if (ptr->flags &
		(RIF_INTERFACE|RIF_AGED|RIF_NOAGE|RIF_LNM|RIF_HANDCRAFT)) {
		if (!ptr->idb) {
		    printf("-        ");
		} else {
		    hwidbtype *hwidb = ptr->idb;
		    if ((hwidb->status & IDB_VIRTUAL) &&
			RUNNING_IF(LINK_BRIDGE, hwidb->firstsw))
			printf("BG%2d     ", idb_sw_get_spanindex(hwidb->firstsw));
		    else
			printf("%9s", hwidb->hw_short_namestring);
		}
	    } else if (ptr->flags & RIF_HANDCRAFT)
		printf("-        ");
	    else if (ptr->flags & (RIF_HANDCRAFT_REM | RIF_AGED_REM))
		printf("rg%4d   ", ptr->vrn);
	    else
		printf("******   ");
	    if (ptr->flags & (RIF_AGED_REM | RIF_AGED))
		printf("%-8ld%c  ", ELAPSED_TIME(ptr->update_time)/ONEMIN,
		       ((ptr->flags & RIF_AGED_REM) ? '+' : ' '));
	    else
		printf("       %c   ",
		       ((ptr->flags & RIF_INTERFACE) ? '*' : '-'));
	    if (ptr->length == 0)
		printf("-");
	    else {
		for (first = TRUE, i = 0; i < ptr->length; i += 2) {
		    printf("%s%-02x%-02x", first ? "" : ".",
			   ptr->rif[i], ptr->rif[i+1]);
		    first = FALSE;
		    if (i > 0 && (i % 16 == 0)) {
			/* is the structure still O.K. */
			if (ptr->next && !validmem(ptr->next)) {
			    automore_disable();
			    return;
			}
			if (ptr->length-i-2 > 0)
			    printf("\n                              ");
			first = TRUE;
		    }
		}
	    }
	    if (ptr->next && !validmem(ptr->next)) {
		/* is the structure still O.K. */
		automore_disable();
		return;
	    }
	    ptr = ptr->next;
	}
    }
    automore_disable();
}

/*
 * rif_merge_rifs
 *
 * Merge two rifs. One from the incoming frame, and one
 * from the rifcache -- to build a new entry in the outgoing frame. The
 * outgoing frame will be an explorer through us to one remote side, suitable
 * for forwarding with vrforward_pak (no, this is not a typo.) The outgoing
 * rif will contain a complete RIF to get to the remote machine.
 *
 * As this code builds on EXPLORERS, it must always keep the new RIF
 * in a FORWARD direction...
 */
boolean rif_merge_rifs (paktype *pak, paktype *newpak, riftype *cacheptr)
{
    trrif_t *trh, *new_trh;
    int orl, nrl, diff;
    srbrd_t *cached, *working;
    boolean add_vrn = FALSE;
    hwidbtype *hwidb;
    srb_triplet_t srb_triplet;
    srb_triplet_t cached_srb_triplet;
    int count, i, j, ringno = 0;

    /* Was someone stupid on input? */
    if (!pak || !newpak || !cacheptr)
        return(FALSE);

    hwidb = hwidb_or_null(pak->if_input);

    /* Split horizon is awfully nice to have... */
    if (cacheptr->idb && hwidb == cacheptr->idb) {
	return(FALSE);
    }

    srbV.srb_sb_get_triplet(firstsw_or_null(hwidb), &srb_triplet);
    srbV.srb_sb_get_triplet(firstsw_or_null(cacheptr->idb), 
                              &cached_srb_triplet);

    trh = (trrif_t *) pak->datagramstart;
    
    /* Gotta have a rif... */
    if (trh->saddr[0] & TR_RII)
	orl = trh->rc_blth & RC_LTHMASK;
    else {
	return(FALSE);
    }

    /* Gotta be some sort of explorer... */
    if ((trh->rc_blth & RC_BROADBIT) == 0) {
	return(FALSE);
    }

    /* Various sanity checks on what we are doing... */
    if (hwidb->tr_vring_blk) {
	if (cacheptr->flags & (RIF_AGED_REM | RIF_HANDCRAFT_REM)) {
	    if (cacheptr->vrn != srb_triplet.remoteRing) {
		if (rif_debug || srb_debug || srbevent_debug)
		    buginf("\nSRB%d: br dropped modified explorer %e for %e, wrong vring (rem)",
			   hwidb->unit, trh->saddr,
			   trh->daddr);
		return(FALSE);
	    }
	} else {
	    if (!cacheptr->idb ||
		(cached_srb_triplet.remoteRing != srb_triplet.remoteRing)) {
		if (rif_debug || srb_debug || srbevent_debug)
		    buginf("\nSRB%d: br dropped modified explorer %e for %e, wrong vring (local)",
			   hwidb->unit, trh->saddr,
			   trh->daddr);
		return(FALSE);
	    }
	    add_vrn = TRUE;
	}
    } else {
	if (!cacheptr->idb ||
	    (cached_srb_triplet.remoteRing != srb_triplet.localRing)) {
	    if (rif_debug || srb_debug || srbevent_debug)
		buginf("\nSRB%d: br dropped modified explorer %e for %e, no path",
		       hwidb->unit, trh->saddr, trh->daddr);
	    return(FALSE);
	}
    }

    if (rif_loop_check(hwidb, trh, cacheptr)) 
	 return(FALSE);

    /*
     * Either a rif has >= 6 bytes, or it is insignificant.  Actually all
     * of this shit is insignificant but IBM says its cool so who are we
     * to argue.  Given a significant old rif, the new length is the old
     * rif, plus two more bytes (next ring) + 2 more if a virtual ring is
     * there. If the old rif is insignificant, then you will need a total
     * rif length of 6 bytes (or 8 if going across a virtual ring.
     */
    if ((orl >= 6) && (cacheptr->length >= 6))
      nrl = orl + cacheptr->length - 2;
    else
      nrl = max(orl, cacheptr->length) + 2;
    if (nrl < 6)
	nrl = 6;
    if (add_vrn)
	nrl += 2;
    if (nrl > SRB_MAXRIF) {
	return(FALSE);
    }
    diff = (nrl - orl);

    /*
     * swap source, destination saps, and make room for the new rif.
     */
    new_trh = (trrif_t *) newpak->datagramstart;
    bcopy((uchar *) trh, (uchar *) new_trh - diff, TRING_ENCAPBYTES + orl);
    (uchar *) new_trh -= diff;

    /*
     * hack (like, with a machete) in the new rif
     */
    new_trh->rc_blth = nrl; /* Make it a specific route rif... */
    new_trh->rc_dlf = ((cacheptr->length > 0) ?
		       (min((trh->rc_dlf & RC_LFMASK),
			    (cacheptr->rif[1] & RC_LFMASK))) :
		       (trh->rc_dlf & RC_LFMASK));

    /* Is the input ring in there already? */
    if (orl < 6) {
	working = (srbrd_t *) &new_trh->rd;
	working->ring_num = srb_triplet.localRing;
    } else
	working = (srbrd_t *) ((uchar *)&new_trh->rc_blth + orl - 2);

    /* Bridge */
    working->bridge_num = srb_triplet.bridgeNum;
    working++;

    /* Add v-ring for local->vring->local case. Guaranteed an idb pointer. */
    if (add_vrn) {
	working->ring_num = srb_triplet.remoteRing;
	working->bridge_num = cached_srb_triplet.bridgeNum;
	working++;
    }	
    
    /* remaining RIF */
    if (cacheptr->length < 6) { /* must have an idb */
	working->ring_num = cached_srb_triplet.localRing;
	working->bridge_num = 0;
    } else if (!(cacheptr->rif[1] & RC_D)) {  /* forward */
	for (cached = (srbrd_t *) &cacheptr->rif[2];
	     cached < (srbrd_t *) &cacheptr->rif[cacheptr->length];
	     working++, cached++) {
	    working->ring_num = cached->ring_num;
	    working->bridge_num = cached->bridge_num;
	}
    } else {
	for (cached = (srbrd_t *) &cacheptr->rif[cacheptr->length - 2];
	     cached > (srbrd_t *) &cacheptr->rif[2];
	     working++) {
	    working->ring_num = cached->ring_num;
	    cached--;
	    working->bridge_num = cached->bridge_num;
	}
	working->ring_num = cached->ring_num;
	working->bridge_num = 0;
    }

    newpak->datagramsize += diff;
    newpak->datagramstart = (uchar *) new_trh;
    newpak->flags |= PAK_SRB_EXPLORER;	/* trans with new src addr */

/* this is to catch the loop in case a local virtual ring was added */
    count = (( new_trh->rc_blth & RC_LTHMASK) - 2 ) / 2;
    for (i = 0; i < count-1; i++) {
       ringno = (new_trh->rd[i] & RD_RNMASK);
       for (j = i+1; j < count; j++) {
           if (ringno == (new_trh->rd[j] & RD_RNMASK)) {
               if (rif_debug)
                   buginf("\nRIF: rif_merge_rifs got a loop.");
               return (FALSE);
           }
       }
    }


    /* Oh -- I guess it was handled correctly */
    return(TRUE);
}

/*
 * Rif utility and classification functions...
 * (add more here as need arises...)
 */

static boolean isRifExplorerTrh (trrif_t *trh)
{
    return((trh->saddr[0] & TR_RII) &&
	   (trh->rc_blth & RC_BROADBIT));
}

boolean isRifExplorerPak (paktype *pak)
{
    return(isRifExplorerTrh((trrif_t *)pak->datagramstart));
}

/*
 * Return an interface's LF (largest frame) encoding value, based on
 * hardware...
 */
int getHwLf (hwidbtype *idb)
{
    return(
	   ((idb->tr_ring_speed == 16) && !(idb->status & IDB_CBUS)) ?
	   RC_TR16_LFDEF : RC_TR_LFDEF
	   );
}
 
boolean rif_loop_check (hwidbtype *idb, trrif_t *trh, riftype *cacheptr)
{
    int i, j, count, count2 = 0;
    ushort ringno, out_ringno;
    ushort srb_thisring;
    srbroute_t *rif_cache;

    if (rif_debug)
        buginf("\nRIF: Proxy rif_loop_check");
    if (!idb || !trh || !cacheptr)
	return (TRUE);

    if (!(trh->saddr[0] & TR_RII))
	return (TRUE);

    count = ((trh->rc_blth & RC_LTHMASK) - 2) / 2;
    count2 = ((cacheptr->rif[0] & RC_LTHMASK) - 2) / 2;
    rif_cache = (srbroute_t *)cacheptr->rif;

    ringno = srb_thisring = srbV.srb_sb_get_thisring(idb->firstsw);
    for (j = 0; j < count2; j++) {
        if (ringno == ((rif_cache->rd[j] & RD_RNMASK) >> RD_RNSHIFT)) {
	    if (rif_debug)
	        buginf("\nRIF: Proxy check got a loop.");
	    return (TRUE);
        }
    }

     for (i = 0; i < count; i++) {
         ringno = GETSHORT(&trh->rd[i]) & RD_RNMASK; /* ringno << RD_RNSHIFT */
         for (j = 0; j < count2; j++) {
             if (ringno == (rif_cache->rd[j] & RD_RNMASK)) {
                 if (rif_debug)
                     buginf("\nRIF: Proxy check got a loop.");
                 return (TRUE);
             }
         }
    }

    if (cacheptr->idb) {
        ushort cached_srb_thisring;
        cached_srb_thisring = srbV.srb_sb_get_thisring(cacheptr->idb->firstsw);
        if (srb_thisring == cached_srb_thisring) {
            if (rif_debug)
                buginf("\nRIF: Proxy check got a loop.");
            return (TRUE);
        }

        out_ringno = cached_srb_thisring << RD_RNSHIFT;
        for (i = 0; i < count; i++) {
             if (out_ringno == (GETSHORT(&trh->rd[i]) & RD_RNMASK)) {
                 if (rif_debug)
                     buginf("\nRIF: Proxy check got a loop.");
                 return (TRUE);
             }
       }
    }

    return (FALSE);
}


/* If the output interface is FDDI, change token ring encapsulation
 * to FDDI encapsulation.  If the output interface is token ring,
 * leave encapsulation as is.  Output SRB frame to interface.
 */


void tr_to_media_out (paktype *pak)
{
    if (!pak->if_output) {
	if (srberr_debug)
	    buginf("\n%%RSRB%d: srb output error: no if_output", 
				    pak->if_input->hwptr->unit);
	datagram_done(pak);
        return;
    }

    if (is_fddi(pak->if_output->hwptr))
        tr_to_fddi(pak, pak->if_output->hwptr);

    else if (pak->if_output->hwptr->status & IDB_CHANNEL) {
	pak->linktype = LINK_SRB;
	if (!(*pak->if_output->hwptr->vencap)(pak,-1)) {
	    datagram_done(pak);
	    return;
	}
    } 
    
    datagram_out(pak);
}

/*
 * fddi_to_tr()
 *
 *   Convert FDDI MAC header to TokenRing
 *
 */
void fddi_to_tr (paktype *pak, hwidbtype *idb)
{
    ushort *sp;

    if (idb->type == IDBTYPE_FDDIT) {
	ieee_swap(pak->datagramstart+FDDI_FC, 
		  pak->datagramstart+FDDI_FC);
	ieee_swap(pak->datagramstart+FDDI_FC+FDDI_DA, 
		  pak->datagramstart+FDDI_FC+FDDI_DA);
    }

    pak->datagramsize = pak->datagramsize - FDDI_ENCAPBYTES +
	TRING_ENCAPBYTES;
    pak->datagramstart = pak->datagramstart + FDDI_ENCAPBYTES - 
	TRING_ENCAPBYTES;
    sp = (ushort *)((uchar *) pak->datagramstart);
    PUTSHORT(sp, ((AC_PRI0 << 8) | FC_TYPE_LLC));
}

/*
 * tr_to_fddi()
 *
 *   Convert TokenRing MAC header to FDDI
 *
 */
void tr_to_fddi (paktype *pak, hwidbtype *idb)
{
    fddi_hdr *fddi;

    pak->datagramsize = pak->datagramsize - TRING_ENCAPBYTES + FDDI_ENCAPBYTES;

    pak->datagramstart = pak->datagramstart + TRING_ENCAPBYTES-FDDI_ENCAPBYTES;


    fddi = (fddi_hdr *)pak->datagramstart;

    if (idb->type == IDBTYPE_FDDIT) {
        ieee_swap(fddi->saddr, fddi->saddr);
        ieee_swap(fddi->daddr, fddi->daddr);
    }

    fddi->fc = FDDI_LLC_FC_BYTE;
}


/*
 * rif_init
 * Initialize the RIF support
 */

void rif_init (void)
{
    int i;
    hwidbtype *idb;

    for (i = 0; i < NETHASHLEN; i++)
      riftable[i] = NULL;
    rif_timeout = RIF_MAXAGE;
    rif_validation_age = RIF_VALIDATE_MAXAGE;
    global_proxy_explorer_enable = FALSE;
    tr_rif_xid_explorer =  FALSE;
    
    rif_timeout_update = ONEMIN +  (10*ONESEC);
    rif_timeout_lookup = ONEMIN +  (10*ONESEC);
    rif_timeout_explore = (30*ONESEC);
    rif_timeout_age_long = (180*ONEMIN);
    rif_validate_enable_age = TRUE;
    rif_validate_enable = TRUE;
    rif_validate_route_cache = FALSE;

    FOR_ALL_HWIDBS(idb) {
	if ((idb->status & IDB_TR) && (idb->state == IDBS_UP))
	    rif_update(idb, 0, idb->hardware, NULL, NULL, RIF_INTERFACE);
 	if ((idb->status & IDB_FDDI) && (idb->state == IDBS_UP))
 	    rif_update(idb, 0, idb->bit_swapped_hardware, NULL, NULL,
 		       RIF_INTERFACE);
    }
    createrifbuffer = FALSE;
    queue_init(&free_rifQ, 0);
    add_rif_buffers();

    allroutes_rif.length = 2;
    allroutes_rif.rif[0] = 0x82;
    allroutes_rif.rif[1] = 0x30;
    spanning_rif.length = 2;
    spanning_rif.rif[0] = 0xC2;
    spanning_rif.rif[1] = 0x30;
    /*
     * Make our packets look just like DEC's so that user's don't
     * complain about different packets.
     */
    dec_func_rif.length = 2;
    dec_func_rif.rif[0] = 0xE2;
    dec_func_rif.rif[1] = 0x10;

    /*
     * Register some functions
     */
    reg_add_onemin(rif_age_cache, "rif_age_cache");
    reg_add_critical_background(add_rif_buffers, "add_rif_buffers");
    reg_add_media_rif_reset(rif_flush_interface, "rif_flush_interface");
    reg_add_media_fddi_to_tr(fddi_to_tr, "fddi_to_tr");
}

