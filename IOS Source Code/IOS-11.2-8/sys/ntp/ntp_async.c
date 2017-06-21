/* $Id: ntp_async.c,v 3.1.2.2 1996/08/19 18:54:43 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_async.c,v $
 *------------------------------------------------------------------
 * NTP async I/O support (for reference clocks)
 *
 * December 1995, Dave Katz
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntp_async.c,v $
 * Revision 3.1.2.2  1996/08/19  18:54:43  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1.2.1  1996/04/16  19:03:02  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  09:24:28  dkatz
 * Add placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CISCO

#include "master.h"
#include "ttysrv.h"
#include "ntpd.h"
#include "ntp_refclock.h"

/*
 * ntp_async_BLOCK
 *
 * Block waiting for a character to arrive
 *
 * We don't check anything but inputpending, since the "user" can't go
 * away!
 */
static boolean ntp_async_BLOCK (tt_soc *tty)
{
    return(!inputpending(tty));
}

/*
 * ntp_async_process
 *
 * Process incoming characters, building packets for the NTP process.
 *
 * The incoming stream contains a receive timestamp just after the
 * terminating character.  We forward the packet after consuming the
 * timestamp.
 *
 * If we're running a PPS-only clock, the only thing in the incoming
 * data stream will be a timestamp (so we better not get out of sync!)
 */
forktype ntp_async_process(tt_soc *tty)
{
    int ch;
    struct recvbuf *buf;
    uchar *bufptr;
    ulong timestamp_left;

    buf = NULL;
    bufptr = NULL;
    timestamp_left = 0;

    while (TRUE) {
	edisms((blockproc *)ntp_async_BLOCK, (long) tty);
	while (TRUE) {
	    ch = getc(tty);		/* Read a character */
	    if (ch < 0)			/* Nobody home */
		break;

	    /* Get a buffer if we haven't got one yet. */

	    if (buf == NULL) {		/* No buffer yet */
		buf = malloc(sizeof(*buf));
		if (buf) {

		    /*
		     * If we are expecting timecodes, set up to receive
		     * the time string.  Otherwise, we're just getting
		     * PPS pulses, so set up to receive the timestamp.
		     */
		    if (tty->capabilities2 & NTP_CLOCK_2) {
			bufptr = buf->recv_buffer;
			timestamp_left = 0;
		    } else {
			bufptr = (uchar *) &buf->recv_time;
			timestamp_left = sizeof(buf->recv_time);
		    }
		    buf->fd = tty->ttynum;
		}
	    }

	    /* If we're reading the timestamp, stash it. */

	    if (timestamp_left) {
		*bufptr++ = ch;

		/* If this is the last of it, hand off the packet. */

		timestamp_left--;
		if (!timestamp_left) {
		    ntp_refclock_input(buf);
		    buf = NULL;
		}

	    } else {

		/* Reading the main part of the packet.  Stash the byte. */

		*bufptr++ = ch;
		buf->recv_length++;

		/*
		 * If we hit the terminating character, shift to timestamp
		 * reading mode.
		 */
		if (ch == tty->ts_char_1 || ch == tty->ts_char_2) {
		    timestamp_left = sizeof(buf->recv_time);
		    bufptr = (uchar *) &buf->recv_time;

		} else {

		    /*
		     * Not timestamp mode.  If we just filled the buffer,
		     * dump it.  Something's bad here.
		     */
		    if (buf->recv_length >= BMAX) {
			free(buf);
			buf = NULL;
			if (ntp_packets_debug) {
			    buginf("\nNTP buffer overflow on tty %d",
				   tty->ttynum);
			}
		    }
		}
	    }
	}				/* Inner loop */
    }					/* Outer loop */
}
