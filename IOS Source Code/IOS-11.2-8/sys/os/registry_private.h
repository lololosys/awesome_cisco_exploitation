/* $Id: registry_private.h,v 3.1 1996/01/25 11:20:37 smackie Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/os/registry_private.h,v $
 *------------------------------------------------------------------
 * registry_private.h - Internal registry definitions and structures
 *
 * December 1995, Scott Mackie
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: registry_private.h,v $
 * Revision 3.1  1996/01/25  11:20:37  smackie
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

#ifndef __REGISTRY_PRIVATE_H__
#define __REGISTRY_PRIVATE_H__


/*
 * Defines
 */

/*
 * This is the maximum number of service that we allow in any one registry.
 * Over 500 services is an outrageous number. If we ever go over this, we
 * should hang our heads in shame.
 */

#define REG_SERVICE_MAXIMUM 512  


/*
 * Structures
 */

typedef struct registrystats_ {
    ulong    items;
    ulong    bytes;
} registrystats;


/*
 * Externs
 */
extern registrystats registry_statistics;
extern registrystats registry_service_statistics[];
extern registrystats registry_function_statistics;

#endif
