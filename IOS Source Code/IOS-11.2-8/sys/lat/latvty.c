/* $Id: latvty.c,v 3.4.28.2 1996/08/28 12:56:12 thille Exp $
 * $Source: /release/112/cvs/Xsys/lat/latvty.c,v $
 *------------------------------------------------------------------
 * latvty.c -- LAT virtual terminals
 *
 * 21-January-1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: latvty.c,v $
 * Revision 3.4.28.2  1996/08/28  12:56:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.28.1  1996/03/18  20:39:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.1  1996/03/07  10:04:26  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/22  06:36:39  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/09  00:43:43  tkolar
 * CSCdi45302:  Need a clean interface between modem hardware and
 * commserver
 * Make it easy to claim a tty for exclusive use by a process, and add a
 * modem_busyout registry.
 *
 * Revision 3.2  1995/11/17  17:34:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/06/20  03:44:46  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.3  1995/06/13  16:38:17  syiu
 * CSCdi35685:  Async callback rotary should be in reverse oder
 *
 * Revision 2.2  1995/06/09  13:11:10  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:26:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "mtcsys.h"
#include "latsys.h"
#include "latob.h"
#include "latas.h"
#include "latsb.h"
#include "lathi.h"
#include "latcb.h"
#include "latnd.h"
#include "latfnc.h"
#include "latmsg.h"
#include "latslt.h"
#include "sysfnc.h"
#include "ttysrv.h"
#include "connect.h"
#include "termqueue.h"
#include "logger.h"
#include "../lat/msg_lat.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "address.h"
#include "name.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "lat.h"
#include "lat_debug.h"
#include "../pt/translate.h"
#include "config.h"
#include "registry.h"
#include "../h/cs_registry.regh"
#include "../pt/pt_registry.h"
#include "../ui/command.h"
#include "../os/async.h"
#include "../os/free.h"
#include "stacks.h"

/*
 * local storage
 */
vectortype LATvector;
queuevector lat_queuevector;

/*
 * forward references
 */
static sb_ptr tty2sb(tt_soc *tty);

/*
 * lattranslate_cfork
 * Create a fork for incoming LAT connection protocol translation.
 * Return PID or -1 if failure. 
 */

static int lattranslate_cfork (tt_soc *tty, translatetype *trans)
{
    trans->totaluses++;
    if (trans->maxallowed && (trans->active >= trans->maxallowed)) {
	trans->failures++;
	return(-1);
    }
    return(reg_invoke_pt_latinput(OUTPROT(trans->type), trans, tty));
}

/*
 * lat_daemon - Startup handler for inbound LAT sessions
 *
 * This is the startup handler for inbound LAT sessions. If this is
 * a host-initiated connection, it waits for the connection to complete.
 * Then it invokes either exec() or tty_daemon() depending on whether
 * this connection is to a VTY or real line.
 */

static forktype lat_daemon (tt_soc *tty)
{
    conntype *conn;

    conn = tty->conntop;	/* Get connection block */

    if (conn->proto.lat.flags & LAT_CONN_HIC)
      edisms((blockproc *)lat_openBLOCK, (long)conn);
				/* Wait for HIC connection to open */

    if (tty->type == VTY_LINE) {
	tty->ovector = tty->ivector; /* Set output vector for VTY lines */
	exec(tty);		/* Run the exec */
    } else {
	tty_daemon(tty);	/* Run the TTY daemon */
    }
    process_kill(THIS_PROCESS);	/* Should never get here... */
}

/*
 * lattranslate_accesscheck (tty, node, trans)
 * Check whether access is allowed to this translation.  This is mostly a
 * data-hiding hack - lat doesn't know about translate data structures, and
 * translate doens't know about lat data structures.
 */

static boolean lattranslate_accesscheck (uchar *node, translatetype *trans)
{
    return(regexp_accesscheck(node, trans->accessclass, lat_accesslists));
}
 
/*
 * lat_inbound - Create a process for a new inbound connection
 *
 * This routine is used to create a new process for an inbound LAT session.
 * The session can either be a host-initiated connection or a host connection.
 * Return 0 if successfull, or an appropriate LAT_SLOT error code
 */

