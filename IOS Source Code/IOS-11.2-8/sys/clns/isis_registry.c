/* $Id: isis_registry.c,v 3.1 1996/01/25 11:16:48 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/clns/isis_registry.c,v $
 *------------------------------------------------------------------
 * isis_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isis_registry.c,v $
 * Revision 3.1  1996/01/25  11:16:48  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "isis.h"
#include "isis_registry.h"
#include "../clns/isis_registry.regc"

/*
 * isis_registry_init
 *
 * Initialize ISIS Registry
 */
 
static void isis_registry_init (subsystype *subsys)
{
    registry_create(REG_ISIS, SERVICE_ISIS_MAX, "ISIS");
    create_registry_isis();
}
 
/*
 * ISIS Registry subsystem header
 */
 
#define ISIS_REGISTRY_MAJVERSION 1
#define ISIS_REGISTRY_MINVERSION 0
#define ISIS_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(isis_registry,
              ISIS_REGISTRY_MAJVERSION, ISIS_REGISTRY_MINVERSION,
              ISIS_REGISTRY_EDITVERSION,
              isis_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
