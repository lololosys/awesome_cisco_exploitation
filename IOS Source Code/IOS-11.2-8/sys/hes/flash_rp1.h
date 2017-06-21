/* $Id: flash_rp1.h,v 3.3.46.1 1996/03/18 19:40:36 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/flash_rp1.h,v $
 *------------------------------------------------------------------
 * flash_rp1.h - clean specific defs/decls for flash driver
 *
 * February 1993, Dit Morse
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_rp1.h,v $
 * Revision 3.3.46.1  1996/03/18  19:40:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.1  1996/01/24  21:45:34  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/08  19:26:11  smackie
 * Move memory size defines into a central place. (CSCdi45214)
 *
 * Revision 3.2  1995/11/17  09:16:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:39:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:39:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *------------------------------------------------------------------
 */

/* Data cache controls */

  #define DCACHESTATUS   
  #define DCACHEOFF      
  #define DCACHEON       

/* flash parameters */

#define FLASH_WIDTH 	4
#define FLASH_BASE  	ADRSPC_FLASH_BASE

/*
 * Flash EPROM command codes:
 * as found in the data sheets, bits ordered correctly for high-end systems
 */
#define INTEL010_READ_CMD        0x00
#define INTEL010_READ_ID         0x90
#define INTEL010_ERASE_CMD       0x20
#define INTEL010_ERASE_VERIFY    0xA0
#define INTEL010_PROGRAM         0x40
#define INTEL010_PROGRAM_VERIFY  0xC0
#define INTEL010_RESET_CMD       0xFF

/*
 * Generic commands.  work on both chip sets, and are needed
 * before the chip set type is known to read ids.
 */

/* Note : flash command values are ordered logically */
#define FLASH_READ_ID         0x90
#define FLASH_RESET_CMD       0xFF
