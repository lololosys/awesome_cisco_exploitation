/* $Id: ipfast.h,v 3.3.6.10 1996/09/05 23:06:22 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast.h,v $
 *------------------------------------------------------------------
 * ipfast.h -- IP Fast Switching Definitions
 *
 * 20-March-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast.h,v $
 * Revision 3.3.6.10  1996/09/05  23:06:22  snyder
 * CSCdi68126:  declare some stuff in ip const
 *              228 out of data, only 4 from image
 * Branch: California_branch
 *
 * Revision 3.3.6.9  1996/09/04  17:16:02  ahh
 * CSCdi67885:  NAT: sub-interfaces not supported properly
 * Allow interface commands to work in sub-interface configuration
 * mode, allow translation to be enabled on some sub-interfaces
 * but not others and still have the fast-path work.
 * Branch: California_branch
 *
 * Revision 3.3.6.8  1996/08/06  08:27:35  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.3.6.7  1996/06/24  16:04:48  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.3.6.6  1996/05/07  20:00:55  xliu
 * CSCdi56777:  memd buffers are queued to process level for
 * encryption/decryption
 * Branch: California_branch
 *
 * Revision 3.3.6.5  1996/05/04  01:03:25  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.3.6.4  1996/04/30  21:17:57  dkerr
 * CSCdi43050:  IP lookup bottleneck for RSP in ISP test setup
 * Branch: California_branch
 *
 * Revision 3.3.6.3  1996/04/19  15:41:40  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.3.14.1  1996/04/09  20:37:10  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.3.6.2  1996/04/16  18:53:10  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.3.6.1  1996/03/18  20:16:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  06:58:29  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/13  01:37:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  14:25:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/03/04  15:31:01  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Better show output.  Also allow flow stats to be exported.
 *
 * Revision 3.2  1995/11/17  09:34:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:04:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/07  23:09:50  wmay
 * CSCdi39353:  MIP to XXX fast switching failures - use common inline
 * for congestion path on high end and ip multicast, and make it go a
 * little more efficiently.
 *
 * Revision 2.3  1995/09/06  17:28:03  pst
 * CSCdi39840:  IP route cache maintenance needs more subtle invalidation
 * code
 *
 * Revision 2.2  1995/06/09  03:29:39  fred
 * Change parameters to ip_pak_mac_rewrite to support IP accounting
 * in the fast path.
 * Add support of FS_DIALER in that routine.
 * Extend ipparse.c registry that displays IP datagram to display
 * UDP/TCP port as well.
 *
 * Revision 2.1  1995/06/07  20:58:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IPFAST_PREFIX_H__
#define __IPFAST_PREFIX_H__

#include "../util/radix.h"

typedef int (*ipencaps_t)(void *, iphdrtype *, ipcache_rntype *, ulong);
typedef void (*crypto_encap_t)(void *, iphdrtype *, ipcache_rntype *, 
                              ulong, void *);

/*
 * IP routing cache data structure
 *
 * Note that there is an ordering issue in the below structure.
 * prefix_length must be the byte before prefix, and mask_length
 * must be the bytes before mask.
 * Also, the 'm' field needs to be last, since it's just a placeholder
 * for mac rewrite.
 */
struct ipcache_rntype_ {
    rntype		rt_nodes[2];
    sys_timestamp	last_update;		/* last time updated */
    idbtype		*idb;			/* output sw interface */
    hwidbtype		*hwidb;			/* output hw interface */
    dialergrouptype	*dialergroup;		/* dialer group */
    ushort		refcount;               /* # ipflows referencing it */
    uchar		flags;
    uchar		prefix_keylen;
    ipaddrtype		prefix;
    ushort              subint_reference_num;
    uchar		length;                 /* encap size minus AC/FC */
    uchar		mask_keylen;
    ipaddrtype		mask;
    ipaddrtype		nexthop;
    ipaddrtype		c_mask;			/* length of creating route */
    ushort              ip_mtu;                 /* VIP temp variable */
    ushort              pad;
    crypto_encap_t      crypto_encap;           /* crypto encap vector */
    union {
	ipencaps_t      encaps;                 /* RSP rewrite vector */
	void            *inval_ptr;             /* CBUS/SP invalidation */
    } c;
    
    union {
	ulong  maclong[0];
	ushort macshort[0];
	uchar  macstring[0];
    } m;
    /* Don't put any more fields here.  Reserved for rewrite data. */
};

#define	IPCACHE_FLAG_RECURSIVE	0x01	/* built through recursive route */
#define	IPCACHE_FLAG_VALID	0x02	/* referenced from secondary cache */

/*
 * rn2cache
 *
 * The ipcache_rntype structure starts off with an application independant
 * header (the rntype).  Therfore, it's always legal to convert between a
 * rntype and an ipcache_rntype when working on leaf nodes.  We're doing
 * this as an inline, rather than a macro, so we can enforce type checking.
 *
 * Do not use this function outside of IP switching, it is application
 * specific. 
 */

inline static ipcache_rntype *
rn2entry (rntype *rn)
{
	return((ipcache_rntype *) rn);
}

/*
 * Stupid compiler forces the charlong structure to be a stack variable.
 * The shortlong structure is held in a register -- performance consideration.
 */
