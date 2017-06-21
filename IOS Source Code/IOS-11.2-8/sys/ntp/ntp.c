/* $Id: ntp.c,v 3.6.22.4 1996/08/28 13:00:46 thille Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp.c,v $
 *------------------------------------------------------------------
 * ntp.c - startup code and main task for NTP
 *
 * October 1992, Dave Katz.
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ntp.c,v $
 * Revision 3.6.22.4  1996/08/28  13:00:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.22.3  1996/08/19  18:54:37  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.6.22.2  1996/05/07  02:53:51  dkatz
 * CSCdi55346:  SCHED-3-THRASHING in NTP
 * Branch: California_branch
 * Make sure we process any repetitive timer events before going back to
 * sleep.
 *
 * Revision 3.6.22.1  1996/04/16  19:02:58  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.6  1996/01/24  20:39:02  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 *
 * Revision 3.5  1996/01/22  06:55:38  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/01  09:19:51  dkatz
 * CSCdi43035:  NTP may not sync quickly at system startup
 *
 * Revision 3.3  1995/11/17  17:50:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:07:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:40:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/08  16:48:08  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.4  1995/08/03 07:42:37  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.3  1995/06/16 04:28:34  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.2  1995/06/09  13:11:50  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:44:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CISCO

#include "master.h"
#include "sys_registry.h"
#include "subsys.h"
#include "packet.h"
#include "logger.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "ntpd.h"
#include "ntp_refclock.h"
#include "../os/clock.h"
#include "../os/clock_private.h"
#include "ntp_debug.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../ip/ip_registry.h"
#include "ntp_registry.h"
#include "ntp_registry_private.h"
#include "../ntp/ntp_registry.regc"
#include "../ntp/ntp_registry_private.regc"

ipsocktype *ntp_sock;
watched_queue *ntp_refclock_queue;
sys_timestamp ntp_sleep_timer;
int initializing;
boolean ntp_running = FALSE;
boolean ntp_task_initialized = FALSE;
boolean ntp_update_calendar;
boolean ntp_global_source_if = FALSE;

/*
 * External declarations
 */

/*
 * NTP subsystem header
 */

#define NTP_MAJVERSION 1
#define NTP_MINVERSION 0
#define NTP_EDITVERSION  1

SUBSYS_HEADER(ntp, NTP_MAJVERSION, NTP_MINVERSION, NTP_EDITVERSION,
	      ntp_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);

/*
 * Open the socket for NTP.  Returns TRUE if successful, or FALSE if not.
 */

static void ntp_open_socket(void)
{
    ntp_sock = open_ipsocket(UDP_PROT, NULL, NULL, NTP_PORT, NTP_PORT,
			     SOCK_ANYSRC);
    if (ntp_sock)
	process_watch_queue(ntp_sock->inq, ENABLE, RECURRING);
}

/*
 * The NTP process itself.
 */

#define MAX_NTP_PACKET_PROCESS_COUNT 20	/* This many per suspend pass */

