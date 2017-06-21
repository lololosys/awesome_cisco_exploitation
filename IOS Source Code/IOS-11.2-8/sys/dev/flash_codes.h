/* $Id: flash_codes.h,v 3.2 1995/11/17 09:00:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dev/flash_codes.h,v $
 *------------------------------------------------------------------
 *flash_codes.h - Flash chip command and status codes (all supported chips).
 *
 * Oct 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1995, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_codes.h,v $
 * Revision 3.2  1995/11/17  09:00:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:24:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FLASH_CODES_H__
#define __FLASH_CODES_H__

#ifdef DATA_BUS_REVERSED

/*------------------------- Bit reversed commands ---------------------*/
/*
 * Commands in bit-reversed form (for platforms that have the data
 * bus all screwed up) - the 2500's, 3000's, 4000's.
 */

/*
 * Generic commands.  work on most chip sets, and are needed
 * before the chip set type is known to read ids.
 */

/* Note : flash command values are bit-reversed  */
#define FLASH_READ_ID         0x09
#define FLASH_RESET_CMD       0xFF

/*
 * Flash EPROM command codes:
 * as found in the data sheets, bit reversed as per PAN systems.
 * INTEL_010 command set found in platform dependent files.
 */
#define INTEL008_READ_CMD        0xFF
#define INTEL008_ERASE_CMD	 0x04
#define INTEL008_ERASE_CNFRM	 0x0B
#define INTEL008_ERASE_VERIFY    0x05
#define INTEL008_PROGRAM	 0x02
#define INTEL008_PROGRAM_VERIFY  0x03
#define INTEL008_READ_STS        0x0E
#define INTEL008_CLEAR_STS       0x0A
#define INTEL008_ERASE_SUSP      0x0D
#define INTEL008_ERASE_RESUME    0x0B 
#define INTEL008_READ_ID	 FLASH_READ_ID
#define INTEL008_RESET_CMD	 FLASH_RESET_CMD


/* AMD/INTEL 010,020 commands */
/*
 * Flash EPROM command codes:
 * as found in the data sheets, bit reversed for PAN systems
 */
#define INTEL010_READ_CMD        0xFF
#define INTEL010_READ_ID         FLASH_READ_ID
#define INTEL010_ERASE_CMD       0x04
#define INTEL010_ERASE_VERIFY    0x05
#define INTEL010_PROGRAM         0x02
#define INTEL010_PROGRAM_VERIFY  0x03
#define INTEL010_RESET_CMD       FLASH_RESET_CMD

/*
 * Flash command codes for the AMD 27040 chips.
 * Codes are bit reversed (compared to data sheets) for PAN systems.
 */
#define AMD040_READ_CMD          0x0F
#define AMD040_READ_ID           FLASH_READ_ID
#define AMD040_PROGRAM           0x05
#define AMD040_CHIP_ERASE_CMD    0x01
#define AMD040_SECT_ERASE_CMD    0x01
#define AMD040_CHIP_ERASE_CNFRM  0x08
#define AMD040_SECT_ERASE_CNFRM  0x0C
#define AMD040_RESET_CMD         0x0F
#define AMD040_UNLOCK_5555       0x55
#define AMD040_UNLOCK_AAAA       0xAA

/*
 * Flash status values for INTEL 008 command set (bit reversed)
 */
#define  FLASH_READY		0x01010101UL
#define  FLASH_ERASE_ERR	0x04040404UL
#define  FLASH_WRITE_ERR 	0x08080808UL
#define  FLASH_VPP_LOW		0x10101010UL
#define  FLASH_TIMEOUT          0x04040404UL

#else  /* NOT DATA_BUS_REVERSED */

/*-------------------- Normal (data sheet) commands -----------------*/

/*
 * Commands in normal form (for platforms that have the data bus
 * wired straight) - the 4500's and 7000's.
 */

/*
 * Generic commands.  work on most chip sets, and are needed
 * before the chip set type is known to read ids.
 */

/* Note : flash command values as found in data sheets */
#define FLASH_READ_ID         0x90
#define FLASH_RESET_CMD       0xFF

/*
 * Flash EPROM command codes:  as found in the data sheets. 
 * Sierra HW is wired correctly, so commands are NOT bit reversed.
 */
#define INTEL008_READ_CMD        0xFF
#define INTEL008_ERASE_CMD	 0x20
#define INTEL008_ERASE_CNFRM	 0xD0
#define INTEL008_PROGRAM	 0x40
#define INTEL008_READ_STS        0x70
#define INTEL008_CLEAR_STS       0x50
#define INTEL008_ERASE_SUSP      0xB0
#define INTEL008_ERASE_RESUME    0xD0 
#define INTEL008_READ_ID	 FLASH_READ_ID
#define INTEL008_RESET_CMD	 FLASH_RESET_CMD


/* AMD/INTEL 010,020 commands */
/*
 * Flash EPROM command codes:  as found in the data sheets. 
 * Sierra HW is wired correctly, so commands are NOT bit reversed.
 */
#define INTEL010_READ_CMD        0xFF
#define INTEL010_READ_ID         FLASH_READ_ID
#define INTEL010_ERASE_CMD       0x20
#define INTEL010_ERASE_VERIFY    0xA0
#define INTEL010_PROGRAM         0x40
#define INTEL010_PROGRAM_VERIFY  0xC0
#define INTEL010_RESET_CMD       FLASH_RESET_CMD

/*
 * Flash command codes for the AMD 27040 chips.
 * Codes are in normal form (as in data sheets).
 */
#define AMD040_READ_CMD          0xF0
#define AMD040_READ_ID           FLASH_READ_ID
#define AMD040_PROGRAM           0xA0
#define AMD040_CHIP_ERASE_CMD    0x80
#define AMD040_SECT_ERASE_CMD    0x80
#define AMD040_CHIP_ERASE_CNFRM  0x10
#define AMD040_SECT_ERASE_CNFRM  0x30
#define AMD040_RESET_CMD         0xF0
#define AMD040_UNLOCK_5555       0xAA
#define AMD040_UNLOCK_AAAA       0x55

/*
 * Flash status values for INTEL 008 command set (NOT bit reversed)
 */
#define  FLASH_READY		0x80808080UL
#define  FLASH_ERASE_ERR	0x20202020UL
#define  FLASH_WRITE_ERR 	0x10101010UL
#define  FLASH_VPP_LOW		0x08080808UL
#define  FLASH_TIMEOUT          0x20202020UL

/*---------------------------------------------------------------------*/

#endif  /* DATA_BUS_REVERSED */

#endif /* __FLASH_CODES_H__ */
