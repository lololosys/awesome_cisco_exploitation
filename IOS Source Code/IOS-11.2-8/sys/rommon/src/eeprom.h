/* $Id: eeprom.h,v 3.2 1995/11/17 18:41:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/eeprom.h,v $
 *------------------------------------------------------------------
 * eeprom.h
 *
 * May 1993, Rob Clevenger
 *
 * Copyright (c) 1994 - 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for the EEPROM that we use in place of NVRAM
 *------------------------------------------------------------------
 * $Log: eeprom.h,v $
 * Revision 3.2  1995/11/17  18:41:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __EEPROM_H__
#define __EEPROM_H__
/*
** Defines for the EEPROM that we now use in place of NVRAM
** The block write size was originally 64 bytes for Sierra but
** Sapphire uses a part that has a block write size of 32.
** Since this will work in all cases and is not much of a
** performance hit we will use it as the standard.
*/

#define EEPROM_BLOCKSIZ     0x20       /* write to NVRAM 32 bytes at a time */
#define EEPROM_MAXLOOPS     10000      /* loop wait for write block of 32 bytes into NVRAM */

#endif  /* __EEPROM_H__ */
