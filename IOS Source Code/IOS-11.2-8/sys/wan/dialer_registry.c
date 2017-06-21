/* $Id: dialer_registry.c,v 3.2 1996/02/13 08:17:32 dstine Exp $
 * $Source: /release/111/cvs/Xsys/wan/dialer_registry.c,v $
 *------------------------------------------------------------------
 * dialer_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dialer_registry.c,v $
 * Revision 3.2  1996/02/13  08:17:32  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.1  1996/01/25  11:23:14  smackie
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
#include "packet.h"
#include "address.h"
#include "../wan/dialer.h"
#include "dialer_registry.h"
#include "../wan/dialer_registry.regc"

/*
 * dialer_registry_init
 *
 * Initialize DIALER Registry
 */
 
static void dialer_registry_init (subsystype *subsys)
{
    registry_create(REG_DIALER, SERVICE_DIALER_MAX, "Dialer");
    create_registry_dialer();
}
 
/*
 * DIALER Registry subsystem header
 */
 
#define DIALER_REGISTRY_MAJVERSION 1
#define DIALER_REGISTRY_MINVERSION 0
#define DIALER_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(dialer_registry,
              DIALER_REGISTRY_MAJVERSION, DIALER_REGISTRY_MINVERSION,
              DIALER_REGISTRY_EDITVERSION,
              dialer_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
