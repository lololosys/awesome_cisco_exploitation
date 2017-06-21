/* $Id: auth_registry.c,v 3.1 1996/01/25 11:20:17 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/os/auth_registry.c,v $
 *------------------------------------------------------------------
 * auth_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: auth_registry.c,v $
 * Revision 3.1  1996/01/25  11:20:17  smackie
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
#include "address.h"
#include "aaa.h"
#include "registry.h"
#include "../h/auth_registry.regh"
#include "../h/auth_registry.regc"

/*
 * auth_registry_init
 *
 * Initialize Authentication Registry
 */
 
static void auth_registry_init (subsystype *subsys)
{
    registry_create(REG_AUTH, SERVICE_AUTH_MAX, "Authentication");
    create_registry_auth();
}
 
/*
 * Authentication Registry subsystem header
 */
 
#define AUTH_REGISTRY_MAJVERSION 1
#define AUTH_REGISTRY_MINVERSION 0
#define AUTH_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(auth_registry,
              AUTH_REGISTRY_MAJVERSION, AUTH_REGISTRY_MINVERSION,
              AUTH_REGISTRY_EDITVERSION,
              auth_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
