/* $Id: login_public.h,v 3.2.60.1 1996/04/15 14:57:45 widmer Exp $
 * $Source: /release/112/cvs/Xsys/h/login_public.h,v $
 *------------------------------------------------------------------
 * Publicly useful externs for login.c
 *
 * November 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: login_public.h,v $
 * Revision 3.2.60.1  1996/04/15  14:57:45  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.2  1995/11/17  09:12:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:35:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef LOGIN_PUBLIC_H
#define LOGIN_PUBLIC_H
extern uint encrypt(char *, char *, char *, ulong);
extern uint decrypt(char *, char *, ulong);
#endif