static int lat_inbound(
     conntype *conn,
     as_ptr as)
{
    tt_soc *tty = conn->tty;

    tty->ivector = & LATvector;		/* Set input vector pointer */
    unqueue(&tty->connhead, conn); /* Remove from conn queue */
    tty->conntop = conn;	/* Set up the driver connection */

    /*
     * Do access check.  Note that conn->hostname should already be set
     */
    if (as && as->as_aid) {
	if (lattranslate_accesscheck((uchar *)conn->hostname,
				     (translatetype *)as->as_aid))
	    tty->tiptop = lattranslate_cfork(tty, (translatetype *)as->as_aid);
	else
	    return(SLOT_STOP_ACCESS_DENIED);
    } else {
	if (regexp_accesscheck((uchar *)conn->hostname, 
			       tty->accessin, lat_accesslists))
	  tty->tiptop = cfork((forkproc *)lat_daemon, (long)tty,
			      EXEC_STACK_SIZE,
			      "LAT daemon", tty->ttynum);
	else
	    return(SLOT_STOP_ACCESS_DENIED);
    }

    if (tty->tiptop < 0) {
	tty_deallocate(tty);
	return (SLOT_STOP_NO_RESOURCE);
    }

    CLAIM_TTY(tty); 		/* Mark VTY as allocated */
    tty->statbits |= READY;	/* Mark VTY as ready */
    tty->statbits &= ~(CARDROP|HANGUP|IDLE); /* Ensure not marked as idle */
    tty->conncount++;			/* Bump the connection count */

    if (lat_debug) {
	buginf("\nLAT%t: created new inbound session",conn->tty->ttynum);
    }

    return(0);			/* Indicate that it worked */
}

/*
 * lat_service_to_ttynum - find a terminal for a service
 *
 * This routine finds a terminal to use for an inbound connection. If the
 * inbound connection is to a rotary group, it picks a free terminal from
 * that group; otherwise, it finds a free VTY to use. If no matching
 * terminal can be found, this returns a zero; if a valid terminal is
 * found, but it isn't available, it returns -(line number).
 *
 * Both the service block pointer and port name are optional parameters.
 * If neither are specified, we will pick a free VTY. If a service is
 * specified, but no port is specified, we will pick a terminal from a
 * rotary group if the service is associated with a rotary group; otherwise,
 * we will pick a free VTY. If a port number is specified, we will not use
 * the service, but instead pick that port. Note that in the future it may
 * be desirable to use attributes from both the service and port. In fact,
 * this works with the current code, as if you select a VTY with a service
 * that has an autocommand, we will use that specified VTY.
 */
 
static int lat_service_to_ttynum(
     as_ptr as,
     byte *port)
{
    int line;

    /* Check to see if a port was specified. */

    if (port && port[0]) {
	port[port[0]<(OB_STR-1) ? port[0]+1 : OB_STR-1] = 0;
						/* Zero terminate the string */

	/* Parse the line number, and check to see if it is free */

	line = select_line(get_ttyline((char *)&port[1]), &lat_connvector);
    } else {

	/* No line number, check for service rotary group */

	if (as && as->as_rotary) {

	    /* We got a rotary group, look for a line in it */

	    line = select_rotary(as->as_rotary, &lat_connvector, NULL, 
                                 ROTARY_ASCENDING);

	} else {

	    line = get_vtyline(TELNET_PORT, &lat_connvector);
	    if (line == 0)
		line = -VTYBase;
	}
    }
    return(line);
}

/*
 * lat_start_host
 *
 * Start a LAT host session. This involves allocating a VTY and hooking
 * together the session block with a connection block.
 */

