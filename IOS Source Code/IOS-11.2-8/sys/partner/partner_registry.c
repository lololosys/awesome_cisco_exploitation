/* $Id: partner_registry.c,v 3.1 1996/01/25 11:21:06 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/partner/partner_registry.c,v $
 *------------------------------------------------------------------
 * partner_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: partner_registry.c,v $
 * Revision 3.1  1996/01/25  11:21:06  smackie
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
#include "partner_registry.h"
#include "../partner/partner_registry.regc"

/*
 * partner_registry_init
 *
 * Initialize PARTNER Registry
 */
 
static void partner_registry_init (subsystype *subsys)
{
    registry_create(REG_PARTNER, SERVICE_PARTNER_MAX, "Partner");
    create_registry_partner();
}
 
/*
 * PARTNER Registry subsystem header
 */
 
#define PARTNER_REGISTRY_MAJVERSION 1
#define PARTNER_REGISTRY_MINVERSION 0
#define PARTNER_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(partner_registry,
              PARTNER_REGISTRY_MAJVERSION, PARTNER_REGISTRY_MINVERSION,
              PARTNER_REGISTRY_EDITVERSION,
              partner_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
