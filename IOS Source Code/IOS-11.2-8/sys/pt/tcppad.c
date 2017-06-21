/* $Id: tcppad.c,v 3.5.6.6 1996/08/26 19:10:51 swenson Exp $
 * $Source: /release/112/cvs/Xsys/pt/tcppad.c,v $
 *------------------------------------------------------------------
 * tcppad.c  TCP <--> PAD protocol translation.
 *
 * April 1988,  Bill Westfield
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcppad.c,v $
 * Revision 3.5.6.6  1996/08/26  19:10:51  swenson
 * CSCdi62987:  stream parameter ignored on x25->tcp trans ...
 * don't send telnet negotiations for pad changes if tcp side is stream
 * Branch: California_branch
 *
 * Revision 3.5.6.5  1996/08/02  18:23:35  swenson
 * CSCdi64955:  pad dispatch timer not working for tcp->pad translations
 * code on same line after #endif is ignored
 * Branch: California_branch
 *
 * Revision 3.5.6.4  1996/06/06  19:46:01  swenson
 * CSCdi59591:  translate user count decremented incorrectly ...
 * Branch: California_branch
 *
 * Revision 3.5.6.3  1996/05/04  01:37:34  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.5.6.2  1996/04/19  15:26:52  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.2.6.6  1996/04/09  21:37:23  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.2.6.5  1996/03/28  09:13:10  che
 * Branch: IosSec_branch
 * Need to initialize kerberos fields in conn_telnet
 *
 * Revision 2.2.6.4  1996/03/19  08:31:33  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.2.6.3  1996/02/21  03:56:23  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.2.6.2  1995/12/17  02:15:46  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.2.6.1  1995/11/17  18:07:17  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.5.6.1  1996/04/04  06:24:51  swenson
 * CSCdi52237:  pad doesnt respond to set or set and read with no
 * parameters
 * Branch: California_branch
 *
 * Revision 3.5  1996/03/06  09:31:31  billw
 * CSCdi44863:  AAA Accounting not done for one-step protocol translation
 * Also ensure tty->ustruct gets set for one-step PT sessions.
 *
 * Revision 3.4  1996/01/29  11:38:45  billw
 * CSCdi46038:  vty access-class takes precedence over translate
 * access-class
 * Always use SYNC_LISTEN for incoming PT tcp sessions, and let the PT
 * process itself do the access check.
 *
 * Revision 3.3  1995/11/20  10:45:30  billw
 * CSCdi42879:  MORE bit handling in PAD code inconsistant
 *
 * Revision 3.2  1995/11/17  19:00:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:22:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/15  23:33:47  eschaffe
 * CSCdi39626:  PT on a PVC connection with "swap" option is terminated
 * after the first connection attempt.
 *
 * Revision 2.6  1995/09/11  21:43:02  tkolar
 * CSCdi40022:  Telnet speed is retained even after the session is closed
 * Move passspeed and maxpassspeed into the userdata struct.
 *
 * Revision 2.5  1995/07/14  07:03:00  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.4  1995/06/20 03:44:57  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.3  1995/06/17  06:26:06  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.2  1995/06/09  13:18:30  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:35:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "address.h"
#include "name.h"
#include "packet.h"
#include "ttysrv.h"
#include "connect.h"
#include "config.h"
#include "../ip/ip.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../tcp/tcp.h"
#include "../tcp/telnet.h"
#include "../x25/x25.h"
#include "../x25/x25_address.h"
#include "../x25/pad.h"
#include "access.h"
#include "translate.h"
#include "../os/async.h"
#include "login.h"
#include "../x25/x25_route.h"
#include "padtcp.h"
#include "pt_tcp_common.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "stacks.h"

extern vectortype PADvector;

/*
 * tcppadBLOCK
 * scheduler test for tcp to pad translator.  Check both connections
 * for significant activity.
 */
