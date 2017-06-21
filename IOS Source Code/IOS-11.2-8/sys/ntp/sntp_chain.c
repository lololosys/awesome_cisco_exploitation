/* $Id: sntp_chain.c,v 3.1.2.2 1996/06/25 22:55:30 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/sntp_chain.c,v $
 *------------------------------------------------------------------
 * Parser chains for SNTP
 *
 * June 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sntp_chain.c,v $
 * Revision 3.1.2.2  1996/06/25  22:55:30  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add rudimentary clock selection logic.  Get rid of most static storage.
 * Keep track of all broadcast servers.
 *
 * Revision 3.1.2.1  1996/06/23  04:40:25  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add an SNTP client.  This is not included in shipping IOS images.
 *
 * Revision 3.1  1996/06/22  21:14:52  dkatz
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CISCO

#include "master.h"
#include "mgd_timers.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../os/clock.h"
#include "ntp_fp.h"
#include "ntp.h"
#include "sntp.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "sntp_debug.h"


/*
 * sntp_create_server
 *
 * Create a server entry.  Returns a pointer to the entry, or NULL if
 * we ran out of memory.
 */
sntp_server *sntp_create_server (sntp_proc *proc, ipaddrtype server_addr)
{
    sntp_server *server;
    server = malloc_named(sizeof(sntp_server), "SNTP server");
    if (server) {
	server->server_address = server_addr;
	mgd_timer_init_leaf(&server->server_timer,
			    &proc->sntp_master_timer,
			    0, server, FALSE);
	enqueue(&proc->sntp_server_queue, server);
	server->server_stratum = STRATUM_UNSPEC;
    }
    return(server);
}


/*
 * sntp_delete_server
 *
 * Delete a server.
 */
void sntp_delete_server (sntp_proc *proc, sntp_server *server)
{
    unqueue(&proc->sntp_server_queue, server);
    mgd_timer_stop(&server->server_timer);
    free(server);
    if (proc->sntp_current_server == server)
	proc->sntp_current_server = NULL;
}


/*
 * sntp_delete_all_servers
 *
 * Wipe out all servers.
 */
void sntp_delete_all_servers (sntp_proc *proc)
{
    sntp_server *server;

    while ((server = proc->sntp_server_queue.qhead)) {
	sntp_delete_server(proc, server);
    }
}


/*
 * sntp_broadcast_command
 *
 * Enable/disable SNTP broadcast client mode
 */
static void sntp_broadcast_command (parseinfo *csb)
{
    sntp_server *server, *next_server;
    sntp_proc *proc;

    proc = sntp_get_procptr();

    /* First, NVGENs. */

    if (csb->nvgen) {
	nv_write(proc && proc->sntp_broadcast_client,
		 csb->nv_command);
	return;
    }

    if (csb->sense) {			/* Configuring it? */
	if (!sntp_start(&proc)) {	/* Start it;  bail if it won't start */
	    return;
	}
    } else {				/* Deconfiguring? */

	if (!proc)		/* It's not running. */
	    return;

	/* Flush out all of the dynamic (broadcast) servers. */

	for (server = proc->sntp_server_queue.qhead; server;
	     server = next_server) {
	    next_server = server->next;	/* May be deleted! */
	    if (!server->server_configured) {
		sntp_delete_server(proc, server);
	    }
	}

	/* If there are no servers configured, kill the process. */

	if (!proc->sntp_server_queue.qhead)
	    sntp_stop(proc);		/* Just about as straightforward. */
    }
    if (proc)
	proc->sntp_broadcast_client = csb->sense;
}


/*
 * sntp_server_command
 *
 * Handle SNTP server command configurations
 */
