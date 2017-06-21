/* $Id: dev_flash.h,v 3.1.58.1 1996/08/08 01:26:35 smackie Exp $
 * $Source: /release/112/cvs/Xboot/src-68-c7000/dev_flash.h,v $
 *------------------------------------------------------------------
 * Header file RP1 internal flash
 *
 * December 1994, Szewei Ju
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dev_flash.h,v $
 * Revision 3.1.58.1  1996/08/08  01:26:35  smackie
 * Change Rom Monitor references from dev_t to flash_dev_t. (CSCdi64993)
 * Branch: California_branch
 *
 * Revision 3.1  1995/11/09  09:10:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/06/07 09:05:07  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.1.4.1  1995/05/02  23:22:27  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.1  1995/04/23  00:22:32  sthormod
 * Placeholders for Arizona branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define FLASH_RESET           0xFFFFFFFF

#define FFH_FLAGS_DELETED     0x0001
#define FFH_FLAGS_SYS_CONFIG  0x0002
#define FFH_FLAGS_SYS_IMAGE   0x0004

#define FC_READ_LEN (32*1024)  

#define flash_dev_t int  
#define flash_t struct flash_t_

void flash_long_copy(ulong, uchar *, ulong , boolean);
ushort update_cksum(ushort, uchar *, ulong);
