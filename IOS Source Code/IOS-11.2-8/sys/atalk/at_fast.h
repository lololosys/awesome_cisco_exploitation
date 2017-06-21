/* $Id: at_fast.h,v 3.2.58.3 1996/05/09 14:10:44 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_fast.h,v $
 *------------------------------------------------------------------
 * AppleTalk fast switching definitions
 *
 * August 1990, David Edwards
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_fast.h,v $
 * Revision 3.2.58.3  1996/05/09  14:10:44  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.58.2  1996/05/02  08:39:27  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.2.58.1.16.1  1996/04/27  06:33:10  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.58.1.6.1  1996/04/08  01:44:12  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.58.1  1996/03/23  01:26:16  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2.64.1  1996/03/22  09:34:53  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.50.1  1996/03/05  05:48:45  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.38.1  1996/02/27  20:40:20  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.12.1  1996/02/15  19:12:15  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.2  1995/11/17  08:42:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:49:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/22  22:46:06  wmay
 * CSCdi36246:  Appletalk doesnt fast-switch smds
 *
 * Revision 2.2  1995/06/21  02:50:39  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  20:08:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_FAST_H__
#define __AT_FAST_H__

#define FAST_SERIAL
#undef FAST_OLD_SERIAL

/*
 * Maximum size of an AppleTalk f/s cache entry, in bytes.
 * Currently, this is set to a worst-cast which will allow us
 * to store just about anything, including tunnel encapsulations.
 */
#define MAXATMACSTRING (100)

#define MAXATMACLONG (btol(MAXATMACSTRING))
#define MAXATMACSHORT (btow(MAXATMACSTRING))
#define ATRCACHE_TIMEOUT (10*ONEMIN)
#define ATRCACHE_DEPTH 25

/*
 * Constants used in writing the SNAP MAC headers.
 */

/*
 * These two longwords are used in unshifted headers.
 */
#define CONST_AA_AA_03_08	0xaaaa0308
#define CONST_00_07_80_9B	0x0007809b

/*
 * These constants are used for shifted header writes; ie, the
 * LLAP header has been stripped off and now we need to start
 * writing the header down from the DDP frame starting on a mid-word
 * boundry.
 */
#define CONST_AA		0xaa
#define CONST_AA_03_08_00	0xaa030800
#define CONST_07_80_9B_00	0x07809b00
#define CONST_AA_AA		0xaaaa
#define CONST_03_08		0x0308
#define CONST_00_07		0x0007
#define CONST_AA_03		0xaa03
#define CONST_08_00		0x0800
#define CONST_07_00		0x0700


/*
 * Unions used to help manipulate shorts and longs by bytes.
 */
typedef struct shortbyte_ {
    union {
	ushort sword;
	uchar byte[2];
    } d;
} shortbyte;

typedef struct longbyte_ {
    union {
	ulong lword;
	ushort sword[2];
	uchar byte[4];
    } d;
} longbyte;

/*
 * Selectors for dispatching into output re-encapsulation functions
 * called from the bottleneck atalk_hes_fastswitch() function.
 */
#define	AT_FS_MIN		(0)
#define	AT_FS_ETHER_SNAP	(AT_FS_MIN+1)
#define	AT_FS_ETHER_ARPA	(AT_FS_ETHER_SNAP+1)
#define	AT_FS_TOKEN_SNAP	(AT_FS_ETHER_ARPA+1)
#define	AT_FS_FDDI          	(AT_FS_TOKEN_SNAP+1)
#define	AT_FS_HDLC		(AT_FS_FDDI+1)
#define	AT_FS_FRAME		(AT_FS_HDLC+1)
#define	AT_FS_FRAME_IETF	(AT_FS_FRAME+1)
#define	AT_FS_SMDS		(AT_FS_FRAME_IETF+1)
#define	AT_FS_PPP		(AT_FS_SMDS+1)
#define AT_FS_LEX_ARPA		(AT_FS_PPP+1)
#define AT_FS_LEX_SNAP		(AT_FS_LEX_ARPA+1)
#define	AT_FS_ATM		(AT_FS_LEX_SNAP+1)
#define	AT_FS_ATM_DXI		(AT_FS_ATM+1)
#define	AT_FS_MAX 		(AT_FS_ATM_DXI+1)


