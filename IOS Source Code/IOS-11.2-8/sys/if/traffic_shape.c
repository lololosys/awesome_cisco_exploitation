/* $Id: traffic_shape.c,v 3.1.4.16 1996/09/01 21:19:17 fowler Exp $
 * $Source: /release/112/cvs/Xsys/if/traffic_shape.c,v $
 *------------------------------------------------------------------
 * Traffic Shaping Routines
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: traffic_shape.c,v $
 * Revision 3.1.4.16  1996/09/01  21:19:17  fowler
 * CSCdi60734:  Frame Relay rate enforcement is not effective
 *              Remove original fix due to its performance impact
 * Branch: California_branch
 *
 * Revision 3.1.4.15  1996/08/30  16:45:22  fowler
 * CSCdi60734:  Frame Relay rate enforcement is not effective
 *              Add registry to check for the need for media dependent
 *              traffic shaping
 * Branch: California_branch
 *
 * Revision 3.1.4.14  1996/08/28  12:48:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.4.13  1996/08/24  04:53:14  fred
 * CSCdi67071:  Clean up traffic-shape display per customer request
 * Branch: California_branch
 *
 * Revision 3.1.4.12  1996/08/17  00:23:37  fred
 * CSCdi61598:  spurious memory access in fair_enqueue/dequeue
 *              caused by tty interrupt interleaving with nets interrupt;
 *              need to control interrupts entirely, so use standard
 *              routines.
 * Branch: California_branch
 *
 * Revision 3.1.4.11  1996/08/07  22:12:32  fred
 * CSCdi63525:  traceback on RSVP beta image, pointing to ip fastswitching
 *         introduce 'traffic-shape group' command
 *         remove traffic in shaping queue from input-hold queue.
 * Branch: California_branch
 *
 * Revision 3.1.4.10  1996/07/20  03:18:44  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.1.4.9  1996/07/13  11:36:34  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.1.4.8  1996/07/02  21:58:23  fred
 * CSCdi55182:  %ALIGN-3-SPURIOUS: Spurious memory in shape_adjust_queue()
 *         make sure that memory handling is up to snuff
 * Branch: California_branch
 *
 * Revision 3.1.4.7  1996/06/21  22:40:34  fred
 * CSCdi61061:  default Bc wrong on traffic-shape rate.
 *         Should be the number of bits that can be sent at CIR in
 *         DEFAULT_SHAPE_INTERVAL milliseconds
 * Branch: California_branch
 *
 * Revision 3.1.4.6  1996/06/04  23:12:45  fred
 * CSCdi58101:  <traffic shaping adaptive> crashes router on ethenet
 * interface
 *         Fix NULL dereference on entry to traffic_shape_adaptive(), and
 *         implement traffic-shape adaptive <lower bound>. The lower bound
 *         was left out.
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/05/21  09:50:11  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.4.4  1996/05/18  01:20:36  fred
 * CSCdi57567:  traffic shaping needs units changed on commands.
 *         This was done at the request of the Documentation folks,
 *         to normalize the parameters for per-interface traffic shaping,
 *         per-dlci traffic-shaping, and per-svc traffic-shaping.
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/05/13  14:47:21  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/04/22  16:10:40  fred
 * CSCdi55265:  code review comments on traffic shaping - NULL dereference
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/17  13:40:31  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "access.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "subsys.h"
#include "fastswitch_registry.h"
#include "sys_registry.h"
#include "network.h"
#include "network_debug.h"
#include "../ip/ip_registry.h"
#include "../os/element.h"
#include "../os/signal.h"
#include "../wan/frame_relay.h"

enum traffic_shape_queue_types {
    PACKETS,
    OTHER_STUFF
};

static pid_t traffic_shape_pid;
static ushort shape_running;
static mgd_timer traffic_shape_root_timer;

/*
 * traffic_shape_add_entry
 * Increment the shaping count, start the timer, and start the traffic shaping
 * process if required
 */
void traffic_shape_add_entry (idbtype *idb,
			      traffic_shape_t *shape)
{
    if (!shape_running)
	traffic_shape_pid =
	    process_create(traffic_shape_process, "traffic_shape",
			   HUGE_STACK, PRIO_NORMAL);
    shape_running++;
    mgd_timer_init_leaf(&shape->shape_restart, 
			&traffic_shape_root_timer,
			TIMER_START_SHAPING_INTERVAL, idb, FALSE);
}

/*
 * traffic_shape_delete_entry
 * Decrement the shaping counter, and stop the traffic shaping process if
 * no shaping structures remain
 */
void traffic_shape_delete_entry (void)
{
    shape_running--;
    if (!shape_running)
	process_kill(traffic_shape_pid);
}

/*
 * fairly queue the traffic shaping queue
 */
