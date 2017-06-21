/* $Id: sierra_atmzr.c,v 3.2 1995/11/17 18:41:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-c4000/sierra_atmzr.c,v $
 *------------------------------------------------------------------
 * Sierra platform specific ATM code.
 *
 * January, 1995  Walter R. Friedrich
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sierra_atmzr.c,v $
 * Revision 3.2  1995/11/17  18:41:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:22:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***********************************************************************
                                Includes
*********************************************************************/
#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "media_registry.h"
#include "exception.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "sierra_nim.h"
#include "sierra_wrappers.h"



/*
 * Set up the saved idb variable, the interrupt vector
 * and initialize the vector register.
 */
boolean
init_atmzr_dev (hwidbtype *idb)
{
    /*
     * Check argument
     */
    if (!idb)
	return (FALSE);

    /*
     * Set up the level 2 jumptable
     */
    set_slot_idb (idb);
    set_slot_vector (idb->slot, atm_wrapper);
   
    return (TRUE);
}

