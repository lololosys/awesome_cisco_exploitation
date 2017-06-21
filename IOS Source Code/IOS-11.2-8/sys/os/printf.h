/* $Id: printf.h,v 3.3.46.2 1996/06/05 15:05:44 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/printf.h,v $
 *------------------------------------------------------------------
 * printf.h - definitions for printf
 *
 * January 7, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module contains the definitions for the printf facility. All
 * definitions within the facility that are to be externally visible
 * are defined here.
 *
 *------------------------------------------------------------------
 * $Log: printf.h,v $
 * Revision 3.3.46.2  1996/06/05  15:05:44  widmer
 * CSCdi57346:  Add printf style formatting to readline prompt
 * Branch: California_branch
 *
 * Revision 3.3.46.1  1996/03/18  21:30:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/07  10:19:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  16:41:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/14  23:24:21  widmer
 * CSCdi45614:  Need vsprintf()
 *
 * Revision 3.2  1995/11/17  18:50:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:57:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern int _doprnt(char *, tt_soc *, const char *, va_list, boolean);
extern int ttyprintf(tt_soc *, const char *, ...);
extern int printf(const char *, ...);
extern int vprintf(const char *, va_list);
extern int sprintf(char *, const char *, ...);
extern int ansi_sprintf(char *, const char *, ...);
extern int vsprintf(char *, const char *, va_list);
extern void console_message(const char *);
extern void ttyflush(tt_soc *);
extern void flush(void);
extern void printc(char);
extern void automore_enable(const char *);
extern void automore_header(const char *);
extern void automore_disable(void);
extern boolean automore_quit(void);
extern void automore_conditional(int);
extern void doprintc(char **, tt_soc *, char);
extern void ttyprintc(tt_soc *, char);
extern int _ttyprintf(tt_soc *, char *, ...);
