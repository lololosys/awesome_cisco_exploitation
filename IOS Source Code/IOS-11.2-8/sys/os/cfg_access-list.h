/* $Id: cfg_access-list.h,v 3.2 1995/11/17 18:45:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_access-list.h,v $
 *------------------------------------------------------------------
 * C F G _ A C C E S S _ L I S T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_access-list.h,v $
 * Revision 3.2  1995/11/17  18:45:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:50:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:00:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(cfg_accl_extend_here, no_alt);

/*****************************************************************/
KEYWORD	(access_list, cfg_accl_extend_here, ALTERNATE, 
	"access-list", "Add an access list entry", PRIV_CONF | PRIV_DISTILLED);

#undef	ALTERNATE
#define	ALTERNATE	access_list
