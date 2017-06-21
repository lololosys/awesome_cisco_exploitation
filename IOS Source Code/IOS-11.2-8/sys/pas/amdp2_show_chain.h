/* $Id: amdp2_show_chain.h,v 3.2.60.1 1996/08/12 16:06:37 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pas/amdp2_show_chain.h,v $
 *------------------------------------------------------------------
 * amdp2_show_chain.h : AMD P2 show parser chains
 *
 * June 1995, Dave Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: amdp2_show_chain.h,v $
 * Revision 3.2.60.1  1996/08/12  16:06:37  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:53:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/12  20:12:59  mbeesley
 * Add placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ------- SHOW COMMANDS ---------
 */

/******************************************************************
 * print PA0 amdp2 PCI registers
 * 
 */

EOLNS    (amdp2_pregs0_eol, amdp2_pregs0);

KEYWORD (amdp2_pregs0, amdp2_pregs0_eol, no_alt,
	 "pregs0", "Print PA0 AMDP2 PCI registers",
	 PRIV_ROOT);

/******************************************************************
 * print PA1 amdp2 PCI registers
 * 
 */

EOLNS    (amdp2_pregs1_eol, amdp2_pregs1);

KEYWORD (amdp2_pregs1, amdp2_pregs1_eol, amdp2_pregs0,
	 "pregs1", "Print PA1 AMDP2 PCI registers",
	 PRIV_ROOT);


KEYWORD (show_amdp2, amdp2_pregs1, ALTERNATE,
	    "amdp2", "Show AMD P-2 Information",
	    PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE  show_amdp2

/* end of file */


