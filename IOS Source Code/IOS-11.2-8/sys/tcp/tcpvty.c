/* $Id: tcpvty.c,v 3.11.4.7 1996/09/05 01:18:09 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/tcp/tcpvty.c,v $
 *------------------------------------------------------------------
 * tcpvty.c -- TCP virtual terminals
 *
 * 3-January-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcpvty.c,v $
 * Revision 3.11.4.7  1996/09/05  01:18:09  sjackson
 * CSCdi67473:  big buffers created not trimmed (queued on TCP input with
 * rtelnet)
 * Enable coalescing of TCP input packets by default for reverse telnet
 * Branch: California_branch
 *
 * Revision 3.11.4.6  1996/05/04  02:07:53  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.11.4.5  1996/04/30  20:50:11  jenny
 * CSCdi53692:  telnet gets stuck in CLOSEWAIT
 * Branch: California_branch
 * tty_daemon() was incorrectly checking the output vector for blocking
 * conditions while it's trying to write to the input vector.
 *
 * Revision 3.11.4.4  1996/04/24  01:31:29  widmer
 * CSCdi55505:  aborted http parser processes hog cpu
 * Branch: California_branch
 * Set CARDROP on tty if tcb->state is closed and turn on
 *   tty->automore_flush to stop http parser output
 *
 * Revision 3.11.4.3  1996/04/19  15:33:43  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.9  1996/04/16  03:57:06  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.1.6.8  1996/04/09  22:04:06  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.7  1996/03/30  03:19:43  carrel
 * cvs fixes
 *
 * Revision 2.1.6.6  1996/03/28  09:13:21  che
 * Branch: IosSec_branch
 * Need to initialize kerberos fields in conn_telnet
 *
 * Revision 2.1.6.5  1996/03/19  08:48:13  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.4  1996/03/05  04:44:42  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.1.6.3  1996/02/21  04:13:47  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.2  1996/01/23  06:51:06  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.1  1995/11/17  18:36:02  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 * Revision 3.11.4.2  1996/04/15  21:24:03  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.11.4.1  1996/03/18  22:13:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  07:41:25  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/07  10:53:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  21:21:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/07  12:10:03  billw
 * CSCdi46565:  <uses> in  <show line> doesnt include PPP, Xremote, etc
 *
 * Revision 3.10  1996/03/05  08:52:40  billw
 * CSCdi49662:  Random crashes tacacs+ accounting may be implicated.
 * Increase stack size of tty_daemon and protocol translation processes.
 *
 * Revision 3.9  1996/02/11  09:21:06  billw
 * CSCdi47099:  not nearly enough processes call process_set_crashblock
 *
 * Revision 3.8  1996/02/09  02:30:27  sjackson
 * CSCdi42583:  Dispatch-character doesnt work for reverse telnet
 *
 * Revision 3.7  1996/01/29  11:39:08  billw
 * CSCdi46038:  vty access-class takes precedence over translate
 * access-class
 * Always use SYNC_LISTEN for incoming PT tcp sessions, and let the PT
 * process itself do the access check.
 *
 * Revision 3.6  1996/01/24  20:41:31  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.5  1996/01/22  19:11:42  jenny
 * CSCdi46967:  crash in tcp_putsetup
 * Make sure checking for blocking condition in both ESTAB and CLOSEWAIT
 * states. Allow writing to TCP in CLOSEWAIT state as well.
 *
 * Revision 3.4  1996/01/16  03:38:06  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.3  1995/12/28  18:29:34  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreight addresses
 *   to tcp_open() for feature portability.
 *
 * Revision 3.2  1995/11/17  18:49:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:36:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:32:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/11  21:43:06  tkolar
 * CSCdi40022:  Telnet speed is retained even after the session is closed
 * Move passspeed and maxpassspeed into the userdata struct.
 *
 * Revision 2.6  1995/07/12  23:45:53  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.5  1995/06/20 03:45:01  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.4  1995/06/17  06:26:51  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.3  1995/06/16  04:29:56  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.2  1995/06/13  16:38:33  syiu
 * CSCdi35685:  Async callback rotary should be in reverse oder
 *
 * Revision 2.1  1995/06/07  23:06:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "logger.h"
#include "../pt/pt_registry.h"
#include "packet.h"
#include "ttysrv.h"
#include "../os/msg_sched.c"            /* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "tcp.h"
#include "tcpinternal.h"
#include "tcp_debug.h"
#include "connect.h"
#include "config.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "telnet.h"
#include "address.h"
#include "name.h"
#include "../ts/ttystatem.h"
#include "../snmp/sr_old_ltsmibdefs.h"
#include "../pt/translate.h"
#include "../xremote/xremote.h"
#include "../ip/ip_registry.h"
#include "../os/async.h"
#include "../ui/service.h"
#include "rcp.h"
#include "rsh.h"
#include "../os/free.h"
#include "../h/auth_registry.regh"
#include "stacks.h"

#define WAITFACTOR 100			/* wait this long for more data */

/*
 * local storage
 */
vectortype TCPvector;
connvector tcp_connvector;
ulong tcpservers_max;
ulong tcpservers_current;


/*
 * tcp_getline
 * Given a port number, return a terminal line or 0 if can't.
 * Return -linenumber, if the only reason the line can't be gotten
 * is because it is busy.
 */

