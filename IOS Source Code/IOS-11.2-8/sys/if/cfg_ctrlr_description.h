/* $Id: cfg_ctrlr_description.h,v 3.2.60.1 1996/08/12 16:02:05 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_ctrlr_description.h,v $
 *------------------------------------------------------------------
 * C F G _ C T R L R _ D E S C R I P T I O N . H
 * 
 * description -- provide a description of the controller
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_ctrlr_description.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:05  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:25:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:47:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:05  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:02:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * description <name-string>
 * no description [<name-string>]
 *
 * OBJ(string,1) = <name-string>
 */
EOLNS	(cc_descr_eol, controller_description_command);
TEXT	(cc_descr_text, cc_descr_eol, no_alt,
	 OBJ(string,1), "Up to 80 characters describing this interface");
NOPREFIX (cc_descr_noprefix, cc_descr_text, cc_descr_eol);
NVGENNS	(cc_descr_nvgen, cc_descr_noprefix, controller_description_command);
KEYWORD	(cc_descr, cc_descr_nvgen, ALTERNATE,
	 "description", "Controller specific description", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cc_descr
