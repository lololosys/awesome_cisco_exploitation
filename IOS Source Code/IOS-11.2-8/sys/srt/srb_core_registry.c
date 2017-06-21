/* $Id: srb_core_registry.c,v 3.1.4.1 1996/08/12 02:16:18 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_core_registry.c,v $
 *------------------------------------------------------------------
 * srb_core_registry.c -- Service Points for SRB_CORE registry
 *
 * August 1996, Paul Pearce
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: srb_core_registry.c,v $
 * Revision 3.1.4.1  1996/08/12  02:16:18  ppearce
 * CSCdi64450:  CSCdi59527 broke c4500-boot-m image
 * Branch: California_branch
 *   Add srb_core_registry that will definitely be included w/SRB subblock
 *
 * Revision 3.1  1996/08/08  16:10:05  ppearce
 * Placeholder for SRB core registry (CSCdi64450)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "srb_core_registry.h"
#include "../srt/srb_core_registry.regc"

/*
 * srb_core_registry_init
 *
 * Initialize SRB_CORE registry
 *
 */
 
static void srb_core_registry_init (subsystype *subsys)
{
    registry_create(REG_SRB_CORE, SERVICE_SRB_CORE_MAX, "SRB_CORE");
    create_registry_srb_core();
}
 

/*
 * SRB_CORE registry subsystem header
 *
 */
 
#define SRB_CORE_REGISTRY_MAJVERSION  1
#define SRB_CORE_REGISTRY_MINVERSION  0
#define SRB_CORE_REGISTRY_EDITVERSION 1
 
SUBSYS_HEADER(srb_core_registry,
              SRB_CORE_REGISTRY_MAJVERSION, 
              SRB_CORE_REGISTRY_MINVERSION,
              SRB_CORE_REGISTRY_EDITVERSION,
              srb_core_registry_init, 
              SUBSYS_CLASS_REGISTRY, NULL, NULL);