static process ntp_process (void)
{
    paktype *pak;
    udptype *udp;
    struct pkt *ntp_pkt;
    int packet_count;
    ulong major, minor;
    struct recvbuf *refclock_buf;

    refclock_buf = NULL;		/* stupid compiler */

    /* Wait for the system to come up first. */

    process_wait_on_system_init();

    /*
     * Open our socket.
     */
    ntp_open_socket();

    /*
     * Set up some debugging flags
     */
    ntp_debug_init();

    /*
     * Report that we're up to any trappers
     */
    report_event(EVNT_SYSRESTART, (struct peer *)0);

    /* Watch the reference clock input queue. */

    process_watch_queue(ntp_refclock_queue, ENABLE, RECURRING);

    /*
     * Loop forever, processing packets and timer expirys.
     */
    process_watch_timer(&ntp_sleep_timer, ENABLE);
    GET_TIMESTAMP(ntp_sleep_timer);	/* Don't spin when we start! */
    while (TRUE) {

	/* Go to sleep. */
	process_wait_for_event();

	/* If we've been unsuccessful opening the socket, try again. */
	if (!ntp_sock) {
	    ntp_open_socket();
	}

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		switch (minor) {
		  case 0:
		    /*
		     * Call the data procedure to handle each received
		     * packet.
		     */
		    if (!ntp_sock)
			continue;
		    packet_count = MAX_NTP_PACKET_PROCESS_COUNT;
		    while ((pak = read_ipsocket(ntp_sock, FALSE, 0, &udp,
						&ntp_pkt))) {
			ntp_input_handler(pak, udp, ntp_pkt);
			retbuffer(pak);
			if (!(packet_count--)) {
			    process_may_suspend();
			    packet_count = MAX_NTP_PACKET_PROCESS_COUNT;
			}
		    }
		    break;

		  case 1:

		    /* Do the same for the reference clocks. */
		    
		    packet_count = MAX_NTP_PACKET_PROCESS_COUNT;
		    while ((refclock_buf =
			    process_dequeue(ntp_refclock_queue))) {
			ntp_refclock_input_handler(refclock_buf);
			if (!(packet_count--)) {
			    process_may_suspend();
			    packet_count = MAX_NTP_PACKET_PROCESS_COUNT;
			}
		    }
		    break;
		}
		break;

	      case TIMER_EVENT:
		/*
		 * Run the timers if appropriate.
		 */
		while (AWAKE(ntp_sleep_timer)) {
		    ntp_timer();
		    TIMER_UPDATE(ntp_sleep_timer,
				 ONESEC * (1 << EVENT_TIMEOUT));
		    process_may_suspend();
		}
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * Initialize the NTP process. 
 */
void init_ntp_process(idbtype *idb)
{
    if (ntp_task_initialized) {
	if (idb != NULL) {
	    ntp_init_idb(idb, FALSE);
	}
	ntp_running = TRUE;
	return;
    }

    ntp_refclock_queue = create_watched_queue("NTP refclock", 0, 1);
    if (!ntp_refclock_queue)
	return;

    initializing = TRUE;
    ntp_task_initialized = TRUE;
    invoke_clock_set(ntp_set_drift_incr); /* Find out about the hardware */

    init_auth();
    init_systime();
    init_timer();
    if (!init_request())
	return;
    if (!init_control())
	return;
    init_leap();
    if (!ntp_init_peer())
	return;
    reg_invoke_ntp_init_refclock();
    init_proto();
    if (!ntp_init_io())
	return;
    init_loopfilter();
    
    initializing = FALSE;

    (void) process_create(ntp_process, "NTP", NORMAL_STACK, PRIO_NORMAL);
    ntp_running = TRUE;
}

/*
 * Initialize NTP as a whole, and link it into the system. 
 */
void ntp_init (subsystype *subsys)
{
    /* Create the registries. */

    ntp_regcode = registry_create(REG_UNKNOWN, SERVICE_NTP_MAX, "NTP");
    if (ntp_regcode != REG_ILLEGAL)
	create_registry_ntp();

    ntp_private_regcode = registry_create(REG_UNKNOWN,
					  SERVICE_NTP_PRIVATE_MAX,
					  "NTP private");
    if (ntp_private_regcode != REG_ILLEGAL)
	create_registry_ntp_private();

    /*
     * Register some functions.
     */
    reg_add_sw_idb_init(ntp_init_idb, "ntp_init_idb");
    reg_add_time_set(ntp_clock_update, "ntp_clock_update");
    reg_add_hw_clock_set(ntp_set_drift_incr, "ntp_set_drift_incr");
    reg_add_ip_address_change(ntp_address_change, "ntp_address_change");
    reg_add_swif_comingup(ntp_int_statechange, "ntp_int_statechange");
    reg_add_swif_goingdown(ntp_int_statechange, "ntp_int_statechange");

    /*
     * Register parse chains
     */
    ntp_chain_init();
}
