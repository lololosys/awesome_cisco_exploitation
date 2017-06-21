/* $Id: clns_registry.c,v 3.1 1996/01/25 11:16:47 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/clns/clns_registry.c,v $
 *------------------------------------------------------------------
 * clns_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_registry.c,v $
 * Revision 3.1  1996/01/25  11:16:47  smackie
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
#include "clns.h"
#include "clns_registry.h"
#include "../clns/clns_registry.regc"

/*
 * clns_registry_init
 *
 * Initialize CLNS Registry
 */
 
static void clns_registry_init (subsystype *subsys)
{
    registry_create(REG_CLNS, SERVICE_CLNS_MAX, "CLNS");
    create_registry_clns();
}
 
/*
 * CLNS Registry subsystem header
 */
 
#define CLNS_REGISTRY_MAJVERSION 1
#define CLNS_REGISTRY_MINVERSION 0
#define CLNS_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(clns_registry,
              CLNS_REGISTRY_MAJVERSION, CLNS_REGISTRY_MINVERSION,
              CLNS_REGISTRY_EDITVERSION,
              clns_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
