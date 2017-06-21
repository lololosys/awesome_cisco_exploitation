/* $Id: lat.c,v 3.4.28.6 1996/08/28 12:56:04 thille Exp $
 * $Source: /release/112/cvs/Xsys/lat/lat.c,v $
 *------------------------------------------------------------------
 * lat.c -- cisco user level LAT code, interfaces to meridian portable LAT 
 *
 * November 1989, Bill Westfield
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * LAT technology licensed from Meridian Technology, Copyright 1989.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lat.c,v $
 * Revision 3.4.28.6  1996/08/28  12:56:04  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.28.5  1996/07/29  19:02:13  snyder
 * CSCdi64402:  declare arrays in lat that should be const, const
 *              save 1084 data, 68 out of image
 * Branch: California_branch
 *
 * Revision 3.4.28.4  1996/06/28  23:20:31  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.28.3  1996/06/12  14:27:51  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.4.28.2  1996/04/16  19:02:37  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.4.28.1  1996/03/18  20:39:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.1  1996/03/07  10:04:09  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/22  06:36:34  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/20  10:47:54  billw
 * CSCdi44088:  LAT service advertisement rating wraps with more than 256
 * vtys
 *
 * Revision 3.2  1995/11/17  17:33:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:13:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/20  05:06:28  smackie
 * Remove scary pointer arithmetic from LAT buffer handling. Use a direct
 * embedded pointer to the packet header instead. (CSCdi40672)
 *
 * Revision 2.6  1995/07/14  07:02:35  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.5  1995/06/26 16:13:07  vandys
 * CSCdi35265:  Should use intfc description if no line description
 * Missed a case here
 *
 * Revision 2.4  1995/06/20  03:44:44  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.3  1995/06/09  23:39:54  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.2  1995/06/09  13:11:06  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:24:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "mtcsys.h"
#include "config.h"
#include "latsb.h"
#include "laths.h"
#include "latls.h"
#include "latslt.h"
#include "latob.h"
#include "lathi.h"
#include "latcb.h"
#include "latas.h"
#include "latnd.h"
#include "laterr.h"
#include "latfnc.h"
#include "sysfnc.h"
#include "ttysrv.h"
#include "connect.h"
#include "latmsg.h"
#include "latgroup.h"
#include "termqueue.h"
#include "name.h"
#include "logger.h"
#include "../lat/msg_lat.c"		/* Not a typo, see logger.h */
#include "lat.h"
#include "access.h"
#include "../tcp/tcp.h"
#include "../ui/command.h"
#include "registry.h"
#include "../h/cs_registry.regh"
#include "../ui/debug.h"
#include "lat_debug.h"
#include "../os/os_debug.h"
#include "../h/auth_registry.regh"

/*
 * Forward references
 */

static void lat_swap_saved(tt_soc *, conntype *);

static void lat_showdatab(tt_soc *tty, conntype *conn,
			  void (*proc) (const char *, ...));

/*
 * local storage
 */

const char parity_tolat[] = { SLOT_PC_NONE, SLOT_PC_ODD,
			SLOT_PC_EVEN, SLOT_PC_SPACE, SLOT_PC_MARK };
const int parity_fromlat[] = { PARITY_SPACE, PARITY_ODD, PARITY_EVEN, PARITY_MARK };

/*
 * lat generic error codes.
 * index by 0x000X0 nibble in error code.
 */
static char *const lat_genericerrors[] = {
	NULL,
	"Memory allocation failure",
	"Name not found	",
	"Next name not found",
	"Non-unique name",
	"Identifier not found",
	"Non-unique identifier",
	"Reach limit of array",
	"Reach limit of struct",
	"Reached limit of sessions",
	"Operation disabled",
	"Operation illegal",
	"Invalid argument",
	"Invalid service class",
	"Reserved - code E0",
	"Reserved - code F0"
};

/*
 * lat facility error codes - index by 000x in the lat_error field
 */

static char *const lat_facilities[] = {
    "Unspecified",
    "Advertised service",
    "Learned service",
    "Node structure",
    "Object structure",
    "Session structure",
    "Circuit structure",
    "Host-initiated",
    "Queued request",
    "Reserved - code 9",
    "Reserved - code A",
    "Reserved - code B",
    "Reserved - code C",
    "Reserved - code D",
    "Reserved - code E",
    "Local host"
  };
      

/*
 * lat slot error codes - these are based on the LAT protocol
 */
char *const lat_sloterrors[]= {
	"No reason specified",
	"User requested disconnect",
	"System shutting down",
	"Bad slot number received",
	"Bad service class",
	"No resources available",
	"Service in use",
	"No such service",
	"Service is disabled",
	"Service not offered by requested port",
	"Port name is unknown",
	"Password required but not sent",
	"Entry is not in the queue",
	"Immediate access rejected",
	"Access denied",
	"Corrupted solicit request",
	"Bad command message",
	"Can not start request",
	"Bad request parameters"
};

/*
 * lat circuit error codes
 */

static char *const lat_circuit_errors[] = {
    "Not an error code",
    "No more sessions on circuit",
    "Invalid message received",
    "User level halt",
    "No progress on circuit",
    "Circuit timeout",
    "Retransmission limit reached",
    "Insufficient resources",
    "Timer out of range",
    "Maximum virtual circuits exceeded"
  };

/*
 * sx_get_char
 * Get characters from terminal and put them in a slot.
 * Based on the template supplied in sxgetc.p
 */

int sx_get_char (sb_ptr sb, of_byte ptr, int max)
{
    int n;

    reg_invoke_lat_sx_get_char( (long)sb->sb_sc,
		  sb, ptr, max, &n);
    return(n);
}

void term_sx_get_char (sb_ptr sb,	/* Pointer to session block     */
		       of_byte ptr,	/* Pointer to slot space        */
		       int max,		/* Maximum slot space available */
		       int *n)		/* Character to return		*/
{
    conntype *conn = (conntype *) sb->sb_sid;
    tt_soc *tty = conn->tty;
    int count = 0;
    int c;

    if (!(conn->status & CONN_INBOUND) && (tty->type != VTY_LINE)) {

	/*
	 * This is an outbound session not on a VTY, so we can safely
	 * get characters using getc().
	 */

	if (conn->proto.lat.flags & LAT_USER_INPUT) {
	    conn->proto.lat.flags &= ~LAT_USER_INPUT;
	    while ((c = getc(tty)) >= 0) {
		if (c < 256) {
		    count++;
		    *ptr++ = c;
		    if (--max <= 0) {
			if (inputpending(tty)) {
			    sb->sb_flags |= SB_DATA_A;
			    conn->proto.lat.flags |= LAT_USER_INPUT;
			}
			break;
		    }
		} else {
		    ungetc(tty, c);
		}
	    }
	}
    } else if (conn->status & CONN_INBOUND && tty->type == VTY_LINE &&
	       tty->txservice && !((tty->statbits & SLIPMODE) || 
	       tty->arapptr)) {
	/*
	 * This is an inbound session on a VTY, and we have a txservice
	 * hook established. Use it to get the data, if not vty async or arap.
	 */
	c = (*(tty->txservice))(tty, ptr, max);
	ptr += c;
	count += c;
	max -= c;
	if (max <= 0) {
	    sb->sb_flags |= SB_DATA_A;	/* Look for more */
	}
    } else {

	/*
	 * No special cases, look in the connection buffer for data
	 */

	while ((count < max) && (conn->proto.lat.datacount != 0)) {
	    ++conn->proto.lat.datahead;	/* Point to the next byte */
	    if (conn->proto.lat.datahead ==
		conn->proto.lat.databuf + LAT_DATABUF) {
		conn->proto.lat.datahead = conn->proto.lat.databuf;
		/* Handle buffer wrapping */
	    }
	    *ptr++ = *conn->proto.lat.datahead; /* Move the character */
	    count++;			/* Count one more byte done */
	    conn->proto.lat.datacount--; /* And one less byte bufferred */
	}
	
	/* Set up to send more data if we still have data bufferred
	 */
	
	if (conn->proto.lat.datacount != 0) {
	    sb->sb_flags |= SB_DATA_A;
	} else {
	    if (conn->proto.lat.datahead != conn->proto.lat.datatail) {
		errmsg(&msgsym(NOSYNC, LAT), tty->ttynum);
	    }
	}
    }

#ifdef DEBUG
    if (latpkt_debug) {
	buginf("\nLAT%t: %d bytes of output data queued",tty->ttynum,count);
    }
#endif

    *n = count;
    return;
}

