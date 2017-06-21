/* $Id: memory.c,v 3.2.62.2 1996/06/28 23:30:48 hampton Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/memory.c,v $
 *------------------------------------------------------------------
 * M E M O R Y . C
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: memory.c,v $
 * Revision 3.2.62.2  1996/06/28  23:30:48  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  22:19:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:54:03  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:22:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:51:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "tn3270.h"			/* Defines CISCO */
#include "proto.h"

#ifdef CISCO				/* CISCO platform only */

#include "master.h"

#else

#include <stdio.h>

#endif					/* endif CISCO */

/*
 * Must match order of definitions in memtype.h
 */
static const unsigned int size_array[] = {
    0,					/* NEVER use element 0. */
    sizeof(struct field_type),		/* Size of a field element */
    0,					/* Not used in cisco version */
};


void *memory_allocate (ScreenPtr screen, int type)
{
    void *block;

    block = (malloc(size_array[type]));

    if (block)
	return block;
    else {
	tn3270_debug(screen, 0, "Error allocating memory");
	return (NULL);
    }
}

void memory_deallocate (ScreenPtr screen, void *mem)
{

    if (!screen)
	fatal(666);

    free(mem);
}
