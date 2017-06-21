/* $Id: exec_show_media.h,v 3.1.2.2 1996/08/12 16:07:51 widmer Exp $
 * $Source: /release/112/cvs/Xsys/srt/exec_show_media.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ M E D I A . H
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_media.h,v $
 * Revision 3.1.2.2  1996/08/12  16:07:51  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/05/30  23:45:48  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/30  20:49:12  hampton
 * Placeholder files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show media access-lists <list-number>
 *
 * OBJ(int,1) = <list-number>
 */
EOLNS	(show_media_access_eol, show_media);
EVAL	(show_media_access_xmac, show_media_access_eol,
	 GETOBJ(int,1) = GETOBJ(int,1) - MACADDR_MIN_EXT_ACL + MINXMACACC);
NUMBER	(show_media_access_list3, show_media_access_xmac, show_media_access_eol,
	 OBJ(int,1), MACADDR_MIN_EXT_ACL, MACADDR_MAX_EXT_ACL,
	 NULL /*"Extended Mac Address access list number"*/);

EVAL	(show_media_access_mac, show_media_access_eol,
	 GETOBJ(int,1) = GETOBJ(int,1) - MACADDR_MIN_STD_ACL + MINMACADDRACC);
NUMBER	(show_media_access_list2, show_media_access_mac, show_media_access_list3,
	 OBJ(int,1), MACADDR_MIN_STD_ACL, MACADDR_MAX_STD_ACL,
	 NULL /*"Mac Address access list number"*/);

EVAL	(show_media_access_type, show_media_access_eol,
	 GETOBJ(int,1) = GETOBJ(int,1) - TYPECODE_MIN_ACL + MINTYPEACC);
NUMBER	(show_media_access_list, show_media_access_type, show_media_access_list2,
	 OBJ(int,1), TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
	 NULL /*"Type Code access list number"*/);
KEYWORD (show_media_access, show_media_access_list, no_alt,
	 "access-lists", NULL /*"Media access lists"*/, PRIV_USER);

/******************************************************************/
KEYWORD (show_media, show_media_access, ALTERNATE,
	 "media", NULL /*"Media information"*/, PRIV_USER|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_media
