/* $Id: priority_private.h,v 3.1.2.6 1996/08/24 04:59:47 fred Exp $
 * $Source: /release/112/cvs/Xsys/if/priority_private.h,v $
 *------------------------------------------------------------------
 * Prototypes for if/priority.c (consistent with interface_private.h)
 *
 * April 1996, Gordon Cox
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: priority_private.h,v $
 * Revision 3.1.2.6  1996/08/24  04:59:47  fred
 * CSCdi67070:  remove upper bound from red queues
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/08/14  13:55:11  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * Prevent set_default_queueing from modifying the multilink bundle
 * interface oqueue vector
 *
 * Revision 3.1.2.4  1996/07/25  23:35:12  ccartee
 * CSCdi63068:  queuing strategy cannot be recovered after reboot or
 * download
 * Branch: California_branch
 * Invoke parser operation in traversed 'plist' order, and not in fixed
 * 'command' order.
 *
 * Revision 3.1.2.3  1996/07/13  11:36:32  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/19  00:26:11  carvalho
 * CSCdi33844:  X25 over B-channel broken
 * Branch: California_branch
 *
 * When changing encapsulations, revert to default queueing method if
 * current queueing method is illegal for new encapsulation.
 *
 * Revision 3.1.2.1  1996/05/17  11:23:22  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.2  1996/05/05  23:10:08  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.1.4.1  1996/04/26  15:20:26  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/04/26  12:28:30  ppearce
 * Add placeholder file for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __PRIORITY_PRIVATE_H__
#define __PRIORITY_PRIVATE_H__

/* The original contents of priority.h has been split, some landing here.
 * Much of the stuff here requires interface_private.h to compile.  
 * What remains in priority.h only requires interface_generic.h.
 * This file must not be explicitly or implicitly included into a .c 
 * file which also implicitly or explicitly includes interface_generic.h.
 * All in the name of decoupling, aka modularity  ... gcox.
 */
#include "priority.h"
#include "packet.h"
#include "../if/network.h"

#define PRIORITY_QLIMIT   20
#define CUSTOM_QUEUE_SIZE PRIORITY_QLIMIT
#define CUSTOM_QUEUE_PACKET_SIZE 1500
#ifdef __INTERFACE_PRIVATE_H__
#define PRIORITY_IP_ROUTECACHE (IP_FAST_ROUTECACHE|IP_FLOW_ROUTECACHE \
			      |IP_OPTIMUM_ROUTECACHE|IP_DISTRIBUTED_ROUTECACHE)

static inline boolean priority_ip_routecache(hwidbtype *hwidb) {
    if (hwidb->ip_routecache & ~PRIORITY_IP_ROUTECACHE)
        return(FALSE);
    else
        return TRUE;
}
#endif
/* A structure for callback functions to call if an priority list is changed */ typedef void (*priority_callbackfunction) (ulong, ushort);
typedef struct _priority_callback {
    struct _priority_callback *next;
    ulong low;
    ulong high;
    priority_callbackfunction callback_function;
} priority_callback;

#define MAXIMUM_RED_SHIFT 16	/* you can scale by dividing by 65K */
#define MINIMUM_RED_SHIFT 1	/* you can scale by dividing by two */
#define RED_SIZE_THRESHOLD 100	/* smallest message RED will discard */
#define RED_STANDARD_SHIFT 9	/* default weight is Sally Floyd's 0.002 */
#define RED_SCALE(old,shift,new) (new - (old >> shift))
#define RED_RSVP_PRECEDENCE 8   /* IP Precedence + 1 */
#define RED_PRECEDENCE_LEVELS 9
struct red_parameters_ {
    ulong red_shift;		/* shift amount */
    ulong min_threshold[RED_PRECEDENCE_LEVELS];	/* Q depth managed to,
						   scaled by red_shift */
    ulong saved_threshold;	/* saved queue maximum */
    ulong max_threshold;	/* max Q depth, scaled by red_shift */
    long mean_queue_depth;	/* mean depth, scaled by red_shift */
    ulong si_packets_marked[RED_PRECEDENCE_LEVELS];
    ulong packets_marked[RED_PRECEDENCE_LEVELS]; /* number of packets 
						    discarded due to RED */
    sys_timestamp idle_time;	/* idle timestamp */
    long count;			/* packets since mark event */
};

/*
 * structure for weighted fair queue (fair) idb component
 */