static
int tcp_getline (ulong port, boolean *telnetflag, boolean *binaryflag,
		 connvector *connv)
{
    int i, j;
 
    *telnetflag = TRUE;                 /* assume telnet protocols */
    *binaryflag = FALSE;                /* assume no binary mode */

    /*
     * Note: Everyone how calls tcp_getline with port set to TELNET_PORT
     * should call get_vtyline() in os/ttysrv.c.
     * First, looking for a free vty should be provided in the generic TTY 
     * support and not TCP.
     * Second, TCP will not be required for the PT to get a VTY.
     */


    /*
     * If port is the Telnet protocol port, return next free vty.
     * This is for backwards compatibility for older patches
     * Tunnels are like incoming telnet, but will decide later not
     * to run any form of telnet protocol on the line.
     */
    if (port == TELNET_PORT)
	return(get_vtyline(port, connv));

    if (tcptunnel_enable && port == TUNNEL_PORT) {
	*telnetflag = FALSE;
	return(get_vtyline(port, connv));
    }

    /*
     * Telnet protocols for a single line.
     */
    if ((port > LINEBASE) && (port < LINEBASE+PORTINTERVAL))
        return(select_line(port-LINEBASE, connv));

    /*
     * Telnet binary protocols for a single line.
     */
    if ((port > BINARYLINEBASE) && (port < BINARYLINEBASE+PORTINTERVAL)) {
        *binaryflag = TRUE;
        return(select_line(port-BINARYLINEBASE, connv));
    }

    /*
     * Raw TCP for a single line
     */
    if ((port > STREAMLINEBASE) && (port < STREAMLINEBASE+PORTINTERVAL)) {
        *telnetflag = FALSE;
        return(select_line(port-STREAMLINEBASE, connv));
    }

    /*
     * Telnet protocols for a rotary group.
     */
    if ((port > ROTARYBASE) && (port < ROTARYBASE+PORTINTERVAL))
        return(select_rotary(port - ROTARYBASE, connv, NULL,
			     ROTARY_ASCENDING));

    /*
     * Telnet binary protocols for a rotary group.
     */
    if ((port > BINARYROTARYBASE) && (port < BINARYROTARYBASE+PORTINTERVAL)) {
        *binaryflag = TRUE;
        return(select_rotary(port - BINARYROTARYBASE, connv, NULL,
			     ROTARY_ASCENDING));
    }

    /*
     * Raw TCP for a rotary group.
     */
    if ((port > STREAMROTARYBASE) && (port < STREAMROTARYBASE+PORTINTERVAL)) {
        *telnetflag = FALSE;
        return(select_rotary(port - STREAMROTARYBASE, connv, NULL,
			     ROTARY_ASCENDING));
    }
 
    /*
     * Reverse XRemote Telnet to a single line
     */
 
    if ((port > REVERSE_XREMOTE_LINE_BASE) &&
        (port < REVERSE_XREMOTE_LINE_BASE + PORTINTERVAL)) {
        *telnetflag = TRUE;
        return(select_line(port - REVERSE_XREMOTE_LINE_BASE, connv));
    }

    /*
     * Reverse XRemote Telnet for a rotary group
     */
 
    if ((port > REVERSE_XREMOTE_ROTARY_BASE) &&
        (port < REVERSE_XREMOTE_ROTARY_BASE + PORTINTERVAL)) {
        *telnetflag = TRUE;
        return(select_rotary(port - REVERSE_XREMOTE_ROTARY_BASE, connv, NULL,
			     ROTARY_ASCENDING));
    }

    /*
     * Check for TCP to PAD translation service
     */
    if ((port & ALIAS_FLAGMASK) == TCPPAD_ALIASPORT) {
        j = VTYBase + nvtylines;
        for (i = VTYBase; i < j; i++)
            if (!(MODEMS[i]->statbits & HASPROC))
                return(i);
    }

    /*
     * We don't know about this TCP port.  Return illegal value.
     */
    return(0);
}


/*
 * tcp_process_message
 * The newly accepted connection is either hung, timed out, or reset by
 * peer. 
 */
static void
tcp_process_message (void)
{
    ulong message, value;
    void *pointer;
    int line;
    boolean telnetflag;                 /* TRUE if telnet protocol */
    boolean binaryflag;                 /* TRUE if telnet binary mode */
    char *command;
    ulong lport;
    tcbtype *tcb;
 
    while (process_get_message(&message, &pointer, &value)) {
	switch (message) {
	  case TCP_PASSTCB_MSG: {
	    tcb = (tcbtype *)pointer;
	    tcb->msg_refcnt--; 
	    /*
	     * This message was queued before we had a chance to clear
	     * the pid and TCB_INT_WAIT_ESTAB flag. Don't do anything
	     * more for this TCB in this process context.
	     */ 
	    if (!(tcb->internal_flags & TCB_INT_WAIT_ESTAB))
		break;

	    /*
	     * This TCB is connected, or the application (probably PT) 
	     * is going to finish opening it, decrement tcp_listenQ_count
	     * so that another new connection can be accepted.
	     */
	    if (tcb->state == ESTAB || tcb->state == CLOSEWAIT ||
		tcb->flags & TCB_SYNCLISTEN) {
		if (tcp_listenQ_count > 0)
		    tcp_listenQ_count--;
		/*
		 * Clear the pid and TCB_INT_WAIT_ESTAB in the internal_flags
		 * for this TCB as we are transfering control of this TCB to
		 * the applications.
		 */
		tcb->pid = 0;
		tcb->internal_flags &= ~TCB_INT_WAIT_ESTAB;
		command = NULL;

		lport = reg_invoke_ipalias_port(tcb->localhost.ip_addr,
						tcb->localport);
		line = tcp_getline(lport, &telnetflag, &binaryflag,
				   &tcp_connvector);
		if (line <= 0) {
		    tcb->tty = NULL;
		    reg_invoke_tcp_listen(tcb->localport, tcb);
		} else {
		    tcp_do_command(line, tcb, command, telnetflag, binaryflag,
				   NULL, FALSE);
		}
	    } else if (tcb->state == FINWAIT1 || tcb->state == CLOSED) {
		/*
		 * Connection is closed. Clean up the mess. Don't forget
		 * to decrement the tcp_listenQ_count now that this new
		 * connection is aborted.
		 */
		if (tcp_listenQ_count > 0)
		     tcp_listenQ_count--;
		tcb->pid = 0;
		if (tcb->msg_refcnt == 0) {
		    /*
		     * The tcb may be freed after coming back from
		     * tcp_close(), call it only if there is no more
		     * message queued for this tcb.
		     */ 
		    if (tcp_debug) {
			buginf("\ntcp0: T %s %s:%d %s:%d early close",
				tcp_statestring(tcb->state),
				address_string(&tcb->foreignhost),
				tcb->foreignport,
				address_string(&tcb->localhost),
				tcb->localport);
		    }
		    tcb->internal_flags &= ~TCB_INT_WAIT_ESTAB;
		    tcp_close(tcb);
		}
	    }
	    break;
	  }
	  default:
	    errmsg(&msgsym(UNEXPECTEDMESSAGE,SCHED), message, pointer, value);
	    break;
	}
    }
}
 

