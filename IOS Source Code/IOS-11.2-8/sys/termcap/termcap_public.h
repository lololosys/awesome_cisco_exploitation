/* $Id: termcap_public.h,v 3.2.62.1 1996/05/13 07:18:19 bjthomas Exp $
 * $Source: /release/112/cvs/Xsys/termcap/termcap_public.h,v $
 *------------------------------------------------------------------
 * TERMCAP_PUBLIC -- Externs for public termcap routines
 *
 * April 1992, Nick Thille
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: termcap_public.h,v $
 * Revision 3.2.62.1  1996/05/13  07:18:19  bjthomas
 * CSCdi55624:  show keymap <specific> fails to find aliased names
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:50:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:36:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TERMCAP_EXPORTS_H_
#define __TERMCAP_EXPORTS_H_

#ifndef __TERMCAPGLO_H_
#include "termcapglo.h"
#endif

extern tty_3270def *termcap_list; /* pointer to termcap defs curses uses */
extern char default_termcap[];

/*
 * termcap.c
 */ 
extern int Ctgetent(char **bp, char *name, conntype *conn, boolean);
extern int tnchktc(void);
extern boolean tnamatch(char *np, char *Bp, boolean keymap);
extern char *Ctgetstr(char *id, char **area, conntype *conn);
extern int tlen(char *str);
extern void termcap_clearscreen(void);

/*
 * termcapglo.c
 */ 
extern termcap_globals_struct *memallocTermcap(void);
extern void freeTermcap(termcap_globals_struct *tg);

/*
 * tgoto.c
 */ 
extern char *Ctgoto(char *CM, int destcol, int destline, conntype *conn);

/*
 * tputs.c
 */ 
extern void Ctputs(char *cp, int affcnt, void (*outc )(char, conntype *),
		   conntype *conn);

#endif  __TERMCAP_EXPORTS_H_
