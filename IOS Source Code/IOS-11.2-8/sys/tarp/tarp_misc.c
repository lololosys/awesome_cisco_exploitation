/* $Id: tarp_misc.c,v 3.5.56.4 1996/05/17 21:03:31 asastry Exp $
 * $Source: /release/112/cvs/Xsys/tarp/tarp_misc.c,v $
 *------------------------------------------------------------------
 * TARP (Target ID Address Resolution Protocol)
 *
 * October, 1995 Arun Sastry
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tarp_misc.c,v $
 * Revision 3.5.56.4  1996/05/17  21:03:31  asastry
 * CSCdi57772:  Missed out a line in the log; add it.
 * Branch: California_branch
 *
 * Revision 3.5.56.3  1996/05/17  20:55:42  asastry
 * CSCdi57772:  Fix misc. TARP bugs found during interoperability testing
 *
 *               - Don't change NSEL during TARP packet propagation
 *               - Implement loop detection as per amended TARP spec
 *               - Don't propagate TARP packet if TTL is zero (after
 *                 decrement)
 *
 * Branch: California_branch
 *
 * Revision 3.5.56.2  1996/04/30  22:02:34  asastry
 * CSCdi56244:  TARP table can get corrupted, causing a crash.
 *
 * Branch: California_branch
 *
 * Revision 3.5.56.1  1996/03/18  22:11:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.22.2  1996/03/07  10:51:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.22.1  1996/02/20  18:52:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1995/11/28  18:58:51  asastry
 * CSCdi44538:  Configuring TARP TID causes crash if CLNS NET is missing
 *
 * Revision 3.4  1995/11/20  19:19:20  asastry
 * CSCdi43154:  TARP corrupted memory pointer. Fix pdb corruption bug. In
 * addition, incorporate these other misc. fixes:
 *
 *
 *         - Fix spurious TARP interface command during NVGEN
 *         - Reset sequence number to zero before sending TARP Type 4 PDU
 *         - Accept TARP PDU with sequence number of zero, regardless of
 *           whether there is an entry in Loop Detection Buffer
 *         - Zero out NSAP field in TARP TID cache if CLNS NET is deleted
 *           from underneath TARP
 *         - create TID entry with a zero NSAP if a TARP TID is entered
 *           before assigning a CLNS NET
 *
 * Revision 3.3  1995/11/17  18:44:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:43:20  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:33:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "../ui/debug.h"
#include "tarp_externs.h"
#include "tarp_debug.h"


/*
 * tarp_hash
 *
 * Hash an entry and return hash value. The argument 'which' specifies 
 * whether it's the TID-NET cache or the LDB cache.
 */
static int tarp_hash (uchar *entry, int length, enum TARP_TABLE_TYPE which)
{
    int hash;
    
    if (length <= 0)
	return (0);

    hash = 0;
    while (length--) {
	hash ^= *entry++;
    }
    return(hash);
}

/*
 * tarp_walk_ldb_table
 *
 * Return a pointer to the next entry in the LDB table. If entry passed in is
 * NULL, start from the first entry in the table.
 */
tarp_ldb_type *tarp_walk_ldb_table (int *hash_index, tarp_ldb_type *entry)
{
    if (entry) {
	entry = entry->next;
	if (!entry) {
	    *hash_index += 1;
	    if (*hash_index >= NETHASHLEN)
		return (NULL);
	}
    }

    while (TRUE){
	if (entry)
	    return(entry);
	while ((entry = tarp_ldb_table[*hash_index]) == 0){
	    *hash_index += 1;
	    if (*hash_index >= NETHASHLEN)
		return(NULL);
	}
    }
}

/*
 * tarp_delete_ldb_entry
 *
 * Delete given LDB entry.
 */
static void tarp_delete_ldb_entry (tarp_ldb_type *entry)
{
    entry->next = NULL;
    free(entry);
    return;
}

/*
 * tarp_clear_ldb_table
 *
 * Delete TARP LDB table
 */
void tarp_clear_ldb_table (void)
{
   int bucket;
   tarp_ldb_type *entry, *next;

   for (bucket = 0; bucket < NETHASHLEN; bucket++) {
       for (entry = tarp_ldb_table[bucket]; entry; entry = next) {
	   next = entry->next;

	   TARP_EVBUGINF("\nTARP-EV: Deleted LDB table entry for %s",
			 station_string(entry->sys_id));

	   tarp_delete_ldb_entry(entry);
       }
       tarp_ldb_table[bucket] = NULL;
   }
}

