/* $Id: at_arp.c,v 3.9.18.11 1996/09/04 22:52:42 snyder Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_arp.c,v $
 *------------------------------------------------------------------
 * atalk_arp.c -- Appletalk ARP (AARP)
 *
 * December 1988, Bill Palmer
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_arp.c,v $
 * Revision 3.9.18.11  1996/09/04  22:52:42  snyder
 * CSCdi68101:  declare things that should be const in sub_atalk
 *              Little one 36 from data, 8 from image
 * Branch: California_branch
 *
 * Revision 3.9.18.10  1996/08/28  12:36:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.18.9  1996/07/18  17:11:20  kleung
 * CSCdi63237:  Spurious memory access  aarp_enqueue(0x6050234c)+0x28
 * Branch: California_branch
 *
 * Revision 3.9.18.8  1996/07/02  23:10:06  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.9.18.7  1996/06/18  01:37:33  kleung
 * CSCdi59554:  Hey! you reply to a probe! messages on console.
 * Branch: California_branch
 *
 * Broke compilation, incorrect type to is_atm_lane().
 *
 * Revision 3.9.18.6  1996/06/17  22:18:56  kleung
 * CSCdi59554:  Hey! you reply to a probe! messages on console.
 * Branch: California_branch
 *
 * Revision 3.9.18.5  1996/06/05  00:44:59  kleung
 * CSCdi57957:  Unsolicited message should use errmsg facility
 * Branch: California_branch
 *
 * Revision 3.9.18.4  1996/05/10  01:36:02  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.9.18.3  1996/05/04  00:33:46  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Atalk protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on IRB/CRB is enabled or not.
 *
 * Revision 3.9.18.2  1996/03/23  01:25:41  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.9.18.1  1996/03/18  18:51:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/13  01:06:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/03/07  08:28:13  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  00:16:32  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/08  23:07:57  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.8  1996/02/08  03:18:28  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.7  1996/01/16  18:50:58  slin
 * CSCdi46968:  AARP scheduler error
 *
 * Revision 3.6  1995/12/08  04:55:09  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.5  1995/12/04  21:24:48  slin
 * CSCdi44934:  AT ARP scheduler error
 * - Missing break in switch statement.
 *
 * Revision 3.4  1995/11/23  06:50:20  hampton
 * Reduce the memory bloat from 64 bit timers.  [CSCdi44415]
 *
 * Revision 3.3  1995/11/17  08:41:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:56:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:49:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/20  19:04:12  slin
 * CSCdi42522:  Remove use of if_number from AppleTalk
 *
 * Revision 2.5  1995/07/13  06:49:20  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.4  1995/06/23 17:36:46  bbenson
 * CSCdi36320:  AARP Probe is needed for LANE
 * - Generate response to AARP_PROBE in the event that IS_ATM_LANE is true.
 *
 * Revision 2.3  1995/06/09  12:58:25  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.2  1995/06/08 21:42:54  rlfs
 * CSCdi33586:  SMDS ATALK AARP frames use the wrong ADDR TYPE value for
 * SMDS
 *
 * Revision 2.1  1995/06/07  20:07:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include "mgd_timers.h"
#include "../os/signal.h"
#include "interface_private.h"
#include "packet.h"
#include "plural.h"
#include "sys_registry.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/arp.h"
#include "atalk_private.h"
#include "at_arp.h"
#include "at_registry.h"

enum {
    ATTIMER_AARP_SCAN,
    ATTIMER_AARP_AGER
};


/*
 * Public data definitions.
 */
ulong aarp_probe_count;			/* number of times to send arp pkts */
ulong aarp_probe_interval;		/* interval in ms between arp pkts */
ulong aarp_request_count;		/* number of times to send arp pkts */
ulong aarp_request_interval;		/* interval in ms between arp pkts */
ulong aarp_request_count;		/* number of times to send arp pkts */
ulong aarp_request_interval;		/* interval in ms between arp pkts */

/*
 * Private data definitions.
 */
static watched_queue *aarpQ;		/* AppleTalk ARP packet queue */
static boolean aarp_reply_flag;		/* send a request to this packet */
static mgd_timer aarp_next_scan; /* clock tick of next aarp pak scan */
static mgd_timer aarp_background_timer;
static mgd_timer aarp_ager_timer;
static rbTree*	aarp_tree;
static uchar const zero_hw_addr[STATIONLEN_MAX]; /* Max-length zero'd MAC */
						 /* address */
static ulong aarp_has_waiting_pak;	/* Number of pak's waiting for MAC */
static ulong aarp_gc_limit;		/* Max # of free'd AARP entries */

/*
 * Private data declarations.
 */
static boolean		aarp_canonicalize(aarpOps_t *);
static void		send_aarp_reply(paktype*, aarpOps_t *);
static void		unset_temp_aarp(uchar const *,ataddrtype,
					atalkidbtype*);
static boolean		aarp_scan_arp_table(void);
static void		aarp_set_ptrs(paktype *, aarp_ptrs_t *);
static boolean  	aarp_validate(aarpOps_t *);
static boolean  	aarp_preValidate(paktype *);
static aarpentry_t *	aarp_CreateEntry(ataddrtype, atalkidbtype *);
static void		aarp_DeleteEntry(aarpentry_t *);
static void             aarp_GC(rbTree *);



/* inline declarations */
static inline aarpentry_t *  
aarpCast (treeLink* link) { 
    return (aarpentry_t*)link; 
}

static inline void
aarp_ForAllEntriesDo (treeProc proc,void* pdata)
{ 
    if (aarp_tree != NULL) 
	RBTreeForEachNode(proc, pdata, aarp_tree, TRUE); 
}

static inline void
aarp_ProtectEntry (aarpentry_t* e, boolean lockIt) 
{ 
    (void) RBTreeNodeProtect(&e->link, lockIt);
}

/*
 * aarp_UnprotectEntry
 * Allow a entry to be freed or reused.
 */
static inline void
aarp_UnprotectEntry (aarpentry_t* e) 
{ 
    (void)RBTreeNodeProtect(&e->link,FALSE); 
}

static inline boolean
aarp_EntryProtected (aarpentry_t* e)
{
    return (RBTreeNodeProtected((treeLink *)e));
}

/*
 * aarp_EntryKey
 * Create a treeKey for an entry which is always unique.
 */
static inline treeKey
aarp_EntryKey (atalkidbtype* idb,ataddrtype src)
{
    treeKey key;

    /* 
     * Use 64 bits key to support more than 256 interfaces
     */
    key.ll = idb->phys_idb->if_number;
    key.ll <<= 24;
    key.ll |= (src & ATALK_ADDR_MASK);
    return (key);
}

static inline boolean
aarp_InsertEntry (aarpentry_t* e)
{
    treeKey key = aarp_EntryKey(e->idb, e->proto_addr);
    return (RBTreeInsert(key, aarp_tree, &e->link) != NULL);
}

static inline aarpentry_t*
aarp_GetFirstEntry (void)
{
    if (aarp_tree == NULL)
	return (NULL);

    return (aarpCast(RBTreeFirstNode(aarp_tree)));
}


static inline aarpentry_t*
aarp_GetBestEntry (treeKey *key)
{
    return (aarpCast(RBTreeBestNode(aarp_tree, *key)));
}


/*
 * GetNextEntry - Search for the next neighbor and return it w/o protection
 * NOTE: An important difference between this and other tree "GetNext"
 *       routines is that this does not UnprotectEntry the previous entry.
 */
aarpentry_t*
aarp_GetNextEntry (aarpentry_t* e)
{
    aarpentry_t* next = e;

    if (aarp_tree == NULL)
	return (NULL);
    if (next == NULL) {
	next = aarpCast(RBTreeFirstNode(aarp_tree));
    } else {
	next = aarpCast(RBTreeNextNode(&e->link));
    }
    return (next);
}


/*
 * ValidAARP
 * Check if AARP entry is valid for prevailing conditions
 */
static boolean
ValidAARP (aarpentry_t* e) 
{
    if (!atalkif_InterfaceUp(e->idb))
	return (FALSE);
    if (!atalkif_ValidAddress(e->idb,e->proto_addr))
	return (FALSE);
    return (TRUE);
}

/*
 * aarp_hwtype
 * Return the appropriate magic number per the appropriate spec
 */
static aarp_hw_t
aarp_hwtype (hwidbtype* idb, int method)
{
    if ((idb->status & (IDB_ETHER|IDB_LEX|IDB_ATM|IDB_BVI)) && 
	(method != ET_ATM))
	return (AARP_HWETHER);
    if (idb->status & IDB_FDDI)
	return ((idb->atalk_compat_flags == AT_COMPAT_FDDI)
	       ? AARP_HWOLDFDDI
	       : AARP_HWFDDI);
    if (idb->status & IDB_TR)
	return (AARP_HWTR);
    if (idb->status & IDB_SERIAL && method == ET_SMDS)
        return (AARP_HWSMDSTALK);
    if ((is_ppp(idb) && idb->ppp_bridge_atalk) ||
	(is_cpp(idb)))
	return(AARP_HWETHER);
    if ( is_frame_relay(idb) || is_atm_dxi(idb)) 
	return (AARP_HWFR); 
    return (AARP_HWUNK);
}


/*
 * aarp_FreeResources
 * Cleanup an AARP entry by releasing any resources associated with it.
 * Note that this does not delete the AARP entry from the tree, only releases
 * any resources hanging off the entry.
 */
static void
aarp_FreeResources (aarpentry_t* e)
{
    if (e->unarped_pak != NULL) {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, e->idb->phys_idb)) {
	    buginf("\n%s: AARP: Discarding packet for %a after %d attempt%s",
		   idb_get_namestring(e->idb->phys_idb), e->proto_addr,
		   ArgAndPlural(e->arp_retries,"","s"));
	}
	e->unarped_pak->enc_flags = 0;
	atalk_dispose(ATALK_ENCAPSFAILED, e->unarped_pak);
	atalk_stat[ATALK_ARP_DROPS]++;
	e->unarped_pak = NULL;
    }
}


/*
 * aarp_hwlen
 * Return the appropriate hardware length per the appropriate spec
 */
static ushort
aarp_hwlen (hwidbtype* idb, int method)
{
    if (idb->status & IDB_SERIAL) {
	if (method == ET_SMDS)
	    return (HL_SMDS_RFC);
	else if (method == ET_FRAME_RELAY)
	    return (HL_ETH);		/* ??? How do we arrive @ this */
	else if (method == ET_X25)
	    return (HL_X25);
    } else if (idb->status & (IDB_ETHER | IDB_FDDI | IDB_TR | IDB_LEX)) {
	return (IEEEBYTES);
    }
    return (IEEEBYTES);			/* assume 802 media is default */
}


