/* $Id: netbios_name.c,v 3.7.18.6 1996/09/11 17:38:54 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ibm/netbios_name.c,v $
 *------------------------------------------------------------------
 * NetBIOS (over LLC2 and Novell IPX) name support routines
 *
 * January 21, 1991 - Joel P. Bion
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: netbios_name.c,v $
 * Revision 3.7.18.6  1996/09/11  17:38:54  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.7.18.5  1996/08/26  15:08:21  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.7.18.4  1996/07/09  05:58:12  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.7.18.3  1996/06/01  01:31:39  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.7.18.2  1996/05/17  11:21:00  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.4.6  1996/05/05  23:08:26  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.7.4.5  1996/04/26  15:19:51  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.4.4  1996/04/26  07:54:21  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.4.3  1996/04/03  14:34:25  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.4.2  1996/03/20  17:54:16  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comment - name changes only
 *
 * Revision 3.7.4.1  1996/03/16  12:09:25  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.7.18.1  1996/03/18  20:11:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.3  1996/03/13  01:23:32  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.12.2  1996/03/07  09:42:17  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.12.1  1996/02/20  00:54:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/07  16:11:54  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.6  1996/02/01  06:03:16  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1995/12/12  06:09:28  gschleic
 * CSCdi44259:  Netbios name-cache timeout parses incorrectly
 *
 * Revision 3.4  1995/12/11  23:18:05  jacobt
 * CSCdi44533:  Proxy explorer, parser bugs and more!
 *
 * Revision 3.3  1995/12/11  10:20:15  ppearce
 * CSCdi44095:  SRB proxy-explorer causes FDDI packet drops
 *   rif_explore() should call tr_to_media_out() rather than datagram_out()
 *     so that packets can be correctly formatted for output to FDDI
 *
 *     - Rename srb_out() to tr_media_out()
 *     - Change srb_out() to invoke tr_fddi_out() for FDDI output
 *     - Move adjust_pak() code into tr_fddi_out()
 *
 * Revision 3.2  1995/11/17  09:23:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:02:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/07  05:35:06  richl
 * CSCdi38315:  makefile rework
 *              move utility routines to libibm_util.a
 *
 * Revision 2.4  1995/07/06  01:32:22  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.3  1995/06/28 09:25:41  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/21  08:54:46  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  20:47:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "ieee.h"
#include "netbios.h"
#include "netbios_sb.h"
#include "../if/tring.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "parser_defs_netbios.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "../if/network.h"
#include "../srt/srb.h"
#include "../srt/srb_core.h"
#include "../srt/rsrb.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../ui/debug.h"
#include "netbios_debug.h"
#include "subsys.h"

/*
 * Global Storage and forward references
 */
netbios_name_cache *netbios_name_cache_table[NETBIOS_HASH_LEN];
int netbios_name_cache_timeout;
int netbios_query_timeout;
int netbios_recognized_timeout;
boolean netbios_cache_add_name;
queuetype free_netbios_name_cacheQ;
boolean global_netbios_name_cache_enable;
long netbios_proxy_datagram_time;
boolean netbios_proxy_datagram;
boolean netbios_broadcast_name_recognized;


/*
 * netbios_retbuffer
 *
 * Return a buffer for NetBIOS at interrupt level
 */
static void netbios_retbuffer (paktype *pak)
{
    if (pak->if_input) {
	protocol_discard(pak, TRUE);
    } else {
	retbuffer(pak);
    }
}

/*
 * netbios_name_hash
 *
 * Returns an offset into an array of netbios names. Uses full name
 */
static int netbios_name_hash (
    uchar *name)
{
    int i, res;
    
    for (res = i = 0; i < netbios_name_length; i++) {
	res = (res + ((uchar) name[i])) % NETBIOS_HASH_LEN;
    }
    return(res);
}

/*
 * netbios_grow_request
 * Pre-initialize buffers for entries in the netbios name cache.  We must do
 * this because they cannot be malloced at interrupt level as the packets
 * arrive.
 */
static void netbios_grow_request (void)
{
    int i;
    netbios_name_cache *entry;
    leveltype level;
    
    if (free_netbios_name_cacheQ.count > NETBIOS_NAME_QUEUE_MIN) {
	return;
    }
    level = raise_interrupt_level(NETS_DISABLE);
    for (i = free_netbios_name_cacheQ.count; i < NETBIOS_NAME_QUEUE_MAX; i++) {
	entry = malloc_named(sizeof(netbios_name_cache), "NETBIOS Name Cache");
	if (!entry)
	    break;
	enqueue(&free_netbios_name_cacheQ, entry);
    }
    reset_interrupt_level(level);
}
/*--------------- Non-volatile memory NETBIOS support -------------------*/

/*
 * netbios_init_idb
 * Initialize an IDB for netbios use
 */
void netbios_init_idb (hwidbtype* idb)
{
    netbios_sb_t *netb_sb = netbios_get_or_create_sb(idb->firstsw);
    if (!netb_sb) {
      printf(nomemory);
      return;
    }
    netb_sb->name_caching_enabled = FALSE;
}

/*
 * netbios_name_cache_lookup
 * Search the NetBIOS name cache for an entry for the name given.
 */
static netbios_name_cache *netbios_name_cache_lookup (
    uchar *name,
    ushort vrn)
{
    netbios_name_cache *entry;
    int netbios_name_cache_entry;

    if (netbios_name_cache_debug)
	buginf("\nNETBIOS: L checking name %16s, vrn=%d", name, vrn);

    /*
     * Pick a bucket and search it.  
     */
    netbios_name_cache_entry = netbios_name_hash(name);
    /* paranoia code */
    entry = netbios_name_cache_table[netbios_name_cache_entry];
    if (entry && !validmem(entry)) {
	netbios_name_cache_table[netbios_name_cache_entry] = NULL;
	buginf("\nNetBIOS name cache table corrupted at offset %d",
	       netbios_name_cache_entry);
	return(NULL);
    }

    if (entry && entry->magic != NETBIOS_NAME_MAGIC) {
	netbios_name_cache_table[netbios_name_cache_entry] = NULL;
	buginf("\nNetBIOS name cache table magic corrupted at offset %d",
	       netbios_name_cache_entry);
	return(NULL);
    }

    while (entry) {
	/* more paranoia code */
	if (entry->next && !validmem(entry->next)) {
	    buginf("\nNetBIOS name cache table corrupted at later offset, at location %d",
		   netbios_name_cache_entry);
	    entry->next = NULL;
	}
	if (entry->next && entry->next->magic != NETBIOS_NAME_MAGIC) {
	    buginf("\nNetBIOS name cache table later offset magic corrupted at offset %d",
	       netbios_name_cache_entry);
	    entry->next = NULL;
	}
	if (!bcmp(name, entry->name, netbios_name_length)) {
	    return(entry);
	}
	entry = entry->next;
    }

    /*
     * Nope, not here. Punt!
     */
    return(NULL);
}