/*
 * tarp_ldb_ager
 *
 * Clean up expired entries in Loop Detection (Buffer) cache
 */
void tarp_ldb_ager (void)
{
    int bucket;
    tarp_ldb_type *entry, *next, *prev;

    for (bucket = 0; bucket < NETHASHLEN; bucket++) {
	prev = NULL;
	for (entry = tarp_ldb_table[bucket]; entry; prev = entry,
	                                     entry = next) {
	    next = entry->next;
	    if (TIMER_RUNNING_AND_AWAKE(entry->expiration_time)) {
		TARP_EVBUGINF("\nTARP-EV: Aging LDB entry for %s ",
 			      station_string(entry->sys_id));

		tarp_delete_ldb_entry(entry);

		/*
		 * Entry being deleted is first one in chain
		 */
		if (!prev)
		    tarp_ldb_table[bucket] = next;
		else
		    prev->next = next;

		entry = prev;
	    }
	}
    }
}

/*
 * tarp_find_ldb_entry
 *
 * Find an entry in the LDB cache that matches the given system ID.
 */
tarp_ldb_type *tarp_find_ldb_entry (uchar *system_id)
{
    int h;
    tarp_ldb_type *hash_entry;

    if (!system_id)
	return (NULL);

    h = tarp_hash(system_id, STATION_LENGTH, TARP_LDB_CACHE);
    for (hash_entry = tarp_ldb_table[h]; hash_entry;
	                                 hash_entry = hash_entry->next) {
	if (station_match(hash_entry->sys_id, system_id)) {
	    break;
        }
    }

    return(hash_entry);
}

/*
 * tarp_update_ldb_entry
 *
 * Update the LDB entry with new sequence number and expiration timer values.
 */
void tarp_update_ldb_entry (tarp_ldb_type *ldb, ushort sequence,
			    uint flush_timer)
{
    if (ldb) {
	ldb->sequence = sequence;
	TIMER_START(ldb->expiration_time, flush_timer);
    }

    return;
}

/*
 * tarp_insert_ldb_entry
 *
 * Insert an entry in the LDB cache. Return pointer to new entry if 
 * successful, NULL otherwise.
 */
tarp_ldb_type *tarp_insert_ldb_entry (ushort sequence, uchar *system_id) 
{
    int h;
    tarp_ldb_type *ldb;

    /*
     * Return if entry already exists.
     */
    ldb = tarp_find_ldb_entry(system_id);
    if (ldb)
	return (ldb);

    ldb = malloc(sizeof(tarp_ldb_type));
    if (!ldb) {
	tarp_traffic[TARP_NOMEM]++;
	return (NULL);
    }

    ldb->sequence = sequence;
    bcopy(system_id, ldb->sys_id, STATION_LENGTH);
    TIMER_START(ldb->expiration_time, tarp_ldb_flush_timer);

    h = tarp_hash(system_id, STATION_LENGTH, TARP_LDB_CACHE);
    ldb->next = tarp_ldb_table[h];
    tarp_ldb_table[h] = ldb;

    return (ldb);
}

/*
 * tarp_walk_tid_table
 *
 * Return a pointer to the next entry in the TID table. If entry passed in is
 * NULL, start from the first entry in the table.
 */
tarp_tid_cache_type *tarp_walk_tid_table (int *hash_index,
					  tarp_tid_cache_type *entry)
{
    if (entry) {
	entry = entry->next;
	if (!entry) {
	    *hash_index += 1;
	    if (*hash_index >= NETHASHLEN)
		return (NULL);
	}
    }

    while (TRUE){
	if (entry)
	    return(entry);
	while ((entry = tarp_tid_table[*hash_index]) == 0){
	    *hash_index += 1;
	    if (*hash_index >= NETHASHLEN)
		return(NULL);
	}
    }
}

/*
 * tarp_find_tid_entry
 *
 * Find an entry in the TID cache that matches the given information. The
 * search can be based either on the TID or the NSAP. The hash is based on
 * the TID, so if we need to search on an NSAP, we need to search the whole
 * table. Sigh, but we need to be able to search on both keys, so there's
 * no other way. Given that this isn't really time-critical, it's not a big
 * deal.
 */
