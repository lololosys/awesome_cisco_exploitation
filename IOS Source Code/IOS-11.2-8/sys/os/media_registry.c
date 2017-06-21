/* $Id: media_registry.c,v 3.1 1996/01/25 11:20:27 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/os/media_registry.c,v $
 *------------------------------------------------------------------
 * media_registry.c
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: media_registry.c,v $
 * Revision 3.1  1996/01/25  11:20:27  smackie
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
#include "media_registry.h"
#include "../h/media_registry.regc"

/*
 * media_registry_init
 *
 * Initialize Media Registry
 */
 
static void media_registry_init (subsystype *subsys)
{
    registry_create(REG_MEDIA, SERVICE_MEDIA_MAX, "Media");
    create_registry_media();
}
 
/*
 * Media Registry subsystem header
 */
 
#define MEDIA_REGISTRY_MAJVERSION 1
#define MEDIA_REGISTRY_MINVERSION 0
#define MEDIA_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(media_registry,
              MEDIA_REGISTRY_MAJVERSION, MEDIA_REGISTRY_MINVERSION,
              MEDIA_REGISTRY_EDITVERSION,
              media_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
