/* $Id: bridge_cbus.c,v 3.6.20.4 1996/08/28 12:43:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/bridge_cbus.c,v $
 *------------------------------------------------------------------
 * cBus bridging functions
 *
 * March 1993, Tony Speakman
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * cBus-bridging specific access functions for:
 * - IFAs
 * - shadow IFAs
 * - bridge cache
 * - bridge-cache entries
 * cBus-bridging specific functions for:
 * - flooding
 * - validating interfaces
 *
 *------------------------------------------------------------------
 * $Log: bridge_cbus.c,v $
 * Revision 3.6.20.4  1996/08/28  12:43:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.20.3  1996/05/21  09:47:32  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.20.2  1996/05/04  00:44:05  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR transparent bridging support.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Revision 3.6.20.1  1996/03/18  19:39:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.4  1996/03/14  00:56:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.5.2.3  1996/03/13  01:20:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.2  1996/02/20  00:50:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/01  06:02:13  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5.2.1  1996/01/24  21:44:34  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1996/01/18  22:46:37  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/17  18:27:14  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/15  03:37:55  hou
 * CSCdi45156:  cbus-bridging on HSSI interface broken
 * - cbus_bridging_idb()
 *   Filter out PPP by checking encapsulation type.
 *
 * Revision 3.2  1995/11/17  09:14:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  20:59:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/08/23  05:30:45  gchristy
 * CSCdi28311:  UDP spanning-tree flooding with DLD creates duplicate
 * packets
 *  - Nuke bridge_flood_pak. Create and initialize udp_flood_pak during FS
 *    init.
 *
 *  - Instead of iterating over all interfaces, use the spanning tree
 *    flood table associated with the input interface to get the list of
 *    interfaces participating in the bridge-group.
 *
 *  - If a potential output interface is part of a circuit-group, use a
 *    hash of the source ip address and destination udp port number to
 *    choose one of the members of the group.
 *
 * Revision 2.7  1995/08/12  07:45:23  fred
 * CSCdi38525:  do not use CBUS accellerator on priority/custom/fir queued
 * interfaces
 *
 * Revision 2.6  1995/07/26  04:02:31  speakman
 * CSCdi37051:  bridge address forward filters fail with AB
 * Check for an interface on the BTE before deriving the
 * permission masks for the BCE.
 *
 * Revision 2.5  1995/07/11  17:38:48  speakman
 * CSCdi36910:  AB loses cBus cache entries
 * Unsign a signed variable.
 *
 * Revision 2.4  1995/07/01  19:14:55  richl
 * CSCdi34435:  %SYS-3-SUPNONE: Registry 6 doesnt exist
 *              o Changed above msg so it isn't rate based
 *              o Protected call to registry with conditional statement
 *              o Resolved conflict in DRIVER/PROTOCOL class between
 *                the cbus_bridge and tbridge subsystems.
 *
 * Revision 2.3  1995/06/28  09:24:14  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/18  23:16:19  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:37:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface_private.h"
#include "config.h"
#include "../if/network.h"
#include "if_mci.h"
#include "if_fci.h"
#include "../srt/span.h"
#include "../hes/cbus_debug.h"
#include "../tbridge/tbridge_debug.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "../srt/srt_registry.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../srt/parser_defs_bridge.h"
#include "cbus_registry.h"
#include "logger.h"
#include "../smf/smf_debug.h"
#include "../tbridge/tbridge_public.h"
#include "msg_cbus.c"			/* Not a typo, see logger.h */

#ifndef C7000
#define MEMA_IFA_BASE	0x423
#else
#define MEMA_IFA_BASE	0x1E70
#endif

/*
 * Forward references
 */
static void cbus_bridge_cache_flush(boolean);
static void cbus_bridgegroup_command(parseinfo *csb);

/*
 * Local Storage
 */
static ushort  cbus_bce_words_allocated;/* how much cbus cache used so far */
static ushort  cbus_bce_words_lowater;  /* minimum we expect w/out
					   invalidating */
static ushort  cbus_bce_words_max;      /* maximum we'll try to allocate */

static boolean cbus_bridging_supported; /* cbus ucode supports autonomous
					   bridging */
static boolean cbus_flooding_supported; /* cbus ucode supports autonomous
					   flooding */
static boolean cbus_bce_blocked = FALSE;/* indicates bce creation is blocked */

/*
 * Set indicates SHIFAs accurately reflect their counterparts in MEMA.
 * Clear indicates MEMA has been ripped up by cBus/SP initialization
 * SHIFAs need to be released and recovered.
 */
static boolean shifa_current;

/*
 * This array is used to map if_number to mci_index for translating
 * RP permission masks to SP permission masks.  We need this array
 * since there is no way to find an idb given just if_number.
 * We really only use the last 31 of the first 32 entries since if_number
 * is 1-based and the SP supports permission bits for only 32 interfaces.
 */
static int mci_index_array[MAX_INTERFACES];

/*
 * Parse chains for CBUS Bridging commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_bridge_cbus.h"
LINK_POINT(cbus_bridging_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Crummy little function to map
 * RP permission masks based on if_number to
 * SP permission masks based on mci_index.
 */

static ulong cbus_convert_ifnummask (ulong ifnum_mask)
{
    ulong mci_index_mask = 0;
    int if_num;

    /* Yes, start from 1. No, don't include 32 */

    for (if_num = 1; if_num < 32; if_num++)
	if (ifnum_mask & (1 << if_num))
	    mci_index_mask |= (1 << mci_index_array[if_num]);
    return(mci_index_mask);
}

/******************************************************************************

 Autonomous Bridging: cBus IFA access functions

 *****************************************************************************/

/*
 * cbus_ifa_set_port_state
 *
 * Set the spanning-tree port state in an IFA.
 *
 * This function may be called with PORT_DISABLED to set the IFA state to
 * IFA_SPAN_ACTION_BURP when the corresponding IDB port state may well
 * be other than  PORT_DISABLED.
 */

static boolean cbus_ifa_set_port_state (hwidbtype *hwidb, int port_state)
{
    mcitype *cbus;
    mciregtype *regptr;
    int response;
    leveltype level;
    int state_arg;

    /*
     * Fix for CSCdi15553
     * This function is called from config commands without first checking
     * for the configuration of AB on an interface.  Make that check here.
     */
    if (!hwidb->cbus_bridge_config)
	return(FALSE);

    cbus = (mcitype *) cbusQ.qhead;
    regptr = cbus->regptr;

    switch (port_state) {
        case PORT_DISABLED:
            state_arg = IFA_SPAN_ACTION_BURP;
            break;
        case PORT_BLOCKING:
            state_arg = IFA_SPAN_ACTION_BLOCK;
            break;
        case PORT_LISTENING:
            state_arg = IFA_SPAN_ACTION_LISTEN;
            break;
        case PORT_LEARNING:
            state_arg = IFA_SPAN_ACTION_LEARN;
            break;
        case PORT_FORWARDING:
            state_arg = IFA_SPAN_ACTION_FORWARD;
            break;
        default:
            return(FALSE);
    }

    level = raise_interrupt_level(NETS_DISABLE);

    regptr->argreg = hwidb->mci_index;
    regptr->cmdreg = FCI_CMD_SELECT;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
        reset_interrupt_level(level);
        buginf("\ncbus_ifa_set_port_state failed to select IFA # %d : %x",
               hwidb->mci_index, response);
        return(FALSE);
    }
    regptr->argreg = state_arg;
    regptr->cmdreg = FCI_BRIDGE_GROUP_FLAGS;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
        reset_interrupt_level(level);
        buginf("\ncbus_ifa_set_port_state failed to write IFA # %d : %x",
               hwidb->mci_index, response);
        return(FALSE);
    }

    reset_interrupt_level(level);
    return(TRUE);
}

/*
 * cbus_ifa_insert
 *
 * Insert an IFA in a list of IFAs.
 *
 */

