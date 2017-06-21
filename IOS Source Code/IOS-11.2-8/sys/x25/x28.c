/* $Id: x28.c,v 3.4.28.7 1996/08/28 13:23:51 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x28.c,v $
 *------------------------------------------------------------------
 * x28.c -- X25 Packet assembler/disassembler user interface.
 *
 * November 1987,  Bill Westfield
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x28.c,v $
 * Revision 3.4.28.7  1996/08/28  13:23:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.28.6  1996/07/12  18:17:36  swenson
 * CSCdi60860:  Busy-message not working for pad calls
 * provide a switch to make pad calls quiet
 * Branch: California_branch
 *
 * Revision 3.4.28.5  1996/06/28  23:33:02  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.28.4  1996/06/12  14:43:56  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.4.28.3  1996/05/23  23:22:07  swenson
 * CSCdi58587:  crash when user specifies profile with pad command
 * fix crash introduced by CSCdi52237
 * Branch: California_branch
 *
 * Revision 3.4.28.2  1996/04/02  19:24:59  swenson
 * CSCdi52237:  pad doesnt respond to set or set and read with no
 * parameters
 * Branch: California_branch
 *
 * Revision 3.4.28.1  1996/03/18  22:50:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.1  1996/03/07  11:20:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/22  07:46:47  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/20  10:45:34  billw
 * CSCdi42879:  MORE bit handling in PAD code inconsistant
 *
 * Revision 3.2  1995/11/17  18:08:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  21:37:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/09/27  22:45:40  eschaffe
 * CSCdi41170:  Resetting a PAD connection on a cleared LCI can cause a
 * reload
 *
 * Revision 2.9  1995/08/08  16:51:29  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.8  1995/07/24 23:55:57  eschaffe
 * CSCdi36312:  Memory leak in Middle and Big buffers
 *
 * Revision 2.7  1995/07/20  07:38:34  hampton
 * Convert X.25 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37392]
 *
 * Revision 2.6  1995/07/20 00:27:03  eschaffe
 * CSCdi34009:  session-timeout doesnt work for outgoing PAD connections
 *
 * Revision 2.5  1995/07/14  07:03:35  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.4  1995/06/20 03:45:18  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.3  1995/06/10  00:05:11  eschaffe
 * CSCdi35298:  Make sure all buginf messages are enabled by
 * "translate_debug" or "pad_debug" boolean.
 *
 * Revision 2.2  1995/06/09  13:25:35  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:23:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "address.h"
#include "packet.h"
#include "ttysrv.h"
#include "connect.h"
#include "config.h"
#include "../ui/debug.h"
#include "x25.h"
#include "pad.h"
#include "pad_debug.h"
#include "../pt/translate.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "../tcp/telnet.h"
#include "name.h"
#include "logger.h"
#include "../x25/msg_pad.c"		/* Not a typo, see logger.h */
#include "x25_switch.h"
#include "x25_route.h"
#include "access.h"
#include "x25_proto.h"
#include "registry.h"
#include "../h/auth_registry.regh"

#define MAXCHRS 80
#define MAXBYTES 400		/* resched after this many characters */

/*
 * This module handles the interface between the 'PAD' and the user's
 * terminal.  Note that this includes things not normally included in
 * the actual X.28 recomendation, since our user interface is considerably
 * different.  The PAD equivilent of CONNECT and TELNET are included here.
 */

/*  Things to think about:
 *
 * a Full X.28 compatable user interface ?
 */

/* Local Storage */
queuetype pcbqueue;		/* list of all PCBs currently in use */


/*
 * pad_connect_command
 * Parse the pad connect command.  Get an x.121 address and a debug option.
 */

