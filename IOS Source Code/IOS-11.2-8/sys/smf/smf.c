/* $Id: smf.c,v 3.4.4.6 1996/08/28 13:09:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/smf/smf.c,v $
 *------------------------------------------------------------------
 * Software MAC Address Filter (SMF) functions.
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The SMF is intended for use when an interface is listening
 * promiscuously to match and dispose of received packets based
 * upon their destination MAC addresses in an efficient way.
 * In particular, it provides a facility that may be used on a
 * per-interface basis in support of Concurrent Routing and
 * Bridging (CRB) and eventually Integrated Routing and Bridging (IRB).
 *
 *------------------------------------------------------------------
 * $Log: smf.c,v $
 * Revision 3.4.4.6  1996/08/28  13:09:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.4.5  1996/06/28  23:25:53  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.4.4  1996/06/27  20:59:00  snyder
 * CSCdi61553:  Yeats quotes waste space
 * Branch: California_branch
 *              get 116 bytes back
 *
 * Revision 3.4.4.3  1996/05/09  14:40:43  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.4.2  1996/05/04  01:45:05  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface smf support.
 *
 * Revision 3.4.4.1  1996/04/29  18:08:40  wilber
 * CSCdi51983:  smf_insert_address call malloc at interrupt level
 * Branch: California_branch
 * Check if running on an interrupt stack before allocating SMF memory
 *
 * Revision 3.4.30.1  1996/04/27  07:24:37  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.20.1  1996/04/08  02:08:13  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.10.2  1996/03/23  04:54:38  rlowe
 * More sync-time fun and games.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4.10.1  1996/03/22  22:56:38  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4  1996/03/07  23:31:35  speakman
 * CSCdi48222:  SMF creates chunk storage even when unconfigured
 * Replace SMF chunk memory with managed memory and fix the original
 * problem with a simple check for onintstack.
 *
 * Revision 3.3.22.1  1996/03/05  05:50:26  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.16.1  1996/02/27  21:19:23  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3  1996/01/10  06:48:06  syiu
 * CSCdi41302:  Concurrent LAT & bridging for HDLC and PPP
 *
 * Revision 3.2.14.1  1996/02/21  04:09:35  cyoung
 * VTP type added to 'show smf' command.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  18:52:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:24:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/26  00:28:58  speakman
 * CSCdi41837:  SMF calls malloc and free at interrupt (SYS-3-INVMEMINT)
 * Convert SMF to use a chunk instead of malloc() and free().
 *
 * Revision 2.6  1995/10/03  14:41:42  speakman
 * CSCdi41428:  SMF debug no longer compiles
 * CSCdi41340 broke SMF debug.  Make smf_mactyp_string external.
 *
 * Revision 2.5  1995/09/30  22:39:59  speakman
 * CSCdi41340:  Platform-independent tbridging performance enhancements
 * Create smf_match_address_inline out of smf_match_address which becomes
 * just a function wrapper for this inline.  Forgo the pointer safety
 * checks in ieee_equal and just do a direct MAC address comparison.
 *
 * Revision 2.4  1995/06/21  18:56:09  alam
 * CSCdi36191:  IBM spanning tree doesnt form with SRB/FDDI
 * Add sbridge_smf_update registry in srb_init.
 *
 * Revision 2.3  1995/06/21  09:00:38  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/20  07:17:55  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:44:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "ieee.h"
#include "linktype.h"

#include "sys_registry.h"

#include "config.h"
#include "parser.h"
#include "logger.h"

#include "../ui/debug.h"
#include "smf_debug_flags.h"

#include "smf_public.h"
#include "smf_private.h"
#include "smf_private_inline.h"
#include "msg_smf.c"

/*
 * Just a couple of low-tech value-to-string mapping
 * functions with a touch of millennial foreboding.
 */

static char *
smf_action_string (ulong result)

{
    switch (result & SMF_ACTION_MASK) {
    case SMF_ACTION_RECEIVE:	return("RCV");
    case SMF_ACTION_FORWARD:	return("FWD");
    case SMF_ACTION_DISCARD:	return("DIS");
    /*
     * Don't code a default so that the compiler will help
     * to keep this function current with the defined enums.
     */
    }
    return("???");
}

char *
smf_mactyp_string (ulong result)