static boolean cbus_ifa_insert (hwidbtype *prev_hwidb, hwidbtype *link_hwidb,
				hwidbtype *next_hwidb)
{
    mcitype *cbus;
    mciregtype *regptr;
    int response;
    leveltype level;

    cbus = (mcitype *) cbusQ.qhead;
    regptr = cbus->regptr;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Set the IFA state to be the same as the sw IDB port state.
     */
    if (!cbus_ifa_set_port_state(link_hwidb,
                                 link_hwidb->firstsw->port_state)) {
	reset_interrupt_level(level);
	return(FALSE);
    }

    /*
     * Put the current IFA in the bridge group.
     */
    regptr->argreg = link_hwidb->firstsw->span_index;
    regptr->cmdreg = FCI_BRIDGE_GROUP_NUMBER;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
        reset_interrupt_level(level);
        buginf("\ncbus_ifa_insert failed to write current IFA %d : %x",
               link_hwidb->mci_index, response);
        return(FALSE);
    }

    /*
     * Point the current IFA at the next IFA.
     */
    regptr->argreg = link_hwidb->mci_index;
    regptr->cmdreg = FCI_CMD_SELECT;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
        reset_interrupt_level(level);
        buginf("\ncbus_ifa_insert failed to select current IFA %d : %x",
               link_hwidb->mci_index, response);
        return(FALSE);
    }

    regptr->argreg = next_hwidb->cbus_icb_address;
    regptr->cmdreg = FCI_BRIDGE_GROUP_NEXT;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
        reset_interrupt_level(level);
        buginf("\ncbus_ifa_insert failed to write current IFA %d : %x",
               link_hwidb->mci_index, response);
        return(FALSE);
    }

    /*
     * Finally, point the previous IFA at the current IFA.
     */
    regptr->argreg = prev_hwidb->mci_index;
    regptr->cmdreg = FCI_CMD_SELECT;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	reset_interrupt_level(level);
	buginf("\ncbus_ifa_insert failed to select previous IFA %d : %x",
	       prev_hwidb->mci_index, response);
	return(FALSE);
    }

    regptr->argreg = link_hwidb->cbus_icb_address;
    regptr->cmdreg = FCI_BRIDGE_GROUP_NEXT;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	reset_interrupt_level(level);
	buginf("\ncbus_ifa_insert failed to write previous IFA %d : %x",
	       prev_hwidb->mci_index, response);
	return(FALSE);
    }

    reset_interrupt_level(level);
    return(TRUE);
}

/*
 * cbus_ifa_remove
 *
 * Remove an IFA from a list of IFAs.
 *
 */

static boolean cbus_ifa_remove (hwidbtype *prev_hwidb, hwidbtype *link_hwidb,
				hwidbtype *next_hwidb)
{
    mcitype *cbus;
    mciregtype *regptr;
    int response;
    leveltype level;

    cbus = (mcitype *) cbusQ.qhead;
    regptr = cbus->regptr;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Set the IFA state to IFA_SPAN_ACTION_BURP before
     * removing it from the IFA list.
     */
    if (!cbus_ifa_set_port_state(link_hwidb, PORT_DISABLED)) {
	reset_interrupt_level(level);
	return(FALSE);
    }

    /*
     * Point the previous IFA at the next IFA.
     * Do this first so that the IFA list stays consistent!
     */
    regptr->argreg = prev_hwidb->mci_index;
    regptr->cmdreg = FCI_CMD_SELECT;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	reset_interrupt_level(level);
	buginf("\ncbus_ifa_remove failed to select previous IFA %d : %x",
	       prev_hwidb->mci_index, response);
	return(FALSE);
    }

    regptr->argreg = next_hwidb->cbus_icb_address;
    regptr->cmdreg = FCI_BRIDGE_GROUP_NEXT;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	reset_interrupt_level(level);
	buginf("\ncbus_ifa_remove failed to write previous IFA %d : %x",
	       prev_hwidb->mci_index, response);
	return(FALSE);
    }

    /*
     * Clear the current IFA's pointers.
     */
    regptr->argreg = link_hwidb->mci_index;
    regptr->cmdreg = FCI_CMD_SELECT;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
        reset_interrupt_level(level);
        buginf("\ncbus_ifa_remove failed to select current IFA %d : %x",
               link_hwidb->mci_index, response);
        return(FALSE);
    }
    regptr->argreg = 0;
    regptr->cmdreg = FCI_BRIDGE_GROUP_NEXT;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
        reset_interrupt_level(level);
        buginf("\ncbus_ifa_remove failed to write current IFA %d : %x",
               link_hwidb->mci_index, response);
        return(FALSE);
    }
    regptr->argreg = 0;
    regptr->cmdreg = FCI_BRIDGE_GROUP_NUMBER;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
        reset_interrupt_level(level);
        buginf("\ncbus_ifa_remove failed to write current IFA %d : %x",
               link_hwidb->mci_index, response);
        return(FALSE);
    }

    reset_interrupt_level(level);
    return(TRUE);
}

/*
 * cbus_list_links
 *
 * Brute force examination of the IFA links in MEMA. Not pretty.
 * Especially the '0x423 + kndx * 6' and 'kndx * 6 + 0x60' expressions.
 * These are offsets and sizes established by the cBus II u-code.
 *
 */

static void cbus_list_links (void)
{
    mcitype *cbus;
    hwidbtype *idb;
    int indx, jndx, kndx;
    leveltype level;
    dcitype *dci;
    mciregtype *regptr;
    ushort link[3];

    /*
     * Scan for all cBus controllers
     */
    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	/*
	 * Scan for all daughter controllers
	 */
	for (indx = 0; indx < fci_max_dci_ctrlrs; indx++) {
	    if (!(dci = cbus->dci[indx]))
		continue;
	    /*
	     * Scan for all interface(s) on this daughter controller
	     */
	    for (jndx = 0; jndx < MAX_CTRLR_INTERFACES; jndx++) {
		idb = dci->interface_list[jndx];
		if (!idb)
		    continue;
		for ( ; idb != NULL; idb = idb->next_idb_on_physint) {
		    regptr = idb->mci_regptr;
		    kndx = idb->mci_index;
		    
		    level = raise_interrupt_level(NETS_DISABLE);
		    regptr->argreg = MEMA_IFA_BASE + kndx * 6;
		    regptr->cmdreg = FCI_CMD_MEMA_SELECT;
		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    link[0]= regptr->argreg;
		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    link[1]= regptr->argreg;
		    regptr->cmdreg = FCI_CMD_MEMA_READ;
		    link[2]= regptr->argreg;
		    reset_interrupt_level(level);

		    if (link[0] || link[1]) {
		    printf("\n %12s - %x : group %x next %x",
			       idb->hw_namestring, kndx * 6 + 0x60,
			       link[0], link[1]);
			switch (link[2]) {
			case IFA_SPAN_ACTION_BLOCK:
			    printf(" blocking");
			    break;
			case IFA_SPAN_ACTION_LISTEN:
			    printf(" listening");
			    break;
			case IFA_SPAN_ACTION_LEARN:
			    printf(" learning");
			    break;
			case IFA_SPAN_ACTION_FORWARD:
			    printf(" forwarding");
			    break;
			case IFA_SPAN_ACTION_BURP:
			    printf(" burping");
			    break;
			default:
			printf(" unknown %x", link[2]);
			    break;
			}
		    }
		}
	    }
	}
    }
}

/******************************************************************************

 Autonomous Bridging: Shadow IFA access functions

 *****************************************************************************/

/*
 * shifa_create
 *
 * Allocate and initialize a shadow IFA structure for an interface.
 * This is split out into a function in anticipation of a more
 * sophisticated initialization process that will record the encap
 * type and interface speed in the shadow IFA structue for sorting.
 */

