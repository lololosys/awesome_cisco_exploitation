/* $Id: ipfast_cbus.c,v 3.4.22.4 1996/09/11 20:09:32 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_cbus.c,v $
 *------------------------------------------------------------------
 * ipfast_cbus.c -- cBus IP Fast Switching Support
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_cbus.c,v $
 * Revision 3.4.22.4  1996/09/11  20:09:32  snyder
 * CSCdi68837:  more things const
 *              156 out of data, 64 image
 * Branch: California_branch
 *
 * Revision 3.4.22.3  1996/07/11  21:32:36  mikel
 * CSCdi57248:  IP switching to/from a host not working when its MAC addr
 * changed
 * Branch: California_branch
 * make sure to flush cbus cache after a software disable
 *
 * Revision 3.4.22.2  1996/07/06  05:53:29  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.4.22.1  1996/03/18  20:16:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  01:37:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  00:59:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/01/23  08:49:48  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.3  1995/11/17  09:34:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:40  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:56:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:04:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/28  09:26:48  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:58:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "../if/tring.h"
#include "../if/ether.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../if/fddi.h"
#include "../wan/smds.h"
#include "../if/channel.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_cbus.h"
#include "../ip/ip_registry.h"
#include "../iprouting/iprouting_debug.h"
#include "config.h"
#include "../if/atm.h"

#include "../vlan/vlan.h"


/*
 * Pseudo structure to define the IP data structure entry in the cbus memory.
 */
typedef struct cbusipcachetype_ {
    ushort link;
    ushort version;		
    ushort macsize;		/* variable -- based on encapsulation */
    ushort output_index;	/* output->mci_index */
    ulong key;			/* destination ip address */
} cbusipcachetype;

/*
 * Cbus shadow cache entry structure.
 */
struct ipcache_cbtype_ {
    ipaddrtype destination;
    ipcache_rntype *rnentry;
    sys_timestamp last_update;
    ushort cbus_addr;
    ushort updates;
    struct ipcache_cbtype_ *prefix_next;
};

/*
 * cbusI max cache entry size is overhead + FDDI's FC+DA+SA rewrite.
 * cbusII/sp must support ATM/SMDS/SNAP encaps (24 words).
 */
#define CBUSI_MAX_ENCAPS      14
#define CBUSII_MAX_ENCAPS     48
#define CBUSI_MAX_CE_BYTES (sizeof(cbusipcachetype) + CBUSI_MAX_ENCAPS)
#define CBUSII_MAX_CE_BYTES (sizeof(cbusipcachetype) + CBUSII_MAX_ENCAPS)
#define CBUSI_RWCACHE_ADDR    0x0282
#define CBUSII_RWCACHE_ADDR   0x0002
#define CBUSI_FREECACHE_ADDR  0x0285
#define CBUSII_FREECACHE_ADDR 0x0005
#define BOGUS_VERSION         0xDEAD
#define CBUS_BUCKET_SIZE      4
#define CBUS_IPCACHE_ENTRIES  (NETHASHLEN * CBUS_BUCKET_SIZE)
#define CBUS_IPCACHE_SIZE     (CBUS_IPCACHE_ENTRIES * sizeof(ipcache_cbtype))
#define CBUS_IPCACHE_RECENT   ONESEC

/*
 * Local variable.
 */
static const char msg_ipcache_err_msg[] = "\nError: (%-04x), %s - %s()";
static const char msg_ipcache_add[] = "cbus_ipcache_add";
static const char msg_ipcache_invalidate[] = "\ncbus_ipcache_invalidate";
static ulong cbus_ipcache_allocated;
static ulong cbus_ipcache_valid_version;
static ulong cbus_ipcache_adds;
static ulong cbus_ipcache_hits;
static ulong cbus_ipcache_resets;
static ulong cbus_ipcache_misses;
static ulong cbus_ipcache_update_dampened;
static ulong cbus_ipcache_replaced;
static ulong cbus_ipcache_replace_dampened;
static ulong cbus_ipcache_invalidations;
static ushort cbus_rwcache_addr;
static ushort cbus_freecache_addr;
static int cbus_ip_cache_bytes;

/*
 * LATER: change to pointers and alloc from cbus_ipcache_init_callback...
 */