tarp_tid_cache_type *tarp_find_tid_entry (uchar *entry, int length,
					  enum TARP_TID_NET_KEY which)
{
    int h;
    tarp_tid_cache_type *hash_entry;
    int index;
    uchar tid[TARP_MAX_TID_LEN];
    uchar tid_len;

    if (!entry)
	return (NULL);

    hash_entry = NULL;
    if (which == TARP_TID_KEY) {
	/*
	 * Search is based on TID.
	 * Extract TID of sender into a null terminated buffer.
	 */
	tid_len = (length < TARP_MAX_TID_LEN) ?
	                          length : (TARP_MAX_TID_LEN - 1);
	bcopy(entry, tid, tid_len);
	tid[tid_len] = '\0';

	h = tarp_hash(entry, length, TARP_TID_CACHE);
	for (hash_entry = tarp_tid_table[h]; hash_entry;
	                                     hash_entry = hash_entry->next) {
	    if (strcmp(hash_entry->tid, tid) == 0) {
		break;
	    }
	}

	return(hash_entry);
    }
    else if (which == TARP_NET_KEY) {
	/*
	 * Search based on CLNS NET.
	 */
	index = 0;
	hash_entry = tarp_walk_tid_table(&index, NULL);
	while (hash_entry) {
	    if (net_equ(hash_entry->net, entry))
		break;

	    hash_entry = tarp_walk_tid_table(&index, hash_entry);
	}
	return (hash_entry);
    }
    else
	return (NULL);
}

/*
 * tarp_update_tid_entry
 *
 * Update the TID cache entry with the new info. If the cache entry doesn't
 * exist, and the 'create' flag is TRUE, create it.
 *
 * tid : pointer to TID info, tid_len : length of TID info
 * prot_addr_type : protocol address type (most probably NSAP)
 * prot : pointer to protocol address info, prot_addr_len : length of 'prot'
 * flags : Flags to indicate entry type.
 */
void tarp_update_tid_entry (uchar *tid, ushort tid_len, uchar prot_addr_type,
			    uchar *prot, ushort prot_addr_len, boolean create,
                            uchar flags)
{
    tarp_tid_cache_type *cache_entry;
    int h;
    boolean new_entry;

    /*
     * Don't bother if the 'tarp_allow_cache' flag is FALSE.
     */
    if (!tarp_allow_cache_flag) {
	TARP_EVBUGINF("\nTARP-EV: TID cache disabled - no cache entry "
		      "created");
	return;
    }

    /*
     * Check if this entry is in our TID cache, and create it if it isn't.
     */
    cache_entry = tarp_find_tid_entry(tid, tid_len, TARP_TID_KEY);
    new_entry = (cache_entry == NULL);

    if (!cache_entry) {
	if (!create)
	    return;

	cache_entry = malloc(sizeof(tarp_tid_cache_type));
	if (!cache_entry) {
	    tarp_traffic[TARP_NOMEM]++;
	    return;
	}
	
	/*
	 * Insert new entry into hash table.
	 */
	h = tarp_hash(tid, tid_len, TARP_TID_CACHE);
	cache_entry->next = tarp_tid_table[h];
	tarp_tid_table[h] = cache_entry;
    }

    /*
     * Update cache entry. If the address is an NSAP, store it in the NSAP
     * format (i.e. first byte indicating the length of the NSAP, followed
     * by the NSAP itself). Else, just copy the address info as is. If an
     * NSAP, zero out the NSEL.
     */
    cache_entry->prot_addr_type = prot_addr_type;
    if (prot_addr_type == TARP_NSAP_PROT_ADDR_TYPE) {
	cache_entry->net[0] = prot_addr_len;
	bcopy(prot, &cache_entry->net[1], prot_addr_len);
	cache_entry->net[prot_addr_len] = 0;
    }
    else 
	bcopy(prot, cache_entry->net, prot_addr_len);

    /*
     * Free TID info and reallocate the buffer, since we don't know whether
     * the current buffer has enough space for the new info.
     */
    if (cache_entry->tid)
	free (cache_entry->tid);
    
    cache_entry->tid = malloc(tid_len + 1);
    if (!cache_entry->tid) {
	free(cache_entry);
	tarp_traffic[TARP_NOMEM]++;
	return;
    }

    /*
     * Copy new info, and null terminate, to make our lives easier.
     */
    cache_entry->flags |= flags;
    bcopy(tid, cache_entry->tid, tid_len);
    cache_entry->tid[tid_len] = '\0';
    if (tarp_pkt_debug) {
	if (new_entry)
	    buginf("\nTARP-PA: Created new %scache entry for %s",
		   tarp_entry_type(cache_entry),
		   cache_entry->tid);
	else
	    buginf("\nTARP-PA: Updated cache entry for %s",
		   cache_entry->tid);
    }

    /*
     * Our own entry and static entries don't expire. Set an arbitrary
     * value of zero. The ager will gloss over these entries.
     */
    if (flags & (TARP_STATIC_FLAG | TARP_MY_ENTRY_FLAG)) {
	TIMER_STOP(cache_entry->expiration_time);
    } else {
	TIMER_START(cache_entry->expiration_time, tarp_tid_cache_timer);
    }
    return;
}

