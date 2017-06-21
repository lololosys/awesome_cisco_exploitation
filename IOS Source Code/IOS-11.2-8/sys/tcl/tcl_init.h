/* $Id: tcl_init.h,v 3.2 1995/11/17 18:47:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tcl/tcl_init.h,v $
 *------------------------------------------------------------------
 * tcl_init.h -- To make tcl a subsystem.
 * April 1994, Satish Gannu
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcl_init.h,v $
 * Revision 3.2  1995/11/17  18:47:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:34:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:04:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../tcl/tcl.h"

#define TCL_BREAKSET "\r\n\032?\t"
/*
 * structure definition used to add tcl information to tt_soc 
 * when tcl subsystem is linked
 */


void tcl_init (subsystype *);
void tcl_tty_init (tt_soc *);
void tclsh_command (parseinfo *);
void tcl_quit(void);
boolean  tcl_parse_cmd (parseinfo *);
void tcl_parser_init (void);
int tcl_print_buffer (char *, tt_soc *, const char *, va_list );
int tcl_parse_prompt (parseinfo *, char *, int *);
void tcl_putc (tt_soc *, int);

/*
 * Used to buffer the output from cisco parser
 */
vectortype tclbuf_vector;
