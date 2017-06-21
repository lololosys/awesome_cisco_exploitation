/* $Id: srt_registry.c,v 3.1.40.1 1996/03/18 22:11:36 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/srt/srt_registry.c,v $
 *------------------------------------------------------------------
 * srt_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srt_registry.c,v $
 * Revision 3.1.40.1  1996/03/18  22:11:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.14.1  1996/02/20  18:50:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.1  1996/01/25  11:48:26  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "ieee.h"
#include "../if/rif.h"
#include "lack_externs.h"
#include "srt_registry.h"
#include "../srt/srt_registry.regc"

/*
 * srt_registry_init
 *
 * Initialize SRT Registry
 */
 
static void srt_registry_init (subsystype *subsys)
{
    registry_create(REG_SRT, SERVICE_SRT_MAX, "SRT");
    create_registry_srt();
}
 
/*
 * SRT Registry subsystem header
 */
 
#define SRT_REGISTRY_MAJVERSION 1
#define SRT_REGISTRY_MINVERSION 0
#define SRT_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(srt_registry,
              SRT_REGISTRY_MAJVERSION, SRT_REGISTRY_MINVERSION,
              SRT_REGISTRY_EDITVERSION,
              srt_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