/*
 * tarp_delete_tid_cache_entry
 *
 * Delete given TID cache entry.
 */
static void tarp_delete_tid_cache_entry (tarp_tid_cache_type *entry)
{
    if (entry->tid)
        free(entry->tid);  

    entry->next = NULL;
    free(entry);
    return;
}

/*
 * tarp_delete_one_tid_entry
 *
 * Delete a specific entry in the TARP TID cache (this routine will also
 * adjust the pointers), given the TID.
 */
void tarp_delete_one_tid_entry (uchar *tid)
{
    tarp_tid_cache_type *prev, *entry;
    boolean found;
    int h;

    h = tarp_hash(tid, strlen(tid), TARP_TID_CACHE);
    prev = NULL;

    found = FALSE;
    entry = tarp_tid_table[h];

    for ( ; entry; prev = entry, entry = entry->next) {
	if (strcmp(tid, entry->tid) != 0) {
	    continue;
	}

	/*
	 * Found the right entry (this will be pointed to by 'entry')
	 */
	TARP_EVBUGINF("\nTARP-EV: Deleting entry with TID %s", entry->tid);

	if (!prev) 
	    tarp_tid_table[h] = entry->next;
	else
	    prev->next = entry->next;	    

	/*
	 * Now delete this entry and get out of here.
	 */
	tarp_delete_tid_cache_entry(entry);
	found = TRUE;
	break;
    }    
    
    if (!found)
	TARP_EVBUGINF("\nTARP-EV: Could not find entry with TID %s "
		      "for deletion", tid);
    return;
}

/*
 * tarp_clear_tid_table
 *
 * Delete TARP TID cache. Don't delete our own entry (or static entries)
 * unless the flag 'delete_all' is TRUE.
 */
void tarp_clear_tid_table (boolean delete_all)
{
   int bucket;
   tarp_tid_cache_type *entry, *next, *prev;

   for (bucket = 0; bucket < NETHASHLEN; bucket++) {
       prev = NULL;
       for (entry = tarp_tid_table[bucket]; entry; entry = next) {
           next = entry->next;

	   if (!delete_all && (entry->flags & TARP_MY_ENTRY_FLAG)) {
	       prev = entry;
	       continue;
	   }

	   if (!delete_all && (entry->flags & TARP_STATIC_FLAG)) {
	       prev = entry;
	       continue;
	   }
	   
	   TARP_EVBUGINF("\nTARP-EV: Deleted table entry for %s",
			 entry->tid);

	   tarp_delete_tid_cache_entry(entry);
	   if (prev == NULL)
	       tarp_tid_table[bucket] = next;
	   else
	       prev->next = next;
       }
   }
}

/*
 * tarp_tid_ager
 *
 * Clean up expired entries in TID cache. We don't age out static entries 
 * or our own entry.
 */