void lat_start_host (sb)
    sb_ptr sb;
{
    tt_soc *tty;
    conntype *conn;
    as_ptr as;
    int ttynum, errcode = SLOT_STOP_NO_RESOURCE;

    /* Find the service being connected to */

    as = as_show(sb->sb_name);
    if (!as) {
	sb_reject(sb, SLOT_STOP_SRV_UNKNOWN);
	return;
    }

    if (!(as->as_flags & AS_ENABLED)) {
	sb_reject(sb, SLOT_STOP_SRV_DISABLED);
	return;
    }

    if (!mempool_is_empty(MEMPOOL_CLASS_LOCAL)) {    /* If we have memory */

	ttynum = lat_service_to_ttynum(as, ob_temp); /* Grab a terminal */
	if (ttynum == 0) {			/* No such terminal? */
	    sb_reject(sb, SLOT_STOP_PORT_UNKNOWN); /* Yes, say so */
	    return;
	}

	/* At this point, we should check out access lists */

	if (ttynum > 0) {			/* If we got a terminal */
	    tty = MODEMS[ttynum];		/* Pick up tty pointer */
	    conn = getconnection(tty, &lat_connvector);
	    if (conn) {
		if (lat_getdatabuf(conn)) {
		    conn->tcb = sb; /* Save connection block */
		    conn->status |= CONN_INBOUND; /* Mark as inbound */
		    conn->proto.lat.max_credits = LAT_INBOUND_CREDITS;
 		    conn->proto.lat.t_capabilities = tty->capabilities;
		    conn->proto.lat.t_icapabilities = tty->capabilities;
 		    conn->proto.lat.t_parity = tty->tty_parity;
 		    conn->proto.lat.t_databits = tty->tty_databits;
		    str_return((byte *)conn->hostname,
			       sb->sb_cb->cb_nd->nd_node,
			       CONNLEN);		/* Copy name */
		    if (tty->type == VTY_LINE)
		      tty->tty_autocommand = as->as_command;
						 /* Set autocommand */
		    else
		      tty->tty_autocommand = NULL;
 		    errcode = lat_inbound(conn, as);
 		    if (errcode == 0) { /* Do final processing if OK */
  			sb_accept(sb, conn, conn->proto.lat.max_credits);
  			return;			/* Get out successfully */
		    }
		}
		freeconnection(conn);
	    }
	} else {
	    sb_reject(sb, SLOT_STOP_SRV_IN_USE); /* Terminal in use */
	    return;
	}
    }
    sb_reject(sb, errcode);	/* By now, we failed */
}

/*
 * lat_start_hic - Attempt to start a host-initiated connection session
 *
 * This routine is used to start up a host-initiated connection. It is
 * passed a pointer to a HIC block. This routine returns TRUE if the
 * session was started or aborted due to an error; it returns FALSE if
 * the session should be queued.
 */

