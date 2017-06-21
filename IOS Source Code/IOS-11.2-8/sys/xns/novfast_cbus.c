/* $Id: novfast_cbus.c,v 3.4.4.3 1996/09/11 17:39:40 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xns/novfast_cbus.c,v $
 *------------------------------------------------------------------
 * novfast_cbus.c -- cBus Novell IPX Autonomous Switching Support
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novfast_cbus.c,v $
 * Revision 3.4.4.3  1996/09/11  17:39:40  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.4.4.2  1996/09/07  02:08:34  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 3.4.4.1  1996/03/18  22:53:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  07:59:56  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/03/13  02:14:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/01/24  23:33:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/03/09  18:13:27  mschaef
 * CSCdi46978:  IPX fastswitch cache can grow very large
 *
 * Revision 3.3  1995/11/17  19:23:30  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:53:50  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:59:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/01  03:39:04  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.3  1995/06/28 09:33:46  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/20  21:53:57  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:29:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "subsys.h"
#include "fastswitch_registry.h"
#include "packet.h"
#include "address.h"
#include "registry.h"
#include "../ui/debug.h"
#include "../if/tring.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../if/fddi.h"
#include "../xns/novell.h"
#include "../xns/novfast.h"
#include "../xns/novfast_cbus.h"
#include "../xns/novell_debug.h"
#include "../srt/span.h"
#include "../hes/cbus_registry.h"

/*
 * Maximum hash bucket size. Allowing a single bucket to exceed these values
 * could result in latency problems if the SP gets bogged down searching
 * oversized buckets.
 */
#define IPXCACHE_BUCKET_MAX        20

/*
 * Define minimum number of minutes between full cache resets triggered
 * by the occurance of buffer overflows since the last cache invalidation.
 * This is to prevent a cache entry which overflows a bucket from being
 * fastswitched forever while invalid (inactive) entries occupy cbus
 * cache memory.
 */

#define MIN_OVERFLOW_DELAY (ONEMIN*15)

/*
 * Pseudo structure to define the IPX data structure entry in cbus memory.
 */
typedef struct cbusipxcachetype_ {
    ushort link;
    ushort version;		
    ushort macsize;		/* variable -- based on encapsulation */
    ushort output_index;	/* output->mci_index */
    struct nov_addrtype key;	/* destination ipx address */
} cbusipxcachetype;

typedef struct cbusipxnewcachetype_ {
    ushort link;
    ushort version;		
    ushort macsize;		/* variable -- based on encapsulation */
    ushort output_index;	/* output->mci_index */
    struct nov_addrtype key;	/* destination ipx address */
    ushort mode;                /* mode flags */
} cbusipxnewcachetype;

/*
 * Allow for max display of snap on tokenring with full rif = 14 + 18 + 8
 */

#define MAX_ENCAP_BYTES (14+18+8)
#define MAX_ENCAP_WORDS MAX_ENCAP_BYTES/2

/*
 * Cbus display descriptor:
 */

typedef struct cbusipxdisplaytype_ {
    struct cbusipxdisplaytype_ *next;
    ushort bucket;
    struct cbusipxcachetype_ cbuscache;
    uchar encapsulation[MAX_ENCAP_BYTES];
} cbusipxdisplaytype;

#define	MODE_CISCO_CHECKSUM	0x8
#define	MODE_HOPCOUNT		0x4
#define	MODE_8BIT_HOPCOUNT	0x1
#define	MODE_4BIT_HOPCOUNT	0x0

#define	MODE_ARPA		0x00
#define	MODE_SAP		0x10
#define	MODE_NOVELL_TR		0x10
#define	MODE_SNAP		0x20
#define	MODE_NOVELL_ETHER	0x30

/*
 * Local variables:
 */
static const char msg_ipxcache_err_msg[] = "\nError: (%-04x), %s - %s()";
static const char msg_ipxcache_update[] = "cbus_ipxcache_update";
static const char msg_ipxcache_add[] = "cbus_ipxcache_add";
static const char msg_ipxcache_invalidate[] = "cbus_ipxcache_invalidate";
static const char msg_ipxcache_show[] = "cbus_ipxcache_show";
static ushort cbus_shadow_ipx_version;
static ulong cbus_ipx_cache_allocated;         /* Number of (IPX) words used */
static ulong cbus_ipx_overflow_count;          /* Hash bucket overflows */
static ulong cbus_ipx_delta_overflow_count;    /* Overflows since last init */
static sys_timestamp cbus_ipx_delta_overflow_time; /* Time of last init */
static ulong cbus_ipx_init_count;
static boolean cbus_ipx_new_mode;              /* Mode word in new position */


/*
 * cbus_ipxcache_init
 * Initialize the cbus cache -- if the microcode supports it.
 */

void cbus_ipxcache_init (void)
{
    cbus_cache_init();
}	

/*
 * cbus_ipxcache_init_callback
 *
 *     Initialize ipx cache state following generic initialization
 */

void cbus_ipxcache_init_callback(void)
{
    mcitype *cbus;

    cbus_ipx_cache_allocated = 0;
    cbus_ipx_delta_overflow_count = 0;
    GET_TIMESTAMP(cbus_ipx_delta_overflow_time);
    cbus_ipx_init_count++;
    novell_auto_sw_supported = FCI_IPX_AUTO(cbus_switching_supported);
    cbus_ipx_new_mode =	(cbus_switching_supported & FCI_CAP_IPX_AUTO_NEW);

    if (novell_auto_sw_supported) {
	/*
	 * Always force FS cache invalidation. Otherwise we will
	 * punt unknown destinations to FS level. There they may be
	 * fastswitched indefinitely (or at least until the FS cache
	 * is invalidated through natural causes).
	 */
	novcache_version++;
	
	if ((cbus = (mcitype *) cbusQ.qhead)) {
#ifndef C7000
	    if (cbus->flags != CBUSII)
	        novell_auto_sw_supported = FALSE;
#else /* C7000 */
#endif
	}
    }
}	

