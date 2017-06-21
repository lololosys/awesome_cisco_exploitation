/* $Id: cfg_x25.h,v 3.3 1996/02/01 23:35:04 widmer Exp $
 * $Source: /release/111/cvs/Xsys/x25/cfg_x25.h,v $
 *------------------------------------------------------------------
 * C F G _ X 2 5 . H
 *
 * X.25 Level 3 support
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_x25.h,v $
 * Revision 3.3  1996/02/01  23:35:04  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:05:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:02  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:14:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(cfg_x25_extend_here, no_alt);
/************************************************************************
 *    [no] x25 xot-confirm-svc-reset
 *
 *    Send a SVC reset confirm packet to a remote VC over a TCP connection
 *
 */

EOLS    (x25_xot_confirm_svc_reset_eol, x25_global_commands,
	 X25_XOT_CONFIRM_SVC_RESET);

KEYWORD (x25_xot_confirm_svc_reset,
	 x25_xot_confirm_svc_reset_eol,
	 cfg_x25_extend_here, "xot-confirm-svc-reset",
	 "Send a reset confirm to a TCP connection", PRIV_CONF|PRIV_HIDDEN);


/************************************************************************
 *	x25 rpoa NAME N [N ...]
 *
 * OBJ(string,1) = NAME
 * OBJ(int,1)  = number of Ns (10 max)
 * uiarray[0-(N-1)] = Ns
 *
 * XXX How many numbers may there be?  Maybe we need a different
 * XXX macro that malloc's an array, growing it as needed.
 */

pdecl(x25_rpoa_2);

EOLS	(x25_rpoa_4, x25_global_commands, X25_RPOA);

EVAL	(x25_rpoa_3a, x25_rpoa_2, csb->uiarray[GETOBJ(int,1) ++] = GETOBJ(int,2));
NUMBER	(x25_rpoa_3, x25_rpoa_3a, NONE, OBJ(int,2), 0, 9999, "RPOA");

NEQ	(x25_rpoa_2a, x25_rpoa_4, no_alt, OBJ(int,1), 0);
NEQ	(x25_rpoa_2, x25_rpoa_3, x25_rpoa_2a, OBJ(int,1), UIARRAY_SIZE);

NOPREFIX(x25_rpoa_1a, x25_rpoa_2, x25_rpoa_4);
STRING	(x25_rpoa_1, x25_rpoa_1a, no_alt,
	 OBJ(string,1), "Name to reference RPOA list by");

NVGENS	(x25_rpoa_0, x25_rpoa_1, x25_global_commands, X25_RPOA);
KEYWORD	(x25_rpoa, x25_rpoa_0, x25_xot_confirm_svc_reset,
	 "rpoa", "Specify transit RPOA list", PRIV_CONF);

/***************************************************************************
 * x25 routing [TCP-USE-IF-DEFS]
 *    OBJ(int,1) = TCP-USE-IF-DEFS
 */

EOLS	(x25_routing_eol, x25_global_commands, X25_ROUTING);

KEYWORD_ID(x25_routing_opt, x25_routing_eol, x25_routing_eol,
	   OBJ(int,1), TRUE,
	   "TCP-USE-IF-DEFS",
	   "Use interface window/packet defaults for incoming remote CALLs "
	   "with unknown values", PRIV_CONF);

NVGENS	(x25_routing_0, x25_routing_opt, x25_global_commands, X25_ROUTING);
KEYWORD	(x25_routing, x25_routing_0, x25_rpoa,
	 "routing", "Enable X.25 switching", PRIV_CONF);

KEYWORD	(config_x25, x25_routing, NONE,
	 "x25", "X.25 Level 3", PRIV_CONF);

TEST_BOOLEAN(config_x25_test, config_x25, NONE, ALTERNATE, x25_enable);

#undef	ALTERNATE
#define	ALTERNATE	config_x25_test