static void
aarp_Shutdown (void)
{
    flush_all_aarp(0);
    aarp_GC(aarp_tree);
}

/*
 * aarp_init
 * Initialize the AppleTalk ARP support
 */
void
aarp_init (void)
{
    /*
     * Initialize the probe & request defaults.
     */
    aarp_probe_count      = AARP_PROBE_RETRANSMIT;
    aarp_probe_interval   = AARP_PROBE_INTERVAL;
    aarp_request_count    = AARP_REQUEST_RETRANSMIT;
    aarp_request_interval = AARP_REQUEST_INTERVAL;

    mgd_timer_init_parent(&aarp_background_timer, NULL);
    mgd_timer_init_leaf(&aarp_next_scan, &aarp_background_timer,
			ATTIMER_AARP_SCAN, NULL, FALSE);
    mgd_timer_init_leaf(&aarp_ager_timer, &aarp_background_timer,
			ATTIMER_AARP_AGER, NULL, FALSE);
    
    aarp_has_waiting_pak  = 0;
    aarp_gc_limit         = AARP_GC_LIMIT;
    aarp_tree = RBTreeCreate("Appletalk", "AT", "AARP Tree",
			     at_print_key_long_idb_address, 
			     &appletalk_debug->flag);
    RBTreeNon32Bit(aarp_tree, RBTreeKeyMaxLongLong, RBTreeKeyCompareLongLong);
    reg_add_atalk_shutdown(aarp_Shutdown, "aarp_Shutdown");
}


/*
 * aarp_set_ptrs
 *
 * Set a aarp_ptrs structure to point to the correct places in a 
 * variable-length AARP packet.
 *
 * Assumptions:
 * - the AARP packet address passed in really is large enough to contain
 *   the packet described in the AARP packet header.
 * - The source and target addresses are the same size.
 */
static void
aarp_set_ptrs (paktype *pak, aarp_ptrs_t *ptrs)
{
    uchar	hw_len;			/* length of h/w addresses */
    uchar	pt_len;			/* length of proto addresses */
    aarp_hdr   *aarp;

    aarp = aarpheadstart(pak);
    
    hw_len = aarp->arhln;
    pt_len = aarp->arpln;
    
    ptrs->src_hw_addr   = aarp->ipshdw;
    ptrs->src_pt_addr   = ptrs->src_hw_addr + hw_len;
    ptrs->src_pt_net    = ptrs->src_pt_addr + AARP_PADBYTES;
    ptrs->src_pt_node   = ptrs->src_pt_net  + sizeof(short);
    
    ptrs->dst_hw_addr   = ptrs->src_pt_addr + pt_len;
    ptrs->dst_pt_addr   = ptrs->dst_hw_addr + hw_len;
    ptrs->dst_pt_net    = ptrs->dst_pt_addr + AARP_PADBYTES;
    ptrs->dst_pt_node   = ptrs->dst_pt_net  + sizeof(short);
}


/*
 * aarp2ops
 *
 * break out an incoming aarp packet of unknown hw addr length into
 * an 'ops' structure
 *
 * This routine returns TRUE in all instances other than a bogus AARP
 * packet type was found during the decode.
 */
static boolean
aarp_aarp2ops (paktype *pak, aarpOps_t *ops)
{
    aarp_ptrs_t	ptrs;
    aarp_hdr   *aarp;

    if (!(pak && ops))
	return (FALSE);

    aarp = aarpheadstart(pak);

    /*
     * If the hardware length is more than we can deal with, bail.
     */
    if (aarp->arhln > sizeof(ops->srcHwAddr)) {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, pak->if_input))
	    buginf("\nat_arp: AARP packet hwlen > alloc'd hw size in aarp2ops");
	return (FALSE);
    }
    
    aarp_set_ptrs(pak, &ptrs);
    memset(ops, 0, sizeof(aarpOps_t));
    ops->pak       = pak;
    ops->opcode    = GETSHORT(&aarp->opcode);
    ops->hw_type   = GETSHORT(&aarp->arhrd);
    ops->protoType = GETSHORT(&aarp->arpro);
    ops->ptAddrLen = aarp->arpln;
    ops->hwAddrLen = aarp->arhln;

    /*
     * If the input interface is a BVI(Bridge-group Virtual 
     * Interface) and the original hw_type is token, bit-swap the 
     * src hardware address(src_hw_addr) *inside* the packet since 
     * the BVI is a virtual-ether interface,
     * - we want the mac in ARP cache is in canonical format
     * - we may decide to send ARP resopnse by using this packet
     *   later, then we copy src_hw_addr as the destination MAC.
     *   For BVI, it should in canonical(ether) format
     *
     * This is for IRB(Integrated Routing and Bridging)
     */
    if (is_bvi(hwidb_or_null(pak->if_input)) && (ops->hw_type == AARP_HWTR))
	ieee_swap(ptrs.src_hw_addr, ptrs.src_hw_addr);
    
    bcopy(ptrs.src_hw_addr, ops->srcHwAddr, ops->hwAddrLen);

    switch (ops->opcode) {
    case OP_AARP_REQUEST:
    case OP_AARP_PROBE:
    case OP_AARP_REPLY:
	ops->srcPtAddrNet = GETSHORT(ptrs.src_pt_net);
	ops->srcPtAddrNode = *ptrs.src_pt_node;
	ops->dstPtAddrNet = GETSHORT(ptrs.dst_pt_net);
	ops->dstPtAddrNode = *ptrs.dst_pt_node;
	ops->srcPtAddr = atalk_address(ops->srcPtAddrNet, ops->srcPtAddrNode);
	ops->dstPtAddr = atalk_address(ops->dstPtAddrNet, ops->dstPtAddrNode);
	break;
    default:
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, pak->if_input))
	    buginf("\n%s at_arp: bogon of type %d in aarp2ops from %e",
		   ops->pak->if_input->namestring, ops->opcode, ops->srcHwAddr);
	atalk_stat[ATALK_ARP_BOGON]++;
	return (FALSE);
    }
    if (ops->opcode == OP_AARP_REPLY)
	bcopy(ptrs.dst_hw_addr, ops->dstHwAddr, ops->hwAddrLen);
    
    return (TRUE);
}

/*
 * aarp_ops2aarp
 *
 * fill in a given aarp packet with the AARP infomation in a given ops
 * structure. This function lays in the data from the Source HW address
 * entry on down. 
 *
 * NOTE: it is assumed that the aarp packet has been allocated big enough
 * to handle the HW addresses involved. In other words, don't pass a ethernet
 * aarp buffer in with a X.25 ops structure.
 */
boolean
aarp_ops2aarp (aarpOps_t *ops, paktype *pak)
{
    aarp_ptrs_t	ptrs;
    aarp_hdr    *aarp;
    uchar	hw_len;
    uchar	pt_len;
    
    if (!(ops && pak))
	return (FALSE);

    aarp = aarpheadstart(pak);
    
    aarp_set_ptrs(pak, &ptrs);
    hw_len = aarp->arhln = ops->hwAddrLen;
    pt_len = aarp->arpln = ops->ptAddrLen;
    bcopy(ops->srcHwAddr, ptrs.src_hw_addr, hw_len);
    /*
     * Note that the first (most sig.) byte of the AppleTalk protocol
     * address must be zero. It's easier to simply zero out the entire
     * protocol address field...
     */
    PUTLONG(ptrs.src_pt_addr, 0);
    PUTSHORT(ptrs.src_pt_net, ops->srcPtAddrNet);
    *ptrs.src_pt_node = ops->srcPtAddrNode;
    memset(ptrs.dst_hw_addr, 0, hw_len);
    memset(ptrs.dst_pt_addr, 0, pt_len);
    
    switch (ops->opcode) {
    case OP_AARP_REQUEST:
	PUTSHORT(ptrs.dst_pt_net, ops->dstPtAddrNet);
	*ptrs.dst_pt_node = ops->dstPtAddrNode;
	break;
    case OP_AARP_REPLY:
    case OP_AARP_PROBE:
	bcopy(ops->dstHwAddr, ptrs.dst_hw_addr, hw_len);
	PUTSHORT(ptrs.dst_pt_net, ops->dstPtAddrNet);
	*ptrs.dst_pt_node = ops->dstPtAddrNode;
	break;
    default:
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, ops->pak->if_input))
	    buginf("\nAT: aarp_ops2aarp, in default:, opcode %d", ops->opcode);
	return (FALSE);
	break;
    }
    return (TRUE);
}

/*
 * aarp_enqueue
 * Handle AARP requests at interrupt level.
 */
void
aarp_enqueue (paktype* pak)
{
    /*
     * If Appletalk if running and is enabled on the incoming interface, then
     * handle this AARP packet.
     * Otherwise, if it was simply a case of AppleTalk not running on this
     * particular interface, then abort the packet. Queue the packet
     * on the netinputQ whether Atalk is running or not. Note that
     * we abort the packet IFF AppleTalk is running. If Atalk isn't running,
     * then the Net background process will try handing the packet over to
     * bridging.
     */
    atalkidbtype *idb;

    idb = atalk_getidb(pak->if_input);
    
    if (atalk_running && idb && (idb->atalk_enabled))
	process_enqueue(aarpQ, pak);
    else {
	protocol_discard(pak, !(BRIDGING_IF(pak->if_input, LINK_APPLETALK)));
    }
}

/*
 * atalk_ifarp
 * Add our interface to the arp table.
 */
void
atalk_ifarp (atalkidbtype* idb, ataddrtype addr)
{
    hwidbtype	*hwidb;

    if (!atalkif_InterfaceUp(idb))
	return;
    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == NULL)
	return;
    if ((idb->aarp_enctype != ET_NULL) && (idb->atalk_enabled)) {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) {
	    buginf("\nAARP: creating entry for %s, %a(%e)",
		   idb_get_namestring(idb->phys_idb),
		   atalkif_MyAddress(idb),
		   hwidb->hardware);
	}
	aarp_insert(idb, addr, hwidb->hardware,
		    aarp_hwlen(hwidb, idb->aarp_enctype),
		    idb->aarp_enctype, AARP_INTERFACE);
    }
}

/*
 * aarp_fix
 * Add all interfaces back into the table after they were cleared.
 * ??? Doesn't seem to be called by anyone.
 */
void
aarp_fix (void)
{
    atalkidbtype *idb;
    
    atalk_fscache_flush();		/* Flush the AT FS cache */
    if (!atalk_running)
	return;
    FOR_ALL_ATIDBS(idb)
	atalk_ifarp(idb, atalkif_MyAddress(idb));
}