void traffic_shape_enqueue (traffic_shape_t *shape, paktype *pak)
{
    shape_fair_queue *fair;
    ushort conversation;
    shaping_queue *this_queue;
    shaping_queue *previous_queue;
    shaping_queue *next_queue;
    paktype *tail;
    int comparison;
    ulong sequence;
    ushort datagramsize;

    if (!shape) {
	datagram_done(pak);
	return;
    }
    fair =  shape->shape_fair;
    if (!fair) {
	datagram_done(pak);
	return;
    }

    pak->shape_acl = shape->shape_acl;
    pak->fq_hash_mask = fair->shape_hash_mask;
    conversation = reg_invoke_determine_fair_queue_flow_id(pak->linktype, pak);
    pak->oqnumber = conversation;
    pak->flags |= PAK_PRIORITIZED;	/* don't classify again in WFQ */
    if (conversation < fair->shape_queue_count) {
	this_queue = shaping_queue_address(shape, conversation);
	if (!pak_queue_depth(&this_queue->q)) {

	    /*
	     * first message in queue for this_queue conversation
	     */
	    if (!pak_enqueue(&this_queue->q, pak)) {

		/* 
		 * pak->refcount <= 0, 
		 * pak_queue_depth(q) > maximum (how?)
		 * or could not get an element 
		 */
		goto tail_drop;
	    }
	    pak->flags |= PAK_DLQ;

	    datagramsize = pak->datagramsize;
	    sequence = fair->shape_sequence + pak->fair_weight * datagramsize;
	    comparison = sequence - fair->shape_high_sequence;
	    if (0 < comparison)	/* new queue tail? */
		fair->shape_high_sequence = sequence;
	    pak->fair_sequence = sequence;
	    previous_queue = fair->shape_first;
	    shape->shape_qcount++;
	    if (!previous_queue) {

		/* 
		 * first in entire queue - there were no active conversations
		 */
		fair->shape_first = this_queue;
		this_queue->shape_fore = NULL;
		this_queue->shape_next = NULL;
		return;
	    }

	    /* 
	     * not first conversation queue; 
	     * sort against other conversations 
	     */
	    pak = pak_queue_get_head(&previous_queue->q);
	    comparison = sequence - pak->fair_sequence;
	    if (comparison < 0) {

		/* 
		 * conversation goes to head of the list 
		 */
		this_queue->shape_fore = NULL;
		this_queue->shape_next = previous_queue;
		previous_queue->shape_fore = this_queue;
		fair->shape_first = this_queue;
		return;
	    }

	    /*
	     * conversation belongs later in the list, maybe the tail
	     */
	    next_queue = previous_queue->shape_next;
	    while (next_queue) {
		pak = pak_queue_get_head(&next_queue->q);
		/* check for NULL should be unnecessary, as this queue 
		   has waiting data */
		comparison = sequence - pak->fair_sequence;
		if (comparison < 0)
		    break;
		previous_queue = next_queue;
		next_queue = previous_queue->shape_next;
	    }
	    this_queue->shape_next = next_queue;
	    previous_queue->shape_next = this_queue;
	    this_queue->shape_fore = previous_queue;
	    if (next_queue)
		next_queue->shape_fore = this_queue;
	    return;
	} /* end empty conversation queue case */

	/*
	 * pak is enqueued where there is already data
	 * we will enqueue to the tail of the conversation queue
	 *
	 * we want to discard when
	 *
	 *	the queue as a whole is very deep, >= discard threshold
	 * AND  this is not a routing or control message (PAK_PRIORITY)
	 * AND  the sequence number of this message says it is the last
	 *      in the queue
	 */
	tail = pak_queue_get_tail(&this_queue->q);
	datagramsize = pak->datagramsize;
	sequence = tail->fair_sequence + pak->fair_weight * datagramsize;
	pak->fair_sequence = sequence;

	comparison = sequence - fair->shape_high_sequence;
	if ((pak->flags & PAK_PRIORITY) ||
	    (comparison < 0 || shape->shape_qcount < fair->shape_max_enqueue)) {
	    if (pak_enqueue(&this_queue->q, pak)) {

		/*
		 * not first in conversation - 
		 * schedule and enqueue at tail of conversation queue
		 */
		pak->flags |= PAK_DLQ;
		shape->shape_qcount++;
		if (0 < comparison)	/* new queue tail? */
		    fair->shape_high_sequence = sequence;
		return;
	    }
	}

	/* 
	 * note that it this point, one could argue for "deterministic early 
	 * discard", similar to the "Random Early Discard" algorithm.
	 *
	 * the Random Early Discard algorithm suggests that one take an early
	 * frame from what might be the overburdening conversation and pitch
	 * it. Here, we do a tail drop from the overburdening conversation.
	 * We could, if we chose, add the tail but drop the head.
	 *
	 * I didn't do that, because I was concerned about benchmarking (would
	 * it make the overburdening conversation in a traffic generator test
	 * simply drop out of the mix or become unpredictable?) and because we
	 * would now have to sort the conversation into place. Here, we simply
	 * count a discard against the overburdening conversation.
	 */
    }

  tail_drop:
    hwidb_or_null(pak->if_output)->counters.output_drops[PRIORITY_NORMAL]++;
    hwidb_or_null(pak->if_output)->counters.output_total_drops++;
    datagram_done(pak);
}

paktype *traffic_shape_dequeue(traffic_shape_t *shape)
{
    shape_fair_queue *fair;
    paktype *pak;
    paktype *next_message;
    shaping_queue *this_queue;
    shaping_queue *previous_queue;
    shaping_queue *next_queue;
    int comparison;
    ulong sequence;

    if (!shape) {
	return NULL;
    }
    fair =  shape->shape_fair;
    if (!fair) {
	return NULL;
    }

    /*
     * search for a matching conversation
     */
    previous_queue = NULL;
    this_queue = fair->shape_first;
    while (this_queue) {
	pak = pak_queue_get_head(&this_queue->q);
	if (!pak)			/* this should never happen */
	    return NULL;
	if (pak->shape_acl == shape->shape_acl)
	    break;
	previous_queue = this_queue;
	this_queue = this_queue->shape_next;
    }
    if (!this_queue)
	return(NULL);			/* no matching conversation */

    pak = pak_dequeue(&this_queue->q);
    if (!pak)  /* this should never happen */
	return NULL;

    pak->flags &= ~PAK_DLQ;
    fair->shape_last = this_queue;
    shape->shape_qcount--;
    fair->shape_resequence = fair->shape_sequence;
    fair->shape_sequence = pak->fair_sequence;
    next_message = pak_queue_get_head(&this_queue->q);
    next_queue = this_queue->shape_next;

    if (!next_message) {

	/*
	 * conversation depleted; remove from the list
	 */
	if (!previous_queue) {
	    fair->shape_first = next_queue;
	    if (next_queue)
		next_queue->shape_fore = previous_queue;
	} else {
	    previous_queue->shape_next = next_queue;
	    if (next_queue)
		next_queue->shape_fore = previous_queue;
	}
	this_queue->shape_next = NULL;
	this_queue->shape_fore = NULL;
	return pak;
    }
    if (!next_queue) {

	/*
	 * conversation not depleted, but no other relevant conversations
	 */
	return pak;
    }

    /* 
     * sort against other conversations 
     */
    sequence = next_message->fair_sequence;
    next_message = pak_queue_get_head(&next_queue->q);
    comparison = sequence - next_message->fair_sequence;
    if (comparison <= 0) {
	/* conversation belongs at head of the list, where it already is */
	return pak;
    }
    /*
     * conversation belongs later in the list, maybe the tail
     */
    if (!previous_queue) {
	fair->shape_first = next_queue;
	if (next_queue)
	    next_queue->shape_fore = previous_queue;
    } else {
	previous_queue->shape_next = next_queue;
	if (next_queue)
	    next_queue->shape_fore = previous_queue;
    }

    while (next_queue) {
	next_message = pak_queue_get_head(&next_queue->q);
	comparison = sequence - next_message->fair_sequence;
	if (comparison < 0)
	    break;
	previous_queue = next_queue;
	next_queue = previous_queue->shape_next;
    }
    this_queue->shape_next = next_queue;
    if (next_queue)
	next_queue->shape_fore = this_queue;
    previous_queue->shape_next = this_queue;
    this_queue->shape_fore = previous_queue;
    return pak;
}

