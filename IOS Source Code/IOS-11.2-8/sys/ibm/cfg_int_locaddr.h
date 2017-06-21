/* $Id: cfg_int_locaddr.h,v 3.2 1995/11/17 09:21:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/cfg_int_locaddr.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ L O C A D D R . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_locaddr.h,v $
 * Revision 3.2  1995/11/17  09:21:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:47  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:06:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * locaddr-priority <num>
 * no locaddr-priority
 */

PARAMS_KEYONLY(ci_locaddr, NONE, "locaddr-priority",
	       OBJ(int,1), 1, 10,
	       locaddr_pri_command, 0,
	       "Assign a priority group", "Priority list number", PRIV_CONF);

TEST_BOOLEAN(ci_locaddr_test, ci_locaddr, NONE, ALTERNATE, bridge_enable);

/***************************************************************
 * sap-priority <list>
 * no locaddr-priority
 */

PARAMS_KEYONLY(ci_sappri, NONE, "sap-priority",
	       OBJ(int,1), 1, 10,
	       sap_pri_command, 0,
	       "Assign a priority group", "Priority list number", PRIV_CONF);

TEST_BOOLEAN(ci_sappri_test, ci_sappri, NONE, ci_locaddr_test, bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	ci_sappri_test
