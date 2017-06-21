/* $Id: dspumib_subsys_init.c,v 3.2 1995/11/17 09:05:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/dspumib_subsys_init.c,v $
 *------------------------------------------------------------------
 * DSPUMIB Subsystem Initialization
 *
 * October 1994, Paul Pearce 
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Initializes DSPU MIB support as a subsystem
 *------------------------------------------------------------------
 * $Log: dspumib_subsys_init.c,v $
 * Revision 3.2  1995/11/17  09:05:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/09  13:16:18  ppearce
 * CSCdi38432:  dspu subsystem causing confusion, rename it
 *   Change dspu subsystem name in subsytems that require dspu
 *
 * Revision 2.2  1995/06/28  10:18:59  smackie
 * Subsystem header cleanup. Round up the last few stragglers and rope 'em
 * in with the rest of the herd. (CSCdi23568)
 *
 * Revision 2.1  1995/06/07  20:28:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "subsys.h"
#include "sr_dspumib.h"
#include "dspumib_mgmt.h"

/*
 * dspumib_subsys_init()
 *   Initialization point for the DSPUMIB subsystem
 *
 */

static void dspumib_subsys_init (subsystype *subsys)
{
    init_dspumib();                /* Initialization for DSPU MIB access */
    InitDspuMibTables();           /* Initialization for DSPU MIB mgmt   */
}


/*
 * DSPUMIB subsystem header
 */

#define DSPUMIB_MAJVERSION 1
#define DSPUMIB_MINVERSION 0
#define DSPUMIB_EDITVERSION  1

SUBSYS_HEADER(dspumib, 
              DSPUMIB_MAJVERSION, DSPUMIB_MINVERSION, DSPUMIB_EDITVERSION,
	      dspumib_subsys_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: sna_pu", NULL);
