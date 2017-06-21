/* $Id: rsh.c,v 3.8.8.8 1996/08/28 13:17:14 thille Exp $
 * $Source: /release/112/cvs/Xsys/tcp/rsh.c,v $
 *------------------------------------------------------------------
 * rsh.c -- Unix remote login service
 *
 * April 1994, Marciano Pitargue
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsh.c,v $
 * Revision 3.8.8.8  1996/08/28  13:17:14  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.8.7  1996/08/13  21:07:37  che
 * CSCdi65824:  kerberos client mandatory broken for rsh
 * Branch: California_branch
 * Make sure non-Kerberized clients fail if "kerberos clients mandatory"
 * is configured.
 *
 * Revision 3.8.8.6  1996/06/11  06:22:29  che
 * CSCdi58486:  router cant do kerberos mutual authentication with other
 * router
 * Branch: California_branch
 * Must add Kerberized RSHD registries regardless.
 *
 * Revision 3.8.8.5  1996/05/21  10:06:23  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.8.4  1996/05/04  02:07:47  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.8.8.3  1996/04/19  15:33:34  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.9  1996/04/16  03:57:01  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.1.6.8  1996/04/09  22:03:40  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.7  1996/03/30  03:20:12  carrel
 * cvs fixes
 *
 * Revision 2.1.6.6  1996/03/19  08:47:47  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.5  1996/03/05  04:44:34  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.1.6.4  1996/02/21  04:13:11  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.3  1996/01/23  06:50:13  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.2  1995/12/19  04:19:58  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.1  1995/11/17  18:35:06  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.8.8.2  1996/04/15  21:23:37  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.8.8.1  1996/03/18  22:13:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/16  07:40:51  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.2  1996/03/07  10:52:54  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  21:20:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/28  21:32:15  ahh
 * CSCdi50227:  RSH: rshell setup mistakenly done for alias or PT addresses
 * Don't set up the tty privileges when it's not a real rsh connection.
 *
 * Revision 3.7  1996/01/24  20:41:05  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.6  1996/01/16  03:37:55  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.5  1996/01/11  02:33:48  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.4  1995/12/28  18:29:08  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreign addresses
 *   to tcp_open() for future portability.
 *
 * Revision 3.3  1995/12/17  18:39:18  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:48:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/10  23:37:48  carrel
 * CSCdi40013:  User information needs to be present even when no login
 * occurs
 * aaa_create_user() now initializes the acc_method to be
 * AAA_ACC_METH_NONE.  So set it here to AAA_ACC_METH_RCMD.
 *
 * Revision 2.3  1995/06/20  07:19:28  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/17  06:26:26  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.1  1995/06/07  23:05:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "logger.h"
#include "../tcp/msg_rcmd.c"		/* Not a typo, see logger.h */
#include "ttysrv.h"
#include "config.h"
#include "connect.h"
#include "name.h"
#include "packet.h"
#include "../ui/service.h"
#include "../ip/ip_registry.h"
#include "../ip/ip.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "tcp.h"
#include "tcpinternal.h"
#include "../snmp/sr_old_ltsmibdefs.h"
#include "../ui/command.h"
#include "aaa.h"
#include "../parser/parser_actions.h"
#include "rcp.h"
#include "rsh.h"
#include "../h/auth_registry.regh"

struct rshcontext_ {
    rhost_entry *entry;
    tcbtype *second_tcb;
    boolean done;
};

static boolean rsh_ioBLOCK(tcbtype *tcb);

/*
 * Set up special capabilities for rsh server functionality
 */

static void rsh_capabilities_init (tt_soc *tty, void *rsh_info)
{
    userstruct *ustruct;
    char port[8], rem_addr[32];
    rshcontext *info;

    info = tty->rshcontext = (rshcontext *) rsh_info;
    if (info == NULL) {
	/*
	 * Here by accident, apparently.
	 *
	 * Someone probably attempted an rsh connection to an IP
	 * address that was aliased due to protocol translation.
	 */
	return;
    }

    SET_PRIV(tty->privilege, info->entry->priv_level);

    tty->capabilities |= NOEXECBANNER;

    if (old_access_control) {
	setstring(&tty->user, info->entry->local_user);
    } else {
	/* This was already authenticated in order for us to get here. */
	if (tty->ustruct) {
	    aaa_free_user(&tty->ustruct);
	    tty->ustruct = NULL;
	}
	sprintf(port, "tty%d", tty->ttynum);
/* DAVE -- should I use this rem_addr or call tty_get_remote_addr() */
	sprintf(rem_addr, "%i", info->entry->addr.ip_addr);
	if (aaa_create_user(&ustruct,
			    info->entry->remote_user,
			    info->entry->local_user,
			    port, rem_addr, AAA_AUTHEN_TYPE_ASCII,
			    AAA_SVC_RCMD,
			    info->entry->priv_level)) {
	    ustruct->acc_method = AAA_ACC_METH_RCMD;
	    tty->ustruct = ustruct;
	}
    }
}

