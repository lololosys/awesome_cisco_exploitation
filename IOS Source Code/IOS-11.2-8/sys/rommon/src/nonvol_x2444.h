/* $Id: nonvol_x2444.h,v 3.2 1995/11/17 18:42:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/nonvol_x2444.h,v $
 *------------------------------------------------------------------
 * nonvol_x2444.h
 *
 * November 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Define the opcodes used by the X2444.  Borrowed from DiagMon. 
 *------------------------------------------------------------------
 * $Log: nonvol_x2444.h,v $
 * Revision 3.2  1995/11/17  18:42:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:05  hampton
 * Bump version numbers from 1.x to 2.x.
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