static void
aarp_GC (rbTree *tree)
{
    aarpentry_t *e;

    if (tree == NULL)
	return;

    tree->gcRuns++;
    if (!RBTreeValid(tree, TRUE, "aarp_GC")) {
	if (!TIMER_RUNNING_AND_SLEEPING(tree->errTimer)) {
	    errmsg(SYS_ERR_TABLEERR, "Appletalk", tree->tableName,
		   tree->lastErr);
	    TIMER_START(tree->errTimer, 5 * ONEMIN);
	}
    }
    while ((e = aarpCast(RBTreeGetFreeNode(tree))) != NULL) {
	free(e);
    }
}

static void
aarp_background_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_mgd_timer(&aarp_background_timer, DISABLE);
    process_watch_queue(aarpQ, DISABLE, RECURRING);
    reg_delete_raw_enqueue(LINK_AARP);
    while ((pak = process_dequeue(aarpQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&aarpQ);
    aarp_pid = 0;
}

/*
 * aarp_background
 * Receive and process AppleTalk ARP requests
 */
process
aarp_background (void)
{
    paktype	*aarp;			/* incoming aarp packet ptr */
    aarpOps_t	ops;
    hwidbtype	*hwidb;
    ulong       major, minor;
    mgd_timer  *expired_timer;
    
    process_wait_on_system_init();
    
    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, aarp_background_teardown);
    aarpQ = create_watched_queue("AARP packets", 0, 0);
    process_watch_queue(aarpQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&aarp_background_timer, ENABLE);
    
    mgd_timer_start(&aarp_ager_timer, ONEMIN);
    reg_add_raw_enqueue(LINK_AARP, aarp_enqueue, "aarp_enqueue");
    
    while (TRUE) {
	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case TIMER_EVENT:
		while (mgd_timer_expired(&aarp_background_timer)) {
		    expired_timer = mgd_timer_first_expired(&aarp_background_timer);
		    switch (mgd_timer_type(expired_timer)) {
		    case ATTIMER_AARP_SCAN:
			/*
			 * Check for packets which are stalled, waiting for
			 * a MAC address.
			 */
			mgd_timer_stop(expired_timer);
			if (aarp_has_waiting_pak > 0)
			    aarp_scan_arp_table();
			break;
		    case ATTIMER_AARP_AGER:
			aarp_ager();
			mgd_timer_start(expired_timer, ONEMIN);
			break;
		    default:
			mgd_timer_stop(expired_timer);
			break;
		    }
		}
		break;
		
	    case QUEUE_EVENT:
		while ((aarp = process_dequeue(aarpQ)) != NULL) {
		    hwidb = hwidb_or_null(aarp->if_input);
		    if (hwidb == NULL) {
			datagram_done(aarp);
			continue;
		    }

		    idb_increment_rx_proto_counts(aarp, ACCT_PROTO_APPLE);
		    aarp->acct_proto = ACCT_PROTO_APPLE;
	    
		    if (!aarp_preValidate(aarp)) {
			datagram_done(aarp);
			continue;
		    }
		    /*
		     * Decode the AARP packet from the media format into our
		     * internal media independent form.
		     */
		    if (!aarp_aarp2ops(aarp, &ops)) {
			datagram_done(aarp);
			continue;
		    }
		    if (!aarp_validate(&ops)) {
			datagram_done(aarp);
			continue;
		    }
		    if (aarp_canonicalize(&ops)) {
			atalkidbtype *idb;
			idb = atalk_getidb(ops.pak->if_input);
			aarp_insert(idb, ops.srcPtAddr,
				    ops.srcHwAddr, ops.hwAddrLen,
				    idb->aarp_enctype, AARP_DYNAMIC);
		    }
		    
		    if (aarp_reply_flag == FALSE)
			datagram_done(aarp);
		    else
			send_aarp_reply(aarp, &ops);
		}
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
	
	/*
	 * Check for AARP entries which have been free'd in excess of the
	 * GC limit.
	 */
	if (RBTreeReleasedNodeCount(aarp_tree) > aarp_gc_limit) {
	    aarp_GC(aarp_tree);
	}
    }
}

/*
 * aarp_scan_arp_table
 *
 * Scans the ARP table, looking for AppleTalk entries which need
 * AARP requests sent to resolve a AppleTalk address so that
 * we can send a pending packet.
 *
 * The AARP response (hopefully) generated by this function will be
 * processed later by aarp_merge and merge; what will then happen is
 * that a companion ARP table-walking function will send the packets
 * that are pending.
 *
 * Returns
 *	TRUE if we sent any aarp_resolves
 *	FALSE if not
 *
 * Side Effects:
 *	AARP requests may be sent
 *	AppleTalk pending packets may be sent
 */
boolean
aarp_scan_arp_table (void)
{
    aarpentry_t	*e;
    aarpentry_t	*next;
    paktype	*pak;
    boolean	res_done = FALSE;
    boolean	delete_it;
    ulong	num_res_done = 0;
    
    /*
     * Walk the entire ARP table. When we find an
     * AppleTalk entry which is waiting for a MAC
     * address, we send an aarp_resolve.
     */
    if (   at_debug(aarp_debug, ATALK_NULL_MASK)
	&& at_debug(atalkerr_debug, ATALK_NULL_MASK))
	buginf("\nAARP: starting scan for waiting AARP entries");

    aarp_has_waiting_pak = 0;	/* reset counter */
    
    if ((e = aarp_GetNextEntry(NULL)) == NULL) {
	if (at_debug(aarp_debug, ATALK_NULL_MASK))
	    buginf("\nAARP: No waiting entries found, returning");
	return (FALSE);
    }
    
    while (e) {
	aarp_ProtectEntry(e, TRUE);
	delete_it = FALSE;
	if (e->type == AARP_WAITING) {
	    if (e->arp_retries > aarp_request_count) {
		if (at_debug(aarp_debug, ATALK_INTERFACE_MASK,
			     e->idb->phys_idb)) {
		    buginf("\nAARP: waiting entry for %a exceeded r-count",
			   e->proto_addr);
		}
		/*
		 * This entry has had it's chance. Nuke it.
		 */
		delete_it = TRUE;
	    } else if (e->unarped_pak != NULL) {
		e->arp_retries++;
		aarp_has_waiting_pak++;
		res_done = aarp_resolve(e->proto_addr,
					e->idb,
					e->enctype,
					FALSE);
		if (at_debug(aarp_debug, ATALK_INTERFACE_MASK,
			     e->idb->phys_idb))
		    buginf("\n%s: AARP: %s resolve for %a",
			   idb_get_namestring(e->idb->phys_idb),
			   res_done ? "Sent" : "Couldn't",
			   e->proto_addr);
		if (res_done)
		    ++num_res_done;
	    }
	}
	/*
	 * If this entry reads as 'dynamic' and we
	 * still have a pointer to an unarp'ed pak,
	 * then send the packet; but mark it as having
	 * flunked once, so if it flunks again, it doesn't
	 * loop in this logic.
	 */
	if ((e->type == AARP_DYNAMIC) && e->unarped_pak) {
	    pak = e->unarped_pak;
	    e->unarped_pak = NULL;
	    pak->enc_flags = ENC_FAILED_ONCE;
	    pak->if_output = e->idb->phys_idb;
	    if (atalk_send1(pak,e->proto_addr))
		atalk_stat[ATALK_ARP_DELAYS]++;
	    else
		atalk_stat[ATALK_ARP_DROPS]++;
	}
	next = aarp_GetNextEntry(e);
	aarp_UnprotectEntry(e);
	if (delete_it) {
	    aarp_FreeResources(e);
	    aarp_DeleteEntry(e);
	}
	e = next;
    }					/* while (e) */

    if (aarp_has_waiting_pak > 0) {
	mgd_timer_start(&aarp_next_scan, aarp_request_interval);
    } else {
	mgd_timer_stop(&aarp_next_scan);
    }

    return (num_res_done > 0);
}					/* aarp_scan_arp_table */

/*
 * aarp_preValidate
 *
 * Here we perform preliminary packet validation.
 * Preliminary validation includes link-layer stuff; proper encapsulation,
 * correct packet setups, etc.
 *
 * Note: the proper way this routine accounts for bogus packets is
 * that we increment the traffic counter in this routine and let the
 * higher layer do the return of the buffer.
 */
boolean
aarp_preValidate (paktype *pak)
{
    atalkidbtype	*idb;
    hwidbtype   *hwidb;
    aarp_hdr    *aarp;
    snap_hdr    *snap;
    aarp_hw_t    hwtype;
    char const  *idb_name;
    
    aarp = aarpheadstart(pak);

    if (aarp == NULL)
	return (FALSE);
    idb = atalk_getidb(pak->if_input);
    if (idb == NULL)
	return (FALSE);
    hwidb = hwidb_or_null(pak->if_input);
    if (hwidb == NULL)
	return (FALSE);
    idb_name = idb_get_namestring(idb->phys_idb);
    
    /*
     * Screen out multicast addresses
     */

    /*
     * If the input interface is a BVI(Birdge-group Virtual 
     * Interface), find it out from the real received bridged
     * interface.
     * For IRB(Integrated Routing and Bridging) support
     */
    if (is_bvi(hwidb)) {
	if (!pak->irb_rcvd_input)
	    return (FALSE);
	hwtype = aarp_hwtype(hwidb_or_null(pak->irb_rcvd_input), 
			     idb->aarp_enctype);
    } else {
	hwtype = aarp_hwtype(hwidb, idb->aarp_enctype);
    }

    if ((hwtype == AARP_HWETHER && aarp->ipshdw[0] & ETHER_MAC_MULTICAST)
	|| (hwtype == AARP_HWTR && aarp->ipshdw[0] & TRING_MAC_MULTICAST)) {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\n%s AARP: Source address has mcast bit set %e",
		   idb_name, aarp->ipshdw);
	atalk_stat[ATALK_ARP_BOGON]++;
	return (FALSE);
    }
    
    /*
     * Packet encapsulation doesn't match the prescribed AARP
     * encaps on the incoming interface. This is a major boo-boo.
     */
    if (pak->enctype != idb->aarp_enctype) {		
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) {
	    buginf("\n%s: AARP: Wrong packet encapsulation from %e "
		   "(found %s,expected %s)",
		   idb_name, aarp->ipshdw, encstring(pak->enctype),
		   encstring(idb->aarp_enctype));
	    /*
	     * This situation isn't entirely unexpected on ethernets;
	     * it could be that there is a transistion routers out there
	     * which is leaking these bogus AARP packets to us.
	     */
	}
	atalk_stat[ATALK_ARP_BADENCS]++;
	return (FALSE);
    }
    
    /*
     * If the packet was coming in on a extended 802.foo interface,
     * enforce the correct Apple/ISO2 SNAP.
     */
    if (pak->enctype == ET_SNAP)
	if ((snap = (snap_hdr *)(pak->info_start)) &&
	    (!ieee_equal_oui(snap->oui, zerosnap))) {
	    /* Enforce a zero snap for future compatibility */
	    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) {
		buginf("\n%s: AARP: Invalid SNAP value (%e) in packet from %e",
		       idb_name, snap->oui, aarp->ipshdw);
	    }
	    atalk_stat[ATALK_ARP_BADENCS]++;
	    return (FALSE);
	}
    
    return (TRUE);
}

