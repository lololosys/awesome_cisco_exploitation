/* $Id: mopvty.c,v 3.4 1995/12/09 00:43:36 tkolar Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/decnet/mopvty.c,v $
 *------------------------------------------------------------------
 * mopvty.c - MOP incoming virtual terminal support
 *
 * 28-Feb-1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mopvty.c,v $
 * Revision 3.4  1995/12/09  00:43:36  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Make it easy to claim a tty for exclusive use by a process, and add a
 * modem_busyout registry.
 *
 * Revision 3.3  1995/11/17  08:59:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:17:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/07/17  23:16:33  hampton
 * Miscellaneous timer cleanups for DECNET.  [CSCdi37215]
 *
 * Revision 2.5  1995/07/07 22:46:49  hampton
 * Convert DECNET to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi36830]
 *
 * Revision 2.4  1995/06/30 20:35:03  asastry
 * CSCdi36500:  DECnet MOP Remote Console Connection Fails
 *
 * Revision 2.3  1995/06/20  03:44:30  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.2  1995/06/18  19:40:23  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:23:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../ui/debug.h"
#include "ttysrv.h"
#include "connect.h"
#include "../if/ether.h"
#include "../if/network.h"
#include "../snmp/sr_old_ltsmib.h"
#include "mop.h"
#include "mop_debug.h"

/*
 * Local storage
 */

static vectortype MOPvector;		/* Terminal dispatch vector */
static connvector mop_connvector;	/* Connection block dispatch vector */

/*
 * External references
 */

extern boolean mop_debug;

/*
 * mop_vty_init - Initialize virtual terminal context
 */

void mop_vty_init (void)
{
    vector_init(&MOPvector);		/* Initialize the tty vector */
    MOPvector.vgetc = mop_getc;
    MOPvector.vclearinput = mop_clearinput;
    MOPvector.vputc = mop_putc;
    MOPvector.vinputpending = mop_inputpending;
    MOPvector.voutputpending = mop_outputpending;
    MOPvector.vstartoutput = mop_startoutput;
    MOPvector.voutputblock = mop_outputblock;
    MOPvector.vputpkt = generic_putpkt;

    conn_vector_init(&mop_connvector,
		     "mop", "DEC MOP Remote Console Protocol");
					/* Initialize the connection vector */

    mop_connvector.snmp_sesstype = D_tslineSesType_mop;

    mop_connvector.valive = mop_alive;
    mop_connvector.vclose = mop_close;
    mop_connvector.vunread = mop_unread;
}


/*
 * mop_reserve_console - reserve the MOP remote console
 *
 * This routine actually reserves the MOP console. When we get a request,
 * we check to see if there is already a reservation for this unit. If so,
 * the request is ignored (the requester will discover this by sending
 * a subsequent request ID), otherwise, we allocate the console data
 * structures and a VTY to use as the console.
 */

