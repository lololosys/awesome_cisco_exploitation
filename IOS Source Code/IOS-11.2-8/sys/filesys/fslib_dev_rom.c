/* $Id: fslib_dev_rom.c,v 3.2.60.1 1996/03/18 19:35:41 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/fslib_dev_rom.c,v $
 *------------------------------------------------------------------
 * Flash file system driver for ROM cards (or any read-only device)
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib_dev_rom.c,v $
 * Revision 3.2.60.1  1996/03/18  19:35:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:35:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:50:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:08:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#if !defined(STANDALONE) 
#include "master.h"
#endif

#include "types.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib_internals.h"
#include "fslib_print.h"
#include "fslib_dev_rsp_internal_flash.h"


/******************************************************************************
 * Name: ROM_read_fn
 *
 * Description: read function for Read-Only devices
 * 
 * Input:
 *   dev_num -- the number of the device from which to read
 *   src -- a pointer into device memory from which to start reading
 *   dst -- a pointer to local memory in which to put the data
 *   length -- the number of bytes to read from the device
 *
 * Return Value:
 *   actual -- the actual number of bytes copied from the device.  If an error
 *     occurred, an error code is returned.  All error codes are negative.
 *
 ******************************************************************************/
int ROM_read_fn (int dev_num, fslib_ptr src, char *dst, ulong length)
{
    int actual;

    actual = FSLIB_ralib->read(dev_num, src, dst, length);

    PRT(P10,("ROM_read_fn(%d,%lx,%lx,%d) = %d\n",dev_num,src,dst,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: ROM_write_fn
 *
 * Description: write function for Read-Only devices (null function)
 * 
 * Input:
 *   dev_num -- the number of the device to which to write
 *   src -- a pointer to local memory buufer that has data to write
 *   dst -- a pointer into device memory at which to start writing
 *   length -- the number of bytes to write to the device
 *
 * Return Value:
 *   actual -- the actual number of bytes written to the device.  If an error
 *     occurred, an error code is returned.  All error codes are negative.
 *
 ******************************************************************************/
int ROM_write_fn (int dev_num, char *src, fslib_ptr dst, ulong length)
{

    PRT(P11,("ROM_write_fn(%d,%lx,%lx,%d) = 0\n",dev_num,src,dst,length));

    return(FSLIB_ERROR_DEVICE_IS_READ_ONLY);
}


/******************************************************************************
 * Name: ROM_erase_fn
 *
 * Description: erase function for Read-Only devices (null function)
 * 
 * Input:
 *   dev_num -- the number of the device to erase (partial erase)
 *   sector -- a pointer into device memory at which to erase
 *   length -- the number of bytes to erase (must be sector size)
 *
 * Return Value:
 *   rc -- return code: FSLIB_OK if successful, otherwise an error code
 *
 ******************************************************************************/
int ROM_erase_fn (int dev_num, fslib_ptr sector, ulong length)
{

    PRT(P12,("ROM_erase_fn(%d,%lx,%d) = 0\n",dev_num,sector,length));

    return(FSLIB_ERROR_DEVICE_IS_READ_ONLY);
}


/******************************************************************************
 * Name: ROM_init_fn
 *
 * Description: init function for generic ROM cards -- nothing to do
 * 
 * Input:
 *   dev_num -- the number of the device to init
 *
 * Return Value:
 *   rc -- return code: FSLIB_OK if successful, otherwise an error code
 *
 ******************************************************************************/
int ROM_init_fn (int dev_num)
{
    return(FSLIB_OK);
}


/******************************************************************************
 * Name: ROM_show_info_fn
 *
 * Description: show info function for generic ROM cards -- no info
 * 
 * Input:
 *   dev_num -- the number of the device
 *
 * Return Value:
 *   rc -- return code: FSLIB_OK if successful, otherwise an error code
 *
 ******************************************************************************/
int ROM_show_info_fn (int dev_num)
{
    printf("This card is being treated as a generic ROM card\n");
    return(FSLIB_OK);
}

