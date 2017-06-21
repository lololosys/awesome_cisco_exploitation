/* $Id: cfg_ip_as-path_access-list.h,v 3.4.10.1 1996/08/12 16:03:28 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_ip_as-path_access-list.h,v $
 *------------------------------------------------------------------
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_as-path_access-list.h,v $
 * Revision 3.4.10.1  1996/08/12  16:03:28  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/02/26  07:14:21  pst
 * CSCdi49774:  remove as-path command from regular expression library
 *                 (whoops: I missed a file when I was moving stuff into
 *                  the interim tree to commit... dunce cap)
 *
 * Revision 3.3  1995/11/27  21:24:46  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  17:31:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:38  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:08:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************************
 * ip as-path access-list <list> { permit | deny } <regexp>
 * no ip as-path access-list <list> [{ permit | deny } <regexp>]
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) =	TRUE = permit
 *		FALSE = deny
 * OBJ(string,1) = <regexp>
 */

EOLNS	(cip_aspath_eol, ip_aspath_command);
TEXT	(cip_aspath_regexp, cip_aspath_eol, no_alt,
	 OBJ(string,1), "A regular-expression to match the BGP AS paths");
PERMIT_DENY(cip_aspath_permit_deny, cip_aspath_regexp, no_alt,
	    OBJ(int,2), PRIV_CONF);
NOPREFIX(cip_aspath_noprefix, cip_aspath_permit_deny, cip_aspath_eol);
NUMBER	(cip_aspath_al_num, cip_aspath_noprefix, no_alt,
	 OBJ(int,1), MINREGEXPACC+1, MAXREGEXPACC,
	 "Regular expression access list number");
NVGENNS	(cip_aspath_nvgen, cip_aspath_al_num, ip_aspath_command);
KEYWORD (cip_aspath_al, cip_aspath_nvgen, no_alt,
	 "access-list", "Specify an access list number", PRIV_CONF);
KEYWORD (cip_aspath, cip_aspath_al, ALTERNATE,
	 "as-path", "BGP autonomous system path filter", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_aspath
