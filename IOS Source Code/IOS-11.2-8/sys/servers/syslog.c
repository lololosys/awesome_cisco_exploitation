/* $Id: syslog.c,v 3.4.22.5 1996/07/06 05:56:21 thille Exp $
 * $Source: /release/112/cvs/Xsys/servers/syslog.c,v $
 *------------------------------------------------------------------
 * syslog.c -- send error messages to syslog daemon
 * 
 * January 1987, Greg Satz
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: syslog.c,v $
 * Revision 3.4.22.5  1996/07/06  05:56:21  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.4.22.4  1996/06/28  23:25:38  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.22.3  1996/05/04  01:37:51  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.4.22.2  1996/04/15  21:22:39  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.4.22.1  1996/03/18  21:47:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.2  1996/03/07  10:42:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/02/20  17:09:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/01/24  20:19:52  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.3  1996/01/22  06:59:14  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:51:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:44:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ip_debug.h"
#include "config.h"
#include "../ip/ip_registry.h"
#include "../os/parser_defs_logger.h"
#include "subsys.h"

/*
 * Definitions
 */

typedef struct loglisttype_ {
    struct loglisttype_ *next;		/* ptr to next entry */
    ipaddrtype addr;			/* internet address */
    ipsocktype *sock;			/* udp socket */
    boolean okay;			/* initialized */
    ulong count;			/* number of messages */
} loglisttype;

/*
 * Storage
 */
static queuetype loghostQ;
static idbtype *syslog_source_idb;

/*
 * syslog_laddr
 *
 * Compute the proper local address given a source interface.
 */

static ipaddrtype syslog_laddr (idbtype *idb)
{
    if (idb && idb->ip_address && interface_up(idb))
	return(syslog_source_idb->ip_address);
    else
	return(0);
}

/*
 * syslog_adjust_source_interface
 *
 * Fix open sockets to use the the interface's address as local address.
 */

static void syslog_adjust_source_interface (idbtype *idb)
{
    loglisttype *lp;
    ipaddrtype laddr;

    laddr = syslog_laddr(idb);
    for (lp = (loglisttype *) loghostQ.qhead; lp; lp = lp->next) {
	lp->sock->laddress.ip_addr = laddr;
	if (laddr)
	    lp->sock->status |= SOCK_SRC_SPECIFIED;
	else
	    lp->sock->status &= ~SOCK_SRC_SPECIFIED;
    }
}

/*
 * syslog_config
 * Configure host addresses for syslog logging
 * Called by SERVICE_LOG_CONFIG
 */