/*
 * tcp_protocols
 * Listen for incoming TCP, telnet and rotary port requests
 */

process tcp_protocols (void)
{
    ulong major, minor;

    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case MESSAGE_EVENT:
		tcp_process_message();
		break;
	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}


/*
 * tcptranslate_cfork
 * Create an approriate type of process for performing protocol translation
 * on an incoming tcp connection.  Create special processes if they exist,
 * otherwise create a normal exec with the appropriate "autocommand".
 */

static int tcptranslate_cfork (tt_soc *tty, translatetype *trans)
{
    trans->totaluses++;
    if (trans->maxallowed && (trans->active >= trans->maxallowed)) {
	trans->failures++;
	return(-1);
    }
    return(reg_invoke_pt_tcpinput(OUTPROT(trans->type), trans, tty));
}

/*
 * start_tcpvty
 * Create a tiptop for a TCP virtual terminal.
 * Returns FALSE if couldn't create tiptop, TRUE otherwise.
 */

static
boolean start_tcpvty (
    tt_soc *tty,
    tcbtype *tcb,
    boolean telnetflag,			/* TRUE if telnet protocol used */
    boolean binaryflag)			/* TRUE if telnet binary mode */
{
    conntype *conn;		/* connection pointer */
    ushort port;
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    conn = getconnection(tty, &tcp_connvector);
    if (conn == NULL) {
	return(FALSE);
    }
    conn->proto.tn.statechange_check = telnetserver_optioncheck;
    conn->proto.tn.do_kerb_auth = FALSE;
    conn->proto.tn.kerberos_info = NULL;
    conn->tcb = tcb;				/* point to TCB */
    tcb->tty = tty;				/* point back to tty */
    tcb->ttynum = tty->ttynum;			/* set line number */
    conn->proto.tn.hostaddr = tcb->foreignhost;	/* remember host address */
    address_copy(&conn->address, &tcb->foreignhost);
    if (tcp_keepalivesin) {
	tcb->flags |= TCB_KEEPALIVE;
    }
    unqueue(&tty->connhead, conn);	/* remove from general q */
    tty->conntop = conn;		/* remember the driver conn */
    tty->statbits &= ~(CARDROP+HANGUP+IDLE);	/* reincarnated again! */
    tty->statbits |= READY;
    tty->usagecount++;				/* another use */
    tty->ivector = &TCPvector;			/* this is a TCP vty */

    if (tty->tty_passspeed) {
        setspeed (tty, tty->tty_passspeed, tty->tty_passspeed);
    }

    /*
     * $$$-dino, need to deal with this.
     */
    if ((reg_invoke_ipalias_port(tcb->localhost.ip_addr, tcb->localport) &
	ALIAS_FLAGMASK) == TCPPAD_ALIASPORT) {
	translatetype *trans;

	trans = tcp_find_translation(tcb->localhost.ip_addr, tcb->localport);
	if (trans)
	    tty->tiptop = tcptranslate_cfork(tty, trans);
	else
	    tty->tiptop = -1;
    } else {
	if (telnetflag) {
	    telnet_defaults(conn,binaryflag);	/* start telnet negotiations*/
	} else {
	    conn->status |= CONN_STREAM; 	/* mark a stream connection */
	    conn->snmp_sesstype = D_tslineSesType_stream; /* Post to SNMP */
	}
	port = reg_invoke_ipalias_port(tcb->localhost.ip_addr, tcb->localport);

	if ((port >= REVERSE_XREMOTE_LINE_BASE) &&
	    (port < REVERSE_XREMOTE_ROTARY_BASE + PORTINTERVAL)) {
	    tty->tiptop = -1;
	    if (!reg_invoke_xremote_create_tty_proc(tty)) {
		tty->tiptop = -1;
	    }
	} else {
	    switch (tty->type) {
	      case VTY_LINE:
   	        tty_create_vty_exec(tty);
		break;
	      case AUX_LINE:
	      case TTY_LINE:
		if (reg_invoke_kerberos_is_running())
		    stacksize = (ulong) EXEC_STACK_SIZE;
		tty->tiptop = cfork((forkproc *)tty_daemon, (long)tty, 
				    stacksize, "TTY Daemon",tty->ttynum);
		break;
	      default:
		tty->tiptop = -1;
		break;
	    }
	}
    }
    if (tty->tiptop < 0) {
	freeconnection(conn);
	return(FALSE);
    }
    process_set_crashblock(tty->tiptop, TRUE);
    return(TRUE);
}