/*
 * aarp_validate
 *
 * Validate a incoming AARP packet. Returns TRUE if the packet meets validity
 * criteria, FALSE if not. Caller decides what to do with packet.
 */
static boolean
aarp_validate (aarpOps_t *ops)
{
    atalkidbtype	*idb;	/* interface on which we heard pak */
    hwidbtype	*hwidb, *rcvd_hwidb;
    ushort tnet = 0;			/* target network # */
    ushort snet = 0;			/* source network # */
    ataddrtype	tpaddr;
    ataddrtype	spaddr;
    boolean	bogon;			/* bogus incoming packet */
    uchar  idb_hw[STATIONLEN_MAX];	/* idb hw address */
    ushort hw_len;			/* length of idb hw address */
    char const *idb_name;
    
    if (ops == NULL)
	return (FALSE);
    idb = atalk_getidb(ops->pak->if_input); /* Grab data from packet */
    if (idb == NULL)
	return (FALSE);
    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == NULL)
	return (FALSE);
    idb_name = idb_get_namestring(idb->phys_idb);
    
    /* Check Hardware type field */

    /*
     * If the input is a BVI(Bridge-group Vitual Interface),
     * find out the real received interface.
     * For IRB(Integrated Routing and Bridging) support
     */
    if (is_bvi(hwidb)) {
	rcvd_hwidb = hwidb_or_null(ops->pak->irb_rcvd_input);
	if (!rcvd_hwidb)
	    return (FALSE);
    } else {
	rcvd_hwidb = hwidb;
    }

    if (ops->hw_type != aarp_hwtype(rcvd_hwidb, idb->aarp_enctype)) {
	atalk_stat[ATALK_ARP_BOGON]++;
	return (FALSE);
    }

    /* Check Protocol type field */
    if (ops->protoType != TYPE_ETHERTALK) {
	atalk_stat[ATALK_ARP_BOGON]++;
	return (FALSE);
    }

    /* Check Hardware address length field */
    if (ops->hwAddrLen != aarp_hwlen(rcvd_hwidb, idb->aarp_enctype)) {
	atalk_stat[ATALK_ARP_BOGON]++;
	return (FALSE);
    }

    /* Check Protocol address length field */
    if (ops->ptAddrLen != sizeof(ataddrtype)) {
	atalk_stat[ATALK_ARP_BOGON]++;
	return (FALSE);
    }

    /* Check Source Hardware address field for strange addresses */
    switch (aarp_hwtype(rcvd_hwidb, idb->aarp_enctype)) {
    case AARP_HWETHER:
    case AARP_HWOLDFDDI:
    case AARP_HWTR:
	if (is_ieee_zero(ops->srcHwAddr)
	    || ieee_equal(ops->srcHwAddr, hwidb->hardware)) {
	    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		buginf("%s: AARP: Packet from invalid source address %e",
		       idb_name, ops->srcHwAddr);
	    atalk_stat[ATALK_ARP_BOGON]++;
	    return (FALSE);
	}
	break;
    default:
	/* Currently no other interfaces support AARP */
	break;
    }

    tpaddr = atalk_address(ops->dstPtAddrNet, ops->dstPtAddrNode);
    spaddr = atalk_address(ops->srcPtAddrNet, ops->srcPtAddrNode);
    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) {
	buginf("\nat_arp: valid'g src: %a, %e; target: %a, %e",
	       spaddr, ops->srcHwAddr, tpaddr, ops->dstHwAddr);
    }
    
    snet = ops->srcPtAddrNet;
    tnet = ops->dstPtAddrNet;
    
    /*
     * This next bit is so silly, it deserves an explanation.
     *
     * For reasons lost in the mists of time, we expect incoming
     * AARP packets on non-extended interfaces to have a network number
     * of 0 in both the destination and source protocol address
     * fields. If these fields are non-zero, we have complained in the
     * past.
     *
     * But our very own code has been sending AARP Reply packets with
     * a non-zero network field in the destination address for some
     * time now. The only reason why this error had not been seen is
     * that DDP address gleaning had been enabled by default on
     * non-extended interfaces since at least version 8.3.
     * Since we now want to run without gleaning for performance reasons,
     * the check is as follows: the AARP packet is bogus if the network
     * numbers are non-zero *and* the network number(s) in the packets
     * do not match the configured network number on the non-extended
     * interface.
     */
    bogon = FALSE;
    if (!atalkif_net_extended(idb)) {	/* non-extended interface */
	if (snet == 0) {
	  ops->srcPtAddrNet = idb->atalknet;
	  ops->srcPtAddr = atalk_address(ops->srcPtAddrNet, ops->srcPtAddrNode);
	  snet = idb->atalknet;
	}
	if (tnet == 0) {
	  ops->dstPtAddrNet = idb->atalknet;
	  ops->dstPtAddr = atalk_address(ops->dstPtAddrNet, ops->dstPtAddrNode);
	  tnet = idb->atalknet;
	}
	if (snet != idb->atalknet || tnet != idb->atalknet)
	    bogon = TRUE;
    } else {				/* extended interface */
	if (snet == 0 || tnet == 0)
	    bogon = TRUE;
    }

    if (bogon) {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\n%s: AARP: Packet %d from %e improperly formatted for "
		   "net", idb_name, ops->opcode, ops->srcHwAddr);
	atalk_stat[ATALK_ARP_BOGON]++;
	return (FALSE);
    }

    /*
     * Here's where we get into a bit of a bind:
     * The atalkif_ValidAddress routine is a bit of a misnomer.
     * The atalkif_ValidAddress routine really just checks for
     * valid destination addresses, not valid source addresses.
     */
    if ((!atalkif_ValidAddress(idb,spaddr)) ||
	(!atalkif_ValidAddress(idb,tpaddr))) {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) 
	    buginf("\n%s: AARP: Packet from %a(%e) for %a not valid for cable",
		   idb_name, ops->srcPtAddr, ops->srcHwAddr, ops->dstPtAddr);
	atalk_stat[ATALK_ARP_BOGON]++;
	return (FALSE);
    }
    
    /*
     * Check for packets which have been:
     * - looped back to us
     * - sent to us because someone else on the cable is using
     *   our protocol address. In this case, restart the port
     *   to gain a new address.
     */
    if (spaddr == atalkif_MyAddress(idb)) {
	idbtype *swidb;
	/*
	 * If the input interface is a BVI(Birdge-group Virtual 
	 * Interface), get the real received bridged interface.
	 * For IRB(Integrated Routing and Bridging) support
	 */
	if (is_bvi(hwidb)) {
	    swidb = rcvd_hwidb->firstsw;
	} else {
	    swidb = idb->phys_idb;
	}
	if (atalk_idb_hwaddr(swidb, idb_hw, &hw_len)) {
	    if (!bcmp(ops->srcHwAddr, idb_hw, hw_len))
		return (FALSE);		/* caught chatting to myself */
	} else {
	    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb)) 
		buginf("\nat_arp: couldn't get idb hw address");
	}
	    
	if ((idb->aarp_tentative == AARP_ADDR_SET) &&
	    (ops->opcode != OP_AARP_PROBE)) {
	    errmsg(AT_ERR_DUPADDR, idb_name, ops->srcHwAddr);
	    atalkif_RestartPort(idb);
	    return (FALSE);
	}
    }
    return (TRUE);
}

/*
 * aarp_canonicalize
 * canonicalize arp info in packet for preparation to merge into arp cache
 *
 * This code no longer depends upon arp globals.  Only the aarp_reply_flag
 * remains.  We build the prototype of the source, for the cache, as we
 * go along.
 *
 */