static void syslog_config (parseinfo *csb)
{
    loglisttype *lp;
    ipaddrtype addr;
    addrtype faddr, laddr;
    uint flags;

    /*
     * Write out configuration
     */
    if (csb->nvgen) {
	switch (csb->which) {
	case P_LOG_HOST:
	    for (lp = (loglisttype *) loghostQ.qhead; lp; lp = lp->next)
		nv_write(TRUE, "%s %i", csb->nv_command, lp->addr);
	    break;
	case P_LOG_SOURCE:
	    if (syslog_source_idb)
		nv_write(TRUE, "%s %s", csb->nv_command, 
			 syslog_source_idb->namestring);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case P_LOG_HOST:
	addr = GETOBJ(paddr,1)->ip_addr;
	if (csb->sense) {
	    for (lp= (loglisttype *)loghostQ.qhead; lp; lp = lp->next) {
		if (addr == lp->addr)
		    return;
	    }
	    lp = malloc(sizeof(loglisttype));
	    if (lp) {
		faddr.type = laddr.type = ADDR_IP;
		faddr.ip_addr = lp->addr = addr;
		laddr.ip_addr = syslog_laddr(syslog_source_idb);
		flags = SOCK_ANYSRC;
		if (laddr.ip_addr)
		    flags |= SOCK_SRC_SPECIFIED;
		lp->okay = FALSE;
		lp->count = 0;
		lp->sock = NULL;
		lp->sock = open_ipsocket(UDP_PROT, &laddr, &faddr, 0,
					 SYSLOG_PORT, flags);
		if (lp->sock == NULL)
		    printf("\nCannot open logging port to %i", lp->addr);
		else
		    lp->okay = TRUE;
		enqueue(&loghostQ, lp);
	    } else
		printf(nomemory);
	} else {
	    for (lp = (loglisttype *)loghostQ.qhead; lp; lp = lp->next) {
		if (lp->addr == addr) {
		    reg_invoke_ip_close_socket(lp->sock);
		    unqueue(&loghostQ, lp);
		    free(lp);
		    return;
		}
	    }
	    printf("\nHost %i not found for logging", addr);
	}
	break;

    case P_LOG_SOURCE:
	if (csb->sense) {
	    if (syslog_source_idb != GETOBJ(idb,1)) {
		syslog_source_idb = GETOBJ(idb,1);
		syslog_adjust_source_interface(syslog_source_idb);
	    }
	} else {
	    syslog_source_idb = NULL;
	    syslog_adjust_source_interface(syslog_source_idb);
	}
	break;
    }
}

/*
 * syslog_send
 * Send an error message to hosts in our syslog list.
 * Called by SERVICE_LOG_SEND.
 */

static void syslog_send (char *buffer, uint buflen, char *msgp, 
			 uint msglen, int msglvl)
{
    loglisttype *lp;
    uint totlen;
    char* pp;
    uint offset;
    paktype *ippak;
    addrtype faddr, laddr;
    uint flags;

    totlen = buflen + msglen;
    for (lp = (loglisttype*) loghostQ.qhead; lp != NULL; lp = lp->next) {
	/*
	 * Try opening the socket it if hasn't already been opened.
	 */
	if (!lp->sock) {
	    faddr.type = laddr.type = ADDR_IP;
	    faddr.ip_addr = lp->addr;
	    laddr.ip_addr = syslog_laddr(syslog_source_idb);
	    flags = SOCK_ANYSRC;
	    if (laddr.ip_addr)
		flags |= SOCK_SRC_SPECIFIED;
	    lp->sock = open_ipsocket(UDP_PROT, &laddr, &faddr, 0, SYSLOG_PORT,
				     flags);
	    if (lp->sock)
		lp->okay = TRUE;
	    else
		continue;
	}
	ippak = setup_ipsocket(lp->sock, totlen, &offset);
	if (!ippak)
	    return;
	pp = (char*) ippak->data_area + offset;
	bcopy(buffer, pp, buflen);
	bcopy(msgp, &pp[buflen], msglen);
	write_ipsocket(lp->sock, ippak, totlen);
	++lp->count;
    }
}

/*
 * syslog_show
 * Support for the "show logging" command
 * Called by SERVICE_LOG_SHOW.
 */

static void syslog_show (void)
{
    loglisttype *lp;

    for (lp = (loglisttype *)loghostQ.qhead; lp; lp = lp->next) {
	if (lp->okay) {
	    printf("\n        Logging to %i, %u message lines logged",
		   lp->addr, lp->count);
	}
    }
}

/*
 * syslog_init
 * Initialize syslog support
 */

static void syslog_init (subsystype *dummy)
{
    queue_init(&loghostQ, 0);

    /*
     * Register some functions
     */
    reg_add_log_config(syslog_config, "syslog_config");
    reg_add_log_send(syslog_send, "syslog_send");
    reg_add_log_show(syslog_show, "syslog_send");
}

/*
 * SYSLOG subsystem header
 */

#define SYSLOG_MAJVERSION 1
#define SYSLOG_MINVERSION 0
#define SYSLOG_EDITVERSION  1

SUBSYS_HEADER(syslog, SYSLOG_MAJVERSION, SYSLOG_MINVERSION, SYSLOG_EDITVERSION,
	      syslog_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
