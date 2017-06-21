/* $Id: mroute.h,v 3.8.42.16 1996/08/31 00:35:24 dino Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/mroute.h,v $
 *------------------------------------------------------------------
 * mroute.h - Structure and Constant defintions for the IP Multicast
 *            routing table.
 *
 * November, 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: mroute.h,v $
 * Revision 3.8.42.16  1996/08/31  00:35:24  dino
 * CSCdi67770:  Improve readability of ipmbug().
 * Branch: California_branch
 *
 * Revision 3.8.42.15  1996/08/21  01:21:16  lwei
 * CSCdi66170:  Sparse-dense/IGMP breaks triggered joins for sparse mode
 *              groups
 * Branch: California_branch
 *
 * Revision 3.8.42.14  1996/08/13  14:52:52  tylin
 * CSCdi61338:  ATM LEC sub-interf missing from PIM-MIB
 *    Incorporate with new ifmib_swidb_get() supporting function.
 * Branch: California_branch
 *
 * Revision 3.8.42.13  1996/08/07  23:50:40  dino
 * CSCdi65425:  Make SPARSE_FLAG setting more consistent amoung rtrs in a
 *              PIM domain
 * Branch: California_branch
 *
 * Revision 3.8.42.12  1996/08/05  19:20:26  dino
 * CSCdi65068:  midb->mode is no longer needed. Configured mode need not
 *              ever change
 * Branch: California_branch
 *
 * Revision 3.8.42.11  1996/08/03  01:20:33  lwei
 * CSCdi60059:  rp entries are not timed out in the gdb
 * Branch: California_branch
 *
 * Revision 3.8.42.10  1996/07/19  15:01:55  snyder
 * CSCdi63480:  char * to char[], save a tiny bit
 *              36 bytes.
 * Branch: California_branch
 *
 * Revision 3.8.42.9  1996/06/27  07:59:58  mleelani
 * CSCdi61079:  Occasional traffic loss because of bogus DVMRP prunes
 * Branch: California_branch
 * o Dont send prune on non-RPF p2p link, if it does not appear in the OIF.
 * o Prevent bogus prune sent to RPF neighbor on tunnel when the dvmrp peer
 *   is a non-pruner.
 *
 * Revision 3.8.42.8  1996/06/27  03:27:48  mleelani
 * CSCdi60876:  Multicast fastswitching flag should be compatible with
 * unicast
 * Branch: California_branch
 * Reflect unicast fastswitching polarity
 *
 * Revision 3.8.42.7  1996/06/24  23:33:48  chwhite
 * CSCdi58419:  <no ip pim rp> should clear the mr table
 * Branch: California_branch
 *
 * Revision 3.8.42.6  1996/06/12  03:56:12  mleelani
 * CSCdi58720:  Multicast fastswitching does not do fast drop.
 * Branch: California_branch
 * DO fast drop.
 *
 * Revision 3.8.42.5  1996/06/08  04:12:03  lwei
 * CSCdi59415:  mdb timer incorrectly shrank at data forwarding time
 * Branch: California_branch
 *
 * Revision 3.8.42.4  1996/05/13  20:10:26  mleelani
 * CSCdi57016:  Multicast helper-maps not functional
 * Branch: California_branch
 * Fix helper-maps.
 *
 * Revision 3.8.42.3  1996/05/08  04:29:41  lwei
 * CSCdi54559:  IP multicast is sending larger than MTU sized packets over
 * GRE
 * Branch: California_branch
 *
 * Revision 3.8.42.2  1996/05/06  16:44:03  dino
 * CSCdi56689:  When a multipoint-VC goes away, the IP multicast cache is
 *              not fixed
 * Branch: California_branch
 *
 * Revision 3.8.42.1  1996/04/17  13:42:31  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.8  1995/12/30  20:43:16  dino
 * CSCdi46247:  Code review comments for CSCdi44742 (SPT threshold feature)
 *
 * Revision 3.7  1995/12/23  00:02:35  dino
 * CSCdi44155:  Logic for RP to prune (S,G) needs to consider DM interfaces
 *
 * Revision 3.6  1995/12/17  19:24:21  dino
 * CSCdi44742:  PIM should only switch to the SPT when traffic threshold
 *              is reached
 *
 * Revision 3.5  1995/12/13  07:03:16  dino
 * CSCdi45504:  (S,G) entries are getting DVMRP prune that dont match
 *              aggregate
 *
 * Revision 3.4  1995/12/01  18:38:43  lwei
 * CSCdi44737:  IP multicast does not support administratively scoped
 * boundaries
 *
 * Revision 3.3  1995/11/21  23:12:08  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.2  1995/11/17  17:30:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:01:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/10/06  23:04:43  lwei
 * CSCdi41753:  Multicast traffic statistics display lacks up-to-date mean
 * values
 *
 * Revision 2.8  1995/10/05  18:35:00  lwei
 * CSCdi41573:  <show ip mroute active> should show last seconds data not
 * current
 *
 * Revision 2.7  1995/09/20  17:05:36  dino
 * CSCdi36738:  Multicast sender being designated router
 *
 * Revision 2.6  1995/08/17  01:58:40  dino
 * CSCdi30823:  PIM Assert metrics use 0 for directly connected sources.
 *
 * Revision 2.5  1995/07/24  07:33:42  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.4  1995/07/17 08:28:59  dino
 * CSCdi37181:  Need to show active multicast sources - show ip mroute
 * active.
 *
 * Revision 2.3  1995/07/17  07:32:51  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/07/01  07:46:14  dino
 * CSCdi27921:  Border router needs to send Registers to RP. ISPs want PIM
 *              over DVMRP unicast routing.
 *
 * Revision 2.1  1995/06/07  21:04:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MROUTE_H__
#define __MROUTE_H__

#include <ciscolib.h>

#include "../util/radix.h"
#include "../ip/ip.h"

/*
 * General constant definitions.
 */