static boolean
aarp_canonicalize (aarpOps_t *ops)
{
    boolean addit;
    atalkidbtype *idb = atalk_getidb(ops->pak->if_input);
    hwidbtype *hwidb = hwidb_or_null(idb->phys_idb);
    char const *idb_name = idb_get_namestring(idb->phys_idb);
    
    aarp_reply_flag = FALSE;		/* Don't send a reply */
    addit = FALSE;			/* Don't add to cache */
    
    
    switch (ops->opcode) {
    case OP_AARP_REQUEST:
	atalk_stat[ATALK_ARP_REQUEST]++;
	if (idb->aarp_tentative != AARP_ADDR_SET) {
	    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		buginf("\n%s: AARP: Request from %a(%e) ignored, port down",
		       idb_name, ops->srcPtAddr, ops->srcHwAddr);
	    return (FALSE);
	}
	/*
	 * Pre-FDDITalk Mar 8 1991 FDDI compatibilty check.
	 * If one router on the ring is pre-FDDItalk, then
	 * we will display a warning message.
	 */
	if ( (hwidb->status & IDB_FDDI)
	    && (hwidb->atalk_compat_flags != AT_COMPAT_FDDI) ) {
	    if (ops->hw_type != AARP_HWFDDI) {
		errmsg(AT_ERR_FDDICOMPAT, ops->srcPtAddr);
		printf("\nAT: heard a non-FDDI hwtype (%d) in AARP request",
		       ops->hw_type);
	    }
	}
	/*
	 * Looking for meee?
	 */
	if (atalkif_net_extended(idb)) {
	    if (atalkif_IsMyAddress(idb,ops->dstPtAddr))
		aarp_reply_flag = TRUE;
	} else {
	    if (atalkif_IsMyAddress(idb, atalk_address(idb->atalknet,ops->dstPtAddr)))
		aarp_reply_flag = TRUE;
	}
	addit = TRUE;				/* Add src to cache */
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\n%s: AARP: Request from src %a(%e) for trgt %a",
		   idb_name, ops->srcPtAddr, ops->srcHwAddr, ops->dstPtAddr);
	break;
	
    case OP_AARP_PROBE:
	atalk_stat[ATALK_ARP_PROBE]++;
	/*
	 * Pre-FDDITalk Mar 8 1991 FDDI compatibilty check.
	 * If one router on the ring is pre-FDDItalk, then
	 * we will display a warning message.
	 */
	if ( (hwidb->status & IDB_FDDI)
	    && (hwidb->atalk_compat_flags != AT_COMPAT_FDDI) ) {
	    if (ops->hw_type != AARP_HWFDDI) {
		errmsg(AT_ERR_FDDICOMPAT, ops->srcPtAddr);
		printf("\nAT: heard a non-FDDI hwtype (%d) in AARP probe",
		       ops->hw_type);
	    }
	}
	/*
	 * if tentative protocol address matches ours, send a reply
	 */
	if (atalkif_MyAddress(idb) == ops->dstPtAddr) {
	    if (idb->aarp_tentative == AARP_ADDR_SET) {
		aarp_reply_flag = TRUE;
		if (idb_is_ieeelike(idb->phys_idb))
		    ieee_copy(hwidb->hardware,ops->dstHwAddr);
		else {
		    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb)) 
			buginf("\nat_arp: Hey! You need to reply to a probe!");
		}
	    } else
		idb->aarp_tentative = AARP_ADDR_FAILED;
	} else {
	    /*
	     * If we're hearing a probe for an address,
	     * then we should delete any entry we currently
	     * have for that address since address assignments might
	     * change very soon.
	     */
	    aarp_remove(ops->dstPtAddr, idb);
	    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		buginf("\n%s: AARP: Probe for %a from hwaddr %e",
		       idb_name, ops->dstPtAddr, ops->srcHwAddr);
	}
	break;
	
    case OP_AARP_REPLY:
	atalk_stat[ATALK_ARP_REPLY]++;
	if (atalkif_MyAddress(idb) != ops->dstPtAddr) {
	    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		buginf("\n%s: AARP reply from %a(%e) for %a(%e) "
		       "ignored, not for me",
		       idb_name, ops->srcPtAddr, ops->srcHwAddr,
		       ops->dstPtAddr, ops->dstHwAddr);
	    atalk_stat[ATALK_ARP_BOGON]++;
	    return (FALSE);
	}		
	if (idb->aarp_tentative != AARP_ADDR_SET) {
	    idb->aarp_tentative = AARP_ADDR_FAILED;
	    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		buginf("\n%s: AARP probe failed - Address %a in use by %e",
		       idb_name, ops->dstPtAddr, ops->srcHwAddr);
	    return (FALSE);
	}
	addit = TRUE;
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\n%s: AARP: Reply from %a(%e) for %a(%e)",
		   idb_name, ops->srcPtAddr, ops->srcHwAddr, ops->dstPtAddr,
		   ops->dstHwAddr);
	break;
	
    default:
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\nat_arp: shouldn't be here");
	return (FALSE);
    }
    return (addit);
}

/*
 * send_aarp_reply
 * send reply to an AppleTalk ARP probe or request packet
 *
 * It is assumed that only the target hardware address has been changed
 * from the original AARP packet and that the new hardware address
 * is in the 'ops' structure.
 */
static void
send_aarp_reply (paktype *pak, aarpOps_t *ops)
{
    atalkidbtype *idb = atalk_getidb(pak->if_input);
    hwidbtype	*hwidb;
    aarp_ptrs_t	ptrs;
    aarp_hdr    *aarp;
    ulong	bytes;			/* size (in bytes) of AARP datagram */
    uchar	hwaddr[STATIONLEN_MAX];	/* MAC-level hw address */
    ushort	hwaddr_len;		/* number of valid bytes in hwaddr */
    uchar	hw_len;			/* size of h/w addr, defined by AARP */
    ushort	src_pt_net;
    char const *idb_name;
    
    aarp = aarpheadstart(pak);

    aarp_set_ptrs(pak, &ptrs);
    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == NULL) {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\nAARP: Response to %a(%e) failed, hwidb is NULL",
		   GETLONG(ptrs.dst_pt_addr), ptrs.dst_hw_addr);
	datagram_done((paktype *)aarp);
	atalk_stat[ATALK_ARP_FAILED]++;
	return;
    }

    idb_name = idb_get_namestring(idb->phys_idb);
    
    hw_len = ops->hwAddrLen;
    aarp->opcode = OP_AARP_REPLY;	/* opcode set to reply */
    /*
     * Copy the source hw address to the destination hw address.
     * (That's the whole point of a reply). Then clear out the
     * source hw address field.
     */
    bcopy(ops->srcHwAddr, ptrs.dst_hw_addr, hw_len);
    memset(ptrs.src_hw_addr, 0, hw_len);
    src_pt_net = GETSHORT(ptrs.src_pt_net);
    if (atalkif_net_extended(idb)) {
	PUTSHORT(ptrs.dst_pt_net, src_pt_net);
	PUTSHORT(ptrs.src_pt_net, idb->atalknet);
    } else {
	PUTSHORT(ptrs.dst_pt_net, idb->atalknet);
    }

    *ptrs.dst_pt_node = *ptrs.src_pt_node; /* swap dest & source */
    *ptrs.src_pt_node = idb->atalknode; /* set the node number */
    atalk_idb_hwaddr(idb->phys_idb, hwaddr, &hwaddr_len);
    if (hwaddr_len != hw_len) {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\naatp_send_reply: packet hw len (%d) != idb hw len (%d)",
		   hw_len, hwaddr_len);
    }
    bcopy(hwaddr, ptrs.src_hw_addr, hw_len);
    
    pak->if_output = idb->phys_idb; /* force reply to input interface */
    pak->linktype = LINK_AARP;		/* probably set but... */
    pak->enctype = idb->aarp_enctype;	/* if this isn't set, we should lose */
    
    /*
     * Change this to use the vencaps vectors? There really isn't
     * any point, since we are sourcing these packets, and don't need
     * to worry about shuffling pak->datagramstart around under existing
     * data.
     */
    bytes = AARP_HEADER_SIZE + 2*hw_len;
    if (pak_encap(hwidb, pak, bytes, (long)ptrs.dst_hw_addr)) {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) {
	    buginf("\n%s: AARP: Sent response to %a(%e)",
		   idb_name, GETLONG(ptrs.dst_pt_addr), ptrs.dst_hw_addr);
	    buginf("\nAARP: Sending AARP reply");
	    if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		at_pak_printOhdr(pak, idb, TRUE);
	}
	datagram_out(pak);
	atalk_stat[ATALK_ARP_SENT]++;
    } else {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\n%s: AARP: Response to %a(%e) failed",
		   idb_name, GETLONG(ptrs.dst_pt_addr), ptrs.dst_hw_addr);
	datagram_done(pak);
	atalk_stat[ATALK_ARP_FAILED]++;
    }
}

/*
 * aarp_resolve
 * Send an RFC826 style address resolution request.
 * Returns TRUE if request sent, FALSE if request cannot be honored.
 *
 *  address	-- the AppleTalk address for which a MAC address is wanted
 *      idb	-- the interface on which the lookup is to be sent
 *   method	-- The aarp_enctype method, which is used in determining the
 *		   MAC address length
 *  probing	-- T if we are sending a probe, F if we are sending a lookup
 */
boolean
aarp_resolve (ataddrtype address, atalkidbtype* idb,int method,boolean probing)
{
    aarp_hdr	*aarp;
    paktype     *pak;
    int		bytes;
    char	*op;
    uchar	hwaddr[STATIONLEN_MAX];
    ushort	hwaddr_len;
    aarp_ptrs_t	ptrs;
    hwidbtype	*hwidb;
    char const *idb_name;

    idb_name = idb_get_namestring(idb->phys_idb);
    
    op = (probing) ? "probe" : "resolve";
    if (!atalkif_ValidAddress(idb,address)) {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\n%s: AARP: Attempt to %s a reserved address - %a",
		   idb_name, op, address);
	atalk_stat[ATALK_ARP_UNKNOWN]++;
	return (FALSE);
    }
    
    if (!interface_up_simple(idb->phys_idb)) {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\n%s: AARP: Unable to send %s for %a, line proto is %s",
		   idb_name, op, address, print_idbstate(idb->phys_idb));
	return (FALSE);
    }

    /*
     * The size of an AARP packet is the fixed header, two longwords,
     * plus two hardware address fields. Ask for a buffer of said
     * size...
     * ??? Must account for the creation of a packet here.
     *     This should not be accounted for with DDP datagrams, but
     *     should be tallied seperately.
     */
    bytes = AARP_HEADER_SIZE + 2 * aarp_hwlen(hwidb_or_null(idb->phys_idb),
					      idb->aarp_enctype);
    pak = getbuffer(bytes);
    if (!pak) {
	atalk_stat[ATALK_NO_BUFFERS]++;
	return (FALSE);
    }
    pak->datagramstart = pak->network_start;

    aarp = aarpheadstart(pak);
    aarp->arhrd = aarp_hwtype(hwidb_or_null(idb->phys_idb), method);
    aarp->arhln = aarp_hwlen(hwidb_or_null(idb->phys_idb), method);
    aarp->arpln = PL_AARP;
    aarp->opcode = (probing) ? OP_AARP_PROBE : OP_AARP_REQUEST;
    pak->if_output = idb->phys_idb;
    pak->if_input  = idb->phys_idb;		/* ??? is this legit */
    pak->linktype = LINK_AARP;
    pak->enctype = idb->aarp_enctype;
    pak->acct_proto = ACCT_PROTO_ARP;

    /*
     * Don't ask me why Apple decided to use the $809B proto type
     * here... but they do. See Inside AppleTalk, p.2-12.
     */
    aarp->arpro = TYPE_ETHERTALK;

    /*
     * Note: aarp_set_ptrs must be called _after_ the aarp->arhln & arpln
     * fields are set above.
     */
    aarp_set_ptrs(pak, &ptrs);
    atalk_idb_hwaddr(idb->phys_idb, hwaddr, &hwaddr_len);
    bcopy(hwaddr, ptrs.src_hw_addr, aarp->arhln);
    memset(ptrs.src_pt_addr, 0, aarp->arpln);
    PUTSHORT(ptrs.src_pt_net, atalkif_net_extended(idb) ? idb->atalknet : 0);
    *ptrs.src_pt_node = idb->atalknode;
    memset(ptrs.dst_hw_addr, 0, aarp->arhln);
    memset(ptrs.dst_pt_addr, 0, aarp->arpln);
    
    PUTSHORT(ptrs.dst_pt_net, atalkif_net_extended(idb) ?
	     atalk_netonly(address) : 0);
    *ptrs.dst_pt_node = atalk_nodeonly(address);
    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == NULL) {
	datagram_done((paktype *)aarp);
	return (FALSE);
    }
    if (!pak_encap(hwidb, pak, bytes,(long)atalk_broadcast(idb))) {
	datagram_done(pak);
	atalk_stat[ATALK_ARP_FAILED]++;
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    buginf("\n%s: AARP: Unable to send %s for %a, failed encap",
		   idb_name, op, address);
	return (FALSE);
    }
    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) {
	buginf("\nAARP: Sending %s, dgs=%d",op, pak->datagramsize);
	if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    at_pak_printOhdr(pak, idb, TRUE);
    }
    datagram_out(pak);
    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) 
	buginf("\n%s: AARP: Sent %s for %a",
	       idb_name, op, address);
    atalk_stat[ATALK_ARP_SENT]++;
    return (TRUE);
}