/*
 * sx_put_char
 * Put characters to the users terminal process.
 * Based on the template supplied in sxputc.p
 */

void sx_put_char (sb_ptr sb,	/* Pointer to session block     */
		  if_byte ptr,	/* Pointer to slot space        */
		  int len)	/* Number of characters copied  */
{
/*-----------------------------------------------------------------------
    Get the session structure.  In this example, the pointer to the 
    session structure is the session block sid.
 -----------------------------------------------------------------------*/
#ifdef DEBUG
    if (latpkt_debug)
	buginf("\nLAT%t: lat_put_char(%lx, %d)", tty->ttynum, ptr, len);
#endif

    reg_invoke_lat_sx_put_char(sb->sb_sc,
		  sb, ptr, len);
}

void term_sx_put_char (sb_ptr sb, if_byte ptr, int len)
{
    conntype *conn;
    tt_soc *tty;
    paktype *pak = lat_currentpak;

    conn = (conntype *)sb->sb_sid;
    tty = conn->tty;

    /*
     * sanity checking.  Make sure slot looks like it is with pak,
     * and length matches when we were told.
     */
    if (!pak || (ptr < (byte *)pak->datagramstart) ||
        (ptr > (pak->datagramstart + DLL_MAX))) {
	errmsg(&msgsym(BADDATA, LAT), tty->ttynum);
	sb->sb_lcl_credits++;		/* discard the data ! */
	sb->sb_flags |= SB_CREDITS;
	return;
    }

    if (conn->proto.lat.input_slot_left[conn->proto.lat.slot_fill]) {
	errmsg(&msgsym(REUSE, LAT), tty->ttynum,
	       conn->proto.lat.slot_empty, conn->proto.lat.slot_fill);
	sb->sb_lcl_credits++;		/* Return the credit */
	sb->sb_flags |= SB_CREDITS;
	return;
    }

    /*
     * If this is an inbound session with an rxservice hook established,
     * pass the data to it.
     */
    if ((conn->status & CONN_INBOUND) && (tty->type == VTY_LINE) &&
	    tty->rxservice) {
	(*(tty->rxservice))(tty, ptr, len);
	sb->sb_lcl_credits++;	/* Return the credit */
	sb->sb_flags |= SB_CREDITS; /* Set the flag to send it */
	return;
    }

    conn->proto.lat.input_pak[conn->proto.lat.slot_fill] = pak;
					/* Save packet pointer */
    conn->proto.lat.input_slot[conn->proto.lat.slot_fill] = (uchar *)ptr;
					/* And data pointer */
    conn->proto.lat.input_slot_left[conn->proto.lat.slot_fill] = len;
					/* And length */
    pak_lock(pak);			/* Lock the packet */

    if (++conn->proto.lat.slot_fill == conn->proto.lat.max_credits) {
	conn->proto.lat.slot_fill = 0;	/* Update pointer and handle wraps */
    }
}

/*
 * sx_signal
 * React to special information on the LAT connection.  This is either
 * a DATA_B (terminal parameter) or an ATTENTION packet. Note that we do
 * not need to check for service class here, as the LAT engine only
 * processes DATA_B and ATTENTION slots for service class 1.
 */
void sx_signal (sb, reason)
    sb_ptr sb;
    int reason;
{
    conntype *conn = (conntype *) sb->sb_sid;
    tt_soc *tty = (tt_soc *)conn->tty;
    int i;
    data_b *db;

    switch(reason) {
      case SX_FLUSH:
	if (!(conn->status & CONN_INBOUND)) {
	    if (tty->type == VTY_LINE) {
		conn->status |= CONN_FLUSH;
	    } else {
		clearoutput(tty);
	    }
	}
	for (i = 0; i < conn->proto.lat.max_credits; i++) {
	    if (conn->proto.lat.input_slot_left[i]) {
		datagram_done(conn->proto.lat.input_pak[i]); /* Return it */
		conn->proto.lat.input_slot_left[i] = 0; /* Mark slot as free */
		sb->sb_lcl_credits++;	/* Count another credit */
	    }
	}
	if (sb->sb_lcl_credits) {
	    sb->sb_flags |= SB_CREDITS;
	}
	conn->proto.lat.slot_empty = 0;	/* Reset empty pointer */
	conn->proto.lat.slot_fill = 0;	/* Reset fill pointer */

	if (lat_debug) {
	    buginf("\nLAT%t: slot queue flushed", conn->tty->ttynum);
	}
	break;

      case SX_DATA_B_REP:		/* Got a report DATA-B */

	/* Check for break notification */

	if (sb->sb_rmt_data.db_config & SLOT_BREAK) {
	    if (conn->status & CONN_INBOUND) {
		clearinput(conn->tty);
	    }
	    conn->status |= CONN_CHANGED;
	    conn->proto.lat.config |= SLOT_BREAK;
	    sb->sb_rmt_data.db_config &= (~SLOT_BREAK);
	}

	/* We just received a report message telling us about the remote
	 * system port characteristics. We only use this for inbound
	 * sessions to VTYs, where the local port information tracks the
	 * remote reports.
	 */

	if ((conn->status & CONN_INBOUND) && (conn->tty->type == VTY_LINE))
	  lat_info_datab(conn, &sb->sb_rmt_data);
	break;

      case SX_DATA_B_SET:		/* Got a set DATA-B */

	/* By now, we have a message that we are willing to process */

	conn->status |= CONN_CHANGED;
 
	/* First of all, parse the configuration byte and modify our
	 * copy of it. Note that we might receive several DATA_B messages
	 * before the user process has a chance to react, so we only
	 * modify the fields in the session block extension which are
	 * marked as having changed in the received DATA_B message.
	 *
	 */

	db = &sb->sb_ob->ob_tmp_data;	/* Get pointer to data b data */

	if (db->db_config & SLOT_INP_ON) {
	    conn->proto.lat.config &= ~SLOT_INP_OFF; /* Clear flow off bit */
	    conn->proto.lat.config |= SLOT_INP_ON; /* Mark flow on request */
	}

	if (db->db_config & SLOT_INP_OFF) {
	    conn->proto.lat.config &= ~SLOT_INP_ON; /* Clear flow on bit */
	    conn->proto.lat.config |= SLOT_INP_OFF; /* Mark flow off request */
	}

	if (db->db_config & SLOT_OUT_ON) {
	    conn->proto.lat.config &= ~SLOT_OUT_OFF; /* Clear flow off bit */
	    conn->proto.lat.config |= SLOT_OUT_ON; /* Mark flow on request */
	}

	if (db->db_config & SLOT_OUT_OFF) {
	    conn->proto.lat.config &= ~SLOT_OUT_ON; /* Clear flow on bit */
	    conn->proto.lat.config |= SLOT_OUT_OFF; /* Mark flow off request */
	}

	/* Process parity change requests */

	if (db->db_flags & DB_E_PARITY) {
	    conn->proto.lat.changes |= DB_E_PARITY;
	    conn->proto.lat.char_parity = db->db_char_parity;
	}

	/* Process baud rate changes */

	if (db->db_flags & DB_E_RECV) {
	    conn->proto.lat.changes |= DB_E_RECV;
	    conn->proto.lat.baud_recv = db->db_baud_recv;
	}

	if (db->db_flags & DB_E_XMIT) {
	    conn->proto.lat.changes |= DB_E_XMIT;
	    conn->proto.lat.baud_xmit = db->db_baud_xmit;
	}

	/* Process transparency mode changes */

	if (db->db_flags & DB_E_MODE) {
	    conn->proto.lat.changes |= DB_E_MODE;
	    conn->proto.lat.char_mode = sb->sb_data_b_lcl_mode;
	}
	break;

      default:
	if (lat_debug)
	  buginf("\nLAT%t: Unknown signal %#x", conn->tty->ttynum, reason);
	break;
    }	    
}