void pad_connect_command (parseinfo *csb)
{
    register conntype *newcon;
    pcbtype *pcb = NULL;
    addrtype x121address;
    idbtype *idb;
    nametype *ptr;
    profiletype *profile = NULL;
    int debug = GETOBJ(int, 1);
    uchar *cud = (uchar *)GETOBJ(string, 2);
    uchar *profile_str = (uchar *)GETOBJ(string, 3);
    uint cudlen = strlen(cud);
    int reverse = 0;
    int quiet = GETOBJ(int, 5);	
    int errptr;

    if (!connect_allowed(&pad_connvector))
      return;			/* Ensure pad connections allowed here */

    /*
     * Check the x.121 address string
     */

    ptr = name_cache_lookup(GETOBJ(string, 1), NULL, NAM_PAD);
    memset(&x121address, 0, sizeof(addrtype));
    if (ptr) {
	x121address.type = ADDR_X121;
	x121address.length = strlen(ptr->v.x121.adr[0].addr.x121_address);
	bcopy(ptr->v.x121.adr[0].addr.x121_address, x121address.x121_addr,
	      x121address.length);
	cudlen = strlen((char *)ptr->v.x121.userdata);
	bcopy(ptr->v.x121.userdata, cud, cudlen);
    } else {
	if (t_number(GETOBJ(string, 1)) && strlen(GETOBJ(string, 1)) < 16) {
	    x121address.type = ADDR_X121;
	    x121address.length = strlen(GETOBJ(string,1));
	    bcopy(GETOBJ(string,1), x121address.x121_addr, x121address.length);
	}
    }

    if (GETOBJ(int,3)) {
	reverse |= TRANSF_X25_REVERSE;
    }
    /*
     * pad <x121addr> /profile <string>
     */
    if (profile_str[0] != '\0') {
        profile = (profiletype *)pad_find_profile(profile_str);
        if (!profile) {
            printf("\n%%Unknown profile name \"%s\"", profile_str);
            return;
        }
    }

    if (x121address.type != ADDR_X121) {
	printf("Invalid X.121 address.");
	return;
    }

    idb = pick_x25_idb(x121address.x121_addr, cud, cudlen);

    if (!idb) {
	printf("X.25 is not available.");
	return;
    }
    /*
     * Now we try to connect to the remote host.
     */
    newcon = pad_open_connection(idb, x121address.x121_addr, 
				 quiet ? &errptr: NULL, debug,
				 cud, cudlen, reverse, NULL);
    if (!newcon) {
	return;
    }

    if (quiet) {
        newcon->status |= CONN_QUIET;
    }

    /*
     * If we have a profile we are supposed to use, fake it by
     * pretending we received a SET for those parameters.
     * pad <x121addr> /profile <string>
     */
    if (profile) {
	pcb = (pcbtype *) newcon->tcb;
	if (pad_debug) {
            buginf("\nPAD%t: Loading profile %s", pcb->ttynum, profile->name);
	}
        do_x3list(newcon, profile->params, profile->length, X3_SETQUIETLY);
	/* save changes to outgoing parameters as our original */
	bcopy(pcb->paramsout,pcb->paramsorig,sizeof(pcb->paramsout));
    }
    newcon->proto.pad.statechange_check = pad_optioncheck;

    connect_new_session(newcon, GETOBJ(string, 1), csb);
		    	/* Start this new session */

}

/*
 * pad_multiproto_open - Handle session open for the connect command
 *
 * This routine is used to attempt to open a PAD session when a target
 * protocol has not been specified. If the supplied name is either a valid
 * X.121 address or a defined PAD name, we make a connect.
 */

conntype *pad_multiproto_open (parseinfo *csb, boolean *abortflag)
{
    nametype *ptr;
    addrtype x121address;
    idbtype *idb;
    conntype *newcon;
    uint cudlen = 0;
    uchar *cud = NULL;

    /*
     * Check the x.121 address string
     */

    ptr = name_cache_lookup(GETOBJ(string, 1), NULL, NAM_PAD);
    memset(&x121address, 0, sizeof(addrtype));
    if (ptr) {
	x121address.type = ADDR_X121;
	x121address.length = strlen(ptr->v.x121.adr[0].addr.x121_address);
	bcopy(ptr->v.x121.adr[0].addr.x121_address, x121address.x121_addr,
	      x121address.length);
	cudlen = strlen((char *)ptr->v.x121.userdata);
	cud = ptr->v.x121.userdata;
    } else {
	if (t_number(GETOBJ(string, 1)) && strlen(GETOBJ(string, 1)) < 16) {
	    x121address.type = ADDR_X121;
	    x121address.length = strlen(GETOBJ(string,1));
	    bcopy(GETOBJ(string,1), x121address.x121_addr, x121address.length);
	}
    }

    if (x121address.type != ADDR_X121)
	return(NULL);

    idb = pick_x25_idb(x121address.x121_addr, NULL, 0);
    if (!idb)
	return(NULL);

    newcon = pad_open_connection(idb, x121address.x121_addr, NULL, FALSE,
				 cud, cudlen, FALSE, abortflag);

    if (!newcon)
      return(NULL);

    newcon->proto.pad.statechange_check = pad_optioncheck;

    return(newcon);		/* Return new connection pointer */
}

/*
 * pad_parse_resume
 * parse arguments to a resume command for a PAD connection.
 * These are not necessarilly the same as commands for other connections
 * Leave the results in the connection block and pcb, as appropriate.
 */
