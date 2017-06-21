/* $Id: cfg_router_exit.h,v 3.2.62.1 1996/08/12 16:05:53 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_router_exit.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ E X I T . H
 * 
 * exit - exit from router configuration mode
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_exit.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:53  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:45:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:12:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(router_exit_eol, exit_config_submode);
KEYWORD	(router_exit_kwd, router_exit_eol, NONE,
	"exit", "Exit from routing protocol configuration mode", PRIV_MIN);
TESTVAR	(router_exit, NONE, router_exit_kwd, NONE,
	 NONE, ALTERNATE, sense, FALSE);

#undef	ALTERNATE
#define	ALTERNATE	router_exit