/*
 *    traffic_shape_dequeue_shim
 *
 *    routine that checks the timer to see if a time slice has expired,
 *    and if it has updates it and moves data forward.
 */
paktype *traffic_shape_dequeue_shim (hwidbtype *hwidb)
{
    paktype *pak_from_holdq;
    paktype *pak_from_shaping;
    traffic_shape_t *shape;
    ulong size;

    pak_from_holdq = hwidb->shape_oqueue_dequeue(hwidb);
    if (!pak_from_holdq) {
	/* 
	 * no pak - nothing to remove from shaping queue
	 */
	return pak_from_holdq;
    }

    if (!(pak_from_holdq->flags & PAK_SHAPED)) {
	/*
	 * shaping not noted, so just let it go
	 */
	return pak_from_holdq;
    }

    shape = pak_from_holdq->traffic_shape;
    if (shape->shape_bytes_enqueued >= shape->shape_byte_limit_scaled) {
	while (TIMER_RUNNING_AND_AWAKE(shape->shape_next_event) &&
	       (shape->shape_bytes_enqueued >= shape->shape_byte_limit_scaled)) {
	    /* gee whiz, a time slice has been crossed */
 
	    if (shape->shape_to_becn)
		traffic_shape_becn_adjust(shape);

	    /* step timer one interval and open up token bucket */
	    TIMER_UPDATE(shape->shape_next_event, shape->shape_interval);
	    shape->shape_bytes_enqueued -= shape->shape_increment_scaled;
	    shape->shape_byte_credit -= shape->shape_increment_scaled;
	    traffic_shape_apply_credit(shape);
	    if (shape->shape_bytes_enqueued <= 0) {
		shape->shape_bytes_enqueued = 0;
		break;
	    }
	}

	if (shape->shape_bytes_enqueued >= shape->shape_byte_limit_scaled) {
	    /*
	     * shaping token bucket exhausted - stop asking for a while
	     */
	    if (!mgd_timer_running(&shape->shape_restart) && 
		shape->shape_bytes_enqueued) {

		/*
		 * start a timer to pop at the next timeslice boundary
		 */
		mgd_timer_start(&shape->shape_restart, shape->shape_interval);
	    }
	    shape->shape_interlock = FALSE;
	    return pak_from_holdq;
	}
    }

    pak_from_shaping = shape->shape_dequeue(shape);
    if (!pak_from_shaping) {
	/*
	 * no shaped message? timer will eventually clean this up
	 */
	shape->shape_interlock = FALSE;
	return pak_from_holdq;
    }

    /*
     * put the shaped message into the hold queue (no change in depth)
     * and return the hold queue's buffer. Lots of bean counting here
     */
    shape->stat.shape_packets_dly++;
    shape->stat.shape_octets_dly += pak_from_shaping->datagramsize;
    size = pak_from_shaping->datagramsize;
    shape->stat.shape_octets += size;
    shape->stat.shape_packets++;

    shape->shape_bytes_enqueued += traffic_shape_adjust_size(hwidb, size);
    if (!((QUEUE_FAIR | QUEUE_FAIR_DEF) & hwidb->queue_algorithm))
	pak_from_shaping->flags &= ~PAK_PRIORITIZED;
    holdq_enqueue_fast(hwidb, pak_from_shaping);
    return pak_from_holdq;
}

/*
 * show_interface_traffic_shape
 *
 * display the parameters
 */
static void
show_traffic_shape_idb_stats (idbtype *idb)
{
    traffic_shape_t *shape = idb->traffic_shape;
    for (; shape; shape = shape->shape_param_next) {
	if (automore_quit())
	    break;
	if (shape->shape_acl)
	    printf("\n%9s %6u %9u %9u %9u %9u %9u %s",
		   idb->short_namestring,
		   shape->shape_acl,
		   shape->shape_qcount,
		   (shape->stat.shape_packets - shape->si.shape_packets) +
		   (shape->stat.shape_packets_dly - shape->si.shape_packets_dly),
		   (shape->stat.shape_octets - shape->si.shape_octets) + 
		   (shape->stat.shape_octets_dly - shape->si.shape_octets_dly),
		   (shape->stat.shape_packets_dly - shape->si.shape_packets_dly),
		   (shape->stat.shape_octets_dly - shape->si.shape_octets_dly),
		   mgd_timer_running(&shape->shape_restart)?"yes":"no");
	else
	    printf("\n%9s %6s %9u %9u %9u %9u %9u %s",
		   idb->short_namestring,
		   "",
		   shape->shape_qcount,
		   (shape->stat.shape_packets - shape->si.shape_packets) +
		   (shape->stat.shape_packets_dly - shape->si.shape_packets_dly),
		   (shape->stat.shape_octets - shape->si.shape_octets) + 
		   (shape->stat.shape_octets_dly - shape->si.shape_octets_dly),
		   (shape->stat.shape_packets_dly - shape->si.shape_packets_dly),
		   (shape->stat.shape_octets_dly - shape->si.shape_octets_dly),
		   mgd_timer_running(&shape->shape_restart)?"yes":"no");
    }
}

/*
 * traffic_shape_capture_if
 */
static void traffic_shape_capture_if (hwidbtype *hwidb) {

    idbtype *idb;
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	if (idb->traffic_shape && 
	    hwidb->oqueue_dequeue != traffic_shape_dequeue_shim) {
	    hwidb->shape_oqueue_dequeue = hwidb->oqueue_dequeue;
	    hwidb->oqueue_dequeue = traffic_shape_dequeue_shim;
	    break;
	}
    }
}

/*
 * display traffic shaping characteristics
 */