#define MDB_PRUNE_FREQUENCY    (2 * ONESEC)
#define MDB_EXPIRATION_TIME    (3 * ONEMIN)
#define IP_RPF_CHECK_FREQUENCY (5 * ONESEC)
#define ASSERT_RPF_FACTOR      (MDB_EXPIRATION_TIME / IP_RPF_CHECK_FREQUENCY)
#define MDB_JITTER_PCT         3
#define MDB_BPS_AVG_DURATION   60
#define IP_MPACKET_CACHE_SIZE  1024
#define IP_MPACKET_SIZE_MASK   0x3ff

/*
 * ---------- IP multicast routing table entry definitions ----------
 */

/*
 * Outgoing interface control block definition. Affectionally known as the
 * midb.
 */
typedef struct midbtype_ {
    struct midbtype_ *next;      /* Next if in outgoing interface list */
    idbtype       *idb;          /* Pointer to interface */
    ipaddrtype    next_hop;      /* Either multicast address or ALLROUTERS */
    sys_timestamp expires;       /* Keep-alive timer for interface only */
    sys_timestamp uptime;        /* When the entry was created */
    sys_timestamp prune_time;    /* When to enter interface in prune state */
    ulong         state;         /* Forward or Prune state */
    boolean       send_prune;    /* Rate-limited prune should be sent */
    sys_timestamp last_join;     /* Time last PIM Join was received */
    boolean       forwarder;     /* This system is forwarder for LAN */
    ulong         out_limit;     /* Rate-limit upper bound */
    ulong         port_number;   /* Rate-limit port number */
    sys_timestamp last_assert;   /* Time of last Assert, for rate limiting */
    ulong         uses;          /* # of times this entry used */
    ulong         mac_length;    /* Length of stored MAC header */
    uchar         *mac_header;   /* For fastswitching, for MAC rewrites */
} midbtype;

/*
 * midbtype states.
 */
#define MIDB_NULL_STATE    0
#define MIDB_FORWARD_STATE 1
#define MIDB_PRUNE_STATE   2
#define MIDB_MAX_STATES    3

/*
 * 5-byte format for radix code.
 */
typedef uchar ipradixtype[sizeof(ipaddrtype) + 1];

