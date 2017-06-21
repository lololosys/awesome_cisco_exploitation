/* $Id: cfg_int_crc4.h,v 3.3.62.2 1996/08/12 16:09:23 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_crc4.h,v $
 *------------------------------------------------------------------
 * cfg_int_crc4.h
 *
 * 08-June-1994, Rob Haragan
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cfg_int_crc4.h,v $
 * Revision 3.3.62.2  1996/08/12  16:09:23  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.62.1  1996/03/18  22:46:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  02:11:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  21:49:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:00:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:47:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:48  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:04:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/***************************************************************
 * crc4
 * no crc4
 */

EOLNS	(ci_crc4_eol, serial_crc4_command);

KEYWORD_MM (ci_crc4, ci_crc4_eol, NONE, 
	 "crc4","Enable use of G.703 CRC4 checking", PRIV_CONF,4);

ASSERT (ci_crc4_test, ci_crc4, ALTERNATE, is_g703(csb->interface->hwptr)); 

#undef ALTERNATE
#define ALTERNATE ci_crc4_test