void
show_traffic_shape_command_stats (parseinfo *csb) 
{
    idbtype *idb;
    idb = GETOBJ(idb, 1);

    if (!idb) {
	automore_enable(NULL);
	printf("\n%9s %6s %9s %9s %9s %9s %9s %s",
	       "", "Access", "Queue", "Packets", "Bytes",
	       "Packets", "Bytes", "Shaping"); 
	printf("\n%9s %6s %9s %9s %9s %9s %9s %s",
	       "I/F", "List", "Depth", "", "", "Delayed", "Delayed", "Active"); 
	FOR_ALL_SWIDBS(idb) {
	    if (automore_quit())
		break;
	    show_traffic_shape_idb_stats(idb);
	} 
	automore_disable();
    } else if (!idb->traffic_shape) {

	printf("\nTraffic shaping not configured on %s", idb->namestring);

    } else {
	automore_enable(NULL);
	printf("\n%9s %6s %9s %9s %9s %9s %9s %s",
	       "", "Access", "Queue", "Packets", "Bytes",
	       "Packets", "Bytes", "Shaping"); 
	printf("\n%9s %6s %9s %9s %9s %9s %9s %s",
	       "I/F", "List", "Depth", "", "", "Delayed", "Delayed", "Active"); 
	show_traffic_shape_idb_stats(idb);
	automore_disable();
    }

}

/*
 * show_interface_traffic_shape
 *
 * display the parameters
 */
static void
show_traffic_shape_idb (idbtype *idb)
{
    traffic_shape_t *shape = idb->traffic_shape;
    for (; shape; shape = shape->shape_param_next) {
	if (automore_quit())
	    break;
	if (shape->shape_acl)
	    printf("\n%9s %6u %9u %6u %9u %9u %9u %9u %s",
		   idb->short_namestring,
		   shape->shape_acl,
		   shape->shape_cir,
		   shape->shape_byte_limit,
		   shape->shape_sustained_bits,
		   shape->shape_excess_bits,
		   shape->shape_interval,
		   shape->shape_increment,
		   shape->shape_to_becn?"BECN":"-");
	else
	    printf("\n%9s %6s %9u %6u %9u %9u %9u %9u %s",
		   idb->short_namestring,
		   "",
		   shape->shape_cir,
		   shape->shape_byte_limit,
		   shape->shape_sustained_bits,
		   shape->shape_excess_bits,
		   shape->shape_interval,
		   shape->shape_increment,
		   shape->shape_to_becn?"BECN":"-");
    }
}

/*
 * shape_adaptive_configure
 *
 * Calculate working variables for traffic shape adaptive.
 */
void
shape_adaptive_configure (traffic_shape_t *shape) {
    
    shape->shape_increment_lower = /* convert to bytes per interval */
	(shape->shape_cir_lower * shape->shape_interval)/8000;
    shape->shape_increment = shape->shape_increment_limit;
}


/*
 * configure Frame Relay adaptation to BECN
 */
void
shape_command_adaptive (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    traffic_shape_t *shape = idb->traffic_shape;

    if (csb->nvgen) {

	/*
	 * store configuration
	 */
	if (shape && !shape->shape_dynamic)
	    nv_write(shape->shape_to_becn, "%s %d",
		     csb->nv_command, shape->shape_cir_lower);
    } else {

	/*
	 * install configuration
	 */
	if (!shape) {
	    if (csb->sense)
		printf("\ntraffic shaping not enabled");
	    return;
	}

	if (csb->sense && GETOBJ(int,1) >= shape->shape_cir) {
	    printf("\nlower bound (%d KBPS) >= upper bound (%d KBPS)",
		   GETOBJ(int,1), shape->shape_cir);
	    return;
	}

	shape->shape_to_becn = csb->sense;
	shape->shape_cir_lower = GETOBJ(int,1);
	shape_adaptive_configure(shape);
    }
}

/*
 * configure Frame Relay FECN setting on shaped traffic
 */
void
shape_command_fecn_create (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    traffic_shape_t *shape = idb->traffic_shape;

    if (csb->nvgen) {

	/*
	 * store configuration
	 */
	if (shape && !shape->shape_dynamic)
	    nv_write(shape->shape_fecn_create, "%s", csb->nv_command);
    } else {

	/*
	 * install configuration
	 */
	if (!shape) {
	    if (csb->sense)
		printf("\ntraffic shaping not enabled");
	    return;
	}

	shape->shape_fecn_create = csb->sense;
    }
}

/*
 * configure Frame Relay FECN->BECN return
 */
void
shape_command_fecn_adapt (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    traffic_shape_t *shape = idb->traffic_shape;

    if (csb->nvgen) {

	/*
	 * store configuration
	 */
	if (shape && !shape->shape_dynamic)
	    nv_write(shape->shape_fecn_adapt, "%s", csb->nv_command);
    } else {

	/*
	 * install configuration
	 */
	if (!shape) {
	    if (csb->sense)
		printf("\ntraffic shaping not enabled");
	    return;
	}

	shape->shape_fecn_adapt = csb->sense;
    }
}

/*
 * display traffic shaping characteristics
 */
void
show_traffic_shape_command (parseinfo *csb) 
{
    idbtype *idb;
    idb = GETOBJ(idb, 1);

    if (!idb) {
	automore_enable(NULL);
	printf("\n%9s %6s %9s %6s %9s %9s %9s %s %s",
	       "", "access", "Target", "Byte", "Sustain", "Excess",
	       "Interval", "Increment", "Adapt"); 
	printf("\n%9s %6s %9s %6s %9s %9s %9s %s %s",
	       "I/F", "list", "Rate", "Limit", "bits/int", "bits/int",
	       "(ms)", " (bytes) ", "Active"); 
	FOR_ALL_SWIDBS(idb) {
	    if (automore_quit())
		break;
	    show_traffic_shape_idb(idb);
	} 
	automore_disable();
    } else if (!idb->traffic_shape) {

	printf("\nTraffic shaping not configured on %s", idb->namestring);

    } else {
	automore_enable(NULL);
	printf("\n%9s %6s %9s %6s %9s %9s %9s %s %s",
	       "", "access", "Target", "Byte", "Sustain", "Excess",
	       "Interval", "Increment", "Adapt"); 
	printf("\n%9s %6s %9s %6s %9s %9s %9s %s %s",
	       "I/F", "list", "Rate", "Limit", "bits/int", "bits/int",
	       "(ms)", " (bytes) ", "Active"); 
	show_traffic_shape_idb(idb);
	automore_disable();
    }

}

