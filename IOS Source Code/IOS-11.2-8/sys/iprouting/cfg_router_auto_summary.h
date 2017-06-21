/* $Id: cfg_router_auto_summary.h,v 3.2.60.1 1996/08/12 16:03:40 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_auto_summary.h,v $
 *------------------------------------------------------------------
 * cfg_router_auto_summary.h
 *
 * October 1993
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Supports the router subcommand "auto-summary". Configures auto aggregation
 * of routing information to the network number level.
 *------------------------------------------------------------------
 * $Log: cfg_router_auto_summary.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:40  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:32:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/01  12:15:48  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * [no] auto-summary
 *
 */
EOLNS (router_auto_summary_eols, auto_summary_command);
KEYWORD	(router_auto_summary_kw, router_auto_summary_eols, NONE,
	"auto-summary", "Enable automatic network number summarization", 
	 PRIV_CONF);

ASSERT	(router_auto_summary, router_auto_summary_kw, ALTERNATE,
	 !strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_IGRP2) ||
	 !strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_RIP) ||
	 !strcmp(((pdbtype *) csb->protocol)->name, RTRNAME_BGP));

#undef	ALTERNATE
#define	ALTERNATE	router_auto_summary

