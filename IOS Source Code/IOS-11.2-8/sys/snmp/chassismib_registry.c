/* $Id: chassismib_registry.c,v 3.1.2.1 1996/05/21 06:38:34 mordock Exp $
 * $Source: /release/112/cvs/Xsys/snmp/chassismib_registry.c,v $
 *------------------------------------------------------------------
 * chassismib_registry.c
 *
 * May 1996, Scott R. Mordock
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: chassismib_registry.c,v $
 * Revision 3.1.2.1  1996/05/21  06:38:34  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1  1996/05/20  17:02:11  mordock
 * placeholders for california
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "chassismib_registry.h"
#include "../snmp/chassismib_registry.regc"

/*
 * chassismib_registry_init
 *
 * Initialize CHASSISMIB Registry
 */
 
static void chassismib_registry_init (subsystype *subsys)
{
    registry_create(REG_CHASSISMIB, SERVICE_CHASSISMIB_MAX, "CHASSISMIB");
    create_registry_chassismib();
}
 
/*
 * CHASSISMIB Registry subsystem header
 */
 
#define CHASSISMIB_REGISTRY_MAJVERSION 1
#define CHASSISMIB_REGISTRY_MINVERSION 0
#define CHASSISMIB_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(chassismib_registry,
              CHASSISMIB_REGISTRY_MAJVERSION, CHASSISMIB_REGISTRY_MINVERSION,
              CHASSISMIB_REGISTRY_EDITVERSION,
              chassismib_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