/*
 * show_shaping_queue_command
 *
 * Display queue contents. This function displays the type of packets that
 * are queued on an inteface output queue. In some cases, the network layer
 * contents are displayed.
 *
 * This function should be modified by anyone that wants to see other
 * network layer headers.
 */
static boolean
show_shaping_queue_interface_command (parseinfo *csb, idbtype *idb)
{

    ushort status;
    paktype     *pak;
    ushort      *ptr;
    shape_fair_queue *fair;
    traffic_shape_t *shape;
    shaping_queue  *queue;
    int 	conversation;
    queuetype   messages;

    shape = idb->traffic_shape;
    if (!shape) {
	return FALSE;
    }
    fair =  shape->shape_fair;
    if (!fair) {
	return FALSE;
    }

    queue_init(&messages, 0);

    /*
     * determine what messages to display
     */
    status = raise_interrupt_level(ALL_DISABLE);
    queue = fair->shape_first;
    while (queue) {
	pak = pak_queue_get_head(&queue->q);
	if (pak) {
	    pak_lock(pak);
	    pak->fair_conversation = pak_queue_depth(&queue->q);
	    pak_enqueue(&messages, pak);
	}
	queue = queue->shape_next;
    }
    reset_interrupt_level(status);

    if (!queryqueuedepth(&messages)) {
	printf("\nNo traffic queued in shaping queue on %s", idb->namestring);
    } else {
	printf("\nTraffic queued in shaping queue on %s", idb->namestring);
	/*
	 * display the messages
	 */
	while ((pak = pak_dequeue(&messages))) {
	    if (automore_quit()) {
		do datagram_done(pak);
		while ((pak = pak_dequeue(&messages)));
		return TRUE;
	    }
	    conversation = pak->oqnumber;
	    queue = shaping_queue_address(shape, conversation);
	    printf("\n  (depth/weight) %d/%d\n",
		   pak->fair_conversation, pak->fair_weight);

	    printf("  Conversation %d, linktype: %s, length: %d",
		   conversation, print_linktype(pak->linktype),
		   pak->datagramsize);

	    /*
	     * Call registry to display network layer header. Protocol
	     * specific function returns pointer to data. Then pitch message.
	     */
	    ptr = NULL;
	    reg_invoke_print_network_header(pak->linktype, pak, &ptr);
	    datagram_done(pak);
	    printf("\n");
	}
    }
    return FALSE;
}
static void
show_shaping_queue_command (parseinfo *csb)
{

    idbtype     *idb;

    automore_enable(NULL);
    idb = GETOBJ(idb,1);
    if (idb)
	show_shaping_queue_interface_command(csb, idb);
    else {
	FOR_ALL_SWIDBS(idb) {
	    if (show_shaping_queue_interface_command(csb, idb))
		break;
	}
    }
    automore_disable();
}

/**********************************************************************
 *      traffic-shape <target-bit-rate>
 *		      [[bits-per-interval>]
 *		       [excess-bits-first-interval>]]
 *
 *      OBJ(int,1) = target bit rate (kilobits per second)
 *      OBJ(int,2) = sustained bits per interval
 *      OBJ(int,3) = excess bits in first interval
 */
static void shape_delete_queue(traffic_shape_t *shape)
{
    shape_fair_queue *fair;
    paktype *pak;
    int status;

    if (!shape)
	return;
    fair = shape->shape_fair;
    if (!fair)
	return;
    
    status = raise_interrupt_level(NETS_DISABLE);
    mgd_timer_stop(&shape->shape_restart);
    while ((pak = traffic_shape_dequeue(shape))) {
	datagram_done(pak);
    }
    reset_interrupt_level(status);
    if (--fair->shape_usage_count)
	return;
    if (fair->shape_hashed_queue)
	free(fair->shape_hashed_queue);
    if (fair->shape_reserved_queue)
	free(fair->shape_reserved_queue);
}

void shape_adjust_queue (hwidbtype *hwidb)
{
    int i;
    fair_queue_head *fq;
    traffic_shape_t *shape;
    shaping_queue *link_queue;
    shaping_queue *hashed_queue;
    shaping_queue *reserved_queue;
    shape_fair_queue *fair;
    int status;
    idbtype *idb;
    ushort congestion_threshold;
    ushort number_hashed_queue;
    ushort number_reserved_queue;

    if (!hwidb)
	return;

    fq = hwidb->fair_queue_head;
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	shape = idb->traffic_shape;
	if (!shape)
	    continue;
	fair = shape->shape_fair;
	if (!fair)
	    continue;

	hashed_queue = NULL;
	reserved_queue = NULL;
	congestion_threshold = FAIR_QUEUE_DEFAULT_QDEPTH;
	number_hashed_queue = FAIR_QUEUE_HASH_QUEUES_DEFAULT;
	number_reserved_queue = FAIR_RSVP_QUEUES_DEFAULT;

	if (fq) {
	    if (fair->shape_hash_mask == fq->fq_hash_mask &&
		fair->shape_queue_count == fq->fq_queue_count) {
		continue;
	    } else {
		number_hashed_queue = fq->fq_hash_mask + 1;
		number_reserved_queue = fq->fq_queue_count;
		shape_delete_queue(shape);
		congestion_threshold = fq->fq_max_enqueue;
	    }
	} else if ((fair->shape_queue_count ==
		    fair->shape_reserved_queue_start + number_reserved_queue) &&
		   (fair->shape_hash_mask == number_hashed_queue - 1) ) {
	    continue;
	}

	fair->shape_usage_count++;
	status = raise_interrupt_level(NETS_DISABLE);
	hashed_queue = malloc_named(number_hashed_queue*sizeof(shaping_queue),
				    "shaping queue");
	if (number_reserved_queue)
	    reserved_queue =
		malloc_named(number_reserved_queue*sizeof(shaping_queue),
			     "shaping queue");
	if (shape && hashed_queue &&
	    (!number_reserved_queue || reserved_queue)) {
	    fair->shape_hash_mask = number_hashed_queue - 1; /* power of 2 */
	    fair->shape_reserved_queue_start =
		number_hashed_queue + 1 + FAIR_LINK_TYPE_HASH;
	    fair->shape_queue_count =
		fair->shape_reserved_queue_start + number_reserved_queue;

	    fair->shape_max_enqueue = congestion_threshold;
	    fair->shape_last = fair->shape_link_queue;
	    idb->traffic_shape = shape;
	    fair->shape_hashed_queue = hashed_queue;
	    for (i = number_hashed_queue; --i >= 0; hashed_queue++) {
		queue_init(&hashed_queue->q, fair->shape_max_enqueue);
	    }
	    link_queue = fair->shape_link_queue;
	    for (i = 1 + FAIR_LINK_TYPE_HASH; --i >= 0; link_queue++) {
		queue_init(&link_queue->q, fair->shape_max_enqueue);
	    }
	    fair->shape_reserved_queue = reserved_queue;
	    for (i = number_reserved_queue; --i >= 0; reserved_queue++) {
		queue_init(&reserved_queue->q, fair->shape_max_enqueue);
	    }
	} else {

	    /*
	     * unable to allocate memory
	     */
	    if (hashed_queue)
		free(hashed_queue);
	    if (reserved_queue)
		free(reserved_queue);
	}
	reset_interrupt_level(status);
    }
}

