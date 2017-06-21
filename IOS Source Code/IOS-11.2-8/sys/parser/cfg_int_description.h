/* $Id: cfg_int_description.h,v 3.2.62.1 1996/08/12 16:05:35 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_int_description.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ D E S C R I P T I O N . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_description.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:35  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:43:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:04:32  hampton
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
EOLNS	(ci_descr_eol, description_command);
TEXT	(ci_descr_text, ci_descr_eol, no_alt,
	 OBJ(string,1), "Up to 80 characters describing this interface");
NOPREFIX (ci_descr_noprefix, ci_descr_text, ci_descr_eol);
NVGENNS	(ci_descr_nvgen, ci_descr_noprefix, description_command);
KEYWORD	(ci_descr, ci_descr_nvgen, ALTERNATE,
	 "description", "Interface specific description", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_descr
