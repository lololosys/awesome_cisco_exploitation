/* $Id: util_4k.h,v 3.1.64.1 1996/04/29 18:04:52 lcheung Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/util_4k.h,v $
 *------------------------------------------------------------------
 * util_4k.h - Prototypes for general R4600 utilities
 *
 * June 1995, Scott Mackie
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: util_4k.h,v $
 * Revision 3.1.64.1  1996/04/29  18:04:52  lcheung
 * CSCdi55723:  use CPUs implementation number to display CPU type
 * Branch: California_branch
 *
 * Revision 3.1  1995/11/09  13:21:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/15  06:39:36  smackie
 * Tidy up R4600/R4700 revision string handler. (CSCdi35883)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define R4600_IMP               0x20       /*IDT R4600 Implementation number*/
#define R4700_IMP               0x21       /*IDT R4700 Implementation number*/
extern void r4k_get_revision_string(char *buffer);