typedef struct mdbtype_ {
    rntype        radix_nodes[2]; /* Radix tree pointers */  
    ipaddrtype    group;          /* Multicast group address */
    ipradixtype   source;         /* Source address */	
    ipradixtype   mask;           /* Network mask associated with source */
    ulong         total_received; /* Total number of packets received for
 				     this mroute. */
    ulong         uses;           /* # of times this entry was used */
    ulong         uses_sec;       /* # of uses per second */
    ulong	  uses_sec_last;  /* # of uses in last 1 second */
    ulong         rpf_failed;     /* # of failed RPF lookups when forwarding */
    ulong         bytes;          /* # of total bytes sent by source */
    ulong         bits_sec;       /* Bits per second for rate-limit checking */
    ulong         bits_sec_last;  /* Bits per second of last 1 second */
    ulong         bits_min;       /* Bits per minute for console command */
    ulong         in_limit;       /* Rate-limit upper bound */
    ulong         port_number;	  /* Rate-limit port number */
    idbtype       *if_input;      /* Expected incoming if - from RPF lookup */
    ipaddrtype    rpf_nbr;        /* Upstream router towards source or RP */
    queuetype     if_outputQ;     /* Outgoing interface list */
    sys_timestamp expires;        /* Keep-alive timer for this entry */
    ulong expiration_time;        /* The timer value used to reset "expires"*/
    sys_timestamp uptime;         /* How long since this entry was created */
    ulong         flags;          /* Defined below */
    sys_timestamp send_prune;     /* For rate-limiting prunes */
    sys_timestamp send_join;      /* When to send delayed join */
    boolean       send_graft;     /* Need to send a graft message for (S,G)? */
    ulong         metric_pref;    /* Stored from Assert packets */
    ulong         metric;         /* Stored from Assert packets */
    tinybool      fastswitch;     /* Fastswitching allowed to use this entry */
    sys_timestamp lastinput_fs;   /* Last time entry was touched by fs code */
    ushort	  ip_min_mtu;	  /* Minimal MTU of all outgoing interfaces */
    uchar         rpf_type;       /* What table was RPF done from */
    uchar         use_assert_rpf; /* RPF neighbor determined from Assert */
    uchar         bits_age_cnt;   /* # of seconds bits_min has been counting */
    tinybool      register_flag;  /* Waiting for Reg-Stop from the RP */
} mdbtype;

/*
 * mdbtype flag definitions.
 */
#define PRUNE_FLAG       0x01   /* All outgoing interfaces in prune state */
#define SPARSE_FLAG      0x02   /* Group is in active sparse mode */
#define CONNECTED_FLAG   0x04   /* Directly connected members exist */
#define LOCAL_FLAG       0x08   /* This system is a member */
#define SET_REG_FLAG     0x10   /* We send Registers for this entry */
#define RP_BIT_FLAG      0x20   /* Don't send joins toward source */
#define SPT_FLAG         0x40   /* Data is being received on SPT tree */
#define FAST_FLAG        0x80   /* All outgoing interfaces are fastswitched */
#define JOIN_FLAG        0x100  /* (S,G) created due to spt thresholds */

/*
 * rpf_type values.
 */
#define RPF_UNICAST 0
#define RPF_DVMRP   1
#define RPF_STATIC  2

/*
 * RP related information stored in the (*,G) or gdb data structure. This
 * is learned information. Configured RP information is in another (PIM)
 * data structure.
 */
typedef struct mdb_rptype_ {
    ipaddrtype    address;  /* RP address associated with group */
    sys_timestamp uptime;   /* How long in up or down state */
    sys_timestamp expires;  /* Time-out if no RP-reachbables received */
} mdb_rptype;

/*
 * mdb_rptype states. Also pim_rptype states.
 */
#define RP_UP        0  /* RP is known to be up. Default state when created */
#define RP_DOWN      1  /* RP is know to be down */
#define RP_COMING_UP 2  /* RP just came up, set register_flags in (S,G) */

/*
 * This entry is the (*,G) entry as well as the pointer to all sources for
 * group G.
 */
typedef struct gdbtype_ {
    struct gdbtype_ *next;          /* Next entry in hash table */
    rnhtype         *radix_head;    /* Radix tree for source addresses */
    mdbtype         mdb;            /* This is the (*,G) entry */
    mdb_rptype      rp;		    /* RP address associated with group */
    sys_timestamp   send_rpreach;   /* When to send RP-reachable if RP */
    sys_timestamp   send_graft_time;/* When to send graft for (*,G) */
    boolean         join_spt;       /* For SPT thresholding */
} gdbtype;