/*
 * cbus_ipxcache_version
 * Update the cBus ipx cache version.
 */
void cbus_ipxcache_version (void)
{
    mcitype *cbus;
    mciregtype *regptr;
    leveltype level;

    if (novell_auto_sw_supported) {
	cbus = (mcitype *)cbusQ.qhead;
	level = raise_interrupt_level(NETS_DISABLE);
	regptr = cbus->regptr;
    	/*
     	 * Select the proper hash table
    	 */
    	regptr->argreg = FCI_IPX_HASH_TABLE;	/* Select the IPX cache */
    	regptr->cmdreg = FCI_SELECT_HASH_TABLE;

	regptr->argreg = FCI_IPX_CACHE_UPDATE; /* IPX cache version */
	regptr->cmdreg = FCI_CACHE_INVALIDATE;
	DELAY(1);
	if (((cbus_shadow_ipx_version = regptr->argreg) == 0) ||
	    ((cbus_ipx_delta_overflow_count) &&
 	     (CLOCK_OUTSIDE_INTERVAL(cbus_ipx_delta_overflow_time,
				     MIN_OVERFLOW_DELAY)))) {
	    /*
	     * If we invalidate the autonomous cache too many times, the
	     * version number will wrap and old version numbers in the
	     * cache could become valid again.  If that happens, reinitialize
	     * the entire cache. Also, blow away the entire cache if
 	     * we have seen overflows since the last invalidation ... but
 	     * not more frequently than every MIN_OVERFLOW_DELAY minutes.
	     */
	    cbus_ipxcache_init();
	}
	reset_interrupt_level(level);
    }
}

/*
 * cbus_ipxcache_macsize()
 *	Calculate the MACsize of the nexthop MAC for cBus cache population
 *	Returns WORDS(16-bit) value.
 *
 * Define a maximum MAC size which is snap encap on ethernet or tokenring
 * plus two mode bytes. Note that while ethernet has 2 length bytes not
 * present on tokenring, tokenring has AC/FC bytes not present on ethernet.
 * The max size (rather than the "true" size) will be returned if requested
 * and when load sharing is in effect. When load sharing is in effect, we
 * always allocate a packet large enough to accomodate the worst (rif-less)
 * encapsulation so as to avoid problems with AS cache entries which are
 * too small. This tends to happen because of the fact that the multiple
 * paths are often via different media/encapsulations, with different sizes.
 * By doing this we avoid reinitializing (blowing away) the entire AS cache
 * as a routine side effect of a normal update.
 */

#define MAX_MAC_SIZE btow(6+6+2+8+2)

static int cbus_ipxcache_macsize(hwidbtype *idb, novpathtype *novp,
 			  boolean true_size_requested)
{
    unsigned macsize = btow(novp->total_encap_bytes);
    
    if ((!true_size_requested) && (novell_maxpaths > 1))
 	return(MAX_MAC_SIZE);
    
    switch(novp->enctype) {
    case ET_SAP:
	if (idb->status & IDB_ETHER)
	    macsize -= btow(ETHER_ISO1_SIZE);
	else if (idb->status & IDB_FDDI)
	    macsize -= btow(FDDI_ISO1_SIZE);
	else if (idb->status & IDB_TR) {
	    macsize -= btow(FDDI_ISO1_SIZE);
	    macsize--;
	}
	break;
    case ET_SNAP:
	if (idb->status & IDB_ETHER)
	    macsize -= btow(ISO2_SIZE);
	else if (idb->status & IDB_FDDI)
	    macsize -= btow(ISO2_SIZE);
	else if (idb->status & IDB_TR)
	    macsize -= btow(ISO2_SIZE);
	break;
    default:
	if (idb->status & IDB_ETHER)
	    macsize -= 1;
	else if (idb->status & IDB_FDDI)
	    macsize -= btow(FDDI_SNAP_SIZE);
	else if (idb->status & IDB_TR)
	    macsize -= btow(TR_SNAP_SIZE);
	break;
    }
    if (idb->status & IDB_FDDI || idb->status & IDB_TR)
	macsize++;	/* Add 1 for a cach alloc fake.  Gets sub'ed later */
    return(macsize);
}

/*
 * cbus_ipxcache_mode - Compute mode field from cache entry
 */
static ushort
cbus_ipxcache_mode ( novpathtype *novp )
{
    ushort mode;

    mode = 0;
    /*
     * Set the MODE & HOP count bytes at the start of encapsulation
     */
    mode = (MODE_CISCO_CHECKSUM |
	    ((novp->igrp_route) ? MODE_8BIT_HOPCOUNT : MODE_4BIT_HOPCOUNT));
    switch(novp->enctype) {
      case ET_SAP:
	mode |= MODE_SAP;
	break;
      case ET_SNAP:
	mode |= MODE_SNAP;
	break;
      case ET_NOVELL_ETHER:
	mode |= MODE_NOVELL_ETHER;
	break;
      default:
	mode |= MODE_ARPA;
	break;
    }
    return(mode);
}

