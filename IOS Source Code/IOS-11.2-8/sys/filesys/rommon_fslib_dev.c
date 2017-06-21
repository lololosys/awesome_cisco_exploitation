/* $Id: rommon_fslib_dev.c,v 3.2 1995/11/17 09:08:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/rommon_fslib_dev.c,v $
 *------------------------------------------------------------------
 * Device table for the flash file system used by the ROM MONITOR
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rommon_fslib_dev.c,v $
 * Revision 3.2  1995/11/17  09:08:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "types.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib_internals.h"
#include "fslib_dev.h"
#include "fslib_dev_rom.h"


/*----------------------------------------------------------------------------*
 * Programming algorithm table
 *----------------------------------------------------------------------------*/
fslib_prog_rec_t  FSLIB_prog_table[FSLIB_PROG_MAX + 1];


/******************************************************************************
 * Name: FSLIB_dev_init
 *
 * Description: platform specific initialization for the flash file
 *   system device drivers for the ROM MONITOR.  This is used when
 *   building the FSLIB that the ROM MONITOR uses during booting.
 *
 * Input: None
 *
 * Return Value: None
 *
 *******************************************************************************/
void FSLIB_dev_init (void)
{
    int i;

    /*
     * Set all the device types to the ROM programs since the rom
     * monitor only reads the file system.
     */
    for (i = 0; (i+1) <= FSLIB_PROG_MAX; i++) {
	FSLIB_prog_table[i].algorithm = i+1;
	FSLIB_prog_table[i].read_fn   = ROM_read_fn;
	FSLIB_prog_table[i].write_fn  = ROM_write_fn;
	FSLIB_prog_table[i].erase_fn  = ROM_erase_fn;
    }

    FSLIB_prog_table[FSLIB_PROG_MAX].algorithm = FSLIB_PROG_LIST_END;
}