static ipcache_cbtype *ipcache_cbtable[NETHASHLEN];
static ipcache_cbtype ipcache_cballoc[CBUS_IPCACHE_ENTRIES];

/*
 * Read a single value from a specific location in cbus cache memory.
 * Takes about 4uS.
 */
static inline ushort cbus_cache_read (mciregtype *reg, ushort off)
{
    reg->argreg = off;
    reg->cmdreg = FCI_CMD_MEMA_SELECT;
    reg->cmdreg = FCI_CMD_MEMA_READ;
    return reg->argreg;
}

/*
 * Write a single value to a specific location in cbus cache memory.
 * Takes about 4uS.
 */
static inline void cbus_cache_write (mciregtype *reg, ushort off, ushort val)
{
    reg->argreg = off;
    reg->cmdreg = FCI_CMD_MEMA_SELECT;
    reg->argreg = val;
    reg->cmdreg = FCI_CMD_MEMA_WRITE;
}

/*
 * Select offset for sequential reads or writes of fast cbus memory.
 * Takes about 4uS.
 */
static inline void cbus_cache_select (mciregtype *reg, ushort off)
{
    cbus_cache_write(reg, cbus_rwcache_addr, off);
}


static inline ipcache_cbtype *cbus_ipcache_replace (ipaddrtype destination)
{
    ipcache_cbtype *set, *tmp, *last, *cbentry;
    ulong t, age;

    cbus_ipcache_misses++;
    set = ipcache_cbtable[nethash(destination)];
    last = &set[CBUS_BUCKET_SIZE];

    /*
     * Search for matching IP destination address in set.
     * If found, don't bother re-downloading the cache entry, since
     * we're seeing it due to latency or a broken cbus cache.
     */
    for (tmp = set; tmp < last; tmp++) {
	if (tmp->destination == destination) {
	    cbus_ipcache_update_dampened++;
	    return NULL;
	}
    }

    /*
     * Search for first invalid entry in set.
     */
    for (tmp = set; tmp < last; tmp++) {
	if (!tmp->rnentry) {
	    cbus_ipcache_adds++;
	    if (++tmp->updates == 0)
		tmp->updates--;
	    return tmp;
	}
    }

    /*
     * Now look to replace oldest entry in the set.
     */
    age = ONESEC;
    cbentry = NULL;

    for (tmp = set; tmp < last; tmp++) {
	t = ELAPSED_TIME(tmp->last_update);
	if (t > age) {
	    cbentry = tmp;
	    age = t;
	}
    }

    if (!cbentry)
	cbus_ipcache_replace_dampened++;
    else {
	ipcache_rntype *rn;
	
	/*
	 * Remove this cbus entry from system cache entry cb list.
	 */
	rn = cbentry->rnentry;
	tmp = rn->c.inval_ptr;

	if (tmp == cbentry) {
	    rn->c.inval_ptr = tmp->prefix_next;
	} else {
	    while (tmp && (tmp->prefix_next != cbentry))
		tmp = tmp->prefix_next;
	    if (tmp)
		tmp->prefix_next = cbentry->prefix_next;
	}

	if (++tmp->updates == 0)
	    tmp->updates--;

	cbus_ipcache_replaced++;
    }
    return cbentry;
}


/*
 * cbus_ipcache_update
 *
 * MUST BE CALLED WITH INTERRUPTS DISABLED!!!
 * (Either from fastswitch routine, or from process level update).
 *
 * A packet that could have been autonomously switched by the the
 * cbus was instead fastswitched by the the system.  If this entry
 * is not in the cbus cache, then swap out the first invalid (or oldest)
 * entry in the bucket hashed from the IP address.
 */
