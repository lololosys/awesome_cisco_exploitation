/* $Id: smf_private_inline.h,v 3.2 1995/11/17 18:52:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smf/smf_private_inline.h,v $
 *------------------------------------------------------------------
 * Software MAC Address Filter (SMF) private inline functions.
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
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
 * $Log: smf_private_inline.h,v $
 * Revision 3.2  1995/11/17  18:52:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/30  22:40:00  speakman
 * CSCdi41340:  Platform-independent tbridging performance enhancements
 * Create smf_match_address_inline out of smf_match_address which becomes
 * just a function wrapper for this inline.  Forgo the pointer safety
 * checks in ieee_equal and just do a direct MAC address comparison.
 *
 * Revision 2.1  1995/06/07  22:44:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * smf_locate_entry_inline
 *
 * Locate an SMF entry by MAC address and result.
 */

static inline
smf_entry_type *smf_locate_entry_inline (uchar const *mac_addr,
					 smf_entry_type **filter,
					 ulong const result)

{
    smf_entry_type *entry;

    for (entry = filter[SMF_HASH(mac_addr)]; entry;
	 entry = entry->smf_entry_next)
        if ((ieee_equal((uchar *)entry->smf_entry_mac_addr.sword, mac_addr)) &&
	    (entry->smf_entry_result == result))
	    break;

    return(entry);
}

/*
 * smf_locate_prev_entry_inline
 *
 * Locate an SMF entry and its predecessor by MAC address and result.
 */

static inline
smf_entry_type *smf_locate_prev_entry_inline (uchar const *mac_addr,
					      smf_entry_type **filter,
					      ulong const result,
					      smf_entry_type **prev_entry)

{
    smf_entry_type *entry, *prev = NULL;

    *prev_entry = NULL;

    for (entry = filter[SMF_HASH(mac_addr)]; entry;
	 entry = entry->smf_entry_next)
        if ((ieee_equal((uchar *)entry->smf_entry_mac_addr.sword, mac_addr)) &&
	    (entry->smf_entry_result == result)) {
	    *prev_entry = prev;
	    break;
	} else {
	    prev = entry;
	}

    return(entry);
}

/*
 * smf_locate_address_inline
 *
 * Locate an SMF entry based soley on MAC address.
 */

static inline
smf_entry_type *smf_locate_address_inline (uchar const *mac_addr,
					   smf_entry_type **filter)

{
    smf_entry_type *entry;
    uchar *smf_addr;

    for (entry = filter[SMF_HASH(mac_addr)]; entry;
	 entry = entry->smf_entry_next) {
	smf_addr = entry->smf_entry_mac_addr.byte;
	if ((GETLONG(&smf_addr[2]) == GETLONG(&mac_addr[2])) &&
	    (GETSHORT(&smf_addr[0]) == GETSHORT(&mac_addr[0])))
	    break;
    }

    return(entry);
}

/*
 * smf_match_address_inline
 *
 * NOTA BENE: ASSUMES INTERRUPTS DISABLED!
 *
 * Locate the first SMF entry in the hash chain that matches a
 * given MAC address, account for a hit if found, and return
 * the result value of that entry.
 *
 * This function is the work horse of the SMF used by the rest
 * of the world to filter received packets.  Those callers have
 * only the MAC address as a key so this function, unlike the
 * inline locate functions, does not take a result value as an
 * argument.  The consequence of this is that when you display
 * the SMF and see multiple instances of the same MAC address
 * inserted by different parties, only the first instance will
 * have a non-zero hit count.  The others are there more as
 * reference counters on the MAC address itself remaining in
 * the SMF.
 *
 * Note that this function does not return a boolean or the
 * SMF entry itself which is private to the SMF.  Instead it
 * returns a result value which is a composite of two
 * attributes described in smf_public.h.
 */

static inline
ulong smf_match_address_inline (uchar *mac_addr, idbtype *swidb)

{
    smf_entry_type *entry, **filter;

    filter = swidb->sfw_mac_filter;

    if (!filter)
	return(SMF_RESULT_NOT_FOUND);

    entry = smf_locate_address_inline(mac_addr, filter);

    if (!entry) {

#ifdef SMF_DEBUG	/* see the monument in smf_debug_flags.h !!! */
	if (smf_debug_miss)
	    buginf("\nSMF: missed %e on %s", mac_addr, swidb->namestring);
#endif  /* SMF_DEBUG */

	return(SMF_RESULT_NOT_FOUND);
    }

    entry->smf_entry_hits++;

#ifdef SMF_DEBUG	/* see the monument in smf_debug_flags.h !!! */
    if (smf_debug_match)
	buginf("\nSMF: matched %e on %s - %s", mac_addr,
	       swidb->namestring, smf_mactyp_string(entry->smf_entry_result));
#endif	/* SMF_DEBUG */

    return(entry->smf_entry_result);
}