/*
 * aarp_lookup
 * Do AppleTalk address resolution for Ethernets and IEEE 802.3 networks.
 * Called directly from driver encapsulation routines through 
 * idb->atalk_lookup vector.
 * 
 * If this function is called with a packet (from the encapsulation
 * function in a driver) and we did not previously have an AARP entry
 * in the table for the destination address, then we create a "pending"
 * AARP entry, which has a h/w address of 0000.0000.0000 and a flag
 * to mark it as pending.
 *
 * Pending AARP entries are never returned as a result of this routine
 * however packets are modified such that they will not be released after
 * we return FALSE.
 */
aarpentry_t*
aarp_lookup (ataddrtype address, atalkidbtype* idb, paktype* pak)
{
    aarpentry_t	*e;
    hwidbtype	*hwidb;
    char const  *idb_name;
    tinybool     do_debug;
    
    idb_name = idb_get_namestring(idb->phys_idb);
    do_debug = at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb) &&
	at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb);
    
    if (!atalkif_ValidAddress(idb,address))
	return (NULL);
    if (atalk_netonly(address) == 0)	/* Partially qualified address? */
	address = atalk_address(atalkif_MyNet(idb), atalk_nodeonly(address));
    if (do_debug)
	buginf("\n%s: AARP: aarp_lookup, looking for %a", idb_name, address);
    e = aarp_FindEntry(address, idb);
    if (e != NULL) {
	if (e->type == AARP_WAITING) {
	    if (do_debug)
		buginf("\n%s: AARP: aarp_lookup, entry for %a is waiting",
		       idb_name, address);
	    if ((pak != NULL) && (pak->enc_flags == 0)) {
		aarp_FreeResources(e);
		pak->enc_flags = ENC_NOARPENTRY;
		e->unarped_pak = pak;
		e->idb = idb;
		e->arp_retries = 0;
		GET_TIMESTAMP(e->time);
		aarp_has_waiting_pak++;
		if (!mgd_timer_running_and_sleeping(&aarp_next_scan))
		    mgd_timer_start(&aarp_next_scan, aarp_request_interval);
	    }
	    return (NULL);
	}
	if (do_debug)
	    buginf("\n%s: AARP: aarp_lookup, returning entry for %a (%e)",
		   idb_name, address,e->hw_addr);

	return (e);			/* found non-waiting entry */
    }

    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) {
	buginf("\n%s: AARP: aarp_lookup, no entry for %a",
	       idb_name, address);
    }
    if ((pak != NULL) && (pak->enc_flags & ENC_FAILED_ONCE))
	return (NULL);

    /*
     * We didn't have an AARP entry for the destination address -- so
     * send a AARP resolve, create a pending entry and defer the packet.
     */
    if (!aarp_resolve(address, idb, idb->aarp_enctype, FALSE))
	return (NULL);

    if (pak != NULL) {
	e = aarp_FindEntry(address, idb);
	hwidb = hwidb_or_null(idb->phys_idb);
	if (e == NULL && hwidb != NULL)
	    e = aarp_insert(idb, address, zero_hw_addr,
			    aarp_hwlen(hwidb, idb->aarp_enctype),
			    idb->aarp_enctype, AARP_WAITING);
	if (e != NULL) {
	    if (e->type != AARP_WAITING)
		return (e);
	    aarp_FreeResources(e);
	    pak->enc_flags = ENC_NOARPENTRY;
	    e->idb = idb;
	    e->arp_retries = 0;
	    e->unarped_pak = pak;
	    aarp_has_waiting_pak++;
	    if (!mgd_timer_running_and_sleeping(&aarp_next_scan))
		mgd_timer_start(&aarp_next_scan, aarp_request_interval);
	}
    }
    return (NULL);
}

/*
 * aarp_probe
 * Do AppleTalk address resolution for Ethernets and IEEE 802.3 networks.
 * called by aarp_pick_addr()
 * returns TRUE if address is not claimed by anyone
 */
boolean
aarp_probe (ataddrtype address, atalkidbtype* idb)
{
    int retries;

    /*
     * Tentatively set the idb's appletalk address to the address
     * we are trying. If the idb has static addressing (ie, the
     * idb doesn't support AARP or it is a half-routing gateway)
     * then if the IDB is up, we're done.
     */
    idb->atalknet = atalk_netonly(address);
    idb->atalknode = atalk_nodeonly(address);
    if (atalkif_static_addresses(idb)) {
	if (!interface_up_simple(idb->phys_idb)) {
	    idb->aarp_tentative = AARP_ADDR_INVALID;
	    return (FALSE);
	}
	idb->aarp_tentative = AARP_ADDR_SET;
	return (TRUE);
    }
    idb->aarp_tentative = AARP_ADDR_TENTATIVE;
    for (retries=aarp_probe_count; retries > 0; retries--) {
	if (idb->aarp_tentative != AARP_ADDR_TENTATIVE)
	    return (FALSE);
	if (!interface_up_simple(idb->phys_idb)) {
	    idb->aarp_tentative = AARP_ADDR_INVALID;
	    return (FALSE);
	}
	if (!aarp_resolve(address, idb, idb->aarp_enctype, TRUE)) {
	    idb->aarp_tentative = AARP_ADDR_INVALID;
	    return (FALSE);
	}
	process_sleep_for(aarp_probe_interval);
    }
    if (idb->aarp_tentative == AARP_ADDR_TENTATIVE) {
	idb->aarp_tentative = AARP_ADDR_SET;
	return (TRUE);
    }
    return (FALSE);
}

/*
 * at_initial_addr
 * pick an address which is valid for this interface and initialize the
 * idb fields.
 */
static ataddrtype
at_initial_addr (atalkidbtype* idb)
{
    ataddrtype myaddr = atalkif_MyAddress(idb);
    
    if (myaddr != 0)
	if (!atalkif_ValidAddress(idb, myaddr)) {
	    errmsg(AT_ERR_ADDRINVALID, idb_get_namestring(idb->phys_idb),
		   myaddr);
	    idb->atalknet = ATALK_NET_UNKNOWN;
	    idb->atalknode = 0;
	}
    if ((idb->atalk_cableend == ATALK_RANGE_UNKNOWN) &&
	(idb->atalknet == ATALK_NET_UNKNOWN)) {
	idb->atalknet = ATALK_STARTUP_START + 
	    atalk_random((ATALK_STARTUP_END - ATALK_STARTUP_START) + 1);
    } else {
	if (idb->atalknet == ATALK_NET_UNKNOWN)
	    idb->atalknet = idb->atalk_cablestart;
    }
    if (idb->atalknode == 0)
	idb->atalknode = atalk_random(ATALK_ETHER_BADADDR-1)+1;
    return atalkif_MyAddress(idb);
}

/*
 * at_next_addr
 * pick the next address which is valid for this interface
 */
static ataddrtype
at_next_addr (atalkidbtype* idb,ataddrtype old)
{
    uchar node = atalk_nodeonly(old);
    ushort net = atalk_netonly(old);
    
    if (!atalkif_ValidAddress(idb,atalk_address(net,node+1))) {
	node = 1;		/* wrapped, so restart at first valid node */
	if (!atalkif_net_extended(idb))
	    return (atalk_address(net,node));
	if (net == idb->atalk_cableend)
	    net = idb->atalk_cablestart;
	else if (net == 0)		/* if no range yet, skip to startup */
	    net = ATALK_STARTUP_START;
	else if (net < idb->atalk_cableend)
	    net++;
	return (atalk_address(net,node));
    }
    return (atalk_address(net,node+1));
}

/*
 * aarp_pick_addr
 * choose an address, starting with the current address, if any
 */
boolean
aarp_pick_addr (atalkidbtype* idb)
{
    ataddrtype tent_addr,first_addr = atalkif_MyAddress(idb);
    hwidbtype	*hwidb;


    /* Clean up our leftovers */
    if (idb->aarp_tentative == AARP_ADDR_SET) {
	if (idb->aarp_enctype != ET_NULL)
	    aarp_remove(first_addr,idb);
	idb->aarp_tentative = AARP_ADDR_INVALID;
	if (atalkif_dstnettype(idb,atalk_netonly(first_addr))
	    == netStartupRange) {
	    idb->atalknet = 0;
	    idb->atalknode = 0;
	}
    }
    
    /*
     * Proceed only if idb is ready to send packets
     */
    if ((!atalk_running) || !interface_up(idb->phys_idb) ||
	!idb->atalk_enabled) {
	idb->aarp_tentative = AARP_ADDR_INVALID;
	return (FALSE);
    }
    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == FALSE)
	return (FALSE);

    first_addr = at_initial_addr(idb);
    idb->aarp_tentative = AARP_ADDR_TENTATIVE;
    if ((*idb->atalk_probe)(first_addr, idb)) {
	aarp_insert(idb, first_addr, hwidb->hardware,
		    aarp_hwlen(hwidb, idb->aarp_enctype),
		    idb->aarp_enctype, AARP_INTERFACE);
	return (TRUE);
    }
    
    for (tent_addr=at_next_addr(idb,first_addr);
	 first_addr != tent_addr;
	 tent_addr=at_next_addr(idb,tent_addr)) {
	
	if (idb->aarp_tentative == AARP_ADDR_INVALID) /* Give up on errors */
	    return (FALSE);
	
	if (aarp_FindEntry(tent_addr, idb) != NULL)
	    continue;
	if ((*idb->atalk_probe)(tent_addr, idb)) {
	    aarp_insert(idb, tent_addr,
			hwidb->hardware,
			aarp_hwlen(hwidb, idb->aarp_enctype),
			idb->aarp_enctype, AARP_INTERFACE);
	    return (TRUE);
	}
	if (!interface_up(idb->phys_idb))
	    return (FALSE);
    }
    return (FALSE);
}

/*
 * aarp_insert
 * Add an AARP entry for AppleTalk but checking all the strange translation
 * traps.
 *
 */