static shifaQtype *shifa_create (hwidbtype *hwidb)
{
    shifaQtype *shifa = NULL;

    shifa = malloc(sizeof(shifaQtype));
    if (shifa)
	shifa->hwidb = hwidb;
    return(shifa);
}

/*
 * shifa_insert
 *
 * Append an IFA to the SP list and append the shadow IFA structure
 * to the shadow list.  In future, we will sort these according to
 * encap type and interface speed.
 */

static boolean shifa_insert (queuetype *shifaQ, shifaQtype *shifa)
{
    hwidbtype *prev_hwidb, *next_hwidb;

    /*
     * Get the tail IFA's IDB from the tail shadow IFA or
     * use this IFA's IDB when no tail shadow IFA exists yet.
     */
    prev_hwidb = (shifaQ->qtail) ? \
	         ((shifaQtype *) shifaQ->qtail)->hwidb : shifa->hwidb;
    /*
     * Default scheme is to append to the tail, so next IFA is always the head.
     * Only the ifa list in the SP is looped in this way to facilitate the
     * flooding algrorithm.  shifaQ is NOT looped so we can continue to use
     * next == NULL as a terminating condition in traversals.
     *
     * Get the next IFA's IDB from the head shadow IFA or
     * use this IFA's IDB when no head shadow IFA exists yet.
     */
    next_hwidb = (shifaQ->qhead) ? \
	         ((shifaQtype *) shifaQ->qhead)->hwidb : shifa->hwidb;

    if (cbus_ifa_insert(prev_hwidb, shifa->hwidb, next_hwidb)) {
	enqueue(shifaQ, shifa);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * shifa_locate
 *
 * Find a shadow IFA structure in the shadow IFA list given a pointer
 * to the corresponding IDB.  Return its predecessor as well to expedite
 * singly-linked list manipulation.
 */

static shifaQtype *shifa_locate (queuetype *shifaQ, hwidbtype *hwidb,
				 shifaQtype **shifa_prev)
{
    shifaQtype *shifa, *prev_shifa;

    *shifa_prev = NULL;
    shifa = (shifaQtype *) shifaQ->qhead;
    /*
     * Although the tail does not point at the head (i.e., this is a
     * NULL-terminating list), the actual IFA list is a loop so we use
     * the tail as the initial value of the predecessor.
     */
    prev_shifa = (shifaQtype *) shifaQ->qtail;

    if (!shifa)
	return(NULL);

    while (shifa) {
        if (shifa->hwidb == hwidb) {
	    *shifa_prev = prev_shifa;
            return(shifa);
        } else {
            prev_shifa = shifa;
            shifa = prev_shifa->next;
        }
    }
    return(NULL);
}

/*
 * shifa_remove
 *
 * Remove an IFA from the SP list and remove the shadow IFA structure
 * from the shadow list.
 */

static boolean shifa_remove (queuetype *shifaQ, shifaQtype *shifa_prev,
			     shifaQtype *shifa)
{
    hwidbtype *next_hwidb;

    /*
     * Specify the head IFA when no next shadow IFA exits.
     */
    next_hwidb = (shifa->next) ? shifa->next->hwidb :
				 ((shifaQtype *) shifaQ->qhead)->hwidb;

    if (cbus_ifa_remove(shifa_prev->hwidb, shifa->hwidb, next_hwidb)) {
	unqueue(shifaQ, shifa);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * shifa_release
 *
 * Free any shadow IFAs in the wake of a cbus/SP initialization.
 * This function assumes the IFAs have been initialized out from
 * under the system code, and do not need to be dismantled with
 * shifa_remove.  Just whiz through the shifa queues freeing the
 * elements.
 */

static void shifa_release (void)
{
    int i;
    shifaQtype *shifa;

    for (i = 1; i < SPAN_MAXINDEX; i++)
	if (spanarray[i] && !QUEUEEMPTY(&(spanarray[i]->shifaQ)))
	    while ((shifa = dequeue(&(spanarray[i]->shifaQ)))) {
		if (tbridge_debug_cbus)
		    buginf("\nshifa_release freed shadow IFA for %s",
			   shifa->hwidb->hw_namestring);
		free(shifa);
	    }
}

/*
 * shifa_recover
 *
 * Restore any shadow IFAs in the wake of a cbus/SP initialization.
 * This function assumes the IFAs have been initialized out from
 * under the system code, and the shadow IFAs have been freed.
 * Just whiz through the hardware IDBs rebuilding the shadow IFAs
 * (which includes linking the IFAs as well).
 */

static void shifa_recover (void)
{
    hwidbtype *hwidb;
    spantype *span;
    shifaQtype *shifa;

    FOR_ALL_HWIDBS(hwidb) {
	if (hwidb->cbus_bridge_enable) {
	    span = spanarray[hwidb->firstsw->span_index];
	    /*
	     * create and initialize the shadow IFA structure for this port
	     * insert it in the shadow ifa list
	     */
	    shifa = shifa_create(hwidb);
	    if (shifa) {
		if (!shifa_insert(&span->shifaQ, shifa)) {
		    hwidb->cbus_bridge_enable = FALSE;
		} else {
		    if (tbridge_debug_cbus)
			buginf("\nshifa_recover recovered shadow IFA for %s",
			       (shifa->hwidb->hw_namestring));
		}
	    }
	}
    }
}

/******************************************************************************

 Autonomous Bridging: cBus bridge port functions

 *****************************************************************************/

/*
 * cbus_bridge_port_enable
 *
 * Enable autonomous bridging on an interface.
 * For the modular release, include a check for the configuration of AB
 * inside this function itself to hide this subsytem dependency from
 * the caller.  Also removed the hardware idb as an argument to the funcion
 * and derive it from the idb inside the function instead.
 */

static void cbus_bridge_port_enable (spantype *span, idbtype *idb)
{
    hwidbtype *hwidb;
    shifaQtype *shifa;

    hwidb = idb->hwptr;
    if (hwidb->cbus_bridge_config) {
	shifa = shifa_create(hwidb);
	if (shifa) {
	    if (shifa_insert(&span->shifaQ, shifa)) {
		hwidb->cbus_bridge_enable = TRUE;
		span_clearif (span, idb);
	    }
	}
    }
}

/*
 * cbus_bridge_port_disable
 *
 * Disable autonomous bridging on an interface.
 * For the modular release, include a check for the configuration of AB
 * inside this function itself to hide this subsytem dependency from
 * the caller.  Also removed the hardware idb as an argument to the funcion
 * and derive it from the idb inside the function instead.
 */

static void cbus_bridge_port_disable (spantype *span, idbtype *idb)
{
    hwidbtype *hwidb;
    shifaQtype *prev_shifa, *shifa;

    hwidb = idb->hwptr;
    if (hwidb->cbus_bridge_config) {

	/* The order of these operations is important: */

	/* Stop the creation of new BCEs */
	hwidb->cbus_bridge_enable = FALSE;

	/* Tell the SP to burp this interface */
	(void)cbus_ifa_set_port_state(hwidb, PORT_DISABLED);

	/* Now remove this interface's BCEs (and BTEs) */
	span_clearif (span, idb);

	/* Finally, find, remove, and free the shadow IFA */
	shifa = shifa_locate(&span->shifaQ, hwidb, &prev_shifa);
	if (shifa) {
	    if (shifa_remove(&span->shifaQ, prev_shifa, shifa)) {
		if (!shifa->next) {
		    free(shifa);
		} else {
		    buginf("\ncbus_bridge_port_disable failed to free shadow IFA");
		}
	    }
	}
    }
}

/*
 *
 */
static void bridge_set_shifa (boolean newstate)
{
    shifa_current = newstate;
}

/*
 * Process-level recovery of AB data structures lost due to
 * cBus initialization or SP download.
 */
static void bridge_shifa_recovery (void)
{
    if (shifa_current)
	return;

    /*
     * Free any previously allocated process-level data structures
     * related to Autonomous Bridging, and reallocate and rebuild new
     * ones if AB is supported.
     */
    shifa_release();
    if (cbus_bridging_supported)
	shifa_recover();
    shifa_current = TRUE;
}

/******************************************************************************

 Autonomous Bridging: cBus bridge cache access functions

 *****************************************************************************/

/*
 * cbus_bridge_cache_preinit_callback()
 *
 */
static void cbus_bridge_cache_preinit_callback (void)
{
    /*
     * Flush system code data structures gracefully prior to actual init:
     */
    cbus_bridge_cache_flush(TRUE);
}

/*
 * cbus_bridge_cache_init_callback()
 *
 */
static void cbus_bridge_cache_init_callback (void)
{
    mcitype *cbus;
    idbtype *swidb;
    hwidbtype *hwidb;

    cbus_bridging_supported = FALSE;
    cbus_flooding_supported = FALSE;

    cbus = (mcitype *) cbusQ.qhead;
    if (cbus) {
	
        if (!(cbus->flags & CBUSII))
            return;

#ifndef C7000
	if (cbus->sw_version < 0x0200) {
	    return;
	}
#endif
	/*
	 * cBus initialization puts the IFAs out of synch with the SHIFAs.
	 * Indicate this for process-level release and recovery.
	 */
	shifa_current = FALSE;

	/*
	 * Do not support cbus flooding without cbus bridging.
	 */
	if (cbus_switching_supported & FCI_CAP_BRIDGE_AUTO) {
	    cbus_bridging_supported = TRUE;
	    if (cbus_switching_supported & FCI_CAP_BRIDGE_FLOOD_AUTO)
		cbus_flooding_supported = TRUE;
	}

	if (cbus_bridging_supported) {

	    cbus_bce_words_allocated = 0;

	    /*
	     * Ick.  For now, limit to about 3/4 of cache.
	     * LATER:  configurable limits for all protocols.
	     */
	    cbus_bce_words_max = (0xF000 * 3) / 4;

	    /*
	     * Make a low-water mark such that if we fail on
	     * a cache-allocate when we've only used up about 1/4
	     * of the cache ourselves, flush the whole cache to
	     * try and get our fair share.
	     */
	    cbus_bce_words_lowater = (0xF000 * 1) / 4;

	    /*
	     * Initialize the array of mci indices we need to convert
	     * RP permission masks to SP permission masks.
	     */
	    FOR_ALL_SWIDBS (swidb)
		mci_index_array[swidb->if_number] = swidb->hwptr->mci_index;

	} else {

	    /*
	     * De-configure cbus bridging on all interfaces since the
	     * SP no longer supports it.
	     */
	    FOR_ALL_HWIDBS(hwidb)
		hwidb->cbus_bridge_enable = FALSE;
	}
    }
}

/*
 * cbus_bce_add
 *
 * Create and add a BCE to the bridge cache.
 */

static void cbus_bce_add (spantype *span, btetype *bte, hwidbtype *hwidb)
{
    mcitype *cbus;
    mciregtype *regptr;
    int response;
    leveltype level;
    ulong in_permit, out_permit;
    ushort ustemp;

    /*
     * Bail out if cBus or SSE bridging are not enabled on this interface.
     */
    if (!hwidb->cbus_bridge_enable && !hwidb->sse_bridge_forwarding)
	return;

    /*
     * Bail if the bridge cache is in the process of being flushed.
     */
    if (cbus_bce_blocked)
	return;
    
    if (cbus_bce_words_allocated > cbus_bce_words_max) {
	/*
	 * Just flush the bridge cache.
	 *
	 */
	cbus_bridge_cache_flush(FALSE);
    }

    level = raise_interrupt_level(NETS_DISABLE);

    cbus = (mcitype *) cbusQ.qhead;
    regptr = cbus->regptr;

    regptr->argreg = FCI_AB_HASH_TABLE;
    regptr->cmdreg = FCI_SELECT_HASH_TABLE;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	reset_interrupt_level(level);
	buginf("\ncbus_bce_add failed to select bridge cache : %x", response);
	return;
    }

    regptr->argreg = BCE_LENGTH;
    regptr->cmdreg = FCI_CACHE_ALLOC_DYNAMIC;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	reset_interrupt_level(level);
	buginf("\ncbus_bce_add failed to allocate a BCE : %x", response);
	return;
    }
    
    if (!(bte->bte_bce_addr = regptr->argreg)){
	/*
	 * We couldn't allocate an entry, even though we were
	 * under our max limit.
	 * See if we're under our low-water mark and flush the
	 * whole cache if we think other protocols are hogging
	 * too much.
	 */

	if (cbus_bce_words_allocated < cbus_bce_words_lowater) {
	    /*
	     * Call the generic cbus init routine. Note that this routine
	     * will do callbacks including a call on cbus_bridge_cache_flush
	     * which will dismantle the bridge cache in an orderly fashion
	     * before the entire cache is actually invalidated.
	     */
	    cbus_cache_init();
	    reset_interrupt_level(level);
	    return;
	}
	/*
	 * Give up until more entries age out.
	 */
	reset_interrupt_level(level);
	return;
    }
    
    span->cbus_bce_count++;
    cbus_bce_words_allocated += BCE_LENGTH;
    cbus_cache_allocated  += BCE_LENGTH;

    /*
     * NOTA BENE
     * This code writes the BCE without explicitly selecting offsets.
     * It assumes that the cache allocate leaves the rw pointer pointing
     * at the MAC in the BCE.  If the size or order of the fields
     * in the BCE changes, this function will need to be updated.
     */

    regptr->rw_cache = bte->bte_mac_addr.sword[2];
    regptr->rw_cache = bte->bte_mac_addr.sword[1];
    regptr->rw_cache = bte->bte_mac_addr.sword[0];

    if (bte->bte_interface)
	regptr->rw_cache = bte->bte_interface->hwptr->cbus_icb_address;
    else
	regptr->rw_cache = 0;

    switch (bte->bte_action) {
    case BRIDGE_FORWARD:
	ustemp = BCE_ACTION_FORWARD;
	break;
    case BRIDGE_DISCARD:
	ustemp = BCE_ACTION_DISCARD;
	break;
/*  case BRIDGE_RECEIVE: */
    default:
	ustemp = BCE_ACTION_RECEIVE;
	break;
    }

    /*
     * If there are address access lists, set the check-permissions bit.
     * Note that if EITHER address access list exists, both input and 
     * output permission masks will be written.  The default value for
     * these is to permit all.
     */

    in_permit = 0xFFFFFFFF;
    out_permit = 0xFFFFFFFF;

    if (bte->bte_interface && bte->bte_interface->span_address_in) {
	ustemp |= BCE_ACTION_CHKPERM;
	in_permit = cbus_convert_ifnummask(bte->bte_permit_in[0]);
    }

    if (bte->bte_interface && bte->bte_interface->span_address_out) {
	ustemp |= BCE_ACTION_CHKPERM;
	out_permit = cbus_convert_ifnummask(bte->bte_permit_out[0]);
    }

    regptr->rw_cache = ustemp;

    ustemp = (ushort)(in_permit >> 16);
    regptr->rw_cache = ustemp;
    ustemp = (ushort)in_permit;
    regptr->rw_cache = ustemp;

    ustemp = (ushort)(out_permit >> 16);
    regptr->rw_cache = ustemp;
    ustemp = (ushort)out_permit;
    regptr->rw_cache = ustemp;

    /*
     * RX and TX counts
     */
    regptr->rw_cache = 0;
    regptr->rw_cache = 0;
    regptr->rw_cache = 0;
    regptr->rw_cache = 0;

    regptr->argreg = FCI_AB_HASH_TABLE;
    regptr->cmdreg = FCI_SELECT_HASH_TABLE;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	reset_interrupt_level(level);	
	buginf("\ncbus_bce_add failed to select bridge cache : %x", response);
	return;
    }
    
    regptr->cmdreg = FCI_CACHE_INSERT;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	reset_interrupt_level(level);	
	buginf("\ncbus_bce_add failed to insert a BCE : %x", response);
	return;
    }
    reset_interrupt_level(level);
    return;
}


