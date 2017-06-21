/* $Id: smrp_maint.c,v 3.2.62.1 1996/08/28 13:09:24 thille Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_maint.c,v $
 *------------------------------------------------------------------
 * Maintenance routines for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_maint.c,v $
 * Revision 3.2.62.1  1996/08/28  13:09:24  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:53:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/01  00:22:49  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.3  1995/06/20  07:18:01  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/14  22:55:04  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../os/signal.h"
#include "interface.h"
#include "packet.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "address.h"
#include "config.h"
#include "smrp_private.h"
#include "smrp_registry.h"
#include "mgd_timers.h"

extern pid_t smrp_maint_pid;

/*
 *------------------------------------------------------------------
 * Module private data and functions
 *------------------------------------------------------------------
 */

/* Private timer types */
enum {
    SMRPMAINT_HELLO,
    SMRPMAINT_NODE
};

static struct smrpmaint_globals_ {
    mgd_timer MasterTimer;	/* Master timer for maint process */
    mgd_timer HelloTimer;	/* Timer for sending hellos */
    mgd_timer NodeAgerTimer;	/* Timer for aging nodes */
} *smrpmaint_globals;

#define smrp_maintMasterTimer (&smrpmaint_globals->MasterTimer)
#define smrp_portHelloTimer   (&smrpmaint_globals->HelloTimer)
#define smrp_nodeAgerTimer    (&smrpmaint_globals->NodeAgerTimer)

/*
 *------------------------------------------------------------------
 * smrp_maint_Initialize		(Module Initialization)
 *
 * Module initialization.  Should be called once at startup.
 *------------------------------------------------------------------
 */
void
smrp_maint_Initialize ()
{
    smrpmaint_globals = malloc(sizeof(struct smrpmaint_globals_));

    if (!smrpmaint_globals)
	return;
    
    /* Initialize managed timers */
    mgd_timer_init_parent(smrp_maintMasterTimer, NULL);
    mgd_timer_init_leaf(smrp_portHelloTimer, smrp_maintMasterTimer,
			SMRPMAINT_HELLO, NULL, FALSE);
    mgd_timer_init_leaf(smrp_nodeAgerTimer, smrp_maintMasterTimer,
			SMRPMAINT_NODE, NULL, FALSE);
}

static void
smrp_maint_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_mgd_timer(smrp_maintMasterTimer, DISABLE);
    process_watch_queue(smrpNodeQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(smrpNodeQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&smrpNodeQ);
    smrp_maint_pid = 0;
}

/*
 *------------------------------------------------------------------
 * smrp_maint				(SMRP Maintenance Process)
 *
 * Process Hello packets that have been queued by Input.  Handle
 * port startup and node maintenance functions.
 *
 *------------------------------------------------------------------
 */
process
smrp_maint (void)
{
    ulong major, minor;
    paktype *pak;
    SMRPPort *port;
    mgd_timer *MaintTimer;
    
    /*
     * Startup
     */
    signal_permanent(SIGEXIT, smrp_maint_teardown);
    smrpNodeQ = create_watched_queue("SMRP Notify packets", 0, 0);
    process_watch_queue(smrpNodeQ, ENABLE, RECURRING);
    MaintTimer = smrp_maintMasterTimer;
    process_watch_mgd_timer(MaintTimer, ENABLE);
    
    mgd_timer_start(smrp_nodeAgerTimer, smrp_nodeAgeInterval);
    mgd_timer_start(smrp_portHelloTimer, smrp_helloInterval);
    
    while (TRUE) {
	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {
	    switch(major) {
	    case TIMER_EVENT:
		while (mgd_timer_expired(MaintTimer)) {
		    mgd_timer *expired_timer;

		    expired_timer = mgd_timer_first_expired(MaintTimer);
		    switch (mgd_timer_type(expired_timer)) {
		    case SMRPMAINT_HELLO:
			smrp_HandleAllPorts();
			mgd_timer_update(expired_timer, smrp_helloInterval);
			break;
		    case SMRPMAINT_NODE:
			smrp_AgeAllNodes();
			mgd_timer_update(expired_timer, smrp_nodeAgeInterval);
			break;
		    default:
			mgd_timer_stop(expired_timer);
			break;
		    }
		}
		break;
	    case QUEUE_EVENT:
		while ((pak = process_dequeue(smrpNodeQ)) != NULL) {

		    smrp_stat[SMRP_HELLO_IN]++; /* count a packet seen */

		    port = smrp_FindPortFromPak(pak);
		    if (port == NULL) {
			smrp_Dispose(SMRP_BAD_PORTID, pak);
			continue;
		    }

		    if (!PORTCB_CheckPacket(port,pak)) {
			smrp_stat[SMRP_INPUT_ERR]++;
			continue;
		    }

		    smrp_ProcessHello(PORTCB_GetPacketSource(port,pak), port,
				      smrp_datastart(pak));
		    datagram_done(pak);
		}
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }
}
