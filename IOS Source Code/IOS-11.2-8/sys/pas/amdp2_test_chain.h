/* $Id: amdp2_test_chain.h,v 3.2.62.1 1996/08/12 16:06:38 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pas/amdp2_test_chain.h,v $
 *------------------------------------------------------------------
 * amdp2_test_chain.h : AMD P2 test parser chains
 *
 * June 1995, Dave Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: amdp2_test_chain.h,v $
 * Revision 3.2.62.1  1996/08/12  16:06:38  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:53:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/12  20:13:00  mbeesley
 * Add placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ------- TEST COMMANDS ---------
 */

/******************************************************************
 * Issue software reset to amdp2
 * 
 */

EOLNS    (amdp2_swreset_eol, amdp2_swreset);

KEYWORD (amdp2_swreset, amdp2_swreset_eol, no_alt,
	 "swreset", "Issue SW reset to AMDP2",
	 PRIV_ROOT);

/******************************************************************
 * Force P-2 to immediately access the current RX & TX descriptors
 * 
 */

EOLNS    (amdp2_rings_poll_eol, amdp2_rings_poll);

KEYWORD (amdp2_rings_poll, amdp2_rings_poll_eol, amdp2_swreset,
	 "rings_poll", "Force P-2 RX/TX rings poll",
	 PRIV_ROOT);


/******************************************************************
 * Force P-2 to immediately access the current RX & TX descriptors
 * 
 */

EOLNS    (amdp2_sblast_eol, amdp2_sblast);

KEYWORD (amdp2_sblast, amdp2_sblast_eol, amdp2_rings_poll,
	 "sblast", "Segpak Tx on all ports that are up",
	 PRIV_ROOT);


/******************************************************************
 * Execute both TX & RX ISRs with interrupts disabled
 * 
 */

EOLNS    (amdp2_intsrv_eol, amdp2_intsrv);

KEYWORD (amdp2_intsrv, amdp2_intsrv_eol, amdp2_sblast,
	 "intsrv", "Call P-2 TX, RX interrupt handlers",
	 PRIV_ROOT);


/******************************************************************
 * test amdp2
 * 
 */

KEYWORD (test_amdp2, amdp2_intsrv, ALTERNATE,
	    "amdp2", "AMD P-2 test system",
	    PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE test_amdp2

/* end of file */

 