/*
 * cbus_bce_update()
 *
 */

static void cbus_bce_update (btetype *bte)
{
    mcitype *cbus;
    mciregtype *regptr;
    int response;
    leveltype level;
    ulong in_permit, out_permit;
    ushort ustemp;
    
    if (!bte->bte_bce_addr)	/* bail out if not on the cBus */
	return;
    cbus = (mcitype *) cbusQ.qhead;
    regptr = cbus->regptr;

    level = raise_interrupt_level(NETS_DISABLE);

    regptr->argreg = bte->bte_bce_addr;
    regptr->cmdreg = FCI_CACHE_SELECT_ABS;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	reset_interrupt_level(level);	
	buginf("\ncbus_bce_update failed to select a BCE : %x", response);
	return;
    }

    /*
     * NOTA BENE
     * This code rewrites the BCE without explicitly selecting offsets
     * except this initial one.  If the size or order of the fields
     * in the BCE changes, this function will need to be updated.
     */

    regptr->argreg = BCE_MAC_OFFSET;
    regptr->cmdreg = FCI_CACHE_SELECT;
    regptr->rw_cache = bte->bte_mac_addr.sword[2];
    regptr->rw_cache = bte->bte_mac_addr.sword[1];
    regptr->rw_cache = bte->bte_mac_addr.sword[0];

    if (bte->bte_interface)
	regptr->rw_cache = bte->bte_interface->hwptr->cbus_icb_address;
    else
	regptr->rw_cache = 0;

    switch (bte->bte_action) {
    case BRIDGE_FORWARD:
	ustemp = BCE_ACTION_FORWARD;
	break;
    case BRIDGE_DISCARD:
	ustemp = BCE_ACTION_DISCARD;
	break;
/*  case BRIDGE_RECEIVE: */
    default:
	ustemp = BCE_ACTION_RECEIVE;
	break;
    }

    /*
     * If there are address access lists, set the check-permissions bit.
     * Note that if EITHER address access list exists, both input and 
     * output permission masks will be written.  The default value for
     * these is to permit all.
     */

    in_permit = 0xFFFFFFFF;
    out_permit = 0xFFFFFFFF;

    if (bte->bte_interface && bte->bte_interface->span_address_in) {
	ustemp |= BCE_ACTION_CHKPERM;
	in_permit = cbus_convert_ifnummask(bte->bte_permit_in[0]);
    }

    if (bte->bte_interface && bte->bte_interface->span_address_out) {
	ustemp |= BCE_ACTION_CHKPERM;
	out_permit = cbus_convert_ifnummask(bte->bte_permit_out[0]);
    }

    regptr->rw_cache = ustemp;

    ustemp = (ushort)(in_permit >> 16);
    regptr->rw_cache = ustemp;
    ustemp = (ushort)in_permit;
    regptr->rw_cache = ustemp;

    ustemp = (ushort)(out_permit >> 16);
    regptr->rw_cache = ustemp;
    ustemp = (ushort)out_permit;
    regptr->rw_cache = ustemp;

    /*
     * Our friends IP and IPX expect the cache pointer to be zero,
     * so we do them the courtesy without checking the return status.
     */
    regptr->argreg = 0;
    regptr->cmdreg = FCI_CACHE_SELECT_ABS;

    reset_interrupt_level(level);
    return;
}