static void
rsh_cleanup (tt_soc *tty)
{
    /*
     * If tcp connections exists, do special things before killing
     * the connection off.  e.g., rsh needs a <cr> before dying...
     */
    if (tty && tty->rshcontext) {
        putc(tty, '\n');
	/*
	 * Kill off stderr before the primary connection gets clobbered.
	 * BSD rsh doesn't appear to send FINs in response to our FINs
	 * until BOTH connections have been FINned by us.
	 * Since tcp_close is synchronous, close_connection() will
	 * attempt to close the primary connection and will time out
	 * waiting for the FIN to arrive.  This translates into the
	 * remote user seeing rsh hang for 5 seconds after the last
	 * bit of output shows up.  If we close the secondary
	 * connection now, the close in close_connection() will not
	 * hang and the remote rsh will exit promptly.
	 * If tcp_close() ever becomes asynchronous, this silliness
	 * won't be needed.
	 */
	if (tty->rshcontext->second_tcb)
	    tcp_close(tty->rshcontext->second_tcb);
	tty->rshcontext->done = TRUE;
	tty->rshcontext = NULL;
	SET_PRIV(tty->privilege, DEFAULT_PRIV);	/* just in case */
    }
}

static
boolean rsh_serverBLOCK (rshcontext *info)
{
    /*
     * Wake up after death() calls rsh_cleanup()
     * via the reg_invoke_death() registry.
     */
    return(!info->done);
}

/*
 * rsh_command - create a tty and send command to the exec process
 *
 * Returns TRUE if a new process was spawned which will do the
 *    command and clean up the passed-in TCB, FALSE otherwise.
 */
void rsh_command (char *command, tcbtype *tcb, tcbtype *second_tcb,
		  rhost_entry *entry)
{
    int line;
    int tcp_session;
    char *error;
    rshcontext info;

    line = get_vtyline(tcb->localport, &tcp_connvector);

    if (line <= 0) {
	rcmd_error(tcb, "No free VTYs.\n");
	if (second_tcb)
	    tcp_close(second_tcb);
	tcp_close(tcb);
	return;
    }

    info.entry = entry;
    info.second_tcb = second_tcb;
    info.done = FALSE;
    tcp_session = tcp_do_command(line, tcb, command, FALSE, TRUE, &info, TRUE);

    switch (tcp_session) {
    case TCP_SUCCESS:
	/*
	 * Block until exec calls death on the tty.
	 */
	edisms((blockproc *) rsh_serverBLOCK, (ulong) &info);
	return;

    case TCP_NO_TTY:
	error = "No free VTYs.\n";
	break;
    case TCP_ACCESS_DENIED:
	error = "Access denied.\n";
	break;
    case TCP_NO_RESOURCES:
	error = "No resources for rsh.\n";
	break;
    default:
	error = "Unknown error.\n";
	break;
    }

    rcmd_error(tcb, error);
    if (second_tcb)
	tcp_close(second_tcb);
    tcp_close(tcb);
}

/*
 * rsh_ioBLOCK - this is the same as tcpmiscBLOCK but w/ the extra
 * added case of scanning for input.  this is required for output
 * that require the space char for the more output coming from the
 * the server side.
 */
boolean rsh_ioBLOCK (tcbtype *tcb)
{
    /*
     * Connection still there
     */
    if (tcp_activity(tcb))
	return(FALSE);

    if ((psiset(tcb->tty)) || (modemcheck(tcb->tty)))
	return(FALSE);

    if (automore_quit())
	return(FALSE);

    if (inputpending(stdio))
	return(FALSE);

    return(TRUE);
}

/*
 * rsh_connect_command - Handle the connect command for rsh
 *
 * This routine handles the connect command for the rsh protocol.
 */

