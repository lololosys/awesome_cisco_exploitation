/* $Id: ifmib_registry.c,v 3.1 1996/01/25 11:22:30 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/ifmib_registry.c,v $
 *------------------------------------------------------------------
 * ifmib_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ifmib_registry.c,v $
 * Revision 3.1  1996/01/25  11:22:30  smackie
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
#include "ifmib_registry.h"
#include "../snmp/ifmib_registry.regc"

/*
 * ifmib_registry_init
 *
 * Initialize IFMIB Registry
 */
 
static void ifmib_registry_init (subsystype *subsys)
{
    registry_create(REG_IFMIB, SERVICE_IFMIB_MAX, "IFMIB");
    create_registry_ifmib();
}
 
/*
 * IFMIB Registry subsystem header
 */
 
#define IFMIB_REGISTRY_MAJVERSION 1
#define IFMIB_REGISTRY_MINVERSION 0
#define IFMIB_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(ifmib_registry,
              IFMIB_REGISTRY_MAJVERSION, IFMIB_REGISTRY_MINVERSION,
              IFMIB_REGISTRY_EDITVERSION,
              ifmib_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
