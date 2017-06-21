/* $Id: cfg_int_dceterm.h,v 3.3.62.1 1996/08/12 16:09:24 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_dceterm.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ D C E T E R M . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_dceterm.h,v $
 * Revision 3.3.62.1  1996/08/12  16:09:24  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/11/17  18:00:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:47:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:49  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:04:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] dce-terminal-timing-enable
 *
 */
EOLNS	(ci_dceterm_eol, serial_internal_transmit_clock_command);
KEYWORD	(ci_dceterm, ci_dceterm_eol, NONE,
	 "dce-terminal-timing-enable", "Enable DCE terminal timing",
	 PRIV_CONF);

ASSERT (ci_dceterm_test, ci_dceterm, ALTERNATE, 
	(csb->interface->hwptr->serial_flags & SERIAL_FLAGS_CAN_INT_TX));

#undef	ALTERNATE
#define	ALTERNATE	ci_dceterm_test