#define FAIR_QUEUE_PAK_PRIORITY_WEIGHT  128 /* weight of PAK_PRIORITY traffic */
#define FAIR_QUEUE_DEFAULT_BW   2048	/* defaults to use on <= 2.048 MBPS */
#define FAIR_QUEUE_RSVP_WEIGHT  128	/* weight RSVP normalizes to */
#define FAIR_QUEUE_BASE_BURST	0x7FFF	/* burst size of a standard conversation in octets */
#define FAIR_LINK_TYPE_HASH 0x00000007  /* hash link procedure queues */
#define FAIR_QUEUE_HASH_QUEUES_DEFAULT	256
#define FAIR_RSVP_QUEUES_DEFAULT	0
#define FAIR_RSVP_QUEUES_RSVP		128
#define FAIR_QUEUE_DYN_CONVERSATIONS	(256 + FAIR_LINK_TYPE_HASH+1)
#define FAIR_QUEUE_DEFAULT_QDEPTH 64
#define FAIR_QUEUE_MAXIMUM_QDEPTH 4096
STRUCTURE_REF (fair_queue);
struct fair_queue_ {
    fair_queue *fq_fore;		/* queue list - back pointer */
    fair_queue *fq_next;		/* queue list - next pointer */
    ulong fq_discards;			/* frames discarded in queue */
    ulong si_fq_discards;		/* for "clear counters" */
    queuetype q;			/* queue of elements */
};

struct fair_queue_head_ {
    fair_queue *fq_first;	/* first queue in list */   
    fair_queue *fq_final;		/* final queue in list */
    fair_queue *fq_last;	/* queue most recently removed from */
    fair_queue *fq_hashed_queue;	/* queues selected by hash */
    fair_queue *fq_reserved_queue;	/* reservable queues */
    fair_queue fq_link_queue[1 + FAIR_LINK_TYPE_HASH];
    ulong  fq_hash_mask;		/* 2^n - 1 */
    ushort fq_reserved_queue_start;	/* number of first reserved queue */
    ushort fq_queue_count;		/* number of queues total */
    ulong  fq_sequence;         /* fair_sequence value last emitted */
    ulong  fq_high_sequence;    /* fair_sequence value last in queue */
    ulong  fq_resequence;       /* fair_sequence value before that */
    ushort fq_active_conversations;	/* #active dynamic conversations */
    ushort fq_max_active_conversations;	/* hi water mark */
    ushort fq_allocated_conversations;	/* #active allocated conversations */
    ushort fq_max_allocated_conversations;	/* hi water mark */
    ushort fq_usage_count;	/* number of idbs pointing to this one */
    ushort fq_max_enqueue;	/* discard threshold */
};


static inline int fair_queue_sum (paktype *pak, int sum) {
    return(pak->fq_hash_mask & sum);
}

static inline
fair_queue *fair_queue_address (fair_queue_head *fq, ushort conversation)
{
    if (conversation <= fq->fq_hash_mask) {
	/* conversation number indicates a hashed queue */
	if (fq->fq_hashed_queue) {
	    /* return the hashed queue in question */
	    return(fq->fq_hashed_queue + conversation);
	}
    }

    if (fq->fq_reserved_queue_start <= conversation && 
	conversation < fq->fq_queue_count) {
	/* conversation number indicates a reserved queue */
	if (fq->fq_reserved_queue) {
	    /* return the reserved queue in question */
	    return(fq->fq_reserved_queue + 
		   (conversation - fq->fq_reserved_queue_start));
	}
    }

    /* assume it is control traffic and return a valid queue */
    return (&fq->fq_link_queue[conversation & FAIR_LINK_TYPE_HASH]);

}

extern int priority_qsize(hwidbtype *, int);
extern int priority_classify(hwidbtype *, paktype *);
extern boolean priority_enqueue(hwidbtype *, paktype *, enum HEADTAIL);

extern ulong custom_qbytecnt(hwidbtype *, int);
extern boolean priority_create_callback(priority_callbackfunction, ulong,
                                        ulong);
extern boolean priority_destroy_callback(void *);
extern void build_fair_queue_head(hwidbtype * idb,
                                ushort congestion_threshold,
                                ushort number_hashed_queue,
                                ushort number_reserved_queue);

extern boolean fair_enqueue(hwidbtype *, paktype *, enum HEADTAIL);

extern void show_fair_queue_command(hwidbtype *idb);
extern pritype *priority_allocate_list(boolean custom, int number);
extern void show_queueing(int type, int list_no);
extern void holdq_display(hwidbtype *idb);
ushort fair_reserve_conversation(hwidbtype * idb);
void fair_release_conversation(hwidbtype * idb, ushort conversation_number);
plist_entrytype pl_get_entrytype(plisttype *plist);
int pl_map_pltype2pricmd(plist_entrytype pl_type);
ulong maximum_interface_queue_depth(hwidbtype *hwidb);


/*
 * priority_chain.c
 */
extern void priority_parser_init(void);
extern uint diallst_getlink(int list);

/*
 * Determine if an encapsulation supports custom queueing.
 */