/*
 * cbus_ipxcache_add
 * Function to add an IPX entry into the cBus cache.
 * N.B.: This function is called with interrupts disabled.
 */

static void cbus_ipxcache_add (paktype *pak, novpathtype *novp)
{
    hwidbtype *idb;
    mciregtype *regptr;
    ushort ustemp, *usptr, uswords, riflen;
    trrif_t *trh;
    fddirif_t *fddih;
    novcachetype *novc = pak->cacheptr;
    int mac_size;
    uchar rii_mask;

    idb = pak->if_output->hwptr;
    riflen = 0;
    if (idb->status & IDB_TR) {
	trh = (trrif_t *)pak->datagramstart;
	if (trh->saddr[0] & TR_RII)
	    riflen = trh->rc_blth & RC_LTHMASK;
    } else if (idb->status & IDB_FDDI) {
	fddih = (fddirif_t *)pak->datagramstart;
 	if (idb->type == IDBTYPE_FDDIT) 
	    rii_mask = FDDIT_RII;
	else
	    rii_mask = FDDI_RII;

	if (fddih->saddr[0] & rii_mask)
 		riflen = fddih->rc_blth & RC_LTHMASK;
	}
    mac_size = cbus_ipxcache_macsize(idb, novp, FALSE) + btow(riflen);
    regptr = idb->mci_regptr;

    /*
     * Select the proper hash table
     */
    regptr->argreg = FCI_IPX_HASH_TABLE;
    regptr->cmdreg = FCI_SELECT_HASH_TABLE;
    
    /*
     * Request an IPX cache entry from the cBus.
     */
    uswords = btow(sizeof(cbusipxcachetype)) + mac_size;
    uswords++;			/* +1 for MODE field @ end */
    regptr->argreg = uswords;	
    regptr->cmdreg = FCI_ALLOCATE_CACHE;
    if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
	cbus_ipxcache_init();
	if (novellrip_event_debug || novelligrp_event_debug)
	    buginf(msg_ipxcache_err_msg, ustemp, "cache allocate",
		   msg_ipxcache_add); 
	return;
    }
    
    /*
     * Keep track of the number of words allocated.
     */
    cbus_cache_allocated += uswords;
    cbus_ipx_cache_allocated += uswords;
    
    /*
     * Point to the rxtype field.
     */
    regptr->argreg = CACHE_MACSIZE;
    regptr->cmdreg = FCI_CACHE_SELECT;
    if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
	buginf(msg_ipxcache_err_msg, ustemp, "cache select", msg_ipxcache_add);
	return;
    }
    
    /*
     * Set size of MAC and encapsulation.
     */
    regptr->rw_cache = novp->total_encap_bytes;
    
    /*
     * Compute the actual mema/memd address of the ICBptr. CHECK
     */
    regptr->rw_cache = idb->cbus_icb_address;
    usptr = (ushort *)&novc->novc_dest_net;
    regptr->rw_cache = *usptr++;
    regptr->rw_cache = *usptr++;
    regptr->rw_cache = *usptr++;
    regptr->rw_cache = *usptr++;
    regptr->rw_cache = *usptr;
    
    if (cbus_ipx_new_mode) {
	/*
	 * Set the MODE & HOP count bytes at the start of encapsulation
	 */
	regptr->rw_cache = cbus_ipxcache_mode(novp);
    }
    
    /*
     * Make adjustment to mac_size for the various output media.
     */
    mac_size = cbus_ipxcache_macsize(idb, novp, TRUE) + btow(riflen);
    if (idb->status & IDB_FDDI) {
	regptr->rw_cache = FDDI_LLC_FC_BYTE;
	mac_size--;
    } else if (idb->status & IDB_TR) {
	regptr->rw_cache = (AC_PRI0 << 8) | FC_TYPE_LLC;
	mac_size--;
    } 

    /*
     * Copy the MAC encapsulation into the cBus.
     */
    usptr = (ushort *)novp->novp_mh.mac_shorts;
    while (mac_size--) {
	regptr->rw_cache = *usptr++;
    }

    if (!cbus_ipx_new_mode) {
	/*
	 * Set the MODE & HOP count bytes at the start of encapsulation
	 */
	regptr->rw_cache = cbus_ipxcache_mode(novp);
    }
    
    /*
     * Now add the constructed entry into the cache.
     */
    regptr->argreg = btow(sizeof(struct nov_addrtype));	/* key size */
    regptr->cmdreg = FCI_CACHE_ENTER;
    if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY)
	buginf(msg_ipxcache_err_msg, ustemp, "cache enter", msg_ipxcache_add);
    novc->novc_ext_cache = TRUE;
}

/*
 * cbus_ipxcache_update
 * Check if we need to update the cBus cache -- called immediately after
 * novcache_update has added or updated the system cache.
 */

