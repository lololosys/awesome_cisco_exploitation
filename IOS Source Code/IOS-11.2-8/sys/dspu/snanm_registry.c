/* $Id: snanm_registry.c,v 3.1 1996/01/25 11:17:07 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/dspu/snanm_registry.c,v $
 *------------------------------------------------------------------
 * snanm_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snanm_registry.c,v $
 * Revision 3.1  1996/01/25  11:17:07  smackie
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
#include "registry.h"
#include "../dspu/snanm.h"
#include "../dspu/snanm_registry.regh"
#include "../dspu/snanm_registry.regc"

/*
 * snanm_registry_init
 *
 * Initialize SNANM Registry
 */
 
static void snanm_registry_init (subsystype *subsys)
{
    registry_create(REG_SNANM, SERVICE_SNANM_MAX, "SNANM");
    create_registry_snanm();
}
 
/*
 * SNANM Registry subsystem header
 */
 
#define SNANM_REGISTRY_MAJVERSION 1
#define SNANM_REGISTRY_MINVERSION 0
#define SNANM_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(snanm_registry,
              SNANM_REGISTRY_MAJVERSION, SNANM_REGISTRY_MINVERSION,
              SNANM_REGISTRY_EDITVERSION,
              snanm_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