boolean tcppadBLOCK (register tt_soc *tty)
{
    conntype *conn = (conntype *)tty->connhead.qhead;

    if (conn_unread(tty->conntop)) {
	if (tty->tty_sessiontimeout)
	    TIMER_START(tty->dallytimer, tty->tty_sessiontimeout);
	return(FALSE);			/* wakeup if input available */
    }
    if (conn_unread(conn)) {
	if (tty->tty_sessiontimeout && (tty->capabilities & OUTIDLE))
	    TIMER_START(tty->dallytimer, tty->tty_sessiontimeout);
	return(FALSE);			/* wakeup if input available */
    }
    if (TIMER_RUNNING_AND_AWAKE(conn->dispatchtimer))
	return(FALSE);
    if (!conn_alive(tty->conntop) || !conn_alive(conn))
	return(FALSE);			/* wakeup if connection is dead */
    if (tty->statbits & CARDROP)
	return(FALSE);			/* wakeup if carrier loss */
    if (TIMER_RUNNING_AND_AWAKE(tty->dallytimer)) {
	tty->statbits |= IDLETIMEOUT;
	return(FALSE);			/* wakeup if idle timeout */
    }
    return(TRUE);
}
/*
 * Code to support a "protocol translator".  Pad or TCP connections
 * come in, and a different type of connection goes out.
 * Minimally parse a host name to connect to (perhaps from call user
 * data or something).
 */

/*
 * tcptopad_daemon
 * process to manage one incoming tcp connection and one outgoing pad
 * connection, passing X.3 type info in both directions, and doing automatic
 * setup of the pad connection.  This is analagous to tty_daemon.
 * Note: Some of the pt_..... helper subroutines are allowed to kill
 * the thread "death()" and might not return to the daemon process.
 */

static forktype tcptopad_daemon (translatetype *trans)
{
    tt_soc *tty = stdio;
    conntype *tcpconn = tty->conntop;
    idbtype *idb = NULL;
    conntype *padconn;
    int n;

    if (translate_debug)
        buginf("\ntcppad%t: fork started", tty->ttynum);

    tcpconn->proto.tn.statechange_check = tcppad_tnoptioncheck;
    tcpconn->proto.tn.statechange_done = tcppad_tnconfirm;

    if ((trans->in.tcp.tcpflags & TRANSF_TCP_PRINTER)) {
	/* we do need to check access now */
	pt_tcp_check_access(trans, tty);
    } else {
	/*
	 * but we only fully establish the tcp connection for non-printer
	 * connections.  (printers are established after outgoing success)
	 */
	pt_setup_tcp_session(trans, tty);
    }
    
    if (x25_enable)
	idb = (idbtype *)pick_x25_idb(trans->out.x25.x121addr,
				      trans->out.x25.cud,
				      strlen((char *)trans->out.x25.cud));
    if (!idb) {
        if (!(trans->flags & TRANSF_GLOB_QUIET)) {
            printf("X.25 is not available.\n");
            startoutput(tty);
	}
	if (translate_debug)
	    buginf("\ntcppad: No X.25 interface present");
	trans->failures++;
	if (!(trans->in.tcp.tcpflags & TRANSF_TCP_PRINTER)) 
	    trans->active--;
	death(tty);                     /* Terminate the connection */
    }

    padconn = pad_open_connection(idb,trans->out. x25.x121addr,
				  trans->flags & TRANSF_GLOB_QUIET ? &n : NULL,
				  FALSE, trans->out.x25.cud[0] 
				  ? trans->out.x25.cud : NULL,
				  strlen((char *) trans->out.x25.cud),
				  trans->out.x25.x25flags,
				  NULL);
    if (padconn == NULL) {
	if (translate_debug)
	    buginf("\ntcppad: can't get pad connection");
	trans->failures++;
	if (!(trans->in.tcp.tcpflags & TRANSF_TCP_PRINTER)) 
	    trans->active--;
	death(tty);
    }

    if (trans->in.tcp.tcpflags & TRANSF_TCP_PRINTER) {
	/* finally finish establishing the tcp connection, etc. */
	pt_setup_tcp_session(trans, tty);
    }

    padconn->acct_info = reg_invoke_aaa_acct_conn(tty->ustruct, NULL,
						  padconn);
    if (trans->flags & TRANSF_GLOB_SWAP) {
	/*
	 * Swap incoming and outgoing connections, and act like a PAD
	 */

	tty->ovector = &PADvector;
	tty->ivector = &PADvector;
	tty->conntop = padconn;
	tty->connhead.qhead = tcpconn;
	run_padtcp(trans, tty);
    } else
	run_tcppad(trans, tty);
    /*
     * connection has died.  Clean up and comit suicide.
     */

    conn_close(padconn);
    conn_close(tcpconn);		/* close connection now */
    tty->statbits &= ~CONNECTED;	/* Not connected anymore */
    tty->statbits |= CARDROP;		/* .... */
    trans->active--;
    death(tty);				/* close down */
    process_kill(THIS_PROCESS);		/* just in case */

}

