/* $Id: exec_show_bstun.h,v 3.4.20.2 1996/09/10 08:48:51 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/exec_show_bstun.h,v $
 *------------------------------------------------------------------
 * exec_show_bstun.h - BSTUN Show commands
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_bstun.h,v $
 * Revision 3.4.20.2  1996/09/10  08:48:51  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/08/12  16:00:34  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/02/01  23:32:57  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1996/01/29  07:08:20  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:49:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:35:37  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.1  1995/06/07  22:22:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*******************************************************************
 * show bstun [ group <bstun-group-number> | addr <address> ]
 */
EOLNS	  (show_bstun_eol, show_bstun);
HEXADECIMAL(show_bstun_addr_val, show_bstun_eol, no_alt, OBJ(int,2));
KEYWORD   (show_bstun_addr, show_bstun_addr_val, show_bstun_eol,
           "address", "control unit poll address", PRIV_USER);
NUMBER    (show_bstun_group_val, show_bstun_addr, show_bstun_eol,
           OBJ(int,1), 1, 255, "bstun group number between 1 and 255");
KEYWORD   (show_bstun_group, show_bstun_group_val, show_bstun_addr,
           "group", "BSTUN group number", PRIV_USER);


/*******************************************************************
 * show bstun
 */
KEYWORD   (show_bstun, show_bstun_group, ALTERNATE,
	   "bstun", "BSTUN interface information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_bstun
