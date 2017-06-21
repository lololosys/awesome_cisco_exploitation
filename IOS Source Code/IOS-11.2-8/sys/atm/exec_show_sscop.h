/* $Id: exec_show_sscop.h,v 3.3.20.2 1996/08/12 16:00:32 widmer Exp $
 * $Source: /release/112/cvs/Xsys/atm/exec_show_sscop.h,v $
 *------------------------------------------------------------------
 * parser/exec_show_sscop.h: SHOW command parsing
 *
 * November, 1993,  Percy P. Khabardar
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_sscop.h,v $
 * Revision 3.3.20.2  1996/08/12  16:00:32  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.1  1996/06/05  18:56:59  widmer
 * CSCdi59571:  PRIV_USER_HIDDEN used without a privilege level
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/29  07:08:17  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:46:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:35:17  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.1  1995/06/07  22:26:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show sscop 
 */

EOLNS	(show_sscop_eol, sscop_show_command);
KEYWORD (show_sscop, show_sscop_eol, ALTERNATE,
	 "sscop", "SSCOP information", PRIV_USER | PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_sscop
