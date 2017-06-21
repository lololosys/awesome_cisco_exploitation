/* $Id: cfg_int_mop.h,v 3.2.60.1 1996/06/17 23:29:51 hampton Exp $
 * $Source: /release/112/cvs/Xsys/decnet/cfg_int_mop.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ M O P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_mop.h,v $
 * Revision 3.2.60.1  1996/06/17  23:29:51  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:58:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:26  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:06:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * mop 
 *
 * exits from config mode
 */

EOLS	(mop_int_sysid_eol, mopif_config, MOP_CFG_SYSID );
KEYWORD	(mop_int_sysid, mop_int_sysid_eol, no_alt,
	 "sysid", "Send out MOP periodic system ID messages", PRIV_CONF);

EOLS	(mop_int_enable_eol, mopif_config, MOP_CFG_ENABLED );
KEYWORD	(mop_int_enable, mop_int_enable_eol, mop_int_sysid,
	 "enabled", "Enable MOP for interface", PRIV_CONF);

KEYWORD	(mop_int, mop_int_enable, no_alt,
	 "mop", "DEC MOP server commands", PRIV_CONF);

TEST_IDBSTATUS(mop_test, mop_int, NONE, ALTERNATE, IDB_ETHER | IDB_SERIAL);

#undef	ALTERNATE
#define	ALTERNATE	mop_test
