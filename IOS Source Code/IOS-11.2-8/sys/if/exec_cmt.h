/* $Id: exec_cmt.h,v 3.3.20.1 1996/08/12 16:02:31 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_cmt.h,v $
 *------------------------------------------------------------------
 * E X E C _ C M T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_cmt.h,v $
 * Revision 3.3.20.1  1996/08/12  16:02:31  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:33:34  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:27:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:19  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:16:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *	cmt connect [<interface> [{phy-a | phy-b}]]
 *	cmt disconnect [<interface> [{phy-a | phy-b}]]
 *
 * csb->func = CMT_CONNECT, CMT_DISCONNECT
 * OBJ(idb,1) = <interface>
 * csb->interface = an idb if an interface was specified, NULL otherwise
 * OBJ(int,1) = TRUE if <interface> was given
 * OBJ(int,2) = FDDI_PHY_BOTH, FDDI_PHY_A, FDDI_PHY_B
 */
EOLNS	(cmt_eol, cmt_command);

SET	(cmt_phy_both, cmt_eol, OBJ(int,2), FDDI_PHY_BOTH);

KEYWORD_ID(cmt_phyb, cmt_eol, cmt_phy_both,
	   OBJ(int,2), FDDI_PHY_B,
	   "phy-b", "select the PHY B physical sublayer", PRIV_USER);
KEYWORD_ID(cmt_phya, cmt_eol, cmt_phyb,
	   OBJ(int,2), FDDI_PHY_A,
	   "phy-a", "select the PHY A physical sublayer", PRIV_USER);

SET	(cmt_if_set_true, cmt_phya, OBJ(int,1), TRUE);
INTERFACE (cmt_interface, cmt_if_set_true, cmt_phy_both,
	   OBJ(idb,1), IFTYPE_FDDI);

KEYWORD_ID(cmt_disconnect, cmt_interface, no_alt,
	   func, CMT_DISCONNECT,
	   "disconnect", "Stop FDDI Connection Management", PRIV_ROOT);

KEYWORD_ID(cmt_connect, cmt_interface, cmt_disconnect,
	   func, CMT_CONNECT,
	   "connect", "Start FDDI Connection Management", PRIV_ROOT);

KEYWORD_ID(exec_cmt, cmt_connect, NONE,
	   func, -1,
	   "cmt", "Start or stop FDDI Connection Management functions",
	   PRIV_ROOT);

TEST_INT(exec_cmt_test, exec_cmt, NONE, ALTERNATE, nfddinets);

#undef	ALTERNATE
#define	ALTERNATE	exec_cmt_test