void run_tcppad (translatetype *trans, tt_soc *tty)
{
    conntype *tcpconn = tty->conntop;
    tcbtype *tcb = (tcbtype *) tcpconn->tcb;
    conntype *padconn= (conntype *)tty->connhead.qhead;
    pcbtype *pcb;
    int n, status;
    long datacount;
    uchar *dataptr;
    paktype *pak;
    uchar *x3params;
    boolean sunhack = TRUE;		/* Support broken SUN telnets */
    uchar echoing;

    tty->statbits |= CONNECTED;
    pcb = (pcbtype *) padconn->tcb;
    x3params = pcb->paramsout;
    if (trans->flags & TRANSF_GLOB_LOCAL) { /* local handling of options ? */
	tcpconn->proto.tn.statechange_check = telnetserver_optioncheck;
	tcpconn->proto.tn.statechange_done = NULL;
	tcpconn->proto.tn.do_kerb_auth = FALSE;
	tcpconn->proto.tn.kerberos_info = NULL;
	padconn->proto.pad.statechange_check = pad_optioncheck;
	telnet_defaults(tcpconn, FALSE);   /* binary already negotiated */
    } else
	padconn->proto.pad.statechange_check = tcppad_padoptioncheck;


    /*
     * If we have a profile we are supposed to use, fake it by
     * pretending we received a SET for those parameters.
     */
    if ((trans->flags & TRANSF_GLOB_SWAP)==0 && trans->out.x25.profile) {
	if (translate_debug)
	    buginf("\ntcppad%t: Loading profile %s", tty->ttynum,
		trans->out.x25.profile->name);
	do_x3list(padconn, trans->out.x25.profile->params,
		  trans->out.x25.profile->length, X3_SETQUIETLY);
	/* save changes to outgoing parameters as our original */
	bcopy(pcb->paramsout,pcb->paramsorig,sizeof(pcb->paramsout));
    } else if ((trans->flags & TRANSF_GLOB_SWAP) && trans->in.x25.profile) {
	if (translate_debug)
	    buginf("\ntcppad%t: Loading profile %s", tty->ttynum,
		trans->in.x25.profile->name);
	do_x3list(padconn, trans->in.x25.profile->params,
		  trans->in.x25.profile->length, X3_SETQUIETLY);
	/* save changes to outgoing parameters as our original */
	bcopy(pcb->paramsout,pcb->paramsorig,sizeof(pcb->paramsout));
    } else {
    
#if 0
	/*
	 * Usually, we will be talking to a hos or PAD that will download us
	 * with a SET command with the appropriate parameters.  Sometimes,
	 * however, we will end up talking to another pad, and must READ
	 * the parameters that it wants from it.  So we send a READ command.
	 * If we get back an INDICATION before we get a SET, we will set
	 * our local parameters to thse values.
	 */
	pcb->flags |= PCB_SETPARAM|PCB_CTLREADY; /* will listen to INDICATE */
	(void) pad_putbyte(pcb,X3_READ,TRUE);	/* read other pads params */
	pad_putcontrol(pcb);
#endif
    }

    /*
     * We now have a connection going in each direction, one TCP, and one
     * PAD.  Shovel bytes both ways, and translate state changes.
     */
    while (TRUE) {
	edisms((blockproc *)tcppadBLOCK,(long)tty);
	GET_TIMESTAMP(padconn->idletime);
	if (tty->statbits & (IDLETIMEOUT + CARDROP))
	    break;
	/* if closed and no pending input */
	if (!conn_alive(tty->conntop)) { 
	    if (translate_debug)
		buginf("\ntcppad%t: conntop not alive",tty->ttynum);
	    break;			/* then close down */
	}

	/*
	 * read from pad, write to tcp
	 */
	if (pcb->state != PAD_ESTABLISHED) {
	    if (translate_debug)
		buginf("\ntcppad%t: pad state %x not alive",
		       tty->ttynum, pcb->state);
	    break;
	}

	n = 400;
	if (pad_unread(padconn) || pad_closing(pcb)) {
	    while (n > 0) {
	        status = pad_getstring(pcb, &dataptr, &datacount, MAXINT);
 		if (status == NO_DATA)
		    break;
	        if (status == OK) {
		    n -= datacount;
		    if (pcb->flags & PCB_DISCARD)
			continue;	/* don't actually print any data */
		    while (datacount--) {
			if (tcpconn->status & CONN_STREAM) {
			    int bytesgone = tcp_putstring((tcbtype *)
							  tcpconn->tcb,
							  dataptr, datacount);
			    datacount -= bytesgone-1;
			    if (datacount) { /* let tcp_putbyte block */
				dataptr += bytesgone;
				tcp_putbyte((tcbtype *)tcpconn->tcb,
					    tty->tty_datamask & (*dataptr++),
					    TRUE);
				datacount--;
			    }
			} else
			    if (!tnof(tcpconn, *dataptr++ & tty->tty_datamask,
				      TRUE, TRUE))
				break;
		    }
		} else
		    return;
	    } /* while */
	    if (tcpconn->proto.tn.tn_option & STO_SENDEOR) {
		if (pcb->flags & PCB_EORIN) {
		    tcp_putbyte(tcb, TN_IAC, TRUE);	/* send IAC... */
		    tcp_putbyte(tcb, TN_EOR, TRUE);	/* ... EOR */
		    (void) tcp_putpush(tcb, TRUE, FALSE);
		    pcb->flags &= ~PCB_EORIN;
		    if (translate_debug)
			buginf("\ntcppad%t: Sent telnet EOR", tty->ttynum);
		}
	    } else
	        (void) tcp_putpush(tcb,TRUE,FALSE); /* make output run */
	} /* if */

	/*
	 * read from tcp, write to pad.
         */

	n = 128;
	if (tcp_unread(tcb) || tcp_closing(tcb)) {
	    echoing = x3params[X3_LOCALECHO];
	    if (echoing && (tcpconn->proto.tn.tn_option & STO_ECHO) == 0)
		x3params[X3_LOCALECHO] = 0;	/* only one protocol echos */
	    while (--n > 0) {
		pak = pcb->curoutputpak;
		if (!pak) {
		    if (pad_putsetup(pcb))
			pak = pcb->curoutputpak;
		    else break;		/* Can't get a buffer */
		}
		TIMER_STOP(padconn->dispatchtimer);
		status = x3_getline(pak, x3params, (int(*)(ulong))tcppad_getc,
				    (ulong)tcpconn, tty);
#if VERBOSEPAK
		if (pad_debug) {
		    char *foo = "unknown";
		    if (status == X3_FULL)
			foo = "full";
		    else if (status == X3_DONE)
			foo = "dispatch";
		    else if (status == X3_BREAKLINE)
			foo = "BREAK";
		    else if (status == X3_BLOCK)
			foo = "noMore";
		    buginf("\nTCPPAD%t: TCP read: %d bytes, last=%d, "
			   "status= %s", tty->ttynum,
			   pak->unspecified[0] - pak->length,
			   *(pak->dataptr - 1), foo);
		}
#endif
		if (status == X3_BLOCK) {
		    if (x3params[X3_IDLETIME] == 0 || x3params[X3_EDIT] != 0)
		        break;		/* don't send packets */
		    if (x3params[X3_IDLETIME] != 1) {
			if (!TIMER_RUNNING(padconn->dispatchtimer)) {
			    TIMER_START(padconn->dispatchtimer,
					x3params[X3_IDLETIME]*ONESEC/20);
			    break;	/* Don't send packet yet */
#if VERBOSEPAK
			    if (pad_debug)
				buginf(" (dispatch+%d)",
				       x3params[X3_IDLETIME]*ONESEC/20);
#endif
			} else if (SLEEPING(padconn->dispatchtimer))
			    break;	/* don't send packet yet! */
		    }
		    /* Else send a packet now! */
		    n = 0;
		}

		/* If the packet is FULL and its not the last packet,
		 * set the M bit 
		 */

		if ((status == X3_FULL) && (tcp_unread(tcb))) {
#if VERBOSEPAK
		    if (pad_debug)
			buginf(" (Set-Mbit)");
#endif
	            pak->flags |= PAK_MBIT;
		}	
		/*
		 * Support broken SUN telnets.  These will use LF instead
		 * of CRLFas a line terminator when operated in single
		 * character local echo mode (WILL SUPRGA, WONT ECHO).
		 * to correct for this, we check whether we have ever
		 * seen a CR in this mode.  If not, we translate LFs
		 * into CRs.  Note that LF must be a dispatch char.
		 */
		if (sunhack) {
		    if ((tcpconn->proto.tn.tn_option & (STO_SUPRGA|STO_ECHO)) 
			== STO_SUPRGA) {
			if (*(pak->dataptr-1) == RETURN) {
			    sunhack = FALSE;
			    if (translate_debug)
				buginf("\npad: saw CR.  Sun hack off.");
			}
			else if (*(pak->dataptr-1) == LINEFEED) {
			    (*(pak->dataptr-1) = RETURN);
			    if (translate_debug)
				buginf("\npad: converting LF to CR (Sun hack)");
			}
		    }
		}
#if VERBOSEPAK
		if (pad_debug)
		    buginf(" (send))");
#endif
		pad_putpush(pcb, FALSE);	/* ensure output is running */
		if (status == X3_BREAKLINE) {
		    int disc = 0;
		    int breakopt = x3params[X3_BREAK];
		    if (breakopt & X3_BREAKDIS) {
			disc = 1;
			pcb->flags |= PCB_DISCARD;
			pcb->paramsout[X3_DISCARD] = 1;
			clearoutput(tty);
		    }			
		    if (breakopt & X3_BREAKINT)
			pad_interrupt(pcb);
		    if (breakopt & X3_BREAKIND) {
			pad_option_emit(pcb,X3_BREAKRECEIVED,X3_DISCARD,disc);
			pad_putcontrol(pcb);
		    }
		}
	    } /* while */
	    x3params[X3_LOCALECHO] = echoing;	/* restore */
	} /* if */
	    if (TIMER_RUNNING_AND_AWAKE(padconn->dispatchtimer)) {
#if VERBOSEPAK
		if (pad_debug)
		    buginf(" (send2)");
#endif		
		pad_putpush(pcb,FALSE);		/* time to send packet ! */
	    }
    } /* while */
}

