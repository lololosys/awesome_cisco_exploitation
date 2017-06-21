/* $Id: padtcp.c,v 3.6.6.5 1996/08/28 13:07:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/pt/padtcp.c,v $
 *------------------------------------------------------------------
 * padtcp.c  PAD <--> TCP protocol translation.
 *
 * May 1990,  Bill Westfield
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: padtcp.c,v $
 * Revision 3.6.6.5  1996/08/28  13:07:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.6.4  1996/08/26  19:10:46  swenson
 * CSCdi62987:  stream parameter ignored on x25->tcp trans ...
 * don't send telnet negotiations for pad changes if tcp side is stream
 * Branch: California_branch
 *
 * Revision 3.6.6.3  1996/07/12  20:09:24  widmer
 * CSCdi62775:  Bogus externs in telnet.c cause 4k builds to fail
 * Branch: California_branch
 * Remove externs and move externs to header files
 *
 * Revision 3.6.6.2  1996/06/20  21:32:13  swenson
 * CSCdi60641:  stream and binary not set for pad-tcp using swap keyword
 * Branch: California_branch
 *
 * Revision 3.6.6.1  1996/05/01  01:59:44  ravindra
 * CSCdi49877:  deleting translate with x25 pvc results in corruption crash
 * Branch: California_branch
 *
 * Revision 3.6  1996/03/06  09:31:26  billw
 * CSCdi44863:  AAA Accounting not done for one-step protocol translation
 * Also ensure tty->ustruct gets set for one-step PT sessions.
 *
 * Revision 3.5  1996/01/23  23:45:24  vandys
 * CSCdi45547:  Desire rotary behavior on PT TCP destinations
 * Add rotor option to randomize destination for TCP host lists
 *
 * Revision 3.4  1995/11/22  05:56:18  eschaffe
 * CSCdi44200:  !23 ports in the translate command should override ip host
 * port
 *
 * Revision 3.3  1995/11/20  23:22:03  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  18:59:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:22:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/13  21:48:07  eschaffe
 * CSCdi18102:  Require one translate statment to multiple hosts
 *         Add "host-name" Telnet/TCP outgoing option.
 *
 * Revision 2.6  1995/09/15  23:33:44  eschaffe
 * CSCdi39626:  PT on a PVC connection with "swap" option is terminated
 * after the first connection attempt.
 *
 * Revision 2.5  1995/07/14  07:02:58  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.4  1995/06/20 03:44:54  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.3  1995/06/17  06:26:01  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.2  1995/06/09  13:18:22  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:34:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../lat/mtcsys.h"
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "name.h"
#include "packet.h"
#include "ttysrv.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "subsys.h"
#include "registry.h"
#include "../pt/pt_registry.h"
#include "connect.h"
#include "../x25/x25.h"
#include "../x25/x25_address.h"
#include "../x25/x25_switch.h"
#include "../x25/pad.h"
#include "../tcp/telnet.h"
#include "access.h"
#include "translate.h"
#include "logger.h"
#include "login.h"
#include "../os/async.h"
#include "padtcp.h"
#include "pt_pad_common.h"
#include "../h/auth_registry.regh"
#include "stacks.h"
#include "../tcp/msg_telnet.c"		/* Not a typo, see logger.h */
#include "vtyasync.h"


static void pt_padtcp_init(subsystype *);
/*
 * PAD-TCP Protocol Translator subsystem header
 */

#define PT_PADTCP_MAJVERSION 1
#define PT_PADTCP_MINVERSION 0
#define PT_PADTCP_EDITVERSION  1

SUBSYS_HEADER(pt_padtcp,
              PT_PADTCP_MAJVERSION, PT_PADTCP_MINVERSION, PT_PADTCP_EDITVERSION,
              pt_padtcp_init, SUBSYS_CLASS_PROTOCOL,
              "seq: pt_pad, pt_tcp",
              "req: pt_pad, pt_tcp");

/*
 * pt_padtcp_init
 *
 * Subsystem initialization code for tcp<->lat translation.
 */

static void pt_padtcp_init (subsystype *subsys)
{
    reg_add_pt_tcpinput(TRANS_X25, tcp_transto_x25, "tcp_transto_x25");
    reg_add_pt_x25input(TRANS_TCP, x25_transto_tcp, "x25_transto_tcp");
}

