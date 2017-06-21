/* $Id: ip_snmp.c,v 3.13.8.12 1996/08/28 12:49:34 thille Exp $
 *------------------------------------------------------------------
 * ip_snmp.c -- Simple Network Management Protocol server
 * (derived from snmp_server.c)
 *
 * July 1988, Greg Satz
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_snmp.c,v $
 * Revision 3.13.8.12  1996/08/28  12:49:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.8.11  1996/08/03  23:28:52  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.13.8.10  1996/07/06  05:53:21  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.13.8.9  1996/07/03  17:35:57  jjohnson
 * CSCdi61958:  Remove support for undocumented snmp configuration commands
 * remove the last vestiges of support for wildcard snmp communities
 * Branch: California_branch
 *
 * Revision 3.13.8.8  1996/06/18  01:45:00  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.13.8.7  1996/06/07  16:24:26  kao
 * CSCdi59777:  no snmp-server crashes router
 * Need to reset proc id after process is killed
 * Branch: California_branch
 *
 * Revision 3.13.8.6  1996/06/01  00:21:24  jjohnson
 * CSCdi56776:  SYS-2-GETBUF, Process= IP SNMP
 * Don't try processing messages with an overly-large udp->length,
 * complain about them instead
 *
 * Revision 3.13.8.5  1996/05/21  09:51:11  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.13.8.4  1996/05/04  01:02:36  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.13.8.3  1996/04/15  21:16:08  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.13.8.2  1996/03/28  16:45:44  jjohnson
 * CSCdi50399:  SNMP can drive CPU utilization to over 90%
 * Make SNMP processes default to low priority, and add a hidden config
 * command to allow customers to change it
 * Branch: California_branch
 *
 * Revision 3.13.8.1  1996/03/18  20:16:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/16  06:57:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.2  1996/03/07  09:45:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  00:59:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/02/28  09:33:01  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.12  1996/02/13  19:50:32  hampton
 * Remove unnecessary includes.  [CSCdi48844]
 *
 * Revision 3.11  1996/02/12  07:31:16  jjohnson
 * CSCdi47099:  not nearly enough processes call process_set_crashblock
 * incorporate code review comment
 *
 * Revision 3.10  1996/02/12  06:57:54  jjohnson
 * CSCdi47099:  not nearly enough processes call process_set_crashblock
 * There isn't much demand for network management during a meltdown
 *
 * Revision 3.9  1996/02/07  16:12:56  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/01/25  11:19:20  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.7  1996/01/24  20:15:00  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.6  1996/01/20  02:35:41  jjohnson
 * CSCdi47220:  need snmp packet debugging
 *
 * Revision 3.5  1996/01/19  00:57:17  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/11  02:26:38  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.3  1995/11/18  00:58:04  thille
 * CSCdi38736:  BGP snmp trap responses getting lost
 * Fix and modularize the bgp trap support.  While I am here, removed some
 * trap stuff in ip_snmp.c that was obsoleted by Jeff's modularization of
 * traps.
 *
 * Revision 3.2  1995/11/17  09:33:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:04:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/11  23:49:14  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.6  1995/08/11 18:37:27  rbatz
 * CSCdi38250:  CIP CSNA SNMP traps do not work
 *
 * Revision 2.5  1995/08/03  07:42:03  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.4  1995/07/24 18:23:51  sdurham
 * CSCdi37557:  snmp stops responding
 *                 snmp processes (ip and trap) need to process all queue
 *                 elements when they get called.
 *
 * Revision 2.3  1995/06/13  04:44:53  thille
 * CSCdi35746:  More control needed over tftp from SNMP
 * Add access-list checking to SNMP sets which cause tftp reads or writes
 * of config files.
 *
 * Revision 2.2  1995/06/09  13:05:26  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:57:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../ip/udp_debug.h"
#include "../tcp/tcp.h"
#include "config.h"
#include "../snmp/snmp_api.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../ip/msg_ipsnmp.c"		/* Not a typo, see logger.h */
#include "parser.h"
#include "../parser/parser_defs_snmp.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../snmp/snmp_debug.h"
#include "../snmp/snmp_registry.h"
#include "../snmp/sr_sysmib2.h"
#include "mgd_timers.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "access.h"

/*
 * Forward References.
 */
static boolean snmp_route_ip(ipsocktype *soc);

/*
 * Local variables
 */
#define logtype struct log_
logtype {
    logtype *next;                      /* ptr to next entry */
    ipaddrtype addr;                    /* internet address */
    ulong port;				/* UDP port */
    ipsocktype *sock;                   /* UDP socket */
    queuetype queue;                    /* hold onto packets to send... */
    mgd_timer timeout;                  /* ...after this times out */
    ulong sent;                         /* traps sent */
    ulong dropped;                      /* traps dropped */
};

