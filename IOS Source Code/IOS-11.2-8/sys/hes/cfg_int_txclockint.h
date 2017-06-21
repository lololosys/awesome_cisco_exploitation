/* $Id: cfg_int_txclockint.h,v 3.1.40.1 1996/08/12 16:01:26 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/cfg_int_txclockint.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ T X C L O C K I N T. H
 *
 * February 1993, William H. Palmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Internal transmit clocking configuration command for C7000 serial cards
 *------------------------------------------------------------------
 * $Log: cfg_int_txclockint.h,v $
 * Revision 3.1.40.1  1996/08/12  16:01:26  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:58:00  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/11/17  18:43:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:20:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:50:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:07:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * transmit-clock-internal
 * no transmit-clock-internal
 */

EOLNS	(ci_txclockint_eol, serial_internal_transmit_clock_command);

KEYWORD (ci_txclockint, ci_txclockint_eol, NONE, "transmit-clock-internal",
	 "Use internal transmit clock", PRIV_CONF);

ASSERT (ci_txclockint_test, ci_txclockint, ALTERNATE, 
	(csb->interface->hwptr->serial_flags & SERIAL_FLAGS_CAN_INT_TX));

#undef ALTERNATE
#define ALTERNATE ci_txclockint_test