aarpentry_t*
aarp_insert (atalkidbtype* idb, ataddrtype address, uchar const * hardware,
	     ushort hardware_len, encapstype enctype, aarp_entry_t entry_type)
{
    aarpentry_t	*e;
    aarpentry_t	*next;
    boolean	delete_it;
    hwidbtype	*hwidb;
    char const  *idb_name;
    tinybool     do_debug;
    
    if (aarp_tree == NULL || idb == NULL) /* no AARP database? Too bad... */
	return (NULL);
    hwidb = hwidb_or_null(idb->phys_idb);
    if (hwidb == NULL)
	return (NULL);
    if (atalkif_static_addresses(idb))	/* Static IDB's cannot be set */
	return (NULL);
    idb_name = idb_get_namestring(idb->phys_idb);
    do_debug = at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb);
    
    unset_temp_aarp(hardware, address, idb);

    if (idb->aarp_tentative != AARP_ADDR_SET) {
	if (do_debug)
	    buginf("\n%s: AARP: aarp_insert; Node %a not added, port down",
		   idb_name, address);
	return (NULL);
    }

    if (!atalkif_ValidAddress(idb, address)) {
	if (do_debug)
	    buginf("\n%s: AARP: aarp_insert; Node %a is not a valid address",
		   idb_name, address);
	return (NULL);
    }

    if (atalk_netonly(address) == 0)	/* Partially qualified address? */
	address = atalk_address(atalkif_MyNet(idb), atalk_nodeonly(address));

    if (aarp_hwlen(hwidb, idb->aarp_enctype) != hardware_len) {
	if (do_debug)
	    buginf("\n%s: AARP\aarp_insert, idb hwlen != passed len(%d)",
		   hardware_len);
	return (NULL);
    }

    /* First, see if database already contains an entry.  Search by key.  */
    if ((e = aarp_FindEntry(address, idb)) != NULL) {
	aarp_ProtectEntry(e, TRUE);
	if (bcmp(e->hw_addr, hardware, hardware_len)) {
	    if (e->type != AARP_WAITING) {
		atalk_InvalidateCache("aarp_insert");
		if (do_debug) {
		    buginf("\n%s: AARP: Node %a moved from %e to %e",
			   idb_name, address, e->hw_addr, hardware);
		}
	    }
	}
	bcopy(hardware,e->hw_addr,hardware_len); /* update hdw address */
	e->idb = idb;
	GET_TIMESTAMP(e->time);	/* update last reference */
	if (e->type == AARP_WAITING) {
	    if (do_debug) {
		buginf("\n%s: AARP: Resolved waiting request for %a(%e)",
		       idb_name, e->proto_addr, e->hw_addr);
	    }
	    e->type = AARP_DYNAMIC;	/* we've now learned this */
	}
	aarp_UnprotectEntry(e);
	return (e);
    }

    /*
     * Search entire AARP database looking for entries which have
     * our same protocol address.
     *
     * We must search the database by loop rather than key.
     * If we find a node with our protocol address on a different
     * interface, then delete it and continue.  We assume the information
     * we're about to insert is correct.
     */
    e = aarp_GetFirstEntry();
    if (e) {
	if (do_debug)
	    buginf("\n%s: AARP aarp_insert, starting search with %a"
		   " if_number=%lu", idb_name, e->proto_addr, 
		   (ulong)(e->link.key.ll >> 24));
    } else {
	if (do_debug) 
	    buginf("\n%s: AARP aarp_insert, initial NULL entry",
		   idb_name);
    }

    while (e != NULL) {
	aarp_ProtectEntry(e, TRUE);
	delete_it = FALSE;
	
	if (e->proto_addr == address) {	/* Protocol addresses match */
	    if (e->idb != idb) {
		/* Somebody's using our protocol address on another
		 * interface.  Blow him away and continue the search.
		 */
		if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb)) 
		    buginf("\n%s: AARP aarp_insert, found address %a on %s, "
			   "deleting",
			   idb_name, address,
			   idb_get_namestring(e->idb->phys_idb));
		delete_it = TRUE;
		atalk_InvalidateCache("aarp_insert");
	    } else {
		/* Hey!  We should have found this!  Squawk!  Delete it
		 * for safety.
		 */
		if (do_debug)
		    buginf("\n%s: AARP aarp_insert, missed node %a", address);
		delete_it = TRUE;
		atalk_InvalidateCache("aarp_insert");
	    }
	} else {
	    /* Continue search.  If idbs and hwaddrs match, we could
	     * optionally delete it, since this indicates that a device
	     * changed protocol addresses.  We may not want to, however,
	     * because a) we may want to do transition routing on the
	     * same interface some day, and b) the aarp ager will delete
	     * the old entry eventually.
	     */
	}

	next = aarp_GetNextEntry(e);
	aarp_UnprotectEntry(e);
	if (delete_it) {
	    aarp_FreeResources(e);
	    aarp_DeleteEntry(e);
	}
	e = next;
	if (e && do_debug)
	    buginf("\n%s: AARP aarp_insert, continuing search with %a"
		   " if_number=%lu", idb_name, e->proto_addr, 
		   (ulong)(e->link.key.ll));
    }

    /*
     * Found no entry with a matching protocol address.  Create a new entry.
     */
    e = aarp_CreateEntry(address, idb);
    if (!e) {
	if (do_debug)
	    buginf("\nAARP: aarp_insert; NULL returned from aarp_CreateEntry");
	return (NULL);			/* malloc failure */
    }
    bcopy(hardware, e->hw_addr, hardware_len); /* set hardware address */
    e->type = entry_type;		/* set entry type */
    e->idb = idb;			/* remembers idb we heard it on */
    e->enctype = enctype;		/* set encapsulation type */
    e->proto_addr = address;
    GET_TIMESTAMP(e->time);	/* set reference time */
    e->arp_retries = 0;
    e->unarped_pak = NULL;
    if (!aarp_InsertEntry(e)) {		/* put the entry into the tree */
	if (do_debug) {
	    buginf("\n%sAARP: aarp_insert, Failed to insert entry %08x"
		   " into aarp_tree", idb_name, e);
	}
	free(e);			/* no aarp_DeleteEntry needed */
	e = NULL;
    }
    if (do_debug) {
	buginf("\nAARP: aarp_insert, entry %08x for node %a(%e) added on %s",
	       e, address, hardware, idb_name);
    }
    return (e);
}					/* end aarp_insert */

/*
 * flush_aarp
 * Flush AARP cache entries for an interface.
 *
 * Note: idb may be passed in as NULL. This is why the entry loop starts
 * with the first possible entry in the tree.
 */
void 
flush_aarp (atalkidbtype* idb)
{
    aarpentry_t	*e = NULL;
    aarpentry_t	*next = NULL;
    boolean	delete_it;

    if (aarp_tree == NULL) 
	return;
    /*
     * Again, we have to loop over the AARP database.
     */
    if ((e = aarp_GetFirstEntry()) == NULL) {
	return;
    }
    
    while (e) {
	aarp_ProtectEntry(e, TRUE);
	delete_it = FALSE;
	if (!(((idb == NULL) && ValidAARP(e)) || (e->idb != idb))) {
	    if (e->type == AARP_DYNAMIC)
		atalk_InvalidateCache("flush_aarp");
	    delete_it = TRUE;
	}
	next = aarp_GetNextEntry(e);
	aarp_UnprotectEntry(e);
	if (delete_it) {
	    aarp_FreeResources(e);
	    aarp_DeleteEntry(e);
	}
	e = next;
    }
}

/*
 * flush_all_aarp
 * Flush all AARP cache entries for an interface.
 * Since the the incorporation of the tree data structure for the AARP entries,
 * we now must search across the entire tree to find all matching entries.
 */
void 
flush_all_aarp (ushort net)
{
    aarpentry_t	*e;
    aarpentry_t	*next;
    boolean	delete_it;

    if (aarp_tree == NULL) 
	return;
    
    if ((e = aarp_GetFirstEntry()) == NULL) {
	return;
    }

    while (e != NULL) {
	delete_it = FALSE;
	aarp_ProtectEntry(e, TRUE);
	if ((net == 0 || (net != 0 && net == atalk_netonly(e->proto_addr)))) {
	    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, e->idb->phys_idb)) {
		buginf("\n%s: AARP: Removing entry for %a(%e)",
		       idb_get_namestring(e->idb->phys_idb),
		       e->proto_addr, e->hw_addr);
	    }
	    if (e->type == AARP_DYNAMIC)
		atalk_InvalidateCache("flush_all_aarp");
	    delete_it = TRUE;
	}
	next = aarp_GetNextEntry(e);
	aarp_UnprotectEntry(e);
	if (delete_it) {
	    aarp_FreeResources(e);
	    aarp_DeleteEntry(e);
	}
	e = next;
    }
    aarp_fix();
}

/*
 * aarp_remove
 * Remove a particular AARP entry.
 *
 *
 * If the ARP entry in question has an unarp'ed packet waiting,
 * we remove and deallocate same before unlinking and deallocating
 * the ARP entry.
 * NOTE: The entry won't be deleted if the busy count > 0.
 */
void 
aarp_remove (ataddrtype address, atalkidbtype* idb)
{
    aarpentry_t *e, *next;
    boolean delete_it;
    char const *idb_name;

    if (aarp_tree == NULL)
	return;

    /* 
     * if idb is unknown, we must search the entire tree
     */
    if (idb == NULL) {
        if ((e = aarp_GetFirstEntry()) == NULL)
	    return;

	while (e != NULL) {
	    delete_it = FALSE;
	    aarp_ProtectEntry(e, TRUE);
	    if (e->proto_addr == address && e->type == AARP_DYNAMIC) {
	        if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, e->idb)) {
		    idb_name = idb_get_namestring(e->idb->phys_idb);
		    buginf("\n%s: AARP: Removing entry for %a(%e)",
			   idb_name, e->proto_addr, e->hw_addr);
		}
		atalk_InvalidateCache("aarp_remove");
		delete_it = TRUE;
	    }
	    next = aarp_GetNextEntry(e);
	    aarp_UnprotectEntry(e);
	    if (delete_it) {
	        aarp_FreeResources(e);
		aarp_DeleteEntry(e);
	    }
	    e = next;
	}
	return;
    }

    if ((e = aarp_FindEntry(address, idb)) == NULL)
	return;

    if (e->proto_addr == address && (e->idb == NULL || e->idb == idb)) {
	if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, idb->phys_idb)) {
	    buginf("\n%s: AARP: Removing entry for %a(%e)",
		   idb_get_namestring(e->idb->phys_idb),
		   e->proto_addr, e->hw_addr);
	}
	aarp_FreeResources(e);
	if (e->type == AARP_DYNAMIC)
	    atalk_InvalidateCache("aarp_remove");
	aarp_DeleteEntry(e);
    }
}

