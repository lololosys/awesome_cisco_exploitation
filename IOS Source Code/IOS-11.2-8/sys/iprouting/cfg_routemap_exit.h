/* $Id: cfg_routemap_exit.h,v 3.2.60.1 1996/08/12 16:03:34 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_routemap_exit.h,v $
 *------------------------------------------------------------------
 * cfg_routemap_exit.h
 * 
 * exit - exit from route-map configuration mode
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_routemap_exit.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:34  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:31:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:44  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(routemap_exit_eol, exit_config_submode);
KEYWORD	(routemap_exit_kwd, routemap_exit_eol, NONE,
	"exit", "Exit from route-map configuration mode", PRIV_MIN);
TESTVAR	(routemap_exit, NONE, routemap_exit_kwd, NONE,
	 NONE, ALTERNATE, sense, FALSE);

#undef	ALTERNATE
#define	ALTERNATE	routemap_exit