{
    switch (result & SMF_MACTYP_MASK) {
    case SMF_MACTYP_OWN_UNICAST:	return("Interface MAC address");
    case SMF_MACTYP_PHYS_BCAST:		return("Physical broadcast");
    case SMF_MACTYP_FDDI_TRANSIT:	return("Cisco transit multicast");
    case SMF_MACTYP_STANDBY_UNICAST:	return("HSRP");
    case SMF_MACTYP_IEEE_SPAN:		return("IEEE spanning tree");
    case SMF_MACTYP_DEC_SPAN:		return("DEC spanning tree");
    case SMF_MACTYP_FUNC_SPAN:		return("Spanning tree");
    case SMF_MACTYP_IEEE_IBM_SPAN:	return("IBM spanning tree");	
    case SMF_MACTYP_CDP_MCAST:		return("CDP");
    case SMF_MACTYP_VTP_MCAST:		return("VTP");
    case SMF_MACTYP_AT_BCAST:		return("Appletalk broadcast");
    case SMF_MACTYP_AT_ZONE:		return("Appletalk zone");
    case SMF_MACTYP_CLNS_MCAST:		return("CLNS");
    case SMF_MACTYP_DECNET_MCAST:	return("DECNET");
    case SMF_MACTYP_HP_MCAST:		return("HP");
    case SMF_MACTYP_IP_MCAST:		return("IP multicast");
    case SMF_MACTYP_LAT_MCAST:		return("LAT multicast");
    case SMF_MACTYP_LAT_UNICAST:        return("LAT");
    case SMF_MACTYP_APOLLO_MCAST:	return("APOLLO");
    case SMF_MACTYP_MCAST_RANGE:	return("Multicast range");
    case SMF_MACTYP_ISL_VLAN_MCAST:     return("ISL vLAN Multicast");
    case SMF_MACTYP_BVI:  		return("Bridge-group Virtual Interface");

    /*
     * Don't code a default so that the compiler will help
     * to keep this function current with the defined enums.
     */
    }
    return("???");
}

/*
 * smf_create_filter
 *
 * An idempotent (can I say that on TV?) function to
 * dynamically allocate the hash table for SMF entries
 * when one does not exist.  Gets invoked by the first
 * call so smf_insert_address for a given interface.
 */

boolean
smf_create_filter (idbtype *swidb)

{
    smf_entry_type **filter;

    if (swidb->sfw_mac_filter)
	return(TRUE);

    filter = malloc_named((sizeof(smf_entry_type *)) * NETHASHLEN,
			  "SMF filter");

    if (!filter) {
	return(FALSE);
    }

    swidb->sfw_mac_filter = filter;
    return(TRUE);
}

/*
 * smf_insert_address
 *
 * Add an SMF entry to the SMF at the end of the hash chain
 * for the given address.  Preserve uniqueness of entries,
 * and be a good citizen about managing the required memory.
 */

boolean
smf_insert_address (uchar *mac_addr, idbtype *swidb,
		    const ulong result)

{
    smf_entry_type **filter, *entry, *last;

    if (onintstack())
        return(FALSE);

    filter = swidb->sfw_mac_filter;

    if (!filter) {
	if (smf_create_filter(swidb)) {
	    filter = swidb->sfw_mac_filter;
	} else {
	    return(FALSE);
	}
    }

    switch (result & SMF_ACTION_MASK) {

    case SMF_ACTION_RECEIVE:
    case SMF_ACTION_FORWARD:
    case SMF_ACTION_DISCARD:
	break;

    default:
	errmsg(SMF_ERR_INVALID_ACTION, mac_addr, swidb->namestring, result);
	return(FALSE);
	break;
    }

    /* Check for an exact duplicate */
    entry = smf_locate_entry_inline(mac_addr, filter, result);

    if (entry)
	return(TRUE);

    entry = malloc_named(sizeof(smf_entry_type), "SMF entry");

    if (!entry) {
	return(FALSE);
    }

    entry->smf_entry_next = NULL;
    ieee_copy(mac_addr, (uchar *)entry->smf_entry_mac_addr.sword);
    entry->smf_entry_result = result;
    entry->smf_entry_hits = 0;

    /* Locate the end of the hash chain */
    for (last = filter[SMF_HASH(mac_addr)];
	 last && last->smf_entry_next;
	 last = last->smf_entry_next);

    if (smf_debug_update)
	buginf("\nSMF: inserting %e on %s - %s", mac_addr,
	       swidb->namestring, smf_mactyp_string(result));

    if (last)
	last->smf_entry_next = entry;
    else
	filter[SMF_HASH(mac_addr)] = entry;

    return(TRUE);
}

/*
 * smf_delete_address
 *
 * Locate an SMF entry by address and result, as well as that
 * entry's predecessor, remove it from the SMF, and free memory.
 */

boolean
smf_delete_address (uchar *mac_addr, idbtype *swidb, ulong result)