void shape_reset(traffic_shape_t *shape)
{
    paktype *pak;
    shape->shape_bytes_enqueued = 0;
    while ((pak = shape->shape_dequeue(shape))) {
	pak->flags &= ~PAK_DLQ;
	datagram_done(pak);
    }
}

/*
 * v o i d  s h a p e _ c o m m a n d   ( p a r s e i n f o  * c s b )
 *
 * Initialize the traffic shaping structures.  Function is responsible
 * for distilling Commited Informaiton Rate (CIR), Bc (sustained burst
 * size, bits), and Be (excess burst size, in bits) into the following
 * working structure values:
 * 
 * Field			Description
 * shape_increment		limit of number of bytes in Token-bucket
 * 				(based upon Bc)
 * shape_interval		duration of count interval in ms
 * shape_byte_limit		max limit on count of bytes in
 * 				Token-bucket. (basec on Bc + Be)
 *
 * For any interval of time:
 *
 *	
 *	   :				       :
 *	   :<------- shape_byte_limit ------------>:
 *	   :	 (bytes)		       :
 *	   :				       :
 *	   :<-- shape_increment -->:	       :
 *	   :    (bytes)	    :	       :
 *	   :      Bc (bits)	:  Be (bits)    :
 *   ...===|=======================:===============|===...
 *	   :		       :
 *	   :<-- shape_interval  -->:
 *	   :    (milliseconds)     :
 *
 * Note that shape_increment and shape_interval _may_ be scaled to
 * more 'reasonable' numbers.  Specifically, excessively large byte
 * counts or small time intervals are avoided through scaling these
 * values.  The ratio of bytes to count is preserved to support
 * configured CIR.
 */