void cbus_ipcache_update (ipcache_rntype *rnentry, paktype *j,
			  ipaddrtype destination)
{
    ushort *usptr;
    short mac_size;
    ipcache_cbtype *cbentry;
    mcitype *cbus;
    hwidbtype *hwidb = rnentry->hwidb;
    idbtype *idb = rnentry->idb;
    leveltype level;
    mciregtype *regptr;

    /*
     * Make sure that the output interface supports and is configured
     * for autonomous switching before continuing.
     *
     * If the input and output interfaces are cBus and the output MTU is
     * at least the default MTU, populate the autonomous switching cache.
     * Fast switching pays attention to the MTU, but autonomous switching
     * currently does not.  We use the default MTU rather than the max MTU
     * for the comparison in order to accommodate FDDI, in which the max
     * interface MTU is 4470 but the max IP MTU is only 4352.
     */
    if (!(cbus_switching_supported & FCI_CAP_IP_AUTO) ||
	!(hwidb->ip_routecache & IP_CBUS_ROUTECACHE) ||
	!(hwidb->status & IDB_CBUS) ||
	hwidb->ip_fast_flags ||
 	(idb->ip_mtu < def_proto_mtu(idb, LINK_IP)))
	return;

    /* 
     * We do not autonomously switch vLAN encapsulated
     * IP packets, but we can SSE switch them!
     */

    if (IS_VLAN_CONFIGURED_SUBINTERFACE(idb) == TRUE)
        return;

    level = raise_interrupt_level(NETS_DISABLE);
    
    cbentry = cbus_ipcache_replace(destination);
    if (!cbentry) {
	reset_interrupt_level(level);
	return;
    }

    cbus = (mcitype *)cbusQ.qhead;
    regptr = cbus->regptr;
    
    GET_TIMESTAMP(cbentry->last_update);	    
    /*
     * Enter into list of all cbus entries built from
     * this system cache entry.
     */
    cbentry->prefix_next = rnentry->c.inval_ptr;
    rnentry->c.inval_ptr = cbentry;
    cbentry->rnentry = rnentry;
    cbentry->destination = destination;

    /*
     * The system fast cache doesn't include FC or ACFC in rewrite length.
     */
    mac_size = btow(rnentry->length);
    if (hwidb->status & (IDB_FDDI|IDB_TR))
	mac_size++;
	
    /*
     * Now fill in cache entry.  Since this is non-atomic, we need to
     * invalidate the cache entry before modifying it.
     */
    cbus_cache_select(regptr, cbentry->cbus_addr + CACHE_VERSION);
    regptr->rw_cache = BOGUS_VERSION;
    regptr->rw_cache = mac_size;
    regptr->rw_cache = hwidb->cbus_icb_address;
    usptr = (ushort *)&destination;
    regptr->rw_cache = *usptr++;
    regptr->rw_cache = *usptr;
    
    /*
     * Hard code in FC or ACFC for FDDI or Tokenring.
     * Also, make adjustment to mac_size for the various output media.
     * SNAP and LLC data is hard-coded in microcode for FDDI/TR/Ethernet,
     * so it need not be written into cache.
     */
    if (hwidb->status & IDB_FDDI) {
	regptr->rw_cache = FDDI_LLC_FC_BYTE;
	mac_size -= btow (FDDI_SNAP_SIZE) + 1;
    } else if (hwidb->status & IDB_TR) {
	regptr->rw_cache = (AC_PRI0 << 8) | FC_TYPE_LLC;
	mac_size -= btow (TR_SNAP_SIZE) + 1;
    } else if (hwidb->status & (IDB_ETHER|IDB_LEX)) {
	mac_size--;
    }
    
    /*
     * Copy the MAC encapsulation into the cBus cache.
     */
    usptr = rnentry->m.macshort;
    while (mac_size-- > 0)
	regptr->rw_cache = *usptr++;

    /*
     * Now validate the cache entry with an atomic write of the version.
     */
    cbus_cache_write(regptr, cbentry->cbus_addr + CACHE_VERSION,
		     cbus_ipcache_valid_version);
    reset_interrupt_level(level);
}


/*
 * cbus_ipcache_init
 * Initialize the cbus cache -- if the microcode supports it.
 */

void cbus_ipcache_init (void)
{
    cbus_cache_init();                  /* Generic init */
}	

/*
 * cbus_ipcache_init_callback
 *
 *     Initialize IP AS state following generic cache initialization
 */

