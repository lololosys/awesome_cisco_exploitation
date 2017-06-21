/* $Id: traffic_shape.h,v 3.1.4.9 1996/09/01 21:19:18 fowler Exp $
 * $Source: /release/112/cvs/Xsys/if/traffic_shape.h,v $
 *------------------------------------------------------------------
 * Traffic Shaping Routines
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: traffic_shape.h,v $
 * Revision 3.1.4.9  1996/09/01  21:19:18  fowler
 * CSCdi60734:  Frame Relay rate enforcement is not effective
 *              Remove original fix due to its performance impact
 * Branch: California_branch
 *
 * Revision 3.1.4.8  1996/08/30  16:45:24  fowler
 * CSCdi60734:  Frame Relay rate enforcement is not effective
 *              Add registry to check for the need for media dependent
 *              traffic shaping
 * Branch: California_branch
 *
 * Revision 3.1.4.7  1996/08/07  22:12:35  fred
 * CSCdi63525:  traceback on RSVP beta image, pointing to ip fastswitching
 *         introduce 'traffic-shape group' command
 *         remove traffic in shaping queue from input-hold queue.
 * Branch: California_branch
 *
 * Revision 3.1.4.6  1996/07/20  03:18:45  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/06/04  23:12:46  fred
 * CSCdi58101:  <traffic shaping adaptive> crashes router on ethenet
 * interface
 *         Fix NULL dereference on entry to traffic_shape_adaptive(), and
 *         implement traffic-shape adaptive <lower bound>. The lower bound
 *         was left out.
 * Branch: California_branch
 *
 * Revision 3.1.4.4  1996/05/18  01:20:37  fred
 * CSCdi57567:  traffic shaping needs units changed on commands.
 *         This was done at the request of the Documentation folks,
 *         to normalize the parameters for per-interface traffic shaping,
 *         per-dlci traffic-shaping, and per-svc traffic-shaping.
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/05/17  11:23:44  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.18.1  1996/05/05  23:10:23  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.1.4.2  1996/04/22  16:10:41  fred
 * CSCdi55265:  code review comments on traffic shaping - NULL dereference
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/17  13:40:33  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef  TRAFFIC_SHAPE_H__
#define TRAFFIC_SHAPE_H__
#include "mgd_timers.h"
#include "priority_private.h"
#include "sys_registry.h"

#define	DEFAULT_SHAPE_INTERVAL		125	/* milliseconds */
#define	TARGET_SHAPE_INTERVAL_MAX	125	/* milliseconds */
#define	TARGET_SHAPE_INTERVAL_MIN	25	/* milliseconds */
#define	SHAPE_TARGET_BYTES		1000
#define	SHAPE_MIN_BITS_IN_INTERVAL	1000
#define	SHAPE_MIN_INTERVAL		10

#define TRAFFIC_SHAPE_NOT_SUPPORTED (IP_CBUS_ROUTECACHE |	\
				     IP_SSE_ROUTECACHE |	\
				     IP_FLOW_ROUTECACHE |	\
				     IP_OPTIMUM_ROUTECACHE |	\
				     IP_DISTRIBUTED_ROUTECACHE)

enum traffic_shape_timer_types {
    TIMER_START_SHAPING_INTERVAL
};


paktype *traffic_shape_dequeue(traffic_shape_t *);
void traffic_shape_enqueue(traffic_shape_t *, paktype *);
void show_traffic_shape_command(parseinfo *csb);
void traffic_shape_process(void);
void traffic_shape_init_thyself(subsystype * subsystem);
boolean traffic_shape_fecn_received(idbtype *idb, ushort in_dlci);
void traffic_shape_becn_received(idbtype *idb, fr_pvc_info_type *pvc);
void traffic_shape_add_entry (idbtype *idb, traffic_shape_t *shape);
void traffic_shape_delete_entry (void);
void shape_adaptive_configure (traffic_shape_t *);

static inline void traffic_shape_apply_credit(traffic_shape_t *shape);
void traffic_shape_out(paktype *pak);

