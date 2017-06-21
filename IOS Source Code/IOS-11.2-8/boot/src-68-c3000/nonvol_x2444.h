/* $Id: nonvol_x2444.h,v 3.1 1995/11/09 09:09:18 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c3000/nonvol_x2444.h,v $
 *------------------------------------------------------------------
 * nonvol_x2444.h - Definitions for Xicor X2444 non-volatile static RAM
 *  
 * December 23, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file was copied from sys/os.
 *
 *------------------------------------------------------------------
 * $Log: nonvol_x2444.h,v $
 * Revision 3.1  1995/11/09  09:09:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:15:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:33:33  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1993/06/25  05:09:03  smackie
 * Bring modular system to Aries 9.21(0.10) patch level.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Define the opcodes used in the X2444.
 */

enum X2444_CMD {
    X2444_CMD_WRDS = 0x01,	/* Reset write enable latch */
    X2444_CMD_STO = 0x81,	/* Store RAM data into EEPROM */
    X2444_CMD_SLEEP = 0x41,	/* Enter SLEEP mode */
    X2444_CMD_WRITE = 0xc1,	/* Write data into RAM */
    X2444_CMD_WREN = 0x21,	/* Set write enable latch */
    X2444_CMD_RCL = 0xbf,	/* Recall EEPROM into RAM */
    X2444_CMD_READ = 0xe1	/* Read data from specified address */
};

/*
 * Function prototypes
 */

extern ulong x2444_io(int, int, int); /* Do I/O to the X2444 */
