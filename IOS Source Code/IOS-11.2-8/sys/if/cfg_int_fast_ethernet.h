/* $Id: cfg_int_fast_ethernet.h,v 3.2.60.1 1996/08/12 16:02:11 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_fast_ethernet.h,v $
 *------------------------------------------------------------------
 * cfg_int_fast_ethernet.h
 *
 * November 1994, Gary Kramling
 * Adapted from cfg_int_channel.h by Stig Thormodsrud
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_fast_ethernet.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:11  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:26:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:09  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/21  18:16:43  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:04:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] full-duplex
 *
 */
EOLNS	(full_duplex_eol, full_duplex_command);

KEYWORD	(full_duplex, full_duplex_eol, NONE,
	 "full-duplex",  "Select full-duplex operational mode", PRIV_CONF);

ASSERT (config_duplex, full_duplex, ALTERNATE,
	(csb->interface->hwptr->type == IDBTYPE_FEIP));

#undef	ALTERNATE
#define	ALTERNATE	config_duplex
