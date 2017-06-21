/* $Id: tacacs.c,v 3.11.18.11 1996/08/27 21:41:42 sbelair Exp $
 * $Source: /release/112/cvs/Xsys/servers/tacacs.c,v $
 *------------------------------------------------------------------
 * tacacs.c - TAC access protocol support
 *
 * May 1988, Greg Satz
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tacacs.c,v $
 * Revision 3.11.18.11  1996/08/27  21:41:42  sbelair
 * CSCdi52745:  username limited to 49 characters
 * Branch: California_branch
 * Instead of copying the username into an array of 50 bytes on the
 * stack, malloc the necessary space for the username once we know
 * how long it is, and the free the buffer later.
 *
 * Revision 3.11.18.10  1996/08/13  18:46:58  widmer
 * CSCdi65503:  security hole when using httpd and no enable passwd is set
 * Branch: California_branch
 * Make http authentication work with "enable use-tacacs"
 *
 * Revision 3.11.18.9  1996/07/06  05:56:24  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.11.18.8  1996/06/28  23:25:41  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.11.18.7  1996/06/17  23:36:35  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.11.18.6  1996/05/23  07:38:12  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.11.18.5  1996/05/21  10:01:34  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.18.4  1996/04/19  15:29:28  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.9  1996/04/10  23:08:21  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 2.1.6.8  1996/04/09  21:45:19  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.7  1996/03/22  17:25:09  lol
 * Branch: IosSec_branch
 *         Modularise tacacs_plus tcp handle cleanup.
 *
 * Revision 2.1.6.6  1996/02/28  06:15:03  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.1.6.5  1996/02/21  03:59:09  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.4  1996/01/23  06:32:40  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.3  1995/12/19  04:13:05  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.2  1995/12/17  02:16:18  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  17:39:28  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.11.18.3  1996/04/15  21:22:42  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.11.18.2  1996/04/15  14:59:47  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.11.18.1  1996/03/18  21:47:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.12.2  1996/03/07  10:42:11  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.12.1  1996/02/20  17:09:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/02/07  16:15:04  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.10  1996/02/01  06:08:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.9  1996/01/24  20:19:54  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.8  1995/12/21  01:00:44  lol
 * CSCdi44342:  spurious memory access
 *
 * Revision 3.7  1995/12/19  00:40:56  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.6  1995/12/17  18:36:45  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/14  21:34:11  lol
 * CSCdi45530:  crash in tacacs/pap
 *
 * Revision 3.4  1995/12/08  00:54:46  irfan
 * CSCdi45224:  tacacs could use a specified source address
 * allow tacacs (TA, XTA, TA+) to use a specific (up) interface's IP
 * address.
 *
 * Revision 3.3  1995/11/22  22:38:26  che
 * CSCdi42049:  TACACS failures when using enable password privledge levels
 * Added new parameter to xtacacs_enable() to specify whether we're
 * doing an "ask" or a "notify".
 *
 * Revision 3.2  1995/11/17  18:51:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/10/19  21:17:19  che
 * CSCdi36726:  tacacs login causes crash
 * Lock the memory location of the Tacacs server we're trying to contact
 * so it doesn't get freed up while we're waiting for a response.
 *
 * Revision 2.7  1995/09/11  19:00:12  che
 * CSCdi38921:  Further TACACS sanity checks
 * Checks to make sure that the TACACS response packet we received
 * originated from a TACACS server in our server list.
 *
 * Revision 2.6  1995/08/29  21:13:30  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * Added new knob "ip bootp server" to turn/on off BOOTP server.
 * Send ICMP port unreachable if a packet is received for TACACS port and
 * there is no TACACS server configured.
 *
 * Revision 2.5  1995/08/11  06:01:38  billw
 * CSCdi36286:  No status msg. displayed when enable fails with (old)
 * TACACS
 *
 * Revision 2.4  1995/08/08  16:48:38  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.3  1995/07/08 02:21:33  irfan
 * CSCdi28744:  memory corruption crashes while using xtacacs
 * Uninitialized ta_blocktypes entries in tacacs queues cause
 * overwrites to unallocated and low memory. While at it, catch
 * and return some allocated packet buffers too.
 *
 * Revision 2.2  1995/06/20  16:11:28  lol
 * CSCdi34401: Tacacs notif.type 6-ena and 10-slipoff not sent to
 *             nondefault server
 *
 * Revision 2.1  1995/06/07  22:44:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "../h/auth_registry.regh"
#include "../ip/ip_registry.h"
#include "packet.h"
#include "ttysrv.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../ip/udp_debug.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_tacacs.h"
#include "../ui/command.h"
#include "aaa.h"
#include "tacacs_plus.h"
#include "tacacs.h"
#include "login.h"
#include "../util/md5_crypt.h"
#include "../parser/parser_actions.h"
#include "../util/random.h"
#include "../if/network.h"
#include "../os/os_debug_flags.h"
#include <ciscolib.h>
#include <string.h>
#include "subsys.h"

#define TACPASSWDLEN 40

/*
 * Forward declarations
 */
static void tacacs_adjust_source_interface(idbtype *);