/*
 * Static mroute type definition.
 */
typedef struct mroutetype_ {
    struct mroutetype_ *next;
    ipaddrtype source;
    ipaddrtype mask;
    uchar      distance;
    pdbtype    *pdb;
    uchar      *route_map;
    ipaddrtype rpf_address;
    idbtype    *idb;
} mroutetype;

/*
 * Rate-limit command data structure.
 */
struct ratetype_ {
    struct ratetype_ *next;
    ulong kbps;
    short media_type;
    acl_headertype *group_acl;
    acl_headertype *source_acl;
    uchar in;
};

/*
 * IP multicast helper map structure.
 */
typedef struct ipmhelpertype_ {
   struct ipmhelpertype_ *next;
   ipaddrtype address_before;	/* Address before translation */
   ipaddrtype address_after;    /* Address after translation */
   acl_headertype *acl;         /* Access list for translation */
   uchar ttl;                   /* TTL remapping value. */ 
} ipmhelpertype;

/*
 * IP multicast mpacket structure. For debugging duplicates and loops in
 * multicast routing. The first 3 fields (the IP header, sport, and dport)
 * are bcopy() from the packet. Any new fields go after this and should
 * adjust the length.
 */
typedef struct ipmpackettype_ {
    iphdrtype     ip;
    ushort        sport;
    ushort        dport;
    sys_timestamp timestamp;
} ipmpackettype;

/*
 * Multicast routing macro definitions.
 */
#define MROUTE_BUGINF(Group,Str) \
    if (mroute_debug &&          \
	(!mroute_group_debug ||  \
	 mroute_group_debug == (ipaddrtype) Group)) buginf Str

#define MCACHE_DEBUG(Group)      \
    (mcache_debug && (!mcache_group_debug || mcache_group_debug == Group))

#define MCACHE_BUGINF(Group,Str) if (MCACHE_DEBUG(Group)) buginf Str

#define MDB_OUTPUTQ_NULL(Mdb) \
    (!(Mdb)->if_outputQ.count || ((Mdb)->flags & PRUNE_FLAG))

/*
 * Macro to call debugging info. There are 3 levels of displays.
 */
#define IPMBUG(Pak, String, Level) \
    if (ipmpkt_debug && ipmpkt_debug_check(Pak)) ipmbug(Pak, Level, String);

#define IPMBUG_MAC 1
#define IPMBUG_IP  2
#define IPMBUG_ALL 3

/*
 * Test if group is in Sparse or Dense mode. If not in sparse-mode, it is in
 * dense-mode.
 */
#define IS_SPARSE_GROUP(gdb) (gdb->mdb.flags & SPARSE_FLAG)

#define FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) \
    FOR_ALL_SWIDBS_IN_LIST(SWIDBLIST_IP_MCAST, l_elt, idb)

/*
 * External data declarations.
 */
extern gdbtype *ip_mroute_cache[NETHASHLEN];
extern uchar *const midb_state_str[MIDB_MAX_STATES];
extern ip_mroute_fs_change;
extern ipmpackettype *ip_mpacket_cache;
extern ulong ip_mpacket_next;

/*
 * ---------- External function declarations. ----------
 */

/*
 * mroute.c
 */
extern void ipmulticast_add_idb(idbtype *);
extern void ipmulticast_remove_idb(idbtype *);
extern void ip_multicast_command(parseinfo *);
extern void ipmulticast_ttl_command(parseinfo *);
extern void ip_mroute_command(parseinfo *);
extern mdbtype *ip_add_mroute(ipaddrtype, ipaddrtype, ipaddrtype, idbtype *,
			      ipaddrtype, uchar, ulong);
extern void ip_delete_mroute(gdbtype *, mdbtype *);
extern void ip_add_mroute_idb(gdbtype *, mdbtype *, idbtype *, ipaddrtype,
			      ulong, ulong, boolean);
