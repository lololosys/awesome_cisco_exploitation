/* $Id: tbridge_monitor.c,v 3.1.18.2 1996/08/28 13:16:46 thille Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_monitor.c,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_monitor.c
 *
 * January 1996, Tony Speakman
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This is a general purpose monitoring process for background
 * processing associated exclusively with transparent bridging
 * exclusive of spanning tree processing which should be kept
 * entirely separate.
 *
 * It simply monitors and processes the usual events (queues,
 * timers, messages,and signals).
 *
 *------------------------------------------------------------------
 * $Log: tbridge_monitor.c,v $
 * Revision 3.1.18.2  1996/08/28  13:16:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.18.1  1996/05/04  05:09:10  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - New file(tbridge_bvi.c) for Bridge-group Virtual Interface
 * functions.
 * - Obsolete tbridge_crb_inline.h with tbridge_irb_inline.h.
 * - Bridge/route decision(tbridge_irb_inline.h).
 * - Learn routable packets(DLLI_IRBPKT) in bridge table.
 * - Move locate_bte_inline() and tbridge_get_arpa_type_inline() to
 *   tbridge_common_inline.h.
 * - Rip out MTR transparent bridging support.
 *
 * Constrained Multicast Flooding
 * - Add the tbridge monitor.
 * - Add the tbridge registry.
 * - Add the entire CMF subsystem.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Add the call outs to CMF from the LES, RSP, and PAS forwarding
 *   functions.
 *
 * Revision 3.1  1996/02/20  22:43:50  speakman
 * Placeholders for constrained IP multicast flooding.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "mgd_timers.h"
#include "../os/signal.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */

#include "address.h"
#include "interface_private.h"
#include "packet.h"

#include "../srt/span.h"

#include "../tbridge/tbridge_public.h"
#include "../tbridge/tbridge_monitor.h"

#include "sys_registry.h"
#include "../tbridge/tbridge_registry.regh"

/*
 * Global definitions:
 */
watched_queue *tbm_igmpQ;
watched_queue *tbm_ospfQ;
watched_queue *tbm_cgmpQ;

mgd_timer tbm_igmp_qry_tmr;
mgd_timer tbm_igmp_rpt_tmr;
mgd_timer tbm_mrouter_tmr;

/*
 * Local definitions:
 */

static mgd_timer tbm_timer;	/* tbridge_monitor's base timer */
static pid_t tbm_pid;		/* tbridge_monitor's own PID */


/******************************************************************************
 * Functions to MONITOR tbridge monitor events:
 *****************************************************************************/

/*
 * Dispatch "interesting" packets to the appropriate handler.
 */
static void
tbridge_monitor_queues (ulong minor)
{
    int		count;
    paktype	*pak;

    switch (minor) {

    case TBM_IGMP_QUEUE:
	count = PAK_SWITCHCOUNT;
	while (count-- > 0) {
	    pak = process_dequeue(tbm_igmpQ);
	    if (pak != NULL && pak->if_input != NULL &&
		pak->if_input->span_ptr != NULL &&
		pak->if_input->span_ptr->running)
		reg_invoke_tbridge_monitor_queue(TBM_IGMP_QUEUE, pak);
	    else
		break;
	}
	break;

    case TBM_OSPF_QUEUE:
	count = PAK_SWITCHCOUNT;
	while (count-- > 0) {
	    pak = process_dequeue(tbm_ospfQ);
	    if (pak != NULL && pak->if_input != NULL &&
		pak->if_input->span_ptr != NULL &&
		pak->if_input->span_ptr->running)
		reg_invoke_tbridge_monitor_queue(TBM_OSPF_QUEUE, pak);
	    else
		break;
	}
	break;

    case TBM_CGMP_QUEUE:
	count = PAK_SWITCHCOUNT;
	while (count-- > 0) {
	    pak = process_dequeue(tbm_cgmpQ);
	    if (pak != NULL && pak->if_input != NULL &&
		pak->if_input->span_ptr != NULL &&
		pak->if_input->span_ptr->running)
		reg_invoke_tbridge_monitor_queue(TBM_CGMP_QUEUE, pak);
	    else
		break;
	}
	break;

    default:
	errmsg(&msgsym(UNEXPECTEDQUEUE,SCHED), minor);
	break;
    }
}

/*
 * Dispatch expired timers to the appropriate handler.
 */
