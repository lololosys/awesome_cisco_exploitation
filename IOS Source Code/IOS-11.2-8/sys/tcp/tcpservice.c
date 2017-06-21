/* $Id: tcpservice.c,v 3.3.28.2 1996/05/09 14:47:24 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/tcp/tcpservice.c,v $
 *------------------------------------------------------------------
 * tcpservice.c -- misc. TCP services
 *
 * June 1987, Greg Satz
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcpservice.c,v $
 * Revision 3.3.28.2  1996/05/09  14:47:24  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.28.1.18.1  1996/04/27  07:37:07  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.28.1.8.1  1996/04/08  02:10:15  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.28.1  1996/03/18  22:13:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.32.1  1996/04/05  03:19:38  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3  1996/01/16  03:38:01  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.2  1995/11/17  18:49:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:36:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:32:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/07/28  23:36:49  ahh
 * CSCdi37880:  TCP: tcp_read wakes up too often
 * Make sure the dally timer is running before checking for expiration.
 *
 * Revision 2.6  1995/07/12  23:45:43  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.5  1995/06/20 07:19:30  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/17  06:26:43  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.3  1995/06/16  04:29:45  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.2  1995/06/09  13:21:45  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:06:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h" 
#include "packet.h"
#include "../ip/ip.h"
#include "tcp.h"
#include "tcpinternal.h"
#include "ttysrv.h"
#include "../os/clock.h"

#define	IDLE_TIMEOUT	(3 * ONEMIN)


/*
 * tcpmiscBLOCK
 * scheduler test for misc. servers
 */

boolean tcpmiscBLOCK (tcbtype *tcb)
{
    /*
     * Connection still there
     */
    if (tcp_closing(tcb))
	return(FALSE);

    /*
     * Any unread input
     */
    if (tcp_unread(tcb))
	return(FALSE);

    /*
     * Check idle timer
     */
    if (TIMER_RUNNING_AND_AWAKE(tcb->dallytime))
	return(FALSE);

    /*
     * Check user break
     */
    if (tcb->tty)
	if (psiset(tcb->tty))
	    return(FALSE);

    return(TRUE);
}

/*
 * tcpecho_daemon
 * Perform TCP echo services
 */

process tcpecho_daemon (void)
{
    int i;
    uchar *ptr;
    int status;
    int count;
    tcbtype *tcb;

    if (!process_get_arg_ptr((void**)&tcb))
	process_kill(THIS_PROCESS);
    process_watch_timer(&tcb->dallytime, ENABLE);
    while (TRUE) {
	TIMER_START(tcb->dallytime, IDLE_TIMEOUT);
	process_wait_for_event();
	while (tcp_unread(tcb)) {
	    status = tcp_getstring(tcb, &ptr, &count, MAXINT);
	    if (status != OK)
		break;
	    for (i = 0; i < count; i++)
		(void) tcp_putbyte(tcb, ptr[i], TRUE);
	    tcp_putpush(tcb, TRUE, FALSE);
	    TIMER_START(tcb->dallytime, IDLE_TIMEOUT);
	}
	if (tcp_closing(tcb) || !SLEEPING(tcb->dallytime)) {
	    tcp_close(tcb);
	    break;
	}
    }
    tcp_decrement_server_count();
    process_kill(THIS_PROCESS);
}

/*
 * discard_daemon
 * Perform TCP discard services
 */

process tcpdiscard_daemon (void)
{
    uchar *ptr;
    int status;
    int count;
    tcbtype *tcb;

    if (!process_get_arg_ptr((void**)&tcb))
	process_kill(THIS_PROCESS);
    process_watch_timer(&tcb->dallytime, ENABLE);
    while (TRUE) {
	TIMER_START(tcb->dallytime, IDLE_TIMEOUT);
	process_wait_for_event();
	while ((count = tcp_unread(tcb))) {
	    status = tcp_getstring(tcb, &ptr, &count, MAXINT);
	    if (status != OK)
		break;
	}
	if (tcp_closing(tcb) || !SLEEPING(tcb->dallytime)) {
	    tcp_close(tcb);
	    break;
	}
    }
    tcp_decrement_server_count();
    process_kill(THIS_PROCESS);
}

/*
 * tcpchargen_daemon
 * Perform TCP chargen services
 */

#define NEXT_ASCII_WRAP(ch)	((((ch) - ' ' + 1) % ('~' + 1 - ' ')) + ' ')

#define CHARGEN_COLS		72

process tcpchargen_daemon (void)
{
    uchar s_char, ch;
    int col, bytes;
    tcbtype *tcb;

    if (!process_get_arg_ptr((void**)&tcb))
	process_kill(THIS_PROCESS);
    for (s_char = ' '; !tcp_closing(tcb); s_char = NEXT_ASCII_WRAP(s_char)) {
	/* Spit out a line. */
	col = 0;
	for (ch = s_char; !tcp_closing(tcb) && col < CHARGEN_COLS;
	     ch = NEXT_ASCII_WRAP(ch)) {
	    if (tcp_putbyte(tcb, ch, TRUE) == FALSE)
		edisms((blockproc *) tcpputBLOCK, (ulong) tcb);
	    col++;
	}
	 
	(void) tcp_putbyte(tcb, RETURN, TRUE);
	(void) tcp_putbyte(tcb, LINEFEED, TRUE);

	tcp_putpush(tcb, TRUE, FALSE);

	/* Get rid of any junk that came in so we can shut this pig down
	 * when the other side goes away.
	 */
	if ((bytes = tcp_unread(tcb)) != 0) {
	    uchar *junk;
	    int bytes_read;

	    while (bytes) {
		if (tcp_getstring(tcb, &junk, &bytes_read, ~0) != OK)
		    break;	/* just in case */
		bytes -= bytes_read;
	    }
	}
	process_suspend();
    }

    tcp_close(tcb);
    tcp_decrement_server_count();
    process_kill(THIS_PROCESS);
}


/*
 * tcpdaytime_daemon
 * Perform TCP daytime services
 */

process tcpdaytime_daemon (void)
{
    tcbtype *tcb;
    char time_buffer[80];
    ulong length;
    clock_epoch curtime;
    int error;

    /* Get the TCB pointer. */
    if (!process_get_arg_ptr((void**)&tcb))
	process_kill(THIS_PROCESS);

    /* Get the time and format it. */

    clock_get_time(&curtime);
    length = sprintf(time_buffer, "%CC\r\n", "%w, %F %T-%tZ", &curtime);

    /* Write it out and hang up the phone. */

    tcp_write(tcb, time_buffer, length, &error);
    tcp_close(tcb);

    /* Sayonara. */

    tcp_decrement_server_count();
    process_kill(THIS_PROCESS);
}
