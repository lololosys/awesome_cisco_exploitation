/* $Id: ipx_snmp.c,v 3.7.8.3 1996/08/03 23:43:56 fox Exp $
 *------------------------------------------------------------------
 * ipx_snmp.c -- Simple Network Management Protocol server over IPX
 *
 * September 1994, Joel P. Bion
 * Shamelessly copied from ip_snmp.c, by Greg Satz.
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipx_snmp.c,v $
 * Revision 3.7.8.3  1996/08/03  23:43:56  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.7.8.2  1996/06/03  17:33:33  john
 * CSCdi57589:  IPX SNMP packets accumulate on input queues
 * Branch: California_branch
 *
 * Revision 3.7.8.1  1996/03/28  16:47:40  jjohnson
 * CSCdi50399:  SNMP can drive CPU utilization to over 90%
 * Make SNMP processes default to low priority, and add a hidden config
 * command to allow customers to change it
 * Branch: California_branch
 *
 * Revision 3.7  1996/02/28  09:34:23  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.6  1996/02/13  19:50:47  hampton
 * Remove unnecessary includes.  [CSCdi48844]
 *
 * Revision 3.5  1996/02/12  06:58:52  jjohnson
 * CSCdi47099:  not nearly enough processes call process_set_crashblock
 * There isn't much demand for network management during a meltdown
 *
 * Revision 3.4  1996/01/20  02:36:30  jjohnson
 * CSCdi47220:  need snmp packet debugging
 *
 * Revision 3.3  1995/11/17  19:21:56  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:53:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:58:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  13:25:43  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:27:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../os/signal.h"
#include "xns.h"
#include "novell.h"
#include "../snmp/snmp_api.h"
#include "logger.h"
#include "../parser/parser_defs_snmp.h"
#include "../snmp/snmp_registry.h"
#include "../snmp/snmp_debug.h"
#include "../snmp/sr_sysmib2.h"
#include "address.h"
#include "../if/network.h"

pid_t novell_snmp_pid;
watched_queue *novell_snmpQ;
static watched_boolean *ipx_snmp_priority_changed;
static process_priority_t ipx_snmp_priority;

static void
ipx_snmp_pri_change (process_priority_t priority)
{
    /*
     * a process can only change its own priority, so tell the
     * ipx snmp process to do it
     */
    ipx_snmp_priority = priority;
    if (ipx_snmp_priority_changed)
	process_set_boolean(ipx_snmp_priority_changed, TRUE);
}

/*
 * ipx_snmp_teardown
 *
 * This process is being killed.  Remove any data structures that this
 * process set up when it started.
 */
