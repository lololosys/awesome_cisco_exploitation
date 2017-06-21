/* $Id: sierra_flash.h,v 3.2 1995/11/17 18:41:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-c4000/sierra_flash.h,v $
 *------------------------------------------------------------------
 * sierra_flash.h  Platform specific Flash routines.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sierra_flash.h,v $
 * Revision 3.2  1995/11/17  18:41:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:22:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SIERRA_FLASH_H__
#define __SIERRA_FLASH_H__

#define FLASH_BASE      ADRSPC_FLASH_BASE 
#define FLASH_BASE_BOOT ADRSPC_BFLASH_BASE 

/* Data cache controls */
#define CLEAR_DCACHE	0x0800

/*
 * DON'T set DATA_BUS_REVERSED here, since sierra has the
 * data bus wired straight.
 */

#include "../dev/flash_codes.h"

#endif /* __SIERRA_FLASH_H__ */
