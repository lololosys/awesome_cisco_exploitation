/* $Id: cfg_int_invtxc.h,v 3.3.62.2 1996/08/12 16:09:29 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_invtxc.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ I N V T X C . H
 *
 * May 1994, Mohsen Mortazavi
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cfg_int_invtxc.h,v $
 * Revision 3.3.62.2  1996/08/12  16:09:29  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.62.1  1996/03/18  22:47:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  02:11:32  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  21:49:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:01:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:11  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:48:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:05:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] invert-txc
 *
 */
EOLNS	(ci_invtxc_eol, serial_invert_transmit_clock_command);
KEYWORD	(ci_invtxc, ci_invtxc_eol, NONE,
	 "invert-txc", "Invert transmit clock", PRIV_CONF);

ASSERT (ci_invtxc_test, ci_invtxc, ALTERNATE,
	((csb->interface->hwptr->status & IDB_SERIAL) &&
	!is_g703(csb->interface->hwptr) &&
	 (csb->interface->hwptr->serial_flags & SERIAL_FLAGS_CAN_CLK_INV)));

#undef	ALTERNATE
#define	ALTERNATE	ci_invtxc_test