void pad_parse_resume (
    register conntype *conn, 
    register parseinfo *csb)
{
    pcbtype *pcb = (pcbtype *) conn->tcb;

    if (GETOBJ(int,2)) {		/*  resume /debug  */
	conn->debug = TRUE;
    }
    if (GETOBJ(int,3)) {		/*  resume /nodebug  */
	conn->debug = FALSE;
    }
    if (GETOBJ(int,4)) {		/*  resume /line  */
	pcb->paramsout[X3_EDIT] = 1;
	pcb->paramsout[X3_IDLETIME] = 0;
    }
    if (GETOBJ(int,5)) {		/*  resume /noline  */
	pcb->paramsout[X3_EDIT] = 0;
	pcb->paramsout[X3_IDLETIME] = 1;
    }
    if (GETOBJ(int,6)) {		/*  resume /echo  */
	pcb->paramsout[X3_LOCALECHO] = 1;
    }
    if (GETOBJ(int,7)) {		/*  resume /noecho  */
	pcb->paramsout[X3_LOCALECHO] = 0;
    }
    if (GETOBJ(int,10)) {		/*  resume /set <x3pair>  */
	pcb->paramsout[GETOBJ(int,11)] = GETOBJ(int,12);
    }
}

/*
 * parse_x3pair
 * parse an x.3 parameter pair that looks sort of like 12:33.  Update
 * the value in the parameter array passed.  Return success or failure.
 */
boolean parse_x3pair (char *ptr, uchar array[])
{
    int param, val;

    param = val = 0;
    if ((ptr = octet(ptr,(ipaddrtype *)&param,':')) &&
	(ptr = octet(ptr,(ipaddrtype *)&val,'\0')))
	if (param < NX3PARAMETERS) {
	    array[param] = val;
	    return(TRUE);
	}
    return(FALSE);
}

/*
 * pad_open_connection
 * Actually open a X25 Pad connection.  Set up the conntype pointer,
 * send the call request packet and listen for the answer, and so on.
 */
conntype *pad_open_connection (
    idbtype *idb,	  /* interface to use */
    uchar *x121address,	  /* x.121 address to connect to */
    int *errptr,          /* if null, print errors, else return code */
    boolean debug,	  /* debug control packets ? */
    uchar *cud,
    uint cudlen,
    boolean reverse,
    boolean *abortflag)
{
    int status;
    pcbtype *pcb;
    lcitype *lci;
    conntype *conn;

    /*
     * Set up the connection block.
     */ 
    conn = getconnection(stdio, &pad_connvector);
    pcb = (pcbtype *) getpcb(stdio);
    if (!conn || !pcb) {
	if (conn)
	    freeconnection(conn);
	if (pcb)
	    freepcb(pcb);	
	if (!errptr)
	    printf(nomemory);
	else
	    *errptr = NO_MEMORY;
	return(NULL);
    }
    conn->debug = debug;
    conn->tcb = pcb;
    pcb->pcb_conn = conn;

    /*
     * Print out message indicating what we're doing
     */
    if (!errptr)
        printf("Trying %s...",x121address);
    flush();

    /*
     * Perform access checking on the current address.
     */
    if (!pad_check_access(stdio,x121address)) {
	if (!errptr) {
	    printf("connections to %s",x121address);
	    printf(" not permitted from this terminal");	
	}
	freepcb(pcb);
	freeconnection(conn);
	return(NULL);
    }

    lci = x25_open(idb, pcb, x121address, cud, cudlen, reverse, &status);
    if (!lci) {
	if (!errptr)
	    printf("\n%% %s\n", tcperror2string(status));
	else
	    *errptr = status;
	freepcb(pcb);
	freeconnection(conn);
	if ((status == LOCAL_RESET) && (abortflag))
	    *abortflag = TRUE;
	return(NULL);
    }

    /*
     * We succeeded.  Finish connection setup.
     */
    pcb->flags |= PCB_INITED;	/* doesn't need pad_defaults set */
    if (!errptr)
	printf("Open\n");
    sstrncpy(conn->hostname, (char *)x121address, CONNLEN+1);
    (void)startoutput(stdio);		/* ensure output is running */
    return(conn);
}

/*
 * padcloseBLOCK
 * After we send an inviation to clear packet, wait a reasonable amount
 * of time for the other side to clear the connection, then clear the call.
 */
int padcloseBLOCK (pcbtype *pcb)
{
    if (pcb->state != PAD_CLOSING)
	return(FALSE);
    return (SLEEPING(pcb->dallytime));
}
/*
 * pad_close
 * After a connection is closed, clean up the pcb and so on.
 */
