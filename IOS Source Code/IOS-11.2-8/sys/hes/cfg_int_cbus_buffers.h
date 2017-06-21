/* $Id: cfg_int_cbus_buffers.h,v 3.1 1996/02/01 04:57:55 hampton Exp $
 * $Source: /release/111/cvs/Xsys/hes/cfg_int_cbus_buffers.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ C B U S _ B U F F E R S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_cbus_buffers.h,v $
 * Revision 3.1  1996/02/01  04:57:55  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:43:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:04:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * cbus-buffers burst-count <num>
 * no cbus-buffers burst-count
 */
PARAMS_KEYONLY(cbus_buffers_burst, no_alt,
	       "burst-count", OBJ(int,1), 1, -1,
	       cbus_buffers_command, CBUS_BUFFERS_BURSTCOUNT,
	       "Cbus buffers for this interface",
	       "Number of buffers", PRIV_CONF );

/******************************************************************************
 * cbus-buffers maximum <num>
 * no cbus-buffers maximum
 */
PARAMS_KEYONLY(cbus_buffers_maximum, cbus_buffers_burst, 
	       "maximum", OBJ(int,1), 1, -1,
	       cbus_buffers_command, CBUS_BUFFERS_MAXIMUM,
	       "Limit the number of buffers on the cbus controller",
	       "Maximum number of buffers", PRIV_CONF );

KEYWORD	(cbus_buffers, cbus_buffers_maximum, NONE,
	 "cbus-buffers", "Cbus buffer parameters",
	 PRIV_CONF);

TEST_IDBSTATUS(cbus_buffers_test, cbus_buffers, NONE, ALTERNATE, IDB_CBUS);

#undef	ALTERNATE
#define	ALTERNATE	cbus_buffers_test
