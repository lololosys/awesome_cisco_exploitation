/* $Id: cfg_int_ts16.h,v 3.3.62.2 1996/08/12 16:09:34 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_ts16.h,v $
 *------------------------------------------------------------------
 * cfg_int_ts16.h
 *
 * 08-June-1994, Rob Haragan
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cfg_int_ts16.h,v $
 * Revision 3.3.62.2  1996/08/12  16:09:34  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.62.1  1996/03/18  22:47:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  02:11:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  21:49:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:01:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:48:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:59  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:07:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/***************************************************************
 * timeslot16
 * no timeslot16
 */

EOLNS	(ci_ts16_eol, serial_timeslot16_command);

KEYWORD (ci_ts16, ci_ts16_eol, NONE, 
	 "ts16","Enable use of timeslot 16 for G.703", PRIV_CONF);

ASSERT (ci_ts16_test, ci_ts16, ALTERNATE, 
	is_g703(csb->interface->hwptr)); 

#undef ALTERNATE
#define ALTERNATE ci_ts16_test