void cbus_ipcache_init_callback (void)
{
    mcitype *cbus;
    mciregtype *regptr;
    int i, size;
    ipcache_cbtype *cbentry;
    ushort addr, ustemp, vaddr;
    short set;
    leveltype level;

    /*
     * Most of these should go to interface stats to avoid
     * redundancy and allow selective clearing of counters.
     */
    cbus_ipcache_resets++;
    cbus_ipcache_allocated = 0;		/* Amount of memory allocated to IP */
    cbus_ipcache_adds = 0;
    cbus_ipcache_hits = 0;
    cbus_ipcache_misses = 0;
    cbus_ipcache_update_dampened = 0;
    cbus_ipcache_replaced = 0;
    cbus_ipcache_replace_dampened = 0;
    cbus_ipcache_invalidations = 0;

    if (cbus_switching_supported && (cbus = (mcitype *) cbusQ.qhead)) {
	if (cbus->flags & CBUSII) {
	    cbus_ip_cache_bytes = CBUSII_MAX_CE_BYTES;
	    cbus_rwcache_addr = CBUSII_RWCACHE_ADDR;
	    cbus_freecache_addr = CBUSII_FREECACHE_ADDR;
	} else {
	    cbus_ip_cache_bytes = CBUSI_MAX_CE_BYTES;
	    cbus_rwcache_addr = CBUSI_RWCACHE_ADDR;
	    cbus_freecache_addr = CBUSI_FREECACHE_ADDR;
	}
	for (i=0; i<NETHASHLEN; i++)
	    ipcache_cbtable[i] = &ipcache_cballoc[i * CBUS_BUCKET_SIZE];
	
	level = raise_interrupt_level(NETS_DISABLE);
	regptr = cbus->regptr;
	/*
	 * Select the proper hash table 
	 */
	if (FCI_OTHER_AUTO(cbus_switching_supported)) {
	    regptr->argreg = FCI_IP_HASH_TABLE;
	    regptr->cmdreg = FCI_SELECT_HASH_TABLE;
	}
	
	addr = cbus_cache_read(regptr, cbus_freecache_addr);
	
	cbentry = &ipcache_cballoc[CBUS_IPCACHE_ENTRIES-1];
	for (set=NETHASHLEN-1; set>=0; set--) {
	    regptr->argreg = 0;
	    regptr->cmdreg = FCI_KEY_SELECT;
	    regptr->rw_key = set;
	    for (i=0; i<CBUS_BUCKET_SIZE; i++, cbentry--) {
		if (cbentry->rnentry) {
		    /*
		     * Delete all references from system cache to cbus cache.
		     */
		    cbentry->rnentry->c.inval_ptr = NULL;
		    cbentry->rnentry = 0;
		}
		cbentry->destination = 0;
		cbentry->prefix_next = NULL;
		cbentry->updates = 0;
		
		regptr->argreg = btow(cbus_ip_cache_bytes);
		regptr->cmdreg = FCI_ALLOCATE_CACHE;
		if ((ustemp = regptr->cmdreg) != FCI_RSP_OKAY) {
		    buginf(msg_ipcache_err_msg, ustemp, "cache allocate",
			   msg_ipcache_add);
		    cbus_switching_supported &= ~FCI_CAP_IP_AUTO;
		    /*  
		     * we're disabled the cache,  let's flush it as well
                     */
		    regptr->argreg = 0x8000;	
		    regptr->cmdreg = FCI_CACHE_INVALIDATE;
		    DELAY(1);			/* delay one (1) millisecond */
		    if (regptr->cmdreg != FCI_RSP_OKAY) 
			cbus_switching_supported = 0;
		    reset_interrupt_level(level);
		    return;
		}
		cbentry->cbus_addr = addr;
		vaddr = addr + CACHE_VERSION;
		addr += btow(cbus_ip_cache_bytes);
		ustemp = cbus_cache_read(regptr, cbus_freecache_addr);
		if (ustemp != addr) {
		    buginf(msg_ipcache_err_msg, ustemp, "ucode CA_CUR_CA_PTR",
			   msg_ipcache_add);
		    cbus_switching_supported &= ~FCI_CAP_IP_AUTO;
		    /*  
		     * we're disabled the cache,  let's flush it as well
                     */
		    regptr->argreg = 0x8000;	
		    regptr->cmdreg = FCI_CACHE_INVALIDATE;
		    DELAY(1);			/* delay one (1) millisecond */
		    if (regptr->cmdreg != FCI_RSP_OKAY) 
			cbus_switching_supported = 0;

		    reset_interrupt_level(level);
		    return;
		}
		regptr->argreg = 1; /* key length */
		regptr->cmdreg = FCI_CACHE_ENTER;
		/*
		 * Learn the cbus's concept of a valid entry, then
		 * invalidate this entry.
		 */
		cbus_ipcache_valid_version = cbus_cache_read(regptr, vaddr);
		cbus_cache_write(regptr, vaddr, BOGUS_VERSION);
	    }
	}

	size = cbus_ip_cache_bytes * CBUS_IPCACHE_ENTRIES;
	cbus_cache_allocated += size;
	cbus_ipcache_allocated += size;
	reset_interrupt_level(level);
    }
}	

