/* $Id: cfg_nov_router_redist.h,v 3.2.62.1 1996/08/12 16:09:58 widmer Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_nov_router_redist.h,v $
 *------------------------------------------------------------------
 * C F G _ N O V _ R O U T E R _ R E D I S T . H
 * 
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_nov_router_redist.h,v $
 * Revision 3.2.62.1  1996/08/12  16:09:58  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  19:21:12  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:38:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/21  18:18:31  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  23:27:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * Format:
 * [no] redistribute [rip | igrp <as> | nlsp [<tag>]]
 *
 * router subcommand 
 */

EOLNS    (nov_redist_eol, novell_redist_command); 
GENERAL_ROUTER(nov_redist_rtr, nov_redist_eol, no_alt, router_name,
               (PROC_IGRP2 | PROC_RIP | PROC_CONNECTED | PROC_STATIC |
		PROC_FSTATIC | PROC_NLSP | RMATCH_NOVELL | RMATCH_ARGS));
NVGENNS  (nov_redist_nvgen, nov_redist_rtr, novell_redist_command);
KEYWORD	(nov_redist, nov_redist_nvgen, NONE,
	 "redistribute", "Enable routing protocol redistribution ", PRIV_CONF);
ASSERT	(nov_redist_test, nov_redist, ALTERNATE,
	 (((novpdbtype *)csb->protocol)->proctype != NET_NLSP));

#undef  ALTERNATE
#define ALTERNATE 	nov_redist_test