/*
 * netbios_name_cache_update
 *
 * Update the NetBIOS name cache entry for this name.  If not already
 * present, then add one.
 *
 * NETBIOS_AGED and NETBIOS_AGED_REM routes are assumed to originate from
 * outside the system.
 *
 * name		a pointer to the NetBIOS name to look up
 * addr		a pointer to a MAC address for this name.
 * flags	what kind of entry
 * idb 		interface on which information came in; could be a ring
 *              group number. Must be given for NETBIOS_AGED
 * vrn	        Virtual ring number on which frame arrived. Must be given
 *		for NETBIOS_AGED_REM
 * command      netbios command
 */

static void netbios_name_cache_update (
    uchar *name,
    uchar *addr,
    int flags,
    hwidbtype *idb,
    ushort vrn,
    uchar command)
{
    netbios_name_cache *entry;
    int hash_entry, i;
    leveltype status;

    if (netbios_name_cache_debug)
	buginf("\nNETBIOS: U chk name=%16s, addr=%e, idb=%s, vrn=%d, type=%d",
	       name, addr, ((idb) ? (idb->hw_namestring) : "N/A"), vrn, 
	       flags);

    hash_entry = netbios_name_hash(name);
    /* paranoia code */
    if (netbios_name_cache_table[hash_entry] &&
	!validmem(netbios_name_cache_table[hash_entry])) {
	netbios_name_cache_table[hash_entry] = NULL;
	buginf("\nNETBIOS: Corrupted NetBIOS cache table at offset %d",
	       hash_entry);
    }
    if (netbios_name_cache_table[hash_entry] &&
	netbios_name_cache_table[hash_entry]->magic != NETBIOS_NAME_MAGIC) {
	netbios_name_cache_table[hash_entry] = NULL;
	buginf("\nNETBIOS: Corrupted NetBIOS cache table at offset %d",
	       hash_entry);
    }

    for (entry = netbios_name_cache_table[hash_entry]; entry;
	 entry = entry->next) {
	/* more paranoia code */
	if (entry->next && !validmem(entry->next)) {
	    entry->next = NULL;
	    buginf("\nNETBIOS: Corrupted NetBIOS cache table at offset %d",
		   hash_entry);
	}
	if (entry->next && entry->next->magic != NETBIOS_NAME_MAGIC) {
	    entry->next = NULL;
	    buginf("\nNETBIOS: Corrupted NetBIOS cache table at offset %d",
		   hash_entry);
	}
	if (!bcmp(name, entry->name, netbios_name_length)) {
	    break;
	}
    }

    if (entry) {
	if (TIMER_RUNNING(entry->query_tick) &&
	    (command == NETBIOS_NAME_RECOGNIZED)) {
	    TIMER_STOP(entry->query_tick);
	}
	switch (flags) {
	  case NETBIOS_NAME_HANDCRAFT:
  	  case NETBIOS_NAME_HANDCRAFT_REM:
	    entry->flags = flags;
	    ieee_copy(addr, entry->addr);
	    GET_TIMESTAMP(entry->time);
	    entry->idb = idb;
	    entry->vrn = vrn;
	    return;
	
	  case NETBIOS_NAME_AGED:
	  case NETBIOS_NAME_AGED_REM:
            /*
             * If this is a static entry, there isn't anything to do.
             * CSCdi13032 & CSCdi13033
             */
            if (entry->flags &
                (NETBIOS_NAME_HANDCRAFT | NETBIOS_NAME_HANDCRAFT_REM))
                return;

	    if (SLEEPING(entry->hold_down_time))
		return; /* no update if hold down time is not expired */
	    if (ieee_equal_sans_rii(addr, entry->addr)) {
		GET_TIMESTAMP(entry->time);
	    }
	    entry->flags = flags;
	    entry->idb = idb;
	    TIMER_START(entry->hold_down_time, NETBIOS_HOLD_DOWN_TIME);
	    entry->vrn = vrn;
	    ieee_copy(addr, entry->addr);
	    entry->addr[0] &= ~TR_RII;
	    return;
	}
	if (netbios_name_cache_debug)
	    buginf("\nNETBIOS: U upd name=%16s,addr=%e,idb=%s,vrn=%d,type=%d",
		   entry->name, entry->addr,
		   ((entry->idb) ? entry->idb->hw_namestring :"N/A"), 
		   vrn, flags);
    }

    /*
     * Nope, add one.
     */
    if (free_netbios_name_cacheQ.count < NETBIOS_NAME_QUEUE_MIN) {
	critical_background_wake();
    }

    entry = dequeue(&free_netbios_name_cacheQ);
    if (entry) {
	entry->magic       = NETBIOS_NAME_MAGIC;
	TIMER_STOP(entry->name_recog_time);
	entry->netb_saved = 0;
	entry->flags       = flags;
	ieee_copy(addr, entry->addr);
	entry->addr[0] &= ~TR_RII;
	bcopy(name, entry->name, netbios_name_length);
	for (i = netbios_name_length; i < NETBIOS_MAX_NAME_LEN; i++)
	    entry->name[i] = ' ';
	entry->idb = idb;
	entry->vrn = vrn;
	entry->name[NETBIOS_MAX_NAME_LEN] = '\0';
	GET_TIMESTAMP(entry->time);
	TIMER_STOP(entry->query_tick);
	TIMER_START(entry->hold_down_time, NETBIOS_HOLD_DOWN_TIME);
	status = raise_interrupt_level(NETS_DISABLE);
	entry->next        = netbios_name_cache_table[hash_entry];
	netbios_name_cache_table[hash_entry] = entry;
	reset_interrupt_level(status);
	if (netbios_name_cache_debug)
	    buginf("\nNETBIOS: U add name=%16s,addr=%e,idb=%s,vrn=%d,type %d",
		   entry->name, entry->addr,
		   ((entry->idb) ? (entry->idb->hw_namestring) : "N/A"),
		   entry->vrn, entry->flags);
	return;
    }

    if (netbios_name_cache_debug)
	buginf("\nNETBIOS: U no memory to add cache entry. name=%16s,addr=%e",
	       name, addr);
    return;
}