paktype *mop_reserve_console (
    hwidbtype *idb,
    paktype *pak,
    int len)
{
    conntype *conn;
    tt_soc *tty;
    mopcon *console;
    ieee_addrs *addrs;

    addrs = (ieee_addrs *)pak->mac_start;

    /* At some point, we will need to check the password */

    console = idb->mop_console;		/* Get current console pointer */
    if (console) {			/* If the console is reserved */
	if (mop_debug) {		/* Report it */
	    buginf("\nMOP: Console reservation from %e while reserved by %e",
		   addrs->saddr, console->user);
	}
	if (ieee_equal(addrs->saddr, console->user)) {
					/* If this is the same user */
	    console->tty->statbits |= CARDROP; /* Dump old vty */
	    conn = console->tty->conntop;
	    mop_close(conn);		/* Clean up the structures */
	} else {
	    return(pak);		/* Otherwise, ignore this */
	}
    }

    if (mop_debug) {
	buginf("\nMOP: Reserving console for %e", addrs->saddr);
    }

    console = malloc(sizeof(mopcon)); /* Get a console block */
    if (!console)			/* If the allocation failed */
      return(pak);				/* Ignore the message */

    console->xmit[0] = getbuffer(MOP_CONSOLE_OUTBUF + 3);
					/* Get a transmit buffer */
    if (!console->xmit[0]) {		/* If the allocation failed */
	free(console);			/* Free the console block */
	return(pak);				/* Return if failure */
    }

    console->xmit[1] = getbuffer(MOP_CONSOLE_OUTBUF + 3);
					/* Get another transmit buffer */
    if (!console->xmit[1]) {		/* If failure */
	datagram_done(console->xmit[0]); /* Free the first one */
	free(console);			/* And the receive buffer */
	return(pak);			/* Get out */
    }

    console->xmit[0]->mop_offset = 1;	/* Set initial offset */
    console->xmit[1]->mop_offset = 1;	/* In both places */

    tty = tty_allocate(0, FALSE);	/* Grab a TTY to use */
    if (!tty) {				/* Return if we failed */
	datagram_done(console->xmit[0]);
	datagram_done(console->xmit[1]);
	free(console);
	return(pak);
    }

    conn = getconnection(tty, &mop_connvector); /* Get a connection block */
    if (conn == NULL) {			/* Return if failed */
	datagram_done(console->xmit[0]);
	datagram_done(console->xmit[1]);
	free(console);
	return(pak);
    }

    unqueue(&tty->connhead, conn);
    					/* Dequeue connection block */
    tty->conntop = conn;   		/* Set up driver connection */
    tty->ivector = & MOPvector;		/* Set input vector pointer */
    conn->tcb = idb;			/* Remember IDB pointer */
    console->tty = tty;			/* Remember tty pointer */

    tty_create_vty_exec(tty);		/* Try to create a process */
    if (tty->tiptop < 0) {
	tty_deallocate(tty);
	datagram_done(console->xmit[0]);
	datagram_done(console->xmit[1]);
	free(console);
	freeconnection(conn);
	return(pak);
    }

    idb->mop_console = console; /* Remember console block */

    CLAIM_TTY(tty);		/* Mark VTY as allocated */
    tty->statbits |= READY;	/* Mark VTY as ready */	
    tty->statbits &= ~(CARDROP|HANGUP|IDLE); /* Ensure not marked as idle */
    tty->conncount++;			/* Bump connection count */

    ieee_copy(addrs->saddr,console->user); /* Remember allocator */
    console->sequence = 0;		/* Set sequence number */
    TIMER_START(console->reservation, MOP_CONSOLE_TIMEOUT * ONESEC);
					/* Set the reservation timeout */
    console->inhead = console->inbuf;	/* Set the input head pointer */
    console->intail = console->inbuf;	/* And the input tail pointer */
    return(pak);
}

/*
 * mop_release_console - Free up the remote console
 *
 * This routine is used to free the remote console. If the message came
 * from the current console requestor, we will dump the connection,
 * otherwise, we'll free up the structures.
 */

paktype *mop_release_console (
    hwidbtype *idb,
    paktype *pak,
    int len)
{
    mopcon *console = idb->mop_console;
    conntype *conn;
    ieee_addrs *addrs;

    addrs = (ieee_addrs *)pak->mac_start;

    if (console && ieee_equal(addrs->saddr,console->user)) {
					/* If we have a remote user, and
					   this is that user */
	console->tty->statbits |= CARDROP; /* Mark tty as dying */
	conn = console->tty->conntop;
	mop_close(conn); 		/* Dump the structures */
	if (mop_debug) {
	    buginf("\nMOP: Console released by %e", addrs->saddr);
	}
    } else {
	if (mop_debug) {
	    buginf("\nMOP: Invalid console release by %e", addrs->saddr);
	}
    }
    return(pak);
}

/*
 * mop_console_command - Process a remote command and send back current data
 *
 * This routine is used to reply to a console command and poll message. If
 * this message is a duplicate, we'll send our previous transmission.
 * Otherwise, we check to see if we can buffer the incoming data. If we
 * can't, we'll dynamically construct a message to send a NAK back to
 * the remote system. We send out NAKs out of special buffers instead of
 * our pre-allocated transmit buffers since the remote MOP requestor
 * (at least for VMS) ignores any of the transmit data sent along with
 * a message that indicates receive data lost.
 */