static int gcd(uint a, uint b)
{
    uint c;
    if (b < a) {
	c = b;
	b = a;
	a = c;
    }
    if (a == 0)
	return b;
    else
	return gcd(b%a, a);
}
void shape_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    hwidbtype *hwidb = hwidb_or_null(idb);
    traffic_shape_t **shapep = &idb->traffic_shape;
    traffic_shape_t *shape = idb->traffic_shape;
    int status;
    boolean new_shape_entry;		/* need to put on list? */
    boolean adjusted_routing;		/* did we play with route caching? */
    ulong shape_interval;		/* duration of interval */
    ulong shape_cir;			/* target traffic rate - upper bound */
    ulong shape_sustained_bits;		/* Bc - sustained bits */
    ulong shape_excess_bits;		/* Be - excess bits first interval */
    ulong shape_byte_limit;		/* max number of bytes for interval */
    ulong shape_increment;		/* number bytes added per interval */
    ulong shape_increment_limit;	/* upper bound of FECN/BECN range */
    ulong param_cir = GETOBJ(int,1);	/* Target Rate */
    ulong param_Bc = GETOBJ(int,2);	/* Burst Size - sustained */
    ulong param_Be = GETOBJ(int,3);	/* Burst Size - excess */
    ulong param_acl = GETOBJ(int,4);	/* acces list */

    if (csb->nvgen) {

	/*
	 * store configuration
	 */
	
	for (; shape; shape = shape->shape_param_next) {
	    if (shape->shape_dynamic)
		continue;
	    if (shape->shape_acl)
		nv_write(csb->which==TRAFFIC_SHAPE_GROUP,
			 "%s %d %d %d %d", csb->nv_command,
			 shape->shape_acl,
			 shape->shape_cir,
			 shape->shape_sustained_bits,
			 shape->shape_excess_bits);
	    else
		nv_write(csb->which==TRAFFIC_SHAPE_RATE,
			 "%s %d %d %d", csb->nv_command,
			 shape->shape_cir,
			 shape->shape_sustained_bits,
			 shape->shape_excess_bits);
	}
    } else if (!csb->sense) {

	/*
	 * remove active configuration
	 */
	for (; (shape = *shapep); shapep = &shape->shape_param_next) {
	    if (param_acl == shape->shape_acl) {
		hwidb = hwidb_or_null(shape->shape_idb);
		if (hwidb) {
		    hwidb->oqueue_dequeue = hwidb->shape_oqueue_dequeue;
		}
		*shapep = shape->shape_param_next; /* remove entry from list */
		status = raise_interrupt_level(NETS_DISABLE);
		shape_delete_queue(shape);
		if (shape->shape_fair) {
		    if (!shape->shape_fair->shape_usage_count)
			free(shape->shape_fair);
		}
		free(shape);
		reset_interrupt_level(status);
		traffic_shape_delete_entry();
		break;
	    }
	}
    } else {

	new_shape_entry = FALSE;
	if (shape &&
	    ((!param_acl && shape->shape_acl) ||
	     (param_acl && !shape->shape_acl))) {
	    printf("\nmix of shaping with and without access lists prohibited");
	    return;
	}

	for (; shape; shape = shape->shape_param_next)
	    if (param_acl == shape->shape_acl)
		break;

	if ((param_Be && param_Be < SHAPE_MIN_BITS_IN_INTERVAL) ||
	    (param_Bc && param_Bc < SHAPE_MIN_BITS_IN_INTERVAL)) {
	    printf("\nless than 1000 bits in an interval doesn't make sense");
	    return;
	}
	if (!param_cir || (!param_Bc && !param_Be)) {
	    printf("\nCIR must be > 0 and either Bc of Be must be > 0");
	    return;
	}

	/*
	 * install active configuration
	 */
	if (!shape) {
	    new_shape_entry = TRUE;
	    shape = malloc_named(sizeof(traffic_shape_t), "shaping queue");
	    if (!shape) {
		printf("\nconfiguration failed");
		return;
	    }
	    if (idb->traffic_shape) {
		shape->shape_fair = idb->traffic_shape->shape_fair;
	    } else {
		shape->shape_fair = malloc_named(sizeof(shape_fair_queue), 
						 "shaping queue");
		if (!shape->shape_fair) {
		    printf("\nconfiguration failed");
		    free(shape);
		    return;
		}
	    }
	    shape->shape_fair->shape_usage_count++;
	    shape->shape_reset = shape_reset;
	    shape->shape_oqueue = traffic_shape_enqueue;
	    shape->shape_dequeue = traffic_shape_dequeue;
 	    shape->shape_dlci_num = FR_NONDATA_DLCI;
 	    shape->shape_idb = idb;
 	    shape->shape_dynamic = FALSE;
	    traffic_shape_add_entry(idb, shape);
        }

	/*
	 * apply default of DEFAULT_SHAPE_INTERVAL milliseconds
	 */
	shape_cir = param_cir;	/* BPS */
	if (-1 == param_Bc) {
	    uint ms_per_8_seconds = 8000;
	    uint bytes_per_8_seconds = shape_cir;
	    uint gcd_8_seconds = gcd(bytes_per_8_seconds, ms_per_8_seconds);
	    uint bytes_per_interval= bytes_per_8_seconds/gcd_8_seconds;
	    shape_interval = ms_per_8_seconds/gcd_8_seconds;

	    while (shape_interval > TARGET_SHAPE_INTERVAL_MAX &&
		   bytes_per_interval > SHAPE_TARGET_BYTES) {
		shape_interval >>= 1;
		bytes_per_interval >>= 1;
	    }
	    if (shape_interval < TARGET_SHAPE_INTERVAL_MIN &&
		bytes_per_interval < SHAPE_TARGET_BYTES) {
		uint multiplier1 = SHAPE_TARGET_BYTES/bytes_per_interval;
		uint multiplier2 = TARGET_SHAPE_INTERVAL_MIN/shape_interval;
		if (multiplier1 < multiplier2)
		    multiplier1 = multiplier2;
		shape_interval *= multiplier1;
		bytes_per_interval *= multiplier1;
	    }
	    shape_sustained_bits = 8*bytes_per_interval;
	} else
	    shape_sustained_bits = param_Bc; /* bits */

	if (-1 == param_Be)
	    shape_excess_bits = shape_sustained_bits;
	else
	    shape_excess_bits = param_Be; /* bits */

	/*
	 * determine length of interval
	 *
	 * note that command line is in bits per second
	 * and we ultimately want the interval in milliseconds and
	 * the limits in bytes.
	 */
	shape_byte_limit = (shape_sustained_bits + shape_excess_bits)/8;
	if (shape_sustained_bits) {
	    shape_increment = shape_sustained_bits/8;
	    shape_interval = shape_sustained_bits*1000/shape_cir;
	} else {
	    shape_increment = shape_excess_bits/8;
	    shape_interval = shape_excess_bits*1000/shape_cir;
	}

	/*
	 * reject nonsense shaping intervals
	 */
	if (shape_interval < SHAPE_MIN_INTERVAL) {
	    printf("\nshaping interval is %d milliseconds. "
		   "intervals below %d milliseconds rejected",
		   shape_interval, SHAPE_MIN_INTERVAL);
	    return;
	}
	 
	shape_increment_limit = shape_increment;

	shape->shape_interval = shape_interval;
	shape->shape_cir = shape_cir;
	shape->shape_sustained_bits = shape_sustained_bits;
	shape->shape_excess_bits = shape_excess_bits;
	shape->shape_byte_limit = shape_byte_limit;
	shape->shape_increment = shape_increment;
	shape->shape_increment_limit = shape_increment_limit;
	shape->shape_byte_credit = 0;
	shape->shape_byte_credit_int = 0;
	traffic_shape_apply_credit(shape); /* initialize NULL credit */
	shape_adaptive_configure(shape); /* adjust adaptive parameters */
	shape->shape_acl = param_acl;
	shape->shape_linktype = diallst_getlink(param_acl);

	if (new_shape_entry) {
	    shape->shape_param_next = idb->traffic_shape;
	    idb->traffic_shape = shape;
	    shape_adjust_queue(hwidb);
	}
	if (traffic_shape_dequeue_shim != hwidb->oqueue_dequeue) {
	    hwidb->shape_oqueue_dequeue = hwidb->oqueue_dequeue;
	    hwidb->oqueue_dequeue = traffic_shape_dequeue_shim;
	}
	reg_invoke_shape_cache(hwidb);
	adjusted_routing = FALSE;
	FOR_ALL_HWIDBS(hwidb) {
	    if (hwidb->ip_routecache & TRAFFIC_SHAPE_NOT_SUPPORTED) {

        	/*
        	 * Because DFS shares this config setting let the VIP
        	 * know of this change if distributed is on.
        	 */
        	if (hwidb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE)
	            reg_invoke_ip_enable_disable_dfs_ipc(hwidb);

		hwidb->ip_routecache &= ~TRAFFIC_SHAPE_NOT_SUPPORTED;
		adjusted_routing = TRUE;
	    }
	}
	if (adjusted_routing)
	    printf("\nIP Route Cache style changed on some interfaces");
    }
}

/*
 * shape_clear_counters
 *
 * Clear interface counters having to do with traffic shaping.
 * (Called via registry from clear_idb.)
 */
static void shape_clear_counters (hwidbtype *hwidb) 
{
    idbtype *idb;
    traffic_shape_t *shape;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	for (shape = idb->traffic_shape; shape;
	     shape = shape->shape_param_next) {
	    shape->si = shape->stat;	/* capture values */
	}
    }
}

/*
 * On a Frame Relay Interface, when a message is received with FECN
 * in it, the following is executed:
 */
 