/*
 * tcppad_getc
 * Get a character from tcp for tcp-to-pad translation.
 */
int tcppad_getc (conntype *conn)
{
    int ch, status;

    if (conn->status & CONN_STREAM)
	return(tcp_getbyte((tcbtype *)conn->tcb,&status));
    else while (TRUE) {
	ch = tcp_getbyte((tcbtype *)conn->tcb,&status);
	if (conn->tty->capabilities & (SFLOWIN|SFLOWOUT))
	    if (ch == conn->tty->tty_startchar ||
		ch == conn->tty->tty_stopchar)
		continue;
	if (ch == -1)
	    return(ch);
	ch = tnif(conn,ch);
	if (ch != -1)		/* if Telnet ate character, try again */
	    break;
    }
    /*
     * Preserve any status bits that may have meaning.  This code is
     * slightly faster than checking for the status bits, but it does
     * require that the MSB of the low byte never have meaning, since
     * that bit can get masked out even when there are status bits.
     */
    return(ch & (0xFFFFFF00 |(int)conn->tty->tty_datamask));
}

/*
 * tcppad_tnoptioncheck
 * Check whether a telnet option state change is allowed for a protocol
 * translation session (tcp to pad connection).
 */

boolean tcppad_tnoptioncheck (conntype *conn, uchar code)
{

    conntype *pcbconn = (conntype *) conn->tty->connhead.qhead;
    pcbtype *pcb = (pcbtype *) pcbconn->tcb;

    switch (code) {

	case TNO_BINARY:
	    return(TRUE);

	case TNO_NAWS:
	    return(TRUE);

	/*
	 * Line used in milking machine applications can be set up to
	 * refuse the Supress GA option, which will cause a good telnet
	 * to stay in line mode.
	 */
	case TNO_SUPRGA:
	    if ((pcb->paramsout[X3_EDIT] != 0) && 
		conn->proto.tn.lastcmd == TN_DO)
		if ((pcb->paramsout[X3_DISPMASK] == 2) ||
		    (pcb->paramsout[X3_DISPMASK] == 0))
		    return(FALSE);
	    return(TRUE);

	case TNO_SENDEOR:
	    if (conn->proto.tn.lastcmd == TN_DO || 
		conn->proto.tn.lastcmd == TN_DONT)
		return(TRUE);
	    break;

	case TNO_TTYSPEED:
	    if (conn->tty->tty_passspeed)
		return(TRUE);
	    break;

	case TNO_TIMEMARK:
	    if (conn->proto.tn.lastcmd == TN_WILL || 
		conn->proto.tn.lastcmd == TN_WONT)
		conn->proto.tn.tn_state &= ~ST_FLUSH;
	    break;		

	  case TNO_TTYTYPE:
            return(TRUE);

	  default:
	    break;
    }
    return(FALSE);
}