/*
 * tcp_do_command
 * Performs a command function
 */
int tcp_do_command (int line, tcbtype *tcb, char *command,
		    boolean telnetflag, boolean binaryflag,
		    void *session_info, boolean savetcb)
{
    tt_soc *tty;

    tty = tty_allocate(line,TRUE);
    if (tty == NULL) {
	if (!savetcb)
	    tcp_close(tcb);
	return(TCP_NO_TTY);		/* No free TVT's or port in use */
    }

    if ((tcb->flags & TCB_SYNCLISTEN) == 0) {
	/*
	 * connections doing a sync listen are responsible for doing
	 * their own access check.
	 */
	if (check_access(tty, tcb->foreignhost.ip_addr, tcb->foreignport,
			 tcb->localport,FALSE) == FALSE) {
	    tty_deallocate(tty);
	    if (!savetcb)
		tcp_close(tcb);
	    return(TCP_ACCESS_DENIED);	/* Access denied to that host */
	}
    }

    tty->tty_autocommand = command;
    if (command) {
	/*
	 * We don't want to pause at end of page for any commands
	 * initialed by tcp "services", so set tty length to 0.
	 * Meanwhile, configured autocommands will use the default length
	 */
	tty_savedefaults(tty);
	tty->tty_length = 0;
    }

    /*
     * leave this here, since for rsh, we need to save the defaults
     * of the tty before mucking w/ the rest of it...
     */
    reg_invoke_tcp_session_init(tcb->localport, tty, session_info);

    if (!start_tcpvty(tty, tcb, telnetflag, binaryflag)) {
	tty_deallocate(tty);
	if (!savetcb)
	    tcp_close(tcb);
	return(TCP_NO_RESOURCES);	/* process creation failure */
    }

    return(TCP_SUCCESS);
}

/*
 * tcp_listen_discard_port
 * Starts a new task for a discard port request
 */
void tcp_listen_discard_port (tcbtype *tcb)
{
    tcp_set_rcvwnd(tcb, 4 * tcb->maxsegmentsize);
    tcb->pid = process_create(tcpdiscard_daemon, "TCP Discard", NORMAL_STACK,
			      PRIO_LOW);
    if (tcb->pid != NO_PROCESS) {
	process_set_arg_ptr(tcb->pid, tcb);
	process_set_crashblock(tcb->pid, TRUE);	
	tcp_increment_server_count();
    } else {
	tcp_abort(tcb);
    }
    return;
}

/*
 * tcp_listen_finger_port
 * Performs processing for a finger port request
 */
void tcp_listen_finger_port (tcbtype *tcb)
{
    int line;

    line = get_vtyline(TELNET_PORT, &tcp_connvector);
    if (line <= 0)
	tcp_close(tcb);
    else
	tcp_do_command(line, tcb, "show users", FALSE, TRUE, NULL, FALSE);
}

/*
 * tcp_listen_echo_port
 * Starts a new task for an echo port request
 */
void tcp_listen_echo_port (tcbtype *tcb)
{
    tcb->pid = process_create(tcpecho_daemon, "TCP Echo", NORMAL_STACK,
			      PRIO_LOW);
    if (tcb->pid != NO_PROCESS) {
	process_set_arg_ptr(tcb->pid, tcb);
	tcp_increment_server_count();
    } else {
	tcp_abort(tcb);
    }
}

/*
 * tcp_listen_chargen_port
 * Starts a new task for an chargen port request
 */
void tcp_listen_chargen_port (tcbtype *tcb)
{
    tcb->pid = process_create(tcpchargen_daemon, "TCP Chargen", NORMAL_STACK,
			      PRIO_LOW);
    if (tcb->pid != NO_PROCESS) {
	process_set_arg_ptr(tcb->pid, tcb);
	tcp_increment_server_count();
    } else {
	tcp_abort(tcb);
    }
}

/*
 * tcp_listen_daytime_port
 * Starts a new task for an daytime port request
 */
void tcp_listen_daytime_port (tcbtype *tcb)
{
    tcb->pid = process_create(tcpdaytime_daemon, "TCP Daytime", NORMAL_STACK,
			      PRIO_LOW);
    if (tcb->pid != NO_PROCESS) {
	process_set_arg_ptr(tcb->pid, tcb);
	process_set_crashblock(tcb->pid, TRUE);	
 	tcp_increment_server_count();	
    } else {
	tcp_abort(tcb);
    }
}

/*
 * tcp_listen_default
 * Processing for an unassigned port request
 */
void tcp_listen_default (tcbtype *tcb)
{
    int line;
    tt_soc *tty;
    boolean dummy;
    ulong lport;

    lport = reg_invoke_ipalias_port(tcb->localhost.ip_addr, tcb->localport);
    line = tcp_getline(lport, &dummy, &dummy, &tcp_connvector);

    if (line < 0) { /* handle busy lines  */
	if (tcp_debug)
	    buginf("\nTCP: connection attempt to unavailable port %d",
		   tcb->localport);
	if (MODEMS[-line]->tty_busymsg) {
	    line = get_vtyline(TELNET_PORT, &tcp_connvector);
	    if (line > 0) {
		tcp_do_command(line, tcb, "show refuse-message", FALSE,
			       TRUE, NULL, FALSE);
		return;
	    }
	}		    
    } else {	/* Check for Xremote connections */
	line = tcb->localport - XREMOTEBASE;
	if ((line >= 0) && (line < ALLlines)) {
	    tcb->ttynum = line;
	    tty = MODEMS[line];
	    tcb->tty = tty;
	    if (reg_invoke_xremote_tcp_conn(tcb, tty)) {
		return;
	    }
	}
    }
    tcp_close(tcb);
    return;
}

