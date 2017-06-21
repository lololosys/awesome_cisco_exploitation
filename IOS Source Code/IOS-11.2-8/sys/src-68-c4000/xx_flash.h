/* $Id: xx_flash.h,v 3.2 1995/11/17 18:43:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-68-c4000/xx_flash.h,v $
 *------------------------------------------------------------------
 * xx_flash.h - Platform specific Flash routines.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xx_flash.h,v $
 * Revision 3.2  1995/11/17  18:43:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __XX_FLASH_H__
#define __XX_FLASH_H__

#define FLASH_BASE      ADRSPC_FLASH_BASE 
#define SUN             struct sun_exec

/* Data cache controls */
#define CLEAR_DCACHE	0x0800

#define DATA_BUS_REVERSED   1

#include "../dev/flash_codes.h"

#endif /* __PAN_FLASH_H__ */
