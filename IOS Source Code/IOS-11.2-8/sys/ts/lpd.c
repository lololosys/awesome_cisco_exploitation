/* $Id: lpd.c,v 3.15.10.10 1996/08/28 13:18:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/ts/lpd.c,v $
 *------------------------------------------------------------------
 * LPD - implement a subset of the unix LPD protocol
 *
 *  September, 1994  By Bill Westfield
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lpd.c,v $
 * Revision 3.15.10.10  1996/08/28  13:18:13  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.15.10.9  1996/08/13  18:20:43  bjthomas
 * CSCdi64909:  Accommodate AIXs LPD usage & on lprm generate an optional
 * FF
 * Branch: California_branch
 *
 * Revision 3.15.10.8  1996/08/07  09:03:15  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.15.10.7  1996/08/01  19:49:22  bjthomas
 * CSCdi60835:  LPD doesnt honor no debug all or undebug all
 * Branch: California_branch
 *
 * Revision 3.15.10.6  1996/07/31  08:49:20  billw
 * CSCdi63342:  printer command defaults to newline-convert (regress
 * CSCdi44091)
 * Branch: California_branch
 *
 * Revision 3.15.10.5  1996/06/11  04:54:46  bjthomas
 * CSCdi57716:  DTR drops on async lines configured for LPD printing
 * Branch: California_branch
 *
 * Revision 3.15.10.4  1996/06/11  04:19:25  bjthomas
 * CSCdi55134:  LPD doesnt flush data of a cancelled job
 * Branch: California_branch
 *
 * Revision 3.15.10.3  1996/05/21  10:07:11  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.15.10.2  1996/05/04  02:08:29  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.15.10.1  1996/03/18  22:19:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.2.3  1996/03/16  07:44:52  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.11.2.2  1996/03/07  10:57:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11.2.1  1996/02/20  21:24:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.15  1996/02/22  14:39:18  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.14  1996/02/01  06:11:08  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.13  1996/01/29  07:30:38  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.12  1996/01/24  20:42:03  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.11  1996/01/22  07:29:56  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.10  1996/01/16  03:38:14  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.9  1996/01/09  10:59:19  billw
 * CSCdi45881:  LPD aborts jobs identified as postscript by host
 *
 * Revision 3.8  1996/01/09  09:50:36  billw
 * CSCdi46598:  DEBUG RADIUS Command Does Not Confirm Status (debug
 * printer too)
 *
 * Revision 3.7  1996/01/09  09:09:39  billw
 * CSCdi44091:  lpd should be able to output formfeed between jobs
 *
 * Revision 3.6  1995/12/28  18:30:19  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreight addresses
 *   to tcp_open() for feature portability.
 *
 * Revision 3.5  1995/12/17  18:39:52  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/12  02:08:14  billw
 * CSCdi42435:  LPD code has assumptions about where packet-boundries will
 * occur
 * this missed getting into 11.1
 *
 * Revision 3.3  1995/11/20  10:42:24  billw
 * CSCdi40721:  LPD newline-convert option inserts too many carriage
 * return chars.
 *
 * Revision 3.2  1995/11/17  18:55:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/10/16  22:56:11  snyder
 * CSCdi42258:  spellink errors
 *              lpd.c: printng -> printing
 *              callback.c: scipt -> script
 *
 * Revision 2.10  1995/09/29  08:50:31  billw
 * CSCdi39404:  LPD code can loop on errors
 * Break out of loop on TCP errors.
 *
 * Revision 2.9  1995/08/03  23:17:51  billw
 * CSCdi37670:  bugs in lpd cause remote server to shut down printer
 *
 * Revision 2.8  1995/07/06  21:46:18  ahh
 * CSCdi36734:  RCP: cannot stop file transfer on a VTY
 * Consume VTY input during file transfer so the TTY code will
 * notice when someone hits the escpae char.
 *
 * Revision 2.7  1995/06/28  09:32:06  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.6  1995/06/22  08:45:24  irfan
 * CSCdi36173:  lpd doesnt check malloc return code
 * Make lpd watch for malloc return code.
 *
 * Revision 2.5  1995/06/20  20:10:50  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/17  06:26:57  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.3  1995/06/13  16:37:29  syiu
 * CSCdi35685:  Async callback rotary should be in reverse oder
 *
 * Revision 2.2  1995/06/09  13:22:02  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:09:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../ts/msg_lpd.c"		/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "subsys.h"
#include "ttysrv.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../ui/command.h"
#include "../parser/parser_defs_config.h"
#include "../h/cs_registry.regh"
#include "../ip/ip.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../tcp/tcp.h"
#include "../ip/ip_registry.h"
#include "connect.h"
#include "../os/async.h"
#include "lpd.h"
#include "../ui/service.h"

#define IDLE_TIMEOUT (2*ONEMIN)

queuetype printerQ;

/*
 * Menus subsystem header and initialization.
 */
#define LPD_MAJVERSION 1
#define LPD_MINVERSION 0
#define LPD_EDITVERSION  1

static boolean lpd_debug = FALSE;
boolean printer_debug_inited = FALSE;

/*
 * lpd_accesscheck
 * Syn-time access check for tcp connection.  We don't know at the time of
 * connection whether access is ok, since we have to wait for the printer
 * name.  Therefore, allow access if there any printers exist at all.
 */
static boolean lpd_accesscheck (ulong sport, ulong dport, ipaddrtype fhost)
{
    if (QUEUEEMPTY(&printerQ))	/* no printers, deny access */
	return(FALSE);
    return(TRUE);
}

/*
 * Send an LPD-level ACK back to our peer.   This is just a null character.
 *
 * WARNING: printer_type lpt needs to be mem_lock'ed() because this routine
 * can block!!!
 */

static void lpd_ack (tcbtype *tcb)
{
    /*
     * send ack
     */
    tcp_putbyte(tcb, 0, TRUE);
    tcp_putpush(tcb, TRUE, FALSE);
}