boolean lat_start_hic (hi)
     hi_ptr hi;
{
    as_ptr as;
    int ttynum;
    tt_soc *tty;
    conntype *conn;
    sb_ptr sb;
    int errcode;

    /* First of all, look for any specified service */

    if (hi->hi_service[0]) {
	as = as_show(hi->hi_service);		/* Look up the service */
	if (!as) {				/* Complain if no session */
	    hi_reject(hi, SLOT_STOP_SRV_UNKNOWN);
	    return(TRUE);			/* Don't queue this */
	}
	if (!(as->as_flags & AS_ENABLED)) {
	    hi_reject(hi, SLOT_STOP_SRV_DISABLED);
	    return(TRUE);			/* Don't queue this */
	}
    } else {
	as = NULL;				/* Mark no service */
    }

    if (mempool_is_empty(MEMPOOL_CLASS_LOCAL)) {
	hi_reject(hi, SLOT_STOP_NO_RESOURCE);	/* Don't queue sessions now! */
	return(TRUE);
    }

    ttynum = lat_service_to_ttynum(as, hi->hi_port);

    if (ttynum == 0) {				/* If no such tty */
	hi_reject(hi, SLOT_STOP_PORT_UNKNOWN);	/* Save so */
	return(TRUE);
    }

    tty = MODEMS[ttynum > 0 ? ttynum : -ttynum]; /* Get TTY pointer */

    if (ttynum > 0) {				/* If we got a terminal */
	conn = getconnection(tty, &lat_connvector);
						/* Get connection block */
	if (conn) {
	    conn->proto.lat.flags |= LAT_CONN_HIC;
	    conn->proto.lat.hiptr = hi;
	    if (lat_getdatabuf(conn)) {		/* Get data buffer */
		/* Set up connection name */
		str_return((byte *)conn->hostname, hi->hi_node, CONNLEN);
		errcode = lat_inbound(conn, as);
		if (as && as->as_aid &&
		    ((errcode == 0) || (errcode == SLOT_STOP_NO_RESOURCE)))
		    return(FALSE); /* Queue protocol translator sessions */
		
		if (errcode == 0) {
		    conn->proto.lat.max_credits = LAT_INBOUND_CREDITS;
		    sb = hi_open(hi, conn, conn->proto.lat.max_credits);
		    if (sb) {
                        conn->proto.lat.hiptr = NULL; /* Clean up HIC pointer */
			conn->status |= CONN_INBOUND; /* Mark as inbound */
			conn->status |= CONN_STREAM;
			conn->proto.lat.t_capabilities = tty->capabilities;
			conn->proto.lat.t_icapabilities = tty->capabilities;
			conn->proto.lat.t_parity = tty->tty_parity;
			conn->proto.lat.t_databits = tty->tty_databits;
			conn->tcb = sb;
			if (as && tty->type == VTY_LINE)
			  tty->tty_autocommand = as->as_command;
			else
			  tty->tty_autocommand = NULL;
			return(TRUE);
		    }
		}
	    }
	    freeconnection(conn);		/* Failed, dump connection */
	}
	hi_reject(hi, SLOT_STOP_NO_RESOURCE);	/* No resources */
	return(TRUE);
    }
    return(FALSE);				/* Queue the request */
}

/*
 * lat_show_entry - Show a HIC queue entry
 */

static void lat_show_entry (
     hi_ptr hi)
{
    byte node[ND_STR];
    byte service[SV_STR];
    byte port[OB_STR];

    /* Copy the fields of interest out of the HI block. This is done so
     * that if printf() blocks and the HI block goes away from under us,
     * we won't display garbage. */

    str_move(node, hi->hi_node);
    str_move(service, hi->hi_service);
    str_move(port, hi->hi_port);

    if (service[0]) {
	printf("service %*s %s", LATSTRING(service),
	       port[0] ? "and " : NULL);
    }

    if (port[0]) {
	printf("port %*s ", LATSTRING(port));
    }

    printf("from LAT node %*s", LATSTRING(node));
}

/*
 * lat_delete_entry - Delete a queue entry
 */

static void lat_delete_entry (
     hi_ptr hi)
{
    hi_reject(hi, SLOT_STOP_ENTRY_DELETED);	/* Indicate entry deleted */
}

/*
 * lat_getc
 * Get a character from the LAT driver.  Returns -1 if none available.
 */

static int lat_getc (
    tt_soc *tty)
{
    int ch;
    sb_ptr sb = tty2sb(tty);
    conntype *conn = tty->conntop;

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

    if (conn->proto.lat.config & SLOT_BREAK) {
	conn->proto.lat.config &= ~SLOT_BREAK;
	ch = BREAK_CHAR;
    } else {

    /*
     * Not a break, go and look at input slots
     */

	if (conn->proto.lat.input_slot_left[conn->proto.lat.slot_empty]) {
					/* If there is input data */
	    ch = (*conn->proto.lat.input_slot[conn->proto.lat.slot_empty]++)
	      & tty->tty_datamask;
					/* Fetch the character */
	    if (!--conn->proto.lat.input_slot_left[conn->proto.lat.slot_empty]) {
					/* If the buffer is now empty */
		datagram_done(conn->proto.lat.input_pak
			      [conn->proto.lat.slot_empty]); /* Free packet */
		if (sb)			/* If session still alive */
		 sb_signal(sb, 0, 1);	/* Return the credit */
		if (++conn->proto.lat.slot_empty == 
		    conn->proto.lat.max_credits) {
		    conn->proto.lat.slot_empty = 0; /* Update empty pointer */
		}
	    }
	} else {
	    if (conn->proto.lat.slot_empty != conn->proto.lat.slot_fill) {
		buginf("\nLAT%t: Slot array data pointer zero, empty = %d, fill = %d",
		       tty->ttynum, conn->proto.lat.slot_empty,
		       conn->proto.lat.slot_fill);
	    }
	    ch = -1;
	}
    }

    if ((ch != -1) && (tty->type == VTY_LINE)) {
	GET_TIMESTAMP(tty->conntop->idletime); /* 0 idle time */
	if (special_chars(tty,ch) < 0) { /* Check for specials */
	    ch = -1;
	} else {
	    if (tty->statbits & LASTWASBREAK) {
		tty->statbits &= ~LASTWASBREAK;
		return(BREAK_CHAR);
	    }
	    if (tty->statbits & LASTWASSPEC) {
		tty->statbits &= ~LASTWASSPEC;
		return(ch | ESCAPED_FLAG);
	    }
	}
    }
    return (ch);
}