/*
 * AppleTalk fast-switching cache entry.
 *
 * Note that there is a treelink at the front of this structure. It
 * must stay there. There is an implicit assumption that the RB tree
 * entry structure overlays the beginning of the fast-switching cache
 * entry.
 */
typedef struct atcachetype_ {
    treeLink	link;

    hwidbtype  *hwif;
    idbtype    *idb;			/* IDB out which we punch packet */
    atalkidbtype *atalkidb;
    ushort	byte_len;		/* Length of MAC bytes */
    ushort	word_len;		/* Length of MAC encap, in words */
    ushort	net;			/* network number */
    uchar	node;			/* node number */
    uchar	encaptype;		/* encapsulation type */
    uchar	phaseOne;		/* != 0 if this is a phase I entry */
    uchar	b_riflen;		/* rif length in bytes, 0 == no rif */
    uchar	w_riflen;		/* rif length in words, 0 == no rif */
    uchar	flags;			/* unspecified flags field */
    ushort	subint_reference_num;	/* frame-relay/atm-dxi accounting */
    ushort      dummy;

    short       delaycount;             /* delay using this entry unitl it is 0 */
    short       equalcount;             /* # of equal-cost cache entries */
    struct atcachetype_ *next_equalcache;/* next equal cost cache to be used */
    struct atcachetype_ *equalcache;    /* link list of equl cost cache entries */

    union {				/* full mac header */
	uchar macstring[MAXATMACSTRING];
	ulong maclong[MAXATMACLONG];
	ushort macshort[MAXATMACSHORT];
    } m;
    union {				/* shifted with first byte omitted */
	uchar macstring[MAXATMACSTRING];
	ulong maclong[MAXATMACLONG];
	ushort macshort[MAXATMACSHORT];
    } s;
    sys_timestamp lastupdate;		/* Timestamp of last update */

    ulong	version;		/* Entry version # */
} atcachetype;

typedef enum {		    /* Reasons for fast-switching to be disabled */
    fsEnabled=-2,	    /* Fast switch is not disabled */
    fsUnknown=-1,	    /* There has not been a reason determined yet */
    fsUserDisabled=0,	    /* User said NO APPLE ROUTE-CACHE */
    fsNotSupported,	    /* Hardware does not support */
    fsOldHardware,	    /* Hardware out of rev */
    fsPortDown,		    /* Port is down */
    fsNotRouting,	    /* Port not operational */
    fsAccessControl,	    /* Port has access control */
    fsIncompLinkProtocol,   /* Not compatible with link protocol */
    fsPriorityList,	    /* Interface has a priority list */
    fsDialOnDemand          /* Dial on Demand is on */
    
} fsReasons;


/*
 * Forward declarations.
 */
extern rbTree *atfast_cache;
extern ulong atfast_cacheVersion;

void atfast_Initialize(void);		/* Initialize the FS module */
char* atfast_Reason(atalkidbtype*); /* Rets string describing FS status */
void atfast_SetupIF(atalkidbtype*); /* Setup interface for FS */
void atfast_Ager(boolean);		/* Age the FS cache entries */
void atalk_cache_increment_version(hwidbtype *, idbtype *, boolean);
rbTree *atfast_GetCachePtr(void);
extern ulong atfast_CacheVersion(void);

/*
 * The following routines are provided for hardware support and are not
 * called by other modules.
 */
#define KeepFirstEntry(_t_) (atcachetype*)atutil_KeepFirstEntry(_t_)
#define KeepNextEntry(_t_,_r_) (atcachetype*)atutil_KeepNextEntry(_t_,&_r_->link)


/*
 * The following are inline routines which are common to all AppleTalk 
 * fast-switching implementations.
 */


/*
 * at_lookup_cache_ptr - return a cache ptr if a valid one exists.
 *
 * This is a specially tuned version of RBTreeSearch(), completely
 * inlined to remove as many function calls as possible.
 */