/*
 * tcppad_padoptioncheck
 * Check whether an X.3 option state change is allowed for a protocol
 * translation session (tcp to pad connection).
 */
boolean tcppad_padoptioncheck (conntype *conn, int param, int value)
{
    tt_soc *tty = conn->tty;
    conntype *tcpconn = tty->conntop;
    pcbtype *pcb = (pcbtype *) conn->tcb;
    boolean success = TRUE;

    if (pcb->paramsout[param] == value)	/* No change in value ? */
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
	    if (value != 0) {
		if (conn->debug)
		    printf("(ignored)");
		success = FALSE;
	    } else
		success = TRUE;
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
		send_option(tcpconn, STO_SUPRGA, 0, TN_WONT, TNO_SUPRGA);
	    if (translate_debug)
		buginf("\ntcppad%t: Sending WONT SUPRGA", tty->ttynum);
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
		    send_option(tcpconn, STO_SUPRGA, 0, TN_WONT, TNO_SUPRGA);
		    if (translate_debug)
			buginf("\ntcppad%t: Sending WONT SUPRGA", tty->ttynum);
		} else {
		    send_option(tcpconn, STO_SUPRGA, STO_SUPRGA, TN_WILL,
			TNO_SUPRGA);
		    if (translate_debug)
			buginf("\ntcppad%t: Sending WILL SUPRGA", tty->ttynum);
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
		if (send_option(tcpconn, STO_ECHO, 0, TN_WONT, TNO_ECHO))
		    if (translate_debug)
			buginf("\ntcppad%t: Sending WONT ECHO", tty->ttynum);

	    } else		/* request remote or no echo */
		if (send_option(tcpconn, STO_ECHO, STO_ECHO, TN_WILL, 
				TNO_ECHO))
		    if (translate_debug)
			buginf("\ntcppad%t: Sending WILL ECHO", tty->ttynum);
	    break;

	case X3_CRPAD:
	case X3_LFPAD:
	    break;

	case X3_FLOWIN:
	    if (value)
		tty->capabilities |= SFLOWIN;
	    else
		tty->capabilities &= ~SFLOWIN;
	    setsflow(tty);
	    break;

	case X3_FLOWOUT:
	    if (value)
		tty->capabilities |= SFLOWOUT;
	    else
		tty->capabilities &= ~SFLOWOUT;
	    setsflow(tty);
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
	    if (conn->debug) printf(padques);
	    break;
    }
    /* ensure output is running */
    tcp_putpush((tcbtype *)tcpconn->tcb,TRUE,FALSE);	
    return(success);
}