/*
 * lat_clearinput
 * Flush the LAT input queues.
 */

static void lat_clearinput (
    tt_soc *tty)
{
    sb_ptr sb = tty2sb(tty);
    int i;
    int credits = 0;
    conntype *conn = tty->conntop;

    tty->isavedchar = -1;		/* clear saved input character */

    for (i = 0; i < conn->proto.lat.max_credits; i++) {
	if (conn->proto.lat.input_slot_left[i]) {
	    datagram_done(conn->proto.lat.input_pak[i]);
					/* Return the packet */
	    conn->proto.lat.input_slot_left[i] = 0; /* Mark slot as free */
	    credits++;
	}
    }

    conn->proto.lat.slot_empty = 0;	/* Reset empty pointer */
    conn->proto.lat.slot_fill = 0;	/* Reset fill pointer */

    if (credits && sb) {
	sb_signal(sb, 0, credits);
    }

    if (lat_debug) {
	buginf("\nLAT%t: Input buffer cleared",tty->ttynum);
    }
}

/*
 * lat_clearoutput
 *
 * Clear bufferred output and send an attention-abort slot
 */

static void lat_clearoutput (
    tt_soc *tty)
{
    sb_ptr sb = tty2sb(tty);
    conntype *conn = tty->conntop;

    conn->proto.lat.datahead = conn->proto.lat.databuf; /* Clear head ptr */
    conn->proto.lat.datatail = conn->proto.lat.databuf; /* And tail pointer */
    conn->proto.lat.datacount = 0;	/* Clear the count */

    if (sb)
      sb_signal(sb, SB_FLUSH, 0);		/* Send the attention slot */
    if (lat_debug) {
	buginf("\nLAT%t: Output buffer cleared",tty->ttynum);
    }
}

/*
 * lat_startoutput
 * Send off a buffer of bytes
 */

static boolean lat_startoutput (
    tt_soc *tty)
{
    sb_ptr sb;
    conntype *conn = tty->conntop;

    /*
     * Get session block pointer and return if session dying
     */
  
    if ((sb = tty2sb(tty)) == NULL) {
	return(FALSE);
    }

    if (tty->txservice || conn->proto.lat.datacount) {
	sb_signal(sb, SB_DATA_A, 0);
    }

#ifdef DEBUG
    if (lat_debug) {
	buginf("\nLAT%t: Output started",tty->ttynum);
    }
#endif

    return(TRUE);
}

/*
 * lat_inputpending
 * Return the number of unread bytes
 */

static int lat_inputpending (
    tt_soc *tty)
{
    conntype *conn = tty->conntop;

    return(lat_unread(conn));
}

/*
 * lat_outputblock - Return TRUE if output would block
 */

static boolean lat_outputblock (
     tt_soc *tty)
{
    conntype *conn = tty->conntop;

    /*
     * If there is space in the buffer, wake up
     */
    if (conn && conn->proto.lat.datacount < LAT_DATABUF)
       return(FALSE);

    /*
     * If connection is dead, flag it and wake up
     */
    if (tty2sb(tty) == NULL) {
        tty->statbits |= CARDROP;  
	return(FALSE);
    }

    /*
     * Block 
     */
    return(TRUE);
}