typedef struct shortlong_ {
    union {
	ulong lword;
	ushort sword[2];
    } d;
} shortlong;

#define MAXMACSTRING		52	/* TR ISO2 frame + full length RIF */

#define IPCACHE_MEM_STARVE_LIMIT 80000	/* no more fast switch entries */
#define IPCACHE_MEMORY_LIMIT	200000	/* fast aging begins */

/* Default values for IP cache aging */

#define	IPCACHE_AGER_DEF_INTERVAL (1*ONEMIN)	/* run every minute */
#define	IPCACHE_AGER_DEF_AGE_FAST	4	/* 1/4th of the old entries */
#define	IPCACHE_AGER_DEF_AGE_SLOW	20	/* 1/20th of the old entries */

/* Default values for IP cache invalidation */

#define IPCACHE_MIN_INVAL_DEFAULT (2*ONESEC)	/* 2 seconds minimum */
#define IPCACHE_MAX_INVAL_DEFAULT (5*ONESEC)	/* 5 seconds maximum */
#define IPCACHE_QUIET_IVL_DEFAULT 3		/* 3 seconds of quiet time */
#define IPCACHE_QUIET_THRESH_DEFAULT 0		/* 0 requests in that time */

/*
 * HW IDB ip_fast_flags for ip fast switching, these are a
 * performance consideration, it is faster to check one (1) short word
 * than look at the booleans for ip_*_accesslist and ip_accounting.
 */
#define IP_FAST_ACCESSLISTS		0x0001
#define IP_FAST_ACCOUNTING		0x0002
#define IP_FAST_INPUTACCESSLISTS	0x0004	/* enabled on ANY idb on box */
#define IP_FAST_POLICY			0x0008  /* policy enabled i/f */
#define IP_FAST_RSP_DDR			0x0010  /* DDR enabled RSP i/f */
#define IP_FAST_NAT			0x0020  /* NAT enabled interface */
#define IP_FAST_unused_bit		0x0040	/* (unused bit) */
#define IP_FAST_CRYPTO_MAP		0x0080	/* Crypto map set on output */
#define IP_FAST_INPUT_CRYPTO_MAP	0x0100	/* Crypto map on ANY input */
#define IP_FAST_INPUT_NACL		0x0200  /* enabled on ANY idb on box */


/*
 * External data
 */
extern rnhtype *ipcache_head;			/* head of prefix trie */
extern const char ipcache_hdr[];


/*
 * External functions
 */
extern boolean ip_fast_accumulate_acctg(ipaddrtype src, ipaddrtype dst,
					ulong bytes, ulong pkts);
extern void ipcache_show_entry(ipcache_rntype *entry, boolean verbose);

/*
 * ipcache_lookup_fast
 *
 * This is a very abbreviated version of rn_match that has been optimized
 * to operate ONLY on the ipcache radix trie.  It takes advantage of the
 * fact that the ipcache structure never has "less specific" entries in it,
 * so when you reach a leaf node, you know right away whether or not you
 * have a good match.  If ipcache_getmask() ever changes, this assumption
 * may change and this code will have to be rewritten. :-(
 */

inline static ipcache_rntype *
ipcache_lookup_fast (ipaddrtype destination)
{
    ipradixkeytype key;
    rntype *rn;
    ipcache_rntype *ipcache_entry;
    uchar *cp;

    cp = ipaddr2radixkey(destination, &key);
    
    /*
     * Search down from the root for the proper leaf node.
     */
    for (rn = ipcache_head->rnh_treetop; rn->rn_b >= 0; ) {
	if (rn->rn_bmask & cp[rn->rn_off])
	    rn = rn->rn_r;
	else
	    rn = rn->rn_l;
    }

    /*
     * If we hit one of the bondaries of the trie, it's not going to be
     * valid (this is an IP only assumption based on the fact that we never
     * route to 0.x.x.x or 255.x.x.x... it could change in the future)
     *
     * This icky slow "if" is in here because the root entries won't have
     * valid mask data.
     */
    if (rn->rn_flags & RNF_ROOT)
	return NULL;

    ipcache_entry = (ipcache_rntype *) rn;

    if ((destination & ipcache_entry->mask) == ipcache_entry->prefix)
	return ipcache_entry;

    return NULL;
}

/*
 * The below routine is a faster way to copy a number 
 * of words or longs.  It's not as fast as bcopy, but is faster if
 * you don't have to take the branch.  If you know the number of
 * bytes you are copying, you should do that directly.
 */
static inline void cache_copy_even_inline (ulong *to,
					   ulong *from,
					   ushort length)
{
    ushort compare;

    compare = sizeof(ulong);
    while (length >= compare) {
	PUTLONG(to++, GETLONG(from++));
	length -= compare;
    }
    if (length) {
	PUTSHORT(to, GETSHORT(from));
    }
}

/*
 * Increment or decrement refcount when adding or removing reference.
 * Since many flow entries may reference a single prefix entry, we
 * need to wait for all to be deleted before we can free the prefix
 * after an invalidate.
 */
static inline void ipcache_adjust_refcount (ipcache_rntype *cptr, int count)
{
    extern ulong ipcache_refcounts;
    
    cptr->refcount += count;
    ipcache_refcounts += count;
}
    
    

#endif /* __IPFAST_H__ */
