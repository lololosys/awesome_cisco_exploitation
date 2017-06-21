/* $Id: envmib_registry.c,v 3.1 1996/01/25 11:17:50 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/hes/envmib_registry.c,v $
 *------------------------------------------------------------------
 * envmib_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: envmib_registry.c,v $
 * Revision 3.1  1996/01/25  11:17:50  smackie
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
#include "envmib_registry.h"
#include "../hes/envmib_registry.regc"

/*
 * envmib_registry_init
 *
 * Initialize ENVMIB Registry
 */
 
static void envmib_registry_init (subsystype *subsys)
{
    registry_create(REG_ENVMIB, SERVICE_ENVMIB_MAX, "ENVMIB");
    create_registry_envmib();
}
 
/*
 * ENVMIB Registry subsystem header
 */
 
#define ENVMIB_REGISTRY_MAJVERSION 1
#define ENVMIB_REGISTRY_MINVERSION 0
#define ENVMIB_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(envmib_registry,
              ENVMIB_REGISTRY_MAJVERSION, ENVMIB_REGISTRY_MINVERSION,
              ENVMIB_REGISTRY_EDITVERSION,
              envmib_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