/*
 * Global variables
 */
boolean tacacs_pw_optional;
queuetype tacacshostQ;
queuetype tacacsreqQ;
queuetype tacacslogQ;
uint tacacs_extended;
uint tacacs_lastresort;
uint tacacs_retrans;
ulong tacacs_timeout;
ulong tacacs_always;
boolean tacacs_directed_request;
boolean tacacs_directed_restricted;
idbtype *tacacs_source_idb;      /* use source IP addr from this idb */


/*
 * tacacs_password
 * Ask the user for a password.  Return NULL if we timeout.
 * Note: we're returning a pointer to the buffer they passed in, or NULL 
 * if we get no password.
 */

static char *tacacs_password (tt_soc *tty, char *buff, int buflen)
{
    if (!tty)
	return(NULL);
    if (!askstring(tty, buff, buflen, "Password: ", 30*ONESEC, 1, TRUE))
	return(NULL);
    return(buff);
}

/*
 * tacacs_server
 * Perform actions based upon incoming TACACS requests
 * Returns TRUE if packet was absorbed, FALSE otherwise.
 */

boolean tacacs_server (paktype *pak, udptype *udp)
{
    tacacstype *tac = (tacacstype *)((uchar *)udp + UDPHEADERBYTES);
    tacacsblock *block;
    servtype *lp, *lp2;
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    if (udp->dport != TACACS_PORT)
	return(FALSE);
    if (!login_have_servers) {
	send_unreachable(pak, ICMP_UNRPORT);
	datagram_done(pak);
	return(TRUE);
    }

    if (udp_debug)
	buginf("\nTACACS: received %d from %i, via %s", tac->type,
	    ip->srcadr, pak->if_input->namestring);

    if (((tac->version & XTA_MAJOR_VERSION_MASK) == XTA_VERSION) &&
	((tac->version & XTA_MINOR_VERSION_MASK) <= XTA_MINOR_VER_CUR)) {
	xtacacs_server(pak);
    } else if (tac->version == TA_VERSION) {
	switch (tac->type) {
	  case TA_QUERY:
	    break;
	  case TA_ANSWER:
	    for (block = (tacacsblock *)tacacsreqQ.qhead; block;
		 block = block->next)
		if (tac->trans == block->trans)
		    break;
	    if (block == NULL)
		break;
	    if (block->done)
		break;
	    lp2 = (servtype *)tacacshostQ.qhead;
	    if (ip->srcadr) {
		while(lp2) {
		    if (lp2->addr == ip->srcadr)
			break;
		    else
			lp2 = lp2->next;
		}
	    } else {
		lp2 = NULL;
	    }
	    if (lp2 == NULL) {
		if (udp_debug) {
		    buginf("\nTAC: Received bogus response from %i.  " 
			   "Discarding...",ip->srcadr);
		}
		break;
	    }
	    block->response = tac->namelen;  /* reused field */
	    block->reason = tac->pwlen;	/* reused field */
	    if (udp->length > TACACS_SIZE)
		block->uuid = GETLONG((ulong *)((uchar *)tac + TACACS_SIZE));
	    block->done = TRUE;
	    break;
	  case TA_CHANGE:
	  case TA_FOLLOW:
 	    for (block = (tacacsblock *)tacacsreqQ.qhead; block;
		 block = block->next)
		if (tac->trans == block->trans)
		    break;
	    if (block == NULL)
		break;
	    GET_TIMESTAMP(block->timeout);	/* Set to timeout */
	    for (lp = (servtype *)tacacshostQ.qhead; lp; lp = lp->next)
		if (ip->srcadr == lp->addr)
		    break;
	    if (lp == NULL)
		break;
	    /*
	     * Add it to head or end
	     */
	    unqueue(&tacacshostQ, lp);
	    if (tac->type == TA_CHANGE)
		enqueue(&tacacshostQ, lp);
	    else
		requeue(&tacacshostQ, lp);
	    break;
	}
    }
    datagram_done(pak);
    return(TRUE);
}

/*
 * tacacs_service
 * Parse the "tacacs-server" and "ip tacacs" command options
 * [no] tacacs-server attempts N
 * [no] tacacs-server host name [single-connection] [port <n> ] [timeout <N>] [key <STRING>]
 * [no] tacacs-server retransmit N
 * [no] tacacs-server timeout N
 * [no] tacacs-server last-resort [password | succeed]
 * [no] tacacs-server extended
 * [no] tacacs-server authenticate connections | slip | enable
 * [no] tacacs-server notify connections | slip | enable | logout
 * [no] tacacs-server optional-passwords
 * [no] tacacs-server directed-request [restricted]
 ****----> [no] ip tacacs source-interface <idb>
 */