/*
 * sx_session
 * called when there is a change in session status
 */
void sx_session (sb_ptr sb, int reason)
{
    boolean ret = FALSE;

    reg_invoke_lat_sx_session(sb->sb_sc, sb, reason, &ret);

    if (ret) {
	return;
    }

    /* By now, we have a signal that is not supported by the receiving
     * service class handler. If this is an access request, we will
     * reject it; otherwise, we will simply ignore the signal.
     */

    if (reason == SX_AS_ACCESS) {
	if (lat_debug) {
	    buginf("\nLAT: Rejecting SC %d inbound connection", sb->sb_sc);
	}
	sb_reject(sb, SLOT_STOP_BAD_CLASS);
    } else {
	if (lat_debug) {
	    buginf("\nLAT: Unknown SC %d session state change %d", reason);
	}
    }
}

/*
 *  term_sx_session
 */
void term_sx_session (sb_ptr sb, int reason, boolean *ret)
{
    conntype *conn;

    conn =  (conntype *)sb->sb_sid;	/* Get connect block pointer */

    switch (reason) {
	/*
	 * Handle successful inbound connections
	 */
      case SX_AS_ACCESS:
	if (lat_debug) {
	    buginf("\nLAT: Got new inbound host connection");
	}
	lat_start_host(sb);
	*ret = TRUE;
	return;

	/*
	 * Handle successful outbound connections
	 */
      case SX_CONNECT:
	if (lat_debug) {
	    buginf("\nLAT%t: Connection complete.",
		   ((conntype *) sb->sb_sid)->tty->ttynum);
	}
	*ret = TRUE;
	return;

	/*
	 * Handle rejected outbound connections
	 */
      case SX_REJECT:
	conn->errorstring = sb->sb_stop_reason < DIM(lat_sloterrors) ?
	    lat_sloterrors[sb->sb_stop_reason] : "Unknown";
	if (lat_debug) {
	    buginf("\nLAT%t: Session rejected, reason = %s (%d)",
		   conn->tty->ttynum, conn->errorstring,
		   sb->sb_stop_reason);
	}

	/*
	 * If the session was rejected due to a bad password, then
	 * this is not a fatal error, so forget the error string.
	 */
	if (sb->sb_stop_reason == SLOT_STOP_PASSWORD) {
	    conn->errorstring = NULL;
	}
	*ret = TRUE;
	break;
	    
	/*
	 * Handle running sessions that are being terminated
	 */
      case SX_STOP:
	conn->errorstring = sb->sb_stop_reason < DIM(lat_sloterrors) ?
	    lat_sloterrors[sb->sb_stop_reason] : "Unknown";
	if (lat_debug) {
	    buginf("\nLAT%t: Session stopped, reason = %s (%d)",
		   conn->tty->ttynum, conn->errorstring,
		   sb->sb_stop_reason);
	}

	/*
	 * If the stop code is <= SLOT_STOP_USER, then this is a
	 * normal termination, so forget the error text.
	 */
	if (sb->sb_stop_reason <= SLOT_STOP_USER) {
	    conn->errorstring = NULL;
	}
	*ret = TRUE;
	break;

	/*
	 * Underlying virtual circuit for this session has failed
	 */
      case SX_CIRCUIT:
	conn->errorstring = lat_circuit_error(sb->sb_cb->cb_halt_reason);
	if (lat_debug) {
	    buginf("\nLAT%t: Virtual circuit disconnected, reason = %s (%d)",
		   conn->tty->ttynum, conn->errorstring,
		   sb->sb_cb->cb_halt_reason);
	}
	*ret = TRUE;
	break;

	/*
	 * Solicit attempt has timed out
	 */
      case SX_TIMEOUT:
	if (lat_debug) {
	    buginf("\nLAT%t: Solicit timed out");
	}
	conn->errorstring = "Remote system is not responding";
	*ret = TRUE;
	break;

	/*
	 * No resources to handle solicit attempt
	 */
      case SX_RESOURCE:
	if (lat_debug) {
	    buginf("\nLAT%t: Solicit resource allocation failure");
	}
	conn->errorstring = lat_sloterrors[SLOT_STOP_NO_RESOURCE];
	*ret = TRUE;
	break;

	/*
	 * Handle a successful host initiated connection
	 */
      case SX_CONNECT_HIC:
	if (lat_debug) {
	    buginf("\nLAT%t: Host-initiated connection complete",
		   conn->tty->ttynum);
	}
	*ret = TRUE;
	return;
    }
    conn->tcb = NULL;	/* Dump session block pointer */
}

/*
 * hx_session
 * Process host-initiated status changes
 */
void hx_session (hi, reason)
    hi_ptr hi;
    int reason;
{
    termqueue *q;

    switch (reason) {
      case HX_ARRIVED:
	if (lat_debug) {
	    buginf("\nLAT: Host Initiated connection "
		   "from %*s to %*s:%*s, sc=%d",
		   LATSTRING(hi->hi_node), LATSTRING(hi->hi_service),
		   LATSTRING(hi->hi_port), hi->hi_sc);
	}

	if (hi->hi_sc == SVC_CLASS_XWIRE) {
	    if (!reg_invoke_xremote_hx_session(hi)) {
		hi_reject(hi, SLOT_STOP_BAD_CLASS);
	    }
	    break;
	}

	/* Implicitly reject SVC_CLASS_XFONT */

	if (hi->hi_sc != SVC_CLASS_TERM) {
	    hi_reject(hi, SLOT_STOP_BAD_CLASS);
	    break;
	}

	if (!lat_start_hic(hi)) {

	    /* At this point, we have discovered that the service is valid
	     * but in use. If this request allows queueing, we will enter
	     * it into the queue, otherwise, we will reject the request.
	     */

	    /* We could also support the notion of queueing allowed and
	     * not allowed; if queueing is not allowed for a service,
	     * rotary group, or port, we would return SLOT_STOP_SRV_IN_USE
	     * for CMD_ACCESS when going for a port where queueing is
	     * disabled. */

	    if (hi->hi_command == CMD_ACCESS) {	/* If queueing not requested */
		hi_reject(hi, SLOT_STOP_IMMED_REJECT); /* Reject the request */
	    } else {				/* Queueing is allowed */
		q = termqueue_enqueue(hi, &lat_queuevector);
						/* Get a queue block */
		if (q) {			/* If we got it */
		    hi->hi_qr = (qr_ptr) q;	/* Save queue block ptr */
						/* Try to get a queue block */
		    hi->hi_que_no = q->id;	/* Set the queue entry ID */
		} else {
		    hi_reject(hi, SLOT_STOP_NO_RESOURCE); /* Reject request */
		}
	    }
	}
	break;

      case HX_CANCEL:
	if (lat_debug) {
	    buginf("\nLAT: Host Initiated connection from %*s to %*s:%*s cancelled",
		   LATSTRING(hi->hi_node), LATSTRING(hi->hi_service),
		   LATSTRING(hi->hi_port));
	}
	if (hi->hi_sc == SVC_CLASS_TERM)
	  termqueue_delete((termqueue *)hi->hi_qr); /* Delete the entry */
	break;
#if LE_SUBJECT_HIC
      case HX_TIMEOUT:
      case HX_REJECTED:
      case HX_STATUS:
      case HX_ACCEPT:
#endif /* LE_SUBJECT_HIC */
      default:
	if (lat_debug)
	  buginf("\nLAT: hx_session call with reason %d to %*s:%*s:%*s, sc=%d",
		 reason, LATSTRING(hi->hi_node), LATSTRING(hi->hi_service),
		 LATSTRING(hi->hi_port), hi->hi_sc);
    }
}