/*
 * netbios_name_cache_update_pak
 * Update the name cache from the packet given, which is known to be NetBIOS
 * but nothing more. This routine may decide to do nothing
 */
void netbios_name_cache_update_pak (uchar *dgstart, int flags,
				    hwidbtype *idb, ushort vrn)
{
    netbios_header *nbh;
    int orl;
    trrif_t *trh = (trrif_t *) dgstart;
    uchar src_copy[IEEEBYTES];
    sap_hdr *llc1;

    if (trh->saddr[0] & TR_RII)
	orl = trh->rc_blth & RC_LTHMASK;
    else
	orl = 0;

    llc1 = (sap_hdr *)((uchar *)&trh->rc_blth + orl);
    if (((llc1->control & ~LLC1_P) != LLC1_UI) ||
	(llc1->dsap != SAP_NETBIOS)) {
	return;
    }

    nbh = (netbios_header *) (dgstart + TRING_ENCAPBYTES + ET_SAP_LEN + orl);
    if ((nbh->delimiter[0] != NETBIOS_DELIMITER_0) ||
	(nbh->delimiter[1] != NETBIOS_DELIMITER_1) ||
	((nbh->command != NETBIOS_NAME_RECOGNIZED) &&
	 (nbh->command != NETBIOS_ADD_NAME_QUERY) &&
	 (nbh->command != NETBIOS_NAME_QUERY)) ||
	((nbh->command == NETBIOS_ADD_NAME_QUERY) &&
	 (!netbios_cache_add_name))) {
	return;
    }
    /* Check for the invalid netbios name */
    if (!nbh->srcname[0] || (nbh->srcname[0] == '*'))
	return;
    ieee_copy(trh->saddr, src_copy);
    src_copy[0] &= ~TR_RII;
    netbios_name_cache_update(nbh->srcname, src_copy, flags, idb, vrn, nbh->command);
}

/*
 * netbios_flush_cache
 * Support EXEC command to clear the netbios name cache
 */

void netbios_flush_cache (parseinfo *csb)
{
    netbios_name_cache_ager(TRUE, FALSE);
}

/*
 * netbios_age_cache
 * Called by SERVICE_ONEMIN to age the name cache
 */

void netbios_age_cache (void)
{
    netbios_name_cache_ager(FALSE, FALSE);
}

/*
 * netbios_name_cache_ager
 * Timeout old entries or flush NetBIOS name cache entries.
 */