/*
 * lpd_nak
 * Send an LPD-level NAK back to the peer.  In theory, this is any non
 * zero byte.  In reality, the bsd code has three special values.  '\1'
 * is the general NAK, can will cause the peer to try the same command
 * again in "a little while".  '\2' is a slower back-off, and indicates
 * that the queuing area is full (in respsonse to a read data file command),
 * and results in a 5 minute backoff.  Codes "less than 0" are equivilent
 * to closing the connection, and cause the peer to retry the entire process,
 * including the connection phase.
 *
 * We use 0xFF == -1, since we want the connection to close to let other
 * potential users in to that printer line.
 */

static void lpd_nak (printer_type *lpt, tcbtype *tcb, uchar *reason)
{
    tcp_putbyte(tcb, 0xFF, TRUE);
    if (lpd_debug && reason) {
	if (lpt) {
	    buginf("\nLPD: %s Sending NAK because %s", lpt->name, reason);
	} else {
	    buginf("\nLPD: Sending NAK because %s", reason);
	}
    }
#if 0
    int error, len = strlen(reason);
    (void) tcp_putstringblock(tcb, reason, len, &error);
#endif
    tcp_putpush(tcb,TRUE,FALSE);
}

static void lpd_tcp_putstringblock(tcbtype *tcb, uchar *msg)
{
    int error, len = strlen(msg);
    (void) tcp_putstringblock(tcb, msg, len, &error);
}

boolean lpdfinishedBLOCK (printer_instance *i_lpt)
{
    if (i_lpt->i_flags & LPD_F_ABORT)	/* wake up if job aborted */
	return(FALSE);

    if (!i_lpt->tcb->tty)		/* wake up if tty closed */
	return(FALSE);

    if (i_lpt->tcb->tty->statbits & CARDROP) /*wake up if line gone*/
	return(FALSE);

    if (tcp_closing(i_lpt->tcb))	/*wake up if tcb closing*/
	return(FALSE);

    if (!outputpending(i_lpt->tcb->tty))  /*wake up if output finished*/
	return(FALSE);

    return(TRUE);
}

/*
 * Scheduler block for control and receive job ie, NotData 
 * A simplified ttyblock, or an enhanced tcp_miscblock.  Wake up when there
 * is data we can send to the line, or if we are killed.
 */
boolean lpdNotDataBLOCK (printer_instance *i_lpt)
{
    if (!tcpmiscBLOCK(i_lpt->tcb)) 
	return(FALSE);

    if (i_lpt->i_flags & LPD_F_ABORT)	/* wake up if job aborted */
	return(FALSE);

    if (!i_lpt->tcb->tty)		/* wake up if tty closed */
	return(FALSE);

    if (i_lpt->tcb->tty->statbits & CARDROP) /*wake up if line gone*/
	return(FALSE);
    else
	return(TRUE);

}

/*
 * Scheduler block for data transfer
 * A simplified ttyblock, or an enhanced tcp_miscblock.  Wake up when there
 * is data we can send to the line, or if we are killed.
 * still block if the line is congested even if there is a tcp event
 * except if there is a close condition ie, tcp close, job aborted,
 * tty closed, or line gone.
 */
boolean lpddataBLOCK (printer_instance *i_lpt)
{
    if (i_lpt->i_flags & LPD_F_ABORT)	/* wake up if job aborted */
	return(FALSE);

    if (!i_lpt->tcb->tty)		/* wake up if tty closed */
	return(FALSE);

    if (i_lpt->tcb->tty->statbits & CARDROP) /*wake up if line gone*/
	return(FALSE);

    if (tcp_closing(i_lpt->tcb))	/*wake up if tcb closing*/
	return(FALSE);

    if (outputblock(i_lpt->tcb->tty))	/*don't wake up if flow controlled*/
	return(TRUE);

    return(tcpmiscBLOCK(i_lpt->tcb));
}

/*
 * lpd_newlinefixup
 * at great expense, turn unix newlines into CRLFs.  Sigh.
 */
static int lpd_newlinefixup (paktype *pak, uchar **ptr, int count)
{
    int added = 0;
    uchar *d, *s, *start = pak->data_area, *end;
    uchar c;

    end = pak_center(pak) + pak_get_pool_size(pak) + 1;
    /*
     * Figure out whether the start or the end of the packet has more room.
     * This allows the most CRs to be added without having the copy overlap
     * itself, which has bad side effects like destroying the original
     * data and replacing it with repeated CRs.
     */
    if ((end - pak->dataptr) < (*ptr - start)) {
	/*
	 * More room at beginning of packet, copy forward from beginning
	 */
	d = start;
	s = *ptr;
	while (count--) {
	    if ((count & 0xFF) == 0)
		process_suspend();	/* scheduler friendly */
	    c = *s++;		/* Get character */
	    if (c == LINEFEED) {
		added++;
		*d++ = RETURN;	/* Insert possible return */
	    }
	    *d++ = c;
	    if (d >= end) {
		/*
		 * There were was not enough room in the packet to add all the
		 * CRs we needed to add.  This should hardly ever happen.  If
		 * it does, we break and return the munged packet anyway - some
		 * data will have been lost!
		 */
		errmsg(&msgsym(MANYLF, LPD), stdio->ttynum);
		break;
	    }
	} /* While */
	*ptr = start;		/* Where the data is now */
	pak->dataptr = d;	/* one past our end (used by tcp) */
	if (lpd_debug)
	    buginf("\nlpd: fwd copy added %d returns", added);
	return(added);
    }
    /* ELSE  (if more data at end of packet ...
     * Copy the data backward to one byte past the end of the packet,
     * and allow the copy to procede to the very beginning of the
     * data area.  This allows ipheadersize + tcpheadersize + encsize +
     * unused data area CRs to be added to the packet.  Rely on the fact
     * that there is trailer area in the packet to ensure that we can go
     * one past the end of the packet.
     */


    d = end;			/* Desitination */
    s = *ptr + count;		/* Source */
    pak->dataptr = d+1;		/* update ptr, 1 past data. (used by tcp) */
    while (count--) {
	if ((count & 0xFF) == 0)
	    process_suspend();	/* scheduler friendly */
	c = *--s;
	*d-- = c;
	if (d <= start) {	/* check this and possible CR at same time */
	    /*
	     * There were was not enough room in the packet to add all the
	     * CRs we needed to add.  This should hardly ever happen.  If
	     * it does, we break and return the munged packet anyway - some
	     * data will have been lost!
	     */
	    errmsg(&msgsym(MANYLF, LPD), stdio->ttynum);
	    break;
	}
	if (c == LINEFEED) {
	    *d-- = RETURN;
	    added++;
	}
    }
    *ptr = ++d;
    if (lpd_debug)
	buginf("\nlpd: bkwrd copy added %d returns", added);
    return(added);
}

