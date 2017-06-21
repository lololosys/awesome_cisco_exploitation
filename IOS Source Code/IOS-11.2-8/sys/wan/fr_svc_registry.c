/* $Id: fr_svc_registry.c,v 3.1.40.1 1996/04/25 23:23:49 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_svc_registry.c,v $
 *------------------------------------------------------------------
 * fr_svc_registry.c
 *
 * February 1996, Shankar Natarajan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_svc_registry.c,v $
 * Revision 3.1.40.1  1996/04/25  23:23:49  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.44.1  1996/04/24  04:11:33  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1996/02/05  00:49:17  shankar
 * Stub file, used within the FR_SVC branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "frame_relay.h"
#include "fr_svc_registry.h"
#include "../wan/fr_svc_registry.regc"

/*
 * fr_svc_registry_init
 *
 * Initialize FR_SVC Registry
 */
 
static void fr_svc_registry_init (subsystype *subsys)
{
    registry_create(REG_FR_SVC, SERVICE_FR_SVC_MAX, "Frame Relay SVC");
    create_registry_fr_svc();
}
 
/*
 * FR_SVC Registry subsystem header
 */
 
#define FR_SVC_REGISTRY_MAJVERSION 1
#define FR_SVC_REGISTRY_MINVERSION 0
#define FR_SVC_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(fr_svc_registry,
              FR_SVC_REGISTRY_MAJVERSION,
	      FR_SVC_REGISTRY_MINVERSION,
              FR_SVC_REGISTRY_EDITVERSION,
              fr_svc_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
