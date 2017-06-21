/* $Id: dlc_registry.c,v 3.1 1996/01/25 11:16:36 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/cls/dlc_registry.c,v $
 *------------------------------------------------------------------
 * dlc_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dlc_registry.c,v $
 * Revision 3.1  1996/01/25  11:16:36  smackie
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
#include "dlc_registry.h"
#include "../cls/dlc_registry.regc"

/*
 * dlc_registry_init
 *
 * Initialize DLC Registry
 */
 
static void dlc_registry_init (subsystype *subsys)
{
    registry_create(REG_DLC, SERVICE_DLC_MAX, "DLC");
    create_registry_dlc();
}
 
/*
 * DLC Registry subsystem header
 */
 
#define DLC_REGISTRY_MAJVERSION 1
#define DLC_REGISTRY_MINVERSION 0
#define DLC_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(dlc_registry,
              DLC_REGISTRY_MAJVERSION, DLC_REGISTRY_MINVERSION,
              DLC_REGISTRY_EDITVERSION,
              dlc_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