/*
 * lat_putc - Buffer an output character
 */

static void lat_putc (
     tt_soc *tty,
     int c)
{
    sb_ptr sb;
    conntype *conn = tty->conntop;
    uchar *t;

    /*
     * Get session block pointer and return if session gone
     */

    if ((sb = tty2sb(tty)) == NULL) {
	return;
    }

    /* Handle breaks through the signal mechanism */

    if (c == BREAK_CHAR) {
#ifdef NOTYET
	db_report_break(sb, 0);
#else
	conn->proto.lat.flags |= LAT_SEND_BREAK;
	db_report_config(sb);
#endif NOTYET
	return;
    }
    c &= tty->tty_datamask;

    /* Wait for output buffer space */

    while (conn->proto.lat.datacount >= LAT_DATABUF) {
	sb_signal(sb, SB_DATA_A, 0);	/* Get the lat process going */
	edisms((blockproc *)lat_outputblock, (ulong)tty);	/* Block on output done */
	if (tty->statbits & (CARDROP | IDLE))
	  return;			/* If session dying, get out */
	t = conn->proto.lat.datatail + 1; /* Reset buffer pointer if clear */
	if (t == conn->proto.lat.databuf + LAT_DATABUF) {
	    t = conn->proto.lat.databuf; /* Handle buffer wrapping */
	}
    }

    /* Check for buffering bug */

    if (conn->proto.lat.datacount &&
	conn->proto.lat.datahead == conn->proto.lat.datatail) {
	errmsg(&msgsym(BUFFULL, LAT), tty->ttynum, conn->proto.lat.datacount);
	return;
    }	    

    /* Time to store the character */

    t = conn->proto.lat.datatail + 1;	/* Get data tail pointer */
    if (t == conn->proto.lat.databuf + LAT_DATABUF) {
	t = conn->proto.lat.databuf;	/* Handle buffer wrapping */
    }
    *t = (uchar) c;			/* Store the character */
    conn->proto.lat.datacount++;	/* Update the count */
    conn->proto.lat.datatail = t;	/* And set the new tail */
}

static int lat_outputpending (
     tt_soc *tty)
{
    sb_ptr sb;
    conntype *conn = tty->conntop;

    if ((sb = tty2sb(tty)) == NULL) {
	return(0);
    }

    return(conn->proto.lat.datacount);	/* Return the count */
}

static boolean lat_setspeed (
    tt_soc *tty,
    int txspeed,
    int rxspeed)
{
    sb_ptr sb;

    /*
     * Get session block pointer and return if session gone
     */

    if ((sb = tty2sb(tty)) == NULL) {
	return(FALSE);
    }

    db_set_config(sb, DB_E_XMIT | DB_E_RECV, rxspeed, txspeed,
		  0, 0,
		  (tty->capabilities & SFLOWOUT) != 0,
		  (tty->capabilities & SFLOWIN) != 0);
    return(TRUE);
}

static void lat_paritybits (
    tt_soc *tty,
    int bits)
{
    sb_ptr sb;

    /*
     * Get session block pointer and return if session gone
     */

    if ((sb = tty2sb(tty)) == NULL) {
	return;
    }

    db_set_config(sb, DB_E_PARITY, 0, 0, 
		  parity_tolat[bits] | tty->tty_databits, 0,
		  (tty->capabilities & SFLOWOUT) != 0,
		  (tty->capabilities & SFLOWIN) != 0);
}

static void lat_databits (
    tt_soc *tty,
    int bits)
{
    sb_ptr sb;

    /*
     * Get session block pointer and return if session gone
     */

    if ((sb = tty2sb(tty)) == NULL) {
	return;
    }

    db_set_config(sb, DB_E_PARITY, 0, 0,
		  parity_tolat[tty->tty_parity] | bits, 0,
		  (tty->capabilities & SFLOWOUT) != 0,
		  (tty->capabilities & SFLOWIN) != 0);
}

