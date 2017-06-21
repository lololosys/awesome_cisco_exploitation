/* $Id: cfg_int_ip_nat.h,v 3.1.6.2 1996/09/04 17:16:00 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_int_ip_nat.h,v $
 *------------------------------------------------------------------
 * NAT interface commands
 *
 * October 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_nat.h,v $
 * Revision 3.1.6.2  1996/09/04  17:16:00  ahh
 * CSCdi67885:  NAT: sub-interfaces not supported properly
 * Allow interface commands to work in sub-interface configuration
 * mode, allow translation to be enabled on some sub-interfaces
 * but not others and still have the fast-path work.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:52:58  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  05:56:03  ahh
 * Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] ip nat inside
 */

EOLS	(ci_ip_nat_inside_eol, ipnat_if_command, IPNAT_IF_INSIDE);
KEYWORD	(ci_ip_nat_inside, ci_ip_nat_inside_eol, NONE,
	 "inside",
	 "Inside interface for address translation",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ip nat outside
 */

EOLS	(ci_ip_nat_outside_eol, ipnat_if_command, IPNAT_IF_OUTSIDE);
KEYWORD	(ci_ip_nat_outside, ci_ip_nat_outside_eol, ci_ip_nat_inside,
	 "outside",
	 "Outside interface for address translation",
	 PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_ip_nat, ci_ip_nat_outside, ALTERNATE,
	 "nat", "NAT interface commands", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_nat