/*
 * ax_set_rate
 * Set our lat services "rating".  This allows a less loaded server to
 * be selected if it offers the same service. If the service is not associated
 * with a rotary group, we will return 255 - the number of processes on the
 * system. If this is associated with a rotary group, we will return the number
 * of free ports associated with the group.
 */

byte ax_set_rate (as)
    as_ptr as;
{
    int i;
    tt_soc *tty;
    int rating = 0;
    int max = 0;

    if (as->as_rotary) {
	for (i = 1; i < ALLlines; i++) {
	    tty = MODEMS[i];
	    if (tty->rotary == as->as_rotary) { 
		max++;
		if (!(tty->statbits & HASPROC))
		    rating++;
	    }
	}
    } else {
	max = nvtylines;
	for (i = VTYBase; i < VTYBase + nvtylines; i++) {
	    if ((MODEMS[i]->statbits & HASPROC) == 0)
		if (MODEMS[i]->transport_allowin & lat_connvector.type)
		    rating++;
	}
    }
    /*
     * Compute a ratio that fits in a single byte: ((available/max) * 255)
     * There can be a max of 1000 lines (vtys) of which all are free.
     * Retain old behavior (rating == free lines) if less than 255 lines.
     */
    if (max > 255)
	return((rating * 255) / max);
    else
	return(rating);		/* old code for normal boxes! */
}

void ox_session (sb_ptr sb,		/* Pointer to session block	*/
		 ob_ptr ob,		/* Pointer to object block	*/
		 int reason)		/* Session delete reason	*/
{
    boolean ret = FALSE;

    reg_invoke_lat_ox_session(sb->sb_sc, sb, ob, reason, &ret);

    if (ret) {
	return;
    }

    /* If we get here, we got an unrecognised signal */

    if (lat_debug) {
	buginf("\nLAT: Unknown ox_session reason %d for sc %d",
	       reason, sb->sb_sc);
    }
}

/*
 * term_ox_session
 */
void term_ox_session (sb_ptr sb, ob_ptr ob, int reason, boolean *ret)
{
    conntype *conn = (conntype *)sb->sb_sid;
    tt_soc *tty = conn->tty;
    data_b *db = &ob->ob_lcl_data;
    
    switch (reason) {
      case OX_START:
/*-----------------------------------------------------------------------
    If generating a start slot, fill in object name (ob_name), type
    (ob_lcl_data.db_type), and idl group flags (ob_idl).
 -----------------------------------------------------------------------*/
	ob->ob_name[0] = sprintf((char *)(&ob->ob_name[1]),
				 "%t", tty->ttynum);
	ob->ob_lcl_data.db_type = 0;	/* Preset no special flags */
	ob->ob_idl[0] = 0;		/* And no group code list */

	if (tty->capabilities & INSECURE) {
	    ob->ob_lcl_data.db_type |= SLOT_PF_REMOTE;
	}

	ob_str_move_sl(ob->ob_idl, tty->tty_lat_groupmask);
	*ret = TRUE;
	break;

      case OX_CONFIG_REP:
	/* We are here to generate a report DATA_B message. */

	/* Temporary code for setting break flags until Meridian fixes
	 * race condition in db_report_break
	 */
	    
	db->db_config = 0;

	if (conn->proto.lat.flags & LAT_SEND_BREAK) {
	    db->db_config |= SLOT_BREAK;
	    conn->proto.lat.flags &= ~LAT_SEND_BREAK;
	}

	db->db_flags = DB_E_PARITY | DB_E_XMIT | DB_E_RECV;
	db->db_char_parity = parity_tolat[tty->tty_parity] |
	    tty->tty_databits;
	db->db_baud_xmit = tty->tty_txspeed;
	db->db_baud_recv = tty->tty_rxspeed;
	*ret = TRUE;
	break;
    }
}

void hx_status(hi)

    hi_ptr	hi;			/* Pointer to HIC block		*/
{
    termqueue *entry, *q;
    int i;

    if (hi->hi_status == STS_ERROR)	/* Get out if we got an error */
      return;

    if (hi->hi_sc != SVC_CLASS_TERM)
      return;				/* Only status on terminal queues */

    entry = (termqueue *)hi->hi_qr;		/* Get the queue block */
    hi->hi_que_time = ELAPSED_TIME(entry->time) / ONESEC; /* Get time in queue */

    /* Calculate the position in the global queue */

    i = 0;					/* Initialize counter */
    for (q = termqueue_queuehead; q != NULL; q = q->next) {
	i++;					/* Count one more entry */
	if (q == entry)				/* If we found the entry */
	  break;				/* We're done */
    }

    if (q) {					/* If entry was found */

	/* We currently don't have any way to keep relative queue information
	 * for a queue entry, so we will report only the depth in the global
	 * queue.
	 */

    hi->hi_p_queue  = i;			/* Set relative position */
    hi->hi_q_queue  = i;			/* Set absolute position */

    } else {					/* If entry was not found */

	hi->hi_status = STS_ERROR;		/* Set status as error */
	hi->hi_error = SLOT_STOP_ENTRY_DELETED;	/* Say entry went away */
	errmsg(&msgsym(QBSPACED, LAT), q);
    }
}

/*
 * lat_openBLOCK
 * Wait for a connection to open, or get some form of error.
 */

boolean lat_openBLOCK (conn)
    conntype *conn;
{
    sb_ptr sb = (sb_ptr) conn->tcb;

    if (sb == NULL)	/* open error ? */
	return(FALSE);
    if (sb->sb_rmt_ses_id != 0)		/* connection open ? */
	return(FALSE);
    if (psiset(conn->tty))
	return(FALSE);
    return(TRUE);
}


/*
 * lat_perror - Print out the most recent LAT error.
 *
 * This routine prints out the most recent LAT error. The lat error is
 * stored in lat_error.
 */

void lat_perror (void)
{
    int facility;
    int code;

    facility = lat_error & 0xf; /* Pick up the facility code */
    code = (lat_error & 0xf0) >> 4;	/* Pick up the generic code */

    printf("\n%% %s LAT error: %s", lat_facilities[facility],
	   lat_genericerrors[code]);
}

/*
 * lat_circuit_error - Convert a circuit error code to a string
 */

char *lat_circuit_error (n)
     int n;
{
    if (n < DIM(lat_circuit_errors))
      return(lat_circuit_errors[n]);
    else
      return("Unknown");
}

/*
 * lat_open_connection
 * Do the work of actually opening a LAT connection (largely by calling
 * existing meridian LAT funtions).  Wait to find out whether it works.
 */