#ifndef FR_FECN_BIT
#define FR_FECN_BIT 0x0008
#endif
/*
 * idb parameters
 */

/* Currently (01/30/97) shaping_queue and shape_fair_queue are subsets
 * of fair queueing structures in weighted fair queue defined in
 * priority_private.h
 */
STRUCTURE_REF (shaping_queue);
struct shaping_queue_ {
    shaping_queue *shape_fore;		/* queue list - back pointer */
    shaping_queue *shape_next;		/* queue list - next pointer */
    queuetype q;			/* queue of elements */
};


STRUCTURE_REF (shape_fair_queue);
struct shape_fair_queue_ {
    shaping_queue *shape_first;		/* first queue in list */   
    shaping_queue *shape_final;		/* final queue in list */
    shaping_queue *shape_last;		/* queue most recently removed from */
    shaping_queue *shape_hashed_queue;	/* queues selected by hash */
    shaping_queue *shape_reserved_queue; /* reservable queues */
    shaping_queue shape_link_queue[1 + FAIR_LINK_TYPE_HASH];
    ulong  shape_hash_mask;		/* 2^n - 1 */
    ulong  shape_sequence;		/* fair_sequence value last emitted */
    ulong  shape_high_sequence;		/* fair_sequence value last in queue */
    ulong  shape_resequence;		/* fair_sequence value before that */
    ushort shape_reserved_queue_start;	/* number of first reserved queue */
    ushort shape_queue_count;		/* number of queues total */
    ushort shape_usage_count;		/* # idbs pointing to this one */
    ushort shape_max_enqueue;		/* discard threshold */
};

struct traffic_shape_t_ {
    /* 
     * pointer to next shaping parameter block.  Access-list-based
     * traffic shaping uses a linked list of parameter blocks.
     */
    traffic_shape_t *shape_param_next;
    /*
     * store and remove packets from queue internal to traffic shaping
     */
    void	    (*shape_oqueue)(traffic_shape_t *, paktype *);
    paktype	  *(*shape_dequeue)(traffic_shape_t *);
    shape_fair_queue *shape_fair;	/* pointer to (internal) fair
					   queue structure */
    /*
     * Configured values...  These fields are taken from command line.
     */
    ulong	shape_cir;		/* target traffic rate - upper
					   bound */
    ulong	shape_sustained_bits;	/* Bc - sustained bits */
    ulong	shape_excess_bits;	/* Be - excess bits first interval */
    ushort	shape_acl;		/* access list that applies */
    ushort	shape_linktype;		/* linktype for access-list */
    /*
     * Derived values...  These fields are derived from command line
     * configured values.
     */
    long	 shape_byte_limit;	/* max number of bytes for
					   interval */
    long	 shape_byte_limit_scaled;

    ulong	shape_increment;	/* number bytes added per
					   interval */
    ulong	shape_increment_scaled;
    ulong	shape_interval;	/* duration of interval (ms) */
    /* 
     * Meat and potatoes...  Counter and timer working areas to manage
     * output rate.  Simple/passive timer is checked on every packet
     * input to determine if new threshold/bucket has opened up.
     * Managed timer will 'kick us in the butt' and let us know when
     * new Token-Bucket has opened up to signal us to send traffic
     * stored in internal shaping queue
     */
    mgd_timer     shape_restart;	/* restart timer */
    sys_timestamp shape_next_event;	/* time of the next interval start */
    long	  shape_bytes_enqueued;	/* number of bytes in hardware queue
					   this interval.  Counts up to
					   shape_byte_limit*/
    long	  shape_byte_credit;	/* # of total credits due to
					   compression 'lossage' */
    long	  shape_byte_credit_int;/* # of credits during current
					   token-bucket interval */
    void (*shape_reset)(traffic_shape_t *shape);
    /*
     * Management counters...
     */
    ulong	shape_qcount;		/* total in output queue */
    struct {
	ulong	 shape_packets;		/* packets that went through shaping */
	ulong	 shape_octets;		/* octets that went through shaping */
	ulong	 shape_packets_dly;	/* packets that were delayed */
	ulong	 shape_octets_dly;	/* octets that were delayed */
    } stat, si;
    /*
     * Frame Relay-specific configurations
     */
    ulong	shape_increment_lower; /* lower bound of FECN/BECN range */
    ulong	shape_increment_limit; /* upper bound of FECN/BECN range */

