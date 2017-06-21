/* $Id: eigrp_registry.c,v 3.2 1996/11/12 04:35:42 dkatz Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/eigrp_registry.c,v $
 *------------------------------------------------------------------
 * Registry code for EIGRP 
 *
 * November 1996, Dave Katz
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: eigrp_registry.c,v $
 * Revision 3.2  1996/11/12 04:35:42  dkatz
 * CSCdi76003:  EIGRP isnt modular
 * Clean up the tendrils reaching into EIGRP from the generic IP routing
 * code.
 *
 * Revision 3.1  1996/11/12 03:46:53  dkatz
 * Add placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "route.h"
#include "eigrp_registry.h"
#include "../iprouting/eigrp_registry.regc"


/*
 * eigrp_registry_init
 *
 * Initialize EIGRP Registry
 */
 
static void eigrp_registry_init (subsystype *subsys)
{
    registry_create(REG_EIGRP, SERVICE_EIGRP_MAX, "EIGRP");
    create_registry_eigrp();
}
 
/*
 * EIGRP Registry subsystem header
 */
 
#define EIGRP_REGISTRY_MAJVERSION 1
#define EIGRP_REGISTRY_MINVERSION 0
#define EIGRP_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(eigrp_registry,
              EIGRP_REGISTRY_MAJVERSION, EIGRP_REGISTRY_MINVERSION,
              EIGRP_REGISTRY_EDITVERSION,
              eigrp_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