paktype *mop_console_command (
    hwidbtype *idb,
    paktype *pak,
    int len)
{
    paktype *xmitpak;
    int i;
    mopcon *console = idb->mop_console;
    ieee_addrs *addrs;
    mophdrtype *mop, *pak_mop;

    addrs = (ieee_addrs *)pak->mac_start;
    pak_mop = (mophdrtype *)mopheadstart(pak);

    /* Ensure that this user has the console reserved */

    if (!console || !ieee_equal(addrs->saddr, console->user)) {
	if (mop_debug) {
	    buginf("\nMOP: Got console command from %e when not reserved",
		   addrs->saddr);
	}
	return(pak);
    }
    
    if (len < 2) {			/* Ignore too short packets */
	if (mop_debug) {
	    buginf("\nMOP: Console command message from %e too short",
		   addrs->saddr);
	}
	return(pak);
    }
    
    xmitpak = console->xmit[(~console->sequence) & 1];
    mop     = (mophdrtype *)mopheadstart(xmitpak);
    xmitpak->acct_proto = ACCT_PROTO_MOP;

    					/* Get previous transmit buffer */
    if ((pak_mop->parameters[0] & 1) != (console->sequence & 1)) {
	if (xmitpak->mop_flags & MOP_WAIT_ACK) {
	    pak_lock(xmitpak);		/* Lock the packet */
	    datagram_out(xmitpak);
				 	/* Retransmit outstanding message */
	}
	return(pak);			/* Get out on duplicates */
    }

    /* Check to see if we have buffer space for this */

    if ((console->incount + (len - 2)) > MOP_INBUF) {

	/* Not enough buffer space, construct a buffer for a NAK */

	xmitpak = getbuffer(SMALLDATA); /* Get a buffer for a NAK */
	if (!xmitpak)			/* Exit if failure */
	    return(pak);
	mop = (mophdrtype *)mopheadstart(xmitpak);
	mop->datalen = h2lit(2);	/* Set the length */
	mop->code = MOP_CONSOLE_RESPONSE; /* Set the type */
	mop->parameters[0] = (console->sequence & 1) |
	  MOP_RESPONSE_CMD_LOST;	/* Indicate command data lost */
	xmitpak->mop_offset = 1;	        /* Indicate one byte of parameters */
    } else {

	/* Have buffer space, process this ACK and fetch the data */

	xmitpak->mop_flags &= ~MOP_WAIT_ACK; /* Flag not waiting for an ACK */
	xmitpak->mop_offset = 1;     	     /* Reset data count */
	
	TIMER_START(console->reservation, MOP_CONSOLE_TIMEOUT * ONESEC);
					/* Restart reservation timeout */

	xmitpak = console->xmit[console->sequence & 1];
    					/* Get new xmit buffer */
	mop = (mophdrtype *)mopheadstart(xmitpak);
	mop->code = MOP_CONSOLE_RESPONSE; /* Set message code */
	mop->parameters[0] = console->sequence++ & 1;
					/* Set the sequence number */

	if (pak_mop->parameters[0] & MOP_CONSOLE_BREAK) {
	    console->tty->statbits |= LASTWASBREAK;
	}

	for (i = 2; i < len; i++) {
	    if (console->intail == &console->inbuf[MOP_INBUF]) {
		console->intail = console->inbuf;
	    }
	    *console->intail++ = pak_mop->parameters[i-1];
	    ++console->incount;
	}

	if (xmitpak->mop_flags & MOP_WAIT_ACK) {
	    if (mop_debug)
	      buginf("\nMOP: ACK flag on unexpectedly");
	}
	xmitpak->mop_flags |= MOP_WAIT_ACK; /* Mark as waiting for an ACK */
	pak_lock(xmitpak);		/* Place a lock on the packet */
	mop->datalen = h2lit(xmitpak->mop_offset + 1);
    }

    xmitpak->if_output = idb->firstsw;	/* Set the output interface */
    xmitpak->enctype = ET_ARPA; 	/* Set the encapsulation */
    xmitpak->linktype = LINK_MOP_CONSOLE; /* Set the link type */
    xmitpak->acct_proto = ACCT_PROTO_MOP; /* set protocol type for acctng */

    if (pak_encap(idb, xmitpak, xmitpak->mop_offset + 3,
		  (long)console->user)) {
	datagram_out(xmitpak);
    } else {
	if (mop_debug) {
	    buginf("\nMOP: Encapsulation of console data to %e failed",
		   console->user);
	}
	datagram_done(xmitpak);
    }
    return(pak);
}

/*
 * mop_alive - Determine if a mop connection is still active
 *
 * If the tcb (really IDB) pointer in the connection block has been
 * cleared, or if the reservation timer has expired, we declare the
 * connection dead. Otherwise, it is alive.
 */

