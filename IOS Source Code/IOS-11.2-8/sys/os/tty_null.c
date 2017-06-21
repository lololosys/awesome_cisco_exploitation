/* $Id: tty_null.c,v 3.1.16.1 1996/03/18 21:31:37 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/os/tty_null.c,v $
 *------------------------------------------------------------------
 * tty_null.c -- support routines for "null" terminal
 *  
 * November 1995, Nick Thille
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tty_null.c,v $
 * Revision 3.1.16.1  1996/03/18  21:31:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/09  05:10:39  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:02  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sched_private.h"
#include "ttysrv.h"


static tt_soc *null_io_tty;	/* pointer to null IO tty */

/*********************************************************************
 *
 *	       DISABLE & RE-ENABLE OUTPUT FOR THE CALLING PROCESS
 *			(redirect to /dev/null)
 *
 **********************************************************************
 */

/*
 * Various null IO vectors.  All just return as if everything has been
 * sent or received as the case may be.
 */
static int null_io_return_minus_one (tt_soc *tty)
{
    return(-1);
}

static int null_io_return_zero (tt_soc *tty)
{
    return(0);
}

static void null_io_just_return (tt_soc *tty)
{
    return;
}

static boolean null_io_return_false (tt_soc *tty) 
{
    return(FALSE);
}

static void null_io_int_param (tt_soc *tty, int int_param)
{
    return;
}

static int null_io_putpkt (tt_soc *tty, paktype *pak,
			   uchar *datastart, int len)
{
    return(len);
}

static boolean null_io_setspeed (tt_soc *tty, int txbaud, int rxbaud)
{
    return(TRUE);
}

static forktype null_io_autobaud (tt_soc *tty)
{
    return;
}

static void null_io_boolean_param (tt_soc *tty, boolean bool_param)
{
    return;
}

static boolean null_io_servicehook (tt_soc *tty, txserv_t txhook,
				    rxserv_t rxhook)
{
    return(FALSE);
}

static modem_status null_io_modemstuff (tt_soc *tty)
{
    modem_status modem_retval = { 0 };

    return(modem_retval);
}

static void null_io_pppmap (tt_soc *tty, ulong txmap, ulong rxmap)
{
    return;
}

static boolean null_io_ntpsetup (tt_soc *tty, ntp_setuptype type,
				 ulong pps_flags)
{
    return(FALSE);			/* This if doesn't support ntp */
}

vectortype null_io_vector = {	/* null IO vectors */
    null_io_just_return,		/* vsetup */
    null_io_return_minus_one,		/* vgetc */
    null_io_just_return,		/* vstartinput */
    null_io_just_return,		/* vclearinput */
    null_io_return_zero,		/* vinputpending */
    null_io_int_param,			/* vputc */
    null_io_return_false,		/* vstartoutput */
    null_io_just_return,		/* vclearoutput */
    null_io_return_zero,		/* voutputpending */
    null_io_return_false,		/* voutputblock */
    null_io_return_false,		/* voutputxoffed */
    null_io_putpkt,			/* vputpkt */
    null_io_setspeed,			/* vsetspeed */
    null_io_just_return,		/* vautosetspeed */
    null_io_autobaud,			/* vauto_baud */
    null_io_int_param,			/* vstopbits */
    null_io_int_param,			/* vparitybits */
    null_io_int_param,			/* vdatabits */
    null_io_just_return,		/* vsendbreak */
    null_io_boolean_param,		/* vdtrstate */
    null_io_just_return,		/* vsetflow */
    null_io_just_return,		/* vsetsflow */
    null_io_servicehook,		/* vservicehook */
    null_io_just_return,		/* vnoservicehook */
    null_io_just_return,		/* vportenable */
    null_io_just_return,		/* vportdisable */
    null_io_modemstuff,			/* vmodemsignal */
    null_io_modemstuff,			/* vmodemchange */
    null_io_just_return,		/* vstopoutput */
    null_io_int_param,			/* vpppmode */
    null_io_pppmap,			/* vpppmap */
    null_io_ntpsetup			/* vntpsetup */
}; 



/*
 * null_io_init
 * Initializes the null_io_vector, then initializes the null_io_tty to
 * use the null_io_vector.  Called in main as part of system startup
 * before enabling interrupts.
 * Note:  Sets tty_length to zero so the line will never be stopped at
 * a more prompt.  Initializes with ttynum of 0 because most processes
 * use console as controlling tty.
 */
void null_io_init (void)
{
    null_io_tty = malloc_named(sizeof(tt_soc), "TTY data");
    if (null_io_tty != NULL) {
	tty_init_data(null_io_tty, 0, READY, BAD_LINE, &null_io_vector);
	null_io_tty->tty_length = 0;
    }
}

/*
 * disable_io
 * Sets controlling tty for this job to null_io_tty, effectively
 * disabling all input from / output to stdio for this process.
 * Returns the old controlling tty to be used later when calling
 * process wants to return io functionally to normal.
 */
tt_soc *disable_io (void) 
{
    tt_soc *tmp_stdio = stdio;
    stdio = null_io_tty;
    forkx->ttyptr = null_io_tty;
    return(tmp_stdio);
}

/*
 * restore_io
 * Sets controlling tty for this job to the controlly tty passed in.
 * Used to restore normal io functionality after a disable_io() call.
 */
void restore_io (tt_soc *controlling_tty)
{
    stdio = controlling_tty;
    forkx->ttyptr = controlling_tty;
}


