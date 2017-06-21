/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * lanmgr_registry.c
 *
 * January 1997, Nitin Karkhanis
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "registry.h"
#include "../ibm/lanmgr_registry.regh"
#include "../ibm/lanmgr_registry.regc"


/*
 * lanmgr_registry_init
 *
 * Initialize LAN MANAGER Registry
 */
 
static void lanmgr_registry_init (subsystype *subsys)
{
    registry_create(REG_LANMGR, SERVICE_LANMGR_MAX, "LANMGR");
    create_registry_lanmgr();
}
 
/*
 * LANMGR Registry subsystem header
 */
 
#define LANMGR_REGISTRY_MAJVERSION 1
#define LANMGR_REGISTRY_MINVERSION 0
#define LANMGR_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(lanmgr_registry,
              LANMGR_REGISTRY_MAJVERSION, LANMGR_REGISTRY_MINVERSION,
              LANMGR_REGISTRY_EDITVERSION,
              lanmgr_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