boolean mop_alive (conntype *conn)
{
    hwidbtype *idb = conn->tcb;

    if (!idb)
      return(FALSE);			/* No IDB means connection dead */
    return(SLEEPING(idb->mop_console->reservation));
					/* See if timed out yet */
}

/*
 * mop_close - Disconnect a mop connection
 *
 * This routine is used to disconnect a MOP connection. Unfortunatly, there
 * is no protocol message to send when this happens, so we just rely on
 * the console requestor to notice our lack of response to further
 * command and poll messages and tear down the appropriate structures.
 */

void mop_close (conntype *conn)
{
    hwidbtype *idb = conn->tcb;

    conn->tcb = NULL;			/* Dump the IDB pointer */
    if (idb)
      mop_cleanup(idb);			/* Now clean up */

    freeconnection(conn);
}

/*
 * mop_cleanup - Throw away mop data structures
 *
 * This routine is used to delete the console data structures and our
 * transmit buffers. Note that we don't touch the associated tty structure;
 * it is assumed that the caller did that.
 */

void mop_cleanup (hwidbtype *idb)
{
    mopcon *console = idb->mop_console;

    idb->mop_console = NULL;		/* Forget console block pointer */

    datagram_done(console->xmit[0]);
    datagram_done(console->xmit[1]);
    
    free(console);			/* Free the console block */
}

/*
 * tty2con - Get a console block pointer from a tty pointer
 *
 * This routine finds the associated console block given a tty pointer.
 * If the tty is marked as dead or dying, or the connection block no longer
 * points to a idb (happens during disconnect), or the reservation timer
 * has expired, we'll return null, otherwise, we return the address of
 * the console block. This is used by the vty service routines below.
 */

mopcon *tty2con (tt_soc *tty)
{
    hwidbtype *idb;
    conntype *conn;
    mopcon *console;

    /* Handle dead or dying connections */

    if (tty->statbits & (CARDROP | IDLE))
      return(NULL);

    conn = tty->conntop;

    if (conn == NULL) {
	tty->statbits |= CARDROP;
	return(NULL);
    }

    /* Find the IDB pointer, and mark the terminal as dying if the
     *  connection is going away.
     */

    idb = conn->tcb;
    if (idb == NULL) {
	tty->statbits |= CARDROP;
	return(NULL);
    }

    console = idb->mop_console; /* Get console block pointer */

    /* Ensure that this tty is still being used as the actual console,
       and that the reservation timer hasn't yet expired. */

    if ((console->tty != tty) || !SLEEPING(console->reservation)) {
	tty->statbits |= CARDROP;
	return(NULL);
    }

    return(console);
}

/*
 * mop_startoutput - start any pending output
 *
 * Since the mop process can only send data in response to a poll message,
 * all we do here is return TRUE if the session is still active, and FALSE
 * otherwise. Some processes in the system use the return from startoutput()
 * to determine whether the connection is hung or not, so this will
 * emulate that feature.
 */

boolean mop_startoutput (tt_soc *tty)
{
    return(tty2con(tty) != NULL);	/* Just indicate whether we're alive */
}

/*
 * mop_getc - Get an input character
 *
 * This routine fetches input characters which were stored from previous
 * command data and poll messages. Note that in order to support the mop
 * break mechanism, the mop process will set the LASTWASBREAK flag for
 * the associated terminal; if we see that flag set, we will return the
 * break character here.
 */

int mop_getc (tt_soc *tty)
{
    mopcon *console;
    int ch = -1;

    console = tty2con(tty);		/* Get console pointer */
    if (!console)
      return(-1);			/* Get out if connection dying */

    /*
     * Check for saved character.
     */

    if (tty->isavedchar != -1) {
	ch = tty->isavedchar;
	tty->isavedchar = -1;
	return(ch);
    }

    /*
     * Check for pending breaks
     */

    if (tty->statbits & LASTWASBREAK) {
	tty->statbits &= ~LASTWASBREAK;
	return(BREAK_CHAR);
    }

    /*
     * Fetch a character from the receive buffer
     */

    if (console->inhead != console->intail) {
	ch = *console->inhead++;
	if (console->inhead == &console->inbuf[MOP_INBUF]) {
	    console->inhead = console->inbuf;
	}
	console->incount--;
    }

    /*
     * Handle special characters
     */

    if ((ch != -1) && (tty->type == VTY_LINE)) {
	GET_TIMESTAMP(tty->conntop->idletime); /* 0 idle time */
	if (special_chars(tty,ch) < 0) { /* Check for specials */
	    ch = -1;
	} else {
	    if (tty->statbits & LASTWASBREAK) {
		tty->statbits &= ~LASTWASBREAK;
		ch = BREAK_CHAR;
	    }
	    if (tty->statbits & LASTWASSPEC) {
		tty->statbits &= ~LASTWASSPEC;
		ch |= ESCAPED_FLAG;
	    }
	}
    }
    return(ch);
}