static mgd_timer ip_snmp_master;
static watched_boolean *ip_snmp_priority_changed;
static queuetype snmphostQ;
static ipsocktype *snmp_soc;
static int ip_snmp_pid;
static process_priority_t ip_snmp_priority;
static sys_timestamp last_trap_set;

/*
 * SNMP IP parser function
 */
void snmp_ip_command (parseinfo *csb)
{
    if (system_loading) {
	return;
    }

    if (csb->nvgen) {
	if (!snmp_serving) {
	    return;
	}
	switch (csb->which) {
	  case SNMP_TFTP_LIST:
	    nv_write((snmp_tftp_server_acl != 0), "%s %d",
		     csb->nv_command, snmp_tftp_server_acl);
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case SNMP_TFTP_LIST:
	snmp_tftp_server_acl = GETOBJ(int,1);
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * Parse chain for IP SNMP commands
 */
#define	ALTERNATE	NONE
#include "cfg_snmp_ip.h"
LINK_POINT(ip_snmp_config_commands, ALTERNATE);
#undef	ALTERNATE

static parser_extension_request ip_snmp_chain_init_table[] = {
    { PARSE_ADD_CFG_SNMP_CMD, &pname(ip_snmp_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ip_snmp
 * Provide IP SNMP services
 */

static process ip_snmp (void)
{
    paktype *pakin;
    paktype *pakout;
    iphdrtype *ip;
    udptype *udp;
    uchar *offset;
    uchar *outbuf;
    int outbuflen, pak_count;
    uint start;
    logtype *lp;
    leveltype level;
    boolean sent;
    mgd_timer *expired_timer;
    ulong major, minor;

    process_wait_on_system_init();

    process_set_crashblock(THIS_PROCESS, TRUE);

    /*
     * loop until we're able to open the incoming snmp udp port
     */
    for (;;) {
	snmp_soc = open_ipsocket(UDP_PROT, NULL, NULL, SNMP_PORT, 0, 
				 SOCK_ANYSRC);
	if (snmp_soc == NULL) {
	    errmsg(&msgsym(SOCKET, IP_SNMP));
	    process_sleep_for(ONEMIN);
	    continue;
	}
	break;
    }

    process_watch_mgd_timer(&ip_snmp_master, ENABLE);
    process_watch_queue(snmp_soc->inq, ENABLE, RECURRING);
    ip_snmp_priority_changed = create_watched_boolean("SNMP reprioritize", 0);
    process_watch_boolean(ip_snmp_priority_changed, ENABLE, RECURRING);

    /*
     * main snmp task loop
     */
    while (TRUE) {
	/*
	 * dismiss the task until we have something to do
	 */
	process_wait_for_event();

	process_get_wakeup(&major, &minor);
	switch (major) {
	  case BOOLEAN_EVENT:
	    process_set_priority(ip_snmp_priority);
	    process_set_boolean(ip_snmp_priority_changed, FALSE);
	    break;
	  case TIMER_EVENT:
	    /*
	     * see if we can send any traps
	     */
	    while (mgd_timer_expired(&ip_snmp_master)) {
		expired_timer = mgd_timer_first_expired(&ip_snmp_master);
		lp = mgd_timer_context(expired_timer);
		sent = FALSE;
		level = raise_interrupt_level(NETS_DISABLE);
		pakin = (paktype *)lp->queue.qhead;
		if (pakin != NULL && snmp_route_ip(lp->sock)) {
		    sent = TRUE;
		    dequeue(&lp->queue);
		    reset_interrupt_level(level);
		    lp->sent++;
		    if (reg_invoke_snmp_debug(DEBUG_SNMP_PACKETS))
			buginf("\nSNMP: Packet sent via UDP to %i",
			       lp->sock->faddress.ip_addr);
		    write_ipsocket(lp->sock, pakin, pakin->length);
		} else
		    reset_interrupt_level(level);
		if (!QUEUEEMPTY(&lp->queue)) {
		    if (sent) {
			mgd_timer_start(&lp->timeout, ONESEC/4);
		    } else {
			mgd_timer_start(&lp->timeout, snmp_def_timeout);
		    }
		} else {
		    mgd_timer_stop(&lp->timeout);
		}
	    }
	    break;

	  case QUEUE_EVENT:
	    pak_count = PAK_SWITCHCOUNT;
	    /*
	     * see if we have an incoming request
	     */
	    while (TRUE) {
		/*
		 * Process only a limited number of packets per pass
		 */
		if (pak_count-- == 0) {
		    break;
		}
		pakin = read_ipsocket(snmp_soc, FALSE, 0, &udp, &offset);
		if (pakin == NULL) {
		    break;
		}
		ip = (iphdrtype *)ipheadstart(pakin);
   
		if (reg_invoke_snmp_debug(DEBUG_SNMP_PACKETS))
		    buginf("\nSNMP: Packet received via UDP from %i on %s",
			   ip->srcadr, pakin->if_input->namestring);
  
		/* setup the reply socket */
		snmp_soc->faddress.ip_addr = ip->srcadr;
		snmp_soc->faddress.type = ADDR_IP;
		snmp_soc->fport = udp->sport;
		snmp_soc->laddress.ip_addr = ip_reply_srcadr(pakin);
		snmp_soc->laddress.type = ADDR_IP;

		/*
		 * size the response buffer:
		 * it should be at least as large as snmp_size
		 * but should be larger if the incoming packet is
		 * larger
		 */

		outbuflen = MAX(snmp_size, udp->length);

		if (outbuflen > SNMP_MAX_MSG_SIZE_MAX) {
		    /*
		     * we have a problem.  the udp length is
		     * probably bogus.  issue an errmsg, and set
		     * pakout to NULL so that we bypass snmp
		     */
		    errmsg(&msgsym(TOOBIG, IP_SNMP),
			   ip->srcadr, pakin->datagramsize, udp->length);
		    pakout = NULL;
		} else {
		    /* attach a response buffer to the socket */
		    pakout = setup_ipsocket(snmp_soc, outbuflen, &start);
		}

		if (pakout == NULL) {
		    /*
		     * we can't attach a buffer, so we can't process the message
		     * we should probably bump an error counter here, but chances
		     * are we're gonna die Real Soon Now(tm), so why bother?
		     */
		} else {
		    /* pass the packet and response buffer to the snmp engine */
		    outbuf = pakout->data_area + start;
		    reg_invoke_snmp_engine(&snmp_soc->faddress,
					   offset, udp->length - UDPHEADERBYTES,
					   outbuf, &outbuflen);
   
		    if (outbuflen) {
			/* if we have a response, send it */
			if (reg_invoke_snmp_debug(DEBUG_SNMP_PACKETS))
			    buginf("\nSNMP: Packet sent via UDP to %i",
				   snmp_soc->faddress.ip_addr);
			write_ipsocket(snmp_soc, pakout, outbuflen);
		    } else {
			/* otherwise deallocate the response buffer */
			retbuffer(pakout);
		    }
		}

		retbuffer(pakin);
                (void) process_may_suspend();
	    }
	    break;

	  default:
	    errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
	    break;
	}
    }
}

/*
 * ip_snmp_process
 * Create or delete a process to handle SNMP for IP
 * Invoked as SERVICE_SNMP_PROCESS
 */

static void ip_snmp_process (boolean createflag)
{
    if (!createflag) {
	if (ip_snmp_pid != NO_PROCESS) {
	    process_kill(ip_snmp_pid);
	    ip_snmp_pid = NO_PROCESS;
	    reg_invoke_ip_close_socket(snmp_soc);
	}
    } else {
	if (ip_snmp_pid == NO_PROCESS) {
	    ip_snmp_pid = process_create(ip_snmp, "IP SNMP", LARGE_STACK,
					 ip_snmp_priority);
	}
    }
}

/*
 * ip_snmp_priority_change
 * Handle snmp process priority change requests
 */
static void ip_snmp_priority_change (process_priority_t priority)
{
    /*
     * a process can only change its own priority, so tell the
     * ip snmp process to do it
     */
    ip_snmp_priority = priority;
    if (ip_snmp_priority_changed) {
	process_set_boolean(ip_snmp_priority_changed, TRUE);
    }
}

/*
 * snmp_route_ip
 * Return whether a route to final destination is up or not.
 */

static boolean snmp_route_ip (ipsocktype *soc)
{
    ipaddrtype ipaddr;
    idbtype *idb;

    ipaddr = soc->faddress.ip_addr;
    idb = reg_invoke_ip_first_hop(&ipaddr, ipaddr, IP_PATH_DEFFLAGS);
    if (idb == NULL)
	return(FALSE);
    return(TRUE);
}

/*
 * ip_snmp_trap_address
 * Return the "source" address of a trap being sent to a particular destination
 */
static ipaddrtype
ip_snmp_trap_address (ipaddrtype destination)
{
    /*
     * if an snmp-server trap-interface has been configured, that
     * interface has an ip address, and that interface is not deleted,
     * then use that interface's ip address as the trap source address
     */
    if ((snmp_trap_idb) && (snmp_trap_idb->ip_address) &&
	(!(snmp_trap_idb->hwptr->status & IDB_DELETED)))
	return (snmp_trap_idb->ip_address);

    /*
     * if an snmp-server trap-interface has been configured, but the
     * above code didn't return an ip address, then complain, but no
     * more than once every ten seconds
     */
    if ((snmp_trap_idb) &&
	ELAPSED_TIME(last_trap_set) > (10 * ONESEC)) {
	errmsg(&msgsym(NOTRAPIP, IP_SNMP), snmp_trap_idb->namestring);
	GET_TIMESTAMP(last_trap_set);
    }

    /*
     * choose a source address based on the destination
     */
    return (ip_best_local_address(destination, FALSE));
} 

/*
 * lookup_host
 * finds the host record in the host queue
 */

static logtype *
lookup_host (ipaddrtype destaddr, ulong destport)
{
    logtype *loghost;

    for (loghost = snmphostQ.qhead; loghost; loghost = loghost->next) {
	if ((loghost->addr == destaddr) &&
	    (loghost->port == destport)) {
	    /*
	     * found the appropriate entry
	     */
	    return (loghost);
	}
    }
    return (NULL);
}

/*
 * add_host
 * adds a host record to the host queue
 */

static logtype *
add_host (ipaddrtype destaddr, ulong destport)
{
    logtype *loghost;

    loghost = malloc(sizeof(logtype));
    if (loghost) {

	/*
	 * initialize the new record
	 */
	loghost->next = NULL;
	loghost->addr = destaddr;
	loghost->port = destport;
	loghost->sock = NULL;
	mgd_timer_init_leaf(&loghost->timeout, &ip_snmp_master, 0, loghost,
			    FALSE);
	loghost->sent = 0;
	loghost->dropped = 0;
	queue_init(&loghost->queue, snmp_def_qlen);

	/*
	 * add the new record to the list
	 */
	enqueue(&snmphostQ, loghost);
    }
    return (loghost);
}

static void
ip_snmp_add_host (ipaddrtype address)
{

    if (lookup_host(address, SNMP_TRAP_PORT) == NULL)
	add_host(address, SNMP_TRAP_PORT);
}

static void
ip_snmp_delete_host (ipaddrtype address)
{
    logtype *loghost;

    loghost = lookup_host(address, SNMP_TRAP_PORT);
    if (loghost) {
	/*
	 * turn off the timer
	 */
	mgd_timer_stop(&loghost->timeout);

	/*
	 * free up any queued packets
	 */
	while (loghost->queue.qhead)
	    IP_DISPOSE(ip, encapsfailed, dequeue(&loghost->queue));

	/*
	 * free up the socket
	 */
	if (loghost->sock)
	    reg_invoke_ip_close_socket(loghost->sock);

	/*
	 * free up the record
	 */
	unqueue(&snmphostQ, loghost);
	free(loghost);
    }
}

/*
 * ip_snmp_trap
 * Send an SNMP trap message
 */

static boolean
ip_snmp_trap (OctetString      *destination,
	      unsigned char    *message_ptr,
	      long		message_len)
{
    ulong	destaddr;
    ulong	destport;
    logtype    *loghost;
    leveltype   level;
    paktype    *pak;
    uint	start;
    addrtype 	faddr;

    /*
     * the destination address must be either 4 octets (IP address)
     * or 6 octets (IP address and port number)
     */
    if (destination->length == 4) {
	destaddr = reg_invoke_OctetStringToIP(destination);
	destport = SNMP_TRAP_PORT;
    } else if (destination->length == 6) {
	OctetString tempOS;

	destaddr = reg_invoke_OctetStringToIP(destination);

	tempOS.length = 2;
	tempOS.octet_ptr = &destination->octet_ptr[4];
	destport = reg_invoke_OctetStringToUlong(&tempOS);
    } else {
	if (reg_invoke_snmp_debug(DEBUG_SNMP_PACKETS))
	    buginf("\nSNMP: UDP detected malformed address");
	return (FALSE);
    }

    /*
     * find the appropriate log host entry in the sorted log host queue
     * if there isn't an entry, create one
     */
    loghost = lookup_host(destaddr, destport);
    if (loghost == NULL) {
	/*
	 * didn't find an entry in the list, so create a new entry
	 */
	loghost = add_host(destaddr, destport);
	if (loghost == NULL)
	    return (FALSE);
    }

    /*
     * make sure the socket is open
     */
    if (loghost->sock == NULL) {
	faddr.type = ADDR_IP;
	faddr.ip_addr = destaddr;
	loghost->sock = open_ipsocket(UDP_PROT, NULL, &faddr,
				      0, destport, SOCK_ANYSRC);
	if (loghost->sock == NULL) {
	    if (reg_invoke_snmp_debug(DEBUG_SNMP_PACKETS))
		buginf("\nSNMP: Unable to open UDP socket to %i(%d)",
		       destaddr, destport);
	    return (FALSE);
	}
    }

    /*
     * make sure the socket has a good source address
     */
    loghost->sock->laddress.ip_addr = ip_snmp_trap_address(destaddr);

    /*
     * grab a buffer and fill it with the trap message
     */
    pak = setup_ipsocket(loghost->sock, message_len, &start);
    if (pak == NULL) {
	/*
	 * we're running out of memory.  no need to bitch
	 * since we'll die real soon now
	 */
	return (FALSE);
    }

    bcopy(message_ptr, pak->data_area + start, message_len);
    pak->length = message_len;

    /*
     * place the message on the host transmit queue.
     * if the queue is full, drop the oldest pending message
     */
    level = raise_interrupt_level(NETS_DISABLE);
    if (QUEUEFULL(&loghost->queue)) {
	IP_DISPOSE(ip, encapsfailed, dequeue(&loghost->queue));
	loghost->dropped++;
	if (reg_invoke_snmp_debug(DEBUG_SNMP_PACKETS))
	    buginf("\nSNMP: %i queue overflow, dropping packet", destaddr);
    }
    enqueue(&loghost->queue, pak);
    if (reg_invoke_snmp_debug(DEBUG_SNMP_PACKETS))
	buginf("\nSNMP: Queuing packet to %i", destaddr);
    reset_interrupt_level(level);
    if (!mgd_timer_running(&loghost->timeout))
	mgd_timer_start(&loghost->timeout, ONESEC/4);
    return (TRUE);
}

/*
 * snmp_logging
 * Output SNMP TRAP information
 */

static void snmp_logging (void)
{
    logtype *lp;
    boolean first;

    if (!snmp_serving)
	return;
    printf("\n\nSNMP logging: %s",
	   snmphostQ.qhead ? "enabled" : "disabled");
    first = TRUE;
    for (lp = (logtype *)snmphostQ.qhead; lp; lp = lp->next) {
	printf("\n    Logging to %i", lp->addr);
	printf(", %u/%u, %u sent, %u dropped.",
	       lp->queue.count, lp->queue.maximum, lp->sent, lp->dropped);
    }
}

/*
 * snmp_access_check
 * check the source of an SNMPv1 message
 */

boolean snmp_access_check (int access_list, addrtype *address)
{
    return(fast_check(access_list, address->ip_addr));
}

/*
 * Initialize IP SNMP
 */
static void ip_snmp_init (subsystype *dummy)
{
    TIMER_STOP(last_trap_set);
    queue_init(&snmphostQ, 0);
    mgd_timer_init_parent(&ip_snmp_master, NULL);
    ip_snmp_pid = NO_PROCESS;
    ip_snmp_priority = SNMP_PRIORITY_DEF;

    if (!registry_populated(REG_SNMP))
	return;

    reg_add_snmp_show(snmp_logging, "snmp_logging");
    reg_add_snmp_access_check(ADDR_IP, snmp_access_check, "snmp_access_check");
    reg_add_snmp_process(ip_snmp_process, "ip_snmp_process");
    reg_add_snmp_priority(ip_snmp_priority_change, "ip_snmp_priority_change");
    reg_add_snmp_trap_address(ip_snmp_trap_address, "ip_snmp_trap_address");
    reg_add_snmp_send_trap(SNMP_UDP_DOMAIN, ip_snmp_trap, "ip_snmp_trap");
    reg_add_snmp_send_trap(RFC_1157_DOMAIN, ip_snmp_trap, "ip_snmp_trap");
    reg_add_snmp_add_host(ip_snmp_add_host, "ip_snmp_add_host");
    reg_add_snmp_delete_host(ip_snmp_delete_host, "ip_snmp_delete_host");
    parser_add_command_list(ip_snmp_chain_init_table, "ip_snmp");
}

/*
 * IPSNMP subsystem header
 */

#define IPSNMP_MAJVERSION 1
#define IPSNMP_MINVERSION 0
#define IPSNMP_EDITVERSION  1

SUBSYS_HEADER(ip_snmp, IPSNMP_MAJVERSION, IPSNMP_MINVERSION,
	      IPSNMP_EDITVERSION,
	      ip_snmp_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
      
