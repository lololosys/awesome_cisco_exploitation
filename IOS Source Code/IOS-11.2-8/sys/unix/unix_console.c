/* $Id: unix_console.c,v 3.3.28.1 1996/03/18 22:29:55 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/unix/unix_console.c,v $
 *------------------------------------------------------------------
 * unix_console.c - Console support for Unix images
 *
 * December 1991, Kirk Lougheed
 * May 1995, Scott Mackie
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_console.c,v $
 * Revision 3.3.28.1  1996/03/18  22:29:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  10:59:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/22  07:30:25  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  17:55:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:42:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:12:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "logger.h"
#include "ttysrv.h"
#include "config.h"
#include "unix.h"

/*
 * External and Forward Declarations
 */
extern tt_soc *console;
extern void serial_setbuffers(tt_soc *,int,int);
void unix_console_putc(tt_soc *, int);
extern int AuxBase;

/*
 * Local Storage
 */
vectortype consolevector;
vectortype auxvector;


/*
 * console_init
 * Initialize console to run under UNIX
 */
				
tt_soc *console_init (void)
{
    vectortype *vector;
    tt_soc *tty;

    vector = & consolevector;
    vector_init(vector);
    vector->vgetc = serial_getc;
    vector->vclearinput = serial_clearinput;
    vector->vputc = unix_console_putc;
    vector->vputpkt = generic_putpkt;
    vector->voutputblock = (boolean (*)(tt_soc *))return_false;
    vector->vinputpending = serial_inputpending;
    vector->voutputpending = serial_outputpending;
    tty = (tt_soc *) tty_init(0,READY,CTY_LINE,vector);
    if (!tty)
	return(NULL);
    serial_setbuffers(tty,MAXINCHARS,0);  /* set up buffering */ 
    return(tty);
}

/*
 * auxline_init
 * Initialize the auxiliary port.
 */

tt_soc *auxline_init (void)
{
    vectortype *vector;
    tt_soc *tty;

    vector = &auxvector;
    vector_init(vector);
    vector->vgetc = serial_getc;
    vector->vclearinput = serial_clearinput;
    vector->vinputpending = serial_inputpending;
    vector->vputc = serial_putc;
    vector->vstartoutput = (boolean(*)(tt_soc *))return_false;
    vector->vclearoutput = serial_clearoutput;
    vector->voutputpending = serial_outputpending;
    vector->vputpkt = generic_putpkt;
    vector->voutputblock = serial_outputblock;
    vector->voutputxoffed = serial_outputxoffed;
    vector->vsetspeed = (boolean(*)(tt_soc *, int, int))return_false;
    vector->vdtrstate = (void(*)(tt_soc *, boolean))return_nothing;
    vector->vservicehook = generic_servicehook;
    vector->vnoservicehook = generic_noservicehook;
    tty = (tt_soc *) tty_init(AuxBase,READY,AUX_LINE,vector);
    if (!tty)
	return(NULL);
    serial_setbuffers(tty,MAXINCHARS,MAXOUTCHARS); /* set up buffering */ 
    tty->tty_txspeed = ASYNC_DEFSPEED;
    tty->tty_rxspeed = ASYNC_DEFSPEED;
    tty->tty_databits = 8;
    tty->tty_parity = PARITY_NO;
    tty->tty_stopbits = STOPBITS2;
    return(tty);
}

/*
 * Console output routine.
 * Use UNIX stdio routines
 */

void unix_console_putc (tt_soc *tty, int c)
{
    unix_putc(c);
}

void console_putc (uchar ch)
{
    unix_putc(ch);
}


/*
 * unix_console
 * Process to do reads from UNIX stdin and put results in cisco tty buffers.
 */

#define BUFSIZ 100

forktype unix_console (tt_soc *tty)
{
    char ttybuffer[BUFSIZ];
    char *inptr;
    char c;
    int bytes;

    unix_setraw();
    while (TRUE) {
	edisms((blockproc *)unix_stdinBLOCK, 0);
	inptr = ttybuffer;
	bytes = unix_stdinread(inptr, BUFSIZ);
	if ((bytes == 0) || (bytes == -1))
	    continue;
	while (bytes--) {
	    c = *inptr++;
	    if (special_chars(tty, c) >= 0) {
		
		/*
		 * Handle buffer wrap
		 */
		if (tty->intail >= tty->inend)
		    tty->intail = tty->instart;

		/*
		 * Handle overflow
		 */
		if (tty->incount && (tty->intail == tty->inhead)) {
		    tty->overrun++;
		    if (modem_debug)
			ttybug(tty,"Input buffer overflow");
		    continue;
		}

		/*
		 * Add character to end of buffer
		 */
		*tty->intail++ = c;
		++tty->incount;
	    }
	}			
    }
}


