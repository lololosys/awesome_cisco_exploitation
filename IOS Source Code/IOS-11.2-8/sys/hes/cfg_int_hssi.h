/* $Id: cfg_int_hssi.h,v 3.1 1996/02/01 04:57:57 hampton Exp $
 * $Source: /release/111/cvs/Xsys/hes/cfg_int_hssi.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ H S S I . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_hssi.h,v $
 * Revision 3.1  1996/02/01  04:57:57  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:43:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:05:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * hssi internal-clock
 * no hssi internal-clock
 */
EOLS	(ci_hssi_internal_eol, hssi_command, HSSI_INTERNAL_CLOCK);

KEYWORD	(ci_hssi_internal, ci_hssi_internal_eol, no_alt,
	 "internal-clock", "Provide clocking on the transmitter clock line",
	 PRIV_CONF);


/******************************************************************
 * hssi external-loopback-request
 * no hssi external-loopback-request
 */
EOLS	(ci_hssi_external_eol, hssi_command, HSSI_LC_INHIBIT);

KEYWORD	(ci_hssi_external, ci_hssi_external_eol, ci_hssi_internal,
	 "external-loopback-request", "Enable external loopback", PRIV_CONF);


KEYWORD	(ci_hssi, ci_hssi_external, NONE,
	 "hssi", "Modify HSSI parameters", PRIV_CONF);

TEST_IDBSTATUS(ci_hssi_test, ci_hssi, NONE, ALTERNATE, IDB_HSSI);

#undef	ALTERNATE
#define	ALTERNATE	ci_hssi_test
