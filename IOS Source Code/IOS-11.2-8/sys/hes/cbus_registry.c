/* $Id: cbus_registry.c,v 3.1 1996/01/25 11:17:48 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/hes/cbus_registry.c,v $
 *------------------------------------------------------------------
 * cbus_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cbus_registry.c,v $
 * Revision 3.1  1996/01/25  11:17:48  smackie
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
#include "if_fci.h"
#include "cbus_registry.h"
#include "../hes/cbus_registry.regc"

/*
 * cbus_registry_init
 *
 * Initialize CBUS Registry
 */
 
static void cbus_registry_init (subsystype *subsys)
{
    registry_create(REG_CBUS, SERVICE_CBUS_MAX, "CBUS Platform");
    create_registry_cbus();
}
 
/*
 * CBUS Registry subsystem header
 */
 
#define CBUS_REGISTRY_MAJVERSION 1
#define CBUS_REGISTRY_MINVERSION 0
#define CBUS_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(cbus_registry,
              CBUS_REGISTRY_MAJVERSION, CBUS_REGISTRY_MINVERSION,
              CBUS_REGISTRY_EDITVERSION,
              cbus_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