/*
 * estabBLOCK
 * Block until a TCB becomes ESTAB or CLOSED or the dally timer expired.
 */

static
boolean estabBLOCK (tcbtype *tcb)
{
    if (tcb->state >= ESTAB || tcb->state == CLOSED)
	return(FALSE);			/* quit if ESTAB or dying */
    if (!SLEEPING(tcb->dallytime))	/* quit if timer has expired */
	return(FALSE);
    return(TRUE);
}

void tcp_idle_estab (tcbtype *tcb)
{
    if (!tcb->pid) {
	edisms((blockproc *) estabBLOCK, (ulong) tcb);
	return;
    }

    process_watch_timer(&tcb->dallytime, ENABLE);
    while (TRUE) {
	process_wait_for_event();
	if (estabBLOCK(tcb))
	    continue;
	process_watch_timer(&tcb->dallytime, DISABLE);
	return;
    }
}

/*
 * tcp_access_check
 * Returns FALSE if the wildcard TCB and tcp_protocols would deny access.
 * Called by tcp_inputsegment to reply with an RST very early on.
 */

boolean tcp_access_check (
    tcbtype *tcb,		/* TCB in question */
    ulong sport,	        /* source remote port */	  
    ulong dport,		/* destination local port */
    ipaddrtype fhost)		/* foreign host */
{
    boolean dummy;
    tt_soc *tty;
    int line;

    /*
     * make sure there is sufficient memory
     */
    if (mempool_is_empty(MEMPOOL_CLASS_LOCAL)) 
	return(FALSE);

    if (tcb->access_check)
	return(tcb->access_check(sport, dport, fhost));
    if (tcb->localport)		/* okay if local port isn't wild */
	return(TRUE);
    line = tcp_getline(dport, &dummy, &dummy, &tcp_connvector);
    if (line <= 0)
	return(reg_invoke_tcp_access_check(dport, sport, dport, fhost));
    tty = tty_allocate(line, FALSE);
    if (!tty)			 /* no free TVT's or port in use */ 
	return(FALSE);
    if (dport == TCPPAD_ALIASPORT) {
	/*
	 * if we're a PT request, check access later, in the PT code,
	 * where we can decide whether we want to override the vty
	 * access list with a pt-specific access-list.  It is probably
	 * a gross hack to do this here instead of through some nice
	 * registry, but we do need to have access to the tcb.
	 */
	tcb->flags |= TCB_SYNCLISTEN;
	return(TRUE);
    }
    if (!check_access(tty, fhost, sport, dport, FALSE))
	return(FALSE);		/* access denied to that host */
    return(TRUE);		/* access valid and permitted */
}

/*
 * tcp_access_check_finger
 * routine to validate access to finger port.
 */
boolean tcp_access_check_finger (ulong sport, ulong dport, ipaddrtype fhost)
{
    if (finger_enable)
	return(TRUE);
    /* Else fall through and do default processing */
    return(tcp_access_check_default(sport, dport, fhost));
}
/*
 * tcp_access_check_small
 * routine to validate access to ECHO/CHARGEN/DISCARD/etc. ports.
 */
boolean tcp_access_check_small (ulong sport, ulong dport, ipaddrtype fhost)
{

    /*
     * Deny TCP small server service if max number allowed TCP small
     * server processes already exist. 
     */
    if (tcpservers_current >= tcpservers_max)
	return(FALSE);

    if (tcpservers_enable)
	return(TRUE);

    /* Else fall through and do default processing */
    return(tcp_access_check_default(sport, dport, fhost));
}

/*
 * tcp_access_check_default
 * Processing for an unassigned port request
 */
boolean tcp_access_check_default (ulong sport, ulong dport, ipaddrtype fhost)
{
    boolean dummy;
    tt_soc *tty;
    int line;

    line = tcp_getline(dport, &dummy, &dummy, &tcp_connvector);
    if (line < 0 && MODEMS[-line]->tty_busymsg &&
	check_access(MODEMS[-line],fhost,sport,dport,FALSE))
	return(TRUE);		/* Busy line that we'll explain */
    
    /* Check access to Xremote ports */
    
    if (dport >= XREMOTEBASE && dport < XREMOTEBASE + ALLlines) {
	line = dport - XREMOTEBASE;
	tty = MODEMS[line];
	if (tty && reg_invoke_xremote_tcp_check_access(
				 tty, fhost, sport, dport)) {
	    return(TRUE);
	}
    }
	    
    if (tcp_debug)
	buginf("\nTCP: connection attempt to port %d", dport);
    return(FALSE);
}

/*
 * tcp_find_translation
 * Find the translation structure for a localhost/localport pair.
 * The search starts by looking at the alias data returned using
 * localhost as a key.  This might not give us the right translation,
 * however, if there are multiple translation using the same IP address
 * but different port numbers.
 * Search the entire translation list as a last-ditch attempt.
 */
translatetype *tcp_find_translation (ipaddrtype ipaddr, ushort lport)
{
    translatetype *trans;

    trans = reg_invoke_ipalias_data(ipaddr);
    if (trans == NULL || trans->in.tcp.tcpport != lport) {
	/*
	 * We have the wrong trans block (right ip address, wrong tcp port),
	 * so we need to scan the list and find the right one!
	 */
	for (trans = (translatetype *) translateQ->qhead;
	     trans; trans = trans->next) {
	    if (INPROT(trans->type) != TRANS_TCP)
		continue;
	    if (trans->in.tcp.ipaddress == ipaddr &&
		(trans->in.tcp.tcpport == 0 ||
		 trans->in.tcp.tcpport == lport))
		break;
	}
    }
    return(trans);
}

