/* $Id: sdlc_public.h,v 3.2 1995/11/17 09:23:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/sdlc_public.h,v $
 *------------------------------------------------------------------
 * Public prototypes for sdlc.c
 *
 * January 1993, Dave Katz
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: sdlc_public.h,v $
 * Revision 3.2  1995/11/17  09:23:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:48:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void sdlc_input(hwidbtype *, paktype *);
extern void sdlc_wrap_input(hwidbtype *, paktype *);
extern void sdlc_holdq(hwidbtype *, sdlc_data_block *, paktype *);
extern void sdlc_setup2(hwidbtype *);
extern void sdlc_reset(hwidbtype *, char *, sdlc_data_block *);