/*
 * mop_outputblock - Determine if an output request would block
 *
 * This is the MOP terminal function for testing to see if there is a
 * free output buffer. We return FALSE (indicating that output would
 * now block) if the session is going away, or if the current transmit
 * buffer isn't marked as waiting for an ACK and has room for more
 * characters. The receipt of a command data and poll message causes
 * the current transmit buffer to change, and also clears the ACK wait flags.
 */

boolean mop_outputblock (tt_soc *tty)
{
    paktype *pak;
    mopcon *console;

    console = tty2con(tty);		/* Get TTY pointer */
    if (!console)
      return(FALSE);			/* Don't block if dying */

    pak = console->xmit[console->sequence & 1];
					/* Get transmit buffer */
    return((pak->mop_flags & MOP_WAIT_ACK) ||
	   (pak->mop_offset  == MOP_CONSOLE_OUTBUF));
}

/*
 * mop_putc - Store an output character
 *
 * This routine stores output characters in the current MOP transmit buffer,
 * if there is room and the buffer isn't marked as waiting for an ACK.
 */

void mop_putc (tt_soc *tty, int ch)
{
    paktype *pak = NULL;
    mopcon *console;
    mophdrtype *mop;

    while (TRUE) {
	console = tty2con(tty);		/* Get console block pointer */
	if (!console)			/* If no console block */
	  return;			/* Get out */

	pak = console->xmit[console->sequence & 1]; /* Get transmit buffer */

	mop = (mophdrtype *)mopheadstart(pak);

	/*
	 * If we are not waiting for an ACK, and there is room in this
	 * buffer, get out.
	 */

	if ((!(pak->mop_flags & MOP_WAIT_ACK)) &&
	    (pak->mop_offset < MOP_CONSOLE_OUTBUF))
	  break;

	edisms((blockproc *)mop_outputblock, (ulong)tty); /* Wait for space */
    }
    mop->parameters[pak->mop_offset++] = ch;	/* Store the character */
}

/*
 * mop_clearinput - Clear pending mop input
 *
 * This is the clear input routine. We simply reset the input pointers
 * and the input count.
 */

void mop_clearinput (tt_soc *tty)
{
    mopcon *console;

    console = tty2con(tty);		/* Get console block pointer */
    if (!console)
      return;

    console->inhead = console->inbuf;	/* Reset the head pointer */
    console->intail = console->inbuf;	/* And the tail pointer */
    console->incount = 0;		/* Reset the count */
}

/*
 * mop_inputpending - Return number of pending input characters
 *
 * This routine returns the number of pending input characters. If the
 * session is dying, we return zero, otherwise, we return the count stored
 * in the console block.
 */

int mop_inputpending (tt_soc *tty)
{
    mopcon *console;

    console = tty2con(tty);		/* Get console block pointer */
    if (!console)			/* Return zero if session dying */
      return(0);

    return(console->incount);		/* Return the current input count */
}

/*
 * mop_outputpending - Return number of pending output characters
 *
 * This routine returns the number of pending output characters. Note that
 * we simply return the number of characters stored so far in the current
 * output buffer. It would be more accurate to look at the count of both
 * buffers, but I can't see of any advantage that it would offer.
 */

int mop_outputpending (tt_soc *tty)
{
    mopcon *console;
    paktype *pak;

    console = tty2con(tty);		/* Get console block pointer */
    if (!console)			/* Return zero if session dying */
      return(0);

    pak = console->xmit[console->sequence & 1]; /* Get current xmit buffer */
    if (pak) {				/* If we have one, get the count */
	return(pak->mop_offset - 1);
    }
    return(0);				/* If no buffers, return zero */
}

/*
 * mop_unread - Return the number of unread input bytes on a connection
 */

int mop_unread (conntype *conn)
{
    if (conn->tty)
      return(mop_inputpending(conn->tty)); /* Get the count from the tty */

    return(0);
}
