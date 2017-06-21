/* $Id: sse_registry.c,v 3.1 1996/01/25 11:18:02 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/hes/sse_registry.c,v $
 *------------------------------------------------------------------
 * sse_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sse_registry.c,v $
 * Revision 3.1  1996/01/25  11:18:02  smackie
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
#include "../parser/parser_defs_sse.h"
#include "sse_registry.h"
#include "../hes/sse_registry.regc"

/*
 * sse_registry_init
 *
 * Initialize SSE Registry
 */
 
static void sse_registry_init (subsystype *subsys)
{
    registry_create(REG_SSE, SERVICE_SSE_MAX, "SSE");
    create_registry_sse();
}
 
/*
 * SSE Registry subsystem header
 */
 
#define SSE_REGISTRY_MAJVERSION 1
#define SSE_REGISTRY_MINVERSION 0
#define SSE_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(sse_registry,
              SSE_REGISTRY_MAJVERSION, SSE_REGISTRY_MINVERSION,
              SSE_REGISTRY_EDITVERSION,
              sse_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