    ulong	shape_cir_lower;	/* target traffic rate - lower bound */
    ulong	shape_congestion_history; /* congestion history */

    ushort       shape_dlci_num;	/* interface allows multiple VCs */
    idbtype      *shape_idb;	    /* software idb structure is on */ 
    tinybool     shape_to_becn;		/* configured to shape to becn? */
    tinybool     shape_fecn_adapt;	/* configured to map fecn to becn? */
    tinybool     shape_fecn_create;	/* configured to set fecn? */
    tinybool     shape_dynamic;       /* structures that are non-user 
 					  defined */
    tinybool	 shape_interlock;	/* shaping at a low level */
};
  
enum traffic_shape_commands {TRAFFIC_SHAPE_RATE, TRAFFIC_SHAPE_GROUP };

static inline
shaping_queue *shaping_queue_address (traffic_shape_t *shape, 
				      ushort conversation)
{
    shape_fair_queue *fair;

    if (!shape)
	return NULL;
    fair = shape->shape_fair;
    if (!fair)
	return NULL;

    if (conversation <= fair->shape_hash_mask) {
	/* conversation number indicates a hashed queue */
	if (fair->shape_hashed_queue) {
	    /* return the hashed queue in question */
	    return(fair->shape_hashed_queue + conversation);
	}
    }

    if (fair->shape_reserved_queue_start <= conversation && 
	conversation < fair->shape_queue_count) {
	/* conversation number indicates a reserved queue */
	if (fair->shape_reserved_queue) {
	    /* return the reserved queue in question */
	    return(fair->shape_reserved_queue + 
		   (conversation - fair->shape_reserved_queue_start));
	}
    }

    /* assume it is control traffic and return a valid queue */
    return (&fair->shape_link_queue[conversation & FAIR_LINK_TYPE_HASH]);

}

#define SHAPING_RECENT_BECN 0x01
#define SHAPING_BECN_DECREASE_RATE 0x1
/*
 * bit pattern for maintaining rate is designed to "slow start". It
 * will not increase the rate if a BECN was received within the last
 * eight time units (one second). During the next eight time units it
 * will increase the rate every other time unit. After that, it will
 * increase  the rate every time unit.
 *
 * Note that there is no magic here; set it as you please.
 */
#define SHAPING_BECN_MAINTAIN_RATE 0xAAFF    


/*
 * on a Frame Relay Interface, when the periodic timer pops, we
 * adjust the rate to BECN in this way
 */
static inline 
void traffic_shape_becn_adjust(traffic_shape_t *shape)
{
    if (shape->shape_congestion_history & SHAPING_BECN_DECREASE_RATE) {
	/*
	 * BECN recently received; decrease the rate
	 */
	shape->shape_increment -= shape->shape_increment/4;
	if (shape->shape_increment < shape->shape_increment_lower)
	    shape->shape_increment = shape->shape_increment_lower;
    } else if (!shape->shape_qcount) {
	/*
	 * Traffic shaping not currently happening, do not raise rate
	 * just because the system is running idle
	 *
	 * The reasoning behind keeping the rate the same when traffic
	 * shaping is not in force is that the current rate is apparently
	 * adequate for the traffic at hand. There's no point in ramping
	 * up the limit if it is not being pushed.
	 */
	;
    } else if (shape->shape_congestion_history & SHAPING_BECN_MAINTAIN_RATE) {
	/*
	 * Traffic shaping currently happing, but we have recently recieved
	 * a BECN. Hold at current rate and see if BECNs go away
	 */
	;
    } else {
	/*
	 * Traffic shaping is currently active, but BECN has not been
	 * received in a long time, so increase the rate
	 */
	shape->shape_increment += shape->shape_increment_limit/16;

	if (shape->shape_increment > shape->shape_increment_limit)
	    shape->shape_increment = shape->shape_increment_limit;
    }
    shape->shape_congestion_history <<= 1;
}
#define FR_NONDATA_DLCI    0
#define FR_DLC_MSB_MASK    0xfc00
#define FR_DLC_LSB_MASK    0xF0
/*
 * fr_get_vc_num
 * Get the DLCI value from the packet
*/
static inline ushort fr_get_vc_num(paktype *pak)
{
    ushort dlci;
    
    if (pak->linktype == LINK_FR_SWITCH_SERIAL)
	dlci = GETSHORT(pak->mac_start);
    else
	dlci = GETSHORT(pak->datagramstart);
    
    return((dlci & FR_DLC_MSB_MASK) >> 6 | (dlci & FR_DLC_LSB_MASK) >> 4);
}

