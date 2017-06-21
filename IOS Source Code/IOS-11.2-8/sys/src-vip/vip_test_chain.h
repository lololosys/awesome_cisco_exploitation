/* $Id: vip_test_chain.h,v 3.1.60.2 1996/08/12 16:07:42 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_test_chain.h,v $
 *------------------------------------------------------------------
 * vip_test_chain.h : VIP  test parser chains
 *
 * April 1995, David Getchell
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: vip_test_chain.h,v $
 * Revision 3.1.60.2  1996/08/12  16:07:42  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.60.1  1996/03/21  23:52:38  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.1  1996/02/10  00:22:42  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ------- TEST COMMANDS ---------
 */
/******************************************************************
 * test vip bringup
 * 
 */

EOLNS    (vip_test_bringup_eol, vip_test_bringup);

KEYWORD (vip_test_bringup, vip_test_bringup_eol, no_alt,
	 "bringup", "Test vip bringup",
	 PRIV_ROOT);

/******************************************************************
 * test vip
 * 
 */

KEYWORD_MM (test_vip, vip_test_bringup, no_alt,
	    "vip", "VIP test system",
	    PRIV_ROOT, 3);

#undef  ALTERNATE
#define ALTERNATE test_vip

/* end of file */