/*
 * cbus_bce_update_ifa (btetype *)
 *
 * This function re-writes the IFA field associated with a cache entry.
 */

static inline void cbus_bce_update_ifa (btetype *bte)
{

int         response;
leveltype   level;
mcitype     *cbus;
mciregtype  *regptr;

    /*
     * Defensive check for the presence of an interface
     * and a BCE associated with the BTE.
     */

    if (!bte->bte_interface || !bte->bte_bce_addr)
        return;

    cbus = (mcitype *) cbusQ.qhead;
    regptr = cbus->regptr;

    level = raise_interrupt_level(NETS_DISABLE);

    regptr->argreg = bte->bte_bce_addr;
    regptr->cmdreg = FCI_CACHE_SELECT_ABS;
    response = regptr->cmdreg;

    if (response != MCI_RSP_OKAY) {

        reset_interrupt_level(level);
        errmsg(&msgsym(CBUSBCE, CBUS), response);
        return;
    }

    /*
     * Index into the BCE to the IFA field,
     * and then rewrite the `new' interface
     */

    regptr->argreg = BCE_IFA_OFFSET;
    regptr->cmdreg = FCI_CACHE_SELECT;

    regptr->rw_cache = bte->bte_interface->hwptr->cbus_icb_address;

    /*
     * Return the cache pointer to be zero, for IP and IPX.
     */

    regptr->argreg = 0;
    regptr->cmdreg = FCI_CACHE_SELECT_ABS;

    reset_interrupt_level(level);
}


/*
 * cbus_bce_permissions_update (btetype *)
 *
 * Update the access filter permission fields associated with a cache entry.
 */

static inline void cbus_bce_update_permissions (btetype *bte)
{

int         response;
leveltype   level;
mcitype     *cbus;
mciregtype  *regptr;
ushort      ustemp;
ulong       in_permit;
ulong       out_permit;

    /*
     * Check for the presence of an interface and an associated
     * BCE and if we have input or output access lists.
     */

    if (!bte->bte_interface || !bte->bte_bce_addr)
        return;

    if ((!bte->bte_interface->span_address_in)
         && (!bte->bte_interface->span_address_out))
        return;

    cbus = (mcitype *) cbusQ.qhead;
    regptr = cbus->regptr;

    level = raise_interrupt_level(NETS_DISABLE);

    regptr->argreg = bte->bte_bce_addr;
    regptr->cmdreg = FCI_CACHE_SELECT_ABS;

    response = regptr->cmdreg;

    if (response != MCI_RSP_OKAY) {

        reset_interrupt_level(level);
        errmsg(&msgsym(CBUSBCEPERM, CBUS), response);
        return;
    }

     /*
      * In writing to the permission masks then both input and output masks are
      * written to, since the offset pointer into the BCE is automatically
      * incremented for us on each write. The default value for these is to
      * permit all.
      */

    in_permit = 0xFFFFFFFF;
    out_permit = 0xFFFFFFFF;

    regptr->argreg = BCE_PERMIT_IN_OFFSET;
    regptr->cmdreg = FCI_CACHE_SELECT;

    if (bte->bte_interface->span_address_in) {
        in_permit = cbus_convert_ifnummask(bte->bte_permit_in[0]);
    }

    if (bte->bte_interface->span_address_out) {
        out_permit = cbus_convert_ifnummask(bte->bte_permit_out[0]);
    }

    regptr->rw_cache = BCE_ACTION_CHKPERM;

    ustemp = (ushort)(in_permit >> 16);
    regptr->rw_cache = ustemp;
    ustemp = (ushort)in_permit;
    regptr->rw_cache = ustemp;

    ustemp = (ushort)(out_permit >> 16);
    regptr->rw_cache = ustemp;
    ustemp = (ushort)out_permit;
    regptr->rw_cache = ustemp;

    /*
     * Return the cache pointer to be zero, for IP and IPX.
     */

    regptr->argreg = 0;
    regptr->cmdreg = FCI_CACHE_SELECT_ABS;

    reset_interrupt_level(level);
    return;
}


/*
 * cbus_bce_poll
 *
 */