/*
 * lpd_getstring
 * Special version of tcp_getstring just for lpd.  If the printer is not
 * configured for raw output, convert all the newlines to crlfs, being
 * especially careful to update the total number of bytes that will be
 * "data" so that we stay in sync on the control connection.
 */
static int lpd_getstring (printer_instance *i_lpt, uchar **ptr, int *count)
{
    tcbtype *tcb = i_lpt->tcb;
    paktype *pak = tcb->curinputpak;

    /*
     * if there is a partial packet, return it using tcp_getstring.
     */
    if ( (i_lpt->lpt->flags & LPD_F_RAW) || (pak && pak->length > 0))
	return(tcp_getstring(tcb, ptr, count, i_lpt->dataleft));

    /*
     * Otherwise, get a fresh packet loaded into curinputpacket, and
     * then adjust the crap out of it adding CRs as appropriate!
     */
    if (OK == tcp_getstring(tcb, ptr, count, i_lpt->dataleft)) {
	int added;
	added = lpd_newlinefixup(tcb->curinputpak, ptr, *count);
	i_lpt->dataleft += added;
	*count += added;
	return(OK);
    }
    return(NO_DATA);
}

static void lpd_stat_print(char *buf, printer_instance *i_lpt, tcbtype *tcb)
{
    char *p;
    int status;

    p = buf + sprintf(buf, "Printer %s, Job %s from %s"
			"  Printed %d of %d bytes Line %d\n",
			i_lpt->lpt->name,i_lpt->jobnum,i_lpt->hostname,
			i_lpt->datasize - i_lpt->dataleft, i_lpt->datasize,
			i_lpt->i_ttynum);
    mem_lock(i_lpt->lpt);
    tcp_putstringblock(tcb, buf, (p-buf)+1, &status);
    tcp_putpush(tcb, TRUE, FALSE);
    free(i_lpt->lpt); /*UNLOCK*/
}

static void lpd_strip_leading_zeros (char *s)
{
    char *t = s;

    while (*s == '0') {			/*skip leading zeros*/
	if (*s++ == '\0')
	    return;			/*no leading zeros*/	
    }

    while (*s != '\0') {		/*copy rest up to the end*/
	*t++ = *s++;
    }
    *t = *s;				/*copy the end*/
}

static boolean lpd_cmp (char *s, char *t)
{

    for ( ; *s == *t; s++, t++) {
	if (*s == '\0')
	    return(TRUE);
    }
    if (LPD_WHITE_SP(t) && *s == '\0')
	return(TRUE);
    
    return(FALSE);
}

/*
 * lpd_transfer_data
 * Does the transfer of an lpd "data file" to the tty line for actual
 * printing.  The data is read directly from the connection to the
 * printer tty line, with no intermediate buffer.  However, during this
 * period we pay careful attention to CARDROP on the tty, so that the
 * printing can be canceled.
 */

static boolean lpd_transfer_data (printer_instance *i_lpt, uchar *cmd, 
					int length)
{
    uchar *name;
    long size;
    uchar *ptr;
    int status, count;
    tcbtype *tcb = i_lpt->tcb;
    tt_soc *tty;

    if (cmd[length-1] != LINEFEED)
	return(FALSE);
    cmd[length-1] = 0;		/* null terminate file name */
    name = strchr(cmd, ' ');    
    *name++ = 0;		/* null terminate ascii count */
    size = parse_decimal_long(cmd+1, &status);
    if (size < 0)
	return(FALSE);
    if (lpd_debug)
	buginf("\nLPD %s: Read data file \"%s\", length %d", i_lpt->lpt->name,
	       name, size);
    bcopy(name+3, &i_lpt->jobnum[0], 3);    /* Copy 3 digits of job number */
    i_lpt->jobnum[3] = '\0';

    /*
     * lpq 076 or lprm 076 send us a job number of 76 (zero stripped).
     * but the job number includes leading zeros.  so we strip it here.
     */
    lpd_strip_leading_zeros(i_lpt->jobnum);
    sstrncpy(&i_lpt->hostname[0], name+6, LPD_STRSIZE);
    i_lpt->datasize = i_lpt->dataleft = size;
    lpd_ack(tcb);
    tty = i_lpt->tcb->tty;
    i_lpt->i_ndata++;

    while (i_lpt->dataleft > 0) {
	TIMER_START(tcb->dallytime, IDLE_TIMEOUT);
	edisms((blockproc *)lpddataBLOCK, (ulong)i_lpt);
	if (i_lpt->i_flags & LPD_F_ABORT) {
	    lpd_nak(i_lpt->lpt, tcb, "aborted");
	    return(FALSE);
	}
	if (!i_lpt->tcb->tty || tty->statbits & CARDROP || 
	   tcp_closing(i_lpt->tcb)) {
	    i_lpt->i_errors++;
	    lpd_nak(i_lpt->lpt, tcb, "cleared");
	    return(FALSE);
	}
	status = lpd_getstring(i_lpt, &ptr, &count);
	if (status == OK) {
	    status = putpkt(tty, tcb->curinputpak, ptr, count);
	    count -= status;	/* Get bytes not moved */
	    if (count)
		tcp_putback(tcb, count);	/* Put back the data */
	    i_lpt->dataleft -= status;
	    i_lpt->i_outbytes += status;
	} else
	    break;
    }
    if (i_lpt->dataleft <= 0)
	(void) tcp_getbyteblock(tcb, &count);
    if (i_lpt->lpt->flags & LPD_F_FFEED) {
	putc(tty, CH_FF);
	startoutput(tty);
    }
    /* can't allow multiple print jobs to be queued to the driver
     * because a Remove Job (lprm) would nuke all of them
     */
    while (outputpending(i_lpt->tcb->tty)) {
	TIMER_START(tcb->dallytime, IDLE_TIMEOUT);
	edisms((blockproc *)lpdfinishedBLOCK, (ulong)i_lpt);
	if (i_lpt->i_flags & LPD_F_ABORT) {
	    lpd_nak(i_lpt->lpt, tcb, "aborted");
	    return(FALSE);
	} else if (!i_lpt->tcb->tty || tty->statbits & CARDROP || 
	   tcp_closing(i_lpt->tcb)) {
	    i_lpt->i_errors++;
	    lpd_nak(i_lpt->lpt, tcb, "cleared");
	    return(FALSE);
	}
    }
    lpd_ack(tcb);
    return(TRUE);
}