void pad_close (conntype *conn)
{
    pcbtype *pcb = (pcbtype *) conn->tcb;

    reg_invoke_aaa_acct_end(conn->tty->ustruct, OK, conn->acct_info,
			    pcb->bytesincount, pcb->bytesoutcount,
			    pcb->pakincount, pcb->pakoutcount);
    if ((pcb->state == PAD_ESTABLISHED) && (pcb->pcb_lci != NULL)){
	pad_putpush(pcb, FALSE);
	if (!IS_LCI_PVC(pcb->pcb_lci->lci_idb->hwptr, pcb->pcb_lci->lci_lci)) {
	    if (x29_inviteclear_time) {
		if (pad_debug)
		    buginf("\nPAD%t: sending invite clear", pcb->ttynum);
		pcb->flags |= PCB_CTLREADY;
		if (!pad_putclear(pcb))
		    if (pad_debug)
			buginf("!!Failed!!");
		pad_putcontrol(pcb);	/* Send invitation to clear */
		pcb->state = PAD_CLOSING;
		/* wait for output to finish */
		TIMER_START(pcb->dallytime, x29_inviteclear_time*ONESEC); 
	    } else {
		TIMER_START(pcb->dallytime, 2*ONESEC);
	    } 
	    edisms((blockproc *)padcloseBLOCK, (ulong)pcb);
	}
    }
    if (pcb->state != PAD_CLEARED) {
	if (pcb->state != PAD_RESTART && pcb->pcb_lci != NULL) {
	    if (IS_LCI_PVC(pcb->pcb_lci->lci_idb->hwptr, 
			   pcb->pcb_lci->lci_lci)) {
		x25_sendcause(pcb->pcb_lci->lci_idb->hwptr,
			      pcb->pcb_lci, X25_RESET,
			      X25_RESET_NETWORK_OUT_OF_ORDER,
			      X25_DIAG_LINK_OUT_OF_ORDER, NULL);
		process_sleep_for(ONESEC);
		x25_call_remove(pcb->pcb_lci, 0, 0);
	    } else
		x25_sendcause(pcb->pcb_lci->lci_idb->hwptr,
			      pcb->pcb_lci, X25_CLEAR,
			      X25_CLEAR_DTE_ORIGINATED, X25_DIAG_NO_INFO, 
			      NULL);
	}
    }

    freepcb(pcb);
    freeconnection(conn);
}

/*
 * x25_open
 * send a call request packet and wait for the connection to open up,
 * or for the user to abort the connection.
 */
lcitype *x25_open (
    idbtype *idb,
    pcbtype *pcb,
    uchar *x121address,
    uchar *cud,
    uint cudlen,
    boolean reverse,
    int *status)
{
    lcitype *lci;
    addrtype addr;
    register leveltype level;

    memset(&addr, 0, sizeof(addrtype));
    sstrncpy(addr.x121_addr, x121address, sizeof(addr.x121_addr));
    addr.length = strlen(addr.x121_addr);
    addr.type = ADDR_X121;

    level = raise_interrupt_level(NETS_DISABLE);
    lci = x25_call_setup(idb, NULL, &addr, NULL,
			 LINK_PAD, X25_UNASSIGNED_PREC, cud, cudlen, reverse);
    reset_interrupt_level(level);
 
    if (lci == NULL) {
	*status = UNREACHABLE;
	return((lcitype *) NULL);
    }
    pcb->pcb_lci = lci;
    lci->lci_pcb = pcb;
    pcb->state = PAD_CALLING;
    psion((tt_soc *)pcb->tty);

    edisms((blockproc *)pad_openBLOCK, (ulong)pcb);

    if (pcb->state == PAD_ESTABLISHED) {	/* connection successful */
	*status = OK;
	psioff((tt_soc *)pcb->tty);
	return(lci);
    }

    /*
     *  connection failed--disassociate the PCB from the LCI
     *  unless pcb->pcb_lci is NULL, in which case it was deleted
     *  out from under us, perhaps via a call collision
     */

    if (pcb->pcb_lci == NULL) {
	lci = NULL;
    } else {
	pcb->pcb_lci = NULL;
	lci->lci_pcb = NULL;
    }

    if (psiset((tt_soc *)pcb->tty)) {		/* user reset the connection */
	*status = LOCAL_RESET;
	/* 
	 * The CALL Request did not succeed.
	 * Do not send a Clear Request on a Cleared or Restarted LCI
	 */
	if ((pcb->state != PAD_CLEARED) && (pcb->state != PAD_RESTART) && lci) {
	    x25_sendcause(idb->hwptr, lci, X25_CLEAR,
			  X25_CLEAR_DTE_ORIGINATED, X25_DIAG_NO_INFO, NULL);
	}
    } else
	*status = REJECTED;
    psioff((tt_soc *)pcb->tty);
    return(NULL);
}

/*
 * getpcb
 * allocate and initialize a Pad Control Block.
 */
pcbtype *getpcb (tt_soc *tty)
{
    pcbtype *pcb;

    pcb = malloc_named(sizeof(pcbtype), "PAD CB");
    if (pcb == NULL)
	return(NULL);
    /* 
     * Initialize to "blank" template state.  Since malloc clears memory,
     * we don't need to initialize zero fields or NULL pointers.
     */
    pcb->tty = tty;
    pcb->ttynum = tty->ttynum;
    pcb->state = PAD_CLOSED;
    pcb->pcb_lsterr = OK;
    queue_init(&pcb->inqueue,PAD_QUEUESIZE);
    enqueue(&pcbqueue, pcb);
    pad_x3init(pcb,tty);		/* set initial X3 parameters */
    return(pcb);
}