/*
 * routine to defer sending
 */
static inline
void traffic_shape_defer_sending (traffic_shape_t *shape, paktype *pak)
{
    if (pak->flags & PAK_INPUTQ) {
	if (pak->if_input)
	    pak->if_input->hwptr->input_qcount--;
	pak->flags &= ~PAK_INPUTQ;
    }

    if (shape->shape_fecn_create)
	pak->datagramstart[1] |= FR_FECN_BIT;
    shape->shape_oqueue(shape, pak);
}

/*
 * traffic_shape_adjust_size
 *
 * routine to adjust size for actual throughput characteristics of the
 * link. Initial concern is ATM: need to account for the excess bandwidth
 * required by slicing and dicing packets for ATM. Obvious extensions include
 * HDLC-based links, Ethernet, and the like
 */
static inline
ulong traffic_shape_adjust_size (hwidbtype *hwidb, ulong size)
{
    ulong number_of_cells;

    if (!hwidb)
	return size;

    switch (hwidb->enctype) {
    case ET_ATM:
    case ET_ATM_DXI:
    case ET_ATM_DXI_IETF:
	/*
	 * account for dead space in last cell,
	 * encapsulation, and AAL5 overhead
	 */
	number_of_cells = (size + 8 + 4 + 4 + 48 - 1)/48;
	/*
	 * now adjust shape to account for transmission
	 * volume on PVC
	 */
	return 53*number_of_cells;

    default:
	return size;
    }
}

/*
 * fr_dlci_to_vc_num
 * Get the DLCI number from the DLCI
*/
static inline ushort fr_dlci_to_vc_num(ushort dlci)
{
    return((dlci & FR_DLC_MSB_MASK) >> 6 | (dlci & FR_DLC_LSB_MASK) >> 4);
}

/*
 *    traffic_shape_timeslice
 *
 *    routine that checks the timer to see if a time slice has expired,
 *    and if it has updates it and moves data forward.
 */
