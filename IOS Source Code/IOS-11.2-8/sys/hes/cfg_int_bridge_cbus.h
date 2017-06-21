/* $Id: cfg_int_bridge_cbus.h,v 3.1 1996/02/01 04:57:52 hampton Exp $
 * $Source: /release/111/cvs/Xsys/hes/cfg_int_bridge_cbus.h,v $
 *------------------------------------------------------------------
 * CBUS Interface Bridging commands
 *
 * October 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_bridge_cbus.h,v $
 * Revision 3.1  1996/02/01  04:57:52  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:43:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:03:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************************
 * bridge <group> cbus-bridging
 */
EOLS	(bgroup_cbusbridge_eol, cbus_bridgegroup_command, BGROUP_CBUSBRIDGE);
KEYWORD	(bgroup_cbusbridge, bgroup_cbusbridge_eol, ALTERNATE,
	 "cbus-bridging", "cBus assisted datagram bridging", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	bgroup_cbusbridge
