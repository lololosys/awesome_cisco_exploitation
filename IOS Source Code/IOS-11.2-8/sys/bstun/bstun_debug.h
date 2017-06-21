/* $Id: bstun_debug.h,v 3.2.60.1 1996/09/10 08:48:40 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bstun_debug.h,v $
 *------------------------------------------------------------------
 * bstun_debug.h - BSTUN debug defintions/prototypes
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun_debug.h,v $
 * Revision 3.2.60.1  1996/09/10  08:48:40  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:49:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:14:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void bstun_debug_init(void);

extern uint bstun_debug_buffer_size;

/* returns TRUE if debugging for the group is on */
extern boolean is_bstun_debugging_group(uint group);

#include "bstun_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "bstun_debug_flags.h"

