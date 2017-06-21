/* $Id: cfg_int_bstun.h,v 3.2 1995/11/17 08:49:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/cfg_int_bstun.h,v $
 *------------------------------------------------------------------
 * cfg_int_bstun.h - BSTUN Interface configuration commands (part1)
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_bstun.h,v $
 * Revision 3.2  1995/11/17  08:49:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:35:31  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.2  1995/06/16  06:23:11  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  22:03:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 * no bstun
 */
EOLS	(ci_bstun_eol, bstunif_command, NO_BSTUN );
TESTVAR	(ci_bstun_noprefix, ci_bstun_eol, NONE,
	 NONE, NONE, no_alt, sense, FALSE );

/************************************************************************
 * [no] bstun group <group-number>
 */
PARAMS (ci_bstun_grp, ci_bstun_noprefix, "group",
	     OBJ(int,1), BSTUN_MIN_GROUP, BSTUN_MAX_GROUP,
	     bstunif_command, BSTUNIF_GROUP,
	     "Assign an interface to a BSTUN group",
	     "A BSTUN group number", PRIV_CONF);

KEYWORD	(ci_bstun, ci_bstun_grp, NONE, "bstun",
	 "BSTUN interface subcommands", PRIV_CONF);

ASSERT	(ci_bstun_test, ci_bstun, ALTERNATE,
	 (csb->interface->hwptr->status & IDB_SERIAL));

#undef  ALTERNATE
#define ALTERNATE ci_bstun_test
