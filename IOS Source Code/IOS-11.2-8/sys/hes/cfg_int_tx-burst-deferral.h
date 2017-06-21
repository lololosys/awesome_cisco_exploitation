/* $Id: cfg_int_tx-burst-deferral.h,v 3.2 1996/02/08 03:20:17 anke Exp $
 * $Source: /release/111/cvs/Xsys/hes/cfg_int_tx-burst-deferral.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ T X - B U R S T - D E F E R R A L . H
 *
 *
 * December 1994, Don Lobete
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Command intended as an Engineering knob to control the
 * number of bursted packets to defer to before initiating
 * a transmit.
 *------------------------------------------------------------------
 * $Log: cfg_int_tx-burst-deferral.h,v $
 * Revision 3.2  1996/02/08  03:20:17  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.1  1996/02/01  04:57:58  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:43:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:07:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 *      tx-burst-deferral { <number of packets> }
 *      no tx-burst-deferral [{ <number of packets> }]
 */
PARAMS_KEYONLY(ci_txburstdefer, NONE,
               "tx-burst-deferral", OBJ(int,1), 0, 100,
               txburst_command, 0,
               "Sets maximum number of back-to-back packets other stations can transmit before the EIP attempts to acquire carrier",
               "Number of back-to-back packets EIP waits for before attempting to acquire carrier",
               PRIV_CONF);

ASSERT (ci_txburstdefer_test, ci_txburstdefer, ALTERNATE,
        ((csb->interface->hwptr->status & (IDB_ETHER | IDB_CBUS)) 
	  == (IDB_ETHER | IDB_CBUS)));

#undef  ALTERNATE
#define ALTERNATE       ci_txburstdefer_test