/*
 * cbus_ipcache_invalidate
 * Invalidate all cBus cache entries associated with this cache entry.
 *
 * In order to speed up invalidations by removing the requirement
 * to search the (possibly) 1000 entry table below, we link all entries
 * created from a single net prefix to that parent system cache entry
 * via the field "rnentry->c.inval_ptr".
 * The disadvantage to this is that the size of system and cbus shadowed
 * cache entries increases, as does the probability of fucking something
 * up in the system fast cache code.
 */

#define VALID_CBENTRY(cb) (((int)cb >= (int)ipcache_cballoc) && \
	((int)cb < ((int)&ipcache_cballoc[CBUS_IPCACHE_ENTRIES])))
void cbus_ipcache_invalidate (ipcache_rntype *rnentry)
{
    mcitype *cbus;
    mciregtype *regptr;
    ipcache_cbtype *cbentry;
    int err;
    leveltype level;

    /*
     * If microcode doesn't support the function -- exit.
     */
    cbentry = rnentry->c.inval_ptr;
    cbus = (mcitype *) cbusQ.qhead;
    if (!cbus || !cbentry || !(cbus_switching_supported & FCI_CAP_IP_AUTO))
	return;
    
    regptr = (mciregtype *) cbus->regptr;    

    level = raise_interrupt_level(NETS_DISABLE);

    rnentry->c.inval_ptr = NULL;
    for (err=0; cbentry && !err; cbentry = cbentry->prefix_next) {
	if (!VALID_CBENTRY(cbentry)) {
	    buginf("%s - out of range: %x", msg_ipcache_invalidate, cbentry);
	    err++;
	} else if (cbentry->rnentry != rnentry) {
	    buginf("%s - incoherent: rn=%x cbrn=%x",
		   msg_ipcache_invalidate, rnentry, cbentry->rnentry);
	    err++;
	} else if ((cbentry->destination & rnentry->mask) != rnentry->prefix) {
	    buginf("%s - incoherent: prefix=%i mask=%i dest=%i addr=%04x",
		   msg_ipcache_invalidate, rnentry->prefix, rnentry->mask,
		   cbentry->destination);
	    err++;
	} else {
	    cbus_cache_write(regptr, cbentry->cbus_addr + CACHE_VERSION,
			     BOGUS_VERSION);
	}
	/*
	 * Mark this entry as invalid and most suitable for re-use.
	 */
	cbentry->rnentry = NULL;
	cbentry->destination = 0;
	cbus_ipcache_invalidations++;
    }
    if (err) {
	cbus_switching_supported &= ~FCI_CAP_IP_AUTO;
	/*  
	 * we're disabled the cache,  let's flush it as well
	 */
	regptr->argreg = 0x8000;	
	regptr->cmdreg = FCI_CACHE_INVALIDATE;
	DELAY(1);			/* delay one (1) millisecond */
	if (regptr->cmdreg != FCI_RSP_OKAY) 
	    cbus_switching_supported = 0;
    }
    reset_interrupt_level(level);	
}

static const char rstring[] =
    "Hash  Destination     Age      Mods Addr ICB MAC Header";
/*
Hash  Destination     Age      Mods Addr ICB MAC Header
0A/0* 131.108.150.111 00:00:00 0100 21C0 060 4050123456789ABC123456789ABC
 */