/*
 * padtotcp_daemon
 * process to manage one incoming pad connection and one outgoing tcp
 * connection, passing X.3 type info in both directions, and doing automatic
 * setup of the tcp connection.  This is analagous to tty_daemon.
 * Note: Some of the pt_..... helper subroutines are allowed to kill
 * the thread "death()" and might not return to the daemon process.
 */

static forktype padtotcp_daemon (translatetype *trans)
{
    tt_soc *tty = stdio;
    conntype *padconn = tty->conntop;
    conntype *tcpconn;
    boolean pvc = trans->in.x25.x25flags & TRANSF_X25_PVC;
    int n;
    addrtype address, source_address, *saddr;
    short tcpport;
    nametype *nameptr;

    if (translate_debug)
        buginf("\npadtcp%t: fork started", tty->ttynum);

    pt_pad_setup(trans, tty);

again:

    if (!pt_pad_check_lci(trans, tty)) {
    	if (pvc) {
    	    if (translate_debug)
    	    	buginf("\npadlat%t:conntop:%x",tty->ttynum,tty->conntop);
    	    pt_pad_clear_connection(trans,tty);
    	}
    }

    address.type = ADDR_IP;
    tcpport = trans->out.tcp.tcpport;

    saddr = telnet_source_address(trans->out.tcp.source_idb, &source_address);
    if (trans->out.tcp.tcpflags & TRANSF_TCP_HOST_NAME) {
	nameptr = trans->out.tcp.tcpnptr;
	/*
	 * Attempt to open the next destination if the first one fails.
	 * The IP host string contains a host name, port, and IP 
	 * addressess.
	 * Where "IP address" can be one to MAXADDRESSES.
	 * "port" is an optional 16 bit port number which will be used 
	 * instead of the default telnet port.
	 *
	 * If the port in the translate command is not a telnet port, then it 
	 * should override the host port.
	 */
	if (tcpport == TELNET_PORT && nameptr->v.ip.port)
	    tcpport = nameptr->v.ip.port;
	tcpconn = open_connection(nameptr, NULL, tcpport, saddr,
	    (trans->flags & TRANSF_GLOB_QUIET ?  &n : NULL), NULL, 0, NULL);
    } else {
	address.ip_addr = trans->out.tcp.ipaddress;
	tcpconn = open_connection(NULL, &address, tcpport, saddr,
	    (trans->flags & TRANSF_GLOB_QUIET ?  &n : NULL), NULL, 0, NULL);
    }
    if (tcpconn == NULL) {
	if (translate_debug)
	    buginf("\npadtcp: can't get tcp connection");
	trans->failures++;
	if (pvc)
	    goto killpvc;
	pt_pad_clear_connection(trans, tty);
    }
    pt_pad_accept_trans(trans, tty);

    /*
     * allow application programs to cause negotiation by treating any
     * number of IACs as a single IAC.  Eg, the IAC IAC WONT ECHO that
     * gets generated when an applicate send IAC WONT ECHO still gets
     * interpretted as the telnet command.  IACs can no longer be sent at all.
     */
    if (trans->out.tcp.tcpflags & TRANSF_TCP_IACHACK)
	tcpconn->proto.tn.tn_state |= ST_IACHACK;

    /*
     * perhaps start some negotiations on the telnet side
     */
    if (trans->out.tcp.tcpflags & TRANSF_TCP_STREAM)
	tcpconn->status |= CONN_STREAM;
    else if (trans->out.tcp.tcpflags & TRANSF_TCP_BINARY) {
	(void)send_option(tcpconn, STO_BINARY,STO_BINARY, TN_WILL, TNO_BINARY);
	(void)send_option(tcpconn, STH_BINARY,STH_BINARY, TN_DO,   TNO_BINARY);
    }

    startoutput(tty);
    tty->statbits |= CONNECTED;

    tcpconn->acct_info = reg_invoke_aaa_acct_conn(stdio->ustruct, NULL,
						  tcpconn);
    if (trans->flags & TRANSF_GLOB_SWAP) {
	/*
	 * Swap incoming and outgoing connections, and act like a TCP to
	 * PAD connection.
	 */

	tty->ovector = &TCPvector;
	tty->ivector = &TCPvector;
	tty->conntop = tcpconn;
	tty->connhead.qhead = padconn;
	run_tcppad(trans, tty);
    } else
	run_padtcp(trans, tty);



    /*
     * connection has died.  Clean up and comit suicide.
     */

    conn_close(tcpconn);		/* close connection now */
killpvc:
    if (pvc) {
        /*
	 * TCP connection is closed, try to establish it again.
         * Swap incoming and outgoing connections back (PAD to TCP session)
         */

	if (trans->flags & TRANSF_GLOB_SWAP) {
	    tty->ovector = &PADvector;
	    tty->ivector = &PADvector;
	    tty->conntop = padconn;
	    tty->connhead.qhead = NULL;
	}
	if ((tty->statbits & CARDROP) || !pad_alive(padconn)) {
	    if (translate_debug)
		buginf("\npadtcp%t: PAD PVC %d went away!", tty->ttynum,
		       trans->in.x25.cudprefix);
	} else
	    goto again;
    }
    conn_close(padconn);
    tty->statbits |= CARDROP;		/* .... */
    tty->statbits &= ~CONNECTED;	/* Mark as not connected anymore */
    trans->active--;
    death(tty);				/* close down */
    process_kill(THIS_PROCESS);		/* just in case */
}


