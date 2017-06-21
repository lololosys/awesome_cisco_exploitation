/* $Id: cfg_oir_c7100.h,v 3.1.2.2 1996/08/12 16:07:21 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/cfg_oir_c7100.h,v $
 *------------------------------------------------------------------
 * cfg_oir_c7100.h : Parser commands for oir support.
 *
 * April 1996, Dave Carroll
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_oir_c7100.h,v $
 * Revision 3.1.2.2  1996/08/12  16:07:21  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/05/01  13:28:27  dcarroll
 * CSCdi54895:  oir fix for ethernet port adapter
 * Branch: California_branch
 * OIR support for pas ethernets.
 *
 * Revision 3.1  1996/04/17  19:25:50  dcarroll
 * Placeholder file for new developement.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * [no] oir-testport
 */
EOLNS (c7100_oir_testport_eols, c7100_oir_testport_enable);

KEYWORD (c7100_oir_testport_kwd, c7100_oir_testport_eols, ALTERNATE,
	 "testport", "c7200 OIR Testport command", PRIV_MAX);

KEYWORD (c7100_oir_kwd, c7100_oir_testport_kwd, ALTERNATE,
	 "oir", "c7200 OIR command", PRIV_MAX | PRIV_HIDDEN);

#undef  ALTERNATE
#define ALTERNATE    c7100_oir_kwd