/*
 * lpd_parse_controlfile
 * Read a control file and parse its contents, filling in the i_lpt data
 * in appropriate places.  This is normally useless, since most bsd systems
 * send teh control file after we have already received and printed the data.
 */
static boolean lpd_parse_controlfile (printer_instance *i_lpt, uchar *cmd, 
					int length)
{
    tcbtype *tcb = i_lpt->tcb;
    uchar *name;
    long size;
    uchar *ptr, *next;
    int status, count;
    uchar *ctrlptr;
    int ctrllen;

    if (cmd[length-1] != LINEFEED)
	return(FALSE);
    cmd[length-1] = 0;		/* null terminate file name */
    name = strchr(cmd, ' ');    
    *name++ = 0;		/* null terminate ascii count */
    size = parse_decimal_long(cmd+1, &status);
    if (size < 0)
	return(FALSE);
    if (lpd_debug)
	buginf("\nLPD%s: Read control file \"%s\", length %d",
	       i_lpt->lpt->name, name, size);
    lpd_ack(tcb);
    i_lpt->i_ncontrol++;
    /*
     * Malloc some memory and read in the entire controlfile.  This
     * way we don't have to worry about individual commands in the file
     * spanning multiple packets, or the closing ack being appended to
     * the last file packet, and so on.
     */
    ctrllen = 0;
    ctrlptr = malloc(size+1);
    if (!ctrlptr)
	return(FALSE);
    while (size > 0) {
	TIMER_START(tcb->dallytime, IDLE_TIMEOUT);
	edisms((blockproc *)lpdNotDataBLOCK, (ulong)i_lpt);
	if (i_lpt->i_flags & LPD_F_ABORT) {
	    free(ctrlptr);
	    lpd_nak(i_lpt->lpt, tcb, "aborted");
	    return(FALSE);
	}
	if (!i_lpt->tcb->tty || i_lpt->tcb->tty->statbits & CARDROP ||
	   tcp_closing(i_lpt->tcb)) {
	    i_lpt->i_errors++;
	    lpd_nak(i_lpt->lpt, tcb, "cleared");
	    return(FALSE);
	}
	/*
	 * Note that we are careful not to read more data than is part
	 * of the controlfile!
	 */
	status = tcp_getstring(tcb, &ptr, &count, size);
	if (status != OK) {
	    if (lpd_debug)
		buginf("\nLPD %s: tcp error %d while reading control file",
		       i_lpt->lpt->name, status);
	    free(ctrlptr);
	    return(FALSE);
	}
	bcopy(ptr, ctrlptr+ctrllen, count);
	size -= count;
	ctrllen += count;
    }
    ptr = ctrlptr;
    count = ctrllen;
    ctrlptr[count] = '\0';	/* null terminate strings, just in case */
    while (count > 0) {
	/*
	 * read through this buffer interpretting individual commands
	 */
	next = strchr(ptr,LINEFEED);
	if (next) {
	    *next = '\0';
	} else {
	    if (lpd_debug)
		buginf("\nLPD %s:, No terminating LF", i_lpt->lpt->name);
	    next = &ptr[count];
	}
	if (lpd_debug)
	    buginf("\nLPD %s: %d byte line in Ctrl file \"%s\"",
			i_lpt->lpt->name, next - ptr, ptr);
	switch (*ptr) {
	  case LPD_CF_HOST:
	    sstrncpy(i_lpt->hostname, ptr+1, LPD_STRSIZE);
	    break;
	  case LPD_CF_CLASS:
	    sstrncpy(i_lpt->classname, ptr+1, LPD_STRSIZE);
	    break;
	  case LPD_CF_USER:
	    sstrncpy(i_lpt->username, ptr+1, LPD_STRSIZE);
	    break;
	  case LPD_CF_JOB:
	  case LPD_CF_BANNER:
	  case LPD_CF_TEXT:
	  case LPD_CF_RAW:
	  case LPD_CF_POST:
	    break;
	  default:
	    if (strchr(LPD_CF_IGNORE, *ptr)) {
		/*
		 * If it Is a command we can ignore, eat this
		 * line and continue parsing.
		 */
		if (lpd_debug)
		    buginf("\nLPD %s: control line ignored: \"%s\"",
			   i_lpt->lpt->name, ptr);
	    } else {
		if (lpd_debug)
		    buginf("\nLPD %s: unrecognized \"%s\", [ignored]",
			   i_lpt->lpt->name, ptr);
	    }
	    break;
	}
	if (next) {
	    count -= (next - ptr) + 1;
	    ptr = next+1;
	} else {
	    if (lpd_debug)
		buginf("\nLPD %s: control file ended early",i_lpt->lpt->name);
	    count = 0;
	}
    }
    free(ctrlptr);
    (void) tcp_getbyteblock(tcb, &count);
    lpd_ack(tcb);
    return(TRUE);
}