void run_padtcp (translatetype *trans, tt_soc *tty)
{
    conntype *padconn = tty->conntop;
    pcbtype *pcb = (pcbtype *) padconn->tcb;
    conntype *tcpconn = (conntype *)tty->connhead.qhead;
    tcbtype *tcb = (tcbtype *)tcpconn->tcb;
    int n, status, datacount;
    uchar *dataptr;
    ulong echosave;
    int pushflag, ch, result;

    pt_pad_setup_x3_options(trans, tty);

    /*
     * Local handling of options on incoming PAD connections. 
     * Allow the remote PAD to perform the echoing of characters.
     */
    if (trans->flags & TRANSF_GLOB_LOCAL)
	pcb->flags |= PCB_ECHO;

    tcpconn->proto.tn.statechange_check = padtcp_tnoptioncheck;
    tcpconn->proto.tn.statechange_done = padtcp_tnconfirm;
    padconn->proto.pad.statechange_check = padtcp_padoptioncheck;

    /*
     * We now have a connection going in each direction, one TCP, and one
     * PAD.  Shovel bytes both ways, and translate state changes.
     */
    while (TRUE) {
	if (tty->tty_sessiontimeout)
	    TIMER_START(tty->dallytimer, tty->tty_sessiontimeout);
	edisms((blockproc *)tcppadBLOCK,(long)tty);
	GET_TIMESTAMP(padconn->idletime);
	if (tty->statbits & (IDLETIMEOUT + CARDROP))
	    break;
	if (!conn_alive(tcpconn))	/* if closed and no pending input */
	    break;			/* then close down */

	/*
	 * read from tcp, write to pad.
         */

	n = 128*4;
	if (tcp_unread(tcb) || tcp_closing(tcb)) {
	    while (--n > 0) {
	        status = tcp_getstring(tcb, &dataptr, &datacount,
				       MAXINT);
		if (status == NO_DATA || status == END_OF_FILE)
		    break;	

		if (status != OK && status != URGENT_DATA) {
		    errmsg(&msgsym(BADSTATE, TN), status);
		    break;
		}
		n -= datacount;
		if (tcpconn->status & CONN_STREAM)
		    status = putpkt(tty, tcb->curinputpak, dataptr, datacount);
		else
		    status = telnet_input(tcpconn, dataptr, datacount);
		datacount -= status; /* Get bytes not moved */
		tcp_putback(tcb, datacount);	/* Put back the data */
		if (!status)
		    n = -1; /* Nothing output, don't loop */
		else
		    n -= status;
	    } /* while */
	    if (pcb->flags & PCB_CTLREADY) {
		/* if there is a control packet waiting to be sent,
		 * we should send it now before we try to collect data
		 */
		pad_putcontrol(pcb);
	    } else
		pad_putpush(pcb, FALSE);	/* ensure output is running */
	} /* if */

	/*
	 * read from pad, write to tcp
	 */
	if (pcb->state != PAD_ESTABLISHED)
	    break;

	if (n < 0)
	    n = 128;

	if (pad_unread(padconn)) {
	    if (tcpconn->status & CONN_STREAM) {
		/*
		 * STREAM mode case - get and put strings to the connections
		 * without looking at the indicidual bytes.
		 */
		while (n > 0) {
		    status = pad_getstring(pcb, &dataptr, (long *) &datacount,
					   MAXINT);
		    if (status == NO_DATA || status == END_OF_FILE)
			break;
		    if (status != OK && status != URGENT_DATA) {
			errmsg(&msgsym(BADSTATE, TN), status);
			break;
		    }
		    n -= datacount;
		    status = tcp_putstring(tcb, dataptr, datacount);
		    /*
		     * If we couldn't send all the data to the tcp
		     * connection, put it back in the pcb.
		     */
		    if (status != datacount) {
			pad_putback(pcb, datacount - status);
			break;
		    }
		}
		(void)tcp_putpush(tcb,TRUE,FALSE); /* ensure output */
		continue;
	    }
	    /*
	     * Non-stream case - handle echoing, telnet, etc
	     */
	    pushflag = FALSE;
	    /* 
	     * save echo state 
	     */
	    echosave = (tcpconn->proto.tn.tn_option & STH_ECHO);
	    if (pcb->paramsin[X3_LOCALECHO])		/* if PAD is echoing */
		tcpconn->proto.tn.tn_option |= STH_ECHO;   /*  TCP shouldn't */
	    while (n > 0) {

		if (tcpconn->proto.tn.tn_option & STO_EDIT) {
		    result = telnet_getline(tcpconn);
		    switch (result) {
		      case READLINE_DONE:
			pushflag = TRUE;
			break;
		      case READLINE_BLOCK:
			break;
		      default:
			errmsg(&msgsym(READLINE, TN), result);
		    }
		} else {
		    if (tty->tty_dispatchtimeout) {
			if (!TIMER_RUNNING(tcpconn->dispatchtimer))
			    TIMER_START(tcpconn->dispatchtimer,
					tty->tty_dispatchtimeout);
		    } else
			pushflag = TRUE;
		}
		while (--n > 0) {
		    ch = getc(tty);	/* get byte from tty */
		    if (ch == -1) {
			n = -1;
			break;		/* quit if no tty input data */
		    }
		    if (tty->tty_editinfo.dispatch[ch & 0x7F] & TTY_DISP_DISP)
			pushflag = TRUE;
		    if (tnof(tcpconn,ch,FALSE, FALSE) == FALSE)
			break;		/* quit if can't do output */
		    if ((tcpconn->proto.tn.tn_option & STH_ECHO) == 0) {
			if (((tcpconn->proto.tn.tn_option & STO_BINARY) == 0) 
			    && ((tty->capabilities & TRANSPARENT) == 0))
			    printc(ch);
			else
			    putc(tty,ch);
		    }
		} /* not local edit */

	    } /* while */
	    (void)tcp_putpush(tcb,TRUE,FALSE); /* ensure output is running */
	    /* restore state of echo flag */
	    tcpconn->proto.tn.tn_option &= ~STH_ECHO;	
	    tcpconn->proto.tn.tn_option |= echosave;
	} /* if */
    } /* while */
}

