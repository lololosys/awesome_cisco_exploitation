/* $Id: rsp_fslib_dev.c,v 3.2.60.1 1996/03/18 19:36:09 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/rsp_fslib_dev.c,v $
 *------------------------------------------------------------------
 * Flash file system device table for the RSP
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_fslib_dev.c,v $
 * Revision 3.2.60.1  1996/03/18  19:36:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:36:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:50:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:08:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:33:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib_internals.h"
#include "fslib_dev.h"
#include "fslib_dev_rom.h"
#include "fslib_dev_ram.h"
#include "fslib_dev_intel_series_2plus.h"
#include "fslib_dev_rsp_internal_flash.h"


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
    FSLIB_prog_table[0].read_fn      = ROM_read_fn;
    FSLIB_prog_table[0].write_fn     = ROM_write_fn;
    FSLIB_prog_table[0].erase_fn     = ROM_erase_fn;
    FSLIB_prog_table[0].init_fn      = ROM_init_fn;
    FSLIB_prog_table[0].show_info_fn = ROM_show_info_fn;

    FSLIB_prog_table[1].algorithm    = FSLIB_PROG_RAM_CARD_00;
    FSLIB_prog_table[1].read_fn      = RAM_read_fn;
    FSLIB_prog_table[1].write_fn     = RAM_write_fn;
    FSLIB_prog_table[1].erase_fn     = RAM_erase_00_fn;
    FSLIB_prog_table[1].init_fn      = RAM_init_fn;
    FSLIB_prog_table[1].show_info_fn = RAM_show_info_fn;

    FSLIB_prog_table[2].algorithm    = FSLIB_PROG_RAM_CARD_FF;
    FSLIB_prog_table[2].read_fn      = RAM_read_fn;
    FSLIB_prog_table[2].write_fn     = RAM_write_fn;
    FSLIB_prog_table[2].erase_fn     = RAM_erase_ff_fn;
    FSLIB_prog_table[2].init_fn      = RAM_init_fn;
    FSLIB_prog_table[2].show_info_fn = RAM_show_info_fn;

    FSLIB_prog_table[3].algorithm    = FSLIB_PROG_INTEL_SERIES_2PLUS_CARD;
    FSLIB_prog_table[3].read_fn      = intel_series_2plus_read_fn;
    FSLIB_prog_table[3].write_fn     = intel_series_2plus_write_fn;
    FSLIB_prog_table[3].erase_fn     = intel_series_2plus_erase_fn;
    FSLIB_prog_table[3].init_fn      = intel_series_2plus_init_fn;
    FSLIB_prog_table[3].show_info_fn = intel_series_2plus_show_info_fn;

    FSLIB_prog_table[4].algorithm    = FSLIB_PROG_RSP_INTERNAL_FLASH;
    FSLIB_prog_table[4].read_fn      = rsp_internal_flash_read_fn;
    FSLIB_prog_table[4].write_fn     = rsp_internal_flash_write_fn;
    FSLIB_prog_table[4].erase_fn     = rsp_internal_flash_erase_fn;
    FSLIB_prog_table[4].init_fn      = rsp_internal_flash_init_fn;
    FSLIB_prog_table[4].show_info_fn = rsp_internal_flash_show_info_fn;

    FSLIB_prog_table[5].algorithm = FSLIB_PROG_LIST_END;

}