/*
 * tcppad_tnconfirm
 * Come here when the telnet client responds to our request for option
 * change.  The only thing that needs to handled specially is echoing.
 * Everything else, it won't matter whether both the translator and the
 * telnet client do the same thing..
 */
void tcppad_tnconfirm (conntype *conn, ulong bit, ulong state, ulong code)
{
#if 0
    if (code == TNO_ECHO && bit == STO_ECHO && state == 0) {
	conntype *pcbconn = (conntype *) conn->tty->connhead.qhead;
	pcbtype *pcb = (pcbtype *) pcbconn->tcb;
	pcb->paramsout[X3_LOCALECHO] = 0;
	if (translate_debug)
	    buginf("\ntcppad%t: Turning off intermediate echoing",
		   conn->tty->ttynum);
    }
#endif
}

/*
 * tcp_transto_x25
 * Translate TCP input to X.25 output
 * Returns PID of translation process or -1 if failure
 * Called by SERVICE_TRANS_TCPINPUT, index is TRANS_X25
 */

int tcp_transto_x25 (translatetype *trans, tt_soc *tty)
{
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    if (reg_invoke_kerberos_is_running())
	stacksize = (ulong) EXEC_STACK_SIZE;
    return(cfork((forkproc *)tcptopad_daemon, (long)trans, stacksize,
		 "TCP to PAD", tty->ttynum));
}
