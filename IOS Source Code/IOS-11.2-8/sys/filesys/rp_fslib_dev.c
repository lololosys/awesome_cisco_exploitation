/* $Id: rp_fslib_dev.c,v 3.2.60.1 1996/03/18 19:36:04 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/rp_fslib_dev.c,v $
 *------------------------------------------------------------------
 * Flash file system device table for the RP
 *
 * April 1995, Szewei Ju
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rp_fslib_dev.c,v $
 * Revision 3.2.60.1  1996/03/18  19:36:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:36:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:50:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:08:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:33:05  hampton
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
#include "fslib_dev_intel_series_2plus.h"


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
    FSLIB_prog_table[0].algorithm = FSLIB_PROG_INTEL_SERIES_2PLUS_CARD;
    FSLIB_prog_table[0].read_fn   = intel_series_2plus_read_fn;
    FSLIB_prog_table[0].write_fn  = intel_series_2plus_write_fn;
    FSLIB_prog_table[0].erase_fn  = intel_series_2plus_erase_fn;
    FSLIB_prog_table[0].init_fn      = intel_series_2plus_init_fn;
    FSLIB_prog_table[0].show_info_fn = intel_series_2plus_show_info_fn;

    FSLIB_prog_table[1].algorithm = FSLIB_PROG_LIST_END;
}

