/* $Id: cfg_int_ip2_routing.h,v 3.2.60.1 1996/08/12 16:03:24 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_int_ip2_routing.h,v $
 *------------------------------------------------------------------
 * cfg_int_ip2_routing.h
 *
 * May 1994, Derek Yeung
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * IP routing interface commands that must be after the encapsulation command
 * in NV order.
 *------------------------------------------------------------------
 * $Log: cfg_int_ip2_routing.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:24  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:31:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/04  03:07:45  dstine
 * CSCdi37681:  Need event logging for EIGRP
 *
 * Revision 2.1  1995/07/01  12:15:32  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:05:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] ip split-horizon 
 * [no] ip split-horizon eigrp <as-number>
 *
 * OBJ(pdb,1) = IGRP1 pdb for <as-number> for the second form
 */
EOLNS (ci_ip_sh_eols, ip_split_horizon_command);
LINK_TRANS(ci_ip_split_link, ci_ip_sh_eols);
KEYWORD	(ci_ip_igrp2_sh, ci_ip_split_link, ALTERNATE,
	 "split-horizon", "Perform split horizon", PRIV_CONF|PRIV_SUBIF);

#undef  ALTERNATE
#define	ALTERNATE	ci_ip_igrp2_sh