/*
 * TCPvector stuff
 */

/*
 * tcp_getc
 * Get a character from TCP.  Returns -1 if none available.
 */

static
int tcp_getc (tt_soc *tty)
{
    tcbtype *tcb;
    int ch;
    int status;

    /*
     * First check if connection is still alive.
     */
    tcb = tty2tcb(tty, TRUE);
    if (tcb == NULL)
	return(-1);

    /*
     * Check for saved character.
     */
    if (tty->isavedchar != -1) {
	ch = tty->isavedchar;
	tty->isavedchar = -1;
	return(ch);
    }

    /*
     * Nothing saved, try protocol driver.
     */
    while (TRUE) {			/* Loop fetching characters */
	ch = tcp_getbyte(tcb,&status);	/* get next raw input byte */
        if (ch == -1)			/* nothing there....  */
	    return(-1);

	if (!(tty->conntop->status & CONN_STREAM)) {
	    ch = tnif(tty->conntop,ch);	/* telnet filter, do CRLF -> CR */
	    if (ch == -1)
	      continue;		/* Get next character if telnet ate this */
	}
	
	/*
	 * Preserve any status bits that may have meaning.  This code is
	 * slightly faster than checking for the status bits, but it does
	 * require that the MSB of the low byte never have meaning, since
	 * that bit can get masked out even when there are status bits.
	 */
	ch &= (0xFFFFFF00 | (int)tty->tty_datamask); /* Trim any parity bits */

	if (tty->type == VTY_LINE) {
	    GET_TIMESTAMP(tty->conntop->idletime); /* 0 idle time */
	    if (special_chars(tty,ch) < 0) /* Check for specials */
	      continue;
	}
	if (tty->statbits & LASTWASBREAK) {
	    tty->statbits &= ~LASTWASBREAK;
	    return(BREAK_CHAR);
	}
	if (tty->statbits & LASTWASSPEC) {
	    tty->statbits &= ~LASTWASSPEC;
	    return(ch | ESCAPED_FLAG);
	}
	return(ch);
    }
}

/*
 * tcp_clearinput
 * Flush the TCP input queues.
 */

static
void tcp_clearinput (tt_soc *tty)
{
    tcbtype *tcb;

    tty->isavedchar = -1;		/* clear saved input character */
    tcb = tty2tcb(tty,FALSE);
    if (tcb != NULL) {
	datagram_done(tcb->curinputpak);/* Flush current input buffer */
	tcb->curinputpak = NULL;	/* .... */
	tcp_flushqueue(tcb, SEGQUEUE);	/* flush input queue */
	tcb->qinput = 0;		/* .... */
	tcp_flushqueue(tcb, SAVEQUEUE);	/* flush saved queue */
	tcb->qsaved = 0;		/* .... */
	tcb->unreadbytes = 0;		/* no unread bytes */
	tcb->rcvwnd = tcb->maxrcvwnd;	/* reset window */
	tcb->delrcvwnd = 0;		/* receive window is very open */
	tcb->brs = tcb->rcvnxt;		/* start out right */
    }
}

/*
 * tcp_outputblock (tty) - Determine if output would block
 */

static
boolean tcp_outputblock (tt_soc *tty)
{
    tcbtype *tcb;

    tcb = tty2tcb(tty, FALSE);
    if (!tcb)
      return(FALSE);
    if ((tcb->state != ESTAB) && (tcb->state != CLOSEWAIT))
	return(FALSE);
    return(tcp_putblock(tcb));
}

/*
 * tcp_putc
 * Write a byte to a TCP connection
 */

static
void tcp_putc (tt_soc *tty, int ch)
{
    tcbtype *tcb;

    /*
     * Experimental code to improve milking machine performance.
     * The dispatch character mechanism also applies here.
     */
    if (tty->tty_dispatchtimeout) {
	if ((tty->type != VTY_LINE) && (tty->tty_dispatchtimeout > 10)) {
	    conntype *conn = tty->conntop;
	    if (!TIMER_RUNNING(conn->dispatchtimer))
		TIMER_START(conn->dispatchtimer, tty->tty_dispatchtimeout);
	} else {
	    tty->pushdata = TRUE;
	}
    } else {
	tty->pushdata = TRUE;
    }

    if (tty->tty_editinfo.dispatch[ch & 0x7F] & TTY_DISP_DISP) {
	tty->pushdata = TRUE;
    }
    if (tty->dispatch_machine) {
	tty->pushdata = tty_runstates(tty->conntop, ch);
    }

    tcb = tty2tcb(tty,FALSE);
    if (tcb == NULL) {
	if ((tty != console) && tty->automore_enabled) {
	    tty->automore_flush = TRUE;
	}
	return;
    }
    ch &= (0xFFFFFF00 | (int)tty->tty_datamask); /* Trim any parity bits */
    if (tty->conntop->status & CONN_STREAM)
	(void) tcp_putbyte(tcb,ch,TRUE);   /* stream, maybe block */
    else
	(void) tnof(tty->conntop,ch,TRUE,TRUE); /* telnet filter, maybe block */
}

/*
 * tcp_startoutput
 * Send off a buffer of bytes
 */