/*
 * padtcp_tnoptioncheck
 * Check whether a telnet option state change is allowed for a protocol
 * translation session (pad to tcp connection).  This is similar to
 * telnetuser_optioncheck, except it generates x.29 message to affect
 * the remote PAD.  In fact, use the telnetuser optioncheck for things
 * that don't need to be forwarded on to the PAD.  This saves code and
 * makes it easier to add new telnet options.
 */

boolean padtcp_tnoptioncheck (conntype *conn, uchar code)
{
 
    char opts[80];
    conntype *padconn = conn->tty->conntop;
    pcbtype *pcb = (pcbtype *) padconn->tcb;
 
    if (translate_debug)
	buginf("\nPADTCP%t: Telnet received %s", conn->tty->ttynum,
	       option_string(conn->proto.tn.lastcmd,code,opts));

    switch (code) {
	/*
	 * If a user process, refuse to echo.  It would be bogus to do so
	 *  in any case, but 4.3 telnetd's use this to detect 4.2 telnet's,
	 *  and we don't want to be considered 4.2, because then they won't
	 *  use SYNC's.
	 * Also always do local echoing if USER requested Line mode.
	 */
      case TNO_ECHO:
	if (conn->proto.tn.lastcmd == TN_DO)
	    return(FALSE);
	if ((conn->proto.tn.tn_option & STO_LINE) && 
	    conn->proto.tn.lastcmd == TN_WILL)
	    return(FALSE);

	/* Allow the remote PAD to perform the echoing of characters. */
	if (conn->proto.tn.lastcmd == TN_WILL) {
	    if (pcb->flags & PCB_ECHO)
		return(FALSE);
	    else
		pad_option_emit(pcb, X3_SETANDREAD, X3_LOCALECHO, 0);
	}
	else if (conn->proto.tn.lastcmd == TN_WONT) {
	    if (pcb->flags & PCB_ECHO)
                return(FALSE);
	    else
		pad_option_emit(pcb, X3_SETANDREAD, X3_LOCALECHO, 1);
	}
	return(TRUE);
	
	/*
	 * The system currently uses SUPRGA to indicate "local lin/edit mode".
	 * A host that refuses to Supress GAs, probably wants line at a time
	 * input, even if it never actually sends any GAs.  This is compatible
	 * with a number of hosts, such as Crays...
	 */
      case TNO_SUPRGA:
	if ((conn->proto.tn.tn_option & STO_LINE) && 
	    (conn->proto.tn.lastcmd == TN_WILL))
		return(FALSE);		/* Stay in user requested line mode */

	if ((conn->proto.tn.lastcmd == TN_WONT) &&
	    ((conn->proto.tn.tn_option & STH_ECHO) == 0)) {
	    if ((conn->proto.tn.tn_option & STO_EDIT) == 0) {
		conn->proto.tn.tn_option |= STO_EDIT;
		pad_option_emit(pcb, X3_SETANDREAD, X3_EDIT, 1);
	    }
	} else
	    if ((conn->proto.tn.lastcmd != TN_DONT) && 
		(conn->proto.tn.tn_option & STO_EDIT)) {
		conn->proto.tn.tn_option &= ~STO_EDIT;
		pad_option_emit(pcb, X3_SETANDREAD, X3_EDIT, 0);
	    }		
	return(TRUE);

      default:
	break;
    }
    return(telnetuser_optioncheck(conn, code));
}

