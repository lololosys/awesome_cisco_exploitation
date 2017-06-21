/* $Id: ip_registry.c,v 3.1 1996/01/25 11:19:18 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/ip/ip_registry.c,v $
 *------------------------------------------------------------------
 * ip_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_registry.c,v $
 * Revision 3.1  1996/01/25  11:19:18  smackie
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
#include "ip.h"
#include "tunnel.h"
#include "../iprouting/route.h"
#include "ip_registry.h"
#include "../ip/ip_registry.regc"

/*
 * ip_registry_init
 *
 * Initialize IP Registry
 */
 
static void ip_registry_init (subsystype *subsys)
{
    registry_create(REG_IP, SERVICE_IP_MAX, "IP");
    create_registry_ip();
}
 
/*
 * IP Registry subsystem header
 */
 
#define IP_REGISTRY_MAJVERSION 1
#define IP_REGISTRY_MINVERSION 0
#define IP_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(ip_registry,
              IP_REGISTRY_MAJVERSION, IP_REGISTRY_MINVERSION,
              IP_REGISTRY_EDITVERSION,
              ip_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