static void
ipx_snmp_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_queue(novell_snmpQ, DISABLE, RECURRING);
    process_watch_boolean(ipx_snmp_priority_changed, DISABLE, RECURRING);
    while ((pak = process_dequeue(novell_snmpQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&novell_snmpQ);
    delete_watched_boolean(&ipx_snmp_priority_changed);
    novell_snmp_pid = 0;
}

/*
 * ipx_snmp
 * Provide IPX SNMP services
 */
static process ipx_snmp (void)
{
    paktype *pakin;
    paktype *pakout;
    uchar *outbuf;
    int outbuflen, pak_count;
    uint start;
    ipxhdrtype *ipx, *ipxout;
    addrtype faddr;
    ulong major, minor;

    process_wait_on_system_init();

    process_set_crashblock(THIS_PROCESS, TRUE);

    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, ipx_snmp_teardown);
    novell_snmpQ = create_watched_queue("IPX SNMP packets", 0, 0);
    process_watch_queue(novell_snmpQ, ENABLE, RECURRING);
    ipx_snmp_priority_changed = create_watched_boolean("SNMP reprioritize", 0);
    process_watch_boolean(ipx_snmp_priority_changed, ENABLE, RECURRING);
    process_set_priority(ipx_snmp_priority);

    while (TRUE) {
	process_wait_for_event();
	process_get_wakeup(&major, &minor);
	if (major == BOOLEAN_EVENT) {
	    process_set_priority(ipx_snmp_priority);
	    process_set_boolean(ipx_snmp_priority_changed, FALSE);
	    continue;
	}
	pak_count = PAK_SWITCHCOUNT;
	while (TRUE) {
	    /*
	     * Process only a limited number of packets per pass
	     */
	    if (pak_count-- == 0) {
		break;
	    }
	    pakin = process_dequeue(novell_snmpQ);
	    if (pakin == NULL) {
		break;
	    }
	    if (!novell_running || novell_shutting_down) {
		retbuffer(pakin);
		continue;
	    }
	    if ((pakin->nif_input == NULL) ||
		(!interface_up(pakin->if_input))) {
		retbuffer(pakin);
		continue;
	    }
	    ipx = (ipxhdrtype *)ipxheadstart(pakin);
		
	    if (novell_debug || reg_invoke_snmp_debug(DEBUG_SNMP_PACKETS)) {
		buginf("\nIPXSNMP: received request from %x.%e via %s",
		       GET2WD(ipx->ssnet), ipx->shost,
		       pakin->if_input->namestring);
	    }

	    /* attach a response buffer to the socket */
	    pakout = getbuffer(snmp_size + XNSHEADERBYTES);

	    if (pakout == NULL) {
		/*
		 * we can't attach a buffer, so we can't process the message
		 * we should probably bump an error counter here, but chances
		 * are we're gonna die Real Soon Now(tm), so why bother?
		 */
	    } else {
		/* pass the packet and response buffer to the snmp engine */

		start = ENCAPBYTES + XNSHEADERBYTES;
		outbuf = pakout->data_area + start;
		outbuflen = snmp_size;
		faddr.type = ADDR_NOVELL;
		faddr.nov_addr.net = GET2WD(ipx->ssnet);
		ieee_copy(ipx->shost, faddr.nov_addr.host);
		reg_invoke_snmp_engine(&faddr,
				       ipxdatastart(pakin),
				       ipx->len - XNSHEADERBYTES,
				       outbuf, &outbuflen);
		
		if (outbuflen) {
		    /* if we have a response, send it */
		    pakout->linktype = LINK_NOVELL;
		    ipxout = (ipxhdrtype *)ipxheadstart(pakout);
		    ipxout->tc = 0;
		    ipxout->pt = NOVELL_TYPE_PEP;
		    ipxout->dsock = ipx->ssock;
		    ipxout->ssock = ipx->dsock;
		    PUT2WD(ipxout->ddnet, GET2WD(ipx->ssnet));
		    PUT2WD(ipxout->ssnet, pakin->nif_input->novellnet ?
			   pakin->nif_input->novellnet : ipx_internal_net);
		    ipxout->cksum = NOVELL_NO_CHECKSUM;
		    ieee_copy(pakin->nif_input->idb->novell_node,
			      ipxout->shost);
		    ieee_copy(ipx->shost, ipxout->dhost);
		    ipx->len = XNSHEADERBYTES + outbuflen;
		    if (reg_invoke_snmp_debug(DEBUG_SNMP_PACKETS)) {
			buginf("\nIPXSNMP: sent response to %x.%e",
			       GET2WD(ipxout->ddnet), ipx->dhost);
		    }
		    nov_send(pakout);
		} else {
		    /* otherwise deallocate the response buffer */
		    retbuffer(pakout);
		}
	    }
	    retbuffer(pakin);
            (void) process_may_suspend();
	}
    }
}

/*
 * ipx_snmp_process
 * Create or delete a process to handle SNMP for IPX
 * Invoked as SERVICE_SNMP_PROCESS
 *
 * NOTE: novell_snmp_pid is not set to NO_PROCESS, but instead
 *       zero when there is no process. This is the way the rest
 *       of the IPX pid's are handled.
 */

static void ipx_snmp_process (boolean createflag)
{

    if (!createflag) {
	if (novell_snmp_pid != 0) {
	    process_kill(novell_snmp_pid);
	    novell_snmp_pid = 0;
	}
    } else {
	if ((novell_snmp_pid == 0) &&
	    (novell_running && !novell_shutting_down)) {
	    novell_snmp_pid = process_create(ipx_snmp, "IPX SNMP", LARGE_STACK,
					     ipx_snmp_priority);
	    if (novell_snmp_pid == NO_PROCESS)
		novell_snmp_pid = 0;
	}
    }
}

/*
 * Initialize IPX SNMP
 *
 * NOTE: novell_snmp_pid is not set to NO_PROCESS, but instead
 *       zero when there is no process. This is the way the rest
 *       of the IPX pid's are handled.
 */
void ipx_snmp_init (void)
{
    novell_snmp_pid = 0;
    ipx_snmp_priority = SNMP_PRIORITY_DEF;
    novell_snmpQ = NULL;
    ipx_snmp_priority_changed = NULL;
    reg_add_snmp_process(ipx_snmp_process, "ipx_snmp_process");
    reg_add_snmp_priority(ipx_snmp_pri_change, "ipx_snmp_pri_change");
}
      