extern void ip_delete_mroute_idb(mdbtype *, idbtype *, ipaddrtype);
extern boolean ip_mforward(paktype *);
extern mdbtype *ip_mroute(paktype *, iphdrtype *);
extern inline void ip_mroute_replicate(mdbtype *, paktype *);
extern gdbtype *ip_get_gdb(ipaddrtype);
extern mdbtype *ip_get_mdb(gdbtype *, ipaddrtype, ipaddrtype);
extern mdbtype *ip_next_specific_mdb(gdbtype *, ipaddrtype, ipaddrtype, 
				     ipaddrtype);
extern mdbtype *ip_next_mdb(gdbtype *, ipaddrtype);
extern mdbtype *ip_get_best_mdb(gdbtype *, ipaddrtype);
extern mdbtype *ip_get_worst_mdb(gdbtype *, ipaddrtype, ipaddrtype);
extern midbtype *ip_get_midb(queuetype *, idbtype *, ipaddrtype, boolean);
extern void ip_mfree_outputQ(queuetype *);
extern void ip_show_mroute(parseinfo *);
extern int ip_display_mroute(rntype *, va_list);
extern void ip_display_one_mroute(gdbtype *, mdbtype *, boolean);
extern inline void ip_addr2radix(ipaddrtype, ipradixtype);
extern inline ipaddrtype ip_radix2addr(ipradixtype);
extern void ip_build_outputQ(queuetype *, gdbtype *, mdbtype *);
extern idbtype *ipmulticast_resolve_nexthop(ndbtype *, ipaddrtype *, int);
extern idbtype *ip_rpf_lookup(ipaddrtype, ipaddrtype, ipaddrtype *, 
			      ipaddrtype *, ipaddrtype *, uchar *, int);
extern void ip_mroute_ager(void);
extern int ip_age_one_mroute(rntype *, va_list);
extern boolean ip_mroute_has_members(gdbtype *, mdbtype *);
extern void ip_clear_mroute_radix(gdbtype *);
extern int ip_clear_one_mroute_radix(rntype *, va_list);
extern void ip_update_mdb_flags(gdbtype *, mdbtype *);
extern void ip_show_mroute_count(ipaddrtype, ipaddrtype, boolean);
extern int ip_count_mroute(rntype *, va_list);
extern int ip_count_one_mroute(rntype *, va_list);
extern void ip_show_mroute_active(ipaddrtype, ulong);
extern int ip_walk_active_mroute (rntype *, va_list);
extern void ip_clear_mroute_command(parseinfo *);
extern void ip_clear_mroute_cache(ipaddrtype, acl_headertype *);
extern boolean ip_rpf_check_mroute(gdbtype *, mdbtype *);
extern boolean ip_multicast_enabled(idbtype *, long*);
extern void ip_check_mdb_fastswitch(mdbtype *);
extern void ip_adjust_tunnel_mroutes(idbtype *);
extern int ip_adjust_one_tunnel_mroute(rntype *, va_list);
extern ulong ipmulticast_rate_limit(mdbtype *, midbtype *, idbtype *, boolean);
extern mdbtype *ip_create_mroute_state(ipaddrtype, ipaddrtype);
extern void ipmulticast_send_prune(mdbtype *);
extern boolean ipmulticast_loopback(paktype *);
extern boolean ipmulticast_helper_forward(paktype *);
extern boolean ipmulticast_test_boundary(idbtype *, ipaddrtype);
extern void ip_store_mdb_timer(mdbtype *);
extern void ip_show_mroute_static(void);

/*
 * ipmulticast_init.c
 */
extern void ipmulticast_init(subsystype *);

/*
 * ipmfast.c
 */
extern void ipmulticast_fs_setup_interface(hwidbtype *);
extern void ip_mcache_update(paktype *);
extern void ip_invalidate_mcache(idbtype *);
extern int ip_invalidate_each_mcache(rntype *, va_list);
extern void ip_mroute_cache_command(parseinfo *);
extern void ip_show_mcache(parseinfo *);
extern int ip_display_mcache(rntype *, va_list);
extern void ip_display_mcache_entry(mdbtype *, gdbtype *);
extern void ip_mdb_toggle_fs(mdbtype *, boolean, uchar *);
extern boolean ipmulticast_les_fastswitch(paktype *);
extern inline boolean ipmulticast_rpf_fail_fast_drop_inline(idbtype *,
						            mdbtype *,
                                                            ipaddrtype);