void freepcb (pcbtype *pcb)
{
    leveltype status = raise_interrupt_level(ALL_DISABLE);

    unqueue(&pcbqueue, pcb);
    if (pcb->pcb_lci) {
	pcb->pcb_lci->lci_pcb = NULL;
        pcb->pcb_lci = NULL;
    }
    if (pad_debug)
	buginf("\nPAD%t: Closing connection to %s.  In %d/%d, out %d/%d",
	       pcb->ttynum, pcb->pcb_conn ? pcb->pcb_conn->hostname : "",
	       pcb->bytesincount, pcb->pakincount,
	       pcb->bytesoutcount, pcb->pakoutcount);
    if (pad_debug)
	if (!QUEUEEMPTY(&pcb->inqueue))
	    buginf("\nfreepcb%lx: packets on input queue",pcb);

    while (!QUEUEEMPTY(&pcb->inqueue))
	datagram_done(dequeue(&pcb->inqueue));
    datagram_done(pcb->curinputpak); pcb->curinputpak = NULL;
    datagram_done(pcb->curoutputpak); pcb->curoutputpak = NULL;
    if (pcb->tty)		/* maybe do CARDROP on vty's */
	(void) modemcheck((tt_soc *)pcb->tty);
    free(pcb);
    reset_interrupt_level(status);
}


/*
 * pad_check_access
 * This could be more complex...   It should take into account the
 * terminal, the address being connected to, the interface, and the type
 * of x25 call (eg reverse charging or not).  It has potential to be icky.
 */
boolean pad_check_access (tt_soc *tty, uchar *x121address)
{
    return(regexp_accesscheck(x121address, tty->accessout, x29_accesslists));
}

/*
 * pad_telnet
 * Perform pad protocol action on the specified connection.
 * Returns I/O status code.
 */
