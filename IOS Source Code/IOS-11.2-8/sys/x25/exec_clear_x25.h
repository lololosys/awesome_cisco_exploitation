/* $Id: exec_clear_x25.h,v 3.2 1995/11/17 18:06:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/exec_clear_x25.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ X 2 5 . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_x25.h,v $
 * Revision 3.2  1995/11/17  18:06:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:06  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:16:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear x25-vc <interface> [<lcn>]
 *
 * OBJ(int,1) = <lcn>	(-1 means no <lcn> was specified)
 * OBJ(idb,1) = <interface>
 */
EOLS	(clear_x25vc_eol, x25_clear_vc, CLEAR_VC);
SET	(clear_x25vc_nolcn, clear_x25vc_eol, OBJ(int,1), -1);
NUMBER (clear_x25vc_lcn, clear_x25vc_eol, clear_x25vc_nolcn,
	OBJ(int,1), 1, 4095, "Logical Circuit Number (LCN)");
INTERFACE (clear_x25vc_if, clear_x25vc_lcn, no_alt, OBJ(idb,1), IFTYPE_SERIAL);
KEYWORD (clear_x25vc_kwd, clear_x25vc_if, NONE,
	"x25-vc", "Clear X.25 virtual circuits on an interface", PRIV_OPR);

TEST_BOOLEAN(clear_x25vc, clear_x25vc_kwd, NONE, ALTERNATE, x25_enable);

#undef	ALTERNATE
#define	ALTERNATE	clear_x25vc