conntype *lat_open_connection (tty, name, node, port, flags, abortflag)
    tt_soc *tty;
    byte name[SV_STR];
    byte *node;
    byte *port;
    int flags;
    boolean *abortflag;
{
    conntype *conn = NULL;
    sb_ptr sb = NULL;
    ls_ptr ls;
    byte password[SV_STR+1];
    boolean retry = TRUE;
    byte rem_node[ND_STR];

    if (!lat_interfaces) {
	printf("\nLAT has not been enabled\n");
	return(NULL);
    }

    password[0] = 0;			/* No password specified yet */

    /* Allocate a connection block and data buffer */

    conn = getconnection(tty, &lat_connvector);

    if (conn) {
	if (tty->type == VTY_LINE) {
	    if (!lat_getdatabuf(conn)) {
		freeconnection(conn);
		conn = NULL;
	    }
	}
    }

    if (!conn) {
	printf(nomemory);
	return(NULL);
    }

    conn->debug = (flags & LATOPEN_DEBUG) ? TRUE : FALSE;
    conn->proto.lat.t_capabilities = tty->capabilities;
    conn->proto.lat.t_icapabilities = tty->capabilities;
    conn->proto.lat.t_parity = tty->tty_parity;
    conn->proto.lat.t_databits = tty->tty_databits;

    conn->proto.lat.max_credits = LAT_OUTBOUND_CREDITS;

    while (retry) {			/* While we should try some more */

	/*
	 * Look for the remote service. First, try to find the service
	 * with the highest rating known to be reachable. If that fails,
	 * try any service.
	 */
	
	ls = ls_show(name, node, NULL, ND_UNKNOWN | ND_UNREACH);
	if (!ls)
	  ls = ls_show(name, node, NULL, 0);

	if (ls) {
	    if (lat_compare_groups(ls->ls_nd->nd_acl, 
				   tty->tty_lat_groupmask)) {

  		/*
		 * Found a match, check access and try to make a connection
  		 */ 
		str_return(rem_node, ls->ls_nd->nd_node, ND_STR);
 		if (!regexp_accesscheck((uchar *)rem_node, 
					stdio->accessout, lat_accesslists)) {
 		    lat_error = OP_DISABLED;
		    if (!(flags & LATOPEN_QUIET))
		      printf("\n%% Connections to that host not permitted "
			     "from this terminal\n");
		    lat_close(conn);
		    return(NULL);
		} else {
		    sb = sb_open(name, conn, 0,
				 conn->proto.lat.max_credits,
				 node, port, password,
				 SVC_CLASS_TERM);
		}
	    } else {
		lat_error = NO_NAME;	/* Fake name not found */
	    }
	}

	if (!sb) {
	    if ((flags & LATOPEN_QUIET) == 0) {
		lat_perror();
	    }
	    lat_close(conn);
	    return(NULL);
	}

	conn->tcb = sb;

	/*
	 * Print out message indicating what we're doing
	 */
	if ((flags & LATOPEN_QUIET) == 0) {
	    printf("Trying %*s...", LATSTRING(name));
	    flush();
	}
	
	psion(conn->tty);

	edisms((blockproc *)lat_openBLOCK, (ulong)conn);

	if (psiset(conn->tty)) {
	    /*
	     * If lat_openBLOCK unblocked due to psiset, it could be
	     * too soon to close the connection. We'll send an illegal
	     * slot if we don't wait for the remote slot id. Hang out
	     * here for a while. Wait at most 3 seconds
	     */
	    int count;
	    count = 15;
	    while ((conn->tcb) && count--) {
		if (sb->sb_rmt_ses_id == 0) {
		    process_sleep_for(ONESEC/5);
		} else {
		    break;
		}
	    }
	    psioff(conn->tty);
	    if ((flags & LATOPEN_QUIET) == 0)
		printf("\n%% %s\n", tcperror2string(LOCAL_RESET));
	    retry = FALSE;
	    if (abortflag)
	      *abortflag = TRUE; /* Mark abort */
	    break;
	}

	if (conn->tcb) {		/* If connection succeeded */
	    if ((flags & LATOPEN_QUIET) == 0)
		printf("Open\n");
	    if (name) {
		sstrncpy(conn->hostname, (char *)&name[1], CONNLEN+1);
	    }
	    (void)startoutput(tty);	/* ensure output is running */
	    return(conn);
	}

	psioff(conn->tty);

	/*
	 * If we got an error string back, then explain the reason for
	 * the connection failure.
	 */

	if (conn->errorstring) {
	    if ((flags & LATOPEN_QUIET) == 0)
	      printf("\n%% %s\n", conn->errorstring);
	    retry = FALSE;
	    break;
	}

	/*
	 * No error string, so this must have been rejected due to a bad
	 * password.
	 */

	if (password[0]) {
	    printf("Invalid password\n");
	} else {
	    printf("Password required\n");
	}
	if (askstring(tty, (char *)&password[1], SV_STR, "Password: ",
		      30 * ONESEC, 1, TRUE)) {
	    password[0] = strlen((char *)&password[1]);
	} else {
	    retry = FALSE;
	    break;
	}
    }
    lat_close(conn);
    return(NULL);
}

/*
 * lat_userBLOCK
 * determine if a user LAT process has anything to do.
 */
static int lat_userBLOCK (
    conntype *conn)
{
    tt_soc *tty = conn->tty;

    conn->status |= CONN_CHECKALL;
    if (TIMER_RUNNING_AND_AWAKE(conn->dispatchtimer))
	return(FALSE);
    if (psiset (tty) || modemcheck (tty))
	return(FALSE);
    if (!SLEEPING(conn->notetime))
	return(FALSE);
    if (tty->tty_sessiontimeout && conn_session(conn, TRUE) == FALSE)
	return(FALSE);
    if (TIMER_RUNNING_AND_AWAKE(conn->logintimer))
	return(FALSE);
    if (!lat_alive(conn))		/* Unblock when session goes away */
      return(FALSE);
    conn->status &= ~CONN_CHECKALL;
    if (tty->statbits & LASTWASBREAK)
	return(FALSE);
    if (!(conn->proto.lat.flags & LAT_USER_INPUT) &&
	inputpending(tty) && (conn->proto.lat.datacount < LAT_DATABUF))
	return(FALSE);
    if (conn->status & (CONN_CHANGED|CONN_FLUSH))
	return(FALSE);
    if (conn->proto.lat.input_slot_left[conn->proto.lat.slot_empty] &&
	!outputblock(tty))
      return(FALSE);
    if (!outputpending(tty) && conn->tcb &&
	!(((sb_ptr)conn->tcb)->sb_lcl_credits) &&
	!(((sb_ptr)conn->tcb)->sb_ext_credits) &&
	!(conn->proto.lat.input_slot_left[conn->proto.lat.slot_empty]))
      return(FALSE);				/* Issue credits */
    if (conn->proto.lat.config & SLOT_BREAK)
      return(FALSE);
    return(TRUE);
}

/*
 * lat_run
 * Run a lat connection.  Note that a lot of the work will typically
 * be done by the lat process, rather than the user process.
 */