void rsh_connect_command (parseinfo *csb)
{
    nametype *nameptr;
    tcbtype *tcb;
    addrtype address;
    char *remote_cmd;
    char *remote_username;
    int dummy, i;
    int datacount, status;
    uchar *dataptr;
    int max_chars = 0;
    char *ptr;
    boolean sent = FALSE;
    boolean use_kerb = FALSE;
    int error;

    /*
     * First see if we want to use Kerberos
     */
    if (reg_invoke_kerberos_is_running() && reg_invoke_kerberos_stackcheck()
        && stdio && stdio->ustruct && stdio->ustruct->user 
	&& stdio->ustruct->cred
        && (stdio->ustruct->cred->cred_type == KERBEROS)) {
        use_kerb = TRUE;
    }

    memset(&address, 0, sizeof(addrtype));
    tcb = NULL;
    remote_username = NULL;

    if (GETOBJ(int, 1))
        remote_username = GETOBJ(string, 2);
    else {
	/*
	 * use rcmd_get_local_username instead of rcmd_get_remote_username
	 * because we don't want the option of using remote_username
	 * because that is only reserved for rcp...
	 */
        remote_username = rcmd_get_local_username();
    }

    psion(stdio);

    /*
     * Attempt to look up the address in dotted format
     */
    if (numeric_lookup(ADDR_IP, GETOBJ(string, 1), &address)) {
	/* Did it look like an IP address? */
	if (address.ip_addr && martian(address.ip_addr, TRUE)) {
	    /* Yes, is it valid? */
	    printf("%% Invalid IP address");
	    psioff(stdio);
	    return;
	}
	nameptr = NULL;
    } else {

	/*
	 * Attempt to look up the address in the name cache
	 */
	nameptr = name_lookup(GETOBJ(string, 1), &dummy, NAM_IP);
	if (!nameptr) {
	    if (!psipending()) {
		printf(connect_unknown);
	    }
	    psioff(stdio);
	    return;
	}
    }

    psioff(stdio);

    remote_cmd = malloc(MAXCMDLEN);
    if (remote_cmd == NULL) {
	return;
    }

    sprintf(remote_cmd, "%s", GETOBJ(string, 3));

    if (nameptr == NULL) {
        /*
         * Perform access checking on the current address.
         */
        if (!check_access(stdio, address.ip_addr, 0, 
			  (use_kerb)? KSHELL_PORT:RSHELL_PORT, TRUE)) {
	    printf("\n%% Connections to that host not permitted from "
		   "this terminal");
        } else
	    tcb = rcmd(stdio, &address, rcmd_get_local_username(),
		       remote_username, remote_cmd, NULL, TRUE, &error);
    } else {
	name_valid(nameptr, TRUE);	/* Say cache entry is good */
	nameptr->flags |= NAM_RLOGIN;	/* rsh did work */

	for (i = 0; i < MAXIPADDRESSES; i++) {
            if (!nameptr->v.ip.adr[i].addr.ip_address)
                break;
	    ip_copy_address(nameptr->v.ip.adr[i].ip_addr, &address);
	    /*
	     * Perform access checking on the current address.
	     */
	    if (!check_access(stdio, address.ip_addr, 0, 
			      (use_kerb) ? KSHELL_PORT:RSHELL_PORT, TRUE)) {
		printf("\n%% Connections to that host not permitted from "
		       "this terminal");
		break;
	    }

	    /*
	     * Try making the connection.  Check for martians.
	     */
	    if (martian(address.ip_addr, TRUE)) {
		printf("%%invalid host address");
		break;
	    }

	    tcb = rcmd(stdio, &address, rcmd_get_local_username(),
		       remote_username, remote_cmd, NULL, TRUE, &error);

	    if (tcb)
		break;
	}
    }

    psion(stdio);

    if (tcb) {
        datacount = tcp_getbyteblock(tcb, &status);

	if (status == OK) {
	    printc((char)datacount);

	    automore_enable(NULL);

	    do {
		edisms((blockproc *)rsh_ioBLOCK, (ulong) tcb);

		if (psipending())
		    break;

		if (automore_quit())
		    break;

		if ((!tcp_unread(tcb)) && (tcb->state != ESTAB))
		    break;

		status = tcp_getstring(tcb, &dataptr, &datacount,
				   tcp_chunksize ? tcp_chunksize : MAXINT);
		switch (status) {
		  case URGENT_DATA:
		  case OK:
		    while (datacount--) {
			/*
			 * don't print out CR's.  when lines come over
			 * from the other side (from a router) don't
			 * print out the CR's.  it messes up the -- More --
			 * prompt
			 */
			if (*dataptr ==CR) {
			    dataptr++;
			    continue;
			}
			printc(*dataptr++);
		    }
		    flush();
		    break;
		  case NO_DATA:
		    break;
		  default:
		    status = END_OF_FILE;
		    break;
		}

		/*
		 * Now handle input stdio (output to the net)
		 * if needed.
		 */

		if (inputpending(stdio)) {
		    max_chars = MAXCMDLEN;
		    *remote_cmd = '\0';       /* clear out remote_cmd */
		    ptr = remote_cmd;

		    while (--max_chars) {
			*ptr = getc(stdio);   /* get byte from stdio */
			if (*ptr == -1)     /* break if no stdio data */
			    break;

			/*
			 * don't use rcmd_put_string, since it appends a NULL
			 */
			sent = tcp_putbyte(tcb, *ptr, TRUE);
			if (sent == FALSE) {
			    ungetc(stdio, *ptr);
			    break;
			}
			ptr++;
		    }
		    (void)tcp_putpush(tcb, TRUE, FALSE);
		}

	    } while (status != END_OF_FILE);

	    automore_disable();
	}

        tcp_close(tcb);
    }
    free(remote_cmd);
    psioff(stdio);
}