static void cbus_bce_poll (btetype *bte)
{
    mcitype *cbus;
    mciregtype *regptr;
    int response;
    uint counter;
    leveltype level;

    cbus = (mcitype *) cbusQ.qhead;
    regptr = cbus->regptr;

    if (!bte->bte_bce_addr)
	return;

    level = raise_interrupt_level(NETS_DISABLE);

    regptr->argreg = bte->bte_bce_addr + BCE_RX_OFFSET;
    regptr->cmdreg = FCI_MEMA_READ_CLEAR_LONG;
    response = regptr->cmdreg;

    if (response != MCI_RSP_OKAY) {
	buginf("\ncbus_bce_poll failed to read IFA %x : %x",
	       bte->bte_bce_addr, response);
    } else {
	counter = regptr->largreg;
	bte->bte_rxcount += counter;
	bte->bte_auto_rxcount += counter;
	if (bte->bte_interface)
	    bte->bte_interface->hwptr->counters.inputs_auto[ACCT_PROTO_BRIDGE] +=
		counter;
    }

    /*
     * Transmit counts are read as often as recieve counts.
     */
    regptr->argreg = bte->bte_bce_addr + BCE_TX_OFFSET;
    regptr->cmdreg = FCI_MEMA_READ_CLEAR_LONG;
    response = regptr->cmdreg;

    if (response != MCI_RSP_OKAY) {
	buginf("\ncbus_bce_poll failed to read IFA %x : %x",
	       bte->bte_bce_addr, response);
    } else {
	counter = regptr->largreg;
	bte->bte_txcount += counter;
	bte->bte_auto_txcount += counter;
	if (bte->bte_interface)
	    bte->bte_interface->hwptr->counters.outputs_auto[ACCT_PROTO_BRIDGE] +=
		counter;
    }

    reset_interrupt_level(level);
}

/*
 * cbus_bce_remove()
 */

static void cbus_bce_remove (spantype *span, btetype *bte)
{
    mcitype *cbus;
    mciregtype *regptr;
    int response;
    leveltype level;
    ushort mema_addr;

    if (!bte->bte_bce_addr)
	return;
    
    reg_invoke_auto_sw_cache_version(LINK_BRIDGE);

    cbus = (mcitype *) cbusQ.qhead;
    regptr = cbus->regptr;

    level = raise_interrupt_level(NETS_DISABLE);

    /* NULL bte->bte_bce_addr regardless of the success of the operation */
    mema_addr = bte->bte_bce_addr;
    bte->bte_bce_addr = 0;
    bte->bte_auto_rxcount = 0;
    bte->bte_auto_txcount = 0;
    
    regptr->argreg = FCI_AB_HASH_TABLE;
    regptr->cmdreg = FCI_SELECT_HASH_TABLE;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	reset_interrupt_level(level);
	buginf("\ncbus_bce_remove failed to select bridge cache : %x",
	       response);
	return;
    }

    regptr->largreg = (BCE_LENGTH << 16) | mema_addr;
    regptr->cmdreg = FCI_CACHE_FREE_DYNAMIC;
    response = regptr->cmdreg;
    if (response != MCI_RSP_OKAY) {
	reset_interrupt_level(level);	
	buginf("\ncbus_bce_remove failed to free a BCE : %x", response);
	return;
    }

    /* account only for entries successfully freed */
    cbus_bce_words_allocated -= BCE_LENGTH;
    cbus_cache_allocated -= BCE_LENGTH;
    span->cbus_bce_count--;

    reset_interrupt_level(level);
    return;
}

/*
 * cbus_bridge_cache_flush
 *
 * Either IP/IPX is about to invalidate the cache or cbus_bce_add
 * has hit cbus_bce_words_max and we need to flush the bces.
 *
 * We will walk the bridge tables freeing all bridge cache entries,
 * and then check that the allocation for cBus bridging is zero.
 */

static void cbus_bridge_cache_flush (boolean dont_touch_the_cache)
{
    ushort i, j;
    leveltype level;
    spantype *span;
    btetype *bte;

    reg_invoke_auto_sw_cache_version(LINK_BRIDGE);

    if (!cbus_bridging_supported)
	return;

    /*
     * Block the creation of new bces; cbus_bce_add checks this.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    cbus_bce_blocked = TRUE;
    reset_interrupt_level(level);

    for (i = 0; i < SPAN_MAXINDEX; i++) {
 	/*
	 * We could use !span->bce_count to skip this loop as well,
	 * but we would be relying on the accuracy of our accounting
	 * to indicate the presence of bce addresses in the bridge
	 * table.  I'd rather examine each bridge table entry directly
	 * to avoid any possibility of future reference to an invalid
	 * cache address.  Also check to make sure the SSE didn't create
	 * any bce's.
	 */
	if (!(span = spanarray[i]) ||
	    (!span->shifaQ.count && !span->sse_interfaces))
	    continue;

	for (j = 0; j < NETHASHLEN; j++) {
	    level = raise_interrupt_level(NETS_DISABLE);
	    bte = (btetype *) span->bridge_tbl[j];
	    while (bte) {
		if (bte->bte_bce_addr)
		    /*
		     * This boolean will be TRUE when this function is called
		     * by IP/IPX since they will have pushed cache utilization
		     * to its limit and it cannot be gracefully freed.
		     * We resort to flushing just the record of bridge cache
		     * allocation rather than actually freeing it.
		     */
		    if (dont_touch_the_cache) {
			span->cbus_bce_count--;
			cbus_bce_words_allocated -= BCE_LENGTH;
			cbus_cache_allocated -= BCE_LENGTH;
			bte->bte_bce_addr = 0;
			bte->bte_auto_rxcount = 0;
			bte->bte_auto_txcount = 0;
		    /*
		     * This boolean will be FALSE when this function is called
		     * by cbus_bce_add and we can do the real thing.
		     */
		    } else {
			cbus_bce_poll(bte);
			cbus_bce_remove(span, bte);
		    }
		bte = bte->bte_next;
	    }
	    reset_interrupt_level(level);
	}

	if (span->cbus_bce_count) {
	    buginf("\ncbus_bridge_cache_flush found %d residual entries, group %d",
		   span->cbus_bce_count, i);
	    span->cbus_bce_count = 0;
	}
    }

    if (cbus_bce_words_allocated) {
	buginf("\ncbus_bridge_cache_flush found %d residual words",
	       cbus_bce_words_allocated);
	cbus_bce_words_allocated = 0;
	cbus_cache_allocated = 0;
    }

    /*
     * Unblock the creation of new bces;  cbus_bce_add checks this.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    cbus_bce_blocked = FALSE;
    reset_interrupt_level(level);
}

/******************************************************************************

 Autonomous Bridging: cBus bridge flood functions

 *****************************************************************************/

/*
 * cbus_flood_all
 *
 * Returns TRUE if all interfaces on which a packet is to be flooded
 * can be flooded entirely by the cBus, and the input interface has
 * cBus bridging enabled.
 *
 * Returns FALSE if any interface on which a packet is to be flooded
 * must be flooded by the RP, or the input interface does not have
 * cBus bridging enabled.
 *
 * As a side effect, it records whether or not an interface can be
 * SP flooded by setting auto_flood on the oif descriptor.
 *
 * copy_the_packet is a pointer of boolean which indicates whether we 
 * need to copy the packet from cBus to network buffer or not when cbus
 * copy assistance is enabled.
 */