int pad_telnet (register conntype *conn, boolean resumeflag)
{
    register pcbtype *pcb;
    register tt_soc *tty;
    register int n;			/* fairness counter */
    register boolean pushflag;		/* non-zero if must send to net */
    int status;				/* status codes */
    long datacount;
    uchar *dataptr;

    tty = conn->tty;			/* set up tty pointer */
    pcb = (pcbtype *) conn->tcb;	/* set up pcb pointer */
    clearnotices (tty);			/* reset notification bits */
    TIMER_START(conn->notetime, 30*ONESEC);/* init notify timer */
    TIMER_STOP(conn->logintimer);	/* this timer not used yet */
    GET_TIMESTAMP(conn->idletime);	/* no idle time yet */
    if (tty->tty_sessiontimeout) {
	start_sessiontimeout(conn, tty);
    }
    else
        TIMER_STOP(conn->sessiontimer); /* no session time out yet */
    tty->statbits &= ~IDLETIMEOUT;	/* ... */
    pushflag = FALSE;			/* not pushing data yet */
    if (pcb->pakoutcount != 0) {	/* resumeing a connection ? */
	if (pcb->paramsout[X3_FLOWOUT])
	    tty->capabilities |= SFLOWOUT;
	else tty->capabilities &= ~SFLOWOUT;
	if (pcb->paramsout[X3_DISCARD] == 0)
	    pcb->flags &= ~PCB_DISCARD;
    }

    while (TRUE) {
	edisms ((blockproc *)padBLOCK, (ulong)conn);  /* block until action */

	if (conn->status & CONN_CHECKALL) {
	    conn->status &= ~CONN_CHECKALL;
	    /*
	     * Check tty driver events:
	     *  - Session timeout occured.
	     *  - return if escape character typed
	     *  - return if carrier off
	     *  - do notifications if timer has expired.
	     *  - magic fingers
	     */
	    if (tty->tty_sessiontimeout)
		(void) conn_session(conn, FALSE);
	    if (tty->statbits & IDLETIMEOUT)
	        return(IDLE_TIMEDOUT);
	    if (psiset(tty)) {
		return(OK);
	    }
	    if (modemcheck (tty))
	        return(CARRIER_DROP);
	    if (!SLEEPING(conn->notetime)) {
	        notification (conn);
		TIMER_START(conn->notetime, 30*ONESEC);
	    }
	}
	/*
	 * Handle input from the net (output to the tty)
 	 */
	if (pcb->state != PAD_ESTABLISHED) {
	    switch (pcb->state) {
	      case PAD_CLEARED:
		return(END_OF_FILE);
	      case PAD_RESET:
		if (conn->debug)
		    printf("[RESET]");
		pcb->state = PAD_ESTABLISHED;
		break;
	      case PAD_RESTART:
		return(ABORTED);
	      default:
		if (pad_debug)
		    buginf("\nPAD%t: Unknown state %d",
			   pcb->ttynum,pcb->state);
		pcb->state = PAD_ESTABLISHED;
		break;
	    }
	}
	n = MAXBYTES;
	if (pad_unread(conn) || pad_closing(pcb)) {
	    while (n > 0) {
	        status = pad_getstring(pcb, &dataptr, &datacount, MAXINT);
 		if (status == NO_DATA)
		    break;
		n -= datacount;
	        switch (status) {
		  case OK:
		    if (pcb->flags & PCB_DISCARD) {
			if (conn->debug) printc('.');
			continue;	/* don't actually print any data */
		    }
		    status = putpkt(tty, pcb->curinputpak,
				    dataptr, datacount);
		    datacount -= status; /* Get bytes not moved */
		    pad_putback(pcb, datacount);
		    if (!status)
			n = -1; /* Nothing output, don't loop */
		    else {
			n -= status;
			if (tty->tty_sessiontimeout && 
			    (tty->capabilities & OUTIDLE))
			    start_sessiontimeout(conn, tty);
		    }	
		    break;
		  default:
		    status = pcb->pcb_lsterr;
		    if (status == OK)
			status = END_OF_FILE;
		    return(status);
	        } /* switch */
		if (pcb->state != PAD_ESTABLISHED)
		    break;
	    } /* while */
	    (void)startoutput(tty);		/* ensure output is running */
	} /* if */

	/* 
	 * Now handle input from the tty (output to the net).
	 */
	if (inputpending(tty)) {
	    paktype *pak;
	    GET_TIMESTAMP(conn->idletime);	/* user typed something */
	    if (tty->tty_sessiontimeout) {
		start_sessiontimeout(conn, tty);
	    }
	    n = MAXBYTES;
	    while (--n > 0) {
		pak = pcb->curoutputpak;
		if (!pak) {
		    if (pad_putsetup(pcb))
			pak = pcb->curoutputpak;
		    else break;		/* Can't get a buffer */
		}
		status = x3_getline(pak, pcb->paramsout, 
				    (void *)tty->ivector->vgetc,
				    (ulong)tty, tty);
		/* if (pad_debug) buginf("\nx3_getline returned %d",status); */
		switch (status) {
		  case X3_BREAKLINE: {
		      int disc = 0;
		      int breakopt = pcb->paramsout[X3_BREAK];
		      pad_putpush(pcb,FALSE);	/* send data */
		      if (breakopt & X3_BREAKDIS) {
			  disc = 1;
			  pcb->flags |= PCB_DISCARD;
			  pcb->paramsout[X3_DISCARD] = 1;
			  clearoutput(tty);
		      }			
		      if (breakopt & X3_BREAKINT)
			  pad_interrupt(pcb);
		      if (breakopt & X3_BREAKIND) {
			  pad_option_emit(pcb, X3_BREAKRECEIVED, X3_DISCARD,
					  disc);
			  pad_putcontrol(pcb);
		      }
		  }
		    break;
		  case X3_BLOCK:
		    /*
		     * If we get here, we must have read some characters.
		     * Set the dispatchime to now plus the idle time, unless
		     * we have 1 (immediate dispatch) or 0 (never dispatch),
		     * or EDIT mode is in use (dispatch only on characters)
		     */
		    if (pcb->paramsout[X3_EDIT] == 0)
			switch (pcb->paramsout[X3_IDLETIME]) {
			  case 0:
			    break;
			  case 1:
			    pushflag = TRUE;
			    break;
			  default:
			    TIMER_START(conn->dispatchtimer,
				pcb->paramsout[X3_IDLETIME] * ONESEC / 20);
			}
		    n = 0;		/* break from outer loop */
		    break;
		  case X3_FULL:
	            pak->flags |= PAK_MBIT;
		    /* Fall through */
		  case X3_DONE:
		    pushflag = TRUE;
		    n = 0;		/* break from outer loop */
		    break;
		    
		  default:
		    if (pad_debug) {
			buginf("\nPad%t bad return code %d from x3_getline",
			       tty->ttynum, status);
		    }
		} /* switch */
	    } /* while --n */
	}   /* if inputpending(tty) */

	if (pushflag || TIMER_RUNNING_AND_AWAKE(conn->dispatchtimer)) {
	    pad_putpush(pcb,FALSE);
	    pushflag = FALSE;
	    TIMER_STOP(conn->dispatchtimer);
	}
    }   /* while (TRUE) */
}

/*
 * pad_unread
 * Return number of unread bytes pending on a PAD.
 * Intended for very high level (protocol independent) routines.
 */
int pad_unread (conntype *conn)
{
    register pcbtype *pcb = (pcbtype *)(conn->tcb);
    register int bytes;
    register paktype *pak;

    if (!pcb)
      return(0);			/* Check for null pointers */

    pak = pcb->curinputpak;		/* current input packet */
    bytes = pcb->unreadbytes;		/* bytes on segment queue */
    if (pcb->tty) {
	if (((tt_soc *)pcb->tty)->isavedchar != -1)
	    bytes++;
    }
    if (pak != NULL)			/* reading something now? */
	bytes += pak->length;		/* yes, count those bytes, too */
    if (pcb->qinput)			/* some random bytes pending? */ 
	bytes++;			/* yes, make our count non-zero */
    return(bytes);			/* return total unread byte count */
}

