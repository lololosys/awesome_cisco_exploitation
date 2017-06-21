/* $Id: cfg_int_dte.h,v 3.3.62.1 1996/08/12 16:09:26 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_dte.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ D T E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_dte.h,v $
 * Revision 3.3.62.1  1996/08/12  16:09:26  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/11/17  18:01:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:48:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:51  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:04:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] dte-invert-txc
 *
 */
EOLNS	(ci_dte_eol, serial_invert_transmit_clock_command);
KEYWORD	(ci_dte, ci_dte_eol, NONE,
	 "dte-invert-txc", NULL /*"Invert transmit clock"*/,
	 PRIV_CONF | PRIV_HIDDEN | PRIV_NONVGEN);

ASSERT (ci_dte_test, ci_dte, ALTERNATE, 
	((csb->interface->hwptr->status & IDB_SERIAL) &&
	 (csb->interface->hwptr->serial_flags & SERIAL_FLAGS_CAN_CLK_INV)));

#undef	ALTERNATE
#define	ALTERNATE	ci_dte_test







