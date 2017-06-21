/* $Id: unix_fslib_dev.c,v 3.2 1995/11/17 09:09:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/unix_fslib_dev.c,v $
 *------------------------------------------------------------------
 * Flash file system device table for Unix stand-alone test system
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_fslib_dev.c,v $
 * Revision 3.2  1995/11/17  09:09:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:33:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:33:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib_internals.h"

#include "fslib_dev.h"
#include "fslib_dev_rom.h"
#include "fslib_dev_ram.h"


/*----------------------------------------------------------------------------*
 * Externs (very bad form!)
 *----------------------------------------------------------------------------*/
extern erase_hack();

/*----------------------------------------------------------------------------*
 * Programming algorithm table
 *----------------------------------------------------------------------------*/
fslib_prog_rec_t  FSLIB_prog_table[FSLIB_PROG_MAX + 1];


/******************************************************************************
 * Name: FSLIB_dev_init
 *
 * Description: platform specific initialization for the flash file
 *   system device drivers
 *
 * Input: None
 *
 * Return Value: None
 *
 *******************************************************************************/
void FSLIB_dev_init (void)
{

    FSLIB_prog_table[0].algorithm    = FSLIB_PROG_ROM_CARD;
    FSLIB_prog_table[0].read_fn      = ralib_read;
    FSLIB_prog_table[0].write_fn     = ralib_write;
    FSLIB_prog_table[0].erase_fn     = erase_hack;
    FSLIB_prog_table[0].init_fn      = ROM_init_fn;
    FSLIB_prog_table[0].show_info_fn = ROM_show_info_fn;

    FSLIB_prog_table[1].algorithm    = FSLIB_PROG_RAM_CARD_00;
    FSLIB_prog_table[1].read_fn      = ralib_read;
    FSLIB_prog_table[1].write_fn     = ralib_write;
    FSLIB_prog_table[1].erase_fn     = erase_hack;
    FSLIB_prog_table[1].init_fn      = RAM_init_fn;
    FSLIB_prog_table[1].show_info_fn = RAM_show_info_fn;

    FSLIB_prog_table[2].algorithm    = FSLIB_PROG_RAM_CARD_FF;
    FSLIB_prog_table[2].read_fn      = ralib_read;
    FSLIB_prog_table[2].write_fn     = ralib_write;
    FSLIB_prog_table[2].erase_fn     = erase_hack;
    FSLIB_prog_table[2].init_fn      = RAM_init_fn;
    FSLIB_prog_table[2].show_info_fn = RAM_show_info_fn;

    FSLIB_prog_table[3].algorithm    = FSLIB_PROG_INTEL_SERIES_2PLUS_CARD;
    FSLIB_prog_table[3].read_fn      = ralib_read;
    FSLIB_prog_table[3].write_fn     = ralib_write;
    FSLIB_prog_table[3].erase_fn     = erase_hack;
    FSLIB_prog_table[3].init_fn      = RAM_init_fn;
    FSLIB_prog_table[3].show_info_fn = RAM_show_info_fn;

    FSLIB_prog_table[4].algorithm    = FSLIB_PROG_RSP_INTERNAL_FLASH;
    FSLIB_prog_table[4].read_fn      = ralib_read;
    FSLIB_prog_table[4].write_fn     = ralib_write;
    FSLIB_prog_table[4].erase_fn     = erase_hack;
    FSLIB_prog_table[4].init_fn      = RAM_init_fn;
    FSLIB_prog_table[4].show_info_fn = RAM_show_info_fn;

    FSLIB_prog_table[5].algorithm = FSLIB_PROG_LIST_END;

}