int lat_run (conn, resumeflag)
     conntype *conn;
     boolean resumeflag;
{
    tt_soc *tty = conn->tty;
    int c;
    int len;
    paktype *pak;
    uchar *data;
    uchar *t;
    boolean psistate;

    psistate = ((tty->statbits & PSIENA)); /* Save PSI flag */
    psioff(tty);			/* Assume it should be off */
    lat_swap_saved(tty, conn);		/* Restore terminal characteristics */
    conn->proto.lat.t_icapabilities = tty->capabilities;
    					/* Set interactive capabilities */

    if (conn->tcb && conn->status & CONN_CHANGED) {
	lat_datab(conn);
    }

    if (conn->tcb)
      db_report_config((sb_ptr)conn->tcb); /* Send out a report data_b */

    if (psistate && conn->proto.lat.cur_char_mode == SB_INTERACTIVE)
      psion(tty);

    GET_TIMESTAMP(conn->idletime);		/* no idle time yet */
    if (tty->tty_sessiontimeout) {
	start_sessiontimeout(conn, tty);
    } else
	TIMER_STOP(conn->sessiontimer);		/* no session time out yet */
    tty->statbits &= ~IDLETIMEOUT;	/* ... */
    while (TRUE) {
	edisms((blockproc *)lat_userBLOCK, (ulong)conn);
	if (conn->status & CONN_CHECKALL) {
	    conn->status &= ~CONN_CHECKALL;
	    if (tty->tty_sessiontimeout)
		(void) conn_session(conn, FALSE);
	    if (tty->statbits & IDLETIMEOUT) {
 	        lat_swap_saved(tty, conn);
		if (modem_debug)
		    buginf("\nTTY%t: Inactivity timeout", tty->ttynum);
	        return(IDLE_TIMEDOUT);
	    }
	    if (psiset(tty)) {
 	        lat_swap_saved(tty, conn);
		return(OK);
	    }
	    if (modemcheck (tty)) {
 	        lat_swap_saved(tty, conn);
	        return(CARRIER_DROP);
	    }
	    if (!lat_alive(conn)) {
		lat_swap_saved(tty, conn);

		/*
		 * If we have an error string, then the session was
		 * aborted due to some kind of error.
		 */

		if (conn->errorstring)
		  return(ABORTED);

		/*
		 * Not aborted, so must have been a clean exit
		 */

		return(END_OF_FILE);

	    }

	    /*
	     * Check notification timer
	     */

	    if (!SLEEPING(conn->notetime)) {
	        notification (conn);
		TIMER_START(conn->notetime, 30*ONESEC);
	    }
	} /* checkall */


	/* Handle new DATA_B messages */

	if (conn->status & CONN_CHANGED) {
	    if (lat_datab(conn) && conn->tcb) {
		if (psistate) {
		    if (conn->proto.lat.cur_char_mode == SB_INTERACTIVE)
		      psion(tty);
		    else
		      psioff(tty);
		}		    
		db_report_config((sb_ptr)conn->tcb);
	    }
	}

	/* Handle break notification */
	if (conn->proto.lat.config & SLOT_BREAK) {
	    if (lat_debug) {
		buginf("\nLAT%t: BREAK character received", tty->ttynum);
	    }
	    conn->proto.lat.config &= ~SLOT_BREAK;
	    putc(tty, BREAK_CHAR);
	}

	/*
	 * Handle input from the net (output to the tty)
 	 */

	if (conn->proto.lat.input_slot_left[conn->proto.lat.slot_empty]) {

	    /*
	     * Queue the current output packet for output.
	     */

	    if (tty->tty_sessiontimeout && (tty->capabilities&OUTIDLE))
		start_sessiontimeout(conn, tty);

	    pak = conn->proto.lat.input_pak[conn->proto.lat.slot_empty];
	    data = conn->proto.lat.input_slot[conn->proto.lat.slot_empty];
	    len = conn->proto.lat.input_slot_left[conn->proto.lat.slot_empty];

	    c = putpkt(tty, pak, data, len);

	    conn->proto.lat.input_slot[conn->proto.lat.slot_empty] += c;
	    conn->proto.lat.input_slot_left[conn->proto.lat.slot_empty] -= c;

	    if (!conn->proto.lat.input_slot_left[conn->proto.lat.slot_empty]) {
		if (++conn->proto.lat.slot_empty == 
		    conn->proto.lat.max_credits) {
		    conn->proto.lat.slot_empty = 0; /* Update empty pointer */
		}
		datagram_done(pak);	/* Free the packet */
	    }
	}	

	/*
	 * Check to see if we need to flush recent output
	 */

	if (conn->status & CONN_FLUSH) {
	    clearoutput(tty);
	    conn->status &= ~CONN_FLUSH;
	    if (conn->debug) {
		printf("[FLUSHED]");
	    }
	}

	/* 
	 * Now handle input from the tty (output to the net).
	 */

	if (conn->tcb &&
	    !(conn->proto.lat.flags & LAT_USER_INPUT) && inputpending(tty)) {
	    GET_TIMESTAMP(conn->idletime);
	    if (tty->tty_sessiontimeout) 
		start_sessiontimeout(conn, tty);
	    while (conn->proto.lat.datacount < LAT_DATABUF) { 
		c = getc(tty);

		if (c == -1 || (tty->statbits & ESCAPED) || !conn->tcb)
		  break;		/* Get out if no characters */

		if (c == BREAK_CHAR) {	/* Handle breaks */
#ifdef NOTYET
		    db_report_break((sb_ptr)conn->tcb, 0);
#else /* NOTYET */
		    conn->proto.lat.flags |= LAT_SEND_BREAK;
		    db_report_config((sb_ptr)conn->tcb);
#endif /* NOTYET */
		    if (conn->debug) {
			printf("[BREAK sent]");
		    }
		} else {
		    if (c & ESCAPED_FLAG) {
			if ((c & 0x1F) == ('O' & 0x1F)) {
			    sb_signal((sb_ptr)conn->tcb, SB_FLUSH, 0);
			    clearoutput(tty);
			    if (conn->debug) {
				printf("[FLUSH sent]");
			    }
			}
		    } else {		/* Real character by now */
			if (tty->type != VTY_LINE) {
			    conn->proto.lat.flags |= LAT_USER_INPUT;
			    ungetc(tty, c);
			    sb_signal((sb_ptr)conn->tcb, SB_DATA_A, 0);
			    break;
			} else {
			    if (conn->proto.lat.datacount &&
				conn->proto.lat.datahead ==
				conn->proto.lat.datatail) {
				errmsg(&msgsym(BUFFULL, LAT),
				       tty->ttynum, conn->proto.lat.datacount);
			    } else {
				t = conn->proto.lat.datatail + 1;
				/* Get tail ptr */
				if (t == conn->proto.lat.databuf +
				    LAT_DATABUF) {
				    t = conn->proto.lat.databuf;
				    /* Handle wraps */
				}

				*t = (uchar) c;	/* Store character! */
				conn->proto.lat.datatail = t; /* Update ptr */
				conn->proto.lat.datacount++; /* Update count */
			    }
			}
		    }
		}
	    }

	}
	/*
	 * If we still have a session, check on sending credits and
	 * starting up the LAT process.
	 */

	if (conn->tcb) {
	    if (!outputpending(tty) && 
		!(((sb_ptr)conn->tcb)->sb_lcl_credits) &&
		!(((sb_ptr)conn->tcb)->sb_ext_credits) &&
		!(conn->proto.lat.input_slot_left
		  [conn->proto.lat.slot_empty]))
	      sb_signal((sb_ptr)conn->tcb, 0, conn->proto.lat.max_credits);

	    if (conn->proto.lat.datacount)
	      sb_signal((sb_ptr)conn->tcb, SB_DATA_A, 0);
	}
    }   /* while (TRUE) */
}

/*
 * lat_swap_saved - Swap saved TTY parameters with the current ones
 */

static void lat_swap_saved (tty, conn)
    tt_soc *tty;
    conntype *conn;
{
    ulong saved_capabilities = tty->capabilities;
    int saved_parity = tty->tty_parity;
    int saved_databits = tty->tty_databits;

    restoreflow(tty, conn->proto.lat.t_capabilities);
    if (conn->proto.lat.t_databits != tty->tty_databits || 
	conn->proto.lat.t_parity != tty->tty_parity) {
	tty->tty_databits = conn->proto.lat.t_databits;
	setparity(tty, conn->proto.lat.t_parity);
    }

    conn->proto.lat.t_capabilities = saved_capabilities;
    conn->proto.lat.t_parity = saved_parity;
    conn->proto.lat.t_databits = saved_databits;
}

