/* $Id: sntp.c,v 3.1.2.4 1996/08/28 13:00:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/ntp/sntp.c,v $
 *------------------------------------------------------------------
 * Simple NTP (SNTP) client
 *
 * June 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sntp.c,v $
 * Revision 3.1.2.4  1996/08/28  13:00:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.3  1996/06/25  22:55:28  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add rudimentary clock selection logic.  Get rid of most static storage.
 * Keep track of all broadcast servers.
 *
 * Revision 3.1.2.2  1996/06/23  18:26:06  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Code review comments.
 *
 * Revision 3.1.2.1  1996/06/23  04:40:24  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add an SNTP client.  This is not included in shipping IOS images.
 *
 * Revision 3.1  1996/06/22  21:14:49  dkatz
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 *
 * This file contains the SNTP process itself.  The process is launched
 * when the first SNTP command is entered, and is killed when the last
 * server is deleted (and broadcast client mode is disabled).
 */

#define CISCO

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "../os/signal.h"
#include "subsys.h"
#include "mgd_timers.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../os/clock.h"
#include "../os/clock_private.h"
#include "ntp_fp.h"
#include "ntp.h"
#include "sntp.h"
#include "sntp_debug.h"

/* Local storage. */

static sntp_proc *sntp_procptr;		/* Pointer to process block, or NULL */


/*
 * sntp_get_procptr
 *
 * Returns the process block pointer, or NULL.
 */
sntp_proc *sntp_get_procptr (void)
{
    return(sntp_procptr);
}


/*
 * sntp_set_procptr
 *
 * Sets the current process block pointer.
 */
static void sntp_set_procptr (sntp_proc *proc) {
    sntp_procptr = proc;
}


/*
 * sntp_timer
 *
 * Process managed timer expirations.
 */
static void sntp_timer (sntp_proc *proc)
{
    mgd_timer *timer;
    sntp_server *server;

    /* Grab all the expired timers. */

    while ((timer =
	    mgd_timer_first_expired(&proc->sntp_master_timer))) {
	server = mgd_timer_context(timer);

	/* If this is a configured server, send a poll. */

	if (server->server_configured) {
	    sntp_send(proc, server);
	    
	    /* If we're doing rapid polls, send the next one more quickly. */

	    if (server->server_rapid_poll_count) {
		server->server_rapid_poll_count--;
		mgd_timer_update(timer, SNTP_RAPID_POLL_RATE);
	    } else {
		mgd_timer_update(timer, SNTP_POLL_RATE);
	    }
	} else {			/* Unconfigured */

	    /* A dynamic server timed out.  Delete it. */

	    sntp_delete_server(proc, server);
	}
    }
}


/*
 * sntp_find_server
 *
 * Return a pointer to a server structure, given the IP address.
 *
 * Returns NULL if not found.
 */
sntp_server *sntp_find_server (sntp_proc *proc, ipaddrtype server_addr)
{
    sntp_server *server;
    for (server = proc->sntp_server_queue.qhead; server;
	 server = server->next) {
	if (server->server_address == server_addr) {
	    return(server);
	}
    }
    return(NULL);
}


/*
 * sntp_start_server_timers
 *
 * Start the poll timers for all servers that were configured before the
 * process started.  We do this because we use mgd_timer_update to bump
 * the timers up--if we started them at config time, they may expire many
 * times in a row when the process finally gets to run (if we're just booting
 * up).
 */
static void sntp_start_server_timers (sntp_proc *proc)
{
    sntp_server *server;

    for (server = proc->sntp_server_queue.qhead; server;
	 server = server->next) {
	if (!mgd_timer_running(&server->server_timer) &&
	    server->server_configured) {
	    mgd_timer_start_jittered(&server->server_timer,
				     SNTP_STARTUP_DELAY, 99);
	}
    }
}


/*
 * sntp_process_exit
 *
 * Clean up as the process is dying.
 */
static void sntp_process_exit (int signal, int dummy1,
			       void *dummy2, char *dummy3)
{
    sntp_proc *proc;

    proc = sntp_get_procptr();
    if (!proc)
	return;

    /* Close the socket. */
    
    sntp_shutdown_io(proc);
    
    /* Wipe out the servers. */
    
    sntp_delete_all_servers(proc);

    /* Do the final cleanup and free the process block. */
	
    process_watch_mgd_timer(&proc->sntp_master_timer, DISABLE);
    free(proc);
    sntp_set_procptr(NULL);
}

/*
 * sntp_process
 *
 * Process for SNTP protocol
 */
static process sntp_process (void)
{
    ulong major, minor;
    sntp_proc *proc;

    /* Wait for system startup. */

    process_wait_on_system_init();

    /* Blow our brains out if we shouldn't be here, just in case. */

    proc = sntp_get_procptr();
    if (!proc) {
	process_kill(THIS_PROCESS);
	return;
    }

    /* Set up our exit handler. */

    signal_permanent(SIGEXIT, sntp_process_exit);

    /* Warm up the I/O portion. */

    if (!sntp_init_io(proc)) {
	sntp_stop(proc);
	return;
    }

    /* Watch the timer. */

    process_watch_mgd_timer(&proc->sntp_master_timer, ENABLE);

    /* Start all the preconfigured server timers. */

    sntp_start_server_timers(proc);

    /* Show that we're actually running now. */

    proc->sntp_running = TRUE;

    /* Now look for events. */

    while (TRUE) {
	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		sntp_readpkt(proc);
		break;

	      case TIMER_EVENT:
		sntp_timer(proc);
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}


/*
 * sntp_start
 *
 * Start the SNTP process.
 *
 * Returns TRUE if we succeeded, FALSE if there was no memory.
 */
boolean sntp_start (sntp_proc **proc)
{
    pid_t sntp_pid;

    if (!*proc) {
	sntp_pid = process_create(sntp_process, "SNTP", NORMAL_STACK,
				  PRIO_NORMAL);
	if (sntp_pid == NO_PROCESS) {
	    return(FALSE);
	}

	*proc = malloc_named(sizeof(sntp_proc), "SNTP process block");
	sntp_set_procptr(*proc);
	if (!*proc) {
	    process_kill(sntp_pid);
	    return(FALSE);
	}
	(*proc)->sntp_pid = sntp_pid;
	mgd_timer_init_parent(&(*proc)->sntp_master_timer, NULL);
	queue_init(&(*proc)->sntp_server_queue, 0);
    }
    return(TRUE);
}


/*
 * sntp_stop
 *
 * Stop the SNTP process.
 */
void sntp_stop (sntp_proc *proc)
{
    if (proc) {

	/* Kill the process.  The cleanup handler will finish wiping up. */

	process_kill(proc->sntp_pid);
    }
}


/*
 * sntp_init - Initialize SNTP
 * 
 * This routine is called at system startup to register the parse chains
 * for SNTP.
 */

static void sntp_init (subsystype *subsys)
{
    sntp_chain_init();
    sntp_debug_init();
}


/*
 * SNTP subsystem header
 */

#define SNTP_MAJVERSION 1
#define SNTP_MINVERSION 0
#define SNTP_EDITVERSION  1

SUBSYS_HEADER(sntp, SNTP_MAJVERSION, SNTP_MINVERSION, SNTP_EDITVERSION,
	      sntp_init, SUBSYS_CLASS_PROTOCOL, NULL, NULL);