void tarp_tid_ager (void)
{
    int bucket;
    tarp_tid_cache_type *entry, *next, *prev;

    for (bucket = 0; bucket < NETHASHLEN; bucket++) {
	prev = NULL;
	for (entry = tarp_tid_table[bucket]; entry; prev = entry,
	                                     entry = next) {
	    next = entry->next;

	    if (entry->flags & TARP_STATIC_FLAG)
		continue;

	    if (entry->flags & TARP_MY_ENTRY_FLAG)
		continue;

	    if (TIMER_RUNNING_AND_AWAKE(entry->expiration_time)) {
		TARP_EVBUGINF("\nTARP-EV: Aging TID cache entry for %s, "
			      "(NSAP %n)", entry->tid, entry->net);

		tarp_delete_tid_cache_entry(entry);

		/*
		 * Entry being deleted is first one in chain
		 */
		if (!prev)
		    tarp_tid_table[bucket] = next;
		else
		    prev->next = next;

		entry = prev;
	    }
	}
    }
}

/*
 * tarp_clean_up
 *
 * Clean up before leaving.
 */
void tarp_clean_up (void)
{
    if (tarp_my_tid) {
	free(tarp_my_tid);
	tarp_my_tid = NULL;
    }

    /*
     * Clear TARP TID-NET cache, and the LDB cache.
     */
    tarp_clear_tid_table(TRUE);
    tarp_clear_ldb_table();
}

/*
 * tarp_build_address_string
 *
 * Return source address string. If the TARP PDU contains a CLNS NSAP, return
 * buffer with an NSAP format. Else just copy the contents of the address
 * field into the buffer provided.
 */
void tarp_build_address_string (tarp_pdutype *tarp_pdu, uchar *buffer)
{
    uchar addr_len;
    
    /*
     * Protect yourself - check address length, in case the address isn't an
     * NSAP!!
     */
    addr_len = (tarp_pdu->prot_addr_len < CLNS_MAX_NSAP_LENGTH) ?
	                 tarp_pdu->prot_addr_len : CLNS_MAX_NSAP_LENGTH-1;

    /*
     * If the address contained in TARP PDU is an NSAP, build buffer
     * to look like an NSAP.
     */
    if (tarp_pdu->prot_addr_type == TARP_NSAP_PROT_ADDR_TYPE) {
	buffer[0] = addr_len;
	bcopy(SOURCE_PROT_ADDR(tarp_pdu), buffer+1, addr_len);
    }
    else {
	bcopy(SOURCE_PROT_ADDR(tarp_pdu), buffer, addr_len);
	buffer[addr_len] = '\0';
    }

    return;
}

/*
 * tarp_addr_string
 *
 * Return pointer to an NSAP string, if the TARP PDU contains a CLNS NSAP,
 * Else just return the original string. Note that it's assumed that the
 * input buffer already has the address info filled in.
 */
uchar *tarp_addr_string (tarp_pdutype *tarp_pdu, uchar *buffer)
{
    if (tarp_pdu->prot_addr_type == TARP_NSAP_PROT_ADDR_TYPE) 
	return(nsap_string(buffer));
    else
	return(buffer);
}

/*
 * tarp_build_tid_string
 *
 * Build a null-terminated TID string, given the TARP PDU and a destination
 * buffer. The parameter 'which' specifies whether we should extract the
 * source TID or destination TID.
 */
void tarp_build_tid_string (tarp_pdutype *tarp_pdu, uchar *buffer,
			    enum TARP_SRC_DST which)
{
    uchar tid_len;

    tid_len = (which == TARP_SOURCE) ? tarp_pdu->source_len :
	                               tarp_pdu->target_len;
    /*
     * Allow maximum length of TARP_MAX_TID_LEN.
     */
    tid_len = (tid_len < TARP_MAX_TID_LEN) ? tid_len : (TARP_MAX_TID_LEN - 1);
    bcopy((which == TARP_SOURCE) ? SOURCE_TID(tarp_pdu) : TARGET_TID(tarp_pdu),
	                                                    buffer, tid_len);
    buffer[tid_len] = '\0';
    
    return;
}

/*
 * tarp_extract_area
 *
 * Extract area info from given nsap into given buffer.
 */
void tarp_extract_area (uchar *nsap, uchar *buffer)
{
    uchar area_len;

    area_len = nsap[0] - (STATION_LENGTH + SELECTOR_LENGTH);
    buffer[0] = area_len;
    bcopy(&nsap[1], &buffer[1], area_len);
}

/*
 * tarp_entry_type
 *
 * Return string containing TID cache entry type.
 */