boolean cbus_flood_all (idbtype *swidb,
			tbifd_type *oif_list,
			boolean *copy_the_packet)
{
    boolean flooded_all, once_flag;
    spantype *span;
    tbifd_type *oif;
    hwidbtype *hwidb = swidb->hwptr;
    idbtype *swidb_out;
    hwidbtype *hwidb_out;

    if (!cbus_flooding_supported)
	return(FALSE);

    flooded_all = TRUE;
    once_flag = FALSE;
    *copy_the_packet = FALSE;

    /*
     * Check for input interfaces from which the packet must be read in
     * before cBus flooding.  These would typically (but not always) be
     * off-complex interfaces.
     */
    if (!hwidb->cbus_bridge_enable)
	flooded_all = FALSE;

    span = swidb->span_ptr;
    span->auto_flood_count = 0;

    for (oif = oif_list; oif; oif = oif->tbifd_next) {
	swidb_out = oif->tbifd_swidb;
	hwidb_out = swidb_out->hwptr;

	/* Assume we will NOT SP flood this interface */
	oif->tbifd_attributes &= ~TBIFD_AUTO_FLOOD;

	/* THESE INTERFACES WILL BE DISREGARDED */

	/* non-forwarding interfaces */
	if (!swidb_out->circ_canforward)
	    continue;
	/* downed interfaces (wouldn't be in the flood list) */
	if (!interface_up_simple(swidb_out))
	    continue;
	/* input interface */
	if (swidb_out->if_number == swidb->if_number)
	    continue;

	/* THESE INTERFACES WILL BE RP FLOODED */

	if (hwidb_out->span_bridge_process_force ||
	    hwidb_out->priority_list)
	    *copy_the_packet = TRUE;

	/* non-cbus-bridging interfaces */
	if (!(hwidb_out->cbus_bridge_enable)) {
	    flooded_all = FALSE;
	    continue;
	}

	/* THESE INTERFACES WILL BE CBUS FLOODED */

	if (tbridge_debug_cbus_flood) {
	    if (!once_flag) {
		buginf("\ncBus flood from %s to :", hwidb->hw_namestring);
		once_flag = TRUE;
	    }
	    buginf(" %s", hwidb_out->hw_namestring);
	}

	/* Indicate that we will SP flood this interface */
	span->auto_flood_count++;
	oif->tbifd_attributes |= TBIFD_AUTO_FLOOD;

	hwidb_out->counters.outputs_auto[ACCT_PROTO_BRIDGE]++;
	hwidb_out->counters.tx_cumbytes_auto[ACCT_PROTO_BRIDGE]
	    += hwidb->rxlength;
    }
    return(flooded_all);
}

boolean cbus_flood_oncomplex (hwidbtype *input)
{
    mciregtype *srcreg;
    int response;

    srcreg = input->mci_regptr;

    /* the zero argument lets the ucode know this is an on-complex flood */

    srcreg->argreg = 0;
    srcreg->cmdreg = FCI_BRIDGE_AUTO_FLOOD;
    response = srcreg->cmdreg;
    if (response != MCI_RSP_OKAY) {
	buginf("\ncbus_flood_oncomplex failed to flood: %x",
	       response);
	return(FALSE);
    }

    if (tbridge_debug_cbus_flood)
	buginf("\n cBus on-complex flood initiated from %s",
	       input->hw_namestring);

    return(TRUE);
}

/******************************************************************************

 Autonomous Bridging: IDB validation functions

 *****************************************************************************/

/*
 * cbus_bridging_idb
 * See if the passed idb is a valid interface for cBus bridging.
 */
static boolean cbus_bridging_idb (idbtype *idbsw)
{
    mcitype *cbus;
    hwidbtype *idb = idbsw->hwptr;

    if (!(idb->status & IDB_CBUS))
	return(FALSE);

    cbus = idb->devctl;
    if (!(cbus->flags & CBUSII))
	return(FALSE);

    /*
     * cBus bridging is only supported on translation media i.e. not
     * encapsulation media.
     */
    switch (idbsw->tbridge_media) {
    case TBR_ETHER:
	return(TRUE);
    case TBR_FDDI:
	return(TRUE);
    case TBR_CTR:
	/* Not supported yet */
	return(FALSE);
    case TBR_HDLC_TRANSIT:
        /* We cann't filter out PPP by checking the media type, because
         * PPP has the same media type as HDLC. In fastswitch, PPP and HDLC
         * packets are handled in the same way. But, here, we have to treat
         * them differently.
         */
	if (is_ppp(idb))
	    return(FALSE);
	switch (idb->type) {
	case IDBTYPE_HSSI:
	    return(TRUE);
	case IDBTYPE_CBUSSERIAL:
	    return(TRUE);
	default:
	    return(FALSE);
	}
	/* Currently not supported*/
    case TBR_LEX_TRANSIT:
	return (FALSE);
    default:
	return(FALSE);
    }
}

/*
 * cbus_srt_command_check
 *
 * After each bridge-group command, check the configuration for
 * dependencies such as the suppression of autonomous bridging
 * due to the configuration of access lists, etc.
 */
static void cbus_srt_config_check (idbtype *idb, spantype *span)
{
    boolean cbus_bridge_suppress;
    hwidbtype *hwidb;

    hwidb = idb->hwptr;
    if (hwidb->cbus_bridge_config) {

        /*
         * Check for the configuration of all the bridging features which
         * CANNOT be supported in conjunction with autonomous bridging.
         * These are access list of any type other than MAC address,
         * LAT compression, and circuit groups.
         * If any of them is present, we must suppress autonomous bridging
         * by disabling it while leaving it configured.
         * If none are present, we may restore autonomous bridging if it
         * is currently configured.
         */

        cbus_bridge_suppress =
            (idb->span_type_in || idb->span_type_out ||
             idb->span_lsap_in || idb->span_lsap_out ||
             idb->span_pattern_in || idb->span_pattern_out ||
             idb->span_latcompress ||
             idb->span_lat_in_svc_deny[0] ||
             idb->span_lat_out_svc_deny[0] ||
             idb->span_lat_in_svc_permit[0] ||
             idb->span_lat_out_svc_permit[0] ||
             idb->circ_group);

        if (cbus_bridge_suppress) {
            if (hwidb->cbus_bridge_enable) {
                printf("\n%%Autonomous bridging suppressed on %s",
                       idb->namestring);
                cbus_bridge_port_disable(span, idb);
            }
        } else {
            if (!hwidb->cbus_bridge_enable) {
                printf("\n%%Autonomous bridging restored on %s",
                       idb->namestring);
                cbus_bridge_port_enable(span, idb);
            }
        }
    }
}


/*
 * cbus_bridgegroup_command
 * Handle "bridge-group 1 cbus-bridging"
 */

static const char nocbusbridging[] = "\n%%%s does not support cBus bridging";
static const char nocbusbridgingsupport[] = "\n%%cBus bridging not supported";
static const char nobridgeconfigured [] = "\n%%%s bridging not configured";
static const char nocbusbridgeconfigured[] =
                                         "\n%%%s cBus bridging not configured";
static const char cbusbridgeconfigured[] =
                                     "\n%%%s cBus bridging already configured";

static void cbus_bridgegroup_command (parseinfo *csb)
{
    spantype *span;
    hwidbtype *hwidb;
    idbtype *idb;
    int index;

    idb = csb->interface;
    hwidb = idb->hwptr;
    if (csb->nvgen) {
	if (!idb->span_index || (idb->span_index != GETOBJ(int,1))) {
	    return;
	}
	nv_write(idb->hwptr->cbus_bridge_config, csb->nv_command);
	return;
    }
    if (!cbus_bridging_supported) {
	printf(nocbusbridgingsupport);
	return;
    }
    /*
     * Get index and make sure that it is in range.
     * Do not allow index to be changed to other than zero.
     */
    index = GETOBJ(int,1);
    if ((index < 1) || (index >= SPAN_MAXINDEX)) {
        printf(badspan);
        return;
    }
    if (idb->span_index && index && (index != idb->span_index)) {
        printf(badspan);
        return;
    }
    span = span_lookup(index, FALSE, 0);
    if (!span) {
        printf(nomemory);
        return;
    }
    if (!span)
	return;
    if (!csb->sense) {
	if (!hwidb->cbus_bridge_config)
	    printf(nocbusbridgeconfigured, idb->namestring);

	cbus_bridge_port_disable(span, idb);
	hwidb->cbus_bridge_config = FALSE;
    } else {
	if (!idb->tbridge_on_swidb) {
	    printf(nobridgeconfigured, idb->namestring);
	    return;
	} 
	if (hwidb->cbus_bridge_enable) {
	    printf(cbusbridgeconfigured, idb->namestring);
	    return;
	}
	if (!cbus_bridging_idb(idb)) {
	    printf(nocbusbridging, idb->namestring);
	    return;
	}

        /*
         * Autonomous Bridging configured and then enabled,
         * provided no other conflicting configuration present.
         */

        hwidb->cbus_bridge_config = TRUE;
        cbus_bridge_port_enable(span, idb);
        cbus_srt_config_check(idb, span);
    }

    /*
     * cbus assistance copy for flooding is not compatible with Autonomous 
     * bridging due to the fact that AB alwasy flushes the packet after 
     * it's done, with the assumption that we have copied the packet to 
     * network buffer already.
     * Need to reevaluate the boolean for cbus copy.
     */
    tbridge_check_cbus_copy_capable(span);
}