void cbus_ipxcache_update (paktype *pak)
{
    hwidbtype *output;
    leveltype level;
    mciregtype *regptr;
    ushort *usptr, ustemp, us_cacheoutput;
    int mac_size, cache_size, riflen;
    trrif_t *trh;
    fddirif_t *fddih;
    uchar rii_mask;
    novcachetype *novc = pak->cacheptr;
    novpathtype *novp, *path_list_ptr;
    ulong bucket_entries;
    
    /*
     * Select the path entry matching the current packet:
     */
    novp = path_list_ptr = novc->novc_path_list_ptr;
    do {
 	if (novp->novp_nidb_ptr == pak->nif_output)
 	    break;
 	novp = novp->novp_next_ptr;
    } while (novp != path_list_ptr);
    
    if (novp->novp_nidb_ptr != pak->nif_output)
	return;
    
    output = pak->if_output->hwptr;
    /*
     * If microcode doesn't support the function -- exit.
     */
    if ((!novell_auto_sw_supported) || 
	(!(output->novell_routecache & NOVELL_RCACHE_CBUS)))
	return;
    
    riflen = 0;
    if (output->status & IDB_TR) {
	trh = (trrif_t *)pak->datagramstart;
	if (trh->saddr[0] & TR_RII)
	    riflen = trh->rc_blth & RC_LTHMASK;
    } else if (output->status & IDB_FDDI) {
	fddih = (fddirif_t *)pak->datagramstart;
 	if (output->type == IDBTYPE_FDDIT) 
	    rii_mask = FDDIT_RII;
	else
	    rii_mask = FDDI_RII;

	if (fddih->saddr[0] & rii_mask)
 		riflen = fddih->rc_blth & RC_LTHMASK;
    }
    
    regptr = output->mci_regptr;
    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Select the proper hash table 
     */
    regptr->argreg = FCI_IPX_HASH_TABLE;
    regptr->cmdreg = FCI_SELECT_HASH_TABLE;
    
    /*
     * Select KEY offset.
     */
    regptr->argreg = 0;
    regptr->cmdreg = FCI_KEY_SELECT;
    if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
	/*
	 * FCI_KEY_SELECT should never ever fail.  If it
	 * does something is very very wrong.  Turn off
	 * switching entirely.
	 */
	novell_auto_sw_supported = FALSE;
	buginf("\nError %04x: FCI_KEY_SELECT, IPX autoswitching disabled",
		ustemp);
	reset_interrupt_level(level);
	return;
    }
    
    /*
     * Write the key(IPX destination host address) into MEMA.
     */
    usptr = (ushort *)&novc->novc_dest_net;
    /*
     * Must write full 10-byte IPX address.
     */
    regptr->rw_key = *usptr++;
    regptr->rw_key = *usptr++;
    regptr->rw_key = *usptr++;
    regptr->rw_key = *usptr++;
    regptr->rw_key = *usptr;
    
    /*
     * Ask the cBus to check if the entry exists.
     */
    regptr->argreg = btow(IPX_ADDR_BYTES);
    regptr->cmdreg = FCI_LOOKUP_CACHE;
    if ((ustemp = regptr->cmdreg) == FCI_RSP_OKAY) {
	
	/*
	 * Scan all entries to find the matching key.
	 */
 	bucket_entries = 0;
	do {
 	    if (bucket_entries++ > IPXCACHE_BUCKET_MAX) {
 		cbus_ipx_overflow_count++;
 		cbus_ipx_delta_overflow_count++;
 		/*
 		 * For now, bail without taking any corrective action if
 		 * this hash bucket is full. All overflow entries will
 		 * fastswitch until the cache is invalidated (at some
 		 * unspecified time in the future).
 		 */
		regptr->argreg = 0;
 		regptr->cmdreg = FCI_CACHE_DELETE;
 		if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
 		    buginf(msg_ipxcache_err_msg, ustemp, "delete cache", 
 			   msg_ipxcache_update);
 		}
 		reset_interrupt_level(level);
 		return;
 	    }
	    /*
	     * Select offset to point at the IPX destination address.
	     */
	    regptr->argreg = CACHE_DSTADR;
	    regptr->cmdreg = FCI_CACHE_SELECT;
	    if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
		buginf(msg_ipxcache_err_msg, ustemp, "select cache", 
		       msg_ipxcache_update);
		reset_interrupt_level(level);
		return;
	    }
	    
	    /*
	     * Read the destination address from the cache. CHECK
	     */
	    usptr = (ushort *) &novc->novc_dest_net;
	    if ((*usptr++ != regptr->rw_cache) || 
		(*usptr++ != regptr->rw_cache) ||
		(*usptr++ != regptr->rw_cache) ||
		(*usptr++ != regptr->rw_cache) ||
		(*usptr != regptr->rw_cache)) {
		regptr->cmdreg = FCI_CACHE_GET_NEXT;
		if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
		    if (ustemp == NO_CACHE_ENTRY) {
			/* 
			 * Add the entry to the cache unless bucket full:
			 */
 			if (bucket_entries < IPXCACHE_BUCKET_MAX) {
 			    cbus_ipxcache_add(pak, novp);
 			} else {
 			    cbus_ipx_overflow_count++;
 			    cbus_ipx_delta_overflow_count++;
 			}
			reset_interrupt_level(level);	
			return;
		    }
		    buginf(msg_ipxcache_err_msg, ustemp, 
			   "get next cache IPX ADDRESS", 
			   msg_ipxcache_update);
		    reset_interrupt_level(level);
		    return;
		}
		continue;
	    }
	    break;
	} while ((ustemp = regptr->cmdreg) == FCI_RSP_OKAY);
	
	/*
	 * Found the correct entry.
	 */
	regptr->argreg = CACHE_OUTPUT;
	regptr->cmdreg = FCI_CACHE_SELECT;
	if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
	    buginf(msg_ipxcache_err_msg, ustemp, "select cache", 
		   msg_ipxcache_update);
	    reset_interrupt_level(level);
	    return;
	}
	us_cacheoutput = regptr->rw_cache;
	mac_size = novp->total_encap_bytes;
	
	/*
	 * See if the current entry is smaller than needed.
	 */
	regptr->argreg = CACHE_MACSIZE;
	regptr->cmdreg = FCI_CACHE_SELECT;
	if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
	    buginf(msg_ipxcache_err_msg, ustemp, "select cache", 
		   msg_ipxcache_update);
	    reset_interrupt_level(level);
	    return;
	}
	cache_size = regptr->rw_cache;
	
	/*
	 * If different output interface or mac size different --
	 * additional work may be needed.
	 */
	if ((us_cacheoutput != output->cbus_icb_address) ||
 	    (cache_size != mac_size)) {
 	    /*
 	     * When load-sharing is in effect, all entries are allocated
	     * with fixed size large enough to accomodate the largest
	     * encap (+ rif). The check for a new, larger encap need be
	     * applied only if not load-sharing or rif present.
 	     */
 	    if (((novell_maxpaths == 1) || (riflen != 0)) &&
 		(mac_size > cache_size)) {
 		/* 
 		 * Blow everything away and start over:
		 */
		cbus_ipxcache_init();
		cbus_ipxcache_add(pak, novp);
		reset_interrupt_level(level);	
		return;
	    }
	    
	    if (us_cacheoutput != output->cbus_icb_address) {
		regptr->argreg = CACHE_OUTPUT;
		regptr->cmdreg = FCI_CACHE_SELECT;
		if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
		    buginf(msg_ipxcache_err_msg, ustemp, "select cache", 
			   msg_ipxcache_update);
		    reset_interrupt_level(level);
		    return;
		}
		regptr->rw_cache = output->cbus_icb_address;
	    }

	    if (cache_size != mac_size) {
		/*
 		 * Update the mac size - new one is different:
		 */
		regptr->argreg = CACHE_MACSIZE;
		regptr->cmdreg = FCI_CACHE_SELECT;
		if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
		    buginf(msg_ipxcache_err_msg, ustemp, "select cache", 
			   msg_ipxcache_update);
		    reset_interrupt_level(level);
		    return;
		}
		regptr->rw_cache = mac_size;
	    }
	}	    
	
	if (cbus_ipx_new_mode) {
	    regptr->argreg = CACHE_MODE;
	    regptr->cmdreg = FCI_CACHE_SELECT;
	    if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
		buginf(msg_ipxcache_err_msg, ustemp, "select cache", 
		       msg_ipxcache_update);
		reset_interrupt_level(level);
		return;
	    }
	    
	    /*
	     * Set the MODE & HOP count bytes at the start of encapsulation
	     */
	    regptr->rw_cache = cbus_ipxcache_mode(novp);
	}
    
	/*
	 * Select window to point at the encapsulation data.
	 */
	if (cbus_ipx_new_mode) {
	    regptr->argreg = btow(sizeof(cbusipxnewcachetype));
	} else {
	    regptr->argreg = btow(sizeof(cbusipxcachetype));
	}
	regptr->cmdreg = FCI_CACHE_SELECT;
	if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
	    buginf(msg_ipxcache_err_msg, ustemp, "select cache", 
		   msg_ipxcache_update);
	    reset_interrupt_level(level);
	    return;
	}
 	mac_size = cbus_ipxcache_macsize(output, novp, TRUE) + btow(riflen);
	if (output->status & IDB_FDDI) {
	    regptr->rw_cache = FDDI_LLC_FC_BYTE;
	    mac_size--;
	} else if (output->status & IDB_TR) {
	    regptr->rw_cache = (AC_PRI0 << 8) | FC_TYPE_LLC;
	    mac_size--;
	} 
    
	/*
	 * Copy the encapsulation data into the cache.
	 */
	usptr =(ushort *)novp->novp_mh.mac_shorts;
	while (mac_size--)
	    regptr->rw_cache = *usptr++;

	if (!cbus_ipx_new_mode) {
	    /*
	     * Set the MODE & HOP count bytes at the end of encapsulation
	     */
	    regptr->rw_cache = cbus_ipxcache_mode(novp);
	}
    
	/* 
	 * Update the cbus cache entry and exit.
	 */
	regptr->cmdreg = FCI_CACHE_VERSION_UPDATE;
	if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY)
	    buginf(msg_ipxcache_err_msg, ustemp, "cache version update", 
		   msg_ipxcache_update);
	if (ustemp == FCI_RSP_OKAY) {
	    novc->novc_ext_cache = TRUE;
	}
	reset_interrupt_level(level);
	return;    
    } else if (ustemp == NO_CACHE_ENTRY) {
	/*
	 * No entry exists -- need to create an entry.
	 */
	cbus_ipxcache_add(pak, novp);
	reset_interrupt_level(level);
	return;
    } else {
	buginf(msg_ipxcache_err_msg, ustemp, "lookup cache", 
	       msg_ipxcache_update);
	reset_interrupt_level(level);
	return;
    }
}

