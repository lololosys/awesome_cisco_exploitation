/* $Id: ukernel_tmpstubs.c,v 3.1.16.1 1996/03/18 21:31:45 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/os/ukernel_tmpstubs.c,v $
 *------------------------------------------------------------------
 * ukernel_tmpstubs.c -- Temporary stub routine for building the
 *			 modular-ukernel target.
 *
 * David Hampton, February 1996
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ukernel_tmpstubs.c,v $
 * Revision 3.1.16.1  1996/03/18  21:31:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/09  05:10:43  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:11  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"

/*
 * From printf.c
 */
int _doprnt (char *cp, tt_soc *tty, const char *fmt, va_list argp,
	     boolean do_precision)		{ return 0; }
int printf (const char *fmt, ...)		{ return 0; }
int sprintf (char *cp, const char *fmt, ...)	{ return 0; }
int ttyprintf (tt_soc *tt, const char *fmt, ...){ return 0; }
void console_message (const char *str)		{ }
void flush (void) 				{ }
void printc (char ch) 				{ }
void ttyflush (tt_soc *tty)			{ }
void ttyprintc (tt_soc *tty, char ch)		{ }


/*
 * Console Support
 */
forktype unix_console (tt_soc *tty)		{ }
int console_check_break(void)  /*mips only */	{ return 0; }
int console_getc (void)				{ return 0; }
int serial_getc(tt_soc *tty)			{ return 0; }
tt_soc *console_init (void)			{ return 0; }
void console_putc (uchar ch)			{ }


/*
 * Platform Support
 */
void platform_exception_init (void)			{ }
void platform_main (void)				{ }
void platform_memory_init (void)			{ }
uint platform_get_value (platform_value_type type)	{ return 0; }
void platform_r4k_set_clock_params (ulong config_reg)	{ } /* mips only */
