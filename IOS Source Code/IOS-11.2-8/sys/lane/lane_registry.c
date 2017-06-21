/* $Id: lane_registry.c,v 3.1 1996/01/25 11:19:54 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/lane/lane_registry.c,v $
 *------------------------------------------------------------------
 * lane_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_registry.c,v $
 * Revision 3.1  1996/01/25  11:19:54  smackie
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
#include "registry.h"
#include "interface_private.h"
#include "address.h"
#include "../if/atm.h"
#include "lane.h"
#include "lane_registry.h"
#include "../lane/lane_registry.regc"

/*
 * lane_registry_init
 *
 * Initialize LANE Registry
 */
 
static void lane_registry_init (subsystype *subsys)
{
    registry_create(REG_LANE, SERVICE_LANE_MAX, "LAN Emulation");
    create_registry_lane();
}
 
/*
 * LANE Registry subsystem header
 */
 
#define LANE_REGISTRY_MAJVERSION 1
#define LANE_REGISTRY_MINVERSION 0
#define LANE_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(lane_registry,
              LANE_REGISTRY_MAJVERSION, LANE_REGISTRY_MINVERSION,
              LANE_REGISTRY_EDITVERSION,
              lane_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