/*
 * cbus_icb2idb
 *
 *     Return short idb string corresponding to "cbus_icb_address", or NULL.
 */
static hwidbtype
*cbus_icb2idb( int icb_address )
{
    hwidbtype *hwidb;

    FOR_ALL_HWIDBS(hwidb)
	if (hwidb->cbus_icb_address == icb_address)
	    return(hwidb);
    return(NULL);
}


/*
 * free_display_queue
 *
 *     Deallocate display queue in case of error
 *
 */
static void
free_display_queue(queuetype *qp)
{
    uchar *mp;

    while ((mp = dequeue(qp)) != NULL)
	free(mp);
}

/*
 * cbus_ipxcache_show
 * Debug quality display of IPX cache ... needs a little more work for
 * prime time.
 */

static const char cbus_ipxcache_string[] =
    "\n Hash              Destination  Itf  MAC Header + Mode";

void
cbus_ipxcache_show (void)
{
    leveltype level;
    mciregtype *regptr;
    ushort *usptr, ustemp, us_macsize, i, ii, bucket, depth;
    hwidbtype *hwidb;
    cbusipxdisplaytype *displayp = NULL;
    queuetype displayQ;
    ulong allocated, ipx_allocated, entries, currenthash, maxhash, loopmax;

    /*
     * Find a hwidb with IPX AS enabled:
     */
    FOR_ALL_HWIDBS(hwidb) {
	if (hwidb->novell_routecache & NOVELL_RCACHE_CBUS)
	    break;
    }

    if (hwidb == NULL) {
	printf("\nIPX autonomous switching not enabled");
	return;
    }

    queue_init(&displayQ, 0);

    regptr = hwidb->mci_regptr;
    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Record interesting stats with interrupts disabled. Then
     * run through the entire cache, building a linked list of
     * entries in system memory ... all this for later display
     * with interrrupts enabled.
     */
    allocated = cbus_cache_allocated;
    ipx_allocated = cbus_ipx_cache_allocated;
    entries = maxhash = 0;

#ifndef C7000
    for (i = 0; i < 256; i++) {
#else /* C7000 */
    for (i = 0; i < 256; i++) {
#endif
	currenthash = 0;
	/*
	 * Select the proper hash table 
	 */
	regptr->argreg = FCI_IPX_HASH_TABLE;
	regptr->cmdreg = FCI_SELECT_HASH_TABLE;
	/*
	 * Select KEY offset.
	 */
	regptr->argreg = 0;
	regptr->cmdreg = FCI_KEY_SELECT;
	if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
	    /*
	     * FCI_KEY_SELECT should never ever fail.  If it
	     * does something is very very wrong.  Turn off
	     * switching entirely.
	     */
	    novell_auto_sw_supported = FALSE;
	    buginf("\nError %04x: FCI_KEY_SELECT, IPX autoswitching disabled",
		   ustemp);
	    reset_interrupt_level(level);
	    free_display_queue(&displayQ);
	    return;
	}
	/*
	 * Write the key (IPX destination host address) into MEMA.
	 */
	regptr->rw_key = 0x0000;
	regptr->rw_key = 0x0000;
	regptr->rw_key = 0x0000;
	regptr->rw_key = 0x0000;
	regptr->rw_key = i;
	/*
	 * Ask the cBus to check if the entry exists.
	 */
	regptr->argreg = btow(IPX_ADDR_BYTES);
	regptr->cmdreg = FCI_LOOKUP_CACHE;
	if ((ustemp = regptr->cmdreg) == FCI_RSP_OKAY) {
	    /*
	     * Got something ... scan all entries in bucket:
	     */
	    loopmax = 0;
	    do {
                /*
		 * Something is seriously wrong if we can't find a match or
		 * the end of the bucket. Reset everything if this happens.
		 */
		if (loopmax++ > (2*IPXCACHE_BUCKET_MAX)) {
		    buginf("\nIPX: Autons Cache loops at point %d! Reset!", i);
		    cbus_ipxcache_init();
		    reset_interrupt_level(level);
		    free_display_queue(&displayQ);
		    return;
		}
		/*
		 * Select offset to point at the IPX destination address.
		 */
		regptr->argreg = CACHE_VERSION;
		regptr->cmdreg = FCI_CACHE_SELECT;
		if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
		    buginf(msg_ipxcache_err_msg, ustemp, "select cache", 
			   msg_ipxcache_show);
		    reset_interrupt_level(level);
		    free_display_queue(&displayQ);
		    return;
		}
		/*
		 * Allocate a memory resident descriptor for later display.
		 */
		displayp = malloc(sizeof(cbusipxdisplaytype));
		if (displayp == NULL) {
		    reset_interrupt_level(level);
		    free_display_queue(&displayQ);
		    return;
		}
		displayp->bucket = i;
		displayp->cbuscache.version = regptr->rw_cache;
		displayp->cbuscache.macsize = regptr->rw_cache;
		displayp->cbuscache.output_index = regptr->rw_cache;
		/*
		 * Read the destination address from the cache.
		 */
		displayp->cbuscache.key.net = 0;
		for (ii=0; ii < IEEEBYTES; ii++)
		    displayp->cbuscache.key.host[ii] = 0;
		usptr = (ushort *) &displayp->cbuscache.key.net;
		*usptr++ = regptr->rw_cache;
		*usptr++ = regptr->rw_cache;
		*usptr++ = regptr->rw_cache;
		*usptr++ = regptr->rw_cache;
		*usptr++ = regptr->rw_cache;

		us_macsize = (ROUND(displayp->cbuscache.macsize)/2) + 1;
		usptr = (ushort *) &displayp->encapsulation;
		for (ii = 0; ii < min(us_macsize,MAX_ENCAP_WORDS); ii++) {
		    *usptr++ = regptr->rw_cache;
		}

		p_enqueue(&displayQ, displayp);
		entries++;
		if (++currenthash > maxhash)
		    maxhash = currenthash;

		regptr->cmdreg = FCI_CACHE_GET_NEXT;
		if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
		    if (ustemp == NO_CACHE_ENTRY) {
			break;
		    } else {
			buginf(msg_ipxcache_err_msg, ustemp, 
			       "get next cache IPX ADDRESS", 
			       msg_ipxcache_show);
			reset_interrupt_level(level);
			free_display_queue(&displayQ);
			return;
		    }
		}
	    } while ((ustemp = regptr->cmdreg) == FCI_RSP_OKAY);
	    
	} else if (ustemp == NO_CACHE_ENTRY) {
	    /*
	     * No entry exists
	     */
	    continue;
	} else {
	    buginf(msg_ipxcache_err_msg, ustemp, "lookup cache", 
		   msg_ipxcache_show);
	    reset_interrupt_level(level);
	    free_display_queue(&displayQ);
	    return;
	}
    }
    reset_interrupt_level(level);

    /*
     * Interrupts are now enabled ... do the display at a civilized pace!
     */
    automore_enable(NULL);

    printf("\nIPX AS/FS cache version: %lu/%lu"
	   ", entries %d, memory (total/ipx) %d/%d"
	   "\nmax hash length %d, overflows %d, ipx inits %d\n",
	   cbus_shadow_ipx_version, novcache_version,
	   entries, allocated, ipx_allocated, maxhash,
	   cbus_ipx_overflow_count, cbus_ipx_init_count);
    
    automore_header(cbus_ipxcache_string);

    depth = bucket = 0;

    while ((displayp = dequeue(&displayQ)) != NULL) {

	if (bucket != displayp->bucket) {
	    bucket = displayp->bucket;
	    depth = 0;
	}
	
	hwidb = cbus_icb2idb(displayp->cbuscache.output_index);

	printf("\n%c%-02x/%2d %-8x.%e  %s  ",
	       ((displayp->cbuscache.version == cbus_shadow_ipx_version) ?
		'*' : ' '),
	       displayp->bucket, depth,
	       displayp->cbuscache.key.net, displayp->cbuscache.key.host,
	       ((hwidb == NULL) ? "???" : hwidb->hw_short_namestring));
	depth++;
	us_macsize = (ROUND(displayp->cbuscache.macsize)/2) + 1;
	usptr = (ushort *) &displayp->encapsulation;
	for (ii = 0; ii < min(us_macsize,MAX_ENCAP_WORDS); ii++) {
	    printf("%-04x", *usptr++);
	}
	free(displayp);
	if (automore_quit())
	    break;
    }
    automore_disable();
    free_display_queue(&displayQ);
}

