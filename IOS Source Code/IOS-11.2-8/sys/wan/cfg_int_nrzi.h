/* $Id: cfg_int_nrzi.h,v 3.3.62.1 1996/08/12 16:09:30 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_nrzi.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ N R Z I . H
 *
 * February 1993, William H. Palmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * NRZI configuration command for C7000 serial cards
 *------------------------------------------------------------------
 * $Log: cfg_int_nrzi.h,v $
 * Revision 3.3.62.1  1996/08/12  16:09:30  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/11/17  18:01:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:48:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:06:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * nrzi-encoding
 * no nrzi-encoding
 */

EOLNS	(ci_nrzi_eol, serial_nrzi_command);

KEYWORD (ci_nrzi, ci_nrzi_eol, NONE, "nrzi-encoding",
	 "Enable use of NRZI encoding", PRIV_CONF);

ASSERT (ci_nrzi_test, ci_nrzi, ALTERNATE,
	((csb->interface->hwptr->status & IDB_SERIAL) &&
	 (csb->interface->hwptr->serial_flags & SERIAL_FLAGS_CAN_NRZI)));

#undef ALTERNATE
#define ALTERNATE ci_nrzi_test
