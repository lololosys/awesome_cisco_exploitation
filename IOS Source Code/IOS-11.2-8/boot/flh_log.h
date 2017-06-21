/* $Id: flh_log.h,v 3.2 1995/11/17 07:41:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/flh_log.h,v $
 *------------------------------------------------------------------
 * header files for flh_log.c
 *
 * Oct 1994, Eric Fronberg
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Definitions for routines defined in flh_log.c
 *------------------------------------------------------------------
 * $Log: flh_log.h,v $
 * Revision 3.2  1995/11/17  07:41:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *------------------------------------------------------------------
 */

extern uint init_flh_buffer(uchar *);
extern void flh_putc(char);
extern uchar *flh_incr(uchar *);
extern uchar *flh_decr(uchar *);
extern void flhbuf_print(char *, int);
extern void clear_flh_buffer(void);
extern uint flh_log_size(void);
extern int printflh(void);

extern struct _flhlog_begin *start_flh; /* flh flh logging pointer */
extern struct _flhlog_end *end_flh ;