/*
 * cbus_ipxcache_invalidate
 * Invalidate a single entry in the cBus cache.  This is passed the
 * destination to invalidate.  There may or may not be a cache entry to
 * invalidate.
 */

void cbus_ipxcache_invalidate (novcachetype *novc)
{
    mcitype *cbus;
    mciregtype *regptr;
    leveltype level;
    ushort ustemp, *usptr, version;
    ulong loopmax;

    /*
     * If microcode doesn't support the function -- exit.
     */
    if (!novell_auto_sw_supported)
	return;
    cbus = (mcitype *) cbusQ.qhead;
    if (!cbus)
	return;
    
    regptr = (mciregtype *) cbus->regptr;    
    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Select the proper hash table 
     */
    regptr->argreg = FCI_IPX_HASH_TABLE;
    regptr->cmdreg = FCI_SELECT_HASH_TABLE;
    
    /*
     * Select KEY offset.
     */
    regptr->argreg = 0;
    regptr->cmdreg = FCI_KEY_SELECT;
    ustemp = regptr->cmdreg;
    if (ustemp) {
	novell_auto_sw_supported = FALSE;
	reset_interrupt_level(level);
	return;
    }
    
    /*
     * Write the key into MEMA.
     */
    usptr = (ushort *)&novc->novc_dest_net;
    regptr->rw_key = *usptr++;
    regptr->rw_key = *usptr++;
    regptr->rw_key = *usptr++;
    regptr->rw_key = *usptr++;
    regptr->rw_key = *usptr;
    
    /*
     * Ask the cBus to check if the entry exits.
     */
    regptr->argreg = (btow(IPX_ADDR_BYTES));
    regptr->cmdreg = FCI_LOOKUP_CACHE;
    if ((ustemp = regptr->cmdreg) == FCI_RSP_OKAY) {
	
	/*
	 * Scan all entries to find the matching key.
	 */
	loopmax = 0;
	while (TRUE) {
	    /*
	     * Something is seriously wrong if we can't find a match or
	     * the end of the bucket. Reset everything if this happens.
	     */
	    if (loopmax++ > (2*IPXCACHE_BUCKET_MAX)) {
		cbus_ipxcache_init();
		reset_interrupt_level(level);
		return;
	    }
	    /*
	     * Select offset to point at the IPX destination address.
	     */
	    regptr->argreg = CACHE_DSTADR;
	    regptr->cmdreg = FCI_CACHE_SELECT;
	    if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
		buginf(msg_ipxcache_err_msg, ustemp, "select cache", 
		       msg_ipxcache_invalidate);
		reset_interrupt_level(level);
		return;
	    }
	    
	    /*
	     * Read the destination address from the cache.
	     */
	    usptr = (ushort *) &novc->novc_dest_net;
	    if ((*usptr++ == regptr->rw_cache) &&
	       (*usptr++ == regptr->rw_cache) &&
	       (*usptr++ == regptr->rw_cache) &&
	       (*usptr++ == regptr->rw_cache) &&
		(*usptr == regptr->rw_cache))
		break;
	    regptr->cmdreg = FCI_CACHE_GET_NEXT;
	    if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
		if (ustemp != NO_CACHE_ENTRY) {
		    buginf(msg_ipxcache_err_msg, ustemp,
			   "get next cache IPX ADDRESS", 
			   msg_ipxcache_invalidate);
		}
		reset_interrupt_level(level);
		return;
	    }
	}
	
	/*
	 * Found the correct entry.  Get the version number.
	 */
	regptr->argreg = CACHE_VERSION;
	regptr->cmdreg = FCI_CACHE_SELECT;
	if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
	    buginf(msg_ipxcache_err_msg, ustemp, "select cache", 
		   msg_ipxcache_invalidate);
	    reset_interrupt_level(level);
	    return;
	}
	version = regptr->rw_cache;
	
	/*
	 * Since the previous access causes an autoincrement, we have to
	 * reselect.
	 */
	regptr->argreg = CACHE_VERSION;
	regptr->cmdreg = FCI_CACHE_SELECT;
	if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
	    buginf(msg_ipxcache_err_msg, ustemp, "select cache", 
		   msg_ipxcache_invalidate);
	    reset_interrupt_level(level);
	    return;
	}
	regptr->rw_cache = --version;

	/*
	 * We need to clear the current cache entry pointer in the
	 * SP.  There is no clean way to do this, so we are using a command
	 * that was never fully implemented that does just what we want.
	 */
	regptr->argreg = 0;
	regptr->cmdreg = FCI_CACHE_DELETE;
	if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
	    buginf(msg_ipxcache_err_msg, ustemp, "delete cache", 
		   msg_ipxcache_invalidate);
	}
    } else if (ustemp != NO_CACHE_ENTRY) {
	buginf(msg_ipxcache_err_msg, ustemp, "lookup cache", 
	       msg_ipxcache_invalidate);
    }
    reset_interrupt_level(level);
    return;
}

