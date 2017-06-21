/* $Id: tcl_new_commands.h,v 3.2 1995/11/17 18:47:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tcl/tcl_new_commands.h,v $
 *------------------------------------------------------------------
 * tcl_new_commands.c -- Add procedures for new commands.
 * April 1994, Satish Gannu
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcl_new_commands.h,v $
 * Revision 3.2  1995/11/17  18:47:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:34:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:04:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

int	Tcl_LoadScriptsCmd  _ANSI_ARGS_((ClientData clientData,
		    Tcl_Interp *interp, int argc, char **argv));
