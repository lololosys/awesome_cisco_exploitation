/* $Id: pt_registry.c,v 3.1 1996/01/25 11:21:21 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/pt/pt_registry.c,v $
 *------------------------------------------------------------------
 * pt_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pt_registry.c,v $
 * Revision 3.1  1996/01/25  11:21:21  smackie
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
#include "translate.h"
#include "pt_registry.h"
#include "../pt/pt_registry.regc"

/*
 * pt_registry_init
 *
 * Initialize PT Registry
 */
 
static void pt_registry_init (subsystype *subsys)
{
    registry_create(REG_PT, SERVICE_PT_MAX, "Protocol Translator");
    create_registry_pt();
}
 
/*
 * PT Registry subsystem header
 */
 
#define PT_REGISTRY_MAJVERSION 1
#define PT_REGISTRY_MINVERSION 0
#define PT_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(pt_registry,
              PT_REGISTRY_MAJVERSION, PT_REGISTRY_MINVERSION,
              PT_REGISTRY_EDITVERSION,
              pt_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
