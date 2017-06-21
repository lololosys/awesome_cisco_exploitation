/* $Id: util_4k.c,v 3.2.60.1 1996/04/29 18:04:50 lcheung Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/util_4k.c,v $
 *------------------------------------------------------------------
 * Common utility routines for all R4000 platforms
 *
 * October 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: util_4k.c,v $
 * Revision 3.2.60.1  1996/04/29  18:04:50  lcheung
 * CSCdi55723:  use CPUs implementation number to display CPU type
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:40:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:21:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/15  06:39:35  smackie
 * Tidy up R4600/R4700 revision string handler. (CSCdi35883)
 *
 * Revision 2.1  1995/06/07  22:53:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "util_4k.h"

/*
 * r4k_get_revision_string
 *
 * gets the R4600 PRId register
 */

void r4k_get_revision_string (char *buffer)
{
    ulong prid = getcp0_prid();
    uint  imp  = (prid >>8) & 0xff;
    char *p;

    switch (imp) {
      case R4600_IMP:      p="R4600";break;
      case R4700_IMP:      p="R4700";break;
      default:             p="Unknown";break;
    }

    sprintf(buffer,
            "%s processor, Implementation %d, Revision %d.%d %s",
            p, imp,
            (prid>>4) & 0x0f, (prid & 0x0f),
            level2_cache_present ? "(Level 2 Cache)" : "");
}
