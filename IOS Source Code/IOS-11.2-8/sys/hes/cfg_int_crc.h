/* $Id: cfg_int_crc.h,v 3.1.36.3 1996/08/12 16:01:24 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/cfg_int_crc.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ C R C . H
 *
 * September 1993, William H. Palmer, Gary W. Kramling
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * CRC 16/32 configuration command for Cbus serial cards
 *------------------------------------------------------------------
 * $Log: cfg_int_crc.h,v $
 * Revision 3.1.36.3  1996/08/12  16:01:24  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.36.2  1996/04/30  16:27:09  gstovall
 * CSCdi55376:  Missing crc configuration command on some platforms.
 * Branch: California_branch
 * Use the new bit in serial_flags.
 *
 * Revision 3.1.36.1  1996/03/21  22:39:47  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.4.1  1996/02/08  07:20:27  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.1  1996/02/01  04:57:56  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:43:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:04:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * crc 16 
 * crc 32
 * no crc 32
 */

EOLNS	(ci_crc_eol, serial_crc_command);

KEYWORD_ID(ci_crc_num16_kwd, ci_crc_eol, no_alt,
	   OBJ(int,1), 16,
	   "16", "crc word-size", PRIV_CONF);

KEYWORD_ID(ci_crc_num32_kwd, ci_crc_eol, ci_crc_num16_kwd, 
	OBJ(int,1), 32,
	"32", "crc word-size", PRIV_CONF);

NOPREFIX(ci_crc_noprefix, ci_crc_num32_kwd, ci_crc_eol);

KEYWORD (ci_crc_kwd, ci_crc_noprefix, no_alt, "crc",
	 "Specify CRC word-size", PRIV_CONF);

ASSERT (ci_crc, ci_crc_kwd, ALTERNATE,
	(csb->interface->hwptr->serial_flags & SERIAL_FLAGS_CAN_CRC_32)); 

#undef ALTERNATE
#define ALTERNATE ci_crc
