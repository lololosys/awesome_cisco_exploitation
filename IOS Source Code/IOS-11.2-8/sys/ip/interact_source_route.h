/* $Id: interact_source_route.h,v 3.1.40.1 1996/08/12 16:03:06 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/interact_source_route.h,v $
 *------------------------------------------------------------------
 * I N T E R A C T _ S O U R C E _ R O U T E . H
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: interact_source_route.h,v $
 * Revision 3.1.40.1  1996/08/12  16:03:06  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:58:32  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:48:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:28:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(isr_eol, interact_parse_ipoptroute);

/* optional 8th address */
IPADDR_NAME(isr_8, isr_eol, isr_eol,
	    OBJ(paddr,8), "Next host in route");

/* optional 7th address */
IPADDR_NAME(isr_7, isr_8, isr_eol,
	    OBJ(paddr,7), "Next host in route");

/* optional 6th address */
IPADDR_NAME(isr_6, isr_7, isr_eol,
	    OBJ(paddr,6), "Next host in route");

/* optional 5th address */
IPADDR_NAME(isr_5, isr_6, isr_eol,
	    OBJ(paddr,5), "Next host in route");

/* optional 4th address */
IPADDR_NAME(isr_4, isr_5, isr_eol,
	    OBJ(paddr,4), "Next host in route");

/* optional 3rd address */
IPADDR_NAME(isr_3, isr_4, isr_eol,
	    OBJ(paddr,3), "Next host in route");

/* optional 2nd address */
IPADDR_NAME(isr_2, isr_3, isr_eol,
	    OBJ(paddr,2), "Next host in route");

/* required 1st address */
IPADDR_NAME(isr_1, isr_2, NONE,
	    OBJ(paddr,1), "Next host in route");

NOP	(interact_source_route, isr_1, ALTERNATE);

#undef	ALTERNATE
#define	ALTERNATE	interact_source_route