/*
 * cbus_srt_status_general
 * "show bridge-group" support.
 * Print out general capabilities of our hardware
 */

static void cbus_srt_status_general (void)
{
    if (tbridge_debug_cbus) {
	if (cbus_bridging_supported)
	    printf("\n\ncBus bridging supported.");
	else
	    printf("\n\ncBus bridging not supported.");
	if (cbus_flooding_supported)
	    printf("   cBus flooding supported.");
	else
	    printf("   cBus flooding not supported.");

	printf("\nBridge cache words in use: %d/%d",
	       cbus_bce_words_allocated, cbus_bce_words_max);

	if (cbus_bridging_supported) {
	    printf("\nIFA links:");
	    cbus_list_links();
	    printf("\n");
	}
    }
}

/*
 * cbus_srt_status_bgroup
 * "show bridge-group" support.
 * Print out capabilities and status of our hardware for this bridge group
 */

static void cbus_srt_status_bgroup (spantype *span)
{
    shifaQtype *shifa;


    /*
     * If cBus transparent bridge debugging is enabled and there
     * are cBus bridged interfaces in the bridge group, display
     * the relevant info.
     */
    if (tbridge_debug_cbus && span->shifaQ.count) {

	printf("\n   Bridge cache entries in use: %d\n",
	       span->cbus_bce_count);
	printf("   cBus bridging ports: %d\n", span->shifaQ.count);
	for (shifa = (shifaQtype *) span->shifaQ.qhead;
	     shifa; shifa = shifa->next)
	    printf("\n   %s", shifa->hwidb->firstsw->namestring);
	printf("\n");
    }
}

/*
 * cbus_srt_status_bce
 * "show bridge-group" support.
 * Print out hardware specific information for this BCE
 */

static void cbus_srt_status_bce (btetype *mac, boolean verbose)
{
    if (tbridge_debug_cbus && mac->bte_bce_addr)

        if (verbose)
            printf ("\n%-54s%-15d%-11d", "cBus bridged packets",
                    mac->bte_auto_rxcount, mac->bte_auto_txcount);
        else
            printf ("\n%-47s%-10d%-11d", "cBus bridged packets",
                    mac->bte_auto_rxcount, mac->bte_auto_txcount);
}


/*
 * cbus_poll_interface_stats (idbtype *)
 *
 * This function reads the bridge table for the Spanning Tree group
 * associated with the given interface. It matches bridge table entries
 * against the interface, and then if the entry has an associated bridge
 * cache entry, it updates the count of received and transmitted frames via
 * cbus_bce_poll (). This way do the interface statistics also incorporate
 * counts of autonomously switched packets.
 */

static void bridge_poll_interface_stats (idbtype *swidb)
{

    int      bt_index;         /* Bridge Table Index */
    btetype  *bte;             /* Arbitrary Bridge Table Entry */
    btetype  **hash_table_ptr;  /* Table pointer for this Spanning Tree
				  Group */

    /* If we're not doing CBus bridging, or this interface
       isn't within a Spanning Tree Group then forget it */

    if (!cbus_bridging_supported)
        return;

    if (!swidb->tbridge_on_swidb)
        return;

    /* Start at the beginning of the Bridge Table
       associated with this Spanning Tree Group */

    hash_table_ptr = swidb->span_ptr->bridge_tbl;

    for (bt_index = 0; bt_index < NETHASHLEN; bt_index++)
	{
        for (bte = hash_table_ptr[bt_index]; bte != NULL; bte =
	     bte->bte_next)
	    {
            /* was this entry learned on this interface and
               does it have a corresponding cache entry ? */

            if ((bte->bte_bce_addr) && (bte->bte_interface)
                 && (bte->bte_interface == swidb))
		{
                cbus_bce_poll (bte);
            }
        }
    }
}


/*
 * cbus_bridge_register_thyself
 * Register cBus specific bridging functions.
 */

static void cbus_bridge_register_thyself (void)
{
    /*
     * The following are for bridging happening on the Switch Processor
     */
    reg_add_srt_set_port_state(cbus_ifa_set_port_state,
			       "cbus_ifa_set_port_state");
    reg_add_srt_set_shifa(bridge_set_shifa , "bridge_set_shifa");
    reg_add_srt_shifa_recovery(bridge_shifa_recovery,"bridge_shifa_recovery");
    reg_add_srt_bce_statistics(cbus_bce_poll, "cbus_bce_poll");
    reg_add_srt_cbus_bridge_port_enable(cbus_bridge_port_enable,
					"cbus_bridge_port_enable");
    reg_add_srt_cbus_bridge_port_disable(cbus_bridge_port_disable,
					 "cbus_bridge_port_disable");
    reg_add_srt_bce_update(cbus_bce_update, "cbus_bce_update");
    reg_add_srt_bce_remove(cbus_bce_remove, "cbus_bce_remove");
    reg_add_srt_bce_add(cbus_bce_add, "cbus_bce_add");
    reg_add_srt_status_general(cbus_srt_status_general,
			       "cbus_srt_status_general");
    reg_add_srt_status_bgroup(cbus_srt_status_bgroup,"cbus_srt_status_bgroup");
    reg_add_srt_status_bce(cbus_srt_status_bce,"cbus_srt_status_bce");
    reg_add_srt_config_check(cbus_srt_config_check,"cbus_srt_config_check");
    reg_add_auto_sw_bridge_cache_flush(cbus_bridge_cache_flush,
				       "cbus_bridge_cache_flush");
    reg_add_auto_sw_cache_preinit_callback(cbus_bridge_cache_preinit_callback,
				   "cbus_bridge_cache_preinit_callback");
    reg_add_auto_sw_cache_init_callback(cbus_bridge_cache_init_callback,
				    "cbus_bridge_cache_init_callback");

    reg_add_bridge_poll_interface_stats	(bridge_poll_interface_stats,
                                         "bridge_poll_interface_stats");

    reg_add_cbus_bce_update_ifa (cbus_bce_update_ifa, "cbus_bce_update_ifa");
    reg_add_cbus_bce_update_permissions (cbus_bce_update_permissions,
                                         "cbus_bce_update_permissions");

}


/*
 * cbus_bridge_init
 * Subsystem initialization for cBus bridging
 */

static void cbus_bridge_init (subsystype *subsys)
{
    cbus_bridge_register_thyself();
    tbridge_debug_init();
    smf_debug_init();
    parser_add_commands(PARSE_ADD_CFG_INT_BRIDGE,
			&pname(cbus_bridging_commands),
			"cbus bridging");
    /*
     * Make sure CBUS is ready:
     */
    cbus_cache_init();
}

/*
 * cBus bridging subsystem header
 */

#define CBUSBR_MAJVERSION 1
#define CBUSBR_MINVERSION 0
#define CBUSBR_EDITVERSION 1

SUBSYS_HEADER(cbus_bridge, CBUSBR_MAJVERSION, CBUSBR_MINVERSION,
	                   CBUSBR_EDITVERSION,
	      cbus_bridge_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: tbridge",
	      "req: tbridge");