/*
 * ipmfast_ags.c, ipmfast_c7000.c, and ipmfast_les.c.
 */
extern void ipmulticast_fs_init(subsystype *);
extern boolean ipmulticast_hes_fastswitch(ipaddrtype, hwidbtype *);

/*
 * Inline functions.
 */

/*
 * ip_get_gdb_inline
 *
 * Find group descriptor block from the multicast hash table.
 */
static inline gdbtype *ip_get_gdb_inline (ipaddrtype group)
{

    gdbtype *gdb;
    int     hash;

    hash = nethash(group);
    for (gdb = ip_mroute_cache[hash]; gdb; gdb = gdb->next) {
	if (gdb->mdb.group == group) break;
    }
    return(gdb);
}

/*
 * ip_get_best_mdb_inline
 *
 * Find the multicast descriptor block from the group's radix tree. A 
 * longest match lookup is performed.
 */
static inline mdbtype *ip_get_best_mdb_inline (gdbtype *gdb, ipaddrtype source)
{

    mdbtype     *mdb;
    ipradixtype address;
    ipaddrtype  addr, mask;

    /*
     * There is no radix tree hung off of this group. Either it was
     * never created or we're looking at this at interrupt level and
     * the ip_delete_mroute() has not finished deleting the gdb.
     */
    if (!gdb->radix_head) return(NULL);

    ip_addr2radix(source, address);
    mdb = (mdbtype *) rn_match(address, gdb->radix_head);
    
    /*
     * If nothing found, return.
     */
    if (!mdb) return(NULL);

    /*	
     * Verify that radix entry is a exact match or a least specific route.
     */
    addr = ip_radix2addr(mdb->source);
    mask = ip_radix2addr(mdb->mask);
    if ((source & mask) != addr) return(NULL);

    /*
     * Otherwise, return longest matched entry.
     */
    return(mdb);
}


/*
 * ip_get_midb_inline
 * 
 * Get specific multicast idb descriptor block from an outgoing interface
 * list. A match on the idb is performed.
 */
static inline midbtype *ip_get_midb_inline (queuetype *queue, idbtype *idb,
					    ipaddrtype next_hop,
					    boolean nbma_sparse_check)
{
    midbtype *midb = NULL;

    for (midb = (midbtype *) queue->qhead; midb; midb = midb->next) {
	if (midb->idb != idb) continue;
	if ((nbma_sparse_check) && (idb->pim_nbma_mode) && 
	    (midb->next_hop != next_hop)) continue;
	break;
    }
    return(midb);
}

/*
 * ipmulticast_cache_header_inline
 *
 * Write header information into cache when "ip multicast cache-headers" is
 * enabled.
 */
static inline void ipmulticast_cache_header_inline (iphdrtype *ip)
{
    ipmpackettype *mpacket;

    if (ip_mpacket_cache) {
	mpacket = ip_mpacket_cache + (ip_mpacket_next & IP_MPACKET_SIZE_MASK);
	GET_TIMESTAMP(mpacket->timestamp);
	bcopy(ip, mpacket, sizeof(iphdrtype) + 4);
	ip_mpacket_next++;
    }
}

/*
 * ip_rate_limit_check
 *
 * Check whether or not number of bytes exceeds kbps limit.  Possibly 
 * (whenever port_number != 0) check that port matches.  Note that port_number
 * may be set to -1 when indicates that no match is desired (note carefully
 * the promotion of dport to a ulong).  Return TRUE if we should rate-limit.
 * FALSE otherwise.
 */
static inline boolean ip_rate_limit_check (uchar prot, ushort dport,
					   ulong bits_sec, int bytes,
					   ulong bps_limit, ulong port_number)
{
    if (!port_number ||	/* !port_number means rate-limit all ports */
	(prot == UDP_PROT && port_number == (ulong) dport)) {
	if (bits_sec + (bytes << 3) > bps_limit) {
	    return (TRUE);
	}
    }
    return (FALSE);
}

#endif __MROUTE_H__