/*
 * pad_closing
 * Return whether we are going to close the connection or not
 */
boolean pad_closing (pcbtype *pcb)
{
    return (pcb->flags & PCB_CLEAR ? TRUE : FALSE);
}

/*
 * pcb_status
 * show the contents of a pcb block
 */
void pcb_status (pcbtype *pcb)
{
    int i;


    printf("\nTotal input: %d, control %d, bytes %d. "
	   "Queued: %d of %d (%d bytes).",
	   pcb->pakincount, pcb->pakincontrol, pcb->bytesincount,
	   pcb->inqueue.count, pcb->inqueue.maximum, pcb->unreadbytes);
    printf("\nTotal output: %d, control %d, bytes %d.",
	   pcb->pakoutcount, pcb->pakoutcontrol, 	pcb->bytesoutcount);
    if (pcb->curoutputpak)
	printf("  %d bytes waiting to send.",
	       pcb->curoutputpak->unspecified[0] - pcb->curoutputpak->length);
    printf("\nFlags: %lx,   State: %x,   Last error: %x", pcb->flags,
	   pcb->state, pcb->pcb_lsterr);
 
    printf("\n ParamsIn: ");
    for (i=1; i < NX3PARAMETERS; i++) {
	if ((i & 7) == 0)
	    printf("\n   ");
	printf(" %d:%d,",i,pcb->paramsin[i]);
    }
    printf("\n ParamsOut: ");
    for (i=1; i < NX3PARAMETERS; i++) {
	if ((i & 7) == 0)
	    printf("\n   ");
	printf(" %d:%d,",i,pcb->paramsout[i]);
    }
    printf("\n");
    show_x25status(pcb->pcb_lci->lci_lci);
}

/*
 * show_padstatus
 * show status of pad connections
 */
void show_padstatus (int line)
{
    register int i, j, pretty = 0;
    register tt_soc *tty;
    register conntype *conn;
    int firstline, lastline;

    if (line == -1) {
	firstline = 0;
	lastline = ALLlines;
    } else {
	firstline = line;
	lastline = line + 1;
    }
    automore_enable(NULL);
    for (i = firstline; i < lastline; i++) {
	tty = MODEMS[i];
	if (tty == NULL)
	    continue;
	conn = tty->conntop;
	if (conn && conn->tcb && (conn->v == &pad_connvector)) {
            mem_lock(conn);
            mem_lock(conn->tcb);
	    if (pretty++ != 0)
		automore_conditional(0);
	    printf("\n\ntty%t, Incoming PAD connection ",i);
	    pcb_status((pcbtype *)conn->tcb);
            free(conn->tcb);
            free(conn);
	}
        for (j=0;
             (conn = queuepick (&tty->connhead, j));
             j++) {
	    if (conn->tcb && (conn->v == &pad_connvector)) {
                if (!validmem(conn) || !validmem(conn->tcb))
                    return;
                mem_lock(conn);
                mem_lock(conn->tcb);
		if (pretty++ != 0)
		    automore_conditional(0);
		printf("\n\ntty%t",i);
		if (tty->location)
		    printf(" (%s)",tty->location);
		printf(", connection %d to host ",conn->number);
		(void) print_host(conn);
		printc('\n');
		pcb_status((pcbtype *)conn->tcb);
                free(conn->tcb);
                free(conn);
	    }
	}
    }
    automore_disable();
}

/*
 * x3_getline(pak,params,mygetc,arg)
 * Read a line (with possible local editing) into the packet.
 * Return when we would block, or when the packet is ready to send.
 */
boolean linemode_erasechar (tt_soc *tty, int c)
{
    if ((tty == NULL) || (c < ' ') || (c == CH_DEL))
	return(FALSE);
    ttyprintf(tty, "\010 \010");
    return(TRUE);
}