static void sntp_server_command (parseinfo *csb)
{
    sntp_server *server;
    ipaddrtype server_addr;
    sntp_proc *proc;

    proc = sntp_get_procptr();

    /* The usual drill.  First check for NVGENs. */

    if (csb->nvgen) {
	if (!proc)		/* Not running. */
	    return;

	for (server = proc->sntp_server_queue.qhead; server;
	     server = server->next) {
	    if (server->server_configured) {
		nv_write(TRUE, "%s %i", csb->nv_command,
			 server->server_address);
		nv_add(server->server_version != SNTP_DEFAULT_VERSION,
		       " version %d", server->server_version);
	    }
	}
	return;
    }

    /* Try to find a matching entry before we decide what to do. */

    server = NULL;
    server_addr = (*GETOBJ(paddr,1)).ip_addr;
    if (proc) {
	for (server = proc->sntp_server_queue.qhead; server;
	     server = server->next) {
	    if (server->server_address == server_addr)
		break;
	}
    }

    /* Now switch based on the presence of "no". */

    if (csb->sense) {			/* Configuring */
	if (!sntp_start(&proc)) {	/* Can't start process? */
	    return;
	}

	if (!server) {			/* No current configuration */

	    /* Create a new server. */

	    server = sntp_create_server(proc, server_addr);
	    if (!server)
		return;
	    server->server_rapid_poll_count = SNTP_RAPID_POLL_COUNT;
	}

	/* Set (or update) the version number. */

	server->server_version = GETOBJ(int,1);
	server->server_configured = TRUE;

	/*
	 * Start the poll timer if the SNTP process is actually up and
	 * running.  If it isn't, the process itself will start the
	 * timer later.
	 */
	if (proc->sntp_running) {
	    mgd_timer_start_jittered(&server->server_timer,
				     SNTP_STARTUP_DELAY, 99);
	}

    } else {				/* Deconfiguring */

	/* Delete the server. */

	if (!proc)			/* Not running. */
	    return;

	if (server) {
	    sntp_delete_server(proc, server);
	} else {
	    printf("\n%%Server not found");
	}
	
	/* If that was the last one, kill the process. */
	
	if (!proc->sntp_server_queue.qhead &&
	    !proc->sntp_broadcast_client) {
	    sntp_stop(proc);
	}
    }
}


/*
 * show_sntp
 *
 * Display SNTP information
 */
static void show_sntp (parseinfo *csb)
{
    sntp_server *server;
    sntp_proc *proc;

    proc = sntp_get_procptr();

    if (!proc) {
	printf("\n%%SNTP is not running.");
	return;
    }

    automore_enable(NULL);

    /* Display each server. */

    if (proc->sntp_server_queue.qhead) {

	printf("\nSNTP server     Stratum   Version    Last Receive");
	/*        xxx.yyy.zzz.www    4         3        00:00:42 */
	
	for (server = proc->sntp_server_queue.qhead; server;
	     server = server->next) {
	    printf("\n%15i    %2d        %d        %10TE",
		   server->server_address, server->server_stratum,
		   server->server_version, server->server_reply_time);
	    if (server == proc->sntp_current_server)
		printf("  Synced");
	    if (!server->server_configured)
		printf("  Bcast");
	}
    }

    if (proc->sntp_broadcast_client) {
	printf("\n\nBroadcast client mode is enabled.");
    }
    automore_disable();
}


/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_sntp.h"
LINK_POINT(sntp_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for SHOW commands
 */
#define ALTERNATE NONE
#include "exec_show_sntp.h"
LINK_POINT(sntp_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for SNTP top-level configuration commands
 */
#define ALTERNATE NONE
#include "cfg_sntp.h"
LINK_POINT(sntp_config_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain registration array for SNTP
 */
static parser_extension_request sntp_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(sntp_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(sntp_show_commands) },
    { PARSE_ADD_CFG_LAST_CMD, &pname(sntp_config_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * sntp_chain_init - Initialize parse chains for SNTP
 * 
 * This routine is called at system startup to register the parse chains
 * for SNTP.
 */

void sntp_chain_init (void)
{
    parser_add_command_list(sntp_chain_init_table, "sntp");
}