/*
 * cbus_ipxcache_stats
 * Retrieve Novell specific statistics for autonomous switching
 */

static void cbus_ipxcache_stats (hwidbtype *idb)
{
    ulong counts;
    mciregtype *regptr;
    mcitype *mci;

    mci = idb->devctl;
    if (!(mci->flags & CBUSII))
	return;
    regptr = idb->mci_regptr;
    if (novell_auto_sw_supported) {
	regptr->argreg = ((FCI_IPX_RX_PKTS << 8) | idb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	counts = regptr->largreg;
	idb->counters.inputs_auto[ACCT_PROTO_NOVELL] += counts;
	novell_stat[NOVELL_INPUT] += counts;
	regptr->argreg = ((FCI_IPX_RX_BYTES << 8) | idb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	idb->counters.rx_cumbytes_auto[ACCT_PROTO_NOVELL] += regptr->largreg;
	regptr->argreg = ((FCI_IPX_TX_PKTS << 8) | idb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	counts = regptr->largreg;
	idb->counters.outputs_auto[ACCT_PROTO_NOVELL] += counts;
	novell_stat[NOVELL_FORWARDED] += counts;
	regptr->argreg = ((FCI_IPX_TX_BYTES << 8) | idb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	idb->counters.tx_cumbytes_auto[ACCT_PROTO_NOVELL] += regptr->largreg;
	regptr->argreg = ((FCI_IPX_TX_DROPS << 8) | idb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	counts = regptr->largreg;
	novell_stat[NOVELL_PITCHED] += counts;
	idb->counters.output_total_drops += counts;
	regptr->argreg = ((FCI_IP_CKS_ERRS << 8) | idb->mci_index);
	regptr->cmdreg = FCI_CMD_STATISTICS;
	novell_stat[NOVELL_CKSUMERR] += regptr->largreg;
    }
}

static void ipx_cbus_init (subsystype* subsys)
{
    reg_add_cbus_get_statistics(cbus_ipxcache_stats, "cbus_ipxcache_stats"); 
    reg_add_auto_sw_cache_version(LINK_NOVELL, cbus_ipxcache_version,
				     "cbus_ipxcache_version");
    reg_add_auto_sw_cache_init(LINK_NOVELL, cbus_ipxcache_init,
				  "cbus_ipxcache_init");
    reg_add_auto_sw_ipx_cache_show(cbus_ipxcache_show, "cbus_ipxcache_show");
    reg_add_auto_sw_ipx_cache_update((service_auto_sw_ipx_cache_update_type)
				     cbus_ipxcache_update,
				    "cbus_ipxcache_update");
    reg_add_auto_sw_ipx_cache_invalidate(cbus_ipxcache_invalidate,
					"cbus_ipxcache_invalidate");
    registry_add_list(REG_FS, SERVICE_AUTO_SW_CACHE_INIT_CALLBACK,
		      cbus_ipxcache_init_callback,
		      "cbus_ipxcache_init_callback");
    /*
     * Make sure CBUS is ready:
     */
    cbus_ipx_init_count = 0;
    cbus_ipxcache_init();
}


/*
 * IPX Autonomous switching subsystem header
 */ 
#define CBUSIPX_MAJVERSION 1
#define CBUSIPX_MINVERSION 0
#define CBUSIPX_EDITVERSION 1

SUBSYS_HEADER(ipx_cbus_fs, CBUSIPX_MAJVERSION, CBUSIPX_MINVERSION,
	      CBUSIPX_EDITVERSION,
	      ipx_cbus_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: ipx",
	      "req: ipx");

