/* $Id: flash_cmnds.h,v 3.2 1995/11/17 09:00:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dev/flash_cmnds.h,v $
 *------------------------------------------------------------------
 * flash_cmnds.h - High level user interface for flash commands.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: flash_cmnds.h,v $
 * Revision 3.2  1995/11/17  09:00:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:24:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FLASH_CMNDS_H__
#define __FLASH_CMNDS_H__

void show_flash (parseinfo *csb);
void flash_copy_command (parseinfo *csb);

#endif  /* __FLASH_CMNDS_H__  */
