/* $Id: dec21140_show_chain.h,v 3.2.62.1 1996/08/12 16:06:39 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pas/dec21140_show_chain.h,v $
 *------------------------------------------------------------------
 * dec21140_show_chain.h : DEC 21140  show parser chains
 *
 * April 1995, Steve Jacobson
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dec21140_show_chain.h,v $
 * Revision 3.2.62.1  1996/08/12  16:06:39  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:53:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ------- SHOW COMMANDS ---------
 */

/******************************************************************
 * Test dec21140 MII stuff on port 0
 *
 */
 
EOLNS    (dec21140_miiregs0_eol, dec21140_miiregs0);
 
KEYWORD (dec21140_miiregs0, dec21140_miiregs0_eol, no_alt,
         "miiregs0", "MII test on port 0",
         PRIV_ROOT);
 
/******************************************************************
 * Test dec21140 MII stuff on port 1
 *
 */
 
EOLNS    (dec21140_miiregs1_eol, dec21140_miiregs1);
 
KEYWORD (dec21140_miiregs1, dec21140_miiregs1_eol, dec21140_miiregs0,
         "miiregs1", "MII test on port 1",
         PRIV_ROOT);
 
/******************************************************************
 * print PA0 dec21140 PCI registers
 * 
 */

EOLNS    (dec21140_pregs0_eol, dec21140_pregs0);

KEYWORD (dec21140_pregs0, dec21140_pregs0_eol, dec21140_miiregs1,
	 "pregs0", "Print PA0 DEC21140 PCI registers",
	 PRIV_ROOT);

/******************************************************************
 * print PA1 dec21140 PCI registers
 * 
 */

EOLNS    (dec21140_pregs1_eol, dec21140_pregs1);

KEYWORD (dec21140_pregs1, dec21140_pregs1_eol, dec21140_pregs0,
	 "pregs1", "Print PA1 DEC21140 PCI registers",
	 PRIV_ROOT);

/******************************************************************
 * print PA0 dec21140 csr registers
 * 
 */

EOLNS    (dec21140_cregs0_eol, dec21140_cregs0);

KEYWORD (dec21140_cregs0, dec21140_cregs0_eol, dec21140_pregs1,
	 "cregs0", "Print PA0 DEC21140 csr registers",
	 PRIV_ROOT);

/******************************************************************
 * print PA1 dec21140 csr registers
 * 
 */

EOLNS    (dec21140_cregs1_eol, dec21140_cregs1);

KEYWORD (dec21140_cregs1, dec21140_cregs1_eol, dec21140_cregs0,
	 "cregs1", "Print PA1 DEC21140 csr registers",
	 PRIV_ROOT);


KEYWORD (show_dec21140, dec21140_cregs1, ALTERNATE,
	    "dec21140", "Show DEC 21140 Information",
	    PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE  show_dec21140

/* end of file */