void tacacs_service (parseinfo *csb)
{
    servtype *lp, *next_lp;
    char *txt;
    addrtype laddr;
    addrtype faddr;

    if (csb->nvgen) {
	if (!tacacshostQ.qhead) {
	    return;
	}
	switch (csb->which) {
	  case TACACS_ATTEMPTS:
	    nv_write(login_server_attempts != LOGIN_DEF_ATTEMPTS,
		     "%s %d", csb->nv_command, login_server_attempts);
	    break;
	  case TACACS_HOST:
	    for (lp = (servtype *)tacacshostQ.qhead; lp; lp = lp->next) {
		nv_write(TRUE, "%s %i", csb->nv_command, lp->addr);
		if (!old_access_control) {
		    nv_add((lp->single_connection), " single-connection");
		    nv_add(lp->port, " port %d", lp->port);
		    nv_add((lp->timeout), " timeout %d", lp->timeout);
		    nv_add(lp->key != NULL, " key %s", lp->key);
		}
	    }
	    break;
	  case TACACS_LAST:
	    if (tacacs_lastresort == GETOBJ(int,1)) {
		nv_write(TRUE, csb->nv_command);
	    }
	    break;
	  case TACACS_RETRANSMIT:
	    nv_write(tacacs_retrans != TACACS_RETRANS, "%s %d",
		     csb->nv_command, tacacs_retrans);
	    break;
	  case TACACS_TIMEOUTVALUE:
	    nv_write(tacacs_timeout != TACACS_TIMEOUT, "%s %d",
		     csb->nv_command, tacacs_timeout/ONESEC);
	    break;
	  case TACACS_EXTENDED:
	    nv_write(tacacs_extended, csb->nv_command);
	    break;
	  case TACACS_ASK:
	  case TACACS_NOTE:
	    nv_write(tacacs_extended & GETOBJ(int,1), csb->nv_command);
	    nv_add((tacacs_always & GETOBJ(int,1)), " always");
	    nv_add(((GETOBJ(int,1) == TACACS_ASKSLIP) &&
		    (tacacs_extended & TACACS_GETSLIPACL)), " access-lists");
	    break;
	  case TACACS_PASSWORDOPT:
	    nv_write(tacacs_pw_optional, csb->nv_command);
	    break;
	  case TACACS_DIRECTED_REQUEST:
	      if (!tacacs_directed_request) {
		  nv_write(TRUE, "no %s", csb->nv_command);
	      } else if (tacacs_directed_restricted) {
		  nv_write(TRUE, "%s restricted", csb->nv_command);
	      }
	      break;
	case TACACS_LOGIN_TIMEOUT:
	    nv_write(login_input_timeout != LOGIN_DEF_TIMEOUT,
	    		"%s %d", csb->nv_command, login_input_timeout);
	    break;
	case TACACS_SOURCE_INTERFACE:
	    if (tacacs_source_idb) {
		nv_write(TRUE, "%s %s", csb->nv_command,
			 tacacs_source_idb->namestring);
	    }
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case TACACS_ATTEMPTS:
	if (csb->sense) {
	    login_server_attempts = GETOBJ(int,1);
	} else {
	    login_server_attempts = LOGIN_DEF_ATTEMPTS;
	}
	break;
      case TACACS_HOST:
	for (lp = (servtype *)tacacshostQ.qhead; lp; lp = next_lp) {
	    next_lp = lp->next; /* we're about to free this */
	    if (lp->addr == GETOBJ(paddr,1)->ip_addr) {
		/* 
		 * If adding an entry which already exists, delete the
		 * existing one first as its options may be changing. 
		 */

		/* Remove the entry and free the item. */
                reg_invoke_ip_close_socket(lp->sock);
                unqueue(&tacacshostQ, lp);
                login_have_servers = tacacshostQ.qhead ? TRUE : FALSE;

		if (lp->handle) {
		    reg_invoke_tacacs_plus_close_handle((void **) &lp->handle);
		}

		free(lp->in_data);
		lp->in_data = NULL;
		lp->allocated = 0;
		free(lp->key); /* checks for null */

		/*
		 * We may be blocking on a Tacacs server request.
		 * If so, they have mem_lock()'d "lp".  Set address
		 * field to 0 to mark this as freed.  The other
		 * process will clean up.
		 */
		lp->addr = (ipaddrtype) 0;
                free(lp);
		if (!csb->sense) {
		    /* if deleting, we're done */
		    return;
		}
            }
	}
	if (!csb->sense) {
	    return;              /* If not found and we are removing, bail */
	}
	/* create an entry and add it to the list */
	lp = malloc_named(sizeof(servtype), "TAC lp");
	if (lp) {
	    ipaddrtype source_ipaddr;
	    ushort sock_status_flags;

	    if (tacacs_source_idb && tacacs_source_idb->ip_address &&
		interface_up(tacacs_source_idb)) {
		source_ipaddr = tacacs_source_idb->ip_address;
		sock_status_flags = SOCK_ANYSRC | SOCK_SRC_SPECIFIED;
	    } else {
		source_ipaddr = (ipaddrtype) 0;
		sock_status_flags = SOCK_ANYSRC | SOCK_SRC_DYN;
	    }
	    faddr.type = laddr.type = ADDR_IP;
	    faddr.ip_addr = lp->addr = GETOBJ(paddr,1)->ip_addr;
	    laddr.ip_addr = source_ipaddr;
	    lp->sock = open_ipsocket(UDP_PROT, &laddr, &faddr, TACACS_PORT, 
				     TACACS_PORT, sock_status_flags);

 	    if (!old_access_control) {
 		if (GETOBJ(int,1) == TACACS_SINGLE_CONNECTION) {
 		    lp->single_connection = TRUE;
 		}
 		lp->timeout = GETOBJ(int,2);
		txt = GETOBJ(string,1);
 		if (txt[0] != '\0') {
 		    lp->key = strdup_named(GETOBJ(string,1), "TAC lp1");
		}
 		lp->port = GETOBJ(int,3);
 	    }
 
	    if (lp->sock != NULL) {
		enqueue(&tacacshostQ, lp);
		login_have_servers = tacacshostQ.qhead ? TRUE : FALSE;
	    } else {
		printf("\nCannot open TACACS connection for %i", lp->addr);
		free(lp);
	    }
	}
	break;
      case TACACS_LAST:
	if (csb->sense) {
	    tacacs_lastresort =GETOBJ(int,1);
	} else {
	    tacacs_lastresort = TACACS_LAST_FAIL;
	}
	break;
      case TACACS_RETRANSMIT:
	if (csb->sense) {
	    tacacs_retrans = GETOBJ(int,1);
	} else {
	    tacacs_retrans = TACACS_RETRANS;
	}
	break;
      case TACACS_TIMEOUTVALUE:
	if (csb->sense) {
	    tacacs_timeout = GETOBJ(int,1) * ONESEC;
	} else {
	    tacacs_timeout = TACACS_TIMEOUT;
	}
	break;
      case TACACS_EXTENDED:
	if (csb->sense) {
	    xtacacs_enable_ext();
	} else {
	    tacacs_extended = 0;        /* clear all bits */
	}
	break;
      case TACACS_ASK:
	xtacacs_setask(csb, GETOBJ(int,1), GETOBJ(int,2), GETOBJ(int,3));
	break;
      case TACACS_NOTE:
	xtacacs_setnote(csb, GETOBJ(int,1), GETOBJ(int,2));
	break;
      case TACACS_PASSWORDOPT:
	tacacs_pw_optional = csb->sense;
	break;
      case TACACS_DIRECTED_REQUEST:
	/* the 'restricted' flag gets set ONLY in the
	 *     tacacs-server directed-request restricted
	 * case. All other cases switch it off.
	 *
	 * The 'directed-request' flag is left unchanged in the
	 *     no tacacs-server directed-request restricted
	 * case
	 */
	  if (!GETOBJ(int,1)) {
	      tacacs_directed_request = csb->set_to_default || csb->sense;
	      tacacs_directed_restricted = FALSE;
	  } else { /* 'restricted' keyword set */
	      tacacs_directed_restricted = csb->sense;
	      tacacs_directed_request = tacacs_directed_restricted ?
		  TRUE : tacacs_directed_request;
	  }
	  break;
      case TACACS_SOURCE_INTERFACE:
	if (csb->sense) {
	    if (tacacs_source_idb != GETOBJ(idb,1)) {
		tacacs_source_idb = GETOBJ(idb,1);
		tacacs_adjust_source_interface(tacacs_source_idb);
	    }
	} else {   /*   csb->sense */
	    tacacs_source_idb = NULL;
	    tacacs_adjust_source_interface(tacacs_source_idb);
	}
	break;
      case TACACS_LOGIN_TIMEOUT:
        if (csb->sense) {
            login_input_timeout = GETOBJ(int,1);
        } else {
            login_input_timeout = LOGIN_DEF_TIMEOUT;
        }
        break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * tacacs_adjust_source_interface
 * re-synch our ip sockets to use the correct source address
 */
static void tacacs_adjust_source_interface (idbtype *swidb)
{

    servtype *lp;
    ipaddrtype source_ipaddr;
    ushort sock_status_flags;
    addrtype laddr;
    addrtype faddr;

    if (tacacs_debug) {
	buginf("\nTAC: Readjusting Tacacs sockets for Interface: %s",
	       (tacacs_source_idb) ?
	       tacacs_source_idb->namestring : "Any");
    }

    if (tacacs_source_idb && tacacs_source_idb->ip_address &&
	interface_up(tacacs_source_idb)) {
	source_ipaddr = tacacs_source_idb->ip_address;
	sock_status_flags = SOCK_ANYSRC | SOCK_SRC_SPECIFIED;
    } else {
	source_ipaddr = (ipaddrtype) 0;
	sock_status_flags = SOCK_ANYSRC | SOCK_SRC_DYN;
    }
    /*
     * we need to close all currently open tacacs's udp
     * sockets and re-open to enforce the possible change
     * of local address for packets.
     */
    for (lp = (servtype *)tacacshostQ.qhead; lp;) {
	servtype *next_lp;
	/*
	 * close the sockets and re-open for
	 * individual tacacs-server host entries
	 */
	if (tacacs_debug) {
	    buginf("\nTAC: Sock to %i will now use %i as"
		   " source", lp->addr, source_ipaddr);
	}
	reg_invoke_ip_close_socket(lp->sock);
	faddr.type = laddr.type = ADDR_IP;
	laddr.ip_addr = source_ipaddr;
 	faddr.ip_addr = lp->addr;	
	lp->sock = open_ipsocket(UDP_PROT, &laddr, &faddr, TACACS_PORT, 
				 TACACS_PORT, sock_status_flags);
	next_lp = lp->next;
	if (NULL == lp->sock) {
	    unqueue(&tacacshostQ, lp);
	    login_have_servers = tacacshostQ.qhead ? TRUE : FALSE;
	    if (tacacs_debug) {
		buginf("\nTAC: Cannot open TACACS connection"
		       " for %i", lp->addr);
	    }
	    free(lp);
	}
	lp = next_lp;
    }  /* for */
}

/*
 * tacacs_parse_server
 * Parse a server address, delimited by '@', validate it, and return
 * valid address or 0.  If valid address passed, terminate string with
 * '\0' where '@' was.
 */
ipaddrtype
tacacs_parse_server (char *user_str)
{
    servtype   *lp;
    ipaddrtype  addr;
    char       *tmp_str;
    char	*nameptr = NULL; 
    int		namelen;

    addr = 0;

    if (!(tacacs_directed_request && user_str)) {
	return(0);
    }

    for (tmp_str = user_str; *tmp_str; tmp_str++)
	if (*tmp_str == '@')
	    break;
    /* 
     * If he said <something>@<somewhere>, parse for server address,
     * check it against tacacs server list, and return it if valid.
     */
    if (*tmp_str) {  
	tmp_str++;
	namelen = strlen(tmp_str);
	nameptr = malloc_named(namelen + 1, "tacacs servername");
	if (!nameptr) {
	    /*
	     * XXX now what?
	     */
	    return(addr);
	} 
	sstrncpy(nameptr, tmp_str, namelen + 1);
	if (parse_hostname(nameptr, &addr)) {
	    if (addr) {
		lp = (servtype *)tacacshostQ.qhead;
		while (lp) {
		    if (lp->addr == addr) {
			/* 
			 * If found addr, it's valid.  Replace '@'
			 * with 0 to truncate string and break out.
			 */
			*--tmp_str = '\0';  
			break;
		    } else 
			lp = lp->next;
		}
		if (lp == NULL)
		    addr = 0;
	    } /* if (addr) */
	} /* parse_hostname() */
    } /* if (*tmp_str) */
    if (nameptr)
	free(nameptr);

    return(addr);
}

/*
 * tacacs_send_query
 * Send a TACACS query request given name and password strings
 */

boolean tacacs_send_query_addr (
    char *name,
    char *pw,
    tt_soc *tty,			/* for grabbing line data */
    boolean enabling,			/* to pass to daemon */
    char **reason,
    ipaddrtype addr)			/* addr of requested server */
{
    paktype *pak;
    servtype *lp;
    tacacstype *tac;
    uint namelen, pwlen, start, count;
    uchar *p;
    tacacsblock block;

    /*
     * If TACACS service not available, then claim okay.
     */
    if (!login_have_servers)
	return(FALSE);
    /*
     * If not hosts in queue then refuse access
     */
    lp = (servtype *)tacacshostQ.qhead;
    if (addr) {
        while (lp) {
	    if (lp->addr == addr)
	        break;
	    else
	        lp = lp->next;
	}
    }
    if (lp == NULL)
	return(FALSE);
    namelen = strlen(name);
    pwlen = strlen(pw);
    block.trans = random_gen() & 0xFFFF;
    count = 0;
    block.next = NULL;
    block.done = FALSE;
    block.response = TA_A_NONE;
    enqueue(&tacacsreqQ, &block);
    do {
	pak = setup_ipsocket(lp->sock, TACACS_SIZE+namelen+pwlen, &start);
	if (pak == NULL)
	    break;
	tac = (tacacstype *)(pak->data_area + start);
	tac->version = TA_VERSION;
	tac->type = TA_QUERY;
	tac->trans = block.trans;
	tac->namelen = namelen;
	tac->pwlen = pwlen;
	p = (uchar *)tac + TACACS_SIZE;
	bcopy(name, p, namelen);
	p += namelen;
	bcopy(pw, p, pwlen);
	block.done = FALSE;
	TIMER_START(block.timeout, tacacs_timeout);
	mem_lock(lp);
	write_ipsocket(lp->sock, pak, TACACS_SIZE+namelen+pwlen);
	edisms((blockproc *)tacacsBLOCK, (ulong)&block);
	if (addr) {
	    if (count++ >= tacacs_retrans) {
		free(lp);
	        break;
	    }
	} else {
	    if (lp->addr != (ipaddrtype) 0) {
		/* All free() is doing is decrementing the ref count. */
		free(lp);
		lp = lp->next;
	    } else {
		free(lp);
		lp = NULL;
	    }

	    if (lp == NULL) {
		if ((count++ >= tacacs_retrans) || (!login_have_servers)) {
		    break;
		} else {
		    lp = (servtype *)tacacshostQ.qhead;
		}
	    }
	}
    } while (!block.done);
    unqueue(&tacacsreqQ, &block);
    if (!block.done) 
	return(tacacs_fallback(enabling ?  XTA_ENABLE : XTA_LOGIN, reason));
    if (reason)
	switch (block.reason) {
	case TA_A_EXPIRING:
	    if (block.response == TA_A_ACCEPTED)
		*reason = "Account is expiring";
	    else
		*reason = "Account has expired";
	    break;
	case TA_A_PASSWORD:
	    if (block.response == TA_A_ACCEPTED)
		*reason = "Please change your TACACS password";
	    else
		*reason = "User/password invalid";
	    break;
	case TA_A_DENIED:
	    *reason = "Access denied";
	    break;
	default:
	    *reason = NULL;
	    break;
	}
    return(block.response == TA_A_ACCEPTED ? TRUE : FALSE);
}

boolean tacacs_send_query (
    char *name,
    char *pw,
    tt_soc *tty,			/* for grabbing line data */
    boolean enabling,			/* to pass to daemon */
    char **reason)
{
  return tacacs_send_query_addr(name, pw, tty, enabling, reason, 0);
}

/*
 * tacacs_fallback
 * if no tacacs server replies to our query, come here to do default action
 */
boolean tacacs_fallback (int which, char **reason)
{
    int whattodo;
    switch (which) {
      case XTA_LOGIN:
      case XTA_CONNECT:
      case XTA_SLIPADDR:
	whattodo = tacacs_lastresort;
	break;
      case XTA_ENABLE:
	whattodo = enable_lastresort;
	break;
      default:
	whattodo = TACACS_LAST_FAIL;
	break;
    }
    switch (whattodo) {
      case TACACS_LAST_SUCCEED:
	if (reason)
	    *reason = (char *)"No response from TACACS authentication server "
		"- permitted by default";
	return(TRUE);

      case TACACS_LAST_PASSWORD:
	if (reason)
	    *reason = NULL;
	printf("\nNo response from TACACS authentication server "
	       "- please enter enable password");
	if (enable_secret[PRIV_MAX].pw) {
	    if (ask_md5secret(stdio, enable_secret[PRIV_MAX].pw))
		return(TRUE);
	} else {
	    if (enable_password[PRIV_MAX].pw == NULL) {
		if (askpassword(stdio,
				console->password ?
				console->password->pw : NULL,
				console->password ?
				console->password->encrypt_type :
				ENCRYPT_NONE)) {
		    return(TRUE);
		}
	    } else {
		if (askpassword(stdio, enable_password[PRIV_MAX].pw,
				enable_password[PRIV_MAX].encrypt_type)) {
		    return(TRUE);
		}
	    }
	}
	printf("\n");
	return(FALSE);

      default:
	if (reason)
	    *reason = (char *)"No response from authentication TACACS server";
	return(FALSE);
    }
    return(FALSE);
}


/*
 * tacacsBLOCK
 * Wait for a TACACS response packet or timeout
 */

boolean tacacsBLOCK (tacacsblock *block)
{
    if (block->done)
	return(FALSE);			/* we finished */
    if (!SLEEPING(block->timeout))
	return(FALSE);			/* we timed out */
    return(TRUE);
}

/*
 * tacacs_reason
 * translate tty statbit into reason for disconnect
 */
int tacacs_reason (tt_soc *tty)
{
    if (tty->statbits & IDLETIMEOUT)
	return(XTA_R_IDLE);
    if (tty->statbits & (CARDROP|HANGUP))
	return(XTA_R_DROP);
    return(XTA_R_QUIT);
}

char *tacacs_whytext (xtacacstype *tac)
{

    if (tac) switch (tac->reason) {
      case TA_A_EXPIRING:
	if (tac->response == TA_A_ACCEPTED)
	    return("Account is expiring");
	else
	    return("Account has expired");
	break;
      case TA_A_PASSWORD:
	if (tac->response == TA_A_ACCEPTED)
	    return("Please change your TACACS password");
	else
	    return("User/password invalid");
	break;
      case TA_A_DENIED:
	return("Access denied");
	break;
      case TA_A_NOROUTE:
	return("Routing not allowed");
	break;
    case TA_A_LOGINREQ:
	return("Login required");
	break;
      default:
	return(NULL);
	break;
    } else return("Internal error");
}

/*
 * tacacs_login
 * Request a login via TACACS.
 * Called by SERVICE_LOGIN_LOGIN
 */

static boolean tacacs_login (tt_soc *tty, char *name, char *pass,
			     char **reason)
{
    ipaddrtype addr;
    boolean optional = tacacs_pw_optional;
    char *pw;
    char buff [TACPASSWDLEN];
    usernametype *info;

    /*
     * See if the user typed "user@somehost".
     */

    addr = tacacs_parse_server(name);

    /*
     * Check the local username first 
     */
    info = find_namedthing(&userQ, name);
    if (info) {
	if (info->flags & USER_NOPASSWORD)
	    return(TRUE);
	else {
	    if (passwdcheck(pass, info->password,
			    info->flags&USER_ENCRYPTION)) {
		if (tty) {
		    sstrncpy(tty->saved_passwd, pass, 
			     sizeof(tty->saved_passwd));
		    tty->saved_passwd[sizeof(tty->saved_passwd) - 1] = 0;
		}
		return(TRUE);
	    }
	}
	/* if there is info, there is a password, but we don't match it,
		then bail. */
	return(FALSE);
     }


    /*
     * This loop will execute once if passwords are not optional, 
     * possibly twice if they are optional and we get a negative
     * response to the first request with a null password.
     * If someone has supplied a password already (machine interface),
     * use that instead of prompting.
     */
    if (reason)
	*reason = NULL;
    for (;;) {
	if (optional && !pass)
	    pw = "";
	else {
	    if (pass)
		pw = pass;
	    else {
		pw = tacacs_password(tty, buff, TACPASSWDLEN);
		if (pw == NULL)
		    return(FALSE);
	    }
	}

	if (tacacs_extended == 0) {
	    if (tacacs_send_query_addr(name, pw, tty, FALSE, reason, addr)) {
		if (tty) {
		    if (addr)
			tty->tty_user_server = addr;
		    else
			tty->tty_user_server = 0;
		    sstrncpy(tty->saved_passwd, pw, sizeof(tty->saved_passwd));
		    tty->saved_passwd[sizeof(tty->saved_passwd) - 1] = 0;
		    setstring_named(&tty->user, name, "XTAC user1");
		}
		return(TRUE);
	    }
	} else {
	    if (xtacacs_login(tty ? tty->ttynum : -1, name, pw, reason, addr)){
		if (tty) {
		    if (addr)
			tty->tty_user_server = addr;
		    else
			tty->tty_user_server = 0;
		    sstrncpy(tty->saved_passwd, pw, sizeof(tty->saved_passwd));
		    tty->saved_passwd[sizeof(tty->saved_passwd) - 1] = 0;
		    setstring_named(&tty->user, name, "XTAC user2");
		}
		return(TRUE);
	    }
	}

	if (!optional)
	    return(FALSE);
	optional = FALSE;
    }
}

/*
 * tacacs_connect
 * Request permission to make a connection
 * Called by SERVICE_LOGIN_CONNECT
 */

static boolean tacacs_connect (ipaddrtype dhost, ushort dport)
{
    if ((tacacs_extended & (TACACS_ASKCONNECT|TACACS_NOTECONNECT)) == 0)
	return(TRUE);
    return(xtacacs_connect(dhost, dport));
}

/*
 * tacacs_enable
 * Request permission to enable
 * Called by SERVICE_LOGIN_ENABLE
 */

static boolean tacacs_enable (tt_soc *tty, char *name, char **reason,
			      uchar level)
{
    boolean optional = tacacs_pw_optional;
    char buff[TACPASSWDLEN];
    char *pw;

    /*
     * This loop will execute once if passwords are not optional, 
     * possibly twice if they are optional and we get a negative
     * response to the first request with a null password.
     */
    *reason = NULL;
    for (;;) {
	if (optional)
	    pw = "";
	else
	    if ((pw = tacacs_password(tty, buff, TACPASSWDLEN)) == NULL)
		return(FALSE);

	if (tacacs_extended == 0) {
	    if (tacacs_send_query(name, pw, stdio, TRUE, reason))
		return(TRUE);
	} else {
	    if (xtacacs_enable(name, pw, reason, level, TRUE))
		return(TRUE);
	}

	if (!optional)
	    return(FALSE);
	optional = FALSE;
    }
}

/*
 * tacacs_enable_pw
 * Request permission to enable with password
 * Called by SERVICE_LOGIN_ENABLE_PW
 */
static boolean tacacs_enable_pw (tt_soc *tty, char *name, char *pw,
				 char **reason, uchar level)
{
    if (tacacs_extended == 0) {
	return(tacacs_send_query(name, pw, stdio, TRUE, reason));
    } else {
	return(xtacacs_enable(name, pw, reason, level, TRUE));
    }
}

/*
 * tacacs_logout
 * Send notification of user logout
 * Called by REGISTRY_DEATH and REGISTRY_KFORK
 */

static void tacacs_logout (tt_soc *tty)
{
    if (tacacs_extended & TACACS_NOTELOGOUT)
	xtacacs_logout(tty);
}

/*
 * tacacs_slipon
 * Request permission to turn on slip
 * Called by SERVICE_LOGIN_SLIPON
 */

static boolean tacacs_slipon (char *name, char *pw, char **reason,
			      tt_soc *tty, ipaddrtype address, 
			      boolean routing, boolean from_exec_cmd)
{
    if ((tacacs_extended & (TACACS_ASKSLIP|TACACS_NOTESLIP)) == 0)
	return(TRUE);
    return(xtacacs_slipon(name, pw, tty, address, reason, routing, 
			  from_exec_cmd));
}

/*
 * tacacs_slipoff
 * Send notification of slip being turned off (eg hangup)
 * Called by SERVICE_LOGIN_SLIPOFF
 */

static void tacacs_slipoff (tt_soc *tty, ipaddrtype address, hwidbtype *idb,
		     boolean from_ipcp)
{
    if ((tacacs_extended & (TACACS_ASKSLIP|TACACS_NOTESLIP)) == 0)
	return;
    xtacacs_slipoff(tty, address, idb, from_ipcp);
}

/*
 * tacacs_slipaddr
 * Check whether a given user is allowed to dynamically set a particular
 * SLIP address.  This has to be different than slipon, since there is
 * more data to be provided.
 * Called by SERVICE_LOGIN_SLIPADDR
 */

static boolean tacacs_slipaddr (tt_soc *tty, char *hostname, char **reason,
			 ipaddrtype remaddr, boolean routing)
{
    ipaddrtype serveraddr;
    char *pw;
    boolean optional = tacacs_pw_optional;
    char buff[TACPASSWDLEN];

    /*
     * This loop will execute once if passwords are not optional, 
     * possibly twice if they are optional and we get a negative
     * response to the first request with a null password.
     */
    *reason = NULL;

    if (!login_have_servers)
	return(TRUE);		/* If no TACACS, claim success */

    for (;;) {
	if (optional)
	    pw = "";
	else
	    if ((pw = tacacs_password(tty, buff, TACPASSWDLEN)) == NULL)
		return(FALSE);

	serveraddr = tacacs_parse_server(hostname);

	if (tacacs_extended == 0) {
	    if (tacacs_send_query_addr(hostname, pw, stdio, FALSE, reason, 
				       serveraddr)) {
		if (serveraddr)
		    tty->tty_slip_server = serveraddr;
		return(TRUE);
	    }
	} else {
	    if (serveraddr) {
		tty->tty_slip_server = serveraddr;
	    }
	    if (xtacacs_slipaddr(tty, hostname, pw, remaddr, reason, routing)) {
		return(TRUE);
	    }
	}

	if (!optional)
	    return(FALSE);
	optional = FALSE;
    }
}

static void tacacs_idb_addr_changed (idbtype *swidb, ipaddrtype address,
				     ipaddrtype mask, boolean secondary,
				     boolean add)
{
    /*
     * the interface address changed. Whether deleted or added,
     * we need to re-adjust our ip sockets for tacacs server hosts.
     */
    if (swidb == tacacs_source_idb) {
	tacacs_adjust_source_interface(swidb);
    }
}

static void tacacs_idb_state_changed (idbtype *swidb)
{
    /*
     * the interface state changed. Whether gone up or down..
     * we need to re-adjust our ip sockets for tacacs server hosts.
     */
    if (swidb == tacacs_source_idb) {
	tacacs_adjust_source_interface(swidb);
    }
}

/*
 * tacacs_init
 * Initialize TACACS protocol
 */

static void tacacs_init (subsystype *dummy)
{
    tacacs_lastresort = TACACS_LAST_FAIL;
    tacacs_retrans = TACACS_RETRANS;
    tacacs_timeout = TACACS_TIMEOUT;
    tacacs_always = 0L;
    tacacs_directed_request = TRUE;
    tacacs_directed_restricted = FALSE;
    tacacs_source_idb = NULL;
    queue_init(&tacacshostQ, 0);
    queue_init(&tacacsreqQ, 0);
    queue_init(&tacacslogQ, 5000);

    /*
     * Register some functions.
     * Initialize tacacs parser routines
     */
    tacacs_parser_init();
    reg_add_ip_udp_input(tacacs_server, "tacacs_server");
    reg_add_onesec(tacacs_resend, "tacacs_resend");
    reg_add_login_connect(tacacs_connect, "tacacs_connect");
    reg_add_login_enable(tacacs_enable, "tacacs_enable");
    reg_add_login_enable_pw(tacacs_enable_pw, "tacacs_enable_pw");
    reg_add_login_login(tacacs_login, "tacacs_login");
    reg_add_death(tacacs_logout, "tacacs_logout");
    reg_add_kfork(tacacs_logout, "tacacs_logout");
    reg_add_login_logout(tacacs_logout, "tacacs_logout");
    reg_add_login_slipon(tacacs_slipon, "tacacs_slipon");
    reg_add_login_slipoff(tacacs_slipoff, "tacacs_slipoff");
    reg_add_login_slipaddr(tacacs_slipaddr, "tacacs_slipaddr");
    reg_add_login_enable_note(xtacacs_enable_notify, "xtacacs_enable_notify");
    reg_add_login_set_enable(xtacacs_set_enable, "xtacacs_set_enable");
    reg_add_login_authen(xtacacs_authen, "xtacacs_authen");
    reg_add_ip_address_change(tacacs_idb_addr_changed,
			      "tacacs_idb_addr_changed");
    reg_add_swif_goingdown(tacacs_idb_state_changed,
			   "tacacs_idb_state_changed");
    reg_add_swif_comingup(tacacs_idb_state_changed,
			  "tacacs_idb_state_changed");
}

/*
 * TACACS subsystem header
 */

#define TACACS_MAJVERSION 1
#define TACACS_MINVERSION 0
#define TACACS_EDITVERSION  1

SUBSYS_HEADER(tacacs, TACACS_MAJVERSION, TACACS_MINVERSION, TACACS_EDITVERSION,
	      tacacs_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