static void lat_sendbreak (
    tt_soc *tty)
{
    sb_ptr sb;
    conntype *conn = tty->conntop;

    /*
     * Get session block pointer and return if session gone
     */

    if ((sb = tty2sb(tty)) == NULL) {
	return;
    }

#ifdef NOTYET
    db_report_break(sb, 0);
#else NOTYET
    conn->proto.lat.flags |= LAT_SEND_BREAK;
    db_report_config(sb);
#endif NOTYET
}

static void lat_setsflow (
    tt_soc *tty)
{
    sb_ptr sb;
    
    /*
     * Get session block pointer and return if session gone
     */

    if ((sb = tty2sb(tty)) == NULL) {
	return;
    }

    db_set_config(sb, 0, 0, 0, 0, 0,
		  (tty->capabilities & SFLOWOUT) != 0,
		  (tty->capabilities & SFLOWIN) != 0);
}

/*
 * tty2sb
 * Obtain a session block pointer given a terminal pointer.
 * Returns NULL if connection is dead or dying.
 */

static sb_ptr tty2sb (tty)
    tt_soc *tty;
{
    if (tty->statbits & (CARDROP|IDLE))
	return(NULL);

    if (tty->conntop == NULL && tty->type == VTY_LINE) {
	tty->statbits |= CARDROP;
	return(NULL);
    }

    if (tty->conntop == NULL)
	return (NULL);
    return((sb_ptr)(tty->conntop->tcb));
}

void lat_info_datab(conn, db)
     conntype *conn;
     data_b *db;
{
    tt_soc *tty = conn->tty;

    if (db->db_flags & DB_E_PARITY) {
	tty->tty_databits = db->db_char_parity & SLOT_PC_SIZE;
	if (db->db_char_parity & 0x10) {
	    tty->tty_parity = parity_fromlat[
	       (db->db_char_parity & 0x60) >> 5];
	} else {
	    tty->tty_parity = PARITY_NO;
	}
    }

    if ((db->db_flags & DB_E_RECV) && db->db_baud_recv){
	tty->tty_rxspeed = db->db_baud_recv;
    }

    if ((db->db_flags & DB_E_XMIT) && db->db_baud_xmit){
	tty->tty_txspeed = db->db_baud_xmit;
    }
}

/*
 * lat_host_init
 *
 * Initialize LAT host data structures.
 */

void lat_host_init ()
{
    vectortype *vector;

    vector = & LATvector;
    vector_init(vector);		/* initialize vector structure */

    /* Fill in all of the routine pointers for LAT virtual terminals */

    vector->vgetc = lat_getc;			/* character input */
    vector->vclearinput = lat_clearinput;	/* clear buffered input */
    vector->vinputpending = lat_inputpending;	/* check for pending input */

    vector->vputc = lat_putc;			/* character output */
    vector->vstartoutput = lat_startoutput;	/* start character output */
    vector->vclearoutput = lat_clearoutput;	/* clear pending output */
    vector->voutputpending = lat_outputpending;	/* get output bytes pending */
    vector->voutputblock = lat_outputblock;	/* Would output block */
    vector->vputpkt = generic_putpkt;		/* put packet routine */

    vector->vsetspeed = lat_setspeed;		/* Set speed */
    vector->vparitybits = lat_paritybits;	/* Set parity bits */
    vector->vdatabits = lat_databits;		/* Set data bits */
    vector->vsendbreak = lat_sendbreak;		/* Send break */
    vector->vsetsflow = lat_setsflow;		/* Set XON/XOFF flow control */
    vector->vservicehook = generic_servicehook;	/* set service hook */
    vector->vnoservicehook = generic_noservicehook; /* clear service hook */

    /* Initialize the queueing vector */

    lat_queuevector.vshow = (void (*)(void *))lat_show_entry;	/* Show a queue entry */
    lat_queuevector.vdelete = (void (*)(void *))lat_delete_entry;	/* Delete a queue entry */
    lat_queuevector.vstart = (boolean (*)(void *))lat_start_hic; /* Start a queue entry */
}
