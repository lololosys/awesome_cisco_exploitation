/* $Id: flash_registry.c,v 3.1 1996/01/25 11:16:57 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/dev/flash_registry.c,v $
 *------------------------------------------------------------------
 * flash_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_registry.c,v $
 * Revision 3.1  1996/01/25  11:16:57  smackie
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
#include "sys_registry.h"
#include "flash_registry.h"
#include "../dev/flash_registry.regc"

/*
 * flash_registry_init
 *
 * Initialize FLASH Registry
 */
 
static void flash_registry_init (subsystype *subsys)
{
    registry_create(REG_FLASH, SERVICE_FLASH_MAX, "Flash");
    create_registry_flash();
}
 
/*
 * FLASH Registry subsystem header
 */
 
#define FLASH_REGISTRY_MAJVERSION 1
#define FLASH_REGISTRY_MINVERSION 0
#define FLASH_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(flash_registry,
              FLASH_REGISTRY_MAJVERSION, FLASH_REGISTRY_MINVERSION,
              FLASH_REGISTRY_EDITVERSION,
              flash_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
