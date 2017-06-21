/* $Id: snmp_registry.c,v 3.1 1996/01/25 11:22:31 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/snmp_registry.c,v $
 *------------------------------------------------------------------
 * snmp_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snmp_registry.c,v $
 * Revision 3.1  1996/01/25  11:22:31  smackie
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
#include "sr_snmp.h"
#include "../parser/parser_defs_snmp.h"
#include "snmp_registry.h"
#include "../snmp/snmp_registry.regc"

/*
 * snmp_registry_init
 *
 * Initialize SNMP Registry
 */
 
static void snmp_registry_init (subsystype *subsys)
{
    registry_create(REG_SNMP, SERVICE_SNMP_MAX, "SNMP");
    create_registry_snmp();
}
 
/*
 * SNMP Registry subsystem header
 */
 
#define SNMP_REGISTRY_MAJVERSION 1
#define SNMP_REGISTRY_MINVERSION 0
#define SNMP_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(snmp_registry,
              SNMP_REGISTRY_MAJVERSION, SNMP_REGISTRY_MINVERSION,
              SNMP_REGISTRY_EDITVERSION,
              snmp_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