uchar *tarp_entry_type (tarp_tid_cache_type *cache_entry)
{
    switch (cache_entry->flags) {
        case TARP_STATIC_FLAG:
	    return("STATIC ");
	case TARP_MY_ENTRY_FLAG:
	    return("LOCAL ");
	default:
	    return("DYNAMIC ");
    }
}

/*
 * tarp_handle_new_net
 *
 * Handle the case when the NET for the ISIS process has changed. If 'net' is
 * NULL, it means that there is no NET associated with the ISIS process, and
 * in this case, we just leave the entry as it is. Unfortunately, the specs
 * don't say how to handle this case. If 'net' is non-NULL, then we have a
 * new NET and we update our entry, and send a Type 4 TARP PDU so that
 * all our pals know about this.
 */
static void tarp_handle_new_net (uchar *net)
{
    if (!tarp_my_tid)
	return;

    if (net) {
	TARP_EVBUGINF("\nTARP-EV: Updating our cache entry with NET "
		      "%s", nsap_string(net));
	tarp_update_tid_entry(tarp_my_tid, strlen(tarp_my_tid),
			      TARP_NSAP_PROT_ADDR_TYPE, net+1,
			      *net, TRUE, TARP_MY_ENTRY_FLAG);

	tarp_propagate_new_info(NULL, tarp_my_tid, net);
    }
    else  {
        tarp_update_tid_entry(tarp_my_tid, strlen(tarp_my_tid),
			      TARP_NSAP_PROT_ADDR_TYPE, &tarp_zero_nsap[1],
			      tarp_zero_nsap[0], TRUE, TARP_MY_ENTRY_FLAG);
	tarp_propagate_new_info(NULL, tarp_my_tid, tarp_zero_nsap);

	TARP_EVBUGINF("\nTARP-EV: ISIS NET removed");
    }
}

/*
 * tarp_check_type4_timer();
 * 
 * Check if type 4 has expired. If it has, send Type 4 PDU, and also stop
 * the Type4 timer.
 */
void tarp_check_type4_timer (void)
{
    uchar *clns_net;
    clns_pdbtype *clns_pdb;

    if (TIMER_RUNNING_AND_AWAKE(tarp_type4_timer)) {
	TARP_EVBUGINF("\nTARP-EV: Type 4 timer expired");
	tarp_get_host_status(&clns_pdb, &clns_net);
	tarp_handle_new_net(clns_net);

	TIMER_STOP(tarp_type4_timer);
	TARP_EVBUGINF("\nTARP-EV: Type 4 timer stopped");
    }

    return;
}

/*
 * tarp_start_type4_timer
 * 
 * Start type 4 timer (called by CLNS when CLNS NET is changed).
 */
void tarp_start_type4_timer (void)
{
    TIMER_START(tarp_type4_timer, TARP_SEND_TYPE4_TIMER);
    TARP_EVBUGINF("\nTARP-EV: Type 4 timer started, expiration : %d secs",
		  (TIME_LEFT_SLEEPING(tarp_type4_timer))/ONESEC);
}

/*
 * tarp_get_host_status
 *
 * Determine if this host is an IS or ES. If it's an IS and is running
 * ISIS, set pdb to be the ISIS pdb and return TARP_IS_TYPE. If ISIS
 * is not running, return TARP_NULL_HOST_STATUS. If it's an ES, return
 * TARP_ES_TYPE. Also set the 'net', if applicable.
 */
enum TARP_HOST_TYPE tarp_get_host_status (clns_pdbtype **pdb, uchar **net)
{
    clns_pdbtype *clns_pdb;
    clns_local_nettype *local_net;

    clns_pdb = find_isis_pdb();

    if (clns_pdb) {
	*pdb = clns_pdb;
	*net = clns_pdb->net[0] ? clns_pdb->net : NULL;
	return (TARP_IS_TYPE);
    }
    else {
	if (clns_router_running) {
	    /*
	     * Running as a router, but no ISIS..not interested.
	     */
	    *net = NULL;
	    *pdb = NULL;
	    return (TARP_NULL_HOST_TYPE);
	}
	else {
	    local_net = (clns_local_nettype *)clns_local_nets.qhead;     
	    *net = local_net ? local_net->net : NULL;
	    *pdb = NULL;
	    return (TARP_ES_TYPE);
	}   
    }

    return (TARP_NULL_HOST_TYPE);
}
