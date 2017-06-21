/* $Id: mlpvt_registry.c,v 1.1.2.1 1996/04/27 06:34:36 syiu Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt_registry.c,v $
 *------------------------------------------------------------------
 * mlpvt_registry.c
 *
 * April 1996, Shoou Yiu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt_registry.c,v $
 * Revision 1.1.2.1  1996/04/27  06:34:36  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.4.1  1996/04/17  03:32:31  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 1.1  1996/04/06  01:56:33  syiu
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "mlpvt_registry.h"
#include "../mlpvt/mlpvt_registry.regc"

/*
 * mlpvt_registry_init
 *
 * Initialize MLPVT Registry
 */
 
static void mlpvt_registry_init (subsystype *subsys)
{
    registry_create(REG_MLPVT, SERVICE_MLPVT_MAX, "MLPVT");
    create_registry_mlpvt();
}
 
/*
 * MLPVT Registry subsystem header
 */
 
#define MLPVT_REGISTRY_MAJVERSION 1
#define MLPVT_REGISTRY_MINVERSION 0
#define MLPVT_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(mlpvt_registry,
              MLPVT_REGISTRY_MAJVERSION, MLPVT_REGISTRY_MINVERSION,
              MLPVT_REGISTRY_EDITVERSION,
              mlpvt_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
