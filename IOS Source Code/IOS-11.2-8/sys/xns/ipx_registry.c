/* $Id: ipx_registry.c,v 3.1 1996/01/25 11:24:25 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/xns/ipx_registry.c,v $
 *------------------------------------------------------------------
 * ipx_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipx_registry.c,v $
 * Revision 3.1  1996/01/25  11:24:25  smackie
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
#include "novell.h"
#include "../clns/clns.h"
#include "../clns/isis_externs.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_ipx.h"
#include "ipx_registry.h"
#include "../xns/ipx_registry.regc"

/*
 * ipx_registry_init
 *
 * Initialize IPX Registry
 */
 
static void ipx_registry_init (subsystype *subsys)
{
    registry_create(REG_IPX, SERVICE_IPX_MAX, "IPX");
    create_registry_ipx();
}
 
/*
 * IPX Registry subsystem header
 */
 
#define IPX_REGISTRY_MAJVERSION 1
#define IPX_REGISTRY_MINVERSION 0
#define IPX_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(ipx_registry,
              IPX_REGISTRY_MAJVERSION, IPX_REGISTRY_MINVERSION,
              IPX_REGISTRY_EDITVERSION,
              ipx_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
