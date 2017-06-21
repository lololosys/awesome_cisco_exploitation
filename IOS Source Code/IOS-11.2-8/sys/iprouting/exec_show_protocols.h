/* $Id: exec_show_protocols.h,v 3.3.20.1 1996/08/12 16:04:12 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_show_protocols.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ P R O T O C O L S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_protocols.h,v $
 * Revision 3.3.20.1  1996/08/12  16:04:12  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:56  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  17:33:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:21  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:25:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show protocols [<interface>]
 *
 * OBJ(idb,1) = <interface>
 */
EOLNS	(show_proto_eol, show_protocol);
INTERFACE (show_proto_if, show_proto_eol, show_proto_eol,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_protocols, show_proto_if, ALTERNATE,
	 "protocols", "Active network routing protocols",
	 PRIV_USER|PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_protocols