static
boolean tcp_startoutput (tt_soc *tty)
{
    tcbtype *tcb;

    tcb = tty2tcb(tty,FALSE);
    if (tcb == NULL)
	return(FALSE);

    /*
     * Signal helper process.
     */
    if (tty_signal_helper(tty))
	return(TRUE);

    /*
     * Efficency improvements:
     * Experimental code to implement dispatch timer in ttydaemons
     * If dispatch timeout is small, then try to send large and small packets.
     * If the push flag has been set then the data has to be sent and the
     * timer is then reset.
     */
    if (!tty->pushdata && tty->tty_dispatchtimeout && (tty->type != VTY_LINE)) {
	conntype *conn = tty->conntop;
	if (tty->tty_dispatchtimeout > 10) {
	    if (TIMER_RUNNING_AND_SLEEPING(conn->dispatchtimer))
		return(TRUE);		/* Don't really send the packet yet */
	} else {
	    paktype *pak = tcb->curoutputpak;
	    if (pak && (pak->length > 0) &&	/* do send full packets */
		((pak->us_seglen - pak->length) > 2)) { /* do send echos */
		if (TIMER_RUNNING(conn->dispatchtimer)) {
		    if (SLEEPING(conn->dispatchtimer))
			return(TRUE);		/* wait a while longer */
		} else {
		    TIMER_START(conn->dispatchtimer,
				(WAITFACTOR * tty->tty_dispatchtimeout));
		    return(TRUE);
		}
	    }
	}
    }

    if (tty->pushdata || TIMER_RUNNING_AND_AWAKE(tty->conntop->dispatchtimer)) {
	tcp_putpush(tcb,TRUE,FALSE);	/* set PSH, don't set FIN */
	tty->pushdata = FALSE;
	TIMER_STOP(tty->conntop->dispatchtimer);
	tty->dispatch_state = 0;
    }
    return(TRUE);
}

/*
 * tcp_inputpending
 * Return the number of unread bytes
 */

static
int tcp_inputpending (tt_soc *tty)
{
    tcbtype *tcb;
    int count;

    tcb = tty2tcb(tty,TRUE);
    if (tcb == NULL)
	return(0);
    count = tcp_unread(tcb);
    if (tty->isavedchar != -1)
	count++;
    return(count);
}

/*
 * tcp_outputpending
 * Return the number of bytes to be written to the net.
 */

static
int tcp_outputpending (tt_soc *tty)
{
    tcbtype *tcb;
    paktype *pak;

    tcb = tty2tcb(tty, FALSE);
    if (tcb == NULL)
	return(0);
    pak = tcb->curoutputpak;
    if (pak == NULL)
	return(0);
    else
	return(pak->us_seglen - pak->length);
}

/*
 * tty2tcb
 * Obtain a TCB pointer given a terminal pointer.
 * Returns NULL if connection is dead or dying.
 */

tcbtype *tty2tcb (tt_soc *tty, boolean readflag)
{
    tcbtype *tcb;

    if (tty->statbits & (CARDROP|IDLE))
	return(NULL);
    if (tty->type == VTY_LINE) { /* on vtys, set cardrop if connection dies */
	if (tty->conntop == NULL) {
	    tty->statbits |= CARDROP;
	} else {
	    tcb = tty->conntop->tcb;
	    if ((tcb->state == CLOSED) ||
		(readflag && (tcp_closing(tcb) == TRUE))) {
		tty->statbits |= CARDROP;
		return(NULL);
	    }
	}
    }
    if (tty->conntop)
	return(tty->conntop->tcb);
    else return (NULL);
}

/*
 * tvt_init
 * Set up the vector table for TCP Virtual Terminals
 */

void tvt_init (void)
{
    vectortype *vector;

    vector = &TCPvector;
    vector_init(vector);		/* initialize vector structure */

    vector->vgetc = tcp_getc;		/* character input */
    vector->vclearinput = tcp_clearinput;/* clear buffered input */
    vector->vinputpending = tcp_inputpending;

    vector->vputc = tcp_putc;			/* character output */
    vector->vputpkt = generic_putpkt;		/* Put packet routine */
    vector->voutputblock = tcp_outputblock;	/* Would output block? */

    vector->vstartoutput = tcp_startoutput;	/* start character output */
    vector->voutputpending = tcp_outputpending;
    vector->vclearoutput = tcp_clearoutput;	/* send AO DM */
    vector->vsetsflow = tcp_setsflow;		/* turn on flow control */

    vector->vservicehook = helper_servicehook;
    vector->vnoservicehook = helper_noservicehook;

    conn_vector_init(&tcp_connvector,
		     "telnet", "TCP/IP Telnet protocol");
					/* Init the connection vector */
    tcp_connvector.snmp_sesstype = D_tslineSesType_telnet;

    tcp_connvector.vsettty = tcp_set_new_tty; /* Set the new tty routine */
    tcp_connvector.valive = telnet_alive; /* Is connection alive? */
    tcp_connvector.vrun = telnet;	/* Run the session */
    tcp_connvector.vclose = close_connection; /* Close the session */
    tcp_connvector.vunread = telnet_unread; /* Get unread input count */
    tcp_connvector.vopen = telnet_multiproto_open; /* Parse connect command */
    tcp_connvector.vresume = tcp_parse_resume; /* Parse resume command */
    tcp_connvector.vaccounting = telnet_conn_acct;
    /*
     * If no other transport has registered itself as the default, set
     * ourselves to be the default.
     */
    if (!connect_sysdef_transport)
	connect_sysdef_transport = tcp_connvector.type;
}

/*
 * show_ttybusy
 * type an appropriate busy mesasge on the terminal.  Since the vty we are
 * talking to is NOT ter terminal whose busy message we want to display,
 * we have to rederive that information from the tcp port numbers.
 */

void show_ttybusy (parseinfo *csb)
{
    conntype *conn = stdio->conntop;
    tcbtype *tcb;
    int dummy;
    int line;
    ulong port;

    /*
     * $$$-dino, need to deal with this.
     */
    if (conn) {
	tcb = conn->tcb;
	if (tcb) {
	    port = reg_invoke_ipalias_port(tcb->localhost.ip_addr,
					   tcb->localport);
	    line = - tcp_getline(port, &dummy, &dummy, NULL);
	    if (line > 0 && line < ALLlines) 
		printf("%s", MODEMS[line]->tty_busymsg);
	}
    }
}

