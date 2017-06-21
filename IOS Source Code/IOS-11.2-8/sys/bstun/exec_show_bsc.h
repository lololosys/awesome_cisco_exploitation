/* $Id: exec_show_bsc.h,v 3.4.20.2 1996/09/10 08:48:49 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/exec_show_bsc.h,v $
 *------------------------------------------------------------------
 * Header for show bsc command.
 *
 * June 1995, Andrew R. Rothwell.
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_bsc.h,v $
 * Revision 3.4.20.2  1996/09/10  08:48:49  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/08/12  16:00:33  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/01/29  07:08:19  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.3  1995/11/23  04:44:20  jbalestr
 * CSCdi43435:  sh ? has wrong case for BSC
 *
 * Revision 3.2  1995/11/17  08:49:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:35:36  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.2  1995/06/21  18:17:51  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/16  06:23:12  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*******************************************************************
 * show bsc [ group <bstun-group-number> | addr <address> ]
 */
EOLNS	  (show_bsc_eol, bsc_show_command);
HEXADECIMAL(show_bsc_addr_val, show_bsc_eol, no_alt, OBJ(int,2));
KEYWORD   (show_bsc_addr, show_bsc_addr_val, show_bsc_eol,
           "address", "control unit poll address", PRIV_USER);
NUMBER    (show_bsc_group_val, show_bsc_addr, show_bsc_eol,
           OBJ(int,1), 1, 255, "bstun group number between 1 and 255");
KEYWORD   (show_bsc_group, show_bsc_group_val, show_bsc_addr,
           "group", "BSTUN group number", PRIV_USER);


/*******************************************************************
 * show bsc
 */
KEYWORD   (show_bsc, show_bsc_group, ALTERNATE,
	   "bsc", "BSC interface information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_bsc