static void
tbridge_monitor_timers (void)
{
    mgd_timer *expired_timer;

    while (mgd_timer_expired(&tbm_timer)) {

	expired_timer = mgd_timer_first_expired(&tbm_timer);

	switch (mgd_timer_type(expired_timer)) {

	case TBM_IGMP_QRY_TIMER:
	    reg_invoke_tbridge_monitor_timer(TBM_IGMP_QRY_TIMER,
					     mgd_timer_context(expired_timer));
	    break;

	case TBM_IGMP_RPT_TIMER:
	    reg_invoke_tbridge_monitor_timer(TBM_IGMP_RPT_TIMER,
					     mgd_timer_context(expired_timer));
	    break;

	case TBM_MROUTER_TIMER:
	    reg_invoke_tbridge_monitor_timer(TBM_MROUTER_TIMER,
					     mgd_timer_context(expired_timer));
	    break;

	default:
	    errmsg(&msgsym(UNEXPECTEDTIMER,SCHED), expired_timer,
		   mgd_timer_type(expired_timer));
	    mgd_timer_stop(expired_timer);
	    break;
	}
    }
}

/******************************************************************************
 * tbridge_monitor process functions
 *****************************************************************************/

/*
 * The tbridge monitor's "self-documenting" SIGEXIT handler.
 */
static void
tbridge_monitor_process_teardown (int signal, int dummy1, void *dummy2,
				 char *dummy3)
{
    paktype *pak;

    process_watch_mgd_timer(&tbm_timer, DISABLE);
    mgd_timer_stop(&tbm_timer);

    process_watch_queue(tbm_igmpQ, DISABLE, RECURRING);
    process_watch_queue(tbm_ospfQ, DISABLE, RECURRING);
    process_watch_queue(tbm_cgmpQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(tbm_igmpQ)) != NULL)
	retbuffer(pak);
    while ((pak = process_dequeue(tbm_ospfQ)) != NULL)
	retbuffer(pak);
    while ((pak = process_dequeue(tbm_cgmpQ)) != NULL)
	retbuffer(pak);

    delete_watched_queue(&tbm_igmpQ);
    delete_watched_queue(&tbm_ospfQ);
    delete_watched_queue(&tbm_cgmpQ);
}

/*
 * Boiler-plate asynchronous event process.
 */
static process
tbridge_monitor_process (void)
{
    ulong major, minor;

    process_wait_on_system_init();

    /*
     * Setup process private structures
     */
    tbm_igmpQ = create_watched_queue("TBM IGMP pkts", 0, TBM_IGMP_QUEUE);
    tbm_ospfQ = create_watched_queue("TBM OSPF pkts", 0, TBM_OSPF_QUEUE);
    tbm_cgmpQ = create_watched_queue("TBM CGMP pkts", 0, TBM_CGMP_QUEUE);
    process_watch_queue(tbm_igmpQ, ENABLE, RECURRING);
    process_watch_queue(tbm_ospfQ, ENABLE, RECURRING);
    process_watch_queue(tbm_cgmpQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&tbm_timer, ENABLE);
    signal_permanent(SIGEXIT, tbridge_monitor_process_teardown);

    while (TRUE) {
	/*
	 * Wait for Godot.
	 */
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		tbridge_monitor_queues(minor);
		break;

	      case TIMER_EVENT:
		tbridge_monitor_timers();
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * An idempotent function to create the tbridge monitor and attendant
 * data structures.  Called from the bridge command.
 */
void
tbridge_monitor_fiat_lux (void)
{
    if (!(mgd_timer_initialized(&tbm_timer))) {
	mgd_timer_init_parent(&tbm_timer, NULL);
	mgd_timer_init_parent(&tbm_igmp_qry_tmr, &tbm_timer);
	mgd_timer_init_parent(&tbm_igmp_rpt_tmr, &tbm_timer);
	mgd_timer_init_parent(&tbm_mrouter_tmr, &tbm_timer);
    }

    if (0x0 == tbm_pid) {
	tbm_pid = process_create(tbridge_monitor_process,
				 "Tbridge Monitor",
				 NORMAL_STACK, PRIO_NORMAL);
    }
}

/*
 * An idempotent function to delete the tbridge monitor process.
 * Called from the bridge command.
 */
void
tbridge_monitor_lights_out (void)
{
    if (tbm_pid != 0x0) {
	process_kill(tbm_pid);
	tbm_pid = 0x0;
    }
}