/*
 * ttyBLOCK
 * Event dismiss for tty daemon
 */

boolean ttyBLOCK (tt_soc *tty)
{
    if ((*tty->ivector->vinputpending)(tty) &&
	!(*tty->ovector->voutputblock)(tty))
	return(FALSE);		/* Wake up if net input and tty output */

    if ((*tty->ovector->vinputpending)(tty) &&
	!(*tty->ivector->voutputblock)(tty))
	return(FALSE);		/* Wake up if tty input and net output */

    if (!conn_alive(tty->conntop))
	return(FALSE);			/* wakeup if connection is dead */
    if (tty->statbits & CARDROP)
	return(FALSE);			/* wakeup if carrier loss */
    if (TIMER_RUNNING_AND_AWAKE(tty->dallytimer)) {
	tty->statbits |= IDLETIMEOUT;
	return(FALSE);			/* wakeup if idle timeout */
    }
    if (TIMER_RUNNING_AND_AWAKE(tty->conntop->dispatchtimer))
	return(FALSE);
    if (tty->conntop->status & CONN_CHANGED)
	return(FALSE);			/* Wakeup if connection changed */
    return(TRUE);
}

/*
 * tty_daemon
 * The process that nurses along a protocol/serial line connection.
 * Assumes that tty->conntop, tty->ivector, tty->ovector,and tty->tiptop
 * have been set up.
 */

forktype tty_daemon (tt_soc *tty)
{
    int count, ch;
    boolean fast_input;
    tcbtype *tcb = NULL;
    uchar *dataptr;
    int datacount;
    int status;

    reg_invoke_tty_birth(TTYP_NW_DAEMON, tty);

    async_daemon_attach(tty);
    tty = stdio;		/* attach can change the tty! */

    /*
     * If stream mode TCP input, we can run faster.
     */
    if (tty->ivector == &TCPvector && (tty->conntop->status & CONN_STREAM)) {
	fast_input = TRUE;
    } else {
	fast_input = FALSE;
    }
    /*
     * Reset the dispatch character mechanism
     */
    tty->pushdata = FALSE;

    /*
     * If this is really a TCP connection, get the tcb and
     * set up the TCP incoming queuemax. A non-zero value is used
     * to enable tcp SEGQUEUE buffer coalescing.
     * tty_daemon is used for non-TCP reverse connections as well
     */
    if (tty->ivector == &TCPvector) {
	tcb = tty2tcb(tty, TRUE);
	if (tcb) {
	    tcb->qinput_max = tty->tty_tcp_coalesce_size;
	}
    }

    /*
     * Shovel bytes both ways until the connection closes or times out.
     */
    while (TRUE) {
	tty_dallytimer(tty);
	edisms((blockproc *)ttyBLOCK,(long)tty);
	GET_TIMESTAMP(tty->conntop->idletime);
	if (tty->statbits & (IDLETIMEOUT + CARDROP)) {
	    if (modem_debug) {
		if (tty->statbits & IDLETIMEOUT)
		    buginf("\nTTY%t: Inactivity timeout", tty->ttynum);
		else
		    ttybug(tty, "Carrier Drop");
	    }
	    break;
	}

	/* Check for port changes on the connection */

	if (tty->conntop->status & CONN_CHANGED && conn_changed(tty->conntop))
	    conn_report(tty->conntop);

	/*
	 * Handle first side of the full duplex connection.  If TCP is
  	 * feeding this connection, we can optimize by using tcp_getstring.
         */

	if (inputpending(tty)) {
	    count = TTYDAEMON_OUTLIMIT(tty);	/* set a limit */
 	    if (fast_input) {
 		while (count > 0) {
 		    status = tcp_getstring(tcb, &dataptr, &datacount, MAXINT);
 		    switch (status) {
	 	    case URGENT_DATA:
 		    case OK:
			status = putpkt(tty, tcb->curinputpak,
					dataptr, datacount); /* Queue packet */
			datacount -= status; /* Get bytes not moved */
			tcp_putback(tcb, datacount);	/* Put back the data */
			if (!status)
			    count = -1; /* Nothing output, don't loop */
			else
			    count -= status; /* Update count */
			break;
		    default:
 			count = -1;
		    }
 		}
 	    } else while (--count && !outputblock(tty)) {
	        ch = getc(tty);		/* get byte from protocol */
	        if (ch == -1)
		    break;		/* no bytes just now */
	        putc(tty,ch);		/* send a byte to the printer */
	    }
	}
	(void)startoutput(tty);		/* make sure output running */

	/*
	 * Handle other side of the full duplex connection.
         */
	if ((*tty->ovector->vinputpending)(tty)) {
	    count = TTYDAEMON_INLIMIT(tty); /* set a limit */
	    while (--count && !(*(tty->ivector->voutputblock))(tty)) {
	        ch = (*tty->ovector->vgetc)(tty); /* get byte from printer */
	        if (ch == -1) {
		    break;		/* no bytes just now */
		}
		/* send a byte to the protocol */
	        (*tty->ivector->vputc)(tty,ch);
	    }
	}
	/* make sure output running */
	(void)(*tty->ivector->vstartoutput)(tty);

	if (!conn_alive(tty->conntop)) {
	    /* if closed and no pending input */
	    if (modem_debug) {
		buginf("\ntcp%t: conn not alive", tty->ttynum);
	    }
	    break;			/* then close down */
	}
    }
    async_daemon_kill(tty);
}
