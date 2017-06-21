/* $Id: cfg_int_shutdown.h,v 3.2.62.1 1996/08/12 16:05:46 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_int_shutdown.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ S H U T D O W N . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_shutdown.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:46  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:43:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:07:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *	shutdown
 */

EOLNS	(ci_shutdown_func, shutdown_command);
KEYWORD	(shutdown, ci_shutdown_func, ALTERNATE, "shutdown", 
	 "Shutdown the selected interface", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	shutdown
