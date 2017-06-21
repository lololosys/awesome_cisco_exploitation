/* $Id: ppp_registry.c,v 3.1.2.1 1996/04/27 06:38:08 syiu Exp $
 * $Source: /release/112/cvs/Xsys/wan/ppp_registry.c,v $
 *------------------------------------------------------------------
 * ppp_registry.c
 *
 * April 1996, Shoou Yiu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ppp_registry.c,v $
 * Revision 3.1.2.1  1996/04/27  06:38:08  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.4.1  1996/04/17  00:02:11  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.1  1996/04/06  02:10:05  syiu
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "ppp_registry.h"
#include "../wan/ppp_registry.regc"

/*
 * ppp_registry_init
 *
 * Initialize PPP Registry
 */
 
static void ppp_registry_init (subsystype *subsys)
{
    registry_create(REG_PPP, SERVICE_PPP_MAX, "PPP");
    create_registry_ppp();
}
 
/*
 * PPP Registry subsystem header
 */
 
#define PPP_REGISTRY_MAJVERSION 1
#define PPP_REGISTRY_MINVERSION 0
#define PPP_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(ppp_registry,
              PPP_REGISTRY_MAJVERSION, PPP_REGISTRY_MINVERSION,
              PPP_REGISTRY_EDITVERSION,
              ppp_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
