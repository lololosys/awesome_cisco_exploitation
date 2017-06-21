/* $Id: cs_registry.c,v 3.1 1996/01/25 11:20:18 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/os/cs_registry.c,v $
 *------------------------------------------------------------------
 * cs_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cs_registry.c,v $
 * Revision 3.1  1996/01/25  11:20:18  smackie
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
#include "registry.h"
#include "../h/cs_registry.regh"
#include "../h/cs_registry.regc"

/*
 * cs_registry_init
 *
 * Initialize Communications Server Registry
 */
 
static void cs_registry_init (subsystype *subsys)
{
    registry_create(REG_CS, SERVICE_CS_MAX, "Communications Server");
    create_registry_cs();
}
 
/*
 * CS Registry subsystem header
 */
 
#define CS_REGISTRY_MAJVERSION 1
#define CS_REGISTRY_MINVERSION 0
#define CS_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(cs_registry,
              CS_REGISTRY_MAJVERSION, CS_REGISTRY_MINVERSION,
              CS_REGISTRY_EDITVERSION,
              cs_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