static inline
boolean custom_queue_supported (hwidbtype *idb) 
{
    /* custom queueing supported on all but X25 interfaces */
    return (!is_x25(idb));
}

/*
 * Determine if an encapsulation supports priority queueing.
 */
static inline
boolean priority_queue_supported (hwidbtype *idb) 
{
    /* priority queueing supported on all interfaces except:
     *        X25 interfaces
     *        tunnel interfaces
     */
    
    return (!(is_x25(idb) || is_tunnel(idb)));
}

/*
 * inlines to determine when WFQ should be the operating mode
 */
#ifdef __INTERFACE_PRIVATE_H__
/*
 * ask if it is one of the supported encapsulations
 */
static inline
boolean fair_queue_supported (hwidbtype *idb)
{
    /*
     *  Fair queueing is not supported on a multilink master
     *  bundle interface, virtual-template and virtual-access
     *  interface.
     */
    if ((idb->firstsw->is_bundle_interface) ||
	(idb->type == IDBTYPE_VTEMPLATE) ||
	(idb->type == IDBTYPE_VACCESS))
	return FALSE;

    switch(idb->enctype) {
    default:
	return FALSE;

    case ET_NULL:			/* accept until encaps set */
    case ET_ARPA:			/* Ethernet - DDN style */
    case ET_SAP:			/* Ethernet - HP blew it */ 
    case ET_SNAP:			/* 802.2 SNAP types */
    case ET_HDLC:			/* Serial - raw HDLC */
    case ET_PPP:			/* Serial PPP */
    case ET_SMDS:			/* Serial SMDS */
    case ET_ATM:                        /* ATM interface encaps */
    case ET_ATM_DXI:                    /* ATM DXI implementation */
    case ET_ATM_DXI_IETF:               /* atm-dxi with IETF encaps */
    case ET_FRAME_RELAY:		/* Frame Relay */
    case ET_LLC2:			/* LLC 2 */
    case ET_SLIP:			/* Async SLIP encapsulation */
    case ET_FR_IETF:			/* Frame Relay with IETF encaps */
    case ET_LAPB:			/* LAPB enapsulation */
	return TRUE;
    }
}
/*
 * b o o l e a n   * c o m p a r e _ f a i r _ q u e u e
 *
 * Determine if fair queue configurations are equal.  If one or both
 * of the passed interfaces isn't configured for fair queueing, return FALSE.
 */

static inline
boolean compare_fair_queue (hwidbtype *idb1, hwidbtype *idb2) {
     fair_queue_head *fq1,*fq2;
       
     fq1 = idb1->fair_queue_head;
     fq2 = idb2->fair_queue_head;
     
     if (!fq1 || !fq2)
	  return (FALSE);
     
     /*
      * Compare the congestion threshold, size of hashed queue and size
      * of reserved queue (in that order) within the return below.
      * For understanding of the actual field assignments, refer to:
      * build_fair_queue_head() where these values get stuffed!
      * Return TRUE if all fields are equal, else return FALSE...
      */
     return ((idb1->priority_list  == idb2->priority_list)   &&
	     (fq1->fq_hash_mask   == fq2->fq_hash_mask)  &&
	     ((fq1->fq_queue_count - fq1->fq_reserved_queue_start) ==
	      (fq2->fq_queue_count - fq2->fq_reserved_queue_start)));
}


/*
 * is it in the appropriate bandwidth range?
 */
static inline
boolean fair_queue_bandwidth (hwidbtype *idb) 
{
    if (!idb->firstsw->visible_bandwidth)
	return(FALSE);
    if (FAIR_QUEUE_DEFAULT_BW < idb->firstsw->visible_bandwidth)
	return(FALSE);
    return(fair_queue_supported(idb));
}
/*
 * Determine if a protocol permits changes to the queuing functions.
 *
 * Some protocols will tolerate a change in the queuing vectors even
 * though they don't support fair queuing.  These protocols inspect
 * the vectors from the _serial_cfg_modified registry to ensure their
 * correct operation.
 */
static inline
boolean queuing_can_be_modifed (hwidbtype *idb) 
{
    /*
     *  X.25 uses LAPB, which depends on the _serial_cfg_modified
     *  registry invocation to install the correct queuing vectors
     */
    if (idb->enctype == ET_X25) {
	return(TRUE);
    }

    /*
     * the BVI(Bridge-group Virtual Interface) needs its own enqueue
     * routine in order to enqueue a packet from routed interfaces
     * to bridgeQ
     */
    if (is_bvi(idb)) {
        return(FALSE);
    }
    /*
     * otherwise the protocols that support WFQ permit queuing changes
     */
    return(fair_queue_supported(idb));
}
#endif __INTERFACE_PRIVATE_H__
#endif    /* __PRIORITY_H__	*/