/*
 * tcp_access_check_rshell
 * Return true if both incoming rcp and rsh are enabled
 */
static boolean
tcp_access_check_rcmd (ulong sport, ulong dport, ipaddrtype fhost)
{
    if (rcp_enable || rsh_enable)
	return(TRUE);

    errmsg(&msgsym(RSHPORTATTEMPT, RCMD), fhost);
    return(FALSE);
}

/*
 * tcp_access_check_kshell
 * Return true if both incoming kcp and ksh are enabled
 */
static boolean
tcp_access_check_kcmd (ulong sport, ulong dport, ipaddrtype fhost)
{
    if ((rcp_enable || rsh_enable) && reg_invoke_kerberos_is_running())
	return(TRUE);

    errmsg(&msgsym(RSHPORTATTEMPT, RCMD), fhost);
    return(FALSE);
}

/*
 * tcp_listen_rshell_port
 * Performs processing for a rshell port request
 */

static void
tcp_listen_rshell_port (tcbtype *tcb)
{
    kerberos_tcbtype *kerb_tcb;

    kerb_tcb = malloc(sizeof(kerberos_tcbtype));
    if (kerb_tcb == NULL) {
	return;
    }

    kerb_tcb->tcb = tcb;
    kerb_tcb->use_kerb = FALSE;

    (void) cfork((forkproc *)rshd_server, (long)kerb_tcb, 0, 
		 "TCP Remote Shell", 0);
}

/*
 * tcp_listen_kshell_port
 * Performs processing for a kshell port request
 */

static void
tcp_listen_kshell_port (tcbtype *tcb)
{
    kerberos_tcbtype *kerb_tcb;

    kerb_tcb = malloc(sizeof(kerberos_tcbtype));
    if (kerb_tcb == NULL) {
	return;
    }

    kerb_tcb->tcb = tcb;
    kerb_tcb->use_kerb = TRUE;

    (void) cfork((forkproc *)rshd_server, (long)kerb_tcb, 1200, 
		 "TCP Kerberos Remote Shell", 0);
}

void
rsh_init (void)
{
    rsh_enable = FALSE;
    reg_add_tcp_access_check(RSHELL_PORT, tcp_access_check_rcmd, 
			     "tcp_access_check_rcmd");
    reg_add_tcp_listen(RSHELL_PORT, tcp_listen_rshell_port,
		       "tcp_listen_rshell_port");
    reg_add_tcp_session_init(RSHELL_PORT, rsh_capabilities_init,
			     "rsh_capabilities_init");
    reg_add_tcp_access_check(KSHELL_PORT, tcp_access_check_kcmd,
			     "tcp_access_check_kcmd");
    reg_add_tcp_listen(KSHELL_PORT, tcp_listen_kshell_port,
		       "tcp_listen_kshell_port");
    reg_add_tcp_session_init(KSHELL_PORT, rsh_capabilities_init,
			     "ksh_capabilities_init");
    reg_add_death(rsh_cleanup, "rsh_cleanup");
}