int x3_getline (
    paktype *pak,
    uchar *params,
    int (*mygetc)(ulong),
    ulong arg,
    tt_soc *tty)
{
    int c;
    boolean oflag = FALSE;		/* we generated some output */
    int retval = X3_BLOCK;

    while ((c = mygetc(arg)) != -1) {
	/*
	 * first check for local editing characters.
	 */
	if (params[X3_EDIT]) {
	    if (c == params[X3_CHARDEL]) {  	/* character delete */
		c = pak_rembyte(pak);
		if (params[X3_LOCALECHO] != 0 && linemode_erasechar(tty, c))
		    oflag++;
		continue;
	    }
	    if (c == params[X3_LINEDEL]) {		/* line delete */
		if (tty) {
		    ttyprintf(tty,"XXX\n");
		    oflag++;
		}
		while ((c = pak_rembyte(pak)) != -1)
		    if (c == RETURN) break;
		continue;
	    }
	    if (c == params[X3_LINEDISP]) {		/* line display */
		*pak->dataptr = '\0';	    
		if (tty && params[X3_LOCALECHO] != 0) {
		    ttyprintf(tty, "\n%s",pak->dataptr -
			      (pak->unspecified[0] - pak->length));
		    oflag++;
		}
		continue;
	    }
	}
	if (c == BREAK_CHAR)
	    return(X3_BREAKLINE);
	/*
	 * Insert the character into the buffer.
	 */
	*pak->dataptr++ = c;
	if (--pak->length == 0) {
	    retval = X3_FULL;
	}
	/*
	 * possible local echoing.
	 */
	if (params[X3_LOCALECHO] && tty) {
	    oflag++;
	    putc(tty,c);	/* do local echo if necessary */
	    if ((c == RETURN) && (params[X3_LFINSERT] & 4))
		putc(tty,LINEFEED);
	}
	/*
	 * now check for dispatchness, etc.
	 */
	if (c == RETURN) {
	    if (params[X3_LFINSERT] & 2) {
		if (pak->length > 0) {
		    *pak->dataptr++ = LINEFEED;
		    if (--pak->length <= 0) {
			if (oflag)
			    startoutput(tty);
			retval = X3_FULL; /* full, unless a dispatch */
		    }
		} else {
		    if (tty)
			tty->isavedchar = LINEFEED; /* Try later */
		}
	    }
	    if (params[X3_DISPMASK] & 2)
		goto x3_done;
	}
	if (params[X3_DISPMASK]) {
	    if ((c == CH_DEL) && (params[X3_DISPMASK] & 8))
		goto x3_done;

	    if (c >= ' ') {
		if (params[X3_DISPMASK] & 1)
		    goto x3_done;
	    } else switch (c) {
	      case CH_ESC:
	      case BELL:
	      case CH_ENQ:
	      case CH_ACK:
		if (params[X3_DISPMASK] & 4)
		    goto x3_done;
		break;
	      case CH_CAN:
	      case CH_DC2:
		if (params[X3_DISPMASK] & 8)
		    goto x3_done;
		break;
	      case CH_ETX:
	      case CH_EOT:
		if (params[X3_DISPMASK] & 16)
		    goto x3_done;
		break;
	      case CH_HT:
	      case LINEFEED:
	      case CH_VT:
	      case CH_FF:
		if (params[X3_DISPMASK] & 32)
		    goto x3_done;
		break;
	      default:
		if (params[X3_DISPMASK] & 64)
		    goto x3_done;
		break;
	    } /* switch */
	} /* dispmask exists */
	if (tty && tty->tty_editinfo.dispatch[c & 0x7F] & TTY_DISP_DISP)
	    goto x3_done;
	if (retval == X3_FULL)
	    break;
    } /* while */
    if (oflag) startoutput(tty);
    return(retval);
  x3_done:
    if (oflag) startoutput(tty);
    return(X3_DONE);
}

/*
 * pad_optioncheck
 * Check whether the given option state change request is OK for a cisco
 * PAD connection.  Side effects to the tty database also happen here.
 */
boolean pad_optioncheck (conntype *conn, int param, int value)
{
    tt_soc *tty = conn->tty;
    boolean success = TRUE;
    pcbtype *pcb = (pcbtype *) conn->tcb;

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
		printf("(refused)");
	    success = FALSE;
	}
	break;
	
	/*
	 * The following parameters are basically handed directly from
	 * x3_getline, and so don't need to have any actual effect on
	 * the terminal database.
	 */
	
      case X3_LFINSERT:
      case X3_EDIT:
      case X3_BREAK:
      case X3_CHARDEL:
      case X3_LINEDEL:
      case X3_LINEDISP:
      case X3_LOCALECHO:
      case X3_DISPMASK:
      case X3_IDLETIME:
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
    return(success);
}

/*
 * x3_command
 * allow the user to generate SET messages for various X.3 paramters.
 */
void x3_command (parseinfo *csb)
{
    conntype *conn = stdio->conntop;
    uchar p,v;
    int i;

    if (!conn || (conn->v != &pad_connvector)) {
	printf("No incoming PAD connection.");
	return;
    }

    pad_putpush((pcbtype *)conn->tcb, FALSE);	/* Send any data pending */
    i = 0;
    for (i=0; i<GETOBJ(int,10); i+=2) {
	p = GETOBJ(string,2)[i];
	v = GETOBJ(string,2)[i+1];
	pad_option_emit((pcbtype *)conn->tcb, X3_SETANDREAD, p, v);
    }
    pad_putcontrol((pcbtype *)conn->tcb);
}