/*
 * remove a temporary AARP entry from the tree.
 */
static void 
unset_temp_aarp (uchar const * hw, ataddrtype addr, atalkidbtype* idb)
{
    aarpentry_t *e;
    uchar	idb_hw_addr[STATIONLEN_MAX];
    ushort	len;
    
    /*
     * See if address is in the table
     */
    if (aarp_tree == NULL)
	return;
    
    if (!atalk_idb_hwaddr(idb->phys_idb, idb_hw_addr, &len))
	return;
    
    if ((e = aarp_FindEntry(addr, idb)) == NULL)
	return;
    
    if ((bcmp(idb_hw_addr, hw, len) == 0) &&
	(e->proto_addr == addr && e->idb == idb)) {
	aarp_DeleteEntry(e);
	return;
    }
}

/*
 * aarp_ReinitEntry
 * Release any resources in use for this entry.
 */
static void
aarp_ReinitEntry (aarpentry_t* e)
{
    if (e)
	aarp_FreeResources(e);
}


/*
 * aarp_CreateEntry
 * initialize a new AARP entry
 */
static aarpentry_t*
aarp_CreateEntry (ataddrtype address, atalkidbtype* idb)
{
    aarpentry_t* e;
    
    if (aarp_tree == NULL)
	return (NULL);
    
    e = (aarpentry_t *) RBTreeGetFreeNode(aarp_tree);
    if (e != NULL) {
	aarp_ReinitEntry(e);
	memset(e, 0, sizeof(aarpentry_t));
    } else
	e = malloc(sizeof(aarpentry_t));
    if (e == NULL)
	return (NULL);
    return (e);
}

aarpentry_t*
aarp_FindEntry (ataddrtype address, atalkidbtype* idb)
{
    treeKey key = aarp_EntryKey(idb,address);
    aarpentry_t* e;

    if (aarp_tree == NULL)
	return (NULL);
    e = aarpCast(RBTreeSearch(aarp_tree,key));
    return (e);
}

static void
aarp_DeleteEntry (aarpentry_t *e)
{
    if ((aarp_tree != NULL) && (e != NULL)) {
	if (RBTreeNodeProtected((treeLink *)e)) {
	    if (at_debug(aarp_debug, ATALK_INTERFACE_MASK, e->idb->phys_idb)) {
		buginf("\nat_arp.c: attempt to delete a busy aarp entry");
	    }
	    return;
	}
	RBTreeDelete(aarp_tree, &e->link);
    }
}

/*
 * aarp_flush_cache
 * Flush the entire AARP cache
 * Called by SERVICE_CLEAR_AR_CACHE
 */

void aarp_flush_cache (idbtype *idb)
{
    flush_all_aarp(0);
}

/*
 * aarp_show_table
 *
 * Dumps the AARP table or portions of it according to the following:
 * - if the passed address (net & node) == 0, then dump whole table
 * - if address passed was net.0, then dump all entries for that net
 * - if address passed was net.node, then dump only that node's entry.
 *
 * ??? to do:
 * 1. Printing support for longer than IEEEBYTES addresses
 * 2. Support for MAP entries from WAN proto's
 * 3. support for selecting entries according to IDB name/unit
 */
void
aarp_show_table (ushort net, uchar node)
{
    aarpentry_t	*e;
    aarpentry_t	*next;
    int 	len;
    ulong 	time;
    char 	*str;
    ushort	e_net;
    uchar	e_node;
    ataddrtype	addr;
    static	char *const entry_type[AARP_ENTRY_TYPE_MAX] =
	{ "Invalid ", "Dynamic ", "Static  ",
	  "Hardware", "Alias   ", "Pending ",
	  "Testing "                          };

    if (aarp_tree == NULL)
	return;				/* no tree allocated */
    if ((e = aarp_GetNextEntry(NULL)) == NULL) {
	printf("\n%%AARP Table is empty");
	return;				/* nothing in the tree */
    }

    while (e) {
	aarp_ProtectEntry(e, TRUE);
	e_net  = atalk_netonly(e->proto_addr);
	e_node = atalk_nodeonly(e->proto_addr);
	addr = atalk_address(net, node);
	if (   (addr == 0)
	    || (addr && (node == 0) && (net == e_net))
	    || (addr && (node == e_node) && (net == e_net))) {
	    len = printf("\n%a",e->proto_addr);
	    len = 13 - len;
	    printf("%-*c", len, ' ');
	    time = ELAPSED_TIME(e->time)/ONEMIN;
	    switch (e->enctype) {
	      case ET_ARPA:       str = "ARPA"; break;
	      case ET_SNAP:       str = "SNAP"; break;
	      case ET_HDLC:       str = "ARPA"; break;
	      case ET_ETHERTALK:  str = "ETLK"; break;
	      case ET_SMDS:       str = "SMDS"; break;
	      case ET_X25:        str = "X.25"; break;
	      default:            str = "????"; break;
	    }
	    
	    /*
	     * There are entries which we learn from the wire and
	     * entries which come from configured interfaces.
	     * The latter are marked with the "-" symbol.
	     */
	    if (e->type == AARP_DYNAMIC) {
		printf("%-10ld  ", time);
	    } else {
		printf("         -  ");
	    }
	    printf("%s  ", entry_type[e->type]);
	    /*
	     * If the entry is waiting on a response from the net
	     * with the h/w address, mark it so. Otherwise, print
	     * out the hardware address we have along with the
	     * wire encapsulation we are using.
	     */
	    if (e->type == AARP_WAITING) {
		printf("        -------       %6s", str);
	    } else {
		char addr[20];
		int cnt = 0;

		cnt = sprintf(addr, "%E", e->hw_addr);
		if (cnt <= 14) {
		/*
		 * Do a normal 48-bit MAC display
		 */
		    printf("%e       %6s", e->hw_addr, str);
		} else {
		/*
		 * Ok.. we're doing a full 64-bit E.164 SMDS display
		 */
		    printf("%E  %6s", e->hw_addr, str);
		}
	    }
	    if (e->idb)
		printf("    %s", idb_get_namestring(e->idb->phys_idb));
	}
	next = aarp_GetNextEntry(e);
	aarp_UnprotectEntry(e);
	e = next;
    }
}

/*
 * aarp_treeprint
 * Print the routing table RB tree.
 *
 */
void
atalk_show_pvt_aarp (ataddrtype addr)
{
    printf("\nAARP tree internals:");
    RBTreePrint(aarp_tree->root,0,aarp_tree);
}

/*
 * nv_atalk_arp
 * nvram support for appletalk arp
 */
void
nv_atalk_arp (void)
{
    if (!atalk_running)
	return;
    nv_write((aarp_request_interval != AARP_REQUEST_INTERVAL) && 
	     (aarp_request_interval != 0),
	     "appletalk arp request interval %d", aarp_request_interval);
    nv_write((aarp_request_count != AARP_REQUEST_RETRANSMIT) &&
	     (aarp_request_count != 0),
	     "appletalk arp request retransmit-count %d", aarp_request_count);
    nv_write((aarp_probe_interval != AARP_PROBE_INTERVAL) && 
	     (aarp_probe_interval != 0),
	     "appletalk arp probe interval %d", aarp_probe_interval);
    nv_write((aarp_probe_count != AARP_PROBE_RETRANSMIT) &&
	     (aarp_probe_count != 0),
	     "appletalk arp probe retransmit-count %d", aarp_probe_count);
}

/*
 * aarp_ager
 *
 * Note: altho this routine is called once a minute, it ages entries
 * according to the "appletalk arp timeout" interval.
 */

void
aarp_ager (void)
{
    aarpentry_t	*e = NULL;		/* entry being checked */
    aarpentry_t	*next;			/* next entry following e */
    boolean	delete_entry;
    ulong	dyn_count = 0;		/* # of dynamic entries we saw */
    ulong	fix_count = 0;		/* # of fixed entries we saw */
    ulong	del_count = 0;
    sys_timestamp wakeup_time;

    if (at_debug(aarp_debug, ATALK_NULL_MASK))
	buginf("\nAARP: aarp_ager; starting scan for old AARP entries");

    e = aarp_GetNextEntry(NULL);	/* get first entry in tree */
    while (e) {
	delete_entry = FALSE;
	if (!aarp_EntryProtected(e)) {
	    aarp_ProtectEntry(e, TRUE);
	    /*
	     * There are 3 reasons why an AARP entry shouldn't be timed
	     * out:
	     * 1. The entry is waiting for a reply; this is the case with
	     *    entries created while holding onto a packet for which
	     *    we didn't have a pre-existing entry.
	     * 2. The entry is for an AppleTalk-configured interface.
	     * 3. This is a static AARP entry. An example of this
	     *    would be a X.25 "mapped" entry. (to be implemented)
	     *    This should never be timed out.
	     */
	    switch (e->type) {
	    case AARP_STATIC:		/* simply skip these entries */
	    case AARP_ALIAS:
	    case AARP_WAITING:
	    case AARP_INTERFACE:
		++fix_count;
		break;
		
	    case AARP_DYNAMIC:
		wakeup_time =
		    TIMER_ADD_DELTA(e->time, atalkif_ArpTimeout(e->idb));
		if (AWAKE(wakeup_time)) {
		    if (at_debug(aarp_debug, ATALK_NULL_MASK))
			buginf("\nAARP: aarp_ager; timing out entry for %a",
			       e->proto_addr);
		    delete_entry = TRUE;
		    ++del_count;
		}
		++dyn_count;
		break;

	    default:
		if (at_debug(aarp_debug, ATALK_NULL_MASK))
		    buginf("\nAARP: aarp_ager; unknown entry type %d"
			   " in entry for %a", e->type, e->proto_addr);
		break;
	    }
	    aarp_UnprotectEntry(e);
	    next = aarp_GetNextEntry(e);
	    if (delete_entry) {
		aarp_FreeResources(e);
		aarp_DeleteEntry(e);
	    }
	    e = next;
	} else {			/* entry is protected */
	    e = aarp_GetNextEntry(e);
	}
    }					/* end while */
    if (at_debug(aarp_debug, ATALK_NULL_MASK))
	buginf("\nAARP: aarp_ager; finished scan; "
	       "%d fixed, %d dynamic examined, %d deleted",
	       fix_count, dyn_count, del_count);
}					/* aarp_ager */

void
aarp_StatusReport (void)
{
    RBTreeStats(aarp_tree,"  Address Resolution Table");
}
