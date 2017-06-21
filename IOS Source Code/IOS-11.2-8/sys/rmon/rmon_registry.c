/* $Id: rmon_registry.c,v 3.3 1996/01/25 11:21:27 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/rmon/rmon_registry.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * September 1995, Jeffrey T. Johnson
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rmon_registry.c,v $
 * Revision 3.3  1996/01/25  11:21:27  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.2  1995/11/17  18:39:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../rmon/rmon_registry.h"
#include "../rmon/rmon_registry.regc"

long rmon_regcode;

void
init_rmon_registry (void)
{
    rmon_regcode = registry_create(REG_UNKNOWN, SERVICE_RMON_MAX, "RMON");
    if (rmon_regcode != REG_ILLEGAL)
	create_registry_rmon();
}