void netbios_name_cache_ager (boolean flushflag, boolean remotesonly)
{
    int i;
    netbios_name_cache *ptr, *pred;
    leveltype status;
    riftype *rif_ptr;

    status = raise_interrupt_level(NETS_DISABLE);
    for (i = 0; i < NETBIOS_HASH_LEN; i++) {
	pred = (netbios_name_cache *)
	    &netbios_name_cache_table[i]; /* assumes next is a zero offset */
	ptr = pred->next;   /* really "ptr = netbios_name_cache_table[i]" */
	/* paranoia code */
	if (ptr && !validmem(ptr)) {
	    buginf("\nNETBIOS: Invalid structure detected in netbios_name_cache_ager");
	    netbios_name_cache_table[i] = NULL;
	    continue;
	}
	if (ptr && ptr->magic != NETBIOS_NAME_MAGIC) {
	    buginf("\nNETBIOS: Invalid structure detected in netbios_name_cache_ager");
	    netbios_name_cache_table[i] = NULL;
	    continue;
	}
	while (ptr) {
	    /* more paranoia code */
	    if (ptr && !validmem(ptr)) {
		buginf("\nNETBIOS: Invalid structure detected in netbios_name_cache_ager");
		pred->next = NULL;
		break;
	    }
	    if (ptr && ptr->magic != NETBIOS_NAME_MAGIC) {
		buginf("\nNETBIOS: Invalid structure detected in netbios_name_cache_ager");
		pred->next = NULL;
		break;
	    }
	    switch (ptr->flags) {
	      case NETBIOS_NAME_AGED:
	      case NETBIOS_NAME_AGED_REM:
		if ((!remotesonly) || (ptr->flags == NETBIOS_NAME_AGED_REM)) {
		    if (flushflag || CLOCK_OUTSIDE_INTERVAL(ptr->time,
					       netbios_name_cache_timeout)) {
			if (netbios_name_cache_debug)
			    buginf("\nNETBIOS: %s name=%16s, addr=%e",
				   (flushflag ? "flushed" : "expired"),
				   ptr->name, ptr->addr);
			rif_ptr = rif_lookup(ptr->addr, ptr->idb, NULL, 0);
			if (rif_ptr != NULL) {
			    rif_ptr->netb_saved = 0;
			    GET_TIMESTAMP(rif_ptr->add_name_query_time);
			    rif_ptr->add_name_query_correl = 0;
			}
			pred->next = ptr->next;	/* link out current entry */
			ptr->next = NULL;
			enqueue(&free_netbios_name_cacheQ, ptr);
			ptr = pred;		/* point to predecessor */
		    }
		}
		break;
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
 * netbios_if_state_change
 *
 * This routine will be called any time an interface has a state change.
 * Check to see if it is a change might should netbios, and if so, effect
 * the necessary changes.
 */
static void netbios_if_state_change (hwidbtype *hwidb)
{
    if (!is_srb_hw(hwidb))
	return;

    if (hwidb->state != IDBS_UP)
	netbios_name_cache_ager(TRUE, FALSE); /* unlearn NetBIOS names */
}

/*
 * display_netbios_name_cache
 * Print out contents of the NetBIOS name cache hash table
 */

static const char netbios_name_cache_head[] =
  "    HW Addr    Name                How      Idle      NetBIOS Packet Savings";

void display_netbios_cache (parseinfo *csb)
{
    int hash_entry, i;
    netbios_name_cache *ptr;
    boolean printed = FALSE;
    riftype *rif_ptr;
    int extra_spaces;
    char tmp_char;

    automore_enable(NULL);
    if (netbios_name_cache_timeout != NETBIOS_DEFAULT_CACHE_TIMEOUT) {
	printf("\nnetbios name-cache timeout %d minutes",
	       netbios_name_cache_timeout/ONEMIN);
	printed = TRUE;
    }
    if (netbios_query_timeout != NETBIOS_DEFAULT_QUERY_TIMEOUT) {
        printf("\nnetbios name-cache query-timeout %d seconds",
	       netbios_query_timeout);
	printed = TRUE;
    }
    if (netbios_cache_add_name) {
        printf("\nCaching information from ADD_NAME frames");
	printed = TRUE;
    }
    if (netbios_recognized_timeout != NETBIOS_DEFAULT_RECOGNIZED_TIMEOUT) {
        printf("\nnetbios name-cache recognized-timeout %d seconds",
	       netbios_recognized_timeout);
	printed = TRUE;
    }
    if (printed) {
	printf("\n");
	printed = FALSE;
    }
    for (hash_entry = 0; hash_entry < NETBIOS_HASH_LEN;
	 hash_entry++) {
	ptr = netbios_name_cache_table[hash_entry];
	while (ptr) {
	    extra_spaces = 4;
	    if (printed == FALSE) {
		printf("\nCode: \"-\" indicates a static entry\n");
		automore_header(netbios_name_cache_head);
		printed = TRUE;
	    }
	    printf("\n%e ", ptr->addr);
	    for (i = 0; i < NETBIOS_MAX_NAME_LEN; i++) {
		tmp_char = ptr->name[i];
		if (tmp_char < ' ') {
		    extra_spaces--;
		    tmp_char = (char) (((int) tmp_char) + ((int) '@'));
		    printf("^");
		}
		printf("%c", tmp_char);
	    }
	    for (i = 0; i < extra_spaces; i++) {
		printf(" ");
	    }
	    if (ptr->idb) {
		hwidbtype *hwidb = ptr->idb;
		if ((hwidb->status & IDB_VIRTUAL) &&
		    (RUNNING_IF(LINK_BRIDGE, hwidb->firstsw))) {
		    printf("BG%2d     ", idb_sw_get_spanindex(hwidb->firstsw));
		} else {
		    printf("%9s", hwidb->hw_short_namestring);
		}
	    } else if (ptr->vrn) {
		printf("rg%5d  ", ptr->vrn);
	    } else
		printf("--       ");
	    if (ptr->flags & (NETBIOS_NAME_AGED_REM | NETBIOS_NAME_AGED))
		printf("%9ld ", ELAPSED_TIME(ptr->time)/ONEMIN);
	    else if (ptr->flags & (NETBIOS_NAME_HANDCRAFT |
				   NETBIOS_NAME_HANDCRAFT_REM))
		printf("-         ");
	    rif_ptr = rif_lookup(ptr->addr, ptr->idb, NULL, 0);
	    printf("%17ld", ptr->netb_saved +
		   (rif_ptr ? rif_ptr->netb_saved : 0));
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
 * unset_netbios_name(name)
 * Remove a particular NetBIOS name entry.  Used by "no netbios ..."
 */
static boolean unset_netbios_name (
    uchar *name)
{
    uint bucket;
    netbios_name_cache *ptr;
    netbios_name_cache *pred;
    leveltype status;

    /*
     * See if address is in the table
     */
    bucket = netbios_name_hash(name);		/* determine hash bucket */
    status = raise_interrupt_level(NETS_DISABLE);
    ptr = netbios_name_cache_table[bucket];	/* set pointer to hash chain */

    /* paranoia code */
    if (ptr && !validmem(ptr)) {
	buginf("\nNETBIOS: Invalid structure detected in unset_netbios_name");
	netbios_name_cache_table[bucket] = NULL;
	reset_interrupt_level(status);
	return(FALSE);
    }
    if (ptr && ptr->magic != NETBIOS_NAME_MAGIC) {
	buginf("\nNETBIOS: Invalid structure detected in unset_netbios_name");
	netbios_name_cache_table[bucket] = NULL;
	reset_interrupt_level(status);
	return(FALSE);
    }
    pred = (netbios_name_cache *) &netbios_name_cache_table[bucket];
    while (ptr) {
	/* more paranoia code */
	if (ptr && !validmem(ptr)) {
	    pred->next = NULL;
	    reset_interrupt_level(status);
	    buginf("\nNETBIOS: Invalid structure detected in unset_netbios_name");
	    return(FALSE);
	}
	if (ptr && ptr->magic != NETBIOS_NAME_MAGIC) {
	    pred->next = NULL;
	    reset_interrupt_level(status);
	    buginf("\nNETBIOS: Invalid structure detected in unset_netbios_name");
	    return(FALSE);
	}
	if (!bcmp(name, ptr->name, NETBIOS_MAX_NAME_LEN)) {
	    if (netbios_name_cache_debug) {
		buginf("\nNETBIOS: removing entry. name=%16s,addr=%e,idb=%s,vrn=%d",
		       ptr->name, ptr->addr,
		       ((ptr->idb) ? (ptr->idb->hw_namestring) : "N/A"),
		       ptr->vrn);
	    }
	    pred->next = ptr->next;	/* link out current entry */
	    ptr->next = NULL;
	    enqueue(&free_netbios_name_cacheQ, ptr);
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
 * netbios_name_cache_command
 *
 * Parse static netBIOS name cache entries.
 */
void netbios_name_cache_command (parseinfo *csb)
{
    int netbstrlen, bucket, i, j;
    uchar *chp;
    uchar tringaddress[IEEEBYTES];
    char name_string[NETBIOS_MAX_NAME_LEN];
    leveltype level;

    if (!bridge_enable) {
	if (!csb->nvgen) {
	    printf("\nNETBIOS commands not supported in this release");
	}
	return;
    }

    if (csb->nvgen) {
	register netbios_name_cache *ptr;
	boolean first = FALSE;

	switch (csb->which) {
	  case NETBIOS_NAMECACHE_TIMEOUT:
	    nv_write(netbios_name_cache_timeout != NETBIOS_DEFAULT_CACHE_TIMEOUT,
		     "%s %d", csb->nv_command,
		     netbios_name_cache_timeout/ONEMIN);
	    break;

	  case NETBIOS_BROADCAST_NAME_RECOGNIZED:
	    nv_write(netbios_broadcast_name_recognized,
		     "%s", csb->nv_command);
	    break;

	  case NETBIOS_PROXY_DATAGRAM:
	    nv_write(netbios_proxy_datagram,
		     "%s %d", csb->nv_command,
		     netbios_proxy_datagram_time/ONESEC);
	    break;

	  case NETBIOS_NAME_LENGTH:
	    nv_write(netbios_name_length != NETBIOS_NAME_LENGTH_DEFAULT,
		     "%s %d", csb->nv_command,
		     netbios_name_length);
	    break;

	  case NETBIOS_NAMECACHE_QUERYTIME:
	    nv_write(netbios_query_timeout != NETBIOS_DEFAULT_QUERY_TIMEOUT,
		     "%s %d", csb->nv_command, netbios_query_timeout);
	    break;

	  case NETBIOS_NAMECACHE_CACHE_ADD_NAME:
	    nv_write(netbios_cache_add_name, "%s", csb->nv_command);
	    break;

	  case NETBIOS_NAMECACHE_RECOGTIME:
	    nv_write(netbios_recognized_timeout != NETBIOS_DEFAULT_RECOGNIZED_TIMEOUT,
		     "%s %d", csb->nv_command,
		     netbios_recognized_timeout);
	    break;

	  case NETBIOS_NAMECACHE_NAME:
	    level = raise_interrupt_level(NETS_DISABLE);
	    for (i = 0; i < NETBIOS_HASH_LEN; i++) {
		for (ptr = netbios_name_cache_table[i]; ptr; ptr = ptr->next) {
		    if (!(ptr->flags & (NETBIOS_NAME_HANDCRAFT_REM |
					NETBIOS_NAME_HANDCRAFT)))
		    continue;
		    if (!first) {
			nv_write(TRUE, "!");
			first = TRUE;
		    }
		    for (j = (NETBIOS_MAX_NAME_LEN - 1); j >= 0; j--) {
			if (ptr->name[j] != ' ')
			break;
		    }
		    if (j >= 0) {
			j++;
			nv_write(TRUE, "%s %e %*s", csb->nv_command, ptr->addr,
				 j, ptr->name);
			if (ptr->flags & NETBIOS_NAME_HANDCRAFT_REM)
			nv_add(TRUE, " ring-group %d", ptr->vrn);
			else if (ptr->idb)
			nv_add(TRUE, " %s", ptr->idb->hw_namestring);
		    }
		}
	    }
	    reset_interrupt_level(level);
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case NETBIOS_NAMECACHE_TIMEOUT:
	if (!csb->sense)
	    netbios_name_cache_timeout = NETBIOS_DEFAULT_CACHE_TIMEOUT;
	else {
	    netbios_name_cache_timeout = GETOBJ(int,1) * ONEMIN;
	}
	break;

      case NETBIOS_BROADCAST_NAME_RECOGNIZED:
	if (!csb->sense)
	    netbios_broadcast_name_recognized = FALSE;
	else {
	    netbios_broadcast_name_recognized = TRUE;
	}
	break;

      case NETBIOS_PROXY_DATAGRAM:
	if (!csb->sense)
	    netbios_proxy_datagram = FALSE;
	else {
	    netbios_proxy_datagram = TRUE;
	    netbios_proxy_datagram_time = GETOBJ(int,1) * ONESEC;
	}
	break;

      case NETBIOS_NAME_LENGTH:
	if (!csb->sense)
	    netbios_name_length = NETBIOS_NAME_LENGTH_DEFAULT;
	else {
	    netbios_name_length = GETOBJ(int,1);
	}
	break;

      case NETBIOS_NAMECACHE_CACHE_ADD_NAME:
	netbios_cache_add_name = csb->sense;
	break;

      case NETBIOS_NAMECACHE_QUERYTIME:
	if (!csb->sense)
	    netbios_query_timeout = NETBIOS_DEFAULT_QUERY_TIMEOUT;
	else {
	    netbios_query_timeout = GETOBJ(int,1);
	}
	break;

      case NETBIOS_NAMECACHE_RECOGTIME:
	if (!csb->sense)
	    netbios_recognized_timeout = NETBIOS_DEFAULT_RECOGNIZED_TIMEOUT;
	else {
	    netbios_recognized_timeout = GETOBJ(int,1);
	}
	break;

      case NETBIOS_NAMECACHE_NAME:
	ieee_copy(GETOBJ(hwaddr,1)->addr, tringaddress);
	bcopy(GETOBJ(string,1), name_string, NETBIOS_MAX_NAME_LEN);
	netbstrlen = strlen(name_string);

	for (chp = (uchar *) name_string, i = 0; i < NETBIOS_MAX_NAME_LEN; i++)
	    chp[i] = ((i < netbstrlen) ? name_string[i] : ' ');
    
	if (netbstrlen > NETBIOS_MAX_NAME_LEN) {
	    printf("\nNETBIOS pattern length too long -- maximum is %d",
		   NETBIOS_MAX_NAME_LEN);
	    return;
	}

	bucket = netbios_name_hash(name_string);

	if (!csb->sense) {
	    /*
	    * no netbios-name-cache etc...
	    */
	    if (!unset_netbios_name(name_string)) {
		printf("\nnetbios name-cache: cannot remove entry");
	    }
	    return;
	}
	if (GETOBJ(idb,1)) {
	    netbios_name_cache_update(name_string, tringaddress,
				      NETBIOS_NAME_HANDCRAFT,
				      GETOBJ(idb,1)->hwptr, 0, 0);
	    return;
	}
	if (!GETOBJ(int,1)) {
	    netbios_name_cache_update(name_string, tringaddress,
				      NETBIOS_NAME_HANDCRAFT, NULL, 0, 0);
	    return;
	} else {
	    netbios_name_cache_update(name_string, tringaddress,
				      NETBIOS_NAME_HANDCRAFT_REM, NULL,
				      (ushort) GETOBJ(int,1), 0);
	    return;
	}

	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
      }
}

/*
 * netbios_flush_interface(idb)
 *
 * Remove all NetBIOS entries that go through a particular interface.  This is
 * called when an interface is reset.
 */

static void netbios_flush_interface (hwidbtype *idb)
{
    int i;
    netbios_name_cache *ptr, *pred;
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);
    for (i = 0; i < NETBIOS_HASH_LEN; i++) {
	/* assumes next is a zero offset */
	pred = (netbios_name_cache *) &netbios_name_cache_table[i];
	/* really "ptr = netbios_name_cache_table[i]" */
	ptr = pred->next;		 
	/* paranoia code */
	if (ptr && !validmem(ptr)) {
	    netbios_name_cache_table[i] = NULL;
	    buginf("\nNETBIOS: Invalid structure detected in netbios_flush_interface");
	    continue;
	}
	if (ptr && ptr->magic != NETBIOS_NAME_MAGIC) {
	    netbios_name_cache_table[i] = NULL;
	    buginf("\nNETBIOS: Invalid structure detected in netbios_flush_interface");
	    continue;
	}
	while (ptr) {
	    /* more paranoia code */
	    if (ptr && !validmem(ptr)) {
		pred->next = NULL;
		buginf("\nNETBIOS: Invalid structure detected in netbios_flush_interface");
		break;
	    }
	    if (ptr && ptr->magic != NETBIOS_NAME_MAGIC) {
		pred->next = NULL;
		buginf("\nNETBIOS: Invalid structure detected in netbios_flush_interface");
		break;
	    }
	    switch (ptr->flags) {
	      case NETBIOS_NAME_AGED:
	      case NETBIOS_NAME_AGED_REM:
		if (ptr->idb == idb) {
		    if (netbios_name_cache_debug)
		      buginf("\nNETBIOS: interface flushed name=%16s,addr=%e on %s",
			     ptr->name, ptr->addr, idb->hw_namestring);
		    pred->next = ptr->next;	/* link out current entry */
		    ptr->next = NULL;
		    enqueue(&free_netbios_name_cacheQ, ptr);
		    ptr = pred;			/* point to predecessor */
		}
		break;
	      case NETBIOS_NAME_HANDCRAFT:
	      case NETBIOS_NAME_HANDCRAFT_REM:
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
 * netbios_name_cache_proxy
 *
 * Replies to a NetBIOS name request as if it were the host.
 *
 * This code assumes that it is only called by bridging code.  Proxies are
 * *not* done for the following conditions:
 *
 *       1) no RIF.  (rejected by bridge code or its for us (multi or direct)
 *       2) directed at us.  (for us, handled by local code)
 *
 * In the bridge section from which this code is called the above conditions
 * are always valid.
 *
 * Returns TRUE if frame was swallowed here; FALSE if it should continue on.
 */
boolean netbios_name_cache_proxy (paktype *pak)
{
    trrif_t *trh;
    int orl;
    sap_hdr *llc1;
    netbios_name_cache *cacheptr = NULL;
    riftype *rif_cacheptr;
    netbios_header *nbh;
    paktype *newpak;
    uchar addr[IEEEBYTES];

    /*
     * Do we want to do name-caching work? If not, let the packet continue on
     */
    if (!netbios_sb_name_caching_is_enabled(pak->if_input->hwptr->firstsw)) {
	return(FALSE);
    }
	
    trh = (trrif_t *) pak->datagramstart;
    if (trh->saddr[0] & TR_RII)
	orl = trh->rc_blth & RC_LTHMASK;
    else {
	return(FALSE);
    }
    if ((trh->rc_blth != RC_NORD_LTH) && /* no kludge and not explorer! */
	((trh->rc_blth & RC_BROADBIT) == 0))
	return(FALSE);

    llc1 = (sap_hdr *)((uchar *) trh + TRING_ENCAPBYTES + orl);
    if (((llc1->control & ~LLC1_P) != LLC1_UI) ||
	(llc1->dsap != SAP_NETBIOS)) {
	return(FALSE);
    }
    nbh = (netbios_header *) (pak->datagramstart + TRING_ENCAPBYTES +
			      ET_SAP_LEN + orl);

    /* data2[1] refers to the session number, which must be zero */
    if ((nbh->delimiter[0] != NETBIOS_DELIMITER_0) ||
	(nbh->delimiter[1] != NETBIOS_DELIMITER_1) ||
	((nbh->command != NETBIOS_NAME_QUERY) &&
	 (nbh->command != NETBIOS_ADD_GROUP_QUERY) &&
	 (nbh->command != NETBIOS_ADD_NAME_QUERY) &&
	 (nbh->command != NETBIOS_STATUS_QUERY) &&
	 (nbh->command != NETBIOS_NAME_RECOGNIZED) &&
	 (nbh->command != NETBIOS_DATAGRAM)) ||
	((nbh->data2[1]) && (nbh->command != NETBIOS_STATUS_QUERY))) {
	return(FALSE);
    }

    if ((nbh->command == NETBIOS_NAME_RECOGNIZED) &&
	(netbios_broadcast_name_recognized == TRUE)) {
	return(FALSE);
    }
    if ((nbh->command == NETBIOS_DATAGRAM) && !netbios_proxy_datagram)
	return(FALSE);

    if ((nbh->command == NETBIOS_ADD_NAME_QUERY) ||
	(nbh->command == NETBIOS_NAME_QUERY) ||
	(nbh->command == NETBIOS_ADD_GROUP_QUERY) ||
	(nbh->command == NETBIOS_STATUS_QUERY)) {
        ieee_copy(trh->saddr, addr);
        addr[0] &= ~TR_RII;
        rif_cacheptr = rif_lookup(addr, pak->if_input->hwptr, NULL, 0);
	/* Not there? Oh well... */
	if (!rif_cacheptr) {
	    if (nbh->command != NETBIOS_NAME_QUERY)
	      return(FALSE);
	} else {
	    if (TIMER_RUNNING_AND_SLEEPING(rif_cacheptr->add_name_query_time) &&
		(GETSHORT(&(nbh->rsp_correlator)) ==
		 rif_cacheptr->add_name_query_correl) &&
		((nbh->command != NETBIOS_NAME_QUERY) ||
		 (!bcmp(nbh->destname, rif_cacheptr->nq_name,
			netbios_name_length)))) {

                if ((nbh->command == NETBIOS_NAME_QUERY) &&
		      (cacheptr = netbios_name_cache_lookup(nbh->destname, 0)) &&
                      TIMER_RUNNING_AND_AWAKE(cacheptr->query_tick)) {
                    if (netbios_name_cache_debug) {
	               buginf("\nNETBIOS: Cache path is not valid, delete the entry .%s.",
                                 cacheptr->name);
	            }
	            unset_netbios_name(cacheptr->name);
	            return(FALSE);
		}
		rif_cacheptr->netb_saved++;
		netbios_retbuffer(pak);
		if (netbios_name_cache_debug) {
		    buginf("\nNETBIOS: Tossing ADD_NAME/STATUS/NAME/ADD_GROUP frame!");
		}
		return(TRUE);
	    } else {
		TIMER_START(rif_cacheptr->add_name_query_time,
			    netbios_query_timeout * ONESEC);
		rif_cacheptr->add_name_query_correl =
		  GETSHORT(&(nbh->rsp_correlator));
		if (nbh->command != NETBIOS_NAME_QUERY)
		  return(FALSE);
		else
		  bcopy(nbh->destname, rif_cacheptr->nq_name,
			netbios_name_length);
	    }
	}
    }

    if (!cacheptr &&
                !(cacheptr = netbios_name_cache_lookup(nbh->destname, 0))) {
        if (netbios_name_cache_debug) {
	    buginf("\nNETBIOS: Lookup Failed -- not in cache!");
	}
	return(FALSE);
    }

    /*
     * Here's where we do some sanity filtering on these NAME_RECOGNIZED 
     * frames. Don't allow more than allowed rate through.
     */
#ifdef JPB_DONT_WANT_THIS
    if (nbh->command == NETBIOS_NAME_RECOGNIZED) {
        if (TIMER_RUNNING_AND_SLEEPING(cacheptr->name_recog_time) &&
	    (!bcmp(nbh->srcname, cacheptr->nr_name, NETBIOS_MAX_NAME_LEN))) {
	    netbios_retbuffer(pak);
	    if (netbios_name_cache_debug) {
	        buginf("\nNETBIOS: Tossing closely-spaced NAME_RECOGNIZED frame!");
	    }
	    cacheptr->netb_saved++;
	    return(TRUE);
	} else {
	    TIMER_START(cacheptr->name_recog_time,
			netbios_recognized_timeout * ONESEC);
	    bcopy(nbh->srcname, cacheptr->nr_name, NETBIOS_MAX_NAME_LEN);
	}
    }
#endif

    if ((nbh->command == NETBIOS_DATAGRAM) && 
	(!netbios_proxy_datagram || !SLEEPING(cacheptr->datagram_tick))) {
        if (netbios_proxy_datagram)
	    TIMER_START(cacheptr->datagram_tick, netbios_proxy_datagram_time);
	return(FALSE);
    }

    /*
     * Do we have the thing in our route cache? If not, punt...
     */
    rif_cacheptr = rif_lookup(cacheptr->addr, cacheptr->idb, NULL, 0);
    if (!rif_cacheptr || (rif_cacheptr->length == 0)) {
	if (netbios_name_cache_debug) 
	    buginf("\nNETBIOS: Could not proxy packet");
	return(FALSE);
    }

    /*
     * Only respond for those not on this ring!  We've already
     * checked to see if we should be doing proxies.  If so then
     * we eat proxies from this ring for nodes on this ring.
     */
    if (rif_cacheptr->idb && pak->if_input->hwptr == rif_cacheptr->idb) {
	netbios_retbuffer(pak);
        if (netbios_name_cache_debug) {
	    buginf("\nNETBIOS: Lookup Worked, but split horizon failed!");
	}
	return(TRUE);
    }

    if ((nbh->command == NETBIOS_DATAGRAM) && 
	(!netbios_proxy_datagram || !SLEEPING(cacheptr->datagram_tick))) {
        if (netbios_proxy_datagram)
	    TIMER_START(cacheptr->datagram_tick, netbios_proxy_datagram_time);
	return(FALSE);
    }

    /*
     * Do we have the thing in our route cache? If not, punt...
     */
    rif_cacheptr = rif_lookup(cacheptr->addr, cacheptr->idb, NULL, 0);
    if (!rif_cacheptr) {
	if (netbios_name_cache_debug) {
	    buginf("\nNETBIOS: Could not find RIF entry");
	}
	return(FALSE);
    }

    /*
     * Okay. Now we think we can respond to this. We must do the following,
     * as we depend on the proxy explorer code to reply for this guy correctly
     * (it is "mega stupid" to put RIFs for the entries in the NetBIOS name
     * cache if the rif cache already tries to keep track of those...)
     *
     * Essentially, the frame comes in, we build up a NetBIOS response in a
     * new frame, and see if srb_proxy can "forward it". If it cannot, then
     * we must pass our packet on. But since we must modify the packet to
     * build a NetBIOS response, we have to keep the original to allow it to
     * be forwarded...
     */
    if ((newpak = pak_duplicate(pak)) == NULL) {
	if (netbios_name_cache_debug)
	  buginf("\nNETBIOS: Cannot duplicate packet in netbios_name_cache_prox!");
	  return(FALSE);
    }

    /*
     * The packet conversion that is needed for these broadcasts is as follows:
     *
     *       NetBIOS Frame at the point:
     *          ac/fc
     *          broadcast daddr
     *	        saddr of find_name request
     *	        RIF so far of trying to reach it.
     *		...rest of frame...
     *
     *       Make it into this new frame:
     *	        ac/fc
     *	        daddr from cacheptr->addr.
     *	        saddr of find_name request
     *          new rif to get specifically to the destination from the source
     *		...rest of frame...
     *
     * 	     Then tell vrforward_pak to forward it along as a "regular frame"!
     *
     */
    bcopy(cacheptr->addr, ((trrif_t *) newpak->datagramstart)->daddr,
	  IEEEBYTES);
    if (!rif_merge_rifs(pak, newpak, rif_cacheptr)) {
	netbios_retbuffer(newpak);
	return(FALSE);
    }
    /* You must do different things if two port SRB versus all others... */
    if (pak->if_input->hwptr->tr_bridge_idb) {
	newpak->if_output = pak->if_input->hwptr->tr_bridge_idb->firstsw;
	newpak->linktype = LINK_RSRB;
	/*
	 * spanning and all routes explorer processed here
	 * newpack is changed into SRF by rif_merge_rifs, so check
	 * explorer status in (presumably ;) unmodified version.
	 * Within if, use 'newpak' as it's this copy which is sent
	 * out the interface...
	 */
	if (isRifExplorerPak(pak)) 
	{   
	    trh = (trrif_t *) newpak->datagramstart;
	    /*
	     *
	     * Processing of simple two-port case was put here, instead of
	     * within 'datagram_out'.  The choice placement was to minimize
	     * the amount of code within datagram_out, and allow faster
	     * operation for other clients, about 150 as of 11/19/92...
	     * The large number of calling locations was also the motivation _not_
	     * to include an extra parameter to differentaite the call made here
	     * from (for example) srb_background().
	     */
	    /*
	     * Set new rif to minimum of that contained in frame, and
	     * the Lf supported by the input and output ports...
	     */
	    setRifLfTrh(trh, min(getRifLfTrh(trh),
				  min(getHwLf(newpak->if_input->hwptr),
				      getHwLf(newpak->if_input->hwptr->tr_bridge_idb)
				      )
				  )
			);
	}
	tr_to_media_out(newpak);
    } else {
	  /*
	   * Unlike the case above, Lf (largest frame) processing of
	   * explorer frames is handled from within vrforward_pak().
	   * The decision of placement was due to the low number of
	   * calling clients (about 10) and the complexity of retrieving
	   * supporting virtual data structures which are already retrieved once.
	   */

	vrforward_pak(newpak, FALSE, RSRB_OP_FORWARDCRC, RSRB_RIFLF_KNOCKDOWN);
    }
    if ((nbh->command == NETBIOS_NAME_QUERY) && !TIMER_RUNNING(cacheptr->query_tick))
	TIMER_START(cacheptr->query_tick, rif_validation_age);
    netbios_retbuffer(pak);
    return(TRUE);
}

/*
 * netbios_name_init
 *
 * Initialize netbios name data structures
 */
void netbios_name_init (subsystype *subsys)
{
    int i;
    hwidbtype *idb;

    for (i = 0; i < NETBIOS_HASH_LEN; i++) {
	netbios_name_cache_table[i] = NULL;
    }
    netbios_name_cache_timeout = NETBIOS_DEFAULT_CACHE_TIMEOUT;
    netbios_query_timeout = NETBIOS_DEFAULT_QUERY_TIMEOUT;
    netbios_name_length = NETBIOS_NAME_LENGTH_DEFAULT;
    netbios_proxy_datagram = FALSE;
    netbios_cache_add_name = FALSE;
    netbios_broadcast_name_recognized = FALSE;
    netbios_recognized_timeout = NETBIOS_DEFAULT_RECOGNIZED_TIMEOUT;
    global_netbios_name_cache_enable = FALSE;
    queue_init(&free_netbios_name_cacheQ, 0);
    FOR_ALL_HWIDBS(idb) {
	netbios_init_idb(idb);
    }

    /*
     * Register some functions
     */
    reg_add_onemin(netbios_age_cache, "netbios_age_cache");
    reg_add_hw_idb_init(netbios_init_idb, "netbios_init_idb");
    reg_add_hwif_state_change(netbios_if_state_change,
			      "netbios_if_state_change");
    reg_add_media_rif_reset(netbios_flush_interface,
			      "netbios_flush_interface");
    netbios_debug_init();
}

/*
 * netbios_name_if_disable
 *
 */
void netbios_name_if_disable (hwidbtype *hwidb)
{
    boolean res;
    hwidbtype *idblist;

    netbios_sb_disable_name_caching(hwidb->firstsw);
    res = FALSE;
    FOR_ALL_HWIDBS(idblist) {
	if (netbios_sb_name_caching_is_enabled(idblist->firstsw)) {
	    res = TRUE;
	    break;
	}
    }
    if (!res) {
	global_netbios_name_cache_enable = FALSE;
    }
}

/*
 * netbios_name_if_command
 *
 * Parse interface specific netbios name commands
 */
void netbios_name_if_command (parseinfo *csb)
{
    idbtype *idb;
    hwidbtype *hwidb;
    netbios_sb_t *netb_sb;

    if (!bridge_enable) {
	if (!csb->nvgen) {
	    printf("\nNETBIOS commands not supported in this release");
	}
	return;
    }

    idb = csb->interface;
    hwidb = hwidb_or_null(idb);

    netb_sb = netbios_get_or_create_sb(idb);
    if (!netb_sb) {
      return; /* Serious problem - no memory */
    }

    if (csb->nvgen) {
	switch (csb->which) {
	  case NETBIOS_ENABLE_NAMECACHE:
	    nv_write(netb_sb->name_caching_enabled, csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case NETBIOS_ENABLE_NAMECACHE:
	if (csb->sense) {
	    if (!srbV.srb_sb_get_proxy_explorer(idb)) {
		printf("\nnetbios: proxy explorer must be enabled "
		       "on interface for netbios name");
		printf("\n         caching to be used.");
		return;
	    }
            netb_sb->name_caching_enabled = TRUE;
	    if (!global_netbios_name_cache_enable) {
		global_netbios_name_cache_enable = TRUE;
		reg_add_critical_background(netbios_grow_request,
					    "netbios_grow_request");
		netbios_grow_request();
	    }
	} else {
	    netbios_name_if_disable(hwidb);
	}
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * Netbios name caching subsystem header
 */

#define NETB_MAJVERSION 1
#define NETB_MINVERSION 0
#define NETB_EDITVERSION  1

SUBSYS_HEADER(netbios_name, NETB_MAJVERSION, NETB_MINVERSION, NETB_EDITVERSION,
	      netbios_name_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, netbios_acl, srb, sdlc",
	      "req: netbios_acl, srb");