inline static atcachetype*
at_lookup_cache_ptr (ushort net, uchar node)
{
    register treeLink* x = atfast_cache->root;
    treeLink	 *nilnode = RBTreeGetNIL();
    treeKey	 key;
    ulong	 cnt = 0;
    atcachetype *CacheEntry;

    if (net == 0 || node == 0 || node == ATALK_BROAD_ADDR)
	return (NULL);
    key.l = atalk_address(net, node);

    atfast_cache->findRequests++;
    while (x != nilnode) {
	++cnt;
	if (x->key.l == key.l) {
	    /*
	     * Keys match, so find first so GetNext will iterate
	     */
	    if (x->left->key.l == key.l) {
		while (x->right != nilnode)
		    x = x->right;
	    } else if (x->right->key.l == key.l) {
		while (x->left != nilnode)
		    x = x->left;
	    }
	    break;			/* leave while loop, we have a node */
	}
	if (key.l < x->key.l)
	    x = x->left;
	else
	    x = x->right;
    }
    atfast_cache->findAccessed += cnt;
    if (cnt > atfast_cache->findDeepest) 
	atfast_cache->findDeepest = cnt;
    if (cnt > atfast_cache->maxNodeDepth)
	atfast_cache->maxNodeDepth = cnt;

    if (x != nilnode) {
	CacheEntry = (atcachetype *) x;
	if (CacheEntry->version == atfast_cacheVersion)
	    return (CacheEntry);
    }
    return (NULL);
}

/*
 * ATFAST_ADJUSTHOPCOUNT
 *
 * Adjust hopcount if hopcount reduction is enabled on input interface
 * and packet is crossing a domain boundary.
 *
 * domain_in is the domain packet came from, domain_out is domain packet
 * is going to (may possibly not exist), hopbyte is first byte in DDP
 * header.
 *
 * Macro assumes that the following variable has been declared:
 *     boolean incrementhop;
 * increment hop is set to FALSE if the hopbyte is altered to indicate
 * to the calling routine that normal hopcount incrementing should not
 * be performed.
 */
#define ATFAST_ADJUSTHOPCOUNT(domain_in, domain_out, hopbyte)           \
    if ((domain_in && domain_in->hop_rdctn && domain_in != domain_out)|| \
        (domain_out && domain_out->hop_rdctn && domain_in != domain_out)) { \
	hopbyte = 0x4 | (hopbyte & 0x3);                                \
        incrementhop = FALSE;                                           \
    }

/*
 * ATFAST_REMAPNETS
 *
 * Perform remapping of destination and source network networks for packets
 * to be fastswitched.  If the appropriate remapping cannot be performed or
 * no cache entry exists, fastswitching is aborted and the packet will be 
 * kicked up to the process level.
 *
 * Macro assumes that the following variables have been declared:
 *     at_Domain *domain;
 *     ushort remap_src;
 *     ushort remap_dst;
 *
 * Remapped networks are placed in remap_dst and remap_src.
 */
#define ATFAST_REMAPNETS(idb_in,  DestNet, DestNode, SrcNet)            \
    domain = idb_in->atalk_dminfo;                                      \
                                                                        \
    if (domain) {                                                       \
	if (domain->in) {                                               \
	    remap_src = atdomain_FindForwardRemap(domain->in, SrcNet);  \
	    if (!remap_src)                                             \
		return (FALSE);                                         \
	} else                                                          \
	    remap_src = SrcNet;                                         \
                                                                        \
	if (domain->out) {                                              \
	    remap_dst = atdomain_FindBackwardRemap(domain->out, DestNet); \
	    if (!remap_dst)                                             \
		return (FALSE);                                         \
	} else                                                          \
	    remap_dst = DestNet;                                        \
    } else {                                                            \
	remap_src = SrcNet;                                             \
	remap_dst = DestNet;                                            \
    }                                                                   \
                                                                        \
    cptr = at_lookup_cache_ptr(remap_dst, DestNode);                    \
    if (!cptr)                                                          \
	return (FALSE);                                                 \
                                                                        \
    if (cptr->delaycount) {                                             \
        return (FALSE);                                                 \
    }                                                                   \
    else {                                                              \
        primary = cptr;                                                 \
        cptr = primary->next_equalcache;                                \
        primary->next_equalcache = cptr->equalcache;                    \
    }                                                                   \
                                                                        \
    domain = cptr->atalkidb->atalk_dminfo;                              \
    if (domain) {                                                       \
	if (domain->out) {                                              \
	    remap_src = atdomain_FindForwardRemap(domain->out, remap_src); \
	    if (!remap_src)                                             \
		return (FALSE);                                         \
	}                                                               \
                                                                        \
	if (domain->in) {                                               \
	    remap_dst = atdomain_FindBackwardRemap(domain->in, remap_dst); \
	    if (!remap_dst)                                             \
		return (FALSE);                                         \
	}                                                               \
    }

#endif __AT_FAST_H__