static inline
void traffic_shape_timeslice (traffic_shape_t *shape)
{
    paktype *pak;
    ushort *output_qcount;
    hwidbtype *hwidb;
    uint size;

    if (!shape)
	return;
    if (!TIMER_RUNNING(shape->shape_next_event)) {
	/*
	 * first message in a flow; token bucket completely empty
	 * allow the transmission to go (prime the pump) and start the timer. 
	 */
	TIMER_START(shape->shape_next_event, shape->shape_interval);
    } else if (!shape->shape_interlock &&
	       TIMER_RUNNING_AND_AWAKE(shape->shape_next_event)) {
	/* gee whiz, a time slice has been crossed */
 
 	if (shape->shape_to_becn)
 	    traffic_shape_becn_adjust(shape);

	do {
	    /* step timer one interval and open up token bucket */
	    TIMER_UPDATE(shape->shape_next_event, shape->shape_interval);
	    shape->shape_bytes_enqueued -= shape->shape_increment_scaled;
  	    shape->shape_byte_credit -= shape->shape_increment_scaled;
	    if (shape->shape_bytes_enqueued <= 0) {
		shape->shape_bytes_enqueued = 0;
		break;
	    }
	}
	while (TIMER_RUNNING_AND_AWAKE(shape->shape_next_event));
	
  	traffic_shape_apply_credit(shape);

	/*
	 * see if there is anything that needs to be forwarded now
	 */
	while (shape->shape_bytes_enqueued < shape->shape_byte_limit_scaled) {
	    /*
	     * token bucket permits transmission, and there is waiting
	     * data. Take it out of the swidb queue and forward on the
	     * hwidb.  note that traffic will be selected based on its
	     * relative priority *within* the idb, and be further queued
	     * on the hwidb based on its relative priority as compared to
	     * all sub-interfaces on that hwidb. Interfaces with no
	     * sub-interfaces (PPP link etc), are just a degenerate case.
	     */
	    pak = shape->shape_dequeue(shape);
	    if (!pak) {
		if (shape->shape_bytes_enqueued <= 0) {
		    /*
		     * if the swidb is now empty and the token bucket has fully
		     * depleted, stop worrying about it.
		     */
		    shape->shape_bytes_enqueued = 0;
		    shape->shape_interlock = 0;
		}
		break;
	    }
	    output_qcount = &pak->if_output->hwptr->output_qcount;
	    shape->stat.shape_packets_dly++;
	    shape->stat.shape_octets_dly += pak->datagramsize;

	    /* 
	     * do accounting normally done in
	     * traffic_shaping_required_inline
	     *
	     * bucket not full; update bucket and let it go
	     */
	    size = pak->datagramsize;
	    hwidb = hwidb_or_null(pak->if_output);
	    shape->stat.shape_octets += size;
	    shape->stat.shape_packets++;
	    shape->shape_bytes_enqueued +=
		traffic_shape_adjust_size(hwidb, size);

	    /*
	     * this is datagram_out() minus the traffic shaping stuff.
	     */
	    if (!((QUEUE_FAIR | QUEUE_FAIR_DEF) & hwidb->queue_algorithm))
		pak->flags &= ~PAK_PRIORITIZED;
	    traffic_shape_out(pak);

	    /* 
	     * Determine if output hold queue is 'backed-up' or not.
	     * If there is something already on the queue, then do not
	     * make congestion worse by trying to add more packets
	     * from traffic shaping's internal queue. But start a timer
	     * to make sure we at least try to get the data out.
	     */
	    if (*output_qcount) {
		shape->shape_interlock = TRUE;
		break;
	    }
	}
    } else {

	/* normal case
	 *
	 * timer is running but time slice not expired, so nothing needs to
	 * be done	  
	 */
	;
    }
}
/*
 * routine that gets put in drivers to determine whether traffic
 * shaping is required. This would go in the idb->fastsend routine or
 * its ags/7000 or rsp equivalent. If traffic shaping is not enabled,
 * it costs two instructions.
 *
 * If traffic shaping is enabled but the token bucket has not triggered 
 * massive action, this normally results in 
 *	2 instructions	determine whether enabled
 *	2 instructions	determine that timer is running
 *	3 instructions	determine that timer has not expired
 *	3 instructions	determine that token bucket not full
 *	2 instructions	update token bucket
 *	1 instruction	 avoid the rest
 *
 * if the token bucket is full, we are going to return TRUE, and the
 * caller will enqueue the message to the software IDB.
 */