void cbus_ipcache_show (ipaddrtype prefix, ipaddrtype mask)
{
    mcitype *cbus;
    mciregtype *regptr;
    ipcache_cbtype *cbentry, cb;
    ipaddrtype dest;
    charlong dst;
    ulong total, hits;
    ushort mac[7], link, vers, mlen, dicb;
    int i, j, k;
    leveltype level;
    char time[20], c;
    boolean cbus_switching;
    hwidbtype *hwidb;

    cbus = (mcitype *) cbusQ.qhead;
    if (!cbus || !(cbus_switching_supported & FCI_CAP_IP_AUTO))
	return;

    /*
     * Don't bother displaying the L2 cache if no one's using cbus switching.
     */
    cbus_switching = FALSE;

    FOR_ALL_HWIDBS(hwidb)
	cbus_switching |= ((hwidb->ip_routecache & IP_CBUS_ROUTECACHE) &&
			   hw_interface_up(hwidb));

    if (!cbus_switching)
	return;

    regptr = (mciregtype *) cbus->regptr;
    
    total = cbus_ipcache_misses + cbus_ipcache_hits;
    hits = cbus_ipcache_hits;
    if (hits < 40000000)
	hits *= 100;
    else
	total /= 100;
    if (!total)
	total = 1;

    automore_enable(NULL);

    printf("\nCBUS Cache: %d hits, %d misses (%d%% hit ratio)",
	   cbus_ipcache_hits, cbus_ipcache_misses, hits/total);
    printf("\n  %d added, %d replaced, %d invalidated, %d resets",
	   cbus_ipcache_adds, cbus_ipcache_replaced,
	   cbus_ipcache_invalidations, cbus_ipcache_resets);
    printf("\n  %d dampened replacements, %d dampened re-populates\n",
	   cbus_ipcache_update_dampened, cbus_ipcache_replace_dampened);
    
    automore_header(rstring);
    for (i = 0; i < NETHASHLEN; i++) {
	cbentry = ipcache_cbtable[i];
	for (j=0; j < CBUS_BUCKET_SIZE; j++, cbentry++) {
	    level = raise_interrupt_level(NETS_DISABLE);
	    dest = cbentry->destination;
	    if (!dest || (dest & mask) != prefix) {
		reset_interrupt_level(level);
		continue;
	    }
	    cb = *cbentry;
	    cbus_cache_select(regptr, cbentry->cbus_addr);
	    link = regptr->rw_cache;
	    vers = regptr->rw_cache;
	    mlen = regptr->rw_cache;
	    dicb = regptr->rw_cache;
	    dst.d.sword[0] = regptr->rw_cache;
	    dst.d.sword[1] = regptr->rw_cache;
	    for (k=0; k<7; k++)
		mac[k] = regptr->rw_cache;
	    reset_interrupt_level(level);
	    c = ' ';
	    if (((ulong)dest != dst.d.lword) ||
		(vers != cbus_ipcache_valid_version))
		c = '?';
	    sprint_dhms(time, cb.last_update);

	    printf("\n%02x/%d%c %15i %8s %04x %04x %03x ",
		   i, j, c, dest, time, cb.updates, cb.cbus_addr, dicb);
	    for (k=0; k<7 && k<mlen; k++)
		printf("%04x", mac[k]);
	} 
    }
    automore_disable();
}


static void ip_cbus_init (subsystype* subsys)
{
    reg_add_auto_sw_cache_init(LINK_IP, cbus_ipcache_init,
				  "cbus_ipcache_init");
    reg_add_auto_sw_ip_cache_update(cbus_ipcache_update,
				    "cbus_ipcache_update");
    reg_add_ipcache_delete_callback(cbus_ipcache_invalidate,
					"cbus_ipcache_invalidate");
    reg_add_auto_sw_ip_cache_show(cbus_ipcache_show,
					"cbus_ipcache_show");
    registry_add_list(REG_FS, SERVICE_AUTO_SW_CACHE_INIT_CALLBACK,
		      cbus_ipcache_init_callback,
		      "cbus_ipcache_init_callback");

    cbus_routecache_parser_init();

    /*
     * Make sure CBUS is ready:
     */
    cbus_ipcache_init();
}


/*
 * IP Autonomous switching subsystem header
 */ 
#define CBUSIP_MAJVERSION 1
#define CBUSIP_MINVERSION 0
#define CBUSIP_EDITVERSION 1

SUBSYS_HEADER(ip_cbus_fs, CBUSIP_MAJVERSION, CBUSIP_MINVERSION,
	      CBUSIP_EDITVERSION, ip_cbus_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost, subsys_req_iphost);
