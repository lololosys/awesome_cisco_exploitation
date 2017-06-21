/* $Id: cfg_controller.h,v 3.2.60.1 1996/08/12 16:02:04 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_controller.h,v $
 *------------------------------------------------------------------
 * cfg_controller.h
 * 
 * 23-July-1993, Ronnie B. Kon
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_controller.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:04  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:25:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:47:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:04  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:02:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/******************************************************************
 * controller <controller name>  <unit>
 *	<unit> ::= <number>  		on a non-7000
 *		   <number>/<number>	on a 7000
 */

EOLNS	(cfg_ctrl_eols, controller_command);

CONTROLLER_COMMAND(config_controller_ctrl, cfg_ctrl_eols, no_alt);

KEYWORD_ID(configure_controller, config_controller_ctrl, ALTERNATE,
	   leave_subcommand, TRUE,
	   "controller", "Configure a specific controller", PRIV_CONF);

#undef ALTERNATE
#define	ALTERNATE configure_controller
