/* $Id: fr_registry.c,v 3.1.40.1 1996/04/25 23:23:28 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_registry.c,v $
 *------------------------------------------------------------------
 * fr_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_registry.c,v $
 * Revision 3.1.40.1  1996/04/25  23:23:28  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.44.1  1996/04/24  04:11:23  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1996/01/25  11:23:15  smackie
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
#include "frame_relay.h"
#include "fr_arp.h"
#include "fr_registry.h"
#include "../wan/fr_registry.regc"

/*
 * frame_relay_registry_init
 *
 * Initialize FRAME_RELAY Registry
 */
 
static void frame_relay_registry_init (subsystype *subsys)
{
    registry_create(REG_FRAME_RELAY, SERVICE_FRAME_RELAY_MAX, "Frame Relay");
    create_registry_frame_relay();
}
 
/*
 * FRAME_RELAY Registry subsystem header
 */
 
#define FRAME_RELAY_REGISTRY_MAJVERSION 1
#define FRAME_RELAY_REGISTRY_MINVERSION 0
#define FRAME_RELAY_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(frame_relay_registry,
              FRAME_RELAY_REGISTRY_MAJVERSION,
	      FRAME_RELAY_REGISTRY_MINVERSION,
              FRAME_RELAY_REGISTRY_EDITVERSION,
              frame_relay_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