/*
 * lat_datab
 * process a "data b" LAT packet.  the info is in the lat object
 * associated with this terminal.
 */

boolean lat_datab (conn)
    conntype *conn;
{
    tt_soc *tty = conn->tty;
    int changed = 0;
    int newrxspeed;
    int newtxspeed;

    conn->status &= ~CONN_CHANGED;

    if (lat_debug)
	lat_showdatab(tty, conn, buginf);

    if (tty->defaults == NULL)
      tty_savedefaults(tty);

    if (conn->debug)
	printf("\n[Set");

    if (conn->tcb && (conn->proto.lat.changes & DB_E_MODE)) {

	/* Set the changed flag and update the current mode. */
	
	changed = TRUE;
	conn->proto.lat.cur_char_mode = conn->proto.lat.char_mode;

	switch (conn->proto.lat.char_mode) {
	  case SB_INTERACTIVE:
	    if (conn->debug)
	      printf(" Interactive,");
	    tty->capabilities = (tty->capabilities & (~(SFLOWIN|SFLOWOUT))) |
	      (conn->proto.lat.t_icapabilities & (SFLOWIN|SFLOWOUT));
	    break;
	  case SB_PASSALL:	
	    tty->capabilities &= ~(SFLOWOUT | SFLOWIN);
	    if (conn->debug)
	      printf(" Passall,");
	    break;
	  case SB_PASSTHRU:
	    tty->capabilities = (tty->capabilities & (~(SFLOWIN|SFLOWOUT))) |
	      (conn->proto.lat.t_icapabilities & (SFLOWIN|SFLOWOUT));
	    if (conn->debug)
	      printf(" Passthru,");
	    break;
	  default:
	    ((sb_ptr)conn->tcb)->sb_data_b_lcl_mode = SB_INTERACTIVE;
	    conn->proto.lat.cur_char_mode = conn->proto.lat.char_mode;
	    if (!(conn->status & CONN_INBOUND))
	      psion(conn->tty);
	    if (conn->debug)
	      printf(" Unknown,");
	    break;
	}
    }

    /*
     * If terminal has remote modification disabled (default), don't
     * change any terminal characteristics
     */
    if (!(tty->capabilities & REMOTE_MOD)){
       if (conn->debug)
          printf("]\n");
       if (lat_debug)
          buginf("\nLAT%t: DataB ignored", tty->ttynum);
       conn->proto.lat.config &= SLOT_BREAK; /* Clear all flags except break */
       conn->proto.lat.changes = 0;	/* Clear all change flags */
       return(FALSE);
    }

    if (conn->proto.lat.config & (SLOT_INP_OFF | SLOT_INP_ON | 
				  SLOT_OUT_OFF | SLOT_OUT_ON)) {
	changed = TRUE;
	
	if (conn->proto.lat.config & SLOT_INP_OFF) { /* flowcontrol OUT off */
	    conn->proto.lat.t_icapabilities &= ~SFLOWOUT;
	    if (conn->debug)
	      printf(" Flow out off,");
	}
	if (conn->proto.lat.config & SLOT_INP_ON) { /* flowcontrol OUT on */
	    conn->proto.lat.t_icapabilities |= SFLOWOUT;
	    if (conn->debug)
	      printf(" Flow out on,");
	}
	if (conn->proto.lat.config & SLOT_OUT_OFF) { /* flowcontrol IN off */
	    conn->proto.lat.t_icapabilities &= ~SFLOWIN;
	    if (conn->debug)
	      printf(" Flow in off,");
	}
	if (conn->proto.lat.config & SLOT_OUT_ON) { /* flowcontrol IN on */
	    conn->proto.lat.t_icapabilities |= SFLOWIN;
	    if (conn->debug)
	      printf(" Flow in on,");
	}
	if (!(conn->proto.lat.changes & DB_E_MODE)) {
	    conn->proto.lat.char_mode = conn->proto.lat.cur_char_mode;
	    conn->proto.lat.changes |= DB_E_MODE;
	}
    }

    if (changed)
      setsflow(tty);

    if (conn->proto.lat.changes & DB_E_PARITY) {
	changed |= ((conn->proto.lat.char_parity & SLOT_PC_SIZE)
		    != tty->tty_databits) |
	  ((conn->proto.lat.char_parity & SLOT_PC_PARITY)
	   != parity_tolat[tty->tty_parity]);
	(void)startoutput(tty);
	while (outputpending(tty))
	    process_sleep_for(ONESEC/5);
	if ((conn->proto.lat.char_parity & SLOT_PC_SIZE) == 7 ||
	    (conn->proto.lat.char_parity & SLOT_PC_SIZE) == 8)
	    tty->tty_databits = (conn->proto.lat.char_parity & SLOT_PC_SIZE);
	if (conn->proto.lat.char_parity & 0x10) {
	    setparity(tty,
	      parity_fromlat[(conn->proto.lat.char_parity & 0x60) >> 5]);
	} else {
	    setparity(tty, PARITY_NO);
	}

	if (conn->debug) {
	    char *parity;
	    switch (tty->tty_parity) {
	      case PARITY_NO:	parity = "none";	break;
	      case PARITY_ODD:	parity = "odd";		break;
	      case PARITY_EVEN:	parity = "even";	break;
	      case PARITY_SPACE:parity = "space";	break;
	      case PARITY_MARK:	parity = "mark";	break;
	      default:		parity = "UNKNOWN";	break;
	    }
	    printf(" Format %d:%s,", tty->tty_databits, parity);
	}
    }

    if (conn->proto.lat.changes & DB_E_RECV) {
	newrxspeed = conn->proto.lat.baud_recv;
    } else {
	newrxspeed = tty->tty_rxspeed;
    }

    if (conn->proto.lat.changes & DB_E_XMIT) {
	newtxspeed = conn->proto.lat.baud_xmit;
    } else {
	newtxspeed = tty->tty_txspeed;
    }
    
    if ((tty->tty_txspeed != newtxspeed) || (tty->tty_rxspeed != newrxspeed)) {
	changed = TRUE;
	(void)startoutput(tty);
	while (outputpending(tty))
	  process_sleep_for(ONESEC/5);
	setspeed(tty, newtxspeed, newrxspeed);
	process_sleep_for(ONESEC/2);
	if (conn->debug)
	  printf(" Speed %d/%d", newtxspeed, newrxspeed);
    }
    if (conn->debug)
	printf("]\n");

    conn->proto.lat.config &= SLOT_BREAK; /* Clear all flags except break */
    conn->proto.lat.changes = 0;	/* Clear all change flags */
    return(TRUE);
}

/*
 * lat_showdatab
 * show the data supplied in a LAT DATAB slot.
 */

static void lat_showdatab (tt_soc *tty, conntype *conn,
			   void (*proc) (const char *, ...))
{
    int c;
    char *mode = "Unknown";

    c = conn->proto.lat.config;

    proc("\nLAT%t: DataB:%s%s%s%s%s%s%s",
	 tty->ttynum,
	 ((c & SLOT_INP_OFF)	? " -FlowIn"	: ""),
	 ((c & SLOT_INP_ON)	? " +FlowIn"	: ""),
	 ((c & SLOT_OUT_OFF)	? " -FlowOut"	: ""),
	 ((c & SLOT_BREAK)	? " Break"	: ""),
	 ((c & SLOT_OUT_ON)	? " +FlowOut"	: ""),
	 ((c & SLOT_SET)	? " Set"	: ""),
	 ((c & SLOT_INFO)	? " Info"	: ""));

    c = conn->proto.lat.changes;

    proc(" %cParity %x", tty->ttynum,
	 (c & DB_E_PARITY) ? '*' : ' ',
	 conn->proto.lat.char_parity);

    switch (conn->proto.lat.char_mode) {
      case SB_INTERACTIVE:
	mode = "Interactive";
	break;
      case SB_PASSALL:
	mode = "Passall";
	break;
      case SB_PASSTHRU:
	mode = "Passthru";
	break;
    }

    proc("      %cMode %s(%x)", (c & DB_E_MODE) ? '*' : ' ',
	 mode, conn->proto.lat.char_mode);
    proc("      Speed %c%d/%c%d",
	 (c & DB_E_XMIT) ? '*' : ' ',
	 conn->proto.lat.baud_xmit,
	 (c & DB_E_RECV) ? '*' : ' ',
	 conn->proto.lat.baud_recv);
}