static inline 
traffic_shape_t *traffic_shape_required_inline (traffic_shape_t *shape,
						paktype *pak)
{
    long future;
    ulong size;
    ushort linktype;
    /* If this is a broadcast packet on a frame relay interface, send it on
     * to the broadcast queue without charging traffic shaping.  Once the
     * packet is dequeued from the broadcast queue, PAK_FR_BROADCAST will be
     * cleared and the packet should take the normal path through traffic
     * shaping.
     */

    if (pak->fr_flags & PAK_FR_BROADCAST)
	return NULL;
    
    linktype = pak->linktype;
    for (; shape; shape = shape->shape_param_next) {

	/*
	 * if packet does not match access list, do not rate control it
	 */
	if (shape->shape_acl) {
	    if (shape->shape_linktype != linktype) {
		continue;
	    }
	    if (!reg_invoke_access_check(linktype, pak, shape->shape_acl))
		continue;
	}
	if (shape->shape_dlci_num)
	    if (shape->shape_dlci_num != fr_get_vc_num(pak))
		continue;
	/*
	 * note that it matched this bucket
	 */
	pak->flags |= PAK_SHAPED;
	pak->traffic_shape = shape;

	/*
	 * update the token bucket
	 */
	traffic_shape_timeslice(shape);

	/*
	 * check message against token bucket status
	 */
	if (!shape->shape_interlock &&
	    shape->shape_bytes_enqueued < shape->shape_byte_limit_scaled) {

	    /*
	     * bucket not full; update bucket and let it go
	     */
	    size = pak->datagramsize;
	    shape->stat.shape_octets += size;
	    shape->stat.shape_packets++;
	    shape->shape_bytes_enqueued +=
		traffic_shape_adjust_size(hwidb_or_null(pak->if_output), size);
	    return NULL;
	} else {
	    /*
	     * bucket full; tell caller to queue it
	     */
	    if (!mgd_timer_running(&shape->shape_restart)) {

		/*
		 * start a timer to pop at the next timeslice boundary
		 */
		future = TIME_LEFT_SLEEPING(shape->shape_next_event);
		if (future <= 0)
		    future = 1;
		mgd_timer_start(&shape->shape_restart, future);
	    }
	    return shape;
	}
    }
    return NULL;
}

/*
 * traffic_shape_credit_bucket
 *
 * Return 'bytes' number of bytes to the traffic shaping token bucket.
 * Traffic shaping performs token-bucket statistics prior to packet's
 * arrival and processing through (various) idb->oqueue_dequeue
 * code paths.  Since header and encaps compression reduce number of bytes
 * sent out on link, traffic shaping needs to be made aware of
 * discrepancy between compressed and uncompressed datastream to be
 * able to send traffic at configured CIR.
 *
 * Implementation note:
 *
 * Traffic shaping has two data paths, unqueued and queued.  Each of
 * these paths will end up in a call to (*idb->oqueue), and the packet
 * will be hung on an appropriate (relative to interface and
 * configuration) output queue.  The credit operation implemented
 * below will be invoked just beyond the packet compression stage,
 * which is currently (3/25/97) implemented in the
 * idb->oqueue_dequeue().
 *
 * The 'problem' is that the debit (i.e. when shape_bytes_enqueued is
 * incremented) and the credit operation and _not_ single threaded.
 * It is quite possible that the credit operation _may_ occur during
 * the a token-bucket interval subsequent to the one which was
 * originally 'overcharged'.
 *
 * Check for valid pak ptr, as compression code may encounter an
 * encapsulation error and return you a NULL pak ptr.
 */
static inline
void traffic_shape_credit (paktype *pak, ushort datagramsize_old) {
    traffic_shape_t *shape;
    ushort credit_bytes;	

    if (pak && (credit_bytes = datagramsize_old - pak->datagramsize) &&
	pak->if_output && (shape = pak->if_output->traffic_shape)) {
	shape->shape_byte_credit_int += credit_bytes;
	shape->stat.shape_octets -= credit_bytes;
    } 
    return;
}

/*
 * traffic_shape_apply_credit
 *
 * Apply credit earned through compression into traffic shaping's
 * control structures.  Leave configured values alone.
 */
static inline
void traffic_shape_apply_credit (traffic_shape_t *shape) {
    
    /*
     * If credit has expired, then reset...
     */
    if (shape->shape_byte_credit < 0)
	shape->shape_byte_credit = 0;

    /*
     * Scale internal state to account for compression
     *
     * Sum interval credit into master credits, and reset.
     */
    shape->shape_byte_credit += shape->shape_byte_credit_int;
    shape->shape_byte_credit_int = 0;

    shape->shape_byte_limit_scaled = shape->shape_byte_limit +
	shape->shape_byte_credit;
    shape->shape_increment_scaled = shape->shape_increment +
	    shape->shape_byte_credit;
}
#endif