{
    smf_entry_type **filter, *entry, *prev_entry;

    /*
     * This one line has a long and ignominious history.  It all started
     * with CSCdi41837, then CSCdi42919, and finally CSCdi48222.  The whole
     * rigamarole could have been avoided just by adding this check in the
     * first place to check for the corner case when cbus_interrupt calls
     * cbus_init which calls the interface reset functions which attempt to
     * clear the SMF.  In this case, do not attempt to free SMF memory.
     */
    if (onintstack())
	return(FALSE);

    filter = swidb->sfw_mac_filter;

    if (!filter)
	return(FALSE);

    entry = smf_locate_prev_entry_inline(mac_addr, filter, result,
					 &prev_entry);

    if (!entry)
	return(FALSE);

    if (smf_debug_update)
	buginf("\nSMF: deleting %e on %s - %s", mac_addr,
	       swidb->namestring, smf_mactyp_string(result));

    if (prev_entry) {
	prev_entry->smf_entry_next = entry->smf_entry_next;
    } else {
	filter[SMF_HASH(mac_addr)] = entry->smf_entry_next;
    }

    free(entry);

    return(TRUE);
}

/*
 * smf_delete_by_result
 *
 * Some protocols (atalk, ip multicast) don't retain
 * a record of all the multicasts they stick in the SMF, so when
 * they are deconfigured, we need to find those addresses just
 * based on their associated results in the SMF.  Thus this function.
 */

boolean
smf_delete_by_result (idbtype *swidb, ulong result)

{
    smf_entry_type **filter, *prev_entry, *entry, *next;
    int hash;
    leveltype level;
    boolean found;

    /*
     * This one line has a long and ignominious history.  It all started
     * with CSCdi41837, then CSCdi42919, and finally CSCdi48222.  The whole
     * rigamarole could have been avoided just by adding this check in the
     * first place to check for the corner case when cbus_interrupt calls
     * cbus_init which calls the interface reset functions which attempt to
     * clear the SMF.  In this case, do not attempt to free SMF memory.
     */
    if (onintstack())
	return(FALSE);

    filter = swidb->sfw_mac_filter;

    if (!filter)
	return(FALSE);

    found = FALSE;
    for (hash = 0; hash < NETHASHLEN; hash++) {

	level = raise_interrupt_level(ALL_DISABLE);
	
	prev_entry = NULL;
	for (entry = filter[hash]; entry; ) {
	    next = entry->smf_entry_next;
	    if (entry->smf_entry_result == result) {
		found = TRUE;

		if (smf_debug_update)
		    buginf("\nSMF: deleting %e on %s - %s (by result)",
			   entry->smf_entry_mac_addr.byte,
			   swidb->namestring, smf_mactyp_string(result));

		if (prev_entry) {
		    prev_entry->smf_entry_next = entry->smf_entry_next;
		} else {
		    filter[hash] = entry->smf_entry_next;
		}
		free(entry);
	    } else {
		prev_entry = entry;
	    }
	    entry = next;
	}

	reset_interrupt_level(level);
    }

    return(found);
}

/*
 * smf_match_address
 *
 * This is just a function call wrapper for the inline of the
 * same name.  Use this interface to the SMF when data hiding
 * is more important than performance.
 */

ulong
smf_match_address (uchar *mac_addr, idbtype *swidb)

{
    return(smf_match_address_inline(mac_addr, swidb));
}

/*
 * smf_show_filter
 *
 * Pretty printer.
 */

void
smf_show_filter (idbtype *swidb)

{
    smf_entry_type **filter, *entry;
    int hash, length;
    boolean print_heading = TRUE;

    filter = swidb->sfw_mac_filter;

    if (!filter)
	return;

    for (hash = 0; hash < NETHASHLEN; hash++) {
	for (entry = filter[hash], length = 0; entry;
	     entry = entry->smf_entry_next, length++) {
	    if (print_heading) {
		printf("\n\n Software MAC address filter on %s",
		       swidb->namestring);
		printf("\n  Hash Len    Address      Matches  Act      Type");
		print_heading = FALSE;
	    }
 	    printf("\n  0x%02x:%-3d %e %-9d %s %s", hash, length,
		   entry->smf_entry_mac_addr.byte, entry->smf_entry_hits,
		   smf_action_string(entry->smf_entry_result),
		   smf_mactyp_string(entry->smf_entry_result));
	}
    }
}

/*
 * smf_show_command
 *
 * Iterative pretty printer.
 */

void
smf_show_command (parseinfo *csb)

{
    idbtype *swidb;

    swidb = GETOBJ(idb,1);

    automore_enable(NULL);

    if (swidb)
	smf_show_filter(swidb);
    else {
	FOR_ALL_SWIDBS(swidb) {
	    smf_show_filter(swidb);
	}
    }

    automore_disable();
}
