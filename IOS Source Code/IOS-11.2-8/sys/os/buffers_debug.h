/* $Id: buffers_debug.h,v 3.1.70.1 1996/06/04 05:15:55 mshannon Exp $
 * $Source: /release/112/cvs/Xsys/os/buffers_debug.h,v $
 *------------------------------------------------------------------
 * buffers_debug.h : Contains function declarations for
 * buffers_debug.c
 *
 * October 1995, Francis Bruneault
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: buffers_debug.h,v $
 * Revision 3.1.70.1  1996/06/04  05:15:55  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Added initial debug sanity code for memory corruptions
 *
 * Revision 3.1  1995/11/09  12:43:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/11  23:29:05  francisb
 * Placeholder for Intello_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BUFFERS_DEBUG_H__
#define __BUFFERS_DEBUG_H__

#include "free.h"
/*
 * Function Declarations
 */
extern void buffer_sanity_init(void);
extern char *validblock_diagnose(mempool *, blocktype *, bad_code_t);
extern boolean pak_pool_item_validate(pooltype *, void *);


#endif /* __BUFFERS_DEBUG_H__ */

