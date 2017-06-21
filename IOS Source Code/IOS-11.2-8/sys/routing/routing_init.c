/* $Id: routing_init.c,v 3.3 1996/01/25 11:21:30 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/routing/routing_init.c,v $
 *------------------------------------------------------------------
 * routing.c -- Integrated routing subsystem initialization
 *
 * Mar 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: routing_init.c,v $
 * Revision 3.3  1996/01/25  11:21:30  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.2  1995/11/17  18:49:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:43:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "routing_registry.h"
#include "../routing/routing_registry.regc"

long routing_regcode;


/*
 * routing_init
 * Initialize the integrated routing subsystem.
 */

static void routing_init (subsystype *subsys)
{
    routing_regcode = registry_create(REG_UNKNOWN, SERVICE_ROUTING_MAX,
				      "Routing");
    if (routing_regcode != REG_ILLEGAL)
	create_registry_routing();
}


/*
 * Routing subsystem header
 */

#define ROUTING_MAJVERSION 1
#define ROUTING_MINVERSION 0
#define ROUTING_EDITVERSION  1

SUBSYS_HEADER(routing, ROUTING_MAJVERSION, ROUTING_MINVERSION,
	      ROUTING_EDITVERSION, routing_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);