/*
 * lat_report
 * Report port parameters to remote system.
 */

boolean lat_report (conn)
     conntype *conn;
{
    if (conn->tcb)
      db_report_config((sb_ptr)conn->tcb);
    return(TRUE);
}

/*
 * lat_unread
 * return number of characters in slot input queue. For inbound connections,
 * this is pending input; for outbound connections, this is pending output.
 */

int lat_unread (conn)
    conntype *conn;
{
    int i;
    int count = 0;

    /* Count characters in the input slot queue */

    for (i = 0; i < conn->proto.lat.max_credits; i++) {
	count += conn->proto.lat.input_slot_left[i];
    }

    /* Count a pending break as another character */

    if (conn->proto.lat.config & SLOT_BREAK) {
	count++;
    }

#ifdef DEBUG
    if (lat_debug) {
	buginf("\nLAT%t: %d bytes of input pending",tty->ttynum, count);
    }
#endif
    return(count);
}

/*
 * lat_alive
 * Check whether connect is still active.  Note that the lat code will zero
 * conn->tcb when the lat session block is deleted.
 */

boolean lat_alive (conn)
    conntype *conn;
{
    sb_ptr sb = (sb_ptr) conn->tcb;

    if (lat_unread(conn) || (sb != NULL && ((sb->sb_flags & SB_STOP) == 0)))
	return(TRUE);
    /*
     * check if this is a sort of pseudo-connection, eg a LAT host initiated
     * connection that hasn't been fully opened yet...
     */
    if (!sb && conn->proto.lat.hiptr) {
	hi_ptr hi = conn->proto.lat.hiptr;
	/*
	 * Don't say conn is alive if the HIC actually got started, or
	 * if it has some sort of error...
	 */
	if ((hi->hi_status & (STS_ERROR|STS_STARTED)) == 0)
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * lat_close
 * Close a lat connection by issuing a STOP message.
 */
void lat_close (conntype *conn)
{
    int timeout = 300;
    int i;

    reg_invoke_aaa_acct_end(conn->tty->ustruct, OK, conn->acct_info,
			    0,0,0,0);
    if (conn->tcb) {

	/* Wait up to timeout seconds for any bufferred I/O to get sent
	 * to the remote system. The timeout is to guard against hangs
	 * which occur because the remote system hasn't given us any
	 * credits but acknowledges keepalive messages sent to it.
         * In this case,  we want to break out of the loop and
         * close the session anyway.
	 * Only do this if the connection is associated with a TTY.
	 */

	if (conn->tty) {
	    startoutput(conn->tty);
	    while (conn->tcb && conn->proto.lat.datacount && timeout--) {
		process_sleep_for(ONESEC);
	    }
	}

	/* Stop the session */

	if (conn->tcb) {
	    sb_stop((sb_ptr)conn->tcb, SLOT_STOP_USER);
	    if (lat_debug) {
		if (conn->tty)
		    buginf("\nLAT%t: Sent STOP message",conn->tty->ttynum);
		else
		    buginf("\nLATxx: Sent STOP message");
	    }
	}
    }

    for (i = 0; i < conn->proto.lat.max_credits; i++)
      if (conn->proto.lat.input_slot_left[i])
	datagram_done(conn->proto.lat.input_pak[i]); /* Return it */

    if (conn->proto.lat.databuf) {		/* If we have a data buffer */
	free(conn->proto.lat.databuf);	/* Free it now */
	conn->proto.lat.databuf = NULL;		/* Forget the pointer */
    }

    freeconnection(conn);
    return;
}

/*
 * show_system_lat_sessions - Show lat sessions opened by "system" processes
 *
 * This routine is used to show the LAT sessions that are not being used
 * by a specific terminal, but instead for "system" connections.
 */

static void show_system_lat_sessions (void)
{
    (void) reg_invoke_xremote_show_lat_font();
}

/*
 * show_lat_sessions
 * Show lat objects (endpoints of connections) and session data structures
 */

void show_lat_sessions (int line)
{
    int i;
    tt_soc *tty;
    conntype *conn;
    int firstline, lastline;
    int conn_index;
    int j;
    sb_ptr sb;
    nd_ptr nd;

    automore_enable(NULL);


    if (line == -1) {
	firstline = 0;
	lastline = ALLlines;
	show_system_lat_sessions();
    } else {
	firstline = line;
	lastline = line + 1;
    }
		
    for (i = firstline; i < lastline; i++) {
	tty = MODEMS[i];
	if (tty == NULL)
	    continue;
	conn = tty->conntop;
	if (conn && (sb = (sb_ptr)conn->tcb) && (conn->v == &lat_connvector)) {
	    nd = sb->sb_cb->cb_nd;
	    printf("\n\ntty%t, virtual tty from host ",i);
	    (void) print_host(conn);
	    lat_status(sb, nd);
	}
	conn_index = 0;
	while (TRUE) {
	    conn = (conntype *) tty->connhead.qhead;
	    for (j = 0; j < conn_index && conn; j++)
	      conn = conn->link;
	    if (!conn)
	      break;

	    if ((sb = (sb_ptr)conn->tcb) && (conn->v == &lat_connvector)) {
		nd = sb->sb_cb->cb_nd;
		printf("\n\ntty%t",i);
		if (get_tty_location(tty)) {
		    printf(" (%s)", get_tty_location(tty));
		}
		printf(", connection %d to service ",conn->number);
		(void) print_host(conn);
		lat_status(sb, nd);
	    }
	    conn_index++;
	}
	(void) reg_invoke_xremote_show_lat(tty);
    }
    automore_disable();
}

static char *const latsbflags[] = {
	"DataA",
	"DataB",
	"BREAK",
	"Flush",
	"Send Credits",
	"20",
	"40",
	"80",
	"100",
	"Delete silently",
	"Delete with stop",
	"Host Initiated",
	"Stalled",
	"Send Stop",
	"Send Start",
	"Send Reject"
	};
/*
 * lat_status
 * Print the status of a LAT session.  This includes session data
 * and node data.
 */

void lat_status (sb_ptr sb, nd_ptr nd)
{
    printf("\n\nSession data:");
    printf("\n  Name %*s, Remote Id %x, Local Id %x", LATSTRING(sb->sb_name),
	sb->sb_rmt_ses_id, sb->sb_lcl_ses_id);
    printf("\n  Remote credits %d, Local credits %d, Advertised Credits %d",
	sb->sb_rmt_credits, sb->sb_lcl_credits, sb->sb_ext_credits);
    printbits("\n  Flags: ", sb->sb_flags, latsbflags, DIM(latsbflags), NULL);
    printf("\n  Max Data Slot %d, Max Attn Slot %d, Stop Reason %d",
	sb->sb_max_data, sb->sb_max_attn, sb->sb_stop_reason);

    printf("\n\nRemote Node data:");
    show_lat_nd(nd);
}
