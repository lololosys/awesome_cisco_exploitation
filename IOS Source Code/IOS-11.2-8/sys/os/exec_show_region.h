/* $Id: exec_show_region.h,v 3.2.62.1 1996/08/12 16:05:15 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_region.h,v $
 *------------------------------------------------------------------
 * exec_show_region.h
 *
 * February 1995, Scott Mackie
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_region.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:15  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:47:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:12  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/21  18:18:05  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:25:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * show region
 */
EOLNS	(region_show_address_eol, region_show_address);

NUMBER  (region_show_address_num, region_show_address_eol, NONE,
	 OBJ(int,1), 0, 0xFFFFFFFF,
	 NULL /*"Address to search region list for"*/);

EOLNS	(region_show_eol, region_show_all);

KEYWORD (region_show_address, region_show_address_num, region_show_eol,
	 "address", NULL /*"Show region for <address>"*/,
	 PRIV_OPR | PRIV_HIDDEN);

KEYWORD (region_show, region_show_address, ALTERNATE,
	 "region", NULL /*"Region Manager Status"*/,
	 PRIV_OPR | PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	region_show