/*
 * tcppad_padoptioncheck
 * Check whether an X.3 option state change is allowed for a protocol
 * translation session (tcp to pad connection).  In theory, we ought never
 * to get any of these from a PAD originating a call, but you never can tell.
 */
boolean padtcp_padoptioncheck (conntype *conn, int param, int value)
{
    tt_soc *tty = conn->tty;
    conntype *tcpconn = (conntype *) tty->connhead.qhead;
    pcbtype *pcb = (pcbtype *) conn->tcb;
    boolean success = TRUE;

    if (pcb->paramsout[X3_DISPMASK] == value)	/* No change in value ? */
	return(success);			/* let it succeed */

    switch (param) {
	/*
	 * Parameter requests which we refuse to do.
	 */
      case X3_RECALLCHAR:
      case X3_SERVICE:
      case X3_FOLDING:
      case X3_SERVED:
      case X3_ECHOMASK:
      case X3_PARITY:
      case X3_PAGEWAIT:
	if (value != 0)
	    success = FALSE;
	break;

	/*
	 * The following parameters are basically handed directly from
	 * x3_getline, and so don't need to have any actual effect on
	 * the terminal database.
	 */
	
      case X3_LFINSERT:
      case X3_BREAK:
      case X3_IDLETIME:
	break;

	/*
	 * Pad host wants to change local editing status.
	 * Try to negotiate this with the TCP host (only works sometimes)
	 * cisco terminal servers and 4.3 bsd use a signal of
	 * Won't Supress-GA to mean local editing.
	 * Try to request this, but do editing here too.  If local
	 * editing is actually done, we just won't see any editing
	 * character ever get to us.  Note that if dispatch characters
	 * other than CR are required, we can't use this!
	 */
      case X3_EDIT:
	if (tcpconn->status & CONN_STREAM)
	    break;
	if (value != 0 && pcb->paramsout[X3_DISPMASK] == 2)
	    send_option(tcpconn, STH_SUPRGA, 0, TN_DONT, TNO_SUPRGA);
	if (translate_debug)
	    buginf("\npadtcp%t: Sending DONT SUPRGA", tty->ttynum);
	break;
      case X3_CHARDEL:
      case X3_LINEDEL:
      case X3_LINEDISP:
	break;

      case X3_DISPMASK:
	if (tcpconn->status & CONN_STREAM)
            break;
	if (pcb->paramsout[X3_EDIT]) {
	    if (value == 2 || value == 0) {
		send_option(tcpconn, STH_SUPRGA, 0, TN_DONT, TNO_SUPRGA);
		if (translate_debug)
		    buginf("\npadtcp%t: Sending DONT SUPRGA", tty->ttynum);
	    } else {
		send_option(tcpconn, STH_SUPRGA, STH_SUPRGA, TN_DO,
			    TNO_SUPRGA);
		if (translate_debug)
		    buginf("\npadtcp%t: Sending DO SUPRGA", tty->ttynum);
	    }
	}
	break;

	/*
	 * Pad host wants local or remote echoing
	 * Try to negotiate this with TCP host (should always work).
	 * The only thing we could do otherwise would be to do echoing
	 * from the translator when the pad host requested it, and the
	 * telent refused to do it, but telnet is supposed to ALWAYS
	 * support local echo.  Note that when the telent client responds
	 * that it will indeed do local editing (DONT ECHO), we clear
	 */
      case X3_LOCALECHO:
	if (tcpconn->status & CONN_STREAM)
            break;
	if (value) {	/* request local echo */
	    if (send_option(tcpconn, STH_ECHO, 0, TN_DONT, TNO_ECHO))
		if (translate_debug)
		    buginf("\npadtcp%t: Sending DONT ECHO", tty->ttynum);
	    
	} else		/* request remote or no echo */
	    if (send_option(tcpconn, STH_ECHO, STH_ECHO, TN_DO, TNO_ECHO))
		if (translate_debug)
		    buginf("\npadtcp%t: Sending DO ECHO", tty->ttynum);
	break;

      case X3_CRPAD:
      case X3_FLOWIN:
      case X3_LFPAD:
	break;

      case X3_FLOWOUT:
	tty->capabilities |= SFLOWOUT;
	break;

      case X3_DISCARD:
	switch (value) {
	  case 0:
	    pcb->flags &= ~PCB_DISCARD;
	    break;
	  case 1:
	    pcb->flags |= PCB_DISCARD;
	    break;
	  default:
	    success = FALSE;
	    if (conn->debug) printf(padques);
	    break;
	}
	break;
      default:
	success = FALSE;
	break;
    }
    /* ensure output is running */
    (void)tcp_putpush((tcbtype *)tcpconn->tcb,TRUE,FALSE); 
    return(success);
}

/*
 * padtcp_tnconfirm
 * Come here when the telnet client responds to our request for option
 * change.  The only thing that needs to handled specially is echoing,
 * expect that it doesn't really happen here.
 * Everything else, it won't matter whether both the translator and the
 * telnet client do the same thing.. *
 * the telnet location option must also be handled specially (or at least
 * in the same way as on a terminal server.
 */
void padtcp_tnconfirm (conntype *conn, ulong bit, ulong state, ulong code)
{
    if (code == TNO_TTYLOC)
	telnetuser_optiondone(conn, bit, state, code);
}

/*
 * x25_transto_tcp
 * Translate X25 input to TCP output
 * Returns PID of translation process or -1 if failure
 * Called by SERVICE_TRANS_X25INPUT, index is TRANS_TCP
 */

int x25_transto_tcp (translatetype *trans, tt_soc *tty)
{
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    if (reg_invoke_kerberos_is_running())
	stacksize = (ulong) EXEC_STACK_SIZE;
    return(cfork((forkproc *)padtotcp_daemon, (long)trans, stacksize,
		 "PAD to TCP", tty->ttynum));

}