boolean traffic_shape_fecn_received (idbtype *idb, ushort in_dlci)
{
    paktype *pak;
    traffic_shape_t *shape;
    ushort *out;
    ushort dlci_num = fr_dlci_to_vc_num(in_dlci);
 
    if (!idb)
	return FALSE;
    for (shape = idb->traffic_shape; shape; shape = shape->shape_param_next) {
	if (shape->shape_dynamic && (dlci_num != shape->shape_dlci_num))
	    continue;
	if (shape->shape_fecn_adapt) {
	    pak = getbuffer(3);
	    if (pak) {
		pak->if_output = idb;
		out = (ushort *) pak->datagramstart;
		pak->mac_start = pak->datagramstart;
		in_dlci &= ~FR_DE_BIT & ~FR_FECN_BIT; /* clear DE+FECN */
		in_dlci |= FR_BECN_BIT | FR_CR_BIT;   /* set BECN+RESPONSE */
		*out++ = in_dlci;		     /* store header */
		*out++ = 256*Q922_TEST;	       /* store Message Type */
		pak->datagramsize = 3;
		pak->linktype = LINK_FR_SWITCH;
		datagram_out(pak);
		return TRUE;
	    }
	}
    }
    return FALSE;
}

/*
 * On a Frame Relay Interface, when a message is received with BECN
 * in it, the following is executed:
 */
void traffic_shape_becn_received (idbtype *idb,
				  fr_pvc_info_type *pvc)
{
    traffic_shape_t *shape;

    for (shape = idb->traffic_shape; shape; shape = shape->shape_param_next)
	if (shape->shape_dynamic && (shape->shape_dlci_num != pvc->this_dlci))
	    continue;
	if (shape->shape_to_becn) {
	    shape->shape_congestion_history |= SHAPING_RECENT_BECN;

	    if (!mgd_timer_running(&shape->shape_restart)) {
	
		/*
		 * start a timer to pop at the next timeslice boundary
		 */
		mgd_timer_start(&shape->shape_restart, shape->shape_interval);
	    }
	}
}

/*
 * this routine is called when the managed timer expires. Note that
 * the effect of it is *also* acheived should a message arrive after
 * the timer has expired but before the process level code runs.
 */
static
void traffic_shape_restart (traffic_shape_t *shape)
{
    if (shape) {
	traffic_shape_timeslice(shape);
    
	if (!mgd_timer_running(&shape->shape_restart)) {
	    if (shape->shape_bytes_enqueued) {
	
		/*
		 * start a timer to pop at the next timeslice boundary
		 */
		mgd_timer_start(&shape->shape_restart, shape->shape_interval);
	    } else {
		TIMER_STOP(shape->shape_next_event); 
	    }
	}
    }
}

/*
 * process_timers
 *
 * Process any expired timers.  This fragment shows how to process all
 * expired timers in the processes private managed timers tree.  This is
 * also where a watched simple timers would be checked if the process was
 * using simple timers.
 */
static void traffic_shape_process_timers (void)
{
    mgd_timer *expired_timer;
    idbtype *idb;
    ushort status;

    while (mgd_timer_expired(&traffic_shape_root_timer)) {
	expired_timer = mgd_timer_first_expired(&traffic_shape_root_timer);
	switch (mgd_timer_type(expired_timer)) {
	case TIMER_START_SHAPING_INTERVAL:
	    status = raise_interrupt_level(NETS_DISABLE);
	    idb = mgd_timer_context(expired_timer);
	    mgd_timer_stop(expired_timer);
	    traffic_shape_restart(idb->traffic_shape);
	    reset_interrupt_level(status);
	    break;

	default:
	    errmsg(&msgsym(UNEXPECTEDTIMER,SCHED), expired_timer,
		   mgd_timer_type(expired_timer));
	    mgd_timer_stop(expired_timer);
	    break;
	}
    }
}

/*
 * traffic_shape_teardown
 * 
 * routine to close out and re-initialized process when
 * last interface has configuration removed
 */
static void traffic_shape_teardown (int signal, int dummy1, 
				    void *dummy2, char *dummy3)
{
    mgd_timer_stop(&traffic_shape_root_timer);
    traffic_shape_init_thyself(NULL);
}

/*
 * traffic_shape_process
 *
 *	This process lives soley to restart lines when they have
 *	gone idle. Ideally, it never gets invoked...
 */
void traffic_shape_process (void)
{
    ulong major, minor;

    /*
     * Setup process private structures
     */
    reg_add_clear_counters(shape_clear_counters, "shape_clear_counters");
    process_watch_mgd_timer(&traffic_shape_root_timer, ENABLE);
    signal_permanent(SIGEXIT, traffic_shape_teardown);

    while (TRUE) {
	/*
	 * Wait for the next event to occur.
	 */
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case TIMER_EVENT:
		traffic_shape_process_timers();
		break;

	    case QUEUE_EVENT:
	    case MESSAGE_EVENT:
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * Parse chains for traffic shaping show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_traffic-shape.h"
LINK_POINT(traffic_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for traffic shaping interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_traffic-shape.h"
LINK_POINT(traffic_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Priority
 */
static parser_extension_request traffic_shape_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(traffic_show_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(traffic_interface_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * priority_parser_init - Initialize Priority parser support
 */
static void traffic_parser_init (void)
{
    parser_add_command_list(traffic_shape_chain_init_table, "traffic shaping");
}

void traffic_shape_init_thyself (subsystype * subsystem)
{
    shape_running = 0;
    bzero(&traffic_shape_root_timer, sizeof(traffic_shape_root_timer));
    mgd_timer_init_parent(&traffic_shape_root_timer, NULL);
    reg_add_interface_cfg_modified(shape_adjust_queue, "shape_adjust_queue");
    reg_add_generic_shaping_fecn_received(traffic_shape_fecn_received,
					  "traffic_shape_fecn_received");
    reg_add_generic_shaping_becn_received(traffic_shape_becn_received,
					  "traffic_shape_becn_received");
    reg_add_interface_cfg_modified(traffic_shape_capture_if,
				   "traffic_shape_capture_if");
    traffic_parser_init();
}

#define SHAPE_MAJOR_VERSION 1
#define SHAPE_MINOR_VERSION 0
#define SHAPE_EDIT_VERSION 1
SUBSYS_HEADER(traffic_shape, SHAPE_MAJOR_VERSION, SHAPE_MINOR_VERSION,
	      SHAPE_EDIT_VERSION, traffic_shape_init_thyself,
	      SUBSYS_CLASS_DRIVER, "seq:", "req:");
