/* $Id: fslib_dev_rom.h,v 3.2 1995/11/17 09:08:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/fslib_dev_rom.h,v $
 *------------------------------------------------------------------
 * Include file for flash file system ROM driver
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib_dev_rom.h,v $
 * Revision 3.2  1995/11/17  09:08:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__fslib_dev_rom_h__)
#define __fslib_dev_rom_h__


/*----------------------------------------------------------------------------*
 * External references to the driver read/write/erase functions
 *----------------------------------------------------------------------------*/
extern int ROM_read_fn (int dev_num, fslib_ptr src, char *dst, ulong length);
extern int ROM_write_fn (int dev_num, char *src, fslib_ptr dst, ulong length);
extern int ROM_erase_fn (int dev_num, fslib_ptr sector, ulong length);
extern int ROM_init_fn (int dev_num);
extern int ROM_show_info_fn (int dev_num);


#endif /* !defined(__fslib_dev_rom_h__) */