/*
 * Receive an lpd job from our Peer.  This consists of input both
 * data and control files, and sending the data to the printer.
 * It is this function that extracts the physical printer data
 * (eg line number) from the logical printer name, and ties our
 * process to a terminal line.
 */
static boolean lpd_recvjob (printer_type *lpt, tcbtype *tcb)
{
    int ttynum = 0;
    tt_soc *tty = NULL;
    conntype *conn;
    boolean done, ctrlfile = FALSE;
    uchar *ptr;
    int count, status, i;
    printer_instance *i_lpt;

    lpt->ntcpconn++;

    /*
     * create a printer instance structure.
     * there are two cases: "line" or "rotary".
     * in the case of a "rotary" printer queue, there may be multiple
     * printer instance linked to a printer queue ie, printer type structure.
     * in the case of a "line" printer queue there will only be one
     * printer instance linked to a printer queue.  the printer instance
     * is free'ed in this routine once all processing for a job is complete.
     * in the case of a rotary queue, select_rotary() (below) determines 
     * if there is an available tty.  in the case of a line queue, lack
     * of a printer instance linked to the printer queue is the test
     * that determines if there is a free line ie, line is a sub set of the
     * rotary case. 
     */ 
    i_lpt = malloc(sizeof(printer_instance));
    if (!i_lpt) {
        lpt->errors++;
        lpd_nak(lpt, tcb, "No memory");
        return (FALSE);
    }

    /*
     * allocate the necessary tty and set it up as out "standard input"
     * (like a vty)  This will allow us to use the nice safe existing
     * parsing commands instead of having to do our own tcp to textstream
     * processing.  Later, the actual data will be sent direct from tcb
     * to tty, bypassing the tty vectors...
     */
    for (i = 0; i < lpt->acq_timeout; i++) {
	if (lpt->rotary) {
	    ttynum = select_rotary(lpt->rotary, &tcp_connvector, NULL,
				   ROTARY_ASCENDING);
	    if (ttynum > 0)
		break;
	} else {
	    if (!QUEUEEMPTY(&lpt->i_lptQ))
		break;				/*not much hope - busy*/
	    /* deal with modemsupport FSM delays and config's:
	     * printer xyz line n .. & printer abc rotary k ..
	     * with line n having a rotary k.
	     * see if line is truly available
	     */
	    if (select_line(lpt->ttynum, &tcp_connvector) > 0) {
		ttynum = lpt->ttynum;
		break;
	    }
	}
	process_sleep_for(ONESEC);
    }
    if (ttynum > 0) {		/* Assign us this line */
	for (count = 0; count < lpt->acq_timeout; count++) {
	    tty = tty_allocate(ttynum, TRUE);
	    if (tty)		/* Wait for line to free up, a little */
		break;
	    process_sleep_for(ONESEC);
	}
    }

    if (tty == NULL || tty->type == VTY_LINE || tty->type == CTY_LINE) {
	if (tty)
	    tty_deallocate(tty);
	/*
	 * For some reason, we could not allocate the printer line.
	 * clean up and return.
	 */
	lpt->errors++;

	lpd_nak(lpt, tcb, "Can't allocate printer line");
	free(i_lpt);
	return(FALSE);
    }
    /*
     * Check for access lists on the tty that might exclude this host.
     */
    if (check_access(tty, tcb->foreignhost.ip_addr,
		     tcb->localport,tcb->foreignport,FALSE) == FALSE) {
	lpd_nak(lpt, tcb, "Access denied");
	tty_deallocate(tty);
	lpt->errors++;
	free(i_lpt);
	return(FALSE);
    }

    conn = getconnection(tty, &tcp_connvector);
    if (conn == NULL) {
	lpt->errors++;
	lpd_nak(lpt, tcb, "No memory");
	free(i_lpt);
	return(FALSE);
    }

    i_lpt->lpt = lpt;
    enqueue(&lpt->i_lptQ, i_lpt);

    conn->tcb = tcb;			/* point to TCB */
    tcb->tty = tty;			/* point back to tty */
    tcb->ttynum = tty->ttynum;		/* set line number */
    i_lpt->tcb = tcb;
    i_lpt->i_ttynum = ttynum;		/* actual line used */

    conn->proto.tn.hostaddr = tcb->foreignhost;	/* remember host address */
    address_copy(&conn->address, &tcb->foreignhost);
    if (tcp_keepalivesin)
	tcb->flags |= TCB_KEEPALIVE;
    unqueue(&tty->connhead, conn);	/* remove from general q */
    tty->conntop = conn;		/* remember the driver conn */
    tty->statbits &= ~(CARDROP+HANGUP+IDLE);	/* reincarnated again! */
    tty->statbits |= READY;
    tty->conncount++;			/* another use */
    tty->ivector = & TCPvector;		/* this is a TCP vty */
    lpd_ack(tcb);
    done = FALSE;    

    while (!done) {
	TIMER_START(tcb->dallytime,IDLE_TIMEOUT);
	edisms((blockproc *)lpdNotDataBLOCK, (ulong)i_lpt);
	if (i_lpt->i_flags & LPD_F_ABORT)
	    break;
	if (!tcb->tty || tty->statbits & CARDROP ||
	   tcp_closing(i_lpt->tcb)) {
	    i_lpt->i_errors++;
	    break;
	}
	status = tcp_getstring(tcb, &ptr, &count, MAXINT);
	if (status != OK) {
	    if (status == END_OF_FILE)
		break;
	    if (lpd_debug)
		buginf("\nLPD%d: Bad tcp status %d", i_lpt->lpt->name, status);
	    break;
	}
	/*
	 * take actions on job commands
	 */
	switch (*ptr) {

	  case LPD_JOB_ABORT:
	    /*a call to this routine, lpd_recvjob() will not occur if 
	     * i_lpt->tcp != NULL, and lpd_transfer_data() will not
	     * return till the driver output is finished or aborted, and
	     * i_lpt->tcp is nuked on return in tcplpd_daemon().  so in
	     * short there is nothing to do.
	     */
	    done = TRUE;
	    lpd_ack(tcb);
	    break;

	  case LPD_JOB_CTRL:
	    if (!lpd_parse_controlfile(i_lpt, ptr, count)) {
		done = TRUE;
		break;
	    }		    
	    ctrlfile = TRUE;
	    break;

	  case LPD_JOB_DATA:
	    if (!ctrlfile) {
		if (lpd_debug)
		    buginf("\nLPD %s: data file before control",
			   i_lpt->lpt->name);
#if 0
		if (lat_debug) {
		    /*
		     * we insist on having gotten a control file before
		     * the data arrives.  Don't allow the data first!
		     */
		    i_lpt->i_errors++;
		    lpd_nak(i_lpt->lpt, tcb, "Must send control file before data");
		    /*			    done = TRUE; */
		    break;
		}
#endif
	    }
	    if (!lpd_transfer_data(i_lpt, ptr, count)) {
		done = TRUE;
	    }
	    break;

	  default:
	    if (lpd_debug) {
		ptr[count] = 0;
		buginf("\nLPD %s: Unknown job command %x (\"%s\")",
		       i_lpt->lpt->name, *ptr, ptr);
	    }
	    i_lpt->i_errors++;	
	    lpd_nak(i_lpt->lpt, tcb, "Unknown JOB command");
	    done = TRUE;
	    break;
	}
    }
    /*one or more of these conditions can be true so do clearoutput() here
     *only once per session.
     */
    if (tcb->tty && ((i_lpt->i_flags & LPD_F_ABORT) ||
       tty->statbits & CARDROP || tcp_closing(i_lpt->tcb))) {
	clearoutput(tcb->tty);    
    }
    if ((i_lpt->lpt->flags & LPD_F_FFEED) && (i_lpt->i_flags & LPD_F_ABORT) &&
       !(tty->statbits & CARDROP)) {
	putc(tty, CH_FF);
	startoutput(tty);
    }

    /* before freeing i_lpt update lpt's counters*/
    lpt->ncontrol += i_lpt->i_ncontrol;
    lpt->ndata += i_lpt->i_ndata;
    lpt->errors += i_lpt->i_errors;
    lpt->outbytes += i_lpt->i_outbytes;

    unqueue(&lpt->i_lptQ, i_lpt);
    free(i_lpt); /*not locked*/

    if (tcb->tty) {
	death(tcb->tty);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * lpd_daemon
 * Perform TCP lpd services.  The fork starts here.  Parse individual
 * commands and act accordingly.
 */
forktype tcplpd_daemon (register tcbtype *tcb)
{
    char buf[70];
    register int i;
    uchar *ptr, *qname, *p;
    int status;
    int count, cmd;
    char *buf_p;
    printer_type *lpt = NULL;
    printer_instance *i_lpt;
    int i_index;
    boolean rm_ack;

    if (lpd_debug)
	buginf("\nLPD: Connection started, remote host %s:%d",
	       address_string(&tcb->foreignhost), tcb->foreignport);
    if (tcb->foreignport > LPD_MAX_PORT) {
	/*
	 * Check to make sure this connection if from a properly
	 * "privledged" unix source port.  Print a unix-like message if not.
	 */
	if (lpd_debug) 
	    buginf(" Bad foreign port");
	i = sprintf(buf, "%s: /usr/lib/lpd: Malformed from address\n",
		    hostname);
	tcp_putstringblock(tcb, buf, i, &status);
	tcp_putpush(tcb, TRUE, FALSE);
	tcp_close(tcb);
	process_kill(THIS_PROCESS);
	return;
    }
    TIMER_START(tcb->dallytime, IDLE_TIMEOUT);
    edisms((blockproc *)tcpmiscBLOCK, (ulong)tcb);
    status = tcp_getstring(tcb, &ptr, &count, MAXINT);

    if (status == OK && ptr[count-1] == LINEFEED && 
       count >= LPD_MIN_TOP_CMD_SIZE) {

	cmd = *ptr;			/* Save comand character */
	ptr[--count] = '\0';		/* Null terminate the string */
	p = qname = ++ptr;		/* Skip command character */
	NEXT_LPD_FIELD(p);		/*get to the end of the qname*/
	if (*p != '\0') {
	    *p = '\0';			/*terminate qname as a string*/
	    ptr = ++p;			/*update ptr to additional text*/
	} else {
	    *p = '\0';			/*terminate qname as a string*/
	}	  	   
	/*
	 * Dispatch on type of command
	 */
	switch (cmd) {
	  case LPD_CMD_STAT:
	  case LPD_CMD_STATL:
	    if (lpd_debug)
		buginf("\nLPD: Send queue state %sfor %s %s",
		       cmd == LPD_CMD_STATL ? "(long) " : "", qname, ptr);
	    lpt = find_namedthing(&printerQ, qname);
	    if (lpt) {
		if (!QUEUEEMPTY(&lpt->i_lptQ)) {
		    mem_lock(lpt);
		    buf_p = malloc(LPD_BUF_SIZE);
		    if (buf_p) {
			/*AIX will send a list with just a WHITE SPACE for 
			 *all the queue status 
			 */
			if (!(*p == '\0' || (*p == ' ' && p[1] == '\0'))) {
			    p--;
			    do {
				p++;
				for (i_index = 0; 
				   (i_lpt =
				     (printer_instance *)queuepick(&lpt->i_lptQ,
				   i_index)); i_index++) {

				    if (lpd_cmp(i_lpt->jobnum, p)) {
					lpd_stat_print(buf_p, i_lpt, tcb);
					break;
				    }
				}
				NEXT_LPD_FIELD(p);
			    } while (*p != '\0');
			} else {/*no job's so its all*/
			    for (i_index = 0;
			       (i_lpt =
				  (printer_instance *)queuepick(&lpt->i_lptQ,
			       i_index)); i_index++) {
				lpd_stat_print(buf_p, i_lpt, tcb);
			    }
		        }
			free(buf_p);
		    }
		    free(lpt);/*UNLOCK*/
		} else
		    lpd_tcp_putstringblock(tcb, "Printer is idle\n");
	    } else {
		lpd_tcp_putstringblock(tcb, " unknown printer\n"); 
	    }
	    break;
			  
	  case LPD_CMD_PRINT:
	    if (lpd_debug)
		buginf("\nLPD: start printing for %s", qname);
	    break;

	  case LPD_CMD_RM:
	    /* per RFC1179 "Remove jobs" does not include the host name!
	     * so we are only able to delete by "job number"  which
	     * may not be unique.  got the user's name (Agent).  but
	     * the Receive data file includes "job number" and host name
	     * only.
	     */

	    if (lpd_debug)
		buginf("\nLPD: Remove job %s from %s", ptr, qname);
	    lpt = find_namedthing(&printerQ, qname);
	    if (!lpt) {
		lpd_nak(lpt, tcb, " unknown printer");
	    } else if (!QUEUEEMPTY(&lpt->i_lptQ)) {
		/*skip agent name to get to job numbers list. if any*/
		NEXT_LPD_FIELD(p);
		if (*p == '\0') {/*no job number nuke all instances*/
		    NEXT_I_LPT(i_lpt) {
			i_lpt->i_flags |= LPD_F_ABORT;
		    }
		    if (lpd_debug)
			buginf("\nLPD: all jobs removed");
		    rm_ack = TRUE;
		} else do {
		    p++;
		    rm_ack = FALSE;
		    NEXT_I_LPT(i_lpt) {
			if (lpd_cmp(i_lpt->jobnum, p)) {
			    i_lpt->i_flags |= LPD_F_ABORT;
			    rm_ack = TRUE;
			    if (lpd_debug)
				buginf("\nLPD:job %s removed", i_lpt->jobnum);
			}
		    }
		    NEXT_LPD_FIELD(p);
		} while (*p != '\0');
		if (rm_ack) {
		    lpd_ack(tcb);
		} else {
		    lpd_nak(lpt, tcb, " no job's matched");
		}
	    } else {
		lpd_nak(lpt, tcb, " no jobs queued");
	    }
	    break;
	
	  case LPD_CMD_RECV:
	    if (lpd_debug)
		buginf("\nLPD: Enter job %s", qname);
	    lpt = find_namedthing(&printerQ, qname);
	    if (!lpt) {
		lpd_nak(lpt, tcb, " unknown printer");
	    } else {
		mem_lock(lpt);
		if (lpd_recvjob(lpt, tcb)) {
		    free(lpt); /*UNLOCK*/
		    /*tcp_close has already been done*/
		    process_kill(THIS_PROCESS);
		    return;
		} else {
		    free(lpt); /*UNLOCK*/
		}
	    }
	    break;

	  default:
	    p = buf;
	    if (count > 10)
		count = 10;
	    for (i = 0; i < count; i++)
		p += sprintf(p, " %02x", *ptr++);
	    if (lpd_debug)
		buginf("\nLPD: got %s", buf);
	    /*can block*/
	    tcp_putbyte(tcb, 'N', TRUE);
	    tcp_putpush(tcb, TRUE, FALSE);
	    break;
	}
    }
    tcp_close(tcb);
    process_kill(THIS_PROCESS);
}

/*
 * tcp_listen_lpd_port
 * Performs processing for a LPD port request.
 */
void tcp_listen_lpd_port (tcbtype *tcb)
{
#ifdef LPD_PDEBUG
    if (lpd_manualdebug) {
	tcp_do_command(1, tcb, NULL, 0, 0, NULL, FALSE);
	return;
    }
#endif
    tcb->rcvwnd = 4 * tcb->maxsegmentsize;
    tcb->maxrcvwnd = tcb->rcvwnd;
    (void) cfork((forkproc *)tcplpd_daemon, (long)tcb, 0, "LPD daemon", 0);
}

/*
 * printer_cfgcommand
 * Do parsing of printer-related config commands.
 */
void printer_cfgcommand (parseinfo *csb)
{
    printer_type *lpt;
    printer_instance *i_lpt;

    if (csb->nvgen) {
	for (lpt = (printer_type *) printerQ.qhead; lpt; lpt = lpt->next) {
	    nv_write(TRUE, "printer %s ", lpt->name);
	    nv_add(lpt->ttynum, "line %d", lpt->ttynum);
	    nv_add(lpt->rotary, "rotary %d", lpt->rotary);
	    nv_add(!(lpt->flags & LPD_F_RAW), " newline-convert");
	    nv_add(lpt->flags & LPD_F_FFEED, " formfeed");
	    nv_add((lpt->acq_timeout != LPD_TTYWAIT), " jobtimeout %d",
		   lpt->acq_timeout);
	    nv_add(lpt->jobtypes[0], " jobtypes %s", lpt->jobtypes);
	}
	return;
    }
    lpt = find_namedthing(&printerQ, GETOBJ(string,1));
    if (csb->sense == FALSE) {
	if (lpt == NULL)
	    return;

	/* be tidy mark all printer instances for death.  otherwise
	 * they would run on till finished ie, lprm could not touch
	 * them.  the lpt->i_lptQ printer_instance's are free'ed in 
	 * lpd_recvjob()
	 */
	NEXT_I_LPT(i_lpt) {
	    i_lpt->i_flags |= LPD_F_ABORT;
	}
	unqueue(&printerQ, lpt);
	free(lpt); /*not locked*/
	return;
    }
    if (lpt == NULL) {
	lpt = malloc(sizeof(printer_type));
	if (lpt) {
	    sstrncpy(lpt->name, GETOBJ(string,1), NAMEDTHINGNAMELEN);
	    enqueue(&printerQ, lpt);
	    queue_init(&lpt->i_lptQ, 0);
	} else {
	    printf("\n%% No memory for Lpt definition");
	    return;
	}
    }
    sstrncpy(lpt->jobtypes, GETOBJ(string,2), 10);
    lpt->ttynum = GETOBJ(int,1);
    lpt->rotary = GETOBJ(int,2);
    if (GETOBJ(int,3))
	lpt->flags &= ~LPD_F_RAW;
    else
	lpt->flags |= LPD_F_RAW;

    if (GETOBJ(int, 4))
	lpt->flags |= LPD_F_FFEED;
    else
	lpt->flags &= ~LPD_F_FFEED;
    
    if (GETOBJ(int,5))
	lpt->acq_timeout = GETOBJ(int,6);

    if (!lpt->acq_timeout) lpt->acq_timeout = LPD_TTYWAIT;
}

/*
 * Exec command "show printers"
 * Show both overall printer status, and status of any active jobs.
 */

static const char show_lpt_hdr[] = 
     "Printer  Line  Rotary  Errors Connections Datafiles Controlfiles   Bytes";

void show_printers (parseinfo *csb)
{
    printer_type *lpt;
    printer_instance *i_lpt;
    int index, i_index;
    ulong ncontrol;
    ulong ndata;
    ulong errors;
    ulong outbytes;

    automore_enable(show_lpt_hdr);
    for (index = 0; (lpt = (printer_type *)queuepick(&printerQ, index)) ; 
       index++) {
	ncontrol = lpt->ncontrol;
	ndata = lpt->ndata;
	errors = lpt->errors;
	outbytes = lpt->outbytes;

	mem_lock(lpt);
	if (!QUEUEEMPTY(&lpt->i_lptQ)) {
	    NEXT_I_LPT(i_lpt) {
		ncontrol += i_lpt->i_ncontrol;		
		ndata += i_lpt->i_ndata;		
		errors += i_lpt->i_errors;		
		outbytes += i_lpt->i_outbytes;		
	    }
	}
	printf("\n%10s%-3t   %-3d  %-8d  %-8d   %-8d  %-8d  %-10d",
	       lpt->name, lpt->ttynum, lpt->rotary, errors,
	       lpt->ntcpconn, ndata, ncontrol, outbytes);
	/*
	 * if printer is active, give current status.
	 */
	if (!QUEUEEMPTY(&lpt->i_lptQ)) {
	    for (i_index = 0;(i_lpt =(printer_instance *)queuepick(&lpt->i_lptQ,
	       i_index)); i_index++) {
		mem_lock(i_lpt);
		printf("\n Job %s Line %d from %s printed %d of %d bytes",
			i_lpt->jobnum, i_lpt->i_ttynum, i_lpt->hostname,
			i_lpt->datasize - i_lpt->dataleft, i_lpt->datasize);
		free(i_lpt); /*UNLOCK*/
	    }
	}
	free(lpt); /*UNLOCK*/
    }
    automore_disable();
}

/*
 * Parser chains for printers.
 * Include these in printers.c to simplify makefile changes necessary to
 * add printers to an image.
 */

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ui/debug.h"

/*
 * LPD debugging support
 */
void printer_debug_all (boolean sense)
{
    /*
     * Printer debugging is NOT turned on by "debug all", but we
     * still want it to be turned off by "undebug all".
     */
    if (!sense)
	lpd_debug = FALSE;
}

void printer_debug_show (void)
{
    debug_show_flag(lpd_debug, FALSE, "LPD Printer");
}

void printer_debug_init (void)
{
    /* Repeated calls to this routine do nothing. */
    if (printer_debug_inited)
	return;
    /*
     * Register for "debug all" and "show debug" events.
     * Note that "debug printing" is NOT turned on by "debug all"
     */
    
    (void) reg_add_debug_show(printer_debug_show, "printer_debug_show");
    (void) reg_add_debug_all(printer_debug_all, "printer_debug_all");

    printer_debug_inited = TRUE;
}

void printer_debug_cmd (parseinfo *csb)
{
    lpd_debug = csb->sense;
    debug_show_flag(lpd_debug, TRUE, "LPD Printer");
}

/*
 * Parser chains and definitions for printers subsystem
 */

/*
 * Parse chains for "show printers" commands
 */
#define ALTERNATE       NONE
#include "exec_show_printer.h"
LINK_POINT(printer_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for Printer config commands
 */
#define	ALTERNATE	NONE
#include "cfg_printer.h"
LINK_POINT(lpd_config_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_printer.h"
LINK_POINT(printer_debug_commands, ALTERNATE);
#undef ALTERNATE


static parser_extension_request lpd_chain_init_table[] = {
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(lpd_config_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(printer_show_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(printer_debug_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * lpd_init
 * Initialize the LPD subsystem
 */
static void lpd_init (subsystype *subsys)
{
    parser_add_command_list(lpd_chain_init_table, "LPD");
    reg_add_tcp_listen(LPD_PORT, tcp_listen_lpd_port,
		   "tcp_listen_lpd_port");
    reg_add_tcp_access_check(LPD_PORT, lpd_accesscheck, 
			 "lpd_accesscheck");
    queue_init(&printerQ, 0);
    printer_debug_init();
}

SUBSYS_HEADER(LPD, LPD_MAJVERSION, LPD_MINVERSION, LPD_EDITVERSION,
	      lpd_init, SUBSYS_CLASS_PROTOCOL,
	      NULL, NULL);

